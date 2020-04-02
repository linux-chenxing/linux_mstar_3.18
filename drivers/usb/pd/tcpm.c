/*
 * Copyright (C) 2016 Richtek Technology Corp.
 *
 * Power Delivery Managert Driver
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

#include <linux/usb/tcpm.h>
#include <linux/usb/tcpci.h>
#include <linux/usb/tcpci_typec.h>


/* Check status */

static inline int tcpm_check_typec_attached(struct tcpc_device *tcpc)
{
	if (tcpc->typec_attach_old == TYPEC_UNATTACHED ||
		tcpc->typec_attach_new == TYPEC_UNATTACHED)
		return TCPM_ERROR_UNATTACHED;

	return 0;
}

/* Inquire TCPC status */

int tcpm_shutdown(struct tcpc_device *tcpc_dev)
{
#ifdef CONFIG_TCPC_SHUTDOWN_VBUS_DISABLE
	if (tcpc_dev->typec_power_ctrl)
		tcpci_disable_vbus_control(tcpc_dev);
#endif	/* CONFIG_TCPC_SHUTDOWN_VBUS_DISABLE */

	if (tcpc_dev->ops->deinit)
		tcpc_dev->ops->deinit(tcpc_dev);

	return 0;
}

int tcpm_inquire_remote_cc(struct tcpc_device *tcpc_dev,
	uint8_t *cc1, uint8_t *cc2, bool from_ic)
{
	int rv = 0;

	if (from_ic) {
		rv = tcpci_get_cc(tcpc_dev);
		if (rv < 0)
			return rv;
	}

	*cc1 = tcpc_dev->typec_remote_cc[0];
	*cc2 = tcpc_dev->typec_remote_cc[1];
	return 0;
}

int tcpm_inquire_vbus_level(
	struct tcpc_device *tcpc_dev, bool from_ic)
{
	int rv = 0;
	uint16_t power_status = 0;

	if (from_ic) {
		rv = tcpci_get_power_status(tcpc_dev, &power_status);
		if (rv < 0)
			return rv;

		tcpci_vbus_level_init(tcpc_dev, power_status);
	}

	return tcpc_dev->vbus_level;
}

bool tcpm_inquire_cc_polarity(
	struct tcpc_device *tcpc_dev)
{
	return tcpc_dev->typec_polarity;
}

uint8_t tcpm_inquire_typec_attach_state(
	struct tcpc_device *tcpc_dev)
{
	return tcpc_dev->typec_attach_new;
}

uint8_t tcpm_inquire_typec_role(
	struct tcpc_device *tcpc_dev)
{
	return tcpc_dev->typec_role;
}

uint8_t tcpm_inquire_typec_local_rp(
	struct tcpc_device *tcpc_dev)
{
	uint8_t level;

	switch (tcpc_dev->typec_local_rp_level) {
	case TYPEC_CC_RP_1_5:
		level = 1;
		break;

	case TYPEC_CC_RP_3_0:
		level = 2;
		break;

	default:
	case TYPEC_CC_RP_DFT:
		level = 0;
		break;
	}

	return level;
}

int tcpm_typec_set_wake_lock(
	struct tcpc_device *tcpc, bool user_lock)
{
	int ret;

	mutex_lock(&tcpc->access_lock);
	ret = tcpci_set_wake_lock(
		tcpc, tcpc->wake_lock_pd, user_lock);
	tcpc->wake_lock_user = user_lock;
	mutex_unlock(&tcpc->access_lock);

	return ret;
}

int tcpm_typec_set_usb_sink_curr(
	struct tcpc_device *tcpc_dev, int curr)
{
	bool force_sink_vbus = true;

	tcpc_dev->typec_usb_sink_curr = curr;

	if (tcpc_dev->typec_remote_rp_level != TYPEC_CC_VOLT_SNK_DFT)
		force_sink_vbus = false;

	if (force_sink_vbus) {
		tcpci_sink_vbus(tcpc_dev,
			TCP_VBUS_CTRL_TYPEC, TCPC_VBUS_SINK_5V, -1);
	}

	return 0;
}

int tcpm_typec_set_rp_level(
	struct tcpc_device *tcpc_dev, uint8_t level)
{
	uint8_t res;

	if (level == 2)
		res = TYPEC_CC_RP_3_0;
	else if (level == 1)
		res = TYPEC_CC_RP_1_5;
	else
		res = TYPEC_CC_RP_DFT;

	return tcpc_typec_set_rp_level(tcpc_dev, res);
}

int tcpm_typec_set_custom_hv(struct tcpc_device *tcpc, bool en)
{
#ifdef CONFIG_TYPEC_CAP_CUSTOM_HV
	int ret;

	mutex_lock(&tcpc->access_lock);
	ret = tcpm_check_typec_attached(tcpc);
	if (ret == TCPM_SUCCESS)
		tcpc->typec_during_custom_hv = en;
	mutex_unlock(&tcpc->access_lock);

	return ret;
#else
	return TCPM_ERROR_NO_SUPPORT;
#endif	/* CONFIG_TYPEC_CAP_CUSTOM_HV */
}

int tcpm_typec_role_swap(struct tcpc_device *tcpc_dev)
{
#ifdef CONFIG_TYPEC_CAP_ROLE_SWAP
	int ret = tcpm_check_typec_attached(tcpc_dev);

	if (ret != TCPM_SUCCESS)
		return ret;

	return tcpc_typec_swap_role(tcpc_dev);
#else
	return TCPM_ERROR_NO_SUPPORT;
#endif /* CONFIG_TYPEC_CAP_ROLE_SWAP */
}

int tcpm_typec_change_role(
	struct tcpc_device *tcpc_dev, uint8_t typec_role)
{
	return tcpc_typec_change_role(tcpc_dev, typec_role);
}

bool tcpm_get_boot_check_flag(struct tcpc_device *tcpc)
{
	return tcpc->boot_check_flag ? true : false;
}

void tcpm_set_boot_check_flag(struct tcpc_device *tcpc, unsigned char en)
{
	tcpc->boot_check_flag = en ? 1 : 0;
}

bool tcpm_get_ta_hw_exist(struct tcpc_device *tcpc)
{
	uint16_t data;

	tcpci_get_power_status(tcpc, &data);
	return (data & TCPC_REG_POWER_STATUS_VBUS_PRES);
}

