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

#ifndef IR_PROTOCOL_OCN_H
#define IR_PROTOCOL_OCN_H

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define IR_OCN_CUSTOM_BITS                (24)
#define IR_OCN_KEYCODE_BITS               (16)
#define IR_OCN_BITS                        (IR_OCN_CUSTOM_BITS+IR_OCN_KEYCODE_BITS) //customer: 24 + Keycode:16

#define IR_OCN_LEADCODE_SHOT_COUNT        (3)
#define IR_OCN_REPEAT_SHOT_COUNT          (4)

#define IR_TIME_OCN_LEADCODE_TOLERENCE       (0.05) //shot time tolerence for lead code, should be more strict
#define IR_TIME_OCN_TOLERENCE                 (0.2)   //shot time tolerence


#define IR_TIME_OCN_TIMEOUT                  (108000)    // us

#define IR_TIME_OCN_HEADER_CODE             (3640)    // us
#define IR_TIME_OCN_HEADER_OFF_CODE         (1800)    // us
#define IR_TIME_OCN_LOGI_0                   (1120)    // us
#define IR_TIME_OCN_LOGI_1                   (1680)    // us

#define IR_TIME_OCN_REPEAT_DELAY            (54000)    // us
#define IR_TIME_OCN_REPEAT_SHOT_1           (3640)
#define IR_TIME_OCN_REPEAT_SHOT_2           (3640)
#define IR_TIME_OCN_REPEAT_SHOT_3           (560)

#endif

