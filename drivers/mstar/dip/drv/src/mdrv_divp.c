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
//==============================================================================
// [hal_divp.c]
// Descriptions: De-interlace and Video Post Process Engine driver for HW setting
//==============================================================================
#define  HAL_DIVP_C

#ifdef MSOS_TYPE_LINUX_KERNEL
    #include <asm/div64.h>
#else
    #define do_div(x,y) ((x)/=(y))
#endif

#include <linux/string.h>
#include <linux/slab.h>
//#include <linux/delay.h>
//#include <linux/jiffies.h>
//#include <asm/io.h>
#include <linux/kernel.h>

#include "MsCommon.h"
#include "apiXC.h"
#include "apiXC_DWIN_v2.h"
#include "mhal_common.h"
#include "mhal_divp_datatype.h"
#include "mhal_divp.h"
#include "mhal_dip.h"

#define MHAL_SUCCESS    (0)
#define MHAL_FAILURE    (-1)

#define DIP_MIU                 0

#define MAX_WIDTH    4096
#define MAX_HEIGHT   4096

#define MAX_CHANNEL_NUM 33
#define DI_BUFFER_CNT 4

#if (DIP_PLATFORM == DIP_K6)
    #if 1  //
        #define _3DDI_BUFFER_OFFSET    (0x000DC22000)
    #else
        #define _3DDI_BUFFER_OFFSET    (0x000EC22000)
    #endif
    #define _3DDI_BUFFER_SIZE      (0x00600000)
#endif

typedef struct
{
    MS_U8     u8BufNum;
    MS_U32    u32BaseAddr;
    MS_U32    u32BufSize;
    MS_U32    u32DISize;// = u32DIPDISize;
    MS_U8     u8WrInx;
    MS_U8     u8RdInx;
} MHAL_DIVP_3ddiInfo_t;

typedef struct
{
    MHAL_DIVP_TnrLevel_e eTnrLevel;//TNR level
    MHAL_DIVP_DiType_e eDiType;//DI type
    MHAL_DIVP_Rotate_e eRotateType;//rotate angle
    MHAL_DIVP_Window_t stCropWin;//crop information
    MHAL_DIVP_Mirror_t stMirror;
} MHAL_DIVP_AttrParams_t;

typedef struct
{
    MS_BOOL bInit;
    MHAL_DIVP_DeviceId_e eId;
    MS_U8 u8ChannelId;
    MS_U16 nMaxWidth;
    MS_U16 nMaxHeight;
    MHAL_DIVP_AttrParams_t stAttr;
    MHAL_DIVP_3ddiInfo_t st3dDiInfo;
    MS_U64 u64AllocAddr;
    MS_U32 u32AllocSize;
    PfnAlloc pfAlloc;
    PfnFree pfFree;
    MS_BOOL bIsDiEnable;
    MS_U32 u32CurFrameCnt;
    char  pCtxString[32];
} __attribute__ ((__packed__)) MHAL_DIVP_InstPrivate_t;

typedef struct
{
    MS_U8 u8NumOpening;
    SCALER_DIP_WIN enDipWin;
    MS_BOOL bIsInit;
    //di buffer info
    MS_U8 u8NwayMaxBufCnt;
    ST_DIP_DI_SETTING st3DDISetting;
    ST_XC_DIPR_PROPERTY_EX stDiprInfo;
    PfnAlloc pfAlloc;
    PfnFree pfFree;
    MHAL_DIVP_InstPrivate_t *ctx[MAX_CHANNEL_NUM];
} __attribute__ ((__packed__)) DivpPrivateDevData;


extern XC_DIP_InternalStatus  g_DIPSrcInfo[MAX_DIP_WINDOW];
SC_DIP_SWDB_INFO g_stDBreg = {0};
extern void DIP_DelayTask(MS_U32 u32Ms);
char g_CtxName[32] = "DIVP_Ctx";

static ST_DIP_DI_NWAY_BUF_INFO stDINwayBuf[MAX_CHANNEL_NUM];
static DivpPrivateDevData g_stDipRes[MAX_DIP_WINDOW];
static MS_BOOL g_bIsInit[MAX_DIP_WINDOW] = {FALSE, FALSE, FALSE};
static DIP_OUTPUT_PATH g_DIVPOutPath = DIP_OUTPUT_PATH_DRAM;

int debug_level = 0;

static void Int2Str(MS_U16 value, char *string)
{
    MS_U32   i, j;
    MS_U8    temp[3];

    for (i = 0; i < 3; i++)
    {
        j = value / 10;
        if ( j != 0 )
        {
            temp[i] = '0' + (value - j * 10); //tbd value % 10;

            value = j;
        }
        else
        {
            temp[i] = '0' + (value - j * 10); //tbd value % 10;
            break;
        }
    }

    for(j = 0; j < (i + 1); j++)
    {
        string[j] = temp[i - j];
    }

    string[j] = 0;
}

void mDrv_XC_DIVP_SourceType_Set(SCALER_DIP_WIN eWindow,
                                 INPUT_SOURCE_TYPE_t enInputSourceType,
                                 SCALER_DIP_SOURCE_TYPE  enDIPSourceType,
                                 MS_BOOL bInterlace,
                                 MS_BOOL bHDuplicate)
{
    P_XC_DIP_InternalStatus pDIPSrcInfo = &g_DIPSrcInfo[eWindow];

    pDIPSrcInfo->enInputSourceType = enInputSourceType;
    pDIPSrcInfo->enDIPSourceType = enDIPSourceType;

    pDIPSrcInfo->bInterlace            = bInterlace;
    pDIPSrcInfo->bHDuplicate           = bHDuplicate;
}

void mDrv_XC_DIVP_Crop_Set(SCALER_DIP_WIN eWindow,
                           MS_U16 u16X,
                           MS_U16 u16Y,
                           MS_U16 u16Width,
                           MS_U16 u16Height)
{
    P_XC_DIP_InternalStatus pDIPSrcInfo = &g_DIPSrcInfo[eWindow];

    pDIPSrcInfo->stCapWin.x = u16X;
    pDIPSrcInfo->stCapWin.y = u16Y;
    pDIPSrcInfo->stCapWin.width = u16Width;
    pDIPSrcInfo->stCapWin.height = u16Height;
}

void mDrv_XC_DIVP_Scale_Set(SCALER_DIP_WIN eWindow,
                            MS_U16 u16SrcWidth,
                            MS_U16 u16SrcHeight,
                            MS_U16 u16DstWidth,
                            MS_U16 u16DstHeight)
{
    P_XC_DIP_InternalStatus pDIPSrcInfo = &g_DIPSrcInfo[eWindow];

#if (DIP_PLATFORM == DIP_I2)

    // Don't support VSD in I2, move scaling down to HVSP engine
    pDIPSrcInfo->bPreHCusScaling = FALSE;
    pDIPSrcInfo->u16PreHCusScalingSrc = u16SrcWidth;
    pDIPSrcInfo->u16PreHCusScalingDst = u16DstWidth;

    pDIPSrcInfo->bPreVCusScaling = FALSE;
    pDIPSrcInfo->u16PreVCusScalingSrc = u16SrcHeight;
    pDIPSrcInfo->u16PreVCusScalingDst = u16DstHeight;

    pDIPSrcInfo->u16H_SizeAfterPreScaling = u16DstWidth;
    pDIPSrcInfo->u16V_SizeAfterPreScaling = u16DstHeight;

#else

    if (u16SrcWidth > u16DstWidth)
        pDIPSrcInfo->bPreHCusScaling = TRUE;
    else
        pDIPSrcInfo->bPreHCusScaling = FALSE;
    pDIPSrcInfo->u16PreHCusScalingSrc = u16SrcWidth;
    pDIPSrcInfo->u16PreHCusScalingDst = u16DstWidth;
    if (u16SrcHeight > u16DstHeight)
        pDIPSrcInfo->bPreVCusScaling = TRUE;
    else
        pDIPSrcInfo->bPreVCusScaling = FALSE;
    pDIPSrcInfo->u16PreVCusScalingSrc = u16SrcHeight;
    pDIPSrcInfo->u16PreVCusScalingDst = u16DstHeight;

    pDIPSrcInfo->u16H_SizeAfterPreScaling = u16DstWidth;
    pDIPSrcInfo->u16V_SizeAfterPreScaling = u16DstHeight;

#endif
}

EN_DRV_XC_DWIN_DATA_FMT MDrv_XC_DIVP_TransPixelFormat(MHAL_DIVP_PixelFormat_e InFmt)
{
    EN_DRV_XC_DWIN_DATA_FMT OutFmt;

    switch (InFmt)
    {
        case E_MHAL_DIVP_PIXEL_FORMAT_YUV422_YUYV :
            OutFmt = XC_DWIN_DATA_FMT_YUV422;
            break;
        case E_MHAL_DIVP_PIXEL_FORMAT_YUV_SEMIPLANAR_422 :
            OutFmt = XC_DWIN_DATA_FMT_YC422;
            break;
        case E_MHAL_DIVP_PIXEL_FORMAT_RGB565 :
            OutFmt = XC_DWIN_DATA_FMT_RGB565;
            break;
        case E_MHAL_DIVP_PIXEL_FORMAT_ARGB8888 :
        case E_MHAL_DIVP_PIXEL_FORMAT_ABGR8888 :
            OutFmt = XC_DWIN_DATA_FMT_ARGB8888;
            break;
        case E_MHAL_DIVP_PIXEL_FORMAT_YUV_SEMIPLANAR_420 :
            OutFmt = XC_DWIN_DATA_FMT_YUV420_SEMI_PLANER;  /// YUV420 semi planer
            break;
        case E_MHAL_DIVP_PIXEL_FORMAT_YUV_MST_420 :  // MDWin output path
            OutFmt = XC_DWIN_DATA_FMT_YUV420_SEMI_PLANER;
            break;
        case E_MHAL_DIVP_PIXEL_FORMAT_YC420_MSTTILE1_H264 :
            OutFmt = XC_DWIN_DATA_FMT_YUV420;  /// YUV420 HVD tile fmt
            break;
        case E_MHAL_DIVP_PIXEL_FORMAT_YC420_MSTTILE2_H265 :
            OutFmt = XC_DWIN_DATA_FMT_YUV420_H265;
            break;
        case E_MHAL_DIVP_PIXEL_FORMAT_YC420_MSTTILE3_H265 :
            OutFmt = XC_DWIN_DATA_FMT_YUV420_H265_10BITS;
            break;
        default :
            OutFmt = XC_DWIN_DATA_FMT_YUV422;
            break;
    }

    return OutFmt;
}

EN_XC_DIP_TILE_BLOCK MDrv_XC_DIVP_TransTileMode(MHAL_DIVP_TileMode_e InTileMode)
{
    EN_XC_DIP_TILE_BLOCK OutTileMode;

    switch (InTileMode)
    {
        case E_MHAL_DIVP_TILE_MODE_32x32 :
            OutTileMode = DIP_TILE_BLOCK_R_32_32;
            break;
        case E_MHAL_DIVP_TILE_MODE_32x16 :
            OutTileMode = DIP_TILE_BLOCK_R_32_16;
            break;
        case E_MHAL_DIVP_TILE_MODE_16x32 :
            OutTileMode = DIP_TILE_BLOCK_R_16_32;
            break;
        default :
            OutTileMode = DIP_TILE_BLOCK_R_32_32;
            break;
    }

    return OutTileMode;
}

SCALER_DIP_SOURCE_TYPE MDrv_XC_DIVP_GetSrcType(MHAL_DIVP_DisplayId_e eDispId, MHAL_DIVP_CapStage_e eCapStage)
{
    SCALER_DIP_SOURCE_TYPE eInSrcType = SCALER_DIP_SOURCE_TYPE_OP_CAPTURE;

    FUNC_MSG("eDispId=%d, eCapStage=%d\n", eDispId, eCapStage);

    if(eDispId == E_MHAL_DIVP_Display0)
    {
        if(eCapStage == E_MHAL_DIVP_CAP_STAGE_INPUT)
        {
            eInSrcType = SCALER_DIP_SOURCE_TYPE_MAIN;
        }
        else if((eCapStage == E_MHAL_DIVP_CAP_STAGE_OUTPUT) || (eCapStage == E_MHAL_DIVP_CAP_STAGE_OUTPUT_WITH_OSD))
        {
            eInSrcType = SCALER_DIP_SOURCE_TYPE_OP_CAPTURE;
        }
        else
        {
            eInSrcType = MAX_SCALER_DIP_SOURCE_NUM;
            FUNC_ERR("Don't support capture source stage: %d\n", eCapStage);
        }
        return eInSrcType;
    }
    else if(eDispId == E_MHAL_DIVP_Display1)
    {
        if(eCapStage == E_MHAL_DIVP_CAP_STAGE_INPUT)
        {
            eInSrcType = SCALER_DIP_SOURCE_TYPE_MAIN;
        }
        else if((eCapStage == E_MHAL_DIVP_CAP_STAGE_OUTPUT) || (eCapStage == E_MHAL_DIVP_CAP_STAGE_OUTPUT_WITH_OSD))
        {
            eInSrcType = SCALER_DIP_SOURCE_TYPE_OP_SC1_CAPTURE;
        }
        else
        {
            eInSrcType = MAX_SCALER_DIP_SOURCE_NUM;
            FUNC_ERR("Don't support capture source stage: %d\n", eCapStage);
        }
        return eInSrcType;
    }
    else
    {
        //not support
        eInSrcType = MAX_SCALER_DIP_SOURCE_NUM;
        FUNC_ERR("Don't support capture source ID: %d\n", eDispId);
    }

    return eInSrcType;
}

EN_MFDEC_TILE_MODE _DipMFDecTileModeTransform(MHAL_DIVP_TileMode_e eHalTile)
{
    EN_MFDEC_TILE_MODE eDipTile = E_DIP_MFEDC_TILE_16_32;
    switch (eHalTile)
    {
        case E_MHAL_DIVP_TILE_MODE_16x32:
            eDipTile = E_DIP_MFEDC_TILE_16_32;
            break;
        case E_MHAL_DIVP_TILE_MODE_32x16:
            eDipTile = E_DIP_MFEDC_TILE_32_16;
            break;
        case E_MHAL_DIVP_TILE_MODE_32x32:
            eDipTile = E_DIP_MFEDC_TILE_32_32;
            break;
        default:
            eDipTile = E_DIP_MFEDC_TILE_16_32;
            break;
    }
    return eDipTile;
}

EN_DIP_DI_FIELD _DipGetDiFieldType(MHAL_DIVP_DiType_e eDiType, MHAL_DIVP_ScanMode_e eScanType, MHAL_DIVP_FieldType_e eFieldType)
{
    EN_DIP_DI_FIELD eDIField = E_DIP_3DDI_TOP_SINGLE_FIELD;

    FUNC_MSG("eDiType=%d, eScanType=%d, eFieldType=%d\n", eDiType, eScanType, eFieldType);

    if(eDiType == E_MHAL_DIVP_DI_TYPE_3D)
    {
        if(eScanType == E_MHAL_DIVP_SCAN_MODE_INTERLACE_FRAME)
        {
            if(eFieldType == E_MHAL_DIVP_FIELD_TYPE_TOP)
            {
                FUNC_MSG("E_MHAL_DIVP_DI_TYPE_3D >>> [E_DIP_3DDI_TOP_BOTH_FIELD_STAGGER]\n");
                eDIField = E_DIP_3DDI_TOP_BOTH_FIELD_STAGGER;
            }
            else if(eFieldType == E_MHAL_DIVP_FIELD_TYPE_BOTTOM)
            {
                FUNC_MSG("E_MHAL_DIVP_DI_TYPE_3D >>> [E_DIP_3DDI_BOTTOM_BOTH_FIELD_STAGGER]\n");
                eDIField = E_DIP_3DDI_BOTTOM_BOTH_FIELD_STAGGER;
            }
        }
        else if(eScanType == E_MHAL_DIVP_SCAN_MODE_INTERLACE_FIELD)
        {
            if(eFieldType == E_MHAL_DIVP_FIELD_TYPE_TOP)
            {
                eDIField = E_DIP_3DDI_TOP_SINGLE_FIELD;
                FUNC_MSG("E_MHAL_DIVP_DI_TYPE_3D >>> [E_DIP_3DDI_TOP_SINGLE_FIELD]\n");
            }
            else if(eFieldType == E_MHAL_DIVP_FIELD_TYPE_BOTTOM)
            {
                eDIField = E_DIP_3DDI_BOTTOM_SINGLE_FIELD;
                FUNC_MSG("E_MHAL_DIVP_DI_TYPE_3D >>> [E_DIP_3DDI_BOTTOM_SINGLE_FIELD]\n");
            }
        }
        return eDIField;
    }
    else if(eDiType == E_MHAL_DIVP_DI_TYPE_MED)
    {
        FUNC_MSG("E_MHAL_DIVP_DI_TYPE_MED\n");
        if(eScanType == E_MHAL_DIVP_SCAN_MODE_INTERLACE_FRAME)
        {
            if(eFieldType == E_MHAL_DIVP_FIELD_TYPE_TOP)
            {
                eDIField = E_DIP_MED_DI_TOP_BOTH_FIELD_STAGGER;
            }
            else if(eFieldType == E_MHAL_DIVP_FIELD_TYPE_BOTTOM)
            {
                eDIField = E_DIP_MED_DI_BOTTOM_BOTH_FIELD_STAGGER;
            }
        }
        else if(eScanType == E_MHAL_DIVP_SCAN_MODE_INTERLACE_FIELD)
        {
            if(eFieldType == E_MHAL_DIVP_FIELD_TYPE_TOP)
            {
                eDIField = E_DIP_MED_DI_TOP_BOTH_FIELD_SEPARATE;
            }
            else if(eFieldType == E_MHAL_DIVP_FIELD_TYPE_BOTTOM)
            {
                eDIField = E_DIP_MED_DI_BOTTOM_BOTH_FIELD_SEPARATE;
            }
        }
        return eDIField;
    }
    else
    {
        //not support
        FUNC_ERR("DI type don't support: %d\n", eDiType);
    }

    return eDIField;
}

DIP_OUTPUT_PATH MDrv_XC_DIVP_GetDestinationPath(MHAL_DIVP_PixelFormat_e OutFmt)
{
    if(OutFmt == E_MHAL_DIVP_PIXEL_FORMAT_YUV_MST_420)
    {
        FUNC_MSG("=====[DIP_OUTPUT_MDWIN]=====\n");
        return DIP_OUTPUT_PATH_MDWIN;
    }
    else
    {
        FUNC_MSG("=====[DIP_OUTPUT_DRAM]=====\n");
        return DIP_OUTPUT_PATH_DRAM;
    }
}

void MDrv_XC_DIVP_EnableIntr(void* pstCmdQInfo, MS_U16 u8mask, MS_BOOL bEnable, SCALER_DIP_WIN eWindow)
{
    HAL_XC_DIP_EnableIntr(pstCmdQInfo, u8mask, bEnable, eWindow);
}
void MDrv_XC_DIVP_ClearIntr(void* pstCmdQInfo, MS_U16 u16mask, SCALER_DIP_WIN eWindow)
{
    HAL_XC_DIP_ClearIntr(pstCmdQInfo, u16mask, eWindow);
}

void MDrv_XC_DIVP_CMDQ_EnableIntr(void* pstCmdQInfo, MS_U16 u8mask, MS_BOOL bEnable, SCALER_DIP_WIN eWindow)
{
    HAL_XC_DIP_CMDQ_EnableIntr(pstCmdQInfo, u8mask, bEnable, eWindow);
}
void MDrv_XC_DIVP_CMDQ_ClearIntr(void* pstCmdQInfo, MS_U16 u16mask, SCALER_DIP_WIN eWindow)
{
    HAL_XC_DIP_CMDQ_ClearIntr(pstCmdQInfo, u16mask, eWindow);
}

void MDrv_XC_DIVP_MDWIN_EnableIntr(void* pstCmdQInfo, MS_BOOL bEnable, SCALER_DIP_WIN eWindow)
{
    HAL_XC_DIP_MDWIN_EnableIntr(pstCmdQInfo, bEnable, eWindow);
}
void MDrv_XC_DIVP_MDWIN_ClearIntr(void* pstCmdQInfo, SCALER_DIP_WIN eWindow)
{
    HAL_XC_DIP_MDWIN_ClearIntr(pstCmdQInfo, eWindow);
}

MS_U16 MDrv_XC_DIVP_MDWIN_GetIntrStatus(void* pstCmdQInfo)
{
    return HAL_XC_DIP_MDWIN_GetIntrStatus(pstCmdQInfo, DIP_WINDOW);
}

void MDrv_XC_DIVP_COVER_SetConfig(void* pstCmdQInfo, DIP_COVER_PROPERTY *stDIPCoverProperty)
{
    HAL_XC_DIP_COVER_SetConfig(pstCmdQInfo, stDIPCoverProperty);
}

#if 0
void MDrv_XC_DIVP_InterruptAttach(void* pstCmdQInfo, InterruptCb pIntCb, SCALER_DIP_WIN eWindow)
{
    HAL_XC_DIP_InterruptAttach(pstCmdQInfo, pIntCb, eWindow);
}
void MDrv_XC_DIVP_InterruptDetach(void* pstCmdQInfo, SCALER_DIP_WIN eWindow)
{
    HAL_XC_DIP_InterruptDetach(pstCmdQInfo, eWindow);
}
#endif

void MDrv_XC_DIVP_Init(void* pstCmdQInfo, SCALER_DIP_WIN eWindow)
{
    SC_DIP_SWDB_INFO stDBreg = {1, 16, 1, 16, 0, 0, 27, 27};

    Hal_SC_DWIN_sw_db(pstCmdQInfo, &stDBreg, eWindow);

    HAL_XC_DIP_Init(pstCmdQInfo, eWindow);
    HAL_XC_DIP_SetFRC(pstCmdQInfo, FALSE, 1, 1, eWindow);
    MDrv_XC_DIVP_ClearIntr(pstCmdQInfo, 0xFF, eWindow);

#ifndef DIP_UT_ENABLE_RIU  // for CMDQ
    MDrv_XC_DIVP_CMDQ_ClearIntr(pstCmdQInfo, 0xFF, eWindow);
    MDrv_XC_DIVP_CMDQ_EnableIntr(pstCmdQInfo, 0xFF, TRUE, eWindow);
#endif
}

void MDrv_XC_DIVP_DeInit(void* pstCmdQInfo, SCALER_DIP_WIN eWindow)
{
#ifndef DIP_UT_ENABLE_RIU  // for CMDQ
    MDrv_XC_DIVP_CMDQ_EnableIntr(pstCmdQInfo, 0xFF, FALSE, eWindow);
    MDrv_XC_DIVP_CMDQ_ClearIntr(pstCmdQInfo, 0xFF, eWindow);
#endif
}

void MDrv_XC_DIVP_SetDataFmt(void* pstCmdQInfo, EN_DRV_XC_DWIN_DATA_FMT fmt, DIP_OUTPUT_PATH outpath, SCALER_DIP_WIN eWindow)
{
    HAL_XC_DIP_SetDataFmt(pstCmdQInfo, fmt, outpath, eWindow);
}

void MDrv_XC_DIVP_SelectSourceScanType(void* pstCmdQInfo, EN_XC_DWIN_SCAN_TYPE enScan, SCALER_DIP_WIN eWindow)
{
    HAL_XC_DIP_SelectSourceScanType(pstCmdQInfo, enScan, eWindow);
}

void MDrv_XC_DIVP_SetMirror(void* pstCmdQInfo, MS_BOOL bHMirror, MS_BOOL bVMirror, SCALER_DIP_WIN eWindow)
{
    HAL_XC_DIP_SetMirror(pstCmdQInfo, bHMirror, bVMirror, eWindow);
}

void MDrv_XC_DIVP_SetInterlaceWrite(void* pstCmdQInfo, MS_BOOL bEnable, SCALER_DIP_WIN eWindow)
{
    HAL_XC_DIP_SetInterlaceWrite(pstCmdQInfo, bEnable, eWindow);
}

MS_BOOL MDrv_XC_DIVP_Set420TileBlock(void* pstCmdQInfo, EN_XC_DIP_TILE_BLOCK eTileBlock, SCALER_DIP_WIN eWindow)
{
    MS_BOOL bSupport = FALSE;

    bSupport = HAL_XC_DIP_Set420TileBlock(pstCmdQInfo, eTileBlock, eWindow);

    return bSupport;
}

void MDrv_XC_DIVP_SetRotation(void* pstCmdQInfo, MS_BOOL bRotation, EN_XC_DIP_ROTATION eRoDirection, SCALER_DIP_WIN eTmpWindow)
{
    HAL_XC_DIP_Rotation(pstCmdQInfo, bRotation, eRoDirection, 0, eTmpWindow);
}

void MDrv_XC_DIVP_SetDIPRProperty(void* pstCmdQInfo, MHAL_DIVP_InstPrivate_t* pInstance, ST_XC_DIPR_PROPERTY *pstDIPRProperty, SCALER_DIP_WIN eWindow)
{
    //MS_U8 u8MIUSel = 0, u8MIUSelTMP = 0;
    MS_U16 u16Bpp = 0, u16Width = 0, u16LineOft = 0;
    EN_DRV_XC_DWIN_DATA_FMT fmt = 0;
    MS_U64 u64_result = 0;

    //FUNC_MSG("[DIPR]u16Pitch=%d\n", pstDIPRProperty->u16Pitch);

    //Transfer pitch to lineoffset
    u16Bpp = HAL_XC_DIP_GetBPP(pstCmdQInfo, (EN_DRV_XC_DWIN_DATA_FMT)(pstDIPRProperty->enDataFmt), eWindow);
    if( pstDIPRProperty->u16Pitch == 0 )
    {
        u16LineOft = u16Width;
    }
    else
    {
#if 0

        u16LineOft = pstDIPRProperty->u16Pitch;

#else

        fmt = (EN_DRV_XC_DWIN_DATA_FMT)(pstDIPRProperty->enDataFmt);
        //if( (fmt == XC_DWIN_DATA_FMT_YUV420) || (fmt == XC_DWIN_DATA_FMT_YUV420_H265) || (fmt == XC_DWIN_DATA_FMT_YUV420_H265_10BITS))
        //{
        //    u64_result = (pstDIPRProperty->u16Pitch * 2);
        //    do_div(u64_result, 3);
        //    u16LineOft = u64_result;
        //}
        //else
        {
            u64_result = (pstDIPRProperty->u16Pitch);
            do_div(u64_result, u16Bpp);
            u16LineOft = u64_result;
        }

        if(u16LineOft < u16Width)
        {
            FUNC_ERR("Pitch is not correct\n");
            return;
        }

#endif

    }
    pstDIPRProperty->u16Pitch = u16LineOft;

    FUNC_MSG("[DIPR]u16Bpp=%d, u16LineOft=%d\n", u16Bpp, u16LineOft);

    // Setup Properties
    HAL_XC_DIP_SetDIPRMiuSel(pstCmdQInfo, DIP_MIU, DIP_MIU, eWindow);
    HAL_XC_DIP_SetDIPRProperty(pstCmdQInfo, pstDIPRProperty, eWindow);

}

void MDrv_XC_DIVP_SetDIPRProperty_EX(void* pstCmdQInfo, MHAL_DIVP_InstPrivate_t* pInstance, ST_XC_DIPR_PROPERTY_EX *pstDIPRProperty_ex, SCALER_DIP_WIN eWindow)
{
    //MS_U8 u8MIUSel = 0, u8MIUSelTMP = 0;
    MS_U16 u16Bpp = 0, u16Width = 0, u16LineOft = 0;
    EN_DRV_XC_DWIN_DATA_FMT fmt = 0;
    MS_U64 u64_result = 0;

    //FUNC_MSG("[DIPR]u16Pitch=%d\n", pstDIPRProperty_ex->stDIPRProperty.u16Pitch);

    //Transfer pitch to lineoffset
    u16Bpp = HAL_XC_DIP_GetBPP(pstCmdQInfo, (EN_DRV_XC_DWIN_DATA_FMT)(pstDIPRProperty_ex->stDIPRProperty.enDataFmt), eWindow);
    if( pstDIPRProperty_ex->stDIPRProperty.u16Pitch == 0 )
    {
        u16LineOft = u16Width;
    }
    else
    {
#if 0

        u16LineOft = pstDIPRProperty_ex->stDIPRProperty.u16Pitch;

#else

        fmt = (EN_DRV_XC_DWIN_DATA_FMT)(pstDIPRProperty_ex->stDIPRProperty.enDataFmt);
        //if( (fmt == XC_DWIN_DATA_FMT_YUV420) || (fmt == XC_DWIN_DATA_FMT_YUV420_H265) || (fmt == XC_DWIN_DATA_FMT_YUV420_H265_10BITS))
        //{
        //    u64_result = (pstDIPRProperty_ex->stDIPRProperty.u16Pitch * 2);
        //    do_div(u64_result, 3);
        //    u16LineOft = u64_result;
        //}
        //else
        {
            u64_result = (pstDIPRProperty_ex->stDIPRProperty.u16Pitch);
            do_div(u64_result, u16Bpp);
            u16LineOft = u64_result;
        }

        if(u16LineOft < u16Width)
        {
            FUNC_ERR("Pitch is not correct\n");
            return;
        }

#endif

    }
    pstDIPRProperty_ex->stDIPRProperty.u16Pitch = u16LineOft;

    FUNC_MSG("[DIPR]u16Bpp=%d, u16LineOft=%d\n", u16Bpp, u16LineOft);

    // Setup Properties
    HAL_XC_DIP_SetDIPRMiuSel(pstCmdQInfo, DIP_MIU, DIP_MIU, eWindow);
    HAL_XC_DIP_SetDIPRProperty(pstCmdQInfo, &pstDIPRProperty_ex->stDIPRProperty, eWindow);
    HAL_XC_DIP_SetDIPRProperty_MFDEC(pstCmdQInfo, pstDIPRProperty_ex->stDIPR_MFDecInfo, eWindow);
    HAL_XC_DIP_SetDIPRProperty_DI(pstCmdQInfo, &pstDIPRProperty_ex->stDIPRProperty, &pstDIPRProperty_ex->stDIPR3DDISetting, eWindow);

}

#define OFFSET_PIXEL_ALIGNMENT  (64)

void MDrv_XC_DIVP_set_fetch_number_limit(void* pInstance, XC_DIP_InternalStatus *pSrcInfo, SCALER_DIP_WIN eWindow)
{

    /////////////////////////////////////////////////
    // Set DNR Memeory Settings                    //
    /////////////////////////////////////////////////
    // Cal DNR Fetch/Offset
    g_stDBreg.u16DNRFetch  = pSrcInfo->u16H_SizeAfterPreScaling;
    g_stDBreg.u16DNROffset = (g_stDBreg.u16DNRFetch + OFFSET_PIXEL_ALIGNMENT - 1) & ~(OFFSET_PIXEL_ALIGNMENT - 1); //TODO: T3 should be 32 alignment;

    //FUNC_MSG("u16DNRFetch=%u\n", g_stDBreg.u16DNRFetch);
    //FUNC_MSG("u16DNROffset=%u\n", g_stDBreg.u16DNROffset);

}

/******************************************************************************/
///Set Capture window (Input window) information.
///@param pstWindow \b IN
///- pointer to window information
/******************************************************************************/
void MDrv_XC_DIVP_set_capture_window(void* pInstance, SCALER_DIP_WIN eWindow)
{
    if(g_DIPSrcInfo[eWindow].bInterlace)
    {
        //For interlace, set clip window V Pos/Size= field Pos/Size
        g_stDBreg.u16V_CapStart = (g_DIPSrcInfo[eWindow].stCapWin.y) >> 1;
        g_stDBreg.u16H_CapStart = g_DIPSrcInfo[eWindow].stCapWin.x;
        g_stDBreg.u16V_CapSize  = (g_DIPSrcInfo[eWindow].stCapWin.height) >> 1;
        g_stDBreg.u16H_CapSize  = g_DIPSrcInfo[eWindow].stCapWin.width;
    }
    else
    {
        g_stDBreg.u16V_CapStart = g_DIPSrcInfo[eWindow].stCapWin.y;
        g_stDBreg.u16H_CapStart = g_DIPSrcInfo[eWindow].stCapWin.x;
        g_stDBreg.u16V_CapSize  = g_DIPSrcInfo[eWindow].stCapWin.height;
        g_stDBreg.u16H_CapSize  = g_DIPSrcInfo[eWindow].stCapWin.width;
    }
}

void MDrv_XC_DIVP_SetWindow(void* pstCmdQInfo, MHAL_DIVP_InstPrivate_t* pInstance, MHAL_DIVP_OutPutInfo_t* pstDivpOutputInfo, SCALER_DIP_WIN eWindow)
{
    MS_U32 u32TmpPre;
    XC_SETWIN_INFO stXC_SetWin_Info;
    XC_DIP_InternalStatus *pSrcInfo;
    MS_BOOL bInterlace = g_DIPSrcInfo[eWindow].bInterlace;
    MS_U16 u16ScaleSrc = 0, u16ScaleDst = 0;
    MS_U16 u16SrcHWidth = 0;

    FUNC_MSG("========= SetWindow Start (Window : %u, Insrc: %u, DIPsrc: %u) ======\n", eWindow, g_DIPSrcInfo[eWindow].enInputSourceType, g_DIPSrcInfo[eWindow].enDIPSourceType);
    FUNC_MSG("CapWin  x: %4u  y: %4u  w: %4u  h: %4u \n", g_DIPSrcInfo[eWindow].stCapWin.x, g_DIPSrcInfo[eWindow].stCapWin.y, g_DIPSrcInfo[eWindow].stCapWin.width, g_DIPSrcInfo[eWindow].stCapWin.height);
    //FUNC_MSG("Interlace/Hdup = %u/%u\n", g_DIPSrcInfo[eWindow].bInterlace, g_DIPSrcInfo[eWindow].bHDuplicate);
    FUNC_MSG("Pre  H cus scaling %u (%u -> %u)\n", g_DIPSrcInfo[eWindow].bPreHCusScaling, g_DIPSrcInfo[eWindow].u16PreHCusScalingSrc, g_DIPSrcInfo[eWindow].u16PreHCusScalingDst);
    FUNC_MSG("Pre  V cus scaling %u (%u -> %u)\n", g_DIPSrcInfo[eWindow].bPreVCusScaling, g_DIPSrcInfo[eWindow].u16PreVCusScalingSrc, g_DIPSrcInfo[eWindow].u16PreVCusScalingDst);

    pSrcInfo = &g_DIPSrcInfo[eWindow];

    stXC_SetWin_Info.u16PreHCusScalingSrc = pSrcInfo->u16PreHCusScalingSrc;
    stXC_SetWin_Info.u16PreHCusScalingDst = pSrcInfo->u16PreHCusScalingDst;
    HAL_XC_DIP_2P_Width_Check(pstCmdQInfo, &stXC_SetWin_Info, eWindow);
    pSrcInfo->u16PreHCusScalingDst = stXC_SetWin_Info.u16PreHCusScalingDst;

    // Capture width & height can not be 0 !!
    if ((0 == pSrcInfo->stCapWin.width) || (0 == pSrcInfo->stCapWin.height))
    {
        FUNC_ERR("Cap/Crop/Disp width or height can not be 0 \n");
        return;
    }

    //MDrv_XC_DWIN_SetMode(pstCmdQInfo,g_DIPSrcInfo[eWindow].enInputSourceType, &g_DIPSrcInfo[eWindow], eWindow);

    u16ScaleSrc = pSrcInfo->u16PreVCusScalingSrc;
    u16ScaleDst = pSrcInfo->u16PreVCusScalingDst;

    if( bInterlace )
    {
        if (u16ScaleDst % 2 )
        {
            u16ScaleDst = ( u16ScaleDst + 1) & ~0x1;
        }
    }

    //MDrv_XC_DWIN_set_prescaling_ratio(pstCmdQInfo, g_DIPSrcInfo[eWindow].enInputSourceType, &g_DIPSrcInfo[eWindow], eWindow );
    // factor = (output * 1048576 / input) +1
    //-----------------------------------------
    // Vertical
    //-----------------------------------------
    if(u16ScaleSrc > u16ScaleDst) //pre scaling only support scaling down
    {
        if (u16ScaleSrc == 0)
        {
            u32TmpPre = 0;
        }
        else
        {
            u32TmpPre = V_PreScalingDownRatioDIP(u16ScaleSrc, u16ScaleDst);
        }
        u32TmpPre &= 0xFFFFFFL;
        u32TmpPre |= 0x80000000L;
    }
    else
    {
        u32TmpPre = 0;
    }
    g_stDBreg.u32V_PreScalingRatio = u32TmpPre;
    pSrcInfo->u16PreVCusScalingSrc = u16ScaleSrc;
    pSrcInfo->u16PreVCusScalingDst = u16ScaleDst;
    pSrcInfo->u16V_SizeAfterPreScaling = u16ScaleDst;

    Hal_SC_DWIN_Set_vsd_output_line_count(pstCmdQInfo, DISABLE, u16ScaleDst, eWindow);
    Hal_SC_DWIN_Set_vsd_input_line_count(pstCmdQInfo, DISABLE, FALSE, 0, eWindow);

    //-----------------------------------------
    // Horizontal
    // HSD rule: 3D scaling > customer scaling > QMap HSD sampling > auto fit
    //-----------------------------------------
    // Note!! Formula:  FHD - If  (H Crop size /2 ) > ( H display size ), Must-do H pre-scaling down.

    {
        // Custimize
        if(pSrcInfo->bPreHCusScaling)
        {
            u16SrcHWidth = pSrcInfo->u16PreHCusScalingSrc;
            if (u16SrcHWidth > pSrcInfo->u16PreHCusScalingDst)
            {
                pSrcInfo->u16H_SizeAfterPreScaling = pSrcInfo->u16PreHCusScalingDst;
            }
            else
            {
                pSrcInfo->u16H_SizeAfterPreScaling = u16SrcHWidth;
            }
            //MDrv_SC_3D_Adjust_PreHorDstSize(enInputSourceType, pSrcInfo, eWindow);
            // 3d adjust may make the pre h dst bigger than cus_pre_H, we need adjust it again.
            if (pSrcInfo->u16H_SizeAfterPreScaling > pSrcInfo->u16PreHCusScalingDst)
            {
                pSrcInfo->u16H_SizeAfterPreScaling = pSrcInfo->u16PreHCusScalingDst;
            }
            FUNC_MSG("Customer H scaling: eWindow[%u] HSrc=%u, HDst=%u\n", eWindow, u16SrcHWidth, pSrcInfo->u16H_SizeAfterPreScaling);
        }
        else
        {
            u16SrcHWidth = pSrcInfo->stCapWin.width;
            if(u16SrcHWidth > 1920)
            {
                pSrcInfo->u16H_SizeAfterPreScaling = 1920;
                FUNC_MSG("DIPR Src width is larger than 1920, then do H scaling!!!\n");
            }
            else
            {
                pSrcInfo->u16H_SizeAfterPreScaling = u16SrcHWidth;
            }
        }
    }

    if(pSrcInfo->u16H_SizeAfterPreScaling & BIT(0))
    {
        //YUV422, h size must be even to prevent showing of garbage at the end of line
        if(pSrcInfo->u16H_SizeAfterPreScaling < pSrcInfo->stCapWin.width)
        {
            pSrcInfo->u16H_SizeAfterPreScaling = (pSrcInfo->u16H_SizeAfterPreScaling + 1) & ~0x1;
        }
        else
        {
            pSrcInfo->u16H_SizeAfterPreScaling = u16SrcHWidth;
        }
    }

    if (u16SrcHWidth > pSrcInfo->u16H_SizeAfterPreScaling)
    {
        MS_U16 u16div = 1;

        // To avoid overflow when do H_PreScalingDownRatioAdv/H_PreScalingDownRatioAdv_No_Minus1
        if(u16SrcHWidth > 4096)
        {
            while(1)
            {
                MS_U16 u16ret;

                u16div <<= 1;

                if (u16div == 0)
                {
                    break;
                }

                u16ret = u16SrcHWidth / u16div;

                if(u16ret == 0)
                {
                    u16div = 1;
                    break;
                }
                else if(u16ret < 4096 && u16ret > pSrcInfo->u16H_SizeAfterPreScaling)
                {
                    break;
                }
            }
        }

        // disable prescaling filter, use cb mode scaling ratio
        if (u16SrcHWidth == 0 )
            u32TmpPre = 0;
        else
            u32TmpPre = (H_PreScalingDownRatioDIP(u16SrcHWidth, pSrcInfo->u16H_SizeAfterPreScaling));

        u32TmpPre &= 0x7FFFFFL;
        u32TmpPre |= 0x80000000L;
    }
    else
    {
        pSrcInfo->u16H_SizeAfterPreScaling = pSrcInfo->u16PreHCusScalingDst;
        {
            u32TmpPre = 0x100000L;
        }
    }
    //store prescaling info here
    pSrcInfo->u16PreHCusScalingSrc = u16SrcHWidth;
    pSrcInfo->u16PreHCusScalingDst = pSrcInfo->u16H_SizeAfterPreScaling;
    g_stDBreg.u32H_PreScalingRatio = u32TmpPre;

    //g_stDBreg.u16DNROffset = pInstance->stAttr.stCropWin.u16Width;
    //g_stDBreg.u16DNRFetch = pInstance->stAttr.stCropWin.u16Width;
    MDrv_XC_DIVP_set_fetch_number_limit(pstCmdQInfo, pSrcInfo, eWindow );

    MDrv_XC_DIVP_set_capture_window(pstCmdQInfo, eWindow );

    Hal_SC_DWIN_sw_db(pstCmdQInfo, &g_stDBreg, eWindow);


}

void MDrv_XC_DIVP_SetY2R(void* pstCmdQInfo, MS_BOOL bEnable, SCALER_DIP_WIN eWindow)
{
    HAL_XC_DIP_SetY2R(pstCmdQInfo, bEnable, eWindow);
}

void MDrv_XC_DIVP_SetAlphaValue(void* pstCmdQInfo, MS_U8 u8AlphaVal, SCALER_DIP_WIN eWindow)
{
    HAL_XC_DIP_SetAlphaValue(pstCmdQInfo, u8AlphaVal, eWindow);
}

void MDrv_XC_DIVP_SetWinProperty(void* pstCmdQInfo, MHAL_DIVP_InstPrivate_t* pInstance, MHAL_DIVP_OutPutInfo_t* pstDivpOutputInfo, SCALER_DIP_WIN eWindow)
{
    EN_DRV_XC_DWIN_DATA_FMT fmt = XC_DWIN_DATA_FMT_MAX;
    MS_U16 u16LineOft = 0, u16Bpp = 0;
    MS_U32 u64TotalLen = 0, u64YLen = 0, u64CLen = 0, u64Offset = 0;
    MS_U32 u64_result = 0;
    MS_BOOL bYUV420 = FALSE;

    //width alignment check
    fmt = HAL_XC_DIP_GetDataFmt(pstCmdQInfo, eWindow);
    if(fmt == XC_DWIN_DATA_FMT_MAX)
    {
        return;
    }

    HAL_XC_DIP_SetMiuSel(pstCmdQInfo, DIP_MIU, DIP_WINDOW);

    HAL_XC_DIP_SetBase0(pstCmdQInfo, (MS_U32)pstDivpOutputInfo->u64BufAddr[0], (MS_U32)(pstDivpOutputInfo->u64BufAddr[0] + pstDivpOutputInfo->u32BufSize), DIP_WINDOW);

    HAL_XC_DIP_MuxDispatch(pstCmdQInfo, g_DIPSrcInfo[eWindow].enDIPSourceType, DIP_WINDOW);

    u16Bpp = HAL_XC_DIP_GetBPP(pstCmdQInfo, fmt, DIP_WINDOW);
    if( pstDivpOutputInfo->u16Stride[0] == 0 )
    {
        u16LineOft = pstDivpOutputInfo->u16OutputWidth;
    }
    else
    {
        u16LineOft = (pstDivpOutputInfo->u16Stride[0] / u16Bpp);
    }

    //buffer size check
    // offset unit : pixel
    if( (fmt == XC_DWIN_DATA_FMT_YUV420) || (fmt == XC_DWIN_DATA_FMT_YUV420_H265) || (fmt == XC_DWIN_DATA_FMT_YUV420_H265_10BITS) || (fmt == XC_DWIN_DATA_FMT_YUV420_PLANER) || (fmt == XC_DWIN_DATA_FMT_YUV420_SEMI_PLANER))
    {
        bYUV420 = TRUE;
        if( (fmt == XC_DWIN_DATA_FMT_YUV420) || (fmt == XC_DWIN_DATA_FMT_YUV420_H265) || (fmt == XC_DWIN_DATA_FMT_YUV420_H265_10BITS))
        {
            //Tile Y block is 16X32
            u64YLen = u16LineOft * ALIGN_CHECK(pstDivpOutputInfo->u16OutputHeight, DIP_HVD_TILE_BLOCK_V);
            //Tile C block is 16X64
            u64_result = u16LineOft * ALIGN_CHECK(pstDivpOutputInfo->u16OutputHeight, DIP_HVD_TILE_BLOCK_V * 2);
            do_div(u64_result, 2);
            u64CLen = u64_result;
        }
        else
        {
            u64YLen = u16LineOft * pstDivpOutputInfo->u16OutputHeight;
            u64_result = u16LineOft * pstDivpOutputInfo->u16OutputHeight;
            do_div(u64_result, 2);
            u64CLen = u64_result;
        }
        u64Offset = u64YLen + u64CLen;
        u64TotalLen = 1 * u64Offset * u16Bpp;
    }
    else
    {
        bYUV420 = FALSE;
        u64Offset = u16LineOft * pstDivpOutputInfo->u16OutputHeight;
        u64TotalLen = 1 * u64Offset * u16Bpp;
    }
    //u64Offset = 0;

    FUNC_MSG("u16Bpp=%d, u16LineOft=%d, u64Offset=0x%x, u64TotalLen=0x%x\n", u16Bpp, u16LineOft, (MS_U32)u64Offset, (MS_U32)u64TotalLen);

#if (DIP_PLATFORM == DIP_I2)  // do scaling up/down in HVSP

    if((pInstance->stAttr.eRotateType == E_MHAL_DIVP_ROTATE_90) || (pInstance->stAttr.eRotateType == E_MHAL_DIVP_ROTATE_270))
    {
        g_DIPSrcInfo[eWindow].u16HCusScalingSrc = g_DIPSrcInfo[eWindow].u16PreVCusScalingDst;
        g_DIPSrcInfo[eWindow].u16VCusScalingSrc = g_DIPSrcInfo[eWindow].u16PreHCusScalingDst;
    }
    else
    {
        g_DIPSrcInfo[eWindow].u16HCusScalingSrc = g_DIPSrcInfo[eWindow].u16PreHCusScalingDst;
        g_DIPSrcInfo[eWindow].u16VCusScalingSrc = g_DIPSrcInfo[eWindow].u16PreVCusScalingDst;
    }
    g_DIPSrcInfo[eWindow].u16HCusScalingDst = pstDivpOutputInfo->u16OutputWidth;
    g_DIPSrcInfo[eWindow].u16VCusScalingDst = pstDivpOutputInfo->u16OutputHeight;
    FUNC_MSG("Post H cus scaling (%u -> %u)\n", g_DIPSrcInfo[eWindow].u16HCusScalingSrc, g_DIPSrcInfo[eWindow].u16HCusScalingDst);
    FUNC_MSG("Post V cus scaling (%u -> %u)\n", g_DIPSrcInfo[eWindow].u16VCusScalingSrc, g_DIPSrcInfo[eWindow].u16VCusScalingDst);

    if(pInstance ->bIsDiEnable == TRUE)
    {
        if (g_DIPSrcInfo[eWindow].u16HCusScalingDst != g_DIPSrcInfo[eWindow].u16HCusScalingSrc ||
                g_DIPSrcInfo[eWindow].u16VCusScalingDst != (g_DIPSrcInfo[eWindow].u16VCusScalingSrc << 1))
            HAL_XC_DIP_SetHVSP(pstCmdQInfo, TRUE, DIP_WINDOW);  // support scaling up only
        else
            HAL_XC_DIP_SetHVSP(pstCmdQInfo, FALSE, DIP_WINDOW);  // disable without scaling up
    }
    else
    {
        if (g_DIPSrcInfo[eWindow].u16HCusScalingDst != g_DIPSrcInfo[eWindow].u16HCusScalingSrc ||
                g_DIPSrcInfo[eWindow].u16VCusScalingDst != g_DIPSrcInfo[eWindow].u16VCusScalingSrc)
            HAL_XC_DIP_SetHVSP(pstCmdQInfo, TRUE, DIP_WINDOW);  // support scaling up only
        else
            HAL_XC_DIP_SetHVSP(pstCmdQInfo, FALSE, DIP_WINDOW);  // disable without scaling up
    }

#else

    if(pInstance ->bIsDiEnable == TRUE)
    {
        if (pstDivpOutputInfo->u16OutputWidth > g_DIPSrcInfo[eWindow].stCapWin.width ||
                pstDivpOutputInfo->u16OutputHeight > (g_DIPSrcInfo[eWindow].stCapWin.height << 1))
            HAL_XC_DIP_SetHVSP(pstCmdQInfo, TRUE, DIP_WINDOW);  // support scaling up only
        else
            HAL_XC_DIP_SetHVSP(pstCmdQInfo, FALSE, DIP_WINDOW);  // disable without scaling up
    }
    else
    {
        if (pstDivpOutputInfo->u16OutputWidth > g_DIPSrcInfo[eWindow].stCapWin.width ||
                pstDivpOutputInfo->u16OutputHeight > g_DIPSrcInfo[eWindow].stCapWin.height)
            HAL_XC_DIP_SetHVSP(pstCmdQInfo, TRUE, DIP_WINDOW);  // support scaling up only
        else
            HAL_XC_DIP_SetHVSP(pstCmdQInfo, FALSE, DIP_WINDOW);  // disable without scaling up
    }

#endif

    HAL_XC_DIP_SetWinProperty(pstCmdQInfo,
                              1,  // Buffer count
                              pstDivpOutputInfo->u16OutputWidth,
                              u16LineOft,  // pitch
                              pstDivpOutputInfo->u16OutputHeight,
                              u64Offset,  // buffer offset
                              g_DIPSrcInfo[eWindow].enDIPSourceType,  //SCALER_DIP_SOURCE_TYPE_DRAM,
                              FALSE,  // bPIP
                              FALSE,  // b2P_Enable
                              DIP_WINDOW);

    if( (fmt == XC_DWIN_DATA_FMT_YUV420) || (fmt == XC_DWIN_DATA_FMT_YUV420_H265) || (fmt == XC_DWIN_DATA_FMT_YUV420_H265_10BITS) || (fmt == XC_DWIN_DATA_FMT_YUV420_PLANER) || (fmt == XC_DWIN_DATA_FMT_YUV420_SEMI_PLANER) )
    {
        HAL_XC_DIP_SetBase1(pstCmdQInfo, (MS_U32)pstDivpOutputInfo->u64BufAddr[1], (MS_U32)(pstDivpOutputInfo->u64BufAddr[1] + pstDivpOutputInfo->u32BufSize), DIP_WINDOW);
        HAL_XC_DIP_SetWinProperty1(pstCmdQInfo, u64Offset, DIP_WINDOW);
    }
    else if( (fmt == XC_DWIN_DATA_FMT_YC422) )
    {
        HAL_XC_DIP_SetBase1(pstCmdQInfo, (MS_U32)pstDivpOutputInfo->u64BufAddr[1], (MS_U32)(pstDivpOutputInfo->u64BufAddr[1] + pstDivpOutputInfo->u32BufSize), DIP_WINDOW);
        HAL_XC_DIP_SetWinProperty1(pstCmdQInfo, u64Offset, DIP_WINDOW);
    }

}

void MDrv_XC_DIVP_EnableCaptureStream(void* pstCmdQInfo, MS_BOOL bEnable, SCALER_DIP_WIN eWindow)
{
    if(bEnable)
    {
        HAL_XC_DIP_SWReset(pstCmdQInfo, eWindow);//Reset HW before DIP enable to avoid garbage
        HAL_XC_DIP_EnableCaptureStream(pstCmdQInfo, bEnable, eWindow);
    }
    else
    {
        MS_U16 u16IntrStus = 0;
        HAL_XC_DIP_EnableCaptureStream(pstCmdQInfo, bEnable, eWindow);
        //Auto clear status to zero
        u16IntrStus = HAL_XC_DIP_GetIntrStatus(pstCmdQInfo, eWindow);
        HAL_XC_DIP_ClearIntr(pstCmdQInfo, u16IntrStus, eWindow);
        HAL_XC_DIP_SWReset(pstCmdQInfo, eWindow);//Reset after disable dip to avoid get old temproary info
    }
}

#define XC_DIP_CAP_TIMEOUT_CNT 100
void MDrv_XC_DIVP_CaptureOneFrame(void* pstCmdQInfo, SCALER_DIP_WIN eWindow)
{
    MS_U16 regval = 0;
    MS_U16 u16Count = 0;

    //HAL_XC_DIP_SWReset(pstCmdQInfo, eWindow);//Reset HW before DIP enable to avoid garbage
    HAL_XC_DIP_CpatureOneFrame2(pstCmdQInfo, eWindow);
    do
    {
        if(g_DIVPOutPath == DIP_OUTPUT_PATH_MDWIN)
            regval = HAL_XC_DIP_MDWIN_GetIntrStatus(pstCmdQInfo, eWindow);
        else
            regval = HAL_XC_DIP_GetIntrStatus(pstCmdQInfo, eWindow);

        if(regval > 0x0)
            break;
        u16Count++;
        DIP_DelayTask(1);
    }
    while(u16Count < XC_DIP_CAP_TIMEOUT_CNT);

    FUNC_MSG("[Cnt = %d]\n", u16Count);
    if(u16Count >= XC_DIP_CAP_TIMEOUT_CNT)
    {
        FUNC_ERR("[ERROR] Capture One Frame time out....\n");
    }
}
void MDrv_XC_DIVP_CaptureOneFrame2(void* pstCmdQInfo, SCALER_DIP_WIN eWindow)
{
    //_XC_ENTRY(pstCmdQInfo);
    //HAL_XC_DIP_SWReset(pstCmdQInfo, eWindow);//Reset HW before DIP enable to avoid garbage
    HAL_XC_DIP_CpatureOneFrame2(pstCmdQInfo, eWindow);
    //_XC_RETURN(pstCmdQInfo);
}

#ifdef DIP_UT_ALLOC_MEM_FOR_3DDI
    static MS_U32 p_u32DiBufAddr;
#endif

MS_BOOL _DipAllocateMem(MHAL_DIVP_InstPrivate_t* pDivpInst, MS_U64 *pAddr, MS_U32 AddrSize)
{
#if (DIP_PLATFORM == DIP_I2)

#ifdef DIP_UT_ALLOC_MEM_FOR_3DDI

    if(p_u32DiBufAddr != 0)
        *pAddr = p_u32DiBufAddr;

#else
    SCALER_DIP_WIN enDipWin = (SCALER_DIP_WIN)pDivpInst->eId;
    PfnAlloc pfMemAlloc = g_stDipRes[enDipWin].pfAlloc;

    //allocate memory
    if (!pfMemAlloc)
    {
        FUNC_ERR("pfMemAllocis null(%p).\n", pfMemAlloc);
        return FALSE;
    }

    pfMemAlloc(NULL, AddrSize, pAddr);

#endif

#else

    *pAddr = _3DDI_BUFFER_OFFSET; //0xD800000;

#endif

    return TRUE;
}

MS_BOOL _DipFreeMem(MHAL_DIVP_InstPrivate_t* pDivpInst, MS_U64 *pAddr)
{
#if (DIP_PLATFORM == DIP_I2)

#ifdef DIP_UT_ALLOC_MEM_FOR_3DDI

    if(p_u32DiBufAddr != 0)
        p_u32DiBufAddr = 0;

#else

    SCALER_DIP_WIN enDipWin = (SCALER_DIP_WIN)pDivpInst->eId;
    PfnFree pfMemFree = g_stDipRes[enDipWin].pfFree;

    if (!pfMemFree)
    {
        FUNC_ERR("pfMemFree is null(%p).\n", pfMemFree);
        return FALSE;
    }
    pfMemFree((MS_U64)*pAddr);

#endif

#else

    *pAddr = 0;

#endif
    return TRUE;
}

MS_S32 MDrv_XC_DIVP_SetDIPRConfig_MFDEC(void * pstCmdQInfo, MHAL_DIVP_InstPrivate_t * pDivpInst, MHAL_DIVP_InputInfo_t* pstDivpInputInfo, SCALER_DIP_WIN eWindow)
{
    SCALER_DIP_WIN enDipWin = (SCALER_DIP_WIN)pDivpInst->eId;
    ST_XC_DIPR_PROPERTY *pstDiprInfo = &g_stDipRes[enDipWin].stDiprInfo.stDIPRProperty;
    ST_DIP_MFDEC_INFO *pstDiprDecInfo = &g_stDipRes[enDipWin].stDiprInfo.stDIPR_MFDecInfo;
    MHAL_DIVP_MFdecInfo_t *pstDecBufInfo = &pstDivpInputInfo->stMfdecInfo;
    EN_MFDEC_TILE_MODE enMFDecTileMode = E_DIP_MFEDC_TILE_16_32;
    EN_DRV_XC_DWIN_DATA_FMT efmt = XC_DWIN_DATA_FMT_MAX;

    if(pstDivpInputInfo->eScanMode != E_MHAL_DIVP_SCAN_MODE_PROGRESSIVE)
    {
        FUNC_ERR("Can't support MFDEC path and 3DDI at same time!!!!\n");
        return -1;
    }

    enMFDecTileMode = _DipMFDecTileModeTransform(pstDivpInputInfo->eTileMode);
    efmt = (EN_DRV_XC_DWIN_DATA_FMT)(pstDiprInfo->enDataFmt);

    pstDiprDecInfo->stMFDec_VerCtl.u32version = 0;
    pstDiprDecInfo->stMFDec_VerCtl.u32size = sizeof(ST_DIP_MFDEC_INFO);
    pstDiprDecInfo->bMFDec_Enable = pstDecBufInfo->bDbEnable;
    pstDiprDecInfo->u8MFDec_Select = pstDecBufInfo->u8DbSelect;
    pstDiprDecInfo->bHMirror = pstDecBufInfo->stMirror.bHMirror;
    pstDiprDecInfo->bVMirror = pstDecBufInfo->stMirror.bVMirror;
    pstDiprDecInfo->enMFDec_tile_mode = enMFDecTileMode;  // E_DIP_MFEDC_TILE_32_16
    pstDiprDecInfo->bUncompress_mode = pstDecBufInfo->bUncompressMode;
    pstDiprDecInfo->bBypass_codec_mode = pstDecBufInfo->bBypassCodecMode;
    switch (pstDecBufInfo->eDbMode)
    {
        case E_MHAL_DIVP_DB_MODE_H264_H265:
            pstDiprDecInfo->en_MFDecVP9_mode = E_XC_DIP_H26X_MODE;
            break;
        case E_MHAL_DIVP_DB_MODE_VP9:
            pstDiprDecInfo->en_MFDecVP9_mode = E_XC_DIP_VP9_MODE;
            break;
        default :
            FUNC_ERR("DIPC Error en_MFDecVP9_mode[%d] not support.\n", pstDecBufInfo->bDbEnable);
            return -1;
    }

    pstDiprDecInfo->phyLuma_FB_Base = pstDiprInfo->u32YBufAddr;
    pstDiprDecInfo->phyChroma_FB_Base = pstDiprInfo->u32CBufAddr;
    //if((efmt == XC_DWIN_DATA_FMT_YUV420) || (efmt == XC_DWIN_DATA_FMT_YUV420_H265) || (efmt == XC_DWIN_DATA_FMT_YUV420_H265_10BITS))
    //{
    //    pstDiprDecInfo->u16FB_Pitch = (pstDiprInfo->u16Pitch * 2) / 3;
    //}
    //else
    {
        pstDiprDecInfo->u16FB_Pitch = pstDiprInfo->u16Pitch;
    }
    pstDiprDecInfo->u16StartX = pstDecBufInfo->u16StartX;
    pstDiprDecInfo->u16StartY = pstDecBufInfo->u16StartY;
    pstDiprDecInfo->u16HSize = pstDecBufInfo->u16Width;
    pstDiprDecInfo->u16VSize = pstDecBufInfo->u16Height;
    pstDiprDecInfo->phyBitlen_Base = (MS_U32)pstDecBufInfo->u64DbBaseAddr;
    //pstDiprDecInfo->u16Bitlen_Pitch = pstDecBufInfo->u16DbPitch;
    pstDiprDecInfo->u16Bitlen_Pitch = (pstDiprDecInfo->u16FB_Pitch + 63) >> 6;

    FUNC_MSG("[%d]FB_Pitch=%d, Bitlen_Pitch=%d\n", efmt, pstDiprDecInfo->u16FB_Pitch, pstDiprDecInfo->u16Bitlen_Pitch);

    pstDiprDecInfo->stMFDec_HTLB_Info.stMFDec_HTLB_VerCtl.u32version = 0;
    pstDiprDecInfo->stMFDec_HTLB_Info.stMFDec_HTLB_VerCtl.u32size = sizeof(ST_DIP_MFDEC_HTLB_INFO);
    pstDiprDecInfo->stMFDec_HTLB_Info.u32HTLBEntriesAddr = pstDecBufInfo->u64LbAddr;
    pstDiprDecInfo->stMFDec_HTLB_Info.u8HTLBEntriesSize = pstDecBufInfo->u8LbSize;
    pstDiprDecInfo->stMFDec_HTLB_Info.u8HTLBTableId = pstDecBufInfo->u8LbTableId;
    pstDiprDecInfo->stMFDec_HTLB_Info.pHTLBInfo = NULL;  // reserved
    pstDiprDecInfo->pMFDecInfo = NULL;  // reserved

    MDrv_XC_DIVP_SetDIPRProperty_EX(pstCmdQInfo, pDivpInst, &g_stDipRes[enDipWin].stDiprInfo, DIP_WINDOW);

    return 0;
}

MS_BOOL DipDiInit(void* pstCmdQInfo, MHAL_DIVP_InstPrivate_t* pDivpInst, MS_BOOL bEnableDi)
{
    MS_U32 u32DipDiSize =  0;
    ST_XC_DIPR_PROPERTY_EX stDIPRProperty_EX;
    ST_DIP_DI_SETTING DIP3DDISetting;
    MS_PHY pDipDiAddr;
    SCALER_DIP_WIN enDipWin = (SCALER_DIP_WIN)pDivpInst->eId;
    DivpPrivateDevData *dev = &g_stDipRes[enDipWin];
    MS_U8 u8ChnID = pDivpInst->u8ChannelId;
    MS_U8 u8Chn;

    FUNC_MSG("DipDiInit, u8ChannelId = %d, bEnableDi=%d.\n", u8ChnID, bEnableDi);

    memset(&stDIPRProperty_EX, 0, sizeof(ST_XC_DIPR_PROPERTY_EX));
    memset(&DIP3DDISetting, 0, sizeof(ST_DIP_DI_SETTING));

    if(bEnableDi == FALSE)
    {
        DIP3DDISetting.bEnableDI = FALSE;
    }
    else
    {
#ifdef DIP_UT_ALLOC_MEM_FOR_3DDI
        u32DipDiSize = 736 * (480 / 2) * 4 * DI_BUFFER_CNT;
#else
        u32DipDiSize = pDivpInst->nMaxWidth * (pDivpInst->nMaxHeight / 2) * 4 * DI_BUFFER_CNT;
#endif
        if(pDivpInst->u64AllocAddr == 0)
        {
            if(_DipAllocateMem(pDivpInst, &pDipDiAddr, u32DipDiSize) == FALSE)
                return FALSE;
            //pDipDiAddr = 0x000FC00000; //0xD800000;

            pDivpInst->u64AllocAddr = pDipDiAddr;
            pDivpInst->u32AllocSize = u32DipDiSize;
        }
        FUNC_MSG("pDipDiAddr = 0x%x, u32DipDiSize = 0x%x\n", (MS_U32)pDipDiAddr, (MS_U32)u32DipDiSize);

        pDivpInst->st3dDiInfo.u32BaseAddr = (MS_U32)pDipDiAddr;
        pDivpInst->st3dDiInfo.u32DISize = u32DipDiSize;
        pDivpInst->st3dDiInfo.u8BufNum = DI_BUFFER_CNT; // 4 DI temp buffer

        // Get maximum channel number for 3DDI process
        dev->u8NwayMaxBufCnt = 0;
        for(u8Chn = 0 ; u8Chn < MAX_CHANNEL_NUM ; u8Chn++)
        {
            if(dev->ctx[u8Chn] != NULL)
            {
                if((u8Chn + 1) >= dev->u8NwayMaxBufCnt)
                    dev->u8NwayMaxBufCnt = (u8Chn + 1);
            }
        }
        FUNC_MSG("NwayMaxBufCnt = %d\n", dev->u8NwayMaxBufCnt);

        //dip di init
        DIP3DDISetting.bEnableDI = bEnableDi;
        DIP3DDISetting.bInitDI = TRUE;
        DIP3DDISetting.stInitInfo.phyDIInitAddr = (MS_U32)pDipDiAddr; //only use one times, so use the same buferr with di temp buffer(stDINwayBuf)
        DIP3DDISetting.stInitInfo.u32DIInitSize = u32DipDiSize;
        DIP3DDISetting.u8NwayIndex = u8ChnID;
        DIP3DDISetting.enDI_Field = E_DIP_3DDI_TOP_BOTH_FIELD_STAGGER;
        DIP3DDISetting.bEnaNwayInfoRefresh = TRUE;
        DIP3DDISetting.u8NwayBufCnt = dev->u8NwayMaxBufCnt;
        DIP3DDISetting.bEnableBob = TRUE;
        stDINwayBuf[u8ChnID].phyDI_BufAddr = (MS_U32)pDipDiAddr;
        stDINwayBuf[u8ChnID].u32DI_BufSize = u32DipDiSize;
        stDINwayBuf[u8ChnID].u8DIBufCnt = DI_BUFFER_CNT;
        DIP3DDISetting.stDINwayBufInfo = (void*)stDINwayBuf;

        FUNC_MSG("phyDI_BufAddr = 0x%x, DISize = 0x%x\n", (MS_U32)stDINwayBuf[u8ChnID].phyDI_BufAddr, (MS_U32)stDINwayBuf[u8ChnID].u32DI_BufSize);

    }
    stDIPRProperty_EX.stDIPR3DDISetting.stDipr3DDI_VerCtl.u32version = 1;
    stDIPRProperty_EX.stDIPR3DDISetting.pDIPR3DDISetting = (void*)&DIP3DDISetting;
    MDrv_XC_DIVP_SetDIPRProperty_EX(pstCmdQInfo, pDivpInst, &stDIPRProperty_EX, DIP_WINDOW);

    return TRUE;
}

MS_S32 MDrv_XC_DIVP_SetDIPRConfig_3DDI(void * pstCmdQInfo, MHAL_DIVP_InstPrivate_t * pDivpInst, MHAL_DIVP_InputInfo_t* pstDivpInputInfo, SCALER_DIP_WIN eWindow)
{
    SCALER_DIP_WIN enDipWin = (SCALER_DIP_WIN)pDivpInst->eId;
    DivpPrivateDevData *dev = &g_stDipRes[enDipWin];
    ST_XC_DIPR_PROPERTY *pstDiprInfo = &g_stDipRes[enDipWin].stDiprInfo.stDIPRProperty;
    ST_DIP_DI_SETTING *pstDipr3DDiSetting = &g_stDipRes[enDipWin].st3DDISetting;
    ST_DIP_DIPR_3DDI_SETTING *pstDipr3DDiInfo = &g_stDipRes[enDipWin].stDiprInfo.stDIPR3DDISetting;
    MHAL_DIVP_DiType_e eDiType = pDivpInst->stAttr.eDiType;  //pstDivpInputInfo->stDiSettings.eDiType;
    MHAL_DIVP_ScanMode_e eScanType = pstDivpInputInfo->eScanMode;
    MHAL_DIVP_FieldType_e eFieldType = pstDivpInputInfo->stDiSettings.eFieldType;
    MHAL_DIVP_DiMode_e eDiMode = pstDivpInputInfo->stDiSettings.eDiMode;
    MS_U8 u8ChnID = pDivpInst->u8ChannelId;

    FUNC_MSG("[%d]eDiType=%d, eScanType=%d, eFieldType=%d, eDiMode = %d\n", u8ChnID, eDiType, eScanType, eFieldType, eDiMode);

    if (eScanType != E_MHAL_DIVP_SCAN_MODE_PROGRESSIVE )
    {
        if(pDivpInst->bIsDiEnable == FALSE)
        {
#ifdef DIP_UT_ALLOC_MEM_FOR_3DDI
            p_u32DiBufAddr = (MS_U32)pstDivpInputInfo->u64BufAddr[2];
            FUNC_MSG("p_u32DiBufAddr=0x%x\n", (MS_U32)p_u32DiBufAddr);
#endif

            if(DipDiInit(pstCmdQInfo, pDivpInst, TRUE) == FALSE)
                return -1;
            pDivpInst->bIsDiEnable = TRUE;
        }
    }

    if (eDiType == E_MHAL_DIVP_DI_TYPE_3D)
    {
        FUNC_MSG("E_MHAL_DIVP_DI_TYPE_3DDI >>>\n");

        pstDipr3DDiSetting->bEnableDI = TRUE;
        pstDipr3DDiSetting->bInitDI = FALSE;
        pstDipr3DDiSetting->u8NwayIndex = u8ChnID;
        pstDipr3DDiSetting->enDI_Field = _DipGetDiFieldType(eDiType, eScanType, eFieldType);
        FUNC_MSG("E_MHAL_DIVP_DI_TYPE_3DDI >>> enDI_Field = %d\n", pstDipr3DDiSetting->enDI_Field);

        pstDipr3DDiSetting->bEnaNwayInfoRefresh = FALSE;
        pstDipr3DDiSetting->u8NwayBufCnt = dev->u8NwayMaxBufCnt;
        //pstDipr3DDiSetting->bEnableBob = TRUE;
        if(eDiMode == E_MHAL_DIVP_DI_MODE_BOB)
        {
            if(pDivpInst->u32CurFrameCnt >= 2)
                pDivpInst->u32CurFrameCnt = 0;  // reset frame count to 0
        }
        else
        {

        }
        if(pDivpInst->u32CurFrameCnt < 2) //the first && the second field need run bob mode
        {
            FUNC_MSG("[Enable Bob mode]\n");
            pstDipr3DDiSetting->bEnableBob = TRUE;// Fix me
            pDivpInst->u32CurFrameCnt ++;
        }
        else //run nonbob mode
        {
            FUNC_MSG("[Disable Bob mode]\n");
            pstDipr3DDiSetting->bEnableBob = FALSE;// Fix me
        }

        stDINwayBuf[u8ChnID].phyDI_BufAddr = pDivpInst->st3dDiInfo.u32BaseAddr;
        stDINwayBuf[u8ChnID].u32DI_BufSize = pDivpInst->st3dDiInfo.u32DISize;
        stDINwayBuf[u8ChnID].u8DIBufCnt = pDivpInst->st3dDiInfo.u8BufNum;
        pstDipr3DDiSetting->stDINwayBufInfo = (void*)stDINwayBuf;

        // interlace mode, height *1/2
        //pstDiprInfo->u16Height = pstDiprInfo->u16Height>>1;

        pstDipr3DDiInfo->stDipr3DDI_VerCtl.u32version = 1;
        pstDipr3DDiInfo->stDipr3DDI_VerCtl.u32size = sizeof(ST_DIP_DI_SETTING);
        pstDipr3DDiInfo->pDIPR3DDISetting = pstDipr3DDiSetting;

    }
    else if (eDiType == E_MHAL_DIVP_DI_TYPE_MED) // 2.5D di
    {
        FUNC_MSG("E_MHAL_DIVP_DI_TYPE_MED>>>");

        pstDipr3DDiSetting->bEnableDI = TRUE;
        pstDipr3DDiSetting->enDI_Field = _DipGetDiFieldType(eDiType, eScanType, eFieldType);//_DMS_DIP_GetDIPDiDataFMT(u32Window, RES_DMS._stXC_DIPPushInfo[u32Window][u16BufferID].eScanType, RES_DMS._stXC_DIPPushInfo[u32Window][u16BufferID].eFieldType);
        if(eScanType == E_MHAL_DIVP_SCAN_MODE_INTERLACE_FIELD)  //(_DMS_Is_SourceFieldBaseInterlace(RES_DMS._stXC_DIPPushInfo[u32Window][u16BufferID].eScanType))
        {
            //field base di
            pstDipr3DDiSetting->phyBotYBufAddr = 0;//RES_DMS._stXC_DIPPushInfo[u32Window][u16BufferID].u32SrcLumaAddrI;
            pstDipr3DDiSetting->phyBotCBufAddr = 0;//RES_DMS._stXC_DIPPushInfo[u32Window][u16BufferID].u32SrcChromaAddrI;
            pstDipr3DDiSetting->phyBotYBufAddr10Bits = 0;//RES_DMS._stXC_DIPPushInfo[u32Window][u16BufferID].u32SrcLumaAddrI;
            pstDipr3DDiSetting->phyBotCBufAddr10Bits = 0;//RES_DMS._stXC_DIPPushInfo[u32Window][u16BufferID].u32SrcChromaAddrI;
        }
        else
        {
            //frame base di
            pstDipr3DDiSetting->phyBotYBufAddr = pstDiprInfo->u32YBufAddr;//DIPRWinPropertytoShow_EX.stDIPRProperty.u32YBufAddr;
            pstDipr3DDiSetting->phyBotCBufAddr = pstDiprInfo->u32CBufAddr;//DIPRWinPropertytoShow_EX.stDIPRProperty.u32CBufAddr;
            pstDipr3DDiSetting->phyBotYBufAddr10Bits = pstDiprInfo->u32YBufAddr10Bits;//DIPRWinPropertytoShow_EX.stDIPRProperty.u32YBufAddr;
            pstDipr3DDiSetting->phyBotCBufAddr10Bits = pstDiprInfo->u32CBufAddr10Bits;//DIPRWinPropertytoShow_EX.stDIPRProperty.u32CBufAddr;
        }

        pstDipr3DDiInfo->stDipr3DDI_VerCtl.u32version = 1;
        pstDipr3DDiInfo->stDipr3DDI_VerCtl.u32size = sizeof(ST_DIP_DI_SETTING);
        pstDipr3DDiInfo->pDIPR3DDISetting = pstDipr3DDiSetting;
    }
    else
    {
        pstDipr3DDiInfo->stDipr3DDI_VerCtl.u32version = 0;
        pstDipr3DDiInfo->stDipr3DDI_VerCtl.u32size = 0;//sizeof(ST_DIP_DI_SETTING);
        pstDipr3DDiInfo->pDIPR3DDISetting = 0;//pstDipr3DDiSetting;
    }

    MDrv_XC_DIVP_SetDIPRProperty_EX(pstCmdQInfo, pDivpInst, &g_stDipRes[enDipWin].stDiprInfo, DIP_WINDOW);

    return 0;
}

MS_S32 mDrv_XC_DIVP_CaptureConfigCheck(MHAL_DIVP_InstPrivate_t *pDivpInst, MHAL_DIVP_CaptureInfo_t* pstCaptureInfo)
{
    FUNC_MSG("TNR=%d, ROT=%d\n", pDivpInst->stAttr.eTnrLevel, pDivpInst->stAttr.eRotateType);
    //FUNC_MSG("HMirror=%d, VMirror=%d\n", pDivpInst->stAttr.stMirror.bHMirror, pDivpInst->stAttr.stMirror.bVMirror);
    FUNC_MSG("CropX=%d, CropY=%d\n", pstCaptureInfo->stCropWin.u16X, pstCaptureInfo->stCropWin.u16Y);
    FUNC_MSG("CropW=%d, CropH=%d\n", pstCaptureInfo->stCropWin.u16Width, pstCaptureInfo->stCropWin.u16Height);
    FUNC_MSG("pstCaptureInfo: Addr=0x%x\n", (MS_U32)(pstCaptureInfo->u64BufAddr[0]));
    FUNC_MSG("    OutW=%d, OutH= %d\n", pstCaptureInfo->u16Width, pstCaptureInfo->u16Height);
    FUNC_MSG("    u16Stride=%d, ePxlFmt=%d\n", pstCaptureInfo->u16Stride[0], pstCaptureInfo->eOutputPxlFmt);
    FUNC_MSG("Input ePxlFmt=%d\n", pstCaptureInfo->eInputPxlFmt);
    FUNC_MSG("    Display ID=%d, CapStage=%d\n", pstCaptureInfo->eDispId, pstCaptureInfo->eCapStage);
    FUNC_MSG("    ROT=%d, MIRROR=(%d,%d)\n", pstCaptureInfo->eRotate, pstCaptureInfo->stMirror.bHMirror, pstCaptureInfo->stMirror.bVMirror);

    if((pstCaptureInfo->stCropWin.u16Width > MAX_WIDTH) || (pstCaptureInfo->stCropWin.u16Height > MAX_WIDTH) || \
            (pstCaptureInfo->u16Width > MAX_WIDTH) || (pstCaptureInfo->u16Height > MAX_WIDTH) || \
            (pstCaptureInfo->eOutputPxlFmt >= E_MHAL_DIVP_PIXEL_FORMAT_MAX))
    {
        FUNC_ERR("Input parameter error!!!!\n");
        return -1;
    }

    if(pstCaptureInfo->u16Width % 2)
    {
        FUNC_ERR("Output width need 2 alignment!!!!\n");
        return -1;
    }

    if((pDivpInst->stAttr.eRotateType !=  E_MHAL_DIVP_ROTATE_NONE) || (pstCaptureInfo->eRotate !=  E_MHAL_DIVP_ROTATE_NONE))
    {
        return -1;
    }

    if((pstCaptureInfo->stCropWin.u16X % 2) || (pstCaptureInfo->stCropWin.u16Width % 2) || \
            (pstCaptureInfo->stCropWin.u16Y % 2) || (pstCaptureInfo->stCropWin.u16Height % 2))
    {
        FUNC_ERR("Crop StartX/Y/width/height need 2 alignment!!!!\n");
        return -1;
    }

    if(pstCaptureInfo->eOutputPxlFmt == E_MHAL_DIVP_PIXEL_FORMAT_YUV422_YUYV)
    {
        if(pstCaptureInfo->u16Width % 16)
        {
            FUNC_ERR("Output width need 16 alignment!!!!\n");
            return -1;
        }
        if((pstCaptureInfo->u16Stride[0] % 16) || (pstCaptureInfo->u16Stride[0] % 16))
        {
            FUNC_ERR("Input/Output line offset need 16 alignment!!!!\n");
            return -1;
        }
    }
    else if(pstCaptureInfo->eOutputPxlFmt == E_MHAL_DIVP_PIXEL_FORMAT_YUV_SEMIPLANAR_420)
    {
        if(pstCaptureInfo->u16Width % 32)
        {
            FUNC_ERR("Output width need 32 alignment!!!!\n");
            return -1;
        }
        if((pstCaptureInfo->u16Stride[0] % 32) || (pstCaptureInfo->u16Stride[0] % 32))
        {
            FUNC_ERR("Input/Output line offset need 32 alignment!!!!\n");
            return -1;
        }
    }
    else if((pstCaptureInfo->eOutputPxlFmt == E_MHAL_DIVP_PIXEL_FORMAT_YUV_MST_420) || \
            (pstCaptureInfo->eOutputPxlFmt == E_MHAL_DIVP_PIXEL_FORMAT_YC420_MSTTILE1_H264) || \
            (pstCaptureInfo->eOutputPxlFmt == E_MHAL_DIVP_PIXEL_FORMAT_YC420_MSTTILE2_H265))  // only support tile 16x32
    {
        if(pstCaptureInfo->u16Width % 32)
        {
            FUNC_ERR("Output width need 32 alignment!!!!\n");
            return -1;
        }
        if((pstCaptureInfo->u16Stride[0] % 32) || (pstCaptureInfo->u16Stride[0] % 32))
        {
            FUNC_ERR("Input/Output line offset need 32 alignment!!!!\n");
            return -1;
        }
    }
    else
    {
        FUNC_ERR("Input format error!!!!\n");
        return -1;
    }

    return 0;
}

MS_S32 mDrv_XC_DIVP_InputConfigCheck(MHAL_DIVP_InstPrivate_t *pDivpInst, MHAL_DIVP_InputInfo_t* pstDivpInputInfo, MHAL_DIVP_OutPutInfo_t* pstDivpOutputInfo)
{
    MS_U16 u16LineOffset;

    FUNC_MSG("TNR=%d, 3DDI=%d, ROT=%d\n", pDivpInst->stAttr.eTnrLevel, pDivpInst->stAttr.eDiType, pDivpInst->stAttr.eRotateType);
    //FUNC_MSG("HMirror=%d, VMirror=%d\n", pDivpInst->stAttr.stMirror.bHMirror, pDivpInst->stAttr.stMirror.bVMirror);
    FUNC_MSG("CropX=%d, CropY=%d\n", pstDivpInputInfo->stCropWin.u16X, pstDivpInputInfo->stCropWin.u16Y);
    FUNC_MSG("CropW=%d, CropH=%d\n", pstDivpInputInfo->stCropWin.u16Width, pstDivpInputInfo->stCropWin.u16Height);
    FUNC_MSG("pstDivpInputInfo: Addr0=0x%x, Addr1=0x%x\n", (MS_U32)(pstDivpInputInfo->u64BufAddr[0]), (MS_U32)(pstDivpInputInfo->u64BufAddr[1]));
    FUNC_MSG("    InW=%d, InH= %d\n", pstDivpInputInfo->u16InputWidth, pstDivpInputInfo->u16InputHeight);
    FUNC_MSG("    u16Stride=%d, ePxlFmt=%d, eTileMode= %d\n", pstDivpInputInfo->u16Stride[0], pstDivpInputInfo->ePxlFmt, pstDivpInputInfo->eTileMode);
    FUNC_MSG("    3DDI eScanMode=%d, eDiMode=%d\n", pstDivpInputInfo->eScanMode, pstDivpInputInfo->stDiSettings.eDiMode);
    FUNC_MSG("    3DDI eFieldType=%d, eFieldOrderType=%d\n", pstDivpInputInfo->stDiSettings.eFieldType, pstDivpInputInfo->stDiSettings.eFieldOrderType);
    FUNC_MSG("    MFDEC enable=%d, bypass=%d\n", pstDivpInputInfo->stMfdecInfo.bDbEnable, pstDivpInputInfo->stMfdecInfo.bBypassCodecMode);
    FUNC_MSG("    eDbMode=%d, u64DbBaseAddr=0x%x\n", pstDivpInputInfo->stMfdecInfo.eDbMode, (MS_U32)pstDivpInputInfo->stMfdecInfo.u64DbBaseAddr);
    FUNC_MSG("pstDivpOutputInfo: Addr=0x%x\n", (MS_U32)(pstDivpOutputInfo->u64BufAddr[0]));
    FUNC_MSG("    OutW=%d, OutH= %d\n", pstDivpOutputInfo->u16OutputWidth, pstDivpOutputInfo->u16OutputHeight);
    FUNC_MSG("    u16Stride=%d, ePxlFmt=%d\n", pstDivpOutputInfo->u16Stride[0], pstDivpOutputInfo->ePxlFmt);

    if((pstDivpInputInfo->stCropWin.u16Width > MAX_WIDTH) || (pstDivpInputInfo->stCropWin.u16Height > MAX_WIDTH) || \
            (pstDivpInputInfo->u16InputWidth > MAX_WIDTH) || (pstDivpInputInfo->u16InputHeight > MAX_WIDTH) || \
            (pstDivpOutputInfo->u16OutputWidth > MAX_WIDTH) || (pstDivpOutputInfo->u16OutputHeight > MAX_WIDTH))
    {
        FUNC_ERR("Input/Output width/height is too large!!!!\n");
        return -1;
    }

    if((pstDivpInputInfo->ePxlFmt >= E_MHAL_DIVP_PIXEL_FORMAT_MAX) || (pstDivpOutputInfo->ePxlFmt >= E_MHAL_DIVP_PIXEL_FORMAT_MAX))
    {
        FUNC_ERR("Input/Output format don't support!!!!\n");
        return -1;
    }

    if((pstDivpOutputInfo->u16OutputWidth < 128) || (pstDivpOutputInfo->u16OutputHeight < 64))
    {
        FUNC_ERR("Output width/height is too small!!!!\n");
        return -1;
    }

    if((pstDivpInputInfo->u16InputWidth < 128) || (pstDivpInputInfo->u16InputHeight < 64))
    {
        FUNC_ERR("Input width/height is too small!!!!\n");
        return -1;
    }

    if(pstDivpOutputInfo->u16OutputWidth % 2)
    {
        FUNC_ERR("Output width need 2 alignment!!!!\n");
        return -1;
    }

    if((pstDivpInputInfo->stCropWin.u16X % 2) || (pstDivpInputInfo->stCropWin.u16Width % 2) || \
            (pstDivpInputInfo->stCropWin.u16Y % 2) || (pstDivpInputInfo->stCropWin.u16Height % 2))
    {
        FUNC_ERR("Crop StartX/Y/width/height need 2 alignment!!!!\n");
        return -1;
    }

    if(pstDivpInputInfo->ePxlFmt == E_MHAL_DIVP_PIXEL_FORMAT_YUV422_YUYV)
    {
        if(pstDivpInputInfo->u16InputWidth % 16)
        {
            FUNC_ERR("Input width need 16 alignment!!!!\n");
            return -1;
        }
        if(pstDivpInputInfo->u16Stride[0] % 32)
        {
            FUNC_ERR("Input line offset need 16 alignment!!!!\n");
            return -1;
        }
    }
    else if(pstDivpInputInfo->ePxlFmt == E_MHAL_DIVP_PIXEL_FORMAT_YUV_SEMIPLANAR_420)
    {
        if(pstDivpInputInfo->u16InputWidth % 32)
        {
            FUNC_ERR("Input width need 32 alignment!!!!\n");
            return -1;
        }
        if(pstDivpInputInfo->u16InputHeight % 2)
        {
            FUNC_ERR("Input height need 2 alignment!!!!\n");
            return -1;
        }
        if(pstDivpInputInfo->u16Stride[0] % 32)
        {
            FUNC_ERR("Input line offset need 32 alignment!!!!\n");
            return -1;
        }
    }
    else if((pstDivpInputInfo->ePxlFmt == E_MHAL_DIVP_PIXEL_FORMAT_YC420_MSTTILE1_H264) || \
            (pstDivpInputInfo->ePxlFmt == E_MHAL_DIVP_PIXEL_FORMAT_YC420_MSTTILE2_H265))  // only support tile 16x32
    {
        if(pstDivpInputInfo->u16InputWidth % 32)
        {
            FUNC_ERR("Input width need 32 alignment!!!!\n");
            return -1;
        }
        //if(pstDivpInputInfo->u16InputHeight % 64)
        //{
        //    FUNC_ERR("Input height need 64 alignment!!!!\n");
        //    return -1;
        //}
        //u16LineOffset = ((pstDivpInputInfo->u16Stride[0] * 2) / 3);
        u16LineOffset = pstDivpInputInfo->u16Stride[0];
        if(u16LineOffset % 32)
        {
            FUNC_ERR("Input line offset need 32 alignment!!!!\n");
            return -1;
        }
    }
    else
    {
        FUNC_ERR("Input format error!!!!\n");
        return -1;
    }

    if(pstDivpOutputInfo->ePxlFmt == E_MHAL_DIVP_PIXEL_FORMAT_YUV422_YUYV)
    {
        if(pstDivpOutputInfo->u16Stride[0] % 32)
        {
            FUNC_ERR("Output line offset need 16 alignment!!!!\n");
            return -1;
        }
    }
    else if((pstDivpOutputInfo->ePxlFmt == E_MHAL_DIVP_PIXEL_FORMAT_YUV_SEMIPLANAR_420) || \
            (pstDivpOutputInfo->ePxlFmt == E_MHAL_DIVP_PIXEL_FORMAT_YC420_MSTTILE1_H264) || \
            (pstDivpOutputInfo->ePxlFmt == E_MHAL_DIVP_PIXEL_FORMAT_YUV_MST_420))
    {
        if(pstDivpOutputInfo->u16OutputWidth > 1920)
        {
            FUNC_ERR("Output width must be equal to or smaller than 1920!!!!\n");
            return -1;
        }

        if(pstDivpOutputInfo->ePxlFmt == E_MHAL_DIVP_PIXEL_FORMAT_YUV_MST_420)
        {
            u16LineOffset = ((pstDivpOutputInfo->u16Stride[0] * 2) / 3);
        }
        else
        {
            u16LineOffset = pstDivpOutputInfo->u16Stride[0];
        }

        if(u16LineOffset % 32)
        {
            FUNC_ERR("Output line offset need 32 alignment!!!!\n");
            return -1;
        }
    }
    else
    {
        FUNC_ERR("Output format error!!!!\n");
        return -1;
    }

    if(pDivpInst->stAttr.eRotateType !=  E_MHAL_DIVP_ROTATE_NONE)
    {
        if((pstDivpInputInfo->ePxlFmt != E_MHAL_DIVP_PIXEL_FORMAT_YUV_SEMIPLANAR_420) || \
                (pstDivpOutputInfo->ePxlFmt != E_MHAL_DIVP_PIXEL_FORMAT_YUV_SEMIPLANAR_420))
        {
            FUNC_ERR("Rotation only support YUV420 semi-planar format!!!!\n");
            return -1;
        }

        if((pstDivpInputInfo->u16InputWidth > 1920) || (pstDivpInputInfo->u16InputHeight > 1920) || \
                (pstDivpOutputInfo->u16OutputWidth > 1920) || (pstDivpOutputInfo->u16OutputHeight > 1920))
        {
            FUNC_ERR("Input/Output width/height should be smaller than 1920 with Rotation 90/270!!!!\n");
            return -1;
        }
        if((pstDivpInputInfo->u16InputWidth % 64) || (pstDivpInputInfo->u16InputHeight % 64) || \
                (pstDivpOutputInfo->u16OutputWidth % 64) || (pstDivpOutputInfo->u16OutputHeight % 64))
        {
            FUNC_ERR("Input/Output width/height need 64 alignment with Rotation 90/270!!!!\n");
            return -1;
        }

        if (pstDivpInputInfo->stCropWin.u16Height != pstDivpOutputInfo->u16OutputWidth ||
                pstDivpInputInfo->stCropWin.u16Width != pstDivpOutputInfo->u16OutputHeight)
        {
            FUNC_ERR("Can't support scaling up/down with Rotation 90/270!!!!\n");
            return -1;
        }

    }

    if(pstDivpInputInfo->stMfdecInfo.bDbEnable == TRUE)
    {
        if(pstDivpInputInfo->stMfdecInfo.u8DbSelect != 0)  // only support MFDEC ID0
        {
            FUNC_ERR("u8DbSelect = %d, Only support MFDEC ID0!!!!\n", pstDivpInputInfo->stMfdecInfo.u8DbSelect);
            return -1;
        }
    }

    return 0;
}

// dip init, only run one times
MS_BOOL DipInit(MHAL_DIVP_DeviceId_e eDevId)
{
    SCALER_DIP_WIN enDipWin = (SCALER_DIP_WIN)eDevId;

    FUNC_MSG("enDipWin = %d\n", enDipWin);
    if(g_bIsInit[enDipWin])
    {
        FUNC_MSG("dip[%d] has already init.\n", enDipWin);
        return TRUE;
    }

    //do dip init
    g_bIsInit[enDipWin] = TRUE;
    memset(&g_stDipRes[enDipWin], 0, sizeof(DivpPrivateDevData));

    MDrv_XC_DIVP_Init(NULL, enDipWin);

    return TRUE;
}

// dip deinit, only run one times
MS_BOOL DipDeInit(MHAL_DIVP_DeviceId_e eDevId)
{
    SCALER_DIP_WIN enDipWin = (SCALER_DIP_WIN)eDevId;

    FUNC_MSG("enDipWin = %d\n", enDipWin);
    g_bIsInit[enDipWin] = FALSE;
    memset(&g_stDipRes[enDipWin], 0, sizeof(DivpPrivateDevData));

    MDrv_XC_DIVP_DeInit(NULL, enDipWin);

    return TRUE;
}

//------------------------------------------------------------------------------
/// @brief Init DIVP Hardware
/// @param[in] eDevId: DIVP device ID.
/// @return MHAL_ErrCode_e
//------------------------------------------------------------------------------
MS_S32 MHAL_DIVP_Init(MHAL_DIVP_DeviceId_e eDevId)
{
    MS_S32 rt = MHAL_FAILURE;
    if(DipInit(eDevId))
    {
        rt = MHAL_SUCCESS;
    }
    else
    {
        FUNC_ERR("DipInit fail .\n");
    }

    return rt ;
}
EXPORT_SYMBOL(MHAL_DIVP_Init);

//------------------------------------------------------------------------------
/// @brief deInit DIVP Hardware
/// @param[in] eDevId: DIVP device ID.
/// @return MHAL_ErrCode_e
//------------------------------------------------------------------------------
MS_S32 MHAL_DIVP_DeInit(MHAL_DIVP_DeviceId_e eDevId)
{
    MS_S32 rt = MHAL_FAILURE;
    if(DipDeInit(eDevId))
    {
        rt = MHAL_SUCCESS;
    }
    else
    {
        FUNC_ERR("DipDeInit fail .\n");
    }

    return rt ;
}
EXPORT_SYMBOL(MHAL_DIVP_DeInit);

MS_S32 MHAL_DIVP_GetChipCapbility(MHAL_DIVP_DeviceId_e eDevId, MS_U32* pu32Caps)
{
    MS_S32 s32Ret = MHAL_FAILURE;
    ST_XC_DIP_CHIPCAPS stDivpCaps;
    memset(&stDivpCaps, 0, sizeof(stDivpCaps));

    if((NULL == pu32Caps) || (eDevId >= E_MHAL_DIVP_Device_MAX))
    {
        FUNC_ERR("invalid parameter : pu32Caps = %p, eDevId = %u .\n", pu32Caps, eDevId);
        return s32Ret;
    }

    if(E_MHAL_DIVP_Device0 == eDevId)
    {
        *pu32Caps = 1;
        s32Ret = MHAL_SUCCESS;
    }
    else
    {
        *pu32Caps = 0;
    }

    return s32Ret;
}
EXPORT_SYMBOL(MHAL_DIVP_GetChipCapbility);

//------------------------------------------------------------------------------
/// @brief Open Video handler
/// @param[in] eDevId: DIVP device ID.
/// @param[in] pstAlloc: function pointer to allocate memory.
/// @param[in] pfFree: function pointer to free memory.
/// @param[in] u16MaxWidth: max width channel supportrd.
/// @param[in] u16MaxHeight: max height channel supportrd.
/// @param[out] pCtx: DIVP channel contex in hal and down layer.
/// @return MHAL_ErrCode_e
//------------------------------------------------------------------------------
MS_S32 MHAL_DIVP_CreateInstance(MHAL_DIVP_DeviceId_e eDevId, MS_U16 u16MaxWidth,
                                MS_U16 u16MaxHeight, PfnAlloc pfAlloc, PfnFree pfFree, MS_U8 u8ChannelId, void** ppCtx)
{
    MHAL_DIVP_InstPrivate_t *pDivpInst = NULL;
    char pCtxNumString[3];
    //    char pSring[128];
    SCALER_DIP_WIN enDipWin = (SCALER_DIP_WIN)eDevId;
    DivpPrivateDevData *dev = &g_stDipRes[enDipWin];

    FUNC_MSG("id = %d, MaxW = %d, MaxH = %d\n", eDevId, u16MaxWidth, u16MaxHeight);

    if((u16MaxWidth > 4096) || (u16MaxHeight > 2160))
    {
        FUNC_ERR("Input parameter error!!!!\n");
        return -1;
    }

    pDivpInst = (MHAL_DIVP_InstPrivate_t *)kzalloc((sizeof(MHAL_DIVP_InstPrivate_t)), GFP_KERNEL);
    if(pDivpInst != NULL)
    {
        //memset(pDivpInst, 0, sizeof(MHAL_DIVP_InstPrivate_t));

        pDivpInst->bInit = TRUE;
        pDivpInst->eId = eDevId;
        pDivpInst->nMaxWidth = u16MaxWidth;
        pDivpInst->nMaxHeight = u16MaxHeight;

        pDivpInst->u64AllocAddr = 0;
        pDivpInst->u32AllocSize = 0;
        pDivpInst->bIsDiEnable = FALSE;
        pDivpInst->u32CurFrameCnt = 0;

        if(pfAlloc != NULL)
            pDivpInst->pfAlloc = (PfnAlloc)pfAlloc;

        if(pfFree != NULL)
            pDivpInst->pfFree = (PfnFree)pfFree;

        if(pfAlloc != NULL)
            dev->pfAlloc = (PfnAlloc)pfAlloc;

        if(pfFree != NULL)
            dev->pfFree = (PfnFree)pfFree;

        dev->u8NumOpening++;

        Int2Str(dev->u8NumOpening, pCtxNumString);
        strcpy(pDivpInst->pCtxString, g_CtxName);
        strcat(pDivpInst->pCtxString, pCtxNumString);

        pDivpInst->u8ChannelId = 0;
        while (dev->ctx[pDivpInst->u8ChannelId])
        {
            pDivpInst->u8ChannelId++;
            if (pDivpInst->u8ChannelId >= MAX_CHANNEL_NUM)
            {
                FUNC_ERR("Too many open channels!!!!\n");
                return -1;
            }
        }
        dev->ctx[pDivpInst->u8ChannelId] = pDivpInst;

        *ppCtx = pDivpInst;

        FUNC_MSG("[%s]NumOpen=%d, ChanId=%d Ctx=0x%x\n", pDivpInst->pCtxString, dev->u8NumOpening, pDivpInst->u8ChannelId, (int)pDivpInst);

    }
    else
    {
        FUNC_ERR("[divp malloc error] pDivpInst == NULL\n");
    }

    return 0;
}
EXPORT_SYMBOL(MHAL_DIVP_CreateInstance);

//------------------------------------------------------------------------------
/// @brief destroy DIVP channel inistance
/// @param[in] pCtx: DIVP channel contex in hal and down layer.
/// @return MHAL_ErrCode_e
//------------------------------------------------------------------------------
MS_S32 MHAL_DIVP_DestroyInstance(void* pCtx)
{
    MHAL_DIVP_InstPrivate_t *pDivpInst = (MHAL_DIVP_InstPrivate_t *)pCtx;
    SCALER_DIP_WIN enDipWin = (SCALER_DIP_WIN)pDivpInst->eId;
    DivpPrivateDevData *dev = &g_stDipRes[enDipWin];

    FUNC_MSG("pDivpInst = 0x%x, enDipWin = %d\n", (int)pDivpInst, enDipWin);

    if(pDivpInst)
    {
        dev->ctx[pDivpInst->u8ChannelId] = NULL;

        if(dev->u8NumOpening > 0)
            dev->u8NumOpening--;

        if(pDivpInst->bIsDiEnable == TRUE)
        {
            if(pDivpInst->u64AllocAddr != 0)
            {
                _DipFreeMem(pDivpInst, &pDivpInst->u64AllocAddr);
            }
            pDivpInst->bIsDiEnable = FALSE;
        }

        if(pDivpInst->pfAlloc != NULL)
            pDivpInst->pfAlloc = NULL;
        if(pDivpInst->pfFree != NULL)
            pDivpInst->pfFree = NULL;

        kfree(pDivpInst);
    }
    return 0;
}
EXPORT_SYMBOL(MHAL_DIVP_DestroyInstance);

//------------------------------------------------------------------------------
/// @brief capture a picture from display VOP/OP2.
/// @param[in] pstCaptureInfo: config information of the picture captured by DIVP.
/// @param[in] pstCmdQInfo: a group of function pointer for controlling HW register. These functions
///    will deside write/read regster with RIU or Command queue.
/// @return MHAL_ErrCode_e
//------------------------------------------------------------------------------
MS_S32 MHAL_DIVP_CaptureTiming(void* pCtx, MHAL_DIVP_CaptureInfo_t* pstCaptureInfo, MHAL_CMDQ_CmdqInterface_t* pstCmdQInfo)
{
    MHAL_DIVP_InstPrivate_t *pDivpInst = (MHAL_DIVP_InstPrivate_t *)pCtx;
    MS_S32 ret = 0;
    EN_DRV_XC_DWIN_DATA_FMT eFmt = XC_DWIN_DATA_FMT_MAX;
    MHAL_DIVP_OutPutInfo_t stDIPOutputInfo;
    SCALER_DIP_SOURCE_TYPE eInSrcType = SCALER_DIP_SOURCE_TYPE_OP_CAPTURE;
    DIP_MDWIN_PROPERTY stMDWinProperty = {0};

    ret = mDrv_XC_DIVP_CaptureConfigCheck(pDivpInst, pstCaptureInfo);
    if(ret != 0)
    {
        return ret;
    }

    //MDrv_XC_DIVP_SetMirror(pstCmdQInfo, pDivpInst->stAttr.stMirror.bHMirror, pDivpInst->stAttr.stMirror.bVMirror, DIP_WINDOW);

    MDrv_XC_DIVP_SelectSourceScanType(pstCmdQInfo, GOPDWIN_SCAN_MODE_PROGRESSIVE, DIP_WINDOW);

    MDrv_XC_DIVP_SetInterlaceWrite(pstCmdQInfo, FALSE, DIP_WINDOW);

    eInSrcType = MDrv_XC_DIVP_GetSrcType(pstCaptureInfo->eDispId, pstCaptureInfo->eCapStage);
    if(eInSrcType >= MAX_SCALER_DIP_SOURCE_NUM)
    {
        return -1;
    }

    // Init Info
    mDrv_XC_DIVP_SourceType_Set(DIP_WINDOW,
                                INPUT_SOURCE_VGA,
                                eInSrcType,
                                FALSE,
                                FALSE);
    mDrv_XC_DIVP_Crop_Set(DIP_WINDOW,
                          pstCaptureInfo->stCropWin.u16X,
                          pstCaptureInfo->stCropWin.u16Y,
                          pstCaptureInfo->stCropWin.u16Width,
                          pstCaptureInfo->stCropWin.u16Height);

#if (DIP_PLATFORM == DIP_I2)

    mDrv_XC_DIVP_Scale_Set(DIP_WINDOW,
                           pstCaptureInfo->stCropWin.u16Width,
                           pstCaptureInfo->stCropWin.u16Height,
                           pstCaptureInfo->stCropWin.u16Width,
                           pstCaptureInfo->stCropWin.u16Height);

#else

    mDrv_XC_DIVP_Scale_Set(DIP_WINDOW,
                           pstCaptureInfo->stCropWin.u16Width,
                           pstCaptureInfo->stCropWin.u16Height,
                           pstCaptureInfo->u16Width,
                           pstCaptureInfo->u16Height);

#endif

    g_DIVPOutPath = MDrv_XC_DIVP_GetDestinationPath(pstCaptureInfo->eOutputPxlFmt);
    eFmt = MDrv_XC_DIVP_TransPixelFormat(pstCaptureInfo->eOutputPxlFmt);
    MDrv_XC_DIVP_SetDataFmt(pstCmdQInfo, eFmt, g_DIVPOutPath, DIP_WINDOW);
    if(eFmt == XC_DWIN_DATA_FMT_ARGB8888)
    {
        MDrv_XC_DIVP_SetAlphaValue(pstCmdQInfo, 0xFF, DIP_WINDOW);
        MDrv_XC_DIVP_SetY2R(pstCmdQInfo, TRUE, DIP_WINDOW);
    }
    else
    {
        MDrv_XC_DIVP_SetY2R(pstCmdQInfo, FALSE, DIP_WINDOW);
    }

    stDIPOutputInfo.u64BufAddr[0] = pstCaptureInfo->u64BufAddr[0];
    stDIPOutputInfo.u64BufAddr[1] = pstCaptureInfo->u64BufAddr[1];
    stDIPOutputInfo.u64BufAddr[2] = pstCaptureInfo->u64BufAddr[2];
    stDIPOutputInfo.u32BufSize = pstCaptureInfo->u32BufSize;
    stDIPOutputInfo.u16OutputWidth = pstCaptureInfo->u16Width;
    stDIPOutputInfo.u16OutputHeight = pstCaptureInfo->u16Height;
    stDIPOutputInfo.u16Stride[0] = pstCaptureInfo->u16Stride[0];
    stDIPOutputInfo.u16Stride[1] = pstCaptureInfo->u16Stride[1];
    stDIPOutputInfo.u16Stride[2] = pstCaptureInfo->u16Stride[2];
    stDIPOutputInfo.ePxlFmt = pstCaptureInfo->eOutputPxlFmt;

    MDrv_XC_DIVP_SetWindow(pstCmdQInfo, pDivpInst, &stDIPOutputInfo, DIP_WINDOW);

    MDrv_XC_DIVP_SetWinProperty(pstCmdQInfo, pDivpInst, &stDIPOutputInfo, DIP_WINDOW);

    stMDWinProperty.bEnable = FALSE;
    HAL_XC_DIP_MDWin_SetConfig(pstCmdQInfo, &stMDWinProperty);

    // Start DIP Process
#ifdef DIP_UT_ENABLE_RIU  // busy waiting until capture done

#ifdef DIP_UT_WO_IRQ    // busy waiting
    MDrv_XC_DIVP_CaptureOneFrame(pstCmdQInfo, DIP_WINDOW);
#else
    MDrv_XC_DIVP_CaptureOneFrame2(pstCmdQInfo, DIP_WINDOW);
#endif

#ifdef DIP_REG_DUMP
    HAL_XC_INFO_dump_Reg(0x34, 0x00, 0x80);
    HAL_XC_INFO_dump_Reg(0x36, 0x00, 0x80);
    HAL_XC_INFO_dump_Reg(0x49, 0x00, 0x80);
#endif

#else

    MDrv_XC_DIVP_CaptureOneFrame2(pstCmdQInfo, DIP_WINDOW);

#endif

    return ret;
}
EXPORT_SYMBOL(MHAL_DIVP_CaptureTiming);

//------------------------------------------------------------------------------
/// @brief DIVP deal with a frame and than write to dram. The frame will be used by VPE...
/// @param[in] pCtx: DIVP channel contex in hal and down layer.
/// @param[in] pstDivpInputInfo: config information of the input frame.
/// @param[in] pstDivpOutputInfo: config information of the output frame.
/// @param[in] pstCmdQInfo: a group of function pointer for controlling HW register. These functions
///    will deside write/read regster with RIU or Command queue.
/// @return MHAL_ErrCode_e
//------------------------------------------------------------------------------
MS_S32 MHAL_DIVP_ProcessDramData(void* pCtx, MHAL_DIVP_InputInfo_t* pstDivpInputInfo,
                                 MHAL_DIVP_OutPutInfo_t* pstDivpOutputInfo, MHAL_CMDQ_CmdqInterface_t* pstCmdQInfo)
{
    MHAL_DIVP_InstPrivate_t *pDivpInst = (MHAL_DIVP_InstPrivate_t *)pCtx;
    MS_S32 ret = 0;
    //ST_XC_DIPR_PROPERTY stDIPRProperty = {0};
    //ST_XC_DIPR_PROPERTY_EX stDIPRProperty_EX;
    EN_DRV_XC_DWIN_DATA_FMT eFmt = XC_DWIN_DATA_FMT_MAX;
    EN_XC_DIP_TILE_BLOCK eTileMode = DIP_TILE_BLOCK_R_32_32;
    MS_BOOL bSupport = FALSE;
    //ST_DIP_DI_SETTING *pstDipr3DDiSetting = &g_stDipRes[0].st3DDISetting;
    ST_XC_DIPR_PROPERTY *pstDiprInfo = &g_stDipRes[0].stDiprInfo.stDIPRProperty;
    EN_XC_DWIN_SCAN_TYPE eScanMode = GOPDWIN_SCAN_MODE_PROGRESSIVE;
    EN_XC_DIP_ROTATION eRotationType = E_XC_DIP_ROTATION_270;
    DIP_MDWIN_PROPERTY stMDWinProperty = {0};

    ret = mDrv_XC_DIVP_InputConfigCheck(pDivpInst, pstDivpInputInfo, pstDivpOutputInfo);
    if(ret != 0)
    {
        return ret;
    }

    //MDrv_XC_DIVP_SetMirror(pstCmdQInfo, pDivpInst->stAttr.stMirror.bHMirror, pDivpInst->stAttr.stMirror.bVMirror, DIP_WINDOW);

#if 0  // test YC swap
    HAL_XC_DIP_SetYCSwap(pstCmdQInfo, TRUE, DIP_WINDOW);
#endif

    if((pDivpInst->stAttr.eRotateType == E_MHAL_DIVP_ROTATE_90) || (pDivpInst->stAttr.eRotateType == E_MHAL_DIVP_ROTATE_270))
    {
        eRotationType = (pDivpInst->stAttr.eRotateType == E_MHAL_DIVP_ROTATE_90) ? E_XC_DIP_ROTATION_90 : E_XC_DIP_ROTATION_270;
        MDrv_XC_DIVP_SetRotation(pstCmdQInfo, TRUE, eRotationType, DIP_WINDOW);
    }
    else
    {
        MDrv_XC_DIVP_SetRotation(pstCmdQInfo, FALSE, eRotationType, DIP_WINDOW);
    }

    eScanMode = (pstDivpInputInfo->eScanMode == E_MHAL_DIVP_SCAN_MODE_PROGRESSIVE) ?  GOPDWIN_SCAN_MODE_PROGRESSIVE : GOPDWIN_SCAN_MODE_INTERLACE;
    MDrv_XC_DIVP_SelectSourceScanType(pstCmdQInfo, eScanMode, DIP_WINDOW);

    MDrv_XC_DIVP_SetInterlaceWrite(pstCmdQInfo, FALSE, DIP_WINDOW);

#if 1
    memset(&g_stDipRes[0].stDiprInfo.stDIPRProperty, 0, sizeof(ST_XC_DIPR_PROPERTY));
    memset(&g_stDipRes[0].stDiprInfo.stDIPR_MFDecInfo, 0, sizeof(ST_DIP_MFDEC_INFO));
    memset(&g_stDipRes[0].stDiprInfo.stDIPR3DDISetting, 0, sizeof(ST_DIP_DIPR_3DDI_SETTING));
#else
    memset(&g_stDipRes[0].stDiprInfo, 0, sizeof(ST_XC_DIPR_PROPERTY_EX));
#endif

    // Setup DIPR Property
    pstDiprInfo->u16Width = pstDivpInputInfo->u16InputWidth;
    pstDiprInfo->u16Height = pstDivpInputInfo->u16InputHeight;
    pstDiprInfo->u16Pitch = pstDivpInputInfo->u16Stride[0];
    pstDiprInfo->u32YBufAddr = (MS_U32)pstDivpInputInfo->u64BufAddr[0];
    pstDiprInfo->u32CBufAddr = (MS_U32)pstDivpInputInfo->u64BufAddr[1];
    pstDiprInfo->enDataFmt = (EN_XC_DIP_DATA_FMT)MDrv_XC_DIVP_TransPixelFormat(pstDivpInputInfo->ePxlFmt);

    g_stDipRes[0].stDiprInfo.stDIPR_VerCtl.u32version = 0;
    g_stDipRes[0].stDiprInfo.stDIPR_VerCtl.u32size = sizeof(ST_XC_DIPR_PROPERTY_EX);
    g_stDipRes[0].stDiprInfo.pDIPRSetting = NULL;

    if(pstDivpInputInfo->stMfdecInfo.bDbEnable == TRUE)
    {
        // Modify DIPR width/height depends on MFDEC crop infomation
        pstDiprInfo->u16Width = pstDivpInputInfo->stMfdecInfo.u16Width;
        pstDiprInfo->u16Height = pstDivpInputInfo->stMfdecInfo.u16Height;
        //pstDiprInfo->u16Pitch = pstDivpInputInfo->stMfdecInfo.u16Width;
        pstDivpInputInfo->u16InputWidth = pstDiprInfo->u16Width;
        pstDivpInputInfo->u16InputHeight = pstDiprInfo->u16Height;
        pstDivpInputInfo->u16Stride[0] = pstDivpInputInfo->stMfdecInfo.u16Width;

        ret = MDrv_XC_DIVP_SetDIPRConfig_MFDEC(pstCmdQInfo, pDivpInst, pstDivpInputInfo, DIP_WINDOW);
        if(ret != 0)
        {
            return ret;
        }
    }
    else if(pstDivpInputInfo->eScanMode != E_MHAL_DIVP_SCAN_MODE_PROGRESSIVE)
    {
        ret = MDrv_XC_DIVP_SetDIPRConfig_3DDI(pstCmdQInfo, pDivpInst, pstDivpInputInfo, DIP_WINDOW);
        if(ret != 0)
        {
            return ret;
        }
    }
    else
    {
        //MDrv_XC_DIVP_SetDIPRProperty(pstCmdQInfo, pDivpInst, pstDiprInfo, DIP_WINDOW);
        MDrv_XC_DIVP_SetDIPRProperty_EX(pstCmdQInfo, pDivpInst, &g_stDipRes[0].stDiprInfo, DIP_WINDOW);
    }

    if( (pstDiprInfo->enDataFmt == DIP_DATA_FMT_YUV420_H265) ||
            (pstDiprInfo->enDataFmt == DIP_DATA_FMT_YUV420_H265_10BITS) ||
            (pstDiprInfo->enDataFmt == DIP_DATA_FMT_YUV420))
    {
        if(pstDivpInputInfo->stMfdecInfo.bDbEnable == TRUE)
        {
            // set Input Tile mode format
            eTileMode = DIP_TILE_BLOCK_R_32_16;  // force tile read 32x16 if MFDEC enable
        }
        else
        {
            // set Input Tile mode format
            eTileMode = MDrv_XC_DIVP_TransTileMode(pstDivpInputInfo->eTileMode);
        }
        bSupport = MDrv_XC_DIVP_Set420TileBlock(pstCmdQInfo, eTileMode, DIP_WINDOW);
        if(bSupport == FALSE)
        {
            FUNC_ERR("Input Tile mode don't support!!!!\n");
            return FALSE;
        }
    }

    // Init Info
    mDrv_XC_DIVP_SourceType_Set(DIP_WINDOW,
                                INPUT_SOURCE_VGA,
                                SCALER_DIP_SOURCE_TYPE_DRAM,
                                FALSE,
                                FALSE);
    mDrv_XC_DIVP_Crop_Set(DIP_WINDOW,
                          pstDivpInputInfo->stCropWin.u16X,
                          pstDivpInputInfo->stCropWin.u16Y,
                          pstDivpInputInfo->stCropWin.u16Width,
                          pstDivpInputInfo->stCropWin.u16Height);

#if (DIP_PLATFORM == DIP_I2)

    if(pDivpInst->bIsDiEnable == TRUE)
    {
        mDrv_XC_DIVP_Scale_Set(DIP_WINDOW,
                               pstDivpInputInfo->stCropWin.u16Width,
                               pstDivpInputInfo->stCropWin.u16Height,
                               pstDivpInputInfo->stCropWin.u16Width,
                               pstDivpInputInfo->stCropWin.u16Height);
    }
    else
    {
        mDrv_XC_DIVP_Scale_Set(DIP_WINDOW,
                               pstDivpInputInfo->stCropWin.u16Width,
                               pstDivpInputInfo->stCropWin.u16Height,
                               pstDivpInputInfo->stCropWin.u16Width,
                               pstDivpInputInfo->stCropWin.u16Height);
    }

#else

    if(pDivpInst->bIsDiEnable == TRUE)
    {
        mDrv_XC_DIVP_Scale_Set(DIP_WINDOW,
                               pstDivpInputInfo->stCropWin.u16Width,
                               pstDivpInputInfo->stCropWin.u16Height,
                               pstDivpOutputInfo->u16OutputWidth,
                               (pstDivpOutputInfo->u16OutputHeight >> 1));
    }
    else
    {
        mDrv_XC_DIVP_Scale_Set(DIP_WINDOW,
                               pstDivpInputInfo->stCropWin.u16Width,
                               pstDivpInputInfo->stCropWin.u16Height,
                               pstDivpOutputInfo->u16OutputWidth,
                               pstDivpOutputInfo->u16OutputHeight);
    }
#endif

    g_DIVPOutPath = MDrv_XC_DIVP_GetDestinationPath(pstDivpOutputInfo->ePxlFmt);
    eFmt = MDrv_XC_DIVP_TransPixelFormat(pstDivpOutputInfo->ePxlFmt);
    MDrv_XC_DIVP_SetDataFmt(pstCmdQInfo, eFmt, g_DIVPOutPath, DIP_WINDOW);
    if(eFmt == XC_DWIN_DATA_FMT_ARGB8888)
    {
        MDrv_XC_DIVP_SetAlphaValue(pstCmdQInfo, 0xFF, DIP_WINDOW);
        MDrv_XC_DIVP_SetY2R(pstCmdQInfo, TRUE, DIP_WINDOW);
    }
    else
    {
        MDrv_XC_DIVP_SetY2R(pstCmdQInfo, FALSE, DIP_WINDOW);
    }

#if 1  // test output YUV420 Tile 16x32 mode
    if(eFmt == XC_DWIN_DATA_FMT_YUV420)
    {
        eTileMode = DIP_TILE_BLOCK_W_16_32;
        //eTileMode = DIP_TILE_BLOCK_W_32_16;
        //eTileMode = DIP_TILE_BLOCK_W_32_32;

        bSupport = MDrv_XC_DIVP_Set420TileBlock(pstCmdQInfo, eTileMode, DIP_WINDOW);
        if(bSupport == FALSE)
        {
            FUNC_ERR("Output Tile mode don't support!!!!\n");
            return FALSE;
        }
    }
#endif

    MDrv_XC_DIVP_SetWindow(pstCmdQInfo, pDivpInst, pstDivpOutputInfo, DIP_WINDOW);

    MDrv_XC_DIVP_SetWinProperty(pstCmdQInfo, pDivpInst, pstDivpOutputInfo, DIP_WINDOW);

    //#if (MDWIN_FOR_UT)
    if(g_DIVPOutPath == DIP_OUTPUT_PATH_MDWIN)
    {
        HAL_XC_DIP_MDWinInit(pstCmdQInfo, 1);  // ID 1 for DIVP
        stMDWinProperty.bEnable = TRUE;
        stMDWinProperty.u8WinId = 1;  // ID 1 for DIVP
        stMDWinProperty.u8WinIndex = 0;
        stMDWinProperty.u8Format = 0x32;  // 0x30: YUV422 8b, 0x32: YC420 8b
        stMDWinProperty.u32BaseAddr = pstDivpOutputInfo->u64BufAddr[0];
        stMDWinProperty.u16Lineoffset = (pstDivpOutputInfo->u16Stride[0] * 2 / 3);  // by pixels
        stMDWinProperty.u16Width = pstDivpOutputInfo->u16OutputWidth;
        stMDWinProperty.u16Height = pstDivpOutputInfo->u16OutputHeight;
        stMDWinProperty.bHmirror = FALSE;
        stMDWinProperty.bVflip = FALSE;
        HAL_XC_DIP_MDWin_SetConfig(pstCmdQInfo, &stMDWinProperty);
    }
    else
    {
        stMDWinProperty.bEnable = FALSE;
        HAL_XC_DIP_MDWin_SetConfig(pstCmdQInfo, &stMDWinProperty);
    }
    //#endif

    // Start DIP Process
#ifdef DIP_UT_ENABLE_RIU  // busy waiting until capture done

#ifdef DIP_UT_WO_IRQ  // busy waiting
    MDrv_XC_DIVP_CaptureOneFrame(pstCmdQInfo, DIP_WINDOW);
#else
    MDrv_XC_DIVP_CaptureOneFrame2(pstCmdQInfo, DIP_WINDOW);
#endif

#ifdef DIP_REG_DUMP
    HAL_XC_INFO_dump_Reg(0x34, 0x00, 0x80);
    HAL_XC_INFO_dump_Reg(0x36, 0x00, 0x80);

    if(pstDivpInputInfo->eScanMode != E_MHAL_DIVP_SCAN_MODE_PROGRESSIVE)
    {
        HAL_XC_INFO_dump_Reg(0x43, 0x00, 0x80);
        HAL_XC_INFO_dump_Reg(0x47, 0x00, 0x80);
    }
    HAL_XC_INFO_dump_Reg(0x49, 0x00, 0x80);
    //HAL_MFDEC_INFO_dump_Reg();
    if(g_DIVPOutPath == DIP_OUTPUT_PATH_MDWIN)
    {
        HAL_XC_INFO_dump_Reg(0xF5, 0x00, 0x80);
        HAL_XC_INFO_dump_Reg(0xF6, 0x00, 0x80);
        //HAL_XC_INFO_dump_Reg(0x00, 0x00, 0x80);
    }
#endif

#else

    MDrv_XC_DIVP_CaptureOneFrame2(pstCmdQInfo, DIP_WINDOW);

#endif

    return ret;
}
EXPORT_SYMBOL(MHAL_DIVP_ProcessDramData);

//------------------------------------------------------------------------------
/// @brief set attribute of DIVP channel.
/// @param[in] pCtx: DIVP channel contex in hal and down layer.
/// @param[in] eAttrType: DIVP channel attribute type.
/// @param[in] pAttrParam: config information of DIVP channel.
///    if(E_MHAL_DIVP_ATTR_TNR == eAttrType) ==> pAttr is MHAL_DIVP_TnrLevel_e*.
///    if(E_MHAL_DIVP_ATTR_DI == eAttrType) ==> pAttr is MHAL_DIVP_DiType_e*.
///    if(E_MHAL_DIVP_ATTR_ROTATE == eAttrType) ==> pAttr is MHAL_DIVP_Rotate_e*.
///    if(E_MHAL_DIVP_ATTR_CROP == eAttrType) ==> pAttr is MHAL_DIVP_Window_t*.
///    if(E_MHAL_DIVP_ATTR_MIRROR == eAttrType) ==> pAttr is MHAL_DIVP_Mirror_t*.
/// @param[in] pstCmdQInfo: a group of function pointer for controlling HW register. These functions
///    will deside write/read regster with RIU or Command queue.
/// @return MHAL_ErrCode_e
//------------------------------------------------------------------------------
MS_S32 MHAL_DIVP_SetAttr(void* pCtx, MHAL_DIVP_AttrType_e eAttrType, const void* pAttrParam, MHAL_CMDQ_CmdqInterface_t* pstCmdQInfo)
{
    MHAL_DIVP_InstPrivate_t *pDivpInst = (MHAL_DIVP_InstPrivate_t *)pCtx;

    if (pDivpInst == NULL)
    {
        FUNC_ERR("No instance existed, please get an instance by calling MHAL_DIVP_CreateInstance() first\n");
        return -1;
    }

    //FUNC_MSG("[0x%x]eAttrType=%d\n", (MS_U32)pCtx, eAttrType);

    switch(eAttrType)
    {
        case E_MHAL_DIVP_ATTR_TNR:
            pDivpInst->stAttr.eTnrLevel = *(MHAL_DIVP_TnrLevel_e *)pAttrParam;
            FUNC_MSG("TNR=%d\n", pDivpInst->stAttr.eTnrLevel);
            break;
        case E_MHAL_DIVP_ATTR_DI:
            pDivpInst->stAttr.eDiType = *(MHAL_DIVP_DiType_e *)pAttrParam;
            FUNC_MSG("DI=%d\n", pDivpInst->stAttr.eDiType);
            break;
        case E_MHAL_DIVP_ATTR_ROTATE:
            pDivpInst->stAttr.eRotateType = *(MHAL_DIVP_Rotate_e *)pAttrParam;
            FUNC_MSG("ROTATE=%d\n", pDivpInst->stAttr.eRotateType);
            if(pDivpInst->stAttr.eRotateType == E_MHAL_DIVP_ROTATE_180)
            {
                FUNC_ERR("Don't support rotate 180 degree!!!!\n");
                return -1;
            }
            break;
        case E_MHAL_DIVP_ATTR_CROP:
            pDivpInst->stAttr.stCropWin.u16X = ((MHAL_DIVP_Window_t *)pAttrParam)->u16X;
            pDivpInst->stAttr.stCropWin.u16Y = ((MHAL_DIVP_Window_t *)pAttrParam)->u16Y;
            pDivpInst->stAttr.stCropWin.u16Width = ((MHAL_DIVP_Window_t *)pAttrParam)->u16Width;
            pDivpInst->stAttr.stCropWin.u16Height = ((MHAL_DIVP_Window_t *)pAttrParam)->u16Height;
            break;
        case E_MHAL_DIVP_ATTR_MIRROR:
            FUNC_ERR("Don't support mirror/flip!!!!\n");
            //pDivpInst->stAttr.stMirror.bHMirror = ((MHAL_DIVP_Mirror_t *)pAttrParam)->bHMirror;
            //pDivpInst->stAttr.stMirror.bVMirror = ((MHAL_DIVP_Mirror_t *)pAttrParam)->bVMirror;
            break;
        default:
            FUNC_ERR("Unkonwn parameter type !!!!\n");
            break;

    }
    return 0;
}
EXPORT_SYMBOL(MHAL_DIVP_SetAttr);

//------------------------------------------------------------------------------
/// @brief DIVP deal with a frame and than write to dram. The frame will be used by DISP.
/// @param[in] bEnable: control DIVP module's ISR of frame done. TURE: accept interrupt,
///    FALSE:ignore frame done ISR.
/// @return MHAL_ErrCode_e
//------------------------------------------------------------------------------
MS_S32 MHAL_DIVP_EnableFrameDoneIsr(MS_BOOL bEnable)
{
    MDrv_XC_DIVP_EnableIntr(NULL, 0x01, bEnable, DIP_WINDOW);
    //MDrv_XC_DIVP_CMDQ_EnableIntr(NULL, 0x01, bEnable, DIP_WINDOW);
    MDrv_XC_DIVP_MDWIN_EnableIntr(NULL, bEnable, DIP_WINDOW);
    return 0;
}
EXPORT_SYMBOL(MHAL_DIVP_EnableFrameDoneIsr);

//------------------------------------------------------------------------------
/// @brief reset frame done interrupt..
/// @return MHAL_ErrCode_e
//------------------------------------------------------------------------------
MS_S32 MHAL_DIVP_CleanFrameDoneIsr(void)
{
    MDrv_XC_DIVP_ClearIntr(NULL, 0x01, DIP_WINDOW);
    //MDrv_XC_DIVP_CMDQ_ClearIntr(NULL, 0x01, DIP_WINDOW);
    MDrv_XC_DIVP_MDWIN_ClearIntr(NULL, DIP_WINDOW);
    return 0;
}
EXPORT_SYMBOL(MHAL_DIVP_CleanFrameDoneIsr);

//------------------------------------------------------------------------------
/// @brief wait cmdq frame done..
/// @return MHAL_ErrCode_e
//------------------------------------------------------------------------------
MS_S32 MHAL_DIVP_WaitCmdQFrameDone(MHAL_CMDQ_CmdqInterface_t* pstCmdQInfo, MHAL_CMDQ_EventId_e eEventId)
{
    if((eEventId == E_MHAL_CMDQEVE_DIP_TRIG) || (eEventId == E_MHAL_CMDQEVE_S1_MDW_W_DONE))
    {
        pstCmdQInfo->MHAL_CMDQ_CmdqAddWaitEventCmd(pstCmdQInfo, eEventId);
    }
    else
    {
        FUNC_ERR("Unkonwn CmdQ event ID!!!!\n");
    }

    MDrv_XC_DIVP_CMDQ_ClearIntr(pstCmdQInfo, 0x01, DIP_WINDOW);

    return 0;
}
EXPORT_SYMBOL(MHAL_DIVP_WaitCmdQFrameDone);

//------------------------------------------------------------------------------
/// @brief get interrupt ID.
/// @param[out] pu32IrqNum: interrupt ID,
/// @return MHAL_ErrCode_e
//------------------------------------------------------------------------------
MS_S32 MHAL_DIVP_GetIrqNum(MS_U32 *pu32IrqNum)
{
    *pu32IrqNum = 221;  // for E_INT_IRQ_DIPW
    return 0;
}
EXPORT_SYMBOL(MHAL_DIVP_GetIrqNum);
