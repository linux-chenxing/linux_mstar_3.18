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
/// @file   mdrv_gpio_io.h
/// @brief  GPIO Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <asm/types.h>
#include "mdrv_types.h"

//-------------------------------------------------------------------------------------------------
//  Driver Capability
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
struct GPIO_Reg
{
    U32 u32Reg;
    U8 u8Enable;
    U8 u8BitMsk;
} __attribute__ ((packed));

typedef struct GPIO_Reg GPIO_Reg_t;

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define GPIO_IOC_MAGIC               'g'

#define MDRV_GPIO_INIT               _IO(GPIO_IOC_MAGIC, 0)
#define MDRV_GPIO_SET                _IOW(GPIO_IOC_MAGIC, 1, U8)
#define MDRV_GPIO_OEN                _IOW(GPIO_IOC_MAGIC, 2, U8)
#define MDRV_GPIO_ODN                _IOW(GPIO_IOC_MAGIC, 3, U8)
#define MDRV_GPIO_READ               _IOWR(GPIO_IOC_MAGIC, 4, U8)
#define MDRV_GPIO_PULL_HIGH          _IOW(GPIO_IOC_MAGIC, 5, U8)
#define MDRV_GPIO_PULL_LOW           _IOW(GPIO_IOC_MAGIC, 6, U8)
#define MDRV_GPIO_INOUT              _IOWR(GPIO_IOC_MAGIC, 7, U8)
#define MDRV_GPIO_WREGB              _IOW(GPIO_IOC_MAGIC, 8, GPIO_Reg_t)
#define KERN_GPIO_OEN                _IOW(GPIO_IOC_MAGIC, 9, U8)
#define KERN_GPIO_PULL_HIGH          _IOW(GPIO_IOC_MAGIC, 10, U8)
#define KERN_GPIO_PULL_LOW           _IOW(GPIO_IOC_MAGIC, 11, U8)

#define GPIO_IOC_MAXNR               12

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

void __mod_gpio_init(void);

