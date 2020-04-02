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
#define REG_SPLIT_2CH_MD            0x60
#define READ_OUTSTANDING            0x0002
#define REG_FIFO_DEPTH              0x61
#define SHIFT_FIFO_DEPTH            0x6
#define MSK_FIFO_DEPTH              (0x3 << SHIFT_FIFO_DEPTH)
#define REG_G3D_RREQ                0x62
#define REG_G3D_RREQ_THRD           0x000f
#define REG_G3D_WREQ                0x63
#define REG_G3D_WREQ_THRD           0x000f
#define REG_RIU_APB_EN              0x6a
#define MSK_RIU_APB_EN              0x0001
#define REG_MIU1_BASE_LOW           0x77
#define REG_MIU1_BASE_HIGH          0x78

#define REG_G3DPLL_BASE             0x160f00
#define REG_GPUPLL_CTRL0_LOW        0x02
#define REG_GPUPLL_CTRL0_HIGH       0x03
#define REG_GPUPLL_CTRL1_LOW        0x04
#define REG_GPUPLL_CTRL1_HIGH       0x05

#define REG_CLKGEN1_BASE            0x103300
#define REG_CKG_GPU                 0x20
#define DISABLE_CLK_GPU             0x0001

/* RIU mapping */
#define G3D_REG(addr)               RIU[REG_G3D_BASE + ((addr) << 1)]
#define G3DPLL_REG(addr)            RIU[REG_G3DPLL_BASE + ((addr) << 1)]
#define CLKGEN1_REG(addr)           RIU[REG_CLKGEN1_BASE + ((addr) << 1)]

/* helper functions */
static void power_on(void)
{
    /* set GPU clock: must before power on */
    G3DPLL_REG(REG_GPUPLL_CTRL0_LOW)  = 0x0000;
    G3DPLL_REG(REG_GPUPLL_CTRL0_HIGH) = 0x0000;
    G3DPLL_REG(REG_GPUPLL_CTRL1_LOW)  = GPU_MAX_CLOCK/12;
    G3DPLL_REG(REG_GPUPLL_CTRL1_HIGH) = 0x0001;
    udelay(100);

    /* enable read by outstanding order */
    G3D_REG(REG_SPLIT_2CH_MD) |= READ_OUTSTANDING;

    /* reg_g3d_rreq_thrd = 0x0 */
    G3D_REG(REG_G3D_RREQ) &= ~REG_G3D_RREQ_THRD;

    /* reg_g3d_wreq_thrd = 0x0 */
    G3D_REG(REG_G3D_WREQ) &= ~REG_G3D_WREQ_THRD;

    /* Set MIU1 base address */
    G3D_REG(REG_MIU1_BASE_LOW)  = MIU1_PHY_BASE_ADDR_LOW;
    G3D_REG(REG_MIU1_BASE_HIGH) = MIU1_PHY_BASE_ADDR_HIGH;
    udelay(100);

    /* enable RIU access */
#ifdef MSTAR_RIU_ENABLED
    G3D_REG(REG_RIU_APB_EN) |= MSK_RIU_APB_EN;
    udelay(100);
#endif

    /* For improve performance, set FIFO depth to 48                   */
    /* Actually, inside the hardware design, the FIFO depth is only 32 */
    G3D_REG(REG_FIFO_DEPTH) = (G3D_REG(REG_FIFO_DEPTH) & ~MSK_FIFO_DEPTH) | ((0x2 << SHIFT_FIFO_DEPTH) & MSK_FIFO_DEPTH);
    udelay(100);

    /* disable GPU clock gating */
    CLKGEN1_REG(REG_CKG_GPU) &= ~DISABLE_CLK_GPU;
    udelay(100);

    /* reset mali */
    G3D_REG(REG_SOFT_RESET) &= ~MSK_SOFT_RESET;
    G3D_REG(REG_SOFT_RESET) |= MSK_SOFT_RESET;
    udelay(100); /* delay for run-time suspend */
    G3D_REG(REG_SOFT_RESET) &= ~MSK_SOFT_RESET;
    udelay(100);
}

static void power_off(void)
{
    /* enable GPU clock gating */
    CLKGEN1_REG(REG_CKG_GPU) |= DISABLE_CLK_GPU;
    udelay(100);
}

/* platform functions */
void mstar_platform_init(void)
{
    mali_gpu_clock = GPU_MAX_CLOCK/12*12;
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
}
