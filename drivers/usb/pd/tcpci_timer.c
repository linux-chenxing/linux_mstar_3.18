/*
 * Copyright (C) 2016 Richtek Technology Corp.
 *
 * TCPC Interface for timer handler
 *
 * Author: TH <tsunghan_tsai@richtek.com>
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/atomic.h>
#include <linux/kthread.h>
#include <linux/hrtimer.h>
#include <linux/version.h>

#if 1 /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 9, 0)) */
#include <linux/sched/rt.h>
#endif /* #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 9, 0)) */

#include <linux/usb/tcpci.h>
#include <linux/usb/tcpci_timer.h>
#include <linux/usb/tcpci_typec.h>

#define RT_MASK64(i)	(((uint64_t)1) << i)

#define TIMEOUT_VAL(val)	(val * 1000)
#define TIMEOUT_RANGE(min, max)		((min * 4000 + max * 1000)/5)
#define TIMEOUT_VAL_US(val)	(val)

/* Debug message Macro */
#if TCPC_TIMER_DBG_EN
#define TCPC_TIMER_DBG(tcpc, id)				\
{								\
	RT_DBG_INFO("Trigger %s\n", tcpc_timer_name[id]);	\
}
#else
#define TCPC_TIMER_DBG(format, args...)
#endif /* TCPC_TIMER_DBG_EN */

#if TCPC_TIMER_INFO_EN
#define TCPC_TIMER_EN_DBG(tcpc, id)				\
{								\
	RT_DBG_INFO("Enable %s\n", tcpc_timer_name[id]);	\
}
#else
#define TCPC_TIMER_EN_DBG(format, args...)
#endif /* TCPC_TIMER_INFO_EN */

static inline uint64_t tcpc_get_timer_enable_mask(struct tcpc_device *tcpc)
{
	uint64_t data;
	unsigned long flags;

	down(&tcpc->timer_enable_mask_lock);
	raw_local_irq_save(flags);
	data = tcpc->timer_enable_mask;
	raw_local_irq_restore(flags);
	up(&tcpc->timer_enable_mask_lock);

	return data;
}

static inline void tcpc_reset_timer_enable_mask(struct tcpc_device *tcpc)
{
	unsigned long flags;

	down(&tcpc->timer_enable_mask_lock);
	raw_local_irq_save(flags);
	tcpc->timer_enable_mask = 0;
	raw_local_irq_restore(flags);
	up(&tcpc->timer_enable_mask_lock);
}

static inline void tcpc_clear_timer_enable_mask(
	struct tcpc_device *tcpc, int nr)
{
	unsigned long flags;

	down(&tcpc->timer_enable_mask_lock);
	raw_local_irq_save(flags);
	tcpc->timer_enable_mask &= ~RT_MASK64(nr);
	raw_local_irq_restore(flags);
	up(&tcpc->timer_enable_mask_lock);
}

static inline void tcpc_set_timer_enable_mask(
	struct tcpc_device *tcpc, int nr)
{
	unsigned long flags;

	down(&tcpc->timer_enable_mask_lock);
	raw_local_irq_save(flags);
	tcpc->timer_enable_mask |= RT_MASK64(nr);
	raw_local_irq_restore(flags);
	up(&tcpc->timer_enable_mask_lock);
}

static inline uint64_t tcpc_get_timer_tick(struct tcpc_device *tcpc)
{
	uint64_t data;
	unsigned long flags;

	spin_lock_irqsave(&tcpc->timer_tick_lock, flags);
	data = tcpc->timer_tick;
	spin_unlock_irqrestore(&tcpc->timer_tick_lock, flags);

	return data;
}

static inline void tcpc_clear_timer_tick(struct tcpc_device *tcpc, int nr)
{
	unsigned long flags;

	spin_lock_irqsave(&tcpc->timer_tick_lock, flags);
	tcpc->timer_tick &= ~RT_MASK64(nr);
	spin_unlock_irqrestore(&tcpc->timer_tick_lock, flags);
}

static inline void tcpc_set_timer_tick(struct tcpc_device *tcpc, int nr)
{
	unsigned long flags;

	spin_lock_irqsave(&tcpc->timer_tick_lock, flags);
	tcpc->timer_tick |= RT_MASK64(nr);
	spin_unlock_irqrestore(&tcpc->timer_tick_lock, flags);
}

static const char *const tcpc_timer_name[] = {
/* TYPEC_RT_TIMER (out of spec )*/
	"TYPEC_RT_TIMER_SAFE0V_DELAY",
	"TYPEC_RT_TIMER_SAFE0V_TOUT",
	"TYPEC_RT_TIMER_ROLE_SWAP_START",
	"TYPEC_RT_TIMER_ROLE_SWAP_STOP",
	"TYPEC_RT_TIMER_LEGACY",
	"TYPEC_RT_TIMER_NOT_LEGACY",
	"TYPEC_RT_TIMER_LEGACY_STABLE",
	"TYPEC_RT_TIMER_LEGACY_RECYCLE",
	"TYPEC_RT_TIMER_AUTO_DISCHARGE",
	"TYPEC_RT_TIMER_LOW_POWER_MODE",
/* TYPEC-TRY-TIMER */
	"TYPEC_TRY_TIMER_DRP_TRY",
	"TYPEC_TRY_TIMER_DRP_TRYWAIT",
/* TYPEC-DEBOUNCE-TIMER */
	"TYPEC_TIMER_CCDEBOUNCE",
	"TYPEC_TIMER_PDDEBOUNCE",
	"TYPEC_TIMER_WAKEUP",
	"TYPEC_TIMER_DRP_SRC_TOGGLE",
};

#ifdef CONFIG_TCPC_VSAFE0V_DETECT
#define TYPEC_RT_TIMER_SAFE0V_DLY_TOUT		TIMEOUT_VAL(35)
#else
#define TYPEC_RT_TIMER_SAFE0V_DLY_TOUT		TIMEOUT_VAL(100)
#endif

static const uint32_t tcpc_timer_timeout[PD_TIMER_NR] = {
/* TYPEC_RT_TIMER (out of spec )*/
	TYPEC_RT_TIMER_SAFE0V_DLY_TOUT,
	TIMEOUT_VAL(650),
	TIMEOUT_VAL(20),
	TIMEOUT_VAL(CONFIG_TYPEC_CAP_ROLE_SWAP_TOUT),
	TIMEOUT_VAL(50),
	TIMEOUT_VAL(5000),
	TIMEOUT_VAL(30*1000),
	TIMEOUT_VAL(300*1000),
	TIMEOUT_VAL(CONFIG_TYPEC_CAP_AUTO_DISCHARGE_TOUT),
	TIMEOUT_VAL(500),
/* TYPEC-TRY-TIMER */
	TIMEOUT_RANGE(75, 150),
	TIMEOUT_RANGE(400, 800),
/* TYPEC-DEBOUNCE-TIMER */
	TIMEOUT_RANGE(100, 200),
	TIMEOUT_RANGE(10, 10),
	TIMEOUT_VAL(300*1000),
	TIMEOUT_VAL(60),
};

typedef enum hrtimer_restart (*tcpc_hrtimer_call)(struct hrtimer *timer);

#define TCPC_TIMER_TRIGGER()	do \
{				\
	tcpc_set_timer_tick(tcpc_dev, index);	\
	wake_up_interruptible(&tcpc_dev->timer_wait_que);	\
} while (0)

/* TYPEC_RT_TIMER (out of spec ) */

static enum hrtimer_restart tcpc_timer_rt_vsafe0v_delay(struct hrtimer *timer)
{
	int index = TYPEC_RT_TIMER_SAFE0V_DELAY;
	struct tcpc_device *tcpc_dev =
		container_of(timer, struct tcpc_device, tcpc_timer[index]);

	TCPC_TIMER_TRIGGER();
	return HRTIMER_NORESTART;
}

static enum hrtimer_restart tcpc_timer_rt_vsafe0v_tout(struct hrtimer *timer)
{
	int index = TYPEC_RT_TIMER_SAFE0V_TOUT;
	struct tcpc_device *tcpc_dev =
		container_of(timer, struct tcpc_device, tcpc_timer[index]);

	TCPC_TIMER_TRIGGER();
	return HRTIMER_NORESTART;
}

static enum hrtimer_restart tcpc_timer_rt_role_swap_start(struct hrtimer *timer)
{
	int index = TYPEC_RT_TIMER_ROLE_SWAP_START;
	struct tcpc_device *tcpc_dev =
		container_of(timer, struct tcpc_device, tcpc_timer[index]);

	TCPC_TIMER_TRIGGER();
	return HRTIMER_NORESTART;
}

static enum hrtimer_restart tcpc_timer_rt_role_swap_stop(struct hrtimer *timer)
{
	int index = TYPEC_RT_TIMER_ROLE_SWAP_STOP;
	struct tcpc_device *tcpc_dev =
		container_of(timer, struct tcpc_device, tcpc_timer[index]);

	TCPC_TIMER_TRIGGER();
	return HRTIMER_NORESTART;
}

static enum hrtimer_restart tcpc_timer_rt_legacy(struct hrtimer *timer)
{
	int index = TYPEC_RT_TIMER_LEGACY;
	struct tcpc_device *tcpc_dev =
		container_of(timer, struct tcpc_device, tcpc_timer[index]);

	TCPC_TIMER_TRIGGER();
	return HRTIMER_NORESTART;
}

static enum hrtimer_restart tcpc_timer_rt_not_legacy(struct hrtimer *timer)
{
	int index = TYPEC_RT_TIMER_NOT_LEGACY;
	struct tcpc_device *tcpc_dev =
		container_of(timer, struct tcpc_device, tcpc_timer[index]);

	TCPC_TIMER_TRIGGER();
	return HRTIMER_NORESTART;
}

static enum hrtimer_restart tcpc_timer_rt_legacy_stable(struct hrtimer *timer)
{
	int index = TYPEC_RT_TIMER_LEGACY_STABLE;
	struct tcpc_device *tcpc_dev =
		container_of(timer, struct tcpc_device, tcpc_timer[index]);

	TCPC_TIMER_TRIGGER();
	return HRTIMER_NORESTART;
}

static enum hrtimer_restart tcpc_timer_rt_legacy_recycle(struct hrtimer *timer)
{
	int index = TYPEC_RT_TIMER_LEGACY_RECYCLE;
	struct tcpc_device *tcpc_dev =
		container_of(timer, struct tcpc_device, tcpc_timer[index]);

	TCPC_TIMER_TRIGGER();
	return HRTIMER_NORESTART;
}

static enum hrtimer_restart tcpc_timer_rt_auto_discharge(struct hrtimer *timer)
{
	int index = TYPEC_RT_TIMER_AUTO_DISCHARGE;
	struct tcpc_device *tcpc_dev =
		container_of(timer, struct tcpc_device, tcpc_timer[index]);

	TCPC_TIMER_TRIGGER();
	return HRTIMER_NORESTART;
}

static enum hrtimer_restart tcpc_timer_rt_low_power_mode(struct hrtimer *timer)
{
	int index = TYPEC_RT_TIMER_LOW_POWER_MODE;
	struct tcpc_device *tcpc_dev =
		container_of(timer, struct tcpc_device, tcpc_timer[index]);

	TCPC_TIMER_TRIGGER();
	return HRTIMER_NORESTART;
}

/* TYPEC-TRY-TIMER */
static enum hrtimer_restart tcpc_timer_try_drp_try(struct hrtimer *timer)
{
	int index = TYPEC_TRY_TIMER_DRP_TRY;
	struct tcpc_device *tcpc_dev =
		container_of(timer, struct tcpc_device, tcpc_timer[index]);

	TCPC_TIMER_TRIGGER();
	return HRTIMER_NORESTART;
}

static enum hrtimer_restart tcpc_timer_try_drp_trywait(struct hrtimer *timer)
{
	int index = TYPEC_TRY_TIMER_DRP_TRYWAIT;
	struct tcpc_device *tcpc_dev =
		container_of(timer, struct tcpc_device, tcpc_timer[index]);

	TCPC_TIMER_TRIGGER();
	return HRTIMER_NORESTART;
}

/* TYPEC-DEBOUNCE-TIMER */
static enum hrtimer_restart tcpc_timer_ccdebounce(struct hrtimer *timer)
{
	int index = TYPEC_TIMER_CCDEBOUNCE;
	struct tcpc_device *tcpc_dev =
		container_of(timer, struct tcpc_device, tcpc_timer[index]);

	TCPC_TIMER_TRIGGER();
	return HRTIMER_NORESTART;
}

static enum hrtimer_restart tcpc_timer_pddebounce(struct hrtimer *timer)
{
	int index = TYPEC_TIMER_PDDEBOUNCE;
	struct tcpc_device *tcpc_dev =
		container_of(timer, struct tcpc_device, tcpc_timer[index]);

	TCPC_TIMER_TRIGGER();
	return HRTIMER_NORESTART;
}

static enum hrtimer_restart tcpc_timer_wakeup(struct hrtimer *timer)
{
	int index = TYPEC_TIMER_WAKEUP;
	struct tcpc_device *tcpc_dev =
		container_of(timer, struct tcpc_device, tcpc_timer[index]);

	TCPC_TIMER_TRIGGER();
	return HRTIMER_NORESTART;
}

static enum hrtimer_restart tcpc_timer_drp_src_toggle(struct hrtimer *timer)
{
	int index = TYPEC_TIMER_DRP_SRC_TOGGLE;
	struct tcpc_device *tcpc_dev =
		container_of(timer, struct tcpc_device, tcpc_timer[index]);

	TCPC_TIMER_TRIGGER();
	return HRTIMER_NORESTART;
}

static tcpc_hrtimer_call tcpc_timer_call[PD_TIMER_NR] = {
/* TYPEC_RT_TIMER (out of spec )*/
	tcpc_timer_rt_vsafe0v_delay,
	tcpc_timer_rt_vsafe0v_tout,
	tcpc_timer_rt_role_swap_start,
	tcpc_timer_rt_role_swap_stop,
	tcpc_timer_rt_legacy,
	tcpc_timer_rt_not_legacy,
	tcpc_timer_rt_legacy_stable,
	tcpc_timer_rt_legacy_recycle,
	tcpc_timer_rt_auto_discharge,
	tcpc_timer_rt_low_power_mode,
/* TYPEC-TRY-TIMER */
	tcpc_timer_try_drp_try,
	tcpc_timer_try_drp_trywait,
/* TYPEC-DEBOUNCE-TIMER */
	tcpc_timer_ccdebounce,
	tcpc_timer_pddebounce,
	tcpc_timer_wakeup,
	tcpc_timer_drp_src_toggle,
};

/*
 * [BLOCK] Control Timer
 */

static inline void tcpc_reset_timer_range(
		struct tcpc_device *tcpc, int start, int end)
{
	int i;
	uint64_t mask;

	mask = tcpc_get_timer_enable_mask(tcpc);

	for (i = start; i <= end; i++) {
		if (mask & RT_MASK64(i)) {
			hrtimer_try_to_cancel(&tcpc->tcpc_timer[i]);
			tcpc_clear_timer_enable_mask(tcpc, i);
		}
	}
}

void tcpc_restart_timer(struct tcpc_device *tcpc, uint32_t timer_id)
{
	uint64_t mask;

	mask = tcpc_get_timer_enable_mask(tcpc);

	if (mask & RT_MASK64(timer_id))
		tcpc_disable_timer(tcpc, timer_id);

	tcpc_enable_timer(tcpc, timer_id);
}

void tcpc_enable_timer(struct tcpc_device *tcpc, uint32_t timer_id)
{
	uint32_t r, mod, tout;

	TCPC_TIMER_EN_DBG(tcpc, timer_id);
	PD_BUG_ON(timer_id >= PD_TIMER_NR);

	mutex_lock(&tcpc->timer_lock);
	if (timer_id >= TYPEC_TIMER_START_ID)
		tcpc_reset_timer_range(tcpc, TYPEC_TIMER_START_ID, PD_TIMER_NR);

	tcpc_set_timer_enable_mask(tcpc, timer_id);

	tout = tcpc_timer_timeout[timer_id];

	r =  tout / 1000000;
	mod = tout % 1000000;

	mutex_unlock(&tcpc->timer_lock);
	hrtimer_start(&tcpc->tcpc_timer[timer_id],
				ktime_set(r, mod*1000), HRTIMER_MODE_REL);
}

void tcpc_disable_timer(struct tcpc_device *tcpc_dev, uint32_t timer_id)
{
	uint64_t mask;

	mask = tcpc_get_timer_enable_mask(tcpc_dev);

	PD_BUG_ON(timer_id >= PD_TIMER_NR);
	if (mask & RT_MASK64(timer_id)) {
		hrtimer_try_to_cancel(&tcpc_dev->tcpc_timer[timer_id]);
		tcpc_clear_timer_enable_mask(tcpc_dev, timer_id);
	}
}

void tcpc_timer_reset(struct tcpc_device *tcpc_dev)
{
	uint64_t mask;
	int i;

	mask = tcpc_get_timer_enable_mask(tcpc_dev);

	for (i = 0; i < PD_TIMER_NR; i++)
		if (mask & RT_MASK64(i))
			hrtimer_try_to_cancel(&tcpc_dev->tcpc_timer[i]);

	tcpc_reset_timer_enable_mask(tcpc_dev);
}

void tcpc_reset_typec_debounce_timer(struct tcpc_device *tcpc)
{
	mutex_lock(&tcpc->timer_lock);
	tcpc_reset_timer_range(tcpc, TYPEC_TIMER_START_ID, PD_TIMER_NR);
	mutex_unlock(&tcpc->timer_lock);
}

void tcpc_reset_typec_try_timer(struct tcpc_device *tcpc)
{
	mutex_lock(&tcpc->timer_lock);
	tcpc_reset_timer_range(tcpc,
			TYPEC_TRY_TIMER_START_ID, TYPEC_TIMER_START_ID);
	mutex_unlock(&tcpc->timer_lock);
}

static void tcpc_handle_timer_triggered(struct tcpc_device *tcpc_dev)
{
	uint64_t triggered_timer;
	int i = 0;

	triggered_timer = tcpc_get_timer_tick(tcpc_dev);

	mutex_lock(&tcpc_dev->typec_lock);
	for (; i < PD_TIMER_NR; i++) {
		if (triggered_timer & RT_MASK64(i)) {
			TCPC_TIMER_DBG(tcpc_dev, i);
			tcpc_typec_handle_timeout(tcpc_dev, i);
			tcpc_clear_timer_tick(tcpc_dev, i);
		}
	}
	mutex_unlock(&tcpc_dev->typec_lock);

}

static int tcpc_timer_thread(void *param)
{
	struct tcpc_device *tcpc_dev = param;

	uint64_t *timer_tick;
	struct sched_param sch_param = {.sched_priority = MAX_RT_PRIO - 1};

	timer_tick = &tcpc_dev->timer_tick;

	sched_setscheduler(current, SCHED_FIFO, &sch_param);
	while (true) {
		wait_event_interruptible(tcpc_dev->timer_wait_que,
				((*timer_tick) ? true : false) |
				tcpc_dev->timer_thead_stop);
		if (kthread_should_stop() || tcpc_dev->timer_thead_stop)
			break;
		do {
			tcpc_handle_timer_triggered(tcpc_dev);
		} while (*timer_tick);
	}
	return 0;
}

int tcpci_timer_init(struct tcpc_device *tcpc_dev)
{
	int i;

	pr_info("PD Timer number = %d\n", PD_TIMER_NR);
	tcpc_dev->timer_task = kthread_create(tcpc_timer_thread, tcpc_dev,
			"tcpc_timer_%s.%p", dev_name(&tcpc_dev->dev), tcpc_dev);
	init_waitqueue_head(&tcpc_dev->timer_wait_que);

	tcpc_dev->timer_tick = 0;
	tcpc_dev->timer_enable_mask = 0;

	wake_up_process(tcpc_dev->timer_task);
	for (i = 0; i < PD_TIMER_NR; i++) {
		hrtimer_init(&tcpc_dev->tcpc_timer[i],
					CLOCK_MONOTONIC, HRTIMER_MODE_REL);
		tcpc_dev->tcpc_timer[i].function = tcpc_timer_call[i];
	}

	pr_info("%s : init OK\n", __func__);
	return 0;
}

int tcpci_timer_deinit(struct tcpc_device *tcpc_dev)
{
	uint64_t mask;
	int i;

	mask = tcpc_get_timer_enable_mask(tcpc_dev);

	mutex_lock(&tcpc_dev->timer_lock);
	wake_up_interruptible(&tcpc_dev->timer_wait_que);
	kthread_stop(tcpc_dev->timer_task);
	for (i = 0; i < PD_TIMER_NR; i++) {
		if (mask & RT_MASK64(i))
			hrtimer_try_to_cancel(&tcpc_dev->tcpc_timer[i]);
	}

	pr_info("%s : de init OK\n", __func__);
	mutex_unlock(&tcpc_dev->timer_lock);
	return 0;
}
