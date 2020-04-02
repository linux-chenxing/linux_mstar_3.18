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

#ifndef _MDRV_MPIF_IO_H_
#define _MDRV_MPIF_IO_H_

//------------------------------------------------------------------------------
// Driver Name
//------------------------------------------------------------------------------
#define MPIF_MODULE_KERNAL_NAME       "/dev/mpif"


//------------------------------------------------------------------------------
// Definition
//------------------------------------------------------------------------------

/* Use 'f' as magic number */
#define MPIF_IOCTL_MAGIC            		'f'

#define IOCTL_MPIF_INIT                     _IOWR(MPIF_IOCTL_MAGIC,    0, int)
#define IOCTL_MPIF_EXIT                      _IOR(MPIF_IOCTL_MAGIC,    1, int)

#define IOCTL_MPIF_2X_CONFIG                _IOWR(MPIF_IOCTL_MAGIC,    2, int)
#define IOCTL_MPIF_3X_CONFIG                _IOWR(MPIF_IOCTL_MAGIC,    3, int)
#define IOCTL_MPIF_4A_CONFIG                _IOWR(MPIF_IOCTL_MAGIC,    4, int)

#define IOCTL_MPIF_1A                      	_IOWR(MPIF_IOCTL_MAGIC,    5, int)
#define IOCTL_MPIF_2A                       _IOWR(MPIF_IOCTL_MAGIC,    6, int)
#define IOCTL_MPIF_2B                       _IOWR(MPIF_IOCTL_MAGIC,    7, int)
#define IOCTL_MPIF_3A_RIU                   _IOWR(MPIF_IOCTL_MAGIC,    8, int)
#define IOCTL_MPIF_3A_MIU                   _IOWR(MPIF_IOCTL_MAGIC,    9, int)
#define IOCTL_MPIF_3B_RIU                   _IOWR(MPIF_IOCTL_MAGIC,   10, int)
#define IOCTL_MPIF_3B_MIU                   _IOWR(MPIF_IOCTL_MAGIC,   11, int)
#define IOCTL_MPIF_4A                       _IOWR(MPIF_IOCTL_MAGIC,   12, int)

#define IOCTL_MPIF_SET_CMDDATA_WIDTH        _IOWR(MPIF_IOCTL_MAGIC,   13, int)
#define IOCTL_MPIF_SET_LC2X_PATH			_IOWR(MPIF_IOCTL_MAGIC,   14, int)


#define IOCTL_MPIF_MAXNR            256

#endif
