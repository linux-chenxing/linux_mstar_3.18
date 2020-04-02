/*
 * Copyright (C) 2016 Richtek Technology Corp.
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

#ifndef TCPC_TIMER_H_INCLUDED
#define TCPC_TIMER_H_INCLUDED

#include <linux/kernel.h>
#include "tcpci_config.h"

struct tcpc_device;
enum {
/* TYPEC_RT_TIMER (out of spec )*/
	TYPEC_RT_TIMER_START_ID = 0,
	TYPEC_RT_TIMER_SAFE0V_DELAY = TYPEC_RT_TIMER_START_ID,
	TYPEC_RT_TIMER_SAFE0V_TOUT,
	TYPEC_RT_TIMER_ROLE_SWAP_START,
	TYPEC_RT_TIMER_ROLE_SWAP_STOP,
	TYPEC_RT_TIMER_LEGACY,
	TYPEC_RT_TIMER_NOT_LEGACY,
	TYPEC_RT_TIMER_LEGACY_STABLE,
	TYPEC_RT_TIMER_LEGACY_RECYCLE,
	TYPEC_RT_TIMER_AUTO_DISCHARGE,
	TYPEC_RT_TIMER_LOW_POWER_MODE,
/* TYPEC-TRY-TIMER */
	TYPEC_TRY_TIMER_START_ID,
	TYPEC_TRY_TIMER_DRP_TRY = TYPEC_TRY_TIMER_START_ID,
	TYPEC_TRY_TIMER_DRP_TRYWAIT,
/* TYPEC-DEBOUNCE-TIMER */
	TYPEC_TIMER_START_ID,
	TYPEC_TIMER_CCDEBOUNCE = TYPEC_TIMER_START_ID,
	TYPEC_TIMER_PDDEBOUNCE,
	TYPEC_TIMER_WAKEUP,
	TYPEC_TIMER_DRP_SRC_TOGGLE,
	PD_TIMER_NR,
};


extern int tcpci_timer_init(struct tcpc_device *tcpc);
extern int tcpci_timer_deinit(struct tcpc_device *tcpc);
extern void tcpc_restart_timer(struct tcpc_device *tcpc, uint32_t timer_id);
extern  void tcpc_enable_timer(struct tcpc_device *tcpc, uint32_t timer_id);
extern  void tcpc_disable_timer(
		struct tcpc_device *tcpc, uint32_t timer_id);
extern void tcpc_reset_typec_try_timer(struct tcpc_device *tcpc);
extern void tcpc_reset_typec_debounce_timer(struct tcpc_device *tcpc);

extern void tcpc_reset_pe_timer(struct tcpc_device *tcpc);

#endif /* TCPC_TIMER_H_INCLUDED */
