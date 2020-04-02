/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __MSTAR_DVB_CI__
#define __MSTAR_DVB_CI__

#define DRIVER_NAME "mstar-ci"

typedef struct {
	struct platform_device *pdev;
    struct dvb_ca_en50221 ca;
    u32 count;
    int irq;

	struct dvb_adapter adapter;
} mstar_ci;

int ci_register(mstar_ci *ci, struct dvb_adapter *adapter);
int ci_unregister(mstar_ci *ci);

#endif /* __MSTAR_DVB_CI__ */
