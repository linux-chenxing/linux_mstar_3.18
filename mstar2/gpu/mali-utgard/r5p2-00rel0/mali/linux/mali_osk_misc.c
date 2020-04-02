/*
 * Copyright (C) 2010-2015 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/**
 * @file mali_osk_misc.c
 * Implementation of the OS abstraction layer for the kernel device driver
 */
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <asm/cacheflush.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/module.h>
#include "mali_osk.h"

#if !defined(CONFIG_MALI_QUIET)
void _mali_osk_dbgmsg(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vprintk(fmt, args);
	va_end(args);
}
#endif /* !defined(CONFIG_MALI_QUIET) */

u32 _mali_osk_snprintf(char *buf, u32 size, const char *fmt, ...)
{
	int res;
	va_list args;
	va_start(args, fmt);

	res = vscnprintf(buf, (size_t)size, fmt, args);

	va_end(args);
	return res;
}

void _mali_osk_ctxprintf(_mali_osk_print_ctx *print_ctx, const char *fmt, ...)
{
	va_list args;
	char buf[512];

#ifdef MSTAR
	/* If buf or va_arg passed to _mali_osk_ctxprintf contains qualifier strings such as '%s',
	 * there are risks driver will fail inside seq_printf */
	int bytes;
	va_start(args, fmt);
	bytes = vscnprintf(buf, 512, fmt, args);
	seq_write(print_ctx, buf, bytes);
#else
	va_start(args, fmt);
	vscnprintf(buf, 512, fmt, args);
	seq_printf(print_ctx, buf);
#endif
	va_end(args);
}

void _mali_osk_abort(void)
{
	/* make a simple fault by dereferencing a NULL pointer */
	dump_stack();
	*(int *)0 = 0;
}

void _mali_osk_break(void)
{
	_mali_osk_abort();
}

u32 _mali_osk_get_pid(void)
{
	/* Thread group ID is the process ID on Linux */
	return (u32)current->tgid;
}

/* MSTAR patch begin */
#ifdef MSTAR
void _mali_osk_get_comm(char *comm)
{
	/* We expected that current has same life cycle with seesion, however on certain
	 * platform there are chances that memory space where
	 * current->comm is pointed to is freed at the time when session info is
	 * requestd. Therefore insted of return the pointer, we copy the value
	 * of comm, and place a '\0' at the end of the string. */
	get_task_comm(comm, current);
	comm[TASK_COMM_LEN-1] = '\0';
}
#else
char *_mali_osk_get_comm(void)
{
	return (char *)current->comm;
}
#endif
/* MSTAR patch end */

u32 _mali_osk_get_tid(void)
{
	/* pid is actually identifying the thread on Linux */
	u32 tid = current->pid;

	/* If the pid is 0 the core was idle.  Instead of returning 0 we return a special number
	 * identifying which core we are on. */
	if (0 == tid) {
		tid = -(1 + raw_smp_processor_id());
	}

	return tid;
}
