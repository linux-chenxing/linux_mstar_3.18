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

/// file    halMMIO.h

/// @brief  memory map io (MMIO) HAL

/// @author MStar Semiconductor Inc.

///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __HAL_MMIO_H__

#define __HAL_MMIO_H__





//--------------------------------------------------------------------------------------------------

//  Hardware Capability

//--------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------

//  Macro and Define

//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------

//  Type and Structure

//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------

//  Function and Variable

//-------------------------------------------------------------------------------------------------



MS_U16  HAL_MMIO_GetType(MS_U32 u32Module);

MS_BOOL HAL_MMIO_GetBase(MS_U32* pu32BaseAddr, MS_U32* pu32BaseSize, MS_U32 u32BankType);

MS_BOOL HAL_MMIO_GetIPBase(MS_U32 *pu32BaseAddr, MS_U16 u16BankType);



//we put the flush & read memory function here due to library dependency consideration

void    HAL_MMIO_FlushMemory(void);

void    HAL_MMIO_ReadMemory(void);



#endif // __HAL_MMIO_H__





