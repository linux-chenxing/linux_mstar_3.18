// $Change: 640114 $
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
/// @file msAPI_vbi.h
/// @brief API for Getting VBI Data from VBI Slicer
/// @author MStar Semiconductor, Inc.
//
////////////////////////////////////////////////////////////////////////////////

#define MSAPI_VE_C
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>

#include "ms_platform.h"

#include "MsCommon.h"
#include "MsTypes.h"
#include "drvXC_IOPort.h"
#include "apiXC.h"
#include "msapi_vd.h"
#include "drvAVD.h"
//#include "apiAUDIO.h"

#define debugVideoPrint(a,b)

#define ENABLE_SW_CH_FREEZE_SCREEN  0
#define ENABLE_TTX                  0
#define ENABLE_VD_PACH_IN_CHINA     0
#define IS_WSS_ENABLED              0
#define IS_SCART_ENABLED            0

typedef MS_U8 MS_REG_INIT;
#define _RV1(addr, value)       (((addr) >> 8) & 0x3F), (MS_U8)(addr), (MS_U8)(value)
#define _END_OF_TBL2_           0xFF, 0xFF
#define FREQ_12MHZ              (12000000UL)
#define FREQ_13MHZ              (13000000UL)
#define MST_XTAL_CLOCK_HZ       FREQ_13MHZ
#define MS_DEBUG_MSG
#define UNUSED(x)               (x=x)
#define PQ_ENABLE_VD_SAMPLING   0
//****************************************************************************
// Private attributes of this file.
//****************************************************************************
#define AFEC_REG_BASE                           0x3500  // 0x3500 - 0x35FF
#define COMB_REG_BASE                           0x3600  // 0x3600 - 0x36FF
#define BK_AFEC_D0  (AFEC_REG_BASE+0xD0)
#define BK_AFEC_D2  (AFEC_REG_BASE+0xD2)
#define BK_AFEC_D4  (AFEC_REG_BASE+0xD4)
#define BK_AFEC_D5  (AFEC_REG_BASE+0xD5)
#define BK_AFEC_DB  (AFEC_REG_BASE+0xDB)
#define BK_AFEC_C4  (AFEC_REG_BASE+0xC4)
#define BK_COMB_22  (COMB_REG_BASE+0x22)
#define BK_COMB_37  (COMB_REG_BASE+0x37)
#define BK_COMB_38  (COMB_REG_BASE+0x38)
#define BK_COMB_45  (COMB_REG_BASE+0x45)
#define BK_COMB_7C  (COMB_REG_BASE+0x7C)
#define BK_COMB_ED  (COMB_REG_BASE+0xED)

/******************************************************************************************/
// b3 b2 b1 b0    AspectRatio        Format            Position        Active Lives
/********************************************************************************/
//  1  0  0  0        4:3            Full format           -                576
//  0  0  0  1        14:9        Letterbox        Center                504
//  0  0  1  0        14:9        Letterbox        Top                    504
//  1  0  1  1        16:9        Letterbox        Center                430
//  0  1  0  0        16:9        Letterbox        Top                    430
//  1  1  0  1        >16:9        Letterbox        Center                 -
//  1  1  1  0        14:9        Full format        Center                576
//  0  1  1  1        16:9        Full format           -                576
/******************************************************************************************/
#define WSS_4x3_FULL                    0x08
#define WSS_14x9_LETTERBOX_CENTER        0x01
#define WSS_14x9_LETTERBOX_TOP            0x02
#define WSS_16x9_LETTERBOX_CENTER        0x0B
#define WSS_16x9_LETTERBOX_TOP            0x04
#define WSS_ABOVE16x9_LETTERBOX_CENTER    0x0D
#define WSS_14x9_FULL_CENTER            0x0E
#define WSS_16x9_ANAMORPHIC                0x07

#define WSS_MASK        0x0F

const MS_WINDOW_TYPE VideoCaptureWinTbl [] =
{
    {MSVD_HSTART_NTSC,      MSVD_VSTART_NTSC,       MSVD_HACTIVE_NTSC,      480}, // NSTC
    {MSVD_HSTART_PAL,       MSVD_VSTART_PAL,        MSVD_HACTIVE_PAL,       576}, // PAL
    {MSVD_HSTART_SECAM,     MSVD_VSTART_SECAM,      MSVD_HACTIVE_SECAM,     576}, // SECAM
    {MSVD_HSTART_NTSC_443,  MSVD_VSTART_NTSC_443,   MSVD_HACTIVE_NTSC_443,  480}, // NTSC-443/PAL-60
    {MSVD_HSTART_PAL_M,     MSVD_VSTART_PAL_M,      MSVD_HACTIVE_PAL_M,     480}, // PAL-M
    {MSVD_HSTART_PAL_NC,    MSVD_VSTART_PAL_NC,     MSVD_HACTIVE_PAL_NC,    576}, // PAL-Nc
};

const MS_WINDOW_TYPE ExtVDVideoCapture[] =
{
    {0x00, 0x00, 720, 480}, // NSTC
    {0x00, 0x00, 720, 576}, // PAL
    {0x00, 0x00, 720, 576}, // SECAM
    {0x00, 0x00, 720, 480}, // NTSC-443/PAL-60
    {0x00, 0x00, 720, 480}, // PAL-M
    {0x00, 0x00, 720, 576}, // PAL-Nc
};



static AVD_VideoStandardType       m_eVideoStandard;
static AVD_VideoStandardType       m_eCurrentVideoStandard;

static AVD_InputSourceType         m_eVideoSource;
static AVD_VideoStandardType m_eFSMDetectedStandard;
static WORD m_wVideoDownCountTimer;
static WORD m_wVideoUpCountTimer;
static MS_BOOL m_bForceVideoStandardFlag = FALSE;

static MS_U16 m_cPreviousScart1IDLevelForAutoAV;
static MS_U16 m_cPreviousScart2IDLevelForAutoAV;

static STANDARD_DETECTION_STATE m_eStandardDetectionState;
static MS_U16 m_u16LatchStatus;
#if 0 // BY 20090406
static DWORD m_dwStartedTimeOfMute;
static WORD m_wLimitedTimeOfMute;
#endif

static SCART_SOURCE_TYPE       VDScart1SrcType;
static ASPECT_RATIO_TYPE      m_eAspectRatioCode;

typedef struct
{
    BYTE bBOOL_0    :1;
    BYTE bBOOL_1    :1;
    BYTE bBOOL_2    :1;
    BYTE bBOOL_3    :1;
    BYTE bBOOL_4    :1;
    BYTE bBOOL_5    :1;
    BYTE bBOOL_6    :1;
    BYTE bBOOL_7    :1;
} BOOLEANS;

static BOOLEANS m_BOOLEANS_1;
#define m_bIsVideoFormatChanged             m_BOOLEANS_1.bBOOL_0
#define m_bIsVideoInputChanged              m_BOOLEANS_1.bBOOL_1
#define m_bIsAspectRatioChanged             m_BOOLEANS_1.bBOOL_2
#define m_bIsSyncChanged                    m_BOOLEANS_1.bBOOL_3
#define m_bIs3DCombfilterUsed               m_BOOLEANS_1.bBOOL_4
#define m_bIsAGCEnabled                     m_BOOLEANS_1.bBOOL_5
#define m_bIsScart1SourceTypeChanged        m_BOOLEANS_1.bBOOL_6


static BOOLEANS m_BOOLEANS_2;
#define m_bPermanentVideoMute               m_BOOLEANS_2.bBOOL_0
#define m_bMomentVideoMute                  m_BOOLEANS_2.bBOOL_1
#define m_bByLockVideoMute                  m_BOOLEANS_2.bBOOL_2
#define m_bByParentalVideoMute              m_BOOLEANS_2.bBOOL_3
#define m_bByDuringLimitedTimeVideoMute     m_BOOLEANS_2.bBOOL_4
#define m_bIsAutoAV1Active                  m_BOOLEANS_2.bBOOL_5
#define m_bIsAutoAV2Active                  m_BOOLEANS_2.bBOOL_6
#define FORCEDFREEFUN FALSE

//   BK_AFEC_DC
#define MSK_UD7_BANK                            0x03
#define VAL_UD7_BANK6                           0x00
#define VAL_UD7_BANK7                           0x01
#define MSK_UD7_STATE                           0x0C
#define VAL_UD7_FREE                            0x00
#define VAL_UD7_READ                            0x04
#define VAL_UD7_WRITE                           0x08
#define VAL_UD7_READ_END                        0x0C
#define IS_BITS_MATCHED(val, bits)              (((val)&(bits)) == (bits))
#define ENABLE_NTSC_50                          0
//#define LOCK3DSPEEDUP                           1 // Designer Robert.Chen for Channel Change, COMB_57=0x50,COMB_58=0x20
#define PATCH_COMB_ZONEPLATE                    0
#define NEW_COMB_STILLIMAGE                     0
#define FINE_TUNE_FH_DOT                        1       // fine-tune for Fh image quality, Eris-ATT, James.Lu, 20080328
#define FINE_TUNE_FSC_SHIFT                     0       // fine-tune for Fsc shift cause color bar edge blur, James.Lu, 20080919
#define PATCH_COMB_STILLIMAGE                   1
#define FINE_TUNE_COMB_F2                       1       // fine-tune for bug's life, for Eris-ATT, James.Lu, 20080327
#define FINE_TUNE_3D_COMB                       1       // fine-tune 3D Comb, for Eris-ATT, James.Lu, 20080328
#define COLOR_KILL_HIGH_BOUND                   0x30 // 0x0E //0x89 // adjust this value for color on when RF signal is weak
#define COLOR_KILL_LOW_BOUND                    0x30 // 0x0E //0x15 // adjust this value for color kill level

static MS_U8 u8StdDetStableCnt = 0;
static MS_U8  _u8PreNoiseMag;
static MS_U16 _u16DataH[3];
static MS_U8  _u8HtotalDebounce;
static MS_U8 _u8AutoTuningIsProgress = 0x00; //YES:0x01
static AVD_VideoSystem _eVideoSystem=E_VIDEO_SYSTEM_DVB;

static MS_BOOL bVD_FWStatus = FALSE;    /* TRUE: Nand Flash; FALSE: SPI Flash; */


#if(ENABLE_SW_CH_FREEZE_SCREEN)
MS_BOOL bVideoStandardChanged = FALSE;
#endif

//****************************************************************************
// Private functions of this file.
//****************************************************************************

static void msAPI_AVD_CheckVideoStandard(void);
//static void msAPI_AVD_CheckVideoSourceSwitch(void);
// static void msAPI_AVD_CheckVerticalTotal(void); // <-<<< UNUSED
static void msAPI_AVD_CheckAspectRatio(void);
#ifndef ATSC_SYSTEM
static void msAPI_AVD_CheckAutoAV(void);
#endif
//static void msAPI_AVD_CheckScart1SourceType(void);
//static void msAPI_AVD_CheckSignalPath(void);
//static void msAPI_AVD_TurnOnAutoAV(void);
#if 0
static void msAPI_AVD_PriTurnOffAutoAV(void);
#endif
static void msAPI_AVD_RetrieveScart1SourceType(void);
#if 0 // BY 20090406
static void msAPI_AVD_SetVideoMuteDuringLimitedTime(WORD w1ms);
static void msAPI_AVD_CheckExpirationOfVideoMute(void);
static void msAPI_AVD_SetVideoMute(VIDEOMUTETYPE eVideoMuteType);
#endif
//static void msAPI_AVD_ClearAllVideoMuteFlags(void);


#if defined(ATSC_SYSTEM)
/******************************************************************************/
///-Get Video Decoder status
///@return
///Status represented bits from VD
///- VD_PAL_SWITCH: BIT9
///- VD_COLOR_LOCKED: BIT10
///- VD_RESET_ON: BIT11
///- VD_VSYNC_50HZ: BIT12
///- VD_INTERLACED: BIT13
///- VD_HSYNC_LOCKED: BIT14
///- VD_SYNC_LOCKED: BIT15
///- VD_MACROVISION: BIT2
///- VD_VCR_MODE: BIT4
///- VD_FSC_3579: BIT6
///- VD_FSC_3575: BIT7
///- VD_FSC_3582: (BIT6|BIT7)
///- VD_FSC_4433: 0
///- VD_FSC_4285: BIT5
/******************************************************************************/
MS_U16 msAPI_AVD_GetStatus ( void )
{
    return MDrv_AVD_GetStatus();
}

/******************************************************************************/
/// -Initial the VD's parameter
/******************************************************************************/
void msAPI_AVD_Parameter_Initial(void)
{
    #if 0
    MDrv_VD_Parameter_Initial();
    #endif
}
#endif


MS_BOOL msAPI_AVD_CheckLockAudioCarrier(void)
{
    MS_U16 u16tmp,u16tmp1=0;
    MS_U8 u8tmp;

    for(u8tmp=0;u8tmp<10;u8tmp++)
    {
        u16tmp = (MDrv_AVD_GetRegValue(BK_AFEC_C4) & 0xFF);
        if(u16tmp>u16tmp1)
             u16tmp1=u16tmp;
        if(u16tmp1 >=0x3B)
        {
            //printf("\r\n REG_35C4  %x",u16tmp1);
            return TRUE;
        }
        //msAPI_Timer_Delayms(1);
        mdelay(1);
    }
    //printf("\r\n REG_35C4 = %x",u16tmp1);

    if(u16tmp1>=0x3B)
        return TRUE;
    else
        return FALSE;
}

MS_BOOL msAPI_AVD_CheckFalseChannel(void)
{
    MS_U16 u16tmp,u16tmp1=0;
    MS_U8 u8tmp;

    for(u8tmp=0;u8tmp<10;u8tmp++)
    {
        u16tmp = (MDrv_AVD_GetRegValue(BK_AFEC_C4) & 0xFF);
        if(u16tmp>u16tmp1)
            u16tmp1=u16tmp;
        if(u16tmp1 ==0x3C)
        {
            //printf("\r\n REG_35C4-1 %x",u16tmp1);
            return TRUE;
        }
        //msAPI_Timer_Delayms(1);
        mdelay(1);
    }
    //printf("\r\n REG_35C4-1 %x",u16tmp1);

    return FALSE;

}

#if ENABLE_TTX
//****************************************************************************
/// Get the WSS value from VBI line#16
/// @param bServiceType \b IN: Service type
/// @param None \b IN:
/// @return ASPECT_RATIO_TYPE : the WSS aspect ratio value
/// -@see ASPECT_RATIO_TYPE
//****************************************************************************
ASPECT_RATIO_TYPE msAPI_AVD_GetAspectRatioCodeOfWSS(void)
{
    ASPECT_RATIO_TYPE eAspectRatioCode;
    WORD wWSSData;

    if(msAPI_TTX_GetWssStatus()==FALSE)
        return ARC_INVALID;

    wWSSData = msAPI_TTX_ReadWssWord();

    wWSSData = wWSSData & WSS_MASK;
    switch(wWSSData)
    {
        case WSS_4x3_FULL:
            eAspectRatioCode = ARC4x3_FULL;
            break;
        case WSS_14x9_LETTERBOX_CENTER:
            eAspectRatioCode = ARC14x9_LETTERBOX_CENTER;
            break;
        case WSS_14x9_LETTERBOX_TOP:
            eAspectRatioCode = ARC14x9_LETTERBOX_TOP;
            break;
        case WSS_16x9_LETTERBOX_CENTER:
            eAspectRatioCode = ARC16x9_LETTERBOX_CENTER;
            break;
        case WSS_16x9_LETTERBOX_TOP:
            eAspectRatioCode = ARC16x9_LETTERBOX_TOP;
            break;
        case WSS_ABOVE16x9_LETTERBOX_CENTER:
            eAspectRatioCode = ARC_ABOVE16x9_LETTERBOX_CENTER;
            break;
        case WSS_14x9_FULL_CENTER:
            eAspectRatioCode = ARC14x9_FULL_CENTER;
            break;
        case WSS_16x9_ANAMORPHIC:
            eAspectRatioCode = ARC16x9_ANAMORPHIC;
            break;
        default:
            eAspectRatioCode = ARC_INVALID;
            break;
    }

    return eAspectRatioCode;
}
#endif

AVD_VideoStandardType msAPI_AVD_GetStandardDetection(void)
{
    MS_U16 u16Status;
    return MDrv_AVD_GetStandardDetection(&u16Status);
}


/******************************************************************************/
///- API to initialize video decoder
/******************************************************************************/
void msAPI_AVD_InitVideoSystem(void)
{
 #if 0
    static const MS_REG_INIT _u8VdDecInitializeExt[] =
    {
        //initial AFEC Setting
       // _RV1(BK_AFEC_D4, 0x00),  // TODO update correct value
        _RV1(BK_AFEC_D5, 0x60),
        //_RV1(BK_AFEC_D8, 0x88),
        //_RV1(BK_AFEC_D9, 0x19),
        _END_OF_TBL2_,
    };
 #endif
    //VD_INITDATA sVD_InitData;
#if ENABLE_VD_PACH_IN_CHINA
    //_u8VdDecInitializeExt[0] =stGenSetting.g_FactorySetting.u8AFEC_D4;
    //_u8VdDecInitializeExt[1] =(BK_AFEC_D5_DEFAULT|(stGenSetting.g_FactorySetting.u8AFEC_D5_Bit2 &0x0004));
    //_u8VdDecInitializeExt[8] =(BK_AFEC_D8_DEFAULT|(stGenSetting.g_FactorySetting.u8AFEC_D8_Bit3210)));
    //_u8VdDecInitializeExt[11]=(BK_AFEC_D9_DEFAULT|(stGenSetting.g_FactorySetting.u8AFEC_D9_Bit4_7 &(_BIT4|_BIT5|_BIT6|_BIT7)));
#endif

    /* Before MDrv_VD_Init(), you should tell VD where F/W is. */
#ifdef ATSC_SYSTEM
    _eVideoSystem=E_VIDEO_SYSTEM_ATSC;
#else
    _eVideoSystem=E_VIDEO_SYSTEM_DVB;
#endif
    // BY 20090409 MDrv_VD_SetFWStaus(bVD_FWStatus, MDrv_Sys_GetBinAddr());

#if 0 // It will be done in AVD_Init
    // Init VD
    // VIF
    sVD_InitData.u32XTAL_Clock = MST_XTAL_CLOCK_HZ; // TODO replace with system define

    sVD_InitData.eDemodType = DEMODE_MSTAR_VIF;

    sVD_InitData.eLoadCodeType=AVD_LOAD_CODE_BYTE_WRITE;
    sVD_InitData.u32VD_DSP_Code_Address=0;
    sVD_InitData.pu8VD_DSP_Code_Address=0;
    sVD_InitData.u32VD_DSP_Code_Len=0; // use driver internal dsp code instead of flash bin version
    sVD_InitData.u16VDDSPBinID = 0xFFFF;

    sVD_InitData.bRFGainSel = VD_GAIN_OF_RF_SEL;
    sVD_InitData.bAVGainSel = VD_GAIN_OF_AV_SEL;
    sVD_InitData.u8RFGain = VD_GAIN_OF_RF;
    sVD_InitData.u8AVGain = VD_GAIN_OF_AV;
#if ENABLE_VD_PACH_IN_CHINA
    sVD_InitData.u32VDPatchFlag = stGenSetting.g_FactorySetting.u32VDPatchFlag;
    sVD_InitData.u8ColorKillHighBound = stGenSetting.g_FactorySetting.u8AFEC_D7_HIGH_BOUND;
    sVD_InitData.u8ColorKillLowBound = stGenSetting.g_FactorySetting.u8AFEC_D7_LOW_BOUND;
    MDrv_WriteByte(0x1035A1, stGenSetting.g_FactorySetting.u8AFEC_A1);
    MDrv_WriteByteMask(0x103566, (stGenSetting.g_FactorySetting.u8AFEC_66_Bit76<<6), (BIT7|BIT6));
    MDrv_WriteByteMask(0x10356E, (stGenSetting.g_FactorySetting.u8AFEC_6E_Bit7654<<4),0xF0);
    MDrv_WriteByteMask(0x10356E, stGenSetting.g_FactorySetting.u8AFEC_6E_Bit3210,0x0F);
    MDrv_WriteByteMask(0x103543, stGenSetting.g_FactorySetting.u8AFEC_43, 0xFF);
#else
    sVD_InitData.u32VDPatchFlag = MSVD_HTOTAL_TYPE|AVD_PATCH_FINE_TUNE_FH_DOT|AVD_PATCH_FINE_TUNE_COMB_F2|AVD_PATCH_FINE_TUNE_3D_COMB|AVD_PATCH_NON_STANDARD_VTOTAL|AVD_PATCH_FINE_TUNE_STILL_IMAGE;
    sVD_InitData.u8ColorKillHighBound = VD_COLOR_KILL_HIGH_BOUND;
    sVD_InitData.u8ColorKillLowBound = VD_COLOR_KILL_LOW_BOUND;
#endif

#if 1 //(!ENABLE_PWS)
    sVD_InitData.u32VDPatchFlag|= AVD_PATCH_DISABLE_PWS;
#endif

    sVD_InitData.u8SwingThreshold = VD_SIG_SWING_THRESH;
    sVD_InitData.eVDHsyncSensitivityNormal.u8DetectWinBeforeLock= VD_HSEN_NORMAL_DETECT_WIN_BEFORE_LOCK;
    sVD_InitData.eVDHsyncSensitivityNormal.u8DetectWinAfterLock= VD_HSEN_NOAMRL_DETECT_WIN_AFTER_LOCK;
    sVD_InitData.eVDHsyncSensitivityNormal.u8CNTRFailBeforeLock= VD_HSEN_NORMAL_CNTR_FAIL_BEFORE_LOCK;
    sVD_InitData.eVDHsyncSensitivityNormal.u8CNTRSyncBeforeLock= VD_HSEN_NORMAL_CNTR_SYNC_BEFORE_LOCK;
    sVD_InitData.eVDHsyncSensitivityNormal.u8CNTRSyncAfterLock= VD_HSEN_NORMAL_CNTR_SYNC_AFTER_LOCK;
    sVD_InitData.eVDHsyncSensitivityTuning.u8DetectWinBeforeLock= VD_HSEN_CHAN_SCAN_DETECT_WIN_BEFORE_LOCK;
    sVD_InitData.eVDHsyncSensitivityTuning.u8DetectWinAfterLock= VD_HSEN_CHAN_SCAN_DETECT_WIN_AFTER_LOCK;
    sVD_InitData.eVDHsyncSensitivityTuning.u8CNTRFailBeforeLock= VD_HSEN_CHAN_SCAN_CNTR_FAIL_BEFORE_LOCK;
    sVD_InitData.eVDHsyncSensitivityTuning.u8CNTRSyncBeforeLock= VD_HSEN_CHAN_SCAN_CNTR_SYNC_BEFORE_LOCK;
    sVD_InitData.eVDHsyncSensitivityTuning.u8CNTRSyncAfterLock= VD_HSEN_CHAN_SCAN_CNTR_SYNC_AFTER_LOCK;
    sVD_InitData.u8VdDecInitializeExt=(MS_U8 *)_u8VdDecInitializeExt;

    // comb
#if 0 //ToDo
    sVD_InitData.u32COMB_3D_ADR = ((COMB_3D_BUF_MEMORY_TYPE & MIU1) ? (COMB_3D_BUF_ADR | MIU_INTERVAL) : (COMB_3D_BUF_ADR));
    sVD_InitData.u32COMB_3D_LEN = COMB_3D_BUF_LEN;
#endif

    if(sVD_InitData.u32VDPatchFlag & AVD_PATCH_FINE_TUNE_STILL_IMAGE)
    {
        AVD_Still_Image_Param param;
        param.bMessageOn =0;

        param.u8Threshold1 = 0x14;
        param.u8Threshold2 = 0x19;
        param.u8Threshold3 = 0x29;
        param.u8Threshold4 = 0x30;

        param.u8Str1_COMB37 = 0x88;
        param.u8Str1_COMB38 = 0xF8;
        param.u8Str1_COMB7C = 0x10;
        param.u8Str1_COMBED = 0x80;

        param.u8Str2_COMB37 = 0x8F;
        param.u8Str2_COMB38 = 0xF8;
        param.u8Str2_COMB7C = 0x20;
        param.u8Str2_COMBED = 0x82;

        param.u8Str3_COMB37 = 0x0B;
        param.u8Str3_COMB38 = 0xF8;
        param.u8Str3_COMB7C = 0x10;
        param.u8Str3_COMBED = 0x80;

        MDrv_AVD_SetStillImageParam(param);
    }

    if (E_AVD_FAIL==MDrv_AVD_Init(&sVD_InitData, sizeof(sVD_InitData)))
    {
        MS_DEBUG_MSG(printf("## %s Fail##\r\n",__FUNCTION__));
    }
#endif

    m_eVideoStandard = E_VIDEOSTANDARD_NOTSTANDARD;
    m_eCurrentVideoStandard	= E_VIDEOSTANDARD_NOTSTANDARD;
    m_eVideoSource = E_INPUT_SOURCE_INVALID;

    m_wVideoDownCountTimer = WAIT_0ms;
    m_wVideoUpCountTimer = WAIT_0ms;

    m_bIsVideoFormatChanged = FALSE;
    m_bIsVideoInputChanged = FALSE;
    m_bIsSyncChanged = FALSE;
    m_bIs3DCombfilterUsed = FALSE;

#if 0 // BY 20090406
    m_dwStartedTimeOfMute = 0;
    m_wLimitedTimeOfMute = 0;
#endif

    m_bPermanentVideoMute = FALSE;
    m_bMomentVideoMute = FALSE;
    m_bByLockVideoMute = FALSE;
    m_bByParentalVideoMute = FALSE;
    m_bByDuringLimitedTimeVideoMute = FALSE;

    m_bIsAutoAV1Active = FALSE;
    m_bIsAutoAV2Active = FALSE;

    m_cPreviousScart1IDLevelForAutoAV = 0; //SCART_ID_LEVEL_0V;
    m_cPreviousScart2IDLevelForAutoAV = 0; //SCART_ID_LEVEL_0V;

    m_eStandardDetectionState = E_DETECTION_START;

    m_bIsAGCEnabled = TRUE;
    m_bIsScart1SourceTypeChanged = FALSE;
    VDScart1SrcType = E_SCART_SRC_TYPE_UNKNOWN;

    m_bIsAspectRatioChanged = FALSE;
    m_eAspectRatioCode = ARC_INVALID;
    _u16DataH[0] = _u16DataH[1] = _u16DataH[2] = VD_HT_PAL;
    _u8PreNoiseMag=0;
    _u8HtotalDebounce = 0;

    MDrv_AVD_SetFreerunFreq(E_FREERUN_FQ_AUTO);

    //To get the VD_Lib Version Number
    {
        //BY MS_U8 u8Verinfo[32];
        //BY MDrv_VD_GetVDVerString(&u8Verinfo[0]);
     //   printf("VD Lib Build Version ---> %c-%c-%c-%c\n",u8Verinfo[10],u8Verinfo[11]
     //       ,u8Verinfo[12],u8Verinfo[13]);
     //   printf("VD Lib Interface Version ---> %c-%c \n",u8Verinfo[8],u8Verinfo[9]);

    }

}


void msAPI_AVD_Exit(void)
{
    MDrv_AVD_Exit();
}

void msAPI_AVD_ZonePlatePatch(void)
{
#if 0 // ToDo!!
    MS_U8 u8Value, u8Motion ;
    static MS_U8 u8PrevLuma = 128;

    #define LUMA_TH1 0x68
    #define LUMA_TH2 0xAA

    u8Value = MApi_XC_DLC_GetAverageValue();

    //printf("MDrv_DLC_GetAverageValue = 0x%x\n",MDrv_DLC_GetAverageValue()) ;
    u8Value=(WORD)((WORD)u8PrevLuma*3+((WORD)u8Value*1))>>2;
    u8PrevLuma = u8Value;

    u8Motion = MApi_XC_GetCurrentMotionValue();

    if(IsATVInUse()||IsAVInUse())
    {
        if(u8Motion < 0x0A )
        {
           MDrv_AVD_SetRegValue(BK_COMB_45, (MDrv_AVD_GetRegValue(BK_COMB_45)&(~BIT7)));

           if(u8Value < LUMA_TH1)
           {
                //printf("AAA\n");
                MDrv_AVD_SetRegValue(BK_COMB_22, (MDrv_AVD_GetRegValue(BK_COMB_22)&(~(BIT1|BIT0)))|(BIT1|BIT0));
           }
           else if ((u8Value >= LUMA_TH1)&&(u8Value < LUMA_TH2))
           {
                //printf("CCC\n");
                MDrv_AVD_SetRegValue(BK_COMB_22, (MDrv_AVD_GetRegValue(BK_COMB_22)&(~(BIT1|BIT0)))|(BIT0));
           }
           else
           {
                //printf("DDD\n");
                MDrv_AVD_SetRegValue(BK_COMB_22, (MDrv_AVD_GetRegValue(BK_COMB_22)&(~(BIT1|BIT0))));
           }
        }
        else
        {
           MDrv_AVD_SetRegValue(BK_COMB_45, (MDrv_AVD_GetRegValue(BK_COMB_45)&(~BIT7))|(BIT7));
           MDrv_AVD_SetRegValue(BK_COMB_22, (MDrv_AVD_GetRegValue(BK_COMB_22)&(~(BIT1|BIT0)))|(BIT1|BIT0));
        }
    }
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: msAPI_AVD_SetRegFromDSP()
/// @brief \b Function \b Description: To set AFEC/COMB registers through VD DSP's register.
/// @param <IN>        \b None     :
///
///
/// @param <OUT>       \b None     :
/// @param <RET>       \b None     :
/// @param <GLOBAL>    \b None     :
////////////////////////////////////////////////////////////////////////////////
void msAPI_AVD_SetPatchOption(void)
{//must set patch option before init.
    MS_U32 ePatchFlag=AVD_PATCH_NONE;

#if ENABLE_NTSC_50
    ePatchFlag|=AVD_PATCH_NTSC_50;
#endif
    MDrv_AVD_SetFlag(ePatchFlag);
}

void msAPI_AVD_SetRegFromDSP(void)
{
#if 0 // for DPL debug
    if (MDrv_AVD_GetRegValue(BK_AFEC_DB) & 0x01)
    {
        #define TOTAL_LINE 525*30
        MS_S16 dpl[TOTAL_LINE];
        MS_U16 count,linenumber[TOTAL_LINE],temp[4],prevline=-1;

        for ( count=0 ; count <TOTAL_LINE ; )
        {
          temp[0]=(MDrv_AVD_GetRegValue(BK_AFEC_D0+1)<<8)+MDrv_AVD_GetRegValue(BK_AFEC_D0);
          temp[1]=(MDrv_AVD_GetRegValue(BK_AFEC_D0+1)<<8)+MDrv_AVD_GetRegValue(BK_AFEC_D0);
          temp[2]=(MDrv_AVD_GetRegValue(BK_AFEC_D2+1)<<8)+MDrv_AVD_GetRegValue(BK_AFEC_D2);
          temp[3]=(MDrv_AVD_GetRegValue(BK_AFEC_D2+1)<<8)+MDrv_AVD_GetRegValue(BK_AFEC_D2);
          if (((temp[0]==temp[1]) && (temp[2]==temp[3])) && (prevline!=temp[2]))
          {
            dpl[count]=temp[0];
            //if (MDrv_AVD_GetRegValue(0x36E0)&0x02)  dpl[count] |= 0x4000;
            prevline=linenumber[count]=temp[2];
            count++;
          }
        }

        for ( count=0 ; count <TOTAL_LINE ; count++)
        {
          printf("%x %x\n",dpl[count],linenumber[count]);
        }
    }

    if (MDrv_AVD_GetRegValue(BK_AFEC_DB) & 0x02)
    {
        MS_S16 temp0, temp1;
        temp0=(MS_S16)((MDrv_AVD_GetRegValue(BK_AFEC_D0+1)<<8)+MDrv_AVD_GetRegValue(BK_AFEC_D0));
        temp1=(MS_S16)((MDrv_AVD_GetRegValue(BK_AFEC_D2+1)<<8)+MDrv_AVD_GetRegValue(BK_AFEC_D2));
        printf("VD: %d %d\r\n",temp0, temp1);
    }
#endif

    MDrv_AVD_SetRegFromDSP();
}


MS_U16 msAPI_AVD_CheckStatusLoop(void) // only for channel scan
{
    return MDrv_AVD_Scan_HsyncCheck(15);
}


/******************************************************************************/
///- API for Video processor of VD
/******************************************************************************/
void msAPI_AVD_VideoProcessor(void)
{
    #define IsVideoSrcVd(x) (E_INPUT_SOURCE_ATV <= (x) && (x) <= E_INPUT_SOURCE_SCART2) // <-<<< CHECK NAMING AND RELOCATION
    if( !IsVideoSrcVd(m_eVideoSource) )
    {
        return;
    }

#if 0// BY 20090406
    msAPI_AVD_CheckExpirationOfVideoMute();
#endif

    m_wVideoUpCountTimer++;
    if( m_wVideoUpCountTimer >= WAIT_MAXms )
    {
        m_wVideoUpCountTimer = WAIT_10ms;
    }

#if 0
  //#if ( ((FRONTEND_IF_DEMODE_TYPE == MSTAR_VIF)||(FRONTEND_IF_DEMODE_TYPE == MSTAR_INTERN_VIF)) && !defined(ATSC_SYSTEM) )
#if ( ((FRONTEND_IF_DEMODE_TYPE == MSTAR_VIF)||(FRONTEND_IF_DEMODE_TYPE == MSTAR_INTERN_VIF)) )
  #ifdef ATSC_SYSTEM
    MDrv_VIF_Handler(FALSE);
  #else
    MDrv_VIF_Handler(gbTVAutoScanChannelEnable);
  #endif
#endif
#endif

    msAPI_AVD_SetRegFromDSP();
    msAPI_AVD_CheckVideoStandard();

#ifndef ATSC_SYSTEM
    if (0)//msAPI_Tuner_IsTuningProcessorBusy() == TRUE ) ToDo!!
    {
        return;
    }

    if( (m_wVideoUpCountTimer % WAIT_430ms) == 0 )
    {
        msAPI_AVD_CheckAutoAV();
    }
#endif

    if( (m_wVideoUpCountTimer % WAIT_290ms) == 0 )
    {
        msAPI_AVD_CheckAspectRatio();
    }

}


/******************************************************************************/
///- This function will get Video standard
/// @return AVD_VideoStandardType: Video standard type
/******************************************************************************/
AVD_VideoStandardType msAPI_AVD_GetVideoStandard(void)
{
    return m_eVideoStandard;
}



AVD_VideoStandardType msAPI_AVD_GetCurrentVideoStandard(void)
{
    return m_eCurrentVideoStandard;
}


/******************************************************************************/
///- This function will check if Video format changed
/// @return MS_BOOL: Video format changed or not
///- TRUE: Video format changed
///- FALSE: Not changed
/******************************************************************************/
MS_BOOL msAPI_AVD_IsVideoFormatChanged(void)
{
    if( m_bIsVideoFormatChanged == FALSE )
    {
        return FALSE;
    }

    //m_bIsVideoFormatChanged = FALSE;  //It's replaced by "msAPI_AVD_SetIsVideoFormatChangedAsFalse" 20100330EL
    return TRUE;
}

void msAPI_AVD_SetIsVideoFormatChangedAsFalse(void)  //20100330EL
{
    m_bIsVideoFormatChanged = FALSE;
}

/******************************************************************************/
///- This function will check if Video input changed
/// @return MS_BOOL: Video input changed or not
///- TRUE: Video input changed
///- FALSE: Not changed
/******************************************************************************/
MS_BOOL msAPI_AVD_IsVideoInputChanged(void)
{
    if( m_bIsVideoInputChanged == FALSE )
    {
        return FALSE;
    }

    m_bIsVideoInputChanged = FALSE;

    return TRUE;
}

/******************************************************************************/
///- This function will start Audo standard detection
/******************************************************************************/
void msAPI_AVD_StartAutoStandardDetection(void)
{
    MDrv_AVD_StartAutoStandardDetection();
}

void msAPI_AVD_ForceVideoStandard(AVD_VideoStandardType eVideoStandardType)
{
    MDrv_AVD_ForceVideoStandard(eVideoStandardType);
}

#if 0 // BY 20090403 use MDrv_VD_StartAutoStandardDetection, not supported
void msAPI_AVD_DisableForceMode(void)
{
    MDrv_VD_DisableForceMode();
}
#endif

/******************************************************************************/
///- This function will get result of auto standard detection
/// @return AVD_VideoStandardType: video standard type
/******************************************************************************/
AVD_VideoStandardType msAPI_AVD_GetResultOfAutoStandardDetection(void)
{
    AVD_VideoStandardType eStandard;

    eStandard = msAPI_AVD_GetStandardDetection();

    msAPI_AVD_SetVideoStandard(eStandard);

    return eStandard;
}


#if (PQ_ENABLE_VD_SAMPLING)
void _msAPI_VD_SetSamplingType(AVD_VideoStandardType eStandard)
{
    MS_PQ_VD_Sampling_Info stVDSamplingInfo;
    PQ_VIDEOSTANDARD_TYPE ePQStandard;
    MS_U32 u32AVDFlag;

    switch(eStandard)
    {
    case E_VIDEOSTANDARD_PAL_BGHI:
        ePQStandard = E_PQ_VIDEOSTANDARD_PAL_BGHI;
        break;
    case E_VIDEOSTANDARD_NTSC_M:
        ePQStandard = E_PQ_VIDEOSTANDARD_NTSC_M;
        break;

    case E_VIDEOSTANDARD_SECAM:
        ePQStandard = E_PQ_VIDEOSTANDARD_SECAM;
        break;

    case E_VIDEOSTANDARD_NTSC_44:
        ePQStandard = E_PQ_VIDEOSTANDARD_NTSC_44;
        break;

    case E_VIDEOSTANDARD_PAL_M:
        ePQStandard = E_PQ_VIDEOSTANDARD_PAL_M;
        break;

    case E_VIDEOSTANDARD_PAL_N:
        ePQStandard = E_PQ_VIDEOSTANDARD_PAL_N;
        break;

    case E_VIDEOSTANDARD_PAL_60:
        ePQStandard = E_PQ_VIDEOSTANDARD_PAL_60;
        break;


    default:
        ePQStandard = E_PQ_VIDEOSTANDARD_NOTSTANDARD;
        break;

    }

    if(ePQStandard != E_PQ_VIDEOSTANDARD_NOTSTANDARD)
    {
        u32AVDFlag = MDrv_AVD_GetFlag() & ~(AVD_PATCH_HTOTAL_MASK);
        memset(&stVDSamplingInfo, 0, sizeof(MS_PQ_VD_Sampling_Info));
        MDrv_PQ_Get_VDSampling_Info(PQ_MAIN_WINDOW, PQ_INPUT_SOURCE_CVBS, ePQStandard, &stVDSamplingInfo);

        if(stVDSamplingInfo.eType == PQ_VD_SAMPLING_ON)
        {
            u32AVDFlag = (u32AVDFlag & ~(AVD_PATCH_HTOTAL_MASK)) | AVD_PATCH_HTOTAL_USER;
            MDrv_AVD_Set_Htt_UserMD(stVDSamplingInfo.u16Htt);
            MDrv_AVD_SetFlag(u32AVDFlag);
        }
        else
        {
            u32AVDFlag = (u32AVDFlag & ~(AVD_PATCH_HTOTAL_MASK)) | AVD_PATCH_HTOTAL_1135;
            MDrv_AVD_SetFlag(u32AVDFlag);
        }


        //printf("VD Sampling SD=%d Sample=%d, Flag=%x\n", stVDSamplingInfo.eType, u32AVDFlag);
    }
}
#endif


/******************************************************************************/
///- This function will set video standard
/// @param eDetectedStandard \b IN: video standard detected
/// -@see AVD_VideoStandardType
/******************************************************************************/
void msAPI_AVD_SetVideoStandard(AVD_VideoStandardType eDetectedStandard)
{
    AVD_VideoStandardType eStandard;


    //msAPI_Memory_DumpDRAM(0,16,56);
    if( eDetectedStandard == E_VIDEOSTANDARD_NOTSTANDARD )
    {
    #ifdef ATSC_SYSTEM
        eStandard=E_VIDEOSTANDARD_NTSC_M;
    #else
        eStandard = m_eVideoStandard;
    #endif
    }
    else
    {
        eStandard = eDetectedStandard;
    }


#if(PQ_ENABLE_VD_SAMPLING)
    _msAPI_VD_SetSamplingType(eStandard);
#endif

#ifdef ATSC_SYSTEM
    MDrv_AVD_SetVideoStandard(eStandard, 0);
#else
    MDrv_AVD_SetVideoStandard(eStandard, 0);
#endif

//    msAPI_TTX_SetVideoStandard(eStandard);

    m_bIsVideoFormatChanged = TRUE;

    m_eVideoStandard = eStandard;

    switch( m_eVideoStandard ) // TODO should remove mvideo_vd_set_videosystem()
    {
        case E_VIDEOSTANDARD_PAL_BGHI:  mvideo_vd_set_videosystem(SIG_PAL);       break;
        case E_VIDEOSTANDARD_NTSC_M:    mvideo_vd_set_videosystem(SIG_NTSC);      break;
        case E_VIDEOSTANDARD_SECAM:     mvideo_vd_set_videosystem(SIG_SECAM);     break;
        case E_VIDEOSTANDARD_NTSC_44:   mvideo_vd_set_videosystem(SIG_NTSC_443);  break;
        case E_VIDEOSTANDARD_PAL_M:     mvideo_vd_set_videosystem(SIG_PAL_M);     break;
        case E_VIDEOSTANDARD_PAL_N:     mvideo_vd_set_videosystem(SIG_PAL_NC);    break;
        case E_VIDEOSTANDARD_PAL_60:    mvideo_vd_set_videosystem(SIG_NTSC_443);  break;
        default:                        mvideo_vd_set_videosystem(SIG_PAL);       break;
    }
        //msAPI_Memory_DumpDRAM(0,16,60);
  #if 0
    msAPI_AVD_ImprovePictureQuality(TTX_DISPLAY_PICTURE_MODE);
  #endif
}

/******************************************************************************/
///- This function will get aspect ratio code
/// @return ASPECT_RATIO_TYPE : aspect ratio type
/******************************************************************************/
ASPECT_RATIO_TYPE msAPI_AVD_GetAspectRatioCode(void)
{
    if ( IS_WSS_ENABLED == FALSE )
    {
        return ARC_INVALID;
    }

    return m_eAspectRatioCode;
}

/******************************************************************************/
///- This function will check if aspect ratio is changed or not
/// @return MS_BOOL : aspect ratio is changed or not
/// - TRUE: aspect ratio is changed
/// - FALSE: Not changed
/******************************************************************************/
MS_BOOL msAPI_AVD_IsAspectRatioChanged(void)
{
    if ( IS_WSS_ENABLED == FALSE )
    {
        return FALSE;
    }

    if ( m_bIsAspectRatioChanged == FALSE )
    {
        return FALSE;
    }

    m_bIsAspectRatioChanged = FALSE;

    return TRUE;
}

/******************************************************************************/
///- This function will be used for waiting Video sync lock
/******************************************************************************/
void msAPI_AVD_WaitForVideoSyncLock(void)
{
    BYTE cLoopStop;

    for(cLoopStop=0; cLoopStop < 10; cLoopStop++)
    {
        if( TRUE == MDrv_AVD_IsSyncLocked() )
        {
            break;
        }
        MsOS_DelayTask(20);
    }
}
/******************************************************************************/
///- This function will set Video factory data
/******************************************************************************/

static void msAPI_AVD_SetFactoryMode(void)
{
#if ENABLE_VD_PACH_IN_CHINA
    MDrv_AVD_SetRegValue(BK_AFEC_D4,stGenSetting.g_FactorySetting.u8AFEC_D4);
    MDrv_WriteByteMask(0x1035D5, (stGenSetting.g_FactorySetting.u8AFEC_D5_Bit2<<2), BIT2);
    MDrv_WriteByteMask(0x1035D8, stGenSetting.g_FactorySetting.u8AFEC_D8_Bit3210, 0x0F);
    MDrv_WriteByteMask(0x1035D9, stGenSetting.g_FactorySetting.u8AFEC_D9_Bit0, BIT0);
#endif
}

/******************************************************************************/
///- This function will get Video source
/// @return AVD_InputSourceType: Video source type
/******************************************************************************/
AVD_InputSourceType msAPI_AVD_GetVideoSource(void)
{
    return m_eVideoSource;
}

/******************************************************************************/
///- This function will enable force videostandard flag
/// @param bEnable \b IN: enable or disable
/******************************************************************************/
void msAPI_AVD_SetForceVideoStandardFlag(MS_BOOL bEnable)
{
    m_bForceVideoStandardFlag = bEnable;
}

/******************************************************************************/
///- This function will return force videostandard flag
/// @return MS_BOOL: enable or disable
/******************************************************************************/
MS_BOOL msAPI_AVD_GetForceVideoStandardFlag(void)
{
    return m_bForceVideoStandardFlag;
}

/******************************************************************************/
///- This function will set Video source
/// @param eSource \b IN: Video source type
/// -@see AVD_InputSourceType
/******************************************************************************/
//------Input Source Mux--------------------------------------------------------
#define INPUT_VGA_MUX               INPUT_PORT_ANALOG0
#define INPUT_VGA_SYNC_MUX          INPUT_PORT_ANALOG0_SYNC
#define INPUT_YPBPR_MUX             INPUT_PORT_ANALOG0
#define INPUT_AV_YMUX				INPUT_PORT_YMUX_CVBS0
#define INPUT_SV_YMUX               INPUT_PORT_YMUX_CVBS2
#define INPUT_SV_CMUX               INPUT_PORT_CMUX_CVBS1

#define INPUT_TV_YMUX               INPUT_PORT_YMUX_CVBS3 //INPUT_PORT_YMUX_CVBS2
#define INPUT_AV2_YMUX              INPUT_PORT_NONE_PORT
#define INPUT_AV3_YMUX              INPUT_PORT_NONE_PORT
#define INPUT_SV2_YMUX              INPUT_PORT_NONE_PORT
#define INPUT_SV2_CMUX              INPUT_PORT_NONE_PORT
#define INPUT_SCART_YMUX            INPUT_PORT_YMUX_CVBS3
#define INPUT_SCART_RGBMUX          INPUT_PORT_ANALOG2
#define INPUT_SCART_FB_MUX          SCART_FB1
#define INPUT_SCART2_YMUX           INPUT_PORT_NONE_PORT
#define INPUT_SCART2_RGBMUX         INPUT_PORT_NONE_PORT
#define INPUT_SCART2_FB_MUX         SCART_FB2
//------Input Source Mux--------------------------------------------------------

void msAPI_AVD_SetVideoSource(AVD_InputSourceType eSource)
{
    AVD_KDBG("msAPI_AVD_SetVideoSource:%d \r\n", eSource);
    AVD_KDBG("m_eVideoSource:%d \r\n", m_eVideoSource);

    msAPI_AVD_SetFactoryMode();

	//TODO!! check if target source as same as before, return right away!!//
	if ( eSource!=(MDrv_AVD_GetInfo()->eVDInputSource) )
	{

		switch(eSource)
		{
		case E_INPUT_SOURCE_ATV:
			MDrv_AVD_SetInput(E_INPUT_SOURCE_ATV,(MS_U8)INPUT_SCART_FB_MUX);
			break;

    case E_INPUT_SOURCE_CVBS1:
        MDrv_AVD_SetInput(E_INPUT_SOURCE_CVBS1,(MS_U8)INPUT_SCART_FB_MUX);
        break;

    case E_INPUT_SOURCE_CVBS2:
        MDrv_AVD_SetInput(E_INPUT_SOURCE_CVBS2,(MS_U8)INPUT_SCART_FB_MUX);
        break;

    case E_INPUT_SOURCE_CVBS3:
        MDrv_AVD_SetInput(E_INPUT_SOURCE_CVBS3,(MS_U8)INPUT_SCART_FB_MUX);
        break;

    case E_INPUT_SOURCE_SVIDEO1:
        MDrv_AVD_SetInput(E_INPUT_SOURCE_SVIDEO1,(MS_U8)INPUT_SCART_FB_MUX);
        break;

    case E_INPUT_SOURCE_SVIDEO2:
        MDrv_AVD_SetInput(E_INPUT_SOURCE_SVIDEO2,(MS_U8)INPUT_SCART_FB_MUX);
        break;

    case E_INPUT_SOURCE_SCART1:
        MDrv_AVD_SetInput(E_INPUT_SOURCE_SCART1,(MS_U8)INPUT_SCART_FB_MUX);
        break;

    case E_INPUT_SOURCE_SCART2:
        MDrv_AVD_SetInput(E_INPUT_SOURCE_SCART2,(MS_U8)INPUT_SCART2_FB_MUX);
        break;

    case E_INPUT_SOURCE_YPbPr: // TODO remove this item
        break;

		default:
			return;
		}
	}

  #if (0)//(FRONTEND_IF_DEMODE_TYPE == MSTAR_VIF) ToDo!!!
    if (eSource==E_INPUT_SOURCE_ATV)
    {
        MDrv_WriteRegBit(0x101E27L, TRUE, BIT0);
        MDrv_WriteRegBit(0x101E0BL, FALSE, BIT0);
    }
    else
    {
        MDrv_WriteRegBit(0x101E27L, FALSE, BIT0);
    }
  #endif

    msAPI_AVD_SetForcedFreeRun(FALSE);

    m_eVideoSource = eSource;
    m_eVideoStandard = E_VIDEOSTANDARD_NOTSTANDARD;
    m_eCurrentVideoStandard = E_VIDEOSTANDARD_NOTSTANDARD;

    m_bIs3DCombfilterUsed = FALSE;
    VDScart1SrcType = E_SCART_SRC_TYPE_UNKNOWN;

    m_eStandardDetectionState = E_DETECTION_START;
    m_eFSMDetectedStandard = E_VIDEOSTANDARD_NOTSTANDARD;
}

/******************************************************************************/
///- This function will detect video standard
/// @param eOperationMethod \b IN: Video source type
/// -@see OPERATIONMETHOD
/******************************************************************************/
#ifndef ATSC_SYSTEM
void msAPI_AVD_DetectVideoStandard(OPERATIONMETHOD eOperationMethod)
{
    AVD_VideoStandardType    eStandard;

    //if( eOperationMethod == OPERATIONMETHOD_AUTOMATICALLY ) // <-<<< OPTIMIZE
    {
        msAPI_AVD_StartAutoStandardDetection();
        eStandard = msAPI_AVD_GetStandardDetection();

      #if 0
        if( eStandard == E_VIDEOSTANDARD_NOTSTANDARD )
        {
            eOperationMethod = OPERATIONMETHOD_MANUALLY;
        }
      #else // Modified by coverity_0464
        UNUSED(eOperationMethod);
      #endif
    }

    msAPI_AVD_SetVideoStandard(eStandard);

    // This is a input source base color standard.
    if( eStandard != E_VIDEOSTANDARD_NOTSTANDARD )
    {
    #if 0 //ToDo!!!
        if( m_eVideoSource == E_INPUT_SOURCE_ATV && TRUE == m_bIsAutoAV2Active )
        {
            msAPI_ATV_SetLastVideoStandard(E_INPUT_SOURCE_SCART2, eStandard);
        }
        else if( m_eVideoSource == E_INPUT_SOURCE_ATV && TRUE == m_bIsAutoAV1Active )
        {
            msAPI_ATV_SetLastVideoStandard(E_INPUT_SOURCE_SCART1, eStandard);
        }
        else
        {
            msAPI_ATV_SetLastVideoStandard(m_eVideoSource, eStandard);
        }
    #endif
    }

#if 0 //ToDo!!
    // This is a channel base color standard.
    if ( m_eVideoSource == E_INPUT_SOURCE_ATV && FALSE == m_bIsAutoAV2Active  && FALSE == m_bIsAutoAV1Active)
    {
        if( TRUE == msAPI_Tuner_IsCurrentChannelAndSavedChannelSame() )
        {
            msAPI_ATV_SetVideoStandardOfProgram(msAPI_ATV_GetCurrentProgramNumber(), m_eVideoStandard);
        }
    }
#endif
}


/******************************************************************************/
///- This function will get Vertical frequency
/// @return AVD_VideoFreq: Video frequency
/******************************************************************************/
AVD_VideoFreq msAPI_AVD_GetVerticalFreq(void)
{
    return MDrv_AVD_GetVerticalFreq();
}

/******************************************************************************/
///- This function will check if sync is changed or NOT
/// @return MS_BOOL:
/// - TRUE: Sync is changed
/// - FALSE: Sync is not changed
/******************************************************************************/
MS_BOOL msAPI_AVD_IsSyncChanged(void)
{
    if( m_bIsSyncChanged == FALSE )
    {
        return FALSE;
    }

    m_bIsSyncChanged = FALSE;
    return TRUE;
}

/******************************************************************************/
///- This function will check if sync is locked or NOT
/// @return MS_BOOL:
/// - TRUE: Sync is locked
/// - FALSE: Sync is not locked
/******************************************************************************/
MS_BOOL msAPI_AVD_IsSyncLocked(void)
{
    return MDrv_AVD_IsSyncLocked();
}

/******************************************************************************/
///- This function will update auto AV state
/// @return MS_BOOL: status
/// - TRUE: Success
/// - FALSE: Fail
/******************************************************************************/
MS_BOOL msAPI_AVD_UpdateAutoAVState(void)
{
#if 0
    MS_U16 cCurrentScart1ID;
    MS_U16 cCurrentScart2ID;
    MS_BOOL bResult = FALSE;

    if( FALSE == IS_SCART_ENABLED )
    {
        return FALSE;
    }

    cCurrentScart1ID = GetScart1IDLevel();

    cCurrentScart2ID = GetScart2IDLevel();

    //printf("Scart1ID Level = %bd\n", cCurrentScart1ID);
    //printf("Scart2ID Level = %bd\n", cCurrentScart2ID);

    if( m_cPreviousScart2IDLevelForAutoAV < SCART_16_9_LEVEL && SCART_16_9_LEVEL <= cCurrentScart2ID )
    {
        if( m_bIsAutoAV2Active == FALSE )
        {
            m_bIsAutoAV2Active = TRUE;
            bResult = TRUE;
        }
    }
    else if( cCurrentScart2ID < SCART_16_9_LEVEL && SCART_16_9_LEVEL <= m_cPreviousScart2IDLevelForAutoAV )
    {
        if( m_bIsAutoAV2Active == TRUE )
        {
            m_bIsAutoAV2Active = FALSE;
            bResult = TRUE;
        }
    }
    else if( m_cPreviousScart1IDLevelForAutoAV < SCART_16_9_LEVEL && SCART_16_9_LEVEL <= cCurrentScart1ID )
    {
        if( m_bIsAutoAV1Active == FALSE )
        {
            m_bIsAutoAV1Active = TRUE;
            bResult = TRUE;
        }
    }
    else if( cCurrentScart1ID < SCART_16_9_LEVEL && SCART_16_9_LEVEL <= m_cPreviousScart1IDLevelForAutoAV )
    {
        if( m_bIsAutoAV1Active == TRUE )
        {
            m_bIsAutoAV1Active = FALSE;
            bResult = TRUE;
        }
    }

    /*
    if(bResult == TRUE)
    {
        printf("\nSCART2=%x, SCART1=%x \n",m_bIsAutoAV2Active,m_bIsAutoAV1Active);
    }
    */

    m_cPreviousScart1IDLevelForAutoAV = cCurrentScart1ID;
    m_cPreviousScart2IDLevelForAutoAV = cCurrentScart2ID;

    return bResult;
#else
    return FALSE;
#endif
}

/******************************************************************************/
///- This function will check if Auto AV is active
/// @param eAutoAVSource \b IN: Auto AV source
/// -@see AVD_AutoAVSourceType
/// @return MS_BOOL: status
/// - TRUE: Auto AV is active
/// - FALSE: Not active
/******************************************************************************/
MS_BOOL msAPI_AVD_IsAutoAVActive(AVD_AutoAVSourceType eAutoAVSource)
{
    if( FALSE == IS_SCART_ENABLED )
    {
        return FALSE;
    }

    switch( eAutoAVSource )
    {
    case E_AUTOAV_SOURCE_1:
        return m_bIsAutoAV1Active;

    case E_AUTOAV_SOURCE_2:
        return m_bIsAutoAV2Active;

    case E_AUTOAV_SOURCE_ALL:
        return (m_bIsAutoAV1Active || m_bIsAutoAV2Active);

    default:
        return FALSE; // Need touch. Invaild parameter.
    }
}

/******************************************************************************/
///- This function will turn off Auto AV
/******************************************************************************/
void msAPI_AVD_TurnOffAutoAV(void)
{
    if( m_bIsAutoAV1Active == FALSE && m_bIsAutoAV2Active == FALSE)
    {
        return;
    }
    m_bIsAutoAV1Active = FALSE;
    m_bIsAutoAV2Active = FALSE;

    msAPI_AVD_SwitchAutoAV();
}

/******************************************************************************/
///- This function will switch Auto AV
/******************************************************************************/
void msAPI_AVD_SwitchAutoAV(void)
{
    m_bIsVideoInputChanged = TRUE;
#if 0
    if(m_bIsAutoAV2Active == TRUE || m_bIsAutoAV1Active == TRUE )
    {
        msAPI_AUD_AdjustAudioFactor(E_ADJUST_AUDIOMUTE,E_AUDIO_MOMENT_MUTEON,E_AUDIOMUTESOURCE_ATV);
        msAPI_Timer_Delayms(DELAY_FOR_ENTERING_MUTE);

        // BY 20090406 msAPI_AVD_SetVideoMuteDuringLimitedTime(DELAY_FOR_STABLE_VIDEO);

        //printf("msAPI_AVD_TurnOnAutoAV\n");
        msAPI_AVD_TurnOnAutoAV();

        msAPI_AUD_AdjustAudioFactor(E_ADJUST_AUDIOMUTE,E_AUDIO_MOMENT_MUTEOFF,E_AUDIOMUTESOURCE_ATV);
    }
    else
    {
        msAPI_AUD_AdjustAudioFactor(E_ADJUST_AUDIOMUTE,E_AUDIO_MOMENT_MUTEON,E_AUDIOMUTESOURCE_ATV);
        msAPI_Timer_Delayms(DELAY_FOR_ENTERING_MUTE);

        // BY 20090406 msAPI_AVD_SetVideoMuteDuringLimitedTime(DELAY_FOR_STABLE_TUNER);

        #if 0
        //printf("msAPI_AVD_PriTurnOffAutoAV\n");
        msAPI_AVD_PriTurnOffAutoAV();
        #endif

        msAPI_AUD_AdjustAudioFactor(E_ADJUST_AUDIOMUTE,E_AUDIO_MOMENT_MUTEOFF,E_AUDIOMUTESOURCE_ATV);
    }
#endif
}

/******************************************************************************/
///- This function will get Scart 1 source type
/// @return SCART_SOURCE_TYPE: SCART 1 source type
/******************************************************************************/
SCART_SOURCE_TYPE msAPI_AVD_GetScart1SrcType ( void )
{
    if( FALSE == IS_SCART_ENABLED )
    {
        return E_SCART_SRC_TYPE_CVBS;
    }

    //if ( VDScart1SrcType == E_SCART_SRC_TYPE_UNKNOWN )
    {
        msAPI_AVD_RetrieveScart1SourceType();
    }

    return VDScart1SrcType;
}

/******************************************************************************/
///- This function will check if Scart 1 source type is changed or NOT
/// @return MS_BOOL: SCART 1 source type is changed or not
/// - TRUE: source type changed
/// - FALSE: not changed
/******************************************************************************/
MS_BOOL msAPI_AVD_IsScart1SourceTypeChanged(void)
{
    if( FALSE == IS_SCART_ENABLED )
    {
        return FALSE;
    }

    if( m_bIsScart1SourceTypeChanged == FALSE )
    {
        return FALSE;
    }
    m_bIsScart1SourceTypeChanged = FALSE;
    return TRUE;
}

/******************************************************************************/
///- This function will clear Aspect ratio
/******************************************************************************/
void msAPI_AVD_ClearAspectRatio(void)
{
    if( m_eAspectRatioCode == ARC_INVALID )
    {
        m_bIsAspectRatioChanged = FALSE;
    }
    else
    {
        m_bIsAspectRatioChanged = TRUE;
    }

    m_eAspectRatioCode = ARC_INVALID;
}

/******************************************************************************/
///- This function will suppress auto AV
/******************************************************************************/
void msAPI_AVD_SuppressAutoAV(void)
{
    msAPI_AVD_UpdateAutoAVState();
    m_bIsAutoAV1Active = FALSE;
    m_bIsAutoAV2Active = FALSE;
}

#if 0 // BY 20090403 unused function
/******************************************************************************/
///- This function will enable/disable Auto gain control
/// @param bEnable: enable/disable Auto gain control
/// - TRUE: enable Auto gain control
/// - FALSE: disable Auto gain control
/******************************************************************************/
void msAPI_AVD_EnableAutoGainControl(MS_BOOL bEnable)
{
    MDrv_VD_EnableAutoGainControl(bEnable);
}
#endif

/******************************************************************************/
///- This function will enable/disable Forced free run
/// @param bEnable: enable/disable Forced free run
/// - TRUE: enable Forced free run
/// - FALSE: disable Forced free run
/******************************************************************************/
void msAPI_AVD_SetForcedFreeRun(MS_BOOL bEnable)
{
#if FORCEDFREEFUN == TRUE
    AVD_VideoStandardType eVideoStandard;

    if( bEnable == FALSE )
    {
        MDrv_AVD_SetFreerunFreq(E_FREERUN_FQ_AUTO);
    }
    else
    {
        eVideoStandard = msAPI_AVD_GetVideoStandard();

        switch(eVideoStandard)
        {
        case E_VIDEOSTANDARD_PAL_BGHI:
        case E_VIDEOSTANDARD_SECAM:
        case E_VIDEOSTANDARD_PAL_N:
            MDrv_AVD_SetFreerunFreq(E_FREERUN_FQ_50Hz);
            break;

        case E_VIDEOSTANDARD_NTSC_M:
        case E_VIDEOSTANDARD_NTSC_44:
        case E_VIDEOSTANDARD_PAL_M:
        case E_VIDEOSTANDARD_PAL_60:
            MDrv_AVD_SetFreerunFreq(E_FREERUN_FQ_60Hz);
            break;
        }
    }
#else
    bEnable = bEnable; // delete waring
#endif
}

/******************************************************************************/
///- This function will enable/disable Horizontal sync detection for tuning
/// @param bEnable: enable/disable Hsync detection
/// - TRUE: enable Hsync detection
/// - FALSE: disable Hsync detection
/******************************************************************************/
void msAPI_AVD_SetHsyncDetectionForTuning(MS_BOOL bEnable)
{
    MDrv_AVD_SetHsyncDetectionForTuning(bEnable);
    if(bEnable)
    {
        _u8AutoTuningIsProgress = 1;
    }
    else
    {
        _u8AutoTuningIsProgress = 0;
    }
}

void msAPI_AVD_3DCombSpeedup(void)
{
    MDrv_AVD_3DCombSpeedup();
}

void msAPI_AVD_SetChannelChange(void)
{
    MDrv_AVD_SetChannelChange();
    MDrv_AVD_3DCombSpeedup();

  #ifdef ATSC_SYSTEM
    m_eVideoStandard = E_VIDEOSTANDARD_NOTSTANDARD;
  #endif
}

/******************************************************************************/
///- This function will set where VD F/W is on.
/// @param bFWStatus: VD F/W Status
/// - TRUE: VD F/W is on Nand Flash
/// - FALSE: VD F/W is on SPI Flash
/******************************************************************************/
void msAPI_AVD_SetFWStatus(MS_BOOL bFWStatus)
{
    bVD_FWStatus = bFWStatus;
}

//****************************************************************************
// Start of private implementation
//****************************************************************************

MS_U8 msAPI_AVD_CheckStdDetStableCnt(void)
{
    return u8StdDetStableCnt;
}

// please call every 10ms


static void msAPI_AVD_CheckVideoStandard(void)
{
#if 1 // 20090722
    MS_U16 u16Status;
    AVD_VideoStandardType eDetectedStandardCurrent;
    eDetectedStandardCurrent = MDrv_AVD_GetStandardDetection(&u16Status);

    //RETAILMSG(TRUE,(TEXT("Std: %d  State:%d Cnt:%d\r\n"), eDetectedStandardCurrent, m_eStandardDetectionState, u8StdDetStableCnt));
    switch (m_eStandardDetectionState)
    {
        case E_DETECTION_START:
            //printf("\r\n E_DETECTION_START");
            m_eFSMDetectedStandard=eDetectedStandardCurrent;
            u8StdDetStableCnt=VD_START_DETECT_COUNT;
            m_eStandardDetectionState=E_DETECTION_WAIT;
            break;

        case E_DETECTION_WAIT:
            //printf("\r\n WAIT %d %d (%d)", m_eFSMDetectedStandard, eDetectedStandardCurrent, u8StdDetStableCnt);
            if ( m_eFSMDetectedStandard == eDetectedStandardCurrent)
            {
                MS_U16 u16DebounceValue;

                if (eDetectedStandardCurrent==E_VIDEOSTANDARD_NOTSTANDARD)
                {
                    u16DebounceValue=SIGNAL_CHANGE_THRESHOLD; // 20100210 increase debouncing time for switch source
                }
                else
                {
                    u16DebounceValue =  ((m_eVideoSource==E_INPUT_SOURCE_ATV) ? ATV_DETECT_THRESHOLD : CVBS_DETECT_THRESHOLD);
                }

                if (u8StdDetStableCnt++ > u16DebounceValue)
                {
                    msAPI_AVD_SetVideoStandard (m_eFSMDetectedStandard);
                    // 20100208 BY m_bIsVideoFormatChanged = TRUE;
                    m_u16LatchStatus=u16Status;
                    m_eStandardDetectionState=E_DETECTION_DETECT;
                    u8StdDetStableCnt =VD_STABLE_COUNT;
                    m_eCurrentVideoStandard = m_eFSMDetectedStandard;
                }
            }
            else
            {
                m_eStandardDetectionState=E_DETECTION_START;
            }
            break;
        case E_DETECTION_DETECT: // signal is stable
            //printf("\r\n DETECT %d %d (%d)", m_eFSMDetectedStandard, eDetectedStandardCurrent, u8StdDetStableCnt);
            if ( m_eFSMDetectedStandard != eDetectedStandardCurrent)
            {
                if (u8StdDetStableCnt++ > SIGNAL_CHANGE_THRESHOLD)
                {
                #if 0 //ToDo!!!
                    if( IsSrcTypeDigitalVD(SYS_INPUT_SOURCE_TYPE(MAIN_WINDOW)) )
                    {
                        //printf("\r\n VD standard changed main %d %d",m_eFSMDetectedStandard,eDetectedStandardCurrent);
                        #if (ENABLE_SW_CH_FREEZE_SCREEN)
                        bVideoStandardChanged = TRUE;
                        #else
                        msAPI_Scaler_SetBlueScreen( ENABLE, E_XC_FREE_RUN_COLOR_BLACK, DEFAULT_SCREEN_UNMUTE_TIME, MAIN_WINDOW);
                        #endif
                    }
                #endif
                    m_eStandardDetectionState=E_DETECTION_START;
                }
            }
            else
            {
                u8StdDetStableCnt=VD_STABLE_COUNT;
                #if (ENABLE_SW_CH_FREEZE_SCREEN)
                bVideoStandardChanged = FALSE;
                #endif
            }
            break;
        default:
            m_eStandardDetectionState=E_DETECTION_START;
            break;
    }
#else
    #define VD_STD_DET_STABLE_CNT    0
    static AVD_VideoStandardType eDetectedStandard = E_VIDEOSTANDARD_NOTSTANDARD;
    static AVD_VideoStandardType eDetectedStandardLast = E_VIDEOSTANDARD_NOTSTANDARD;
//    static AVD_VideoStandardType ePreStandard = E_VIDEOSTANDARD_NOTSTANDARD;
    AVD_VideoStandardType ePreStandard;

    ePreStandard=MDrv_AVD_GetLastDetectedStandard();//must be called before GetStandardDetection()
    eDetectedStandard = MDrv_AVD_GetStandardDetection();

    if ((eDetectedStandard != ePreStandard)||(eDetectedStandard != eDetectedStandardLast))
    {
        if( IsSrcTypeDigitalVD(SYS_INPUT_SOURCE_TYPE(MAIN_WINDOW)) )
        {
            msAPI_Scaler_SetBlueScreen( ENABLE, E_XC_FREE_RUN_COLOR_BLACK, DEFAULT_SCREEN_UNMUTE_TIME, MAIN_WINDOW);
        }

        //#if (ENABLE_PIP)
        if(MApi_XC_GetInfo()->u8MaxWindowNum > 0)
        {
            if( IsSrcTypeDigitalVD(SYS_INPUT_SOURCE_TYPE(SUB_WINDOW)) )
            {
                msAPI_Scaler_SetBlueScreen( ENABLE, E_XC_FREE_RUN_COLOR_BLACK, DEFAULT_SCREEN_UNMUTE_TIME, SUB_WINDOW);
            }
        }
        //#endif
        u8StdDetStableCnt = 0;
        eDetectedStandardLast = eDetectedStandard;
    }
    else
    {
        if (u8StdDetStableCnt <= VD_STD_DET_STABLE_CNT)
        {
            if (u8StdDetStableCnt == VD_STD_DET_STABLE_CNT)
            {
                if (eDetectedStandard == E_VIDEOSTANDARD_NOTSTANDARD)
                {
                    m_bIsVideoFormatChanged = TRUE;
                }
                else
                {
                    //printf ("%bu\n", eDetectedStandard);
                    m_eVideoStandard = eDetectedStandard;
                    msAPI_AVD_SetVideoStandard (m_eVideoStandard);
                }
            }

            u8StdDetStableCnt++;
        }
    }

//    ePreStandard = eDetectedStandard;
#endif
}

static void msAPI_AVD_CheckAspectRatio(void)
{
    return;
}

#ifndef ATSC_SYSTEM
static void msAPI_AVD_CheckAutoAV(void)
{
    if( FALSE == IS_SCART_ENABLED )
    {
        return;
    }

    if( TRUE == msAPI_AVD_UpdateAutoAVState() )
    {
        if( E_INPUT_SOURCE_ATV != m_eVideoSource )
        {
            return;
        }
        //printf("[msAPI_AVD_CheckAutoAV]\n");
        msAPI_AVD_SwitchAutoAV();
    }
}
#endif

static void msAPI_AVD_RetrieveScart1SourceType(void)
{
    SCART_SOURCE_TYPE ePreviousScart1SrcType;

    ePreviousScart1SrcType = VDScart1SrcType;

    if( m_eVideoSource == E_INPUT_SOURCE_SCART1 ||
        (m_eVideoSource == E_INPUT_SOURCE_ATV && m_bIsAutoAV1Active == TRUE) )
    {
        if ( TRUE == msAPI_AVD_IsScartRGB() )
        {
            VDScart1SrcType = E_SCART_SRC_TYPE_RGB;
        }
        else
        {
            VDScart1SrcType = E_SCART_SRC_TYPE_CVBS;
        }

        if ( ePreviousScart1SrcType != E_SCART_SRC_TYPE_UNKNOWN &&
             ePreviousScart1SrcType != VDScart1SrcType )
        {
            m_bIsScart1SourceTypeChanged = TRUE;
        }
    }
    else
    {
        VDScart1SrcType = E_SCART_SRC_TYPE_UNKNOWN;
    }
}

MS_BOOL msAPI_AVD_IsScartRGB(void)
{
    return FALSE;
}

#if(PQ_ENABLE_VD_SAMPLING)
void msAPI_AVD_GetCaptureWindowByPQ(MS_WINDOW_TYPE *stCapWin, EN_VD_SIGNALTYPE enVideoSystem, MS_PQ_VD_Sampling_Info *pInfo)
{
    if(pInfo->eType == PQ_VD_SAMPLING_ON)
    {
        stCapWin->x = pInfo->u16Hstart;
        stCapWin->y = pInfo->u16Vstart;
        stCapWin->width = pInfo->u16Hsize;
        stCapWin->height = pInfo->u16Vsize;
    }
    else
    {
        memcpy(stCapWin, &VideoCaptureWinTbl[enVideoSystem], sizeof(MS_WINDOW_TYPE));
    }

    //printf("AVD Get Capture: sd=%d, x:%x y:%x w:%d h:%d\n", enVideoSystem, stCapWin->x, stCapWin->y, stCapWin->width, stCapWin->height);
}
#endif

void msAPI_AVD_GetCaptureWindow(MS_WINDOW_TYPE *stCapWin, EN_VD_SIGNALTYPE enVideoSystem, MS_BOOL IsUseExtVD)
{
    if(0)//IsUseExtVD) ToDo
    {
        memcpy(stCapWin, &ExtVDVideoCapture[enVideoSystem], sizeof(MS_WINDOW_TYPE));
    }
    else
    {
        memcpy(stCapWin, &VideoCaptureWinTbl[enVideoSystem], sizeof(MS_WINDOW_TYPE));
        #if 0 // TODO, add externsion function
        switch (enVideoSystem) // for future extension
        {
            case SIG_NTSC:
                MDrv_AVD_GetCaptureWindow(stCapWin, E_VIDEOSTANDARD_NTSC_M);
                break;
            case SIG_PAL:
                MDrv_AVD_GetCaptureWindow(stCapWin, E_VIDEOSTANDARD_PAL_BGHI);
                break;
            case SIG_SECAM:
                MDrv_AVD_GetCaptureWindow(stCapWin, E_VIDEOSTANDARD_SECAM);
                break;
            case SIG_NTSC_443:
                MDrv_AVD_GetCaptureWindow(stCapWin, E_VIDEOSTANDARD_NTSC_44);
                break;
            case SIG_PAL_M:
                MDrv_AVD_GetCaptureWindow(stCapWin, E_VIDEOSTANDARD_PAL_M);
                break;
            case SIG_PAL_NC:
                MDrv_AVD_GetCaptureWindow(stCapWin, E_VIDEOSTANDARD_PAL_N);
                break;
            default:
                MDrv_AVD_GetCaptureWindow(stCapWin, E_VIDEOSTANDARD_PAL_BGHI);
                break;
        }
        #endif
    }
    //printf("msAPI_AVD_GetCaptureWindow %x %x %d %d\n", stCapWin->x, stCapWin->y, stCapWin->width, stCapWin->height);
}

//VD
static EN_VD_SIGNALTYPE g_ucVideoSystem;

EN_VD_SIGNALTYPE mvideo_vd_get_videosystem(void)
{
    return g_ucVideoSystem;
}

void mvideo_vd_set_videosystem(EN_VD_SIGNALTYPE ucVideoSystem)
{
    g_ucVideoSystem = ucVideoSystem;
}

MS_U16 mvideo_vd_get_videostatus(void)
{
    return m_u16LatchStatus;
}

MS_BOOL IsVDHasSignal(void)
{
    return (m_eCurrentVideoStandard != E_VIDEOSTANDARD_NOTSTANDARD);
}

/******************************************************************************/
///This function will return VD's Horizontal period
///@param enGetSyncType \b IN
///EN_GET_SYNC_TYPE is SYNC type
///-GET_SYNC_DIRECTLY
///-GET_SYNC_STABLE
///-GET_SYNC_VIRTUAL
///@return
///- MS_U16 VD Horizontal Period
/******************************************************************************/
MS_U16 msAPI_Scaler_VD_GetHPeriod (SCALER_WIN eWindow, INPUT_SOURCE_TYPE_t eCurrentSrc,EN_GET_SYNC_TYPE enGetSyncType)
{
    MS_U8 u8StableCounter;
    XC_IP_SYNC_STATUS sXC_SyncStatus;

    switch(enGetSyncType)
    {
        case GET_SYNC_DIRECTLY:
            MApi_XC_GetSyncStatus(eCurrentSrc, &sXC_SyncStatus, eWindow);
            break;
        case GET_SYNC_STABLE:
        case GET_SYNC_VIRTUAL:
            for(u8StableCounter = 0 ; u8StableCounter < 5 ; u8StableCounter++)
            {
                MApi_XC_GetSyncStatus(eCurrentSrc, &sXC_SyncStatus, eWindow);
                if( (MApi_XC_CalculateHFreqx10(sXC_SyncStatus.u16Hperiod) >= 135) &&
                    (MApi_XC_CalculateHFreqx10(sXC_SyncStatus.u16Hperiod) <= 178) )
                {
                    break;
                }
                else
                {
                    if(enGetSyncType==GET_SYNC_VIRTUAL)
                        sXC_SyncStatus.u16Hperiod = 0;
                }
            }
            if((enGetSyncType == GET_SYNC_VIRTUAL) && (sXC_SyncStatus.u16Hperiod == 0))
            {
                if(sXC_SyncStatus.u16Vtotal > 566)
                    sXC_SyncStatus.u16Hperiod = MST_XTAL_CLOCK_HZ/15625;
                else
                    sXC_SyncStatus.u16Hperiod = MST_XTAL_CLOCK_HZ/15734;
            }
            break;
    }
    return sXC_SyncStatus.u16Hperiod;
}

/******************************************************************************/
///This function will return VD's Vertical total
///@param enGetSyncType \b IN
///enGetSyncType is SYNC type
///- GET_SYNC_DIRECTLY
///- GET_SYNC_STABLE
///- GET_SYNC_VIRTUAL
///@param u16HFreqX10 \b IN
///- Horizontal frequncy
///@return
///- MS_U16: VD Vertical Total
/******************************************************************************/
MS_U16 msAPI_Scaler_VD_GetVTotal (SCALER_WIN eWindow, INPUT_SOURCE_TYPE_t eCurrentSrc,EN_GET_SYNC_TYPE enGetSyncType, MS_U16 u16HFreqX10)
{
    MS_U8 u8StableCounter = 0;
    XC_IP_SYNC_STATUS sXC_Sync_Status;

    switch(enGetSyncType)
    {
        case GET_SYNC_DIRECTLY:
            MApi_XC_GetSyncStatus(eCurrentSrc, &sXC_Sync_Status, eWindow);
            break;
        case GET_SYNC_STABLE:
        case GET_SYNC_VIRTUAL:
            for(u8StableCounter = 0 ; u8StableCounter < 3 ; u8StableCounter++)
            {
                MApi_XC_GetSyncStatus(eCurrentSrc, &sXC_Sync_Status, eWindow);
                if(sXC_Sync_Status.u16Vtotal >= 465 && sXC_Sync_Status.u16Vtotal <= 730)//+-7Hz if(u16VerticalTotal >= 510 && u16VerticalTotal <= 640)
                {
                    break;
                }
                else
                {
                    if(enGetSyncType==GET_SYNC_VIRTUAL)
                        sXC_Sync_Status.u16Vtotal = 0;
                }
            }
            if(enGetSyncType==GET_SYNC_VIRTUAL && sXC_Sync_Status.u16Vtotal==0)
            {
                if(u16HFreqX10 == 156)
                    sXC_Sync_Status.u16Vtotal = 625;
                else
                    sXC_Sync_Status.u16Vtotal = 525;
            }
            break;
    }
    return sXC_Sync_Status.u16Vtotal;
}

STANDARD_DETECTION_STATE msAPI_AVD_ChekcStdDetStableState(void)
{
    return m_eStandardDetectionState;
}

#endif

