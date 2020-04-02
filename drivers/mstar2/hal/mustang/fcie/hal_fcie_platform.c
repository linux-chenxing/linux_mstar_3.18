///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2006 - 2011 MStar Semiconductor, Inc.
// This program is free software.
// You can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation;
// either version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program;
// if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

// this file is use only for manhattan project 2015.5.21

//#include <config.h>
//#include "drvFCIE_config.h"


//#include <common.h>
//#include <stdio.h>
//#include <mmc.h>
//#include "drvFCIE5.h"
#include <linux/string.h> // memcpy
#include <linux/delay.h> // udelay mdelay
#include <linux/sched.h> // DECLARE_WAIT_QUEUE_HEAD & wait_event_timeout() & TASK_NORMAL
#include <linux/semaphore.h> // semaphore, down(), up()

struct mmc_host;
#include "hal_fcie.h"

extern unsigned char sd_ddr_mode;
extern void HalFcie_DumpDebugBus(void);
extern unsigned char gu8FcieSilenceTuning;
extern unsigned long ulSD_CONFIG_NUM;

/**************************************
* Config definition
**************************************/
#define DBG_CR_CLK(MSG)             //MSG
#define DBG_CR_PAD(MSG)             //MSG

/**************************************
* Local function definition
**************************************/
void HalFcieDelayUs(U32 u32Us);


/**************************************
* Global function definition
**************************************/
void HalFcie_ResetIP(void)
{
	U16 u16Reg, u16Cnt;

	REG_FCIE_W(FCIE_SD_CTRL, 0); // clear job start for safety

	//printk(LIGHT_CYAN"fcie reset\n"NONE);

	FCIE_RIU_W16(FCIE_MIE_FUNC_CTL, BIT_SD_EN);

	REG_FCIE_CLRBIT(FCIE_RST, BIT_FCIE_SOFT_RST_n); /* active low */

	// FCIE reset - wait
	u16Cnt=0;

	do
	{
		REG_FCIE_R(FCIE_RST, u16Reg);

		HalFcieDelayUs(1);

	  	if(0x1000 == u16Cnt++)
		{
			printk("SD Err: FCIE Reset fail!! FCIE_RST = %04Xh\n", u16Reg);
			while(1);
		}

	} while (BIT_RST_STS_MASK  != (u16Reg  & BIT_RST_STS_MASK));

	REG_FCIE_SETBIT(FCIE_RST, BIT_FCIE_SOFT_RST_n);

	u16Cnt=0;

	do
	{
		REG_FCIE_R(FCIE_RST, u16Reg);
		//printk("FCIE_RST = %04Xh\n", u16Reg);

	  	if(0x1000 == u16Cnt++)
		{
			printk("SD Err: FCIE Reset fail2:h \n");
			return ;
		}

		HalFcieDelayUs(1);


	} while (0  != (u16Reg  & BIT_RST_STS_MASK));

	//printk("ok\n");

}

/*U32 HalFcie_TranslateVirtAddr(U32 u32_DMAAddr, U32 u32_ByteCnt)
{
	//flush_cache(u32_DMAAddr, u32_ByteCnt);
	#ifdef CONFIG_MCU_ARM

	//printk("MIU0[%Xh], MIU1[%Xh]\n", CONFIG_MIU0_BUSADDR, CONFIG_MIU1_BUSADDR);
	if(u32_DMAAddr < CONFIG_MIU1_BUSADDR) // MIU0
	{
		//REG_FCIE_CLRBIT(FCIE_MIU_DMA_26_16, BIT_MIU1_SELECT);
		return  (u32_DMAAddr - CONFIG_MIU0_BUSADDR);
	}
	else
	{
		// REG_FCIE_SETBIT(FCIE_MIU_DMA_26_16, BIT_MIU1_SELECT);
		printk("MIU1\n");
		return  (u32_DMAAddr - CONFIG_MIU1_BUSADDR);
	}
	#endif

	return 0;
}*/

void HalFcie_Platform_InitChiptop(void)
{
	FCIE_RIU_16_OF(CHIPTOP_50h, REG_ALL_PAD_IN);
	FCIE_RIU_16_OF(CHIPTOP_50h, BIT07|WBIT08|WBIT09);		//clear nand mode
	
	FCIE_RIU_16_OF(CHIPTOP_64h, BIT00);
	FCIE_RIU_16_OF(CHIPTOP_6Eh, BIT07|BIT06); // clear emmc_cmode
	FCIE_RIU_16_OF(CHIPTOP_4Fh, WBIT13|WBIT14|WBIT15); //

	// 215C up side socket use sd mode 3
	// 215C back side socket use sd mode 2
	FCIE_RIU_16_OF(CHIPTOP_57h, REG_SD_CONFIG_MSK);
	FCIE_RIU_16_OF(CHIPTOP_1Dh, WBIT09);

	switch(ulSD_CONFIG_NUM)
	{
		case 1:						//sd_config=1
			FCIE_RIU_16_ON(CHIPTOP_57h, REG_SD_MODE_1); // PAD
			FCIE_RIU_16_ON(CHIPTOP_0Bh,0x1E);				//bit 1-- bit4
			break;
		case 2:						//sd_config=2
			FCIE_RIU_16_ON(CHIPTOP_57h, REG_SD_MODE_2); // PAD_GT0_TX
			FCIE_RIU_16_ON(CHIPTOP_0Ch,0xf0000);			//bit 16-19
			break;
		case 3:						//sd_config=3
			FCIE_RIU_16_ON(CHIPTOP_57h, REG_SD_MODE_3); // PAD_GTO0_RX, ex: MST245A
			FCIE_RIU_16_OF(CHIPTOP_5Ah, BIT00);
			FCIE_RIU_16_OF(CHIPTOP_5Bh, WBIT08|WBIT09);
			FCIE_RIU_16_OF(CHIPTOP_1Dh, BIT00|BIT01);
			FCIE_RIU_16_ON(CHIPTOP_0Bh,0x1E);				//bit 1-- bit4

			break;
		case 4:				//sd_config2=1
			FCIE_RIU_16_ON(CHIPTOP_57h, REG_SD2_MODE_1); // PAD_SD. ex: MST245B & MSO9380AM
			FCIE_RIU_16_ON(CHIPTOP_0Bh,0x1E);				//bit 1-- bit4
	
			break;
		case 5:				//sd_config2=2
			FCIE_RIU_16_ON(CHIPTOP_57h, REG_SD2_MODE_2);  // PAD_I2C & PAD_TS	
			FCIE_RIU_16_ON(CHIPTOP_0Ch,0xf0000);			//bit 16-19
		
			break;
		case 6:						//sd_config_eco
			FCIE_RIU_16_ON(CHIPTOP_57h, REG_SD_MODE_2); // PAD_GT0_TX
			FCIE_RIU_16_ON(CHIPTOP_1Dh, WBIT09);
			FCIE_RIU_16_ON(CHIPTOP_0Ch,0xf0000);			//bit 16-19
		
			break;
		default:
			printk(LIGHT_RED"SdErr: wrong parameter for sd_config %ld!\n"NONE, ulSD_CONFIG_NUM);
			break;
	}
		// EMMC_PLL
	FCIE_RIU_16_OF(EMMC_PLL_68h, BIT00|BIT01);
	FCIE_RIU_16_OF(EMMC_PLL_6Dh, BIT00);
	FCIE_RIU_16_OF(EMMC_PLL_70h, BIT08);
	FCIE_RIU_W16(EMMC_PLL_71h, 0xFFFF);
	FCIE_RIU_W16(EMMC_PLL_73h, 0xFFFF);
	FCIE_RIU_16_ON(EMMC_PLL_74h, BIT15);

	// MIU select by FCIE
	FCIE_RIU_16_ON(MIU2_79, BIT04);

}


#if !defined FPGA_BOARD || FPGA_BOARD==0

const U8 FcieClockIdxNum = 7;

const U32 FCIE_CLOCK[] =
{
    48000, //  0
    43200, //  1
    40000, //  2
    36000, //  3
    32000, //  4
    20000, //  5
    12000, //  6
      300, //  7
};

// find clock close to target but not over

U32 HalFcie_SetClock(U32 u32Clock)
{
    U8 u8ClockSlector;
	/*static U32 u32_OldClock=0xFFFFFFFF;

	//printk("HalFcie_SetClock(%ld)\n", u32Clock);

	if(u32_OldClock == u32Clock)
		return 0;
	else
		u32_OldClock = u32Clock;*/

    FCIE_RIU_16_ON(REG_CLK_SDIO, WBIT00); // turn on clock gating

    if(u32Clock>1000)
    {
        DBG_CR_CLK(printk("Set SDIO clock as %d.%d MHz, ", u32Clock/1000, (u32Clock%1000)/100 ) );
    }
    else
    {
        DBG_CR_CLK(printk("Set SDIO clock as %d KHz, ", u32Clock));
    }

    for(u8ClockSlector=0; u8ClockSlector<=FcieClockIdxNum; u8ClockSlector++)
    {
        if( FCIE_CLOCK[u8ClockSlector] <= u32Clock )
		{
			break;
        }
    }

    if(u8ClockSlector>FcieClockIdxNum)
    {
        DBG_CR_CLK(printk("Error!!! Can not find proper clock!\r\n"));
		while(1);
        return 0x01;
    }

    if(u32Clock>1000)
    {
        DBG_CR_CLK(printk("select SDIO clock as %d.%d MHz\r\n", FCIE_CLOCK[u8ClockSlector]/1000, (FCIE_CLOCK[u8ClockSlector]%1000)/100));
    }
    else
    {
        DBG_CR_CLK(printk("select SDIO clock as %d KHz\r\n", FCIE_CLOCK[u8ClockSlector]));
    }

    FCIE_RIU_16_ON(REG_CLK_SDIO, WBIT06);
    FCIE_RIU_16_OF(REG_CLK_SDIO, WBIT05+WBIT04+WBIT03+WBIT02); // mask all clock select

	//printk("switch to clock: %d\n", u8ClockSlector);

    switch(u8ClockSlector)
    {
		case 0: // 48M
		    FCIE_RIU_16_ON(REG_CLK_SDIO, 0xF<<2);
			break;

		case 1: // 43.2M
		    FCIE_RIU_16_ON(REG_CLK_SDIO, 0x5<<2);
			break;

		case 2: // 40M
		    FCIE_RIU_16_ON(REG_CLK_SDIO, 0x4<<2);
			break;

		case 3: // 36M
		    FCIE_RIU_16_ON(REG_CLK_SDIO, 0x3<<2);
			break;

		case 4: // 32M
		    FCIE_RIU_16_ON(REG_CLK_SDIO, 0x2<<2);
			break;

		case 5: // 20M
		    FCIE_RIU_16_ON(REG_CLK_SDIO, 0x1<<2);
			break;

		case 6: // 12M
		    FCIE_RIU_16_ON(REG_CLK_SDIO, 0xE<<2);
			break;

		case 7: // 300K
		    FCIE_RIU_16_ON(REG_CLK_SDIO, 0xD<<2);
			break;

		default:
			printk("FCIE Err: wrong clock selector!\n");
			while(1);
			break;

	}

	DBG_CR_CLK(printk("REG_CLK_SDIO = 0x%04X\r\n", FCIE_RIU_R16(REG_CLK_SDIO)));

    FCIE_RIU_16_OF(REG_CLK_SDIO, WBIT00); // turn off clock gating

    return FCIE_CLOCK[u8ClockSlector];

}

static U32 gu32SdClock = 0;

#define BUS_SPEED_SDR12		1
#define BUS_SPEED_SDR25		2
#define BUS_SPEED_DDR50		3
#define BUS_SPEED_SDR50		4
#define BUS_SPEED_SDR104	5

//static U32 gu8BusSpeed = BUS_SPEED_SDR12;

U32 HalFcie_clock_setting(U16 u16_ClkParam)
{
    DBG_CR_CLK(printk("HalFcie_clock_setting(%Xh)\n", u16_ClkParam));

    REG_FCIE_CLRBIT(FCIE_SD_MODE, BIT_CLK_EN);

    switch(u16_ClkParam)
    {
		// emmc_pll clock
		case eMMC_PLL_CLK__20M	: gu32SdClock =  20000; break;
		case eMMC_PLL_CLK__27M	: gu32SdClock =  27000; break;
		case eMMC_PLL_CLK__32M	: gu32SdClock =  32000; break;
		case eMMC_PLL_CLK__36M	: gu32SdClock =  36000; break;
		case eMMC_PLL_CLK__40M	: gu32SdClock =  40000; break;
		case eMMC_PLL_CLK__48M	: gu32SdClock =  48000; break;
		case eMMC_PLL_CLK__52M	: gu32SdClock =  52000; break;
		case eMMC_PLL_CLK__62M	: gu32SdClock =  62000; break;
		case eMMC_PLL_CLK__72M	: gu32SdClock =  72000; break;
		case eMMC_PLL_CLK__80M	: gu32SdClock =  80000; break;
		case eMMC_PLL_CLK__86M	: gu32SdClock =  86000; break;
		case eMMC_PLL_CLK_100M	: gu32SdClock = 100000; break;
		case eMMC_PLL_CLK_120M	: gu32SdClock = 120000; break;
		case eMMC_PLL_CLK_140M	: gu32SdClock = 140000; break;
		case eMMC_PLL_CLK_160M	: gu32SdClock = 160000; break;
		case eMMC_PLL_CLK_200M	: gu32SdClock = 200000; break;

		// clock_gen sdio clock
        //case BIT_FCIE_CLK_12M	: gu32SdClock =  12000; break;
		case BIT_FCIE_CLK_20M	: gu32SdClock =  20000; break;
		case BIT_FCIE_CLK_32M	: gu32SdClock =  32000; break;
		case BIT_FCIE_CLK_36M	: gu32SdClock =  36000; break;
		case BIT_FCIE_CLK_40M	: gu32SdClock =  40000; break;
		case BIT_FCIE_CLK_43M	: gu32SdClock =  43200; break;
		case BIT_FCIE_CLK_300K	: gu32SdClock =    300; break;
        //case BIT_FCIE_CLK_24M	: gu32SdClock =  24000; break;
		case BIT_FCIE_CLK_48M	: gu32SdClock =  48000; break;

        default:
            printk("SdErr: wrong clock parameter\n");
            return 1;;
    }

	//printk("set clock to %d KHz\n", gu32SdClock);

    /*if(u16_ClkParam & eMMC_PLL_FLAG)
    {
		//printk("sdio pll clock\n");

        REG_FCIE_CLRBIT(REG_CLK_SDIO, BIT_SDIO_CLK_GATING|BIT_SDIO_CLK_INVERSE|BIT_CLKGEN_SDIO_MASK);

        HalFcie_pll_setting(u16_ClkParam);

        //if( gu8BusSpeed == BUS_SPEED_SDR104 ) // HS200
        {
            REG_FCIE_SETBIT(REG_CLK_SDIO, BIT_SDIO_CLK_SRC_SEL|0xB<<2);
        }
    }
    else*/
    {
		printk("clkgen clock\n");

        REG_FCIE_CLRBIT(REG_CLK_SDIO, BIT_SDIO_CLK_GATING|BIT_SDIO_CLK_INVERSE|BIT_CLKGEN_SDIO_MASK);
        REG_FCIE_SETBIT(REG_CLK_SDIO, BIT_SDIO_CLK_SRC_SEL|u16_ClkParam<<2);
    }


    return 0;
}

#else

U8 HalFcie_SetSdioClock(U32 u32Clock)
{
	if (u32Clock < 400)
	{
		printk("SDIO FPGA clock 187.5KHz\n");
		REG_FCIE_SETBIT(R_SDIO_PLL_0x1D, BIT0);
	}
	else
	{
		printk("SDIO FPGA clock 1.5MHz\n");
		REG_FCIE_CLRBIT(R_SDIO_PLL_0x1D, BIT0);
	}
	REG_FCIE_SETBIT(FCIE_SD_MODE, BIT_CLK_EN); // enable clk

	return 0;
}

#endif

static U8 u8CurrentPadType = SDIO_MODE_UNKNOWN;

U8 HalFcie_GetPadType(void)
{
	return u8CurrentPadType;
}

void HalFcie_SwitchPad(unsigned char u32Mode)
{
	//printk("switch pad %d, current pad type = %d\n", u32Mode, u8CurrentPadType);

	// chiptop
	HalFcie_Platform_InitChiptop();

	REG_FCIE_CLRBIT(FCIE_NC_FUNC_CTL,BIT00);		//clear nand en
	// sdio
	REG_FCIE_CLRBIT(FCIE_DDR_MODE, BIT_FALL_LATCH|BIT_PAD_IN_SEL_SD|BIT_32BIT_MACRO_EN|BIT_DDR_EN|BIT_8BIT_MACRO_EN);

	REG_FCIE_SETBIT(FCIE_DDR_MODE,BIT01|BIT02|BIT03|WBIT10|WBIT13|WBIT14);		//enable pad-in-rdy-sel, pre-full-sel

	switch(u32Mode)
	{
		///////////////////////////////////////////////////////////////////////////////////////////
		case SDIO_MODE_GPIO_PAD_BPS:
			DBG_CR_PAD(printk(LIGHT_CYAN"SDIO_MODE_GPIO_PAD_BPS\n"NONE));
			u8CurrentPadType = SDIO_MODE_GPIO_PAD_BPS;
			break;

		///////////////////////////////////////////////////////////////////////////////////////////
		case SDIO_MODE_GPIO_PAD_SDR:
			DBG_CR_PAD(printk(LIGHT_CYAN"SDIO_MODE_GPIO_PAD_SDR\n"NONE));
			u8CurrentPadType = SDIO_MODE_GPIO_PAD_SDR;
			//REG_FCIE_SETBIT(FCIE_DDR_MODE, BIT_FALL_LATCH|BIT_PAD_IN_SEL_SD);
			// New patch for feedback clock reflex
			//REG_FCIE_SETBIT(FCIE_DDR_MODE, BIT_FEEDBACK_CLK2);
			//REG_FCIE_SETBIT(EMMC_PLL_1Ah, BIT_NEW_PATCH2);
			break;

		default:
			u8CurrentPadType = SDIO_MODE_UNKNOWN;
			DBG_CR_PAD(printk(LIGHT_CYAN"SdErr: wrong parameter for switch pad func\n"NONE));
			while(1);
			//return 0x01;
			break;
	}


	//return 0x00;

//ErrorHandle:

	//printk("SDIO Err: set bus width before pad switch\n");
	//return 0x02;

}


U8 HalFcie_Platform_ClearEvent(U16 nReq)
{
    U16  u16Tmp;

    u16Tmp = 0x0080;
    while((FCIE_RIU_R16(FCIE_MIE_EVENT)& nReq) != 0x00)
    {
        FCIE_RIU_W16(FCIE_MIE_EVENT, nReq); // write 1 clear register
        if (u16Tmp==0)
        {
            printk("Error!!! Can not clear MIE event.\r\n");
            return(1);
        }
        else
        {
            u16Tmp--;
        }
    }

    return 0;
}

#if defined(ENABLE_FCIE_INTERRUPT_MODE)&&ENABLE_FCIE_INTERRUPT_MODE

///////////////////////////////////////////////////////////////////////////////////////////////////

static DECLARE_WAIT_QUEUE_HEAD(fcie_mie_wait);

static volatile U16 fcie_event;

typedef enum
{
    IRQ_TYPE_NONE	= 0,
    IRQ_TYPE_EVENT	= 1,
    IRQ_TYPE_D1INT	= 2,

} E_IRQ_TYPE;


void HalFcie_ClearWaitQueue(void)
{
	fcie_event = 0;
}

E_IRQ_TYPE HalFcie_SaveMieEvent(struct mmc_host *host)
{

	//U16 u16Reg = FCIE_RIU_R16(FCIE_MIE_EVENT);

#if defined SDIO_D1_INTR_MODE && SDIO_D1_INTR_MODE

	if(u16Reg & BIT_SDIO_INT)
	{
 		mmc_signal_sdio_irq(host);
		FCIE_RIU_W16(FCIE_MIE_EVENT, BIT_SDIO_INT); // W1C event
		return IRQ_TYPE_D1INT;
	}
	else

#endif

	{
		//fcie_event |= u16Reg; // summary all mie event

		fcie_event = FCIE_RIU_R16(FCIE_MIE_EVENT) & FCIE_RIU_R16(FCIE_MIE_INT_EN);

		if(fcie_event & BIT_DMA_END)
	    {
	        FCIE_RIU_16_OF(FCIE_MIE_INT_EN, BIT_DMA_END);
	    }

		if(fcie_event & BIT_SD_CMD_END)
		{
			FCIE_RIU_16_OF(FCIE_MIE_INT_EN, BIT_SD_CMD_END);
		}

		if(fcie_event & BIT_BUSY_END_INT)
		{
			FCIE_RIU_16_OF(FCIE_MIE_INT_EN, BIT_BUSY_END_INT);
		}

		return IRQ_TYPE_EVENT;
	}

	return IRQ_TYPE_NONE;

}


irqreturn_t HalFcie_KernelIrq(int irq, void *devid)
{
	irqreturn_t irq_t = IRQ_NONE;
	struct mmc_host *host = devid;
	E_IRQ_TYPE eIrqType;

	//printk("FCIE IRQ EV_%04Xh, IE_%04Xh\n", FCIE_RIU_R16(FCIE_MIE_EVENT), FCIE_RIU_R16(FCIE_MIE_INT_EN));

	if(FCIE_RIU_R16(FCIE_MIE_FUNC_CTL) & BIT_EMMC_ACTIVE)
	{
		return IRQ_NONE;
	}

	eIrqType = HalFcie_SaveMieEvent(host);

	if(eIrqType ==IRQ_TYPE_EVENT)
	{
		wake_up(&fcie_mie_wait);
		irq_t = IRQ_HANDLED;
	}
	else if(eIrqType ==IRQ_TYPE_D1INT) // no need to wake up wait queue head
	{
		irq_t = IRQ_HANDLED;
	}

	return irq_t;
}

E_IO_STS HalFcie_WaitMieEventPolling(U16 u16ReqVal, U32 u32WaitMs)
{
    U32 u32Count = 0;
    //u32WaitMs *= 100;
	U16 u16Event;

	while(1)
	{
		u16Event = FCIE_RIU_R16(FCIE_MIE_EVENT);

		if(u16Event&BIT_ERR_STS)
		{
			/*if(!gu8FcieSilenceTuning)
			{
				printk("\33[1;31mSDIO ErrDet, SD_STS = %04Xh\33[m\n", FCIE_RIU_R16(FCIE_SD_STATUS));
			}*/
			HalFcie_Platform_ClearEvent(BIT_ERR_STS);
			return IO_ERROR_DETECT;
		}
		else if((u16Event&u16ReqVal)==u16ReqVal)
		{
			//printk("Got event %04X\n", u16Event);
			break;
		}

		HalFcieDelayUs(1000);

		u32Count++;

		if(u32Count>u32WaitMs)
		{
			printk("\r\n");
			printk("------------------------------------------\r\n");
			printk("ERROR!!! MIE EVENT TIME OUT!!!\n");
			printk("request event = %04Xh, event = %04Xh\n", u16ReqVal, u16Event);
			printk("------------------------------------------\r\n");

			if(!gu8FcieSilenceTuning)
			{
				HalFcie_DumpRegister();
				HalFcie_DumpDebugBus();
				//while(1);
			}

			return(IO_TIME_OUT);
		}
	}

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Clear mie event
    // Only can clear request value,
    // because read operation might enable command and data transfer at the same time

    if (HalFcie_Platform_ClearEvent(u16ReqVal))
    {
	    return (IO_TIME_OUT);
    }

    return(IO_SUCCESS);
}


E_IO_STS HalFcie_WaitMieEvent(U16 u16ReqVal, U32 u32WaitMs)
{
	unsigned long timeout;

	timeout = msecs_to_jiffies(u32WaitMs+10);

	if(wait_event_timeout(fcie_mie_wait, fcie_event==u16ReqVal, timeout))
	{
		return IO_SUCCESS;
	}
	else
	{
		if(HalFcie_WaitMieEventPolling(u16ReqVal, 2)) // try polling event
		{
			printk("wait fcie mie evnet timeout req %04Xh, event = %04Xh\n", u16ReqVal, fcie_event);
			return IO_TIME_OUT;
		}
		else
		{
			printk("wait mie intr timeout but polling evnet %04Xh OK\n", u16ReqVal);
			return IO_SUCCESS;
		}
	}
}

#else

E_IO_STS HalFcie_WaitMieEvent(U16 u16ReqVal, U32 u32WaitMs)
{
    U32 u32Count = 0;
    //u32WaitMs *= 100;
	U16 u16Event;

	while(1)
	{
		u16Event = FCIE_RIU_R16(FCIE_MIE_EVENT);

		if(u16Event&BIT_ERR_STS)
		{
			/*if(!gu8FcieSilenceTuning)
			{
				printk("\33[1;31mSDIO ErrDet, SD_STS = %04Xh\33[m\n", FCIE_RIU_R16(FCIE_SD_STATUS));
			}*/
			HalFcie_Platform_ClearEvent(BIT_ERR_STS);
			return IO_ERROR_DETECT;
		}
		else if((u16Event&u16ReqVal)==u16ReqVal)
		{
			//printk("Got event %04X\n", u16Event);
			break;
		}

		HalFcieDelayUs(1000);

		u32Count++;

		if(u32Count>u32WaitMs)
		{
			printk("\r\n");
			printk("------------------------------------------\r\n");
			printk("ERROR!!! MIE EVENT TIME OUT!!!\n");
			printk("request event = %04Xh, event = %04Xh\n", u16ReqVal, u16Event);
			printk("------------------------------------------\r\n");

			if(!gu8FcieSilenceTuning)
			{
				HalFcie_DumpRegister();
				HalFcie_DumpDebugBus();
				//while(1);
			}

			return(IO_TIME_OUT);
		}
	}

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Clear mie event
    // Only can clear request value,
    // because read operation might enable command and data transfer at the same time

    if (HalFcie_Platform_ClearEvent(u16ReqVal))
    {
	    return (IO_TIME_OUT);
    }

    return(IO_SUCCESS);
}

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////

void HalFcie_SetVDD(U8 u8OnOff)
{
	// MST232A
	// rework Q14 for always have power
	// But David suggest use TGPIO1 0x47 to control power
	FCIE_RIU_16_OF(CHIP_GPIO_47, TGPIO1_OEN); // set oen = 0
	if(u8OnOff)
	{
		FCIE_RIU_16_OF(CHIP_GPIO_47, TGPIO1_OUT); // low to turn on power
	}
	else
	{
		FCIE_RIU_16_ON(CHIP_GPIO_47, TGPIO1_OUT); // high to turn off power
	}
}

U32 HalFcie_FindSkewer(U32 u32_Candidate, SKEWER * st_skewer)
{
	U32 u32_i;
	st_skewer->u32LatchOKStart = 0xBEEF;
	st_skewer->u32LatchOKEnd = 0xBEEF;
	st_skewer->u8_edge = 0;

	//printk("%s(%05Xh)\n", __FUNCTION__, u32_Candidate);

	for(u32_i=0; u32_i<20; u32_i++) {
		if( (u32_Candidate&(1<<u32_i)) ^ ((u32_Candidate>>1)&(1<<u32_i)) ) { // use XOR to find 0b01/0b10 edge
			st_skewer->u8_edge++;
			//printk("%d [%04Xh ^ %04Xh] = %04Xh, ", u32_i, (u32_Candidate&(1<<u32_i)), ((u32_Candidate>>1)&(1<<u32_i)),
			//	( (u32_Candidate&(1<<u32_i)) ^ ((u32_Candidate>>1)&(1<<u32_i)) ) );
			if(st_skewer->u32LatchOKStart==0xBEEF) {
				st_skewer->u32LatchOKStart = u32_i;
			} else if(st_skewer->u32LatchOKEnd==0xBEEF) {
				st_skewer->u32LatchOKEnd = u32_i;
			}
		}
	}

	st_skewer->u32LatchOKEnd--;

	//eMMC_printk("u8_edge = %d, ", st_skewer->u8_edge);
	//eMMC_printk("start = %d, ", st_skewer->u32LatchOKStart);
	//eMMC_printk("end = %d\n", st_skewer->u32LatchOKEnd);

	return 0;
}

U32 HalFcie_SlectBestSkew4(U32 u32_Candidate, SKEWER * pSkewer)
{
	SKEWER st_skewer1/* = {0}*/;
	SKEWER st_skewer2/* = {0}*/;
	U32 u32_LatchBestSelect = 0;
	U32 u32_Temp;
	U32 u32Ret = 0;

	//printk("\n%s(%05Xh)\n", __FUNCTION__, u32_Candidate);

	if(!u32_Candidate) {
		printk("ATOP fail: no workable skew4\n");
		u32Ret = 1;
		goto ErrorHandle;
	}

	u32_Candidate<<=1; // bit 0~17 --> bit 1~18, add 0 former and later
	HalFcie_FindSkewer(u32_Candidate, &st_skewer1);

	u32_Candidate |= 0x00180001; // add 1 former and later
	HalFcie_FindSkewer(u32_Candidate, &st_skewer2);

	u32_Candidate &= ~0x00180001; // bit 1~18
	u32_Candidate>>=1; // bit 1~18 --> bit 0~17, shift back

	if( (st_skewer1.u8_edge==2) || (st_skewer2.u8_edge==2) ) { // only 1 continuous workable skew4 group

		if(st_skewer1.u8_edge==2) {
			// select best skew4
			if((st_skewer1.u32LatchOKStart + st_skewer1.u32LatchOKEnd)%2) { // odd

				u32_LatchBestSelect = (st_skewer1.u32LatchOKStart + st_skewer1.u32LatchOKEnd) >> 1;
				//u32_LatchBestSelect++; // select later
			}
			else { // even

				u32_LatchBestSelect = (st_skewer1.u32LatchOKStart + st_skewer1.u32LatchOKEnd) >> 1;
			}
		}
		else {
			memcpy(&st_skewer1, &st_skewer2, sizeof(SKEWER));
			//eMMC_printk("ring type:\n");
			st_skewer1.u32LatchOKStart--;
			st_skewer1.u32LatchOKEnd++;

			//eMMC_printk("start = %d, ", st_skewer1.u32LatchOKStart);
			//eMMC_printk("end = %d\n", st_skewer1.u32LatchOKEnd);

			u32_Temp = ((st_skewer1.u32LatchOKStart+1)+(18-st_skewer1.u32LatchOKEnd)) >> 1;
			if(st_skewer1.u32LatchOKStart>=u32_Temp) {
				//printk("enough\n");
				u32_LatchBestSelect = st_skewer1.u32LatchOKStart - u32_Temp;
			}
			else {
				u32_LatchBestSelect = st_skewer1.u32LatchOKEnd + u32_Temp;
			}
			if(u32_LatchBestSelect==18) u32_LatchBestSelect = 0;
			// swap start & end
			u32_Temp = st_skewer1.u32LatchOKStart; st_skewer1.u32LatchOKStart = st_skewer1.u32LatchOKEnd; st_skewer1.u32LatchOKEnd = u32_Temp;
		}

		if( st_skewer1.u32LatchOKStart == st_skewer1.u32LatchOKEnd ) {
			printk("ATOP warn: %05Xh (", u32_Candidate);
			printk("%d ~ ", st_skewer1.u32LatchOKStart);
			printk("%d)\n", st_skewer1.u32LatchOKEnd);
			u32Ret = 2;
			goto ErrorHandle;
		}
		else {
			printk("ATOP success: %05Xh (", u32_Candidate);
			printk("%d ~ ", st_skewer1.u32LatchOKStart);
			printk("%d)", st_skewer1.u32LatchOKEnd);
		}
	}
	else {
		printk("ATOP fail: not continuous: %05Xh, u8_edge: ", u32_Candidate);
		printk("%d (", st_skewer1.u8_edge);
		printk("%d ~ ", st_skewer1.u32LatchOKStart);
		printk("%d)\n", st_skewer1.u32LatchOKEnd);
		u32Ret = 3;
		goto ErrorHandle;
	}

	printk(" --> %d\n", u32_LatchBestSelect);

	pSkewer->u32LatchOKStart = st_skewer1.u32LatchOKStart;
	pSkewer->u32LatchOKEnd = st_skewer1.u32LatchOKEnd;
	pSkewer->u32LatchBest = u32_LatchBestSelect;

ErrorHandle:

	return u32Ret;
}

#if 0
void FCIE_HWTimer_Start(void)
{
    // reset HW timer
    FCIE_RIU_W16(TIMER0_MAX_LOW, 0xFFFF);
    FCIE_RIU_W16(TIMER0_MAX_HIGH, 0xFFFF);
    FCIE_RIU_W16(TIMER0_ENABLE, 0);

    // start HW timer
    FCIE_RIU_16_ON(TIMER0_ENABLE, 0x0001);

}

U32 FCIE_HWTimer_End(void)
{
    U32 u32HWTimer = 0;
    U16 u16TimerLow = 0;
    U16 u16TimerHigh = 0;

    // Get HW timer
    u16TimerLow = FCIE_RIU_R16(TIMER0_CAP_LOW);
    u16TimerHigh = FCIE_RIU_R16(TIMER0_CAP_HIGH);

    u32HWTimer = (u16TimerHigh<<16) | u16TimerLow;

    return u32HWTimer;
}
#endif

extern void MsOS_DelayTaskUs_Poll(U32 u32Us); // MsOS.h
extern void MsOS_DelayTaskUs(U32 u32Us);


void HalFcieDelayUs(U32 u32Us)
{
	U32 u32_i, u32Ms;

	if(u32Us<1000)
	{
		udelay(u32Us);
	}
	else
	{
		u32Ms = u32Us/1000;
		for(u32_i=0; u32_i<u32Ms; u32_i++)
			udelay(1000);
	}
}

void HalFcieDelayMs(U32 u32Ms)
{
	U32 u32_i;

	for(u32_i=0; u32_i<u32Ms; u32_i++)
		udelay(1000);
}

#if 0

void HalFcie_TimerEnable(void)
{
    // reset HW timer
    FCIE_RIU_W16(TIMER1_MAX_LOW, 0xFFFF);
    FCIE_RIU_W16(TIMER1_MAX_HIGH, 0xFFFF);
    FCIE_RIU_W16(TIMER1_ENABLE, 0);

    // start HW timer
    FCIE_RIU_16_ON(TIMER1_ENABLE, 0x0001);

	// 0xFFFFFFFF = 4,294,967,295 tick
	// divide 12 --> 357,913,941 us --> 357 sec --> 6 min
}


U32 HalFcie_TimerGetTick(void)
{
    U32 u32HWTimer = 0;
    U16 u16TimerLow = 0;
    U16 u16TimerHigh = 0;

    // Get HW timer
    u16TimerLow = FCIE_RIU_R16(TIMER1_CAP_LOW);
    u16TimerHigh = FCIE_RIU_R16(TIMER1_CAP_HIGH);

    u32HWTimer = (u16TimerHigh<<16) | u16TimerLow;

    return u32HWTimer;
}

// max: 357,913,941 = 0x15555555
U32 HalFcie_TimerGetMicroSec(void)
{
    return (HalFcie_TimerGetTick()/12);
}

void HalFcie_TimerTest(void)
{
	unsigned int sec = 0;

	HalFcie_TimerEnable();
	printk("count to 3 then start test: ");
	while(1)
	{
		if (HalFcie_TimerGetMicroSec() >= (1000000+sec*1000000))
		{
			printk("%d ", ++sec);
		}
		if(sec==3)
		{
			printk("Go!\n");
			break;
		}
	}
}

static unsigned int tick_start;
static unsigned int tick_stop;

void HalFcie_TimerStart(void)
{
	HalFcie_TimerEnable();
	tick_start = HalFcie_TimerGetTick();
}

U32 HalFcie_TimerStop(void)
{
	tick_stop = HalFcie_TimerGetTick();
	return ((tick_stop - tick_start) / 12);
}

#endif
extern struct semaphore	PfModeSem;

S32 HalFcie_GetCardDetect(void)
{

	int CDZ_option;
	int CdzGpioBank;
	int CdzGpioOffset;
	int CdzGpioBits;

	if ((FCIE_RIU_R16(FCIE_ZDEC_CTL0) & 0xff00)==SBOOT_SIGNATURE)
	{
		CDZ_option=(FCIE_RIU_R16(FCIE_TEST_MODE)  >> 8) & 0x3 ;
		if (CDZ_option==3)
			return 1; 		//disable CDZ, always true
		else if (CDZ_option==1)			//use sboot setting
		{
    down(&PfModeSem);
			REG_FCIE_W(FCIE_SD_CTRL,0x80);			//enable register access
			CdzGpioBank=FCIE_RIU_R16(CDZ_GPIO_BANK);
			CdzGpioOffset=FCIE_RIU_R16(CDZ_GPIO_OFFSET);
			REG_FCIE_W(FCIE_SD_CTRL,0x00);			//disable register access
    up(&PfModeSem);

			//printk("Offset:%x\n",CdzGpioOffset);
			CdzGpioBits=CdzGpioOffset &0xff;
			CdzGpioOffset=CdzGpioOffset >> 8;
			//printk("cdz bank:%x offset:%x bit:%x\n",CdzGpioBank,CdzGpioOffset,CdzGpioBits);
	//printk(LIGHT_CYAN"CDZ addr:%x\n",((RIU_BANK_2_BASE(CdzGpioBank)+(CdzGpioOffset << 1))));

			
			if (FCIE_RIU_R16((RIU_BANK_2_BASE(CdzGpioBank)+(CdzGpioOffset << 1))) & CdzGpioBits)
				return 0;
			else
				return 1;
		}
	}
	else
	{
		printk("Cannot get valid CDZ GPIO setting from sboot\n");
	}

	return 0;

}

// 0: not write protect
// 1: write protect
S32 HalFcie_GetWriteProtect(void)
{
	int wp_option;
	int wpGpioBank;
	int wpGpioOffset;
	int wpGpioBits;


	if ((FCIE_RIU_R16(FCIE_ZDEC_CTL0) & 0xff00)==SBOOT_SIGNATURE)
	{
		wp_option=(FCIE_RIU_R16(FCIE_TEST_MODE)  >> 12) & 0x3 ;
		//printk("wp_option:%x\n",wp_option);
		
		if (wp_option==3)
			return 1; 		//disable WP, always true
		else if (wp_option==1)			//use sboot setting
		{
			REG_FCIE_W(FCIE_SD_CTRL,0x80);			//enable register access
			wpGpioBank=FCIE_RIU_R16(WP_GPIO_BANK);
			wpGpioBits=FCIE_RIU_R16(WP_GPIO_BITS);
			wpGpioOffset=FCIE_RIU_R16(FCIE_BOOT_CONFIG);
			//printk("wp Offset:%x\n",wpGpioOffset);
	
			REG_FCIE_W(FCIE_SD_CTRL,0x00);			//disable register access
			wpGpioOffset=wpGpioOffset >> 8;

	//		printk("wp Offset:%x\n",wpGpioOffset);
	//		printk("wp bank:%x\n",wpGpioBank);
	//		printk("wp bits:%x\n",wpGpioBits);
	//printk(LIGHT_CYAN"WP addr:%lx\n",((RIU_BANK_2_BASE(wpGpioBank)+(wpGpioOffset << 1))));
	//printk(LIGHT_CYAN"WP:%lx\n",FCIE_RIU_R16((RIU_BANK_2_BASE(wpGpioBank)+(wpGpioOffset << 1))));

			if (FCIE_RIU_R16((RIU_BANK_2_BASE(wpGpioBank)+(wpGpioOffset << 1))) & wpGpioBits)
				return 0;
			else
				return 1;

		}

	}

	return 1;
}

void HalFcie_SetCardPower(U8 u8OnOff)
{
	int pwr_option;
	int pwrGpioBank;
	int pwrGpioOffset;
	int pwrGpioBits;

	if ((FCIE_RIU_R16(FCIE_ZDEC_CTL0) & 0xff00)==SBOOT_SIGNATURE)
	{
		pwr_option=(FCIE_RIU_R16(FCIE_TEST_MODE)  >> 10) & 0x3 ;
	//	printk("pwr_option:%x\n",pwr_option);
		if (pwr_option==3)
			return; 		//disable WP, always true
		else if (pwr_option==1)			//use sboot setting
		{
			REG_FCIE_W(FCIE_SD_CTRL,0x80);			//enable register access
			pwrGpioBank=FCIE_RIU_R16(PWR_GPIO_BANK);
			pwrGpioOffset=FCIE_RIU_R16(PWR_GPIO_OFFSET);
			REG_FCIE_W(FCIE_SD_CTRL,0x00);			//disable register access
			pwrGpioBits=pwrGpioOffset &0xff;
			pwrGpioOffset=pwrGpioOffset >> 8;

			//printk("pwr bank:%x\n",pwrGpioBank);
			//printk("pwr Offset:%x\n",pwrGpioOffset);
			//printk("pwr bits:%x\n",pwrGpioBits);
			//printk("OnOff:%x\n",u8OnOff);
			if (u8OnOff)
				FCIE_RIU_16_OF(RIU_BANK_2_BASE(pwrGpioBank)+(pwrGpioOffset << 1), pwrGpioBits);
			else
				FCIE_RIU_16_ON(RIU_BANK_2_BASE(pwrGpioBank)+(pwrGpioOffset << 1), pwrGpioBits);
	

		}
	}	
	else
	{
		printk("Cannot get power control GPIO from sboot\n");
	}
}



