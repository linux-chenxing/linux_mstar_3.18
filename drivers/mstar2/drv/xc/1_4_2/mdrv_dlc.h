///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2008 - 2009 MStar Semiconductor, Inc.
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
//////////////////////////////////////////////////////////////////////////////////////////////////
//
// @file   mdrv_dlc.h
// @brief  DLC driver interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_DLC_H
#define _MDRV_DLC_H

#include "mdrv_xc_st.h"
#ifdef _MDRV_DLC_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

#if defined(__cplusplus)
extern "C" {
#endif

INTERFACE void MDrv_DLC_SetBlePoint(void);
MS_BOOL MDrv_DLC_init(void);
INTERFACE MS_BOOL MDrv_DLC_Ini(char *con_file ,char *con_fileUpdate);
INTERFACE void MDrv_DLC_ReadInfo(char *buf);
INTERFACE void MDrv_DLC_SetCurve(void);
INTERFACE void MDrv_DLC_CopyInitInfo(void);
INTERFACE void MDrv_DLC_ReadCurveInfo(char *buf);
INTERFACE MS_BOOL MDrv_DLC_SetBleOnOffInfo(MS_BOOL bSetBleOn );
INTERFACE MS_U8 MDrv_DLC_GetCGain( BOOL bWindow);
INTERFACE MS_BOOL MDrv_DLC_SetChangeCurveInfo(ST_KDRV_XC_DLC_INFO *pstDlcInfo);
INTERFACE MS_BOOL KDrv_DLC_SetInitInfo(ST_KDRV_XC_DLC_INIT_INFO *pstDlcInitInfo);
INTERFACE MS_BOOL KDrv_DLC_SetTMOInfo(ST_KDRV_XC_TMO_INFO *pstTMOInfo);
INTERFACE MS_BOOL MDrv_DLC_SetOnOffInfo(MS_BOOL bSetDlcOn );
INTERFACE MS_BOOL MDrv_DLC_SetBlePointChangeInfo(ST_KDRV_XC_DLC_BLE_INFO *pstBleInfo);
INTERFACE MS_BOOL MDrv_DLC_GetHistogram32Info(MS_U16 *pu16Histogram);
INTERFACE MS_U8 MDrv_DLC_GetAverageValue(BOOL bWindow);
INTERFACE void MDrv_DLC_SetYGain(MS_U8 u8YGain, BOOL bWindow);
INTERFACE MS_U8 MDrv_DLC_GetYGain( BOOL bWindow);
INTERFACE void MDrv_DLC_SetCGain(MS_U8 u8CGain, BOOL bWindow);
INTERFACE void MDrv_DLC_ProcessIRQ(void);
#if defined(__cplusplus)
}
#endif

#undef INTERFACE
#endif //_MDRV_DLC_H