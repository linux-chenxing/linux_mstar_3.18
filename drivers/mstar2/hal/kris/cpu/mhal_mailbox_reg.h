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
/// file    regMailBox.h
/// @brief  MailBox Registers Definition
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MHAL_MAILBOX_REG_H_
#define _MHAL_MAILBOX_REG_H_


//-------------------------------------------------------------------------------------------------
//  Hardware Capability
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
//Total Mailbox buffer 96 Bytes

//TBD: Refine it
#if 1
#define MB_SIZE_8051_TO_AEON    16
#define MB_SIZE_AEON_TO_8051    16
#define MB_SIZE_AEON_TO_MIPS    16
#define MB_SIZE_MIPS_TO_AEON    16
#define MB_SIZE_MIPS_TO_8051    16
#define MB_SIZE_8051_TO_MIPS    16
#endif

typedef enum
{
    //IRQ
    E_MB_START                  = 0,
    E_MB_8051_TO_AEON           = E_MB_START+0,
    E_MB_AEON_TO_8051           = E_MB_8051_TO_AEON + MB_SIZE_8051_TO_AEON,
    E_MB_AEON_TO_MIPS           = E_MB_AEON_TO_8051 + MB_SIZE_AEON_TO_8051,
    E_MB_MIPS_TO_AEON           = E_MB_AEON_TO_MIPS + MB_SIZE_AEON_TO_MIPS,
    E_MB_MIPS_TO_8051           = E_MB_MIPS_TO_AEON + MB_SIZE_MIPS_TO_AEON,
    E_MB_8051_TO_MIPS           = E_MB_MIPS_TO_8051 + MB_SIZE_MIPS_TO_8051,
    E_MB_END                    = E_MB_8051_TO_MIPS + MB_SIZE_8051_TO_MIPS
} MailBoxOffset;

// @FIXME: correct the register
#define MAILBOX_REG_BASE                0xBF206700  /*RT:0x19c0*/

#define REG_MB_8051_TO_AEON     (MAILBOX_REG_BASE + (E_MB_8051_TO_AEON*2))
#define REG_MB_AEON_TO_8051     (MAILBOX_REG_BASE + (E_MB_AEON_TO_8051*2))
#define REG_MB_AEON_TO_MIPS     (MAILBOX_REG_BASE + (E_MB_AEON_TO_MIPS*2))
#define REG_MB_MIPS_TO_AEON     (MAILBOX_REG_BASE + (E_MB_MIPS_TO_AEON*2))
#define REG_MB_MIPS_TO_8051     (MAILBOX_REG_BASE + (E_MB_MIPS_TO_8051*2))
#define REG_MB_8051_TO_MIPS     (MAILBOX_REG_BASE + (E_MB_8051_TO_MIPS*2))


//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

#endif // _MHAL_MAILBOX_REG_H_

