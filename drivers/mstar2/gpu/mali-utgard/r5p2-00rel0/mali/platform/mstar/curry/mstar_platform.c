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

#define REG_GPU_OUTSDNG_READ        0x60
#define MSK_GPU_OUTSDNG_READ        0x0002

#define REG_GPU_RREQ_RD_LEN         0x61
#define MSK_GPU_RREQ_RD_LEN         0x00e0
#define SHIFT_GPU_RREQ_RD_LEN       5
#define VAL_GPU_RREQ_RD_LEN_48      0x0
#define VAL_GPU_RREQ_RD_LEN_56      0x1
#define VAL_GPU_RREQ_RD_LEN_64      0x2
#define VAL_GPU_RREQ_RD_LEN_68      0x3
#define VAL_GPU_RREQ_RD_LEN_72      0x4
#define VAL_GPU_RREQ_RD_LEN_76      0x5
#define VAL_GPU_RREQ_RD_LEN_80      0x6
#define VAL_GPU_RREQ_RD_LEN_84      0x7

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
#define MSK_GPU_64B_WFUL_ON         0x0002

#define REG_FIFO_DEPTH              0x61
#define SHIFT_FIFO_DEPTH            0x6
#define MSK_FIFO_DEPTH              (0x3 << SHIFT_FIFO_DEPTH)
#define SHIFT_TIMEOUT_FLUSH         0x8
#define MSK_TIMEOUT_FLUSH           (0xff << SHIFT_TIMEOUT_FLUSH)
#define TIME_OUT_FLUSH_1T           0x1

#define REG_GPU_MIU0_TLB_START      0x7c
#define REG_GPU_MIU0_TLB_END        0x7d
#define REG_GPU_MIU1_TLB_START      0x7e
#define REG_GPU_MIU1_TLB_END        0x7f

#define MIU0_PHY_BASE_ADDR_HIGH     0x0000
#define MIU0_PHY_END_ADDR_HIGH      MIU1_PHY_BASE_ADDR_HIGH
#define MIU1_PHY_END_ADDR_HIGH      0x0fff

#define TLB_MIU0_START_ADDR         0xA0000000UL
#define TLB_MIU0_START_ADDR_H       ((TLB_MIU0_START_ADDR >> 20) & 0xFFFF)
#define TLB_MIU0_SIZE               0x20000000UL
#define TLB_MIU0_END_ADDR           (TLB_MIU0_START_ADDR + TLB_MIU0_SIZE)
#define TLB_MIU0_END_ADDR_H         ((TLB_MIU0_END_ADDR >> 20) & 0xFFFF)

/* REG_GPU_PLL */
#define REG_GPU_PLL_BASE            0x160400

#define REG_G3DPLL                  0x11
#define MSK_G3DPLL                  0xff00

#define REG_SET_0                   0x60
#define REG_SET_1                   0x61

#define REG_SLD                     0x62
#define MSK_SLD                     0x0001

/* REG_CLKGEN2 */
#define REG_CLKGEN2_BASE            0x100a00

#define REG_CKG_GPU                 0x3d
#define MSK_DISABLE_CLK             0x0001

/* REG_ANA_MISC_MPLLTOP */
#define REG_ANA_MISC_MPLLTOP_BASE   0x110B00
#define REG_MPLL                    0x01
#define MSK_MPLL                    0xff00

/* RIU mapping */
#define GPU_REG(addr)               RIU[(REG_GPU_BASE) + ((addr) << 1)]
#define GPU_PLL_REG(addr)           RIU[(REG_GPU_PLL_BASE) + ((addr) << 1)]
#define CLKGEN2_REG(addr)           RIU[(REG_CLKGEN2_BASE) + ((addr) << 1)]
#define MPLL_REG(addr)              RIU[(REG_ANA_MISC_MPLLTOP_BASE) + ((addr) << 1)]

#define MHZ_TO_REG(MHZ)             (0x6c000000 / MHZ)

/* helper functions */
static void power_on(void)
{
    unsigned int clk_reg = MHZ_TO_REG(GPU_MAX_CLOCK);

    /* GPU clock */
    GPU_PLL_REG(REG_SET_0) = clk_reg & 0xffff;
    GPU_PLL_REG(REG_SET_1) = (clk_reg >> 16) & 0xffff;
    GPU_PLL_REG(REG_SLD) |= MSK_SLD;
    udelay(10);

    /* Power on MPLL */
    MPLL_REG(REG_MPLL) &= ~MSK_MPLL;
    udelay(1);

    /* Set MIU0 base and end address */
    GPU_REG(REG_GPU_MIU0_START)  = MIU0_PHY_BASE_ADDR_HIGH;
    GPU_REG(REG_GPU_MIU0_END)  = MIU0_PHY_END_ADDR_HIGH;
    udelay(1);

    /* Set MIU1 base and end address */
    /* Curry do not support MIU1*/
    /* This setting is to make sure that Mali will not out of range */
    GPU_REG(REG_GPU_MIU1_START) = MIU1_PHY_BASE_ADDR_HIGH;
    GPU_REG(REG_GPU_MIU1_END) = MIU1_PHY_END_ADDR_HIGH;
    udelay(1);

    /* Set MIU TLB addr */
    GPU_REG(REG_GPU_MIU0_TLB_START) = TLB_MIU0_START_ADDR_H;
    GPU_REG(REG_GPU_MIU0_TLB_END)   = TLB_MIU0_END_ADDR_H;

    /* Enable RIU */
#ifdef MSTAR_RIU_ENABLED
    GPU_REG(REG_GPU_RIU_EN) |= MSK_GPU_RIU_EN;
#endif

    /* Set read request length to 64 */
    GPU_REG(REG_GPU_RREQ_RD_LEN) &= ~MSK_GPU_RREQ_RD_LEN;
    GPU_REG(REG_GPU_RREQ_RD_LEN) |= (VAL_GPU_RREQ_RD_LEN_64 << SHIFT_GPU_RREQ_RD_LEN);

    /* Disable TLB and disable 64-bit full write func */
    GPU_REG(REG_GPU_TLB_ENABLE) &= ~(MSK_GPU_TLB_ENABLE | MSK_GPU_64B_WFUL_ON);

    /* Set request flush timeout to 1 to increase performance */
    GPU_REG(REG_FIFO_DEPTH) = (GPU_REG(REG_FIFO_DEPTH) & ~MSK_TIMEOUT_FLUSH) |
                              ((TIME_OUT_FLUSH_1T << SHIFT_TIMEOUT_FLUSH) & MSK_TIMEOUT_FLUSH);

    /* Power on G3DPLL */
    GPU_PLL_REG(REG_G3DPLL) &= ~MSK_G3DPLL;
    udelay(1);

    /* Enable GPU (by disabling clock gating) */
    CLKGEN2_REG(REG_CKG_GPU) &= ~MSK_DISABLE_CLK;
    udelay(1);

    /* Soft reset GPU */
    GPU_REG(REG_GPU_SOFT_RESET) &= ~MSK_GPU_SOFT_RESET;
    GPU_REG(REG_GPU_SOFT_RESET) |= MSK_GPU_SOFT_RESET;
    udelay(1); /* delay for runtime suspend */
    GPU_REG(REG_GPU_SOFT_RESET) &= ~MSK_GPU_SOFT_RESET;
    udelay(1);
}

static void power_off(void)
{
    /* Disable GPU (by enabling clock gating) */
    CLKGEN2_REG(REG_CKG_GPU) |= MSK_DISABLE_CLK;
    udelay(1);
}

#if defined(CONFIG_MALI_DVFS)
static void dvfs_init(void)
{
#define GPU_CLOCK_STEP 50

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
    unsigned int clk_reg = MHZ_TO_REG(frequency);

    MALI_DEBUG_ASSERT((frequency >= GPU_MIN_CLOCK) || (frequency <= GPU_MAX_CLOCK));

    mali_gpu_clock = frequency;

    GPU_PLL_REG(REG_SET_0) = clk_reg & 0xffff;
    GPU_PLL_REG(REG_SET_1) = (clk_reg >> 16) & 0xffff;
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
    power_off();
}

void mstar_platform_runtime_resume(void)
{
    power_on();
}
