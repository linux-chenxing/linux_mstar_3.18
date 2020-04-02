////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    mdrv_iic.c
/// @brief  IIC Driver Interface
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
#include <linux/time.h>
#include <linux/timer.h>
#include <asm/io.h>
#include <linux/platform_device.h>

#if defined(CONFIG_COMPAT)
#include <linux/compat.h>
#endif



#include "mst_devid.h"

#include "mdrv_iic_io.h"
#include "mhal_iic_reg.h"
#include "mdrv_iic.h"
#include "mhal_iic.h"

//#include "mdrv_probe.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define IIC_MUTEX_WAIT_TIME         3000

#define IIC_DBG_ENABLE              0

#if IIC_DBG_ENABLE
#define IIC_DBG(_f)                 (_f)
#else
#define IIC_DBG(_f)
#endif

#if 0
#define LINE_DBG()                  printf("IIC %d\n", __LINE__)
#else
#define LINE_DBG()
#endif

#define IIC_PRINT(fmt, args...)		//printk("[IIC][%05d] " fmt, __LINE__, ## args)

typedef struct
{
    S32                         s32MajorIIC;
    S32                         s32MinorIIC;
    struct cdev                 cDevice;
    struct file_operations      IICFop;
    struct fasync_struct        *async_queue; /* asynchronous readers */
} IIC_ModHandle_t;


#define MOD_IIC_DEVICE_COUNT         1
#define MOD_IIC_NAME                 "ModIIC"


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------
// Forward declaration
//--------------------------------------------------------------------------------------------------
static int                      _MDrv_IIC_Open (struct inode *inode, struct file *filp);
static int                      _MDrv_IIC_Release(struct inode *inode, struct file *filp);
static ssize_t                  _MDrv_IIC_Read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
static ssize_t                  _MDrv_IIC_Write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);
static unsigned int             _MDrv_IIC_Poll(struct file *filp, poll_table *wait);
#ifdef HAVE_UNLOCKED_IOCTL
static long _MDrv_IIC_Ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
#else
static int _MDrv_IIC_Ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
#endif
#if defined(CONFIG_COMPAT)
static long Compat_MDrv_IIC_Ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
#endif

static int                      _MDrv_IIC_Fasync(int fd, struct file *filp, int mode);

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------

static IIC_ModHandle_t IICDev=
{
    .s32MajorIIC = MDRV_MAJOR_IIC,
    .s32MinorIIC = MDRV_MINOR_IIC,
    .cDevice =
    {
        .kobj = {.name= MOD_IIC_NAME, },
        .owner = THIS_MODULE,
    },
    .IICFop =
    {
        .open =     _MDrv_IIC_Open,
        .release =  _MDrv_IIC_Release,
        .read =     _MDrv_IIC_Read,
        .write =    _MDrv_IIC_Write,
        .poll =     _MDrv_IIC_Poll,
        #ifdef HAVE_UNLOCKED_IOCTL
        .unlocked_ioctl = _MDrv_IIC_Ioctl,
        #else        
        .ioctl = _MDrv_IIC_Ioctl,
        #endif
		#if defined(CONFIG_COMPAT)
		.compat_ioctl = Compat_MDrv_IIC_Ioctl,
		#endif
        .fasync =   _MDrv_IIC_Fasync,
    },
};
//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
#if defined(CONFIG_COMPAT)
int mdrv_iic_compat_get_ion_allaction_data(IIC_Param_Compat __user *data32, IIC_Param __user *data)
{
	IIC_Param_Compat temp;
	U8 *ptr_msg; 
	int err;



	err = get_user(temp.u8IdIIC, &data32->u8IdIIC);
	err = get_user(temp.u8ClockIIC, &data32->u8ClockIIC);
	err = get_user(temp.u8SlaveIdIIC, &data32->u8SlaveIdIIC);
	err = get_user(temp.u8AddrSizeIIC, &data32->u8AddrSizeIIC);
	
	err = get_user(temp.u8AddrIIC[0], &data32->u8AddrIIC[0]);
	err = get_user(temp.u8AddrIIC[1], &data32->u8AddrIIC[1]);
	err = get_user(temp.u8AddrIIC[2], &data32->u8AddrIIC[2]);
	err = get_user(temp.u8AddrIIC[3], &data32->u8AddrIIC[3]);
	err = get_user(ptr_msg, &data32->u8pbufIIC); 
	temp.u8pbufIIC = compat_ptr(ptr_msg); //(1) pointer transmit for 32bits to 64bits
	err = get_user(temp.u32DataSizeIIC, &data32->u32DataSizeIIC);

	err = put_user(temp.u8IdIIC, &data->u8IdIIC);
	err = put_user(temp.u8ClockIIC, &data->u8ClockIIC);
	err = put_user(temp.u8SlaveIdIIC, &data->u8SlaveIdIIC);
	err = put_user(temp.u8AddrSizeIIC, &data->u8AddrSizeIIC); //(2) lost : u8AddrSizeIIC convert
	err = put_user(temp.u8AddrIIC[0], &data->u8AddrIIC[0]);
	err = put_user(temp.u8AddrIIC[1], &data->u8AddrIIC[1]);
	err = put_user(temp.u8AddrIIC[2], &data->u8AddrIIC[2]);
	err = put_user(temp.u8AddrIIC[3], &data->u8AddrIIC[3]);
	err = put_user(temp.u8pbufIIC, &data->u8pbufIIC);
	err = put_user(temp.u32DataSizeIIC, &data->u32DataSizeIIC);

	return err;
}

int mdrv_iic_compat_put_ion_allaction_data(IIC_Param_Compat __user *data32, IIC_Param  __user *data, int rw_flag)
{
	IIC_Param temp;
	U8 *ptr_msg; 
	int err;
	int i;

	if (rw_flag > 0) // read iic
	{
		err = get_user(ptr_msg, &data->u8pbufIIC); 
		err = put_user(ptr_msg, &data32->u8pbufIIC);
	}
	return err;
}
#endif

static int _MDrv_IIC_Open (struct inode *inode, struct file *filp)
{
	IIC_ModHandle_t *dev;

    IIC_PRINT("%s is invoked\n", __FUNCTION__);

    dev = container_of(inode->i_cdev, IIC_ModHandle_t, cDevice);
	filp->private_data = dev;

    return 0;
}

static int _MDrv_IIC_Release(struct inode *inode, struct file *filp)
{
    IIC_PRINT("%s is invoked\n", __FUNCTION__);
    return 0;
}

static ssize_t _MDrv_IIC_Read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    U32     u32RetCountIIC = 0;
	IIC_Param_t IIC_ReadParam;
	U8        	pRdBuffer[IIC_RD_BUF_SIZE];

#if defined(CONFIG_COMPAT)
	IIC_Param_Compat __user *data32;
#endif
	IIC_Param __user *data;
	int ret = 0;

#if defined(CONFIG_COMPAT)
	if(is_compat_task())
        { //32bit
	
		data32 = compat_ptr(buf);
		data = compat_alloc_user_space(sizeof(*data));
		if (data == NULL)
			return -EFAULT;

		mdrv_iic_compat_get_ion_allaction_data(data32, data);
		if( copy_from_user(&IIC_ReadParam, (IIC_Param_t *)data, sizeof(IIC_Param_t)) ) //(3) "'count'" is old size with 32bits stuct , need using 64bits struct size 
		{
			return -1;
		}

#if 0
printk("\033[0;32;31m [Ian] IIC_ReadParam %s %d %d %d %d %d %d %d %d %d %p %d\033[m\n",__func__,__LINE__,
			IIC_ReadParam.u8IdIIC,
			IIC_ReadParam.u8ClockIIC,
			IIC_ReadParam.u8SlaveIdIIC,
			IIC_ReadParam.u8AddrSizeIIC,
			IIC_ReadParam.u8AddrIIC[0],
			IIC_ReadParam.u8AddrIIC[1],
			IIC_ReadParam.u8AddrIIC[2],
			IIC_ReadParam.u8AddrIIC[3],
			IIC_ReadParam.u8pbufIIC,
			IIC_ReadParam.u32DataSizeIIC);
#else
                 udelay(1000); //delay ok
                //no delay may read fail randomly and read data is always 0
#endif

		if( IIC_ReadParam.u32DataSizeIIC > IIC_RD_BUF_SIZE )
			IIC_ReadParam.u8pbufIIC = kmalloc(IIC_ReadParam.u32DataSizeIIC, GFP_KERNEL);
		else
			IIC_ReadParam.u8pbufIIC = pRdBuffer;

		if( IIC_ReadParam.u8pbufIIC == NULL )
		{
			return -1;
		}

		if( IIC_ReadParam.u8IdIIC >= IIC_NUM_OF_MAX )
		{
			printk("_MDrv_IIC_Read():ERROR: invalid IIC ID\n");
			if( IIC_ReadParam.u8pbufIIC != pRdBuffer )	kfree(IIC_ReadParam.u8pbufIIC);
			return(-1);
		}

		if( IIC_ReadParam.u8IdIIC <= IIC_NUM_OF_HW )
		{
			u32RetCountIIC = MDrv_HW_IIC_Read(IIC_ReadParam.u8SlaveIdIIC,IIC_ReadParam.u8AddrSizeIIC, IIC_ReadParam.u8AddrIIC, IIC_ReadParam.u32DataSizeIIC,IIC_ReadParam.u8pbufIIC);
		}
		else
		{
			u32RetCountIIC = MDrv_SW_IIC_Read(IIC_ReadParam.u8IdIIC,IIC_ReadParam.u8SlaveIdIIC,IIC_ReadParam.u8AddrSizeIIC, IIC_ReadParam.u8AddrIIC, IIC_ReadParam.u32DataSizeIIC,IIC_ReadParam.u8pbufIIC);
		}

	        IIC_DBG(printk("_MDrv_IIC_Read() -> u32RetCountIIC=%d\n", u32RetCountIIC));

		if (u32RetCountIIC)
		{
		int err;
		if( copy_to_user(data, &IIC_ReadParam, sizeof(IIC_Param_t)) )
                {
				printk("\033[0;32;31m [Ian] %s %d Error\033[m\n",__func__,__LINE__);
			}
			
		if( copy_to_user(data32->u8pbufIIC, data->u8pbufIIC, sizeof(U8)*u32RetCountIIC) )
                {
				printk("\033[0;32;31m [Ian] %s %d \033[m\n",__func__,__LINE__);
				return (-1);
			}
			printk("\033[0;32;31m [Ian] %s %d \033[m\n",__func__,__LINE__);
		}
		if( IIC_ReadParam.u8pbufIIC != pRdBuffer )	kfree(IIC_ReadParam.u8pbufIIC);

		return u32RetCountIIC;
	}
	else
#endif
        {
		if( copy_from_user(&IIC_ReadParam, (IIC_Param_t *)buf, sizeof(IIC_Param_t)) ) //(3) "'count'" is old size with 32bits stuct , need using 64bits struct size 
		{
			return -1;
		}

#if 0
                printk("\033[0;32;31m [Ian] IIC_ReadParam %s %d %d %d %d %d %d %d %d %d %p %d\033[m\n",__func__,__LINE__,
			IIC_ReadParam.u8IdIIC,
			IIC_ReadParam.u8ClockIIC,
			IIC_ReadParam.u8SlaveIdIIC,
			IIC_ReadParam.u8AddrSizeIIC,
			IIC_ReadParam.u8AddrIIC[0],
			IIC_ReadParam.u8AddrIIC[1],
			IIC_ReadParam.u8AddrIIC[2],
			IIC_ReadParam.u8AddrIIC[3],
			IIC_ReadParam.u8pbufIIC,
			IIC_ReadParam.u32DataSizeIIC);
#else
                udelay(1000); //delay ok
                //no delay may read fail randomly and read data is always 0
#endif


		if( IIC_ReadParam.u32DataSizeIIC > IIC_RD_BUF_SIZE )
			IIC_ReadParam.u8pbufIIC = kmalloc(IIC_ReadParam.u32DataSizeIIC, GFP_KERNEL);
		else
			IIC_ReadParam.u8pbufIIC = pRdBuffer;

		if( IIC_ReadParam.u8pbufIIC == NULL )
		{
			return -1;
		}

		if( IIC_ReadParam.u8IdIIC >= IIC_NUM_OF_MAX )
		{
			printk("_MDrv_IIC_Read():ERROR: invalid IIC ID\n");
			if( IIC_ReadParam.u8pbufIIC != pRdBuffer )	kfree(IIC_ReadParam.u8pbufIIC);
			return(-1);
		}

		if( IIC_ReadParam.u8IdIIC <= IIC_NUM_OF_HW )
		{
			u32RetCountIIC = MDrv_HW_IIC_Read(IIC_ReadParam.u8SlaveIdIIC,IIC_ReadParam.u8AddrSizeIIC, IIC_ReadParam.u8AddrIIC, IIC_ReadParam.u32DataSizeIIC,IIC_ReadParam.u8pbufIIC);
		}
		else
		{
			u32RetCountIIC = MDrv_SW_IIC_Read(IIC_ReadParam.u8IdIIC,IIC_ReadParam.u8SlaveIdIIC,IIC_ReadParam.u8AddrSizeIIC, IIC_ReadParam.u8AddrIIC, IIC_ReadParam.u32DataSizeIIC,IIC_ReadParam.u8pbufIIC);
		}

		if (u32RetCountIIC)
		{

	        if( copy_to_user(((IIC_Param_t *)buf)->u8pbufIIC, IIC_ReadParam.u8pbufIIC, sizeof(U8)*u32RetCountIIC) )
                {
				printk("\033[0;32;31m [Ian] %s %d \033[m\n",__func__,__LINE__);
				return (-1);
			}
		}
		if( IIC_ReadParam.u8pbufIIC != pRdBuffer )	kfree(IIC_ReadParam.u8pbufIIC);

		return u32RetCountIIC;
	}
}

static ssize_t _MDrv_IIC_Write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    U32     u32RetCountIIC = 0;
	IIC_Param_t IIC_WriteParam;
	U8        	pWrBuffer[IIC_WR_BUF_SIZE];

#if defined(CONFIG_COMPAT)
	if(is_compat_task())
        {
		IIC_Param_Compat __user *data32;
		IIC_Param __user *data;
		int ret = 0;

		data32 = compat_ptr(buf);
		data = compat_alloc_user_space(sizeof(*data));
		if (data == NULL)
			return -EFAULT;

		mdrv_iic_compat_get_ion_allaction_data(data32, data);

		if( copy_from_user(&IIC_WriteParam, (IIC_Param_t *)data, sizeof(IIC_Param_t)) )
		{
			return -1;
		}
		if( IIC_WriteParam.u32DataSizeIIC > IIC_WR_BUF_SIZE )
			IIC_WriteParam.u8pbufIIC = kmalloc(IIC_WriteParam.u32DataSizeIIC, GFP_KERNEL);
		else
			IIC_WriteParam.u8pbufIIC = pWrBuffer;

		if( IIC_WriteParam.u8pbufIIC == NULL )
		{
			return -1;
		}

		if( copy_from_user(IIC_WriteParam.u8pbufIIC, ((IIC_Param_Compat *)buf)->u8pbufIIC, IIC_WriteParam.u32DataSizeIIC) )
		{
			if( IIC_WriteParam.u8pbufIIC != pWrBuffer )	kfree(IIC_WriteParam.u8pbufIIC);
			return -1;
		}

		if( IIC_WriteParam.u8IdIIC >= IIC_NUM_OF_MAX )
		{
			if( IIC_WriteParam.u8pbufIIC != pWrBuffer )	kfree(IIC_WriteParam.u8pbufIIC);
			return(-1);
		}

		if( IIC_WriteParam.u8IdIIC <= IIC_NUM_OF_HW )
		{
			u32RetCountIIC = MDrv_HW_IIC_Write(IIC_WriteParam.u8SlaveIdIIC, IIC_WriteParam.u8AddrSizeIIC, IIC_WriteParam.u8AddrIIC, IIC_WriteParam.u32DataSizeIIC, IIC_WriteParam.u8pbufIIC);
		}
		else
		{
			u32RetCountIIC = MDrv_SW_IIC_Write(IIC_WriteParam.u8IdIIC, IIC_WriteParam.u8SlaveIdIIC, IIC_WriteParam.u8AddrSizeIIC, IIC_WriteParam.u8AddrIIC, IIC_WriteParam.u32DataSizeIIC, IIC_WriteParam.u8pbufIIC);
		}
		if( IIC_WriteParam.u8pbufIIC != pWrBuffer )	kfree(IIC_WriteParam.u8pbufIIC);

		return u32RetCountIIC;
	}
	else
#endif
        {
		if( copy_from_user(&IIC_WriteParam, (IIC_Param_t *)buf, count) )
		{
			return -1;
		}
		#if 0
		printk("u8IdIIC[%d] u8SlaveIdIIC[%x] u8ClockIIC[%d] u8AddrSizeIIC[%d] u32DataSizeIIC[%d]\n",\
			IIC_WriteParam.u8IdIIC,\
			IIC_WriteParam.u8SlaveIdIIC,\
			IIC_WriteParam.u8ClockIIC,\
			IIC_WriteParam.u8AddrSizeIIC,\
			IIC_WriteParam.u32DataSizeIIC);
		#endif
		if( IIC_WriteParam.u32DataSizeIIC > IIC_WR_BUF_SIZE )
			IIC_WriteParam.u8pbufIIC = kmalloc(IIC_WriteParam.u32DataSizeIIC, GFP_KERNEL);
		else
			IIC_WriteParam.u8pbufIIC = pWrBuffer;

		if( IIC_WriteParam.u8pbufIIC == NULL )
		{
			return -1;
		}

		if( copy_from_user(IIC_WriteParam.u8pbufIIC, ((IIC_Param_t *)buf)->u8pbufIIC, IIC_WriteParam.u32DataSizeIIC) )
		{
			if( IIC_WriteParam.u8pbufIIC != pWrBuffer )	kfree(IIC_WriteParam.u8pbufIIC);
			return -1;
		}

		if( IIC_WriteParam.u8IdIIC >= IIC_NUM_OF_MAX )
		{
			printk("_MDrv_IIC_Write():ERROR: invalid IIC ID\n");
			if( IIC_WriteParam.u8pbufIIC != pWrBuffer )	kfree(IIC_WriteParam.u8pbufIIC);
			return(-1);
		}

		if( IIC_WriteParam.u8IdIIC <= IIC_NUM_OF_HW )
		{
			u32RetCountIIC = MDrv_HW_IIC_Write(IIC_WriteParam.u8SlaveIdIIC, IIC_WriteParam.u8AddrSizeIIC, IIC_WriteParam.u8AddrIIC, IIC_WriteParam.u32DataSizeIIC, IIC_WriteParam.u8pbufIIC);
		}
		else
		{
			u32RetCountIIC = MDrv_SW_IIC_Write(IIC_WriteParam.u8IdIIC, IIC_WriteParam.u8SlaveIdIIC, IIC_WriteParam.u8AddrSizeIIC, IIC_WriteParam.u8AddrIIC, IIC_WriteParam.u32DataSizeIIC, IIC_WriteParam.u8pbufIIC);
		}
		if( IIC_WriteParam.u8pbufIIC != pWrBuffer )	kfree(IIC_WriteParam.u8pbufIIC);

		return u32RetCountIIC;

	}

}

static unsigned int _MDrv_IIC_Poll(struct file *filp, poll_table *wait)
{
    IIC_PRINT("%s is invoked\n", __FUNCTION__);
    return 0;
}


static int _MDrv_IIC_Fasync(int fd, struct file *filp, int mode)
{
    IIC_PRINT("%s is invoked\n", __FUNCTION__);

	return 0;//fasync_helper(fd, filp, mode, &IICDev.async_queue);
}

#if defined(CONFIG_COMPAT)
static long Compat_MDrv_IIC_Ioctl(struct file *filp, unsigned int cmd, unsigned long arg){
	int	err= 0;
	switch(cmd)
	{
        case Compat_MDRV_IIC_INIT:
        case Compat_MDRV_IIC_BUSCFG:
		{
			return filp->f_op->unlocked_ioctl(filp, cmd,(unsigned long)compat_ptr(arg));
		}
        case Compat_MDRV_IIC_CLOCK:
		{
			IIC_Param_Compat __user *data32;
			IIC_Param __user *data;

			data32 = compat_ptr(arg);
			data = compat_alloc_user_space(sizeof(*data));
			if (data == NULL)
				return -EFAULT;
			mdrv_iic_compat_get_ion_allaction_data(data32, data);
			return filp->f_op->unlocked_ioctl(filp, MDRV_IIC_CLOCK,(unsigned long)data);
		}
		case Compat_MDRV_IIC_ENABLE:
		{
			IIC_Param_Compat __user *data32;
			IIC_Param __user *data;

			data32 = compat_ptr(arg);
			data = compat_alloc_user_space(sizeof(*data));
			if (data == NULL)
				return -EFAULT;
			mdrv_iic_compat_get_ion_allaction_data(data32, data);

	
			return filp->f_op->unlocked_ioctl(filp, MDRV_IIC_ENABLE,(unsigned long)data);
		}
		default:
			printk("\033[0;32;31m %s %d Ioctl cmd %d undefined \033[m\n",__func__,__LINE__,cmd);
			return -ENOIOCTLCMD;
	}

}
#endif

#ifdef HAVE_UNLOCKED_IOCTL
static long _MDrv_IIC_Ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
#else
static int _MDrv_IIC_Ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
#endif
{
    S32 s32Err= 0;
	IIC_Param_t IIC_param;
    #if (defined(CONFIG_MSTAR_TITANIA)||defined(CONFIG_MSTAR_TITANIA2))
    #else
	IIC_BusCfg_t IIC_BusObj;
    #endif

    IIC_PRINT("%s is invoked\n", __FUNCTION__);

    /*
     * extract the type and number bitfields, and don't decode
     * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
     */
    if ((IIC_IOC_MAGIC!= _IOC_TYPE(cmd)) || (_IOC_NR(cmd)> IIC_IOC_MAXNR))
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
        s32Err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
    }
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
    {
        s32Err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
    }
    if (s32Err)
    {
        return -EFAULT;
    }


    //PROBE_IO_ENTRY(MDRV_MAJOR_IIC, _IOC_NR(cmd));

    switch(cmd)
    {
        case MDRV_IIC_INIT:
            IIC_PRINT("ioctl: MDRV_IIC_INIT\n");
            MDrv_IIC_Init();
            break;
        case MDRV_IIC_CLOCK:
            IIC_PRINT("ioctl: MDRV_IIC_CLOCK\n");
            if (copy_from_user(&IIC_param, (IIC_Param_t __user *)arg, sizeof(IIC_Param_t)))
            {
                //PROBE_IO_EXIT(MDRV_MAJOR_IIC, _IOC_NR(cmd));
 	            return -EFAULT;
            }

			if( IIC_param.u8IdIIC >= IIC_NUM_OF_MAX )
			{
			    //PROBE_IO_EXIT(MDRV_MAJOR_IIC, _IOC_NR(cmd));
				return -EFAULT;
			}

            if( IIC_param.u8IdIIC <= IIC_NUM_OF_HW )
			{
				MDrv_HW_IIC_Clock_Select(IIC_param.u8ClockIIC);
			}
			else
			{
				MDrv_SW_IIC_SetSpeed(IIC_param.u8IdIIC, IIC_param.u8ClockIIC);
			}
            break;

    #if (defined(CONFIG_MSTAR_TITANIA)||defined(CONFIG_MSTAR_TITANIA2))
    #else
		case MDRV_IIC_ENABLE:
			IIC_PRINT("ioctl: MDRV_IIC_ENABLE\n");
			if (copy_from_user(&IIC_param, (IIC_Param_t __user *)arg, sizeof(IIC_Param_t)))
			{
			    //PROBE_IO_EXIT(MDRV_MAJOR_IIC, _IOC_NR(cmd));
				return -EFAULT;
            }
			if(( IIC_param.u8IdIIC <= IIC_NUM_OF_HW )||( IIC_param.u8IdIIC >= IIC_NUM_OF_MAX ))
			{
                IIC_PRINT("SW I2C Channel is BTW (%d) ~ %d\n",IIC_NUM_OF_HW+1,IIC_NUM_OF_MAX-1);
			    //PROBE_IO_EXIT(MDRV_MAJOR_IIC, _IOC_NR(cmd));
				return -EFAULT;
			}
            IIC_PRINT("MDRV_IIC_ENABLE: SW I2C Channel = %d\n",IIC_param.u8IdIIC);
            IIC_PRINT("MDRV_IIC_ENABLE: enable = %d\n",IIC_param.u8ClockIIC);
            MDrv_SW_IIC_Enable( IIC_param.u8IdIIC, ((IIC_param.u8ClockIIC) ? TRUE : FALSE) );
			break;

            case MDRV_IIC_BUSCFG:
            IIC_PRINT("ioctl: MDRV_IIC_BUSCFG\n");
            if (copy_from_user(&IIC_BusObj, (IIC_BusCfg_t __user *)arg, sizeof(IIC_BusCfg_t)))
            {
                //PROBE_IO_EXIT(MDRV_MAJOR_IIC, _IOC_NR(cmd));
 	            return -EFAULT;
            }
            IIC_PRINT("SW I2C u8ChIdx --> is %d\n",IIC_BusObj.u8ChIdx);
			if(( IIC_BusObj.u8ChIdx <= IIC_NUM_OF_HW )||( IIC_BusObj.u8ChIdx >= IIC_NUM_OF_MAX ))
			{
                IIC_PRINT("SW I2C Channel is BTW (%d) ~ %d\n",IIC_NUM_OF_HW+1,IIC_NUM_OF_MAX-1);
			    //PROBE_IO_EXIT(MDRV_MAJOR_IIC, _IOC_NR(cmd));
				return -EFAULT;
			}
            MDrv_SW_IIC_ConfigBus((I2C_BusCfg_t*)&IIC_BusObj);
            break;
    #endif
    
        default:
            IIC_PRINT("ioctl: unknown command\n");
            //PROBE_IO_EXIT(MDRV_MAJOR_IIC, _IOC_NR(cmd));
            return -ENOTTY;
    }
    //PROBE_IO_EXIT(MDRV_MAJOR_IIC, _IOC_NR(cmd));
    return 0;
}


static int mod_iic_init(void)
{
    S32         s32Ret;
    dev_t       dev;

    printk("%s is invoked\n", __FUNCTION__);

    if (IICDev.s32MajorIIC)
    {
        dev = MKDEV(IICDev.s32MajorIIC, IICDev.s32MinorIIC);
        s32Ret = register_chrdev_region(dev, MOD_IIC_DEVICE_COUNT, MOD_IIC_NAME);
    }
    else
    {
        s32Ret = alloc_chrdev_region(&dev, IICDev.s32MinorIIC, MOD_IIC_DEVICE_COUNT, MOD_IIC_NAME);
        IICDev.s32MajorIIC = MAJOR(dev);
    }

    if ( 0 > s32Ret)
    {
        IIC_PRINT("Unable to get major %d\n", IICDev.s32MajorIIC);
        return s32Ret;
    }

    cdev_init(&IICDev.cDevice, &IICDev.IICFop);
    if (0!= (s32Ret= cdev_add(&IICDev.cDevice, dev, MOD_IIC_DEVICE_COUNT)))
    {
        IIC_PRINT("Unable add a character device\n");
        unregister_chrdev_region(dev, MOD_IIC_DEVICE_COUNT);
        return s32Ret;
    }

    return 0;
}

static void mod_iic_exit(void)
{
    IIC_PRINT("%s is invoked\n", __FUNCTION__);

    cdev_del(&IICDev.cDevice);
    unregister_chrdev_region(MKDEV(IICDev.s32MajorIIC, IICDev.s32MinorIIC), MOD_IIC_DEVICE_COUNT);
}

static int mstar_iic_drv_suspend(struct platform_device *dev, pm_message_t state)
{
    return 0;
}
static int mstar_iic_drv_resume(struct platform_device *dev)
{
    return 0;
}

static int mstar_iic_drv_probe(struct platform_device *pdev)
{
    int retval=0;
    IIC_PRINT("[ki2c] %s\n",__FUNCTION__);
    if( !(pdev->name) || strcmp(pdev->name,"Mstar-iic")
        || pdev->id!=0)
    {
        retval = -ENXIO;
    }

    //retval = mod_iic_init();
    if(!retval)
    {
        pdev->dev.platform_data=&IICDev;
    }
	return retval;
}

static int mstar_iic_drv_remove(struct platform_device *pdev)
{
    if( !(pdev->name) || strcmp(pdev->name,"Mstar-iic")
        || pdev->id!=0)
    {
        return -1;
    }

    mod_iic_exit();
    pdev->dev.platform_data=NULL;
    return 0;
}


#if defined (CONFIG_ARM64)
static struct of_device_id mstariic_of_device_ids[] = {
         {.compatible = "mstar-iic"},
         {},
};
#endif

static struct platform_driver Mstar_iic_driver = {
	.probe 		= mstar_iic_drv_probe,
	.remove 	= mstar_iic_drv_remove,
    .suspend    = mstar_iic_drv_suspend,
    .resume     = mstar_iic_drv_resume,

	.driver = {
#if defined(CONFIG_ARM64)
                .of_match_table = mstariic_of_device_ids,
#endif
		.name	= "Mstar-iic",
        .owner  = THIS_MODULE,
	}
};


static int __init mstar_iic_drv_init_module(void)
{
    int retval=0;
    IIC_PRINT("[ki2c] %s\n",__FUNCTION__);
    retval = mod_iic_init();
    retval = platform_driver_register(&Mstar_iic_driver);
    return retval;
}

static void __exit mstar_iic_drv_exit_module(void)
{
    platform_driver_unregister(&Mstar_iic_driver);
}


module_init(mstar_iic_drv_init_module);
module_exit(mstar_iic_drv_exit_module);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("IIC driver");
MODULE_LICENSE("GPL");
