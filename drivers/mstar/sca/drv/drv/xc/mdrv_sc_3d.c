////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2010 MStar Semiconductor, Inc.
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
/// file    Mdrv_sc_3d.c
/// @brief  Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////
#define DRV_SC_3D_C

#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/string.h>
#include <linux/wait.h>
#include <linux/irqreturn.h>
#else
#include <string.h>
#endif
// Common Definition
#include "MsCommon.h"
#include "MsOS.h"
#include "mhal_xc_chip_config.h"

#include "drvXC_IOPort.h"
#include "apiXC.h"
#include "mvideo_context.h"
#include "drv_sc_ip.h"

#include "mhal_sc.h"
#include "mhal_pip.h"
#include "mhal_mux.h"

#include "halCHIP.h"
#include "drv_sc_scaling.h"
#include "xc_hwreg_utility2.h"
#include "mdrv_sc_3d.h"
#include "drv_sc_display.h"
#include "drv_sc_menuload.h"
#include "drv_sc_isr.h"
#include "drvXC_HDMI_if.h"
#include "mhal_hdmi.h"

#if FRC_INSIDE
#include "mdrv_frc.h"
#include "mhal_frc.h"
#endif
//----------------------------------------
//--------3D macro define-----------------
//----------------------------------------
#define SC_3D_DBG(x)            //x

#define MS_IPM_BASE0(eWin) (MDrv_XC_GetIPMBase(0, eWin))
#define MS_IPM_BASE1(eWin) (MDrv_XC_GetIPMBase(1, eWin))

#define IS_INPUT_FRAME_PACKING(eWin)                ((_eInput3DMode[eWin] == E_XC_3D_INPUT_FRAME_PACKING))
#define IS_INPUT_FIELD_ALTERNATIVE(eWin)            ((_eInput3DMode[eWin] == E_XC_3D_INPUT_FIELD_ALTERNATIVE))
#define IS_INPUT_LINE_ALTERNATIVE(eWin)             ((_eInput3DMode[eWin] == E_XC_3D_INPUT_LINE_ALTERNATIVE))
#define IS_INPUT_SIDE_BY_SIDE_FULL(eWin)            ((_eInput3DMode[eWin] == E_XC_3D_INPUT_SIDE_BY_SIDE_FULL))
#define IS_INPUT_L_DEPTH(eWin)                      ((_eInput3DMode[eWin] == E_XC_3D_INPUT_L_DEPTH))
#define IS_INPUT_L_DEPTH_GFX_GFX_DEPTH(eWin)        ((_eInput3DMode[eWin] == E_XC_3D_INPUT_L_DEPTH_GRAPHICS_GRAPHICS_DEPTH))
#define IS_INPUT_TOP_BOTTOM(eWin)                   ((_eInput3DMode[eWin] == E_XC_3D_INPUT_TOP_BOTTOM))
#define IS_INPUT_SIDE_BY_SIDE_HALF(eWin)            ((_eInput3DMode[eWin] == E_XC_3D_INPUT_SIDE_BY_SIDE_HALF))
#define IS_INPUT_MODE_NONE(eWin)                    ((_eInput3DMode[eWin] == E_XC_3D_INPUT_MODE_NONE))
#define IS_INPUT_FRAME_ALTERNATIVE(eWin)            ((_eInput3DMode[eWin] == E_XC_3D_INPUT_FRAME_ALTERNATIVE))
#define IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWin)  ((_eInput3DMode[eWin] == E_XC_3D_INPUT_SIDE_BY_SIDE_HALF_INTERLACE))
#define IS_INPUT_FRAME_PACKING_OPT(eWin)            ((_eInput3DMode[eWin] == E_XC_3D_INPUT_FRAME_PACKING_OPT ))
#define IS_INPUT_TOP_BOTTOM_OPT(eWin)               ((_eInput3DMode[eWin] == E_XC_3D_INPUT_TOP_BOTTOM_OPT ))
#define IS_INPUT_NORMAL_2D(eWin)                    (_eInput3DMode[eWin] == E_XC_3D_INPUT_NORMAL_2D )
#define IS_INPUT_NORMAL_2D_INTERLACE(eWin)          (_eInput3DMode[eWin] == E_XC_3D_INPUT_NORMAL_2D_INTERLACE )
#define IS_INPUT_NORMAL_2D_INTERLACE_PTP(eWin)      (_eInput3DMode[eWin] == E_XC_3D_INPUT_NORMAL_2D_INTERLACE_PTP )
#define IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE_OPT(eWin) (_eInput3DMode[eWin] == E_XC_3D_INPUT_SIDE_BY_SIDE_HALF_INTERLACE_OPT )
#define IS_INPUT_NORMAL_2D_HW(eWin)                 (_eInput3DMode[eWin] == E_XC_3D_INPUT_NORMAL_2D_HW )
#define IS_INPUT_CHECK_BOARD(eWin)                  (_eInput3DMode[eWin] == E_XC_3D_INPUT_CHECK_BORAD)

#define IS_OUTPUT_MODE_NONE()                       ((_eOutput3DMode == E_XC_3D_OUTPUT_MODE_NONE))
#define IS_OUTPUT_LINE_ALTERNATIVE()                ((_eOutput3DMode == E_XC_3D_OUTPUT_LINE_ALTERNATIVE))
#define IS_OUTPUT_TOP_BOTTOM()                      ((_eOutput3DMode == E_XC_3D_OUTPUT_TOP_BOTTOM))
#define IS_OUTPUT_SIDE_BY_SIDE_HALF()               ((_eOutput3DMode == E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF))
#define IS_OUTPUT_FRAME_ALTERNATIVE()               ((_eOutput3DMode == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE))
#define IS_OUTPUT_FRAME_L()                         ((_eOutput3DMode == E_XC_3D_OUTPUT_FRAME_L))
#define IS_OUTPUT_FRAME_R()                         ((_eOutput3DMode == E_XC_3D_OUTPUT_FRAME_R))
#define IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC()         ((_eOutput3DMode == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE_NOFRC ))
#define IS_OUTPUT_LINE_ALTERNATIVE_HW()             ((_eOutput3DMode == E_XC_3D_OUTPUT_LINE_ALTERNATIVE_HW))
#define IS_OUTPUT_PIXEL_ALTERNATIVE_HW()            ((_eOutput3DMode == E_XC_3D_OUTPUT_PIXEL_ALTERNATIVE_HW))
#define IS_OUTPUT_CHECKBOARD_HW()                   ((_eOutput3DMode == E_XC_3D_OUTPUT_CHECKBOARD_HW))
#define IS_OUTPUT_FRAME_L_HW()                      ((_eOutput3DMode == E_XC_3D_OUTPUT_FRAME_L_HW))
#define IS_OUTPUT_FRAME_R_HW()                      ((_eOutput3DMode == E_XC_3D_OUTPUT_FRAME_R_HW))

#ifndef MAX
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif

#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

//hw 2d to 3d macro
#define HW_2DTO3D_DEFAULT_CONCAVE                       0x0006
#define HW_2DTO3D_DEFAULT_ARTIFICIALGAIN                0x0000
#define HW_2DTO3D_DEFAULT_GAIN                          0x001E
#define HW_2DTO3D_DEFAULT_OFFSET                        0x00FE
#define HW_2DTO3D_DEFAULT_ELESEL                        0x0002
#define HW_2DTO3D_DEFAULT_MODSEL                        0x0003

//detect 3d format macro
#define DETECT3DFORMAT_DEFAULT_HORSEARCHRANGE           80
#define DETECT3DFORMAT_DEFAULT_VERSEARCHRANGE           1
#define DETECT3DFORMAT_DEFAULT_GYPIXELTHRESHOLD         8
#define DETECT3DFORMAT_DEFAULT_RCRPIXELTHRESHOLD        8
#define DETECT3DFORMAT_DEFAULT_BCBPIXELTHRESHOLD        8
#define DETECT3DFORMAT_DEFAULT_HORSAMPLECOUNT           6
#define DETECT3DFORMAT_DEFAULT_VERSAMPLECOUNT           6
#define DETECT3DFORMAT_DEFAULT_MAXCHECKINGFRAMECOUNT    256
#define DETECT3DFORMAT_DEFAULT_HITPIXELPERCENTAGE       70

//--------------------------------------------
//--------3D struct define-----------------
//--------------------------------------------
typedef struct
{
    MS_BOOL                 bSetCusCropWindow[MAX_CHANNEL];
    MS_WINDOW_TYPE          stCus3DCropWindow[MAX_CHANNEL];

    MS_BOOL                 bSetCusDispWindow[MAX_CHANNEL];
    MS_WINDOW_TYPE          stCus3DDispWindow[MAX_CHANNEL];

    MS_BOOL                 bDeMainPipSignal;
    MS_BOOL                 bMainFirst;

    MS_BOOL                 bSetVFreq;
    FREQUENCY_TO_PANEL      eOutputVFreq;
    MAIN_PIP_OUTPUT_TYPE    eSyncType;

    MS_BOOL                 bDeMainPipSignal_Ver;
    MS_BOOL                 bMainFirst_Ver;
}Cus_3DProcess;

//--------------------------------------------
//--------3D static variables-----------------
//--------------------------------------------
static E_XC_3D_INPUT_MODE _eInput3DMode[MAX_WINDOW]; // main and sub win could have different 3d input mode
static E_XC_3D_OUTPUT_MODE _eOutput3DMode; // main and sub win could only have the same 3d input mode
static E_XC_3D_PANEL_TYPE _e3DPanelType; // main and sub win could only have the same 3d input mode
static MS_BOOL  _b3DMainFirst; // main and sub win start address exchange
static Cus_3DProcess _stCus3DProcess;    // struct for Custom 3D API
static MS_U16   _u163DHShift;            // horizontal shift pixel
#if HW_2DTO3D_SUPPORT
static MS_PHYADDR _u32HW2DTO3D_DD_Buf;   //hw 2D to 3D depth detect buffer
static MS_PHYADDR _u32HW2DTO3D_DR_Buf;   //hw 2D to 3D depth render buffer
static MS_XC_3D_HW2DTO3D_PARA _st3DHw2DTo3DPara; //hw 2d to 3d para
static MS_BOOL _bIsHW2Dto3DPatchEnabled;
static MS_U16  _u16HW2Dto3DCurrentLR;
#endif
#if (HW_DESIGN_3D_VER < 2)
static MS_BOOL  _bFrmpackToLinealterOpt;                    //optimize falg for framepacking in,line alternative out
#endif
static MS_BOOL  _bIsLR_Sbs2Line;                            //is sbs2line case
static MS_BOOL  _bSkipDefaultLRFlag;                        //do we need to skip lib default LR flag report, and using the Ap layer's setting?
static MS_XC_3D_DETECT3DFORMAT_PARA _stDetect3DFormatPara;  //detect 3d format para

//----------------------------------------
//--------3D internal interface-----------
//----------------------------------------
void MDrv_SC_3D_Variable_Init(void)
{
    _eInput3DMode[MAIN_WINDOW]   = E_XC_3D_INPUT_MODE_NONE;
    _eInput3DMode[SUB_WINDOW]    = E_XC_3D_INPUT_MODE_NONE;
#ifdef MULTI_SCALER_SUPPORTED
    _eInput3DMode[SC1_MAIN_WINDOW]   = E_XC_3D_INPUT_MODE_NONE;
    _eInput3DMode[SC2_MAIN_WINDOW]   = E_XC_3D_INPUT_MODE_NONE;
    _eInput3DMode[SC2_SUB_WINDOW]    = E_XC_3D_INPUT_MODE_NONE;
#endif

    _eOutput3DMode               = E_XC_3D_OUTPUT_MODE_NONE;
    _e3DPanelType                = E_XC_3D_PANEL_NONE;
    _b3DMainFirst                = TRUE;
    _u163DHShift = 0;
    memset(&_stCus3DProcess, 0, sizeof(Cus_3DProcess));
#if HW_2DTO3D_SUPPORT
    _u32HW2DTO3D_DD_Buf = 0;
    _u32HW2DTO3D_DR_Buf = 0;
    _st3DHw2DTo3DPara.u32Hw2dTo3dPara_Version = HW2DTO3DPARA_VERSION;
    _st3DHw2DTo3DPara.u16Concave = HW_2DTO3D_DEFAULT_CONCAVE;
    _st3DHw2DTo3DPara.u16ArtificialGain = HW_2DTO3D_DEFAULT_ARTIFICIALGAIN;
    _st3DHw2DTo3DPara.u16Gain = HW_2DTO3D_DEFAULT_GAIN;
    _st3DHw2DTo3DPara.u16Offset = HW_2DTO3D_DEFAULT_OFFSET;
    _st3DHw2DTo3DPara.u16EleSel = HW_2DTO3D_DEFAULT_ELESEL;
    _st3DHw2DTo3DPara.u16ModSel = HW_2DTO3D_DEFAULT_MODSEL;
    _bIsHW2Dto3DPatchEnabled = FALSE;
    _u16HW2Dto3DCurrentLR = 0x0004;
#endif
#if (HW_DESIGN_3D_VER < 2)
    _bFrmpackToLinealterOpt      = FALSE;
#endif
    _bIsLR_Sbs2Line = FALSE;
    _bSkipDefaultLRFlag = FALSE;
    _stDetect3DFormatPara.u32Detect3DFormatPara_Version = DETECT3DFORMATPARA_VERSION;
    _stDetect3DFormatPara.u16HorSearchRange             = DETECT3DFORMAT_DEFAULT_HORSEARCHRANGE;
    _stDetect3DFormatPara.u16VerSearchRange             = DETECT3DFORMAT_DEFAULT_VERSEARCHRANGE;
    _stDetect3DFormatPara.u16GYPixelThreshold           = DETECT3DFORMAT_DEFAULT_GYPIXELTHRESHOLD;
    _stDetect3DFormatPara.u16RCrPixelThreshold          = DETECT3DFORMAT_DEFAULT_RCRPIXELTHRESHOLD;
    _stDetect3DFormatPara.u16BCbPixelThreshold          = DETECT3DFORMAT_DEFAULT_BCBPIXELTHRESHOLD;
    _stDetect3DFormatPara.u16HorSampleCount             = DETECT3DFORMAT_DEFAULT_HORSAMPLECOUNT;
    _stDetect3DFormatPara.u16VerSampleCount             = DETECT3DFORMAT_DEFAULT_VERSAMPLECOUNT;
    _stDetect3DFormatPara.u16MaxCheckingFrameCount      = DETECT3DFORMAT_DEFAULT_MAXCHECKINGFRAMECOUNT;
    _stDetect3DFormatPara.u16HitPixelPercentage         = DETECT3DFORMAT_DEFAULT_HITPIXELPERCENTAGE;
}

// call this function to know whether current 3D status is 48hz required.
// if it is 48hz output, we need to adjust the logic of framelock judgement.
MS_BOOL MDrv_SC_3D_IsOutputVfreq48hz(SCALER_WIN eWindow, MS_U16 u16InputVFreq)
{
    MS_ASSERT(eWindow < MAX_WINDOW);

    if(eWindow >= MAX_WINDOW)
    {
        return FALSE;
    }

    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW, "@@ gSrcInfo[eWindow].stCapWin.height=%u, Input Vfreq=%u\n",gSrcInfo[eWindow].stCapWin.height, u16InputVFreq)
    if(IS_OUTPUT_FRAME_ALTERNATIVE()
       && (u16InputVFreq >= 230)
       && (u16InputVFreq <= 245)
      )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

MS_BOOL MDrv_SC_3D_AdjustPreVerSrcForFramepacking(SCALER_WIN eWindow)
{
    MS_ASSERT(eWindow < MAX_WINDOW);

    if(eWindow >= MAX_WINDOW)
    {
        return FALSE;
    }

#if (HW_DESIGN_3D_VER >= 2)
    return FALSE;
#else
    if(IS_INPUT_FRAME_PACKING(eWindow)
           && IS_OUTPUT_LINE_ALTERNATIVE()
           && _bFrmpackToLinealterOpt)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
#endif
}

MS_BOOL MDrv_SC_3D_Is2TapModeSupportedFormat(XC_InternalStatus *pSrcInfo, SCALER_WIN eWindow)
{
    MS_ASSERT(eWindow < MAX_WINDOW);
    MS_ASSERT(pSrcInfo);

    if(eWindow >= MAX_WINDOW)
    {
        return FALSE;
    }

    if(pSrcInfo == NULL)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: pSrcInfo is null pointer!\r\n");
        return FALSE;
    }

    //LBL to LBL, TB to LBL, FI to LBL can use 2 tap mode to avoid line buffer limitation
    //SBS to LBL don't need use 2 tap mode, since it's op only fetch 960 pixel
    //FP  to LBL and sw 2d to 3d have fclk issue, need prescaling down, so can not use 2 tap mode
    //interlace signal can not use 2 tap mode
    //if capture.width<=lblength, no need to use 2 tap mode
#if (HW_DESIGN_3D_VER >= 2)
    return  ((!pSrcInfo->bInterlace)
               && (pSrcInfo->stCapWin.width > (SCALER_LINE_BUFFER_MAX + SUB_SCALER_LINE_BUFFER_MAX)/2)
               && ((IS_INPUT_LINE_ALTERNATIVE(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
                    || (IS_INPUT_TOP_BOTTOM(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
                    || (IS_INPUT_FRAME_ALTERNATIVE(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())));
#else
    return  ((!pSrcInfo->bInterlace)
               && (pSrcInfo->stCapWin.width > (SCALER_LINE_BUFFER_MAX + SUB_SCALER_LINE_BUFFER_MAX)/2)
               && ((IS_INPUT_LINE_ALTERNATIVE(eWindow) && IS_OUTPUT_TOP_BOTTOM())
                    || (IS_INPUT_TOP_BOTTOM(eWindow) && IS_OUTPUT_TOP_BOTTOM())
                    || (IS_INPUT_FRAME_ALTERNATIVE(eWindow) && IS_OUTPUT_TOP_BOTTOM()))
               && MDrv_SC_3D_Is_LR_Sbs2Line());
#endif
}

MS_BOOL MDrv_SC_3D_Set2TapMode(XC_InternalStatus *pSrcInfo, SCALER_WIN eWindow)
{
    MS_ASSERT(eWindow < MAX_WINDOW);
    MS_ASSERT(pSrcInfo);

    if(eWindow >= MAX_WINDOW)
    {
        return FALSE;
    }

    if(pSrcInfo == NULL)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: pSrcInfo is null pointer!\r\n");
        return FALSE;
    }

    if(MDrv_SC_3D_Is2TapModeSupportedFormat(pSrcInfo, eWindow))
    {
        SC_W2BYTEMSK(REG_SC_BK20_10_L, BIT(5), BIT(5)); //Enable HD side by side line buffer mode
        //The starting address of f1 stored at line buffer
        //Main and Sub window will use the (1920+960) in 2Tap st the same time
        SC_W2BYTEMSK(REG_SC_BK20_13_L, 0x00, 0x0FFF);
        // When main and sub is start from 0, it means the main&sub win don't need Fill the sub windows line buffer in vertical blanking
        SC_W2BYTEMSK(REG_SC_BK20_10_L, 0x00, BIT(10)); //[11]reg_vblank_main
        SC_W2BYTEMSK(REG_SC_BK20_10_L, 0x00, BIT(11)); //[11]reg_vblank_sub

        //LB offset should be even when 2 tap mode
        if(stDBreg.u16LBOffset > 0)
        {
            stDBreg.u16LBOffset = (stDBreg.u16LBOffset - 0x1) & ~0x1;
        }
    }
    else
    {
        SC_W2BYTEMSK(REG_SC_BK20_10_L, 0x0, BIT(5)); //Disable HD side by side line buffer mode
    }
    return TRUE;
}

MS_BOOL MDrv_SC_3D_PostPQSetting(XC_InternalStatus *pSrcInfo, SCALER_WIN eWindow)
{
    MS_ASSERT(eWindow < MAX_WINDOW);
    MS_ASSERT(pSrcInfo);

    if(eWindow >= MAX_WINDOW)
    {
        return FALSE;
    }

    if(pSrcInfo == NULL)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: pSrcInfo is null pointer!\r\n");
        return FALSE;
    }

    if(MDrv_SC_3D_Is2TapModeSupportedFormat(pSrcInfo, eWindow))
    {
        //Set SRAM to linear mode
        SC_W2BYTE(REG_SC_BK23_0B_L, 0x0303);
        //disable snr_vertical vertical c low pass and spike NR
        SC_W2BYTEMSK(REG_SC_BK26_50_L, 0x0000, 0x00FF);
    }
    return TRUE;
}

MS_BOOL MDrv_SC_3D_IsIPMFetchNoLimitCase(SCALER_WIN eWindow)
{
    MS_ASSERT(eWindow < MAX_WINDOW);

    if(eWindow >= MAX_WINDOW)
    {
        return FALSE;
    }

#if (HW_DESIGN_3D_VER >= 2)
    return FALSE;
#else
    if((IS_INPUT_FRAME_PACKING(eWindow)
          && (gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_720X2P_VSIZE))
       ||
       IS_INPUT_SIDE_BY_SIDE_HALF(eWindow)
       ||
       MDrv_SC_3D_Is2TapModeSupportedFormat(&gSrcInfo[eWindow], eWindow)
       )
    {
        //no any limits
        return TRUE;
    }
    else
    {
        return FALSE;
    }
#endif
}

void MDrv_SC_3D_Adjust_FillLBMode(SCALER_WIN eWindow)
{
    MS_ASSERT(eWindow < MAX_WINDOW);

    _XC_ENTRY();

#if (HW_DESIGN_3D_VER >= 2)
    if(IS_OUTPUT_TOP_BOTTOM()
       || IS_OUTPUT_SIDE_BY_SIDE_HALF()
       || IS_OUTPUT_LINE_ALTERNATIVE()
      )
    {
        MDrv_XC_FilLineBuffer(TRUE, SUB_WINDOW);
    }
    else
    {
        MDrv_XC_FilLineBuffer(FALSE, SUB_WINDOW);
    }
#else
    MS_BOOL bEnable = FALSE;
    if ( IS_OUTPUT_MODE_NONE() )
    {
        bEnable = TRUE;  // RD suggestion.
    }
    else
    {
        if ( (IS_OUTPUT_SIDE_BY_SIDE_HALF() || IS_OUTPUT_TOP_BOTTOM())
            && (_b3DMainFirst ^ (eWindow == MAIN_WINDOW))) //only trigger the later one
            bEnable = TRUE;
        else
            bEnable = FALSE;
    }
    if (eWindow == SUB_WINDOW)
    {
        MDrv_XC_FilLineBuffer(bEnable, SUB_WINDOW);
    }
#endif

    // Designer Jeff.Lin: disable fill line buffer of main_window forever.
    if (eWindow == MAIN_WINDOW)
    {
        MDrv_XC_FilLineBuffer(DISABLE, MAIN_WINDOW);
    }

    _XC_RETURN();
}

void MDrv_SC_3D_Adjust_PreVLength(INPUT_SOURCE_TYPE_t enInputSourceType,
                                  XC_InternalStatus *pSrcInfo,
                                  SCALER_WIN eWindow)
{
    MS_ASSERT(eWindow < MAX_WINDOW);
    MS_ASSERT(pSrcInfo);

    if(pSrcInfo == NULL)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: pSrcInfo is null pointer!\r\n");
        return;
    }

    if(eWindow >= MAX_WINDOW)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: eWindow is wrong!\r\n");
        return;
    }

#if (HW_DESIGN_3D_VER >= 2)
    return;
#else
    SC_3D_DBG(printf("MDrv_SC_3D_Adjust_PreVLength(%s): from %u\n", eWindow?"Subwindow":"Mainwindow",pSrcInfo->u16V_Length);)
    if(((IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) && (!pSrcInfo->bInterlace))
       || IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow)
       || IS_INPUT_NORMAL_2D_INTERLACE(eWindow)
       || IS_INPUT_NORMAL_2D_INTERLACE_PTP(eWindow)
       || IS_INPUT_NORMAL_2D(eWindow)) &&
       IS_OUTPUT_LINE_ALTERNATIVE())
    {
        pSrcInfo->u16V_Length *= 2;
    }
    else if((IS_INPUT_LINE_ALTERNATIVE(eWindow) &&
               IS_OUTPUT_TOP_BOTTOM())
               ||
               (IS_INPUT_NORMAL_2D(eWindow) &&
               IS_OUTPUT_TOP_BOTTOM())
               ||
               (IS_INPUT_TOP_BOTTOM(eWindow) &&
               IS_OUTPUT_TOP_BOTTOM())
               ||
               (IS_INPUT_TOP_BOTTOM(eWindow) &&
               IS_OUTPUT_SIDE_BY_SIDE_HALF()))
    {
        pSrcInfo->u16V_Length /= 2;
    }
    else if((IS_INPUT_FRAME_PACKING(eWindow) && IS_OUTPUT_TOP_BOTTOM())
            || (IS_INPUT_FRAME_PACKING(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF()))
    {
        if(gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_720X2P_VSIZE)
        {
            pSrcInfo->u16V_Length = DOUBLEHD_720X2P_FRAME_VSIZE;
        }
        else if(gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_1080X2P_VSIZE)
        {
            pSrcInfo->u16V_Length = DOUBLEHD_1080X2P_FRAME_VSIZE;
        }
        else if(gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_1080X2I_VSIZE)
        {
            pSrcInfo->u16V_Length = DOUBLEHD_1080X2I_FRAME_VSIZE;
        }
    }
    else if((IS_INPUT_LINE_ALTERNATIVE(eWindow) || IS_INPUT_TOP_BOTTOM(eWindow))
            && IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC())
    {
        pSrcInfo->u16V_Length /= 2;
    }
    else if(IS_INPUT_FRAME_ALTERNATIVE(eWindow) &&
               IS_OUTPUT_LINE_ALTERNATIVE())
    {
        pSrcInfo->u16V_Length = pSrcInfo->stDispWin.height;
    }
    else  if(IS_INPUT_FRAME_PACKING(eWindow) &&
            (IS_OUTPUT_FRAME_ALTERNATIVE() || IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC()))
    {
        if(gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_720X2P_VSIZE)
        {
            pSrcInfo->u16V_Length = DOUBLEHD_720X2P_FRAME_VSIZE;
        }
        else if(gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_1080X2P_VSIZE)
        {
            pSrcInfo->u16V_Length = DOUBLEHD_1080X2P_FRAME_VSIZE;
        }
    }
    else if (IS_INPUT_FRAME_PACKING(eWindow) &&
        (IS_OUTPUT_FRAME_L() || IS_OUTPUT_FRAME_R()))
    {
        if(gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_720X2P_VSIZE)
        {
            pSrcInfo->u16V_Length = DOUBLEHD_720X2P_FRAME_VSIZE;
        }
        else if(gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_1080X2P_VSIZE)
        {
            pSrcInfo->u16V_Length = DOUBLEHD_1080X2P_FRAME_VSIZE;
        }
        else if(gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_1080X2I_VSIZE)
        {
            pSrcInfo->u16V_Length = DOUBLEHD_1080X2I_FRAME_VSIZE;
        }
    }
    else if ((IS_INPUT_TOP_BOTTOM(eWindow) || IS_INPUT_LINE_ALTERNATIVE(eWindow)) &&
        (IS_OUTPUT_FRAME_L() || IS_OUTPUT_FRAME_R()))
    {
        pSrcInfo->u16V_Length /= 2;
    }
    else if (IS_INPUT_FRAME_ALTERNATIVE(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
    {
        pSrcInfo->u16V_Length *= 2;
    }
    if((SUB_WINDOW == eWindow)
       && MDrv_SC_3D_IsUseSameBuffer()
       && (IS_OUTPUT_TOP_BOTTOM() || IS_OUTPUT_SIDE_BY_SIDE_HALF())
       )
    {
        pSrcInfo->u16V_Length = gSrcInfo[MAIN_WINDOW].u16V_Length;
    }

    SC_3D_DBG(printf("MDrv_SC_3D_Adjust_PreVLength(%s): to %u\n",eWindow?"Subwindow":"Mainwindow",pSrcInfo->u16V_Length);)
#endif
}

void MDrv_SC_3D_Adjust_PreHorDstSize(INPUT_SOURCE_TYPE_t enInputSourceType,
                                     XC_InternalStatus *pSrcInfo,
                                     SCALER_WIN eWindow)
{
    MS_ASSERT(eWindow < MAX_WINDOW);
    MS_ASSERT(pSrcInfo);

    if(pSrcInfo == NULL)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: pSrcInfo is null pointer!\r\n");
        return;
    }

    if(eWindow >= MAX_WINDOW)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: eWindow is wrong!\r\n");
        return;
    }

#if HW_2DTO3D_SUPPORT
#define HW_2DTO3DBUFFERSIZE  0x200000
    static MS_BOOL bAdjustDNRSize = FALSE;
    static MS_U32    u32OriginalDNRSize = 0;
    if(IS_INPUT_NORMAL_2D_HW(eWindow))
    {
        if(((_u32HW2DTO3D_DD_Buf >= (MS_IPM_BASE0(eWindow) * BYTE_PER_WORD)) && (_u32HW2DTO3D_DD_Buf < ((MS_IPM_BASE0(eWindow) * BYTE_PER_WORD) + MDrv_XC_GetDNRBufSize(eWindow))))
            || ((_u32HW2DTO3D_DR_Buf >= (MS_IPM_BASE0(eWindow) * BYTE_PER_WORD)) && (_u32HW2DTO3D_DR_Buf < ((MS_IPM_BASE0(eWindow) * BYTE_PER_WORD) + MDrv_XC_GetDNRBufSize(eWindow))))
          )
        {
            SC_3D_DBG(printf("scaler dnr buffer is overflow with hw 2d to 3d buffer\n");)
            //E_MMAP_ID_XC_2DTO3D_DD_BUF_LEN+E_MMAP_ID_XC_2DTO3D_DR_BUF_LEN:max:HW_2DTO3DBUFFERSIZE
            u32OriginalDNRSize = MDrv_XC_GetDNRBufSize(eWindow);
            MDrv_XC_SetDNRBufSize(MDrv_XC_GetDNRBufSize(eWindow) - HW_2DTO3DBUFFERSIZE, eWindow);
            bAdjustDNRSize = TRUE;
            MS_U32 u32TempWidth = MApi_XC_GetAvailableSize(eWindow,
                                                           2,
                                                           pSrcInfo->u16V_SizeAfterPreScaling);
            if(pSrcInfo->u16H_SizeAfterPreScaling > u32TempWidth)
            {
                pSrcInfo->u16H_SizeAfterPreScaling = u32TempWidth & ~(OFFSET_PIXEL_ALIGNMENT -1);
            }
        }
    }
    else if(bAdjustDNRSize && ((MDrv_XC_GetDNRBufSize(eWindow) + HW_2DTO3DBUFFERSIZE) <= u32OriginalDNRSize))
    {
        MDrv_XC_SetDNRBufSize(MDrv_XC_GetDNRBufSize(eWindow) + HW_2DTO3DBUFFERSIZE, eWindow);
        bAdjustDNRSize = FALSE;
    }
#endif

#if (HW_DESIGN_3D_VER >= 4)
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW, "PreHorDstSize(%s): %u\n",eWindow?"Subwindow":"Mainwindow",pSrcInfo->u16H_SizeAfterPreScaling);
    return;
#elif (HW_DESIGN_3D_VER >= 2)
    if(eWindow == MAIN_WINDOW)
    {
        if (pSrcInfo->u16H_SizeAfterPreScaling > MST_LINE_BFF_MAX)
            pSrcInfo->u16H_SizeAfterPreScaling = MST_LINE_BFF_MAX;
    }
    else
    {
        // sub window
        if(PIP_SUPPORTED)
        {
            if (pSrcInfo->u16H_SizeAfterPreScaling > SUB_MST_LINE_BFF_MAX)
                pSrcInfo->u16H_SizeAfterPreScaling = SUB_MST_LINE_BFF_MAX;
        }
    }
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW, "PreHorDstSize(%s): %u\n",eWindow?"Subwindow":"Mainwindow",pSrcInfo->u16H_SizeAfterPreScaling);
    return;
#else

    //need do prescaling for 3D case especially when pip use same memory
    if(!(IS_INPUT_MODE_NONE(eWindow) && IS_OUTPUT_MODE_NONE()))
    {
        if(pSrcInfo->u16H_SizeAfterPreScaling > pSrcInfo->stDispWin.width)
        {
            pSrcInfo->u16H_SizeAfterPreScaling = pSrcInfo->stDispWin.width;
        }
    }

    SC_3D_DBG(printf("MDrv_SC_3D_Adjust_PreHorDstSize(%s): from %u\n",eWindow?"Subwindow":"Mainwindow",pSrcInfo->u16H_SizeAfterPreScaling);)

    if(IS_INPUT_FRAME_PACKING(eWindow) &&
        IS_OUTPUT_FRAME_ALTERNATIVE())
    {
        //pSrcInfo->u16H_SizeAfterPreScaling = pSrcInfo->stDispWin.width / 2;
#if 1
        MS_U32 u32TempWidth = MApi_XC_GetAvailableSize(MAIN_WINDOW,
                                                       2,
                                                       pSrcInfo->u16V_SizeAfterPreScaling);
        if(pSrcInfo->u16H_SizeAfterPreScaling > u32TempWidth)
        {
            pSrcInfo->u16H_SizeAfterPreScaling = u32TempWidth & ~(OFFSET_PIXEL_ALIGNMENT -1);
        }
#endif
    }
    else if(((IS_INPUT_LINE_ALTERNATIVE(eWindow) || IS_INPUT_TOP_BOTTOM(eWindow)
            || IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) || IS_INPUT_FRAME_PACKING(eWindow)
            || IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow))
            && IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC())
            || (IS_INPUT_NORMAL_2D(eWindow) && IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC())
            || (IS_INPUT_FRAME_ALTERNATIVE(eWindow) && IS_OUTPUT_FRAME_L())
            || (IS_INPUT_FRAME_ALTERNATIVE(eWindow) && IS_OUTPUT_FRAME_R())
            || (IS_INPUT_FRAME_ALTERNATIVE(eWindow) && IS_OUTPUT_TOP_BOTTOM())
            )
    {
        MS_U32 u32TempWidth = MApi_XC_GetAvailableSize(MAIN_WINDOW,
                                                       4 /* 4 frame mode */,
                                                       pSrcInfo->u16V_SizeAfterPreScaling);
        // for memory reason, need support 4 frame mode
        if(pSrcInfo->u16H_SizeAfterPreScaling > u32TempWidth)
        {
            pSrcInfo->u16H_SizeAfterPreScaling = u32TempWidth & ~(OFFSET_PIXEL_ALIGNMENT -1);
        }
    }
    else if(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF())
    {
        //for best quality of side by side in, side by side out
        pSrcInfo->u16H_SizeAfterPreScaling = pSrcInfo->stCapWin.width;
    }

    if(((IS_INPUT_SIDE_BY_SIDE_HALF(eWindow)
       || IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow)) &&
       IS_OUTPUT_LINE_ALTERNATIVE())
       ||
       (IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) &&
       IS_OUTPUT_SIDE_BY_SIDE_HALF() && (pSrcInfo->stCapWin.width != pSrcInfo->stDispWin.width))
       ||
       (IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) &&
       IS_OUTPUT_TOP_BOTTOM())
       ||
#if (HW_DESIGN_3D_VER < 1)
       (IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) &&
       IS_OUTPUT_FRAME_ALTERNATIVE())
       ||
#endif
       (IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) &&
       IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC())
       ||
       (IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow) && (IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC()))
       ||
       (IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) &&
       (IS_OUTPUT_FRAME_L() || IS_OUTPUT_FRAME_R()))
       ||
       (IS_INPUT_FRAME_PACKING(eWindow) &&
       IS_OUTPUT_FRAME_ALTERNATIVE())
       ||
       (IS_INPUT_FRAME_PACKING(eWindow) &&
       IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC())
       ||
       (IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE_OPT(eWindow) &&
       IS_OUTPUT_LINE_ALTERNATIVE())
       )
    {
        MS_U16 u16AlignedWidth = (pSrcInfo->u16H_SizeAfterPreScaling) & ~(OFFSET_PIXEL_ALIGNMENT*2 -1);
        //SC_3D_DBG(printf("3D: pre scaling: pSrcInfo->u16H_SizeAfterPreScaling original---%u\n",pSrcInfo->u16H_SizeAfterPreScaling);)
        pSrcInfo->u16H_SizeAfterPreScaling = u16AlignedWidth;
        //SC_3D_DBG(printf("3D: pre scaling: pSrcInfo->u16H_SizeAfterPreScaling Aligned---%u\n",pSrcInfo->u16H_SizeAfterPreScaling);)
    }

    if(MDrv_SC_3D_IsIPMFetchNoLimitCase(eWindow))
    {
    }
    else if(IS_OUTPUT_TOP_BOTTOM() || IS_OUTPUT_SIDE_BY_SIDE_HALF())
    {
        // line buffer check
        if (pSrcInfo->u16H_SizeAfterPreScaling > MS_3D_LINE_BFF_MAX)
        {
            pSrcInfo->u16H_SizeAfterPreScaling = MS_3D_LINE_BFF_MAX;
        }
    }
    else
    {
        // line buffer check
        if(eWindow == MAIN_WINDOW)
        {
            if (pSrcInfo->u16H_SizeAfterPreScaling > MST_LINE_BFF_MAX)
                pSrcInfo->u16H_SizeAfterPreScaling = MST_LINE_BFF_MAX;
        }
        else
        {
            // sub window
            if(PIP_SUPPORTED)
            {
                if (pSrcInfo->u16H_SizeAfterPreScaling > SUB_MST_LINE_BFF_MAX)
                    pSrcInfo->u16H_SizeAfterPreScaling = SUB_MST_LINE_BFF_MAX;
            }
        }
    }

    if((IS_INPUT_FRAME_PACKING(eWindow) && IS_OUTPUT_TOP_BOTTOM())
       || (IS_INPUT_FRAME_PACKING(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF())
       || (IS_INPUT_FRAME_PACKING(eWindow) && IS_OUTPUT_FRAME_L())
       || (IS_INPUT_FRAME_PACKING(eWindow) && IS_OUTPUT_FRAME_R())
       )
    {
        if(IS_INPUT_FRAME_PACKING(eWindow)
           && IS_OUTPUT_TOP_BOTTOM()
           && (gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_1080X2P_VSIZE))
        {
            //do more prescaling down to fclk issue for 1080p fp:the following formula is from experiment
            MS_U16 u16TempWidth = 0;
            if(MDrv_SC_3D_Is_LR_Sbs2Line()) //output line by line
            {
                u16TempWidth = 1920 * (MS_U32)pSrcInfo->stDispWin.height * 13 / 10800;
            }
            else
            {
                u16TempWidth = 640;
            }
            pSrcInfo->u16H_SizeAfterPreScaling = pSrcInfo->u16H_SizeAfterPreScaling > u16TempWidth ?
                    u16TempWidth : pSrcInfo->u16H_SizeAfterPreScaling;
        }
        MS_U32 u32TempWidth = MApi_XC_GetAvailableSize(MAIN_WINDOW,
                                                       2,
                                                       pSrcInfo->u16V_SizeAfterPreScaling);
        if(pSrcInfo->u16H_SizeAfterPreScaling > u32TempWidth)
        {
            pSrcInfo->u16H_SizeAfterPreScaling = u32TempWidth & ~(OFFSET_PIXEL_ALIGNMENT -1);
        }
    }
#if (HW_DESIGN_3D_VER == 0)
    else if(IS_INPUT_LINE_ALTERNATIVE(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF())
    {
        if(pSrcInfo->stCapWin.width > MST_LINE_BFF_MAX / 2)
        {
            pSrcInfo->u16H_SizeAfterPreScaling = (MST_LINE_BFF_MAX / 2) & ~(OFFSET_PIXEL_ALIGNMENT - 1);
        }
        else
        {
            pSrcInfo->u16H_SizeAfterPreScaling = pSrcInfo->stCapWin.width & ~(OFFSET_PIXEL_ALIGNMENT - 1);
        }
    }
#endif
    else if(IS_INPUT_FRAME_ALTERNATIVE(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF())
    {
        MS_U32 u32TempWidth;
        pSrcInfo->u16H_SizeAfterPreScaling = pSrcInfo->stCapWin.width;
        u32TempWidth = MApi_XC_GetAvailableSize(MAIN_WINDOW,
                                                4,
                                                pSrcInfo->u16V_SizeAfterPreScaling);
        // for memory reason, need support 4 frame mode
        if(pSrcInfo->u16H_SizeAfterPreScaling > u32TempWidth)
        {
            pSrcInfo->u16H_SizeAfterPreScaling = u32TempWidth;
        }
#if (HW_DESIGN_3D_VER == 0)
        if(pSrcInfo->u16H_SizeAfterPreScaling > MST_LINE_BFF_MAX / 2)
        {
            pSrcInfo->u16H_SizeAfterPreScaling = MST_LINE_BFF_MAX / 2;
        }
#endif
        pSrcInfo->u16H_SizeAfterPreScaling &= ~(OFFSET_PIXEL_ALIGNMENT - 1);
    }
    else if((IS_INPUT_FRAME_PACKING_OPT(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF())
             || (IS_INPUT_TOP_BOTTOM_OPT(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF()))
    {
        MS_U32 u32TempWidth;
        pSrcInfo->u16H_SizeAfterPreScaling = pSrcInfo->stCapWin.width;
        u32TempWidth = MApi_XC_GetAvailableSize(MAIN_WINDOW,
                                                2,
                                                pSrcInfo->u16V_SizeAfterPreScaling);
        if(pSrcInfo->u16H_SizeAfterPreScaling > u32TempWidth)
        {
            pSrcInfo->u16H_SizeAfterPreScaling = u32TempWidth;
        }

        if (pSrcInfo->u16H_SizeAfterPreScaling > SUB_MST_LINE_BFF_MAX)
        {
            pSrcInfo->u16H_SizeAfterPreScaling = SUB_MST_LINE_BFF_MAX;
        }

        pSrcInfo->u16H_SizeAfterPreScaling &= ~(OFFSET_PIXEL_ALIGNMENT - 1);
    }
    else if(IS_INPUT_FRAME_PACKING(eWindow)
            && IS_OUTPUT_LINE_ALTERNATIVE())
    {
        MS_U32 u32TempWidth = MApi_XC_GetAvailableSize(MAIN_WINDOW,
                                                       2,
                                                       pSrcInfo->u16V_SizeAfterPreScaling);
        if(pSrcInfo->u16H_SizeAfterPreScaling > u32TempWidth)
        {
            pSrcInfo->u16H_SizeAfterPreScaling = u32TempWidth & ~(OFFSET_PIXEL_ALIGNMENT -1);
        }
    }

#if ENABLE_2_FRAME_SIZE_PROTECTION
    //do a rough check after all, for 2 frame case.
    //if it's 4 or other frame case, need detail check in the upper code.
    {
        MS_U32 u32TempWidth = MApi_XC_GetAvailableSize(MAIN_WINDOW,
                                                       2,
                                                       pSrcInfo->u16V_SizeAfterPreScaling);
        if(pSrcInfo->u16H_SizeAfterPreScaling > u32TempWidth)
        {
            pSrcInfo->u16H_SizeAfterPreScaling = u32TempWidth & ~(OFFSET_PIXEL_ALIGNMENT -1);
        }
    }
#endif

    if((SUB_WINDOW == eWindow)
       && (IS_OUTPUT_TOP_BOTTOM() || IS_OUTPUT_SIDE_BY_SIDE_HALF())
       )
    {
        pSrcInfo->u16H_SizeAfterPreScaling = gSrcInfo[MAIN_WINDOW].u16H_SizeAfterPreScaling;
    }

    SC_3D_DBG(printf("MDrv_SC_3D_Adjust_PreHorDstSize(%s): to %u\n",eWindow?"Subwindow":"Mainwindow",pSrcInfo->u16H_SizeAfterPreScaling);)
#endif
}

void MDrv_SC_3D_Adjust_SubLineBufferOffset(INPUT_SOURCE_TYPE_t enInputSourceType,
                                           XC_InternalStatus *pSrcInfo,
                                           SCALER_WIN eWindow)
{
    MS_ASSERT(eWindow < MAX_WINDOW);
    MS_ASSERT(pSrcInfo);

#if (HW_DESIGN_3D_VER >= 2)
    return;
#else
    //Setting Line Buffer Start offset for Sub Win
    if((eWindow == MAIN_WINDOW)
        && (IS_OUTPUT_TOP_BOTTOM() || IS_OUTPUT_SIDE_BY_SIDE_HALF())
       )
    {
        // we can extend sub window line buffer by setting this register
		#ifdef MULTI_SCALER_SUPPORTED
		HAL_SC_SetSubLineBufferOffset(MS_3D_LINE_BFF_MAX, eWindow);
		#else
        HAL_SC_SetSubLineBufferOffset(MS_3D_LINE_BFF_MAX);
		#endif
    }
#endif
}

MS_U16 MDrv_SC_3D_Adjust_PreVerDstSize(INPUT_SOURCE_TYPE_t enInputSourceType,
                                       XC_InternalStatus *pSrcInfo,
                                       MS_U16 u16ScaleDst,
                                       SCALER_WIN eWindow)
{
    MS_U16 u16ScaleDstOriginal = u16ScaleDst;
    MS_ASSERT(eWindow < MAX_WINDOW);
    MS_ASSERT(pSrcInfo);

    if(pSrcInfo == NULL)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: pSrcInfo is null pointer!\r\n");
        return u16ScaleDstOriginal;
    }

    if(eWindow >= MAX_WINDOW)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: eWindow is wrong!\r\n");
        return u16ScaleDstOriginal;
    }
#if (HW_DESIGN_3D_VER >= 2)
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"MDrv_SC_3D_Adjust_PreVerDstSize(%s)= %u\n",eWindow?"Subwindow":"Mainwindow",u16ScaleDstOriginal)
    return u16ScaleDstOriginal;
#else
    MS_U16 u16ScaleSrc;
    u16ScaleSrc = pSrcInfo->stCapWin.height;
    u16ScaleDst = pSrcInfo->stDispWin.height;

    SC_3D_DBG(printf("MDrv_SC_3D_Adjust_PreVerSrcSize(%s): from %u\n",eWindow?"Subwindow":"Mainwindow",u16ScaleSrc);)
    SC_3D_DBG(printf("MDrv_SC_3D_Adjust_PreVerDstSize(%s): from %u\n",eWindow?"Subwindow":"Mainwindow",u16ScaleDst);)

    if(((IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) && (!pSrcInfo->bInterlace))
         || IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow)
         || IS_INPUT_NORMAL_2D(eWindow)
         || IS_INPUT_NORMAL_2D_INTERLACE(eWindow)) &&
            IS_OUTPUT_LINE_ALTERNATIVE())
    {
        if(u16ScaleSrc >= u16ScaleDst/2)
        {
             u16ScaleDst = u16ScaleDst/2;
             SC_3D_DBG(printf("3D: pre scaling: source V size is bigger than u16ScaleDst/2(%u)\r\n",u16ScaleDst);)
        }
        else
        {
            u16ScaleDst = u16ScaleSrc;
             SC_3D_DBG(printf("3D: pre scaling: source V size is smaller than u16ScaleDst/2(%u)\r\n",u16ScaleDst);)
        }
    }
    else if((IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE_OPT(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
            || (IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE() && pSrcInfo->bInterlace)
            || (IS_INPUT_TOP_BOTTOM(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE()))
    {
        if (u16ScaleSrc > u16ScaleDst)
        {
            u16ScaleDst = u16ScaleDst;
        }
        else
        {
            u16ScaleDst = u16ScaleSrc;
        }
    }
    else if(IS_INPUT_LINE_ALTERNATIVE(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF())
    {
        u16ScaleDst = u16ScaleSrc;
    }
    else if(IS_INPUT_NORMAL_2D_INTERLACE_PTP(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
    {
        //do noting, just not follow into statement:else
    }
    else if((IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) &&
            (IS_OUTPUT_FRAME_ALTERNATIVE() || IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC()))
            ||
            (IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow) && (IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC()))
            )
    {
        u16ScaleDst = u16ScaleSrc;
    }
    else if(IS_INPUT_LINE_ALTERNATIVE(eWindow) &&
            (IS_OUTPUT_FRAME_ALTERNATIVE() || IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC()))//only for T8 new reg method
    {
        u16ScaleDst = u16ScaleSrc;
    }
    else if(IS_INPUT_FRAME_PACKING(eWindow) &&
            (IS_OUTPUT_FRAME_ALTERNATIVE() || IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC()))
    {
        u16ScaleDst = u16ScaleSrc;
    }
    else if(IS_INPUT_FRAME_PACKING(eWindow)
            && IS_OUTPUT_LINE_ALTERNATIVE())
    {
        if((u16ScaleSrc == DOUBLEHD_720X2P_VSIZE) //720 centering opt
           && (pSrcInfo->stDispWin.height == DOUBLEHD_720X2P_FRAME_VSIZE))
        {
            u16ScaleDst = 735;
        }
        else
        {
            // we can merge 1080p and 720p frame packing, since
            // 1080(one frame size)/45(garbage band) = 720(one frame size)/30(garbage band)
            MS_U16 u16Temp = DOUBLEHD_1080X2P_GARBAGE_VSIZE * u16ScaleDst / 2;
            // if can not mod, need take more 1 lines to do opt
            if(u16Temp % DOUBLEHD_1080X2P_FRAME_VSIZE)
            {
                _bFrmpackToLinealterOpt = TRUE;
                u16Temp = u16Temp / DOUBLEHD_1080X2P_FRAME_VSIZE + 1;
            }
            else
            {
                _bFrmpackToLinealterOpt = FALSE;
                u16Temp = u16Temp / DOUBLEHD_1080X2P_FRAME_VSIZE;
            }
            u16ScaleDst = u16ScaleDst + u16Temp;
        }
        SC_3D_DBG(printf("3D: pre scaling: frame packing, V down to %u\n",u16ScaleDst);)
    }
    else if((IS_INPUT_LINE_ALTERNATIVE(eWindow) &&
       (IS_OUTPUT_TOP_BOTTOM() || IS_OUTPUT_LINE_ALTERNATIVE()))
       ||
       (IS_INPUT_TOP_BOTTOM(eWindow) &&
       IS_OUTPUT_TOP_BOTTOM())
       ||
       (IS_INPUT_TOP_BOTTOM(eWindow) &&
       IS_OUTPUT_SIDE_BY_SIDE_HALF()))
    {
        u16ScaleDst = u16ScaleSrc;
        //printf("3D: pre scaling: E_XC_3D_INPUT_LINE_ALTERNATIVE case, V cannot prescale\n");
    }
    else if (IS_INPUT_LINE_ALTERNATIVE(eWindow) &&
            IS_OUTPUT_FRAME_L())
    {
        u16ScaleDst = u16ScaleSrc;
    }
    else if((IS_INPUT_FRAME_PACKING(eWindow) && IS_OUTPUT_TOP_BOTTOM())
            || (IS_INPUT_FRAME_PACKING(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF()))
    {
        u16ScaleDst = u16ScaleSrc;
    }
    else if(IS_INPUT_FRAME_ALTERNATIVE(eWindow) &&
         IS_OUTPUT_TOP_BOTTOM())
    {
        u16ScaleDst = u16ScaleSrc;
        //SC_3D_DBG(printf("3D: pre scaling: cut source V size to screen's half---%u\n",u16ScaleDst);)
    }
    else if(IS_INPUT_FIELD_ALTERNATIVE(eWindow) &&
            (IS_OUTPUT_TOP_BOTTOM()
             || IS_OUTPUT_SIDE_BY_SIDE_HALF()
             || IS_OUTPUT_LINE_ALTERNATIVE()))
    {
        u16ScaleDst = u16ScaleSrc;
    }
    else if(IS_INPUT_FRAME_ALTERNATIVE(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE() )
    {
        u16ScaleDst = pSrcInfo->stDispWin.height / 2;
        //SC_3D_DBG(printf("3D: pre scaling: cut source V size to screen's half---%u\n",u16ScaleDst);)
    }
    else if (IS_INPUT_FRAME_PACKING(eWindow) &&
            (IS_OUTPUT_FRAME_L()||IS_OUTPUT_FRAME_R()))
    {
        u16ScaleDst = u16ScaleSrc;
    }
    else if(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF())
    {
        u16ScaleDst = u16ScaleSrc;
    }
    else if(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) && IS_OUTPUT_TOP_BOTTOM())
    {
        if(u16ScaleSrc < pSrcInfo->stDispWin.height)
        {
            u16ScaleDst = u16ScaleSrc;
        }
        else
        {
            u16ScaleDst = pSrcInfo->stDispWin.height;
        }
    }
    else if(IS_INPUT_FRAME_PACKING_OPT(eWindow) && IS_OUTPUT_TOP_BOTTOM())
    {
        u16ScaleDst = u16ScaleDst / 2;
    }
    else if((IS_INPUT_FRAME_PACKING_OPT(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF())
             || (IS_INPUT_TOP_BOTTOM_OPT(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF()))
    {
        u16ScaleDst = u16ScaleSrc;
    }
    else if(IS_INPUT_FRAME_ALTERNATIVE(eWindow) &&
            IS_OUTPUT_SIDE_BY_SIDE_HALF())
    {
        u16ScaleDst = u16ScaleSrc;
    }
    else if(IS_INPUT_NORMAL_2D(eWindow) && IS_OUTPUT_TOP_BOTTOM())
    {
        if(u16ScaleSrc < pSrcInfo->stDispWin.height)
        {
            u16ScaleDst = u16ScaleSrc;
        }
        else
        {
            u16ScaleDst = pSrcInfo->stDispWin.height;
        }
    }
    else
    {
        u16ScaleDst = u16ScaleDstOriginal;
    }

    if((SUB_WINDOW == eWindow)
       && MDrv_SC_3D_IsUseSameBuffer()
       && (IS_OUTPUT_TOP_BOTTOM() || IS_OUTPUT_SIDE_BY_SIDE_HALF())
       )
    {
        pSrcInfo->u16V_SizeAfterPreScaling = gSrcInfo[MAIN_WINDOW].u16V_SizeAfterPreScaling;
        u16ScaleDst = gSrcInfo[MAIN_WINDOW].u16V_SizeAfterPreScaling;
    }

    SC_3D_DBG(printf("MDrv_SC_3D_Adjust_PreVerDstSize(%s): to %u\n",eWindow?"Subwindow":"Mainwindow",u16ScaleDst);)
    return u16ScaleDst;
#endif
}

MS_U16 MDrv_SC_3D_Adjust_PostHorSrcSize(XC_InternalStatus *pSrcInfo,
                                        MS_U16 u16ScaleSrc,
                                        SCALER_WIN eWindow)
{
    MS_ASSERT(eWindow < MAX_WINDOW);
    MS_ASSERT(pSrcInfo);

    if(pSrcInfo == NULL)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: pSrcInfo is null pointer!\r\n");
        return u16ScaleSrc;
    }

    if(eWindow >= MAX_WINDOW)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: eWindow is wrong!\r\n");
        return u16ScaleSrc;
    }

#if (HW_DESIGN_3D_VER >= 2)

    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW, "PostHorSrcSize(%s): %u\n",eWindow?"Subwindow":"Mainwindow",pSrcInfo->u16H_SizeAfterPreScaling);
    return u16ScaleSrc;
#else
    SC_3D_DBG(printf("MDrv_SC_3D_Adjust_PostHorSrcSize(%s): from %u\n",eWindow?"Subwindow":"Mainwindow",u16ScaleSrc);)
    if((((IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) && (!pSrcInfo->bInterlace))
       || IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow)) &&
       IS_OUTPUT_LINE_ALTERNATIVE())
       ||
       (IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) &&
       IS_OUTPUT_SIDE_BY_SIDE_HALF() && (pSrcInfo->stCapWin.width != pSrcInfo->stDispWin.width))
       ||
       (IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) &&
       IS_OUTPUT_TOP_BOTTOM())
       ||
       (IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) &&
       (IS_OUTPUT_FRAME_ALTERNATIVE() || IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC()))
       ||
       (IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow) && (IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC()))
        )
    {
        u16ScaleSrc = gSrcInfo[eWindow].ScaledCropWin.width/2;
    }
    else if (IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) &&
        (IS_OUTPUT_FRAME_L()||IS_OUTPUT_FRAME_R()))
    {
        u16ScaleSrc = u16ScaleSrc/2;
    }
#if (HW_DESIGN_3D_VER == 0)
    else if(IS_INPUT_LINE_ALTERNATIVE(eWindow)
            && IS_OUTPUT_SIDE_BY_SIDE_HALF())
    {
        u16ScaleSrc = u16ScaleSrc*2;
    }
    else if(IS_INPUT_FRAME_ALTERNATIVE(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF())
    {
        u16ScaleSrc = gSrcInfo[eWindow].ScaledCropWin.width * 2;
    }
#endif
    else if((IS_INPUT_NORMAL_2D_INTERLACE(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
         ||(IS_INPUT_NORMAL_2D_INTERLACE_PTP(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
         ||(IS_INPUT_NORMAL_2D(eWindow) && IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC())
         ||(IS_INPUT_NORMAL_2D(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE()))
    {
        gSrcInfo[eWindow].ScaledCropWin.width -= _u163DHShift;
        u16ScaleSrc = gSrcInfo[eWindow].ScaledCropWin.width;
    }
    else if((IS_INPUT_FRAME_PACKING_OPT(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF())
             || (IS_INPUT_TOP_BOTTOM_OPT(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF()))
    {
        u16ScaleSrc = gSrcInfo[eWindow].ScaledCropWin.width * 2;
    }
    else if(IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE_OPT(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
    {
        u16ScaleSrc = gSrcInfo[eWindow].ScaledCropWin.width/2;
    }
    else if(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE() && pSrcInfo->bInterlace)
    {
        u16ScaleSrc = gSrcInfo[eWindow].ScaledCropWin.width/2;
    }

    SC_3D_DBG(printf("MDrv_SC_3D_Adjust_PostHorSrcSize(%s): to %u\n",eWindow?"Subwindow":"Mainwindow",u16ScaleSrc);)

    return u16ScaleSrc;
#endif
}

MS_U16 MDrv_SC_3D_Adjust_PostHorDstSize(XC_InternalStatus *pSrcInfo,
                                        MS_U16 u16ScaleDst,
                                        SCALER_WIN eWindow)
{
    MS_ASSERT(eWindow < MAX_WINDOW);
    MS_ASSERT(pSrcInfo);

    if(pSrcInfo == NULL)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: pSrcInfo is null pointer!\r\n");
        return u16ScaleDst;
    }

    if(eWindow >= MAX_WINDOW)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: eWindow is wrong!\r\n");
        return u16ScaleDst;
    }

#if (HW_DESIGN_3D_VER >= 2)
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW, "PostHorDstSize(%s): %u\n",eWindow?"Subwindow":"Mainwindow",pSrcInfo->u16H_SizeAfterPreScaling);
    return u16ScaleDst;
#else

    SC_3D_DBG(printf("MDrv_SC_3D_Adjust_PostHorDstSize(%s): from %u\n",eWindow?"Subwindow":"Mainwindow",u16ScaleDst);)
    if(((IS_INPUT_SIDE_BY_SIDE_HALF(eWindow)
       || IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow)) &&
       IS_OUTPUT_LINE_ALTERNATIVE())
       ||
       (IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) &&
       IS_OUTPUT_SIDE_BY_SIDE_HALF() && (pSrcInfo->stCapWin.width != pSrcInfo->stDispWin.width))
       ||
       (IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) &&
       IS_OUTPUT_TOP_BOTTOM())
       ||
       (IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) &&
       (IS_OUTPUT_FRAME_ALTERNATIVE() || IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC()))
       ||
       (IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow) && (IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC()))
       ||
       (IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE_OPT(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
        )
    {
        if (u16ScaleDst > g_XC_InitData.stPanelInfo.u16Width)
        {
            u16ScaleDst = g_XC_InitData.stPanelInfo.u16Width;
            //SC_3D_DBG(printf("3D: post scaling: adjust the very big horizontal u16ScaleDst to---%u\n",u16ScaleDst);)
        }
    }
    SC_3D_DBG(printf("MDrv_SC_3D_Adjust_PostHorDstSize(%s): to %u\n",eWindow?"Subwindow":"Mainwindow",u16ScaleDst);)

    return u16ScaleDst;
#endif
}

MS_U16 MDrv_SC_3D_Adjust_PostVerSrcSize(XC_InternalStatus *pSrcInfo,
                                        MS_U16 u16ScaleSrc,
                                        SCALER_WIN eWindow)
{
    MS_ASSERT(eWindow < MAX_WINDOW);
    MS_ASSERT(pSrcInfo);

    if(pSrcInfo == NULL)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: pSrcInfo is null pointer!\r\n");
        return u16ScaleSrc;
    }

    if(eWindow >= MAX_WINDOW)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: eWindow is wrong!\r\n");
        return u16ScaleSrc;
    }

#if (HW_DESIGN_3D_VER >= 2)
    return u16ScaleSrc;
#else
    SC_3D_DBG(printf("MDrv_SC_3D_Adjust_PostVerSrcSize(%s): from %u\n",eWindow?"Subwindow":"Mainwindow",u16ScaleSrc);)
    if(((IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) && (!pSrcInfo->bInterlace))
       || IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow)
       || IS_INPUT_NORMAL_2D_INTERLACE(eWindow)
       || IS_INPUT_NORMAL_2D(eWindow)) &&
       IS_OUTPUT_LINE_ALTERNATIVE())
    {
        u16ScaleSrc = gSrcInfo[eWindow].ScaledCropWin.height * 2;
        //SC_3D_DBG(printf("3D: post scaling: Vertical u16ScaleSrc---%u\n",u16ScaleSrc);)
    }
    else if(IS_INPUT_FRAME_PACKING(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
    {
        u16ScaleSrc = pSrcInfo->stDispWin.height - gSrcInfo[eWindow].ScaledCropWin.y*2;
        //SC_3D_DBG(printf("3D: post scaling: Vertical u16ScaleSrc(1280X1470 and 1920X2205 case)---%u\n", u16ScaleSrc);)
    }
    else if((IS_INPUT_LINE_ALTERNATIVE(eWindow) &&
               IS_OUTPUT_TOP_BOTTOM())
               ||
               (IS_INPUT_TOP_BOTTOM(eWindow) &&
               IS_OUTPUT_TOP_BOTTOM())
               ||
               (IS_INPUT_TOP_BOTTOM(eWindow) &&
               IS_OUTPUT_SIDE_BY_SIDE_HALF()))
    {
        u16ScaleSrc = gSrcInfo[eWindow].ScaledCropWin.height/2;//E_XC_3D_INPUT_LINE_ALTERNATIVE cannot do v-scaling.
        //SC_3D_DBG(printf("3D: post scaling: Vertical u16ScaleSrc(E_XC_3D_INPUT_LINE_ALTERNATIVE)---%u\n", u16ScaleSrc);)
    }
    else if(IS_INPUT_LINE_ALTERNATIVE(eWindow) &&
               IS_OUTPUT_FRAME_L())
    {
        u16ScaleSrc = gSrcInfo[eWindow].ScaledCropWin.height/2;//E_XC_3D_INPUT_LINE_ALTERNATIVE cannot do v-scaling.
    }
    else if((IS_INPUT_FRAME_PACKING(eWindow) && IS_OUTPUT_TOP_BOTTOM())
            || (IS_INPUT_FRAME_PACKING(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF()))
    {
        if(gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_720X2P_VSIZE)
        {
            u16ScaleSrc = DOUBLEHD_720X2P_FRAME_VSIZE - gSrcInfo[eWindow].ScaledCropWin.y*2;
        }
        else if(gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_1080X2P_VSIZE)
        {
            u16ScaleSrc = DOUBLEHD_1080X2P_FRAME_VSIZE - gSrcInfo[eWindow].ScaledCropWin.y*2;
        }
        else if(gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_1080X2I_VSIZE)
        {
            u16ScaleSrc = DOUBLEHD_1080X2I_FRAME_VSIZE - gSrcInfo[eWindow].ScaledCropWin.y*2;
        }
    }
    else if(IS_INPUT_FRAME_PACKING(eWindow) &&
           (IS_OUTPUT_FRAME_ALTERNATIVE() || IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC()))
    {
        if(gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_720X2P_VSIZE)
        {
            u16ScaleSrc = DOUBLEHD_720X2P_FRAME_VSIZE - gSrcInfo[eWindow].ScaledCropWin.y*2;
        }
        else if(gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_1080X2P_VSIZE)
        {
            u16ScaleSrc = DOUBLEHD_1080X2P_FRAME_VSIZE - gSrcInfo[eWindow].ScaledCropWin.y*2;
        }

        //SC_3D_DBG(printf("3D: post scaling: Vertical u16ScaleSrc(E_XC_3D_INPUT_LINE_ALTERNATIVE)---%u\n", u16ScaleSrc);)
    }
    else if(IS_INPUT_LINE_ALTERNATIVE(eWindow) &&
            IS_OUTPUT_FRAME_ALTERNATIVE())
    {
        u16ScaleSrc = u16ScaleSrc/2;
    }
    else if(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) &&
               IS_OUTPUT_SIDE_BY_SIDE_HALF() && (pSrcInfo->stCapWin.width != pSrcInfo->stDispWin.width))
    {
        u16ScaleSrc = gSrcInfo[eWindow].ScaledCropWin.height;//E_XC_3D_INPUT_LINE_ALTERNATIVE cannot do v-scaling.
        //SC_3D_DBG(printf("3D: post scaling: Vertical u16ScaleSrc(E_XC_3D_INPUT_SIDE_BY_SIDE_HALF)---%u\n", u16ScaleSrc);)
    }
    else if(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) &&
               IS_OUTPUT_TOP_BOTTOM())
    {
        u16ScaleSrc = gSrcInfo[eWindow].ScaledCropWin.height;//E_XC_3D_INPUT_LINE_ALTERNATIVE cannot do v-scaling.
        //SC_3D_DBG(printf("3D: post scaling: Vertical u16ScaleSrc(E_XC_3D_INPUT_SIDE_BY_SIDE_HALF)---%u\n", u16ScaleSrc);)
    }
    else if(IS_INPUT_FRAME_ALTERNATIVE(eWindow) &&
                  IS_OUTPUT_TOP_BOTTOM())
    {
        u16ScaleSrc = pSrcInfo->ScaledCropWin.height;
        //SC_3D_DBG(printf("3D: post scaling: E_XC_3D_INPUT_FRAME_ALTERNATIVE V_src---%u\n", u16ScaleSrc);)
    }
    else if(IS_INPUT_FIELD_ALTERNATIVE(eWindow) &&
             (IS_OUTPUT_TOP_BOTTOM() || IS_OUTPUT_SIDE_BY_SIDE_HALF()))
    {
        u16ScaleSrc = gSrcInfo[eWindow].ScaledCropWin.height / 2;
    }
    else if(IS_INPUT_FRAME_ALTERNATIVE(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
    {
        u16ScaleSrc = pSrcInfo->stDispWin.height - gSrcInfo[eWindow].ScaledCropWin.y*2;
        //SC_3D_DBG(printf("3D: post scaling: E_XC_3D_INPUT_FRAME_ALTERNATIVE V_src---%u\n", u16ScaleSrc);)
    }
    else if(IS_INPUT_FRAME_PACKING(eWindow) &&
             (IS_OUTPUT_FRAME_L()||IS_OUTPUT_FRAME_R()))
    {
        if(gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_720X2P_VSIZE)
        {
            u16ScaleSrc = DOUBLEHD_720X2P_FRAME_VSIZE - gSrcInfo[eWindow].ScaledCropWin.y*2;
        }
        else if(gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_1080X2P_VSIZE)
        {
            u16ScaleSrc = DOUBLEHD_1080X2P_FRAME_VSIZE - gSrcInfo[eWindow].ScaledCropWin.y*2;
        }
        else if(gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_1080X2I_VSIZE)
        {
            u16ScaleSrc = DOUBLEHD_1080X2I_FRAME_VSIZE - gSrcInfo[eWindow].ScaledCropWin.y*2;
        }
    }
    else if (IS_INPUT_TOP_BOTTOM(eWindow) &&
        (IS_OUTPUT_FRAME_L()||IS_OUTPUT_FRAME_R()))
    {
        u16ScaleSrc = u16ScaleSrc/2;
    }
    else if((IS_INPUT_LINE_ALTERNATIVE(eWindow) || IS_INPUT_TOP_BOTTOM(eWindow))
            && IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC())
    {
        u16ScaleSrc = u16ScaleSrc/2;
    }
    else if(IS_INPUT_LINE_ALTERNATIVE(eWindow)
            && IS_OUTPUT_SIDE_BY_SIDE_HALF())
    {
        u16ScaleSrc = u16ScaleSrc/2;
    }
    else if(IS_INPUT_FRAME_PACKING_OPT(eWindow) && IS_OUTPUT_TOP_BOTTOM())
    {
        u16ScaleSrc = pSrcInfo->stDispWin.height;
    }
    SC_3D_DBG(printf("MDrv_SC_3D_Adjust_PostVerSrcSize(%s): to %u\n",eWindow?"Subwindow":"Mainwindow",u16ScaleSrc);)

    return u16ScaleSrc;
#endif
}

void MDrv_SC_3D_Adjust_PostVLength(XC_InternalStatus *pSrcInfo, SCALER_WIN eWindow)
{
    MS_ASSERT(eWindow < MAX_WINDOW);
    MS_ASSERT(pSrcInfo);

    if(pSrcInfo == NULL)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: pSrcInfo is null pointer!\r\n");
        return;
    }

    if(eWindow >= MAX_WINDOW)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: eWindow is wrong!\r\n");
        return;
    }

#if (HW_DESIGN_3D_VER >= 2)
    return;
#else
    if(IS_INPUT_FRAME_PACKING(eWindow)
       && IS_OUTPUT_LINE_ALTERNATIVE()
       && _bFrmpackToLinealterOpt)
    {
        pSrcInfo->u16V_Length -= 2; //to handle odd garbage line scaling
    }
#endif
}

XC_FRAME_STORE_NUMBER MDrv_SC_3D_Adjust_FBNum(XC_FRAME_STORE_NUMBER u8FBNumType,
                                       SCALER_WIN eWindow)
{
    MS_ASSERT(eWindow < MAX_WINDOW);

    if(eWindow >= MAX_WINDOW)
    {
        return u8FBNumType;
    }

#if (HW_DESIGN_3D_VER == 2)
    if((IS_INPUT_FRAME_ALTERNATIVE(eWindow) && (!gSrcInfo[eWindow].bInterlace))||
    (IS_INPUT_FRAME_PACKING(eWindow) && (gSrcInfo[eWindow].stCapWin.height == 2228)))
    {
        u8FBNumType = IMAGE_STORE_4_FRAMES;
    }
    return u8FBNumType;
#elif (HW_DESIGN_3D_VER > 2)
    if(IS_INPUT_FRAME_PACKING(eWindow) && (gSrcInfo[eWindow].stCapWin.height == 2228))
    {
        u8FBNumType = IMAGE_STORE_4_FRAMES;
    }
#ifdef TBP_1920X2160_OUTPUT
    else if((!g_XC_Pnl_Misc.FRCInfo.bFRC)&&
            ((gSrcInfo[MAIN_WINDOW].stCapWin.height == DOUBLEHD_1080X2P_VSIZE)||
             (gSrcInfo[MAIN_WINDOW].stCapWin.height == DOUBLEHD_1080X2I_VSIZE))&&
            (MApi_XC_Get_3D_Input_Mode(MAIN_WINDOW) == E_XC_3D_INPUT_FRAME_PACKING)&&
            (MApi_XC_Get_3D_Output_Mode() == E_XC_3D_OUTPUT_TOP_BOTTOM))
    {
        u8FBNumType = IMAGE_STORE_4_FRAMES;
    }
#endif
    return u8FBNumType;
#else
    SC_3D_DBG(printf("_Mdrv_SC_3D_Adjust_FBNum(%s): from %u\n",eWindow?"Subwindow":"Mainwindow",u8FBNumType);)
    if(
#if (HW_DESIGN_3D_VER == 0)
        ((IS_INPUT_TOP_BOTTOM(eWindow) ||
        (IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) && (!gSrcInfo[eWindow].bInterlace)) ||
        IS_INPUT_FRAME_PACKING(eWindow) ) &&
          IS_OUTPUT_LINE_ALTERNATIVE())
        ||
       (IS_INPUT_LINE_ALTERNATIVE(eWindow) &&
        (IS_OUTPUT_TOP_BOTTOM() || IS_OUTPUT_LINE_ALTERNATIVE()))
        ||
       (IS_INPUT_LINE_ALTERNATIVE(eWindow) &&
        IS_OUTPUT_FRAME_L())
        ||
       (IS_INPUT_FRAME_PACKING(eWindow) &&
        IS_OUTPUT_TOP_BOTTOM())
        ||
       (IS_INPUT_FRAME_PACKING(eWindow) &&
        IS_OUTPUT_SIDE_BY_SIDE_HALF())
        ||
       (IS_INPUT_FRAME_PACKING(eWindow) &&
        IS_OUTPUT_FRAME_ALTERNATIVE())
        ||
       (IS_INPUT_FRAME_PACKING(eWindow) &&
        IS_OUTPUT_FRAME_L())
        ||
       (IS_INPUT_FRAME_PACKING(eWindow) &&
        IS_OUTPUT_FRAME_R())
        ||
       (IS_INPUT_TOP_BOTTOM(eWindow) &&
        IS_OUTPUT_TOP_BOTTOM())
        ||
       (IS_INPUT_TOP_BOTTOM(eWindow) &&
        IS_OUTPUT_SIDE_BY_SIDE_HALF())
        ||
       (IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) &&
        IS_OUTPUT_FRAME_ALTERNATIVE())
        ||
       (IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) &&
       IS_OUTPUT_SIDE_BY_SIDE_HALF())
        ||
       (IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) &&
       IS_OUTPUT_TOP_BOTTOM())
       ||
       (IS_INPUT_FRAME_PACKING_OPT(eWindow) &&
       IS_OUTPUT_TOP_BOTTOM())
       ||
       (IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow) &&
       IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC())// 2 frames = 4 fields
       ||
       (IS_INPUT_NORMAL_2D_INTERLACE(eWindow) &&
       IS_OUTPUT_LINE_ALTERNATIVE())// 2 frames = 4 fields
       ||
       (IS_INPUT_NORMAL_2D_INTERLACE_PTP(eWindow) &&
       IS_OUTPUT_LINE_ALTERNATIVE())// 2 frames = 4 fields
       ||
       (IS_INPUT_FRAME_PACKING_OPT(eWindow) &&
       IS_OUTPUT_SIDE_BY_SIDE_HALF())
       ||
       (IS_INPUT_TOP_BOTTOM_OPT(eWindow) &&
       IS_OUTPUT_SIDE_BY_SIDE_HALF())
       ||
       (IS_INPUT_NORMAL_2D(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
       ||
#endif
       (IS_INPUT_FRAME_PACKING(eWindow)
        && IS_OUTPUT_TOP_BOTTOM()
        && (gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_1080X2I_VSIZE))
        ||
        IS_OUTPUT_FRAME_ALTERNATIVE()
       )
    {
        u8FBNumType = IMAGE_STORE_2_FRAMES;
    }
    else if((IS_INPUT_FRAME_ALTERNATIVE(eWindow) && (IS_OUTPUT_TOP_BOTTOM()|| IS_OUTPUT_LINE_ALTERNATIVE()))
            || (IS_INPUT_LINE_ALTERNATIVE(eWindow) && IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC())
            || (IS_INPUT_TOP_BOTTOM(eWindow) && IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC())
            || (IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) && IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC())
            || (IS_INPUT_FRAME_PACKING(eWindow) && IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC())
            || (IS_INPUT_FRAME_ALTERNATIVE(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF())
            || (IS_INPUT_FRAME_ALTERNATIVE(eWindow) && IS_OUTPUT_FRAME_L())
            || (IS_INPUT_FRAME_ALTERNATIVE(eWindow) && IS_OUTPUT_FRAME_R())
            || (IS_INPUT_NORMAL_2D(eWindow) && IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC()))
    {
        u8FBNumType = IMAGE_STORE_4_FRAMES;
    }

    if((!IS_INPUT_MODE_NONE(eWindow)) && (SUB_WINDOW == eWindow))
    {
        u8FBNumType = gSrcInfo[MAIN_WINDOW].Status2.eFrameStoreNumber;
        gSrcInfo[eWindow].bLinearMode = gSrcInfo[MAIN_WINDOW].bLinearMode;
    }

    SC_3D_DBG(printf("_Mdrv_SC_3D_Adjust_FBNum(%s): to %u\n",eWindow?"Subwindow":"Mainwindow",u8FBNumType);)

    return u8FBNumType;
#endif
}

void MDrv_SC_3D_Adjust_DNRBase(MS_U32 *pu32DNRBase0,
                               MS_U32 *pu32DNRBase1,
                               SCALER_WIN eWindow)
{
    MS_ASSERT(eWindow < MAX_WINDOW);
    MS_ASSERT(pu32DNRBase0);
    MS_ASSERT(pu32DNRBase1);

    if(eWindow >= MAX_WINDOW)
    {
        return;
    }

    if(pu32DNRBase0 == NULL)
    {
        SC_3D_DBG(printf("3D:pu32DNRBase0 is NULL\n"));
        return;
    }

    if(pu32DNRBase1 == NULL)
    {
        SC_3D_DBG(printf("3D:pu32DNRBase1 is NULL\n"));
        return;
    }

#if (HW_DESIGN_3D_VER >= 2)
    return;
#else
    SC_3D_DBG(printf("MDrv_SC_3D_Adjust_DNRBase(%s): from pu32DNRBase0:0x%lx, pu32DNRBase1:0x%lx\n",eWindow?"Subwindow":"Mainwindow",*pu32DNRBase0,*pu32DNRBase1);)
    MS_U16  u16DNROffset = (gSrcInfo[eWindow].u16IPMFetch + OFFSET_PIXEL_ALIGNMENT - 1)
                                                          & ~(OFFSET_PIXEL_ALIGNMENT-1);
    MS_U8   u8BytesPer2Pixel = gSrcInfo[eWindow].u8BitPerPixel * 2 / 8;
    if(IS_INPUT_FRAME_ALTERNATIVE(eWindow) && IS_OUTPUT_TOP_BOTTOM())
    {
        *pu32DNRBase1 = MS_IPM_BASE0(eWindow) + (((gSrcInfo[eWindow].ScaledCropWin.height)*
                        (MS_U32)u16DNROffset * ((MS_U32)u8BytesPer2Pixel)/(BYTE_PER_WORD)+ 0x01) & ~0x01L);
        //SC_3D_DBG(printf("3D: adjust pu32DNRBase1(%lu) for E_XC_3D_INPUT_FRAME_ALTERNATIVE\n", *pu32DNRBase1);)
    }
    else if((IS_INPUT_FRAME_ALTERNATIVE(eWindow)
             || IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow)
             || IS_INPUT_NORMAL_2D_INTERLACE(eWindow))
            && IS_OUTPUT_LINE_ALTERNATIVE()
                 )
    {
        if( g_XC_InitData.bMirror[eWindow] )
        {
            *pu32DNRBase1 = MS_IPM_BASE0(eWindow) - (((1)*(MS_U32)u16DNROffset *
                            ((MS_U32)u8BytesPer2Pixel)/(BYTE_PER_WORD)+ 0x01) & ~0x01L);
        }
        else
        {
            *pu32DNRBase1 = MS_IPM_BASE0(eWindow) + (((1)*(MS_U32)(u16DNROffset)*
                            ((MS_U32)u8BytesPer2Pixel)/(BYTE_PER_WORD)+ 0x01) & ~0x01L);
        }
        //SC_3D_DBG(printf("3D: adjust pu32DNRBase1(%lu) for E_XC_3D_INPUT_FRAME_ALTERNATIVE\n", *pu32DNRBase1);)
    }
    else if(IS_INPUT_TOP_BOTTOM(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE() && gSrcInfo[eWindow].bInterlace)
    {
        if( g_XC_InitData.bMirror[eWindow] )
        {
            *pu32DNRBase1 = MS_IPM_BASE0(eWindow) - (((1)*(MS_U32)u16DNROffset *
                            ((MS_U32)u8BytesPer2Pixel)/(BYTE_PER_WORD)+ 0x01) & ~0x01L);
        }
        else
        {
            *pu32DNRBase1 = MS_IPM_BASE0(eWindow) + (((1)*(MS_U32)u16DNROffset *
                            ((MS_U32)u8BytesPer2Pixel)/(BYTE_PER_WORD)+ 0x01) & ~0x01L);
        }
    }
    else if(IS_INPUT_FIELD_ALTERNATIVE(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
    {
        if( g_XC_InitData.bMirror[eWindow] )
        {
            *pu32DNRBase1 = MS_IPM_BASE0(eWindow) - (((1)*(MS_U32)u16DNROffset *
                            ((MS_U32)u8BytesPer2Pixel)/(BYTE_PER_WORD)+ 0x01) & ~0x01L);
        }
        else
        {
            *pu32DNRBase1 = MS_IPM_BASE0(eWindow) + (((1)*(MS_U32)u16DNROffset *
                            ((MS_U32)u8BytesPer2Pixel)/(BYTE_PER_WORD)+ 0x01) & ~0x01L);
        }
    }
    else if(IS_INPUT_FRAME_PACKING_OPT(eWindow) && IS_OUTPUT_TOP_BOTTOM() && (eWindow == SUB_WINDOW))
    {
		#ifdef MULTI_SCALER_SUPPORTED
		 Hal_SC_subwindow_disable(eWindow);
		#else
        Hal_SC_subwindow_disable();
		#endif
        *pu32DNRBase0 = MS_IPM_BASE0(MAIN_WINDOW) + (((gSrcInfo[eWindow].stDispWin.height / 2)
                      * (MS_U32)u16DNROffset * ((MS_U32)u8BytesPer2Pixel)/(BYTE_PER_WORD)+ 0x01) & ~0x01L);
    }
    else if(((IS_INPUT_FRAME_PACKING_OPT(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF())
             || (IS_INPUT_TOP_BOTTOM_OPT(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF()))
             && (SUB_WINDOW == eWindow))
    {
		#ifdef MULTI_SCALER_SUPPORTED
		Hal_SC_subwindow_disable(eWindow);
		#else
        Hal_SC_subwindow_disable();
        #endif
		*pu32DNRBase0 = MS_IPM_BASE0(MAIN_WINDOW) + (((1)*(MS_U32)u16DNROffset * ((MS_U32)u8BytesPer2Pixel)/(BYTE_PER_WORD)+ 0x01) & ~0x01L);
    }
#if (HW_DESIGN_3D_VER == 0)
    else if(IS_INPUT_FRAME_ALTERNATIVE(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF())
    {
        *pu32DNRBase1 = MS_IPM_BASE0(eWindow) + (((1)*(MS_U32)u16DNROffset * ((MS_U32)u8BytesPer2Pixel)/(BYTE_PER_WORD)+ 0x01) & ~0x01L);
    }
#endif
    SC_3D_DBG(printf("MDrv_SC_3D_Adjust_DNRBase(%s): to pu32DNRBase0:0x%lx, pu32DNRBase1:0x%lx\n",eWindow?"Subwindow":"Mainwindow",*pu32DNRBase0,*pu32DNRBase1);)
#endif
}

void MDrv_SC_3D_Adjust_OPMBase(MS_U32 *pu32OPMBase0,
                               MS_U32 *pu32OPMBase1,
                               MS_U32 *pu32OPMBase2,
                               MS_U32 u32Offset,
                               SCALER_WIN eWindow)
{
    MS_ASSERT(eWindow < MAX_WINDOW);

    if(eWindow == MAX_WINDOW)
    {
        return;
    }

    if(pu32OPMBase0 == NULL)
    {
        SC_3D_DBG(printf("pu32OPMBase0 is NULL\n"));
        return;
    }

    if(pu32OPMBase1 == NULL)
    {
        SC_3D_DBG(printf("pu32OPMBase1 is NULL\n"));
        return;
    }

    if(pu32OPMBase2 == NULL)
    {
        SC_3D_DBG(printf("pu32OPMBase2 is NULL\n"));
        return;
    }

#if (HW_DESIGN_3D_VER >= 2)
    return;
#else
    MS_U32  u32DNRBase0 = MS_IPM_BASE0(eWindow);
    MS_U32  u32DNRBase1 = u32DNRBase0 + ((u32Offset / BYTE_PER_WORD + 0x01) & ~0x01L);
    MS_U32  u32DNRBase0Main = MS_IPM_BASE0(MAIN_WINDOW);
    MS_U32  u32DNRBase1Main = 0;
    MS_U32  u32DNRBase2Main = 0;
    MS_U8   u8BytesPer2Pixel = gSrcInfo[eWindow].u8BitPerPixel * 2 / 8;
    MS_BOOL bInterlace = gSrcInfo[eWindow].bInterlace;
    MS_U16  u16DNROffset = (gSrcInfo[eWindow].u16IPMFetch + OFFSET_PIXEL_ALIGNMENT - 1)
                                                          & ~(OFFSET_PIXEL_ALIGNMENT-1);
    MS_U32  u32Framesize = (MS_U32)gSrcInfo[eWindow].u16V_SizeAfterPreScaling *
                                  (MS_U32)u16DNROffset * (MS_U32)u8BytesPer2Pixel/2;
    MS_U8   u8FrameNum = MDrv_SC_Get_FrameStoreNum_Factor(gSrcInfo[eWindow].Status2.eFrameStoreNumber, gSrcInfo[eWindow].bLinearMode);

    MS_ASSERT(eWindow < MAX_WINDOW);
    MS_ASSERT(pu32OPMBase0);
    MS_ASSERT(pu32OPMBase1);

    SC_3D_DBG(printf("MDrv_SC_3D_Adjust_OPMBase(%s): from pu32OPMBase0:0x%lx, pu32OPMBase1:0x%lx, pu32OPMBase2:0x%lx\n",eWindow?"Subwindow":"Mainwindow",*pu32OPMBase0,*pu32OPMBase1,*pu32OPMBase2);)

    if( gSrcInfo[eWindow].Status2.eFrameStoreNumber == IMAGE_STORE_3_FRAMES )
    {
        u32DNRBase1Main = u32DNRBase0Main + ((MDrv_XC_GetDNRBufSize(MAIN_WINDOW) / 3 / BYTE_PER_WORD + 0x01) & ~0x01L);
        u32DNRBase2Main = u32DNRBase1Main + ((MDrv_XC_GetDNRBufSize(MAIN_WINDOW) / 3 / BYTE_PER_WORD + 0x01) & ~0x01L);
    }
    else
    {
        u32DNRBase1Main = u32DNRBase0Main + ((MDrv_XC_GetDNRBufSize(MAIN_WINDOW) / 2 / BYTE_PER_WORD + 0x01) & ~0x01L);
        u32DNRBase2Main = u32DNRBase0Main;
    }

    MDrv_SC_3D_Adjust_DNRBase(&u32DNRBase0,
                              &u32DNRBase1,
                              eWindow);

    //default setting: sub window use mainwindow's scaler buffer
    if((!IS_INPUT_MODE_NONE(eWindow)) && (SUB_WINDOW == eWindow))
    {
        *pu32OPMBase0 = (u32DNRBase0Main + 0x01) & ~0x01L;
        *pu32OPMBase1 = (u32DNRBase1Main + 0x01) & ~0x01L;
        *pu32OPMBase2 = (u32DNRBase2Main + 0x01) & ~0x01L;
    }

    if((IS_INPUT_TOP_BOTTOM(eWindow) ||
        (IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) && (!gSrcInfo[eWindow].bInterlace)) ||
        IS_INPUT_FRAME_ALTERNATIVE(eWindow)||
        IS_INPUT_FRAME_PACKING(eWindow)||
        IS_INPUT_NORMAL_2D(eWindow)) &&
       IS_OUTPUT_LINE_ALTERNATIVE())
    {
        MS_U32 u32DoubleHDMidBandSize = 0;
        if((gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_720X2P_VSIZE)
            || (gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_1080X2P_VSIZE))
        {
            u32DoubleHDMidBandSize = (MS_U32)(gSrcInfo[eWindow].u16V_SizeAfterPreScaling - gSrcInfo[eWindow].stDispWin.height) * (MS_U32)u16DNROffset * (MS_U32)u8BytesPer2Pixel;
            SC_3D_DBG(printf("u32DoubleHDMidBandSize=%u\n", gSrcInfo[eWindow].u16V_SizeAfterPreScaling - gSrcInfo[eWindow].stDispWin.height);)
        }

        if(IS_INPUT_FRAME_ALTERNATIVE(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
        {
            *pu32OPMBase1 = u32DNRBase0;
        }
        else if(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow)
                && (!gSrcInfo[eWindow].bInterlace)
                && IS_OUTPUT_LINE_ALTERNATIVE())
        {
            *pu32OPMBase1 = *pu32OPMBase0 + ((((MS_U32)u16DNROffset * (MS_U32)u8BytesPer2Pixel)/(BYTE_PER_WORD) + 0x01) & ~0x01L);
        }
        else if(IS_INPUT_NORMAL_2D(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
        {
            if( g_XC_InitData.bMirror[eWindow] )
            {
                *pu32OPMBase1 = *pu32OPMBase0 + ((((MS_U32)1 * (MS_U32)(_u163DHShift)  *
                                    (MS_U32)u8BytesPer2Pixel)/(BYTE_PER_WORD) + 0x01) & ~0x01L);
            }
            else
            {
                *pu32OPMBase1 = *pu32OPMBase0;
                *pu32OPMBase0 = *pu32OPMBase1 + ((((MS_U32)1 * (MS_U32)(_u163DHShift)  *
                                    (MS_U32)u8BytesPer2Pixel)/(BYTE_PER_WORD) + 0x01) & ~0x01L);
            }
        }
        else if(IS_INPUT_TOP_BOTTOM(eWindow) &&
                IS_OUTPUT_LINE_ALTERNATIVE())
        {
            *pu32OPMBase1 = *pu32OPMBase0 + (((u32Framesize)/(BYTE_PER_WORD) + 0x01) & ~0x01L);
        }
        else if(IS_INPUT_FRAME_PACKING(eWindow) &&
                IS_OUTPUT_LINE_ALTERNATIVE())
        {
            MS_U32 u32SecondFrameOffset = 0;
            if(gSrcInfo[eWindow].stDispWin.height == DOUBLEHD_720X2P_FRAME_VSIZE) //720p centering opt
            {
                u32SecondFrameOffset = (MS_U32)(DOUBLEHD_720X2P_FRAME_VSIZE / 2) * (MS_U32)u16DNROffset * (MS_U32)u8BytesPer2Pixel +
                                             (MS_U32)15 * (MS_U32)u16DNROffset * (MS_U32)u8BytesPer2Pixel;
            }
            else
            {
                u32SecondFrameOffset = (MS_U32)(gSrcInfo[eWindow].stDispWin.height / 2) * (MS_U32)u16DNROffset * (MS_U32)u8BytesPer2Pixel +
                                             u32DoubleHDMidBandSize;
            }
            *pu32OPMBase1 = *pu32OPMBase0 + ((u32SecondFrameOffset/(BYTE_PER_WORD) + 0x01) & ~0x01L);
        }
    }
    else if(IS_INPUT_FRAME_ALTERNATIVE(eWindow) && IS_OUTPUT_FRAME_L())
    {
        *pu32OPMBase1 = *pu32OPMBase0 = u32DNRBase0;
    }
    else if(IS_INPUT_FRAME_ALTERNATIVE(eWindow) && IS_OUTPUT_FRAME_R())
    {
        *pu32OPMBase1 = *pu32OPMBase0 = u32DNRBase1;
    }
    else if(IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow)
           && IS_OUTPUT_LINE_ALTERNATIVE())
    {
        *pu32OPMBase1 = u32DNRBase0 + ((((MS_U32)1 * (MS_U32)u16DNROffset / 2 * (MS_U32)u8BytesPer2Pixel)/(BYTE_PER_WORD) + 0x01) & ~0x01L);
    }
    else if(IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE_OPT(eWindow)
           && IS_OUTPUT_LINE_ALTERNATIVE())
    {
        *pu32OPMBase1 = u32DNRBase0 + ((((MS_U32)1 * (MS_U32)u16DNROffset / 2 *
                            (MS_U32)u8BytesPer2Pixel)/(BYTE_PER_WORD) + 0x01) & ~0x01L);
    }
    else if(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE() && gSrcInfo[eWindow].bInterlace)
    {
        *pu32OPMBase1 = u32DNRBase0 + ((((MS_U32)1 * (MS_U32)u16DNROffset / 2 *
                            (MS_U32)u8BytesPer2Pixel)/(BYTE_PER_WORD) + 0x01) & ~0x01L);
    }
    else if(IS_INPUT_NORMAL_2D_INTERLACE(eWindow)
           && IS_OUTPUT_LINE_ALTERNATIVE())
    {
        if( g_XC_InitData.bMirror[eWindow] )
        {
            *pu32OPMBase1 = *pu32OPMBase0 + ((((MS_U32)1 * (MS_U32)(_u163DHShift)  *
                                (MS_U32)u8BytesPer2Pixel)/(BYTE_PER_WORD) + 0x01) & ~0x01L);
        }
        else
        {
            *pu32OPMBase1 = *pu32OPMBase0;
            *pu32OPMBase0 = *pu32OPMBase1 + ((((MS_U32)1 * (MS_U32)(_u163DHShift)  *
                                (MS_U32)u8BytesPer2Pixel)/(BYTE_PER_WORD) + 0x01) & ~0x01L);
        }
    }
    else if((IS_INPUT_NORMAL_2D_INTERLACE_PTP(eWindow))
           && IS_OUTPUT_LINE_ALTERNATIVE())
    {
        if( g_XC_InitData.bMirror[eWindow] )
        {
            *pu32OPMBase1 = *pu32OPMBase0 + ((((MS_U32)1 * (MS_U32)(_u163DHShift)  *
                                (MS_U32)u8BytesPer2Pixel)/(BYTE_PER_WORD) + 0x01) & ~0x01L);
        }
        else
        {
            *pu32OPMBase1 = *pu32OPMBase0;
            *pu32OPMBase0 = *pu32OPMBase1 + ((((MS_U32)1 * (MS_U32)(_u163DHShift)  *
                                (MS_U32)u8BytesPer2Pixel)/(BYTE_PER_WORD) + 0x01) & ~0x01L);
        }
    }
    else if(IS_INPUT_FRAME_ALTERNATIVE(eWindow) &&
        IS_OUTPUT_TOP_BOTTOM() &&
        (eWindow == ((_b3DMainFirst == FALSE)? MAIN_WINDOW:SUB_WINDOW)))
    {
        *pu32OPMBase0 = u32DNRBase0;
        *pu32OPMBase1 = u32DNRBase0;
    }
    else if(IS_INPUT_FRAME_ALTERNATIVE(eWindow) &&
        IS_OUTPUT_TOP_BOTTOM() &&
        (eWindow == ((_b3DMainFirst != FALSE)? MAIN_WINDOW:SUB_WINDOW)))
    {
        *pu32OPMBase0 = u32DNRBase1;
        *pu32OPMBase1 = u32DNRBase1;
    }
    else if(IS_INPUT_FIELD_ALTERNATIVE(eWindow) &&
            (IS_OUTPUT_TOP_BOTTOM() || IS_OUTPUT_SIDE_BY_SIDE_HALF()) &&
            (eWindow == ((_b3DMainFirst == TRUE)? SUB_WINDOW:MAIN_WINDOW)))
    {
        *pu32OPMBase0 = ((u32DNRBase0Main + 0x01) & ~0x01L)
                        + (((gSrcInfo[eWindow].stCapWin.height/4)*
                        (MS_U32)u16DNROffset * ((MS_U32)u8BytesPer2Pixel)/(BYTE_PER_WORD)+ 0x01) & ~0x01L);
        *pu32OPMBase1 = ((u32DNRBase1Main + 0x01) & ~0x01L)
                        + (((gSrcInfo[eWindow].stCapWin.height/4)*(MS_U32)u16DNROffset *
                        ((MS_U32)u8BytesPer2Pixel)/(BYTE_PER_WORD)+ 0x01) & ~0x01L);
    }
    else if(IS_INPUT_FIELD_ALTERNATIVE(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
    {
        *pu32OPMBase1 = u32DNRBase1 + (((gSrcInfo[eWindow].stCapWin.height/2)*(MS_U32)u16DNROffset *
                          ((MS_U32)u8BytesPer2Pixel)/(BYTE_PER_WORD)+ 0x01) & ~0x01L);
    }
    else if(IS_INPUT_LINE_ALTERNATIVE(eWindow) &&
           (IS_OUTPUT_TOP_BOTTOM()
#if (HW_DESIGN_3D_VER == 1)
           || IS_OUTPUT_SIDE_BY_SIDE_HALF()
#endif
           ) &&
           (eWindow == ((_b3DMainFirst == FALSE)? MAIN_WINDOW:SUB_WINDOW)))
    {
        *pu32OPMBase0 = u32DNRBase0Main + 1*(MS_U32)u16DNROffset * u8FrameNum * (MS_U32)u8BytesPer2Pixel/(BYTE_PER_WORD*2);
        *pu32OPMBase1 = u32DNRBase1Main + 1*(MS_U32)u16DNROffset * u8FrameNum * (MS_U32)u8BytesPer2Pixel/(BYTE_PER_WORD*2);
        *pu32OPMBase2 = u32DNRBase2Main + 1*(MS_U32)u16DNROffset * u8FrameNum * (MS_U32)u8BytesPer2Pixel/(BYTE_PER_WORD*2);
    }
    else if(IS_INPUT_LINE_ALTERNATIVE(eWindow)
            && IS_OUTPUT_FRAME_L()
            && gSrcInfo[eWindow].bInterlace)
    {
        *pu32OPMBase1 = (((*pu32OPMBase0 + 1*(MS_U32)u16DNROffset * (MS_U32)u8BytesPer2Pixel/(BYTE_PER_WORD)) + 0x01) & ~0x01L);
    }
    else if(((IS_INPUT_FRAME_PACKING(eWindow) && IS_OUTPUT_TOP_BOTTOM())
           || (IS_INPUT_FRAME_PACKING(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF()))
           && (eWindow == ((_b3DMainFirst == FALSE)? MAIN_WINDOW:SUB_WINDOW)))
    {
        MS_U16 u16SkipLine_OpmBase0 = 0;
        MS_U16 u16SkipLine_OpmBase1 = 0;
        if(gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_720X2P_VSIZE)
        {
            u16SkipLine_OpmBase0 = DOUBLEHD_720X2P_VSIZE - DOUBLEHD_720X2P_FRAME_VSIZE;
        }
        else if(gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_1080X2P_VSIZE)
        {
            u16SkipLine_OpmBase0 = DOUBLEHD_1080X2P_VSIZE - DOUBLEHD_1080X2P_FRAME_VSIZE;
        }
        else if(gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_1080X2I_VSIZE)
        {
            u16SkipLine_OpmBase0 = DOUBLEHD_1080X2I_FIELD_VSIZE + DOUBLEHD_1080X2I_VACT_SPACE1;
            u16SkipLine_OpmBase1 = DOUBLEHD_1080X2I_VSIZE - DOUBLEHD_1080X2I_FIELD_VSIZE;
        }

        SC_3D_DBG(printf("3D----------garbage band----------------u16SkipLine_OpmBase0=%u, u16SkipLine_OpmBase1=%u\n", u16SkipLine_OpmBase0, u16SkipLine_OpmBase1);)
        *pu32OPMBase0 = u32DNRBase0Main + ((u16SkipLine_OpmBase0*u8FrameNum*(MS_U32)u16DNROffset * (MS_U32)u8BytesPer2Pixel/(BYTE_PER_WORD*2) + 0x01) & ~0x01L);
        *pu32OPMBase1 = u32DNRBase1Main + ((u16SkipLine_OpmBase0*u8FrameNum*(MS_U32)u16DNROffset * (MS_U32)u8BytesPer2Pixel/(BYTE_PER_WORD*2) + 0x01) & ~0x01L);
        *pu32OPMBase2 = u32DNRBase2Main + ((u16SkipLine_OpmBase0*u8FrameNum*(MS_U32)u16DNROffset * (MS_U32)u8BytesPer2Pixel/(BYTE_PER_WORD*2) + 0x01) & ~0x01L);
        if(gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_1080X2I_VSIZE)
        {//this format need use madi
            *pu32OPMBase1 = u32DNRBase0Main + ((u16SkipLine_OpmBase1*(MS_U32)u16DNROffset * (MS_U32)u8BytesPer2Pixel/(BYTE_PER_WORD) + 0x01) & ~0x01L);
        }
    }
    else if(((IS_INPUT_FRAME_PACKING(eWindow) && IS_OUTPUT_TOP_BOTTOM())
           || (IS_INPUT_FRAME_PACKING(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF()))
           && (eWindow == ((_b3DMainFirst == FALSE)? SUB_WINDOW:MAIN_WINDOW)))
    {
        if(gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_1080X2I_VSIZE)
        {
            MS_U16 u16SkipLine_OpmBase1 = DOUBLEHD_1080X2I_FIELD_VSIZE*2+DOUBLEHD_1080X2I_VACT_SPACE1+DOUBLEHD_1080X2I_VACT_SPACE2;
            *pu32OPMBase1 = u32DNRBase0Main + ((u16SkipLine_OpmBase1*(MS_U32)u16DNROffset * (MS_U32)u8BytesPer2Pixel/(BYTE_PER_WORD) + 0x01) & ~0x01L);
        }
    }
    else if(IS_INPUT_TOP_BOTTOM(eWindow) &&
           (IS_OUTPUT_TOP_BOTTOM() || IS_OUTPUT_SIDE_BY_SIDE_HALF()) &&
           (eWindow == ((_b3DMainFirst == FALSE)? MAIN_WINDOW:SUB_WINDOW)))
    {
        MS_U16 u16SkipLine = gSrcInfo[eWindow].stCapWin.height / 2;
        SC_3D_DBG(printf("3D----------garbage band----------------u16SkipLine=%u\n", u16SkipLine);)
        *pu32OPMBase0 = u32DNRBase0Main + u16SkipLine/(1 + (TRUE == bInterlace))*u8FrameNum*(MS_U32)u16DNROffset * (MS_U32)u8BytesPer2Pixel/(BYTE_PER_WORD*2);
        *pu32OPMBase1 = u32DNRBase1Main + u16SkipLine/(1 + (TRUE == bInterlace))*u8FrameNum*(MS_U32)u16DNROffset * (MS_U32)u8BytesPer2Pixel/(BYTE_PER_WORD*2);
        *pu32OPMBase2 = u32DNRBase2Main + u16SkipLine/(1 + (TRUE == bInterlace))*u8FrameNum*(MS_U32)u16DNROffset * (MS_U32)u8BytesPer2Pixel/(BYTE_PER_WORD*2);
    }
    else if(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) &&
           IS_OUTPUT_SIDE_BY_SIDE_HALF()&&
           (eWindow == ((_b3DMainFirst == FALSE)? MAIN_WINDOW:SUB_WINDOW)) && (gSrcInfo[eWindow].stCapWin.width != gSrcInfo[eWindow].stDispWin.width))
    {
        *pu32OPMBase0 = u32DNRBase0Main + u8FrameNum*(MS_U32)u16DNROffset/2 * (MS_U32)u8BytesPer2Pixel/(BYTE_PER_WORD*2);
        *pu32OPMBase1 = u32DNRBase1Main + u8FrameNum*(MS_U32)u16DNROffset/2 * (MS_U32)u8BytesPer2Pixel/(BYTE_PER_WORD*2);
        *pu32OPMBase2 = u32DNRBase2Main + u8FrameNum*(MS_U32)u16DNROffset/2 * (MS_U32)u8BytesPer2Pixel/(BYTE_PER_WORD*2);
    }
    else if(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) &&
           IS_OUTPUT_TOP_BOTTOM() &&
           (eWindow == ((_b3DMainFirst == FALSE)? MAIN_WINDOW:SUB_WINDOW)))
    {
        *pu32OPMBase0 = (u32DNRBase0Main + (MS_U32)u16DNROffset/2 * u8FrameNum * (MS_U32)u8BytesPer2Pixel/(BYTE_PER_WORD*2) + 0x01) & ~0x01L;
        *pu32OPMBase1 = (u32DNRBase1Main + (MS_U32)u16DNROffset/2 * u8FrameNum * (MS_U32)u8BytesPer2Pixel/(BYTE_PER_WORD*2) + 0x01) & ~0x01L;
        *pu32OPMBase2 = (u32DNRBase2Main + (MS_U32)u16DNROffset/2 * u8FrameNum * (MS_U32)u8BytesPer2Pixel/(BYTE_PER_WORD*2) + 0x01) & ~0x01L;
    }
#if (HW_DESIGN_3D_VER < 1)
    else if((IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) &&
           (IS_OUTPUT_FRAME_ALTERNATIVE() || IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC()))
           ||(IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow) && (IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC())))

    {
        *pu32OPMBase0 = u32DNRBase0;
        *pu32OPMBase1 = *pu32OPMBase0 + (MS_U32)(u16DNROffset) * (MS_U32)u8BytesPer2Pixel/(BYTE_PER_WORD*2);
    }
    else if(IS_INPUT_NORMAL_2D(eWindow) && IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC())
    {
        if( g_XC_InitData.bMirror[eWindow] )
        {
            *pu32OPMBase1 = *pu32OPMBase1 + ((((MS_U32)(_u163DHShift)  *
                                (MS_U32)u8BytesPer2Pixel)/(BYTE_PER_WORD) + 0x01) & ~0x01L);
        }
        else
        {
            *pu32OPMBase0 = *pu32OPMBase0 + ((((MS_U32)(_u163DHShift)  *
                                (MS_U32)u8BytesPer2Pixel)/(BYTE_PER_WORD) + 0x01) & ~0x01L);
        }
    }
    else if(IS_INPUT_FRAME_PACKING(eWindow) &&
           (IS_OUTPUT_FRAME_ALTERNATIVE() || IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC()))
    {
        MS_U16 u16SkipLine = 0;
        if(gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_720X2P_VSIZE)
        {
            u16SkipLine = DOUBLEHD_720X2P_VSIZE - DOUBLEHD_720X2P_FRAME_VSIZE;
        }
        else if(gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_1080X2P_VSIZE)
        {
            u16SkipLine = DOUBLEHD_1080X2P_VSIZE - DOUBLEHD_1080X2P_FRAME_VSIZE;
        }
        else
        {
            printf("3D-wrong case in to framepacking\n");
        }
        SC_3D_DBG(printf("3D----------garbage band----------------u16SkipLine=%u\n", u16SkipLine);)

        *pu32OPMBase0 = u32DNRBase0;
        *pu32OPMBase1 = *pu32OPMBase0 + (MS_U32)u16DNROffset *  u16SkipLine * (MS_U32)u8BytesPer2Pixel/(BYTE_PER_WORD);
    }
#endif
    else if(IS_INPUT_LINE_ALTERNATIVE(eWindow) &&
            IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC())
    {
        *pu32OPMBase1 = *pu32OPMBase0 + (((MS_U32)u16DNROffset * (MS_U32)u8BytesPer2Pixel/BYTE_PER_WORD + 0x01) & ~0x01L);
    }
    else if(IS_INPUT_TOP_BOTTOM(eWindow) &&
            IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC())
    {
        *pu32OPMBase1 = *pu32OPMBase0 + ((u32Framesize/BYTE_PER_WORD + 0x01) & ~0x01L);
    }
    else if(IS_INPUT_LINE_ALTERNATIVE(eWindow) &&
                IS_OUTPUT_LINE_ALTERNATIVE())
    {
        *pu32OPMBase1 = *pu32OPMBase0 + ((1*(MS_U32)u16DNROffset * (MS_U32)u8BytesPer2Pixel/(BYTE_PER_WORD) + 0x01) & ~0x01L);
    }
    else if(IS_INPUT_FRAME_PACKING(eWindow) && (IS_OUTPUT_FRAME_L()))
    {
        if(gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_1080X2I_VSIZE)
        {
            MS_U16 u16SkipLine_OpmBase1 = DOUBLEHD_1080X2I_FIELD_VSIZE*2+DOUBLEHD_1080X2I_VACT_SPACE1+DOUBLEHD_1080X2I_VACT_SPACE2;
            *pu32OPMBase0 = u32DNRBase0Main;
            *pu32OPMBase1 = u32DNRBase0Main + ((u16SkipLine_OpmBase1*(MS_U32)u16DNROffset * (MS_U32)u8BytesPer2Pixel/(BYTE_PER_WORD) + 0x01) & ~0x01L);
        }
    }
    else if(IS_INPUT_FRAME_PACKING(eWindow) &&
               (IS_OUTPUT_FRAME_R()))
    {
        MS_U16 u16SkipLine = 0;
        if(gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_720X2P_VSIZE)
        {
            u16SkipLine = DOUBLEHD_720X2P_VSIZE - DOUBLEHD_720X2P_FRAME_VSIZE;
        }
        else if(gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_1080X2P_VSIZE)
        {
            u16SkipLine = DOUBLEHD_1080X2P_VSIZE - DOUBLEHD_1080X2P_FRAME_VSIZE;
        }
        SC_3D_DBG(printf("3D----------garbage band----------------u16SkipLine=%u\n", u16SkipLine);)
        *pu32OPMBase0 = u32DNRBase0Main + u16SkipLine*(MS_U32)u16DNROffset * (MS_U32)u8BytesPer2Pixel/(BYTE_PER_WORD);
    }
    else if(IS_INPUT_TOP_BOTTOM(eWindow) &&
               (IS_OUTPUT_FRAME_R()))
    {
        *pu32OPMBase1 = *pu32OPMBase0 + (((u32Framesize)/(BYTE_PER_WORD) + 0x01) & ~0x01L);
        *pu32OPMBase0 = *pu32OPMBase1;
    }
    else if(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) &&
               (IS_OUTPUT_FRAME_R()))
    {
        *pu32OPMBase1 = *pu32OPMBase0 + ((((MS_U32)u16DNROffset * (MS_U32)u8BytesPer2Pixel/2)/(BYTE_PER_WORD) + 0x01) & ~0x01L);
        *pu32OPMBase0 = *pu32OPMBase1;
    }
#if (HW_DESIGN_3D_VER == 0)
    else if(IS_INPUT_FRAME_ALTERNATIVE(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF())
    {
        *pu32OPMBase1 = *pu32OPMBase0 = u32DNRBase0;
    }
#else
    else if(IS_INPUT_FRAME_ALTERNATIVE(eWindow) &&
        IS_OUTPUT_SIDE_BY_SIDE_HALF() &&
        (eWindow == ((_b3DMainFirst == FALSE)? MAIN_WINDOW:SUB_WINDOW)))
    {
        *pu32OPMBase0 = u32DNRBase0;
        *pu32OPMBase1 = u32DNRBase0;
    }
    else if(IS_INPUT_FRAME_ALTERNATIVE(eWindow) &&
        IS_OUTPUT_SIDE_BY_SIDE_HALF() &&
        (eWindow == ((_b3DMainFirst != FALSE)? MAIN_WINDOW:SUB_WINDOW)))
    {
        *pu32OPMBase0 = u32DNRBase1;
        *pu32OPMBase1 = u32DNRBase1;
    }
#endif

    SC_3D_DBG(printf("MDrv_SC_3D_Adjust_OPMBase(%s): to pu32OPMBase0:0x%lx, pu32OPMBase1:0x%lx, pu32OPMBase2:0x%lx\n",eWindow?"Subwindow":"Mainwindow",*pu32OPMBase0,*pu32OPMBase1,*pu32OPMBase2);)
#endif
}



void MDrv_SC_3D_Adjust_FetchOffset(XC_InternalStatus *pSrcInfo,
                                   SCALER_WIN eWindow)
{
    MS_ASSERT(eWindow < MAX_WINDOW);
    MS_ASSERT(pSrcInfo);

    if(eWindow >= MAX_WINDOW)
    {
        return;
    }

    if(pSrcInfo == NULL)
    {
        SC_3D_DBG(printf("3D: pSrcInfo is NULL\n"));
        return;
    }

#if (HW_DESIGN_3D_VER >= 2)

    if(IS_INPUT_SIDE_BY_SIDE_FULL(eWindow)
       && (!pSrcInfo->bInterlace)
       && (IS_OUTPUT_FRAME_ALTERNATIVE()||IS_OUTPUT_TOP_BOTTOM()))
    {

        stDBreg.u16DNROffset = stDBreg.u16H_CapSize;
        stDBreg.u16DNRFetch  = stDBreg.u16H_CapSize;
        stDBreg.u16OPMOffset = stDBreg.u16DNROffset/2;
        stDBreg.u16OPMFetch  = stDBreg.u16DNRFetch/2;

        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW, "3D: adjust u16OPMOffset(%u) and u16OPMFetch(%u) to half for E_XC_3D_INPUT_SIDE_BY_SIDE\n",
                stDBreg.u16OPMOffset,stDBreg.u16OPMFetch);
    }

    return;
#else
    if(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow)
       && (!pSrcInfo->bInterlace)
       && IS_OUTPUT_LINE_ALTERNATIVE())
    {
        //stDBreg.u16OPMOffset /= 2;
        stDBreg.u16OPMFetch  /= 2;
        //SC_3D_DBG(printf("3D: adjust u16OPMOffset(%u) and u16OPMFetch(%u) to half for E_XC_3D_INPUT_SIDE_BY_SIDE_HALF\n",
        //        stDBreg.u16OPMOffset,stDBreg.u16OPMFetch);)
    }
    else if(IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow) &&
       IS_OUTPUT_LINE_ALTERNATIVE())
    {
        stDBreg.u16DNROffset *= 2;
        stDBreg.u16OPMFetch /=2;
        //SC_3D_DBG(printf("3D: adjust u16OPMOffset(%u) and u16OPMFetch(%u) to half for E_XC_3D_INPUT_SIDE_BY_SIDE_HALF\n",
        //        stDBreg.u16OPMOffset,stDBreg.u16OPMFetch);)
    }
    else if(IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE_OPT(eWindow) &&
       IS_OUTPUT_LINE_ALTERNATIVE())
    {
        stDBreg.u16OPMFetch /=2;
    }
    else if(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE() && gSrcInfo[eWindow].bInterlace)
    {
        stDBreg.u16OPMFetch /=2;
    }
    else if(IS_INPUT_NORMAL_2D_INTERLACE(eWindow) &&
       IS_OUTPUT_LINE_ALTERNATIVE())
    {
        stDBreg.u16DNROffset *= 2;
        //stDBreg.u16OPMFetch /=2;
        //SC_3D_DBG(printf("3D: adjust u16OPMOffset(%u) and u16OPMFetch(%u) to half for E_XC_3D_INPUT_SIDE_BY_SIDE_HALF\n",
        //        stDBreg.u16OPMOffset,stDBreg.u16OPMFetch);)
    }
    else if(IS_INPUT_LINE_ALTERNATIVE(eWindow) &&
       (IS_OUTPUT_TOP_BOTTOM() || IS_OUTPUT_LINE_ALTERNATIVE() || IS_OUTPUT_FRAME_L()))
    {
        stDBreg.u16OPMOffset *= 2;
        //SC_3D_DBG(printf("3D: adjust u16OPMOffset(%u) to double and u16OPMFetch(%u) keep original for E_XC_3D_INPUT_LINE_ALTERNATIVE\n",
        //        stDBreg.u16OPMOffset,stDBreg.u16OPMFetch);)
    }
    else if(IS_INPUT_TOP_BOTTOM(eWindow) &&
            (IS_OUTPUT_TOP_BOTTOM() || IS_OUTPUT_SIDE_BY_SIDE_HALF()))
    {
    //change nothing
    }
    else if(IS_INPUT_TOP_BOTTOM(eWindow)
            && IS_OUTPUT_LINE_ALTERNATIVE()
            && pSrcInfo->bInterlace)
    {
        stDBreg.u16DNROffset *= 2;
    }
    else if(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) &&
               IS_OUTPUT_SIDE_BY_SIDE_HALF() && (pSrcInfo->stCapWin.width != pSrcInfo->stDispWin.width))
    {
        stDBreg.u16OPMFetch  /= 2;
        //SC_3D_DBG(printf("3D: adjust u16OPMFetch(%u) to half for E_XC_3D_INPUT_SIDE_BY_SIDE_HALF\n",
        //        stDBreg.u16OPMFetch);)
    }
    else if(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) &&
               IS_OUTPUT_TOP_BOTTOM())
    {
        stDBreg.u16OPMFetch  /= 2;
        //SC_3D_DBG(printf("3D: adjust u16OPMFetch(%u) to half for E_XC_3D_INPUT_SIDE_BY_SIDE_HALF\n",
        //        stDBreg.u16OPMFetch);)
    }
    else if(IS_INPUT_FRAME_ALTERNATIVE(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE() )
    {
        stDBreg.u16DNROffset *= 2;
        //SC_3D_DBG(printf("3D: adjust stDBreg.u16DNROffset(%u) to double for E_XC_3D_INPUT_SIDE_BY_SIDE_HALF\n",
        //        stDBreg.u16DNROffset);)
    }
    else if((IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) &&
                (IS_OUTPUT_FRAME_ALTERNATIVE() || IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC()))
           ||(IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow) && (IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC()))
            )
    {
        stDBreg.u16OPMFetch  /= 2;
    }
    else if(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) &&
               (IS_OUTPUT_FRAME_L()||IS_OUTPUT_FRAME_R()))
    {
        stDBreg.u16OPMFetch  /= 2;
    }
    else if(IS_INPUT_LINE_ALTERNATIVE(eWindow) &&
            IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC())
    {
        stDBreg.u16OPMOffset *= 2;
    }
    else if(IS_INPUT_FIELD_ALTERNATIVE(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
    {
        stDBreg.u16DNROffset *= 2;
    }
    else if(IS_INPUT_LINE_ALTERNATIVE(eWindow)
            && (IS_OUTPUT_SIDE_BY_SIDE_HALF()))
    {
        stDBreg.u16OPMOffset *= 2;
#if (HW_DESIGN_3D_VER == 0)
        stDBreg.u16OPMFetch  *= 2;
#endif
        //SC_3D_DBG(printf("3D: adjust u16OPMOffset(%u) to double and u16OPMFetch(%u) keep original for E_XC_3D_INPUT_LINE_ALTERNATIVE\n",
        //        stDBreg.u16OPMOffset,stDBreg.u16OPMFetch);)
    }
    else if(IS_INPUT_LINE_ALTERNATIVE(eWindow)
            && IS_OUTPUT_FRAME_L()
            && gSrcInfo[eWindow].bInterlace)
    {
        stDBreg.u16OPMOffset *= 2;
    }
#if (HW_DESIGN_3D_VER == 0)
    else if(IS_INPUT_FRAME_ALTERNATIVE(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF())
    {
        stDBreg.u16DNROffset *= 2;
        stDBreg.u16OPMOffset *= 2;
        stDBreg.u16OPMFetch  *= 2;
    }
#endif
    else if((IS_INPUT_FRAME_PACKING_OPT(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF())
             || (IS_INPUT_TOP_BOTTOM_OPT(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF()))
    {
        stDBreg.u16DNROffset *= 2;
        stDBreg.u16OPMOffset *= 2;
        stDBreg.u16OPMFetch  *= 2;
    }

#if (HW_DESIGN_3D_VER < 1)
    //set free_md
    Hal_XC_Set_FreeFRCMD((IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) &&
                IS_OUTPUT_FRAME_ALTERNATIVE()) ||
                (IS_INPUT_FRAME_PACKING(eWindow) &&
                IS_OUTPUT_FRAME_ALTERNATIVE()));
#endif

#if (HW_DESIGN_3D_VER == 1)
    {
        if(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) &&
                   IS_OUTPUT_FRAME_ALTERNATIVE())
        {
            stDBreg.u16DNRFetch /= 2;
            stDBreg.u16OPMOffset = (stDBreg.u16OPMOffset + (OFFSET_PIXEL_ALIGNMENT*2 -1)) & ~(OFFSET_PIXEL_ALIGNMENT*2 -1);
            stDBreg.u16DNROffset = (stDBreg.u16DNROffset + (OFFSET_PIXEL_ALIGNMENT*2 -1)) & ~(OFFSET_PIXEL_ALIGNMENT*2 -1);
            Hal_XC_H3D_Enable(TRUE);
            Hal_XC_H3D_Input3DType(E_XC_3D_INPUT_SIDE_BY_SIDE_FULL);
            Hal_XC_H3D_Breakline_Enable(TRUE);
            Hal_XC_H3D_HDE(pSrcInfo->stCapWin.width);
            if(!_bSkipDefaultLRFlag)
            {
                Hal_XC_H3D_LR_Toggle_Enable(TRUE);
            }
            else
            {
                SC_W2BYTEMSK(REG_SC_BK12_33_L, (TRUE<<1), (BIT(1)));
            }
        }
        else if(IS_INPUT_LINE_ALTERNATIVE(eWindow) &&
                    IS_OUTPUT_FRAME_ALTERNATIVE())
        {
            stDBreg.u32OPMBase1 = stDBreg.u32OPMBase0;
            stDBreg.u16OPMOffset = (stDBreg.u16OPMOffset*2 + (OFFSET_PIXEL_ALIGNMENT*2 -1)) & ~(OFFSET_PIXEL_ALIGNMENT*2 -1);
            stDBreg.u16DNROffset = (stDBreg.u16DNROffset*2 + (OFFSET_PIXEL_ALIGNMENT*2 -1)) & ~(OFFSET_PIXEL_ALIGNMENT*2 -1);
            Hal_XC_H3D_Enable(TRUE);
            Hal_XC_H3D_Input3DType(E_XC_3D_INPUT_LINE_ALTERNATIVE);
            Hal_XC_H3D_Breakline_Enable(FALSE);
            Hal_XC_H3D_HDE(0);
            HAL_XC_H3D_OPM_SBYS_PIP_Enable(FALSE);
            if(!_bSkipDefaultLRFlag)
            {
                Hal_XC_H3D_LR_Toggle_Enable(TRUE);
            }
            else
            {
                SC_W2BYTEMSK(REG_SC_BK12_33_L, (TRUE<<1), (BIT(1)));
            }
        }
        else if(IS_INPUT_FRAME_PACKING(eWindow) &&
                    IS_OUTPUT_FRAME_ALTERNATIVE())
        {
            stDBreg.u16OPMOffset = stDBreg.u16OPMOffset*2;
            stDBreg.u16DNROffset = stDBreg.u16DNROffset*2;
            Hal_XC_H3D_HBLANK(0x33E);
            Hal_XC_H3D_HDE(DOUBLEHD_1080X2P_HSIZE);//0x780
            Hal_XC_H3D_INIT_VBLANK(0x02);
            Hal_XC_H3D_VDE_F0(DOUBLEHD_1080X2P_FRAME_VSIZE);//0x438
            Hal_XC_H3D_VBLANK0(DOUBLEHD_1080X2P_GARBAGE_VSIZE);//0x2D
            Hal_XC_H3D_VBLANK1(DOUBLEHD_1080X2P_GARBAGE_VSIZE);//0x2D
            Hal_XC_H3D_VDE_F2(DOUBLEHD_1080X2P_FRAME_VSIZE);//0x438
            Hal_XC_H3D_VBLANK2(DOUBLEHD_1080X2P_GARBAGE_VSIZE);//0x2D
            Hal_XC_H3D_VSYNC_WIDTH(0x01);
            Hal_XC_H3D_VSYNC_POSITION(0x0400);
            Hal_XC_H3D_Enable(TRUE);
            Hal_XC_H3D_Input3DType(E_XC_3D_INPUT_FRAME_PACKING);
            Hal_XC_H3D_SELECT_REGEN_TIMING(TRUE);
            Hal_XC_H3D_Breakline_Enable(FALSE);
            HAL_XC_H3D_OPM_SBYS_PIP_Enable(TRUE);
            if(!_bSkipDefaultLRFlag)
            {
                Hal_XC_H3D_LR_Toggle_Enable(TRUE);
            }
            else
            {
                SC_W2BYTEMSK(REG_SC_BK12_33_L, (TRUE<<1), (BIT(1)));
            }

            if ((gSrcInfo[eWindow].stCapWin.width == DOUBLEHD_1080X2I_HSIZE) && (gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_1080X2I_VSIZE))
            {
                //stDBreg.u16OPMFetch = stDBreg.u16OPMFetch/2;
                //stDBreg.u16DNRFetch = stDBreg.u16DNRFetch/2;
                if ((pSrcInfo->u16InputVFreq >= 240) && (pSrcInfo->u16InputVFreq <= 260))
                {
                    Hal_XC_H3D_HBLANK(0x2D0);//1080i@50Hz
                }
                else if ((pSrcInfo->u16InputVFreq >= 290) && (pSrcInfo->u16InputVFreq <= 310))
                {
                    Hal_XC_H3D_HBLANK(0x118);//1080i@60Hz
                }
                Hal_XC_H3D_HDE(DOUBLEHD_1080X2I_HSIZE);//0x780
                Hal_XC_H3D_INIT_VBLANK(0x02);
                Hal_XC_H3D_VDE_F0(DOUBLEHD_1080X2I_FRAME_VSIZE/2);//0x21C
                Hal_XC_H3D_VBLANK0(0x17);//0x17
                Hal_XC_H3D_VBLANK1(0x16);//0x16
                Hal_XC_H3D_VDE_F2(DOUBLEHD_1080X2I_FRAME_VSIZE/2);//0x21C
                Hal_XC_H3D_VBLANK2(0x17);//0x17
                Hal_XC_H3D_VSYNC_WIDTH(0x01);
                Hal_XC_H3D_VSYNC_POSITION(0x0400);
                Hal_XC_H3D_Enable(TRUE);
                Hal_XC_H3D_Input3DType(E_XC_3D_INPUT_FRAME_PACKING);
                Hal_XC_H3D_SELECT_REGEN_TIMING(TRUE);
                Hal_XC_H3D_Breakline_Enable(FALSE);
                HAL_XC_H3D_OPM_SBYS_PIP_Enable(TRUE);
                if(!_bSkipDefaultLRFlag)
                {
                    Hal_XC_H3D_LR_Toggle_Enable(TRUE);
                }
                else
                {
                    SC_W2BYTEMSK(REG_SC_BK12_33_L, (TRUE<<1), (BIT(1)));
                }
            }
            else if ((gSrcInfo[eWindow].stCapWin.width == DOUBLEHD_720X2P_HSIZE) && (gSrcInfo[eWindow].stCapWin.height == DOUBLEHD_720X2P_VSIZE))
            {
                if ((pSrcInfo->u16InputVFreq >= 470) && (pSrcInfo->u16InputVFreq <= 520))
                {
                    Hal_XC_H3D_HBLANK(0x2BC);//50Hz=700
                }
                else if ((pSrcInfo->u16InputVFreq >= 570) && (pSrcInfo->u16InputVFreq <= 620))
                {
                    Hal_XC_H3D_HBLANK(0x172);//59.94/60Hz=370
                }

                Hal_XC_H3D_HDE(DOUBLEHD_720X2P_HSIZE);//1280
                Hal_XC_H3D_INIT_VBLANK(0x02);
                Hal_XC_H3D_VDE_F0(DOUBLEHD_720X2P_FRAME_VSIZE);//720
                Hal_XC_H3D_VBLANK0(DOUBLEHD_720X2P_GARBAGE_VSIZE);//0x1E
                Hal_XC_H3D_VBLANK1(DOUBLEHD_720X2P_GARBAGE_VSIZE);//0x1E
                Hal_XC_H3D_VDE_F2(DOUBLEHD_720X2P_FRAME_VSIZE);//0x2D0
                Hal_XC_H3D_VBLANK2(DOUBLEHD_720X2P_GARBAGE_VSIZE);//0x1E
                Hal_XC_H3D_VSYNC_WIDTH(0x01);
                Hal_XC_H3D_VSYNC_POSITION(0x0400);
                Hal_XC_H3D_Enable(TRUE);
                Hal_XC_H3D_Input3DType(E_XC_3D_INPUT_FRAME_PACKING);
                Hal_XC_H3D_SELECT_REGEN_TIMING(TRUE);
                Hal_XC_H3D_Breakline_Enable(FALSE);
                HAL_XC_H3D_OPM_SBYS_PIP_Enable(TRUE);
                if(!_bSkipDefaultLRFlag)
                {
                    Hal_XC_H3D_LR_Toggle_Enable(TRUE);
                }
                else
                {
                    SC_W2BYTEMSK(REG_SC_BK12_33_L, (TRUE<<1), (BIT(1)));
                }
            }
        }
        else if(IS_INPUT_FRAME_PACKING(eWindow) &&
                    IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC())
        {
            stDBreg.u16OPMOffset = stDBreg.u16OPMOffset*2;
            stDBreg.u16DNROffset = stDBreg.u16DNROffset*2;
            if ((pSrcInfo->u16InputVFreq >= 470) && (pSrcInfo->u16InputVFreq <= 520))
            {
                Hal_XC_H3D_HBLANK(0x2BC);//50Hz=700
            }
            else if ((pSrcInfo->u16InputVFreq >= 570) && (pSrcInfo->u16InputVFreq <= 620))
            {
                Hal_XC_H3D_HBLANK(0x172);//59.94/60Hz=370
            }
            Hal_XC_H3D_HDE(DOUBLEHD_720X2P_HSIZE);//1280
            Hal_XC_H3D_INIT_VBLANK(0x02);
            Hal_XC_H3D_VDE_F0(DOUBLEHD_720X2P_FRAME_VSIZE);//720
            Hal_XC_H3D_VBLANK0(DOUBLEHD_720X2P_GARBAGE_VSIZE);//0x1E
            Hal_XC_H3D_VBLANK1(DOUBLEHD_720X2P_GARBAGE_VSIZE);//0x1E
            Hal_XC_H3D_VDE_F2(DOUBLEHD_720X2P_FRAME_VSIZE);//0x2D0
            Hal_XC_H3D_VBLANK2(DOUBLEHD_720X2P_GARBAGE_VSIZE);//0x1E no need in 1280x1470
            Hal_XC_H3D_VSYNC_WIDTH(0x01);
            Hal_XC_H3D_VSYNC_POSITION(0x0400);
            Hal_XC_H3D_Enable(TRUE);
            Hal_XC_H3D_Input3DType(E_XC_3D_INPUT_FRAME_PACKING);
            Hal_XC_H3D_SELECT_REGEN_TIMING(TRUE);
            Hal_XC_H3D_Breakline_Enable(FALSE);
            HAL_XC_H3D_OPM_SBYS_PIP_Enable(TRUE);
            if(!_bSkipDefaultLRFlag)
            {
                Hal_XC_H3D_LR_Toggle_Enable(TRUE);
            }
            else
            {
                SC_W2BYTEMSK(REG_SC_BK12_33_L, (TRUE<<1), (BIT(1)));
            }
            //Hal_SC_force4fb(FALSE, MAIN_WINDOW);
            //Hal_SC_set_wr_bank_mapping(0x00, MAIN_WINDOW);
        }
        else
        {
            Hal_XC_H3D_Enable(FALSE);
            Hal_XC_H3D_Input3DType(E_XC_3D_INPUT_FRAME_PACKING);
            Hal_XC_H3D_Breakline_Enable(FALSE);
            Hal_XC_H3D_HDE(0);
            if(!_bSkipDefaultLRFlag)
            {
                Hal_XC_H3D_LR_Toggle_Enable(FALSE);
            }
            else
            {
                SC_W2BYTEMSK(REG_SC_BK12_33_L, (FALSE<<1), (BIT(1)));
            }
            Hal_XC_H3D_SELECT_REGEN_TIMING(FALSE);

            if( MApi_XC_GetHdmiSyncMode() == HDMI_SYNC_DE)
            {
                Hal_HDMI_Set_YUV422to444_Bypass(TRUE);
            }
            else
            {
                Hal_HDMI_Set_YUV422to444_Bypass(FALSE);
            }
        }

        // Version = 1. 3D mode enable
        if (SC_R2BYTEMSK(REG_SC_BK01_70_L, (BIT(15))) )
        {
            Hal_HDMI_Set_YUV422to444_Bypass(FALSE);
        }
    }
#endif
    //  IPM offset / IPM fetch /OPM offset: All project --> 32pix align-----Fix me: Need consider DNR offset and DNR/OPM base
    //  OPM fetch:
    //            Janus /T7 --> 32pix align
    //            Others ( T8/T9/T12/T13/m10/J2 ) --> 2pix align
    stDBreg.u16OPMFetch  = ( stDBreg.u16OPMFetch + 1 ) & ~1;  //  pixels  alignment;
#endif
}

MS_BOOL MDrv_SC_3D_FormatValidation(XC_InternalStatus *pSrcInfo, SCALER_WIN eWindow)
{
    MS_BOOL bRet = TRUE;
    MS_BOOL bFramePacking = (((pSrcInfo->stCapWin.width == DOUBLEHD_720X2P_HSIZE)
                                && (pSrcInfo->stCapWin.height == DOUBLEHD_720X2P_VSIZE))
                              || ((pSrcInfo->stCapWin.width == DOUBLEHD_1080X2P_HSIZE)
                                && (pSrcInfo->stCapWin.height == DOUBLEHD_1080X2P_VSIZE))
                              || ((pSrcInfo->stCapWin.width == DOUBLEHD_1080X2I_HSIZE)
                                && (pSrcInfo->stCapWin.height == DOUBLEHD_1080X2I_VSIZE))
                                ) && (!pSrcInfo->bInterlace);
    MS_ASSERT(eWindow < MAX_WINDOW);
    MS_ASSERT(pSrcInfo);

    if(eWindow >= MAX_WINDOW)
    {
        return FALSE;
    }

    if(IS_INPUT_MODE_NONE(eWindow))
    {
        return TRUE;
    }

    if(IS_INPUT_FRAME_PACKING_OPT(eWindow))
    {
        return TRUE;
    }

    bRet = !(IS_INPUT_FRAME_PACKING(eWindow) ^ bFramePacking);

    if(!bRet)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: Frame Packing mismatch: bFPP=%u, 3D Mode=%u !\r\n", bFramePacking, _eInput3DMode[eWindow]);
    }

    if(IS_INPUT_FRAME_ALTERNATIVE(eWindow))
    {
        if(pSrcInfo->bInterlace)
        {
            bRet = FALSE;
        }
    }


#if (HW_DESIGN_3D_VER < 2)
    else if(IS_INPUT_FIELD_ALTERNATIVE(eWindow)
            || IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow))
    {
        if(!pSrcInfo->bInterlace)
        {
            bRet = FALSE;
            XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: only interlace timing is supported\n");
        }
    }
    else if(IS_INPUT_LINE_ALTERNATIVE(eWindow)
            || IS_OUTPUT_FRAME_ALTERNATIVE())
    {
        if(pSrcInfo->bInterlace)
        {
            bRet = FALSE;
            XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: only progresive timing is supported\n");
        }
    }
#endif
#if (HW_DESIGN_3D_VER!= 4)
    if(IS_OUTPUT_FRAME_ALTERNATIVE())
    {
        if((pSrcInfo->u16InputVFreq > 350)
           &&
           ((g_XC_InitData.stPanelInfo.u16DefaultVFreq == 500)
            || (g_XC_InitData.stPanelInfo.u16DefaultVFreq == 600)))
        {

            bRet = FALSE;

            printf("3D: need double framerate for 3D formatE_XC_3D_OUTPUT_FRAME_ALTERNATIVE\n");
        }
    }
#endif

    if(!bRet)
    {
        printf("MDrv_SC_3D_FormatValidation(): invalid format, set to 3d_none\n");
        _XC_ENTRY();
		#ifdef MULTI_SCALER_SUPPORTED
		Hal_SC_subwindow_disable(eWindow);
		#else
        Hal_SC_subwindow_disable();
		#endif
		_XC_RETURN();
        _eInput3DMode[eWindow] = E_XC_3D_INPUT_MODE_NONE;
        _eOutput3DMode = E_XC_3D_OUTPUT_MODE_NONE;
        pSrcInfo->stDispWin.x = 0;
        pSrcInfo->stDispWin.y = 0;
        pSrcInfo->stDispWin.width = g_XC_InitData.stPanelInfo.u16Width;
        pSrcInfo->stDispWin.height = g_XC_InitData.stPanelInfo.u16Height;
        #if (HW_DESIGN_3D_VER < 2)
            MApi_XC_Enable_3D_LR_Sbs2Line(DISABLE);
        #endif
    }
    return bRet;
}

void MDrv_SC_3D_AdjustHShift(XC_InternalStatus *pSrcInfo, SCALER_WIN eWindow)
{
    MS_ASSERT(eWindow < MAX_WINDOW);
    MS_ASSERT(pSrcInfo);

    if(pSrcInfo == NULL)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: pSrcInfo is null pointer!\r\n");
        return;
    }

    if(eWindow >= MAX_WINDOW)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: eWindow is wrong!\r\n");
        return;
    }

#if (HW_DESIGN_3D_VER < 2)
    if((IS_INPUT_NORMAL_2D(eWindow) && IS_OUTPUT_TOP_BOTTOM())
       ||  (IS_INPUT_NORMAL_2D(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF()))
    {
        _u163DHShift = (_u163DHShift + 1) & ~1;
        pSrcInfo->stCropWin.width -= _u163DHShift;
        if(MAIN_WINDOW == eWindow)
        {
            pSrcInfo->stCropWin.x += _u163DHShift;
        }
    }
#if (HW_2DTO3D_SUPPORT == FALSE)
    else
    {
        _u163DHShift = (_u163DHShift + OFFSET_PIXEL_ALIGNMENT - 1) & ~(OFFSET_PIXEL_ALIGNMENT-1);
    }
#endif
#endif
}

//disable sub window write dnr buffer when no needed
void MDrv_SC_3D_SetWriteMemory(SCALER_WIN eWindow)
{
    MS_ASSERT(eWindow < MAX_WINDOW);
    if(eWindow >= MAX_WINDOW)
    {
        return;
    }

    _XC_ENTRY();

#if (HW_DESIGN_3D_VER >= 2)
    if(IS_OUTPUT_TOP_BOTTOM()
       || IS_OUTPUT_SIDE_BY_SIDE_HALF()
       || IS_OUTPUT_LINE_ALTERNATIVE()
      )
    {
        Hal_SC_memory_write_request(FALSE, SUB_WINDOW);
        Hal_SC_memory_read_request(FALSE, SUB_WINDOW);
    }
    else
    {
        Hal_SC_memory_write_request(TRUE, SUB_WINDOW);
        Hal_SC_memory_read_request(TRUE, SUB_WINDOW);
    }
#else
    if(SUB_WINDOW == eWindow)
    {
        if(IS_INPUT_MODE_NONE(eWindow)
           || (IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF()
                && (gSrcInfo[eWindow].stCapWin.width == gSrcInfo[eWindow].stDispWin.width))
           || IS_INPUT_FRAME_PACKING_OPT(eWindow)
           || IS_INPUT_TOP_BOTTOM_OPT(eWindow)
           || (IS_INPUT_FRAME_ALTERNATIVE(eWindow) && IS_OUTPUT_TOP_BOTTOM() && (!MDrv_SC_3D_IsUseSameBuffer()))
           || (IS_INPUT_NORMAL_2D(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF() && (!MDrv_SC_3D_IsUseSameBuffer()))
           || (IS_INPUT_NORMAL_2D(eWindow) && IS_OUTPUT_TOP_BOTTOM() && (!MDrv_SC_3D_IsUseSameBuffer()))
#if (HW_DESIGN_3D_VER == 1)
           || (IS_INPUT_FRAME_ALTERNATIVE(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF() && (!MDrv_SC_3D_IsUseSameBuffer()))
#endif
           )
        {// 2D format, sbs in, sbs out centering, framepacking opt,
         // or frame alternative in, top bottom out
            Hal_SC_memory_write_request(TRUE, eWindow);
            Hal_SC_memory_read_request(TRUE, eWindow);
        }
        else
        {
            Hal_SC_memory_write_request(FALSE, eWindow);
            Hal_SC_memory_read_request(FALSE, eWindow);
        }
    }
#endif

    _XC_RETURN();
}

MS_BOOL MDrv_SC_3D_Is3DProcessByPIP(SCALER_WIN eWindow)
{
#if (HW_DESIGN_3D_VER <= 1)
    return ((IS_OUTPUT_TOP_BOTTOM() || IS_OUTPUT_SIDE_BY_SIDE_HALF())
            && (!(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF()
                && (gSrcInfo[eWindow].stCapWin.width == gSrcInfo[eWindow].stDispWin.width))));
#else
    return FALSE;
#endif
}

MS_BOOL MDrv_SC_3D_Is2Dto3DCase(E_XC_3D_INPUT_MODE e3dInputMode,
                                E_XC_3D_OUTPUT_MODE e3dOutputMode)
{
    MS_BOOL bRet = FALSE;

#if (HW_DESIGN_3D_VER <= 1)
    if(((e3dInputMode == E_XC_3D_INPUT_NORMAL_2D) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_LINE_ALTERNATIVE))
       ||
       ((e3dInputMode == E_XC_3D_INPUT_NORMAL_2D_INTERLACE) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_LINE_ALTERNATIVE))
       ||
       ((e3dInputMode == E_XC_3D_INPUT_NORMAL_2D_INTERLACE_PTP) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_LINE_ALTERNATIVE))
       ||
       ((e3dInputMode == E_XC_3D_INPUT_NORMAL_2D) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE_NOFRC))
      )
    {
        bRet = TRUE;
    }
#endif

    return bRet;
}

void MDrv_SC_3D_CloneMainSettings(XC_InternalStatus *pSrcInfo,
                                  SCALER_WIN eWindow)
{

    MS_ASSERT(eWindow < MAX_WINDOW);
    MS_ASSERT(pSrcInfo);

    if(pSrcInfo == NULL)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: pSrcInfo is null pointer!\r\n");
        return;
    }

    if(eWindow == MAX_WINDOW)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: eWindow is wrong!\r\n");
        return;
    }

#if (HW_DESIGN_3D_VER <= 1)
    if(SUB_WINDOW == eWindow)
    {
        if(IS_OUTPUT_TOP_BOTTOM() || IS_OUTPUT_SIDE_BY_SIDE_HALF())
        {
            MS_U8 u8Data_Mux = 0;
            MS_U8 u8Clk_Mux = 0;

        #ifdef MULTI_SCALER_SUPPORTED
            MDrv_XC_EnableCLK_for_SUB(TRUE, eWindow);
        #else
            MDrv_XC_EnableCLK_for_SUB(TRUE);
        #endif
            Hal_SC_mux_get_mainwin_ip_mux(&u8Data_Mux, &u8Clk_Mux);
            Hal_SC_set_subwin_ip_mux(u8Data_Mux, u8Clk_Mux);
            if(MDrv_SC_3D_IsUseSameBuffer())
            {
                if(pSrcInfo->bInterlace)
                {
                    pSrcInfo->eDeInterlaceMode = gSrcInfo[MAIN_WINDOW].eDeInterlaceMode;
                }
                pSrcInfo->u8BitPerPixel = gSrcInfo[MAIN_WINDOW].u8BitPerPixel;
            }
        }
    }
#else
    if(IS_OUTPUT_TOP_BOTTOM()
       || IS_OUTPUT_SIDE_BY_SIDE_HALF()
       || IS_OUTPUT_LINE_ALTERNATIVE()
      )
    {
        MS_U8 u8Data_Mux = 0;
        MS_U8 u8Clk_Mux = 0;

    #ifdef MULTI_SCALER_SUPPORTED
        MDrv_XC_EnableCLK_for_SUB(TRUE, eWindow);
    #else
        MDrv_XC_EnableCLK_for_SUB(TRUE);
    #endif
        Hal_SC_mux_get_mainwin_ip_mux(&u8Data_Mux, &u8Clk_Mux);
        Hal_SC_set_subwin_ip_mux(u8Data_Mux, u8Clk_Mux);
    }
    else if(!IS_INPUT_MODE_NONE(MAIN_WINDOW))
    {
    #ifdef MULTI_SCALER_SUPPORTED
        MDrv_XC_EnableCLK_for_SUB(FALSE, eWindow);
    #else
        MDrv_XC_EnableCLK_for_SUB(FALSE);
    #endif
    }
#endif
}

MS_BOOL MDrv_SC_3D_IsUseSameBuffer(void)
{
    MS_BOOL bRet = FALSE;

#if (HW_DESIGN_3D_VER <= 1)
    return (MS_IPM_BASE0(MAIN_WINDOW) == MS_IPM_BASE0(SUB_WINDOW));
#endif

    return bRet;
}

void MDrv_SC_3D_Adjust_MirrorDNROffset(XC_InternalStatus *pSrcInfo,
                                       MS_U16 *pu16V_SizeAfterPreScaling,
                                       MS_U16 *pu16DNROffset,
                                       MS_U16 *pu16Ratio,
                                       SCALER_WIN eWindow)
{
    MS_ASSERT(eWindow < MAX_WINDOW);
    MS_ASSERT(pSrcInfo);

    if(pSrcInfo == NULL)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: pSrcInfo is null pointer!\r\n");
        return;
    }

    if(eWindow == MAX_WINDOW)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: eWindow is wrong!\r\n");
        return;
    }

#if (HW_DESIGN_3D_VER >= 2)
    if(IS_INPUT_LINE_ALTERNATIVE(eWindow)
       || IS_INPUT_TOP_BOTTOM(eWindow))
    {
        *pu16V_SizeAfterPreScaling /= 2;
        *pu16Ratio *= 2;
    }
    else if(IS_INPUT_FRAME_PACKING(eWindow))
    {
        MS_U16 u16VactVideo = 0;
        MS_U16 u16VactSpace = 0;
        MDrv_SC_3D_GetFP_Info(pSrcInfo, &u16VactVideo, &u16VactSpace, (MS_BOOL)((stDBreg.u32V_PreScalingRatio & BIT(31))>>31));
        *pu16V_SizeAfterPreScaling = u16VactVideo;
        *pu16Ratio *= 2;
    }
    else if(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow))
    {
        *pu16DNROffset = ((*pu16DNROffset / 2) + OFFSET_PIXEL_ALIGNMENT - 1) & ~(OFFSET_PIXEL_ALIGNMENT-1);
        *pu16Ratio *= 2;
    }
    else if(IS_INPUT_FRAME_ALTERNATIVE(eWindow))
    {
        *pu16DNROffset *= 2;
        *pu16Ratio *= 1;
    }
#else
    if((IS_INPUT_FRAME_ALTERNATIVE(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
         ||(IS_INPUT_NORMAL_2D_INTERLACE(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
         ||(IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
         ||(IS_INPUT_TOP_BOTTOM(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE() && gSrcInfo[eWindow].bInterlace)
         ||(IS_INPUT_FIELD_ALTERNATIVE(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
       )
    {
        *pu16V_SizeAfterPreScaling *= 2;
    }
#if (HW_DESIGN_3D_VER == 1)
    else if(IS_INPUT_FRAME_PACKING(eWindow) && IS_OUTPUT_FRAME_ALTERNATIVE())
    {
        MS_U16 u16VactVideo = 0;
        MS_U16 u16VactSpace = 0;
        MDrv_SC_3D_GetFP_Info(pSrcInfo, &u16VactVideo, &u16VactSpace, (stDBreg.u32V_PreScalingRatio & BIT(31))>>31);
        *pu16V_SizeAfterPreScaling = u16VactVideo;
        *pu16Ratio *= 2;
    }
#endif
#endif
}

void MDrv_SC_3D_AdjustMirrorCrop(XC_InternalStatus *pSrcInfo,SCALER_WIN eWindow)
{
#if (HW_DESIGN_3D_VER >= 2)
     if(IS_INPUT_TOP_BOTTOM(eWindow)
     || IS_INPUT_FRAME_PACKING(eWindow)
     || IS_INPUT_FRAME_ALTERNATIVE(eWindow)
     || IS_INPUT_LINE_ALTERNATIVE(eWindow))
     {
         //app set overscan'value that is L crop value,but L and R is two crop'value.
         gSrcInfo[eWindow].ScaledCropWin.y = gSrcInfo[eWindow].ScaledCropWin.y * 2;
     }
     if(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow)
     ||IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow)
     ||IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE_OPT(eWindow)
     ||IS_INPUT_SIDE_BY_SIDE_FULL(eWindow))
     {
        gSrcInfo[eWindow].ScaledCropWin.x =gSrcInfo[eWindow].ScaledCropWin.x * 2;
     }
#endif
     if( pSrcInfo->u16H_SizeAfterPreScaling > (gSrcInfo[eWindow].ScaledCropWin.x + gSrcInfo[eWindow].ScaledCropWin.width) )
     {
        gSrcInfo[eWindow].ScaledCropWin.x = pSrcInfo->u16H_SizeAfterPreScaling - (gSrcInfo[eWindow].ScaledCropWin.x + gSrcInfo[eWindow].ScaledCropWin.width);
         // select nearest even number
     }
     else
     {
         gSrcInfo[eWindow].ScaledCropWin.x = 0;
         if(gSrcInfo[eWindow].ScaledCropWin.width > pSrcInfo->u16H_SizeAfterPreScaling)
         {
             gSrcInfo[eWindow].ScaledCropWin.width = pSrcInfo->u16H_SizeAfterPreScaling;
         }
         XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"Scaled crop[2.a] Force x=%u, width=%u\n",gSrcInfo[eWindow].ScaledCropWin.x, gSrcInfo[eWindow].ScaledCropWin.width);
     }

     // for interlace, keep y in multiple of 2
     if( pSrcInfo->u16V_SizeAfterPreScaling > (gSrcInfo[eWindow].ScaledCropWin.y + gSrcInfo[eWindow].ScaledCropWin.height) )
     {
         gSrcInfo[eWindow].ScaledCropWin.y = (pSrcInfo->u16V_SizeAfterPreScaling - (gSrcInfo[eWindow].ScaledCropWin.y + gSrcInfo[eWindow].ScaledCropWin.height) + 1) & ~0x1;
     }
     else
     {
         gSrcInfo[eWindow].ScaledCropWin.y = 0;
         if(gSrcInfo[eWindow].ScaledCropWin.height > pSrcInfo->u16V_SizeAfterPreScaling)
         {
             gSrcInfo[eWindow].ScaledCropWin.height = pSrcInfo->u16V_SizeAfterPreScaling;
         }
         XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"Scaled crop[2.b] Force y=%u, height=%u\n",gSrcInfo[eWindow].ScaledCropWin.y, gSrcInfo[eWindow].ScaledCropWin.height);
     }
#if (HW_DESIGN_3D_VER >= 2)
     if(IS_INPUT_TOP_BOTTOM(eWindow)
     || IS_INPUT_FRAME_PACKING(eWindow)
     || IS_INPUT_FRAME_ALTERNATIVE(eWindow)
     || IS_INPUT_LINE_ALTERNATIVE(eWindow))
     {
         gSrcInfo[eWindow].ScaledCropWin.y /= 2;
     }
     if(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow)
     ||IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow)
     ||IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE_OPT(eWindow)
     ||IS_INPUT_SIDE_BY_SIDE_FULL(eWindow))
     {
        gSrcInfo[eWindow].ScaledCropWin.x /= 2;
     }
#endif
}

// In Side by Side input case,  DNROffset will be divided into half.
void MDrv_SC_3D_Adjust_DNROffset(MS_U16 *pu16DNROffsetTemp,MS_U16 *pu16DNROffset, SCALER_WIN eWindow)
{
    MS_ASSERT(eWindow < MAX_WINDOW);

    if(eWindow == MAX_WINDOW)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: eWindow is wrong!\r\n");
        return;
    }

#if (HW_DESIGN_3D_VER >= 2)
    if(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow)
        ||IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow)
        ||IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE_OPT(eWindow)
        ||IS_INPUT_SIDE_BY_SIDE_FULL(eWindow))
    {
        *pu16DNROffsetTemp = ((*pu16DNROffsetTemp / 2) + OFFSET_PIXEL_ALIGNMENT - 1) & ~(OFFSET_PIXEL_ALIGNMENT-1);
        *pu16DNROffset = *pu16DNROffsetTemp*2;
        //In Side by Side input case, main and sub set alignment in same way.But in different
        //timing, alignment is different.
        //If we just care DNROffset's alignment.HW will set main DNROffset/2 and sub DNROffset/2,
        //maybe main's DNROffset isn't alignment.
        //eg. 800x600 case:
        //if we don't do *pu16DNROffset = *pu16DNROffsetTemp*2.DNROffset is 800,
        //HW will give main 400 and sub 400.
        //But DNROffsetTemp is 416,so it was croped 16*cropwin.y .
    }
#endif
}

// In Side by Side input case,main and sub's Cropwinwidth are 1/2;
void MDrv_SC_3D_Adjust_CropWinWidth(MS_U16 *pu16CropWinWidth,SCALER_WIN eWindow)
{
    MS_ASSERT(eWindow < MAX_WINDOW);

    if(eWindow == MAX_WINDOW)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: eWindow is wrong!\r\n");
        return;
    }

#if (HW_DESIGN_3D_VER >= 2)
    if(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow)
     ||IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow)
     ||IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE_OPT(eWindow)
     ||IS_INPUT_SIDE_BY_SIDE_FULL(eWindow))
    {
        *pu16CropWinWidth /= 2;
        if((*pu16CropWinWidth & 0x1)!= 0)
        {
            *pu16CropWinWidth = (*pu16CropWinWidth+1)& ~1;
        }
    }
#endif
}


// If 3d is enable for (HW_DESIGN_3D_VER >= 2),
// mem unit will split into L and R, so bytesPerPixel should be doubled.
void MDrv_SC_3D_Adjust_BytesPer2Pixel(MS_U8 *u8BytesPer2PixelTemp,SCALER_WIN eWindow)
{
     MS_ASSERT(eWindow < MAX_WINDOW);

    if(eWindow == MAX_WINDOW)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"3D: eWindow is wrong!\r\n");
        return;
    }
 #if (HW_DESIGN_3D_VER >= 2)
    //if REG_SC_BK12_30_L bit0 is enabled, we should add into this case.
    if(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow)
        || IS_INPUT_SIDE_BY_SIDE_FULL(eWindow)
        || IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE_OPT(eWindow)
        || IS_INPUT_TOP_BOTTOM(eWindow)
        || IS_INPUT_FRAME_PACKING(eWindow)
        || IS_INPUT_FRAME_ALTERNATIVE(eWindow)
        || IS_INPUT_LINE_ALTERNATIVE(eWindow)
        || IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow))
    {
        *u8BytesPer2PixelTemp *= 2;
    }

#endif

}

MS_BOOL MDrv_SC_3D_Is_LR_Sbs2Line(void)
{
    return _bIsLR_Sbs2Line;
}

MS_BOOL MApi_SC_3D_Is_Skip_Default_LR_Flag(void)
{
    return _bSkipDefaultLRFlag;
}

MS_BOOL MApi_XC_3D_Enable_Skip_Default_LR_Flag(MS_BOOL bEnable)
{
    _bSkipDefaultLRFlag = bEnable;
    return TRUE;
}


//------------------------------------------------------------------------------
//--------HW 2D to 3D CPU Depth Refine Part Begin-------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// the following part codes is copied from ursa5
#if HW_2DTO3D_SUPPORT

#define YMAX            0
#define YMIN            1
#define YCTRS           2
#define BLUE            3
#define YAVG            4
#define HEDGE           5
#define VEDGE           6
#define GREEN           7
#define BUILDING        8
#define YAVGEDGE        9
#define CBAVG           10
#define CRVG            11
#define OBJIBNDRY       12
#define OBJIDENT        13

MS_U8  _u8FieldArray[14][2040];
MS_U8  _u8YCtrArray[256][2040];
MS_U32 _32YSumArray[2040];
MS_U32 _u32HisWeight = 0;
MS_U32 _u32ArtWeight = 0;
MS_U32 _u32ReturnWeightCur = 0;
MS_U32 _u32ReturnWeight = 0;
MS_U32 _u32X = 0xe8c9af37;
MS_U32 _u32RemapAvgCurArray[256];
MS_U32 _u32RemapAvgCurSum = 0;
MS_U32 _u32RemapAvgBseArray[256];
MS_U32 _u32RemapAvgBseSum = 0;
MS_U32 _u32RemapAvgCur = 0;
MS_U32 _u32RemapAvgBse = 0;
MS_U8 _u8RemapAvgBias = 0;
MS_U8 _u8RemapLpfArray[2040];

static MS_U32 minmax(MS_U32 u32X, MS_U32 u32Min, MS_U32 u32Max)
{
   return (u32X <= u32Min) ? u32Min :
          (u32X >= u32Max) ? u32Max : u32X;
}

static MS_U32 max(MS_U32 u32A, MS_U32 u32B)
{
   return (u32A <= u32B) ? u32B : u32A;
}

static MS_U32 min(MS_U32 u32A, MS_U32 u32B)
{
   return (u32A <= u32B) ? u32A : u32B;
}

static MS_U32 slope_gen(MS_U8 i,
                        MS_U8 u8Slope,
                        MS_U8 u8Element,
                        MS_U8 u8ModSel,
                        MS_U8 u8Motion)
{
    MS_U32 u32Rnd = 1;
    MS_U32 u32Tmp = (MS_U32)i * u8Slope;
    if(u8Motion != 0)
    {
        u32Rnd = (u8ModSel == 0) ? u8Element >> 4:
                 (u8ModSel == 1) ? u8Element >> 3:
                 (u8ModSel == 2) ? u8Element >> 2:
                                   u8Element >> 1;
        u32Tmp = u32Tmp + u32Rnd;
        u32Tmp = (u32Tmp > 255) ? 255 : u32Tmp;
    }
    return u32Tmp;
}

static MS_U8 remap(MS_U8 u8Cls,
                   MS_U8 u8BlkHght,
                   MS_U8 u8BlkWdth,
                   MS_U8 i,
                   MS_U8 j,
                   MS_U8 u8NrmlEn)//i: block line number, j: block idx
{
    static MS_U8 s_u8flg = 0;
    static MS_U8 s_u8Ptr = 0;
    MS_U32 u32Idx = 0;
    MS_U8 u8Shift = 0;
    MS_U32 u32MapIdx = 0;
    MS_U8 u8Result = 0;
    MS_U8 u8Bln_u = 0;
    MS_U8 u8Bln_d = 0;
    if(s_u8flg == 0)
    {
        for(u32Idx = 0; u32Idx < 256; u32Idx++)
        {
            _u32RemapAvgCurArray[u32Idx] = 0;
            _u32RemapAvgBseArray[u32Idx] = 0;
        }
        for(u32Idx = 0; u32Idx < 2040; u32Idx++)
        {
            _u8RemapLpfArray[u32Idx] = 0;
        }
        _u32RemapAvgCurSum = 0;
        _u32RemapAvgBseSum = 0;
        s_u8flg = 1;
    }
    s_u8Ptr = s_u8Ptr + 1;
    u8Shift = (i == 5) ? u8BlkWdth-1 :
            (i == 6) ? u8BlkWdth-2 :
            (i == 7) ? u8BlkWdth-3 :
            (i >= 8) ? u8BlkWdth-4 : 0;

    u32MapIdx = i * u8BlkWdth + ( (j + u8Shift) % u8BlkWdth );
    u8Result = _u8FieldArray[u8Cls][u32MapIdx];

    // noise reduction start
    if(i == 0 && j == 0)
    {
        _u32RemapAvgCur = _u32RemapAvgCur / ((u8BlkHght - 1) * u8BlkWdth);
        _u32RemapAvgBse = _u32RemapAvgBse / u8BlkWdth;
        _u32RemapAvgCurSum = _u32RemapAvgCurSum + _u32RemapAvgCur - _u32RemapAvgCurArray[s_u8Ptr];
        _u32RemapAvgCurArray[s_u8Ptr] = _u32RemapAvgCur;
        _u32RemapAvgBseSum = _u32RemapAvgBseSum + _u32RemapAvgBse - _u32RemapAvgBseArray[s_u8Ptr];
        _u32RemapAvgBseArray[s_u8Ptr] = _u32RemapAvgBse;
        _u8RemapAvgBias = (MS_U8)( (_u32RemapAvgCurSum >> 8) - (_u32RemapAvgBseSum >> 8));
    }

    _u32RemapAvgCur = (i == 1 && j == 0) ? u8Result :
                      (i != 0) ? _u32RemapAvgCur + (MS_U32)u8Result : _u32RemapAvgCur;

    _u32RemapAvgBse = (i == 0 && j == 0) ? u8Result :
                      (i == 0) ? _u32RemapAvgBse + (MS_U32)u8Result : _u32RemapAvgBse;

    u8Result = ((u8NrmlEn & 0x02) == 2) ? _u8RemapAvgBias :
                ( (u8NrmlEn & 0x01) == 1 && i != 0) ? u8Result - _u8RemapAvgBias : u8Result;

    _u8RemapLpfArray[i * u8BlkWdth + j] = u8Result;
    u8Bln_u = (i == 0) ? 0 : i - 1;
    u8Bln_d = (i == u8BlkHght - 1) ? u8BlkHght - 1 : i + 1;
    u8Bln_u = _u8RemapLpfArray[u8Bln_u * u8BlkWdth + j];
    u8Bln_d = _u8RemapLpfArray[u8Bln_d * u8BlkWdth + j];

    u8Result = ( (u8NrmlEn & 0x04) == 4) ? ((u8Bln_u + (u8Result  << 1) + u8Bln_d) >> 2 ) : u8Result;
    // noise reduction end

    return u8Result;
}

static MS_U32 lpf3x3(MS_U32 u32YY, MS_U32 u32XX, MS_U32 u32Index)
{
    MS_U32 d_r = 0, d_l = 0, d_c = 0;
    MS_U32 u_r = 0, u_l = 0, u_c = 0;
    MS_U32 c_r = 0, c_l = 0, c_c = 0;

    c_l = _u8FieldArray[u32Index][u32YY * 60 + u32XX - 1];
    c_c = _u8FieldArray[u32Index][u32YY * 60 + u32XX];
    c_r = _u8FieldArray[u32Index][u32YY * 60 + u32XX + 1];
    u_l = _u8FieldArray[u32Index][(u32YY - 1) * 60 + u32XX - 1];
    u_c = _u8FieldArray[u32Index][(u32YY - 1) * 60 + u32XX];
    u_r = _u8FieldArray[u32Index][(u32YY - 1) * 60 + u32XX + 1];
    d_l = _u8FieldArray[u32Index][(u32YY + 1) * 60 + u32XX - 1];
    d_c = _u8FieldArray[u32Index][(u32YY + 1) * 60 + u32XX];
    d_r = _u8FieldArray[u32Index][(u32YY + 1) * 60 + u32XX + 1];

    c_l = (u32XX == 0) ? c_c : c_l;
    c_r = (u32XX == 59) ? c_c : c_r;
    u_l = (u32YY == 0 || u32XX == 0) ? c_c : u_l;
    u_r = (u32YY == 0 || u32XX == 59) ? c_c : u_r;
    u_c = (u32YY == 0) ? c_c : u_c;
    d_l = (u32YY == 33|| u32XX == 0) ? c_c : d_l;
    d_r = (u32YY == 33|| u32XX == 59) ? c_c : d_r;
    d_c = (u32YY == 33) ? c_c : d_c;
    c_c = (u_r + u_c + u_l +
           c_r + c_c + c_l +
           d_r + d_c + d_l) >> 4;

    return c_c;
}

#if 0
static MS_U32 yslp_func(MS_U32 u32VDirEn,
                        MS_U32 u32VDirPn,
                        MS_U32 u32VDirStrd,
                        MS_U32 u32BuildFlag,
                        MS_U32 u32YAxis,
                        MS_U32 u32Result)
{
    MS_U32 u32Slope = (u32VDirPn) ? (34 - u32YAxis) : u32YAxis;
    MS_U32 u32YResult = (u32BuildFlag) ? u32Result : (u32Slope * u32VDirStrd + ( u32Result - 128 ) / 4 );
    return u32Result = (u32VDirEn) ? u32YResult : u32Result;
}

static MS_U32 vlpf1x3(MS_U32 u32YY, MS_U32 u32XX, MS_U32 u32Index, MS_U32 u32FltrEn)
{
    MS_U32 d_c = 0;
    MS_U32 u_c = 0;
    MS_U32 c_c = 0;

    c_c = _u8FieldArray[u32Index][u32YY * 60 + u32XX];
    u_c = _u8FieldArray[u32Index][(u32YY - 1) * 60 + u32XX];
    d_c = _u8FieldArray[u32Index][(u32YY + 1) * 60 + u32XX];

    if(u32FltrEn)
    {
        u_c = (u32YY == 0)?c_c:u_c;
        d_c = (u32YY == 33)?c_c:d_c;
        c_c = (u_c + (c_c << 1) + d_c) >> 2;
    }
    return c_c;
}
#endif

static MS_U32 _MDrv_SC_3D_2DTo3D_Art_Detect(void)
{
    MS_U32 i = 0;
    MS_U32 u32HisgrmArray[16];
    MS_U32 u32XStart =  512;
    MS_U32 u32XEnd   =  1408;    //896
    MS_U32 u32YStart =  256;     //640
    MS_U32 u32YEnd   =  896;
    MS_U32 u32PureColorBlkCnt = SC_R2BYTE(REG_SC_BK15_3D_L); //16bit 3d => 8bit 7a,7b
    MS_U32 u32GradLevelBlkCnt = SC_R2BYTE(REG_SC_BK15_3E_L); //16bit 3e => 8bit 7c,7d
    MS_U32 u32HighFreqBlkCnt  = SC_R2BYTE(REG_SC_BK15_3F_L); //16bit 3f => 8bit 7e,7f
    MS_U32 u32HisWeightCur = 0;
    MS_U32 u32MinRatio = 0;
    MS_U32 u32NonSmallHistogramCnt = 0;
    MS_U32 u32NonzeroHistogramCnt = 0;
    MS_U32 u32BiggestHistogram = 0;
    MS_U32 u32BiggestHistogramIdx = 0;
    MS_U32 u32SecondHistogram = 0;
    MS_U32 u32SecondHistogramIdx = 0;
    MS_U32 u32ThirdHistogram = 0;
    MS_U32 u32TempRatio1 = 0;
    MS_U32 u32TempRatio2 = 0;
    MS_U32 u32ArtWeightCurHF = 0;    // HF weight
    MS_U32 u32ArtIndex = 0;
    MS_U32 u32ArtWeightCurAll = 0;
    MS_U32 u32ArtWeightCur = 0;
    MS_U32 u32X1        = 0;
    MS_U32 u32DBit0     =  0;
    MS_U32 u32DBit1     =  0;
    MS_U32 u32DBit2     =  0;
    MS_U32 u32DBit3     =  0;
    MS_U32 u32DitherOut =  0;

    // read histogram
    u32HisgrmArray[0] = SC_R2BYTEMSK(REG_SC_BK15_30_L, 0xFF00) >> 8;
    u32HisgrmArray[1] = SC_R2BYTEMSK(REG_SC_BK15_30_L, 0x00FF);
    u32HisgrmArray[2] = SC_R2BYTEMSK(REG_SC_BK15_31_L, 0xFF00) >> 8;
    u32HisgrmArray[3] = SC_R2BYTEMSK(REG_SC_BK15_31_L, 0x00FF);
    u32HisgrmArray[4] = SC_R2BYTEMSK(REG_SC_BK15_32_L, 0xFF00) >> 8;
    u32HisgrmArray[5] = SC_R2BYTEMSK(REG_SC_BK15_32_L, 0x00FF);
    u32HisgrmArray[6] = SC_R2BYTEMSK(REG_SC_BK15_33_L, 0xFF00) >> 8;
    u32HisgrmArray[7] = SC_R2BYTEMSK(REG_SC_BK15_33_L, 0x00FF);
    u32HisgrmArray[8] = SC_R2BYTEMSK(REG_SC_BK15_34_L, 0xFF00) >> 8;
    u32HisgrmArray[9] = SC_R2BYTEMSK(REG_SC_BK15_34_L, 0x00FF);
    u32HisgrmArray[10] = SC_R2BYTEMSK(REG_SC_BK15_35_L, 0xFF00) >> 8;
    u32HisgrmArray[11] = SC_R2BYTEMSK(REG_SC_BK15_35_L, 0x00FF);
    u32HisgrmArray[12] = SC_R2BYTEMSK(REG_SC_BK15_36_L, 0xFF00) >> 8;
    u32HisgrmArray[13] = SC_R2BYTEMSK(REG_SC_BK15_36_L, 0x00FF);
    u32HisgrmArray[14] = SC_R2BYTEMSK(REG_SC_BK15_37_L, 0xFF00) >> 8;
    u32HisgrmArray[15] = SC_R2BYTEMSK(REG_SC_BK15_37_L, 0x00FF);

    //------------- Set Detection Window --------------------------
    SC_W2BYTE(REG_SC_BK15_28_L, u32XStart);
    SC_W2BYTE(REG_SC_BK15_29_L, u32XEnd);
    SC_W2BYTE(REG_SC_BK15_2A_L, u32YStart);
    SC_W2BYTE(REG_SC_BK15_2B_L, u32YEnd);

    u32X1 = ((_u32X & 0x01000000) >> 24) ^ ((_u32X & 0x00001000) >> 12);
    u32DBit0 =  ((_u32X & 0x00000008) >> 3)   ^  ((_u32X & 0x00001000) >> 12);                                 // bit 3    ^  bit 12
    u32DBit1 =  ((_u32X & 0x00000040) >> 6)   ^  ((_u32X & 0x00040000) >> 18);                                 // bit 6    ^  bit 18
    u32DBit2 =  ((_u32X & 0x01000000) >> 24)  ^  ((_u32X & 0x40000000) >> 30);                                 // bit 24   ^  bit 30
    u32DBit3 =  ((_u32X & 0x00000200) >> 9)   ^  ((_u32X & 0x00100000) >> 20);                                 // bit  9   ^  bit  20
    u32DitherOut =  (u32DBit3 << 3) + (u32DBit2 << 2) + (u32DBit1 << 1) + u32DBit0;

    _u32X = ((_u32X & 0x7fffffff) << 1) + u32X1;

    u32HisWeightCur = 0;
    u32SecondHistogram = 0;
    u32BiggestHistogram = 0;

    for(i = 0; i < 16; i++)
    {
        if(u32HisgrmArray[i] >= 0x3)
        {
            u32NonSmallHistogramCnt++;
        }

        if(u32HisgrmArray[i] > 0)
        {
            u32NonzeroHistogramCnt++;
        }

        if(u32HisgrmArray[i] == 0x0)
        {
            u32HisgrmArray[i] = 0x1;
        }

        if(u32HisgrmArray[i] > u32BiggestHistogram)
        {
            u32BiggestHistogram = u32HisgrmArray[i];
            u32BiggestHistogramIdx = i;
        }
    }

    for(i = 0; i < 16; i++)
    {
        if(i != u32BiggestHistogramIdx && u32HisgrmArray[i] >= u32SecondHistogram)
        {
            u32SecondHistogram = u32HisgrmArray[i];
            u32SecondHistogramIdx = i;
        }
    }

    for(i = 0; i < 16; i++)
    {
        if(i != u32BiggestHistogramIdx && i != u32SecondHistogramIdx && u32HisgrmArray[i] >= u32ThirdHistogram)
        {
            u32ThirdHistogram = u32HisgrmArray[i];
        }
    }

    for(i = 0; i < 16; i++)
    {
        if(i == 0)
        {
            u32MinRatio = min(u32HisgrmArray[0] / (u32HisgrmArray[1] + u32HisgrmArray[2]), 8);
            if(u32HisgrmArray[0] > 2 * u32HisgrmArray[1])
            {
                u32HisWeightCur = u32HisWeightCur + min((u32HisgrmArray[0] * u32MinRatio)/64, 8);
            }
        }
        else if (i == 15)
        {
            u32MinRatio = min(u32HisgrmArray[15] / (u32HisgrmArray[14] + u32HisgrmArray[13]), 8);
            if(u32HisgrmArray[15] > 2 * u32HisgrmArray[14])
            {
                u32HisWeightCur = u32HisWeightCur + min((u32HisgrmArray[15] * u32MinRatio) / 64, 8);
            }
        }
        else
        {
            u32MinRatio = min((u32HisgrmArray[i] / ((u32HisgrmArray[i + 1]) + (u32HisgrmArray[i - 1]))), 8);
            if((u32HisgrmArray[i] > 2 * u32HisgrmArray[i + 1]) && (u32HisgrmArray[i] > 2 * u32HisgrmArray[i - 1]))
            {
                u32HisWeightCur = u32HisWeightCur + min((u32HisgrmArray[i] * u32MinRatio) / 64, 8);
            }
        }
    }

    u32TempRatio1 = (u32BiggestHistogram / (u32SecondHistogram+u32ThirdHistogram + (u32NonzeroHistogramCnt + u32NonSmallHistogramCnt) / 3));
    u32TempRatio2 = ((u32BiggestHistogram + u32SecondHistogram)/(u32ThirdHistogram + u32NonzeroHistogramCnt + u32NonSmallHistogramCnt) );

    u32TempRatio1 = min(u32TempRatio1, 12);
    u32TempRatio2 = min(u32TempRatio2, 8);

    if(u32TempRatio1 <= 2)
    {
        u32TempRatio1 = 0;
    }
    if(u32TempRatio2 <= 3)
    {
        u32TempRatio2 = 0;
    }

    u32HisWeightCur = u32HisWeightCur + u32TempRatio1 + u32TempRatio2;

    if((u32BiggestHistogram + u32SecondHistogram + u32ThirdHistogram) * 18000 > u32PureColorBlkCnt * 128 * 4)
    {
        u32HisWeightCur = u32HisWeightCur / 3;
    }
    else if((u32BiggestHistogram + u32SecondHistogram + u32ThirdHistogram) * 18000 > u32PureColorBlkCnt * 128 * 3)
    {
       u32HisWeightCur = u32HisWeightCur / 2;
    }
    else if((u32BiggestHistogram + u32SecondHistogram + u32ThirdHistogram) * 18000 > u32PureColorBlkCnt * 128 * 2)
    {
       u32HisWeightCur = u32HisWeightCur * 2 / 3;
    }

    if(u32HisWeightCur <= 5)
    {
        u32HisWeightCur = 0;
    }

    u32HisWeightCur = min(u32HisWeightCur, 15);

    _u32HisWeight = (u32HisWeightCur * 1 + _u32HisWeight * 15 + u32DitherOut) / 16;

    _u32HisWeight     = min(_u32HisWeight    , 15);

    if(u32HighFreqBlkCnt >= 450 )
    {
        u32ArtWeightCurHF = 15;
    }
    else if(u32HighFreqBlkCnt > 300 )
    {
        u32ArtWeightCurHF = 8;
    }
    else
    {
        u32ArtWeightCurHF = 0;
    }

    // All weight
    u32ArtIndex = (u32PureColorBlkCnt / 4 * (min(u32HisWeightCur, 8) / 8)) + u32GradLevelBlkCnt;
    u32ArtWeightCurAll = 0;
    if(u32ArtIndex >= 3500)
    {
        u32ArtWeightCurAll = 15;
    }
    else if(u32ArtIndex <= 2600)
    {
        u32ArtWeightCurAll = 0;
    }
    else
    {
        u32ArtWeightCurAll = (u32ArtIndex - 2600) * 15 / 900;
    }

    u32ArtWeightCur =max(u32ArtWeightCurHF, u32ArtWeightCurAll);
    u32ArtWeightCur = min(u32ArtWeightCur, 15);
    _u32ArtWeight = (u32ArtWeightCur * 1 + _u32ArtWeight * 15 + u32DitherOut) / 16;
    _u32ArtWeight     = min(_u32ArtWeight, 15);
    _u32ReturnWeightCur = max(_u32HisWeight, _u32ArtWeight);
    _u32ReturnWeightCur = min(_u32ReturnWeightCur, 15);
    _u32ReturnWeight     = (_u32ReturnWeightCur + _u32ReturnWeight * 7 + (u32DitherOut >> 1)) / 8;

    return _u32ReturnWeight;
}

static void _MDrv_SC_3D_2DTo3D_DepthRefine(SC_INT_SRC eIntNum, void *pParam)
{
    MS_U32 i = 0, j = 0;
    MS_U32 u32DDBase = (MS_U32)MsOS_PA2KSEG1_1(_u32HW2DTO3D_DD_Buf);
    MS_U32 u32DRBase = (MS_U32)MsOS_PA2KSEG1_1(_u32HW2DTO3D_DR_Buf);
    MS_U8  u8DepthMode = 0;
    MS_U32 u32Base =  u32DDBase;
    MS_U32 u32ArtWt = 0;
    MS_U32 u32NatWt = 0;
    MS_U8 u8ArtWt_t = 0;
    MS_U8  u8YCtr_Cur = 0;
    static MS_U8 u8Ptr = 0;
    static MS_BOOL bFirstFlag = TRUE;

    eIntNum = eIntNum;
    pParam = pParam;

    SC_W2BYTEMSK(REG_SC_BK15_67_L, BIT(12),  BIT(12)); // DR force bank0
    SC_W2BYTEMSK(REG_SC_BK15_40_L, BIT(6),  BIT(6));   // DD force bank0

    if(bFirstFlag) //initial
    {
        for(j = 0; j < 2040; j++)
        {
            _32YSumArray[j] = 0;
        }

        for(i = 0; i < 256; i++)
        {
            for(j = 0; j < 2040; j++)
            {
                _u8YCtrArray[i][j] = 0;
            }
        }
    }

    for(i = 0; i < 34; i++)
    {
        for(j = 0; j < 60; j++)
        {
            _u8FieldArray[YMAX][i * 60 + j]  = *((volatile MS_U8*)(( i * 60 * 32 + j * 32 + 9) + u32Base));//y max
            _u8FieldArray[YMIN][i * 60 + j]  = *((volatile MS_U8*)(( i * 60 * 32 + j * 32 + 12) + u32Base));//y min
            _u8FieldArray[YCTRS][i * 60 + j]  = minmax ((_u8FieldArray[YMAX][i * 60 + j] - _u8FieldArray[YMIN][i * 60 + j]), 0, 255);
#if 0
            _u8FieldArray[YAVG][i * 60 + j]  = *((volatile MS_U8*)(( i * 60 * 32 + j * 32 + 15) + u32Base));//y avg
            _u8FieldArray[HEDGE][i * 60 + j]  = *((volatile MS_U8*)(( i * 60 * 32 + j * 32 + 28) + u32Base));//prog cnt 0
            _u8FieldArray[VEDGE][i * 60 + j]  = *((volatile MS_U8*)(( i * 60 * 32 + j * 32 + 29) + u32Base));//prog cnt 1
            _u8FieldArray[BUILDING][i * 60 + j]  = max(_u8FieldArray[HEDGE][i * 60 + j], _u8FieldArray[VEDGE][i * 60 + j]);
#endif
            if(_bIsHW2Dto3DPatchEnabled)
            {
                u8YCtr_Cur = _u8FieldArray[YCTRS][i * 60 + j];
                _32YSumArray[i * 60 + j] = _32YSumArray[i * 60 + j] - _u8YCtrArray[u8Ptr][i * 60 + j] + u8YCtr_Cur;
                _u8FieldArray[BLUE][i * 60 + j] = _32YSumArray[i * 60 + j] >> 8;
                _u8YCtrArray[u8Ptr][i * 60 + j]= u8YCtr_Cur;
            }
            else if(i == 0)
            {
                u8YCtr_Cur = _u8FieldArray[YCTRS][i * 60 + j];
                _32YSumArray[i * 60 + j] = _32YSumArray[i * 60 + j] - _u8YCtrArray[u8Ptr][i * 60 + j] + u8YCtr_Cur;
                _u8FieldArray[YCTRS][i * 60 + j] = _32YSumArray[i * 60 + j] >> 8;
                _u8YCtrArray[u8Ptr][i * 60 + j]= u8YCtr_Cur;
            }
        }
    }
    u8Ptr++;
    bFirstFlag = FALSE;

    if (_st3DHw2DTo3DPara.u16ArtificialGain != 0)
    {
        u8ArtWt_t         = (_st3DHw2DTo3DPara.u16ArtificialGain == 15) ? 16 : _st3DHw2DTo3DPara.u16ArtificialGain;
        u32ArtWt = _MDrv_SC_3D_2DTo3D_Art_Detect();
        u32NatWt    = 15 - min((u32ArtWt * u8ArtWt_t) >> 4, 15);
    }
    else
    {
        u32NatWt    = 15;
    }

    SC_W2BYTEMSK(REG_SC_BK15_65_L, _st3DHw2DTo3DPara.u16Offset, 0x00FF);//left depth offset
    SC_W2BYTEMSK(REG_SC_BK15_65_L, ((_st3DHw2DTo3DPara.u16Gain*u32NatWt)>>4)<<8, 0x3F00);//left depth gain
    SC_W2BYTEMSK(REG_SC_BK15_66_L, _st3DHw2DTo3DPara.u16Offset, 0x00FF);//right depth offset
    SC_W2BYTEMSK(REG_SC_BK15_66_L, ((_st3DHw2DTo3DPara.u16Gain*u32NatWt)>>4)<<8, 0x3F00);//right depth gain

    for(i = 0; i < 34; i++)
    {
        for(j = 0; j < 60; j++)
        {
            MS_U32 u32Result = 0;
            MS_U8  u8Res = 0;
            if(_bIsHW2Dto3DPatchEnabled)
            {
                u8Res = (_st3DHw2DTo3DPara.u16EleSel == 0) ? remap(YCTRS, 34, 60, i, j, 0x05) :
                        (_st3DHw2DTo3DPara.u16EleSel == 1) ? remap(YAVG,  34, 60, i, j, 0x05) :
                        (_st3DHw2DTo3DPara.u16EleSel == 2) ? remap(BLUE,  34, 60, i, j, 0x05) : 0;
            }
            else
            {
                _u8FieldArray[BLUE][i * 60 + j]    = lpf3x3(i,j,YCTRS);
                u8Res = (_st3DHw2DTo3DPara.u16EleSel == 0) ? _u8FieldArray[YCTRS][i*60 + j]:
                        (_st3DHw2DTo3DPara.u16EleSel == 1) ? _u8FieldArray[YAVG ][i*60 + j]:
                        (_st3DHw2DTo3DPara.u16EleSel == 2) ? _u8FieldArray[BLUE ][i*60 + j]: 0;
            }
            switch(u8DepthMode)
            {
                case 0:
                    u32Result = slope_gen(i, _st3DHw2DTo3DPara.u16Concave, u8Res, _st3DHw2DTo3DPara.u16ModSel, 1);// V gradual pattern test
                    break;
                case 1:
                    u32Result = 255 - (7 * i);// V gradual pattern test (inverse)
                    break;
                case 2:
                    u32Result = 4 * j;// H gradual pattern test
                    break;
                case 3:
                    u32Result = 255 - (4 * j);// H gradual pattern test (inverse)
                    break;
                case 4:
                    u32Result = (abs(i - 17) + abs(j - 30)) <<3;// center gradual pattern test
                    break;
                default:
                    u32Result = 7 * i;// V gradual pattern test
                    break;
            }
            u32Result = minmax(u32Result, 0, 255);
            u32Result = (j == 0 || j >= (((gSrcInfo[MAIN_WINDOW].stDispWin.width+31)/32)-1)) ? _st3DHw2DTo3DPara.u16Offset : u32Result;//left/rightest side no 2d3d effect
            (*((volatile MS_U8*)((i * 0x40 + j) + u32DRBase))) = u32Result;
        }
    }
    MsOS_FlushMemory();
}

#endif
//------------------------------------------------------------------------------
//--------HW 2D to 3D CPU Depth Refine Part End---------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------



//----------------------------------------
//--------3D external interface-----------
//----------------------------------------
//-------------------------------------------------------------------------------------------------
/// get 3D hw version
/// @return MS_U16                      \b OUT: 3d hw version
//-------------------------------------------------------------------------------------------------
MS_U16 MApi_XC_Get_3D_HW_Version()
{
// Note: HW_DESIGN_3D_VER
// 0: T3, Janus. non 3d chip, use opm, madi etc to make 3d
// 1: T12, T13, A1: it has some 3d type specific registers. It can send out LR flag by hw
//        Yet A1 has hw 2d-->3d, but we use HW_2DTO3D_SUPPORT to indicate it.
// 2: M10, J2: based on version 1, it has all the 3d type registers. input and output 3d registers are independent.
// 3: A6: based on version 2, it removes black space before reading into memory.
// 4: A3, A5, A5P: based on version3, main sub window can both use 1920+960 line buffer, so no H prescaling is needed.
//        It has FRC-inside feature also, but we use FRC_INSIDE to indicate this feature.
    return HW_DESIGN_3D_VER;
}

//-------------------------------------------------------------------------------------------------
/// Is Supported HW 2D To 3D
/// @return MS_BOOL                      \b OUT: TRUE: HW 2D to 3D FALSE: SW 2D to 3D
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_Get_3D_IsSupportedHW2DTo3D(void)
{
    return HW_2DTO3D_SUPPORT;
}

//-------------------------------------------------------------------------------------------------
/// set 3D convert format type
/// @param  e3dInputMode                \b IN: the input format
/// @param  e3dOutputMode               \b IN: the format we want to convert to
/// @param  e3dPanelType                \b IN: the panel type.
/// @param  eWindow                     \b IN: which window we are going to set
/// @return MS_BOOL                     \b OUT: TRUE:Success FALSE:Fail
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_Set_3D_Mode(E_XC_3D_INPUT_MODE e3dInputMode,
                            E_XC_3D_OUTPUT_MODE e3dOutputMode,
                            E_XC_3D_PANEL_TYPE e3dPanelType,
                            SCALER_WIN eWindow)
{
	MS_BOOL bRet = TRUE;
    MS_ASSERT(eWindow < MAX_WINDOW);

    if(eWindow >= MAX_WINDOW)
    {
        return FALSE;
    }

    if(eWindow == MAIN_WINDOW)
    {
        bRet = MApi_XC_Is3DFormatSupported(e3dInputMode, e3dOutputMode);
    }

    if(bRet == TRUE)
    {
        _eInput3DMode[eWindow] = e3dInputMode;
        _eOutput3DMode         = e3dOutputMode;
        _e3DPanelType          = e3dPanelType;
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW, "e3dInputMode=%u, e3dOutputMode=%u\n", e3dInputMode, e3dOutputMode);
    }
    else //if not supported, display as 2D
    {
        printf("MApi_XC_Set_3D_Mode(): invalid format, set to 3d_none\n");
        _XC_ENTRY();
		#ifdef MULTI_SCALER_SUPPORTED
		Hal_SC_subwindow_disable(eWindow);
		#else
        Hal_SC_subwindow_disable();
		#endif
        _XC_RETURN();
        _eInput3DMode[eWindow] = E_XC_3D_INPUT_MODE_NONE;
        gSrcInfo[eWindow].stDispWin.x = 0;
        gSrcInfo[eWindow].stDispWin.y = 0;
        gSrcInfo[eWindow].stDispWin.width = g_XC_InitData.stPanelInfo.u16Width;
        gSrcInfo[eWindow].stDispWin.height = g_XC_InitData.stPanelInfo.u16Height;
    }

#if (HW_DESIGN_3D_VER == 4)
    if(e3dOutputMode == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE)
    {
        if((e3dInputMode == E_XC_3D_INPUT_SIDE_BY_SIDE_HALF)
        ||(e3dInputMode == E_XC_3D_INPUT_CHECK_BORAD))
        {
#if 0
            if(!g_XC_Pnl_Misc.FRCInfo.bFRC)
                g_XC_Pnl_Misc.FRCInfo.u83D_FI_out = E_XC_3D_OUTPUT_FI_1920x540;
            else
                g_XC_Pnl_Misc.FRCInfo.u83D_FI_out = E_XC_3D_OUTPUT_FI_960x1080;
#else
#ifdef FA_1920X540_OUTPUT
            g_XC_Pnl_Misc.FRCInfo.u83D_FI_out = E_XC_3D_OUTPUT_FI_1920x540;
#else
            g_XC_Pnl_Misc.FRCInfo.u83D_FI_out = E_XC_3D_OUTPUT_FI_960x1080;
#endif
#endif
        }
        else if((e3dInputMode == E_XC_3D_INPUT_TOP_BOTTOM)
        ||(e3dInputMode == E_XC_3D_INPUT_SIDE_BY_SIDE_FULL)
        ||(e3dInputMode == E_XC_3D_INPUT_FIELD_ALTERNATIVE))
            g_XC_Pnl_Misc.FRCInfo.u83D_FI_out = E_XC_3D_OUTPUT_FI_1920x540;
        else
            g_XC_Pnl_Misc.FRCInfo.u83D_FI_out = E_XC_3D_OUTPUT_FI_1920x1080;
    }
    else
        g_XC_Pnl_Misc.FRCInfo.u83D_FI_out = E_XC_3D_OUTPUT_FI_MODE_NONE;

    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW, "u83D_FI_out=%x\n",g_XC_Pnl_Misc.FRCInfo.u83D_FI_out);
#endif
    return bRet;
}

//-------------------------------------------------------------------------------------------------
/// This API return 3D input format
/// @param  eWindow                \b IN: which window we are going to set
//-------------------------------------------------------------------------------------------------
E_XC_3D_INPUT_MODE MApi_XC_Get_3D_Input_Mode(SCALER_WIN eWindow)
{
    MS_ASSERT(eWindow < MAX_WINDOW);
    if(eWindow >= MAX_WINDOW)
    {
        return E_XC_3D_INPUT_MODE_NONE;
    }
    return _eInput3DMode[eWindow];
}

//-------------------------------------------------------------------------------------------------
/// This API return 3D output format
//-------------------------------------------------------------------------------------------------
E_XC_3D_OUTPUT_MODE MApi_XC_Get_3D_Output_Mode(void)
{
    return _eOutput3DMode;
}

//-------------------------------------------------------------------------------------------------
/// This API return 3D panel type
//-------------------------------------------------------------------------------------------------
E_XC_3D_PANEL_TYPE MApi_XC_Get_3D_Panel_Type(void)
{
    return _e3DPanelType;
}

//-------------------------------------------------------------------------------------------------
/// do Ip Sync to make main and sub window same relation to OP, thus framelock could be applied to
/// both main and sub window
/// @return MS_BOOL                \b OUT: TRUE:Success FALSE:Fail
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_3DMainSub_IPSync(void)
{
    _XC_ENTRY();
    HAL_SC_ip_3DMainSub_IPSync();
    _XC_RETURN();
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// When we do output: line_alter, we can decide the odd line coming from which part of the input
/// (left or right, up or down, first frame or second frame, etc)
/// @param  bMainFirst                \b IN: True: Main first; False: Sub first
/// @return MS_BOOL                   \b OUT: TRUE:Success FALSE:Fail
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_Set_3D_MainWin_FirstMode(MS_BOOL bMainFirst)
{
    //True: Main first; False: Sub first
    _b3DMainFirst = bMainFirst;
    SC_3D_DBG(printf("3D: exchange main-sub win sequence! (%u)\r\n", bMainFirst);)
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// the API return the relation that odd line is coming from which part of the input content.
/// @param  bMainFirst                \b IN: True: Main first; False: Sub first
/// @return MS_BOOL                   \b OUT: TRUE:Success FALSE:Fail
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_Get_3D_MainWin_First(void)
{
    return _b3DMainFirst;
}

//-------------------------------------------------------------------------------------------------
/// Some mm file maybe have unofficial format,
/// eg.
/// --------------------------
/// |     black_L            |
/// --------------------------
/// |                        |
/// |          L             |
/// |                        |
/// --------------------------
/// |                        |
/// |          R             |
/// |                        |
/// --------------------------
/// |      black+R           |
/// --------------------------
/// this format cannot direct convert to 3D output. Because black_L will cover with R
/// so we define the black area height, then handle it in 3D process.
/// But the height must be told by AP layer
/// @param  bMainFirst                \b IN: True: Main first; False: Sub first
/// @return MS_BOOL                   \b OUT: TRUE:Success FALSE:Fail
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_Set_3D_VerVideoOffset(MS_U16 u163DVerVideoOffset)
{
    printf("MApi_XC_Set_3D_VerVideoOffset:this function is obsolete\n");
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// This API return Top Bottom's black area height
//-------------------------------------------------------------------------------------------------
MS_U16 MApi_XC_Get_3D_VerVideoOffset(void)
{
    printf("MApi_XC_Get_3D_VerVideoOffset:this function is obsolete\n");
    return 0;
}

MS_BOOL MDrv_XC_Set_3D_LR_Frame_Exchg_burst(SCALER_WIN eWindow)
{
    MS_BOOL bRet = FALSE;
	MS_U16 u16IPM, u16OPM;
#if HW_2DTO3D_SUPPORT
	MS_U16 u16OutputFormat;
#endif

    MS_ASSERT(eWindow < MAX_WINDOW);

    if(eWindow >= MAX_WINDOW)
    {
        return FALSE;
    }

#if (HW_DESIGN_3D_VER < 2)

    if((IS_INPUT_FRAME_ALTERNATIVE(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
#if (HW_DESIGN_3D_VER == 0)
        || (IS_INPUT_FRAME_ALTERNATIVE(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF())
#endif
        )
    {
        MS_U32 u32Temp1 = 0, u32Temp2 = 0;
        //exchange the ipm base0 and ipm base1

        _MLOAD_ENTRY();
        u32Temp1 = Hal_SC_Get_DNRBase0(eWindow);
        u32Temp2 = Hal_SC_Get_DNRBase1(eWindow);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_08_L, (MS_U16)(u32Temp2>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_09_L, (MS_U16)(u32Temp2>>16), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_0A_L, (MS_U16)(u32Temp1>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_0B_L, (MS_U16)(u32Temp1>>16), 0xFFFF);
        MApi_XC_MLoad_Fire(TRUE);
        _MLOAD_RETURN();
        bRet = TRUE;
        SC_3D_DBG(printf("3D: exchange dnr base\n");)
    }
    else if(((IS_INPUT_TOP_BOTTOM(eWindow) || IS_INPUT_SIDE_BY_SIDE_HALF(eWindow)
                  || IS_INPUT_LINE_ALTERNATIVE(eWindow))
               && IS_OUTPUT_LINE_ALTERNATIVE())
             || (IS_INPUT_FRAME_PACKING(eWindow) && IS_OUTPUT_FRAME_ALTERNATIVE())
             || (IS_INPUT_FRAME_PACKING(eWindow) && IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC())
             || (IS_INPUT_FRAME_PACKING(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
             || (IS_INPUT_LINE_ALTERNATIVE(eWindow) && IS_OUTPUT_FRAME_ALTERNATIVE())
             || (IS_INPUT_LINE_ALTERNATIVE(eWindow) && IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC())
             || (IS_INPUT_TOP_BOTTOM(eWindow) && IS_OUTPUT_FRAME_ALTERNATIVE())
             || (IS_INPUT_TOP_BOTTOM(eWindow) && IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC())
             || (IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) && IS_OUTPUT_FRAME_ALTERNATIVE())
             || (IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) && IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC())
             || (IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow) && IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC())
             || (IS_INPUT_FIELD_ALTERNATIVE(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
             || (IS_INPUT_NORMAL_2D(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
             || (IS_INPUT_NORMAL_2D_INTERLACE(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
             || (IS_INPUT_NORMAL_2D_INTERLACE_PTP(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
             || (IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
             || (IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE_OPT(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
            )
    {
        MS_U32 u32Temp1 = 0, u32Temp2 = 0;
        //exchange the dnr base0 of main & sub

        _MLOAD_ENTRY();
        u32Temp1 = Hal_SC_Get_OPMBase0(eWindow);
        u32Temp2 = Hal_SC_Get_OPMBase1(eWindow);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_10_L, (MS_U16)(u32Temp2>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_11_L, (MS_U16)(u32Temp2>>16), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_12_L, (MS_U16)(u32Temp1>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_13_L, (MS_U16)(u32Temp1>>16), 0xFFFF);
        MApi_XC_MLoad_Fire(TRUE);
        _MLOAD_RETURN();
        bRet = TRUE;
        SC_3D_DBG(printf("3D: exchange opm base\n");)
    }
    else if(IS_INPUT_FRAME_PACKING_OPT(eWindow) && IS_OUTPUT_TOP_BOTTOM())
    {
        MS_U32 u32Temp1 = 0, u32Temp2 = 0;
        //exchange the dnr base0 of main & sub

        _MLOAD_ENTRY();
        u32Temp1 = Hal_SC_Get_DNRBase0(MAIN_WINDOW);
        u32Temp2 = Hal_SC_Get_DNRBase0(SUB_WINDOW);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_08_L, (MS_U16)(u32Temp2>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_09_L, (MS_U16)(u32Temp2>>16), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_48_L, (MS_U16)(u32Temp1>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_49_L, (MS_U16)(u32Temp1>>16), 0xFFFF);
        MApi_XC_MLoad_Fire(TRUE);
        _MLOAD_RETURN();
        bRet = TRUE;
        SC_3D_DBG(printf("3D: exchange main/sub dnr base0\n");)
    }
    else if(IS_OUTPUT_TOP_BOTTOM() || IS_OUTPUT_SIDE_BY_SIDE_HALF())
    {
        MS_U32 u32Temp1 = 0, u32Temp2 = 0;
        MS_U16 u16Temp = 0;
        //exchange the opm base0/base1 of main & sub

        _MLOAD_ENTRY();
        u32Temp1 = Hal_SC_Get_OPMBase0(MAIN_WINDOW);
        u32Temp2 = Hal_SC_Get_OPMBase0(SUB_WINDOW);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_10_L, (MS_U16)(u32Temp2>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_11_L, (MS_U16)(u32Temp2>>16), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_50_L, (MS_U16)(u32Temp1>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_51_L, (MS_U16)(u32Temp1>>16), 0xFFFF);

        u32Temp1 = Hal_SC_Get_OPMBase1(MAIN_WINDOW);
        u32Temp2 = Hal_SC_Get_OPMBase1(SUB_WINDOW);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_12_L, (MS_U16)(u32Temp2>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_13_L, (MS_U16)(u32Temp2>>16), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_52_L, (MS_U16)(u32Temp1>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_53_L, (MS_U16)(u32Temp1>>16), 0xFFFF);

        u32Temp1 = Hal_SC_Get_OPMBase2(MAIN_WINDOW);
        u32Temp2 = Hal_SC_Get_OPMBase2(SUB_WINDOW);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_14_L, (MS_U16)(u32Temp2>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_15_L, (MS_U16)(u32Temp2>>16), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_54_L, (MS_U16)(u32Temp1>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_55_L, (MS_U16)(u32Temp1>>16), 0xFFFF);

        u16Temp = Hal_SC_get_opm_fetch(MAIN_WINDOW);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_17_L, Hal_SC_get_opm_fetch(SUB_WINDOW), 0x0FFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_57_L, u16Temp, 0x0FFF);

        u16Temp = (Hal_SC_Get_LBOffset(MAIN_WINDOW)<<8)|(Hal_SC_Get_LBOffset(SUB_WINDOW));

        MApi_XC_MLoad_WriteCmd(REG_SC_BK20_1D_L, u16Temp , 0xFFFF);
        MApi_XC_MLoad_Fire(TRUE);
        _MLOAD_RETURN();
        bRet = TRUE;
    }
#elif (HW_DESIGN_3D_VER >= 2)
    if((IS_INPUT_NORMAL_2D(eWindow) || IS_INPUT_NORMAL_2D_INTERLACE(eWindow)) &&
        IS_OUTPUT_LINE_ALTERNATIVE())
    {
        if(_u163DHShift & 0xFF)
        {
            _u163DHShift <<= 8; //Switch shift to sub window
        }
        else
        {
            _u163DHShift >>= 8; //Switch shift to Main window
        }
        _MLOAD_ENTRY();
        MApi_XC_MLoad_WriteCmd(REG_SC_BK20_1C_L, _u163DHShift, 0xFFFF);//Main/sub display window offset in right direction
        MApi_XC_MLoad_Fire(TRUE);
        _MLOAD_RETURN();
        bRet = TRUE;
    }
    else if(!(IS_INPUT_MODE_NONE(eWindow) || IS_OUTPUT_MODE_NONE())
            && !(IS_INPUT_NORMAL_2D_HW(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE_HW())
            )
    {
        //MS_U16 u16IPM, u16OPM;
        u16IPM = SC_R2BYTE(REG_SC_BK12_30_L);
        u16OPM = SC_R2BYTE(REG_SC_BK12_33_L);
        if(u16OPM & BIT(7))
        {
            //Recommend to use OP L/R switch as designer's advice
            u16OPM = u16OPM^BIT(8);
            _MLOAD_ENTRY();
            MApi_XC_MLoad_WriteCmd(REG_SC_BK12_33_L, u16OPM, BIT(8));//F2 OPM 3D LR invert
            MApi_XC_MLoad_Fire(TRUE);
            _MLOAD_RETURN();
            bRet = TRUE;
        }
        else if(u16IPM & BIT(0))
        {
            u16IPM = u16IPM^BIT(4);
            _MLOAD_ENTRY();
            MApi_XC_MLoad_WriteCmd(REG_SC_BK12_30_L, u16IPM, BIT(4));//F2 IPM 3D LR invert
            MApi_XC_MLoad_Fire(TRUE);
            _MLOAD_RETURN();
            bRet = TRUE;
        }
        else
        {
            printf("----------------3D L/R switch: UnSupported format I: %u->O: %u\n", _eInput3DMode[eWindow], _eOutput3DMode);
        }
    }
#endif

#if HW_2DTO3D_SUPPORT
    if(IS_INPUT_NORMAL_2D_HW(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE_HW())
    {
        u16OutputFormat = SC_R2BYTEMSK(REG_SC_BK15_67_L, 0x0007);
        if(0x0001 == u16OutputFormat)
        {
            _MLOAD_ENTRY();
            MApi_XC_MLoad_WriteCmd(REG_SC_BK15_67_L, 0x0004, 0x0007);
            MApi_XC_MLoad_Fire(TRUE);
            _MLOAD_RETURN();
            _u16HW2Dto3DCurrentLR = 0x0004;
            bRet = TRUE;
        }
        else if(0x0004 == u16OutputFormat)
        {
            _MLOAD_ENTRY();
            MApi_XC_MLoad_WriteCmd(REG_SC_BK15_67_L, 0x0001, 0x0007);
            MApi_XC_MLoad_Fire(TRUE);
            _MLOAD_RETURN();
            _u16HW2Dto3DCurrentLR = 0x0001;
            bRet = TRUE;
        }
        else
        {
            printf("----------------3D L/R switch: UnSupported hw 2d to 3d output format\n");
        }
    }
#endif
    return bRet;
}

MS_BOOL MDrv_XC_Set_3D_LR_Frame_Exchg(SCALER_WIN eWindow)
{
    MS_BOOL bRet = FALSE;

    MS_ASSERT(eWindow < MAX_WINDOW);

    if(eWindow >= MAX_WINDOW)
    {
        return FALSE;
    }

#if (HW_DESIGN_3D_VER < 2)

    if((IS_INPUT_FRAME_ALTERNATIVE(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
#if (HW_DESIGN_3D_VER == 0)
        || (IS_INPUT_FRAME_ALTERNATIVE(eWindow) && IS_OUTPUT_SIDE_BY_SIDE_HALF())
#endif
        )
    {
        MS_U32 u32Temp = 0;
        //exchange the ipm base0 and ipm base1
        u32Temp = Hal_SC_Get_DNRBase0(eWindow) * BYTE_PER_WORD;
        Hal_SC_set_DNRBase0(Hal_SC_Get_DNRBase1(eWindow) * BYTE_PER_WORD, eWindow);
        Hal_SC_set_DNRBase1(u32Temp, eWindow);
        bRet = TRUE;
        SC_3D_DBG(printf("3D: exchange dnr base\n");)
    }
    else if(((IS_INPUT_TOP_BOTTOM(eWindow) || IS_INPUT_SIDE_BY_SIDE_HALF(eWindow)
                  || IS_INPUT_LINE_ALTERNATIVE(eWindow))
               && IS_OUTPUT_LINE_ALTERNATIVE())
             || (IS_INPUT_FRAME_PACKING(eWindow) && IS_OUTPUT_FRAME_ALTERNATIVE())
             || (IS_INPUT_FRAME_PACKING(eWindow) && IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC())
             || (IS_INPUT_FRAME_PACKING(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
             || (IS_INPUT_LINE_ALTERNATIVE(eWindow) && IS_OUTPUT_FRAME_ALTERNATIVE())
             || (IS_INPUT_LINE_ALTERNATIVE(eWindow) && IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC())
             || (IS_INPUT_TOP_BOTTOM(eWindow) && IS_OUTPUT_FRAME_ALTERNATIVE())
             || (IS_INPUT_TOP_BOTTOM(eWindow) && IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC())
             || (IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) && IS_OUTPUT_FRAME_ALTERNATIVE())
             || (IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) && IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC())
             || (IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow) && IS_OUTPUT_FRAME_ALTERNATIVE_NOFRC())
             || (IS_INPUT_FIELD_ALTERNATIVE(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
             || (IS_INPUT_NORMAL_2D(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
             || (IS_INPUT_NORMAL_2D_INTERLACE(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
             || (IS_INPUT_NORMAL_2D_INTERLACE_PTP(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
             || (IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
             || (IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE_OPT(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
            )
    {
        MS_U32 u32Temp = 0;
        u32Temp = Hal_SC_Get_OPMBase0(eWindow) * BYTE_PER_WORD;
        Hal_SC_set_OPMBase0(Hal_SC_Get_OPMBase1(eWindow) * BYTE_PER_WORD, eWindow);
        Hal_SC_set_OPMBase1(u32Temp, eWindow);
        bRet = TRUE;
        SC_3D_DBG(printf("3D: exchange opm base\n");)
    }
    else if(IS_INPUT_FRAME_PACKING_OPT(eWindow) && IS_OUTPUT_TOP_BOTTOM())
    {
        MS_U32 u32Temp = 0;
        //exchange the dnr base0 of main & sub
        u32Temp = Hal_SC_Get_DNRBase0(MAIN_WINDOW) * BYTE_PER_WORD;
        Hal_SC_set_DNRBase0(Hal_SC_Get_DNRBase0(SUB_WINDOW) * BYTE_PER_WORD, MAIN_WINDOW);
        Hal_SC_set_DNRBase0(u32Temp, SUB_WINDOW);
        bRet = TRUE;
        SC_3D_DBG(printf("3D: exchange main/sub dnr base0\n");)

    }
    else if(IS_OUTPUT_TOP_BOTTOM() || IS_OUTPUT_SIDE_BY_SIDE_HALF())
    {
        MS_U32 u32Temp = 0;
        //exchange the opm base0/base1 of main & sub
        u32Temp = Hal_SC_Get_OPMBase0(MAIN_WINDOW) * BYTE_PER_WORD;
        Hal_SC_set_OPMBase0(Hal_SC_Get_OPMBase0(SUB_WINDOW) * BYTE_PER_WORD, MAIN_WINDOW);
        Hal_SC_set_OPMBase0(u32Temp, SUB_WINDOW);

        u32Temp = Hal_SC_Get_OPMBase1(MAIN_WINDOW) * BYTE_PER_WORD;
        Hal_SC_set_OPMBase1(Hal_SC_Get_OPMBase1(SUB_WINDOW) * BYTE_PER_WORD, MAIN_WINDOW);
        Hal_SC_set_OPMBase1(u32Temp, SUB_WINDOW);

        u32Temp = Hal_SC_Get_OPMBase2(MAIN_WINDOW) * BYTE_PER_WORD;
        Hal_SC_set_OPMBase2(Hal_SC_Get_OPMBase2(SUB_WINDOW) * BYTE_PER_WORD, MAIN_WINDOW);
        Hal_SC_set_OPMBase2(u32Temp, SUB_WINDOW);

        u32Temp = Hal_SC_get_opm_fetch(MAIN_WINDOW);
        Hal_SC_set_opm_fetch(MAIN_WINDOW, Hal_SC_get_opm_fetch(SUB_WINDOW));
        Hal_SC_set_opm_fetch(SUB_WINDOW, u32Temp);

        u32Temp = Hal_SC_Get_LBOffset(MAIN_WINDOW);
        Hal_SC_Set_LBOffset(Hal_SC_Get_LBOffset(SUB_WINDOW), MAIN_WINDOW);
        Hal_SC_Set_LBOffset(u32Temp, SUB_WINDOW);
        bRet = TRUE;
        SC_3D_DBG(printf("3D: exchange main/sub opm base\n");)
    }
#elif (HW_DESIGN_3D_VER >= 2)
    if((IS_INPUT_NORMAL_2D(eWindow) || IS_INPUT_NORMAL_2D_INTERLACE(eWindow)) &&
        IS_OUTPUT_LINE_ALTERNATIVE())
    {
        if(_u163DHShift & 0xFF)
        {
            _u163DHShift <<= 8; //Switch shift to sub window
        }
        else
        {
            _u163DHShift >>= 8; //Switch shift to Main window
        }
        SC_W2BYTE(REG_SC_BK20_1C_L, _u163DHShift);//Main/sub display window offset in right direction
        bRet = TRUE;
    }
    else if(!(IS_INPUT_MODE_NONE(eWindow) || IS_OUTPUT_MODE_NONE())
            && !(IS_INPUT_NORMAL_2D_HW(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE_HW())
            )
    {
        MS_U16 u16IPM, u16OPM;
        u16IPM = SC_R2BYTE(REG_SC_BK12_30_L);
        u16OPM = SC_R2BYTE(REG_SC_BK12_33_L);
        if(u16OPM & BIT(7))
        {
            //Recommend to use OP L/R switch as designer's advice
            u16OPM = u16OPM^BIT(8);
            SC_W2BYTEMSK(REG_SC_BK12_33_L, u16OPM, BIT(8));//F2 OPM 3D LR invert
            bRet = TRUE;
        }
        else if(u16IPM & BIT(0))
        {
            u16IPM = u16IPM^BIT(4);
            SC_W2BYTEMSK(REG_SC_BK12_30_L, u16IPM, BIT(4));//F2 IPM 3D LR invert
            bRet = TRUE;
        }
        else
        {
            printf("----------------3D L/R switch: UnSupported format I: %u->O: %u\n", _eInput3DMode[eWindow], _eOutput3DMode);
        }
    }
#endif

#if HW_2DTO3D_SUPPORT
    if(IS_INPUT_NORMAL_2D_HW(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE_HW())
    {
        MS_U16 u16OutputFormat = SC_R2BYTEMSK(REG_SC_BK15_67_L, 0x0007);
        if(0x0001 == u16OutputFormat)
        {
            SC_W2BYTEMSK(REG_SC_BK15_67_L, 0x0004, 0x0007);
            _u16HW2Dto3DCurrentLR = 0x0004;
            bRet = TRUE;
        }
        else if(0x0004 == u16OutputFormat)
        {
            SC_W2BYTEMSK(REG_SC_BK15_67_L, 0x0001, 0x0007);
            _u16HW2Dto3DCurrentLR = 0x0001;
            bRet = TRUE;
        }
        else
        {
            printf("----------------3D L/R switch: UnSupported hw 2d to 3d output format\n");
        }
    }
#endif

    return bRet;
}

//-------------------------------------------------------------------------------------------------
/// When we do PIP 3D output, maybe users want to adjust the main and wub window for 3D effect.
/// We call this api to exchange once, and call it again to return to original status.
/// @param  eWindow                \b IN: which window we are going to set
/// @return MS_BOOL                \b OUT: TRUE:Success FALSE:Fail
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_Set_3D_LR_Frame_Exchg(SCALER_WIN eWindow)
{
    MS_BOOL bRet = FALSE;
    _XC_ENTRY();
    if (MApi_XC_MLoad_GetStatus(eWindow) == E_MLOAD_ENABLED)
    {
        bRet = MDrv_XC_Set_3D_LR_Frame_Exchg_burst(eWindow);
    }
    else
    {
        MDrv_XC_wait_output_vsync(2, 100, eWindow);
        bRet = MDrv_XC_Set_3D_LR_Frame_Exchg(eWindow);
    }
    _XC_RETURN();
    return bRet;
}

//------------------------------------------------------------------------------------------------------
/// Set 3D horizontal shift pixel
/// @param  u16HShift                \b IN: input  horizontal shift pixel
/// @return MS_BOOL                  \b OUT: TRUE: set is ok FALSE: set value is odd, need align to even
//------------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_Set_3D_HShift(MS_U16 u16HShift)
{
    MS_BOOL bRet;
#if HW_2DTO3D_SUPPORT
    bRet = TRUE;
    _XC_ENTRY();
    _u163DHShift = u16HShift&0xff;
    if((!IS_INPUT_MODE_NONE(MAIN_WINDOW)) && (!IS_OUTPUT_MODE_NONE())
       && (!IS_INPUT_NORMAL_2D_HW(MAIN_WINDOW))
       && (!IS_INPUT_NORMAL_2D(MAIN_WINDOW))) //real 3d is enabled
    {
        if((u16HShift == 0x80) || (u16HShift == 0x00))
        {
            SC_W2BYTEMSK(REG_SC_BK15_5D_L, 0x0000, 0x8000); //2  // 2nd offset enable
        }
        else if(u16HShift > 0x80)
        {
            SC_W2BYTEMSK(REG_SC_BK15_5D_L, 0x8000, 0x8000); //2  // 2nd offset enable
            SC_W2BYTE(REG_SC_BK15_5E_L, 0x0000); //2  // 2nd offset L
            SC_W2BYTE(REG_SC_BK15_5F_L, (u16HShift-0x80)<<8); //2  // 2nd offset R
        }
        else
        {
            SC_W2BYTEMSK(REG_SC_BK15_5D_L, 0x8000, 0x8000); //2  // 2nd offset enable
            SC_W2BYTE(REG_SC_BK15_5E_L, (0x80-u16HShift)<<8); //2  // 2nd offset L
            SC_W2BYTE(REG_SC_BK15_5F_L, 0x0000); //2  // 2nd offset R
        }
    }
    _XC_RETURN();
#elif (HW_DESIGN_3D_VER != 2)
    if(u16HShift & 0x01)
    {
        bRet = FALSE;
        _u163DHShift = u16HShift + 1;
        printf("horizontal shift pixel should be even: adjust to %u\n", _u163DHShift);
    }
    else
    {
        bRet = TRUE;
        _u163DHShift = u16HShift;
    }
#else
        bRet = TRUE;
        _u163DHShift = u16HShift & 0xFF; //Offset is 8 bit for M10
#endif
    return bRet;
}

//------------------------------------------------------------------------------------------------------
/// Set 3D reg_3dlr_side2line_en, for line_alternative full screen output
/// @param  u16HShift                \b IN: input  horizontal shift pixel
/// @return MS_BOOL                  \b OUT: TRUE: set is ok FALSE: set value is odd, need align to even
//------------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_Enable_3D_LR_Sbs2Line(MS_BOOL bEnable)
{
    MS_BOOL bRet = FALSE;
#if (HW_DESIGN_3D_VER == 1)
    _XC_ENTRY();
    _bIsLR_Sbs2Line = bEnable;
    SC_W2BYTEMSK(REG_SC_BK23_53_L, (bEnable << 15), 0x8000);//dnr offset
    bRet = TRUE;
    _XC_RETURN();
#endif
    return bRet;
}

//-------------------------------------------------------------------------------------------------
/// Get 3D horizontal shift pixel
/// @param  void
/// @return MS_U16                            \b OUT: need horizontal shift pixel value
//-------------------------------------------------------------------------------------------------
MS_U16 MApi_XC_Get_3D_HShift(void)
{
    return _u163DHShift;
}

//------------------------------------------------------------------------------------------------------
/// Set HW 2D to 3D para
/// @param  st3DHw2DTo3DPara          \b IN: parameters
/// @return MS_BOOL                   \b OUT: TRUE: set is ok FALSE: set value is wrong
//------------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_Set_3D_HW2DTo3D_Parameters(MS_XC_3D_HW2DTO3D_PARA st3DHw2DTo3DPara)
{
    MS_BOOL bRet = TRUE;
#if HW_2DTO3D_SUPPORT
    if(st3DHw2DTo3DPara.u16Concave > 15)
    {
        printf("u16Concave only have 4 bits\n");
        bRet = FALSE;
        _st3DHw2DTo3DPara.u16Concave = HW_2DTO3D_DEFAULT_CONCAVE;
    }
    else
    {
        _st3DHw2DTo3DPara.u16Concave = st3DHw2DTo3DPara.u16Concave;
    }

    if(st3DHw2DTo3DPara.u16ArtificialGain > 15)
    {
        printf("u16ArtificialGain only have 4 bits\n");
        bRet = FALSE;
        _st3DHw2DTo3DPara.u16ArtificialGain = HW_2DTO3D_DEFAULT_ARTIFICIALGAIN;
    }
    else
    {
        _st3DHw2DTo3DPara.u16ArtificialGain = st3DHw2DTo3DPara.u16ArtificialGain;
    }

    if(st3DHw2DTo3DPara.u16Gain > 63)
    {
        printf("u16Gain only have 6 bits\n");
        bRet = FALSE;
        _st3DHw2DTo3DPara.u16Gain = HW_2DTO3D_DEFAULT_GAIN;
    }
    else
    {
        _st3DHw2DTo3DPara.u16Gain = st3DHw2DTo3DPara.u16Gain;
    }

    if(st3DHw2DTo3DPara.u16Offset > 255)
    {
        printf("u16Offset only have 8 bits\n");
        bRet = FALSE;
        _st3DHw2DTo3DPara.u16Offset = HW_2DTO3D_DEFAULT_OFFSET;
    }
    else
    {
        _st3DHw2DTo3DPara.u16Offset = st3DHw2DTo3DPara.u16Offset;
    }

    if(st3DHw2DTo3DPara.u16EleSel > 3)
    {
        printf("u16Offset only have 2 bits\n");
        bRet = FALSE;
        _st3DHw2DTo3DPara.u16EleSel = HW_2DTO3D_DEFAULT_ELESEL;
    }
    else
    {
        _st3DHw2DTo3DPara.u16EleSel = st3DHw2DTo3DPara.u16EleSel;
    }

    if(st3DHw2DTo3DPara.u16ModSel > 3)
    {
        printf("u16Offset only have 2 bits\n");
        bRet = FALSE;
        _st3DHw2DTo3DPara.u16ModSel = HW_2DTO3D_DEFAULT_MODSEL;
    }
    else
    {
        _st3DHw2DTo3DPara.u16ModSel = st3DHw2DTo3DPara.u16ModSel;
    }
#else
    printf("this function is only supported by HW_2DTO3D_SUPPORT=TRUE\n");
    bRet = FALSE;
#endif
    return bRet;
}

//------------------------------------------------------------------------------------------------------
/// Get HW 2D to 3D para
/// @param  pst3DHw2DTo3DPara          \b IN: pointer to parameters
/// @return MS_BOOL                    \b OUT: TRUE: set is ok FALSE: set value is wrong
//------------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_Get_3D_HW2DTo3D_Parameters(MS_XC_3D_HW2DTO3D_PARA *pst3DHw2DTo3DPara)
{
    MS_BOOL bRet = TRUE;

    MS_ASSERT(pst3DHw2DTo3DPara);
    if(pst3DHw2DTo3DPara == NULL)
    {
        printf("3D: pst3DHw2DTo3DPara is null pointer!\r\n");
        return FALSE;
    }

#if HW_2DTO3D_SUPPORT
    memcpy(pst3DHw2DTo3DPara, &_st3DHw2DTo3DPara, sizeof(MS_XC_3D_HW2DTO3D_PARA));
    bRet = TRUE;
#else
    printf("this function is only supported by HW_2DTO3D_SUPPORT=TRUE\n");
    memset(pst3DHw2DTo3DPara, 0, sizeof(MS_XC_3D_HW2DTO3D_PARA));
    bRet = FALSE;
#endif
    return bRet;
}

//------------------------------------------------------------------------------------------------------
/// Set HW 2D to 3D dd buffer&dr buffer
/// @param  MS_PHYADDR                \b IN: dd buffer address
/// @param  MS_PHYADDR                \b IN: dr buffer address
/// @return MS_BOOL                   \b OUT: TRUE: set is ok FALSE: set value is wrong
//------------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_Set_3D_HW2DTo3D_Buffer(MS_PHYADDR u32HW2DTO3D_DD_Buf, MS_PHYADDR u32HW2DTO3D_DR_Buf)
{
#if HW_2DTO3D_SUPPORT
    _u32HW2DTO3D_DD_Buf = u32HW2DTO3D_DD_Buf;
    _u32HW2DTO3D_DR_Buf = u32HW2DTO3D_DR_Buf;
    return TRUE;
#else
    printf("this function is only supported by HW_2DTO3D_SUPPORT=TRUE\n");
    return FALSE;
#endif
}

//------------------------------------------------------------------------------------------------------
/// Set Color Registers after PQ
/// @param  eWindow                   \b IN: which window to do post pq setting
/// @return MS_BOOL                   \b OUT: TRUE: set is ok FALSE: set value is wrong
//------------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_3D_PostPQSetting(SCALER_WIN eWindow)
{
    MS_BOOL bRet = FALSE;

    MS_ASSERT(eWindow < MAX_WINDOW);
    if(eWindow >= MAX_WINDOW)
    {
        return FALSE;
    }
    _XC_ENTRY();
    bRet = MDrv_SC_3D_PostPQSetting(&gSrcInfo[eWindow], eWindow);
    _XC_RETURN();
    return bRet;
}

//------------------------------------------------------------------------------------------------------
/// Set Detect 3D Format Para
/// @param  pstDetect3DFormatPara       \b IN: pointer to parameters
/// @return MS_BOOL                     \b OUT: TRUE: set is ok FALSE: set value is wrong
//------------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_Set_3D_Detect3DFormat_Parameters(MS_XC_3D_DETECT3DFORMAT_PARA *pstDetect3DFormatPara)
{
    MS_ASSERT(pstDetect3DFormatPara);
    if(pstDetect3DFormatPara == NULL)
    {
        printf("3D: pstDetect3DFormatPara is null pointer!\r\n");
        return FALSE;
    }
    memcpy(&_stDetect3DFormatPara, pstDetect3DFormatPara, sizeof(MS_XC_3D_DETECT3DFORMAT_PARA));
    return TRUE;
}

//------------------------------------------------------------------------------------------------------
/// Get Detect 3D Format Para
/// @param  pstDetect3DFormatPara       \b IN: pointer to parameters
/// @return MS_BOOL                     \b OUT: TRUE: set is ok FALSE: set value is wrong
//------------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_Get_3D_Detect3DFormat_Parameters(MS_XC_3D_DETECT3DFORMAT_PARA *pstDetect3DFormatPara)
{
    MS_ASSERT(pstDetect3DFormatPara);
    if(pstDetect3DFormatPara == NULL)
    {
        printf("3D: pstDetect3DFormatPara is null pointer!\r\n");
        return FALSE;
    }
    memcpy(pstDetect3DFormatPara, &_stDetect3DFormatPara, sizeof(MS_XC_3D_DETECT3DFORMAT_PARA));
    return TRUE;
}

#if ( defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_NOS) )
#define	MAX_u16HorSearchRange	512
#define MAX_u16VerSearchRange	512
#endif

//------------------------------------------------------------------------------------------------------
/// Detect 3D Format By Content
/// @param  eWindow                   \b IN: which window view we are going to detect
/// @return E_XC_3D_INPUT_MODE        \b OUT: detected 3d format
//------------------------------------------------------------------------------------------------------
E_XC_3D_INPUT_MODE MApi_XC_Detect3DFormatByContent(SCALER_WIN eWindow)
{
    MS_PIXEL_24BIT stSource;
    MS_U16 u16SBSMatchedPixelCount = 0;
    MS_U16 u16TBMatchedPixelCount = 0;
#if ( defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_NOS) )
	MS_PIXEL_24BIT stDest[MAX_u16HorSearchRange*MAX_u16VerSearchRange];
#else
    MS_PIXEL_24BIT stDest[_stDetect3DFormatPara.u16HorSearchRange*_stDetect3DFormatPara.u16VerSearchRange];
#endif
    MS_WINDOW_TYPE stSrcRect;
    MS_WINDOW_TYPE stDestRect;
    MS_U16 i = 0;
    MS_U16 j = 0;
    MS_U16 k = 0;
    MS_U16 m = 0;
    MS_U16 u16Temp = 0;
    MS_U16 u16SBSMatchedFrameCount = 0;
    MS_U16 u16TBMatchedFrameCount = 0;
    MS_BOOL bSBSAllPixelMatched = FALSE;
    MS_BOOL bTBAllPixelMatched = FALSE;
    MS_BOOL bExit = FALSE;
    MS_U16 u16FrameCount = 0;
    E_XC_3D_INPUT_MODE e3DInput = E_XC_3D_INPUT_MODE_NONE;

    MS_ASSERT(eWindow < MAX_WINDOW);

    if(eWindow >= MAX_WINDOW)
    {
        return E_XC_3D_INPUT_MODE_NONE;
    }

    if(gSrcInfo[eWindow].bFBL || gSrcInfo[eWindow].bR_FBL)
    {
        printf("Attention: this function does not support fbl/rfbl case!\r\n");
        return E_XC_3D_INPUT_MODE_NONE;
    }

    memset(&stDest, 0 , sizeof(stDest));

    _XC_ENTRY();

    do
    {
        u16SBSMatchedPixelCount = 0;
        u16TBMatchedPixelCount = 0;
        MDrv_XC_wait_input_vsync(1, 100, eWindow);
        Hal_SC_set_freezeimg(ENABLE, eWindow);

        //sbs check
        for(j = 0; j < _stDetect3DFormatPara.u16VerSampleCount; j++)
        {
            for(i = 0; i < _stDetect3DFormatPara.u16HorSampleCount; i++)
            {
                stSrcRect.x = gSrcInfo[eWindow].u16H_SizeAfterPreScaling*(i+1)/2/(_stDetect3DFormatPara.u16HorSampleCount+1);
                stSrcRect.y = gSrcInfo[eWindow].u16V_SizeAfterPreScaling*(j+1)/(_stDetect3DFormatPara.u16VerSampleCount+1);
                stSrcRect.width = 1;
                stSrcRect.height = 1;

                stDestRect.x = stSrcRect.x + gSrcInfo[eWindow].u16H_SizeAfterPreScaling/2 - _stDetect3DFormatPara.u16HorSearchRange/2;
                stDestRect.y = stSrcRect.y;
                stDestRect.width = _stDetect3DFormatPara.u16HorSearchRange;
                stDestRect.height = _stDetect3DFormatPara.u16VerSearchRange;

                SC_3D_DBG(printf("====================SBS Attention! i=%u, j=%u=======================================================\r\n", i, j);)
                MApi_XC_Get_BufferData(E_XC_OUTPUTDATA_RGB8BITS, &stSrcRect, (MS_U8 *)&stSource, eWindow);
                SC_3D_DBG(printf("===================x=%u, y=%u==============================\r\n", stSrcRect.x, stSrcRect.y);)
                SC_3D_DBG(printf("y=%x, cb=%x, cr=%x\n", stSource.G_Y, stSource.B_Cb, stSource.R_Cr);)
                MApi_XC_Get_BufferData(E_XC_OUTPUTDATA_RGB8BITS, &stDestRect, (MS_U8 *)stDest, eWindow);
                bExit = FALSE;
                for(m = 0; (!bExit) && (m < _stDetect3DFormatPara.u16VerSearchRange); m++)
                {
                    for(k = 0; k < _stDetect3DFormatPara.u16HorSearchRange; k++)
                    {
                        SC_3D_DBG(printf("===================x=%u, y=%u==============================\r\n", stDestRect.x+k, stDestRect.y);)
                        SC_3D_DBG(printf("y=%x, cb=%x, cr=%x\n", stDest[i].G_Y, stDest[i].B_Cb, stDest[i].R_Cr);)
                        u16Temp = k + m * _stDetect3DFormatPara.u16HorSearchRange;
                        if((abs((MS_S16)stDest[u16Temp].G_Y-stSource.G_Y) < _stDetect3DFormatPara.u16GYPixelThreshold)    //find the matched pixel
                           && (abs((MS_S16)stDest[u16Temp].B_Cb-stSource.B_Cb) < _stDetect3DFormatPara.u16BCbPixelThreshold)
                           && (abs((MS_S16)stDest[u16Temp].R_Cr-stSource.R_Cr) < _stDetect3DFormatPara.u16RCrPixelThreshold)
                           )
                        {
                            SC_3D_DBG(printf("SBS hit: i=%u, j=%u\n", i, j);)
                            u16SBSMatchedPixelCount++;
                            bExit = TRUE;
                            break;
                        }
                    }
                }
            }
        }

        //tb check
        for(j = 0; j < _stDetect3DFormatPara.u16VerSampleCount; j++)
        {
            for(i = 0; i < _stDetect3DFormatPara.u16HorSampleCount; i++)
            {
                stSrcRect.x = gSrcInfo[eWindow].u16H_SizeAfterPreScaling*(i+1)/(_stDetect3DFormatPara.u16HorSampleCount+1);
                stSrcRect.y = gSrcInfo[eWindow].u16V_SizeAfterPreScaling*(j+1)/2/(_stDetect3DFormatPara.u16VerSampleCount+1);
                stSrcRect.width = 1;
                stSrcRect.height = 1;

                stDestRect.x = stSrcRect.x - _stDetect3DFormatPara.u16HorSearchRange/2;
                stDestRect.y = stSrcRect.y + gSrcInfo[eWindow].u16V_SizeAfterPreScaling/2;
                stDestRect.width = _stDetect3DFormatPara.u16HorSearchRange;
                stDestRect.height = _stDetect3DFormatPara.u16VerSearchRange;

                SC_3D_DBG(printf("====================TB Attention! i=%u, j=%u=======================================================\r\n", i, j);)
                MApi_XC_Get_BufferData(E_XC_OUTPUTDATA_RGB8BITS, &stSrcRect, (MS_U8 *)&stSource, eWindow);
                SC_3D_DBG(printf("===================x=%u, y=%u==============================\r\n", stSrcRect.x, stSrcRect.y);)
                SC_3D_DBG(printf("y=%x, cb=%x, cr=%x\n", stSource.G_Y, stSource.B_Cb, stSource.R_Cr);)
                MApi_XC_Get_BufferData(E_XC_OUTPUTDATA_RGB8BITS, &stDestRect, (MS_U8 *)stDest, eWindow);
                bExit = FALSE;
                for(m = 0; (!bExit) && (m < _stDetect3DFormatPara.u16VerSearchRange); m++)
                {
                    for(k = 0; k < _stDetect3DFormatPara.u16HorSearchRange; k++)
                    {
                        SC_3D_DBG(printf("===================x=%u, y=%u==============================\r\n", stDestRect.x+k, stDestRect.y);)
                        SC_3D_DBG(printf("y=%x, cb=%x, cr=%x\n", stDest[i].G_Y, stDest[i].B_Cb, stDest[i].R_Cr);)
                        u16Temp = k + m * _stDetect3DFormatPara.u16HorSearchRange;
                        if((abs((MS_S16)stDest[u16Temp].G_Y-stSource.G_Y) < _stDetect3DFormatPara.u16GYPixelThreshold)    //find the matched pixel
                           && (abs((MS_S16)stDest[u16Temp].B_Cb-stSource.B_Cb) < _stDetect3DFormatPara.u16BCbPixelThreshold)
                           && (abs((MS_S16)stDest[u16Temp].R_Cr-stSource.R_Cr) < _stDetect3DFormatPara.u16RCrPixelThreshold)
                           )
                        {
                            SC_3D_DBG(printf("TB hit: i=%u, j=%u\n", i, j);)
                            u16TBMatchedPixelCount++;
                            bExit = TRUE;
                            break;
                        }
                    }
                }
            }
        }

        Hal_SC_set_freezeimg(DISABLE, eWindow);

        if((u16SBSMatchedPixelCount == _stDetect3DFormatPara.u16HorSampleCount*_stDetect3DFormatPara.u16VerSampleCount)
        ^(u16TBMatchedPixelCount == _stDetect3DFormatPara.u16HorSampleCount*_stDetect3DFormatPara.u16VerSampleCount))
        {
            bTBAllPixelMatched = (u16TBMatchedPixelCount == _stDetect3DFormatPara.u16HorSampleCount*_stDetect3DFormatPara.u16VerSampleCount);
            bSBSAllPixelMatched = (u16SBSMatchedPixelCount == _stDetect3DFormatPara.u16HorSampleCount*_stDetect3DFormatPara.u16VerSampleCount);
        }

        if(u16SBSMatchedPixelCount >= (_stDetect3DFormatPara.u16HorSampleCount*_stDetect3DFormatPara.u16VerSampleCount*_stDetect3DFormatPara.u16HitPixelPercentage/100))
        {
            u16SBSMatchedFrameCount++;
        }

        if(u16TBMatchedPixelCount >= (_stDetect3DFormatPara.u16HorSampleCount*_stDetect3DFormatPara.u16VerSampleCount*_stDetect3DFormatPara.u16HitPixelPercentage/100))
        {
            u16TBMatchedFrameCount++;
        }

        u16FrameCount++;

        SC_3D_DBG(printf("u16SBSMatchedPixelCount=%u, u16TBMatchedPixelCount=%u\n", u16SBSMatchedPixelCount, u16TBMatchedPixelCount);)
    }while((!bTBAllPixelMatched) && (!bSBSAllPixelMatched) && (u16SBSMatchedFrameCount == u16TBMatchedFrameCount)
                       && (u16SBSMatchedFrameCount != 0) && (u16FrameCount < _stDetect3DFormatPara.u16MaxCheckingFrameCount));

    _XC_RETURN();

    if((u16SBSMatchedFrameCount > u16TBMatchedFrameCount) || bSBSAllPixelMatched)
    {
        e3DInput = E_XC_3D_INPUT_SIDE_BY_SIDE_HALF;
    }
    else if((u16TBMatchedFrameCount > u16SBSMatchedFrameCount) || bTBAllPixelMatched)
    {
        e3DInput = E_XC_3D_INPUT_TOP_BOTTOM;
    }
    else
    {
        e3DInput = E_XC_3D_INPUT_MODE_NONE;
    }
    return e3DInput;
}

//-------------------------------------------------------------------------------------------------
/// Query the input 3D format is supported or not
/// @param  E_XC_3D_INPUT_MODE                \b IN: input  3d format
/// @param  E_XC_3D_INPUT_MODE                \b IN: output 3d format
/// @return MS_BOOL                           \b OUT: TRUE:Supported FALSE:NotSupported
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_Is3DFormatSupported(E_XC_3D_INPUT_MODE e3dInputMode,
                                    E_XC_3D_OUTPUT_MODE e3dOutputMode)
{
    MS_BOOL bRet = FALSE;

    SC_3D_DBG(printf("==============================================================\r\n");)
    SC_3D_DBG(printf("3D:this 3D format is e3dInputMode:0x%x;e3dOutputMode:%u\n",
                     e3dInputMode, e3dOutputMode);)
    SC_3D_DBG(printf("==============================================================\r\n");)

    if(((e3dInputMode == E_XC_3D_INPUT_MODE_NONE) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_MODE_NONE))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_FRAME_PACKING) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_FRAME_PACKING) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE_NOFRC))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_FRAME_PACKING) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_LINE_ALTERNATIVE))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_FRAME_PACKING) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_TOP_BOTTOM))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_FRAME_PACKING) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_FRAME_PACKING) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_FRAME_L))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_FRAME_PACKING) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_FRAME_R))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_FRAME_PACKING_OPT) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_TOP_BOTTOM))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_FRAME_PACKING_OPT) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_LINE_ALTERNATIVE) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_LINE_ALTERNATIVE) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE_NOFRC))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_LINE_ALTERNATIVE) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_FRAME_L))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_LINE_ALTERNATIVE) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_LINE_ALTERNATIVE))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_LINE_ALTERNATIVE) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_TOP_BOTTOM))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_LINE_ALTERNATIVE) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_TOP_BOTTOM) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE_NOFRC))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_TOP_BOTTOM) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_LINE_ALTERNATIVE))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_TOP_BOTTOM) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_TOP_BOTTOM))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_TOP_BOTTOM) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_FRAME_L))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_TOP_BOTTOM) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_FRAME_R))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_TOP_BOTTOM) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF))
        ||
#if (HW_DESIGN_3D_VER == 4)
        ((e3dInputMode == E_XC_3D_INPUT_TOP_BOTTOM) &&
         (e3dOutputMode == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE))
        ||
#endif
        ((e3dInputMode == E_XC_3D_INPUT_TOP_BOTTOM_OPT) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_SIDE_BY_SIDE_HALF) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_SIDE_BY_SIDE_HALF) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE_NOFRC))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_SIDE_BY_SIDE_HALF) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_LINE_ALTERNATIVE))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_SIDE_BY_SIDE_HALF) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_SIDE_BY_SIDE_HALF) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_FRAME_L))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_SIDE_BY_SIDE_HALF) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_FRAME_R))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_SIDE_BY_SIDE_HALF) &&
         (e3dOutputMode == E_XC_3D_OUTPUT_TOP_BOTTOM))
        ||
#if (HW_DESIGN_3D_VER == 4)
        ((e3dInputMode == E_XC_3D_INPUT_SIDE_BY_SIDE_FULL) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_SIDE_BY_SIDE_FULL) &&
         (e3dOutputMode == E_XC_3D_OUTPUT_TOP_BOTTOM))
        ||
#endif
        ((e3dInputMode == E_XC_3D_INPUT_SIDE_BY_SIDE_HALF_INTERLACE_OPT) &&
         (e3dOutputMode == E_XC_3D_OUTPUT_LINE_ALTERNATIVE))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_SIDE_BY_SIDE_HALF_INTERLACE) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_LINE_ALTERNATIVE))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_SIDE_BY_SIDE_HALF_INTERLACE) &&
         (e3dOutputMode == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE_NOFRC))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_FRAME_ALTERNATIVE) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_LINE_ALTERNATIVE))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_FRAME_ALTERNATIVE) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_TOP_BOTTOM))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_FRAME_ALTERNATIVE) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_FRAME_ALTERNATIVE) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_FRAME_L))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_FRAME_ALTERNATIVE) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_FRAME_R))
        ||
#if (HW_DESIGN_3D_VER == 4)
        ((e3dInputMode == E_XC_3D_INPUT_FRAME_ALTERNATIVE) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE))
        ||
#endif
        ((e3dInputMode == E_XC_3D_INPUT_FIELD_ALTERNATIVE) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_LINE_ALTERNATIVE))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_FIELD_ALTERNATIVE) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_TOP_BOTTOM))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_FIELD_ALTERNATIVE) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_FIELD_ALTERNATIVE) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE))
        ||
#if (HW_DESIGN_3D_VER == 4)
        ((e3dInputMode == E_XC_3D_INPUT_CHECK_BORAD) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_CHECK_BORAD) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_TOP_BOTTOM))
        ||
#endif
        ((e3dInputMode == E_XC_3D_INPUT_NORMAL_2D) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_NORMAL_2D) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_TOP_BOTTOM))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_NORMAL_2D) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE_NOFRC))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_NORMAL_2D) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_LINE_ALTERNATIVE))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_NORMAL_2D_INTERLACE) &&
         (e3dOutputMode == E_XC_3D_OUTPUT_LINE_ALTERNATIVE))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_NORMAL_2D_INTERLACE_PTP) &&
         (e3dOutputMode == E_XC_3D_OUTPUT_LINE_ALTERNATIVE))
#if (HW_2DTO3D_SUPPORT || (HW_DESIGN_3D_VER == 4))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_NORMAL_2D_HW) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_LINE_ALTERNATIVE_HW))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_NORMAL_2D_HW) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_PIXEL_ALTERNATIVE_HW))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_NORMAL_2D_HW) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_CHECKBOARD_HW))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_NORMAL_2D_HW) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_FRAME_L_HW))
        ||
        ((e3dInputMode == E_XC_3D_INPUT_NORMAL_2D_HW) &&
        (e3dOutputMode == E_XC_3D_OUTPUT_FRAME_R_HW))
#endif

    )
    {
        bRet = TRUE;
    }
    else
    {
        bRet = FALSE;
        SC_3D_DBG(printf("3D:this 3D format is not supported!e3dInputMode:%u;e3dOutputMode:%u\n",
                         e3dInputMode, e3dOutputMode);)
    }
    return bRet;
}

//by Aiken, we don't add this kind of special customer request in trunk code
#if 1
//--------------------------------------------------
//--------3D external Custom 3D interface-----------
//--------------------------------------------------
//-------------------------------------------------------------------------------------------------
/// Disable Custom 3D API
/// @param  void
/// @return MS_BOOL                           \b OUT: TRUE:Successful FALSE:FAIL
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_DisableCus3D(void)
{
    memset(&_stCus3DProcess, 0, sizeof(Cus_3DProcess));
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Get Custom 3D Mode
/// @param  eChannelType                      \b IN: which channel
/// @return CUS3DMODE                         \b OUT: Custom 3D Mode
//-------------------------------------------------------------------------------------------------
CUS3DMODE MApi_XC_GetCus3DMode(SCALER_WIN eChannelType)
{
    CUS3DMODE enCus3DMode = E_CUS3D_NO;
    MS_ASSERT(eChannelType < MAX_WINDOW);
    if(eChannelType >= MAX_WINDOW)
    {
        return E_CUS3D_NO;
    }

    if(_stCus3DProcess.bDeMainPipSignal_Ver)
    {
        enCus3DMode = E_CUS3D_DEINTERLACESIGNAL_VER;
    }
    else if(_stCus3DProcess.bDeMainPipSignal)
    {
        enCus3DMode = E_CUS3D_DEINTERLACESIGNAL;
    }
    else if(_stCus3DProcess.eSyncType == MAIN_PIP_OUTPUT_ASYNC)
    {
        enCus3DMode = E_CUS3D_OUTPUTASYNC;
    }
    else if(_stCus3DProcess.bSetCusCropWindow[eChannelType])
    {
        enCus3DMode = E_CUS3D_NORMAL;
    }
    return enCus3DMode;
}

//-------------------------------------------------------------------------------------------------
/// Set Custom 3D Crop Window, MAIN_SOURCE_CHANNEL is L, PIP_SOURCE_CHANNEL is R, and we will process
/// 3D based on L/R memory
/// @param  u16Upper                          \b IN: upper location in memory
/// @param  u16Bottom                         \b IN: bottom location in memory
/// @param  u16Left                           \b IN: left location in memory
/// @param  u16Right                          \b IN: right location in memory
/// @param  eChannelType                      \b IN: which channel
/// @return MS_BOOL                           \b OUT: TRUE:Successful FALSE:FAIL
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_Crop_MemoryWindow(MS_U16 u16Upper,
                                  MS_U16 u16Bottom,
                                  MS_U16 u16Left,
                                  MS_U16 u16Right,
                                  SOURCE_CHANNEL_TYPE eChannelType)
{
    MS_BOOL bRet = TRUE;
    MS_ASSERT(eChannelType < MAX_CHANNEL);

    if(eChannelType >= MAX_CHANNEL)
    {
        return FALSE;
    }

    if((u16Bottom <= u16Upper) || (u16Right <= u16Left))
    {
        MS_ASSERT(0);
        _stCus3DProcess.bSetCusCropWindow[eChannelType]         = FALSE;
        bRet = FALSE;
    }
    else
    {
        _stCus3DProcess.bSetCusCropWindow[eChannelType]         = TRUE;
        _stCus3DProcess.stCus3DCropWindow[eChannelType].x       = u16Left;
        _stCus3DProcess.stCus3DCropWindow[eChannelType].width   = u16Right - u16Left + 1;
        _stCus3DProcess.stCus3DCropWindow[eChannelType].y       = u16Upper;
        _stCus3DProcess.stCus3DCropWindow[eChannelType].height  = u16Bottom - u16Upper + 1;
    }
    return bRet;
}

//-------------------------------------------------------------------------------------------------
/// Set Custom 3D Display Window, which will be displayed for 3D signal
/// @param  u16XPos                            \b IN: x pos on panel
/// @param  u16YPos                            \b IN: y pos on panel
/// @param  u16Width                           \b IN: width on panel
/// @param  u16Height                          \b IN: height on panel
/// @param  eChannelType                       \b IN: which channel
/// @return MS_BOOL                            \b OUT: TRUE:Successful FALSE:FAIL
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_SetSize_DisplayWindow(MS_U16 u16XPos,
                                     MS_U16 u16YPos,
                                     MS_U16 u16Width,
                                     MS_U16 u16Height,
                                     SOURCE_CHANNEL_TYPE eChannelType)
{
    MS_BOOL bRet = TRUE;
    MS_ASSERT(eChannelType < MAX_CHANNEL);

    if(eChannelType >= MAX_CHANNEL)
    {
        return FALSE;
    }

    if((u16Width == 0) || (u16Height == 0))
    {
        MS_ASSERT(0);
        _stCus3DProcess.bSetCusDispWindow[eChannelType]        = FALSE;
        bRet = FALSE;
    }
    else
    {
        _stCus3DProcess.bSetCusDispWindow[eChannelType]        = TRUE;
        _stCus3DProcess.stCus3DDispWindow[eChannelType].x      = u16XPos;
        _stCus3DProcess.stCus3DDispWindow[eChannelType].y      = u16YPos;
        _stCus3DProcess.stCus3DDispWindow[eChannelType].width  = u16Width;
        _stCus3DProcess.stCus3DDispWindow[eChannelType].height = u16Height;
    }
    return bRet;
}

//-------------------------------------------------------------------------------------------------
/// Deinterlace Custom 3D Main PIP Signal
/// 1st line is 1st line of MAIN_SOURCE_CHANNEL, 2nd line is 1st line of PIP_SOURCE_CHANNEL
/// 3rd line is 3rd line of MAIN_SOURCE_CHANNEL, 4th line is 3rd line of PIP_SOURCE_CHANNEL
/// and so on
/// @param  bMainFirst                         \b IN: Main Channel first or Sub Channel
/// @return MS_BOOL                            \b OUT: TRUE:Successful FALSE:FAIL
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_Deinterlace_MainPipSignal(MS_BOOL bMainFirst)
{
    _stCus3DProcess.bDeMainPipSignal = TRUE;
    _stCus3DProcess.bMainFirst       = bMainFirst;
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Deinterlace Custom 3D Main PIP Signal and need prescaling for Vertical Lines
/// if L or R > DispWin/2, need prescaling to DispWin/2 firstly
/// 1st line is 1st line of MAIN_SOURCE_CHANNEL, 2nd line is 1st line of PIP_SOURCE_CHANNEL
/// 3rd line is 2nd line of MAIN_SOURCE_CHANNEL, 4th line is 2nd line of PIP_SOURCE_CHANNEL
/// and so on
/// @param  bMainFirst                         \b IN: Main Channel first or Sub Channel
/// @return MS_BOOL                            \b OUT: TRUE:Successful FALSE:FAIL
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_VerDeinterlace_MainPipSignal(MS_BOOL bMainFirst)
{
    _stCus3DProcess.bDeMainPipSignal_Ver = TRUE;
    _stCus3DProcess.bMainFirst_Ver       = bMainFirst;
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Set Custom 3D Frequency and SyncType
/// @param  eFreq                              \b IN: output vfreq to panel
/// @param  eSync                              \b IN: async or sync
/// @return MS_BOOL                            \b OUT: TRUE:Successful FALSE:FAIL
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_SetFrequency_DisplayWindow(FREQUENCY_TO_PANEL eFreq,
                                           MAIN_PIP_OUTPUT_TYPE eSync)
{
    _stCus3DProcess.bSetVFreq     = TRUE;
    _stCus3DProcess.eOutputVFreq = eFreq;
    _stCus3DProcess.eSyncType    = eSync;
    return TRUE;
}

#endif

void MDrv_SC_3D_GetFP_Info(XC_InternalStatus *pSrcInfo, MS_U16 *pu16VactVideo, MS_U16 *pu16VactSpace, MS_BOOL bPreVscalingEna)
{
    if(pSrcInfo->stCapWin.height == DOUBLEHD_1080X2P_VSIZE)
    {
        *pu16VactVideo = DOUBLEHD_1080X2P_FRAME_VSIZE;
        *pu16VactSpace = DOUBLEHD_1080X2P_GARBAGE_VSIZE;
    }
    else if(pSrcInfo->stCapWin.height == DOUBLEHD_720X2P_VSIZE)
    {
        *pu16VactVideo = DOUBLEHD_720X2P_FRAME_VSIZE;
        *pu16VactSpace = DOUBLEHD_720X2P_GARBAGE_VSIZE;
    }
    else if(pSrcInfo->stCapWin.height == DOUBLEHD_1080X2I_VSIZE)
    {
        *pu16VactVideo = DOUBLEHD_1080X2I_FIELD_VSIZE;
        *pu16VactSpace = (DOUBLEHD_1080X2I_VACT_SPACE1*2+DOUBLEHD_1080X2I_VACT_SPACE2);
    }
    else if(pSrcInfo->stCapWin.height == FIELD_INTERLEAVE_1080X2I_VISZE)
    {
        *pu16VactVideo = FIELD_INTERLEAVE_1080X2I_FIELD_VISZE;
        *pu16VactSpace = FIELD_INTERLEAVE_1080X2I_VACT_SPACE;
    }
    else
    {
        printf("MDrv_SC_3D_GetFP_Info: it's not regular framepacking!\r\n");
        *pu16VactVideo = 0;
        *pu16VactSpace = 0;
    }
//Because the u16VactSpace is took off first at BK2 <New design!!>
#if ((HW_DESIGN_3D_VER < 3) || (HW_DESIGN_3D_VER == 4))
    if(bPreVscalingEna)
    {
        MS_U8 u8Ratio;
        if(pSrcInfo->stCapWin.height > pSrcInfo->u16V_SizeAfterPreScaling)//Avoid divide overflow
        {
            u8Ratio = pSrcInfo->stCapWin.height/pSrcInfo->u16V_SizeAfterPreScaling;
            *pu16VactVideo /= u8Ratio;
            *pu16VactSpace /= u8Ratio;
        }
    }
#endif
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW, "FP_Info: bPreVEna=%u, VactVideo=%u, VactSpace=%u, Vsrc=%u, Vdst=%u\n",
                 bPreVscalingEna, *pu16VactVideo, *pu16VactSpace, pSrcInfo->stCapWin.height, pSrcInfo->u16V_SizeAfterPreScaling);
}

#if (HW_DESIGN_3D_VER >= 2)
// This function is used to calculate exact pre V scaling dst, to let u16VactSpace can be totally divided
void MDrv_SC_3D_Adjust_FP_PreVerDstSize(E_XC_3D_INPUT_MODE eInputMode,
                                    E_XC_3D_OUTPUT_MODE eOutputMode,
                                    XC_InternalStatus *pSrcInfo,
                                    MS_U16 *pu16ScaleDst,
                                    SCALER_WIN eWindow)
{
    #define MAX_ACCEPTABLE_VRATIO 5 //Notes: 10 means scaling down to 1/5 of Vsrc
    MS_U16 u16VactVideo, u16VactSpace;

    MDrv_SC_3D_GetFP_Info(pSrcInfo, &u16VactVideo, &u16VactSpace, FALSE);//Get the original frame packing V act info
//A5: the u16VactSpace is took off first at BK2 <New design!!>
#if (HW_DESIGN_3D_VER > 3)

    if((g_XC_Pnl_Misc.FRCInfo.bFRC)&&(pSrcInfo->stCapWin.height == DOUBLEHD_720X2P_VSIZE)&&
       (eOutputMode == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE))
        g_XC_Pnl_Misc.FRCInfo.u83D_FI_out = E_XC_3D_OUTPUT_FI_1920x540;

    if(pSrcInfo->Status2.bPreVCusScaling == FALSE)
    {
        if(IS_OUTPUT_TOP_BOTTOM() || IS_OUTPUT_LINE_ALTERNATIVE() || IS_OUTPUT_FRAME_L() || IS_OUTPUT_FRAME_R())
        {
            *pu16ScaleDst = pSrcInfo->stDispWin.height/2;
        }
        else if(IS_OUTPUT_SIDE_BY_SIDE_HALF())
        {
            *pu16ScaleDst = pSrcInfo->stDispWin.height;
        }
        else if(IS_OUTPUT_FRAME_ALTERNATIVE())
        {
            if((g_XC_Pnl_Misc.FRCInfo.bFRC)&&(pSrcInfo->stCapWin.height == DOUBLEHD_720X2P_VSIZE))
            {
                //FPI input, no prescaling down
                *pu16ScaleDst = pSrcInfo->stDispWin.height/2;  // target for output 1920x540

            }
            else
            {
                *pu16ScaleDst = pSrcInfo->stDispWin.height;
            }
        }
        else
        {
            //Default disable prescaling
            *pu16ScaleDst = pSrcInfo->stCapWin.height-u16VactSpace;
        }
        *pu16ScaleDst = *pu16ScaleDst & (~0x01);
    }
    else
    {
        *pu16ScaleDst = pSrcInfo->Status2.u16PreVCusScalingDst & (~0x01);
    }
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW, "Adjust_FP_PreVerDstSize:bPreVCusScaling=%s, dst=%u, eOutputMode=%u\n",
                 (pSrcInfo->Status2.bPreVCusScaling?"Yes":"No"),*pu16ScaleDst, eOutputMode);
#elif (HW_DESIGN_3D_VER < 3)
    MS_U8 i = 0;

    if(pSrcInfo->stCapWin.height == DOUBLEHD_1080X2I_VSIZE)
    {
        //FPI input, no prescaling down
        *pu16ScaleDst = pSrcInfo->stCapWin.height;
        return;
    }
    else if(pSrcInfo->Status2.bPreVCusScaling == TRUE)
    {
        //check if can use AP assigned pre V Customer scaling
        if((pSrcInfo->Status2.u16PreVCusScalingSrc % pSrcInfo->Status2.u16PreVCusScalingDst) == 0)
        {
            i = pSrcInfo->Status2.u16PreVCusScalingSrc/pSrcInfo->Status2.u16PreVCusScalingDst;
            if((u16VactSpace%i) == 0)
            {
                //Vact space can be divided with no remainder, so this customer pre V scaling is ok.
                return;
            }
        }
    }

    for(i=1; i<=MAX_ACCEPTABLE_VRATIO; i++)
    {
        if((u16VactSpace%i) != 0)
        {
            // The u16VactSpace must be divisible by the prescaling ratio
            continue;
        }
        if(IS_OUTPUT_TOP_BOTTOM() || IS_OUTPUT_LINE_ALTERNATIVE() || IS_OUTPUT_FRAME_L() || IS_OUTPUT_FRAME_R())
        {
            if((pSrcInfo->stCapWin.height-u16VactSpace)/i <= pSrcInfo->stDispWin.height)
            {
                *pu16ScaleDst = pSrcInfo->stCapWin.height/i;
                 break;
            }
            else
            {
                continue;
            }
        }
        else if(IS_OUTPUT_SIDE_BY_SIDE_HALF())
        {
            if((pSrcInfo->stCapWin.height-u16VactSpace)/2/i <= pSrcInfo->stDispWin.height)
            {
                *pu16ScaleDst = pSrcInfo->stCapWin.height/i;
                break;
            }
            else
            {
                continue;
            }
        }
        else if(IS_OUTPUT_FRAME_ALTERNATIVE())
        {
            if((pSrcInfo->stCapWin.height-u16VactSpace)/2/i <= pSrcInfo->stDispWin.height)
            {
                *pu16ScaleDst = pSrcInfo->stCapWin.height/i;
                break;
            }
            else
            {
                continue;
            }
        }
        else
        {
            //Default disable prescaling
            *pu16ScaleDst = pSrcInfo->stCapWin.height;
            break;
        }
    }
    if(i > MAX_ACCEPTABLE_VRATIO)
    {
        *pu16ScaleDst = pSrcInfo->stCapWin.height; //Disable prescaling
    }
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW, "Adjust_FP_PreVerDstSize:i=%u, dst=%u, eOutputMode=%u\n",
                 i, *pu16ScaleDst, eOutputMode);
#else
    if(pSrcInfo->Status2.bPreVCusScaling == FALSE)
    {
        if(IS_OUTPUT_TOP_BOTTOM() || IS_OUTPUT_LINE_ALTERNATIVE() || IS_OUTPUT_FRAME_L() || IS_OUTPUT_FRAME_R())
        {
            *pu16ScaleDst = pSrcInfo->stDispWin.height;
        }
        else if(IS_OUTPUT_SIDE_BY_SIDE_HALF() || (IS_OUTPUT_FRAME_ALTERNATIVE()))
        {
            *pu16ScaleDst = pSrcInfo->stDispWin.height<<1;
        }
        else
        {
            //Default disable prescaling
            *pu16ScaleDst = pSrcInfo->stCapWin.height-u16VactSpace;
        }
        *pu16ScaleDst = *pu16ScaleDst & (~0x01);
    }
    else if(pSrcInfo->Status2.u16PreVCusScalingDst & BIT(0))
    {
        *pu16ScaleDst = pSrcInfo->Status2.u16PreVCusScalingDst & (~0x01);
    }
#endif
}

void MDrv_SC_3D_Adjust_PreScaling(E_XC_3D_INPUT_MODE eInputMode,
                                    E_XC_3D_OUTPUT_MODE eOutputMode,
                                    XC_InternalStatus *pSrcInfo,
                                    SCALER_WIN eWindow)
{
    MS_BOOL b3DPreVScaling=FALSE, b3DPreHScaling=FALSE;
    MS_U32 u32H_PreScalingRatio, u32V_PreScalingRatio, u32TempWidth;

#if (HW_DESIGN_3D_VER != 4)
    if(IS_OUTPUT_LINE_ALTERNATIVE() || IS_OUTPUT_FRAME_L() || IS_OUTPUT_FRAME_R())
     {
         //for best quality, don't do prescaling when not necessary
         if(pSrcInfo->Status2.bPreVCusScaling == FALSE)
         {
             b3DPreVScaling=TRUE;
             pSrcInfo->Status2.u16PreVCusScalingSrc = pSrcInfo->stCapWin.height;
             pSrcInfo->Status2.u16PreVCusScalingDst = pSrcInfo->stCapWin.height;
         }

         if(pSrcInfo->Status2.bPreHCusScaling == FALSE)
         {
             b3DPreHScaling=TRUE;
             pSrcInfo->Status2.u16PreHCusScalingSrc = pSrcInfo->stCapWin.width;
             pSrcInfo->Status2.u16PreHCusScalingDst = pSrcInfo->stCapWin.width;
         }
     }
#endif
     if(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) ||  IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow))
     {
         if(IS_OUTPUT_TOP_BOTTOM())
         {
             if(pSrcInfo->Status2.bPreVCusScaling == FALSE)
             {
                 b3DPreVScaling=TRUE;
                 pSrcInfo->Status2.u16PreVCusScalingSrc = pSrcInfo->stCapWin.height;
                 pSrcInfo->Status2.u16PreVCusScalingDst = pSrcInfo->stDispWin.height/2;
             }
         }
#ifdef FA_1920X540_OUTPUT
         else if(IS_OUTPUT_FRAME_ALTERNATIVE()&&(!g_XC_Pnl_Misc.FRCInfo.bFRC))
         {
             b3DPreHScaling=TRUE;
             pSrcInfo->Status2.u16PreHCusScalingSrc = pSrcInfo->stCapWin.width;
             pSrcInfo->Status2.u16PreHCusScalingSrc = pSrcInfo->stCapWin.width;


             b3DPreVScaling=TRUE;
             pSrcInfo->Status2.u16PreVCusScalingSrc = pSrcInfo->stCapWin.height;
             pSrcInfo->Status2.u16PreVCusScalingDst = pSrcInfo->stCapWin.height/2;
         }
#endif
     }
     else if(IS_INPUT_SIDE_BY_SIDE_FULL(eWindow))
     {

         b3DPreHScaling=TRUE;
         pSrcInfo->Status2.u16PreHCusScalingSrc = pSrcInfo->stCapWin.width/2;
         pSrcInfo->Status2.u16PreHCusScalingDst = pSrcInfo->Status2.u16PreHCusScalingSrc;

         b3DPreVScaling=TRUE;
         pSrcInfo->Status2.u16PreVCusScalingDst = pSrcInfo->stDispWin.height/2;
     }
     else if(IS_INPUT_FRAME_PACKING(eWindow))
     {
         MS_U16 u16VactVideo, u16VactSpace, u16ScaleDst = 0xFFFF;
         MDrv_SC_3D_GetFP_Info(pSrcInfo, &u16VactVideo, &u16VactSpace, FALSE);//Get the original frame packing V act info
         MDrv_SC_3D_Adjust_FP_PreVerDstSize(eInputMode, eOutputMode, pSrcInfo, &u16ScaleDst, eWindow);//calculate exact pre V scaling dst, to let u16VactSpace can be totally divided

         if(pSrcInfo->Status2.bPreHCusScaling == FALSE)
         {
             if(IS_OUTPUT_SIDE_BY_SIDE_HALF() && (pSrcInfo->Status2.u16PreHCusScalingDst > pSrcInfo->stDispWin.width/2))
             {
                 b3DPreHScaling=TRUE;
                 pSrcInfo->Status2.u16PreHCusScalingSrc = pSrcInfo->stCapWin.width;
                 pSrcInfo->Status2.u16PreHCusScalingDst = pSrcInfo->stDispWin.width/2;
             }
         }

     if(u16ScaleDst != 0xFFFF)
     {

#if (HW_DESIGN_3D_VER > 3)
             if(/*(g_XC_Pnl_Misc.FRCInfo.bFRC)&&*/
                     (pSrcInfo->stCapWin.height == DOUBLEHD_720X2P_VSIZE)&&
                     (eOutputMode == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE))
         {
             b3DPreVScaling=TRUE;
             pSrcInfo->Status2.u16PreVCusScalingSrc = u16VactVideo; //pSrcInfo->stCapWin.height - u16VactSpace;
             pSrcInfo->Status2.u16PreVCusScalingDst = u16ScaleDst;
         }
         else if((pSrcInfo->stCapWin.height == DOUBLEHD_720X2P_VSIZE)&&
                     (eOutputMode == E_XC_3D_OUTPUT_TOP_BOTTOM))
         {
             b3DPreVScaling=TRUE;
             pSrcInfo->Status2.u16PreVCusScalingSrc = u16VactVideo; //pSrcInfo->stCapWin.height - u16VactSpace;
             pSrcInfo->Status2.u16PreVCusScalingDst = pSrcInfo->stDispWin.width;
         }
         else if((pSrcInfo->stCapWin.height == DOUBLEHD_720X2P_VSIZE)&&
                     (eOutputMode == E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF))
         {
             b3DPreVScaling=TRUE;
             pSrcInfo->Status2.u16PreVCusScalingSrc = u16VactVideo; //pSrcInfo->stCapWin.height - u16VactSpace;
             pSrcInfo->Status2.u16PreVCusScalingDst = pSrcInfo->stDispWin.width;
         }
#ifdef TBP_1920X2160_OUTPUT
         else if((pSrcInfo->stCapWin.height == DOUBLEHD_1080X2P_VSIZE)&&
                     (eOutputMode == E_XC_3D_OUTPUT_TOP_BOTTOM))
         {
             b3DPreVScaling=TRUE;
             pSrcInfo->Status2.u16PreVCusScalingSrc = pSrcInfo->stCapWin.height - u16VactSpace;
             pSrcInfo->Status2.u16PreVCusScalingDst = pSrcInfo->stCapWin.height - u16VactSpace;
         }
         else if((pSrcInfo->stCapWin.height == DOUBLEHD_1080X2I_VSIZE)&&
                     (eOutputMode == E_XC_3D_OUTPUT_TOP_BOTTOM))
         {
             b3DPreVScaling=TRUE;
             pSrcInfo->Status2.u16PreVCusScalingSrc = (pSrcInfo->stCapWin.height - u16VactSpace);
             pSrcInfo->Status2.u16PreVCusScalingDst = (pSrcInfo->stCapWin.height - u16VactSpace);
         }
         else
         {
             b3DPreVScaling=TRUE;
             //pSrcInfo->Status2.u16PreVCusScalingSrc = u16ScaleDst;
             pSrcInfo->Status2.u16PreVCusScalingSrc = pSrcInfo->stCapWin.height - u16VactSpace;
         }
#else
         else
         {
             b3DPreVScaling=TRUE;
             //pSrcInfo->Status2.u16PreVCusScalingSrc = u16ScaleDst;
             pSrcInfo->Status2.u16PreVCusScalingSrc = pSrcInfo->stCapWin.height - u16VactSpace;
         }
#endif
#elif (HW_DESIGN_3D_VER > 2)
             b3DPreVScaling=TRUE;

             pSrcInfo->Status2.u16PreVCusScalingSrc = pSrcInfo->stCapWin.height - u16VactSpace;
             //don't do v prescaling
             pSrcInfo->Status2.u16PreVCusScalingDst = pSrcInfo->Status2.u16PreVCusScalingSrc;
#else
             b3DPreVScaling=TRUE;

             pSrcInfo->Status2.u16PreVCusScalingSrc = pSrcInfo->stCapWin.height;
             //don't do v prescaling
             pSrcInfo->Status2.u16PreVCusScalingDst = pSrcInfo->Status2.u16PreVCusScalingSrc;
#endif

         }
     }
     else if(IS_INPUT_TOP_BOTTOM(eWindow) || IS_INPUT_LINE_ALTERNATIVE(eWindow))
     {
         if(pSrcInfo->Status2.bPreHCusScaling == FALSE)
         {
             if(IS_OUTPUT_SIDE_BY_SIDE_HALF())
             {
                 b3DPreHScaling=TRUE;
                 pSrcInfo->Status2.u16PreHCusScalingSrc = pSrcInfo->stCapWin.width;
                 pSrcInfo->Status2.u16PreHCusScalingDst = pSrcInfo->stDispWin.width/2;
             }
         }
         // for better quality src don't use pr scaling down, but post scaling
         if(IS_OUTPUT_FRAME_ALTERNATIVE()&&IS_INPUT_LINE_ALTERNATIVE(eWindow))
         {
             b3DPreVScaling=TRUE;
             pSrcInfo->Status2.u16PreVCusScalingDst = pSrcInfo->Status2.u16PreVCusScalingSrc;
         }
     }
     else if(IS_INPUT_FRAME_ALTERNATIVE(eWindow))
     {
         if(pSrcInfo->Status2.bPreHCusScaling == FALSE)
         {
             if(IS_OUTPUT_SIDE_BY_SIDE_HALF())
             {
                 b3DPreHScaling=TRUE;
                 pSrcInfo->Status2.u16PreHCusScalingSrc = pSrcInfo->stCapWin.width;
                 pSrcInfo->Status2.u16PreHCusScalingDst = pSrcInfo->stDispWin.width/2;
             }
             else if(IS_OUTPUT_TOP_BOTTOM())
             {
                 b3DPreVScaling=TRUE;
                 pSrcInfo->Status2.u16PreVCusScalingSrc = pSrcInfo->stCapWin.height;
                 if(pSrcInfo->stCapWin.height <= pSrcInfo->stDispWin.height/2)
                 {
                     pSrcInfo->Status2.u16PreVCusScalingDst = pSrcInfo->stCapWin.height;
                 }
                 else
                 {
                     pSrcInfo->Status2.u16PreVCusScalingDst = pSrcInfo->stDispWin.height/2;
                 }
             }
         }
     }
#if (HW_DESIGN_3D_VER >= 4)
     else if(IS_INPUT_FIELD_ALTERNATIVE(eWindow))
     {
         MS_U16 u16VactVideo, u16VactSpace;
         MDrv_SC_3D_GetFP_Info(pSrcInfo, &u16VactVideo, &u16VactSpace, FALSE);//Get the original frame packing V act info

         if(IS_OUTPUT_FRAME_ALTERNATIVE())
         {
             b3DPreVScaling=TRUE;
             pSrcInfo->Status2.u16PreVCusScalingSrc = pSrcInfo->stCapWin.height - u16VactSpace;
             pSrcInfo->Status2.u16PreVCusScalingDst = pSrcInfo->stDispWin.height;
         }
         else if(IS_OUTPUT_TOP_BOTTOM())
         {
             b3DPreVScaling=TRUE;
             pSrcInfo->Status2.u16PreVCusScalingSrc = (pSrcInfo->stCapWin.height - u16VactSpace)/2;
             pSrcInfo->Status2.u16PreVCusScalingDst = pSrcInfo->stDispWin.height/2;
         }
     }
     else if(IS_INPUT_CHECK_BOARD(eWindow))
     {
         if(IS_OUTPUT_TOP_BOTTOM())
         {
             b3DPreHScaling=TRUE;
             pSrcInfo->Status2.u16PreHCusScalingSrc = pSrcInfo->stCapWin.width;
             pSrcInfo->Status2.u16PreHCusScalingDst = pSrcInfo->stDispWin.width;

             b3DPreVScaling=TRUE;
             pSrcInfo->Status2.u16PreVCusScalingSrc = pSrcInfo->stCapWin.height;
             pSrcInfo->Status2.u16PreVCusScalingDst = pSrcInfo->stDispWin.height/2;
         }
     }
#endif
    else if(IS_INPUT_NORMAL_2D(eWindow))
    {
        b3DPreVScaling=TRUE;
        pSrcInfo->Status2.u16PreVCusScalingSrc = pSrcInfo->stCapWin.height;
        if(pSrcInfo->stCapWin.height <= pSrcInfo->stDispWin.height/2)
        {
            pSrcInfo->Status2.u16PreVCusScalingDst = pSrcInfo->stCapWin.height;
        }
        else
        {
            pSrcInfo->Status2.u16PreVCusScalingDst = pSrcInfo->stDispWin.height/2;
        }
    }

    if(MDrv_SC_GetPQHSDFlag(eWindow) == TRUE)
    {
        b3DPreHScaling = FALSE;
        pSrcInfo->Status2.u16PreHCusScalingSrc = pSrcInfo->stCapWin.width;
        pSrcInfo->Status2.u16PreHCusScalingDst = pSrcInfo->u16H_SizeAfterPreScaling;
    }

    //protect HCusscaling size according to the line buffer
    if(!(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow)
       || IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow)
       || IS_INPUT_NORMAL_2D_HW(eWindow)
       || IS_OUTPUT_FRAME_ALTERNATIVE()
#if (HW_DESIGN_3D_VER >= 4)
       || IS_OUTPUT_TOP_BOTTOM()
#endif
       || MDrv_SC_3D_Is2TapModeSupportedFormat(pSrcInfo, eWindow)
       )
       ||(IS_INPUT_LINE_ALTERNATIVE(eWindow)&&IS_OUTPUT_TOP_BOTTOM()))
    {
        //These foramts will use subwindow, need check if the linebuff is enough
        //SCALER_LINE_BUFFER_MAX is DI line buffer max
        if(pSrcInfo->Status2.u16PreHCusScalingDst > (SCALER_LINE_BUFFER_MAX + SUB_SCALER_LINE_BUFFER_MAX)/2)
        {
            b3DPreHScaling=TRUE;
            pSrcInfo->Status2.u16PreHCusScalingSrc = pSrcInfo->stCapWin.width;
            pSrcInfo->Status2.u16PreHCusScalingDst = (SCALER_LINE_BUFFER_MAX + SUB_SCALER_LINE_BUFFER_MAX)/2;
            XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW, "For output line buffer limit => H :PreHScaling=%s, src=%u, dst=%u\n",
                         (b3DPreHScaling?"Yes":"No"), pSrcInfo->Status2.u16PreHCusScalingSrc, pSrcInfo->Status2.u16PreHCusScalingDst);

        }
    }

    if(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) ||  IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow))
    {
       //For SBS input, should make sure the Hsize/2 is even to avoid the color mismatch issue in 422 mode
        if(pSrcInfo->Status2.u16PreHCusScalingDst & (BIT(1)))
        {
            b3DPreVScaling=TRUE;
            pSrcInfo->Status2.u16PreHCusScalingDst = (pSrcInfo->Status2.u16PreHCusScalingDst>>2)<<2;
        }
    }

    if(IS_INPUT_TOP_BOTTOM(eWindow) || IS_INPUT_LINE_ALTERNATIVE(eWindow))
    {
        //For TPB/LAP input, should make sure the Vsize is even
        if(pSrcInfo->Status2.u16PreVCusScalingDst & BIT(0))
        {
            b3DPreVScaling=TRUE;
            pSrcInfo->Status2.u16PreVCusScalingDst = pSrcInfo->Status2.u16PreVCusScalingDst & ~0x01;
        }
    }

    //Below will check: if FB is enough or not
    if(IS_INPUT_FRAME_ALTERNATIVE(eWindow) &&
       (!pSrcInfo->bInterlace))
    {
#if (HW_DESIGN_3D_VER > 2)
        u32TempWidth = MApi_XC_GetAvailableSize(MAIN_WINDOW,
                                                2,
                                                pSrcInfo->Status2.u16PreVCusScalingDst);
        // for input fi, memory store double piece data
		if((MS_U32)(pSrcInfo->Status2.u16PreHCusScalingDst*2) > u32TempWidth)
        {
            b3DPreHScaling=TRUE;
            pSrcInfo->Status2.u16PreHCusScalingSrc = pSrcInfo->stCapWin.width;
            pSrcInfo->Status2.u16PreHCusScalingDst = (MS_U16)((u32TempWidth/2) & ~(OFFSET_PIXEL_ALIGNMENT - 1));
        }
#else
        u32TempWidth = MApi_XC_GetAvailableSize(MAIN_WINDOW,
                                                   4 /* 4 frame mode */,
                                                   pSrcInfo->Status2.u16PreVCusScalingDst);
        // for memory reason, need support 4 frame mode
        if(pSrcInfo->Status2.u16PreHCusScalingDst > u32TempWidth)
        {
            b3DPreHScaling=TRUE;
            pSrcInfo->Status2.u16PreHCusScalingSrc = pSrcInfo->stCapWin.width;
            pSrcInfo->Status2.u16PreHCusScalingDst = u32TempWidth & ~(OFFSET_PIXEL_ALIGNMENT - 1);
        }
#endif
    }
    else
    {
        u32TempWidth = MApi_XC_GetAvailableSize(MAIN_WINDOW,
                                                2,
                                                pSrcInfo->Status2.u16PreVCusScalingDst);
        //Check if memory is enough for at least 2 frame mode
        if(pSrcInfo->Status2.u16PreHCusScalingDst > u32TempWidth)
        {
            b3DPreHScaling=TRUE;
            pSrcInfo->Status2.u16PreHCusScalingSrc = pSrcInfo->stCapWin.width;
            pSrcInfo->Status2.u16PreHCusScalingDst = (MS_U16)(u32TempWidth & ~(OFFSET_PIXEL_ALIGNMENT -1));
        }
    }
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW, "MDrv_SC_3D_Adjust_PreScaling for H :PreHScaling=%s, src=%u, dst=%u\n",
                 (b3DPreHScaling?"Yes":"No"), pSrcInfo->Status2.u16PreHCusScalingSrc, pSrcInfo->Status2.u16PreHCusScalingDst);

    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW, "MDrv_SC_3D_Adjust_PreScaling for V :PreVScaling=%s, src=%u, dst=%u\n",
                 (b3DPreVScaling?"Yes":"No"), pSrcInfo->Status2.u16PreVCusScalingSrc, pSrcInfo->Status2.u16PreVCusScalingDst);

    if(eWindow == MAIN_WINDOW)
    {
        if(b3DPreVScaling || b3DPreHScaling)
        {
            if(b3DPreHScaling && MDrv_SC_GetPQHSDFlag(eWindow))
            {
                MDrv_SC_SetPQHSDFlag(FALSE, eWindow);//Scaling is changed, ignore PQ setting
            }
            pSrcInfo->Status2.bPreHCusScaling = TRUE;//If enable, then need Enable customer scaling both
            pSrcInfo->Status2.bPreVCusScaling = TRUE;
            MDrv_SC_set_prescaling_ratio(pSrcInfo->enInputSourceType, pSrcInfo, eWindow);
            _XC_ENTRY();
            if(!pSrcInfo->bPreV_ScalingDown)
            {
                Hal_SC_set_ficlk( FALSE, eWindow );
            }
            u32H_PreScalingRatio = SC_R4BYTE(REG_SC_BK02_04_L) & 0x40000000;
            u32H_PreScalingRatio |= (stDBreg.u32H_PreScalingRatio & ~0x40000000);
            u32V_PreScalingRatio = SC_R4BYTE(REG_SC_BK02_08_L) & 0x40000000;
            u32V_PreScalingRatio |= (stDBreg.u32V_PreScalingRatio & ~0x40000000);
            if ( pSrcInfo->bPreV_ScalingDown )
            {
                Hal_SC_set_ficlk( TRUE, eWindow );
            }
            SC_W4BYTE(REG_SC_BK02_04_L, u32H_PreScalingRatio);   // H pre-scaling
            SC_W4BYTE(REG_SC_BK02_08_L, u32V_PreScalingRatio);   // V pre-scaling
            _XC_RETURN();
        }
    }
}

void MDrv_SC_3D_Adjust_PstScaling(E_XC_3D_INPUT_MODE eInputMode,
                                    E_XC_3D_OUTPUT_MODE eOutputMode,
                                    XC_InternalStatus *pSrcInfo,
                                    SCALER_WIN eWindow)
{
    MS_U16 u16VactVideo=0, u16VactSpace=0;
    MS_BOOL b3DPstVScaling=FALSE, b3DPstHScaling=FALSE;
    if(IS_INPUT_FRAME_PACKING(eWindow))
    {
        MDrv_SC_3D_GetFP_Info(pSrcInfo, &u16VactVideo, &u16VactSpace, (MS_BOOL)((stDBreg.u32V_PreScalingRatio & BIT(31))>>31));
    }

    if(IS_OUTPUT_LINE_ALTERNATIVE() || IS_OUTPUT_TOP_BOTTOM() ||
       IS_OUTPUT_FRAME_L() || IS_OUTPUT_FRAME_R())
    {
        if(IS_INPUT_FRAME_PACKING(eWindow))
        {
            b3DPstVScaling=TRUE;
            b3DPstHScaling=TRUE;

            if(pSrcInfo->stCapWin.height == DOUBLEHD_1080X2I_VSIZE)
            {
                pSrcInfo->u16VCusScalingSrc = (pSrcInfo->ScaledCropWin.height-u16VactSpace)/4;
            }
            else
            {
                pSrcInfo->u16VCusScalingSrc = (pSrcInfo->ScaledCropWin.height-u16VactSpace)/2;
            }
            pSrcInfo->u16VCusScalingDst = pSrcInfo->stDispWin.height/2;
#if (HW_DESIGN_3D_VER >= 3)
#ifdef TBP_1920X2160_OUTPUT
            if(pSrcInfo->stCapWin.height == DOUBLEHD_1080X2P_VSIZE)
            {
                pSrcInfo->u16VCusScalingSrc = u16VactVideo;//pSrcInfo->ScaledCropWin.height;
                pSrcInfo->u16VCusScalingDst = pSrcInfo->stDispWin.height;
            }
            else if((pSrcInfo->stCapWin.height == DOUBLEHD_1080X2I_VSIZE)&&
                    (eOutputMode == E_XC_3D_OUTPUT_TOP_BOTTOM))
            {
                pSrcInfo->u16VCusScalingSrc = (pSrcInfo->ScaledCropWin.height)/2;;
                pSrcInfo->u16VCusScalingDst = pSrcInfo->stDispWin.height;
            }
#else
            if((pSrcInfo->stCapWin.height == DOUBLEHD_1080X2I_VSIZE)&&
                    (eOutputMode == E_XC_3D_OUTPUT_TOP_BOTTOM))
            {
                pSrcInfo->u16VCusScalingSrc = (pSrcInfo->ScaledCropWin.height)/2;
                pSrcInfo->u16VCusScalingDst = pSrcInfo->stDispWin.height/2;
            }
#endif
#endif

            SC_3D_DBG(printf("##[%s, %d]u16V_SizeAfterPreScaling_%d, u16VCusScalingSrc=%d\n", __FUNCTION__, __LINE__, pSrcInfo->u16V_SizeAfterPreScaling, pSrcInfo->u16VCusScalingSrc));
        }
        else if(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) || IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow))
        {
            b3DPstHScaling=TRUE;
            pSrcInfo->u16HCusScalingSrc = pSrcInfo->ScaledCropWin.width/2;
            pSrcInfo->u16HCusScalingDst = pSrcInfo->stDispWin.width;

            b3DPstVScaling=TRUE;
            pSrcInfo->u16VCusScalingSrc = pSrcInfo->ScaledCropWin.height;
            pSrcInfo->u16VCusScalingDst = pSrcInfo->stDispWin.height/2;
        }
        else if(IS_INPUT_SIDE_BY_SIDE_FULL(eWindow))
        {

            b3DPstHScaling=TRUE;
            pSrcInfo->u16HCusScalingSrc = pSrcInfo->ScaledCropWin.width;
            pSrcInfo->u16HCusScalingDst = pSrcInfo->stDispWin.width;

            b3DPstVScaling=TRUE;
            pSrcInfo->u16VCusScalingSrc = pSrcInfo->ScaledCropWin.height;
            pSrcInfo->u16VCusScalingDst = pSrcInfo->stDispWin.height/2;
        }
        else if(IS_INPUT_TOP_BOTTOM(eWindow))
        {
            b3DPstVScaling=TRUE;
            b3DPstHScaling=TRUE;
            pSrcInfo->u16HCusScalingSrc = pSrcInfo->ScaledCropWin.width;
            pSrcInfo->u16HCusScalingDst = pSrcInfo->stDispWin.width;
            pSrcInfo->u16VCusScalingSrc = pSrcInfo->ScaledCropWin.height/2;
            pSrcInfo->u16VCusScalingDst = pSrcInfo->stDispWin.height/2;
        }
        else if(IS_INPUT_NORMAL_2D(eWindow) && IS_OUTPUT_LINE_ALTERNATIVE())
        {
            b3DPstVScaling=TRUE;
            b3DPstHScaling=TRUE;
            pSrcInfo->u16HCusScalingSrc = pSrcInfo->ScaledCropWin.width;
            pSrcInfo->u16HCusScalingDst = pSrcInfo->stDispWin.width + (_u163DHShift&0xFF);

            pSrcInfo->u16VCusScalingSrc = pSrcInfo->ScaledCropWin.height;
            pSrcInfo->u16VCusScalingDst = pSrcInfo->stDispWin.height/2;
        }
        else if(IS_INPUT_FRAME_ALTERNATIVE(eWindow))
        {
            b3DPstVScaling=TRUE;
            b3DPstHScaling=TRUE;
            pSrcInfo->u16HCusScalingSrc = pSrcInfo->ScaledCropWin.width;
            pSrcInfo->u16HCusScalingDst = pSrcInfo->stDispWin.width;

            pSrcInfo->u16VCusScalingSrc = pSrcInfo->ScaledCropWin.height;
            pSrcInfo->u16VCusScalingDst = pSrcInfo->stDispWin.height/2;
        }
        else if(IS_INPUT_LINE_ALTERNATIVE(eWindow))
        {
            b3DPstHScaling=TRUE;
            pSrcInfo->u16HCusScalingSrc = pSrcInfo->ScaledCropWin.width;
            pSrcInfo->u16HCusScalingDst = pSrcInfo->stDispWin.width;

            b3DPstVScaling=TRUE;
            pSrcInfo->u16VCusScalingSrc = pSrcInfo->ScaledCropWin.height/2;
            pSrcInfo->u16VCusScalingDst = pSrcInfo->stDispWin.height/2;
        }
    #if (HW_DESIGN_3D_VER >= 4)
        else if(IS_INPUT_FIELD_ALTERNATIVE(eWindow))
        {
            b3DPstHScaling=TRUE;
            pSrcInfo->u16HCusScalingSrc = pSrcInfo->ScaledCropWin.width;
            pSrcInfo->u16HCusScalingDst = pSrcInfo->stDispWin.width;

            b3DPstVScaling=TRUE;
            pSrcInfo->u16VCusScalingSrc = pSrcInfo->ScaledCropWin.height;
            pSrcInfo->u16VCusScalingDst = pSrcInfo->stDispWin.height/2;
        }
        else if(IS_INPUT_CHECK_BOARD(eWindow))
        {
            b3DPstHScaling=TRUE;
            pSrcInfo->u16HCusScalingSrc = pSrcInfo->ScaledCropWin.width/2;
            pSrcInfo->u16HCusScalingDst = pSrcInfo->stDispWin.width;

            b3DPstVScaling=TRUE;
            pSrcInfo->u16VCusScalingSrc = pSrcInfo->ScaledCropWin.height;
            pSrcInfo->u16VCusScalingDst = pSrcInfo->stDispWin.height/2;
        }
    #endif

        if(IS_OUTPUT_FRAME_L() || IS_OUTPUT_FRAME_R())
        {
            b3DPstVScaling=TRUE;
            b3DPstHScaling=TRUE;
            pSrcInfo->u16VCusScalingDst <<= 1; //For 3D->2D L/R frame output, set to scaling up twice
        }
    }
    else if(IS_OUTPUT_SIDE_BY_SIDE_HALF())
    {
        if(IS_INPUT_FRAME_PACKING(eWindow))
        {
            b3DPstVScaling=TRUE;
            b3DPstHScaling=TRUE;

            pSrcInfo->u16HCusScalingSrc = pSrcInfo->ScaledCropWin.width;
            pSrcInfo->u16HCusScalingDst = pSrcInfo->stDispWin.width/2;
#if (HW_DESIGN_3D_VER >= 3)
            if(pSrcInfo->stCapWin.height == DOUBLEHD_720X2P_VSIZE)
            {
                pSrcInfo->u16VCusScalingSrc = u16VactVideo;
                pSrcInfo->u16VCusScalingDst = pSrcInfo->stDispWin.height;
            }
            else
            {
                pSrcInfo->u16VCusScalingSrc = (pSrcInfo->ScaledCropWin.height)/2;
                pSrcInfo->u16VCusScalingDst = pSrcInfo->stDispWin.height;
            }
#else
            if(pSrcInfo->stCapWin.height == DOUBLEHD_1080X2I_VSIZE)
            {
                pSrcInfo->u16VCusScalingSrc = (pSrcInfo->ScaledCropWin.height-u16VactSpace)/4;
                pSrcInfo->u16VCusScalingDst = pSrcInfo->stDispWin.height;
            }
            else
            {
                pSrcInfo->u16VCusScalingSrc = (pSrcInfo->ScaledCropWin.height-u16VactSpace)/2;
                pSrcInfo->u16VCusScalingDst = pSrcInfo->stDispWin.height;
            }
#endif
        }
        else if(IS_INPUT_TOP_BOTTOM(eWindow) || IS_INPUT_LINE_ALTERNATIVE(eWindow))
        {
            b3DPstVScaling=TRUE;
            b3DPstHScaling=TRUE;
            pSrcInfo->u16HCusScalingSrc = pSrcInfo->ScaledCropWin.width;
            pSrcInfo->u16HCusScalingDst = pSrcInfo->stDispWin.width/2;

            pSrcInfo->u16VCusScalingSrc = pSrcInfo->ScaledCropWin.height/2;
            pSrcInfo->u16VCusScalingDst = pSrcInfo->stDispWin.height;
        }
        else if(IS_INPUT_FRAME_ALTERNATIVE(eWindow))
        {
            b3DPstVScaling=TRUE;
            b3DPstHScaling=TRUE;
            pSrcInfo->u16HCusScalingSrc = pSrcInfo->ScaledCropWin.width;
            pSrcInfo->u16HCusScalingDst = pSrcInfo->stDispWin.width/2;
            pSrcInfo->u16VCusScalingSrc = pSrcInfo->ScaledCropWin.height;
            pSrcInfo->u16VCusScalingDst = pSrcInfo->stDispWin.height;
        }
    }
    else if(IS_OUTPUT_FRAME_ALTERNATIVE())
    {
        if(IS_INPUT_FRAME_PACKING(eWindow))
        {
#if (HW_DESIGN_3D_VER >= 3)
            if((pSrcInfo->stCapWin.height == DOUBLEHD_720X2P_VSIZE)&&
                        (eOutputMode == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE))
            {
                if(g_XC_Pnl_Misc.FRCInfo.bFRC)
                {
                    b3DPstHScaling=TRUE;
                    pSrcInfo->u16HCusScalingSrc = pSrcInfo->ScaledCropWin.width;
                    pSrcInfo->u16HCusScalingDst = pSrcInfo->stDispWin.width;

                    b3DPstVScaling=FALSE;
                    pSrcInfo->u16VCusScalingSrc = pSrcInfo->ScaledCropWin.height;
                    pSrcInfo->u16VCusScalingDst = pSrcInfo->ScaledCropWin.height;
                }
                else
                {
                    b3DPstHScaling=TRUE;
                    pSrcInfo->u16HCusScalingSrc = pSrcInfo->ScaledCropWin.width;
                    pSrcInfo->u16HCusScalingDst = pSrcInfo->stDispWin.width;

                    b3DPstVScaling=TRUE;
                    pSrcInfo->u16VCusScalingSrc = u16VactVideo;//pSrcInfo->ScaledCropWin.height;
                    pSrcInfo->u16VCusScalingDst = pSrcInfo->stDispWin.height;
                }
            }
            else
#endif
            {
                b3DPstVScaling=TRUE;
                b3DPstHScaling=TRUE;
                pSrcInfo->u16HCusScalingSrc = pSrcInfo->ScaledCropWin.width;
                pSrcInfo->u16HCusScalingDst = pSrcInfo->stDispWin.width;

#if (HW_DESIGN_3D_VER >= 3)
                pSrcInfo->u16VCusScalingSrc = pSrcInfo->ScaledCropWin.height/2;
#else
                if(pSrcInfo->stCapWin.height == DOUBLEHD_1080X2I_VSIZE)
                {
                    pSrcInfo->u16VCusScalingSrc = (pSrcInfo->ScaledCropWin.height-u16VactSpace)/4;
                }
                else
                {
                    pSrcInfo->u16VCusScalingSrc = (pSrcInfo->ScaledCropWin.height-u16VactSpace)/2;
                }
#endif
                pSrcInfo->u16VCusScalingDst = pSrcInfo->stDispWin.height;
            }
        }
        else if(IS_INPUT_LINE_ALTERNATIVE(eWindow))
        {
            b3DPstVScaling=TRUE;
            pSrcInfo->u16VCusScalingSrc = pSrcInfo->ScaledCropWin.height/2;
            pSrcInfo->u16VCusScalingDst = pSrcInfo->stDispWin.height;
        }
        else if(IS_INPUT_SIDE_BY_SIDE_FULL(eWindow))
        {
            b3DPstHScaling=TRUE;
            pSrcInfo->u16HCusScalingSrc = pSrcInfo->ScaledCropWin.width/2;
            pSrcInfo->u16HCusScalingDst = pSrcInfo->stDispWin.width;

            b3DPstVScaling=TRUE;
            pSrcInfo->u16VCusScalingSrc = pSrcInfo->ScaledCropWin.height;
            pSrcInfo->u16VCusScalingDst = pSrcInfo->stDispWin.height/2;
        }
#if (HW_DESIGN_3D_VER >= 4)
        else if(IS_INPUT_FIELD_ALTERNATIVE(eWindow))
        {
            b3DPstHScaling=TRUE;
            pSrcInfo->u16HCusScalingSrc = pSrcInfo->ScaledCropWin.width;
            pSrcInfo->u16HCusScalingDst = pSrcInfo->stDispWin.width;

            b3DPstVScaling=TRUE;
            pSrcInfo->u16VCusScalingSrc = pSrcInfo->ScaledCropWin.height/2;
            pSrcInfo->u16VCusScalingDst = pSrcInfo->stDispWin.height/2;
        }
        else if(IS_INPUT_CHECK_BOARD(eWindow))
        {

        }
#endif
#ifdef FA_1920X540_OUTPUT  // for 1920x540x120Hz output
        else if(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow)&&(!g_XC_Pnl_Misc.FRCInfo.bFRC))
        {
            b3DPstHScaling=TRUE;
            pSrcInfo->u16HCusScalingSrc = pSrcInfo->ScaledCropWin.width/2;
            pSrcInfo->u16HCusScalingDst = pSrcInfo->stDispWin.width;

            b3DPstVScaling=TRUE;
            pSrcInfo->u16VCusScalingSrc = pSrcInfo->ScaledCropWin.height;
            pSrcInfo->u16VCusScalingDst = pSrcInfo->stDispWin.height/2;
        }
#endif

    }
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW, "u16HCusScalingSrc=%d, u16HCusScalingDst=%d\n", pSrcInfo->u16HCusScalingSrc, pSrcInfo->u16HCusScalingDst)
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW, "u16VCusScalingSrc=%d, u16VCusScalingDst=%d\n", pSrcInfo->u16VCusScalingSrc, pSrcInfo->u16VCusScalingDst)

    if(eWindow == MAIN_WINDOW)
    {
        if(b3DPstVScaling || b3DPstHScaling)
        {
            pSrcInfo->bVCusScaling = TRUE;
            pSrcInfo->bHCusScaling = TRUE;
            MDrv_SC_set_postscaling_ratio(pSrcInfo, eWindow);
            //HVSP
            _XC_ENTRY();
            SC_W4BYTE(REG_SC_BK23_07_L, stDBreg.u32H_PostScalingRatio);  // H post-scaling
            SC_W4BYTE(REG_SC_BK23_09_L, stDBreg.u32V_PostScalingRatio);  // V post-scaling
            _XC_RETURN()
        }
    }
}

//------------------------------------------------------------------------------------------------------
/// config sub win for 3d
/// @param  bEn                \b IN: input  enable sub window or disable
/// @return MS_BOOL                  \b OUT: TRUE: set is ok FALSE: set value is odd, need align to even
//------------------------------------------------------------------------------------------------------
MS_BOOL MDrv_XC_3D_CfgSubWin(MS_BOOL bEn)
{
    if(ENABLE == bEn)
    {
        MS_U32 u32tmp;
        SC_W2BYTEMSK(REG_SC_BK20_10_L, 0x0002, 0x0002);//Enable sub window shown on the screen
        SC_W2BYTEMSK(REG_SC_BK20_18_L, 0x0002, 0x0002);//Force F1 use F2's register setting
        SC_W2BYTEMSK(REG_SC_BK03_02_L, 0x0000, 0x0080);//source sync enable
        //MDrv_ACE_3DClonePQMap(FALSE, FALSE);
        u32tmp = SC_R4BYTE(REG_SC_BK02_04_L);
        SC_W4BYTE(REG_SC_BK04_04_L, u32tmp);   // H pre-scaling
        u32tmp = SC_R4BYTE(REG_SC_BK02_08_L);
        SC_W4BYTE(REG_SC_BK04_08_L, u32tmp);   // V pre-scaling
        u32tmp = SC_R4BYTE(REG_SC_BK23_07_L);
        SC_W4BYTE(REG_SC_BK23_27_L, u32tmp);  // H post-scaling
        u32tmp = SC_R4BYTE(REG_SC_BK23_09_L);
        SC_W4BYTE(REG_SC_BK23_29_L, u32tmp);  // V post-scaling

        u32tmp = (MS_U32)(SC_R2BYTE(REG_SC_BK12_19_L));
        SC_W2BYTE(REG_SC_BK12_59_L, (MS_U16)u32tmp);//frame number and opm/ipm seperate.

        u32tmp = (MS_U32)(SC_R2BYTE(REG_SC_BK12_01_L));
        SC_W2BYTE(REG_SC_BK12_41_L, (MS_U16)u32tmp);//madi format

        u32tmp = (MS_U32)(SC_R2BYTE(REG_SC_BK12_02_L));
        SC_W2BYTE(REG_SC_BK12_42_L, (MS_U16)u32tmp);

        u32tmp = (MS_U32)(SC_R2BYTE(REG_SC_BK12_03_L));
        SC_W2BYTE(REG_SC_BK12_43_L, (MS_U16)u32tmp);

        u32tmp = (MS_U32)(SC_R2BYTE(REG_SC_BK12_04_L));
        SC_W2BYTE(REG_SC_BK12_44_L, (MS_U16)u32tmp);

        u32tmp = (MS_U32)(SC_R2BYTE(REG_SC_BK12_05_L));
        SC_W2BYTE(REG_SC_BK12_45_L, (MS_U16)u32tmp);
    }
    else
    {
        SC_W2BYTEMSK(REG_SC_BK20_10_L, 0x0000, 0x0002);//Enable sub window shown on the screen
        SC_W2BYTEMSK(REG_SC_BK20_18_L, 0x0000, 0x0002);//Force F1 use F2's register setting
        SC_W2BYTEMSK(REG_SC_BK03_02_L, 0x0080, 0x0080);//source sync disable
        //MDrv_ACE_3DClonePQMap(FALSE, FALSE);
    }

    return TRUE;
}
#endif

void MDrv_SC_3D_Disable_Crop(XC_InternalStatus *pSrcInfo, SCALER_WIN eWindow)
{
#if(HW_DESIGN_3D_VER < 2)
    //only 2d to 3d can do overscan for all chips
    if((!IS_INPUT_MODE_NONE(eWindow)) && (!IS_OUTPUT_MODE_NONE())
       && (!IS_INPUT_NORMAL_2D(eWindow)) && (!IS_INPUT_NORMAL_2D_HW(eWindow))
       && (!IS_INPUT_NORMAL_2D_INTERLACE(eWindow))
       && (!IS_INPUT_NORMAL_2D_INTERLACE_PTP(eWindow)))
    {
        pSrcInfo->ScaledCropWin.x     = 0;
        pSrcInfo->ScaledCropWin.y     = 0;
        pSrcInfo->ScaledCropWin.width = pSrcInfo->u16H_SizeAfterPreScaling;
        pSrcInfo->ScaledCropWin.height= pSrcInfo->u16V_SizeAfterPreScaling;
    }
#endif
}

MS_BOOL MDrv_XC_3D_LoadReg(E_XC_3D_INPUT_MODE eInputMode,
                           E_XC_3D_OUTPUT_MODE eOutputMode,
                           XC_InternalStatus *pSrcInfo,
                           SCALER_WIN eWindow)
{
	MS_U16 u16VactVideo, u16VactSpace;
#if ((HW_DESIGN_3D_VER == 1))
    _XC_ENTRY();
    MDrv_SC_3D_Set2TapMode(pSrcInfo, eWindow);
    _XC_RETURN();
#elif (HW_DESIGN_3D_VER >= 2)
    static MS_BOOL b3Dstate = DISABLE;
    //First init all control register to HW init value
    _XC_ENTRY();
    if(b3Dstate == ENABLE)
    {
        SC_W2BYTEMSK(REG_SC_BK20_10_L, 0x0000, 0x0002);//Enable sub window shown on the screen
        SC_W2BYTEMSK(REG_SC_BK20_18_L, 0x0000, 0x0002);//Force F1 use F2's register setting
        SC_W2BYTEMSK(REG_SC_BK12_30_L, 0x0000, 0x0001);// 3d enable
        SC_W2BYTEMSK(REG_SC_BK12_33_L, 0x0000, 0x0082);//HDMI 3D field select toggle enable, OP 3D mode enable
        if(!_bSkipDefaultLRFlag)
        {
            Hal_XC_H3D_LR_Toggle_Enable(FALSE);//HDMI 3D field select toggle disable
        }
        else
        {
            SC_W2BYTEMSK(REG_SC_BK12_33_L, (FALSE<<1), (BIT(1)));
        }
        SC_W2BYTEMSK(REG_SC_BK23_53_L, 0x0000, 0x8000);// 3D LR side-by-side to line-to-line enable
        SC_W2BYTEMSK(REG_SC_BK02_62_L, 0x0000, 0x0130);//external lr signal enable and swap
        SC_W2BYTE(REG_SC_BK02_63_L, 0x0000);//hw 3d TYPE, L/R split etc.
        SC_W2BYTE(REG_SC_BK02_64_L, 0x0000);//FPP, FPI, FIdA related control
        SC_W2BYTEMSK(REG_SC_BK02_65_L, 0x0000, 0x1FFF);//v_active region
        SC_W2BYTEMSK(REG_SC_BK02_66_L, 0x0000, 0x1FFF);//v blanking between field1&field2 or field3&field4
        SC_W2BYTEMSK(REG_SC_BK02_67_L, 0x0000, 0x1FFF);//v blanking between field2&field3

        SC_W2BYTEMSK(REG_SC_BK02_6B_L, 0x00, BIT(0)|BIT(2)); //[0]pix_sep_en,[2]LR_Alt_line
        SC_W2BYTEMSK(REG_SC_BK02_6F_L, 0x00, 0x010F); //resample, [8]enable

        SC_W2BYTEMSK(REG_SC_BK12_30_L, 0, BIT(4));//F2 IPM 3D LR invert
        SC_W2BYTEMSK(REG_SC_BK12_33_L, 0, BIT(11)|BIT(8)|BIT(1)|BIT(0));//[11]opm3d: OPM SBS using PIP
                                                                //[8] F2 OPM 3D LR invert
                                                                //[1] bsel_r_toggle_en
                                                                //[0] bsel_r_inv
        SC_W2BYTEMSK(REG_SC_BK12_05_L, 0x0000, 0x0100);//reg_bk_field_sel_f2
        SC_W2BYTEMSK(REG_SC_BK12_19_L, 0x0000, 0x8800);//clear opm/ipm frame number separate control
        SC_W2BYTE(REG_SC_BK20_1C_L, 0x00); //Offset main/sub display window in right direction
        SC_W2BYTEMSK(REG_SC_BK20_10_L, 0x0, BIT(5)); //Enable HD side by side line buffer mode
#if (HW_DESIGN_3D_VER >= 4)
        SC_W2BYTEMSK(REG_SC_BK12_06_L, 0x0000, 0x0F00);//[11]Enable, [10:8]F2 IPM user define read
        W2BYTEMSK(REG_CLKGEN0_51_L,0x4000,0xF000);
#endif
    }
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"Windows[%u] eInputMode=%u, eOutputMode=%u, b3Dstate=%u\n", eWindow, eInputMode, eOutputMode, b3Dstate);

    if((IS_INPUT_MODE_NONE(eWindow) || IS_OUTPUT_MODE_NONE() || IS_INPUT_NORMAL_2D_HW(eWindow)))
    {
        //Close 3D, just need restore Mainwindow here
        if(b3Dstate == ENABLE)
        {
        #ifdef MULTI_SCALER_SUPPORTED
            MDrv_XC_EnableCLK_for_SUB(FALSE, eWindow); //Disable 3D, so disable sub window clk here
        #else
            MDrv_XC_EnableCLK_for_SUB(FALSE); //Disable 3D, so disable sub window clk here
        #endif
            SC_W2BYTEMSK(REG_SC_BK10_08_L, stDBreg.u16H_DisStart, 0x1FFF); // Display H start
            SC_W2BYTEMSK(REG_SC_BK10_09_L, stDBreg.u16H_DisEnd, 0x1FFF);   // Display H end
            SC_W2BYTEMSK(REG_SC_BK10_0A_L, stDBreg.u16V_DisStart, 0x0FFF);//Main window display window
            SC_W2BYTEMSK(REG_SC_BK10_0B_L, stDBreg.u16V_DisEnd, 0x0FFF);


            if(g_XC_Pnl_Misc.FRCInfo.u83D_FI_out == E_XC_3D_OUTPUT_FI_MODE_NONE)
            {
                SC_W2BYTEMSK(REG_SC_BK10_09_L, stDBreg.u16H_DisEnd, 0x1FFF);
                SC_W2BYTEMSK(REG_SC_BK10_0B_L, stDBreg.u16V_DisEnd, 0x0FFF);
                SC_W2BYTEMSK(REG_SC_BK10_05_L, (g_XC_InitData.stPanelInfo.u16HStart+g_XC_InitData.stPanelInfo.u16Width - 1), 0x0FFF);
                SC_W2BYTEMSK(REG_SC_BK10_07_L, (g_XC_InitData.stPanelInfo.u16VStart+g_XC_InitData.stPanelInfo.u16Height - 1), 0x0FFF);
            }

            b3Dstate = DISABLE;
        }
        //_XC_RETURN();
        //return TRUE;
    }
    else
    {
        SC_W2BYTEMSK(REG_SC_BK12_05_L, 0x0000, BIT(9));//Enable OPM F1 register
        b3Dstate = ENABLE;
    }
    _XC_RETURN();

    if(b3Dstate)
    {
        //input part

        //because prescaling and postscaling function will use pq functions
        //and pq will use mutex protect, so we should release mutex for these two cases.
        MDrv_SC_3D_Adjust_PreScaling(eInputMode, eOutputMode, pSrcInfo, eWindow);
        MDrv_SC_set_crop_window(pSrcInfo, eWindow);
        MDrv_SC_3D_Adjust_PstScaling(eInputMode, eOutputMode, pSrcInfo, eWindow);
    }

    _XC_ENTRY();
    if(b3Dstate)
    {
        if(pSrcInfo->bHCusScaling || pSrcInfo->bVCusScaling || pSrcInfo->Status2.bPreVCusScaling || pSrcInfo->Status2.bPreHCusScaling)
        {
            MDrv_SC_set_fetch_number_limit(pSrcInfo, eWindow);
        }
        SC_W4BYTE(REG_SC_BK12_08_L, stDBreg.u32DNRBase0);//DNR OPM base set the same
        SC_W4BYTE(REG_SC_BK12_10_L, stDBreg.u32OPMBase0);
        SC_W2BYTEMSK(REG_SC_BK20_12_L, 0x0000, 0x0FFF);//The starting address of f2 stored at line buffer

#if (HW_DESIGN_3D_VER >= 4)
        if((IS_INPUT_FRAME_PACKING(eWindow)&&IS_OUTPUT_TOP_BOTTOM())||
           (IS_INPUT_LINE_ALTERNATIVE(eWindow)&&IS_OUTPUT_TOP_BOTTOM())||
           (IS_INPUT_FIELD_ALTERNATIVE(eWindow)&&IS_OUTPUT_TOP_BOTTOM())||
           (IS_INPUT_FRAME_ALTERNATIVE(eWindow)&&IS_OUTPUT_TOP_BOTTOM())||
           (IS_INPUT_TOP_BOTTOM(eWindow)&&IS_OUTPUT_TOP_BOTTOM()&&(pSrcInfo->u16H_SizeAfterPreScaling > (SCALER_LINE_BUFFER_MAX + SUB_SCALER_LINE_BUFFER_MAX)/2))
           )
        {
            //The starting address of f1 stored at line buffer
            //in A5, Main and Sub window will use the (1920+960) in 2Tap st the same time
            SC_W2BYTEMSK(REG_SC_BK20_13_L, 0x00, 0x0FFF);
            // When main and sub is start from 0, it means the sub win don't need Fill the sub windows line buffer in vertical blanking
            SC_W2BYTEMSK(REG_SC_BK20_10_L, 0x00, BIT(11)); //[11]reg_vblank_sub
        }
        else
#endif
        if(MDrv_SC_3D_Is2TapModeSupportedFormat(pSrcInfo, eWindow))
        {
            MDrv_SC_3D_Set2TapMode(pSrcInfo, eWindow);
        }
        else
        {
            SC_W2BYTEMSK(REG_SC_BK20_13_L, (SCALER_LINE_BUFFER_MAX + SUB_SCALER_LINE_BUFFER_MAX)/2, 0x0FFF);//The starting address of f1 stored at line buffer
		}
    }

    if(IS_INPUT_NORMAL_2D(eWindow))
    {
        SC_W2BYTEMSK(REG_SC_BK12_0E_L, stDBreg.u16DNROffset, 0x1FFF);//dnr offset
        SC_W2BYTEMSK(REG_SC_BK12_0F_L, stDBreg.u16DNRFetch, 0x1FFF);//dnr fetch
        SC_W2BYTEMSK(REG_SC_BK12_16_L, stDBreg.u16OPMOffset, 0x1FFF);//OPM offset
        SC_W2BYTEMSK(REG_SC_BK12_17_L, stDBreg.u16OPMFetch, 0x0FFF);//OPM FETCH
        SC_W2BYTEMSK(REG_SC_BK12_56_L, stDBreg.u16OPMOffset, 0x1FFF);//F1 OPM OFFSET
        SC_W2BYTEMSK(REG_SC_BK12_57_L, stDBreg.u16OPMFetch, 0x0FFF);//F1 OPM FETCH
        SC_W2BYTEMSK(REG_SC_BK12_33_L, 0x0800, 0x0800);//opm3d: OPM SBS using PIP
        SC_W2BYTE(REG_SC_BK20_1C_L, _u163DHShift);//Main/sub display window offset in right direction
    }
    else if(IS_INPUT_FRAME_PACKING(eWindow))
    {
        MS_U16 u16VactVideo, u16VactSpace;
#if (HW_DESIGN_3D_VER >= 3) // for A5
        MDrv_SC_3D_GetFP_Info(pSrcInfo, &u16VactVideo, &u16VactSpace, DISABLE);
#else
        MDrv_SC_3D_GetFP_Info(pSrcInfo, &u16VactVideo, &u16VactSpace, (stDBreg.u32V_PreScalingRatio & BIT(31))>>31);
#endif
        SC_W2BYTEMSK(REG_SC_BK12_0E_L, stDBreg.u16DNROffset, 0x1FFF);//dnr offset
        SC_W2BYTEMSK(REG_SC_BK12_0F_L, stDBreg.u16DNRFetch, 0x1FFF);//dnr fetch
        SC_W2BYTEMSK(REG_SC_BK12_16_L, stDBreg.u16OPMOffset, 0x1FFF);//OPM offset
        SC_W2BYTEMSK(REG_SC_BK12_17_L, stDBreg.u16OPMFetch, 0x0FFF);//OPM FETCH
        SC_W2BYTEMSK(REG_SC_BK12_56_L, stDBreg.u16OPMOffset, 0x1FFF);//F1 OPM OFFSET
        SC_W2BYTEMSK(REG_SC_BK12_57_L, stDBreg.u16OPMFetch, 0x0FFF);//F1 OPM FETCH
#if (HW_DESIGN_3D_VER >= 3)
        if(pSrcInfo->stCapWin.height == DOUBLEHD_1080X2I_VSIZE)
        {
            //interlace framepacking
            SC_W2BYTEMSK(REG_SC_BK02_63_L, 0x0002, 0x0103);//hw 3d TYPE, L/R split
            SC_W2BYTEMSK(REG_SC_BK02_64_L, 0x0237, 0xff37);//Blanking number in one vsync,enable gen pseudo vsync in 3d
            SC_W2BYTEMSK(REG_SC_BK02_65_L, u16VactVideo, 0x1FFF);//v_active region
            SC_W2BYTEMSK(REG_SC_BK02_66_L, DOUBLEHD_1080X2I_VACT_SPACE1, 0x1FFF);//v blanking between field1&field2 or field3&field4
            SC_W2BYTEMSK(REG_SC_BK02_67_L, DOUBLEHD_1080X2I_VACT_SPACE2, 0x1FFF);//v blanking between field2&field3
            SC_W2BYTEMSK(REG_SC_BK12_05_L, 0x0100, 0x0100);//reg_bk_field_sel_f2
            SC_W2BYTEMSK(REG_SC_BK12_30_L, BIT(0), BIT(0));//F2 IPM 3D input enable
            SC_W2BYTEMSK(REG_SC_BK12_33_L, BIT(7), BIT(7));//OP 3D ENABLE

            MDrv_SC_3D_GetFP_Info(pSrcInfo, &u16VactVideo, &u16VactSpace, (MS_BOOL)((stDBreg.u32V_PreScalingRatio & BIT(31))>>31));
            SC_W2BYTEMSK(REG_SC_BK02_38_L, u16VactVideo, 0x1FFF);
            SC_W2BYTEMSK(REG_SC_BK02_38_L, BIT(15), BIT(15));

            if(IS_OUTPUT_TOP_BOTTOM())  // for A5p and A3
            {
                SC_3D_DBG(printf("## 3D input is FPI and output is TBP\n"));
            #if (HW_DESIGN_3D_VER >= 4)
                SC_W2BYTEMSK(REG_SC_BK12_06_L, 0x0A00, 0x0F00);//[11]Enable, [10:8]F2 IPM user define read
            #endif
            }
            else if(IS_OUTPUT_FRAME_ALTERNATIVE()) // for A5
            {
                SC_3D_DBG(printf("## 3D input is FPI and output is FI\n"));
            }
            SC_W2BYTEMSK(REG_SC_BK02_7F_L, pSrcInfo->stCapWin.width, 0x1FFF);
        }
        else
        {
            SC_3D_DBG(printf("## 3D input is FPP\n"));
            //progressive framepacking
            SC_W2BYTEMSK(REG_SC_BK02_63_L, 0x0002, 0x0103);//hw 3d TYPE, L/R split
            SC_W2BYTEMSK(REG_SC_BK02_64_L, 0x0011, 0x0037);//Blanking number in one vsync
            SC_W2BYTEMSK(REG_SC_BK02_65_L, u16VactVideo, 0x1FFF);//v_active region=L Frame V size after prescaling
            SC_W2BYTEMSK(REG_SC_BK02_66_L, u16VactSpace, 0x1FFF);//v blanking between field1&field2 or field3&field4
            SC_W2BYTEMSK(REG_SC_BK12_30_L, BIT(0), BIT(0));//F2 IPM 3D input enable
            SC_W2BYTEMSK(REG_SC_BK12_33_L, BIT(7), BIT(7));//OP 3D ENABLE
            #if (HW_DESIGN_3D_VER >= 4)
            SC_W2BYTEMSK(REG_SC_BK02_38_L, stDBreg.u16VLen, 0x1FFF);
            #else
            MDrv_SC_3D_GetFP_Info(pSrcInfo, &u16VactVideo, &u16VactSpace, (stDBreg.u32V_PreScalingRatio & BIT(31))>>31);
            SC_W2BYTEMSK(REG_SC_BK02_38_L, u16VactVideo, 0x1FFF);
            #endif
            SC_W2BYTEMSK(REG_SC_BK02_38_L, BIT(15), BIT(15));
            SC_W2BYTEMSK(REG_SC_BK02_7F_L, pSrcInfo->stCapWin.width, 0x1FFF);
        }
#else
        if(pSrcInfo->stCapWin.height == DOUBLEHD_1080X2I_VSIZE)
        {
            //interlace framepacking
            SC_W2BYTEMSK(REG_SC_BK02_63_L, 0x0002, 0x0103);//hw 3d TYPE, L/R split
            SC_W2BYTEMSK(REG_SC_BK02_64_L, 0x0237, 0xff37);//Blanking number in one vsync,enable gen pseudo vsync in 3d
            SC_W2BYTEMSK(REG_SC_BK02_65_L, DOUBLEHD_1080X2I_FIELD_VSIZE, 0x1FFF);//v_active region
            SC_W2BYTEMSK(REG_SC_BK02_66_L, DOUBLEHD_1080X2I_VACT_SPACE1, 0x1FFF);//v blanking between field1&field2 or field3&field4
            SC_W2BYTEMSK(REG_SC_BK02_67_L, DOUBLEHD_1080X2I_VACT_SPACE2, 0x1FFF);//v blanking between field2&field3
            SC_W2BYTEMSK(REG_SC_BK12_05_L, 0x0100, 0x0100);//reg_bk_field_sel_f2
            SC_W2BYTEMSK(REG_SC_BK12_30_L, BIT(0), BIT(0));//F2 IPM 3D input enable
            SC_W2BYTEMSK(REG_SC_BK12_33_L, BIT(7), BIT(7));//OP 3D ENABLE
        }
        else
        {
            //progressive framepacking
            SC_W2BYTEMSK(REG_SC_BK02_63_L, 0x0002, 0x0103);//hw 3d TYPE, L/R split
            SC_W2BYTEMSK(REG_SC_BK02_64_L, 0x0011, 0x0037);//Blanking number in one vsync
            SC_W2BYTEMSK(REG_SC_BK02_65_L, u16VactVideo, 0x1FFF);//v_active region=L Frame V size after prescaling
            SC_W2BYTEMSK(REG_SC_BK02_66_L, u16VactSpace, 0x1FFF);//v blanking between field1&field2 or field3&field4
            SC_W2BYTEMSK(REG_SC_BK12_30_L, BIT(0), BIT(0));//F2 IPM 3D input enable
            SC_W2BYTEMSK(REG_SC_BK12_33_L, BIT(7), BIT(7));//OP 3D ENABLE
        }
#endif
    }
    else if(IS_INPUT_FIELD_ALTERNATIVE(eWindow))
    {
        SC_3D_DBG(printf("## 3D input is FA\n"));
#if (HW_DESIGN_3D_VER >= 4) // for A5
        //MS_U16 u16VactVideo, u16VactSpace;

        MDrv_SC_3D_GetFP_Info(pSrcInfo, &u16VactVideo, &u16VactSpace, DISABLE);
        SC_W2BYTEMSK(REG_SC_BK02_63_L, 0x0002, 0x0103);//hw 3d TYPE, L/R split
        SC_W2BYTEMSK(REG_SC_BK02_64_L, 0x0011, 0x0037);//Blanking number in one vsync
        SC_W2BYTEMSK(REG_SC_BK02_65_L, u16VactVideo, 0x1FFF);//v_active region=L Frame V size after prescaling
        SC_W2BYTEMSK(REG_SC_BK02_66_L, u16VactSpace, 0x1FFF);//v blanking between field1&field2 or field3&field4

        SC_W2BYTEMSK(REG_SC_BK02_38_L, stDBreg.u16VLen, 0x1FFF);
        SC_W2BYTEMSK(REG_SC_BK02_38_L, BIT(15), BIT(15));
        SC_W2BYTEMSK(REG_SC_BK02_7F_L, pSrcInfo->stCapWin.width, 0x1FFF);

        SC_W2BYTEMSK(REG_SC_BK12_0E_L, stDBreg.u16DNROffset, 0x1FFF);//dnr offset
        SC_W2BYTEMSK(REG_SC_BK12_0F_L, stDBreg.u16DNRFetch, 0x1FFF);//dnr fetch
        SC_W2BYTEMSK(REG_SC_BK12_16_L, stDBreg.u16OPMOffset, 0x1FFF);//OPM offset
        SC_W2BYTEMSK(REG_SC_BK12_17_L, stDBreg.u16OPMFetch, 0x0FFF);//OPM FETCH
        SC_W2BYTEMSK(REG_SC_BK12_56_L, stDBreg.u16OPMOffset, 0x1FFF);//F1 OPM OFFSET
        SC_W2BYTEMSK(REG_SC_BK12_57_L, stDBreg.u16OPMFetch, 0x0FFF);//F1 OPM FETCH

        SC_W2BYTEMSK(REG_SC_BK12_30_L, BIT(0), BIT(0));//F2 IPM 3D input enable
        SC_W2BYTEMSK(REG_SC_BK12_33_L, BIT(7), BIT(7));//OP 3D ENABLE

        //==> In new QMAP(after add 3D item), these patch need to remove
        // TODO: QMAP patch
        #if 1
        //Disable MCDi
        SC_W2BYTEMSK(REG_SC_BK2A_02_L, 0x00, 0xE008); // [15:13]|[3]
        //Disable DMS_V12Line
        SC_W2BYTEMSK(REG_SC_BK20_10_L, 0x00, BIT(4)); // DMS_V12Line
        //Set SRAM to linear mode
        SC_W2BYTE(REG_SC_BK23_0B_L, 0x303);
        #endif
#else
        if(pSrcInfo->bInterlace == FALSE)
        {
            //progressive
            printf("it's not interlace mode, for E_XC_3D_INPUT_FIELD_ALTERNATIVE\n");
            return FALSE;
        }
        printf("*******************Field Alternative mode, Unsupport now !\r\n");
        SC_W2BYTEMSK(REG_SC_BK12_0E_L, stDBreg.u16DNROffset, 0x1FFF);//dnr offset
        SC_W2BYTEMSK(REG_SC_BK12_0F_L, stDBreg.u16DNRFetch, 0x1FFF);//dnr fetch
        SC_W2BYTEMSK(REG_SC_BK12_16_L, stDBreg.u16OPMOffset, 0x1FFF);//OPM offset
        SC_W2BYTEMSK(REG_SC_BK12_17_L, stDBreg.u16OPMFetch, 0x0FFF);//OPM FETCH
        SC_W2BYTEMSK(REG_SC_BK12_56_L, stDBreg.u16OPMOffset, 0x1FFF);//F1 OPM OFFSET
        SC_W2BYTEMSK(REG_SC_BK12_57_L, stDBreg.u16OPMFetch, 0x0FFF);//F1 OPM FETCH
#endif
    }
    else if(IS_INPUT_FRAME_ALTERNATIVE(eWindow))
    {
        SC_3D_DBG(printf("## 3D input is FA\n"));

        SC_W2BYTEMSK(REG_SC_BK12_0E_L, stDBreg.u16DNROffset, 0x1FFF);//dnr offset
        SC_W2BYTEMSK(REG_SC_BK12_0F_L, stDBreg.u16DNRFetch, 0x1FFF);//dnr fetch
        SC_W2BYTEMSK(REG_SC_BK12_16_L, stDBreg.u16OPMOffset, 0x1FFF);//OPM offset
        SC_W2BYTEMSK(REG_SC_BK12_17_L, stDBreg.u16OPMFetch, 0x0FFF);//OPM FETCH
        SC_W2BYTEMSK(REG_SC_BK12_56_L, stDBreg.u16OPMOffset, 0x1FFF);//F1 OPM OFFSET
        SC_W2BYTEMSK(REG_SC_BK12_57_L, stDBreg.u16OPMFetch, 0x0FFF);//F1 OPM FETCH

#if (HW_DESIGN_3D_VER >= 3)
        SC_W2BYTEMSK(REG_SC_BK12_30_L, BIT(0), BIT(0));// 3d enable
        SC_W2BYTEMSK(REG_SC_BK12_33_L, BIT(7), BIT(7));
        SC_W2BYTEMSK(REG_SC_BK02_62_L, 0x0030, 0x0030);// input Vsync exist between L and R frame
        SC_W2BYTEMSK(REG_SC_BK02_63_L, 0x0001, 0x0103);// hw 3d TYPE, L/R split=0

        SC_W2BYTEMSK(REG_SC_BK02_7F_L, pSrcInfo->stCapWin.width, 0x1FFF);
#else
        if(pSrcInfo->bInterlace == TRUE)
        {
            SC_W2BYTEMSK(REG_SC_BK12_30_L, 0x0001, 0x0001);// 3d enable
            SC_W2BYTEMSK(REG_SC_BK12_33_L, 0x0083, 0x0080);//[8] F2 OPM 3D LR invert
            SC_W2BYTEMSK(REG_SC_BK02_62_L, 0x0010, 0x0010);// input Vsync exist between L and R frame
            SC_W2BYTEMSK(REG_SC_BK02_63_L, 0x0001, 0x0103);// hw 3d TYPE, L/R split=0
        }
        else
        {
            //for progressive frame alternative case, we should disable ip 3d, enable op 3d
            //to let main,sub engine read opm frame data alternatively
            SC_W2BYTEMSK(REG_SC_BK12_33_L, 0x0083, 0x0080);//[8] F2 OPM 3D LR invert
            SC_W2BYTE(REG_SC_BK12_19_L, 0x8804);//input 4 frame, output 2 frame with 3d
        }
#endif
    }
    else if(IS_INPUT_TOP_BOTTOM(eWindow))
    {
#if (HW_DESIGN_3D_VER >= 3)
        SC_3D_DBG(printf("## 3D input is TBP\n"));

        SC_W2BYTEMSK(REG_SC_BK12_0E_L, stDBreg.u16DNROffset, 0x1FFF);//dnr offset
        SC_W2BYTEMSK(REG_SC_BK12_0F_L, stDBreg.u16DNRFetch, 0x1FFF);//dnr fetch
        SC_W2BYTEMSK(REG_SC_BK12_16_L, stDBreg.u16OPMOffset, 0x1FFF);//OPM offset
        SC_W2BYTEMSK(REG_SC_BK12_17_L, stDBreg.u16OPMFetch, 0x0FFF);//OPM FETCH
        SC_W2BYTEMSK(REG_SC_BK12_56_L, stDBreg.u16OPMOffset, 0x1FFF);//F1 OPM OFFSET
        SC_W2BYTEMSK(REG_SC_BK12_57_L, stDBreg.u16OPMFetch, 0x0FFF);//F1 OPM FETCH
        SC_W2BYTEMSK(REG_SC_BK12_30_L, 0x0001, 0x0001);// 3d enable
        SC_W2BYTEMSK(REG_SC_BK12_33_L, 0x0080, 0x0080);
        SC_W2BYTEMSK(REG_SC_BK02_63_L, 0x0002, 0x0103);//hw 3d TYPE, L/R split
        if(pSrcInfo->bInterlace == FALSE)
        {
            //progressive
            SC_W2BYTEMSK(REG_SC_BK02_65_L, pSrcInfo->stCapWin.height, 0x1FFF); //v_active region=L Frame V size after prescaling

            SC_W2BYTEMSK(REG_SC_BK02_38_L, pSrcInfo->u16V_SizeAfterPreScaling/2, 0x1FFF);
        }
        else
        {
            //interlace
            SC_W2BYTEMSK(REG_SC_BK02_65_L, pSrcInfo->stCapWin.height, 0x1FFF);
            SC_W2BYTEMSK(REG_SC_BK02_38_L, pSrcInfo->u16V_SizeAfterPreScaling/4, 0x1FFF);
        }

        SC_W2BYTEMSK(REG_SC_BK02_7F_L, pSrcInfo->stCapWin.width, 0x1FFF);
        SC_W2BYTEMSK(REG_SC_BK02_38_L, BIT(15), BIT(15));

    //==> In new QMAP(after add 3D item), these patch need to remove
    // TODO: QMAP patch
    #if 1
        //Disable MCDi
        SC_W2BYTEMSK(REG_SC_BK2A_02_L, 0x00, 0xE008); // [15:13]|[3]
        //Disable DMS_V12Line
        SC_W2BYTEMSK(REG_SC_BK20_10_L, 0x00, BIT(4)); // DMS_V12Line
        //Set SRAM to linear mode
        SC_W2BYTE(REG_SC_BK23_0B_L, 0x303);

    #endif

#else
        SC_W2BYTEMSK(REG_SC_BK12_0E_L, stDBreg.u16DNROffset, 0x1FFF);//dnr offset
        SC_W2BYTEMSK(REG_SC_BK12_0F_L, stDBreg.u16DNRFetch, 0x1FFF);//dnr fetch
        SC_W2BYTEMSK(REG_SC_BK12_16_L, stDBreg.u16OPMOffset, 0x1FFF);//OPM offset
        SC_W2BYTEMSK(REG_SC_BK12_17_L, stDBreg.u16OPMFetch, 0x0FFF);//OPM FETCH
        SC_W2BYTEMSK(REG_SC_BK12_56_L, stDBreg.u16OPMOffset, 0x1FFF);//F1 OPM OFFSET
        SC_W2BYTEMSK(REG_SC_BK12_57_L, stDBreg.u16OPMFetch, 0x0FFF);//F1 OPM FETCH
        SC_W2BYTEMSK(REG_SC_BK12_30_L, 0x0001, 0x0001);// 3d enable
        SC_W2BYTEMSK(REG_SC_BK12_33_L, 0x0080, 0x0080);
        SC_W2BYTEMSK(REG_SC_BK02_63_L, 0x0002, 0x0103);//hw 3d TYPE, L/R split
        if(pSrcInfo->bInterlace == FALSE)
        {
            //progressive
            SC_W2BYTEMSK(REG_SC_BK02_65_L, pSrcInfo->u16V_SizeAfterPreScaling/2, 0x1FFF); //v_active region=L Frame V size after prescaling
        }
        else
        {
            //interlace
            SC_W2BYTEMSK(REG_SC_BK02_65_L, pSrcInfo->u16V_SizeAfterPreScaling/4, 0x1FFF);
        }
#endif
    }
    else if(IS_INPUT_LINE_ALTERNATIVE(eWindow))
    {
        SC_W2BYTEMSK(REG_SC_BK12_0E_L, stDBreg.u16DNROffset, 0x1FFF);//dnr offset
        SC_W2BYTEMSK(REG_SC_BK12_0F_L, stDBreg.u16DNRFetch, 0x1FFF);//dnr fetch
        SC_W2BYTEMSK(REG_SC_BK12_16_L, stDBreg.u16OPMOffset, 0x1FFF);//OPM offset
        SC_W2BYTEMSK(REG_SC_BK12_17_L, stDBreg.u16OPMFetch, 0x0FFF);//OPM FETCH
        SC_W2BYTEMSK(REG_SC_BK12_56_L, stDBreg.u16OPMOffset, 0x1FFF);//F1 OPM OFFSET
        SC_W2BYTEMSK(REG_SC_BK12_57_L, stDBreg.u16OPMFetch, 0x0FFF);//F1 OPM FETCH
        SC_W2BYTEMSK(REG_SC_BK12_30_L, 0x0001, 0x0001);// 3d enable
        SC_W2BYTEMSK(REG_SC_BK12_33_L, 0x0080, 0x0080);//OP 3D ENABLE
        SC_W2BYTEMSK(REG_SC_BK02_63_L, 0x0003, 0x0013);//hw 3d TYPE, L/R idx
#if (HW_DESIGN_3D_VER >= 3)
        SC_W2BYTEMSK(REG_SC_BK02_7F_L, pSrcInfo->stCapWin.width, 0x1FFF);

        if((stDBreg.u32V_PreScalingRatio & BIT(31))>>31)
            SC_W2BYTEMSK(REG_SC_BK02_09_L, BIT(12), BIT(12));
#endif
    }
    else if(IS_INPUT_SIDE_BY_SIDE_HALF(eWindow) || IS_INPUT_SIDE_BY_SIDE_HALF_INTERLACE(eWindow))
    {
        SC_3D_DBG(printf("## 3D input is SBSH\n"));

        SC_W2BYTEMSK(REG_SC_BK12_0E_L, stDBreg.u16DNROffset, 0x1FFF);//dnr offset
        SC_W2BYTEMSK(REG_SC_BK12_0F_L, stDBreg.u16DNRFetch, 0x1FFF);//dnr fetch
        SC_W2BYTEMSK(REG_SC_BK12_16_L, (stDBreg.u16OPMOffset/2 + 1) & ~0x1, 0x1FFF);//OPM offset
        SC_W2BYTEMSK(REG_SC_BK12_17_L, (stDBreg.u16OPMFetch  + 1) & ~0x1, 0x0FFF);//OPM FETCH
        SC_W2BYTEMSK(REG_SC_BK12_56_L, (stDBreg.u16OPMOffset/2 + 1) & ~0x1, 0x1FFF);//F1 OPM OFFSET
        SC_W2BYTEMSK(REG_SC_BK12_57_L, (stDBreg.u16OPMFetch  + 1) & ~0x1, 0x0FFF);//F1 OPM FETCH
        SC_W2BYTEMSK(REG_SC_BK02_63_L, 0x0103, 0x0103);//hw 3d TYPE
        SC_W2BYTEMSK(REG_SC_BK12_30_L, 0x0001, 0x0001);//IPM 3D ENABLE
        SC_W2BYTEMSK(REG_SC_BK12_33_L, 0x0080, 0x0080);//OP 3D ENABLE
        SC_W2BYTEMSK(REG_SC_BK12_18_L, 0x00, BIT(15));
        SC_W2BYTEMSK(REG_SC_BK12_18_L, pSrcInfo->stCapWin.height*2, 0x1FFF);
        SC_W2BYTEMSK(REG_SC_BK12_18_L, BIT(15), BIT(15));
#if (HW_DESIGN_3D_VER >= 3)
        SC_W2BYTEMSK(REG_SC_BK02_7F_L, pSrcInfo->stCapWin.width/2, 0x1FFF);
        if((stDBreg.u32V_PreScalingRatio & BIT(31))>>31)
            SC_W2BYTEMSK(REG_SC_BK02_09_L, BIT(12), BIT(12));
        //==> In new QMAP(after add 3D item), these patch need to remove
#if (HW_DESIGN_3D_VER >= 4)
        // TODO: QMAP patch
        #if 1
            //Disable MCDi
            SC_W2BYTEMSK(REG_SC_BK2A_02_L, 0x00, 0xE008); // [15:13]|[3]
            //Disable DMS_V12Line
            SC_W2BYTEMSK(REG_SC_BK20_10_L, 0x00, BIT(4)); // DMS_V12Line
            //Set SRAM to linear mode
            if(g_XC_Pnl_Misc.FRCInfo.bFRC)
                SC_W2BYTE(REG_SC_BK23_0B_L, 0x303);
            else
                SC_W2BYTE(REG_SC_BK23_0B_L, 0x44C4);
        #endif
#endif
#endif
    }
    else if(IS_INPUT_SIDE_BY_SIDE_FULL(eWindow))
    {
        SC_3D_DBG(printf("## 3D input is SBSF\n"));

        SC_W2BYTEMSK(REG_SC_BK12_0E_L, stDBreg.u16DNROffset, 0x1FFF);//dnr offset
        SC_W2BYTEMSK(REG_SC_BK12_0F_L, stDBreg.u16DNRFetch, 0x1FFF);//dnr fetch
        SC_W2BYTEMSK(REG_SC_BK12_16_L, (stDBreg.u16OPMOffset + 1) & ~0x1, 0x1FFF);//OPM offset
        SC_W2BYTEMSK(REG_SC_BK12_17_L, (stDBreg.u16OPMFetch  + 1) & ~0x1, 0x0FFF);//OPM FETCH
        SC_W2BYTEMSK(REG_SC_BK12_56_L, (stDBreg.u16OPMOffset + 1) & ~0x1, 0x1FFF);//F1 OPM OFFSET
        SC_W2BYTEMSK(REG_SC_BK12_57_L, (stDBreg.u16OPMFetch  + 1) & ~0x1, 0x0FFF);//F1 OPM FETCH
        SC_W2BYTEMSK(REG_SC_BK02_63_L, 0x0103, 0x0103);//hw 3d TYPE
        SC_W2BYTEMSK(REG_SC_BK12_30_L, 0x0001, 0x0001);//IPM 3D ENABLE
        SC_W2BYTEMSK(REG_SC_BK12_33_L, 0x0080, 0x0080);//OP 3D ENABLE
        SC_W2BYTEMSK(REG_SC_BK12_18_L, 0x00, BIT(15));
        SC_W2BYTEMSK(REG_SC_BK12_18_L, pSrcInfo->stCapWin.height*2, 0x1FFF);
        SC_W2BYTEMSK(REG_SC_BK12_18_L, BIT(15), BIT(15));
#if (HW_DESIGN_3D_VER >= 3)
        SC_W2BYTEMSK(REG_SC_BK02_7F_L, pSrcInfo->stCapWin.width/2, 0x1FFF);
        if((stDBreg.u32V_PreScalingRatio & BIT(31))>>31)
            SC_W2BYTEMSK(REG_SC_BK02_09_L, BIT(12), BIT(12));
        //==> In new QMAP(after add 3D item), these patch need to remove
#if (HW_DESIGN_3D_VER >= 4)
        // TODO: QMAP patch
        //Disable MCDi
        SC_W2BYTEMSK(REG_SC_BK2A_02_L, 0x00, 0xE008); // [15:13]|[3]
        //Disable DMS_V12Line
        SC_W2BYTEMSK(REG_SC_BK20_10_L, 0x00, BIT(4)); // DMS_V12Line
        //Set SRAM to linear mode
        SC_W2BYTE(REG_SC_BK23_0B_L, 0x303);
#endif
#endif
    }
#if (HW_DESIGN_3D_VER >= 4)
    else if(IS_INPUT_CHECK_BOARD(eWindow))
    {
        SC_3D_DBG(printf("## 3D input is CKB\n"));

        SC_W2BYTEMSK(REG_SC_BK12_0E_L, stDBreg.u16DNROffset, 0x1FFF);//dnr offset
        SC_W2BYTEMSK(REG_SC_BK12_0F_L, stDBreg.u16DNRFetch, 0x1FFF);//dnr fetch
        SC_W2BYTEMSK(REG_SC_BK12_16_L, (stDBreg.u16OPMOffset/2 + 1) & ~0x1, 0x1FFF);//OPM offset
        SC_W2BYTEMSK(REG_SC_BK12_17_L, (stDBreg.u16OPMFetch/2  + 1) & ~0x1, 0x0FFF);//OPM FETCH
        SC_W2BYTEMSK(REG_SC_BK12_56_L, (stDBreg.u16OPMOffset/2 + 1) & ~0x1, 0x1FFF);//F1 OPM OFFSET
        SC_W2BYTEMSK(REG_SC_BK12_57_L, (stDBreg.u16OPMFetch/2  + 1) & ~0x1, 0x0FFF);//F1 OPM FETCH

        SC_W2BYTEMSK(REG_SC_BK02_7F_L, pSrcInfo->stCapWin.width/2, 0x1FFF);
        SC_W2BYTEMSK(REG_SC_BK02_63_L, 0x0103, 0x0103);//hw 3d TYPE
        if((stDBreg.u32V_PreScalingRatio & BIT(31))>>31)
            SC_W2BYTEMSK(REG_SC_BK02_09_L, BIT(12), BIT(12));

        SC_W2BYTEMSK(REG_SC_BK02_6B_L, BIT(0)|BIT(2), BIT(0)|BIT(2)); //[0]pix_sep_en,[2]LR_Alt_line
        SC_W2BYTEMSK(REG_SC_BK02_6C_L, pSrcInfo->stCapWin.width/2, 0x7FF); //[10:0] pix_sep_half
        // TODO: Need to check how to setup resample
        SC_W2BYTE(REG_SC_BK02_6D_L, 0x8000); //resample phase0 coef0/1
        SC_W2BYTE(REG_SC_BK02_6E_L, 0x4000); //resample phase1 coef0/1
        SC_W2BYTEMSK(REG_SC_BK02_6F_L, 0x0109, 0x010F); //resample line phase, [8]enable

        SC_W2BYTEMSK(REG_SC_BK12_30_L, BIT(0), BIT(0)); //IPM 3D ENABLE
        SC_W2BYTEMSK(REG_SC_BK12_33_L, BIT(7), BIT(7)); //OP 3D ENABLE

#ifdef MSOS_TYPE_CE
		W2BYTEMSK1(REG_CLKGEN0_51_L,0x0000,0xF000);
#else
        W2BYTEMSK(REG_CLKGEN0_51_L,0x0000,0xF000);
#endif
        SC_W2BYTEMSK(REG_SC_BK12_18_L, 0x00, BIT(15));
        SC_W2BYTEMSK(REG_SC_BK12_18_L, pSrcInfo->stCapWin.height*2, 0x1FFF);
        SC_W2BYTEMSK(REG_SC_BK12_18_L, BIT(15), BIT(15));
#if 1
        //==> In new QMAP(after add 3D item), these patch need to remove
        // TODO: QMAP patch
        //Disable MCDi
        SC_W2BYTEMSK(REG_SC_BK2A_02_L, 0x00, 0xE008); // [15:13]|[3]
        //Disable DMS_V12Line
        SC_W2BYTEMSK(REG_SC_BK20_10_L, 0x00, BIT(4)); // DMS_V12Line
        //Set SRAM to linear mode
        SC_W2BYTE(REG_SC_BK23_0B_L, 0x303);
#endif
    }
#endif

    //below is output part
    if(IS_OUTPUT_FRAME_ALTERNATIVE())
    {
        SC_3D_DBG(printf("## 3D output is FA\n"));
        {
#if (HW_DESIGN_3D_VER == 4)
            SC_W2BYTEMSK(REG_SC_BK10_08_L, stDBreg.u16H_DisStart, 0x1FFF); // Display H start
            SC_W2BYTEMSK(REG_SC_BK10_0A_L, stDBreg.u16V_DisStart, 0x0FFF);

            if(g_XC_Pnl_Misc.FRCInfo.u83D_FI_out == E_XC_3D_OUTPUT_FI_960x1080)
            {
                SC_W2BYTEMSK(REG_SC_BK10_09_L, stDBreg.u16H_DisStart+(stDBreg.u16H_DisEnd-stDBreg.u16H_DisStart)/2, 0x1FFF);   // Display H end
                SC_W2BYTEMSK(REG_SC_BK10_0B_L, stDBreg.u16V_DisEnd, 0x0FFF);
                SC_W2BYTEMSK(REG_SC_BK10_05_L, (g_XC_InitData.stPanelInfo.u16HStart+g_XC_InitData.stPanelInfo.u16Width/2 - 1), 0x0FFF);
                SC_W2BYTEMSK(REG_SC_BK10_07_L, (g_XC_InitData.stPanelInfo.u16VStart+g_XC_InitData.stPanelInfo.u16Height - 1), 0x0FFF);
            }
            else if(g_XC_Pnl_Misc.FRCInfo.u83D_FI_out == E_XC_3D_OUTPUT_FI_1920x540)
            {
                SC_W2BYTEMSK(REG_SC_BK10_09_L, stDBreg.u16H_DisEnd, 0x1FFF);
                SC_W2BYTEMSK(REG_SC_BK10_0B_L, stDBreg.u16V_DisStart+(stDBreg.u16V_DisEnd-stDBreg.u16V_DisStart)/2, 0x0FFF);
                SC_W2BYTEMSK(REG_SC_BK10_05_L, (g_XC_InitData.stPanelInfo.u16HStart+g_XC_InitData.stPanelInfo.u16Width - 1), 0x0FFF);
                SC_W2BYTEMSK(REG_SC_BK10_07_L, (g_XC_InitData.stPanelInfo.u16VStart+g_XC_InitData.stPanelInfo.u16Height/2 - 1), 0x0FFF);
            }
            else
            {
                SC_W2BYTEMSK(REG_SC_BK10_09_L, stDBreg.u16H_DisEnd, 0x1FFF);
                SC_W2BYTEMSK(REG_SC_BK10_0B_L, stDBreg.u16V_DisEnd, 0x0FFF);
                SC_W2BYTEMSK(REG_SC_BK10_05_L, (g_XC_InitData.stPanelInfo.u16HStart+g_XC_InitData.stPanelInfo.u16Width - 1), 0x0FFF);
                SC_W2BYTEMSK(REG_SC_BK10_07_L, (g_XC_InitData.stPanelInfo.u16VStart+g_XC_InitData.stPanelInfo.u16Height - 1), 0x0FFF);
            }
#else
            //if output is FP, do not need sub window
            SC_W2BYTEMSK(REG_SC_BK10_08_L, stDBreg.u16H_DisStart, 0x1FFF); // Display H start
            SC_W2BYTEMSK(REG_SC_BK10_09_L, stDBreg.u16H_DisEnd, 0x1FFF);   // Display H end
            SC_W2BYTEMSK(REG_SC_BK10_0A_L, stDBreg.u16V_DisStart, 0x0FFF);
            SC_W2BYTEMSK(REG_SC_BK10_0B_L, stDBreg.u16V_DisEnd, 0x0FFF);
#endif
        }
        SC_3D_DBG(printf("## stDBreg.u16VLen =%x\n", stDBreg.u16VLen));
        if(IS_INPUT_TOP_BOTTOM(eWindow))
        {
            SC_W2BYTEMSK(REG_SC_BK20_15_L, stDBreg.u16VLen/2, 0x0FFF);//Set the maximum request lines for second channel
        }
        else
        {
            SC_W2BYTEMSK(REG_SC_BK20_15_L, stDBreg.u16VLen, 0x0FFF);//Set the maximum request lines for second channel
        }
        if(!_bSkipDefaultLRFlag)
        {
            Hal_XC_H3D_LR_Toggle_Enable(TRUE);//HDMI 3D field select toggle enable
        }
        else
        {
            SC_W2BYTEMSK(REG_SC_BK12_33_L, (TRUE<<1), (BIT(1)));
        }
        SC_W2BYTEMSK(REG_SC_BK20_1D_L, (stDBreg.u16LBOffset&0xFF), 0xFF);
        // TODO: need to double check
        SC_W2BYTEMSK(REG_SC_BK12_33_L, FALSE, BIT(0));
        //Set SRAM to linear mode
        if( g_XC_InitData.bMirror[eWindow] )
        {
           SC_W2BYTE(REG_SC_BK23_0B_L, 0x303);
        }

    }
    else if(IS_OUTPUT_TOP_BOTTOM())
    {
        SC_3D_DBG(printf("## 3D output is TBP\n"));
        //MS_U16 u16VactVideo, u16VactSpace;
        MDrv_SC_3D_GetFP_Info(pSrcInfo, &u16VactVideo, &u16VactSpace, DISABLE);

        if( g_XC_InitData.bMirror[eWindow] )
        {
#ifdef TBP_1920X2160_OUTPUT
            if((IS_INPUT_FRAME_PACKING(eWindow))&&
            ((pSrcInfo->stCapWin.height == DOUBLEHD_1080X2P_VSIZE)||
            (pSrcInfo->stCapWin.height == DOUBLEHD_1080X2I_VSIZE)))
            {
                SC_W2BYTEMSK(REG_SC_BK0F_07_L, stDBreg.u16H_DisStart, 0x1FFF); // Display H start
                SC_W2BYTEMSK(REG_SC_BK0F_08_L, stDBreg.u16H_DisEnd, 0x1FFF);   // Display H end
                SC_W2BYTEMSK(REG_SC_BK0F_09_L, stDBreg.u16V_DisStart, 0x0FFF);//Sub window display window
                SC_W2BYTEMSK(REG_SC_BK0F_0A_L, stDBreg.u16V_DisStart + pSrcInfo->stDispWin.height-1, 0x0FFF);
                SC_W2BYTEMSK(REG_SC_BK10_08_L, stDBreg.u16H_DisStart, 0x0FFF);//Main window display window
                SC_W2BYTEMSK(REG_SC_BK10_09_L, stDBreg.u16H_DisEnd, 0x0FFF);
                SC_W2BYTEMSK(REG_SC_BK10_0A_L, stDBreg.u16V_DisStart + pSrcInfo->stDispWin.height, 0x0FFF);
                SC_W2BYTEMSK(REG_SC_BK10_0B_L, stDBreg.u16V_DisEnd*2-1, 0x0FFF);
            }
            else
#endif
            {
                SC_W2BYTEMSK(REG_SC_BK0F_07_L, stDBreg.u16H_DisStart, 0x1FFF); // Display H start
                SC_W2BYTEMSK(REG_SC_BK0F_08_L, stDBreg.u16H_DisEnd, 0x1FFF);   // Display H end
                SC_W2BYTEMSK(REG_SC_BK0F_09_L, stDBreg.u16V_DisStart, 0x0FFF);//Sub window display window
                SC_W2BYTEMSK(REG_SC_BK0F_0A_L, stDBreg.u16V_DisStart + pSrcInfo->stDispWin.height/2-1, 0x0FFF);
                SC_W2BYTEMSK(REG_SC_BK10_08_L, stDBreg.u16H_DisStart, 0x0FFF);//Main window display window
                SC_W2BYTEMSK(REG_SC_BK10_09_L, stDBreg.u16H_DisEnd, 0x0FFF);
                SC_W2BYTEMSK(REG_SC_BK10_0A_L, stDBreg.u16V_DisStart + pSrcInfo->stDispWin.height/2, 0x0FFF);
                SC_W2BYTEMSK(REG_SC_BK10_0B_L, stDBreg.u16V_DisEnd, 0x0FFF);
            }

        }
        else
        {
#ifdef TBP_1920X2160_OUTPUT
            if((IS_INPUT_FRAME_PACKING(eWindow))&&
            ((pSrcInfo->stCapWin.height == DOUBLEHD_1080X2P_VSIZE)||
            (pSrcInfo->stCapWin.height == DOUBLEHD_1080X2I_VSIZE)))
            {
                SC_W2BYTEMSK(REG_SC_BK0F_07_L, stDBreg.u16H_DisStart, 0x0FFF);//Sub window display window
                SC_W2BYTEMSK(REG_SC_BK0F_08_L, stDBreg.u16H_DisEnd, 0x0FFF);
                SC_W2BYTEMSK(REG_SC_BK0F_09_L, stDBreg.u16V_DisStart + pSrcInfo->stDispWin.height, 0x0FFF);
                SC_W2BYTEMSK(REG_SC_BK0F_0A_L, stDBreg.u16V_DisEnd*2-1, 0x0FFF);
                SC_W2BYTEMSK(REG_SC_BK10_08_L, stDBreg.u16H_DisStart, 0x1FFF); // Display H start
                SC_W2BYTEMSK(REG_SC_BK10_09_L, stDBreg.u16H_DisEnd, 0x1FFF);   // Display H end
                SC_W2BYTEMSK(REG_SC_BK10_0A_L, stDBreg.u16V_DisStart, 0x0FFF);//Main window display window
                SC_W2BYTEMSK(REG_SC_BK10_0B_L, stDBreg.u16V_DisStart + pSrcInfo->stDispWin.height-1, 0x0FFF);
            }
            else
#endif
            {
                SC_W2BYTEMSK(REG_SC_BK0F_07_L, stDBreg.u16H_DisStart, 0x0FFF);//Sub window display window
                SC_W2BYTEMSK(REG_SC_BK0F_08_L, stDBreg.u16H_DisEnd, 0x0FFF);
                SC_W2BYTEMSK(REG_SC_BK0F_09_L, stDBreg.u16V_DisStart + pSrcInfo->stDispWin.height/2, 0x0FFF);
                SC_W2BYTEMSK(REG_SC_BK0F_0A_L, stDBreg.u16V_DisEnd, 0x0FFF);
                SC_W2BYTEMSK(REG_SC_BK10_08_L, stDBreg.u16H_DisStart, 0x1FFF); // Display H start
                SC_W2BYTEMSK(REG_SC_BK10_09_L, stDBreg.u16H_DisEnd, 0x1FFF);   // Display H end
                SC_W2BYTEMSK(REG_SC_BK10_0A_L, stDBreg.u16V_DisStart, 0x0FFF);//Main window display window
                SC_W2BYTEMSK(REG_SC_BK10_0B_L, stDBreg.u16V_DisStart + pSrcInfo->stDispWin.height/2-1, 0x0FFF);
            }
        }
        SC_W2BYTEMSK(REG_SC_BK12_33_L, BIT(11), BIT(11)); //h3d_opm, sub win follow main's read bank
        SC_W4BYTE(REG_SC_BK12_50_L, stDBreg.u32OPMBase0);

        SC_W2BYTEMSK(REG_SC_BK20_15_L, stDBreg.u16VLen, 0x0FFF);//Set the maximum request lines for second channel
        SC_W2BYTEMSK(REG_SC_BK20_16_L, stDBreg.u16VLen, 0x0FFF);//Set the maximum request lines for first channel
        SC_W2BYTEMSK(REG_SC_BK20_1D_L, (stDBreg.u16LBOffset&0xFF), 0xFF);
        SC_W2BYTEMSK(REG_SC_BK20_1D_L, (stDBreg.u16LBOffset&0xFF)<<8, 0xFF00);
        //Set SRAM to linear mode
        if( g_XC_InitData.bMirror[eWindow] )
        {
           SC_W2BYTE(REG_SC_BK23_0B_L, 0x303);
        }

        MDrv_XC_3D_CfgSubWin(ENABLE);
    }
    else if(IS_OUTPUT_LINE_ALTERNATIVE())
    {
        SC_3D_DBG(printf("## 3D output is LA\n"));
        SC_W2BYTEMSK(REG_SC_BK0F_07_L, stDBreg.u16H_DisStart, 0x0FFF);//Sub window display window
        SC_W2BYTEMSK(REG_SC_BK0F_08_L, stDBreg.u16H_DisEnd, 0x0FFF);
        SC_W2BYTEMSK(REG_SC_BK0F_09_L, stDBreg.u16V_DisStart, 0x0FFF);
        SC_W2BYTEMSK(REG_SC_BK0F_0A_L, stDBreg.u16V_DisEnd, 0x0FFF);

        SC_W2BYTEMSK(REG_SC_BK10_08_L, stDBreg.u16H_DisStart, 0x1FFF); // Display H start
        SC_W2BYTEMSK(REG_SC_BK10_09_L, stDBreg.u16H_DisEnd, 0x1FFF);   // Display H end
        SC_W2BYTEMSK(REG_SC_BK10_0A_L, stDBreg.u16V_DisStart, 0x0FFF);//Main window display window
        SC_W2BYTEMSK(REG_SC_BK10_0B_L, stDBreg.u16V_DisEnd, 0x0FFF);

        SC_W2BYTEMSK(REG_SC_BK12_33_L, BIT(11), BIT(11)); //h3d_opm, sub win follow main's read bank
        SC_W4BYTE(REG_SC_BK12_50_L, stDBreg.u32OPMBase0);//F1 OPM BASE0 sub

        SC_W2BYTEMSK(REG_SC_BK20_15_L, stDBreg.u16VLen, 0x0FFF);//Set the maximum request lines for second channel, added for protect
        SC_W2BYTEMSK(REG_SC_BK20_16_L, stDBreg.u16VLen, 0x0FFF);//Set the maximum request lines for first channel
        SC_W2BYTEMSK(REG_SC_BK23_53_L, 0x8000, 0x8000);// 3D LR side-by-side to line-to-line enable
        SC_W2BYTEMSK(REG_SC_BK20_1D_L, (stDBreg.u16LBOffset&0xFF), 0xFF);
        SC_W2BYTEMSK(REG_SC_BK20_1D_L, (stDBreg.u16LBOffset&0xFF)<<8, 0xFF00);
        MDrv_XC_3D_CfgSubWin(ENABLE);
    }
    else if(IS_OUTPUT_FRAME_L())
    {
        MDrv_XC_3D_CfgSubWin(DISABLE);
        SC_W2BYTEMSK(REG_SC_BK10_08_L, stDBreg.u16H_DisStart, 0x1FFF); // Display H start
        SC_W2BYTEMSK(REG_SC_BK10_09_L, stDBreg.u16H_DisEnd, 0x1FFF);   // Display H end
        SC_W2BYTEMSK(REG_SC_BK10_0A_L, stDBreg.u16V_DisStart, 0x0FFF);//Main window display window
        SC_W2BYTEMSK(REG_SC_BK10_0B_L, stDBreg.u16V_DisEnd, 0x0FFF);
        SC_W2BYTEMSK(REG_SC_BK20_15_L, stDBreg.u16VLen, 0x0FFF);//Set the maximum request lines for second channel, added for protect
        SC_W2BYTEMSK(REG_SC_BK20_1D_L, (stDBreg.u16LBOffset&0xFF), 0xFF);
    }
    else if(IS_OUTPUT_FRAME_R())
    {
        SC_W2BYTEMSK(REG_SC_BK0F_07_L, stDBreg.u16H_DisStart, 0x0FFF);//Sub window display window
        SC_W2BYTEMSK(REG_SC_BK0F_08_L, stDBreg.u16H_DisEnd, 0x0FFF);
        SC_W2BYTEMSK(REG_SC_BK0F_09_L, stDBreg.u16V_DisStart, 0x0FFF);
        SC_W2BYTEMSK(REG_SC_BK0F_0A_L, stDBreg.u16V_DisEnd, 0x0FFF);
        SC_W4BYTE(REG_SC_BK12_50_L, stDBreg.u32OPMBase0);//F1 OPM BASE0
        SC_W2BYTEMSK(REG_SC_BK20_16_L, stDBreg.u16VLen, 0x0FFF);//Set the maximum request lines for first channel
        SC_W2BYTEMSK(REG_SC_BK20_1D_L, (stDBreg.u16LBOffset&0xFF)<<8, 0xFF00);
        MDrv_XC_3D_CfgSubWin(ENABLE);
    }
    else if(IS_OUTPUT_SIDE_BY_SIDE_HALF())
    {
        if( g_XC_InitData.bMirror[eWindow] )
        {
            SC_W2BYTEMSK(REG_SC_BK0F_07_L, stDBreg.u16H_DisStart, 0x1FFF);
            SC_W2BYTEMSK(REG_SC_BK0F_08_L, stDBreg.u16H_DisStart + pSrcInfo->stDispWin.width/2-1, 0x1FFF);
            SC_W2BYTEMSK(REG_SC_BK0F_09_L, stDBreg.u16V_DisStart, 0x0FFF);//sub window display window
            SC_W2BYTEMSK(REG_SC_BK0F_0A_L, stDBreg.u16V_DisEnd, 0x0FFF);
            SC_W2BYTEMSK(REG_SC_BK10_08_L, stDBreg.u16H_DisStart + pSrcInfo->stDispWin.width/2, 0x0FFF);
            SC_W2BYTEMSK(REG_SC_BK10_09_L, stDBreg.u16H_DisEnd, 0x0FFF);
            SC_W2BYTEMSK(REG_SC_BK10_0A_L, stDBreg.u16V_DisStart, 0x0FFF);
            SC_W2BYTEMSK(REG_SC_BK10_0B_L, stDBreg.u16V_DisEnd, 0x0FFF);
        }
        else
        {
            SC_W2BYTEMSK(REG_SC_BK0F_07_L, stDBreg.u16H_DisStart + pSrcInfo->stDispWin.width/2, 0x0FFF);
            SC_W2BYTEMSK(REG_SC_BK0F_08_L, stDBreg.u16H_DisEnd, 0x0FFF);
            SC_W2BYTEMSK(REG_SC_BK0F_09_L, stDBreg.u16V_DisStart, 0x0FFF);
            SC_W2BYTEMSK(REG_SC_BK0F_0A_L, stDBreg.u16V_DisEnd, 0x0FFF);
            SC_W2BYTEMSK(REG_SC_BK10_08_L, stDBreg.u16H_DisStart, 0x1FFF);
            SC_W2BYTEMSK(REG_SC_BK10_09_L, stDBreg.u16H_DisStart + pSrcInfo->stDispWin.width/2-1, 0x1FFF);
            SC_W2BYTEMSK(REG_SC_BK10_0A_L, stDBreg.u16V_DisStart, 0x0FFF);//Main window display window
            SC_W2BYTEMSK(REG_SC_BK10_0B_L, stDBreg.u16V_DisEnd, 0x0FFF);
        }

        SC_W2BYTEMSK(REG_SC_BK12_33_L, BIT(11), BIT(11)); //h3d_opm, sub win follow main's read bank
        SC_W4BYTE(REG_SC_BK12_50_L, stDBreg.u32OPMBase0);

        SC_W2BYTEMSK(REG_SC_BK20_15_L, stDBreg.u16VLen, 0x0FFF);
        SC_W2BYTEMSK(REG_SC_BK20_16_L, stDBreg.u16VLen, 0x0FFF);
        SC_W2BYTEMSK(REG_SC_BK20_1D_L, (stDBreg.u16LBOffset&0xFF), 0xFF);
        SC_W2BYTEMSK(REG_SC_BK20_1D_L, (stDBreg.u16LBOffset&0xFF)<<8, 0xFF00);
        //Set SRAM to linear mode
        if( g_XC_InitData.bMirror[eWindow] )
        {
           SC_W2BYTE(REG_SC_BK23_0B_L, 0x303);
        }

        MDrv_XC_3D_CfgSubWin(ENABLE);
    }

    _XC_RETURN();
#endif

#if HW_2DTO3D_SUPPORT
    static MS_BOOL bHw2DTo3Dstate = DISABLE;
    //First init all control register to HW init value
    _XC_ENTRY();
    if(bHw2DTo3Dstate == ENABLE)
    {
        //detach vsync interrupt
        MDrv_XC_InterruptDeAttachWithoutMutex(SC_INT_VSINT, _MDrv_SC_3D_2DTo3D_DepthRefine, NULL);
        SC_W2BYTEMSK(REG_SC_BK15_6C_L, 0x0000, 0x0001);//Enable Depth Render
        SC_W2BYTEMSK(REG_SC_BK15_06_L, 0x0000, BIT(13)|BIT(15));//src domain select&start to write miu
        //dd
        SC_W2BYTEMSK(REG_SC_BK15_01_L, 0x0780, 0x07FF);//src width
        SC_W2BYTEMSK(REG_SC_BK15_02_L, 0x0438, 0x07FF);//src height
        SC_W2BYTEMSK(REG_SC_BK15_05_L, 0x7878, 0x7F7F);//dram request number
        //dr
        SC_W2BYTEMSK(REG_SC_BK15_64_L, 0x0000, 0x1FFF);//pixel width
        SC_W2BYTEMSK(REG_SC_BK15_60_L, 0x0000, 0x07FF);//MV width
        SC_W2BYTEMSK(REG_SC_BK15_62_L, 0x0000, 0x00FF);//horizontal number of 32x32 block
        SC_W2BYTEMSK(REG_SC_BK15_61_L, 0x0000, 0x007F);//vertical number of 32x32 block
        SC_W2BYTEMSK(REG_SC_BK15_62_L, 0x0F00, 0xFF00);//TOTAL FRAME
        SC_W2BYTEMSK(REG_SC_BK15_65_L, 0x0000, 0x00FF);//left depth offset
        SC_W2BYTEMSK(REG_SC_BK15_65_L, 0x0000, 0x3F00);//left depth gain
        SC_W2BYTEMSK(REG_SC_BK15_66_L, 0x0000, 0x00FF);//right depth offset
        SC_W2BYTEMSK(REG_SC_BK15_66_L, 0x0000, 0x3F00);//right depth gain
        SC_W2BYTEMSK(REG_SC_BK15_73_L, 0x8000, 0xBF3F);//dp info software reset, last number,request number
        SC_W2BYTEMSK(REG_SC_BK15_67_L, 0x0000, 0x0007);//Passive mode Select,1:Line alternative
    }

    if((IS_INPUT_MODE_NONE(eWindow) || IS_OUTPUT_MODE_NONE()))
    {
        //Close 3D, just need restore Mainwindow here
        if(bHw2DTo3Dstate == ENABLE)
        {
            SC_W2BYTEMSK(REG_SC_BK10_08_L, stDBreg.u16H_DisStart, 0x1FFF); // Display H start
            SC_W2BYTEMSK(REG_SC_BK10_09_L, stDBreg.u16H_DisEnd, 0x1FFF);   // Display H end
            SC_W2BYTEMSK(REG_SC_BK10_0A_L, stDBreg.u16V_DisStart, 0x0FFF);//Main window display window
            SC_W2BYTEMSK(REG_SC_BK10_0B_L, stDBreg.u16V_DisEnd, 0x0FFF);
            bHw2DTo3Dstate = DISABLE;
        }
        _XC_RETURN();
        return TRUE;
    }
    else
    {
        bHw2DTo3Dstate = ENABLE;
    }

    //input
    if(IS_INPUT_NORMAL_2D_HW(eWindow))
    {
        //clk
        MDrv_WriteRegBit(REG_CKG_FICLK_F1, ENABLE, BIT(6)); //reg_ckg_ficlk_3d
        //dd
        SC_W2BYTEMSK(REG_SC_BK15_01_L, pSrcInfo->stDispWin.width, 0x07FF);//src width
        SC_W2BYTEMSK(REG_SC_BK15_02_L, pSrcInfo->stDispWin.height, 0x07FF);//src height
        SC_W2BYTEMSK(REG_SC_BK15_05_L, 0x7878, 0x7F7F);//dram request number
        SC_W2BYTEMSK(REG_SC_BK15_06_L, (_u32HW2DTO3D_DD_Buf/BYTE_PER_WORD)>>16, 0x01FF);//dd buffer high 9bit
        SC_W2BYTE(REG_SC_BK15_07_L, _u32HW2DTO3D_DD_Buf/BYTE_PER_WORD);//dd buffer low 2 bytes
        SC_W2BYTE(REG_SC_BK15_74_L, _u32HW2DTO3D_DR_Buf/BYTE_PER_WORD);//dr buffer low 2 bytes
        SC_W2BYTEMSK(REG_SC_BK15_75_L, (_u32HW2DTO3D_DR_Buf/BYTE_PER_WORD)>>16, 0x01FF);//dr buffer high 9bit
        MDrv_XC_wait_output_vsync(1, 100, eWindow);
        SC_W2BYTEMSK(REG_SC_BK15_06_L, BIT(13)|BIT(15), BIT(13)|BIT(15));//src domain select&start to write miu
        //dr
        SC_W2BYTEMSK(REG_SC_BK15_64_L, pSrcInfo->stDispWin.width, 0x1FFF);//pixel width
        SC_W2BYTEMSK(REG_SC_BK15_60_L, (pSrcInfo->stDispWin.width+3)/4, 0x07FF);//MV width
        SC_W2BYTEMSK(REG_SC_BK15_62_L, (pSrcInfo->stDispWin.width+31)/32, 0x00FF);//horizontal number of 32x32 block
        SC_W2BYTEMSK(REG_SC_BK15_61_L, (pSrcInfo->stDispWin.height+31)/32, 0x007F);//vertical number of 32x32 block
        SC_W2BYTEMSK(REG_SC_BK15_62_L, 0x0F00, 0xFF00);//TOTAL FRAME
        SC_W2BYTEMSK(REG_SC_BK15_65_L, BIT(14), BIT(14));//left depth gain for debug
        SC_W2BYTEMSK(REG_SC_BK15_65_L, _st3DHw2DTo3DPara.u16Offset, 0x00FF);//left depth offset
        SC_W2BYTEMSK(REG_SC_BK15_65_L, _st3DHw2DTo3DPara.u16Gain<<8, 0x3F00);//left depth gain
        SC_W2BYTEMSK(REG_SC_BK15_66_L, _st3DHw2DTo3DPara.u16Offset, 0x00FF);//right depth offset
        SC_W2BYTEMSK(REG_SC_BK15_66_L, _st3DHw2DTo3DPara.u16Gain<<8, 0x3F00);//right depth gain
        SC_W2BYTEMSK(REG_SC_BK15_73_L, 0x0404, 0xBF3F);//dp info software reset, last number,request number
        SC_W2BYTEMSK(REG_SC_BK15_6C_L, 0x0001, 0x0001);//Enable Depth Render
#if HW_2DTO3D_PATCH
        _bIsHW2Dto3DPatchEnabled = Hal_SC_IsHW2Dto3DPatch_Enable();
#endif
        MDrv_XC_InterruptAttachWithoutMutex(SC_INT_VSINT, _MDrv_SC_3D_2DTo3D_DepthRefine, NULL);
    }
    else if((!IS_INPUT_MODE_NONE(eWindow)) && (!IS_INPUT_NORMAL_2D(eWindow))) //real 3d adjust depth using hw 2d to 3d function
    {
        SC_W2BYTEMSK(REG_SC_BK15_6C_L, 0x0001, 0x0001);//Enable Depth Render
        if((_u163DHShift == 0x80) || (_u163DHShift == 0x00))
        {
            SC_W2BYTEMSK(REG_SC_BK15_5D_L, 0x0000, 0x8000); //2  // 2nd offset enable
        }
        else if(_u163DHShift > 0x80)
        {
            SC_W2BYTEMSK(REG_SC_BK15_5D_L, 0x8000, 0x8000); //2  // 2nd offset enable
            SC_W2BYTE(REG_SC_BK15_5E_L, 0x0000); //2  // 2nd offset L
            SC_W2BYTE(REG_SC_BK15_5F_L, (_u163DHShift-0x80)<<8); //2  // 2nd offset R
        }
        else
        {
            SC_W2BYTEMSK(REG_SC_BK15_5D_L, 0x8000, 0x8000); //2  // 2nd offset enable
            SC_W2BYTE(REG_SC_BK15_5E_L, (0x80-_u163DHShift)<<8); //2  // 2nd offset L
            SC_W2BYTE(REG_SC_BK15_5F_L, 0x0000); //2  // 2nd offset R
        }
    }

    //output
    if(IS_OUTPUT_CHECKBOARD_HW())
    {
        SC_W2BYTEMSK(REG_SC_BK15_67_L, 0x0000, 0x0007);//Passive mode Select,0:Check Board
    }
    else if(IS_OUTPUT_LINE_ALTERNATIVE_HW())
    {
        SC_W2BYTEMSK(REG_SC_BK15_67_L, _u16HW2Dto3DCurrentLR, 0x0007);//Passive mode Select,1:Line Alternative
    }
    else if(IS_OUTPUT_PIXEL_ALTERNATIVE_HW())
    {
        SC_W2BYTEMSK(REG_SC_BK15_67_L, 0x0002, 0x0007);//Passive mode Select,2:Pixel Alternative
    }
    else if(IS_OUTPUT_FRAME_L_HW())
    {
        SC_W2BYTEMSK(REG_SC_BK15_67_L, 0x0006, 0x0007);//Passive mode Select,6:L Only
    }
    else if(IS_OUTPUT_FRAME_R_HW())
    {
        SC_W2BYTEMSK(REG_SC_BK15_67_L, 0x0007, 0x0007);//Passive mode Select,7:R Only
    }
    else if((!IS_OUTPUT_MODE_NONE()) && (!IS_OUTPUT_FRAME_ALTERNATIVE())) //real 3d adjust depth using hw 2d to 3d function
    {
        SC_W2BYTEMSK(REG_SC_BK15_67_L, 0x0001, 0x0007);//Passive mode Select,1:Line Alternative
    }

    _XC_RETURN();
#endif

    _XC_ENTRY();
    //below is moved from hal sw db
#ifdef MULTI_SCALER_SUPPORTED
	if(Hal_SC_is_extra_req_en(NULL, NULL, NULL, NULL, eWindow))
#else
    if(Hal_SC_is_extra_req_en(NULL, NULL, NULL, NULL))
#endif
    {
        SC_W2BYTEMSK(REG_SC_BK20_11_L, BIT(15), BIT(15));
    }
    else
    {
         SC_W2BYTEMSK(REG_SC_BK20_11_L, 0, BIT(15));
    }
    _XC_RETURN();

    return TRUE;
}

