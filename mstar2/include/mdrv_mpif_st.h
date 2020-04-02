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
// @file   mdrv_mpif_st.h
// @brief  MPIF Driver Interface
// @author MStar Semiconductor Inc.
//
// Driver to initialize and access mailbox.
//     - Provide functions to initialize/de-initialize mailbox
//     - Provide mailbox functional access.
//////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef _MDRV_MPIF_ST_H
#define _MDRV_MPIF_ST_H

//=============================================================================
// Includs
//=============================================================================
#include <asm/types.h>
#include "MsTypes.h"
#include "mdrv_mpif.h"

//=============================================================================
// Type and Structure Declaration
//=============================================================================
typedef struct _MS_MPIF_2XCFG
{
	MS_U8 		   slaveid;	
	DRV_MPIF_2XCTL  *pst2xctl;
}MS_MPIF_2XCFG;

typedef struct _MS_MPIF_3XCFG
{
	MS_U8 		   slaveid;	
	DRV_MPIF_3XCTL  *pst3xctl;
}MS_MPIF_3XCFG;

typedef struct _MS_MPIF_4ACFG
{
	MS_U8 		   slaveid;	
	DRV_MPIF_4ACTL  *pst4Actl;
}MS_MPIF_4ACFG;

typedef struct _MS_MPIF_1ARW
{
	MS_U8 		   			slaveid;	
	MS_BOOL		   			bWrite;	
	MS_U8		   			u8index;
	MS_U8		   			*pu8data;
	DRV_MPIF_DrvStatus		Res;	
}MS_MPIF_1ARW;

typedef struct _MS_MPIF_2XRW
{
	MS_U8 		   				slaveid;	
	MS_BOOL		   				bWrite;	
	MS_U16		   				u16addr;
	MS_U16		   				*pu16data;
	DRV_MPIF_DrvStatus		    Res;	
}MS_MPIF_2XRW;

typedef struct _MS_MPIF_3XRIURW
{
	MS_U8 		   				slaveid;	
	MS_BOOL		   				bWrite;	
	MS_U8		   				u8datalen;
	MS_U8*		   				pu8data;
	DRV_MPIF_DrvStatus		    Res;	
}MS_MPIF_3XRIURW;

typedef struct _MS_MPIF_MIURW
{
	MS_U8 		   				slaveid;	
	MS_BOOL		   				bWrite;	
	MS_U32		   				u32datalen;
	MS_PHYADDR	   				u32miu_addr;
	MS_PHYADDR	   				u32spif_miuaddr;		
}MS_MPIF_MIURW;

typedef struct _MS_MPIF_BUS_PARAM
{
	MS_U8 		   u8slaveid;	
	MS_U8		   u8cmdwidth;	
	MS_U8		   u8datawidth;	
}MS_MPIF_BUS_PARAM;

typedef struct _MS_MPIF_1BYTE_PARAM
{
	MS_U8 		   u8slaveid;	
	MS_U8		   u8data;	
}MS_MPIF_1BYTE_PARAM;






#endif //_MDRV_MPIF_ST_H
