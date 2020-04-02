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

#include <linux/ump.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

#include "mstar/mstar_chip.h"
#define PHYS_TO_BUS_ADDRESS_ADJUST  MSTAR_MIU0_BUS_BASE

/* */
struct pa_wrapping_info
{
    int num_phys_blocks;
};

static int import_pa_client_create(void** const custom_session_data)
{
    uint32_t** dummy;

    dummy = (uint32_t**)custom_session_data;
    *dummy = (uint32_t*)1;  /* return anything other than 0 */

    return 0;
}

static void import_pa_client_destroy(void* custom_session_data)
{
    uint32_t* dummy;

    dummy = (uint32_t*)custom_session_data;
    BUG_ON(!dummy);
}

static void import_pa_final_release_callback(const ump_dd_handle handle, void* info)
{
    struct pa_wrapping_info * pa_info;

    BUG_ON(!info);

    (void)handle;

    pa_info = (struct pa_wrapping_info*)info;
    kfree(pa_info);
    module_put(THIS_MODULE);
}

static ump_dd_handle import_pa_import(void* custom_session_data, void* pfd, size_t size, ump_alloc_flags flags)
{
    ump_dd_handle ump_handle;
    ump_dd_physical_block_64* phys_block;
    struct pa_wrapping_info* pa_info;

    BUG_ON(!custom_session_data);
    BUG_ON(!pfd);

    pa_info = kzalloc(sizeof(*pa_info), GFP_KERNEL);

    if (NULL == pa_info)
    {
        return UMP_DD_INVALID_MEMORY_HANDLE;
    }

    phys_block = vmalloc(sizeof(*phys_block));

    if (NULL == phys_block)
    {
        goto out;
    }

    if (0 == (flags & UMP_CONSTRAINT_BUS_ADDRESS))
    {
        /*
         * we adjust the physical address to the bus address because it will be adjusted again when pages are insterted into MMU page tables.
         */
        pfd += PHYS_TO_BUS_ADDRESS_ADJUST;
    }

    phys_block->addr = (uint64_t)(uintptr_t)pfd;
    phys_block->size = size;

    ump_handle = ump_dd_create_from_phys_blocks_64(phys_block, 1, flags, NULL, import_pa_final_release_callback, pa_info);

    vfree(phys_block);

    if (ump_handle != UMP_DD_INVALID_MEMORY_HANDLE)
    {
        /*
         * As we have a final release callback installed
         * we must keep the module locked until
         * the callback has been triggered
         * */
        __module_get(THIS_MODULE);
        return ump_handle;
    }

    /* failed*/
out:
    kfree(pa_info);
    return UMP_DD_INVALID_MEMORY_HANDLE;
}

struct ump_import_handler import_handler_pa =
{
    .linux_module   = THIS_MODULE,
    .session_begin  = import_pa_client_create,
    .session_end    = import_pa_client_destroy,
    .import         = import_pa_import
};

#if MSTAR_UMP_MONOLITHIC
int __init import_pa_initialize_module(void)
#else
static int __init import_pa_initialize_module(void)
#endif
{
    /* register with UMP */
    return ump_import_module_register(UMP_EXTERNAL_MEM_TYPE_PA, &import_handler_pa);
}

#if MSTAR_UMP_MONOLITHIC
void __exit import_pa_cleanup_module(void)
#else
static void __exit import_pa_cleanup_module(void)
#endif
{
    /* unregister import handler */
    ump_import_module_unregister(UMP_EXTERNAL_MEM_TYPE_PA);
}

/* Setup init and exit functions for this module */
#if !MSTAR_UMP_MONOLITHIC
module_init(import_pa_initialize_module);
module_exit(import_pa_cleanup_module);
#endif

/* And some module information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("MStar Semiconductor, Inc.");
MODULE_VERSION("1.0");
