/***************************************************************************************************************
 *
 * FileName hal_card_platform.c
 *     @author benson.hsiao (2012/7/14)
 * Desc:
 *     The platform Setting of all cards will run here.
 *     Every Project will have XX project name for different hal_card_platform_XX.c files
 *     The goal is that we don't need to change "other" HAL Level code.
 *
 *     The limitations were listed as below:
 *     (1) This c file belongs to HAL level.
 *     (2) Its h file is included by driver API level, not driver flow process.
 *     (3) IP Init, PADPath, PADInit, Clock and Power function belong to here.
 *     (4) Timer Setting doesn't belong to here, because it will be included by other HAL level.
 *     (5) FCIE/SDIO IP Reg Setting doesn't belong to here.
 *     (6) If we could, we don't need to change any code of hal_card_platform.h
 *
 ***************************************************************************************************************/
#include "hal_card_regs.h"
#include "hal_card_common.h"
#include "hal_card_timer.h"
#include "hal_sdmmc.h"

#include "chip_int.h"
#include <mstar/mstar_chip.h>
#include <linux/sched.h>
#include "sd_platform.h"

#if defined(SHARE_FCIE_WITH_EMMC)  
extern bool mstar_mci_exit_checkdone_ForSD(void);
#endif

//***********************************************************************************************************
// Config Setting (Internel)
//***********************************************************************************************************
#define A_SD_REG_BANK(IP)           GET_CARD_BANK(IP, EV_REG_BANK)
#define A_MIE_EVENT_REG(IP)         GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x00)
#define A_MIE_INT_EN_REG(IP)        GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x01)
#define A_MIU_DMA1_REG(IP)          GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x03)
#define A_MIE_PATCH_CTL(IP)		  GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x0a)	
#define A_SD_MODE_REG(IP)           GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x10)
#define A_SD_STS_REG(IP)            GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x12)
#define A_NC_REORDER_REG(IP)        GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x2D)
#define A_EMMC_BOOTCFG_REG(IP)      GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x2F)

// Reg Dynamic Variable
//-----------------------------------------------------------------------------------------------------------
static volatile BOOL_T	    gb_SD_Mode_HighSpeed[2] = {0};
// Timer Definition
//-----------------------------------------------------------------------------------------------------------

#define SD_DELAY_1us					1
#define SD_DELAY_10us					10
#define SD_DELAY_100us				    100
#define SD_DELAY_1ms					(1000 * SD_DELAY_1us)
#define SD_DELAY_10ms					(10   * SD_DELAY_1ms)
#define SD_DELAY_100ms				    (100  * SD_DELAY_1ms)
#define SD_DELAY_1s					    (1000 * SD_DELAY_1ms)

//***********************************************************************************************************
// IP Setting for Card Platform
//***********************************************************************************************************

#define WAIT_EMMC_D0_TIME      3000

bool ms_sdmmc_wait_d0_for_emmc(void) // currently wait D0 by sd driver itself
{

    return TRUE;
}


bool ms_sdmmc_card_chg(unsigned int slotNo)
{
        return FALSE; // edison don't have share pin issue
}
/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_InitIPOnce
 *     @author jeremy.wang (2012/6/7)
 * Desc:  IP Once Setting , it's about platform setting.
 *
 * @param eIP :
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_IPOnceSetting(IPEmType eIP)
{

}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_Wait_Emmc_D0
 *     @author benson.hsiao (2012/6/21)
 * Desc:  Wait EMMC D0 to High.
 *
 * @param eIP :
 ----------------------------------------------------------------------------------------------------------*/
BOOL_T Hal_CARD_Wait_Emmc_D0(void)
{
	#if defined(SHARE_FCIE_WITH_EMMC)
	return (mstar_mci_exit_checkdone_ForSD());
    #else
    return TRUE;
	#endif

}

// Switch PAD
//------------------------------------------------------------------------------------------------


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_SetPADToPortPath
 *     @author jeremy.wang (2011/12/1)
 * Desc: Set PAD to connect IP Port
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param ePort : IP Port
 * @param ePAD : PAD (Could Set NOPAD for 1-bit two cards)
 * @param bTwoCard : 1-bit two cards or not
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_SetPADToPortPath(IPEmType eIP, PortEmType ePort, PADEmType ePAD, BOOL_T bTwoCard)
{
    SET_CARD_PORT(eIP, ePort);
#if 0
    if(eIP == EV_IP_FCIE1)
    {

    }
#endif
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_PullPADPin
 *     @author jeremy.wang (2011/12/1)
 * Desc: Pull PAD Pin for Special Purpose (Avoid Power loss.., Save Power)
 *
 * @param ePAD :  PAD
 * @param ePin :  Group Pin
 * @param ePinPull : Pull up/Pull down
 * @param bTwoCard :  two card(1 bit) or not
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_PullPADPin(PADEmType ePAD, PinEmType ePin, PinPullEmType ePinPull, BOOL_T bTwoCard)
{
}

//***********************************************************************************************************
// Clock Setting for Card Platform
//***********************************************************************************************************

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_SetClock
 *     @author jeremy.wang (2011/12/14)
 * Desc: Set Clock Level by Real Clock from IP
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param u32KClkFromIP : Clock Value From IP Source Set
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_SetClock(IPEmType eIP, U32_T u32ClkFromIPSet)
{

    if(eIP == EV_IP_FCIE1)
    {

        ClockBeginSetting();
//	printk("set clock %d\n",u32ClkFromIPSet);

	//if (u32ClkFromIPSet!=300000)
		//mdelay(5000);
	
        switch(u32ClkFromIPSet)
        {
            case CLK_F:      //48000KHz
	        CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,CLOCK_GEN_REG), _48M);

                break;
            case CLK_E:      //43200KHz
	        CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,CLOCK_GEN_REG), _43M);
		 
                break;
            case CLK_D:      //40000KHz
	        CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,CLOCK_GEN_REG), _36M);	//36M	 
                break;
            case CLK_C:      //36000KHz
	        CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,CLOCK_GEN_REG), _36M);	//36M	 
                break;
            case CLK_B:      //32000KHz
	        CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,CLOCK_GEN_REG), _32M);	//32M	 
                break;
            case CLK_A:      //27000KHz
	        CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,CLOCK_GEN_REG), _27M);	// 27M	 		 
		break;
            case CLK_9:      //20000KHz
	        CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,CLOCK_GEN_REG), _18M);	// 18M	 

                break;
            case CLK_8:      //12000KHz
                break;
            case CLK_7:      //300KHz
	 
	        CARD_REG16_ON(GET_CARD_REG_ADDR(A_CLKGEN_BANK,CLOCK_GEN_REG), _300K);	// 300K	 		 
                break;

        }
    }
}



/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_FindClockSetting
 *     @author jeremy.wang (2012/5/22)
 * Desc: Find Real Clock Level Setting by Reference Clock
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param u32ReffClk : Reference Clock Value
 * @param u8PassLevel : Pass Level to Clock Speed
 * @param u8DownLevel : Down Level to Decrease Clock Speed
 *
 * @return U32_T  : Real Clock
 ----------------------------------------------------------------------------------------------------------*/
U32_T Hal_CARD_FindClockSetting(IPEmType eIP, U32_T u32ReffClk, U8_T u8PassLevel, U8_T u8DownLevel)
{
    U8_T  u8LV = u8PassLevel;
    U32_T u32RealClk = 0;
    U32_T u32ClkArr[16] = {CLK_F, CLK_E, CLK_D, CLK_C, CLK_B, CLK_A, \
        CLK_9, CLK_8, CLK_7, CLK_6, CLK_5, CLK_4, CLK_3, CLK_2, CLK_1, CLK_0};

    for(; u8LV<16; u8LV++)
    {
        if( (u32ReffClk >= u32ClkArr[u8LV]) || (u8LV==15) || (u32ClkArr[u8LV+1]==0) )
        {
            u32RealClk = u32ClkArr[u8LV];
            break;
        }
    }

    /****** For decrease clock speed******/
    if( (u8DownLevel) && (u32RealClk) && ((u8LV+u8DownLevel)<=15) )
    {
        if(u32ClkArr[u8LV+u8DownLevel]>0) //Have Level for setting
            u32RealClk = u32ClkArr[u8LV+u8DownLevel];
    }

    return u32RealClk;
}


//***********************************************************************************************************
// Power Setting for Card Platform
//***********************************************************************************************************

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_SetPADPower
 *     @author jeremy.wang (2012/1/4)
 * Desc: Set PAD Power to Different Voltage
 *
 * @param ePAD : PAD
 * @param ePADVdd : LOW/MIDDLE/HIGH Voltage Level
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_SetPADPower(PADEmType ePAD, PADVddEmType ePADVdd)
{


}



/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_SetHighSpeed
 *     @author jeremy.wang (2012/3/29)
 * Desc: Set High Speed registers in other bank for SDR/DDR
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param bEnable : Enable or Disable
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_SetHighSpeed(IPEmType eIP, BOOL_T bEnable)
{
    gb_SD_Mode_HighSpeed[eIP] = bEnable;
}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_GetGPIONum
 *     @author jeremy.wang (2012/5/22)
 * Desc:
 *
 * @param eGPIO :
 *
 * @return U32_T  :
 ----------------------------------------------------------------------------------------------------------*/
U32_T Hal_CARD_GetGPIONum(GPIOEmType eGPIO)
{
    S32_T s32GPIO = -1;

    if( eGPIO==EV_GPIO1 ) //EV_GPIO1 for Slot 0
    {
    }
    else if( eGPIO==EV_GPIO2)
    {
    }

    if(s32GPIO>0)
        return (U32_T)s32GPIO;
    else
        return 0;
}
#if (D_PROJECT == D_PROJECT__MONACO)

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_PowerOn
 *     @author jeremy.wang (2011/12/13)
 * Desc: Power on Card Power
 *
 * @param ePAD : PAD
 * @param u16DelayMs : Delay ms for Stable Power
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_PowerOn(PADEmType ePAD, U16_T u16DelayMs)
{

    //printk("power on card\n");
    CARD_REG16_OFF(PMGPIO_11, BIT00);		//Set GPIO11 as output
    CARD_REG16_OFF(PMGPIO_11, BIT01);		//output hi

    Hal_Timer_uDelay(u16DelayMs);

}
/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_PowerOff
 *     @author jeremy.wang (2011/12/13)
 * Desc: Power off Card Power
 *
 * @param ePAD : PAD
 * @param u16DelayMs :  Delay ms to Confirm No Power
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_PowerOff(PADEmType ePAD, U16_T u16DelayMs)
{

  //  printk("power off card\n");
    CARD_REG16_OFF(PMGPIO_11, BIT00);		//Set GPIO11 as output
    CARD_REG16_ON(PMGPIO_11, BIT01);		//output hi

    Hal_Timer_uDelay(u16DelayMs);
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_TransMIUAddr
 *     @author jeremy.wang (2012/6/7)
 * Desc:
 *
 * @param eIP
 * @param u32Addr :
 * @return U32_T  :
 ----------------------------------------------------------------------------------------------------------*/
U32_T Hal_CARD_TransMIUAddr(IPEmType eIP, U32_T u32Addr)
{

  CARD_REG16_OFF(A_MIU_DMA1_REG(eIP), BIT14|BIT15);

   if( u32Addr >= MSTAR_MIU2_BUS_BASE) 
   {
    //	printk("miu2 buff\n");
	u32Addr -= MSTAR_MIU2_BUS_BASE;
        CARD_REG16_ON(A_MIU_DMA1_REG(eIP), BIT15);

   }
   else if( u32Addr >= MSTAR_MIU1_BUS_BASE)
   {
        u32Addr -= MSTAR_MIU1_BUS_BASE;
        CARD_REG16_ON(A_MIU_DMA1_REG(eIP), BIT14);
    }
    else
    {
    	  u32Addr -= MSTAR_MIU0_BUS_BASE;

    }
    return u32Addr;
}

#else
/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_PowerOn
 *     @author jeremy.wang (2011/12/13)
 * Desc: Power on Card Power
 *
 * @param ePAD : PAD
 * @param u16DelayMs : Delay ms for Stable Power
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_PowerOn(PADEmType ePAD, U16_T u16DelayMs)
{

    //Hal_Timer_uDelay(u16DelayMs);

}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_PowerOff
 *     @author jeremy.wang (2011/12/13)
 * Desc: Power off Card Power
 *
 * @param ePAD : PAD
 * @param u16DelayMs :  Delay ms to Confirm No Power
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_PowerOff(PADEmType ePAD, U16_T u16DelayMs)
{


    Hal_Timer_uDelay(u16DelayMs);
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_TransMIUAddr
 *     @author jeremy.wang (2012/6/7)
 * Desc:
 *
 * @param eIP
 * @param u32Addr :
 * @return U32_T  :
 ----------------------------------------------------------------------------------------------------------*/
U32_T Hal_CARD_TransMIUAddr(IPEmType eIP, U32_T u32Addr)
{
    #if (D_PRODUCT == D_TV)
    if( u32Addr >= MSTAR_MIU1_BUS_BASE)
    {
        u32Addr -= MSTAR_MIU1_BUS_BASE;
        CARD_REG16_ON(A_MIU_DMA1_REG(eIP), BIT15);
    }
    else
    {
        u32Addr -= MSTAR_MIU0_BUS_BASE;
        CARD_REG16_OFF(A_MIU_DMA1_REG(eIP), BIT15);
    }
    #endif
    return u32Addr;
}

#endif


















