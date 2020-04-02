/*
 * Dummy Charger driver for test RT1716
 *
 * Copyright (c) 2012 Marvell International Ltd.
 * Author:	Jeff Chang <jeff_chagn@mrichtek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/power_supply.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/usb/tcpci.h>
#include <linux/workqueue.h>
#include <linux/usb/tcpm.h>

int test_flag;
struct rt_charger_info {
	struct device *dev;
	struct power_supply chg;
	struct notifier_block nb;
	struct tcpc_device *tcpc;
	struct delayed_work dwork;
	int vbus_gpio;
	u8 status;
	u8 online:1;
};

int tcpc_check_vsafe0v(struct tcpc_device *tcpc)
{
	pr_info("%s !!!!!!!!!!Please inpement check vsafe0v function !!!!!!!!!\n", __func__);
	return 0;
}

static int chg_enable_vbus(struct rt_charger_info *info, int enable)
{
	//pd_dbg_info("%s enable = %d\n", __func__, enable);
	gpio_set_value(info->vbus_gpio, enable);
	info->status = enable ?
		POWER_SUPPLY_STATUS_CHARGING : POWER_SUPPLY_STATUS_DISCHARGING;
	return 0;
}

static int rt_chg_handle_source_vbus(struct tcp_notify *tcp_noti, int enable)
{
	struct power_supply *chg;
	struct tcpc_device *tcpc;
	union power_supply_propval val;
	
	chg = power_supply_get_by_name("rt-chg");
	if (!chg) {
		pr_err("%s: no rt-charger psy\n", __func__);
		return -ENODEV;
	}

	val.intval = enable;
	chg->set_property(chg, POWER_SUPPLY_PROP_ONLINE, &val);

	tcpc = tcpc_dev_get_by_name("type_c_port0");
	if (!tcpc)
		return -EINVAL;

	return 0;
}

static int chg_get_prop(struct power_supply *psy,
		enum power_supply_property psp,
		union power_supply_propval *val)
{
	struct rt_charger_info *info =
		dev_get_drvdata(psy->dev->parent);

	switch (psp) {
	case POWER_SUPPLY_PROP_STATUS:
		val->intval = info->status;
		break;
	case POWER_SUPPLY_PROP_ONLINE:
		val->intval = info->online;
		break;
	default:
		return -ENODEV;
	}
	return 0;
}

static int chg_set_prop(struct power_supply *psy,
		enum power_supply_property psp,
		const union power_supply_propval *val)
{
	struct rt_charger_info *info =
		dev_get_drvdata(psy->dev->parent);

	switch (psp) {
	case POWER_SUPPLY_PROP_STATUS:
		break;
	case POWER_SUPPLY_PROP_ONLINE:
		info->online = val->intval;
		chg_enable_vbus(info, info->online);
		break;
	default:
		return -ENODEV;
	};
	return 0;
}

static enum power_supply_property rt_chg_props[] = {
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_ONLINE,
};

static int rtchg_init_vbus(struct rt_charger_info *info)
{
#ifdef CONFIG_OF
	struct device_node *np = info->dev->of_node;
	int ret;

	if (np == NULL) {
		pr_err("Error: rt-changer np = NULL\n");
		return -1;
	}

	info->vbus_gpio = of_get_named_gpio(np, "rt,vbus_gpio", 0);
	ret = gpio_request(info->vbus_gpio, "DUMMY CHG VBUS CONTRL");
	if (ret < 0) {
		pr_err("Error: failed to request GPIO %d\n", info->vbus_gpio);
		return ret;
	}
	ret = gpio_direction_output(info->vbus_gpio, 0);
	if (ret < 0) {
		pr_err("Error: failed to set GPIO as output pin\n");
		return ret;
	}
#endif /* CONFIG_OF */

	pr_info("%s: OK\n", __func__);
	return 0;
}

/* Please handle the notification in notifier call function,
 * User should control the Power here when you got SOURCE_VBUS notification
 * and SINK_VBUS notification
 */
static int chg_tcp_notifer_call(struct notifier_block *nb,
				unsigned long event, void *data)
{
	struct tcp_notify *tcp_noti = data;

	switch (event) {
	case TCP_NOTIFY_PR_SWAP:
	case TCP_NOTIFY_DR_SWAP:
	case TCP_NOTIFY_VCONN_SWAP:
		/* Do what you want to do here */
		break;
	case TCP_NOTIFY_DIS_VBUS_CTRL:
		/* Implement disable power path (otg & charger) behavior here */
		rt_chg_handle_source_vbus(tcp_noti, 0);
		break;
	case TCP_NOTIFY_SOURCE_VBUS:
		/* Implement source vbus behavior here */
		rt_chg_handle_source_vbus(
			tcp_noti, (tcp_noti->vbus_state.mv > 0) ? 1 : 0);
		break;
	case TCP_NOTIFY_SINK_VBUS:
		/* Implement sink vubs behavior here */
		break;
	default:
		break;
	};

	return NOTIFY_OK;
}

void test_set_flag(int en)
{
	test_flag = en;
}

void dwork_func(struct work_struct *work)
{
	struct rt_charger_info *info =
		container_of(work, struct rt_charger_info, dwork.work);

	if (test_flag)
		pr_info("WHATWHATWHATWHATWHATWHATWWWWWWWHATHATHATHATHATHATHAT\n");

	schedule_delayed_work(&info->dwork, msecs_to_jiffies(10));
}

static int rt_charger_probe(struct platform_device *pdev)
{
	struct rt_charger_info *info;
	int ret;

	pr_info("%s\n", __func__);
	info = devm_kzalloc(&pdev->dev, sizeof(*info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	info->dev = &pdev->dev;
	platform_set_drvdata(pdev, info);

	info->chg.name = "rt-chg";
	info->chg.type = POWER_SUPPLY_TYPE_UNKNOWN;
	info->chg.properties = rt_chg_props;
	info->chg.num_properties = ARRAY_SIZE(rt_chg_props);
	info->chg.get_property = chg_get_prop;
	info->chg.set_property = chg_set_prop;
	ret = power_supply_register(&pdev->dev, &info->chg);
	if (ret < 0) {
		dev_err(&pdev->dev, "chg register fail\n");
		return -EINVAL;
	}

	ret = rtchg_init_vbus(info);
	if (ret < 0) {
		pr_err("%s gpio init fail\n", __func__);
		return -EINVAL;
	}

	/* Get tcpc device by tcpc_device'name */
	info->tcpc = tcpc_dev_get_by_name("rt1716");
	if (!info->tcpc) {
		dev_err(&pdev->dev, "get rt1716-tcpc fail\n");
		power_supply_unregister(&info->chg);
		return -ENODEV;
	}

	/* register tcpc notifier */
	info->nb.notifier_call = chg_tcp_notifer_call;
	ret = register_tcp_dev_notifier(info->tcpc, &info->nb);
	if (ret < 0) {
		dev_err(&pdev->dev, "register tcpc notifer fail\n");
		return -EINVAL;
	}

	INIT_DELAYED_WORK(&info->dwork, dwork_func);

	schedule_delayed_work(&info->dwork, 0);

	pr_info("%s: OK!\n", __func__);
	return 0;
}

static int rt_charger_remove(struct platform_device *pdev)
{
	struct rt_charger_info *info = platform_get_drvdata(pdev);

	power_supply_unregister(&info->chg);
	return 0;
}

static struct of_device_id rt_match_table[] = {
	{.compatible = "richtek,rt-charger",},
};

static struct platform_driver rt_charger_driver = {
	.driver = {
		.name = "rt-charger",
		.owner = THIS_MODULE,
		.of_match_table = rt_match_table,
	},
	.probe = rt_charger_probe,
	.remove = rt_charger_remove,
};

static int __init rt_chg_init(void)
{
	return platform_driver_register(&rt_charger_driver);
}

static void __exit rt_chg_exit(void)
{
	platform_driver_unregister(&rt_charger_driver);
}
late_initcall(rt_chg_init);
module_exit(rt_chg_exit);

MODULE_DESCRIPTION("Dummy Charger driver");
MODULE_LICENSE("GPL");
