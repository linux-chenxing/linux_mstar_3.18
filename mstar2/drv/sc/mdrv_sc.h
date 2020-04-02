///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2006 - 2008 MStar Semiconductor, Inc.
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
/// @file   mdrv_temp.h
/// @brief  TEMP Driver Interface
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_SC_H_
#define _MDRV_SC_H_

#include <linux/fs.h>
#include <linux/cdev.h>
#include "mdrv_types.h"
#include <linux/version.h>

//-------------------------------------------------------------------------------------------------
//  Driver Capability
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define SC_FIFO_SIZE                512                                 // Rx fifo size

// #define SC_DEBUG
#ifdef SC_DEBUG
#define SC_PRINT(_fmt, _args...)    printk(KERN_WARNING "[%s][%d] " _fmt, __FUNCTION__, __LINE__, ## _args)
#define SC_ASSERT(_con) \
    do { \
        if (!(_con)) { \
            printk(KERN_CRIT "BUG at %s:%d assert(%s)\n", \
                    __FILE__, __LINE__, #_con); \
            BUG(); \
        } \
    } while (0)
#else
#define SC_PRINT(fmt, args...)
#define SC_ASSERT(arg)
#endif


//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

typedef struct
{
    int                         s32Major;
    int                         s32Minor;
    struct cdev                 stCDev;
    struct file_operations      fops;
    struct fasync_struct        *async_queue; /* asynchronous readers */
} SC_DEV;

/// SmartCard Info
typedef struct
{
    BOOL                        bCardIn;                            ///Status care in
    BOOL                        bLastCardIn;
    U32                         u32CardStatus;
    wait_queue_head_t           stWaitQue;

    U8                          u8FifoRx[SC_FIFO_SIZE];
    U16                         u16FifoRxRead;
    U16                         u16FifoRxWrite;

    U8                          u8FifoTx[SC_FIFO_SIZE];
    U16                         u16FifoTxRead;
    U16                         u16FifoTxWrite;
    U32                         u32CardAttr;
} SC_Info;


//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
int MDrv_SC_Open(struct inode *inode, struct file *filp);
ssize_t MDrv_SC_Read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
ssize_t MDrv_SC_Write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);
unsigned int MDrv_SC_Poll(struct file *filp, poll_table *wait);
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,36)
int MDrv_SC_AttachInterrupt(struct file *filp, unsigned long arg);
int MDrv_SC_DetachInterrupt(struct file *filp, unsigned long arg);
int MDrv_SC_ResetFIFO(struct file *filp, unsigned long arg);
int MDrv_SC_GetEvent(struct file *filp, unsigned long arg);
int MDrv_SC_SetEvent(struct file *filp, unsigned long arg);
int MDrv_SC_GetAttribute(struct file *filp, unsigned long arg);
int MDrv_SC_SetAttribute(struct file *filp, unsigned long arg);
int MDrv_SC_CheckRstToATR(struct file *filp, unsigned long arg);
#else
int MDrv_SC_AttachInterrupt(struct inode *inode, struct file *filp, unsigned long arg);
int MDrv_SC_DetachInterrupt(struct inode *inode, struct file *filp, unsigned long arg);
int MDrv_SC_ResetFIFO(struct inode *inode, struct file *filp, unsigned long arg);
int MDrv_SC_GetEvent(struct inode *inode, struct file *filp, unsigned long arg);
int MDrv_SC_SetEvent(struct inode *inode, struct file *filp, unsigned long arg);
int MDrv_SC_GetAttribute(struct inode *inode, struct file *filp, unsigned long arg);
int MDrv_SC_SetAttribute(struct inode *inode, struct file *filp, unsigned long arg);
int MDrv_SC_CheckRstToATR(struct inode *inode, struct file *filp, unsigned long arg);
#endif


#endif // _MDRV_TEMP_H_

