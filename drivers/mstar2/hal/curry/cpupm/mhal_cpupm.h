#ifndef _MHAL_CPUPM_H
#define _MHAL_CPUPM_H

#define HIGH_TEMPERATURE_MODE 3
#define NORMAL_TEMPERATURE_MODE 2

#define CPU_RUNNING   1
#define CPU_SLEEPING  2
#define CPU_OCCUPYING  3


struct cpu_info
{
    unsigned long long  prev_idle_time;
    unsigned long long prev_wall_time;
    unsigned loading;
    int stat;
    
    unsigned long long start_sleep_timestamp;

};

struct cpu_info* get_cpu_info(int cpu);

int check_adjust_high_temperature();


int cpupm_cpu_down(int cpu_id);
int cpupm_cpu_up(int cpu_id);


#endif


