
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/timer.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>



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
#include <linux/semaphore.h>
#include <linux/platform_device.h>





static long mdrv_rtc_ioctl(struct file *file, unsigned int cmd,
							unsigned long arg)
{

	return 0;

}

static ssize_t mdrv_rtc_write(struct file *file, const char __user *data,
						size_t len, loff_t *ppos)
{

	return 0;

}

static int mdrv_rtc_open(struct inode *inode, struct file *file)
{
	//printk(KERN_EMERG "==RTC== %s, %d\n", __FUNCTION__, __LINE__);
	return 0;
}

static int mdrv_rtc_release(struct inode *inode, struct file *file)
{
	return 0;
}


static int mdrv_rtc_notify_sys(struct notifier_block *this, unsigned long code,
	void *unused)
{

	return NOTIFY_DONE;
}

static const struct file_operations mstar_rtc_fops = {
	.owner		= THIS_MODULE,
	.llseek		= no_llseek,
	.write		= mdrv_rtc_write,
	.unlocked_ioctl	= mdrv_rtc_ioctl,
	.open		= mdrv_rtc_open,
	.release	= mdrv_rtc_release,
};

static struct miscdevice mstar_rtc_miscdev = {
	.minor		= RTC_MINOR,
	.name		= "rtc",
	.fops		= &mstar_rtc_fops,
};

static int __init mstar_rtc_init(void)
{
    
	//printk(KERN_EMERG "==RTC== %s, %d\n", __FUNCTION__, __LINE__);
	printk(KERN_EMERG "[%s] RTC module init!!!!\n", __FUNCTION__);    
	return 0;
}

static void __exit mstar_rtc_exit(void)
{
    printk(KERN_EMERG "[%s] RTC module exit!!!!\n", __FUNCTION__);
}

module_init(mstar_rtc_init);
module_exit(mstar_rtc_exit);

MODULE_DESCRIPTION("Mstar RTC Device Driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS_MISCDEV(RTC_MINOR);


