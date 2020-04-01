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

#ifndef _HAL_MVOP_H_
#define _HAL_MVOP_H_


//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define STB_DC                  0   //0 for TV series MVOP; 1 for STB DC.

#define MVOP_MIU_CLIENT_MAIN    MIU_CLIENT_MVOP_128BIT_R
#define MVOP_MIU_CLIENT_SUB     MIU_CLIENT_MVOP1_R

#define _MVOP_128BIT_BUS        0//128-bit MIU bus
#define _MVOP_64BIT_BUS         1// 64-bit MIU bus
#define MVOP_BUS_WIDTH          _MVOP_128BIT_BUS

#define HAL_MVOP_MODULE_CNT     3
#if (HAL_MVOP_MODULE_CNT >= 2)
#define MVOP_SUPPORT_SUB        1
#else
#define MVOP_SUPPORT_SUB        0
#endif
#if (HAL_MVOP_MODULE_CNT >= 3)
#define MVOP_SUPPORT_3RD        1
#else
#define MVOP_SUPPORT_3RD        0
#endif

#define ENABLE_3D_LR_MODE           1 //Enable 3D L/R feature
#define SUPPORT_3DLR_INST_VBLANK    1
#define SUPPORT_3DLR_ALT_SBS        0

#if (STB_DC == 0)
#define MVOP_BASE_ADD_BITS          31  //29 - 1 (MSB is signed bit) + 3 (unit in 8-byte)
#endif

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
typedef enum
{
    HALMVOP_SYNCMODE,
    HALMVOP_FREERUNMODE,
    HALMVOP_160MHZ = 160000000ul,
    HALMVOP_144MHZ = 144000000ul,
    HALMVOP_86MHZ  =  86400000ul,
    HALMVOP_54MHZ  =  54000000ul,
    HALMVOP_27MHZ  =  27000000ul
}HALMVOPFREQUENCY;

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
void HAL_MVOP_RegSetBase(MS_U32 u32Base);
void HAL_MVOP_Init(void);
void HAL_MVOP_SetFieldInverse(MS_BOOL b2MVD, MS_BOOL b2IP);
void HAL_MVOP_SetChromaWeighting(MS_BOOL bEnable);
void HAL_MVOP_LoadReg(void);
void HAL_MVOP_SetMIUReqMask(MS_BOOL bEnable);
void HAL_MVOP_Enable(MS_BOOL bEnable);
MS_BOOL HAL_MVOP_GetEnableState(void);
HALMVOPFREQUENCY HAL_MVOP_GetMaxFreerunClk(void);
void HAL_MVOP_SetFrequency(HALMVOPFREQUENCY enFrequency);
void HAL_MVOP_Rst(void);
void HAL_MVOP_SetBlackBG(void);
void HAL_MVOP_SetOutputInterlace(MS_BOOL bEnable);
void HAL_MVOP_SetCropWindow(MVOP_InputCfg *pparam);
void HAL_MVOP_SetInputMode( VOPINPUTMODE mode, MVOP_InputCfg *pparam );
void HAL_MVOP_EnableUVShift(MS_BOOL bEnable);
void HAL_MVOP_SetEnable60P(MS_BOOL bEnable);
void HAL_MVOP_SetVSyncMode(MS_U8 u8Mode);
void HAL_MVOP_SetOutputTiming( MVOP_Timing *ptiming );
void HAL_MVOP_SetDCClk(MS_U8 clkNum, MS_BOOL bEnable);
void HAL_MVOP_SetSynClk(MVOP_Timing *ptiming);
void HAL_MVOP_SetMonoMode(MS_BOOL bEnable);
void HAL_MVOP_SetH264HardwireMode(void);
void HAL_MVOP_SetRMHardwireMode(void);
void HAL_MVOP_SetJpegHardwireMode(void);
void HAL_MVOP_SetPattern(MVOP_Pattern enMVOPPattern);
MS_BOOL HAL_MVOP_SetTileFormat(MVOP_TileFormat eTileFmt);
MS_BOOL HAL_MVOP_SetRgbFormat(MVOP_RgbFormat eRgbFmt);
MS_BOOL HAL_MVOP_GetSupportRgbFormat(MVOP_RgbFormat eRgbFmt);
MS_BOOL HAL_MVOP_Enable3DLR(MS_BOOL bEnable);
MS_BOOL HAL_MVOP_Get3DLRMode(void);
MS_BOOL HAL_MVOP_Set3DLRInsVBlank(MS_U16 u16InsLines);
MS_BOOL HAL_MVOP_GetTimingInfoFromRegisters(MVOP_TimingInfo_FromRegisters *mvopTimingInfo);
void HAL_MVOP_SetHorizontallMirrorMode(MS_BOOL bEnable);
void HAL_MVOP_SetVerticalMirrorMode(MS_BOOL bEnable);
void HAL_MVOP_SetYUVBaseAdd(MS_U32 u32YOffset, MS_U32 u32UVOffset, MS_BOOL bProgressive, MS_BOOL b422pack);
void HAL_MVOP_SetRepeatField(MVOP_RptFldMode eMode);
void HAL_MVOP_EnableFreerunMode(MS_BOOL bEnable);
MS_U32 HAL_MVOP_GetYBaseAdd(void);
MS_U32 HAL_MVOP_GetUVBaseAdd(void);
MS_BOOL HAL_MVOP_Set3DLRAltOutput(MS_BOOL bEnable);
MS_BOOL HAL_MVOP_Get3DLRAltOutput(void);
MS_BOOL HAL_MVOP_Set3DLR2ndCfg(MS_BOOL bEnable);
MS_BOOL HAL_MVOP_Get3DLR2ndCfg(void);
MVOP_DrvMirror HAL_MVOP_GetMirrorMode(MVOP_DevID eID);
MS_BOOL HAL_MVOP_SetVerDup(MS_BOOL bEnable);
MS_BOOL HAL_MVOP_GetVerDup(void);

void HAL_MVOP_SubRegSetBase(MS_U32 u32Base);
void HAL_MVOP_SubInit(void);
void HAL_MVOP_SubSetFieldInverse(MS_BOOL b2MVD, MS_BOOL b2IP);
void HAL_MVOP_SubSetChromaWeighting(MS_BOOL bEnable);
void HAL_MVOP_SubLoadReg(void);
void HAL_MVOP_SubSetMIUReqMask(MS_BOOL bEnable);
void HAL_MVOP_SubRst(void);
void HAL_MVOP_SubEnable(MS_BOOL bEnable);
MS_BOOL HAL_MVOP_SubGetEnableState(void);
HALMVOPFREQUENCY HAL_MVOP_SubGetMaxFreerunClk(void);
void HAL_MVOP_SubSetFrequency(HALMVOPFREQUENCY enFrequency);
void HAL_MVOP_SubSetOutputInterlace(MS_BOOL bEnable);
void HAL_MVOP_SubSetPattern(MVOP_Pattern enMVOPPattern);
MS_BOOL HAL_MVOP_SubSetTileFormat(MVOP_TileFormat eTileFmt);
MS_BOOL HAL_MVOP_SubSetRgbFormat(MVOP_RgbFormat eRgbFmt);
MS_BOOL HAL_MVOP_SubEnable3DLR(MS_BOOL bEnable);
MS_BOOL HAL_MVOP_SubGet3DLRMode(void);
void HAL_MVOP_SubSetBlackBG(void);
void HAL_MVOP_SubSetCropWindow(MVOP_InputCfg *pparam);
void HAL_MVOP_SubSetInputMode( VOPINPUTMODE mode, MVOP_InputCfg *pparam );
void HAL_MVOP_SubEnableUVShift(MS_BOOL bEnable);
void HAL_MVOP_SubSetEnable60P(MS_BOOL bEnable);
void HAL_MVOP_SubSetOutputTiming( MVOP_Timing *ptiming );
void HAL_MVOP_SubSetDCClk(MS_U8 clkNum, MS_BOOL bEnable);
void HAL_MVOP_SubSetSynClk(MVOP_Timing *ptiming);
void HAL_MVOP_SubSetMonoMode(MS_BOOL bEnable);
void HAL_MVOP_SubSetH264HardwireMode(void);
void HAL_MVOP_SubSetRMHardwireMode(void);
void HAL_MVOP_SubSetJpegHardwireMode(void);
void HAL_MVOP_SubEnableMVDInterface(MS_BOOL bEnable);
MS_BOOL HAL_MVOP_SubGetTimingInfoFromRegisters(MVOP_TimingInfo_FromRegisters *pMvopTimingInfo);
void HAL_MVOP_SubSetHorizontallMirrorMode(MS_BOOL bEnable);
void HAL_MVOP_SubSetVerticalMirrorMode(MS_BOOL bEnable);
void HAL_MVOP_SubEnableFreerunMode(MS_BOOL bEnable);
void HAL_MVOP_SubSetYUVBaseAdd(MS_U32 u32YOffset, MS_U32 u32UVOffset, MS_BOOL bProgressive, MS_BOOL b422pack);
void HAL_MVOP_SubSetRepeatField(MVOP_RptFldMode eMode);

void HAL_MVOP_EX_RegSetBase(MS_U32 u32Base);
void HAL_MVOP_EX_InitMirrorMode(MVOP_DevID eID, MS_BOOL bMir);
MS_BOOL HAL_MVOP_EX_Init(MVOP_DevID eID);
void HAL_MVOP_EX_SetVerticalMirrorMode(MVOP_DevID eID, MS_BOOL bEnable);
void HAL_MVOP_EX_SetHorizontallMirrorMode(MVOP_DevID eID, MS_BOOL bEnable);
void HAL_MVOP_EX_SetFieldInverse(MVOP_DevID eID, MS_BOOL b2MVD, MS_BOOL b2IP);
void HAL_MVOP_EX_SetChromaWeighting(MVOP_DevID eID, MS_BOOL bEnable);
void HAL_MVOP_EX_LoadReg(MVOP_DevID eID);
void HAL_MVOP_EX_SetMIUReqMask(MVOP_DevID eID, MS_BOOL bEnable);
void HAL_MVOP_EX_Rst(MVOP_DevID eID);
void HAL_MVOP_EX_Enable(MVOP_DevID eID, MS_BOOL bEnable);
MS_BOOL HAL_MVOP_EX_GetEnableState(MVOP_DevID eID);
HALMVOPFREQUENCY HAL_MVOP_EX_GetMaxFreerunClk(MVOP_DevID eID);
HALMVOPFREQUENCY HAL_MVOP_EX_GetClk(MS_U32 u32PixClk);
void HAL_MVOP_EX_SetFrequency(MVOP_DevID eID, HALMVOPFREQUENCY enFrequency);
void HAL_MVOP_EX_SetOutputInterlace(MVOP_DevID eID, MS_BOOL bEnable);
void HAL_MVOP_EX_SetPattern(MVOP_DevID eID, MVOP_Pattern enMVOPPattern);
MS_BOOL HAL_MVOP_EX_SetTileFormat(MVOP_DevID eID, MVOP_TileFormat eTileFmt);
MS_BOOL HAL_MVOP_EX_SetRgbFormat(MVOP_DevID eID, MVOP_RgbFormat eRgbFmt);
void HAL_MVOP_EX_SetBlackBG(MVOP_DevID eID);
void HAL_MVOP_EX_SetCropWindow(MVOP_DevID eID, MVOP_InputCfg *pparam);
void HAL_MVOP_EX_SetInputMode(MVOP_DevID eID, VOPINPUTMODE mode, MVOP_InputCfg *pparam );
void HAL_MVOP_EX_EnableUVShift(MVOP_DevID eID, MS_BOOL bEnable);
void HAL_MVOP_EX_SetEnable60P(MVOP_DevID eID, MS_BOOL bEnable);
MVOP_Result HAL_MVOP_EX_EnableFreerunMode(MVOP_DevID eID, MS_BOOL bEnable);
void HAL_MVOP_EX_SetOutputTiming(MVOP_DevID eID, MVOP_Timing *ptiming );
void HAL_MVOP_EX_SetDCClk(MVOP_DevID eID, MS_U8 clkNum, MS_BOOL bEnable);
void HAL_MVOP_EX_SetSynClk(MVOP_DevID eID, MVOP_Timing *ptiming, HALMVOPFREQUENCY eFreq);
void HAL_MVOP_EX_SetMonoMode(MVOP_DevID eID, MS_BOOL bEnable);
void HAL_MVOP_EX_SetH264HardwireMode(MVOP_DevID eID);
void HAL_MVOP_EX_EnableMVDInterface(MVOP_DevID eID, MS_BOOL bEnable);
void HAL_MVOP_EX_SetRMHardwireMode(MVOP_DevID eID);
void HAL_MVOP_EX_SetJpegHardwireMode(MVOP_DevID eID);
MS_BOOL HAL_MVOP_EX_Enable3DLR(MVOP_DevID eID, MS_BOOL bEnable);
MS_BOOL HAL_MVOP_EX_Get3DLRMode(MVOP_DevID eID);
MS_BOOL HAL_MVOP_EX_GetTimingInfoFromRegisters(MVOP_DevID eID, MVOP_TimingInfo_FromRegisters *pMvopTimingInfo);
#endif // _HAL_MVOP_H_

