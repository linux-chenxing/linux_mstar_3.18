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

// this file is use only for curry project 2015.12.14

//#include <config.h>
//#include "drvFCIE_config.h"


//#include <common.h>
//#include <stdio.h>
//#include <mmc.h>
//#include "drvFCIE5.h"
#include <linux/string.h> // memcpy
#include <linux/delay.h> // udelay mdelay
#include <linux/sched.h> // DECLARE_WAIT_QUEUE_HEAD & wait_event_timeout() & TASK_NORMAL
#include <linux/mmc/host.h> // mmc_signal_sdio_irq()
struct mmc_host;
#include "hal_sdio.h"

extern unsigned char sd_ddr_mode;
extern void HalSdio_DumpDebugBus(void);
extern unsigned char gu8SdioSilenceTuning;
extern unsigned long ulSDIO_CONFIG_NUM;

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

	REG_FCIE_CLRBIT(CHIPTOP_08h, REG_SDIO_CONFIG_MSK);
	switch(ulSDIO_CONFIG_NUM)
	{
		case 1:
			FCIE_RIU_16_ON(CHIPTOP_08h, REG_SDIO_MODE_1); // PAD_SDIO, ex: MST245A & MST245B
			break;
		default:
			printk(LIGHT_RED"SdErr: wrong parameter for sdio_config %ld!\n"NONE, ulSDIO_CONFIG_NUM);
			break;
	}

	HalSdio_SetIOVoltage(VOLT_33V);

	HalSdio_SetDrivingStrength(2);

	// WiFi module CON5 pin: 17 WIFI_RST
	// output high to enable WiFi module
	#if 0
		FCIE_RIU_16_OF(PM_SAR_0x11, PM_SAR_GPIO2_OEN);
		FCIE_RIU_16_ON(PM_SAR_0x12, PM_SAR_GPIO2_OUT);
	#endif
}


static U8 u8CurrentPadType = SDIO_MODE_UNKNOWN;

U8 HalSdio_GetPadType(void)
{
	return u8CurrentPadType;
}

void HalSdio_SwitchPad(unsigned char u32Mode)
{
	//printk("switch pad to %d, current pad type = %d\n", u32Mode, u8CurrentPadType);

	// chiptop
	//HalSdio_Platform_InitChiptop(); // eMMC & nand driver should NOT touch sdio related chiptop regs.

	// sdio
	REG_FCIE_CLRBIT(FCIE_DDR_MODE, BIT_MACRO_MODE_MASK);

	// sdio_pll
	FCIE_RIU_16_OF(R_SDIO_PLL_0x1A, BIT00|BIT03|BIT04|BIT10);
	FCIE_RIU_16_ON(R_SDIO_PLL_0x1C, BIT_32BIF_RX_ECO_EN|BIT_1X_SYNC_ECO_EN);
	FCIE_RIU_16_OF(R_SDIO_PLL_0x68, BIT_EMMC_EN);
	FCIE_RIU_16_OF(R_SDIO_PLL_0x69, (BIT_TUNE_SHOT_OFFSET_MSK|BIT_CLK_DIG_INV));
	FCIE_RIU_16_OF(R_SDIO_PLL_0x6D, BIT_DDR_IO_MODE);
	FCIE_RIU_16_OF(R_SDIO_PLL_0x70, (BIT_SEL_FLASH_32BIF|BIT_RX_AFIFO_EN|BIT_RSP_AFIFO_EN));
	FCIE_RIU_16_ON(R_SDIO_PLL_0x71, BIT_TX_BPS_EN);
	FCIE_RIU_16_ON(R_SDIO_PLL_0x73, BIT_RX_BPS_EN);
	FCIE_RIU_16_ON(R_SDIO_PLL_0x74, BIT_ATOP_BYP_RX_EN);

	switch(u32Mode)
	{
		case SDIO_MODE_GPIO_PAD_BPS:
			DBG_CR_PAD(printk(LIGHT_CYAN"SDIO_MODE_GPIO_PAD_BPS\n"NONE));
			u8CurrentPadType = SDIO_MODE_GPIO_PAD_BPS;
			break;

		case SDIO_MODE_GPIO_PAD_SDR:
			DBG_CR_PAD(printk(LIGHT_CYAN"SDIO_MODE_GPIO_PAD_SDR\n"NONE));
			u8CurrentPadType = SDIO_MODE_GPIO_PAD_SDR;
			// sdio
			REG_FCIE_SETBIT(FCIE_DDR_MODE, BIT_FALL_LATCH|BIT_PAD_IN_SEL|BIT_PAD_CLK_SEL);

			//sdio_pll
			FCIE_RIU_16_ON(R_SDIO_PLL_0x1A, BIT10); // C2 clock for sdr macro
			break;

		////////////////////////////////////////////////////////////////////////////////////////////
		case SDIO_MODE_8BITS_MACRO_SDR:
			DBG_CR_PAD(printk(LIGHT_CYAN"SDIO_MODE_8BITS_MACRO_SDR\n"NONE));
			u8CurrentPadType = SDIO_MODE_8BITS_MACRO_SDR;
			// sdio
			REG_FCIE_SETBIT(FCIE_DDR_MODE, BIT_8BIT_MACRO_EN);
			// sdio_pll
			FCIE_RIU_16_ON(R_SDIO_PLL_0x1A, BIT10); // C2 clock for sdr macro
			FCIE_RIU_16_ON(R_SDIO_PLL_0x68, BIT_EMMC_EN);

			FCIE_RIU_W16(R_IO_BUS_WIDTH, BIT_4_BIT);
			FCIE_RIU_W16(R_SDIO_PLL_0x6B, 0x0413);

			break;

		case SDIO_MODE_8BITS_MACRO_DDR:
			u8CurrentPadType = SDIO_MODE_8BITS_MACRO_DDR;
			DBG_CR_PAD(printk(LIGHT_CYAN"FCIE_MODE_8BITS_MACRO_DDR52\n"NONE));
			// sdio
			REG_FCIE_SETBIT(FCIE_DDR_MODE, BIT_DDR_EN|BIT_8BIT_MACRO_EN);

			// sdio_pll
			FCIE_RIU_16_ON(R_SDIO_PLL_0x68, BIT_EMMC_EN);
			FCIE_RIU_W16(R_IO_BUS_WIDTH, BIT_4_BIT);
			FCIE_RIU_W16(R_SDIO_PLL_0x6B, 0x0213);
			FCIE_RIU_16_ON(R_SDIO_PLL_0x6D, BIT_DDR_IO_MODE);
			break;

		////////////////////////////////////////////////////////////////////////////////////////////
		case SDIO_MODE_32BITS_MACRO_SDR104:
			u8CurrentPadType = SDIO_MODE_32BITS_MACRO_SDR104;
			DBG_CR_PAD(printk(LIGHT_CYAN"SDIO_MODE_32BITS_MACRO_SDR104\n"NONE));
			// sdio
			REG_FCIE_SETBIT(FCIE_DDR_MODE, BIT_32BIT_MACRO_EN);
			REG_FCIE_W(FCIE_DDR_TOGGLE_CNT, 0x0410);
			// emmc_pll
			HalSdio_SetSkew(1, 0);
			HalSdio_SetSkew(2, 0);
			HalSdio_SetSkew(3, 0);
			HalSdio_SetSkew(4, 0);
			HalSdio_SetTriggerLevel(0);

			FCIE_RIU_16_ON(R_SDIO_PLL_0x1A, BIT00); // driving
			FCIE_RIU_16_ON(R_SDIO_PLL_0x1A, BIT03|BIT04); // SDIO GATE_RX_W_OEN for sdio bus
			FCIE_RIU_16_ON(R_SDIO_PLL_0x68, BIT_EMMC_EN);
			FCIE_RIU_16_ON(R_SDIO_PLL_0x69, 4<<4);
			FCIE_RIU_16_ON(R_SDIO_PLL_0x70, BIT_SEL_FLASH_32BIF);
			FCIE_RIU_16_OF(R_SDIO_PLL_0x71, BIT_TX_BPS_EN);
			break;

		////////////////////////////////////////////////////////////////////////////////////////////
		case SDIO_MODE_32BITS_MACRO_AFIFO:
			u8CurrentPadType = SDIO_MODE_32BITS_MACRO_AFIFO;
			DBG_CR_PAD(printk(LIGHT_CYAN"SDIO_MODE_32BITS_MACRO_AFIFO\n"NONE));
			// sdio
			REG_FCIE_SETBIT(FCIE_DDR_MODE, BIT_32BIT_MACRO_EN);
			REG_FCIE_W(FCIE_DDR_TOGGLE_CNT, 0x0410);
			// emmc_pll
			HalSdio_SetSkew(1, 0);
			HalSdio_SetSkew(2, 0);
			HalSdio_SetSkew(3, 0);
			HalSdio_SetSkew(4, 0);
			HalSdio_SetTriggerLevel(0);
			FCIE_RIU_16_ON(R_SDIO_PLL_0x1A, BIT00); // driving
			FCIE_RIU_16_ON(R_SDIO_PLL_0x1A, BIT03|BIT04); // SDIO GATE_RX_W_OEN for sdio bus
			FCIE_RIU_16_ON(R_SDIO_PLL_0x68, BIT_EMMC_EN);
			FCIE_RIU_16_ON(R_SDIO_PLL_0x69, 4<<4);
			FCIE_RIU_16_ON(R_SDIO_PLL_0x70, (BIT_SEL_FLASH_32BIF|BIT_RX_AFIFO_EN|BIT_RSP_AFIFO_EN));
			FCIE_RIU_16_OF(R_SDIO_PLL_0x71, BIT_TX_BPS_EN);
			break;

		default:
			u8CurrentPadType = SDIO_MODE_UNKNOWN;
			DBG_CR_PAD(printk(LIGHT_CYAN"SDIO Err: wrong parameter for switch pad func\n"NONE));
			//return 0x01;
			break;
	}

	// 8 bits macro reset + 32 bits macro reset
	REG_FCIE_CLRBIT(R_SDIO_PLL_0x6F, BIT_DQS_CNT_RSTN|BIT_MACRO_SW_RSTZ);
	HalSdioDelayMs(1);
	REG_FCIE_SETBIT(R_SDIO_PLL_0x6F, BIT_DQS_CNT_RSTN|BIT_MACRO_SW_RSTZ);
	HalSdioDelayMs(1);

}

//

#if !defined FPGA_BOARD || FPGA_BOARD==0

const U8 SdioClockIdxNum = 9;

const U32 SDIO_CLOCK[] =
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

U32 HalSdio_SetClkgenClock(U32 u32Clock)
{
    U8 u8ClockSlector;
	/*static U32 u32_OldClock=0xFFFFFFFF;

	//printk("HalSdio_SetClkgenClock(%ld)\n", u32Clock);

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
        printk("Error!!! Can not find proper clock!\r\n");
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

		case 5: // 24M
		    FCIE_RIU_16_ON(REG_CLK_SDIO, 0xE<<2);
			break;

		case 6: // 20M
		    FCIE_RIU_16_ON(REG_CLK_SDIO, 0x1<<2);
			break;

		case 7: // 12M
		    FCIE_RIU_16_ON(REG_CLK_SDIO, 0x0<<2);
			break;

		case 8: // 5.4M
		    FCIE_RIU_16_ON(REG_CLK_SDIO, 0xA<<2);
			break;

		case 9: // 300K
		    FCIE_RIU_16_ON(REG_CLK_SDIO, 0xD<<2);
			break;

		default:
			printk("SDIO Err: wrong clock selector!\n");
			while(1);
			break;

	}

	DBG_CR_CLK(printk("REG_CLK_SDIO = 0x%04X\r\n", FCIE_RIU_R16(REG_CLK_SDIO)));

	// synthesizer clock
	if((FCIE_RIU_R16(REG_CLK_SYNTHESIZER)&0x000F)!=BIT_CLK_432MHz)
	{
		printk("FcieErr: REG_CLK_SYNTHESIZER = %04X, reconfig it!\n", FCIE_RIU_R16(REG_CLK_SYNTHESIZER));
		FCIE_RIU_16_OF(REG_CLK_SYNTHESIZER, (BIT_CLK_GATING|BIT_CLK_INVERSE|BIT_CLK_SOURCE));
		FCIE_RIU_16_ON(REG_CLK_SYNTHESIZER, BIT_CLK_432MHz);
	}

    FCIE_RIU_16_OF(REG_CLK_SDIO, WBIT00); // turn off clock gating

    return SDIO_CLOCK[u8ClockSlector];

}

const U8 SdioPllClockIdxNum = 16;

const U32 SDIO_PLL_CLOCK[] =
{
   200000, //  0
   180000, //  1
   160000, //  2
   140000, //  3
   120000, //  4
   100000, //  5
    86000, //  6
    80000, //  7
    72000, //  8
    62000, //  9
    52000, // 10
    48000, // 11
    40000, // 12
    36000, // 13
    32000, // 14
    27000, // 15
    20000, // 16
};

U32 HalSdio_SetPllClock(U32 u32Clock)
{
    U8 u8PllClockSlector;
	U16 u16_value_reg_emmc_pll_pdiv;
	U16 u16_value_reg_emmc_pll_test;

	DBG_CR_CLK(printk("Set SDIO PLL clock as %d.%d MHz, ", u32Clock/1000, (u32Clock%1000)/100 ) );

    for(u8PllClockSlector=0; u8PllClockSlector<=SdioPllClockIdxNum; u8PllClockSlector++)
    {
        if( SDIO_PLL_CLOCK[u8PllClockSlector] <= u32Clock )
		{
			break;
        }
    }

	//printk(YELLOW"u8PllClockSlector = %d, "NONE, u8PllClockSlector);

    if(u8PllClockSlector>SdioPllClockIdxNum)
    {
        printk("Error!!! Can not find proper PLL clock! %d\r\n", u8PllClockSlector);
		while(1);
        return 0x01;
    }

	DBG_CR_CLK(printk("select SDIO PLL clock as %d.%d MHz\r\n", SDIO_PLL_CLOCK[u8PllClockSlector]/1000, (SDIO_PLL_CLOCK[u8PllClockSlector]%1000)/100));

	// 1. reset emmc pll
	FCIE_RIU_16_ON(reg_emmc_pll_reset, BIT0);
	FCIE_RIU_16_OF(reg_emmc_pll_reset, BIT0);

	// 2. synth clock
	switch(u8PllClockSlector)
	{
	    case 0: // 200M
			FCIE_RIU_W16(reg_ddfset_23_16, 0x0022);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x8F5C);
			// 195M
			//FCIE_RIU_W16(reg_ddfset_23_16, 0x0024);
			//FCIE_RIU_W16(reg_ddfset_15_00, 0x03D8);
			u16_value_reg_emmc_pll_pdiv = 1;// PostDIV: 2
			u16_value_reg_emmc_pll_test = 0;
			break;
		case 1: // 180M
			FCIE_RIU_W16(reg_ddfset_23_16, 0x0026);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x6666);
			u16_value_reg_emmc_pll_pdiv = 1;// PostDIV: 2
			u16_value_reg_emmc_pll_test = 0;
			break;
		case 2: // 160M
			FCIE_RIU_W16(reg_ddfset_23_16, 0x002B);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x3333);
			u16_value_reg_emmc_pll_pdiv = 1;// PostDIV: 2
			u16_value_reg_emmc_pll_test = 0;
			break;
		case 3: // 140M
			FCIE_RIU_W16(reg_ddfset_23_16, 0x0031);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x5F15);
			u16_value_reg_emmc_pll_pdiv = 1;// PostDIV: 2
			u16_value_reg_emmc_pll_test = 0;
			break;
		case 4: // 120M
			FCIE_RIU_W16(reg_ddfset_23_16, 0x0039);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x9999);
			u16_value_reg_emmc_pll_pdiv = 1;// PostDIV: 2
			u16_value_reg_emmc_pll_test = 0;
			break;
		case 5: // 100M
			FCIE_RIU_W16(reg_ddfset_23_16, 0x0045);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x1EB8);
			u16_value_reg_emmc_pll_pdiv = 1;// PostDIV: 2
			u16_value_reg_emmc_pll_test = 0;
			break;
		case 6: // 86M
			FCIE_RIU_W16(reg_ddfset_23_16, 0x0028);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x2FA0);
			u16_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
			u16_value_reg_emmc_pll_test = 0;
			break;
		case 7: // 80M
			FCIE_RIU_W16(reg_ddfset_23_16, 0x002B);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x3333);
			u16_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
			u16_value_reg_emmc_pll_test = 0;
			break;
		case 8: // 72M
			FCIE_RIU_W16(reg_ddfset_23_16, 0x0030);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x0000);
			u16_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
			u16_value_reg_emmc_pll_test = 0;
			break;
		case 9: // 62M
			FCIE_RIU_W16(reg_ddfset_23_16, 0x0037);
			FCIE_RIU_W16(reg_ddfset_15_00, 0xBDEF);
			u16_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
			u16_value_reg_emmc_pll_test = 0;
			break;
		case 10: // 52M
			FCIE_RIU_W16(reg_ddfset_23_16, 0x0042);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x7627);
			u16_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
			u16_value_reg_emmc_pll_test = 0;
			break;
		case 11: // 48M
			FCIE_RIU_W16(reg_ddfset_23_16, 0x0048);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x0000);
			u16_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
			u16_value_reg_emmc_pll_test = 0;
			break;
		case 12: // 40M
			FCIE_RIU_W16(reg_ddfset_23_16, 0x002B);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x3333);
			u16_value_reg_emmc_pll_pdiv = 4;// PostDIV: 8
			u16_value_reg_emmc_pll_test = 0;
			break;
		case 13: // 36M
			FCIE_RIU_W16(reg_ddfset_23_16, 0x0030);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x0000);
			u16_value_reg_emmc_pll_pdiv = 4;// PostDIV: 8
			u16_value_reg_emmc_pll_test = 0;
			break;
		case 14: // 32M
			FCIE_RIU_W16(reg_ddfset_23_16, 0x0036);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x0000);
			u16_value_reg_emmc_pll_pdiv = 4;// PostDIV: 8
			u16_value_reg_emmc_pll_test = 0;
			break;
		case 15: // 27M
			FCIE_RIU_W16(reg_ddfset_23_16, 0x0040);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x0000);
			u16_value_reg_emmc_pll_pdiv = 4;// PostDIV: 8
			u16_value_reg_emmc_pll_test = 0;
			break;
		case 16: // 20M
			FCIE_RIU_W16(reg_ddfset_23_16, 0x002B);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x3333);
			u16_value_reg_emmc_pll_pdiv = 7;// PostDIV: 16
			u16_value_reg_emmc_pll_test = 1;
			break;
		default:
			printk("SDIO Err: wrong PLL clock selector!\n");
			while(1);
			break;
	}

	// 3. VCO clock ( loop N = 4 )
	FCIE_RIU_16_OF(reg_sdiopll_fbdiv, 0xFFFF);
	FCIE_RIU_16_ON(reg_sdiopll_fbdiv, 0x6);// PostDIV: 8

	// 4. 1X clock
	FCIE_RIU_16_OF(reg_sdiopll_pdiv, BIT2|BIT1|BIT0);
	FCIE_RIU_16_ON(reg_sdiopll_pdiv, u16_value_reg_emmc_pll_pdiv);// PostDIV: 8

	if(u16_value_reg_emmc_pll_test) {
	    FCIE_RIU_16_ON(reg_emmc_pll_test, BIT10);
	}
	else {
		FCIE_RIU_16_OF(reg_emmc_pll_test, BIT10);
	}

	mdelay(1); // asked by Irwin

	return SDIO_PLL_CLOCK[u8PllClockSlector];
}


U32 HalSdio_SetClock(U32 u32Clock)
{
	U32 u32RealClock;

	REG_FCIE_CLRBIT(FCIE_SD_MODE, BIT_CLK_EN);

	switch(HalSdio_GetPadType())
	{
		case SDIO_MODE_8BITS_MACRO_SDR:
		case SDIO_MODE_32BITS_MACRO_SDR104:
		case SDIO_MODE_32BITS_MACRO_AFIFO:
			REG_FCIE_CLRBIT(REG_CLK_SDIO, BIT_SDIO_CLK_GATING|BIT_SDIO_CLK_INVERSE|BIT_CLKGEN_SDIO_MASK);
			u32RealClock = HalSdio_SetPllClock(u32Clock);
			REG_FCIE_SETBIT(REG_CLK_SDIO, BIT_SDIO_CLK_SRC_SEL|0xB<<2);
			break;

		case SDIO_MODE_8BITS_MACRO_DDR:
			REG_FCIE_CLRBIT(REG_CLK_SDIO, BIT_SDIO_CLK_GATING|BIT_SDIO_CLK_INVERSE|BIT_CLKGEN_SDIO_MASK);
			u32RealClock = HalSdio_SetPllClock(u32Clock<<2);
			REG_FCIE_SETBIT(REG_CLK_SDIO, BIT_SDIO_CLK_SRC_SEL|0xB<<2);
			break;

		default:
			#if 1 // use SDIO PLL to have 50MHz clock for improved 4% performance
			if(u32Clock==50000)
			{
				REG_FCIE_CLRBIT(REG_CLK_SDIO, BIT_SDIO_CLK_GATING|BIT_SDIO_CLK_INVERSE|BIT_CLKGEN_SDIO_MASK);
				u32RealClock = HalSdio_SetPllClock(200000);
				REG_FCIE_SETBIT(REG_CLK_SDIO, BIT_SDIO_CLK_SRC_SEL|0xB<<2);
			}
			else
			#endif
			{
				u32RealClock = HalSdio_SetClkgenClock(u32Clock);
			}
			break;
	}

	return u32RealClock;
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

static volatile U16 sdio_event;

typedef enum
{
    IRQ_TYPE_NONE	= 0,
    IRQ_TYPE_EVENT	= 1,
    IRQ_TYPE_D1INT	= 2,

} E_IRQ_TYPE;


void HalSdio_ClearWaitQueue(void)
{
	sdio_event = 0;
}

E_IRQ_TYPE HalSdio_SaveMieEvent(struct mmc_host *host)
{
#if defined ENABLE_SDIO_D1_INTERRUPT && ENABLE_SDIO_D1_INTERRUPT

	U16 u16Reg = FCIE_RIU_R16(FCIE_MIE_EVENT);

	if(u16Reg & BIT_SDIO_INT)
	{
 		mmc_signal_sdio_irq(host);
		FCIE_RIU_W16(FCIE_MIE_EVENT, BIT_SDIO_INT); // W1C event
		return IRQ_TYPE_D1INT;
	}
	else

#endif

	{
		//sdio_event |= u16Reg; // summary all mie event

		#if defined ENABLE_SDIO_D1_INTERRUPT && ENABLE_SDIO_D1_INTERRUPT
			sdio_event = FCIE_RIU_R16(FCIE_MIE_EVENT) & FCIE_RIU_R16(FCIE_MIE_INT_EN) & (~BIT_SDIO_INT);
		#else
			sdio_event = FCIE_RIU_R16(FCIE_MIE_EVENT) & FCIE_RIU_R16(FCIE_MIE_INT_EN);
		#endif

		if(sdio_event & BIT_DMA_END)
	    {
	        FCIE_RIU_16_OF(FCIE_MIE_INT_EN, BIT_DMA_END);
	    }

		if(sdio_event & BIT_SD_CMD_END)
		{
			FCIE_RIU_16_OF(FCIE_MIE_INT_EN, BIT_SD_CMD_END);
		}

		if(sdio_event & BIT_BUSY_END_INT)
		{
			FCIE_RIU_16_OF(FCIE_MIE_INT_EN, BIT_BUSY_END_INT);
		}

		if(sdio_event & BIT_ERR_STS)
		{
			FCIE_RIU_16_OF(FCIE_MIE_INT_EN, BIT_ERR_STS);
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

	if(wait_event_timeout(sdio_mie_wait, (sdio_event==u16ReqVal) || ( sdio_event&BIT_ERR_STS), timeout))
	{
		if(sdio_event&BIT_ERR_STS)
		{
			return IO_ERROR_DETECT;
		}
		return IO_SUCCESS;
	}
	else
	{
		printk("wait sdio mie evnet to req %04Xh, event = %04Xh\n", u16ReqVal, sdio_event);
		return IO_TIME_OUT;
	}

	return IO_SUCCESS;
}

void HalSdio_EnableD1Interrupt(U8 u8Enable)
{
	FCIE_RIU_16_ON(FCIE_SDIO_MOD, BIT_SDIO_DET_INT_SRC); // level detect

	if(u8Enable)
	{
		FCIE_RIU_16_ON(FCIE_MIE_INT_EN, BIT_SDIO_INT);
		FCIE_RIU_16_ON(SDIO_INTR_DET, BIT_SDIO_DET_ON); // enable SDIO interrupt
	}
	else
	{
		//FCIE_RIU_16_ON(SDIO_INTR_DET, BIT_SDIO_DET_ON); // hardware auto clear detect_on
	}
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

U32 HalSdio_FindSkewer(U32 u32_Candidate, SKEWER * st_skewer)
{
	U32 u32_i;
	st_skewer->u32LatchOKStart = 0xBEEF;
	st_skewer->u32LatchOKEnd = 0xBEEF;
	st_skewer->u8_edge = 0;

	//printf("%s(%05Xh)\n", __FUNCTION__, u32_Candidate);

	for(u32_i=0; u32_i<20; u32_i++) {
		if( (u32_Candidate&(1<<u32_i)) ^ ((u32_Candidate>>1)&(1<<u32_i)) ) { // use XOR to find 0b01/0b10 edge
			st_skewer->u8_edge++;
			//printf("%d [%04Xh ^ %04Xh] = %04Xh, ", u32_i, (u32_Candidate&(1<<u32_i)), ((u32_Candidate>>1)&(1<<u32_i)),
			//	( (u32_Candidate&(1<<u32_i)) ^ ((u32_Candidate>>1)&(1<<u32_i)) ) );
			if(st_skewer->u32LatchOKStart==0xBEEF) {
				st_skewer->u32LatchOKStart = u32_i;
			} else if(st_skewer->u32LatchOKEnd==0xBEEF) {
				st_skewer->u32LatchOKEnd = u32_i;
			}
		}
	}

	st_skewer->u32LatchOKEnd--;

	//eMMC_printf("u8_edge = %d, ", st_skewer->u8_edge);
	//eMMC_printf("start = %d, ", st_skewer->u32LatchOKStart);
	//eMMC_printf("end = %d\n", st_skewer->u32LatchOKEnd);

	return 0;
}

U32 HalSdio_SlectBestPhase(U32 u32_Candidate, SKEWER * pSkewer)
{
	SKEWER st_skewer1 = {0};
	SKEWER st_skewer2 = {0};
	U32 u32_LatchBestSelect = 0;
	U32 u32_Temp;
	U32 u32Ret = 0;

	//printk("\n%s(%05Xh)\n", __FUNCTION__, u32_Candidate);

	if(!u32_Candidate) {
		printk("Scan fail: no workable phase\n");
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
			//eMMC_printf("ring type:\n");
			st_skewer1.u32LatchOKStart--;
			st_skewer1.u32LatchOKEnd++;

			//eMMC_printf("start = %d, ", st_skewer1.u32LatchOKStart);
			//eMMC_printf("end = %d\n", st_skewer1.u32LatchOKEnd);

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
			printk("Scan warn: %05Xh (", u32_Candidate);
			printk("%d ~ ", st_skewer1.u32LatchOKStart);
			printk("%d)\n", st_skewer1.u32LatchOKEnd);
			u32Ret = 2;
			goto ErrorHandle;
		}
		else {
			printk("Scan success: %05Xh (", u32_Candidate);
			printk("%d ~ ", st_skewer1.u32LatchOKStart);
			printk("%d)", st_skewer1.u32LatchOKEnd);
		}
	}
	else {
		printk("Scan fail: not continuous: %05Xh, u8_edge: ", u32_Candidate);
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

void HalSdio_SetTriggerLevel(U8 u8STrigLevel)
{
	REG_FCIE_CLRBIT(R_SDIO_PLL_0x20, BIT_TRIG_LEVEL_MASK);
	switch(u8STrigLevel)
	{
		case 0:
			break;
		case 1:
			REG_FCIE_SETBIT(R_SDIO_PLL_0x20, BIT_TRIG_LEVEL_1);
			break;
		case 2:
			REG_FCIE_SETBIT(R_SDIO_PLL_0x20, BIT_TRIG_LEVEL_2);
			break;
		case 3:
			REG_FCIE_SETBIT(R_SDIO_PLL_0x20, BIT_TRIG_LEVEL_3);
			break;
		case 4:
			REG_FCIE_SETBIT(R_SDIO_PLL_0x20, BIT_TRIG_LEVEL_4);
			break;
		case 5:
			REG_FCIE_SETBIT(R_SDIO_PLL_0x20, BIT_TRIG_LEVEL_5);
			break;
		case 6:
			REG_FCIE_SETBIT(R_SDIO_PLL_0x20, BIT_TRIG_LEVEL_6);
			break;
		case 7:
			REG_FCIE_SETBIT(R_SDIO_PLL_0x20, BIT_TRIG_LEVEL_7);
			break;
		default:
			printk("SdErr: wrong parameter for set trigger level\n");
			break;
	}
}

void HalSdio_SetVDD(U8 u8OnOff)
{
	printk("please porting me...\n");
}



// must set 3.3V fisrt, then switch to 1.8V

void HalSdio_SetIOVoltage(U8 u8Volt)
{
	U32 u32_i = 0;
	static U8 only_need_run_once = 0;
	static U8 ldo_on_off_status = 0;

	if(!only_need_run_once)
	{
		// 1. disable OSP (short protection curcuit related)
		REG_FCIE_SETBIT(R_SDIO_PLL_0x1D, BIT15);

		// 2. OSP count = 0x30
		REG_FCIE_CLRBIT(R_SDIO_PLL_0x37, 0xFF00);
		REG_FCIE_SETBIT(R_SDIO_PLL_0x37, 0x3000);

		only_need_run_once = 1;
	}

	if(u8Volt==VOLT_00V)
	{
		ldo_on_off_status = 0;
		only_need_run_once = 0;
		REG_FCIE_SETBIT(R_SDIO_PLL_0x37, BIT05);		// Turning off LDO
	}
	else
	{
		if(!ldo_on_off_status) // LDO is off
		{
			// 3. tunning on LDO
			REG_FCIE_SETBIT(R_SDIO_PLL_0x37, BIT05);		// Turning off LDO
			mdelay(10);							// delay 10ms
			REG_FCIE_CLRBIT(R_SDIO_PLL_0x37, BIT05);		// Turning on LDO
			ldo_on_off_status = 1;
		}

		// 4. check if 3.3V ready
		if(u8Volt==VOLT_33V)
		{
			REG_FCIE_CLRBIT(R_SDIO_PLL_0x37, BIT02|BIT04);		// set to 3.3V

			//printk("wait 3.3V ready\n");

			while(1)
			{
				if (FCIE_RIU_R16(R_SDIO_PLL_0x10) & BIT12) break;
				//printk(".");

				mdelay(1); // delay 1ms
				if(u32_i++>1000)
				{
					printk(LIGHT_RED"SdErr: timeout to set 3.3V\n"NONE);
					break;
				}
			}
			//printk(LIGHT_RED"3.3V is ready\n"NONE);
		}
		// 5. check if 1.8V ready
		else if(u8Volt==VOLT_18V)
		{
			REG_FCIE_SETBIT(R_SDIO_PLL_0x37, BIT02|BIT04);		// set to 1.8V

			//printk("wait 1.8V ready\n");

			while(1)
			{
				if (FCIE_RIU_R16(R_SDIO_PLL_0x10) & BIT13) break;
				//printk(".");

				mdelay(1); // delay 1ms
				if(u32_i++>1000)
				{
					printk(LIGHT_RED"SdErr: timeout to set 1.8V\n"NONE);
					break;
				}
			}

			//printk(LIGHT_RED"1.8V is ready\n"NONE);
		}
	}
}

//
// skew1: 0~17 (18 steps)
// skew2: 0~8  ( 9 steps)
// skew3: 0~8  ( 9 steps)
// skew4: 0~17 (18 steps)

void HalSdio_SetSkew(U8 u8SkewIdx, U32 u32SkewValue)
{
	//if( u8SkewIdx==1 || u8SkewIdx==2 )
		//printk(LIGHT_RED"HalSdio_SetSkew%d --> %d\n"NONE, u8SkewIdx, u32SkewValue);

	switch(u8SkewIdx)
	{
		case 1:
			if(u32SkewValue>17) goto ErrorHandle;

			REG_FCIE_CLRBIT(R_SDIO_PLL_0x03, BIT_SKEW1_MASK);
			if(u32SkewValue<9)
			{
				REG_FCIE_CLRBIT(R_SDIO_PLL_0x69, BIT3);
				REG_FCIE_SETBIT(R_SDIO_PLL_0x03, u32SkewValue);
			}
			else
			{
				REG_FCIE_SETBIT(R_SDIO_PLL_0x69, BIT3);
				REG_FCIE_SETBIT(R_SDIO_PLL_0x03, u32SkewValue-9);
			}
			break;

		case 2:
			if(u32SkewValue>8) goto ErrorHandle;

			REG_FCIE_CLRBIT(R_SDIO_PLL_0x03, BIT_SKEW2_MASK);
			REG_FCIE_SETBIT(R_SDIO_PLL_0x03, u32SkewValue<<4);
			break;

		case 3:
			if(u32SkewValue>8) goto ErrorHandle;

			REG_FCIE_CLRBIT(R_SDIO_PLL_0x03, BIT_SKEW3_MASK);
			REG_FCIE_SETBIT(R_SDIO_PLL_0x03, u32SkewValue<<8);
			break;

		case 4:
			if(u32SkewValue>17) goto ErrorHandle;

			REG_FCIE_CLRBIT(R_SDIO_PLL_0x03, BIT_SKEW4_MASK);
			if(u32SkewValue<9)
			{
				REG_FCIE_CLRBIT(R_SDIO_PLL_0x69, BIT_CLK_SKEW_INVERSE); // inverse of CLK_SKEW_LAT
				REG_FCIE_SETBIT(R_SDIO_PLL_0x03, u32SkewValue<<12);
			}
			else
			{
				REG_FCIE_SETBIT(R_SDIO_PLL_0x69, BIT_CLK_SKEW_INVERSE);
				REG_FCIE_SETBIT(R_SDIO_PLL_0x03, (u32SkewValue-9)<<12);
			}
			break;

		default:
			goto ErrorHandle;
			break;
	}

	return;

ErrorHandle:

	printk("SDErr: wrong skew index (%d) or wrong skew value (%d)\n", u8SkewIdx, u32SkewValue);

}

void HalSdio_SetDqs(U32 u32DqsValue)
{
	if(u32DqsValue>7) goto ErrorHandle;

	REG_FCIE_W(R_SDIO_PLL_0x6C, u32DqsValue);

	return;

ErrorHandle:

	printk("SdErr: wrong dqs value %d\n", u32DqsValue);
}

void HalSdio_SetDrivingStrength(U16 u16DrivingLevel)
{
	const unsigned short u16RegValue[8] = {0x00, 0x03, 0x0C, 0x0F, 0x30, 0x33, 0x3C, 0x3F};

	FCIE_RIU_16_OF(CHIPTOP_33h, 0x3F); // bit 0~6 driving mask

	FCIE_RIU_16_ON(CHIPTOP_33h, u16RegValue[u16DrivingLevel]);

	//printk(LIGHT_PURPLE"Set driving level to %04Xh\n"NONE, FCIE_RIU_R16(CHIPTOP_33h));
}

#ifdef SDIO_SUPPORT_SD30

//static U32 gu32BestDqs;

unsigned char temp_buf[512] __attribute__((aligned(0x80)));

static const unsigned char sd_tunning_pattern[64] = {

	0xFF,0x0F,0xFF,0x00,  0xFF,0xCC,0xC3,0xCC,  0xC3,0x3C,0xCC,0xFF,  0xFE,0xFF,0xFE,0xEF,
    0xFF,0xDF,0xFF,0xDD,  0xFF,0xFB,0xFF,0xFB,  0xBF,0xFF,0x7F,0xFF,  0x77,0xF7,0xBD,0xEF,
    0xFF,0xF0,0xFF,0xF0,  0x0F,0xFC,0xCC,0x3C,  0xCC,0x33,0xCC,0xCF,  0xFF,0xEF,0xFF,0xEE,
    0xFF,0xFD,0xFF,0xFD,  0xDF,0xFF,0xBF,0xFF,  0xBB,0xFF,0xF7,0xFF,  0xF7,0x7F,0x7B,0xDE
};

unsigned char tuning_buf[64] __attribute__((aligned(0x80)));

U32 DrvSd_CMD19(U32 u32TuningClock)
{
	U32 u32_err = 0;
    CMD_RSP_INFO CmdRspInfo;
	U32 u32_dma_addr;
	uintptr_t * BufferVirtAddr = NULL;
	//struct mmc_data data_tuning;
	//data_tuning.sg =

	memset(&CmdRspInfo, 0, sizeof(CMD_RSP_INFO));

	BufferVirtAddr = (uintptr_t *)tuning_buf;
	u32_dma_addr = virt_to_phys((void *)BufferVirtAddr); // virtual to bus address
	Chip_Clean_Cache_Range_VA_PA((uintptr_t)BufferVirtAddr, (uintptr_t)u32_dma_addr, 64);

	CmdRspInfo.CardClock = u32TuningClock;
	CmdRspInfo.DirectTuningClock = u32TuningClock;
	CmdRspInfo.Command.Cmd.Index = 19;
	CmdRspInfo.Command.Cmd.Arg = U32BEND2LEND(0);
	//CmdRspInfo.ClockStop = CLK_KEEP;
    CmdRspInfo.CmdType = CMD_TYPE_ADTC;
    CmdRspInfo.RspType = RSP_TYPE_R1;
	CmdRspInfo.ReadWriteDir = DIR_R;
	CmdRspInfo.DataPath = PATH_DMA;
	CmdRspInfo.BusWidth = BUS_4_BITS;
	CmdRspInfo.BusAddr = u32_dma_addr;
	CmdRspInfo.pBufferAddr = NULL; // DMA don't need this field
	CmdRspInfo.BlockSize = 64;
	CmdRspInfo.BlockCount = 1;
	CmdRspInfo.ReadWriteTimeOut = 77;	// 20ns x 65536 x 77 = 100.9254 ms

    u32_err = HalSdio_SendCommandEx(&CmdRspInfo);

	if(u32_err)
	{
		goto ErrorHandle;
	}

	return 0;

ErrorHandle:

	return u32_err;
}

static void ms_sdmmc_dump_mem_line(char *buf, int cnt)
{
	int i;
	for(i = 0; i< cnt; i ++)
		printk("%02X ", (unsigned char)buf[i]);
	printk("|");
	for(i = 0; i< cnt; i ++)
		printk("%c", (buf[i] >= 32 && buf[i] < 128) ? buf[i] : '.');

	printk("\n");
}

void ms_sdmmc_dump_mem(void * buf, unsigned int count)
{
	unsigned int i;
	unsigned char * uc_pointer = NULL;

	uc_pointer = (unsigned char *)buf;

	printk("ms_sdmmc_dump_mem(0x%p, %d)\n", buf, count);

	for(i = 0; i < count; i +=16)
	{
		printk("0x%p: ", uc_pointer+i);
		ms_sdmmc_dump_mem_line(uc_pointer + i, (count<16)?count:16);
		if(i%512==496) printk("\n");
	}
}

void ms_sdmmc_dump_data(struct mmc_data * pData)
{
	U32 i, u32BusAddr, u32DmaLeng;
	struct scatterlist  *pScatterList = 0;

	for(i=0; i<pData->sg_len; i++) {

		pScatterList = &(pData->sg[i]);
		u32BusAddr = sg_dma_address(pScatterList);
		u32DmaLeng = sg_dma_len(pScatterList);

		#if 0
			// 64 bit kernel get 0 in sg_dma_len(), don't know why
			ms_sdmmc_dump_mem(phys_to_virt(u32BusAddr), u32DmaLeng);
		#else
			if(pData->sg_len==1)
			{
				//printk(YELLOW"length = %d\n"NONE, pScatterList->length);
				ms_sdmmc_dump_mem(phys_to_virt(u32BusAddr), pScatterList->length);
			}
			else
			{
				//printk(YELLOW"sg_dma_length = %d\n"NONE, u32DmaLeng);
				ms_sdmmc_dump_mem(phys_to_virt(u32BusAddr), u32DmaLeng);
			}
		#endif
	}
}


#define DBG_SKEW_SCAN(MSG)             //MSG

int HalSdio_ScanSkew_SetBest(U32 u32TuningClock)
{
	unsigned int u32_i;
	unsigned int u32_Skew4Result = 0;
	SKEWER stSkewer;
	int result;

	memset(&stSkewer, 0, sizeof(SKEWER));
	//gu8TuningMode = TUNE_SILENCE+TUNE_NOW;

	for(u32_i=0; u32_i<18; u32_i++)
	{
		//mmc_set_ios(mmc); // need to config clock again
		HalSdio_SetSkew(4, u32_i);
		result = DrvSd_CMD19(u32TuningClock);
		if(result)
		{
			DBG_SKEW_SCAN(printk("CMD19 fail @ skew4 = %d %04Xh\n", u32_i, result));
		}

		if( (result==0) || (result==1) ) // success or only CRC error
		{
			if(memcmp((void*)sd_tunning_pattern, (void*)tuning_buf, 64))
			{
				DBG_SKEW_SCAN(printk("tuning pattern compare error @ skew4 = %d\n", u32_i));
				//fcie_dump_mem(temp_buf, 64);
				//ms_sdmmc_dump_mem(sd_tunning_pattern, 64);
				ms_sdmmc_dump_mem(tuning_buf, 64);
			}
			else
			{
				//printf("tuning pattern compare OK\n");
				u32_Skew4Result |= (1<<u32_i);
			}
		}
	}

	//gu8TuningMode = TUNE_FINISH;

	if(HalSdio_SlectBestPhase(u32_Skew4Result, &stSkewer))
	{
		printk(LIGHT_RED"SdErr: Select skew4, %04Xh, fail\n"NONE, u32_Skew4Result);
	}

	//gu32BestSkew4 = stSkewer.u32LatchBest;

	HalSdio_SetSkew(4, stSkewer.u32LatchBest);

	return stSkewer.u32LatchBest;
}

int HalSdio_ScanDqs_SetBest(U32 u32TuningClock)
{
	unsigned int u32_i;
	unsigned int u32_Skew4Result = 0;
	SKEWER stSkewer;
	int result;

	memset(&stSkewer, 0, sizeof(SKEWER));
	//gu8TuningMode = TUNE_SILENCE+TUNE_NOW;

	for(u32_i=0; u32_i<8; u32_i++)
	{
		//if(!(gu8TuningMode&TUNE_SILENCE)) printf("DQS = %d\n", u32_i);

		//mmc_set_ios(mmc); // need to config clock again

		HalSdio_SetDqs(u32_i);

		#if 0
			result = DrvSd_CMD17(0, (U8*)0x20400000);
		#else
			result = DrvSd_CMD19(u32TuningClock);
		#endif

		if(result==0) u32_Skew4Result |= (1<<u32_i);
	}

	//gu8TuningMode = TUNE_FINISH;

	if(HalSdio_SlectBestPhase(u32_Skew4Result, &stSkewer))
	{
		printk(LIGHT_RED"SdErr: Select dqs fail\n"NONE);
	}

	//gu32BestDqs = stSkewer.u32LatchBest;

	HalSdio_SetDqs(stSkewer.u32LatchBest);

	return u32_i;
}

int gu32_Skew4__50M = 0;
int gu32_Skew4_100M = 0;
int gu32_Skew4_200M = 0;

void HalSdio_Tuning(U32 u32TuningClock)
{
	switch(HalSdio_GetPadType())
	{
		case SDIO_MODE_8BITS_MACRO_SDR:
		case SDIO_MODE_32BITS_MACRO_SDR104:
		case SDIO_MODE_32BITS_MACRO_AFIFO:
			gu32_Skew4__50M = HalSdio_ScanSkew_SetBest(u32TuningClock>>2);
			gu32_Skew4_100M = HalSdio_ScanSkew_SetBest(u32TuningClock>>1);
			gu32_Skew4_200M = HalSdio_ScanSkew_SetBest(u32TuningClock);
			break;

		case SDIO_MODE_8BITS_MACRO_DDR:
			HalSdio_ScanDqs_SetBest(u32TuningClock);
			break;

		default:
			printk("SdErr, this pad type not support tuning\n");
			break;
	}

}

U16 HalSdio_GetBusyStatus(void)
{
	#if 1
		return (FCIE_RIU_R16(FCIE_SD_STATUS) & BIT_SD_CARD_BUSY)? 1:0;
	#else
	if(FCIE_RIU_R16(FCIE_SD_STATUS) & BIT_SD_CARD_BUSY)
	{
		printk("card busy\n");
		return 1;
	}
	else
	{
		printk("card not busy\n");
		return 0;
	}
	#endif
}

#endif // SDIO_SUPPORT_SD30

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

extern void MsOS_DelayTaskUs_Poll(U32 u32Us); // MsOS.h
extern void MsOS_DelayTaskUs(U32 u32Us);


void HalSdioDelayUs(U32 u32Us)
{
	U32 u32_i, u32Ms;

	if(u32Us<1000)
	{
		mdelay(1);
	}
	else
	{
		u32Ms = u32Us/1000;
		for(u32_i=0; u32_i<u32Ms; u32_i++)
			mdelay(1);
	}
}

void HalSdioDelayMs(U32 u32Ms)
{
	U32 u32_i;

	for(u32_i=0; u32_i<u32Ms; u32_i++)
		mdelay(1);
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
	#if 0
	// MST245A big SD socket & small t-flash socket use the same card detect pin
	FCIE_RIU_16_OF(REG_PM_SLEEP_0x35, REG_SPIHOLDN_MODE);
	FCIE_RIU_16_OF(REG_PM_SLEEP_0x28, PWM1_MODE);
	FCIE_RIU_16_OF(REG_PM_SLEEP_0x35, REG_SW_GPIO_PM5_PM1_2_HK51_UART|REG_SW_GPIO_PM5_PM1_2_DIR_UAR);

	FCIE_RIU_16_ON(PM_GPIO_REG_0x01, PAD_PM_GPIO1_OEN); // oen = 1 for input
	if (FCIE_RIU_R16(PM_GPIO_REG_0x01) & PAD_PM_GPIO1_IN)
	{
		return 0;
	}
	else
	{
		return 1; // insert low SD socket
	}
	#else
		printk("sdio cdz not porting...\n");
		return 1;
	#endif
}

// 0: not write protect
// 1: write protect
S32 HalSdio_GetWriteProtect(void)
{
	return 1;
}

void HalSdio_SetCardPower(U8 u8OnOff)
{
	// MST245A big SD socket power control
	// GPIO_PM3 conflict with SPI boot case
	#if 0
	FCIE_RIU_16_ON(REG_PM_SLEEP_0x35, REG_SPI_GPIO); // config as GPIO
	FCIE_RIU_16_OF(PM_GPIO_REG_0x1A, PAD_PM_SPI_DI_OEN); // oen = 0 for output
	if(u8OnOff)
	{
		FCIE_RIU_16_OF(PM_GPIO_REG_0x1A, PAD_PM_SPI_DI_OUT); // power on
	}
	else
	{
		FCIE_RIU_16_ON(PM_GPIO_REG_0x1A, PAD_PM_SPI_DI_OUT); // power off
	}
	#endif
}



