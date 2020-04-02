
#include <linux/mmc/host.h> // struct mmc_host_ops, mmc_host
#include <linux/mmc/core.h> // struct mmc_command, struct mmc_data, struct mmc_request
#include <linux/platform_device.h> // struct platform_device
#include <linux/module.h> //
#include <linux/dma-mapping.h> // dma_map_sg, dma_unmap_sg
#include <linux/errno.h> // error code
#include <linux/semaphore.h> // semaphore, down(), up()
#include <linux/kthread.h> // kthread_run()
#include <linux/delay.h> // msleep()
#include <linux/mmc/card.h> // struct mmc_card
#include "hal_fcie.h"

struct host_fcie {

	unsigned char rw_timeout;
	unsigned char bad_card;
};

void mmc_fcie_request(struct mmc_host *host, struct mmc_request *req);
void mmc_fcie_pre_req(struct mmc_data *data);
void mmc_fcie_post_req(struct mmc_data *data);
void mmc_fcie_set_ios(struct mmc_host *host, struct mmc_ios *ios);
int mmc_fcie_get_cd(struct mmc_host *host);
int mmc_fcie_get_ro(struct mmc_host *host);

int mmc_fcie_hotplug(void *data);


