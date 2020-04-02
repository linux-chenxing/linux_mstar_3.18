/*
 *  linux/arch/arm/arm-boards/plat-mstar/sched-clock.c
 *
 *  Copyright (C) 1999 - 2003 ARM Limited
 *  Copyright (C) 2000 Deep Blue Solutions Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <linux/io.h>
#include <linux/sched.h>
#include <linux/clocksource.h>

#include <asm/sched_clock.h>
#include <plat/sched_clock.h>
#include <mstar/mpatch_macro.h>
#include <asm/sched_clock.h>

extern struct clocksource clocksource_timer;

#if (MP_CA7_QUAD_CORE_PATCH == 1)


#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <asm/mach/time.h>
#include <mach/hardware.h>
#include <linux/clocksource.h>
#include <linux/clockchips.h>

#include <mach/io.h>
#include <mach/timex.h>
#include <asm/irq.h>
#include <linux/timer.h>
#include <plat/localtimer.h>
#include "chip_int.h"

cycle_t arch_counter_read(void)
{
	return arch_counter_get_cntpct();
}
#endif

//static DEFINE_CLOCK_DATA(cd);
static void __iomem *ctr;

//clocks_calc_mult_shift(m, s, 450MHz, NSEC_PER_SEC, 8)
#if 0
#define SC_MULT		2386092942u
#define SC_SHIFT	30
#else
u32 SC_MULT=0;
u32 SC_SHIFT=0;
#endif

#if (MP_PLATFORM_CPU_SETTING == 1)
#if defined CONFIG_MSTAR_CPU_calibrating
extern unsigned int current_frequency;
extern unsigned int register_frequency;
extern void clocks_calc_mult_shift(u32 *mult, u32 *shift, u32 from, u32 to, u32 maxsec);
extern void notrace mstar_update_sched_clock();
#endif //CONFIG_MSTAR_CPU_calibrating
#endif // MP_PLATFORM_CPU_SETTING
static u32 notrace mstar_sched_clock(void)
{
#if (MP_PLATFORM_CPU_SETTING == 1)
#if defined CONFIG_MSTAR_CPU_calibrating
	u32 shift,mult;
	if (register_frequency)
	{
	    clocks_calc_mult_shift(&mult, &shift,register_frequency/2 * 1000, NSEC_PER_SEC, 0);

	    SC_MULT = mult;
	    SC_SHIFT = shift;

		if(register_frequency != current_frequency )
			mstar_update_sched_clock();
    }	
#endif //CONFIG_MSTAR_CPU_calibrating
#endif // CONFIG_MP_PLATFORM_CPU_SETTING    
	u32 cyc;
#if defined(CONFIG_MP_CA7_QUAD_CORE_PATCH)
        cyc = arch_counter_read();
#else      
	if (ctr)	
		cyc = readl(ctr);	
	 else
		cyc = 0;
#endif
	return cyc;
}

void __init mstar_sched_clock_init(void __iomem *reg, unsigned long rate)
{
//printk("SC_MULT  = %u  ,SC_SHIFT  = %u \n",SC_MULT,SC_SHIFT);
	ctr = reg;
	setup_sched_clock(mstar_sched_clock, 32, rate);
}
