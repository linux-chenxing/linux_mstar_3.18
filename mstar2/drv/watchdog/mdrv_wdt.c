#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/timer.h>
#include <linux/miscdevice.h> /* for MODULE_ALIAS_MISCDEV */
#include <linux/watchdog.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/clk.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/cpufreq.h>
#include <linux/slab.h>
#include <linux/err.h>

#include "mdrv_wdt.h"

//#define OPEN_WDT_DEBUG 1
#ifdef OPEN_WDT_DEBUG //switch printk
#define wdtDbg  printk
#else
#define wdtDbg(...)
#endif
#define OPTIONS (WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING | WDIOF_MAGICCLOSE)
#define CONFIG_WATCHDOG_DEFAULT_TIME	        (10)
#define REPEAT_DELAY   250        ////4s   250HZ

static int tmr_margin	= CONFIG_WATCHDOG_DEFAULT_TIME;
static bool nowayout	= WATCHDOG_NOWAYOUT;

extern U32 OALReadChipRevision(void);
static DEFINE_SPINLOCK(wdt_lock);

U32 g_u32Clock;
struct timer_list wdt_timer;
U8 watchdog_open_flag=0;
static unsigned long next_heartbeat;
U32 TempNum;
#define RIU_MAP               0xfd000000
#define REG16(_u32RegBase)     *((volatile U16*)(RIU_MAP+((_u32RegBase)<<1)))

static void _Wachdog_Write2Byte(U32 u32RegBase, U16 data)
{
	REG16(u32RegBase) = data;
}

void check_osc_clk(void)
{
    g_u32Clock = OSC_CLK_12000000;
}

static void __k6lite_wdt_stop(void)
{
    wdtDbg("__k6lite_wdt_stop \n");
    _Wachdog_Write2Byte(BASE_REG_WDT_PA + WDT_WDT_CLR, CLEAR_WDT);
    _Wachdog_Write2Byte(BASE_REG_WDT_PA + WDT_MAX_PRD_H, 0x0000);
    _Wachdog_Write2Byte(BASE_REG_WDT_PA + WDT_MAX_PRD_L, 0x0000);
}

static int k6lite_wdt_stop(struct watchdog_device *wdd)
{
    wdtDbg("[WatchDog]k6lite_wdt_stop \n");

	spin_lock(&wdt_lock);
    del_timer(&wdt_timer);
	__k6lite_wdt_stop();
	watchdog_open_flag=0;
	spin_unlock(&wdt_lock);

	return 0;
}

static void __k6lite_wdt_start(U32 u32LaunchTim)
{
    wdtDbg("__k6lite_wdt_start \n");
    _Wachdog_Write2Byte(BASE_REG_WDT_PA + WDT_WDT_CLR, CLEAR_WDT);
    _Wachdog_Write2Byte(BASE_REG_WDT_PA + WDT_MAX_PRD_H, (((g_u32Clock*u32LaunchTim)>>16) & 0xFFFF));
    _Wachdog_Write2Byte(BASE_REG_WDT_PA + WDT_MAX_PRD_L, ((g_u32Clock*u32LaunchTim) & 0xFFFF));
	wdtDbg("1111111111__k6lite_wdt_start =%lx\n",(g_u32Clock*u32LaunchTim));

}

static int k6lite_wdt_start(struct watchdog_device *wdd)
{
    unsigned long j;
    wdtDbg("[WatchDog]clipper_wdt_start \n");
	spin_lock(&wdt_lock);
	__k6lite_wdt_stop();
    __k6lite_wdt_start(tmr_margin);
	//init_timer(&wdt_timer);

    j = jiffies;
    wdt_timer.expires = j + REPEAT_DELAY;
    next_heartbeat = wdt_timer.expires;
    //add_timer(&wdt_timer);
	watchdog_open_flag=1;	
    //add_timer(&wdt_timer);
	spin_unlock(&wdt_lock);
	return 0;
}

static int k6lite_wdt_set_timeout(struct watchdog_device *wdd, unsigned int timeout)
{
    unsigned long j;
    wdtDbg("[WatchDog]k6lite_wdt_set_timeout=%d \n",timeout);
	if (timeout < 5)
		timeout=5;
		//return -EINVAL;
    j = jiffies;
    next_heartbeat = j + REPEAT_DELAY*timeout;
		
	 spin_lock(&wdt_lock);
	wdd->timeout = timeout;
	tmr_margin=timeout;
	wdt_timer.data = (unsigned long)tmr_margin;
    _Wachdog_Write2Byte(BASE_REG_WDT_PA + WDT_MAX_PRD_H, (((g_u32Clock*tmr_margin)>>16) & 0xFFFF));
    _Wachdog_Write2Byte(BASE_REG_WDT_PA + WDT_MAX_PRD_L, ((g_u32Clock*tmr_margin) & 0xFFFF));
	
	spin_unlock(&wdt_lock);
    wdtDbg("[WatchDog]k6lite_wdt_set_timeout data=%lx \r\n",wdt_timer.data);
	return 0;
}

static int k6lite_wdt_ping(struct watchdog_device *wdd)
{
	/* If we got a heartbeat pulse within the WDT_US_INTERVAL
	 * we agree to ping the WDT
	 */
    unsigned long j;
	 
	if (time_before(jiffies, next_heartbeat))
	{
       // wdtDbg("[WatchDog] infinity_wdt_ping tmr_margin=%lx \r\n",(jiffies-next_heartbeat));
		
		/* Ping the WDT */
		spin_lock(&wdt_lock);
		
        _Wachdog_Write2Byte(BASE_REG_WDT_PA + WDT_WDT_CLR, CLEAR_WDT);
        j = jiffies;
       next_heartbeat = j + REPEAT_DELAY*tmr_margin;


		spin_unlock(&wdt_lock);
        wdtDbg("[WatchDog] clipper_wdt_ping tmr_margin=%lx \r\n",tmr_margin);

		/* Re-set the timer interval */
		//mod_timer(&wdt_timer, jiffies + REPEAT_DELAY);
	} 
	else
		pr_warn("Heartbeat lost! Will not ping the watchdog\n");
	return 0;
	
}
static int k6lite_wdt_set_heartbeat(struct watchdog_device *wdd, unsigned timeout)
{
	if (timeout < 1)
		return -EINVAL;
   //TempNum++;
   //if(TempNum>20)
   //    return 0;
	wdtDbg("[WatchDog]clipper_wdt_set_heartbeat \n");

    _Wachdog_Write2Byte(BASE_REG_WDT_PA + WDT_WDT_CLR, CLEAR_WDT);

	wdd->timeout = timeout;
    
	return 0;
}

static const struct watchdog_info k6lite_wdt_ident = {
	.options          =     OPTIONS,
	.firmware_version =	0,
	.identity         =	"k6lite Watchdog",
};


static struct watchdog_ops k6lite_wdt_ops = {
	.owner = THIS_MODULE,
	.start = k6lite_wdt_start,
	.stop = k6lite_wdt_stop,
	.set_timeout= k6lite_wdt_set_timeout,
	.ping = k6lite_wdt_ping,
};

static struct watchdog_device k6lite_wdd = {
	.info = &k6lite_wdt_ident,
	.ops = &k6lite_wdt_ops,
};

static void k6lite_wdt_timer(unsigned long data)
{
    unsigned long j;
    k6lite_wdt_set_heartbeat(&k6lite_wdd, tmr_margin);
    j = jiffies;
    wdt_timer.expires = j + REPEAT_DELAY;
    add_timer(&wdt_timer);
}

static int  k6lite_wdt_probe(struct platform_device *pdev)
{
    int ret = 0;
    struct device *dev;
    int started = 0;
    wdtDbg("[WatchDog]k6lite_wdt_probe \n");
    dev = &pdev->dev;
    check_osc_clk();
   
	if (k6lite_wdt_set_heartbeat(&k6lite_wdd, tmr_margin)) {
		started = k6lite_wdt_set_heartbeat(&k6lite_wdd,
					CONFIG_WATCHDOG_DEFAULT_TIME);

		if (started == 0)
			dev_info(dev,
			   "tmr_margin value out of range, default %d used\n",
			       CONFIG_WATCHDOG_DEFAULT_TIME);
		else
			dev_info(dev, "default timer value is out of range, "
							"cannot start\n");
	}

	//watchdog_set_nowayout(&k6lite_wdd, nowayout);
    TempNum=0;
	ret = watchdog_register_device(&k6lite_wdd);
	if (ret) {
		dev_err(dev, "cannot register watchdog (%d)\n", ret);
		goto err;
	}
  
    //init_timer(&wdt_timer);
    
    //wdt_timer.data = (unsigned long)tmr_margin;
    //wdt_timer.function = k6lite_wdt_timer;
    
    //k6lite_wdt_start(&k6lite_wdd);

	return 0;

 err:
	watchdog_unregister_device(&k6lite_wdd);

	return ret;
}

static int  k6lite_wdt_remove(struct platform_device *dev)
{
    wdtDbg("[WatchDog]k6lite_wdt_remove \n");
    k6lite_wdt_stop(&k6lite_wdd);
	watchdog_unregister_device(&k6lite_wdd);

	return 0;
}


static void k6lite_wdt_shutdown(struct platform_device *dev)
{
    wdtDbg("[WatchDog]k6lite_wdt_shutdown \n");
	k6lite_wdt_stop(&k6lite_wdd);
}

#ifdef CONFIG_PM

static int k6lite_wdt_suspend(struct platform_device *dev, pm_message_t state)
{
    wdtDbg("[WatchDog]k6lite_wdt_suspend \n");
    if(watchdog_open_flag==1)
        k6lite_wdt_stop(&k6lite_wdd);

	return 0;
}

static int k6lite_wdt_resume(struct platform_device *dev)
{
    wdtDbg("[WatchDog]clipper_wdt_resume \n");
	/* Restore watchdog state. */
    if(watchdog_open_flag==1)  
        k6lite_wdt_start(&k6lite_wdd);
	return 0;
}

#else
#define k6lite_wdt_suspend NULL
#define k6lite_wdt_resume  NULL
#endif /* CONFIG_PM */

static struct platform_driver k6lite_wdt_driver = {
	.probe		= k6lite_wdt_probe,
	.remove		= k6lite_wdt_remove,
	.shutdown	= k6lite_wdt_shutdown,
	.suspend	= k6lite_wdt_suspend,
	.resume		= k6lite_wdt_resume,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= "k6lite-wdt",
	},
};

/*  platform device */
static struct platform_device k6lite_wdt_device =
{
    .name = "k6lite-wdt",
    .id = -1,
};


static int __init watchdog_init(void)
{
    int ret = 0;
	wdtDbg("k6lite Watchdog Timer, function : %s\n",__func__);
    ret = platform_driver_register(&k6lite_wdt_driver);
    if(ret)
    {
        wdtDbg("Register k6lite Watchdog Timer Driver Fail, Error : %d\n",ret);
    }
    else
    {
        ret = platform_device_register(&k6lite_wdt_device);
        if(ret)
        {
            wdtDbg("Register k6lite Watchdog Timer Device Fail, Error : %d\n",ret);
        }
    }
    return ret;
    
}

static void __exit watchdog_exit(void)
{
    wdtDbg("k6lite Watchdog Timer, function : %s\n",__func__);
	platform_driver_unregister(&k6lite_wdt_driver);
    platform_device_unregister(&k6lite_wdt_device);
}

module_init(watchdog_init);
module_exit(watchdog_exit);

MODULE_AUTHOR("MStar Semiconductor, Inc.");
MODULE_DESCRIPTION("k6lite Watchdog Device Driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS_MISCDEV(WATCHDOG_MINOR);
MODULE_ALIAS("platform:k6lite-wdt");
