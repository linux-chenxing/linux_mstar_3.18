///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2017 MStar Semiconductor, Inc.
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
// @file   mdrv_pnl_io.h
// @brief  PNL KMD Driver Interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_PNL_IOC_H
#define _MDRV_PNL_IOC_H

//=============================================================================
// Includs
//=============================================================================

//=============================================================================
// Defines
//=============================================================================


// use 'P' as magic number
#define IOCTL_PNL_MAGIC                   ('P')

typedef enum
{
    IOCTL_PNL_RESET_NR=0,
    IOCTL_PNL_SET_CONFIG_NR,
    //IOCTL_PNL_GET_CONFIG_NR,
    IOCTL_PNL_ENABLE_NR,
    // Test Function
    IOCTL_PNL_READ_REG_NR,
    IOCTL_PNL_WRITE_REG_NR,
    IOCTL_PNL_WRITE_REG_BIT_NR,
    IOCTL_PNL_MAX_NR
} IoctlPnlNR_e;

#define IOCTL_PNL_RESET             _IO(IOCTL_PNL_MAGIC,   IOCTL_PNL_RESET_NR)
#define IOCTL_PNL_SET_CONFIG        _IOW(IOCTL_PNL_MAGIC,  IOCTL_PNL_SET_CONFIG_NR, PnlConfig_t)
//#define IOCTL_PNL_GET_CONFIG        _IOR(IOCTL_PNL_MAGIC,  IOCTL_PNL_GET_CONFIG_NR, PnlConfig_t)
#define IOCTL_PNL_ENABLE            _IOW(IOCTL_PNL_MAGIC,  IOCTL_PNL_ENABLE_NR, bool)
// Test Function
#define IOCTL_PNL_READ_REG          _IOWR(IOCTL_PNL_MAGIC, IOCTL_PNL_READ_REG_NR, PnlReg_t)
#define IOCTL_PNL_WRITE_REG         _IOW(IOCTL_PNL_MAGIC,  IOCTL_PNL_WRITE_REG_NR, PnlReg_t)
#define IOCTL_PNL_WRITE_REG_BIT     _IOW(IOCTL_PNL_MAGIC,  IOCTL_PNL_WRITE_REG_BIT_NR, PnlReg_t)

#endif //_MDRV_PNL_IOC_H



