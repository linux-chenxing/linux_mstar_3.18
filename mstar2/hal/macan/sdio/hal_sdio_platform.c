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
struct mmc_host;
#include "hal_sdio.h"

extern unsigned char sd_ddr_mode;
extern void HalSdio_DumpDebugBus(void);
extern unsigned char gu8SdioSilenceTuning;

/**************************************
* Config definition
**************************************/
#define DBG_CR_CLK(MSG)             //MSG
#define DBG_CR_PAD(MSG)             //MSG

/**************************************
* Local function definition
**************************************/
void HalSdioDelayUs(U32 u32Us);


/**************************************
* Global function definition
**************************************/
void HalSdio_ResetIP(void)
{
	U16 u16Reg, u16Cnt;

	REG_FCIE_W(FCIE_SD_CTRL, 0); // clear job start for safety

	//printk(LIGHT_CYAN"sdio reset\n"NONE);

	FCIE_RIU_W16(FCIE_MIE_FUNC_CTL, BIT_SDIO_MOD);

	REG_FCIE_CLRBIT(FCIE_RST, BIT_FCIE_SOFT_RST_n); /* active low */

	// SDIO reset - wait
	u16Cnt=0;

	do
	{
		REG_FCIE_R(FCIE_RST, u16Reg);

		HalSdioDelayUs(1);

	  	if(0x1000 == u16Cnt++)
		{
			printk("SD Err: SDIO Reset fail!! FCIE_RST = %04Xh\n", u16Reg);
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
			printk("SD Err: SDIO Reset fail2:h \n");
			return ;
		}

		HalSdioDelayUs(1);


	} while (0  != (u16Reg  & BIT_RST_STS_MASK));

	//printk("ok\n");

}

/*U32 HalSdio_TranslateVirtAddr(U32 u32_DMAAddr, U32 u32_ByteCnt)
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

void HalSdio_Platform_InitChiptop(void)
{
	REG_FCIE_CLRBIT(CHIPTOP_50h, REG_ALL_PAD_IN);
	REG_FCIE_CLRBIT(CHIPTOP_64h, REG_CIADCONFIG|REG_PCM2CTRLCONFIG|REG_PCMADCONFIG);
	REG_FCIE_CLRBIT(CHIPTOP_5Ah, BIT08|BIT09|BIT12|BIT13|BIT14);
	//REG_FCIE_CLRBIT(CHIPTOP_6Fh, BIT05|BIT06|BIT07);
	REG_FCIE_SETBIT(CHIPTOP_0Bh,BIT02|BIT03);
	REG_FCIE_SETBIT(CHIPTOP_0Ah,0xf00);

	// 215C up side socket use sdio mode 3
	REG_FCIE_CLRBIT(CHIPTOP_7Bh, REG_SDIO_CONFIG_MSK);
	REG_FCIE_SETBIT(CHIPTOP_7Bh, REG_SDIO_MODE_1);
}


#if !defined FPGA_BOARD || FPGA_BOARD==0

const U8 SdioClockIdxNum = 7;

const U32 SDIO_CLOCK[] =
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

U32 HalSdio_SetClock(U32 u32Clock)
{
    U8 u8ClockSlector;
	/*static U32 u32_OldClock=0xFFFFFFFF;

	//printk("HalSdio_SetClock(%ld)\n", u32Clock);

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

    for(u8ClockSlector=0; u8ClockSlector<=SdioClockIdxNum; u8ClockSlector++)
    {
        if( SDIO_CLOCK[u8ClockSlector] <= u32Clock )
		{
			break;
        }
    }

    if(u8ClockSlector>SdioClockIdxNum)
    {
        DBG_CR_CLK(printk("Error!!! Can not find proper clock!\r\n"));
		while(1);
        return 0x01;
    }

    if(u32Clock>1000)
    {
        DBG_CR_CLK(printk("select SDIO clock as %d.%d MHz\r\n", SDIO_CLOCK[u8ClockSlector]/1000, (SDIO_CLOCK[u8ClockSlector]%1000)/100));
    }
    else
    {
        DBG_CR_CLK(printk("select SDIO clock as %d KHz\r\n", SDIO_CLOCK[u8ClockSlector]));
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
			printk("SDIO Err: wrong clock selector!\n");
			while(1);
			break;

	}

	DBG_CR_CLK(printk("REG_CLK_SDIO = 0x%04X\r\n", FCIE_RIU_R16(REG_CLK_SDIO)));

    FCIE_RIU_16_OF(REG_CLK_SDIO, WBIT00); // turn off clock gating

    return SDIO_CLOCK[u8ClockSlector];

}

static U32 gu32SdClock = 0;

#define BUS_SPEED_SDR12		1
#define BUS_SPEED_SDR25		2
#define BUS_SPEED_DDR50		3
#define BUS_SPEED_SDR50		4
#define BUS_SPEED_SDR104	5

//static U32 gu8BusSpeed = BUS_SPEED_SDR12;

U32 HalSdio_clock_setting(U16 u16_ClkParam)
{
    DBG_CR_CLK(printk("HalSdio_clock_setting(%Xh)\n", u16_ClkParam));

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

        HalSdio_pll_setting(u16_ClkParam);

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

U8 HalSdio_SetSdioClock(U32 u32Clock)
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

U8 HalSdio_GetPadType(void)
{
	return u8CurrentPadType;
}

void HalSdio_SwitchPad(unsigned char u32Mode)
{
	//printk("switch pad %d, current pad type = %d\n", u32Mode, u8CurrentPadType);

	// chiptop
	HalSdio_Platform_InitChiptop();
	FCIE_RIU_W16(FCIE_MIE_FUNC_CTL, BIT_SDIO_MOD);

	// sdio
	REG_FCIE_CLRBIT(FCIE_DDR_MODE, BIT_FALL_LATCH|BIT_PAD_IN_SEL_SD|BIT_32BIT_MACRO_EN|BIT_DDR_EN|BIT_8BIT_MACRO_EN);


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
			REG_FCIE_SETBIT(FCIE_DDR_MODE, BIT_FALL_LATCH|BIT_PAD_IN_SEL_SD);
			// New patch for feedback clock reflex
			REG_FCIE_SETBIT(FCIE_DDR_MODE, BIT_FEEDBACK_CLK2);
			REG_FCIE_SETBIT(EMMC_PLL_1Ah, BIT_NEW_PATCH2);
			break;

		default:
			u8CurrentPadType = SDIO_MODE_UNKNOWN;
			DBG_CR_PAD(printk(LIGHT_CYAN"SdErr: wrong parameter for switch pad func\n"NONE));
			while(1);
			//return 0x01;
			break;
	}

	REG_FCIE_SETBIT(MIU2_7A,BIT10);		//MIU control

	//return 0x00;

//ErrorHandle:

	//printk("SDIO Err: set bus width before pad switch\n");
	//return 0x02;

}


U8 HalSdio_Platform_ClearEvent(U16 nReq)
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

#if defined(ENABLE_SDIO_INTERRUPT_MODE)&&ENABLE_SDIO_INTERRUPT_MODE

///////////////////////////////////////////////////////////////////////////////////////////////////

static DECLARE_WAIT_QUEUE_HEAD(sdio_mie_wait);

static volatile U16 fcie_event;

typedef enum
{
    IRQ_TYPE_NONE	= 0,
    IRQ_TYPE_EVENT	= 1,
    IRQ_TYPE_D1INT	= 2,

} E_IRQ_TYPE;


void HalSdio_ClearWaitQueue(void)
{
	fcie_event = 0;
}

E_IRQ_TYPE HalSdio_SaveMieEvent(struct mmc_host *host)
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


irqreturn_t HalSdio_KernelIrq(int irq, void *devid)
{
	irqreturn_t irq_t = IRQ_NONE;
	struct mmc_host *host = devid;
	E_IRQ_TYPE eIrqType;

	//printk("SDIO IRQ EV_%04Xh, IE_%04Xh\n", FCIE_RIU_R16(FCIE_MIE_EVENT), FCIE_RIU_R16(FCIE_MIE_INT_EN));

	eIrqType = HalSdio_SaveMieEvent(host);

	if(eIrqType ==IRQ_TYPE_EVENT)
	{
		wake_up(&sdio_mie_wait);
		irq_t = IRQ_HANDLED;
	}
	else if(eIrqType ==IRQ_TYPE_D1INT) // no need to wake up wait queue head
	{
		irq_t = IRQ_HANDLED;
	}

	return irq_t;
}


E_IO_STS HalSdio_WaitMieEvent(U16 u16ReqVal, U32 u32WaitMs)
{
	unsigned long timeout;

	timeout = msecs_to_jiffies(u32WaitMs+10);

	if(!wait_event_timeout(sdio_mie_wait, fcie_event==u16ReqVal, timeout))
	{
		printk("wait sdio mie evnet to req %04Xh, event = %04Xh\n", u16ReqVal, fcie_event);
		return IO_TIME_OUT;
	}

	return IO_SUCCESS;
}

#else

E_IO_STS HalSdio_WaitMieEvent(U16 u16ReqVal, U32 u32WaitMs)
{
    U32 u32Count = 0;
    //u32WaitMs *= 100;
	U16 u16Event;

	while(1)
	{
		u16Event = FCIE_RIU_R16(FCIE_MIE_EVENT);

		if(u16Event&BIT_ERR_STS)
		{
			/*if(!gu8SdioSilenceTuning)
			{
				printk("\33[1;31mSDIO ErrDet, SD_STS = %04Xh\33[m\n", FCIE_RIU_R16(FCIE_SD_STATUS));
			}*/
			HalSdio_Platform_ClearEvent(BIT_ERR_STS);
			return IO_ERROR_DETECT;
		}
		else if((u16Event&u16ReqVal)==u16ReqVal)
		{
			//printk("Got event %04X\n", u16Event);
			break;
		}

		HalSdioDelayUs(1000);

		u32Count++;

		if(u32Count>u32WaitMs)
		{
			printk("u32Counter=%d\r\n", u32Count);
			printk("\r\n");
			printk("------------------------------------------\r\n");
			printk("ERROR!!! MIE EVENT TIME OUT!!!\n");
			printk("request event = %04Xh, event = %04Xh\n", u16ReqVal, u16Event);
			printk("------------------------------------------\r\n");

			if(!gu8SdioSilenceTuning)
			{
				HalSdio_DumpRegister();
				HalSdio_DumpDebugBus();
				//while(1);
			}

			return(IO_TIME_OUT);
		}
	}

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Clear mie event
    // Only can clear request value,
    // because read operation might enable command and data transfer at the same time

    if (HalSdio_Platform_ClearEvent(u16ReqVal))
    {
	    return (IO_TIME_OUT);
    }

    return(IO_SUCCESS);
} // wait MIE Event End

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////

void HalSdio_SetVDD(U8 u8OnOff)
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

U32 HalSdio_FindSkewer(U32 u32_Candidate, SKEWER * st_skewer)
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

U32 HalSdio_SlectBestSkew4(U32 u32_Candidate, SKEWER * pSkewer)
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
	HalSdio_FindSkewer(u32_Candidate, &st_skewer1);

	u32_Candidate |= 0x00180001; // add 1 former and later
	HalSdio_FindSkewer(u32_Candidate, &st_skewer2);

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


void HalSdioDelayUs(U32 u32Us)
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

void HalSdioDelayMs(U32 u32Ms)
{
	U32 u32_i;

	for(u32_i=0; u32_i<u32Ms; u32_i++)
		udelay(1000);
}

#if 0

void HalSdio_TimerEnable(void)
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


U32 HalSdio_TimerGetTick(void)
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
U32 HalSdio_TimerGetMicroSec(void)
{
    return (HalSdio_TimerGetTick()/12);
}

void HalSdio_TimerTest(void)
{
	unsigned int sec = 0;

	HalSdio_TimerEnable();
	printk("count to 3 then start test: ");
	while(1)
	{
		if (HalSdio_TimerGetMicroSec() >= (1000000+sec*1000000))
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

void HalSdio_TimerStart(void)
{
	HalSdio_TimerEnable();
	tick_start = HalSdio_TimerGetTick();
}

U32 HalSdio_TimerStop(void)
{
	tick_stop = HalSdio_TimerGetTick();
	return ((tick_stop - tick_start) / 12);
}

#endif

S32 HalSdio_GetCardDetect(void)
{
	// 215C up side socket

	FCIE_RIU_16_ON(PMGPIO_REG_07, PMGPIO7_OEN); // oen = 1 for input

	if (FCIE_RIU_R16(PMGPIO_REG_07) & PMGPIO7_IN) // 079B & MST215C JP1 CDZ
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

// 0: not write protect
// 1: write protect
S32 HalSdio_GetWriteProtect(void)
{
	return 1;
}

void HalSdio_SetCardPower(U8 u8OnOff)
{
#if 0
	// 215C up side socket
	FCIE_RIU_16_ON(REG_PWM_PM_IS_GPIO, IS_GPIO); // config as GPIO
	FCIE_RIU_16_OF(PMGPIO_REG_14, PM_PWM_OEN); // oen = 0 for output
	if(u8OnOff)
	{
		FCIE_RIU_16_OF(PMGPIO_REG_14, PM_PWM_OUT); // power on
	}
	else
	{
		FCIE_RIU_16_ON(PMGPIO_REG_14, PM_PWM_OUT); // power off
	}
#endif	
}



