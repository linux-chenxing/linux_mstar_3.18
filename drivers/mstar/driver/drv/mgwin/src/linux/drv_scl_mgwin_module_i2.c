////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2011 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#include <linux/pfn.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>          /* seems do not need this */
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <asm/string.h>
#include <linux/clk.h>
#include <linux/clk-provider.h>

#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/poll.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/blkdev.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/wait.h>


#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include "ms_platform.h"
#include "ms_msys.h"

#include "drv_scl_os.h"
#include "drv_scl_mgwin_dbg.h"
#include "hal_scl_mgwin_util.h"

#include "drv_scl_mgwin_ioctl.h"
#include "drv_scl_mgwin_io_st.h"
#include "drv_scl_mgwin_io_wrapper.h"
//-------------------------------------------------------------------------------------------------
// Define & Macro
//-------------------------------------------------------------------------------------------------
#define DRV_SCLMGWIN_DEVICE_COUNT    1
#define DRV_SCLMGWIN_DEVICE_NAME     "msclmgwin"
#define DRV_SCLMGWIN_DEVICE_MAJOR    0xea
#define DRV_SCLMGWIN_DEVICE_MINOR    0x40

//-------------------------------------------------------------------------------------------------
// Prototype
//-------------------------------------------------------------------------------------------------

int DrvSclMgwinModuleOpen(struct inode *inode, struct file *filp);
int DrvSclMgwinModuleRelease(struct inode *inode, struct file *filp);
long DrvSclMgwinModuleIoctl(struct file *filp, unsigned int u32Cmd, unsigned long u32Arg);
static int DrvSclMgwinModuleProbe(struct platform_device *pdev);
static int DrvSclMgwinModuleRemove(struct platform_device *pdev);
static int DrvSclMgwinModuleSuspend(struct platform_device *dev, pm_message_t state);
static int DrvSclMgwinModuleResume(struct platform_device *dev);
static unsigned int DrvSclMgwinModulePoll(struct file *filp, struct poll_table_struct *wait);
extern int DrvSclMgwinIoctlParse(struct file *filp, unsigned int u32Cmd, unsigned long u32Arg);

//-------------------------------------------------------------------------------------------------
// Structure
//-------------------------------------------------------------------------------------------------
typedef struct
{
    int s32Major;
    int s32Minor;
    int refCnt;
    struct cdev cdev;
    struct file_operations fops;
    struct device *devicenode;
}DrvSclMgwinModuleDevice_t;

//-------------------------------------------------------------------------------------------------
// Variable
//-------------------------------------------------------------------------------------------------
static DrvSclMgwinModuleDevice_t _tSclMgwinDevice =
{
    .s32Major = DRV_SCLMGWIN_DEVICE_MAJOR,
    .s32Minor = DRV_SCLMGWIN_DEVICE_MINOR,
    .refCnt = 0,
    .cdev =
    {
        .kobj = {.name= DRV_SCLMGWIN_DEVICE_NAME, },
        .owner = THIS_MODULE,
    },
    .fops =
    {
        .open = DrvSclMgwinModuleOpen,
        .release = DrvSclMgwinModuleRelease,
        .unlocked_ioctl = DrvSclMgwinModuleIoctl,
        .poll = DrvSclMgwinModulePoll,
    },
};

static struct class * _tSclMgwinClass = NULL;
static char * SclHvspClassName = "m_sclmgwin_class";


static const struct of_device_id _SclMgwinMatchTable[] =
{
    { .compatible = "mstar,sclmgwin" },
    {}
};

static struct platform_driver stDrvSclMgwinPlatformDriver =
{
    .probe      = DrvSclMgwinModuleProbe,
    .remove     = DrvSclMgwinModuleRemove,
    .suspend    = DrvSclMgwinModuleSuspend,
    .resume     = DrvSclMgwinModuleResume,
    .driver =
    {
        .name   = DRV_SCLMGWIN_DEVICE_NAME,
        .owner  = THIS_MODULE,
        .of_match_table = of_match_ptr(_SclMgwinMatchTable),
    },
};

static u64 u64SclHvsp_DmaMask = 0xffffffffUL;

static struct platform_device stDrvSclMgwinPlatformDevice =
{
    .name = DRV_SCLMGWIN_DEVICE_NAME,
    .id = 0,
    .dev =
    {
        .dma_mask = &u64SclHvsp_DmaMask,
        .coherent_dma_mask = 0xffffffffUL
    }
};

//-------------------------------------------------------------------------------------------------
// internal function
//-------------------------------------------------------------------------------------------------



//==============================================================================
long DrvSclMgwinModuleIoctl(struct file *filp, unsigned int u32Cmd, unsigned long u32Arg)
{
    int err = 0;
    int retval = 0;

    if(_tSclMgwinDevice.refCnt <= 0)
    {
        SCL_ERR( "[SCLMGWIN] HVSP1IO_IOCTL refCnt =%d!!! \n", _tSclMgwinDevice.refCnt);
        return -EFAULT;
    }
    /* check u32Cmd valid */
    if(IOCTL_SCLMGWIN_MAGIC == _IOC_TYPE(u32Cmd))
    {
        if(_IOC_NR(u32Cmd) >= IOCTL_SCLMGWIN_MAX_NR)
        {
            SCL_ERR( "[SCLMGWIN] IOCtl NR Error!!! (Cmd=%x)\n",u32Cmd);
            return -ENOTTY;
        }
    }
    else
    {
        SCL_ERR( "[SCLMGWIN] IOCtl MAGIC Error!!! (Cmd=%x)\n",u32Cmd);
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
    /* not allow query or command once driver suspend */

    retval = DrvSclMgwinIoctlParse(filp, u32Cmd, u32Arg);
    return retval;
}


static unsigned int DrvSclMgwinModulePoll(struct file *filp, struct poll_table_struct *wait)
{
    return 0;
}


static int DrvSclMgwinModuleSuspend(struct platform_device *dev, pm_message_t state)
{
    return 0;
}

static int DrvSclMgwinModuleResume(struct platform_device *dev)
{
    return 0;
}


int DrvSclMgwinModuleOpen(struct inode *inode, struct file *filp)
{
    int ret = 0;
    SCL_DBG(SCL_DBG_LV_MDRV_IO(), "[SCLMGWIN] %s\n",__FUNCTION__);

    SCL_ASSERT(_tSclMgwinDevice.refCnt>=0);

    if(filp->private_data == NULL)
    {

        filp->private_data = DrvSclOsVirMemalloc(sizeof(s32));

        if(filp->private_data == NULL)
        {
            SCL_ERR("[SCLMGWIN] %s %d, allocate memory fail\n", __FUNCTION__, __LINE__);
            ret = -EFAULT;
        }
        else
        {
            s32 s32Handler = _DrvSclMgwinIoOpen();

            if(s32Handler != -1)
            {
                *((s32 *)filp->private_data) = s32Handler;
            }
            else
            {
                SCL_ERR("[HVSP1] %s %d, handler error fail\n", __FUNCTION__, __LINE__);
                ret = -EFAULT;
                DrvSclOsVirMemFree(filp->private_data);
            }
        }
    }
    if(!ret)
    {
        _tSclMgwinDevice.refCnt++;
    }

    return ret;
}


int DrvSclMgwinModuleRelease(struct inode *inode, struct file *filp)
{
    int ret = 0;
    s32 s32Handler;

    SCL_DBG(SCL_DBG_LV_MDRV_IO(), "[SCLMGWIN] %s\n",__FUNCTION__);

    s32Handler = *((s32 *)filp->private_data);

    if(_DrvSclMgwinIoRelease(s32Handler) == E_DRV_SCLMGWIN_IO_ERR_OK)
    {
        DrvSclOsVirMemFree(filp->private_data);
        filp->private_data = NULL;
    }
    else
    {
        ret = -EFAULT;
        SCL_ERR("[SCLMGWIN] Release Fail\n");
    }

    _tSclMgwinDevice.refCnt--;
    SCL_ASSERT(_tSclMgwinDevice.refCnt>=0);
    return ret;
}


#if CONFIG_OF
static int DrvSclMgwinModuleProbe(struct platform_device *pdev)
{
    int s32Ret;
    dev_t  dev;
    DrvSclOsClkConfig_t stClkCfg;

    //struct resource *res_irq;
    //struct device_node *np;
    SCL_DBG(SCL_DBG_LV_MDRV_IO(), "[SCLMGWIN] %s:%d\n",__FUNCTION__,__LINE__);

    if(_tSclMgwinDevice.s32Major)
    {
        dev     = MKDEV(_tSclMgwinDevice.s32Major, _tSclMgwinDevice.s32Minor);
        s32Ret  = register_chrdev_region(dev, DRV_SCLMGWIN_DEVICE_COUNT, DRV_SCLMGWIN_DEVICE_NAME);
    }
    else
    {
        s32Ret                  = alloc_chrdev_region(&dev, _tSclMgwinDevice.s32Minor, DRV_SCLMGWIN_DEVICE_COUNT, DRV_SCLMGWIN_DEVICE_NAME);
        _tSclMgwinDevice.s32Major  = MAJOR(dev);
    }

    if (0 > s32Ret)
    {
        SCL_ERR( "[SCLMGWIN] Unable to get major %d\n", _tSclMgwinDevice.s32Major);
        return s32Ret;
    }

    cdev_init(&_tSclMgwinDevice.cdev, &_tSclMgwinDevice.fops);
    if (0 != (s32Ret= cdev_add(&_tSclMgwinDevice.cdev, dev, DRV_SCLMGWIN_DEVICE_COUNT)))
    {
        SCL_ERR( "[SCLMGWIN] Unable add a character device\n");
        unregister_chrdev_region(dev, DRV_SCLMGWIN_DEVICE_COUNT);
        return s32Ret;
    }
    _tSclMgwinClass = msys_get_sysfs_class();
    if(!_tSclMgwinClass)
    {
        _tSclMgwinClass = class_create(THIS_MODULE, SclHvspClassName);
    }
    if(IS_ERR(_tSclMgwinClass))
    {
        printk(KERN_WARNING"Failed at class_create().Please exec [mknod] before operate the device/n");
    }
    else
    {
        _tSclMgwinDevice.devicenode = device_create(_tSclMgwinClass, NULL, dev,NULL, DRV_SCLMGWIN_DEVICE_NAME);
        _tSclMgwinDevice.devicenode->dma_mask=&u64SclHvsp_DmaMask;
        _tSclMgwinDevice.devicenode->coherent_dma_mask=u64SclHvsp_DmaMask;
    }

    //probe
    SCL_DBG(SCL_DBG_LV_MDRV_IO(), "[SCLMGWIN] %s\n",__FUNCTION__);
    stDrvSclMgwinPlatformDevice.dev.of_node = pdev->dev.of_node;


    //clk enable
    stClkCfg.ptIdclk = DrvSclOsClkGetClk((void *)stDrvSclMgwinPlatformDevice.dev.of_node,0);
    stClkCfg.ptFclk1 = DrvSclOsClkGetClk((void *)stDrvSclMgwinPlatformDevice.dev.of_node,1);
    stClkCfg.ptFclk2 = DrvSclOsClkGetClk((void *)stDrvSclMgwinPlatformDevice.dev.of_node,2);
    stClkCfg.ptOdclk = DrvSclOsClkGetClk((void *)stDrvSclMgwinPlatformDevice.dev.of_node,3);
    //DrvSclOsClkSetConfig(E_DRV_SCLOS_CLK_ID_HVSP1, stClkCfg); ToDo Ryan


    _DrvSclMgwinIoInit();

    return 0;
}

static int DrvSclMgwinModuleRemove(struct platform_device *pdev)
{
    DrvSclOsClkConfig_t stClkCfg;
    SCL_DBG(SCL_DBG_LV_MDRV_IO(), "[SCLDMA_1] %s\n",__FUNCTION__);
    stClkCfg.ptIdclk = DrvSclOsClkGetClk((void *)stDrvSclMgwinPlatformDevice.dev.of_node,0);
    stClkCfg.ptFclk1 = DrvSclOsClkGetClk((void *)stDrvSclMgwinPlatformDevice.dev.of_node,1);
    stClkCfg.ptFclk2 = DrvSclOsClkGetClk((void *)stDrvSclMgwinPlatformDevice.dev.of_node,2);
    stClkCfg.ptOdclk = DrvSclOsClkGetClk((void *)stDrvSclMgwinPlatformDevice.dev.of_node,3);

    //ToDo
    //_DrvSclHvspIoMemFree();

    DrvSclOsClearProbeInformation(E_DRV_SCLOS_INIT_HVSP_1);
    if(DrvSclOsGetProbeInformation(E_DRV_SCLOS_INIT_ALL) == 0)
    {
    	//ToDo

    }
    else if(!DrvSclOsGetProbeInformation(E_DRV_SCLOS_INIT_HVSP))
    {
       //ToDo
    }
    cdev_del(&_tSclMgwinDevice.cdev);
    device_destroy(_tSclMgwinClass, MKDEV(_tSclMgwinDevice.s32Major, _tSclMgwinDevice.s32Minor));
    class_destroy(_tSclMgwinClass);
    unregister_chrdev_region(MKDEV(_tSclMgwinDevice.s32Major, _tSclMgwinDevice.s32Minor), DRV_SCLMGWIN_DEVICE_COUNT);
    return 0;
}
#else
static int DrvSclMgwinModuleProbe(struct platform_device *pdev)
{
    SCL_DBG(SCL_DBG_LV_MDRV_IO(), "[SCLMGWIN] %s\n",__FUNCTION__);

    return 0;
}
static int DrvSclMgwinModuleRemove(struct platform_device *pdev)
{
    SCL_DBG(SCL_DBG_LV_MDRV_IO(), "[SCLMGWIN] %s\n",__FUNCTION__);

    return 0;
}

#endif


//-------------------------------------------------------------------------------------------------
// Module functions
//-------------------------------------------------------------------------------------------------
#if CONFIG_OF
int _DrvSclMgwinModuleInit(void)
{
    int ret = 0;
    SCL_DBG(SCL_DBG_LV_MDRV_IO(), "[SCLMGWIN] %s:%d\n",__FUNCTION__,__LINE__);

    ret = platform_driver_register(&stDrvSclMgwinPlatformDriver);
    if (!ret)
    {
        SCL_DBG(SCL_DBG_LV_MDRV_IO(), "[SCLMGWIN] platform_driver_register success\n");
    }
    else
    {
        SCL_ERR( "[SCLMGWIN] platform_driver_register failed\n");
        platform_driver_unregister(&stDrvSclMgwinPlatformDriver);
    }


    return ret;
}
void _DrvSclMgwinModuleExit(void)
{
    /*de-initial the who GFLIPDriver */
    SCL_DBG(SCL_DBG_LV_MDRV_IO(), "[SCLMGWIN] %s\n",__FUNCTION__);
    platform_driver_unregister(&stDrvSclMgwinPlatformDriver);
}
#else

int _DrvSclMgwinModuleInit(void)
{
    int ret = 0;
    int s32Ret;
    dev_t  dev;
    //struct device_node *np;

    SCL_DBG(SCL_DBG_LV_MDRV_IO(), "[SCLMGWIN] %s\n",__FUNCTION__);
    //np = of_find_compatible_node(NULL, NULL, "mstar,hvsp1");
    //if (np)
    //{
    //  SCL_DBG(SCL_DBG_LV_MDRV_IO(), "Find scl dts node\n");
    //  stDrvSclMgwinPlatformDevice.dev.of_node = of_node_get(np);
    //  of_node_put(np);
    //}
    //else
    //{
    //    return -ENODEV;
    //}

    if(_tSclMgwinDevice.s32Major)
    {
        dev     = MKDEV(_tSclMgwinDevice.s32Major, _tSclMgwinDevice.s32Minor);
        s32Ret  = register_chrdev_region(dev, DRV_SCLMGWIN_DEVICE_COUNT, DRV_SCLMGWIN_DEVICE_NAME);
    }
    else
    {
        s32Ret                  = alloc_chrdev_region(&dev, _tSclMgwinDevice.s32Minor, DRV_SCLMGWIN_DEVICE_COUNT, DRV_SCLMGWIN_DEVICE_NAME);
        _tSclMgwinDevice.s32Major  = MAJOR(dev);
    }

    if (0 > s32Ret)
    {
        SCL_ERR( "[SCLMGWIN] Unable to get major %d\n", _tSclMgwinDevice.s32Major);
        return s32Ret;
    }

    cdev_init(&_tSclMgwinDevice.cdev, &_tSclMgwinDevice.fops);
    if (0 != (s32Ret= cdev_add(&_tSclMgwinDevice.cdev, dev, DRV_SCLMGWIN_DEVICE_COUNT)))
    {
        SCL_ERR( "[SCLMGWIN] Unable add a character device\n");
        unregister_chrdev_region(dev, DRV_SCLMGWIN_DEVICE_COUNT);
        return s32Ret;
    }

    _tSclMgwinClass = class_create(THIS_MODULE, SclHvspClassName);
    if(IS_ERR(_tSclMgwinClass))
    {
        printk(KERN_WARNING"Failed at class_create().Please exec [mknod] before operate the device/n");
    }
    else
    {
        device_create(_tSclMgwinClass, NULL, dev,NULL, DRV_SCLMGWIN_DEVICE_NAME);
    }

    ret = platform_driver_register(&stDrvSclMgwinPlatformDriver);

    if (!ret)
    {
        ret = platform_device_register(&stDrvSclMgwinPlatformDevice);
        if (ret)    // if register device fail, then unregister the driver.
        {
            platform_driver_unregister(&stDrvSclMgwinPlatformDriver);
            SCL_ERR( "[SCLMGWIN] platform_driver_register failed\n");

        }
        else
        {
            SCL_DBG(SCL_DBG_LV_MDRV_IO(), "[SCLMGWIN] platform_driver_register success\n");
        }
    }


    return ret;
}
void _DrvSclMgwinModuleExit(void)
{
    /*de-initial the who GFLIPDriver */
    SCL_DBG(SCL_DBG_LV_MDRV_IO(), "[SCLMGWIN] %s\n",__FUNCTION__);

    cdev_del(&_tSclMgwinDevice.cdev);
    device_destroy(_tSclMgwinClass, MKDEV(_tSclMgwinDevice.s32Major, _tSclMgwinDevice.s32Minor));
    class_destroy(_tSclMgwinClass);
    unregister_chrdev_region(MKDEV(_tSclMgwinDevice.s32Major, _tSclMgwinDevice.s32Minor), DRV_SCLMGWIN_DEVICE_COUNT);
    platform_driver_unregister(&stDrvSclMgwinPlatformDriver);
}

#endif

module_init(_DrvSclMgwinModuleInit);
module_exit(_DrvSclMgwinModuleExit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("mstar slchvsp ioctrl driver");
MODULE_LICENSE("GPL");
