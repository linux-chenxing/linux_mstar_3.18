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

#ifndef IR_PROTOCOL_NEC_H
#define IR_PROTOCOL_NEC_H

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define IR_NEC_BITS                        (32)

#define IR_NEC_LEADCODE_SHOT_COUNT        (3)
#define IR_NEC_REPEAT_SHOT_COUNT          (4)

#define IR_TIME_NEC_LEADCODE_TOLERENCE       (0.05) //shot time tolerence for lead code, should be more strict
#define IR_TIME_NEC_TOLERENCE                 (0.2) //shot time tolerence


#define IR_TIME_NEC_TIMEOUT                  (110000)    // us

#define IR_TIME_NEC_HEADER_CODE             (9000)    // us
#define IR_TIME_NEC_HEADER_OFF_CODE         (4500)    // us
#define IR_TIME_NEC_LOGI_0                   (1120)    // us
#define IR_TIME_NEC_LOGI_1                   (2250)    // us

#define IR_TIME_NEC_REPEAT_DELAY             (24500)    // us
#define IR_TIME_NEC_REPEAT_SHOT_1            (9000)    // us
#define IR_TIME_NEC_REPEAT_SHOT_2            (2250)    // us
#define IR_TIME_NEC_REPEAT_SHOT_3            (560)    // us

#endif

