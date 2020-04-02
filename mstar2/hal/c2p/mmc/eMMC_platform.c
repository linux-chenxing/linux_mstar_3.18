//<MStar Software>
//******************************************************************************
// MStar Software
// Copyright (c) 2010 - 2012 MStar Semiconductor, Inc. All rights reserved.
// All software, firmware and related documentation herein ("MStar Software") are
// intellectual property of MStar Semiconductor, Inc. ("MStar") and protected by
// law, including, but not limited to, copyright law and international treaties.
// Any use, modification, reproduction, retransmission, or republication of all
// or part of MStar Software is expressly prohibited, unless prior written
// permission has been granted by MStar.
//
// By accessing, browsing and/or using MStar Software, you acknowledge that you
// have read, understood, and agree, to be bound by below terms ("Terms") and to
// comply with all applicable laws and regulations:
//
// 1. MStar shall retain any and all right, ownership and interest to MStar
//    Software and any modification/derivatives thereof.
//    No right, ownership, or interest to MStar Software and any
//    modification/derivatives thereof is transferred to you under Terms.
//
// 2. You understand that MStar Software might include, incorporate or be
//    supplied together with third party`s software and the use of MStar
//    Software may require additional licenses from third parties.
//    Therefore, you hereby agree it is your sole responsibility to separately
//    obtain any and all third party right and license necessary for your use of
//    such third party`s software.
//
// 3. MStar Software and any modification/derivatives thereof shall be deemed as
//    MStar`s confidential information and you agree to keep MStar`s
//    confidential information in strictest confidence and not disclose to any
//    third party.
//
// 4. MStar Software is provided on an "AS IS" basis without warranties of any
//    kind. Any warranties are hereby expressly disclaimed by MStar, including
//    without limitation, any warranties of merchantability, non-infringement of
//    intellectual property rights, fitness for a particular purpose, error free
//    and in conformity with any international standard.  You agree to waive any
//    claim against MStar for any loss, damage, cost or expense that you may
//    incur related to your use of MStar Software.
//    In no event shall MStar be liable for any direct, indirect, incidental or
//    consequential damages, including without limitation, lost of profit or
//    revenues, lost or damage of data, and unauthorized system use.
//    You agree that this Section 4 shall still apply without being affected
//    even if MStar Software has been modified by MStar in accordance with your
//    request or instruction for your use, except otherwise agreed by both
//    parties in writing.
//
// 5. If requested, MStar may from time to time provide technical supports or
//    services in relation with MStar Software to you for your use of
//    MStar Software in conjunction with your or your customer`s product
//    ("Services").
//    You understand and agree that, except otherwise agreed by both parties in
//    writing, Services are provided on an "AS IS" basis and the warranty
//    disclaimer set forth in Section 4 above shall apply.
//
// 6. Nothing contained herein shall be construed as by implication, estoppels
//    or otherwise:
//    (a) conferring any license or right to use MStar name, trademark, service
//        mark, symbol or any other identification;
//    (b) obligating MStar or any of its affiliates to furnish any person,
//        including without limitation, you and your customers, any assistance
//        of any kind whatsoever, or any information; or
//    (c) conferring any license or right under any intellectual property right.
//
// 7. These terms shall be governed by and construed in accordance with the laws
//    of Taiwan, R.O.C., excluding its conflict of law rules.
//    Any and all dispute arising out hereof or related hereto shall be finally
//    settled by arbitration referred to the Chinese Arbitration Association,
//    Taipei in accordance with the ROC Arbitration Law and the Arbitration
//    Rules of the Association by three (3) arbitrators appointed in accordance
//    with the said Rules.
//    The place of arbitration shall be in Taipei, Taiwan and the language shall
//    be English.
//    The arbitration award shall be final and binding to both parties.
//
//******************************************************************************
//<MStar Software>

#include "eMMC.h"
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>

#if defined(UNIFIED_eMMC_DRIVER) && UNIFIED_eMMC_DRIVER

U32 gu32_eMMCDrvExtFlag = 0;

#ifdef CONFIG_MSTAR_CLKM
#include <linux/clkm.h>

S32 gs32_eMMCCLKPHandle = 0;
S32 gs32_eMMCCLKHandle = 0;

static char *gu8clkeMMCtable[] =
{
    "g_nfie_clk_xtal",
    "g_nfie_clk_20",
    "g_nfie_clk_32",
    "g_nfie_clk_36",
    "g_nfie_clk_40",
    "g_nfie_clk_43",
    "g_nfie_clk_54",
    "g_nfie_clk62",
    "g_nfie_clk_72",
    "g_nfie_clk_86",
    "g_nfie_clk_5p4",
    "g_nfie_clk_1x_p",
    "g_nfie_clk_2x_p",
    "g_nfie_clk_300k",
    "g_nfie_clk_xtal_24",
    "g_nfie_clk_48",
};
#endif

#if defined(IF_FCIE_SHARE_IP) && IF_FCIE_SHARE_IP
// common for TV linux platform
extern bool ms_sdmmc_card_chg(unsigned int slotNo);
#endif

extern struct platform_device sg_mstar_emmc_device_st;
extern void (*sleep_or_delay)(U32);

void mdelay_MacroToFun(u32 time)
{
    mdelay(time);
}

U32 eMMC_hw_timer_delay(U32 u32us)
{
    #if 0 // udelay not work
    volatile u32 i = 0;

    for (i = 0; i < (u32us>>0); i++)
    {
        #if 1
        volatile int j = 0, tmp;
        for (j = 0; j < 0x38; j++)
        {
            tmp = j;
        }
        #endif
    }
    #else
    U32 u32_i = u32us;

    while(u32_i > 1000)
    {
        mdelay(1);
        u32_i -= 1000;
    }

    udelay(u32_i);
    #endif

    return u32us;
}

U32 eMMC_hw_timer_sleep(U32 u32ms)
{
    U32 u32_i = u32ms;

    while(u32_i > 1000)
    {
        if(sleep_or_delay!=NULL)
        {
                sleep_or_delay(1000);
        }

        u32_i -= 1000;
    }

    if(sleep_or_delay!=NULL)
    {
         sleep_or_delay(u32_i);
    }

    return u32ms;
}

//--------------------------------
// use to performance test
static U32 u32_ms_start;
U32 eMMC_hw_timer_start(void)
{
    u32_ms_start = jiffies_to_usecs(jiffies);
    return eMMC_ST_SUCCESS;
}

U32 eMMC_hw_timer_tick(void)
{
    return jiffies_to_usecs(jiffies) - u32_ms_start;
}

//--------------------------------
void eMMC_set_WatchDog(U8 u8_IfEnable)
{
    // do nothing
}

void eMMC_reset_WatchDog(void)
{
    // do nothing
}

//---------------------------------------

U32 eMMC_translate_DMA_address_Ex(dma_addr_t dma_DMAAddr, U32 u32_ByteCnt)
{
    REG_FCIE_CLRBIT(FCIE_MMA_PRI_REG, BIT_MIU_SELECT_MASK);

    if( dma_DMAAddr >= MSTAR_MIU1_BUS_BASE) // MIU1
    {
        REG_FCIE_SETBIT(FCIE_MMA_PRI_REG, BIT_MIU1_SELECT);
        dma_DMAAddr -= MSTAR_MIU1_BUS_BASE;
    }
    else // MIU0
    {
        dma_DMAAddr -= MSTAR_MIU0_BUS_BASE;
    }

    return ((U32)dma_DMAAddr);
}

dma_addr_t eMMC_DMA_MAP_address(uintptr_t ulongBuffer, U32 u32_ByteCnt, int mode)
{
    dma_addr_t dma_addr;

    #if defined(CONFIG_ENABLE_EMMC_ACP) && CONFIG_ENABLE_EMMC_ACP

    dma_addr = dma_map_single(&sg_mstar_emmc_device_st.dev, (void*)ulongBuffer, u32_ByteCnt, DMA_ACP);

    #else

    if(mode == 0)   //write
    {
        dma_addr = dma_map_single(&sg_mstar_emmc_device_st.dev, (void*)ulongBuffer, u32_ByteCnt, DMA_TO_DEVICE);
    }
    else
    {
        dma_addr = dma_map_single(&sg_mstar_emmc_device_st.dev, (void*)ulongBuffer, u32_ByteCnt, DMA_FROM_DEVICE);
    }

    if( dma_mapping_error(&sg_mstar_emmc_device_st.dev, dma_addr) )
    {
        dma_unmap_single(&sg_mstar_emmc_device_st.dev, dma_addr, u32_ByteCnt, (mode) ? DMA_FROM_DEVICE : DMA_TO_DEVICE);
        eMMC_die("eMMC_DMA_MAP_address: Kernel can't mapping dma correctly\n");
    }

    #endif

    return(dma_addr);
}

void eMMC_DMA_UNMAP_address(dma_addr_t dma_DMAAddr, U32 u32_ByteCnt, int mode)
{
    #if defined(CONFIG_ENABLE_EMMC_ACP) && CONFIG_ENABLE_EMMC_ACP

    dma_unmap_single(&sg_mstar_emmc_device_st.dev, dma_DMAAddr, u32_ByteCnt, DMA_ACP);

    #else

    if(mode == 0)   //write
    {
        dma_unmap_single(&sg_mstar_emmc_device_st.dev, dma_DMAAddr, u32_ByteCnt, DMA_TO_DEVICE);
    }
    else
    {
        dma_unmap_single(&sg_mstar_emmc_device_st.dev, dma_DMAAddr, u32_ByteCnt, DMA_FROM_DEVICE);
    }

    #endif
}

void eMMC_flush_data_cache_buffer(uintptr_t ulongDMAAddr, U32 u32_ByteCnt)
{
    Chip_Clean_Cache_Range_VA_PA(ulongDMAAddr,__pa(ulongDMAAddr), u32_ByteCnt);
}

void eMMC_Invalidate_data_cache_buffer(uintptr_t ulongDMAAddr, U32 u32_ByteCnt)
{
    Chip_Inv_Cache_Range_VA_PA(ulongDMAAddr,__pa(ulongDMAAddr), u32_ByteCnt);
}

void eMMC_flush_miu_pipe(void)
{
}

void eMMC_DumpChiptop(void)
{
    U16 u16_i, u16_reg;

    eMMC_debug(eMMC_DEBUG_LEVEL, 1, "\n\n chiptop:");

    for(u16_i=0 ; u16_i<0x80; u16_i++)
    {
        if(0 == u16_i%8)
            eMMC_debug(eMMC_DEBUG_LEVEL,0,"\n%02Xh:| ", u16_i);

        REG_FCIE_R(GET_REG_ADDR(PAD_CHIPTOP_BASE, u16_i), u16_reg);
        eMMC_debug(eMMC_DEBUG_LEVEL, 0, "%04Xh ", u16_reg);

        if((u16_i & 0x7) == 0x7)
            eMMC_debug(eMMC_DEBUG_LEVEL, 0, "| %02Xh ", u16_i);
    }
}

#if defined(ENABLE_EMMC_POWER_SAVING_MODE) && ENABLE_EMMC_POWER_SAVING_MODE
void eMMC_CheckPowerCut(void)
{
}

void eMMC_Prepare_Power_Saving_Mode_Queue(void)
{
    REG_FCIE_SETBIT(reg_pwrgd_int_glirm, BIT_PWRGD_INT_GLIRM_EN);

    REG_FCIE_CLRBIT(reg_pwrgd_int_glirm, BIT_PWEGD_INT_GLIRM_MASK);
    REG_FCIE_SETBIT(reg_pwrgd_int_glirm, (0x2<<10));

    /* (1) Clear HW Enable */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x00), 0x0000);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x01),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x07);

    /* (2) Clear All Interrupt */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x02), 0xffff);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x03),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x00);

    /* (3) Clear SD MODE Enable */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x04), 0x0000);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x05),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x0B);

    /* (4) Clear SD CTL Enable */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x06), 0x0000);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x07),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x0C);

    /* (5) Reset Start */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x08), 0x0000);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x09),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x3F);

    /* (6) Reset End */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x0A), 0x0001);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x0B),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x3F);

    /* (7) Set "SD_MOD" */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x0C), 0x0021);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x0D),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x0B);

    /* (8) Enable "reg_sd_en" */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x0E), 0x0001);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x0F),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x07);

    /* (9) Command Content, IDLE */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x10), 0x0040);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x11),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x20);

    /* (10) Command Content, STOP */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x12), 0x0000);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x13),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x21);

    /* (11) Command Content, STOP */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x14), 0x0000);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x15),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x22);

    /* (12) Command & Response Size */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x16), 0x0500);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x17),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x0A);

    /* (13) Enable Interrupt, SD_CMD_END */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x18), 0x0002);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x19),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x01);

    /* (14) Command Enable + job Start */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x1A), 0x0044);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x1B),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x0C);

    /* (15) Wait Interrupt */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x1C), 0x0000);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x1D),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WINT);

    /* (16) STOP */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x1E), 0x0000);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x1F),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_STOP);

    REG_FCIE_CLRBIT(FCIE_PWR_SAVE_CTL, BIT_SD_POWER_SAVE_RST);
    REG_FCIE_SETBIT(FCIE_PWR_SAVE_CTL, BIT_SD_POWER_SAVE_RST);

    REG_FCIE_SETBIT(FCIE_PWR_SAVE_CTL, BIT_POWER_SAVE_MODE|BIT_POWER_SAVE_MODE_INT_EN);
}

#endif

// [FIXME] -->
void eMMC_DumpPadClk(void)
{
    eMMC_debug(0, 0, "\nclk setting:\n");
    eMMC_debug(0, 0, "reg_ckg_fcie(0x%lX):0x%x\n", reg_ckg_fcie ,REG_FCIE_U16(reg_ckg_fcie));
    eMMC_debug(0, 0, "FCIE Clk: %uKHz\n", g_eMMCDrv.u32_ClkKHz);
    eMMC_debug(0, 0, "Reg Val: %Xh\n", g_eMMCDrv.u16_ClkRegVal);

    //---------------------------------------------------------------------
    eMMC_debug(0, 0, "[pad setting]:\n");
    switch(g_eMMCDrv.u8_PadType)
    {
        case FCIE_eMMC_BYPASS:          eMMC_debug(0, 0, "Bypass\n");           break;
        case FCIE_eMMC_SDR:             eMMC_debug(0, 0, "SDR\n");  break;
        case FCIE_eMMC_DDR:             eMMC_debug(0, 0, "DDR\n");  break;
        case FCIE_eMMC_HS200:           eMMC_debug(0, 0, "HS200\n");            break;
        case FCIE_eMMC_HS400:           eMMC_debug(0, 0, "HS400\n");         break;
        default:
            eMMC_debug(0, 0, "eMMC Err: Pad unknown, %d\n", g_eMMCDrv.u8_PadType); eMMC_die("\n");
            break;
    }
}

U32 eMMC_pads_switch(U32 u32Mode)
{
    g_eMMCDrv.u8_PadType = u32Mode;

    // chiptop
    REG_FCIE_CLRBIT(reg_chiptop_0x08, BIT_SD_CONFIG);
    REG_FCIE_CLRBIT(reg_chiptop_0x08, BIT_NAND_MODE);
    REG_FCIE_SETBIT(reg_chiptop_0x08, BIT_EMMC_CONFIG);
    REG_FCIE_CLRBIT(reg_chiptop_0x50, BIT_ALL_PAD_IN);

    // fcie
    REG_FCIE_CLRBIT(FCIE_DDR_MODE,
        BIT_PAD_IN_MASK|BIT_FALL_LATCH|BIT_PAD_IN_SEL_SD|BIT_32BIT_MACRO_EN|BIT_CLK2_SEL|BIT_DDR_EN|BIT_8BIT_MACRO_EN);

    // emmc_pll
    REG_FCIE_CLRBIT(reg_emmcpll_0x20, BIT9|BIT10);

    REG_FCIE_CLRBIT(reg_emmcpll_0x1a, BIT10|BIT5|BIT4);

    REG_FCIE_CLRBIT(reg_emmcpll_0x1c, BIT8|BIT9);

    REG_FCIE_CLRBIT(reg_emmcpll_0x63, BIT0);

    REG_FCIE_CLRBIT(reg_emmcpll_0x68, BIT0|BIT1);

    REG_FCIE_CLRBIT(reg_emmcpll_0x69, BIT7|BIT6|BIT5|BIT4);
    REG_FCIE_CLRBIT(reg_emmcpll_0x69, BIT3);

    REG_FCIE_CLRBIT(reg_emmcpll_0x6a, BIT0|BIT1);

    REG_FCIE_W(reg_emmcpll_0x6b, 0x0000);

    REG_FCIE_CLRBIT(reg_emmcpll_0x6d, BIT0);

    REG_FCIE_CLRBIT(reg_emmcpll_0x70, BIT11|BIT10|BIT8);

    REG_FCIE_W(reg_emmcpll_0x71, 0xFFFF);

    REG_FCIE_W(reg_emmcpll_0x73, 0xFFFF);

    REG_FCIE_CLRBIT(reg_emmcpll_0x7f, BIT8|BIT3|BIT2|BIT1);

    switch(u32Mode)
    {
        case FCIE_eMMC_BYPASS:
            eMMC_debug(eMMC_DEBUG_LEVEL_HIGH, 1, "Bypass\n");
            REG_FCIE_SETBIT(FCIE_DDR_MODE, BIT_PAD_IN_MASK|BIT_PAD_IN_SEL_SD|BIT_FALL_LATCH|BIT_CLK2_SEL);
            REG_FCIE_SETBIT(reg_emmcpll_0x1a, BIT10);
            break;

        case FCIE_eMMC_SDR:
            eMMC_debug(eMMC_DEBUG_LEVEL_HIGH, 1, "SDR\n");
            // fcie
            REG_FCIE_SETBIT(FCIE_DDR_MODE, BIT_8BIT_MACRO_EN);

            // emmc_pll
            REG_FCIE_SETBIT(reg_emmcpll_0x68, BIT0);
            REG_FCIE_SETBIT(reg_emmcpll_0x1c, BIT9);

            break;

        case FCIE_eMMC_DDR:
            eMMC_debug(eMMC_DEBUG_LEVEL_HIGH, 1, "DDR\n");

            // fcie
            REG_FCIE_SETBIT(FCIE_DDR_MODE, BIT_DDR_EN|BIT_8BIT_MACRO_EN);

            // emmc_pll
            REG_FCIE_SETBIT(reg_emmcpll_0x68, BIT0);
            REG_FCIE_SETBIT(reg_emmcpll_0x1c, BIT9);

            if(g_eMMCDrv.u8_BUS_WIDTH == BIT_SD_DATA_WIDTH_4)
            {
                REG_FCIE_SETBIT(reg_emmcpll_0x6a, 1<<0);
                REG_FCIE_W(reg_emmcpll_0x6b, 0x0213);
            }
            else if(g_eMMCDrv.u8_BUS_WIDTH == BIT_SD_DATA_WIDTH_8)
            {
                REG_FCIE_SETBIT(reg_emmcpll_0x6a, 2<<0);
                REG_FCIE_W(reg_emmcpll_0x6b, 0x0113);
            }
            else
                goto ErrorHandle;

            REG_FCIE_SETBIT(reg_emmcpll_0x6d, BIT0);

            break;

        case FCIE_eMMC_HS200:
            eMMC_debug(eMMC_DEBUG_LEVEL_HIGH, 1, "HS200\n");

            // fcie
            REG_FCIE_SETBIT(FCIE_DDR_MODE, BIT_32BIT_MACRO_EN);

            // emmc_pll
            REG_FCIE_CLRBIT(reg_emmcpll_0x03, 0x0FFF);

            REG_FCIE_SETBIT(reg_emmcpll_0x1a, BIT10|BIT5|BIT4);

            REG_FCIE_SETBIT(reg_emmcpll_0x1c, BIT8);

            REG_FCIE_SETBIT(reg_emmcpll_0x20, BIT10|BIT9);

            REG_FCIE_SETBIT(reg_emmcpll_0x68, BIT0);

            REG_FCIE_CLRBIT(reg_emmcpll_0x69, BIT3);
            REG_FCIE_CLRBIT(reg_emmcpll_0x69, 0xF<<4);
            REG_FCIE_SETBIT(reg_emmcpll_0x69, 4<<4);

            if(g_eMMCDrv.u8_BUS_WIDTH == BIT_SD_DATA_WIDTH_4)
            {
                REG_FCIE_SETBIT(reg_emmcpll_0x6a, 1<<0);
                REG_FCIE_W(reg_emmcpll_0x6b, 0x0413);
            }
            else if(g_eMMCDrv.u8_BUS_WIDTH == BIT_SD_DATA_WIDTH_8)
            {
                REG_FCIE_SETBIT(reg_emmcpll_0x6a, 2<<0);
                REG_FCIE_W(reg_emmcpll_0x6b, 0x0213);
            }
            else
                goto ErrorHandle;

            REG_FCIE_SETBIT(reg_emmcpll_0x70, BIT8);

            REG_FCIE_W(reg_emmcpll_0x71, 0xF800);

            REG_FCIE_W(reg_emmcpll_0x73, 0xFD00);

            #if defined(ENABLE_eMMC_AFIFO) && ENABLE_eMMC_AFIFO
            REG_FCIE_SETBIT(reg_emmcpll_0x70, BIT11|BIT10);
            #endif

            break;

        case FCIE_eMMC_HS400:
            eMMC_debug(eMMC_DEBUG_LEVEL_HIGH, 1, "HS400\n");

            // fcie
            REG_FCIE_SETBIT(FCIE_DDR_MODE, BIT_32BIT_MACRO_EN|BIT_DDR_EN);

            // emmc_pll
            REG_FCIE_CLRBIT(reg_emmcpll_0x03, 0x0F0F);

            REG_FCIE_SETBIT(reg_emmcpll_0x1a, BIT10|BIT5|BIT4);

            REG_FCIE_SETBIT(reg_emmcpll_0x1c, BIT8);

            REG_FCIE_SETBIT(reg_emmcpll_0x20, BIT10);

            REG_FCIE_SETBIT(reg_emmcpll_0x63, BIT0);

            REG_FCIE_SETBIT(reg_emmcpll_0x68, BIT0|BIT1);

            REG_FCIE_CLRBIT(reg_emmcpll_0x69, BIT3);
            REG_FCIE_CLRBIT(reg_emmcpll_0x69, 0xF<<4);
            REG_FCIE_SETBIT(reg_emmcpll_0x69, 6<<4);

            if(g_eMMCDrv.u8_BUS_WIDTH == BIT_SD_DATA_WIDTH_4)
            {
                REG_FCIE_SETBIT(reg_emmcpll_0x6a, 1<<0);
                REG_FCIE_W(reg_emmcpll_0x6b, 0x0213);
            }
            else if(g_eMMCDrv.u8_BUS_WIDTH == BIT_SD_DATA_WIDTH_8)
            {
                REG_FCIE_SETBIT(reg_emmcpll_0x6a, 2<<0);
                REG_FCIE_W(reg_emmcpll_0x6b, 0x0113);
            }
            else
                goto ErrorHandle;

            REG_FCIE_SETBIT(reg_emmcpll_0x70, BIT8);

            REG_FCIE_W(reg_emmcpll_0x71, 0xF800);

            REG_FCIE_W(reg_emmcpll_0x73, 0xFD00);

            #if defined(ENABLE_eMMC_AFIFO) && ENABLE_eMMC_AFIFO

            REG_FCIE_SETBIT(reg_emmcpll_0x70, BIT11|BIT10);

            REG_FCIE_SETBIT(reg_emmcpll_0x7f, BIT2);

            #endif

            break;

        default:
            eMMC_debug(1, 1, "eMMC Err: wrong parameter for switch pad func\n");
            return eMMC_ST_ERR_PARAMETER;
    }

    // 8 bits macro reset + 32 bits macro reset
    REG_FCIE_CLRBIT(reg_emmcpll_0x6f, BIT0|BIT1);
    REG_FCIE_SETBIT(reg_emmcpll_0x6f, BIT0|BIT1); // 8 bits macro reset + 32 bits macro reset

    g_eMMCDrv.u32_DrvFlag &= (~(DRV_FLAG_SPEED_HIGH|DRV_FLAG_DDR_MODE|DRV_FLAG_SPEED_HS200|DRV_FLAG_SPEED_HS400));

    if( g_eMMCDrv.u8_PadType == FCIE_eMMC_BYPASS )
    {
    }
    else if( g_eMMCDrv.u8_PadType == FCIE_eMMC_SDR )
    {
        g_eMMCDrv.u32_DrvFlag |= DRV_FLAG_SPEED_HIGH;
    }
    else if( g_eMMCDrv.u8_PadType == FCIE_eMMC_DDR )
    {
        g_eMMCDrv.u32_DrvFlag |= DRV_FLAG_DDR_MODE|DRV_FLAG_SPEED_HIGH;
    }
    else if( g_eMMCDrv.u8_PadType == FCIE_eMMC_HS200 )
    {
        g_eMMCDrv.u32_DrvFlag |= DRV_FLAG_SPEED_HS200;
    }
    else if( g_eMMCDrv.u8_PadType == FCIE_eMMC_HS400)
    {
        g_eMMCDrv.u32_DrvFlag |= DRV_FLAG_DDR_MODE|DRV_FLAG_SPEED_HS400;
    }

    return eMMC_ST_SUCCESS;

    ErrorHandle:

    eMMC_debug(1, 1, "eMMC Err: set bus width before pad switch\n");

    return eMMC_ST_ERR_INVALID_PARAM;
}

U32 eMMC_pll_setting(U16 u16_ClkParam)
{
    U32 u32_value_reg_emmc_pll_pdiv;
    static U16 u16_OldClkParam=0xFFFF;

    if(u16_ClkParam == u16_OldClkParam)
        return eMMC_ST_SUCCESS;
    else
        u16_OldClkParam = u16_ClkParam;

    // 1. reset emmc pll
    REG_FCIE_SETBIT(reg_emmc_pll_reset,BIT0);
    REG_FCIE_CLRBIT(reg_emmc_pll_reset,BIT0);

    // 2. synth clock
    switch(u16_ClkParam)
    {
        case eMMC_PLL_CLK_200M: // 200M
            REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
            REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
            #if 0
            REG_FCIE_SETBIT(reg_ddfset_23_16,0x22); // 20xMHz
            REG_FCIE_SETBIT(reg_ddfset_15_00,0x8F5C);
            #else
            REG_FCIE_SETBIT(reg_ddfset_23_16,0x24); // 195MHz
            REG_FCIE_SETBIT(reg_ddfset_15_00,0x03D8);
            #endif
            u32_value_reg_emmc_pll_pdiv =1;// PostDIV: 2
            break;

        case eMMC_PLL_CLK_160M:
            REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
            REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
            REG_FCIE_SETBIT(reg_ddfset_23_16,0x2B);
            REG_FCIE_SETBIT(reg_ddfset_15_00,0x3333);
            u32_value_reg_emmc_pll_pdiv =1;// PostDIV: 2
            break;

        case eMMC_PLL_CLK_140M:
            REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
            REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
            REG_FCIE_SETBIT(reg_ddfset_23_16,0x31);
            REG_FCIE_SETBIT(reg_ddfset_15_00,0x5F15);
            u32_value_reg_emmc_pll_pdiv = 1;// PostDIV: 2
            break;

        case eMMC_PLL_CLK_120M:
            REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
            REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
            REG_FCIE_SETBIT(reg_ddfset_23_16,0x39);
            REG_FCIE_SETBIT(reg_ddfset_15_00,0x9999);
            u32_value_reg_emmc_pll_pdiv = 1;// PostDIV: 2
            break;

        case eMMC_PLL_CLK_100M:
            REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
            REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
            REG_FCIE_SETBIT(reg_ddfset_23_16,0x45);
            REG_FCIE_SETBIT(reg_ddfset_15_00,0x1EB8);
            u32_value_reg_emmc_pll_pdiv = 1;// PostDIV: 2
            break;

        case eMMC_PLL_CLK__86M:
            REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
            REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
            REG_FCIE_SETBIT(reg_ddfset_23_16,0x28);
            REG_FCIE_SETBIT(reg_ddfset_15_00,0x2FA0);
            u32_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
            break;

        case eMMC_PLL_CLK__80M:
            REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
            REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
            REG_FCIE_SETBIT(reg_ddfset_23_16,0x2B);
            REG_FCIE_SETBIT(reg_ddfset_15_00,0x3333);
            u32_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
            break;

        case eMMC_PLL_CLK__72M:
            REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
            REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
            REG_FCIE_SETBIT(reg_ddfset_23_16,0x30);
            REG_FCIE_SETBIT(reg_ddfset_15_00,0x0000);
            u32_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
            break;

        case eMMC_PLL_CLK__62M:
            REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
            REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
            REG_FCIE_SETBIT(reg_ddfset_23_16,0x37);
            REG_FCIE_SETBIT(reg_ddfset_15_00,0xBDEF);
            u32_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
            break;

        case eMMC_PLL_CLK__52M:
            REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
            REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
            REG_FCIE_SETBIT(reg_ddfset_23_16,0x42);
            REG_FCIE_SETBIT(reg_ddfset_15_00,0x7627);
            u32_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
            break;

        case eMMC_PLL_CLK__48M:
            REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
            REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
            REG_FCIE_SETBIT(reg_ddfset_23_16,0x48);
            REG_FCIE_SETBIT(reg_ddfset_15_00,0x0000);
            u32_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
            break;

        case eMMC_PLL_CLK__40M:
            REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
            REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
            REG_FCIE_SETBIT(reg_ddfset_23_16,0x2B);
            REG_FCIE_SETBIT(reg_ddfset_15_00,0x3333);
            u32_value_reg_emmc_pll_pdiv = 4;// PostDIV: 8
            break;

        case eMMC_PLL_CLK__36M:
            REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
            REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
            REG_FCIE_SETBIT(reg_ddfset_23_16,0x30);
            REG_FCIE_SETBIT(reg_ddfset_15_00,0x0000);
            u32_value_reg_emmc_pll_pdiv = 4;// PostDIV: 8
            break;

        case eMMC_PLL_CLK__32M:
            REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
            REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
            REG_FCIE_SETBIT(reg_ddfset_23_16,0x36);
            REG_FCIE_SETBIT(reg_ddfset_15_00,0x0000);
            u32_value_reg_emmc_pll_pdiv = 4;// PostDIV: 8
            break;

        case eMMC_PLL_CLK__27M:
            REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
            REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
            REG_FCIE_SETBIT(reg_ddfset_23_16,0x40);
            REG_FCIE_SETBIT(reg_ddfset_15_00,0x0000);
            u32_value_reg_emmc_pll_pdiv = 4;// PostDIV: 8
            break;

        case eMMC_PLL_CLK__20M:
            REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
            REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
            REG_FCIE_SETBIT(reg_ddfset_23_16,0x2B);
            REG_FCIE_SETBIT(reg_ddfset_15_00,0x3333);
            u32_value_reg_emmc_pll_pdiv = 7;// PostDIV: 16
            break;

        default:
            eMMC_debug(0,0,"eMMC Err: emmc PLL not configed %Xh\n", u16_ClkParam);
            eMMC_die(" ");
            return eMMC_ST_ERR_UNKNOWN_CLK;
    }

    // 3. VCO clock ( loop N = 4 )
    REG_FCIE_CLRBIT(reg_emmcpll_fbdiv,0xffff);
    REG_FCIE_SETBIT(reg_emmcpll_fbdiv,0x6);// PostDIV: 8

    // 4. 1X clock
    REG_FCIE_CLRBIT(reg_emmcpll_pdiv,BIT2|BIT1|BIT0);
    REG_FCIE_SETBIT(reg_emmcpll_pdiv,u32_value_reg_emmc_pll_pdiv);// PostDIV: 8

    if(u16_ClkParam==eMMC_PLL_CLK__20M)
    {
        REG_FCIE_SETBIT(reg_emmc_pll_test, BIT10);
    }
    else
    {
        REG_FCIE_CLRBIT(reg_emmc_pll_test, BIT10);
    }

    eMMC_hw_timer_delay(HW_TIMER_DELAY_100us); // asked by Irwin

    return eMMC_ST_SUCCESS;
}

void eMMC_pll_dll_setting(U16 u16_ClkParam)
{
    volatile U16 u16_reg;
    static U16 u16_OldClkParam=0xFFFF;

    if(u16_ClkParam == u16_OldClkParam)
        return ;
    else
        u16_OldClkParam = u16_ClkParam;

    REG_FCIE_CLRBIT(reg_emmcpll_0x09, BIT0); // ­«½Æ reg_emmc_rxdll_dline_en

    // Reset eMMC_DLL
    REG_FCIE_SETBIT(REG_EMMC_PLL_RX30, BIT2);
    REG_FCIE_CLRBIT(REG_EMMC_PLL_RX30, BIT2);

    //DLL pulse width and phase
    REG_FCIE_W(REG_EMMC_PLL_RX01, 0x7F72);

    // DLL code
    REG_FCIE_W(REG_EMMC_PLL_RX32, 0xF200);

    // DLL calibration
    REG_FCIE_W(REG_EMMC_PLL_RX30, 0x3378);
    REG_FCIE_SETBIT(REG_EMMC_PLL_RX33, BIT15);

    // Wait 250us
    eMMC_hw_timer_delay(250);

    // Get hw dll0 code
    REG_FCIE_R(REG_EMMC_PLL_RX33, u16_reg);

    //eMMC_debug(eMMC_DEBUG_LEVEL_LOW, 1, "EMMCPLL 0x33=%04Xh\n", u16_reg);

    REG_FCIE_CLRBIT(REG_EMMC_PLL_RX34, (BIT10 - 1));

    // Set dw dll0 code
    REG_FCIE_SETBIT(REG_EMMC_PLL_RX34, u16_reg & 0x03FF);

    // Disable reg_hw_upcode_en
    REG_FCIE_CLRBIT(REG_EMMC_PLL_RX30, BIT9);

    // Clear reg_emmc_dll_test[7]
    REG_FCIE_CLRBIT(reg_emmcpll_0x02, BIT15); // switch pad ¾Þ§@ bit13, excel undefine ??

    // Enable reg_rxdll_dline_en
    REG_FCIE_SETBIT(reg_emmcpll_0x09, BIT0); // ­«½Æ

}

// Notice!!! you need to set pad before config clock
U32 eMMC_clock_setting(U16 u16_ClkParam)
{
    eMMC_debug(eMMC_DEBUG_LEVEL_LOW, 1, "eMMC_clock_setting(%Xh)\n", u16_ClkParam);

//    eMMC_PlatformResetPre();

    REG_FCIE_CLRBIT(FCIE_SD_MODE, BIT_SD_CLK_EN);

    switch(u16_ClkParam)
    {
        // emmc_pll clock
        case eMMC_PLL_CLK__20M  : g_eMMCDrv.u32_ClkKHz =  20000; break;
        case eMMC_PLL_CLK__27M  : g_eMMCDrv.u32_ClkKHz =  27000; break;
        case eMMC_PLL_CLK__32M  : g_eMMCDrv.u32_ClkKHz =  32000; break;
        case eMMC_PLL_CLK__36M  : g_eMMCDrv.u32_ClkKHz =  36000; break;
        case eMMC_PLL_CLK__40M  : g_eMMCDrv.u32_ClkKHz =  40000; break;
        case eMMC_PLL_CLK__48M  : g_eMMCDrv.u32_ClkKHz =  48000; break;
        case eMMC_PLL_CLK__52M  : g_eMMCDrv.u32_ClkKHz =  52000; break;
        case eMMC_PLL_CLK__62M  : g_eMMCDrv.u32_ClkKHz =  62000; break;
        case eMMC_PLL_CLK__72M  : g_eMMCDrv.u32_ClkKHz =  72000; break;
        case eMMC_PLL_CLK__80M  : g_eMMCDrv.u32_ClkKHz =  80000; break;
        case eMMC_PLL_CLK__86M  : g_eMMCDrv.u32_ClkKHz =  86000; break;
        case eMMC_PLL_CLK_100M  : g_eMMCDrv.u32_ClkKHz = 100000; break;
        case eMMC_PLL_CLK_120M  : g_eMMCDrv.u32_ClkKHz = 120000; break;
        case eMMC_PLL_CLK_140M  : g_eMMCDrv.u32_ClkKHz = 140000; break;
        case eMMC_PLL_CLK_160M  : g_eMMCDrv.u32_ClkKHz = 160000; break;
        case eMMC_PLL_CLK_200M  : g_eMMCDrv.u32_ClkKHz = 200000; break;

        // clock_gen fcie clock
        case BIT_FCIE_CLK_12M   : g_eMMCDrv.u32_ClkKHz =  12000; break;
        case BIT_FCIE_CLK_20M   : g_eMMCDrv.u32_ClkKHz =  20000; break;
        case BIT_FCIE_CLK_32M   : g_eMMCDrv.u32_ClkKHz =  32000; break;
        case BIT_FCIE_CLK_36M   : g_eMMCDrv.u32_ClkKHz =  36000; break;
        case BIT_FCIE_CLK_40M   : g_eMMCDrv.u32_ClkKHz =  40000; break;
        case BIT_FCIE_CLK_43_2M : g_eMMCDrv.u32_ClkKHz =  43200; break;
        case BIT_FCIE_CLK_300K  : g_eMMCDrv.u32_ClkKHz =    300; break;
        case BIT_FCIE_CLK_24M   : g_eMMCDrv.u32_ClkKHz =  24000; break;
        case BIT_FCIE_CLK_48M   : g_eMMCDrv.u32_ClkKHz =  48000; break;

        default:
            eMMC_debug(1, 1, "eMMC Err: clkgen %Xh\n", eMMC_ST_ERR_INVALID_PARAM);
            eMMC_die(" ");
            return eMMC_ST_ERR_INVALID_PARAM;
    }

    if(u16_ClkParam & eMMC_PLL_FLAG)
    {
        //eMMC_debug(0,0,"eMMC PLL: %Xh\n", u16_ClkParam);

        #ifndef CONFIG_MSTAR_CLKM
        REG_FCIE_CLRBIT(reg_ckg_fcie, BIT_FCIE_CLK_GATING|BIT_FCIE_CLK_INVERSE|BIT_CLKGEN_FCIE_MASK);
        #endif

        REG_FCIE_CLRBIT(reg_ckg_mpll, BIT_CKG_MPLL_GATING|BIT_CKG_MPLL_INVERSE|BIT_CLKGEN2_CLK_SEL_MASK);
        REG_FCIE_SETBIT(reg_ckg_mpll, BIT_CLKGEN2_CLK_SEL);

        eMMC_pll_setting(u16_ClkParam);

        if( (g_eMMCDrv.u32_DrvFlag & (DRV_FLAG_DDR_MODE|DRV_FLAG_SPEED_HS400))==(DRV_FLAG_DDR_MODE|DRV_FLAG_SPEED_HS400) ) // HS400
        {
            #ifdef CONFIG_MSTAR_CLKM
            set_clk_source(gs32_eMMCCLKHandle, "g_nfie_clk_xtal");
            set_clk_source(gs32_eMMCCLKPHandle, "g_nfie_clk_2x_p");
            set_clk_source(gs32_eMMCCLKHandle, "g_nfie_clk_p");
            #else
            REG_FCIE_SETBIT(reg_ckg_fcie, BIT_FCIE_CLK_SRC_SEL|BIT_FCIE_CLK_EMMC_PLL_2X<<2);
            #endif
            eMMC_pll_dll_setting(u16_ClkParam); // tuning DLL setting
			//500msec= 0x10000 x 1/100MHZ x 0x2FB
 	        REG_FCIE_W(FCIE_WR_SBIT_TIMER, BIT_WR_SBIT_TIMER_EN|0x2FB);
	        REG_FCIE_W(FCIE_RD_SBIT_TIMER, BIT_RD_SBIT_TIMER_EN|0x2FB);            
        }
        else
        {
            #ifdef CONFIG_MSTAR_CLKM
            set_clk_source(gs32_eMMCCLKHandle, "g_nfie_clk_xtal");
            set_clk_source(gs32_eMMCCLKPHandle, "g_nfie_clk_1x_p");
            set_clk_source(gs32_eMMCCLKHandle, "g_nfie_clk_p");
            #else
            REG_FCIE_SETBIT(reg_ckg_fcie, BIT_FCIE_CLK_SRC_SEL|BIT_FCIE_CLK_EMMC_PLL_1X<<2);
            #endif
			//500msec= 0x10000 x 1/50MHZ x 0x17E
		    REG_FCIE_W(FCIE_WR_SBIT_TIMER, BIT_WR_SBIT_TIMER_EN|0x17E);
	        REG_FCIE_W(FCIE_RD_SBIT_TIMER, BIT_RD_SBIT_TIMER_EN|0x17E);
        }
    }
    else
    {
        //eMMC_debug(0,0,"eMMC CLKGEN: %Xh\n", u16_ClkParam);
        #ifdef CONFIG_MSTAR_CLKM
        set_clk_source(gs32_eMMCCLKHandle, "g_nfie_clk_xtal");
        set_clk_source(gs32_eMMCCLKPHandle, gu8clkeMMCtable[u16_ClkParam]);
        set_clk_source(gs32_eMMCCLKHandle, "g_nfie_clk_p");
        #else
        REG_FCIE_CLRBIT(reg_ckg_fcie, BIT_FCIE_CLK_GATING|BIT_FCIE_CLK_INVERSE|BIT_CLKGEN_FCIE_MASK);
        REG_FCIE_SETBIT(reg_ckg_fcie, BIT_FCIE_CLK_SRC_SEL|u16_ClkParam<<2);
        #endif
		//500msec= 0x10000 x 1/12MHZ x 0x5C
		REG_FCIE_W(FCIE_WR_SBIT_TIMER, BIT_WR_SBIT_TIMER_EN|0x5C);
	    REG_FCIE_W(FCIE_RD_SBIT_TIMER, BIT_RD_SBIT_TIMER_EN|0x5C);
    }

    g_eMMCDrv.u16_ClkRegVal = (U16)u16_ClkParam;

    eMMC_debug(eMMC_DEBUG_LEVEL_LOW, 1, "reg_ckg_fcie=%04X\n", REG_FCIE(reg_ckg_fcie));

    return eMMC_ST_SUCCESS;
}

U32 eMMC_clock_gating(void)
{
//    eMMC_PlatformResetPre();
    REG_FCIE_CLRBIT(FCIE_SD_MODE, BIT_CLK_EN);
//    eMMC_PlatformResetPost();
    return eMMC_ST_SUCCESS;
}

U8 gau8_eMMCPLLSel_52[eMMC_FCIE_VALID_CLK_CNT] =
{
    eMMC_PLL_CLK__52M,
    eMMC_PLL_CLK__32M,
    eMMC_PLL_CLK__27M
};

U8 gau8_eMMCPLLSel_200[eMMC_FCIE_VALID_CLK_CNT] =
{
    eMMC_PLL_CLK_200M,
    eMMC_PLL_CLK_160M,
    eMMC_PLL_CLK_140M
};


U8 gau8_FCIEClkSel[eMMC_FCIE_VALID_CLK_CNT] =
{
    eMMC_PLL_CLK_200M,//BIT_FCIE_CLK_48M,
    eMMC_PLL_CLK_140M,//BIT_FCIE_CLK_36M,
    eMMC_PLL_CLK__80M//BIT_FCIE_CLK_20M,
};

U32 eMMC_FCIE_BuildDDRTimingTable(void)
{
    return eMMC_ST_SUCCESS;
}

#if (defined(ENABLE_eMMC_HS400) && ENABLE_eMMC_HS400) ||\
    (defined(ENABLE_eMMC_HS200) && ENABLE_eMMC_HS200)

U32 eMMC_FCIE_ResetToHS400(U8 u8_ClkParam)
{
    U32 u32_err;

    //eMMC_debug(0, 0, "\033[7;31m%s\033[m\n", __FUNCTION__);

    // --------------------------------
    // reset FCIE & eMMC to normal SDR mode
    if(!eMMC_IF_NORMAL_SDR())
    {
        //eMMC_debug(0,0,"eMMC: re-init to SDR\n");
        g_eMMCDrv.u32_DrvFlag = 0;
        eMMC_PlatformInit();
        u32_err = eMMC_FCIE_Init();
        if(u32_err)
        {
            eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: eMMC_FCIE_Init fail, %Xh\n", u32_err);
            return u32_err;
        }
        u32_err = eMMC_Init_Device_Ex();
        if(eMMC_ST_SUCCESS != u32_err)
        {
            eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,0,"eMMC Err: eMMC_Init_Device_Ex fail: %X\n", u32_err);
            return u32_err;
        }
    }

    // --------------------------------
    // set eMMC to HS200 mode
    #if 0
    u32_err = eMMC_FCIE_EnableFastMode_Ex(FCIE_eMMC_HS200);
    if(eMMC_ST_SUCCESS != u32_err)
    {
        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,0,"eMMC Err: set HS200 IF fail: %X\n", u32_err);

        return u32_err;
    }
    #endif

    u32_err = eMMC_FCIE_EnableFastMode_Ex(FCIE_eMMC_HS400);
    if(eMMC_ST_SUCCESS != u32_err)
    {
        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,0,"eMMC Err: set HS400 IF fail: %X\n", u32_err);

        return u32_err;
    }

    eMMC_clock_setting(u8_ClkParam);

    return u32_err;
}


U32 eMMC_FCIE_ResetToHS200(U8 u8_ClkParam)
{
    U32 u32_err;

    // --------------------------------
    // reset FCIE & eMMC to normal SDR mode
    if(!eMMC_IF_NORMAL_SDR())
    {
        //eMMC_debug(0,1,"eMMC: re-init to SDR\n");
        g_eMMCDrv.u32_DrvFlag = 0;

        eMMC_PlatformInit();
        u32_err = eMMC_FCIE_Init();
        if(u32_err)
        {
            eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: eMMC_FCIE_Init fail, %Xh\n", u32_err);
            return u32_err;
        }
        u32_err = eMMC_Init_Device_Ex();
        if(eMMC_ST_SUCCESS != u32_err)
        {
            eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,0,"eMMC Err: eMMC_Init_Device_Ex fail: %X\n", u32_err);
            return u32_err;
        }
    }

    // --------------------------------
    // set eMMC to HS200 mode
    u32_err = eMMC_FCIE_EnableFastMode_Ex(FCIE_eMMC_HS200);
    if(eMMC_ST_SUCCESS != u32_err)
    {
        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,0,"eMMC Err: set HS200 IF fail: %X\n", u32_err);
        return u32_err;
    }

    eMMC_clock_setting(u8_ClkParam);

    return u32_err;
}

U32 eMMC_FCIE_DetectHS400Timing(void)
{
    return eMMC_ST_SUCCESS;
}

U32 eMMC_FCIE_DetectHS200Timing(void)
{
    return eMMC_ST_SUCCESS;
}

U32 eMMC_FCIE_BuildHS200TimingTable(void)
{
    return eMMC_ST_SUCCESS;
}
#endif

// skew4: 0~17
void eMMC_FCIE_SetSkew4Value(U32 u32Value)
{
    eMMC_debug(eMMC_DEBUG_LEVEL_LOW, 1, "set skew4 value(%d)\n", u32Value);

    if(u32Value>17)
    {
        eMMC_debug(0, 1, "eMMC Err: wrong skew4 value\n");
        return;
    }

    REG_FCIE_CLRBIT(reg_emmcpll_0x03, 0xF000);

    if(u32Value<9) // 0~8
    {
        REG_FCIE_CLRBIT(reg_emmcpll_0x02, BIT14);
        REG_FCIE_CLRBIT(reg_emmcpll_0x6c, BIT7);
        REG_FCIE_SETBIT(reg_emmcpll_0x03, u32Value<<12);
    }
    else // 9~17
    {
        REG_FCIE_SETBIT(reg_emmcpll_0x02, BIT14); // inverse
        REG_FCIE_SETBIT(reg_emmcpll_0x6c, BIT7); // inverse
        REG_FCIE_SETBIT(reg_emmcpll_0x03, (u32Value-9)<<12);
    }

    eMMC_debug(eMMC_DEBUG_LEVEL_LOW, 1, "EMMCPLL[03h]=%04X, [02h]=%04X\n",
        REG_FCIE(reg_emmcpll_0x03), REG_FCIE(reg_emmcpll_0x02));

}

// 0~31
void eMMC_FCIE_SetDelayLatch(U32 u32Value)
{
    eMMC_debug(eMMC_DEBUG_LEVEL_LOW, 1, "SetDelayLatch(%d)\n", u32Value);

    if(u32Value>31)
    {
        eMMC_debug(0, 1, "eMMC Err: wrong delay latch value\n");
        return;
    }

    if(u32Value%2)
        REG_FCIE_SETBIT(reg_emmcpll_0x09, BIT1);
    else
        REG_FCIE_CLRBIT(reg_emmcpll_0x09, BIT1);

    u32Value = (u32Value&0xFFFE)>>1;

    REG_FCIE_CLRBIT(reg_emmcpll_0x09, (0x000F<<4));
    REG_FCIE_SETBIT(reg_emmcpll_0x09, (u32Value<<4));

    eMMC_debug(eMMC_DEBUG_LEVEL_LOW, 1, "EMMCPLL[09]=%04X\n", REG_FCIE(reg_emmcpll_0x09));

}

//---------------------------------------
#if defined(ENABLE_eMMC_INTERRUPT_MODE)&&ENABLE_eMMC_INTERRUPT_MODE

static DECLARE_WAIT_QUEUE_HEAD(fcie_wait);
static volatile U32 fcie_int = 0;
static volatile U32 SAR5_int = 0;

#define eMMC_IRQ_DEBUG  0

irqreturn_t eMMC_FCIE_IRQ(int irq, void *dummy)
{
    volatile u16 u16_Events;

    //printk("fcie IRQ\n");

    REG_FCIE_R(FCIE_PWR_SAVE_CTL, u16_Events);

    if(u16_Events & BIT_POWER_SAVE_MODE_INT)
    {
        REG_FCIE_CLRBIT(FCIE_PWR_SAVE_CTL, BIT_POWER_SAVE_MODE_INT_EN);
        fcie_int = 1;
		SAR5_int = 1;
        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,0, "SAR5 eMMC WARN.\n");
        while(1);
        wake_up(&fcie_wait);

        return IRQ_HANDLED;
    }

    if((REG_FCIE(FCIE_MIE_FUNC_CTL) & BIT_EMMC_ACTIVE) != BIT_EMMC_ACTIVE)
    {
        return IRQ_NONE;
    }

    // one time enable one bit
    u16_Events = REG_FCIE(FCIE_MIE_EVENT) & REG_FCIE(FCIE_MIE_INT_EN);

    if(u16_Events & (BIT_DMA_END|BIT_ERR_STS))
    {
        REG_FCIE_CLRBIT(FCIE_MIE_INT_EN, (BIT_DMA_END|BIT_ERR_STS));

        fcie_int = 1;
        wake_up(&fcie_wait);

        return IRQ_HANDLED;
    }
    else if(u16_Events & BIT_CMD_END)
    {
        REG_FCIE_CLRBIT(FCIE_MIE_INT_EN, BIT_CMD_END);

        fcie_int = 1;
        wake_up(&fcie_wait);

        return IRQ_HANDLED;
    }
    #if defined(ENABLE_FCIE_HW_BUSY_CHECK)&&ENABLE_FCIE_HW_BUSY_CHECK
    else if(u16_Events & BIT_BUSY_END_INT)
    {
        REG_FCIE_CLRBIT(FCIE_MIE_INT_EN, BIT_BUSY_END_INT);
        REG_FCIE_CLRBIT(FCIE_SD_CTRL, BIT_BUSY_DET_ON);

        fcie_int = 1;
        wake_up(&fcie_wait);

        return IRQ_HANDLED;
    }
    #endif

    #if eMMC_IRQ_DEBUG
    if(0==fcie_int)
        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Warn: Int St:%Xh, En:%Xh, Evt:%Xh \n",
        REG_FCIE(FCIE_MIE_EVENT), REG_FCIE(FCIE_MIE_INT_EN), u16_Events);
    #endif

    return IRQ_NONE;
}

U32 eMMC_WaitCompleteIntr(uintptr_t ulongRegAddr, U16 u16_WaitEvent, U32 u32_MicroSec)
{
    U32 u32_i=0;

    #if eMMC_IRQ_DEBUG
    U32 u32_isr_tmp[2];
    unsigned long long u64_jiffies_tmp, u64_jiffies_now;
    struct timeval time_st;
    time_t sec_tmp;
    suseconds_t us_tmp;

    u32_isr_tmp[0] = fcie_int;
    do_gettimeofday(&time_st);
    sec_tmp = time_st.tv_sec;
    us_tmp = time_st.tv_usec;
    u64_jiffies_tmp = jiffies_64;
    #endif

    if(wait_event_timeout(fcie_wait, (fcie_int == 1), usecs_to_jiffies(u32_MicroSec)) == 0)
    {
		if(SAR5_int == 1)
		{
	        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,0, "SAR5 eMMC trap.\n");
			while(1);
		}
        #if eMMC_IRQ_DEBUG
        u32_isr_tmp[1] = fcie_int;
        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,
            "eMMC Warn: int timeout, WaitEvt:%Xh, NowEvt:%Xh, IntEn:%Xh, ISR:%u->%u->%u \n",
            u16_WaitEvent, REG_FCIE(FCIE_MIE_EVENT), REG_FCIE(FCIE_MIE_INT_EN),
            u32_isr_tmp[0], u32_isr_tmp[1], fcie_int);

        do_gettimeofday(&time_st);

        u64_jiffies_now = jiffies_64;
        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,
            " PassTime: %lu s, %lu us, %llu jiffies.  WaitTime: %u us, %lu jiffies, HZ:%u.\n",
            time_st.tv_sec-sec_tmp, time_st.tv_usec-us_tmp, u64_jiffies_now-u64_jiffies_tmp,
            u32_MicroSec, usecs_to_jiffies(u32_MicroSec), HZ);
        #else
        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,
            "eMMC Warn: int timeout, WaitEvt:%Xh, NowEvt:%Xh, IntEn:%Xh \n",
            u16_WaitEvent, REG_FCIE(FCIE_MIE_EVENT), REG_FCIE(FCIE_MIE_INT_EN));
        #endif

        // switch to polling
        for(u32_i=0; u32_i<u32_MicroSec; u32_i++)
        {
            //if((REG_FCIE(ulongRegAddr) & u16_WaitEvent) == u16_WaitEvent )
                //break;
            if(REG_FCIE(ulongRegAddr) & u16_WaitEvent)
                break;
            eMMC_hw_timer_delay(HW_TIMER_DELAY_1us);
        }

        if(u32_i == u32_MicroSec)
        {
            eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: events lose, WaitEvent: %Xh \n", u16_WaitEvent);
            eMMC_DumpDriverStatus();  eMMC_DumpPadClk();
            eMMC_FCIE_DumpRegisters();eMMC_FCIE_DumpDebugBus();
            REG_FCIE_CLRBIT(FCIE_MIE_INT_EN, u16_WaitEvent);
            return eMMC_ST_ERR_TIMEOUT_WAIT_REG0;
        }
        else
        {
            REG_FCIE_CLRBIT(FCIE_MIE_INT_EN, u16_WaitEvent);
            eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Warn: but polling ok: %Xh \n", REG_FCIE(ulongRegAddr));
        }
    }

    //----------------------------------------
    fcie_int = 0;

    return eMMC_ST_SUCCESS;

}
#endif


int mstar_mci_Housekeep(void *pData)
{
    #if !(defined(eMMC_HOUSEKEEP_THREAD) && eMMC_HOUSEKEEP_THREAD)
    return 0;
    #endif

    #if eMMC_PROFILE_WR
    g_eMMCDrv.u32_CNT_MinRBlk = 0xFFFFFFFF;
    g_eMMCDrv.u32_CNT_MinWBlk = 0xFFFFFFFF;
    #endif

    while(1)
    {
        #if defined(eMMC_PROFILE_WR) && eMMC_PROFILE_WR
        msleep(1000);
        eMMC_dump_WR_Count();
        #endif

        if(kthread_should_stop())
            break;
    }

    return 0;
}


int mstar_mci_exit_checkdone_Ex(void)
{
    U16 u16Reg = 0;

    u16Reg = REG_FCIE(FCIE_SD_STATUS);

    if(u16Reg & BIT_SD_D0)
        return 1;
    else
        return 0;
}

bool mstar_mci_exit_checkdone_ForCI(void)
{
    return true;
}

EXPORT_SYMBOL(mstar_mci_exit_checkdone_ForCI);

#if (defined(IF_FCIE_SHARE_IP) && IF_FCIE_SHARE_IP)
bool mstar_mci_exit_checkdone_ForSD(void)
{
    U32 u32_Cnt=0;

    // nike SD_mode_x does not share pins
    //if(BIT_eMMC_CFG_MODE2 == (REG_FCIE(reg_chiptop_0x5D)&BIT_eMMC_CFG_MASK))
    //  return TRUE;

    REG_FCIE_SETBIT(FCIE_SD_MODE, BIT_SD_CLK_EN); // output clock
    while(mstar_mci_exit_checkdone_Ex() == 0) /* card busy */
    {
        if ((++u32_Cnt) >= HW_TIMER_DELAY_1s*10)
        {
            REG_FCIE_CLRBIT(FCIE_SD_MODE, BIT_SD_CLK_EN); // not output clock
            eMMC_debug(0,1,"eMMC Info: SD check -> D0 busy\n");
            return false;
        }

        eMMC_hw_timer_delay(HW_TIMER_DELAY_1us);

        if(0 == (u32_Cnt%HW_TIMER_DELAY_100us))
        {
            msleep(1);
            u32_Cnt += HW_TIMER_DELAY_1ms;
        }
    }

    REG_FCIE_CLRBIT(FCIE_SD_MODE, BIT_SD_CLK_EN); // not output clock

    return true;
}
EXPORT_SYMBOL(mstar_mci_exit_checkdone_ForSD);
#endif

#if 0
U32 mstar_SD_CardChange(void)
{
    return 0;
}
#endif

//---------------------------------------

#if (defined(IF_FCIE_SHARE_PINS) && IF_FCIE_SHARE_PINS) || \
    (defined(IF_FCIE_SHARE_IP) && IF_FCIE_SHARE_IP)
extern struct semaphore PfModeSem;
#endif

extern bool ms_sdmmc_wait_d0_for_emmc(void);
void eMMC_LockFCIE(U8 *pu8_str)
{
    //eMMC_debug(0,1,"%s 1\n", pu8_str);
    #if (defined(IF_FCIE_SHARE_PINS) && IF_FCIE_SHARE_PINS) || \
        (defined(IF_FCIE_SHARE_IP) && IF_FCIE_SHARE_IP)
    down(&PfModeSem);

    #if (defined(IF_FCIE_SHARE_PINS) && IF_FCIE_SHARE_PINS) && \
        defined(CONFIG_MSTAR_SDMMC)
    if(false == ms_sdmmc_wait_d0_for_emmc())
    {
        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: SD keep D0 low \n");
        eMMC_FCIE_ErrHandler_Stop();
    }
    #endif

    REG_FCIE_CLRBIT(FCIE_TEST_MODE, BIT_DEBUG_MODE_MASK);
    REG_FCIE_SETBIT(FCIE_TEST_MODE, 2<<BIT_DEBUG_MODE_SHIFT); // 2: card_data1_dbus = {xxxxx, Wrstate, RDstate}

    REG_FCIE_CLRBIT(FCIE_EMMC_DEBUG_BUS1, BIT11|BIT10|BIT9|BIT8);
    REG_FCIE_SETBIT(FCIE_EMMC_DEBUG_BUS1, 5<<8); // 5: card


    if(REG_FCIE(FCIE_EMMC_DEBUG_BUS0)&0x0FFF) //Check FICE5 StateMachine
        eMMC_FCIE_Init();

    eMMC_pads_switch(g_eMMCDrv.u8_PadType);
    eMMC_clock_setting(g_eMMCDrv.u16_ClkRegVal);
    #endif

    REG_FCIE_SETBIT(FCIE_SD_MODE, BIT_SD_CLK_EN); // output clock

    REG_FCIE_SETBIT(FCIE_MIE_FUNC_CTL, BIT_EMMC_ACTIVE);

    //eMMC_debug(0,1,"%s 2\n", pu8_str);
}

void eMMC_UnlockFCIE(U8 *pu8_str)
{
    //eMMC_debug(0,1,"%s 1\n", pu8_str);
    REG_FCIE_CLRBIT(FCIE_MIE_FUNC_CTL, BIT_EMMC_ACTIVE);
    REG_FCIE_CLRBIT(FCIE_SD_MODE, BIT_SD_CLK_EN); // not output clock

    #if (defined(IF_FCIE_SHARE_PINS) && IF_FCIE_SHARE_PINS) || \
        (defined(IF_FCIE_SHARE_IP) && IF_FCIE_SHARE_IP)
    REG_FCIE_CLRBIT(reg_chiptop_0x08, BIT_EMMC_CONFIG);
    up(&PfModeSem);
    #endif

    //eMMC_debug(0,1,"%s 2\n", pu8_str);
}

//---------------------------------------

U32 eMMC_PlatformResetPre(void)
{
    //add pll setting which is overwritten by others
    REG_FCIE_CLRBIT(reg_chiptop_0x08, BIT_EMMC_CONFIG);
    return eMMC_ST_SUCCESS;
}

U32 eMMC_PlatformResetPost(void)
{
    REG_FCIE_SETBIT(reg_chiptop_0x08, BIT_EMMC_CONFIG);

    #if defined(ENABLE_EMMC_POWER_SAVING_MODE) && ENABLE_EMMC_POWER_SAVING_MODE
    eMMC_Prepare_Power_Saving_Mode_Queue();
    #endif

    return eMMC_ST_SUCCESS;
}


U32 eMMC_PlatformInit(void)
{
    #if 1
    // for eMMC 4.5 HS200 need 1.8V, unify all eMMC IO power to 1.8V
    // works both for eMMC 4.4 & 4.5
    // eMMC_debug(eMMC_DEBUG_LEVEL_LOW, 1, "1.8V IO power for eMMC\n");
    // Irwin Tyan: set this bit to boost IO performance at low power supply.

    if((REG_FCIE(reg_emmc_test) & BIT0) != 1)
    {
        //eMMC_debug(0, 0, "eMMC: set 1.8V \n");
        REG_FCIE_SETBIT(reg_emmc_test, BIT0); // 1.8V must set this bit
        REG_FCIE_SETBIT(reg_emmc_test, BIT2); // atop patch
        REG_FCIE_CLRBIT(reg_nand_pad_driving, 0xffff);// 1.8V must set this bit
        REG_FCIE_SETBIT(reg_nand_pad_driving, 0xf);// 1.8V must set this bit
    }
    #else
    eMMC_debug(eMMC_DEBUG_LEVEL_LOW, 1, "3.3V IO power for eMMC\n");
    pRegEmmcPll->reg_emmc_test = 0; // 3.3V must clear this bit
    #endif

    eMMC_pads_switch(FCIE_eMMC_BYPASS);

    #if defined(CONFIG_MSTAR_CLKM)
    gs32_eMMCCLKPHandle = get_handle("g_clk_nfie_p");
    gs32_eMMCCLKHandle = get_handle("g_clk_nfie");
    #endif

    eMMC_clock_setting(FCIE_SLOWEST_CLK);

    return eMMC_ST_SUCCESS;
}

U32 eMMC_PlatformDeinit(void)
{
    return eMMC_ST_SUCCESS;
}

// --------------------------------------------
static U32 sgu32_MemGuard0 = 0xA55A;
eMMC_ALIGN0 eMMC_DRIVER g_eMMCDrv eMMC_ALIGN1;
static U32 sgu32_MemGuard1 = 0x1289;

eMMC_ALIGN0 U8 gau8_eMMC_SectorBuf[eMMC_SECTOR_BUF_16KB] eMMC_ALIGN1; // 512 bytes
eMMC_ALIGN0 U8 gau8_eMMC_PartInfoBuf[eMMC_SECTOR_512BYTE] eMMC_ALIGN1; // 512 bytes

U32 eMMC_CheckIfMemCorrupt(void)
{
    if(0xA55A != sgu32_MemGuard0 || 0x1289 != sgu32_MemGuard1)
        return eMMC_ST_ERR_MEM_CORRUPT;
    return eMMC_ST_SUCCESS;
}
// <-- [FIXME]

#endif
