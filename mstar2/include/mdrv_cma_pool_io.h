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

#ifndef __DRV_CMA_POOL_IO_H__
#define __DRV_CMA_POOL_IO_H__
#include "mdrv_cma_pool_st.h"

//-------------------------------------------------------------------------------------------------
//  IO command
//-------------------------------------------------------------------------------------------------
#define CMA_POOL_IOC_MAGIC   'P'

#define CMA_POOL_IOC_INIT   _IOWR(CMA_POOL_IOC_MAGIC, 0x00, struct cma_heap_info)
#define COMPAT_CMA_POOL_IOC_INIT   _IOWR(CMA_POOL_IOC_MAGIC, 0x00, struct compat_cma_heap_info)
#define CMA_POOL_IOC_ALLOC  _IOWR(CMA_POOL_IOC_MAGIC, 0x02, struct cma_alloc_args)
#define COMPAT_CMA_POOL_IOC_ALLOC  _IOWR(CMA_POOL_IOC_MAGIC, 0x02, struct compat_cma_alloc_args)
#define CMA_POOL_IOC_FREE   _IOWR(CMA_POOL_IOC_MAGIC, 0x03, struct cma_free_args)
#define COMPAT_CMA_POOL_IOC_FREE   _IOWR(CMA_POOL_IOC_MAGIC, 0x03, struct compat_cma_free_args)
#define CMA_POOL_IOC_RELEASE   _IOW(CMA_POOL_IOC_MAGIC, 0x04, unsigned int)
#define CMA_POOL_IOC_GET_HEAP_INFO_FROM_PA   _IOWR(CMA_POOL_IOC_MAGIC, 0x05, struct get_info_from_pa)
#define COMPAT_CMA_POOL_IOC_GET_HEAP_INFO_FROM_PA   _IOWR(CMA_POOL_IOC_MAGIC, 0x05, struct compat_get_info_from_pa)
#define CMA_POOL_IOC_KERNEL_MODE_GET_USER_VA   _IOWR(CMA_POOL_IOC_MAGIC, 0x06, struct cma_heap_get_user_va)
#define COMPAT_CMA_POOL_IOC_KERNEL_MODE_GET_USER_VA   _IOWR(CMA_POOL_IOC_MAGIC, 0x06, struct compat_cma_heap_get_user_va)
#define CMA_POOL_IOC_KERNEL_MODE_MAPPING_USER_VA_PAGE  _IOWR(CMA_POOL_IOC_MAGIC, 0x07, struct cma_mmap_user_va_page)
#define COMPAT_CMA_POOL_IOC_KERNEL_MODE_MAPPING_USER_VA_PAGE  _IOWR(CMA_POOL_IOC_MAGIC, 0x07, struct compat_cma_mmap_user_va_page)
#endif

