/*
 * Spin Table SMP initialisation
 *
 * Copyright (C) 2013 ARM Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/init.h>
#include <linux/of.h>
#include <linux/smp.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <chip_setup.h>
#include <mach/io.h>
#include<linux/kernel.h>
#include <asm/mach/arch.h>
#include <asm/cacheflush.h>
#include <asm/tlbflush.h>
#include <linux/cpu.h>
#include <asm/smp_plat.h>
#include <mach/platform.h>
#include <mach/pm.h>

 //reg_core0_pwr_ctrl_st
#define PMU_ON_TRUE 0x00
#define PMU_PWRON_RST 0x0D


#define PMU_POWER_ADDR_REGL            (unsigned long)(mstar_pm_base+(0x1213 << 9))

#define CORE_PWR_OFF_EN  (0x00 << 2)
#define CORE_PWR_ONOFF_IN (0x08 << 2)
#define CORE_PWR_CTRL_ST (0x01 << 2)
#define CORE_PWR_REG7 (0x07 << 2)

static phys_addr_t cpu_release_addr[NR_CPUS];
extern ptrdiff_t   mstar_pm_base;



volatile int __cpuinitdata pen_release = -1;

static ptrdiff_t *mstar_acpu_base;
extern void secondary_startup(void);
extern void secondary_entry();
extern void secondary_holding_pen_test();
extern void secondary_holding_pen();
extern void secondary_entry_test();




extern void sleep_set_wakeup_addr_phy(unsigned long phy_addr, void *virt_addr);
extern phys_addr_t mstar_virt_to_phy(void* virtaddr);


//extern void  _flush_dcache_all(void);

phys_addr_t magic_number_address;


static int __init mstar_smp_spin_table_init_cpu(struct device_node *dn, int cpu)
{
	/*
	 * Determine the address from which the CPU is polling.
	 */
	if (of_property_read_u64(dn, "cpu-release-addr",
				 &cpu_release_addr[cpu])) {
		pr_err("CPU %d: missing or invalid cpu-release-addr property\n",
		       cpu);

		return -1;
	}

	return 0;
}

void clear_magic()
{
	ptrdiff_t secondary_lo_addr_reg;
	ptrdiff_t secondary_magic_reg;

	secondary_lo_addr_reg = mstar_pm_base + 0x200A20 ;  //riu 0x100510 <<1
	secondary_magic_reg = secondary_lo_addr_reg + 8;
	writel_relaxed(0x0000, (void*)secondary_magic_reg);
}

int mstar_smp_spin_table_prepare_cpu(int cpu)
{
        ptrdiff_t secondary_lo_addr_reg;
        ptrdiff_t secondary_hi_addr_reg;
        ptrdiff_t secondary_magic_reg;

	if (!cpu_release_addr[cpu])
		return -ENODEV;

        secondary_lo_addr_reg = mstar_pm_base + (cpu_release_addr[cpu] << 1) ;
        secondary_hi_addr_reg = secondary_lo_addr_reg + 4;
        secondary_magic_reg = secondary_lo_addr_reg + 8;

        writel_relaxed((__pa(secondary_holding_pen) & 0x000000000000ffff), (void*)secondary_lo_addr_reg);
        writel_relaxed((__pa(secondary_holding_pen) >> 16), (void*)secondary_hi_addr_reg);
        writel_relaxed(0xbabe, (void*)secondary_magic_reg);

        /*
	 * Send an event to wake up the secondary CPU.
	 */
	sev();

	return 0;
}

void set_secondary_magic_reg_val(u16 val)
{
	ptrdiff_t secondary_magic_reg;
	secondary_magic_reg =  PMU_POWER_ADDR_REGL + (0x01e<<2);
	writel_relaxed(0xbabe, (void*)secondary_magic_reg);
	udelay(1);
}

void mstar_set_power(unsigned int cpu, bool state)
{

	unsigned short val;
	if(state)
	{
	

		val = (*(volatile unsigned short*)(PMU_POWER_ADDR_REGL+CORE_PWR_OFF_EN+0x40*cpu)) ;

		
		val =   (*(volatile unsigned short*)(PMU_POWER_ADDR_REGL+(0x0e<<2)+0x40*cpu));
		
		val =  (*(volatile unsigned short*)(PMU_POWER_ADDR_REGL+CORE_PWR_CTRL_ST+0x40*cpu));
	
		(*(volatile unsigned short*)(PMU_POWER_ADDR_REGL+CORE_PWR_OFF_EN+0x40*cpu)) =0x97;
		
	}
    else
	{
		val  &= ~(1 << 4);
		val &= ~(1<<6);
		(*(volatile unsigned short*)(PMU_POWER_ADDR_REGL+0x40*cpu+CORE_PWR_ONOFF_IN)) = val;
		(*(volatile unsigned short*)(PMU_POWER_ADDR_REGL+CORE_PWR_OFF_EN+0x40*cpu)) =0x36;
	}
}


unsigned short get_acpu_power_stat(unsigned int cpu)
{
     unsigned short val = 0;

     bool ret = false;

     //val = readl(PMU_POWER_ADDR_REGL+(cpu*0x40+CORE_PWR_CTRL_ST));
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

	ptrdiff_t secondary_lo_addr_reg;
	ptrdiff_t secondary_hi_addr_reg;
	ptrdiff_t secondary_magic_reg;
	unsigned long long boot_addr;

	secondary_lo_addr_reg = PMU_POWER_ADDR_REGL + (0x0e<<2) ;
	secondary_hi_addr_reg =  PMU_POWER_ADDR_REGL + (0x0f<<2) ;
	secondary_magic_reg =  PMU_POWER_ADDR_REGL + (0x01e<<2);

	boot_addr = (__pa(secondary_holding_pen) & 0x000000000000ffff);
	//printk(KERN_INFO "before writing secondary_holding_pen lo val=0x%llx\n",boot_addr);
	boot_addr = (__pa(secondary_holding_pen) >> 16);
	//printk(KERN_INFO "before writing secondary_holding_pen high val=0x%llx\n",boot_addr);
	writel_relaxed((__pa(secondary_holding_pen) & 0x000000000000ffff), (void*)secondary_lo_addr_reg);
	writel_relaxed((__pa(secondary_holding_pen) >> 16), (void*)secondary_hi_addr_reg);
}

EXPORT_SYMBOL(mstar_set_secondary_cpu_kernel_entry);


void mstar_cpu_die(unsigned int cpu)
{

       //printk(KERN_INFO "+++mstar_cpu_die cpu%d+++\n",cpu);

	if (unlikely(cpu != smp_processor_id()))
	 {
		pr_crit("%s: running on %u, should be %u\n",
			__func__, smp_processor_id(), cpu);
		BUG();
	  }

         unsigned short val;
         mstar_disable_irq(cpu);
        //-----------------------------------------------------------------------------

        val =  (*(volatile unsigned short*)(PMU_POWER_ADDR_REGL+CORE_PWR_CTRL_ST+0x40*cpu));

	  //shihong modify start
	  //val  &= ~(1 << 7); //clear irq
	  //val &= ~(1<<15);//clear sleep
	   val  |= (1 << 7); //clear irq
	  val |= (1<<15);//clear sleep
	  //shihong modify end
        //val |= (1<<15);
	   (*(volatile unsigned short*)(PMU_POWER_ADDR_REGL+CORE_PWR_CTRL_ST+0x40*cpu)) = val;

	     val =  (*(volatile unsigned short*)(PMU_POWER_ADDR_REGL+CORE_PWR_CTRL_ST+0x40*cpu));

	 //  printk(KERN_INFO "CORE_PWR_CTRL_ST val=0x%x\n",val);

        //-----------------------------------------------------
	 val =  (*(volatile unsigned short*)(PMU_POWER_ADDR_REGL+CORE_PWR_ONOFF_IN+0x40*cpu));
	 val  &= ~(1 << 4);
	 val &= ~(1<<6);
	 (*(volatile unsigned short*)(PMU_POWER_ADDR_REGL+CORE_PWR_ONOFF_IN+0x40*cpu)) =val;

       //--------------------------------------------------------------------------------------------



        /*config core PMU sleep register*/
	 mstar_set_power(cpu,false);


      //printk(KERN_INFO "+++mstar_set_power ok++++\n");

       asm volatile(

                          "bl __flush_dcache_all\n\t"
                          "isb\n\t"
		            "mrs x0, sctlr_el1\n\t"
		            "bic x0, x0, #1 << 2\n\t"  // clear SCTLR.C
		            "msr sctlr_el1 ,x0\n\t"
		            "isb\n\t"
	                   "bl __flush_dcache_all\n\t"
	                   "isb\n\t"
	                   "dsb sy\n\t"
	                  "mrs x0, s3_1_c15_c2_1\n\t"
		           "bic x0, x0, #1 << 6\n\t"         /* Leave coherency, clear SMPEN */
		           "msr s3_1_c15_c2_1 ,x0\n\t"
		            "isb\n\t"
		            "dsb sy\n\t"
		            "wfi\n\t"
		            :::"memory" );
}

 int mstar_boot_secondary(unsigned int cpu, struct task_struct *idle)
{

	 ptrdiff_t secondary_lo_addr_reg;
        ptrdiff_t secondary_hi_addr_reg;
        ptrdiff_t secondary_magic_reg;
        void *pWakeup=0;
	 unsigned long timeout;

	 unsigned short val;
	 int ret = 0;

      printk("+++mstar_boot_secondary current processor:%d++++\n",cpu);
      if(get_acpu_power_stat(cpu) == PMU_ON_TRUE)
       {
           printk(KERN_INFO "++++warning:cpu%u has already power on\n",cpu);
           goto end;
       }

	if (!cpu_release_addr[cpu])
	{
		ret = -ENODEV;
		goto end;
	}

        //mstar_set_reset_vector();
        set_secondary_magic_reg_val(0x0000);

	  /*power on core_x*/
	  mstar_set_power(cpu,true);

	timeout = 100000;

	  while(get_acpu_power_stat(cpu)  != PMU_ON_TRUE)
         {
              if(timeout == 0)
              {
                  printk(KERN_INFO " boot cpu%d failed+++++\n",cpu);
		    return -ETIMEDOUT;
              }

             udelay(1);

	       --timeout;
       }

      mstar_set_secondary_cpu_kernel_entry();
       set_secondary_magic_reg_val(0xbabe);

        ret = 1;


end:


        return ret;
}


const struct smp_enable_ops mstar_smp_spin_table __initconst = {
	.name		= "mstar-spin-table",
	.init_cpu 	= mstar_smp_spin_table_init_cpu,
	.prepare_cpu	= mstar_smp_spin_table_prepare_cpu,
};

struct smp_operations mstar_smp_ops __initdata = {
	.smp_boot_secondary	= mstar_boot_secondary,
	.cpu_die		= mstar_cpu_die,
};

DT_MACHINE_START(MSTAR_DT, "curry")
	.smp = smp_ops(mstar_smp_ops),
MACHINE_END
