//<MStar Software>
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2010 - 2012 Mstar Semiconductor, Inc.
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
/// file    mdrv_ldm_parse.h
/// @brief  local dimming Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_LD_PARSE_H_
#define _MDRV_LD_PARSE_H_


//#include "MsCommon.h"
//#include "MsOS.h"


#ifdef _DRVLDPARSE_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif


INTERFACE MS_BOOL MDrv_LD_ParseIni(char *con_file ,char *con_fileUpdate);
INTERFACE MS_BOOL MDrv_LD_ParseBin(char *buff, int buff_len, char *con_file, char *con_fileUpdate);


#undef INTERFACE

#endif

