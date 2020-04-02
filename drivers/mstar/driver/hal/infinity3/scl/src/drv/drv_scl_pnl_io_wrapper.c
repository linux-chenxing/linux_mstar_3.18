//<MStar Software>
//******************************************************************************
// MStar Software
// Copyright (c) 2010 - 2012 MStar Semiconductor, Inc. All rights reserved.
// All software, firmware and related documentation herein ("MStar Software") are
// intellectual property of MStar Semiconductor, Inc. ("MStar") and protected by
// law, including, but not limited to, copyright law and international treaties.
// Any use, modification, reproduction, retransmission, or republication of all
// or part of MStar Software is expressly prohibited, unless prior written
// permission has been granted by MStar.
//
// By accessing, browsing and/or using MStar Software, you acknowledge that you
// have read, understood, and agree, to be bound by below terms ("Terms") and to
// comply with all applicable laws and regulations:
//
// 1. MStar shall retain any and all right, ownership and interest to MStar
//    Software and any modification/derivatives thereof.
//    No right, ownership, or interest to MStar Software and any
//    modification/derivatives thereof is transferred to you under Terms.
//
// 2. You understand that MStar Software might include, incorporate or be
//    supplied together with third party`s software and the use of MStar
//    Software may require additional licenses from third parties.
//    Therefore, you hereby agree it is your sole responsibility to separately
//    obtain any and all third party right and license necessary for your use of
//    such third party`s software.
//
// 3. MStar Software and any modification/derivatives thereof shall be deemed as
//    MStar`s confidential information and you agree to keep MStar`s
//    confidential information in strictest confidence and not disclose to any
//    third party.
//
// 4. MStar Software is provided on an "AS IS" basis without warranties of any
//    kind. Any warranties are hereby expressly disclaimed by MStar, including
//    without limitation, any warranties of merchantability, non-infringement of
//    intellectual property rights, fitness for a particular purpose, error free
//    and in conformity with any international standard.  You agree to waive any
//    claim against MStar for any loss, damage, cost or expense that you may
//    incur related to your use of MStar Software.
//    In no event shall MStar be liable for any direct, indirect, incidental or
//    consequential damages, including without limitation, lost of profit or
//    revenues, lost or damage of data, and unauthorized system use.
//    You agree that this Section 4 shall still apply without being affected
//    even if MStar Software has been modified by MStar in accordance with your
//    request or instruction for your use, except otherwise agreed by both
//    parties in writing.
//
// 5. If requested, MStar may from time to time provide technical supports or
//    services in relation with MStar Software to you for your use of
//    MStar Software in conjunction with your or your customer`s product
//    ("Services").
//    You understand and agree that, except otherwise agreed by both parties in
//    writing, Services are provided on an "AS IS" basis and the warranty
//    disclaimer set forth in Section 4 above shall apply.
//
// 6. Nothing contained herein shall be construed as by implication, estoppels
//    or otherwise:
//    (a) conferring any license or right to use MStar name, trademark, service
//        mark, symbol or any other identification;
//    (b) obligating MStar or any of its affiliates to furnish any person,
//        including without limitation, you and your customers, any assistance
//        of any kind whatsoever, or any information; or
//    (c) conferring any license or right under any intellectual property right.
//
// 7. These terms shall be governed by and construed in accordance with the laws
//    of Taiwan, R.O.C., excluding its conflict of law rules.
//    Any and all dispute arising out hereof or related hereto shall be finally
//    settled by arbitration referred to the Chinese Arbitration Association,
//    Taipei in accordance with the ROC Arbitration Law and the Arbitration
//    Rules of the Association by three (3) arbitrators appointed in accordance
//    with the said Rules.
//    The place of arbitration shall be in Taipei, Taiwan and the language shall
//    be English.
//    The arbitration award shall be final and binding to both parties.
//
//******************************************************************************
//<MStar Software>
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
#define __DRV_SCL_PNL_IO_WRAPPER_C__


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------

#include "drv_scl_os.h"
#include "drv_scl_dbg.h"
#include "hal_scl_util.h"

#include "drv_scl_verchk.h"
#include "drv_scl_irq_st.h"
#include "drv_scl_pnl_m.h"
#include "drv_scl_pnl_io_st.h"
#include "drv_scl_pnl_io_wrapper.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
DrvSclPnlHandleConfig_t _gstSclPnlHandler[DRV_SCLPNL_HANDLER_MAX];

DrvSclPnlIoFunctionConfig_t _gstSclPnlIoFunc;

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
bool _DrvSclPnlCheckHandler(s32 s32Handler, DrvSclPnlIoIdType_e *penIdType)
{
    bool bRet = TRUE;
    s16  i;
    s16 s16Idx = -1;

    for(i = 0; i < DRV_SCLPNL_HANDLER_MAX; i++)
    {
        if(_gstSclPnlHandler[i].s32Handle == s32Handler)
        {
            s16Idx = i;
            break;
        }
    }

    if(s16Idx == -1)
    {
        *penIdType = E_DRV_SCLPNL_IO_ID_NUM;
        bRet = FALSE;
    }
    else
    {
        *penIdType = _gstSclPnlHandler[s16Idx].enSclPnlId;
        if(IsSclPnlIdValid(*penIdType ))
        {
            bRet = TRUE;
        }
        else
        {
            bRet = FALSE;
        }
    }

    return bRet;
}


DrvSclPnlVersionChkConfig_t _DrvSclPnlFillVersionChkStruct(u32 u32StructSize, u32 u32VersionSize, u32 *pVersion)
{
    DrvSclPnlVersionChkConfig_t stVersion;
    stVersion.u32StructSize  = (u32)u32StructSize;
    stVersion.u32VersionSize = (u32)u32VersionSize;
    stVersion.pVersion      = (u32 *)pVersion;
    return stVersion;
}


s32 _DrvSclPnlVersionCheck(DrvSclPnlVersionChkConfig_t stVersion)
{
    if ( CHK_VERCHK_HEADER(stVersion.pVersion) )
    {
        if( CHK_VERCHK_MAJORVERSION_LESS( stVersion.pVersion, DRV_SCLPNL_VERSION) )
        {

            VERCHK_ERR("[PNL] Version(%04lx) < %04x!!! \n",
                       *(stVersion.pVersion) & VERCHK_VERSION_MASK,
                       DRV_SCLPNL_VERSION);

            return -1;
        }
        else
        {
            if( CHK_VERCHK_SIZE( &stVersion.u32VersionSize, stVersion.u32StructSize) == 0 )
            {
                VERCHK_ERR("[PNL] Size(%04lx) != %04lx!!! \n",
                           stVersion.u32StructSize,
                           stVersion.u32VersionSize);

                return -1;
            }
            else
            {
                return VersionCheckSuccess;
            }
        }
    }
    else
    {
        VERCHK_ERR("[PNL] No Header !!! \n");
        SCL_ERR( "[PNL]   %s  \n", __FUNCTION__);
        return -1;
    }
}

u8 _DrvSclPnlIoGetIdOpenTime(DrvSclPnlIoIdType_e enSclPnlId)
{
    s16 i = 0;
    u8 u8Cnt = 0;
    for(i = 0; i < DRV_SCLPNL_HANDLER_MAX; i++)
    {
        if(_gstSclPnlHandler[i].enSclPnlId == enSclPnlId && _gstSclPnlHandler[i].s32Handle != -1)
        {
            u8Cnt ++;
        }
    }
    return u8Cnt;
}


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
void _DrvSclPnlIoInitHandler(void)
{
    u8 i;
    for(i = 0; i < DRV_SCLPNL_HANDLER_MAX; i++)
    {
        _gstSclPnlHandler[i].s32Handle = -1;
        _gstSclPnlHandler[i].pPrivate_Data = NULL;
        _gstSclPnlHandler[i].enSclPnlId = E_DRV_SCLPNL_IO_ID_NUM;
    }

    DrvSclOsMemset(&_gstSclPnlIoFunc, 0, sizeof(DrvSclPnlIoFunctionConfig_t));
    _gstSclPnlIoFunc.DrvSclPnlIoSetTimingConfig     = _DrvSclPnlIoSetTimingConfig;
    _gstSclPnlIoFunc.DrvSclPnlIoSetLpllConfig       = _DrvSclPnlIoSetLpllConfig;
    _gstSclPnlIoFunc.DrvSclPnlIoGetVersionConfig    = _DrvSclPnlIoGetVersionConfig;
}

s32 _DrvSclPnlIoOpen(DrvSclPnlIoIdType_e enSclPnlId)
{
    s32 s32Handle = -1;
    s16 i;
    s16 s16Idx = -1;

    for(i = 0; i < DRV_SCLPNL_HANDLER_MAX; i++)
    {
        if(_gstSclPnlHandler[i].s32Handle == -1)
        {
            s16Idx = i;
            break;
        }
    }

    if(s16Idx == -1)
    {
        s32Handle = -1;

    }
    else
    {
        if(enSclPnlId == E_DRV_SCLPNL_IO_ID_1)
        {
            if(s16Idx >= DRV_SCLPNL_HANDLER_MAX)
            {
                SCL_ERR( "[PNL]   %s  %d\n", __FUNCTION__, __LINE__);
                s32Handle = -1;
            }
            else
            {
                s32Handle = s16Idx | DRV_SCLPNL_HANDLER_PRE_FIX;
                _gstSclPnlHandler[s16Idx].s32Handle = s32Handle ;
                _gstSclPnlHandler[s16Idx].pPrivate_Data = NULL;
                _gstSclPnlHandler[s16Idx].enSclPnlId = enSclPnlId;

            }
        }
    }
    return s32Handle;
}

DrvSclPnlIoErrType_e _DrvSclPnlIoRelease(s32 s32Handler)
{
    DrvSclPnlIoErrType_e eRet = E_DRV_SCLPNL_IO_ERR_OK;
    s16 i;
    s16 s16Idx = -1;

    for(i = 0; i < DRV_SCLPNL_HANDLER_MAX; i++)
    {
        if(_gstSclPnlHandler[i].s32Handle == s32Handler)
        {
            s16Idx = i;
            break;
        }
    }

    if(s16Idx == -1)
    {
        eRet = E_DRV_SCLPNL_IO_ERR_FAULT;
    }
    else
    {
        if( _DrvSclPnlIoGetIdOpenTime(_gstSclPnlHandler[s16Idx].enSclPnlId) == 1)
        {
            MDrvSclPnlRelease();
        }

        _gstSclPnlHandler[s16Idx].s32Handle = -1;
        _gstSclPnlHandler[s16Idx].pPrivate_Data = NULL;
        _gstSclPnlHandler[s16Idx].enSclPnlId = E_DRV_SCLPNL_IO_ID_NUM;
        eRet = E_DRV_SCLPNL_IO_ERR_OK;
    }

    return eRet;
}


DrvSclPnlIoErrType_e _DrvSclPnlIoSetTimingConfig(s32 s32Handler, DrvSclPnlIoTimingConfig_t *pstIOPnlTimingCfg)
{
    DrvSclPnlVersionChkConfig_t stVersion;
    MDrvSclPnlTimingConfig_t stPnlTimingCfg;
    DrvSclPnlIoIdType_e enSclPnlIdType;

    stVersion =  _DrvSclPnlFillVersionChkStruct(sizeof(DrvSclPnlIoTimingConfig_t),
                 pstIOPnlTimingCfg->VerChk_Size,
                 &pstIOPnlTimingCfg->VerChk_Version);
    if(_DrvSclPnlVersionCheck(stVersion))
    {
        SCL_ERR( "[PNL]   %s  \n", __FUNCTION__);
        return E_DRV_SCLPNL_IO_ERR_INVAL;
    }

    if(_DrvSclPnlCheckHandler(s32Handler, &enSclPnlIdType) == FALSE)
    {
        return E_DRV_SCLPNL_IO_ERR_INVAL;
    }

    stPnlTimingCfg.u16Vsync_St  = pstIOPnlTimingCfg->u16Vsync_St;
    stPnlTimingCfg.u16Vsync_End = pstIOPnlTimingCfg->u16Vsync_End;
    stPnlTimingCfg.u16Vde_St    = pstIOPnlTimingCfg->u16Vde_St;
    stPnlTimingCfg.u16Vde_End   = pstIOPnlTimingCfg->u16Vde_End;
    stPnlTimingCfg.u16Vfde_St   = pstIOPnlTimingCfg->u16Vfde_St;
    stPnlTimingCfg.u16Vfde_End  = pstIOPnlTimingCfg->u16Vfde_End;
    stPnlTimingCfg.u16Vtt       = pstIOPnlTimingCfg->u16Vtt;
    stPnlTimingCfg.u16Hsync_St  = pstIOPnlTimingCfg->u16Hsync_St;
    stPnlTimingCfg.u16Hsync_End = pstIOPnlTimingCfg->u16Hsync_End;
    stPnlTimingCfg.u16Hde_St    = pstIOPnlTimingCfg->u16Hde_St;
    stPnlTimingCfg.u16Hde_End   = pstIOPnlTimingCfg->u16Hde_End;
    stPnlTimingCfg.u16Hfde_St   = pstIOPnlTimingCfg->u16Hfde_St;
    stPnlTimingCfg.u16Hfde_End  = pstIOPnlTimingCfg->u16Hfde_End;
    stPnlTimingCfg.u16Htt       = pstIOPnlTimingCfg->u16Htt;
    stPnlTimingCfg.u16VFreqx10  = pstIOPnlTimingCfg->u16VFreqx10;


    if(!MDrvSclPnlSetTimingConfig(&stPnlTimingCfg))
    {
        SCL_ERR( "[PNL]   %s, MDrvSclPnlSetTimingConfig Fail  \n", __FUNCTION__);
        return E_DRV_SCLPNL_IO_ERR_FAULT;
    }
    else
    {
        return E_DRV_SCLPNL_IO_ERR_OK;
    }
}


DrvSclPnlIoErrType_e _DrvSclPnlIoSetLpllConfig(s32 s32Handler, DrvSclPnlIoTimingConfig_t *pstIOPnlTimingCfg)
{
    DrvSclPnlVersionChkConfig_t stVersion;
    MDrvSclPnlTimingConfig_t stPnlTimingCfg;
    DrvSclPnlIoIdType_e enSclPnlIdType;

    stVersion =  _DrvSclPnlFillVersionChkStruct(sizeof(DrvSclPnlIoTimingConfig_t),
                 pstIOPnlTimingCfg->VerChk_Size,
                 &pstIOPnlTimingCfg->VerChk_Version);
    if(_DrvSclPnlVersionCheck(stVersion))
    {
        SCL_ERR( "[PNL]   %s,%d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLPNL_IO_ERR_INVAL;
    }

    if(_DrvSclPnlCheckHandler(s32Handler, &enSclPnlIdType) == FALSE)
    {
        return E_DRV_SCLPNL_IO_ERR_INVAL;
    }

    SCL_DBG(SCL_DBG_LV_IOCTL()&EN_DBGMG_IOCTLEVEL_ELSE, "[PNL] Size(%04ld) \n", (pstIOPnlTimingCfg->VerChk_Size) );

    stPnlTimingCfg.u16Vsync_St  = pstIOPnlTimingCfg->u16Vsync_St;
    stPnlTimingCfg.u16Vsync_End = pstIOPnlTimingCfg->u16Vsync_End;
    stPnlTimingCfg.u16Vde_St    = pstIOPnlTimingCfg->u16Vde_St;
    stPnlTimingCfg.u16Vde_End   = pstIOPnlTimingCfg->u16Vde_End;
    stPnlTimingCfg.u16Vfde_St   = pstIOPnlTimingCfg->u16Vfde_St;
    stPnlTimingCfg.u16Vfde_End  = pstIOPnlTimingCfg->u16Vfde_End;
    stPnlTimingCfg.u16Vtt       = pstIOPnlTimingCfg->u16Vtt;
    stPnlTimingCfg.u16Hsync_St  = pstIOPnlTimingCfg->u16Hsync_St;
    stPnlTimingCfg.u16Hsync_End = pstIOPnlTimingCfg->u16Hsync_End;
    stPnlTimingCfg.u16Hde_St    = pstIOPnlTimingCfg->u16Hde_St;
    stPnlTimingCfg.u16Hde_End   = pstIOPnlTimingCfg->u16Hde_End;
    stPnlTimingCfg.u16Hfde_St   = pstIOPnlTimingCfg->u16Hfde_St;
    stPnlTimingCfg.u16Hfde_End  = pstIOPnlTimingCfg->u16Hfde_End;
    stPnlTimingCfg.u16Htt       = pstIOPnlTimingCfg->u16Htt;
    stPnlTimingCfg.u16VFreqx10  = pstIOPnlTimingCfg->u16VFreqx10;

    if(!MDrvSclPnlSetLPLLConfig(&stPnlTimingCfg))
    {
        return E_DRV_SCLPNL_IO_ERR_FAULT;
    }
    else
    {
        return E_DRV_SCLPNL_IO_ERR_OK;
    }
}


DrvSclPnlIoErrType_e _DrvSclPnlIoGetVersionConfig(s32 s32Handler, DrvSclPnlIoVersionConfig_t *psIOVersionCfg)
{
    DrvSclPnlIoErrType_e eRet = E_DRV_SCLPNL_IO_ERR_OK;
    if (CHK_VERCHK_HEADER( &(psIOVersionCfg->VerChk_Version)) )
    {
        if( CHK_VERCHK_VERSION_LESS( &(psIOVersionCfg->VerChk_Version), DRV_SCLPNL_VERSION) )
        {

            VERCHK_ERR("[PNL] Version(%04lx) < %04x!!! \n",
                       psIOVersionCfg->VerChk_Version & VERCHK_VERSION_MASK,
                       DRV_SCLPNL_VERSION);

            eRet = E_DRV_SCLPNL_IO_ERR_FAULT;
        }
        else
        {
            if( CHK_VERCHK_SIZE( &(psIOVersionCfg->VerChk_Size), sizeof(DrvSclPnlIoVersionConfig_t)) == 0 )
            {
                VERCHK_ERR("[PNL] Size(%04x) != %04lx!!! \n",
                           sizeof(DrvSclPnlIoVersionConfig_t),
                           (psIOVersionCfg->VerChk_Size));

                eRet = E_DRV_SCLPNL_IO_ERR_FAULT;
            }
            else
            {
                DrvSclPnlIoVersionConfig_t stCfg;

                stCfg = FILL_VERCHK_TYPE(stCfg, stCfg.VerChk_Version, stCfg.VerChk_Size, DRV_SCLPNL_VERSION);
                stCfg.u32Version = DRV_SCLPNL_VERSION;
                DrvSclOsMemcpy(psIOVersionCfg, &stCfg, sizeof(DrvSclPnlIoVersionConfig_t));
                eRet = E_DRV_SCLPNL_IO_ERR_OK;
            }
        }
    }
    else
    {
        VERCHK_ERR("[PNL] No Header !!! \n");
        SCL_ERR( "[PNL]%s  \n", __FUNCTION__);
        eRet = E_DRV_SCLPNL_IO_ERR_FAULT;
    }

    return eRet;
}

