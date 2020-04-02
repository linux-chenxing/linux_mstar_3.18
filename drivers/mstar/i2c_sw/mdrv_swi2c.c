#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/of.h>
#include <linux/ctype.h>
#include <linux/i2c.h>


#include "mdrv_sw_iic.h"

#define I2C_BYTE_MASK	       0xFF
//#define I2C_DELAY_MODIFY

struct mstar_swi2c_dev {
	struct device *dev;
	struct i2c_adapter adapter;
	int cont_id;
	int i2cgroup;
};

#ifdef I2C_DELAY_MODIFY
static ssize_t swi2c_delay_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    char *str = buf;
    char *end = buf + PAGE_SIZE;

    str += scnprintf(str, end - str, "%d\n", gpioi2c_delay_us);
    return (str - buf);
}

static ssize_t swi2c_delay_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t n)
{
	size_t len;
    const char *str = buf;

    if(NULL!=buf)
    {
        while (*str && !isspace(*str)) str++;
        len = str - buf;
        if(len)
        {
            gpioi2c_delay_us = simple_strtoul(buf, NULL, 10);
            printk("\ngpioi2c_delay_us=%d\n", gpioi2c_delay_us);
            return n;
        }
        return -EINVAL;
    }

    return -EINVAL;
}

DEVICE_ATTR(swi2c_delay, 0644, swi2c_delay_show, swi2c_delay_store);

static struct attribute *swi2c_delay_attrs[] = {
	&dev_attr_swi2c_delay.attr,
	NULL
};

static struct attribute_group swi2c_delay_attr_group = {
	.attrs		= swi2c_delay_attrs,
};

static const struct attribute_group *swi2c_delay_attr_groups[] = {
	&swi2c_delay_attr_group,
	NULL
};
#endif

static int ms_sw_i2c_xfer_read(int port, struct i2c_msg *pmsg, u8 *pbuf, int length)
{
    if(!MDrv_SW_IIC_AccessStart(port, ((pmsg->addr & I2C_BYTE_MASK) << 1), 1))
	{
        printk(KERN_INFO "%s:%d send start failed\n", __func__, __LINE__);
        return -ETIMEDOUT;
    }

    while(length--)
	{
        *pbuf = MDrv_SW_IIC_GetByte(port, (U16)length);
        pbuf++;
    }
    return 0;
}

static int ms_sw_i2c_xfer_write(int port, struct i2c_msg *pmsg, u8 *pbuf, int length)
{
    if(!MDrv_SW_IIC_AccessStart(port, ((pmsg->addr & I2C_BYTE_MASK) << 1), 0))
	{
        printk(KERN_INFO "%s:%d send start failed\n", __func__, __LINE__);
        return -ETIMEDOUT;
    }

    while(length--)
	{
        if(MDrv_SW_IIC_SendByte(port, *pbuf, 0))
            pbuf++;
        else
            return -ETIMEDOUT;
    }

    return 0;
}

static int mdrv_swi2c_xfer(struct i2c_adapter *padap, struct i2c_msg *pmsg, int num)
{
    int i=0, err=0;
	struct mstar_swi2c_dev *i2c_dev;

    if (padap == NULL)
	{
        printk(KERN_INFO "%s:%d padap is null\n", __func__, __LINE__);
        return -ENOTTY;
    }

    if (pmsg == NULL)
	{
        printk(KERN_INFO "%s:%d pmsg is null\n", __func__, __LINE__);
        return -ENOTTY;
    }

	i2c_dev = i2c_get_adapdata(padap);

/* in i2c-master_send or recv, the num is always 1,  */
/* but use i2c_transfer() can set multiple message */
    for (i = 0; i < num; i++)
	{
#if 0
        int j=0;
        printk(KERN_INFO " #%d: %sing %d byte%s %s 0x%02x\n", i,
                pmsg->flags & I2C_M_RD ? "read" : "write",
                pmsg->len, pmsg->len > 1 ? "s" : "",
                pmsg->flags & I2C_M_RD ? "from" : "to", pmsg->addr);
        for(j = 0; j < pmsg->len; j++)
        {
            printk(KERN_INFO "%s:%d pmsg[%d]=0x%x\n", __func__, __LINE__, j, pmsg->buf[j]);
        }
#endif
        /* do Read/Write */


        if (pmsg->len && pmsg->buf)
		{
            if (pmsg->flags & I2C_M_RD)
                err = ms_sw_i2c_xfer_read(i2c_dev->cont_id, pmsg, pmsg->buf, pmsg->len);
            else
                err = ms_sw_i2c_xfer_write(i2c_dev->cont_id, pmsg, pmsg->buf, pmsg->len);
            if (err < 0)
			{
				MDrv_SW_IIC_Stop(i2c_dev->cont_id);
                return err;
            }
        }
        pmsg++;        /* next message */
    }

    MDrv_SW_IIC_Stop(i2c_dev->cont_id);

    return i;
}

static u32 mdrv_swi2c_func(struct i2c_adapter *padapter)
{
    return I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL;
}

/* implement the i2c transfer function in algorithm structure */
static struct i2c_algorithm mstar_swi2c_algorithm =
{
    .master_xfer = mdrv_swi2c_xfer,
    .functionality = mdrv_swi2c_func,
};

static const struct of_device_id mstar_swi2c_of_match[] = {
    { .compatible = "mstar,swi2c" },
    {}
};

static int mdrv_swi2c_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct I2C_BusCfg i2c_cfg;
	struct mstar_swi2c_dev *i2c_dev;
	int i2cgroup = 0, i2cscl = 0, i2csda = 0, i2cspeed = 0, port_idx = 0;
    printk("%s:%d\n", __func__, __LINE__);
    i2c_dev = devm_kzalloc(&pdev->dev, sizeof(*i2c_dev), GFP_KERNEL);
	if (!i2c_dev)
		return -ENOMEM;

	of_property_read_u32(pdev->dev.of_node, "i2c-group", &i2cgroup);
	of_property_read_u32(pdev->dev.of_node, "i2c-speed", &i2cspeed);
	of_property_read_u32(pdev->dev.of_node, "sda-gpio", &i2csda);
	of_property_read_u32(pdev->dev.of_node, "scl-gpio", &i2cscl);
	of_property_read_u32(pdev->dev.of_node, "port-idx", &port_idx);

	i2c_cfg.u8ChIdx  = port_idx;
	i2c_cfg.u8Enable = ENABLE;
	i2c_cfg.u8PadSCL = i2cscl;
	i2c_cfg.u8PadSDA = i2csda;
	i2c_cfg.u16SpeedKHz = i2cspeed;
    printk("%s:%d u8ChIdx=%d\n", __func__, __LINE__, i2c_cfg.u8ChIdx);

	MDrv_SW_IIC_ConfigBus(&i2c_cfg);

    i2c_dev->dev = &pdev->dev;
    i2c_dev->cont_id = port_idx;
	i2c_dev->i2cgroup = i2cgroup;

    platform_set_drvdata(pdev, i2c_dev);

    scnprintf(i2c_dev->adapter.name, sizeof(i2c_dev->adapter.name),
		 "Mstar swi2c adapter %d", i2cgroup);
	i2c_dev->adapter.owner = THIS_MODULE;
	i2c_dev->adapter.class = I2C_CLASS_DEPRECATED;
	i2c_dev->adapter.algo = &mstar_swi2c_algorithm;
	i2c_dev->adapter.dev.parent = &pdev->dev;
	i2c_dev->adapter.nr = i2cgroup;
	i2c_dev->adapter.dev.of_node = pdev->dev.of_node;
#ifdef I2C_DELAY_MODIFY
	i2c_dev->adapter.dev.groups = swi2c_delay_attr_groups;
#endif
	i2c_set_adapdata(&i2c_dev->adapter, i2c_dev);

	ret = i2c_add_numbered_adapter(&i2c_dev->adapter);
	if (ret) {
		dev_err(&pdev->dev, "Failed to add I2C adapter\n");
		return  ret;
    }
    printk("%s:%d\n", __func__, __LINE__);
	return 0;
}

static int mdrv_swi2c_remove(struct platform_device *pdev)
{
	struct mstar_swi2c_dev *i2c_dev = platform_get_drvdata(pdev);
	i2c_del_adapter(&i2c_dev->adapter);
	return 0;
}

#ifdef CONFIG_PM
static int mdrv_swi2c_suspend(struct platform_device *pdev, pm_message_t mesg)
{
    return 0;
}

static int mdrv_swi2c_resume(struct platform_device *pdev)
{
    /* initialize MIIC controller */
    return 0;
}
#endif  /* CONFIG_PM */

/* i2c driver sturcut, used to register to kernel */
static struct platform_driver mstar_swi2c_driver =
{
    .probe      = mdrv_swi2c_probe,
    .remove     = mdrv_swi2c_remove,
    #ifdef CONFIG_PM
    .suspend    = mdrv_swi2c_suspend,
    .resume     = mdrv_swi2c_resume,
    #endif
    .driver     =
    {
        .name   = "mstar_swi2c",
        .owner  = THIS_MODULE,
        .of_match_table = mstar_swi2c_of_match,
    },
};

static int __init mdrv_swi2c_init(void)
{
    return platform_driver_register(&mstar_swi2c_driver);
}

static void __exit mdrv_swi2c_exit(void)
{
    platform_driver_unregister(&mstar_swi2c_driver);
}

subsys_initcall(mdrv_swi2c_init);
module_exit(mdrv_swi2c_exit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("mstar swi2c driver");
MODULE_LICENSE("GPL");
