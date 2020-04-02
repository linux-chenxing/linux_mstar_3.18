#define MAX_CPU_FREQ 1452000
#define MIN_CPU_FREQ 948000
#define IRBOOST_CPU_FREQ 1452000

#define TRANSITION_LATENCY 1000000
// ( TRANSITION_LATENCY / 1000 ) * LATENCY_MULTIPLIER ==> DVFS on_demand sampling rate, > 100000

/* Minimum CLK support, each accross 20 percentage */
enum
{
    DC_ZERO, DC_20PT, DC_40PT, DC_60PT, DC_80PT, DC_MAX_NUM
};

/* Voltage for each cpu_load percentage */
#define V_ZERO MIN_CPU_FREQ
#define V_20PT 1008000
#define V_40PT 1056000
#define V_60PT 1104000
#define V_80PT MAX_CPU_FREQ

#define ON_DEMAND_TABLE_INTERVAL (100/DC_MAX_NUM)

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
void change_cpus_timer(char *caller, unsigned int target_freq);
#endif
