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
#include "IR_PROTOCOL_OCN.h"

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
static unsigned long long _u64OcnLastDecodeData=0;
static unsigned long _ulOcnPreDecodeTime=0;
static unsigned long long _u64OcnData=0;
static U8 _u8OcnBits=0;
static BOOL _b2ndLendCodeExist = FALSE;

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static IR_DECODE_STATUS _ParseRepeatCodeOcn(void)//check if repeat symbol
{
    U8 u8Index;
    U32 u32ShotValue[IR_OCN_REPEAT_SHOT_COUNT];
    BOOL bNegShot[IR_OCN_REPEAT_SHOT_COUNT];

    if(_MulProtCommon_GetShotDataSize()<IR_OCN_REPEAT_SHOT_COUNT)
    {
        return E_IR_DECODE_DATA_SHORTAGE;
    }

    for(u8Index=0; u8Index<=_MulProtCommon_GetShotDataSize()-IR_OCN_REPEAT_SHOT_COUNT; u8Index++)
    {
        if(_MulProtCommon_PeekShot(u8Index, IR_OCN_REPEAT_SHOT_COUNT, u32ShotValue, bNegShot)==TRUE)
        {
            //first shot must be positive and 54ms delay
            if(bNegShot[0]==FALSE && u32ShotValue[0] >= IR_TIME_LOB(IR_TIME_OCN_REPEAT_DELAY, IR_TIME_OCN_TOLERENCE) &&
               u32ShotValue[1] >= IR_TIME_LOB(IR_TIME_OCN_REPEAT_SHOT_1, IR_TIME_OCN_TOLERENCE) &&
               u32ShotValue[1] <= IR_TIME_UPD(IR_TIME_OCN_REPEAT_SHOT_1, IR_TIME_OCN_TOLERENCE) &&
               u32ShotValue[2] >= IR_TIME_LOB(IR_TIME_OCN_REPEAT_SHOT_2, IR_TIME_OCN_TOLERENCE) &&
               u32ShotValue[2] <= IR_TIME_UPD(IR_TIME_OCN_REPEAT_SHOT_2, IR_TIME_OCN_TOLERENCE) &&
               u32ShotValue[3] >= IR_TIME_LOB(IR_TIME_OCN_REPEAT_SHOT_3, IR_TIME_OCN_TOLERENCE) &&
               u32ShotValue[3] <= IR_TIME_UPD(IR_TIME_OCN_REPEAT_SHOT_3, IR_TIME_OCN_TOLERENCE))
            {
                return E_IR_DECODE_DATA_OK;
            }
        }
        else
        {
            return E_IR_DECODE_ERR;
        }
    }

    return E_IR_DECODE_ERR;
}

static void _OcnRAW2Format(unsigned long long u64RawData, U32 *pu32CustCode, U16 *pu16KeyCode, U8 *pu8State, U8 *pu8Reserved)
{
    BOOL bRepeatKey=FALSE;

    if(LAST_KEY_PROTOCOL(E_IR_PROTOCOL_PZ_OCN))
    {
        if(u64RawData==_u64OcnLastDecodeData)
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

    *pu32CustCode = (_MulProtCommon_LSB2MSB((u64RawData>>32)&0xFFUL))<<16 | (_MulProtCommon_LSB2MSB((u64RawData>>24)&0xFFUL))<<8 |_MulProtCommon_LSB2MSB((u64RawData>>16)&0xFFUL);//24 bits custom code, must be 'O','C','N'
    *pu16KeyCode = (_MulProtCommon_LSB2MSB((u64RawData>>8)&0xFFUL))<<8 | _MulProtCommon_LSB2MSB(u64RawData&0xFFUL);

}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
BOOL _Parse1stLeadCodeOcn(U8 *pu8StartIndex)
{
    U8 u8Index;
    U32 u32ShotValue[IR_OCN_LEADCODE_SHOT_COUNT];
    BOOL bNegshot[IR_OCN_LEADCODE_SHOT_COUNT];
    U8 u8Datasize=0;

    u8Datasize = _MulProtCommon_GetShotDataSize();
    if(u8Datasize< IR_OCN_LEADCODE_SHOT_COUNT)
    {
        return FALSE;
    }

    for(u8Index=0; u8Index<= u8Datasize-(IR_OCN_LEADCODE_SHOT_COUNT); u8Index++)
    {
        if(_MulProtCommon_PeekShot(u8Index, IR_OCN_LEADCODE_SHOT_COUNT, u32ShotValue, bNegshot)==TRUE)
        {
            if(bNegshot[0]==TRUE)
            {
                continue;//1st shot must be positive
            }

            if(bNegshot[1]==TRUE && bNegshot[2]==FALSE)//2nd shot must be negtive
            {
                if((u32ShotValue[1] > IR_TIME_LOB(IR_TIME_OCN_HEADER_CODE, IR_TIME_OCN_LEADCODE_TOLERENCE) &&
                    u32ShotValue[1] < IR_TIME_UPD(IR_TIME_OCN_HEADER_CODE, IR_TIME_OCN_LEADCODE_TOLERENCE)) &&
                   (u32ShotValue[2] > IR_TIME_LOB(IR_TIME_OCN_HEADER_OFF_CODE, IR_TIME_OCN_LEADCODE_TOLERENCE) &&
                    u32ShotValue[2] < IR_TIME_UPD(IR_TIME_OCN_HEADER_OFF_CODE, IR_TIME_OCN_LEADCODE_TOLERENCE)))
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

IR_DECODE_STATUS _ParseOCN(U32 *pu32CustCode, U16 *pu16KeyCode, U8 *pu8State, U8 *pu8Reserved)
{
    IR_DECODE_STATUS eDecodeStatus = E_IR_DECODE_ERR;
    U32 u32ShotValue;
    BOOL bNegShot=FALSE;
    //unsigned long long u64OcnData=0;
    //U8 u8OcnBits=0;
    BOOL bLastShotN=FALSE;
    U32 u32LastNValue=0;
    //BOOL b2ndLendCodeExist = FALSE;

    if (_MDrv_IR_GetSystemTime() - _ulOcnPreDecodeTime> (IR_TIME_OCN_TIMEOUT/1000))//>108ms timeout
    {
        _u64OcnLastDecodeData=0;
        _u64OcnData=0;
        _u8OcnBits=0;
        _b2ndLendCodeExist=FALSE;
    }

    _ulOcnPreDecodeTime = _MDrv_IR_GetSystemTime();
    if(_u64OcnLastDecodeData!=0)//decode success last time, should check if repeat symbol
    {
        if((eDecodeStatus= _ParseRepeatCodeOcn())==E_IR_DECODE_DATA_OK)
        {
            _OcnRAW2Format(_u64OcnLastDecodeData, pu32CustCode, pu16KeyCode, pu8State, pu8Reserved);
        }

        return eDecodeStatus;
    }
    else
    {
        U8 u8StartIndex;
        if(_MulProtCommon_GetShotDataSize()<IR_OCN_LEADCODE_SHOT_COUNT)
        {
            return E_IR_DECODE_DATA_SHORTAGE;
        }

        if(_Parse1stLeadCodeOcn(&u8StartIndex)==FALSE)
        {
            return E_IR_DECODE_ERR;
        }

        while(_MulProtCommon_PeekShot(u8StartIndex+IR_OCN_LEADCODE_SHOT_COUNT+(_u8OcnBits+_b2ndLendCodeExist)*2, 1, &u32ShotValue, &bNegShot)==TRUE)//only use peek here
        {
            if(bNegShot==TRUE)//Negtive
            {
                bLastShotN = TRUE;
                u32LastNValue = u32ShotValue;
            }
            else//positive
            {
                if(bLastShotN==TRUE)//A complete logic is: N+P
                {
                    if(_u8OcnBits<IR_OCN_CUSTOM_BITS)
                    {
                        if(u32LastNValue+u32ShotValue >= IR_TIME_LOB(IR_TIME_OCN_LOGI_0, IR_TIME_OCN_TOLERENCE) && u32LastNValue+u32ShotValue <= IR_TIME_UPD(IR_TIME_OCN_LOGI_0, IR_TIME_OCN_TOLERENCE))//logical 0
                        {
                            _u64OcnData<<=1;
                            _u64OcnData |=0;
                            _u8OcnBits+=1;
                        }
                        else if(u32LastNValue+u32ShotValue >= IR_TIME_LOB(IR_TIME_OCN_LOGI_1, IR_TIME_OCN_TOLERENCE) && u32LastNValue+u32ShotValue <= IR_TIME_UPD(IR_TIME_OCN_LOGI_1, IR_TIME_OCN_TOLERENCE))//logical 1
                        {
                            _u64OcnData <<=1;
                            _u64OcnData |=1;
                            _u8OcnBits+=1;
                        }
                        else
                        {
                            _u64OcnLastDecodeData=0;
                            _u64OcnData=0;
                            _u8OcnBits=0;
                            _b2ndLendCodeExist=FALSE;
                            return E_IR_DECODE_ERR;
                        }
                    }
                    else if(_u8OcnBits>=IR_OCN_CUSTOM_BITS&&_u8OcnBits<IR_OCN_BITS)
                    {
                        if(_b2ndLendCodeExist==FALSE)
                        {
                            if(u32LastNValue+u32ShotValue >= IR_TIME_LOB(IR_TIME_OCN_HEADER_CODE+IR_TIME_OCN_HEADER_OFF_CODE, IR_TIME_OCN_TOLERENCE) &&
                               u32LastNValue+u32ShotValue <= IR_TIME_UPD(IR_TIME_OCN_HEADER_CODE+IR_TIME_OCN_HEADER_OFF_CODE, IR_TIME_OCN_TOLERENCE))// 2nd LeadCode
                            {
                                _b2ndLendCodeExist=TRUE;
                            }
                            else
                            {
                                _u64OcnLastDecodeData=0;
                                _u64OcnData=0;
                                _u8OcnBits=0;
                                _b2ndLendCodeExist=FALSE;
                                return E_IR_DECODE_ERR;//2nd Leadcode recognize fail
                            }
                        }
                        else//b2ndLendCodeExist==TRUE
                        {
                            if(u32LastNValue+u32ShotValue >= IR_TIME_LOB(IR_TIME_OCN_LOGI_0, IR_TIME_OCN_TOLERENCE) && u32LastNValue+u32ShotValue <= IR_TIME_UPD(IR_TIME_OCN_LOGI_0, IR_TIME_OCN_TOLERENCE))//logical 0
                            {
                                _u64OcnData<<=1;
                                _u64OcnData |=0;
                                _u8OcnBits+=1;
                            }
                            else if(u32LastNValue+u32ShotValue >= IR_TIME_LOB(IR_TIME_OCN_LOGI_1, IR_TIME_OCN_TOLERENCE) && u32LastNValue+u32ShotValue <= IR_TIME_UPD(IR_TIME_OCN_LOGI_1, IR_TIME_OCN_TOLERENCE))//logical 1
                            {
                                _u64OcnData <<=1;
                                _u64OcnData |=1;
                                _u8OcnBits+=1;
                            }
                            else
                            {
                                _u64OcnLastDecodeData=0;
                                _u64OcnData=0;
                                _u8OcnBits=0;
                                _b2ndLendCodeExist=FALSE;
                                return E_IR_DECODE_ERR;
                            }
                        }
                    }
                }

                bLastShotN=FALSE;
                u32LastNValue = 0;
            }

            u8StartIndex++;
        }

        if(_u8OcnBits==IR_OCN_BITS && bLastShotN==TRUE)
        {
            _OcnRAW2Format(_u64OcnData, pu32CustCode, pu16KeyCode, pu8State, pu8Reserved);
            _u64OcnLastDecodeData=_u64OcnData;
            _u64OcnData=0;
            _u8OcnBits=0;
            _b2ndLendCodeExist=FALSE;
            return E_IR_DECODE_DATA_OK;
        }

        return E_IR_DECODE_DATA_SHORTAGE;
    }

}

DRV_IR_PROTOCOL_TYPE GET_IR_PROTOCOL_ENTRY(E_IR_PROTOCOL_PZ_OCN) =
{
    .name               ="IR_PROTOCOL_OCN",
    .etype              =E_IR_PROTOCOL_PZ_OCN,
    .findleadcode       =_Parse1stLeadCodeOcn,
    .parseprotocol      =_ParseOCN,
    .u8LeadCodeMinCount = IR_OCN_LEADCODE_SHOT_COUNT,
    .u32Timeout         = IR_TIME_UPD(IR_TIME_OCN_TIMEOUT, IR_TIME_OCN_TOLERENCE),
};

