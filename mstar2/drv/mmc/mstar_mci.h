///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2006 - 2011 MStar Semiconductor, Inc.
// This program is free software.
// You can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation;
// either version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program;
// if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef MSTAR_MCI_H
#define MSTAR_MCI_H

#include "eMMC.h"

/******************************************************************************
* Function define for this driver
******************************************************************************/

/******************************************************************************
* Register Address Base
******************************************************************************/
#define CLK_400KHz       		400*1000
#define CLK_200MHz       		200*1000*1000

#define eMMC_GENERIC_WAIT_TIME  (HW_TIMER_DELAY_1s*10)
#define eMMC_READ_WAIT_TIME     (HW_TIMER_DELAY_500ms)

/******************************************************************************
* Low level type for this driver
******************************************************************************/
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,20)
#if defined(ENABLE_EMMC_ASYNC_IO) && ENABLE_EMMC_ASYNC_IO
struct mstar_mci_host_next
{
    unsigned int                dma_len;
    s32                         cookie;
};
#endif
#endif

struct mstar_mci_host
{
    struct mmc_host			    *mmc;
    struct mmc_command		    *cmd;
    struct mmc_request		    *request;

    void __iomem			    *baseaddr;
    s32						    irq;

    u16						    sd_clk;
    u16						    sd_mod;

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,20)
    #if defined(ENABLE_EMMC_ASYNC_IO) && ENABLE_EMMC_ASYNC_IO
    struct mstar_mci_host_next  next_data;
    struct work_struct          async_work;
    #endif
    #endif

};  /* struct mstar_mci_host*/

#endif
