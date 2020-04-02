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
#include <linux/io.h>
#include <chip_setup.h>
#include <asm/cacheflush.h>

static phys_addr_t cpu_release_addr[NR_CPUS];
extern ptrdiff_t mstar_pm_base;

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

const struct smp_enable_ops mstar_smp_spin_table __initconst = {
    .name       = "mstar-spin-table",
    .init_cpu   = mstar_smp_spin_table_init_cpu,
    .prepare_cpu    = mstar_smp_spin_table_prepare_cpu,
};
