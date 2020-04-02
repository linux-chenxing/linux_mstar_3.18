

#include "mmc_host_sdio.h"

#ifdef ENABLE_SDIO_DEBUGFS_PROFILE
struct time_stamp_request
{
    U32 t_begin;
    U32 t_end;
};
struct time_stamp_transfer
{
    U32 t_start_dma;
    U32 t_irq_happen;
    U32 t_bh_run;
};
struct sector_info
{
    U32 addr;
    U32 length;
};
struct profile_request
{
    struct sector_info          sector;

    struct time_stamp_request   pre_request;
    struct time_stamp_request   request;
    struct time_stamp_request   post_request;

    struct time_stamp_transfer  transfer;
};
#define REC_REQUEST_NUM 500
struct profile_hcd
{
	struct profile_request	req[REC_REQUEST_NUM];
	U32						idx_pre_req;
	U32						idx_request;
	U32						idx_post_req;
};
struct profile_hcd profile_fcie;
#endif

struct dentry *debug_root;
struct dentry *debug_perf;
static void mmc_sdio_debugfs_attach(struct mmc_host *host);
static void mmc_sdio_debugfs_remove(void);

#define DBG_REQ(MSG)             //MSG


void mmc_sdio_send_command(struct mmc_command *cmd)
{
	E_IO_STS IoStatus;
    CMD_RSP_INFO CmdRspInfo;
	static U32 u32LastClock = 0;
	static unsigned char u8LastTiming = 0;

	DBG_REQ(printk("\tCMD%d_%08X", cmd->opcode, cmd->arg));
	memset(&CmdRspInfo, 0, sizeof(CMD_RSP_INFO));
	CmdRspInfo.Command.Cmd.Index = cmd->opcode;
	CmdRspInfo.Command.Cmd.Arg = U32BEND2LEND(cmd->arg);
	#if defined KEEP_SDIO_BUS_CLOCK && KEEP_SDIO_BUS_CLOCK
		CmdRspInfo.ClockStop = CLK_KEEP;
	#else
		CmdRspInfo.ClockStop = CLK_STOP;
	#endif

	CmdRspInfo.CardClock = cmd->mrq->host->ios.clock/1000;
	if(u32LastClock!=CmdRspInfo.CardClock)
	{
		u32LastClock = CmdRspInfo.CardClock;
		//printk(YELLOW"clock -> %d"NONE"\n", u32LastClock);
	}

	//CmdRspInfo.Clock = cmd->mrq->host->ios.timing;
	if(u8LastTiming!=cmd->mrq->host->ios.timing)
	{
		u8LastTiming = cmd->mrq->host->ios.timing;
		//printk(YELLOW"timing -> %d"NONE"\n", u8LastTiming);
	}

	switch (mmc_resp_type(cmd))
	{
		case MMC_RSP_R1: //MMC_RSP_R5, MMC_RSP_R6, MMC_RSP_R7
			CmdRspInfo.RspType = RSP_TYPE_R1;
			break;
		case MMC_RSP_NONE:
			CmdRspInfo.RspType = RSP_TYPE_NO;
			break;

		case MMC_RSP_R1B:
			CmdRspInfo.RspType = RSP_TYPE_R1b;
			break;
		case MMC_RSP_R2:
			CmdRspInfo.RspType = RSP_TYPE_R2;
			break;
		case MMC_RSP_R3:
		//case MMC_RSP_R4:
			CmdRspInfo.RspType = RSP_TYPE_R3;
			break;
		default:
			printk("SDIO Err: not support response type");
			break;
	}

	if(!cmd->data)
	{
		CmdRspInfo.CmdType = CMD_TYPE_NDTC; // simple command
	}
	else
	{
		CmdRspInfo.CmdType = CMD_TYPE_ADTC; // data transder command

		switch(cmd->data->flags)
		{
			case MMC_DATA_WRITE:
				//printf("MMC_DATA_WRITE, ");
				CmdRspInfo.ReadWriteDir = DIR_W;
				CmdRspInfo.ReadWriteTimeOut = 77; // not NCRC, detect MIU side hang up
				break;
			case MMC_DATA_READ:
				//printf("MMC_DATA_READ, ");
				CmdRspInfo.ReadWriteDir = DIR_R;	// SD 2.0 SPEC P.86, use 100 ms for general case NAC
				CmdRspInfo.ReadWriteTimeOut = 77;	// 20ns x 65536 x 77 = 100.9254 ms
				break;
			default:
				printk("SD Err: data->flags = %08Xh, ", cmd->data->flags);
				break;
		}

		switch(cmd->data->mrq->host->ios.bus_width)
		{
			case MMC_BUS_WIDTH_1:
				CmdRspInfo.BusWidth = BUS_1_BIT; //printk("1 bits\n");
				break;
			case MMC_BUS_WIDTH_4:
				CmdRspInfo.BusWidth = BUS_4_BITS; //printk("4 bits\n");
				break;
			case MMC_BUS_WIDTH_8:
				CmdRspInfo.BusWidth = BUS_8_BITS; //printk("8 bits\n");
				break;
			default:
				printk("SD Err: wrong bus width!\n");
				break;
		}

		CmdRspInfo.BlockSize = cmd->data->blksz;

		#if defined(ENABLE_SDIO_ADMA)&&ENABLE_SDIO_ADMA
			CmdRspInfo.DataPath = PATH_ADMA;
			HalSdio_PrepareDescriptors(cmd->data);
			HalSdio_SetupDescriptorAddr(&CmdRspInfo);
		#else
			CmdRspInfo.DataPath = PATH_DMA;
			CmdRspInfo.BusAddr = sg_dma_address(cmd->data->sg);
			CmdRspInfo.BlockCount = cmd->data->blocks;
		#endif

		cmd->data->bytes_xfered = cmd->data->blksz * cmd->data->blocks;

	}


	IoStatus = HalSdio_SendCommand(&CmdRspInfo);


	#if 0 // force card run at SDR50
	if( cmd->opcode==6 && cmd->arg==0x00FFFFF0 && cmd->data )
	{
		unsigned char * pucBuf;
		//ms_sdmmc_dump_data(cmd->data);
		pucBuf = (unsigned char *)phys_to_virt(sg_dma_address(&(cmd->data->sg[0])));
		if(pucBuf[13] & SD_MODE_UHS_SDR50)
		{
			pucBuf[13] = SD_MODE_UHS_SDR12|SD_MODE_UHS_SDR25|SD_MODE_UHS_SDR50;
			printk(LIGHT_RED"force card run at SDR50\n"NONE);
		}
		//ms_sdmmc_dump_data(cmd->data);
	}
	#endif

	if(IoStatus==IO_SUCCESS)
	{
		// R1: cmd idx +  4 bytes + crc =  6
		if( cmd->flags & MMC_RSP_PRESENT )
		{
			cmd->resp[0] = U32BEND2LEND(CmdRspInfo.Response.Resp[1]);
			DBG_REQ(printk(", RSP: %08X", cmd->resp[0]));
		}

		// R2: cmd idx + 15 bytes + crc = 17
		if( cmd->flags & MMC_RSP_136 )
		{
			cmd->resp[1] = U32BEND2LEND(CmdRspInfo.Response.Resp[2]);
			cmd->resp[2] = U32BEND2LEND(CmdRspInfo.Response.Resp[3]);
			cmd->resp[3] = U32BEND2LEND(CmdRspInfo.Response.Resp[4]);
			DBG_REQ(printk(" %08X %08X %08X", cmd->resp[1], cmd->resp[2], cmd->resp[3]));
		}
		DBG_REQ(printk("\n"));
	}
	else
	{
		printk(LIGHT_RED"SDIO C%d_%08X Err %02Xh\n"NONE, cmd->opcode, cmd->arg, IoStatus);
		switch(IoStatus)
		{
			case IO_TIME_OUT:
			case IO_RSP_CRC_ERROR:
			case IO_R_DATA_CRC_ERROR:
			case IO_W_DATA_STS_ERROR:
			case IO_W_DATA_STS_NEGATIVE:
				cmd->error = -EILSEQ;
				break;
			case IO_CMD_NO_RSP:
				cmd->error = -ETIMEDOUT;
				break;
			default:
				printk("IoStatus = %02Xh\n", IoStatus);
				break;
		}

		if(cmd->data) // adtc
		{
			cmd->data->bytes_xfered = 0;
			cmd->data->error = cmd->error;
		}

	}

}


void mmc_sdio_request(struct mmc_host *host, struct mmc_request *req)
{
	DBG_REQ(printk(LIGHT_CYAN"%s()"NONE"\n", __FUNCTION__));

	if(!req->cmd->mrq->host)
	{
		req->cmd->mrq->host = host;
	}

	if(req->data) // command with data transfer
	{
		mmc_sdio_pre_req(req->data);
	}

	//HalSdio_SwitchPad(HalSdio_GetPadType()); // restore pad type
	// reconfig clock for share IP

	#ifdef ENABLE_SDIO_DEBUGFS_PROFILE
	if(req->cmd->opcode==18)
	{
		if(profile_fcie.idx_request < REC_REQUEST_NUM)
		{
			profile_fcie.req[profile_fcie.idx_request].request.t_begin = HalSdio_TimerStop();
			profile_fcie.req[profile_fcie.idx_request].sector.addr = req->cmd->arg;
			profile_fcie.req[profile_fcie.idx_request].sector.length = req->data->blocks;
		}
	}
	#endif

	if(req->sbc)
	{
		mmc_sdio_send_command(req->sbc);
	}

	mmc_sdio_send_command(req->cmd);

	if(req->stop)
	{
		mmc_sdio_send_command(req->stop);
	}

	#ifdef ENABLE_SDIO_DEBUGFS_PROFILE
	if(req->cmd->opcode==18)
	{
		if(profile_fcie.idx_request < REC_REQUEST_NUM)
		{
			profile_fcie.req[profile_fcie.idx_request++].request.t_end = HalSdio_TimerStop();
		}
	}
	#endif

	if(req->data) // command with data transfer
	{
		mmc_sdio_post_req(req->data);
	}

	mmc_request_done(host, req);
}


void mmc_sdio_pre_req(struct mmc_data *data)
{
	//printk("data sg_len = %d\n", data->sg_len);

	#ifdef ENABLE_SDIO_DEBUGFS_PROFILE
	if(data->mrq->cmd->opcode==18)
	{
		if(profile_fcie.idx_pre_req == 0)
		{
			HalSdio_TimerStart();
		}
		if(profile_fcie.idx_pre_req < REC_REQUEST_NUM)
		{
			profile_fcie.req[profile_fcie.idx_pre_req].pre_request.t_begin = HalSdio_TimerStop();
		}
	}
	#endif

	if(data->flags & MMC_DATA_READ)
	{
		dma_map_sg(data->mrq->host->parent, data->sg, data->sg_len, DMA_FROM_DEVICE);
	}
	else
	{
		dma_map_sg(data->mrq->host->parent, data->sg, data->sg_len, DMA_TO_DEVICE);
	}

	#ifdef ENABLE_SDIO_DEBUGFS_PROFILE
	if(data->mrq->cmd->opcode==18)
	{
		if(profile_fcie.idx_pre_req < REC_REQUEST_NUM)
		{
			profile_fcie.req[profile_fcie.idx_pre_req++].pre_request.t_end = HalSdio_TimerStop();
		}
	}
	#endif
}


void mmc_sdio_post_req(struct mmc_data *data)
{
	#ifdef ENABLE_SDIO_DEBUGFS_PROFILE
	if(data->mrq->cmd->opcode==18)
	{
		if(profile_fcie.idx_post_req < REC_REQUEST_NUM)
		{
			profile_fcie.req[profile_fcie.idx_post_req].post_request.t_begin = HalSdio_TimerStop();
		}
	}
	#endif

	if(data->flags & MMC_DATA_READ)
	{
		dma_unmap_sg(data->mrq->host->parent, data->sg, data->sg_len, DMA_FROM_DEVICE);
	}
	else
	{
		dma_unmap_sg(data->mrq->host->parent, data->sg, data->sg_len, DMA_TO_DEVICE);
	}

	#ifdef ENABLE_SDIO_DEBUGFS_PROFILE
	if(data->mrq->cmd->opcode==18)
	{
		if(profile_fcie.idx_post_req < REC_REQUEST_NUM)
		{
			profile_fcie.req[profile_fcie.idx_post_req++].post_request.t_end = HalSdio_TimerStop();
		}
	}
	#endif
}


void mmc_sdio_set_ios(struct mmc_host *host, struct mmc_ios *ios)
{
	static unsigned int clock = 0;
	static unsigned short vdd = 0;
	static unsigned char power_mode = 0;
	static unsigned char timing = MMC_TIMING_LEGACY;

	//printk(LIGHT_CYAN"%s()"NONE"\n", __FUNCTION__);

	if(clock!=ios->clock)
	{
		clock = ios->clock;
		//printk(YELLOW"clock = %d"NONE"\n", clock/1000);
		if(clock)
		{
			#ifdef SDIO_SUPPORT_SD30
			#if defined PATCH_BCM_USING_SLOW_CLK_IN_SDR104 && PATCH_BCM_USING_SLOW_CLK_IN_SDR104
				if(clock==300000)
				{
					HalSdio_SwitchPad(SDIO_PAD_SDR12);
				}
				else if(clock==200000000)
				{
					HalSdio_SwitchPad(SDIO_PAD_SDR104);
				}
			#endif
			#endif
			HalSdio_SetClock(clock/1000);
			HalSdio_OpenClock(1);
		}
		else
		{
			HalSdio_OpenClock(0);
		}
	}

	if(vdd!=ios->vdd)
	{
		vdd = ios->vdd;
		//printk(YELLOW"vdd = %Xh"NONE"\n", vdd);
	}

	if(power_mode!=ios->power_mode)
	{
		power_mode = ios->power_mode;
		//printk(YELLOW"power_mode = %d"NONE"\n", power_mode);
		if(power_mode)
		{
			HalSdio_SetCardPower(1);
		}
		else
		{
			HalSdio_SetCardPower(0);
		}
	}

	if(timing!=ios->timing)
	{
		timing = ios->timing;
		//printk(YELLOW"timing = %d "NONE"", timing);
		switch(timing)
		{
			case MMC_TIMING_LEGACY:
				//printk(YELLOW"MMC_TIMING_LEGACY\n"NONE"");
				HalSdio_SwitchPad(SDIO_PAD_SDR12);
				break;
			case MMC_TIMING_SD_HS:
				//printk(YELLOW"MMC_TIMING_SD_HS\n"NONE"");
				HalSdio_SwitchPad(SDIO_PAD_SDR25);
				break;
			#ifdef SDIO_SUPPORT_SD30
			case MMC_TIMING_UHS_SDR50:
				//printk(YELLOW"MMC_TIMING_UHS_SDR50\n"NONE"");
				HalSdio_SwitchPad(SDIO_PAD_SDR104);
				break;
			case MMC_TIMING_UHS_DDR50:
				//printk(YELLOW"MMC_TIMING_UHS_DDR50\n"NONE"");
				HalSdio_SwitchPad(SDIO_PAD_DDR50);
				break;
			case MMC_TIMING_UHS_SDR104:
				//printk(YELLOW"MMC_TIMING_UHS_SDR104\n"NONE"");
				HalSdio_SwitchPad(SDIO_PAD_SDR104);
				break;
			#endif
			default:
				printk(YELLOW"SdErr: timing = %d not set"NONE"\n", timing);
				break;
		}
	}
}


int mmc_sdio_get_cd(struct mmc_host *host)
{
	#ifdef CONFIG_MSTAR_SDIO_CARD_DETECT
		int cd;
		cd = HalSdio_GetCardDetect();
		//printk(LIGHT_CYAN"%s() %d"NONE"\n", __FUNCTION__, cd);
		return cd;
	#else
		return 1;
	#endif
}


int mmc_sdio_get_ro(struct mmc_host *host)
{
	#ifdef CONFIG_MSTAR_SDIO_WRITE_PROTECT
		//printk(LIGHT_CYAN"%s()"NONE"\n", __FUNCTION__);
		return HalSdio_GetWriteProtect();
	#else
		return 0;
	#endif
}

#ifdef SDIO_SUPPORT_SD30

int mmc_sdio_volt_switch(struct mmc_host *host, struct mmc_ios *ios)
{
	//printk(YELLOW"mmc_sdio_volt_switch(%d)\n"NONE, ios->signal_voltage);

	if(ios->signal_voltage==MMC_SIGNAL_VOLTAGE_330)
	{
		#if defined(SDIO_CTRL_PCB_IO_VOLT_TYPE) && (SDIO_CTRL_PCB_IO_VOLT_TYPE==SW_CTRL_PCB_IO_VOLT)
			HalSdio_SetIOVoltage(VOLT_33V);
		#elif defined(SDIO_CTRL_PCB_IO_VOLT_TYPE) && (SDIO_CTRL_PCB_IO_VOLT_TYPE==SW_CTRL_PCB_IO_VOLT_FORCE_18V)
			HalSdio_SetIOVoltage(VOLT_18V);
		#endif
	}
	else if(ios->signal_voltage==MMC_SIGNAL_VOLTAGE_180)
	{
		#if defined(SDIO_CTRL_PCB_IO_VOLT_TYPE) && \
		((SDIO_CTRL_PCB_IO_VOLT_TYPE==SW_CTRL_PCB_IO_VOLT) || (SDIO_CTRL_PCB_IO_VOLT_TYPE==SW_CTRL_PCB_IO_VOLT_FORCE_18V))
			HalSdio_SetIOVoltage(VOLT_18V);
		#endif
	}
	else
	{
		printk(LIGHT_RED"not support signal_voltage\n"NONE);
		return 1;
	}
	return 0;
}

int mmc_sdio_tuning(struct mmc_host *host, u32 opcode)
{
	//printk(YELLOW"mmc_sdio_tuning(%d), opcode = %d\n"NONE, host->ios.clock, opcode);
	HalSdio_Tuning(host->ios.clock/1000);
	return 0;
}

int mmc_sdio_card_busy(struct mmc_host *host)
{
	//printk(YELLOW"card busy check\n"NONE);
	return HalSdio_GetBusyStatus();
}

#endif

#if defined ENABLE_SDIO_D1_INTERRUPT && ENABLE_SDIO_D1_INTERRUPT
void mmc_sdio_enable_sdio_irq(struct mmc_host *host, int enable)
{
	//printk(YELLOW"mmc_sdio_enable_sdio_irq(%d)\n"NONE, enable);
	HalSdio_EnableD1Interrupt(enable);
}
#endif

static const struct mmc_host_ops mmc_sdio_ops =
{
	.request	= mmc_sdio_request,
	.set_ios	= mmc_sdio_set_ios,
	.get_cd		= mmc_sdio_get_cd,
	.get_ro		= mmc_sdio_get_ro,

	#ifdef SDIO_SUPPORT_SD30
	.start_signal_voltage_switch	= mmc_sdio_volt_switch,
	.execute_tuning					= mmc_sdio_tuning,
	.card_busy						= mmc_sdio_card_busy,
	#endif
	//.pre_req	= sdio_pre_req,
	//.post_req	= sdio_post_req,
	#if defined ENABLE_SDIO_D1_INTERRUPT && ENABLE_SDIO_D1_INTERRUPT
	.enable_sdio_irq = mmc_sdio_enable_sdio_irq,
	#endif
};


static int mmc_sdio_probe(struct platform_device *pdev)
{
	struct mmc_host *host = NULL;
	int IrqRequestResult = 0;

	printk("mstar sdio host running...\n");

	HalSdio_Init();

	host = mmc_alloc_host(0, &pdev->dev);
	if(!host)
	{
		printk("mmc_alloc_host fail!\n");
	}

	host->ops = &mmc_sdio_ops;
	host->f_min =   300000;
	host->f_max = 50000000;
	host->ocr_avail = MMC_VDD_165_195|MMC_VDD_27_28|MMC_VDD_28_29|MMC_VDD_29_30|MMC_VDD_30_31|MMC_VDD_31_32|MMC_VDD_32_33;
	host->caps = MMC_CAP_4_BIT_DATA|MMC_CAP_MMC_HIGHSPEED|MMC_CAP_SD_HIGHSPEED;
	//host->caps |= MMC_CAP_NEEDS_POLL;
#ifdef SDIO_SUPPORT_SD30
	host->f_max = 200000000;
	host->caps |= MMC_CAP_UHS_SDR50|MMC_CAP_UHS_DDR50|MMC_CAP_UHS_SDR104;
	#if defined ENABLE_SDIO_D1_INTERRUPT && ENABLE_SDIO_D1_INTERRUPT
		host->caps |= MMC_CAP_SDIO_IRQ;
	#endif
#endif
	host->max_seg_size = 512 * 1024;
	host->max_segs = 1;
	#if defined(ENABLE_SDIO_ADMA)&&ENABLE_SDIO_ADMA
	host->max_segs = 512;
	#endif
	host->max_req_size = 512 * 1024;
	host->max_blk_size = 512;
	host->max_blk_count = 1024;

	mmc_add_host(host);
	platform_set_drvdata(pdev, host);

	#if defined(ENABLE_SDIO_INTERRUPT_MODE)&&ENABLE_SDIO_INTERRUPT_MODE
		IrqRequestResult = request_irq(SDIO_INT_VECTOR, HalSdio_KernelIrq, IRQF_SHARED, "mstar sdio", host);
		if(IrqRequestResult)
		{
			printk("request fcie irq fail %d\n", IrqRequestResult);
		}
	#endif

	#ifdef CONFIG_MSTAR_SDIO_CARD_DETECT
   		kthread_run(mmc_sdio_hotplug, host, "mstar sdio hotplug");
	#endif

	#ifdef CONFIG_DEBUG_FS
		mmc_sdio_debugfs_attach(host);
	#endif

	return 0;
}


static int mmc_sdio_remove(struct platform_device *pdev)
{
	#ifdef CONFIG_DEBUG_FS
		mmc_sdio_debugfs_remove();
	#endif

	printk(LIGHT_CYAN"%s()\n"NONE, __FUNCTION__);
	return 0;
}


static int mmc_sdio_suspend(struct platform_device *pdev, pm_message_t state)
{
	int ret = 0;
	struct mmc_host *host= platform_get_drvdata(pdev);

	printk(LIGHT_CYAN"%s()\n"NONE, __FUNCTION__);
	ret = mmc_suspend_host(host);
	if(ret){
		printk("sdio mmc_suspend_host fail %d\n", ret);
		return ret;
	}

	#ifdef SDIO_SUPPORT_SD30
		#if defined(SDIO_CTRL_PCB_IO_VOLT_TYPE) && \
		((SDIO_CTRL_PCB_IO_VOLT_TYPE==SW_CTRL_PCB_IO_VOLT) || (SDIO_CTRL_PCB_IO_VOLT_TYPE==SW_CTRL_PCB_IO_VOLT_FORCE_18V))
			HalSdio_SetIOVoltage(VOLT_00V); // need to clear for resume
		#endif
	#endif

	return 0;
}


static int mmc_sdio_resume(struct platform_device *pdev)
{
	int ret = 0;
	struct mmc_host *host= platform_get_drvdata(pdev);

	printk(LIGHT_CYAN"%s()\n"NONE, __FUNCTION__);
	HalSdio_Init();
	ret = mmc_resume_host(host);
	if(ret){
		printk("sdio mmc_suspend_host fail %d\n", ret);
		return ret;
	}

	return 0;
}


static u64 mmc_dmamask = 0xffffffffUL;


static struct platform_device mmc_sdio_device =
{
    .name = "mstar_sdio",
    .id = 0,
    .dev =
    {
        .dma_mask = &mmc_dmamask,
        .coherent_dma_mask = 0xffffffffUL,
    },
};


static struct platform_driver mmc_sdio_driver =
{
    .probe   = mmc_sdio_probe,
    .remove  = mmc_sdio_remove,
    .suspend = mmc_sdio_suspend,
    .resume  = mmc_sdio_resume,
    .driver  =
    {
        .name  = "mstar_sdio",
        .owner = THIS_MODULE,
    },
};


static s32 mmc_sdio_init(void)
{
	platform_device_register(&mmc_sdio_device);
	platform_driver_register(&mmc_sdio_driver);
	return 0;
}


static void mmc_sdio_exit(void)
{
    platform_driver_unregister(&mmc_sdio_driver);
}

#ifdef CONFIG_MSTAR_SDIO_CARD_DETECT

static int _CardDetect_PlugDebounce(unsigned int u32WaitMs, unsigned char bPrePlugStatus)
{
	unsigned char bCurrPlugStatus = bPrePlugStatus;

	#if 0
		U32_T u32DiffTime = 0;
	#else
		struct timespec time_spec_curre;
		struct timespec time_spec_start;
		U32 u32WaitNs = u32WaitMs*1000000;
	#endif


	#if 0

		while(u32DiffTime < u32WaitMs)
		{
			mdelay(1);
			u32DiffTime++;

			bCurrPlugStatus = _GetCardDetect(eSlot);

	        if (bPrePlugStatus != bCurrPlugStatus)
	            break;
		}

	#else

		getnstimeofday(&time_spec_start);

		while(1)
		{
			bCurrPlugStatus = HalSdio_GetCardDetect();

	        if (bPrePlugStatus != bCurrPlugStatus)
	            break;

			getnstimeofday(&time_spec_curre);

			//printk("tv_sec = %d, tv_nsec = %9d\n", time_spec_curre.tv_sec, time_spec_curre.tv_nsec);

			if( time_spec_curre.tv_sec - time_spec_start.tv_sec >= 2 )
			{
				break;
			}
			else if( time_spec_curre.tv_sec - time_spec_start.tv_sec == 1 )
			{
				if( time_spec_curre.tv_nsec + (1000000000 - time_spec_start.tv_nsec) >= u32WaitNs )
				{
					break;
				}
 			}
			else
			{
				if( time_spec_curre.tv_nsec - time_spec_start.tv_nsec >= u32WaitNs )
				{
					break;
				}
			}
		}

		//printk("start: tv_sec = %d, tv_nsec = %9d\n", time_spec_start.tv_sec, time_spec_start.tv_nsec);
		//printk("curre: tv_sec = %d, tv_nsec = %9d\n", time_spec_curre.tv_sec, time_spec_curre.tv_nsec);

	#endif

    return bCurrPlugStatus;
}


int mmc_sdio_hotplug(void *data)
{
	struct mmc_host *host = data;
	static int card_det = 0;

	while(1)
	{
		LABEL_LOOP_HOTPLUG:

		if(HalSdio_GetCardDetect()) // Insert (CDZ)
		{
			if (card_det == 0)
			{
				//printk("sd plug in\n");
				if( 0 == _CardDetect_PlugDebounce(500, 1) )
					goto LABEL_LOOP_HOTPLUG;

				//Hal_SDMMC_WaitProcessCtrl(eIP, FALSE);
				mmc_detect_change(host, msecs_to_jiffies(300));
				printk("\n>> [SDIO] ##########...(Inserted) OK! \n");
			}
			card_det = 1;
		}
		else // Remove (CDZ)
		{
			if (card_det == 1)
			{
				//printk("sd plug out\n");

				if(host->card)
					host->card->state |= MMC_CARD_REMOVED;

				//Hal_SDMMC_WaitProcessCtrl(eIP, TRUE);
				mmc_detect_change(host, msecs_to_jiffies(0));
				printk("\n>> [SDIO] ##########...(Ejected) OK!\n");
			}
			card_det = 0;
			//p_sdmmc_slot->bad_card = FALSE;
			//p_sdmmc_slot->rca = 0;
		}

		msleep(50); // most fast handy hotplug is 80ms, set 50ms here to make sure detect every change

	}
}

#endif


#ifdef CONFIG_DEBUG_FS

static int mmc_sdio_perf_show(struct seq_file *seq, void *v)
{
	#ifdef ENABLE_SDIO_DEBUGFS_PROFILE

	U32 i;

	for(i=0; i < REC_REQUEST_NUM; i++)
	{
		if(!profile_fcie.req[i].sector.length)
		{
            printk("break at %d\n", i);
            break;
        }
        printk(", %d, %d, %d, %d, %d, %d, %d, %d\n",
                                            profile_fcie.req[i].sector.addr,
                                            profile_fcie.req[i].sector.length,

                                            profile_fcie.req[i].pre_request.t_begin,
                                            profile_fcie.req[i].pre_request.t_end,

                                            profile_fcie.req[i].request.t_begin,
                                            /*profile_fcie.req[i].transfer.t_start_dma,
                                            profile_fcie.req[i].transfer.t_bh_run,
                                            profile_fcie.req[i].transfer.t_irq_happen,*/
                                            profile_fcie.req[i].request.t_end,

                                            profile_fcie.req[i].post_request.t_begin,
                                            profile_fcie.req[i].post_request.t_end  );
	}

	__memzero(&profile_fcie, sizeof(struct profile_hcd));

	#endif

	return 0;
}

static int mmc_sdio_perf_open(struct inode *inode, struct file *file)
{
	return single_open(file, mmc_sdio_perf_show, inode->i_private);
}

static const struct file_operations mmc_sdio_fops_perf = {
	.owner		= THIS_MODULE,
	.open		= mmc_sdio_perf_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static void mmc_sdio_debugfs_attach(struct mmc_host *host)
{
	struct device *dev = mmc_dev(host);

	debug_root = debugfs_create_dir(dev_name(dev), NULL);
	if(IS_ERR(debug_root))
	{
		dev_err(dev, "failed to create debugfs root\n");
		return;
	}

	debug_perf = debugfs_create_file("sdio_performance", 0444, debug_root, NULL, &mmc_sdio_fops_perf);
	if(IS_ERR(debug_perf))
	{
		dev_err(dev, "failed to create debug regs file\n");
	}
}

static void mmc_sdio_debugfs_remove(void)
{
	debugfs_remove(debug_perf);
	debugfs_remove(debug_root);
}

#else

static inline void mmc_sdio_debugfs_attach(struct mmc_host *host) { }
static inline void mmc_sdio_debugfs_remove(void) { }

#endif


module_init(mmc_sdio_init);
module_exit(mmc_sdio_exit);

MODULE_DESCRIPTION("mstar mmc host sdio");

