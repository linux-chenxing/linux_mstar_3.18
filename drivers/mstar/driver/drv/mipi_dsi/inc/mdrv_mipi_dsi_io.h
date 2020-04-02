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
// @file   mdrv_mipi_dsi_io.h
// @brief  PNL KMD Driver Interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_MIPI_DSI_IO_H
#define _MDRV_MIPI_DSI_IO_H

//=============================================================================
// Includs
//=============================================================================

//=============================================================================
// Defines
//=============================================================================


// use 'M' as magic number
#define IOCTL_MIPI_DSI_MAGIC                   ('M')

#define FLAG_DELAY            0xFE
#define FLAG_END_OF_TABLE     0xFF   // END OF REGISTERS MARKER

typedef enum
{
    IOCTL_MIPI_DSI_RESET_NR=0,
    IOCTL_MIPI_DSI_SET_CONFIG_NR,
    //IOCTL_MIPI_DSI_GET_CONFIG_NR,
    IOCTL_MIPI_DSI_ENABLE_NR,
    // Test Function
    IOCTL_MIPI_DSI_READ_REG_NR,
    IOCTL_MIPI_DSI_WRITE_REG_NR,
    IOCTL_MIPI_DSI_WRITE_REG_BIT_NR,
    IOCTL_MIPI_DSI_MAX_NR
} IoctlMIPIDSINR_e;

#define IOCTL_MIPI_DSI_RESET             _IO(IOCTL_MIPI_DSI_MAGIC,   IOCTL_MIPI_DSI_RESET_NR)
#define IOCTL_MIPI_DSI_SET_CONFIG        _IOW(IOCTL_MIPI_DSI_MAGIC,  IOCTL_MIPI_DSI_SET_CONFIG_NR, MipiDsiConfig_t)
//#define IOCTL_MIPI_DSI_GET_CONFIG        _IOR(IOCTL_MIPI_DSI_MAGIC,  IOCTL_MIPI_DSI_GET_CONFIG_NR, MipiDsiConfig_t)
#define IOCTL_MIPI_DSI_ENABLE            _IOW(IOCTL_MIPI_DSI_MAGIC,  IOCTL_MIPI_DSI_ENABLE_NR, unsigned char)
// Test Function
#define IOCTL_MIPI_DSI_READ_REG          _IOWR(IOCTL_MIPI_DSI_MAGIC, IOCTL_MIPI_DSI_READ_REG_NR, MipiDsiReg_t)
#define IOCTL_MIPI_DSI_WRITE_REG         _IOW(IOCTL_MIPI_DSI_MAGIC,  IOCTL_MIPI_DSI_WRITE_REG_NR, MipiDsiReg_t)
#define IOCTL_MIPI_DSI_WRITE_REG_BIT     _IOW(IOCTL_MIPI_DSI_MAGIC,  IOCTL_MIPI_DSI_WRITE_REG_BIT_NR, MipiDsiReg_t)

#endif //_MDRV_MIPI_DSI_IO_H



