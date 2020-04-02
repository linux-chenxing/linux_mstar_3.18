#ifndef _HAL_CWA_H_
#define _HAL_CWA_H_

#include <linux/kernel.h>
#include "hal_cwa_reg.h"
#include "mdrv_cwa_io_st.h"

typedef struct
{
    phys_addr_t base_addr;
    cwa_hal_reg_bank reg_bank;

} cwa_hal_handle;

void cwa_hal_set_AXI(cwa_hal_handle* handle, cwa_ioc_config* config);
void cwa_hal_set_image_point(cwa_hal_handle* handle, cwa_ioc_config* config);
void cwa_hal_set_image_size(cwa_hal_handle* handle, cwa_ioc_config* config);
void cwa_hal_set_output_tile(cwa_hal_handle* handle, cwa_ioc_config* config);
void cwa_hal_set_disp(cwa_hal_handle* handle, cwa_ioc_config* config);
void cwa_hal_set_bb(cwa_hal_handle* handle, cwa_ioc_config* config);
void cwa_hal_set_pers_matirx(cwa_hal_handle* handle, cwa_ioc_config* config);
void cwa_hal_set_out_of_range(cwa_hal_handle* handle, cwa_ioc_config* config);
void cwa_hal_set_config(cwa_hal_handle* handle, cwa_ioc_config* config);
void cwa_hal_get_hw_status(cwa_hal_handle* handle);
void cwa_hal_init(cwa_hal_handle *handle, phys_addr_t base_addr);

#endif //_HAL_CWA_H_
