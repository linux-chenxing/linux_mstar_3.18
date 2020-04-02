///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2006 - 2007 Mstar Semiconductor, Inc.
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
/// @file   mdrv_miomap_io.h
/// @brief  Memory IO Map Driver IO Interface
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_MIOMAP_IO_H_
#define _MDRV_MIOMAP_IO_H_

/* Use 'M' as magic number */
#define MIOMAP_IOC_MAGIC                'I'

#define MIOMAP_IOC_CHIPINFO         _IOWR(MIOMAP_IOC_MAGIC, 0x00, DrvMIOMap_ChipInfo_t)
#define MIOMAP_IOC_SET_MAP          _IOW (MIOMAP_IOC_MAGIC, 0x01, DrvMIOMap_Info_t)
#define MIOMAP_IOC_FLUSHDCACHE      _IOR (MIOMAP_IOC_MAGIC, 0x02, DrvMIOMap_Info_t)
#define MIOMAP_IOC_GET_BLOCK_OFFSET _IOR (MIOMAP_IOC_MAGIC, 0x03, unsigned int)

#endif

