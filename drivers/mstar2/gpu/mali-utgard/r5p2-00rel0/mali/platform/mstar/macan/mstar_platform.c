/*
 * Copyright (C) 2010-2016 MStar Semiconductor, Inc. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 *
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/**
 * @file mstar_platform.c
 * MStar platform specific driver functions
 */
#include "mstar/mstar_platform.h"
#include <linux/delay.h>

#define REG_G3D_BASE                0x110800
#define REG_SOFT_RESET              0x00
#define MSK_SOFT_RESET              0x0001
#define MSK_SECURITY_SOFT_RESET     0x0002
#define REG_GPU_PROTECT_MODE        0x18
#define MSK_GPU_PROTECT_MODE        0x0001
#define REG_GPU_SRAM_SD_EN          0x43
#define MSK_GPU_SRAM_SD_EN          0x0008
#define REG_GPUPLL_CTRL0_LOW        0x44
#define REG_GPUPLL_CTRL0_HIGH       0x45
#define REG_GPUPLL_CTRL1_LOW        0x46
#define REG_GPUPLL_CTRL1_HIGH       0x47
#define MSK_GPU_CLOCK               0x00ff
#define REG_SPLIT_2CH_MD            0x60
#define READ_OUTSTANDING            0x0002
#define REG_FIFO_DEPTH              0x61
#define SHIFT_FIFO_DEPTH            0x5
#define MSK_FIFO_DEPTH              (0x7 << SHIFT_FIFO_DEPTH)
#define REG_G3D_RREQ                0x62
#define REG_G3D_RREQ_THRD           0x000f
#define REG_G3D_WREQ                0x63
#define REG_G3D_WREQ_THRD           0x000f
#define REG_RIU_APB_EN              0x6a
#define MSK_RIU_APB_EN              0x0001
#define REG_MIU0_START_ADDR         0x75
#define REG_MIU0_END_ADDR           0x76
#define REG_MIU1_START_ADDR         0x77
#define REG_MIU1_END_ADDR           0x78
#define MIU0_PHY_BASE_ADDR_HIGH     0x0000
#define MIU0_PHY_END_ADDR_HIGH      MIU1_PHY_BASE_ADDR_HIGH
#define MIU1_PHY_END_ADDR_HIGH      0x0fff

#define REG_CLKGEN1_BASE            0x103300
#define REG_CKG_GPU                 0x20
#define DISABLE_CLK_GPU             0x0001

#if defined(CONFIG_MALI_EFUSE_PP_CORE_NUM)
#define REG_EFUSE_BASE              0x002000
#define REG_EFUSE_STATUS            0x0a
#define REG_DISABLE_GPU_HALF_CORE_MASK  0x0002
#endif

/* RIU mapping */
#define G3D_REG(addr)               RIU[REG_G3D_BASE + ((addr) << 1)]
#define CLKGEN1_REG(addr)           RIU[REG_CLKGEN1_BASE + ((addr) << 1)]
#if defined(CONFIG_MALI_EFUSE_PP_CORE_NUM)
#define EFUSE_REG(addr)             RIU[REG_EFUSE_BASE + ((addr) << 1)]
#endif

/* helper functions */
static void power_on(void)
{
    /* set GPU clock: must before power on */
    G3D_REG(REG_GPUPLL_CTRL0_LOW)  = 0x0000;
    G3D_REG(REG_GPUPLL_CTRL0_HIGH) = 0x0080;
    G3D_REG(REG_GPUPLL_CTRL1_LOW)  = (G3D_REG(REG_GPUPLL_CTRL1_LOW) & ~MSK_GPU_CLOCK) | ((GPU_MAX_CLOCK/24) & MSK_GPU_CLOCK);
    G3D_REG(REG_GPUPLL_CTRL1_HIGH) = 0x0001;
    udelay(100);

    /* enable read by outstanding order*/
    G3D_REG(REG_SPLIT_2CH_MD) |= READ_OUTSTANDING;

    /* reg_g3d_rreq_thrd = 0x0 */
    G3D_REG(REG_G3D_RREQ) &= ~REG_G3D_RREQ_THRD;

    /* reg_g3d_wreq_thrd = 0x0 */
    G3D_REG(REG_G3D_WREQ) &= ~REG_G3D_WREQ_THRD;

    /* Set MIU0 base and end address */
    G3D_REG(REG_MIU0_START_ADDR) = MIU0_PHY_BASE_ADDR_HIGH;
    G3D_REG(REG_MIU0_END_ADDR) = MIU0_PHY_END_ADDR_HIGH;
    udelay(100);

    /* Set MIU1 base and end address */
    G3D_REG(REG_MIU1_START_ADDR) = MIU1_PHY_BASE_ADDR_HIGH;
    G3D_REG(REG_MIU1_END_ADDR) = MIU1_PHY_END_ADDR_HIGH;
    udelay(100);

    /* enable RIU access */
#ifdef MSTAR_RIU_ENABLED
    G3D_REG(REG_RIU_APB_EN) |= MSK_RIU_APB_EN;
    udelay(100);
#endif

    /* In the hardware design, the maximun FIFO depth is 64.
     * It will simulate higher FIFO depth level according to this register, but the data could be wrong */
    G3D_REG(REG_FIFO_DEPTH) = (G3D_REG(REG_FIFO_DEPTH) & ~MSK_FIFO_DEPTH) | (0x2 << SHIFT_FIFO_DEPTH);
    udelay(100);

    /* disable GPU clock gating */
    CLKGEN1_REG(REG_CKG_GPU) &= ~DISABLE_CLK_GPU;
    udelay(100);

    /* power up SRAM */
    G3D_REG(REG_GPU_SRAM_SD_EN) &= ~MSK_GPU_SRAM_SD_EN;
    udelay(100);

    /* reset mali */
    G3D_REG(REG_SOFT_RESET) &= ~MSK_SOFT_RESET;
    G3D_REG(REG_SOFT_RESET) |= MSK_SOFT_RESET;
    udelay(100); /*delay for run-time suspend*/
    G3D_REG(REG_SOFT_RESET) &= ~MSK_SOFT_RESET;
    udelay(100);
}

static void power_off(void)
{
    /* power down SRAM */
    G3D_REG(REG_GPU_SRAM_SD_EN) |= MSK_GPU_SRAM_SD_EN;
    udelay(100);

    /* enable GPU clock gating */
    CLKGEN1_REG(REG_CKG_GPU) |= DISABLE_CLK_GPU;
    udelay(100);
}

/* platform functions */
void mstar_platform_init(void)
{
    mali_gpu_clock = GPU_MAX_CLOCK;
    power_on();
}

void mstar_platform_deinit(void)
{
    power_off();
}

void mstar_platform_suspend(void)
{
    power_off(); /* just power off */
}

void mstar_platform_resume(void)
{
    power_on(); /* just power on */
}

void mstar_platform_runtime_suspend(void)
{
    power_off();
}

void mstar_platform_runtime_resume(void)
{
    /* disable GPU clock gating */
    CLKGEN1_REG(REG_CKG_GPU) &= ~DISABLE_CLK_GPU;
    udelay(100);

    /* power up SRAM */
    G3D_REG(REG_GPU_SRAM_SD_EN) &= ~MSK_GPU_SRAM_SD_EN;
    udelay(100);
}

#if defined(CONFIG_MALI_EFUSE_PP_CORE_NUM)
int mstar_platform_get_efuse_core_num(void)
{
    int core_num;
    bool bDisable_half_core = (EFUSE_REG(REG_EFUSE_STATUS) & REG_DISABLE_GPU_HALF_CORE_MASK) ? true : false;
    core_num = bDisable_half_core ? NUM_PP >> 1 : NUM_PP;
    return core_num;
}
#endif
