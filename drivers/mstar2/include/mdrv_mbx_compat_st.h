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
// @file   mdrv_mbx_st.h
// @brief  Mialbox KMD Driver Interface
// @author MStar Semiconductor Inc.
//
// Driver to initialize and access mailbox.
//     - Provide functions to initialize/de-initialize mailbox
//     - Provide mailbox functional access.
//////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef _MDRV_MBX_COMPAT_ST_H
#define _MDRV_MBX_COMPAT_ST_H

//=============================================================================
// Includs
//=============================================================================

//=============================================================================
// Type and Structure Declaration
//=============================================================================
//IO Ctrl struct defines:

typedef struct  __attribute__((packed))
{
    /// Mail message Queue status,
    /// @ref MBX_STATUS_QUEUE_OVER_FLOW
    /// @ref MBX_STATUS_QUEUE_HAS_INSTANT_MSG
    /// @ref MBX_STATUS_QUEUE_HAS_NORMAL_MSG
    compat_long_t status;
    /// pended normal message count in class message queue
    compat_long_t u32NormalMsgCount;
    /// pended Instant message count in class message queue
    compat_long_t u32InstantMsgCount;
}COMPAT_MBX_MSGQ_Status;

//=============================================================================

typedef struct   __attribute__((packed))
{
    MBX_CPU_ID eHKCPU;
    MBX_ROLE_ID eHostRole;
    compat_long_t u32TimeoutMillSecs;
    MBX_Result mbxResult;
}COMPAT_MS_MBX_INIT_INFO, *PCOMPAT_MS_MBX_INIT_INFO;

typedef struct  __attribute__((packed))
{
    MS_BOOL  bInfo;
    MBX_Result mbxResult;
}COMPAT_MS_MBX_SET_BINFO, *PCOMPAT_MS_MBX_SET_BINFO;

typedef struct  __attribute__((packed))
{
    MBX_Class eMsgClass;
    compat_int_t 	u16MsgQueueSize;
    MBX_Result mbxResult;
}COMPAT_MS_MBX_REGISTER_MSG, *PCOMPAT_MS_MBX_REGISTER_MSG;

typedef struct  __attribute__((packed))
{
    MBX_Class eMsgClass;
    MS_BOOL bForceDiscardMsgQueue;
    MBX_Result mbxResult;
}COMPAT_MS_MBX_UNREGISTER_MSG, *PCOMPAT_MS_MBX_UNREGISTER_MSG;

typedef struct  __attribute__((packed))
{
    MBX_Class eMsgClass;
    MBX_Result mbxResult;
}COMPAT_MS_MBX_CLEAR_MSG, *PCOMPAT_MS_MBX_CLEAR_MSG;


typedef struct  __attribute__((packed))
{
    MBX_Class eTargetClass;
    compat_uptr_t pMsg;
    compat_long_t u32WaitMillSecs;
    compat_long_t u32Flag;
    MBX_Result mbxResult;
}COMPAT_MS_MBX_RECV_MSG, *PCOMPAT_MS_MBX_RECV_MSG;

typedef struct __attribute__((packed))
{
    compat_uptr_t pMsg;
    MBX_Result mbxResult;
}COMPAT_MS_MBX_SEND_MSG, *PCOMPAT_MS_MBX_SEND_MSG;

typedef struct  __attribute__((packed))
{
    MBX_Class eTargetClass;
    compat_uptr_t pMsgQueueStatus;
    MBX_Result mbxResult;
}COMPAT_MS_MBX_GET_MSGQSTATUS, *PCOMPAT_MS_MBX_GET_MSGQSTATUS;

typedef struct  __attribute__((packed))
{
    MS_BOOL bEnabled;
    compat_long_t  s32RefCnt;
}COMPAT_MS_MBX_GET_DRVSTATUS, *PCOMPAT_MS_MBX_GET_DRVSTATUS;

typedef struct  __attribute__((packed))
{
    MBX_ROLE_ID eTargetRole;
    compat_uptr_t pU8Info;
    MS_U8 u8Size;
    MBX_Result mbxResult;
}COMPAT_MS_MBX_CPROSYNC_INFORMATION, *PCOMPAT_MS_MBX_CPROSYNC_INFORMATION;

typedef struct  __attribute__((packed))
{   
    MS_U8 u8Value;
}COMPAT_MS_PM_BRICK_TERMINATOR_INFO, *PCOMPAT_MS_PM_BRICK_TERMINATOR_INFO;

#endif //_MDRV_MBX_COMPAT_ST_H
