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
#ifndef MDRV_SCA_HDMITX_H
#define MDRV_SCA_HDMITX_H

//------------------------------------------------------------------------------
// Default Contrast & Saturation & Hue & Sharpness Value
//------------------------------------------------------------------------------

#define HDMITX_ANALOG_TUNING_SD		    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define HDMITX_ANALOG_TUNING_HD         {0x00, 0x00, 0x00, 0x00, 0x00, 0x07}
#define HDMITX_ANALOG_TUNING_DEEP_HD    {0x00, 0x00, 0x00, 0x00, 0x00, 0x07}


// Contrast
typedef enum
{
    E_XC_HDMITX_OUTPUT_HDMI = 0,
    E_XC_HDMITX_OUTPUT_HDMI_HDCP,
    E_XC_HDMITX_OUTPUT_DVI,
    E_XC_HDMITX_OUTPUT_DVI_HDCP,
}E_XC_HDMITX_OUTPUT_MODE;

typedef enum
{
    E_XC_HDMITX_CD_8BITS = 0,
    E_XC_HDMITX_CD_10BITS,
    E_XC_HDMITX_CD_12BITS,
    E_XC_HDMITX_CD_AUTO,
}E_XC_HDMITX_CD_TYPE;

typedef enum
{
    E_XC_HDMITX_480_60I,
    E_XC_HDMITX_480_60P,
    E_XC_HDMITX_576_50I,
    E_XC_HDMITX_576_50P,
    E_XC_HDMITX_720_50P,
    E_XC_HDMITX_720_60P,
    E_XC_HDMITX_1080_50I,
    E_XC_HDMITX_1080_50P,
    E_XC_HDMITX_1080_60I,
    E_XC_HDMITX_1080_60P,
    E_XC_HDMITX_1080_30P,
    E_XC_HDMITX_1080_25P,
    E_XC_HDMITX_1080_24P,
}E_XC_HDMITX_OUTPUT_TIMING_TYPE;

typedef struct
{
    HDMITX_ANALOG_TUNING stSDInfo;      //clock < 75MHz
    HDMITX_ANALOG_TUNING stHDInfo;      //clock < 165MHz
    HDMITX_ANALOG_TUNING stDeepHDInfo;  //clock > 165MHz
} XC_HDMITX_BOARD_INFO;

typedef struct
{

    E_XC_HDMITX_OUTPUT_MODE eHdmitxOutputMode;        ///<HDMITx Output Mode
    E_XC_HDMITX_CD_TYPE eHdmitxCDType;                ///<HDMITx Color Depth Value
    XC_HDMITX_BOARD_INFO stHdmitxBoardInfo;           ///<HDMITx Analog Configuration of Board
    MS_BOOL bDisableHDMI;
    MS_U16  u16HdmitxHpdPin;
    E_XC_HDMITX_OUTPUT_TIMING_TYPE eHdmitxOutputTiming;
}XC_HDMITX_INITDATA;




#ifdef MDRV_SCA_HDMITX_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

INTERFACE void MApi_Init_HDMITx(XC_HDMITX_INITDATA stInitData);
INTERFACE void MApi_Config_HDMITx(MS_BOOL bEn, E_XC_HDMITX_OUTPUT_MODE enOutputMode, E_XC_HDMITX_OUTPUT_TIMING_TYPE enOutPutTiming);
INTERFACE HDMITX_VIDEO_COLORDEPTH_VAL MApi_Get_HDMITx_CD(void);

#undef INTERFACE

#endif
