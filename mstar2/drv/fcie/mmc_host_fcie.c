

#include "mmc_host_fcie.h"


extern struct semaphore	PfModeSem;


#define DBG_REQ(MSG)             //MSG


void mmc_fcie_send_command(struct mmc_command *cmd)
{
	E_IO_STS IoStatus;
    CMD_RSP_INFO CmdRspInfo;
	static U32 u32LastClock = 0;
	static unsigned char u8LastTiming = 0;
	struct host_fcie *pHostFcie;

	pHostFcie = mmc_priv(cmd->mrq->host);

	DBG_REQ(printk("  CMD%d_%08X", cmd->opcode, cmd->arg));
	memset(&CmdRspInfo, 0, sizeof(CMD_RSP_INFO));
	CmdRspInfo.Command.Cmd.Index = cmd->opcode;
	CmdRspInfo.Command.Cmd.Arg = U32BEND2LEND(cmd->arg);
	CmdRspInfo.ClockStop = CLK_STOP;

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
			printk("FCIE Err: not support response type");
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
				CmdRspInfo.ReadWriteDir = DIR_R;
				CmdRspInfo.ReadWriteTimeOut = 10; // need to refine or check SD card register
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

		#if defined(ENABLE_FCIE_ADMA)&&ENABLE_FCIE_ADMA
			CmdRspInfo.DataPath = PATH_ADMA;
			HalFcie_PrepareDescriptors(cmd->data);
			HalFcie_SetupDescriptorAddr(&CmdRspInfo);
		#else
			CmdRspInfo.DataPath = PATH_DMA;
			CmdRspInfo.BusAddr = sg_dma_address(cmd->data->sg);
			CmdRspInfo.BlockCount = cmd->data->blocks;
		#endif

		cmd->data->bytes_xfered = cmd->data->blksz * cmd->data->blocks;

	}


	IoStatus = HalFcie_SendCommand(&CmdRspInfo);


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

		if( cmd->opcode==17 || cmd->opcode==18 || cmd->opcode==24 || cmd->opcode==25 )
		{
			pHostFcie->rw_timeout = 0;
		}

	}
	else
	{
		if( cmd->opcode!=52 && cmd->opcode!=5 )
		{
			printk(LIGHT_RED"fcie req CMD%d_%08X fail %02Xh"NONE"\n", cmd->opcode, cmd->arg, IoStatus);
		}

		if( cmd->opcode==17 || cmd->opcode==18 || cmd->opcode==24 || cmd->opcode==25 )
		{
			pHostFcie->rw_timeout++;
			if(pHostFcie->rw_timeout>=2)
			{
				pHostFcie->rw_timeout = 0;
				pHostFcie->bad_card = 1;
			}
		}

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


void mmc_fcie_request(struct mmc_host *host, struct mmc_request *req)
{
	struct host_fcie *pHostFcie;
	DBG_REQ(printk(LIGHT_CYAN"%s()"NONE"\n", __FUNCTION__));

	if(!req->cmd->mrq->host)
	{
		req->cmd->mrq->host = host; // fill in for mmc_fcie_send_command() use
	}

	pHostFcie = mmc_priv(host);

	if(pHostFcie->bad_card)
	{
		printk("fcie reject bad card request CMD%02d_%08Xh\n", req->cmd->opcode, req->cmd->arg);
		req->cmd->error = -EIO;
		req->cmd->retries = 0;
		mmc_request_done(host, req);
		return;
	}

	if(req->data) // command with data transfer
	{
		mmc_fcie_pre_req(req->data);
	}

    down(&PfModeSem);

	HalFcie_SwitchPad(SDIO_PAD_SDR12); // switch pad for share IP
	// reconfig clock for share IP

	if(req->sbc)
	{
		mmc_fcie_send_command(req->sbc);
	}

	mmc_fcie_send_command(req->cmd);

	if(req->stop)
	{
		mmc_fcie_send_command(req->stop);
	}

    up(&PfModeSem);

	if(req->data) // command with data transfer
	{
		mmc_fcie_post_req(req->data);
	}

	mmc_request_done(host, req);
}


void mmc_fcie_pre_req(struct mmc_data *data)
{
	//printk("data sg_len = %d\n", data->sg_len);

	if(data->flags & MMC_DATA_READ)
	{
		dma_map_sg(data->mrq->host->parent, data->sg, data->sg_len, DMA_FROM_DEVICE);
	}
	else
	{
		dma_map_sg(data->mrq->host->parent, data->sg, data->sg_len, DMA_TO_DEVICE);
	}
}


void mmc_fcie_post_req(struct mmc_data *data)
{
	if(data->flags & MMC_DATA_READ)
	{
		dma_unmap_sg(data->mrq->host->parent, data->sg, data->sg_len, DMA_FROM_DEVICE);
	}
	else
	{
		dma_unmap_sg(data->mrq->host->parent, data->sg, data->sg_len, DMA_TO_DEVICE);
	}
}


void mmc_fcie_set_ios(struct mmc_host *host, struct mmc_ios *ios)
{
	static unsigned int clock = 0;
	static unsigned short vdd = 0;
	static unsigned char power_mode = 0;

	//printk(LIGHT_CYAN"%s()"NONE"\n", __FUNCTION__);

	if(clock!=ios->clock)
	{
		down(&PfModeSem);

		clock = ios->clock;
		//printk(YELLOW"clock = %d"NONE"\n", clock/1000);
		if(clock)
		{
			HalFcie_SetClock(clock/1000);
			HalFcie_OpenClock(1);
		}
		else
		{
			HalFcie_OpenClock(0);
		}

		up(&PfModeSem);
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
			HalFcie_SetCardPower(1);
		}
		else
		{
			HalFcie_SetCardPower(0);
		}
	}
}


int mmc_fcie_get_cd(struct mmc_host *host)
{
	#ifdef CONFIG_MSTAR_FCIE_CARD_DETECT
		int cd;
		cd = HalFcie_GetCardDetect();
		//printk(LIGHT_CYAN"%s() %d"NONE"\n", __FUNCTION__, cd);
		return cd;
	#else
		return 1;
	#endif
}


int mmc_fcie_get_ro(struct mmc_host *host)
{
	#ifdef CONFIG_MSTAR_FCIE_WRITE_PROTECT
		//printk(LIGHT_CYAN"%s()"NONE"\n", __FUNCTION__);
		return HalFcie_GetWriteProtect();
	#else
		return 0;
	#endif
}


static const struct mmc_host_ops mmc_fcie_ops =
{
	.request	= mmc_fcie_request,
	.set_ios	= mmc_fcie_set_ios,
	.get_cd		= mmc_fcie_get_cd,
	.get_ro		= mmc_fcie_get_ro,

	//.pre_req	= fcie_pre_req,
	//.post_req	= fcie_post_req,

	//.enable_sdio_irq = ms_sdmmc_enable_sdio_irq,
};


static int mmc_fcie_probe(struct platform_device *pdev)
{
	struct mmc_host *host = NULL;
	int IrqRequestResult = 0;

	printk("mstar fcie host running...\n");

    down(&PfModeSem);
	HalFcie_Init();
	up(&PfModeSem);

	host = mmc_alloc_host(sizeof(struct host_fcie), &pdev->dev);
	if(!host)
	{
		printk("mmc_alloc_host fail!\n");
	}

	host->ops = &mmc_fcie_ops;
	host->f_min =   300000;
	host->f_max = 50000000;
	host->ocr_avail = MMC_VDD_165_195|MMC_VDD_27_28|MMC_VDD_28_29|MMC_VDD_29_30|MMC_VDD_30_31|MMC_VDD_31_32|MMC_VDD_32_33;
	host->caps = MMC_CAP_4_BIT_DATA|MMC_CAP_MMC_HIGHSPEED|MMC_CAP_SD_HIGHSPEED;
	//host->caps |= MMC_CAP_NEEDS_POLL;
	//host->caps |= MMC_CAP_UHS_SDR50|MMC_CAP_UHS_DDR50|MMC_CAP_UHS_SDR104;
	host->max_seg_size = 512 * 1024;
	host->max_segs = 1;
	#if defined(ENABLE_FCIE_ADMA)&&ENABLE_FCIE_ADMA
	host->max_segs = 512;
	#endif
	host->max_req_size = 512 * 1024;
	host->max_blk_size = 512;
	host->max_blk_count = 1024;

	mmc_add_host(host);
	platform_set_drvdata(pdev, host);

	#if defined(ENABLE_FCIE_INTERRUPT_MODE)&&ENABLE_FCIE_INTERRUPT_MODE
    	IrqRequestResult = request_irq(FCIE_INT_VECTOR, HalFcie_KernelIrq, IRQF_SHARED, "mstar fcie", host);
		if(IrqRequestResult)
		{
			printk("request fcie irq fail %d\n", IrqRequestResult);
		}
	#endif

	#ifdef CONFIG_MSTAR_FCIE_CARD_DETECT
   		kthread_run(mmc_fcie_hotplug, host, "mstar fcie hotplug");
	#endif

	return 0;
}


static int mmc_fcie_remove(struct platform_device *pdev)
{
	printk(LIGHT_CYAN"%s()\n"NONE, __FUNCTION__);
	return 0;
}


static int mmc_fcie_suspend(struct platform_device *pdev, pm_message_t state)
{
	int ret = 0;
	struct mmc_host *host= platform_get_drvdata(pdev);

	printk(LIGHT_CYAN"%s()\n"NONE, __FUNCTION__);
	ret = mmc_suspend_host(host);
	if(ret){
		printk("fcie mmc_suspend_host fail %d\n", ret);
		return ret;
	}

	#ifdef SDIO_SUPPORT_SD30
		HalSdio_SetIOVoltage(VOLT_00V); // need to clear for resume
	#endif

	return 0;
}


static int mmc_fcie_resume(struct platform_device *pdev)
{
	int ret = 0;
	struct mmc_host *host= platform_get_drvdata(pdev);

	printk(LIGHT_CYAN"%s()\n"NONE, __FUNCTION__);
	down(&PfModeSem);
	HalFcie_Init();
	up(&PfModeSem);
	ret = mmc_resume_host(host);
	if(ret){
		printk("fcie mmc_suspend_host fail %d\n", ret);
		return ret;
	}

	return 0;
}


static u64 mmc_dmamask = 0xffffffffUL;


static struct platform_device mmc_fcie_device =
{
    .name = "mstar_fcie",
    .id = 0,
    .dev =
    {
        .dma_mask = &mmc_dmamask,
        .coherent_dma_mask = 0xffffffffUL,
    },
};


static struct platform_driver mmc_fcie_driver =
{
    .probe   = mmc_fcie_probe,
    .remove  = mmc_fcie_remove,
    .suspend = mmc_fcie_suspend,
    .resume  = mmc_fcie_resume,
    .driver  =
    {
        .name  = "mstar_fcie",
        .owner = THIS_MODULE,
    },
};


static s32 mmc_fcie_init(void)
{
	platform_device_register(&mmc_fcie_device);
	platform_driver_register(&mmc_fcie_driver);
	return 0;
}


static void mmc_fcie_exit(void)
{
    platform_driver_unregister(&mmc_fcie_driver);
}

#ifdef CONFIG_MSTAR_FCIE_CARD_DETECT

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
			bCurrPlugStatus = HalFcie_GetCardDetect();

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


int mmc_fcie_hotplug(void *data)
{
	struct mmc_host *host = data;
	static int card_det = 0;
	struct host_fcie *pHostFcie;

	pHostFcie = mmc_priv(host);

	while(1)
	{
		LABEL_LOOP_HOTPLUG:

		if(HalFcie_GetCardDetect()) // Insert (CDZ)
		{
			if (card_det == 0)
			{
				//printk("sd plug in\n");
				if( 0 == _CardDetect_PlugDebounce(500, 1) )
					goto LABEL_LOOP_HOTPLUG;

				//Hal_SDMMC_WaitProcessCtrl(eIP, FALSE);
				mmc_detect_change(host, msecs_to_jiffies(300));
				printk("\n>> [FCIE] ##########...(Inserted) OK! \n");
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
				printk("\n>> [FCIE] ##########...(Ejected) OK!\n");
			}
			card_det = 0;
			pHostFcie->bad_card = 0;
			//p_sdmmc_slot->rca = 0;
		}

		msleep(50); // most fast handy hotplug is 80ms, set 50ms here to make sure detect every change

	}
}

#endif

module_init(mmc_fcie_init);
module_exit(mmc_fcie_exit);

MODULE_DESCRIPTION("mstar mmc host fcie");

