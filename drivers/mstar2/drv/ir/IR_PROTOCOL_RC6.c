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
#include "IR_PROTOCOL_RC6.h"

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
static U64 _u64LastRc6RawData = 0;
static unsigned long _ulRc6PreDecodeTime=0;
static U8 _ulastToggle =0xF;
#define MAX_HALF_BITS    60

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static void _GetRc6Mode(U64  u64RawData, U8 u8NBits, U8 *pu8RC6Mode)
{
    U8 u8parseIndx = 0;

    u8parseIndx++;
    u8parseIndx+=IR_RC6_MODE_NBITS;
    *pu8RC6Mode =(u64RawData>>(u8NBits-u8parseIndx))&0x07UL;
}

static BOOL _RawDataFormatRc6(U64  u64RawData, U8 u8NBits, U32 *pu32CustCode, U16 *pu16KeyCode, U8 *pu8State, U8 *pu8Reserved)
{
    BOOL bRepeatKey=FALSE;
    U8 u8StartBit =0;
    U8 u8RC6Mode =0;
    U8 u8Toggle = 0;
    U8 u8Control = 0;
    U8 u8Mode0Data = 0;
    U8 u8CustBit = 0;
    U16 u16CustomerCode = 0 ;
    U32 u32ShortData = 0;
    U32 u32LongData[4]={0,0,0,0};
    U8 u8parseIndx = 0;
    U8 i=0,Mask=0;

    //Get Satrtbit: should be always 1:
    u8parseIndx++;
    u8StartBit = (u64RawData>>(u8NBits-u8parseIndx))&0x01UL;
    //Get RC6 Mode: Mode0=>000, Mode6A/6B=>110
    u8parseIndx+=IR_RC6_MODE_NBITS;
    u8RC6Mode = (u64RawData>>(u8NBits-u8parseIndx))&0x07UL;
    //Get Toggle bit
    u8parseIndx+=IR_RC6_TOGGLE_NBITS;
    u8Toggle = (u64RawData>>(u8NBits-u8parseIndx))&0x01UL;
    if(u8RC6Mode==IR_RC6_MODE_0)
    {
        //Get control
        u8parseIndx+=IR_RC6_MODE_0_CONTROL_NBITS;
        u8Control = (u64RawData>>(u8NBits-u8parseIndx))&0xFFUL;
        //Get Information
        u8parseIndx+=IR_RC6_MODE_0_DATA_NBITS;
        u8Mode0Data = (u64RawData>>(u8NBits-u8parseIndx))&0xFFUL;
        *pu32CustCode = u8StartBit;
        *pu32CustCode<<=0x3;
        *pu32CustCode |= u8RC6Mode;
        *pu32CustCode<<=0x1;
        *pu32CustCode |=u8Toggle;
        *pu32CustCode<<=0x8;
        *pu32CustCode |= u8Control;
        *pu16KeyCode = u8Mode0Data;
        _ulastToggle = u8Toggle;
        *pu8Reserved = u8StartBit;
    } else if (u8RC6Mode==IR_RC6_MODE_6) {
        //Get SCC or LCC
        u8parseIndx+=1;
        u8CustBit = (u64RawData>>(u8NBits-u8parseIndx))&0x01UL;
        if(u8CustBit==0)//SCC
        {
            //Get Short Customer Code
            u8parseIndx+=IR_RC6_MODE_6A_SCC_FIELD_NBITS;
            u16CustomerCode = (u64RawData>>(u8NBits-u8parseIndx))&0x7FUL;
            //Get Information
            for (i=0; i<=u8NBits-u8parseIndx; i++)
            {
                Mask |= 1 << i;
            }
            u32ShortData = Mask & u64RawData;
            *pu32CustCode = u16CustomerCode;
            *pu16KeyCode = (U16)u32ShortData;
            _ulastToggle = u8Toggle;
            *pu8Reserved = u8StartBit;
        }
        else if(u8CustBit==1)//LCC
        {
            //Get Long Customer Code
            u8parseIndx+=IR_RC6_MODE_6A_LCC_FIELD_NBITS;
            u16CustomerCode = (u64RawData>>(u8NBits-u8parseIndx))&0xFFFFUL;
            //Get Information
            for (i=0; i<=u8NBits-u8parseIndx; i++)
            {
                Mask |= 1 << i;
            }
            if((u8NBits-u8parseIndx)<=32)
            {
                u32LongData[0] = (Mask & u64RawData);
            } else if ((u8NBits-u8parseIndx)<=64)
            {
                u32LongData[0] = (Mask & u64RawData) & 0xFFFFFFFF;
                u32LongData[1] = ((Mask & u64RawData) >> 32) & 0xFFFFFFFF;
            } else if ((u8NBits-u8parseIndx)<=96)
            {
                u32LongData[0] = (Mask & u64RawData) & 0xFFFFFFFF;
                  u32LongData[1] = ((Mask & u64RawData) >> 32) & 0xFFFFFFFF;
                u32LongData[2] = ((Mask & u64RawData) >> 64) & 0xFFFFFFFF;
            } else if ((u8NBits-u8parseIndx)<=128)
            {
                u32LongData[0] = (Mask & u64RawData) & 0xFFFFFFFF;
                  u32LongData[1] = ((Mask & u64RawData) >> 32) & 0xFFFFFFFF;
                u32LongData[2] = ((Mask & u64RawData) >> 64) & 0xFFFFFFFF;
                u32LongData[3] = ((Mask & u64RawData) >> 96) & 0xFFFFFFFF;
            }
            *pu32CustCode = u16CustomerCode;
            *pu16KeyCode = *(U16*)u32LongData;
            _ulastToggle = u8Toggle;
            *pu8Reserved = u8StartBit;
        }
    }

    if(LAST_KEY_PROTOCOL(E_IR_PROTOCOL_RC6))
    {
        if(u64RawData ==_u64LastRc6RawData)
        {
            bRepeatKey=TRUE;
        }

        if(bRepeatKey==TRUE)
        {
            *pu8State = E_IR_KEY_STATE_REPEAT;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        *pu8State = E_IR_KEY_STATE_PRESS;
        _u64LastRc6RawData = u64RawData;
    }
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
BOOL  _ParseLeadCodeRc6(U8 *pu8StartIndex)
{
    U8 u8Index;
    U32 u32ShotValue[IR_RC6_LEADCODE_SHOT_COUNT];
    BOOL bNegshot[IR_RC6_LEADCODE_SHOT_COUNT];
    U8 u8Datasize=0;

    u8Datasize = _MulProtCommon_GetShotDataSize();

    if(u8Datasize< IR_RC6_LEADCODE_SHOT_COUNT)
    {
        return FALSE;
    }
    for(u8Index=0; u8Index<= u8Datasize-(IR_RC6_LEADCODE_SHOT_COUNT); u8Index++)
    {
        if(_MulProtCommon_PeekShot(u8Index, IR_RC6_LEADCODE_SHOT_COUNT, u32ShotValue, bNegshot)==TRUE)
        {
            if(bNegshot[0]==TRUE)
            {
                continue;//1st shot must be positive
            }

            if(bNegshot[1]==TRUE && bNegshot[2]==FALSE)//2nd shot must be negtive
            {
                if((u32ShotValue[1] > IR_TIME_LOB(IR_RC6_LEAD_P_SHOT_TIME, IR_TIME_RC6_LEADCODE_TOLERENCE) && u32ShotValue[1] < IR_TIME_UPD(IR_RC6_LEAD_P_SHOT_TIME, IR_TIME_RC6_LEADCODE_TOLERENCE)) &&
                   (u32ShotValue[2] > IR_TIME_LOB(IR_RC6_LEAD_N_SHOT_TIME, IR_TIME_RC6_LEADCODE_TOLERENCE) && u32ShotValue[2] < IR_TIME_UPD(IR_RC6_LEAD_N_SHOT_TIME, IR_TIME_RC6_LEADCODE_TOLERENCE)))
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

IR_DECODE_STATUS _ParseRC6(U32 *pu32CustCode, U16 *pu16KeyCode, U8 *pu8State, U8 *pu8Reserved)
{
    U32 u32ShotValue;
    BOOL bNegShot=FALSE;
    U64 u64Rc6Data=0,u64Rc6MsbData=0,u64Rc6LsbData=0;
    U64 u64Rc6HalfBitData=0;
    U64 u64MsbRc6HalfBitData=0;
    U64 temp=0;
    U64 data[2];
    U8 u8parseIndex=0;
    U8 u8Rc6NHalfBits=0;
    U8 u8MsbRc6NHalfBits=0;
    U8 firstHalfbit=0,secondHalfbit=0;
    U64 u64Mask=0;
    U8 u8Rc6Bits=0,u8Rc6MsbBits=0,u8Rc6LsbBits=0;
    U8 u8Index=0;
    U8 u8StartIdex=0;
    BOOL bValidDataEnd=FALSE;
    BOOL bDataSplit = FALSE;
    U8 u8Rc6Mode=0;
    BOOL bResult;

    if (_MDrv_IR_GetSystemTime() - _ulRc6PreDecodeTime> IR_TIME_UPD(IR_TIME_RC6_MODE_0_TIMEOUT, IR_TIME_RC6_TOLERENCE)/1000)
    {
        if (_MDrv_IR_GetSystemTime() - _ulRc6PreDecodeTime> IR_TIME_UPD(IR_TIME_RC6_MODE_6A_SCC_TIMEOUT, IR_TIME_RC6_TOLERENCE)/1000)
        {
            if (_MDrv_IR_GetSystemTime() - _ulRc6PreDecodeTime> IR_TIME_UPD(IR_TIME_RC6_MODE_6A_LCC_TIMEOUT, IR_TIME_RC6_TOLERENCE)/1000)
            {
                _u64LastRc6RawData=0;
            }
        }
    }

    _ulRc6PreDecodeTime = _MDrv_IR_GetSystemTime();

    if(_MulProtCommon_GetShotDataSize()< IR_RC6_LEADCODE_SHOT_COUNT)
    {
        return E_IR_DECODE_DATA_SHORTAGE;
    }
    if(_MulProtCommon_GetShotDataSize()< IR_RC6_VALID_MESSAGE_SHOTS_LENGTH)
    {
        return E_IR_DECODE_DATA_SHORTAGE;
    }
    if(_ParseLeadCodeRc6(&u8Index)==FALSE)
    {
        _u64LastRc6RawData=0;
        return E_IR_DECODE_ERR;
    }
    u8StartIdex = u8Index;
    while((_MulProtCommon_PeekShot(u8Index+IR_RC6_LEADCODE_SHOT_COUNT, 1, &u32ShotValue, &bNegShot)==TRUE) && (bValidDataEnd==FALSE))//only use peek here
    {
        if(u32ShotValue > IR_TIME_LOB(IR_TIME_RC6_HALF_BIT, IR_TIME_RC6_TOLERENCE) &&
            u32ShotValue < IR_TIME_UPD(IR_TIME_RC6_HALF_BIT, IR_TIME_RC6_TOLERENCE))
        {
            if(bNegShot==FALSE)//positive
            {
                if(u8Rc6NHalfBits>=MAX_HALF_BITS)
                {
                    if(u8Rc6NHalfBits%2)//impaire
                    {
                        temp = u64Rc6HalfBitData;
                        u64MsbRc6HalfBitData = (u64Rc6HalfBitData>>0x1);
                        u8MsbRc6NHalfBits = (u8Rc6NHalfBits-1);
                        u64Rc6HalfBitData=(temp&0x1);
                        u8Rc6NHalfBits=1;
                    }
                    else//paire
                    {
                        u64MsbRc6HalfBitData = u64Rc6HalfBitData;
                        u8MsbRc6NHalfBits = u8Rc6NHalfBits;
                        u64Rc6HalfBitData=0;
                        u8Rc6NHalfBits=0;
                    }
                    u64Rc6HalfBitData <<=1;
                    u64Rc6HalfBitData |= 0x1UL;
                    bDataSplit = TRUE;
                }
                else
                {
                    u64Rc6HalfBitData <<=1;
                    u64Rc6HalfBitData |= 0x1UL;
                }
            }
            else
            {
                if(u8Rc6NHalfBits>=MAX_HALF_BITS)
                {
                    if(u8Rc6NHalfBits%2)//impaire
                    {
                        temp = u64Rc6HalfBitData;
                        u64MsbRc6HalfBitData = (u64Rc6HalfBitData>>0x1);
                        u8MsbRc6NHalfBits = (u8Rc6NHalfBits-1);
                        u64Rc6HalfBitData=(temp&0x1);
                        u8Rc6NHalfBits=1;
                    }
                    else//paire
                    {
                        u64MsbRc6HalfBitData = u64Rc6HalfBitData;
                        u8MsbRc6NHalfBits = u8Rc6NHalfBits;
                        u64Rc6HalfBitData=0;
                        u8Rc6NHalfBits=0;
                    }
                    u64Rc6HalfBitData <<=1;
                    u64Rc6HalfBitData |= 0x0UL;
                    bDataSplit = TRUE;
                }
                else
                {
                    u64Rc6HalfBitData <<=1;
                    u64Rc6HalfBitData |= 0x0UL;
                }

            }
            u8Rc6NHalfBits +=1;
        }
        else if(u32ShotValue > IR_TIME_LOB(IR_TIME_RC6_ONE_BIT, IR_TIME_RC6_TOLERENCE) &&
                 u32ShotValue < IR_TIME_UPD(IR_TIME_RC6_ONE_BIT, IR_TIME_RC6_TOLERENCE))
        {
            if(bNegShot==FALSE)//positive
            {
                if(u8Rc6NHalfBits>=MAX_HALF_BITS)
                {
                    if(u8Rc6NHalfBits%2)//impaire
                    {
                        temp = u64Rc6HalfBitData;
                        u64MsbRc6HalfBitData = (u64Rc6HalfBitData>>0x1);
                        u8MsbRc6NHalfBits = (u8Rc6NHalfBits-1);
                        u64Rc6HalfBitData=(temp&0x1);
                        u8Rc6NHalfBits=1;
                    }
                    else//paire
                    {
                        u64MsbRc6HalfBitData = u64Rc6HalfBitData;
                        u8MsbRc6NHalfBits = u8Rc6NHalfBits;
                        u64Rc6HalfBitData=0;
                        u8Rc6NHalfBits=0;
                    }
                    u64Rc6HalfBitData <<=2;
                    u64Rc6HalfBitData |= 0x3UL;
                    bDataSplit = TRUE;
                }
                else
                {
                    u64Rc6HalfBitData <<=2;
                    u64Rc6HalfBitData |= 0x3UL;
                }
            }
            else
            {
                if(u8Rc6NHalfBits>=MAX_HALF_BITS)
                {
                    if(u8Rc6NHalfBits%2)//impaire
                    {
                        temp = u64Rc6HalfBitData;
                        u64MsbRc6HalfBitData = (u64Rc6HalfBitData>>0x1);
                        u8MsbRc6NHalfBits = (u8Rc6NHalfBits-1);
                        u64Rc6HalfBitData=(temp&0x1);
                        u8Rc6NHalfBits=1;
                    }
                    else//paire
                    {
                        u64MsbRc6HalfBitData = u64Rc6HalfBitData;
                        u8MsbRc6NHalfBits = u8Rc6NHalfBits;
                        u64Rc6HalfBitData=0;
                        u8Rc6NHalfBits=0;
                    }
                    u64Rc6HalfBitData <<=2;
                    u64Rc6HalfBitData |= 0x0UL;
                    bDataSplit = TRUE;
                }
                else
                {
                    u64Rc6HalfBitData <<=2;
                    u64Rc6HalfBitData |= 0x0UL;
                }
            }
            u8Rc6NHalfBits +=2;
        }
        else if(u32ShotValue > IR_TIME_LOB(IR_TIME_RC6_ONE_BIT+IR_TIME_RC6_HALF_BIT, IR_TIME_RC6_TOLERENCE) &&
                 u32ShotValue < IR_TIME_UPD(IR_TIME_RC6_ONE_BIT+IR_TIME_RC6_HALF_BIT, IR_TIME_RC6_TOLERENCE))
        {
            if((u8Index+IR_RC6_LEADCODE_SHOT_COUNT) <= (IR_RC6_HEADER_SHOTS_LENGTH + u8StartIdex))
            {
                if(bNegShot==FALSE)//positive
                {
                    if(u8Rc6NHalfBits>=MAX_HALF_BITS)
                    {
                        if(u8Rc6NHalfBits%2)//impaire
                        {
                            temp = u64Rc6HalfBitData;
                            u64MsbRc6HalfBitData = (u64Rc6HalfBitData>>0x1);
                            u8MsbRc6NHalfBits = (u8Rc6NHalfBits-1);
                            u64Rc6HalfBitData=(temp&0x1);
                            u8Rc6NHalfBits=1;
                        }
                        else//paire
                        {
                            u64MsbRc6HalfBitData = u64Rc6HalfBitData;
                            u8MsbRc6NHalfBits = u8Rc6NHalfBits;
                            u64Rc6HalfBitData=0;
                            u8Rc6NHalfBits=0;
                        }
                        u64Rc6HalfBitData <<=3;
                        u64Rc6HalfBitData |= 0x7UL;
                        bDataSplit = TRUE;

                    }
                    else
                    {
                        u64Rc6HalfBitData <<=3;
                        u64Rc6HalfBitData |= 0x7UL;

                    }
                }
                else
                {
                    if(u8Rc6NHalfBits>=MAX_HALF_BITS)
                    {
                        if(u8Rc6NHalfBits%2)//impaire
                        {
                            temp = u64Rc6HalfBitData;
                            u64MsbRc6HalfBitData = (u64Rc6HalfBitData>>0x1);
                            u8MsbRc6NHalfBits = (u8Rc6NHalfBits-1);
                            u64Rc6HalfBitData=(temp&0x1);
                            u8Rc6NHalfBits=1;
                        }
                        else//paire
                        {
                            u64MsbRc6HalfBitData = u64Rc6HalfBitData;
                            u8MsbRc6NHalfBits = u8Rc6NHalfBits;
                            u64Rc6HalfBitData=0;
                            u8Rc6NHalfBits=0;
                        }
                        u64Rc6HalfBitData <<=3;
                        u64Rc6HalfBitData |= 0x0UL;
                        bDataSplit = TRUE;
                    }
                    else
                    {
                        u64Rc6HalfBitData <<=3;
                        u64Rc6HalfBitData |= 0x0UL;
                    }
                }
                u8Rc6NHalfBits +=3;
            }
            else
            {
                _u64LastRc6RawData=0;
                return E_IR_DECODE_ERR;
            }
        }
        else
        {
            if(u32ShotValue <IR_RC6_FREE_TIME_SHOTS_LENGTH)
            {
                _u64LastRc6RawData=0;
                return E_IR_DECODE_ERR;
            }
            else
            {
                if((u8Rc6NHalfBits%2)!=0)
                {//Add last half bit
                    u64Rc6HalfBitData <<=1;
                    u64Rc6HalfBitData |= 0x1UL;
                    u8Rc6NHalfBits++;
                }
                bValidDataEnd = TRUE;
            }
        }
        u8Index++;
    }

    if(bDataSplit==FALSE)
    {
      u8parseIndex = 1;
      while(u8parseIndex < u8Rc6NHalfBits)
      {
        firstHalfbit = (u64Rc6HalfBitData>>(u8Rc6NHalfBits-u8parseIndex))&0x1;
        u8parseIndex++;
        secondHalfbit = (u64Rc6HalfBitData>>(u8Rc6NHalfBits-u8parseIndex))&0x1;
        if((firstHalfbit==0) && (secondHalfbit==1))
        {//Negatif bit
            u64Rc6Data <<=1;
            u64Rc6Data |= 0x0UL;
            u8Rc6Bits++;
            u64Mask<<=1;
            u64Mask |= 0x1UL;
            u8parseIndex++;
        }
        if((firstHalfbit==1) && (secondHalfbit==0))
        {//positif bit
            u64Rc6Data <<=1;
            u64Rc6Data |= 0x1UL;
            u8Rc6Bits++;
            u64Mask<<=1;
            u64Mask |= 0x1UL;
            u8parseIndex++;
        }
        if((firstHalfbit==0) && (secondHalfbit==0))
        {//toggle bit
            u64Rc6Data <<=1;
            u64Rc6Data |= 0x0UL;
            u8Rc6Bits++;
            u64Mask<<=1;
            u64Mask |= 0x1UL;
            u8parseIndex+=3;
        }
        if((firstHalfbit==1) && (secondHalfbit==1))
        {//toggle bit
            u64Rc6Data <<=1;
            u64Rc6Data |= 0x1UL;
            u8Rc6Bits++;
            u64Mask<<=1;
            u64Mask |= 0x1UL;
            u8parseIndex+=3;
        }
      }
      u64Rc6Data^=0xFFFFFFFF;
      u64Rc6Data &= u64Mask;
    }
    else {
      u8parseIndex = 1;
      while(u8parseIndex < u8MsbRc6NHalfBits)
      {
        firstHalfbit = (u64MsbRc6HalfBitData>>(u8MsbRc6NHalfBits-u8parseIndex))&0x1;
        u8parseIndex++;
        secondHalfbit = (u64MsbRc6HalfBitData>>(u8MsbRc6NHalfBits-u8parseIndex))&0x1;
        if((firstHalfbit==0) && (secondHalfbit==1))
        {//Negatif bit
            u64Rc6MsbData <<=1;
            u64Rc6MsbData |= 0x0UL;
            u8Rc6MsbBits++;
            u64Mask<<=1;
            u64Mask |= 0x1UL;
            u8parseIndex++;
        }
        if((firstHalfbit==1) && (secondHalfbit==0))
        {//positif bit
            u64Rc6MsbData <<=1;
            u64Rc6MsbData |= 0x1UL;
            u8Rc6MsbBits++;
            u64Mask<<=1;
            u64Mask |= 0x1UL;
            u8parseIndex++;
        }
        if((firstHalfbit==0) && (secondHalfbit==0))
        {//toggle bit
            u64Rc6MsbData <<=1;
            u64Rc6MsbData |= 0x0UL;
            u8Rc6MsbBits++;
            u64Mask<<=1;
            u64Mask |= 0x1UL;
            u8parseIndex+=3;
        }
        if((firstHalfbit==1) && (secondHalfbit==1))
        {//toggle bit
            u64Rc6MsbData <<=1;
            u64Rc6MsbData |= 0x1UL;
            u8Rc6MsbBits++;
            u64Mask<<=1;
            u64Mask |= 0x1UL;
            u8parseIndex+=3;
        }
      }
      u64Rc6MsbData^=0xFFFFFFFF;
      u64Rc6MsbData &= u64Mask;

      u8parseIndex = 1;
      u64Mask = 0;
      while(u8parseIndex < u8Rc6NHalfBits)
      {
        firstHalfbit = (u64Rc6HalfBitData>>(u8Rc6NHalfBits-u8parseIndex))&0x1;
        u8parseIndex++;
        secondHalfbit = (u64Rc6HalfBitData>>(u8Rc6NHalfBits-u8parseIndex))&0x1;
        if((firstHalfbit==0) && (secondHalfbit==1))
        {//Negatif bit
            u64Rc6LsbData <<=1;
            u64Rc6LsbData |= 0x0UL;
            u8Rc6LsbBits++;
            u64Mask<<=1;
            u64Mask |= 0x1UL;
            u8parseIndex++;
        }
        if((firstHalfbit==1) && (secondHalfbit==0))
        {//positif bit
            u64Rc6LsbData <<=1;
            u64Rc6LsbData |= 0x1UL;
            u8Rc6LsbBits++;
            u64Mask<<=1;
            u64Mask |= 0x1UL;
            u8parseIndex++;
        }
        if((firstHalfbit==0) && (secondHalfbit==0))
        {//toggle bit
            u64Rc6LsbData <<=1;
            u64Rc6LsbData |= 0x0UL;
            u8Rc6LsbBits++;
            u64Mask<<=1;
            u64Mask |= 0x1UL;
            u8parseIndex+=3;
        }
        if((firstHalfbit==1) && (secondHalfbit==1))
        {//toggle bit
            u64Rc6LsbData <<=1;
            u64Rc6LsbData |= 0x1UL;
            u8Rc6LsbBits++;
            u64Mask<<=1;
            u64Mask |= 0x1UL;
            u8parseIndex+=3;
        }
      }
      u64Rc6LsbData^=0xFFFFFFFF;
      u64Rc6LsbData &= u64Mask;

      u8Rc6Bits = u8Rc6MsbBits+u8Rc6LsbBits;
      u64Rc6Data = u64Rc6MsbData;
      u64Rc6Data<<=u8Rc6LsbBits;
      u64Rc6Data |= u64Rc6LsbData;
    }

    if(u8Rc6Bits >= IR_RC6_MODE_0_NBITS)
    {
        _GetRc6Mode(u64Rc6Data, u8Rc6Bits, &u8Rc6Mode);
        if(u8Rc6Mode == IR_RC6_MODE_0)
        {
            bResult =  _RawDataFormatRc6(u64Rc6Data, u8Rc6Bits, pu32CustCode, pu16KeyCode, pu8State, pu8Reserved);
            _u64LastRc6RawData = u64Rc6Data;
            bValidDataEnd = FALSE;
            if(bResult == FALSE)
                return E_IR_DECODE_ERR;
            else
                return E_IR_DECODE_DATA_OK;
        }

        if(u8Rc6Mode == IR_RC6_MODE_6)
        {
           if(u8Rc6Bits < IR_RC6_MODE_6A_SCC_NBITS)
           {
               return E_IR_DECODE_DATA_SHORTAGE;
           }
           if(u8Rc6Bits >= IR_RC6_MODE_6A_SCC_NBITS)
           {
               bResult =  _RawDataFormatRc6(u64Rc6Data, u8Rc6Bits, pu32CustCode, pu16KeyCode, pu8State, pu8Reserved);
               _u64LastRc6RawData = u64Rc6Data;
               bValidDataEnd = FALSE;
               if(bResult == FALSE)
                   return E_IR_DECODE_ERR;
               else
                   return E_IR_DECODE_DATA_OK;
           }
        }
    }
    return E_IR_DECODE_DATA_SHORTAGE;
}

DRV_IR_PROTOCOL_TYPE GET_IR_PROTOCOL_ENTRY(E_IR_PROTOCOL_RC6) =
{
    .name               ="IR_PROTOCOL_RC6",
    .etype              =E_IR_PROTOCOL_RC6,
    .findleadcode       =_ParseLeadCodeRc6,
    .parseprotocol      =_ParseRC6,
    .u8LeadCodeMinCount = IR_RC6_LEADCODE_SHOT_COUNT,
    .u32Timeout         = IR_TIME_UPD(IR_TIME_RC6_MODE_6A_SCC_TIMEOUT, IR_TIME_RC6_TOLERENCE),
};
