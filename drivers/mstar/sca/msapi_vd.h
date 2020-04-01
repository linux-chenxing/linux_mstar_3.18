// $Change: 640114 $
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (MStar Confidential Information!¡L) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __MSAPI_VD_H__
#define __MSAPI_VD_H__

#define AVD_DBG_EN  0

#if AVD_DBG_EN
#define  AVD_KDBG(_fmt, _args...)   printk(KERN_WARNING _fmt, ## _args)
#else
#define  AVD_KDBG(_fmt, _args...)
#endif



#include "drvAVD.h"
#include "drvPQ.h"

#if !defined(VD_GAIN_OF_RF_SEL)
#define VD_GAIN_OF_RF_SEL                       VD_USE_AUTO_GAIN
#endif
#if !defined(VD_GAIN_OF_AV_SEL)
#define VD_GAIN_OF_AV_SEL                       VD_USE_AUTO_GAIN
#endif


#if !defined(VD_GAIN_OF_RF)
#define VD_GAIN_OF_RF                       0x58
#endif

#if !defined(VD_GAIN_OF_AV)
#define VD_GAIN_OF_AV                       0x58
#else
#endif

#if !defined(VD_COLOR_KILL_HIGH_BOUND)
#define VD_COLOR_KILL_HIGH_BOUND                       0x18
#endif
#if !defined(VD_COLOR_KILL_LOW_BOUND)
#define VD_COLOR_KILL_LOW_BOUND                       0x0E
#endif
#ifdef ENABLE_VD_PACH_IN_CHINA
#define BK_AFEC_D4_DEFAULT  0x00
#define BK_AFEC_D5_DEFAULT  0x60
#define BK_AFEC_D8_DEFAULT  0x20
#define BK_AFEC_D9_DEFAULT  0x00
#endif

#define VD_STABLE_COUNT             0
#define VD_START_DETECT_COUNT       1
#define ATV_DETECT_THRESHOLD        8
#define CVBS_DETECT_THRESHOLD       4
#define SIGNAL_CHANGE_THRESHOLD     10

//------------------------------------
//  Misc.
//------------------------------------
/// SCART Source type
typedef enum
{
    E_SCART_SRC_TYPE_CVBS,         ///< SCART source type - CVBS
    E_SCART_SRC_TYPE_RGB,          ///< SCART source type - RGB
    E_SCART_SRC_TYPE_SVIDEO,      ///< SCART source type - SVIDEO
    E_SCART_SRC_TYPE_UNKNOWN      ///< SCART source type - Unknown
} SCART_SOURCE_TYPE;

typedef enum
{
    E_VIDEO_PERMANENT_MUTEOFF                = 0x00,
    E_VIDEO_PERMANENT_MUTEON                 = 0x01,
    E_VIDEO_MOMENT_MUTEOFF                   = 0x10,
    E_VIDEO_MOMENT_MUTEON                    = 0x11,
    E_VIDEO_BYLOCK_MUTEOFF                   = 0x20,
    E_VIDEO_BYLOCK_MUTEON                    = 0x21,
    E_VIDEO_BYPARENTAL_MUTEOFF               = 0x30,
    E_VIDEO_BYPARENTAL_MUTEON                = 0x31,
    E_VIDEO_DURING_LIMITED_TIME_MUTEOFF      = 0x40,
    E_VIDEO_DURING_LIMITED_TIME_MUTEON       = 0x41
} VIDEOMUTETYPE;

/// Aspect ratio type
typedef enum
{
    ARC4x3_FULL,                        ///< Aspect ratio 4:3 Full
    ARC14x9_LETTERBOX_CENTER,           ///< Aspect ratio 14:9 letterbox center
    ARC14x9_LETTERBOX_TOP,              ///< Aspect ratio 14:9 letterbox TOP
    ARC16x9_LETTERBOX_CENTER,           ///< Aspect ratio 16:9 letterbox center
    ARC16x9_LETTERBOX_TOP,              ///< Aspect ratio 16:9 letterbox TOP
    ARC_ABOVE16x9_LETTERBOX_CENTER,     ///< Aspect ratio Above 16:9 letterbox center
    ARC14x9_FULL_CENTER,                ///< Aspect ratio 14:9 full center
    ARC16x9_ANAMORPHIC,                 ///< Aspect ratio 16:9 anamorphic
    ARC_INVALID                         ///< Invalid Aspect ratio
} ASPECT_RATIO_TYPE;

typedef enum
{
    TTX_DISPLAY_INVALID_MODE,
    TTX_DISPLAY_UPDATE_MODE,
    TTX_DISPLAY_PICTURE_MODE,
    TTX_DISPLAY_MIX_MODE,
    TTX_DISPLAY_TEXT_MODE,
    TTX_DISPLAY_LOCK_MODE,
    TTX_DISPLAY_SUBTITLE_MODE
} TELETEXT_DISPLAY_MODE;

///Get sync type
typedef enum
{
    GET_SYNC_DIRECTLY,  ///< 0:get sync directly
    GET_SYNC_STABLE,    ///< 1:get sync stable
    GET_SYNC_VIRTUAL,   ///< 2: get sync virtually
} EN_GET_SYNC_TYPE;

/// color standard select
typedef enum
{
    SIG_NTSC,           ///< NTSC
    SIG_PAL,            ///< PAL
    SIG_SECAM,          ///< SECAM
    SIG_NTSC_443,       ///< NTSC 443
    SIG_PAL_M,          ///< PAL M
    SIG_PAL_NC,         ///< PAL NC
    SIG_NUMS,           ///< signal numbers

    SIG_NONE = -1
}EN_VD_SIGNALTYPE;

// Operation method
typedef enum
{
    OPERATIONMETHOD_MANUALLY,        ///< Operation method manually
    OPERATIONMETHOD_AUTOMATICALLY   ///< Operation method automatically
} OPERATIONMETHOD;

typedef enum
{
    E_DETECTION_VERIFY,
    E_DETECTION_START,
    E_DETECTION_WAIT,
    E_DETECTION_DETECT
} STANDARD_DETECTION_STATE;

// Signal Swing (just used in normal mode)
#define VD_SIG_SWING_THRESH                        2

#define MSVD_HTOTAL_TYPE AVD_PATCH_HTOTAL_1135 //AVD_PATCH_HTOTAL_DYNAMIC //

#define MAIN_LOOP_INTERVAL          10    // ms
#define WAIT_0ms        (0/MAIN_LOOP_INTERVAL)
#define WAIT_10ms       (10/MAIN_LOOP_INTERVAL)
#define WAIT_50ms       (50/MAIN_LOOP_INTERVAL)
#define WAIT_70ms       (70/MAIN_LOOP_INTERVAL)
#define WAIT_90ms       (90/MAIN_LOOP_INTERVAL)     // It is already used in TuningProcessor.
#define WAIT_110ms      (110/MAIN_LOOP_INTERVAL)    // It is already used in VideoProcessor.
#define WAIT_120ms      (120/MAIN_LOOP_INTERVAL)
#define WAIT_130ms      (130/MAIN_LOOP_INTERVAL)    // It is already used in AudioProcessor.
#define WAIT_150ms    (150/MAIN_LOOP_INTERVAL)
#define WAIT_170ms    (170/MAIN_LOOP_INTERVAL)    // It is already used in VideoProcessor.
#define WAIT_190ms    (190/MAIN_LOOP_INTERVAL)    // It is already used in TuningProcessor.
#define WAIT_230ms    (230/MAIN_LOOP_INTERVAL)
#define WAIT_290ms    (290/MAIN_LOOP_INTERVAL)    // It is already used in TuningProcessor.
#define WAIT_310ms    (310/MAIN_LOOP_INTERVAL)    // It is already used in VBIDataProcessor.
#define WAIT_370ms    (370/MAIN_LOOP_INTERVAL)    // It is already used in TuningProcessor.
#define WAIT_410ms    (410/MAIN_LOOP_INTERVAL)    // It is already used in Notification.
#define WAIT_430ms    (430/MAIN_LOOP_INTERVAL)    // It is already used in VideoProcessor.
#define WAIT_470ms    (470/MAIN_LOOP_INTERVAL)    // It is already used in VBIDataProcessor.
#define WAIT_500ms    (500/MAIN_LOOP_INTERVAL)    // It is already used in TuningProcessor.
#define WAIT_510ms    (510/MAIN_LOOP_INTERVAL)    // It is already used in VideoProcessor.
#define WAIT_530ms    (530/MAIN_LOOP_INTERVAL)    // It is already used in VideoProcessor.
#define WAIT_590ms    (590/MAIN_LOOP_INTERVAL)    // It is already used in TuningProcessor.
#define WAIT_610ms    (610/MAIN_LOOP_INTERVAL)    // It is already used in VideoProcessor.
#define WAIT_670ms    (670/MAIN_LOOP_INTERVAL)
#define WAIT_700ms    (700/MAIN_LOOP_INTERVAL)    // It is already used in TuningProcessor.
#define WAIT_770ms    (770/MAIN_LOOP_INTERVAL)    // It is already used in TuningProcessor.
#define WAIT_900ms    (900/MAIN_LOOP_INTERVAL)    // It is already used in TuningProcessor.
#define WAIT_1100ms    (1100/MAIN_LOOP_INTERVAL)    // It is already used in TuningProcessor.
#define WAIT_1230ms    (1230/MAIN_LOOP_INTERVAL)    // It is already used in VBIDataProcessor.
#define WAIT_1830ms    (1830/MAIN_LOOP_INTERVAL)    // It is already used in TuningProcessor.
#define WAIT_1970ms    (1970/MAIN_LOOP_INTERVAL)    //
#define WAIT_3000ms    (3000/MAIN_LOOP_INTERVAL)    // It is already used in VBIDataProcessor.
#define WAIT_5000ms    (5000/MAIN_LOOP_INTERVAL)    // It is already used in VBIDataProcessor.
#define WAIT_MAXms    ((0xFFFF/MAIN_LOOP_INTERVAL)*MAIN_LOOP_INTERVAL-MAIN_LOOP_INTERVAL)


#if (MSVD_HTOTAL_TYPE==AVD_PATCH_HTOTAL_1135_MUL_15)
    #define MSVD_HSTART_NTSC            0xE4
    #define MSVD_HSTART_PAL             0xE6
    #define MSVD_HSTART_SECAM           0xca//0xE6
    #define MSVD_HSTART_NTSC_443        0xDF
    #define MSVD_HSTART_PAL_M           0x7D
    #define MSVD_HSTART_PAL_NC          0x83//0xEA

    #define MSVD_VSTART_NTSC            0x12// 0x0A
    #define MSVD_VSTART_PAL             0x0D //0x1B//0x0D
    #define MSVD_VSTART_SECAM           0x0A
    #define MSVD_VSTART_NTSC_443        0x06
    #define MSVD_VSTART_PAL_M           0x09
    #define MSVD_VSTART_PAL_NC          0x10

    #define VD_HT_NTSC                  (1135L*3/2)
    #define VD_HT_PAL                   (1135L*3/2)
    #define VD_HT_SECAM                 (1135L*3/2)
    #define VD_HT_NTSC_443              (1135L*3/2)
    #define VD_HT_PAL_M                 (1135L*3/2)
    #define VD_HT_PAL_NC                (1135L*3/2)
    #define VD_HT_PAL_60                VD_HT_NTSC_443
#elif (MSVD_HTOTAL_TYPE==AVD_PATCH_HTOTAL_1135)
  #if 0
    #define MSVD_HSTART_NTSC            (MS_U16)0xF5*2/3
    #define MSVD_HSTART_PAL             (MS_U16)0xED*2/3
    #define MSVD_HSTART_SECAM           (MS_U16)0xE6*2/3
    #define MSVD_HSTART_NTSC_443        (MS_U16)0xDF*2/3
    #define MSVD_HSTART_PAL_M           (MS_U16)0x7D*2/3
    #define MSVD_HSTART_PAL_NC          (MS_U16)0x83*2/3 //0xEA

    #define MSVD_HSTART_NTSC            83
    #define MSVD_HSTART_PAL             82
    #define MSVD_HSTART_SECAM           77
    #define MSVD_HSTART_NTSC_443        75
    #define MSVD_HSTART_PAL_M           83
    #define MSVD_HSTART_PAL_NC          80//0xEA

    #define MSVD_VSTART_NTSC            0x12// 0x0A
    #define MSVD_VSTART_PAL             0x0D //0x1B//0x0D
    #define MSVD_VSTART_SECAM           0x0A
    #define MSVD_VSTART_NTSC_443        0x06
    #define MSVD_VSTART_PAL_M           0x09
    #define MSVD_VSTART_PAL_NC          0x10
#else
// calibrated with TG39 Marker pattern, full hd panel, 20100203
    #define MSVD_HSTART_NTSC            0x6B //96
    #define MSVD_HSTART_PAL             0x72 //97// 99
    #define MSVD_HSTART_SECAM           105	 // TODO: need fintune //
    #define MSVD_HSTART_NTSC_443        90	// TODO: need fintune //
    #define MSVD_HSTART_PAL_M           91	// TODO: need fintune //
    #define MSVD_HSTART_PAL_NC          90	// TODO: need fintune // //0xEA

    #define MSVD_VSTART_NTSC            0x08
    #define MSVD_VSTART_PAL             0x0F
    #define MSVD_VSTART_SECAM           0x0D
    #define MSVD_VSTART_NTSC_443        0x07
    #define MSVD_VSTART_PAL_M           0x08
    #define MSVD_VSTART_PAL_NC          0x10
#endif
    #define VD_HT_NTSC                  (1135L)
    #define VD_HT_PAL                   (1135L)
    #define VD_HT_SECAM                 (1135L)
    #define VD_HT_NTSC_443              (1135L)
    #define VD_HT_PAL_M                 (1135L)
    #define VD_HT_PAL_NC                (1135L)
    #define VD_HT_PAL_60                VD_HT_NTSC_443
#elif (MSVD_HTOTAL_TYPE==AVD_PATCH_HTOTAL_DYNAMIC)
    #define MSVD_HSTART_NTSC            0x34//0x49//0xF5
    #define MSVD_HSTART_PAL             102//0x70//0xED
    #define MSVD_HSTART_SECAM           102//0x7C//0xE6
    #define MSVD_HSTART_NTSC_443        0x34//0x49//0xDF
    #define MSVD_HSTART_PAL_M           102//0x70//0x7D
    #define MSVD_HSTART_PAL_NC          102//0x7C//0x83//0xEA

    #define MSVD_VSTART_NTSC            0x08//0x05
    #define MSVD_VSTART_PAL             0x0E //0x1B//0x0D
    #define MSVD_VSTART_SECAM           0x0E
    #define MSVD_VSTART_NTSC_443        0x08//0x05
    #define MSVD_VSTART_PAL_M           0x0E
    #define MSVD_VSTART_PAL_NC          0x0E

    #define VD_HT_NTSC                  (910L)
    #define VD_HT_PAL                   (1135L)
    #define VD_HT_SECAM                 (1097L)
    #define VD_HT_NTSC_443              (1127L)
    #define VD_HT_PAL_M                 (909L)
    #define VD_HT_PAL_NC                (917L)
    #define VD_HT_PAL_60                VD_HT_NTSC_443
#else // user mode
    #define MSVD_HSTART_NTSC            0xF5
    #define MSVD_HSTART_PAL             0xED
    #define MSVD_HSTART_SECAM           0xE6
    #define MSVD_HSTART_NTSC_443        0xDF
    #define MSVD_HSTART_PAL_M           0x7D
    #define MSVD_HSTART_PAL_NC          0x83//0xEA

    #define MSVD_VSTART_NTSC            0x12// 0x0A
    #define MSVD_VSTART_PAL             0x0D //0x1B//0x0D
    #define MSVD_VSTART_SECAM           0x0A
    #define MSVD_VSTART_NTSC_443        0x06
    #define MSVD_VSTART_PAL_M           0x09
    #define MSVD_VSTART_PAL_NC          0x10

    #define VD_HT_NTSC                  (1135L*3/2)
    #define VD_HT_PAL                   (1135L*3/2)
    #define VD_HT_SECAM                 (1135L*3/2)
    #define VD_HT_NTSC_443              (1135L*3/2)
    #define VD_HT_PAL_M                 (1135L*3/2)
    #define VD_HT_PAL_NC                (1135L*3/2)
    #define VD_HT_PAL_60                VD_HT_NTSC_443
#endif

#define MSVD_HACTIVE_NTSC               (((MS_U32)720*VD_HT_NTSC+429)/858)
#define MSVD_HACTIVE_PAL                (((MS_U32)720*VD_HT_PAL+432)/864)
#define MSVD_HACTIVE_SECAM              (((MS_U32)720*VD_HT_SECAM+432)/864)
#define MSVD_HACTIVE_NTSC_443           (((MS_U32)720*VD_HT_NTSC_443+432)/864)
#define MSVD_HACTIVE_PAL_M              (((MS_U32)720*VD_HT_PAL_M+429)/858)
#define MSVD_HACTIVE_PAL_NC             (((MS_U32)720*VD_HT_PAL_NC+429)/858)
#define MSVD_HACTIVE_PAL_60             MSVD_HACTIVE_NTSC_443


//------------------------------------
//  H Sensitivity Configuration
//------------------------------------
// for normal mode
#if !defined(VD_HSEN_NORMAL_DETECT_WIN_BEFORE_LOCK)
#define VD_HSEN_NORMAL_DETECT_WIN_BEFORE_LOCK      0x08
#endif
#if !defined(VD_HSEN_NOAMRL_DETECT_WIN_AFTER_LOCK)
#define VD_HSEN_NOAMRL_DETECT_WIN_AFTER_LOCK       0x08
#endif
#if !defined(VD_HSEN_NORMAL_CNTR_FAIL_BEFORE_LOCK)
#define VD_HSEN_NORMAL_CNTR_FAIL_BEFORE_LOCK       0x0F
#endif
#if !defined(VD_HSEN_NORMAL_CNTR_SYNC_BEFORE_LOCK)
#define VD_HSEN_NORMAL_CNTR_SYNC_BEFORE_LOCK       0x10
#endif
#if !defined(VD_HSEN_NORMAL_CNTR_SYNC_AFTER_LOCK)
#define VD_HSEN_NORMAL_CNTR_SYNC_AFTER_LOCK        0x1C
#endif
// for auto-tuning
#if !defined(VD_HSEN_CHAN_SCAN_DETECT_WIN_BEFORE_LOCK)
#define VD_HSEN_CHAN_SCAN_DETECT_WIN_BEFORE_LOCK   0x04 // 0x03
#endif
#if !defined(VD_HSEN_CHAN_SCAN_DETECT_WIN_AFTER_LOCK)
#define VD_HSEN_CHAN_SCAN_DETECT_WIN_AFTER_LOCK    0x04 // 0x03
#endif
#if !defined(VD_HSEN_CHAN_SCAN_CNTR_FAIL_BEFORE_LOCK)
#define VD_HSEN_CHAN_SCAN_CNTR_FAIL_BEFORE_LOCK    0x08 // 0x08
#endif
#if !defined(VD_HSEN_CHAN_SCAN_CNTR_SYNC_BEFORE_LOCK)
#define VD_HSEN_CHAN_SCAN_CNTR_SYNC_BEFORE_LOCK    0x10 // 0x30
#endif
#if !defined(VD_HSEN_CHAN_SCAN_CNTR_SYNC_AFTER_LOCK)
#define VD_HSEN_CHAN_SCAN_CNTR_SYNC_AFTER_LOCK     0x1C // 0x05
#endif

//******************************************************************************
// Public functions.
//******************************************************************************
#if 1
#if defined(ATSC_SYSTEM)
MS_U16 msAPI_AVD_GetStatus ( void );
void msAPI_AVD_Parameter_Initial(void);
#endif
void msAPI_AVD_Exit(void);
void msAPI_AVD_InitVideoSystem(void);
void msAPI_AVD_VideoProcessor(void);
AVD_VideoStandardType msAPI_AVD_GetVideoStandard(void);
MS_BOOL msAPI_AVD_IsVideoFormatChanged(void);
void msAPI_AVD_SetIsVideoFormatChangedAsFalse(void);  //20100330EL

MS_BOOL msAPI_AVD_IsVideoInputChanged(void);
MS_BOOL msAPI_AVD_CheckLockAudioCarrier(void);
MS_BOOL msAPI_AVD_CheckFalseChannel(void);
void msAPI_AVD_StartAutoStandardDetection(void);
void msAPI_AVD_ForceVideoStandard(AVD_VideoStandardType eVideoStandardType);
void msAPI_AVD_SetForceVideoStandardFlag(MS_BOOL bEnable);
MS_BOOL msAPI_AVD_GetForceVideoStandardFlag(void);
#if 0 //unused
void msAPI_AVD_DisableForceMode(void);
#endif
MS_BOOL IsVDHasSignal(void);
AVD_VideoStandardType msAPI_AVD_GetResultOfAutoStandardDetection(void);
void msAPI_AVD_SetVideoStandard(AVD_VideoStandardType eStandard);
ASPECT_RATIO_TYPE msAPI_AVD_GetAspectRatioCode(void);
MS_BOOL msAPI_AVD_IsAspectRatioChanged(void);
void msAPI_AVD_WaitForVideoSyncLock(void);
AVD_InputSourceType msAPI_AVD_GetVideoSource(void);
void msAPI_AVD_SetMuxPower(AVD_InputSourceType eSource);
void msAPI_AVD_SetVideoSource(AVD_InputSourceType eSource);
void msAPI_AVD_DetectVideoStandard(OPERATIONMETHOD eOperationMethod);
#if 0 //unused
AVD_VideoFreq msAPI_AVD_GetStandardFreq(void);
#endif
AVD_VideoFreq msAPI_AVD_GetVerticalFreq(void);
MS_BOOL msAPI_AVD_IsSyncChanged(void);
MS_BOOL msAPI_AVD_IsSyncLocked(void);
#if 0 // use msAPI_AVD_IsSyncLocked
MS_BOOL msAPI_AVD_IsSyncDetected(void);
#endif
#if 0
void msAPI_AVD_ClearSyncCheckCounter(void);
#endif
#if 0 // TODO remove later
void msAPI_AVD_ImprovePictureQuality(TELETEXT_DISPLAY_MODE eDisplayMode);
#endif
#if 0 //BY 20090406
void msAPI_AVD_AdjustVideoFactor(ADJUST_TVAVFACTOR eFactor, WORD wParam1);
#endif
MS_BOOL msAPI_AVD_UpdateAutoAVState(void);
MS_BOOL msAPI_AVD_IsAutoAVActive(AVD_AutoAVSourceType eAutoAVSource);
void msAPI_AVD_TurnOffAutoAV(void);
void msAPI_AVD_SwitchAutoAV(void);
SCART_SOURCE_TYPE msAPI_AVD_GetScart1SrcType(void);
MS_BOOL msAPI_AVD_IsScart1SourceTypeChanged(void);
void msAPI_AVD_ClearAspectRatio(void);
void msAPI_AVD_SuppressAutoAV(void);
#if 0 // BY 20090403 unused function
void msAPI_AVD_EnableAutoGainControl(MS_BOOL bEnable);
#endif
void msAPI_AVD_SetForcedFreeRun(MS_BOOL bEnable);
void msAPI_AVD_SetHsyncDetectionForTuning(MS_BOOL bEnable);
void msAPI_AVD_3DCombSpeedup(void);
void msAPI_AVD_SetChannelChange(void);
MS_U8 msAPI_AVD_CheckStdDetStableCnt(void);
void msAPI_AVD_SetFWStatus(MS_BOOL bFWStatus);
MS_BOOL msAPI_AVD_IsScartRGB(void);
AVD_VideoStandardType msAPI_AVD_GetStandardDetection(void);
AVD_VideoStandardType msAPI_AVD_GetCurrentVideoStandard(void);

void msAPI_AVD_SetRegFromDSP(void);
void msAPI_AVD_SetPatchOption(void);
MS_U16 msAPI_AVD_CheckStatusLoop(void);
void msAPI_AVD_GetCaptureWindow(MS_WINDOW_TYPE *stCapWin, EN_VD_SIGNALTYPE enVideoSystem, MS_BOOL IsUseExtVD);
void msAPI_AVD_GetCaptureWindowByPQ(MS_WINDOW_TYPE *stCapWin, EN_VD_SIGNALTYPE enVideoSystem, MS_PQ_VD_Sampling_Info *pInfo);

EN_VD_SIGNALTYPE mvideo_vd_get_videosystem(void);
void mvideo_vd_set_videosystem(EN_VD_SIGNALTYPE ucVideoSystem);

MS_U16 mvideo_vd_get_videostatus(void);

MS_U16 msAPI_Scaler_VD_GetVTotal(SCALER_WIN eWindow, INPUT_SOURCE_TYPE_t eCurrentSrc,EN_GET_SYNC_TYPE enGetSyncType, MS_U16 u16HFreqX10);
MS_U16 msAPI_Scaler_VD_GetHPeriod(SCALER_WIN eWindow, INPUT_SOURCE_TYPE_t eCurrentSrc, EN_GET_SYNC_TYPE enGetSyncType);
#endif

STANDARD_DETECTION_STATE msAPI_AVD_ChekcStdDetStableState(void);


#endif

