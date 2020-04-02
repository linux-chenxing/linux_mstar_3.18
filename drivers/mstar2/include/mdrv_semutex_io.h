///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2006 - 2008 MStar Semiconductor, Inc.
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

// PUT THIS FILE under INLCUDE path

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   mdrv_temp_io.h
/// @brief  TEMP Driver Interface.
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_SEMUTEX_IO_H_
#define _MDRV_SEMUTEX_IO_H_

//-------------------------------------------------------------------------------------------------
//  Driver Capability
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

#define SEMUTEX_IOC_MAGIC               'S'
#define MUTEX_INDEX_BEGIN               0xFF
#define SEMAPHORE_INDEX_BEGIN           0x3FF

/* IOCTL functions. */
/* int ioctl(int fd,
             int request == MDRV_SEMUTEX_LOCK);
*/
#define MDRV_SEMUTEX_LOCK                (int)_IO(SEMUTEX_IOC_MAGIC, 0U)

/* int ioctl(int fd,
             int request == MDRV_SC_DETACH_INTERRUPT);
*/
#define MDRV_SEMUTEX_UNLOCK                (int)_IO(SEMUTEX_IOC_MAGIC, 1U)

/* int ioctl(int fd,
             int request == MDRV_SC_RESET_FIFO);
*/
#define MDRV_SEMUTEX_CREATE_SHAREMEMORY                      (int)_IO(SEMUTEX_IOC_MAGIC, 2U)

/* int ioctl(int fd,
             int request == MDRV_SC_GET_EVENTS,
             int *events);
*/
#define MDRV_SEMUTEX_MAP_SHAREMEMORY                      (int)_IOR(SEMUTEX_IOC_MAGIC, 3U, int)

#define MDRV_SEMUTEX_UNMAP_SHAREMEMORY                      (int)_IOR(SEMUTEX_IOC_MAGIC, 4U, int)

#define MDRV_SEMUTEX_CREATE_MUTEX                      _IOR(SEMUTEX_IOC_MAGIC, 5U, int)

#define MDRV_SEMUTEX_DEL_MUTEX                      _IOR(SEMUTEX_IOC_MAGIC, 6U, int)

#define MDRV_SEMUTEX_TRY_LOCK                     _IOR(SEMUTEX_IOC_MAGIC, 7U, int)

#define MDRV_SEMUTEX_QUERY_ISFIRST_SHAREMEMORY                      (int)_IO(SEMUTEX_IOC_MAGIC, 8U)

#define MDRV_SEMUTEX_LOCK_WITHTIMEOUT                      _IOR(SEMUTEX_IOC_MAGIC, 9U, int)

#define MDRV_SEMUTEX_EXPAND_SHAREMEMORY                      _IOR(SEMUTEX_IOC_MAGIC, 10U, int)

#define MDRV_SEMUTEX_GET_SHMSIZE		             _IOR(SEMUTEX_IOC_MAGIC, 11U, unsigned int)

#define MDRV_SEMUTEX_CREATE_SEMAPHORE               _IOR(SEMUTEX_IOC_MAGIC, 12U, int)

#define MDRV_SEMUTEX_SEMA_LOCK                      _IOR(SEMUTEX_IOC_MAGIC, 13U, int)

#define MDRV_SEMUTEX_SEMA_TRY_LOCK                  _IOR(SEMUTEX_IOC_MAGIC, 14U, int)

#define MDRV_SEMUTEX_SEMA_UNLOCK                    _IOR(SEMUTEX_IOC_MAGIC, 15U, int)

#define MDRV_SEMUTEX_SEMA_RESET                     _IOR(SEMUTEX_IOC_MAGIC, 16U, int)

#define SEMA_NAME_LEN   64

typedef struct{
int index;
int time;
}LOCKARG;

typedef struct{
    int     semanum;
    char    semaname[SEMA_NAME_LEN];
}CREATE_SEMA_ARG;

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

#endif // SEMUTEX

