//<MStar Software>
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

#ifndef _DRVCLKM_IO_H_
#define _DRVCLKM_IO_H_



typedef struct _CLKM_GetHandle_PARAM
{
    char *s8_Handle_Name;
    S32 s32_Handle;
	
}CLKM_GetHandle_PARAM,*PCLKM_GetHandle_PARAM;


typedef struct _CLKM_SET_CLK_SRC_PARAM
{
    S32 s32_Handle;
    char *clk_src_name;
	
}CLKM_SET_CLK_SRC_PARAM,*PCLKM_SET_CLK_SRC_PARAM;


typedef struct _CLKM_CLK_GATE_DISABLE_PARAM
{

    S32 s32_Handle;
	
}CLKM_CLK_GATE_DISABLE_PARAM,*PCLKM_CLK_GATE_DISABLE_PARAM;



#define CLKM_MAGIC                ('6')
#define CMD_CLKM_GET_HANDLE                             _IOWR(CLKM_MAGIC, 0x00, CLKM_GetHandle_PARAM)
#define CMD_CLKM_SET_CLK_SOURCE  			    _IOWR(CLKM_MAGIC, 0x01, int)
#define CMD_CLKM_CLK_GATE_DISABLE  	           _IOW(CLKM_MAGIC, 0x02, int)



#endif 
