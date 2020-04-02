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

#ifndef IR_PROTOCOL_RC6_H
#define IR_PROTOCOL_RC6_H

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define IR_RC6_BIT                          1
#define IR_RC6_HALF_BIT_TIME_UNIT          (444) //us
#define IR_RC6_BIT_TIME                    (889) //us

#define IR_TIME_RC6_HALF_BIT                (444) // us
#define IR_TIME_RC6_ONE_BIT                 (889) // us

//LEAD field
#define IR_RC6_LEAD_P_SHOT_NBITS           3
#define IR_RC6_LEAD_N_SHOT_NBITS           1
#define IR_RC6_LEAD_P_SHOT_TIME            (2666) //us //3*IR_TIME_RC6_ONE_BIT
#define IR_RC6_LEAD_N_SHOT_TIME            (889) //us //1*IR_TIME_RC6_ONE_BIT
//Start Bit field
#define IR_RC6_START_BIT_NBITS             1
#define IR_RC6_START_BIT_TIME              (889) //us //1*IR_TIME_RC6_ONE_BIT
//Mode field
#define IR_RC6_MODE_NBITS                  3
#define IR_RC6_MODE_TIME                   (2667)//us //3*IR_RC6_BIT_TIME
#define IR_RC6_MODE_0                      0x0
#define IR_RC6_MODE_6                      0x6 // mb2=1, mb1=1, mb1=0 , binary =110
//Toggle field
#define IR_RC6_TOGGLE_NBITS                1
#define IR_RC6_TOGGLE_TIME                 (1778)//us //2*IR_RC6_BIT_TIME
#define IR_RC6_TOGGLE_MODE_6A_VALUE        0
#define IR_RC6_TOGGLE_MODE_6B_VALUE        1
//Control field
#define IR_RC6_MODE_0_CONTROL_NBITS         (8)
//Customer Code
#define IR_RC6_MODE_6A_SCC_FIELD_NBITS      (7)
#define IR_RC6_MODE_6A_LCC_FIELD_NBITS      (15)
//Data field
#define IR_RC6_MODE_0_DATA_NBITS            (8)
#define IR_RC6_MODE_6A_SCC_DATA_NBITS       (24)
#define IR_RC6_MODE_6A_LCC_DATA_NBITS       (128)
//Prefix field
#define IR_RC6_SIGNAL_FREE_TIME             (2666) //us

#define IR_RC6_MODE_0_NBITS                 (21)//21 bits for RC6 Mode 0: SB+Mode+TR+Control+Information
#define IR_RC6_MODE_6A_SCC_NBITS            (37)//37 bits max for RC6 Mode 6A short: SB+Mode+TR+1+CustomerCode+Information
#define IR_RC6_MODE_6A_LCC_NBITS           (150)//157 bits max for RC6 Mode 6A long: SB+Mode+TR+1+CustomerCode+Information

#define IR_RC6_LEADCODE_SHOT_COUNT         (3)

#define IR_TIME_RC6_LEADCODE_TOLERENCE     (0.1) //shot time tolerence for lead code, should be more strict
#define IR_TIME_RC6_TOLERENCE              (0.3) //shot time tolerence

#define IR_TIME_RC6_MODE_0_TIMEOUT               (130000) //us
#define IR_TIME_RC6_MODE_6A_SCC_TIMEOUT          (180000) //us
#define IR_TIME_RC6_MODE_6A_LCC_TIMEOUT          (273000) //us: IR_TIME_RC6_MODE_6A_SCC_TIMEOUT + (889*(128-24))

#define IR_TIME_RC6_STUCK_MODE              (30000) //ms
#define IR_TIME_RC6_STUCK_MODE_TOLERANCE    (3000)//ms

#define IR_RC6_HEADER_SHOTS_LENGTH           11//Number of shots to get the complete header including toggle
#define IR_RC6_FREE_TIME_SHOTS_LENGTH        2666 //Value of the last shot indicating the end of valid message
#define IR_RC6_VALID_MESSAGE_SHOTS_LENGTH    65

#endif
