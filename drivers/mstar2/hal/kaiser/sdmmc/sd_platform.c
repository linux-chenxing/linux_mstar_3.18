#include "hal_card_regs.h"
#include "hal_card_common.h"
#include "sd_platform.h"
#include "hal_card_timer.h"
#include "hal_sdmmc.h"


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
	CARD_REG16_OFF(CHIPTOP6,BIT00|BIT01|BIT04|BIT05|BIT06|BIT12|BIT13);		// clear nand mode. eMMC mode

	CARD_REG16_ON(CHIPTOP6, BIT04);			// choose SDIO mode 1
	//   FCIE_RIU_16_ON(FCIE_SETTING,BIT6);					//sd bypass mode

	CARD_REG16_OFF(CHIPTOP35,BIT00); 				// SDR MODE
	CARD_REG16_OFF(ALL_PAD_IN, BIT15);
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
	CARD_REG16_OFF(MIE_PATH_CTL, BIT01);			//disable SD/SDIO interface
}

//***********************************************************************************************************
// PAD Setting for Card Platform
//***********************************************************************************************************
/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_InitPADPin
 *     @author jeremy.wang (2011/12/1)
 * Desc: Init PAD Pin Status ( pull enable, pull up/down, driving strength)
 *
 * @param ePAD : PAD
 * @param bTwoCard : two card(1 bit) or not
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_InitPADPin(PADEmType ePAD, BOOL_T bTwoCard)
{

}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_InitGPIO
 *     @author jeremy.wang (2012/5/22)
 * Desc:
 *
 * @param eGPIO :
 * @param bEnable :
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_InitGPIO(GPIOEmType eGPIO, BOOL_T bEnable)
{
	if( eGPIO==EV_GPIO1 ) //EV_GPIO1 for Slot 0
	{
		if (bEnable)
		{
			CARD_REG16_ON(PMGPIO_CTL,BIT02);
			CARD_REG16_ON(PMGPIO_OE, BIT09);
			CARD_REG16_OFF(PMGPIO_MASK, BIT09);
		}
		else
		{
			CARD_REG16_ON(PMGPIO_MASK,BIT09);
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
	if( eGPIO==EV_GPIO1 ) //EV_GPIO1 for Slot 0
	{
		if (CARD_REG_R16(PMGPIO_POL) & BIT09)	//POLARITY decide
		{
			if (CARD_REG_R16(PMGPIO_RAWST) & BIT09)
				return TRUE;
			else
				return FALSE;
		}
		else
		{
			if (CARD_REG_R16(PMGPIO_RAWST) & BIT09)
				return FALSE;
			else
				return TRUE;
		}
	}

	return (FALSE);

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
    if( eGPIO==EV_GPIO1 ) //EV_GPIO1 for Slot 0
    {
        if(eGPIOOPT==EV_GPIO_OPT1)
        {
        }
        else if((eGPIOOPT==EV_GPIO_OPT2))
        {
        }
        else if((eGPIOOPT==EV_GPIO_OPT3))
        {
			CARD_REG16_OFF(PMGPIO_POL,BIT09);		//polarity 0, 0->1
        }
        else if((eGPIOOPT==EV_GPIO_OPT4))
        {
 			CARD_REG16_ON(PMGPIO_POL,BIT09);		//polarity 0, 0->1
        }
    }

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
    if( eGPIO==EV_GPIO1 ) //EV_GPIO1 for Slot 0
    {
		if ( (CARD_REG_R16(PMGPIO_MASK) & BIT09) == 0 && (CARD_REG_R16(PMGPIO_FINALST) & BIT09) )
			bRet = TRUE;
    }

    // clear FIQ anyway
    CARD_REG16_ON(PMGPIO_CLR,BIT09);			//clear FIQ_FINAL
    return bRet;
}
void ClockBeginSetting(void)
{
	  CARD_REG16_OFF(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x69),0x7f);		// 1X
	 CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x69),BIT06);		// 1X

}
