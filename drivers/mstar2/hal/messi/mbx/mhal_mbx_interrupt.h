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
/// @file   mhal_mbx_interrupt.h
/// @brief  MStar Mailbox Interrupt HAL DDI
/// @author MStar Semiconductor Inc.
/// @attention
/// <b>(OBSOLETED) <em>legacy interface is only used by MStar proprietary Mail Message communication\n
/// It's API level for backward compatible and will be remove in the next version.\n
/// Please refer @ref drvGE.h for future compatibility.</em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MHAL_MBX_INTERRUPT_H
#define _MHAL_MBX_INTERRUPT_H

#ifdef _MHAL_MBX_INTERRUPT_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

//=============================================================================
// Includs
//=============================================================================
#include "mhal_mbx_interrupt_reg.h"
#include "chip_int.h"

//=============================================================================
// Defines & Macros
//=============================================================================
#define E_FIQ_INT_AEON_TO_8051  (0xFF-0) 
#define E_FIQ_INT_MIPS_TO_8051   E_FIQEXPL_ARM_TO_8051 //VALID //E_FIQ_INT_ARM_TO_8051
#define E_FIQ_INT_8051_TO_AEON  (0xFF-1)
#define E_FIQ_INT_MIPS_TO_AEON   (0xFF-2)
#define E_FIQ_INT_8051_TO_MIPS   E_FIQEXPL_8051_TO_ARM //VALID //E_FIQ_INT_8051_TO_ARM
#define E_FIQ_INT_AEON_TO_MIPS   (0xFF-3)
#define E_FIQ_INT_FRCR2_TO_MIPS   (0xFF-4)//frcr2_integration###

#define MBX_INT_AEON2PM     E_FIQ_INT_AEON_TO_8051
#define MBX_INT_PM2AEON     E_FIQ_INT_8051_TO_AEON
#define MBX_INT_PM2MIPS     (E_FIQ_INT_8051_TO_MIPS-E_FIQEXPL_START)
#define MBX_INT_MIPS2PM     (E_FIQ_INT_MIPS_TO_8051-E_FIQEXPL_START)
#define MBX_INT_MIPS2AEON   E_FIQ_INT_MIPS_TO_AEON
#define MBX_INT_AEON2MIPS   E_FIQ_INT_AEON_TO_MIPS
#define MBX_INT_R2MIPS     (E_FIQEXPL_SECURE_R2_TO_ARM-E_FIQEXPH_START)
#define MBX_INT_FRC2MIPS   E_FIQ_INT_FRCR2_TO_MIPS //frcr2_integration###

//=============================================================================
// Type and Structure Declaration
//=============================================================================
typedef void (*MBX_MSGRECV_CB_FUNC)(MS_S32 s32Irq);

//=============================================================================
// Function
//=============================================================================
INTERFACE MBX_Result MHAL_MBXINT_Init (MBX_CPU_ID eHostCPU, MBX_MSGRECV_CB_FUNC pMBXRecvMsgCBFunc);
INTERFACE void MHAL_MBXINT_DeInit (MBX_CPU_ID eHostCPU);
INTERFACE MBX_Result MHAL_MBXINT_ResetHostCPU (MBX_CPU_ID ePrevCPU, MBX_CPU_ID eConfigCpu);
INTERFACE MBX_Result MHAL_MBXINT_Enable (MS_S32 s32Fiq, MS_BOOL bEnable);
INTERFACE MBX_Result MHAL_MBXINT_Fire (MBX_CPU_ID eDstCPUID, MBX_CPU_ID eSrcCPUID);
INTERFACE MBX_Result MHAL_MBXINT_Clear (MS_S32 s32Fiq);
INTERFACE MBX_Result MHAL_MBXINT_Suspend(MBX_CPU_ID eHostCPU);
INTERFACE MBX_Result MHAL_MBXINT_Resume(MBX_CPU_ID eHostCPU);
#undef INTERFACE

#endif //_MHAL_MBX_INTERRUPT_H

