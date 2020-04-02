#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/miscdevice.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/sched.h>
#include <linux/poll.h>

#include "chip_int.h"
#include "mdrv_types.h"
#include "ms_timer2.h"

U32  DEFAULT_MAX_VALUE = 12000000;
static DEFINE_MUTEX(timer2_mutex);

void mstar_write_reg16( u32 bank, u32 reg, u16 val )
{
    u32 address = RIU_REG_BASE + bank*0x100*2 + (reg << 2);
    *( ( volatile u16* ) address ) = val;
}

U16 mstar_read_reg16( u32 bank, u32 reg)
{
	u16 val = 0; 
	
    u32 address = RIU_REG_BASE + bank*0x100*2 + (reg << 2);
    val = *( ( volatile u16* ) address );
	
	return val;
}

static void mstar_timer2_set_max_tick(U32 max_value)
{
	mstar_write_reg16(TIMER2_BANK, TIMER2_MAX_ADDR_LO, max_value&0XFFFF);
	mstar_write_reg16(TIMER2_BANK, TIMER2_MAX_ADDR_HI, (max_value&0xFFFF0000)>>16);
}

static void mstar_timer2_enable(void)
{
	u16 val = mstar_read_reg16(IRQ_BANK,FIQ_MASK_ADDR);
	printk("%s:%d val=%d \n", __func__, __LINE__,val);
	val = val & (~(1 << 12));
	printk("%s:%d val=%d \n", __func__, __LINE__,val);
    mstar_write_reg16(IRQ_BANK, FIQ_MASK_ADDR, val);
	mstar_write_reg16(TIMER2_BANK, TIMER2_EN_ADDR, 0x0101);
}

static void mstar_timer2_disable(void)
{
	u16 val = mstar_read_reg16(IRQ_BANK,FIQ_MASK_ADDR);
	val = val | (1 << 12);
	mstar_write_reg16(IRQ_BANK, FIQ_MASK_ADDR, val);	
	mstar_write_reg16(TIMER2_BANK, TIMER2_EN_ADDR, 0x0000);	
}

static void mstar_timer2_irq_state_clr(void)
{
	u16 val = mstar_read_reg16(IRQ_BANK,FIQ_STATUS_ADDR);
	val = val | (1 << 12);
	mstar_write_reg16(IRQ_BANK, FIQ_STATUS_ADDR, val);
	mstar_write_reg16(TIMER2_BANK, TIMER2_EN_ADDR, 0x0001);
	mstar_write_reg16(TIMER2_BANK, TIMER2_EN_ADDR, 0x0101);
}

static irqreturn_t mstar_timer2_irq_handle(int irq,void *dev_id)
{
	struct timer2_st *tmr_st = (struct timer2_st *)dev_id;
	
	atomic_inc(&tmr_st->count);
	wake_up(&tmr_st->q);
    //printk("%s:%d enter \n", __func__, __LINE__);
	return IRQ_HANDLED;
}

static int mstar_timer2_open(struct inode *inode, struct file *fp)
{
	struct timer2_st *tmr_st;

	mutex_lock(&timer2_mutex);
    printk("%s:%d enter \n", __func__, __LINE__);
	tmr_st = kzalloc(sizeof *tmr_st, GFP_KERNEL);
	if (!tmr_st) {
		mutex_unlock(&timer2_mutex);
		return -ENOMEM;
	}

	strcpy(tmr_st->devname, current->comm);
	init_waitqueue_head(&tmr_st->q);
	atomic_set(&tmr_st->count, 0);

	fp->private_data = (void *)tmr_st;
	
    mstar_timer2_disable();    
	
	if (request_irq(E_FIQEXPH_FRM_PM, mstar_timer2_irq_handle, SA_INTERRUPT, MSTAR_TIMER2_DRV_NAME, tmr_st))
	{        
		printk("timer IRQ init fail!!\n");
		kfree(tmr_st);
		mutex_unlock(&timer2_mutex);
		return -1;
	}   
	
	disable_irq(E_FIQEXPH_FRM_PM);

	mutex_unlock(&timer2_mutex);
	printk("%s:%d exit \n", __func__, __LINE__);
    return 0;
}

static int mstar_timer2_release(struct inode *inop, struct file *fp)
{
	printk("%s:%d enter \n", __func__, __LINE__);
	
	mutex_lock(&timer2_mutex); 

    mstar_timer2_disable();   
    disable_irq(E_FIQEXPH_FRM_PM);
	
	if (fp->private_data != NULL) {
		struct timer2_st *tmr_st = (struct timer2_st *)fp->private_data;
        free_irq(E_FIQEXPH_FRM_PM, tmr_st);
		kfree(tmr_st);
		fp->private_data = NULL;
	}

	mutex_unlock(&timer2_mutex);

	return 0;
}

static ssize_t mstar_timer2_read(struct file *fp, char __user *buf, size_t count, loff_t *loff)
{
	
    return 0;
}

static ssize_t mstar_timer2_write(struct file *fp, const char __user *buf, size_t count, loff_t *loff)
{
	int option;
	int err;

	if (count < sizeof(int))
		return -EINVAL;

    if((err = copy_from_user(&option, buf, sizeof(option))))
	{
		return err;
	}

	switch (option) 
	{
	case E_IRQ_ENABLE:
		mstar_timer2_set_max_tick(DEFAULT_MAX_VALUE);
        mstar_timer2_enable();
		enable_irq(E_FIQEXPH_FRM_PM); 
		break;
	case E_IRQ_DISABLE:
		mstar_timer2_disable();   
        disable_irq(E_FIQEXPH_FRM_PM);
		break;
	case E_IRQ_ACK:
		mstar_timer2_irq_state_clr();
		break;
	default:
		break;
	}

	*loff += sizeof(option);
	return sizeof(option);
}

static long mstar_timer2_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
	U32 temp;

	if(MSTAR_TIMER2_IOC_MAGIC != _IOC_TYPE(cmd))
	{
		printk("IOCtl MAGIC Error!!! (Cmd=%x)\n",cmd);
        return -ENOTTY;
	}

	/* verify Access */
    if (_IOC_DIR(cmd) & IOC_IN)
    {
        if (!access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd)))
            return -EFAULT;
    }
    else if (_IOC_DIR(cmd) & IOC_OUT)
    {
        if (!access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd)))
            return -EFAULT;
    }

    /* handle cmd */
    switch(cmd)
    {
        case MSTAR_TIMER2_IOC_SET_MAX_VALUE:
		    if(__get_user(temp, (U32 __user *)arg))
		    {
		        return -EFAULT;
		    }
			DEFAULT_MAX_VALUE = TIMER2_MAX_VALUE/temp; //here use frame convert to time
			//DEFAULT_MAX_VALUE = temp;
			mstar_timer2_set_max_tick(DEFAULT_MAX_VALUE);
            break;
		default:
			break;
    }
	
    return 0;
}

static unsigned int mstar_timer2_poll(struct file *fp, struct poll_table_struct *wait)
{
    struct timer2_st *tmr_st = (struct timer2_st *)fp->private_data;

    poll_wait(fp, &tmr_st->q, wait);
    if (atomic_read(&tmr_st->count) > 0) {
        atomic_dec(&tmr_st->count);
        return POLLIN | POLLRDNORM;
    }
	
    return 0;
}

static struct file_operations mstar_timer2_fops = {
	.owner			= THIS_MODULE,
	.open	        = mstar_timer2_open,
	.release        = mstar_timer2_release,
	.read           = mstar_timer2_read,
	.write          = mstar_timer2_write,
	.unlocked_ioctl = mstar_timer2_ioctl,
	.poll           = mstar_timer2_poll
};

static struct miscdevice mstar_timer2_driver = {
	.minor = MISC_DYNAMIC_MINOR,
	.name  = MSTAR_TIMER2_DRV_NAME,
	.fops  = &mstar_timer2_fops,
};

static int __init mstar_timer2_init(void)
{
	int ret;

    printk("%s:%d enter \n", __func__, __LINE__);
	ret = misc_register(&mstar_timer2_driver);	
    printk("%s:%d exit \n", __func__, __LINE__);

	return ret;
}

static void __exit mstar_timer2_exit(void)
{
	misc_deregister(&mstar_timer2_driver);
}

module_init(mstar_timer2_init);
module_exit(mstar_timer2_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("mstar.timer");
