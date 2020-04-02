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
/// @file   mdrv_mpool_io.h
/// @brief  Memory Pool  Driver IO Interface
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_MPOOL_IO_H_
#define _MDRV_MPOOL_IO_H_

/* Use 'M' as magic number */
#define MPOOL_IOC_MAGIC                'M'

#define MPOOL_IOC_INFO           _IOWR(MPOOL_IOC_MAGIC, 0x00, DrvMPool_Info_t)
#define MPOOL_IOC_FLUSHDCACHE    _IOR(MPOOL_IOC_MAGIC,0x01,DrvMPool_Info_t)
#define MPOOL_IOC_GET_BLOCK_OFFSET    _IOR(MPOOL_IOC_MAGIC,0x02,unsigned int)
#define MPOOL_IOC_SET_MAP_CACHE _IOR(MPOOL_IOC_MAGIC,0x03,unsigned int)
#define MPOOL_IOC_TEST           _IOWR(MPOOL_IOC_MAGIC, 0x06, De_Sys_Info_t)
#define MPOOL_IOC_SET _IOR(MPOOL_IOC_MAGIC,0x04,DrvMPool_Info_t)
#define MPOOL_IOC_KERNEL_DETECT _IOWR(MPOOL_IOC_MAGIC,0x05,DrvMPool_Info_t)
#define MPOOL_IOC_FLUSHDCACHE_PAVA    _IOR(MPOOL_IOC_MAGIC,0x07,DrvMPool_Flush_Info_t)
#define MPOOL_IOC_VERSION _IOWR(MPOOL_IOC_MAGIC,0x08,unsigned int)
#define MPOOL_IOC_FLUSHDCACHE_ALL  _IOR(MPOOL_IOC_MAGIC,0x09,DrvMPool_Flush_Info_t)
#define MPOOL_IOC_SETWATCHPT       _IOW(MPOOL_IOC_MAGIC,0x0A,DrvMPool_Watchpt_Info_t)
#define MPOOL_IOC_GETWATCHPT       _IOR(MPOOL_IOC_MAGIC,0x0B,char*)
#define MPOOL_IOC_BA2PA            _IOR(MPOOL_IOC_MAGIC,0x0C,MS_PHY64)
#define MPOOL_IOC_PA2BA            _IOR(MPOOL_IOC_MAGIC,0x0D,MS_PHY64)
#define MPOOL_IOC_DLMALLOC_CREATE_POOL	_IOWR(MPOOL_IOC_MAGIC, 0x20, DrvMPool_Dlmalloc_Info_t)
#define MPOOL_IOC_DLMALLOC_DELETE_POOL	_IOWR(MPOOL_IOC_MAGIC, 0x21, DrvMPool_Dlmalloc_Delete_Info_t)
#define MPOOL_IOC_DLMALLOC_ALLOC_POOL_MEMORY	_IOWR(MPOOL_IOC_MAGIC, 0x22, DrvMPool_Dlmalloc_Alloc_Free_Info_t)
#define MPOOL_IOC_DLMALLOC_FREE_POOL_MEMORY   _IOWR(MPOOL_IOC_MAGIC, 0x23, DrvMPool_Dlmalloc_Alloc_Free_Info_t)
#define MPOOL_IOC_DLMALLOC_REALLOC_POOL_MEMORY   _IOWR(MPOOL_IOC_MAGIC, 0x24, DrvMPool_Dlmalloc_Alloc_Free_Info_t)

#if defined(CONFIG_COMPAT)
#define COMPAT_MPOOL_IOC_FLUSHDCACHE_PAVA    _IOR(MPOOL_IOC_MAGIC,0x07,DrvMPool_Flush_Info_t32)
#define COMPAT_MPOOL_IOC_SETWATCHPT       _IOW(MPOOL_IOC_MAGIC,0x0A,DrvMPool_Watchpt_Info_t32)
#define COMPAT_MPOOL_IOC_DLMALLOC_CREATE_POOL    _IOWR(MPOOL_IOC_MAGIC, 0x20, DrvMPool_Dlmalloc_Info_t32)
#define COMPAT_MPOOL_IOC_DLMALLOC_ALLOC_POOL_MEMORY    _IOWR(MPOOL_IOC_MAGIC, 0x22, DrvMPool_Dlmalloc_Alloc_Free_Info_t32)
#define COMPAT_MPOOL_IOC_DLMALLOC_FREE_POOL_MEMORY    _IOWR(MPOOL_IOC_MAGIC, 0x23, DrvMPool_Dlmalloc_Alloc_Free_Info_t32)
#define COMPAT_MPOOL_IOC_DLMALLOC_REALLOC_POOL_MEMORY    _IOWR(MPOOL_IOC_MAGIC, 0x24, DrvMPool_Dlmalloc_Alloc_Free_Info_t32)
#endif

#endif

