
#include <linux/kernel.h>

//#include <linux/platform_device.h>

//#include <linux/pm.h>
#include "mhal_cpupm.h"
#if defined(CONFIG_CPU_FREQ)
#include "mhal_dvfs.h"
#endif

#if defined(CONFIG_CPU_FREQ)
int check_adjust_high_temperature()
{

	static int ret = NORMAL_TEMPERATURE_MODE;


	struct cpu_info *info;

      unsigned temp = MHalDvfsQueryTemperature();
	if(temp > 135)
	{
	    ret = HIGH_TEMPERATURE_MODE;

	    info = get_cpu_info(1);

          if(info != NULL)
             if(info->stat == CPU_RUNNING)
             {
                cpupm_cpu_down(1);
             }

	    info = get_cpu_info(2);

            if(info != NULL)
               if(info->stat == CPU_RUNNING)
               {
                  cpupm_cpu_down(2);
               }

	}
	else if(temp <= 135 && temp >= 115)
	{
	    goto done;
	}
	else if(temp < 115)
	{
	    ret = NORMAL_TEMPERATURE_MODE;

	    info = get_cpu_info(1);

          if(info != NULL)
            if(info->stat == CPU_SLEEPING)
            {
                cpupm_cpu_up(1);
            }

	    info = get_cpu_info(2);

           if(info != NULL)
               if(info->stat == CPU_SLEEPING)
               {
                  cpupm_cpu_up(2);
               }


	}

done:

       return ret;
}

#else
int check_adjust_high_temperature()
{
    return NORMAL_TEMPERATURE_MODE;
}

#endif