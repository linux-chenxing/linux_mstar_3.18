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
#define __DRV_SCL_MGWIN_IO_WRAPPER_C__


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------

#include "ms_platform.h"
#include "ms_types.h"
#include "cam_os_wrapper.h"
#include "drv_scl_mgwin_dbg.h"
#include "hal_scl_mgwin_util.h"

#include "drv_scl_mgwin_verchk.h"
#include "drv_scl_mgwin_io_st.h"
#include "drv_scl_mgwin_io_wrapper.h"
#include "drv_scl_mgwin.h"
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

#if 0

#define DRV_SCLMGWIN_IO_LOCK_MUTEX(x)    \
    SCL_ERR("+++ [MUTEX_LOCK][%s]_1_[%d] \n", __FUNCTION__, __LINE__); \
    CamOsMutexLock(x, SCLOS_WAIT_FOREVER); \
    SCL_ERR("+++ [MUTEX_LOCK][%s]_2_[%d] \n", __FUNCTION__, __LINE__);

#define DRV_SCLMGWIN_IO_UNLOCK_MUTEX(x)  \
    SCL_ERR("--- [MUTEX_LOCK][%s]   [%d] \n", __FUNCTION__, __LINE__); \
    CamOsMutexUnlock(x);



#else
#define DRV_SCLMGWIN_IO_LOCK_MUTEX(x)    CamOsMutexLock(x)
#define DRV_SCLMGWIN_IO_UNLOCK_MUTEX(x)  CamOsMutexUnlock(x)

#endif

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
DrvSclMgwinHandleConfig_t _gstSclMgwinHandler[DRV_SCLMGWIN_HANDLER_MAX];
DrvSclMgwinIoFunctionConfig_t _gstSclMgwinIoFunc;
CamOsMutex_t stSclMgwinIoHandlerMutex;
bool bDrvSclMgwinIoInit = FALSE;

u8  gMgwinbdbgmessage[EN_DBGMG_NUM_CONFIG];

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
bool _DrvSclMgwinGetCtx(s32 s32Handler, DrvSclMgwinIoCtxConfig_t *pCtxCfg)
{
    bool bRet = TRUE;
    s16  i;
    s16 s16Idx = -1;

    DRV_SCLMGWIN_IO_LOCK_MUTEX(&stSclMgwinIoHandlerMutex);

    for(i = 0; i < DRV_SCLMGWIN_HANDLER_MAX; i++)
    {
        if(_gstSclMgwinHandler[i].s32Handle == s32Handler)
        {
            s16Idx = i;
            break;
        }
    }

    if(s16Idx == -1)
    {
        pCtxCfg->pCtx = NULL;
        bRet = FALSE;
        SCL_ERR( "[SCLMGWIN]   %s %d  \n", __FUNCTION__, __LINE__);
    }
    else
    {
        pCtxCfg->pCtx = _gstSclMgwinHandler[s16Idx].stCtxCfg.pCtx;
        if(pCtxCfg->pCtx != NULL)
        {
            bRet = TRUE;
        }
        else
        {
            bRet = FALSE;
            SCL_ERR( "[SCLMGWIN]   %s %d  \n", __FUNCTION__, __LINE__);
        }
    }

    DRV_SCLMGWIN_IO_UNLOCK_MUTEX(&stSclMgwinIoHandlerMutex);

    return bRet;
}


DrvSclMgwinVersionChkConfig_t _DrvSclMgwinFillVersionChkStruct(u32 u32StructSize, u32 u32VersionSize, u32 *pVersion)
{
    DrvSclMgwinVersionChkConfig_t stVersion;
    stVersion.u32StructSize  = (u32)u32StructSize;
    stVersion.u32VersionSize = (u32)u32VersionSize;
    stVersion.pVersion      = (u32 *)pVersion;
    return stVersion;
}


s32 _DrvSclMgwinVersionCheck(DrvSclMgwinVersionChkConfig_t stVersion)
{
    if ( CHK_VERCHK_HEADER(stVersion.pVersion) )
    {
        if( CHK_VERCHK_MAJORVERSION_LESS( stVersion.pVersion, DRV_SCLMGWIN_VERSION) )
        {

            VERCHK_ERR("[SCLMGWIN] Version(%04x) < %04x!!! \n",
                       *(stVersion.pVersion) & VERCHK_VERSION_MASK,
                       DRV_SCLMGWIN_VERSION);

            return -1;
        }
        else
        {
            if( CHK_VERCHK_SIZE( &stVersion.u32VersionSize, stVersion.u32StructSize) == 0 )
            {
                VERCHK_ERR("[SCLMGWIN] Size(%04x) != %04x!!! \n",
                           stVersion.u32StructSize,
                           stVersion.u32VersionSize);

                return -1;
            }
            else
            {
                return 0;
            }
        }
    }
    else
    {
        VERCHK_ERR("[SCLMGWIN] No Header !!! \n");
        SCL_ERR( "[SCLMGWIN]   %s  \n", __FUNCTION__);
        return -1;
    }
}

DrvSclMgwinWinIdType_e _DrvSclMgwinIoTransWinId(DrvSclMgwinIoWinIdType_e enIoWinId)
{
    DrvSclMgwinWinIdType_e enWinId;

    enWinId = enIoWinId == E_DRV_SCLMGWIN_IO_WIN_ID_1 ? E_DRV_SCLMGWIN_WIN_ID_1 :
              enIoWinId == E_DRV_SCLMGWIN_IO_WIN_ID_2 ? E_DRV_SCLMGWIN_WIN_ID_2 :
                                                        E_DRV_SCLMGWIN_WIN_ID_NUM;
    return enWinId;
}

DrvSclMgwinSubWinIdType_e _DrvSclMgwinIoTransSubWinId(DrvSclMgwinIoSubWinIdType_e enIoSubWinId)
{
    DrvSclMgwinSubWinIdType_e enSubWinId;

    enSubWinId = enIoSubWinId == E_DRV_SCLMGWIN_IO_SUBWIN_ID_0  ? E_DRV_SCLMGWIN_SUBWIN_ID_0 :
                 enIoSubWinId == E_DRV_SCLMGWIN_IO_SUBWIN_ID_1  ? E_DRV_SCLMGWIN_SUBWIN_ID_1 :
                 enIoSubWinId == E_DRV_SCLMGWIN_IO_SUBWIN_ID_2  ? E_DRV_SCLMGWIN_SUBWIN_ID_2 :
                 enIoSubWinId == E_DRV_SCLMGWIN_IO_SUBWIN_ID_3  ? E_DRV_SCLMGWIN_SUBWIN_ID_3 :
                 enIoSubWinId == E_DRV_SCLMGWIN_IO_SUBWIN_ID_4  ? E_DRV_SCLMGWIN_SUBWIN_ID_4 :
                 enIoSubWinId == E_DRV_SCLMGWIN_IO_SUBWIN_ID_5  ? E_DRV_SCLMGWIN_SUBWIN_ID_5 :
                 enIoSubWinId == E_DRV_SCLMGWIN_IO_SUBWIN_ID_6  ? E_DRV_SCLMGWIN_SUBWIN_ID_6 :
                 enIoSubWinId == E_DRV_SCLMGWIN_IO_SUBWIN_ID_7  ? E_DRV_SCLMGWIN_SUBWIN_ID_7 :
                 enIoSubWinId == E_DRV_SCLMGWIN_IO_SUBWIN_ID_8  ? E_DRV_SCLMGWIN_SUBWIN_ID_8 :
                 enIoSubWinId == E_DRV_SCLMGWIN_IO_SUBWIN_ID_9  ? E_DRV_SCLMGWIN_SUBWIN_ID_9 :
                 enIoSubWinId == E_DRV_SCLMGWIN_IO_SUBWIN_ID_10 ? E_DRV_SCLMGWIN_SUBWIN_ID_10 :
                 enIoSubWinId == E_DRV_SCLMGWIN_IO_SUBWIN_ID_11 ? E_DRV_SCLMGWIN_SUBWIN_ID_11 :
                 enIoSubWinId == E_DRV_SCLMGWIN_IO_SUBWIN_ID_12 ? E_DRV_SCLMGWIN_SUBWIN_ID_12 :
                 enIoSubWinId == E_DRV_SCLMGWIN_IO_SUBWIN_ID_13 ? E_DRV_SCLMGWIN_SUBWIN_ID_13 :
                 enIoSubWinId == E_DRV_SCLMGWIN_IO_SUBWIN_ID_14 ? E_DRV_SCLMGWIN_SUBWIN_ID_14 :
                 enIoSubWinId == E_DRV_SCLMGWIN_IO_SUBWIN_ID_15 ? E_DRV_SCLMGWIN_SUBWIN_ID_15 :
                                                                  E_DRV_SCLMGWIN_SUBWIN_ID_NUM;

    return enSubWinId;
}

DrvSclMgwinMemFormat_e _DrvSclMgwinIoTransMemFormat(DrvSclMgwinIoMemFormat_e enIoMemFmt)
{
    DrvSclMgwinMemFormat_e  enMemFmt;

    enMemFmt =  enIoMemFmt == E_DRV_SCLMGWIN_IO_MEM_FMT_ARGB8888   ? E_DRV_SCLMGWIN_MEM_FMT_ARGB8888   :
                enIoMemFmt == E_DRV_SCLMGWIN_IO_MEM_FMT_ABGR8888   ? E_DRV_SCLMGWIN_MEM_FMT_ABGR8888   :
                enIoMemFmt == E_DRV_SCLMGWIN_IO_MEM_FMT_RGBA8888   ? E_DRV_SCLMGWIN_MEM_FMT_ABGR8888   :
                enIoMemFmt == E_DRV_SCLMGWIN_IO_MEM_FMT_BGRA8888   ? E_DRV_SCLMGWIN_MEM_FMT_BGRA8888   :
                enIoMemFmt == E_DRV_SCLMGWIN_IO_MEM_FMT_RGB565     ? E_DRV_SCLMGWIN_MEM_FMT_RGB565     :
                enIoMemFmt == E_DRV_SCLMGWIN_IO_MEM_FMT_YUV444_8B  ? E_DRV_SCLMGWIN_MEM_FMT_YUV444_8B  :
                enIoMemFmt == E_DRV_SCLMGWIN_IO_MEM_FMT_YUV444_10B ? E_DRV_SCLMGWIN_MEM_FMT_YUV444_10B :
                enIoMemFmt == E_DRV_SCLMGWIN_IO_MEM_FMT_YUV422_8B  ? E_DRV_SCLMGWIN_MEM_FMT_YUV422_8B  :
                enIoMemFmt == E_DRV_SCLMGWIN_IO_MEM_FMT_YUV422_8CE ? E_DRV_SCLMGWIN_MEM_FMT_YUV422_8CE :
                enIoMemFmt == E_DRV_SCLMGWIN_IO_MEM_FMT_YUV422_6CE ? E_DRV_SCLMGWIN_MEM_FMT_YUV422_6CE :
                enIoMemFmt == E_DRV_SCLMGWIN_IO_MEM_FMT_YUV420_8B  ? E_DRV_SCLMGWIN_MEM_FMT_YUV420_8B  :
                enIoMemFmt == E_DRV_SCLMGWIN_IO_MEM_FMT_YUV420_8CE ? E_DRV_SCLMGWIN_MEM_FMT_YUV420_8CE :
                enIoMemFmt == E_DRV_SCLMGWIN_IO_MEM_FMT_YUV420_6CE ? E_DRV_SCLMGWIN_MEM_FMT_YUV420_6CE :
                                                                     E_DRV_SCLMGWIN_MEM_FMT_NUM;
    return enMemFmt;
}

DrvSclMgwinFramePath_e _DrvSclMgwinIoTransFramepath(DrvSclMgwinIoFramePath_e enIoFramePath)
{
    DrvSclMgwinFramePath_e enPath;

    enPath = enIoFramePath == E_DRV_SCLMGWIN_IO_PATH_SC1 ? E_DRV_SCLMGWIN_PATH_SC1 :
             enIoFramePath == E_DRV_SCLMGWIN_IO_PATH_SC2 ? E_DRV_SCLMGWIN_PATH_SC2 :
                                                           E_DRV_SCLMGWIN_PATH_NUM;

    return enPath;
}
//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
bool _DrvSclMgwinIoInit(void)
{
    u8 i;

    if(bDrvSclMgwinIoInit == TRUE)
    {
        SCL_DBG(SCL_DBG_LV_IOCTL(), "[SCLMGWIN] %s %d, Already Init \n", __FUNCTION__, __LINE__ );
        return TRUE;
    }

    if( CamOsMutexInit(&stSclMgwinIoHandlerMutex) != CAM_OS_OK)
    {
        SCL_ERR("%s %d, Create Mutex Fail\n", __FUNCTION__, __LINE__);
        return FALSE;
    }


    DrvSclMgwinInit();

    for(i=0; i<DRV_SCLMGWIN_HANDLER_MAX; i++)
    {
        _gstSclMgwinHandler[i].stCtxCfg.pCtx = NULL;
        _gstSclMgwinHandler[i].s32Handle = -1;
    }
    memset(&_gstSclMgwinIoFunc, 0, sizeof(DrvSclMgwinIoFunctionConfig_t));

    _gstSclMgwinIoFunc.DrvSclMgwinIoSetFrameConfig          = _DrvSclMgwinIoSetFrameConfig;
    _gstSclMgwinIoFunc.DrvSclMgwinIoSetFrameOnOffConfig     = _DrvSclMgwinIoSetFrameOnOffConfig;
    _gstSclMgwinIoFunc.DrvSclMgwinIoSetSubWinConfig         = _DrvSclMgwinIoSetSubWinConfig;
    _gstSclMgwinIoFunc.DrvSclMgwinIoSetSubWinOnOffConfig    = _DrvSclMgwinIoSetSubWinOnOffConfig;
    _gstSclMgwinIoFunc.DrvSclMgwinIoSetSubWinFlipConfig     = _DrvSclMgwinIoSetSubWinFlipConfig;
    _gstSclMgwinIoFunc.DrvSclMgwinIoSetDbfConfig            = _DrvSclMgwinIoSetDbfConfig;
    _gstSclMgwinIoFunc.DrvSclMgwinIoSetFramePathConfig      = _DrvSclMgwinIoSetFramePathConfig;
    _gstSclMgwinIoFunc.DrvSclMgwinIoGetDbfConfig            = _DrvSclMgwinIoGetDbfConfig;
    _gstSclMgwinIoFunc.DrvSclMgwinIoSetRegisterConfig       = _DrvSclMgwinIoSetRegisterConfig;

    Set_DBGMG_FLAG(EN_DBGMG_PRIORITY_CONFIG, 1);
    Set_DBGMG_FLAG(EN_DBGMG_DRVSCLMGWIN_CONFIG, 0xFF);

    bDrvSclMgwinIoInit = TRUE;

    return TRUE;
}

s32 _DrvSclMgwinIoOpen(void)
{
    s32 s32Handle = -1;
    s16 s16Idx = -1;
    s16 i ;

    DRV_SCLMGWIN_IO_LOCK_MUTEX(&stSclMgwinIoHandlerMutex);

    for(i=0; i<DRV_SCLMGWIN_HANDLER_MAX; i++)
    {
        if(_gstSclMgwinHandler[i].s32Handle == -1)
        {
            s16Idx = i;
            break;
        }
    }

    if(s16Idx == -1)
    {
        s32Handle = -1;
        SCL_ERR("[SCLMGWIN]: Handler is not empyt\n");
    }
    else
    {
        void *pCtx = DrvSclMgwinCtxAllocate();
        if(pCtx)
        {
            s32Handle = s16Idx | DRV_SCLMGWIN_HANDLER_PRE_FIX;
            _gstSclMgwinHandler[s16Idx].s32Handle = s32Handle ;
            _gstSclMgwinHandler[s16Idx].stCtxCfg.pCtx = pCtx;
        }
        else
        {
            SCL_ERR("%s %d::Allocate Ctx Fail\n", __FUNCTION__, __LINE__);
        }
    }

    DRV_SCLMGWIN_IO_UNLOCK_MUTEX(&stSclMgwinIoHandlerMutex);

    return s32Handle;
}


DrvSclMgwinIoErrType_e _DrvSclMgwinIoRelease(s32 s32Handler)
{
    s16 s16Idx = -1;
    s16 i ;
    DrvSclMgwinIoErrType_e eRet = E_DRV_SCLMGWIN_IO_ERR_OK;

    DRV_SCLMGWIN_IO_LOCK_MUTEX(&stSclMgwinIoHandlerMutex);

    for(i=0; i<DRV_SCLMGWIN_HANDLER_MAX; i++)
    {
        if(_gstSclMgwinHandler[i].s32Handle == s32Handler)
        {
            s16Idx = i;
            break;
        }
    }

    if(s16Idx == -1)
    {
        SCL_ERR( "[SCLMGWIN]   %s %d  \n", __FUNCTION__, __LINE__);
        eRet = E_DRV_SCLMGWIN_IO_ERR_FAULT;
    }
    else
    {
        _gstSclMgwinHandler[s16Idx].s32Handle = -1;
        DrvSclMgwinCtxFree(_gstSclMgwinHandler[s16Idx].stCtxCfg.pCtx);
        _gstSclMgwinHandler[s16Idx].stCtxCfg.pCtx = NULL;
        eRet = E_DRV_SCLMGWIN_IO_ERR_OK;
    }

    DRV_SCLMGWIN_IO_UNLOCK_MUTEX(&stSclMgwinIoHandlerMutex);

    return eRet;
}


DrvSclMgwinIoErrType_e _DrvSclMgwinIoSetFrameConfig(s32 s32Handler, DrvSclMgwinIoFrameConfig_t *pstIoFrameCfg)
{
    DrvSclMgwinIoCtxConfig_t stCtxCfg;
    DrvSclMgwinVersionChkConfig_t stVersion;
    DrvSclMgwinFrameConfig_t stMgwinFrameCfg;
    stVersion =  _DrvSclMgwinFillVersionChkStruct(sizeof(DrvSclMgwinIoFrameConfig_t),
                                                 pstIoFrameCfg->VerChk_Size,
                                                 &pstIoFrameCfg->VerChk_Version);

    if(_DrvSclMgwinVersionCheck(stVersion))
    {
        SCL_ERR( "[SCLMGWIN]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLMGWIN_IO_ERR_INVAL;
    }

    if(_DrvSclMgwinGetCtx(s32Handler, &stCtxCfg) == FALSE)
    {
        SCL_ERR( "[SCLMGWIN]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLMGWIN_IO_ERR_FAULT;
    }

    stMgwinFrameCfg.enWinId = _DrvSclMgwinIoTransWinId(pstIoFrameCfg->enWinId);
    stMgwinFrameCfg.stWin.u16X = pstIoFrameCfg->stWin.u16X;
    stMgwinFrameCfg.stWin.u16Y = pstIoFrameCfg->stWin.u16Y;
    stMgwinFrameCfg.stWin.u16Width = pstIoFrameCfg->stWin.u16Width;
    stMgwinFrameCfg.stWin.u16Height = pstIoFrameCfg->stWin.u16Height;

    if(DrvSclMgwinSetFrameConfig(stCtxCfg.pCtx, &stMgwinFrameCfg))
    {
        return E_DRV_SCLMGWIN_IO_ERR_OK;
    }
    else
    {
        return E_DRV_SCLMGWIN_IO_ERR_FAULT;
    }
}

DrvSclMgwinIoErrType_e _DrvSclMgwinIoSetFrameOnOffConfig(s32 s32Handler, DrvSclMgwinIoFrameOnOffConfig_t *pstIoOnOffCfg)
{
    DrvSclMgwinIoCtxConfig_t stCtxCfg;
    DrvSclMgwinVersionChkConfig_t stVersion;
    DrvSclMgwinFrameOnOffConfig_t stFrameOnOffCfg;

    stVersion =  _DrvSclMgwinFillVersionChkStruct(sizeof(DrvSclMgwinIoFrameOnOffConfig_t),
                                                 pstIoOnOffCfg->VerChk_Size,
                                                 &pstIoOnOffCfg->VerChk_Version);

    if(_DrvSclMgwinVersionCheck(stVersion))
    {
        SCL_ERR( "[SCLMGWIN]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLMGWIN_IO_ERR_INVAL;
    }

    if(_DrvSclMgwinGetCtx(s32Handler, &stCtxCfg) == FALSE)
    {
        SCL_ERR( "[SCLMGWIN]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLMGWIN_IO_ERR_FAULT;
    }

    stFrameOnOffCfg.enWinId = _DrvSclMgwinIoTransWinId(pstIoOnOffCfg->enWinId);
    stFrameOnOffCfg.bEn = pstIoOnOffCfg->bEn;

    if(DrvSclMgwinSetFrameOnOffConfig(stCtxCfg.pCtx, &stFrameOnOffCfg))
    {
        return E_DRV_SCLMGWIN_IO_ERR_OK;
    }
    else
    {
        return E_DRV_SCLMGWIN_IO_ERR_FAULT;
    }
}

DrvSclMgwinIoErrType_e _DrvSclMgwinIoSetSubWinConfig(s32 s32Handler, DrvSclMgwinIoSubWinConfig_t *pstIoSubWinCfg)
{
    DrvSclMgwinIoCtxConfig_t stCtxCfg;
    DrvSclMgwinVersionChkConfig_t stVersion;
    DrvSclMgwinSubWinConfig_t stSubWinCfg;

    stVersion =  _DrvSclMgwinFillVersionChkStruct(sizeof(DrvSclMgwinIoSubWinConfig_t),
                                                 pstIoSubWinCfg->VerChk_Size,
                                                 &pstIoSubWinCfg->VerChk_Version);

    if(_DrvSclMgwinVersionCheck(stVersion))
    {
        SCL_ERR( "[SCLMGWIN]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLMGWIN_IO_ERR_INVAL;
    }

    if(_DrvSclMgwinGetCtx(s32Handler, &stCtxCfg) == FALSE)
    {
        SCL_ERR( "[SCLMGWIN]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLMGWIN_IO_ERR_FAULT;
    }
    stSubWinCfg.enWinId             = _DrvSclMgwinIoTransWinId(pstIoSubWinCfg->enWinId);
    stSubWinCfg.enSubWinId          = _DrvSclMgwinIoTransSubWinId(pstIoSubWinCfg->enSubWinId);
    stSubWinCfg.stWin.u16X          = pstIoSubWinCfg->stWin.u16X;
    stSubWinCfg.stWin.u16Y          = pstIoSubWinCfg->stWin.u16Y;
    stSubWinCfg.stWin.u16Width      = pstIoSubWinCfg->stWin.u16Width;
    stSubWinCfg.stWin.u16Height     = pstIoSubWinCfg->stWin.u16Height;

    if(DrvSclMgwinSetSubWinConfig(stCtxCfg.pCtx, &stSubWinCfg))
    {
        return E_DRV_SCLMGWIN_IO_ERR_OK;
    }
    else
    {
        return E_DRV_SCLMGWIN_IO_ERR_FAULT;
    }
}

DrvSclMgwinIoErrType_e _DrvSclMgwinIoSetSubWinOnOffConfig(s32 s32Handler, DrvSclMgwinIoSubWinOnOffConfig_t *pstIoSubWinOnOffCfg)
{
    DrvSclMgwinIoCtxConfig_t stCtxCfg;
    DrvSclMgwinVersionChkConfig_t stVersion;
    DrvSclMgwinSubWinOnOffConfig_t stSubWinOnOffCfg;

    stVersion =  _DrvSclMgwinFillVersionChkStruct(sizeof(DrvSclMgwinIoSubWinOnOffConfig_t),
                                                 pstIoSubWinOnOffCfg->VerChk_Size,
                                                 &pstIoSubWinOnOffCfg->VerChk_Version);

    if(_DrvSclMgwinVersionCheck(stVersion))
    {
        SCL_ERR( "[SCLMGWIN]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLMGWIN_IO_ERR_INVAL;
    }

    if(_DrvSclMgwinGetCtx(s32Handler, &stCtxCfg) == FALSE)
    {
        SCL_ERR( "[SCLMGWIN]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLMGWIN_IO_ERR_FAULT;
    }

    stSubWinOnOffCfg.enWinId    = _DrvSclMgwinIoTransWinId(pstIoSubWinOnOffCfg->enWinId);
    stSubWinOnOffCfg.enSubWinId = _DrvSclMgwinIoTransSubWinId(pstIoSubWinOnOffCfg->enSubWinId);
    stSubWinOnOffCfg.bEnable    = pstIoSubWinOnOffCfg->bEnable;

    if(DrvSclMgwinSetSubWinOnOffConfig(stCtxCfg.pCtx, &stSubWinOnOffCfg))
    {
        return E_DRV_SCLMGWIN_IO_ERR_OK;
    }
    else
    {
        return E_DRV_SCLMGWIN_IO_ERR_FAULT;
    }
}

DrvSclMgwinIoErrType_e _DrvSclMgwinIoSetSubWinFlipConfig(s32 s32Handler, DrvSclMgwinIoSubWinFlipConfig_t *pstIoSubWinFlipCfg)
{
    DrvSclMgwinIoCtxConfig_t stCtxCfg;
    DrvSclMgwinVersionChkConfig_t stVersion;
    DrvSclMgwinSubWinFlipConfig_t stSubWinFlipCfg;

    stVersion =  _DrvSclMgwinFillVersionChkStruct(sizeof(DrvSclMgwinIoSubWinFlipConfig_t),
                                                 pstIoSubWinFlipCfg->VerChk_Size,
                                                 &pstIoSubWinFlipCfg->VerChk_Version);

    if(_DrvSclMgwinVersionCheck(stVersion))
    {
        SCL_ERR( "[SCLMGWIN]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLMGWIN_IO_ERR_INVAL;
    }

    if(_DrvSclMgwinGetCtx(s32Handler, &stCtxCfg) == FALSE)
    {
        SCL_ERR( "[SCLMGWIN]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLMGWIN_IO_ERR_FAULT;
    }

    stSubWinFlipCfg.enWinId     = _DrvSclMgwinIoTransWinId(pstIoSubWinFlipCfg->enWinId);
    stSubWinFlipCfg.enSubWinId  = _DrvSclMgwinIoTransSubWinId(pstIoSubWinFlipCfg->enSubWinId);
    stSubWinFlipCfg.enMemFmt    = _DrvSclMgwinIoTransMemFormat(pstIoSubWinFlipCfg->enMemFmt);
    stSubWinFlipCfg.u32BaseAddr = pstIoSubWinFlipCfg->u32BaseAddr;
    stSubWinFlipCfg.u32Pitch    = pstIoSubWinFlipCfg->u32Pitch;

    if(DrvSclMgwinSetSubWinFlipConfig(stCtxCfg.pCtx, &stSubWinFlipCfg))
    {
        return E_DRV_SCLMGWIN_IO_ERR_OK;
    }
    else
    {
        return E_DRV_SCLMGWIN_IO_ERR_FAULT;
    }
}

DrvSclMgwinIoErrType_e _DrvSclMgwinIoSetDbfConfig(s32 s32Handler, DrvSclMgwinIoDbfConfig_t *pstIoDbfCfg)
{
    DrvSclMgwinIoCtxConfig_t stCtxCfg;
    DrvSclMgwinVersionChkConfig_t stVersion;
    DrvSclMgwinDbfConfig_t stDbfCfg;

    stVersion =  _DrvSclMgwinFillVersionChkStruct(sizeof(DrvSclMgwinIoDbfConfig_t),
                                                 pstIoDbfCfg->VerChk_Size,
                                                 &pstIoDbfCfg->VerChk_Version);

    if(_DrvSclMgwinVersionCheck(stVersion))
    {
        SCL_ERR( "[SCLMGWIN]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLMGWIN_IO_ERR_INVAL;
    }

    if(_DrvSclMgwinGetCtx(s32Handler, &stCtxCfg) == FALSE)
    {
        SCL_ERR( "[SCLMGWIN]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLMGWIN_IO_ERR_FAULT;
    }

    stDbfCfg.enWinId = _DrvSclMgwinIoTransWinId(pstIoDbfCfg->enWinId);

    if(DrvSclMgwinSetDbfConfig(stCtxCfg.pCtx, &stDbfCfg))
    {
        return E_DRV_SCLMGWIN_IO_ERR_OK;
    }
    else
    {
        return E_DRV_SCLMGWIN_IO_ERR_FAULT;
    }
}

DrvSclMgwinIoErrType_e _DrvSclMgwinIoGetDbfConfig(s32 s32Handler, DrvSclMgwinIoDbfConfig_t *pstIoDbfCfg)
{
    DrvSclMgwinIoCtxConfig_t stCtxCfg;
    DrvSclMgwinVersionChkConfig_t stVersion;
    DrvSclMgwinDbfConfig_t stDbfCfg;

    stVersion =  _DrvSclMgwinFillVersionChkStruct(sizeof(DrvSclMgwinIoDbfConfig_t),
                                                 pstIoDbfCfg->VerChk_Size,
                                                 &pstIoDbfCfg->VerChk_Version);

    if(_DrvSclMgwinVersionCheck(stVersion))
    {
        SCL_ERR( "[SCLMGWIN]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLMGWIN_IO_ERR_INVAL;
    }

    if(_DrvSclMgwinGetCtx(s32Handler, &stCtxCfg) == FALSE)
    {
        SCL_ERR( "[SCLMGWIN]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLMGWIN_IO_ERR_FAULT;
    }

    stDbfCfg.enWinId = _DrvSclMgwinIoTransWinId(pstIoDbfCfg->enWinId);
    stDbfCfg.bTrigger = 0;

    if(DrvSclMgwinGetDbfConfig(stCtxCfg.pCtx, &stDbfCfg))
    {
        pstIoDbfCfg->bTrigger = stDbfCfg.bTrigger;
        return E_DRV_SCLMGWIN_IO_ERR_OK;
    }
    else
    {
        return E_DRV_SCLMGWIN_IO_ERR_FAULT;
    }
}

DrvSclMgwinIoErrType_e _DrvSclMgwinIoSetFramePathConfig(s32 s32Handler, DrvSclMgwinIoFramePathConfig_t *pstIoFramePathCfg)
{
    DrvSclMgwinIoCtxConfig_t stCtxCfg;
    DrvSclMgwinVersionChkConfig_t stVersion;
    DrvSclMgwinFramePathConfig_t stFramePathCfg;

    stVersion =  _DrvSclMgwinFillVersionChkStruct(sizeof(DrvSclMgwinIoFramePathConfig_t),
                                                 pstIoFramePathCfg->VerChk_Size,
                                                 &pstIoFramePathCfg->VerChk_Version);

    if(_DrvSclMgwinVersionCheck(stVersion))
    {
        SCL_ERR( "[SCLMGWIN]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLMGWIN_IO_ERR_INVAL;
    }

    if(_DrvSclMgwinGetCtx(s32Handler, &stCtxCfg) == FALSE)
    {
        SCL_ERR( "[SCLMGWIN]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLMGWIN_IO_ERR_FAULT;
    }

    stFramePathCfg.enWinId = _DrvSclMgwinIoTransWinId(pstIoFramePathCfg->enWinId);
    stFramePathCfg.enPathType = _DrvSclMgwinIoTransFramepath(pstIoFramePathCfg->enPathType);

    if(DrvSclMgwinSetFramePathConfig(stCtxCfg.pCtx, &stFramePathCfg))
    {
        return E_DRV_SCLMGWIN_IO_ERR_OK;
    }
    else
    {
        return E_DRV_SCLMGWIN_IO_ERR_FAULT;
    }
}


DrvSclMgwinIoErrType_e _DrvSclMgwinIoSetRegisterConfig(s32 s32Handler, DrvSclMgwinIoRegisterConfig_t *pstIoRegCfg)
{
    W2BYTEMSK(pstIoRegCfg->u32Addr, pstIoRegCfg->u16Val, pstIoRegCfg->u16Msk);
    return E_DRV_SCLMGWIN_IO_ERR_OK;
}

EXPORT_SYMBOL(_DrvSclMgwinIoInit);
EXPORT_SYMBOL(_DrvSclMgwinIoOpen);
EXPORT_SYMBOL(_DrvSclMgwinIoRelease);
EXPORT_SYMBOL(_DrvSclMgwinIoSetFrameConfig);
EXPORT_SYMBOL(_DrvSclMgwinIoSetFrameOnOffConfig);
EXPORT_SYMBOL(_DrvSclMgwinIoSetSubWinConfig);
EXPORT_SYMBOL(_DrvSclMgwinIoSetSubWinOnOffConfig);
EXPORT_SYMBOL(_DrvSclMgwinIoSetSubWinFlipConfig);
EXPORT_SYMBOL(_DrvSclMgwinIoSetDbfConfig);
EXPORT_SYMBOL(_DrvSclMgwinIoSetFramePathConfig);
EXPORT_SYMBOL(_DrvSclMgwinIoGetDbfConfig);
EXPORT_SYMBOL(_DrvSclMgwinIoSetRegisterConfig);
