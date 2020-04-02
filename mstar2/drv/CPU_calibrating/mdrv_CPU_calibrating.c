#include <linux/fs.h>
#include <linux/hugetlb.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/mmzone.h>
#include <linux/proc_fs.h>
#include <linux/quicklist.h>
#include <linux/seq_file.h>
#include <linux/swap.h>
#include <linux/vmstat.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/ctype.h>

#include <asm/setup.h>
#include <asm/cacheflush.h>
#include <linux/delay.h>

#include <include/mstar/mstar_chip.h>
#include <linux/spinlock.h>

#include <linux/module.h>
#include <linux/cpufreq.h>
#include <linux/smp.h>
#include <linux/cpu.h>
#include <linux/io.h>
#include <linux/timer.h>
#include <linux/vmalloc.h>
#include <mstar/mpatch_macro.h>

#if (MP_PLATFORM_T_SENSOR_OBSERVATION == 1)
/*patch for add_timer issue(york)*/
#include <linux/kthread.h>
/*patch for add_timer issue*/
#endif

#include "./include/mach/hardware.h"
#include "./include/mach/platform.h"

#include <chip_dvfs_calibrating.h>
#include <linux/version.h>

#ifdef CONFIG_MSTAR_DVFS
#ifndef __MDRV_DVFS_H__
#include "mdrv_dvfs.h"
#include <mach/io.h>
#endif
#endif


#define MAX_DMSG_WRITE_BUFFER	64
#define CM_ID  	IO_ADDRESS(INTEGRATOR_HDR_ID)
#define CM_OSC	IO_ADDRESS(INTEGRATOR_HDR_OSC)
#define CM_STAT IO_ADDRESS(INTEGRATOR_HDR_STAT)
#define CM_LOCK IO_ADDRESS(INTEGRATOR_HDR_LOCK)
#define FREQ_CHECK_LINK_TIME (HZ)
#define UNUSUAL_DVFS_HANDSHAKE_TYPE 0xFF
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,10,23)
#define cpufreq_notify_transition(A, B, C)	cpufreq_notify_transition(B, C)
#endif	// 3.10.40 only pass 2 parameters

#ifdef CONFIG_MP_PLATFORM_DVFS_GET_ERR_STATE_FROM_CPUHOTPLG_DISABLE
extern int cpu_hotplug_GetStatus_disable_Err_busy(void);
#endif
extern unsigned int forcibly_set_target_flag;
extern unsigned int over_thermal_flag;
extern void change_interval(unsigned int old_freq, unsigned int new_freq);
extern unsigned int query_frequency(void);
extern struct cpufreq_policy *cpufreq_cpu_data;
#ifndef CONFIG_MP_STATIC_TIMER_CLOCK_SOURCE
extern void mstar_update_sched_clock(void);
extern unsigned int SC_MULT;
extern unsigned int SC_SHIFT;
#endif // CONFIG_MP_STATIC_TIMER_CLOCK_SOURCE

/* str usage */
volatile int first_init_done = 0;
atomic_t ac_str_cpufreq = ATOMIC_INIT(0);
atomic_t disable_dvfs = ATOMIC_INIT(0);

#if (MP_PLATFORM_T_SENSOR_OBSERVATION == 1)
/*patch for add_timer issue(york)*/
static struct task_struct *t_sensor_tsk = NULL;
static void t_sensor_thread(void* arg);
/*patch for add_timer issue*/
#endif

/* proc usage */
DEFINE_MUTEX(mstar_cpufreq_lock);
static atomic_t proc_is_open = ATOMIC_INIT(0);
static struct cpufreq_driver integrator_driver;
atomic_t echo_calibrating_freq = ATOMIC_INIT(0);
unsigned int current_frequency = 0;
unsigned int register_frequency = 0;
static int ORI_FREQ_KHZ;

/* on_demand handshake usage */
static atomic_t on_demand_handshake_is_open = ATOMIC_INIT(0);
DECLARE_WAIT_QUEUE_HEAD(DVFS_on_demand_event_waitqueue);
DECLARE_WAIT_QUEUE_HEAD(DVFS_on_demand_event_waitqueue_userspace_return);
EXPORT_SYMBOL(DVFS_on_demand_event_waitqueue);
EXPORT_SYMBOL(DVFS_on_demand_event_waitqueue_userspace_return);
DEFINE_SEMAPHORE(DVFS_on_demand_event_SEM);
DEFINE_SEMAPHORE(DVFS_disable_SEM);

/* this is for muji test cmdq (we don not use SAR to adjust voltage) */
#if defined(CONFIG_MSTAR_IIC) && defined(CONFIG_MSTAR_DVFS_KERNEL_IIC)
unsigned int start_userspace_ondemand_handshake = 1;
#else
unsigned int start_userspace_ondemand_handshake = 0;
#endif
unsigned int ready_to_change_cpufreq = 0;
unsigned int ready_to_change_voltage = 0;
unsigned int ready_to_change_voltage_type = 0;
unsigned int change_cnt = 0;
unsigned int finished_change_cnt = 0;
int voltage_change_result = 0;

unsigned int bootarg_dvfs_disable = 0;

/* callback function usage */
static struct cpufreq_policy *ondemand_timer[CONFIG_NR_CPUS];
#if defined(CONFIG_CPU_FREQ_DEFAULT_GOV_ONDEMAND) || defined(CONFIG_CPU_FREQ_DEFAULT_GOV_INTERACTIVE)
static struct timer_list Check_T_sensor_timer;
static void Mdrv_CPU_T_sensor_Check_callback(unsigned long value);
#else
static struct timer_list Check_Freq_timer;
static void Mdrv_CPU_Freq_Check_callback(unsigned long value);
#endif

#if defined(CONFIG_MP_DVFS_CPUHOTPLUG_USE_ONLINE_CPU_MAX_LOAD) || defined(CONFIG_MP_DVFS_CPUHOTPLUG_USE_ONLINE_CPU_AVERAGE_LOAD)
DEFINE_MUTEX(mstar_cpuload_lock);
unsigned int mstar_cpu_load_freq[CONFIG_NR_CPUS];
#endif

#ifdef CONFIG_MP_DVFS_CPUHOTPLUG
static int show_mstar_cpu_dvfs_hotplug_scenario_table = 0;
DEFINE_MUTEX(mstar_cpuload_threshold);
DEFINE_MUTEX(mstar_dvfs_cpuhotplug_lock);

Cpu_Dvfs_Hotplug_Scenario mstar_cpu_dvfs_hotplug_scenario[CPU_DVFS_HOTPLUG_LEVEL_MAX] =
{
	{1,  700000, 1, 0, 0},
	{2,  700000, 1, 0, 0},
	{3,  700000, 1, 0, 0},
	{4,  700000, 1, 0, 0},
	{4,  800000, 1, 0, 0},
	{4,  900000, 1, 0, 0},
	{4, 1000000, 1, 0, 0},
	{4, 1100000, 1, 0, 0},
	{4, 1200000, 1, 0, 0},
	{4, 1300000, 1, 0, 0},
	{4, 1400000, 1, 0, 0},
	{4, 1500000, 1, 0, 0},
};

/* this total work_table is from mstar_cpu_dvfs_hotplug_scenario * 80 (up_adj_threshold) */
Cpu_Dvfs_Hotplug_Total_Workload_Region mstar_cpu_dvfs_hotplug_workload_table[CPU_DVFS_HOTPLUG_LEVEL_MAX] =
{
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
};
#endif

static struct cpufreq_frequency_table mstar_freq_table[] = {
	{0x01, 0},
	{0x02, 0},
	{0, CPUFREQ_TABLE_END},
};


#if (defined CONFIG_MP_DVFS_ANTUTU_HOTPLUG) && (defined CONFIG_MP_DVFS_CPUHOTPLUG)

#define STOP_ANTUTU_HOTPLUG 0
#define START_ANTUTU_HOTPLUG 1

DEFINE_MUTEX(antut_hotplug_lock);
static int antutu_hotplug_flag = STOP_ANTUTU_HOTPLUG;

void set_antutu_hotplug_flag(int flag);
int get_antutu_hotplug_flag();
bool antutu_exist();

#endif


int get_freq(void)
{
	return query_frequency() * 1000;
}

/*
 * Validate the speed policy.
 */
static int integrator_verify_policy(struct cpufreq_policy *policy)
{
	cpufreq_verify_within_limits(policy, policy->cpuinfo.min_freq, policy->cpuinfo.max_freq);

	return 0;
}

static unsigned int integrator_get(unsigned int cpu)
{
	unsigned int current_freq;
	cpumask_t cpus_allowed = current->cpus_allowed;

	set_cpus_allowed(current, cpumask_of_cpu(0));
	current_freq = get_freq();
	set_cpus_allowed(current, cpus_allowed);

	return current_freq;
}
#if (MP_PLATFORM_T_SENSOR_OBSERVATION == 1)
static void t_sensor_thread(void* arg){
	while(1){
		msleep(1000);
		mutex_lock(&mstar_cpufreq_lock);
		if(atomic_read(&disable_dvfs) != 1)
			Mdrv_CPU_T_sensor_Check_callback(NULL);
		mutex_unlock(&mstar_cpufreq_lock);
	}
}
#endif
static int integrator_cpufreq_init(struct cpufreq_policy *policy)
{
#if (MP_PLATFORM_T_SENSOR_OBSERVATION == 1)
	/*patch for add_timer issue(york)*/
	int ret;
	/*patch for add_timer issue*/
#endif

	/* set default policy and cpuinfo */
	/* set default policy and cpuinfo */

        policy->cpuinfo.max_freq = policy->max = mstar_freq_table[1].frequency = MAX_CPU_FREQ;
	policy->cpuinfo.min_freq = policy->min = mstar_freq_table[0].frequency = MIN_CPU_FREQ;
	policy->cpuinfo.transition_latency = TRANSITION_LATENCY; 	/* 1 ms, assumed */
	policy->cur = integrator_get(policy->cpu);

	//FIXME: this shall match frequency table.
	if(policy->cur > MAX_CPU_FREQ)
        policy->cur = MAX_CPU_FREQ;
    else if(policy->cur < MIN_CPU_FREQ)
        policy->cur = MIN_CPU_FREQ;

	ondemand_timer[policy->cpu] = policy;
	if(!first_init_done)
		printk("\033[35mFunction = %s, Line = %d, policy->cpu = %d, integrator_get return %d\033[m\n", __PRETTY_FUNCTION__, __LINE__, policy->cpu, policy->cur);

	/* move to here to let the timer works on cpu_0 */
	if(get_cpu() == 0)
	{
		put_cpu();
		if(atomic_read(&ac_str_cpufreq) == 0)
		{
			atomic_set(&ac_str_cpufreq, policy->cur);
			printk("\033[31mFunction = %s, Line = %d, set ac_str_cpufreq to %dkHz\033[m\n", __PRETTY_FUNCTION__, __LINE__, atomic_read(&ac_str_cpufreq));
		}
		else
			printk(KERN_DEBUG "\033[31mFunction = %s, Line = %d, ac_str_cpufreq is already set to %dkHz\033[m\n", __PRETTY_FUNCTION__, __LINE__, atomic_read(&ac_str_cpufreq));

#if defined(CONFIG_CPU_FREQ_DEFAULT_GOV_ONDEMAND) || defined(CONFIG_CPU_FREQ_DEFAULT_GOV_INTERACTIVE)
#if (MP_PLATFORM_T_SENSOR_OBSERVATION == 0)
    	init_timer(&Check_T_sensor_timer);
    	Check_T_sensor_timer.data = 1;
    	Check_T_sensor_timer.function = Mdrv_CPU_T_sensor_Check_callback;
    	Check_T_sensor_timer.expires = jiffies + FREQ_CHECK_LINK_TIME;  // 1 second do once
    	add_timer(&Check_T_sensor_timer);
#else
		/*patch for add_timer issue(york)*/
		if(t_sensor_tsk == NULL)
		{
			t_sensor_tsk = kthread_create(t_sensor_thread, NULL, "T_sensor_Check");

			kthread_bind(t_sensor_tsk, 0);
			if (IS_ERR(t_sensor_tsk)) {
				printk("create kthread for t_sensor temperature observation fail\n");
				ret = PTR_ERR(t_sensor_tsk);
				t_sensor_tsk = NULL;
				goto out;
			}
			else
				wake_up_process(t_sensor_tsk);
		}
		/*patch for add_timer issue*/
#endif
#endif // CONFIG_CPU_FREQ_DEFAULT_GOV_ONDEMAND
	}
	else
	{
		put_cpu();
	}

	return 0;
#if (MP_PLATFORM_T_SENSOR_OBSERVATION == 1)
out:
    return ret;
#endif
}

typedef struct _IO_CPU_calibrating_INFO
{
	char* MESSAGE_BUFF;
	char CPUID;
	char  MESSAGE_LEN;
	int MID;
}IO_CPU_calibrating_INFO;

static int CPU_calibrating_proc_ioctl(struct file *filp, unsigned int cmd, IO_CPU_calibrating_INFO* message_buf)
{
	IO_CPU_calibrating_INFO* bb = message_buf;
	char usr_buf[256];

	if (copy_from_user(usr_buf, bb->MESSAGE_BUFF, bb->MESSAGE_LEN))
	{
		printk(KERN_ERR "setgreq_proc_ioctl error\n");
		return -EFAULT;
	}

	return 0;
}

int _CPU_calibrating_proc_write(const unsigned long, const unsigned long);
int __CPU_calibrating_proc_write(const unsigned long);
void write_cpufreq_to_RIU(const unsigned long);
static unsigned long read_echo_calibrating_freq(void);
static void write_echo_calibrating_freq(const unsigned long);

static unsigned long jiffies_boost_lasttime = 0;

static ssize_t CPU_calibrating_proc_write(struct file *file, const char __user *buf,
		size_t count, loff_t *ppos)
{
	char buffer[MAX_DMSG_WRITE_BUFFER];
	long idx;
	unsigned long cpu_freq_in_khz = 0;
	unsigned long client_id = 0;

	if (!count)
		return count;

	if (count >= MAX_DMSG_WRITE_BUFFER)
		count = MAX_DMSG_WRITE_BUFFER - 1;

	/*
	 * Prevent Tainted Scalar Warning:
	 * Buffer can't be tainted because:
	 * 1. The count never exceeds MAX_DMSG_WRITE_BUFFER i.e. buffer size.
	 * 2. copy_from_user returns 0 in case of correct copy.
	 *So, we don't need to sanitize buffer.
	 *
	 */
	if (copy_from_user(buffer, buf, count))
		return -EFAULT;

	buffer[count] = '\0';

	if (buffer[0] == '/')
		idx = 3;
	else if (strict_strtol(buffer, 0, &idx) != 0)  //Force change str to decimal conversion (str, base, *converted_num), base is hex, decimal, or ...
		return -EINVAL;

	// idx[29:22] is the client id
	client_id = (idx >> 22) & 0xff;

	// idx[21:0] is the cpu frequency in Khz
	cpu_freq_in_khz = idx & 0x3FFFFF;

	_CPU_calibrating_proc_write(client_id, cpu_freq_in_khz);

	return count;
}

static DEFINE_MUTEX(boost_client_mutex);
static LIST_HEAD(boost_client_head);

struct boost_client
{
	unsigned long client_id;
	unsigned long cpu_freq_in_khz;
	struct list_head list_head;
};

static bool is_any_boost_client_running(void)
{
	bool i;

	mutex_lock(&boost_client_mutex);
	i = !list_empty(&boost_client_head);
	mutex_unlock(&boost_client_mutex);

	return i;
}

static int add_boost_client(unsigned long client_id, unsigned long cpu_freq_in_khz, void *arg)
{
	struct boost_client *bc = NULL;

	if ((bc = kmalloc(sizeof(*bc), GFP_KERNEL)) == NULL)
		return -ENOMEM;

	bc->client_id = client_id;
	bc->cpu_freq_in_khz = cpu_freq_in_khz;

	mutex_lock(&boost_client_mutex);
	list_add(&bc->list_head, &boost_client_head);
	mutex_unlock(&boost_client_mutex);

	return 0;
}

static struct boost_client *find_boost_client(unsigned long client_id)
{
	struct boost_client *i = NULL;
	struct boost_client *bc = NULL;

	mutex_lock(&boost_client_mutex);
	list_for_each_entry(i, &boost_client_head, list_head) {
		if (i->client_id == client_id) {
			bc = i;
		}
	}
	mutex_unlock(&boost_client_mutex);

	return bc;
}

static int del_boost_client(struct boost_client *bc)
{
	// remove the client node
	BUG_ON(!bc);

	mutex_lock(&boost_client_mutex);
	list_del(&bc->list_head);
	mutex_unlock(&boost_client_mutex);

	kfree(bc);

	return 0;
}

static bool del_all_boost_client(void)
{
	struct boost_client *bc = NULL;
	struct boost_client *bc2 = NULL;

	mutex_lock(&boost_client_mutex);
	list_for_each_entry_safe(bc, bc2, &boost_client_head, list_head) {
		list_del(&bc->list_head);
		kfree(bc);
	}
	mutex_unlock(&boost_client_mutex);

	return true;
}

static bool is_boost_client_running(struct boost_client *bc)
{
	return (bc != NULL) ? true : false;
}

static struct boost_client *find_boost_client_with_highest_priority(void)
{
	struct boost_client *i = NULL;
	struct boost_client *bc = NULL;

	mutex_lock(&boost_client_mutex);
	bc = list_entry(boost_client_head.next, struct boost_client, list_head);
	list_for_each_entry(i, &boost_client_head, list_head) {
		if(i->client_id < bc->client_id) {
			bc = i;
		}
	}
	mutex_unlock(&boost_client_mutex);

	return bc;
}

static unsigned long read_echo_calibrating_freq(void)
{
	return (unsigned long)atomic_read(&echo_calibrating_freq);
}

static void write_echo_calibrating_freq(const unsigned long value)
{
	atomic_set(&echo_calibrating_freq, value);
}

int _CPU_calibrating_proc_write(
		const unsigned long client_id,
		const unsigned long cpu_freq_in_khz)
{
    struct cpufreq_policy *policy;
	int i;
    bool ret = 0;
#if (defined CONFIG_MP_DVFS_FORCE_USE_ONE_FREQ) || (defined CONFIG_MP_DVFS_FORCE_PINGO_TEST) || (defined CONFIG_MP_DVFS_VID_ONLY)
	return 0;
#endif

    mutex_lock(&mstar_cpufreq_lock);
	if( (!start_userspace_ondemand_handshake) || (atomic_read(&disable_dvfs) == 1) )
	{
		printk("\033[35mFunction = %s, [Return], client_id = %lu and cpu_freq_in_khz =%lu, disable_dvfs is %d\033[m\n", __PRETTY_FUNCTION__, client_id, cpu_freq_in_khz, atomic_read(&disable_dvfs));
		ret = -1;
        goto not_change_cpu_freq;
	}

	struct boost_client *bc = find_boost_client(client_id);
	printk(KERN_DEBUG "[dvfs_boost] here comes a client with client_id = %lu and cpu_freq_in_khz = %lu\n", client_id, cpu_freq_in_khz);

	get_online_cpus();
	for_each_online_cpu(i)
	{
		policy = cpufreq_cpu_get(i);

		if(!policy)
		{
			printk("\033[31mFunction = %s, Line = %d, cpu%d do: [cpu%d] policy is NULL\033[m\n",
						__PRETTY_FUNCTION__, __LINE__, get_cpu(), i);
			put_cpu();
			goto not_change_cpu_freq;
		}
		cpufreq_cpu_put(policy);
	}
	put_online_cpus();

	// extend the timer
	jiffies_boost_lasttime = jiffies;

	if (cpu_freq_in_khz == 0x5566) {
		goto not_change_cpu_freq;

	} else if (cpu_freq_in_khz == 0) {
		if (is_boost_client_running(bc) == true) {
			del_boost_client(bc);

			if (is_any_boost_client_running() == true) {
				bc = find_boost_client_with_highest_priority();
				write_echo_calibrating_freq(bc->cpu_freq_in_khz);
				goto change_cpu_freq;
			} else {
#if defined(CONFIG_CPU_FREQ_DEFAULT_GOV_ONDEMAND) || defined(CONFIG_CPU_FREQ_DEFAULT_GOV_INTERACTIVE)
				goto not_change_cpu_freq;
#else
				write_echo_calibrating_freq(1008000);
				goto change_cpu_freq;
#endif
			}
		} else {
			printk(KERN_WARNING "[dvfs_boost] this client is not running\n");
		}
	} else {
		// check if a client with same client_id already exists
		if (is_boost_client_running(bc) == true) {
			goto not_change_cpu_freq;
		} else {
			// registered a boost client with client_id and cpu_freq_in_khz
			add_boost_client(client_id, cpu_freq_in_khz, NULL);
			// echo_calibrating_freq = the cpu_freq_in_khz of the client with highest priority
			bc = find_boost_client_with_highest_priority();
			write_echo_calibrating_freq(bc->cpu_freq_in_khz);
			goto change_cpu_freq;
		}
	}

	// something wrong happened
	ret = -1;
	goto not_change_cpu_freq;

change_cpu_freq:
	__CPU_calibrating_proc_write(read_echo_calibrating_freq());
not_change_cpu_freq:
    mutex_unlock(&mstar_cpufreq_lock);
	return ret;
}


int __CPU_calibrating_proc_write(const unsigned long idx)
{
	struct cpufreq_freqs freqs;
	struct cpufreq_policy *policy;
    int i;
	unsigned long idx_max;
	unsigned long idx_temp;
	idx_temp = idx;
#ifdef CONFIG_MP_DVFS_CPUHOTPLUG
	unsigned int target_freq;
#endif

	if(atomic_read(&disable_dvfs) == 1)
	{
		printk("\033[35m[Return] Function = %s, force setting cpufreq to %u, disable_dvfs is %d\033[m\n", __PRETTY_FUNCTION__, idx_temp, atomic_read(&disable_dvfs));
		return 0;
	}
  	if(idx_temp > CONFIG_DVFS_CPU_CLOCK_MAX * 1000)
  	{
  		printk(KERN_WARNING "\033[36m\n freq %d KHZ > CONFIG_DVFS_CPU_CLOCK_MAX %d KHZ ,not allowed\033[m\n", (int)idx_temp, CONFIG_DVFS_CPU_CLOCK_MAX);
  		return -1;
  	}
	idx_max = MDrvDvfsQueryCpuClockByTemperature(0);
	if(idx_temp > idx_max)
		idx_temp = idx_max;
	register_frequency = idx_temp;

#if (!defined CONFIG_MP_STATIC_TIMER_CLOCK_SOURCE) && (!defined CONFIG_MP_GLOBAL_TIMER_12MHZ_PATCH)
	mstar_update_sched_clock();
#endif // CONFIG_MP_STATIC_TIMER_CLOCK_SOURCE

	get_online_cpus();
	for_each_online_cpu(i)
	{
		ondemand_timer[i]->max = idx_temp;
	}
	put_online_cpus();

#ifdef CONFIG_MP_DVFS_CPUHOTPLUG
	/* combine total_cpu_nr with target_freq, the leftest 4 bits will be total_cpu_nr, boost mode using BOOST_CPU_NR */
	target_freq = idx_temp;
	target_freq = target_freq | (BOOST_CPU_NR << 28);
	change_cpus_timer(__FUNCTION__, target_freq, 0);
#else
	change_cpus_timer(__FUNCTION__, idx_temp, 0);
#endif


#if (!defined CONFIG_MP_STATIC_TIMER_CLOCK_SOURCE) && (!defined CONFIG_MP_GLOBAL_TIMER_12MHZ_PATCH)
	if(policy->cur != idx_temp)
	{
		change_interval(freqs.old, freqs.new);
	}
#endif

	current_frequency = idx_temp;

	return 0;
}

void write_cpufreq_to_RIU(const unsigned long new_scaling_cur_freq)
{
	printk(KERN_DEBUG "\033[31mFunction = %s, Line = %d, write cpu_freq to RIU: 0x%X\033[m\n", __PRETTY_FUNCTION__, __LINE__, (unsigned int)(new_scaling_cur_freq/1000)); // joe.liu
	reg_writew((new_scaling_cur_freq/1000), 0x1F200A1C);
}

#ifdef CONFIG_MSTAR_DVFS
void MDrvDvfsVoltageSetup(unsigned int dwCpuClock, unsigned int dwVoltage, unsigned int dwVoltageType);
void MDrvDvfsVoltageSetup(unsigned int dwCpuClock, unsigned int dwVoltage, unsigned int dwVoltageType)
{
	//dwVoltage = 145; // set this for checking if voltage setting correct
    if(start_userspace_ondemand_handshake == 1)
    {
    	ready_to_change_cpufreq = dwCpuClock;
    	ready_to_change_voltage = dwVoltage;
    	ready_to_change_voltage_type = dwVoltageType;
    	change_cnt++;

		up(&DVFS_on_demand_event_SEM);

    	printk(KERN_DEBUG "\033[35m[INFO] Data Exchange Count to User Space: %d\033[m\n", change_cnt);
    	printk(KERN_DEBUG "\033[35m[INFO] Voltage: %d\033[m\n", dwVoltage);
    	printk(KERN_DEBUG "\033[35m[INFO] VoltageType: %d\033[m\n", dwVoltageType);

		wait_event_interruptible_timeout(DVFS_on_demand_event_waitqueue_userspace_return, finished_change_cnt==change_cnt , MAX_SCHEDULE_TIMEOUT);
    	printk(KERN_DEBUG "\033[35m[INFO] Data Exchange Count from User Space: %d\033[m\n", finished_change_cnt);
    }
    else
    	printk("\033[35m[Return] Function = %s, start_userspace_ondemand_handshake is %d\033[m\n", __PRETTY_FUNCTION__, start_userspace_ondemand_handshake);
}
EXPORT_SYMBOL(MDrvDvfsVoltageSetup);
#endif

#if defined(CONFIG_CPU_FREQ_DEFAULT_GOV_ONDEMAND) || defined(CONFIG_CPU_FREQ_DEFAULT_GOV_INTERACTIVE)
static int integrator_set_target_on_demand(struct cpufreq_policy *policy, unsigned int target_freq, unsigned int relation)
{
	int cpu = policy->cpu;
	int cpu_id = get_cpu();
	put_cpu();
	cpumask_t cpus_allowed;

#ifdef CONFIG_MP_DVFS_CPUHOTPLUG
	/* get total_cpu_nr and real target_freq */
	unsigned int total_cpu_nr = target_freq >> 28;
	target_freq = target_freq & ~(total_cpu_nr << 28);
#endif

	if(target_freq == 0)
	{
		printk("\033[0;32;31m [DVFS] %s %d target_freq %d\033[m\n", __func__, __LINE__, target_freq);
		BUG_ON(1);
	}

#if 1
	/* only cpu0 with its own policy is allowed to adjust cpufreq(due to no mutex for now) */
	if(cpu_id != 0 || cpu != cpu_id)
	{
		printk(KERN_DEBUG "\033[31mFunction = %s, Line = %d, cpu_id is %d, policy->cpu is %d\033[m\n", __PRETTY_FUNCTION__, __LINE__, cpu_id, policy->cpu);
		return 0;
	}
#else
	/* only if policy->cpu is cpu0, we can do integrator_set_target_on_demand, used if you set policy->cpus to all cpu */
	if(cpu != 0)
	{
		printk("\033[31mFunction = %s, Line = %d, policy->cpu is %d, it should be 0\033[m\n", __PRETTY_FUNCTION__, __LINE__, policy->cpu);
		return 0;
	}
#endif

	mutex_lock(&mstar_cpufreq_lock);
	if( (!start_userspace_ondemand_handshake) || (atomic_read(&disable_dvfs) == 1) )
	{
		printk(KERN_DEBUG "\033[34mdoing integrator_set_target_on_demand, but start_userspace_ondemand_handshake is %d, disable_dvfs is %d\033[m\n", start_userspace_ondemand_handshake, atomic_read(&disable_dvfs));
		goto set_target_out;
	}
	else
	{
#ifdef CONFIG_MP_DVFS_CPUHOTPLUG
		printk(KERN_DEBUG "\033[32mFunction = %s, Line = %d, set online_cpu_nr to %d, cpufreq from %d KHz to %d KHz\033[m\n", __PRETTY_FUNCTION__, __LINE__, total_cpu_nr, policy->cur, target_freq);
#else
		printk(KERN_DEBUG "\033[35mFunction = %s, Line = %d, set copufreq from %d KHz to %d KHz\033[m\n", __PRETTY_FUNCTION__, __LINE__, policy->cur, target_freq);
#endif
		/*
		 * Save this threads cpus_allowed mask.
		 */
		cpus_allowed = current->cpus_allowed;

		/*
		 * Bind to the specified CPU.  When this call returns,
		 * we should be running on the right CPU.
		 */
		set_cpus_allowed(current, cpumask_of_cpu(cpu));
		BUG_ON(cpu != smp_processor_id());

		if(target_freq > policy->max)
		{
			printk(KERN_ERR "\033[31mFunction = %s, Line = %d, adjust target_freq from %d KHz to policy->max %d KHz (over_max case), over_thermal_flag is %d\033[m\n",
					__PRETTY_FUNCTION__, __LINE__, target_freq, policy->max, over_thermal_flag);
			target_freq = policy->max;
		}

		if (is_any_boost_client_running() == true)
		{
			target_freq = policy->max;
		}

#ifdef CONFIG_MP_DVFS_CPUHOTPLUG
		/* combine total_cpu_nr with target_freq, the leftest 4 bits will be total_cpu_nr */
		target_freq = target_freq | (total_cpu_nr << 28);
#endif

		/* update timer, cpu_freq(RIU), and scaling_cur_freq for all cpus */
		change_cpus_timer((char *)__FUNCTION__, target_freq, cpu);

		/*
		 * Restore the CPUs allowed mask.
		 */
		set_cpus_allowed(current, cpus_allowed);
	}

set_target_out:
	mutex_unlock(&mstar_cpufreq_lock);
	return 0;
}
#else
static int integrator_set_target(struct cpufreq_policy *policy,
				 unsigned int target_freq,
				 unsigned int relation)
{
	cpumask_t cpus_allowed;
	int cpu = policy->cpu;
	struct cpufreq_freqs freqs;

	/*
	 * Save this threads cpus_allowed mask.
	 */
	cpus_allowed = current->cpus_allowed;

	/*
	 * Bind to the specified CPU.  When this call returns,
	 * we should be running on the right CPU.
	 */
	set_cpus_allowed(current, cpumask_of_cpu(cpu));
	BUG_ON(cpu != smp_processor_id());

  	freqs.old = get_freq(); //  johnson

	/* icst_hz_to_vco rounds down -- so we need the next
	 * larger freq in case of CPUFREQ_RELATION_L.
	 */
	if (relation == CPUFREQ_RELATION_L)
		target_freq += 999;
	if (target_freq > policy->max)
		target_freq = policy->max;

	freqs.new = get_freq(); //johnson
	freqs.cpu = policy->cpu;

	if (freqs.old == freqs.new) {
		set_cpus_allowed(current, cpus_allowed);
		return 0;
	}

	cpufreq_notify_transition(policy, &freqs, CPUFREQ_PRECHANGE);

	/*
	 * Restore the CPUs allowed mask.
	 */
	set_cpus_allowed(current, cpus_allowed);

	cpufreq_notify_transition(policy, &freqs, CPUFREQ_POSTCHANGE);

	return 0;
}
#endif

#ifdef CONFIG_MP_DVFS_CPUHOTPLUG
static void mstar_update_cpu_online_nr(unsigned int requested_total_cpu_nr)
{
	int candidate_cpu = 0;
	unsigned int dvfs_mode = 0;
        int cpu_down_Err_ret = 0;
        int cpu_up_Err_ret = 0;

	/* str: total_cpu_nr = 0(do not update) */
	if(requested_total_cpu_nr ==  STR_CPU_NR)
		return;

    mutex_lock(&mstar_dvfs_cpuhotplug_lock);

	if(requested_total_cpu_nr > CONFIG_NR_CPUS)
	{
		printk("\033[31mrequested_total_cpu_nr is %u, error!!\033[m\n", requested_total_cpu_nr);
		BUG_ON(1);
	}

	/* to check if we are under boot_boost mode */
	dvfs_mode = reg_readw(0x1f200A0CUL);
	dvfs_mode &= ~(1 << 15);
	dvfs_mode = dvfs_mode >> 12;

	/* according to current_dvfs_mode to decide a satisfied requested_total_cpu_nr */
	if(MDrvDvfsGetOverTemperatureFlag() == 1)
	{
		/* over thermal case */
		printk(KERN_ERR "\033[31mFunction = %s, Line = %d, over thermal mode using OVER_THERMAL_CPU_NR: %d\033[m\n", __PRETTY_FUNCTION__, __LINE__, OVER_THERMAL_CPU_NR);
		requested_total_cpu_nr = OVER_THERMAL_CPU_NR;
	}
	else if (is_any_boost_client_running() == true)
	{
		/* boost mode */
		requested_total_cpu_nr = BOOST_CPU_NR;
	}
	else if (dvfs_mode == 5)
	{
		/* ac and str boost mode */
		requested_total_cpu_nr = BOOST_CPU_NR;
	}

       #if (defined CONFIG_MP_DVFS_ANTUTU_HOTPLUG) && (defined CONFIG_MP_DVFS_CPUHOTPLUG)

	if(antutu_exist() && get_antutu_hotplug_flag() == START_ANTUTU_HOTPLUG && MDrvDvfsGetOverTemperatureFlag() != 1)
	{
	     requested_total_cpu_nr = MdrvDvfsGetAntutuCpuNR();

	}
	#endif

	if(requested_total_cpu_nr < num_online_cpus())	// cpu_down (from N-1 to 1)
	{
		for(candidate_cpu = CONFIG_NR_CPUS-1; candidate_cpu > 0; candidate_cpu--)
		{
			if(cpu_online(candidate_cpu))
			{       
				//printk("\033[31mFunction = %s, Line = %d, cpu_down for cpu%d, cpu_online_mask is %u\033[m\n", __PRETTY_FUNCTION__, __LINE__, candidate_cpu, *cpu_online_mask);
                                cpu_down_Err_ret = cpu_down(candidate_cpu);
				if(cpu_down_Err_ret !=0)
				{
                                        printk(KERN_ERR "\033[31m[Error], fail to power down cpu%d, cpu_down_Err_ret = %d\033[m\n", candidate_cpu, cpu_down_Err_ret);
                                        #ifdef CONFIG_MP_PLATFORM_DVFS_GET_ERR_STATE_FROM_CPUHOTPLG_DISABLE
                                        if(cpu_hotplug_GetStatus_disable_Err_busy() == 1)
                                        {
                                                /* 
                                                 * cpu_hotplug_GetStatus_disable_Err_busy() return 1
                                                 * this case occur when cpu_down flow detect cpuhotplug has been disbled
                                                 */
                                                printk(KERN_ERR "\033[31m[Error], fail to due to cpu hotplug diable%d\033[m\n");                               
                                                goto out;
                                        }
                                        #endif
				}
				//printk("\033[31m[DONE cpu_down for cpu%d] cpu_online_mask is %u\033[m\n", candidate_cpu, *cpu_online_mask);

#if defined(CONFIG_MP_DVFS_CPUHOTPLUG_USE_ONLINE_CPU_MAX_LOAD) || defined(CONFIG_MP_DVFS_CPUHOTPLUG_USE_ONLINE_CPU_AVERAGE_LOAD)
				mutex_lock(&mstar_cpuload_lock);
				mstar_cpu_load_freq[candidate_cpu] = 0;
				mutex_unlock(&mstar_cpuload_lock);
#endif
			}

			if(requested_total_cpu_nr == num_online_cpus())
			{
				goto out;
			}
		}
	}
	else if (requested_total_cpu_nr > num_online_cpus())	// cpu_up (from 1 to N-1)
	{
		for(candidate_cpu = 1; candidate_cpu < CONFIG_NR_CPUS; candidate_cpu++)
		{
			if(!cpu_online(candidate_cpu))
			{
				//printk("\033[31mFunction = %s, Line = %d, cpu_up for cpu%d, cpu_online_mask is %u\033[m\n", __PRETTY_FUNCTION__, __LINE__, candidate_cpu, *cpu_online_mask);
                                cpu_up_Err_ret = cpu_up(candidate_cpu);
				if(cpu_up_Err_ret !=0)
				{
                                        printk(KERN_ERR "\033[31m[Error], fail to power on cpu%d, cpu_up_Err_ret = %d \033[m\n", candidate_cpu, cpu_up_Err_ret);
                                        #ifdef CONFIG_MP_PLATFORM_DVFS_GET_ERR_STATE_FROM_CPUHOTPLG_DISABLE
                                        if(cpu_hotplug_GetStatus_disable_Err_busy() == 1)
                                        {
                                                /* 
                                                 * cpu_hotplug_GetStatus_disable_Err_busy() return 1
                                                 * this case occur when cpu_up flow detect cpuhotplug has been disbled
                                                 */
                                                printk(KERN_ERR "\033[31m[Error], fail to due to cpu hotplug diable%d\033[m\n"); 
                                                goto out;                                                                                                                                        
                                        }
                                        #endif
				}
				//printk("\033[31m[DONE cpu_up for cpu%d] cpu_online_mask is %u\033[m\n", candidate_cpu, *cpu_online_mask);
			}

			if(requested_total_cpu_nr == num_online_cpus())
			{
				goto out;
			}
		}
	}
	else if (requested_total_cpu_nr == num_online_cpus())
	{
		//printk("\033[35mFunction = %s, Line = %d, already same cpu_nr\033[m\n", __PRETTY_FUNCTION__, __LINE__);
		goto out;
	}
	else
		BUG_ON(1);

	if(requested_total_cpu_nr != num_online_cpus())
	{
		printk(KERN_ERR "\033[31m[Error], requested_total_cpu_nr is %u, but num_online_cpus() is %u\033[m\n", requested_total_cpu_nr, num_online_cpus());
		BUG_ON(1);
	}
out:

	mutex_unlock(&mstar_dvfs_cpuhotplug_lock);
	return;

	return;
}

void update_mstar_cpu_dvfs_hotplug_workload_table(void)
{
	unsigned int region_high, region_min;
	int i = 0;

	mutex_lock(&mstar_cpuload_threshold);
	if(!show_mstar_cpu_dvfs_hotplug_scenario_table)
	{
		printk("\033[31mUpdate mstar_cpu_dvfs_hotplug_workload_table\033[m\n");
		for(i = 0; i < CPU_DVFS_HOTPLUG_LEVEL_MAX; i++)
		{
#if 0
			/* Rule_1 */
			if(i == 0)
				region_min = 0;
			else
				region_min = ( (mstar_cpu_dvfs_hotplug_scenario[i].total_cpu_nr * mstar_cpu_dvfs_hotplug_scenario[i].target_freq) +
					(mstar_cpu_dvfs_hotplug_scenario[i-1].total_cpu_nr * mstar_cpu_dvfs_hotplug_scenario[i-1].target_freq) ) / 2;

			if(i == (CPU_DVFS_HOTPLUG_LEVEL_MAX - 1))
				region_high = 0xFFFFFFFF;
			else
				region_high = ( (mstar_cpu_dvfs_hotplug_scenario[i].total_cpu_nr * mstar_cpu_dvfs_hotplug_scenario[i].target_freq) +
					(mstar_cpu_dvfs_hotplug_scenario[i+1].total_cpu_nr * mstar_cpu_dvfs_hotplug_scenario[i+1].target_freq) ) / 2;

			mstar_cpu_dvfs_hotplug_workload_table[i].supported_workload_lower_bound = region_min * mstar_cpu_dvfs_hotplug_scenario[i].mstar_adj_up_threshold;
			mstar_cpu_dvfs_hotplug_workload_table[i].supported_workload_upper_bound = region_high * mstar_cpu_dvfs_hotplug_scenario[i].mstar_adj_up_threshold;
#else
			/* Rule_2 */
			if(i == 0)
				mstar_cpu_dvfs_hotplug_workload_table[i].supported_workload_lower_bound = 0;
			else
				mstar_cpu_dvfs_hotplug_workload_table[i].supported_workload_lower_bound = mstar_cpu_dvfs_hotplug_workload_table[i-1].supported_workload_upper_bound;

			if(i == (CPU_DVFS_HOTPLUG_LEVEL_MAX - 1))
				mstar_cpu_dvfs_hotplug_workload_table[i].supported_workload_upper_bound = 0xFFFFFFFF;
			else
				mstar_cpu_dvfs_hotplug_workload_table[i].supported_workload_upper_bound =
					(mstar_cpu_dvfs_hotplug_scenario[i].total_cpu_nr * mstar_cpu_dvfs_hotplug_scenario[i].target_freq) * mstar_cpu_dvfs_hotplug_scenario[i].mstar_up_threshold;
#endif

			printk("\033[31mfor scenario_%d: cpu_nr: %d, target_freq: %dkHz\033[m\n", i, mstar_cpu_dvfs_hotplug_scenario[i].total_cpu_nr, mstar_cpu_dvfs_hotplug_scenario[i].target_freq);
			printk("\033[35m    supported_workload_lower_bound: %u, supported_workload_upper_bound: %u\033[m\n\n",
				mstar_cpu_dvfs_hotplug_workload_table[i].supported_workload_lower_bound, mstar_cpu_dvfs_hotplug_workload_table[i].supported_workload_upper_bound);
		}
		//show_mstar_cpu_dvfs_hotplug_scenario_table = 1;
	}
	mutex_unlock(&mstar_cpuload_threshold);

	return;
}
#endif

void change_cpus_timer(char *caller, unsigned int target_freq, unsigned int cpu_id)
{
	int i = 0;
	int ret[CONFIG_NR_CPUS] = {0};
	int ret_value = 1;
	unsigned int ori_target_freq = 0;
	struct cpufreq_policy *other_cpu_policy;
	struct cpufreq_freqs freqs;

	if(bootarg_dvfs_disable)
		return;

#ifdef CONFIG_MP_DVFS_CPUHOTPLUG
	/* get total_cpu_nr and real target_freq */
	unsigned int total_cpu_nr = target_freq >> 28;
	target_freq = target_freq & ~(total_cpu_nr << 28);
#endif

	printk(KERN_DEBUG "\033[36mcaller: %s, target_freq: %d KHz\033[m\n", caller, target_freq);

	ori_target_freq = target_freq;

	get_online_cpus();	// to avoid using policy, but having cpuhotplug at the same time
	for_each_online_cpu(i)
	{
		other_cpu_policy = cpufreq_cpu_get(i);

		if(!other_cpu_policy) // to avoid some cpus have not had its policy
		{
			printk("\033[35mFunction = %s, Line = %d, cpu%d do: [cpu%d] policy is NULL\033[m\n", __PRETTY_FUNCTION__, __LINE__, get_cpu(), i);

			put_cpu();
			ret[i] = 0;
      		ret_value = ret_value * ret[i];
			continue;
		}

		ret[i] = 1;
		ret_value = ret_value * ret[i];
		cpufreq_cpu_put(other_cpu_policy);
	}
	put_online_cpus();

	if (ret_value)
	{
		/*
		 * set all cpus to new cpu_freq(scaling_cur_freq), and set the timer
		 * always let wait_for_voltage to do voltage_change, adjust target_freq according to Temperature, write adjusted target_freq to RIU, then use new target_freq to set timer, jiffes
		 */

		/* rise voltage first, wait for voltage change(accroding to target_freq) */
		target_freq = MDrvDvfsProc(target_freq, 0);
		printk(KERN_DEBUG "\033[35mFunction = %s, Line = %d, the target_freq is changed(by MDrvDvfsProc), from %d KHz to %d KHz\033[m\n", __PRETTY_FUNCTION__, __LINE__, ori_target_freq, target_freq);

		if(voltage_change_result != 1) // pass in userspace voltage change
		{
#if (!defined CONFIG_MP_STATIC_TIMER_CLOCK_SOURCE) && (!defined CONFIG_MP_GLOBAL_TIMER_12MHZ_PATCH)
			mstar_update_sched_clock();
#endif // CONFIG_MP_STATIC_TIMER_CLOCK_SOURCE

			get_online_cpus();
			for_each_online_cpu(i)
			{
				other_cpu_policy = cpufreq_cpu_get(i);

				if(!other_cpu_policy)
				{
					printk("\033[35mFunction = %s, Line = %d, cpu%d do: [cpu%d] policy is NULL\033[m\n", __PRETTY_FUNCTION__, __LINE__, get_cpu(), i);
					put_cpu();
					BUG_ON(!other_cpu_policy);
				}

				if(other_cpu_policy->cur == target_freq)
				{
					cpufreq_cpu_put(other_cpu_policy);
					continue;
				}

				freqs.cpu = i;
				freqs.old = other_cpu_policy->cur;
				freqs.new = target_freq;

				if(i == 0)
				{
					printk(KERN_DEBUG "\033[31mFunction = %s, Line = %d, cpu%d do: [cpu%d] adjust cpufreq from %d KHZ to %d KHZ\033[m\n", __PRETTY_FUNCTION__, __LINE__, get_cpu(), i, freqs.old, freqs.new);
					put_cpu();
				}

				cpufreq_notify_transition(other_cpu_policy, &freqs, CPUFREQ_PRECHANGE);
				cpufreq_notify_transition(other_cpu_policy, &freqs, CPUFREQ_POSTCHANGE);
				cpufreq_cpu_put(other_cpu_policy);
			}
			put_online_cpus();

#ifdef CONFIG_MP_DVFS_CPUHOTPLUG
			/* mstar cpu_hotplug policy */
			mstar_update_cpu_online_nr(total_cpu_nr);
#endif

#if (!defined CONFIG_MP_STATIC_TIMER_CLOCK_SOURCE) && (!defined CONFIG_MP_GLOBAL_TIMER_12MHZ_PATCH)
			if(other_cpu_policy->cur != target_freq)
			{
				change_interval(freqs.old, freqs.new);
			}
#endif
			current_frequency = target_freq;
		}
	}
}

#if defined(CONFIG_CPU_FREQ_DEFAULT_GOV_ONDEMAND) || defined(CONFIG_CPU_FREQ_DEFAULT_GOV_INTERACTIVE)
static struct cpufreq_driver integrator_driver = {
	.verify		= integrator_verify_policy,
	.target		= integrator_set_target_on_demand,
	.get		= integrator_get,
	.init		= integrator_cpufreq_init,
	.name		= "ondemand",
};
#else
static struct cpufreq_driver integrator_driver = {
	.verify		= integrator_verify_policy,
	.target		= integrator_set_target,
	.get		= integrator_get,
	.init		= integrator_cpufreq_init,
	.name		= "integrator",
};
#endif

static int CPU_calibrating_proc_open(struct inode *inode, struct file *file)
{
	if (atomic_read(&proc_is_open))
		return -EACCES;

	atomic_set(&proc_is_open, 1);
	return 0;
}

static int CPU_calibrating_proc_release(struct inode *inode, struct file * file)
{

	WARN_ON(!atomic_read(&proc_is_open));
	atomic_set(&proc_is_open, 0);
	return 0;
}

static int on_demand_handshake_proc_open(struct inode *inode, struct file *file)
{
	if(!bootarg_dvfs_disable)
		start_userspace_ondemand_handshake = 1;

	if(atomic_read(&on_demand_handshake_is_open))
		return -EACCES;

        ready_to_change_voltage = MDrvDvfsQueryCpuPower(CONFIG_DVFS_DEFAULT_CPU_POWER);
	atomic_set(&on_demand_handshake_is_open, 1);

	return 0;
}

static int on_demand_handshake_proc_release(struct inode *inode, struct file * file)
{
	printk("\033[35mFunction = %s, Line = %d, set start_userspace_ondemand_handshake to be 0\033[m\n", __PRETTY_FUNCTION__, __LINE__); // joe.liu
	start_userspace_ondemand_handshake = 0;

	WARN_ON(!atomic_read(&on_demand_handshake_is_open));
	atomic_set(&on_demand_handshake_is_open, 0);
	return 0;
}

ssize_t on_demand_handshake_proc_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	ON_Demand_From_Userspace from_user_data;

	if(!start_userspace_ondemand_handshake)
	{
		printk("\033[35mFunction = %s, start_userspace_ondemand_handshake is %d\033[m\n", __PRETTY_FUNCTION__, start_userspace_ondemand_handshake);
		return  -EFAULT;
	}
	else
	{
		if(!count)
		{
			printk("\033[35mFunction = %s, Line = %d\033[m\n", __PRETTY_FUNCTION__, __LINE__); // joe.liu
			return count;
		}

		if(count >= MAX_DMSG_WRITE_BUFFER)
			count = MAX_DMSG_WRITE_BUFFER - 1;

		copy_from_user(&from_user_data, buf, sizeof(from_user_data));
		finished_change_cnt = from_user_data.from_userspace_finished_change_cnt;
		voltage_change_result = from_user_data.from_userspace_voltage_change_result;

		printk(KERN_DEBUG "\033[33m[result is %d] input: %d, wake_up wait_queue for write_down\033[m\n", voltage_change_result, finished_change_cnt);
		wake_up_interruptible(&DVFS_on_demand_event_waitqueue_userspace_return);	// to wake_up a wait_queue waiting for voltage change

		return count;
	}
}

ssize_t on_demand_handshake_proc_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	ON_Demand_To_Userspace to_userspace_data;
	int err;

	if(!start_userspace_ondemand_handshake)
	{
		if(bootarg_dvfs_disable)
		{
			printk("\033[31mFunction = %s, Line = %d, [Warning] DVFS=disable is set @ bootargs, the dvfs_thread will wait forever\033[m\n", __PRETTY_FUNCTION__, __LINE__); // joe.liu
			down_interruptible(&DVFS_disable_SEM);
		}

		printk("\033[35mFunction = %s, start_userspace_ondemand_handshake is %d\033[m\n", __PRETTY_FUNCTION__, start_userspace_ondemand_handshake);
		return  -EFAULT;
	}
	else
	{
		down_interruptible(&DVFS_on_demand_event_SEM);

		to_userspace_data.to_userspace_cpufreq = ready_to_change_cpufreq;
		to_userspace_data.to_userspace_voltage = ready_to_change_voltage;
		to_userspace_data.to_userspace_voltage_type = ready_to_change_voltage_type;
		to_userspace_data.to_userspace_change_cnt = change_cnt;

		err = copy_to_user((void *)buf, &to_userspace_data, sizeof(to_userspace_data));

		*ppos += sizeof(to_userspace_data);

		return sizeof(to_userspace_data);
	}
}

const struct file_operations proc_CPU_calibrating_operations = {
	.write      = CPU_calibrating_proc_write,
	.open       = CPU_calibrating_proc_open,
	.release    = CPU_calibrating_proc_release,
  	.unlocked_ioctl  = CPU_calibrating_proc_ioctl,
};


//--------------------------------------------------

#if (defined CONFIG_MP_DVFS_ANTUTU_HOTPLUG) && (defined CONFIG_MP_DVFS_CPUHOTPLUG)

bool antutu_exist()
{
     struct task_struct *tsk;
     bool ret = false;

     for_each_process (tsk)
     {

	  if (strstr(tsk->comm, "ABenchMark") || \
	      strstr(tsk->comm, "antutu2dtest") || \
	      strstr(tsk->comm, "antutu3dtest") || \
	      strstr(tsk->comm, "bench64") || \
	      strstr(tsk->comm, "remote2d") || \
	      strstr(tsk->comm, "benchmark.full"))
              {
                  ret =true;
		    break;
              }

       }

       return ret;

}


void set_antutu_hotplug_flag(int flag)
{
     mutex_lock(&antut_hotplug_lock);

     antutu_hotplug_flag = flag;

     mutex_unlock(&antut_hotplug_lock);
}

int get_antutu_hotplug_flag()
{
     int ret;

     mutex_lock(&antut_hotplug_lock);

     ret = antutu_hotplug_flag ;

     mutex_unlock(&antut_hotplug_lock);

    return ret;

}

static int antutu_hotplug_proc_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int antutu_hotplug_proc_release(struct inode *inode, struct file * file)
{
	return 0;
}

ssize_t antutu_hotplug_proc_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{

	const unsigned int BUFFSIZE = 8;
	char buffer[BUFFSIZE];
	int selection;
	if (count >= BUFFSIZE)
		count = BUFFSIZE - 1;


	if (copy_from_user(buffer, buf, count))
		return -EFAULT;

       selection = simple_strtol(buffer, NULL, 10);

       set_antutu_hotplug_flag(selection);

       return count;
}



const struct file_operations proc_antutu_hotplug_operations = {
	.write      = antutu_hotplug_proc_write,
	.open       = antutu_hotplug_proc_open,
	.release    = antutu_hotplug_proc_release,
  	.unlocked_ioctl  = NULL,
};


#endif


//-----------------------------------------------------

const struct file_operations proc_on_demand_handshake_operations = {
	.open       = on_demand_handshake_proc_open,
	.write      = on_demand_handshake_proc_write,
	.read		= on_demand_handshake_proc_read,
	.release    = on_demand_handshake_proc_release,
};

#if defined(CONFIG_CPU_FREQ_DEFAULT_GOV_ONDEMAND) || defined(CONFIG_CPU_FREQ_DEFAULT_GOV_INTERACTIVE)
#define BOOST_AGING_TIMEOUT_IN_MS 600000 //10*60*1000
static DEFINE_SPINLOCK(T_sensor_lock);
static void Mdrv_CPU_T_sensor_Check_callback(unsigned long value)
{
	int i = 0;
	int T_sensor_max_freq = 0;
	unsigned long echo_calibrating_freq_tmp = 0;

	if (is_any_boost_client_running() == true) {
		if (jiffies_to_msecs(jiffies - jiffies_boost_lasttime) > BOOST_AGING_TIMEOUT_IN_MS) {
			printk(KERN_DEBUG "[dvfs boost] timout happens and delete all of the running clients\n");
			del_all_boost_client();
#if !defined(CONFIG_CPU_FREQ_DEFAULT_GOV_ONDEMAND) && !defined(CONFIG_CPU_FREQ_DEFAULT_GOV_INTERACTIVE)
            __CPU_calibrating_proc_write(1008000);
#endif
		} else {
			// nothing
		}
	} else {
		// nothing
	}

	if(!start_userspace_ondemand_handshake)
	{
		spin_lock(&T_sensor_lock);
#if (MP_PLATFORM_T_SENSOR_OBSERVATION == 0)
		Check_T_sensor_timer.expires = jiffies + FREQ_CHECK_LINK_TIME;
		add_timer(&Check_T_sensor_timer);
#endif
		spin_unlock(&T_sensor_lock);

		printk(KERN_DEBUG "\033[34mFunction = %s, return Mdrv_CPU_T_sensor_Check_callback, start_userspace_ondemand_handshake is %d\033[m\n", __PRETTY_FUNCTION__, start_userspace_ondemand_handshake);
		return;
	}

	T_sensor_max_freq = MDrvDvfsQueryCpuClockByTemperature(0);
	printk(KERN_DEBUG "\033[34mFunction = %s, Line = %d, do Mdrv_CPU_T_sensor_Check_callback: %d, over_thermal_flag is %d\033[m\n", __PRETTY_FUNCTION__, __LINE__, T_sensor_max_freq, over_thermal_flag);

	if(T_sensor_max_freq == 0){
		printk("\033[0;32;31m [DVFS] %s %d T_sensor_max_freq %d\033[m\n",__func__,__LINE__,T_sensor_max_freq);
		BUG_ON(1);
	}

#if defined(CONFIG_MP_DVFS_VID_ONLY)
        get_online_cpus();
        for_each_online_cpu(i)
        {    
                if(T_sensor_max_freq == 100000) // this is a special case, only adjust the cpu_voltage only
                {    
                        T_sensor_max_freq = ondemand_timer[i]->max;
                        /* change to set a flag to forcibly trigger integrator_set_target_on_demand() in od_check_cpu() */
                        forcibly_set_target_flag = 5; 
                }    
                else 
                {    
                        if( (i == 0) && (forcibly_set_target_flag > 0) )
                        {
                                forcibly_set_target_flag--;
                        }        
                }    
        }    
        put_online_cpus();
#elif (!defined CONFIG_MP_DVFS_FORCE_PINGO_TEST) && (!defined CONFIG_MP_DVFS_VID_ONLY)
	if (is_any_boost_client_running() == true)
	{
		echo_calibrating_freq_tmp = read_echo_calibrating_freq();
		if(echo_calibrating_freq_tmp < T_sensor_max_freq)
		{
			T_sensor_max_freq = echo_calibrating_freq_tmp;
			if(T_sensor_max_freq == 0){
			printk("\033[0;32;31m [DVFS] %s %d T_sensor_max_freq %d\033[m\n",__func__,__LINE__,T_sensor_max_freq);
			BUG_ON(1);
			}
		}
	}
	else if(T_sensor_max_freq > MAX_CPU_FREQ)
	{
		T_sensor_max_freq = MAX_CPU_FREQ;
		if(T_sensor_max_freq == 0){
			printk("\033[0;32;31m [DVFS] %s %d T_sensor_max_freq %d\033[m\n",__func__,__LINE__,T_sensor_max_freq);
			BUG_ON(1);
		}
	}

	get_online_cpus();
	for_each_online_cpu(i)
	{
		if(T_sensor_max_freq == 100000)	// this is a special case, only adjust the cpu_voltage only
		{
            T_sensor_max_freq = ondemand_timer[i]->max;
            forcibly_set_target_flag = 5;
        }
        else
		{
	    	ondemand_timer[i]->max = T_sensor_max_freq;

			if(T_sensor_max_freq < ondemand_timer[i]->cur)
    	    {
	    	    printk("\033[31mFunction = %s, Line = %d, T_sensor_max_freq < ondemand_timer[%d]->cur\033[m\n", __func__, __LINE__, i);
    			printk("\033[34mFunction = %s, Line = %d, T_sensor_max_freq: %d\033[m\n", __PRETTY_FUNCTION__, __LINE__, T_sensor_max_freq);
    			printk("\033[34mFunction = %s, Line = %d, ondemand_timer[%d]->cur: %d\033[m\n", __PRETTY_FUNCTION__, __LINE__, i, ondemand_timer[i]->cur);

				/* change to set a flag to forcibly trigger integrator_set_target_on_demand() in od_check_cpu() */
				over_thermal_flag = 2;
    		}
	    	else
	    	{
				if( (i == 0) && (forcibly_set_target_flag > 0) )
					forcibly_set_target_flag--;

				if( (i == 0) && (over_thermal_flag > 0) )
					over_thermal_flag--;
	   		}
       }
	}
	put_online_cpus();
#endif

	spin_lock(&T_sensor_lock);
#if (MP_PLATFORM_T_SENSOR_OBSERVATION == 0)
	Check_T_sensor_timer.expires = jiffies + FREQ_CHECK_LINK_TIME;
	add_timer(&Check_T_sensor_timer);
#endif
	spin_unlock(&T_sensor_lock);
}
#else
static void Mdrv_CPU_Freq_Check_callback(unsigned long value)
{
	int freq, i;
	struct cpufreq_freqs freqs;
	struct cpufreq_policy *policy;

	if(*(volatile u32 *)(0xfd200a00) == 0x3697)
   	{
   		freq = *(volatile u32 *)(0xfd200a04);
   		freq = freq * 1000;

   		if (freq != current_frequency)
   		{
#if (!defined CONFIG_MP_STATIC_TIMER_CLOCK_SOURCE) && (!defined CONFIG_MP_GLOBAL_TIMER_12MHZ_PATCH)
			mstar_update_sched_clock();
#endif // CONFIG_MP_STATIC_TIMER_CLOCK_SOURCE
			printk(KERN_DEBUG "\033[36m\nFunction = %s, Line = %d, (freq = %d KHZ) != (current_frequency = %d KHZ)\033[m\n", __PRETTY_FUNCTION__, __LINE__, freq, current_frequency);
			if(freq < MIN_CPU_FREQ)
			{
					printk(KERN_WARNING "\033[36m\n freq %d KHZ < MIN_CPU_FREQ %d KHZ ,not allowed\033[m\n",freq,MIN_CPU_FREQ);
					return;
			}

			if(freq > MAX_CPU_FREQ)
			{
					printk(KERN_WARNING "\033[36m\n freq %d KHZ > MAX_CPU_FREQ %d KHZ ,not allowed\033[m\n",freq,MAX_CPU_FREQ);
					return;
			}

			register_frequency = freq;

	get_online_cpus();
			for_each_online_cpu(i)
	        {
	        	policy=cpufreq_cpu_get(i);
	        	freqs.cpu = i;
		        freqs.old = current_frequency;
		        freqs.new = freq;
		        cpufreq_notify_transition(policy, &freqs, CPUFREQ_PRECHANGE);
              	 cpufreq_notify_transition(policy, &freqs, CPUFREQ_POSTCHANGE);
            	cpufreq_cpu_put(policy);
            }
	put_online_cpus();

			current_frequency = freq;
#if (!defined CONFIG_MP_STATIC_TIMER_CLOCK_SOURCE) && (!defined CONFIG_MP_GLOBAL_TIMER_12MHZ_PATCH)
			change_interval(freqs.old, freqs.new);
#endif
   		}
   	}
	Check_Freq_timer.expires = jiffies + FREQ_CHECK_LINK_TIME;
	add_timer(&Check_Freq_timer);
}
#endif

static int __init init_procfs_msg(void)
{

       int i,j;
       struct cpufreq_frequency_table *freq_table;
	ORI_FREQ_KHZ = query_frequency() * 1000;

	current_frequency = ORI_FREQ_KHZ;

	printk(KERN_DEBUG "\033[35mFunction = %s, Line = %d, [cpu %d] current_frequency = %d\033[m\n", __PRETTY_FUNCTION__, __LINE__, get_cpu(), current_frequency);
	put_cpu();

      //shihong test start
     	get_online_cpus();
	for_each_online_cpu(i)
       {
           freq_table = kzalloc(sizeof(struct cpufreq_frequency_table) * DVFS_FREQ_LEVEL_MAX_INDEX, GFP_KERNEL);
           if (!freq_table) 
	    {
                  printk("\033[35m%s: Unable to allocate frequency table \033[m\n", __PRETTY_FUNCTION__);
		    put_online_cpus();
		    return -ENOMEM;
           }
        for (j = 0; j < DVFS_FREQ_LEVEL_MAX_INDEX; j ++) {
            freq_table[j].frequency = hal_freq_table[j].frequency;  //hal_freq_table is in chip_dvfs_calibrating.h
            freq_table[j].index = hal_freq_table[j].index;
        }
        cpufreq_frequency_table_get_attr(&freq_table[0], i);

	    
       }
	put_online_cpus();

     //shihong test end
	

	cpufreq_register_driver(&integrator_driver);
	proc_create("CPU_calibrating", S_IRUSR | S_IWUSR, NULL, &proc_CPU_calibrating_operations);
	proc_create("on_demand_ctl"  , S_IRUSR | S_IWUSR, NULL, &proc_on_demand_handshake_operations);
	sema_init(&DVFS_on_demand_event_SEM, 0);

       #if (defined CONFIG_MP_DVFS_ANTUTU_HOTPLUG) && (defined CONFIG_MP_DVFS_CPUHOTPLUG)
	proc_create("antutu_hotplug", 0x777, NULL, &proc_antutu_hotplug_operations);
       #endif

#if !defined(CONFIG_CPU_FREQ_DEFAULT_GOV_ONDEMAND)  && !defined(CONFIG_CPU_FREQ_DEFAULT_GOV_INTERACTIVE)
	init_timer(&Check_Freq_timer);
    Check_Freq_timer.data = 1;
    Check_Freq_timer.function = Mdrv_CPU_Freq_Check_callback;
    Check_Freq_timer.expires = jiffies + FREQ_CHECK_LINK_TIME;
    add_timer(&Check_Freq_timer);
#endif

	return 0;
}

static int __init CPU_calibrating_init(void)
{
	printk("\033[35mFunction = %s, Line = %d, [cpu %d] do CPU_calibrating_init\033[m\n", __PRETTY_FUNCTION__, __LINE__, get_cpu());
	put_cpu();

	init_procfs_msg();

#if (MP_PLATFORM_T_SENSOR_OBSERVATION == 1)
/*patch for add_timer issue(york)*/
	if(t_sensor_tsk == NULL)
	{
		printk("\033[31m[Error!!!] t_sensor_tsk is NULL\033[m\n");
		WARN_ON(1);
	}
/*patch for add_timer issue*/
#endif

	//update_mstar_cpu_dvfs_hotplug_workload_table();	// if changing mstar_up_threshold / mstar_adj_up_threshold
	first_init_done = 1;
	return 0;
}

static int __init DVFS_enable(char *str)
{
    if(strcmp(str, "disable") == 0)
    {
        printk("\nDVFS_disable\n");
	    bootarg_dvfs_disable = 1;
	    sema_init(&DVFS_disable_SEM, 0);
    }
    else
    {
	    bootarg_dvfs_disable = 0;
    }
    return 0;
}
early_param("DVFS", DVFS_enable);

module_init(CPU_calibrating_init);
