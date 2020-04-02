//=======================================================================
//  MStar Semiconductor - Unified Nand Flash Driver
//
//  drvNAND_platform.c - Storage Team, 2009/08/20
//
//  Design Notes: defines common platform-dependent functions.
//
//    1. 2009/08/25 - support C5 eCos platform
//
//=======================================================================
#if defined(CONFIG_ARM)
#include <mstar/mstar_chip.h>

#ifdef REG
#undef REG
#endif

#endif

#include "drvNAND.h"
#include "chip_int.h"
#include "chip_setup.h"
#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,20) && defined(CONFIG_MIPS))
#include  <linux/sched.h>
#endif

//=============================================================
//=============================================================
#if (defined(NAND_DRV_TV_LINUX)&&NAND_DRV_TV_LINUX)

#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/dma-mapping.h>

struct mstar_nand_info{
	struct mtd_info nand_mtd;
	struct platform_device *pdev;
	struct nand_chip	nand;
	struct mtd_partition	*parts;
};
extern struct mstar_nand_info *info;

NAND_DRIVER sg_NandDrv;

static UNFD_ALIGN0 U32 gau32_PartInfo[NAND_PARTITAION_BYTE_CNT/4]UNFD_ALIGN1;
static u16 fciepad = 0;

U32 nand_hw_timer_delay(U32 u32usTick)
{
    #if 0	// Use PIU timer

	U32 u32HWTimer = 0;
	volatile U16 u16TimerLow = 0;
	volatile U16 u16TimerHigh = 0;

	// reset HW timer
	REG_WRITE_UINT16(TIMER0_MAX_LOW, 0xFFFF);
	REG_WRITE_UINT16(TIMER0_MAX_HIGH, 0xFFFF);
	REG_WRITE_UINT16(TIMER0_ENABLE, 0);

	// start HW timer
	REG_SET_BITS_UINT16(TIMER0_ENABLE, 0x0001);

	while( u32HWTimer < 12*u32usTick ) // wait for u32usTick micro seconds
	{
		REG_READ_UINT16(TIMER0_CAP_LOW, u16TimerLow);
		REG_READ_UINT16(TIMER0_CAP_HIGH, u16TimerHigh);

		u32HWTimer = (u16TimerHigh<<16) | u16TimerLow;
	}

	REG_WRITE_UINT16(TIMER0_ENABLE, 0);

    #else	// Use kernel udelay
	if(u32usTick <= MAX_UDELAY_MS * 1000)
		udelay(u32usTick);
	else
		mdelay(u32usTick/1000);

    #endif

	return u32usTick+1;
}

void nand_CheckPowerCut(void)
{
}

U32 nand_pads_init(void)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();

	//Read CEZ Configure Setting from CEXZ GPIO.
	if((REG(REG_NAND_CS1_EN) & BIT_NAND_CS1_EN) == BIT_NAND_CS1_EN)
	{
		pNandDrv->u16_Reg40_Signal =
			(BIT_NC_CE1Z | BIT_NC_WP_AUTO | BIT_NC_WP_H | BIT_NC_CE_AUTO | BIT_NC_CE_H) &
			~(BIT_NC_CHK_RB_EDGEn);
	}
	else
	{
		pNandDrv->u16_Reg40_Signal =
			(BIT_NC_WP_AUTO | BIT_NC_WP_H | BIT_NC_CE_AUTO | BIT_NC_CE_H) &
			~(BIT_NC_CHK_RB_EDGEn | BIT_NC_CE_SEL_MASK);
	}

    return UNFD_ST_SUCCESS;
}

#if defined(FCIE4_DDR) && FCIE4_DDR
U32 nand_check_DDR_pad(void)
{
	//check nand mode for wether this mode supports DDR NAND
	U16 u16_NandMode;
	REG_READ_UINT16(reg_nf_en, u16_NandMode);

	u16_NandMode &= REG_NAND_MODE_MASK;

    if(u16_NandMode != NAND_MODE2)
    {
        nand_debug(0,1,"Nand pad type does not support DDR NAND\n");
        return UNFD_ST_ERR_PAD_UNSUPPORT_DDR_NAND;
    }

    return UNFD_ST_SUCCESS;
}
#endif

U32 nand_pads_switch(U32 u32EnableFCIE)
{
    NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();

	REG_WRITE_UINT16(NC_PATH_CTL, BIT_NC_EN);	//nand chip enable

    REG_CLR_BITS_UINT16(REG_EMMC_CONFIG, BIT7|BIT6);
    REG_CLR_BITS_UINT16(reg_sd_config, BIT9|BIT8);
    REG_CLR_BITS_UINT16(reg_sdio_config, BIT5|BIT4);
    REG_CLR_BITS_UINT16(reg_sd_use_bypass, BIT_SD_USE_BYPASS);

    //pad drv
    REG_CLR_BITS_UINT16(reg_pad_drv, BIT_PAD_DRV_MASK);
    REG_SET_BITS_UINT16(reg_pad_drv, 0x0FF & BIT_PAD_DRV_MASK);

    if( fciepad )
    {
        REG_SET_BITS_UINT16(reg_nf_en, fciepad);
    }
    else
    {
        fciepad = REG(reg_nf_en) & REG_NAND_MODE_MASK;
    }

    pNandDrv->u16_Reg58_DDRCtrl &= ~(BIT_DDR_MASM|BIT_SDR_DIN_FROM_MACRO);

    if(NAND_PAD_BYPASS_MODE == u32EnableFCIE)
    {
        REG_SET_BITS_UINT16(NC_REG_2Fh, BIT11);
        REG_SET_BITS_UINT16(reg_fcie2macro_sd_bypass, BIT_FCIE2MACRO_SD_BYPASS);
        NC_DQS_PULL_H();
    }
    else if(NAND_PAD_TOGGLE_MODE == u32EnableFCIE)
    {
        REG_CLR_BITS_UINT16(NC_REG_2Fh, BIT11);
        //for DDR disable BYPASS MODE
        REG_CLR_BITS_UINT16(reg_fcie2macro_sd_bypass, BIT_FCIE2MACRO_SD_BYPASS);
        NC_DQS_PULL_L();
        pNandDrv->u16_Reg58_DDRCtrl |= (BIT_DDR_TOGGLE|BIT_SDR_DIN_FROM_MACRO);
    }
    else if(NAND_PAD_ONFI_SYNC_MODE == u32EnableFCIE)
    {
        REG_CLR_BITS_UINT16(NC_REG_2Fh, BIT11);
        //for DDR disable BYPASS MODE
        REG_CLR_BITS_UINT16(reg_fcie2macro_sd_bypass, BIT_FCIE2MACRO_SD_BYPASS);
        NC_DQS_PULL_L();
        pNandDrv->u16_Reg58_DDRCtrl |= (BIT_DDR_ONFI|BIT_SDR_DIN_FROM_MACRO);
    }
    REG_WRITE_UINT16(NC_DDR_CTRL, pNandDrv->u16_Reg58_DDRCtrl);

	if(NAND_MODE1 == fciepad )
	{
		REG_SET_BITS_UINT16(reg_pcm_a_pe, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7);
	}
	else if(NAND_MODE2 == fciepad)
	{
		REG_SET_BITS_UINT16(reg_emmc_ps, 0xF700);
		REG_SET_BITS_UINT16(reg_ts2_d0, BIT8);
	}

	nand_debug(UNFD_DEBUG_LEVEL_LOW,1,"reg_nf_en(%08X)=%04X\n", reg_nf_en, REG(reg_nf_en));
	nand_debug(UNFD_DEBUG_LEVEL_LOW,1,"reg_allpad_in(%08X)=%04X\n", reg_allpad_in, REG(reg_allpad_in));

	return UNFD_ST_SUCCESS;
}

void nand_pads_release(void)
{
	//release pad setting for CI card default setting
	REG_CLR_BITS_UINT16(reg_pcm_a_pe, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7);
	REG_CLR_BITS_UINT16(reg_emmc_ps, 0xF700);
	REG_CLR_BITS_UINT16(reg_ts2_d0, BIT8);
}

#if defined(FCIE4_DDR_EMMC_PLL) && FCIE4_DDR_EMMC_PLL
void nand_pll_clock_setting(U32 u32EmmcClkParam)
{
    EMMC_PLL_SETTINGS sEmmcPLLSetting[EMMC_PLL_1XCLK_TABLE_CNT] = EMMC_PLL_CLK_TABLE;

    // Reset eMMC_PLL
    REG_SET_BITS_UINT16(REG_EMMC_PLL_RX06, BIT0);
    REG_CLR_BITS_UINT16(REG_EMMC_PLL_RX06, BIT0);

    // Synth clock
    REG_WRITE_UINT16(REG_EMMC_PLL_RX18, sEmmcPLLSetting[u32EmmcClkParam].emmc_pll_1xclk_rx18);
    REG_WRITE_UINT16(REG_EMMC_PLL_RX19, sEmmcPLLSetting[u32EmmcClkParam].emmc_pll_1xclk_rx19);

    // VCO clock
    REG_WRITE_UINT16(REG_EMMC_PLL_RX04, 0x0006);

    // 1X clock
    REG_CLR_BITS_UINT16(REG_EMMC_PLL_RX05, EMMC_PLL_1XCLK_RX05_MASK);
    REG_SET_BITS_UINT16(REG_EMMC_PLL_RX05, sEmmcPLLSetting[u32EmmcClkParam].emmc_pll_1xclk_rx05);

    if( u32EmmcClkParam )
        REG_CLR_BITS_UINT16(REG_EMMC_PLL_RX07, BIT10);
    else
        REG_SET_BITS_UINT16(REG_EMMC_PLL_RX07, BIT10);

    // Wait 100us
    udelay(1000);
}

void nand_skew_clock_setting(void)
{
    // Skew clock setting
    REG_WRITE_UINT16(REG_EMMC_PLL_RX03, 0x0040);
}

void nand_dll_setting(void)
{
	volatile U16 u16_reg;
	REG_CLR_BITS_UINT16(REG_EMMC_PLL_RX09, BIT0);

	// Reset eMMC_DLL
	REG_SET_BITS_UINT16(REG_EMMC_PLL_RX30, BIT2);
	REG_CLR_BITS_UINT16(REG_EMMC_PLL_RX30, BIT2);

	//DLL pulse width and phase
	REG_WRITE_UINT16(REG_EMMC_PLL_RX01, 0x7F72);

	// DLL code
	REG_WRITE_UINT16(REG_EMMC_PLL_RX32, 0xF200);

	// DLL calibration
	REG_WRITE_UINT16(REG_EMMC_PLL_RX30, 0x3378);
	REG_SET_BITS_UINT16(REG_EMMC_PLL_RX33, BIT15);

	// Wait 100us
	udelay(1000);

	// Get hw dll0 code
	REG_READ_UINT16(REG_EMMC_PLL_RX33, u16_reg);

	REG_CLR_BITS_UINT16(REG_EMMC_PLL_RX34, (BIT10 - 1));
	// Set dw dll0 code
	REG_SET_BITS_UINT16(REG_EMMC_PLL_RX34, u16_reg & 0x03FF);

	// Disable reg_hw_upcode_en
	REG_CLR_BITS_UINT16(REG_EMMC_PLL_RX30, BIT9);

	// Clear reg_emmc_dll_test[7]
	REG_CLR_BITS_UINT16(REG_EMMC_PLL_RX02, BIT15);

	// Enable reg_rxdll_dline_en
	REG_SET_BITS_UINT16(REG_EMMC_PLL_RX09, BIT0);

}
#endif


U32 nand_clock_setting(U32 u32ClkParam)
{
    REG_CLR_BITS_UINT16(reg_ckg_fcie, BIT6-1);

    #if defined(FCIE4_DDR_EMMC_PLL) && FCIE4_DDR_EMMC_PLL
    if( REG(NC_DDR_CTRL) & BIT_DDR_MASM )
    {
        if( (u32ClkParam & NFIE_CLK_EMMC_PLL) != NFIE_CLK_EMMC_PLL )
        {
            // DDR mode uses clocks from EMMC ATOP
            switch(u32ClkParam)
            {
                case NFIE_CLK_20M: nand_pll_clock_setting(EMMC_PLL_1XCLK_20M_IDX); break;
                case NFIE_CLK_27M: nand_pll_clock_setting(EMMC_PLL_1XCLK_27M_IDX); break;
                case NFIE_CLK_32M: nand_pll_clock_setting(EMMC_PLL_1XCLK_32M_IDX); break;
                case NFIE_CLK_36M: nand_pll_clock_setting(EMMC_PLL_1XCLK_36M_IDX); break;
                case NFIE_CLK_40M: nand_pll_clock_setting(EMMC_PLL_1XCLK_40M_IDX); break;
                case NFIE_CLK_43M: nand_pll_clock_setting(EMMC_PLL_1XCLK_43M_IDX); break;
                case NFIE_CLK_48M: nand_pll_clock_setting(EMMC_PLL_1XCLK_48M_IDX); break;
                case NFIE_CLK_54M: nand_pll_clock_setting(EMMC_PLL_1XCLK_54M_IDX); break;
                case NFIE_CLK_62M: nand_pll_clock_setting(EMMC_PLL_1XCLK_62M_IDX); break;
                case NFIE_CLK_72M: nand_pll_clock_setting(EMMC_PLL_1XCLK_72M_IDX); break;
                case NFIE_CLK_86M: nand_pll_clock_setting(EMMC_PLL_1XCLK_86M_IDX); break;
                default:
                    nand_die();
                    break;
            }

            nand_skew_clock_setting();
            nand_dll_setting();
        }

        REG_SET_BITS_UINT16(reg_ckg_fcie, NFIE_CLK_EMMC_PLL);
    }
    else
    {
    #endif  //defined(FCIE4_DDR_EMMC_PLL) && FCIE4_DDR_EMMC_PLL

        // SDR mode uses clocks from TOP CLKGEN
        switch(u32ClkParam)
        {
            case NFIE_CLK_20M:
            case NFIE_CLK_27M:
            case NFIE_CLK_32M:
            case NFIE_CLK_36M:
            case NFIE_CLK_40M:
            case NFIE_CLK_43M:
            case NFIE_CLK_48M:
            case NFIE_CLK_54M:
            case NFIE_CLK_62M:
            case NFIE_CLK_72M:
            case NFIE_CLK_86M:
                break;

            default:
			    nand_die();
			    break;
        }

        REG_SET_BITS_UINT16(reg_ckg_fcie, u32ClkParam);

        nand_debug(UNFD_DEBUG_LEVEL_HIGH, 0,"reg_ckg_fcie(%08X)=%08X\n",
		    reg_ckg_fcie, REG(reg_ckg_fcie));

    #if defined(FCIE4_DDR_EMMC_PLL) && FCIE4_DDR_EMMC_PLL
    }
    #endif

	return UNFD_ST_SUCCESS;
}


void nand_DumpPadClk(void)
{
	nand_debug(0, 1, "clk setting: \n");
	nand_debug(0, 1, "reg_ckg_fcie(0x%X):0x%x\n", reg_ckg_fcie, REG(reg_ckg_fcie));
    nand_debug(0, 1, "\nemmc pll setting: \n");
    nand_debug(0, 1, "REG_EMMC_PLL_RX01(0x%X):0x%x\n", REG_EMMC_PLL_RX01, REG(REG_EMMC_PLL_RX01));
    nand_debug(0, 1, "REG_EMMC_PLL_RX02(0x%X):0x%x\n", REG_EMMC_PLL_RX02, REG(REG_EMMC_PLL_RX02));
    nand_debug(0, 1, "REG_EMMC_PLL_RX03(0x%X):0x%x\n", REG_EMMC_PLL_RX03, REG(REG_EMMC_PLL_RX03));
    nand_debug(0, 1, "REG_EMMC_PLL_RX04(0x%X):0x%x\n", REG_EMMC_PLL_RX04, REG(REG_EMMC_PLL_RX04));
    nand_debug(0, 1, "REG_EMMC_PLL_RX05(0x%X):0x%x\n", REG_EMMC_PLL_RX05, REG(REG_EMMC_PLL_RX05));
    nand_debug(0, 1, "REG_EMMC_PLL_RX06(0x%X):0x%x\n", REG_EMMC_PLL_RX06, REG(REG_EMMC_PLL_RX06));
    nand_debug(0, 1, "REG_EMMC_PLL_RX07(0x%X):0x%x\n", REG_EMMC_PLL_RX07, REG(REG_EMMC_PLL_RX07));
    nand_debug(0, 1, "REG_EMMC_PLL_RX09(0x%X):0x%x\n", REG_EMMC_PLL_RX09, REG(REG_EMMC_PLL_RX09));
    nand_debug(0, 1, "REG_EMMC_PLL_RX18(0x%X):0x%x\n", REG_EMMC_PLL_RX18, REG(REG_EMMC_PLL_RX18));
    nand_debug(0, 1, "REG_EMMC_PLL_RX19(0x%X):0x%x\n", REG_EMMC_PLL_RX19, REG(REG_EMMC_PLL_RX19));
    nand_debug(0, 1, "REG_EMMC_PLL_RX30(0x%X):0x%x\n", REG_EMMC_PLL_RX30, REG(REG_EMMC_PLL_RX30));
    nand_debug(0, 1, "REG_EMMC_PLL_RX32(0x%X):0x%x\n", REG_EMMC_PLL_RX32, REG(REG_EMMC_PLL_RX32));
    nand_debug(0, 1, "REG_EMMC_PLL_RX33(0x%X):0x%x\n", REG_EMMC_PLL_RX33, REG(REG_EMMC_PLL_RX33));
    nand_debug(0, 1, "REG_EMMC_PLL_RX34(0x%X):0x%x\n", REG_EMMC_PLL_RX34, REG(REG_EMMC_PLL_RX34));

    nand_debug(0, 1, "\npad setting: \n");
	//fcie pad register
	nand_debug(0, 1, "NC_REG_2Fh(0x%X):0x%x\n", NC_REG_2Fh, REG(NC_REG_2Fh));
	nand_debug(0, 1, "NC_DDR_CTRL(0x%X):0x%x\n", NC_DDR_CTRL, REG(NC_DDR_CTRL));
	//chiptop pad register
	nand_debug(0, 1, "reg_all_pad_in(0x%X):0x%x\n", reg_allpad_in, REG(reg_allpad_in));

    nand_debug(0, 1, " reg_pcm_a_pe(0x%08X): 0x%04X\n", reg_pcm_a_pe, REG(reg_pcm_a_pe));
    nand_debug(0, 1, " reg_emmc_ps(0x%08X): 0x%04X\n", reg_emmc_ps, REG(reg_emmc_ps));
    nand_debug(0, 1, " reg_ts2_d0(0x%08X): 0x%04X\n", reg_ts2_d0, REG(reg_ts2_d0));
    nand_debug(0, 1, " reg_fcie2macro_sd_bypass(0x%08X): 0x%04X\n", reg_fcie2macro_sd_bypass, REG(reg_fcie2macro_sd_bypass));
    nand_debug(0, 1, " reg_sd_use_bypass(0x%08X): 0x%04X\n", reg_sd_use_bypass, REG(reg_sd_use_bypass));
    nand_debug(0, 1, " reg_sd_config(0x%08X): 0x%04X\n", reg_sd_config, REG(reg_sd_config));
    nand_debug(0, 1, " reg_sdio_config(0x%08X): 0x%04X\n", reg_sdio_config, REG(reg_sdio_config));
    nand_debug(0, 1, " REG_EMMC_CONFIG(0x%08X): 0x%04X\n", REG_EMMC_CONFIG, REG(REG_EMMC_CONFIG));

    nand_debug(0, 1, "reg_nf_en(0x%X):0x%x\n", reg_nf_en, REG(reg_nf_en));

}

#if defined(DECIDE_CLOCK_BY_NAND) && DECIDE_CLOCK_BY_NAND
#define MAX(a,b) ((a) > (b) ? (a) : (b))

U32 nand_config_timing(U16 u16_1T)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	U16 u16_DefaultTRR;
	U16 u16_DefaultTCS;
	U16 u16_DefaultTWW;
	U16 u16_DefaultRX40Cmd;
	U16 u16_DefaultRX40Adr;
	U16 u16_DefaultRX56;
	U16 u16_DefaultTADL;
	U16 u16_DefaultTCWAW;
	#if defined(NC_TWHR_TCLHZ) && NC_TWHR_TCLHZ
	U16 u16_DefaultTCLHZ = 4;
	#endif
	U16 u16_DefaultTWHR;
	#if (defined(NC_INST_DELAY) && NC_INST_DELAY) || \
		(defined(NC_HWCMD_DELAY) && NC_HWCMD_DELAY) || \
		(defined(NC_TRR_TCS) && NC_TRR_TCS)	||	\
		(defined(NC_TCWAW_TADL) && NC_TCWAW_TADL)	||	\
		(defined(NC_TWHR_TCLHZ) && NC_TWHR_TCLHZ)
	U16 u16_Tmp, u16_Cnt;
	U16 u16_Tmp2, u16_Cnt2;
	#endif

	#if defined(FCIE4_DDR) && FCIE4_DDR
	if(pNandDrv->u16_Reg58_DDRCtrl&BIT_DDR_ONFI)
	{
		u16_DefaultTRR = NC_ONFI_DEFAULT_TRR;
		u16_DefaultTCS = NC_ONFI_DEFAULT_TCS;
		u16_DefaultTWW = NC_ONFI_DEFAULT_TWW;
		u16_DefaultRX40Cmd = NC_ONFI_DEFAULT_RX40CMD;
		u16_DefaultRX40Adr = NC_ONFI_DEFAULT_RX40ADR;
		u16_DefaultRX56 = NC_ONFI_DEFAULT_RX56;
		u16_DefaultTADL = NC_ONFI_DEFAULT_TADL;
		u16_DefaultTCWAW = NC_ONFI_DEFAULT_TCWAW;
		u16_DefaultTWHR = NC_ONFI_DEFAULT_TWHR;
	}
	else if(pNandDrv->u16_Reg58_DDRCtrl&BIT_DDR_TOGGLE)
	{
		u16_DefaultTRR = NC_TOGGLE_DEFAULT_TRR;
		u16_DefaultTCS = NC_TOGGLE_DEFAULT_TCS;
		u16_DefaultTWW = NC_TOGGLE_DEFAULT_TWW;
		u16_DefaultRX40Cmd = NC_TOGGLE_DEFAULT_RX40CMD;
		u16_DefaultRX40Adr = NC_TOGGLE_DEFAULT_RX40ADR;
		u16_DefaultRX56 = NC_TOGGLE_DEFAULT_RX56;
		u16_DefaultTADL = NC_TOGGLE_DEFAULT_TADL;
		u16_DefaultTCWAW = NC_TOGGLE_DEFAULT_TCWAW;
		u16_DefaultTWHR = NC_TOGGLE_DEFAULT_TWHR;
	}
	else
	#endif
	{
		u16_DefaultTRR = NC_SDR_DEFAULT_TRR;
		u16_DefaultTCS = NC_SDR_DEFAULT_TCS;
		u16_DefaultTWW = NC_SDR_DEFAULT_TWW;
		u16_DefaultRX40Cmd = NC_SDR_DEFAULT_RX40CMD;
		u16_DefaultRX40Adr = NC_SDR_DEFAULT_RX40ADR;
		u16_DefaultRX56 = NC_SDR_DEFAULT_RX56;
		u16_DefaultTADL = NC_SDR_DEFAULT_TADL;
		u16_DefaultTCWAW = NC_SDR_DEFAULT_TCWAW;
		u16_DefaultTWHR = NC_SDR_DEFAULT_TWHR;
	}

	#if defined(NC_INST_DELAY) && NC_INST_DELAY
	// Check CMD_END
	u16_Tmp = MAX(pNandDrv->u16_tWHR, pNandDrv->u16_tCWAW);
	u16_Cnt = (u16_Tmp+u16_1T-1)/u16_1T;

	if(u16_DefaultRX40Cmd >= u16_Cnt)
		u16_Cnt = 0;
	else if(u16_Cnt-u16_DefaultRX40Cmd > 0xFF)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
		u16_Cnt -= u16_DefaultRX40Cmd;

	// Check ADR_END
	u16_Tmp2 = MAX(MAX(pNandDrv->u16_tWHR, pNandDrv->u16_tADL), pNandDrv->u16_tCCS);
	u16_Cnt2 = (u16_Tmp2+u16_1T-1)/u16_1T;

	if(u16_DefaultRX40Adr >= u16_Cnt2)
		u16_Cnt2 = 0;
	else if(u16_Cnt2-u16_DefaultRX40Adr > 0xFF)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
		u16_Cnt2 -= u16_DefaultRX40Adr;

	// get the max cnt
	u16_Cnt = MAX(u16_Cnt, u16_Cnt2);

	pNandDrv->u16_Reg40_Signal &= ~(0x00FF<<8);
	pNandDrv->u16_Reg40_Signal |= (u16_Cnt<<8);
	nand_debug(UNFD_DEBUG_LEVEL_HIGH,1, "u16_Reg40_Signal =  %X\n",pNandDrv->u16_Reg40_Signal);
	#endif

	#if defined(NC_HWCMD_DELAY) && NC_HWCMD_DELAY
	u16_Cnt = (pNandDrv->u16_tRHW+u16_1T-1)/u16_1T + 2;

	if(u16_DefaultRX56 >= u16_Cnt)
		u16_Cnt = 0;
	else if(u16_Cnt-u16_DefaultRX56 > 0xFF)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
		u16_Cnt -= u16_DefaultRX56;

	pNandDrv->u16_Reg56_Rand_W_Cmd &= ~(0x00FF<<8);
	pNandDrv->u16_Reg56_Rand_W_Cmd |= (u16_Cnt<<8);
	nand_debug(UNFD_DEBUG_LEVEL_HIGH,1, "u16_Reg56_Rand_W_Cmd =  %X\n",pNandDrv->u16_Reg56_Rand_W_Cmd);
	#endif

	#if defined(NC_TRR_TCS) && NC_TRR_TCS
	u16_Cnt = (pNandDrv->u8_tRR+u16_1T-1)/u16_1T + 2;

	if(u16_DefaultTRR >= u16_Cnt)
		u16_Cnt = 0;
	else if(u16_Cnt-u16_DefaultTRR > 0x0F)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
		u16_Cnt -= u16_DefaultTRR;

	u16_Tmp = (pNandDrv->u8_tCS+u16_1T-1)/u16_1T + 2;

	if(u16_DefaultTCS >= u16_Tmp)
		u16_Tmp = 0;
	else if(u16_Tmp-u16_DefaultTCS > 0x0F)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
		u16_Tmp -= u16_DefaultTCS;

	u16_Tmp2 = (pNandDrv->u16_tWW+u16_1T-1)/u16_1T + 2;

    if(u16_DefaultTWW >= u16_Tmp2)
        u16_Tmp2 = 0;
    else if(u16_Tmp2-u16_DefaultTWW > 0x0F)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
        u16_Tmp2 -= u16_DefaultTWW;

	u16_Cnt2 = MAX(u16_Tmp, u16_Tmp2);

	pNandDrv->u16_Reg59_LFSRCtrl &= ~(0x00FF);
	pNandDrv->u16_Reg59_LFSRCtrl |= (u16_Cnt|(u16_Cnt2<<4));
	nand_debug(UNFD_DEBUG_LEVEL_HIGH,1, "u16_Reg59_LFSRCtrl =  %X\n",pNandDrv->u16_Reg59_LFSRCtrl);
	#endif

	#if defined(NC_TCWAW_TADL) && NC_TCWAW_TADL
	u16_Cnt = (pNandDrv->u16_tADL + u16_1T - 1) / u16_1T + 2;

	if(u16_DefaultTADL > u16_Cnt)
		u16_Cnt = 0;
	else if(u16_Cnt-u16_DefaultTADL > 0xFF)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
		u16_Cnt -= u16_DefaultTADL;

	u16_Cnt2 = (pNandDrv->u16_tCWAW + u16_1T - 1) / u16_1T + 2;

	if(u16_DefaultTADL > u16_Cnt2)
		u16_Cnt2 = 0;
	else if(u16_Cnt2-u16_DefaultTCWAW > 0xFF)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
		u16_Cnt2 -= u16_DefaultTCWAW;

	pNandDrv->u16_Reg5D_tCWAW_tADL &= ~(0xFFFF);
	pNandDrv->u16_Reg5D_tCWAW_tADL |= (u16_Cnt|(u16_Cnt2<<8));
	nand_debug(UNFD_DEBUG_LEVEL_HIGH,1, "u16_Reg5D_tCWAW_tADL =  %X\n",pNandDrv->u16_Reg5D_tCWAW_tADL);
	#endif

	#if defined(NC_TWHR_TCLHZ) && NC_TWHR_TCLHZ
	u16_Cnt = (pNandDrv->u8_tCLHZ + u16_1T - 1) / u16_1T + 2;

	if(u16_DefaultTCLHZ > u16_Cnt)
		u16_Cnt = 0;
	else if(u16_Cnt-u16_DefaultTCLHZ > 0xF)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
		u16_Cnt -= u16_DefaultTCLHZ;

	u16_Cnt2 = (pNandDrv->u16_tWHR + u16_1T - 1) / u16_1T + 2;

	if(u16_DefaultTWHR > u16_Cnt2)
		u16_Cnt2 = 0;
	else if(u16_Cnt2-u16_DefaultTWHR > 0xFF)
		return UNFD_ST_ERR_INVALID_PARAM;
	else
		u16_Cnt2 -= u16_DefaultTWHR;

	pNandDrv->u16_Reg5A_tWHR_tCLHZ &= ~(0xFFFF);
	pNandDrv->u16_Reg5A_tWHR_tCLHZ |= ((u16_Cnt&0xF)|(u16_Cnt2<<8));
	nand_debug(UNFD_DEBUG_LEVEL_HIGH,1, "u16_Reg5A_tWHR_tCLHZ =  %X\n",pNandDrv->u16_Reg5A_tWHR_tCLHZ);
	#endif

	NC_Config();
	return UNFD_ST_SUCCESS;
}

U32 nand_find_timing(U8 *pu8_ClkIdx, U8 u8_find_DDR_timg)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	U32 au32_1TTable[NFIE_CLK_TABLE_CNT] = NFIE_1T_TABLE;
	U32 au32_ClkValueTable[NFIE_CLK_TABLE_CNT] = NFIE_CLK_VALUE_TABLE;

	#if defined(FCIE4_DDR) && FCIE4_DDR
	U32 au32_4Clk1TTable[NFIE_4CLK_TABLE_CNT] = NFIE_4CLK_1T_TABLE;
	U32 au32_4ClkValueTable[NFIE_4CLK_TABLE_CNT] = NFIE_4CLK_VALUE_TABLE;
	#endif
	U32 u32_Clk;
	U16 u16_SeqAccessTime, u16_Tmp, u16_Tmp2, u16_1T, u16_RE_LATCH_DELAY;
	S8 s8_ClkIdx;

	s8_ClkIdx = 0;
	u16_1T = 0;

	if(pNandDrv->u16_Reg58_DDRCtrl&BIT_DDR_ONFI)
	{
		u16_SeqAccessTime = 10;
	}
	else if(pNandDrv->u16_Reg58_DDRCtrl&BIT_DDR_TOGGLE)
	{
		u16_Tmp = MAX(MAX(2*pNandDrv->u8_tRP, 2*pNandDrv->u8_tREH), pNandDrv->u16_tRC);
		u16_Tmp2 = MAX(MAX(pNandDrv->u8_tWP, pNandDrv->u8_tWH), (pNandDrv->u16_tWC+1)/2);
		u16_SeqAccessTime = MAX(u16_Tmp, u16_Tmp2);
	}
	else
	{
		u16_Tmp = MAX(MAX(pNandDrv->u8_tRP, pNandDrv->u8_tREH), (pNandDrv->u16_tRC+1)/2);
		u16_Tmp2 = MAX(MAX(pNandDrv->u8_tWP, pNandDrv->u8_tWH), (pNandDrv->u16_tWC+1)/2);
		u16_SeqAccessTime = MAX(u16_Tmp, u16_Tmp2);

		u16_Tmp = (pNandDrv->u8_tREA + NAND_SEQ_ACC_TIME_TOL)/2;
		u16_Tmp2 = u16_SeqAccessTime;
		u16_SeqAccessTime = MAX(u16_Tmp, u16_Tmp2);

	}

	u32_Clk = 1000000000/((U32)u16_SeqAccessTime);

	if(!u8_find_DDR_timg)
	{
		for(s8_ClkIdx =  0; s8_ClkIdx <= NFIE_CLK_TABLE_CNT - 1; s8_ClkIdx ++)
		{
			if(u32_Clk <= au32_ClkValueTable[s8_ClkIdx])
			{
				break;
			}
		}
	}
	else
	{
		#if defined(FCIE4_DDR) && FCIE4_DDR
		for(s8_ClkIdx =  0; s8_ClkIdx <= NFIE_4CLK_TABLE_CNT - 1; s8_ClkIdx ++)
		{
			if(u32_Clk < au32_4ClkValueTable[s8_ClkIdx])
			{
				break;
			}
		}
		#endif
	}
	s8_ClkIdx --;


RETRY:
	if(s8_ClkIdx<0)
		return UNFD_ST_ERR_INVALID_PARAM;
	if(!u8_find_DDR_timg)
		u16_1T = au32_1TTable[s8_ClkIdx];
	#if defined(FCIE4_DDR) && FCIE4_DDR
	else
		u16_1T = au32_4Clk1TTable[s8_ClkIdx];
	#endif

	if(nand_config_timing(u16_1T) != UNFD_ST_SUCCESS)
	{
		s8_ClkIdx--;
		goto RETRY;
	}

	u16_RE_LATCH_DELAY = BIT_NC_LATCH_DATA_1_0_T;


	pNandDrv->u16_Reg57_RELatch &= ~BIT_NC_LATCH_DATA_MASK;
	pNandDrv->u16_Reg57_RELatch |= u16_RE_LATCH_DELAY;

	*pu8_ClkIdx	= (U8)s8_ClkIdx;

	return UNFD_ST_SUCCESS;

}
#endif

#if defined( FCIE4_DDR_RETRY_DQS) && FCIE4_DDR_RETRY_DQS
void nand_retry_dqs_post(void)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	U32 u32_TmpClk;
	DDR_TIMING_GROUP_t tTmpDDR;

	nand_debug(UNFD_DEBUG_LEVEL, 1,"exchange dqs %d to %d\r\n", pNandDrv->tDefaultDDR.u8_DqsMode, pNandDrv->tMinDDR.u8_DqsMode);

	u32_TmpClk = pNandDrv->u32_Clk;
	memcpy((void *)&tTmpDDR, (const void *)&pNandDrv->tDefaultDDR, sizeof(DDR_TIMING_GROUP_t));

	pNandDrv->u32_Clk = pNandDrv->u32_minClk;
	memcpy((void *)&pNandDrv->tDefaultDDR, (const void *)&pNandDrv->tMinDDR, sizeof(DDR_TIMING_GROUP_t));

	pNandDrv->u32_minClk = u32_TmpClk;
	memcpy((void *)&pNandDrv->tMinDDR, (const void *)&tTmpDDR, sizeof(DDR_TIMING_GROUP_t));

	nand_clock_setting(pNandDrv->u32_Clk);
	NC_FCIE4SetInterface(1, pNandDrv->tDefaultDDR.u8_DqsMode, pNandDrv->tDefaultDDR.u8_DelayCell, pNandDrv->tDefaultDDR.u8_DdrTiming);
}
#endif

U32 nand_config_clock(U16 u16_SeqAccessTime)
{

#if defined(DECIDE_CLOCK_BY_NAND) && DECIDE_CLOCK_BY_NAND
	NAND_DRIVER * pNandDrv = drvNAND_get_DrvContext_address();
	U32 u32_Err = 0;
	U32 au32_ClkTable[NFIE_CLK_TABLE_CNT] = NFIE_CLK_TABLE;
	//char *ClkStrTable[NFIE_CLK_TABLE_CNT] = NFIE_CLK_TABLE_STR;

	#if defined(FCIE4_DDR) && FCIE4_DDR
	U32 au32_4ClkTable[NFIE_4CLK_TABLE_CNT] = NFIE_4CLK_TABLE;
	//char *Clk4StrTable[NFIE_4CLK_TABLE_CNT] = NFIE_4CLK_TABLE_STR;
	U32 au32_1TTable[NFIE_4CLK_TABLE_CNT] = NFIE_4CLK_1T_TABLE;
	#endif

	U8 u8_ClkIdx = 0;

	#if defined(FCIE4_DDR) && FCIE4_DDR
	if(pNandDrv->u16_Reg58_DDRCtrl&BIT_DDR_MASM)
	{
		if(pNandDrv->tDefaultDDR.u8_DdrTiming == 0)
		{
			printk("NAND Error: Empty Timing setting for DDR NAND is detected\n");
			nand_die();
		}
		else
		{
			pNandDrv->u32_minClk = au32_4ClkTable[pNandDrv->tMinDDR.u8_ClkIdx];
			pNandDrv->u32_Clk = au32_4ClkTable[pNandDrv->tDefaultDDR.u8_ClkIdx];
			#if defined(FCIE4_DDR_EMMC_PLL) && FCIE4_DDR_EMMC_PLL
			NC_FCIE4SetInterface_EMMC_PLL(1, pNandDrv->tDefaultDDR.u8_DqsMode,
				 pNandDrv->tDefaultDDR.u8_DdrTiming);
			#else
			NC_FCIE4SetInterface(1, pNandDrv->tDefaultDDR.u8_DqsMode,
				pNandDrv->tDefaultDDR.u8_DelayCell, pNandDrv->tDefaultDDR.u8_DdrTiming);
			#endif

			if(nand_config_timing(au32_1TTable[pNandDrv->tDefaultDDR.u8_ClkIdx]) != UNFD_ST_SUCCESS)
			{
				nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Err, NAND, Cannot config nand timing\n");
				nand_die();
				return(u32_Err);
			}
		}
        #if defined(FCIE4_DDR_EMMC_PLL) && FCIE4_DDR_EMMC_PLL
        nand_debug(UNFD_DEBUG_LEVEL,1,"ok, get default DDR timing: EMMC_PLL 09h:%X, 57h:%X\n",
					pNandDrv->u16_Emmc_Pll_Reg09, pNandDrv->u16_Reg57_RELatch);
        #else
		nand_debug(UNFD_DEBUG_LEVEL,1,"ok, get default DDR timing: 2Ch:%X, 57h:%X\n",
					pNandDrv->u16_Reg2C_SMStatus, pNandDrv->u16_Reg57_RELatch);
        #endif
		u8_ClkIdx = pNandDrv->tDefaultDDR.u8_ClkIdx;
		//printk(KERN_CRIT "[%s]\tFCIE is set to %sHz\n",__func__, Clk4StrTable[u8_ClkIdx]);

	}
	else
	#endif
	{

		u32_Err = nand_find_timing(&u8_ClkIdx, 0);
		if(u32_Err != UNFD_ST_SUCCESS)
		{
			nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Err, NAND, Cannot config nand timing\n");
			nand_die();
			return(u32_Err);
		}
		pNandDrv->u32_Clk = au32_ClkTable[u8_ClkIdx];
		//printk(KERN_CRIT "[%s]\tFCIE is set to %sHz\n",__func__, ClkStrTable[u8_ClkIdx]);
	}

	nand_clock_setting(pNandDrv->u32_Clk);
	//printk(KERN_CRIT "[%s]\treg_ckg_fcie(%08X)=%08X\n", __func__, reg_ckg_fcie, REG(reg_ckg_fcie));

	REG_WRITE_UINT16(NC_LATCH_DATA, pNandDrv->u16_Reg57_RELatch);
	//printk(KERN_CRIT "[%s]\tRE LATCH is set to %X\n",__func__, pNandDrv->u16_Reg57_RELatch);

#endif
	return UNFD_ST_SUCCESS;
}

#if defined(ENABLE_NAND_POWER_SAVING_MODE) && ENABLE_NAND_POWER_SAVING_MODE
void nand_Prepare_Power_Saving_Mode_Queue(void)
{
    #if defined(ENABLE_NAND_POWER_SAVING_DEGLITCH) && ENABLE_NAND_POWER_SAVING_DEGLITCH
    REG_SET_BITS_UINT16(reg_pwrgd_int_glirm, BIT_PWRGD_INT_GLIRM_EN);
    #endif

    /* (1) Clear HW Enable */
	REG_WRITE_UINT16(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x00), 0x0000);
	REG_WRITE_UINT16(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x01),
					 PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x0A);

	/* (2) Clear All Interrupt */
	REG_WRITE_UINT16(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x02), 0xffff);
	REG_WRITE_UINT16(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x03),
					 PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x00);

	/* (3) Reset Start */
	REG_WRITE_UINT16(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x04), 0x4800);
	REG_WRITE_UINT16(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x05),
					 PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x30);

	/* (4) Reset End */
	REG_WRITE_UINT16(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x06), 0x5800);
	REG_WRITE_UINT16(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x07),
					 PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x30);

	/* (5) STOP */
	REG_WRITE_UINT16(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x08), 0x0000);
	REG_WRITE_UINT16(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x09),
					 PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_STOP);

    REG_CLR_BITS_UINT16(NC_PWRSAVE_CTL, BIT_SD_POWER_SAVE_RST);
    REG_SET_BITS_UINT16(NC_PWRSAVE_CTL, BIT_SD_POWER_SAVE_RST);

    REG_SET_BITS_UINT16(NC_PWRSAVE_CTL, BIT_POWER_SAVE_MODE);
}
#endif

void nand_set_WatchDog(U8 u8_IfEnable)
{
	// do nothing in Linux
}

void nand_reset_WatchDog(void)
{
	// do nothing in Linux
}


//extern struct mtd_info *nand_mtd ;
U32 nand_translate_DMA_address_Ex(U32 u32_DMAAddr, U32 u32_ByteCnt, int mode)
{
    if( u32_DMAAddr >= MSTAR_MIU1_BUS_BASE)
	{
		REG_SET_BITS_UINT16( NC_MIU_DMA_SEL, BIT_MIU1_SELECT);
		u32_DMAAddr -= MSTAR_MIU1_BUS_BASE;
	}
	else
	{
		REG_CLR_BITS_UINT16( NC_MIU_DMA_SEL, BIT_MIU1_SELECT);
		u32_DMAAddr -= MSTAR_MIU0_BUS_BASE;
	}

	return u32_DMAAddr;
}
U32 nand_DMA_MAP_address(U32 u32_Buffer, U32 u32_ByteCnt, int mode)
{
	dma_addr_t dma_addr;

	if(mode == 0)	//write
	{
		dma_addr = dma_map_single(&info->pdev->dev, (void*)u32_Buffer, u32_ByteCnt, DMA_TO_DEVICE);
	}
	else
	{
		dma_addr = dma_map_single(&info->pdev->dev, (void*)u32_Buffer, u32_ByteCnt, DMA_FROM_DEVICE);
	}
	return dma_addr;
}

void nand_DMA_UNMAP_address(U32 u32_DMAAddr, U32 u32_ByteCnt, int mode)
{
	if(mode == 0)	//write
	{
		dma_unmap_single(&info->pdev->dev, u32_DMAAddr, u32_ByteCnt, DMA_TO_DEVICE);
	}
	else
	{
		dma_unmap_single(&info->pdev->dev, u32_DMAAddr, u32_ByteCnt, DMA_FROM_DEVICE);
	}
}

void nand_flush_miu_pipe(void)
{
}


#if defined(ENABLE_NAND_INTERRUPT_MODE) && ENABLE_NAND_INTERRUPT_MODE
static DECLARE_WAIT_QUEUE_HEAD(fcie_wait);
static U16 u16CurNCMIEEvent = 0;			// Used to store current IRQ state

irqreturn_t NC_FCIE_IRQ(int irq, void *dummy)
{
	if((REG(NC_PATH_CTL) & BIT_NC_EN) != BIT_NC_EN)
		return IRQ_NONE;

#if NC_REG_MIU_LAST_DONE == NC_MIE_EVENT
	//avoid to cleaning LAST_DONE Flag
	u16CurNCMIEEvent |= REG(NC_MIE_EVENT) & (~BIT_MIU_LAST_DONE);
#else
	u16CurNCMIEEvent |= REG(NC_MIE_EVENT);
#endif
	REG_WRITE_UINT16(NC_MIE_EVENT, u16CurNCMIEEvent);
	wake_up(&fcie_wait);

	return IRQ_HANDLED;
}

U32 nand_WaitCompleteIntr(U16 u16_WaitEvent, U32 u32_MicroSec, U16* u16_WaitedEvent)
{
	U16 u16_Reg;
	U32 u32_Timeout = (usecs_to_jiffies(u32_MicroSec) > 0) ? usecs_to_jiffies(u32_MicroSec) : 1; // timeout time

	wait_event_timeout(fcie_wait, ((u16CurNCMIEEvent & u16_WaitEvent) == u16_WaitEvent), u32_Timeout);

	if( (u16CurNCMIEEvent & u16_WaitEvent) != u16_WaitEvent ) // wait at least 2 second for FCIE3 events
	{
		*u16_WaitedEvent = u16CurNCMIEEvent;
		u16CurNCMIEEvent = 0;
		REG_READ_UINT16(NC_MIE_EVENT, u16_Reg);		// Read all events
		nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Timeout: REG(NC_MIE_EVENT) = 0x%X\n", u16_Reg);
		return UNFD_ST_ERR_E_TIMEOUT;
	}

	*u16_WaitedEvent = u16CurNCMIEEvent = 0;
	REG_WRITE_UINT16(NC_MIE_EVENT, u16_WaitEvent);

	return UNFD_ST_SUCCESS;
}



void nand_enable_intr_mode(void)
{
    int err = 0;
  	err = request_irq(E_IRQ_NFIE, NC_FCIE_IRQ, IRQF_SHARED, "fcie", &sg_NandDrv);
	REG_WRITE_UINT16(NC_MIE_INT_EN, BIT_NC_JOB_END|BIT_MMA_DATA_END);
}

#endif

void *drvNAND_get_DrvContext_address(void) // exposed API
{
	return &sg_NandDrv;
}

void *drvNAND_get_DrvContext_PartInfo(void)
{
	return (void*)((U32)gau32_PartInfo);
}

U32 NC_PlatformResetPre(void)
{
	//HalMiuMaskReq(MIU_CLT_FCIE);
	return UNFD_ST_SUCCESS;
}

U32 NC_PlatformResetPost(void)
{
	U16 u16_Reg;
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();

	REG_WRITE_UINT16(NC_PATH_CTL, BIT_NC_EN);

	REG_READ_UINT16(FCIE_NC_CIFD_BASE, u16_Reg); // dummy read for CIFD clock

	REG_READ_UINT16(FCIE_NC_CIFD_BASE, u16_Reg); // dummy read for CIFD clock

	//#if defined(DUTY_CYCLE_PATCH)&&DUTY_CYCLE_PATCH
	REG_WRITE_UINT16(NC_WIDTH, FCIE_REG41_VAL);	// duty cycle 3:1 in 86Mhz
	//#endif

	#if defined (DECIDE_CLOCK_BY_NAND) && DECIDE_CLOCK_BY_NAND
	REG_WRITE_UINT16(NC_LATCH_DATA, pNandDrv->u16_Reg57_RELatch);
	#else

	#if defined(REG57_ECO_FIX_INIT_VALUE)
	REG_WRITE_UINT16(NC_LATCH_DATA, REG57_ECO_FIX_INIT_VALUE);
	pNandDrv->u16_Reg57_RELatch = REG57_ECO_FIX_INIT_VALUE;
	#endif

	#endif

	/*HW bug
	In order to fix the front data overlapped by tail data of spare in CIFD when spare byte > 512
	But when ecc correctable Not in first sector, the following sector spare data will not be copied to CIFD.

	when spare > 512 using spare dma
	*/
	REG_SET_BITS_UINT16(NC_DDR_CTRL, BIT7);
	pNandDrv->u16_Reg58_DDRCtrl |= BIT7;

	//HalMiuUnMaskReq(MIU_CLT_FCIE);

    return UNFD_ST_SUCCESS;
}

U32 NC_PlatformInit(void)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();

	nand_pads_init();
	REG_WRITE_UINT16(NC_PATH_CTL, BIT_NC_EN);
	pNandDrv->u8_WordMode = 0; // TV/Set-Top Box projects did not support x16 NAND flash
	nand_pads_switch(1);

#if defined (DECIDE_CLOCK_BY_NAND) && DECIDE_CLOCK_BY_NAND
	pNandDrv->u32_Clk =FCIE3_SW_SLOWEST_CLK;
	nand_clock_setting(FCIE3_SW_SLOWEST_CLK);
#else
	pNandDrv->u32_Clk =FCIE3_SW_DEFAULT_CLK;
	nand_clock_setting(FCIE3_SW_DEFAULT_CLK);
#endif
    // print clock setting
	//printk(KERN_CRIT "reg_ckg_fcie(%08X)=%08X\n", reg_ckg_fcie, REG(reg_ckg_fcie));

	// no shared-bus with Disp
	pNandDrv->u8_SwPatchWaitRb= 0;
	pNandDrv->u8_SwPatchJobStart= 0;

	return UNFD_ST_SUCCESS;
}

U32 NC_CheckStorageType(void)
{
	U16 u16_regval;
	u16_regval = REG(NC_REG_16h);

	if( (u16_regval & BIT_KERN_CHK_NAND_EMMC) == BIT_KERN_CHK_NAND_EMMC )
	{
		if( (u16_regval & BIT_KERN_NAND) != BIT_KERN_NAND )
			return 0;
	}
	return 1;
}

U32 NC_CheckBlankPage(U8 * pu8_Buffer)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	int ECCErrBitCnt, j;

	if(pNandDrv->u8_RequireRandomizer != 1)
	{
		U16 u16_AllOneBit;
		u16_AllOneBit = BIT13;
		if( ((REG(NC_CTRL) & u16_AllOneBit) == u16_AllOneBit))
		{
		//	printk("BLK %X, free page @ %X\n", u16_BlkIdx, realpage % pNandDrv->u16_BlkPageCnt);
			return 1;
		}
	}
	else
	{
		ECCErrBitCnt = 0;
		for(j = 0; j < pNandDrv->u16_PageByteCnt * pNandDrv->u8_PlaneCnt; j++)
		{
			ECCErrBitCnt += ZeroBitCountfor0xFFByte(pu8_Buffer[j]);
			if(ECCErrBitCnt != 0)
				break;
		}
		if(ECCErrBitCnt == 0)
		{
			return 1;
		}
	}
	return 0;
}


#else
  #error "Error! no platform functions."
#endif
