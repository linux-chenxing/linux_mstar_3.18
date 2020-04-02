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

#ifndef IR_PROTOCOL_RC5_H
#define IR_PROTOCOL_RC5_H

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define IR_RC5_BITS                          (14)

#define IR_RC5_LEADCODE_SHOT_COUNT         (2)

#define IR_TIME_RC5_LEADCODE_TOLERENCE     (0.1) //shot time tolerence for lead code, should be more strict
#define IR_TIME_RC5_TOLERENCE               (0.15) //shot time tolerence


#define IR_TIME_RC5_FIRST_SHOT              (25000)    // us

#define IR_TIME_RC5_HALF_BIT                (889)    // us
#define IR_TIME_RC5_ONE_BIT                 (1772)    // us

#define IR_TIME_RC5_TIMEOUT                 (114000)
//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
//BOOL _ParseLeadCodeRc5(U8 *pu8StartIndex);
//IR_DECODE_STATUS _MDrv_ParseRC5(unsigned long long *pu64RawData);

#endif

