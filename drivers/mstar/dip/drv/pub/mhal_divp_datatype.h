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

#ifndef _MHAL_DIVP_DATATYPE_H_
#define _MHAL_DIVP_DATATYPE_H_

#include "MsTypes.h"
#include "mhal_cmdq.h"

typedef MS_S32 (*PfnAlloc)(MS_U8 *pu8MMAHeapName, MS_U32 u32Size, MS_U64 *pu64Addr);
typedef MS_S32 (*PfnFree)(MS_U64 u64Addr);

typedef enum
{
    E_MHAL_DIVP_Device0 = 0,         ///< DIP window
    E_MHAL_DIVP_Device1 = 1,
    E_MHAL_DIVP_Device2 = 2,
    E_MHAL_DIVP_Device_MAX          /// The max support window
} MHAL_DIVP_DeviceId_e;

typedef enum
{
    E_MHAL_DIVP_ATTR_TNR = 0,
    E_MHAL_DIVP_ATTR_DI,
    E_MHAL_DIVP_ATTR_ROTATE,
    E_MHAL_DIVP_ATTR_CROP,
    E_MHAL_DIVP_ATTR_MIRROR,
} MHAL_DIVP_AttrType_e;

typedef enum
{
    E_MHAL_DIVP_Display0 = 0,         ///< DIP window
    E_MHAL_DIVP_Display1 = 1,
    E_MHAL_DIVP_Display_MAX          /// The max support window
} MHAL_DIVP_DisplayId_e;

typedef enum
{
    E_MHAL_DIVP_CAP_STAGE_INPUT = 0,
    E_MHAL_DIVP_CAP_STAGE_OUTPUT = 1,
    E_MHAL_DIVP_CAP_STAGE_OUTPUT_WITH_OSD = 2,
    E_MHAL_DIVP_CAP_STAGE_MAX = 3,
} MHAL_DIVP_CapStage_e;

typedef enum
{
    E_MHAL_DIVP_TILE_MODE_NONE     = 0x00,
    E_MHAL_DIVP_TILE_MODE_16x16    = 0x01,
    E_MHAL_DIVP_TILE_MODE_16x32    = 0x02,
    E_MHAL_DIVP_TILE_MODE_32x16    = 0x03,
    E_MHAL_DIVP_TILE_MODE_32x32    = 0x04,
    E_MHAL_DIVP_TILE_MODE_MAX,
} MHAL_DIVP_TileMode_e;

typedef enum
{
    E_MHAL_DIVP_PIXEL_FORMAT_YUV422_YUYV = 0,//(0)
    E_MHAL_DIVP_PIXEL_FORMAT_ARGB8888,//(1)
    E_MHAL_DIVP_PIXEL_FORMAT_ABGR8888,//(2)
    E_MHAL_DIVP_PIXEL_FORMAT_RGB565,//(3)
    E_MHAL_DIVP_PIXEL_FORMAT_ARGB1555,//(4)
    E_MHAL_DIVP_PIXEL_FORMAT_I2,//(5)
    E_MHAL_DIVP_PIXEL_FORMAT_I4,//(6)
    E_MHAL_DIVP_PIXEL_FORMAT_I8,//(7)
    E_MHAL_DIVP_PIXEL_FORMAT_YUV_SEMIPLANAR_422,//(8)
    E_MHAL_DIVP_PIXEL_FORMAT_YUV_SEMIPLANAR_420,//(9)
    E_MHAL_DIVP_PIXEL_FORMAT_YUV_MST_420,//(10)

    //vdec mstar private video format
    E_MHAL_DIVP_PIXEL_FORMAT_YC420_MSTTILE1_H264,//(11)
    E_MHAL_DIVP_PIXEL_FORMAT_YC420_MSTTILE2_H265,//(12)
    E_MHAL_DIVP_PIXEL_FORMAT_YC420_MSTTILE3_H265,//(13)
    E_MHAL_DIVP_PIXEL_FORMAT_MAX,
} MHAL_DIVP_PixelFormat_e;

typedef enum
{
    E_MHAL_DIVP_ROTATE_NONE = 0, //Rotate 0 degrees
    E_MHAL_DIVP_ROTATE_90, //Rotate 90 degrees
    E_MHAL_DIVP_ROTATE_180, //Rotate 180 degrees
    E_MHAL_DIVP_ROTATE_270, //Rotate 270 degrees
    E_MHAL_DIVP_ROTATE_NUM,
} MHAL_DIVP_Rotate_e;

typedef enum
{
    E_MHAL_DIVP_SCAN_MODE_PROGRESSIVE = 0x00,
    E_MHAL_DIVP_SCAN_MODE_INTERLACE_FRAME,
    E_MHAL_DIVP_SCAN_MODE_INTERLACE_FIELD,
    E_MHAL_DIVP_SCAN_MODE_MAX,
} MHAL_DIVP_ScanMode_e;

typedef enum
{
    E_MHAL_DIVP_FIELD_TYPE_NONE = 0,        //< no field.
    E_MHAL_DIVP_FIELD_TYPE_TOP,           //< Top field only.
    E_MHAL_DIVP_FIELD_TYPE_BOTTOM,    //< Bottom field only.
    E_MHAL_DIVP_FIELD_TYPE_BOTH,        //< Both fields.
    E_MHAL_DIVP_FIELD_TYPE_NUM
} MHAL_DIVP_FieldType_e;

typedef enum
{
    E_MHAL_DIVP_DB_MODE_H264_H265   = 0x00,
    E_MHAL_DIVP_DB_MODE_VP9         = 0x01,
    E_MHAL_DIVP_DB_MODE_MAX,
} MHAL_DIVP_DbMode_e; // Decoder Buffer Mode

typedef enum
{
    E_MHAL_DIVP_DI_TYPE_OFF = 0,//off
    E_MHAL_DIVP_DI_TYPE_2D,///2.5D DI
    E_MHAL_DIVP_DI_TYPE_3D,///3D DI

    E_MHAL_DIVP_DI_TYPE_MED,///2.5D DI
    E_MHAL_DIVP_DI_TYPE_3DDI,///3D DI
    E_MHAL_DIVP_DI_TYPE_NUM,
} MHAL_DIVP_DiType_e;

typedef enum
{
    E_MHAL_DIVP_DI_MODE_BOB = 0,
    E_MHAL_DIVP_DI_MODE_EODI,///2.5D DI only
    E_MHAL_DIVP_DI_MODE_WAVE,///3D DI only
    E_MHAL_DIVP_DI_MODE_MAX,
} MHAL_DIVP_DiMode_e;

typedef enum
{
    E_MHAL_DIVP_TNR_LEVEL_OFF = 0,
    E_MHAL_DIVP_TNR_LEVEL_LOW,
    E_MHAL_DIVP_TNR_LEVEL_MIDDLE,
    E_MHAL_DIVP_TNR_LEVEL_HIGH,
    E_MHAL_DIVP_TNR_LEVEL_NUM,
} MHAL_DIVP_TnrLevel_e;

typedef enum
{
    E_MHAL_DIVP_FIELD_ORDER_TYPE_BOTTOM = 0,
    E_MHAL_DIVP_FIELD_ORDER_TYPE_TOP,
    E_MHAL_DIVP_FIELD_ORDER_TYPE_MAX,
} MHAL_DIVP_FieldOrderType_e;

typedef struct
{
    MS_U16 u16X;
    MS_U16 u16Y;
    MS_U16 u16Width;
    MS_U16 u16Height;
} MHAL_DIVP_Window_t;

typedef struct
{
    MHAL_DIVP_DiMode_e eDiMode;///depend DI type, 2.5D DI(BOB or EODI), 3D DI(BOB or WAVE)
    MHAL_DIVP_FieldOrderType_e eFieldOrderType;//It will tell DIP process which field firstly.
    MHAL_DIVP_FieldType_e eFieldType;// = 0;//
} MHAL_DIVP_DiSettings_t;

typedef struct
{
    MS_BOOL bHMirror;
    MS_BOOL bVMirror;
} MHAL_DIVP_Mirror_t;

typedef struct
{
    MS_U64 u64BufAddr[3];//output buffer address
    MS_U32 u32BufSize;//output buffer size
    MS_U16 u16Width;//output frame width
    MS_U16 u16Height;//output frame height
    MS_U16 u16Stride[3];//pitch (unit: bytes)
    MHAL_DIVP_PixelFormat_e eInputPxlFmt;//input pixel format
    MHAL_DIVP_PixelFormat_e eOutputPxlFmt;//output frame pixel format
    MHAL_DIVP_DisplayId_e eDispId;//whitch display will be captured the screen
    MHAL_DIVP_CapStage_e eCapStage;//the postion captured the frame data
    MHAL_DIVP_Rotate_e eRotate;//rotate angles
    MHAL_DIVP_Mirror_t stMirror;//whether need mirror
    MHAL_DIVP_Window_t stCropWin;//crop size
} MHAL_DIVP_CaptureInfo_t;

typedef struct
{
    MS_U64 u64BufAddr[3];
    MS_U32 u32BufSize;
    MS_U16 u16OutputWidth;
    MS_U16 u16OutputHeight;
    MS_U16 u16Stride[3];//bytes
    MHAL_DIVP_PixelFormat_e ePxlFmt;
} MHAL_DIVP_OutPutInfo_t;

typedef struct
{
    MS_BOOL bDbEnable;// Decoder Buffer Enable
    MS_U8 u8DbSelect;// Decoder Buffer select
    MHAL_DIVP_Mirror_t stMirror;
    MS_BOOL bUncompressMode;
    MS_BOOL bBypassCodecMode;
    MHAL_DIVP_DbMode_e eDbMode;// Decoder Buffer mode
    MS_U16 u16StartX;
    MS_U16 u16StartY;
    MS_U16 u16Width;
    MS_U16 u16Height;
    MS_U64 u64DbBaseAddr;// Decoder Buffer base addr
    MS_U16 u16DbPitch;// Decoder Buffer pitch
    MS_U8 u8DbMiuSel;// Decoder Buffer Miu select
    MS_U64 u64LbAddr;// Lookaside buffer addr
    MS_U8 u8LbSize;// Lookaside buffer size
    MS_U8 u8LbTableId;// Lookaside buffer table Id
} MHAL_DIVP_MFdecInfo_t; // Decoder Buffer Info

typedef struct
{
    MS_U64 u64BufAddr[3];
    MS_U32 u32BufSize;
    MS_U16 u16InputWidth;
    MS_U16 u16InputHeight;
    MS_U16 u16Stride[3];
    MS_U64 u64Pts;
    MHAL_DIVP_PixelFormat_e ePxlFmt;
    MHAL_DIVP_TileMode_e eTileMode;
    MHAL_DIVP_ScanMode_e eScanMode;
    MHAL_DIVP_Window_t stCropWin;//crop size
    MHAL_DIVP_MFdecInfo_t stMfdecInfo;///MFdec info
    MHAL_DIVP_DiSettings_t stDiSettings;///DI settings
} MHAL_DIVP_InputInfo_t;

#endif
