//#include "hal_card_common.h"
#include "hal_card_regs5.h"
#include "sd_platform.h"
#include "hal_card_common.h"
#include <linux/delay.h>
#include "hal_sdmmc.h"
#include "hal_card_timer.h"

#define A_SD_REG_BANK(IP)			GET_CARD_BANK(IP, EV_REG_BANK)
#define A_MIE_FUNC_CTL(IP)			GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x07)
#define A_TEST_MODE(IP)				GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x15)
#define A_DDR_MOD_REG(IP)			GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x0F)
#define A_DEBUG_BUS_1(IP)			GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x39)
#define A_NC_FUNC_CTL(IP)			GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x63)

#ifdef SDIO30
U32 sdio_pll_setting(U16_T u16_ClkParam)
{
	static U16_T u16_OldClkParam=0xFFFF;
	U32 u32_value_reg_emmc_pll_pdiv;

	if(u16_ClkParam == u16_OldClkParam)
		return 0;
	else
		u16_OldClkParam = u16_ClkParam;

	// 1. reset emmc pll
	FCIE_RIU_16_ON(reg_emmc_pll_reset, BIT0);
	FCIE_RIU_16_OF(reg_emmc_pll_reset, BIT0);

	// 2. synth clock
	switch(u16_ClkParam) {
	    case eMMC_PLL_CLK_200M: // 200M
			FCIE_RIU_W16(reg_ddfset_23_16, 0x0022);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x8F5C);
			// 195MHz
			//FCIE_RIU_W16(reg_ddfset_23_16, 0x0024);
			//FCIE_RIU_W16(reg_ddfset_15_00, 0x03D8);
			u32_value_reg_emmc_pll_pdiv =1;// PostDIV: 2
			break;
		case eMMC_PLL_CLK_160M:
			FCIE_RIU_W16(reg_ddfset_23_16, 0x002B);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x3333);
			u32_value_reg_emmc_pll_pdiv =1;// PostDIV: 2
			break;
		case eMMC_PLL_CLK_140M:
			FCIE_RIU_W16(reg_ddfset_23_16, 0x0031);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x5F15);
			u32_value_reg_emmc_pll_pdiv = 1;// PostDIV: 2
			break;
		case eMMC_PLL_CLK_120M:
			FCIE_RIU_W16(reg_ddfset_23_16, 0x0039);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x9999);
			u32_value_reg_emmc_pll_pdiv = 1;// PostDIV: 2
			break;
		case eMMC_PLL_CLK_100M:
			FCIE_RIU_W16(reg_ddfset_23_16, 0x0045);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x1EB8);
			u32_value_reg_emmc_pll_pdiv = 1;// PostDIV: 2
			break;
		case eMMC_PLL_CLK__86M:
			FCIE_RIU_W16(reg_ddfset_23_16, 0x0028);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x2FA0);
			u32_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
			break;
		case eMMC_PLL_CLK__80M:
			FCIE_RIU_W16(reg_ddfset_23_16, 0x002B);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x3333);
			u32_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
			break;
		case eMMC_PLL_CLK__72M:
			FCIE_RIU_W16(reg_ddfset_23_16, 0x0030);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x0000);
			u32_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
			break;
		case eMMC_PLL_CLK__62M:
			FCIE_RIU_W16(reg_ddfset_23_16, 0x0037);
			FCIE_RIU_W16(reg_ddfset_15_00, 0xBDEF);
			u32_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
			break;
		case eMMC_PLL_CLK__52M:
			FCIE_RIU_W16(reg_ddfset_23_16, 0x0042);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x7627);
			u32_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
			break;
		case eMMC_PLL_CLK__48M:
			FCIE_RIU_W16(reg_ddfset_23_16, 0x0048);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x0000);
			u32_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
			break;
		case eMMC_PLL_CLK__40M:
			FCIE_RIU_W16(reg_ddfset_23_16, 0x002B);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x3333);
			u32_value_reg_emmc_pll_pdiv = 4;// PostDIV: 8
			break;
		case eMMC_PLL_CLK__36M:
			FCIE_RIU_W16(reg_ddfset_23_16, 0x0030);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x0000);
			u32_value_reg_emmc_pll_pdiv = 4;// PostDIV: 8
			break;
		case eMMC_PLL_CLK__32M:
			FCIE_RIU_W16(reg_ddfset_23_16, 0x0036);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x0000);
			u32_value_reg_emmc_pll_pdiv = 4;// PostDIV: 8
			break;
		case eMMC_PLL_CLK__27M:
			FCIE_RIU_W16(reg_ddfset_23_16, 0x0040);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x0000);
			u32_value_reg_emmc_pll_pdiv = 4;// PostDIV: 8
			break;
		case eMMC_PLL_CLK__20M:
			FCIE_RIU_W16(reg_ddfset_23_16, 0x002B);
			FCIE_RIU_W16(reg_ddfset_15_00, 0x3333);
			u32_value_reg_emmc_pll_pdiv = 7;// PostDIV: 16
			break;
		default:
			printk("SD Err: emmc PLL not configed %x\n", u16_ClkParam);
			return 0xff;
			break;
	}

	// 3. VCO clock ( loop N = 4 )
	FCIE_RIU_16_OF(reg_sdiopll_fbdiv, 0xFFFF);
	FCIE_RIU_16_ON(reg_sdiopll_fbdiv, 0x6);// PostDIV: 8

	// 4. 1X clock
	FCIE_RIU_16_OF(reg_sdiopll_pdiv, BIT2|BIT1|BIT0);
	FCIE_RIU_16_ON(reg_sdiopll_pdiv, u32_value_reg_emmc_pll_pdiv);// PostDIV: 8

	if(u16_ClkParam==eMMC_PLL_CLK__20M) {
	    FCIE_RIU_16_ON(reg_emmc_pll_test, BIT10);
	}
	else {
		FCIE_RIU_16_OF(reg_emmc_pll_test, BIT10);
	}

	mdelay(1); // asked by Irwin

	return 0;
}

void sd_go_18v(void)
{
	U32 u32_i = 0;

	//--- switch to 1.8V----
	FCIE_RIU_16_ON(R_SDIO_PLL_0x1D,BIT15);
	FCIE_RIU_16_OF(R_SDIO_PLL_0x37,0xff00);
	FCIE_RIU_16_ON(R_SDIO_PLL_0x37,0x3000);		//OSP count =0x30

	FCIE_RIU_16_ON(R_SDIO_PLL_0x37,BIT05);		// Turning on LDO
	mdelay(10);			//delay 10ms
	FCIE_RIU_16_OF(R_SDIO_PLL_0x37,BIT05);		// Turning on LDO

//	while (1)
	//{
	//	if (FCIE_RIU_R16(SDIOPLL_10) & BIT12) break;
	//	printk("wait 3.3 V ready\n");
//	}

	FCIE_RIU_16_ON(R_SDIO_PLL_0x37,BIT02|BIT04);		// enable 1.8V

	while (1)
	{
		if (FCIE_RIU_R16(R_SDIO_PLL_0x10) & BIT13) break;
		mdelay(1); // delay 1ms
		if(u32_i++>1000)
		{
			printk("timeout to set 1.8V\n");
			return;
		}
		
	}

	printk("1.8V is ready\n");

}

void GoSDR50(int scan)			//SDR50 or 104
{
	if (scan > 17)
	{
		printk("scan too large\n");
		return;
	}

//	#if (defined SDBUS) && (SDBUS==SDR50)
		printk("go SDR50/104:%x\n",scan);
	//#elif (defined SDBUS) && (SDBUS==SDR104)
	//	printk("go SDR104:%x\n",scan);
	//#endif

	FCIE_RIU_16_OF(FCIE_DDR_MODE, BIT07|BIT08); //
	FCIE_RIU_16_ON(FCIE_DDR_MODE, BIT12); // 32 bit marco  mode
	
	FCIE_RIU_W16(R_SDIO_PLL_0x03,0);

	if(scan<9)
	{
		FCIE_RIU_16_OF(R_SDIO_PLL_0x69, BIT11); // inverse of CLK_SKEW_LAT
		FCIE_RIU_16_ON(R_SDIO_PLL_0x03, scan<<12);
	}
	else
	{
		FCIE_RIU_16_ON(R_SDIO_PLL_0x69, BIT11);
		FCIE_RIU_16_ON(R_SDIO_PLL_0x03, (scan-9)<<12);
	}

	FCIE_RIU_16_ON(R_SDIO_PLL_0x1A,BIT00);
	FCIE_RIU_16_ON(R_SDIO_PLL_0x1C,BIT08);
	FCIE_RIU_16_OF(R_SDIO_PLL_0x1D,0x7E);
	FCIE_RIU_16_ON(R_SDIO_PLL_0x1D,0x22);
			
	FCIE_RIU_16_OF(R_SDIO_PLL_0x63,BIT00);

			
	FCIE_RIU_16_ON(R_SDIO_PLL_0x68,BIT00);
	FCIE_RIU_16_OF(R_SDIO_PLL_0x68,BIT01);
			
	FCIE_RIU_16_OF(R_SDIO_PLL_0x69,0xF7);		//clr bit 3-7
	FCIE_RIU_16_ON(R_SDIO_PLL_0x69,BIT06);	         // set 4
		
	FCIE_RIU_16_ON(R_SDIO_PLL_0x6A,BIT00);
	FCIE_RIU_16_OF(R_SDIO_PLL_0x6A,BIT01);

	FCIE_RIU_W16(R_SDIO_PLL_0x6B,0x413);
	FCIE_RIU_16_OF(R_SDIO_PLL_0x6D,BIT00);
	FCIE_RIU_16_ON(R_SDIO_PLL_0x6F,3);
	FCIE_RIU_16_ON(R_SDIO_PLL_0x70,BIT08);
		
	FCIE_RIU_W16(R_SDIO_PLL_0x71,0);
	FCIE_RIU_W16(R_SDIO_PLL_0x73,0xffff);
			
	FCIE_RIU_16_ON(R_SDIO_PLL_0x74,BIT15);


	FCIE_RIU_16_ON(CLKGEN_SDIO, 0x0b<<2);   //select clk_1x_p

	//#if (defined SDBUS) && (SDBUS==SDR104)
	//	sdio_pll_setting(eMMC_PLL_CLK_200M);
	//#elif (defined SDBUS) && (SDBUS==SDR50)
	//	sdio_pll_setting(eMMC_PLL_CLK_100M);
	//#endif
	// CLOCK already set
	
}

void GoDDR50(int scan)
{
	if (scan > 17)
	{
		printk("scan too large\n");
		return;
	}
	printk("go DDR50:%x\n",scan);

			FCIE_RIU_16_ON(FCIE_DDR_MODE, BIT_8BIT_MACRO_EN|BIT_DDR_EN);
			FCIE_RIU_16_OF(FCIE_DDR_MODE, BIT_32BIT_MACRO_EN);

			FCIE_RIU_16_OF(R_SDIO_PLL_0x1A,BIT00);
			FCIE_RIU_16_ON(R_SDIO_PLL_0x1C,BIT09);
			FCIE_RIU_16_OF(R_SDIO_PLL_0x1D,0x7E);
			FCIE_RIU_16_OF(R_SDIO_PLL_0x63,BIT00);

			
			FCIE_RIU_16_ON(R_SDIO_PLL_0x68,BIT00);
			FCIE_RIU_16_OF(R_SDIO_PLL_0x68,BIT01);
			FCIE_RIU_16_OF(R_SDIO_PLL_0x69,BIT03);
			FCIE_RIU_16_ON(R_SDIO_PLL_0x6A,BIT00);
			FCIE_RIU_16_OF(R_SDIO_PLL_0x6A,BIT01);

			FCIE_RIU_W16(R_SDIO_PLL_0x6B,0x213);
			FCIE_RIU_16_ON(R_SDIO_PLL_0x6D,BIT00);
			FCIE_RIU_16_ON(R_SDIO_PLL_0x6F,3);
			FCIE_RIU_16_OF(R_SDIO_PLL_0x70,BIT08);
			
			FCIE_RIU_W16(R_SDIO_PLL_0x71,0xffff);
			FCIE_RIU_W16(R_SDIO_PLL_0x73,0xffff);
			
			FCIE_RIU_16_ON(R_SDIO_PLL_0x74,BIT15);



	sdio_pll_setting(eMMC_PLL_CLK__52M);
}

#endif

void Hal_CARD_SerDriving(U8_T u8Driving)
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

	// bring up note, chiptio related register
	FCIE_RIU_16_OF(CHIPTOP_50, BIT15); // all_pad_in


	if(eIP==0) // SDIO
	{
		FCIE_RIU_16_OF(A_DDR_MOD_REG(eIP), BIT7|BIT8|BIT12);
		FCIE_RIU_16_ON(A_DDR_MOD_REG(eIP), BIT10|BIT13|BIT14);

		//if( !(FCIE_RIU_R16(MIU2_7A) & MIU_SELECT_BY_IP))
		//{
		///	printk(LIGHT_RED"SdWarn: MIU select not enable for SDIO IP\n"NONE);
		//	FCIE_RIU_16_ON(MIU2_7A, MIU_SELECT_BY_IP);	// miu select by IP
		//}

		//Hal_CARD_SerDriving(0);

		//printk(LIGHT_RED"IP begin setting %d\n"NONE, eIP);

		FCIE_RIU_W16(A_MIE_FUNC_CTL(eIP), BIT_SDIO_MOD);

	#if 1
		// reg_sdio_config = 3 --> MST215A
		// PAD_EMMC_IO13, PAD_EMMC_IO12, PAD_EMMC_IO14, PAD_EMMC_IO15, PAD_EMMC_IO16, PAD_EMMC_IO17
		//FCIE_RIU_16_ON(CHIPTOP_7B, BIT04|BIT05);
		FCIE_RIU_16_OF(CHIPTOP_08, BIT10|BIT11);
		FCIE_RIU_16_ON(CHIPTOP_08, BIT10);
		FCIE_RIU_16_OF(CHIPTOP_50, BIT15);

	#else
			// reg_sdio_config = 1
		// PAD_PCM2_CE_N, PAD_PCM2_IRQA_N, PAD_PCM_A[8], PAD_PCM_A[9], PAD_PCM_A[10], PAD_PCM_A[11]
		FCIE_RIU_16_ON(CHIPTOP_7B, BIT04);
		FCIE_RIU_16_OF(CHIPTOP_7B, BIT05);
	#endif

	//SDR GPIO pad
			// emmc pll bank setting
		FCIE_RIU_16_OF(EMMC_PLL_REG_1A, BIT00|BIT10); // reg_emmc_en = 0, reg_emmc_ddr_en = 0
		FCIE_RIU_16_ON(EMMC_PLL_REG_1A, BIT10); // reg_emmc_en = 0, reg_emmc_ddr_en = 0

	//	FCIE_RIU_16_ON(EMMC_PLL_REG_1C, BIT09); // reg_emmc_en = 0, reg_emmc_ddr_en = 0
		FCIE_RIU_16_OF(EMMC_PLL_REG_1D, 0x7e); // reg_emmc_en = 0, reg_emmc_ddr_en = 0

		FCIE_RIU_16_OF(EMMC_PLL_REG_63, BIT00); // reg_emmc_en = 0, reg_emmc_ddr_en = 0

		FCIE_RIU_16_OF(EMMC_PLL_REG_68, BIT01|BIT00); // reg_emmc_en = 0, reg_emmc_ddr_en = 0

		FCIE_RIU_16_OF(EMMC_PLL_REG_69, BIT03);

	//	FCIE_RIU_16_OF(EMMC_PLL_REG_6A, BIT00|BIT01);
	//	FCIE_RIU_16_ON(EMMC_PLL_REG_6A, BIT00);

	//	FCIE_RIU_W16(  EMMC_PLL_REG_6B, 0x413);

		FCIE_RIU_16_OF(EMMC_PLL_REG_6D, BIT00); // reg_ddr_io_mode = 0
		FCIE_RIU_16_ON(EMMC_PLL_REG_6F, BIT00|BIT01);


		FCIE_RIU_16_OF(EMMC_PLL_REG_70, BIT08);

		FCIE_RIU_W16(  EMMC_PLL_REG_71, 0xFFFF);
		FCIE_RIU_W16(  EMMC_PLL_REG_73, 0xFFFF);
		FCIE_RIU_16_ON(EMMC_PLL_REG_74, BIT15);

	}
	else if(eIP==1) // FCIE
	{

		FCIE_RIU_16_OF(CHIPTOP_08, BIT08|BIT09|BIT10|BIT11);
		FCIE_RIU_16_ON(CHIPTOP_08, BIT08);
		FCIE_RIU_16_OF(CHIPTOP_50, BIT15);

		//printk(YELLOW"IP begin setting %d\n"NONE, eIP);
		FCIE_RIU_16_OF(A_DDR_MOD_REG(eIP), BIT7|BIT8|BIT12);
		FCIE_RIU_16_ON(A_DDR_MOD_REG(eIP), BIT13|BIT14);

		FCIE_RIU_W16(A_MIE_FUNC_CTL(eIP), BIT_SD_EN); // clear BIT_EMMC_ACTIVE by the way
		FCIE_RIU_16_OF(A_NC_FUNC_CTL(eIP),BIT0);		//clear nand en
		// reg_sd_config = 2 --> MST215C & 072B
		// PAD_PCM2_RESET, PAD_PCM2_WAIT_N, PAD_PCM_A[4], PAD_PCM_A[5], PAD_PCM_A[6], PAD_PCM_A[7]
		//FCIE_RIU_16_OF(CHIPTOP_08, BIT04|BIT05|BIT08|BIT09|BIT00);
		//FCIE_RIU_16_ON(CHIPTOP_08, BIT08);

		//FCIE_RIU_16_OF(CHIPTOP_64, BIT00|BIT03|BIT04);
		//FCIE_RIU_16_ON(CHIPTOP_0A, BIT07|BIT06|BIT05|BIT04);
		//FCIE_RIU_16_OF(CHIPTOP_6F, BIT07|BIT06);

		//FCIE_RIU_16_ON(CHIPTOP_5A, BIT09); // sd_config = 2
		//FCIE_RIU_16_OF(CHIPTOP_6E, BIT07|BIT06); // emmc_config = 0

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
		//printk(LIGHT_RED"SDIO Init GPIO\n"NONE);
		if (bEnable)
		{
			FCIE_RIU_16_OF(GPIO_SAR1_SETTING, GPIO_SAR1_EN); // 
			FCIE_RIU_16_ON(GPIO_SAR1_SETTING, GPIO_SAR1_OEN); // set OE for input
		//printk("SAR GPIO:%x\n",FCIE_RIU_R16(GPIO_SAR1_SETTING));

		}
		else
		{
		}
    }
	else if( eGPIO==EV_GPIO2 )
	{
		//printk(YELLOW"FCIE Init GPIO\n"NONE);
		if (bEnable)
		{
			FCIE_RIU_16_ON(PMGPIO_REG_07, PMGPIO7_OEN); // set oen to high for input
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
		//printk(LIGHT_RED"SDIO Get GPIO\n"NONE);
	//	printk("SDIO Get GPIO:%x\n",FCIE_RIU_R16(GPIO_SAR1));
	
		if (FCIE_RIU_R16(GPIO_SAR1) & GPIO_SAR1_IN)
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
		//printk(YELLOW"FCIE Get GPIO\n"NONE);
		if (FCIE_RIU_R16(PMGPIO_REG_07) & PMGPIO7_IN)
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

#define VOLT_00V 0
#define VOLT_33V 1
#define VOLT_18V 2

// must set 3.3V fisrt, then switch to 1.8V

void HalSdio_SetIOVoltage(U8_T u8Volt)
{
	U32 u32_i = 0;
	static U8_T only_need_run_once = 0;
	static U8_T ldo_on_off_status = 0;

	if(!only_need_run_once)
	{
		// 1. disable OSP (short protection curcuit related)
		FCIE_RIU_16_ON(EMMC_PLL_REG_1D, BIT15);

		// 2. OSP count = 0x30
		FCIE_RIU_16_OF(EMMC_PLL_REG_37, 0xFF00);
		FCIE_RIU_16_ON(EMMC_PLL_REG_37, 0x3000);

		only_need_run_once = 1;
	}

	if(u8Volt==VOLT_00V)
	{
		ldo_on_off_status = 0;
		FCIE_RIU_16_ON(EMMC_PLL_REG_37, BIT05);		// Turning off LDO
	}
	else
	{
		if(!ldo_on_off_status) // LDO is off
		{
			// 3. tunning on LDO
			FCIE_RIU_16_ON(EMMC_PLL_REG_37, BIT05);		// Turning off LDO
			mdelay(10);							// delay 10ms
			FCIE_RIU_16_OF(EMMC_PLL_REG_37, BIT05);		// Turning on LDO
			ldo_on_off_status = 1;
		}

		// 4. check if 3.3V ready
		if(u8Volt==VOLT_33V)
		{
			FCIE_RIU_16_OF(EMMC_PLL_REG_37, BIT02|BIT04);		// set to 3.3V

			//printf("wait 3.3V ready\n");

			while(1)
			{
				if (FCIE_RIU_R16(EMMC_PLL_REG_37) & BIT12) break;
				//printf(".");

				mdelay(1); // delay 1ms
				if(u32_i++>1000)
				{
					printk("timeout to set 3.3V\n");
					//while(1);
				}
			}
		}
		// 5. check if 1.8V ready
		else if(u8Volt==VOLT_18V)
		{
			FCIE_RIU_16_ON(EMMC_PLL_REG_37, BIT02|BIT04);		// set to 1.8V

			//printf("wait 1.8V ready");

			while(1)
			{
				if (FCIE_RIU_R16(EMMC_PLL_REG_10) & BIT13) break;
				//printf(".");

				mdelay(1); // delay 1ms
				if(u32_i++>1000)
				{
					printk("timeout to set 1.8V\n");
					//while(1);
				}
			}

			//printf("1.8V is ready\n");
		}
	}

}
void HalCard_SetCardPower(PADEmType ePAD, U8_T u8OnOff)
{
	if(ePAD==EV_PAD1)
	{

		if(u8OnOff)
		{
			HalSdio_SetIOVoltage(VOLT_33V);
				
		}
		else
		{
			HalSdio_SetIOVoltage(VOLT_00V);
		}
	}
	else if(ePAD==EV_PAD2)
	{
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

void HalCard_ControlGPIO(U8_T u8GPIO, U8_T u8HighLow) // 1, 2, 14, 65
{
	if(u8GPIO==1) // J2 PIN 3
	{
		FCIE_RIU_16_OF(PMGPIO_REG_01, OEN);

		if(u8HighLow)
		{
			FCIE_RIU_16_ON(PMGPIO_REG_01, OUT);
		}
		else
		{
			FCIE_RIU_16_OF(PMGPIO_REG_01, OUT);
		}
	}
	else if(u8GPIO==2) // J2 PIN 2
	{
		FCIE_RIU_16_OF(PMGPIO_REG_02, OEN);

		if(u8HighLow)
		{
			FCIE_RIU_16_ON(PMGPIO_REG_02, OUT);
		}
		else
		{
			FCIE_RIU_16_OF(PMGPIO_REG_02, OUT);
		}
	}
	else if(u8GPIO==14) // PAD_PM_GPIO14 J2 PIN 7
	{
		FCIE_RIU_16_OF(PMGPIO_REG_0E, OEN);

		if(u8HighLow)
		{
			FCIE_RIU_16_ON(PMGPIO_REG_0E, OUT);
		}
		else
		{
			FCIE_RIU_16_OF(PMGPIO_REG_0E, OUT);
		}
	}
	else if(u8GPIO==65) // PAD_SD_D1 J2 PIN 4
	{
		FCIE_RIU_16_OF(CHIP_GPIO_55, GPIO_OEN);

		if(u8HighLow)
		{
			FCIE_RIU_16_ON(CHIP_GPIO_55, GPIO_OUT);
		}
		else
		{
			FCIE_RIU_16_OF(CHIP_GPIO_55, GPIO_OUT);
		}
	}
	else
	{
		printk(LIGHT_RED"wrong GPIO index!!!\n"NONE);
	}
}

void HalCardTestBus(void)
{
	// BANK SDIO
	FCIE_RIU_16_OF(A_TEST_MODE(0), BIT03|BIT02|BIT01);
	FCIE_RIU_16_ON(A_TEST_MODE(0), BIT03|BIT02); // 6

	FCIE_RIU_16_OF(A_DEBUG_BUS_1(0), BIT11|BIT10|BIT09|BIT08);
	FCIE_RIU_16_ON(A_DEBUG_BUS_1(0), BIT10|BIT08);

	// BANK CHIPTOP
	FCIE_RIU_16_OF(CHIPTOP_12, BIT05|BIT04);
	FCIE_RIU_16_ON(CHIPTOP_12, BIT05); // reg_test_out_mode = 2
	FCIE_RIU_16_OF(CHIPTOP_75, BIT07|BIT06|BIT05|BIT03);
	FCIE_RIU_16_ON(CHIPTOP_75, BIT14);
	FCIE_RIU_16_OF(CHIPTOP_76, BIT02|BIT01|BIT00);
	FCIE_RIU_16_OF(CHIPTOP_77, BIT05|BIT04|BIT03|BIT02|BIT01|BIT00);

	// BANK ONEWAY
	FCIE_RIU_16_OF(ONEWAY_70, 0x00FF);
	FCIE_RIU_16_ON(ONEWAY_70, 0x0014);
}

