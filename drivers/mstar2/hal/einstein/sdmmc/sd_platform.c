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
	CARD_REG16_OFF(CHIPTOP_50,BIT15);		//by pass mode
	 CARD_REG16_ON(CHIPTOP_0A,0xf00);		//
 //	 FCIE_RIU_16_OF(CHIPTOP_6F,BIT06|BIT07);		 
	// FCIE_RIU_16_OF(CHIPTOP_5A,WBIT08|WBIT09);		 
	 CARD_REG16_OFF(CHIPTOP_7B,BIT04|BIT05);		//clr SDIO pad	 
	 CARD_REG16_ON(CHIPTOP_7B,BIT04|BIT05);		//SDIO pad 3
	// FCIE_RIU_16_OF(CHIPTOP_6E,BIT06|BIT07);		//clr SDIO pad	 
	 CARD_REG16_OFF(CHIPTOP_64,BIT00|BIT03|BIT04);		//	 
	 CARD_REG16_OFF(CHIPTOP_52,BIT08|BIT10);		//	 
	CARD_REG16_ON(MIU2_79,BIT10);
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

    
   CARD_REG16_OFF(FCIE_2F, 0x1f00);
    CARD_REG16_ON(FCIE_2F, BIT10);		//by pass mode
   CARD_REG16_OFF(SD_MODE,BIT09);	//SDIO mode
    //printk("R_CLKGEN_FCIE = 0x%04X\r\n", FCIE_RIU_R16(R_CLKGEN_FCIE));
    //FCIE_RIU_W16(R_CLKGEN_FCIE, 0x0034); // 300K
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
		   CARD_REG16_ON(PMGPIO_7, BIT00);		//Set GPIO11 as input
		   CARD_REG16_OFF(PMGPIO_7,BIT_GPIO_FIQ_MASK);		//enable GPIO 11 mask 
		}
		else
		   CARD_REG16_ON(PMGPIO_7,BIT_GPIO_FIQ_MASK);		//enable GPIO 11 mask 
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
	         if ( CARD_REG_R16(PMGPIO_7) & BIT_GPIO_IN )        
                return (FALSE);
            else
              return (TRUE);
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
                     CARD_REG16_OFF(PMGPIO_7, BIT_GPIO_FIQ_POL);

        }
        else if((eGPIOOPT==EV_GPIO_OPT4))
        {
                     CARD_REG16_ON(PMGPIO_7, BIT_GPIO_FIQ_POL);

 
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
        if ( (CARD_REG_R16(PMGPIO_7) & BIT_GPIO_FIQ_MASK) == 0
            && (CARD_REG_R16(PMGPIO_7) & BIT_GPIO_FIQ_FINAL) )
            bRet = TRUE;
    }
    // clear FIQ anyway
    CARD_REG16_ON(PMGPIO_7, BIT_GPIO_FIQ_CLR);	//clear FIQ_FINAL	
    return bRet;
}
void ClockBeginSetting(void)
{
  	 CARD_REG16_OFF(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x69),0x7f);		// 1X
	 CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x69),BIT06);		// 1X	

}

