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
#include "IR_PROTOCOL_RC5.h"

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
static U16 _u16LastRc5RawData = 0;
static unsigned long _ulRc5PreDecodeTime=0;

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static BOOL _RawDataFormatRc5(U16  u16RawData, U32 *pu32CustCode, U16 *pu16KeyCode, U8 *pu8State, U8 *pu8Reserved)
{
    BOOL bRepeatKey=FALSE;

    *pu32CustCode = (u16RawData>>6)&0xFFUL;
    *pu16KeyCode = u16RawData&0x3FUL;
    if(LAST_KEY_PROTOCOL(E_IR_PROTOCOL_RC5))
    {
        if((_u16LastRc5RawData&0x800UL)==(u16RawData&0x800UL))//toggle bit equal
        {
            bRepeatKey=TRUE;
        }

        if(bRepeatKey==TRUE)
        {
            *pu8State = E_IR_KEY_STATE_REPEAT;
        }
    }
    else
    {
        *pu8State = E_IR_KEY_STATE_PRESS;
    }

    return TRUE;
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
BOOL _ParseLeadCodeRc5(U8 *pu8StartIndex)
{
    U8 u8Index;
    U32 u32ShotValue[IR_RC5_LEADCODE_SHOT_COUNT];
    BOOL bNegshot[IR_RC5_LEADCODE_SHOT_COUNT];
    U8 u8Datasize=0;

    u8Datasize = _MulProtCommon_GetShotDataSize();
    if(u8Datasize< IR_RC5_LEADCODE_SHOT_COUNT)
    {
        return FALSE;
    }

    for(u8Index=0; u8Index<= u8Datasize-IR_RC5_LEADCODE_SHOT_COUNT; u8Index++)
    {
        if(_MulProtCommon_PeekShot(u8Index, IR_RC5_LEADCODE_SHOT_COUNT, u32ShotValue, bNegshot)==TRUE)
        {
            if((bNegshot[0]==FALSE && u32ShotValue[0]>(IR_TIME_RC5_FIRST_SHOT))&&//1st shot must be positive, might timeout
                bNegshot[1]==TRUE)// 2nd shot must be negtive
            {
                if(u32ShotValue[1] > IR_TIME_LOB(IR_TIME_RC5_HALF_BIT, IR_TIME_RC5_LEADCODE_TOLERENCE) &&
                   u32ShotValue[1] < IR_TIME_UPD(IR_TIME_RC5_HALF_BIT, IR_TIME_RC5_LEADCODE_TOLERENCE))//lead code 0b11, normal RC5
                {
                    *pu8StartIndex = u8Index;
                    return TRUE;
                }
                else if(u32ShotValue[1] > IR_TIME_LOB(IR_TIME_RC5_ONE_BIT, IR_TIME_RC5_LEADCODE_TOLERENCE) &&
                         u32ShotValue[1] < IR_TIME_UPD(IR_TIME_RC5_ONE_BIT, IR_TIME_RC5_LEADCODE_TOLERENCE))//lead code 0b10
                {
                    *pu8StartIndex = u8Index;
                    return TRUE;
                }
            }
        }
        else
        {
            return FALSE;
        }
    }

    return FALSE;
}

IR_DECODE_STATUS _ParseRC5(U32 *pu32CustCode, U16 *pu16KeyCode, U8 *pu8State, U8 *pu8Reserved)
{
    U32 u32ShotValue;
    BOOL bNegShot=FALSE;
    U16 u16Rc5Data=0;
    U8 u8Rc5Bits=0;
    BOOL bLastBit0=FALSE;
    BOOL bLastPShotHalfBitLong=FALSE;
    U8 u8Index=0;

    if (_MDrv_IR_GetSystemTime() - _ulRc5PreDecodeTime> IR_TIME_UPD(IR_TIME_RC5_TIMEOUT, IR_TIME_RC5_TOLERENCE)/1000)
    {
        _u16LastRc5RawData=0;
    }

    _ulRc5PreDecodeTime = _MDrv_IR_GetSystemTime();

#if 1
    if(_MulProtCommon_GetShotDataSize()< IR_RC5_LEADCODE_SHOT_COUNT)
    {
        return E_IR_DECODE_DATA_SHORTAGE;
    }

    if(_ParseLeadCodeRc5(&u8Index)==FALSE)
    {
        _u16LastRc5RawData=0;
        return E_IR_DECODE_ERR;
    }

    u8Index+=1;//skip first p-shot
#endif

    while(_MulProtCommon_PeekShot(u8Index, 1, &u32ShotValue, &bNegShot)==TRUE)//only use peek here
    {
        if(u32ShotValue > IR_TIME_LOB(IR_TIME_RC5_HALF_BIT, IR_TIME_RC5_TOLERENCE) && u32ShotValue < IR_TIME_UPD(IR_TIME_RC5_HALF_BIT, IR_TIME_RC5_TOLERENCE))
        {
            if(bNegShot==TRUE)//negtive
            {
                u16Rc5Data <<= 1;
                if(bLastBit0==TRUE&&bLastPShotHalfBitLong==TRUE)
                {
                    u16Rc5Data |= 0x0UL;
                    bLastBit0 = TRUE;
                }
                else
                {
                    u16Rc5Data |= 0x1UL;
                    bLastBit0 = FALSE;
                }
                u8Rc5Bits += 1;
            }
            else
            {
                bLastPShotHalfBitLong=TRUE;
            }
        }
        else if(u32ShotValue > IR_TIME_LOB(IR_TIME_RC5_ONE_BIT, IR_TIME_RC5_TOLERENCE) &&
                 u32ShotValue < IR_TIME_UPD(IR_TIME_RC5_ONE_BIT, IR_TIME_RC5_TOLERENCE))
        {
            if(bNegShot==TRUE)//negtive
            {
                u16Rc5Data <<= 2;
                u16Rc5Data |= 0x2UL;
                u8Rc5Bits += 2;
                bLastBit0 = TRUE;
            }
            else
            {
                bLastPShotHalfBitLong=FALSE;
            }
        }
        else
        {
            _u16LastRc5RawData=0;
            return E_IR_DECODE_ERR;
        }

        if(u8Rc5Bits==IR_RC5_BITS)
        {
            _RawDataFormatRc5(u16Rc5Data, pu32CustCode, pu16KeyCode, pu8State, pu8Reserved);
            _u16LastRc5RawData=u16Rc5Data;
            return E_IR_DECODE_DATA_OK;
        }

        u8Index++;
    }

    return E_IR_DECODE_DATA_SHORTAGE;
}

DRV_IR_PROTOCOL_TYPE GET_IR_PROTOCOL_ENTRY(E_IR_PROTOCOL_RC5) =
{
    .name               ="IR_PROTOCOL_RC5",
    .etype              =E_IR_PROTOCOL_RC5,
    .findleadcode       =_ParseLeadCodeRc5,
    .parseprotocol      =_ParseRC5,
    .u8LeadCodeMinCount = IR_RC5_LEADCODE_SHOT_COUNT,
    .u32Timeout         = IR_TIME_UPD(IR_TIME_RC5_TIMEOUT, IR_TIME_RC5_TOLERENCE),
};

