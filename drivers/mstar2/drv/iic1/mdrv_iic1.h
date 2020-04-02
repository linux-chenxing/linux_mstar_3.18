////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   mdrv_iic1.h
/// @brief  IIC Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DRV_IIC1_H_
#define _DRV_IIC1_H_

#include "mdrv_iic.h"

#define MDRV_NAME_IIC1                   "iic1"
#define MDRV_MAJOR_IIC1                  0x8b
#define MDRV_MINOR_IIC1                  0x01
////////////////////////////////////////////////////////////////////////////////
// Define & data type
////////////////////////////////////////////////////////////////////////////////
#define MSIF_HWI2C1_LIB_CODE                     {'H','I','2','C'}    //Lib code
#define MSIF_HWI2C1_LIBVER                       {'0','6'}            //LIB version
#define MSIF_HWI2C1_BUILDNUM                     {'0','6'}            //Build Number
#define MSIF_HWI2C1_CHANGELIST                   {'0','0','5','4','9','6','1','5'} //P4 ChangeList Number


////////////////////////////////////////////////////////////////////////////////
// Extern Function
////////////////////////////////////////////////////////////////////////////////
extern void MDrv_HW_IIC1_Init(void);
extern BOOL MDrv_HWI2C1_Init(HWI2C_UnitCfg *psCfg);
extern BOOL MDrv_HWI2C1_WriteBytes(U16 u16SlaveCfg, U32 uAddrCnt, U8 *pRegAddr, U32 uSize, U8 *pData);
extern BOOL MDrv_HWI2C1_ReadBytes(U16 u16SlaveCfg, U32 uAddrCnt, U8 *pRegAddr, U32 uSize, U8 *pData);
extern BOOL MDrv_HWI2C1_SetClk(HWI2C_CLKSEL eClk);


extern BOOL MDrv_HWI2C1_Start(U16 u16PortOffset);
extern BOOL MDrv_HWI2C1_Stop(U16 u16PortOffset);
extern BOOL MDrv_HWI2C1_Send_Byte(U16 u16PortOffset, U8 u8Data);
extern BOOL MDrv_HWI2C1_Recv_Byte(U16 u16PortOffset, U8 *pData);
extern BOOL MDrv_HWI2C1_NoAck(U16 u16PortOffset);
extern BOOL _MDrv_HWI2C1_GetPortRegOffset(U8 u8Port, U16 *pu16Offset);
extern BOOL MDrv_HWI2C1_Reset(U16 u16PortOffset, BOOL bReset);



#endif // _DRV_IIC1_H_

