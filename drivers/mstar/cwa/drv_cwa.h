#ifndef _DRV_CWA_H_
#define _DRV_CWA_H_

#include <linux/kernel.h>
#include "mdrv_cwa.h"
#include "mdrv_cwa_io_st.h"
#include "hal_cwa.h"

CWA_DRV_STATE  cwa_drv_isr_handler(s32 irq, cwa_drv_handle *handle);
s32 cwa_drv_init(cwa_drv_handle *handle, struct platform_device *pdev, phys_addr_t base_addr, phys_addr_t base_sys, phys_addr_t base_axi2miu0, phys_addr_t base_axi2miu1, phys_addr_t base_axi2miu2, phys_addr_t base_axi2miu3);
s32 cwa_drv_enable_irq(cwa_drv_handle *handle);
void cwa_drv_release(cwa_drv_handle *handle);
CWA_IOC_ERROR cwa_drv_process(cwa_drv_handle *handle, cwa_file_data *file_data);
cwa_file_data* cwa_drv_post_process(cwa_drv_handle *handle);
CWA_IOC_ERROR cwa_drv_check(cwa_drv_handle *handle);

#endif //_DRV_CWA_H_
