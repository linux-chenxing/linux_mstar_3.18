//<MStar Software>
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2010 - 2012 MStar Semiconductor, Inc.
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
//==============================================================================

#ifndef MDRV_XC_MENULOAD_H
#define MDRV_XC_MENULOAD_H

#ifdef SUPPORT_KERNEL_MLOAD
#include "mhal_menuload.h"
#endif

#ifdef MDRV_XC_MENULOAD
#define INTERFACE
#else
#define INTERFACE extern
#endif

#define MLOAD_MUTEX

typedef struct __attribute__((packed))
{
    MS_U16 u16WPoint;
    MS_U16 u16RPoint;
    MS_U16 u16FPoint;
    MS_PHY PhyAddr;
    MS_U16 u16MaxCmdCnt;
    MS_BOOL bEnable;

    MS_VIRT va_phy;
    phys_addr_t len;
}KDRV_MS_MLoad_Info;

INTERFACE void KApi_XC_MLoad_Init(EN_MLOAD_CLIENT_TYPE _client_type,MS_U64 PhyAddr, MS_U32 u32BufByteLen);
INTERFACE void KApi_XC_MLoad_Enable(EN_MLOAD_CLIENT_TYPE _client_type,MS_BOOL bEnable);
INTERFACE EN_KDRV_MLOAD_TYPE KApi_XC_MLoad_GetStatus(EN_MLOAD_CLIENT_TYPE _client_type);
INTERFACE MS_BOOL KApi_XC_MLoad_WriteCmd(EN_MLOAD_CLIENT_TYPE _client_type,MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask);
INTERFACE MS_BOOL KApi_XC_MLoad_WriteCmd_NonXC(EN_MLOAD_CLIENT_TYPE _client_type,MS_U32 u32Bank,MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask);
INTERFACE MS_BOOL KApi_XC_MLoad_Fire(EN_MLOAD_CLIENT_TYPE _client_type,MS_BOOL bImmeidate);
INTERFACE MS_BOOL KDrv_XC_MLoad_set_IP_trig_p(EN_MLOAD_CLIENT_TYPE _client_type,MS_U16 u16train, MS_U16 u16disp);
INTERFACE MS_BOOL KDrv_XC_MLoad_get_IP_trig_p(EN_MLOAD_CLIENT_TYPE _client_type,MS_U16 *pu16Train, MS_U16 *pu16Disp);
INTERFACE void KDrv_XC_MLoad_set_trigger_sync(EN_MLOAD_CLIENT_TYPE _client_type,EN_MLOAD_TRIG_SYNC eTriggerSync);

#undef INTERFACE
#endif

