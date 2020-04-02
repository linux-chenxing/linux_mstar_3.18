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
///////////////////////////////////////////////////////////////////////////////
///
/// @file   apiXC_DWIN.h
/// @brief  MStar XC_DWIN Driver Interface
/// @author MStar Semiconductor Inc.
/// @version 1.0
///////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------
/*! \defgroup Video Video modules

	* \defgroup XC_DWIN XC_DWIN interface
	* \ingroup Video

	DIP is a video capture function ,the whole function block separate four part,source select, clip window, HV scaling down , output window.\n

	<b> Fucntion block </b> \n
	\image html apiXC_DWIN_pic1.png

	<b> Source select </b> \n
	\image html apiXC_DWIN_pic3.png

	<b> Clip window , HV scaling down , output window</b> \n
	\image html apiXC_DWIN_pic4.png

	<b> API call flow  </b> \n
	\image html apiXC_DWIN_pic2.png

	* \defgroup DIP_INIT DIP init related functions
	* \ingroup   XC_DWIN

	* \defgroup DIP_RESOURCE DIP resource related functions
	* \ingroup   XC_DWIN

	* \defgroup DIP_SET_WINDOW DIP set window related functions
	* \ingroup   XC_DWIN

	* \defgroup DIP_SET_PROPERTY DIP set properity related functions
	* \ingroup   XC_DWIN

	* \defgroup DIP_SET_COLOR_TRANSFORM  DIP set color transform properity functions
	* \ingroup   XC_DWIN

	* \defgroup DIP_SET_WINPROPERTY DIP set window properity related functions
	* \ingroup   XC_DWIN

	* \defgroup DIP_INTERRUPT DIP interrupt related functions
	* \ingroup   XC_DWIN

	* \defgroup DIP_TRIGGER DIP trigger related functions
	* \ingroup   XC_DWIN

	* \defgroup DIP_GET_BUFINFO DIP get info related functions
	* \ingroup   XC_DWIN

	* \defgroup DIP_POWER_STATE DIP suspend and resume related functions
	* \ingroup   XC_DWIN

	* \defgroup DIP_ToBeRemove DIP api to be removed
	* \ingroup   XC_DWIN

  */
//-------------------------------------------------




#ifndef APIXC_DWIN_H
#define APIXC_DWIN_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "MsCommon.h"
//#include "mvideo_context.h"
//==============================================================================

//#include "ULog.h"

// Debug Logs, level form low(INFO) to high(FATAL, always show)
// Function information, ex function entry
//#define XC_INFO(x, args...) ULOGI("DIP API", x, ##args)
// Warning, illegal paramter but can be self fixed in functions
//#define XC_WARN(x, args...) ULOGW("DIP API", x, ##args)
//  Need debug, illegal paramter.
//#define XC_DBUG(x, args...) ULOGD("DIP API", x, ##args)
// Error, function will be terminated but system not crash
//#define XC_ERR(x, args...) ULOGE("DIP API", x, ##args)
// Critical, system crash. (ex. assert)
//#define XC_FATAL(x, args...) ULOGF("DIP API", x, ##args)

//==============================================================================
#define MAX_BUFF_CNT 8UL
#define AUTO_BUFF_CNT 0UL
/// Define window type for DIP
typedef enum
{
    DIP_WINDOW = 0,         ///< DIP window
    DWIN0_WINDOW = 1,
    DWIN1_WINDOW = 2,
    MAX_DIP_WINDOW          /// The max support window
}SCALER_DIP_WIN;

typedef enum
{
    E_XC_DIP_RGB_SWAPTO_BGR = 0,         ///< RGB Swap to BGR
    E_XC_DIP_RGB_SWAP_MAX                ///< The max support format
}EN_XC_DIP_RGB_SWAP_TYPE;

/// DIP source data format
typedef enum
{
    /// YCrYCb.
    DIP_DATA_FMT_YUV422 = 0,
    /// RGB domain
    DIP_DATA_FMT_RGB565,
    /// RGB domain
    DIP_DATA_FMT_ARGB8888,
    /// YUV420 HVD tile
    DIP_DATA_FMT_YUV420,
    /// YC separate 422
    DIP_DATA_FMT_YC422,
    /// YUV420 H265 tile
    DIP_DATA_FMT_YUV420_H265,
    /// YUV420 H265_10bits tile
    DIP_DATA_FMT_YUV420_H265_10BITS,
    /// YUV420 planer
    DIP_DATA_FMT_YUV420_PLANER,
    /// YUV420 semi planer
    DIP_DATA_FMT_YUV420_SEMI_PLANER,
    /// YUV422 2.5D DI Top field
    DIP_DATA_FMT_YUV422_MED_DI_FIELDTYPE_TOP,
    /// YUV422 2.5D DI Bottom field
    DIP_DATA_FMT_YUV422_MED_DI_FIELDTYPE_BOTTOM,
    /// YUV420 2.5D DI Top field
    DIP_DATA_FMT_YUV420_MED_DI_FIELDTYPE_TOP,
    /// YUV420 2.5D DI Bottom field
    DIP_DATA_FMT_YUV420_MED_DI_FIELDTYPE_BOTTOM,
    DIP_DATA_FMT_MAX
} EN_XC_DIP_DATA_FMT;

/// DIP tile block
typedef enum
{
    DIP_TILE_BLOCK_R_NONE  = 0x0,
    DIP_TILE_BLOCK_W_NONE  = 0x1,
    DIP_TILE_BLOCK_R_16_32 = 0x2,
    DIP_TILE_BLOCK_W_16_32 = 0x3,
    DIP_TILE_BLOCK_R_32_16 = 0x4,
    DIP_TILE_BLOCK_W_32_16 = 0x5,
    DIP_TILE_BLOCK_R_32_32 = 0x6,
    DIP_TILE_BLOCK_W_32_32 = 0x7,
    DIP_TILE_BLOCK_R_10Bit_LSB_32_16  = 0x8,
    DIP_TILE_BLOCK_R_10Bit_LSB_128_64 = 0x9,
}EN_XC_DIP_TILE_BLOCK;
/// DIP scan mode
typedef enum
{
    /// progessive.
    DIP_SCAN_MODE_PROGRESSIVE,
    /// interlace.
    DIP_SCAN_MODE_extern,
} EN_XC_DIP_SCAN_MODE;

typedef enum
{
    DIP_CAP_NOT_EXIST = 0x0,
    DIP_CAP_EXIST =     0x1,
    DIP_CAP_420TILE =   0x2,
    DIP_CAP_SCALING_DOWN =   0x4,
    DIP_CAP_MIRROR =    0x8,
    DIP_CAP_DIPR    =   0x10,
    DIP_CAP_R2Y     =   0x20,
    DIP_CAP_OP1_CAPTURE    = 0x40,
    //not support OP1 capture when traveling main source with PIP mode
    DIP_CAP_OP1_CAPTURE_V1 = 0x40,
    //support OP1 capture in all case
    DIP_CAP_OP1_CAPTURE_V2 = 0x80,
    DIP_CAP_SCALING_UP = 0x100,
    DIP_CAP_OSD_BLENDING = 0x200,
    DIP_CAP_OC_CAPTURE = 0x400,
    DIP_CAP_SC2_NON_SCALING = 0x800,
}EN_XC_DIP_CAP;

typedef enum
{
    E_XC_DIP_VOP2   =0x00,
    E_XC_DIP_OP2    =0x01,
    E_XC_DIP_VIP    =0x02,
    E_XC_DIP_BRI    =0x03,
    E_XC_DIP_NONE   ,
}EN_XC_DIP_OP_CAPTURE;

typedef enum
{
    E_XC_DIP_ROTATION_270   =0x00,
    E_XC_DIP_ROTATION_90    =0x01,
}EN_XC_DIP_ROTATION;

// Define DEBUG level.
typedef enum
{
    // DIP DEBUG LEVEL LOW. Just printf error message.
    E_XC_DIP_Debug_Level_LOW   = 0,
    // GOP DEBUG LEVEL Medium. printf warning message and error message.
    E_XC_DIP_Debug_Level_MED   = 1,
    // GOP DEBUG LEVEL hIGH. printf all message with function.
    E_XC_DIP_Debug_Level_HIGH   = 2,
} EN_XC_DIP_DEBUG_LEVEL;

typedef enum
{
    E_XC_DIP_CMDQ_BEGIN = 0,
    E_XC_DIP_CMDQ_END = 1,
}EN_XC_DIP_CMDQ_ACTION;

typedef enum
{
    E_XC_DIP_IMI_ENABLE = 0x0,
}
EN_DIP_CONFIG_TYPE;

typedef struct
{
    MS_U8               u8BufCnt;
    u32              u32YBuf[MAX_BUFF_CNT];
    u32              u32CBuf[MAX_BUFF_CNT];
} BUFFER_INFO;

typedef struct
{
    SCALER_DIP_WIN      eWindow;
    MS_U32              u32DipChipCaps;
} ST_XC_DIP_CHIPCAPS;

typedef struct
{
    MS_U8  u8BufCnt;
    MS_U16 u16Width;
    MS_U16 u16Height;
    MS_U16 u16Pitch;
    u32 u32BufStart;
    u32 u32BufEnd;
    SCALER_DIP_SOURCE_TYPE enSource;
}ST_XC_DIP_WINPROPERTY;

typedef struct
{
    MS_U16 u16Width;
    MS_U16 u16Height;
    MS_U16 u16Pitch;
    u32 u32YBufAddr;
    u32 u32CBufAddr;
    EN_XC_DIP_DATA_FMT enDataFmt;
    u32 u32YBufAddr10Bits;
    u32 u32CBufAddr10Bits;
}ST_XC_DIPR_PROPERTY;

typedef struct
{
    MS_U32 ID[4];
} DIPRESOURCE;

// Define DIP version control
typedef struct
{
    MS_U32 u32version;
    MS_U32 u32size;
} ST_DIPR_VER_CRL;

typedef enum
{
    E_DIP_MFEDC_TILE_16_32     =0x00,
    E_DIP_MFEDC_TILE_32_16     =0x01,
    E_DIP_MFEDC_TILE_32_32     =0x02,
}EN_MFDEC_TILE_MODE;

typedef enum
{
    E_XC_DIP_H26X_MODE     =0x00,
    E_XC_DIP_VP9_MODE      =0x01,
}EN_MFDEC_VP9_MODE;

// Define DIP version control for MFDec
typedef struct
{
    MS_U32 u32version;
    MS_U32 u32size;
} ST_MFDEC_HTLB_VER_CRL;

typedef struct
{
    ST_MFDEC_HTLB_VER_CRL stMFDec_HTLB_VerCtl;
    u32 u32HTLBEntriesAddr;
    MS_U8  u8HTLBEntriesSize;
    MS_U8  u8HTLBTableId;
    void* pHTLBInfo;
} ST_DIP_MFDEC_HTLB_INFO;

// Define DIP version control
typedef struct
{
    MS_U32 u32version;
    MS_U32 u32size;
} ST_MFDEC_VER_CRL;

typedef struct
{
    ST_MFDEC_VER_CRL stMFDec_VerCtl;
    MS_BOOL bMFDec_Enable;
    MS_U8 u8MFDec_Select;
    MS_BOOL bHMirror;
    MS_BOOL bVMirror;
    EN_MFDEC_TILE_MODE enMFDec_tile_mode;
    MS_BOOL bUncompress_mode;
    MS_BOOL bBypass_codec_mode;
    EN_MFDEC_VP9_MODE en_MFDecVP9_mode;
    u32 phyLuma_FB_Base;
    u32 phyChroma_FB_Base;
    MS_U16 u16FB_Pitch;
    MS_U16 u16StartX;
    MS_U16 u16StartY;
    MS_U16 u16HSize;
    MS_U16 u16VSize;
    u32 phyBitlen_Base;
    MS_U16 u16Bitlen_Pitch;
    ST_DIP_MFDEC_HTLB_INFO stMFDec_HTLB_Info;
    void* pMFDecInfo;
} ST_DIP_MFDEC_INFO;

typedef enum
{
    //one top field in one address
    E_DIP_3DDI_TOP_SINGLE_FIELD        =0x00,
    //one bottom field in one address
    E_DIP_3DDI_BOTTOM_SINGLE_FIELD     =0x01,
    //stagger Both field in one address, keep top field
    E_DIP_3DDI_TOP_BOTH_FIELD_STAGGER        =0x02,
    //stagger Both field in one address, keep bottom field
    E_DIP_3DDI_BOTTOM_BOTH_FIELD_STAGGER     =0x03,
    //stagger Both field in one address, DIP 2.5D DI top field
    E_DIP_MED_DI_TOP_BOTH_FIELD_STAGGER       =0x04,
    //stagger Both field in one address, DIP 2.5D DI bottom field
    E_DIP_MED_DI_BOTTOM_BOTH_FIELD_STAGGER    =0x05,
    //two field in two address, DI top field
    E_DIP_MED_DI_TOP_BOTH_FIELD_SEPARATE       =0x06,
    //two field in two address, DI bottom field
    E_DIP_MED_DI_BOTTOM_BOTH_FIELD_SEPARATE    =0x07,
}EN_DIP_DI_FIELD;

typedef struct
{
    u32 phyDIInitAddr;
    u32 u32DIInitSize;
}ST_DIPR_DI_INIT_INFO;

typedef struct
{
    u32 phyDI_BufAddr; //DI buf address
    u32 u32DI_BufSize; //DI buf size
    u8 u8DIBufCnt;
}ST_DIP_DI_NWAY_BUF_INFO;

typedef struct
{
    bool bEnableDI;
    bool bInitDI;
    ST_DIPR_DI_INIT_INFO stInitInfo;
    u8 u8NwayIndex;
    EN_DIP_DI_FIELD enDI_Field;
    u32 phyBotYBufAddr;
    u32 phyBotCBufAddr;
    u32 phyBotYBufAddr10Bits;
    u32 phyBotCBufAddr10Bits;
    bool bEnableBob;
    bool bEnaNwayInfoRefresh;
    ST_DIP_DI_NWAY_BUF_INFO *stDINwayBufInfo;
    u8 u8NwayBufCnt;
} ST_DIP_DI_SETTING;

typedef enum
{
    E_DIP_DI_FIELD_NONE             =0x00,
    E_DIP_DI_FIELD_TOP_FIRST        =0x01,
    E_DIP_DI_FIELD_BOTTOM_FIRST     =0x02,
}EN_DIP_DI_FIRST_FIELD;

typedef struct
{
    ST_DIP_DI_SETTING stDIP_DI_setting;
    EN_DIP_DI_FIRST_FIELD enDIFirstField;
} ST_DIP_DI_SETTING_V2;

// Define DIP version control
typedef struct
{
    u32 u32version;
    u32 u32size;
} ST_DIPR_3DDI_VER_CRL;

typedef struct
{
    ST_DIPR_3DDI_VER_CRL stDipr3DDI_VerCtl;
    void* pDIPR3DDISetting;
} ST_DIP_DIPR_3DDI_SETTING;

typedef struct
{
    ST_DIPR_VER_CRL stDIPR_VerCtl;
    ST_XC_DIPR_PROPERTY stDIPRProperty;
    ST_DIP_MFDEC_INFO stDIPR_MFDecInfo;
    ST_DIP_DIPR_3DDI_SETTING stDIPR3DDISetting;
    void* pDIPRSetting;
}ST_XC_DIPR_PROPERTY_EX;


#ifdef __cplusplus
}
#endif
#endif /* APIXC_DWIN_H */