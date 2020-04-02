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
#define __DRV_SCL_VIP_IO_WRAPPER_C__


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "drv_scl_os.h"
#include "drv_scl_dbg.h"
#include "hal_scl_util.h"

#include "drv_scl_verchk.h"
#include "drv_scl_vip_m_st.h"
#include "drv_scl_vip_m.h"
#include "drv_scl_vip_io_st.h"
#include "drv_scl_vip_io_wrapper.h"
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

#define AIP_PARSING(x)  ( x == E_MDRV_SCLVIP_AIP_YEE                     ?    "E_MDRV_SCLVIP_AIP_YEE"           : \
                          x == E_MDRV_SCLVIP_AIP_YEE_AC_LUT              ?    "E_MDRV_SCLVIP_AIP_YEE_AC_LUT"    : \
                          x == E_MDRV_SCLVIP_AIP_WDR_GLOB                ?    "E_MDRV_SCLVIP_AIP_WDR_GLOB"      : \
                          x == E_MDRV_SCLVIP_AIP_WDR_LOC                 ?    "E_MDRV_SCLVIP_AIP_WDR_LOC"       : \
                          x == E_MDRV_SCLVIP_AIP_MXNR                    ?    "E_MDRV_SCLVIP_AIP_MXNR"          : \
                          x == E_MDRV_SCLVIP_AIP_UVADJ                   ?    "E_MDRV_SCLVIP_AIP_UVADJ"         : \
                          x == E_MDRV_SCLVIP_AIP_XNR                     ?    "E_MDRV_SCLVIP_AIP_XNR"           : \
                          x == E_MDRV_SCLVIP_AIP_YCUVM                   ?    "E_MDRV_SCLVIP_AIP_YCUVM"         : \
                          x == E_MDRV_SCLVIP_AIP_COLORTRAN               ?    "E_MDRV_SCLVIP_AIP_COLORTRAN"     : \
                          x == E_MDRV_SCLVIP_AIP_GAMMA                   ?    "E_MDRV_SCLVIP_AIP_GAMMA"         : \
                          x == E_MDRV_SCLVIP_AIP_422TO444                ?    "E_MDRV_SCLVIP_AIP_422TO444"      : \
                          x == E_MDRV_SCLVIP_AIP_YUVTORGB                ?    "E_MDRV_SCLVIP_AIP_YUVTORGB"      : \
                          x == E_MDRV_SCLVIP_AIP_GM10TO12                ?    "E_MDRV_SCLVIP_AIP_GM10TO12"      :\
                          x == E_MDRV_SCLVIP_AIP_CCM                     ?    "E_MDRV_SCLVIP_AIP_CCM"           : \
                          x == E_MDRV_SCLVIP_AIP_HSV                     ?    "E_MDRV_SCLVIP_AIP_HSV"           : \
                          x == E_MDRV_SCLVIP_AIP_GM12TO10                ?    "E_MDRV_SCLVIP_AIP_GM12TO10"      : \
                          x == E_MDRV_SCLVIP_AIP_RGBTOYUV                ?    "E_MDRV_SCLVIP_AIP_RGBTOYUV"      : \
                          x == E_MDRV_SCLVIP_AIP_444TO422                ?    "E_MDRV_SCLVIP_AIP_444TO422"      : \
                                                                            "UNKNOWN")

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
DrvSclVipHandleConfig_t _gstSclVipHandler[DRV_SCLVIP_HANDLER_MAX];


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
void _DrvSclVipIoInitHandler(void)
{
    u16 i;

    for(i = 0; i < DRV_SCLVIP_HANDLER_MAX; i++)
    {
        _gstSclVipHandler[i].s32Handle = -1;
        _gstSclVipHandler[i].pPrivate_Data = NULL;
        _gstSclVipHandler[i].enSclVipId = E_DRV_SCLVIP_IO_ID_NUM;
    }
}


void _DrvSclVipIoLogConfigStruct(MDrvSclVipSetPqConfig_t stSetPQCfg)
{
    u8 u8offset;
    u8 *pu8value = NULL;
    for(u8offset = 0;u8offset < stSetPQCfg.u32StructSize;u8offset++)
    {
        pu8value = stSetPQCfg.pPointToCfg + u8offset*1;
        SCL_DBG(SCL_DBG_LV_VIP_LOG(), "[VIP] offset%hhd:%hhx \n",u8offset,*pu8value);

    }
}

void _DrvSclVipIoLogConfigByIP(DrvSclVipIoConfigType_e enVIPtype,u8 *pPointToCfg)
{
    MDrvSclVipSetPqConfig_t stSetPQCfg;
    stSetPQCfg = MDrvSclVipFillBasicStructSetPqCfg(enVIPtype,pPointToCfg);
    _DrvSclVipIoLogConfigStruct(stSetPQCfg);
}

MDrvSclVipVersionChkConfig_t _DrvSclVipIoFillVersionChkStruct
(u32 u32StructSize, u32 u32VersionSize, u32 *pVersion)
{
    MDrvSclVipVersionChkConfig_t stVersion;
    stVersion.u32StructSize  = (u32)u32StructSize;
    stVersion.u32VersionSize = (u32)u32VersionSize;
    stVersion.pVersion      = (u32 *)pVersion;
    return stVersion;
}

s32 _DrvSclVipIoVersionCheck(MDrvSclVipVersionChkConfig_t stVersion)
{
    if ( CHK_VERCHK_HEADER(stVersion.pVersion) )
    {
        if( CHK_VERCHK_MAJORVERSION_LESS( stVersion.pVersion, DRV_SCLVIP_VERSION) )
        {

            VERCHK_ERR("[VIP] Version(%04lx) < %04x!!! \n",
                *(stVersion.pVersion) & VERCHK_VERSION_MASK,
                DRV_SCLVIP_VERSION);

            return -1;
        }
        else
        {
            if( CHK_VERCHK_SIZE( &stVersion.u32VersionSize, stVersion.u32StructSize) == 0 )
            {
                VERCHK_ERR("[VIP] Size(%04lx) != %04lx!!! \n",
                    stVersion.u32StructSize,
                    stVersion.u32VersionSize);

                return -1;
            }
            else
            {
                SCL_DBG(SCL_DBG_LV_IOCTL()&EN_DBGMG_IOCTLEVEL_ELSE, "[VIP] Size(%ld) \n",stVersion.u32StructSize );
                return VersionCheckSuccess;
            }
        }
    }
    else
    {
        VERCHK_ERR("[VIP] No Header !!! \n");
        SCL_ERR( "[VIP]   %s  %d\n", __FUNCTION__, __LINE__);
        return -1;
    }
}


u8 _DrvSclVipIoGetIdOpenTime(DrvSclVipIoIdType_e enVipId)
{
    s16 i = 0;
    u8 u8Cnt = 0;
    for(i = 0; i < DRV_SCLVIP_HANDLER_MAX; i++)
    {
        if(_gstSclVipHandler[i].enSclVipId == enVipId && _gstSclVipHandler[i].s32Handle != -1)
        {
            u8Cnt ++;
        }
    }
    return u8Cnt;
}


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
s32 _DrvSclVipIoOpen(DrvSclVipIoIdType_e enSclVipId)
{
    MDrvSclVipInitConfig_t stVipInitCfg;

    s32 s32Handle = -1;
    s16 s16Idx = -1;
    s16 i ;

    if(_DrvSclVipIoGetIdOpenTime(enSclVipId) == 0)
    {
        stVipInitCfg.u32RiuBase = 0x1F000000; //ToDo
        if(MDrvSclVipSysInit(&stVipInitCfg) == 0)
        {
            return -1;
        }

    }

    for(i = 0; i < DRV_SCLVIP_HANDLER_MAX; i++)
    {
        if(_gstSclVipHandler[i].s32Handle == -1)
        {
            s16Idx = i;
            break;
        }
    }

    if(s16Idx == -1)
    {
        s32Handle = -1;
        SCL_ERR( "[VIP]   %s  %d\n", __FUNCTION__, __LINE__);
    }
    else
    {

        s32Handle = s16Idx | DRV_SCLVIP_HANDLER_PRE_FIX;
        _gstSclVipHandler[s16Idx].s32Handle = s32Handle ;
        _gstSclVipHandler[s16Idx].enSclVipId = enSclVipId;
    }

    return s32Handle;
}



DrvSclVipIoErrType_e _DrvSclVipIoRelease(s32 s32Handler)
{
    s16 s16Idx = -1;
    s16 i ;
    DrvSclVipIoErrType_e eRet = TRUE;

    for(i = 0; i < DRV_SCLVIP_HANDLER_MAX; i++)
    {
        if(_gstSclVipHandler[i].s32Handle == s32Handler)
        {
            s16Idx = i;
            break;
        }
    }

    if(s16Idx == -1)
    {
        SCL_ERR( "[VIP]   %s  %d\n", __FUNCTION__, __LINE__);
        eRet = E_DRV_SCLVIP_IO_ERR_INVAL;
    }
    else
    {
        if(_DrvSclVipIoGetIdOpenTime(_gstSclVipHandler[s16Idx].enSclVipId) == 1)
        {
            MDrvSclVipRelease();
        }
        _gstSclVipHandler[s16Idx].s32Handle = -1;
        _gstSclVipHandler[s16Idx].pPrivate_Data = NULL;
        _gstSclVipHandler[s16Idx].enSclVipId = E_DRV_SCLVIP_IO_ID_NUM;
        eRet = E_DRV_SCLVIP_IO_ERR_OK;
    }

    return eRet;
}

DrvSclVipIoErrType_e _DrvSclVipIoSetPeakingConfig(s32 s32Handler, DrvSclVipIoPeakingConfig_t *pstCfg)
{
    MDrvSclVipVersionChkConfig_t stVersion;

    stVersion = _DrvSclVipIoFillVersionChkStruct(sizeof(DrvSclVipIoPeakingConfig_t),
                                              (pstCfg->VerChk_Size),
                                              &(pstCfg->VerChk_Version));

    if(_DrvSclVipIoVersionCheck(stVersion))
    {
        SCL_ERR( "[VIP]   %s  %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    _DrvSclVipIoLogConfigByIP(E_DRV_SCLVIP_IO_PEAKING_CONFIG, (u8 *)pstCfg);

    if(!MDrvSclVipSetPeakingConfig((void *)pstCfg))
    {
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    return E_DRV_SCLVIP_IO_ERR_OK;
}

DrvSclVipIoErrType_e _DrvSclVipIoSetDlcHistogramConfig(s32 s32Handler, DrvSclVipIoDlcHistogramConfig_t *pstCfg)
{
    MDrvSclVipVersionChkConfig_t stVersion;

    stVersion = _DrvSclVipIoFillVersionChkStruct(sizeof(DrvSclVipIoDlcHistogramConfig_t),
                                              (pstCfg->VerChk_Size),
                                              &(pstCfg->VerChk_Version));

    if(_DrvSclVipIoVersionCheck(stVersion))
    {
        SCL_ERR( "[VIP]   %s  %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    _DrvSclVipIoLogConfigByIP(E_DRV_SCLVIP_IO_DLC_HISTOGRAM_CONFIG, (u8 *)pstCfg);
    if(!MDrvSclVipSetHistogramConfig((void *)pstCfg))
    {
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    return E_DRV_SCLVIP_IO_ERR_OK;
}

DrvSclVipIoErrType_e _DrvSclVipIoGetDlcHistogramConfig(s32 s32Handler, DrvSclVipIoDlcHistogramReport_t *pstCfg)
{

    if(!MDrvSclVipGetDlcHistogramReport((void *)pstCfg))
    {
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    return E_DRV_SCLVIP_IO_ERR_OK;
}


DrvSclVipIoErrType_e _DrvSclVipIoSetDlcConfig(s32 s32Handler, DrvSclVipIoDlcConfig_t *pstCfg)
{
    MDrvSclVipVersionChkConfig_t stVersion;

    stVersion = _DrvSclVipIoFillVersionChkStruct(sizeof(DrvSclVipIoDlcConfig_t),
                                              (pstCfg->VerChk_Size),
                                              &(pstCfg->VerChk_Version));
    if(_DrvSclVipIoVersionCheck(stVersion))
    {
        SCL_ERR( "[VIP]   %s  %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    _DrvSclVipIoLogConfigByIP(E_DRV_SCLVIP_IO_DLC_CONFIG,(u8 *)pstCfg);
    if(!MDrvSclVipSetDlcConfig((void *)pstCfg))
    {
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    return E_DRV_SCLVIP_IO_ERR_OK;
}

DrvSclVipIoErrType_e  _DrvSclVipIoSetLceConfig(s32 s32Handler, DrvSclVipIoLceConfig_t *pstCfg)
{
    MDrvSclVipVersionChkConfig_t stVersion;

    stVersion = _DrvSclVipIoFillVersionChkStruct(sizeof(DrvSclVipIoLceConfig_t),
                                              (pstCfg->VerChk_Size),
                                              &(pstCfg->VerChk_Version));
    if(_DrvSclVipIoVersionCheck(stVersion))
    {
        SCL_ERR( "[VIP]   %s  %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }


    _DrvSclVipIoLogConfigByIP(E_DRV_SCLVIP_IO_LCE_CONFIG,(u8 *)pstCfg);
    if(!MDrvSclVipSetLceConfig((void *)pstCfg))
    {
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    return E_DRV_SCLVIP_IO_ERR_OK;
}


DrvSclVipIoErrType_e _DrvSclVipIoSetUvcConfig(s32 s32Handler, DrvSclVipIoUvcConfig_t *pstCfg)
{
    MDrvSclVipVersionChkConfig_t stVersion;

    stVersion = _DrvSclVipIoFillVersionChkStruct(sizeof(DrvSclVipIoUvcConfig_t),
                                               (pstCfg->VerChk_Size),
                                               &(pstCfg->VerChk_Version));

    if(_DrvSclVipIoVersionCheck(stVersion))
    {
        SCL_ERR( "[VIP]   %s  %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    _DrvSclVipIoLogConfigByIP(E_DRV_SCLVIP_IO_UVC_CONFIG,(u8 *)pstCfg);
    if(!MDrvSclVipSetUvcConfig((void *)pstCfg))
    {
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    return E_DRV_SCLVIP_IO_ERR_OK;
}


DrvSclVipIoErrType_e _DrvSclVipIoSetIhcConfig(s32 s32Handler, DrvSclVipIoIhcConfig_t *pstCfg)
{
    MDrvSclVipVersionChkConfig_t stVersion;

    stVersion = _DrvSclVipIoFillVersionChkStruct(sizeof(DrvSclVipIoIhcConfig_t),
                                              (pstCfg->VerChk_Size),
                                              &(pstCfg->VerChk_Version));
    if(_DrvSclVipIoVersionCheck(stVersion))
    {
        SCL_ERR( "[VIP]   %s  %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    _DrvSclVipIoLogConfigByIP(E_DRV_SCLVIP_IO_IHC_CONFIG,(u8 *)pstCfg);
    if(!MDrvSclVipSetIhcConfig((void *)pstCfg))
    {
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    return E_DRV_SCLVIP_IO_ERR_OK;
}

DrvSclVipIoErrType_e _DrvSclVipIoSetIccConfig(s32 s32Handler, DrvSclVipIoIccConfig_t *pstCfg)
{
    MDrvSclVipVersionChkConfig_t stVersion;

    stVersion = _DrvSclVipIoFillVersionChkStruct(sizeof(DrvSclVipIoIccConfig_t),
                                               (pstCfg->VerChk_Size),
                                               &(pstCfg->VerChk_Version));
    if(_DrvSclVipIoVersionCheck(stVersion))
    {
        SCL_ERR( "[VIP]   %s  %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    _DrvSclVipIoLogConfigByIP(E_DRV_SCLVIP_IO_ICC_CONFIG,(u8 *)pstCfg);
    if(!MDrvSclVipSetICEConfig((void *)pstCfg))
    {
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    return E_DRV_SCLVIP_IO_ERR_OK;
}


DrvSclVipIoErrType_e _DrvSclVipIoSetIhcIceAdpYConfig(s32 s32Handler, DrvSclVipIoIhcIccConfig_t *pstCfg)
{
    MDrvSclVipVersionChkConfig_t stVersion;

    stVersion = _DrvSclVipIoFillVersionChkStruct(sizeof(DrvSclVipIoIhcIccConfig_t),
                                               (pstCfg->VerChk_Size),
                                               &(pstCfg->VerChk_Version));
    if(_DrvSclVipIoVersionCheck(stVersion))
    {
        SCL_ERR( "[VIP]   %s  %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    _DrvSclVipIoLogConfigByIP(E_DRV_SCLVIP_IO_IHCICC_CONFIG,(u8 *)pstCfg);
    if(!MDrvSclVipSetIhcICCADPYConfig((void *)pstCfg))
    {
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    return E_DRV_SCLVIP_IO_ERR_OK;
}


DrvSclVipIoErrType_e _DrvSclVipIoSetIbcConfig(s32 s32Handler, DrvSclVipIoIbcConfig_t *pstCfg)
{
    MDrvSclVipVersionChkConfig_t stVersion;

    stVersion = _DrvSclVipIoFillVersionChkStruct(sizeof(DrvSclVipIoIbcConfig_t),
        (pstCfg->VerChk_Size),
        &(pstCfg->VerChk_Version));

    if(_DrvSclVipIoVersionCheck(stVersion))
    {
        SCL_ERR( "[VIP]   %s  %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    _DrvSclVipIoLogConfigByIP(E_DRV_SCLVIP_IO_IBC_CONFIG,(u8 *)pstCfg);
    if(!MDrvSclVipSetIbcConfig((void *)pstCfg))
    {
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    return E_DRV_SCLVIP_IO_ERR_OK;
}

DrvSclVipIoErrType_e _DrvSclVipIoSetFccConfig(s32 s32Handler, DrvSclVipIoFccConfig_t *pstCfg)
{
    MDrvSclVipVersionChkConfig_t stVersion;

    stVersion = _DrvSclVipIoFillVersionChkStruct(sizeof(DrvSclVipIoFccConfig_t),
                                               (pstCfg->VerChk_Size),
                                               &(pstCfg->VerChk_Version));

    if(_DrvSclVipIoVersionCheck(stVersion))
    {
        SCL_ERR( "[VIP]   %s  %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    _DrvSclVipIoLogConfigByIP(E_DRV_SCLVIP_IO_FCC_CONFIG,(u8 *)pstCfg);
    if(!MDrvSclVipSetFccConfig((void *)pstCfg))
    {
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    return E_DRV_SCLVIP_IO_ERR_OK;
}


DrvSclVipIoErrType_e _DrvSclVipIoSetNlmConfig(s32 s32Handler, DrvSclVipIoNlmConfig_t *pstIoCfg)
{
    MDrvSclVipNlmConfig_t stCfg;
    MDrvSclVipVersionChkConfig_t stVersion;

    stVersion = _DrvSclVipIoFillVersionChkStruct(sizeof(DrvSclVipIoNlmConfig_t),
                                              (pstIoCfg->VerChk_Size),
                                              &(pstIoCfg->VerChk_Version));

    if(_DrvSclVipIoVersionCheck(stVersion))
    {
        SCL_ERR( "[VIP]   %s  %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    DrvSclOsMemcpy(&stCfg, pstIoCfg, sizeof(MDrvSclVipNlmConfig_t));

    _DrvSclVipIoLogConfigByIP(E_DRV_SCLVIP_IO_NLM_CONFIG,(u8 *)pstIoCfg);
    if(!MDrvSclVipSetNlmConfig((void *)&stCfg))
    {
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    if(stCfg.stSRAM.bEn)
    {
        MDrvSclVipSetNlmSramConfig(stCfg.stSRAM);
    }
    else if(!stCfg.stSRAM.bEn && stCfg.stSRAM.u32viradr)
    {
        MDrvSclVipSetNlmSramConfig(stCfg.stSRAM);
    }

    return E_DRV_SCLVIP_IO_ERR_OK;
}

DrvSclVipIoErrType_e _DrvSclVipIoSetAckConfig(s32 s32Handler, DrvSclVipIoAckConfig_t *pstCfg)
{
    MDrvSclVipVersionChkConfig_t stVersion;

    stVersion = _DrvSclVipIoFillVersionChkStruct(sizeof(DrvSclVipIoAckConfig_t),
                                               (pstCfg->VerChk_Size),
                                               &(pstCfg->VerChk_Version));
    if(_DrvSclVipIoVersionCheck(stVersion))
    {
        SCL_ERR( "[VIP]   %s  %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    _DrvSclVipIoLogConfigByIP(E_DRV_SCLVIP_IO_ACK_CONFIG,(u8 *)pstCfg);

    if(!MDrvSclVipSetAckConfig((void *)pstCfg))
    {
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    return E_DRV_SCLVIP_IO_ERR_OK;
}

DrvSclVipIoErrType_e _DrvSclVipIoSetMcnrConfig(s32 s32Handler, DrvSclVipIoMcnrConfig_t *pstIoCfg)
{
    MDrvSclVipMcnrConfig_t stCfg;
    MDrvSclVipVersionChkConfig_t stVersion;

    stVersion = _DrvSclVipIoFillVersionChkStruct(sizeof(DrvSclVipIoMcnrConfig_t),
                                               (pstIoCfg->VerChk_Size),
                                               &(pstIoCfg->VerChk_Version));
    if(_DrvSclVipIoVersionCheck(stVersion))
    {
        SCL_ERR( "[VIP]   %s  %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }
    else
    {
        stCfg.u32Viraddr = pstIoCfg->u32Viraddr;
        stCfg.bEnMCNR = pstIoCfg->bEnMCNR;
        stCfg.bEnCIIR = pstIoCfg->bEnCIIR;
        DrvSclOsMemcpy(&stCfg.stFCfg, &pstIoCfg->stFCfg, sizeof(MDrvSclVipFcconfig_t));
    }

    if(!MDrvSclVipSetMcnrConfig((void *)&stCfg))
    {
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }
    return E_DRV_SCLVIP_IO_ERR_OK;
}


DrvSclVipIoErrType_e _DrvSclVipIoSetAipConfig(s32 s32Handler, DrvSclVipIoAipConfig_t *pstIoConfig)
{
    MDrvSclVipAipConfig_t stCfg;
    MDrvSclVipVersionChkConfig_t stVersion;

    stVersion = _DrvSclVipIoFillVersionChkStruct(sizeof(DrvSclVipIoAipConfig_t),
                                               (pstIoConfig->VerChk_Size),
                                               &(pstIoConfig->VerChk_Version));
    if(_DrvSclVipIoVersionCheck(stVersion))
    {
        SCL_ERR( "[VIP]   %s  %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }
    else
    {
        stCfg.u32Viraddr = pstIoConfig->u32Viraddr;
        stCfg.u16AIPType = (u16)pstIoConfig->enAIPType;
        DrvSclOsMemcpy(&stCfg.stFCfg, &pstIoConfig->stFCfg,sizeof(MDrvSclVipFcconfig_t));
    }

    SCL_DBG(SCL_DBG_LV_IOCTL()&EN_DBGMG_IOCTLEVEL_VIP, "[VIP] IOCTL_NUM:: == %s ==  \n", (AIP_PARSING(stCfg.u16AIPType)));

    if(!MDrvSclVipSetAipConfig(stCfg))
    {
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }
    return E_DRV_SCLVIP_IO_ERR_OK;
}


DrvSclVipIoErrType_e _DrvSclVipIoSetAipSramConfig(s32 s32Handler, DrvSclVipIoAipSramConfig_t *pstIoCfg)
{
    MDrvSclVipAipSramConfig_t stCfg;
    MDrvSclVipVersionChkConfig_t stVersion;

    stVersion = _DrvSclVipIoFillVersionChkStruct(sizeof(DrvSclVipIoAipSramConfig_t),
        (pstIoCfg->VerChk_Size),
        &(pstIoCfg->VerChk_Version));

    if(_DrvSclVipIoVersionCheck(stVersion))
    {
        SCL_ERR( "[VIP]   %s  %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }
    else
    {
        stCfg.u32Viraddr = pstIoCfg->u32Viraddr;
        stCfg.enAIPType = pstIoCfg->enAIPType;
    }

    if(!MDrvSclVipSetAipSramConfig(stCfg))
    {
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    return E_DRV_SCLVIP_IO_ERR_OK;
}

DrvSclVipIoErrType_e _DrvSclVipIoCmdqWriteConfig(s32 s32Handler, DrvSclVipIoCmdqConfig_t *pstIoCfg)
{
    MDrvSclVipCmdqConfig_t stCfg;

    DrvSclOsMemcpy(&stCfg, pstIoCfg, sizeof(DrvSclVipIoCmdqConfig_t));

    if(!MDrvSclVipCmdqWriteConfig(&stCfg))
    {
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    return E_DRV_SCLVIP_IO_ERR_OK;
}


DrvSclVipIoErrType_e _DrvSclVipIoSetLdcConfig(s32 s32Handler, DrvSclVipIoLdcConfig_t *pstCfg)
{
    MDrvSclVipVersionChkConfig_t stVersion;

    stVersion = _DrvSclVipIoFillVersionChkStruct(sizeof(DrvSclVipIoLdcConfig_t),
                                               (pstCfg->VerChk_Size),
                                               &(pstCfg->VerChk_Version));

    if(_DrvSclVipIoVersionCheck(stVersion))
    {
        SCL_ERR( "[VIP]   %s  %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    _DrvSclVipIoLogConfigByIP(E_DRV_SCLVIP_IO_LDC_CONFIG,(u8 *)pstCfg);

    if(!MDrvSclVipSetLdcConfig((void *)pstCfg))
    {
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    return E_DRV_SCLVIP_IO_ERR_OK;
}

DrvSclVipIoErrType_e _DrvSclVipIoSetVipConfig(s32 s32Handler, DrvSclVipIoConfig_t *pstCfg)
{
    MDrvSclVipVersionChkConfig_t stVersion;

    stVersion = _DrvSclVipIoFillVersionChkStruct(sizeof(DrvSclVipIoConfig_t),
                                                      (pstCfg->VerChk_Size),
                                                      &(pstCfg->VerChk_Version));
    if(_DrvSclVipIoVersionCheck(stVersion))
    {
        SCL_ERR( "[VIP]   %s  %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    _DrvSclVipIoLogConfigByIP(E_DRV_SCLVIP_IO_CONFIG,(u8 *)pstCfg);

    if(!MDrvSclVipSetVipOtherConfig((void *)pstCfg))
    {
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    return E_DRV_SCLVIP_IO_ERR_OK;
}

DrvSclVipIoErrType_e _DrvSclVipIoSetLdcMdConfig(s32 s32Handler, DrvSclVipIoLdcMdConfig_t *pstCfg)
{
    MDrvSclVipVersionChkConfig_t stVersion;

    stVersion = _DrvSclVipIoFillVersionChkStruct(sizeof(DrvSclVipIoLdcMdConfig_t),
                                               (pstCfg->VerChk_Size),
                                               &(pstCfg->VerChk_Version));

    if(_DrvSclVipIoVersionCheck(stVersion))
    {
        SCL_ERR( "[VIP]   %s  %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    _DrvSclVipIoLogConfigByIP(E_DRV_SCLVIP_IO_LDC_MD_CONFIG,(u8 *)pstCfg);

    if(!MDrvSclVipSetLdcmdConfig((void *)pstCfg))
    {
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    return E_DRV_SCLVIP_IO_ERR_OK;
}

DrvSclVipIoErrType_e _DrvSclVipIoSetLdcDmapConfig(s32 s32Handler, DrvSclVipIoLdcDmapConfig_t *pstCfg)
{
    MDrvSclVipVersionChkConfig_t stVersion;

    stVersion = _DrvSclVipIoFillVersionChkStruct(sizeof(DrvSclVipIoLdcDmapConfig_t),
                                               (pstCfg->VerChk_Size),
                                               &(pstCfg->VerChk_Version));

    if(_DrvSclVipIoVersionCheck(stVersion))
    {
        SCL_ERR( "[VIP]   %s  %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }


    _DrvSclVipIoLogConfigByIP(E_DRV_SCLVIP_IO_LDC_DMAP_CONFIG,(unsigned char *)pstCfg);

    if(!MDrvSclVipSetLdcDmapConfig((void *)pstCfg))
    {
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    return E_DRV_SCLVIP_IO_ERR_OK;
}

DrvSclVipIoErrType_e _DrvSclVipIoSetLdcSramConfig(s32 s32Handler, DrvSclVipIoLdcSramConfig_t *pstCfg)
{
    MDrvSclVipVersionChkConfig_t stVersion;

    stVersion = _DrvSclVipIoFillVersionChkStruct(sizeof(DrvSclVipIoLdcSramConfig_t),
                                               (pstCfg->VerChk_Size),
                                               &(pstCfg->VerChk_Version));

    if(_DrvSclVipIoVersionCheck(stVersion))
    {
        SCL_ERR( "[VIP]   %s  %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    _DrvSclVipIoLogConfigByIP(E_DRV_SCLVIP_IO_LDC_SRAM_CONFIG,(unsigned char *)pstCfg);

    if(!MDrvSclVipSetLdcSramConfig((void *)pstCfg))
    {
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    return E_DRV_SCLVIP_IO_ERR_OK;
}

DrvSclVipIoErrType_e _DrvSclVipIoSetVtrackConfig(s32 s32Handler, DrvSclVipIoVtrackConfig_t *pstCfg)
{
    MDrvSclVipVersionChkConfig_t stVersion;

    stVersion = _DrvSclVipIoFillVersionChkStruct(sizeof(DrvSclVipIoVtrackConfig_t),
                                               (pstCfg->VerChk_Size),
                                               &(pstCfg->VerChk_Version));

    if(_DrvSclVipIoVersionCheck(stVersion))
    {
        SCL_ERR( "[VIP]   %s  %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    if(pstCfg->bSetKey)
    {
        MDrvSclVipVtrackSetPayloadData(pstCfg->u16Timecode, pstCfg->u8OperatorID);
        MDrvSclVipVtrackSetKey(pstCfg->bSetKey, pstCfg->u8SetKey);
    }
    else
    {
        MDrvSclVipVtrackSetPayloadData(pstCfg->u16Timecode, pstCfg->u8OperatorID);
        MDrvSclVipVtrackSetKey(0,NULL);
    }
    if(pstCfg->bSetUserDef)
    {
        MDrvSclVipVtrackSetUserDefindedSetting(pstCfg->bSetUserDef, pstCfg->u8SetUserDef);
    }
    else
    {
        MDrvSclVipVtrackSetUserDefindedSetting(0,NULL);
    }
    return E_DRV_SCLVIP_IO_ERR_OK;
}


DrvSclVipIoErrType_e _DrvSclVipIoSetVtrackOnOffConfig(s32 s32Handler, DrvSclVipIoVtrackOnOffConfig_t *pstCfg)
{
    MDrvSclVipVersionChkConfig_t stVersion;

    stVersion = _DrvSclVipIoFillVersionChkStruct(sizeof(DrvSclVipIoVtrackOnOffConfig_t),
                                               (pstCfg->VerChk_Size),
                                               &(pstCfg->VerChk_Version));

    if(_DrvSclVipIoVersionCheck(stVersion))
    {
        SCL_ERR( "[VIP]   %s  %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    if(!MDrvSclVipVtrackEnable(pstCfg->u8framerate,  pstCfg->EnType))
    {
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }

    return E_DRV_SCLVIP_IO_ERR_OK;
}

DrvSclVipIoErrType_e _DrvSclVipIoGetVersion(s32 s32Handler, DrvSclVipIoVersionConfig_t *pstCfg)
{
    DrvSclVipIoErrType_e ret = E_DRV_SCLVIP_IO_ERR_OK;

    if (CHK_VERCHK_HEADER( &(pstCfg->VerChk_Version)) )
    {
        if( CHK_VERCHK_MAJORVERSION_LESS( &(pstCfg->VerChk_Version), DRV_SCLVIP_VERSION) )
        {

            VERCHK_ERR("[VIP] Version(%04lx) < %04x!!! \n",
                pstCfg->VerChk_Version & VERCHK_VERSION_MASK,
                DRV_SCLVIP_VERSION);

            ret = E_DRV_SCLVIP_IO_ERR_INVAL;
        }
        else
        {
            if( CHK_VERCHK_SIZE( &(pstCfg->VerChk_Size), sizeof(DrvSclVipIoVersionConfig_t)) == 0 )
            {
                VERCHK_ERR("[VIP] Size(%04x) != %04lx!!! \n",
                    sizeof(DrvSclVipIoVersionConfig_t),
                    (pstCfg->VerChk_Size));

                ret = E_DRV_SCLVIP_IO_ERR_INVAL;
            }
            else
            {
                DrvSclVipIoVersionConfig_t stCfg;

                stCfg = FILL_VERCHK_TYPE(stCfg, stCfg.VerChk_Version, stCfg.VerChk_Size, DRV_SCLVIP_VERSION);
                stCfg.u32Version = DRV_SCLVIP_VERSION;
                DrvSclOsMemcpy(pstCfg, &stCfg, sizeof(DrvSclVipIoVersionConfig_t));
                ret = E_DRV_SCLVIP_IO_ERR_OK;
            }
        }
    }
    else
    {
        VERCHK_ERR("[VIP] No Header !!! \n");
        SCL_ERR( "[VIP]   %s  %d\n", __FUNCTION__, __LINE__);
        ret = E_DRV_SCLVIP_IO_ERR_INVAL;
    }

    return ret;
}

DrvSclVipIoErrType_e _DrvSclVipIoSetAllVip(s32 s32Handler, DrvSclVipIoAllSetConfig_t *pstIoCfg)
{
    MDrvSclVipAllSEtConfig_t stCfg;
    MDrvSclVipVersionChkConfig_t stVersion;
    stVersion = _DrvSclVipIoFillVersionChkStruct(sizeof(DrvSclVipIoAllSetConfig_t),
                                               (pstIoCfg->VerChk_Size),
                                               &(pstIoCfg->VerChk_Version));

    if(_DrvSclVipIoVersionCheck(stVersion))
    {
        SCL_ERR( "[VIP]   %s  %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLVIP_IO_ERR_FAULT;
    }
    else
    {
        DrvSclOsMemcpy(&stCfg, pstIoCfg, sizeof(DrvSclVipIoAllSetConfig_t));
    }

    MDrvSclVipSetAllVipOneShot(&stCfg.stvipCfg);
    return E_DRV_SCLVIP_IO_ERR_OK;
}

