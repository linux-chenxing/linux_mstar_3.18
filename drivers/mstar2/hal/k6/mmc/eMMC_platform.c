//<MStar Software>
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2006 - 2007 MStar Semiconductor, Inc.
// This program is free software.
// You can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation;
// either version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program;
// if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

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
        case FCIE_eMMC_BYPASS:  eMMC_debug(0, 0, "Bypass\n");   break;
        case FCIE_eMMC_SDR:     eMMC_debug(0, 0, "SDR\n");      break;
        case FCIE_eMMC_DDR:     eMMC_debug(0, 0, "DDR\n");      break;
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
        BIT_FALL_LATCH|BIT_PAD_IN_SEL_SD|BIT_32BIT_MACRO_EN|BIT_CLK2_SEL|BIT_DDR_EN|BIT_8BIT_MACRO_EN);

    // emmc_pll
    REG_FCIE_CLRBIT(reg_emmcpll_0x1c, BIT9);

    REG_FCIE_CLRBIT(reg_emmcpll_0x68, BIT0);

    REG_FCIE_CLRBIT(reg_emmcpll_0x6a, BIT0|BIT1);

    REG_FCIE_W(reg_emmcpll_0x6b, 0x0000);

    REG_FCIE_CLRBIT(reg_emmcpll_0x6d, BIT0);

    switch(u32Mode)
    {
        case FCIE_eMMC_BYPASS:
            eMMC_debug(eMMC_DEBUG_LEVEL_HIGH, 1, "Bypass\n");

            //fcie
            REG_FCIE_SETBIT(FCIE_DDR_MODE, BIT_PAD_IN_SEL_SD|BIT_FALL_LATCH|BIT_CLK2_SEL);

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

    return eMMC_ST_SUCCESS;

    ErrorHandle:

    eMMC_debug(1, 1, "eMMC Err: set bus width before pad switch\n");

    return eMMC_ST_ERR_INVALID_PARAM;
}

// Notice!!! you need to set pad before config clock
U32 eMMC_clock_setting(U16 u16_ClkParam)
{
    eMMC_debug(eMMC_DEBUG_LEVEL_LOW, 1, "eMMC_clock_setting(%Xh)\n", u16_ClkParam);

//    eMMC_PlatformResetPre();

    REG_FCIE_CLRBIT(FCIE_SD_MODE, BIT_SD_CLK_EN);

    switch(u16_ClkParam)
    {
        // clock_gen fcie clock
        case BIT_FCIE_CLK_1X_XTAL:  g_eMMCDrv.u32_ClkKHz =   3000;  break;
        case BIT_FCIE_CLK_1X_12M:   g_eMMCDrv.u32_ClkKHz =  12000;  break;
        case BIT_FCIE_CLK_1X_15_5M: g_eMMCDrv.u32_ClkKHz =  15500;  break;
        case BIT_FCIE_CLK_1X_18M:   g_eMMCDrv.u32_ClkKHz =  18000;  break;
        case BIT_FCIE_CLK_1X_21_5M: g_eMMCDrv.u32_ClkKHz =  21500;  break;
        case BIT_FCIE_CLK_1X_27M:   g_eMMCDrv.u32_ClkKHz =  27000;  break;
        case BIT_FCIE_CLK_1X_36M:   g_eMMCDrv.u32_ClkKHz =  36000;  break;
        case BIT_FCIE_CLK_1X_40M:   g_eMMCDrv.u32_ClkKHz =  40000;  break;
        case BIT_FCIE_CLK_1X_48M:   g_eMMCDrv.u32_ClkKHz =  48000;  break;
        case BIT_FCIE_CLK_1X_54M:   g_eMMCDrv.u32_ClkKHz =  54000;  break;
        case BIT_FCIE_CLK_1X_60M:   g_eMMCDrv.u32_ClkKHz =  60000;  break;
        case BIT_FCIE_CLK_1X_88_5M: g_eMMCDrv.u32_ClkKHz =  88500;  break;
        case BIT_FCIE_CLK_1X_75K:   g_eMMCDrv.u32_ClkKHz =     75;  break;
        case BIT_FCIE_CLK_1X_80M:   g_eMMCDrv.u32_ClkKHz =  80000;  break;

        case BIT_FCIE_CLK_4X_XTAL:  g_eMMCDrv.u32_ClkKHz =  12000;  break;
        case BIT_FCIE_CLK_4X_48M:   g_eMMCDrv.u32_ClkKHz =  48000;  break;
        case BIT_FCIE_CLK_4X_62M:   g_eMMCDrv.u32_ClkKHz =  62000;  break;
        case BIT_FCIE_CLK_4X_72M:   g_eMMCDrv.u32_ClkKHz =  72000;  break;
        case BIT_FCIE_CLK_4X_86M:   g_eMMCDrv.u32_ClkKHz =  86000;  break;
        case BIT_FCIE_CLK_4X_108M:  g_eMMCDrv.u32_ClkKHz = 108000;  break;
        case BIT_FCIE_CLK_4X_144M:  g_eMMCDrv.u32_ClkKHz = 144000;  break;
        case BIT_FCIE_CLK_4X_160M:  g_eMMCDrv.u32_ClkKHz = 160000;  break;
        case BIT_FCIE_CLK_4X_192M:  g_eMMCDrv.u32_ClkKHz = 192000;  break;
        case BIT_FCIE_CLK_4X_216M:  g_eMMCDrv.u32_ClkKHz = 216000;  break;
        case BIT_FCIE_CLK_4X_240M:  g_eMMCDrv.u32_ClkKHz = 240000;  break;
        case BIT_FCIE_CLK_4X_354M:  g_eMMCDrv.u32_ClkKHz = 354000;  break;
        case BIT_FCIE_CLK_4X_300K:  g_eMMCDrv.u32_ClkKHz =    300;  break;
        case BIT_FCIE_CLK_4X_320M:  g_eMMCDrv.u32_ClkKHz = 320000;  break;

        default:
            eMMC_debug(1, 1, "eMMC Err: clkgen %Xh\n", eMMC_ST_ERR_INVALID_PARAM);
            eMMC_die(" ");
            return eMMC_ST_ERR_INVALID_PARAM;
    }

    REG_FCIE_CLRBIT(reg_ckg_fcie, BIT_FCIE_CLK_GATING|BIT_FCIE_CLK_INVERSE|BIT_CLKGEN_FCIE_MASK|BIT_CKG_NFIE_DVI4_EN);
    if(u16_ClkParam & eMMC_4X_FLAG) {
        if( (g_eMMCDrv.u8_PadType == FCIE_eMMC_SDR) || (g_eMMCDrv.u8_PadType == FCIE_eMMC_DDR) )
        {
            REG_FCIE_SETBIT(reg_ckg_fcie, BIT_CKG_NFIE_DVI4_EN);
            g_eMMCDrv.u32_ClkKHz /= 4;
			//500msec= 0x10000 x 1/48MHZ x 0x16F
			REG_FCIE_W(FCIE_WR_SBIT_TIMER, BIT_WR_SBIT_TIMER_EN|0x16F);
			REG_FCIE_W(FCIE_RD_SBIT_TIMER, BIT_RD_SBIT_TIMER_EN|0x16F);
        }
        else
        {
            REG_FCIE_SETBIT(reg_ckg_fcie, BIT_CKG_NFIE_DVI4_EN);
			//500msec= 0x10000 x 1/12MHZ x 0x5C
			REG_FCIE_W(FCIE_WR_SBIT_TIMER, BIT_WR_SBIT_TIMER_EN|0x5C);
			REG_FCIE_W(FCIE_RD_SBIT_TIMER, BIT_RD_SBIT_TIMER_EN|0x5C);
        }
    }
    REG_FCIE_SETBIT(reg_ckg_fcie, BIT_FCIE_CLK_SRC_SEL|((u16_ClkParam & 0xF)<<10));

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

U8 gau8_eMMCPLLSel_52[eMMC_FCIE_VALID_CLK_CNT] = {
    eMMC_PLL_CLK_200M,
	eMMC_PLL_CLK_160M,
	eMMC_PLL_CLK_120M
};

U8 gau8_eMMCPLLSel_200[eMMC_FCIE_VALID_CLK_CNT] = {
    eMMC_PLL_CLK_200M,
    eMMC_PLL_CLK_160M,
    eMMC_PLL_CLK_140M
};

U8 gau8_FCIEClkSel[eMMC_FCIE_VALID_CLK_CNT] =
{
    BIT_FCIE_CLK_4X_192M,
    BIT_FCIE_CLK_4X_160M,
    BIT_FCIE_CLK_4X_144M,
};

U32 eMMC_FCIE_BuildDDRTimingTable(void)
{
    return eMMC_ST_SUCCESS;
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
