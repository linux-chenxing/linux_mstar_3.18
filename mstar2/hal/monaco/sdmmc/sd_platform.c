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

	// CARD_REG16_ON(CHIPTOP_0C,BIT04|BIT05|BIT06|BIT07);
     //   CARD_REG16_ON(CHIPTOP_42,0x7f);
       // CARD_REG16_ON(CHIPTOP_43,BIT01);
        CARD_REG16_OFF(CHIPTOP_50,BIT15);
        CARD_REG16_OFF(CHIPTOP_57,BIT14|BIT15);
	 CARD_REG16_OFF(CHIPTOP_64,BIT03);
	 CARD_REG16_ON(CHIPTOP_10,BIT08);
 	CARD_REG16_OFF(CHIPTOP_6F,BIT06|BIT07);			
 	CARD_REG16_OFF(CHIPTOP_5A,BIT08|BIT09);		

        CARD_REG16_OFF(CHIPTOP_7B,BIT04|BIT05);
        CARD_REG16_ON(CHIPTOP_7B,BIT05);			//SDIO PAD2

	CARD_REG16_ON(MIU2_7A,BIT10);	//miu select


		
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
	
 //   CARD_REG16_OFF(FCIE_2D, BIT15);		//by pass mode
   CARD_REG16_ON(FCIE_2D, BIT15);		//by SDR mode


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
	     	  CARD_REG16_ON(PMGPIO_1, BIT00);		//Set GPIO11 as input
		   CARD_REG16_OFF(PMGPIO_1,BIT_GPIO_FIQ_MASK);		//enable GPIO 11 mask 
	     	  CARD_REG16_ON(PMGPIO_5, BIT00);		//Set PM5 as WP

		}
		else
		   CARD_REG16_ON(PMGPIO_1,BIT_GPIO_FIQ_MASK);		//enable GPIO 11 mask 
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
	     if ( CARD_REG_R16(PMGPIO_1) & BIT_GPIO_IN )        
                return (FALSE);
            else
              return (TRUE);


			
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
                     CARD_REG16_OFF(PMGPIO_1, BIT_GPIO_FIQ_POL);

        }
        else if((eGPIOOPT==EV_GPIO_OPT4))
        {
                     CARD_REG16_ON(PMGPIO_1, BIT_GPIO_FIQ_POL);

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

        if ( (CARD_REG_R16(PMGPIO_1) & BIT_GPIO_FIQ_MASK) == 0
            && (CARD_REG_R16(PMGPIO_1) & BIT_GPIO_FIQ_FINAL) )
            bRet = TRUE;

    }
    // clear FIQ anyway
    CARD_REG16_ON(PMGPIO_1, BIT_GPIO_FIQ_CLR);	//clear FIQ_FINAL	
    return bRet;
}
void ClockBeginSetting(void)
{
	  CARD_REG16_OFF(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x69),0x7f);		// 1X
	 CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x69),BIT06);		// 1X	

}	
BOOL_T Hal_GetCardWP(IPEmType eIP)
{
	     if ( CARD_REG_R16(PMGPIO_5) & BIT_GPIO_IN )        
                return (TRUE);
            else
              return (FALSE);

}
	
