////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2017 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

/**
* @file    mdrv_pnl_io.c
* @version
*
*/

#define __MDRV_PNL_IO_C__


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
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <asm/string.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/poll.h>
#include <linux/types.h>
#include <linux/ioctl.h>

#include "cam_os_wrapper.h"
#include "mdrv_pnl_ioc_st.h"
#include "mdrv_pnl_ioc.h"
#include "mdrv_pnl.h"
#include "drv_pnl.h"

#define MDRV_PNL_DEVICE_COUNT   1
#define MDRV_PNL_NAME           "ms_pnl"
#define MAX_FILE_HANDLE_SUPPRT  1
#define MDRV_NAME_PNL           "ms_pnl"
#define MDRV_MAJOR_PNL          0xea
#define MDRV_MINOR_PNL          0x0a


int mstar_pnl_drv_open(struct inode *inode, struct file *filp);
int mstar_pnl_drv_release(struct inode *inode, struct file *filp);
long mstar_pnl_drv_ioctl(struct file *filp, unsigned int u32Cmd, unsigned long u32Arg);
static int mstar_pnl_drv_probe(struct platform_device *pdev);
static int mstar_pnl_drv_remove(struct platform_device *pdev);
static int mstar_pnl_drv_suspend(struct platform_device *dev, pm_message_t state);
static int mstar_pnl_drv_resume(struct platform_device *dev);
static void mstar_pnl_drv_platfrom_release(struct device *device);

//-------------------------------------------------------------------------------------------------

typedef struct
{
    int s32Major;
    int s32Minor;
    int refCnt;
    struct cdev cdev;
    struct file_operations fops;
} PNL_DEV;

static PNL_DEV _devPNL =
{
    .s32Major = MDRV_MAJOR_PNL,
    .s32Minor = MDRV_MINOR_PNL,
    .refCnt = 0,
    .cdev =
    {
        .kobj = {.name = MDRV_NAME_PNL, },
        .owner = THIS_MODULE,
    },
    .fops =
    {
        .open = mstar_pnl_drv_open,
        .release = mstar_pnl_drv_release,
        .unlocked_ioctl = mstar_pnl_drv_ioctl,
    }
};

static struct class * pnl_class;
static char * pnl_classname = "ms_pnl_class";

static struct platform_driver Mstar_pnl_driver =
{
    .probe 		= mstar_pnl_drv_probe,
    .remove 	= mstar_pnl_drv_remove,
    .suspend    = mstar_pnl_drv_suspend,
    .resume     = mstar_pnl_drv_resume,

    .driver = {
        .name	= MDRV_NAME_PNL,
        .owner  = THIS_MODULE,
    }
};

static u64 sg_mstar_device_pnl_dmamask = 0xffffffffUL;

static struct platform_device sg_mdrv_pnl_device =
{
    .name = MDRV_NAME_PNL,
    .id = 0,
    .dev =
    {
        .release = mstar_pnl_drv_platfrom_release,
        .dma_mask = &sg_mstar_device_pnl_dmamask,
        .coherent_dma_mask = 0xffffffffUL
    }
};
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
// IOCtrl Driver interface functions
//-------------------------------------------------------------------------------------------------
int _MDrv_PNLIO_IOC_RESET(struct file *filp, unsigned long arg)
{
    MDRVPNLDBG("%s\n", __FUNCTION__);

    DrvLCDSWRest();

    return 0;
}

int _MDrv_PNLIO_IOC_Set_Config(struct file *filp, unsigned long arg)
{
    s32 nRet = 0;
    PnlConfig_t tPnlCfg;

    MDRVPNLDBG("%s\n", __FUNCTION__);

    if(copy_from_user(&tPnlCfg, (PnlConfig_t __user *)arg, sizeof(PnlConfig_t)))
    {
        MDRVPNLPRINT("%s, copy_from_user fail!\n", __FUNCTION__);
        return -EFAULT;
    }

    DrvLCDSWRest();
    DrvLCDInit(tPnlCfg.tPnlAttr);
    DrvLCDSetRGB_CLK(28000000);

    switch(tPnlCfg.tPnlAttr.ubDevType)
    {
        case LCD_TYPE_PLCD:
            // all settings are done by DrvLCDInit()
            //DrvLCDConfigPLCD(tPnlCfg.tPLcdAttr);
            break;
        case LCD_TYPE_RGBLCD:
            DrvLCDConfigRGB_Paras(tPnlCfg.tRgbLcdAttr);
            break;
        case LCD_TYPE_PLCD_FLM:
            // all settings are done by DrvLCDInit()
            //DrvLCDConfigPLCD(tPnlCfg.tPLcdAttr);
            break;
        default:
            MDRVPNLPRINT("Unsupport panel type : %d\n", tPnlCfg.tPnlAttr.ubDevType);
            nRet = -EINVAL;
            break;
    }

    return nRet;
}
#if 0
int _MDrv_PNLIO_IOC_Get_Config(struct file *filp, unsigned long arg)
{
    MDRVPNLDBG("%s\n", __FUNCTION__);
}
#endif
int _MDrv_PNLIO_IOC_ENABLE(struct file *filp, unsigned long arg)
{
    MDRVPNLDBG("%s\n", __FUNCTION__);
    // TBD
    return 0;
}

int _MDrv_PNLIO_IOC_READ_REG(struct file *filp, unsigned long arg)
{
    int nRet = 0;
    PnlReg_t tPnlReg;

    MDRVPNLDBG("%s\n", __FUNCTION__);

    if(copy_from_user(&tPnlReg, (PnlReg_t __user *)arg, sizeof(PnlReg_t)))
    {
        MDRVPNLPRINT("%s, copy_from_user fail!\n", __FUNCTION__);
        return -EFAULT;
    }

    MDRVPNLPRINT("address = 0x%x\n", tPnlReg.nRegAddr);

    tPnlReg.nRegVal = DrvLCDRegisterRead(tPnlReg.nRegAddr);

    if(copy_to_user((PnlReg_t __user *)arg, &tPnlReg, sizeof(PnlReg_t)))
    {
        MDRVPNLPRINT("%s, copy_to_user fail!\n", __FUNCTION__);
        return -EFAULT;
    }

    return nRet;
}

int _MDrv_PNLIO_IOC_WRITE_REG(struct file *filp, unsigned long arg)
{
    PnlReg_t tPnlReg;

    MDRVPNLDBG("%s\n", __FUNCTION__);

    if(copy_from_user(&tPnlReg, (PnlReg_t __user *)arg, sizeof(PnlReg_t)))
    {
        MDRVPNLPRINT("%s, copy_from_user fail!\n", __FUNCTION__);
        return -EFAULT;
    }

    DrvLCDRegisterWrite(tPnlReg.nRegAddr, tPnlReg.nRegVal);

    return 0;
}

int _MDrv_PNLIO_IOC_WRITE_REG_BIT(struct file *filp, unsigned long arg)
{
    PnlReg_t tPnlReg;

    MDRVPNLDBG("%s\n", __FUNCTION__);

    if(copy_from_user(&tPnlReg, (PnlReg_t __user *)arg, sizeof(PnlReg_t)))
    {
        MDRVPNLPRINT("%s, copy_from_user fail!\n", __FUNCTION__);
        return -EFAULT;
    }

    DrvLCDRegisterWriteBit(tPnlReg.nRegAddr, tPnlReg.nRegVal, tPnlReg.nRegMask);

    return 0;
}


//----------------------------------------------------------------------------------------------

//==============================================================================
long mstar_pnl_drv_ioctl(struct file *filp, unsigned int u32Cmd, unsigned long u32Arg)
{
    int nErr = 0;
    int nRetval = 0;

    if(_devPNL.refCnt <= 0)
    {
        MDRVPNLPRINT("IOCTL refCnt =%d!!! \n", _devPNL.refCnt);
        return -EFAULT;
    }
    /* check u32Cmd valid */
    if(IOCTL_PNL_MAGIC == _IOC_TYPE(u32Cmd))
    {
        if(_IOC_NR(u32Cmd) >= IOCTL_PNL_MAX_NR)
        {
            MDRVPNLPRINT("IOCtl NR Error!!! (Cmd=%x)\n", u32Cmd);
            return -ENOTTY;
        }
    }
    else
    {
        MDRVPNLPRINT("IOCtl MAGIC Error!!! (Cmd=%x)\n", u32Cmd);
        return -ENOTTY;
    }

    /* verify Access */
    if(_IOC_DIR(u32Cmd) & _IOC_READ)
    {
        nErr = !access_ok(VERIFY_WRITE, (void __user *)u32Arg, _IOC_SIZE(u32Cmd));
    }
    else if(_IOC_DIR(u32Cmd) & _IOC_WRITE)
    {
        nErr =  !access_ok(VERIFY_READ, (void __user *)u32Arg, _IOC_SIZE(u32Cmd));
    }
    if(nErr)
    {
        return -EFAULT;
    }
    /* not allow query or command once driver suspend */

    MDRVPNLDBG("PNLIO_IOCTL %x!!!\n", u32Cmd);

    switch(u32Cmd)
    {
        case IOCTL_PNL_RESET:
            nRetval = _MDrv_PNLIO_IOC_RESET(filp, u32Arg);
            break;
        case IOCTL_PNL_SET_CONFIG:
            nRetval = _MDrv_PNLIO_IOC_Set_Config(filp, u32Arg);
            break;
#if 0
        case IOCTL_PNL_GET_CONFIG:
            retval = _MDrv_PNLIO_IOC_Get_Config(filp, u32Arg);
            break;
#endif
        case IOCTL_PNL_ENABLE:
            nRetval = _MDrv_PNLIO_IOC_ENABLE(filp, u32Arg);
            break;
        case IOCTL_PNL_READ_REG: /* test function */
            nRetval = _MDrv_PNLIO_IOC_READ_REG(filp, u32Arg);
            break;
        case IOCTL_PNL_WRITE_REG: /* test function */
            nRetval = _MDrv_PNLIO_IOC_WRITE_REG(filp, u32Arg);
            break;
        case IOCTL_PNL_WRITE_REG_BIT: /* test function */
            nRetval = _MDrv_PNLIO_IOC_WRITE_REG_BIT(filp, u32Arg);
            break;
        default:  /* redundant, as cmd was checked against MAXNR */
            MDRVPNLPRINT("ERROR IOCtl number %x\n ", u32Cmd);
            nRetval = -ENOTTY;
            break;
    }

    return nRetval;
}


static int mstar_pnl_drv_probe(struct platform_device *pdev)
{
    MDRVPNLDBG("%s\n", __FUNCTION__);

    return 0;
}

static int mstar_pnl_drv_remove(struct platform_device *pdev)
{
    MDRVPNLDBG("%s\n", __FUNCTION__);

    return 0;
}


static int mstar_pnl_drv_suspend(struct platform_device *dev, pm_message_t state)
{
    MDRVPNLDBG("%s\n", __FUNCTION__);

    return 0;
}


static int mstar_pnl_drv_resume(struct platform_device *dev)
{
    MDRVPNLDBG("%s\n", __FUNCTION__);

    return 0;
}

static void mstar_pnl_drv_platfrom_release(struct device *device)
{
    MDRVPNLDBG("%s\n", __FUNCTION__);
}


int mstar_pnl_drv_open(struct inode *inode, struct file *filp)
{
    MDRVPNLDBG("%s\n", __FUNCTION__);

    if(_devPNL.refCnt >= MAX_FILE_HANDLE_SUPPRT)
    {
        MDRVPNLPRINT("mstar pnl driver reach max handled file number(%d)\n", MAX_FILE_HANDLE_SUPPRT);
        return -EMFILE;
    }

    _devPNL.refCnt++;

    return 0;
}


int mstar_pnl_drv_release(struct inode *inode, struct file *filp)
{
    MDRVPNLDBG("%s\n", __FUNCTION__);

    if(_devPNL.refCnt < 1)
    {
        MDRVPNLPRINT("mstar pnl driver have no opened file\n");
        return -EPERM;
    }

    _devPNL.refCnt--;

    return 0;
}


//-------------------------------------------------------------------------------------------------
// Module functions
//-------------------------------------------------------------------------------------------------
int _MDrv_PNLIO_ModuleInit(void)
{
    s32 nRet = 0;
    dev_t tDev;

    MDRVPNLDBG("_Init \n");

    if(_devPNL.s32Major)
    {
        tDev = MKDEV(_devPNL.s32Major, _devPNL.s32Minor);
        nRet = register_chrdev_region(tDev, MDRV_PNL_DEVICE_COUNT, MDRV_PNL_NAME);
    }
    else
    {
        nRet = alloc_chrdev_region(&tDev, _devPNL.s32Minor, MDRV_PNL_DEVICE_COUNT, MDRV_PNL_NAME);
        _devPNL.s32Major = MAJOR(tDev);
    }

    if(0 > nRet)
    {
        MDRVPNLPRINT("Unable to get major %d\n", _devPNL.s32Major);
        return nRet;
    }

    cdev_init(&_devPNL.cdev, &_devPNL.fops);
    if(0 != (nRet = cdev_add(&_devPNL.cdev, tDev, MDRV_PNL_DEVICE_COUNT)))
    {
        MDRVPNLPRINT("Unable add a character device\n");
        unregister_chrdev_region(tDev, MDRV_PNL_DEVICE_COUNT);
        return nRet;
    }

    pnl_class = class_create(THIS_MODULE, pnl_classname);
    if(IS_ERR(pnl_class))
    {
        MDRVPNLPRINT("Failed at class_create().Please exec [mknod] before operate the device/n");
    }
    else
    {
        device_create(pnl_class, NULL, tDev, NULL, MDRV_NAME_PNL);
    }
    /* initial the whole PNL Driver */
    nRet = platform_driver_register(&Mstar_pnl_driver);

    if(!nRet)
    {
        nRet = platform_device_register(&sg_mdrv_pnl_device);
        if(nRet)     /* if register device fail, then unregister the driver.*/
        {
            platform_driver_unregister(&Mstar_pnl_driver);
            MDRVPNLPRINT("register failed\n");

        }
        else
        {
            MDRVPNLPRINT("register success\n");
        }
    }

    return nRet;
}


void _MDrv_PNLIO_ModuleExit(void)
{
    /*de-initial the who PNL Driver */
    MDRVPNLPRINT("_Exit \n");
    cdev_del(&_devPNL.cdev);
    device_destroy(pnl_class, MKDEV(_devPNL.s32Major, _devPNL.s32Minor));
    class_destroy(pnl_class);
    unregister_chrdev_region(MKDEV(_devPNL.s32Major, _devPNL.s32Minor), MDRV_PNL_DEVICE_COUNT);
    platform_device_unregister(&sg_mdrv_pnl_device);
    platform_driver_unregister(&Mstar_pnl_driver);
}


module_init(_MDrv_PNLIO_ModuleInit);
module_exit(_MDrv_PNLIO_ModuleExit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("PNL ioctrl driver");
MODULE_LICENSE("GPL");


