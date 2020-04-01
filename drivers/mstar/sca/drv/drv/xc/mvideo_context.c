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
/// file    Mvideo_context.c
/// @brief  TEMP Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////
#define  MVIDEO_CONTEXT_C
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
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

// Internal Definition

#include "drvXC_IOPort.h"
#include "apiXC.h"
#include "mvideo_context.h"
#include "drv_sc_scaling.h"
#include "drv_sc_display.h"
#include "drv_sc_ip.h"
#include "mhal_sc.h"
#include "xc_hwreg_utility2.h"
#include "xc_Analog_Reg.h"
#include "drv_sc_menuload.h"
#include "drv_sc_isr.h"
#include "mdrv_sc_3d.h"
#include "drvXC_HDMI_if.h"
#include "mhal_hdmi.h"
#include "drvXC_HDMI_Internal.h"
//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------
#define MSXC_WR_DBG(x)  //x
#define SC_3D_DBG(x)    //x

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
MS_U8 SC_VBuf[32] =  {
    'M','S','V','C','0','0',                //0,0 fixed
    'S','3',                                //LIB ID
    '0','0',                                //Interface
    '0','0','0','0',                        //Build number
    '0','0','0','0','0','0','0','0',        // Change List
    'A','E','0','0','0','0','0','0','0',    // AE:Saturn4LE  0: MStar Trunk
    'T'                                     // check sum
};



//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static E_HDMI_SYNC_TYPE s_HDMISYNC_type = HDMI_SYNC_DE;
MS_BOOL _bSupportHVMode = FALSE;


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
MS_U16 MDrv_SC_Cal_FRC_Output_Vfreq(MS_U16 u16InputVFreq,
                             MS_U16 u16DefaultVFreq,
                             MS_BOOL bFBL,
                             MS_U8 *u8FrcIn,
                             MS_U8 *u8FrcOut,
                             MS_BOOL *bSetFrameLock,
                             MS_U8 u8Lpll_bank,
                             SCALER_WIN eWin);


extern PQ_Function_Info    s_PQ_Function_Info;
//==============================================================================
//==============================================================================

void mvideo_sc_variable_init(MS_BOOL bFirstInstance, XC_INITDATA *pXC_InitData)
{
    MS_U8 i, j;
    XC_PANEL_INFO_EX stPanelInfoEx;

    // context
#if(XC_CONTEXT_ENABLE)
    g_pSContext->bEnableFPLLManualDebug = FALSE;
    if (bFirstInstance)
    {
        gSrcInfo[MAIN_WINDOW].Status2.u16OutputVFreqAfterFRC = pXC_InitData->stPanelInfo.u16DefaultVFreq;
        gSrcInfo[MAIN_WINDOW].stDispWin.x       = 0;
        gSrcInfo[MAIN_WINDOW].stDispWin.y       = 0;
        gSrcInfo[MAIN_WINDOW].stDispWin.width   = 0;
        gSrcInfo[MAIN_WINDOW].stDispWin.height  = 0;

        gSrcInfo[MAIN_WINDOW].bEn3DNR = FALSE;
        gSrcInfo[MAIN_WINDOW].bUseYUVSpace = FALSE;
        gSrcInfo[MAIN_WINDOW].bMemYUVFmt = FALSE;
        gSrcInfo[MAIN_WINDOW].bForceNRoff = FALSE;
        gSrcInfo[MAIN_WINDOW].bFBL = FALSE;
        gSrcInfo[MAIN_WINDOW].bR_FBL = FALSE;
        gSrcInfo[MAIN_WINDOW].bForceRGBin = FALSE;
        gSrcInfo[MAIN_WINDOW].bFastFrameLock = FALSE;//mvideo_sc_set_fastframelock(FALSE);
        gSrcInfo[MAIN_WINDOW].bRWBankAuto= TRUE;

        gSrcInfo[MAIN_WINDOW].u16BytePerWord   = BYTE_PER_WORD;
        gSrcInfo[MAIN_WINDOW].u16OffsetPixelAlignment = OFFSET_PIXEL_ALIGNMENT;
        gSrcInfo[MAIN_WINDOW].Status2.bAutoBestPreScaling = FALSE;
        gSrcInfo[MAIN_WINDOW].Status2.enOutputColorType = E_XC_OUTPUT_COLOR_NONE;
        gSrcInfo[MAIN_WINDOW].bInterlace = FALSE;
        gSrcInfo[MAIN_WINDOW].bBlackscreenEnabled = FALSE;

		#ifdef MULTI_SCALER_SUPPORTED
        gSrcInfo[SC1_MAIN_WINDOW].Status2.u16OutputVFreqAfterFRC = pXC_InitData->stPanelInfo.u16DefaultVFreq;
        gSrcInfo[SC1_MAIN_WINDOW].stDispWin.x       = 0;
        gSrcInfo[SC1_MAIN_WINDOW].stDispWin.y       = 0;
        gSrcInfo[SC1_MAIN_WINDOW].stDispWin.width   = 0;
        gSrcInfo[SC1_MAIN_WINDOW].stDispWin.height  = 0;

        gSrcInfo[SC1_MAIN_WINDOW].bEn3DNR = FALSE;
        gSrcInfo[SC1_MAIN_WINDOW].bUseYUVSpace = FALSE;
        gSrcInfo[SC1_MAIN_WINDOW].bMemYUVFmt = FALSE;
        gSrcInfo[SC1_MAIN_WINDOW].bForceNRoff = FALSE;
        gSrcInfo[SC1_MAIN_WINDOW].bFBL = FALSE;
        gSrcInfo[SC1_MAIN_WINDOW].bR_FBL = FALSE;
        gSrcInfo[SC1_MAIN_WINDOW].bForceRGBin = FALSE;
        gSrcInfo[SC1_MAIN_WINDOW].bFastFrameLock = FALSE;//mvideo_sc_set_fastframelock(FALSE);
        gSrcInfo[SC1_MAIN_WINDOW].bRWBankAuto= TRUE;

        gSrcInfo[SC1_MAIN_WINDOW].u16BytePerWord   = BYTE_PER_WORD;
        gSrcInfo[SC1_MAIN_WINDOW].u16OffsetPixelAlignment = OFFSET_PIXEL_ALIGNMENT;
        gSrcInfo[SC1_MAIN_WINDOW].Status2.bAutoBestPreScaling = FALSE;
        gSrcInfo[SC1_MAIN_WINDOW].Status2.enOutputColorType = E_XC_OUTPUT_COLOR_NONE;
        gSrcInfo[SC1_MAIN_WINDOW].bInterlace = FALSE;
        gSrcInfo[SC1_MAIN_WINDOW].bBlackscreenEnabled = FALSE;

        gSrcInfo[SC2_MAIN_WINDOW].Status2.u16OutputVFreqAfterFRC = pXC_InitData->stPanelInfo.u16DefaultVFreq;
        gSrcInfo[SC2_MAIN_WINDOW].stDispWin.x       = 0;
        gSrcInfo[SC2_MAIN_WINDOW].stDispWin.y       = 0;
        gSrcInfo[SC2_MAIN_WINDOW].stDispWin.width   = 0;
        gSrcInfo[SC2_MAIN_WINDOW].stDispWin.height  = 0;

        gSrcInfo[SC2_MAIN_WINDOW].bEn3DNR = FALSE;
        gSrcInfo[SC2_MAIN_WINDOW].bUseYUVSpace = FALSE;
        gSrcInfo[SC2_MAIN_WINDOW].bMemYUVFmt = FALSE;
        gSrcInfo[SC2_MAIN_WINDOW].bForceNRoff = FALSE;
        gSrcInfo[SC2_MAIN_WINDOW].bFBL = FALSE;
        gSrcInfo[SC2_MAIN_WINDOW].bR_FBL = FALSE;
        gSrcInfo[SC2_MAIN_WINDOW].bForceRGBin = FALSE;
        gSrcInfo[SC2_MAIN_WINDOW].bFastFrameLock = FALSE;//mvideo_sc_set_fastframelock(FALSE);
        gSrcInfo[SC2_MAIN_WINDOW].bRWBankAuto= TRUE;

        gSrcInfo[SC2_MAIN_WINDOW].u16BytePerWord   = BYTE_PER_WORD;
        gSrcInfo[SC2_MAIN_WINDOW].u16OffsetPixelAlignment = OFFSET_PIXEL_ALIGNMENT;
        gSrcInfo[SC2_MAIN_WINDOW].Status2.bAutoBestPreScaling = FALSE;
        gSrcInfo[SC2_MAIN_WINDOW].Status2.enOutputColorType = E_XC_OUTPUT_COLOR_NONE;
        gSrcInfo[SC2_MAIN_WINDOW].bInterlace = FALSE;
        gSrcInfo[SC2_MAIN_WINDOW].bBlackscreenEnabled = FALSE;
		#endif

        gSrcInfo[SUB_WINDOW].stDispWin.x       = 0;
        gSrcInfo[SUB_WINDOW].stDispWin.y       = 0;
        gSrcInfo[SUB_WINDOW].stDispWin.width   = 0;
        gSrcInfo[SUB_WINDOW].stDispWin.height  = 0;

        gSrcInfo[SUB_WINDOW].bEn3DNR = FALSE;
        gSrcInfo[SUB_WINDOW].bUseYUVSpace = FALSE;
        gSrcInfo[SUB_WINDOW].bForceNRoff = FALSE;
        gSrcInfo[SUB_WINDOW].bMemYUVFmt = FALSE;
        gSrcInfo[SUB_WINDOW].bFBL = FALSE;
        gSrcInfo[SUB_WINDOW].bForceRGBin = FALSE;
        gSrcInfo[SUB_WINDOW].bFastFrameLock = FALSE;
        gSrcInfo[SUB_WINDOW].bRWBankAuto= TRUE;

        gSrcInfo[SUB_WINDOW].u16BytePerWord   = BYTE_PER_WORD;
        gSrcInfo[SUB_WINDOW].u16OffsetPixelAlignment = OFFSET_PIXEL_ALIGNMENT;
        gSrcInfo[SUB_WINDOW].Status2.bAutoBestPreScaling = FALSE;
        gSrcInfo[SUB_WINDOW].Status2.enOutputColorType = E_XC_OUTPUT_COLOR_NONE;
        gSrcInfo[SUB_WINDOW].bInterlace = FALSE;
        gSrcInfo[SUB_WINDOW].bBlackscreenEnabled = FALSE;

		#ifdef MULTI_SCALER_SUPPORTED
        gSrcInfo[SC2_SUB_WINDOW].stDispWin.x       = 0;
        gSrcInfo[SC2_SUB_WINDOW].stDispWin.y       = 0;
        gSrcInfo[SC2_SUB_WINDOW].stDispWin.width   = 0;
        gSrcInfo[SC2_SUB_WINDOW].stDispWin.height  = 0;

        gSrcInfo[SC2_SUB_WINDOW].bEn3DNR = FALSE;
        gSrcInfo[SC2_SUB_WINDOW].bUseYUVSpace = FALSE;
        gSrcInfo[SC2_SUB_WINDOW].bForceNRoff = FALSE;
        gSrcInfo[SC2_SUB_WINDOW].bMemYUVFmt = FALSE;
        gSrcInfo[SC2_SUB_WINDOW].bFBL = FALSE;
        gSrcInfo[SC2_SUB_WINDOW].bForceRGBin = FALSE;
        gSrcInfo[SC2_SUB_WINDOW].bFastFrameLock = FALSE;
        gSrcInfo[SC2_SUB_WINDOW].bRWBankAuto= TRUE;

        gSrcInfo[SC2_SUB_WINDOW].u16BytePerWord   = BYTE_PER_WORD;
        gSrcInfo[SC2_SUB_WINDOW].u16OffsetPixelAlignment = OFFSET_PIXEL_ALIGNMENT;
        gSrcInfo[SC2_SUB_WINDOW].Status2.bAutoBestPreScaling = FALSE;
        gSrcInfo[SC2_SUB_WINDOW].Status2.enOutputColorType = E_XC_OUTPUT_COLOR_NONE;
        gSrcInfo[SC2_SUB_WINDOW].bInterlace = FALSE;
        gSrcInfo[SC2_SUB_WINDOW].bBlackscreenEnabled = FALSE;
		#endif
    }
#endif
    //save panel default timing
    memcpy(&(gSrcInfo[MAIN_WINDOW].Status2.stXCPanelDefaultTiming), &(pXC_InitData->stPanelInfo), sizeof(XC_PANEL_TIMING));
    memset(&g_XC_Init_Misc, 0, sizeof(XC_INITMISC));

    g_bEnableRepWindowForFrameColor = FALSE;

    MDrv_SC_3D_Variable_Init();

    // interrupts
    for(i=0; i<MAX_SC_INT; i++)
    {
        for(j=0; j<MAX_ISR_NUM_OF_EACH_INT; j++)
        {
            stXCIsrInfo[i][j].aryXC_ISR_Param = NULL;
            stXCIsrInfo[i][j].aryXC_ISR = NULL;
        }
    }
    memset(&stPanelInfoEx, 0, sizeof(XC_PANEL_INFO_EX));
    stPanelInfoEx.u16VFreq = PANEL_INFO_EX_INVALID_ALL;
    stPanelInfoEx.u32PanelInfoEx_Version = PANEL_INFO_EX_VERSION;
    stPanelInfoEx.u16PanelInfoEX_Length = sizeof(XC_PANEL_INFO_EX);
    MApi_XC_SetExPanelInfo(FALSE, &stPanelInfoEx);

    _XC_ENTRY();
    MDrv_XC_GetDefaultHVSyncInfo();
    _XC_RETURN();
}

void MDrv_XC_SetHdmiSyncMode(E_HDMI_SYNC_TYPE eSynctype)
{
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"set HDMI sync mode mode: %u (0 is DE mode, 1 is HV mode)\n",eSynctype);
    s_HDMISYNC_type = eSynctype;
}

void MApi_XC_SetHdmiSyncMode(E_HDMI_SYNC_TYPE eSynctype)
{
    //Whether we should support special timing with HV sync mode. Some timing, like 720@30p must use HV mode
    //  to detect it. so this API is actually to enable or disable the capablity for these special timing.
    //  DE mode: not support special timing, HV mode: support special timing
#if SUPPORT_IP_HDMI_FOR_HV_MODE
    if(HDMI_SYNC_HV == eSynctype)
    {
        _bSupportHVMode = TRUE;
    }
    else
#endif
    {
        _bSupportHVMode = FALSE;
    }
}

E_HDMI_SYNC_TYPE MApi_XC_GetHdmiSyncMode(void)
{
    //Check whether which mode are we using: DE or HV.
    return s_HDMISYNC_type;
}

void MDrv_XC_Switch_DE_HV_Mode_By_Timing(SCALER_WIN eWindow)
{
#if SUPPORT_IP_HDMI_FOR_HV_MODE
#if ONLY_ALLOW_HV_MODE_FOR_720P
    MS_U16 u16HDE = 0;
    MS_U8 u8InputSrcSel, u8SyncSel, u8VideoSel=0, u8isYPbPr;

    if(_bSupportHVMode)
    {
        _XC_ENTRY();
        u16HDE = MDrv_HDMI_GetHDE();
        //printf("---------------------MDrv_HDMI_GetHDE=%u\n",u16HDE);
        if((u16HDE >= 1200) && (u16HDE <= 1360))
        {
            if(HDMI_SYNC_HV != MApi_XC_GetHdmiSyncMode())
            {
                MDrv_XC_SetHdmiSyncMode(HDMI_SYNC_HV);

                MDrv_SC_ip_set_input_vsync_delay( NO_DELAY, eWindow );
                MDrv_SC_ip_set_de_only_mode( DISABLE, eWindow );
                // Use DE as capture window
                MDrv_SC_ip_set_de_bypass_mode(ENABLE, eWindow);
                // Disable scaling for get correct DE.
                //MDrv_SC_Enable_PreScaling(FALSE, FALSE, eWindow);
                // Set full range capture size for de-bypass mode
                //MDrv_SC_SetFullRangeCapture(eWindow);
                MDrv_SC_ip_set_input_sync_sample_mode( GLITCH_REMOVAL, eWindow );
                MDrv_SC_ip_set_post_glitch_removal( ENABLE, 0x01, eWindow );
                Hal_HDMI_Set_YUV422to444_Bypass(FALSE);

                //720p with IP_HDMI, color space problem. must set it to IP_VIDEO
                u8InputSrcSel = IP_VIDEO;
                u8VideoSel = IP_MST_VD_A;
                u8SyncSel = (AUTO_DETECT << 1) | CSYNC;
                u8isYPbPr = TRUE;
                MDrv_SC_ip_set_input_source( u8InputSrcSel, u8SyncSel, u8VideoSel, u8isYPbPr, eWindow);
                MDrv_XC_reset_ip( eWindow );
            }
        }
        else
        {
            if(HDMI_SYNC_DE != MApi_XC_GetHdmiSyncMode())
            {
                MDrv_XC_SetHdmiSyncMode(HDMI_SYNC_DE);

                MDrv_SC_ip_set_input_vsync_delay( DELAY_QUARTER_HSYNC, eWindow );
                MDrv_SC_ip_set_de_only_mode( ENABLE, eWindow );
                MDrv_SC_ip_set_de_bypass_mode(DISABLE, eWindow);
                MDrv_SC_ip_set_input_sync_sample_mode( NORMAL_MODE, eWindow );
                MDrv_SC_ip_set_post_glitch_removal( ENABLE, 0x07, eWindow );
                Hal_HDMI_Set_YUV422to444_Bypass(TRUE);

                //720p with IP_HDMI, color space problem. must set it to IP_VIDEO
                //here should recover the setting of 720p
                u8InputSrcSel = IP_HDMI;
                u8VideoSel = IP_HDMI;
                u8SyncSel = (AUTO_DETECT << 1) | CSYNC;
                u8isYPbPr = TRUE;
                MDrv_SC_ip_set_input_source( u8InputSrcSel, u8SyncSel, u8VideoSel, u8isYPbPr, eWindow);
                MDrv_XC_reset_ip( eWindow );
            }
        }
        _XC_RETURN();
    }
#endif
#endif
}

void mvideo_sc_getcropwin( MS_WINDOW_TYPE *pCropWin, SCALER_WIN eWindow )
{
    pCropWin->x = gSrcInfo[eWindow].stCropWin.x;
    pCropWin->y = gSrcInfo[eWindow].stCropWin.y;
    pCropWin->width = gSrcInfo[eWindow].stCropWin.width;
    pCropWin->height = gSrcInfo[eWindow].stCropWin.height;
}

MS_U8  mvideo_pnl_is_dualport(void)
{
    return g_XC_InitData.stPanelInfo.u8LPLL_Mode; // 0: single mode, 1: dual mode
}

void mvideo_pnl_set_dclkmode(E_XC_PNL_OUT_TIMING_MODE enPnl_Out_Timing_Mode)
{
    g_XC_InitData.stPanelInfo.enPnl_Out_Timing_Mode = enPnl_Out_Timing_Mode;
}

MS_BOOL mvideo_sc_is_enable_3dnr( SCALER_WIN eWindow )
{
    return gSrcInfo[eWindow].bEn3DNR;
}

void mvideo_sc_set_3dnr( MS_BOOL bEnable,  SCALER_WIN eWindow )
{
    gSrcInfo[eWindow].bEn3DNR = bEnable;
}

//-------------------------------------------------------------------------------------------------
/// check whether the specific window is YUV space
/// @param  eWindow                \b IN: which window we are going to set
/// @return @ref MS_BOOL
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_IsYUVSpace( SCALER_WIN eWindow )
{
    return gSrcInfo[eWindow].bUseYUVSpace;
}

//-------------------------------------------------------------------------------------------------
/// Enable/Disable force RGB in
/// @param  bEnable                 \b IN: enable or disable
/// @param  eWindow                 \b IN: which window we are going to enable or disable
//-------------------------------------------------------------------------------------------------
void MApi_XC_EnableForceRGBin(MS_BOOL bEnable, SCALER_WIN eWindow)
{
    gSrcInfo[eWindow].bForceRGBin = bEnable;
}

//-------------------------------------------------------------------------------------------------
/// This function will enable/diable framebufferless mode
/// @param  bEnable      \b IN: TRUE: enable, FALSE: otherwise
//-------------------------------------------------------------------------------------------------
#ifdef MULTI_SCALER_SUPPORTED
void MApi_XC_EnableFrameBufferLess(MS_BOOL bEnable, SCALER_WIN eWindow)
{
    gSrcInfo[MAIN_WINDOW].bFBL = bEnable;
    _XC_ENTRY();
    MDrv_SC_set_fbl(bEnable, eWindow);
    _XC_RETURN();
}
#else
void MApi_XC_EnableFrameBufferLess(MS_BOOL bEnable)
{
    gSrcInfo[MAIN_WINDOW].bFBL = bEnable;
    _XC_ENTRY();
    MDrv_SC_set_fbl(bEnable);
    _XC_RETURN();
}
#endif
//-------------------------------------------------------------------------------------------------
/// Query whether current XC is framebufferless or not
/// @return @ref MS_BOOL
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_IsCurrentFrameBufferLessMode(void)
{
    //gSrcInfo[MAIN_WINDOW].bFBL = MDrv_SC_get_fbl();
    return gSrcInfo[MAIN_WINDOW].bFBL;
}

//-------------------------------------------------------------------------------------------------
/// This function will enable/diable request framebufferless mode
/// @param  bEnable      \b IN: TRUE: enable, FALSE: otherwise
//-------------------------------------------------------------------------------------------------
void MApi_XC_EnableRequest_FrameBufferLess(MS_BOOL bEnable)
{
    if(Hal_SC_Detect_RequestFBL_Mode())
        gSrcInfo[MAIN_WINDOW].bR_FBL = bEnable;
    else
        printf("Not support\n");
}

//-------------------------------------------------------------------------------------------------
/// Query whether current XC is request framebufferless or not
/// @return @ref MS_BOOL
//-------------------------------------------------------------------------------------------------
#ifdef MULTI_SCALER_SUPPORTED
MS_BOOL MApi_XC_IsCurrentRequest_FrameBufferLessMode(SCALER_WIN eWindow)
{
    return gSrcInfo[eWindow].bR_FBL;
}
#else
MS_BOOL MApi_XC_IsCurrentRequest_FrameBufferLessMode(void)
{
    return gSrcInfo[MAIN_WINDOW].bR_FBL;
}
#endif

MS_U16 mvideo_sc_get_output_vfreq(void)
{
    return (MDrv_SC_GetOutputVFreqX100()/10);
}

//-------------------------------------------------------------------------------------------------
/// check whether specific window is memory format 422
/// @param  eWindow                \b IN: which window we are going to query
/// @return @ref MS_BOOL
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_IsMemoryFormat422( SCALER_WIN eWindow )
{
    return gSrcInfo[eWindow].bMemFmt422;
}

//-------------------------------------------------------------------------------------------------
/// Set XC mirror mode
/// @param  bEnable                \b IN: enable or disable XC mirror mode
//-------------------------------------------------------------------------------------------------
void MApi_XC_EnableMirrorMode( MS_BOOL bEnable, SCALER_WIN eWindow )
{
    _XC_ENTRY();
    g_XC_InitData.bMirror[eWindow] = bEnable;
    MDrv_SC_set_mirror( bEnable, eWindow );
    _XC_RETURN();
}

MirrorMode_t MApi_XC_GetMirrorModeType( SCALER_WIN eWindow )
{
    if ( g_XC_InitData.bMirror[eWindow])
    {
        return MIRROR_HV;
    }
    else
    {
        return MIRROR_NORMAL;
    }

}

//-------------------------------------------------------------------------------------------------
/// Set XC mirror mode
/// @param  bEnable                \b IN: enable or disable XC mirror mode
//-------------------------------------------------------------------------------------------------
void MApi_XC_EnableMirrorMode2( MirrorMode_t eMirrorMode, SCALER_WIN eWindow )
{
	DrvMirrorMode_t eDrvMirrorMode;
    _XC_ENTRY();

    eDrvMirrorMode = DRV_MIRROR_NORMAL;

    if ( eMirrorMode == MIRROR_NORMAL )
    {
        g_XC_InitData.bMirror[eWindow] = FALSE;
        eDrvMirrorMode = DRV_MIRROR_NORMAL;
    }
    else if ( eMirrorMode == MIRROR_H_ONLY)
    {
        g_XC_InitData.bMirror[eWindow] = FALSE;
        eDrvMirrorMode = DRV_MIRROR_H_ONLY;
    }
    else if ( eMirrorMode == MIRROR_V_ONLY)
    {
        g_XC_InitData.bMirror[eWindow] = TRUE;
        eDrvMirrorMode = DRV_MIRROR_V_ONLY;
    }
    else if ( eMirrorMode == MIRROR_HV )
    {
        g_XC_InitData.bMirror[eWindow] = TRUE;
        eDrvMirrorMode = DRV_MIRROR_HV;
    }
    else
    {
        printf("Unsupport MirrorMode Type\n ");
        _XC_RETURN();
    }

    MDrv_SC_set_mirror2(eDrvMirrorMode, eWindow);
#if 0
    MDrv_SC_set_mirror2( eDrvMirrorMode, MAIN_WINDOW);
    MDrv_SC_set_mirror2( eDrvMirrorMode, SUB_WINDOW);
	#ifdef MULTI_SCALER_SUPPORTED
    MDrv_SC_set_mirror2( eDrvMirrorMode, SC1_MAIN_WINDOW);
    MDrv_SC_set_mirror2( eDrvMirrorMode, SC2_MAIN_WINDOW);
    MDrv_SC_set_mirror2( eDrvMirrorMode, SC2_SUB_WINDOW);
	#endif
#endif

    _XC_RETURN();
}


//-------------------------------------------------------------------------------------------------
/// let display video freeze by disable scaler buffer write
/// @param  bEnable        \b IN: TRUE: enable, FALSE: otherwise
/// @param  eWindow        \b IN: which window we are going to set
//-------------------------------------------------------------------------------------------------
void MApi_XC_FreezeImg(MS_BOOL bEnable, SCALER_WIN eWindow)
{

    PQ_WIN ePQWin;

    if(gSrcInfo[eWindow].bFBL)
    {
        return; //not support freeze for FBL mode
    }
    ePQWin = (eWindow == MAIN_WINDOW)     ? PQ_MAIN_WINDOW :
             (eWindow == SUB_WINDOW)      ? PQ_SUB_WINDOW :
			 #ifdef MULTI_SCALER_SUPPORTED
             (eWindow == SC1_MAIN_WINDOW) ? PQ_SC1_MAIN_WINDOW :
             (eWindow == SC2_MAIN_WINDOW) ? PQ_SC2_MAIN_WINDOW :
             (eWindow == SC2_SUB_WINDOW)  ? PQ_SC2_SUB_WINDOW :
			 #endif
                                            PQ_MAIN_WINDOW;
    if(bEnable)
    {
        _XC_ENTRY();

        if(MApi_XC_MLoad_GetStatus(eWindow) == E_MLOAD_ENABLED)
            Hal_SC_set_freezeimg_burst(bEnable, eWindow);
        else
            Hal_SC_set_freezeimg(bEnable, eWindow);
        _XC_RETURN();

        //warning: This function is already protected with mutex
        if(s_PQ_Function_Info.pq_disable_filmmode)
            s_PQ_Function_Info.pq_disable_filmmode(ePQWin, bEnable);
        //

        _XC_ENTRY();
        MDrv_XC_wait_input_vsync(2, 50 , eWindow);
        _XC_RETURN();
    }
    else
    {
        //warning: This function is already protected with mutex
        if(s_PQ_Function_Info.pq_disable_filmmode)
            s_PQ_Function_Info.pq_disable_filmmode(ePQWin, bEnable);
        //

        _XC_ENTRY();
        if(MApi_XC_MLoad_GetStatus(eWindow) == E_MLOAD_ENABLED)
            Hal_SC_set_freezeimg_burst(bEnable, eWindow);
        else
            Hal_SC_set_freezeimg(bEnable, eWindow);
        _XC_RETURN();
    }
}

//-------------------------------------------------------------------------------------------------
/// Query whether the specific window is freeze image status or not
/// @param  eWindow                \b IN: which window we are going to query
/// @return @ref MS_BOOL
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_IsFreezeImg(SCALER_WIN eWindow)
{
    MS_BOOL bXCFreezeImg;
    _XC_ENTRY();

    bXCFreezeImg = Hal_SC_get_freezeimg(eWindow);
    //return Hal_SC_get_freezeimg(eWindow);
    _XC_RETURN();
    return bXCFreezeImg;
}

void MDrv_XC_GetInfo(XC_ApiInfo *pXC_Info)
{
    pXC_Info->u8MaxWindowNum = MAX_WINDOW_NUM;
    pXC_Info->u8NumOfDigitalDDCRam = NUM_OF_DIGITAL_DDCRAM;
    pXC_Info->u8MaxFrameNumInMem = MAX_FRAME_NUM_IN_MEM;
    pXC_Info->u8MaxFieldNumInMem = MAX_FIELD_NUM_IN_MEM;
}

void MDrv_XC_ShowTrace_Header(MS_U16 u16LogSwitch)
{
    printf("[XC,");
    if (u16LogSwitch & XC_DBGLEVEL_SETTIMING)
        printf("LPLL]");
    else if (u16LogSwitch & XC_DBGLEVEL_SETWINDOW)
        printf("SetWindow]");
    else if (u16LogSwitch & XC_DBGLEVEL_MUX)
        printf("Mux]");
    else if (u16LogSwitch & XC_DGBLEVEL_CROPCALC)
        printf("Cropping]");
    else if (u16LogSwitch & XC_DBGLEVEL_AUTOGEOMETRY)
        printf("AUTOGEO]");
    else
        printf("Unknown]");
}

//-------------------------------------------------------------------------------------------------
/// General Read Byte Function
/// @param  u32Reg                \b IN: which value shouldn't contain xc sub bank and address
/// @return @ref MS_U8
//-------------------------------------------------------------------------------------------------
MS_U8   MApi_XC_ReadByte(MS_U32 u32Reg)
{return (MS_U8)MDrv_ReadByte( u32Reg );}

//-------------------------------------------------------------------------------------------------
/// General Write Byte Function
/// @param  u32Reg                \b IN: which value shouldn't contain xc sub bank and address
/// @param  u8Val                \b IN: setting value
//-------------------------------------------------------------------------------------------------
void   MApi_XC_WriteByte(MS_U32 u32Reg, MS_U8 u8Val)
{MDrv_WriteByte( u32Reg, u8Val );}

//-------------------------------------------------------------------------------------------------
/// General 2 Write Byte Function
/// @param  u32Reg                \b IN: which value shouldn't contain xc sub bank and address
/// @param  u16Val                \b IN: setting value
/// @param  u16Msk                \b IN: setting value
//-------------------------------------------------------------------------------------------------
void MApi_XC_Write2ByteMask(MS_U32 u32Reg, MS_U16 u16val, MS_U16 u16Mask)
{MDrv_Write2ByteMask(u32Reg, u16val, u16Mask);}

//-------------------------------------------------------------------------------------------------
/// General Write Byte Mask Function
/// @param  u32Reg                \b IN: which value shouldn't contain xc sub bank and address
/// @param  u8Val                \b IN: setting value
/// @param  u8Msk                \b IN: decide which bit is valied
//-------------------------------------------------------------------------------------------------
void    MApi_XC_WriteByteMask(MS_U32 u32Reg, MS_U8 u8Val, MS_U8 u8Msk)
{MDrv_WriteByteMask( u32Reg, u8Val, u8Msk );}


//-------------------------------------------------------------------------------------------------
/// Scaler Write 2 Byte Function, and use mutex to protect while switch bank and write address
/// @param  u32Reg                \b IN: which value contain xc sub bank and address
/// @param  u16Val                \b IN: setting value
//-------------------------------------------------------------------------------------------------
void    MApi_XC_W2BYTE(MS_U32 u32Reg, MS_U16 u16Val)
{
    MSXC_WR_DBG(printf("W2 BK= %x, REG = %x, Val =%x\n", (MS_U16)((u32Reg&0xFF00)>>16), (MS_U16)(u32Reg&0x00FF), u16Val));
    _XC_ENTRY();
    SC_W2BYTE( u32Reg, u16Val);
    _XC_RETURN();
}

//-------------------------------------------------------------------------------------------------
/// Scaler Read 2 Byte Function, and use mutex to protect while switch bank and write address
/// @param  u32Reg                \b IN: which value contain xc sub bank and address
/// @return @ref MS_U16
//-------------------------------------------------------------------------------------------------
MS_U16  MApi_XC_R2BYTE(MS_U32 u32Reg)
{
    MS_U16 u16tmp;
    MSXC_WR_DBG(printf("R2 BK= %x, REG = %x\n", (MS_U16)((u32Reg&0xFF00)>>16), (MS_U16)(u32Reg&0x00FF)));
    _XC_ENTRY();
    u16tmp = (MS_U16)SC_R2BYTE( u32Reg );
    _XC_RETURN();
    return u16tmp;
}

//-------------------------------------------------------------------------------------------------
/// Scaler Write 4 Byte Function, and use mutex to protect while switch bank and write address
/// @param  u32Reg                \b IN: which value contain xc sub bank and address
/// @param  u32Val                \b IN: setting value
//-------------------------------------------------------------------------------------------------
void    MApi_XC_W4BYTE(MS_U32 u32Reg, MS_U32 u32Val)
{
    MSXC_WR_DBG(printf("W4 BK= %x, REG = %x, Val =%lx\n", (MS_U16)((u32Reg&0xFF00)>>16), (MS_U16)(u32Reg&0x00FF), u32Val));
    _XC_ENTRY();
    SC_W4BYTE( u32Reg, u32Val);
    _XC_RETURN();
}

//-------------------------------------------------------------------------------------------------
/// Scaler Read 4 Byte Function, and use mutex to protect while switch bank and write address
/// @param  u32Reg                \b IN: which value contain xc sub bank and address
/// @return @ref MS_U32
//-------------------------------------------------------------------------------------------------
MS_U32  MApi_XC_R4BYTE(MS_U32 u32Reg)
{
    MS_U32 u32tmp;
    MSXC_WR_DBG(printf("R4 BK= %x, REG = %x\n", (MS_U16)((u32Reg&0xFF00)>>16), (MS_U16)(u32Reg&0x00FF)));
    _XC_ENTRY();
    u32tmp = (MS_U32)SC_R4BYTE( u32Reg );
    _XC_RETURN();
    return u32tmp;

}

//-------------------------------------------------------------------------------------------------
/// Scaler Read 2 Byte Mask Function, and use mutex to protect while switch bank and write address
/// @param  u32Reg                \b IN: which value contain xc sub bank and address
/// @param  u16mask              \b IN: decide which bit is valied
/// @return @ref MS_U16
//-------------------------------------------------------------------------------------------------
MS_U16  MApi_XC_R2BYTEMSK(MS_U32 u32Reg, MS_U16 u16mask)
{
    MS_U16 u16tmp;
    //MS_U8  u8OldBank;

    MSXC_WR_DBG(printf("R2M BK= %x, REG = %x\n", (MS_U16)((u32Reg&0xFF00)>>16), (MS_U16)(u32Reg&0x00FF)));
    _XC_ENTRY();

    //u8OldBank = MDrv_ReadByte(BK_SELECT_00);

    u16tmp = (MS_U16)SC_R2BYTEMSK( u32Reg, u16mask);

    //MDrv_WriteByte(BK_SELECT_00, u8OldBank);

    _XC_RETURN();
    return u16tmp;
}

//-------------------------------------------------------------------------------------------------
/// Scaler Write 2 Byte Mask Function, and use mutex to protect while switch bank and write address
/// @param  u32Reg                \b IN: which value contain xc sub bank and address
/// @param  u16Val              \b IN: Setting value
/// @param  u16mask              \b IN: decide which bit is valied
//-------------------------------------------------------------------------------------------------
void    MApi_XC_W2BYTEMSK(MS_U32 u32Reg, MS_U16 u16Val, MS_U16 u16mask)
{
    //MS_U8 u8OldBank;

    MSXC_WR_DBG(printf("W2M BK= %x, REG = %x\n", (MS_U16)((u32Reg&0xFF00)>>16), (MS_U16)(u32Reg&0x00FF)));
    _XC_ENTRY();

    //u8OldBank = MDrv_ReadByte(BK_SELECT_00);

    SC_W2BYTEMSK( u32Reg, u16Val, u16mask);

    //MDrv_WriteByte(BK_SELECT_00, u8OldBank);

    _XC_RETURN();
}

#if FPGA
void mvideo_set_pnl_info_fpga(XC_PANEL_INFO *pPanel_Info)
{
    g_XC_InitData.stPanelInfo.u16HStart = pPanel_Info->u16HStart;
    g_XC_InitData.stPanelInfo.u16Width = pPanel_Info->u16Width;
    g_XC_InitData.stPanelInfo.u16VStart = pPanel_Info->u16VStart;
    g_XC_InitData.stPanelInfo.u16Height = pPanel_Info->u16Height;
}
#endif

void MApi_XC_EnableRepWindowForFrameColor(MS_BOOL bEnable)
{
    _XC_ENTRY();

    g_bEnableRepWindowForFrameColor = bEnable;

#if 0
    if( bEnable == FALSE )
    {
        SC_W2BYTEMSK(REG_SC_BK0F_57_L, 0, BIT(8)|BIT(9) ); // Disable display rep window
        SC_W2BYTEMSK(REG_SC_BK0F_42_L, 0, 0xFFFF);
        SC_W2BYTEMSK(REG_SC_BK0F_43_L, 0, 0xFFFF);
        SC_W2BYTEMSK(REG_SC_BK0F_44_L, 0, 0xFFFF);
        SC_W2BYTEMSK(REG_SC_BK0F_45_L, 0, 0xFFFF);
    }
#endif
    _XC_RETURN();
}

// scmi type 0: A1/A7/T12/T13, 1: M10/J2/A5/A6/A3
MS_BOOL MDrv_XC_Get_SCMI_Type(void)
{
    return _FIELD_PACKING_MODE_SUPPORTED;
}


void MApi_XC_Get_PanelInfo(XC_PANEL_INFO *pPanelInfo)
{
    memcpy(pPanelInfo, &g_XC_InitData.stPanelInfo, sizeof(XC_PANEL_INFO));

}


void MApi_XC_Set_PanelInfo(XC_PANEL_INFO *pPanelInfo)
{
    memcpy(&g_XC_InitData.stPanelInfo, pPanelInfo, sizeof(XC_PANEL_INFO));
}


