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
// @file   mdrv_gflip_ve_st.h
// @brief  GFlip KMD Driver Interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef _MDRV_GFLIP_VE_ST_H
#define _MDRV_GFLIP_VE_ST_H

//=============================================================================
// Macros
//=============================================================================
#define VE_VEC_CONFIG_LENGTH_1STVERSION     16 //DO NOT CHANGE THIS, it is the structure length of the first version
#define VE_VEC_CONFIG_VERSION               0 //Version number for compatibility with kernel and VE driver

//=============================================================================
// Type and Structure Declaration
//=============================================================================
typedef enum
{
    MS_VEC_ISR_GOP_OP,
    MS_VEC_ISR_VE,
    MS_VEC_ISR_MAXNUM,
} MS_VEC_ISR_TYPE;

typedef enum
{
    MS_VEC_CONFIG_INIT,
    MS_VEC_CONFIG_ENABLE,
} MS_VEC_CONFIG_TYPE;

//IO Ctrl struct defines:
typedef struct
{
    MS_BOOL bEnable;      //< InOut, VE capture enable state
    MS_U8   u8FrameCount; //< Out, Current captured frame number,value range: 0~3
    MS_U8   u8Result;     //< Out, Function return status
}MS_GFLIP_VEC_STATE, *PMS_GFLIP_VEC_STATE;

typedef struct
{
    MS_U16  u16Version;   //< Version number for this structure
    MS_U16  u16Length;    //< Length of this structure, unit: byte
    MS_U8   u8Result;     //< Out, Function return status
    MS_BOOL bInterlace;   //< If the VEC capture source is interlace
    MS_U8   u8MaxFrameNumber_P;     //< The max frame number for progressive capture. This setting should obey with VE driver
    MS_U8   u8MaxFrameNumber_I;     //< The max frame number for Interlace capture. This setting should obey with VE driver
    MS_VEC_ISR_TYPE     eIsrType;   //< The ISR type of VEC capture
    MS_VEC_CONFIG_TYPE eConfigType; //< The ISR type of VEC capture
}MS_GFLIP_VEC_CONFIG, *PMS_GFLIP_VEC_CONFIG;

#endif //_MDRV_GFLIP_VE_ST_H
