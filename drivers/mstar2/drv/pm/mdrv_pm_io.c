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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    mdrv_pm_io.c
/// @brief  PM I/O Control Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
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
#include <linux/time.h> 
#include <linux/timer.h>
#include <linux/platform_device.h>
#include <asm/io.h>
#if defined(CONFIG_COMPAT)
#include <linux/compat.h>
#endif

#include "mdrv_pm_io.h"
#include "mdrv_pm.h"
#include "mst_devid.h"



//--------------------------------------------------------------------------------------------------
// Forward declaration
//--------------------------------------------------------------------------------------------------
static int _MDrv_PM_io_open(struct inode *inode, struct file *filp);
static int _MDrv_PM_io_release(struct inode *inode, struct file *filp);

#define     MDRV_PM_DEVICE_COUNT            1
#define     MDRV_PM_NAME                    "PM"


#ifdef HAVE_UNLOCKED_IOCTL
long _MDrv_PM_io_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
#else
int  _MDrv_PM_io_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
#endif
#if defined(CONFIG_COMPAT)
static long _Compat_MDrv_PM_io_ioctlOCtl(struct file *filp, unsigned int cmd, unsigned long arg);
#endif

//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------
typedef struct
{
    u32 Address;
    u32 Size;
} PM_DRAM_INFO;


typedef struct
{
    int s32Major;
    int s32Minor;
    struct cdev cdev;
    struct file_operations fops;
} PM_DEV;

PM_DEV _PMDev=
{
    .s32Major=               MDRV_MAJOR_PM,
    .s32Minor=               MDRV_MINOR_PM,
    .cdev=
    {
        .kobj=                  {.name= MDRV_NAME_PM, },
        .owner  =               THIS_MODULE,
    },
    .fops=
    {
        .open=                  _MDrv_PM_io_open,
        .release=               _MDrv_PM_io_release,
        #ifdef HAVE_UNLOCKED_IOCTL
        .unlocked_ioctl =       _MDrv_PM_io_ioctl,
        #else
        .ioctl =                _MDrv_PM_io_ioctl,
        #endif
        #if defined(CONFIG_COMPAT)
        .compat_ioctl = _Compat_MDrv_PM_io_ioctlOCtl,
        #endif
    },
};

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

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
int _MDrv_PM_SetCodeDramAddr(struct file *filp, unsigned long arg)
{
    PM_DRAM_INFO stPM_temp;
    if(copy_from_user(&stPM_temp, (PM_DRAM_INFO __user *)arg, sizeof(PM_DRAM_INFO)))
    {
        return EFAULT;
    }
    MDrv_SetDram(stPM_temp.Address, stPM_temp.Size);

    printk("\033[32m [%s, %s, %d] address: %x size: %x \033[0m \n", __FILE__, __FUNCTION__, __LINE__, stPM_temp.Address, stPM_temp.Size);
    return 0;
}

//-------------------------------------------------------------------------------------------------
int _MDrv_PM_SetDataDramAddr(struct file *filp, unsigned long arg)
{
    PM_DRAM_INFO stPM_temp;
    if(copy_from_user(&stPM_temp, (PM_DRAM_INFO __user *)arg, sizeof(PM_DRAM_INFO)))
    {
        return EFAULT;
    }
    MDrv_SetData(stPM_temp.Address, stPM_temp.Size);  
    printk("\033[32m [%s, %s, %d] address: %x size: %x \033[0m \n", __FILE__, __FUNCTION__, __LINE__, stPM_temp.Address, stPM_temp.Size);
    return 0;
}


//-------------------------------------------------------------------------------------------------
// IOCtrl Driver interface functions
//-------------------------------------------------------------------------------------------------
int _MDrv_PM_io_open(struct inode *inode, struct file *filp)
{    
    printk("Inside open \n");
    return 0;
}

//-------------------------------------------------------------------------------------------------
static int _MDrv_PM_io_release(struct inode *inode, struct file *filp)
{
    printk("Inside close \n");
    return 0;
}

//-------------------------------------------------------------------------------------------------
#ifdef HAVE_UNLOCKED_IOCTL
long _MDrv_PM_io_ioctl(struct file *filp, U32 u32Cmd, unsigned long u32Arg)
#else
int _MDrv_PM_io_ioctl(struct inode *inode, struct file *filp, unsigned long u32Cmd, unsigned long u32Arg)
#endif
{
    int retval;
    switch (u32Cmd)
    {
        case IOCTL_PM_SET_DRAM_CODE_ADDRESS:
            retval = _MDrv_PM_SetCodeDramAddr(filp, u32Arg);
            break;

        case IOCTL_PM_SET_DRAM_DATA_ADDRESS:
            retval = _MDrv_PM_SetDataDramAddr(filp, u32Arg);
            
        default:
            return -ENOTTY;

    }

    return 0;
}
//-------------------------------------------------------------------------------------------------
#if defined(CONFIG_COMPAT)
static long _Compat_MDrv_PM_io_ioctlOCtl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    printk("\033[32m [%s, %s, %d] \033[0m \n", __FILE__, __FUNCTION__, __LINE__);
    switch (cmd)
    {
        case IOCTL_PM_SET_DRAM_CODE_ADDRESS:
        case IOCTL_PM_SET_DRAM_DATA_ADDRESS:
            return filp->f_op->unlocked_ioctl(filp, cmd,(unsigned long)compat_ptr(arg));            
        default:
            return -ENOTTY;
    }

    return 0;
}
#endif




//-------------------------------------------------------------------------------------------------
static MSTAR_PM_DEV _st_pmdev={0};
static int _mstar_drv_pm_suspend(struct platform_device *dev, pm_message_t state)
{
    return 0;
}

//-------------------------------------------------------------------------------------------------
static int _mstar_drv_pm_resume(struct platform_device *dev)
{
    return 0;
}

//-------------------------------------------------------------------------------------------------
static int _mstar_drv_pm_probe(struct platform_device *pdev)
{
    pdev->dev.platform_data=&_st_pmdev;
	return 0;
}

//-------------------------------------------------------------------------------------------------
static int _mstar_drv_pm_remove(struct platform_device *pdev)
{
    pdev->dev.platform_data=NULL;
    return 0;
}

//-------------------------------------------------------------------------------------------------
static struct of_device_id mstar_pm_of_device_ids[] = {
         {.compatible = "mstar-pm"},
         {},
};

//-------------------------------------------------------------------------------------------------
static struct platform_driver Mstar_pm_driver = {
	.probe 		= _mstar_drv_pm_probe,
	.remove 	= _mstar_drv_pm_remove,
    .suspend    = _mstar_drv_pm_suspend,
    .resume     = _mstar_drv_pm_resume,

	.driver = {
#if defined(CONFIG_ARM64)
    .of_match_table = mstar_pm_of_device_ids,
#endif
		.name	= "Mstar-pm",
        .owner  = THIS_MODULE,
	}
};

//-------------------------------------------------------------------------------------------------
// Module functions
//-------------------------------------------------------------------------------------------------
MSYSTEM_STATIC int _MDrv_PMIO_ModuleInit(void)
{
    int s32Ret;
    dev_t  dev;

    if(_PMDev.s32Major)
    {
        dev = MKDEV(_PMDev.s32Major, _PMDev.s32Minor);
        s32Ret = register_chrdev_region(dev, MDRV_PM_DEVICE_COUNT, MDRV_PM_NAME);
    }
    else
    {
        s32Ret = alloc_chrdev_region(&dev, _PMDev.s32Minor, MDRV_PM_DEVICE_COUNT, MDRV_PM_NAME);
        _PMDev.s32Major = MAJOR(dev);
    }

    if (0 > s32Ret)
    {
        printk("Unable to get major %d\n", _PMDev.s32Major);
        return s32Ret;
    }

    cdev_init(&_PMDev.cdev, &_PMDev.fops);
    if (0 != (s32Ret= cdev_add(&_PMDev.cdev, dev, MDRV_PM_DEVICE_COUNT)))
    {
        printk("Unable add a character device\n");
        unregister_chrdev_region(dev, MDRV_PM_DEVICE_COUNT);
        return s32Ret;
    }

    /* initial the whole MBX Driver */
/*
    if(E_PM_OK != MDrv_MBX_Startup())
    {
        MBXIO_KDBG("Startup MBX Driver Failed! %d\n", _devMBX.s32Major);
        cdev_del(&_devMBX.cdev);
        unregister_chrdev_region(dev, MDRV_MBX_DEVICE_COUNT);
	 return -ENOMEM;
    }

    DRV_MBX_LockIOCTL_Init();
*/	
    platform_driver_register(&Mstar_pm_driver);
    return 0;
}


MSYSTEM_STATIC void _MDrv_PMIO_ModuleExit(void)
{
    /*de-initial the who MBX Driver */
 /*   MDrv_MBX_Exit(); */

    cdev_del(&_PMDev.cdev);
    unregister_chrdev_region(MKDEV(_PMDev.s32Major, _PMDev.s32Minor), MDRV_PM_DEVICE_COUNT);
    platform_driver_unregister(&Mstar_pm_driver);
}

#if defined(CONFIG_MSTAR_MSYSTEM) || defined(CONFIG_MSTAR_MSYSTEM_MODULE)
#else//#if defined(CONFIG_MSTAR_MSYSTEM) || defined(CONFIG_MSTAR_MSYSTEM_MODULE)
module_init(_MDrv_PMIO_ModuleInit);
module_exit(_MDrv_PMIO_ModuleExit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("PM ioctrl driver");
MODULE_LICENSE("GPL");
#endif


