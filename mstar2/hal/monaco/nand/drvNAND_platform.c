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
#include "drvNAND.h"

//=============================================================
//=============================================================
#if (defined(NAND_DRV_TV_LINUX)&&NAND_DRV_TV_LINUX)

struct mstar_nand_info{
    struct mtd_info nand_mtd;
    struct platform_device *pdev;
    struct nand_chip    nand;
    struct mtd_partition    *parts;
};
extern struct mstar_nand_info *info;

NAND_DRIVER sg_NandDrv;

static UNFD_ALIGN0 U32 gau32_PartInfo[NAND_PARTITAION_BYTE_CNT/4]UNFD_ALIGN1;
static u16 fciepad = 0;
extern int enable_sar5;

U32 nand_hw_timer_delay(U32 u32usTick)
{
    #if 0   // Use PIU timer

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

    #else   // Use kernel udelay
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

#if defined(DDR_NAND_SUPPORT) && DDR_NAND_SUPPORT
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

    REG_WRITE_UINT16(NC_FUN_CTL, BIT_NC_EN);

    REG_CLR_BITS_UINT16(reg_emmc_config, BIT7|BIT6);                //clean EMMC config 0x5D
    REG_CLR_BITS_UINT16(reg_sd_config, BIT9|BIT8);
    REG_CLR_BITS_UINT16(reg_sdio_config, BIT5|BIT4);

    if( fciepad )
    {
        REG_SET_BITS_UINT16(reg_nf_en, fciepad);
    }
    else
    {
        fciepad = REG(reg_nf_en) & REG_NAND_MODE_MASK;
    }

    pNandDrv->u16_Reg58_DDRCtrl &= ~(BIT_DDR_MASM|BIT_SDR_DIN_FROM_MACRO|BIT_NC_32B_MODE);

    REG_WRITE_UINT16(reg_emmcpll_rx71, 0xffff);
    REG_SET_BITS_UINT16(reg_emmcpll_rx72, BIT0|BIT1);
    REG_WRITE_UINT16(reg_emmcpll_rx73, 0xffff);
    REG_SET_BITS_UINT16(reg_emmcpll_rx74, BIT0|BIT1|BIT15);
    REG_CLR_BITS_UINT16(reg_emmcpll_rx63, BIT0);

    if(NAND_PAD_BYPASS_MODE == u32EnableFCIE)
    {
        REG_CLR_BITS_UINT16(reg_emmcpll_rx60, BIT0|BIT1|BIT2);
        REG_CLR_BITS_UINT16(reg_emmcpll_rx6d, BIT0);
        REG_CLR_BITS_UINT16(reg_emmcpll_rx70, BIT8);
        NC_DQS_PULL_H();
    }
    else if(NAND_PAD_TOGGLE_MODE == u32EnableFCIE)
    {
        if(pNandDrv->u8_MacroType == MACRO_TYPE_8BIT)
        {
            REG_CLR_BITS_UINT16(reg_emmcpll_rx60, BIT0|BIT1|BIT2);
            REG_SET_BITS_UINT16(reg_emmcpll_rx60, BIT0);
            REG_SET_BITS_UINT16(reg_emmcpll_rx6d, BIT0);
            REG_CLR_BITS_UINT16(reg_emmcpll_rx70, BIT8);            
            pNandDrv->u16_Reg58_DDRCtrl |= (BIT_DDR_TOGGLE|BIT_SDR_DIN_FROM_MACRO);
        }
        else if(pNandDrv->u8_MacroType == MACRO_TYPE_32BIT)
        {
            REG_CLR_BITS_UINT16(reg_emmcpll_rx60, BIT0|BIT1|BIT2);
            REG_SET_BITS_UINT16(reg_emmcpll_rx60, BIT0|BIT1);
            REG_CLR_BITS_UINT16(reg_emmcpll_rx6d, BIT0);
            REG_SET_BITS_UINT16(reg_emmcpll_rx70, BIT8);

            REG_WRITE_UINT16(reg_emmcpll_rx71, 0);
            REG_CLR_BITS_UINT16(reg_emmcpll_rx72, BIT0|BIT1);
            REG_WRITE_UINT16(reg_emmcpll_rx73, 0);
            REG_CLR_BITS_UINT16(reg_emmcpll_rx74, BIT0|BIT1|BIT15);
            REG_SET_BITS_UINT16(reg_emmcpll_rx63, BIT0);            
            pNandDrv->u16_Reg58_DDRCtrl |= (BIT_DDR_TOGGLE|BIT_NC_32B_MODE);
        }

        NC_DQS_PULL_L();
    }
    else if(NAND_PAD_ONFI_SYNC_MODE == u32EnableFCIE)
    {
        if(pNandDrv->u8_MacroType == MACRO_TYPE_8BIT)
        {
            REG_CLR_BITS_UINT16(reg_emmcpll_rx60, BIT0|BIT1|BIT2);
            REG_SET_BITS_UINT16(reg_emmcpll_rx60, BIT0);
            REG_SET_BITS_UINT16(reg_emmcpll_rx6d, BIT0);
            REG_CLR_BITS_UINT16(reg_emmcpll_rx70, BIT8);            
            pNandDrv->u16_Reg58_DDRCtrl |= (BIT_DDR_ONFI|BIT_SDR_DIN_FROM_MACRO);
        }
        else if(pNandDrv->u8_MacroType == MACRO_TYPE_32BIT)
        {
            REG_CLR_BITS_UINT16(reg_emmcpll_rx60, BIT0|BIT1|BIT2);
            REG_SET_BITS_UINT16(reg_emmcpll_rx60, BIT0|BIT2);
            REG_CLR_BITS_UINT16(reg_emmcpll_rx6d, BIT0);
            REG_SET_BITS_UINT16(reg_emmcpll_rx70, BIT8);

            REG_WRITE_UINT16(reg_emmcpll_rx71, 0);
            REG_CLR_BITS_UINT16(reg_emmcpll_rx72, BIT0|BIT1);
            REG_WRITE_UINT16(reg_emmcpll_rx73, 0);
            REG_CLR_BITS_UINT16(reg_emmcpll_rx74, BIT0|BIT1|BIT15);
            REG_SET_BITS_UINT16(reg_emmcpll_rx63, BIT0);
            pNandDrv->u16_Reg58_DDRCtrl |= (BIT_DDR_ONFI|BIT_NC_32B_MODE);
        }
        NC_DQS_PULL_L();
    }
    REG_WRITE_UINT16(NC_DDR_CTRL, pNandDrv->u16_Reg58_DDRCtrl);


    if(NAND_MODE1 == fciepad )
    {
        REG_SET_BITS_UINT16(reg_pcm_a_pe, 0xFF);
    }
    nand_debug(UNFD_DEBUG_LEVEL_LOW,1,"reg_nf_en(%08X)=%04X\n", reg_nf_en, REG(reg_nf_en));
    nand_debug(UNFD_DEBUG_LEVEL_LOW,1,"reg_allpad_in(%08X)=%04X\n", reg_allpad_in, REG(reg_allpad_in));


    return UNFD_ST_SUCCESS;
}

void nand_pads_release(void)
{
    //release pad setting for CI card default setting
    REG_CLR_BITS_UINT16(reg_pcm_a_pe, 0xFF);
}

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
   nand_hw_timer_delay(1000);
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
    nand_hw_timer_delay(1000);

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

U32 nand_clock_setting(U32 u32ClkParam)
{
    REG_CLR_BITS_UINT16(reg_ckg_fcie, BIT2|BIT3|BIT4|BIT5);

    REG_SET_BITS_UINT16(reg_ckg_fcie, BIT_CLK_ENABLE);
    REG_CLR_BITS_UINT16(reg_ckg_fcie, BIT1|BIT0);

    if( REG(NC_DDR_CTRL) & BIT_DDR_MASM )
    {

        // DDR mode uses clocks from EMMC ATOP
        switch(u32ClkParam)
        {
            case NFIE_CLK_12M: nand_pll_clock_setting(EMMC_PLL_1XCLK_12M_IDX); break;        
            case NFIE_CLK_20M: nand_pll_clock_setting(EMMC_PLL_1XCLK_20M_IDX); break;
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
    REG_SET_BITS_UINT16(reg_ckg_fcie, NFIE_CLK_2X_P_DDR);

            nand_skew_clock_setting();
            nand_dll_setting();
        }

    }
    else
    {

    // SDR mode uses clocks from TOP CLKGEN
    switch(u32ClkParam)
    {
        case NFIE_CLK_12M:
        case NFIE_CLK_20M:
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

    }

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
    nand_debug(0, 1, "NC_DDR_CTRL(0x%X):0x%x\n", NC_DDR_CTRL, REG(NC_DDR_CTRL));
    //chiptop pad register
    nand_debug(0, 1, "reg_all_pad_in(0x%X):0x%x\n", reg_allpad_in, REG(reg_allpad_in));

    nand_debug(0, 1, " reg_pcm_a_pe(0x%08X): 0x%04X\n", reg_pcm_a_pe, REG(reg_pcm_a_pe));
    nand_debug(0, 1, " reg_sd_config(0x%08X): 0x%04X\n", reg_sd_config, REG(reg_sd_config));
    nand_debug(0, 1, " reg_sdio_config(0x%08X): 0x%04X\n", reg_sdio_config, REG(reg_sdio_config));
    nand_debug(0, 1, " reg_emmc_config(0x%08X): 0x%04X\n", reg_emmc_config, REG(reg_emmc_config));

    nand_debug(0, 1, "reg_nf_en(0x%X):0x%x\n", reg_nf_en, REG(reg_nf_en));

    
    nand_debug(0, 1, "reg_emmcpll_rx60(0x%X):0x%x\n", reg_emmcpll_rx60, REG(reg_emmcpll_rx60)); 
    nand_debug(0, 1, "reg_emmcpll_rx63(0x%X):0x%x\n", reg_emmcpll_rx63, REG(reg_emmcpll_rx63));
    nand_debug(0, 1, "reg_emmcpll_rx6d(0x%X):0x%x\n", reg_emmcpll_rx6d, REG(reg_emmcpll_rx6d));
    nand_debug(0, 1, "reg_emmcpll_rx70(0x%X):0x%x\n", reg_emmcpll_rx70, REG(reg_emmcpll_rx70));
    nand_debug(0, 1, "reg_emmcpll_rx71(0x%X):0x%x\n", reg_emmcpll_rx71, REG(reg_emmcpll_rx71)); 
    nand_debug(0, 1, "reg_emmcpll_rx72(0x%X):0x%x\n", reg_emmcpll_rx72, REG(reg_emmcpll_rx72));
    nand_debug(0, 1, "reg_emmcpll_rx73(0x%X):0x%x\n", reg_emmcpll_rx73, REG(reg_emmcpll_rx73));
    nand_debug(0, 1, "reg_emmcpll_rx74(0x%X):0x%x\n", reg_emmcpll_rx74, REG(reg_emmcpll_rx74));
    
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
        (defined(NC_TRR_TCS) && NC_TRR_TCS) ||  \
        (defined(NC_TCWAW_TADL) && NC_TCWAW_TADL)   ||  \
        (defined(NC_TWHR_TCLHZ) && NC_TWHR_TCLHZ)
    U16 u16_Tmp, u16_Cnt;
    U16 u16_Tmp2, u16_Cnt2;
    #endif

    #if defined(DDR_NAND_SUPPORT) && DDR_NAND_SUPPORT
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

    #if  defined(DDR_NAND_SUPPORT) && DDR_NAND_SUPPORT
    U32 au32_4Clk1TTable[NFIE_4CLK_TABLE_CNT] = NFIE_4CLK_1T_TABLE;
    U32 au32_4ClkValueTable[NFIE_4CLK_TABLE_CNT] = NFIE_4CLK_VALUE_TABLE;
    #endif
    U16 u16_i, u16_j, u16_pass_begin = 0xFF, u16_pass_cnt = 0;
    U16 u16_nandstatus;
    U32 u32_Err;
    U8 au8_ID[NAND_ID_BYTE_CNT];    
    U32 au32_ClkTable[NFIE_CLK_TABLE_CNT] = NFIE_CLK_TABLE;
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
        #if  defined(DDR_NAND_SUPPORT) && DDR_NAND_SUPPORT
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
    #if  defined(DDR_NAND_SUPPORT) && DDR_NAND_SUPPORT
    else
        u16_1T = au32_4Clk1TTable[s8_ClkIdx];
    #endif

    if(nand_config_timing(u16_1T) != UNFD_ST_SUCCESS)
    {
        s8_ClkIdx--;
        goto RETRY;
    }

    NC_ReadStatus();
    REG_READ_UINT16(NC_ST_READ, u16_nandstatus);
    
    pNandDrv->u32_Clk = au32_ClkTable[s8_ClkIdx];
    nand_clock_setting(pNandDrv->u32_Clk);

    //using read id to detect relatch
    memcpy(au8_ID, pNandDrv->au8_ID, NAND_ID_BYTE_CNT);
    for(u16_i = 0 ; u16_i < 16 ; u16_i ++)
    {

        pNandDrv->u16_Reg57_RELatch &= ~(BIT_NC_LATCH_DATA_MASK|BIT0);
        if(u16_i &1)    //select falling edge otherwise rising edge is selected
            pNandDrv->u16_Reg57_RELatch|=BIT0;
        pNandDrv->u16_Reg57_RELatch |= ((u16_i/2) << 1) &BIT_NC_LATCH_DATA_MASK;
        NC_Config();        
        u32_Err = NC_ReadID();
        if(u32_Err != UNFD_ST_SUCCESS)
        {
            nand_debug(0, 1, "ReadID Error with ErrCode 0x%lX\n", u32_Err);
            nand_die();
        }
        for(u16_j = 0; u16_j < NAND_ID_BYTE_CNT; u16_j++)
        {
            if(pNandDrv->au8_ID[u16_j] != au8_ID[u16_j])
            {
                break;
            }
        }
        if(u16_j == NAND_ID_BYTE_CNT)
        {
            if(u16_pass_begin == 0xFF)
                u16_pass_begin = u16_i;
            u16_pass_cnt ++;
        }
        //  break;
    }
    if(u16_pass_cnt == 0)
    {
        nand_debug(0, 1, "Read ID detect timing fails\n");
        pNandDrv->u16_Reg57_RELatch = BIT_NC_LATCH_DATA_2_0_T|BIT_NC_LATCH_STS_2_0_T;
        NC_Config();
        s8_ClkIdx = 0;
        *pu8_ClkIdx = (U8)s8_ClkIdx;

        return UNFD_ST_SUCCESS;        
    }
    else
    {
        u16_i = u16_pass_begin + u16_pass_cnt/2;
        pNandDrv->u16_Reg57_RELatch &= ~(BIT_NC_LATCH_DATA_MASK|BIT0);
        if(u16_i &1)    //select falling edge otherwise rising edge is selected
            pNandDrv->u16_Reg57_RELatch|=BIT0;
        pNandDrv->u16_Reg57_RELatch |= ((u16_i/2) << 1) &BIT_NC_LATCH_DATA_MASK;

    }
    //detect read status
    
    u16_pass_begin = 0xFF;
    u16_pass_cnt = 0;

    for(u16_i = 0 ; u16_i < 8 ; u16_i ++)
    {
        U16 u16_tmpStatus;
        pNandDrv->u16_Reg57_RELatch &= ~(BIT_NC_LATCH_STS_MASK);
        pNandDrv->u16_Reg57_RELatch |= ((u16_i) << 5) & BIT_NC_LATCH_STS_MASK;

        NC_Config();        
        NC_ReadStatus();
        REG_READ_UINT16(NC_ST_READ, u16_tmpStatus);

        if(u16_tmpStatus == u16_nandstatus)
        {
            if(u16_pass_begin == 0xFF)
                u16_pass_begin = u16_i;
            u16_pass_cnt ++;
        }
    }
    if(u16_pass_cnt == 0)
    {
        nand_debug(0, 1, "Read status detect timing fails\n");
        pNandDrv->u16_Reg57_RELatch = BIT_NC_LATCH_DATA_2_0_T|BIT_NC_LATCH_STS_2_0_T;
        NC_Config();
        s8_ClkIdx = 0;
        *pu8_ClkIdx = (U8)s8_ClkIdx;

        return UNFD_ST_SUCCESS;        
    }
    else
    {
        u16_i = u16_pass_begin + u16_pass_cnt/2;
        pNandDrv->u16_Reg57_RELatch &= ~(BIT_NC_LATCH_STS_MASK);
        pNandDrv->u16_Reg57_RELatch |= ((u16_i) << 5) & BIT_NC_LATCH_STS_MASK;
    }

    u16_RE_LATCH_DELAY = pNandDrv->u16_Reg57_RELatch;


    *pu8_ClkIdx = (U8)s8_ClkIdx;

    return UNFD_ST_SUCCESS;

}
#endif

U32 NC_FCIE5SetInterface(U8 u8_IfDDR, U8 u8_dqs_mode, U8 u8_dqs_delaycell, U8 u8_rd_ddr_timing)
{
    NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
    volatile U32 u32_Err = UNFD_ST_SUCCESS;

    if(u8_IfDDR)
    {
        #if !(defined(ENABLE_32BIT_MACRO) && ENABLE_32BIT_MACRO)
        //FIXME for real chip
        
        /*
        // set DQS_MODE
        pNandDrv->u16_Reg2C_SMStatus &= ~BIT_DQS_MODE_MASK;
        pNandDrv->u16_Reg2C_SMStatus |= u8_dqs_mode<<BIT_DQS_MDOE_SHIFT;
        REG_CLR_BITS_UINT16(NC_SM_STS, BIT_DQS_MODE_MASK);
        REG_SET_BITS_UINT16(NC_SM_STS, pNandDrv->u16_Reg2C_SMStatus & BIT_DQS_MODE_MASK);
        
        // set DQS_DELAY_CELL
        pNandDrv->u16_Reg2C_SMStatus &= ~BIT_DQS_DELAY_CELL_MASK;
        pNandDrv->u16_Reg2C_SMStatus |= u8_dqs_delaycell<<BIT_DQS_DELAY_CELL_SHIFT;
        REG_CLR_BITS_UINT16(NC_SM_STS, BIT_DQS_DELAY_CELL_MASK);
        REG_SET_BITS_UINT16(NC_SM_STS, pNandDrv->u16_Reg2C_SMStatus & BIT_DQS_DELAY_CELL_MASK);
        */
                // set RE latch timing
        pNandDrv->u16_Reg57_RELatch &= ~(BIT_RE_DDR_TIMING_MASK|BIT_NC_LATCH_DATA_MASK|BIT_NC_LATCH_STS_MASK);
        pNandDrv->u16_Reg57_RELatch |= u8_rd_ddr_timing << BIT_RE_DDR_TIMING_SHIFT;
        #else
        //FIXME for real chip
        
        //for 32bit DQS Mode = Latch Window, DDR Timing = skew clock phase
        /*
        pNandDrv->u16_EmmcPllReg62_LatWin &= ~BIT_LAT_WIN_MASK;
        pNandDrv->u16_EmmcPllReg62_LatWin |= u8_dqs_mode<<BIT_LAT_WIN_SHIFT;
        REG_CLR_BITS_UINT16(REG_LAT_WIN_SEL,BIT_LAT_WIN_MASK);  
        REG_SET_BITS_UINT16(REG_LAT_WIN_SEL,pNandDrv->u16_EmmcPllReg62_LatWin & BIT_LAT_WIN_MASK);  
        
        //for clock latch rising or falling
        if(u8_dqs_delaycell == 1)   //clock
            REG_SET_BITS_UINT16(NC_SM_STS, CLOCK_LATCH_RISING_FALLING);         
        else
            REG_CLR_BITS_UINT16(NC_SM_STS, CLOCK_LATCH_RISING_FALLING);
        //clock phase tuning
        
        pNandDrv->u16_EmmcPllReg09_PhaseSel &= ~BIT_EMMC_RXDLL_PHASE_SEL_MASK;
        pNandDrv->u16_EmmcPllReg09_PhaseSel |= u8_rd_ddr_timing << BIT_EMMC_RXDLL_PHASE_SEL_SHIFT;
        REG_CLR_BITS_UINT16(REG_EMMC_PLL_RX09, BIT_EMMC_RXDLL_PHASE_SEL_MASK);
        REG_CLR_BITS_UINT16(REG_EMMC_PLL_RX09, pNandDrv->u16_EmmcPllReg09_PhaseSel & BIT_EMMC_RXDLL_PHASE_SEL_MASK);
        */
        #endif

        // walk around timing bug
        pNandDrv->u16_Reg57_RELatch &= ~(BIT_NC_LATCH_DATA_MASK|BIT_NC_LATCH_STS_MASK|BIT_RE_SEC_TURN_CNT_MASK);
        pNandDrv->u16_Reg57_RELatch |= 0xE << BIT_RE_SEC_TURN_CNT_SHIFT;
        REG_WRITE_UINT16(NC_LATCH_DATA, pNandDrv->u16_Reg57_RELatch);
    }
    else
    {
        nand_pads_switch(NAND_PAD_BYPASS_MODE);
        u32_Err = NC_ResetNandFlash(); // switch ONFI to ASync Mode
    }

    return u32_Err;
}


#if defined( DDR_NAND_SUPPORT_RETRY_DQS) && DDR_NAND_SUPPORT_RETRY_DQS
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
    NC_FCIE5SetInterface(1, pNandDrv->tDefaultDDR.u8_DqsMode, pNandDrv->tDefaultDDR.u8_DelayCell, pNandDrv->tDefaultDDR.u8_DdrTiming);
}
#endif

U32 nand_config_clock(U16 u16_SeqAccessTime)
{

#if defined(DECIDE_CLOCK_BY_NAND) && DECIDE_CLOCK_BY_NAND
    NAND_DRIVER * pNandDrv = drvNAND_get_DrvContext_address();
    U32 u32_Err = 0;
    U32 au32_ClkTable[NFIE_CLK_TABLE_CNT] = NFIE_CLK_TABLE;

    #if  defined(DDR_NAND_SUPPORT) && DDR_NAND_SUPPORT
    U32 au32_4ClkTable[NFIE_4CLK_TABLE_CNT] = NFIE_4CLK_TABLE;
    U32 au32_1TTable[NFIE_4CLK_TABLE_CNT] = NFIE_4CLK_1T_TABLE;
    #endif

    U8 u8_ClkIdx = 0;

    #if  defined(DDR_NAND_SUPPORT) && DDR_NAND_SUPPORT
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
            
            NC_FCIE5SetInterface(1, pNandDrv->tDefaultDDR.u8_DqsMode,
                pNandDrv->tDefaultDDR.u8_DelayCell, pNandDrv->tDefaultDDR.u8_DdrTiming);
            
            if(nand_config_timing(au32_1TTable[pNandDrv->tDefaultDDR.u8_ClkIdx]) != UNFD_ST_SUCCESS)
            {
                nand_debug(UNFD_DEBUG_LEVEL_ERROR, 1, "Err, NAND, Cannot config nand timing\n");
                nand_die();
                return(u32_Err);
            }
        }
        //nand_debug(UNFD_DEBUG_LEVEL,1,"ok, get default DDR timing: 2Ch:%X, 57h:%X\n",
        //          pNandDrv->u16_Reg2C_SMStatus, pNandDrv->u16_Reg57_RELatch);
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
    REG_CLR_BITS_UINT16(reg_pwrgd_int_glirm, BIT_PWEGD_INT_GLIRM_MASK);
    REG_SET_BITS_UINT16(reg_pwrgd_int_glirm, (0x2<<10));
    #endif

    /* (1) Clear HW Enable */
    REG_WRITE_UINT16(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x00), 0x0001);
    REG_WRITE_UINT16(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x01),
                     PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x63);

    /* (2) Clear All Interrupt */
    REG_WRITE_UINT16(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x02), 0xffff);
    REG_WRITE_UINT16(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x03),
                     PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x60);

    /* (3) Reset Start */
    REG_WRITE_UINT16(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x04), 0x0014);
    REG_WRITE_UINT16(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x05),
                     PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x40);

    /* (4) STOP */
    REG_WRITE_UINT16(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x06), 0x0000);
    REG_WRITE_UINT16(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x07),
                     PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_STOP);

    REG_CLR_BITS_UINT16(NC_PWR_SAVE_CTL, BIT_SD_PWR_SAVE_RST);
    REG_SET_BITS_UINT16(NC_PWR_SAVE_CTL, BIT_SD_PWR_SAVE_RST);

    REG_CLR_BITS_UINT16(NC_PWR_SAVE_CTL, BIT_BAT_SD_PWR_SAVE_MASK|BIT_RST_SD_PWR_SAVE_MASK);
    
    REG_SET_BITS_UINT16(NC_PWR_SAVE_CTL, BIT_PWR_SAVE_MODE|BIT_PWR_SAVE_INT_EN);
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
U32 nand_translate_DMA_address_Ex(dma_addr_t dma_DMAAddr, U32 u32_ByteCnt, int mode)
{
    REG_CLR_BITS_UINT16(NC_MIU_DMA_SEL,BIT_MIU0_SELECTn);
    
    if( dma_DMAAddr >= MSTAR_MIU2_BUS_BASE)
    {
        REG_SET_BITS_UINT16( NC_MIU_DMA_SEL, BIT_MIU2_SELECT);
        dma_DMAAddr -= MSTAR_MIU2_BUS_BASE;
    }   
    else if( dma_DMAAddr >= MSTAR_MIU1_BUS_BASE)
    {
        REG_SET_BITS_UINT16( NC_MIU_DMA_SEL, BIT_MIU1_SELECT);
        dma_DMAAddr -= MSTAR_MIU1_BUS_BASE;
    }
    else
    {
        REG_CLR_BITS_UINT16(NC_MIU_DMA_SEL,BIT_MIU0_SELECTn);
        dma_DMAAddr -= MSTAR_MIU0_BUS_BASE;
    }

    return (U32)dma_DMAAddr;
}

U32 nand_translate_Spare_address_Ex(dma_addr_t dma_DMAAddr, U32 u32_ByteCnt, int mode)
{

    REG_CLR_BITS_UINT16(NC_MIU_DMA_SEL,BIT_SPARE_MIU_SEL_MASK);
    if( dma_DMAAddr >= MSTAR_MIU2_BUS_BASE)
    {
        REG_SET_BITS_UINT16( NC_MIU_DMA_SEL, BIT_SPARE_MIU2_SELECT);
        dma_DMAAddr -= MSTAR_MIU2_BUS_BASE;
    }
    else if( dma_DMAAddr >= MSTAR_MIU1_BUS_BASE)
    {
        REG_SET_BITS_UINT16( NC_MIU_DMA_SEL, BIT_SPARE_MIU1_SELECT);
        dma_DMAAddr -= MSTAR_MIU1_BUS_BASE;
    }
    else
    {
        REG_CLR_BITS_UINT16(NC_MIU_DMA_SEL,BIT_SPARE_MIU0_SELECTn);
        dma_DMAAddr -= MSTAR_MIU0_BUS_BASE;
    }

    return (U32)dma_DMAAddr;
}


dma_addr_t nand_DMA_MAP_address(void* p_Buffer, U32 u32_ByteCnt, int mode)
{
    dma_addr_t dma_addr;

    if(mode == 0)   //write
    {
        dma_addr = dma_map_single(&info->pdev->dev, p_Buffer, u32_ByteCnt, DMA_TO_DEVICE);
    }
    else
    {
        dma_addr = dma_map_single(&info->pdev->dev, p_Buffer, u32_ByteCnt, DMA_FROM_DEVICE);
    }
    return dma_addr;
}

void nand_DMA_UNMAP_address(dma_addr_t DMAAddr, U32 u32_ByteCnt, int mode)
{
    if(mode == 0)   //write
    {
        dma_unmap_single(&info->pdev->dev, DMAAddr, u32_ByteCnt, DMA_TO_DEVICE);
    }
    else
    {
        dma_unmap_single(&info->pdev->dev, DMAAddr, u32_ByteCnt, DMA_FROM_DEVICE);
    }
}

void nand_flush_cache_post_read(uintptr_t DMAAddr, U32 u32_ByteCnt)
{
}

void nand_flush_miu_pipe(void)
{

}


#if defined(ENABLE_NAND_INTERRUPT_MODE) && ENABLE_NAND_INTERRUPT_MODE
static DECLARE_WAIT_QUEUE_HEAD(fcie_wait);
static U16 u16CurNCMIEEvent = 0;            // Used to store current IRQ state
static volatile U32 SAR5_int = 0;

irqreturn_t NC_FCIE_IRQ(int irq, void *dummy)
{
    volatile u16 u16_Events;
    
    REG_READ_UINT16(NC_PWR_SAVE_CTL, u16_Events);                         
                                                                                                            
    if(u16_Events & BIT_PWR_SAVE_MODE_INT)                           
    {                                                                  
        REG_CLR_BITS_UINT16(NC_PWR_SAVE_CTL, BIT_PWR_SAVE_INT_EN);                                                
        nand_debug(UNFD_DEBUG_LEVEL_ERROR, 0, "SAR5 NAND WARN.\n");
		SAR5_int = 1;
        while(1);                                                                                               
                                                                       
        return IRQ_HANDLED;                                            
    }
    
    if((REG(NC_FUN_CTL) & BIT_NC_EN) != BIT_NC_EN)
        return IRQ_NONE;

    u16CurNCMIEEvent |= REG(NC_MIE_EVENT);
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
		if(SAR5_int == 1)
		{
			nand_debug(UNFD_DEBUG_LEVEL_ERROR, 0, "SAR5 NAND trap.\n");
			while(1);
		}    
        *u16_WaitedEvent = u16CurNCMIEEvent;
        u16CurNCMIEEvent = 0;
        REG_READ_UINT16(NC_MIE_EVENT, u16_Reg);     // Read all events
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
    REG_WRITE_UINT16(NC_MIE_INT_EN, BIT_NC_JOB_END_EN);
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

    REG_WRITE_UINT16(NC_FUN_CTL, BIT_NC_EN);
    
    REG_READ_UINT16(FCIE_NC_WBUF_CIFD_BASE, u16_Reg); // dummy read for WCIFD clock
    REG_READ_UINT16(FCIE_NC_WBUF_CIFD_BASE, u16_Reg); // dummy read for WCIFD clock

    REG_READ_UINT16(FCIE_NC_RBUF_CIFD_BASE, u16_Reg); // dummy read for RCIFD clock twice said by designer  
    REG_READ_UINT16(FCIE_NC_RBUF_CIFD_BASE, u16_Reg); // dummy read for RCIFD clock twice said by designer  

    #if defined(ENABLE_NAND_POWER_SAVING_MODE) && ENABLE_NAND_POWER_SAVING_MODE
    if(enable_sar5)
	    nand_Prepare_Power_Saving_Mode_Queue();
    #endif

    return UNFD_ST_SUCCESS;
}

U32 NC_PlatformInit(void)
{
    NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();

    nand_pads_init();
    pNandDrv->u8_WordMode = 0; // TV/Set-Top Box projects did not support x16 NAND flash
    nand_pads_switch(1);

    pNandDrv->u8_MacroType = MACRO_TYPE_32BIT;
    pNandDrv->u32_Clk =FCIE3_SW_SLOWEST_CLK;
    nand_clock_setting(FCIE3_SW_SLOWEST_CLK);
    pNandDrv->u16_Reg57_RELatch = BIT_NC_LATCH_DATA_2_0_T|BIT_NC_LATCH_STS_2_0_T;
    REG_WRITE_UINT16(NC_LATCH_DATA, pNandDrv->u16_Reg57_RELatch);   

    // print clock setting
    //printk(KERN_CRIT "reg_ckg_fcie(%08X)=%08X\n", reg_ckg_fcie, REG(reg_ckg_fcie));

    return UNFD_ST_SUCCESS;
}

U32 NC_CheckStorageType(void)
{
    U16 u16_regval;
    u16_regval = REG(NC_FUN_CTL);

    if( (u16_regval & BIT_NC_EN) != BIT_NC_EN )
    {
        return 0;
    }
    return 1;
}

U32 NC_CheckBlankPage(U8 * pu8_Buffer)
{
    NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
    U16 u16_ZeroBitCnt;
    
    if(pNandDrv->u8_RequireRandomizer != 1)
        REG_READ_UINT16(NC_ZERO_CNT,  u16_ZeroBitCnt);
    else 
        REG_READ_UINT16(NC_ZERO_CNT_SCRAMBLE,  u16_ZeroBitCnt);
        
    if(u16_ZeroBitCnt <= pNandDrv->u16_ECCCorretableBit)
    {
        nand_debug(UNFD_DEBUG_LEVEL_LOW, 0, "Buffer 0x%lX is blank page\n", (U32)pu8_Buffer);
        return 1;
    }
    return 0;
}

#else
  #error "Error! no platform functions."
#endif
