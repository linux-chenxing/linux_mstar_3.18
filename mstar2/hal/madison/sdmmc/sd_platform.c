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

	 CARD_REG16_ON(CHIPTOP_0C,BIT04|BIT05|BIT06|BIT07);
        CARD_REG16_ON(CHIPTOP_42,0x7f);
        CARD_REG16_ON(CHIPTOP_43,BIT01);
        CARD_REG16_OFF(CHIPTOP_50,BIT15);
        CARD_REG16_OFF(CHIPTOP_5B,BIT10|BIT11);
        CARD_REG16_ON(CHIPTOP_5B,BIT11);			//SDIO PAD2
        CARD_REG16_OFF(CHIPTOP_4F,BIT13|BIT14);		
 	CARD_REG16_OFF(CHIPTOP_12,0x77);			 
 	CARD_REG16_OFF(CHIPTOP_64,BIT00);			
 	CARD_REG16_OFF(CHIPTOP_1F,BIT01);		
 	CARD_REG16_OFF(CHIPTOP_6F,BIT14|BIT15);			
 	CARD_REG16_OFF(CHIPTOP_70,BIT00|BIT02);			
	CARD_REG16_ON(MIU2_7B,BIT14);					//miu switch
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

    
 
    //printk("R_CLKGEN_FCIE = 0x%04X\r\n", FCIE_RIU_R16(R_CLKGEN_FCIE));
    //FCIE_RIU_W16(R_CLKGEN_FCIE, 0x0034); // 300K
     CARD_REG16_OFF(FCIE_2F, 0x1f00);
    CARD_REG16_ON(FCIE_2F, BIT10);		//by pass mode
    CARD_REG16_OFF(FCIE_2D, BIT15);		//by pass mode

   CARD_REG16_ON(SD_MODE,BIT09);	//SDIO mode
   CARD_REG16_OFF(SD_MODE,BIT12);	//port 1

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
		   CARD_REG16_ON(PMGPIO_OE, BIT07);		//PM7 for MADISON SDIO CDZ
	     	   CARD_REG16_OFF(PMGPIO_MASK, BIT07);  // 
		}
		else
		CARD_REG16_ON(PMGPIO_MASK, BIT07);  // for PM07
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
	    
	 if (CARD_REG_R16(PMGPIO_POL) & BIT07)	//POLARITY decide
	{
	  if (CARD_REG_R16(PMGPIO_RAWST) & BIT07) 
	  	return TRUE;
	  else 
	  	return FALSE;	  
	}
	 else
	{
	if (CARD_REG_R16(PMGPIO_RAWST) & BIT07) 
	  	return FALSE;
	  else 
	  	return TRUE;	  
	 }

			
    }
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
		CARD_REG16_OFF(PMGPIO_POL,BIT07);		//polarity 0, 0->1	 

        }
        else if((eGPIOOPT==EV_GPIO_OPT4))
        {
 		CARD_REG16_ON(PMGPIO_POL,BIT07);		//polarity 0, 0->1	 

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

	 if ( (CARD_REG_R16(PMGPIO_MASK) & BIT07) == 0
            && (CARD_REG_R16(PMGPIO_RAWST) & BIT07) )
            bRet = TRUE;

    }
    // clear FIQ anyway
    CARD_REG16_ON(PMGPIO_CLR,BIT07);   
    return bRet;
}
void ClockBeginSetting(void)
{
	  CARD_REG16_OFF(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x69),0x7f);		// 1X
	 CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x69),BIT06);		// 1X	

}	
