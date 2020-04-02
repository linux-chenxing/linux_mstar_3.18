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
/// @file   mdrv_iic.h
/// @brief  IIC Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <asm/types.h>//<asm-mips/types.h>
#include "mdrv_types.h"
#if defined(CONFIG_COMPAT)
#include <linux/compat.h>
#endif

//-------------------------------------------------------------------------------------------------
//  Driver Capability
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

struct IIC_Param
{
    U8 u8IdIIC;      	/// IIC ID: Channel 1~7
    U8 u8ClockIIC;   	/// IIC clock speed
    U8 u8SlaveIdIIC;    /// Device slave ID
    U8 u8AddrSizeIIC;	/// Address length in bytes
    U8 u8AddrIIC[4];	/// Starting address inside the device
    U8 *u8pbufIIC;     	/// buffer
    U32 u32DataSizeIIC;	/// size of buffer

} __attribute__ ((packed));

#if defined(CONFIG_COMPAT)
struct IIC_Param_Compat
{
    U8 u8IdIIC;      	/// IIC ID: Channel 1~7
    U8 u8ClockIIC;   	/// IIC clock speed
    U8 u8SlaveIdIIC;    /// Device slave ID
    U8 u8AddrSizeIIC;	/// Address length in bytes
    U8 u8AddrIIC[4];	/// Starting address inside the device
    compat_uptr_t u8pbufIIC;     	/// buffer //(4) not using pointer
    U32 u32DataSizeIIC;	/// size of buffer

} __attribute__ ((packed));

typedef struct IIC_Param_Compat  IIC_Param_Compat;
#endif
typedef struct IIC_Param  IIC_Param;

typedef struct IIC_Param IIC_Param_t;

struct IIC_BusCfg
{
    U8 u8ChIdx;         ///Channel index
    U16 u16PadSCL;      ///Pad(Gpio) number for SCL
    U16 u16PadSDA;      ///Pad(Gpio) number for SDA
    U16 u16SpeedKHz;    ///Speed in KHz
    U8 u8Enable;        ///Enable
} __attribute__ ((packed));

typedef struct IIC_BusCfg IIC_BusCfg_t;

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define IIC_IOC_MAGIC               'u'

#if defined(CONFIG_COMPAT)
#define Compat_MDRV_IIC_INIT               _IO(IIC_IOC_MAGIC, 0)
#define Compat_MDRV_IIC_CLOCK              _IOW(IIC_IOC_MAGIC, 2, IIC_Param_Compat)
#define Compat_MDRV_IIC_ENABLE             _IOW(IIC_IOC_MAGIC, 3, IIC_Param_Compat) 
#define Compat_MDRV_IIC_BUSCFG             _IOW(IIC_IOC_MAGIC, 4, IIC_BusCfg_t)
#endif

#define MDRV_IIC_INIT               _IO(IIC_IOC_MAGIC, 0)
#define MDRV_IIC_SET_PARAM          _IOW(IIC_IOC_MAGIC, 1, IIC_Param_t)
#define MDRV_IIC_CLOCK              _IOW(IIC_IOC_MAGIC, 2, IIC_Param_t)
#define MDRV_IIC_ENABLE             _IOW(IIC_IOC_MAGIC, 3, IIC_Param_t) 
#define MDRV_IIC_BUSCFG             _IOW(IIC_IOC_MAGIC, 4, IIC_BusCfg_t)
#define IIC_IOC_MAXNR               5

#define IIC_RW_BUF_SIZE             1024
#define IIC_WR_BUF_SIZE             128		// added for RGB EDID
#define IIC_RD_BUF_SIZE             256		// added for RGB EDID

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

