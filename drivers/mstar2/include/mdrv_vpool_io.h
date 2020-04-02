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

#ifndef _MDRV_VPOOL_IO_H_
#define _MDRV_VPOOL_IO_H_

/* Use 'M' as magic number */
#define VPOOL_IOC_MAGIC                'V'

#define VPOOL_IOC_FLUSH_INV_DCACHE    _IOR(VPOOL_IOC_MAGIC,0x01,DrvVPool_Info_t)

typedef struct
{
    void*                         pAddr;
    unsigned int                u32Size;
} DrvVPool_Info_t;


#endif

