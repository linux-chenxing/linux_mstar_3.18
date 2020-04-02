/*
 * linux/driver/mstar/trustzone/trustzone.c
 *
 * Copyright (C) 1992, 1998-2004 Linus Torvalds, Ingo Molnar
 *
 * This file contains the /proc/irq/ handling code.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/poll.h>
#include <asm/uaccess.h>
#include <asm/pgtable.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include "mdrv_xc_proc.h"
#include <linux/kthread.h>

//#define MAX_NAMELEN 10

static struct proc_dir_entry *proc_entry;
static struct proc_dir_entry *root_xc_dir;
static spinlock_t _spinlock_xc_proc;
static DECLARE_WAIT_QUEUE_HEAD(xc_wait);
static bool _bDirty = false;
unsigned int xc_proc_wake_up(void)
{
    _bDirty = true;
    //wake_up(&xc_wait);
    wake_up_interruptible(&xc_wait);
    return 0;
}
unsigned int xc_proc_poll(struct file *fp, struct poll_table_struct *wait)
{
    unsigned long flags;
    struct irq_proc *ip = (struct irq_proc *)fp->private_data;
    spin_lock_irqsave(&_spinlock_xc_proc,flags);
    if(_bDirty)
    {
        spin_unlock_irqrestore(&_spinlock_xc_proc, flags);
        return 0;
    }
    spin_unlock_irqrestore(&_spinlock_xc_proc, flags);
    //poll_wait(fp, &xc_wait, wait);
    wait_event_interruptible(xc_wait, _bDirty == true);
    return 0;
}
ssize_t xc_proc_read(struct file *fp, char *bufp, size_t len, loff_t *where)
{
    unsigned long flags;
    spin_lock_irqsave(&_spinlock_xc_proc,flags);
    if(_bDirty)
    {
        _bDirty = false;
    }
    spin_unlock_irqrestore(&_spinlock_xc_proc, flags);
    //printk("xc_read\n");
    return 0;
}
struct file_operations xc_proc_file_operations = {
    .read = xc_proc_read,
    .poll = xc_proc_poll,
};
int init_xc_proc( void )
{
    int ret = 0;

    spin_lock_init(&_spinlock_xc_proc);

    printk("Init xc proc\n");
    //printk(KERN_CRIT"\033[1;32m[%s:%d]Init xc proc\033[m\n",__FUNCTION__,__LINE__);

    root_xc_dir = proc_mkdir("xc", NULL);

    proc_entry = proc_create("dolby_hdr", 0644, root_xc_dir, &xc_proc_file_operations);

    return ret;
}

int exit_xc_proc(void)
{
    printk("exit!\n");
    return 1;
}
