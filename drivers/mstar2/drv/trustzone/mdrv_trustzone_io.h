///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2006 - 2007 MStar Semiconductor, Inc.
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
///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   mdrv_trustzone_io.h
/// @brief  Trustzone Driver IO Interface
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_TRUSTZONE_IO_H_
#define _MDRV_TRUSTZONE_IO_H_



/* Use 'T' as magic number */
#define TZ_IOC_MAGIC                'T'

#define TZ_IOC_INFO                             _IOWR(TZ_IOC_MAGIC, 0x00, DrvTZ_Info_t)
#define TZ_IOC_Get_Reg  			                  _IOWR(TZ_IOC_MAGIC, 0x01, DrvTZ_Cpu_Regs_t)
#define TZ_IOC_Register_Class	                  _IOWR(TZ_IOC_MAGIC, 0x02, unsigned int)
#define TZ_IOC_Call			                      	_IOWR(TZ_IOC_MAGIC, 0x03, DrvTZ_Args_t)

#endif

