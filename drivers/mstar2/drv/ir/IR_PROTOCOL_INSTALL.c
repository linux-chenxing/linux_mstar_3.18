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
#include "IR_PROTOCOL_INSTALL.h"
#ifdef SUPPORT_MULTI_PROTOCOL
#include "IR_PROTOCOL_NONE.c"
#include "IR_PROTOCOL_NEC.c"
#include "IR_PROTOCOL_RC5.c"
#include "IR_PROTOCOL_OCN.c"
#include "IR_PROTOCOL_RC6.c"

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
DRV_IR_PROTOCOL_TYPE* _GetProtocolEntry(IR_PROCOCOL_TYPE eProtocol)
{
    extern DRV_IR_PROTOCOL_TYPE GET_IR_PROTOCOL_ENTRY(E_IR_PROTOCOL_NONE);
    extern DRV_IR_PROTOCOL_TYPE GET_IR_PROTOCOL_ENTRY(E_IR_PROTOCOL_RC5);
    extern DRV_IR_PROTOCOL_TYPE GET_IR_PROTOCOL_ENTRY(E_IR_PROTOCOL_NEC);
    extern DRV_IR_PROTOCOL_TYPE GET_IR_PROTOCOL_ENTRY(E_IR_PROTOCOL_PZ_OCN);
    extern DRV_IR_PROTOCOL_TYPE GET_IR_PROTOCOL_ENTRY(E_IR_PROTOCOL_RC6);

    switch(eProtocol)
    {
        case E_IR_PROTOCOL_NONE:
            return &GET_IR_PROTOCOL_ENTRY(E_IR_PROTOCOL_NONE);
            break;
        case E_IR_PROTOCOL_NEC:
            return &GET_IR_PROTOCOL_ENTRY(E_IR_PROTOCOL_NEC);
            break;
        case E_IR_PROTOCOL_RC5:
            return &GET_IR_PROTOCOL_ENTRY(E_IR_PROTOCOL_RC5);
            break;
        case E_IR_PROTOCOL_PZ_OCN:
            return &GET_IR_PROTOCOL_ENTRY(E_IR_PROTOCOL_PZ_OCN);
            break;
        case E_IR_PROTOCOL_RC6:
            return &GET_IR_PROTOCOL_ENTRY(E_IR_PROTOCOL_RC6);
            break;

        default:
            printk("not support protocol\n");
    }

    return &GET_IR_PROTOCOL_ENTRY(E_IR_PROTOCOL_NONE);
}
#endif
