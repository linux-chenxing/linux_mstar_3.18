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
/// file    mdrv_xc_io.c
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
#include <linux/poll.h>
#include <linux/version.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/irq.h>
#include <linux/platform_device.h>

//drver header files
#include "mst_devid.h"
#include "mdrv_mstypes.h"
#include "mhal_xc.h"
#include "mdrv_xc.h"
#include "chip_int.h"
#include <linux/vmalloc.h>
#include <linux/slab.h>
#if defined(CONFIG_COMPAT)
#include <linux/compat.h>
#endif
#include "mdrv_xc_st.h"
#include "mdrv_xc_io.h"
#include "mdrv_xc_menuload.h"
#include "mdrv_xc_dynamicscaling.h"
#include "mdrv_dlc.h"
#include "mdrv_xc_proc.h"
//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define     MDRV_XC_DEVICE_COUNT           1
#define     MDRV_XC_NAME                           "XC"

//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------
//#define XC_DEBUG_ENABLE
#ifdef      XC_DEBUG_ENABLE
#define     XC_KDBG(_fmt, _args...)        printk("[XC (IO)][%s:%05d] " _fmt, __FUNCTION__, __LINE__, ## _args)
#define     XC_ASSERT(_con)   do {\
                                                            if (!(_con)) {\
                                                                printk(KERN_CRIT "BUG at %s:%d assert(%s)\n",\
                                                                                                    __FILE__, __LINE__, #_con);\
                                                                BUG();\
                                                            }\
                                                          } while (0)
#else
#define     XC_KDBG(fmt, args...)
#define     XC_ASSERT(arg)
#endif

//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------

typedef struct
{
    int s32Major;
    int s32Minor;
    int refCnt;
    struct cdev cdev;
    struct file_operations fops;
} XC_DEV;

typedef struct
{
    int     id;
    bool        id_auto;
} platform_xc_device;

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// Forward declaration
//--------------------------------------------------------------------------------------------------
static int _MDrv_XC_Open (struct inode *inode, struct file *filp);
static ssize_t _MDrv_XC_Write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);
static ssize_t _MDrv_XC_Read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
static int _MDrv_XC_Release(struct inode *inode, struct file *filp);
static long _MDrv_XC_IOCtl(struct file *filp, unsigned int u32Cmd, unsigned long u32Arg);
#ifdef CONFIG_COMPAT
static long _Compat_MDrv_XC_IOCtl(struct file *filp, unsigned int u32Cmd, unsigned long u32Arg);
#endif

// STR
static int _MDrv_XC_Probe(struct platform_device *pdev);
static int _MDrv_XC_Remove(struct platform_device *pdev);
static int _MDrv_XC_Suspend(struct platform_device *dev, pm_message_t state);
static int _MDrv_XC_Resume(struct platform_device *dev);

#ifdef CONFIG_MSTAR_XC_HDR_SUPPORT
int _MDrv_XC_IOC_SetHDRType(struct file *filp, unsigned long arg);
int _MDrv_XC_IOC_GetHDRType(struct file *filp, unsigned long arg);
int _MDrv_XC_IOC_SetInputSourceType(struct file *filp, unsigned long arg);
int _MDrv_XC_IOC_GetInputSourceType(struct file *filp, unsigned long arg);
int _MDrv_XC_IOC_SetOpenMetadataInfo(struct file *filp, unsigned long arg);
int _MDrv_XC_IOC_Set3DLutInfo(struct file *filp, unsigned long arg);
int _MDrv_XC_IOC_SetShareMemInfo(struct file *filp, unsigned long arg);
#endif

MSYSTEM_STATIC int _MDrv_XC_ModuleInit(void);
MSYSTEM_STATIC void _MDrv_XC_ModuleExit(void);

MS_U32 _MDrv_XC_RegisterInterrupt(void);
MS_U32 _MDrv_XC_DeRegisterInterrupt(void);

EXPORT_SYMBOL(_MDrv_XC_RegisterInterrupt);
EXPORT_SYMBOL(_MDrv_XC_DeRegisterInterrupt);
//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static struct class *mxc_class;
static XC_DEV devXC =
{
    .s32Major = MDRV_MAJOR_SCALER,
    .s32Minor = MDRV_MINOR_SCALER,
    .refCnt = 0,

    .cdev =
    {
        .kobj = {.name= MDRV_NAME_SCALER, },
        .owner = THIS_MODULE,
    },
    .fops =
    {
        .open =                     _MDrv_XC_Open,
        .write =                    _MDrv_XC_Write,
        .read =                     _MDrv_XC_Read,
        .release =                  _MDrv_XC_Release,
        .unlocked_ioctl =           _MDrv_XC_IOCtl,
#ifdef CONFIG_COMPAT
        .compat_ioctl =             _Compat_MDrv_XC_IOCtl,
#endif
    }

};

static platform_xc_device _devXC =
{
    .id = 5,
    .id_auto = 0,
};

// STR
#if defined (CONFIG_ARM64)
static struct of_device_id mstarXC_of_device_ids[] =
{
    {.compatible = "mstar-scaler"},
    {},
};
#endif
static struct platform_driver Mstar_XC_driver =
{
    .probe      = _MDrv_XC_Probe,
    .remove     = _MDrv_XC_Remove,
    .suspend    = _MDrv_XC_Suspend,
    .resume     = _MDrv_XC_Resume,

    .driver = {
#if defined(CONFIG_ARM64)
        .of_match_table = mstarXC_of_device_ids,
#endif
        .name   = "Mstar-scaler",
        .owner  = THIS_MODULE,
    }
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
static int _MDrv_XC_Probe(struct platform_device *pdev)
{
    int retval=0;
    pdev->dev.platform_data=NULL;
    return retval;
}

static int _MDrv_XC_Remove(struct platform_device *pdev)
{
    pdev->dev.platform_data=NULL;
    return 0;
}
static int _MDrv_XC_Suspend(struct platform_device *dev, pm_message_t state)
{
    return MDrv_XC_Suspend();
}

static int _MDrv_XC_Resume(struct platform_device *dev)
{
    return MDrv_XC_Resume();
}

#ifdef CONFIG_MSTAR_XC_HDR_SUPPORT
int _MDrv_XC_IOC_SetHDRType(struct file *filp, unsigned long arg)
{
    EN_KDRV_XC_HDR_TYPE enHDRType = E_KDRV_XC_HDR_NONE;

    if(__get_user(enHDRType, (EN_KDRV_XC_HDR_TYPE __user *)arg))
    {
        return -EFAULT;
    }
    XC_KDBG("enHDRTYPE: %d\n", enHDRType);

    if (enHDRType >= E_KDRV_XC_HDR_MAX)
    {
        return -EINVAL;
    }

    if (MDrv_XC_SetHDRType(enHDRType) != TRUE)
    {
        return -EPERM;
    }

    return 0;
}

int _MDrv_XC_IOC_GetHDRType(struct file *filp, unsigned long arg)
{
    EN_KDRV_XC_HDR_TYPE enHDRType = E_KDRV_XC_HDR_NONE;

    if (MDrv_XC_GetHDRType(&enHDRType) == TRUE)
    {
        __put_user(enHDRType, (EN_KDRV_XC_HDR_TYPE __user *)arg);
    }
    else
    {
        return -EPERM;
    }

    return 0;
}

int _MDrv_XC_IOC_SetInputSourceType(struct file *filp, unsigned long arg)
{
    EN_KDRV_XC_INPUT_SOURCE_TYPE enInputSourceType =  E_KDRV_XC_INPUT_SOURCE_NONE;

    if(__get_user(enInputSourceType, (EN_KDRV_XC_INPUT_SOURCE_TYPE __user *)arg))
    {
        return -EFAULT;
    }
    XC_KDBG("enInputSourceType: %d\n", enInputSourceType);

    if (enInputSourceType >= E_KDRV_XC_INPUT_SOURCE_MAX)
    {
        return -EINVAL;
    }

    if (MDrv_XC_SetInputSourceType(enInputSourceType) != TRUE)
    {
        return -EPERM;
    }

    return 0;
}

int _MDrv_XC_IOC_GetInputSourceType(struct file *filp, unsigned long arg)
{
    EN_KDRV_XC_INPUT_SOURCE_TYPE enInputSourceType =  E_KDRV_XC_INPUT_SOURCE_NONE;

    if (MDrv_XC_GetInputSourceType(&enInputSourceType))
    {
        __put_user(enInputSourceType, (EN_KDRV_XC_INPUT_SOURCE_TYPE __user *)arg);
    }
    else
    {
        return -EPERM;
    }

    return 0;
}

int _MDrv_XC_IOC_SetOpenMetadataInfo(struct file *filp, unsigned long arg)
{
    return -ENOSYS;
}

int _MDrv_XC_IOC_Set3DLutInfo(struct file *filp, unsigned long arg)
{
    MS_U8* pu8Data;
    ST_KDRV_XC_3DLUT_INFO st3DLutInfo;

    if(copy_from_user(&st3DLutInfo, (ST_KDRV_XC_3DLUT_INFO __user *)arg, sizeof(ST_KDRV_XC_3DLUT_INFO)))
    {
        return -EFAULT;
    }
    XC_KDBG("u32Size: %d.\n", st3DLutInfo.u32Size);

    pu8Data = vmalloc(st3DLutInfo.u32Size);
    if (pu8Data)
    {
        if(copy_from_user(pu8Data, (MS_U8 __user *)(((ST_KDRV_XC_3DLUT_INFO __user *)arg)->pu8Data), st3DLutInfo.u32Size))
        {
            //free buffer
            if(pu8Data)
                vfree(pu8Data);
            return -EFAULT;
        }
#if 0 //for debug
        printk("\n3Dlut Data: \n");
        int i = 0;
        for (i = 0; i < st3DLutInfo.u32Size; i++)
        {
            printk("pu8Data[%d]: %d\n", i, *(pu8Data+i));
        }
        printk("\n");
#endif

        st3DLutInfo.pu8Data = pu8Data;
        if (MDrv_XC_Set3DLutInfo(&st3DLutInfo)!= TRUE)
        {
            //free buffer
            if(pu8Data)
                vfree(pu8Data);
            return -EPERM;
        }

        //free buffer
        if(pu8Data)
            vfree(pu8Data);
    }
    else
    {
        return -ENOMEM;
    }

    return 0;
}

int _MDrv_XC_IOC_SetShareMemInfo(struct file *filp, unsigned long arg)
{
    ST_KDRV_XC_SHARE_MEMORY_INFO stShmemInfo;

    if(copy_from_user(&stShmemInfo, (ST_KDRV_XC_SHARE_MEMORY_INFO __user *)arg, sizeof(ST_KDRV_XC_SHARE_MEMORY_INFO)))
    {
        return -EFAULT;
    }

    XC_KDBG("addr: 0x%08x, miuno: %d, u32Size: %d.\n", stShmemInfo.phyBaseAddr, stShmemInfo.u32MiuNo, stShmemInfo.u32Size);
    if (MDrv_XC_SetShareMemInfo(&stShmemInfo) != TRUE)
    {
        return -EPERM;
    }

    return 0;
}

int _MDrv_XC_IOC_EnableHDR(struct file *filp, unsigned long arg)
{
    MS_BOOL bEnableHDR = FALSE;

    if(__get_user(bEnableHDR, (MS_BOOL __user *)arg))
    {
        return -EFAULT;
    }
    XC_KDBG("HDR enable: %d\n", bEnableHDR);

    if (MDrv_XC_EnableHDR(bEnableHDR) != TRUE)
    {
        return -EPERM;
    }

    return 0;
}

int _MDrv_XC_IOC_SetHDRWindow(struct file *filp, unsigned long arg)
{
    ST_KDRV_XC_WINDOW_INFO stWindowInfo;

    if(copy_from_user(&stWindowInfo, (ST_KDRV_XC_WINDOW_INFO __user *)arg, sizeof(ST_KDRV_XC_WINDOW_INFO)))
    {
        return -EFAULT;
    }
    XC_KDBG("Set HDR window: (x: %d, y: %d, width: %d, height: %d)\n",
            stWindowInfo.u16X, stWindowInfo.u16Y, stWindowInfo.u16Width, stWindowInfo.u16Height);

    if (MDrv_XC_SetHDRWindow(&stWindowInfo) != TRUE)
    {
        return -EPERM;
    }

    return 0;
}

int _MDrv_XC_IOC_ConfigAutoDownload(struct file *filp, unsigned long arg)
{
    ST_KDRV_XC_AUTODOWNLOAD_CONFIG_INFO stConfigInfo;

    if(copy_from_user(&stConfigInfo, (ST_KDRV_XC_AUTODOWNLOAD_CONFIG_INFO __user *)arg, sizeof(ST_KDRV_XC_AUTODOWNLOAD_CONFIG_INFO)))
    {
        return -EFAULT;
    }
    XC_KDBG("Config auto download: (enable: %d, client: %d, mode: %d, phyBaseaddr: 0x%8x, u32Size: %d)\n",
            stConfigInfo.bEnable, stConfigInfo.enClient, stConfigInfo.enMode, stConfigInfo.phyBaseAddr, stConfigInfo.u32Size);

    if (MDrv_XC_ConfigAutoDownload(&stConfigInfo) != TRUE)
    {
        return -EPERM;
    }

    return 0;
}

int _MDrv_XC_IOC_WriteAutoDownload(struct file *filp, unsigned long arg)
{
    ST_KDRV_XC_AUTODOWNLOAD_DATA_INFO stDataInfo;
    ST_KDRV_XC_AUTODOWNLOAD_FORMAT_INFO stFormatInfo;
    MS_U8* pu8Data;

    if(copy_from_user(&stDataInfo, (ST_KDRV_XC_AUTODOWNLOAD_DATA_INFO __user *)arg, sizeof(ST_KDRV_XC_AUTODOWNLOAD_DATA_INFO)))
    {
        return -EFAULT;
    }
    XC_KDBG("client :%d, u32Size: %d.\n", stDataInfo.enClient, stDataInfo.u32Size);

    pu8Data = kmalloc(stDataInfo.u32Size, GFP_KERNEL);
    if (pu8Data != NULL && stDataInfo.pu8Data != NULL)
    {
        if(copy_from_user(pu8Data, (MS_U8 __user *)(((ST_KDRV_XC_AUTODOWNLOAD_DATA_INFO __user *)arg)->pu8Data), sizeof(stDataInfo.u32Size)))
        {
            //free buffer
            if(pu8Data)
                kfree(pu8Data);
            return -EFAULT;
        }
        stDataInfo.pu8Data = pu8Data;
#if 0 //for debug
        printk("\nWrite auto downlaod Data: \n");
        int i = 0;
        for (i = 0; i < stDataInfo.u32Size; i++)
        {
            printk("pu8Data[%d]: %d\n", i, *(pu8Data+i));
        }
        printk("\n");
#endif
    }
    else
    {
        //free buffer
        if(pu8Data)
            kfree(pu8Data);

        return -ENOMEM;
    }

    if (stDataInfo.pParam != NULL)
    {
        if(copy_from_user(&stFormatInfo, (ST_KDRV_XC_AUTODOWNLOAD_FORMAT_INFO __user *)(((ST_KDRV_XC_AUTODOWNLOAD_DATA_INFO __user *)arg)->pParam), sizeof(ST_KDRV_XC_AUTODOWNLOAD_FORMAT_INFO)))
        {
            //free buffer
            if(pu8Data)
                kfree(pu8Data);
            return -EFAULT;
        }
        stDataInfo.pParam = &stFormatInfo;
        XC_KDBG("Subclient :%d, Range(Enable: %d, startAddr: %d, endAddr: %d).\n",
                stFormatInfo.enSubClient, stFormatInfo.bEnableRange, stFormatInfo.u16StartAddr, stFormatInfo.u16EndAddr);
    }

    if (MDrv_XC_WriteAutoDownload(&stDataInfo) != TRUE)
    {
        //free buffer
        if(pu8Data)
            kfree(pu8Data);
        return -EPERM;
    }

    //free buffer
    if(pu8Data)
        kfree(pu8Data);

    return 0;
}

int _MDrv_XC_IOC_FireAutoDownload(struct file *filp, unsigned long arg)
{
    EN_KDRV_XC_AUTODOWNLOAD_CLIENT enClient;

    if(__get_user(enClient, (EN_KDRV_XC_AUTODOWNLOAD_CLIENT __user *)arg))
    {
        return -EFAULT;
    }

    XC_KDBG("Client: %d\n", enClient);

    if (MDrv_XC_FireAutoDownload(enClient) != TRUE)
    {
        return -EPERM;
    }

    return 0;
}

int _MDrv_XC_IOC_GetAutoDownloadCaps(struct file *filp, unsigned long arg)
{
    ST_KDRV_XC_AUTODOWNLOAD_CLIENT_SUPPORTED_CAPS stAutoDownlaodCaps;

    if(copy_from_user(&stAutoDownlaodCaps, (ST_KDRV_XC_AUTODOWNLOAD_CLIENT_SUPPORTED_CAPS __user *)arg, sizeof(ST_KDRV_XC_AUTODOWNLOAD_CLIENT_SUPPORTED_CAPS)))
    {
        return -EFAULT;
    }

    XC_KDBG("Client: %d\n", stAutoDownlaodCaps.enClient);

    if (KDrv_XC_GetAutoDownloadCaps(&stAutoDownlaodCaps) != TRUE)
    {
        return -EPERM;
    }

    XC_KDBG("bSupport: %d\n", stAutoDownlaodCaps.bSupported);

    __put_user(stAutoDownlaodCaps.bSupported, &(((ST_KDRV_XC_AUTODOWNLOAD_CLIENT_SUPPORTED_CAPS __user *)arg)->bSupported));

    return 0;
}

int _MDrv_XC_IOC_SetColorFormat(struct file *filp, unsigned long arg)
{
    EN_KDRV_XC_HDR_COLOR_FORMAT enColorFormat;

    if(__get_user(enColorFormat, (EN_KDRV_XC_HDR_COLOR_FORMAT __user *)arg))
    {
        return -EFAULT;
    }

    XC_KDBG("Color Format: %d\n", enColorFormat);

    if ((enColorFormat >= E_KDRV_XC_HDR_COLOR_MAX) || (enColorFormat <= E_KDRV_XC_HDR_COLOR_NONE))
    {
        return -EINVAL;
    }

    if (MDrv_XC_SetColorFormat(enColorFormat) != TRUE)
    {
        return -EPERM;
    }

    return 0;
}

int _MDrv_XC_IOC_CFDControl(struct file *filp, unsigned long arg)
{
    EN_KDRV_XC_CFD_CTRL_TYPE enCtrlType;
    ST_KDRV_XC_CFD_CONTROL_INFO stKdrvCFDCtrlInfo;
    MS_U8 *pu8Data = NULL;
    void *pParam = NULL;

    if(copy_from_user(&stKdrvCFDCtrlInfo, (ST_KDRV_XC_CFD_CONTROL_INFO __user *)arg, sizeof(ST_KDRV_XC_CFD_CONTROL_INFO)))
    {
        return -EFAULT;
    }

    pParam = stKdrvCFDCtrlInfo.pParam;

    enCtrlType = stKdrvCFDCtrlInfo.enCtrlType;
    XC_KDBG("enCtrlType: %d\n", enCtrlType);

    if (enCtrlType >= E_KDRV_XC_CFD_CTRL_MAX)
    {
        return -EINVAL;
    }

    if ((pu8Data == NULL) && (stKdrvCFDCtrlInfo.u32ParamLen != 0))
    {
        pu8Data = kmalloc(stKdrvCFDCtrlInfo.u32ParamLen, GFP_KERNEL);
    }

    if (pu8Data != NULL)
    {
        if (stKdrvCFDCtrlInfo.pParam != NULL)
        {
            if(copy_from_user(pu8Data, ((ST_KDRV_XC_CFD_CONTROL_INFO __user *)arg)->pParam, stKdrvCFDCtrlInfo.u32ParamLen))
            {
                //free buffer
                if(pu8Data)
                    kfree(pu8Data);
                return -EFAULT;
            }
            stKdrvCFDCtrlInfo.pParam = pu8Data;
        }
    }

    if (MDrv_XC_CFDControl(&stKdrvCFDCtrlInfo) != TRUE)
    {
        //free buffer
        if(pu8Data)
            kfree(pu8Data);
        return -EPERM;
    }

    if (stKdrvCFDCtrlInfo.enCtrlType == E_KDRV_XC_CFD_CTRL_GET_STATUS)
    {
        stKdrvCFDCtrlInfo.pParam = pParam;
        if(copy_to_user((ST_KDRV_XC_CFD_CONTROL_INFO __user *)arg, &stKdrvCFDCtrlInfo, sizeof(ST_KDRV_XC_CFD_CONTROL_INFO)))
        {
            //free buffer
            if(pu8Data)
                kfree(pu8Data);
            return -EFAULT;
        }

        if (copy_to_user((ST_KDRV_XC_CFD_STATUS*)((ST_KDRV_XC_CFD_CONTROL_INFO __user *)arg)->pParam, pu8Data, stKdrvCFDCtrlInfo.u32ParamLen))
        {
            //free buffer
            if(pu8Data)
                kfree(pu8Data);
            return -EFAULT;
        }
    }

    //free buffer
    if(pu8Data)
        kfree(pu8Data);

    return 0;
}

int _MDrv_XC_IOC_SetDSHDRInfo(struct file *filp, unsigned long arg)
{
    ST_KDRV_XC_DS_HDRInfo stDsHDRInfo;

    if(copy_from_user(&stDsHDRInfo, (ST_KDRV_XC_DS_HDRInfo __user *)arg, sizeof(ST_KDRV_XC_DS_HDRInfo)))
    {
        return -EFAULT;
    }

    XC_KDBG("index: %d, DmEnable: %d, CompEnable: %d, LutAddr: 0x%x, LutSize: 0x%x, RegsetAddr: 0x%x, RegsetSize: 0x%x\n",
            stDsHDRInfo.u8CurrentIndex, stDsHDRInfo.bDMEnable, stDsHDRInfo.bCompEnable, stDsHDRInfo.phyLutAddr, stDsHDRInfo.u32LutSize, stDsHDRInfo.phyRegAddr, stDsHDRInfo.u32RegSize);

    if (MDrv_XC_SetDSHDRInfo(&stDsHDRInfo))
    {
        return 0;
    }
    else
    {
        // The return value is not so appropriate.
        return -EPERM;
    }
}
#endif

int _MDrv_XC_IOC_DLCBleOnOffInfo(struct file *filp, unsigned long arg)
{

    MS_BOOL bSetBleOn ;

    if (__get_user(bSetBleOn, (MS_BOOL __user *)arg))
    {
        return -EFAULT;
    }

    if (MDrv_DLC_SetBleOnOffInfo(bSetBleOn))
    {
        return 0;
    }
    else
    {
        // The return value is not so appropriate.
        return -EPERM;
    }
}

int _MDrv_XC_IOC_DLCGetHistogram32Info(struct file *filp, unsigned long arg)
{
    MS_U16 g_wLumaKernelHistogram32H[32];

    if (!MDrv_DLC_GetHistogram32Info(g_wLumaKernelHistogram32H))
    {
        // The return value is not so appropriate.
        return -EPERM;
    }

    copy_to_user( (void*)arg,g_wLumaKernelHistogram32H,sizeof(g_wLumaKernelHistogram32H));

    return 0;
}

int _MDrv_XC_IOC_DLCBleChangeSlopPointInfo(struct file *filp, unsigned long arg)
{
    ST_KDRV_XC_DLC_BLE_INFO stBleInfo;

    if(copy_from_user(&stBleInfo, (ST_KDRV_XC_DLC_BLE_INFO __user *)arg, sizeof(ST_KDRV_XC_DLC_BLE_INFO)))
    {
        return -EFAULT;
    }

    if (MDrv_DLC_SetBlePointChangeInfo(&stBleInfo))
    {
        return 0;
    }
    else
    {
        // The return value is not so appropriate.
        return -EPERM;
    }
}

int _MDrv_XC_IOC_DLCOnOffInfo(struct file *filp, unsigned long arg)
{

    MS_BOOL bSetDlcOn ;

    if (__get_user(bSetDlcOn, (MS_BOOL __user *)arg))
    {
        return -EFAULT;
    }

    if (MDrv_DLC_SetOnOffInfo(bSetDlcOn))
    {
        return 0;
    }
    else
    {
        // The return value is not so appropriate.
        return -EPERM;
    }
}

int _MDrv_XC_IOC_DLCSetTMOInitInfo(struct file *filp, unsigned long arg)
{
    ST_KDRV_XC_TMO_INFO stTMOInfo;

    memset(&stTMOInfo, 0, sizeof(ST_KDRV_XC_TMO_INFO));
    if(copy_from_user(&stTMOInfo, (ST_KDRV_XC_TMO_INFO __user *)arg, sizeof(ST_KDRV_XC_TMO_INFO)))
    {
        return -EFAULT;
    }

    if (KDrv_DLC_SetTMOInfo(&stTMOInfo))
    {
        return 0;
    }
    else
    {
        // The return value is not so appropriate.
        return -EPERM;
    }
}

int _MDrv_XC_IOC_DLCSetInitInfo(struct file *filp, unsigned long arg)
{
    ST_KDRV_XC_DLC_INIT_INFO stDlcInitInfo;

    if(copy_from_user(&stDlcInitInfo, (ST_KDRV_XC_DLC_INIT_INFO __user *)arg, sizeof(ST_KDRV_XC_DLC_INIT_INFO)))
    {
        return -EFAULT;
    }

    if (KDrv_DLC_SetInitInfo(&stDlcInitInfo))
    {
        return 0;
    }
    else
    {
        // The return value is not so appropriate.
        return -EPERM;
    }
}

int _MDrv_XC_IOC_DLCChangeCurveInfo(struct file *filp, unsigned long arg)
{
    ST_KDRV_XC_DLC_INFO stDlcInfo;

    if(copy_from_user(&stDlcInfo, (ST_KDRV_XC_DLC_INFO __user *)arg, sizeof(ST_KDRV_XC_DLC_INFO)))
    {
        return -EFAULT;
    }

    if (MDrv_DLC_SetChangeCurveInfo(&stDlcInfo))
    {
        return 0;
    }
    else
    {
        // The return value is not so appropriate.
        return -EPERM;
    }
}


int _MDrv_XC_IOC_Mload_Init(struct file *filp, unsigned long arg)
{
    ST_KDRV_MLOAD_INIT init_ptr;
    if(copy_from_user(&init_ptr, (ST_KDRV_MLOAD_INIT __user *)arg, sizeof(ST_KDRV_MLOAD_INIT)))
    {
        return -EFAULT;
    }

    KApi_XC_MLoad_Init(init_ptr._client_type,init_ptr.PhyAddr, init_ptr.u32BufByteLen);

    return 0;
}

int _MDrv_XC_IOC_Mload_Enable(struct file *filp, unsigned long arg)
{
    ST_KDRV_MLOAD_ENABLE enable_ptr;
    if(copy_from_user(&enable_ptr, (ST_KDRV_MLOAD_ENABLE __user *)arg, sizeof(ST_KDRV_MLOAD_ENABLE)))
    {
        return -EFAULT;
    }

    KApi_XC_MLoad_Enable(enable_ptr._client_type,enable_ptr.bEnable);

    return 0;
}

int _MDrv_XC_IOC_Mload_GetStatus(struct file *filp, unsigned long arg)
{
    ST_KDRV_MLOAD_GETSTATUS status_ptr;

    if(copy_from_user(&status_ptr, (ST_KDRV_MLOAD_GETSTATUS __user *)arg, sizeof(ST_KDRV_MLOAD_GETSTATUS)))
    {
        return -EFAULT;
    }
    status_ptr.eReturn = KApi_XC_MLoad_GetStatus(status_ptr._client_type);
    if (copy_to_user((ST_KDRV_MLOAD_GETSTATUS __user *) arg, &status_ptr, sizeof(ST_KDRV_MLOAD_GETSTATUS)))
    {
        return -EFAULT;
    }
    return 0;
}

int _MDrv_XC_IOC_Mload_WriteCmd(struct file *filp, unsigned long arg)
{
    ST_KDRV_MLOAD_WRT_CMD cmd_ptr;
    if(copy_from_user(&cmd_ptr, (ST_KDRV_MLOAD_WRT_CMD __user *)arg, sizeof(ST_KDRV_MLOAD_WRT_CMD)))
    {
        return -EFAULT;
    }
    cmd_ptr.bReturn = KApi_XC_MLoad_WriteCmd(cmd_ptr._client_type,cmd_ptr.u32Addr, cmd_ptr.u16Data, cmd_ptr.u16Mask);
    if (copy_to_user((ST_KDRV_MLOAD_WRT_CMD __user *) arg, &cmd_ptr, sizeof(ST_KDRV_MLOAD_WRT_CMD)))
    {
        return -EFAULT;
    }
    return 0;
}

int _MDrv_XC_IOC_Mload_Fire(struct file *filp, unsigned long arg)
{
    ST_KDRV_MLOAD_FIRE fire_ptr;
    if(copy_from_user(&fire_ptr, (ST_KDRV_MLOAD_FIRE __user *)arg, sizeof(ST_KDRV_MLOAD_FIRE)))
    {
        return -EFAULT;
    }
    fire_ptr.bReturn = KApi_XC_MLoad_Fire(fire_ptr._client_type,fire_ptr.bImmeidate);
    if (copy_to_user((ST_KDRV_MLOAD_FIRE __user *) arg, &fire_ptr, sizeof(ST_KDRV_MLOAD_FIRE)))
    {
        return -EFAULT;
    }
    return 0;
}

int _MDrv_XC_IOC_Mload_WriteCmd_NonXC(struct file *filp, unsigned long arg)
{
    ST_KDRV_MLOAD_WRT_CMD_NONXC cmd_ptr;
    if(copy_from_user(&cmd_ptr, (ST_KDRV_MLOAD_WRT_CMD_NONXC __user *)arg, sizeof(ST_KDRV_MLOAD_WRT_CMD_NONXC)))
    {
        return -EFAULT;
    }
    cmd_ptr.bReturn = KApi_XC_MLoad_WriteCmd_NonXC(cmd_ptr._client_type,cmd_ptr.u32Bank,cmd_ptr.u32Addr, cmd_ptr.u16Data, cmd_ptr.u16Mask);
    if (copy_to_user((ST_KDRV_MLOAD_WRT_CMD_NONXC __user *) arg, &cmd_ptr, sizeof(ST_KDRV_MLOAD_WRT_CMD_NONXC)))
    {
        return -EFAULT;
    }
    return 0;
}

int _MDrv_XC_IOC_Mload_set_IP_trig_p(struct file *filp, unsigned long arg)
{
    ST_KDRV_MLOAD_SET_IP_TRIG_P set_ptr;
    if(copy_from_user(&set_ptr, (ST_KDRV_MLOAD_SET_IP_TRIG_P __user *)arg, sizeof(ST_KDRV_MLOAD_SET_IP_TRIG_P)))
    {
        return -EFAULT;
    }
    set_ptr.bReturn = KDrv_XC_MLoad_set_IP_trig_p(set_ptr._client_type,set_ptr.u16train, set_ptr.u16disp);
    if (copy_to_user((ST_KDRV_MLOAD_SET_IP_TRIG_P __user *) arg, &set_ptr, sizeof(ST_KDRV_MLOAD_SET_IP_TRIG_P)))
    {
        return -EFAULT;
    }
    return 0;
}

int _MDrv_XC_IOC_Mload_get_IP_trig_p(struct file *filp, unsigned long arg)
{
    ST_KDRV_MLOAD_GET_IP_TRIG_P get_ptr;
    MS_U16 tmp_Train;
    MS_U16 tmp_Disp;
    MS_U16 *tmp;
    MS_U16 *tmp1;
    if(copy_from_user(&get_ptr, (ST_KDRV_MLOAD_GET_IP_TRIG_P __user *)arg, sizeof(ST_KDRV_MLOAD_GET_IP_TRIG_P)))
    {
        return -EFAULT;
    }
    tmp=get_ptr.pu16Train;
    tmp1=get_ptr.pu16Disp;
    if(copy_from_user(&tmp_Train,(MS_U16 *)(((ST_KDRV_MLOAD_GET_IP_TRIG_P __user *)arg)->pu16Train), sizeof(MS_U16)))
    {
        return -EFAULT;
    }
    if(copy_from_user(&tmp_Disp,(MS_U16 *)(((ST_KDRV_MLOAD_GET_IP_TRIG_P __user *)arg)->pu16Disp), sizeof(MS_U16)))
    {
        return -EFAULT;
    }
    get_ptr.pu16Train=&tmp_Train;
    get_ptr.pu16Disp=&tmp_Disp;
    get_ptr.bReturn = KDrv_XC_MLoad_get_IP_trig_p(get_ptr._client_type,get_ptr.pu16Train, get_ptr.pu16Disp);


    if (copy_to_user((MS_U16 *)(((ST_KDRV_MLOAD_GET_IP_TRIG_P __user *) arg)->pu16Train), get_ptr.pu16Train, sizeof(MS_U16)))
    {
        return -EFAULT;
    }
    if (copy_to_user((MS_U16 *)(((ST_KDRV_MLOAD_GET_IP_TRIG_P __user *) arg)->pu16Disp), get_ptr.pu16Disp, sizeof(MS_U16)))
    {
        return -EFAULT;
    }
    get_ptr.pu16Train=tmp;
    get_ptr.pu16Disp=tmp1;
    if (copy_to_user((ST_KDRV_MLOAD_GET_IP_TRIG_P __user *) arg, &get_ptr, sizeof(ST_KDRV_MLOAD_GET_IP_TRIG_P)))
    {
        return -EFAULT;
    }

    return 0;
}

int _MDrv_XC_IOC_Mload_set_trigger_sync(struct file *filp, unsigned long arg)
{
    ST_KDRV_MLOAD_SET_TRIG_SYNC set_ptr;
    if(copy_from_user(&set_ptr, (ST_KDRV_MLOAD_SET_TRIG_SYNC __user *)arg, sizeof(ST_KDRV_MLOAD_SET_TRIG_SYNC)))
    {
        return -EFAULT;
    }
    KDrv_XC_MLoad_set_trigger_sync(set_ptr._client_type,set_ptr.eTriggerSync);
    if (copy_to_user((ST_KDRV_MLOAD_SET_TRIG_SYNC __user *) arg, &set_ptr, sizeof(ST_KDRV_MLOAD_SET_TRIG_SYNC)))
    {
        return -EFAULT;
    }
    return 0;
}

int _MDrv_XC_IOC_Get_DSForceIndexSupported(struct file *filp, unsigned long arg)
{
    ST_KDRV_DS_GET_DSForceIndexSupported get_ptr;
    if(copy_from_user(&get_ptr, (ST_KDRV_DS_GET_DSForceIndexSupported __user *)arg, sizeof(ST_KDRV_DS_GET_DSForceIndexSupported)))
    {
        return -EFAULT;
    }
    get_ptr.eReturn = KApi_XC_Get_DSForceIndexSupported(get_ptr.u32DeviceID, get_ptr.eWindow);
    if (copy_to_user((ST_KDRV_DS_GET_DSForceIndexSupported __user *) arg, &get_ptr, sizeof(ST_KDRV_DS_GET_DSForceIndexSupported)))
    {
        return -EFAULT;
    }
    return 0;
}

int _MDrv_XC_IOC_Set_DSIndexSourceSelect(struct file *filp, unsigned long arg)
{
    ST_KDRV_DS_SET_DSIndexSourceSelect set_ptr;
    if(copy_from_user(&set_ptr, (ST_KDRV_DS_SET_DSIndexSourceSelect __user *)arg, sizeof(ST_KDRV_DS_SET_DSIndexSourceSelect)))
    {
        return -EFAULT;
    }
    set_ptr.eReturn = KApi_XC_Set_DSIndexSourceSelect(set_ptr.u32DeviceID, set_ptr.eDSIdxSrc, set_ptr.eWindow);
    if (copy_to_user((ST_KDRV_DS_SET_DSIndexSourceSelect __user *) arg, &set_ptr, sizeof(ST_KDRV_DS_SET_DSIndexSourceSelect)))
    {
        return -EFAULT;
    }
    return 0;
}

int _MDrv_XC_IOC_Set_DSForceIndex(struct file *filp, unsigned long arg)
{
    ST_KDRV_DS_SET_DSForceIndex set_ptr;
    if(copy_from_user(&set_ptr, (ST_KDRV_DS_SET_DSForceIndex __user *)arg, sizeof(ST_KDRV_DS_SET_DSForceIndex)))
    {
        return -EFAULT;
    }
    set_ptr.eReturn = KApi_XC_Set_DSForceIndex(set_ptr.u32DeviceID, set_ptr.bEnable, set_ptr.u8Index, set_ptr.eWindow);
    if (copy_to_user((ST_KDRV_DS_SET_DSForceIndex __user *) arg, &set_ptr, sizeof(ST_KDRV_DS_SET_DSForceIndex)))
    {
        return -EFAULT;
    }
    return 0;
}

int _MDrv_XC_IOC_SetDynamicScaling(struct file *filp, unsigned long arg)
{
    ST_KDRV_DS_SetDynamicScaling set_ptr;
    K_XC_DynamicScaling_Info *tmp_info;
    if(copy_from_user(&set_ptr, (ST_KDRV_DS_SetDynamicScaling __user *)arg, sizeof(ST_KDRV_DS_SetDynamicScaling)))
    {
        return -EFAULT;
    }
    tmp_info = kmalloc(sizeof(K_XC_DynamicScaling_Info), GFP_KERNEL);
    if(tmp_info != NULL)
    {
        if(copy_from_user(tmp_info, (K_XC_DynamicScaling_Info *)(((ST_KDRV_DS_SetDynamicScaling __user *)arg)->pstDSInfo), sizeof(K_XC_DynamicScaling_Info)))
        {
            if (tmp_info)
                kfree(tmp_info);
            return -EFAULT;
        }
        set_ptr.pstDSInfo = tmp_info;
    }
    else
    {
        return -ENOMEM;
    }
    set_ptr.bReturn = KApi_XC_SetDynamicScaling(set_ptr.u32DeviceID, set_ptr.pstDSInfo,set_ptr.u32DSInfoLen, set_ptr.eWindow);
    if (copy_to_user((ST_KDRV_DS_SetDynamicScaling __user *) arg, &set_ptr, sizeof(ST_KDRV_DS_SetDynamicScaling)))
    {
        if (tmp_info)
            kfree(tmp_info);
        return -EFAULT;
    }

    if (tmp_info)
        kfree(tmp_info);

    return 0;
}

int _MDrv_XC_IOC_Set_DynamicScalingFlag(struct file *filp, unsigned long arg)
{
    ST_KDRV_DS_SET_DynamicScalingFlag set_ptr;
    if(copy_from_user(&set_ptr, (ST_KDRV_DS_SET_DynamicScalingFlag __user *)arg, sizeof(ST_KDRV_DS_SET_DynamicScalingFlag)))
    {
        return -EFAULT;
    }
    KApi_XC_Set_DynamicScalingFlag(set_ptr.u32DeviceID,set_ptr.bEnable);
    if (copy_to_user((ST_KDRV_DS_SET_DynamicScalingFlag __user *) arg, &set_ptr, sizeof(ST_KDRV_DS_SET_DynamicScalingFlag)))
    {
        return -EFAULT;
    }
    return 0;
}

int _MDrv_XC_IOC_GetDynamicScalingStatus(struct file *filp, unsigned long arg)
{
    ST_KDRV_DS_GetDynamicScalingStatus get_ptr;
    if(copy_from_user(&get_ptr, (ST_KDRV_DS_GetDynamicScalingStatus __user *)arg, sizeof(ST_KDRV_DS_GetDynamicScalingStatus)))
    {
        return -EFAULT;
    }
    get_ptr.bReturn = KDrv_XC_GetDynamicScalingStatus(get_ptr.u32DeviceID);
    if (copy_to_user((ST_KDRV_DS_GetDynamicScalingStatus __user *) arg, &get_ptr, sizeof(ST_KDRV_DS_GetDynamicScalingStatus)))
    {
        return -EFAULT;
    }
    return 0;
}

int _MDrv_XC_IOC_DS_EnableIPMTuneAfterDS(struct file *filp, unsigned long arg)
{
    ST_KDRV_DS_EnableIPMTuneAfterDS set_ptr;
    if(copy_from_user(&set_ptr, (ST_KDRV_DS_EnableIPMTuneAfterDS __user *)arg, sizeof(ST_KDRV_DS_EnableIPMTuneAfterDS)))
    {
        return -EFAULT;
    }
    set_ptr.bReturn = KDrv_XC_EnableIPMTuneAfterDS(set_ptr.u32DeviceID, set_ptr.bEnable);
    if (copy_to_user((ST_KDRV_DS_EnableIPMTuneAfterDS __user *) arg, &set_ptr, sizeof(ST_KDRV_DS_EnableIPMTuneAfterDS)))
    {
        return -EFAULT;
    }
    return 0;
}

int _MDrv_XC_IOC_DS_WriteSWDSCommand(struct file *filp, unsigned long arg)
{
    ST_KDRV_DS_WriteSWDSCommand write_ptr;
    if(copy_from_user(&write_ptr, (ST_KDRV_DS_WriteSWDSCommand __user *)arg, sizeof(ST_KDRV_DS_WriteSWDSCommand)))
    {
        return -EFAULT;
    }
    KApi_XC_WriteSWDSCommand(write_ptr.u32DeviceID,write_ptr.eWindow,write_ptr.client, write_ptr.u32CmdRegAddr, write_ptr.u16CmdRegValue,write_ptr.IPOP_Sel,write_ptr.Source_Select,write_ptr.pstXC_DS_CmdCnt);
    if (copy_to_user((ST_KDRV_DS_WriteSWDSCommand __user *) arg, &write_ptr, sizeof(ST_KDRV_DS_WriteSWDSCommand)))
    {
        return -EFAULT;
    }
    return 0;
}

int _MDrv_XC_IOC_DS_Add_NullCommand(struct file *filp, unsigned long arg)
{
    ST_KDRV_DS_Add_NullCommand write_ptr;
    if(copy_from_user(&write_ptr, (ST_KDRV_DS_Add_NullCommand __user *)arg, sizeof(ST_KDRV_DS_Add_NullCommand)))
    {
        return -EFAULT;
    }
    KApi_XC_Add_NullCommand(write_ptr.u32DeviceID,write_ptr.eWindow,write_ptr.client,write_ptr.IPOP_Sel,write_ptr.pstXC_DS_CmdCnt);
    if (copy_to_user((ST_KDRV_DS_Add_NullCommand __user *) arg, &write_ptr, sizeof(ST_KDRV_DS_Add_NullCommand)))
    {
        return -EFAULT;
    }
    return 0;
}

int _MDrv_XC_IOC_DS_SET_CLIENT(struct file *filp, unsigned long arg)
{
    ST_KDRV_DS_SET_CLIENT set_ptr;
    if(copy_from_user(&set_ptr, (ST_KDRV_DS_SET_CLIENT __user *)arg, sizeof(ST_KDRV_DS_SET_CLIENT)))
    {
        return -EFAULT;
    }
    set_ptr.bReturn = KApi_DS_set_client(set_ptr.u32DeviceID, set_ptr.client , set_ptr.max_num);
    if (copy_to_user((ST_KDRV_DS_SET_CLIENT __user *) arg, &set_ptr, sizeof(ST_KDRV_DS_SET_CLIENT)))
    {
        return -EFAULT;
    }
    return 0;
}

int _MDrv_XC_IOC_DS_GetSupportIndexNum(struct file *filp, unsigned long arg)
{
    ST_KDRV_DS_GET_SUPPORT_INDEX_NUM stIndexNum;

    stIndexNum.u8SupportIndexNum = KApi_DS_get_support_index_num();

    if (copy_to_user((ST_KDRV_DS_GET_SUPPORT_INDEX_NUM __user *) arg, &stIndexNum, sizeof(ST_KDRV_DS_GET_SUPPORT_INDEX_NUM)))
    {
        return -EFAULT;
    }

    return 0;
}

int _MDrv_XC_IOC_DS_WriteSWDSCommandNonXC(struct file *filp, unsigned long arg)
{
    ST_KDRV_DS_WriteSWDSCommandNonXC write_ptr;

    if(copy_from_user(&write_ptr, (ST_KDRV_DS_WriteSWDSCommandNonXC __user *)arg, sizeof(ST_KDRV_DS_WriteSWDSCommandNonXC)))
    {
        return -EFAULT;
    }

    KApi_XC_WriteSWDSCommandNonXC(write_ptr.u32DeviceID, write_ptr.eWindow, write_ptr.client, write_ptr.u32Bank, write_ptr.u32Addr, write_ptr.u16Data, write_ptr.u16Mask, write_ptr.IPOP_Sel, write_ptr.pstXC_DS_CmdCnt);

    if (copy_to_user((ST_KDRV_DS_WriteSWDSCommandNonXC __user *) arg, &write_ptr, sizeof(ST_KDRV_DS_WriteSWDSCommandNonXC)))
    {
        return -EFAULT;
    }

    return 0;
}

int _MDrv_XC_IOC_DS_WriteSWDSCommand_Mask(struct file *filp, unsigned long arg)
{
    ST_KDRV_DS_WriteSWDSCommand_Mask write_ptr;
    if(copy_from_user(&write_ptr, (ST_KDRV_DS_WriteSWDSCommand_Mask __user *)arg, sizeof(ST_KDRV_DS_WriteSWDSCommand_Mask)))
    {
        return -EFAULT;
    }
    KApi_XC_WriteSWDSCommand_Mask(write_ptr.u32DeviceID,write_ptr.eWindow,write_ptr.client, write_ptr.u32CmdRegAddr, write_ptr.u16CmdRegValue,write_ptr.IPOP_Sel,write_ptr.Source_Select,write_ptr.pstXC_DS_CmdCnt,write_ptr.u16Mask);
    if (copy_to_user((ST_KDRV_DS_WriteSWDSCommand_Mask __user *) arg, &write_ptr, sizeof(ST_KDRV_DS_WriteSWDSCommand_Mask)))
    {
        return -EFAULT;
    }
    return 0;
}

int _MDrv_XC_IOC_DS_GetIndex(struct file *filp, unsigned long arg)
{
    ST_KDRV_DS_GET_INDEX_INFO stGetInfoInfo;

    if(copy_from_user(&stGetInfoInfo, (ST_KDRV_DS_GET_INDEX_INFO __user *)arg, sizeof(ST_KDRV_DS_GET_INDEX_INFO)))
    {
        return -EFAULT;
    }

    if (KApi_XC_GetDynamicScalingIndex(stGetInfoInfo.u32DeviceID, stGetInfoInfo.eWindow, &stGetInfoInfo.u8DSIndex) != TRUE)
    {
        return -EPERM;
    }

    if (copy_to_user((ST_KDRV_DS_GET_INDEX_INFO __user *) arg, &stGetInfoInfo, sizeof(ST_KDRV_DS_GET_INDEX_INFO)))
    {
        return -EFAULT;
    }

    return 0;
}

int _MDrv_XC_IOC_DS_FireIndex(struct file *filp, unsigned long arg)
{
    ST_KDRV_DS_FIRE_INDEX_INFO stFireIndexInfo;

    if(copy_from_user(&stFireIndexInfo, (ST_KDRV_DS_FIRE_INDEX_INFO __user *)arg, sizeof(ST_KDRV_DS_FIRE_INDEX_INFO)))
    {
        return -EFAULT;
    }

    if (KApi_XC_FireDynamicScalingIndex(stFireIndexInfo.u32DeviceID, stFireIndexInfo.eWindow) != TRUE)
    {
        return -EPERM;
    }

    return 0;
}

void _MDrv_XC_GetCFDSupportStatus(MS_BOOL *pSupported)
{
#ifdef CONFIG_MSTAR_XC_HDR_SUPPORT
    *pSupported = TRUE;
#else
    *pSupported = FALSE;
#endif
}

int _MDrv_XC_IOC_GetCFDSupportStatus(struct file *filp, unsigned long arg)
{
    MS_BOOL bSupported = FALSE;

//#ifdef CONFIG_MSTAR_XC_HDR_SUPPORT
//    bSupported = TRUE;
//#else
//    bSupported = FALSE;
//#endif
    _MDrv_XC_GetCFDSupportStatus(&bSupported);

    if (copy_to_user((MS_BOOL __user *) arg, &bSupported, sizeof(MS_BOOL)))
    {
        return -EFAULT;
    }

    return 0;
}

static irqreturn_t _irq_top(int eIntNum, void* dev_id)
{
    return IRQ_WAKE_THREAD;
}

MS_U32 _MDrv_XC_RegisterInterrupt(void)
{
    int error;

    if(0 > (error=request_threaded_irq(E_IRQ_DISP, _irq_top, MDrv_XC_IntHandler, IRQF_SHARED | IRQF_ONESHOT, "scaler", &_devXC)))
    {
        XC_KDBG("\n[XC] Fail to request IRQ:%d\n", E_IRQ_DISP);
        return -EFAULT;
    }

    return 0;
}

MS_U32 _MDrv_XC_DeRegisterInterrupt(void)
{
    XC_KDBG("[XC]  dismiss IRQ:%d\n", E_IRQ_DISP);
    free_irq(E_IRQ_DISP,NULL);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------------------------
// IOCtrl Driver interface functions
//-------------------------------------------------------------------------------------------------
int _MDrv_XC_Open (struct inode *inode, struct file *filp)
{
    XC_KDBG("[XC] XC DRIVER OPEN\n");

    XC_ASSERT(devXC.refCnt>=0);
    devXC.refCnt++;
    return 0;
}

static ssize_t _MDrv_XC_Write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    printk("%s is invoked\n", __FUNCTION__);
    return count;
}
static ssize_t _MDrv_XC_Read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    printk("%s is invoked\n", __FUNCTION__);
    return 0;
}

int _MDrv_XC_Release(struct inode *inode, struct file *filp)
{
    XC_KDBG("[XC] XC DRIVER CLOSE\n");

    XC_ASSERT(devXC.refCnt>0);
    devXC.refCnt--;

    return 0;
}

long _MDrv_XC_IOCtl(struct file *filp, unsigned int u32Cmd, unsigned long u32Arg)
{
    int err = 0;
    int retval = 0;
    //if((u32Cmd == MDRV_XC_IOC_DS_SET_DYNAMICSCALING) || (u32Cmd==MDRV_XC_IOC_DS_GET_DS_STATUS))
    //    printf("\033[1;32m[%s:%d]\033[m\n",__FUNCTION__,__LINE__);
    if(devXC.refCnt <= 0)
    {
        printk("\n[XC] refCnt <= 0  \n ");
        return -EFAULT;
    }
    /* check u32Cmd valid */
    if(MDRV_XC_IOC_MAGIC == _IOC_TYPE(u32Cmd))
    {
        if(_IOC_NR(u32Cmd) >= MDRV_XC_IOC_MAX_NR)
        {
            XC_KDBG("[XC] IOCtl NR Error!!! (Cmd=%x)\n",u32Cmd);
            return -ENOTTY;
        }
    }
    else
    {
        XC_KDBG("[XC] IOCtl MAGIC Error!!! (Cmd=%x)\n",u32Cmd);
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
        printk("\n[XC] verify fail  \n ");
        return -EFAULT;
    }

    XC_KDBG("[XC] IOCtl number %x, 0x%x, 0x%x, 0x%x \n ", _IOC_NR(u32Cmd), u32Cmd, MDRV_XC_IOC_MLOAD_INIT, sizeof(ST_KDRV_MLOAD_INIT));
    /* handle u32Cmd */
    switch(u32Cmd)
    {
#ifdef CONFIG_MSTAR_XC_HDR_SUPPORT
        case MDRV_XC_IOC_SET_HDR_TYPE:
            retval = _MDrv_XC_IOC_SetHDRType(filp, u32Arg);
            break;
        case MDRV_XC_IOC_GET_HDR_TYPE:
            retval = _MDrv_XC_IOC_GetHDRType(filp, u32Arg);
            break;
        case MDRV_XC_IOC_SET_INPUT_SOURCE_TYPE:
            retval = _MDrv_XC_IOC_SetInputSourceType(filp, u32Arg);
            break;
        case MDRV_XC_IOC_GET_INPUT_SOURCE_TYPE:
            retval = _MDrv_XC_IOC_GetInputSourceType(filp, u32Arg);
            break;
        case MDRV_XC_IOC_SET_OPEN_METADATA_INFO:
            retval = _MDrv_XC_IOC_SetOpenMetadataInfo(filp, u32Arg);
            break;
        case MDRV_XC_IOC_SET_3DLUT_INFO:
            retval = _MDrv_XC_IOC_Set3DLutInfo(filp, u32Arg);
            break;
        case MDRV_XC_IOC_SET_SHMEM_INFO:
            retval = _MDrv_XC_IOC_SetShareMemInfo(filp, u32Arg);
            break;
        case MDRV_XC_IOC_ENABLE_HDR:
            retval = _MDrv_XC_IOC_EnableHDR(filp, u32Arg);
            break;
        case MDRV_XC_IOC_SET_HDR_WINDOW:
            retval = _MDrv_XC_IOC_SetHDRWindow(filp, u32Arg);
            break;
        case MDRV_XC_IOC_CONFIG_AUTO_DOWNLOAD:
            retval = _MDrv_XC_IOC_ConfigAutoDownload(filp, u32Arg);
            break;
        case MDRV_XC_IOC_WRITE_AUTO_DOWNLOAD:
            retval = _MDrv_XC_IOC_WriteAutoDownload(filp, u32Arg);
            break;
        case MDRV_XC_IOC_FIRE_AUTO_DOWNLOAD:
            retval = _MDrv_XC_IOC_FireAutoDownload(filp, u32Arg);
            break;
        case MDRV_XC_IOC_GET_AUTO_DOWNLOAD_CAPS:
            retval = _MDrv_XC_IOC_GetAutoDownloadCaps(filp, u32Arg);
            break;
        case MDRV_XC_IOC_SET_COLOR_FORMAT:
            retval = _MDrv_XC_IOC_SetColorFormat(filp, u32Arg);
            break;
        case MDRV_XC_IOC_CFDCONCTRL:
            retval = _MDrv_XC_IOC_CFDControl(filp, u32Arg);
            break;
        case MDRV_XC_IOC_SET_DSHDRINFO:
            retval = _MDrv_XC_IOC_SetDSHDRInfo(filp, u32Arg);
            break;
#endif
        case MDRV_XC_IOC_DLCCHANGECURVE:
            retval = _MDrv_XC_IOC_DLCChangeCurveInfo(filp, u32Arg);
            break;
        case MDRV_XC_IOC_DLCONOFFINFO:
            retval = _MDrv_XC_IOC_DLCOnOffInfo(filp, u32Arg);
            break;
        case MDRV_XC_IOC_DLCBLECHANGECURVE:
            retval = _MDrv_XC_IOC_DLCBleChangeSlopPointInfo(filp, u32Arg);
            break;
        case MDRV_XC_IOC_DLCBLEONOFFINFO:
            retval = _MDrv_XC_IOC_DLCBleOnOffInfo(filp, u32Arg);
            break;
        case MDRV_XC_IOC_DLCGETHISTOGRAMINFO:
            retval = _MDrv_XC_IOC_DLCGetHistogram32Info(filp, u32Arg);
            break;
        case MDRV_XC_IOC_DLCSETDLCINITINFO:
            retval = _MDrv_XC_IOC_DLCSetInitInfo(filp, u32Arg);
            break;
        case MDRV_XC_IOC_DLCSETTMOINFO:
            retval = _MDrv_XC_IOC_DLCSetTMOInitInfo(filp, u32Arg);
            break;
        case MDRV_XC_IOC_MLOAD_INIT:
            retval = _MDrv_XC_IOC_Mload_Init(filp, u32Arg);
            break;
        case MDRV_XC_IOC_MLOAD_ENABLE:
            retval = _MDrv_XC_IOC_Mload_Enable(filp, u32Arg);
            break;
        case MDRV_XC_IOC_MLOAD_GETSTATUS:
            retval = _MDrv_XC_IOC_Mload_GetStatus(filp, u32Arg);
            break;
        case MDRV_XC_IOC_MLOAD_WRT_CMD:
            retval = _MDrv_XC_IOC_Mload_WriteCmd(filp, u32Arg);
            break;
        case MDRV_XC_IOC_MLOAD_FIRE:
            retval = _MDrv_XC_IOC_Mload_Fire(filp, u32Arg);
            break;
        case MDRV_XC_IOC_MLOAD_WRT_CMD_NONXC:
            retval = _MDrv_XC_IOC_Mload_WriteCmd_NonXC(filp, u32Arg);
            break;
        case MDRV_XC_IOC_MLOAD_SET_IP_TRIG_P:
            retval = _MDrv_XC_IOC_Mload_set_IP_trig_p(filp, u32Arg);
            break;
        case MDRV_XC_IOC_MLOAD_GET_IP_TRIG_P:
            retval = _MDrv_XC_IOC_Mload_get_IP_trig_p(filp, u32Arg);
            break;
        case MDRV_XC_IOC_MLOAD_SET_TRIG_SYNC:
            retval = _MDrv_XC_IOC_Mload_set_trigger_sync(filp, u32Arg);
            break;
        case MDRV_XC_IOC_DS_GET_DSFORCE_INDEX_SUPPORT:
            retval = _MDrv_XC_IOC_Get_DSForceIndexSupported(filp, u32Arg);
            break;
        case MDRV_XC_IOC_DS_SET_DSINDEX_SOURCE_SELECT:
            retval = _MDrv_XC_IOC_Set_DSIndexSourceSelect(filp, u32Arg);
            break;
        case MDRV_XC_IOC_DS_SET_DSFORCE_INDEX:
            retval = _MDrv_XC_IOC_Set_DSForceIndex(filp, u32Arg);
            break;
        case MDRV_XC_IOC_DS_SET_DYNAMICSCALING:
            retval = _MDrv_XC_IOC_SetDynamicScaling(filp, u32Arg);
            break;
        case MDRV_XC_IOC_DS_SET_DYNAMICSCALING_FLAG:
            retval = _MDrv_XC_IOC_Set_DynamicScalingFlag(filp, u32Arg);
            break;
        case MDRV_XC_IOC_DS_GET_DS_STATUS:
            retval = _MDrv_XC_IOC_GetDynamicScalingStatus(filp, u32Arg);
            break;
        case MDRV_XC_IOC_DS_ENABLE_IPM_TUNE_AFTER_DS:
            retval = _MDrv_XC_IOC_DS_EnableIPMTuneAfterDS(filp, u32Arg);
            break;
        case MDRV_XC_IOC_DS_WRITE_SWDS_CMD:
            retval = _MDrv_XC_IOC_DS_WriteSWDSCommand(filp, u32Arg);
            break;
        case MDRV_XC_IOC_DS_ADD_NULL_CMD:
            retval = _MDrv_XC_IOC_DS_Add_NullCommand(filp, u32Arg);
            break;
        case MDRV_XC_IOC_DS_SET_CLIENT:
            retval = _MDrv_XC_IOC_DS_SET_CLIENT(filp, u32Arg);
            break;
        case MDRV_XC_IOC_DS_GET_SUPPORT_INDEX_NUM:
            retval = _MDrv_XC_IOC_DS_GetSupportIndexNum(filp, u32Arg);
            break;
        case MDRV_XC_IOC_DS_WRITE_SWDS_NONXC_CMD:
            retval = _MDrv_XC_IOC_DS_WriteSWDSCommandNonXC(filp, u32Arg);
            break;
        case MDRV_XC_IOC_DS_GET_INDEX:
            retval = _MDrv_XC_IOC_DS_GetIndex(filp, u32Arg);
            break;
        case MDRV_XC_IOC_DS_FIRE_INDEX:
            retval = _MDrv_XC_IOC_DS_FireIndex(filp, u32Arg);
            break;
        case MDRV_XC_IOC_GET_CFD_SUPPORT_STATUS:
            retval = _MDrv_XC_IOC_GetCFDSupportStatus(filp, u32Arg);
            break;
        case MDRV_XC_IOC_DS_WRITE_SWDS_CMD_MASK:
            retval = _MDrv_XC_IOC_DS_WriteSWDSCommand_Mask(filp, u32Arg);
            break;
        default:  /* redundant, as cmd was checked against MAXNR */
            XC_KDBG("[XC] ERROR IOCtl number %x\n ", _IOC_NR(u32Cmd));
            return -ENOTTY;
    }

    return (long)retval;
}

#ifdef CONFIG_COMPAT
#define CMP_CPY_FROM_USER(a,b,c)                                if(copy_from_user(a, compat_ptr((unsigned long)b), c) != 0)  {  break; }
#define CMP_CPY_TO_USER(a,b,c)                                  if(copy_to_user(compat_ptr((unsigned long)a), b, c) != 0) { break;  }

typedef struct
{
    EN_MLOAD_CLIENT_TYPE _client_type;
    compat_uptr_t PhyAddr;
    MS_U32 u32BufByteLen;
} _CMP_ST_KDRV_MLOAD_INIT;

typedef struct
{
    EN_MLOAD_CLIENT_TYPE _client_type;
    compat_uptr_t pu16Train;
    compat_uptr_t pu16Disp;
    MS_BOOL bReturn;
} _CMP_ST_KDRV_MLOAD_GET_IP_TRIG_P;

typedef struct __attribute__((packed))
{
    EN_KDRV_SC_DEVICE u32DeviceID;
    compat_uptr_t pstDSInfo;
    MS_U32    pDummy;
    MS_U32 u32DSInfoLen;
    EN_KDRV_WINDOW  eWindow;
    MS_BOOL bReturn;
} _CMP_ST_KDRV_DS_SetDynamicScaling;

typedef struct __attribute__((packed))
{
    MS_PHY u32DS_Info_BaseAddr;
    MS_U8  u8MIU_Select;
    MS_U8  u8DS_Index_Depth;
    MS_BOOL bOP_DS_On;
    MS_BOOL bIPS_DS_On;
    MS_BOOL bIPM_DS_On;
} _CMP_K_XC_DynamicScaling_Info;

typedef struct __attribute__((packed))
{
    EN_KDRV_SC_DEVICE u32DeviceID;
    EN_KDRV_WINDOW eWindow;
    E_DS_CLIENT client;
    MS_U32 u32CmdRegAddr;
    MS_U16 u16CmdRegValue;
    k_ds_reg_ip_op_sel IPOP_Sel;
    k_ds_reg_source_sel Source_Select;
    compat_uptr_t pstXC_DS_CmdCnt;
    MS_U32    pDummy;
} _CMP_ST_KDRV_DS_WriteSWDSCommand;

typedef struct __attribute__((packed))
{
    EN_KDRV_SC_DEVICE u32DeviceID;
    EN_KDRV_WINDOW eWindow;
    E_DS_CLIENT client;
    MS_U32 u32Bank;
    MS_U32 u32Addr;
    MS_U16 u16Data;
    MS_U16 u16Mask;
    k_ds_reg_ip_op_sel IPOP_Sel;
    compat_uptr_t pstXC_DS_CmdCnt;
    MS_U32    pDummy;
} _CMP_ST_KDRV_DS_WriteSWDSCommandNonXC;

typedef struct __attribute__((packed))
{
    EN_KDRV_SC_DEVICE u32DeviceID;
    EN_KDRV_WINDOW eWindow;
    E_DS_CLIENT client;
    MS_U32 u32CmdRegAddr;
    MS_U16 u16CmdRegValue;
    MS_U16 u16Mask;
    k_ds_reg_ip_op_sel IPOP_Sel;
    k_ds_reg_source_sel Source_Select;
    compat_uptr_t pstXC_DS_CmdCnt;
    MS_U32    pDummy;
}_CMP_ST_KDRV_DS_WriteSWDSCommand_Mask;

typedef struct __attribute__((packed))
{
    EN_KDRV_SC_DEVICE u32DeviceID;
    EN_KDRV_WINDOW eWindow;
    E_DS_CLIENT client;
    k_ds_reg_ip_op_sel IPOP_Sel;
    compat_uptr_t pstXC_DS_CmdCnt;
    MS_U32    pDummy;
} _CMP_ST_KDRV_DS_Add_NullCommand;

long _Compat_MDrv_XC_IOCtl(struct file *filp, unsigned int u32Cmd, unsigned long u32Arg)
{
    int retval = 0;
    compat_uptr_t ptr;
    //MS_U32 u32;
    MS_BOOL btmp;
    EN_MLOAD_CLIENT_TYPE eClient;

    if (!filp->f_op || !filp->f_op->unlocked_ioctl)
        return -ENOTTY;
    switch(u32Cmd)
    {
#ifdef CONFIG_MSTAR_XC_HDR_SUPPORT
        case MDRV_XC_IOC_SET_HDR_TYPE:
        case MDRV_XC_IOC_GET_HDR_TYPE:
        case MDRV_XC_IOC_SET_INPUT_SOURCE_TYPE:
        case MDRV_XC_IOC_GET_INPUT_SOURCE_TYPE:
        case MDRV_XC_IOC_SET_OPEN_METADATA_INFO:
        case MDRV_XC_IOC_SET_3DLUT_INFO:
        case MDRV_XC_IOC_SET_SHMEM_INFO:
        case MDRV_XC_IOC_ENABLE_HDR:
        case MDRV_XC_IOC_SET_HDR_WINDOW:
        case MDRV_XC_IOC_CONFIG_AUTO_DOWNLOAD:
        case MDRV_XC_IOC_WRITE_AUTO_DOWNLOAD:
        case MDRV_XC_IOC_FIRE_AUTO_DOWNLOAD:
        case MDRV_XC_IOC_GET_AUTO_DOWNLOAD_CAPS:
        case MDRV_XC_IOC_SET_COLOR_FORMAT:
        case MDRV_XC_IOC_CFDCONCTRL:
#endif
        case MDRV_XC_IOC_DLCCHANGECURVE:
        case MDRV_XC_IOC_DLCONOFFINFO:
        case MDRV_XC_IOC_DLCBLECHANGECURVE:
        case MDRV_XC_IOC_DLCBLEONOFFINFO:
        case MDRV_XC_IOC_DLCGETHISTOGRAMINFO:
        case MDRV_XC_IOC_DLCSETDLCINITINFO:
        case MDRV_XC_IOC_DLCSETTMOINFO:
        case MDRV_XC_IOC_SET_DSHDRINFO:
        case MDRV_XC_IOC_MLOAD_INIT:
        case MDRV_XC_IOC_MLOAD_ENABLE:
        case MDRV_XC_IOC_MLOAD_GETSTATUS:
        case MDRV_XC_IOC_MLOAD_WRT_CMD:
        case MDRV_XC_IOC_MLOAD_FIRE:
        case MDRV_XC_IOC_MLOAD_WRT_CMD_NONXC:
        case MDRV_XC_IOC_MLOAD_SET_IP_TRIG_P:
        case MDRV_XC_IOC_MLOAD_SET_TRIG_SYNC:
        case MDRV_XC_IOC_DS_GET_DSFORCE_INDEX_SUPPORT:
        case MDRV_XC_IOC_DS_SET_DSINDEX_SOURCE_SELECT:
        case MDRV_XC_IOC_DS_SET_DSFORCE_INDEX:
        case MDRV_XC_IOC_DS_SET_DYNAMICSCALING_FLAG:
        case MDRV_XC_IOC_DS_GET_DS_STATUS:
        case MDRV_XC_IOC_DS_ENABLE_IPM_TUNE_AFTER_DS:
        case MDRV_XC_IOC_DS_SET_CLIENT:
        case MDRV_XC_IOC_DS_GET_SUPPORT_INDEX_NUM:
        case MDRV_XC_IOC_DS_GET_INDEX:
        case MDRV_XC_IOC_DS_FIRE_INDEX:
        case MDRV_XC_IOC_GET_CFD_SUPPORT_STATUS:
        {
            return filp->f_op->unlocked_ioctl(filp, u32Cmd,
                                              (unsigned long)compat_ptr(u32Arg));
        }
        break;
        case MDRV_XC_IOC_MLOAD_GET_IP_TRIG_P:
        {
            _CMP_ST_KDRV_MLOAD_GET_IP_TRIG_P __user *uTmp1;
            ST_KDRV_MLOAD_GET_IP_TRIG_P __user *kTmp1;
            uTmp1 = compat_ptr(u32Arg);
            kTmp1 = (ST_KDRV_MLOAD_GET_IP_TRIG_P *)compat_alloc_user_space(sizeof(ST_KDRV_MLOAD_GET_IP_TRIG_P));
            get_user(eClient, &uTmp1->_client_type);
            put_user(eClient, &kTmp1->_client_type);
            get_user(ptr, &uTmp1->pu16Train);
            put_user(ptr, &kTmp1->pu16Train);
            get_user(ptr, &uTmp1->pu16Disp);
            put_user(ptr, &kTmp1->pu16Disp);
            get_user(btmp, &uTmp1->bReturn);
            put_user(btmp, &kTmp1->bReturn);

            retval = filp->f_op->unlocked_ioctl(filp, u32Cmd, (unsigned long)kTmp1);

            get_user(eClient, &kTmp1->_client_type);
            put_user(eClient, &uTmp1->_client_type);
            get_user(ptr, &kTmp1->pu16Train);
            put_user(ptr, &uTmp1->pu16Train);
            get_user(ptr, &kTmp1->pu16Disp);
            put_user(ptr, &uTmp1->pu16Disp);
            get_user(btmp, &kTmp1->bReturn);
            put_user(btmp, &uTmp1->bReturn);
            return retval;
        }
        break;
        case MDRV_XC_IOC_DS_WRITE_SWDS_CMD:
        {
            _CMP_ST_KDRV_DS_WriteSWDSCommand uPtr;
            ST_KDRV_DS_WriteSWDSCommand kPtr;
            CMP_CPY_FROM_USER(&uPtr, u32Arg, sizeof(_CMP_ST_KDRV_DS_WriteSWDSCommand));
            kPtr.pstXC_DS_CmdCnt =  kmalloc(sizeof(K_XC_DS_CMDCNT),GFP_KERNEL);
            CMP_CPY_FROM_USER(kPtr.pstXC_DS_CmdCnt,uPtr.pstXC_DS_CmdCnt,sizeof(K_XC_DS_CMDCNT));
            kPtr.u32DeviceID        = uPtr.u32DeviceID;
            kPtr.eWindow            = uPtr.eWindow;
            kPtr.client             = uPtr.client;
            kPtr.u32CmdRegAddr      = uPtr.u32CmdRegAddr;
            kPtr.u16CmdRegValue     = uPtr.u16CmdRegValue;
            kPtr.IPOP_Sel           = uPtr.IPOP_Sel;
            kPtr.Source_Select      = uPtr.Source_Select;

            KApi_XC_WriteSWDSCommand(kPtr.u32DeviceID,kPtr.eWindow,kPtr.client, kPtr.u32CmdRegAddr, kPtr.u16CmdRegValue,kPtr.IPOP_Sel,kPtr.Source_Select,kPtr.pstXC_DS_CmdCnt);

            CMP_CPY_TO_USER(uPtr.pstXC_DS_CmdCnt,kPtr.pstXC_DS_CmdCnt,sizeof(K_XC_DS_CMDCNT));
            CMP_CPY_TO_USER(u32Arg,&uPtr,sizeof(_CMP_ST_KDRV_DS_WriteSWDSCommand));

            if (kPtr.pstXC_DS_CmdCnt)
                kfree(kPtr.pstXC_DS_CmdCnt);

            return retval;

        }
        break;
        case MDRV_XC_IOC_DS_WRITE_SWDS_NONXC_CMD:
        {
            _CMP_ST_KDRV_DS_WriteSWDSCommandNonXC uPtr;
            ST_KDRV_DS_WriteSWDSCommandNonXC kPtr;
            CMP_CPY_FROM_USER(&uPtr, u32Arg, sizeof(_CMP_ST_KDRV_DS_WriteSWDSCommandNonXC));
            kPtr.pstXC_DS_CmdCnt =  kmalloc(sizeof(K_XC_DS_CMDCNT),GFP_KERNEL);
            CMP_CPY_FROM_USER(kPtr.pstXC_DS_CmdCnt, uPtr.pstXC_DS_CmdCnt, sizeof(K_XC_DS_CMDCNT));
            kPtr.u32DeviceID        = uPtr.u32DeviceID;
            kPtr.eWindow            = uPtr.eWindow;
            kPtr.client             = uPtr.client;
            kPtr.u32Bank            = uPtr.u32Bank;
            kPtr.u32Addr            = uPtr.u32Addr;
            kPtr.u16Data            = uPtr.u16Data;
            kPtr.u16Mask            = uPtr.u16Mask;
            kPtr.IPOP_Sel           = uPtr.IPOP_Sel;

            KApi_XC_WriteSWDSCommandNonXC(kPtr.u32DeviceID, kPtr.eWindow, kPtr.client, kPtr.u32Bank, kPtr.u32Addr, kPtr.u16Data, kPtr.u16Mask, kPtr.IPOP_Sel, kPtr.pstXC_DS_CmdCnt);

            CMP_CPY_TO_USER(uPtr.pstXC_DS_CmdCnt, kPtr.pstXC_DS_CmdCnt, sizeof(K_XC_DS_CMDCNT));
            CMP_CPY_TO_USER(u32Arg, &uPtr, sizeof(_CMP_ST_KDRV_DS_WriteSWDSCommandNonXC));

            if (kPtr.pstXC_DS_CmdCnt)
                kfree(kPtr.pstXC_DS_CmdCnt);

            return retval;

        }
        break;
        case MDRV_XC_IOC_DS_WRITE_SWDS_CMD_MASK:
        {
            _CMP_ST_KDRV_DS_WriteSWDSCommand_Mask uPtr;
            ST_KDRV_DS_WriteSWDSCommand_Mask kPtr;
            CMP_CPY_FROM_USER(&uPtr, u32Arg, sizeof(_CMP_ST_KDRV_DS_WriteSWDSCommand_Mask));
            kPtr.pstXC_DS_CmdCnt =  kmalloc(sizeof(K_XC_DS_CMDCNT),GFP_KERNEL);
            CMP_CPY_FROM_USER(kPtr.pstXC_DS_CmdCnt,uPtr.pstXC_DS_CmdCnt,sizeof(K_XC_DS_CMDCNT));
            kPtr.u32DeviceID        = uPtr.u32DeviceID;
            kPtr.eWindow            = uPtr.eWindow;
            kPtr.client             = uPtr.client;
            kPtr.u32CmdRegAddr      = uPtr.u32CmdRegAddr;
            kPtr.u16CmdRegValue     = uPtr.u16CmdRegValue;
            kPtr.u16Mask           = uPtr.u16Mask;
            kPtr.IPOP_Sel           = uPtr.IPOP_Sel;
            kPtr.Source_Select      = uPtr.Source_Select;

            KApi_XC_WriteSWDSCommand_Mask(kPtr.u32DeviceID,kPtr.eWindow,kPtr.client, kPtr.u32CmdRegAddr, kPtr.u16CmdRegValue,kPtr.IPOP_Sel,kPtr.Source_Select,kPtr.pstXC_DS_CmdCnt,kPtr.u16Mask);

            CMP_CPY_TO_USER(uPtr.pstXC_DS_CmdCnt,kPtr.pstXC_DS_CmdCnt,sizeof(K_XC_DS_CMDCNT));
            CMP_CPY_TO_USER(u32Arg,&uPtr,sizeof(_CMP_ST_KDRV_DS_WriteSWDSCommand_Mask));

            if (kPtr.pstXC_DS_CmdCnt)
                kfree(kPtr.pstXC_DS_CmdCnt);

            return retval;

        }
        break;
        case MDRV_XC_IOC_DS_ADD_NULL_CMD:
        {
            _CMP_ST_KDRV_DS_Add_NullCommand uPtr;
            ST_KDRV_DS_Add_NullCommand kPtr;
            CMP_CPY_FROM_USER(&uPtr, u32Arg, sizeof(_CMP_ST_KDRV_DS_Add_NullCommand));
            kPtr.pstXC_DS_CmdCnt =  kmalloc(sizeof(K_XC_DS_CMDCNT),GFP_KERNEL);
            CMP_CPY_FROM_USER(kPtr.pstXC_DS_CmdCnt,uPtr.pstXC_DS_CmdCnt,sizeof(K_XC_DS_CMDCNT));
            kPtr.u32DeviceID        = uPtr.u32DeviceID;
            kPtr.eWindow            = uPtr.eWindow;
            kPtr.client             = uPtr.client;
            kPtr.IPOP_Sel           = uPtr.IPOP_Sel;

            KApi_XC_Add_NullCommand(kPtr.u32DeviceID,kPtr.eWindow,kPtr.client,kPtr.IPOP_Sel,kPtr.pstXC_DS_CmdCnt);

            CMP_CPY_TO_USER(uPtr.pstXC_DS_CmdCnt,kPtr.pstXC_DS_CmdCnt,sizeof(K_XC_DS_CMDCNT));
            CMP_CPY_TO_USER(u32Arg,&uPtr,sizeof(_CMP_ST_KDRV_DS_Add_NullCommand));

            if (kPtr.pstXC_DS_CmdCnt)
                kfree(kPtr.pstXC_DS_CmdCnt);

            return retval;
        }
        break;
        case MDRV_XC_IOC_DS_SET_DYNAMICSCALING:
        {
            _CMP_ST_KDRV_DS_SetDynamicScaling uPtr;
            ST_KDRV_DS_SetDynamicScaling kPtr;
            K_XC_DynamicScaling_Info *ds_info;

            CMP_CPY_FROM_USER(&uPtr, u32Arg, sizeof(_CMP_ST_KDRV_DS_SetDynamicScaling));
            ds_info = uPtr.pstDSInfo;
            kPtr.pstDSInfo      = kmalloc(sizeof(K_XC_DynamicScaling_Info),GFP_KERNEL);
            CMP_CPY_FROM_USER(kPtr.pstDSInfo,ds_info,sizeof(K_XC_DynamicScaling_Info));
            kPtr.u32DeviceID    =uPtr.u32DeviceID;
            kPtr.u32DSInfoLen   =uPtr.u32DSInfoLen;
            kPtr.eWindow        =uPtr.eWindow;
            kPtr.bReturn = KApi_XC_SetDynamicScaling(kPtr.u32DeviceID, kPtr.pstDSInfo,kPtr.u32DSInfoLen, kPtr.eWindow);
            uPtr.bReturn = kPtr.bReturn;
            CMP_CPY_TO_USER(u32Arg,&uPtr,sizeof(_CMP_ST_KDRV_DS_SetDynamicScaling));

            if (kPtr.pstDSInfo)
                kfree(kPtr.pstDSInfo);

            return retval;
        }
        break;
        default:
            return -ENOTTY;
            break;

    }
    return -ENOTTY;
}
#endif

int _MDrv_XC_ModuleInit(void)
{
    int         s32Ret;
    dev_t       dev;

    mxc_class = class_create(THIS_MODULE, MDRV_NAME_SCALER);
    if (IS_ERR(mxc_class))
    {
        return PTR_ERR(mxc_class);
    }

    if (devXC.s32Major)
    {
        dev = MKDEV(devXC.s32Major, devXC.s32Minor);
        s32Ret = register_chrdev_region(dev, MDRV_XC_DEVICE_COUNT, MDRV_XC_NAME);
    }
    else
    {
        s32Ret = alloc_chrdev_region(&dev, devXC.s32Minor, MDRV_XC_DEVICE_COUNT, MDRV_XC_NAME);
        devXC.s32Major = MAJOR(dev);
    }

    if (0 > s32Ret)
    {
        XC_KDBG("[XC] Unable to get major= %d  ;s32Ret=%d  ;\n", devXC.s32Major,s32Ret);
        PTR_ERR(mxc_class);
        return s32Ret;
    }

    cdev_init(&devXC.cdev ,&devXC.fops);
    if (0 != (s32Ret = cdev_add(&devXC.cdev, dev, MDRV_XC_DEVICE_COUNT)))
    {
        XC_KDBG("[XC] Unable add a character device\n");
        unregister_chrdev_region(dev, MDRV_XC_DEVICE_COUNT);
        PTR_ERR(mxc_class);
        return s32Ret;
    }

    /* initial the whole XC Driver */
    if(EFAULT == _MDrv_XC_RegisterInterrupt())
    {
        XC_KDBG("[XC] Startup XC Driver Failed! %d\n", devXC.s32Major);
        cdev_del(&devXC.cdev);
        unregister_chrdev_region(dev, MDRV_XC_DEVICE_COUNT);
        PTR_ERR(mxc_class);
        return -ENOMEM;
    }
    device_create(mxc_class, NULL, dev, NULL, MDRV_NAME_SCALER);
    platform_driver_register(&Mstar_XC_driver);
    /*  init proc file  */
    init_xc_proc();
#ifdef CONFIG_MSTAR_XC_HDR_SUPPORT
    MDrv_XC_Init();
#endif
    MDrv_DLC_init();
    return 0;
}


void _MDrv_XC_ModuleExit(void)
{
    /*de-initial the who XCDriver */
    _MDrv_XC_DeRegisterInterrupt();

    cdev_del(&devXC.cdev);
    unregister_chrdev_region(MKDEV(devXC.s32Major, devXC.s32Minor), MDRV_XC_DEVICE_COUNT);
    device_destroy(mxc_class, MKDEV(devXC.s32Major, devXC.s32Minor));
    class_destroy(mxc_class);
    platform_driver_unregister(&Mstar_XC_driver);

#ifdef CONFIG_MSTAR_XC_HDR_SUPPORT
    MDrv_XC_Exit();
#endif
}

module_init(_MDrv_XC_ModuleInit);
module_exit(_MDrv_XC_ModuleExit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("XC ioctrl driver");
MODULE_LICENSE("GPL");

EXPORT_SYMBOL(_MDrv_XC_GetCFDSupportStatus);
