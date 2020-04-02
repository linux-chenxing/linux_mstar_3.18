/*
 *  A mstar dummy rtc driver
 *  ToDo : Implement the real rtc...
 */

#include <linux/i2c.h>
#include <linux/bcd.h>
#include <linux/rtc.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/err.h>

#define IT7C4337_REG_SC		0x00 /* datetime */
#define IT7C4337_REG_MN		0x01
#define IT7C4337_REG_HR		0x02
#define IT7C4337_REG_DW		0x03
#define IT7C4337_REG_DM		0x04
#define IT7C4337_REG_MO		0x05
#define IT7C4337_REG_YR		0x06
#define IT7C4337_REG_AMN    0x07 /* alarm */
#define IT7C4337_REG_CTL    0x0E

#define IT7C4337_BIT_AIE	(1)
#define IT7C4337_BIT_AF		(1)
#define IT7C4337_BIT_INTCN  (4)

#define IT7C4337_MO_C		0x80 /* century */

#define RTC_DBG 1
#if RTC_DBG
    #define rtc_dbg(args...) printk(args)
#else
    #define rtc_dbg(args...)
#endif

///////////////////////////////////////////////////
static struct i2c_driver it7c4337_driver;

struct rtc_it7c4337 {
	struct rtc_device *rtc;
	int c_polarity;	/* 0: MO_C=1 means 19xx, otherwise MO_C=1 means 20xx */
	struct i2c_client *client;
};

static int it7c4337_read_block_data(struct i2c_client *client, unsigned char reg,
				   unsigned char length, unsigned char *buf)
{
	struct i2c_msg msgs[] = {
		{/* setup read ptr */
			.addr = client->addr,
			.len = 1,
			.buf = &reg,
		},
		{
			.addr = client->addr,
			.flags = I2C_M_RD,
			.len = length,
			.buf = buf
		},
	};

	if ((i2c_transfer(client->adapter, msgs, 2)) != 2) {
		dev_err(&client->dev, "%s: read error\n", __func__);
		return -EIO;
	}

	return 0;
}

static int it7c4337_write_block_data(struct i2c_client *client,
				   unsigned char reg, unsigned char length,
				   unsigned char *buf)
{
	int i, err;

	for (i = 0; i < length; i++) {
		unsigned char data[2] = { reg + i, buf[i] };

		err = i2c_master_send(client, data, sizeof(data));
		if (err != sizeof(data)) {
			dev_err(&client->dev,
				"%s: err=%d addr=%02x, data=%02x\n",
				__func__, err, data[0], data[1]);
			return -EIO;
		}
	}

	return 0;
}

static int it7c4337_set_alarm_mode(struct i2c_client *client, bool on)
{
	unsigned char buf[2];
	int err;

	err = it7c4337_read_block_data(client, IT7C4337_REG_CTL, 2, buf);
	if (err < 0)
		return err;

	if (on)
	{
		buf[0] |= IT7C4337_BIT_AIE;
		buf[0] |= IT7C4337_BIT_INTCN;
	}
	else
	{
		buf[0] &= ~IT7C4337_BIT_AIE;
		//buf[0] &= ~IT7C4337_BIT_INTCN;
	}

	buf[1] &= ~IT7C4337_BIT_AF;

	err = it7c4337_write_block_data(client, IT7C4337_REG_CTL, 2, buf);
	if (err < 0) {
		dev_err(&client->dev, "%s: write error\n", __func__);
		return -EIO;
	}

	return 0;
}

static int it7c4337_get_alarm_mode(struct i2c_client *client, unsigned char *en,
				  unsigned char *pen)
{
	unsigned char buf[2];
	int err;

	err = it7c4337_read_block_data(client, IT7C4337_REG_CTL, 2, buf);
	if (err)
		return err;

	rtc_dbg("%s: buf[0]=0x%02x, buf[1]=0x%02x \n", __func__, buf[0],buf[1]);

	if (en)
		*en = !!(buf[0] & IT7C4337_BIT_AIE);
	if (pen)
		*pen = !!(buf[1] & IT7C4337_BIT_AF);

	return 0;
}

static int it7c4337_get_datetime(struct i2c_client *client, struct rtc_time *tm)
{
	struct rtc_it7c4337 *it7c4337 = i2c_get_clientdata(client);
	unsigned char buf[9];
	int err;

	rtc_dbg("%s:%d\n", __func__, __LINE__);

	err = it7c4337_read_block_data(client, IT7C4337_REG_SC, 7, buf);
	if (err)
		return err;

	rtc_dbg("%s: raw data is sec=%02x, min=%02x, hr=%02x, "
		"wday=%02x, mday=%02x, mon=%02x, year=%02x\n",
		__func__,
		buf[0], buf[1], buf[2], buf[3],
		buf[4], buf[5], buf[6]);

	tm->tm_sec = bcd2bin(buf[IT7C4337_REG_SC] & 0x7F);
	tm->tm_min = bcd2bin(buf[IT7C4337_REG_MN] & 0x7F);
	tm->tm_hour = bcd2bin(buf[IT7C4337_REG_HR] & 0x3F); /* rtc hr 0-23 */
	tm->tm_wday = buf[IT7C4337_REG_DW] & 0x07;
	tm->tm_mday = bcd2bin(buf[IT7C4337_REG_DM] & 0x3F);
	tm->tm_mon = bcd2bin(buf[IT7C4337_REG_MO] & 0x1F) - 1; /* rtc mn 1-12 */
	tm->tm_year = bcd2bin(buf[IT7C4337_REG_YR]);
	if (tm->tm_year < 70)
		tm->tm_year += 100;	/* assume we are in 1970...2069 */
	/* detect the polarity heuristically. see note above. */
	it7c4337->c_polarity = (buf[IT7C4337_REG_MO] & IT7C4337_MO_C) ?
		(tm->tm_year >= 100) : (tm->tm_year < 100);

	rtc_dbg("%s: tm is secs=%d, mins=%d, hours=%d, "
		"mday=%d, mon=%d, year=%d, wday=%d\n",
		__func__,
		tm->tm_sec, tm->tm_min, tm->tm_hour,
		tm->tm_mday, tm->tm_mon, tm->tm_year, tm->tm_wday);

	/* the clock can give out invalid datetime, but we cannot return
	 * -EINVAL otherwise hwclock will refuse to set the time on bootup.
	 */
	if (rtc_valid_tm(tm) < 0)
		dev_err(&client->dev, "retrieved date/time is not valid.\n");

	return 0;
}

static int it7c4337_set_datetime(struct i2c_client *client, struct rtc_time *tm)
{
	struct rtc_it7c4337 *it7c4337 = i2c_get_clientdata(client);
	int err;
	unsigned char buf[9];

	rtc_dbg("%s: secs=%d, mins=%d, hours=%d, "
		"mday=%d, mon=%d, year=%d, wday=%d\n",
		__func__,
		tm->tm_sec, tm->tm_min, tm->tm_hour,
		tm->tm_mday, tm->tm_mon, tm->tm_year, tm->tm_wday);

	/* hours, minutes and seconds */
	buf[IT7C4337_REG_SC] = bin2bcd(tm->tm_sec);
	buf[IT7C4337_REG_MN] = bin2bcd(tm->tm_min);
	buf[IT7C4337_REG_HR] = bin2bcd(tm->tm_hour);

	buf[IT7C4337_REG_DM] = bin2bcd(tm->tm_mday);

	/* month, 1 - 12 */
	buf[IT7C4337_REG_MO] = bin2bcd(tm->tm_mon + 1);

	/* year and century */
	buf[IT7C4337_REG_YR] = bin2bcd(tm->tm_year % 100);
	if (it7c4337->c_polarity ? (tm->tm_year >= 100) : (tm->tm_year < 100))
		buf[IT7C4337_REG_MO] |= IT7C4337_MO_C;

	buf[IT7C4337_REG_DW] = tm->tm_wday & 0x07;

	err = it7c4337_write_block_data(client, IT7C4337_REG_SC,7, buf);
	if (err)
		return err;

	return 0;
}

#ifdef CONFIG_RTC_INTF_DEV
static int it7c4337_rtc_ioctl(struct device *dev, unsigned int cmd, unsigned long arg)
{
	//struct rtc_it7c4337 *it7c4337 = i2c_get_clientdata(to_i2c_client(dev));
    rtc_dbg("%s:%d\n", __func__, __LINE__);
	switch (cmd) {
	default:
		return -ENOIOCTLCMD;
	}
}
#else
#define it7c4337_rtc_ioctl NULL
#endif

static int it7c4337_rtc_read_time(struct device *dev, struct rtc_time *tm)
{
	rtc_dbg("%s:%d\n", __func__, __LINE__);
	return it7c4337_get_datetime(to_i2c_client(dev), tm);
}

static int it7c4337_rtc_set_time(struct device *dev, struct rtc_time *tm)
{
	rtc_dbg("%s:%d\n", __func__, __LINE__);
	return it7c4337_set_datetime(to_i2c_client(dev), tm);
}

static int it7c4337_rtc_read_alarm(struct device *dev, struct rtc_wkalrm *tm)
{
	struct i2c_client *client = to_i2c_client(dev);
	unsigned char buf[4];
	int err;

    rtc_dbg("%s:%d\n", __func__, __LINE__);

	err = it7c4337_read_block_data(client, IT7C4337_REG_AMN, 4, buf);
	if (err)
		return err;

	dev_dbg(&client->dev,
		"%s: raw data is sec=%02x, min=%02x, hr=%02x, mday=%02x\n",
		__func__, buf[0], buf[1], buf[2], buf[3]);

    tm->time.tm_sec = bcd2bin(buf[0] & 0x7F);
	tm->time.tm_min = bcd2bin(buf[1] & 0x7F);
	tm->time.tm_hour = bcd2bin(buf[2] & 0x3F);
	tm->time.tm_mday = bcd2bin(buf[3] & 0x3F);
	tm->time.tm_wday = -1;
	tm->time.tm_mon = -1;
	tm->time.tm_year = -1;
	tm->time.tm_yday = -1;
	tm->time.tm_isdst = -1;

	err = it7c4337_get_alarm_mode(client, &tm->enabled, &tm->pending);
	if (err < 0)
		return err;

	rtc_dbg("%s: tm is sec=%d, mins=%d, hours=%d, mday=%d "
		" enabled=%d, pending=%d\n", __func__, tm->time.tm_sec,
		tm->time.tm_min, tm->time.tm_hour, tm->time.tm_mday,
		tm->enabled, tm->pending);

	return 0;
}

static int it7c4337_rtc_set_alarm(struct device *dev, struct rtc_wkalrm *tm)
{
	struct i2c_client *client = to_i2c_client(dev);
	unsigned char buf[4] = {0};
	int err;

	rtc_dbg("%s, sec=%d,min=%d hour=%d mday=%d "
		"enabled=%d pending=%d\n", __func__,
		tm->time.tm_sec, tm->time.tm_min, tm->time.tm_hour,
		tm->time.tm_mday, tm->enabled, tm->pending);

	err = it7c4337_write_block_data(client, IT7C4337_REG_CTL, 2, buf);
	if (err)
		return err;

    buf[0] = bin2bcd(tm->time.tm_sec);
	buf[1] = bin2bcd(tm->time.tm_min);
	buf[2] = bin2bcd(tm->time.tm_hour);
	buf[3] = bin2bcd(tm->time.tm_mday) | 0x80;

	err = it7c4337_write_block_data(client, IT7C4337_REG_AMN, 4, buf);
	if (err)
		return err;

	return it7c4337_set_alarm_mode(client, 1);
}

static int it7c4337_irq_enable(struct device *dev, unsigned int enabled)
{
	rtc_dbg("%s:%d\n", __func__, __LINE__);
	return it7c4337_set_alarm_mode(to_i2c_client(dev), !!enabled);
}

static irqreturn_t it7c4337_irq(int irq, void *dev_id)
{
	struct rtc_it7c4337 *it7c4337 = i2c_get_clientdata(dev_id);
	int err;
	char pending;
    rtc_dbg("%s:%d", __func__, __LINE__);

	err = it7c4337_get_alarm_mode(it7c4337->client, NULL, &pending);
	if (err)
		return IRQ_NONE;

	if (pending) {
		rtc_update_irq(it7c4337->rtc, 1, RTC_IRQF | RTC_AF);
		it7c4337_set_alarm_mode(it7c4337->client, 1);
		return IRQ_HANDLED;
	}

	return IRQ_NONE;
}

static const struct rtc_class_ops it7c4337_rtc_ops = {
	.ioctl		= it7c4337_rtc_ioctl,
	.read_time	= it7c4337_rtc_read_time,
	.set_time	= it7c4337_rtc_set_time,
	.read_alarm	= it7c4337_rtc_read_alarm,
	.set_alarm	= it7c4337_rtc_set_alarm,
	.alarm_irq_enable = it7c4337_irq_enable,
};

static int it7c4337_probe(struct i2c_client *client,
				const struct i2c_device_id *id)
{
	struct rtc_it7c4337 *it7c4337;
	int err;

	rtc_dbg("%s:%d\n", __func__, __LINE__);

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C))
		return -ENODEV;

	it7c4337 = devm_kzalloc(&client->dev, sizeof(struct rtc_it7c4337),
				GFP_KERNEL);
	if (!it7c4337)
		return -ENOMEM;

	i2c_set_clientdata(client, it7c4337);
	it7c4337->client = client;
	device_set_wakeup_capable(&client->dev, 1);

	it7c4337->rtc = devm_rtc_device_register(&client->dev,
				it7c4337_driver.driver.name,
				&it7c4337_rtc_ops, THIS_MODULE);

	if (IS_ERR(it7c4337->rtc))
		return PTR_ERR(it7c4337->rtc);

	if (client->irq > 0) {
		err = devm_request_threaded_irq(&client->dev, client->irq,
				NULL, it7c4337_irq,
				IRQF_SHARED|IRQF_ONESHOT|IRQF_TRIGGER_FALLING,
				it7c4337->rtc->name, client);
		if (err) {
			dev_err(&client->dev, "unable to request IRQ %d\n",
								client->irq);
			return err;
		}
	}

	return 0;
}

static const struct i2c_device_id it7c4337_id[] = {
	{ "it7c4337", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, it7c4337_id);

#ifdef CONFIG_OF
static const struct of_device_id it7c4337_of_match[] = {
	{ .compatible = "rtc,it7c4337" },
	{}
};
MODULE_DEVICE_TABLE(of, it7c4337_of_match);
#endif

static struct i2c_driver it7c4337_driver = {
	.driver		= {
		.name	= "rtc-it7c43337",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(it7c4337_of_match),
	},
	.probe		= it7c4337_probe,
	.id_table	= it7c4337_id,
};

module_i2c_driver(it7c4337_driver);


MODULE_AUTHOR("Mstarsemi");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("IT7C4337 RTC driver");
