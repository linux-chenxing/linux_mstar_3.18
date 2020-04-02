/*
 *
 * (C) COPYRIGHT 2014-2015 MStar Semiconductor, Inc. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * A copy of the licence is included with the program, and can also be obtained
 * from Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 */

#include <mali_kbase.h>
#include <mali_kbase_config.h>
#include "mali_kbase_platform_mstar.h"

#include <chip_int.h>

#define MALI_BASE_ADDRESS (MSTAR_RIU_BASE + MALI_REG_OFFSET)

#ifdef MSTAR_RIU_ENABLED
#define mstar_riu_offset(offset) ((((offset) & 0xFFFF0000) << 1) | ((offset) & 0x0000FFFF))
#endif

/* platform configs */
#ifndef CONFIG_OF
static struct kbase_io_resources io_resources =
{
    .job_irq_number = MALI_IRQ,
    .mmu_irq_number = MALI_IRQ,
    .gpu_irq_number = MALI_IRQ,
    .io_memory_region =
    {
        /* io_memory_region is assigned values in kbase_get_platform_config */
        .start  = 0,
        .end    = 0
    }
};
#endif

struct kbase_platform_funcs_conf platform_funcs =
{
    .platform_init_func = mstar_platform_init,
    .platform_term_func = mstar_platform_term,
};

struct kbase_pm_callback_conf pm_callbacks =
{
    .power_off_callback             = mstar_pm_off,
    .power_on_callback              = mstar_pm_on,
    .power_suspend_callback         = mstar_pm_suspend,
    .power_resume_callback          = mstar_pm_resume,
    .power_runtime_init_callback    = NULL,
    .power_runtime_term_callback    = NULL,
    .power_runtime_off_callback     = NULL,
    .power_runtime_on_callback      = NULL,
};

static struct kbase_platform_config platform_config =
{
#ifndef CONFIG_OF
    .io_resources = &io_resources
#endif
};

/* platform functions */
struct kbase_platform_config *kbase_get_platform_config(void)
{
    /* MALI_BASE_ADDRESS is not a constant if CONFIG_ARM64 is defined so io_resources is assigned values here */
#ifndef CONFIG_OF
    struct kbase_io_resources* io_resources = (struct kbase_io_resources*)platform_config.io_resources;

    io_resources->io_memory_region.start = MALI_BASE_ADDRESS;
#ifdef MSTAR_RIU_ENABLED
    io_resources->io_memory_region.end = MALI_BASE_ADDRESS + mstar_riu_offset((4096 * 4) - 1);
#else
    io_resources->io_memory_region.end = MALI_BASE_ADDRESS + (4096 * 4) - 1;
#endif
#endif

    return &platform_config;
}

int kbase_platform_early_init(void)
{
    /* Nothing needed at this stage */
    return 0;
}
