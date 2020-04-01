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

#ifndef _MDRV_SC_IP_H_
#define _MDRV_SC_IP_H_

#include "mhal_ip.h"

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

#ifdef _MDRV_SC_IP_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif

typedef enum
{
    IP_ANALOG1 = 0x00,
    IP_ANALOG2 = 0x01,
    IP_ANALOG3 = 0x02,
    IP_DVI     = 0x03,
    IP_VIDEO   = 0x04,
    IP_HDTV    = 0x05,
    IP_HDMI    = 0x07,
} EN_ISELECT;

// This can be added in the future.
// FCLK have to be changed case by case.
typedef enum
{
    EN_FCLK_NORMAL,
    EN_FCLK_FBL,
} EN_SET_FCLK_CASE;

extern MS_U16 MDrv_XC_cal_pixclk(MS_U16 u16HTotal, SCALER_WIN eWindow);
extern void MDrv_XC_reset_ip(SCALER_WIN eWindow);

#define MDrv_SC_de_only_en  Hal_SC_de_only_en
INTERFACE void MDrv_SC_de_only_en(MS_BOOL benable, SCALER_WIN eWindow);

#define MDrv_SC_de_direct_en  Hal_SC_de_direct_en
INTERFACE void MDrv_SC_de_direct_en(MS_BOOL benable, SCALER_WIN eWindow);

#define MDrv_SC_set_delayline  Hal_SC_set_delayline
INTERFACE void MDrv_SC_set_delayline( MS_U8 u8DelayLines, SCALER_WIN eWindow );

#define MDrv_SC_set_Fclk  Hal_SC_set_Fclk
INTERFACE void MDrv_SC_set_Fclk( EN_SET_FCLK_CASE enCase );

#define MDrv_SC_setfield Hal_SC_setfield
INTERFACE void MDrv_SC_setfield( MS_U16 reg_1D, MS_U16 reg_21, MS_U16 reg_23, SCALER_WIN eWindow );

#define MDrv_SC_ip_set_input_source Hal_SC_ip_set_input_source
INTERFACE void MDrv_SC_ip_set_input_source( MS_U8 u8InputSrcSel, MS_U8 u8SyncSel, MS_U8 u8VideoSel, MS_U8 u8isYPbPr, SCALER_WIN eWindow );

#define MDrv_SC_ip_set_image_wrap Hal_SC_ip_set_image_wrap
INTERFACE void MDrv_SC_ip_set_image_wrap( MS_BOOL bHEnable, MS_BOOL bVEnable, SCALER_WIN eWindow );

#define MDrv_SC_ip_set_input_sync_reference_edge  Hal_SC_ip_set_input_sync_reference_edge
INTERFACE void MDrv_SC_ip_set_input_sync_reference_edge( MS_BOOL bHRef, MS_BOOL bVRef, SCALER_WIN eWindow );

#define MDrv_SC_ip_set_input_vsync_delay Hal_SC_ip_set_input_vsync_delay
INTERFACE void MDrv_SC_ip_set_input_vsync_delay( MS_BOOL bDelay, SCALER_WIN eWindow );

#define MDrv_SC_ip_set_de_only_mode Hal_SC_ip_set_de_only_mode
INTERFACE void MDrv_SC_ip_set_de_only_mode( MS_BOOL bEnable, SCALER_WIN eWindow );

#define MDrv_SC_ip_set_coast_input Hal_SC_ip_set_coast_input
INTERFACE void MDrv_SC_ip_set_coast_input( MS_BOOL bInputSel, SCALER_WIN eWindow );

#define MDrv_ip_set_input_10bit hal_ip_set_input_10bit
INTERFACE void MDrv_ip_set_input_10bit( MS_BOOL bInput10Bit, SCALER_WIN eWindow );

#define MDrv_SC_ip_set_ms_filter Hal_SC_ip_set_ms_filter
INTERFACE void MDrv_SC_ip_set_ms_filter(MS_BOOL bEnable, MS_U16 u16FilterRange, SCALER_WIN eWindow );

#define MDrv_SC_ip_set_coast_window Hal_SC_ip_set_coast_window
INTERFACE void MDrv_SC_ip_set_coast_window( MS_U8 u8Start, MS_U8 u8End, SCALER_WIN eWindow );

#define MDrv_SC_ip_set_input_sync_sample_mode Hal_SC_ip_set_input_sync_sample_mode
INTERFACE void MDrv_SC_ip_set_input_sync_sample_mode( MS_BOOL bMode, SCALER_WIN eWindow );

#define MDrv_SC_ip_set_de_bypass_mode Hal_SC_ip_set_de_bypass_mode
INTERFACE void Hal_SC_ip_set_de_bypass_mode( MS_BOOL bEnable, SCALER_WIN eWindow );

#define MDrv_SC_get_hsd_pixel_count Hal_SC_get_hsd_pixel_count
INTERFACE MS_U16 Hal_SC_get_hsd_pixel_count( SCALER_WIN eWindow );

#define MDrv_SC_get_vsd_line_count Hal_SC_get_vsd_line_count
INTERFACE MS_U16 Hal_SC_get_vsd_line_count( SCALER_WIN eWindow );

#define MDrv_SC_ip_set_post_glitch_removal Hal_SC_ip_set_post_glitch_removal
INTERFACE void MDrv_SC_ip_set_post_glitch_removal( MS_BOOL bEnble, MS_U8 u8Range, SCALER_WIN eWindow );

#define MDrv_SC_ip_set_DE_Mode_Glitch Hal_SC_ip_set_DE_Mode_Glitch
INTERFACE void MDrv_SC_ip_set_DE_Mode_Glitch(MS_U8 u8Setting , SCALER_WIN eWindow );

// Htt Vtt
INTERFACE MS_U16 Hal_SC_ip_get_horizontalPeriod(SCALER_WIN eWindow);
#define MDrv_SC_ip_get_horizontalPeriod  Hal_SC_ip_get_horizontalPeriod

INTERFACE MS_U16 Hal_SC_ip_get_verticaltotal(SCALER_WIN eWindow);
#define MDrv_SC_ip_get_verticaltotal     Hal_SC_ip_get_verticaltotal

#define MDrv_SC_ip_set_handshaking_md Hal_SC_ip_set_handshaking_md
INTERFACE void MDrv_SC_ip_set_handshaking_md(MS_U8 u8MD, SCALER_WIN eWindow);

INTERFACE MS_BOOL MDrv_SC_Check_IP_Gen_Timing(void);
INTERFACE void MDrv_XC_ip_get_sync_status(XC_IP_SYNC_STATUS *sXC_Sync_Status, E_MUX_INPUTPORT enInputPort , SCALER_WIN eWindow);
INTERFACE MS_U8 MDrv_XC_wait_output_vsync(MS_U8 u8NumVSyncs, MS_U16 u16Timeout, SCALER_WIN eWindow);
INTERFACE MS_U8 MDrv_XC_wait_input_vsync(MS_U8 u8NumVSyncs, MS_U16 u16Timeout, SCALER_WIN eWindow);
INTERFACE MS_U32 MDrv_XC_DelayAndWaitOutVsync(MS_U32 u32DelayTime, SCALER_WIN eWindow);
INTERFACE void MDrv_XC_HWAuto_SyncSeparate(SCALER_WIN eWindow);
INTERFACE MS_BOOL MApi_XC_IsAutoPositionResultReady(SCALER_WIN eWindow);
INTERFACE void MApi_XC_AutoPositionEnable(MS_U8 u8Enable , SCALER_WIN eWindow);
INTERFACE void    MDrv_XC_SetValidDataThreshold( MS_U8 u8Threshold, SCALER_WIN eWindow );
INTERFACE MS_U16  MApi_XC_SetAutoPositionVstart( SCALER_WIN eWindow );
INTERFACE MS_U16  MApi_XC_SetAutoPositionHstart( SCALER_WIN eWindow );
INTERFACE MS_U16  MApi_XC_SetAutoPositionVend( SCALER_WIN eWindow );
INTERFACE MS_U16  MApi_XC_SetAutoPositionHend( SCALER_WIN eWindow );
INTERFACE void    MApi_XC_VsyncPolarityDetectMode(MS_U8 u8Enable, SCALER_WIN eWindow );
INTERFACE void    MApi_XC_TimingChangeInit(SCALER_WIN eWindow);
INTERFACE MS_AUTOADC_TYPE MApi_XC_GetAverageDataFromMemory(MS_WINDOW_TYPE* mem_win);
INTERFACE MS_U8   MApi_XC_CalculateVsyncTime(MS_U16 u16VTotal, MS_U16 u16HPeriod);
INTERFACE void    MApi_XC_EnableIPAutoCoast(MS_BOOL bEnable);
INTERFACE MS_U8   MApi_XC_PCMonitor_GetVSyncTime(SCALER_WIN eWindow);
INTERFACE void MApi_XC_AutoRangeEnable(MS_U8 u8Enable , SCALER_WIN eWindow);
INTERFACE void MApi_XC_SetAutoRangeWindow(MS_U16 u16Vstart, MS_U16 u16Hstart, MS_U16 u16Vsize, MS_U16 u16Hsize, SCALER_WIN eWindow);
////===========
////get scaler de info from sc_ip
INTERFACE MS_U16 MDrv_SC_de_hend_info(void);
INTERFACE MS_U16 MDrv_SC_de_vend_info(void);

INTERFACE void MDrv_XC_init_riu_base(MS_U32 u32riu_base, MS_U32 u32PMriu_base);
//INTERFACE void MApi_XC_GetFrameData(XC_ApiStatus stSrcInfo, MS_RECT_INFO eRect_Info);
INTERFACE void MDrv_SC_SetFrameDataCore(MS_S16 x0, MS_S16 y0, MS_S16 x1, MS_S16 y1,
                              MS_U8 *pRect,
                              MS_U32 u32RectPitch,
                              MS_U32 u32RectSize,
                              MS_U16 u16PointSize);

INTERFACE void MApi_XC_Rerout_for_Capture_VOP_Data(void);

INTERFACE MS_U8 MDrv_XC_WaitOutputVSync(MS_U8 u8NumVSyncs, MS_U16 u16Timeout, SCALER_WIN eWindow);
INTERFACE void MDrv_XC_DisableInputSource(MS_BOOL bDisable, SCALER_WIN eWindow);
INTERFACE MS_BOOL MDrv_XC_IsInputSourceDisabled(SCALER_WIN eWindow);
INTERFACE void MDrv_XC_SoftwareReset(MS_U8 u8Reset, SCALER_WIN eWindow);
INTERFACE void MDrv_XC_GetSyncStatus(INPUT_SOURCE_TYPE_t eCurrentSrc, XC_IP_SYNC_STATUS *sXC_Sync_Status, SCALER_WIN eWindow);
INTERFACE MS_U16 MApi_XC_GetAutoGainResult(AUTO_GAIN_TYPE u8Type, MS_U8 u8DelayMS, SCALER_WIN eWindow);
INTERFACE MS_U8 MApi_XC_GetAutoGainMaxValueStatus(SCALER_WIN eWindow);
INTERFACE void MApi_XC_EuroHdtvHandler(SCALER_WIN eWindow);
INTERFACE MS_DETECT_EURO_HDTV_STATUS_TYPE MDrv_XC_ip_GetEuroHdtvStatus(SCALER_WIN eWindow);
INTERFACE MS_BOOL MApi_XC_IsInputSourceDisabled(SCALER_WIN eWindow);
INTERFACE void MApi_XC_SetFIRDownSampleDivider(MS_U8 u8Enable , MS_U16 u16OverSamplingCount , SCALER_WIN eWindow );
INTERFACE void MApi_XC_SetCaptureWindowVsize(MS_U16 u16Vsize , SCALER_WIN eWindow);
INTERFACE void MApi_XC_SetCaptureWindowHsize(MS_U16 u16Hsize , SCALER_WIN eWindow);

#define MDrv_SC_IP2_PreFilter_Enable  Hal_SC_IP2_PreFilter_Enable
INTERFACE void MDrv_SC_IP2_PreFilter_Enable(MS_BOOL bEnable);
INTERFACE void MApi_XC_InitRegInTimingChange_Post( MS_U8 u8CurrentSyncStatus );
INTERFACE MS_BOOL MApi_XC_GetDstInfo_IP1(MS_XC_DST_DispInfo *pDstInfo, MS_U32 u32SizeofDstInfo, SCALER_WIN eWindow);
INTERFACE void MDrv_XC_GetDEWidthHeightInDEByPassMode(MS_U16* pu16Width, MS_U16* pu16Height, SCALER_WIN eWindow);

#undef INTERFACE
#endif /* _MDRV_SC_IP_H_ */
