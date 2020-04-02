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
#include "hal_card_regs5.h"

#include "hal_card_common.h"
#include "hal_card_timer.h"
#include "hal_sdmmc.h"
#include "chip_int.h"
#include <mstar/mstar_chip.h>
#include <linux/sched.h>
#include "sd_platform.h"
#include "ms_sdmmc_lnx.h"
#include <linux/clkm.h>

#if defined(SHARE_DATA_BUS_WITH_EMMC) && (SHARE_DATA_BUS_WITH_EMMC)
extern bool mstar_mci_exit_checkdone_ForSD(void);
#endif
int gScan=0;

//***********************************************************************************************************
// Config Setting (Internel)
//***********************************************************************************************************
#define A_SD_REG_BANK(IP)           GET_CARD_BANK(IP, EV_REG_BANK)

#define A_MMA_PRI_REG(IP)			GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x02)
#define A_MIU_DMA1_REG(IP)          GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x03)
#define A_MIE_FUNC_CTL(IP)			GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x07)
#define A_SD_MODE_REG(IP)           GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x0B)
#define A_SD_STS_REG(IP)            GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x0D)
#define A_NC_REORDER_REG(IP)        GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x2D)

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
int Hal_Check_Card_Pins(void)
{
	//printk("card sts:%x\n", FCIE_RIU_R16(A_SD_STS_REG(0)));
	return !(FCIE_RIU_R16(A_SD_STS_REG(0)) & 0xf00);
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
	#if defined(SHARE_DATA_BUS_WITH_EMMC) && (SHARE_DATA_BUS_WITH_EMMC)

		printk("Hal_CARD_Wait_Emmc_D0()\n");
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
extern void GoSDR50(int);
extern void GoDDR50(int );
extern U32 sdio_pll_setting(U16_T u16_ClkParam);

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_SetClock
 *     @author jeremy.wang (2011/12/14)
 * Desc: Set Clock Level by Real Clock from IP
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param u32KClkFromIP : Clock Value From IP Source Set
 ----------------------------------------------------------------------------------------------------------*/
#if defined(CONFIG_MSTAR_CLKM)
void Hal_CARD_SetClock(IPEmType eIP, U32_T u32ClkFromIPSet)
{
int sdio_clk,sdio_gate,fcie_clk,fcie_gate;
    if(eIP == EV_IP_FCIE1) // SDIO
    {

    	sdio_clk=get_handle("g_clk_sdio_p");
	sdio_gate=get_handle("g_clk_sdio");
	set_clk_source(sdio_gate,"g_sdio_clk_xtal");	//clock gated
	
	 switch(u32ClkFromIPSet)
	 {
	 	case CLK_F:
				set_clk_source(sdio_clk,"g_sdio_clk_48");
				break;
		case CLK_E:      //43200KHz
				set_clk_source(sdio_clk,"g_sdio_clk_43");
				break;

		case CLK_D:      //40000KHz
				set_clk_source(sdio_clk,"g_sdio_clk_40");
				break;

		case CLK_C:      //36000KHz
				set_clk_source(sdio_clk,"g_sdio_clk_36");
				break;

		case CLK_B:      //32000KHz
				set_clk_source(sdio_clk,"g_sdio_clk_32");
				break;

		case CLK_A:      //27000KHz
				set_clk_source(sdio_clk,"g_sdio_clk_20");
				break;

		case CLK_9:      //20000KHz
				set_clk_source(sdio_clk,"g_sdio_clk_20");
				break;

		case CLK_8:      //12000KHz
				set_clk_source(sdio_clk,"g_sdio_clk_5p4");

				break;

		case CLK_7:      //300KHz
				set_clk_source(sdio_clk,"g_sdio_clk_300k");
				break;
		case 0:
				printk("clock gating\n");
				break;

	 }
	set_clk_source(sdio_gate,"g_sdio_clk_p");	//clock enable

		
    }
else
{    
	fcie_clk=get_handle("g_clk_nfie_p");
	fcie_gate=get_handle("g_clk_nfie");
	set_clk_source(fcie_gate,"g_nfie_clk_xtal");	//clock gated
	
	 switch(u32ClkFromIPSet)
	 {
	 	case CLK_F:
				set_clk_source(fcie_clk,"g_nfie_clk_48");
				break;
		case CLK_E:      //43200KHz
				set_clk_source(fcie_clk,"g_nfie_clk_43");
				break;

		case CLK_D:      //40000KHz
				set_clk_source(fcie_clk,"g_nfie_clk_40");
				break;

		case CLK_C:      //36000KHz
				set_clk_source(fcie_clk,"g_nfie_clk_36");
				break;

		case CLK_B:      //32000KHz
				set_clk_source(fcie_clk,"g_nfie_clk_32");
				break;

		case CLK_A:      //27000KHz
				set_clk_source(fcie_clk,"g_nfie_clk_20");
				break;

		case CLK_9:      //20000KHz
				set_clk_source(fcie_clk,"g_nfie_clk_20");
				break;

		case CLK_8:      //12000KHz
				set_clk_source(fcie_clk,"g_nfie_clk_5p4");

				break;

		case CLK_7:      //300KHz
				set_clk_source(fcie_clk,"g_nfie_clk_300k");
				break;
		case 0:
				printk("clock gating\n");
				break;

	 }
	set_clk_source(fcie_gate,"g_nfie_clk_p");	//clock enable

}
}

#else
void Hal_CARD_SetClock(IPEmType eIP, U32_T u32ClkFromIPSet)
{
    if(eIP == EV_IP_FCIE1) // SDIO
    {
	//printk("SDIO set clock %d\n", u32ClkFromIPSet);

        ClockBeginSetting(); // clear clkgen setting

		#if 1
        switch(u32ClkFromIPSet)
        {
 #ifdef SDIO30       
			case CLK_11:
				 printk("200M\n");

				GoSDR50(gScan);
		   		sdio_pll_setting(eMMC_PLL_CLK_200M);
				break;
			case CLK_10:
				 printk("100M\n");

				GoSDR50(gScan);
		   		sdio_pll_setting(eMMC_PLL_CLK_100M);
				break;
#endif
			case CLK_F:      //48000KHz
				//	FCIE_RIU_16_ON(GET_CARD_REG_ADDR(CLKGEN_BANK, CLOCK_GEN_REG), _48M);
				//mdelay(3000);
				//#if defined SDBUS && (SDBUS==DDR50)
				//	FCIE_RIU_16_ON(CLKGEN_SDIO, (0x0c<<2));
					//GoDDR50(gScan);
				//#elif defined SDBUS && ((SDBUS==SDR50) ||(SDBUS==SDR104))
				//	FCIE_RIU_16_ON(CLKGEN_SDIO, (0x0b<<2));
				//	GoSDR50(gScan);
				//#else
				 //    printk("48M\n");

			 		FCIE_RIU_16_ON(CLKGEN_SDIO, _48M);
			//	#endif
				break;

			case CLK_E:      //43200KHz
				FCIE_RIU_16_ON(CLKGEN_SDIO, _43M);
				break;

			case CLK_D:      //40000KHz
				FCIE_RIU_16_ON(CLKGEN_SDIO, _36M);	//36M
				break;

			case CLK_C:      //36000KHz
				FCIE_RIU_16_ON(CLKGEN_SDIO, _36M);	//36M
				break;

			case CLK_B:      //32000KHz
				FCIE_RIU_16_ON(CLKGEN_SDIO, _32M);	//32M
				break;

			case CLK_A:      //27000KHz
				FCIE_RIU_16_ON(CLKGEN_SDIO, _27M);	// 27M
				break;

			case CLK_9:      //20000KHz
				FCIE_RIU_16_ON(CLKGEN_SDIO, _18M);	// 18M
				break;

			case CLK_8:      //12000KHz
				break;

			case CLK_7:      //300KHz
				FCIE_RIU_16_ON(CLKGEN_SDIO, _300K);	// 300K
				break;
			case 0:
				printk("clock gating\n");
				FCIE_RIU_16_ON(CLKGEN_SDIO, BIT00);	// clock gating
				break;

        }
		#else
			FCIE_RIU_16_ON(GET_CARD_REG_ADDR(CLKGEN_BANK, CLOCK_GEN_REG), _300K);	// 300K
		#endif

    }
	else // FCIE
	{
		//printk("FCIE set clock %d\n", u32ClkFromIPSet);

		FCIE_RIU_16_ON(CLKGEN_FCIE, FCIE_CLK_GATING); // gate clock

		FCIE_RIU_16_OF(CLKGEN_FCIE, FCIE_CLK_INVERSE|FCIE_CLK_SOURCE_MSK); // clear inverse & clock source
		FCIE_RIU_16_ON(CLKGEN_FCIE, FCIE_CLK_SOURCE_SEL); // select source from clk_fcie_p1

		switch(u32ClkFromIPSet)
		{
			case FCIE_CLK_F: // 48000000
				FCIE_RIU_16_ON(CLKGEN_FCIE, 0xF<<2);
				break;
			case FCIE_CLK_5: // 43200000
				FCIE_RIU_16_ON(CLKGEN_FCIE, 0x5<<2);
				break;
			case FCIE_CLK_4: // 40000000
				FCIE_RIU_16_ON(CLKGEN_FCIE, 0x4<<2);
				break;
			case FCIE_CLK_3: // 36000000
				FCIE_RIU_16_ON(CLKGEN_FCIE, 0x3<<2);
				break;
			case FCIE_CLK_2: // 32000000
				FCIE_RIU_16_ON(CLKGEN_FCIE, 0x2<<2);
				break;
			case FCIE_CLK_1: // 20000000
				FCIE_RIU_16_ON(CLKGEN_FCIE, 0x1<<2);
				break;
			case FCIE_CLK_D: //   300000
				FCIE_RIU_16_ON(CLKGEN_FCIE, 0xD<<2);
				break;
			default:
				printk("Select wrong FCIE clock\n");
				break;
		}

		FCIE_RIU_16_OF(CLKGEN_FCIE, FCIE_CLK_GATING); // open clock

	}
}

#endif

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

	if(eIP==0) // SDIO
	{
	    U8_T  u8LV = u8PassLevel;
	    U32_T u32RealClk = 0;
	#ifdef SDIO30	
	   #define CLKNUM		18
	    U32_T u32ClkArr[CLKNUM] = {CLK_11, CLK_10, CLK_F, CLK_E, CLK_D, CLK_C, CLK_B, CLK_A, \
	        CLK_9, CLK_8, CLK_7, CLK_6, CLK_5, CLK_4, CLK_3, CLK_2, CLK_1, CLK_0};
	#else
	   #define CLKNUM		16
	    U32_T u32ClkArr[CLKNUM] = {CLK_F, CLK_E, CLK_D, CLK_C, CLK_B, CLK_A, \
	        CLK_9, CLK_8, CLK_7, CLK_6, CLK_5, CLK_4, CLK_3, CLK_2, CLK_1, CLK_0};

	#endif

		//printk(LIGHT_RED"find clock setting %d\n"NONE, eIP);

		//printk("Reffclk:%d\n",u32ReffClk);
	    for(; u8LV<CLKNUM; u8LV++)
	    {
	        if( (u32ReffClk >= u32ClkArr[u8LV]) || (u8LV==(CLKNUM-1))  )
	        {
	            u32RealClk = u32ClkArr[u8LV];
	            break;
	        }
	    }
		//printk("realclk:%d\n",u32RealClk);
	    /****** For decrease clock speed******/
	    if( (u8DownLevel) && (u32RealClk) && ((u8LV+u8DownLevel)< CLKNUM) )
	    {
	        if(u32ClkArr[u8LV+u8DownLevel]>0) //Have Level for setting
	            u32RealClk = u32ClkArr[u8LV+u8DownLevel];
	    }
		//printk(LIGHT_RED"real clock select: %u\n"NONE,u32RealClk);

	    return u32RealClk;
	}
	else
	{
	    U8_T  u8LV = u8PassLevel;
	    U32_T u32RealClk = 0;
	    U32_T u32ClkArr[7] = {FCIE_CLK_F, FCIE_CLK_5, FCIE_CLK_4, FCIE_CLK_3, FCIE_CLK_2, FCIE_CLK_1, FCIE_CLK_D};

		//printk(YELLOW"find clock setting %d\n"NONE, eIP);

	    for(; u8LV<16; u8LV++)
	    {
	        if( (u32ReffClk >= u32ClkArr[u8LV]) || (u8LV==6)  )
	        {
	            u32RealClk = u32ClkArr[u8LV];
	            break;
	        }
	    }

		//printk(YELLOW"real clock select: %u\n"NONE, u32RealClk);

	    return u32RealClk;
	}
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
 * Function: Hal_CARD_PowerOn
 *     @author jeremy.wang (2011/12/13)
 * Desc: Power on Card Power
 *
 * @param ePAD : PAD
 * @param u16DelayMs : Delay ms for Stable Power
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_PowerOn(PADEmType ePAD, U16_T u16DelayMs)
{
	HalCard_SetCardPower(ePAD, 1);
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
	HalCard_SetCardPower(ePAD, 0);
    Hal_Timer_uDelay(u16DelayMs);
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

#define SDIO_DBG_MIU_ADDR(MSG) //MSG

U32_T Hal_CARD_TransMIUAddr(IPEmType eIP, U64_T u64BusAddr)
{

    FCIE_RIU_16_OF(A_MMA_PRI_REG(eIP), BIT02|BIT03);

    if( u64BusAddr >= MSTAR_MIU1_BUS_BASE)
    {
		SDIO_DBG_MIU_ADDR(printk("miu 1 addr: %08llX_%08llXh - %08llX_%08llXh = ", u64BusAddr>>32, u64BusAddr&0xFFFFFFFF, ((U64_T)MSTAR_MIU1_BUS_BASE)>>32, ((U64_T)MSTAR_MIU1_BUS_BASE)&0xFFFFFFFF));
        u64BusAddr -= MSTAR_MIU1_BUS_BASE;
		SDIO_DBG_MIU_ADDR(printk("%08llX_%08llXh\n", u64BusAddr>>32, u64BusAddr&0xFFFFFFFF));

        FCIE_RIU_16_ON(A_MMA_PRI_REG(eIP), BIT02);
    }
    else // miu 0
    {
		SDIO_DBG_MIU_ADDR(printk("miu 0 addr: %08llX_%08llXh - %08llX_%08llXh = ", u64BusAddr>>32, u64BusAddr&0xFFFFFFFF, ((U64_T)MSTAR_MIU0_BUS_BASE)>>32, ((U64_T)MSTAR_MIU0_BUS_BASE)&0xFFFFFFFF));
        u64BusAddr -= MSTAR_MIU0_BUS_BASE;
		SDIO_DBG_MIU_ADDR(printk("%08llX_%08llXh\n", u64BusAddr>>32, u64BusAddr&0xFFFFFFFF));
    }

    return (U32_T)u64BusAddr; // FCIE/SDIO IP DMA address is 32 bits
}




















