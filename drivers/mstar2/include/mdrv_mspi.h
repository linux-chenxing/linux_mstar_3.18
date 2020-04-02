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

///////////////////////////////////////////////////////////////////////////////////////////////////
/// file    mdrv_mspi.h
/// @brief MSPI drv Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_MSPI_H_
#define _MDRV_MSPI_H_


#include "mdrv_mstypes.h"
#include "mhal_mspi.h"
#include "reg_mspi.h"

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------



MS_BOOL MDrv_MSPI_SetDbgLevel(MS_U8 u8DbgLevel);

MSPI_ErrorNo MDrv_MSPI_Init(MSPI_CH eChannel);
MSPI_ErrorNo MDrv_MSPI_Read(MSPI_CH eChannel, MS_U8 *pData, MS_U16 u16Size);
MSPI_ErrorNo MDrv_MSPI_Write(MSPI_CH eChannel, MS_U8 *pData, MS_U16 u16Size);
MSPI_ErrorNo MDrv_MSPI_SetReadBufferSize(MSPI_CH eChannel,  MS_U8 u8Size);
MSPI_ErrorNo MDrv_MSPI_SetWriteBufferSize(MSPI_CH eChannel,  MS_U8 u8Size);
MS_U32  MDrv_MSPI_Read_Write(MSPI_CH eChannel,MS_U8 *pReadData,MS_U8 *pWriteData, MS_U16 u8WriteSiz);
MSPI_ErrorNo MDrv_MSPI_DCConfig(MSPI_CH eChannel, MSPI_DCConfig *ptDCConfig);
MSPI_ErrorNo MDrv_MSPI_SetMode(MSPI_CH eChannel, MSPI_Mode_Config_e eMode);
MSPI_ErrorNo MDrv_MSPI_SetCLK(MSPI_CH eChannel, MS_U8 U8Clock);
MSPI_ErrorNo MDrv_MSPI_SetCLKByINI(MSPI_CH eChannel, MS_U32 u32Clock);

MSPI_ErrorNo MDrv_MSPI_FRAMEConfig(MSPI_CH eChannel, MSPI_FrameConfig *ptFrameConfig);
void MDrv_MSPI_SlaveEnable(MSPI_CH eChannel, MS_BOOL Enable,MSPI_ChipSelect_e eCS);
MS_U32 MDrv_MSPI_SetPowerState(void);

#endif
