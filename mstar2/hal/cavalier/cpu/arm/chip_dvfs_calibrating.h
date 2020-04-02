#include <linux/cpufreq.h>


#if (defined CONFIG_MP_DVFS_FORCE_USE_ONE_FREQ) || (defined CONFIG_MP_DVFS_FORCE_PINGO_TEST)
#define MAX_CPU_FREQ 1100000
#else
#define MAX_CPU_FREQ MDrvDvfsQueryCpuClock(CONFIG_DVFS_MAX_CPU_CLOCK)
#endif
#define MIN_CPU_FREQ MDrvDvfsQueryCpuClock(CONFIG_DVFS_MIN_CPU_CLOCK)

#define IRBOOST_CPU_FREQ MDrvDvfsQueryCpuClock(CONFIG_DVFS_IR_BOOTS_CPU_CLOCK)

#if (defined CONFIG_MP_DVFS_FORCE_USE_ONE_FREQ) || (defined CONFIG_MP_DVFS_FORCE_PINGO_TEST)
#define TRANSITION_LATENCY 1000000
#else
#define TRANSITION_LATENCY 100000
#endif
// ( TRANSITION_LATENCY / 1000 ) * LATENCY_MULTIPLIER ==> DVFS on_demand sampling rate, > 100000

#ifdef CONFIG_MP_DVFS_CPUHOTPLUG
/* t-sensor: total_cpu_nr = 2; boost: total_cpu_nr = 4; on_demand: total_cpu_nr = x(2 ~ 4, by_loading); str: total_cpu_nr = 0(do not update) */
#define OVER_THERMAL_CPU_NR 2			// if over thermal, adjust cpu_nr to OVER_THERMAL_CPU_NR
#define BOOST_CPU_NR CONFIG_NR_CPUS		// if boost mode, adjust cpu_nr to BOOST_CPU_NR
#define STR_CPU_NR 0					// if str, we do not care about the online_cpu_nr, do noting

#define CPU_DVFS_HOTPLUG_LEVEL_MAX 12	// we have totally 12 levels
typedef struct
{
	unsigned int total_cpu_nr;
	unsigned int target_freq;

	unsigned int sample_rate_mult;

	unsigned int mstar_up_threshold;
	unsigned int mstar_adj_up_threshold;
} Cpu_Dvfs_Hotplug_Scenario;

typedef struct
{
	unsigned int supported_workload_lower_bound;
	unsigned int supported_workload_upper_bound;
} Cpu_Dvfs_Hotplug_Total_Workload_Region;
#endif

/* Minimum CLK support, each accross 20 percentage */
enum
{
    DC_ZERO, DC_20PT, DC_40PT, DC_60PT, DC_80PT, DC_MAX_NUM
};

/* Voltage for each cpu_load percentage */
#define V_ZERO MIN_CPU_FREQ
#define V_20PT 1200000
#define V_40PT 1300000
#define V_60PT 1400000
#define V_80PT MAX_CPU_FREQ

#define ON_DEMAND_TABLE_INTERVAL (100/DC_MAX_NUM)
#define DVFS_FREQ_LEVEL_MAX_INDEX 32

static struct cpufreq_frequency_table hal_freq_table[DVFS_FREQ_LEVEL_MAX_INDEX] = {
   {0, 216000},
   {1, 400000},
   {2, 600000},
   {3, 750000},
   {4, 800000},
   {5, 850000},
   {6, 900000},
   {7, 950000},
   {8, 1000000},
   {9, 1008000},
   {10, 1050000},
   {11, 1100000},
   {12, 1150000},
   {13, 1200000},
   {14, 1250000},
   {15, 1300000},
   {16, 1350000},
   {17, 1400000},
   {18, 1450000},
   {19, 1500000},
   {20, 1550000},
   {21, 1600000},
   {22, 1650000},
   {23, 1700000},
   {24, 1740000},
   {25, 1750000},
   {26, 1800000},
   {27, 1850000},
   {28, 1900000},
   {29, 1950000},
   {30, 2000000},
   {31, CPUFREQ_TABLE_END}
};




/* data that will copy to user_space when need change voltage */
typedef struct
{
    unsigned int to_userspace_cpufreq;
    unsigned int to_userspace_voltage;
    unsigned int to_userspace_voltage_type;
    unsigned int to_userspace_change_cnt;
} ON_Demand_To_Userspace;

/* data from user space when voltage is changed */
typedef struct
{
	unsigned int from_userspace_finished_change_cnt;
	int from_userspace_voltage_change_result;
} ON_Demand_From_Userspace;

#ifdef CONFIG_CPU_FREQ
void change_cpus_timer(char *caller, unsigned int target_freq, unsigned int cpu_id);
#endif
