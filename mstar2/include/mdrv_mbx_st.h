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


#ifndef _MDRV_MBX_ST_H
#define _MDRV_MBX_ST_H

//=============================================================================
// Includs
//=============================================================================

//=============================================================================
// Type and Structure Declaration
//=============================================================================
//IO Ctrl struct defines:
typedef struct   __attribute__((packed))
{
    MBX_CPU_ID eHKCPU;
    MBX_ROLE_ID eHostRole;
    MS_U32 u32TimeoutMillSecs;
    MBX_Result mbxResult;
}MS_MBX_INIT_INFO, *PMS_MBX_INIT_INFO;

typedef struct  __attribute__((packed))
{
    MS_BOOL  bInfo;
    MBX_Result mbxResult;
}MS_MBX_SET_BINFO, *PMS_MBX_SET_BINFO;

typedef struct  __attribute__((packed))
{
    MBX_Class eMsgClass;
    MS_U16 	u16MsgQueueSize;
    MBX_Result mbxResult;
}MS_MBX_REGISTER_MSG, *PMS_MBX_REGISTER_MSG;

typedef struct  __attribute__((packed))
{
    MBX_Class eMsgClass;
    MS_BOOL bForceDiscardMsgQueue;
    MBX_Result mbxResult;
}MS_MBX_UNREGISTER_MSG, *PMS_MBX_UNREGISTER_MSG;

typedef struct  __attribute__((packed))
{
    MBX_Class eMsgClass;
    MBX_Result mbxResult;
}MS_MBX_CLEAR_MSG, *PMS_MBX_CLEAR_MSG;

typedef struct __attribute__((packed))
{
    MBX_Msg *pMsg;
    MBX_Result mbxResult;
}MS_MBX_SEND_MSG, *PMS_MBX_SEND_MSG;

typedef struct  __attribute__((packed))
{
    MBX_Class eTargetClass;
    MBX_Msg *pMsg;
    MS_U32 u32WaitMillSecs;
    MS_U32 u32Flag;
    MBX_Result mbxResult;
}MS_MBX_RECV_MSG, *PMS_MBX_RECV_MSG;

typedef struct  __attribute__((packed))
{
    MBX_Class eTargetClass;
    MBX_MSGQ_Status *pMsgQueueStatus;
    MBX_Result mbxResult;
}MS_MBX_GET_MSGQSTATUS, *PMS_MBX_GET_MSGQSTATUS;

typedef struct  __attribute__((packed))
{
    MS_BOOL bEnabled;
    MS_S32  s32RefCnt;
}MS_MBX_GET_DRVSTATUS, *PMS_MBX_GET_DRVSTATUS;

typedef struct  __attribute__((packed))
{
    MBX_ROLE_ID eTargetRole;
    MS_U8 *pU8Info;
    MS_U8 u8Size;
    MBX_Result mbxResult;
}MS_MBX_CPROSYNC_INFORMATION, *PMS_MBX_CPROSYNC_INFORMATION;

typedef struct  __attribute__((packed))
{   
    MS_U8 u8Value;
}MS_PM_BRICK_TERMINATOR_INFO, *PMS_PM_BRICK_TERMINATOR_INFO;


#endif //_MDRV_MBX_ST_H
