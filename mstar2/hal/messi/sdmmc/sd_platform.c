//#include "hal_card_common.h"
#include "hal_card_regs5.h"
#include "sd_platform.h"
#include "hal_card_common.h"
#include <linux/delay.h>
#include "hal_sdmmc.h"

#define A_SD_REG_BANK(IP)			GET_CARD_BANK(IP, EV_REG_BANK)
#define A_MIE_FUNC_CTL(IP)			GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x07)
#define A_DDR_MOD_REG(IP)			GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x0F)


/*void Hal_CARD_SerDriving(U8_T u8Driving)
{
	if(u8Driving>7)
	{
		printk("not correct driving level\n");
		return;
	}

	FCIE_RIU_16_OF(CHIPTOP_19, BIT11|BIT07|BIT03);

	if(u8Driving&BIT0)
	{
		FCIE_RIU_16_ON(CHIPTOP_19, BIT03);
	}
	if(u8Driving&BIT1)
	{
		FCIE_RIU_16_ON(CHIPTOP_19, BIT07);
	}
	if(u8Driving&BIT2)
	{
		FCIE_RIU_16_ON(CHIPTOP_19, BIT11);
	}

	printk("[%d] CHIPTOP_19 = %04Xh\n", u8Driving, FCIE_RIU_R16(CHIPTOP_19));
}*/

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

	// bring up note, chiptio related register
	FCIE_RIU_16_OF(CHIPTOP_6F, BIT07|BIT06); // reg_nand_mode = 0
	FCIE_RIU_16_OF(CHIPTOP_5A, BIT09|BIT08); // clear reg_sd_config
	FCIE_RIU_16_OF(CHIPTOP_7B, BIT05|BIT04); // clear reg_sdio_config
	FCIE_RIU_16_ON(CHIPTOP_7B, BIT04); // mode 1
	FCIE_RIU_16_OF(CHIPTOP_30, BIT08); // disable SPI
	FCIE_RIU_16_OF(CHIPTOP_12, 0x77); // test

	FCIE_RIU_16_ON(CHIPTOP_43, BIT15|BIT14|BIT13|BIT12); // reg_sd_pe = 0xF
	FCIE_RIU_16_OF(CHIPTOP_50, BIT15); // all_pad_in
	FCIE_RIU_16_ON(MIU2_79, MIU_SELECT_BY_IP);	// miu select by IP

	// Bring up note: GPIO_PAD SDR mode
	FCIE_RIU_16_OF(A_DDR_MOD_REG(eIP), BIT7|BIT8|BIT12);
	FCIE_RIU_16_ON(A_DDR_MOD_REG(eIP), BIT13|BIT14|BIT10);	//enable C2 patch

	if(eIP==0) // SDIO
	{
		if( !(FCIE_RIU_R16(MIU2_79) & MIU_SELECT_BY_IP))
		{
			printk(LIGHT_RED"SdWarn: MIU select not enable for SDIO IP\n"NONE);
			FCIE_RIU_16_ON(MIU2_79, MIU_SELECT_BY_IP);	// miu select by IP
		}
		#if 1
			FCIE_RIU_W16(A_MIE_FUNC_CTL(eIP), BIT_SDIO_MOD);
			FCIE_RIU_16_ON(CHIPTOP_7B, BIT04); // reg_sdio_config = 1 --> MST224A/B/C
		#else
			FCIE_RIU_W16(A_MIE_FUNC_CTL(eIP), BIT_SDIO_MOD);
			FCIE_RIU_16_ON(CHIPTOP_5A, BIT08); // reg_sd_config = 1
		#endif
	}
	else if(eIP==1) // FCIE
	{
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
U8_T HalCard_GetWriteProtect(IPEmType eIP)
{
	if(eIP==0) // SDIO
	{
		FCIE_RIU_16_OF(CHIPTOP_51, BIT14); // 
		FCIE_RIU_16_OF(CHIPTOP_6E, BIT1); // 

		FCIE_RIU_16_OF(CHIPTOP_52, BIT9); // 
		FCIE_RIU_16_ON(CHIP_GPIO_66, BIT9); // set oen to high for input
		if (FCIE_RIU_R16(CHIP_GPIO_66) & BIT10)
		       return TRUE;
		else
			return FALSE;
	}
	else // FCIE
	{
			return FALSE;
	}
}	

void Hal_CARD_InitGPIO(GPIOEmType eGPIO, BOOL_T bEnable)
{
	if( eGPIO==EV_GPIO1 ) // SDIO
    {
		if (bEnable)
		{
			FCIE_RIU_16_OF(CHIPTOP_6E, BIT1); // 
			FCIE_RIU_16_OF(CHIPTOP_52, BIT9); // 
			FCIE_RIU_16_ON(CHIP_GPIO_67, BIT1); // set oen to high for input

		}
		else
		{
		}
    }
	else if( eGPIO==EV_GPIO2 )
	{
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
		//printk("GPIO67:%x\n", FCIE_RIU_R16(CHIP_GPIO_67));

		if (FCIE_RIU_R16(CHIP_GPIO_67) & BIT2)
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
}


