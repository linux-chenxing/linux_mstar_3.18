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
/// @file   mdrv_ldm_interrupt.c
/// @brief  MStar localdimming Interrupt Interface
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em>Define the interface which corespond to sysem's interrupt notification</em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////
#if (defined (CONFIG_HAS_LD) || defined (CONFIG_HAS_LD_MODULE)|| defined (CONFIG_HAS_LD_DMA_MODE))

#define _MDRV_LDM_INTERRUPT_C

//=============================================================================
// Include Files
//=============================================================================
#include <linux/interrupt.h>

#include "mdrv_mstypes.h"
#include "mdrv_ldm_interrupt.h"

//=============================================================================
// Compile options
//=============================================================================


//=============================================================================
// Local Defines
//=============================================================================

//=============================================================================
// Debug Macros
//=============================================================================

//=============================================================================
// Macros
//=============================================================================

//=============================================================================
// Local Variables
//=============================================================================

//=============================================================================
// Global Variables
//=============================================================================

//=============================================================================
// Local Function Prototypes
//=============================================================================

//=============================================================================
// Local Function
//=============================================================================

extern MS_BOOL MDrv_LD_ProcessIRQ(void);
//-------------------------------------------------------------------------------------------------
/// Handle local dimming Interrupt notification handler
/// @param  irq                  \b IN: interrupt number
/// @param  devid                  \b IN: device id
/// @return IRQ_HANDLED
/// @attention
/// <b>[OBAMA] <em></em></b>
//-------------------------------------------------------------------------------------------------
irqreturn_t MDrv_LD_IntHandler(int irq,void *devid)
{
    MDrv_LD_ProcessIRQ();

    return IRQ_HANDLED;
}

#endif
