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

///

/// @file   _drvMVOP_weak.h

/// @brief  MVOP weak symbols declaration.

/// @author MStar Semiconductor Inc.

///////////////////////////////////////////////////////////////////////////////////////////////////



#ifndef _DRV_MVOP_WEAK_H_

#define _DRV_MVOP_WEAK_H_



#include "halMVOP.h"



//-------------------------------------------------------------------------------------------------

//  Macro and Define

//-------------------------------------------------------------------------------------------------

#ifdef MSOS_TYPE_CE

	#define __attribute__(x)

#endif



#define __mvop_weak      __attribute__((weak))

#define __mvop_if        extern



//-------------------------------------------------------------------------------------------------

//  Function and Variable

//-------------------------------------------------------------------------------------------------



//======================================================

// MAIN MVOP

__mvop_if MS_BOOL HAL_MVOP_Set3DLRInsVBlank(MS_U16 u16InsLines) __mvop_weak;

__mvop_if MS_BOOL HAL_MVOP_Set3DLRAltOutput(MS_BOOL bEnable) __mvop_weak;

__mvop_if void HAL_MVOP_SetYUVBaseAdd(MS_U32 u32YOffset, MS_U32 u32UVOffset, MS_BOOL bProgressive, MS_BOOL b422pack) __mvop_weak;



//======================================================

// SUB MVOP

__mvop_if void HAL_MVOP_SubRegSetBase(MS_U32 u32Base)__mvop_weak;

__mvop_if void HAL_MVOP_SubInit(void)__mvop_weak;

__mvop_if void HAL_MVOP_SubSetFieldInverse(MS_BOOL b2MVD, MS_BOOL b2IP)__mvop_weak;

__mvop_if void HAL_MVOP_SubSetChromaWeighting(MS_BOOL bEnable)__mvop_weak;

__mvop_if void HAL_MVOP_SubLoadReg(void)__mvop_weak;

__mvop_if void HAL_MVOP_SubSetMIUReqMask(MS_BOOL bEnable)__mvop_weak;

__mvop_if void HAL_MVOP_SubRst(void)__mvop_weak;

__mvop_if void HAL_MVOP_SubEnable(MS_BOOL bEnable)__mvop_weak;

__mvop_if MS_BOOL HAL_MVOP_SubGetEnableState(void)__mvop_weak;

__mvop_if HALMVOPFREQUENCY HAL_MVOP_SubGetMaxFreerunClk(void)__mvop_weak;

__mvop_if void HAL_MVOP_SubSetFrequency(HALMVOPFREQUENCY enFrequency)__mvop_weak;

__mvop_if void HAL_MVOP_SubSetOutputInterlace(MS_BOOL bEnable)__mvop_weak;

__mvop_if void HAL_MVOP_SubSetPattern(MVOP_Pattern enMVOPPattern)__mvop_weak;

__mvop_if MS_BOOL HAL_MVOP_SubSetTileFormat(MVOP_TileFormat eTileFmt)__mvop_weak;

__mvop_if MS_BOOL HAL_MVOP_SubEnable3DLR(MS_BOOL bEnable)__mvop_weak;

__mvop_if MS_BOOL HAL_MVOP_SubGet3DLRMode(void)__mvop_weak;

__mvop_if MS_BOOL HAL_MVOP_SubSet3DLRInsVBlank(MS_U16 u16InsLines) __mvop_weak;

__mvop_if void HAL_MVOP_SubSetBlackBG(void)__mvop_weak;

__mvop_if void HAL_MVOP_SubSetCropWindow(MVOP_InputCfg *pparam)__mvop_weak;

__mvop_if void HAL_MVOP_SubSetInputMode( VOPINPUTMODE mode, MVOP_InputCfg *pparam )__mvop_weak;

__mvop_if void HAL_MVOP_SubEnableUVShift(MS_BOOL bEnable)__mvop_weak;

__mvop_if void HAL_MVOP_SubSetEnable60P(MS_BOOL bEnable)__mvop_weak;

__mvop_if void HAL_MVOP_SubSetOutputTiming( MVOP_Timing *ptiming )__mvop_weak;

__mvop_if void HAL_MVOP_SubSetDCClk(MS_U8 clkNum, MS_BOOL bEnable)__mvop_weak;

__mvop_if void HAL_MVOP_SubSetSynClk(MVOP_Timing *ptiming)__mvop_weak;

__mvop_if void HAL_MVOP_SubSetMonoMode(MS_BOOL bEnable)__mvop_weak;

__mvop_if void HAL_MVOP_SubSetH264HardwireMode(void)__mvop_weak;

__mvop_if void HAL_MVOP_SubSetRMHardwireMode(void)__mvop_weak;

__mvop_if void HAL_MVOP_SubSetJpegHardwireMode(void)__mvop_weak;

__mvop_if void HAL_MVOP_SubEnableMVDInterface(MS_BOOL bEnable)__mvop_weak;

__mvop_if MS_BOOL HAL_MVOP_SubGetTimingInfoFromRegisters(MVOP_TimingInfo_FromRegisters *pMvopTimingInfo)__mvop_weak;

__mvop_if void HAL_MVOP_SubSetHorizontallMirrorMode(MS_BOOL bEnable)__mvop_weak;

__mvop_if void HAL_MVOP_SubSetVerticalMirrorMode(MS_BOOL bEnable)__mvop_weak;

__mvop_if void HAL_MVOP_SubEnableFreerunMode(MS_BOOL bEnable)__mvop_weak;

__mvop_if void HAL_MVOP_SubSetYUVBaseAdd(MS_U32 u32YOffset, MS_U32 u32UVOffset, MS_BOOL bProgressive, MS_BOOL b422pack)__mvop_weak;



//======================================================

// EX MVOP:: Extended HAL functions for MVOP modules

__mvop_if void HAL_MVOP_EX_RegSetBase(MS_U32 u32Base)__mvop_weak;

__mvop_if void HAL_MVOP_EX_InitMirrorMode(MVOP_DevID eID, MS_BOOL bMir)__mvop_weak;

__mvop_if MS_BOOL HAL_MVOP_EX_Init(MVOP_DevID eID)__mvop_weak;

__mvop_if void HAL_MVOP_EX_SetVerticalMirrorMode(MVOP_DevID eID, MS_BOOL bEnable)__mvop_weak;

__mvop_if void HAL_MVOP_EX_SetHorizontallMirrorMode(MVOP_DevID eID, MS_BOOL bEnable)__mvop_weak;

__mvop_if void HAL_MVOP_EX_SetFieldInverse(MVOP_DevID eID, MS_BOOL b2MVD, MS_BOOL b2IP)__mvop_weak;

__mvop_if void HAL_MVOP_EX_SetChromaWeighting(MVOP_DevID eID, MS_BOOL bEnable)__mvop_weak;

__mvop_if void HAL_MVOP_EX_LoadReg(MVOP_DevID eID)__mvop_weak;

__mvop_if void HAL_MVOP_EX_SetMIUReqMask(MVOP_DevID eID, MS_BOOL bEnable)__mvop_weak;

__mvop_if void HAL_MVOP_EX_Rst(MVOP_DevID eID)__mvop_weak;

__mvop_if void HAL_MVOP_EX_Enable(MVOP_DevID eID, MS_BOOL bEnable)__mvop_weak;

__mvop_if MS_BOOL HAL_MVOP_EX_GetEnableState(MVOP_DevID eID)__mvop_weak;

__mvop_if HALMVOPFREQUENCY HAL_MVOP_EX_GetMaxFreerunClk(MVOP_DevID eID)__mvop_weak;

__mvop_if void HAL_MVOP_EX_SetFrequency(MVOP_DevID eID, HALMVOPFREQUENCY enFrequency)__mvop_weak;

__mvop_if void HAL_MVOP_EX_SetOutputInterlace(MVOP_DevID eID, MS_BOOL bEnable)__mvop_weak;

__mvop_if void HAL_MVOP_EX_SetPattern(MVOP_DevID eID, MVOP_Pattern enMVOPPattern)__mvop_weak;

__mvop_if MS_BOOL HAL_MVOP_EX_SetTileFormat(MVOP_DevID eID, MVOP_TileFormat eTileFmt)__mvop_weak;

__mvop_if void HAL_MVOP_EX_SetBlackBG(MVOP_DevID eID)__mvop_weak;

__mvop_if void HAL_MVOP_EX_SetCropWindow(MVOP_DevID eID, MVOP_InputCfg *pparam)__mvop_weak;

__mvop_if void HAL_MVOP_EX_SetInputMode(MVOP_DevID eID, VOPINPUTMODE mode, MVOP_InputCfg *pparam )__mvop_weak;

__mvop_if void HAL_MVOP_EX_EnableUVShift(MVOP_DevID eID, MS_BOOL bEnable)__mvop_weak;

__mvop_if void HAL_MVOP_EX_SetEnable60P(MVOP_DevID eID, MS_BOOL bEnable)__mvop_weak;
#if defined(CHIP_A3)
__mvop_if void HAL_MVOP_EX_EnableFreerunMode(MVOP_DevID eID, MS_BOOL bEnable)__mvop_weak;
#endif
__mvop_if void HAL_MVOP_EX_SetOutputTiming(MVOP_DevID eID, MVOP_Timing *ptiming )__mvop_weak;

__mvop_if void HAL_MVOP_EX_SetDCClk(MVOP_DevID eID, MS_U8 clkNum, MS_BOOL bEnable)__mvop_weak;
#if defined(CHIP_A3)
__mvop_if void HAL_MVOP_EX_SetSynClk(MVOP_DevID eID, MVOP_Timing *ptiming)__mvop_weak;
#endif
__mvop_if void HAL_MVOP_EX_SetMonoMode(MVOP_DevID eID, MS_BOOL bEnable)__mvop_weak;

__mvop_if void HAL_MVOP_EX_SetH264HardwireMode(MVOP_DevID eID)__mvop_weak;

__mvop_if void HAL_MVOP_EX_EnableMVDInterface(MVOP_DevID eID, MS_BOOL bEnable)__mvop_weak;

__mvop_if void HAL_MVOP_EX_SetRMHardwireMode(MVOP_DevID eID)__mvop_weak;

__mvop_if void HAL_MVOP_EX_SetJpegHardwireMode(MVOP_DevID eID)__mvop_weak;

__mvop_if MS_BOOL HAL_MVOP_EX_Enable3DLR(MVOP_DevID eID, MS_BOOL bEnable)__mvop_weak;

__mvop_if MS_BOOL HAL_MVOP_EX_Get3DLRMode(MVOP_DevID eID)__mvop_weak;

__mvop_if MS_BOOL HAL_MVOP_EX_Set3DLRInsVBlank(MS_U16 u16InsLines) __mvop_weak;

__mvop_if MS_BOOL HAL_MVOP_EX_GetTimingInfoFromRegisters(MVOP_DevID eID, MVOP_TimingInfo_FromRegisters *pMvopTimingInfo)__mvop_weak;



#endif // _DRV_MVOP_WEAK_H_

