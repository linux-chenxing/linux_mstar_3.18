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
/// @file   mhal_mbx_interrupt_reg.h
/// @brief  MStar Mailbox Driver DDI
/// @author MStar Semiconductor Inc.
/// @attention
/// <b>(OBSOLETED) <em>legacy interface is only used by MStar proprietary Mail Message communication\n
/// It's API level for backward compatible and will be remove in the next version.\n
/// Please refer @ref drvGE.h for future compatibility.</em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MHAL_MBX_INTERRUPT_REG_H
#define _MHAL_MBX_INTERRUPT_REG_H

//=============================================================================
// Includs
//=============================================================================
#include "mdrv_types.h"

extern ptrdiff_t mstar_pm_base;

//=============================================================================
// Defines & Macros
//=============================================================================
//#define REG_INT_BASE                     (0xBF203200)
#define INT_REG(address)                 (*((volatile MS_U16 *)(REG_INT_BASE + ((address)<<2) )))

#define INTNO_BASE_32                       32
#define INTNO_BASE_47                       47
#define INTNO_PM_ARM                        38
#define INTNO_ARM_PM                        42
#define INTNO_R2_ARM                        48



// interrupt index address
#define REG_INT_FIQMASK_PM                  0x06UL
#define REG_INT_FIQMASK_ARM                 0x26UL
#define REG_INT_FIQMASK_ARM_H               0x27UL


#define REG_INT_FIQCLEAR_PM                 0x0EUL
#define REG_INT_FIQCLEAR_ARM                0x2EUL

#define REG_INT_FIQFINALSTATUS_PM           0x0EUL
#define REG_INT_FIQFINALSTATUS_ARM          0x2EUL

    #define _BIT_PM_ARM              (1<<(INTNO_PM_ARM-INTNO_BASE_32))
    #define _BIT_ARM_PM              (1<<(INTNO_ARM_PM-INTNO_BASE_32))
	#define _BIT_R2_ARM              (1<<(INTNO_R2_ARM-INTNO_BASE_47))


#define REG_CPU_INT_BASE               (mstar_pm_base + (0x100540UL<<1))
#define CPU_INT_REG(address)             (*((volatile MS_U16 *)(REG_CPU_INT_BASE + ((address)<<2) )))
#define REG_INT_PMFIRE                   0x0000UL
    #define INT_PM_ARM                   BIT0

#define REG_INT_ARMFIRE                  0x0002UL
    #define INT_ARM_PM                   BIT0
    #define INT_ARM_Core1                BIT1
    #define INT_ARM_R2                   BIT2
	


#endif //_MHAL_MBX_INTERRUPT_REG_H

