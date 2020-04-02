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
#if defined(CONFIG_MALI_DVFS)
#include "mali_kernel_common.h"
#endif

#include "mstar/mstar_chip.h"

/* REG_GPU */
#define REG_GPU_BASE                0x110800

#define REG_GPU_SOFT_RESET          0x00
#define MSK_GPU_SOFT_RESET          0x0001

#define REG_GPU_RREQ_RD_LEN         0x61
#define SHIFT_GPU_RREQ_RD_LEN       0x5
#define MSK_GPU_RREQ_RD_LEN         (0x7 << SHIFT_GPU_RREQ_RD_LEN)
#define VAL_GPU_RREQ_RD_LEN         (0x2 << SHIFT_GPU_RREQ_RD_LEN)

#define REG_GPU_FLUSH               0x61
#define SHIFT_TIMEOUT_FLUSH         0x8
#define MSK_TIMEOUT_FLUSH           (0xff << SHIFT_TIMEOUT_FLUSH)
#define VAL_TIME_OUT_FLUSH_1T       (0x1 << SHIFT_TIMEOUT_FLUSH)

#define REG_GPU_RREQ                0x62
#define MSK_GPU_RREQ_THRD           0x000f
#define MSK_GPU_RREQ_MAX            0xff00

#define REG_GPU_WREQ                0x63
#define MSK_GPU_WREQ_THRD           0x000f
#define MSK_GPU_WREQ_MAX            0xff00

#define REG_GPU_RIU_EN              0x6a
#define MSK_GPU_RIU_EN              0x0001

#define REG_GPU_MIU0_START          0x75
#define REG_GPU_MIU0_END            0x76
#define REG_GPU_MIU1_START          0x77
#define REG_GPU_MIU1_END            0x78

#define REG_GPU_TLB_ENABLE          0x7b
#define MSK_GPU_TLB_ENABLE          0x0004

#define REG_GPU_BWP_ENABLE          0x7b
#define MSK_GPU_BWP                 0x0002

#define REG_GPU_MIU0_TLB_START      0x7c
#define REG_GPU_MIU0_TLB_END        0x7d
#define REG_GPU_MIU1_TLB_START      0x7e
#define REG_GPU_MIU1_TLB_END        0x7f

#define MIU0_PHY_BASE_ADDR_HIGH     0x0000
#define MIU0_PHY_END_ADDR_HIGH      MIU1_PHY_BASE_ADDR_HIGH
#define MIU1_PHY_END_ADDR_HIGH      0x0fff

#define TLB_MIU0_START_ADDR         0x40000000UL
#define TLB_MIU0_START_ADDR_H       ((TLB_MIU0_START_ADDR >> 16) & 0xFFFF)
#define TLB_MIU0_SIZE               0x10000000UL
#define TLB_MIU0_END_ADDR           (TLB_MIU0_START_ADDR + TLB_MIU0_SIZE)
#define TLB_MIU0_END_ADDR_H         ((TLB_MIU0_END_ADDR >> 16) & 0xFFFF)

#define TLB_MIU1_START_ADDR         0x50000000UL
#define TLB_MIU1_START_ADDR_H       ((TLB_MIU1_START_ADDR >> 16) & 0xFFFF)
#define TLB_MIU1_SIZE               0x10000000UL
#define TLB_MIU1_END_ADDR           (TLB_MIU1_START_ADDR + TLB_MIU1_SIZE)
#define TLB_MIU1_END_ADDR_H         ((TLB_MIU1_END_ADDR >> 16) & 0xFFFF)

/* REG_GPU_PLL */
#define REG_GPU_PLL_BASE            0x160400

#define REG_GPUPLL_CTRL0            0x03
#define REG_GPUPLL_CTRL1            0x04
#define REG_GPUPLL_CTRL2            0x05
#define VAL_GPUPLL_POWER_ON         0x0080

#define MSK_GPU_CLOCK               0x00ff

/* REG_CLKGEN */
#define REG_CLKGEN_BASE             0x100a00

#define REG_CKG_GPU                 0x3d
#define MSK_DISABLE_CLK             0x0001

/* REG_CLKGEN */
#define REG_SRAM_BASE               0x171700

#define REG_GPU_SRAM                0x10
#define MSK_GPU_SRAM_DISABLE        0x0001

/* RIU mapping */
#define GPU_REG(addr)               RIU[(REG_GPU_BASE) + ((addr) << 1)]
#define GPU_PLL_REG(addr)           RIU[(REG_GPU_PLL_BASE) + ((addr) << 1)]
#define CLKGEN_REG(addr)            RIU[(REG_CLKGEN_BASE) + ((addr) << 1)]
#define SRAM_REG(addr)              RIU[(REG_SRAM_BASE) + ((addr) << 1)]

#define GPU_CLOCK_STEP              24

/* helper functions */
static void power_on(void)
{
    /* Init GPU clock */
    GPU_PLL_REG(REG_GPUPLL_CTRL0) = 0x0082;
    GPU_PLL_REG(REG_GPUPLL_CTRL1) = 0x1000 | ((GPU_MAX_CLOCK/GPU_CLOCK_STEP) & MSK_GPU_CLOCK);
    GPU_PLL_REG(REG_GPUPLL_CTRL2) = 0x0001;
    udelay(10);

    /* Set MIU0 base and end address */
    GPU_REG(REG_GPU_MIU0_START)  = MIU0_PHY_BASE_ADDR_HIGH;
    GPU_REG(REG_GPU_MIU0_END)  = MIU0_PHY_END_ADDR_HIGH;
    udelay(1);

    /* Set MIU1 base and end address */
    GPU_REG(REG_GPU_MIU1_START) = MIU1_PHY_BASE_ADDR_HIGH;
    GPU_REG(REG_GPU_MIU1_END) = MIU1_PHY_END_ADDR_HIGH;
    udelay(1);

    /* Default disable TLB */
    GPU_REG(REG_GPU_TLB_ENABLE) &= ~(MSK_GPU_TLB_ENABLE);
    udelay(1);

    /* Set MIU0 TLB addr */
    GPU_REG(REG_GPU_MIU0_TLB_START) = TLB_MIU0_START_ADDR_H;
    GPU_REG(REG_GPU_MIU0_TLB_END)   = TLB_MIU0_END_ADDR_H;

    /* Set MIU1 TLB addr */
    GPU_REG(REG_GPU_MIU1_TLB_START) = TLB_MIU1_START_ADDR_H;
    GPU_REG(REG_GPU_MIU1_TLB_END)   = TLB_MIU1_END_ADDR_H;
    udelay(1);

    /* Enable RIU */
#ifdef MSTAR_RIU_ENABLED
    GPU_REG(REG_GPU_RIU_EN) |= MSK_GPU_RIU_EN;
    udelay(1);
#endif

    /* Set read request length to 64 */
    GPU_REG(REG_GPU_RREQ_RD_LEN) = (GPU_REG(REG_GPU_RREQ_RD_LEN) & ~MSK_GPU_RREQ_RD_LEN) | VAL_GPU_RREQ_RD_LEN;
    udelay(1);

    /* Disable GPU BWP */
    GPU_REG(REG_GPU_BWP_ENABLE) &= ~(MSK_GPU_BWP);
    udelay(1);

    /* Set request flush timeout to 1 to increase performance */
    GPU_REG(REG_GPU_FLUSH) = (GPU_REG(REG_GPU_FLUSH) & ~MSK_TIMEOUT_FLUSH) | VAL_TIME_OUT_FLUSH_1T;
    udelay(1);

    /* Power on G3DPLL */
    GPU_PLL_REG(REG_GPUPLL_CTRL0) = VAL_GPUPLL_POWER_ON;
    udelay(1);

    /* Enable GPU by disabling clock gating */
    CLKGEN_REG(REG_CKG_GPU) &= ~MSK_DISABLE_CLK;
    udelay(1);

    /* GPU soft reset */
    GPU_REG(REG_GPU_SOFT_RESET) &= ~MSK_GPU_SOFT_RESET;
    GPU_REG(REG_GPU_SOFT_RESET) |= MSK_GPU_SOFT_RESET;
    udelay(1); /* delay for runtime suspend */
    GPU_REG(REG_GPU_SOFT_RESET) &= ~MSK_GPU_SOFT_RESET;
    udelay(10);
}

static void power_off(void)
{
    /* Disable GPU by enabling clock gating */
    CLKGEN_REG(REG_CKG_GPU) |= MSK_DISABLE_CLK;
    udelay(1);
}

#if defined(CONFIG_MALI_DVFS)
static void dvfs_init(void)
{
    u32 i;

    mali_clock_items.num_of_steps = (GPU_MAX_CLOCK - GPU_MIN_CLOCK) / GPU_CLOCK_STEP + 1;
    mali_clock_items.item = _mali_osk_valloc(sizeof(struct mali_gpu_clk_item) * mali_clock_items.num_of_steps);

    if (NULL == mali_clock_items.item)
    {
        mali_clock_items.num_of_steps = 0;
        return;
    }

    for (i = 0; i < mali_clock_items.num_of_steps; ++i)
    {
        mali_clock_items.item[i].clock = GPU_MIN_CLOCK + GPU_CLOCK_STEP * i;
        mali_clock_items.item[i].vol = 0;
    }
}

static void dvfs_deinit(void)
{
    if (mali_clock_items.item)
    {
        _mali_osk_vfree(mali_clock_items.item);
        mali_clock_items.item = NULL;
        mali_clock_items.num_of_steps = 0;
    }
}

void adjust_frequency(int frequency)
{
    MALI_DEBUG_ASSERT((frequency >= GPU_MIN_CLOCK) || (frequency <= GPU_MAX_CLOCK));

    mali_gpu_clock = frequency;

    GPU_PLL_REG(REG_GPUPLL_CTRL0) = 0x0080;
    GPU_PLL_REG(REG_GPUPLL_CTRL1) = 0x1000 | ((frequency/GPU_CLOCK_STEP) & MSK_GPU_CLOCK);
    GPU_PLL_REG(REG_GPUPLL_CTRL2) = 0x0001;
    udelay(10);
}
#endif

/* platform functions */
void mstar_platform_init(void)
{
    mali_gpu_clock = GPU_MAX_CLOCK;

#ifdef CONFIG_MALI_DVFS
    dvfs_init();
#endif
    power_on();
}

void mstar_platform_deinit(void)
{
    power_off();
#ifdef CONFIG_MALI_DVFS
    dvfs_deinit();
#endif
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
    /* Pull up GPU soft reset */
    GPU_REG(REG_GPU_SOFT_RESET) |= MSK_GPU_SOFT_RESET;
    udelay(10);

    /* Power down SRAM */
    SRAM_REG(REG_GPU_SRAM) |= MSK_GPU_SRAM_DISABLE;
    udelay(1);

     /* Disable GPU by enabling clock gating */
    CLKGEN_REG(REG_CKG_GPU) |= MSK_DISABLE_CLK;
}

void mstar_platform_runtime_resume(void)
{
    /* Enable GPU by disabling clock gating */
    CLKGEN_REG(REG_CKG_GPU) &= ~MSK_DISABLE_CLK;
    udelay(1);

    /* Power up SRAM */
    SRAM_REG(REG_GPU_SRAM) &= ~MSK_GPU_SRAM_DISABLE;
    udelay(1);

    /* Pull down GPU soft reset */
    GPU_REG(REG_GPU_SOFT_RESET) &= ~MSK_GPU_SOFT_RESET;
    udelay(10);
}
