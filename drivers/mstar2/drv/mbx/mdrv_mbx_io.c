///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2006 - 2007 Mstar Semiconductor, Inc.
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
/// file    mdrv_mbx_io.c
/// @brief  MS MailBox ioctrol driver
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

//=============================================================================
// Include Files
//=============================================================================
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <asm/string.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/version.h>
#if defined(CONFIG_COMPAT)
#include <linux/compat.h>
#endif

//drver header files
#include "mdrv_mstypes.h"
#include "mdrv_mbx.h"
#include "mdrv_mbx_io.h"
#include "mst_devid.h"
#if defined(CONFIG_COMPAT)
#include "mdrv_mbx_compat_st.h"
#include "mdrv_mbx_compat.h"
#include "mdrv_mbx_compat_io.h"
#endif

//=============================================================================
// Local Defines
//=============================================================================
#define     MDRV_MBX_DEVICE_COUNT           1
#define     MDRV_MBX_NAME                           "MSMAILBOX"

//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------
#define     MBXIO_DEBUG_ENABLE  //
#ifdef      MBXIO_DEBUG_ENABLE
#define     MBXIO_KDBG(_fmt, _args...)        printk(KERN_WARNING _fmt, ## _args)
#define     MBXIO_ASSERT(_con)   do {\
                                                            if (!(_con)) {\
                                                                printk(KERN_CRIT "BUG at %s:%d assert(%s)\n",\
                                                                                                    __FILE__, __LINE__, #_con);\
                                                                BUG();\
                                                            }\
                                                          } while (0)
#else
#define     MBXIO_KDBG(fmt, args...)
#define     MBXIO_ASSERT(arg)
#endif

static struct mutex _mutexMBXIOCTL;
#define DRV_MBX_LockIOCTL_Init()     mutex_init(&_mutexMBXIOCTL)
#define DRV_MBX_LockIOCTL()   mutex_lock(&_mutexMBXIOCTL)
#define DRV_MBX_UnLockIOCTL()   mutex_unlock(&_mutexMBXIOCTL)


//--------------------------------------------------------------------------------------------------
// IOCtrl functions declaration
//--------------------------------------------------------------------------------------------------
static int _MDrv_MBXIO_Open (struct inode *inode, struct file *filp);
static int _MDrv_MBXIO_Release(struct inode *inode, struct file *filp);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
static long _MDrv_MBXIO_IOCtl(struct file *filp, unsigned int cmd, unsigned long arg);
#else
static long _MDrv_MBXIO_IOCtl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
#endif
static int _MDrv_MBXIO_FASYNC(int fd, struct file *filp, int mode);
MSYSTEM_STATIC int _MDrv_MBXIO_ModuleInit(void);
MSYSTEM_STATIC void _MDrv_MBXIO_ModuleExit(void);

ssize_t _MDrv_MBXIO_Read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos);
ssize_t _MDrv_MBXIO_Write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos);
#if defined(CONFIG_COMPAT)
static long _Compat_MDrv_MBXIO_IOCtl(struct file *filp, unsigned int cmd, unsigned long arg);
#endif

//=============================================================================
// Local Variables: Device handler
//=============================================================================
typedef struct
{
    int s32Major;
    int s32Minor;
    int refCnt;
    struct cdev cdev;
    struct file_operations fops;
}MBX_DEV;

static MBX_DEV _devMBX =
{
    .s32Major = MDRV_MAJOR_MSMAILBOX,
    .s32Minor = MDRV_MINOR_MSMAILBOX,
    .refCnt = 0,
    .cdev =
    {
        .kobj = {.name= MDRV_NAME_MSMAILBOX, },
        .owner = THIS_MODULE,
    },
    .fops =
    {
        .open = _MDrv_MBXIO_Open,
        .read = _MDrv_MBXIO_Read,
        .write = _MDrv_MBXIO_Write,
        .release = _MDrv_MBXIO_Release,
        #if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
        .unlocked_ioctl = _MDrv_MBXIO_IOCtl,
	    #if defined(CONFIG_COMPAT)
        .compat_ioctl = _Compat_MDrv_MBXIO_IOCtl,
        #endif
        #else
        .ioctl = _MDrv_MBXIO_IOCtl,
        #endif
        .fasync = _MDrv_MBXIO_FASYNC,
    }
};

//=============================================================================
// Local Function Prototypes
//=============================================================================
static int _MDrv_MBXIO_IOC_Init(struct file *filp, unsigned long arg);
static int _MDrv_MBXIO_IOC_DeInit(struct file *filp, unsigned long arg);
static int _MDrv_MBXIO_IOC_RegisterMsg(struct file *filp, unsigned long arg);
static int _MDrv_MBXIO_IOC_UnRegisterMsg(struct file *filp, unsigned long arg);
static int _MDrv_MBXIO_IOC_ClearMsg(struct file *filp, unsigned long arg);
static int _MDrv_MBXIO_IOC_SendMsg(struct file *filp, unsigned long arg);
static int _MDrv_MBXIO_IOC_SendMsgLoopback(struct file *filp, unsigned long arg);
static int _MDrv_MBXIO_IOC_RecvMsg(struct file *filp, unsigned long arg);
static int _MDrv_MBXIO_IOC_CheckMsg(struct file *filp, unsigned long arg);
static int _MDrv_MBXIO_IOC_RemoveLatestMsg(struct file *filp, unsigned long arg);
static int _MDrv_MBXIO_IOC_GetMsgQStatus(struct file *filp, unsigned long arg);
static int _MDrv_MBXIO_IOC_MbxEnable(struct file *filp, unsigned long arg);
static int _MDrv_MBXIO_IOC_SetInformation(struct file *filp, unsigned long arg);
static int _MDrv_MBXIO_IOC_GetInformation(struct file *filp, unsigned long arg);
static int _MDrv_MBXIO_IOC_GetDrvStatus(struct file *filp, unsigned long arg);

static int _MDrv_PMIO_IOC_Get_BrickTerminator_Info(struct file *filp, unsigned long arg);  
static int _MDrv_PMIO_IOC_Set_BrickTerminator_Info(struct file *filp, unsigned long arg);  
//-------------------------------------------------------------------------------------------------
// IOCtrl Driver functions
//-------------------------------------------------------------------------------------------------

void _MDrv_MBXIO_IOC_Lock(void)
{
    DRV_MBX_LockIOCTL();
}

void _MDrv_MBXIO_IOC_UnLock(void)
{
    DRV_MBX_UnLockIOCTL();
}


int _MDrv_MBXIO_IOC_Init(struct file *filp, unsigned long arg)
{
    MS_MBX_INIT_INFO stInitInfo;

    if(copy_from_user(&stInitInfo, (MS_MBX_INIT_INFO __user *)arg, sizeof(MS_MBX_INIT_INFO)))
        return EFAULT;

    stInitInfo.mbxResult = MDrv_MBX_Init((TYPE_MBX_C_U64)filp, stInitInfo.eHKCPU, stInitInfo.eHostRole, stInitInfo.u32TimeoutMillSecs);

    if(copy_to_user((( MS_MBX_INIT_INFO  __user *)arg), &stInitInfo, sizeof(MS_MBX_INIT_INFO)))
        return EFAULT;
        
    return 0;
}

int _MDrv_MBXIO_IOC_DeInit(struct file *filp, unsigned long arg)
{
    MS_MBX_SET_BINFO stSetBInfo;

    if(copy_from_user(&stSetBInfo, (MS_MBX_SET_BINFO  __user *)arg, sizeof(MS_MBX_SET_BINFO)))
        return EFAULT;

    stSetBInfo.mbxResult = MDrv_MBX_DeInit((TYPE_MBX_C_U64)filp, stSetBInfo.bInfo);

     if(copy_to_user((( MS_MBX_SET_BINFO  __user *)arg), &stSetBInfo, sizeof(MS_MBX_SET_BINFO)))
        return EFAULT;

    return 0;
}

int _MDrv_MBXIO_IOC_RegisterMsg(struct file *filp, unsigned long arg)
{
    MS_MBX_REGISTER_MSG stRegisterMsg;

    if(copy_from_user(&stRegisterMsg, (MS_MBX_REGISTER_MSG __user *)arg, sizeof(MS_MBX_REGISTER_MSG)))
        return EFAULT;

    stRegisterMsg.mbxResult = MDrv_MBX_RegisterMSG((TYPE_MBX_C_U64)filp, stRegisterMsg.eMsgClass, stRegisterMsg.u16MsgQueueSize);
    
    if(copy_to_user((( MS_MBX_REGISTER_MSG  __user *)arg), &stRegisterMsg, sizeof(MS_MBX_REGISTER_MSG)))
        return EFAULT;
    
    return 0;
}

int _MDrv_MBXIO_IOC_UnRegisterMsg(struct file *filp, unsigned long arg)
{
    MS_MBX_UNREGISTER_MSG stUnRegisterMsg;

    if(copy_from_user(&stUnRegisterMsg, (MS_MBX_UNREGISTER_MSG __user *)arg, sizeof(MS_MBX_UNREGISTER_MSG)))
        return EFAULT;

    stUnRegisterMsg.mbxResult = MDrv_MBX_UnRegisterMSG((TYPE_MBX_C_U64)filp, stUnRegisterMsg.eMsgClass, stUnRegisterMsg.bForceDiscardMsgQueue);

    if(copy_to_user((( MS_MBX_UNREGISTER_MSG  __user *)arg), &stUnRegisterMsg, sizeof(MS_MBX_UNREGISTER_MSG)))
        return EFAULT;
        
    return 0;
}

int _MDrv_MBXIO_IOC_ClearMsg(struct file *filp, unsigned long arg)
{
    MS_MBX_CLEAR_MSG stClearMsg;

    if(copy_from_user(&stClearMsg, (MS_MBX_CLEAR_MSG __user *)arg, sizeof(MS_MBX_CLEAR_MSG)))
        return EFAULT;

    stClearMsg.mbxResult = MDrv_MBX_ClearMSG((TYPE_MBX_C_U64)filp, stClearMsg.eMsgClass);

    if(copy_to_user((( MS_MBX_CLEAR_MSG  __user *)arg), &stClearMsg, sizeof(MS_MBX_CLEAR_MSG)))
        return EFAULT;
        
    return 0;
}

int _MDrv_MBXIO_IOC_SendMsg(struct file *filp, unsigned long arg)
{
    MBX_Msg mbxMsg;
    MS_MBX_SEND_MSG  stMBXSend;
    
    if(copy_from_user(&stMBXSend, (MS_MBX_SEND_MSG __user *)arg, sizeof(MS_MBX_SEND_MSG)))
        return EFAULT;

    if(copy_from_user(&mbxMsg, stMBXSend.pMsg ,sizeof(MBX_Msg)))
        return EFAULT;

    stMBXSend.mbxResult = MDrv_MBX_SendMsg((TYPE_MBX_C_U64)filp, &mbxMsg, 0);
    
    if(copy_to_user((( MS_MBX_SEND_MSG  __user *)arg), &stMBXSend, sizeof(MS_MBX_SEND_MSG)))
        return EFAULT;

    return 0;
}

int _MDrv_MBXIO_IOC_SendMsgLoopback(struct file *filp, unsigned long arg)
{
    MS_MBX_SEND_MSG stSendMsg;
    MBX_Msg mbxMsg;

    if(copy_from_user(&stSendMsg, (MS_MBX_SEND_MSG __user *)arg, sizeof(MS_MBX_SEND_MSG)))
        return EFAULT;

    if(copy_from_user(&mbxMsg, stSendMsg.pMsg , sizeof(MBX_Msg)))
        return EFAULT;

    stSendMsg.mbxResult = MDrv_MBX_SendMsgLoopback((TYPE_MBX_C_U64)filp, &mbxMsg, stSendMsg.mbxResult);

    if(copy_to_user((( MS_MBX_SEND_MSG  __user *)arg), &stSendMsg, sizeof(MS_MBX_SEND_MSG)))
        return EFAULT;
        
    return 0;
}

int _MDrv_MBXIO_IOC_RecvMsg(struct file *filp, unsigned long arg)
{
    MS_MBX_RECV_MSG stRecvMsg;
    MBX_Msg mbxMsg;

    if(copy_from_user(&stRecvMsg, (MS_MBX_RECV_MSG __user *)arg, sizeof(MS_MBX_RECV_MSG)))
        return EFAULT;

    stRecvMsg.mbxResult = MDrv_MBX_RecvMsg((TYPE_MBX_C_U64)filp, stRecvMsg.eTargetClass, &mbxMsg, stRecvMsg.u32WaitMillSecs, stRecvMsg.u32Flag, 0);
    
    if(copy_to_user((TYPE_MBX_C_U64 *)stRecvMsg.pMsg  , &mbxMsg, sizeof(MBX_Msg)))
        return EFAULT;

    if(copy_to_user((( MS_MBX_RECV_MSG  __user *)arg), &stRecvMsg, sizeof(MS_MBX_RECV_MSG)))
        return EFAULT;
    
    return 0;
}

int _MDrv_MBXIO_IOC_CheckMsg(struct file *filp, unsigned long arg)
{
    MS_MBX_RECV_MSG stRecvMsg;
    MBX_Msg mbxMsg;

    if(copy_from_user(&stRecvMsg, (MS_MBX_RECV_MSG __user *)arg, sizeof(MS_MBX_RECV_MSG)))
        return EFAULT;

    stRecvMsg.mbxResult = MDrv_MBX_CheckMsg((TYPE_MBX_C_U64)filp, stRecvMsg.eTargetClass, &mbxMsg, stRecvMsg.u32WaitMillSecs, stRecvMsg.u32Flag);
    
    if(copy_to_user(stRecvMsg.pMsg , &mbxMsg, sizeof(MBX_Msg)))
        return EFAULT;
        
    if(copy_to_user((( MS_MBX_RECV_MSG  __user *)arg), &stRecvMsg, sizeof(MS_MBX_RECV_MSG)))
        return EFAULT;

    return 0;
}

int _MDrv_MBXIO_IOC_RemoveLatestMsg(struct file *filp, unsigned long arg)
{
    MS_MBX_SET_BINFO stSetBInfo;

    stSetBInfo.mbxResult = MDrv_MBX_RemoveLatestMsg();

    if(copy_to_user((( MS_MBX_SET_BINFO  __user *)arg), &stSetBInfo, sizeof(MS_MBX_SET_BINFO)))
        return EFAULT;
    
    return 0;
}


int _MDrv_MBXIO_IOC_GetMsgQStatus(struct file *filp, unsigned long arg)
{
    MS_MBX_GET_MSGQSTATUS stGetMsgQStatus;
    MBX_MSGQ_Status mbxMsgQueueStatus;


    if(copy_from_user(&stGetMsgQStatus, (MS_MBX_GET_MSGQSTATUS  __user *)arg, sizeof(MS_MBX_GET_MSGQSTATUS)))
        return EFAULT;

    stGetMsgQStatus.mbxResult = MDrv_MBX_GetMsgQueueStatus((TYPE_MBX_C_U64)filp, stGetMsgQStatus.eTargetClass, &mbxMsgQueueStatus);

    if(copy_to_user(stGetMsgQStatus.pMsgQueueStatus , &mbxMsgQueueStatus, sizeof(MBX_MSGQ_Status)))
        return EFAULT;

   if(copy_to_user((( MS_MBX_GET_MSGQSTATUS  __user *)arg), &stGetMsgQStatus, sizeof(MS_MBX_GET_MSGQSTATUS)))
        return EFAULT;
        
    return 0;
}

int _MDrv_MBXIO_IOC_MbxEnable(struct file *filp, unsigned long arg)
{
    MS_MBX_SET_BINFO stSetBInfo;

    if(copy_from_user(&stSetBInfo, (MS_MBX_SET_BINFO  __user *)arg, sizeof(MS_MBX_SET_BINFO)))
        return EFAULT;

    stSetBInfo.mbxResult = MDrv_MBX_Enable((TYPE_MBX_C_U64)filp, stSetBInfo.bInfo);

    if(copy_to_user((( MS_MBX_SET_BINFO  __user *)arg), &stSetBInfo, sizeof(MS_MBX_SET_BINFO)))
        return EFAULT;

    return 0;
}

int _MDrv_MBXIO_IOC_SetInformation(struct file *filp, unsigned long arg)
{
    MS_MBX_CPROSYNC_INFORMATION stSetInfo;
    MS_U8 u8Info[MAX_MBX_INFORMATION_SIZE]={0};

    if(stSetInfo.u8Size > MAX_MBX_INFORMATION_SIZE)
        return EFAULT;

    if(copy_from_user(&stSetInfo, (MS_MBX_CPROSYNC_INFORMATION __user *)arg, sizeof(MS_MBX_CPROSYNC_INFORMATION)))
        return EFAULT;

    if(copy_from_user(u8Info, stSetInfo.pU8Info , stSetInfo.u8Size))
        return EFAULT;

    stSetInfo.mbxResult = MDrv_MBX_SetInformation((TYPE_MBX_C_U64)filp, stSetInfo.eTargetRole, u8Info, stSetInfo.u8Size);

    if(copy_to_user((( MS_MBX_CPROSYNC_INFORMATION  __user *)arg), &stSetInfo, sizeof(MS_MBX_CPROSYNC_INFORMATION)))
        return EFAULT;

    return 0;
}

int _MDrv_MBXIO_IOC_GetInformation(struct file *filp, unsigned long arg)
{
    MS_MBX_CPROSYNC_INFORMATION stGetInfo;
    MS_U8 u8Info[MAX_MBX_INFORMATION_SIZE];

    if(copy_from_user(&stGetInfo, (MS_MBX_CPROSYNC_INFORMATION __user *)arg, sizeof(MS_MBX_CPROSYNC_INFORMATION)))
        return EFAULT;

    stGetInfo.mbxResult = MDrv_MBX_GetInformation((TYPE_MBX_C_U64)filp, stGetInfo.eTargetRole, u8Info, stGetInfo.u8Size);

    if(copy_to_user(stGetInfo.pU8Info, u8Info, stGetInfo.u8Size))
        return EFAULT;

    if(copy_to_user((( MS_MBX_CPROSYNC_INFORMATION  __user *)arg), &stGetInfo, sizeof(MS_MBX_CPROSYNC_INFORMATION)))
        return EFAULT;
        
    return 0;
}

int _MDrv_MBXIO_IOC_GetDrvStatus(struct file *filp, unsigned long arg)
{
    MS_MBX_GET_DRVSTATUS stGetDrvStatus;

    stGetDrvStatus.bEnabled = MDrv_MBX_GetEnableStatus((TYPE_MBX_C_U64)filp);
    stGetDrvStatus.s32RefCnt = _devMBX.refCnt;

    if(copy_to_user((( MS_MBX_GET_DRVSTATUS __user *)arg), &stGetDrvStatus, sizeof(MS_MBX_GET_DRVSTATUS)))
        return EFAULT;

    return 0;
}

int _MDrv_PMIO_IOC_Get_BrickTerminator_Info(struct file *filp, unsigned long arg)
{
	MS_PM_BRICK_TERMINATOR_INFO	stReadByte;
	
    stReadByte.u8Value = MDrv_PM_Get_BrickTerminator_Info();

    if (copy_to_user((U8 *) arg, &stReadByte, sizeof(MS_PM_BRICK_TERMINATOR_INFO)))
        return EFAULT;
    
    return 0;
}

int _MDrv_PMIO_IOC_Set_BrickTerminator_Info(struct file *filp, unsigned long arg)
{
    MS_PM_BRICK_TERMINATOR_INFO	stWriteByte;
    
	if (copy_from_user(&stWriteByte, (MS_PM_BRICK_TERMINATOR_INFO __user *) arg, sizeof(MS_PM_BRICK_TERMINATOR_INFO)))
        return EFAULT;

    MDrv_PM_Set_BrickTerminator_Info(stWriteByte.u8Value);
    return 0;
}
//-------------------------------------------------------------------------------------------------
// IOCtrl Driver interface functions
//-------------------------------------------------------------------------------------------------
int _MDrv_MBXIO_Open(struct inode *inode, struct file *filp)
{
    MBXIO_KDBG("--------->MBX DRIVER OPEN\n");

    if(_devMBX.refCnt == 0)
    {
        _devMBX.refCnt = 1;
        return 0;
    }
    else
    {
        _devMBX.refCnt++;
    }

    return 0;
}

ssize_t _MDrv_MBXIO_Read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
    return 0;
}

ssize_t _MDrv_MBXIO_Write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
    return 0;
}

int _MDrv_MBXIO_Release(struct inode *inode, struct file *filp)
{
    MBXIO_KDBG("<---------MBX DRIVER CLOSE\n");

    MBXIO_ASSERT(_devMBX.refCnt>0);

    //de-register from signal queue:
    MDrv_MBX_ReleaseFASYNC((TYPE_MBX_C_U64)filp); //call driver interface to un-register signal queue.

    //force to release since application is exit!
    MDrv_MBX_DeInit((MS_U32)filp, TRUE);

    if(_devMBX.refCnt > 0)
    {
        _devMBX.refCnt = _devMBX.refCnt -1;
    }

    return 0;
}

int _MDrv_MBXIO_FASYNC(int fd, struct file *filp, int mode)
{
    MBXIO_KDBG("%s is invoked\n", __FUNCTION__);

    if(E_MBX_SUCCESS != MDrv_MBX_FASYNC(fd,(TYPE_MBX_C_U64) filp, mode)) //fasync_helper(fd, filp, mode, &IRDev.async_queue);//call driver interface to register signal queue.
        return -EBUSY;

    return 0;
}

#if defined(CONFIG_COMPAT)
static long _Compat_MDrv_MBXIO_IOCtl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	
	unsigned int cmd_cpt = 0;
	
	switch (cmd)
	{
		case COMPAT_MDRV_MBX_IOC_INIT:
		{
			COMPAT_MS_MBX_INIT_INFO __user *data32;
			MS_MBX_INIT_INFO __user *data;

			int ret = 0;

			data32 = compat_ptr(arg);
			data = compat_alloc_user_space(sizeof(*data));
			if (data == NULL)
				return -EFAULT;

			cmd_cpt = MDRV_MBX_IOC_INIT;

			mdrv_mbx_init_compat_get_ion_allaction_data(data32, data);
    		ret = filp->f_op->unlocked_ioctl(filp, cmd_cpt, (unsigned long) data);
			mdrv_mbx_init_compat_put_ion_allaction_data(data32, data);

			return ret;
		}
		break;

		case COMPAT_MDRV_MBX_IOC_REGISTERMSG:
		{
			COMPAT_MS_MBX_REGISTER_MSG __user *data32;
			MS_MBX_REGISTER_MSG __user *data;

			int ret;

			data32 = compat_ptr(arg);
			data = compat_alloc_user_space(sizeof(*data));
			if (data == NULL)
				return -EFAULT;

			cmd_cpt = MDRV_MBX_IOC_REGISTERMSG;

			mdrv_mbx_register_msg_compat_get_ion_allaction_data(data32, data);
    		ret = filp->f_op->unlocked_ioctl(filp, cmd_cpt, (unsigned long) data);
			mdrv_mbx_register_msg_compat_put_ion_allaction_data(data32, data);

			return ret;
		}
		break;

		case COMPAT_MDRV_MBX_IOC_SENDMSGLOOPBACK:
			cmd_cpt = (cmd_cpt == 0 ? MDRV_MBX_IOC_SENDMSGLOOPBACK : cmd_cpt);
		case COMPAT_MDRV_MBX_IOC_SENDMSG:
		{
			COMPAT_MS_MBX_SEND_MSG __user *data32;
			MS_MBX_SEND_MSG __user *data;

			int ret = 0;

			data32 = compat_ptr(arg);
			data = compat_alloc_user_space(sizeof(*data));
			if (data == NULL)
				return -EFAULT;

			cmd_cpt = (cmd_cpt == 0 ? MDRV_MBX_IOC_SENDMSG : cmd_cpt);

			mdrv_mbx_sendmsg_compat_get_ion_allaction_data(data32, data);
    		ret = filp->f_op->unlocked_ioctl(filp, cmd_cpt, (unsigned long) data);
			mdrv_mbx_sendmsg_compat_put_ion_allaction_data(data32, data);

			return ret;
		}
		break;

		case COMPAT_MDRV_MBX_IOC_CHECKMSG:
			cmd_cpt = (cmd_cpt == 0 ? MDRV_MBX_IOC_CHECKMSG : cmd_cpt);
		case COMPAT_MDRV_MBX_IOC_RECVMSG:
		{
			COMPAT_MS_MBX_RECV_MSG __user *data32;
			MS_MBX_RECV_MSG __user *data;
			int ret = 0;

			data32 = compat_ptr(arg);
			data = compat_alloc_user_space(sizeof(*data));
			if (data == NULL)
				return -EFAULT;

			cmd_cpt = (cmd_cpt == 0 ? MDRV_MBX_IOC_RECVMSG : cmd_cpt);

			mdrv_mbx_recvmsg_compat_get_ion_allaction_data(data32, data);
    		ret = filp->f_op->unlocked_ioctl(filp, cmd_cpt, (unsigned long) data);
			mdrv_mbx_recvmsg_compat_put_ion_allaction_data(data32, data);

			return ret;
		}	
		break;

		case COMPAT_MDRV_MBX_IOC_GETMSGQSTATUS:
		{
			COMPAT_MS_MBX_GET_MSGQSTATUS __user *data32;
			MS_MBX_GET_MSGQSTATUS __user *data;
			int ret = 0;

			data32 = compat_ptr(arg);
			data = compat_alloc_user_space(sizeof(*data));
			if (data == NULL)
				return -EFAULT;

			cmd_cpt = (cmd_cpt == 0 ? MDRV_MBX_IOC_GETMSGQSTATUS : cmd_cpt);

			mdrv_mbx_get_msgqstatus_compat_get_ion_allaction_data(data32, data);
    		ret = filp->f_op->unlocked_ioctl(filp, cmd_cpt, (unsigned long) data);
			mdrv_mbx_get_msgqstatus_compat_put_ion_allaction_data(data32, data);

			return ret;
		}
		break;

		case COMPAT_MDRV_MBX_IOC_GETINFORMATION:
			cmd_cpt = (cmd_cpt == 0 ? MDRV_MBX_IOC_GETINFORMATION : cmd_cpt);
		case COMPAT_MDRV_MBX_IOC_SETINFORMATION:
		{
			COMPAT_MS_MBX_CPROSYNC_INFORMATION __user *data32;
			MS_MBX_CPROSYNC_INFORMATION __user *data;
			int ret = 0;

			data32 = compat_ptr(arg);
			data = compat_alloc_user_space(sizeof(*data));
			if (data == NULL)
				return -EFAULT;

			cmd_cpt = (cmd_cpt == 0 ? MDRV_MBX_IOC_SETINFORMATION : cmd_cpt);

			mdrv_mbx_crosync_info_compat_get_ion_allaction_data(data32, data);
    		ret = filp->f_op->unlocked_ioctl(filp, cmd_cpt, (unsigned long) data);
			mdrv_mbx_crosync_info_compat_put_ion_allaction_data(data32, data);

			return ret;
		}
		break;

		case COMPAT_MDRV_MBX_IOC_GETDRVSTATUS:
		{
			COMPAT_MS_MBX_GET_DRVSTATUS __user *data32;
			MS_MBX_GET_DRVSTATUS __user *data;
			int ret = 0;

			data32 = compat_ptr(arg);
			data = compat_alloc_user_space(sizeof(*data));
			if (data == NULL)
				return -EFAULT;

			cmd_cpt = (cmd_cpt == 0 ? MDRV_MBX_IOC_GETDRVSTATUS : cmd_cpt);

			mdrv_mbx_get_drvstatus_compat_get_ion_allaction_data(data32, data);
    		ret = filp->f_op->unlocked_ioctl(filp, cmd_cpt, (unsigned long) data);
			mdrv_mbx_get_drvstatus_compat_put_ion_allaction_data(data32, data);

			return ret;
		}
		break;

		
		case MDRV_MBX_IOC_DEINIT:
		case MDRV_MBX_IOC_UNREGISTERMSG:
		case MDRV_mBX_IOC_CLEARMSG:
		case MDRV_MBX_IOC_REMOVEMSG:
		case MDRV_MBX_IOC_MBXENABLE:
		case MDRV_PM_IOC_GET_BRICKTERMINATOR_INFO:
		case MDRV_PM_IOC_SET_BRICKTERMINATOR_INFO:
		default:
    		return filp->f_op->unlocked_ioctl(filp, cmd,(unsigned long)compat_ptr(arg));
		    break;

	}

	return 0;
}
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
long _MDrv_MBXIO_IOCtl(struct file *filp, U32 u32Cmd, unsigned long u32Arg)
#else
long _MDrv_MBXIO_IOCtl(struct inode *inode, struct file *filp, U32 u32Cmd, unsigned long u32Arg)
#endif
{
    int err = 0;
    int retval = 0;

    if(_devMBX.refCnt <= 0)
    {
        return -EFAULT;
    }

    /* check u32Cmd valid */
    if (MDRV_MBX_IOC_MAGIC!= _IOC_TYPE(u32Cmd))
    {
        return -ENOTTY;
    }

    if (_IOC_NR(u32Cmd) > MDRV_MBX_IOC_MAX_NR)
    {
        MBXIO_KDBG("IOCtl NR Error!!! (Cmd=%x)\n",u32Cmd);
        return -ENOTTY;
    }

    /* verify Access */
    if (_IOC_DIR(u32Cmd) & _IOC_READ)
    {
        err = !access_ok(VERIFY_WRITE, (void __user *)u32Arg, _IOC_SIZE(u32Cmd));
    }
    else if (_IOC_DIR(u32Cmd) & _IOC_WRITE)
    {
        err =  !access_ok(VERIFY_READ, (void __user *)u32Arg, _IOC_SIZE(u32Cmd));
    }
    if (err)
    {
        return -EFAULT;
    }

    _MDrv_MBXIO_IOC_Lock();

    /* handle u32Cmd */
    switch(u32Cmd)
    {
        case MDRV_MBX_IOC_INIT:
            retval = _MDrv_MBXIO_IOC_Init(filp, u32Arg);
            break;
        case MDRV_MBX_IOC_DEINIT:
            retval = _MDrv_MBXIO_IOC_DeInit(filp, u32Arg);
            break;
        case MDRV_MBX_IOC_REGISTERMSG:
            retval = _MDrv_MBXIO_IOC_RegisterMsg(filp, u32Arg);
            break;
        case MDRV_MBX_IOC_UNREGISTERMSG:
            retval = _MDrv_MBXIO_IOC_UnRegisterMsg(filp, u32Arg);
            break;
        case MDRV_mBX_IOC_CLEARMSG:
            retval = _MDrv_MBXIO_IOC_ClearMsg(filp, u32Arg);
            break;
        case MDRV_MBX_IOC_SENDMSG:
            retval = _MDrv_MBXIO_IOC_SendMsg(filp, u32Arg);
            break;
        case MDRV_MBX_IOC_RECVMSG:
            retval = _MDrv_MBXIO_IOC_RecvMsg(filp, u32Arg);
            break;
        case MDRV_MBX_IOC_CHECKMSG:
            retval = _MDrv_MBXIO_IOC_CheckMsg(filp, u32Arg);
            break;
        case MDRV_MBX_IOC_REMOVEMSG:
            retval = _MDrv_MBXIO_IOC_RemoveLatestMsg(filp, u32Arg);
            break;
        case MDRV_MBX_IOC_GETMSGQSTATUS:
            retval = _MDrv_MBXIO_IOC_GetMsgQStatus(filp, u32Arg);
            break;
        case MDRV_MBX_IOC_MBXENABLE:
            retval = _MDrv_MBXIO_IOC_MbxEnable(filp, u32Arg);
            break;
        case MDRV_MBX_IOC_SETINFORMATION:
            retval = _MDrv_MBXIO_IOC_SetInformation(filp, u32Arg);
            break;
        case MDRV_MBX_IOC_GETINFORMATION:
            retval = _MDrv_MBXIO_IOC_GetInformation(filp, u32Arg);
            break;
        case MDRV_MBX_IOC_SENDMSGLOOPBACK:
            retval = _MDrv_MBXIO_IOC_SendMsgLoopback(filp, u32Arg);
            break;
        case MDRV_MBX_IOC_GETDRVSTATUS:
            retval = _MDrv_MBXIO_IOC_GetDrvStatus(filp, u32Arg);
            break;
        case MDRV_PM_IOC_GET_BRICKTERMINATOR_INFO:
            retval = _MDrv_PMIO_IOC_Get_BrickTerminator_Info(filp, u32Arg);  
            break;
		case MDRV_PM_IOC_SET_BRICKTERMINATOR_INFO:
            retval = _MDrv_PMIO_IOC_Set_BrickTerminator_Info(filp, u32Arg);
            break;
        
        default:  /* redundant, as cmd was checked against MAXNR */
	        _MDrv_MBXIO_IOC_UnLock();
			if ((_IOC_NR(MDRV_MBX_IOC_INIT) == _IOC_NR(u32Cmd)) &&
				(_IOC_SIZE(MDRV_MBX_IOC_INIT) != _IOC_SIZE(u32Cmd)))
				MBXIO_ASSERT(0);
			else
                MBXIO_KDBG(" ERROR IOCtl number %x\n ",u32Cmd);
			return -ENOTTY;
    }

    _MDrv_MBXIO_IOC_UnLock();
    return retval;
}

static MSTAR_MBX_DEV _st_mbxdev={0};
static int mstar_mbx_drv_suspend(struct platform_device *dev, pm_message_t state)
{
    MSTAR_MBX_DEV *mbxdev=(MSTAR_MBX_DEV*)dev->dev.platform_data;

    return MDrv_MBX_Suspend(mbxdev);
}
static int mstar_mbx_drv_resume(struct platform_device *dev)
{
    MSTAR_MBX_DEV *mbxdev=(MSTAR_MBX_DEV*)dev->dev.platform_data;

    return MDrv_MBX_Resume(mbxdev);
}

static int mstar_mbx_drv_probe(struct platform_device *pdev)
{
    int retval=0;
    pdev->dev.platform_data=&_st_mbxdev;
	return retval;
}

static int mstar_mbx_drv_remove(struct platform_device *pdev)
{
    pdev->dev.platform_data=NULL;
    return 0;
}

#if defined (CONFIG_ARM64)
static struct of_device_id mstarmbx_of_device_ids[] = {
         {.compatible = "mstar-mbx"},
         {},
};
#endif

static struct platform_driver Mstar_mbx_driver = {
	.probe 		= mstar_mbx_drv_probe,
	.remove 	= mstar_mbx_drv_remove,
    .suspend    = mstar_mbx_drv_suspend,
    .resume     = mstar_mbx_drv_resume,

	.driver = {
#if defined(CONFIG_ARM64)
                .of_match_table = mstarmbx_of_device_ids,
#endif
		.name	= "Mstar-mbx",
        .owner  = THIS_MODULE,
	}
};
//-------------------------------------------------------------------------------------------------
// Module functions
//-------------------------------------------------------------------------------------------------
MSYSTEM_STATIC int _MDrv_MBXIO_ModuleInit(void)
{
    int s32Ret;
    dev_t  dev;

    if(_devMBX.s32Major)
    {
        dev = MKDEV(_devMBX.s32Major, _devMBX.s32Minor);
        s32Ret = register_chrdev_region(dev, MDRV_MBX_DEVICE_COUNT, MDRV_MBX_NAME);
    }
    else
    {
        s32Ret = alloc_chrdev_region(&dev, _devMBX.s32Minor, MDRV_MBX_DEVICE_COUNT, MDRV_MBX_NAME);
        _devMBX.s32Major = MAJOR(dev);
    }

    if (0 > s32Ret)
    {
        MBXIO_KDBG("Unable to get major %d\n", _devMBX.s32Major);
        return s32Ret;
    }

    cdev_init(&_devMBX.cdev, &_devMBX.fops);
    if (0 != (s32Ret= cdev_add(&_devMBX.cdev, dev, MDRV_MBX_DEVICE_COUNT)))
    {
        MBXIO_KDBG("Unable add a character device\n");
        unregister_chrdev_region(dev, MDRV_MBX_DEVICE_COUNT);
        return s32Ret;
    }

    /* initial the whole MBX Driver */
    if(E_MBX_SUCCESS != MDrv_MBX_Startup())
    {
        MBXIO_KDBG("Startup MBX Driver Failed! %d\n", _devMBX.s32Major);
        cdev_del(&_devMBX.cdev);
        unregister_chrdev_region(dev, MDRV_MBX_DEVICE_COUNT);
	 return -ENOMEM;
    }

    DRV_MBX_LockIOCTL_Init();
	
    platform_driver_register(&Mstar_mbx_driver);
    return 0;
}


MSYSTEM_STATIC void _MDrv_MBXIO_ModuleExit(void)
{
    /*de-initial the who MBX Driver */
    MDrv_MBX_Exit();

    cdev_del(&_devMBX.cdev);
    unregister_chrdev_region(MKDEV(_devMBX.s32Major, _devMBX.s32Minor), MDRV_MBX_DEVICE_COUNT);
    platform_driver_unregister(&Mstar_mbx_driver);
}

#if defined(CONFIG_MSTAR_MSYSTEM) || defined(CONFIG_MSTAR_MSYSTEM_MODULE)
#else//#if defined(CONFIG_MSTAR_MSYSTEM) || defined(CONFIG_MSTAR_MSYSTEM_MODULE)
module_init(_MDrv_MBXIO_ModuleInit);
module_exit(_MDrv_MBXIO_ModuleExit);


EXPORT_SYMBOL(_MDrv_MBXIO_IOC_Lock);
EXPORT_SYMBOL(_MDrv_MBXIO_IOC_UnLock);


MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("MSMAILBOX ioctrl driver");
MODULE_LICENSE("GPL");
#endif//#if defined(CONFIG_MSTAR_MSYSTEM) || defined(CONFIG_MSTAR_MSYSTEM_MODULE)
