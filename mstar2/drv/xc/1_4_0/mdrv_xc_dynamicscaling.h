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

#ifndef _MDRV_XC_DYNAMICSCALING_H_
#define _MDRV_XC_DYNAMICSCALING_H_

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

#ifdef _MDRV_XC_DYNAMICSCALING_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif

INTERFACE E_K_APIXC_ReturnValue KApi_XC_Get_DSForceIndexSupported(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW  eWindow);
INTERFACE E_K_APIXC_ReturnValue KApi_XC_Set_DSIndexSourceSelect(EN_KDRV_SC_DEVICE u32DeviceID, E_K_XC_DS_INDEX_SOURCE eDSIdxSrc, EN_KDRV_WINDOW  eWindow);
INTERFACE E_K_APIXC_ReturnValue KApi_XC_Set_DSForceIndex(EN_KDRV_SC_DEVICE u32DeviceID, MS_BOOL bEnable, MS_U8 u8Index, EN_KDRV_WINDOW  eWindow);
INTERFACE MS_BOOL KApi_XC_SetDynamicScaling(EN_KDRV_SC_DEVICE u32DeviceID, K_XC_DynamicScaling_Info *pstDSInfo,MS_U32 u32DSInfoLen, EN_KDRV_WINDOW  eWindow);
INTERFACE void KApi_XC_Set_DynamicScalingFlag(EN_KDRV_SC_DEVICE u32DeviceID,MS_BOOL bEnable);
INTERFACE MS_BOOL KDrv_XC_GetDynamicScalingStatus(EN_KDRV_SC_DEVICE u32DeviceID);
INTERFACE MS_BOOL KDrv_XC_EnableIPMTuneAfterDS(EN_KDRV_SC_DEVICE u32DeviceID, MS_BOOL bEnable);
INTERFACE void KApi_XC_WriteSWDSCommand(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW eWindow, E_DS_CLIENT client, MS_U32 u32CmdRegAddr, MS_U16 u16CmdRegValue, k_ds_reg_ip_op_sel IPOP_Sel, k_ds_reg_source_sel Source_Select, K_XC_DS_CMDCNT *pstXC_DS_CmdCnt);
INTERFACE void KApi_XC_Add_NullCommand(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW eWindow, E_DS_CLIENT client, k_ds_reg_ip_op_sel IPOP_Sel, K_XC_DS_CMDCNT *pstXC_DS_CmdCnt);
INTERFACE MS_BOOL KApi_DS_set_client(EN_KDRV_SC_DEVICE u32DeviceID,E_DS_CLIENT client,MS_U32 max_num);
INTERFACE void KApi_XC_WriteSWDSCommandNonXC(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW eWindow, E_DS_CLIENT client, MS_U32 u32Bank, MS_U16 u32Addr, MS_U32 u16Data, MS_U16 u16Mask, k_ds_reg_ip_op_sel IPOP_Sel, K_XC_DS_CMDCNT *pstXC_DS_CmdCnt);
INTERFACE void KApi_XC_WriteSWDSCommand_Mask(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW eWindow, E_DS_CLIENT client, MS_U32 u32CmdRegAddr, MS_U16 u16CmdRegValue, k_ds_reg_ip_op_sel IPOP_Sel, k_ds_reg_source_sel Source_Select, K_XC_DS_CMDCNT *pstXC_DS_CmdCnt,MS_U16 u16Mask);
INTERFACE MS_BOOL KApi_XC_GetDynamicScalingIndex(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW eWindow, MS_U8* pu8DSIndex);
INTERFACE MS_BOOL KApi_XC_FireDynamicScalingIndex(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW eWindow);
INTERFACE MS_U32 KApi_DS_get_support_index_num(void);
#undef INTERFACE
#endif //_MDRV_SC_DYNAMICSCALING_H_

