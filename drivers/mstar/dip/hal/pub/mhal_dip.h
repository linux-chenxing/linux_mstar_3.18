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
//==============================================================================
// [mhal_dip.h]
// Date: 20120208
// Descriptions: Add a new layer for HW setting
//==============================================================================
#ifndef MHAL_DIP_H
#define MHAL_DIP_H

//#include "hwreg_sc.h"

//#include "apiXC.h"

#define  DIP_K6        (0)
#define  DIP_I2        (1)

#define  DIP_PLATFORM        (DIP_I2)

#define  CONFIG_NEW_3DDI_ALGO        (1)  //

#define MVOP_PATH_COUNT_SUPPT            2
#define XC_PATH_COUNT_SUPPT              2

//HW patch
#define DIP_DWIN0_TILE_32_32_PATCH       1

#define printf(_fmt, _args...)        printk(KERN_INFO _fmt, ## _args)

#define ASCII_COLOR_RED     "\033[1;31m"
#define ASCII_COLOR_WHITE   "\033[1;37m"
#define ASCII_COLOR_YELLOW  "\033[1;33m"
#define ASCII_COLOR_BLUE    "\033[1;36m"
#define ASCII_COLOR_GREEN   "\033[1;32m"
#define ASCII_COLOR_END     "\033[0m"

#if 1  //
#define FUNC_MSG(fmt, args...)    \
    if(debug_level >= 1)\
    {\
        do{\
            printf(fmt, ##args);\
        }while(0);\
    }
#else
#define FUNC_MSG(fmt, args...)            ({do{printf(ASCII_COLOR_END"%s[%d] ",__FUNCTION__,__LINE__);printf(fmt, ##args);printf(ASCII_COLOR_END);}while(0);})
#endif
#define FUNC_ERR(fmt, args...)            ({do{printf(ASCII_COLOR_RED"%s[%d] ",__FUNCTION__,__LINE__);printf(fmt, ##args);printf(ASCII_COLOR_END);}while(0);})


#define ALIGN_CHECK(value,factor) ((value + factor-1) & (~(factor-1)))
#define DIP_BYTE_PER_WORD       16UL
#define DIP_HVD_TILE_BLOCK_H    16UL
#define DIP_HVD_TILE_BLOCK_V    32UL
#define DIP_H265_TILE_BLOCK_H   32UL
#define DWIN_W_LIMITE_OFT       31UL
#define DWIN_BYTE_PER_WORD      32UL
#ifndef UNUSED
    #define UNUSED(var)             ((var) = (var))
#endif
#define DIP_TRY_RESOURCE_TIMES 1000UL

typedef struct
{
    MS_U16 u16H_CapStart;
    MS_U16 u16H_CapSize;
    MS_U16 u16V_CapStart;
    MS_U16 u16V_CapSize;

    MS_U32 u32H_PreScalingRatio;
    MS_U32 u32V_PreScalingRatio;

    MS_U16 u16DNROffset;
    MS_U16 u16DNRFetch;
} SC_DIP_SWDB_INFO, *P_SC_DIP_SWDB_INFO;

/// DWIN scan mode
typedef enum
{
    GOPDWIN_SCAN_MODE_AUTO = 0,
    GOPDWIN_SCAN_MODE_PROGRESSIVE = GOPDWIN_SCAN_MODE_AUTO,  ///< Progressive scan
    GOPDWIN_SCAN_MODE_INTERLACE,  ///< Interlace scan
    GOPDWIN_SCAN_MODE_MAX
} EN_XC_DWIN_SCAN_TYPE;

/// DWIN source data format
typedef enum
{
    /// YCrYCb.
    XC_DWIN_DATA_FMT_YUV422 = 0,
    /// RGB domain
    XC_DWIN_DATA_FMT_RGB565,
    /// RGB domain
    XC_DWIN_DATA_FMT_ARGB8888,
    /// YUV420 HVD tile fmt
    XC_DWIN_DATA_FMT_YUV420,
    /// YC separate 422
    XC_DWIN_DATA_FMT_YC422,
    /// YUV420 H265 tile fmt
    XC_DWIN_DATA_FMT_YUV420_H265,
    /// YUV420 H265 10 bits tile fmt
    XC_DWIN_DATA_FMT_YUV420_H265_10BITS,
    /// YUV420 planer
    XC_DWIN_DATA_FMT_YUV420_PLANER,
    /// YUV420 semi planer
    XC_DWIN_DATA_FMT_YUV420_SEMI_PLANER,
    XC_DWIN_DATA_FMT_MAX
} EN_DRV_XC_DWIN_DATA_FMT;

/// DIP OP-TEE Operation mode
typedef enum
{
    /// unLock_output.
    XC_DIP_OPMODE_UNLOCK_OUTPUT = 0,
    /// unLock_output.
    XC_DIP_OPMODE_LOCK_OUTPUT = 1,
    /// Enable.
    XC_DIP_OPMODE_ENABLE = 2,
    /// Disable.
    XC_DIP_OPMODE_DISABLE = 3,
} EN_DRV_XC_DIP_OPERATIONMODE;

typedef struct
{
    MS_U32 u32PipeID;
    MS_U32 u32SecureDMA;
    MS_U32 u32OperationMode;
    MS_U32 u32ReturnValue;
} DIP_CONFIG_PIPE;

/// Define DIP destion
typedef struct
{
    MS_BOOL                 bSOURCE_TYPE_MVOP[MVOP_PATH_COUNT_SUPPT][MAX_DIP_WINDOW];         ///< DIP from MVOP
    MS_BOOL                 bSOURCE_TYPE_IP_MAIN[XC_PATH_COUNT_SUPPT][MAX_DIP_WINDOW];        ///< DIP from MAIN(IP_MAIN)
    MS_BOOL                 bSOURCE_TYPE_IP_SUB[XC_PATH_COUNT_SUPPT][MAX_DIP_WINDOW];         ///< DIP from SUB (IP_SUB)
    MS_BOOL                 bSOURCE_TYPE_OP_MAIN[XC_PATH_COUNT_SUPPT][MAX_DIP_WINDOW];        ///< DIP from MAIN(OP_MAIN)
    MS_BOOL                 bSOURCE_TYPE_OP_SUB[XC_PATH_COUNT_SUPPT][MAX_DIP_WINDOW];         ///< DIP from SUB(OP_SUB)
    MS_BOOL                 bSOURCE_TYPE_OP_CAPTURE[XC_PATH_COUNT_SUPPT][MAX_DIP_WINDOW];     ///< DIP from OP capture
    MS_BOOL                 bSOURCE_TYPE_DRAM[MAX_DIP_WINDOW];                                ///< DIP from DRAM
    MS_BOOL                 bSOURCE_TYPE_OSD[MAX_DIP_WINDOW];                                 ///< DIP from OSD capture
} DIP_CHIP_SOURCE_SEL;

/// Define DIP chip property for different chip characteristic.
typedef struct
{
    MS_BOOL                 bSourceSupport[MAX_DIP_WINDOW];                                    ///<DIP resource support number
    MS_BOOL                 bDIPBuildIn[MAX_DIP_WINDOW];                                       ///<DIP XC buildIn or GOP
    MS_U8                   XCPathCount;                                                       ///<SC0/SC1/SC2
    MS_U16                  BusWordUnit[MAX_DIP_WINDOW];                                       ///<DIP Bus
    DIP_CHIP_SOURCE_SEL     bSourceSel;                                                        ///<DIP Source select
} DIP_CHIP_PROPERTY;

typedef struct
{
    MS_BOOL bEnable;
    MS_U8 u8WinId;
    MS_U8 u8WinIndex;
    MS_U8 u8Format;
    MS_U16 u16Lineoffset;
    MS_U32 u32BaseAddr;
    MS_U16 u16Width;
    MS_U16 u16Height;
    MS_BOOL bHmirror;
    MS_BOOL bVflip;
} DIP_MDWIN_PROPERTY;

typedef struct
{
    MS_BOOL bEnable;
    MS_U16 u16WinHStart;
    MS_U16 u16WinHSize;
    MS_U16 u16WinVStart;
    MS_U16 u16WinVSize;

    MS_U16 u16ColorR;
    MS_U16 u16ColorG;
    MS_U16 u16ColorB;
} DIP_COVER_PROPERTY;

typedef enum
{
    SCAN_AUTO = 0,
    SCAN_PROGRESSIVE = SCAN_AUTO,
    SCAN_INTERLACE = 1
} EN_VIDEO_SCAN_TYPE;

// Define DIP output destination path
typedef enum
{
    DIP_OUTPUT_PATH_DRAM = 0,
    DIP_OUTPUT_PATH_MDWIN,
    DIP_OUTPUT_PATH_MAX
} DIP_OUTPUT_PATH;

//SC
typedef struct __attribute__((packed))
{
    //----------------------
    // Customer setting
    //----------------------
    INPUT_SOURCE_TYPE_t enInputSourceType; 	 ///< Input source type
    //----------------------
    // Source type for DIP
    //----------------------
    SCALER_DIP_SOURCE_TYPE  enDIPSourceType;   ///<DIP input source type
    //----------------------
    // Window
    //----------------------
    MS_WINDOW_TYPE stCapWin;		///<Capture window
    //----------------------
    // Timing
    //----------------------
    MS_BOOL bInterlace;			 ///<Interlaced or Progressive
    MS_BOOL bHDuplicate;			 ///<flag for vop horizontal duplicate, for MVD, YPbPr, indicate input double sampled or not
    //----------------------
    // customized post scaling
    //----------------------
    MS_BOOL bHCusScaling;			 ///<assign H customized scaling instead of using XC scaling
    MS_U16  u16HCusScalingSrc; 	 ///<H customized scaling src width
    MS_U16  u16HCusScalingDst; 	 ///<H customized scaling dst width
    MS_BOOL bVCusScaling;			 ///<assign V manuel scaling instead of using XC scaling
    MS_U16  u16VCusScalingSrc; 	 ///<V customized scaling src height
    MS_U16  u16VCusScalingDst; 	 ///<V customized scaling dst height
    //-------------------------
    // customized pre scaling
    //-------------------------
    MS_BOOL bPreHCusScaling;            ///<assign pre H customized scaling instead of using XC scaling
    MS_U16  u16PreHCusScalingSrc;       ///<pre H customized scaling src width
    MS_U16  u16PreHCusScalingDst;       ///<pre H customized scaling dst width
    MS_BOOL bPreVCusScaling;            ///<assign pre V manuel scaling instead of using XC scaling
    MS_U16  u16PreVCusScalingSrc;       ///<pre V customized scaling src height
    MS_U16  u16PreVCusScalingDst;       ///<pre V customized scaling dst height
    //----------------------
    // XC internal setting
    //----------------------
    /* scaling ratio */
    MS_U16 u16H_SizeAfterPreScaling;		  ///<Horizontal size after prescaling
    MS_U16 u16V_SizeAfterPreScaling;		  ///<Vertical size after prescaling

    MS_U8 u8DelayLines;			 ///<delay lines
    MS_BOOL bMemFmt422;			 ///<whether memory format is 422, for easy usage
    MS_BOOL bMemYUVFmt;			 ///< memroy color format

    //XC_DIP_BUFFER_INFO stBufInfo;
}
XC_DIP_InternalStatus, *P_XC_DIP_InternalStatus;


//==============================================================================
// Scaling Ratio Macro

// H_PreScalingDownRatio() was refined to reduce the calculation error.
// Use round up (x+y/2)/y might reduce the scaling ratio and induce right vertical garbage line.
// So use un-conditional add by 1 (x+y)/y.
#define H_PreScalingDownRatioDIP(Input, Output)                ( (((MS_U32)(Output)) * 1048576ul)/ (Input) + 1 )
#define V_PreScalingDownRatioDIP(Input, Output)                ( (((MS_U32)(Output)) * 1048576ul)/ (Input) + 1 )  // CB mode

#define DIP_CHIP_CAP(eWindow,_ret)     do{    \
        switch(eWindow){                \
            case DIP_WINDOW:            \
                _ret = DIP_CAP_EXIST|   \
                       DIP_CAP_420TILE| \
                       DIP_CAP_MIRROR|  \
                       DIP_CAP_DIPR|    \
                       DIP_CAP_R2Y|     \
                       DIP_CAP_OP1_CAPTURE_V2|\
                       DIP_CAP_SCALING_DOWN| \
                       DIP_CAP_SCALING_UP; \
                break;                  \
            case DWIN0_WINDOW:            \
                _ret = DIP_CAP_EXIST|   \
                       DIP_CAP_MIRROR|  \
                       DIP_CAP_DIPR|    \
                       DIP_CAP_R2Y|     \
                       DIP_CAP_OP1_CAPTURE_V2|\
                       DIP_CAP_SCALING_DOWN| \
                       DIP_CAP_SCALING_UP; \
                break;                  \
            default:                    \
                _ret = 0;               \
                break;                  \
        }                               \
    }while(0)

typedef enum
{
    E_XC_DIP_SOURCE_TYPE_SUB2,
    E_XC_DIP_SOURCE_TYPE_SUB,
    E_XC_DIP_SOURCE_TYPE_MAIN,
    E_XC_DIP_SOURCE_TYPE_OP,
} MS_XC_DIP_SOURCE_TYPE;

typedef enum
{
    E_DIP_NULL_FIELD        = 0x00,
    E_DIP_TOP_FIELD         = 0x01,
    E_DIP_BOTTOM_FIELD      = 0x02,
} EN_TB_FIELD;

//==============================================================================
//==============================================================================
#ifdef MHAL_DIP_C
    #define INTERFACE
#else
    #define INTERFACE extern
#endif

#define Hal_SC_DWIN_set_ficlk(args...)
#define Hal_SC_DWIN_set_wr_bank_mapping(args...)
#define Hal_SC_DWIN_set_csc(args...)
#define Hal_SC_DWIN_set_delayline(args...)
#define Hal_SC_DWIN_sw_db_burst(args...)
#define Hal_SC_DWIN_Set_FB_Num(args...)
#define Hal_SC_DWIN_get_memory_bit_fmt(args...) 0
#define Hal_SC_DWIN_set_capture_v_start(args...)
#define Hal_SC_DWIN_set_capture_h_start(args...)
#define Hal_SC_DWIN_set_capture_v_size(args...)
#define Hal_SC_DWIN_set_capture_h_size(args...)
#define Hal_SC_DWIN_get_capture_window(args...)
#define Hal_SC_DWIN_set_miusel(args...)
#define Hal_SC_DWIN_set_memoryaddress(args...)
#define Hal_SC_DWIN_DisableInputSource(args...)
#define Hal_SC_DWIN_SetInputSource(args...)
#define Hal_SC_DWIN_software_reset(args...)
#define Hal_SC_DWIN_setfield(args...)
#define Hal_SC_DWIN_set_image_wrap(args...)
#define Hal_SC_DWIN_set_input_sync_reference_edge(args...)
#define Hal_SC_DWIN_set_input_vsync_delay(args...)
#define Hal_SC_DWIN_set_de_only_mode(args...)
#define Hal_SC_DWIN_set_input_10bit(args...)
#define Hal_SC_DWIN_set_fir_down_sample_divider(args...)
#define Hal_SC_DWIN_set_coast_input(args...)
#define Hal_SC_DWIN_set_coast_window(args...)
#define Hal_SC_DWIN_set_input_sync_sample_mode(args...)
#define Hal_SC_DWIN_set_post_glitch_removal(args...)
#define Hal_SC_DWIN_set_DE_Mode_Glitch(args...)
#define Hal_SC_DWIN_set_de_bypass_mode(args...)
#define Hal_SC_DWIN_set_ms_filter(args...)
#define Hal_SC_DWIN_Disable_IPM_ReadWriteRequest(args...)
#define Hal_SC_DWIN_get_de_window(args...)
#define Hal_SC_DWIN_EnableIPAutoCoast(args...)
INTERFACE void Hal_SC_DWIN_set_422_cbcr_swap(void *pstCmdQInfo, MS_BOOL bEnable, SCALER_DIP_WIN eWindow);
INTERFACE void Hal_SC_DWIN_set_pre_align_pixel(void *pstCmdQInfo, MS_BOOL bEnable, MS_U16 pixels, SCALER_DIP_WIN eWindow);
INTERFACE void Hal_SC_DWIN_EnableR2YCSC(void *pstCmdQInfo, MS_BOOL bEnable, SCALER_DIP_WIN eWindow);
INTERFACE void Hal_SC_DWIN_Set_vsd_output_line_count(void *pstCmdQInfo, MS_BOOL bEnable, MS_U32 u32LineCount, SCALER_DIP_WIN eWindow);
INTERFACE void Hal_SC_DWIN_Set_vsd_input_line_count(void *pstCmdQInfo, MS_BOOL bEnable, MS_BOOL bUserMode, MS_U32 u32UserLineCount, SCALER_DIP_WIN eWindow);
INTERFACE void Hal_SC_DWIN_sw_db(void *pstCmdQInfo, P_SC_DIP_SWDB_INFO pDBreg, SCALER_DIP_WIN eWindow);
INTERFACE void Hal_SC_DWIN_set_input_vsync_inverse(void *pstCmdQInfo, MS_BOOL bEnable, SCALER_DIP_WIN eWindow);
//==============Set===================
INTERFACE MS_U16 HAL_XC_DIP_GetBusSize(void *pstCmdQInfo, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_SWReset(void *pstCmdQInfo, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_Init(void *pstCmdQInfo, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_SetBase0(void *pstCmdQInfo, MS_U32 u64BufStart, MS_U32 u64BufEnd, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_SetBase1(void *pstCmdQInfo, MS_U32 u64BufStart, MS_U32 u64BufEnd, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_SetWinProperty(void *pstCmdQInfo, MS_U8 u8BufCnt, MS_U16 u16Width, MS_U16 u16LineOft, MS_U16 u16Height, MS_U32 u64OffSet, SCALER_DIP_SOURCE_TYPE eSource, MS_BOOL bPIP, MS_BOOL b2P_Enable, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_SetWinProperty1(void *pstCmdQInfo, MS_U32 u64OffSet, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_SetMiuSel(void *pstCmdQInfo, MS_U8 u8MIUSel, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_SetDIPRMiuSel(void *pstCmdQInfo, MS_U8 u8YBufMIUSel, MS_U8 u8CBufMIUSel, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_CpatureOneFrame(void *pstCmdQInfo, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_CpatureOneFrame2(void *pstCmdQInfo, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_ClearIntr(void *pstCmdQInfo, MS_U16 u16mask, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_CMDQ_ClearIntr(void *pstCmdQInfo, MS_U16 u16mask, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_EnableCaptureStream(void *pstCmdQInfo, MS_BOOL bEnable, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_EnableIntr(void *pstCmdQInfo, MS_U16 u8mask, MS_BOOL bEnable, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_CMDQ_EnableIntr(void *pstCmdQInfo, MS_U16 u8mask, MS_BOOL bEnable, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_SetDataFmt(void *pstCmdQInfo, EN_DRV_XC_DWIN_DATA_FMT fmt, DIP_OUTPUT_PATH eOutPath, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_SelectSourceScanType(void *pstCmdQInfo, EN_XC_DWIN_SCAN_TYPE enScan, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_SetAlphaValue(void *pstCmdQInfo, MS_U8 u8AlphaVal, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_SetUVSwap(void *pstCmdQInfo, MS_BOOL bEnable, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_SetInterlaceWrite(void *pstCmdQInfo, MS_BOOL bEnable, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_SetY2R(void *pstCmdQInfo, MS_BOOL bEnable, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_SetYCSwap(void *pstCmdQInfo, MS_BOOL bEnable, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_SetRGBSwap(void *pstCmdQInfo, MS_BOOL bEnable, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_SetOutputCapture(void *pstCmdQInfo, MS_BOOL bEnable, EN_XC_DIP_OP_CAPTURE eOpCapture, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_Set444to422(void *pstCmdQInfo, EN_DRV_XC_DWIN_DATA_FMT fmt, MS_BOOL bSrcYUVFmt, MS_BOOL bSrcFmt422, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_SetFRC(void *pstCmdQInfo, MS_BOOL bEnable, MS_U16 u16In, MS_U16 u16Out, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_SetMirror(void *pstCmdQInfo, MS_BOOL bHMirror, MS_BOOL bVMirror, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_SetDIPRProperty(void *pstCmdQInfo, ST_XC_DIPR_PROPERTY *pstDIPRProperty, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_SetDIPRProperty_MFDEC(void *pstCmdQInfo, ST_DIP_MFDEC_INFO stDIPR_MFDecInfo, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_SetDIPRProperty_DI(void *pstCmdQInfo, ST_XC_DIPR_PROPERTY *pstDIPRProperty, ST_DIP_DIPR_3DDI_SETTING *stDIPR_DIInfo, SCALER_DIP_WIN eWindow);
//INTERFACE void HAL_XC_DIP_InterruptAttach(void *pstCmdQInfo, InterruptCb pIntCb,SCALER_DIP_WIN eWindow);
//INTERFACE void HAL_XC_DIP_InterruptDetach(void *pstCmdQInfo, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_MuxDispatch(void *pstCmdQInfo, SCALER_DIP_SOURCE_TYPE eSource, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_Rotation(void *pstCmdQInfo, MS_BOOL bRotation, EN_XC_DIP_ROTATION eRoDirection, MS_U32 u64StartAddr, SCALER_DIP_WIN eTmpWindow);
INTERFACE void HAL_XC_DIP_SetPinpon(void *pstCmdQInfo, MS_BOOL bPinpon, MS_U32 u64PinponAddr, MS_U32 u64OffSet, SCALER_DIP_WIN eWindow);
INTERFACE SCALER_DIP_WIN HAL_XC_DIP_GetHVSP(void* pstCmdQInfo, SCALER_DIP_WIN eWindow);
INTERFACE MS_BOOL HAL_XC_DIP_SetHVSP(void* pstCmdQInfo, MS_BOOL bSelect, SCALER_DIP_WIN eWindow);
INTERFACE MS_BOOL HAL_XC_DIP_Set420TileBlock(void* pstCmdQInfo, EN_XC_DIP_TILE_BLOCK eTileBlock, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_Enable(void *pstCmdQInfo, MS_BOOL bEnable);
INTERFACE void HAL_XC_DIP_SetPixelShift(void *pstCmdQInfo, MS_U16 u16OffSet, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_MDWinInit(void *pstCmdQInfo, MS_U8 u8WinId);
INTERFACE void HAL_XC_DIP_MDWin_SetConfig(void *pstCmdQInfo, DIP_MDWIN_PROPERTY *stMDWinProperty);
INTERFACE void HAL_XC_DIP_MDWIN_EnableIntr(void *pstCmdQInfo, MS_BOOL bEnable, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_MDWIN_ClearIntr(void *pstCmdQInfo, SCALER_DIP_WIN eWindow);
INTERFACE MS_U16 HAL_XC_DIP_MDWIN_GetIntrStatus(void *pstCmdQInfo, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_COVER_SetConfig(void *pstCmdQInfo, DIP_COVER_PROPERTY *stDIPCoverProperty);


//==============Get===================
INTERFACE MS_U8 HAL_XC_DIP_GetBufCnt(void *pstCmdQInfo, SCALER_DIP_WIN eWindow);
INTERFACE MS_U16 HAL_XC_DIP_GetBPP(void *pstCmdQInfo, EN_DRV_XC_DWIN_DATA_FMT fbFmt, SCALER_DIP_WIN eWindow);
INTERFACE MS_U16 HAL_XC_DIP_WidthAlignCheck(void *pstCmdQInfo, MS_U16 u16Width, MS_U16 u16Bpp, SCALER_DIP_WIN eWindow);
INTERFACE MS_U16 HAL_XC_DIP_GetIntrStatus(void *pstCmdQInfo, SCALER_DIP_WIN eWindow);
INTERFACE MS_U16 HAL_XC_DIP_CMDQ_GetIntrStatus(void *pstCmdQInfo, SCALER_DIP_WIN eWindow);
INTERFACE MS_BOOL HAL_XC_DIP_GetInterlaceWrite(void *pstCmdQInfo, SCALER_DIP_WIN eWindow);
INTERFACE EN_DRV_XC_DWIN_DATA_FMT HAL_XC_DIP_GetDataFmt(void *pstCmdQInfo, SCALER_DIP_WIN eWindow);
INTERFACE EN_XC_DWIN_SCAN_TYPE HAL_XC_DIP_GetSourceScanType(void *pstCmdQInfo, SCALER_DIP_WIN eWindow);
INTERFACE void HAL_XC_DIP_2P_Width_Check(void *pstCmdQInfo, XC_SETWIN_INFO *pstXC_SetWin_Info, SCALER_DIP_WIN eWindow);
INTERFACE MS_BOOL HAL_XC_DIP_GetCaptureSourceStatus(void *pstCmdQInfo, SCALER_DIP_SOURCE_TYPE eSource, SCALER_DIP_WIN eWindow, MS_BOOL* bstatus);
INTERFACE MS_BOOL HAL_XC_DIP_CMDQBegin(void *pstCmdQInfo, SCALER_DIP_WIN eWindow);
INTERFACE MS_BOOL HAL_XC_DIP_CMDQEnd(void *pstCmdQInfo, SCALER_DIP_WIN eWindow);

#ifdef DIP_REG_DUMP
    INTERFACE void HAL_XC_INFO_dump_Reg(MS_U16 BankNo, MS_U16 RegOffset, MS_U32 len);
    INTERFACE void HAL_MFDEC_INFO_dump_Reg(void);
#endif

#undef INTERFACE
#endif /* MHAL_DIP_H */
