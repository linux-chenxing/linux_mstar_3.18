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
/// file    mdrv_pwm.c
/// @brief  TEMP Driver Interface for Export
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/version.h>
#include <asm/io.h>
#include <asm/uaccess.h>

//drver header files
#include "mst_devid.h"
//#include "mdrv_mstypes.h"
#include "MsTypes.h"
#include "mdrv_pwm.h"
#include "mhal_pwm.h"
#include "mreg_pwm.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------
typedef struct
{
    int                         s32Major;
    int                         s32Minor;
    struct cdev                 stCDev;
    struct file_operations      fops;
} PWM_DEV;


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// Forward declaration
//--------------------------------------------------------------------------------------------------
static int                  _mod_pwm_open (struct inode *inode, struct file *filp);
static int                  _mod_pwm_release(struct inode *inode, struct file *filp);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
static long                 _mod_pwm_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
#else
static int                  _mod_pwm_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
#endif

PWM_DEV devPWM =
{
    .s32Major   =           MDRV_MAJOR_PWM,
    .s32Minor   =           MDRV_MINOR_PWM,
    .stCDev =
    {
        .kobj   =           {.name = MDRV_NAME_PWM, },
        .owner  =           THIS_MODULE,
    },
    .fops =
    {
        .open   =           _mod_pwm_open,
        .release=           _mod_pwm_release,
        #if LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,20)
        .unlocked_ioctl  =  _mod_pwm_ioctl,
        #else
        .ioctl  =           _mod_pwm_ioctl,
        #endif
    },
};


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static int _mod_pwm_open(struct inode *inode, struct file *filp)
{
    PWM_PRINT("%s is invoked\n", __FUNCTION__);
    filp->private_data = (void*)(iminor(inode) - devPWM.s32Minor);

    return FALSE;
}

static int _mod_pwm_release(struct inode *inode, struct file *filp)
{
    PWM_PRINT("%s is invoked\n", __FUNCTION__);
    return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
static long _mod_pwm_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
#else
static int _mod_pwm_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
#endif
{
    int err= 0;

    PWM_PRINT("%s is invoked\n", __FUNCTION__);
    /*
     * extract the type and number bitfields, and don't decode
     * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
     */
    if ((PWM_IOC_MAGIC != _IOC_TYPE(cmd)) || (_IOC_NR(cmd) > PWM_IOC_MAXNR))
    {
        return -ENOTTY;
    }

    /*
     * the direction is a bitmask, and VERIFY_WRITE catches R/W
     * transfers. `Type' is user-oriented, while
     * access_ok is kernel-oriented, so the concept of "read" and
     * "write" is reversed
     */
    if (_IOC_DIR(cmd) & _IOC_READ)
    {
        err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
    }
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
    {
        err = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
    }
    if (err)
    {
        return -EFAULT;
    }

    #if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,36)
    switch(cmd)
    {
        default:
            PWM_PRINT("ioctl: unknown command\n");
            return -ENOTTY;
    }
    #else
    switch(cmd)
    {
        default:
            PWM_PRINT("ioctl: unknown command\n");
            return -ENOTTY;
    }
    #endif

    return 0;
}


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
MSYSTEM_STATIC int __init mod_pwm_init(void)
{
    int         s32Ret;
    dev_t       dev;

   
    PWM_PRINT("%s is invoked\n", __FUNCTION__);

    if (devPWM.s32Major)
    {
        dev = MKDEV(devPWM.s32Major, devPWM.s32Minor);
        s32Ret = register_chrdev_region(dev, PWM_Num, MDRV_NAME_PWM);
    }
    else
    {
        s32Ret = alloc_chrdev_region(&dev, devPWM.s32Minor, PWM_Num, MDRV_NAME_PWM);
        devPWM.s32Major = MAJOR(dev);
    }

    if (0 > s32Ret)
    {
        PWM_PRINT("Unable to get major %d\n", devPWM.s32Major);
        return s32Ret;
    }

    cdev_init(&devPWM.stCDev, &devPWM.fops);
    if (0 != (s32Ret = cdev_add(&devPWM.stCDev, dev, PWM_Num)))
    {
        PWM_PRINT("Unable add a character device\n");
        unregister_chrdev_region(dev, PWM_Num);
        return s32Ret;
    }
    MDrv_PWM_Init(E_PWM_DBGLV_ERR_ONLY);

    return 0;
}

MSYSTEM_STATIC void __exit mod_pwm_exit(void)
{
    PWM_PRINT("%s is invoked\n", __FUNCTION__);

    cdev_del(&devPWM.stCDev);
    unregister_chrdev_region(MKDEV(devPWM.s32Major, devPWM.s32Minor), PWM_Num);
}

module_init(mod_pwm_init);
module_exit(mod_pwm_exit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("pwm driver");
MODULE_LICENSE("GPL");

