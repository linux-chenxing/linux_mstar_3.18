#ifndef _BOOT_LOADER_H_
#define _BOOT_LOADER_H_

#include <linux/types.h>
#include "hal_ceva.h"

long boot_loader(ceva_hal_handle *handle, phys_addr_t buffer_phys, u8 *buffer_virt, u32 buffer_size, u8 *image, u32 image_size);

#endif // _BOOT_LOADER_H_
