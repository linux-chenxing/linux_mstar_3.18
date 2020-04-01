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

#ifndef _HAL_IRQ_H_
#define _HAL_IRQ_H_

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
void HAL_IRQ_Init(void);
void HAL_IRQ_Restore(void);
void HAL_IRQ_DetechAll(void);
void HAL_IRQ_Detech(MS_U32 u32Vector);
void HAL_IRQ_Attach(MS_U32 u32Vector, void *pIntCb);
void HAL_IRQ_Mask(MS_U32 u32Vector);
void HAL_IRQ_MaskAll(MS_BOOL bMask);
void HAL_IRQ_UnMask(MS_U32 u32Vector);
void HAL_IRQ_NotifyCpu(IRQ_CPU_TYPE type);
void HAL_IRQ_Set_IOMap(MS_U32 u32Base);
MS_BOOL HAL_IRQ_InISR(void);
#endif // _HAL_IRQ_H_

