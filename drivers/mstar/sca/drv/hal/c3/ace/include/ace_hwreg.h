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

// Scaler register serpead define
#define SCALER_REGISTER_SPREAD 1

//NONPM
#if SCALER_REGISTER_SPREAD
#define REG_SCALER_BASE             0x130000
#define BK_SCALER_BASE              0x130000
#else
#define REG_SCALER_BASE             0x102F00
#define BK_SCALER_BASE              0x102F00
#endif

