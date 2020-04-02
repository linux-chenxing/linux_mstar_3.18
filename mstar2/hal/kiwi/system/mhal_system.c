///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2006 - 2007 MStar Semiconductor, Inc.
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    drv_system_io.c
/// @brief  System Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <asm/uaccess.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <asm/io.h>     //robert.yang
#include <linux/semaphore.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/version.h>
#include <linux/autoconf.h>
#include <linux/vmalloc.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,39)
#include <linux/slab.h>
#endif
#include "mdrv_types.h"
#include "mst_platform.h"
#include "mdrv_system.h"
#include "mdrv_system_io.h"
#include "board/Board.h"
#if !(defined(CONFIG_MSTAR_TITANIA3) || defined(CONFIG_MSTAR_TITANIA10) )
#include "chip_setup.h"
#endif
#ifdef CONFIG_MSTAR_SPI_FLASH
#include "drvSERFLASH.h"
#endif

#include <linux/reboot.h>
#include <linux/vmalloc.h>

#include <linux/seq_file.h>
#include <linux/proc_fs.h>


static DEFINE_SPINLOCK(oad_upgrade_lock);

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
/*
#define DEF_STC_SYNTH               0x14000000                          // 216/2.5  = 86.4, 2.5(dec) = 00010.1(bin) = 0x14000000(u5.27)
#define MAX_STC_SYNTH               0x15000000                          // 216/2.75 = 78.55
#define MIN_STC_SYNTH               0x13000000                          // 216/2.25 = 96
*/

#define SYS_DEBUG                   1

#if SYS_DEBUG
#define SYS_PRINT(fmt, args...)     printk("[SYS][%06d]     " fmt, __LINE__, ## args)
#else
#define SYS_PRINT(fmt, args...)
#endif

#define ALL_INT_DISABLE()       local_irq_disable()
#define ALL_INT_ENABLE()        local_irq_enable()
#define printf                  printk

#define PCMCIA_NAND_SEM(x)          (x)

#if defined(CONFIG_MMC_MSTAR_MMC_EMMC) && CONFIG_MMC_MSTAR_MMC_EMMC
#define PCMCIA_LOOP_DELAY_WAIT_EMMC_DONE	(2000000)
#endif


#define KERNEL_PCMCIA_PERF 0

#if defined(CONFIG_MMC_MSTAR_MMC_EMMC) && CONFIG_MMC_MSTAR_MMC_EMMC
BOOL MHal_PCMCIA_WaitEMMCDone(U32 u32loopCnt)
{
    #if !defined(CONFIG_MSTAR_KAISER) && \
        !defined(CONFIG_MSTAR_KAISERS)
    return mstar_mci_exit_checkdone_ForCI();
    #endif
}
#endif

static U16 u16TmpReg[7];
void MHal_PCMCIA_SetPad(BOOL bRestore)
{
    if(bRestore == FALSE)
    {

        u16TmpReg[5] = REG_ADDR(REG_CHIP_PCMCFG) & REG_CHIP_PCMCFG_MASK;
        REG_ADDR(REG_CHIP_PCMCFG) |= REG_CHIP_PCMCFG_CTRL_EN;
        
        //u16RegVal3 = REG_ADDR(REG_EMMC_CONFIG) & REG_EMMC_CONFIG_MASK;
        u16TmpReg[2] = REG_ADDR(REG_EMMC_CONFIG) & REG_EMMC_CONFIG_MASK;
        REG_ADDR(REG_EMMC_CONFIG) &= ~REG_EMMC_CONFIG_MASK;
        //u16RegVal1 = REG_ADDR(REG_CHIP_PCMCFG) & REG_CHIP_PCMCFG_MASK;
        u16TmpReg[3] = REG_ADDR(REG_CHIP_PCMCFG) & REG_CHIP_PCMCFG_MASK;
        REG_ADDR(REG_CHIP_PCMCFG) |= REG_CHIP_PCMCFG_CTRL_EN;

   
        
        //u16RegVal4 = REG_ADDR(REG_SD_CONFIG2) & REG_SD_CONFIG2_MASK;
        u16TmpReg[4] = REG_ADDR(REG_SD_CONFIG2) & REG_SD_CONFIG2_MASK;
        REG_ADDR(REG_SD_CONFIG2) &= ~REG_SD_CONFIG2_MASK;

    }
    else
    {

        REG_ADDR(REG_EMMC_CONFIG) = (REG_ADDR(REG_EMMC_CONFIG) & ~REG_EMMC_CONFIG_MASK) | u16TmpReg[2];
        REG_ADDR(REG_CHIP_PCMCFG) = (REG_ADDR(REG_CHIP_PCMCFG) & ~REG_CHIP_PCMCFG_MASK) | u16TmpReg[3];


        REG_ADDR(REG_SD_CONFIG2) = (REG_ADDR(REG_SD_CONFIG2) & ~REG_SD_CONFIG2_MASK) | u16TmpReg[4];


        REG_ADDR(REG_CHIP_PCMCFG) = (REG_ADDR(REG_CHIP_PCMCFG) & ~REG_CHIP_PCMCFG_MASK) | u16TmpReg[5];

    }


}
