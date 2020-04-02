///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2008 - 2009 Mstar Semiconductor, Inc.
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
/// @file   mdrv_ldm_interrupt.h
/// @brief  MStar local dimming Interrupt Interface header file
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em>Define the interface which corespond to sysem's interrupt notification</em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_LDM_INTERRUPT_H
#define _MDRV_LDM_INTERRUPT_H

#ifdef _MDRV_LDM_INTERRUPT_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

#if defined(__cplusplus)
extern "C" {
#endif

//=============================================================================
// Defines & Macros
//=============================================================================

//=============================================================================
// Type and Structure Declaration
//=============================================================================

//=============================================================================
// Function
//=============================================================================
INTERFACE irqreturn_t MDrv_LD_IntHandler(int irq,void *devid);

#if defined(__cplusplus)
}
#endif

#undef INTERFACE

#endif //_MDRV_LDM_INTERRUPT_H

