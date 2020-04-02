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
//==============================================================================
// [mhal_offline.h]
// Date: 20090220
// Descriptions: Add a new mux layer for HW setting
//==============================================================================
#ifndef MHAL_OFFLINE_H
#define MHAL_OFFLINE_H


#ifdef MHAL_OFFLINE_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

INTERFACE void Hal_XC_SetOffLineToSog_AV( MS_U8 Channel );
INTERFACE void Hal_XC_SetOffLineToSog_YUV( MS_U8 Channel );
INTERFACE void Hal_XC_SetOffLineToHv( MS_U8 Channel );
INTERFACE void Hal_XC_SetOffLineToHDMI( MS_U8 Channel );
INTERFACE void Hal_XC_SetOffLineToUSB( MS_U8 Channel );

INTERFACE MS_U16 Hal_XC_GetOffLineOfV(void);
INTERFACE MS_U16 Hal_XC_GetOffLineOfH(void);
INTERFACE MS_U16 Hal_XC_GetOffLineOfDVI01(void);
INTERFACE MS_U16 Hal_XC_GetOffLineOfDVI23(void);
INTERFACE MS_U16 Hal_XC_GetOffLineOfStorage(void);

INTERFACE void Hal_XC_TurnOnDetectClkPath(MS_U8 u8DetectClkPath);
INTERFACE MS_BOOL Hal_XC_SetOffLineSogThreshold(MS_U8 u8Threshold);
INTERFACE MS_BOOL Hal_XC_SetOffLineSogBW(MS_U8 u8BW);
INTERFACE void Hal_XC_OffLineInit(void);
INTERFACE void Hal_XC_OffLineExit(void);
INTERFACE void Hal_XC_SetOfflineDetectClk(void);
INTERFACE MS_U8 Hal_XC_GetOffLineDetection(MS_U8 u8CheckSrc);
INTERFACE void Hal_XC_WaitForHVCleared_AV(MS_U8 Channel);
INTERFACE void Hal_XC_WaitForHVCleared(MS_U8 Channel);

#undef INTERFACE
#endif // MHAL_OFFLINE_H
