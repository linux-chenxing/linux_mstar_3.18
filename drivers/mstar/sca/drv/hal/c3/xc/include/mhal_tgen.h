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
#ifndef _MHAL_TGEN_H_
#define _MHAL_TGEN_H_


void Hal_SC_TGen_SetTiming(SCALER_WIN eWindow, XC_TGEN_TIMING_TYPE enType, MS_BOOL b10Bits);

MS_BOOL Hal_SC_Get_TGen_SC_Tbl_HV_Start(SCALER_WIN eWindow, MS_U16 *u16Hstart, MS_U16 *u16Vstart );

MS_BOOL Hal_SC_Get_TGen_SC_Tbl_H_Bacporch(SCALER_WIN eWindow, MS_U16 *u16Hbackporch );

#endif
