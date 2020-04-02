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
	FCIE_RIU_16_OF(CHIPTOP_08h, REG_SD_CONFIG_MSK|REG_NAND_CS1_EN|REG_NAND_MODE|REG_EMMC_CONFIG);

	switch(ulSD_CONFIG_NUM)
	{
		case 1:
			FCIE_RIU_16_ON(CHIPTOP_08h, REG_SD_MODE_1); // PAD_SDIO
			if((FCIE_RIU_R16(CHIPTOP_05h)&REG_I2S_OUT_MODE)==I2S_OUT_MODE_2)
			{
				printk(LIGHT_RED"SdErr: i2s_out_mode=2 conflict w/ sd_mode=1!\n"NONE);
			}
			if((FCIE_RIU_R16(CHIPTOP_0Ch)&REG_SM0_MODE)==SM0_MODE_2)
			{
				printk(LIGHT_RED"SdErr: sm0_mode=2 conflict w/ sd_mode=1!\n"NONE);
			}
			break;
		case 2:
			FCIE_RIU_16_ON(CHIPTOP_08h, REG_SD_MODE_2); // PAD_GT0_TX
			if((FCIE_RIU_R16(CHIPTOP_02h)&REG_TS0_MODE)==TS0_MODE_2)
			{
				printk(LIGHT_RED"SdErr: sm0_mode=2 conflict w/ sd_mode=2!\n"NONE);
			}
			if((FCIE_RIU_R16(CHIPTOP_02h)&REG_TS2_MODE)==TS2_MODE_1)
			{
				printk(LIGHT_RED"SdErr: ts2_mode=1 conflict w/ sd_mode=2!\n"NONE);
			}
			if((FCIE_RIU_R16(CHIPTOP_02h)&REG_MSPI1_MODE2)==MSPI1_MODE2_1)
			{
				printk(LIGHT_RED"SdErr: mspi1_mode2=1 conflict w/ sd_mode=2!\n"NONE);
			}
			break;
		case 3:
			FCIE_RIU_16_ON(CHIPTOP_08h, REG_SD_MODE_3); // PAD_GTO0_RX, ex: MST245A

			FCIE_RIU_16_ON(CHIPTOP_2Ch, REG_GT0_RX_PE); // pull enable
			FCIE_RIU_16_OF(CHIPTOP_2Dh, REG_GT0_RX_PS_CLK); // pull down sd clock
			FCIE_RIU_16_ON(CHIPTOP_2Dh, REG_GT0_RX_PS_SD_BUS); // pull up sd bus

			if((FCIE_RIU_R16(CHIPTOP_05h)&REG_I2S_OUT_MODE)==I2S_OUT_MODE_1)
			{
				printk(LIGHT_RED"SdErr: i2s_out_mode=1 conflict w/ sd_mode=3!\n"NONE);
			}
			if((FCIE_RIU_R16(CHIPTOP_0Ch)&REG_SM0_MODE)==SM0_MODE_1)
			{
				printk(LIGHT_RED"SdErr: sm0_mode=1 conflict w/ sd_mode=3!\n"NONE);
			}
			break;
		case 4:
			FCIE_RIU_16_ON(CHIPTOP_08h, REG_SD_MODE_4); // PAD_SD. ex: MST245B & MSO9380AM
			if((FCIE_RIU_R16(CHIPTOP_02h)&REG_MSPI1_MODE2)==MSPI1_MODE2_2)
			{
				printk(LIGHT_RED"SdErr: mspi1_mode=2 conflict w/ sd_mode=4!\n"NONE);
			}
			break;
		case 5:
			FCIE_RIU_16_ON(CHIPTOP_08h, REG_SD_MODE_5); // PAD_I2C & PAD_TS
			if((FCIE_RIU_R16(CHIPTOP_02h)&REG_TS0_MODE)==TS0_MODE_1)
			{
				printk(LIGHT_RED"SdErr: ts0_mode=1 conflict w/ sd_mode=5!\n"NONE);
			}
			if((FCIE_RIU_R16(CHIPTOP_02h)&REG_TS0_MODE)==TS0_MODE_2)
			{
				printk(LIGHT_RED"SdErr: ts0_mode=2 conflict w/ sd_mode=5!\n"NONE);
			}
			if((FCIE_RIU_R16(CHIPTOP_02h)&REG_TS1_MODE)==TS1_MODE_1)
			{
				printk(LIGHT_RED"SdErr: ts1_mode=1 conflict w/ sd_mode=5!\n"NONE);
			}
			if((FCIE_RIU_R16(CHIPTOP_02h)&REG_TS1_MODE)==TS1_MODE_2)
			{
				printk(LIGHT_RED"SdErr: ts1_mode=2 conflict w/ sd_mode=5!\n"NONE);
			}
			if((FCIE_RIU_R16(CHIPTOP_02h)&REG_MSPI1_MODE1)==MSPI1_MODE1_1)
			{
				printk(LIGHT_RED"SdErr: mspi1_mode1=1 conflict w/ sd_mode=5!\n"NONE);
			}
			if((FCIE_RIU_R16(CHIPTOP_02h)&REG_MSPI1_MODE1)==MSPI1_MODE1_2)
			{
				printk(LIGHT_RED"SdErr: mspi1_mode1=2 conflict w/ sd_mode=5!\n"NONE);
			}
			if((FCIE_RIU_R16(CHIPTOP_02h)&REG_MSPI1_MODE1)==MSPI1_MODE1_3)
			{
				printk(LIGHT_RED"SdErr: mspi1_mode1=3 conflict w/ sd_mode=5!\n"NONE);
			}
			if((FCIE_RIU_R16(CHIPTOP_05h)&REG_I2S_OUT_MODE)==I2S_OUT_MODE_2)
			{
				printk(LIGHT_RED"SdErr: i2s_out_mode=2 conflict w/ sd_mode=5!\n"NONE);
			}
			if((FCIE_RIU_R16(CHIPTOP_09h)&REG_I2CM0_MODE)==I2CM0_MODE_1)
			{
				printk(LIGHT_RED"SdErr: i2cm0_mode=1 conflict w/ sd_mode=5!\n"NONE);
			}
			if((FCIE_RIU_R16(CHIPTOP_09h)&REG_I2CM0_MODE)==I2CM0_MODE_2)
			{
				printk(LIGHT_RED"SdErr: i2cm0_mode=2 conflict w/ sd_mode=5!\n"NONE);
			}
			if((FCIE_RIU_R16(CHIPTOP_09h)&REG_I2CM1_MODE)==I2CM1_MODE_2)
			{
				printk(LIGHT_RED"SdErr: i2cm1_mode=2 conflict w/ sd_mode=5!\n"NONE);
			}
			if((FCIE_RIU_R16(CHIPTOP_09h)&REG_I2CM4_MODE)==I2CM4_MODE_1)
			{
				printk(LIGHT_RED"SdErr: i2cm4_mode=1 conflict w/ sd_mode=5!\n"NONE);
			}
			if((FCIE_RIU_R16(CHIPTOP_09h)&REG_FUART_MODE)==FUART_MODE_2)
			{
				printk(LIGHT_RED"SdErr: fuart_mode=2 conflict w/ sd_mode=5!\n"NONE);
			}
			if((FCIE_RIU_R16(CHIPTOP_03h)&REG_2ND_UART_MODE)==S2ND_UART_MODE_2)
			{
				printk(LIGHT_RED"SdErr: fuart_mode=2 conflict w/ sd_mode=5!\n"NONE);
			}
			if((FCIE_RIU_R16(CHIPTOP_03h)&REG_2ND_UART_MODE)==S2ND_UART_MODE_3)
			{
				printk(LIGHT_RED"SdErr: fuart_mode=3 conflict w/ sd_mode=5!\n"NONE);
			}
			break;
		default:
			printk("SdWarm: wrong parameter for sd_config %ld!\n", ulSD_CONFIG_NUM);
			break;
	}

}


#if !defined FPGA_BOARD || FPGA_BOARD==0

const U8 FcieClockIdxNum = 9;

const U32 FCIE_CLOCK[] =
{
    48000, //  0
    43200, //  1
    40000, //  2
    36000, //  3
    32000, //  4
    24000, //  5
    20000, //  6
    12000, //  7
     5400, //  8
      300, //  9
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

    FCIE_RIU_16_ON(REG_CLK_FCIE, BIT_FCIE_CLK_GATING); // turn on clock gating

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

    FCIE_RIU_16_ON(REG_CLK_FCIE, BIT_FCIE_CLK_SRC_SEL);
    FCIE_RIU_16_OF(REG_CLK_FCIE, BIT_CLKGEN_FCIE_MASK); // mask all clock select

	//printk("switch to clock: %d\n", u8ClockSlector);

    switch(u8ClockSlector)
    {
		case 0: // 48M
		    FCIE_RIU_16_ON(REG_CLK_FCIE, 0xF<<2);
			break;

		case 1: // 43.2M
		    FCIE_RIU_16_ON(REG_CLK_FCIE, 0x5<<2);
			break;

		case 2: // 40M
		    FCIE_RIU_16_ON(REG_CLK_FCIE, 0x4<<2);
			break;

		case 3: // 36M
		    FCIE_RIU_16_ON(REG_CLK_FCIE, 0x3<<2);
			break;

		case 4: // 32M
		    FCIE_RIU_16_ON(REG_CLK_FCIE, 0x2<<2);
			break;

		case 5: // 24M
		    FCIE_RIU_16_ON(REG_CLK_FCIE, 0xE<<2);
			break;

		case 6: // 20M
		    FCIE_RIU_16_ON(REG_CLK_FCIE, 0x1<<2);
			break;

		case 7: // 12M
		    FCIE_RIU_16_ON(REG_CLK_FCIE, 0x0<<2);
			break;

		case 8: // 5.4M
		    FCIE_RIU_16_ON(REG_CLK_FCIE, 0xA<<2);
			break;

		case 9: // 300K
		    FCIE_RIU_16_ON(REG_CLK_FCIE, 0xD<<2);
			break;

		default:
			printk("FCIE Err: wrong clock selector!\n");
			while(1);
			break;

	}

	DBG_CR_CLK(printk("REG_CLK_FCIE = 0x%04X\r\n", FCIE_RIU_R16(REG_CLK_FCIE)));

	// synthesizer clock
	if((FCIE_RIU_R16(REG_CLK_SYNTHESIZER)&0x000F)!=BIT_CLK_432MHz)
	{
		printk("FcieErr: REG_CLK_SYNTHESIZER = %04X, reconfig it!\n", FCIE_RIU_R16(REG_CLK_SYNTHESIZER));
		FCIE_RIU_16_OF(REG_CLK_SYNTHESIZER, (BIT_CLK_GATING|BIT_CLK_INVERSE|BIT_CLK_SOURCE));
		FCIE_RIU_16_ON(REG_CLK_SYNTHESIZER, BIT_CLK_432MHz);
	}

    FCIE_RIU_16_OF(REG_CLK_FCIE, BIT_FCIE_CLK_GATING); // turn off clock gating

    return FCIE_CLOCK[u8ClockSlector];

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

	// fcie
	REG_FCIE_CLRBIT(FCIE_DDR_MODE, BIT_MACRO_MODE_MASK);

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
			REG_FCIE_SETBIT(FCIE_DDR_MODE, BIT_FALL_LATCH|BIT_PAD_IN_SEL_SD|BIT_PAD_CLK_SEL);
			break;

		default:
			u8CurrentPadType = SDIO_MODE_UNKNOWN;
			printk(LIGHT_CYAN"SdErr: wrong parameter for switch fcie pad func\n"NONE);
			while(1);
			break;
	}
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

S32 HalFcie_GetCardDetect(void)
{
	// MST245A big SD socket & small t-flash socket use the same card detect pin
	#if 1
	FCIE_RIU_16_OF(REG_PM_SLEEP_0x35, REG_SPIHOLDN_MODE);
	FCIE_RIU_16_OF(REG_PM_SLEEP_0x28, PWM1_MODE);
	FCIE_RIU_16_OF(REG_PM_SLEEP_0x35, REG_SW_GPIO_PM5_PM1_2_HK51_UART|REG_SW_GPIO_PM5_PM1_2_DIR_UAR);

	FCIE_RIU_16_ON(PM_GPIO_REG_0x01, PAD_PM_GPIO1_OEN); // oen = 1 for input
	if (FCIE_RIU_R16(PM_GPIO_REG_0x01) & PAD_PM_GPIO1_IN)
	{
		//printk("card remove\n");
		return 0;
	}
	else
	{
		//printk("card insert\n");
		return 1; // insert low SD socket
	}
	#else
		return 1;
	#endif
}

// 0: not write protect
// 1: write protect
S32 HalFcie_GetWriteProtect(void)
{
	return 1;
}

void HalFcie_SetCardPower(U8 u8OnOff)
{
}



