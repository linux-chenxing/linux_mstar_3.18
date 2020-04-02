/***************************************************************************************************************
 *
 * FileName hal_sdmmc.c
 *     @author jeremy.wang (2011/07/21)
 * Desc:
 * 	   HAL SD Driver will support basic SD function but not flow process.
 *         The goal is that we don't need to change HAL Level code (But its h file code)
 *
 *	   The limitation were listed as below:
 * 	   (1) This c file belongs to HAL level.
 *	   (2) Its h file is included by driver API level, not driver flow process.
 * 	   (3) FCIE/SDIO IP register and buffer opertion function belong to here.
 * 	   (4) Only IP flow concept, but not SD command concept.
 * 	   (5) This c file could not use project/os/cpu/icver/specific define option here, but its h file could.
 *
 * 	   P.S. EN_XX for ON/OFF Define, , V_XX  for Value Define,
 * 			RT_XX for Retry Times Define, WT_XX for Wait Time Define, M_XX for Mask Value Define
 *
 ***************************************************************************************************************/
#include "ms_sdmmc_lnx.h"
#include "hal_sdmmc.h"
#include "hal_card_timer.h"
#include "hal_card_regs5.h"
#include "hal_card_intr.h"  //inlcue but may not use
#include "hal_card_common.h"
#include "sd_platform.h"
//***********************************************************************************************************
// Config Setting (Internel)
//***********************************************************************************************************

// Enable Setting
//-----------------------------------------------------------------------------------------------------------
#define EN_TRFUNC         (FALSE)
#define EN_DUMPREG		  (TRUE)
#define EN_BYTEMODE		  (TRUE)
#define EN_DYNTIMEOUT     (FALSE)

// Retry Times
//-----------------------------------------------------------------------------------------------------------
#define RT_CLEAN_SDSTS				3
#define RT_CLEAN_MIEEVENT			3

// Wait Time
//-----------------------------------------------------------------------------------------------------------
#define WT_DAT0HI_END				3000	//(ms)
#define WT_FIFO_CLK_RDY				100		//(ms)
#define WT_RATIO_READ               1       //(ms)
#define WT_RATIO_WRITE              2       //(ms)

//***********************************************************************************************************
//***********************************************************************************************************


//#define M_SD_ERRSTS				(R_SD_DAT_CERR|R_SD_DAT_STSERR|R_SD_DAT_STSNEG|R_SD_CMD_NORSP|R_SD_CMDRSP_CERR) //0x1F
#define M_REG_SDMIEEvent(IP)        (FCIE_RIU_R16(A_MIE_EVENT_REG(IP)) & BIT_ALL_CARD_INT_EVENTS)

#define M_REG_STSERR(IP)            (FCIE_RIU_R16(A_SD_STS_REG(IP)) & BIT_SD_ERR_BITS)  //0x1F
#define A_SD_REG_BANK(IP)           GET_CARD_BANK(IP, EV_REG_BANK)

#define A_SD_CIFC_BANK(IP)          GET_CARD_BANK(IP, EV_CIFC_BANK) // TODO: fix here...

#define A_SDPLL_BANK()           	GET_CARD_REG_ADDR(A_RIU_BASE, 0x11f00)

#define A_MIE_EVENT_REG(IP)         GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x00)
#define A_MIE_INT_EN_REG(IP)        GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x01)
#define A_MMA_PRI_REG(IP)			GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x02)
#define A_SDIO_ADDR0_REG(IP)        GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x03)
#define A_SDIO_ADDR1_REG(IP)        GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x04)
#define A_DMA_LEN_LO(IP)			GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x05)
#define A_DMA_LEN_HI(IP)			GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x06)
#define A_MIE_FUNC_CTL(IP)			GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x07)
#define A_JOB_BL_CNT_REG(IP)        GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x08)
#define A_BLK_SIZE_REG(IP)			GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x09)
#define A_RSP_SIZE_REG(IP)          GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x0A)
#define A_SD_MODE_REG(IP)           GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x0B)
#define A_SD_CTL_REG(IP)            GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x0C)
#define A_SD_STS_REG(IP)            GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x0D)
#define A_SDIO_MODE(IP)				GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x11)
#define A_TEST_MODE(IP)				GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x15)
#define A_NC_REORDER_REG(IP)        GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x2D)
#define A_SDIO_INTR_DET(IP)			GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x2F)
#define A_DEBUG_BUS_0(IP)			GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x38)
#define A_DEBUG_BUS_1(IP)			GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x39)
#define A_CLK_EN(IP)				GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x3E)
#define A_FCIE_RST(IP)				GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x3F)

// Reg Dynamic Variable
//-----------------------------------------------------------------------------------------------------------
static RspStruct gst_RspStruct[2];
static volatile BOOL_T  gb_StopWProc[2] = {0};
static volatile U16_T   gu16_SD_MODE_DatLine[2] = {0};
static volatile U16_T	gu16_SD_Mode_DatSync[2] = {0};
static volatile U16_T   gu16_WT_NAC[2] = {0};             //Waiting Time for Nac (us)
static volatile U16_T   gu16_WT_READ[2] = {0};            //Waiting Time for Reading (ms)
static volatile U16_T   gu16_WT_WRITE[2] = {0};           //Waiting Time for Writing (ms)

// Trace Funcion
//-----------------------------------------------------------------------------------------------------------
#if (EN_TRFUNC)
	#define TR_SDMMC(p)	    p
#else
	#define TR_SDMMC(p)
#endif

void _SDMMC_REG_Dump(/*IPEmType*/unsigned char eIP);

// Register Operation Define ==> For Get Current DAT0 Value
//-----------------------------------------------------------------------------------------------------------
static U16_T _REG_GetDAT0(IPEmType eIP)
{
	return (FCIE_RIU_R16(A_SD_STS_REG(eIP)) & BIT_SD_CARD_BUSY);
}

// Register Operation Define ==> For Get Current CDET Value
// Register Operation Define ==> For Clean Reg and Special Case
//-----------------------------------------------------------------------------------------------------------
static RetEmType _REG_ClearSDSTS(IPEmType eIP, U8_T u8Retry)
{
	do
	{
		FCIE_RIU_16_ON(A_SD_STS_REG(eIP), BIT_SD_ERR_BITS);

		if ( gb_StopWProc[eIP] )
		{
			printk("stop w proc in clear sts\n");
			return EV_FAIL;
		}
		if ( !M_REG_STSERR(eIP) )
		{
			return EV_OK;
		}
		else if(!u8Retry)
		{
			printk("clear sts to\n");
			return EV_FAIL;
		}
	} while(u8Retry--);

	return EV_FAIL;
}


// Register Operation Define ==> For Clear MIE Event
//-----------------------------------------------------------------------------------------------------------
static RetEmType _REG_ClearMIEEvent(IPEmType eIP, U8_T u8Retry)
{
	// clear mie event until event clean
	do
	{
		FCIE_RIU_W16(A_MIE_EVENT_REG(eIP), BIT_ALL_CARD_INT_EVENTS);

		if ( gb_StopWProc[eIP] )
			return EV_FAIL;

		if ( !M_REG_SDMIEEvent(eIP) )
			break;
		else if(!u8Retry)
			return EV_FAIL;

	} while(u8Retry);


	#if(EN_BIND_CARD_INT)

		Hal_CARD_INT_ClearMIEEvent(eIP); // clear global variable which save interrupt event

	#endif

	return EV_OK;
}

// Register Operation Define ==> For Wait DAT0 High
//-----------------------------------------------------------------------------------------------------------
static RetEmType _REG_WaitDat0HI(IPEmType eIP, U32_T u32WaitMs)
{
    U32_T u32DiffTime = 0;

	do
	{
		if ( gb_StopWProc[eIP] )
			return EV_FAIL;

		if ( _REG_GetDAT0(eIP) )
			return EV_OK;

		Hal_Timer_uDelay(1);
		u32DiffTime++;

	} while(u32DiffTime <= (u32WaitMs*1000));

	printk("_REG_WaitDat0HI() timeout\n");

	_SDMMC_REG_Dump(eIP);

	return EV_FAIL;
}


// Register Operation Define ==> For Wait MIE Event
//-----------------------------------------------------------------------------------------------------------
static RetEmType _REG_WaitMIEEvent(IPEmType eIP, U16_T u16ReqEvent, U32_T u32WaitMs)
{
	U32_T u32DiffTime = 0;

	#if(EN_BIND_CARD_INT)
	if ( Hal_CARD_INT_MIEModeRunning(eIP, EV_INT_SD) )	// Interrupt Mode
	{
		if ( !Hal_CARD_INT_WaitMIEEvent(eIP, u16ReqEvent, u32WaitMs) )
		{
			do
			{
				if ( (FCIE_RIU_R16(A_MIE_EVENT_REG(eIP))&u16ReqEvent) == u16ReqEvent )
				{
					printk("wait mie intr timeout but polling evnet %04Xh OK\n", u16ReqEvent);
					return EV_OK;
				}
				Hal_Timer_uDelay(1);
				u32DiffTime++;

			} while(u32DiffTime <= (u32WaitMs*1000));

			printk("wait mie intr & polling event %04Xh timeout, Event now: %04Xh\n", u16ReqEvent, FCIE_RIU_R16(A_MIE_EVENT_REG(eIP)));

			return EV_FAIL;
		}
		else
			return EV_OK;
	}
	else // Polling Mode
	#endif
	{
		do
		{
			if ( gb_StopWProc[eIP] )
				return EV_FAIL;

			if ( (FCIE_RIU_R16(A_MIE_EVENT_REG(eIP))&u16ReqEvent) == u16ReqEvent )
				return EV_OK;

			Hal_Timer_uDelay(1);
			u32DiffTime++;

		} while(u32DiffTime <= (u32WaitMs*1000));

		printk("polling mie event timeout\n");

		return EV_FAIL;
	}

}

// Register Operation Define ==> For Software Reset
//-----------------------------------------------------------------------------------------------------------

#define RESET_WAIT_US 2000

static void _REG_ResetIP(IPEmType eIP)
{
    U16_T u16_us = 0;

    FCIE_RIU_16_OF(A_SD_CTL_REG(eIP), BIT_JOB_START); // clear for safe

	Hal_CARD_SetClock(eIP, CLK_F);

    FCIE_RIU_16_OF(A_FCIE_RST(eIP), BIT_FCIE_SOFT_RST_n); /* active low */

    for(u16_us=0; u16_us<RESET_WAIT_US; u16_us++)
    {
        if((FCIE_RIU_R16(A_FCIE_RST(eIP))&BIT_RST_STS_MASK)==BIT_RST_STS_MASK)
        {
            break;
        }
        udelay(1);
    }

	if(u16_us==RESET_WAIT_US)
	{
		printk("SdErr: FCIE/SDIO(%d) reset fail %04Xh\n", eIP, FCIE_RIU_R16(A_FCIE_RST(eIP)));

	}

    FCIE_RIU_16_ON(A_FCIE_RST(eIP), BIT_FCIE_SOFT_RST_n);
}



// SDMMC Internel Logic Function
//-----------------------------------------------------------------------------------------------------------

void HalFcie_DumpRegisterBank(ULONG_T ulBank, U16_T u16WordCount)
{
	U16_T u16_i;
	volatile U16_T u16_reg;

	printk("\n BANK [%08lXh]", ulBank);

	for(u16_i=0 ; u16_i<u16WordCount; u16_i++)
	{
		if(0 == u16_i%8)
			printk("\n%02Xh:| ", u16_i);

		u16_reg = FCIE_RIU_R16(GET_REG_ADDR(ulBank, u16_i));
		printk("%04Xh ", u16_reg);
	}
    printk("\n");
}

void HalFcie_DumpRegister(void)
{
	//U16 u16_i;

	printk("\n\nfcie reg:");

	HalFcie_DumpRegisterBank(A_FCIE2_0_BANK, 0x40); // FCIE

	printk("\n\nsdio reg:");
	HalFcie_DumpRegisterBank(A_FCIE1_0_BANK, 0x40); // SDIO

#if 0
	printk("FCIE JobCnt: \n");
	printk("JobCnt: %Xh \n", REG_FCIE(FCIE_JOB_BL_CNT));

	printk("\nFCIE CMDFIFO:");
	//eMMC_FCIE_GetCMDFIFO(0, 0x09, (U16*)sgau16_eMMCDebugReg);
	for(u16_i=0 ; u16_i<0x20; u16_i++)
	{
		if(0 == u16_i%8)
			printk("\n%02Xh:| ", u16_i);

		printk("%04Xh ", sgau16_eMMCDebugReg[u16_i]);
	}
#endif

    printk("\n\nchiptop reg:");
	HalFcie_DumpRegisterBank(REG_BANK_CHIPTOP, 0x80);

//    printk("\n\nsdio_pll reg:");
//	HalFcie_DumpRegisterBank(SDIO_PLL_BASE, 0x80);

    printk("\n\nclockgen reg:");
	HalFcie_DumpRegisterBank(CLKGEN_BANK, 0x80);


}

void HalFcieSdio_ClockTest(unsigned char eIP)
{
	U8_T u8_i;

	FCIE_RIU_16_ON(A_CLK_EN(eIP), BIT_TEST_CLK);

	for(u8_i=0; u8_i<10; u8_i++)
	{
		if( (FCIE_RIU_R16(A_CLK_EN(eIP))&BITS_TEST_OK)==BITS_TEST_OK )
		{
			break;
		}
		udelay(100);
	}
	if(u8_i==10)
	{
		printk("SdErr: Clock test fail to set, %04Xh", FCIE_RIU_R16(A_CLK_EN(eIP)));
		panic("SdErr");
	}

	FCIE_RIU_16_OF(A_CLK_EN(eIP), BIT_TEST_CLK);

	for(u8_i=0; u8_i<10; u8_i++)
	{
		if( (FCIE_RIU_R16(A_CLK_EN(eIP))&BITS_TEST_OK)==0 )
		{
			break;
		}
		udelay(100);
	}
	if(u8_i==10)
	{
		printk("SdErr: Clock test fail to clear, %04Xh", FCIE_RIU_R16(A_CLK_EN(eIP)));
		panic("SdErr");
	}

	printk("Clock test OK\n");

}

void HalFcieSdio_DumpDebugBus(unsigned char eIP)
{
	const char *s_debud_mode[4] = {	"CARD_CMD_DBUS", "CARD_DATA1_DBUS", "CARD_DATA2_DBUS", "CARD_ADMA_DBUS"	};
	int i;

	FCIE_RIU_16_OF(A_DEBUG_BUS_1(eIP), BIT11|BIT10|BIT9|BIT8);
	FCIE_RIU_16_ON(A_DEBUG_BUS_1(eIP), 5<<8); // 5: card

	for(i=1; i<5; i++)
	{
		FCIE_RIU_16_OF(A_TEST_MODE(eIP), BIT_DEBUG_MODE_MASK);
		FCIE_RIU_16_ON(A_TEST_MODE(eIP), i<<BIT_DEBUG_MODE_SHIFT); // 2: card_data1_dbus = {xxxxx, Wrstate, RDstate}

		printk("\n=== FCIE/SDIO [%d] %s ===\n", i, s_debud_mode[i-1]);
		printk("\tFCIE/SDIO[0x15] = %04Xh\n", FCIE_RIU_R16(A_TEST_MODE(eIP)));
		printk("\tFCIE/SDIO[0x39] = %04Xh\n", FCIE_RIU_R16(A_DEBUG_BUS_1(eIP)));
		printk("\tFCIE/SDIO[0x38] = %04Xh\n", FCIE_RIU_R16(A_DEBUG_BUS_0(eIP)));
		printk("===================================\n");
	}

}

void _SDMMC_REG_Dump(/*IPEmType*/unsigned char eIP)
{
	int i;

	printk("_SDMMC_REG_Dump\n");

	FCIE_RIU_16_ON(A_SD_CTL_REG(eIP), BIT_CHK_CMD); // add for check CIFC
	printk("------------------------------------------\n");
    printk("\nFCIE/SDIO[%lXh]:\n", A_SD_REG_BANK(eIP));
	for(i=0; i<0x80; i++)
	{
		if(i%8==0) printk("%02X: ", (i/8)*8);
        printk("0x%04X ", FCIE_RIU_R16(GET_CARD_REG_ADDR(A_SD_REG_BANK(eIP), i)) );
        if(i%8==7) printk(":%02X\n", ((i/8)*8)+7);
	}
	FCIE_RIU_16_OF(A_SD_CTL_REG(eIP), BIT_CHK_CMD); // remove check bit

	HalFcieSdio_DumpDebugBus(eIP);

    printk("------------------------------------------\n");
    printk("\nCHIPTOP:\n");
	for(i=0; i<0x80; i++)
	{
		if(i%8==0) printk("%02X: ", (i/8)*8);
        printk("0x%04X ", FCIE_RIU_R16(GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, i)) );
        if(i%8==7) printk(":%02X\n", ((i/8)*8)+7);
	}

    printk("------------------------------------------\n");
    printk("\nCLKGEN:\n");
	for(i=0; i<0x80; i++)
	{
		if(i%8==0) printk("%02X: ", (i/8)*8);
        printk("0x%04X ", FCIE_RIU_R16(GET_CARD_REG_ADDR(CLKGEN_BANK, i)) );
        if(i%8==7) printk(":%02X\n", ((i/8)*8)+7);
	}
#if 0
    printk("------------------------------------------\n");
    printk("\nEMMC_PLL:\n");
	for(i=0; i<0x80; i++)
	{
		if(i%8==0) printk("%02X: ", (i/8)*8);
        printk("0x%04X ", FCIE_RIU_R16(GET_CARD_REG_ADDR(EMMC_PLL_BASE, i)) );
        if(i%8==7) printk(":%02X\n", ((i/8)*8)+7);
	}

	HalFcieSdio_ClockTest(eIP);
#endif

#if 0
    printk("------------------------------------------\r\n");
     printk("\nSDIOPLL:\n");
	for(i=0; i<0x80; i++)
    {        if(i%8==0) printk("%02X: ", (i/8)*8);

        printk("0x%04X ", FCIE_RIU_R16(GET_CARD_REG_ADDR(A_SDPLL_BANK(), i)) );
        if(i%8==7) printk(":%02X\r\n", ((i/8)*8)+7);
	}
    printk("------------------------------------------\r\n");
#endif
#if 0
    printk("\nDump FCIE3 debug ports:\n");
	for(i=0; i<=7; i++)
	{		// Enable debug ports
        FCIE_RIU_16_OF(A_TEST_MODE_REG(eIP), (BIT10|BIT09|BIT08));
        FCIE_RIU_16_ON(A_TEST_MODE_REG(eIP), i<<8);
		// Dump debug ports
		printk("\nDebug Mode \033[31m%d:\033[m\n", i);
		printk("DBus[15:0]=\033[34m%04X\033[m\n", FCIE_RIU_R16(A_DBGL_REG(eIP)));
		printk("DBus[23:16]=\033[34m%04X\033[m\n", FCIE_RIU_R16(A_DBGH_REG(eIP)));
	}
	printk("------------------------------------------\r\n");
#endif
}

void HalFcieSdio_CheckStateMachine(unsigned char eIP)
{
	FCIE_RIU_16_OF(A_DEBUG_BUS_1(eIP), BIT11|BIT10|BIT9|BIT8);
	FCIE_RIU_16_ON(A_DEBUG_BUS_1(eIP), 5<<8); // 5: card

	FCIE_RIU_16_OF(A_TEST_MODE(eIP), BIT_DEBUG_MODE_MASK);
	FCIE_RIU_16_ON(A_TEST_MODE(eIP), 2<<BIT_DEBUG_MODE_SHIFT); // 2: card_data1_dbus = {xxxxx, Wrstate, RDstate}

	if(FCIE_RIU_R16(A_DEBUG_BUS_0(eIP))&0x0FFF) // Brian: check bit 0~11, RDstate, Wrstate, xxxxx(DMA?)
	{
		printk(LIGHT_RED"SdWarn: state machine not clean %04Xh, reset IP %s\n"NONE, FCIE_RIU_R16(A_DEBUG_BUS_0(eIP)), (eIP==0)?"SDIO":"FCIE");
		_REG_ResetIP(eIP);
	}
}

void HalFcieSdio_CheckIfFIFOStateIdle(unsigned char eIP)
{
	FCIE_RIU_16_OF(A_DEBUG_BUS_1(eIP), BIT11|BIT10|BIT9|BIT8);
	FCIE_RIU_16_ON(A_DEBUG_BUS_1(eIP), 5<<8); // 5: card

	FCIE_RIU_16_OF(A_TEST_MODE(eIP), BIT_DEBUG_MODE_MASK);
	FCIE_RIU_16_ON(A_TEST_MODE(eIP), 2<<BIT_DEBUG_MODE_SHIFT); // 2: card_data1_dbus = {xxxxx, Wrstate, RDstate}

	if(FCIE_RIU_R16(A_DEBUG_BUS_0(eIP))&0x0F00) // Brian: check bit 0~11, RDstate, Wrstate, xxxxx(DMA?)
	{
		printk(LIGHT_RED"SdErr: FIFO state not idle %04Xh\n"NONE, FCIE_RIU_R16(A_DEBUG_BUS_0(eIP)));
		//panic("fifo state not idle\n");
	}
}

void HalFcieSdio_CheckIfCmdStateIdle(unsigned char eIP)
{
	struct timespec time_spec_curre;
	struct timespec time_spec_start;
	struct timespec time_spec_difrt;
	U16_T u16Reg, u16RegUpdate;

	FCIE_RIU_16_OF(A_DEBUG_BUS_1(eIP), BIT11|BIT10|BIT9|BIT8);
	FCIE_RIU_16_ON(A_DEBUG_BUS_1(eIP), 5<<8); // 5: card

	FCIE_RIU_16_OF(A_TEST_MODE(eIP), BIT_DEBUG_MODE_MASK);
	FCIE_RIU_16_ON(A_TEST_MODE(eIP), 1<<BIT_DEBUG_MODE_SHIFT); // 2: card_data1_dbus = {xxxxx, Wrstate, RDstate}

	u16Reg = FCIE_RIU_R16(A_DEBUG_BUS_0(eIP));

	if(u16Reg&0x000F) // Fangson: [3:0] = cmd state
	{
		printk(LIGHT_RED"SdErr: CMD state not idle %04Xh\n"NONE, u16Reg);
		//panic("fifo state not idle\n");

		getnstimeofday(&time_spec_start);

		while(1)
		{
			u16RegUpdate = FCIE_RIU_R16(A_DEBUG_BUS_0(eIP));

			if( u16RegUpdate != u16Reg ) // reg change
			{
				printk(LIGHT_RED"update to %04Xh\n"NONE, u16RegUpdate);
				u16Reg = u16RegUpdate;
			}

			if( !(u16RegUpdate&0x000F) ) // cmd state back to idle
			{
				break;
			}

			getnstimeofday(&time_spec_curre);

			time_spec_difrt = timespec_sub(time_spec_curre, time_spec_start);

			if(time_spec_difrt.tv_nsec > 1000000) // 1 ms timeout
			{
				printk(LIGHT_RED"Wait %s IP back to idle timeout\n"NONE, (eIP==0)?"SDIO":"FCIE");
				break;
			}
		}
	}
}


void HalFcieSdio_WaitStateMachineFinsih(unsigned char eIP)
{
	//U16_T u16_i;
	struct timespec time_spec_curre;
	struct timespec time_spec_start;
	struct timespec time_spec_difrt;

	FCIE_RIU_16_OF(A_DEBUG_BUS_1(eIP), BIT11|BIT10|BIT9|BIT8);
	FCIE_RIU_16_ON(A_DEBUG_BUS_1(eIP), 5<<8); // 5: card

	FCIE_RIU_16_OF(A_TEST_MODE(eIP), BIT_DEBUG_MODE_MASK);
	FCIE_RIU_16_ON(A_TEST_MODE(eIP), 2<<BIT_DEBUG_MODE_SHIFT); // 2: card_data1_dbus = {xxxxx, Wrstate, RDstate}

	getnstimeofday(&time_spec_start);

	while(1)
	{
		if( !(FCIE_RIU_R16(A_DEBUG_BUS_0(eIP))&0x0FFF) ) // RdState back to "0: RD idle"
		{
			break;
		}

		getnstimeofday(&time_spec_curre);

		time_spec_difrt = timespec_sub(time_spec_curre, time_spec_start);

		//printk(LIGHT_RED"tv_nsec = %d\n"NONE, time_spec_difrt.tv_nsec);
		//printk(LIGHT_RED"%d "NONE, u16_i);
		//udelay(1); // udelay not precise while muji bring up
		//u16_i++;

		if(time_spec_difrt.tv_nsec > 1000000) // 1 ms timeout
		{
			printk(LIGHT_RED"Wait %s IP back to idle timeout\n"NONE, (eIP==0)?"SDIO":"FCIE");
			break;
		}
	}
}

static RspErrEmType _SDMMC_ErrProcess(IPEmType eIP, CmdEmType eCmdType, RspErrEmType eRspErr, BOOL_T bCloseClk, int Line)
{
	U16_T u16RspErr = (U16_T)eRspErr;
	U16_T u16IPErr = EV_STS_RIU_ERR | EV_STS_MIE_TOUT | EV_STS_FIFO_NRDY | EV_STS_DAT0_BUSY;
	unsigned short u16Reg;

	/****** (1) Record Information *******/
	gst_RspStruct[eIP].u32ErrLine = (U32_T)Line;
	gst_RspStruct[eIP].u8RspSize = (U8_T)FCIE_RIU_R16(A_RSP_SIZE_REG(eIP));
	gst_RspStruct[eIP].eErrCode = eRspErr;

	/****** (2) Dump and the Reg Info + Reset IP *******/

	if ( u16RspErr && gb_StopWProc[eIP] )
	{
		eRspErr = EV_SWPROC_ERR;
		_REG_ResetIP(eIP);
	}
	else if( u16RspErr & u16IPErr ) //SD IP Error
	{
		printk("SD IP Error u16RspErr = %04Xh, u16IPErr = %04Xh, line %d\n", u16RspErr, u16IPErr , Line);
		HalFcie_DumpRegister();
		_REG_ResetIP(eIP);
	}
	else if( u16RspErr & BIT_SD_ERR_BITS ) //SD_STS Reg Error
	{
		//Do Nothing
		//_SDMMC_REG_Dump(eIP);
	}
	//printk("sd status: %04Xh, ", FCIE_RIU_R16(A_SD_STS_REG(eIP)));
	if (FCIE_RIU_R16(A_SD_STS_REG(eIP)) & 0x37) // not check no rsp becuase of SDIO command
	{
		u16Reg = FCIE_RIU_R16(A_SD_STS_REG(eIP));
		printk("Check SDIO status error SDIO_STS = %04Xh\n", u16Reg);
		if(u16Reg&BIT_SD_R_CRC_ERR)		printk("SD Err: read CRC error\n");
		if(u16Reg&BIT_SD_RSP_CRC_ERR)	printk("SD Err: response CRC error\n");
		if(u16Reg&BIT_SD_W_CRC_ERR)		printk("SD Err: write CRC status error\n");
		if(u16Reg&BIT_SD_W_TIMEOUT)		printk("SD Err: transmitted data phase to error\n");
		if(u16Reg&BIT_SD_RSP_TIMEOUT)	printk("SD Err: command no response\n");
		if(u16Reg&BIT_SD_R_TIMEOUT)		printk("SD Err: received data phase to error\n");

#ifdef SDIO30
		if  ((u16Reg&0x11)==0)					//avoid tuning error
#endif
			_SDMMC_REG_Dump(eIP);

		//while(1);
	}

	//_REG_WaitDat0HI(eIP, WT_DAT0HI_END);	// some IC , hardware wait D0 high, some is not , so we have to wait
	//_SDMMC_REG_Dump(eIP);

	/****** (3) Close clock and DMA Stop function ******/
	if(bCloseClk)
	{
		#if defined SDIO_D1_INTR_MODE && SDIO_D1_INTR_MODE
		    if(eIP == EV_IP_FCIE1) // SDIO
		    {
				FCIE_RIU_16_OF(A_SD_MODE_REG(eIP), BIT_SD_DMA_R_CLK_STOP);
		    }
			else // FCIE
			{
				FCIE_RIU_16_OF(A_SD_MODE_REG(eIP), BIT_CLK_EN|BIT_SD_DMA_R_CLK_STOP);
			}
		#else
			FCIE_RIU_16_OF(A_SD_MODE_REG(eIP), BIT_CLK_EN|BIT_SD_DMA_R_CLK_STOP);
		#endif
	}

	/****** (4) Clock MIU Clock (SW) when SW open or not *******/
	//FCIE_RIU_16_OF(A_MMA_PRI_REG(eIP), R_MIU_CLK_EN_SW);

	return eRspErr;
}

//***********************************************************************************************************
// SDMMC HAL Function
//***********************************************************************************************************


static U8_T HalFcieSdio_GetCIFC(unsigned int * fcie_sdio_addr, U8_T u8addr)
{
    U16_T u16RegValue;

	//printk("HalFcieSdio_GetCIFC(%d)\n", u8addr);

	//printk("%08Xh --> ", ((unsigned int)(fcie_sdio_addr)) + ((u8addr>>1)<<2)   );

	u16RegValue = FCIE_RIU_R16(fcie_sdio_addr+(u8addr>>1));

	//printk("%04Xh\n", u16RegValue);

    if(u8addr&0x1) // high byte
    {
        return (U8_T)(u16RegValue>>8);
    }
    else // low byte
    {
        return (U8_T)(u16RegValue&0xFF);
    }
}

static void HalFcieSdio_SetCIFC(unsigned int * fcie_sdio_addr, IPEmType eIP, U8_T u8addr, U16_T u16Value)
{
    U16_T u16RegValue;

	FCIE_RIU_16_ON(A_SD_CTL_REG(eIP), BIT_CHK_CMD);

	u16RegValue = FCIE_RIU_R16(fcie_sdio_addr+(u8addr>>1));

    if(u8addr&0x1) // high byte
    {
		u16RegValue &= 0x00FF;
		u16RegValue |= (u16Value<<8);
    }
    else // low byte
    {
		u16RegValue &= 0xFF00;
		u16RegValue |= u16Value;
    }

	FCIE_RIU_W16(fcie_sdio_addr+(u8addr>>1), u16RegValue);

	FCIE_RIU_16_OF(A_SD_CTL_REG(eIP), BIT_CHK_CMD);

}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_SetCmdTokenToCIFC
 *     @author jeremy.wang (2011/11/29)
 * Desc: Set Cmd Token to CIFD
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param u8Cmd : SD Command
 * @param u32Arg : SD Argument
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_SetCmdTokenToCIFC(IPEmType eIP, U8_T u8Cmd, U32_T u32Arg)
{
	//U16_T au16_tmp[3];
	U32_T *_pu32_CIFCBuf;// = (U32_T *)(CARD_BANK(A_SD_CIFC_BANK(eIP)));

	gst_RspStruct[eIP].u8Cmd  = u8Cmd;
	gst_RspStruct[eIP].u32Arg = u32Arg;

	if(eIP==0) // SDIO
	{
		_pu32_CIFCBuf = (U32_T *)A_FCIE1_1_BANK;
	}
	else
	{
		_pu32_CIFCBuf = (U32_T *)A_FCIE2_1_BANK;
	}

#ifdef PRINT_CMD_ARG_RSP

	if(eIP==0) // SDIO
	{
		printk(LIGHT_RED"C%02d_%08Xh"NONE, u8Cmd, u32Arg);
	}
	else
	{
		printk(YELLOW	"C%02d_%08Xh"NONE, u8Cmd, u32Arg);
	}

#endif

#if 0
	FCIE_RIU_16_ON(FCIE_SD_CTRL, BIT_CHK_CMD);

	while(1)
	{
		//		eMMC_debug(0, 1, "FIFO 0 =  0x%4X\n",  ((u32_Arg>>24)<<8) | (0x40|u8_CmdIdx));
		//		eMMC_debug(0, 1, "FIFO 1 =  0x%4X\n",  (u32_Arg&0xFF00) | ((u32_Arg>>16)&0xFF));
		//		eMMC_debug(0, 1, "FIFO 2 =  0x%4X\n", u32_Arg&0xFF);
		FCIE_RIU_W16(FCIE_CMDFIFO_ADDR(0),((u32Arg>>24)<<8) | (u8Cmd|0x40));
		FCIE_RIU_W16(FCIE_CMDFIFO_ADDR(1), (u32Arg&0xFF00) | ((u32Arg>>16)&0xFF));
		FCIE_RIU_W16(FCIE_CMDFIFO_ADDR(2),  u32Arg&0xFF);

		au16_tmp[0]=FCIE_RIU_R16(FCIE_CMDFIFO_ADDR(0));
		au16_tmp[1]=FCIE_RIU_R16(FCIE_CMDFIFO_ADDR(1));
		au16_tmp[2]=FCIE_RIU_R16(FCIE_CMDFIFO_ADDR(2));
		//	printf("CMDFIFO 0:%x\n",au16_tmp[0]);
		//	printf("CMDFIFO 1:%x\n",au16_tmp[1]);
		//	printf("CMDFIFO 2:%x\n",au16_tmp[2]);

		if(au16_tmp[0] == (((u32Arg>>24)<<8) | (u8Cmd|0x40))&&
			au16_tmp[1] == ((u32Arg&0xFF00) | ((u32Arg>>16)&0xFF))&&
			au16_tmp[2] == (u32Arg&0xFF))
			break;
	}

	//clear command check for reading response value
	FCIE_RIU_16_OF(FCIE_SD_CTRL, BIT_CHK_CMD);

	//HalFcie_DumpRegisterBank(A_FCIE1_0_BANK, 0x40); // SDIO

#else

	HalFcieSdio_SetCIFC(_pu32_CIFCBuf, eIP, 0, (U16_T) (u8Cmd|0x40)        );
	HalFcieSdio_SetCIFC(_pu32_CIFCBuf, eIP, 1, (U16_T) ((u32Arg>>24)&0xFF) );
	HalFcieSdio_SetCIFC(_pu32_CIFCBuf, eIP, 2, (U16_T) ((u32Arg>>16)&0xFF) );
	HalFcieSdio_SetCIFC(_pu32_CIFCBuf, eIP, 3, (U16_T) ((u32Arg>> 8)&0xFF) );
	HalFcieSdio_SetCIFC(_pu32_CIFCBuf, eIP, 4, (U16_T) ((u32Arg    )&0xFF) );



	//FCIE_RIU_16_ON(A_SD_CTL_REG(eIP), BIT_CHK_CMD); // need to enable command check for CIFD
	//HalFcie_DumpRegisterBank(A_FCIE1_0_BANK, 0x40); // SDIO
	//HalFcie_DumpRegisterBank(A_FCIE2_0_BANK, 0x40); // FCIE
	//FCIE_RIU_16_OF(A_SD_CTL_REG(eIP), BIT_CHK_CMD);

#endif

}
/*
static U8_T _halFCIE_GetCIFC(U8_T u8addr)
{
    U32 u32Tmp;

    u32Tmp =FCIE_RIU_R16(FCIE_CMDFIFO_ADDR(u8addr>>1));

    if(u8addr&0x1)
    {
        return(u32Tmp>>8);
    }
    else
    {
        return(u32Tmp&0xFF);
    }

}
*/
/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_GetRspTokenFromCIFC
 *     @author jeremy.wang (2011/11/30)
 * Desc: Get Command Response Info.
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param eError :  Response Error Code
 *
 * @return RspStruct*  : Response Struct
 ----------------------------------------------------------------------------------------------------------*/
RspStruct* Hal_SDMMC_GetRspTokenFromCIFC(IPEmType eIP, RspErrEmType eError)
{
	U32_T *_pu32_CIFCBuf;// = (U32_T *)(CARD_BANK(A_SD_CIFC_BANK(eIP)));
	U8_T u8Pos;

	TR_SDMMC(prtstring("==>(Rsp) Error: "));
	TR_SDMMC(prtU32Hex((U32_T)eError));
	TR_SDMMC(prtstring("\r\n"));

	if(eIP==0) // SDIO
	{
		_pu32_CIFCBuf = (U32_T *)A_FCIE1_1_BANK;
	}
	else
	{
		_pu32_CIFCBuf = (U32_T *)A_FCIE2_1_BANK;
	}

	for(u8Pos=0; u8Pos<0x10; u8Pos++ )
		gst_RspStruct[eIP].u8ArrRspToken[u8Pos] = 0;

	TR_SDMMC(prtstring("["));

#ifdef PRINT_CMD_ARG_RSP
	printk("\tR: ");
#endif

	for(u8Pos=0; u8Pos < gst_RspStruct[eIP].u8RspSize; u8Pos++)
	{
		//gst_RspStruct[eIP].u8ArrRspToken[u8Pos] = _halFCIE_GetCIFC( u8Pos);	//need to rework
		gst_RspStruct[eIP].u8ArrRspToken[u8Pos] = HalFcieSdio_GetCIFC(_pu32_CIFCBuf, u8Pos);

		#ifdef PRINT_CMD_ARG_RSP
			printk("%02X ", gst_RspStruct[eIP].u8ArrRspToken[u8Pos]);
		#endif

		TR_SDMMC(prtU8Hex(gst_RspStruct[eIP].u8ArrRspToken[u8Pos]));
		#ifdef PRINT_CMD_ARG_RSP
			TR_SDMMC(prtstring(", "));
		#endif
	}

	#ifdef PRINT_CMD_ARG_RSP
	printk("\n");
	#endif

	TR_SDMMC(prtstring("]"));

	TR_SDMMC(prtstring("\r\n\r\n"));

	return &gst_RspStruct[eIP];
}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_SetDataWidth
 *     @author jeremy.wang (2011/11/28)
 * Desc: According as Data Bus Width to Set IP DataWidth
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param eBusWidth : 1BIT/4BITs/8BITs
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_SetDataWidth(IPEmType eIP, SDMMCBusWidthEmType eBusWidth)
{
	gu16_SD_MODE_DatLine[eIP] = (U16_T)eBusWidth;

}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_SetDataSync
 *     @author jeremy.wang (2011/11/28)
 * Desc: According as High or Default Speed to Set IP DataSync
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param bEnable : TRUE: Sync, FALSE: NoSync
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_SetDataSync(IPEmType eIP, BOOL_T bEnable)
{
	if(bEnable) // high speed 300K ~ 50MHz
	{
		//printk("set high speed timing\n");
   		//FCIE_RIU_16_ON(FCIE_DDR_MODE, BIT13|BIT14);
	}
	else // default speed 300K ~ 25MHz
	{
		//printk("set defo speed timing\n");
   		//FCIE_RIU_16_OF(FCIE_DDR_MODE, BIT13|BIT14);
	}

	return;
}




/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_SetNcrDelay
 *     @author jeremy.wang (2011/11/29)
 * Desc: According as Current Clock to Set Nrc Delay
 *
 * @param eIP :  FCIE1/FCIE2/...
 * @param u32RealClk : Real Clock
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_SetNcrDelay(IPEmType eIP, U32_T u32RealClk)
{

	if( u32RealClk >= 8000000 )			//>=8MHz
		gu16_WT_NAC[eIP] = 1;
	else if( u32RealClk >= 4000000 )	//>=4MHz
		gu16_WT_NAC[eIP] = 2;
	else if( u32RealClk >= 2000000 )	//>=2MHz
		gu16_WT_NAC[eIP] = 4;
	else if( u32RealClk >= 1000000 )	//>=1MHz
		gu16_WT_NAC[eIP] = 8;
	else if( u32RealClk >= 400000 )     //>=400KHz
		gu16_WT_NAC[eIP] = 20;
	else if( u32RealClk >= 300000 )	    //>=300KHz
		gu16_WT_NAC[eIP] = 27;
	else if( u32RealClk >= 100000 )     //>=100KHz
		gu16_WT_NAC[eIP] = 81;
	else if(u32RealClk==0)
		gu16_WT_NAC[eIP] = 0;            //NoNeed
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_TransCmdSetting
 *     @author jeremy.wang (2011/12/1)
 * Desc: For Data Transfer Setting
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param eTransType : CIFD/DMA/NONE
 * @param eCmdType : CMDRSP/READ/WRITIE
 * @param u16BlkCnt : Block Cnt
 * @param u16BlkSize : Block Size
 * @param u32Addr : Memory Address
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_TransCmdSetting(IPEmType eIP, TransEmType eTransType, CmdEmType eCmdType, U16_T u16BlkCnt, U16_T u16BlkSize, volatile U32_T u32Addr)
{
	unsigned int datasize = u16BlkCnt * u16BlkSize;

#if(!EN_BYTEMODE)
	#if (D_MIU_WIDTH==4)
	U8_T u8BitMask[8]={0x0F,0x0E,0x0C,0x08};
	#else
	U8_T u8BitMask[8]={0xFF,0xFE,0xFC,0xF8,0xF0,0xE0,0xC0,0x80};
	#endif
#endif

#ifdef FCIE_V5_ADMA
		u16BlkCnt=1;
	//	printk("blksize:%x\n",u16BlkSize);
		datasize=0x10;
#endif

	if(eTransType == EV_DMA)
	{
		//FCIE_RIU_R16(A_MMA_PRI_REG(eIP)) = V_MMA_PRI_INIT | (eCmdType>>6);

		/***** Block Count ******/
		FCIE_RIU_R16(A_JOB_BL_CNT_REG(eIP)) =  u16BlkCnt;

		/****** Block Size and DMA Addr******/
		FCIE_RIU_R16(A_SDIO_ADDR1_REG(eIP)) = (U16_T)(u32Addr>>16);
		FCIE_RIU_R16(A_SDIO_ADDR0_REG(eIP)) = (U16_T)(u32Addr);

		FCIE_RIU_R16(A_BLK_SIZE_REG(eIP)) = u16BlkSize ;

       	FCIE_RIU_R16(A_DMA_LEN_LO(eIP))=datasize & 0xffff;
       	FCIE_RIU_R16(A_DMA_LEN_HI(eIP))=(datasize >> 16);
	}

	/****** Set Waiting Time for Data Transfer *******/

#if (EN_DYNTIMEOUT)

	if(gu16_SD_MODE_DatLine[eIP] == EV_BUS_1BIT)
		u16BlkCnt = u16BlkCnt * 4;

	if(eCmdType == EV_CMDREAD)
	{
		gu16_WT_READ[eIP] = WT_RATIO_READ * u16BlkCnt;
		if( gu16_WT_READ[eIP] < 100) //100ms
			gu16_WT_READ[eIP] = 100;
	}
	else if(eCmdType == EV_CMDWRITE)
	{
		gu16_WT_WRITE[eIP] = WT_RATIO_WRITE * u16BlkCnt;
		if( gu16_WT_WRITE[eIP] < 250) //250ms
			gu16_WT_WRITE[eIP] = 250;
	}
#else

	if(eCmdType == EV_CMDREAD)
		gu16_WT_READ[eIP] = WT_EVENT_READ;
	else if(eCmdType == EV_CMDWRITE)
		gu16_WT_WRITE[eIP] = WT_EVENT_WRITE;
#endif

}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_SendCmdAndWaitProcess
 *     @author jeremy.wang (2011/11/28)
 * Desc: Send CMD and Wait Process
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param eTransType : CIFD/DMA/NONE
 * @param eCmdType : CMDRSP/READ/WRITE
 * @param eRspType : R1/R2/R3/...
 * @param bCloseClk : Close Clock or not
 *
 * @return RspErrEmType  : Response Error Code
 ----------------------------------------------------------------------------------------------------------*/
RspErrEmType Hal_SDMMC_SendCmdAndWaitProcess(IPEmType eIP, TransEmType eTransType, CmdEmType eCmdType, SDMMCRspEmType eRspType, BOOL_T bCloseClk, BOOL_T stop_clk ,BOOL_T bADMA)
{
	U32_T u32WaitMS	= 0;
	//U16_T u16MIE_PATH_MMA_EN = (U8_T)eTransType;
	U16_T u16VAR_SD_MODE = 0;// = gu16_SD_MODE_DatLine[eIP];// | gu16_SD_Mode_DatSync[eIP];
	//U16_T u16BlkCnt;
	//U16_T u16_reg;

	//printk("\33[1;31mCMD%02d_%08Xh\33[m\n", p_mmc_req->cmd->opcode, p_mmc_req->cmd->arg);
	//u16BlkCnt=FCIE_RIU_R16(A_JOB_BL_CNT_REG(eIP)) & 0xfff;
	//   if (stop_clk)
	//          FCIE_RIU_16_ON(FCIE_SD_MODE,BIT_SD_DMA_R_CLK_STOP) ;
	//else
	//     FCIE_RIU_16_OF(FCIE_SD_MODE,BIT_SD_DMA_R_CLK_STOP) ;

	switch (gu16_SD_MODE_DatLine[eIP])
	{
		case EV_BUS_1BIT:
			u16VAR_SD_MODE = BIT_SD_DATA_WIDTH_1;
			break;
		case EV_BUS_4BITS:
			u16VAR_SD_MODE = BIT_SD_DATA_WIDTH_4;
			break;
		case EV_BUS_8BITS:
			u16VAR_SD_MODE = BIT_SD_DATA_WIDTH_8;
			break;
	}

	FCIE_RIU_W16(A_SD_MODE_REG(eIP), BIT_CLK_EN|u16VAR_SD_MODE);

	if(stop_clk)
	{
		FCIE_RIU_16_ON(A_SD_MODE_REG(eIP), BIT_SD_DMA_R_CLK_STOP);
		FCIE_RIU_16_OF(A_SD_MODE_REG(eIP), BIT_DIS_BLOCK_CLK_STOP);
	}
	else
	{
		FCIE_RIU_16_OF(A_SD_MODE_REG(eIP), BIT_SD_DMA_R_CLK_STOP);
		FCIE_RIU_16_ON(A_SD_MODE_REG(eIP), BIT_DIS_BLOCK_CLK_STOP);
	}

	FCIE_RIU_16_OF(A_RSP_SIZE_REG(eIP), BIT_CMD_SIZE_MASK);
	FCIE_RIU_16_ON(A_RSP_SIZE_REG(eIP), (5)<< BIT_CMD_SIZE_SHIFT);

	FCIE_RIU_16_OF(A_RSP_SIZE_REG(eIP), BIT_RSP_SIZE_MASK);
	FCIE_RIU_16_ON(A_RSP_SIZE_REG(eIP), (eRspType) & BIT_RSP_SIZE_MASK);

	// setup reg_rsp_en & reg_rspr2_en | reg_dtrx_en
	FCIE_RIU_R16(A_SD_CTL_REG(eIP)) = (eRspType>>12);

	//FCIE_RIU_R16(A_CMD_SIZE_REG(eIP)) = V_CMD_SIZE_INIT;
	//	FCIE_RIU_R16(A_RSP_SIZE_REG(eIP)) = V_RSP_SIZE_INIT | ((U8_T)eRspType);
	//FCIE_RIU_R16(A_MIE_PATH_CTL_REG(eIP)) = V_MIE_PATH_INIT;
	//FCIE_RIU_R16(A_EMMC_PATH_CTL_REG(eIP)) = 0;
	//FCIE_RIU_R16(A_SD_CTL_REG(eIP))  = V_SD_CTL_INIT | (eRspType>>12) | (eCmdType>>4);

	Hal_Timer_uDelay(gu16_WT_NAC[eIP]);


	/*if ( _REG_WaitDat0HI(eIP, WT_DAT0HI_END) )
		return _SDMMC_ErrProcess(eIP, EV_STS_DAT0_BUSY, TRUE, __LINE__);*/

	// clear error flags & event bits
	if ( _REG_ClearSDSTS(eIP, RT_CLEAN_SDSTS) || _REG_ClearMIEEvent(eIP, RT_CLEAN_MIEEVENT) )
		return _SDMMC_ErrProcess(eIP, eCmdType, EV_STS_RIU_ERR, bCloseClk, __LINE__);


	if(eCmdType==EV_CMDREAD) // read, start command & dtrx at the same time
	{
		#if(EN_BIND_CARD_INT)
			Hal_CARD_INT_SetMIEIntEn(eIP, EV_INT_SD, BIT_DMA_END);
		#endif

		u32WaitMS = WT_EVENT_RSP + gu16_WT_READ[eIP];

		
		if (bADMA)
			FCIE_RIU_16_ON(A_SD_CTL_REG(eIP), BIT_SD_CMD_EN|BIT_SD_DTRX_EN|BIT_ADMA_EN|BIT_JOB_START);
		else
			FCIE_RIU_16_ON(A_SD_CTL_REG(eIP), BIT_SD_CMD_EN|BIT_SD_DTRX_EN|            BIT_JOB_START);
		

		if ( _REG_WaitMIEEvent(eIP, BIT_SD_CMD_END|BIT_DMA_END, u32WaitMS) )
			return _SDMMC_ErrProcess(eIP, eCmdType, EV_STS_MIE_TOUT, bCloseClk, __LINE__);
	}
	else // "simple command without data transfer" or "write start command first"
	{
		#if(EN_BIND_CARD_INT)
			Hal_CARD_INT_SetMIEIntEn(eIP, EV_INT_SD, BIT_SD_CMD_END);
		#endif

		u32WaitMS = WT_EVENT_RSP;
		FCIE_RIU_16_ON(A_SD_CTL_REG(eIP), BIT_SD_CMD_EN|BIT_JOB_START);

		if ( _REG_WaitMIEEvent(eIP, BIT_SD_CMD_END, u32WaitMS) )
			return _SDMMC_ErrProcess(eIP, eCmdType, EV_STS_MIE_TOUT, bCloseClk, __LINE__);
	}


	if(eRspType == EV_R3)  // For IP CRC bug
		FCIE_RIU_R16(A_SD_STS_REG(eIP)) = BIT_SD_RSP_CRC_ERR; //Clear CMD CRC Error
	else if ( (eRspType == EV_R1B) && _REG_WaitDat0HI(eIP, WT_DAT0HI_END) )
		return _SDMMC_ErrProcess(eIP, eCmdType, EV_STS_DAT0_BUSY, bCloseClk, __LINE__);

	#if 0
	u16_reg = FCIE_RIU_R16(FCIE_SD_STATUS);
	//printk("FCIE_SD_STATUS = %04X\n", u16_reg);
	if(u16_reg&BIT_SD_ERR_BITS) {
		printk("SD Err: FCIE_SD_STATUS = %04X\n", u16_reg);
		if(u16_reg&BIT_SD_R_CRC_ERR)	printk("SD Err: read CRC error\n");
		if(u16_reg&BIT_SD_W_CRC_ERR)	printk("SD Err: write CRC status error\n");
		if(u16_reg&BIT_SD_W_TIMEOUT)	printk("SD Err: transmitted data phase to error\n");
		if(u16_reg&BIT_SD_RSP_TIMEOUT)	printk("SD Err: command no response\n");
		if(u16_reg&BIT_SD_RSP_CRC_ERR)	printk("SD Err: response CRC error\n");
		if(u16_reg&BIT_SD_R_TIMEOUT)	printk("SD Err: received data phase to error\n");
	}
	#endif

	if( (eCmdType==EV_CMDWRITE) && (!M_REG_STSERR(eIP)) ) // write, start dtrx secondary
	{

		if ( _REG_ClearSDSTS(eIP, RT_CLEAN_SDSTS) || _REG_ClearMIEEvent(eIP, RT_CLEAN_MIEEVENT) )
			return _SDMMC_ErrProcess(eIP, eCmdType, EV_STS_RIU_ERR, bCloseClk, __LINE__);

		#if(EN_BIND_CARD_INT)
			Hal_CARD_INT_SetMIEIntEn(eIP, EV_INT_SD, BIT_DMA_END);
		#endif

		FCIE_RIU_16_OF(A_SD_CTL_REG(eIP), BIT_SD_CMD_EN|BIT_SD_RSP_EN);

		if (bADMA)
			FCIE_RIU_16_ON(A_SD_CTL_REG(eIP), BIT_JOB_START|BIT_ADMA_EN|BIT_SD_DAT_DIR_W|BIT_SD_DTRX_EN);
		else
			FCIE_RIU_16_ON(A_SD_CTL_REG(eIP), BIT_JOB_START|            BIT_SD_DAT_DIR_W|BIT_SD_DTRX_EN);

		if ( _REG_WaitMIEEvent(eIP, BIT_DMA_END, gu16_WT_WRITE[eIP]) )
			return _SDMMC_ErrProcess(eIP, eCmdType, EV_STS_MIE_TOUT, bCloseClk, __LINE__);

		//if ( (eTransType== EV_CIF) && _REG_WaitDat0HI(eIP, WT_DAT0HI_END) )
			//return _SDMMC_ErrProcess(eIP, eCmdType, EV_STS_DAT0_BUSY, bCloseClk, __LINE__);

	}

	return _SDMMC_ErrProcess(eIP, eCmdType, (RspErrEmType)M_REG_STSERR(eIP), bCloseClk, __LINE__);
}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_RunBrokenDmaAndWaitProcess
 *     @author jeremy.wang (2011/12/1)
 * Desc: For Broken DMA Data Transfer
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param eCmdType : READ/WRITE
 *
 * @return RspErrEmType  : Response Error Code
 ----------------------------------------------------------------------------------------------------------*/
RspErrEmType Hal_SDMMC_RunBrokenDmaAndWaitProcess(IPEmType eIP, CmdEmType eCmdType)
{
	U32_T u32WaitMS	= 0;

	if(eCmdType==EV_CMDREAD)
		u32WaitMS = gu16_WT_READ[eIP];
	else if(eCmdType==EV_CMDWRITE)
		u32WaitMS = gu16_WT_WRITE[eIP];

	if ( _REG_ClearMIEEvent(eIP, RT_CLEAN_MIEEVENT) )
		return _SDMMC_ErrProcess(eIP, eCmdType, EV_STS_RIU_ERR, FALSE, __LINE__);

	//if (_REG_WaitFIFOClkRdy(eIP, eCmdType, WT_FIFO_CLK_RDY))
		//return _SDMMC_ErrProcess(eIP, eCmdType, EV_STS_FIFO_NRDY, FALSE, __LINE__);

    if ( _REG_WaitDat0HI(eIP, WT_DAT0HI_END) )
		return _SDMMC_ErrProcess(eIP, eCmdType, EV_STS_DAT0_BUSY, TRUE, __LINE__);

	//FCIE_RIU_16_ON(A_MIE_PATH_CTL_REG(eIP), R_MMA_ENABLE);
	//FCIE_RIU_16_ON(A_SD_CTL_REG(eIP), R_SD_DTRX_EN);
	FCIE_RIU_16_ON(A_SD_CTL_REG(eIP),  BIT_SD_DTRX_EN |BIT_JOB_START);

	if ( _REG_WaitMIEEvent(eIP, BIT_DMA_END, u32WaitMS) )
		return _SDMMC_ErrProcess(eIP, eCmdType, EV_STS_MIE_TOUT, FALSE, __LINE__);

	return _SDMMC_ErrProcess(eIP, eCmdType, (RspErrEmType)M_REG_STSERR(eIP), FALSE, __LINE__);

}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_WriteProtect
 *     @author jeremy.wang (2011/12/1)
 * Desc: Check Card Write Protect
 *
 * @param eIP : FCIE1/FCIE2/...
 *
 * @return BOOL_T  : Write Protect or not
 ----------------------------------------------------------------------------------------------------------*/
BOOL_T Hal_SDMMC_WriteProtect(IPEmType eIP)
{
	// TRUE: write protected
	// FALSE: not write protected

	if(eIP==0) // SDIO
	{
		#if defined(CONFIG_MSTAR_SDMMC1_WP)
			return HalCard_GetWriteProtect(eIP);
		#else
			return FALSE;
		#endif
	}
	else // FCIE
	{
		#if defined(CONFIG_MSTAR_SDMMC2_WP)
			return HalCard_GetWriteProtect(eIP);
		#else
			return FALSE;
		#endif
	}
}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_ClkCtrl
 *     @author jeremy.wang (2011/12/14)
 * Desc: OpenCard Clk for Special Request (We always Open/Close Clk by Every Cmd)
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param bOpen : Open Clk or not
 * @param u16DelayMs :  Delay ms to Specail Purpose
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_ClkCtrl(IPEmType eIP, BOOL_T bOpen, U16_T u16DelayMs)
{
    if (GET_CARD_PORT(eIP)==EV_PORT_SDIO1)
	FCIE_RIU_R16(A_MIE_FUNC_CTL(eIP)) = BIT_SDIO_MOD;
    else if (GET_CARD_PORT(eIP)==EV_PORT_SD)
	FCIE_RIU_R16(A_MIE_FUNC_CTL(eIP)) = BIT_SD_EN;

	//FCIE_RIU_R16(A_SD_MODE_REG(eIP)) = _REG_GetPortSetting(eIP);

	if( bOpen )
		FCIE_RIU_16_ON(A_SD_MODE_REG(eIP), BIT_CLK_EN );
	else
		FCIE_RIU_16_OF(A_SD_MODE_REG(eIP), BIT_CLK_EN );

    Hal_Timer_mDelay(u16DelayMs);
}


//***********************************************************************************************************
// SD Other Setting
//***********************************************************************************************************

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_Reset
 *     @author jeremy.wang (2011/11/30)
 * Desc: Reset IP to avoid IP Dead + Touch CIFD first time to avoid latch issue
 *
 * @param eIP :  FCIE1/FCIE2/...
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_Reset(IPEmType eIP)
{
	_REG_ResetIP(eIP);
	//_REG_CIFDEmptyRW(eIP);
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_WaitProcessCtrl
 *     @author jeremy.wang (2011/12/1)
 * Desc: Stop Process to Avoid Long Time Waiting
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param bStop : Stop Process or not
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_WaitProcessCtrl(IPEmType eIP, BOOL_T bStop)
{
	gb_StopWProc[eIP] = bStop;

#if(EN_BIND_CARD_INT)
	if ( gb_StopWProc[eIP] )
		Hal_CARD_INT_WaitMIEEventCtrl(eIP, TRUE);
	else
		Hal_CARD_INT_WaitMIEEventCtrl(eIP, FALSE);
#endif

}

#if defined SDIO_D1_INTR_MODE && SDIO_D1_INTR_MODE

void HalSdio_SetIntMode(IPEmType eIP, unsigned char Mode) // software mode use only
{
	U16_T u16SdioMode;

    if(eIP == EV_IP_FCIE1) // SDIO
    {
		u16SdioMode = FCIE_RIU_R16(A_SDIO_MODE(eIP));
		u16SdioMode &= ~ BIT_REG_SDIO_MOD_MASK;
		if(Mode==2)
		{
			u16SdioMode |= SDIO_INT_MOD_2;
		}
		else if(Mode==3)
		{
			u16SdioMode |= SDIO_INT_MOD_3;
		}
		FCIE_RIU_W16(A_SDIO_MODE(eIP), u16SdioMode);
    }
	else
	{
		printk("FCIE not support D1 interrupt");
	}

}

void HalSdio_InterruptCtrl(IPEmType eIP, unsigned char Enable)
{
	static unsigned char u8EnableIntr = 0;

    if(eIP != EV_IP_FCIE1) // not SDIO
    {
		printk("FCIE not support D1 interrupt");
		return;
    }

	if(!u8EnableIntr) // Enable once
	{
		u8EnableIntr= 1;

		#if (defined SDIO_D1_INTR_MODE) && (SDIO_D1_INTR_MODE == SDIO_D1_INTR_MODE_SW) // software mode
			FCIE_RIU_16_ON(A_SDIO_MODE(eIP), BIT_SDIO_INT_SW_MODE|BIT_SDIO_DET_INT_SRC);
		#else
			FCIE_RIU_16_ON(A_SDIO_MODE(eIP), BIT_SDIO_DET_INT_SRC); // hardware mode
		#endif

		FCIE_RIU_16_ON(A_MIE_INT_EN_REG(eIP), BIT_SDIO_INT); // enable interrupt
	}

	if(Enable)
	{
		FCIE_RIU_16_ON(A_SDIO_INTR_DET(eIP), BIT_SDIO_DET_ON);
	}
	else
	{
		FCIE_RIU_16_OF(A_SDIO_INTR_DET(eIP), BIT_SDIO_DET_ON);
	}
}

#endif

