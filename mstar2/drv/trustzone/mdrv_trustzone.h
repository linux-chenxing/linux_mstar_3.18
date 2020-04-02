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
/// @file   mdrv_trustzone.h
/// @brief  MALLOC Driver Interface
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DRV_TRUSTZONE_H_
#define _DRV_TRUSTZONE_H_

#include "mdrv_trustzone_io.h"
#include "mdrv_trustzone_st.h"
//#include "mdrv_smc.h"

//-------------------------------------------------------------------------------------------------
//  Driver Capability
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
typedef enum {
      TZ_LLV_RES_OK                      //= 0
     ,TZ_LLV_RES_ERR                     //= 1
     ,TZ_LLV_RES_UND                     //= 2
     ,TZ_LLV_RES_MAX
}TZ_LLV_Result;

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
struct tz_struct{

    volatile unsigned int cmd1;
    volatile unsigned int cmd2;

    volatile unsigned int* private_data;

};

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
//void smc_call(struct tz_struct *tz);
int tz_call(struct tz_struct *tz);
#endif // _DRV_TRUSTZONE_H_

