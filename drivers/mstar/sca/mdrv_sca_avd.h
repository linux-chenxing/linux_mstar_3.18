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
#ifndef MDRV_SCA_AVD_H
#define MDRV_SCA_AVD_H

//------------------------------------------------------------------------------
// Default Contrast & Saturation & Hue & Sharpness Value
//------------------------------------------------------------------------------
// Contrast

//------------------------------------------------------------------------------
//  Export Functions & Variable
//------------------------------------------------------------------------------

#ifdef MDRV_SCA_AVD_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

INTERFACE MS_U32    m_dwATVAVHandlerTimer;
INTERFACE MS_U16    m_wTVAVTimer;


INTERFACE void MApi_AVD_Sys_SetBuf(MS_U32 u32BufAddr, MS_U32 u32BufSize);
INTERFACE MS_BOOL MApi_AVD_Sys_Init(MS_BOOL bForceInit);
INTERFACE MS_BOOL MApi_AVD_Sys_ReInit(void);
INTERFACE void MApi_Clear_AVDFlag(void);

INTERFACE void MApi_InputSource_ChangeVideoSource( INPUT_SOURCE_TYPE_t enInputSourceType );
INTERFACE void MApi_ATVProc_Handler( void );
INTERFACE MS_U16 GetMailBoxStatus(void);

#undef INTERFACE
#endif
