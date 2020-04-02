//<MStar Software>
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2010 - 2012 MStar Semiconductor, Inc.
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

#ifndef _HAL_PCMCIA_C_
#define _HAL_PCMCIA_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition
//#include "MsCommon.h"
#include "drvPCMCIA.h"

// Internal Definition
//#include "regCHIP.h"
#include "regPCMCIA.h"
#include "halPCMCIA.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
// Address bus of RIU is 16 bits.
#define PCMCIA_READ_WORD(addr)         READ_WORD(PCMCIA_BASE_ADDRESS + ((addr)<<2))

#define PCMCIA_WRITE_WORD(addr, val)   WRITE_WORD((PCMCIA_BASE_ADDRESS + ((addr)<<2)), (val))

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
u32 u32PCMCIA_RIU_BaseAdd;

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
#define WRITE_BYTE(_reg, _val)      { (*((volatile u8*)(_reg))) = (u8)(_val); }
#define READ_BYTE(_reg)             (*(volatile u8*)(_reg))

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
// Put this function here because hwreg_utility2 only for hal.
void HAL_PCMCIA_Set_RIU_base(u32 u32RIU_base)
{
    u32PCMCIA_RIU_BaseAdd = u32RIU_base;
}

void HAL_PCMCIA_Write_Byte(u32 u32Addr, u8 u8Val)
{
    WRITE_BYTE(PCMCIA_BASE_ADDRESS+((u32Addr<<1)-(u32Addr&0x01)), u8Val);
}

u8 HAL_PCMCIA_Read_Byte(u32 u32Addr)
{
    return READ_BYTE(PCMCIA_BASE_ADDRESS+((u32Addr<<1)-(u32Addr&0x01)));    
}

bool HAL_PCMCIA_GetIntStatus(ISR_STS* isr_status)
{
    u8 u8Tmp = 0;
    u8Tmp = HAL_PCMCIA_Read_Byte(REG_PCMCIA_STAT_INT_RAW_INT1);
    if(u8Tmp & 0x01)
    {
        isr_status->bCardAInsert = true;
    }
    if(u8Tmp & 0x02)
    {
        isr_status->bCardARemove = true;
    }
    if(u8Tmp & 0x04)
    {
        isr_status->bCardAData = true;
    }
    if(u8Tmp & 0x08)
    {
        isr_status->bCardBInsert = true;
    }
    if(u8Tmp & 0x10)
    {
        isr_status->bCardBRemove = true;
    }
    if(u8Tmp & 0x20)
    {
        isr_status->bCardBData = true;
    }        
    
    
    return true;
}

void   HAL_PCMCIA_ClrInt(u32 bits)
{
    u8 u8Tmp = 0;
    u8Tmp = (0x1 << 0) | // cardA insert
            (0x1 << 1) | // cardA remove
            (0x1 << 2) | // IRQ from cardA
            (0x1 << 3) | // cardB insert
            (0x1 << 4) | // cardB remove
            (0x1 << 5) | // IRQ from cardB
            (0x1 << 6) | // timeout from cardA
            (0x1 << 7) ; // timeout from cardA
     
    HAL_PCMCIA_Write_Byte(REG_PCMCIA_INT_MASK_CLEAR1, u8Tmp);        
    u8Tmp = 0;
    HAL_PCMCIA_Write_Byte(REG_PCMCIA_INT_MASK_CLEAR1, u8Tmp);            
}

void HAL_PCMCIA_MaskInt(u32 bits, bool bMask)
{
    u8 u8Tmp = 0;
    
    if(bMask == true)
    {
        u8Tmp = (0x1 << 0) | // cardA insert
                (0x1 << 1) | // cardA remove
                (0x1 << 2) | // IRQ from cardA
                (0x1 << 3) | // cardB insert            
                (0x1 << 4) | // cardB remove
                (0x1 << 5) | // IRQ from cardB
                (0x1 << 6) | // timeout from cardA
                (0x1 << 7) ; // timeout from cardB
    }
    else // unmask
    {
        u8Tmp = 0x0;
    }
    HAL_PCMCIA_Write_Byte(REG_PCMCIA_INT_MASK_CLEAR, u8Tmp);        
}

#endif
