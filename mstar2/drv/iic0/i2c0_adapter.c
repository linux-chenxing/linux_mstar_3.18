/******************************************************************************
 *-----------------------------------------------------------------------------
 *
 *  Copyright (c) 2011 MStar Semiconductor, Inc.  All rights reserved.
 *
 *-----------------------------------------------------------------------------
 * FILE NAME      ms_i2c.c
 * DESCRIPTION    For Columbus2 LinuxBSP I2C master driver, support Generic I2C
 *                protocol
 *
 *
 * HISTORY        2011/04/18     initial version
 ******************************************************************************/

#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
//#include <mach/io.h>
//#include <mach/ms_types.h>
#include "mdrv_iic0.h"
#include <linux/delay.h>

//#define msi2cDbg  printk
#define msi2cDbg(...)

#define I2C_BYTE_MASK	0xFF

#define HWI2C_DBG_ERR(x, args...)      printk(x, ##args)

//#else
//#define swi2cDbg(...)
//#endif

static struct i2c_board_info i2c_devs0[]  = {
	{ I2C_BOARD_INFO("24c16", 0x50), },
};

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

/*
+------------------------------------------------------------------------------
| FUNCTION    : ms_i2c_xfer_read
+------------------------------------------------------------------------------
| DESCRIPTION : This function is called by ms_i2c_xfer,
|                     used to read data from i2c bus
|           1. send start
|           2. send address + R (read bit), and wait ack
|           3. just set start_byte_read,
|              loop
|           4. wait interrupt is arised, then clear interrupt and read byte in
|           5. Auto generate NACK by IP,
|           6. the master does not acknowledge the final byte it receives.
|              This tells the slave that its transmission is done
|
| RETURN      : When the operation is success, it return 0.
|               Otherwise Negative number will be returned.
|
+------------------------------------------------------------------------------
| Variable Name      |IN |OUT|                   Usage
|--------------------+---+---+-------------------------------------------------
| pmsg               | x |   | pass in the slave id (addr) and R/W flag
|--------------------+---+---+-------------------------------------------------
| pbuf               | x |   | the message buffer, the buffer used to fill
|                    |   |   | data readed
|--------------------+---+---+-------------------------------------------------
| length             | x |   | the byte to be readed from slave
+------------------------------------------------------------------------------
*/
static int
ms_i2c_xfer_read(struct i2c_msg *pmsg, u8 *pbuf, int length)
{
    BOOL ret = FALSE;
    u32 u32i = 0;
    U8 u8Port = 0;
    U16 u16Offset = 0x00;
    u8Port = 0; //HWI2C_PORT0;
   
   if (NULL == pmsg)
   {
	   printk(KERN_INFO
			 "ERROR: in ms_i2c_xfer_read: pmsg is NULL pointer \r\n");
	   return -ENOTTY;
   }
   if (NULL == pbuf)
   {
	   printk(KERN_INFO
			 "ERROR: in ms_i2c_xfer_read: pbuf is NULL pointer \r\n");
	   return -ENOTTY;
   }

   //configure port register offset ==> important
    if(!_MDrv_HWI2C0_GetPortRegOffset(u8Port,&u16Offset))
    {
        HWI2C_DBG_ERR("Port index error!\n");
        return FALSE;
    }

   /* ***** 1. Send start bit ***** */
   if(!MDrv_HWI2C0_Start(u16Offset))
   {
	   printk(KERN_INFO
			 "ERROR: in ms_i2c_xfer_read: Send Start error \r\n");
       return -ETIMEDOUT;

   }
   // Delay for 1 SCL cycle 10us -> 4000T
   udelay(5);
   //LOOP_DELAY(8000); //20us

   /* ***** 2. Send slave id + read bit ***** */

   if (!MDrv_HWI2C0_Send_Byte(u16Offset, ((pmsg->addr & I2C_BYTE_MASK) << 1) |
				   ((pmsg->flags & I2C_M_RD) ? 1 : 0)))
    {
        printk(KERN_INFO
              "ERROR: in ms_i2c_xfer_read: Send slave id error \r\n");
        return -ETIMEDOUT;
    }

   /* Read data */
   for (u32i = 0; u32i < length; u32i++)
   {
	  /* ***** 6. Read byte data from slave ***** */
	   if ((length-1) == u32i)
	   {
		   MDrv_HWI2C0_NoAck(u16Offset);
	   }
	   ret = MDrv_HWI2C0_Recv_Byte(u16Offset, pbuf);
	   pbuf++;
   }



    return 0;

}

/*
+------------------------------------------------------------------------------
| FUNCTION    : ms_i2c_xfer_write
+------------------------------------------------------------------------------
| DESCRIPTION : This function is called by ms_i2c_xfer
|               used to write data to i2c bus the procedure is as following

|           1. send start
|           2. send address, and wait ack and clear interrupt in wait_ack()
|           loop
|           3. send byte
|           4. wait interrupt is arised, then clear interrupt
|                             and check if recieve ACK
|
| RETURN      : When the operation is success, it return 0.
|               Otherwise Negative number will be returned.
|
+------------------------------------------------------------------------------
| Variable Name      |IN |OUT|                   Usage
|--------------------+---+---+-------------------------------------------------
| pmsg               | x |   | pass in the slave id (addr) and R/W flag
|--------------------+---+---+-------------------------------------------------
| pbuf                | x |   | the message buffer, the buffer used to fill
|                    |   |   | data readed
|--------------------+---+---+-------------------------------------------------
| length             | x |   | the byte to be writen from slave
+------------------------------------------------------------------------------
*/
static int
ms_i2c_xfer_write(struct i2c_msg *pmsg, u8 *pbuf, int length)
{
    u32 u32i = 0;
    U8 u8Port = 0;
    U16 u16Offset = 0x00;
    int ret = FALSE;
    u8Port = 0;


    if (NULL == pmsg)
    {
        printk(KERN_INFO
              "ERROR: in ms_i2c_xfer_write: pmsg is NULL pointer \r\n");
        return -ENOTTY;
    }
    if (NULL == pbuf)
    {
        printk(KERN_INFO
              "ERROR: in ms_i2c_xfer_write: pbuf is NULL pointer \r\n");
        return -ENOTTY;
    }

    //configure port register offset ==> important
    if(!_MDrv_HWI2C0_GetPortRegOffset(u8Port,&u16Offset))
    {
        HWI2C_DBG_ERR("Port index error!\n");
        return FALSE;
    }

    /* ***** 1. Send start bit ***** */
    if(!MDrv_HWI2C0_Start(u16Offset))
    {
        printk(KERN_INFO
              "ERROR: in ms_i2c_xfer_write: Send Start error \r\n");
        return -ETIMEDOUT;
    }
    // Delay for 1 SCL cycle 10us -> 4000T
    //LOOP_DELAY(8000); //20us
    udelay(5);
    
    /* ***** 2. Send slave id + read bit ***** */
     if (!MDrv_HWI2C0_Send_Byte(u16Offset, ((pmsg->addr & I2C_BYTE_MASK) << 1) |
                    ((pmsg->flags & I2C_M_RD) ? 1 : 0)))
     {
         printk(KERN_INFO
               "ERROR: in ms_i2c_xfer_write: Send slave id error \r\n");

         return -ETIMEDOUT;
     }

    /* ***** 3. Send register address and data to write ***** */
    /* we send register is first buffer */
    for (u32i = 0; u32i < length; u32i++)
    {
        /* ***** 4. Write high byte data to slave ***** */
        if(MDrv_HWI2C0_Send_Byte(u16Offset, *pbuf))
        {
            pbuf++;
        }
        else
        {
            return -ETIMEDOUT;
        }

    }
        MDrv_HWI2C0_Reset(u16Offset,TRUE);
        MDrv_HWI2C0_Reset(u16Offset,FALSE);

    return ret;
}

/*
+------------------------------------------------------------------------------
| FUNCTION    : ms_i2c_xfer
+------------------------------------------------------------------------------
| DESCRIPTION : This function will be called by i2c-core.c i2c-transfer()
|               i2c_master_send(), and i2c_master_recv()
|               We implement the I2C communication protocol here
|               Generic i2c master transfer entrypoint.
|
| RETURN      : When the operation is success, it return the number of message
|               requrested. Negative number when error occurs.
|
+------------------------------------------------------------------------------
| Variable Name      |IN |OUT|                   Usage
|--------------------+---+---+-------------------------------------------------
| padap               | x |   | the adaptor which the communication will be
|                    |   |   | procceed
|--------------------+---+---+-------------------------------------------------
| pmsg               | x |   | the message buffer, the buffer with message to
|                    |   |   | be sent or used to fill data readed
|--------------------+---+---+-------------------------------------------------
| num                | x |   | number of message to be transfer
+------------------------------------------------------------------------------
*/
static int
ms_i2c_xfer(struct i2c_adapter *padap, struct i2c_msg *pmsg, int num)
{
    int i, err;
    U8 u8Port = 0;
    U16 u16Offset = 0x00;

#if 1
    printk(KERN_INFO "ms_i2c_xfer: processing %d messages:\n", num);
#endif

    i = 0;
    err = 0;

    if (NULL == padap)
    {
        printk(KERN_INFO
                "ERROR: in ms_i2c_xfer: adap is NULL pointer \r\n");
        return -ENOTTY;
    }
    if (NULL == pmsg)
    {
        printk(KERN_INFO
                "ERROR: in ms_i2c_xfer: pmsg is NULL pointer \r\n");
        return -ENOTTY;
    }

/* in i2c-master_send or recv, the num is always 1,  */
/* but use i2c_transfer() can set multiple message */

#if 0
for (i = 0; i < num; i++)
{
    printk(KERN_INFO " #%d: %sing %d byte%s %s 0x%02x\n", i,
            pmsg->flags & I2C_M_RD ? "read" : "writ",
            pmsg->len, pmsg->len > 1 ? "s" : "",
            pmsg->flags & I2C_M_RD ? "from" : "to", pmsg->addr);
            pmsg++;
}
#endif
//    for (i = 0; i < 40; i++)
//        udelay(100);

    for (i = 0; i < num; i++)
    {
#if 0
        printk(KERN_INFO " #%d: %sing %d byte%s %s 0x%02x\n", i,
                pmsg->flags & I2C_M_RD ? "read" : "writ",
                pmsg->len, pmsg->len > 1 ? "s" : "",
                pmsg->flags & I2C_M_RD ? "from" : "to", pmsg->addr);
#endif
        /* do Read/Write */
        if (pmsg->len && pmsg->buf) /* sanity check */
        {
            if (pmsg->flags & I2C_M_RD)
                {
                //return;
                err = ms_i2c_xfer_read(pmsg, pmsg->buf, pmsg->len);
                }
            else
                err = ms_i2c_xfer_write(pmsg, pmsg->buf, pmsg->len);
            if (err)
                return err;

        }
        pmsg++;        /* next message */
    }

    /* ***** 6. Send stop bit ***** */
    /* finish the read/write, then issues the stop condition (P).
     * for repeat start, diposit stop, two start and one stop only
     */

    //configure port register offset ==> important
    if(!_MDrv_HWI2C0_GetPortRegOffset(u8Port,&u16Offset))
    {
        HWI2C_DBG_ERR("Port index error!\n");
        return FALSE;
    }

    MDrv_HWI2C0_Stop(u16Offset);
    // MDrv_SW_IIC_Stop(BusCfg[0].u8ChIdx);
    //mdrv_i2c_send_stop();
    MDrv_HWI2C0_Reset(u16Offset,TRUE);
    MDrv_HWI2C0_Reset(u16Offset,FALSE);

    return i;
}


/*
+------------------------------------------------------------------------------
| FUNCTION    : ms_i2c_func
+------------------------------------------------------------------------------
| DESCRIPTION : This function is returned list of supported functionality.
|
| RETURN      : return list of supported functionality
|
| Variable    : no variable
+------------------------------------------------------------------------------
*/
static u32 ms_i2c_func(struct i2c_adapter *padapter)
{
    return I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL;
}

/* implement the i2c transfer function in algorithm structure */
static struct i2c_algorithm sg_ms_i2c_algorithm =
{
    .master_xfer = ms_i2c_xfer,
    .functionality = ms_i2c_func,
};

/*
+------------------------------------------------------------------------------
| FUNCTION    : ms_i2c_probe
+------------------------------------------------------------------------------
| DESCRIPTION : Main initialization routine. it fill adaptor table,
|               allocate memory which module needed, and call HW init function,
|               at finally, use i2c_add_numbered_adapter(adapter) to register
|               adaptor to kernel
|
| RETURN      : When 0, the operation is success. Negative number
|               when error occurs.
|
+------------------------------------------------------------------------------
| Variable Name      |IN |OUT|                   Usage
|--------------------+---+---+-------------------------------------------------
| pdev               | x |   | struct platform_device,
+------------------------------------------------------------------------------
*/
static int  ms_i2c_probe(struct platform_device *pdev)
{
    struct i2c_adapter *padapter;
    int err;

    err = 0;
    if (NULL == pdev)
    {
        printk(KERN_INFO
            "ERROR: in ms_i2c_probe: pdev is NULL pointer \r\n");
        return -ENOTTY;
    }

    /* allocate memory and set it zero */
    padapter = kzalloc(sizeof(struct i2c_adapter), GFP_KERNEL);
    if (NULL == padapter)
    {
        dev_err(&pdev->dev, "can't allocate inteface!\n");
        err = -ENOMEM;
        goto FAIL1;
    }
    sprintf(padapter->name, "HW-I2C");
    /* this will register to kernel name, we can see it by
     * cat /sys/class/i2c-dev/i2c-0/name
     */

    padapter->algo = &sg_ms_i2c_algorithm;
    padapter->class = I2C_CLASS_HWMON;
    padapter->dev.parent = &pdev->dev;
    /* If the I2C adapter lives on a type of device that has a struct device
     * associated with it, such as a PCI or USB device, then before the call
     * to i2c_add_adapter, the adapter device's parent pointer should be set
     * to that device.
     */

    platform_set_drvdata(pdev, padapter);

    /* initialize MIIC controller */
    MDrv_HW_IIC0_Init();

    err = i2c_add_numbered_adapter(padapter);
    /* i2c_add_numbered_adapter - declare i2c adapter, use static bus number
     * This routine is used to declare an I2C adapter when its bus number
     * matters.  Example: for I2C adapters from system-on-chip CPUs.
     */

    if (err)
    {
        printk(KERN_INFO "Adapter %s registration failed\n",
            padapter->name);
        goto FAIL2;
    }

    return 0;

FAIL2:
    platform_set_drvdata(pdev, NULL);
    if(NULL != padapter)
        kfree(padapter);

FAIL1:
    return err;
}

/*
+------------------------------------------------------------------------------
| FUNCTION    : ms_i2c_remove
+------------------------------------------------------------------------------
| DESCRIPTION : Main release routine. it release resource which it allocated
|               unregister adapter from kernel
|
| RETURN      : When 0, the operation is success. Negative number
|               when error occurs.
|
+------------------------------------------------------------------------------
| Variable Name      |IN |OUT|                   Usage
|--------------------+---+---+-------------------------------------------------
| pdev               | x |   | struct platform_device,
+------------------------------------------------------------------------------
*/
static int ms_i2c_remove(struct platform_device *pdev)
{
    struct i2c_adapter *padapter;
    int err;

    err = 0;
    if(NULL == pdev)
    {
        printk(KERN_INFO
                "ERROR: in ms_i2c_remove: pdev is NULL pointer \r\n");
        return -ENOTTY;
    }

    padapter = platform_get_drvdata(pdev);

    if(NULL != padapter)
        i2c_del_adapter(padapter);

    platform_set_drvdata(pdev, NULL);

    if(NULL != padapter)
        kfree(padapter);

    return err;
}

#ifdef CONFIG_PM

/*
+------------------------------------------------------------------------------
| FUNCTION    : ms_i2c_suspend, ms_i2c_resume
+------------------------------------------------------------------------------
| DESCRIPTION : for saving few mA by keeping clock off,
|               and resume the functionality,
|
| RETURN      : When 0, the operation is success. Negative number
|               when error occurs.
|
+------------------------------------------------------------------------------
| Variable Name      |IN |OUT|                   Usage
|--------------------+---+---+-------------------------------------------------
| pdev               | x |   | struct platform_device,
|--------------------+---+---+-------------------------------------------------
| mesg               | x |   | message from power management module
+------------------------------------------------------------------------------
*/
static
int ms_i2c_suspend(struct platform_device *pdev, pm_message_t mesg)
{
    return 0;
}

static int ms_i2c_resume(struct platform_device *pdev)
{
    /* initialize MIIC controller */
    MDrv_HW_IIC0_Init();
    return 0;
}

#else /* CONFIG_PM */
#define ms_i2c_suspend      NULL
#define ms_i2c_resume       NULL
#endif  /* CONFIG_PM */

/* work with "modprobe ms_i2c" from hotplugging or coldplugging */
MODULE_ALIAS("hw_i2c");

/* i2c driver sturcut, used to register to kernel */
static struct platform_driver sg_ms_i2c_driver =
{
    .probe      = ms_i2c_probe,
    .remove     = ms_i2c_remove,
    .suspend    = ms_i2c_suspend,
    .resume   = ms_i2c_resume,
    .driver     =
    {
        .name   = "hw_i2c",
        .owner  = THIS_MODULE,
    },
};

/* i2c device sturcut, used to register to kernel */
static struct platform_device sg_ms_i2c_device =
{
    .name = "hw_i2c",
    .id = 0,
    .dev =
    {
        .coherent_dma_mask = 0xffffffffUL
    }
};

/*
+------------------------------------------------------------------------------
| FUNCTION    : ms_i2c_init
+------------------------------------------------------------------------------
| DESCRIPTION : This function is the entry function, when ms_i2c module
|               loaded
|
| RETURN      : When 0, the operation is success. Negative number
|               when error occurs.
|
| Variable      : no variable
+------------------------------------------------------------------------------
*/
static int __init ms_i2c_init(void)
{
    int ret = 0;
    i2c_register_board_info(0, i2c_devs0, ARRAY_SIZE(i2c_devs0));

    printk(KERN_INFO "ms_i2c_init\n");
    ret = platform_driver_register(&sg_ms_i2c_driver);
    if (!ret)
    {
        /* register driver sucess */
        ret = platform_device_register(&sg_ms_i2c_device);
        /* if do not register a device, the probe() won't be called */

        if (ret)    /* if register device fail, then unregister the driver. */
            platform_driver_unregister(&sg_ms_i2c_driver);
    }
    return ret;
}

/*
+------------------------------------------------------------------------------
| FUNCTION    : ms_i2c_exit
+------------------------------------------------------------------------------
| DESCRIPTION : This function is the exit function,
|               the function is called when module is unload
|
| RETURN      : No return value
|
| Variable      : no variable
+------------------------------------------------------------------------------
*/
static void __exit ms_i2c_exit(void)
{
    platform_driver_unregister(&sg_ms_i2c_driver);
}

subsys_initcall(ms_i2c_init);
module_exit(ms_i2c_exit);

MODULE_AUTHOR("Mstar");
MODULE_DESCRIPTION("MSTAR HW I2C driver");
MODULE_LICENSE("GPL");
