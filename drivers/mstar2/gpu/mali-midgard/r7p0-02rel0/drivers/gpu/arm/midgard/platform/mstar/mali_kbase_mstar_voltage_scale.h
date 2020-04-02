/*
 *
 * (C) COPYRIGHT MStar Semiconductor, Inc. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * A copy of the licence is included with the program, and can also be obtained
 * from Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 */
#ifndef MSTAR_ENABLE_VOLTAGE_SCALE
#error MSTAR_ENABLE_VOLTAGE_SCALE must be defined
#endif

#ifdef CONFIG_MSTAR_IIC

#define voltage_scale_init(kbdev) 0
#define voltage_scale_term(kbdev)

#else

#include <mali_kbase.h>

#include <asm/atomic.h>
#include <linux/wait.h>
#include <linux/semaphore.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/workqueue.h>
#include <linux/moduleparam.h>

#define mstar_platform_gpu_dynamic_power_init() 0

#define MAX_DMSG_WRITE_BUFFER 64

#define PROC_CLT_GPU "on_demand_ctl_gpu"

extern int mali_debug_level;
#define ENABLE_DEBUG() (mali_debug_level >= 2)

static atomic_t on_demand_handshake_is_open = ATOMIC_INIT(0);
static DECLARE_WAIT_QUEUE_HEAD(gpu_on_demand_event_waitqueue_userspace_return);
static DEFINE_SEMAPHORE(dvfs_on_demand_gpu_event_sem);

static unsigned int start_userspace_ondemand_handshake = 0;

static unsigned int ready_to_change_frequency = 0;
static unsigned int ready_to_change_voltage = 0;
static unsigned int change_cnt = 0;
static unsigned int finished_change_cnt = 0;
static int voltage_change_result = 0;

/* data that will copy to user_space when need change voltage */
typedef struct
{
    unsigned int to_userspace_frequency;
    unsigned int to_userspace_voltage;
    unsigned int to_userspace_change_cnt;
} on_demand_to_userspace_st;

/* data from user space when voltage is changed */
typedef struct
{
    unsigned int from_userspace_finished_change_cnt;
    int from_userspace_voltage_change_result;
} on_demand_from_user_space_st;

static int on_demand_handshake_proc_open(struct inode *inode, struct file *file)
{
    if(atomic_read(&on_demand_handshake_is_open))
        return -EACCES;

    atomic_set(&on_demand_handshake_is_open, 1);
    start_userspace_ondemand_handshake = 1;

    return 0;
}

static ssize_t on_demand_handshake_proc_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    on_demand_from_user_space_st from_user_data;

    if (!start_userspace_ondemand_handshake)
    {
        if (ENABLE_DEBUG())
            printk("\033[35m[MALI] Function = %s, start_userspace_ondemand_handshake is %d\033[m\n",
                    __PRETTY_FUNCTION__, start_userspace_ondemand_handshake);
        return  -EFAULT;
    }
    else
    {
        if(!count)
        {
            if (ENABLE_DEBUG())
                printk("\033[35m[MALI] Function = %s, Line = %d\033[m\n", __PRETTY_FUNCTION__, __LINE__);
            return count;
        }

        if(count >= MAX_DMSG_WRITE_BUFFER)
            count = MAX_DMSG_WRITE_BUFFER - 1;

        copy_from_user(&from_user_data, buf, sizeof(from_user_data));
        finished_change_cnt = from_user_data.from_userspace_finished_change_cnt;
        voltage_change_result = from_user_data.from_userspace_voltage_change_result;

        if (ENABLE_DEBUG())
            printk(KERN_DEBUG "\033[33m[MALI] [result is %d] input: %d, wake_up wait_queue for write_down\033[m\n",
                    voltage_change_result, finished_change_cnt);
        // to wake_up a wait_queue waiting for voltage change
        wake_up_interruptible(&gpu_on_demand_event_waitqueue_userspace_return);

        return count;
    }
}

ssize_t on_demand_handshake_proc_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    on_demand_to_userspace_st to_userspace_data;
    int err;

    if (!start_userspace_ondemand_handshake)
    {
        if (ENABLE_DEBUG())
            printk("\033[35m[MALI] Function = %s, start_userspace_ondemand_handshake is %d\033[m\n",
                    __PRETTY_FUNCTION__, start_userspace_ondemand_handshake);
        return  -EFAULT;
    }
    else
    {
        if (down_interruptible(&dvfs_on_demand_gpu_event_sem))
            return -ERESTARTSYS;

        to_userspace_data.to_userspace_frequency = ready_to_change_frequency;
        to_userspace_data.to_userspace_voltage = ready_to_change_voltage;
        to_userspace_data.to_userspace_change_cnt = change_cnt;

        err = copy_to_user((void *)buf, &to_userspace_data, sizeof(to_userspace_data));

        *ppos += sizeof(to_userspace_data);
        return sizeof(to_userspace_data);
    }
}

static int on_demand_handshake_proc_release(struct inode *inode, struct file * file)
{
    if (ENABLE_DEBUG())
        printk("\033[35m[MALI] Function = %s, Line = %d, "
                "set start_userspace_ondemand_handshake to be 0\033[m\n",
                __PRETTY_FUNCTION__, __LINE__);
    start_userspace_ondemand_handshake = 0;
    WARN_ON(!atomic_read(&on_demand_handshake_is_open));
    atomic_set(&on_demand_handshake_is_open, 0);
    return 0;
}

const struct file_operations proc_on_demand_handshake_operations = {
    .open    = on_demand_handshake_proc_open,
    .write   = on_demand_handshake_proc_write,
    .read    = on_demand_handshake_proc_read,
    .release = on_demand_handshake_proc_release,
};

static int proc_func_init(void)
{
    if (proc_create(PROC_CLT_GPU, S_IRUSR|S_IWUSR, NULL, &proc_on_demand_handshake_operations) == NULL)
        return -ENOMEM;

    sema_init(&dvfs_on_demand_gpu_event_sem, 0);
    return 0;
}

typedef struct
{
    struct kbase_device *kbdev;
    bool (*action_callback)(struct kbase_device *);
    struct work_struct action_work;
    struct workqueue_struct *action_wq;
    bool (*clock_up)(struct kbase_device *);    /* increase the frequency */
    bool (*clock_down)(struct kbase_device *);  /* decrease the frequency */
} voltage_scale_platform;

static void action_callback(struct work_struct *work)
{
    voltage_scale_platform *platform;
    struct kbasep_pm_metrics_data *metrics;
    unsigned long flags;

    platform = container_of(work, voltage_scale_platform, action_work);

    if (platform->action_callback)
    {
        platform->action_callback(platform->kbdev);
        platform->action_callback = NULL;
    }

    metrics = &platform->kbdev->pm.backend.metrics;

    spin_lock_irqsave(&metrics->lock, flags);

    metrics->timer_active = true;
    hrtimer_start(&metrics->timer,
            HR_TIMER_DELAY_MSEC(platform->kbdev->pm.dvfs_period),
            HRTIMER_MODE_REL);

    spin_unlock_irqrestore(&metrics->lock, flags);
}

static void proc_func_term(void)
{
    remove_proc_entry(PROC_CLT_GPU, NULL);
}

static bool dvfs_adjust_clock(struct kbase_device* kbdev,
        voltage_scale_platform* platform,
        bool (*action)(struct kbase_device* kbdev))
{
    if (NULL != action && NULL == platform->action_callback)
    {
        platform->action_callback = action;
        queue_work(platform->action_wq, &platform->action_work);
        kbdev->pm.backend.metrics.timer_active = false;
    }

    return true;
}

static bool dvfs_clock_up_wrapper(struct kbase_device* kbdev)
{
    voltage_scale_platform* platform = kbdev->dvfs.platform;

    if (unlikely(platform == NULL))
        return false;

    return dvfs_adjust_clock(kbdev, platform, platform->clock_up);
}

static bool dvfs_clock_down_wrapper(struct kbase_device* kbdev)
{
    voltage_scale_platform* platform = kbdev->dvfs.platform;

    if (unlikely(platform == NULL))
        return false;

    return dvfs_adjust_clock(kbdev, platform, platform->clock_down);
}


static int voltage_scale_init(struct kbase_device* kbdev)
{
    int err;
    voltage_scale_platform* platform;

    platform = kzalloc(sizeof(voltage_scale_platform), GFP_KERNEL);
    if (NULL == platform)
    {
        err = -ENOMEM;
        goto error;
    }

    platform->kbdev = kbdev;
    platform->action_callback = NULL;

    platform->action_wq = alloc_workqueue("kbase_dvfs_action",
            WQ_HIGHPRI | WQ_UNBOUND, 1);
    if (NULL == platform->action_wq)
    {
        err = -ENOMEM;
        goto error;
    }

    err = proc_func_init();
    if (err != 0)
        goto error;

    INIT_WORK(&platform->action_work, action_callback);

    platform->clock_up = kbdev->dvfs.clock_up;
    kbdev->dvfs.clock_up = dvfs_clock_up_wrapper;

    platform->clock_down = kbdev->dvfs.clock_down;
    kbdev->dvfs.clock_down = dvfs_clock_down_wrapper;

    WARN_ON(kbdev->dvfs.platform != NULL);
    kbdev->dvfs.platform = platform;

    return 0;

error:
    if (platform)
    {
        if (platform->action_wq)
        {
            destroy_workqueue(platform->action_wq);
            platform->action_wq = NULL;
        }

        kfree(platform);
    }

    return err;
}

static void voltage_scale_term(struct kbase_device* kbdev)
{
    voltage_scale_platform* platform = kbdev->dvfs.platform;

    if (platform == NULL)
        return;

    kbdev->dvfs.platform = NULL;
    destroy_workqueue(platform->action_wq);
    platform->action_wq = NULL;
    proc_func_term();
    kfree(platform);
}

static bool adjust_voltage(u32 voltage, u32 frequency)
{
    bool bRet = false;
    if (start_userspace_ondemand_handshake == 1)
    {
        bRet = true;
        ready_to_change_frequency= frequency;
        ready_to_change_voltage = voltage;
        change_cnt++;

        up(&dvfs_on_demand_gpu_event_sem);

        if (ENABLE_DEBUG())
        {
            printk(KERN_DEBUG "\033[35m[MALI] Data Exchange Count to User Space: %d\033[m\n", change_cnt);
            printk(KERN_DEBUG "\033[35m[MALI] Voltage: %d, Frequency: %d\033[m\n", voltage, frequency);
        }
        wait_event_interruptible_timeout(gpu_on_demand_event_waitqueue_userspace_return,
                finished_change_cnt == change_cnt, MAX_SCHEDULE_TIMEOUT);
        if (ENABLE_DEBUG())
            printk(KERN_DEBUG "\033[35m[MALI] Data Exchange Count from User Space: %d\033[m\n", finished_change_cnt);
    }
    else
    {
        if (ENABLE_DEBUG())
            printk("\033[35m[MALI] Function = %s, start_userspace_ondemand_handshake is %d\033[m\n",
                    __PRETTY_FUNCTION__, start_userspace_ondemand_handshake);
    }

    return bRet;
}

#endif /* CONFIG_MSTAR_IIC */
