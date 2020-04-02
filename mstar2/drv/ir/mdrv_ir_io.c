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
/// file    drvIR.c
/// @brief  IR Control Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
//#include "MsCommon.h"
#include <linux/autoconf.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/cdev.h>
#include <linux/time.h>  //added
#include <linux/timer.h> //added
#include <linux/platform_device.h>
#include <asm/io.h>

#include "mst_devid.h"
#include "mdrv_ir_io.h"
#include "mdrv_ir.h"

#include "mdrv_system.h"

//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------
#define IR_PRINT(fmt, args...)      //printk("IR: [%05d] \n" fmt, __LINE__, ## args)

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define MOD_IR_DEVICE_COUNT     1

//#define IR_DEBUG
#ifdef IR_DEBUG
    #define DEBUG_IR(x) (x)
#else
    #define DEBUG_IR(x)
#endif

//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// Forward declaration
//--------------------------------------------------------------------------------------------------
static int                      _mod_ir_open (struct inode *inode, struct file *filp);
static int                      _mod_ir_release(struct inode *inode, struct file *filp);
static ssize_t                  _mod_ir_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
static ssize_t                  _mod_ir_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);
static unsigned int             _mod_ir_poll(struct file *filp, poll_table *wait);
#if defined(CONFIG_COMPAT)
static long _mod_ir_compat_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
#endif
#ifdef HAVE_UNLOCKED_IOCTL
static long _mod_ir_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
#else
static int _mod_ir_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
#endif
static int                      _mod_ir_fasync(int fd, struct file *filp, int mode);

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
IRModHandle IRDev=
{
    .s32IRMajor=               MDRV_MAJOR_IR,
    .s32IRMinor=               MDRV_MINOR_IR,
    .cDevice=
    {
        .kobj=                  {.name= MDRV_NAME_IR, },
        .owner  =               THIS_MODULE,
    },
    .IRFop=
    {
        .open=                  _mod_ir_open,
        .release=               _mod_ir_release,
        .read=                  _mod_ir_read,
        .write=                 _mod_ir_write,
        .poll=                  _mod_ir_poll,
#ifdef HAVE_UNLOCKED_IOCTL
        .unlocked_ioctl = _mod_ir_ioctl,
#else
        .ioctl = _mod_ir_ioctl,
#endif
#if defined(CONFIG_COMPAT)
        .compat_ioctl = _mod_ir_compat_ioctl,
#endif

        .fasync =               _mod_ir_fasync,
    },
};

//-------------------------------------------------------------------------------------------------
// Local Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
/// Initialize IR timing and enable IR.
/// @return None
//-------------------------------------------------------------------------------------------------
static int _mod_ir_open(struct inode *inode, struct file *filp) {
    IRModHandle *dev;

    dev = container_of(inode->i_cdev, IRModHandle, cDevice);
    filp->private_data = dev;
    return 0;
}

static int _mod_ir_release(struct inode *inode, struct file *filp) {
    return 0;
}

static ssize_t _mod_ir_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {
#if defined(CONFIG_MSTAR_GPIO)
#if defined(CONFIG_MSTAR_IR_GPIO_TOGGLE)
    gpio_toggle_trigger();
#endif
#endif
    return MDrv_IR_Read(filp,buf,count,f_pos);
}

static ssize_t _mod_ir_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {
    return 0;
}

static unsigned int _mod_ir_poll(struct file *filp, poll_table *wait) {
    return MDrv_IR_Poll(filp,wait);
}

static int _mod_ir_fasync(int fd, struct file *filp, int mode) {
    return fasync_helper(fd, filp, mode, &IRDev.async_queue);
}

static MS_IR_ShotInfo stIrShotInfo; //for compile warnning , stack size exceed 1024kb
#if defined(CONFIG_COMPAT)
static long _mod_ir_compat_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int err= 0;
    int retval = 0;
    MS_IR_DelayTime delaytime;
    MS_IR_LastKeyTime keytime;
    MS_IR_KeyValue keyvalue;
    U8 bEnableIR;
    MS_IR_KeyInfo keyinfo;
    pid_t masterPid;

    /*
     * extract the type and number bitfields, and don't decode
     * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
     */
    if ((IR_IOC_MAGIC!= _IOC_TYPE(cmd)) || (_IOC_NR(cmd)> IR_IOC_MAXNR)) {
        return -ENOTTY;
    }

    /*
     * the direction is a bitmask, and VERIFY_WRITE catches R/W
     * transfers. `Type' is user-oriented, while
     * access_ok is kernel-oriented, so the concept of "read" and
     * "write" is reversed
     */
    if (_IOC_DIR(cmd) & _IOC_READ) {
        err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
    } else if (_IOC_DIR(cmd) & _IOC_WRITE) {
        err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
    }
    if (err) {
        return -EFAULT;
    }

    switch (cmd) {
        case MDRV_IR_INIT:
            MDrv_IR_Init(0);
            IRDev.u32IRFlag |= (IRFLAG_IRENABLE|IRFLAG_HWINITED);
            break;

        case MDRV_IR_SEND_KEY:
            retval = __get_user(keyinfo.u8Key, &(((MS_IR_KeyInfo __user *)arg)->u8Key));
            retval = __get_user(keyinfo.u8Flag, &(((MS_IR_KeyInfo __user *)arg)->u8Flag));
            MDrv_IR_SendKey(keyinfo.u8Key, keyinfo.u8Flag);
            break;

        case MDRV_IR_SET_DELAYTIME:
            retval = __get_user(delaytime.u32_1stDelayTimeMs, &(((MS_IR_DelayTime __user *)arg)->u32_1stDelayTimeMs));
            retval = __get_user(delaytime.u32_2ndDelayTimeMs, &(((MS_IR_DelayTime __user *)arg)->u32_2ndDelayTimeMs));
            MDrv_IR_SetDelayTime(delaytime.u32_1stDelayTimeMs, delaytime.u32_2ndDelayTimeMs);
            break;

        case MDRV_IR_ENABLE_IR:
            retval = __get_user(bEnableIR, (int __user *)arg);
            MDrv_IR_EnableIR(bEnableIR);
            if (bEnableIR) {
                IRDev.u32IRFlag |= IRFLAG_IRENABLE;
            } else {
                IRDev.u32IRFlag &= ~IRFLAG_IRENABLE;
            }
            break;

        case MDRV_IR_SET_MASTER_PID:
            retval = __get_user(masterPid, (pid_t __user *)arg);
            MDrv_IR_SetMasterPid(masterPid);
            break;

        case MDRV_IR_GET_MASTER_PID:
            masterPid = MDrv_IR_GetMasterPid();
            retval = __put_user(masterPid, (pid_t __user *)arg);
            break;

        case MDRV_IR_INITCFG: {
                MS_IR_InitCfg stInitCfg;
                if (copy_from_user(&stInitCfg, (MS_IR_InitCfg __user *)arg, sizeof(MS_IR_InitCfg))) {
                    return EFAULT;
                }
                MDrv_IR_InitCfg((MS_IR_InitCfg*)&stInitCfg);
            } break;

        case MDRV_IR_TIMECFG: {
                MS_IR_TimeCfg stTimeCfg;
                if (copy_from_user(&stTimeCfg, (MS_IR_TimeCfg __user *)arg, sizeof(MS_IR_TimeCfg))) {
                    return EFAULT;
                }
                MDrv_IR_TimeCfg((MS_IR_TimeCfg*)&stTimeCfg);
            } break;

        case MDRV_IR_GET_SWSHOT_BUF:
            MDrv_IR_ReadShotBuffer(&stIrShotInfo);
            if (copy_to_user((MS_IR_ShotInfo  __user *) arg, &stIrShotInfo, sizeof(MS_IR_ShotInfo))) {
                return -EFAULT;
            }
            break;

        case MDRV_IR_SET_HEADER: {
                MS_MultiIR_HeaderInfo stItMulti_HeaderInfo;
                if (copy_from_user(&stItMulti_HeaderInfo, (MS_MultiIR_HeaderInfo __user *)arg, sizeof(MS_MultiIR_HeaderInfo))) {
                    return EFAULT;
                }
                MDrv_IR_SetHeaderCode((MS_MultiIR_HeaderInfo*)&stItMulti_HeaderInfo);
            } break;

        case MDRV_IR_SET_PROTOCOL: {
                MS_MultiProtocolCfg stProtocolCfg[E_IR_PROTOCOL_MAX];
                U8 u8ListSize,u8index=0;
                U8 protocolsList[E_IR_PROTOCOL_MAX];

                if (copy_from_user(protocolsList, (U8 __user *)arg, sizeof(protocolsList))) {
                    return EFAULT;
                }

                u8ListSize = protocolsList[0];//User space array size is passed to kernel as first element,we get the size from protocolsList[0]
                for(u8index=0;u8index<(u8ListSize-1);u8index++)
                {
                    stProtocolCfg[u8index].eProtocol = protocolsList[u8index+1];
                    stProtocolCfg[u8index].pNextProtCfg = &stProtocolCfg[u8index+1];
                }
                stProtocolCfg[u8index-1].pNextProtCfg = NULL;//After filling all linked list elements we set last element Next pointer to NULL

                MDrv_IR_SetProtocol((MS_MultiProtocolCfg*)stProtocolCfg);
            } break;

        default:
            IR_PRINT("ioctl: unknown command\n");
            return -ENOTTY;
    }
    return 0;
}
#endif

#ifdef HAVE_UNLOCKED_IOCTL
static long _mod_ir_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
#else
static int _mod_ir_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
#endif
{
    int err= 0;
    int retval = 0;
    MS_IR_DelayTime delaytime;
    U8 bEnableIR;
    MS_IR_KeyInfo keyinfo;
    pid_t masterPid;

    /*
     * extract the type and number bitfields, and don't decode
     * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
     */
    if ((IR_IOC_MAGIC!= _IOC_TYPE(cmd)) || (_IOC_NR(cmd)> IR_IOC_MAXNR)) {
        return -ENOTTY;
    }

    /*
     * the direction is a bitmask, and VERIFY_WRITE catches R/W
     * transfers. `Type' is user-oriented, while
     * access_ok is kernel-oriented, so the concept of "read" and
     * "write" is reversed
     */
    if (_IOC_DIR(cmd) & _IOC_READ) {
        err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
    } else if (_IOC_DIR(cmd) & _IOC_WRITE) {
        err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
    }
    if (err) {
        return -EFAULT;
    }

    switch (cmd) {
        case MDRV_IR_INIT:
            MDrv_IR_Init(0);
            IRDev.u32IRFlag |= (IRFLAG_IRENABLE|IRFLAG_HWINITED);
            break;

        case MDRV_IR_SEND_KEY:
            retval = __get_user(keyinfo.u8Key, &(((MS_IR_KeyInfo __user *)arg)->u8Key));
            retval = __get_user(keyinfo.u8Flag, &(((MS_IR_KeyInfo __user *)arg)->u8Flag));
            MDrv_IR_SendKey(keyinfo.u8Key, keyinfo.u8Flag);
            break;

        case MDRV_IR_SET_DELAYTIME:
            retval = __get_user(delaytime.u32_1stDelayTimeMs, &(((MS_IR_DelayTime __user *)arg)->u32_1stDelayTimeMs));
            retval = __get_user(delaytime.u32_2ndDelayTimeMs, &(((MS_IR_DelayTime __user *)arg)->u32_2ndDelayTimeMs));
            MDrv_IR_SetDelayTime(delaytime.u32_1stDelayTimeMs, delaytime.u32_2ndDelayTimeMs);
            break;

        case MDRV_IR_ENABLE_IR:
            retval = __get_user(bEnableIR, (int __user *)arg);
            MDrv_IR_EnableIR(bEnableIR);
            if (bEnableIR) {
                IRDev.u32IRFlag |= IRFLAG_IRENABLE;
            } else {
                IRDev.u32IRFlag &= ~IRFLAG_IRENABLE;
            }
            break;

        case MDRV_IR_SET_MASTER_PID:
            retval = __get_user(masterPid, (pid_t __user *)arg);
            MDrv_IR_SetMasterPid(masterPid);
            break;

        case MDRV_IR_GET_MASTER_PID:
            masterPid = MDrv_IR_GetMasterPid();
            retval = __put_user(masterPid, (pid_t __user *)arg);
            break;

        case MDRV_IR_INITCFG: {
                MS_IR_InitCfg stInitCfg;
                if (copy_from_user(&stInitCfg, (MS_IR_InitCfg __user *)arg, sizeof(MS_IR_InitCfg))) {
                    return EFAULT;
                }
                MDrv_IR_InitCfg((MS_IR_InitCfg*)&stInitCfg);
            } break;

        case MDRV_IR_TIMECFG: {
                MS_IR_TimeCfg stTimeCfg;
                if (copy_from_user(&stTimeCfg, (MS_IR_TimeCfg __user *)arg, sizeof(MS_IR_TimeCfg))) {
                    return EFAULT;
                }
                MDrv_IR_TimeCfg((MS_IR_TimeCfg*)&stTimeCfg);
            } break;

        case MDRV_IR_GET_SWSHOT_BUF:
            MDrv_IR_ReadShotBuffer(&stIrShotInfo);
            if (copy_to_user((MS_IR_ShotInfo  __user *) arg, &stIrShotInfo, sizeof(MS_IR_ShotInfo))) {
                return -EFAULT;
            }
            break;

        case MDRV_IR_SET_HEADER: {
                MS_MultiIR_HeaderInfo stItMulti_HeaderInfo;
                if (copy_from_user(&stItMulti_HeaderInfo, (MS_MultiIR_HeaderInfo __user *)arg, sizeof(MS_MultiIR_HeaderInfo))) {
                    return EFAULT;
                }
                MDrv_IR_SetHeaderCode((MS_MultiIR_HeaderInfo*)&stItMulti_HeaderInfo);
            } break;

        case MDRV_IR_SET_PROTOCOL: {
                MS_MultiProtocolCfg stProtocolCfg[E_IR_PROTOCOL_MAX];
                U8 u8ListSize,u8index=0;
                U8 protocolsList[E_IR_PROTOCOL_MAX];

                if (copy_from_user(protocolsList, (U8 __user *)arg, sizeof(protocolsList))) {
                    return EFAULT;
                }

                u8ListSize = protocolsList[0];//User space array size is passed to kernel as first element,we get the size from protocolsList[0]
                for(u8index=0;u8index<(u8ListSize-1);u8index++)
                {
                    stProtocolCfg[u8index].eProtocol = protocolsList[u8index+1];
                    stProtocolCfg[u8index].pNextProtCfg = &stProtocolCfg[u8index+1];
                }
                stProtocolCfg[u8index-1].pNextProtCfg = NULL;//After filling all linked list elements we set last element Next pointer to NULL

                MDrv_IR_SetProtocol((MS_MultiProtocolCfg*)stProtocolCfg);
            } break;

        default:
            IR_PRINT("ioctl: unknown command\n");
            return -ENOTTY;
    }

    return 0;
}

static int mod_ir_init(void) {
    int ret;
    dev_t dev = 0;

    if (IRDev.s32IRMajor) {
        dev = MKDEV(IRDev.s32IRMajor, IRDev.s32IRMinor);
        ret = register_chrdev_region(dev, MOD_IR_DEVICE_COUNT, MDRV_NAME_IR);
    } else {
        ret = alloc_chrdev_region(&dev, IRDev.s32IRMinor, MOD_IR_DEVICE_COUNT, MDRV_NAME_IR);
        IRDev.s32IRMajor = MAJOR(dev);
    }

    if ( 0 > ret) {
        IR_PRINT("Unable to get major %d\n", IRDev.s32IRMajor);
        return ret;
    }

    cdev_init(&IRDev.cDevice, &IRDev.IRFop);
    if (0!= (ret= cdev_add(&IRDev.cDevice, dev, MOD_IR_DEVICE_COUNT))) {
        IR_PRINT("Unable add a character device\n");
        unregister_chrdev_region(dev, MOD_IR_DEVICE_COUNT);
        return ret;
    }

#ifdef CONFIG_MSTAR_IR_INPUT_DEVICE
    MDrv_IR_Input_Init();
    MDrv_IR_Init(0);
    IRDev.u32IRFlag |= (IRFLAG_IRENABLE|IRFLAG_HWINITED);
#endif

    return 0;
}

static void mod_ir_exit(void) {
    cdev_del(&IRDev.cDevice);
    unregister_chrdev_region(MKDEV(IRDev.s32IRMajor, IRDev.s32IRMinor), MOD_IR_DEVICE_COUNT);

#ifdef CONFIG_MSTAR_IR_INPUT_DEVICE
    MDrv_IR_Input_Exit();
#endif
}

static int mstar_ir_drv_suspend(struct platform_device *dev, pm_message_t state) {
    IRModHandle *pIRHandle = (IRModHandle *)(dev->dev.platform_data);
    if (pIRHandle && (pIRHandle->u32IRFlag&IRFLAG_HWINITED)) {
        if (pIRHandle->u32IRFlag & IRFLAG_IRENABLE) {
            MDrv_IR_EnableIR(0);
        }
        MDrv_IR_Disable_SWFIFO();
    }
    return 0;
}

static int mstar_ir_drv_resume(struct platform_device *dev) {
    IRModHandle *pIRHandle=(IRModHandle *)(dev->dev.platform_data);
    if (pIRHandle && (pIRHandle->u32IRFlag&IRFLAG_HWINITED)) {
        MDrv_IR_Init(1);
        if (pIRHandle->u32IRFlag & IRFLAG_IRENABLE) {
            MDrv_IR_EnableIR(1);
        }
    }
    return 0;
}

#if defined (CONFIG_ARM64)
static struct of_device_id mstarir_of_device_ids[] = {
    {.compatible = "mstar-ir"},
    {},
};
#endif

static int mstar_ir_drv_probe(struct platform_device *pdev) {
    int retval=0;
    if (!(pdev->name) || strcmp(pdev->name,"Mstar-ir")
        || pdev->id!=0) {
        retval = -ENXIO;
    }

    IRDev.u32IRFlag = 0;
    retval = mod_ir_init();
    if (!retval) {
        pdev->dev.platform_data=&IRDev;
    }
    return retval;
}

static int mstar_ir_drv_remove(struct platform_device *pdev) {
    if (!(pdev->name) || strcmp(pdev->name,"Mstar-ir")
        || pdev->id!=0) {
        return -1;
    }
    mod_ir_exit();
    IRDev.u32IRFlag = 0;
    pdev->dev.platform_data=NULL;
    return 0;
}

static struct platform_driver Mstar_ir_driver = {
    .probe      = mstar_ir_drv_probe,
    .remove     = mstar_ir_drv_remove,
    .suspend    = mstar_ir_drv_suspend,
    .resume     = mstar_ir_drv_resume,

    .driver = {
#if defined(CONFIG_ARM64)
        .of_match_table = mstarir_of_device_ids,
#endif
        .name   = "Mstar-ir",
        .owner  = THIS_MODULE,
    }
};


static int __init mstar_ir_drv_init_module(void) {
    int retval=0;
    retval = platform_driver_register(&Mstar_ir_driver);
    return retval;
}

static void __exit mstar_ir_drv_exit_module(void) {
    platform_driver_unregister(&Mstar_ir_driver);
}

module_init(mstar_ir_drv_init_module);
module_exit(mstar_ir_drv_exit_module);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("IR driver");
MODULE_LICENSE("GPL");
