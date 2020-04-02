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


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "IR_PROTOCOL_Common.h"
#ifdef SUPPORT_MULTI_PROTOCOL
//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

#define SHOTLIST_EMPTY()    (_u8ShotHeadIdx==_u8ShotTailIdx)
#define SHOTLIST_FULL()     ((_u8ShotHeadIdx+1)%SHOT_BUF_MAX ==_u8ShotTailIdx)

#ifndef NULL
#define NULL 0
#endif
//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
U8 _u8ShotHeadIdx=0;
U8 _u8ShotTailIdx=0;
U32 _u32ShotValue[SHOT_BUF_MAX];
BOOL _bNegShot[SHOT_BUF_MAX];

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
U8 _MulProtCommon_GetShotDataSize(void)
{
    if(_u8ShotHeadIdx>=_u8ShotTailIdx)
        return _u8ShotHeadIdx- _u8ShotTailIdx;
    else
        return (SHOT_BUF_MAX-_u8ShotTailIdx)+_u8ShotHeadIdx;
}

BOOL _MulProtCommon_PeekShot(U8 u8Offset, U8 u8Count, U32 *pu32Value, BOOL *pbNegtive)
{
    U8 u8Index;
    if(pu32Value==NULL||pbNegtive==NULL)
    {
        //printk("Null parameter!\n");
        return FALSE;
    }

    if((u8Offset+u8Count)>_MulProtCommon_GetShotDataSize())//peek data out of bound
    {
        return FALSE;
    }
    else
    {
        for(u8Index=u8Offset; u8Index< u8Offset+u8Count; u8Index++)
        {
            if((_u8ShotTailIdx+u8Index)>=SHOT_BUF_MAX)
            {
                pu32Value[u8Index-u8Offset] =_u32ShotValue[(_u8ShotTailIdx+u8Index)%SHOT_BUF_MAX];
                pbNegtive[u8Index-u8Offset] = _bNegShot[(_u8ShotTailIdx+u8Index)%SHOT_BUF_MAX];
            }
            else
            {
                pu32Value[u8Index-u8Offset] =_u32ShotValue[_u8ShotTailIdx+u8Index];
                pbNegtive[u8Index-u8Offset] = _bNegShot[_u8ShotTailIdx+u8Index];
            }
        }
        return TRUE;
    }
}

U8 _MulProtCommon_LSB2MSB(U8 u8OrgData)
{
    U8 u8Index;
    U8 u8NewData=0;

    for(u8Index=0; u8Index<8; u8Index++)
    {
        u8NewData <<=1;
        u8NewData = u8NewData|(u8OrgData&0x1UL);
        u8OrgData >>=1;
    }

    return u8NewData;
}

void _Mdrv_MulProtCommon_ShotDataReset(void)
{
    _u8ShotHeadIdx=0;
    _u8ShotTailIdx=0;
}

BOOL _Mdrv_MulProtCommon_AddShot(U32 u32Value, BOOL bNegtive)
{
    if(SHOTLIST_FULL())
    {
        printk("Error buffer full\n");
        return FALSE;
    }
    else
    {
        _u32ShotValue[_u8ShotHeadIdx]= u32Value;
        _bNegShot[_u8ShotHeadIdx] = bNegtive;
        _u8ShotHeadIdx= (_u8ShotHeadIdx+1)%SHOT_BUF_MAX;
        return TRUE;
    }
}

void _MulProtCommon_dumpShotData(void)
{
    printk("-----dump data----\n");
    if(!SHOTLIST_EMPTY())
    {
        U8 u8Index=0;
        U32 u32ShotValue;
        BOOL bNegShot;
        while(_MulProtCommon_PeekShot(u8Index, 1, &u32ShotValue, &bNegShot)==TRUE)
        {
            if(bNegShot==TRUE)
                printk("N[%d]: %ld\n", u8Index, u32ShotValue);
            else
                printk("P[%d]: %ld\n", u8Index, u32ShotValue);

            u8Index++;
        }
    }
    printk("-----dump end----\n");
}
#endif
