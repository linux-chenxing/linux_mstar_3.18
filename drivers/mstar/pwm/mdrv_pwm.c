/*
 * Copyright (C) ST-Ericsson SA 2010
 *
 * Author: Arun R Murthy <arun.murthy@stericsson.com>
 * License terms: GNU General Public License (GPL) version 2
 */
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/pwm.h>
#include <linux/module.h>
#include "mhal_pwm.h"

/* backlight driver constants */
#define ENABLE_PWM			1
#define DISABLE_PWM			0

static int mstar_pwm_config(struct pwm_chip *chip, struct pwm_device *pwm, int duty_ns, int period_ns)
{
    printk("mstar_pwm_config, duty_ns=%d, period_ns=%d\n", duty_ns, period_ns);
    DrvPWMSetPeriod(pwm->hwpwm,period_ns);
    DrvPWMSetDuty(pwm->hwpwm,duty_ns);
	return 0;
}

static int mstar_pwm_enable(struct pwm_chip *chip, struct pwm_device *pwm)
{
    printk("mstar_pwm_enable\n");
    DrvPWMEnable(pwm->hwpwm,ENABLE_PWM);
    //DrvPWMInit(ENABLE_PWM);
	return 0;
}


static void mstar_pwm_disable(struct pwm_chip *chip, struct pwm_device *pwm)
{
    printk("mstar_pwm_disable\n");
    DrvPWMEnable(pwm->hwpwm,DISABLE_PWM);
}

static int mstar_pwm_set_polarity(struct pwm_chip *chip, struct pwm_device *pwm, enum pwm_polarity polarity)
{
    printk("mstar_pwm_set_polarity=%d\n", (U32)polarity);
    DrvPWMSetPolarity(pwm->hwpwm,(U8)polarity);
	return 0;
}


static const struct pwm_ops mstar_pwm_ops = {
	.config = mstar_pwm_config,
	.enable = mstar_pwm_enable,
	.disable = mstar_pwm_disable,
	.set_polarity = mstar_pwm_set_polarity,
	.owner = THIS_MODULE,
};

static int ms_pwm_probe(struct platform_device *pdev)
{
	struct pwm_chip *mstar_pwm_chip;
	int err;

	mstar_pwm_chip = devm_kzalloc(&pdev->dev, sizeof(*mstar_pwm_chip), GFP_KERNEL);
	if (mstar_pwm_chip == NULL) {
		dev_err(&pdev->dev, "failed to allocate memory\n");
		return -ENOMEM;
	}

	mstar_pwm_chip->dev = &pdev->dev;
	mstar_pwm_chip->ops = &mstar_pwm_ops;
	mstar_pwm_chip->base = -1;
	mstar_pwm_chip->npwm = 4;

	err = pwmchip_add(mstar_pwm_chip);
	if (err < 0)
		return err;

	dev_info(&pdev->dev, "probe successful\n");
    platform_set_drvdata(pdev, mstar_pwm_chip);

	return 0;
}

static int ms_pwm_remove(struct platform_device *pdev)
{
	struct pwm_chip *mstar_pwm_chip = dev_get_drvdata(&pdev->dev);
	int err;

	err = pwmchip_remove(mstar_pwm_chip);
	if (err < 0)
		return err;

	dev_info(&pdev->dev, "remove successful\n");

	return 0;
}

static const struct of_device_id ms_pwm_of_match_table[] = {
    { .compatible = "mstar,infinity-pwm" },
    {}
};

MODULE_DEVICE_TABLE(of, ms_pwm_of_match_table);

static struct platform_driver ms_pwm_driver = {
    .remove = ms_pwm_remove,
    .probe = ms_pwm_probe,
    .driver = {
        .name = "ms_pwm_driver",
        .owner = THIS_MODULE,
        .of_match_table = ms_pwm_of_match_table,
    },
};

module_platform_driver(ms_pwm_driver);

MODULE_AUTHOR("MStar Semiconductor, Inc.");
MODULE_DESCRIPTION("MStar PWM Driver");
MODULE_LICENSE("GPL v2");
