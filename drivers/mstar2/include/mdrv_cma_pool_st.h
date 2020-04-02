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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   cma_mpool_manager.h
/// @brief  CMA mpool Manager interface
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DRV_CMA_POOL_ST_H__
#define __DRV_CMA_POOL_ST_H__
#define KERN_CHUNK_NUM 3

#define CMA_FLAG_MAP_VMA    (1<<0)
#define CMA_FLAG_CACHED      (1<<1)
#define CMA_FLAG_VIRT_ADDR   (1<<2)
#define CMA_FLAG_MTLB_POOL  (1<<3)

#ifdef  CONFIG_MP_CMA_PATCH_POOL_UTOPIA_TO_KERNEL
#define CMA_FLAG_MAP_KERNEL (1<<4)//Be notice This CMA_FLAG_MAP_KERNEL only kernel utopia use,user utopia not use!
#endif

#define CMA_FLAG_KERNEL_MODE_USER_SPACE_MAP_CACHED (1<<5)//for cma kernel mode map use va use:whether cache or un-cache
#define CMA_FLAG_CACHEATTR_VALID (1<<6)

enum USER_VA_VALID_FLAG
{
    USER_VA_VALID = (1<<0),
    NO_HEAP = (1<<1),
    NO_USER_VA = (1<<2),
    DIFFERENT_CACHE_TYPE = (1<<3),
};

#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
#ifdef CONFIG_MP_NEW_UTOPIA_32BIT
/* this is for 32-bit kernel and new_utopia(utopia2) */
struct cma_alloc_args {
    u64 offset_in_heap;
    unsigned long cpu_addr;
    unsigned long length;
    u64 align;
    unsigned int heap_id;
    unsigned int flags;
};

struct cma_free_args {
    unsigned int heap_id;
    u64 offset_in_heap;
    unsigned long length;
};

struct cma_heap_info {
    unsigned int heap_id;
    unsigned long flags;

    unsigned int miu;
    u64 bus_addr;
    u64 heap_miu_start_offset;
    unsigned long heap_length;
#ifdef  CONFIG_MP_CMA_PATCH_POOL_UTOPIA_TO_KERNEL
    size_t/*MS_VIRT*/ virt_addr;//out:if need,shoud give this out put
#endif    
};


struct get_info_from_pa
{
    u64 PA;//in:phy addr that input

    unsigned int heap_id;//out:in which heap
    unsigned int miu;//out:in which miu
    u64 heap_miu_start_offset;//out:heap start offset in miu
    unsigned long heap_length;//out:heap length
    u64 pa_offset_in_heap;//out :pa offset in heap
};

struct cma_heap_get_user_va
{
    unsigned int heap_id;//in:in which heap
    unsigned int flags;//in:flags,such as cache or uncache

    enum USER_VA_VALID_FLAG user_va_valid_flag;//out:whether vma is valid
    size_t heap_start_user_space_virt_addr;//out:if vma is valid ,get this user space virt addr of heap start.
};

struct cma_mmap_user_va_page 
{
    unsigned int heap_id;//in: heap id
    unsigned int flags;//in:flags,such as cache or uncache
};


#else
/* this is for 32-bit kernel and old_utopia(utopia2) */
struct cma_alloc_args {
    unsigned long offset_in_heap;
    unsigned long cpu_addr;
    unsigned long length;        
    unsigned long align;
    unsigned int heap_id;
    unsigned int flags;
};

struct cma_free_args {
    unsigned int heap_id;
    unsigned long offset_in_heap;
    unsigned long length;
};

struct cma_heap_info {
    unsigned int heap_id;
    unsigned long flags;

    unsigned int miu;
    unsigned long bus_addr;
    unsigned long heap_miu_start_offset;
    unsigned long heap_length;
#ifdef  CONFIG_MP_CMA_PATCH_POOL_UTOPIA_TO_KERNEL
    size_t/*MS_VIRT*/ virt_addr;//out:if need,shoud give this out put
#endif    
};


struct get_info_from_pa
{
    unsigned long PA;//in:phy addr that input

    unsigned int heap_id;//out:in which heap
    unsigned int miu;//out:in which miu
    unsigned long heap_miu_start_offset;//out:heap start offset in miu
    unsigned long heap_length;//out:heap length
    unsigned long pa_offset_in_heap;//out :pa offset in heap
};

struct cma_heap_get_user_va
{
    unsigned int heap_id;//in:in which heap
    unsigned int flags;//in:flags,such as cache or uncache

    enum USER_VA_VALID_FLAG user_va_valid_flag;//out:whether vma is valid
    size_t heap_start_user_space_virt_addr;//out:if vma is valid and alloced,get this user space virt addr.
};

struct cma_mmap_user_va_page 
{
    unsigned int heap_id;//in: heap id
    unsigned int flags;//in:
};

#endif
#elif defined(CONFIG_ARM64)
/* this is for 64-bit kernel and new_utopia(utopia_muji) */
#if defined(CONFIG_COMPAT)
/* compat size will be as 32-bit utopia */
struct compat_cma_heap_info {
    compat_uint_t heap_id;
    compat_ulong_t flags;

    compat_uint_t miu;
    compat_u64 bus_addr;
    compat_u64 heap_miu_start_offset;
    compat_size_t heap_length;
};
struct compat_cma_alloc_args {
    compat_u64 offset_in_heap;
    compat_ulong_t cpu_addr;
    compat_size_t length;        
    compat_u64 align;
	compat_uint_t heap_id;
    compat_uint_t flags;
};

struct compat_cma_free_args {
    compat_uint_t heap_id;
    compat_u64 offset_in_heap;
    compat_size_t length;
};

struct compat_get_info_from_pa
{
    compat_u64 PA;//in:phy addr that input

    compat_uint_t heap_id;//out:in which heap
    compat_uint_t miu;//out:in which miu
    compat_u64 heap_miu_start_offset;//out:heap start offset in miu
    compat_size_t heap_length;//out:heap length
    compat_u64 pa_offset_in_heap;//out :pa offset in heap

};
struct compat_cma_heap_get_user_va
{
    compat_uint_t heap_id;//in:in which heap
    compat_uint_t flags;//in:flags,such as cache or uncache

    enum USER_VA_VALID_FLAG user_va_valid_flag;//out:whether vma is valid
    compat_size_t heap_start_user_space_virt_addr;//out:if vma is valid and alloced,get this user space virt addr.

};
struct compat_cma_mmap_user_va_page
{
    compat_uint_t heap_id;//in: heap id
    compat_uint_t flags;//in:flags,such as cache or uncache

};

#endif

/* this is 64-bit data type, which will be like 64-bit utopia */
struct cma_heap_info {
    u32 heap_id;
    u64 flags;

    u32 miu;
    u64 bus_addr;
    u64 heap_miu_start_offset;
    size_t heap_length;
#ifdef  CONFIG_MP_CMA_PATCH_POOL_UTOPIA_TO_KERNEL
    size_t/*MS_VIRT*/ virt_addr;//out:if need,shoud give this out put
#endif
};

struct cma_alloc_args {
    u64 offset_in_heap;
    u64 cpu_addr;
    size_t length;        
    u64 align;
	u32 heap_id;
    u32 flags;
};

struct cma_free_args {
    u32 heap_id;
    u64 offset_in_heap;
    size_t length;
};

struct get_info_from_pa
{
    u64 PA;//in:phy addr that input

    u32 heap_id;//out:in which heap
    u32 miu;//out:in which miu
    u64 heap_miu_start_offset;//out:heap start offset in miu
    u64 heap_length;//out:heap length
    u64 pa_offset_in_heap;//out :pa offset in heap
};


struct cma_heap_get_user_va
{
    u32 heap_id;//in:in which heap
    unsigned int flags;//in:flags,such as cache or uncache

    enum USER_VA_VALID_FLAG user_va_valid_flag;//out:whether vma is valid
    size_t heap_start_user_space_virt_addr;//out:if vma is valid and alloced,get this user space virt addr.
};

struct cma_mmap_user_va_page 
{
    unsigned int heap_id;//in
    unsigned int flags;//in
};

#endif // CONFIG_ARM
#endif

