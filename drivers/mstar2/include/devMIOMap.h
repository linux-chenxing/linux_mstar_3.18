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
/// @file   drvMIOMap.h
/// @brief  MIOMAP Driver Interface
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DRV_MIOMAP_H_
#define _DRV_MIOMAP_H_


//-------------------------------------------------------------------------------------------------
//  Driver Capability
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
/* Use 'I' as magic number */
#define MIOMAP_IOC_MAGIC                'I'

#define MIOMAP_IOC_INFO             _IOWR(MIOMAP_IOC_MAGIC, 0x00, DrvMIOMap_Info_t)
#define MIOMAP_IOC_SET_MAP          _IOW (MIOMAP_IOC_MAGIC, 0x01, DrvMIOMap_Info_t)

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
// MIOMAP_IOC_INFO 
#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
#ifdef CONFIG_MP_NEW_UTOPIA_32BIT
typedef struct
{
    u64                u32Addr;
    u64                u32Size;
} DrvMIOMap_Info_t;
#else
typedef struct
{
    unsigned int                u32Addr;
    unsigned int                u32Size;
} DrvMIOMap_Info_t;
#endif //CONFIG_MP_NEW_UTOPIA_32BIT
#elif defined(CONFIG_ARM64)
typedef struct
{
    u64                u32Addr;
    u64                u32Size;
} DrvMIOMap_Info_t;
#endif

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------


#endif // _DRV_MIOMAP_H_
