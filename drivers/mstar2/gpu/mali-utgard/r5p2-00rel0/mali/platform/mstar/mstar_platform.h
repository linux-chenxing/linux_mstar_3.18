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
 * @file mstar_platform.h
 * MStar platform specific driver functions
 */

#include <linux/types.h>
#include "mstar/mstar_chip.h"
#if defined(CONFIG_MALI_DVFS)
#include <linux/mali/mali_utgard.h>
#endif

/* RIU */
#ifdef CONFIG_ARM64
extern ptrdiff_t mstar_pm_base;
#define MSTAR_RIU_BASE mstar_pm_base
#else
#define MSTAR_RIU_BASE 0xfd000000
#endif

#define RIU ((volatile unsigned short*)MSTAR_RIU_BASE)

/* MIU */
#ifndef MSTAR_MIU1_PHY_BASE
#ifdef ARM_MIU1_BASE_ADDR
#define MSTAR_MIU1_PHY_BASE ARM_MIU1_BASE_ADDR
#else
#define MSTAR_MIU1_PHY_BASE 0x80000000UL
#endif
#endif

#if (MSTAR_MIU_BOUNDARY_ADDR_SETTING == 1)
/* Setting MIU start/end address to support TLB address mapping in case TLB region is between MIU0 and MIU1
 * The lower 20 bits are always zero and no longer need to be set in register.
 */
#define MIU1_PHY_BASE_ADDR_HIGH ((MSTAR_MIU1_PHY_BASE >> 20) & 0xffff)
#else
#define MIU1_PHY_BASE_ADDR_LOW  ( MSTAR_MIU1_PHY_BASE        & 0xffff)
#define MIU1_PHY_BASE_ADDR_HIGH ((MSTAR_MIU1_PHY_BASE >> 16) & 0xffff)
#endif

#define PHYS_TO_BUS_ADDRESS_ADJUST MSTAR_MIU0_BUS_BASE
#define PHYS_TO_BUS_ADDRESS_ADJUST_MIU1 (MSTAR_MIU1_BUS_BASE - MSTAR_MIU1_PHY_BASE)

static __inline__ __attribute__ ((always_inline)) void bus_to_phys_address_adjust(dma_addr_t *addr)
{
    if (*addr < MSTAR_MIU1_BUS_BASE)
    {
        *addr -= PHYS_TO_BUS_ADDRESS_ADJUST;
    }
    else
    {
        /* TODO: Fix mapping strategy, CPU will not use bus address >= 0x2_8000_0000 for now */
        *addr -= PHYS_TO_BUS_ADDRESS_ADJUST_MIU1;
    }
}

static __inline__ __attribute__ ((always_inline)) void phys_to_bus_address_adjust(dma_addr_t *addr)
{
    if (*addr < MSTAR_MIU1_PHY_BASE)
    {
        *addr += PHYS_TO_BUS_ADDRESS_ADJUST;
    }
    else
    {
        *addr += PHYS_TO_BUS_ADDRESS_ADJUST_MIU1;
    }
}

#ifdef __cplusplus
extern "C" {
#endif

/* GPU clock */
extern int mali_gpu_clock;
extern struct mali_gpu_clock mali_clock_items;

/* platform functions */
void mstar_platform_init(void);
void mstar_platform_deinit(void);
void mstar_platform_suspend(void);
void mstar_platform_resume(void);
void mstar_platform_runtime_suspend(void);
void mstar_platform_runtime_resume(void);

#if defined(CONFIG_MALI_DVFS)
void adjust_frequency(int frequency);
#endif

#if defined(CONFIG_MALI_EFUSE_PP_CORE_NUM)
int mstar_platform_get_efuse_core_num(void);
#endif

#ifdef __cplusplus
}
#endif
