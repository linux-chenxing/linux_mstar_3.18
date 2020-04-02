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

#ifndef TCPM_H_
#define TCPM_H_

#include <linux/kernel.h>
#include <linux/notifier.h>

#include "tcpci_config.h"

struct tcpc_device;

/*
 * Type-C Port Notify Chain
 */

enum typec_attach_type {
	TYPEC_UNATTACHED = 0,
	TYPEC_ATTACHED_SNK,
	TYPEC_ATTACHED_SRC,
	TYPEC_ATTACHED_AUDIO,
	TYPEC_ATTACHED_DEBUG,		/* Rd, Rd */

#ifdef CONFIG_TYPEC_CAP_DBGACC_SNK
	TYPEC_ATTACHED_DBGACC_SNK,		/* Rp, Rp */
#endif	/* CONFIG_TYPEC_CAP_DBGACC_SNK */

#ifdef CONFIG_TYPEC_CAP_CUSTOM_SRC
	TYPEC_ATTACHED_CUSTOM_SRC,		/* Same Rp */
#endif	/* CONFIG_TYPEC_CAP_CUSTOM_SRC */
};

enum pd_connect_result {
	PD_CONNECT_NONE = 0,
	PD_CONNECT_TYPEC_ONLY,	/* Internal Only */
	PD_CONNECT_TYPEC_ONLY_SNK_DFT,
	PD_CONNECT_TYPEC_ONLY_SNK,
	PD_CONNECT_TYPEC_ONLY_SRC,
	PD_CONNECT_PE_READY,	/* Internal Only */
	PD_CONNECT_PE_READY_SNK,
	PD_CONNECT_PE_READY_SRC,
};

#ifdef CONFIG_TCPC_VCONN_SUPPLY_MODE

enum tcpc_vconn_supply_mode {
	/* Never provide vconn even in TYPE-C state (reject swap to On) */
	TCPC_VCONN_SUPPLY_NEVER = 0,

	/* Always provide vconn */
	TCPC_VCONN_SUPPLY_ALWAYS,

	/* Always provide vconn only if we detect Ra, otherwise startup only */
	TCPC_VCONN_SUPPLY_EMARK_ONLY,

	/* Only provide vconn during DPM initial */
	TCPC_VCONN_SUPPLY_STARTUP,

	TCPC_VCONN_SUPPLY_NR,
};

#endif	/* CONFIG_TCPC_VCONN_SUPPLY_MODE */

/* Power role */
#define PD_ROLE_SINK   0
#define PD_ROLE_SOURCE 1

/* Data role */
#define PD_ROLE_UFP    0
#define PD_ROLE_DFP    1

/* Vconn role */
#define PD_ROLE_VCONN_OFF 0
#define PD_ROLE_VCONN_ON  1

enum {
	TCP_NOTIFY_DIS_VBUS_CTRL,
	TCP_NOTIFY_SOURCE_VCONN,
	TCP_NOTIFY_SOURCE_VBUS,
	TCP_NOTIFY_SINK_VBUS,
	TCP_NOTIFY_PR_SWAP,
	TCP_NOTIFY_DR_SWAP,
	TCP_NOTIFY_VCONN_SWAP,
	TCP_NOTIFY_ENTER_MODE,
	TCP_NOTIFY_EXIT_MODE,
	TCP_NOTIFY_AMA_DP_STATE,
	TCP_NOTIFY_AMA_DP_ATTENTION,
	TCP_NOTIFY_AMA_DP_HPD_STATE,

	TCP_NOTIFY_TYPEC_STATE,
	TCP_NOTIFY_PD_STATE,

#ifdef CONFIG_TYPEC_NOTIFY_ATTACHWAIT_SNK
	TCP_NOTIFY_ATTACHWAIT_SNK,
#endif	/* CONFIG_TYPEC_NOTIFY_ATTACHWAIT_SNK */

#ifdef CONFIG_TYPEC_NOTIFY_ATTACHWAIT_SRC
	TCP_NOTIFY_ATTACHWAIT_SRC,
#endif	/* CONFIG_TYPEC_NOTIFY_ATTACHWAIT_SRC */

	TCP_NOTIFY_EXT_DISCHARGE,

	TCP_NOTIFY_HARD_RESET_STATE,
};

struct tcp_ny_pd_state {
	uint8_t connected;
};

struct tcp_ny_swap_state {
	uint8_t new_role;
};

struct tcp_ny_enable_state {
	bool en;
};

struct tcp_ny_typec_state {
	uint8_t rp_level;
	uint8_t polarity;
	uint8_t old_state;
	uint8_t new_state;
};

enum {
	TCP_VBUS_CTRL_REMOVE = 0,
	TCP_VBUS_CTRL_TYPEC = 1,
	TCP_VBUS_CTRL_PD = 2,

	TCP_VBUS_CTRL_HRESET = TCP_VBUS_CTRL_PD,
	TCP_VBUS_CTRL_PR_SWAP = 3,
	TCP_VBUS_CTRL_REQUEST = 4,
	TCP_VBUS_CTRL_STANDBY = 5,
	TCP_VBUS_CTRL_STANDBY_UP = 6,
	TCP_VBUS_CTRL_STANDBY_DOWN = 7,

	TCP_VBUS_CTRL_PD_DETECT = (1 << 7),

	TCP_VBUS_CTRL_PD_HRESET =
		TCP_VBUS_CTRL_HRESET | TCP_VBUS_CTRL_PD_DETECT,

	TCP_VBUS_CTRL_PD_PR_SWAP =
		TCP_VBUS_CTRL_PR_SWAP | TCP_VBUS_CTRL_PD_DETECT,

	TCP_VBUS_CTRL_PD_REQUEST =
		TCP_VBUS_CTRL_REQUEST | TCP_VBUS_CTRL_PD_DETECT,

	TCP_VBUS_CTRL_PD_STANDBY =
		TCP_VBUS_CTRL_STANDBY | TCP_VBUS_CTRL_PD_DETECT,

	TCP_VBUS_CTRL_PD_STANDBY_UP =
		TCP_VBUS_CTRL_STANDBY_UP | TCP_VBUS_CTRL_PD_DETECT,

	TCP_VBUS_CTRL_PD_STANDBY_DOWN =
		TCP_VBUS_CTRL_STANDBY_DOWN | TCP_VBUS_CTRL_PD_DETECT,
};

struct tcp_ny_vbus_state {
	int mv;
	int ma;
	uint8_t type;
};

struct tcp_ny_mode_ctrl {
	uint16_t svid;
	uint8_t ops;
	uint32_t mode;
};

enum {
	SW_USB = 0,
	SW_DFP_D,
	SW_UFP_D,
};

struct tcp_ny_ama_dp_state {
	uint8_t sel_config;
	uint8_t signal;
	uint8_t pin_assignment;
	uint8_t polarity;
	uint8_t active;
};

enum {
	TCP_DP_UFP_U_MASK = 0x7C,
	TCP_DP_UFP_U_POWER_LOW = 1 << 2,
	TCP_DP_UFP_U_ENABLED = 1 << 3,
	TCP_DP_UFP_U_MF_PREFER = 1 << 4,
	TCP_DP_UFP_U_USB_CONFIG = 1 << 5,
	TCP_DP_UFP_U_EXIT_MODE = 1 << 6,
};

struct tcp_ny_ama_dp_attention {
	uint8_t state;
};

struct tcp_ny_ama_dp_hpd_state {
	bool irq:1;
	bool state:1;
};

struct tcp_ny_uvdm {
	bool ack;
	uint8_t uvdm_cnt;
	uint16_t uvdm_svid;
	uint32_t *uvdm_data;
};

/*
 * TCP_NOTIFY_HARD_RESET_STATE
 *
 * Please don't expect that every signal will have a corresponding result.
 * The signal can be generated multiple times before receiving a result.
 */

enum {
	/* HardReset finished because recv GoodCRC or TYPE-C only */
	TCP_HRESET_RESULT_DONE = 0,

	/* HardReset failed because detach or error recovery */
	TCP_HRESET_RESULT_FAIL,

	/* HardReset signal from Local Policy Engine */
	TCP_HRESET_SIGNAL_SEND,

	/* HardReset signal from Port Partner */
	TCP_HRESET_SIGNAL_RECV,
};

struct tcp_ny_hard_reset_state {
	uint8_t state;
};

struct tcp_notify {
	union {
		struct tcp_ny_enable_state en_state;
		struct tcp_ny_vbus_state vbus_state;
		struct tcp_ny_typec_state typec_state;
		struct tcp_ny_swap_state swap_state;
		struct tcp_ny_pd_state pd_state;
		struct tcp_ny_mode_ctrl mode_ctrl;
		struct tcp_ny_ama_dp_state ama_dp_state;
		struct tcp_ny_ama_dp_attention ama_dp_attention;
		struct tcp_ny_ama_dp_hpd_state ama_dp_hpd_state;
		struct tcp_ny_uvdm uvdm_msg;
		struct tcp_ny_hard_reset_state hreset_state;
	};
};

#ifdef CONFIG_TCPC_CLASS
extern struct tcpc_device
		*tcpc_dev_get_by_name(const char *name);
#else
static inline struct tcpc_device
		*tcpc_dev_get_by_name(const char *name)
{
	return NULL;
}
#endif

extern int register_tcp_dev_notifier(struct tcpc_device *tcp_dev,
				     struct notifier_block *nb);
extern int unregister_tcp_dev_notifier(struct tcpc_device *tcp_dev,
				       struct notifier_block *nb);

/*
 * Type-C Port Control I/F
 */

enum tcpm_error_list {
	TCPM_SUCCESS = 0,
	TCPM_ERROR_UNKNOWN = -1,
	TCPM_ERROR_UNATTACHED = -2,
	TCPM_ERROR_PARAMETER = -3,
	TCPM_ERROR_PUT_EVENT = -4,
	TCPM_ERROR_NO_SUPPORT = -5,
	TCPM_ERROR_NO_PD_CONNECTED = -6,
	TCPM_ERROR_NO_POWER_CABLE = -7,
	TCPM_ERROR_NO_PARTNER_INFORM = -8,
	TCPM_ERROR_NO_SOURCE_CAP = -9,
	TCPM_ERROR_NO_SINK_CAP = -10,
	TCPM_ERROR_NOT_DRP_ROLE = -11,
	TCPM_ERROR_DURING_ROLE_SWAP = -12,
	TCPM_ERROR_NO_EXPLICIT_CONTRACT = -13,
	TCPM_ERROR_ERROR_RECOVERY = -14,
	TCPM_ERROR_NOT_FOUND = -15,
	TCPM_ERROR_CHARGING_POLICY = -16,
};

#define TCPM_PDO_MAX_SIZE	7

struct tcpm_remote_power_cap {
	uint8_t selected_cap_idx;
	uint8_t nr;
	int max_mv[TCPM_PDO_MAX_SIZE];
	int min_mv[TCPM_PDO_MAX_SIZE];
	int ma[TCPM_PDO_MAX_SIZE];
};

/* Inquire TCPM status */

enum tcpc_cc_voltage_status {
	TYPEC_CC_VOLT_OPEN = 0,
	TYPEC_CC_VOLT_RA = 1,
	TYPEC_CC_VOLT_RD = 2,

	TYPEC_CC_VOLT_SNK_DFT = 5,
	TYPEC_CC_VOLT_SNK_1_5 = 6,
	TYPEC_CC_VOLT_SNK_3_0 = 7,

	TYPEC_CC_DRP_TOGGLING = 15,
};

enum tcpm_vbus_level {
#ifdef CONFIG_TCPC_VSAFE0V_DETECT
	TCPC_VBUS_SAFE0V = 0,		/* < 0.8V */
	TCPC_VBUS_INVALID,		/* > 0.8V */
	TCPC_VBUS_VALID,		/* > 4.5V */
#else
	TCPC_VBUS_INVALID = 0,
	TCPC_VBUS_VALID,
#endif
};

enum typec_role_defination {
	TYPEC_ROLE_UNKNOWN = 0,
	TYPEC_ROLE_SNK,
	TYPEC_ROLE_SRC,
	TYPEC_ROLE_DRP,
	TYPEC_ROLE_TRY_SRC,
	TYPEC_ROLE_TRY_SNK,
	TYPEC_ROLE_NR,
};

enum pd_cable_current_limit {
	PD_CABLE_CURR_UNKNOWN = 0,
	PD_CABLE_CURR_1A5 = 1,
	PD_CABLE_CURR_3A = 2,
	PD_CABLE_CURR_5A = 3,
};

/* DPM Flags */

#define DPM_FLAGS_PARTNER_DR_POWER		(1<<0)
#define DPM_FLAGS_PARTNER_DR_DATA		(1<<1)
#define DPM_FLAGS_PARTNER_EXTPOWER		(1<<2)
#define DPM_FLAGS_PARTNER_USB_COMM		(1<<3)
#define DPM_FLAGS_PARTNER_USB_SUSPEND	(1<<4)
#define DPM_FLAGS_PARTNER_HIGH_CAP		(1<<5)

#define DPM_FLAGS_PARTNER_MISMATCH		(1<<7)
#define DPM_FLAGS_PARTNER_GIVE_BACK		(1<<8)
#define DPM_FLAGS_PARTNER_NO_SUSPEND	(1<<9)

#define DPM_FLAGS_RESET_PARTNER_MASK	\
	(DPM_FLAGS_PARTNER_DR_POWER | DPM_FLAGS_PARTNER_DR_DATA|\
	DPM_FLAGS_PARTNER_EXTPOWER | DPM_FLAGS_PARTNER_USB_COMM)

#define DPM_FLAGS_CHECK_DC_MODE			(1<<20)
#define DPM_FLAGS_CHECK_UFP_SVID		(1<<21)
#define DPM_FLAGS_CHECK_EXT_POWER		(1<<22)
#define DPM_FLAGS_CHECK_DP_MODE			(1<<23)
#define DPM_FLAGS_CHECK_SINK_CAP		(1<<24)
#define DPM_FLAGS_CHECK_SOURCE_CAP		(1<<25)
#define DPM_FLAGS_CHECK_UFP_ID			(1<<26)
#define DPM_FLAGS_CHECK_CABLE_ID		(1<<27)
#define DPM_FLAGS_CHECK_CABLE_ID_DFP		(1<<28)
#define DPM_FLAGS_CHECK_PR_ROLE			(1<<29)
#define DPM_FLAGS_CHECK_DR_ROLE			(1<<30)

/* DPM_CAPS */

#define DPM_CAP_LOCAL_DR_POWER			(1<<0)
#define DPM_CAP_LOCAL_DR_DATA			(1<<1)
#define DPM_CAP_LOCAL_EXT_POWER			(1<<2)
#define DPM_CAP_LOCAL_USB_COMM			(1<<3)
#define DPM_CAP_LOCAL_USB_SUSPEND		(1<<4)
#define DPM_CAP_LOCAL_HIGH_CAP			(1<<5)
#define DPM_CAP_LOCAL_GIVE_BACK			(1<<6)
#define DPM_CAP_LOCAL_NO_SUSPEND		(1<<7)
#define DPM_CAP_LOCAL_VCONN_SUPPLY		(1<<8)

#define DPM_CAP_ATTEMP_ENTER_DC_MODE		(1<<11)
#define DPM_CAP_ATTEMP_DISCOVER_CABLE_DFP	(1<<12)
#define DPM_CAP_ATTEMP_ENTER_DP_MODE		(1<<13)
#define DPM_CAP_ATTEMP_DISCOVER_CABLE		(1<<14)
#define DPM_CAP_ATTEMP_DISCOVER_ID		(1<<15)

enum dpm_cap_pr_check_prefer {
	DPM_CAP_PR_CHECK_DISABLE = 0,
	DPM_CAP_PR_CHECK_PREFER_SNK = 1,
	DPM_CAP_PR_CHECK_PREFER_SRC = 2,
};

#define DPM_CAP_PR_CHECK_PROP(cap)			((cap & 0x03) << 16)
#define DPM_CAP_EXTRACT_PR_CHECK(raw)		((raw >> 16) & 0x03)
#define DPM_CAP_PR_SWAP_REJECT_AS_SRC		(1<<18)
#define DPM_CAP_PR_SWAP_REJECT_AS_SNK		(1<<19)
#define DPM_CAP_PR_SWAP_CHECK_GP_SRC		(1<<20)
#define DPM_CAP_PR_SWAP_CHECK_GP_SNK		(1<<21)
#define DPM_CAP_PR_SWAP_CHECK_GOOD_POWER	\
	(DPM_CAP_PR_SWAP_CHECK_GP_SRC | DPM_CAP_PR_SWAP_CHECK_GP_SNK)

enum dpm_cap_dr_check_prefer {
	DPM_CAP_DR_CHECK_DISABLE = 0,
	DPM_CAP_DR_CHECK_PREFER_UFP = 1,
	DPM_CAP_DR_CHECK_PREFER_DFP = 2,
};

#define DPM_CAP_DR_CHECK_PROP(cap)		((cap & 0x03) << 22)
#define DPM_CAP_EXTRACT_DR_CHECK(raw)		((raw >> 22) & 0x03)
#define DPM_CAP_DR_SWAP_REJECT_AS_DFP		(1<<24)
#define DPM_CAP_DR_SWAP_REJECT_AS_UFP		(1<<25)

#define DPM_CAP_DP_PREFER_MF				(1<<29)

extern int tcpm_shutdown(struct tcpc_device *tcpc_dev);

extern int tcpm_inquire_remote_cc(struct tcpc_device *tcpc_dev,
	uint8_t *cc1, uint8_t *cc2, bool from_ic);
extern int tcpm_inquire_vbus_level(struct tcpc_device *tcpc_dev, bool from_ic);
extern bool tcpm_inquire_cc_polarity(struct tcpc_device *tcpc_dev);
extern uint8_t tcpm_inquire_typec_attach_state(struct tcpc_device *tcpc_dev);
extern uint8_t tcpm_inquire_typec_role(struct tcpc_device *tcpc_dev);
extern uint8_t tcpm_inquire_typec_local_rp(struct tcpc_device *tcpc_dev);

extern int tcpm_typec_set_wake_lock(
	struct tcpc_device *tcpc, bool user_lock);

extern int tcpm_typec_set_usb_sink_curr(
	struct tcpc_device *tcpc_dev, int curr);

extern int tcpm_typec_set_rp_level(
	struct tcpc_device *tcpc_dev, uint8_t level);

extern int tcpm_typec_set_custom_hv(
	struct tcpc_device *tcpc_dev, bool en);

extern int tcpm_typec_role_swap(
	struct tcpc_device *tcpc_dev);

extern int tcpm_typec_change_role(
	struct tcpc_device *tcpc_dev, uint8_t typec_role);

static inline bool tcpm_inquire_pd_prev_connected(
	struct tcpc_device *tcpc_dev)
{
	return false;
}

static inline int tcpm_get_remote_power_cap(struct tcpc_device *tcpc_dev,
					struct tcpm_remote_power_cap *cap)
{
	return 0;
}

static inline int tcpm_set_remote_power_cap(struct tcpc_device *tcpc,
					int mv, int ma)
{
	return 0;
}

extern bool tcpm_get_boot_check_flag(struct tcpc_device *tcpc);
extern void tcpm_set_boot_check_flag(struct tcpc_device *tcpc, unsigned char en);
extern bool tcpm_get_ta_hw_exist(struct tcpc_device *tcpc);

#endif /* TCPM_H_ */
