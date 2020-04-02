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

#ifndef _REG_PM_H_
#define _REG_PM_H_

#include <mstar/mstar_chip.h>
#include "mdrv_types.h" 


#if defined(CONFIG_ARM) || defined(CONFIG_MIPS) 
#define PM_RIU_REG_BASE                 0xFD000000 
#elif defined(CONFIG_ARM64) 
extern ptrdiff_t mstar_pm_base;
#define PM_RIU_REG_BASE                 mstar_pm_base 
#endif

#define REG_PM_SLEEP_BASE               (0x0e00UL) 
#define REG_PM_MISC_BASE                (0x2e00UL) 
#define REG_MCU_BASE                    (0x1000UL)
#define REG_MCU_CACHE_BASE              (0x2b00UL)

//------------------------------------------------------------------------------ 
// pm sleep Reg 
//------------------------------------------------------------------------------
#define REG_PM_MCU_CLK                  (REG_PM_SLEEP_BASE + (0x20UL << 1))
#define REG_PM_LOCK                     (REG_PM_SLEEP_BASE + (0x12UL << 1))



//------------------------------------------------------------------------------ 
// pm misc Reg 
//------------------------------------------------------------------------------
#define REG_PM_CPU_SW_RST               (REG_PM_MISC_BASE + (0x29UL << 1))
#define REG_PM_RST_CPU0_PASSWORD        (REG_PM_MISC_BASE + (0x2aUL << 1))


//------------------------------------------------------------------------------ 
// mcu Reg 
//------------------------------------------------------------------------------
#define REG_MCU_SRAM_START_ADDR_H        (REG_MCU_BASE + (0x00UL << 1))
#define REG_MCU_SRAM_END_ADDR_H          (REG_MCU_BASE + (0x01UL << 1))
#define REG_MCU_SRAM_START_ADDR_L        (REG_MCU_BASE + (0x02UL << 1))
#define REG_MCU_SRAM_END_ADDR_L          (REG_MCU_BASE + (0x03UL << 1))

#define REG_MCU_CONFIG                   (REG_MCU_BASE + (0x0cUL << 1))

//------------------------------------------------------------------------------ 
// 8051 cache Reg 
//------------------------------------------------------------------------------
#define REG_MCU_CACHE_CONFIG             (REG_MCU_BASE + (0x50UL << 1))

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------


#endif // _REG_PM_H_

