
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
#include <linux/debugfs.h> // single_open()
#include "hal_sdio.h"

#define PATCH_BCM_USING_SLOW_CLK_IN_SDR104	0

#ifdef SDIO_SUPPORT_SD30
#define SW_CTRL_PCB_IO_VOLT					1 // for normal SD 3.0
#define SW_CTRL_PCB_IO_VOLT_FORCE_18V		2 // for SDIO WiFi device using SW control IO voltage
#define HW_CTRL_PCB_IO_VOLT					3 // for SDIO WiFi device using HW fixed IO voltage
#define SDIO_CTRL_PCB_IO_VOLT_TYPE			SW_CTRL_PCB_IO_VOLT // change me if your PCB has different type
#endif

#define KEEP_SDIO_BUS_CLOCK	0

void mmc_sdio_request(struct mmc_host *host, struct mmc_request *req);
void mmc_sdio_pre_req(struct mmc_data *data);
void mmc_sdio_post_req(struct mmc_data *data);
void mmc_sdio_set_ios(struct mmc_host *host, struct mmc_ios *ios);
int mmc_sdio_get_cd(struct mmc_host *host);
int mmc_sdio_get_ro(struct mmc_host *host);

int mmc_sdio_hotplug(void *data);
