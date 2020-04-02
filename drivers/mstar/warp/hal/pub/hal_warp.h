#ifndef _HAL_WARP_H_
#define _HAL_WARP_H_

#include "hal_warp_reg.h"
#include "hal_warp_data.h"

#include <linux/kernel.h>

typedef struct
{
    phys_addr_t base_addr;
    warp_hal_reg_bank reg_bank;

} warp_hal_handle;

void warp_hal_set_axi(warp_hal_handle* handle, HAL_WARP_CONFIG* config);
void warp_hal_set_image_point(warp_hal_handle* handle, HAL_WARP_CONFIG* config);
void warp_hal_set_image_size(warp_hal_handle* handle, HAL_WARP_CONFIG* config);
void warp_hal_set_output_tile(warp_hal_handle* handle, HAL_WARP_CONFIG* config);
void warp_hal_set_disp(warp_hal_handle* handle, HAL_WARP_CONFIG* config);
void warp_hal_set_bb(warp_hal_handle* handle, HAL_WARP_CONFIG* config);
void warp_hal_set_pers_matirx(warp_hal_handle* handle, HAL_WARP_CONFIG* config);
void warp_hal_set_out_of_range(warp_hal_handle* handle, HAL_WARP_CONFIG* config);
void warp_hal_set_config(warp_hal_handle* handle, HAL_WARP_CONFIG* config);
void warp_hal_get_hw_status(warp_hal_handle* handle);
void warp_hal_init(warp_hal_handle *handle, phys_addr_t base_addr);

#endif //_HAL_WARP_H_