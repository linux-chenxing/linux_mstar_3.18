#ifndef HAL_CLK_H
#define HAL_CLK_H

#include <linux/kernel.h>
#include "ms_platform.h"

#define RIU_BASE_ADDR (0x1F000000) //need to match real address
#define GET_REG8_ADDR(x, y)     (x+(y)*2)
#define INSREG8(addr, mask, val) OUTREG8(addr, ((INREG8(addr)&(~(mask))) | val))

int clk_hal_init(void);

#endif
