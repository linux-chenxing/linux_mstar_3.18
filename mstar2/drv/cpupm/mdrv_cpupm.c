
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/percpu-defs.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/cpu.h>
#include <linux/tick.h>
#include <linux/kernel_stat.h>
#include <linux/workqueue.h>
#include <linux/percpu.h>
#include <linux/percpu-defs.h>
#include <linux/spinlock.h>
#include <linux/cpumask.h>
#include <linux/cpufreq.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <mach/pm.h>
#include <linux/completion.h>
#include <linux/smp.h>
#include <chip_dvfs_calibrating.h>
#include <linux/topology.h>
#include <linux/cpufeature.h>
#include <linux/platform_device.h>
#include <linux/node.h>
#include <linux/kthread.h>
//#include <cpufreq_governor.h>
#include <linux/suspend.h>
#include "mhal_cpupm.h"

#define CPUPM_DEBUG KERN_INFO
#define CPUPM_MSG 1

#define NOT_FOUND_CPU  -1


#define WAITTIME 300
#define CPUPM_SAMPLE_TIME 100000
#define SHOWINFO_TIME 3000000
#define DETECT_TEMPERATURE_LATENCY (4800000) //delay 4.8 seconds


#define BOOST_MODE 1
#define NON_BOOST_MODE 0


#define CPUFREQ_GOVERNOR_SUCCESS 0

#define performance_str "performance"



unsigned short get_acpu_power_stat(unsigned int cpu);


typedef struct _cpupm_history
{
     u64 total_online_cpunum;
     u64 count;
}cpupm_history;


typedef struct _scenario_app
{
    char app_name[256];
    s32 occupy_cpu;

}scenario_app;



#define NR_SECENARIO_APP 8

scenario_app scenario_app_array[NR_SECENARIO_APP] =
{
    {"test1",1},
    {"test2",1},
    {"test3",1},
    {"test4",1},
    {"test5",1},
    {"test6",1},
    {"test7",1},
    {"test8",1},

};

cpupm_history cpu_history;

typedef enum {
	UP = 70,
	DOWN=45,
} CPU_THRESHOLD;

typedef enum {
	CPU_STATE_UNKNOWN=0,
	CPU_STATE_UP,
       CPU_STATE_BUFFER,
	CPU_STATE_DOWN,
} CPU_STATE;

/*
typedef enum {
     UNKNOWN_CPU_STAGE = 0,
     START_ADJUST_CPU_STAGE,
     END_ADJUST_CPU_STAGE,
}ADJUST_CPU_STAGE;
*/

static DEFINE_PER_CPU(struct cpu_info, cpuinfo);
static DEFINE_MUTEX(cpupm_cpu_hotplug_mutex);
static DEFINE_MUTEX(cpupm_adjust_cpu_mutex);


static struct workqueue_struct *cpuhotplug_wq;
static struct delayed_work cpuhotplug_work;

static struct workqueue_struct *cpuload_wq;
static struct delayed_work cpuload_work;
static struct kobject *cpupm_kobject;


static u32 delay;
static u32 prev_state = CPU_STATE_UNKNOWN;
static int occupy_nr_cpus=0;
static bool stop_adjust_cpu_flag = false;
static u32 cpupm_time_slice;

int cpu_up_flag =0;

/*
static char *cpupm_performance_gov = "performance";
static char *cpupm_ondemand_gov = "ondemand";
*/


typedef struct cpunormalmask { DECLARE_BITMAP(bits, NR_CPUS); } cpu_normal_mask_t;

static struct timer_list loading_timer;
static bool start_loading_timer_flag;


static u32 cpu_up_threshold;
static u32 cpu_down_threshold;


int get_lightest_cpu(void);
int get_coolest_cpu(void);
int get_scenario_cpu(void);
int get_running_cpu(void);
static int get_average_cpu_load(void);
void mstar_set_secondary_cpu_kernel_entry(void);


struct cpu_info* get_cpu_info(int cpu)
{
       struct cpu_info *info;

       if(cpu < 0)
	    return NULL;

	info = &per_cpu(cpuinfo,cpu);

	return info;
}

void cpupm_adjust_cpu_driver_lock()
{
      mutex_lock(&cpupm_adjust_cpu_mutex);
}


void cpupm_adjust_cpu_driver_unlock()
{
      mutex_unlock(&cpupm_adjust_cpu_mutex);
}

void cpupm_hotplug_driver_lock()
{
	mutex_lock(&cpupm_cpu_hotplug_mutex);
}

void cpupm_hotplug_driver_unlock()
{
	mutex_unlock(&cpupm_cpu_hotplug_mutex);
}


#if defined(CONFIG_CPU_FREQ)
int change_scaling_governor(unsigned int cpu,const char *new_governor);
#endif

int get_boost_status()
{

     return NON_BOOST_MODE;
}



void init_cpupm_time_slice()
{
       #if defined(CONFIG_CPU_FREQ)

       if(TRANSITION_LATENCY < CPUPM_SAMPLE_TIME)
           cpupm_time_slice = CPUPM_SAMPLE_TIME;
	 else
	    cpupm_time_slice = DETECT_TEMPERATURE_LATENCY;

       #else

	   cpupm_time_slice = CPUPM_SAMPLE_TIME;

	#endif
}


#if 1

int  _cpupm_cpu_up(int cpu_id)
{

     s32 ret = 0;
     s32 timeout;
      struct cpu_info *info;
     struct device *dev = get_cpu_device(cpu_id);
     struct cpu *cpu = container_of(dev, struct cpu, dev);
     int cpuid = cpu->dev.id;

     printk(KERN_INFO "\033[31m%s %d:cpupm_cpu_up: cpuid:%d \033[m\n",__func__,__LINE__,cpuid);
     int from_nid, to_nid;

  //   from_nid = cpu_to_node(cpuid);



     ret = cpu_up(cpuid);

/*
     to_nid = cpu_to_node(cpuid);

     if (from_nid != to_nid)
	   change_cpu_under_node(cpu, from_nid, to_nid);

*/
    if (!ret)
	 kobject_uevent(&dev->kobj, KOBJ_ONLINE);

      ret = -1;
      info = &per_cpu(cpuinfo,cpuid);

      timeout = WAITTIME;

      while (timeout--)
      {

	      if(info->stat == CPU_RUNNING)
	      	{

	      	       ret = 0;
		  	break;
	      	}

		udelay(1);

     }


     if(ret == -1)
	  printk(CPUPM_DEBUG "\033[31 cpupm cpu_up cpu%d failed \033[m\n",cpuid);
	  #if CPUPM_MSG
	  else
	  	 printk(CPUPM_DEBUG "\033[31 cpupm cpu up cpu%d successfully \033[m\n",cpuid);
	 #endif

end:
     return ret;
}


#else

s32  _cpupm_cpu_up(s32 cpu)
{
     s32 ret=0;
     s32 timeout;

     struct cpu_info *info;

      cpu_up(cpu);

      printk(KERN_INFO "+++cpupm_cpu_up\n");

      //if(!ret)
 	//goto end;

      ret = -1;
      info = &per_cpu(cpuinfo,cpu);

      timeout = WAITTIME;

      while (timeout--)
      {


	      if(info->stat == CPU_RUNNING)
	      	{
	      	       //printk("get it-----------\n");
	      	       ret = 0;
		  	break;
	      	}
		else
		{
		   // printk("try again+++\n");
		}

		mdelay(1);

     }

	  if(ret == -1)
		 printk(CPUPM_DEBUG "cpupm cpu_up cpu%d failed \n",cpu);
	  #if CPUPM_MSG
	  else
	  	 printk(CPUPM_DEBUG "cpupm cpu up cpu%d successfully\n",cpu);
	 #endif

end:
	return ret;

}

#endif


int cpupm_cpu_up(int cpu_id)
{
      int ret;
      cpupm_hotplug_driver_lock();
      ret = _cpupm_cpu_up(cpu_id);
      cpupm_hotplug_driver_unlock();

      return ret;
}



#if 1

int _cpupm_cpu_down(int cpu_id)
{
       s32 ret = 0;
	s32 timeout;
	struct cpu_info *info;
       struct device *dev = get_cpu_device(cpu_id);
       struct cpu *cpu = container_of(dev, struct cpu, dev);
       int cpuid = cpu->dev.id;
       printk(KERN_INFO "\033[31mcpupm_cpu_down: cpuid:%d \033[m\n",cpuid);


	ret = cpu_down(cpuid);

	if (!ret)
	   kobject_uevent(&dev->kobj, KOBJ_OFFLINE);



	 info = &per_cpu(cpuinfo,cpuid);


	  timeout = WAITTIME;
	  ret = -1;

         while (timeout--)
	  {


	      if(info->stat == CPU_SLEEPING)
	      	{

	      	       ret = 0;
		  	break;
	      	}
		else
		{
		    //printk("try again+++\n");
		}

		 udelay(1);
	  }

        /*
	  if(ret == -1)
		 printk(CPUPM_DEBUG "\033[31 cpupm cpu_down cpu%d failed \033[m\n",cpu_id);
	  #if CPUPM_MSG
         else
		 printk(CPUPM_DEBUG "\033[31 cpupm cpu down cpu%d successfully \033[m\n",cpu_id);
	 #endif
        */

end:
	return ret;

}

#else
int _cpupm_cpu_down(s32 cpu)
{

        s32 ret=0;
        s32 timeout;
        struct cpu_info *info;
       char	str_governor[16];

	printk(KERN_INFO "+++cpupm_cpu_down start \n");

       #if defined(CONFIG_CPU_FREQ)

	//sysfs_modify_freq_policy_governor(cpu,performance_str);

       #endif

         cpu_down(cpu);



	// if(!ret)
	//  	goto end;

         ret = -1;
         info = &per_cpu(cpuinfo,cpu);

         timeout = WAITTIME;

         while (timeout--)
	  {


	      if(info->stat == CPU_SLEEPING)
	      	{
	      	       //printk("get it-----------\n");
	      	       ret = 0;
		  	break;
	      	}
		else
		{
		    //printk("try again+++\n");
		}

		 mdelay(1);
	  }

         if(ret == -1)
		 printk(CPUPM_DEBUG "cpupm cpu_down cpu%d failed\n",cpu);
	  #if CPUPM_MSG
         else
		 printk(CPUPM_DEBUG "cpupm cpu down cpu%d successfully\n",cpu);
	 #endif

	  is_power_on_again(cpu);

end:

      return ret;
}

#endif

int cpupm_cpu_down(int cpu_id)
{
    int ret;
    cpupm_hotplug_driver_lock();
    ret = _cpupm_cpu_down(cpu_id);
    cpupm_hotplug_driver_unlock();

    return ret;
}


void wakeup_all_cpus()
{
      struct cpu_info *info;
      int i;

      printk(KERN_INFO "+++wakeup_all_cpus+++\n");

      for_each_possible_cpu(i)
      {
            info = &per_cpu(cpuinfo,i);

	     if(info->stat == CPU_SLEEPING)
	     {
                    cpupm_cpu_up(i);
	     }
      }

      for_each_online_cpu(i)
	{
	    printk(KERN_INFO "wakeup_all_cpus:online cpu core%d\n",i);
	}

}

static ssize_t cpu_up_show(struct kobject *kobj, struct kobj_attribute *attr,
   char *buf)
{
     return sprintf(buf, "%d\n", cpu_up_threshold);
}


static ssize_t cpu_up_store(struct kobject *kobj, struct kobj_attribute *attr,
    const char *buf, size_t count)
{

     unsigned int tmp;

     sscanf(buf, "%du", &tmp);

     if(tmp > 100)
	 tmp = 100;
     else if(tmp < cpu_down_threshold)
	 tmp = cpu_down_threshold;

     cpu_up_threshold = tmp;

      return count;
}


static struct kobj_attribute cpu_up_attribute =
 __ATTR(cpu_up_threshold, 0644, cpu_up_show, cpu_up_store);



static ssize_t cpu_down_show(struct kobject *kobj, struct kobj_attribute *attr,
   char *buf)
{
     return sprintf(buf, "%d\n", cpu_down_threshold);
}


static ssize_t cpu_down_store(struct kobject *kobj, struct kobj_attribute *attr,
    const char *buf, size_t count)
{

       unsigned int tmp;
       sscanf(buf, "%du", &tmp);

       if(tmp > 100)
	  tmp = 100;
	else if(tmp > cpu_up_threshold)
		tmp = cpu_up_threshold;

       cpu_down_threshold = tmp;

      return count;
}


static struct kobj_attribute cpu_down_attribute =
 __ATTR(cpu_down_threshold, 0644, cpu_down_show, cpu_down_store);


static struct attribute *attrs[] = {
 &cpu_up_attribute.attr,
 &cpu_down_attribute.attr,
 NULL, /* need to NULL terminate the list of attributes */
};


static struct attribute_group attr_group = {
 .attrs = attrs,
};

static struct kobject *cpupm_kobj;



s32 init_cpupm_sysfs()
{

     s32 retval;

     cpupm_kobj = kobject_create_and_add("cpupm", kernel_kobj);

     if (!cpupm_kobj)
         return -ENOMEM;


     retval = sysfs_create_group(cpupm_kobj, &attr_group);

     if (retval)
        kobject_put(cpupm_kobj);

      return retval;
}



void show_cpu_info()
{
       static s32 showcount = 0;
	s32 avg_load ;
       s32 i;
	s32 count=0;
	s32 avr_online_cpu;

       if(showcount < (SHOWINFO_TIME / CPUPM_SAMPLE_TIME))
        {
            showcount++;
	     return;
      	 }
	 else
	 {
	    showcount = 0;
	 }

	 for_each_online_cpu(i)
	 {
	     count++;
	 }

	 cpu_history.count++;
	 cpu_history.total_online_cpunum += count;

       avg_load = get_average_cpu_load();
       printk(KERN_INFO "\ncurrent cpu loading:%d\n",avg_load);

	for_each_online_cpu(i)
	{
	    printk(KERN_INFO "online cpu core%d\n",i);
	}

       avr_online_cpu = cpu_history.total_online_cpunum * 100  / cpu_history.count;

	printk(KERN_INFO "average online cpu core:%d\n",avr_online_cpu);


}


void check_secenario_app()
{


	struct task_struct *tsk;
	struct cpu_info *info;
       s32 i;
	s32 cpus=0;
       s32 hit_cpu;
	s32 diff;
	s32 ret;

       printk(CPUPM_DEBUG "++++++++++++++++++++++++++++++++++\n");
	printk(CPUPM_DEBUG "prev occpy cpu = %d\n",occupy_nr_cpus);

       for_each_process (tsk)
       {

           for(i=0;i<NR_SECENARIO_APP;i++)
           {
                  if(strcmp(tsk->comm,scenario_app_array[i].app_name) == 0)
                  {
                            cpus += scenario_app_array[i].occupy_cpu;
				printk(CPUPM_DEBUG "find process:%s\n",scenario_app_array[i].app_name);
                  }
           }

       }

        diff = cpus - occupy_nr_cpus;

        occupy_nr_cpus = cpus;


       printk(CPUPM_DEBUG "current occpy cpu = %d\n",cpus);

        if(diff > 0)
        {
              for(i=0;i<diff;i++)
              {
                    hit_cpu = get_running_cpu();

		      if(hit_cpu == NOT_FOUND_CPU)
		      {
		          hit_cpu = get_coolest_cpu();

			   if(hit_cpu != NOT_FOUND_CPU)
			   {
			        ret = cpupm_cpu_up(hit_cpu);

				 if(ret != -1)
				 {

				    printk(CPUPM_DEBUG "check_secenario_app:wake up cpu%d\n",hit_cpu);
				     info = &per_cpu(cpuinfo,hit_cpu);
			            info->stat = CPU_OCCUPYING;
				 }

			   }
		      }
		      else
		      {
		             printk(CPUPM_DEBUG "check_secenario_app:set up cpu%d occupying\n",hit_cpu);
		             info = &per_cpu(cpuinfo,hit_cpu);
			      info->stat = CPU_OCCUPYING;
		      }



              }
        }
        else if(diff < 0)
       {

		for(i=diff;i<0;i++)
              {
                   hit_cpu = get_scenario_cpu();

		     if(hit_cpu != NOT_FOUND_CPU)
		     {
		           info = &per_cpu(cpuinfo,hit_cpu);
			    info->stat = CPU_RUNNING;
			    printk(CPUPM_DEBUG "check_secenario_app:set up cpu%d running\n",hit_cpu);
		     }
		     else
			  break;

              }

       }


}


s32 get_scenario_cpu()
{

      printk(CPUPM_DEBUG "+++get_scenario_cpu+++\n");
      s32 i;
      s32 cur_cpu=NOT_FOUND_CPU;
      struct cpu_info *info;


      for_each_possible_cpu(i)
      {
           info = &per_cpu(cpuinfo,i);

	    if(info->stat == CPU_OCCUPYING)
	    {
	        cur_cpu = i;
		 break;
	    }
      }

      return cur_cpu;

}


s32 get_running_cpu()
{
      s32 i;
      s32 cur_cpu=NOT_FOUND_CPU;
      struct cpu_info *info;

     for_each_online_cpu(i)
     {
          info = &per_cpu(cpuinfo,i);

	   if(info->stat == CPU_RUNNING)
	   {
	       cur_cpu = i;
		break;
	   }
     }

     return cur_cpu;

}


/*get the longest sleep time cpu*/
int get_coolest_cpu()
{

     s32 i;
     s32 cur_cpu=NOT_FOUND_CPU;
     struct cpu_info *info;
     u64 diff_time = 0UL;

     u64 cur_timestamp =  jiffies64_to_cputime64(get_jiffies_64());

    //printk("++++get_coolest_cpu start ++++\n");

     for_each_possible_cpu(i)
     {
            u64 tmp;
            info = &per_cpu(cpuinfo,i);

	     if(info->stat == CPU_SLEEPING)
	     {
                tmp = cur_timestamp - info->start_sleep_timestamp;


	         if(tmp > diff_time)
	         {
		      diff_time = tmp;
		      cur_cpu = i;
	         }
	     }

     }


     //printk("+++get_coolest_cpu: get cpu=%d\n",cur_cpu);

     return cur_cpu;

}


//get lightest loading cpu
s32 get_lightest_cpu(void)
{

     s32 i;
     s32 cpuload = 0xffff;
     s32 hit_cpu = -1;

     for_each_possible_cpu(i)
     {
          struct cpu_info *info = &per_cpu(cpuinfo,i);

	   if(info->stat == CPU_RUNNING &&  info->stat != CPU_OCCUPYING)
	   {

               if(info->loading < cpuload)
               {
                    cpuload = info->loading;
		      hit_cpu = i;
               }
	   }
     }

    /*we can not choose cpu0 , cpu0 is always running*/
     if(hit_cpu == 0)
     {
         return NOT_FOUND_CPU;
     }

     return hit_cpu;

}



static inline u64 cpupm_get_cpu_idle_time_jiffy(unsigned int cpu, u64 *wall)
{
	u64 idle_time;
	u64 cur_wall_time;
	u64 busy_time;

	cur_wall_time = jiffies64_to_cputime64(get_jiffies_64());

	busy_time = kcpustat_cpu(cpu).cpustat[CPUTIME_USER];
	busy_time += kcpustat_cpu(cpu).cpustat[CPUTIME_SYSTEM];
	busy_time += kcpustat_cpu(cpu).cpustat[CPUTIME_IRQ];
	busy_time += kcpustat_cpu(cpu).cpustat[CPUTIME_SOFTIRQ];
	busy_time += kcpustat_cpu(cpu).cpustat[CPUTIME_STEAL];
	busy_time += kcpustat_cpu(cpu).cpustat[CPUTIME_NICE];

	idle_time = cur_wall_time - busy_time;
	if (wall)
		*wall = cputime_to_usecs(cur_wall_time);

	return cputime_to_usecs(idle_time);
}

u64 cpupm_get_cpu_idle_time(unsigned int cpu, u64 *wall, int io_busy)
{
	u64 idle_time = get_cpu_idle_time_us(cpu, io_busy ? wall : NULL);

	if (idle_time == -1ULL)
		return cpupm_get_cpu_idle_time_jiffy(cpu, wall);
	else if (!io_busy)
		idle_time += get_cpu_iowait_time_us(cpu, wall);

	return idle_time;
}


#if defined(CONFIG_CPU_FREQ)


 //unit:us
static s64 get_sample_time()
{

      extern s64 cpu0_ktime_us;
      s64 sample_time;
      unsigned long mod;
      s64 cpufreq_us;

      //only get cpu0 cpufreq_ktime_us;
      struct cpufreq_policy *policy = cpufreq_cpu_get(0);
      cpufreq_us =  policy->cpufreq_ktime_us;
      ktime_t time_now = ktime_get();
      s64 time_now_us = ktime_to_us(time_now);


      s64 diff_us = time_now_us - cpufreq_us;

      if(diff_us < 0)
      {
           sample_time = cpupm_time_slice - diff_us;
      }
      else
      {


               mod = do_div(diff_us,cpupm_time_slice);
	        sample_time = cpupm_time_slice - mod;


		 if(sample_time < (90*1000))
		 {
                     sample_time += TRANSITION_LATENCY;
		 }

      	}


    //   printk(KERN_INFO "time now:%lld, dvfs time:%lld,diff ktime: %lld\n,",time_now_us,cpufreq_us,sample_time);

       return usecs_to_jiffies(sample_time);
}

static void calculate_cpuload()
{

      s32 i;

      for_each_online_cpu(i)
      {
             struct cpu_info *info = &per_cpu(cpuinfo,i);

	      struct cpufreq_policy *policy = cpufreq_cpu_get(i);

	       info->loading = policy->cpu_load;

		//printk(KERN_INFO "calculate_cpuload cpu%d load:%d\n",i,policy->cpu_load);

	      cpufreq_cpu_put(policy);

      }



}



#else


 static s64 get_sample_time()
 {
      return usecs_to_jiffies(cpupm_time_slice);
 }


static void calculate_cpuload()
{
       s32 i;
       s32 load;
      static s32 count=0;
      u64 zero_time = 0;
       u64 idle_time,wall_time;
	u64 cur_wall_time, cur_idle_time;
	int io_busy = 0;

	for_each_online_cpu(i)
       {

          struct cpu_info *info = &per_cpu(cpuinfo,i);
	   cur_idle_time = cpupm_get_cpu_idle_time(i,&cur_wall_time,io_busy);
	   wall_time = (unsigned int)(cur_wall_time - info->prev_wall_time);
	   // printk("wall_time=%llu\n",wall_time);
	   info->prev_wall_time = cur_wall_time;
	   idle_time = (unsigned int)(cur_idle_time - info->prev_idle_time);
	   info->prev_idle_time = cur_idle_time;
	   //  printk("idle_time=%u\n",idle_time);
	   if(idle_time != 0)
	   {
	         idle_time *= 100;
	         do_div(idle_time,wall_time);
		  load =  100  - idle_time;

	    }
	    else
	    {
	        load = 100;
	     }



	   info->loading = load;

	}
}



#endif




static s32 get_average_cpu_load()
{
          s32 i =0;
	   s32 load=0;
          s32 _count=0;

	   for_each_online_cpu(i) {
		struct cpu_info *info = &per_cpu(cpuinfo,i);

		load += info->loading;
              _count++;
   	}


	load /= _count;

	return load;
}



static void test_cpu_hotplug_func(struct work_struct *work)
{
          extern int stop_dvfs_flag ;
          static s32 i=1;
	   static int count = 0;
	   static flag = 0;

#if 1//elliott
	  printk("\033%s %d:[31+++test_cpu_hotplug_func++++ i:%d\033[m \n",__func__,__LINE__,i);
#else
	  printk(KERN_INFO "\033[31+++test_cpu_hotplug_func++++\033[m \n");
#endif
	   // while(1)
            //for(i=1;i<4;i++)
            {

		 cpupm_adjust_cpu_driver_lock();

              if(stop_adjust_cpu_flag)
              {
                  printk(KERN_INFO "ggg+++\n");
	           goto out;
              }


		cpupm_cpu_down(i);
		mdelay(100);
	       cpupm_cpu_up(i);

		if(i==3)
		  i=1;
		else
		  i++;
out:
	      cpupm_adjust_cpu_driver_unlock();

            }


        queue_delayed_work(cpuhotplug_wq, &cpuhotplug_work, get_sample_time());

}


static void adjust_cpu_load_func(struct work_struct *work)
{

      s32 cpu;
      s32 ret;
      struct cpu_info *info ;
      s32 avg_load ;


#if 0
     check_secenario_app();
#endif


     if(get_boost_status() == BOOST_MODE)
	 	goto end;

   // printk("-----------------------------------------------------------\n");


      cpupm_adjust_cpu_driver_lock();


      if(stop_adjust_cpu_flag)
      {
           printk(KERN_INFO "\033[31 +++ adjust_cpu_load_func stop_adjust_cpu_flag+++ \033[m \n");
	    goto out;
      }

      #if defined(CONFIG_CPU_FREQ)
      if(check_adjust_high_temperature() == HIGH_TEMPERATURE_MODE)
      {
         // printk(KERN_INFO "warning:in high temperature mode+++\n");
          goto out;
      }
      #endif

      goto out;


//------------------------------------------------------------------

      calculate_cpuload();

      avg_load = get_average_cpu_load();

      if(avg_load >  cpu_up_threshold)
      {

	     //wake up cpu

	     if(prev_state != CPU_STATE_BUFFER)
	     {
                  cpu = get_coolest_cpu();

	           if(cpu != NOT_FOUND_CPU)
	           {

                       ret = cpupm_cpu_up(cpu);

                       if(!ret)
                       {
                           #if CPUPM_MSG
		              printk(CPUPM_DEBUG "power on cpu%u successfully, cpu load:%d+++\n",cpu,avg_load);
                           #endif
		
                       }
		         else
		         {
		              printk(CPUPM_DEBUG "power on cpu%u failed+++\n",cpu);
		         }
	           }

	           prev_state = CPU_STATE_UP;

	     }
	     else
	     {
	         
	         #if CPUPM_MSG
	        // printk(CPUPM_DEBUG "cpu up : do not do any change\n");
		  #endif
	     }

			
      }
      else if(avg_load <= cpu_up_threshold && avg_load >= cpu_down_threshold)
      {
           //do nothing

          prev_state = CPU_STATE_BUFFER;
      }
      else if(avg_load < cpu_down_threshold)
      {

	  
          //power off cpu

	   if(prev_state != CPU_STATE_BUFFER)
	   {
        
                cpu = get_lightest_cpu();

	         if(cpu != NOT_FOUND_CPU)
	         {

 		 
                  ret = cpupm_cpu_down(cpu);

		    if(!ret)
		    {	
		         #if CPUPM_MSG
	                printk(CPUPM_DEBUG "++++adjust_cpu_load_func :::power off cpu%u successfully, cpuload:%d\n",cpu,avg_load);
                       #endif
		    }
		    else
		    {
		         printk(CPUPM_DEBUG "++++adjust_cpu_load_func :::power off cpu%u failed+++\n",cpu);
		    }
	         }
	  }
	  else
	  {
	         #if CPUPM_MSG
	         //printk(CPUPM_DEBUG "cpu down : do not do any change\n");
		  #endif
	  }

       
         prev_state = CPU_STATE_DOWN;
      }


      // printk("-----------------------------------------------------------\n"); 
out:
       cpupm_adjust_cpu_driver_unlock();
	  
      // show_cpu_info();
       //get_sample_time();

end:

	queue_delayed_work(cpuload_wq, &cpuload_work, get_sample_time());
       
}


#if defined(CONFIG_CPU_FREQ)

static int cpupm_cpufreq_transition(struct notifier_block *nb,
	unsigned long state, void *data)
{



      switch (state) {


	  case CPUFREQ_START:
	            
	          
		break;

      	}

      return 0;
}

#if defined(CONFIG_CPU_FREQ)

static struct notifier_block cpupm_cpufreq_nb = {
	.notifier_call = cpupm_cpufreq_transition,
};

#endif



#endif




static s32 __cpuinit cpupm_cpu_notify(struct notifier_block *self,
					   unsigned long action, void *hcpu)
{


    //   printk("+++cpupm_cpu_notify++++\n");
	s32 hotcpu = (unsigned long)hcpu;
	struct cpu_info *info = &per_cpu(cpuinfo,hotcpu);

	
	switch (action & 0xf) {


	  case CPU_ONLINE:
	             // #if CPUPM_MSG
	              printk(KERN_INFO "%s %d:+++cpupm_cpu_notify cpu%d online\n",__func__,__LINE__,hotcpu);
			//#endif
	              info->stat = CPU_RUNNING;
	              info->start_sleep_timestamp = 0;

	      		  
		break;
	  case CPU_DEAD:

	            // #if CPUPM_MSG
	             printk(KERN_INFO "%s %d:+++cpupm_cpu_notify cpu%d offline\n",__func__,__LINE__,hotcpu);
		      //#endif
		      info->stat = CPU_SLEEPING;
	             info->start_sleep_timestamp = jiffies64_to_cputime64(get_jiffies_64());
	            
		break;
	}

       

	return NOTIFY_OK;
}


static struct notifier_block cpupm_cpu_nb  = {
	.notifier_call = cpupm_cpu_notify,
};


static int cpupm_pm_notifier(struct notifier_block *notifier,
				       unsigned long val, void *v)
{ 

     printk(KERN_INFO "\033[31 ++++cpupm_pm_notifier++++\033[m\n");

      switch (val) {
	  case PM_POST_SUSPEND:
	  case PM_POST_RESTORE:
	  	cpupm_adjust_cpu_driver_lock();
		wakeup_all_cpus();
		stop_adjust_cpu_flag = false;
		cpupm_adjust_cpu_driver_unlock();
		break;
	   case PM_SUSPEND_PREPARE:
	   	cpupm_adjust_cpu_driver_lock();
		stop_adjust_cpu_flag = true;
		cpupm_adjust_cpu_driver_unlock();
		break;
	    default:
		return NOTIFY_DONE;
	}
	return NOTIFY_OK;

}


static struct notifier_block cpupm_pm_nb = {
	.notifier_call = cpupm_pm_notifier,
};


static s32 __init cpupm_init(void)
{  
 
    s32 i;

    cpu_down_threshold = DOWN;
    cpu_up_threshold = UP; 

    prev_state = CPU_STATE_UNKNOWN;



   init_cpupm_time_slice();
   
   struct cpu_info *info ;

   for_each_possible_cpu(i)
   {
       info = &per_cpu(cpuinfo,i);

	 if (cpu_online(i))
	 	info->stat = CPU_RUNNING;
	 else
	 	info->stat = CPU_DEAD;

      
   }


   occupy_nr_cpus = 0;

   delay = msecs_to_jiffies(35*1000*1);


#if 0

  cpuhotplug_wq = alloc_workqueue("cpuhotplug-balanced",WQ_UNBOUND | WQ_FREEZABLE, 1);
	if (!cpuhotplug_wq)
		return -ENOMEM;   

  INIT_DELAYED_WORK(&cpuhotplug_work, test_cpu_hotplug_func); 

   queue_delayed_work(cpuhotplug_wq, &cpuhotplug_work, delay);

#else

  cpuload_wq = alloc_workqueue("cpu-balanced",WQ_UNBOUND | WQ_FREEZABLE, 1);
	if (!cpuload_wq)
		return -ENOMEM; 

  INIT_DELAYED_WORK(&cpuload_work, adjust_cpu_load_func); 

 
  queue_delayed_work(cpuload_wq, &cpuload_work, delay);
 
#endif 


   #if defined(CONFIG_CPU_FREQ)
   cpufreq_register_notifier(&cpupm_cpufreq_nb,CPUFREQ_TRANSITION_NOTIFIER);
   #endif
  
   register_cpu_notifier(&cpupm_cpu_nb); 

   register_pm_notifier(&cpupm_pm_nb);

   init_cpupm_sysfs();
   
   return 0;
}


static void  cpupm_exit(void)
{
       cancel_delayed_work_sync(&cpuload_work);
	destroy_workqueue(cpuload_wq);
	kobject_put(cpupm_kobj);
	unregister_cpu_notifier(&cpupm_cpu_nb); 
}


static void mstar_cpupm_exit(void) 
{
    cpupm_exit();
}

static int mstar_cpupm_suspend(struct platform_device *dev, pm_message_t state)
{

   printk(KERN_INFO "+++mstar_cpupm_suspend start+++\n");

  
    return 0;
}

static int mstar_cpupm_resume(struct platform_device *dev)
{

   printk(KERN_INFO "mstar_cpupm_resume start+++\n");


   return 0;
   
}


static int mstar_cpupm_probe(struct platform_device *pdev)
{
    cpupm_init();
    return 0;
}

static int mstar_cpupm_remove(struct platform_device *pdev)
{
     cpupm_exit();
     return 0;
}


static struct platform_driver mstar_cpupm_driver = {
    .probe      = mstar_cpupm_probe,
    .remove     = mstar_cpupm_remove,
    .suspend    = mstar_cpupm_suspend,
    .resume     = mstar_cpupm_resume,

    .driver = {
        .name   = "Mstar-cpupm",
        .owner  = THIS_MODULE,
    }
};


static int __init mstar_cpupm_init_module(void) {
    int retval=0;
    retval = platform_driver_register(&mstar_cpupm_driver);
    return retval;
}

static void __exit mstar_cpupm_exit_module(void) {
    platform_driver_unregister(&mstar_cpupm_driver);
}

late_initcall(cpupm_init);
module_exit(mstar_cpupm_exit_module);


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Mstar cpu pm!!");
MODULE_AUTHOR("MSTAR");


