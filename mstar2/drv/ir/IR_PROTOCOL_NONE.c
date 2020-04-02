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

//<MStar Software>

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "IR_PROTOCOL_NONE.h"

//-------------------------------------------------------------------------------------------------
//  Debug Defines
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
BOOL _ParseLeadCodeNone(U8 *pu8StartIndex)
{
    return FALSE;
}

IR_DECODE_STATUS _MDrv_ParseNone(U32 *pu32CustCode, U16 *pu16KeyCode, U8 *pu8State, U8 *pu8Reserved)
{
    return E_IR_DECODE_ERR;
}

DRV_IR_PROTOCOL_TYPE GET_IR_PROTOCOL_ENTRY(E_IR_PROTOCOL_NONE) =
{
    .name               ="IR_PROTOCOL_NONE",
    .etype              =E_IR_PROTOCOL_NONE,
    .findleadcode       =_ParseLeadCodeNone,
    .parseprotocol      =_MDrv_ParseNone,
    .u8LeadCodeMinCount = IR_NULL_LEADCODE_SHOT_COUNT,
    .u32Timeout         = IR_NULL_TIMEOUT,
};

