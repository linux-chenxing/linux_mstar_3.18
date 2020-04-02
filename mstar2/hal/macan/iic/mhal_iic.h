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

#ifndef _HAL_IIC_H_
#define _HAL_IIC_H_

#include <asm/types.h>
#include "mdrv_types.h"

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
extern void MHal_IIC_Init(void);
extern void MHal_IIC_Clock_Select(U8 u8ClockIIC);
extern void MHal_IIC_Start(void);
extern void MHal_IIC_Stop(void);
extern void MHal_IIC_NoAck(void);
extern B16 MHal_IIC_SendData(U8 u8DataIIC);
extern B16 MHal_IIC_SendByte(U8 u8DataIIC);
extern B16 MHal_IIC_GetByte(U8* pu8DataIIC);

#endif // _HAL_IIC_H_

