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
/// @file   mdrv_utopia2k_str_io.h
/// @brief  UTOPIA2K STR interface
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DRV_UTOPIA2K_STR_IO_H__
#define __DRV_UTOPIA2K_STR_IO_H__

////////////////////////////////////////////////////////////////////////////////
// Header Files
////////////////////////////////////////////////////////////////////////////////

#include "MsTypes.h"

////////////////////////////////////////////////////////////////////////////////
// Define & data type
////////////////////////////////////////////////////////////////////////////////

enum utopia2k_str_power_mode {
    UTOPIA2K_STR_POWER_SUSPEND = 1,
    UTOPIA2K_STR_POWER_RESUME = 2,
    UTOPIA2K_STR_POWER_MAX,
};

typedef int (*FUtopiaSTR)(int u32PowerState, void* pModule);

////////////////////////////////////////////////////////////////////////////////
// Extern Function
////////////////////////////////////////////////////////////////////////////////

int mdrv_utopia2k_str_setup_function_ptr(void* pModuleTmp, FUtopiaSTR fpSTR);
int mdrv_utopia2k_str_wait_condition(const char* name, MS_U32 mode, MS_U32 stage);
int mdrv_utopia2k_str_send_condition(const char* name, MS_U32 mode, MS_U32 stage);

#endif

