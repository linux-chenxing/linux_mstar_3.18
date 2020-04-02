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

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <linux/autoconf.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/cdev.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <asm/io.h>

#include "mdrv_types.h"
#include "mdrv_pm.h"
#include "mhal_pm.h"
#include "mhal_pm_reg.h"

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
/*16-bit RIU address*/
u16 MHal_PM_ReadReg16(u32 reg)
{
    u8 val;
    val = *((volatile u16 *) (PM_RIU_REG_BASE + (reg << 1)));
    return val;
}

void MHal_PM_WriteReg16(u32 reg, u16 val)
{
    *((volatile u16 *) (PM_RIU_REG_BASE + (reg << 1))) = val;
}


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
PM_Result MHal_PM_SetSRAMOffsetForMCU(void)
{
    u32 start_addr = 0x0000UL;
    u32 end_addr   = 0x5FFFUL; // Size: 24K
    u32 u32Cnt;

    //Disable 8051
    MHal_PM_WriteReg16(REG_PM_CPU_SW_RST, MHal_PM_ReadReg16(REG_PM_CPU_SW_RST) & ~(BIT12|BIT8));
    //MCK CLK=XTAL
    MHal_PM_WriteReg16(REG_PM_MCU_CLK, MHal_PM_ReadReg16(REG_PM_MCU_CLK) & ~BIT7);

    //i_cache rstz
    MHal_PM_WriteReg16(REG_MCU_CONFIG, MHal_PM_ReadReg16(REG_MCU_CONFIG) & ~BIT3);

    //disable i cache (enable icache bypass)
    MHal_PM_WriteReg16(REG_MCU_CACHE_CONFIG, MHal_PM_ReadReg16(REG_MCU_CACHE_CONFIG) | BIT0);

    //DISALBE SPI/DRAM
    MHal_PM_WriteReg16(REG_MCU_CONFIG, MHal_PM_ReadReg16(REG_MCU_CONFIG) & ~(BIT1|BIT2|BIT3));

    //ENABLE SRAM
    MHal_PM_WriteReg16(REG_MCU_CONFIG, MHal_PM_ReadReg16(REG_MCU_CONFIG) | BIT0);

    //Set Sram address
    MHal_PM_WriteReg16(REG_MCU_SRAM_START_ADDR_L, (u16)  start_addr & 0xFFFFUL);
    MHal_PM_WriteReg16(REG_MCU_SRAM_START_ADDR_H, (u16) (start_addr >> 16) & 0xFFUL);
    MHal_PM_WriteReg16(REG_MCU_SRAM_END_ADDR_L, (u16)  end_addr & 0xFFFFUL);
    MHal_PM_WriteReg16(REG_MCU_SRAM_END_ADDR_H, (u16) (end_addr >> 16) & 0xFFUL);

    MHal_PM_WriteReg16(REG_PM_RST_CPU0_PASSWORD, 0x829fUL);
    MHal_PM_WriteReg16(REG_PM_CPU_SW_RST, MHal_PM_ReadReg16(REG_PM_CPU_SW_RST) | BIT8);
    udelay(1200);
    MHal_PM_WriteReg16(REG_PM_CPU_SW_RST, MHal_PM_ReadReg16(REG_PM_CPU_SW_RST) | BIT12);

    printk("PM Wait for PM51 standby...........\n");

	u32Cnt = 0;
	while(MHal_PM_ReadReg16(REG_PM_LOCK) != 0x02UL)
    {
        u32Cnt++;
        if (u32Cnt == 0x10000)
        {
            printk("PM51 run fail...........\n");
            return E_PM_TIMEOUT;
        }
        udelay(120);
    }
    printk("PM51 run ok...........\n");
    return E_PM_OK;
}

//-------------------------------------------------------------------------------------------------
PM_Result MHal_PM_CopyBin2Sram(u32 u32SrcAddr)
{
    u32 u32Cnt;

    //Disable 8051
    MHal_PM_WriteReg16(REG_PM_CPU_SW_RST, MHal_PM_ReadReg16(REG_PM_CPU_SW_RST) & ~(BIT12|BIT8));

    MHal_PM_WriteReg16(0x100904UL, 0x0a40);
    //src address
    MHal_PM_WriteReg16(0x100908UL, u32SrcAddr & 0xffff);
    MHal_PM_WriteReg16(0x10090aUL, (u32SrcAddr >> 16) & 0xffff);
    //dst address
    MHal_PM_WriteReg16(0x10090cUL, 0x0000);
    MHal_PM_WriteReg16(0x10090eUL, 0x0000);
    //set size
    MHal_PM_WriteReg16(0x100910UL, 0x6000);
    MHal_PM_WriteReg16(0x100912UL, 0x0000);


    MHal_PM_WriteReg16(0x100900UL, 0x0001);

	u32Cnt=0;
	while(MHal_PM_ReadReg16(0x100900UL) & 0x1)
    {
        u32Cnt++;
        if (u32Cnt == 0x10000)
        {
            return E_PM_TIMEOUT;
        }
        udelay(120);
    }
    return E_PM_OK;
}

//-------------------------------------------------------------------------------------------------
void MHal_PM_SetDataAddr2Register(u32 u32DramAddr, u32 u32Size)
{
    u8 loop;
    for (loop=0; loop<0x8; loop++)
    {
        MHal_PM_WriteReg16(0x103380+(loop<<1), 0x0000);
    }
    MHal_PM_WriteReg16(0x103380, 0x2015);
    MHal_PM_WriteReg16(0x103382, (u16) (u32DramAddr >> 16));
    MHal_PM_WriteReg16(0x103384, (u16) u32DramAddr);
    MHal_PM_WriteReg16(0x103386, (u16) (u32Size >> 16));
    MHal_PM_WriteReg16(0x103388, (u16) u32Size);
   
    MHal_PM_WriteReg16(0x10338a, 0x55aa);
}












