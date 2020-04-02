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

#ifndef _BD_GENERIC_
#define _BD_GENERIC_

#define BOARD_NAME                  "BD_GENERIC"

//------Peripheral Device Setting----------------------------
#if defined(CONFIG_ENABLE_CUSTOMER_IR_PATH)

/*
1. Use CONFIG_ENABLE_CUSTOMER_IR_PATH_STRING to decide IR header file in mdrv_ir.c
2. Move IR_TYPE_SEL into IR header files.
3. default header file is IR_MSTAR_DTV.h
*/

#else

#ifndef IR_TYPE_SEL
#define IR_TYPE_SEL                 IR_TYPE_MSTAR_DTV
#endif

#endif /* CONFIG_ENABLE_CUSTOMER_IR_PATH */

//------MCU use Scaler internal MPLL clock-------------------
#define XTAL_CLOCK_FREQ             12000000    //12 MHz
#define MCU_XTAL_CLK_HZ             (XTAL_CLOCK_FREQ*MCU_PLL_MUL) // Hz
#define MCU_XTAL_CLK_KHZ            (MCU_XTAL_CLK_HZ / 1000)
#define MCU_XTAL_CLK_MHZ            (MCU_XTAL_CLK_KHZ / 1000)

#if defined(CONFIG_MSTAR_ARM_MMAP_128MB_128MB)
#include "mmap/mmap_128mb_128mb.h"
#elif defined(CONFIG_MSTAR_ARM_MMAP_256MB_256MB)
#include "mmap/mmap_256mb_256mb.h"
#elif defined(CONFIG_MSTAR_MMAP_128MB_128MB_DEFAULT)
#include "mmap/mmap_128mb_128mb_default.h"
#elif defined(CONFIG_MSTAR_ARM_MMAP_GENERIC)
#include "mmap/mmap_generic.h"
#endif

#endif
