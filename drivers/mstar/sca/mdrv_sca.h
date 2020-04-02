///////////////////////////////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//
// @file   mdrv_sca_io.c
// @brief  SCA KMD Driver Interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////

//=============================================================================
// Include Files
//=============================================================================
#ifndef MDRV_SCA_H
#define MDRV_SCA_H

#define     SCAIO_DEBUG_ENABLE  //

#define SCA_DBG_LV_IOCTL            0
#define SCA_DBG_LV_0                1
#define SCA_DBG_LV_INIT_XC          1
#define SCA_DBG_LV_INPUTSRC         0
#define SCA_DBG_LV_PROGWIN          0
#define SCA_DBG_LV_MONTHREAD        0
#define SCA_DBG_LV_COLOR            0
#define SCA_DBG_LV_MODE_PARSE       0
#define SCA_DBG_LV_DLC              0
#define SCA_DBG_LV_PQ_BIN           1
#define SCA_DBG_LV_SCREEN_MUTE      0
#define SCA_DBG_LV_HDMITX           0
#define SCA_DBG_LV_PQ_COLOR         0
#define SCA_DBG_LV_CALI             0
#define SCA_DBG_LV_COLOR_MATRIX     0
#define SCA_DBG_LV_HDMITX_HP        0

#ifdef      SCAIO_DEBUG_ENABLE
#define     SCAIO_ASSERT(_con)   \
            do {\
                if (!(_con)) {\
                    printk(KERN_CRIT "BUG at %s:%d assert(%s)\n",\
                                                        __FILE__, __LINE__, #_con);\
                    BUG();\
                }\
            } while (0)

#define SCA_DBG(dbglv, _fmt, _args...)             \
    do                                             \
    if(dbglv)                                      \
    {                                              \
        printk(KERN_WARNING _fmt, ## _args);       \
    }while(0)

#else
#define     SCAIO_ASSERT(arg)
#define     SCA_DBG(dbglv, _fmt, _args...)

#endif



#define MSG(x) //

#define ENABLE_2_GOP_BLENDING_PATCH    0

#define PNL_HSTART  80
#define PNL_VSTART  0
#define PNL_WIDTH   800
#define PNL_HEIGHT  480
#define PNL_HTOTAL  928
#define PNL_VTOTAL  525

#define FREQ_12MHZ                      (12000000UL)
#define FREQ_13MHZ                      (13000000UL)
#define MST_XTAL_CLOCK_HZ               FREQ_13MHZ
#define MST_XTAL_CLOCK_KHZ              (MST_XTAL_CLOCK_HZ/1000UL)
#define MST_XTAL_CLOCK_MHZ              (MST_XTAL_CLOCK_KHZ/1000UL)

#define SCALER_DNR_BUF_LEN              (800*600*2*8)
#define SCALER_DNR_BUF_ADR              0x00200000 // 0x40200000
#define SHARE_GND                       DISABLE

#define SCART_ID_SEL                    0x04  // HSYNC2
#define SCART2_ID_SEL                   0
#define ENABLE_FRC                      0


#define PANEL_POWER_VCC              0x01
#define PANEL_POWER_LIGHT_ON         0x02
#define PANEL_POWER_BLUESCREEN       0x04
#define PANEL_POWER_FREEZESCREEN     0x08


#define SYS_INPUT_SOURCE_TYPE(WIN) stSystemInfo[WIN].enInputSourceType

#define IsHDMIInUse()               (msAPI_InputSrcType_InUsedType(EN_SYS_INPUT_SOURCE_TYPE_HDMI))
#define IsVgaInUse()                (msAPI_InputSrcType_InUsedType(EN_SYS_INPUT_SOURCE_TYPE_VGA))
#define IsYPbPrInUse()              (msAPI_InputSrcType_InUsedType(EN_SYS_INPUT_SOURCE_TYPE_YPBPR))
#define IsAVInUse()                 (msAPI_InputSrcType_InUsedType(EN_SYS_INPUT_SOURCE_TYPE_CVBS))
#define IsSVInUse()                 (msAPI_InputSrcType_InUsedType(EN_SYS_INPUT_SOURCE_TYPE_SVIDEO))
#define IsATVInUse()                (msAPI_InputSrcType_InUsedType(EN_SYS_INPUT_SOURCE_TYPE_ATV))
#define IsDTVInUse()                (msAPI_InputSrcType_InUsedType(EN_SYS_INPUT_SOURCE_TYPE_DTV))
#define IsScartInUse()              (msAPI_InputSrcType_InUsedType(EN_SYS_INPUT_SOURCE_TYPE_SCART))
#define IsStorageInUse()            (msAPI_InputSrcType_InUsedType(EN_SYS_INPUT_SOURCE_TYPE_STORAGE))


#ifndef BIT0
#define BIT0  0x0001
#define BIT1  0x0002
#define BIT2  0x0004
#define BIT3  0x0008
#define BIT4  0x0010
#define BIT5  0x0020
#define BIT6  0x0040
#define BIT7  0x0080
#define BIT8  0x0100
#define BIT9  0x0200
#define BIT10 0x0400
#define BIT11 0x0800
#define BIT12 0x1000
#define BIT13 0x2000
#define BIT14 0x4000
#define BIT15 0x8000
#endif

//------------------------------------------------------------------------------
//  struct & enum
//------------------------------------------------------------------------------
typedef enum
{
    EN_SYS_INPUT_SOURCE_TYPE_NONE       = 0x0000,
    EN_SYS_INPUT_SOURCE_TYPE_DTV        = 0x0001,
    EN_SYS_INPUT_SOURCE_TYPE_ATV        = 0x0002,
    EN_SYS_INPUT_SOURCE_TYPE_SCART      = 0x0004,
    EN_SYS_INPUT_SOURCE_TYPE_CVBS       = 0x0008,
    EN_SYS_INPUT_SOURCE_TYPE_VGA        = 0x0010,
    EN_SYS_INPUT_SOURCE_TYPE_HDMI       = 0x0020,
    EN_SYS_INPUT_SOURCE_TYPE_YPBPR      = 0x0040,
    EN_SYS_INPUT_SOURCE_TYPE_SVIDEO     = 0x0080,
    EN_SYS_INPUT_SOURCE_TYPE_STORAGE    = 0x0100,
    EN_SYS_INPUT_SOURCE_TYPE_BT656      = 0x0200,
} EN_SYS_INPUT_SOURCE_TYPE;

// For ScreenMute
typedef enum
{
    E_SCREEN_MUTE_INIT,

    E_SCREEN_MUTE_TEMPORARY = BIT0,

    E_SCREEN_MUTE_RATING =  BIT1,
    E_SCREEN_MUTE_FREERUN = BIT2,
    E_SCREEN_MUTE_CHANNEL = BIT3,
    E_SCREEN_MUTE_BLOCK =   BIT4,
    E_SCREEN_MUTE_MHEG5 =   BIT5,

    E_SCREEN_MUTE_INPUT =   BIT6,       ///< lock some input such as change to AV always mute
    E_SCREEN_MUTE_MONITOR = BIT7,
    E_SCREEN_MUTE_PERMANENT = (E_SCREEN_MUTE_RATING | E_SCREEN_MUTE_FREERUN | E_SCREEN_MUTE_CHANNEL | E_SCREEN_MUTE_BLOCK | E_SCREEN_MUTE_MHEG5 | E_SCREEN_MUTE_INPUT),
}E_SCREEN_MUTE_STATUS;

typedef struct
{
    INPUT_SOURCE_TYPE_t enInputSourceType; //< Input source type
    MS_U8 u8PanelPowerStatus;
} MS_SYS_INFO;

typedef struct
{
    INPUT_SOURCE_TYPE_t enInputSourceType;
}MS_VE_INFO;

typedef struct
{
    MS_BOOL bEn;
    MS_BOOL bUserOnOff;
    MS_BOOL bUserControl;
}MS_DLC_MONITOR_CONFIG;

typedef struct
{
    MS_U32 u32SC0_Main_Phy;
    MS_U32 u32SC1_Main_Phy;
    MS_U32 u32SC2_Main_Phy;
    MS_U32 u32SC0_Sub_Phy;
    MS_U32 u32SC2_Sub_Phy;
    MS_U32 u32SC0_Main_Size;
    MS_U32 u32SC1_Main_Size;
    MS_U32 u32SC2_Main_Size;
    MS_U32 u32SC0_Sub_Size;
    MS_U32 u32SC2_Sub_Size;
    MS_U32 u32MLoad_Phy;
    MS_U32 u32MLoad_Size;
    MS_U32 u32MLoad_Vir;
}XC_INIT_INFO;


typedef struct
{
    SCA_PQ_BIN_ID_TYPE enPQBinIDType;
    MS_U32 u32Len;
    MS_U8 *pBuff;
}SCA_PQ_BIN_DATA;

#ifdef INTERFACE
#undef INTERFACE
#endif

#ifdef  MDRV_SCA_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

#ifndef MDRV_SCA_C

#if PQ_ENABLE_COLOR

INTERFACE MS_U16 S16SC0_ColorCorrectMatrix[32];
INTERFACE MS_U16 S16SC1_ColorCorrectMatrix[32];
INTERFACE MS_U16 S16SC2_ColorCorrectMatrix[32];
INTERFACE MS_U8 tSC0_NormalGammaR[386];
INTERFACE MS_U8 tSC0_NormalGammaG[386];
INTERFACE MS_U8 tSC0_NormalGammaB[386];
INTERFACE MS_U8 *tSC0_AllGammaTab[3];
INTERFACE MS_U8 tSC1_NormalGammaR[386];
INTERFACE MS_U8 tSC1_NormalGammaG[386];
INTERFACE MS_U8 tSC1_NormalGammaB[386];
INTERFACE MS_U8 *tSC1_AllGammaTab[3];

#else

INTERFACE MS_S16  S16DefaultColorCorrectionMatrix[32];
INTERFACE MS_S16  S16ByPassColorCorrectionMatrix[32];
#endif

INTERFACE MS_S16  S16DefaultRGB[3][3];
INTERFACE MS_S16  S16ByPassDefaultRGB[3][3];

#endif


INTERFACE MS_SYS_INFO stSystemInfo[MAX_WINDOW];
INTERFACE MS_BOOL bInitPNL;
INTERFACE MS_DLC_MONITOR_CONFIG g_stDLCMonitorCfg;
//------------------------------------------------------------------------------
INTERFACE void msAPI_InputSrcType_SetType(EN_SYS_INPUT_SOURCE_TYPE enInpSrcType);
INTERFACE EN_SYS_INPUT_SOURCE_TYPE msAPI_InputSrcType_GetType(void);
INTERFACE void msAPI_InputSrcType_ClrType(EN_SYS_INPUT_SOURCE_TYPE enClrType);
INTERFACE MS_BOOL msAPI_InputSrcType_InUsedType(EN_SYS_INPUT_SOURCE_TYPE enInpSrcType);

INTERFACE void MApi_XC_Sys_Init(XC_INIT_INFO stInitInfo);
INTERFACE MS_BOOL MApi_XC_GetSysInitFlag(void);
INTERFACE void MApi_XC_ClearSysInitFlag(void);
INTERFACE void MApi_SetBlueScreen( MS_BOOL bEnable, MS_U8 u8Color, MS_U16 u16ScreenUnMuteTime, SCALER_WIN eWindow);
INTERFACE void MApi_SetScreenMute(E_SCREEN_MUTE_STATUS eScreenMute, MS_BOOL bMuteEnable, MS_U16 u161ms , SCALER_WIN eWindow);
INTERFACE void MApi_XC_Sys_Init_Pnl(void);
INTERFACE MS_BOOL IsRTKCvbsInputLocked(void);
INTERFACE MS_BOOL IsRTKCarBacking(void);
INTERFACE void MApi_XC_Sys_Init_PQ(MS_U16 u16PNL_Width, MS_U16 u16PNL_Vtt);

INTERFACE void Test_input(void);
INTERFACE void Test_Analog(void);
INTERFACE void Test_AVD(void);
INTERFACE void Test_AutoDetect(SCA_SRC_TYPE enCurSrc, SCA_DEST_TYPE enCurDest);

INTERFACE void MApi_XC_Get_ColorCorrectMatix_From_PQ(SCALER_WIN eWindow, MS_S16 *pBuf);
INTERFACE void MApi_XC_GetGamma_From_PQ(SCALER_WIN eWindow, MS_U8 *pTabR, MS_U8 *pTabG, MS_U8 *pTabB);

#if defined(SCA_ENABLE_DLC)
INTERFACE void MApi_XC_Sys_Init_DLC(void);
#endif

INTERFACE PanelType* MApi_GetProjPaneltype(void);

#undef INTERFACE

#endif
