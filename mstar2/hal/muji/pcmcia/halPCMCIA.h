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

#ifndef _HAL_PCMCIA_H_
#define _HAL_PCMCIA_H_
#include <linux/kernel.h>
#include "regPCMCIA.h"

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
extern u32 u32PCMCIA_RIU_BaseAdd;

#define RIU_READ_BYTE(Addr)         ( READ_BYTE( u32PCMCIA_RIU_BaseAdd + (Addr) ) )
#define RIU_READ_2BYTE(Addr)        ( READ_WORD( u32PCMCIA_RIU_BaseAdd + (Addr) ) )
#define RIU_WRITE_BYTE(Addr, Val)   { WRITE_BYTE( u32PCMCIA_RIU_BaseAdd + (Addr), Val) }
#define RIU_WRITE_2BYTE(Addr, Val)  { WRITE_WORD( u32PCMCIA_RIU_BaseAdd + (Addr), Val) }

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
typedef struct {
   bool bCardAInsert;
   bool bCardARemove;
   bool bCardAData;   
   bool bCardBInsert;
   bool bCardBRemove;
   bool bCardBData;   
} ISR_STS;

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
void HAL_PCMCIA_Set_RIU_base(u32 u32RIU_base);
u8 HAL_PCMCIA_Read_Byte(u32 u32Addr);
void HAL_PCMCIA_Write_Byte(u32 u32Addr, u8 u8Val);

bool HAL_PCMCIA_GetIntStatus(ISR_STS*);
void HAL_PCMCIA_ClrInt(u32);
void HAL_PCMCIA_MaskInt(u32, bool);


#endif // _HAL_PCMCIA_H_

