///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2008 - 2009 Mstar Semiconductor, Inc.
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
// @file   mdrv_ldm_io.h
// @brief  GFlip KMD Driver Interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_LDM_IO_H
#define _MDRV_LDM_IO_H

//=============================================================================
// Includs
//=============================================================================


//=============================================================================
// Defines
//=============================================================================
//IO Ctrl defines:
#define MDRV_LDM_IOC_INIT_NR        (0)
#define MDRV_LDM_IOC_DEINIT_NR      (MDRV_LDM_IOC_INIT_NR+1)

//#define MDRV_LDM_IOC_MAX_NR              (MDRV_LDM_IOC_DEINIT_NR+1)



// use 'L' as magic number
#define MDRV_LDM_IOC_MAGIC      ('L')
#define MDRV_LDM_IOC_INIT       _IO(MDRV_LDM_IOC_MAGIC, 0)
#define MDRV_LDM_IOC_DEINIT     _IO(MDRV_LDM_IOC_MAGIC, 1)
#define MDRV_LDM_IOC_ENABLE                 _IOW(MDRV_LDM_IOC_MAGIC, 2, MS_U32)
#define MDRV_LDM_IOC_DISABLE                _IOW(MDRV_LDM_IOC_MAGIC, 3, MS_U32)
#define MDRV_LDM_IOC_SUSPEND    _IOW(MDRV_LDM_IOC_MAGIC, 4, MS_U32)
#define MDRV_LDM_IOC_RESUME                 _IOW(MDRV_LDM_IOC_MAGIC, 5, MS_U32)
#define MDRV_LDM_IOC_SET_STRENGTH           _IOW(MDRV_LDM_IOC_MAGIC, 6, MS_U32)
#define MDRV_LDM_IOC_GET_DATA               _IOR(MDRV_LDM_IOC_MAGIC, 7, MS_U32)
#define MDRV_LDM_IOC_DEBUG                  _IOW(MDRV_LDM_IOC_MAGIC, 8, MS_U32)
#define MDRV_LDM_IOC_CUS_PATH               _IOW(MDRV_LDM_IOC_MAGIC, 9, MS_U32)
#define MDRV_LDM_IOC_DEMO_PATTERN           _IOW(MDRV_LDM_IOC_MAGIC, 10, MS_U32)
#define MDRV_LDM_IOC_DUMP_DRAM              _IOW(MDRV_LDM_IOC_MAGIC, 11, MS_U32)
#define MDRV_LDM_IOC_SET_LOCALSTRENGTH      _IOW(MDRV_LDM_IOC_MAGIC, 12, MS_U32)
#define MDRV_LDM_IOC_MAX_NR                 (13)


typedef enum
{
    E_LD_DATA_TYPE_LDF=0x01,
    E_LD_DATA_TYPE_LDB=0x02,
    E_LD_DATA_TYPE_SPI=0x03,
    E_LD_DATA_TYPE_MAX
}EN_LD_GET_DATA_TYPE;

typedef enum
{
    E_LD_DEBUG_LEVEL_ERROR=0x01,
    E_LD_DEBUG_LEVEL_WARNING=0x02,
    E_LD_DEBUG_LEVEL_INFO=0x04,
    E_LD_DEBUG_LEVEL_ALL=0x07,
    E_LD_DEBUG_LEVEL_MAX
}EN_LD_DEBUG_LEVEL;

typedef enum
{
    E_LD_DEMO_PATTERN_SWITCH_SINGLE_LED=0x00,
    E_LD_DEMO_PATTERN_LEFT_RIGHT_HALF=0x01,
    E_LD_DEMO_PATTERN_MARQUEE=0x02,
    E_LD_DEMO_PATTERN_LEFT_RIGHT_COLOR_SHELTER=0x03,
    E_LD_DEMO_PATTERN_MAX
}EN_LD_DEMO_PATTERN;



typedef struct __attribute__((packed))
{
    MS_U8 aCusPath[64];
    MS_U8 aCusPathU[64];
}ST_DRV_LD_CUS_PATH;

typedef struct __attribute__((packed))
{
    EN_LD_GET_DATA_TYPE enDataType;
    MS_PHY phyAddr;
}ST_DRV_LD_GET_DATA;

typedef struct __attribute__((packed))
{
    MS_BOOL bOn;
    EN_LD_DEMO_PATTERN enDemoPattern;
    MS_U16 u16LEDNum;
}ST_DRV_LD_DEMO_PATTERN;

#endif //_MDRV_LDM_IO_H
