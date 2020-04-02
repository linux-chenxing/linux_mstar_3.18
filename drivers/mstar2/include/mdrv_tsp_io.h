///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2008 - 2010 MStar Semiconductor, Inc.
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
// @file   mdrv_tsp_io.h
// @brief  GFlip KMD Driver Interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_TSP_IO_H
#define _MDRV_TSP_IO_H

//=============================================================================
// Includs
//=============================================================================
#include "mdrv_tsp_st.h"

//=============================================================================
// Defines
//=============================================================================
//IO Ctrl defines:
#define MDRV_TSP_IOC_INIT_NR            0
#define MDRV_TSP_IOC_GETEVENTINFO_NR    MDRV_TSP_IOC_INIT_NR+1
#define MDRV_TSP_IOC_CLEAREVENTINFO_NR  MDRV_TSP_IOC_GETEVENTINFO_NR+1

#define MDRV_TSP_IOC_MAX_NR             MDRV_TSP_IOC_CLEAREVENTINFO_NR+1

// use 'T' as magic number
#define MDRV_TSP_IOC_MAGIC              'T'
#define MDRV_TSP_IOC_INIT               _IOR(MDRV_TSP_IOC_MAGIC,  MDRV_TSP_IOC_INIT_NR, MS_U32)
#define MDRV_TSP_IOC_GETEVENTINFO       _IOWR(MDRV_TSP_IOC_MAGIC, MDRV_TSP_IOC_GETEVENTINFO_NR, MS_TSP_EVENT_INFO)
#define MDRV_TSP_IOC_CLEAREVENTINFO     _IOWR(MDRV_TSP_IOC_MAGIC, MDRV_TSP_IOC_CLEAREVENTINFO_NR, MS_U32)

#endif //_MDRV_TSP_IO_H
