//<MStar Software>
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2010 - 2012 Mstar Semiconductor, Inc.
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
/// file    mdrv_ldm_parse.h
/// @brief  local dimming Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_LD_DMA_H_
#define _MDRV_LD_DMA_H_

#include "mhal_ldm_dma.h"


#ifdef _DRVLDDMA_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif



INTERFACE MS_U16 MDrv_LD_ConvertLumaData2SPIData(MS_U16* pu16Addr, MS_U16 u16LEDNum);

INTERFACE MS_U8 MDrv_LDM_DMA_SetMenuloadNumber(LDMA_CH_e eCH, MS_U32 u32MenuldNum );

INTERFACE MS_U8 MDrv_LDM_DMA_SetSPICommandFormat( LDMA_CH_e eCH,MS_U8 u8CmdLen, MS_U16* pu16CmdBuf );

INTERFACE MS_U8 MDrv_LDMA_SetCheckSumMode(LDMA_CH_e eCH,LDMA_CheckSumMode_e eSumMode);

INTERFACE MS_U8 MDrv_LDM_DMA_SetSpiTriggerMode( LDMA_CH_e eCH, LDMA_SPITriggerMode_e eTriggerMode );

INTERFACE MS_U8 MDrv_LDMA_SetTrigDelay(LDMA_CH_e eCH,LDMA_TrigDelay_st *pstTrigDelay);

INTERFACE MS_U8 MDrv_LDMA_EnableCS(LDMA_CH_e eCH, MS_BOOL bEnable);

INTERFACE MS_U8  MDrv_LDM_DMA_Init(LDMA_CH_e eCH);

INTERFACE MS_U8 MDrv_LDMA_SetSPI16BitMode(LDMA_CH_e eCH, MS_BOOL bEnable);


#endif


