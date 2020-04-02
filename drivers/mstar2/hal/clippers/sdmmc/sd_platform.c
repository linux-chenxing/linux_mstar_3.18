//#include "hal_card_common.h"
#include "hal_card_regs5.h"
#include "sd_platform.h"
#include "hal_card_common.h"
#include <linux/delay.h>
#include "hal_sdmmc.h"

#define A_SD_REG_BANK(IP)           GET_CARD_BANK(IP, EV_REG_BANK)

#define A_DDR_MOD_REG(IP)            GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x0F)

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

	//miu switch
	FCIE_RIU_16_OF(CHIPTOP_7B,BIT06|BIT07);
	FCIE_RIU_16_ON(CHIPTOP_7B,BIT06);

	FCIE_RIU_16_OF(CHIPTOP_42,0x3f);
	FCIE_RIU_16_ON(CHIPTOP_42,0x3f00);
	FCIE_RIU_16_ON(CHIPTOP_43,0x1f);
	//FCIE_RIU_16_ON(CHIPTOP_4F,BIT02);			//SDIO PAD1
	//FCIE_RIU_16_OF(CHIPTOP_43,BIT08);
	FCIE_RIU_16_OF(CHIPTOP_50,BIT15);
	FCIE_RIU_16_ON(MIU2_79,BIT10);	//miu select
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
		FCIE_RIU_16_OF(CHIPTOP_5A, BIT8|BIT9); // diable SD pad to prevent eMMC driver send command to SD card
	}
}


void sd_go_18v(void)
{
	//--- switch to 1.8V----
	FCIE_RIU_16_ON(SDIOPLL_1D,BIT15);
	FCIE_RIU_16_OF(SDIOPLL_36,0xff00);
	FCIE_RIU_16_ON(SDIOPLL_36,0x3000);		//OSP count =0x30

	FCIE_RIU_16_ON(SDIOPLL_36,BIT05);		// Turning on LDO
	mdelay(10);			//delay 10ms
	FCIE_RIU_16_OF(SDIOPLL_36,BIT05);		// Turning on LDO

	while (1)
	{
		if (FCIE_RIU_R16(SDIOPLL_10) & BIT12) break;
		printk("wait 3.3 V ready\n");
	}

	FCIE_RIU_16_ON(SDIOPLL_36,BIT02|BIT04);		// enable 1.8V

	while (1)
	{
		if (FCIE_RIU_R16(SDIOPLL_10) & BIT13) break;
		printk("wait 1.8 V ready\n");
	}

	printk("1.8V is ready\n");

}
U32 sdio_pll_setting(U16_T u16_ClkParam)
{
	static U16_T u16_OldClkParam=0xFFFF;
	 U32 u32_value_reg_emmc_pll_pdiv;

	 if(u16_ClkParam == u16_OldClkParam)
		 return 0;
	 else
		 u16_OldClkParam = u16_ClkParam;

	 // 1. reset emmc pll
	FCIE_RIU_16_ON(reg_emmc_pll_reset,BIT0);
	FCIE_RIU_16_OF(reg_emmc_pll_reset,BIT0);

	// 2. synth clock
	switch(u16_ClkParam) {
	    case eMMC_PLL_CLK_200M: // 200M
		    FCIE_RIU_16_OF(reg_ddfset_23_16,0xffff);
			FCIE_RIU_16_OF(reg_ddfset_15_00,0xffff);
			#if 0
			FCIE_RIU_16_ON(reg_ddfset_23_16,0x22); // 20xMHz
			FCIE_RIU_16_ON(reg_ddfset_15_00,0x8F5C);
			#else
			FCIE_RIU_16_ON(reg_ddfset_23_16,0x24); // 195MHz
			FCIE_RIU_16_ON(reg_ddfset_15_00,0x03D8);
			#endif
			u32_value_reg_emmc_pll_pdiv =1;// PostDIV: 2
			break;
		case eMMC_PLL_CLK_160M:
			FCIE_RIU_16_OF(reg_ddfset_23_16,0xffff);
			FCIE_RIU_16_OF(reg_ddfset_15_00,0xffff);
			FCIE_RIU_16_ON(reg_ddfset_23_16,0x2B);
			FCIE_RIU_16_ON(reg_ddfset_15_00,0x3333);
			u32_value_reg_emmc_pll_pdiv =1;// PostDIV: 2
			break;
		case eMMC_PLL_CLK_140M:
			FCIE_RIU_16_OF(reg_ddfset_23_16,0xffff);
			FCIE_RIU_16_OF(reg_ddfset_15_00,0xffff);
			FCIE_RIU_16_ON(reg_ddfset_23_16,0x31);
			FCIE_RIU_16_ON(reg_ddfset_15_00,0x5F15);
			u32_value_reg_emmc_pll_pdiv = 1;// PostDIV: 2
			break;
		case eMMC_PLL_CLK_120M:
			FCIE_RIU_16_OF(reg_ddfset_23_16,0xffff);
			FCIE_RIU_16_OF(reg_ddfset_15_00,0xffff);
			FCIE_RIU_16_ON(reg_ddfset_23_16,0x39);
			FCIE_RIU_16_ON(reg_ddfset_15_00,0x9999);
			u32_value_reg_emmc_pll_pdiv = 1;// PostDIV: 2
			break;
		case eMMC_PLL_CLK_100M:
			FCIE_RIU_16_OF(reg_ddfset_23_16,0xffff);
			FCIE_RIU_16_OF(reg_ddfset_15_00,0xffff);
			FCIE_RIU_16_ON(reg_ddfset_23_16,0x45);
			FCIE_RIU_16_ON(reg_ddfset_15_00,0x1EB8);
			u32_value_reg_emmc_pll_pdiv = 1;// PostDIV: 2
			break;
		case eMMC_PLL_CLK__86M:
			FCIE_RIU_16_OF(reg_ddfset_23_16,0xffff);
			FCIE_RIU_16_OF(reg_ddfset_15_00,0xffff);
			FCIE_RIU_16_ON(reg_ddfset_23_16,0x28);
			FCIE_RIU_16_ON(reg_ddfset_15_00,0x2FA0);
			u32_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
			break;
		case eMMC_PLL_CLK__80M:
			FCIE_RIU_16_OF(reg_ddfset_23_16,0xffff);
			FCIE_RIU_16_OF(reg_ddfset_15_00,0xffff);
			FCIE_RIU_16_ON(reg_ddfset_23_16,0x2B);
			FCIE_RIU_16_ON(reg_ddfset_15_00,0x3333);
			u32_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
			break;
		case eMMC_PLL_CLK__72M:
			FCIE_RIU_16_OF(reg_ddfset_23_16,0xffff);
			FCIE_RIU_16_OF(reg_ddfset_15_00,0xffff);
			FCIE_RIU_16_ON(reg_ddfset_23_16,0x30);
			FCIE_RIU_16_ON(reg_ddfset_15_00,0x0000);
			u32_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
			break;
		case eMMC_PLL_CLK__62M:
			FCIE_RIU_16_OF(reg_ddfset_23_16,0xffff);
			FCIE_RIU_16_OF(reg_ddfset_15_00,0xffff);
			FCIE_RIU_16_ON(reg_ddfset_23_16,0x37);
			FCIE_RIU_16_ON(reg_ddfset_15_00,0xBDEF);
			u32_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
			break;
		case eMMC_PLL_CLK__52M:
			FCIE_RIU_16_OF(reg_ddfset_23_16,0xffff);
			FCIE_RIU_16_OF(reg_ddfset_15_00,0xffff);
			FCIE_RIU_16_ON(reg_ddfset_23_16,0x42);
			FCIE_RIU_16_ON(reg_ddfset_15_00,0x7627);
			u32_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
			break;
		case eMMC_PLL_CLK__48M:
			FCIE_RIU_16_OF(reg_ddfset_23_16,0xffff);
			FCIE_RIU_16_OF(reg_ddfset_15_00,0xffff);
			FCIE_RIU_16_ON(reg_ddfset_23_16,0x48);
			FCIE_RIU_16_ON(reg_ddfset_15_00,0x0000);
			u32_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
			break;
		case eMMC_PLL_CLK__40M:
			FCIE_RIU_16_OF(reg_ddfset_23_16,0xffff);
			FCIE_RIU_16_OF(reg_ddfset_15_00,0xffff);
			FCIE_RIU_16_ON(reg_ddfset_23_16,0x2B);
			FCIE_RIU_16_ON(reg_ddfset_15_00,0x3333);
			u32_value_reg_emmc_pll_pdiv = 4;// PostDIV: 8
			break;
		case eMMC_PLL_CLK__36M:
			FCIE_RIU_16_OF(reg_ddfset_23_16,0xffff);
			FCIE_RIU_16_OF(reg_ddfset_15_00,0xffff);
			FCIE_RIU_16_ON(reg_ddfset_23_16,0x30);
			FCIE_RIU_16_ON(reg_ddfset_15_00,0x0000);
			u32_value_reg_emmc_pll_pdiv = 4;// PostDIV: 8
			break;
		case eMMC_PLL_CLK__32M:
			FCIE_RIU_16_OF(reg_ddfset_23_16,0xffff);
			FCIE_RIU_16_OF(reg_ddfset_15_00,0xffff);
			FCIE_RIU_16_ON(reg_ddfset_23_16,0x36);
			FCIE_RIU_16_ON(reg_ddfset_15_00,0x0000);
			u32_value_reg_emmc_pll_pdiv = 4;// PostDIV: 8
			break;
		case eMMC_PLL_CLK__27M:
			FCIE_RIU_16_OF(reg_ddfset_23_16,0xffff);
			FCIE_RIU_16_OF(reg_ddfset_15_00,0xffff);
			FCIE_RIU_16_ON(reg_ddfset_23_16,0x40);
			FCIE_RIU_16_ON(reg_ddfset_15_00,0x0000);
			u32_value_reg_emmc_pll_pdiv = 4;// PostDIV: 8
			break;
		case eMMC_PLL_CLK__20M:
			FCIE_RIU_16_OF(reg_ddfset_23_16,0xffff);
			FCIE_RIU_16_OF(reg_ddfset_15_00,0xffff);
			FCIE_RIU_16_ON(reg_ddfset_23_16,0x2B);
			FCIE_RIU_16_ON(reg_ddfset_15_00,0x3333);
			u32_value_reg_emmc_pll_pdiv = 7;// PostDIV: 16
			break;
		default:
			printk("eMMC Err: emmc PLL not configed %x\n", u16_ClkParam);
			return 0xff;
			break;
	}

	// 3. VCO clock ( loop N = 4 )
	FCIE_RIU_16_OF(reg_sdiopll_fbdiv,0xffff);
	FCIE_RIU_16_ON(reg_sdiopll_fbdiv,0x6);// PostDIV: 8

	// 4. 1X clock
	FCIE_RIU_16_OF(reg_sdiopll_pdiv,BIT2|BIT1|BIT0);
	FCIE_RIU_16_ON(reg_sdiopll_pdiv,u32_value_reg_emmc_pll_pdiv);// PostDIV: 8

	if(u16_ClkParam==eMMC_PLL_CLK__20M) {
	    FCIE_RIU_16_ON(reg_emmc_pll_test, BIT10);
	}
	else {
		FCIE_RIU_16_OF(reg_emmc_pll_test, BIT10);
	}

	mdelay(1); // asked by Irwin

	return 0;
}


// Switch PAD
//------------------------------------------------------------------------------------------------


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
	//printk("switch pad hal %d\n", ePAD);

	if(ePAD==1) // SDIO coneect to PAD_SDIO_IO0 ~ PAD_SDIO_IO5
	{
		//printk("switch pad hal for SDIO connect to PAD_SDIO...\n");

		FCIE_RIU_16_OF(FCIE_MIE_FUNC_CTL,3);		//clear SD_EN, EMMC_EN
	  	FCIE_RIU_16_ON(FCIE_MIE_FUNC_CTL,BIT_SDIO_MOD);
	   	FCIE_RIU_16_OF(FCIE_DDR_MODE,BIT07|BIT08|BIT12|BIT13|BIT14); // bypass mode
		FCIE_RIU_16_ON(FCIE_DDR_MODE, BIT13|BIT14); //// GPIO SDR mode
		FCIE_RIU_16_ON(reg_sdiopll_0x36, 0x3000);
		FCIE_RIU_16_ON(reg_sdiopll_0x36,BIT5);		//power down LDO
		mdelay(1);
		FCIE_RIU_16_OF(reg_sdiopll_0x36,BIT5);		//turn on LDO
	   	FCIE_RIU_16_OF(reg_sdiopll_0x68,BIT00|BIT01);
	   	FCIE_RIU_16_OF(reg_sdiopll_0x6d,BIT00);
	   	FCIE_RIU_16_ON(reg_sdiopll_0x6f,BIT00|BIT01);
	   	FCIE_RIU_16_OF(reg_sdiopll_0x70,BIT08);
	   	FCIE_RIU_16_ON(reg_sdiopll_0x71,0xffff);
	   	FCIE_RIU_16_ON(reg_sdiopll_0x73,0xffff);
	   	FCIE_RIU_16_ON(reg_sdiopll_0x74,BIT15);
	    //printk("R_CLKGEN_FCIE = 0x%04X\r\n", FCIE_RIU_R16(R_CLKGEN_FCIE));
	    //FCIE_RIU_W16(R_CLKGEN_FCIE, 0x0034); // 300K
	}
	else if(ePAD==0) // FCIE connect to PAD_SD_CLK, PAD_SD_CMD, PAD_SD_DX
	{
		//printk("switch pad hal for FCIE connect to PAD_SD...\n");
		//printk("%08Xh\n", A_DDR_MOD_REG(0));
		//printk("%08Xh\n", A_DDR_MOD_REG(1));

		//FCIE_RIU_16_OF(CHIPTOP_6E, BIT6|BIT7); // disable reg_emmc_config

		FCIE_RIU_16_OF(CHIPTOP_50, BIT15);

		FCIE_RIU_16_OF(CHIPTOP_5A, BIT8|BIT9);
		FCIE_RIU_16_ON(CHIPTOP_5A, BIT8);

		FCIE_RIU_16_OF(A_DDR_MOD_REG(1), BIT7|BIT8|BIT12|BIT13|BIT14);
	}
}

void GoSDR50(int scan)
{
	if (scan > 17)
	{
		printk("scan too large\n");
		return;
	}

	#if (defined SDBUS) && (SDBUS==SDR50)
		printk("go SDR50:%x\n",scan);
	#elif (defined SDBUS) && (SDBUS==SDR104)
		printk("go SDR104:%x\n",scan);
	#endif

	FCIE_RIU_16_OF(FCIE_DDR_MODE, BIT07|BIT08); //
	FCIE_RIU_16_ON(FCIE_DDR_MODE, BIT12); // 32 bit marco  mode
	FCIE_RIU_W16(FCIE_DDR_TOGGLE_CNT, 0x0410);


	FCIE_RIU_16_OF(reg_sdiopll_0x6c,0xf0);		//clr bit 4-7
	if (scan >8)
	{
     	  FCIE_RIU_16_ON(reg_sdiopll_0x6c, BIT07);
	   scan-=9;
	}

	FCIE_RIU_W16(reg_sdiopll_0x03, 0);
	FCIE_RIU_16_ON(reg_sdiopll_0x03, (scan  <<12));

	FCIE_RIU_16_ON(reg_sdiopll_0x1a,BIT00);

	FCIE_RIU_16_OF(reg_sdiopll_0x1d,BIT04|BIT05|BIT06);
	FCIE_RIU_16_ON(reg_sdiopll_0x1d,BIT05);

   	FCIE_RIU_16_OF(reg_sdiopll_0x68,BIT00|BIT01);
   	FCIE_RIU_16_ON(reg_sdiopll_0x68,BIT00);			//8 but marco
   	FCIE_RIU_16_OF(reg_sdiopll_0x69,0xf0);			//tuning
   	FCIE_RIU_16_ON(reg_sdiopll_0x69,0x40);			//tuning

   	FCIE_RIU_16_ON(reg_sdiopll_0x6a,BIT00);
   	FCIE_RIU_16_OF(reg_sdiopll_0x6a,BIT01);

   	FCIE_RIU_W16(reg_sdiopll_0x6b,0x413);
   	FCIE_RIU_16_OF(reg_sdiopll_0x6d,BIT00);
   	FCIE_RIU_16_ON(reg_sdiopll_0x6f,BIT00|BIT01);
   	FCIE_RIU_16_ON(reg_sdiopll_0x70,BIT08);
   	FCIE_RIU_W16(reg_sdiopll_0x71,0);
   	FCIE_RIU_16_ON(reg_sdiopll_0x73,0xffff);
   	FCIE_RIU_16_ON(reg_sdiopll_0x74,BIT15);

	#if (defined SDBUS) && (SDBUS==SDR104)
		sdio_pll_setting(eMMC_PLL_CLK_200M);
	#elif (defined SDBUS) && (SDBUS==SDR50)
		sdio_pll_setting(eMMC_PLL_CLK_100M);
	#endif

}

void GoDDR50(int scan)
{
	if (scan > 17)
	{
		printk("scan too large\n");
		return;
	}
	printk("go DDR50:%x\n",scan);
	FCIE_RIU_16_OF(FCIE_DDR_MODE, BIT07); //
	FCIE_RIU_16_ON(FCIE_DDR_MODE, BIT08|BIT12); // 32 bit marco  mode
	FCIE_RIU_W16(FCIE_DDR_TOGGLE_CNT, 0x210);


	FCIE_RIU_16_OF(reg_sdiopll_0x6c,0xf0);		//clr bit 4-7
	if (scan >8)
	{
     	  FCIE_RIU_16_ON(reg_sdiopll_0x6c, BIT07);
	   scan-=9;
	}

	FCIE_RIU_W16(reg_sdiopll_0x03,0);
	FCIE_RIU_16_ON(reg_sdiopll_0x03, BIT06);

	FCIE_RIU_16_ON(reg_sdiopll_0x03, (scan  <<12));

	FCIE_RIU_16_ON(reg_sdiopll_0x1a,BIT00);		//driving

	FCIE_RIU_16_OF(reg_sdiopll_0x1d,BIT04|BIT05|BIT06);
	//FCIE_RIU_16_ON(reg_sdiopll_0x1d,BIT05);

   	FCIE_RIU_16_ON(reg_sdiopll_0x68,BIT00|BIT01);			//8 but marco
   	FCIE_RIU_16_OF(reg_sdiopll_0x69,0xf0);			//tuning
   	FCIE_RIU_16_ON(reg_sdiopll_0x69,0x40);			//tuning

   	FCIE_RIU_16_ON(reg_sdiopll_0x6a,BIT00);
   	FCIE_RIU_16_OF(reg_sdiopll_0x6a,BIT01);

   	FCIE_RIU_W16(reg_sdiopll_0x6b,0x213);
   	FCIE_RIU_16_OF(reg_sdiopll_0x6d,BIT00);
   	FCIE_RIU_16_ON(reg_sdiopll_0x6f,BIT00|BIT01);
   	FCIE_RIU_16_ON(reg_sdiopll_0x70,BIT08);
   	FCIE_RIU_W16(reg_sdiopll_0x71,0);
   	FCIE_RIU_16_ON(reg_sdiopll_0x73,0xffff);
   	FCIE_RIU_16_ON(reg_sdiopll_0x74,BIT15);

	sdio_pll_setting(eMMC_PLL_CLK__52M);
}

//***********************************************************************************************************
// Clock Setting for Card Platform
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
	//printk(CYAN"Init GPIO %d\n"NONE, eGPIO);

    if( eGPIO==EV_GPIO2 ) // for FCIE, PAD_SD_XXX
    {
		if (bEnable)
		{
			FCIE_RIU_16_ON(PMGPIO_REG_07, PMGPIO7_OEN); // config as input
			FCIE_RIU_16_OF(PMGPIO_REG_07, PMGPIO7_FIQ_MASK); // FIQ enable
		}
		else
		{
			FCIE_RIU_16_ON(PMGPIO_REG_07, PMGPIO7_FIQ_MASK); // FIQ mask
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
	//printk(CYAN"Get GPIO state %d\n"NONE, eGPIO);

	if( eGPIO==EV_GPIO2 ) // FCIE
	{
		if (FCIE_RIU_R16(PMGPIO_REG_07) & PMGPIO7_FIQ_POLARITY) // POLARITY decide
		{
			if (FCIE_RIU_R16(PMGPIO_REG_07) & PMGPIO7_FIQ_RAW_STS)
				return TRUE;
			else
				return FALSE;
		}
		else
		{
			if (FCIE_RIU_R16(PMGPIO_REG_07) & PMGPIO7_FIQ_RAW_STS)
				return FALSE;
			else
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

    if( eGPIO==EV_GPIO2 ) //EV_GPIO1 for Slot 0
    {
        if(eGPIOOPT==EV_GPIO_OPT1)
        {
        }
        else if((eGPIOOPT==EV_GPIO_OPT2))
        {
        }
        else if((eGPIOOPT==EV_GPIO_OPT3))
        {
			FCIE_RIU_16_OF(PMGPIO_REG_07, PMGPIO7_FIQ_POLARITY); // polarity 0, 0->1
        }
        else if((eGPIOOPT==EV_GPIO_OPT4))
        {
 			FCIE_RIU_16_ON(PMGPIO_REG_07, PMGPIO7_FIQ_POLARITY); // polarity 0, 0->1
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

	//printk(CYAN"Set GPIO int filter %d\n"NONE, eGPIO);

    if( eGPIO==EV_GPIO2 ) //EV_GPIO1 for Slot 0
    {
		if( (FCIE_RIU_R16(PMGPIO_REG_07) & PMGPIO7_FIQ_MASK) == 0)
		{
			if(FCIE_RIU_R16(PMGPIO_REG_07) & PMGPIO7_FIQ_RAW_STS)
			{
         	   bRet = TRUE;
			}
		}
    }

    // clear FIQ anyway
    FCIE_RIU_16_ON(PMGPIO_REG_07, PMGPIO7_FIQ_CLR);

    return bRet;
}


void ClockBeginSetting(void)
{
	FCIE_RIU_16_OF(CLKGEN_SDIO, 0x3F);

	// bit6 --> clock source select, 0: clk_xtal(12M), 1: clk_nfie_p1
	FCIE_RIU_16_ON(CLKGEN_SDIO, BIT06);
}


void ClockClock(void)
{
	FCIE_RIU_16_ON(CLKGEN_SDIO, BIT00);		//clock gating
}

void HalCard_SetCardPower(U8_T u8OnOff)
{
	// TODO: Add power control
}


