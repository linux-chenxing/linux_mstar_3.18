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
//==============================================================================
// [mdrv_sc_dynamicscaling.c]
// Date: 2012/10/29
// Descriptions: dynamic scaling related functions
//==============================================================================
#ifndef  _MDRV_XC_DYNAMICSCALING_C_
#define  _MDRV_XC_DYNAMICSCALING_C_

// Common Definition
#include <linux/string.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/slab.h>

#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/wait.h>
#include <linux/types.h>
#include <linux/semaphore.h>
#include <linux/hrtimer.h>

#include "mdrv_mstypes.h"
#include "mdrv_xc_st.h"
#include "mdrv_xc_dynamicscaling.h"
#include "mhal_xc.h"
#ifdef SUPPORT_KERNEL_MLOAD
#include "mhal_xc_chip_config.h"
#include "mhal_menuload.h"
#endif
#ifdef SUPPORT_KERNEL_DS
#include "mhal_dynamicscaling.h"
#endif

static MS_BOOL _bDSClientInited = FALSE;
E_K_APIXC_ReturnValue KDrv_XC_Get_DSForceIndexSupported(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW  eWindow);
E_K_APIXC_ReturnValue KDrv_XC_Set_DSIndexSourceSelect(EN_KDRV_SC_DEVICE u32DeviceID, E_K_XC_DS_INDEX_SOURCE eDSIdxSrc, EN_KDRV_WINDOW  eWindow);
E_K_APIXC_ReturnValue KDrv_XC_Set_DSForceIndex(EN_KDRV_SC_DEVICE u32DeviceID, MS_BOOL bEnable, MS_U8 u8Index, EN_KDRV_WINDOW  eWindow);
MS_BOOL KDrv_XC_Set_DynamicScaling(EN_KDRV_SC_DEVICE u32DeviceID, MS_PHY u32MemBaseAddr, MS_U8 u8MIU_Select, MS_U8 u8IdxDepth, MS_BOOL bOP_On, MS_BOOL bIPS_On, MS_BOOL bIPM_On, MS_U32 u32DSBufferSize, MS_BOOL bEnable_ForceP, EN_KDRV_WINDOW eWindow);

E_K_APIXC_ReturnValue KDrv_XC_Get_DSForceIndexSupported(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW  eWindow)
{
    MS_BOOL bReturn = FALSE;
#ifdef SUPPORT_KERNEL_DS
    bReturn = KHal_XC_Get_DSForceIndexSupported(u32DeviceID, eWindow);
#endif
    return bReturn?E_K_APIXC_RET_OK:E_K_APIXC_RET_FAIL;
}

E_K_APIXC_ReturnValue KDrv_XC_Set_DSIndexSourceSelect(EN_KDRV_SC_DEVICE u32DeviceID, E_K_XC_DS_INDEX_SOURCE eDSIdxSrc, EN_KDRV_WINDOW  eWindow)
{
#ifdef SUPPORT_KERNEL_DS
    KHal_XC_Set_DSIndexSourceSelect(u32DeviceID, eDSIdxSrc, eWindow);
#endif

    return E_K_APIXC_RET_OK;
}

E_K_APIXC_ReturnValue KDrv_XC_Set_DSForceIndex(EN_KDRV_SC_DEVICE u32DeviceID, MS_BOOL bEnable, MS_U8 u8Index, EN_KDRV_WINDOW  eWindow)
{
#ifdef SUPPORT_KERNEL_DS
    KHal_XC_Set_DSForceIndex(u32DeviceID, bEnable, u8Index, eWindow);
#endif

    return E_K_APIXC_RET_OK;
}

E_K_APIXC_ReturnValue KApi_XC_Get_DSForceIndexSupported(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW  eWindow)
{
#ifdef SUPPORT_KERNEL_DS
    return KDrv_XC_Get_DSForceIndexSupported(u32DeviceID, eWindow);
#else
    return E_K_APIXC_RET_FAIL;
#endif
}


E_K_APIXC_ReturnValue KApi_XC_Set_DSIndexSourceSelect(EN_KDRV_SC_DEVICE u32DeviceID, E_K_XC_DS_INDEX_SOURCE eDSIdxSrc, EN_KDRV_WINDOW  eWindow)
{
    E_K_APIXC_ReturnValue eReturn = E_K_APIXC_RET_FAIL;
#ifdef SUPPORT_KERNEL_DS
    eReturn = KDrv_XC_Set_DSIndexSourceSelect(u32DeviceID, eDSIdxSrc, eWindow);
#endif
    return eReturn;
}


E_K_APIXC_ReturnValue KApi_XC_Set_DSForceIndex(EN_KDRV_SC_DEVICE u32DeviceID, MS_BOOL bEnable, MS_U8 u8Index, EN_KDRV_WINDOW  eWindow)
{
    E_K_APIXC_ReturnValue eReturn = E_K_APIXC_RET_FAIL;
#ifdef SUPPORT_KERNEL_DS
    eReturn = KDrv_XC_Set_DSForceIndex(u32DeviceID, bEnable, u8Index, eWindow);
#endif
    return eReturn;
}



MS_BOOL KDrv_XC_GetDynamicScalingStatus(EN_KDRV_SC_DEVICE u32DeviceID)
{
#ifdef SUPPORT_KERNEL_DS
    return KHAL_SC_Get_DynamicScaling_Status(u32DeviceID);
#else
    return FALSE;
#endif
}

MS_BOOL KDrv_XC_EnableIPMTuneAfterDS(EN_KDRV_SC_DEVICE u32DeviceID, MS_BOOL bEnable)
{
#ifdef SUPPORT_KERNEL_DS
    return KHAL_SC_Enable_IPMTuneAfterDS(u32DeviceID, bEnable);
#else
    return FALSE;
#endif
}

extern MS_BOOL ll_delete_all(void);
//-------------------------------------------------------------------------------------------------
/// Set Dynamic Scaling
/// @param  pstDSInfo              \b IN: the information of Dynamic Scaling
/// @param  u32DSInforLen          \b IN: the length of the pstDSInfo
/// @param  eWindow                \b IN: which window we are going to set
/// @return @ref MS_BOOL
//-------------------------------------------------------------------------------------------------
MS_BOOL KApi_XC_SetDynamicScaling(EN_KDRV_SC_DEVICE u32DeviceID, K_XC_DynamicScaling_Info *pstDSInfo,MS_U32 u32DSInfoLen, EN_KDRV_WINDOW  eWindow)
{
    MS_BOOL bStatus = FALSE;
#ifdef SUPPORT_KERNEL_DS
    printf("\033[1;32m[%s:%d]u32DeviceID=%d , u32DSInfoLen = %d, eWindow =%d\033[m\n",__FUNCTION__,__LINE__,u32DeviceID,u32DSInfoLen,eWindow);
    printf("\033[1;32m[%s:%d]0x%x,%d,%d,%d,%d,%d,0x%x\033[m\n",__FUNCTION__,__LINE__,
        (MS_U32)pstDSInfo->u64DS_Info_BaseAddr,
        pstDSInfo->u8MIU_Select,
        pstDSInfo->u8DS_Index_Depth,
        pstDSInfo->bOP_DS_On,
        pstDSInfo->bIPS_DS_On,
        pstDSInfo->bIPM_DS_On,
        pstDSInfo->u32DSBufferSize,
        pstDSInfo->bEnable_ForceP);
    if(u32DSInfoLen != sizeof(K_XC_DynamicScaling_Info))
    {
        return FALSE;
    }

    bStatus = KDrv_XC_Set_DynamicScaling(
        u32DeviceID,
        pstDSInfo->u64DS_Info_BaseAddr,
        pstDSInfo->u8MIU_Select,
        pstDSInfo->u8DS_Index_Depth,
        pstDSInfo->bOP_DS_On,
        pstDSInfo->bIPS_DS_On,
        pstDSInfo->bIPM_DS_On,
        pstDSInfo->u32DSBufferSize,
        pstDSInfo->bEnable_ForceP,
        eWindow);
    printf("\033[1;32m[%s:%d]\033[m\n",__FUNCTION__,__LINE__);

    if ((_bDSClientInited == FALSE) && (pstDSInfo->u64DS_Info_BaseAddr != 0)
        && (pstDSInfo->bOP_DS_On || pstDSInfo->bIPM_DS_On || pstDSInfo->bIPS_DS_On))
    {
        KApi_DS_set_client(u32DeviceID, E_DS_CLIENT_XC, 16 * (MS_MLOAD_BUS_WIDTH / MS_MLOAD_CMD_LEN_64BITS));
#if defined(CONFIG_MSTAR_XC_HDR_SUPPORT)
        KApi_DS_set_client(u32DeviceID, E_DS_CLIENT_HDR, 238 * (MS_MLOAD_BUS_WIDTH / MS_MLOAD_CMD_LEN_64BITS));
#endif
        _bDSClientInited = TRUE;
    }
    else if ((pstDSInfo->bOP_DS_On == FALSE) && (pstDSInfo->bIPM_DS_On == FALSE) && (pstDSInfo->bIPS_DS_On == FALSE))
    {
        ll_delete_all();
        _bDSClientInited = FALSE;
    }
#endif
    return bStatus;
}


MS_BOOL KDrv_XC_Set_DynamicScaling(EN_KDRV_SC_DEVICE u32DeviceID,MS_PHY u32MemBaseAddr, MS_U8 u8MIU_Select, MS_U8 u8IdxDepth, MS_BOOL bOP_On, MS_BOOL bIPS_On, MS_BOOL bIPM_On, MS_U32 u32DSBufferSize, MS_BOOL bEnable_ForceP, EN_KDRV_WINDOW eWindow)
{
#ifdef SUPPORT_KERNEL_DS
    KHal_XC_InitDynamicScalingIndex(u32DeviceID, eWindow);

    return KHAL_SC_Set_DynamicScaling(u32DeviceID,u32MemBaseAddr, u8MIU_Select,u8IdxDepth, bOP_On, bIPS_On, bIPM_On, u32DSBufferSize, bEnable_ForceP, eWindow);
#else
    return FALSE;
#endif
}

void KApi_XC_Set_DynamicScalingFlag(EN_KDRV_SC_DEVICE u32DeviceID,MS_BOOL bEnable)
{
#ifdef SUPPORT_KERNEL_DS
    KHAL_SC_Set_DynamicScalingFlag(u32DeviceID, bEnable);
#endif
}
void KApi_XC_WriteSWDSCommand(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW eWindow, E_DS_CLIENT client, MS_U32 u32CmdRegAddr, MS_U16 u16CmdRegValue, k_ds_reg_ip_op_sel IPOP_Sel, k_ds_reg_source_sel Source_Select, K_XC_DS_CMDCNT *pstXC_DS_CmdCnt)
{
#ifdef SUPPORT_KERNEL_DS
    MS_U8 u8DSIndex = 0;

    KHal_XC_GetDynamicScalingCurrentIndex(u32DeviceID, eWindow, &u8DSIndex);
    KHal_SC_WriteSWDSCommand(u32DeviceID,eWindow,client,u32CmdRegAddr,u16CmdRegValue,IPOP_Sel,Source_Select,pstXC_DS_CmdCnt,u8DSIndex);
#endif
}
void KApi_XC_Add_NullCommand(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW eWindow, E_DS_CLIENT client, k_ds_reg_ip_op_sel IPOP_Sel, K_XC_DS_CMDCNT *pstXC_DS_CmdCnt)
{
#ifdef SUPPORT_KERNEL_DS
    MS_U8 u8DSIndex = 0;

    KHal_XC_GetDynamicScalingCurrentIndex(u32DeviceID, eWindow, &u8DSIndex);
    KHal_SC_Add_NullCommand(u32DeviceID,eWindow,client,IPOP_Sel,pstXC_DS_CmdCnt,u8DSIndex);
#endif
}
MS_BOOL KApi_DS_set_client(EN_KDRV_SC_DEVICE u32DeviceID,E_DS_CLIENT client,MS_U32 max_num)
{
#ifdef SUPPORT_KERNEL_DS
    return KHal_DS_set_client(u32DeviceID,client,max_num);
#else
    return FALSE;
#endif
}
MS_U32 KApi_DS_get_support_index_num(void)
{
#ifdef SUPPORT_KERNEL_DS
    return DS_BUFFER_NUM_EX;
#else
    return 0;
#endif
}

void KApi_XC_WriteSWDSCommandNonXC(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW eWindow, E_DS_CLIENT client, MS_U32 u32Bank, MS_U16 u32Addr, MS_U32 u16Data, MS_U16 u16Mask, k_ds_reg_ip_op_sel IPOP_Sel, K_XC_DS_CMDCNT *pstXC_DS_CmdCnt)
{
#ifdef SUPPORT_KERNEL_DS
    MS_U8 u8DSIndex = 0;

    KHal_XC_GetDynamicScalingCurrentIndex(u32DeviceID, eWindow, &u8DSIndex);
    KHal_XC_WriteSWDSCommandNonXC(u32DeviceID, eWindow, client, u32Bank, u32Addr, u16Data, u16Mask, IPOP_Sel, pstXC_DS_CmdCnt, u8DSIndex);
#endif
}

void KApi_XC_WriteSWDSCommand_Mask(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW eWindow, E_DS_CLIENT client, MS_U32 u32CmdRegAddr, MS_U16 u16CmdRegValue, k_ds_reg_ip_op_sel IPOP_Sel, k_ds_reg_source_sel Source_Select, K_XC_DS_CMDCNT *pstXC_DS_CmdCnt,MS_U16 u16Mask)
{
#ifdef SUPPORT_KERNEL_DS
    MS_U8 u8DSIndex = 0;

    KHal_XC_GetDynamicScalingCurrentIndex(u32DeviceID, eWindow, &u8DSIndex);
    KHal_SC_WriteSWDSCommand_Mask(u32DeviceID,eWindow,client,u32CmdRegAddr,u16CmdRegValue,IPOP_Sel,Source_Select,pstXC_DS_CmdCnt,u8DSIndex,u16Mask);
#endif
}

MS_BOOL KApi_XC_GetDynamicScalingIndex(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW eWindow, MS_U8* pu8DSIndex)
{
#ifdef SUPPORT_KERNEL_DS
    return KHal_XC_GetDynamicScalingFireIndex(u32DeviceID, eWindow, pu8DSIndex);
#else
    return FALSE;
#endif
}

MS_BOOL KApi_XC_FireDynamicScalingIndex(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW eWindow)
{
#ifdef SUPPORT_KERNEL_DS
    return KHal_XC_FireDynamicScalingIndex(u32DeviceID, eWindow);
#else
    return FALSE;
#endif
}

EXPORT_SYMBOL(KApi_XC_Get_DSForceIndexSupported);
EXPORT_SYMBOL(KApi_XC_Set_DSIndexSourceSelect);
EXPORT_SYMBOL(KApi_XC_Set_DSForceIndex);
EXPORT_SYMBOL(KApi_XC_SetDynamicScaling);
EXPORT_SYMBOL(KApi_XC_Set_DynamicScalingFlag);
EXPORT_SYMBOL(KDrv_XC_GetDynamicScalingStatus);
EXPORT_SYMBOL(KDrv_XC_EnableIPMTuneAfterDS);
EXPORT_SYMBOL(KApi_XC_WriteSWDSCommand);
EXPORT_SYMBOL(KApi_XC_Add_NullCommand);
EXPORT_SYMBOL(KApi_DS_set_client);
EXPORT_SYMBOL(KApi_DS_get_support_index_num);
EXPORT_SYMBOL(KApi_XC_WriteSWDSCommandNonXC);
EXPORT_SYMBOL(KApi_XC_WriteSWDSCommand_Mask);
EXPORT_SYMBOL(KApi_XC_GetDynamicScalingIndex);
EXPORT_SYMBOL(KApi_XC_FireDynamicScalingIndex);
#undef _MDRV_SC_DYNAMICSCALING_C_
#endif //_MDRV_SC_DYNAMICSCALING_C_
