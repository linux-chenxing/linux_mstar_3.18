/*
 * /sys/class/clk
 * usclk: usclk {
        compatible = "usclk";
        clocks = <&foo 15>, <&bar>;
        clock-count = <2>;
    };
*/
#include <linux/clk-provider.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/device.h>
#include <linux/slab.h>

#define DRIVER_NAME    "usclk"

struct usclk_data {
    struct clk_hw *hw;
    int enabled;
};

static ssize_t enable_show(struct device *dev, struct device_attribute *attr,
        char *buf)
{
    struct usclk_data *pdata = dev_get_drvdata(dev);

    if(!pdata->hw->init->ops->is_enabled)
        return scnprintf(buf, PAGE_SIZE, "%u\n", pdata->enabled);
    else
        return scnprintf(buf, PAGE_SIZE, "%u\n", pdata->hw->init->ops->is_enabled(pdata->hw));
}

static ssize_t enable_store(struct device *dev, struct device_attribute *attr,
        const char *buf, size_t count)
{
    unsigned long enable;
    int ret;
    struct usclk_data *pdata = dev_get_drvdata(dev);

    ret = kstrtoul(buf, 0, &enable);
    if (ret)
        return -EINVAL;

    enable = !!enable;

    if (enable)
        pdata->hw->init->ops->enable(pdata->hw);
    else
        pdata->hw->init->ops->disable(pdata->hw);

    pdata->enabled = enable;

    return count;
}

static DEVICE_ATTR(enable, 0644, enable_show, enable_store);

static ssize_t set_rate_show(struct device *dev, struct device_attribute *attr,
        char *buf)
{
    struct usclk_data *pdata = dev_get_drvdata(dev);

    return scnprintf(buf, PAGE_SIZE, "%lu\n", clk_get_rate(pdata->hw->clk));
}

static ssize_t set_rate_store(struct device *dev, struct device_attribute *attr,
        const char *buf, size_t count)
{
    int ret = 0;
    unsigned long rate;
    struct usclk_data *pdata = dev_get_drvdata(dev);

    ret = kstrtoul(buf, 0, &rate);
    if (ret)
    {
        return -EINVAL;
    }

    rate = clk_round_rate(pdata->hw->clk, rate);
    ret = clk_set_rate(pdata->hw->clk, rate);
    if (ret)
    {
        return -EBUSY;
    }
    else
    {
        pr_info("round rate to %lu\n", rate);
    }

    return count;
}

static DEVICE_ATTR(rate, 0644, set_rate_show, set_rate_store);

static const struct attribute *usclk_attrs[] = {
    &dev_attr_enable.attr,
    &dev_attr_rate.attr,
    NULL
};

static const struct attribute_group usclk_attr_grp = {
    .attrs = (struct attribute **)usclk_attrs,
};

static int usclk_setup(void)
{
    int ret;
    int i;
    struct usclk_data *pdata;
    u32 clock_count;
    struct class *clk_class;
    struct device *dev;
    struct device_node *np = of_find_compatible_node(NULL, NULL, "usclk");


//    printk(KERN_INFO"setup ms_usclk interface\n");
    ret = of_property_read_u32(np, "clock-count", &clock_count);
    if (ret || !clock_count)
        return ret;

    pdata = kzalloc(clock_count * sizeof(*pdata), GFP_KERNEL);
    if (!pdata)
        return -ENOMEM;

    clk_class = class_create(THIS_MODULE, "usclk");
    if (!clk_class) {
        pr_err("unable to create class\n");
        goto err_free;
    }

    for (i = 0; i < clock_count; i++) {
        pdata[i].hw = __clk_get_hw(of_clk_get(np, i));
        if (IS_ERR(pdata[i].hw)) {
            pr_warn("input clock #%u not found\n", i);
            continue;
        }

        dev = device_create(clk_class, NULL, MKDEV(0, 0), NULL,
                of_clk_get_parent_name(np, i));
        if (!dev) {
            pr_warn("unable to create device #%d\n", i);
            continue;
        }

        dev_set_drvdata(dev, &pdata[i]);
        if(0!=sysfs_create_group(&dev->kobj, &usclk_attr_grp))
        {
            pr_warn("create device #%d failed...\n", i);
        }
    }

    printk(KERN_WARNING "ms_usclk: initialized\n");
    return 0;

err_free:
    kfree(pdata);

    return ret;
}
//

#ifdef CONFIG_MS_USCLK_MODULE

static int __init ms_usclk_module_init(void)
{
//    int retval=0;
//    retval = platform_driver_register(&ms_ir_driver);

    return usclk_setup();
//    return retval;
}

static void __exit ms_usclk_module_exit(void)
{
//    platform_driver_unregis.ter(&ms_ir_driver);
}


module_init(ms_usclk_module_init);
module_exit(ms_usclk_module_exit);


MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("usclk driver");
MODULE_LICENSE("GPL");

#else
late_initcall(usclk_setup);
#endif
