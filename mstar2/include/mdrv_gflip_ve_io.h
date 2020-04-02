///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2008 - 2009 MStar Semiconductor, Inc.
// This program is free software.
// You can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation;
// either version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program;
// if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//
// @file   mdrv_gflip_ve_io.h
// @brief  GFlip KMD Driver Interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_GFLIP_VE_IO_H
#define _MDRV_GFLIP_VE_IO_H

//=============================================================================
// Includs
//=============================================================================
#include "mdrv_gflip_ve_st.h"

//=============================================================================
// Defines
//=============================================================================
//IO Ctrl defines:
#define MDRV_GFLIP_VEC_IOC_CONFIG_NR        0
#define MDRV_GFLIP_VEC_IOC_ENABLEVECAPTURE_NR         MDRV_GFLIP_VEC_IOC_CONFIG_NR+1
#define MDRV_GFLIP_VEC_IOC_GETVECAPTURESTATE_NR       MDRV_GFLIP_VEC_IOC_ENABLEVECAPTURE_NR+1
#define MDRV_GFLIP_VEC_IOC_VECAPTUREWAITONFRAME_NR    MDRV_GFLIP_VEC_IOC_GETVECAPTURESTATE_NR+1
#define MDRV_GFLIP_VEC_IOC_MAX_NR                     MDRV_GFLIP_VEC_IOC_VECAPTUREWAITONFRAME_NR+1

// use 'v' as magic number for ve driver
#define MDRV_GFLIP_VEC_IOC_MAGIC         'v'
#define MDRV_GFLIP_VEC_IOC_CONFIG                     _IOR (MDRV_GFLIP_VEC_IOC_MAGIC,  MDRV_GFLIP_VEC_IOC_CONFIG_NR, MS_GFLIP_VEC_CONFIG)
#define MDRV_GFLIP_VEC_IOC_ENABLEVECAPTURE            _IOWR(MDRV_GFLIP_VEC_IOC_MAGIC,  MDRV_GFLIP_VEC_IOC_ENABLEVECAPTURE_NR, MS_GFLIP_VEC_STATE)
#define MDRV_GFLIP_VEC_IOC_GETVECAPTURESTATE          _IOWR(MDRV_GFLIP_VEC_IOC_MAGIC,  MDRV_GFLIP_VEC_IOC_GETVECAPTURESTATE_NR, MS_GFLIP_VEC_STATE)
#define MDRV_GFLIP_VEC_IOC_VECAPTUREWAITONFRAME       _IOWR(MDRV_GFLIP_VEC_IOC_MAGIC,  MDRV_GFLIP_VEC_IOC_VECAPTUREWAITONFRAME_NR, MS_GFLIP_VEC_STATE)
#endif //_MDRV_GFLIP_VE_IO_H
