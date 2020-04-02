///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2008 - 2009 MStar Semiconductor, Inc.
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
/// @file   mdrv_gflip_interrupt.c
/// @brief  MStar gflip Interrupt Interface
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em>Define the interface which corespond to sysem's interrupt notification</em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _MDRV_GFLIP_INTERRUPT_C

//=============================================================================
// Include Files
//=============================================================================
#include <linux/interrupt.h>

#include "mdrv_mstypes.h"
#include "mdrv_gflip.h"
#include "chip_int.h"
#include "mdrv_gflip_interrupt.h"

//=============================================================================
// Compile options
//=============================================================================


//=============================================================================
// Local Defines
//=============================================================================

//=============================================================================
// Debug Macros
//=============================================================================
#define GFLIPINT_DEBUG
#ifdef GFLIPINT_DEBUG
    #define GFLIPINT_PRINT(fmt, args...)      printk("[GFlip (Interrupter)][%05d] " fmt, __LINE__, ## args)
    #define GFLIPINT_ASSERT(_cnd, _fmt, _args...)                   \
                                    if (!(_cnd)) {              \
                                        GFLIPINT_PRINT(_fmt, ##_args);  \
                                    }
#else
    #define GFLIPINT_PRINT(_fmt, _args...)
    #define GFLIPINT_ASSERT(_cnd, _fmt, _args...)
#endif

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

//=============================================================================
// GFLIP DRV Interrupt Driver Function
//=============================================================================

//-------------------------------------------------------------------------------------------------
/// Handle GFLIP Interrupt notification handler
/// @param  irq                  \b IN: interrupt number
/// @param  devid                  \b IN: device id
/// @return IRQ_HANDLED
/// @attention
/// <b>[OBAMA] <em></em></b>
//-------------------------------------------------------------------------------------------------
irqreturn_t MDrv_GFLIPINT_IntHandler(int irq,void *devid)
{
    //process gflip interrupt:
    MDrv_GFLIP_ProcessIRQ();

    //clear interrupt
    MDrv_GFLIP_ClearIRQ();

    return IRQ_HANDLED;
}

#if	( defined (CONFIG_MSTAR_VE_CAPTURE_SUPPORT))
//-------------------------------------------------------------------------------------------------
/// Handle GFLIP VEC Interrupt notification handler
/// @param  irq                  \b IN: interrupt number
/// @param  devid                  \b IN: device id
/// @return IRQ_HANDLED
/// @attention
/// <b>[OBAMA] <em></em></b>
//-------------------------------------------------------------------------------------------------
irqreturn_t MDrv_GFLIP_VECINT_IntHandler(int irq,void *devid)
{
    //process gflip_vec interrupt:
    MDrv_GFLIP_VEC_ProcessIRQ();
    //clear interrupt
    MDrv_GFLIP_VEC_ClearIRQ();
    return IRQ_HANDLED;
}
#endif //CONFIG_MSTAR_VE_CAPTURE_SUPPORT
