/*
 *  linux/arch/arm/mach-vexpress/platsmp.c
 *
 *  Copyright (C) 2002 ARM Ltd.
 *  All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/smp.h>
#include <linux/io.h>

#include <asm/unified.h>
#include <asm/smp_scu.h>
#include <linux/irqchip/arm-gic.h>
#include <asm/cacheflush.h>
#include <asm/smp_plat.h>
#include <chip_setup.h>
#include <mach/platform.h>
#include <asm/psci.h>

extern ptrdiff_t   mstar_pm_base;

#define PMU_ON_TRUE 0x00
#define PMU_PWRON_RST 0x0D

#define PMU_POWER_ADDR_REGL            (unsigned long)(mstar_pm_base+(0x1213 << 9))

#define CORE_PWR_OFF_EN  (0x00 << 2)
#define CORE_PWR_ONOFF_IN (0x08 << 2)
#define CORE_PWR_CTRL_ST (0x01 << 2)
#define CORE_PWR_REG7 (0x07 << 2)

static DECLARE_COMPLETION(cpu_killed);

extern void vexpress_secondary_startup(void);
extern void __cpuinit platform_secondary_init(unsigned int cpu);
extern int __cpuinit platform_boot_secondary(unsigned int cpu, struct task_struct *idle);
void release_cpukill_completion();
#ifdef CONFIG_MP_TRUSTZONE_PATCH
extern uint32_t isPSCI;
#endif
/*
 * Initialise the CPU possible map early - this describes the CPUs
 * which may be present or become present in the system.
 */
extern void gic_raise_softirq(const struct cpumask *mask, unsigned int irq);
void __init platform_smp_init_cpus(void)
{
	unsigned int i, ncores;
	ncores = scu_get_core_count((void __iomem *)(PERI_ADDRESS(chip_MPCORE_SCU)));

	/* sanity check */
	if (ncores == 0) {
		printk(KERN_ERR
		       "vexpress: strange CM count of 0? Default to 1\n");

		ncores = 1;
	}

	if (ncores > NR_CPUS) {
		printk(KERN_WARNING
		       "vexpress: no. of cores (%d) greater than configured "
		       "maximum of %d - clipping\n",
		       ncores, NR_CPUS);
		ncores = NR_CPUS;
	}

	for (i = 0; i < ncores; i++)
		set_cpu_possible(i, true);

	set_smp_cross_call(gic_raise_softirq);
}

void clear_magic()
{
    writel(0x0, SECOND_MAGIC_NUMBER_ADRESS);
    writel(0x0, SECOND_START_ADDR);
    writel(0x0, SECOND_START_ADDR + 4);
}

void __init platform_smp_prepare_cpus(unsigned int max_cpus)
{
	int i;

	/*
	 * Initialise the present map, which describes the set of CPUs
	 * actually populated at the present time.
	 */
	for (i = 0; i < max_cpus; i++)
		set_cpu_present(i, true);

	scu_enable((void __iomem *)(PERI_ADDRESS(chip_MPCORE_SCU))); // SCU PA = 0x16004000

	/*
	 * Write the address of secondary startup into the
	 * system-wide flags register. The boot monitor waits
	 * until it receives a soft interrupt, and then the
	 * secondary CPU branches to this address.
	 */

    //printk("_Secondary_startup physical address = 0x%08x\n",BSYM(virt_to_phys(vexpress_secondary_startup)));
	//writew((BSYM(virt_to_phys(vexpress_secondary_startup))>>16), SECOND_START_ADDR_HI);
	//writew(BSYM(virt_to_phys(vexpress_secondary_startup)), SECOND_START_ADDR_LO);
    writel(0xbabe, SECOND_MAGIC_NUMBER_ADRESS);
    writel(virt_to_phys(vexpress_secondary_startup), SECOND_START_ADDR);
	//writel(BSYM(virt_to_phys(vexpress_secondary_startup)) & 0xFFFF, SECOND_START_ADDR);
	//writel(BSYM(virt_to_phys(vexpress_secondary_startup)) >> 16, SECOND_START_ADDR + 4);
	__cpuc_flush_kern_all();

#ifdef CONFIG_MP_TRUSTZONE_PATCH
	writel_relaxed((__pa(vexpress_secondary_startup)), (void*)PAGE_OFFSET + 0x1004); //entry point put in 0x20201004
	__cpuc_flush_kern_all();
	writel_relaxed(0xbabe, (void*)PAGE_OFFSET + 0x1000); //magic put in 0x20201000
	__cpuc_flush_kern_all();

    writel(virt_to_phys(vexpress_secondary_startup) & 0xFFFF, (0xFD000000 + (0x100510 << 1)));
    writel(virt_to_phys(vexpress_secondary_startup) >> 16, (0xFD000000 + (0x100512 << 1)));
    writel(0xbabe, (0xFD000000 + (0x100514 << 1)));
    __cpuc_flush_kern_all();
    sev();
#endif
}
#if defined(CONFIG_MP_MSTAR_STR_BASE)
int platform_smp_boot_secondary_init(unsigned int cpu)
{
    scu_enable((void __iomem *)(PERI_ADDRESS(chip_MPCORE_SCU))); // SCU PA = 0x16004000
    writel(0xbabe, SECOND_MAGIC_NUMBER_ADRESS);
	writel((virt_to_phys(vexpress_secondary_startup)), SECOND_START_ADDR);
	__cpuc_flush_kern_all();

#ifdef CONFIG_MP_TRUSTZONE_PATCH
	writel_relaxed((__pa(vexpress_secondary_startup)), (void*)PAGE_OFFSET + 0x1004); //entry point put in 0x20201004
	__cpuc_flush_kern_all();
	writel_relaxed(0xbabe, (void*)PAGE_OFFSET + 0x1000); //magic put in 0x20201000
	__cpuc_flush_kern_all();

    writel(virt_to_phys(vexpress_secondary_startup) & 0xFFFF, (0xFD000000 + (0x100510 << 1)));
    writel(virt_to_phys(vexpress_secondary_startup) >> 16, (0xFD000000 + (0x100512 << 1)));
    writel(0xbabe, (0xFD000000 + (0x100514 << 1)));
    __cpuc_flush_kern_all();
    sev();
#endif
    return 0;
}
int platform_smp_boot_secondary_reset(unsigned int cpu)
{
    writel(0, SECOND_MAGIC_NUMBER_ADRESS);
	writel(0, SECOND_START_ADDR);
	__cpuc_flush_kern_all();
    return 0;
}
#endif


/*
 * Write pen_release in a way that is guaranteed to be visible to all
 * observers, irrespective of whether they're taking part in coherency
 * or not.  This is necessary for the hotplug code to work reliably.
 */
static void __cpuinit write_pen_release(int val)
{
	pen_release = val;
	smp_wmb();
#if 1
	Chip_Flush_Cache_Range((unsigned long)&pen_release, sizeof(pen_release));
#else
	__cpuc_flush_dcache_area((void *)&pen_release, sizeof(pen_release));
	outer_clean_range(__pa(&pen_release), __pa(&pen_release + 1));
#endif
}

static DEFINE_SPINLOCK(boot_lock);

/*This function is registered to the function with same name which is in "plat-mstar/platsmp.c"
  It's because the content of this function is not related to chip hardware setting*/
#if 0
int __cpuinit platform_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	unsigned long timeout;

	/*
	 * Set synchronisation state between this boot processor
	 * and the secondary one
	 */
	spin_lock(&boot_lock);

	/*
	 * This is really belt and braces; we hold unintended secondary
	 * CPUs in the holding pen until we're ready for them.  However,
	 * since we haven't sent them a soft interrupt, they shouldn't
	 * be there.
	 */
	write_pen_release(cpu_logical_map(cpu));

	#if 0  //debug
	unsigned int *ptr=&pen_release;
	//printk("pen_release = 0x%08x, addr= 0x%08x, pen_release ptr = 0x%08x\n ",pen_release,&pen_release,*ptr);
	#endif

	/*
	 * Send the secondary CPU a soft interrupt, thereby causing
	 * the boot monitor to read the system wide flags register,
	 * and branch to the address found there.
	 */
	gic_raise_softirq(cpumask_of(cpu), 0);

	timeout = jiffies + (1 * HZ);
	while (time_before(jiffies, timeout)) {
		smp_rmb();
		if (pen_release == -1)
			break;

		udelay(10);
	}

	/*
	 * now the secondary core is starting up let it run its
	 * calibrations, then wait for it to finish
	 */
	spin_unlock(&boot_lock);

	return pen_release != -1 ? -ENOSYS : 0;
}
#endif

void mstar_set_power(unsigned int cpu, bool state)
{

        unsigned short val;

        if(state)
        {

               (*(volatile unsigned short*)(PMU_POWER_ADDR_REGL+CORE_PWR_OFF_EN+0x40*cpu)) =0x97;

        }
        else
        {

                (*(volatile unsigned short*)(PMU_POWER_ADDR_REGL+CORE_PWR_OFF_EN+0x40*cpu)) =0x36;
        }

}

bool mstar_get_power(unsigned int cpu)
{

    unsigned short val;

    val = (*(volatile unsigned short*)(PMU_POWER_ADDR_REGL+CORE_PWR_OFF_EN+0x40*cpu));
    if(val == 0x97)
    {
       return 1;
    }
    else
    {
        return 0;
    }

}

unsigned short get_acpu_power_stat(unsigned int cpu)
{
     unsigned short val = 0;

     val =  (*(volatile unsigned short*)(PMU_POWER_ADDR_REGL+CORE_PWR_CTRL_ST+0x40*cpu));
     val &= 0xf;

     return val;

}

void mstar_disable_irq(int cpu)
{

          //printk(KERN_INFO "\033[m disable cpu%d irq \033[m\n",cpu);
          unsigned short val;
          val =  (*(volatile unsigned short*)(PMU_POWER_ADDR_REGL+CORE_PWR_REG7+0x40*cpu));
          val  |= (1<<14);
          val |= (1<<15);
          val  &= ~(1 << 11);
          val |= (1<<10);

          (*(volatile unsigned short*)(PMU_POWER_ADDR_REGL+CORE_PWR_REG7+0x40*cpu)) =val;

}


void mstar_set_secondary_cpu_kernel_entry()
{

       writel(0xbabe, SECOND_MAGIC_NUMBER_ADRESS);
       writel((virt_to_phys(vexpress_secondary_startup)), SECOND_START_ADDR);
    (*(volatile unsigned short*)((unsigned long)(mstar_pm_base+(0x1005 << 9)+(0x8<<2)))) = (virt_to_phys(vexpress_secondary_startup) & 0xFFFF);
    (*(volatile unsigned short*)((unsigned long)(mstar_pm_base+(0x1005 << 9)+(0x9<<2)))) = (virt_to_phys(vexpress_secondary_startup) >> 16);
    (*(volatile unsigned short*)((unsigned long)(mstar_pm_base+(0x1005 << 9)+(0xa<<2)))) =0xbabe;
       __cpuc_flush_kern_all();

}

#ifdef CONFIG_HOTPLUG_CPU
extern int platform_mstar_cpu_kill(unsigned int cpu);
extern void platform_mstar_cpu_die(unsigned int cpu);
extern int platform_mstar_cpu_disable(unsigned int cpu);
extern void platform_mstar_cpu_done_kill(unsigned int cpu);
#endif

extern unsigned int MdrvSecurePwMng_CoreOff(unsigned int u32CoreNum);
void gic_dist_subset_save();
void mstar_cpu_die(unsigned int cpu)
{
#ifdef CONFIG_MSTAR_CPU_HOTPLUG
	unsigned int val = 0;
#ifdef CONFIG_MP_DVFS_CPUHOTPLUG
	printk(KERN_DEBUG "+++mstar_cpu_die cpu%d+++\n",cpu);
#else
	printk(KERN_INFO "+++mstar_cpu_die cpu%d+++\n",cpu);
#endif

       if (unlikely(cpu != smp_processor_id()))
        {
               pr_crit("%s: running on %u, should be %u\n",
                   __func__, smp_processor_id(), cpu);
               BUG();
        }

         mstar_disable_irq(cpu);

        val =  (*(volatile unsigned short*)(PMU_POWER_ADDR_REGL+CORE_PWR_CTRL_ST+0x40*cpu));

        val  |= (1 << 7); //clear irq
        val |= (1<<15);//clear sleep

        (*(volatile unsigned short*)(PMU_POWER_ADDR_REGL+CORE_PWR_CTRL_ST+0x40*cpu)) = val;

        val =  (*(volatile unsigned short*)(PMU_POWER_ADDR_REGL+CORE_PWR_CTRL_ST+0x40*cpu));



        //-----------------------------------------------------
	 val =  (*(volatile unsigned short*)(PMU_POWER_ADDR_REGL+CORE_PWR_ONOFF_IN+0x40*cpu));
	 val  &= ~(1 << 4);
	 val &= ~(1<<6);
	 (*(volatile unsigned short*)(PMU_POWER_ADDR_REGL+CORE_PWR_ONOFF_IN+0x40*cpu)) =val;

       //--------------------------------------------------------------------------------------------

    gic_dist_subset_save();
    complete(&cpu_killed);

          /*config core PMU sleep register*/
	   mstar_set_power(cpu,false);

          flush_cache_all();

	    isb();
	    dsb();

         asm volatile(  "mrc p15, 0, r0, c1, c0, 0\n\t"
				 "bic r0, r0, #(1 << 2)\n\t"  // clear SCTLR.C
				  "mcr p15, 0, r0, c1, c0, 0\n\t"
				  "isb\n\t");

         flush_cache_all();
	 isb();
	 dsb();

#ifdef CONFIG_MP_TRUSTZONE_PATCH
	if(isPSCI == PSCI_RET_SUCCESS)
	{
	asm volatile(
		"isb\n\t"
		"dsb sy\n\t"
		:::"memory");
	cpu_psci_die(cpu);
	}else
#endif
	{
#ifdef CONFIG_MP_TRUSTZONE_PATCH
	MdrvSecurePwMng_CoreOff(cpu);
#endif
        asm volatile(
                             "mrrc p15, 1,r0, r1, c15\n\t"
                             "bic r0, r0, #(1 << 6)\n\t"
                             "mcrr p15, 1, r0, r1, c15\n\t"
                             "isb\n\t"
                             "dsb sy\n\t"
                              "wfi\n\t"
				    :::"memory");
	}
#else//CONFIG_MSTAR_CPU_HOTPLUG
#ifdef CONFIG_MP_TRUSTZONE_PATCH
	if(isPSCI == PSCI_RET_SUCCESS)
	{
		platform_mstar_cpu_done_kill(cpu);
		cpu_psci_die(cpu);
	}
	else
#endif
	{
#ifdef CONFIG_MP_TRUSTZONE_PATCH
		MdrvSecurePwMng_CoreOff(cpu);
#endif
		platform_mstar_cpu_die(cpu);
	}
#endif
}

extern unsigned int MdrvSecurePwMng_CoreOn(unsigned int u32CoreNum, unsigned long u64WakeupAddress);
static DEFINE_SPINLOCK(hotplug_lock);
 int mstar_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
        int ret = 0;
#ifdef CONFIG_MSTAR_CPU_HOTPLUG
	ptrdiff_t secondary_lo_addr_reg;
	ptrdiff_t secondary_hi_addr_reg;
	ptrdiff_t secondary_magic_reg;
	void *pWakeup = 0;
	unsigned long timeout;

	unsigned short val;


    spin_lock(&hotplug_lock);
#ifdef CONFIG_MP_DVFS_CPUHOTPLUG
	printk(KERN_DEBUG "+++mstar_boot_secondary current processor:%d++++\n",cpu);
#else
	printk("+++mstar_boot_secondary current processor:%d++++\n",cpu);
#endif

    if( (get_acpu_power_stat(cpu) == PMU_ON_TRUE) && (mstar_get_power(cpu) == 1))//AC
	{
		printk(KERN_INFO "++++warning:cpu%u has already power on\n",cpu);
		goto end;
	}
    else if( (get_acpu_power_stat(cpu) == PMU_ON_TRUE) && (mstar_get_power(cpu) == 0)) //cpu offline not ready
    {
	    timeout = 100000;
	    while(get_acpu_power_stat(cpu)  == PMU_ON_TRUE)
	    {
	        if(timeout == 0)
	        {
	            printk(KERN_INFO " boot cpu%d failed+++++\n",cpu);
	            spin_unlock(&hotplug_lock);
	            return -ETIMEDOUT;
	        }

	        udelay(1);
	        --timeout;
	    }
    }
	release_cpukill_completion();

	mstar_set_secondary_cpu_kernel_entry();

	//power on core_x
	mstar_set_power(cpu,true);
	timeout = 100000;

	while(get_acpu_power_stat(cpu)  != PMU_ON_TRUE)
	{
		if(timeout == 0)
		{
			printk(KERN_INFO " boot cpu%d failed+++++\n",cpu);
            spin_unlock(&hotplug_lock);
			return -ETIMEDOUT;
		}

		udelay(1);
		--timeout;
	}
#ifdef CONFIG_MP_TRUSTZONE_PATCH
	if(isPSCI == PSCI_RET_SUCCESS)
		ret = cpu_psci_boot_secondary(cpu,idle);
	else
#endif
	{
#ifdef CONFIG_MP_TRUSTZONE_PATCH
		MdrvSecurePwMng_CoreOn(cpu, 0x0);
#endif
		ret = platform_boot_secondary(cpu,idle);
		platform_smp_boot_secondary_reset(cpu);
	}
	spin_unlock(&hotplug_lock);
	return ret;

end:

#ifdef CONFIG_MP_TRUSTZONE_PATCH
	if(isPSCI == PSCI_RET_SUCCESS)
		ret = cpu_psci_boot_secondary(cpu,idle);
	else
#endif
	{
#ifdef CONFIG_MP_TRUSTZONE_PATCH
		MdrvSecurePwMng_CoreOn(cpu, 0x0);
#endif
		ret = platform_boot_secondary(cpu,idle);
	}
    spin_unlock(&hotplug_lock);
#else //CONFIG_MSTAR_CPU_HOTPLUG

#ifdef CONFIG_MP_TRUSTZONE_PATCH
	if(isPSCI == PSCI_RET_SUCCESS)
		ret = cpu_psci_boot_secondary(cpu,idle);
	else
#endif
	{
#ifdef CONFIG_MP_TRUSTZONE_PATCH
		MdrvSecurePwMng_CoreOn(cpu, 0x0);
#endif
	ret = platform_boot_secondary(cpu,idle);
	}
#endif

	return ret;
}

struct smp_operations __initdata chip_smp_ops = {
	.smp_init_cpus		= platform_smp_init_cpus,
	.smp_prepare_cpus	= platform_smp_prepare_cpus,
	.smp_secondary_init	= platform_secondary_init,
	.smp_boot_secondary	= mstar_boot_secondary,
#ifdef CONFIG_HOTPLUG_CPU
	.cpu_kill = platform_mstar_cpu_kill,
	.cpu_die = mstar_cpu_die,
	.cpu_disable = platform_mstar_cpu_disable,
#endif

};
