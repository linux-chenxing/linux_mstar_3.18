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

#if defined(IF_FCIE_SHARE_IP) && IF_FCIE_SHARE_IP
// common for TV linux platform
extern bool ms_sdmmc_card_chg(unsigned int slotNo);
#endif

extern struct platform_device sg_mstar_emmc_device_st;

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
        msleep(1000);
        u32_i -= 1000;
    }

    msleep(u32_i);

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

U32 eMMC_translate_DMA_address_Ex(U32 u32_DMAAddr, U32 u32_ByteCnt)
{
    REG_FCIE_CLRBIT(FCIE_MMA_PRI_REG, BIT_MIU_SELECT_MASK);

    #ifdef MSTAR_MIU2_BUS_BASE
    if( u32_DMAAddr >= MSTAR_MIU2_BUS_BASE) // MIU2
    {
        REG_FCIE_SETBIT(FCIE_MMA_PRI_REG, BIT_MIU2_SELECT);
        u32_DMAAddr -= MSTAR_MIU2_BUS_BASE;
    }
    else
    #endif
    if( u32_DMAAddr >= MSTAR_MIU1_BUS_BASE) // MIU1
    {
        REG_FCIE_SETBIT(FCIE_MMA_PRI_REG, BIT_MIU1_SELECT);
        u32_DMAAddr -= MSTAR_MIU1_BUS_BASE;
    }
    else // MIU0
    {
        u32_DMAAddr -= MSTAR_MIU0_BUS_BASE;
    }

    return u32_DMAAddr;
}

U32 eMMC_DMA_MAP_address(U32 u32_Buffer, U32 u32_ByteCnt, int mode)
{
    dma_addr_t dma_addr;

    #if defined(CONFIG_ENABLE_EMMC_ACP) && CONFIG_ENABLE_EMMC_ACP

    dma_addr = dma_map_single(&sg_mstar_emmc_device_st.dev, (void*)u32_Buffer, u32_ByteCnt, DMA_ACP);

    #else

    if(mode == 0)   //write
    {
        dma_addr = dma_map_single(&sg_mstar_emmc_device_st.dev, (void*)u32_Buffer, u32_ByteCnt, DMA_TO_DEVICE);
    }
    else
    {
        dma_addr = dma_map_single(&sg_mstar_emmc_device_st.dev, (void*)u32_Buffer, u32_ByteCnt, DMA_FROM_DEVICE);
    }

    if( dma_mapping_error(&sg_mstar_emmc_device_st.dev, dma_addr) )
    {
        dma_unmap_single(&sg_mstar_emmc_device_st.dev, dma_addr, u32_ByteCnt, (mode) ? DMA_FROM_DEVICE : DMA_TO_DEVICE);
        eMMC_die("eMMC_DMA_MAP_address: Kernel can't mapping dma correctly\n");
    }

    #endif

    return dma_addr;
}

void eMMC_DMA_UNMAP_address(U32 u32_DMAAddr, U32 u32_ByteCnt, int mode)
{
    #if defined(CONFIG_ENABLE_EMMC_ACP) && CONFIG_ENABLE_EMMC_ACP

    dma_unmap_single(&sg_mstar_emmc_device_st.dev, u32_DMAAddr, u32_ByteCnt, DMA_ACP);

    #else

    if(mode == 0)	//write
    {
        dma_unmap_single(&sg_mstar_emmc_device_st.dev, u32_DMAAddr, u32_ByteCnt, DMA_TO_DEVICE);
    }
    else
    {
        dma_unmap_single(&sg_mstar_emmc_device_st.dev, u32_DMAAddr, u32_ByteCnt, DMA_FROM_DEVICE);
    }

    #endif
}

void eMMC_flush_data_cache_buffer(U32 u32_DMAAddr, U32 u32_ByteCnt)
{
    Chip_Clean_Cache_Range_VA_PA(u32_DMAAddr,__pa(u32_DMAAddr), u32_ByteCnt);
}

void eMMC_Invalidate_data_cache_buffer(U32 u32_DMAAddr, U32 u32_ByteCnt)
{
    Chip_Inv_Cache_Range_VA_PA(u32_DMAAddr,__pa(u32_DMAAddr), u32_ByteCnt);
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
    REG_FCIE_SETBIT(reg_pwrgd_int_glirm, (0x1<<10));

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

    REG_FCIE_SETBIT(FCIE_PWR_SAVE_CTL, BIT_POWER_SAVE_MODE);
}

#endif

// [FIXME] -->
void eMMC_DumpPadClk(void)
{
    eMMC_debug(0, 0, "\nclk setting:\n");
    eMMC_debug(0, 0, "reg_ckg_fcie(0x%X):0x%x\n", reg_ckg_fcie ,REG_FCIE_U16(reg_ckg_fcie));
    eMMC_debug(0, 0, "FCIE Clk: %uKHz\n", g_eMMCDrv.u32_ClkKHz);
    eMMC_debug(0, 0, "Reg Val: %Xh\n", g_eMMCDrv.u16_ClkRegVal);

    //---------------------------------------------------------------------
    eMMC_debug(0, 0, "[pad setting]:\n");
    switch(g_eMMCDrv.u8_PadType)
    {
        case FCIE_eMMC_BYPASS:          eMMC_debug(0, 0, "Bypass\n");           break;
        case FCIE_eMMC_SDR:             eMMC_debug(0, 0, "SDR 8-bit macro\n");  break;
        case FCIE_eMMC_DDR_8BIT_MACRO:  eMMC_debug(0, 0, "DDR 8-bit macro\n");  break;
        case FCIE_eMMC_DDR:             eMMC_debug(0, 0, "DDR 32-bit macro\n"); break;
        case FCIE_eMMC_HS200:           eMMC_debug(0, 0, "HS200\n");            break;
        case FCIE_eMMC_HS400_DS:        eMMC_debug(0, 0, "HS400 DS\n");         break;
        case FCIE_eMMC_HS400_SKEW4:     eMMC_debug(0, 0, "HS400 Skew4\n");      break;
        default:
            eMMC_debug(0, 0, "eMMC Err: Pad unknown, %d\n", g_eMMCDrv.u8_PadType); eMMC_die("\n");
            break;
    }
}

U32 eMMC_pads_switch(U32 u32Mode)
{
    g_eMMCDrv.u8_PadType = u32Mode;

    // common part:
    // chiptop
    REG_FCIE_CLRBIT(reg_chiptop_0x5A, BIT_SD_CONFIG);
    REG_FCIE_CLRBIT(reg_chiptop_0x6F, BIT_NAND_MODE);
    REG_FCIE_CLRBIT(reg_chiptop_0x6E, BIT_EMMC_CONFIG_MSK);
    REG_FCIE_SETBIT(reg_chiptop_0x6E, BIT_EMMC_CONFIG_EMMC_MODE_1);
    REG_FCIE_CLRBIT(reg_chiptop_0x50,BIT_ALL_PAD_IN);

    // fcie
    REG_FCIE_CLRBIT(FCIE_DDR_MODE, BIT_FALL_LATCH|BIT_PAD_IN_SEL_SD|BIT_32BIT_MACRO_EN|BIT_DDR_EN|BIT_8BIT_MACRO_EN);

    // emmc_pll
    REG_FCIE_CLRBIT(reg_emmcpll_0x02, BIT13);
    REG_FCIE_CLRBIT(reg_emmcpll_0x09, BIT0);
    REG_FCIE_CLRBIT(reg_emmcpll_0x20, BIT9);
    REG_FCIE_CLRBIT(reg_emmcpll_0x1d, BIT8);
    REG_FCIE_CLRBIT(reg_emmcpll_0x63, BIT0);
    REG_FCIE_CLRBIT(reg_emmcpll_0x68, BIT0|BIT1);
    REG_FCIE_CLRBIT(reg_emmcpll_0x6a, BIT0|BIT1);
    REG_FCIE_W(reg_emmcpll_0x6b, 0x0000);
    REG_FCIE_CLRBIT(reg_emmcpll_0x6d, BIT0);
    REG_FCIE_CLRBIT(reg_emmcpll_0x70, BIT8);
    REG_FCIE_W(reg_emmcpll_0x71, 0xFFFF);
    REG_FCIE_W(reg_emmcpll_0x73, 0xFFFF);
    REG_FCIE_SETBIT(reg_emmcpll_0x74, BIT15);

    switch(u32Mode)
    {
        case FCIE_eMMC_BYPASS:
            eMMC_debug(eMMC_DEBUG_LEVEL_HIGH, 1, "Bypass\n");

            REG_FCIE_SETBIT(reg_emmcpll_0x6f, BIT0|BIT1);
			break;

        case FCIE_eMMC_SDR:
            eMMC_debug(eMMC_DEBUG_LEVEL_HIGH, 1, "SDR 8-bit macro\n");

            // fcie
            REG_FCIE_SETBIT(FCIE_DDR_MODE, BIT_8BIT_MACRO_EN);

            // emmc_pll
            REG_FCIE_SETBIT(reg_emmcpll_0x68, BIT0);

            if(g_eMMCDrv.u8_BUS_WIDTH == BIT_SD_DATA_WIDTH_1)
            {
                REG_FCIE_W(reg_emmcpll_0x6b, 0x0813);
            }
            else if(g_eMMCDrv.u8_BUS_WIDTH == BIT_SD_DATA_WIDTH_4)
            {
                REG_FCIE_SETBIT(reg_emmcpll_0x6a, BIT0);
                REG_FCIE_W(reg_emmcpll_0x6b, 0x0413);
            }
            else if(g_eMMCDrv.u8_BUS_WIDTH == BIT_SD_DATA_WIDTH_8)
            {
                REG_FCIE_SETBIT(reg_emmcpll_0x6a, BIT1);
                REG_FCIE_W(reg_emmcpll_0x6b, 0x0213);
            }

            REG_FCIE_SETBIT(reg_emmcpll_0x6f, BIT0|BIT1);
            break;

        case FCIE_eMMC_DDR_8BIT_MACRO:
            eMMC_debug(eMMC_DEBUG_LEVEL_HIGH, 1, "DDR 8-bit macro\n");

            // fcie
            REG_FCIE_SETBIT(FCIE_DDR_MODE, BIT_DDR_EN|BIT_8BIT_MACRO_EN);

            // emmc_pll
            REG_FCIE_SETBIT(reg_emmcpll_0x68, BIT0);

            if(g_eMMCDrv.u8_BUS_WIDTH == BIT_SD_DATA_WIDTH_4)
            {
                REG_FCIE_SETBIT(reg_emmcpll_0x6a, BIT0);
                REG_FCIE_W(reg_emmcpll_0x6b, 0x0213);
            }
            else if(g_eMMCDrv.u8_BUS_WIDTH == BIT_SD_DATA_WIDTH_8)
            {
                REG_FCIE_SETBIT(reg_emmcpll_0x6a, BIT1);
                REG_FCIE_W(reg_emmcpll_0x6b, 0x0113);
            }
            else
                goto ErrorHandle;

            REG_FCIE_SETBIT(reg_emmcpll_0x6d, BIT0);
            /* reset 8bit macro start*/
            REG_FCIE_CLRBIT(reg_emmcpll_0x6f, BIT1);
            REG_FCIE_SETBIT(reg_emmcpll_0x6f, BIT1);
            /* reset 8bit macro end */
            break;

        case FCIE_eMMC_DDR:
            eMMC_debug(eMMC_DEBUG_LEVEL_HIGH, 1, "DDR 32-bit macro\n");

            // fcie
            REG_FCIE_SETBIT(FCIE_DDR_MODE, BIT_32BIT_MACRO_EN|BIT_DDR_EN);

            // emmc_pll
            REG_FCIE_CLRBIT(reg_emmcpll_0x03, 0x0FFF);
            REG_FCIE_SETBIT(reg_emmcpll_0x03, 0x0004);

            REG_FCIE_CLRBIT(reg_emmcpll_0x1d, BIT3|BIT2|BIT1);
            REG_FCIE_SETBIT(reg_emmcpll_0x1d, BIT2);

            REG_FCIE_CLRBIT(reg_emmcpll_0x1d, BIT6|BIT5|BIT4);
            REG_FCIE_SETBIT(reg_emmcpll_0x1d, BIT5);

            REG_FCIE_SETBIT(reg_emmcpll_0x68, BIT0|BIT1);

            REG_FCIE_CLRBIT(reg_emmcpll_0x69, 0xF<<4);
            REG_FCIE_SETBIT(reg_emmcpll_0x69, 4<<4);
            REG_FCIE_SETBIT(reg_emmcpll_0x69, BIT3);

            if(g_eMMCDrv.u8_BUS_WIDTH == BIT_SD_DATA_WIDTH_4)
            {
                REG_FCIE_SETBIT(reg_emmcpll_0x6a, BIT0);
                REG_FCIE_W(reg_emmcpll_0x6b, 0x0213);
            }
            else if(g_eMMCDrv.u8_BUS_WIDTH == BIT_SD_DATA_WIDTH_8)
            {
                REG_FCIE_SETBIT(reg_emmcpll_0x6a, BIT1);
                REG_FCIE_W(reg_emmcpll_0x6b, 0x0113);
            }
            else
                goto ErrorHandle;

            /* reset 32bit macro start*/
            REG_FCIE_CLRBIT(reg_emmcpll_0x6f, BIT0);
            REG_FCIE_SETBIT(reg_emmcpll_0x6f, BIT0);
            /* reset 32bit macro end */

            REG_FCIE_SETBIT(reg_emmcpll_0x6f, BIT1);

            REG_FCIE_SETBIT(reg_emmcpll_0x70, BIT8);
            REG_FCIE_W(reg_emmcpll_0x71, 0x0000);
            break;

        case FCIE_eMMC_HS200:
            eMMC_debug(eMMC_DEBUG_LEVEL_HIGH, 1, "HS200\n");

            // fcie
            REG_FCIE_SETBIT(FCIE_DDR_MODE, BIT_32BIT_MACRO_EN);

            // emmc_pll
            REG_FCIE_CLRBIT(reg_emmcpll_0x03, 0x0FFF);
            REG_FCIE_SETBIT(reg_emmcpll_0x03, 0x0008);

            REG_FCIE_CLRBIT(reg_emmcpll_0x1d, BIT6|BIT5|BIT4);
            REG_FCIE_SETBIT(reg_emmcpll_0x1d, BIT5);

            REG_FCIE_SETBIT(reg_emmcpll_0x68, BIT0);

            REG_FCIE_CLRBIT(reg_emmcpll_0x69, 0xF<<4);
            REG_FCIE_SETBIT(reg_emmcpll_0x69, 4<<4);

            REG_FCIE_SETBIT(reg_emmcpll_0x69, BIT3);

            if(g_eMMCDrv.u8_BUS_WIDTH == BIT_SD_DATA_WIDTH_4)
            {
                REG_FCIE_SETBIT(reg_emmcpll_0x6a, BIT0);
                REG_FCIE_W(reg_emmcpll_0x6b, 0x0413);
            }
            else if(g_eMMCDrv.u8_BUS_WIDTH == BIT_SD_DATA_WIDTH_8)
            {
                REG_FCIE_SETBIT(reg_emmcpll_0x6a, BIT1);
                REG_FCIE_W(reg_emmcpll_0x6b, 0x0213);
            }
            else
                goto ErrorHandle;

            /* reset 32bit macro start*/
            REG_FCIE_CLRBIT(reg_emmcpll_0x6f, BIT0);
            REG_FCIE_SETBIT(reg_emmcpll_0x6f, BIT0);
            /* reset 32bit macro end */
            REG_FCIE_SETBIT(reg_emmcpll_0x6f, BIT1);

            REG_FCIE_SETBIT(reg_emmcpll_0x70, BIT8);
            REG_FCIE_W(reg_emmcpll_0x71, 0x0000);
            break;

        case FCIE_eMMC_HS400_DS:
            eMMC_debug(eMMC_DEBUG_LEVEL_HIGH, 1, "HS400 DS\n");

            // fcie
            REG_FCIE_SETBIT(FCIE_DDR_MODE, BIT_32BIT_MACRO_EN|BIT_DDR_EN);

            // emmc_pll
            REG_FCIE_CLRBIT(reg_emmcpll_0x03, 0x0FFF);
            REG_FCIE_SETBIT(reg_emmcpll_0x03, 0x0004);

            REG_FCIE_SETBIT(reg_emmcpll_0x09, BIT0);

            REG_FCIE_CLRBIT(reg_emmcpll_0x1d, BIT6|BIT5|BIT4);
            REG_FCIE_SETBIT(reg_emmcpll_0x1d, BIT5);

            REG_FCIE_SETBIT(reg_emmcpll_0x63, BIT0);

            REG_FCIE_SETBIT(reg_emmcpll_0x68, BIT0|BIT1);

            REG_FCIE_CLRBIT(reg_emmcpll_0x69, 0xF<<4);
            REG_FCIE_SETBIT(reg_emmcpll_0x69, 6<<4);
            REG_FCIE_SETBIT(reg_emmcpll_0x69, BIT3);

            if(g_eMMCDrv.u8_BUS_WIDTH == BIT_SD_DATA_WIDTH_8)
            {
                REG_FCIE_SETBIT(reg_emmcpll_0x6a, BIT1);
                REG_FCIE_W(reg_emmcpll_0x6b, 0x0113);
            }
            else
                goto ErrorHandle;

            REG_FCIE_SETBIT(reg_emmcpll_0x70, BIT8);
            REG_FCIE_W(reg_emmcpll_0x71, 0x0000);
            REG_FCIE_W(reg_emmcpll_0x73, 0xFF00);
            REG_FCIE_CLRBIT(reg_emmcpll_0x74, BIT15);

            ///////////////////////////////////////////////////////////////////
            // Use delay latch to make HS400 work patch
            REG_FCIE_W(reg_emmcpll_0x73, 0xFFFF);
            REG_FCIE_W(reg_emmcpll_0x74, 0x80FF);
            eMMC_FCIE_SetDelayLatch(2);
            ///////////////////////////////////////////////////////////////////
            break;

        case FCIE_eMMC_HS400_SKEW4:
            eMMC_debug(eMMC_DEBUG_LEVEL_HIGH, 1, "HS400 SKEW4\n");

            // fcie
            REG_FCIE_SETBIT(FCIE_DDR_MODE, BIT_32BIT_MACRO_EN|BIT_DDR_EN);

            // emmc_pll
            REG_FCIE_CLRBIT(reg_emmcpll_0x03, 0x0FFF);
            REG_FCIE_SETBIT(reg_emmcpll_0x03, 0x0004);

            REG_FCIE_CLRBIT(reg_emmcpll_0x1d, BIT6|BIT5|BIT4);
            REG_FCIE_SETBIT(reg_emmcpll_0x1d, BIT5);

            REG_FCIE_SETBIT(reg_emmcpll_0x09, BIT0);
            REG_FCIE_SETBIT(reg_emmcpll_0x20, BIT9);
            REG_FCIE_SETBIT(reg_emmcpll_0x1d, BIT8);
            REG_FCIE_SETBIT(reg_emmcpll_0x63, BIT0);
            REG_FCIE_SETBIT(reg_emmcpll_0x68, BIT0|BIT1);

            REG_FCIE_CLRBIT(reg_emmcpll_0x69, 0xF<<4);
            REG_FCIE_SETBIT(reg_emmcpll_0x69, 6<<4);
            REG_FCIE_SETBIT(reg_emmcpll_0x69, BIT3);

            if(g_eMMCDrv.u8_BUS_WIDTH == BIT_SD_DATA_WIDTH_8)
            {
                REG_FCIE_SETBIT(reg_emmcpll_0x6a, BIT1);
                REG_FCIE_W(reg_emmcpll_0x6b, 0x0113);
            }
            else
                goto ErrorHandle;

            REG_FCIE_SETBIT(reg_emmcpll_0x70, BIT8);
            REG_FCIE_W(reg_emmcpll_0x71, 0x0000);
            REG_FCIE_W(reg_emmcpll_0x73, 0xFF00);
            REG_FCIE_CLRBIT(reg_emmcpll_0x74, BIT15);
            break;

        default:
            eMMC_debug(1, 1, "eMMC Err: wrong parameter for switch pad func\n");
            return eMMC_ST_ERR_PARAMETER;
	}

    // 8 bits macro reset + 32 bits macro reset
    REG_FCIE_CLRBIT(reg_emmcpll_0x6f, BIT0);
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
    else if( g_eMMCDrv.u8_PadType == FCIE_eMMC_HS400_DS)
    {
        g_eMMCDrv.u32_DrvFlag |= DRV_FLAG_SPEED_HS400;
    }
    else if( g_eMMCDrv.u8_PadType == FCIE_eMMC_HS400_SKEW4)
    {
        g_eMMCDrv.u32_DrvFlag |= DRV_FLAG_SPEED_HS400;
    }

    return eMMC_ST_SUCCESS;

    ErrorHandle:

    eMMC_debug(1, 1, "eMMC Err: set bus width before pad switch\n");

    return eMMC_ST_ERR_INVALID_PARAM;
}

U32 eMMC_pll_setting(U16 u16_ClkParam)
{
    static U16 u16_OldClkParam=0xFFFF;
    U32 u32_value_reg_emmc_pll_pdiv;

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

void eMMC_pll_dll_setting(void)
{
    volatile U16 u16_reg;

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

    // Wait 100us
    udelay(1000);

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

    eMMC_PlatformResetPre();

    REG_FCIE_CLRBIT(FCIE_SD_MODE, BIT_SD_CLK_EN);

    switch(u16_ClkParam)
    {
        // emmc_pll clock
        case eMMC_PLL_CLK__20M	: g_eMMCDrv.u32_ClkKHz =  20000; break;
        case eMMC_PLL_CLK__27M	: g_eMMCDrv.u32_ClkKHz =  27000; break;
        case eMMC_PLL_CLK__32M	: g_eMMCDrv.u32_ClkKHz =  32000; break;
        case eMMC_PLL_CLK__36M	: g_eMMCDrv.u32_ClkKHz =  36000; break;
        case eMMC_PLL_CLK__40M	: g_eMMCDrv.u32_ClkKHz =  40000; break;
        case eMMC_PLL_CLK__48M	: g_eMMCDrv.u32_ClkKHz =  48000; break;
        case eMMC_PLL_CLK__52M	: g_eMMCDrv.u32_ClkKHz =  52000; break;
        case eMMC_PLL_CLK__62M	: g_eMMCDrv.u32_ClkKHz =  62000; break;
        case eMMC_PLL_CLK__72M	: g_eMMCDrv.u32_ClkKHz =  72000; break;
        case eMMC_PLL_CLK__80M	: g_eMMCDrv.u32_ClkKHz =  80000; break;
        case eMMC_PLL_CLK__86M	: g_eMMCDrv.u32_ClkKHz =  86000; break;
        case eMMC_PLL_CLK_100M	: g_eMMCDrv.u32_ClkKHz = 100000; break;
        case eMMC_PLL_CLK_120M	: g_eMMCDrv.u32_ClkKHz = 120000; break;
        case eMMC_PLL_CLK_140M	: g_eMMCDrv.u32_ClkKHz = 140000; break;
        case eMMC_PLL_CLK_160M	: g_eMMCDrv.u32_ClkKHz = 160000; break;
        case eMMC_PLL_CLK_200M	: g_eMMCDrv.u32_ClkKHz = 200000; break;

        // clock_gen fcie clock
        case BIT_FCIE_CLK_12M	: g_eMMCDrv.u32_ClkKHz =  12000; break;
        case BIT_FCIE_CLK_20M	: g_eMMCDrv.u32_ClkKHz =  20000; break;
        case BIT_FCIE_CLK_32M	: g_eMMCDrv.u32_ClkKHz =  32000; break;
        case BIT_FCIE_CLK_36M	: g_eMMCDrv.u32_ClkKHz =  36000; break;
        case BIT_FCIE_CLK_40M	: g_eMMCDrv.u32_ClkKHz =  40000; break;
        case BIT_FCIE_CLK_43_2M	: g_eMMCDrv.u32_ClkKHz =  43200; break;
        case BIT_FCIE_CLK_300K	: g_eMMCDrv.u32_ClkKHz =    300; break;
        case BIT_FCIE_CLK_24M	: g_eMMCDrv.u32_ClkKHz =  24000; break;
        case BIT_FCIE_CLK_48M	: g_eMMCDrv.u32_ClkKHz =  48000; break;

        default:
            eMMC_debug(1, 1, "eMMC Err: clkgen %Xh\n", eMMC_ST_ERR_INVALID_PARAM);
            eMMC_die(" ");
            return eMMC_ST_ERR_INVALID_PARAM;
    }

    if(u16_ClkParam & eMMC_PLL_FLAG)
    {
        //eMMC_debug(0,0,"eMMC PLL: %Xh\n", u16_ClkParam);
        REG_FCIE_CLRBIT(reg_ckg_fcie, BIT_FCIE_CLK_GATING|BIT_FCIE_CLK_INVERSE|BIT_CLKGEN_FCIE_MASK);

        eMMC_pll_setting(u16_ClkParam);

        if( g_eMMCDrv.u32_DrvFlag & DRV_FLAG_SPEED_HS400 ) // HS400
        {
            if( g_eMMCDrv.u8_PadType == FCIE_eMMC_HS400_DS )
            {
                REG_FCIE_SETBIT(reg_ckg_fcie, BIT_FCIE_CLK_SRC_SEL|BIT_FCIE_CLK_EMMC_PLL_2X<<2);
                eMMC_pll_dll_setting(); // tuning DLL setting
            }
            else
            {
                REG_FCIE_SETBIT(reg_ckg_fcie, BIT_FCIE_CLK_SRC_SEL|BIT_FCIE_CLK_EMMC_PLL_2X<<2);
            }
        }
        else if( g_eMMCDrv.u32_DrvFlag & DRV_FLAG_SPEED_HS200 ) // HS200
        {
            REG_FCIE_SETBIT(reg_ckg_fcie, BIT_FCIE_CLK_SRC_SEL|BIT_FCIE_CLK_EMMC_PLL_1X<<2);
        }
        else if( g_eMMCDrv.u32_DrvFlag & DRV_FLAG_DDR_MODE ) // DDR52
        {
            if( g_eMMCDrv.u8_PadType == FCIE_eMMC_DDR_8BIT_MACRO )
            {
                REG_FCIE_SETBIT(reg_ckg_fcie, BIT_FCIE_CLK_SRC_SEL|BIT_FCIE_CLK_EMMC_PLL_1X<<2);
            }
            else
            {
                REG_FCIE_SETBIT(reg_ckg_fcie, BIT_FCIE_CLK_SRC_SEL|BIT_FCIE_CLK_EMMC_PLL_2X<<2);
            }
		}
        else if( g_eMMCDrv.u32_DrvFlag & DRV_FLAG_SPEED_HIGH ) // HS
        {
            REG_FCIE_SETBIT(reg_ckg_fcie, BIT_FCIE_CLK_SRC_SEL|BIT_FCIE_CLK_EMMC_PLL_1X<<2);
        }
    }
    else
    {
        //eMMC_debug(0,0,"eMMC CLKGEN: %Xh\n", u16_ClkParam);
        REG_FCIE_CLRBIT(reg_ckg_fcie, BIT_FCIE_CLK_GATING|BIT_FCIE_CLK_INVERSE|BIT_CLKGEN_FCIE_MASK);
        REG_FCIE_SETBIT(reg_ckg_fcie, BIT_FCIE_CLK_SRC_SEL|u16_ClkParam<<2);
    }

    g_eMMCDrv.u16_ClkRegVal = (U16)u16_ClkParam;

    return eMMC_ST_SUCCESS;
}

U32 eMMC_clock_gating(void)
{
    eMMC_PlatformResetPre();
    REG_FCIE_CLRBIT(FCIE_SD_MODE, BIT_CLK_EN);
    eMMC_PlatformResetPost();
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

static  eMMC_FCIE_ATOP_SET_t sgTSetTmp_t;

U8 gau8_FCIEClkSel[eMMC_FCIE_VALID_CLK_CNT] =
{
    BIT_FCIE_CLK_48M,
    BIT_FCIE_CLK_43_2M,
    BIT_FCIE_CLK_40M,
    BIT_FCIE_CLK_36M,
    BIT_FCIE_CLK_32M,
    BIT_FCIE_CLK_20M,
    BIT_FCIE_CLK_300K
};

static U32 eMMC_FCIE_DetectTiming_Ex(U32 u32_Skew4Result)
{
    U16 u16_i, u16_StartBit=0xFFFF, u16_EndBit=0xFFFF, u16_cnt;
    U32 u32_tmp;

    sgTSetTmp_t.u32_ScanResult = u32_Skew4Result;

    // ==================================================
    // case.1: all bits set
    if((u32_Skew4Result & ((1<<PLL_SKEW4_CNT*2)-1)) == ((1<<PLL_SKEW4_CNT*2)-1) )
    {
        eMMC_debug(eMMC_DEBUG_LEVEL,0,"  case1: all\n");
        sgTSetTmp_t.u8_Reg2Ch=1;
        sgTSetTmp_t.u8_Skew4=0;
    }
    // ==================================================
    // case.2: not wrapped
    else if( 0==(u32_Skew4Result&BIT0) || 0==(u32_Skew4Result&(1<<(PLL_SKEW4_CNT*2-1))) )
    {
        for(u16_i=0; u16_i<PLL_SKEW4_CNT*2; u16_i++)
        {
            if((u32_Skew4Result&1<<u16_i) && 0xFFFF==u16_StartBit)
                u16_StartBit = u16_i;
            else if(0xFFFF!=u16_StartBit && 0==(u32_Skew4Result&1<<u16_i))
                u16_EndBit = u16_i-1;

            if(0xFFFF != u16_EndBit)
            {
                if(u16_EndBit-u16_StartBit+1<MIN_OK_SKEW_CNT)
                {   // to ignore "discontinous case"
                    u16_StartBit = 0xFFFF;
                    u16_EndBit = 0xFFFF;
                }
                else
                    break;
            }
        }

        if(0xFFFF==u16_EndBit)
        {
            if( u32_Skew4Result&1<<(u16_i-1) )
                u16_EndBit = u16_i-1;
            else
                u16_EndBit = u16_StartBit;
        }

        eMMC_debug(eMMC_DEBUG_LEVEL,0,"  case2: not wrapped: %u %u\n", u16_StartBit, u16_EndBit);

        if(u16_EndBit-u16_StartBit+1<MIN_OK_SKEW_CNT)
        {
            eMMC_debug(eMMC_DEBUG_LEVEL_ERROR, 1, "eMMC Err: Clk:%uMHz Case2 not enough skew4: %Xh %Xh\n",
                g_eMMCDrv.u32_ClkKHz/1000, u32_Skew4Result, u16_EndBit-u16_StartBit+1);
            return eMMC_ST_ERR_SKEW4;
        }

        if( (u16_StartBit+u16_EndBit)/2 < PLL_SKEW4_CNT)
        {
            sgTSetTmp_t.u8_Reg2Ch=0;
            sgTSetTmp_t.u8_Skew4=(u16_StartBit+u16_EndBit)/2;
        }
        else
        {
            sgTSetTmp_t.u8_Reg2Ch=1;
            sgTSetTmp_t.u8_Skew4=(u16_StartBit+u16_EndBit)/2 -PLL_SKEW4_CNT;
        }
    }
    // ==================================================
    // case.3: wrapped
    else
    {   // --------------------------------
        // to ignore "discontinous case"
        u16_cnt = 0;
        for(u16_i=0; u16_i<PLL_SKEW4_CNT*2; u16_i++)
        {
            if(u32_Skew4Result&1<<u16_i)
                u16_cnt++;
            else
            {
                u16_StartBit = u16_i-1;
                break;
            }
        }
        for(u16_i=PLL_SKEW4_CNT*2-1; u16_i>0; u16_i--)
        {
            if(u32_Skew4Result&1<<u16_i)
                u16_cnt++;
            else
            {
                u16_EndBit = u16_i+1;
                break;
            }
        }
        if(u16_cnt < MIN_OK_SKEW_CNT)
        {
            eMMC_debug(eMMC_DEBUG_LEVEL,0,"  case3: wrapped but goto case2: %u, %u %u\n",
                u16_cnt, u16_StartBit, u16_EndBit);
            u32_Skew4Result &= ~((1<<(u16_StartBit+1))-1);
            return eMMC_FCIE_DetectTiming_Ex(u32_Skew4Result);
        }
        else
        {
            u32_tmp = u32_Skew4Result;
            for(u16_i=u16_StartBit+1; u16_i<u16_EndBit; u16_i++)
                u32_Skew4Result &= ~(1<<u16_i);
            u16_StartBit = 0xFFFF;
            u16_EndBit = 0xFFFF;
        }

        // --------------------------------
        // normal judgement
        for(u16_i=0; u16_i<PLL_SKEW4_CNT*2; u16_i++)
        {
            if(0==(u32_Skew4Result&1<<u16_i) && 0xFFFF==u16_StartBit)
                u16_StartBit = u16_i-1;
            else if(0xFFFF!=u16_StartBit && (u32_Skew4Result&1<<u16_i))
                u16_EndBit = u16_i;
            if(0xFFFF != u16_EndBit)
                break;
        }
        eMMC_debug(eMMC_DEBUG_LEVEL,0,"  case3: wrapped: %u %u, %Xh %Xh\n",
            u16_StartBit, u16_EndBit, u32_tmp, u32_Skew4Result);

        if(u16_StartBit+1 > PLL_SKEW4_CNT*2-u16_EndBit)
        {
            sgTSetTmp_t.u8_Reg2Ch=0;
            sgTSetTmp_t.u8_Skew4 = (u16_StartBit-(PLL_SKEW4_CNT*2-u16_EndBit))/2;
        }
        else
        {
            sgTSetTmp_t.u8_Reg2Ch=1;
            sgTSetTmp_t.u8_Skew4 = u16_EndBit +
                ((PLL_SKEW4_CNT*2-u16_EndBit)+u16_StartBit)/2 -
                PLL_SKEW4_CNT;
        }
    }

    eMMC_debug(eMMC_DEBUG_LEVEL, 0, "  %Xh %Xh \n", sgTSetTmp_t.u8_Reg2Ch, sgTSetTmp_t.u8_Skew4);
    if(sgTSetTmp_t.u8_Skew4 >= PLL_SKEW4_CNT)
        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: Skew4:%u > %u\n", sgTSetTmp_t.u8_Skew4, PLL_SKEW4_CNT);

    return eMMC_ST_SUCCESS;
}

U32 eMMC_FCIE_DetectDDRTiming(void)
{
    U32 u32_i, u32_err, u32_Skew4Result=0;
    U16 u16_SkewCnt=0;

    g_eMMCDrv.u32_DrvFlag |= DRV_FLAG_TUNING_TTABLE; // turn on tunning flag

    REG_FCIE_CLRBIT(reg_emmcpll_0x6c, BIT7);
    for(u32_i=0; u32_i<PLL_SKEW4_CNT; u32_i++)
    {
        // make sure a complete outside clock cycle
        REG_FCIE_CLRBIT(FCIE_SD_MODE, BIT_CLK_EN);

        REG_FCIE_CLRBIT(reg_emmcpll_0x03, BIT_SKEW4_MASK);
        REG_FCIE_SETBIT(reg_emmcpll_0x03, u32_i<<12);

        u32_err = eMMCTest_BlkWRC_ProbeTiming(eMMC_TEST_BLK_0);
        if(eMMC_ST_SUCCESS==u32_err)
        {
            u16_SkewCnt++;
            u32_Skew4Result |= (1<<u32_i);
        }
    }

    REG_FCIE_SETBIT(reg_emmcpll_0x6c, BIT7);
    for(u32_i=PLL_SKEW4_CNT; u32_i<PLL_SKEW4_CNT*2; u32_i++)
    {
        // make sure a complete outside clock cycle
        REG_FCIE_CLRBIT(FCIE_SD_MODE, BIT_CLK_EN);

        REG_FCIE_CLRBIT(reg_emmcpll_0x03, BIT_SKEW4_MASK);
        REG_FCIE_SETBIT(reg_emmcpll_0x03, (u32_i-PLL_SKEW4_CNT)<<12);

        u32_err = eMMCTest_BlkWRC_ProbeTiming(eMMC_TEST_BLK_0);
        if(eMMC_ST_SUCCESS == u32_err)
        {
            u16_SkewCnt++;
            u32_Skew4Result |= (1<<u32_i);
        }
    }

    g_eMMCDrv.u32_DrvFlag &= ~DRV_FLAG_TUNING_TTABLE; // turn off tunning flag

    if(0==u32_Skew4Result || u16_SkewCnt<MIN_OK_SKEW_CNT)
    {
        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: Clk: %uMHz SkewResult: %Xh SkewCnt: %u\n",
            g_eMMCDrv.u32_ClkKHz, u32_Skew4Result, u16_SkewCnt);
        return eMMC_ST_ERR_SKEW4;
    }

    return eMMC_FCIE_DetectTiming_Ex(u32_Skew4Result);
}

U32 eMMC_FCIE_BuildDDRTimingTable(void)
{
    U32 u32_err, u32_ret;
    U8  u8_SetIdx, u8_ClkIdx;

    eMMC_debug(eMMC_DEBUG_LEVEL,1,"eMMC Info: building DDR table, please wait... \n");
    memset((void*)&g_eMMCDrv.TimingTable_t, 0, sizeof(g_eMMCDrv.TimingTable_t));
    u8_SetIdx = 0;

    if(!eMMC_IF_NORMAL_SDR())
    {
        u32_err = eMMC_FCIE_EnableSDRMode();
        if(eMMC_ST_SUCCESS != u32_err)
        {
            eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,0,"eMMC Err: set SDR IF fail: %X\n", u32_err);
            return u32_err;
        }
    }

    u32_err = eMMC_FCIE_EnableFastMode_Ex(FCIE_eMMC_DDR);
    if(eMMC_ST_SUCCESS != u32_err)
    {
        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,0,"eMMC Err: set DDR IF fail: %X\n", u32_err);
        return u32_err;
    }

    for(u8_ClkIdx=0; u8_ClkIdx<eMMC_FCIE_VALID_CLK_CNT; u8_ClkIdx++)
    {
        eMMC_clock_setting(gau8_eMMCPLLSel_52[u8_ClkIdx]);
        eMMC_debug(eMMC_DEBUG_LEVEL_HIGH,0,"\nclk: %uMHz\n", g_eMMCDrv.u32_ClkKHz/1000);

        // ---------------------------
        // search and set the Windows
        u32_err = eMMC_FCIE_DetectDDRTiming();

        // ---------------------------
        // set the Table
        if(eMMC_ST_SUCCESS == u32_err)
        {
            g_eMMCDrv.TimingTable_t.u8_SetCnt++;
            g_eMMCDrv.TimingTable_t.Set[u8_ClkIdx].u8_Clk = gau8_eMMCPLLSel_52[u8_ClkIdx];
            g_eMMCDrv.TimingTable_t.Set[u8_ClkIdx].u8_Reg2Ch = sgTSetTmp_t.u8_Reg2Ch;
            g_eMMCDrv.TimingTable_t.Set[u8_ClkIdx].u8_Skew4 = sgTSetTmp_t.u8_Skew4;
            g_eMMCDrv.TimingTable_t.Set[u8_ClkIdx].u32_ScanResult = sgTSetTmp_t.u32_ScanResult;
            g_eMMCDrv.TimingTable_t.u32_VerNo = eMMC_TIMING_TABLE_VERSION;

            eMMC_DumpTimingTable();
            eMMC_debug(eMMC_DEBUG_LEVEL, 0, "\nOK\n");

            g_eMMCDrv.TimingTable_t.u32_ChkSum =
                eMMC_ChkSum((U8*)&g_eMMCDrv.TimingTable_t,
                sizeof(g_eMMCDrv.TimingTable_t)-eMMC_TIMING_TABLE_CHKSUM_OFFSET);
            memcpy(gau8_eMMC_SectorBuf, (U8*)&g_eMMCDrv.TimingTable_t, sizeof(g_eMMCDrv.TimingTable_t));

            eMMC_FCIE_ErrHandler_ReInit();

            u32_err = eMMC_CMD24(eMMC_DDRTABLE_BLK_0, gau8_eMMC_SectorBuf);
            u32_ret = eMMC_CMD24(eMMC_DDRTABLE_BLK_1, gau8_eMMC_SectorBuf);
            if(eMMC_ST_SUCCESS!=u32_err && eMMC_ST_SUCCESS!=u32_ret)
            {
                eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: Save TTable fail: %Xh %Xh\n",
                    u32_err, u32_ret);
                return eMMC_ST_ERR_SAVE_DDRT_FAIL;
            }

            return eMMC_ST_SUCCESS;
        }
    }

    eMMC_DumpTimingTable();
    eMMC_die("");
    return eMMC_ST_ERR_SKEW4;

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

U32 eMMC_FCIE_DetectHS200Timing(void)
{
    U32 u32_i, u32_err, u32_Skew4Result=0;
    U16 u16_SkewCnt=0;

    for(u32_i=0; u32_i<PLL_SKEW4_CNT; u32_i++)
    {
        // make sure a complete outside clock cycle
        REG_FCIE_CLRBIT(FCIE_SD_MODE, BIT_CLK_EN);

        g_eMMCDrv.u32_DrvFlag |= DRV_FLAG_TUNING_TTABLE; // turn on tunning flag
        REG_FCIE_CLRBIT(reg_emmcpll_0x6c, BIT7);

        REG_FCIE_CLRBIT(reg_emmcpll_0x03, BIT_SKEW4_MASK);
        REG_FCIE_SETBIT(reg_emmcpll_0x03, u32_i<<12);

        u32_err = eMMC_CMD21();
        if(eMMC_ST_SUCCESS==u32_err && eMMC_ST_SUCCESS==eMMCTest_BlkWRC_ProbeTiming(eMMC_TEST_BLK_0))
        {
            u16_SkewCnt++;
            u32_Skew4Result |= (1<<u32_i);
        }
        else
        {   //u32_err = eMMC_FCIE_Init();
            eMMC_debug(0,1,"failed skew4: %u\n", u32_i);
            u32_err = eMMC_FCIE_ResetToHS200((U8)g_eMMCDrv.u16_ClkRegVal);
            if(eMMC_ST_SUCCESS != u32_err)
                break;
        }
    }

    for(u32_i=PLL_SKEW4_CNT; u32_i<PLL_SKEW4_CNT*2; u32_i++)
    {
        // make sure a complete outside clock cycle
        REG_FCIE_CLRBIT(FCIE_SD_MODE, BIT_CLK_EN);

        g_eMMCDrv.u32_DrvFlag |= DRV_FLAG_TUNING_TTABLE; // turn on tunning flag
        REG_FCIE_SETBIT(reg_emmcpll_0x6c, BIT7);

        REG_FCIE_CLRBIT(reg_emmcpll_0x03, BIT_SKEW4_MASK);
        REG_FCIE_SETBIT(reg_emmcpll_0x03, (u32_i-PLL_SKEW4_CNT)<<12);

        u32_err = eMMC_CMD21();
        if(eMMC_ST_SUCCESS==u32_err && eMMC_ST_SUCCESS==eMMCTest_BlkWRC_ProbeTiming(eMMC_TEST_BLK_0))
        {
            u16_SkewCnt++;
            u32_Skew4Result |= (1<<u32_i);
        }
        else
        {   //u32_err = eMMC_FCIE_Init();
            eMMC_debug(0,1,"failed skew4: %u\n", u32_i);

            u32_err = eMMC_FCIE_ResetToHS200((U8)g_eMMCDrv.u16_ClkRegVal);
            if(eMMC_ST_SUCCESS != u32_err)
                break;
        }
    }

    g_eMMCDrv.u32_DrvFlag &= ~DRV_FLAG_TUNING_TTABLE; // turn off tunning flag

    if(0==u32_Skew4Result || u16_SkewCnt<MIN_OK_SKEW_CNT)
    {
        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: Clk: %uMHz SkewResult: %Xh SkewCnt: %u\n",
            g_eMMCDrv.u32_ClkKHz, u32_Skew4Result, u16_SkewCnt);
        return eMMC_ST_ERR_SKEW4;
    }

    return eMMC_FCIE_DetectTiming_Ex(u32_Skew4Result);
}

U32 eMMC_FCIE_BuildHS200TimingTable(void)
{
    U32 u32_err, u32_ret;
    U8  u8_SetIdx, u8_ClkIdx;

    eMMC_debug(eMMC_DEBUG_LEVEL,0,"eMMC Info: building HS200 table, please wait... \n");
    memset((void*)&g_eMMCDrv.TimingTable_t, 0, sizeof(g_eMMCDrv.TimingTable_t));
    u8_SetIdx = 0;

    // --------------------------------
    // tuning FCIE & macro for HS200
    for(u8_ClkIdx=0; u8_ClkIdx<eMMC_FCIE_VALID_CLK_CNT; u8_ClkIdx++)
    {
        u32_err = eMMC_FCIE_ResetToHS200(gau8_eMMCPLLSel_200[u8_ClkIdx]);
        if(eMMC_ST_SUCCESS != u32_err)
        {
            eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: eMMC_FCIE_ResetToHS200 fail: %Xh\n", u32_err);
            return u32_err;
        }
        eMMC_debug(eMMC_DEBUG_LEVEL,0,"\nclk: %uMHz\n", g_eMMCDrv.u32_ClkKHz/1000);

        // ---------------------------
        // search and set the Windows
        u32_err = eMMC_FCIE_DetectHS200Timing();

        // ---------------------------
        // set the Table
        if(eMMC_ST_SUCCESS == u32_err)
        {
            //eMMC_FCIE_Init();
            g_eMMCDrv.TimingTable_t.u8_SetCnt++;
            g_eMMCDrv.TimingTable_t.Set[0].u8_Clk = gau8_eMMCPLLSel_200[u8_ClkIdx];
            g_eMMCDrv.TimingTable_t.Set[0].u8_Reg2Ch = sgTSetTmp_t.u8_Reg2Ch;
            g_eMMCDrv.TimingTable_t.Set[0].u8_Skew4 = sgTSetTmp_t.u8_Skew4;
            g_eMMCDrv.TimingTable_t.Set[0].u32_ScanResult = sgTSetTmp_t.u32_ScanResult;
            g_eMMCDrv.TimingTable_t.u32_VerNo = eMMC_TIMING_TABLE_VERSION;

            eMMC_DumpTimingTable();
            eMMC_debug(eMMC_DEBUG_LEVEL, 0, "\nOK\n");

            g_eMMCDrv.TimingTable_t.u32_ChkSum =
                eMMC_ChkSum((U8*)&g_eMMCDrv.TimingTable_t,
                sizeof(g_eMMCDrv.TimingTable_t)-eMMC_TIMING_TABLE_CHKSUM_OFFSET);
            memcpy(gau8_eMMC_SectorBuf, (U8*)&g_eMMCDrv.TimingTable_t, sizeof(g_eMMCDrv.TimingTable_t));

            eMMC_FCIE_ErrHandler_ReInit();

            if(!eMMC_CHK_BKG_SCAN_HS200())
            {
                u32_err = eMMC_CMD24(eMMC_HS200TABLE_BLK_0, gau8_eMMC_SectorBuf);
                u32_ret = eMMC_CMD24(eMMC_HS200TABLE_BLK_1, gau8_eMMC_SectorBuf);

                if(eMMC_ST_SUCCESS!=u32_err && eMMC_ST_SUCCESS!=u32_ret)
                {
                    eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: Save TTable fail: %Xh %Xh\n",
                        u32_err, u32_ret);
                    return eMMC_ST_ERR_SAVE_DDRT_FAIL;
                }
            }

            return eMMC_ST_SUCCESS;
        }
    }

    eMMC_DumpTimingTable();
    eMMC_die("");

    return eMMC_ST_ERR_SKEW4;
}

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
        REG_FCIE_CLRBIT(reg_emmcpll_0x6c, BIT7);
        REG_FCIE_SETBIT(reg_emmcpll_0x03, u32Value<<12);
    }
    else // 9~17
    {
        REG_FCIE_SETBIT(reg_emmcpll_0x6c, BIT7); // inverse
        REG_FCIE_SETBIT(reg_emmcpll_0x03, (u32Value-9)<<12);
    }

	eMMC_debug(eMMC_DEBUG_LEVEL_LOW, 1, "EMMCPLL[03h]=%04X, [6Ch]=%04X\n",
        REG_FCIE(reg_emmcpll_0x03), REG_FCIE(reg_emmcpll_0x6c));

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

#define eMMC_IRQ_DEBUG  0

irqreturn_t eMMC_FCIE_IRQ(int irq, void *dummy)
{
    volatile u16 u16_Events;

    //printk("fcie IRQ\n");

    if((REG_FCIE(FCIE_MIE_FUNC_CTL) & BIT_EMMC_ACTIVE) != BIT_EMMC_ACTIVE)
    {
        return IRQ_NONE;
    }

    // one time enable one bit
    u16_Events = REG_FCIE(FCIE_MIE_EVENT) & REG_FCIE(FCIE_MIE_INT_EN);

    if(u16_Events & BIT_DMA_END)
    {
        REG_FCIE_CLRBIT(FCIE_MIE_INT_EN, BIT_DMA_END);

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


U32 eMMC_WaitCompleteIntr(U32 u32_RegAddr, U16 u16_WaitEvent, U32 u32_MicroSec)
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
            if((REG_FCIE(u32_RegAddr) & u16_WaitEvent) == u16_WaitEvent )
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
            eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Warn: but polling ok: %Xh \n", REG_FCIE(u32_RegAddr));
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
    //	return TRUE;

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
extern struct semaphore	PfModeSem;
#endif

extern bool ms_sdmmc_wait_d0_for_emmc(void);
#if IF_FCIE_SHARE_IP && defined(CONFIG_MSTAR_SDMMC)
static U16 sgu16_SD_Pad;
#endif

void eMMC_LockFCIE(U8 *pu8_str)
{
    #if 0
    //eMMC_debug(0,1,"%s 1\n", pu8_str);
    #if (defined(IF_FCIE_SHARE_PINS) && IF_FCIE_SHARE_PINS) || \
        (defined(IF_FCIE_SHARE_IP) && IF_FCIE_SHARE_IP)
    down(&PfModeSem);
    #endif
    //eMMC_debug(0,1,"%s 2\n", pu8_str);

    #if IF_FCIE_SHARE_IP && defined(CONFIG_MSTAR_SDMMC)
    if(false == ms_sdmmc_wait_d0_for_emmc())
    {
        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: SD keep D0 low \n");
        eMMC_FCIE_ErrHandler_Stop();
    }

    eMMC_pads_switch(g_eMMCDrv.u8_PadType);
    eMMC_clock_setting(g_eMMCDrv.u16_ClkRegVal);
    #endif

    REG_FCIE_SETBIT(FCIE_SD_MODE, BIT_SD_CLK_EN); // output clock
    #endif

    REG_FCIE_SETBIT(FCIE_MIE_FUNC_CTL, BIT_EMMC_ACTIVE);
}

void eMMC_UnlockFCIE(U8 *pu8_str)
{
    //eMMC_debug(0,1,"%s 1\n", pu8_str);
    #if 0
    REG_FCIE_CLRBIT(FCIE_MIE_FUNC_CTL, BIT_EMMC_ACTIVE);
    REG_FCIE_CLRBIT(FCIE_SD_MODE, BIT_SD_CLK_EN); // not output clock

    #if (defined(IF_FCIE_SHARE_PINS) && IF_FCIE_SHARE_PINS) || \
        (defined(IF_FCIE_SHARE_IP) && IF_FCIE_SHARE_IP)
    up(&PfModeSem);
    #endif
    #endif

    //eMMC_debug(0,1,"%s 2\n", pu8_str);
}

//---------------------------------------

U32 eMMC_PlatformResetPre(void)
{
    return eMMC_ST_SUCCESS;
}

U32 eMMC_PlatformResetPost(void)
{
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
