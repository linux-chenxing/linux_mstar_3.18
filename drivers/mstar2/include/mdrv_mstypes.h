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
/// @file   MsTypes.h
/// @brief  MStar General Data Types
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MSTYPES_H
#define _MSTYPES_H

//=============================================================================
// Includs
//=============================================================================
#include "mdrv_types.h"

#ifdef CONFIG_64BIT
//=============================================================================
// Type and Structure Declaration
//=============================================================================
    /// data type unsigned char, data length 1 byte
    typedef unsigned char               MS_U8;                              // 1 byte
    /// data type unsigned short, data length 2 byte
    typedef unsigned short              MS_U16;                             // 2 bytes
    /// data type unsigned int, data length 4 byte
    typedef unsigned int                MS_U32;                             // 4 bytes
    /// data type unsigned int, data length 8 byte
    typedef unsigned long               MS_U64;                             // 8 bytes
    /// data type signed char, data length 1 byte
    typedef signed char                 MS_S8;                              // 1 byte
    /// data type signed short, data length 2 byte
    typedef signed short                MS_S16;                             // 2 bytes
    /// data type signed int, data length 4 byte
    typedef signed int                  MS_S32;                             // 4 bytes
    /// data type signed int, data length 8 byte
    typedef signed long                 MS_S64;                             // 8 bytes
    /// data type float, data length 4 byte
    typedef float                       MS_FLOAT;                           // 4 bytes
    /// data type pointer content
    typedef size_t                      MS_VIRT;                            // 8 bytes
    /// data type hardware physical address
    typedef size_t                      MS_PHYADDR;                         // 8 bytes
    /// data type size_t
    typedef size_t                      MS_SIZE;                            // 8 bytes
    /// data type 64bit physical address
    typedef MS_U64                      MS_PHY;                             // 8 bytes
	
	    /// print type  MPRI_PHY
    #define                             MPRI_PHY                            "%x"
    /// print type  MPRI_VIRT
    #define                             MPRI_VIRT                           "%tx"

#else
//=============================================================================
// Type and Structure Declaration
//=============================================================================
  /// data type unsigned char, data length 1 byte
    typedef unsigned char               MS_U8;                              // 1 byte
    /// data type unsigned short, data length 2 byte
    typedef unsigned short              MS_U16;                             // 2 bytes
    /// data type unsigned int, data length 4 byte
    typedef unsigned int                MS_U32;                             // 4 bytes
    /// data type unsigned int, data length 8 byte
    typedef unsigned long long          MS_U64;                             // 8 bytes
    /// data type signed char, data length 1 byte
    typedef signed char                 MS_S8;                              // 1 byte
    /// data type signed short, data length 2 byte
    typedef signed short                MS_S16;                             // 2 bytes
    /// data type signed int, data length 4 byte
    typedef signed int                  MS_S32;                             // 4 bytes
    /// data type signed int, data length 8 byte
    typedef signed long long            MS_S64;                             // 8 bytes
    /// data type float, data length 4 byte
    typedef float                       MS_FLOAT;                           // 4 bytes
    /// data type pointer content
    typedef size_t                      MS_VIRT;                            // 8 bytes
    /// data type hardware physical address
    typedef size_t                      MS_PHYADDR;                         // 8 bytes
    /// data type 64bit physical address
    typedef MS_U64                      MS_PHY;                             // 8 bytes
    /// data type size_t
    typedef size_t                      MS_SIZE;                            // 8 bytes

	
    /// print type  MPRI_PHY
    #define                             MPRI_PHY                            "%x"
    /// print type  MPRI_PHY
    #define                             MPRI_VIRT                           "%tx"
#endif

/// data type null pointer
#ifdef NULL
#undef NULL
#endif
#define NULL                        0

#ifdef CONFIG_MP_PURE_SN_32BIT
typedef unsigned int                MS_PHY64;                         // 32bit physical address
#else
typedef	unsigned long long          MS_PHY64;                         // 64bit physical address
#endif

//-------------------------------------------------------------------------------------------------
//  Software Data Type
//-------------------------------------------------------------------------------------------------

/// definition for MS_BOOL
typedef unsigned char	                        MS_BOOL;
/// definition for VOID
typedef void                        VOID;
/// definition for FILEID
typedef MS_S32                      FILEID;

//[TODO] use MS_U8, ... instead
// data type for 8051 code
typedef MS_U16                      WORD;
typedef MS_U8                       BYTE;

#endif // _MSTYPES_H
