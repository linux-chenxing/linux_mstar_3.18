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
/// @file   mdrv_ldm_interface.h
/// @brief  MStar local dimming Interface header file
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_LD_INTERFACE_H
#define _MDRV_LD_INTERFACE_H


#if defined(__cplusplus)
extern "C" {
#endif

//=============================================================================
// Defines & Macros
//=============================================================================


//=============================================================================
// Debug Macros
//=============================================================================

//=============================================================================
// Type and Structure Declaration
//=============================================================================

//=============================================================================
// Variable
//=============================================================================

extern ST_DRV_LD_INFO stDrvLdInfo;
extern ST_DRV_LD_PQ_INFO stDrvLdPQInfo;
extern ST_DRV_LD_MISC_INFO stDrvLdMiscInfo;
extern EN_LD_DEBUG_LEVEL enDbgLevel;
extern ST_DRV_LD_CUS_PATH stCusPath;
extern ST_DRV_LD_CUS_PATH stCusPathBck;

extern MS_U16 *pSPI_blocks;
extern MS_U16 gu16LedWidth;
extern MS_U16 gu16LedHeight;

//=============================================================================
// Function
//=============================================================================


#if defined(__cplusplus)
}
#endif


#endif //_MDRV_LD_INTERFACE_H

