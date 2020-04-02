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
* @file    mdrv_mipi_dsi_io.c
* @version
*
*/

#define __MDRV_MIPI_DSI_IO_C__


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
#include <linux/delay.h>
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
#include "mdrv_mipi_dsi_io_st.h"
#include "mdrv_mipi_dsi_io.h"
#include "mdrv_mipi_dsi.h"
#include "drv_mipi_dsi.h"
// Workaround before MV5 MIPI TX DSI driver layer is ready
#include "hal_mipi_dsi.h"
// MIPI TX DSI bug: must trigger TTL to keep MIPI TX DSI working
#include "mdrv_pnl_ioc_st.h"

#define MDRV_MIPI_DSI_DEVICE_COUNT   1
#define MDRV_MIPI_DSI_NAME           "ms_mipi_dsi"
#define MAX_FILE_HANDLE_SUPPRT       1
#define MDRV_NAME_MIPI_DSI           "ms_mipi_dsi"
#define MDRV_MAJOR_MIPI_DSI          0xea
#define MDRV_MINOR_MIPI_DSI          0x1a

extern void DrvLCDSWRest(void);
extern void DrvLCDOpenClk( LCD_TOP_CLK_SEL eCLK_SEL);
extern void DrvLCDSetRGB_CLK(u32 Frequency);
extern void DrvLCDSetOutPanelType(LCD_TYPE ubDevType);

int mstar_mipi_dsi_drv_open(struct inode *inode, struct file *filp);
int mstar_mipi_dsi_drv_release(struct inode *inode, struct file *filp);
long mstar_mipi_dsi_drv_ioctl(struct file *filp, unsigned int u32Cmd, unsigned long u32Arg);
static int mstar_mipi_dsi_drv_probe(struct platform_device *pdev);
static int mstar_mipi_dsi_drv_remove(struct platform_device *pdev);
static int mstar_mipi_dsi_drv_suspend(struct platform_device *dev, pm_message_t state);
static int mstar_mipi_dsi_drv_resume(struct platform_device *dev);
static void mstar_mipi_dsi_drv_platfrom_release(struct device *device);

//-------------------------------------------------------------------------------------------------

typedef struct
{
    int s32Major;
    int s32Minor;
    int refCnt;
    struct cdev cdev;
    struct file_operations fops;
} MIPI_DSI_DEV;

static MIPI_DSI_DEV _devMIPIDSI =
{
    .s32Major = MDRV_MAJOR_MIPI_DSI,
    .s32Minor = MDRV_MINOR_MIPI_DSI,
    .refCnt = 0,
    .cdev =
    {
        .kobj = {.name = MDRV_NAME_MIPI_DSI, },
        .owner = THIS_MODULE,
    },
    .fops =
    {
        .open = mstar_mipi_dsi_drv_open,
        .release = mstar_mipi_dsi_drv_release,
        .unlocked_ioctl = mstar_mipi_dsi_drv_ioctl,
    }
};

static struct class * mipi_dsi_class;
static char * mipi_dsi_classname = "ms_mipi_dsi_class";

static struct platform_driver Mstar_mipi_dsi_driver =
{
    .probe 		= mstar_mipi_dsi_drv_probe,
    .remove 	= mstar_mipi_dsi_drv_remove,
    .suspend    = mstar_mipi_dsi_drv_suspend,
    .resume     = mstar_mipi_dsi_drv_resume,

    .driver = {
        .name	= MDRV_NAME_MIPI_DSI,
        .owner  = THIS_MODULE,
    }
};

static u64 sg_mstar_device_mipi_dsi_dmamask = 0xffffffffUL;

static struct platform_device sg_mdrv_mipi_dsi_device =
{
    .name = MDRV_NAME_MIPI_DSI,
    .id = 0,
    .dev =
    {
        .release = mstar_mipi_dsi_drv_platfrom_release,
        .dma_mask = &sg_mstar_device_mipi_dsi_dmamask,
        .coherent_dma_mask = 0xffffffffUL
    }
};
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
// IOCtrl Driver interface functions
//-------------------------------------------------------------------------------------------------
int _MDrv_MIPIDSIIO_IOC_RESET(struct file *filp, unsigned long arg)
{
    MDRVMIPIDSIDBG("%s\n", __FUNCTION__);

    DrvLCDSWRest();

    return 0;
}

int _MDrv_MIPIDSIIO_IOC_Set_Config(struct file *filp, unsigned long arg)
{
    s32 nRet = 0;
    MipiDsiConfig_t tMipiDsiCfg;
    u8 *pPacketData=NULL;
    s32 nPacketIdx;
    u8 nCmd, nCnt;

    MDRVMIPIDSIDBG("%s\n", __FUNCTION__);

    if(copy_from_user(&tMipiDsiCfg, (MipiDsiConfig_t __user *)arg, sizeof(MipiDsiConfig_t)))
    {
        MDRVMIPIDSIPRINT("%s, copy_from_user fail!\n", __FUNCTION__);
        return -EFAULT;
    }

    // Workaround before MV5 MIPI TX DSI driver layer is ready
    Hal_DSI_DHY_Init();
    Hal_DSI_PadOutSel(tMipiDsiCfg.tLcmParams.LANE_NUM);
    Hal_DSI_SetLaneNum(tMipiDsiCfg.tLcmParams.LANE_NUM);
    Hal_DSI_Switch_Mode(DSI_CMD_MODE);
    Hal_DSI_clk_HS_mode(1);

    // Send DSI packets
    if (tMipiDsiCfg.nPacketDataLen > 0)
    {
        if ((pPacketData = vmalloc(tMipiDsiCfg.nPacketDataLen)) != NULL)
        {
            if(copy_from_user(pPacketData, (u8 __user *)tMipiDsiCfg.nPacketDataAddr, tMipiDsiCfg.nPacketDataLen))
            {
                MDRVMIPIDSIPRINT("%s, copy_from_user fail!(packet)\n", __FUNCTION__);
                return -EFAULT;
            }

            nPacketIdx = 0;
            while (nPacketIdx < (tMipiDsiCfg.nPacketDataLen-1))
            {
                nCmd = pPacketData[nPacketIdx];
                nCnt = pPacketData[nPacketIdx+1];

                if (nCmd == FLAG_END_OF_TABLE && nCnt == FLAG_END_OF_TABLE)
                {
                    break;
                }
                else if (nCmd == FLAG_DELAY && nCnt == FLAG_DELAY)
                {
                    udelay(pPacketData[nPacketIdx+2]);
                    nPacketIdx += 3;
                }
                else
                {
                    if (nCnt <= 1)
                        Hal_DSI_WriteShortPacket(nCnt+1, nCmd, &pPacketData[nPacketIdx+2]);
                    else
                        Hal_DSI_WriteLongPacket(nCnt+1, nCmd, &pPacketData[nPacketIdx+2]);

                    if (nCnt == 0)
                        nPacketIdx += 3;
                    else
                        nPacketIdx += (nCnt + 2);
                }
            }

            vfree(pPacketData);
        }
        else
            MDRVMIPIDSIPRINT("%s, vmalloc fail!\n", __FUNCTION__);
    }

    Hal_DSI_PHY_TIMCONFIG(&tMipiDsiCfg.tLcmParams);
    Hal_DSI_Config_VDO_Timing(&tMipiDsiCfg.tLcmParams);
    Hal_DSI_Switch_Mode(tMipiDsiCfg.tLcmParams.vdo_mode);

    DrvLCDSWRest();
    DrvLCDOpenClk(LCD_TOP_CLK_MPLL_216M);
    DrvLCDSetRGB_CLK(54000000);
    DrvLCDSetOutPanelType(LCD_TYPE_MIPIDSI);
#if 0
    DrvLCDInit(tPnlCfg.tPnlAttr);

    switch(tPnlCfg.tPnlAttr.ubDevType)
    {
        case LCD_TYPE_PLCD:
            DrvLCDConfigPLCD(tPnlCfg.tPLcdAttr);
            break;
        case LCD_TYPE_RGBLCD:
            DrvLCDConfigRGB_Paras(tPnlCfg.tRgbLcdAttr);
            break;
        default:
            MDRVMIPIDSIPRINT("Unsupport panel type : %d\n", tPnlCfg.tPnlAttr.ubDevType);
            nRet = -EINVAL;
            break;
    }
#endif
    return nRet;
}
#if 0
int _MDrv_MIPIDSIIO_IOC_Get_Config(struct file *filp, unsigned long arg)
{
    MDRVMIPIDSIDBG("%s\n", __FUNCTION__);
}
#endif
int _MDrv_MIPIDSIIO_IOC_ENABLE(struct file *filp, unsigned long arg)
{
    MDRVMIPIDSIDBG("%s\n", __FUNCTION__);
    // TBD
    return 0;
}

int _MDrv_MIPIDSIIO_IOC_READ_REG(struct file *filp, unsigned long arg)
{
    int nRet = 0;
    MipiDsiReg_t tMipiDsiReg;

    MDRVMIPIDSIDBG("%s\n", __FUNCTION__);

    if(copy_from_user(&tMipiDsiReg, (MipiDsiReg_t __user *)arg, sizeof(MipiDsiReg_t)))
    {
        MDRVMIPIDSIPRINT("%s, copy_from_user fail!\n", __FUNCTION__);
        return -EFAULT;
    }

    MDRVMIPIDSIPRINT("address = 0x%x\n", tMipiDsiReg.nRegAddr);

    // Workaround before MV5 MIPI TX DSI driver layer is ready
    tMipiDsiReg.nRegVal = Hal_MIPI_DSI_RegisterRead(tMipiDsiReg.nRegAddr);

    if(copy_to_user((MipiDsiReg_t __user *)arg, &tMipiDsiReg, sizeof(MipiDsiReg_t)))
    {
        MDRVMIPIDSIPRINT("%s, copy_to_user fail!\n", __FUNCTION__);
        return -EFAULT;
    }

    return nRet;
}

int _MDrv_MIPIDSIIO_IOC_WRITE_REG(struct file *filp, unsigned long arg)
{
    MipiDsiReg_t tMipiDsiReg;

    MDRVMIPIDSIDBG("%s\n", __FUNCTION__);

    if(copy_from_user(&tMipiDsiReg, (MipiDsiReg_t __user *)arg, sizeof(MipiDsiReg_t)))
    {
        MDRVMIPIDSIPRINT("%s, copy_from_user fail!\n", __FUNCTION__);
        return -EFAULT;
    }

    // Workaround before MV5 MIPI TX DSI driver layer is ready
    Hal_MIPI_DSI_RegisterWrite(tMipiDsiReg.nRegAddr, tMipiDsiReg.nRegVal);

    return 0;
}

int _MDrv_MIPIDSIIO_IOC_WRITE_REG_BIT(struct file *filp, unsigned long arg)
{
    MipiDsiReg_t tMipiDsiReg;

    MDRVMIPIDSIDBG("%s\n", __FUNCTION__);

    if(copy_from_user(&tMipiDsiReg, (MipiDsiReg_t __user *)arg, sizeof(MipiDsiReg_t)))
    {
        MDRVMIPIDSIPRINT("%s, copy_from_user fail!\n", __FUNCTION__);
        return -EFAULT;
    }

    // Workaround before MV5 MIPI TX DSI driver layer is ready
    Hal_MIPI_DSI_RegisterWriteBit(tMipiDsiReg.nRegAddr, tMipiDsiReg.nRegVal, tMipiDsiReg.nRegMask);

    return 0;
}


//----------------------------------------------------------------------------------------------

//==============================================================================
long mstar_mipi_dsi_drv_ioctl(struct file *filp, unsigned int u32Cmd, unsigned long u32Arg)
{
    int nErr = 0;
    int nRetval = 0;

    if(_devMIPIDSI.refCnt <= 0)
    {
        MDRVMIPIDSIPRINT("IOCTL refCnt =%d!!! \n", _devMIPIDSI.refCnt);
        return -EFAULT;
    }
    /* check u32Cmd valid */
    if(IOCTL_MIPI_DSI_MAGIC == _IOC_TYPE(u32Cmd))
    {
        if(_IOC_NR(u32Cmd) >= IOCTL_MIPI_DSI_MAX_NR)
        {
            MDRVMIPIDSIPRINT("IOCtl NR Error!!! (Cmd=%x)\n", u32Cmd);
            return -ENOTTY;
        }
    }
    else
    {
        MDRVMIPIDSIPRINT("IOCtl MAGIC Error!!! (Cmd=%x)\n", u32Cmd);
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

    MDRVMIPIDSIDBG("PNLIO_IOCTL %x!!!\n", u32Cmd);

    switch(u32Cmd)
    {
        case IOCTL_MIPI_DSI_RESET:
            nRetval = _MDrv_MIPIDSIIO_IOC_RESET(filp, u32Arg);
            break;
        case IOCTL_MIPI_DSI_SET_CONFIG:
            nRetval = _MDrv_MIPIDSIIO_IOC_Set_Config(filp, u32Arg);
            break;
#if 0
        case IOCTL_MIPI_DSI_GET_CONFIG:
            retval = _MDrv_MIPIDSIIO_IOC_Get_PNL_Config(filp, u32Arg);
            break;
#endif
        case IOCTL_MIPI_DSI_ENABLE:
            nRetval = _MDrv_MIPIDSIIO_IOC_ENABLE(filp, u32Arg);
            break;
        case IOCTL_MIPI_DSI_READ_REG: /* test function */
            nRetval = _MDrv_MIPIDSIIO_IOC_READ_REG(filp, u32Arg);
            break;
        case IOCTL_MIPI_DSI_WRITE_REG: /* test function */
            nRetval = _MDrv_MIPIDSIIO_IOC_WRITE_REG(filp, u32Arg);
            break;
        case IOCTL_MIPI_DSI_WRITE_REG_BIT: /* test function */
            nRetval = _MDrv_MIPIDSIIO_IOC_WRITE_REG_BIT(filp, u32Arg);
            break;
        default:  /* redundant, as cmd was checked against MAXNR */
            MDRVMIPIDSIPRINT("ERROR IOCtl number %x\n ", u32Cmd);
            nRetval = -ENOTTY;
            break;
    }

    return nRetval;
}


static int mstar_mipi_dsi_drv_probe(struct platform_device *pdev)
{
    MDRVMIPIDSIDBG("%s\n", __FUNCTION__);

    return 0;
}

static int mstar_mipi_dsi_drv_remove(struct platform_device *pdev)
{
    MDRVMIPIDSIDBG("%s\n", __FUNCTION__);

    return 0;
}


static int mstar_mipi_dsi_drv_suspend(struct platform_device *dev, pm_message_t state)
{
    MDRVMIPIDSIDBG("%s\n", __FUNCTION__);

    return 0;
}


static int mstar_mipi_dsi_drv_resume(struct platform_device *dev)
{
    MDRVMIPIDSIDBG("%s\n", __FUNCTION__);

    return 0;
}

static void mstar_mipi_dsi_drv_platfrom_release(struct device *device)
{
    MDRVMIPIDSIDBG("%s\n", __FUNCTION__);
}


int mstar_mipi_dsi_drv_open(struct inode *inode, struct file *filp)
{
    MDRVMIPIDSIDBG("%s\n", __FUNCTION__);

    if(_devMIPIDSI.refCnt >= MAX_FILE_HANDLE_SUPPRT)
    {
        MDRVMIPIDSIPRINT("mstar pnl driver reach max handled file number(%d)\n", MAX_FILE_HANDLE_SUPPRT);
        return -EMFILE;
    }

    _devMIPIDSI.refCnt++;

    return 0;
}


int mstar_mipi_dsi_drv_release(struct inode *inode, struct file *filp)
{
    MDRVMIPIDSIDBG("%s\n", __FUNCTION__);

    if(_devMIPIDSI.refCnt < 1)
    {
        MDRVMIPIDSIPRINT("mstar pnl driver have no opened file\n");
        return -EPERM;
    }

    _devMIPIDSI.refCnt--;

    return 0;
}


//-------------------------------------------------------------------------------------------------
// Module functions
//-------------------------------------------------------------------------------------------------
int _MDrv_MIPIDSIIO_ModuleInit(void)
{
    s32 nRet = 0;
    dev_t tDev;

    MDRVMIPIDSIPRINT("_Init \n");

    if(_devMIPIDSI.s32Major)
    {
        tDev = MKDEV(_devMIPIDSI.s32Major, _devMIPIDSI.s32Minor);
        nRet = register_chrdev_region(tDev, MDRV_MIPI_DSI_DEVICE_COUNT, MDRV_MIPI_DSI_NAME);
    }
    else
    {
        nRet = alloc_chrdev_region(&tDev, _devMIPIDSI.s32Minor, MDRV_MIPI_DSI_DEVICE_COUNT, MDRV_MIPI_DSI_NAME);
        _devMIPIDSI.s32Major = MAJOR(tDev);
    }

    if(0 > nRet)
    {
        MDRVMIPIDSIPRINT("Unable to get major %d\n", _devMIPIDSI.s32Major);
        return nRet;
    }

    cdev_init(&_devMIPIDSI.cdev, &_devMIPIDSI.fops);
    if(0 != (nRet = cdev_add(&_devMIPIDSI.cdev, tDev, MDRV_MIPI_DSI_DEVICE_COUNT)))
    {
        MDRVMIPIDSIPRINT("Unable add a character device\n");
        unregister_chrdev_region(tDev, MDRV_MIPI_DSI_DEVICE_COUNT);
        return nRet;
    }

    mipi_dsi_class = class_create(THIS_MODULE, mipi_dsi_classname);
    if(IS_ERR(mipi_dsi_class))
    {
        MDRVMIPIDSIPRINT("Failed at class_create().Please exec [mknod] before operate the device/n");
    }
    else
    {
        device_create(mipi_dsi_class, NULL, tDev, NULL, MDRV_NAME_MIPI_DSI);
    }
    /* initial the whole PNL Driver */
    nRet = platform_driver_register(&Mstar_mipi_dsi_driver);

    if(!nRet)
    {
        nRet = platform_device_register(&sg_mdrv_mipi_dsi_device);
        if(nRet)     /* if register device fail, then unregister the driver.*/
        {
            platform_driver_unregister(&Mstar_mipi_dsi_driver);
            MDRVMIPIDSIPRINT("register failed\n");

        }
        else
        {
            MDRVMIPIDSIPRINT("register success\n");
        }
    }

    return nRet;
}


void _MDrv_MIPIDSIIO_ModuleExit(void)
{
    /*de-initial the who MIPI TX DSI Driver */
    MDRVMIPIDSIPRINT("_Exit \n");
    cdev_del(&_devMIPIDSI.cdev);
    device_destroy(mipi_dsi_class, MKDEV(_devMIPIDSI.s32Major, _devMIPIDSI.s32Minor));
    class_destroy(mipi_dsi_class);
    unregister_chrdev_region(MKDEV(_devMIPIDSI.s32Major, _devMIPIDSI.s32Minor), MDRV_MIPI_DSI_DEVICE_COUNT);
    platform_device_unregister(&sg_mdrv_mipi_dsi_device);
    platform_driver_unregister(&Mstar_mipi_dsi_driver);
}


module_init(_MDrv_MIPIDSIIO_ModuleInit);
module_exit(_MDrv_MIPIDSIIO_ModuleExit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("MIPI TX DSI ioctrl driver");
MODULE_LICENSE("GPL");


