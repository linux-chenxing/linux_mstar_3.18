///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2006 - 2008 MStar Semiconductor, Inc.
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
///
/// @file   mdrv_temp.h
/// @brief  TEMP Driver Interface
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_XC_H_
#define _MDRV_XC_H_

#include <linux/fs.h>
#include <linux/cdev.h>
#include "mdrv_types.h"
#include <linux/version.h>
#ifdef CONFIG_MSTAR_XC_HDR_SUPPORT
#include "mdrv_xc_st.h"
#include "mhal_xc.h"
#endif
//-------------------------------------------------------------------------------------------------
//  Driver Capability
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------


// #define SC_DEBUG



//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
irqreturn_t MDrv_XC_IntHandler(int irq,void *devid);

INTERFACE void MDrv_XC_FilmDriverHWVer1(void);
INTERFACE void MDrv_XC_FilmDriverHWVer2(void);
#ifdef CONFIG_MSTAR_XC_HDR_SUPPORT
INTERFACE void MDrv_XC_ProcessCFDIRQ(void);
INTERFACE MS_BOOL MDrv_XC_SetHDRType(EN_KDRV_XC_HDR_TYPE enHDRType);
INTERFACE MS_BOOL MDrv_XC_GetHDRType(EN_KDRV_XC_HDR_TYPE *penHDRType);
INTERFACE MS_BOOL MDrv_XC_SetInputSourceType(EN_KDRV_XC_INPUT_SOURCE_TYPE enInputSourceType);
INTERFACE MS_BOOL MDrv_XC_GetInputSourceType(EN_KDRV_XC_INPUT_SOURCE_TYPE *penInputSourceType);
INTERFACE MS_BOOL MDrv_XC_SetOpenMetadataInfo(ST_KDRV_XC_OPEN_METADATA_INFO *pstMetadataInfo);
INTERFACE MS_BOOL MDrv_XC_Set3DLutInfo(ST_KDRV_XC_3DLUT_INFO *pst3DLutInfo);
INTERFACE MS_BOOL MDrv_XC_GetShareMemInfo(ST_KDRV_XC_SHARE_MEMORY_INFO *pstShmemInfo);
INTERFACE MS_BOOL MDrv_XC_SetShareMemInfo(ST_KDRV_XC_SHARE_MEMORY_INFO *pstShmemInfo);
INTERFACE MS_BOOL MDrv_XC_EnableHDR(MS_BOOL bEnableHDR);
INTERFACE MS_BOOL MDrv_XC_SetHDRWindow(ST_KDRV_XC_WINDOW_INFO *pstWindowInfo);
INTERFACE MS_BOOL MDrv_XC_ConfigAutoDownload(ST_KDRV_XC_AUTODOWNLOAD_CONFIG_INFO *pstConfigInfo);
INTERFACE MS_BOOL MDrv_XC_WriteAutoDownload(ST_KDRV_XC_AUTODOWNLOAD_DATA_INFO *pstDataInfo);
INTERFACE MS_BOOL MDrv_XC_FireAutoDownload(EN_KDRV_XC_AUTODOWNLOAD_CLIENT enClient);
INTERFACE MS_BOOL KDrv_XC_GetAutoDownloadCaps(ST_KDRV_XC_AUTODOWNLOAD_CLIENT_SUPPORTED_CAPS *pstAutoDownlaodCaps);
INTERFACE MS_BOOL MDrv_XC_SetColorFormat(EN_KDRV_XC_HDR_COLOR_FORMAT enColorFormat);
INTERFACE MS_BOOL MDrv_XC_CFDControl(ST_KDRV_XC_CFD_CONTROL_INFO *pstKdrvCFDCtrlInfo);
INTERFACE MS_BOOL MDrv_XC_SetDSHDRInfo(ST_KDRV_XC_DS_HDRInfo *pstDSHDRInfo);
INTERFACE MS_BOOL MDrv_XC_Init(void);
INTERFACE MS_BOOL MDrv_XC_Exit(void);
#endif
INTERFACE int MDrv_XC_Suspend(void);
INTERFACE int MDrv_XC_Resume(void);
#endif // _MDRV_TEMP_H_

