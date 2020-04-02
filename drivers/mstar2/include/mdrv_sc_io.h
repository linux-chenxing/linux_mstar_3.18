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

#ifndef _MDRV_SC_IO_H_
#define _MDRV_SC_IO_H_

//-------------------------------------------------------------------------------------------------
//  Driver Capability
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

#define SC_IOC_MAGIC                's'

/* IOCTL functions. */
/* int ioctl(int fd,
             int request == MDRV_SC_ATTACH_INTERRUPT);
*/
#define MDRV_SC_ATTACH_INTERRUPT                (int)_IO(SC_IOC_MAGIC, 0U)

/* int ioctl(int fd,
             int request == MDRV_SC_DETACH_INTERRUPT);
*/
#define MDRV_SC_DETACH_INTERRUPT                (int)_IO(SC_IOC_MAGIC, 1U)

/* int ioctl(int fd,
             int request == MDRV_SC_RESET_FIFO);
*/
#define MDRV_SC_RESET_FIFO                      (int)_IO(SC_IOC_MAGIC, 2U)

/* int ioctl(int fd,
             int request == MDRV_SC_GET_EVENTS,
             int *events);
*/
#define MDRV_SC_GET_EVENTS                      _IOR(SC_IOC_MAGIC, 3U, int)

/* int ioctl(int fd,
             int request == MDRV_SC_SET_EVENTS,
             int *events);
*/
#define MDRV_SC_SET_EVENTS                      _IOW(SC_IOC_MAGIC, 4U, int)

/* int ioctl(int fd,
             int request == MDRV_SC_GET_ATTRIBUTE,
             int *events);
*/
#define MDRV_SC_GET_ATTRIBUTE                   _IOR(SC_IOC_MAGIC, 5U, int)


/* int ioctl(int fd,
             int request == MDRV_SC_SET_ATTRIBUTE,
             int *events);
*/
#define MDRV_SC_SET_ATTRIBUTE                   _IOW(SC_IOC_MAGIC, 6U, int)


/* int ioctl(int fd,
             int request == MDRV_SC_CHECK_RST_TO_ATR,
             int *events);
*/
#define MDRV_SC_CHECK_RST_TO_ATR                _IOW(SC_IOC_MAGIC, 7U, int)





#define SC_IOC_MAXNR                            7

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

#endif // _MDRV_TEMP_IO_H_

