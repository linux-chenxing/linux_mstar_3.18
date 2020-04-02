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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   apiXC.h
/// @brief  XC Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! \defgroup Video Video modules

	XC is used for \n
	1. video display \n
	2. 3D support  \n
	3. mirror mode spoort \n
	4. frame rate convert and output timing control \n
	5. OSDC timing control \n
	For more information,see \link XC_CORE XC_CORE modules \endlink \n

	There are three parts as follow,\n
	ACE is used for	\n
	1. control the picture quality,include brightness, contrast, CSC ,etc.\n
	2. MWE function.\n
	DBC is used for\n
	1. Dynamic Backlight Curve : Calculate the DBC base on average value.\n
	DLC is used for\n
	1. Dynamic Luma Curve : Calculate the DLC curve base on histogram and average value.\n
	For more information,see \link XC_BE XC_BE modules   \endlink \n

	ADC is used to control ADC relative setting for YPbPr, VGA, SCART, AV, SV, ATV.\n
	For more information,see \link XC_SRC XC_SRC modules  \endlink \n

	There are three parts as follow,\n
	Auto is used to implement auto adjust and auto calibration.\n
	Mode Parse mechanism is a fast and easy way to look up input timing according to different timing characteristics.\n
	PC monitor is used to detect signal information.\n
	For more information,see \link XC_FE XC_FE modules \endlink \n

	DIP is a video capture function ,the whole function block separate four part,source select, clip window, HV scaling down , output window.\n
	For more information,see \link XC_DWIN XC_DWIN modules \endlink

 *  \defgroup XC_CORE XC_CORE modules
 *  \ingroup Video

    XC is used for \n
	1. video display \n
	2. 3D support  \n
	3. mirror mode spoort \n
	4. frame rate convert and output timing control \n
	5. OSDC timing control \n
	For more information,see \link XC XC interface (apiXC.h) \endlink

 *  \defgroup XC XC interface (apiXC.h)
 *  \ingroup XC_CORE

    XC is used for:

    - 1. video display
    - support input source as VGA, ATV, CVBS, S-video, YPbPr, SCART, HDMI, DTV, MM
    - support output as main window, sub window, CVBS out
    - suoport crop, scaling video to any size and position for display

    - 2. 3D support
    - support 3D input format as frame packing, field alternative, line alternative, side by side, top bottom, checker board, frame alternative
    - support 3D output format as line alternative, top bottom, side by side, frame alternative, checker board, pixel alternative, only L frame, only R frame
    - support normal 2D convert to 3D

    - 3. mirror mode spoort
    - support H mirror, V mirror, and H+V mirror

    - 4. frame rate convert and output timing control

    - 5. OSDC timing control
    - support for video and OSD seperate transfer with VB1

    <b> Operation Code Flow: </b> \n
    check flow chart directly.
    \image html apiXC_code_follow.png

 *  \defgroup XC_INIT XC init control
 *  \ingroup XC

 *! \defgroup XC_INFO XC Infomation pool
 *  \ingroup XC

 *! \defgroup XC_TIMING_DETECT XC Timing detect
 *  \ingroup XC

 *! \defgroup XC_MUX XC mux and path setting
 *  \ingroup XC

 *! \defgroup XC_SCALER XC scaler function
 *  \ingroup XC

 *! \defgroup XC_FRC XC embedded FRC function
 *  \ingroup XC

 *! \defgroup XC_3D XC 3D function
 *  \ingroup XC

 *! \defgroup XC_DS XC DS function
 *  \ingroup XC

 *! \defgroup XC_PQ XC PQ related function
 *  \ingroup XC

 *! \defgroup XC_LD XC Local Dimming function
 *  \ingroup XC

 *! \defgroup XC_OSD XC OSD related function
 *  \ingroup XC

 *! \defgroup XC_VE XC VE function
 *  \ingroup XC

 *! \defgroup XC_DISPLAY XC display function
 *  \ingroup XC

 *! \defgroup XC_GAMMA XC gamma function
 *  \ingroup XC

 *! \defgroup XC_TEST_PATTERN XC test pattern
 *  \ingroup XC

 *! \defgroup XC_UTILITY XC utility function
 *  \ingroup XC

 *! \defgroup XC_ToBeModified XC api to be modified
 *  \ingroup XC

 *! \defgroup XC_ToBeRemove XC api to be removed
 *  \ingroup XC
 */

#ifndef _API_XC_H_
#define _API_XC_H_

//#include "MsDevice.h"
// Driver
//#include "drvXC_IOPort.h"
//#include "UFO.h"
//? SYMBOL_WEAK defined here , but cause SN compile error
//#include "MsCommon.h"
//#ifdef __cplusplus
//extern "C"
//{
//#endif
#ifdef MVIDEO_C
    #define INTERFACE
#else
    #define INTERFACE extern
#endif
//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  This macro defined in mscommon.h originally, here just for avoid SN compile error
//-------------------------------------------------------------------------------------------------
#ifdef MSOS_TYPE_LINUX_KERNEL
    #define SYMBOL_WEAK
#else
    #define SYMBOL_WEAK __attribute__((weak))
#endif
//  library information
#define MSIF_XC_LIB_CODE               {'X','C','_','_'}
#define MSIF_XC_LIBVER                 {'0','9'}
#define MSIF_XC_BUILDNUM               {'1','0'}
#define MSIF_XC_CHANGELIST             {'0','0','7','0','9','8','0','1'}
#define XC_API_VERSION                  /* Character String for DRV/API version             */  \
    MSIF_TAG,                           /* 'MSIF'                                           */  \
    MSIF_CLASS,                         /* '00'                                             */  \
    MSIF_CUS,                           /* 0x0000                                           */  \
    MSIF_MOD,                           /* 0x0000                                           */  \
    MSIF_CHIP,                                                                                  \
    MSIF_CPU,                                                                                   \
    MSIF_XC_LIB_CODE  ,                 /* IP__                                             */  \
    MSIF_XC_LIBVER    ,                 /* 0.0 ~ Z.Z                                        */  \
    MSIF_XC_BUILDNUM  ,                 /* 00 ~ 99                                          */  \
    MSIF_XC_CHANGELIST,                 /* CL#                                              */  \
    MSIF_OS

#define XC_INITDATA_VERSION                            1
#define XC_OUTPUT_TIMING_ADJUST_VERSION                1
/// PANEL_INFO_EX version of current XC lib
#define PANEL_INFO_EX_VERSION                          2
/// hw 2d to 3d para version
#define HW2DTO3DPARA_VERSION                           2
/// 3d detect format para version
#define DETECT3DFORMATPARA_VERSION                     1

/// ApiStatusEX version of current XC lib
/// Define below to distinguish from API_PNLSTATUS_EX_VERSION
#define API_XCSTATUS_EX_VERSION                        8
#define API_STATUS_EX_VERSION                          API_XCSTATUS_EX_VERSION

/// 3d fp info version of current XC lib
#define XC_3D_FPINFOPARA_VERSION                       1
/// report pixel info version of current XC lib
#define XC_REPORT_PIXELINFO_VERSION                    1

#define API_XCDS_INFO_VERSION                          3
#define API_DS_INFO_VERSION                            API_XCDS_INFO_VERSION

/// osdc disp info version of current XC lib
#define ODSC_DISPINFO_VERSIN                          1

#define XC_INVALID_HEAP_ID                            0xFFFFFFFF

#define MAX_FRC_TABLE_INDEX                           10

#define _BIT(_bit_)                  (1 << (_bit_))

//----------------------------
// Return value
//----------------------------

/// XC Return Value
typedef enum
{
    E_APIXC_RET_FAIL = 0,                   ///<Fail
    E_APIXC_RET_OK,                         ///<OK
    E_APIXC_RET_FAIL_FUN_NOT_SUPPORT,       ///<Fail
    E_APIXC_RET_FAIL_ENUM_NOT_SUPPORT,      ///<Fail
    E_APIXC_RET_FAIL_INVALID_PARAMETER,     ///<Fail
} E_APIXC_ReturnValue;

//----------------------------
// Debug Switch
//----------------------------
#define XC_DBGLEVEL_OFF             (0x0000)   ///< Turn off
#define XC_DBGLEVEL_SETWINDOW       (0x0001)   ///< Set Window
#define XC_DBGLEVEL_SETTIMING       (0x0002)   ///< LPLL  / SetPanelTiming
#define XC_DBGLEVEL_SYNCDETECT      (0x0004)   ///< Sync detection        ======>Not using it by now
#define XC_DBGLEVEL_MUX             (0x0008)   ///< Mux / Path Creation
#define XC_DBGLEVEL_MODEPARSE       (0x0010)   ///< Mode Parse            ======>Not using it by now
#define XC_DBGLEVEL_HDMIMONITOR     (0x0020)   ///< HDMI and Package Monitor ===>Not using it by now
#define XC_DBGLEVEL_AUTOGEOMETRY    (0x0040)   ///< Auto Geometry   / Auto Calibration
#define XC_DGBLEVEL_CROPCALC        (0x0080)   ///< Crop Window / Memory / OPM / IPM calculation

#define XC_DBGLEVEL_SETTIMING_ISR   (0x0100)   ///< ISR / SetPanelTiming
#define XC_DBGLEVEL_DWIN_SETWINDOW  (0x0200)   ///< DWIN / Set Window
#define XC_DBGLEVEL_FUNCTION_TRACE  (0x0400)   ///< Trace XC function using status
#define XC_DBGLEVEL_PARAMETER_TRACE (0x0800)   ///< Trace XC function parameter

#define XC_DBGLEVEL_SEAMLESSZAP     (0x1000)   ///< Seamless Zapping
//----------------------------
// Input signal detect
//----------------------------

#define XC_MD_VSYNC_POR_BIT            _BIT(0) ///< VSnc polarity bit(0/1 = positive/negative)
#define XC_MD_HSYNC_POR_BIT            _BIT(1) ///< HSync polarity bit(0/1 = positive/negative)
#define XC_MD_HSYNC_LOSS_BIT           _BIT(2) ///< HSync loss bit
#define XC_MD_VSYNC_LOSS_BIT           _BIT(3) ///< VSync loss bit
#define XC_MD_INTERLACE_BIT            _BIT(4) ///< Interlace mode
#define XC_MD_USER_MODE_BIT            _BIT(7) ///< User new mode (Not found in mode table)
#define XC_MD_SYNC_LOSS                (XC_MD_HSYNC_LOSS_BIT | XC_MD_VSYNC_LOSS_BIT) ///< VSync & HSync loss bit

//----------------------------
// 3D macros
//----------------------------
#define DOUBLEHD_PRESCALE_VONEHDSIZE (540)

#define DOUBLEHD_1080X2I_FRAME_VSIZE (1080)
#define DOUBLEHD_1080X2I_FIELD_VSIZE (540)
#define DOUBLEHD_1080X2I_HSIZE (1920)
#define DOUBLEHD_1080X2I_VACT_SPACE1 (23)
#define DOUBLEHD_1080X2I_VACT_SPACE2 (22)
#define DOUBLEHD_1080X2I_VSIZE (DOUBLEHD_1080X2I_FRAME_VSIZE*2+DOUBLEHD_1080X2I_VACT_SPACE1*2+DOUBLEHD_1080X2I_VACT_SPACE2)

#define DOUBLEHD_1080X2P_VSIZE (2205)
#define DOUBLEHD_1080X2P_FRAME_VSIZE (1080)
#define DOUBLEHD_1080X2P_HSIZE (1920)
#define DOUBLEHD_1080X2P_GARBAGE_VSIZE (45)

#define DOUBLEHD_720X2P_VSIZE (1470)
#define DOUBLEHD_720X2P_FRAME_VSIZE (720)
#define DOUBLEHD_720X2P_HSIZE (1280)
#define DOUBLEHD_720X2P_GARBAGE_VSIZE (30)

#define DOUBLEHD_576X2P_VSIZE (1201)
#define DOUBLEHD_576X2P_FRAME_VSIZE (576)
#define DOUBLEHD_576X2P_HSIZE (720)
#define DOUBLEHD_576X2P_GARBAGE_VSIZE (49)

#define DOUBLEHD_576X2I_FRAME_VSIZE (576)
#define DOUBLEHD_576X2I_FIELD_VSIZE (288)
#define DOUBLEHD_576X2I_HSIZE (720)
#define DOUBLEHD_576X2I_VACT_SPACE1 (25)
#define DOUBLEHD_576X2I_VACT_SPACE2 (24)
#define DOUBLEHD_576X2I_VSIZE (DOUBLEHD_576X2I_FRAME_VSIZE*2+DOUBLEHD_576X2I_VACT_SPACE1*2+DOUBLEHD_576X2I_VACT_SPACE2)

//720X480p
#define DOUBLEHD_480X2P_VSIZE (1005)
#define DOUBLEHD_480X2P_FRAME_VSIZE (480)
#define DOUBLEHD_480X2P_HSIZE (720)
#define DOUBLEHD_480X2P_GARBAGE_VSIZE (45)

#define DOUBLEHD_480X2I_FRAME_VSIZE (480)
#define DOUBLEHD_480X2I_FIELD_VSIZE (240)
#define DOUBLEHD_480X2I_HSIZE (720)
#define DOUBLEHD_480X2I_VACT_SPACE1 (23)
#define DOUBLEHD_480X2I_VACT_SPACE2 (22)
#define DOUBLEHD_480X2I_VSIZE (DOUBLEHD_480X2I_FRAME_VSIZE*2+DOUBLEHD_480X2I_VACT_SPACE1*2+DOUBLEHD_480X2I_VACT_SPACE2)

//640X480p
#define DOUBLEHD_640_480X2P_VSIZE (1005)
#define DOUBLEHD_640_480X2P_FRAME_VSIZE (480)
#define DOUBLEHD_640_480X2P_HSIZE (640)
#define DOUBLEHD_640_480X2P_GARBAGE_VSIZE (45)

// Field Interleave
#define FIELD_INTERLEAVE_1080X2I_FRAME_VSIZE  (1080)
#define FIELD_INTERLEAVE_1080X2I_FIELD_VISZE  (540)
#define FIELD_INTERLEAVE_1080X2I_HISZE        (1920)
#define FIELD_INTERLEAVE_1080X2I_VACT_SPACE   (23)
#define FIELD_INTERLEAVE_1080X2I_VISZE        (FIELD_INTERLEAVE_1080X2I_FRAME_VSIZE+FIELD_INTERLEAVE_1080X2I_VACT_SPACE)


///< When disable panel_info_ex, this means disable all valid panel info
#define PANEL_INFO_EX_INVALID_ALL    0xFFFF
//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
INTERFACE void* pu32XC_Kernel_Inst;
//----------------------------
// Common
//----------------------------

/// Define mute type
typedef enum
{
    E_MAIN_WINDOW_MUTE = 0,         ///< main window mute
    E_SUB_WINDOW_MUTE = 1,          ///< sub window mute
    E_PANEL_MUTE = 2            ///< panel mute
} EN_MUTE_TYPE;

typedef enum
{
    E_XC_FB_LEVEL_FB,        //  frame buff mode
    E_XC_FB_LEVEL_FBL,        // same as fbl,not use miu
    E_XC_FB_LEVEL_RFBL_DI,    // use miu to deinterlace
    E_XC_FB_LEVEL_NUM,    // number
} E_XC_FB_LEVEL;

/// Define window type
typedef enum
{
    MAIN_WINDOW = 0,        ///< main window if with PIP or without PIP
    SUB_WINDOW = 1,         ///< sub window if PIP
    MAX_WINDOW              /// The max support window
} SCALER_WIN;

#define PIP_WINDOWS MAX_WINDOW

/// Define Window position and size attribute
typedef struct __attribute__((packed))
{
    MS_U16 x;           ///<start x of the window
    MS_U16 y;           ///<start y of the window
    MS_U16 width;       ///<width of the window
    MS_U16 height;      ///<height of the window
}
MS_WINDOW_TYPE;

typedef struct
{
    MS_WINDOW_TYPE  stCropWin;       ///<Crop window
    MS_WINDOW_TYPE  stDispWin;       ///<Display window
    MS_BOOL         bInterlace;
    //out value
    E_XC_FB_LEVEL   eFBLevel;        ///return for FB or FBL mode
    MS_U16          u16HSizeAfterPrescaling;
    MS_U16          u16VSizeAfterPrescaling;
    SCALER_WIN eWindow;
} XC_GET_FB_LEVEL;
////////////////////////////////////////////////////////////////////////////////
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// This is abstract input source for XC library. If you want to add another one.
// Please contace with XC member
////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    MS_U32 u32EnablePort;
    MS_U32 u32Port[2];
} XC_MUX_INPUTSRCTABLE;
/// Define Input type
typedef enum
{
    INPUT_SOURCE_VGA,           ///<0   VGA input
    INPUT_SOURCE_TV,            ///<1   TV input

    INPUT_SOURCE_CVBS,          ///<2   AV 1
    INPUT_SOURCE_CVBS2,         ///<3   AV 2
    INPUT_SOURCE_CVBS3,         ///<4   AV 3
    INPUT_SOURCE_CVBS4,         ///<5   AV 4
    INPUT_SOURCE_CVBS5,         ///<6   AV 5
    INPUT_SOURCE_CVBS6,         ///<7   AV 6
    INPUT_SOURCE_CVBS7,         ///<8   AV 7
    INPUT_SOURCE_CVBS8,         ///<9   AV 8
    INPUT_SOURCE_CVBS_MAX,      ///<10 AV max

    INPUT_SOURCE_SVIDEO,        ///<11 S-video 1
    INPUT_SOURCE_SVIDEO2,       ///<12 S-video 2
    INPUT_SOURCE_SVIDEO3,       ///<13 S-video 3
    INPUT_SOURCE_SVIDEO4,       ///<14 S-video 4
    INPUT_SOURCE_SVIDEO_MAX,    ///<15 S-video max

    INPUT_SOURCE_YPBPR,         ///<16 Component 1
    INPUT_SOURCE_YPBPR2,        ///<17 Component 2
    INPUT_SOURCE_YPBPR3,        ///<18 Component 3
    INPUT_SOURCE_YPBPR_MAX,     ///<19 Component max

    INPUT_SOURCE_SCART,         ///<20 Scart 1
    INPUT_SOURCE_SCART2,        ///<21 Scart 2
    INPUT_SOURCE_SCART_MAX,     ///<22 Scart max

    INPUT_SOURCE_HDMI,          ///<23 HDMI 1
    INPUT_SOURCE_HDMI2,         ///<24 HDMI 2
    INPUT_SOURCE_HDMI3,         ///<25 HDMI 3
    INPUT_SOURCE_HDMI4,         ///<26 HDMI 4
    INPUT_SOURCE_HDMI_MAX,      ///<27 HDMI max

    INPUT_SOURCE_DTV,           ///<28 DTV

    INPUT_SOURCE_DVI,           ///<29 DVI 1
    INPUT_SOURCE_DVI2,          ///<30 DVI 2
    INPUT_SOURCE_DVI3,          ///<31 DVI 2
    INPUT_SOURCE_DVI4,          ///<32 DVI 4
    INPUT_SOURCE_DVI_MAX,       ///<33 DVI max

    // Application source
    INPUT_SOURCE_STORAGE,       ///<34 Storage
    INPUT_SOURCE_KTV,           ///<35 KTV
    INPUT_SOURCE_JPEG,          ///<36 JPEG

    //Support Dual MVOP port
    INPUT_SOURCE_DTV2,          ///<37 DTV
    INPUT_SOURCE_STORAGE2,	    ///<38 Storage

    //Support Third MVOP port
    INPUT_SOURCE_DTV3,          ///<39 DTV

    // Support OP capture
    INPUT_SOURCE_SCALER_OP,     ///<40 scaler OP

    INPUT_SOURCE_VGA2,          /// <41 VGA2
    INPUT_SOURCE_VGA3,          /// <42 VGA3

    INPUT_SOURCE_NUM,           ///<43 number of the source
    INPUT_SOURCE_NONE = INPUT_SOURCE_NUM,    ///<NULL input
} INPUT_SOURCE_TYPE_t;

#define IsSrcTypeAV(x)             (((x)>=INPUT_SOURCE_CVBS)&&((x)<=INPUT_SOURCE_CVBS_MAX))/// x is AV
#define IsSrcTypeSV(x)             (((x)>=INPUT_SOURCE_SVIDEO)&&((x)<=INPUT_SOURCE_SVIDEO_MAX))///< x is SV
#define IsSrcTypeATV(x)            ((x)==INPUT_SOURCE_TV)///< x is ATV
#define IsSrcTypeScart(x)          (((x)>=INPUT_SOURCE_SCART)&&((x)<=INPUT_SOURCE_SCART_MAX))///< x is Scart
#define IsSrcTypeHDMI(x)           (((x)>=INPUT_SOURCE_HDMI)&&((x)<=INPUT_SOURCE_HDMI_MAX))///< x is HDMI
#define IsSrcTypeVga(x)            (((x)==INPUT_SOURCE_VGA) || ((x)==INPUT_SOURCE_VGA2) || ((x)==INPUT_SOURCE_VGA3))///< x is VGA
#define IsSrcTypeYPbPr(x)          (((x)>=INPUT_SOURCE_YPBPR)&&((x)<=INPUT_SOURCE_YPBPR_MAX))///<  x is YPbPr
#define IsSrcTypeDTV(x)            (((x)==INPUT_SOURCE_DTV) || ((x)==INPUT_SOURCE_DTV2) || ((x)==INPUT_SOURCE_DTV3))///< x is DTV
#define IsSrcTypeDVI(x)            (((x)>=INPUT_SOURCE_DVI)&&((x)<=INPUT_SOURCE_DVI_MAX))///< x is DVI
#define IsSrcTypeJpeg(x)           ((x)==INPUT_SOURCE_JPEG)///< x is JPEG
#define IsSrcTypeStorage(x)        (((x)==INPUT_SOURCE_STORAGE) || ((x)==INPUT_SOURCE_STORAGE2) ||IsSrcTypeJpeg(x))///< x is Storage
#define IsSrcTypeDigitalVD(x)      ( IsSrcTypeAV(x)||IsSrcTypeSV(x)||IsSrcTypeATV(x)||IsSrcTypeScart(x))///< x is VD
#define IsSrcTypeAnalog(x)         ( IsSrcTypeVga(x)|| IsSrcTypeYPbPr(x))///< x is Analog
#define IsSrcTypeVideo(x)          ( IsSrcTypeYPbPr(x) || IsSrcTypeATV(x) || IsSrcTypeAV(x) || IsSrcTypeSV(x) || IsSrcTypeScart(x) || IsSrcTypeDTV(x) ) ///< x is Video
#define IsSrcTypeCapture(x)        ((x)==INPUT_SOURCE_SCALER_OP)///< x is scaler OP

////////////////////////////////////////////////////////////////////////////////
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// This is abstract output source for XC library. If you want to add another one.
// Please contace with XC member
////////////////////////////////////////////////////////////////////////////////

/// Define output type
typedef enum
{
    OUTPUT_NONE     = 0,                ///<NULL output
    OUTPUT_SCALER_MAIN_WINDOW = 1,          ///<output to Scaler main window
    OUTPUT_SCALER2_MAIN_WINDOW = 2,

    OUTPUT_SCALER_SUB_WINDOW  = 10,     ///<output to Scaler sub window
    OUTPUT_SCALER2_SUB_WINDOW = 11,

    OUTPUT_CVBS1    = 20,               ///<output to CVBS1
    OUTPUT_CVBS2,                       ///<output to CVBS2
    // Reserved area

    OUTPUT_YPBPR0   = 40,               ///<output to YPBPR0
    OUTPUT_YPBPR1,                      ///<output to YPBPR1
    // Reserved area

    OUTPUT_HDMI1    = 60,               ///<output to HDMI1
    OUTPUT_HDMI2,                       ///<output to HDMI2

    OUTPUT_OFFLINE_DETECT = 80,         ///<output to OFFLINE detect

    OUTPUT_SCALER_DWIN = 100,           ///<output to Dwin

    OUTPUT_NUM,                         ///<number of outputs
} E_DEST_TYPE;


/// Define DeInterlace mode
typedef enum
{

    MS_DEINT_OFF = 0,       ///<deinterlace mode off
    MS_DEINT_2DDI_BOB,      ///<deinterlace mode: BOB
    MS_DEINT_2DDI_AVG,      ///<deinterlace mode: AVG
    MS_DEINT_3DDI_HISTORY,  ///<deinterlace mode: HISTORY // 24 bit
    MS_DEINT_3DDI,          ///<deinterlace mode:3DDI// 16 bit
} MS_DEINTERLACE_MODE;


/// Define the members of IP Sync Status
typedef struct
{
    MS_U8  u8SyncStatus;    ///< IP Sync status: refer to MD_VSYNC_POR_BIT, etc
    MS_U16 u16Hperiod;      ///<H Period
    MS_U16 u16Vtotal;       ///<Vertcal total
} XC_IP_SYNC_STATUS;

typedef enum
{
    CMA_XC_SELF_MEM = 0,
    CMA_XC_COBUFF_MEM = 1,
    CMA_XC_MEM_NUM = 2,
} XC_CMA_CLIENT;

typedef struct
{
    MS_U32   u32HeapID;
    MS_U64   u64AddrHeapOffset;
} XC_CMA_CONFIG;

/// Define Freerun color
typedef enum
{
    E_XC_FREE_RUN_COLOR_BLACK,
    E_XC_FREE_RUN_COLOR_WHITE,
    E_XC_FREE_RUN_COLOR_BLUE,
    E_XC_FREE_RUN_COLOR_RED,
    E_XC_FREE_RUN_COLOR_GREEN,
    E_XC_FREE_RUN_COLOR_MAX,
} XC_FREERUN_COLOR;

/// Define which panel output timing change mode is used to change VFreq for same panel
typedef enum
{
    E_XC_PNL_CHG_DCLK   = 0,        ///<change output DClk to change Vfreq.
    E_XC_PNL_CHG_HTOTAL = 1,        ///<change H total to change Vfreq.
    E_XC_PNL_CHG_VTOTAL = 2,        ///<change V total to change Vfreq.
} E_XC_PNL_OUT_TIMING_MODE;

typedef enum
{
    E_XC_PNL_LPLL_TTL,                              ///< TTL  type
    E_XC_PNL_LPLL_LVDS,                             ///< LVDS type
    E_XC_PNL_LPLL_RSDS,                             ///< RSDS type
    E_XC_PNL_LPLL_MINILVDS,                         ///< TCON  //E_XC_PNL_LPLL_MINILVDS_6P_2Link
    E_XC_PNL_LPLL_ANALOG_MINILVDS,                  ///< Analog TCON
    E_XC_PNL_LPLL_DIGITAL_MINILVDS,                 ///< Digital TCON
    E_XC_PNL_LPLL_MFC,                              ///< Ursa (TTL output to Ursa)
    E_XC_PNL_LPLL_DAC_I,                            ///< DAC output
    E_XC_PNL_LPLL_DAC_P,                            ///< DAC output
    E_XC_PNL_LPLL_PDPLVDS,                          ///< For PDP(Vsync use Manually MODE)
    E_XC_PNL_LPLL_EXT,
} E_XC_PNL_LPLL_TYPE;


typedef enum
{

    E_XC_PNL_LPLL_EPI34_8P = E_XC_PNL_LPLL_EXT,
    E_XC_PNL_LPLL_EPI28_8P,
    E_XC_PNL_LPLL_EPI34_6P,
    E_XC_PNL_LPLL_EPI28_6P,

    ///< replace this with E_XC_PNL_LPLL_MINILVD
    E_XC_PNL_LPLL_MINILVDS_5P_2L,
    E_XC_PNL_LPLL_MINILVDS_4P_2L,
    E_XC_PNL_LPLL_MINILVDS_3P_2L,
    E_XC_PNL_LPLL_MINILVDS_6P_1L,
    E_XC_PNL_LPLL_MINILVDS_5P_1L,
    E_XC_PNL_LPLL_MINILVDS_4P_1L,
    E_XC_PNL_LPLL_MINILVDS_3P_1L,

    E_XC_PNL_LPLL_HS_LVDS,                          ///< High speed LVDS
    E_XC_PNL_LPLL_HF_LVDS,                          ///< High Freqquency LVDS

    E_XC_PNL_LPLL_TTL_TCON,
    E_XC_PNL_LPLL_MINILVDS_2CH_3P_8BIT,             ///< 2 channel, 3 pair, 8 bits
    E_XC_PNL_LPLL_MINILVDS_2CH_4P_8BIT,             ///< 2 channel, 4 pair, 8 bits
    E_XC_PNL_LPLL_MINILVDS_2CH_5P_8BIT,             ///< 2 channel, 5 pair, 8 bits
    E_XC_PNL_LPLL_MINILVDS_2CH_6P_8BIT,             ///< 2 channel, 6 pair, 8 bits

    E_XC_PNL_LPLL_MINILVDS_1CH_3P_8BIT,             ///< 1 channel, 3 pair, 8 bits
    E_XC_PNL_LPLL_MINILVDS_1CH_4P_8BIT,             ///< 1 channel, 4 pair, 8 bits
    E_XC_PNL_LPLL_MINILVDS_1CH_5P_8BIT,             ///< 1 channel, 5 pair, 8 bits
    E_XC_PNL_LPLL_MINILVDS_1CH_6P_8BIT,             ///< 1 channel, 6 pair, 8 bits

    E_XC_PNL_LPLL_MINILVDS_2CH_3P_6BIT,             ///< 2 channel, 3 pari, 6 bits
    E_XC_PNL_LPLL_MINILVDS_2CH_4P_6BIT,             ///< 2 channel, 4 pari, 6 bits
    E_XC_PNL_LPLL_MINILVDS_2CH_5P_6BIT,             ///< 2 channel, 5 pari, 6 bits
    E_XC_PNL_LPLL_MINILVDS_2CH_6P_6BIT,             ///< 2 channel, 6 pari, 6 bits

    E_XC_PNL_LPLL_MINILVDS_1CH_3P_6BIT,             ///< 1 channel, 3 pair, 6 bits
    E_XC_PNL_LPLL_MINILVDS_1CH_4P_6BIT,             ///< 1 channel, 4 pair, 6 bits
    E_XC_PNL_LPLL_MINILVDS_1CH_5P_6BIT,             ///< 1 channel, 5 pair, 6 bits
    E_XC_PNL_LPLL_MINILVDS_1CH_6P_6BIT,             ///< 1 channel, 6 pair, 6 bits

    E_XC_PNL_LPLL_HDMI_BYPASS_MODE,        //40  // HDMI Bypass Mode

    E_XC_PNL_LPLL_EPI34_2P,                         /// 41
    E_XC_PNL_LPLL_EPI34_4P,                         /// 42
    E_XC_PNL_LPLL_EPI28_2P,                         /// 43
    E_XC_PNL_LPLL_EPI28_4P,                         /// 44

    E_XC_PNL_LPLL_VBY1_10BIT_4LANE,    ///45
    E_XC_PNL_LPLL_VBY1_10BIT_2LANE,    ///46
    E_XC_PNL_LPLL_VBY1_10BIT_1LANE,    ///47
    E_XC_PNL_LPLL_VBY1_8BIT_4LANE,     ///48
    E_XC_PNL_LPLL_VBY1_8BIT_2LANE,     ///49
    E_XC_PNL_LPLL_VBY1_8BIT_1LANE,     ///50

    E_XC_PNL_LPLL_VBY1_10BIT_8LANE,    ///51
    E_XC_PNL_LPLL_VBY1_8BIT_8LANE,     ///52

    E_XC_PNL_LPLL_EPI28_12P,           /// 53

    E_XC_PNL_LPLL_HS_LVDS_BYPASS_MODE, ///54
    E_XC_PNL_LPLL_VBY1_10BIT_4LANE_BYPASS_MODE, ///55
    E_XC_PNL_LPLL_VBY1_8BIT_4LANE_BYPASS_MODE, ///56

    E_XC_PNL_LPLL_VBY1_10BIT_16LANE, //57
    E_XC_PNL_LPLL_VBY1_8BIT_16LANE, //58
    E_XC_PNL_LPLL_EPI24_12P, //59
    E_XC_PNL_LPLL_USI_T_8BIT_12P, //60
    E_XC_PNL_LPLL_USI_T_10BIT_12P, //61
    E_XC_PNL_LPLL_ISP_8BIT_12P, //62
    E_XC_PNL_LPLL_ISP_8BIT_6P_D, //63

    E_XC_PNL_LPLL_TYPE_MAX,
} E_XC_PNL_LPLL_EXT_TYPE;

typedef enum
{
    E_XC_MOD_OUTPUT_SINGLE = 0,          ///< single channel
    E_XC_MOD_OUTPUT_DUAL = 1,            ///< dual channel

    E_XC_MOD_OUTPUT_QUAD = 2,            ///< quad channel

} E_XC_OUTPUT_MODE;

typedef enum
{
    VFREQ_50HZ          = 0x00,
    VFREQ_60HZ          = 0x01,
    VFREQ_FROM_SRC      = 0x02,
    VFREQ_FROM_PANEL    = 0x03,
} E_VFREQ_SEL;

/// Sources to VE
typedef enum
{
    E_XC_NONE    = 0x00,
    E_XC_IP     = 0x01,     ///< Scaler IP
    E_XC_VOP2   = 0x02,     ///< Scaler VOP2
    E_XC_BRI    = 0x04,     ///< Scaler BRI

    /// Please use this for OSD OP video capture!!!
    E_XC_OP2    = 0x08,     ///< Scaler OP2 - After Blending with OSD
    E_XC_GAM    = 0x10,     ///< Scaler Gamma - Without Blending with OSD
    E_XC_DITHER  = 0x20,    ///< Scaler noise dither - Without Blending with OSD

    /// Please use this for non-OSD OP video capture!!!
    E_XC_OVERLAP = 0x40,    ///< Scaler color overlap - Without Blending with OSD
    E_XC_IP_SUB  = 0x80,    ///< Scaler IP of sub window
} E_XC_SOURCE_TO_VE;

///Port connect to pin 8 of SCART
typedef enum
{
    E_XC_SCARTID_NONE      = 0x00,  ///< No connection
    E_XC_SCARTID_TO_HSYNC0 = 0x01,  ///< Pin 8 of SCART connects to Hsync 0
    E_XC_SCARTID_TO_HSYNC1 = 0x02,  ///< Pin 8 of SCART connects to Hsync 1
    E_XC_SCARTID_TO_HSYNC2 = 0x04,  ///< Pin 8 of SCART connects to Hsync 2
    E_XC_SCARTID_TO_SOG0   = 0x08,  ///< Pin 8 of SCART connects to SOG0
    E_XC_SCARTID_TO_SOG1   = 0x10,  ///< Pin 8 of SCART connects to SOG1
    E_XC_SCARTID_TO_SOG2   = 0x20,  ///< Pin 8 of SCART connects to SOG2
} E_XC_SCART_ID_PORT;

/// Items for query driver or H/W capability.
typedef enum
{
    E_XC_SUPPORT_IMMESWITCH        = 0x00000001,  ///< return true if H/W support HDMI immeswitch

    E_XC_SUPPORT_DVI_AUTO_EQ       = 0x00000005,  ///< return true if driver support Auto EQ.

    E_XC_SUPPORT_FRC_INSIDE        = 0x00000100,  ///< return true if scaler driver support FRC (MFC) function.

} E_XC_CAPABILITY;

typedef enum
{
    E_XC_IMMESWITCH          = 0x00000001,  ///< return true if H/W support HDMI immeswitch

    E_XC_DVI_AUTO_EQ         = 0x00000002,  ///< return true if driver support Auto EQ.

    E_XC_FRC_INSIDE          = 0x00000004,  ///< return true if scaler driver support FRC (MFC) function.

    E_XC_DIP_CHIP_CAPS       = 0x00000008,  ///< return dip chip caps.

    E_XC_3D_FBL_CAPS         = 0x00000010,  ///< return true if chip support fbl 3d.attention:fbl 3d only support sbs to lbl and sbs to sbs

    E_XC_HW_SEAMLESS_ZAPPING   = 0x00000020,  ///< return true if H/W support seamless zapping

    E_XC_SUPPORT_DEVICE1       = 0x00000040,  ///< return true if H/W support scaler device1

    E_XC_SUPPORT_DETECT3D_IN3DMODE = 0x00000080,  ///< return true if H/W support detecting 3d when already in 3d mode

    E_XC_2DTO3D_VERSION = 0x00000100,  ///< return value 2D-to-3D version

    E_XC_SUPPORT_FORCE_VSP_IN_DS_MODE = 0x00000200,  ///< return if H/W support force post-Vscalin-down in DS mode

    E_XC_SUPPORT_FRCM_MODE = 0x00000400,  ///< return if H/W support frcm

    E_XC_SUPPORT_INTERLACE_OUT = 0x00000800, ///< return if H/W supports interlace output timing

    E_XC_SUPPORT_4K2K_WITH_PIP = 0x00001000, ///< return if H/W supports output is 4k2k and open pip

    E_XC_SUPPORT_4K2K_60P = 0x00002000, ///< return if H/W supports output is 4k2k_60P timing

    E_XC_SUPPORT_PIP_PATCH_USING_SC1_MAIN_AS_SC0_SUB =  0x00004000,///<return if sc1 suport PIP

    E_XC_HW_4K2K_VIP_PEAKING_LIMITATION = 0x00008000, ///return if H/W has vip peaking limitation
#ifdef UFO_XC_AUTO_DOWNLOAD
    E_XC_SUPPORT_AUTODOWNLOAD_CLIENT = 0x00010000, ///return whether the client is supported, refer to XC_AUTODOWNLOAD_CLIENT_SUPPORTED_CAPS
#endif
#ifdef UFO_XC_HDR
#if (UFO_XC_HDR_VERSION == 2)
    E_XC_SUPPORT_HDR = 0x00020000, ///return whether HDR is supported, refer to XC_HDR_SUPPORTED_CAPS
#endif
#endif
    E_XC_SUPPORT_3D_DS = 0x00040000, ///return whether 3D DS is supported

    E_XC_DIP_CHIP_SOURCESEL = 0x00080000, ///< return if H/W supports output is 4k2k_60P timing

    E_XC_DIP_CHIP_WINDOWBUS = 0x00100000, ///< return DIP window bus length

    E_XC_SCALING_LIMITATION = 0x00800000, ///return whether scaling is supported

    E_XC_FB_CAPS_GET_FB_LEVEL = 0x01000000,//get fb caps
} E_XC_CAPS;

typedef enum
{
    E_XC_DS_INDEX_MVOP,
    E_XC_DS_INDEX_SCALER,
    E_XC_DS_INDEX_MAX,
} E_XC_DS_INDEX_SOURCE;

typedef enum
{
    E_XC_FRC_InputTiming_2K1K,
    E_XC_FRC_InputTiming_4K2K,
    E_XC_FRC_InputTiming_MAX,
} E_XC_FRC_InputTiming;

typedef enum
{
    PIXEL_SHIFT_MOVE_MAIN_WINDOW = 0x0001,
    PIXEL_SHIFT_MOVE_SUB_WINDOW = 0x0002,
    PIXEL_SHIFT_MOVE_REPORT_WINDOW  = 0x0004,
    PIXEL_SHIFT_MOVE_OSD_LAYER      = 0x0008,
    PIXEL_SHIFT_DO_OVERSCAN         = 0x0010,
} PIXEL_SHIFT_FEATURE;

/// OP2 Video/GOP layer switch
typedef enum
{
    E_VIDEO_ON_OSD_LAYER_DEAFULT = 0, ///< video -> osd layer 0 -> osd layer 1 -> osd layer 2 -> osd layer 3 -> osd layer 4
    E_VIDEO_ON_OSD_LAYER_0 = 1,       ///< osd layer 0 -> video -> osd layer 1 -> osd layer 2 -> osd layer 3 -> osd layer 4
    E_VIDEO_ON_OSD_LAYER_1 = 2,       ///< osd layer 0 -> osd layer 1 -> video -> osd layer 2 -> osd layer 3 -> osd layer 4
    E_VIDEO_ON_OSD_LAYER_2 = 3,       ///< osd layer 0 -> osd layer 1 -> osd layer 2 -> video -> osd layer 3 -> osd layer 4
    E_VIDEO_ON_OSD_LAYER_3 = 4,       ///< osd layer 0 -> osd layer 1 -> osd layer 2 -> osd layer 3 -> video -> osd layer 4
    E_VIDEO_ON_OSD_LAYER_4 = 5,       ///< osd layer 0 -> osd layer 1 -> osd layer 2 -> osd layer 3 -> osd layer 4 -> video
    E_VIDEO_ON_OSD_LAYER_NUM,
} E_VIDEO_ON_OSD_LAYER;

/// Define panel information
typedef struct __attribute__((packed))
{
    // XC need below information do to frame lock

    MS_U16 u16HStart;           ///<DE H start

    MS_U16 u16VStart;           ///<DE V start

    MS_U16 u16Width;            ///<DE H width

    MS_U16 u16Height;           ///< DE V height

    MS_U16 u16HTotal;           ///<H total

    MS_U16 u16VTotal;           ///<V total

    MS_U16 u16DefaultVFreq;     ///<Panel output Vfreq., used in free run

    MS_U8  u8LPLL_Mode;         ///<0: single mode, 1: dual mode
    E_XC_PNL_OUT_TIMING_MODE enPnl_Out_Timing_Mode;    ///<Define which panel output timing change mode is used to change VFreq for same panel

    // Later need to refine to use Min/Max SET for PDP panel, but for LCD, it maybe no need to check the Min/Max SET
    MS_U16 u16DefaultHTotal;    ///<default H total
    MS_U16 u16DefaultVTotal;    ///<default V total

    MS_U32 u32MinSET;
    MS_U32 u32MaxSET;
    E_XC_PNL_LPLL_TYPE  eLPLL_Type;         ///
}
XC_PANEL_INFO;


/// Define the extern special panel information for FPLL or other usage
/// Usage:
///       The first 3 members of below structure must be set as their corresponding description
///       The other members are set according to your request, none used members should be cleared to zero
/// Example code of set VTT slowly:
///    XC_PANEL_INFO_EX stPanelInfoEx;
///    memset(&stPanelInfoEx, 0, sizeof(XC_PANEL_INFO_EX));
///    stPanelInfoEx.u32PanelInfoEx_Version = PANEL_INFO_EX_VERSION;
///    stPanelInfoEx.u16PanelInfoEX_Length = sizeof(XC_PANEL_INFO_EX);
///
///    stPanelInfoEx.bVttStepsValid = TRUE;
///    stPanelInfoEx.bVttDelayValid = TRUE;
///    stPanelInfoEx.u16VttSteps = xx; //set how many steps u want to set the vtt
///    stPanelInfoEx.u16VttDelay = xx;// set the delay between steps of setting vtt
///    stPanelInfoEx.u16VFreq = 500; //this step setting info is only for 50hz
///    if(MApi_XC_SetExPanelInfo(TRUE, &stPanelInfoEx))//Check the set is accepted or not
///    stPanelInfoEx.u16VFreq = 600; //set same steps info for 60 hz
///    if(MApi_XC_SetExPanelInfo(TRUE, &stPanelInfoEx))//Check the set is accepted or not
///
typedef struct __attribute__((packed))
{
    MS_U32 u32PanelInfoEx_Version;   ///<Version of current structure. Please always set to "PANEL_INFO_EX_VERSION" as input
    MS_U16 u16PanelInfoEX_Length;    ///<Length of this structure, u16PanelInfoEX_Length=sizeof(XC_PANEL_INFO_EX)
    MS_U16 u16VFreq;                 ///<Output Vfreq, unit: (1Hz/10), range 0~PANEL_INFO_EX_INVALID_ALL
    ///<If disable ex panel info and u16VFreq = 0xFFFF means disable all valid ex panel info
    ///<If disable ex panel info and u16VFreq = 600 means disable the valid ex panel info for 60 hz
    MS_U16 u16HTotal;                ///<H total of output "u16VFreq"
    MS_BOOL bHTotalValid;            ///H total setting is valid in this structure
    MS_U16 u16VTotal;                ///<V total of output "u16VFreq"
    MS_BOOL bVTotalValid;            ///V total setting is valid in this structure
    MS_U32 u32DClk;                  ///<DCLK of output "u16VFreq", unit: KHz
    MS_BOOL bDClkValid;              ///DCLK, unit: KHz
    MS_U16 u16MinHTotal;             ///<Min H total
    MS_BOOL bMinHTotalValid;         ///Min H total
    MS_U16 u16MinVTotal;             ///<Min V total
    MS_BOOL bMinVTotalValid;         ///Min V total
    MS_U32 u32MinDClk;               ///<Min DCLK, unit: KHz
    MS_BOOL bMinDClkValid;           ///Min DCLK, unit: KHz
    MS_U16 u16MaxHTotal;             ///<Max H total
    MS_BOOL bMaxHTotalValid;         ///Max H total
    MS_U16 u16MaxVTotal;             ///<Max V total
    MS_BOOL bMaxVTotalValid;         ///Max V total
    MS_U32 u32MaxDClk;               ///<Min DCLK, unit: KHz
    MS_BOOL bMaxDClkValid;           ///Min DCLK, unit: KHz
    // sync related
    MS_U16 u16HSyncWidth;            ///<VOP_01[7:0], PANEL_HSYNC_WIDTH
    MS_BOOL bHSyncWidthValid;        ///<  VOP_01[7:0], PANEL_HSYNC_WIDTH
    MS_U16 u16HSyncBackPorch;        ///<PANEL_HSYNC_BACK_PORCH, no register setting, provide value for query only
    ///<not support Manuel VSync Start/End now
    ///<VOP_02[10:0] VSync start = Vtt - VBackPorch - VSyncWidth
    ///<VOP_03[10:0] VSync end = Vtt - VBackPorch
    MS_BOOL bHSyncBackPorchValid;
    MS_U16 u16VSyncWidth;            ///<define PANEL_VSYNC_WIDTH
    MS_BOOL bVSyncWidthValid;
    MS_U16 u16VSyncBackPorch;        ///<define PANEL_VSYNC_BACK_PORCH
    MS_BOOL bVSyncBackPorchValid;
    //step by step adjustment when 50/60 HZ Output switch
    MS_U16 u16HttSteps;              ///<Number of adjusting step from Old Htt to new htt
    MS_BOOL bHttStepsValid;
    MS_U16 u16HttDelay;              ///<Delay time in two steps
    MS_BOOL bHttDelayValid;
    MS_U16 u16VttSteps;              ///<Number of adjusting step from Old Htt to new htt
    MS_BOOL bVttStepsValid;
    MS_U16 u16VttDelay;              ///<Delay time in two steps
    MS_BOOL bVttDelayValid;
    MS_U16 u16DclkSteps;             ///<Number of adjusting step from Old Htt to new htt
    MS_BOOL bDclkStepsValid;
    MS_U16 u16DclkDelay;             ///<Delay time in two steps
    MS_BOOL bDclkDelayValid;         ///<DclkDelay setting is valid in this structure
}
XC_PANEL_INFO_EX;

/// Define new panel information
typedef struct
{
    MS_U32 u32TimingAdjustSetting_version;    ///<Version of current structure. Please always set this value as XC_OUTPUT_TIMING_ADJUST_VERSION

    MS_U16 u16HTotal_Step;              ///<Number of adjusting step from Old Htt to new htt
    MS_U16 u16HTotal_Delay;             ///<Delay time in two steps
    MS_U16 u16HTotal_Ratio;             ///<Ratio of Htt for change to specific framerate (X10).

    MS_U16 u16VTotal_Step;              ///<Number of adjusting step from Old Vtt to new Vtt
    MS_U16 u16VTotal_Delay;             ///<Delay time in two steps
    MS_U16 u16VTotal_Ratio;             ///<Ratio of Vtt for change to specific framerate (X10).

    MS_U16 u16Dclk_Step;                ///<Number of adjusting step from Old Dclk to new Dclk
    MS_U16 u16Dclk_Delay;               ///<Delay time in two steps
    MS_U16 u16Dclk_Ratio;               ///<Ratio of Dclk for change to specific framerate (X10).

} XC_OUTPUT_TIMING_ADJUST_SETTING;

/// Define the initial data for XC
typedef struct __attribute__((packed))
{
    MS_U32 u32XC_version;                   ///<Version of current structure.
    // system related
    MS_U32 u32XTAL_Clock;                   ///<Crystal clock in Hz

    // frame buffer related
    u32 u32Main_FB_Start_Addr;       ///<scaler main window frame buffer start address, absolute without any alignment
    u32 u32Main_FB_Size;                 ///<scaler main window frame buffer size, the unit is BYTES
    u32 u32Sub_FB_Start_Addr;        ///<scaler sub window frame buffer start address, absolute without any alignment
    u32 u32Sub_FB_Size;                  ///<scaler sub window frame buffer size, the unit is BYTES

    // HDMI related, will be moved to HDMI module
    MS_BOOL bCEC_Use_Interrupt;             ///<CEC use interrupt or not, if not, will use polling

    // This is related to chip package. ( Share Ground / Non-Share Ground )
    MS_BOOL bIsShareGround;

    // function related
    MS_BOOL bEnableIPAutoCoast;             ///<Enable IP auto coast

    MS_BOOL bMirror;                        ///<mirror mode

    // panel related
    XC_PANEL_INFO stPanelInfo;              ///<panel infomation

    // DLC
    MS_BOOL bDLC_Histogram_From_VBlank;     ///<If set to TRUE, the Y max/min report value is read from V blanking area

    // This is related to layout
    MS_U16 eScartIDPort_Sel;    ///<This is port selection (E_XC_SCART_ID_PORT) of Scart ID pin 8

    // frcm frame buffer related
    u32 u32Main_FRCM_FB_Start_Addr;       ///<scaler main window frcm frame buffer start address, absolute without any alignment
    u32 u32Main_FRCM_FB_Size;                 ///<scaler main window frcm frame buffer size, the unit is BYTES
    u32 u32Sub_FRCM_FB_Start_Addr;        ///<scaler sub window frcm frame buffer start address, absolute without any alignment
    u32 u32Sub_FRCM_FB_Size;                  ///<scaler sub window frcm frame buffer size, the unit is BYTES
}
XC_INITDATA;

/// define generic API infomation
typedef struct __attribute__((packed))
{
    MS_U8 u8MaxWindowNum;                   ///<max window number that XC driver supports such as MAIN/SUB window
    MS_U8 u8NumOfDigitalDDCRam;             ///<indicate how many Digital DDCRam that use can use without external EDID EEPROM
    MS_U8 u8MaxFrameNumInMem;               ///<indicate maximal number of frames (Progressive mode) supported by scaler simultaneously
    MS_U8 u8MaxFieldNumInMem;               ///<indicate maximal number of fields (Interlace mode) supported by scaler simultaneously
}
XC_ApiInfo;

/// define the information of set window
typedef struct
{
    //-------------
    // Input
    //-------------
    INPUT_SOURCE_TYPE_t enInputSourceType;      ///<Input source

    //-------------
    // Window
    //-------------
    MS_WINDOW_TYPE stCapWin;        ///<Capture window
    MS_WINDOW_TYPE stDispWin;       ///<Display window
    MS_WINDOW_TYPE stCropWin;       ///<Crop window

    //-------------
    // Timing
    //-------------
    MS_BOOL bInterlace;             ///<Interlaced or Progressive
    MS_BOOL bHDuplicate;            ///<flag for vop horizontal duplicate, for MVD, YPbPr, indicate input double sampled or not
    MS_U16  u16InputVFreq;          ///<Input V Frequency, VFreqx10, for calculate output panel timing
    MS_U16  u16InputVTotal;         ///<Input Vertical total, for calculate output panel timing
    MS_U16  u16DefaultHtotal;       ///<Default Htotal for VGA/YPbPr input
    MS_U8  u8DefaultPhase;          ///<Obsolete

    //-------------------------
    // customized post scaling
    //-------------------------
    MS_BOOL bHCusScaling;               ///<assign post H customized scaling instead of using XC scaling
    MS_U16  u16HCusScalingSrc;          ///<post H customized scaling src width
    MS_U16  u16HCusScalingDst;          ///<post H customized scaling dst width
    MS_BOOL bVCusScaling;               ///<assign post V manuel scaling instead of using XC scaling
    MS_U16  u16VCusScalingSrc;          ///<post V customized scaling src height
    MS_U16  u16VCusScalingDst;          ///<post V customized scaling dst height

    //--------------
    // 9 lattice
    //--------------
    MS_BOOL bDisplayNineLattice;        ///<used to indicate where to display in panel and where to put in frame buffer

    //-------------------------
    // customized pre scaling
    //-------------------------
    MS_BOOL bPreHCusScaling;            ///<assign pre H customized scaling instead of using XC scaling
    MS_U16  u16PreHCusScalingSrc;       ///<pre H customized scaling src width
    MS_U16  u16PreHCusScalingDst;       ///<pre H customized scaling dst width
    MS_BOOL bPreVCusScaling;            ///<assign pre V manuel scaling instead of using XC scaling
    MS_U16  u16PreVCusScalingSrc;       ///<pre V customized scaling src height
    MS_U16  u16PreVCusScalingDst;       ///<pre V customized scaling dst height
    MS_U16  u16DefaultPhase;            ///<Default Phase for VGA/YPbPr input
} XC_SETWIN_INFO;

typedef struct
{
    //mastering color display: color volumne of a display
    MS_U32 u32MaxLuminance;
    MS_U32 u32MinLuminance;
    MS_U16 u16DisplayPrimaries[3][2];
    MS_U16 u16WhitePoint[2];
} XC_DS_MasterColorDisplay;

typedef struct
{
    //color_description: indicates the chromaticity/opto-electronic coordinates of the source primaries
    MS_U8 u8ColorPrimaries;
    MS_U8 u8TransferCharacteristics;
    // matrix coefficients in deriving YUV signal from RGB
    MS_U8 u8MatrixCoefficients;
} XC_DS_ColorDescription;

typedef struct
{
    // bit[0]: MS_ColorDescription present or valid, bit[1]: MS_MasterColorDisplay present or valid
    MS_U32 u32FrmInfoExtAvail;
    // mastering color display: color volumne of a display
    XC_DS_ColorDescription stColorDescription;
    XC_DS_MasterColorDisplay stMasterColorDisplay;

    MS_U8   u8CurrentIndex;
    u32  phyRegAddr;
    MS_U32  u32RegSize;
    u32  phyLutAddr;
    MS_U32  u32LutSize;
    MS_BOOL bDMEnable;
    MS_BOOL bCompEnable;
} XC_DS_HDRInfo;

typedef struct
{
    MS_U32 u32ApiDSInfo_Version;   ///<Version of current structure. Please always set to "API_XCDS_INFO_VERSION" as input
    MS_U16 u16ApiDSInfo_Length;
    //MVOP
    MS_U32 u32MFCodecInfo;
    XC_DS_HDRInfo stHDRInfo;
    //XC
    MS_BOOL bUpdate_DS_CMD[MAX_WINDOW];
    MS_BOOL bEnableDNR[MAX_WINDOW];
    MS_U32  u32DSBufferSize;
} XC_DS_INFO;


/// define the information for dynamic scaling (DS)
/// one entry of DS is:
///
/// 8 bit alignment
/// LSB ----> MSB
/// OP (32bits) -> IP (32bits)
/// [7:0]   [15:8]   [23:16]          [31:24]
/// Data_L  Data_H   Reg_Addr(16bit)  Reg_Bank
///
/// 16 bit alignment
/// LSB ----> MSB
/// OP (32bits) -> IP (32bits) -> 0xFFFFFFFF, 0xFFFFFFFF (dummy)
///
/// If bIP_DS_On == FALSE, the DS will ignore the IP information
/// If bOP_DS_On == FALSE, the DS will ignore the OP information
typedef struct
{
    u32 u32DS_Info_BaseAddr;         ///< The memory base address to update IP/OP dynamic scaling registers. Absolute without any alignment.
    MS_U8  u8MIU_Select;                ///< 0: MIU0, 1: MIU1, etc.
    MS_U8  u8DS_Index_Depth;            ///< The index depth (how many entries to fire per MVOP Vsync falling) of DS
    MS_BOOL bOP_DS_On;                  ///< OP dynamic scaling on/off
    MS_BOOL bIPS_DS_On;                 ///< IPS dynamic scaling on/off
    MS_BOOL bIPM_DS_On;                 ///< IPM dynamic scaling on/off
} XC_DynamicScaling_Info;

/// Define source type for DIP
typedef enum
{
    SCALER_DIP_SOURCE_TYPE_SUB2 = 0,        ///< DIP from SUB2(IP_F3)
    SCALER_DIP_SOURCE_TYPE_MAIN = 1,        ///< DIP from MAIN(IP_MAIN)
    SCALER_DIP_SOURCE_TYPE_SUB  = 2,        ///< DIP from SUB (IP_SUB)
    SCALER_DIP_SOURCE_TYPE_OP_MAIN = 3,     ///< HVSP MAIN
    SCALER_DIP_SOURCE_TYPE_OP_SUB = 4,      ///< HVSP SUB
    SCALER_DIP_SOURCE_TYPE_DRAM = 5,        ///< DIP from DRAM
    SCALER_DIP_SOURCE_TYPE_OP_CAPTURE = 6, ///< DIP from OP capture
    SCALER_DIP_SOURCE_TYPE_OSD = 7,        ///< DIP from OSD capture
    SCALER_DIP_SOURCE_TYPE_SC2_OP_CAPTURE = 8, ///< DIP from SC2 OP capture
    SCALER_DIP_SOURCE_TYPE_OP_SC1_CAPTURE = 9, ///< DIP from OP SC1 capture
    SCALER_DIP_SOURCE_TYPE_MVOP0 = 10,           ///< DIP from MVOP0 capture
    SCALER_DIP_SOURCE_TYPE_MVOP1 = 11,          ///< DIP from MVOP1 capture
    MAX_SCALER_DIP_SOURCE_NUM            ///< The max support window
} SCALER_DIP_SOURCE_TYPE;

/// Define the path type
typedef enum
{
    PATH_TYPE_SYNCHRONOUS,      ///<synchronous path type
    PATH_TYPE_ASYNCHRONOUS,     ///<asynchronous path type
} E_PATH_TYPE;

/// Define the mux path information
typedef struct
{
    E_PATH_TYPE Path_Type;      ///<path type
    INPUT_SOURCE_TYPE_t src;    ///<source input type
    E_DEST_TYPE dest;           ///<destination type of output

    void (* path_thread)(INPUT_SOURCE_TYPE_t src, MS_BOOL bRealTimeMonitorOnly);    ///<path read
    void (* SyncEventHandler )(INPUT_SOURCE_TYPE_t src, void* para);                ///<sync event handler
    void (* DestOnOff_Event_Handler )(INPUT_SOURCE_TYPE_t src, void* para);         ///<destination on off event handler
    void (* dest_periodic_handler )(INPUT_SOURCE_TYPE_t src, MS_BOOL bRealTimeMonitorOnly) ;    ///<destination periodic handler
} XC_MUX_PATH_INFO;

/// Image store format in XC
typedef enum
{
    IMAGE_STORE_444_24BIT,    ///< (8+8+8)   Y Cb Cr   / B G R
    IMAGE_STORE_422_16BIT,    ///< (8+8)     Y Cb Y Cr / G B G R
    IMAGE_STORE_422_24BIT,    ///< (10+10+4) Y Cb Y Cr / G B G R -- Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0 - C5 C4 C3 C2 C1 C0 Y9 Y8 - xx xx xx xx C9 C8 C7 C6
} MS_IMAGE_STORE_FMT;

/// XC API status
typedef struct
{
    //----------------------
    // Customer setting
    //----------------------
    INPUT_SOURCE_TYPE_t enInputSourceType;      ///< Input source type

    //----------------------
    // Window
    //----------------------
    MS_WINDOW_TYPE stCapWin;       ///<Capture window
    MS_WINDOW_TYPE stDispWin;      ///<Display window
    MS_WINDOW_TYPE stCropWin;      ///<Crop window

    //----------------------
    // Timing
    //----------------------
    MS_BOOL bInterlace;             ///<Interlaced or Progressive
    MS_BOOL bHDuplicate;            ///<flag for vop horizontal duplicate, for MVD, YPbPr, indicate input double sampled or not
    MS_U16  u16InputVFreq;          ///<Input V Frequency, VFreqx10, for calculate output panel timing
    MS_U16  u16InputVTotal;         ///<Input Vertical total, for calculate output panel timing
    MS_U16  u16DefaultHtotal;       ///<Default Htotal for VGA/YPbPr input
    MS_U8   u8DefaultPhase;         ///<Obsolete, use u16DefaultPhase instead

    //----------------------
    // customized scaling
    //----------------------
    MS_BOOL bHCusScaling;           ///<assign H customized scaling instead of using XC scaling
    MS_U16  u16HCusScalingSrc;      ///<H customized scaling src width
    MS_U16  u16HCusScalingDst;      ///<H customized scaling dst width
    MS_BOOL bVCusScaling;           ///<assign V manuel scaling instead of using XC scaling
    MS_U16  u16VCusScalingSrc;      ///<V customized scaling src height
    MS_U16  u16VCusScalingDst;      ///<V customized scaling dst height

    //--------------
    // 9 lattice
    //--------------
    MS_BOOL bDisplayNineLattice;    ///<used to indicate where to display in panel and where to put in frame buffer

    //----------------------
    // XC internal setting
    //----------------------

    /* scaling ratio */
    MS_U16 u16H_SizeAfterPreScaling;         ///<Horizontal size after prescaling
    MS_U16 u16V_SizeAfterPreScaling;         ///<Vertical size after prescaling
    MS_BOOL bPreV_ScalingDown;      ///<define whether it's pre-Vertical scaling down

    /* real crop win in memory */
    MS_WINDOW_TYPE ScaledCropWin;

    /* others */
    MS_U32 u32Op2DclkSet;           ///<OP to Dot clock set

    /* Video screen status */
    MS_BOOL bBlackscreenEnabled;      ///<Black screen status
    MS_BOOL bBluescreenEnabled;       ///<Blue screen status
    MS_U16 u16VideoDark;            ///<Video dark

    MS_U16 u16V_Length;             ///<for MDrv_Scaler_SetFetchNumberLimit
    MS_U16 u16BytePerWord;          ///<BytePerWord in Scaler
    MS_U16 u16OffsetPixelAlignment; ///<Pixel alignment of Offset (including IPM/OPM)
    MS_U8 u8BitPerPixel;            ///<Bits number Per Pixel
    MS_DEINTERLACE_MODE eDeInterlaceMode;       ///<De-interlace mode
    MS_U8 u8DelayLines;             ///<delay lines
    MS_BOOL bMemFmt422;             ///<whether memory format is 422, for easy usage
    MS_IMAGE_STORE_FMT eMemory_FMT; ///<memory format
    MS_BOOL bForceNRoff;            ///<Force NR off
    MS_BOOL bEn3DNR;                ///<whether it's 3DNR enabled
    MS_BOOL bUseYUVSpace;           ///< color format before 3x3 matrix
    MS_BOOL bMemYUVFmt;             ///< memroy color format
    MS_BOOL bForceRGBin;            ///<Force set format in memory as RGB (only for RGB input source)
    MS_BOOL bLinearMode;            ///<Is current memory format LinearMode?

    // frame lock related
    // only main can select FB/FBL because panel output timing depends on main window, sub will always use FB
    MS_BOOL bFBL;                   ///<whether it's framebufferless case
    MS_BOOL bFastFrameLock;         ///<whether framelock is enabled or not
    MS_BOOL bDoneFPLL;              ///<whether the FPLL is done
    MS_BOOL bEnableFPLL;            ///<enable FPLL or not
    MS_BOOL bFPLL_LOCK;             ///<whether FPLL is locked (in specific threshold.)

    // Capture_Memory
    MS_U32 u32IPMBase0;             ///<IPM base 0
    MS_U32 u32IPMBase1;             ///<IPM base 1
    MS_U32 u32IPMBase2;             ///<IPM base 2
    MS_U16 u16IPMOffset;            ///<IPM offset
    MS_U32 u16IPMFetch;             ///<IPM fetch
} XC_ApiStatus;

/// XC API status
typedef struct
{
    MS_U32 u32ApiStatusEx_Version;   ///<Version of current structure. Please always set to "PANEL_INFO_EX_VERSION" as input
    MS_U16 u16ApiStatusEX_Length;    ///<Length of this structure, u16PanelInfoEX_Length=sizeof(XC_PANEL_INFO_EX)

    //----------------------
    // Customer setting
    //----------------------
    INPUT_SOURCE_TYPE_t enInputSourceType;      ///< Input source type

    //----------------------
    // Window
    //----------------------
    MS_WINDOW_TYPE stCapWin;       ///<Capture window
    MS_WINDOW_TYPE stDispWin;      ///<Display window
    MS_WINDOW_TYPE stCropWin;      ///<Crop window

    //----------------------
    // Timing
    //----------------------
    MS_BOOL bInterlace;             ///<Interlaced or Progressive
    MS_BOOL bHDuplicate;            ///<flag for vop horizontal duplicate, for MVD, YPbPr, indicate input double sampled or not
    MS_U16  u16InputVFreq;          ///<Input V Frequency, VFreqx10, for calculate output panel timing
    MS_U16  u16InputVTotal;         ///<Input Vertical total, for calculate output panel timing
    MS_U16  u16DefaultHtotal;       ///<Default Htotal for VGA/YPbPr input
    MS_U8   u8DefaultPhase;         ///<Obsolete, use u16DefaultPhase instead

    //----------------------
    // Post customized scaling
    //----------------------
    MS_BOOL bHCusScaling;           ///<assign H customized scaling instead of using XC scaling
    MS_U16  u16HCusScalingSrc;      ///<H customized scaling src width
    MS_U16  u16HCusScalingDst;      ///<H customized scaling dst width
    MS_BOOL bVCusScaling;           ///<assign V manuel scaling instead of using XC scaling
    MS_U16  u16VCusScalingSrc;      ///<V customized scaling src height
    MS_U16  u16VCusScalingDst;      ///<V customized scaling dst height

    //--------------
    // 9 lattice
    //--------------
    MS_BOOL bDisplayNineLattice;    ///<used to indicate where to display in panel and where to put in frame buffer

    //----------------------
    // XC internal setting
    //----------------------

    /* scaling ratio */
    MS_U16 u16H_SizeAfterPreScaling;         ///<Horizontal size after prescaling
    MS_U16 u16V_SizeAfterPreScaling;         ///<Vertical size after prescaling
    MS_BOOL bPreV_ScalingDown;      ///<define whether it's pre-Vertical scaling down

    /* real crop win in memory */
    MS_WINDOW_TYPE ScaledCropWin;

    /* others */
    MS_U32 u32Op2DclkSet;           ///<OP to Dot clock set

    /* Video screen status */
    MS_BOOL bBlackscreenEnabled;      ///<Black screen status
    MS_BOOL bBluescreenEnabled;       ///<Blue screen status
    MS_U16 u16VideoDark;            ///<Video dark

    MS_U16 u16V_Length;             ///<for MDrv_Scaler_SetFetchNumberLimit
    MS_U16 u16BytePerWord;          ///<BytePerWord in Scaler
    MS_U16 u16OffsetPixelAlignment; ///<Pixel alignment of Offset (including IPM/OPM)
    MS_U8 u8BitPerPixel;            ///<Bits number Per Pixel
    MS_DEINTERLACE_MODE eDeInterlaceMode;       ///<De-interlace mode
    MS_U8 u8DelayLines;             ///<delay lines
    MS_BOOL bMemFmt422;             ///<whether memory format is 422, for easy usage
    MS_IMAGE_STORE_FMT eMemory_FMT; ///<memory format
    MS_BOOL bForceNRoff;            ///<Force NR off
    MS_BOOL bEn3DNR;                ///<whether it's 3DNR enabled
    MS_BOOL bUseYUVSpace;           ///< color format before 3x3 matrix
    MS_BOOL bMemYUVFmt;             ///< memroy color format
    MS_BOOL bForceRGBin;            ///<Force set format in memory as RGB (only for RGB input source)
    MS_BOOL bLinearMode;            ///<Is current memory format LinearMode?

    // frame lock related
    // only main can select FB/FBL because panel output timing depends on main window, sub will always use FB
    MS_BOOL bFBL;                   ///<whether it's framebufferless case
    MS_BOOL bFastFrameLock;         ///<whether framelock is enabled or not
    MS_BOOL bDoneFPLL;              ///<whether the FPLL is done
    MS_BOOL bEnableFPLL;            ///<enable FPLL or not
    MS_BOOL bFPLL_LOCK;             ///<whether FPLL is locked (in specific threshold.)

    // Capture_Memory
    MS_U32 u32IPMBase0;             ///<IPM base 0
    MS_U32 u32IPMBase1;             ///<IPM base 1
    MS_U32 u32IPMBase2;             ///<IPM base 2
    MS_U16 u16IPMOffset;            ///<IPM offset
    MS_U32 u16IPMFetch;             ///<IPM fetch

    //----------------------
    // Pre customized scaling
    //----------------------
    MS_BOOL bPreHCusScaling;           ///<assign H customized scaling instead of using XC scaling
    MS_U16  u16PreHCusScalingSrc;      ///<H customized scaling src width
    MS_U16  u16PreHCusScalingDst;      ///<H customized scaling dst width
    MS_BOOL bPreVCusScaling;           ///<assign V manuel scaling instead of using XC scaling
    MS_U16  u16PreVCusScalingSrc;      ///<V customized scaling src height
    MS_U16  u16PreVCusScalingDst;      ///<V customized scaling dst height

    MS_BOOL bPQSetHSD;///<H_SizeAfterPreScaling is changed manually, which will skip PQ HSD sampling and filter

    MS_U16  u16DefaultPhase;           ///<Default Phase for VGA/YPbPr input
    MS_BOOL bIsHWDepthAdjSupported;    ///<Is hw depth adjustment for real 3d supported or not
    MS_BOOL bIs2LineMode;              ///<Is 2 line mode or not
    MS_BOOL bIsPNLYUVOutput;         ///<Is Scaler YUV output

    MS_U8  u8HDMIPixelRepetition;   ///<the HDMI pixel repetition info

    MS_U32 u32FRCMBase0;             ///<FRCM base 0
    MS_U32 u32FRCMBase1;             ///<FRCM base 1
    MS_U32 u32FRCMBase2;             ///<FRCM base 2
#ifdef UFO_XC_ZORDER
    E_VIDEO_ON_OSD_LAYER enZOrderIndex;   ///<VIDEO and the relative position of OSD.
#endif

    MS_BOOL bFSCEnabled;             ///< used only for FSC enbaled case
    MS_BOOL bFRCEnabled;             ///< used only for FRC enbaled case
} XC_ApiStatusEx;

/// XC API status
typedef struct
{
    MS_U32 u32ApiStatusEx_Version;   ///<Version of current structure. Please always set to "PANEL_INFO_EX_VERSION" as input
    MS_U16 u16ApiStatusEX_Length;    ///<Length of this structure, u16PanelInfoEX_Length=sizeof(XC_PANEL_INFO_EX)

    //----------------------
    // Customer setting
    //----------------------
    INPUT_SOURCE_TYPE_t enInputSourceType;      ///< Input source type

    //----------------------
    // Window
    //----------------------
    MS_WINDOW_TYPE stDispWin;      ///<Display window

} ST_XC_APISTATUSNODELAY;

/// Define the set timing information
typedef struct
{
    MS_U32  u32HighAccurateInputVFreq;      ///<high accurate input V frequency
    MS_U16  u16InputVFreq;                  ///<input V frequency
    MS_U16  u16InputVTotal;                 ///<input vertical total
    MS_BOOL bMVOPSrc;                       ///<MVOP source
    MS_BOOL bFastFrameLock;                 ///<whether it's fast frame lock case
    MS_BOOL bInterlace;                     ///<whether it's interlace
} XC_SetTiming_Info;

//------------------------------
// XC Auto
//------------------------------

/// internal calibration
typedef struct
{
    MS_U16  u16CH_AVG[3];
} MS_AUTOADC_TYPE;

/// get IP1 report min/max R/G/B
typedef enum
{
    AUTO_MIN_R,
    AUTO_MIN_G,
    AUTO_MIN_B,
    AUTO_MAX_R,
    AUTO_MAX_G,
    AUTO_MAX_B,
} AUTO_GAIN_TYPE;

/// the tming types for internal calibration
typedef enum
{
    E_XC_OFF,
    E_XC_480P,
    E_XC_576P,
    E_XC_720P,
    E_XC_1080P,
    E_XC_480I,
    E_XC_576I,
    E_XC_720I,
    E_XC_1080I,
} XC_Internal_TimingType;

//------------------------------
// Software Reset
//------------------------------
/// software reset type
typedef enum
{
    REST_SCALER_ALL  = _BIT(0),
    REST_IP_F1       = _BIT(1),
    REST_IP_F2       = _BIT(2),
    REST_OP          = _BIT(3),
    REST_IP_ALL      = _BIT(4),
    REST_CLK         = (_BIT(6) | _BIT(3)),
} SOFTWARE_REST_TYPE_t;

/// Detect Europe HDTV status type
typedef enum
{
    EURO_AUS_HDTV_NORMAL = 0,         ///<not detected Euro or Aus HDTV
    EURO_HDTV_DETECTED,               ///<Euro HDTV detected
    AUSTRALIA_HDTV_DETECTED,          ///<Aus HDTV detected
} MS_DETECT_EURO_HDTV_STATUS_TYPE;


//Capture_Memory

/// Define the rectangle information
typedef struct
{
    MS_S16 s16X_Start;      ///<rectangle start x
    MS_S16 s16Y_Start;      ///<rectangle start x
    MS_S16 s16X_Size;       ///<rectangle horizontal size
    MS_S16 s16Y_Size;       ///<rectangle vertical size

    MS_U8 *pRect;           ///<the buffer address for the rectangle
    MS_U32 u32RectPitch;    ///<rectangle pitch
    MS_U16 u16PointSize;    ///<point size, consider the memory size
} MS_RECT_INFO;

/// Define for format of pixel 24 bit
typedef struct
{
    MS_U8 R_Cr;
    MS_U8 G_Y;
    MS_U8 B_Cb;
} MS_PIXEL_24BIT;

/// Define for format of pixel 32 bit
typedef struct
{
    MS_U32 B_Cb   : 10;
    MS_U32 G_Y    : 10;
    MS_U32 R_Cr   : 10;
    MS_U32 u8Dummy: 2;
} MS_PIXEL_32BIT;

/// Define for format of pixel 16 bit
typedef struct
{
    MS_U8 u8G_Y;
    MS_U8 u8BR_CbCr;
} MS_PIXEL_422_8BIT;

typedef enum
{
    E_XC_OUTPUTDATA_RGB10BITS = 0x00, ///< Dummy[31:30]  R[29:20]  G[19:10] B[9:0]
    E_XC_OUTPUTDATA_RGB8BITS = 0x01, ///<   R[23:16]  G[15:8] B[7:0]
    E_XC_OUTPUTDATA_4228BITS = 0x03, ///<   CbCr[15:8] Y[7:0]

    E_XC_OUTPUTDATA_FRC_RGB10BITS = 0x04, ///< Dummy[31:30]  R[29:20]  G[19:10] B[9:0]
    E_XC_OUTPUTDATA_FRC_RGB8BITS = 0x05, ///<   R[23:16]  G[15:8] B[7:0]
    E_XC_OUTPUTDATA_FRC_4228BITS = 0x06, ///<   CbCr[15:8] Y[7:0]
} E_XC_OUTPUTDATA_TYPE;

typedef enum
{
    E_XC_INPUTDATA_RGB10BITS = 0x00, ///< Dummy[31:30]  R[29:20]  G[19:10] B[9:0]
    E_XC_INPUTDATA_RGB8BITS = 0x01, ///<   R[23:16]  G[15:8] B[7:0]
    E_XC_INPUTDATA_4228BITS = 0x03, ///<   CbCr[15:8] Y[7:0]
} E_XC_INPUTDATA_TYPE;


/// define for the auto calbration window type
typedef struct
{
    MS_U16 u16X;        ///<window start x
    MS_U16 u16Xsize;    ///<window horizontal size
    MS_U16 u16Y;        ///<window start y
    MS_U16 u16Ysize;    ///<window vertical size
} MS_AUTO_CAL_WINDOW_t;

typedef struct
{
    MS_IMAGE_STORE_FMT eStoreFmt;    ///<image store format
    MS_AUTO_CAL_WINDOW_t eCal_Win_Size;    ///<auto calbration window type
    MS_AUTOADC_TYPE pixelMax;    ///<maximum value among the pixels
    MS_AUTOADC_TYPE pixelMin;    ///<minimum value among the pixels
    MS_AUTOADC_TYPE pixelAverage;    ///<average value among the pixels
} MS_IMAGE_MEM_INFO;

/// defined channel select for contrast adjusting.
typedef enum
{
    XC_VOP_CHR = 0,    ///< Channel R
    XC_VOP_CHG,        ///< Channel G
    XC_VOP_CHB,        ///< Channel B
    XC_VOP_ALL,        ///< All Channel
} MS_XC_VOP_CHANNEL_t;

/// Define Scaler GOP IP setting.
typedef enum
{
    MS_IP0_SEL_GOP0,
    MS_IP0_SEL_GOP1,
    MS_IP0_SEL_GOP2,
    MS_NIP_SEL_GOP0,
    MS_NIP_SEL_GOP1,
    MS_NIP_SEL_GOP2,
    MS_MVOP_SEL,
} MS_XC_IPSEL_GOP;

/// Define Scaler memory format
typedef enum
{
    E_MS_XC_MEM_FMT_AUTO = 0,
    E_MS_XC_MEM_FMT_444  = 1,
    E_MS_XC_MEM_FMT_422  = 2,
} MS_XC_MEM_FMT;

/// Define the PQ path information
/**
 *  The active Window for PQ function to takes place.
 */
typedef enum
{
    /// Main window
    PQ_MAIN_WINDOW = 0,
    /// Sub window
    PQ_SUB_WINDOW = 1,
    /// The max support window of PQ
    PQ_MAX_WINDOW
} PQ_WIN;

typedef enum
{
    PQ_XC_ID_0,
    PQ_XC_ID_1,
    PQ_XC_ID_NUM,
} PQ_XC_ID;

/**
 *  FOURCE COLOR FMT
 */
typedef enum
{
    /// Default
    PQ_FOURCE_COLOR_DEFAULT,
    /// RGB
    PQ_FOURCE_COLOR_RGB,
    /// YUV
    PQ_FOURCE_COLOR_YUV,
} PQ_FOURCE_COLOR_FMT;

/**
 * PQ DEINTERLACE MODE
 */
typedef enum
{
    /// off
    PQ_DEINT_OFF = 0,
    /// 2DDI BOB
    PQ_DEINT_2DDI_BOB,
    /// 2DDI AVG
    PQ_DEINT_2DDI_AVG,
    /// 3DDI History(24 bit)
    PQ_DEINT_3DDI_HISTORY,
    /// 3DDI(16 bit)
    PQ_DEINT_3DDI,
} PQ_DEINTERLACE_MODE;
/**
 *  Video data information to supply when in PQ mode.
 */
typedef struct
{
    /// is FBL or not
    MS_BOOL bFBL;
    /// is interlace mode or not
    MS_BOOL bInterlace;
    /// input Horizontal size
    MS_U16  u16input_hsize;
    /// input Vertical size
    MS_U16  u16input_vsize;
    /// input Vertical total
    MS_U16  u16input_vtotal;
    /// input Vertical frequency
    MS_U16  u16input_vfreq;
    /// output Vertical frequency
    MS_U16  u16ouput_vfreq;
    /// Display Horizontal size
    MS_U16  u16display_hsize;
    /// Display Vertical size
    MS_U16  u16display_vsize;
} MS_PQ_Mode_Info;
/**
 *  INPUT SOURCE TYPE
 */
typedef enum
{
    /// VGA
    PQ_INPUT_SOURCE_VGA,
    /// TV
    PQ_INPUT_SOURCE_TV,

    /// CVBS
    PQ_INPUT_SOURCE_CVBS,

    /// S-video
    PQ_INPUT_SOURCE_SVIDEO,

    /// Component
    PQ_INPUT_SOURCE_YPBPR,
    /// Scart
    PQ_INPUT_SOURCE_SCART,


    /// HDMI
    PQ_INPUT_SOURCE_HDMI,

    /// DTV
    PQ_INPUT_SOURCE_DTV,

    /// DVI
    PQ_INPUT_SOURCE_DVI,

    // Application source
    /// Storage
    PQ_INPUT_SOURCE_STORAGE,
    /// KTV
    PQ_INPUT_SOURCE_KTV,
    /// JPEG
    PQ_INPUT_SOURCE_JPEG,

    /// The max support number of PQ input source
    PQ_INPUT_SOURCE_NUM,
    /// None
    PQ_INPUT_SOURCE_NONE = PQ_INPUT_SOURCE_NUM,
} PQ_INPUT_SOURCE_TYPE;


// PQ Function
typedef enum
{
    E_PQ_IOCTL_NONE             = 0x00000000,
    E_PQ_IOCTL_HSD_SAMPLING     = 0x00000001,
    E_PQ_IOCTL_PREVSD_BILINEAR  = 0x00000002,
    E_PQ_IOCTL_ADC_SAMPLING     = 0x00000003,
    E_PQ_IOCTL_RFBL_CTRL        = 0x00000004,
    E_PQ_IOCTL_PQ_SUGGESTED_FRAMENUM        = 0x00000008,
    E_PQ_IOCTL_SET_UCFEATURE                = 0x00000010,
    E_PQ_IOCTL_PQ_SUGGESTED_FRCMFRAMENUM    = 0x00000020,

    E_PQ_IOCTL_NUM,
} PQ_IOCTL_FLAG_TYPE;

typedef struct   s_PQ_Function
{

    void (* pq_disable_filmmode)(PQ_WIN eWindow, MS_BOOL bOn);

    MS_BOOL (* pq_load_scalingtable)(PQ_WIN eWindow, MS_U8 eXRuleType, MS_BOOL bPreV_ScalingDown,
                                     MS_BOOL bInterlace, MS_BOOL bColorSpaceYUV, MS_U16 u16InputSize, MS_U16 u16SizeAfterScaling);

    MS_BOOL (* pq_set_csc)(PQ_WIN eWindow, PQ_FOURCE_COLOR_FMT enFourceColor);

    PQ_DEINTERLACE_MODE (* pq_set_memformat)(PQ_WIN eWindow, MS_BOOL bMemFmt422, MS_BOOL bFBL, MS_U8 *u8BitsPerPixel);

    void (* pq_set_420upsampling)(PQ_WIN eWindow, MS_BOOL bFBL, MS_BOOL bPreV_ScalingDown, MS_U16 u16V_CropStart);

    void (* pq_set_modeInfo)(PQ_WIN eWindow, PQ_INPUT_SOURCE_TYPE enInputSourceType, MS_PQ_Mode_Info *pstPQModeInfo);

    void (* pq_deside_srctype)(PQ_WIN eWindow, PQ_INPUT_SOURCE_TYPE enInputSourceType);

    MS_BOOL (* pq_get_memyuvfmt)(PQ_WIN eWindow, PQ_FOURCE_COLOR_FMT enFourceColor);

    MS_BOOL (* pq_ioctl)(PQ_WIN eWindow, MS_U32 u32Flag, void *pBuf, MS_U32 u32BufSize);

    void (* pq_disable_filmmode_ex)(PQ_XC_ID eXCID, PQ_WIN eWindow, MS_BOOL bOn);

    MS_BOOL (* pq_load_scalingtable_ex)(PQ_XC_ID eXCID, PQ_WIN eWindow, MS_U8 eXRuleType, MS_BOOL bPreV_ScalingDown,
                                        MS_BOOL bInterlace, MS_BOOL bColorSpaceYUV, MS_U16 u16InputSize, MS_U16 u16SizeAfterScaling);

    MS_BOOL (* pq_set_csc_ex)(PQ_XC_ID eXCID, PQ_WIN eWindow, PQ_FOURCE_COLOR_FMT enFourceColor);

    PQ_DEINTERLACE_MODE (* pq_set_memformat_ex)(PQ_XC_ID eXCID, PQ_WIN eWindow, MS_BOOL bMemFmt422, MS_BOOL bFBL, MS_U8 *u8BitsPerPixel);

    void (* pq_set_420upsampling_ex)(PQ_XC_ID eXCID, PQ_WIN eWindow, MS_BOOL bFBL, MS_BOOL bPreV_ScalingDown, MS_U16 u16V_CropStart);

    void (* pq_set_modeInfo_ex)(PQ_XC_ID eXCID, PQ_WIN eWindow, PQ_INPUT_SOURCE_TYPE enInputSourceType, MS_PQ_Mode_Info *pstPQModeInfo);

    void (* pq_deside_srctype_ex)(PQ_XC_ID eXCID, PQ_WIN eWindow, PQ_INPUT_SOURCE_TYPE enInputSourceType);

    MS_BOOL (* pq_get_memyuvfmt_ex)(PQ_XC_ID eXCID, PQ_WIN eWindow, PQ_FOURCE_COLOR_FMT enFourceColor);

    MS_BOOL (* pq_ioctl_ex)(PQ_XC_ID eXCID, PQ_WIN eWindow, MS_U32 u32Flag, void *pBuf, MS_U32 u32BufSize);
} PQ_Function_Info;

typedef struct
{
    MS_U32 u32ratio;
    MS_BOOL bADVMode;
} PQ_HSD_SAMPLING_INFO;

typedef struct
{
    PQ_INPUT_SOURCE_TYPE enPQSourceType;
    MS_PQ_Mode_Info stPQModeInfo;
    MS_U16 u16ratio;
} PQ_ADC_SAMPLING_INFO;

//Display information
typedef struct
{
    MS_U32 VDTOT; //Output vertical total
    MS_U32 DEVST; //Output DE vertical start
    MS_U32 DEVEND;//Output DE Vertical end
    MS_U32 HDTOT;// Output horizontal total
    MS_U32 DEHST; //Output DE horizontal start
    MS_U32 DEHEND;// Output DE horizontal end
    MS_BOOL bInterlaceMode;
    MS_BOOL bYUVInput;
} MS_XC_DST_DispInfo;

typedef enum
{
    ///IP2 path.
    E_GOP_XCDST_IP2 = 0,

    /// IP man path.
    E_GOP_XCDST_IP1_MAIN = 1,

    ///IP sub path.
    E_GOP_XCDST_IP1_SUB = 2,

    /// op1 main path.
    E_GOP_XCDST_OP1_MAIN = 3,

    MAX_GOP_XCDST_SUPPORT

} EN_GOP_XCDST_TYPE;

/// Set data mux to VE
typedef enum
{
    /// GOP mux0 to VE
    E_VOP_SEL_OSD_BLEND0,
    /// GOP mux1 to VE
    E_VOP_SEL_OSD_BLEND1,
    /// GOP mux2 to VE
    E_VOP_SEL_OSD_BLEND2,
    /// mux from XC MACE
    E_VOP_SEL_MACE_RGB,

    E_VOP_SEL_OSD_NONE,
    /// Last GOP mux to VE
    E_VOP_SEL_OSD_LAST = 0x80,
} EN_VOP_SEL_OSD_XC2VE_MUX;

// 3D info
typedef enum
{
    //range [0000 ~ 1111] reserved for hdmi 3D spec
    E_XC_3D_INPUT_FRAME_PACKING                     = 0x00, //0000
    E_XC_3D_INPUT_FIELD_ALTERNATIVE                 = 0x01, //0001
    E_XC_3D_INPUT_LINE_ALTERNATIVE                  = 0x02, //0010
    E_XC_3D_INPUT_SIDE_BY_SIDE_FULL                 = 0x03, //0011
    E_XC_3D_INPUT_L_DEPTH                           = 0x04, //0100
    E_XC_3D_INPUT_L_DEPTH_GRAPHICS_GRAPHICS_DEPTH   = 0x05, //0101
    E_XC_3D_INPUT_TOP_BOTTOM                        = 0x06, //0110
    E_XC_3D_INPUT_SIDE_BY_SIDE_HALF                 = 0x08, //1000

    E_XC_3D_INPUT_CHECK_BORAD                       = 0x09, //1001

    //user defined
    E_XC_3D_INPUT_MODE_USER                         = 0x10,
    E_XC_3D_INPUT_MODE_NONE                         = E_XC_3D_INPUT_MODE_USER,
    E_XC_3D_INPUT_FRAME_ALTERNATIVE,
    E_XC_3D_INPUT_SIDE_BY_SIDE_HALF_INTERLACE,
    // optimize for format: in:E_XC_3D_INPUT_FRAME_PACKING, out:E_XC_3D_OUTPUT_TOP_BOTTOM,E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF
    E_XC_3D_INPUT_FRAME_PACKING_OPT,
    // optimize for format: in:E_XC_3D_INPUT_TOP_BOTTOM, out:E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF
    E_XC_3D_INPUT_TOP_BOTTOM_OPT,
    E_XC_3D_INPUT_NORMAL_2D,
    E_XC_3D_INPUT_NORMAL_2D_INTERLACE,
    E_XC_3D_INPUT_NORMAL_2D_INTERLACE_PTP,
    E_XC_3D_INPUT_SIDE_BY_SIDE_HALF_INTERLACE_OPT,
    E_XC_3D_INPUT_NORMAL_2D_HW,  //for hw 2D to 3D use
    E_XC_3D_INPUT_PIXEL_ALTERNATIVE,
} E_XC_3D_INPUT_MODE;


typedef enum
{
    E_XC_3D_OUTPUT_MODE_NONE,
    E_XC_3D_OUTPUT_LINE_ALTERNATIVE,
    E_XC_3D_OUTPUT_TOP_BOTTOM,
    E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF,
    E_XC_3D_OUTPUT_FRAME_ALTERNATIVE,       //25-->50,30-->60,24-->48,50-->100,60-->120----FRC 1:2
    E_XC_3D_OUTPUT_FRAME_L,
    E_XC_3D_OUTPUT_FRAME_R,
    E_XC_3D_OUTPUT_FRAME_ALTERNATIVE_NOFRC,  //50->50, 60->60-------------------------------FRC 1:1
    E_XC_3D_OUTPUT_CHECKBOARD_HW,       //for hw 2d to 3d use
    E_XC_3D_OUTPUT_LINE_ALTERNATIVE_HW,  //for hw 2d to 3d use
    E_XC_3D_OUTPUT_PIXEL_ALTERNATIVE_HW, //for hw 2d to 3d use
    E_XC_3D_OUTPUT_FRAME_L_HW,          //for hw 2d to 3d use
    E_XC_3D_OUTPUT_FRAME_R_HW,          //for hw 2d to 3d use
    E_XC_3D_OUTPUT_FRAME_ALTERNATIVE_HW, //for hw 2d to 3d use
    E_XC_3D_OUTPUT_TOP_BOTTOM_HW,       //for hw 2d to 3d use, it based on E_XC_3D_OUTPUT_TOP_BOTTOM implement
    E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF_HW, //for hw 2d to 3d use, it based on E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF implement
    E_XC_3D_OUTPUT_FRAME_PACKING,
    E_XC_3D_OUTPUT_FRAME_ALTERNATIVE_LLRR,//for 4k0.5k@240 3D
    E_XC_3D_OUTPUT_FRAME_ALTERNATIVE_LLRR_HW,//for HW 2Dto3D 4k0.5k@240 3D
    E_XC_3D_OUTPUT_TOP_TOP,
    E_XC_3D_OUTPUT_BOTTOM_BOTTOM,
    E_XC_3D_OUTPUT_LEFT_LEFT,
    E_XC_3D_OUTPUT_RIGHT_RIGHT,
} E_XC_3D_OUTPUT_MODE;

typedef enum
{
    E_XC_3D_OUTPUT_FI_MODE_NONE,
    E_XC_3D_OUTPUT_FI_1920x1080,
    E_XC_3D_OUTPUT_FI_960x1080,
    E_XC_3D_OUTPUT_FI_1920x540,

    E_XC_3D_OUTPUT_FI_1280x720,
} E_XC_3D_OUTPUT_FI_MODE;

typedef enum
{
    E_XC_3D_AUTODETECT_SW,
    E_XC_3D_AUTODETECT_HW,
    E_XC_3D_AUTODETECT_HW_COMPATIBLE,
    E_XC_3D_AUTODETECT_MAX
} E_XC_3D_AUTODETECT_METHOD;

typedef enum
{
    E_XC_3D_PANEL_NONE,
    E_XC_3D_PANEL_SHUTTER,      //240hz panel, which can process updown, leftright,vertical or horizontal line weave
    E_XC_3D_PANEL_PELLICLE,     //120hz panel, which can only process horizontal line weave
    E_XC_3D_PANEL_4K1K_SHUTTER, //120hz 4K1K panel, which can process updown, leftright,vertical or horizontal line weave
    E_XC_3D_PANEL_MAX,
} E_XC_3D_PANEL_TYPE;

//hw 2d to 3d para
typedef struct __attribute__((packed))
{
    MS_U32  u32Hw2dTo3dPara_Version;
    MS_U16  u16Concave;
    MS_U16  u16Gain;
    MS_U16  u16Offset;
    MS_U16  u16ArtificialGain;
    MS_U16  u16EleSel;
    MS_U16  u16ModSel;
    MS_U16  u16EdgeBlackWidth;
}
MS_XC_3D_HW2DTO3D_PARA;

//detect 3d format para
typedef struct
{
    MS_U32  u32Detect3DFormatPara_Version;  //version control, back compatible
    //////////////////obsolete field begin///////////////////////////////////////////////////////////////////////////////////////////////////////////
    MS_U16  u16HorSearchRange;              //the horizontal range for find the similar pixel at R image
    MS_U16  u16VerSearchRange;              //the vertical range for find the similar pixel at R image
    MS_U16  u16GYPixelThreshold;            //g/y pixel threshold for define the similar pixel
    MS_U16  u16RCrPixelThreshold;           //r/cr pixel threshold for define the similar pixel
    MS_U16  u16BCbPixelThreshold;           //b/cb pixel threshold for define the similar pixel
    MS_U16  u16HorSampleCount;              //the horizontal sample count, the total checked pixel will be u16HorSampleCount*u16VerSampleCount
    MS_U16  u16VerSampleCount;              //the vertical sample count, the total checked pixel will be u16HorSampleCount*u16VerSampleCount
    MS_U16  u16MaxCheckingFrameCount;       //the max checking frame count
    MS_U16  u16HitPixelPercentage;          //the percentage about hit pixel in one frame total checked pixel, for example: 70%, need set as 70
    //////////////////obsolete field end/////////////////////////////////////////////////////////////////////////////////////////////////////////////
    MS_BOOL bEnableOverscan;                //detect 3d format for considering overscan shift
} MS_XC_3D_DETECT3DFORMAT_PARA;

//define 3d fp info para
typedef struct
{
    MS_U32  u32FPInfoPara_Version;          //version control, back compatible
    MS_U16  u16HBlank;                      //horizontal blank
    MS_BOOL bEnableUserSetting;             //enable user setting
} MS_XC_3D_FPINFO_PARA;

typedef enum
{
    DBG_FPLL_MODE_DISABLE_ALL = 0,
    DBG_FPLL_MODE_DISABLE     = 1,
    DBG_FPLL_MODE_ENABLE      = 2,
    DBG_FPLL_MODE_MAX,
} EN_FPLL_DBG_MODE; //For debug purpose only!

typedef enum
{
    DBG_FPLL_FLAG_PHASELIMIT  = 0,
    DBG_FPLL_FLAG_D5D6D7      = 1,
    DBG_FPLL_FLAG_IGAIN       = 2,
    DBG_FPLL_FLAG_PGAIN       = 3,
    DBG_FPLL_FLAG_INITSETSTEP = 4,
    DBG_FPLL_FLAG_INITSETDELAY = 5,
    DBG_FPLL_FLAG_MAX,
} EN_FPLL_DBG_FLAG; //For debug purpose only!

typedef enum
{
    E_FPLL_MODE_DISABLE_ALL = 0, /// Disable all current FPLL customer setting(then scaler will auto decide it)
    E_FPLL_MODE_DISABLE     = 1, /// Disable the specified(by other function parameter) FPLL customer setting
    E_FPLL_MODE_ENABLE      = 2, /// Enable the specified(by other function parameter) FPLL customer setting
    E_FPLL_MODE_MAX,
} EN_FPLL_MODE;

typedef enum
{
    E_FPLL_FLAG_PHASELIMIT  = 0, ///Set customer setting of PHASE limit
    E_FPLL_FLAG_D5D6D7      = 1,///Set customer setting of D5D6D7 limit
    E_FPLL_FLAG_IGAIN       = 2,///Set customer setting of IGAIN
    E_FPLL_FLAG_PGAIN       = 3,///Set customer setting of PGAIN
    E_FPLL_FLAG_INITSETSTEP = 4, ///steps to set DCLK
    E_FPLL_FLAG_INITSETDELAY = 5, ///delay between steps when setting DCLK
    E_FPLL_FLAG_MAX,
} EN_FPLL_FLAG;

typedef enum
{
    E_MLOAD_UNSUPPORTED = 0,
    E_MLOAD_DISABLED    = 1,
    E_MLOAD_ENABLED     = 2,
} MLOAD_TYPE;

typedef enum
{
    E_MLCLIENT_MAIN_HDR = 0,

    E_MLCLIENT_MAX,
} EN_MLOAD_CLIENT_SELECT;

typedef enum
{
    E_MLG_UNSUPPORTED = 0,
    E_MLG_DISABLED    = 1,
    E_MLG_ENABLED     = 2,
} MLG_TYPE;

typedef enum
{
    HDMI_SYNC_DE,
    HDMI_SYNC_HV,
} E_HDMI_SYNC_TYPE;

typedef enum
{
    MS_FRC_1_1  = 0,
    MS_FRC_1_2  = 1,
    MS_FRC_5_12 = 2,
    MS_FRC_2_5  = 3,
    MS_FRC_1_4  = 4,
    MS_FRC_1_3  = 5,

    MS_FRC_2_1  = 8,
} MS_FRC_TYPE;

// scaler FRC table
typedef struct __attribute__((packed))
{
    MS_U16 u16LowerBound;
    MS_U16 u16HigherBound;
    MS_U8  u8Padding;
    MS_U8  u8FRC_In;          // ivs
    MS_U8  u8FRC_Out;         // ovs
    MS_FRC_TYPE eFRC_Type;
}
SC_FRC_SETTING;

typedef enum
{
    E_PQ_IOCTL_GET_HSD_SAMPLING,
    E_PQ_IOCTL_MAX,
} E_PQ_IOCTL_FLAG;


typedef enum
{
    E_GET_PIXEL_RET_FAIL = 0,       ///<Fail
    E_GET_PIXEL_RET_OK,             ///<OK
    E_GET_PIXEL_RET_OUT_OF_RANGE,   ///< out of range
} E_APIXC_GET_PixelRGB_ReturnValue;


typedef enum
{
    E_XC_GET_PIXEL_STAGE_AFTER_DLC = 0x01,
    E_XC_GET_PIXEL_STAGE_PRE_GAMMA = 0x02,
    E_XC_GET_PIXEL_STAGE_AFTER_OSD = 0x03,
    E_XC_GET_PIXEL_STAGE_MAX       = 0xFF,
} EN_XC_GET_PIXEL_RGB_STAGE;

typedef struct
{
    EN_XC_GET_PIXEL_RGB_STAGE enStage;
    MS_U16 u16x;
    MS_U16 u16y;
    MS_U32 u32r;
    MS_U32 u32g;
    MS_U32 u32b;
} XC_Get_Pixel_RGB;

typedef struct
{
    MS_U32 u32ReportPixelInfo_Version;   ///<Input: Version of current structure. Please always set to "XC_REPORT_PIXELINFO_VERSION" as input
    MS_U16 u16ReportPixelInfo_Length;    ///<Input: Length of this structure, u16ReportPixelInfo_Length=sizeof(MS_XC_REPORT_PIXELINFO)
    EN_XC_GET_PIXEL_RGB_STAGE enStage;   ///<Input: Pixel info report stage
    MS_U16 u16RepWinColor;               ///<Input:Report window Color
    MS_U16 u16XStart;                    ///<Input: X start location of report window
    MS_U16 u16XEnd;                      ///<Input: X end location of report window
    MS_U16 u16YStart;                    ///<Input: Y start location of report window
    MS_U16 u16YEnd;                      ///<Input: Y end location of report window
    MS_U16 u16RCrMin;                    ///<Output:R or Cr min value
    MS_U16 u16RCrMax;                    ///<Output:R or Cr max value
    MS_U16 u16GYMin;                     ///<Output:G or Y min value
    MS_U16 u16GYMax;                     ///<Output:G or Y max value
    MS_U16 u16BCbMin;                    ///<Output:B or Cb min value
    MS_U16 u16BCbMax;                    ///<Output:B or Cb max value
    MS_U32 u32RCrSum;                    ///<Output:R or Cr sum value
    MS_U32 u32GYSum;                     ///<Output:G or Y sum value
    MS_U32 u32BCbSum;                    ///<Output:B or Cb sum value
    MS_BOOL bShowRepWin;                 ///<Input: Show report window or not
} MS_XC_REPORT_PIXELINFO;

// scaler interrupt sources
typedef enum
{
    // 0x00, first 4 bits are unused
    SC_INT_DIPW = 1,					//DIPW write one frame done interrupt
    SC_INT_MEMSYNC_MAIN = 3,
    SC_INT_START = 4,
    SC_INT_RESERVED1 = SC_INT_START,    // before is SC_INT_TUNE_FAIL_P, FBL line buffer overrun/underrun
    // scaler dosen't have this interrupt now,

    SC_INT_VSINT,                       // output Vsync interrupt, can select polarity with BK0_04[1]
    SC_INT_F2_VTT_CHG,                  // main window, HDMI mute or Vsync polarity changed, Vtt change exceed BK1_1D[11:8]
    SC_INT_F1_VTT_CHG,
    SC_INT_F2_VS_LOSE,                  // didn't received Vsync for a while or Vtt count BK1_1F[10:0] exceed max value
    SC_INT_F1_VS_LOSE,
    SC_INT_F2_JITTER,                   // H/V start/end didn't be the same with privous value, usually used in HDMI/DVI input
    SC_INT_F1_JITTER,
    SC_INT_F2_IPVS_SB,                  // input V sync interrupt, can select which edge to trigger this interrupt with BK0_04[0]
    SC_INT_F1_IPVS_SB,
    SC_INT_F2_IPHCS_DET,                // input H sync interrupt
    SC_INT_F1_IPHCS_DET,

    // 0x10
    SC_INT_PWM_RP_L_INT,                // pwm rising edge of left frame
    SC_INT_PWM_FP_L_INT,                // pwm falling edge of left frame
    SC_INT_F2_HTT_CHG,                  // Hsync polarity changed or Hperiod change exceed BK1_1D[5:0]
    SC_INT_F1_HTT_CHG,
    SC_INT_F2_HS_LOSE,                  // didn't received H sync for a while or Hperiod count BK1_20[13:0] exceed max value
    SC_INT_F1_HS_LOSE,
    SC_INT_PWM_RP_R_INT,                // pwm rising edge of right frame
    SC_INT_PWM_FP_R_INT,                // pwm falling edge of right frame
    SC_INT_F2_CSOG,                     // composite sync or SoG input signal type changed (for example, SoG signal from none -> valid, valid -> none)
    SC_INT_F1_CSOG,
    SC_INT_F2_RESERVED2,                // scaler dosen't have this interrupt now, before is SC_INT_F2_ATS_READY
    SC_INT_F1_RESERVED2,                // scaler dosen't have this interrupt now, before is SC_INT_F1_ATS_READY
    SC_INT_F2_ATP_READY,                // auto phase ready interrupt
    SC_INT_F1_ATP_READY,
    SC_INT_F2_RESERVED3,                // scaler dosen't have this interrupt now, before is SC_INT_F2_ATG_READY
    SC_INT_F1_RESERVED3,                // scaler dosen't have this interrupt now, before is SC_INT_F1_ATG_READY

    MAX_SC_INT,
} SC_INT_SRC;

typedef enum
{
    // 0x00, first 4 bits are unused
    XC_INT_RESERVED1 = 4,               ///< scaler dosen't have this interrupt now
    XC_INT_VSINT,                       ///< output Vsync interrupt
    XC_INT_F2_VTT_CHG,                  ///< main window, HDMI mute or Vsync polarity changed, Vtt change exceed BK1_1D[11:8]
    XC_INT_F1_VTT_CHG,
    XC_INT_F2_VS_LOSE,                  ///< didn't received Vsync for a while or Vtt count BK1_1F[10:0] exceed max value
    XC_INT_F1_VS_LOSE,
    XC_INT_F2_JITTER,                   ///< H/V start/end didn't be the same with privous value, usually used in HDMI/DVI input
    XC_INT_F1_JITTER,
    XC_INT_F2_IPVS_SB,                  ///< input V sync interrupt
    XC_INT_F1_IPVS_SB,
    XC_INT_F2_IPHCS_DET,                ///< input H sync interrupt
    XC_INT_F1_IPHCS_DET,

    // 0x10
    XC_INT_PWM_RP_L_INT,                ///< pwm rising edge of left frame, please use MDrv_XC_InterruptAvaliable() to check if avalible or not
    XC_INT_PWM_FP_L_INT,                ///< pwm falling edge of left frame, please use MDrv_XC_InterruptAvaliable() to check if avalible or not
    XC_INT_F2_HTT_CHG,                  ///< Hsync polarity changed or Hperiod change exceed BK1_1D[5:0]
    XC_INT_F1_HTT_CHG,
    XC_INT_F2_HS_LOSE,                  ///< didn't received H sync for a while or Hperiod count BK1_20[13:0] exceed max value
    XC_INT_F1_HS_LOSE,
    XC_INT_PWM_RP_R_INT,                ///< pwm rising edge of right frame, please use MDrv_XC_InterruptAvaliable() to check if avalible or not
    XC_INT_PWM_FP_R_INT,                ///< pwm falling edge of right frame, please use MDrv_XC_InterruptAvaliable() to check if avalible or not
    XC_INT_F2_CSOG,                     ///< composite sync or SoG input signal type changed (for example, SoG signal from none -> valid, valid -> none)
    XC_INT_F1_CSOG,
    XC_INT_F2_RESERVED2,                ///< scaler dosen't have this interrupt now
    XC_INT_F1_RESERVED2,
    XC_INT_F2_ATP_READY,                ///< auto phase ready interrupt
    XC_INT_F1_ATP_READY,
    XC_INT_F2_RESERVED3,                ///< scaler dosen't have this interrupt now
    XC_INT_F1_RESERVED3,
} XC_INT_SRC;

/// OP2 Video/GOP layer switch
typedef enum
{
    E_VOP_LAYER_VIDEO_MUX1_MUX2 = 0,        ///<0: Video->GOP1->GOP2 (GOP2 is one pipe later than GOP1)
    E_VOP_LAYER_FRAME_VIDEO_MUX1_MUX2,      ///<1: FrameColor->Video->GOP1->GOP2 (GOP1/GOP2 smae pipe)
    E_VOP_LAYER_FRAME_VIDEO_MUX2_MUX1,      ///<2: FrameColor->Video->GOP2->GOP1 (GOP1/GOP2 smae pipe)
    E_VOP_LAYER_FRAME_MUX1_VIDEO_MUX2,      ///<3: FrameColor->GOP1->Video->GOP2 (GOP1/GOP2 smae pipe)
    E_VOP_LAYER_FRAME_MUX1_MUX2_VIDEO,      ///<4: FrameColor->GOP1->GOP2->Video (GOP1/GOP2 smae pipe)
    E_VOP_LAYER_FRAME_MUX2_VIDEO_MUX1,      ///<5: FrameColor->GOP2->Video->GOP1 (GOP1/GOP2 smae pipe)
    E_VOP_LAYER_FRAME_MUX2_MUX1_VIDEO,      ///<6: FrameColor->GOP2->GOP1->Video (GOP1/GOP2 smae pipe)
    E_VOP_LAYER_RESERVED,
} E_VOP_OSD_LAYER_SEL;

/// OP2VOP de select
typedef enum
{
    E_OP2VOPDE_MAINWINDOW = 0,  ///<0: capture main display window, no osd
    E_OP2VOPDE_SUBWINDOW,       ///<1: capture sub display window, no osd
    E_OP2VOPDE_WHOLEFRAME,      ///<2: Whole frame of panel DE, no osd
    E_OP2VOPDE_WHOLEFRAME_WITHOSD, ///<3: Whole frame of panel DE, with osd
} E_OP2VOP_DE_SEL;

typedef void (*SC_InterruptCb) (SC_INT_SRC enIntNum, void *pParam);               ///< Interrupt callback function


//Define Output Frame control
typedef struct
{
    MS_U32 u32XC_version;    ///<Version of current structure.
    MS_U16  u16InVFreq;      ///<input V frequency
    MS_U16  u16OutVFreq;     ///<output V frequncy
    MS_BOOL bInterlace;      ///<whether it's interlace

} XC_OUTPUTFRAME_Info;

/// Define XC Init MISC
/// please enum use BIT0 = 1, BIT1 = 2, BIT3 = 4
typedef enum
{
    E_XC_INIT_MISC_A_NULL = 0,
    E_XC_INIT_MISC_A_IMMESWITCH = 1,
    E_XC_INIT_MISC_A_IMMESWITCH_DVI_POWERSAVING = 2,
    E_XC_INIT_MISC_A_DVI_AUTO_EQ = 4,
    E_XC_INIT_MISC_A_FRC_INSIDE = 8,

    E_XC_INIT_MISC_A_OSD_TO_HSLVDS = 0x10,
    E_XC_INIT_MISC_A_FRC_INSIDE_60HZ = 0x20, // for 60Hz FRC case
    E_XC_INIT_MISC_A_FRC_INSIDE_240HZ = 0x40, // for 240Hz FRC case
    E_XC_INIT_MISC_A_FRC_INSIDE_4K1K_120HZ = 0x80, // for 4K1K 120Hz FRC case

    E_XC_INIT_MISC_A_ADC_AUTO_SCART_SV = 0x100, // Support S-Video Auto Switch on SCART
    E_XC_INIT_MISC_A_FRC_INSIDE_KEEP_OP_4K2K = 0x200, // always keep OP timing as 4K2K
    E_XC_INIT_MISC_A_FRC_INSIDE_4K_HALFK_240HZ = 0x400, // for 4K 0.5K 240Hz FRC case
    E_XC_INIT_MISC_A_FCLK_DYNAMIC_CONTROL = 0x800, // for Dynamic FCLK control

    E_XC_INIT_MISC_A_SKIP_UC_DOWNLOAD = 0x1000, // for iframe case skip UC
    E_XC_INIT_MISC_A_SKIP_VIP_PEAKING_CONTROL = 0x2000, // for skipping VIP peaking control related unnecesary timing change
    E_XC_INIT_MISC_A_FAST_GET_VFREQ = 0x4000,//for get freq fast
    E_XC_INIT_MISC_A_LEGACY_MODE = 0x8000, // for monaco mode
    E_XC_INIT_MISC_A_SAVE_MEM_MODE = 0x10000, // for muji save memory
    E_XC_INIT_MISC_A_FRC_DUAL_MIU  = 0x20000, // for Manhattan FRC MEMC dual-MUU
    E_XC_INIT_MISC_A_IS_ANDROID = 0x40000, // android flag
    E_XC_S_DISPLAY_INITMISC_A_ENABLE_CVBSOX_ADCX = 0x80000, //enable ADCX code flow
    E_XC_INIT_MISC_A_IPMUX_HDR_MODE = 0x100000, // for Kano IPMux HDR
} XC_INIT_MISC_A;

typedef enum
{
    E_XC_INIT_MISC_B_NULL = 0,
    E_XC_INIT_MISC_B_PQ_SKIP_PCMODE_NEWFLOW = 1,
    E_XC_INIT_MISC_B_SKIP_SR = 2,
    E_XC_INIT_MISC_B_HDMITX_ENABLE = 4,
    E_XC_INIT_MISC_B_DRAM_DDR4_MODE = 8,
    E_XC_INIT_MISC_B_HDMITX_FREERUN_UCNR_ENABLE = 0x10, //Enable UC in SOC+Raptor output freerun case
} XC_INIT_MISC_B;

typedef enum
{
    E_XC_INIT_MISC_C_NULL = 0,
} XC_INIT_MISC_C;


typedef enum
{
    E_XC_INIT_MISC_D_NULL = 0,
} XC_INIT_MISC_D;

typedef enum
{
    E_XC_PQ_4K_PATH = 0,
    E_XC_PQ_UFSC_4K_PATH = 1,
    E_XC_PQ_PATH_MAX,
} E_XC_PQ_Path_Type;

/// Define the initial MISC for XC
typedef struct
{
    MS_U32 u32XCMISC_version;                   ///<Version of current structure.
    MS_U32 u32MISC_A;
    MS_U32 u32MISC_B;
    MS_U32 u32MISC_C;
    MS_U32 u32MISC_D;
} XC_INITMISC;

typedef enum
{
    E_XC_FLOCK_DIV_OFF = 0,
    E_XC_FLOCK_DIV_ON,
    E_XC_FLOCK_FPLL_ON,

} E_XC_FLOCK_TYPE;

typedef struct __attribute__((packed))
{
    // memory
    u32 u32FRC_MEMC_MemAddr;
    u32 u32FRC_MEMC_MemSize;
    u32 u32FRC_OD_MemAddr;
    u32 u32FRC_OD_MemSize;
    u32 u32FRC_LD_MemAddr;
    u32 u32FRC_LD_MemSize;
    u32 u32FRC_ME1_MemAddr;
    u32 u32FRC_ME1_MemSize;
    u32 u32FRC_ME2_MemAddr;
    u32 u32FRC_ME2_MemSize;
    u32 u32FRC_2D3D_Render_MemAddr;
    u32 u32FRC_2D3D_Render_MemSize;
    u32 u32FRC_2D3D_Render_Detection_MemAddr;
    u32 u32FRC_2D3D_Render_Detection_MemSize;
    u32 u32FRC_Halo_MemAddr;
    u32 u32FRC_Halo_MemSize;
    u32 u32FRC_R2_MemAddr;
    u32 u32FRC_R2_MemSize;
    u32 u32FRC_ADL_MemAddr;
    u32 u32FRC_ADL_MemSize;

    u32 u32FRC_FrameSize;

    u32 u16FB_YcountLinePitch;
    MS_U16 u16PanelWidth;
    MS_U16 u16PanelHeigh;
    MS_U8 u8FRC3DPanelType;
    MS_U8 u83Dmode;
    MS_U8 u8IpMode;
    MS_U8 u8MirrorMode;
    MS_U8 u83D_FI_out;
    MS_BOOL bFRC;

}
FRC_INFO_t, *PFRC_INFO_t;

typedef struct __attribute__((packed))
{
    MS_U32 u32XC_PREINIT_version;           ///<Version of current structure.
    // FRC control info
    FRC_INFO_t     FRCInfo;

    MS_U16 u16VTrigX;
    MS_U16 u16VTrigY;

    ///////////////////////////////////////////////
    // panel timing spec.
    ///////////////////////////////////////////////
    // sync related
    MS_U8 u8PanelHSyncWidth;                ///<  VOP_01[7:0], PANEL_HSYNC_WIDTH
    MS_U8 u8PanelHSyncBackPorch;            ///<  PANEL_HSYNC_BACK_PORCH, no register setting, provide value for query only

    ///<  not support Manuel VSync Start/End now
    ///<  VOP_02[10:0] VSync start = Vtt - VBackPorch - VSyncWidth
    ///<  VOP_03[10:0] VSync end = Vtt - VBackPorch
    MS_U8 u8PanelVSyncWidth;                ///<  define PANEL_VSYNC_WIDTH
    MS_U8 u8PanelVSyncBackPorch;            ///<  define PANEL_VSYNC_BACK_PORCH

}
XC_PREINIT_INFO_t, *p_XC_PREINIT_INFO_t;

typedef enum
{
    E_XC_PREINIT_NULL = 0x00,
    E_XC_PREINIT_FRC  = 0x01,
    E_XC_PREINIT_FRC_3D_TYPE  = 0x02,
    E_XC_PREINIT_NUM,

} E_XC_PREINIT_t;

typedef enum
{
    MIRROR_NORMAL,
    MIRROR_H_ONLY,
    MIRROR_V_ONLY,
    MIRROR_HV,
    MIRROR_MAX,
} MirrorMode_t;

typedef enum
{
    E_XC_MEMORY_IP_READ_REQUEST,    ///< memory request for IP read
    E_XC_MEMORY_IP_WRITE_REQUEST,   ///< memory request for IP write
    E_XC_MEMORY_OP_READ_REQUEST,    ///< memory request for OP read
    E_XC_MEMORY_OP_WRITE_REQUEST,   ///< memory request for OP write
    E_XC_MEMORY_REQUEST_MAX,        ///< Invalid request
} E_XC_MEMORY_REQUEST_TYPE;

/// Osd Index
typedef enum
{
    E_XC_OSD_0 = 0,       ///< osd0
    E_XC_OSD_1 = 1,       ///< osd1
    E_XC_OSD_2 = 2,       ///< osd2
    E_XC_OSD_3 = 3,       ///< osd3
    E_XC_OSD_4 = 4,       ///< osd3
    E_XC_OSD_NUM,
} E_XC_OSD_INDEX;

/// Osd Index
typedef enum
{
    E_XC_OSD_BlENDING_MODE0 = 0,       ///< a*OSD + (1-a)*Video
    E_XC_OSD_BlENDING_MODE1 = 1,       ///< OSD + (1-a)*Video
    E_XC_OSD_BlENDING_TYPE_NUM,
} E_XC_OSD_BlENDING_TYPE;

/// Scaling type
typedef enum
{
    E_XC_PRE_SCALING = 0,
    E_XC_POST_SCALING,
    E_XC_BOTH_SCALING,
} E_XC_SCALING_TYPE;

/// HV Vector type
typedef enum
{
    E_XC_H_VECTOR = 0,
    E_XC_V_VECTOR,
    E_XC_HV_VECTOR,
} E_XC_VECTOR_TYPE;

/// Define MCDI buffer type
typedef enum
{
    E_XC_MCDI_ME1 = 0,          ///< Main MDCI ME1
    E_XC_MCDI_ME2 = 1,          ///< Main MDCI ME2
    E_XC_MCDI_BOTH,             ///< Main MDCI ME1+ME2
    E_XC_MCDI_SUB_ME1,          ///< Sub MDCI ME1
    E_XC_MCDI_SUB_ME2,          ///< Sub MDCI ME2
    E_XC_MCDI_SUB_BOTH,         ///< Sub MDCI ME1+ME2
    E_XC_MCDI_MAX,
} E_XC_MCDI_TYPE;

typedef struct
{
    MS_U8 p1;
    MS_U8 p2;
    MS_U8 p3;
    MS_U8 p4;
    MS_U8 p5;
    MS_U8 p6;
    MS_U8 p7;
    MS_U8 p8;
} FRC_R2_CMD_PARAMETER_t, *p_FRC_R2_CMD_PARAMETER_t;

typedef enum
{
    E_XC_FRC_MB_CMD_NONE = 0x00,
    E_XC_FRC_MB_CMD_TEST = 0x01, // Print parameter
    E_XC_FRC_MB_CMD_GET_SW_VERSION = 0x02, // ParaCnt=0, return 2 Byte, P0=Main, P1=Minor
    E_XC_FRC_MB_CMD_GET_CMD_VERSION = 0x03, // ParaCnt=0, return 2 Byte, P0=Low byte, P1=High byte
    E_XC_FRC_MB_CMD_INIT = 0x10,         // ParaCnt=5, P0 = Panel 3D type(0=2D, 1=passive, 2=active, 3=PDP)
    //          P1 = Panel max V Freq, 60/120/240, but 240 Hz is not supported
    //          P2 = LocalDimming panel type, (0=LG32inch_LR10, 1=LG37inch_LR10, 2=LG42inch_LR16, 3=LG47inch_LR16, 4=LG55inch_LR16,
    //                  5=TOSHIBA_LR16, 6=TOSHIBA_TB16, 8=LG50inchCinema_TB32, 9=LG60inchCinema_TB32, A=LG47inch_Direct72, B=LG55inch_Direct96, C=LG72inch_Direct480)
    //          P3 = ChipRevision (0=U01, 1=U02, 2=U03, etc.)
    //          P4 = Is60HzPackage (0=FALSE, 1=TRUE)
    E_XC_FRC_MB_CMD_SET_TIMING = 0x11,   // ParaCnt=2, P0 = Input V Freq, P1 = Output V Freq
    E_XC_FRC_MB_CMD_SET_INPUT_FRAME_SIZE = 0x12, // ParaCnt=4, P0 = Width[15:8], P1 = width[7:0], P2 = height[15:8], P3 = height[7:0]
    E_XC_FRC_MB_CMD_SET_OUTPUT_FRAME_SIZE = 0x13,// ParaCnt=4, P0 = Width[15:8], P1 = width[7:0], P2 = height[15:8], P3 = height[7:0]
    E_XC_FRC_MB_CMD_SET_INPUT_3D_FORMAT = 0x14,  // ParaCnt=2, P0 = input 3D format(0=2D, 1=Side-by-side, 2=Top-Down, 3=Check-Board, 4=Frame-interleave, 6=Line alternative, 7=Frame-packing)
    //          P1 = 3DModeConfig(0=Normal, 1=L/R Swap, 2=L-Only, 3=R-Only)
    E_XC_FRC_MB_CMD_SET_3D_DEPTH_OFFSET = 0x15,  // ParaCnt=2, P0 = Enable3DDepthOffset(0=Disable, 1=Enable)
    //          P1 = 3DDepthOffset(0~255)
    E_XC_FRC_MB_CMD_SET_FPLL_LOCK_DONE = 0x16,   // ParaCnt=1, P0 = FPLL lock done(0: free run or not locked yet, 1: Locked)
    E_XC_FRC_MB_CMD_SET_MFC_MDOE = 0x20,         // ParaCnt=3, P0 = MFCMode(0=ff, 1=Low, 2=Mid, 3=High, 4=User, 5=55 pull down, 6=64 pull down)
    //          P1 = DeJudderLevel(0~10) [for User mode only]
    //          P2 = DeBlurLevel(0~10) [for User mode only]
    E_XC_FRC_MB_CMD_SET_MFC_DEMO_MODE = 0x21,    // ParaCnt=1, P0 = DemoModeEnable(0=Disable,1=Enable)
    E_XC_FRC_MB_CMD_SET_MFC_LOW_BANDWIDTH = 0x23,    // ParaCnt=1, P0 = MEMC low badnwidth switch(0 :off, 1 on)
    E_XC_FRC_MB_CMD_SET_LOCAL_DIMMING_MODE = 0x30,   // ParaCnt=1, P0 = LocalDimmingMode(0=Off, 1=Low, 2=Mid, 3=High, 4=Demo)
    E_XC_FRC_MB_CMD_SET_2D_TO_3D_MODE = 0x40,    // ParaCnt=5, P0 = 2D to 3D ConversionMode(0=Disable, 1=Enable)
    //          P1 = 2D to 3D Depth Gain(0~31)
    //          P2 = 2D to 3D Depth Offset(0~127)
    //          P3 = Artificial Gain(0~15)
    //          P4 = L/R Swap (0=Normal, 1=L/R Swap)
    ///*    the value is not decided yet
    E_XC_FRC_MB_CMD_SET_PWM_ENABLE = 0x50,       // ParaCnt=1, P0 = PWM Enable(0=Disable, 1=Enable)
    E_XC_FRC_MB_CMD_SET_PWM_DUTY_PHASE = 0x51,   // ParaCnt=6, P0 = PWM No.(0, 1, 3)
    //                  P1 P2 = PWMX_DutyPercentX100(0~65536)
    //                  P3 P4 = PWMX_360DegreeX100(0~65536)
    //                  P5 = PWMX_Polarity(0=not invert, 1=invert)
    E_XC_FRC_MB_CMD_SET_PWM_FREQ = 0x52,         // ParaCnt=4, P0 = PWM No.(0, 1, 3)
    //                  P1 = PWMX_FreqMul
    //                  P2 = PWMX_FreqDiv
    //                  P3 = PWMX_VsorHsRST
    //                  P4 = PWMX_SyncRstCnt
    E_XC_FRC_MB_CMD_SET_FORCE_PWM = 0x53,        // ParaCnt=0
    E_XC_FRC_MB_CMD_SET_IR_EMMITER_LR_SHIFT = 0x54,  // ParaCnt=3, P0 = Select the CMD(0=Lo, 1=Lc, 2=Ro, 3=Rc)
    //                  P2 P1= Pulse time shift(0~65536)
    E_XC_FRC_MB_CMD_SET_IR_EMMITER_LR_MODE = 0x55,   // ParaCnt=1, P0 = CMD Shift(0=Lo Lc, 1=Lc Ro, 2=Ro Rc, 3=Rc Lo)
    /*
        E_XC_FRC_MB_CMD_SET_IR_EMMITER_LR_MODE = 0x55,      // ParaCnt=4, P0 = Action Pattern Selection(0=Action Pattern 1, 1=Action Pattern 2)
                                                //          P1 = L frame CMD(0=Lo Lc, 1=Lc Ro, 2=Ro Rc, 3=Rc Lo for Pattern 1; 0=Lo Rc, 1=Rc Ro, 2=Ro Lc, 3=Lc Lo for Pattern 2)
                                                //          P2 = First CMD shift(0~255)
                                                //          P3 = Second CMD shift(0~255)
    */
    /*
        E_XC_FRC_MB_CMD_SET_PWM_CMD_MODE = 0x56,      // ParaCnt=4, P0 = Action Pattern Selection(0=Action Pattern 1, 1=Action Pattern 2)
                                                //          P1 = L frame CMD(0=Lo Lc, 1=Lc Ro, 2=Ro Rc, 3=Rc Lo for Pattern 1; 0=Lo Rc, 1=Rc Ro, 2=Ro Lc, 3=Lc Lo for Pattern 2)
                                                //          P2 = First CMD shift(0~255)
                                                //          P3 = Second CMD shift(0~255)
    */
} E_XC_FRC_MAILBOX_CMD;

//LD

typedef enum
{
    E_LD_PANEL_LG32inch_LR10 = 0x0,
    E_LD_PANEL_LG37inch_LR10 = 0x1,
    E_LD_PANEL_LG42inch_LR16 = 0x2,
    E_LD_PANEL_LG47inch_LR16 = 0x3,
    E_LD_PANEL_LG55inch_LR16 = 0x4,
    E_LD_PANEL_LG55inch_LR12 = 0x5,
    E_LD_PANEL_CMO42inch_LR16 = 0x6,
    E_LD_PANEL_LG50inch_4K2K_VB1 = 0x7,
    E_LD_PANEL_DEFAULT,
    E_LD_PANEL_NUMS,
} EN_LD_PANEL_TYPE;

typedef enum
{
    E_LD_MODE_OFF  = 0,
    E_LD_MODE_LOW  = 1,
    E_LD_MODE_MID  = 2,
    E_LD_MODE_HIGH = 3,
} EN_LD_MODE;

//parameters for detecting nine lattice
typedef struct
{
    MS_U16 u16PixelStep;        // distance between sample points
    MS_U8  u8ColorThreshold;    // max color difference
    MS_U8  u8TryTimes;          // max times for detecting
    MS_U16 u16LinearDiff;       // difference for every 3 lattice in a line
} ST_DETECTNL_PARA;

#define FRC_PATHCTRL_ALLOFF            (0x0000)
#define FRC_PATHCTRL_OP2COLORMATRIX    (0x0001)
#define FRC_PATHCTRL_OP2CSCDITHER      (0x0002)
#define FRC_PATHCTRL_OP2BRIGHTNESS     (0x0004)
#define FRC_PATHCTRL_OP2CONTRAST       (0x0008)
#define FRC_PATHCTRL_OP2NOISEROUND     (0x0010)

// OSDC TIMING GEN
typedef struct
{
    MS_U16 u16OC_Tgen_HTotal;
    MS_U16 u16OC_Tgen_VTotal;

    MS_U16 u16OC_Tgen_Hsync_Start;
    MS_U16 u16OC_Tgen_Hsync_End;
    MS_U16 u16OC_Tgen_HframDE_Start;
    MS_U16 u16OC_Tgen_HframDE_End;

    MS_U16 u16OC_Tgen_Vsync_Start;
    MS_U16 u16OC_Tgen_Vsync_End;
    MS_U16 u16OC_Tgen_VframDE_Start;
    MS_U16 u16OC_Tgen_VframDE_End;
} MS_XC_OSDC_TGEN_INFO;

// OSDC MISC Control
typedef struct
{
    MS_BOOL bOC_ClK_En;
    MS_BOOL bOC_Mixer_Bypass_En;
    MS_BOOL bOC_Mixer_InvAlpha_En;
    MS_BOOL bOC_Mixer_Hsync_Vfde_Out;    // 1:hs_out = hs&vfde
    MS_BOOL bOC_Mixer_Hfde_Vfde_Out;     // 1:hs_out = de(hfde)&vfde
    MS_U16  u16OC_Lpll_type;
    MS_U8   u8OC_OutputFormat;           // output bit order
} MS_XC_OSDC_CTRL_INFO;

typedef enum
{
    E_XC_OSDC_TGEN_MANUAL   = 0,
    E_XC_OSDC_TGEN_1366x768,
    E_XC_OSDC_TGEN_1920x1080,
    E_XC_OSDC_TGEN_3840x2160,
    E_XC_OSDC_TGEN_3840x1080,

    E_XC_OSDC_TGEN_MAX,
} E_XC_OSDC_TGEN_Type;

typedef enum
{
    E_XC_OSDC_INIT   = 0,
    E_XC_OSDC_CTRL_TGEN,
    E_XC_OSDC_CTRL_MISC,
    E_XC_OSDC_CTRL_OUTPUT_FORMAT,
} E_XC_OSDC_CTRL_TYPE;

//OSDC Display information
typedef struct
{
    MS_U32 ODSC_DISPInfo_Version;   ///<Version of current structure. Please always set to "u32Osdc_dispinfo_Version" as input

    MS_U32 VDTOT;      //Output vertical total
    MS_U32 DEVST;      //Output DE Vertical start
    MS_U32 DEVEND;     //Output DE Vertical end
    MS_U32 HDTOT;      // Output horizontal total
    MS_U32 DEHST;      //Output DE horizontal start
    MS_U32 DEHEND;     // Output DE horizontal end

    MS_U32 SYNCHST;
    MS_U32 SYNCHEND;
    MS_U32 SYNCVST;
    MS_U32 SYNCVEND;

    MS_BOOL bCLK_EN;                // OUTPUT ENABLE
    MS_BOOL bMIXER_BYPASS_EN;   // MIXER BYPASS ENABLE
    MS_BOOL bMIXER_INVALPHA_EN;
    MS_BOOL bMIXER_HSTVFDEOUT_EN;// 1:hs_out = hs&vfde
    MS_BOOL bMIXER_HFDEVFDEOUT_EN;// 1:hs_out = de(hfde)&vfde

} MS_OSDC_DST_DispInfo;

//select the input for bypass mode.
typedef enum
{
    E_XC_BYPASS_HDMI   = 0,
    E_XC_BYPASS_DC_MAIN,
    E_XC_BYPASS_DC_SUB,
} E_XC_BYPASS_InputSource;


// For HW internal test pattern
typedef enum
{
    E_XC_OP1_PATGEN_DISP_LB_MODE,   // pattern gen on display line buffer, can't support post-sclaing
    E_XC_OP1_PATGEN_OPM_MODE,       // pattern gen before post scaling engine, can support post-scaling, but not implement now
} EN_XC_OP1_PATGEN_MODE;

typedef enum
{
    E_XC_OP1_PATTERN_OFF = 0,
    E_XC_OP1_WB_PATTERN,                        // 1:
    E_XC_OP1_PURE_RED_PATTERN,                  // 2: red
    E_XC_OP1_PURE_GREEN_PATTERN,                // 3: green
    E_XC_OP1_PURE_BLUE_PATTERN,                 // 4: blue
    E_XC_OP1_PURE_WHITE_PATTERN,                // 5: white
    E_XC_OP1_PURE_BLACK_PATTERN,                // 6: black
    E_XC_OP1_PURE_GREY_PATTERN,                 // 7: gray
    E_XC_OP1_PURE_COLOR_V_BAR_PATTERN,          // 8: vertical color-bar
    E_XC_OP1_PURE_COLOR_H_BAR_PATTERN,          // 9: horizontal color-bar
    E_XC_OP1_PURE_GREY_BAR_16_PATTERN,          // 10: 16 vertical gray-bar
    E_XC_OP1_PURE_GREY_BAR_32_PATTERN,          // 11: 32 vertical gray-bar
    E_XC_OP1_PURE_GREY_BAR_64_PATTERN,          // 12: 64 vertical gray-bar
    E_XC_OP1_PURE_GREY_RGB_32_PATTERN,          // 13: 32 vertical 4 color bar
    E_XC_OP1_PURE_RGB_CHECKERBOARD_PATTERN,     // 14: 3x3 checkerboard R/B/G
    E_XC_OP1_DOAFADE_GRAY_PATTERN,              // 15: dotfade gary
    E_XC_OP1_CALIBRATION_COLOR_DOTFADE_PATTERN, // 16: checkerboard + dotfade gary
    E_XC_OP1_CALIBRATION_PATTERN,               // 17: checkerboard
    E_XC_OP1_PATTERN_1,                         // 18
    E_XC_OP1_PATTERN_2,                         // 19:
    E_XC_OP1_PATTERN_3,                         // 20: same as pattern2, but not full screen
    E_XC_OP1_3D_PATTERN,                        // 21: side-by-side, if enable 3D effect, can see the word "3D"
    E_XC_OP1_PURE_RED_100IRE_PATTERN,           // 22: red
    E_XC_OP1_PURE_GREEN_100IRE_PATTERN,         // 23: green
    E_XC_OP1_PURE_BLUE_100IRE_PATTERN,          // 24: blue
    E_XC_OP1_PURE_RED_70IRE_PATTERN,            // 25
    E_XC_OP1_PURE_GREEN_70IRE_PATTERN,          // 26
    E_XC_OP1_PURE_BLUE_70IRE_PATTERN,           // 27
    E_XC_OP1_PURE_RED_40IRE_PATTERN,            // 28
    E_XC_OP1_PURE_GREEN_40IRE_PATTERN,          // 29
    E_XC_OP1_PURE_BLUE_40IRE_PATTERN,           // 30
    E_XC_OP1_PURE_RED_20IRE_PATTERN,            // 31
    E_XC_OP1_PURE_GREEN_20IRE_PATTERN,          // 32
    E_XC_OP1_PURE_BLUE_20IRE_PATTERN,           // 33
    E_XC_OP1_PURE_RED_10IRE_PATTERN,            // 34
    E_XC_OP1_PURE_GREEN_10IRE_PATTERN,          // 35
    E_XC_OP1_PURE_BLUE_10IRE_PATTERN,           // 36
    E_XC_OP1_PURE_WHITE_100IRE_PATTERN,         // 37
    E_XC_OP1_PURE_WHITE_90IRE_PATTERN,          // 38
    E_XC_OP1_PURE_WHITE_80IRE_PATTERN,          // 39
    E_XC_OP1_PURE_WHITE_70IRE_PATTERN,          // 40
    E_XC_OP1_PURE_WHITE_60IRE_PATTERN,          // 41
    E_XC_OP1_PURE_WHITE_50IRE_PATTERN,          // 42
    E_XC_OP1_PURE_WHITE_40IRE_PATTERN,          // 43
    E_XC_OP1_PURE_WHITE_30IRE_PATTERN,          // 44
    E_XC_OP1_PURE_WHITE_20IRE_PATTERN,          // 45
    E_XC_OP1_PURE_WHITE_10IRE_PATTERN,          // 46
    E_XC_OP1_PURE_GAMMA_GREY1_PATTERN,          // 47: gray
    E_XC_OP1_PURE_GAMMA_GREY2_PATTERN,          // 48: gray
    E_XC_OP1_PURE_RGB_CHECKERBOARD_PATTERN_2,   // 49: 3x3 checkerboard R/G/B
} EN_XC_OP1_PATTERN;

//white balance pattern mode
typedef enum
{
    E_XC_OP1_PATTERN_MODE,         // OP1 tset Pattern
    E_XC_OP2_PATTERN_MODE,         // OP2 tset Pattern
} EN_XC_WB_PATTERN_MODE;


typedef struct
{
    // Output timing
    MS_U16 u16HStart;               ///< DE H start
    MS_U16 u16VStart;               ///< DE V start
    MS_U16 u16Width;                ///< DE H width
    MS_U16 u16Height;               ///< DE V height
    MS_U16 u16HTotal;               ///< H total
    MS_U16 u16VTotal;               ///< V total

    MS_U16 u16DefaultVFreq;         ///< Panel output Vfreq., used in free run

    // sync
    MS_U8  u8HSyncWidth;            ///< H sync width
    MS_U16 u16VSyncStart;           ///< V sync start = Vtotal - backporch - VSyncWidth
    MS_U8  u8VSyncWidth;            ///< V sync width
    MS_BOOL bManuelVSyncCtrl;       ///< enable manuel V sync control

    // output control
    MS_U16 u16OCTRL;                ///< Output control such as Swap port, etc.
    MS_U16 u16OSTRL;                ///< Output control sync as Invert sync/DE, etc.
    MS_U16 u16ODRV;                 ///< Driving current
    MS_U16 u16DITHCTRL;             ///< Dither control

} MS_XC_VOP_Data;

#ifdef UFO_XC_SETBLACKVIDEOBYMODE
// Black video mode
typedef enum
{
    E_XC_BLACK_VIDEO_BASIC,
    E_XC_BLACK_VIDEO_MEMSYNC,       // HW auto memory sync
} EN_XC_BLACK_VIDEO_MODE;
#endif

typedef enum
{
    /*
    __________________________________________________________________________________________________________________________________________
    |Vby1 Output Format|D0|D1|D2|D3|D4|D5|D6|D7|D8|D9|D10|D11|D12|D13|D14|D15|D16|D17|D18|D19|D20|D21|D22|D23|D24|D25|D26|D27|D28|D29|D30|D31|
    |------------------|--|--|--|--|--|--|--|--|--|--|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
    | ARGB 1           |R0|R1|R2|R3|R4|R5|R6|R7|G0|G1|G2 |G3 |G4 |G5 |G6 |G7 |B0 |B1 |B2 |B3 |B4 |B5 |B6 |B7 |A1 |A0 |A2 |A3 |A4 |A5 |A6 |A7 |
    |------------------|--|--|--|--|--|--|--|--|--|--|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
    | ARGB 2           |R2|R3|R4|R5|R6|R7|A6|A7|G2|G3|G4 |G5 |G6 |G7 |A4 |A5 |B2 |B3 |B4 |B5 |B6 |B7 |A2 |A3 |A0 |A1 |B0 |B1 |G0 |G1 |R0 |R1 |
    ------------------------------------------------------------------------------------------------------------------------------------------
    */
    E_XC_OSDC_OUTPUT_FORMAT_VBY1_ARGB1,
    E_XC_OSDC_OUTPUT_FORMAT_VBY1_ARGB2,
} E_XC_OSDC_OUTPUT_FORMAT;

typedef struct
{
    u32 u32FBSize[2];
    MS_U16 u16VBox_Htotal[2];
    MS_U16 u16VBox_Vtotal[2];
    MS_U16 u16VBox_PanelHsize;
    MS_U16 u16VBox_PanelVsize;
    MS_U16 u16Vfreq[2];
    MS_BOOL bInterface[2];
} XC_VBOX_INFO;

typedef enum
{
    E_XC_BWD_UPDATE_BWR_CONFIG = 0,           // Update bwr configuration
} E_XC_BWD_CONFIG_TYPE;

#ifdef UFO_XC_TEST_PATTERN
//xc test pattern type
typedef enum
{
    E_XC_ADC_PATTERN_MODE,             //adc test pattern
    E_XC_IPMUX_PATTERN_MODE,        // ipmux test Pattern
    E_XC_IP1_PATTERN_MODE,               // ip test Pattern
    E_XC_OP_PATTERN_MODE,              // op test Pattern
    E_XC_VOP_PATTERN_MODE,            // vop test Pattern
    E_XC_VOP2_PATTERN_MODE,          // vop2 test Pattern
    E_XC_MOD_PATTERN_MODE,           // mod test Pattern
} EN_XC_TEST_PATTERN_MODE;

//adc test pattern
typedef struct
{
    MS_U8 u8EnableADCType;
    MS_U16 u16Ramp;
} XC_SET_ADC_TESTPATTERN_t, *p_XC_SET_ADC_TESTPATTERN_t;

//ipmux test Pattern
typedef struct
{
    MS_BOOL bEnable;
    MS_U16 u16R_CR_Data;
    MS_U16 u16G_Y_Data;
    MS_U16 u16B_CB_Data;
} XC_SET_IPMUX_TESTPATTERN_t, *p_XC_SET_IPMUX_TESTPATTERN_t;

//ip test Pattern
typedef struct
{
    MS_U16 u16Enable;
    MS_U32 u32Pattern_type;
    SCALER_WIN eWindow;
} XC_SET_IP1_TESTPATTERN_t, *p_XC_SET_IP1_TESTPATTERN_t;

//op test Pattern
typedef struct
{
    MS_BOOL bMiuLineBuff;
    MS_BOOL bLineBuffHVSP;
} XC_SET_OP_TESTPATTERN_t, *p_XC_SET_OP_TESTPATTERN_t;

//vop test Pattern
typedef struct
{
    MS_BOOL bEnable;
} XC_SET_VOP_TESTPATTERN_t, *p_XC_SET_VOP_TESTPATTERN_t;

//vop2 test Pattern
typedef struct
{
    MS_BOOL bEnable;
    MS_U16 u16R_CR_Data;
    MS_U16 u16G_Y_Data;
    MS_U16 u16B_CB_Data;
} XC_SET_VOP2_TESTPATTERN_t, *p_XC_SET_VOP2_TESTPATTERN_t;

//mod test Pattern
typedef struct
{
    MS_BOOL bEnable;
    MS_U16 u16R_CR_Data;
    MS_U16 u16G_Y_Data;
    MS_U16 u16B_CB_Data;
} XC_SET_MOD_TESTPATTERN_t, *p_XC_SET_MOD_TESTPATTERN_t;
#endif

//-------------------------------------------------------------------------------------------------
//MApi_XC_VideoPlaybackController enum of cmd
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_XC_VPC_MULTI_SPEED_PLAYBACK_CTRL,
} E_XC_VIDEO_PLAYBACK_CTRL_ID;
//-------------------------------------------------------------------------------------------------
//MApi_XC_VideoPlaybackController struct of cmd
//-------------------------------------------------------------------------------------------------
typedef struct
{
    MS_BOOL bEnable;
    SCALER_WIN eWindow;
} ST_XC_VPC_MULTI_SPEED_PLAYBACK_CTRL;
//-------------------------------------------------------------------------------------------------

#ifdef UFO_XC_AUTO_DOWNLOAD
/// AUTODOWNLOAD_CONFIG_INFO version of current XC lib
#define AUTODOWNLOAD_CONFIG_INFO_VERSION          1
/// AUTODOWNLOAD_DATA_INFO version of current XC lib
#define AUTODOWNLOAD_DATA_INFO_VERSION            1
/// AUTODOWNLOAD_FORMAT_INFO version of current XC lib
#define AUTODOWNLOAD_FORMAT_INFO_VERSION          1

typedef enum
{
    E_XC_AUTODOWNLOAD_CLIENT_HDR,
    E_XC_AUTODOWNLOAD_CLIENT_OP2GAMMA,
    E_XC_AUTODOWNLOAD_CLIENT_FRCOP2GAMMA,
    E_XC_AUTODOWNLOAD_CLIENT_XVYCC,
    E_XC_AUTODOWNLOAD_CLIENT_ODTABLE1,
    E_XC_AUTODOWNLOAD_CLIENT_ODTABLE2,
    E_XC_AUTODOWNLOAD_CLIENT_ODTABLE3,
    E_XC_AUTODOWNLOAD_CLIENT_DEMURA,
    E_XC_AUTODOWNLOAD_CLIENT_OP2LUT,
    E_XC_AUTODOWNLOAD_CLIENT_T3D_0,        /// t3d of sc0
    E_XC_AUTODOWNLOAD_CLIENT_T3D_1,        /// t3d of sc1
    E_XC_AUTODOWNLOAD_CLIENT_FRCSPTPOPM,
    E_XC_AUTODOWNLOAD_CLIENT_FOOPM,
    E_XC_AUTODOWNLOAD_CLIENT_MAX,
} EN_XC_AUTODOWNLOAD_CLIENT;

typedef enum
{
    E_XC_AUTODOWNLOAD_TRIGGER_MODE,
    E_XC_AUTODOWNLOAD_ENABLE_MODE,
} EN_XC_AUTODOWNLOAD_MODE;

typedef enum
{
    E_XC_AUTODOWNLOAD_HDR_SUB_TMO,
    E_XC_AUTODOWNLOAD_HDR_SUB_3DLUT,
    E_XC_AUTODOWNLOAD_HDR_SUB_GAMMA,
    E_XC_AUTODOWNLOAD_HDR_SUB_DEGAMMA,
} EN_XC_AUTODOWNLOAD_SUB_CLIENT;

typedef struct
{
    MS_U32 u32ConfigInfo_Version;       ///<Version of current structure. Please always set to "AUTODOWNLOAD_CONFIG_INFO_VERSION" as input
    MS_U16 u16ConfigInfo_Length;        ///<Length of this structure, u16ConfigInfo_Length=sizeof(XC_AUTODOWNLOAD_CONFIG_INFO)

    EN_XC_AUTODOWNLOAD_CLIENT enClient; /// current client
    u32 phyBaseAddr;                 /// baseaddr
    MS_U32 u32Size;                     /// size
    MS_BOOL bEnable;                    /// enable/disable the client
    EN_XC_AUTODOWNLOAD_MODE enMode;     /// work mode
} XC_AUTODOWNLOAD_CONFIG_INFO;

typedef struct
{
    MS_U32 u32FormatInfo_Version;               ///<Version of current structure. Please always set to "AUTODOWNLOAD_FORMAT_INFO_VERSION" as input
    MS_U16 u16FormatInfo_Length;                ///<Length of this structure, u16FormatInfo_Length=sizeof(XC_AUTODOWNLOAD_FORMAT_INFO)

    EN_XC_AUTODOWNLOAD_SUB_CLIENT enSubClient;  /// sub client
    MS_BOOL bEnableRange;                       /// whether enable range, 0: write data to (0-511), 1: write data to (u32StartAddr-u16EndAddr)
    MS_U16 u16StartAddr;                        /// start addr, range: 0-511, startaddr <= endaddr
    MS_U16 u16EndAddr;                          /// end addr: range: 0-511, startaddr <= endaddr
} XC_AUTODOWNLOAD_FORMAT_INFO;

typedef struct
{
    MS_U32 u32DataInfo_Version;         ///<Version of current structure. Please always set to "AUTODOWNLOAD_DATA_INFO_VERSION" as input
    MS_U16 u16DataInfo_Length;          ///<Length of this structure, u16ConfigInfo_Length=sizeof(XC_AUTODOWNLOAD_DATA_INFO)

    EN_XC_AUTODOWNLOAD_CLIENT enClient; /// current client
    MS_U8* pu8Data;                     /// pointer to data
    MS_U32 u32Size;                     /// data size
    void* pParam;                       /// pointer to XC_AUTODOWNLOAD_FORMAT_INFO
} XC_AUTODOWNLOAD_DATA_INFO;

typedef struct
{
    EN_XC_AUTODOWNLOAD_CLIENT enClient;     /// current client
    MS_BOOL bSupported;                     /// whether it is supported
} XC_AUTODOWNLOAD_CLIENT_SUPPORTED_CAPS;
#endif

#ifdef UFO_XC_HDR
#if (UFO_XC_HDR_VERSION == 2)
#define HDR_3DLUT_INFO_VERSION 1
#define HDR_OPEN_METADATA_INFO_VERSION 1
#define HDR_DMA_CONFIG_INFO_VERSION 1
#define HDR_DMA_INIT_INFO_VERSION 1
#define HDR_OTT_SHARE_MEMORY_INFO_VERSION 1

typedef enum
{
    E_XC_HDR_CTRL_NONE,
    E_XC_HDR_CTRL_ENABLE,                   /// enable/disable hdr, pParam: pointer to MS_BOOL
    E_XC_HDR_CTRL_SET_TYPE,                 /// set hdr type, pParam: pointer to EN_XC_HDR_TYPE
    E_XC_HDR_CTRL_SET_3DLUT,                /// set 3dlut, pParam: pointer to XC_HDR_3DLUT_INFO, for dobly hdr only
    E_XC_HDR_CTRL_INIT_DMA,                 /// init dma info, pParam: pointer to XC_HDR_DMA_INIT_INFO
    E_XC_HDR_CTRL_CONFIG_DMA,               /// config dma info, pParam: pointer to XC_HDR_DMA_CONFIG_INFO
    E_XC_HDR_CTRL_SET_OTT_SHARE_MEMORY,     /// set share memory, pParam: pointer to XC_HDR_OTT_SHARE_MEMORY_INFO, communcation for VDEC and XC driver, for OTT only
    E_XC_HDR_CTRL_SET_OPEN_METADATA,        /// set open metadata, pParam: pointer to XC_HDR_OPEN_METADATA_INFO, for open hdr only
    E_XC_HDR_CTRL_SET_COLOR_FORMAT,         /// set color format, pParam: pointer to EN_XC_HDR_COLOR_FORMAT
    E_XC_HDR_CTRL_CFD_CONTROL,              /// CFD control
    E_XC_HDR_CTRL_GET_TYPE,                 /// get hdr type, pParam: pointer to EN_XC_HDR_TYPE
#ifdef UFO_XC_HDMI_4K2K_DMA
    E_XC_HDR_CTRL_INIT_DMA_HDMI,                 /// init dma info, pParam: pointer to XC_HDR_DMA_INIT_INFO
#endif
    E_XC_HDR_CTRL_MAX,
} EN_XC_HDR_CTRL_TYPE;

typedef enum
{
    E_XC_HDR_TYPE_NONE,
    E_XC_HDR_TYPE_OPEN,
    E_XC_HDR_TYPE_DOLBY,
    E_XC_HDR_TYPE_MAX,
} EN_XC_HDR_TYPE;

typedef enum
{
    E_XC_HDR_DMA_MODE_NONE,
    E_XC_HDR_DMA_MODE_8BIT,     // color depth: 8bits
    E_XC_HDR_DMA_MODE_10BIT,    // color depth: 10bits
    E_XC_HDR_DMA_MODE_12BIT,    // color depth: 12bits
    E_XC_HDR_DMA_MODE_MAX,
} EN_XC_HDR_DMA_MODE;

typedef enum
{
    E_XC_HDR_COLOR_NONE,
    E_XC_HDR_COLOR_YUV420,
    E_XC_HDR_COLOR_YUV422,
    E_XC_HDR_COLOR_YUV444,
    E_XC_HDR_COLOR_RGB,
    E_XC_HDR_COLOR_MAX,
} EN_XC_HDR_COLOR_FORMAT;

typedef struct
{
    MS_U32 u32OpenInfo_Version;         ///<Version of current structure. Please always set to "HDR_OPEN_METADATA_INFO_VERSION" as input
    MS_U16 u16OpenInfo_Length;          ///<Length of this structure, u16OpenInfo_Length=sizeof(XC_HDR_OPEN_METADATA_INFO)

    // the flowing metadata info
    // to do
} XC_HDR_OPEN_METADATA_INFO;

typedef struct
{
    MS_U32 u323DLutInfo_Version;         ///<Version of current structure. Please always set to "HDR_3DLUT_INFO_VERSION" as input
    MS_U16 u163DLutInfo_Length;          ///<Length of this structure, u163DLutInfo_Length=sizeof(XC_HDR_3DLUT_INFO)

    MS_U8* pu8Data;                     /// pointer to data
    MS_U32 u32Size;                     /// data size
} XC_HDR_3DLUT_INFO;

typedef struct
{
    MS_U32 u32DMAConfigInfo_Version;            ///<Version of current structure. Please always set to "HDR_DMA_CONFIG_INFO_VERSION" as input
    MS_U16 u16DMAConfigInfo_Length;             ///<Length of this structure, u16DMAConfigInfo_Length=sizeof(XC_HDR_DMA_CONFIG_INFO)

    EN_XC_HDR_DMA_MODE enMode;                  /// DMA mode
} XC_HDR_DMA_CONFIG_INFO;

typedef struct
{
    MS_U32 u32DMAInitInfo_Version;              ///<Version of current structure. Please always set to "HDR_DMA_INIT_INFO_VERSION" as input
    MS_U16 u16DMAInitInfo_Length;               ///<Length of this structure, u16DMAInitInfo_Length=sizeof(XC_HDR_DMA_INIT_INFO)

    u32 phyBaseAddr;                         /// DMA baseaddr
    MS_U32 u32Size;                             /// DMA size
} XC_HDR_DMA_INIT_INFO;

typedef struct
{
    MS_U32 u32ShareMemInfo_Version;             ///<Version of current structure. Please always set to "HDR_OTT_SHARE_MEMORY_INFO_VERSION" as input
    MS_U16 u16ShareMemInfo_Length;              ///<Length of this structure, u16ShareMemInfo_Length=sizeof(XC_HDR_SHARE_MEMORY_INFO)

    u32 phyBaseAddr;                         /// Share memory baseaddr
    MS_U32 u32Size;                             /// Share memory size
} XC_HDR_OTT_SHARE_MEMORY_INFO;

typedef struct
{
    EN_XC_HDR_TYPE enHDRType;               /// HDR type
    MS_BOOL bSupported;                     /// whether it is supported
} XC_HDR_SUPPORTED_CAPS;
#endif
#endif

#define CFD_INIT_VERSION 0
#define CFD_HDMI_VERSION 0
/// CFD Panel structure version
/// Version 0: Init structure
/// Version 1: Add Linear RGB setting and customer color primaries.
#define CFD_PANEL_VERSION 1
#define CFD_ANALOG_VERSION 0
#define CFD_HDR_VERSION 0
#define CFD_EDID_VERSION 0
/// CFD OSD structure version
/// Version 0: Init structure
/// Version 1: Add backlight and hue/contrast/saturation valid flag
/// Version 2: Add color range and main/sub window.
/// Version 3: Add ultra black / white
/// Version 4: Add skip picture setting (hue/ contrast/ saturation)
/// Version 5: Add color correction matrix
#define CFD_OSD_VERSION 5
#define CFD_DLC_VERSION 0
#define CFD_LINEAR_RGB_VERSION 0
#define CFD_FIRE_VERSION 0
/// Version 0: Init structure
/// Version 1: Add content is full range and ultra black & white active
/// Version 2: Add color type of the source
/// Version 3: Add HDR metadata of the source(ott or hdmi)
#define CFD_STATUS_VERSION 3

/// CFD control type. This enum sync from mdrv_xc_st.h in kernel
typedef enum
{
    /// Initialize
    E_XC_CFD_CTRL_TYPE_INIT = 0,
    /// VGA
    E_XC_CFD_CTRL_TYPE_VGA = 1,
    /// TV (ATV)
    E_XC_CFD_CTRL_TYPE_TV = 2,
    /// CVBS (AV)
    E_XC_CFD_CTRL_TYPE_CVBS = 3,
    /// S-Video
    E_XC_CFD_CTRL_TYPE_SVIDEO = 4,
    /// YPbPr
    E_XC_CFD_CTRL_TYPE_YPBPR = 5,
    /// Scart
    E_XC_CFD_CTRL_TYPE_SCART = 6,
    /// HDMI
    E_XC_CFD_CTRL_TYPE_HDMI = 7,
    /// DTV
    E_XC_CFD_CTRL_TYPE_DTV = 8,
    /// DVI
    E_XC_CFD_CTRL_TYPE_DVI = 9,
    /// MM
    E_XC_CFD_CTRL_TYPE_MM = 10,
    /// Panel
    E_XC_CFD_CTRL_TYPE_PANEL = 11,
    /// HDR
    E_XC_CFD_CTRL_TYPE_HDR = 12,
    /// EDID
    E_XC_CFD_CTRL_TYPE_EDID = 13,
    /// OSD
    E_XC_CFD_CTRL_TYPE_OSD = 14,
    /// Fire
    E_XC_CFD_CTRL_TYPE_FIRE = 15,
    /// DLC
    E_XC_CFD_CTRL_TYPE_DLC = 16,
    /// Linear RGB
    E_XC_CFD_CTRL_TYPE_LINEAR_RGB = 17,
    /// Get CFD Output to HDMI Status and dicison.
    E_XC_CFD_CTRL_GET_HDMI_STATUS = 18,
    /// Set force HDR OnOff or auto select
    E_XC_CFD_CTRL_SET_HDR_ONOFF_SETTING = 19,
    /// Status
    E_XC_CFD_CTRL_TYPE_STATUS = 20,
    /// CFD CTRL_TYPE_EX start
    /// (Set this base is 0x40000000 for int type)
    E_XC_CFD_CTRL_TYPE_EX_BASE = 0x40000000,
    /// Max
    E_XC_CFD_CTRL_TYPE_MAX,
} EN_XC_CFD_CTRL_TYPE;

/// HDR type.  This enum define is the same with E_CFIO_HDR_STATUS in CFD document.
typedef enum
{
    /// None HDR
    E_XC_CFD_HDR_TYPE_NONE = 0x00,
    /// Dolby HDR (HDR1)
    E_XC_CFD_HDR_TYPE_DOLBY = 0x01,
    /// Open HDR (HDR2)
    E_XC_CFD_HDR_TYPE_OPEN = 0x02,
    /// Max
    E_XC_CFD_HDR_TYPE_MAX = 0xff
} EN_XC_CFD_HDR_TYPE;

/// Update type, reference mhal_xc.h in kernel.
typedef enum
{
    /// Update all
    E_XC_CFD_UPDATE_TYPE_ALL,
    /// Update OSD only
    E_XC_CFD_UPDATE_TYPE_OSD_ONLY,
    /// Max
    E_XC_CFD_UPDATE_TYPE_MAX
} EN_XC_CFD_UPDATE_TYPE;

/// CFD init structure, reference mhal_xc.h in kernel.
typedef struct
{
    /// Structure version
    MS_U32 u32Version;
    /// Structure length
    MS_U16 u16Length;

    /// Window (main or sub window)
    MS_U8 u8Win;
    /// Input source
    MS_U8 u8InputSource;
} XC_CFD_INIT;

/// CFD HDMI structure, reference mhal_xc.h in kernel.
typedef struct
{
    /// Structure version
    MS_U32 u32Version;
    /// Structure length
    MS_U16 u16Length;

    /// Window (main or sub window)
    MS_U8 u8Win;

    /// Full range
    MS_BOOL bIsFullRange;

    /// AVI infoframe
    /// Pixel Format
    MS_U8 u8PixelFormat;
    /// Color imetry
    MS_U8 u8Colorimetry;
    /// Extended Color imetry
    MS_U8 u8ExtendedColorimetry;
    /// Rgb Quantization Range
    MS_U8 u8RgbQuantizationRange;
    /// Ycc Quantization Range
    MS_U8 u8YccQuantizationRange;

    /// HDR infoframe
    /// HDR infoframe valid
    MS_BOOL bHDRInfoFrameValid;
    /// EOTF (/// 0:SDR gamma, 1:HDR gamma, 2:SMPTE ST2084, 3:Future EOTF, 4-7:Reserved)
    MS_U8 u8EOTF;
    /// Static metadata ID (0: Static Metadata Type 1, 1-7:Reserved for future use)
    MS_U8 u8SMDID;
    /// Display primaries x
    MS_U16 u16Display_Primaries_x[3];
    /// Display primaries y
    MS_U16 u16Display_Primaries_y[3];
    /// White point x
    MS_U16 u16White_point_x;
    /// White point y
    MS_U16 u16White_point_y;
    /// Panel max luminance
    MS_U16 u16MasterPanelMaxLuminance;
    /// Panel min luminance
    MS_U16 u16MasterPanelMinLuminance;
    /// Max content light level
    MS_U16 u16MaxContentLightLevel;
    /// Max frame average light level
    MS_U16 u16MaxFrameAvgLightLevel;

} XC_CFD_HDMI;

/// CFD analog structure, reference mhal_xc.h in kernel.
typedef struct
{
    /// Structure version
    MS_U32 u32Version;
    /// Structure length
    MS_U16 u16Length;

    /// Window (main or sub window)
    MS_U8 u8Win;

    /// Color format, reference EN_KDRV_XC_CFD_COLOR_FORMAT.
    MS_U8 u8ColorFormat;
    /// Color data format, reference EN_KDRV_XC_CFD_COLOR_DATA_FORMAT.
    MS_U8 u8ColorDataFormat;
    /// Full range
    MS_BOOL bIsFullRange;
    /// Color primaries
    MS_U8 u8ColorPrimaries;
    /// Transfer characteristics
    MS_U8 u8TransferCharacteristics;
    /// Matrix coefficients
    MS_U8 u8MatrixCoefficients;

} XC_CFD_ANALOG;

/// CFD panel structure, reference mhal_xc.h in kernel.
typedef struct
{
    /// Structure version
    MS_U32 u32Version;
    /// Structure length
    MS_U16 u16Length;

    /// Color format, reference CFD document.
    MS_U8 u8ColorFormat;
    /// Color data format, reference CFD document.
    MS_U8 u8ColorDataFormat;
    /// Full range
    MS_BOOL bIsFullRange;

    /// Display primaries x, data *0.00002 0xC350 = 1
    MS_U16 u16Display_Primaries_x[3];
    /// Display primaries y, data *0.00002 0xC350 = 1
    MS_U16 u16Display_Primaries_y[3];
    /// White point x, data *0.00002 0xC350 = 1
    MS_U16 u16White_point_x;
    /// White point y, data *0.00002 0xC350 = 1
    MS_U16 u16White_point_y;
    /// Max luminance, data * 1 nits
    MS_U16 u16MaxLuminance;
    /// Med luminance, data * 1 nits
    MS_U16 u16MedLuminance;
    /// Min luminance, data * 0.0001 nits
    MS_U16 u16MinLuminance;
    /// Linear RGB
    MS_BOOL bLinearRgb;
    /// Customer color primaries
    MS_BOOL bCustomerColorPrimaries;
    /// Source white x
    MS_U16 u16SourceWx;
    /// Source white y
    MS_U16 u16SourceWy;
} XC_CFD_PANEL;

/// CFD EDID structure, reference mhal_xc.h in kernel.
typedef struct
{
    /// Structure version
    MS_U32 u32Version;
    /// Structure length
    MS_U16 u16Length;

    /// HDR data block valid
    MS_U8 u8HDMISinkHDRDataBlockValid;
    /// HDR EOTF
    MS_U8 u8HDMISinkEOTF;
    /// HDR static metadata descriptor
    MS_U8 u8HDMISinkSM;
    /// Desired content max luminance
    MS_U8 u8HDMISinkDesiredContentMaxLuminance;
    /// Desired content max frame average luminance
    MS_U8 u8HDMISinkDesiredContentMaxFrameAvgLuminance;
    /// Desired content min luminance
    MS_U8 u8HDMISinkDesiredContentMinLuminance;
    /// HDR data block length
    MS_U8 u8HDMISinkHDRDataBlockLength;

    /// Display primaries x
    MS_U16 u16Display_Primaries_x[3];
    /// Display primaries y
    MS_U16 u16Display_Primaries_y[3];
    /// White point x
    MS_U16 u16White_point_x;
    /// White point y
    MS_U16 u16White_point_y;

    /// EDID version
    MS_U8 u8HDMISinkEDIDBaseBlockVersion;
    /// EDID reversion
    MS_U8 u8HDMISinkEDIDBaseBlockReversion;
    /// CEA block reversion
    MS_U8 u8HDMISinkEDIDCEABlockReversion;

    /// Video capability data block valid
    MS_U8 u8HDMISinkVCDBValid;
    /// Support YUV format, bit0: YUV444, bit1: YUV422, bit2: YUV420, bit3: QY, bit4: QS
    MS_U8 u8HDMISinkSupportYUVFormat;

    /// Colorimetry
    MS_U8 u8HDMISinkExtendedColorspace;
    /// EDID valid
    MS_U8 u8HDMISinkEDIDValid;
} XC_CFD_EDID;

/// CFD HDR structure, reference mhal_xc.h in kernel.
typedef struct
{
    /// Structure version
    MS_U32 u32Version;
    /// Structure length
    MS_U16 u16Length;

    /// Window (main or sub window)
    MS_U8 u8Win;
    /// HDR type, reference EN_MAPI_CFD_HDR_TYPE
    MS_U8 u8HdrType;
} XC_CFD_HDR;

/// CFD HDR structure, reference mhal_xc.h in kernel.
typedef struct
{
    /// Structure version
    MS_U32 u32Version;
    /// Structure length
    MS_U16 u16Length;

    /// customer dlc curve
    MS_BOOL bUseCustomerDlcCurve;
} XC_CFD_DLC;

/// CFD linear structure, reference mhal_xc.h in kernel.
typedef struct
{
    /// Structure version
    MS_U32 u32Version;
    /// Structure length
    MS_U16 u16Length;

    /// Window (main or sub window)
    MS_U8 u8Win;
    /// Linear RGB enable
    MS_BOOL bEnable;
} XC_CFD_LINEAR_RGB;

//controls from Panel OSD
typedef struct
{
    MS_U32 u32Version;   ///<Version of current structure. Please always set to "ACE_OSD_CONTROL_VERSION" as input
    MS_U16 u16Length;    ///<Length of this structure, u16Length=sizeof(ST_HAL_PQ_CFD_OSD_CONTROL)

    MS_U16 u16Hue;
    MS_U16 u16Saturation;
    MS_U16 u16Contrast;

    //default on , not in the document
    MS_U8  u8OSDUIEn;

    //Mode 0: update matrix by OSD and color format driver
    //Mode 1: only update matrix by OSD controls
    //for mode1 : the configures of matrix keep the same as the values by calling CFD last time
    MS_U8  u8OSDUIMode;

    ///
    MS_BOOL bHueValid;
    MS_BOOL bSaturationValid;
    MS_BOOL bContrastValid;

    MS_BOOL bBacklightValid;
    MS_U32 u32MinBacklight;
    MS_U32 u32MaxBacklight;
    MS_U32 u32Backlight;

    MS_U8 u8Win;
    MS_BOOL bColorRangeValid;
    MS_U8 u8ColorRange; // 0: Auto, 1: Full Range (0~255), 2: Limit Range (16~235)

    MS_BOOL bUltraBlackLevelValid;
    MS_U8 u8UltraBlackLevel;
    MS_BOOL bUltraWhiteLevelValid;
    MS_U8 u8UltraWhiteLevel;

    MS_BOOL bSkipPictureSettingValid;
    MS_BOOL bSkipPictureSetting;

    MS_BOOL bColorCorrectionValid;
    MS_S16 s16ColorCorrectionMatrix[32];
} XC_CFD_OSD_CONTROL;

typedef struct
{
    MS_U8 u8CurrentIndex;
    MS_U8 u8InputFormat;
    MS_U8 u8InputDataFormat;
    MS_BOOL u8Video_Full_Range_Flag;

    MS_BOOL bVUIValid;
    MS_U8 u8Colour_primaries;
    MS_U8 u8Transfer_Characteristics;
    MS_U8 u8Matrix_Coeffs;

    MS_BOOL bSEIValid;
    MS_U16 u16Display_Primaries_x[3];
    MS_U16 u16Display_Primaries_y[3];
    MS_U16 u16White_point_x;
    MS_U16 u16White_point_y;
    MS_U32 u32Master_Panel_Max_Luminance;
    MS_U32 u32Master_Panel_Min_Luminance;

    MS_BOOL bContentLightLevelEnabled;
    MS_U16  u16maxContentLightLevel;
    MS_U16  u16maxPicAverageLightLevel;
} XC_HDR_CFD_MEMORY_FORMAT;

typedef struct
{
    /// Structure version
    MS_U32 u32Version;
    /// Structure length
    MS_U16 u16Length;

    /// Window (main or sub window)
    MS_U8 u8Win;
    /// HDR type (0: SDR, 1: Dolby HDR, 2: Open HDR)
    MS_U8 u8VideoHdrType;
    /// HDR running
    MS_BOOL bIsHdrRunning;
    /// Is full range
    MS_BOOL bIsFullRange;
    /// Ultra black & white active
    MS_BOOL bUltraBlackWhiteActive;
    /// Color Type
    MS_U8 u8ColorType;

    /// open HDR metadata
    union
    {
        XC_CFD_HDMI stHdmiInfoCFD;
        XC_HDR_CFD_MEMORY_FORMAT stHDRMemFormatCFD;
    } HDRMetadata;
} XC_CFD_STATUS;


/// CFD fire structure, reference mhal_xc.h in kernel.
typedef struct
{
    /// Structure version
    MS_U32 u32Version;
    /// Structure length
    MS_U16 u16Length;

    /// Window (main or sub window)
    MS_U8 u8Win;
    /// Input source, reference PQ_INPUT_SOURCE_TYPE in apxXC.h
    MS_U8 u8InputSource;
    /// Update type, reference EN_MAPI_CFD_UPDATE_TYPE
    MS_U8 u8UpdateType;
    /// RGB bypass
    MS_BOOL bIsRgbBypass;
    /// HD mode
    MS_BOOL bIsHdMode;
} XC_CFD_FIRE;

/// CFD control information, reference mdrv_xc_st.h in kernel.
typedef struct  __attribute__((packed))
{
    /// Control type, reference EN_MAPI_CFD_CTRL_TYPE
    EN_XC_CFD_CTRL_TYPE enCtrlType;
    /// Control parameter
    void *pParam;
#if !defined (__aarch64__)
    /// Dummy parameter
    void *pDummy;
#endif
    /// Control parameter length
    MS_U32 u32ParamLen;
    /// Error code
    MS_U16 u16ErrCode;
}
XC_CFD_CONTROL_INFO;

////////////////////////////////////////////

/// OPTEE
typedef struct
{
    MS_U32 ID[4];
} RESOURCE;
#define OP_TEE_XC_REG_MAX_CNT   0x10
#define OP_TEE_XC_DRAM_MAX_CNT  0x10
typedef enum
{
    E_XC_OPTEE_GET_PIPE_ID,
    E_XC_OPTEE_ENABLE,
    E_XC_OPTEE_DISABLE,
    E_XC_OPTEE_SET_HANDLER,
    E_XC_OPTEE_SYNC_HANDLER,
    E_XC_OPTEE_GET_HANDLER,
    E_XC_OPTEE_UPDATE_HANDLER,
    E_XC_OPTEE_SET_MUX,
    E_XC_OPTEE_SYNC_MUX,
    E_XC_OPTEE_GET_MUX,
    E_XC_OPTEE_MAX,
} EN_XC_OPTEE_ACTION;
typedef struct
{
    MS_U32      bk;
    MS_U16      value;
    MS_U16      mask;
} XC_OPTEE_REG_DATA;
typedef struct
{
    MS_U32      address;
    MS_U32      length;
} XC_OPTEE_DRAM_DATA;
typedef struct
{
    MS_U16                  version;
    MS_U32                  length;
    SCALER_WIN              eWindow;
    //    EN_XC_OPTEE_ACTION      action;
    /* E_XC_OPTEE_GET_PIPE_ID */
    MS_U32                  pipeID;
    /* E_XC_OPTEE_ENABLE / E_XC_OPTEE_DISABLE */
    MS_BOOL                 isEnable;
    /* E_XC_OPTEE_SET_HANDLER / E_XC_OPTEE_GET_HANDLER / E_XC_OPTEE_UPDATE_HANDLER */
    MS_U32                  regs_cnt;
    XC_OPTEE_REG_DATA       regs_data[OP_TEE_XC_REG_MAX_CNT];
    MS_U32                  drams_cnt;
    XC_OPTEE_DRAM_DATA      drams_data[OP_TEE_XC_DRAM_MAX_CNT];

} XC_OPTEE_HANDLER;
typedef struct
{
    MS_U16                  version;
    MS_U32                  length;
    SCALER_WIN              eWindow;
    MS_U32                  regs_cnt;
    XC_OPTEE_REG_DATA       regs_data[OP_TEE_XC_REG_MAX_CNT];
    MS_BOOL                 bfire;
} XC_OPTEE_MUX_DATA;

//define 3D attribute type
typedef enum
{
    E_XC_3D_ATTRIBUTE_OSD,
    E_XC_3D_ATTRIBUTE_NUM,
} E_XC_3D_ATTRIBUTE_TYPE;

//define 3D OSD blending point
typedef enum
{
    E_XC_3D_OSD_BLENDING_SC_IP,   //scaler IP
    E_XC_3D_OSD_BLENDING_SC_OP,   //scaler IP
    E_XC_3D_OSD_BLENDING_FRC_IP, // inside FRC IP
    E_XC_3D_OSD_BLENDING_FRC_OP, // inside FRC IP
    E_XC_3D_OSD_BLENDING_NUM,
} E_XC_3D_OSD_BLENDING_POINT;

#undef INTERFACE
//#ifdef __cplusplus
//}
//#endif

#endif /* _API_XC_H_ */