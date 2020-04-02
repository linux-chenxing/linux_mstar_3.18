//#include "hal_card_common.h"
#include "hal_card_regs5.h"
#include "sd_platform.h"
#include "hal_card_common.h"
#include <linux/delay.h>
#include "hal_sdmmc.h"

// this file use for manhattan only

#define A_SD_REG_BANK(IP)			GET_CARD_BANK(IP, EV_REG_BANK)
#define A_MIE_FUNC_CTL(IP)			GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x07)
#define A_DDR_MOD_REG(IP)			GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x0F)


void Hal_CARD_SerDriving(U8_T u8Driving)
{
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_InitSetting
 *     @author jeremy.wang (2011/12/1)
 * Desc: IP Begin Setting before every operation, it's about platform setting.
 *
 * @param eIP : FCIE1/FCIE2/...
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_IPBeginSetting(IPEmType eIP, PADEmType ePAD)
{
	HalFcieSdio_CheckStateMachine(eIP);

	if(eIP==0) // SDIO
	{
		// bring up note, chiptio related register
		FCIE_RIU_16_OF(CHIPTOP_50, BIT15); // all_pad_in
		FCIE_RIU_16_OF(CHIPTOP_7B, BIT04|BIT05); // reg_sdio_config = 0
		FCIE_RIU_16_OF(CHIPTOP_64, BIT00|BIT03|BIT04);

		// sdio
		// bring up note: GPIO_PAD SDR mode
		FCIE_RIU_16_OF(A_DDR_MOD_REG(eIP), BIT7|BIT8|BIT12);
		FCIE_RIU_16_ON(A_DDR_MOD_REG(eIP), BIT13|BIT14);
		#if 1
		// New patch for feedback clock reflex
		FCIE_RIU_16_ON(A_DDR_MOD_REG(eIP), BIT10);
		FCIE_RIU_16_ON(EMMC_PLL_REG_1A, BIT_EMMC_TEST);
		#endif

		if( !(FCIE_RIU_R16(MIU2_7A) & MIU_SELECT_BY_SDIO) )
		{
			printk(LIGHT_RED"SdWarn: MIU select not enable for SDIO IP\n"NONE);
			FCIE_RIU_16_ON(MIU2_7A, MIU_SELECT_BY_SDIO);	// miu select by IP
		}

		/*if( FCIE_RIU_R16(MIU2_69) & MIU2_PROTECT )
		{
			printk(LIGHT_RED"SdWarn: MIU2 protected\n"NONE);
			FCIE_RIU_16_OF(MIU2_69, MIU2_PROTECT);
		}

		if( FCIE_RIU_R16(MIU_69) & MIU_PROTECT )
		{
			printk(LIGHT_RED"SdWarn: MIU protected\n"NONE);
			FCIE_RIU_16_OF(MIU_69, MIU_PROTECT);
		}*/


		FCIE_RIU_W16(A_MIE_FUNC_CTL(eIP), BIT_SDIO_MOD);

		FCIE_RIU_16_ON(CHIPTOP_7B, BIT04|BIT05); // reg_sdio_config = 3
	}
	else if(eIP==1) // FCIE
	{
		// fcie
		// bring up note: GPIO_PAD SDR mode
		FCIE_RIU_16_OF(A_DDR_MOD_REG(eIP), BIT7|BIT8|BIT12);
		FCIE_RIU_16_ON(A_DDR_MOD_REG(eIP), BIT13|BIT14);

		// New patch for feedback clock reflex
		FCIE_RIU_16_ON(A_DDR_MOD_REG(eIP), BIT10);
		//FCIE_RIU_16_ON(EMMC_PLL_REG_1A, BIT_EMMC_TEST); // only sd_mode = 3 need this, MST079B use 2

		// chiptop
		FCIE_RIU_16_OF(CHIPTOP_50, BIT15); // all_pad_in
		FCIE_RIU_16_OF(CHIPTOP_64, BIT00|BIT03|BIT04);
		FCIE_RIU_16_ON(CHIPTOP_0A, BIT07|BIT06|BIT05|BIT04);
		FCIE_RIU_16_OF(CHIPTOP_6F, BIT07|BIT06);
		FCIE_RIU_16_OF(CHIPTOP_5A, BIT09|BIT08);
		FCIE_RIU_16_ON(CHIPTOP_5A, BIT09); // reg_sd_config = 2, MST079B
		FCIE_RIU_16_OF(CHIPTOP_6E, BIT07|BIT06); // emmc_config = 0

		// emmcpll
		FCIE_RIU_16_OF(EMMC_PLL_REG_68, BIT00|BIT01);
		FCIE_RIU_16_OF(EMMC_PLL_REG_6D, BIT00);
		FCIE_RIU_16_OF(EMMC_PLL_REG_70, BIT08);
		FCIE_RIU_W16(EMMC_PLL_REG_71, 0xFFFF);
		FCIE_RIU_W16(EMMC_PLL_REG_73, 0xFFFF);
		FCIE_RIU_16_ON(EMMC_PLL_REG_74, BIT15);

		// MIU
		if( !(FCIE_RIU_R16(MIU2_7A) & MIU_SELECT_BY_FCIE) )
		{
			printk(LIGHT_RED"SdWarn: MIU select not enable for FCIE IP\n"NONE);
			FCIE_RIU_16_ON(MIU2_7A, MIU_SELECT_BY_FCIE);	// miu select by IP
		}

	}

}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_IPEndSetting
 *     @author jeremy.wang (2012/5/3)
 * Desc: IP End Setting after every operation, it's about platform setting.
 *
 * @param eIP :
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_IPEndSetting(IPEmType eIP, PADEmType ePAD)
{
	if(eIP==1) // FCIE, the share IP
	{
		//FCIE_RIU_16_OF(CHIPTOP_5A, BIT8|BIT9); // diable SD pad to prevent eMMC driver send command to SD card
	}
}

void Hal_CARD_InitPADPin(PADEmType ePAD, BOOL_T bTwoCard)
{
	//printk("switch pad hal %d\n", ePAD);

	if(ePAD==1) // SDIO coneect to PAD_SDIO_IO0 ~ PAD_SDIO_IO5
	{
		//printk(LIGHT_RED"switch pad hal for SDIO connect to PAD_SDIO...\n"NONE);
	}
	else if(ePAD==0) // FCIE connect to PAD_SD_CLK, PAD_SD_CMD, PAD_SD_DX
	{
		//printk(YELLOW"switch pad hal for FCIE connect to PAD_SD...\n"NONE);
	}
}

void Hal_CARD_InitGPIO(GPIOEmType eGPIO, BOOL_T bEnable)
{
	//printk(CYAN"Init GPIO %d\n"NONE, eGPIO);

	if( eGPIO==EV_GPIO1 ) // for SDIO
    {
		if (bEnable)
		{
			FCIE_RIU_16_ON(CHIP_GPIO_52, GPIO88_OEN); // Config MST232A CDZ to input
		}
		else
		{
		}
    }
	else if( eGPIO==EV_GPIO2 )
	{
		if (bEnable)
		{
			FCIE_RIU_16_ON(PMGPIO_REG_07, PMGPIO7_OEN); // Config 079B CDZ to input
		}
		else
		{
		}
    }
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_GetGPIOState
 *     @author jeremy.wang (2012/5/22)
 * Desc:
 *
 * @param eGPIO :
 *
 * @return BOOL_T  :
 ----------------------------------------------------------------------------------------------------------*/
BOOL_T Hal_CARD_GetGPIOState(GPIOEmType eGPIO)
{
	if( eGPIO==EV_GPIO1 ) // SDIO
	{
		if (FCIE_RIU_R16(CHIP_GPIO_52) & GPIO88_IN) // MST232A CDZ
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	else if( eGPIO==EV_GPIO2 )
	{
		if (FCIE_RIU_R16(PMGPIO_REG_07) & PMGPIO7_IN) // 079B CDZ
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}

	return TRUE;
}
/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_SetGPIOIntAttr
 *     @author jeremy.wang (2012/5/22)
 * Desc:
 *
 * @param eGPIO :
 * @param eGPIOOPT :
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_SetGPIOIntAttr(GPIOEmType eGPIO, GPIOOptEmType eGPIOOPT)
{
	//printk(CYAN"Set GPIO int attr %d\n"NONE, eGPIO);
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_GPIOIntFilter
 *     @author jeremy.wang (2012/5/9)
 * Desc:
 *
 * @param eGPIO :
 *
 * @return BOOL_T  :
 ----------------------------------------------------------------------------------------------------------*/

BOOL_T Hal_CARD_GPIOIntFilter(GPIOEmType eGPIO)
{
    BOOL_T bRet=FALSE;
    return bRet;
}


void ClockBeginSetting(void)
{
	FCIE_RIU_16_OF(CLKGEN_SDIO, 0x3F);
	FCIE_RIU_16_ON(CLKGEN_SDIO, BIT06);
}


void HalCard_SetCardPower(PADEmType ePAD, U8_T u8OnOff)
{
	if(ePAD==EV_PAD1)
	{
		// MST232A using SDIO IP
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
	else if(ePAD==EV_PAD2) // FCIE
	{
		// MST079B always has power on SD socket
	}
}

void HalCard_ControlGPIO(U8_T u8GPIO, U8_T u8HighLow)
{
	if(u8GPIO==5)
	{
		FCIE_RIU_16_OF(CHIP_GPIO_02, BIT09);

		if(u8HighLow)
		{
			FCIE_RIU_16_ON(CHIP_GPIO_02, BIT08);
		}
		else
		{
			FCIE_RIU_16_OF(CHIP_GPIO_02, BIT08);
		}
	}
	else if(u8GPIO==6)
	{
		FCIE_RIU_16_OF(CHIP_GPIO_03, BIT01);

		if(u8HighLow)
		{
			FCIE_RIU_16_ON(CHIP_GPIO_03, BIT01);
		}
		else
		{
			FCIE_RIU_16_OF(CHIP_GPIO_03, BIT01);
		}
	}
}

U8_T HalCard_GetWriteProtect(IPEmType eIP)
{
	if(eIP==0) // SDIO
	{
		return FALSE; // not write protected
	}
	else // FCIE
	{
		return FALSE; // not write protected
	}
}
