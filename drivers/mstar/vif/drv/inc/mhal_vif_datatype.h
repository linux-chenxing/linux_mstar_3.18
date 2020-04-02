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
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
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
#ifndef _MHAL_VIF_DATATYPE_H_
#define _MHAL_VIF_DATATYPE_H_

//#include "../../include/common/mhal_common.h"
//#include <linux/types.h>

#define MHAL_VIF_MAX_DEV_NUM   4
#define MHAL_VIF_MAX_WAY_NUM_PER_DEV   4
#define MHAL_VIF_MAX_PHYCHN_NUM (MHAL_VIF_MAX_DEV_NUM*MHAL_VIF_MAX_WAY_NUM_PER_DEV)
#define MHAL_VIF_MAX_CHN_OUTPORT 2

#define MHAL_SUCCESS    (0)
#define MHAL_FAILURE    (-1)
#define E_HAL_VIF_SUCCESS MHAL_SUCCESS
#define E_HAL_VIF_ERROR MHAL_FAILURE

typedef signed int MHal_VIF_DEV;
typedef signed int MHal_VIF_CHN;
typedef signed int MHal_VIF_PORT;

typedef enum
{
    E_MHAL_VIF_MODE_BT656,
    E_MHAL_VIF_MODE_DIGITAL_CAMERA,
    E_MHAL_VIF_MODE_BT1120_STANDARD,
    E_MHAL_VIF_MODE_BT1120_INTERLEAVED,
    E_MHAL_VIF_MODE_MAX
} MHal_VIF_IntfMode_e;

typedef enum
{
    E_MHAL_VIF_WORK_MODE_1MULTIPLEX,
    E_MHAL_VIF_WORK_MODE_2MULTIPLEX,
    E_MHAL_VIF_WORK_MODE_4MULTIPLEX,
    E_MHAL_VIF_WORK_MODE_MAX
} MHal_VIF_WorkMode_e;

typedef enum
{
    E_MHAL_VIF_CLK_EDGE_SINGLE_UP,
    E_MHAL_VIF_CLK_EDGE_SINGLE_DOWN,
    E_MHAL_VIF_CLK_EDGE_DOUBLE,
    E_MHAL_VIF_CLK_EDGE_MAX
} MHal_VIF_ClkEdge_e;

typedef enum
{
    /*The input sequence of the second component(only contains u and v) in BT.1120 mode */
    E_MHAL_VIF_INPUT_DATA_VUVU = 0,
    E_MHAL_VIF_INPUT_DATA_UVUV,

    /* The input sequence for yuv */
    E_MHAL_VIF_INPUT_DATA_UYVY = 0,
    E_MHAL_VIF_INPUT_DATA_VYUY,
    E_MHAL_VIF_INPUT_DATA_YUYV,
    E_MHAL_VIF_INPUT_DATA_YVYU,
    E_MHAL_VIF_DATA_YUV_MAX
} MHal_VIF_DataYuvSeq_e;

typedef enum
{
    E_MHAL_VIF_VSYNC_FIELD,
    E_MHAL_VIF_VSYNC_PULSE
} MHal_VIF_Vsync_e;

typedef enum
{
    E_MHAL_VIF_VSYNC_NEG_HIGH,
    E_MHAL_VIF_VSYNC_NEG_LOW
} MHal_VIF_VsyncNeg_e;

typedef enum
{
    E_MHAL_VIF_HSYNC_VALID_SINGNAL,
    E_MHAL_VIF_HSYNC_PULSE
} MHal_VIF_Hsync_e;

typedef enum
{
    E_MHAL_VIF_HSYNC_NEG_HIGH,
    E_MHAL_VIF_HSYNC_NEG_LOW
} MHal_VIF_HsyncNeg_e;

typedef enum
{
    E_MHAL_VIF_VSYNC_NORM_PULSE,
    E_MHAL_VIF_VSYNC_VALID_SINGAL,
} MHal_VIF_VsyncValid_e;

typedef enum
{
    E_MHAL_VIF_VSYNC_VALID_NEG_HIGH,
    E_MHAL_VIF_VSYNC_VALID_NEG_LOW
} MHal_VIF_VsyncValidNeg_e;

typedef enum
{
    E_MHAL_VI_DATA_TYPE_YUV,
    E_MHAL_VI_DATA_TYPE_RGB,
    E_MHAL_VI_DATA_TYPE_MAX
} MHal_VIF_DataType_e;

typedef enum
{
    E_MHAL_VIF_CAPSEL_TOP,
    E_MHAL_VIF_CAPSEL_BOTTOM,
    E_MHAL_VIF_CAPSEL_BOTH,
    E_MHAL_VIF_CAPSEL_MAX
} MHal_VIF_Capsel_e;

typedef enum
{
    E_MHAL_VIF_SCAN_INTERLACED,
    E_MHAL_VIF_SCAN_PROGRESSIVE,
    E_MHAL_VIF_SCAN_MAX
} MHal_VIF_ScanMode_e;

typedef enum
{
    E_MHAL_VIF_PIXEL_FORMAT_RGB_1BPP,
    E_MHAL_VIF_PIXEL_FORMAT_RGB_2BPP,
    E_MHAL_VIF_PIXEL_FORMAT_RGB_4BPP,
    E_MHAL_VIF_PIXEL_FORMAT_RGB_8BPP,
    E_MHAL_VIF_PIXEL_FORMAT_RGB_444,

    E_MHAL_VIF_PIXEL_FORMAT_RGB_4444,
    E_MHAL_VIF_PIXEL_FORMAT_RGB_555,
    E_MHAL_VIF_PIXEL_FORMAT_RGB_565,
    E_MHAL_VIF_PIXEL_FORMAT_RGB_1555,

    /*  9 reserved */
    E_MHAL_VIF_PIXEL_FORMAT_RGB_888,
    E_MHAL_VIF_PIXEL_FORMAT_RGB_8888,

    E_MHAL_VIF_PIXEL_FORMAT_RGB_PLANAR_888,
    E_MHAL_VIF_PIXEL_FORMAT_RGB_BAYER_8BPP,
    E_MHAL_VIF_PIXEL_FORMAT_RGB_BAYER_10BPP,
    E_MHAL_VIF_PIXEL_FORMAT_RGB_BAYER_12BPP,
    E_MHAL_VIF_PIXEL_FORMAT_RGB_BAYER_14BPP,

    E_MHAL_VIF_PIXEL_FORMAT_RGB_BAYER,         /* 16 bpp */

    E_MHAL_VIF_PIXEL_FORMAT_YUV_A422,
    E_MHAL_VIF_PIXEL_FORMAT_YUV_A444,

    E_MHAL_VIF_PIXEL_FORMAT_YUV_PLANAR_422,
    E_MHAL_VIF_PIXEL_FORMAT_YUV_PLANAR_420,

    E_MHAL_VIF_PIXEL_FORMAT_YUV_PLANAR_444,

    E_MHAL_VIF_PIXEL_FORMAT_YUV_SEMIPLANAR_422,
    E_MHAL_VIF_PIXEL_FORMAT_YUV_SEMIPLANAR_420,
    E_MHAL_VIF_PIXEL_FORMAT_YUV_SEMIPLANAR_444,

    E_MHAL_VIF_PIXEL_FORMAT_UYVY_PACKAGE_422,
    E_MHAL_VIF_PIXEL_FORMAT_YUYV_PACKAGE_422,
    E_MHAL_VIF_PIXEL_FORMAT_VYUY_PACKAGE_422,
    E_MHAL_VIF_PIXEL_FORMAT_YCbCr_PLANAR,

    E_MHAL_VIF_PIXEL_FORMAT_YUV_400,

    E_MHAL_VIF_PIXEL_FORMAT_MAX
} MHal_VIF_PixelFormat_e;

typedef enum
{
    E_MHAL_VIF_FRAMERATE_FULL = 0,
    E_MHAL_VIF_FRAMERATE_HALF,
    E_MHAL_VIF_FRAMERATE_QUARTER,
    E_MHAL_VIF_FRAMERATE_OCTANT,
    E_MHAL_VIF_FRAMERATE_THREE_QUARTERS,
    E_MHAL_VIF_FRAMERATE_MANUAL,   //Manual bitmask making
    E_MHAL_VIF_FRAMERATE_MAX
} MHal_VIF_FrameRate_e;

typedef struct MHal_VIF_TimingBlank_s
{
	u32 u32HsyncHfb;
	u32 u32HsyncAct;
	u32 u32HsyncHbb;
	u32 u32VsyncVfb;
    u32 u32VsyncVact;
    u32 u32VsyncVbb;
    u32 u32VsyncVbfb;
    u32 u32VsyncVbact;
    u32 u32VsyncVbbb;
} MHal_VIF_TimingBlank_t;

typedef struct MHal_VIF_SyncCfg_s
{
    MHal_VIF_Vsync_e         eVsync;
    MHal_VIF_VsyncNeg_e      eVsyncNeg;
    MHal_VIF_Hsync_e         eHsync;
    MHal_VIF_HsyncNeg_e      eHsyncNeg;
    MHal_VIF_VsyncValid_e    eVsyncValid;
    MHal_VIF_VsyncValidNeg_e eVsyncValidNeg;
    MHal_VIF_TimingBlank_t   stTimingBlank;
} MHal_VIF_SyncCfg_t;

typedef struct MHal_VIF_DevCfg_s
{
    MHal_VIF_IntfMode_e       eIntfMode;
    MHal_VIF_WorkMode_e       eWorkMode;
    u32                  au32CompMask[2];
    MHal_VIF_ClkEdge_e        eClkEdge;
    int                    as32AdChnId[4];
    MHal_VIF_DataYuvSeq_e     eDataSeq;
    MHal_VIF_SyncCfg_t        stSynCfg;
    //MHal_VIF_DataType_e       eInputDataType;
    u8                   bDataRev;
} MHal_VIF_DevCfg_t;

typedef struct MHal_VIF_Rect_s
{
	u32 u32X;
	u32 u32Y;
	u32 u32Width;
	u32 u32Height;
} MHal_VIF_Rect_t;

typedef struct MHal_VIF_ChnCfg_s
{
    MHal_VIF_Rect_t          stCapRect;
    MHal_VIF_Capsel_e        eCapSel;
    MHal_VIF_ScanMode_e      eScanMode;
    MHal_VIF_PixelFormat_e   ePixFormat;
    MHal_VIF_FrameRate_e     eFrameRate;
} MHal_VIF_ChnCfg_t;

typedef struct MHal_VIF_SubChnCfg_s
{
    //MHal_VIF_Rect_t             stDestSize;
    MHal_VIF_FrameRate_e                eFrameRate;
} MHal_VIF_SubChnCfg_t;

typedef struct MHal_VIF_ChnStat_s
{
    //u8 bEnable;
    //u32  u32IntCnt;
    //u32  u32FrmRate;
    //u32  u32LostInt;
	u32  u32VbFail;
    //u32  u32PicWidth;
    //u32  u32PicHeight;
} MHal_VIF_ChnStat_t;

/*! @brief ring buffer element*/
typedef struct MHal_VIF_RingBufElm_s// stride ,   yuv 420   422  ....
{
	u64 u64PhyAddr[2]; /**< physical buffer address*/
	u32 u32Stride[2];

	u16 nCropX;      /**< crop start x*/
	u16 nCropY;      /**< crop start y*/
	u16 nCropW;      /**< crop start width*/
	u16 nCropH;      /**< crop start height*/
	u32 nPTS;        /**< timestamp in 90KHz*/
#define VIF_BUF_EMPTY   0 /**< Vif buffer is ready to write*/
#define VIF_BUF_FILLING 1 /**< DMA occupied */
#define VIF_BUF_READY   2 /**< Image ready */
#define VIF_BUF_INVALID 3 /**< Do not use */
	u32 nStatus;      /**< Vif buffer status*/
	u32 nMiPriv;
} __attribute__((packed, aligned(4))) MHal_VIF_RingBufElm_t;

#endif // _MHAL_VIF_DATATYPE_H_
