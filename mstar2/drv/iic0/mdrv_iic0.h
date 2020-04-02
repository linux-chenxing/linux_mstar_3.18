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
/// @file   mdrv_iic.h
/// @brief  IIC Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DRV_IIC0_H_
#define _DRV_IIC0_H_

#include "mdrv_iic.h"

#define MDRV_NAME_IIC0                   "iic0"
#define MDRV_MAJOR_IIC0                  0x8a
#define MDRV_MINOR_IIC0                  0x00
////////////////////////////////////////////////////////////////////////////////
// Define & data type
////////////////////////////////////////////////////////////////////////////////
#define MSIF_HWI2C0_LIB_CODE                     {'H','I','2','C'}    //Lib code
#define MSIF_HWI2C0_LIBVER                       {'0','6'}            //LIB version
#define MSIF_HWI2C0_BUILDNUM                     {'0','6'}            //Build Number
#define MSIF_HWI2C0_CHANGELIST                   {'0','0','5','4','9','6','1','5'} //P4 ChangeList Number

////////////////////////////////////////////////////////////////////////////////
// Extern Function
////////////////////////////////////////////////////////////////////////////////
extern void MDrv_HW_IIC0_Init(void);
extern BOOL MDrv_HWI2C0_Init(HWI2C_UnitCfg *psCfg);
extern BOOL MDrv_HWI2C0_WriteBytes(U16 u16SlaveCfg, U32 uAddrCnt, U8 *pRegAddr, U32 uSize, U8 *pData);
extern BOOL MDrv_HWI2C0_ReadBytes(U16 u16SlaveCfg, U32 uAddrCnt, U8 *pRegAddr, U32 uSize, U8 *pData);
extern BOOL MDrv_HWI2C0_SetClk(HWI2C_CLKSEL eClk);


extern BOOL MDrv_HWI2C0_Start(U16 u16PortOffset);
extern BOOL MDrv_HWI2C0_Stop(U16 u16PortOffset);
extern BOOL MDrv_HWI2C0_Send_Byte(U16 u16PortOffset, U8 u8Data);
extern BOOL MDrv_HWI2C0_Recv_Byte(U16 u16PortOffset, U8 *pData);
extern BOOL MDrv_HWI2C0_NoAck(U16 u16PortOffset);
extern BOOL _MDrv_HWI2C0_GetPortRegOffset(U8 u8Port, U16 *pu16Offset);
extern BOOL MDrv_HWI2C0_Reset(U16 u16PortOffset, BOOL bReset);



#endif // _DRV_IIC0_H_

