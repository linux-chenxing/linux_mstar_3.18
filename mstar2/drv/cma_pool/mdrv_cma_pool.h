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
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   cma_mpool_manager.h
/// @brief  CMA mpool Manager interface
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DRV_MCMA_H__
#define __DRV_MCMA_H__
#include "mdrv_cma_pool_io.h"
#include "mdrv_cma_pool_st.h"

#define size_1M (1<<20)

#ifdef CONFIG_MSTAR_MIUSLITS
#define SLIT_NUM   (256)
#define CMA_HEAP_LENGTH_ALIGNMENT ((pageblock_nr_pages)*(PAGE_SIZE))
#endif

#define MIU_BLOCK_NUM 4

#ifdef CONFIG_MP_CMA_PATCH_MBOOT_STR_USE_CMA
void str_reserve_mboot_cma_buffer(void);
void str_release_mboot_cma_buffer(void);
#endif

typedef enum
{
    MIU_BLOCK_IDLE = 0,
    MIU_BLOCK_BUSY
}MIU_PROTECT_BLOCK_STATUS;

struct cma_buffer{
    bool freed;
    int mapped;
    void *cpu_addr;
    struct page * page;
    pid_t pid;
    //bus address
    unsigned long start_pa;
    unsigned long length;
    struct list_head list;
};

struct cma_allocation_list{
    unsigned long min_start;
    unsigned long max_end;
    unsigned long using_count;

    unsigned long freed_count;
    struct list_head list_head;
};

struct MIU_ProtectRanges
{
    unsigned char miu;
    MIU_PROTECT_BLOCK_STATUS miuBlockStatus[MIU_BLOCK_NUM];
    
    unsigned int krange_num;
    struct list_head list_head;
    struct mutex lock;
};

typedef struct
{
    unsigned char miuBlockIndex;
    unsigned long start_pa;
    unsigned long length;
}MIU_ProtectRange_Record;

typedef struct
{
    MIU_ProtectRange_Record old;
    MIU_ProtectRange_Record new;
}MIU_ProtectRange_pair;

struct cma_memory_info{
    char miu;
    unsigned int heap_id;
    struct device *dev;
    struct cma_allocation_list alloc_list;    
    struct MIU_ProtectRanges * pranges;

    struct list_head list_node;
    struct mutex lock;
};

#ifdef  CONFIG_MP_CMA_PATCH_POOL_UTOPIA_TO_KERNEL
enum type_states
{
    TYPE_STATES_USER = (1<<0),//for user
    TYPE_STATES_KERNEL= (1<<1),//for kernel 
    TYPE_STATES_NUM,
};
enum type_kermode_flag
{
    KERNEL_MODE_CMA_POOL_USER_SPACE_MAP_CACHE = (1<<1),    

};
struct kernel_cma_pool_vma_item
{
	struct list_head list_node;//node for vma_list
	pid_t pid;
	unsigned long flags;//
	struct vm_area_struct *vma;//for kernel mode user space va use.
};
#endif

struct heap_proc_info
{
    unsigned int heap_id;
    unsigned long base_phy;    
    struct vm_area_struct *vma;
    bool vma_cached;
    struct list_head list_node;
#ifdef  CONFIG_MP_CMA_PATCH_POOL_UTOPIA_TO_KERNEL
    struct vm_struct *kernel_vm;//"vm_struct" for kernel,can only set value for once.
    enum type_states  type_states;//user utopia map or kernel utopia map
    struct list_head vma_list;
	struct mutex vma_list_lock;
#endif    
};

typedef struct
{
    unsigned char miuBlockIndex;
    unsigned long start_pa;
    unsigned long length;
    struct list_head list_node;
}MIU_ProtectRange;

typedef struct
{
    pid_t pid;
    
    unsigned int temp_heap_id;
    
    struct list_head list_heap_proc_info;
    struct mutex lock;
}filp_private;

typedef struct
{
    unsigned char miu;
    int krange_change_num;
    MIU_ProtectRange_pair pair[KERN_CHUNK_NUM];
}MIU_KERN_PRange_Info_user;
#endif

