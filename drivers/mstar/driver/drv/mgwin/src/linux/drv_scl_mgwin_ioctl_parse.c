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
#define __DRV_SCL_MGWIN_IOCTL_PARSE_C__

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
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
// Define & Macro
//-------------------------------------------------------------------------------------------------
#define CMD_PARSING(x)  (x==IOCTL_SCLMGWIN_SET_FRAME_CONFIG            ?   "IOCTL_SCLMGWIN_SET_FRAME_CONFIG" : \
                         x==IOCTL_SCLMGWIN_SET_FRAME_ONOFF_CONFIG      ?   "IOCTL_SCLMGWIN_SET_FRAME_ONOFF_CONFIG" : \
                         x==IOCTL_SCLMGWIN_SET_SUBWIN_CONFIG           ?   "IOCTL_SCLMGWIN_SET_SUBWIN_CONFIG" : \
                         x==IOCTL_SCLMGWIN_SET_SUBWIN_FLIP_CONFIG      ?   "IOCTL_SCLMGWIN_SET_SUBWIN_FLIP_CONFIG" :\
                         x==IOCTL_SCLMGWIN_SET_SUBWIN_ONOFF_CONFIG     ?   "IOCTL_SCLMGWIN_SET_SUBWIN_ONOFF_CONFIG" :\
                         x==IOCTL_SCLMGWIN_SET_DBF_CONFIG              ?   "IOCTL_SCLMGWIN_SET_DBF_CONFIG" :\
                         x==IOCTL_SCLMGWIN_SET_FRAME_PATH_CONFIG       ?   "IOCTL_SCLMGWIN_SET_FRAME_PATH_CONFIG" :\
                         x==IOCTL_SCLMGWIN_GET_DBF_CONFIG              ?   "IOCTL_SCLMGWIN_GET_DBF_CONFIG" :\
                         x==IOCTL_SCLMGWIN_SET_REGISTER_CONFIG         ?   "IOCTL_SCLMGWIN_SET_REGISTER_CONFIG" :\
                                                                           "UNKNOWN")


//-------------------------------------------------------------------------------------------------
// Variable
//-------------------------------------------------------------------------------------------------
extern DrvSclMgwinIoFunctionConfig_t _gstSclMgwinIoFunc;


//-------------------------------------------------------------------------------------------------
// Pirvate Function
//-------------------------------------------------------------------------------------------------
int _DrvSclMgwinIoctlTransErrType(DrvSclMgwinIoErrType_e eErrType)
{
    int ret;

    switch(eErrType)
    {
        case E_DRV_SCLMGWIN_IO_ERR_OK:
            ret = 0;
            break;
        case E_DRV_SCLMGWIN_IO_ERR_INVAL:
            ret = -EINVAL;
            break;
        case E_DRV_SCLMGWIN_IO_ERR_MULTI:
            ret = -EFAULT;
            break;

        default:
        case E_DRV_SCLMGWIN_IO_ERR_FAULT:
            ret = -EFAULT;
            break;
    }
    return ret;
}

int _DrvSclMgwinIoctlSetFrameConfig(struct file *filp, unsigned long arg)
{
    DrvSclMgwinIoFrameConfig_t stIoCfg;

    if(_gstSclMgwinIoFunc.DrvSclMgwinIoSetFrameConfig == NULL)
    {
        SCL_ERR("%s %d: Null Function\n", __FUNCTION__, __LINE__);
        return -EFAULT;
    }

    if(copy_from_user(&stIoCfg, (DrvSclMgwinIoFrameConfig_t __user *)arg, sizeof(DrvSclMgwinIoFrameConfig_t)))
    {
        return -EFAULT;
    }
    else
    {
        DrvSclMgwinIoErrType_e eErrType;
        s32 s32Handler = *((s32 *)filp->private_data);

        eErrType = _gstSclMgwinIoFunc.DrvSclMgwinIoSetFrameConfig(s32Handler, &stIoCfg);
        return _DrvSclMgwinIoctlTransErrType(eErrType);
    }
}

int _DrvSclMgwinIoctlSetFrameOnOffConfig(struct file *filp, unsigned long arg)
{
    DrvSclMgwinIoFrameOnOffConfig_t stIoCfg;

    if(_gstSclMgwinIoFunc.DrvSclMgwinIoSetFrameOnOffConfig == NULL)
    {
        SCL_ERR("%s %d: Null Function\n", __FUNCTION__, __LINE__);
        return -EFAULT;
    }

    if(copy_from_user(&stIoCfg, (DrvSclMgwinIoFrameOnOffConfig_t __user *)arg, sizeof(DrvSclMgwinIoFrameOnOffConfig_t)))
    {
        return -EFAULT;
    }
    else
    {
        DrvSclMgwinIoErrType_e eErrType;
        s32 s32Handler = *((s32 *)filp->private_data);

        eErrType = _gstSclMgwinIoFunc.DrvSclMgwinIoSetFrameOnOffConfig(s32Handler, &stIoCfg);
        return _DrvSclMgwinIoctlTransErrType(eErrType);
    }
}


int _DrvSclMgwinIoctlSetSubWinConfig(struct file *filp, unsigned long arg)
{
    DrvSclMgwinIoSubWinConfig_t stIoCfg;

    if(_gstSclMgwinIoFunc.DrvSclMgwinIoSetSubWinConfig == NULL)
    {
        SCL_ERR("%s %d: Null Function\n", __FUNCTION__, __LINE__);
        return -EFAULT;
    }

    if(copy_from_user(&stIoCfg, (DrvSclMgwinIoSubWinConfig_t __user *)arg, sizeof(DrvSclMgwinIoSubWinConfig_t)))
    {
        return -EFAULT;
    }
    else
    {
        DrvSclMgwinIoErrType_e eErrType;
        s32 s32Handler = *((s32 *)filp->private_data);

        eErrType = _gstSclMgwinIoFunc.DrvSclMgwinIoSetSubWinConfig(s32Handler, &stIoCfg);
        return _DrvSclMgwinIoctlTransErrType(eErrType);
    }
}


int _DrvSclMgwinIoctlSetSubWinOnOffConfig(struct file *filp, unsigned long arg)
{
    DrvSclMgwinIoSubWinOnOffConfig_t stIoCfg;

    if(_gstSclMgwinIoFunc.DrvSclMgwinIoSetSubWinOnOffConfig == NULL)
    {
        SCL_ERR("%s %d: Null Function\n", __FUNCTION__, __LINE__);
        return -EFAULT;
    }

    if(copy_from_user(&stIoCfg, (DrvSclMgwinIoSubWinOnOffConfig_t __user *)arg, sizeof(DrvSclMgwinIoSubWinOnOffConfig_t)))
    {
        return -EFAULT;
    }
    else
    {
        DrvSclMgwinIoErrType_e eErrType;
        s32 s32Handler = *((s32 *)filp->private_data);

        eErrType = _gstSclMgwinIoFunc.DrvSclMgwinIoSetSubWinOnOffConfig(s32Handler, &stIoCfg);
        return _DrvSclMgwinIoctlTransErrType(eErrType);
    }
}
int _DrvSclMgwinIoctlSetSubWinFlipConfig(struct file *filp, unsigned long arg)
{
    DrvSclMgwinIoSubWinFlipConfig_t stIoCfg;

    if(_gstSclMgwinIoFunc.DrvSclMgwinIoSetSubWinFlipConfig == NULL)
    {
        SCL_ERR("%s %d: Null Function\n", __FUNCTION__, __LINE__);
        return -EFAULT;
    }

    if(copy_from_user(&stIoCfg, (DrvSclMgwinIoSubWinFlipConfig_t __user *)arg, sizeof(DrvSclMgwinIoSubWinFlipConfig_t)))
    {
        return -EFAULT;
    }
    else
    {
        DrvSclMgwinIoErrType_e eErrType;
        s32 s32Handler = *((s32 *)filp->private_data);

        eErrType = _gstSclMgwinIoFunc.DrvSclMgwinIoSetSubWinFlipConfig(s32Handler, &stIoCfg);
        return _DrvSclMgwinIoctlTransErrType(eErrType);
    }
}



int _DrvSclMgwinIoctlSetDbfFireConfig(struct file *filp, unsigned long arg)
{
    DrvSclMgwinIoDbfConfig_t stIoCfg;

    if(_gstSclMgwinIoFunc.DrvSclMgwinIoSetDbfConfig == NULL)
    {
        SCL_ERR("%s %d: Null Function\n", __FUNCTION__, __LINE__);
        return -EFAULT;
    }

    if(copy_from_user(&stIoCfg, (DrvSclMgwinIoDbfConfig_t __user *)arg, sizeof(DrvSclMgwinIoDbfConfig_t)))
    {
        return -EFAULT;
    }
    else
    {
        DrvSclMgwinIoErrType_e eErrType;
        s32 s32Handler = *((s32 *)filp->private_data);

        eErrType = _gstSclMgwinIoFunc.DrvSclMgwinIoSetDbfConfig(s32Handler, &stIoCfg);
        return _DrvSclMgwinIoctlTransErrType(eErrType);
    }
}

int _DrvSclMgwinIoctlGetDbfFireConfig(struct file *filp, unsigned long arg)
{
    DrvSclMgwinIoDbfConfig_t stIoCfg;

    if(_gstSclMgwinIoFunc.DrvSclMgwinIoGetDbfConfig == NULL)
    {
        SCL_ERR("%s %d: Null Function\n", __FUNCTION__, __LINE__);
        return -EFAULT;
    }

    if(copy_from_user(&stIoCfg, (DrvSclMgwinIoDbfConfig_t __user *)arg, sizeof(DrvSclMgwinIoDbfConfig_t)))
    {
        return -EFAULT;
    }
    else
    {
        DrvSclMgwinIoErrType_e eErrType;
        s32 s32Handler = *((s32 *)filp->private_data);

        eErrType = _gstSclMgwinIoFunc.DrvSclMgwinIoGetDbfConfig(s32Handler, &stIoCfg);

        if(copy_to_user((DrvSclMgwinIoDbfConfig_t __user *)arg, &stIoCfg, sizeof(DrvSclMgwinIoDbfConfig_t)))
        {
            return -EFAULT;
        }
        else
        {
            return _DrvSclMgwinIoctlTransErrType(eErrType);
        }
    }
}

int _DrvSclMgwinIoctlSetFramePathConfig(struct file *filp, unsigned long arg)
{
    DrvSclMgwinIoFramePathConfig_t stIoCfg;

    if(_gstSclMgwinIoFunc.DrvSclMgwinIoSetFramePathConfig == NULL)
    {
        SCL_ERR("%s %d: Null Function\n", __FUNCTION__, __LINE__);
        return -EFAULT;
    }

    if(copy_from_user(&stIoCfg, (DrvSclMgwinIoFramePathConfig_t __user *)arg, sizeof(DrvSclMgwinIoFramePathConfig_t)))
    {
        return -EFAULT;
    }
    else
    {
        DrvSclMgwinIoErrType_e eErrType;
        s32 s32Handler = *((s32 *)filp->private_data);

        eErrType = _gstSclMgwinIoFunc.DrvSclMgwinIoSetFramePathConfig(s32Handler, &stIoCfg);
        return _DrvSclMgwinIoctlTransErrType(eErrType);
    }
}

int _DrvSclMgwinIoctlSetRegsiterConfig(struct file *filp, unsigned long arg)
{
    DrvSclMgwinIoRegisterConfig_t stIoCfg;

    if(_gstSclMgwinIoFunc.DrvSclMgwinIoSetRegisterConfig == NULL)
    {
        SCL_ERR("%s %d: Null Function\n", __FUNCTION__, __LINE__);
        return -EFAULT;
    }

    if(copy_from_user(&stIoCfg, (DrvSclMgwinIoRegisterConfig_t __user *)arg, sizeof(DrvSclMgwinIoRegisterConfig_t)))
    {
        return -EFAULT;
    }
    else
    {
        DrvSclMgwinIoErrType_e eErrType;
        s32 s32Handler = *((s32 *)filp->private_data);

        eErrType = _gstSclMgwinIoFunc.DrvSclMgwinIoSetRegisterConfig(s32Handler, &stIoCfg);
        return _DrvSclMgwinIoctlTransErrType(eErrType);
    }
}

//-------------------------------------------------------------------------------------------------
// Public Function
//-------------------------------------------------------------------------------------------------
int DrvSclMgwinIoctlParse(struct file *filp, unsigned int u32Cmd, unsigned long u32Arg)
{
    int retval = 0;

    SCL_DBG(SCL_DBG_LV_IOCTL(), "[MGWIN] IOCTL_NUM:: == %s ==  \n", (CMD_PARSING(u32Cmd)));

    switch(u32Cmd)
    {
        case IOCTL_SCLMGWIN_SET_FRAME_CONFIG:
            retval = _DrvSclMgwinIoctlSetFrameConfig(filp, u32Arg);
            break;

        case IOCTL_SCLMGWIN_SET_FRAME_ONOFF_CONFIG:
            retval = _DrvSclMgwinIoctlSetFrameOnOffConfig(filp, u32Arg);
            break;

        case IOCTL_SCLMGWIN_SET_SUBWIN_CONFIG:
            retval = _DrvSclMgwinIoctlSetSubWinConfig(filp, u32Arg);
            break;

        case IOCTL_SCLMGWIN_SET_SUBWIN_FLIP_CONFIG:
            retval = _DrvSclMgwinIoctlSetSubWinFlipConfig(filp, u32Arg);
            break;

        case IOCTL_SCLMGWIN_SET_SUBWIN_ONOFF_CONFIG:
            retval = _DrvSclMgwinIoctlSetSubWinOnOffConfig(filp, u32Arg);
            break;

        case IOCTL_SCLMGWIN_SET_DBF_CONFIG:
            retval = _DrvSclMgwinIoctlSetDbfFireConfig(filp, u32Arg);
            break;

        case IOCTL_SCLMGWIN_SET_FRAME_PATH_CONFIG:
            retval = _DrvSclMgwinIoctlSetFramePathConfig(filp, u32Arg);
            break;

        case IOCTL_SCLMGWIN_GET_DBF_CONFIG:
            retval = _DrvSclMgwinIoctlGetDbfFireConfig(filp, u32Arg);
            break;

        case IOCTL_SCLMGWIN_SET_REGISTER_CONFIG:
            break;

        default:  /* redundant, as cmd was checked against MAXNR */
            SCL_ERR( "[MGWIN] ERROR IOCtl number %x\n ",u32Cmd);
            retval = -ENOTTY;
            break;
    }

    return retval;
}

//==============================================================================

