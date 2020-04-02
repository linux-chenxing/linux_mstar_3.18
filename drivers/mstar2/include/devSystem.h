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

#ifndef __DEV_SYSTEM_H__
#define __DEV_SYSTEM_H__

/* Use 'S' as magic number */
#define SYS_IOC_MAGIC           'S'

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

/// System output pad switch type
typedef enum
{
    E_SYS_PAD_MSD5010_SM2_IIC2_DEV,                                     ///< 5010 SM2, IIC2
    E_SYS_PAD_MSD5011_SM2_IIC2_DEV,                                     ///< 5011 SM2, IIC2
    E_SYS_PAD_MSD5015_GPIO_DEV,                                         ///< 5015 GPIO
    E_SYS_PAD_MSD5018_SM2_DEV,                                          ///< 5018 SM2
} SYS_PadType_DEV;


//------------------------------------------------------------------------------
// Data structure
//------------------------------------------------------------------------------
// DEV_SYS_IOC_SWITCH_PAD
typedef struct
{
    SYS_PadType_DEV             PadType;
    MS_U32                         bSuccess;
} DevSys_Switch_Pad;


//-------------------------------------------------------------------------------------------------
//  ioctl method
//-------------------------------------------------------------------------------------------------
// MS_BOOL MDrv_System_Init(void);
#define DEV_SYS_IOC_INIT                _IOWR(SYS_IOC_MAGIC, 0x00, MS_U32)

// MS_BOOL MDrv_System_SwitchPad(SYS_PadType ePadType);
#define DEV_SYS_IOC_SWITCH_PAD          _IOWR(SYS_IOC_MAGIC, 0x01, DevSys_Switch_Pad)

// void MDrv_System_WDTEnable(MS_BOOL bEnable);
#define DEV_SYS_IOC_WDT_ENABLE          _IOW (SYS_IOC_MAGIC, 0x02, MS_U32)

// void MDrv_System_WDTClear(void);
#define DEV_SYS_IOC_WDT_CLEAR           _IO  (SYS_IOC_MAGIC, 0x03)

// MS_BOOL MDrv_System_WDTLastStatus(void);
#define DEV_SYS_IOC_WDT_LASTSTATUS      _IOWR(SYS_IOC_MAGIC, 0x04, MS_U32)

// void MDrv_System_WDTSetTime(MS_U32 u32Ms);
#define DEV_SYS_IOC_WDT_SETTIME         _IOW (SYS_IOC_MAGIC, 0x05, MS_U32)

// void MDrv_System_ResetChip(void);
#define DEV_SYS_IOC_RESET_CHIP          _IO  (SYS_IOC_MAGIC, 0x06)

// void MDrv_System_ResetCPU(void);
#define DEV_SYS_IOC_RESET_CPU           _IO  (SYS_IOC_MAGIC, 0x07)

#endif // #ifndef __DEV_SYSTEM_H__
