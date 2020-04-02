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
/// @file   mhal_mbx_reg.h
/// @brief  MStar Mailbox Driver DDI
/// @author MStar Semiconductor Inc.
/// @attention
/// <b>(OBSOLETED) <em>legacy interface is only used by MStar proprietary Mail Message communication\n
/// It's API level for backward compatible and will be remove in the next version.\n
/// Please refer @ref drvGE.h for future compatibility.</em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef _MHAL_MBX_REG_H
#define _MHAL_MBX_REG_H

//=============================================================================
// Includs
//=============================================================================
#include "mdrv_types.h"

//=============================================================================
// Defines & Macros
//=============================================================================

#if defined(CONFIG_ARM64)
extern ptrdiff_t mstar_pm_base;
#define RIU_MAP (mstar_pm_base+0x200000UL)
#else
#define RIU_MAP 0xFD200000UL
#endif

#define RIU8    ((unsigned char  volatile *) RIU_MAP)

#define REG_MBX_BASE                (0x19C0UL)
#define MBX_REG8(gid, addr)         RIU8[((gid+REG_MBX_BASE)<<2) + ((addr) * 2) - ((addr) & 1)]

//Reg8 defines:
#define REG8_MBX_CTRL               0x0000UL
    #define MBX_CTRL_FIRE             BIT(0)
    #define MBX_CTRL_READBACK         BIT(1)
    #define MBX_CTRL_INSTANT          BIT(2)
#define REG8_MBX_MAIL_CLASS          0x0001UL
#define REG8_MBX_MAIL_IDX            0x0002UL
#define REG8_MBX_PARAMETER_CNT       0x0003UL
#define REG8_MBX_PARAMETER_S         0x0004UL
#define REG8_MBX_PARAMETER_E         0x000DUL
#define REG8_MBX_STATE_0              0x000EUL
#define REG8_MBX_STATE_1              0x000FUL
    #define MBX_STATE1_DISABLED          BIT(4)
    #define MBX_STATE1_OVERFLOW           BIT(5)
    #define MBX_STATE1_ERROR              BIT(6)
    #define MBX_STATE1_BUSY               BIT(7)

#define REG_MBX_GROUP0      0x00UL
#define REG_MBX_GROUP1      0x08UL
#define REG_MBX_GROUP2      0x10UL
#define REG_MBX_GROUP3      0x18UL
#define REG_MBX_GROUP4      0x20UL
#define REG_MBX_GROUP5      0x28UL

#define REG8_MBX_GROUP(gid, addr) MBX_REG8(gid, addr)

#define REG8_MBX_MIPS(addr)        MBX_REG8(REG_MBX_GROUP1, addr) //group0 allocated for mips
#define REG8_MBX_AEON(addr)        MBX_REG8(REG_MBX_GROUP0, addr) //group1 allocated for aeon
#define REG8_MBX_PM(addr)          MBX_REG8(REG_MBX_GROUP3, addr) //group3 allocated for pm

//  for MDrv_PM_Get_BrickTerminator_Info  (pm bank dummy register)
// 0x0E h0053 BIT[15:8]
#if defined(CONFIG_ARM64)
#define RIU_MAP_MBX (mstar_pm_base)
#else
#define RIU_MAP_MBX     0xFD000000UL
#endif
#define REG_PM_BASE   (0x0EA7UL)
#define MHAL_PM_REG()	((volatile MS_U8*)(RIU_MAP_MBX))[(REG_PM_BASE << 1) - (REG_PM_BASE & 1)]


#endif //__MHAL_MBX_REG_H

