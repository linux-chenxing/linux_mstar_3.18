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
/// file    mhal_sata_host.c
/// @brief  SATA Host Hal Driver
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <linux/kernel.h>
#include <asm/io.h>
#include <mhal_sata_host.h>
#include <linux/delay.h>
#include "ms_platform.h"
extern ptrdiff_t mstar_pm_base;
#define sata_reg_write16(val, addr) { (*((volatile unsigned short*)(addr))) = (unsigned short)(val); }
#define sata_reg_write8(val, addr) { (*((volatile unsigned char*)(addr))) = (unsigned char)(val);}
#define GET_REG8_ADDR(x, y)     (x+(y)*2)
#define GET_REG16_ADDR(x, y)    (x+(y)*4)
#define RIU_BASE_ADDR 0xFD000000

void MHal_SATA_HW_Inital(phys_addr_t misc_base, phys_addr_t port_base, phys_addr_t hba_base)
{
    u16 u16Temp;
    u32 GHC_PHY = 0x0;
    //u8 u8Temp;
    //u32 GHC_CLK;
    // unsigned volatile short debreg;
    //unsigned volatile char debreg_b;
    printk("sata hal init\n");

    if(port_base == SATA_GHC_0_P0_ADDRESS_START)
        GHC_PHY = SATA_GHC_0_PHY;//0x103900
    else if(port_base == SATA_GHC_1_P0_ADDRESS_START)
        GHC_PHY = SATA_GHC_1_PHY;//0x162A00

    printk("=================MSTAR_RIU_BASE:%08x, misc_base:%08X , hba_base:%08X , port_base:%08X , GHC_PHY : %08X\n", MSTAR_RIU_BASE, misc_base, hba_base, port_base, GHC_PHY);
    //=================MSTAR_RIU_BASE:fd000000, misc_base:FD205A00 , hba_base:FD205800 , port_base:FD226400 , GHC_PHY : 00000000

    // ECO bit, to fix 1k boundary issue.

    //u16Temp = readw((const volatile void *)MSTAR_RIU_BASE + (0x2d35 << 1));
    u16Temp = readw((volatile void *)misc_base + SATA_MISC_AGEN_F_VAL);
    u16Temp |= 0x8000;
    writew(u16Temp, (volatile void *)misc_base + SATA_MISC_AGEN_F_VAL);

    // Enable MAC Clock, bank is chip dependent
    writew(0x0c0c, (volatile void *)MSTAR_RIU_BASE + (0x100b64 << 1)); // [0] gate
    // [1] clk invert
    // [4:2] 0:240M, 1:216M, [3:clk_miu]
    //    writew(0x00c0,(volatile void *)MSTAR_RIU_BASE + (0x100b64 << 1)); // [0] gate
    // [1] clk invert
    // [4:2] 0:240M, 1:216M, [3:clk_miu]

    //       writew(0x000c, (volatile void *)MSTAR_RIU_BASE + (0x100b64 << 1)); // [0] gate
    // [1] clk invert
    // [4:2] 0:240M, 1:216M, [3:clk_miu]
    //	writew(0x000c, (volatile void *)MSTAR_RIU_BASE + (0x100b32 << 1));
    //}else{
    //writew(0x00c0, (volatile void *)MSTAR_RIU_BASE + (0x100b64 << 1)); // [0] gate
    // [1] clk invert
    // [4:2] 0:240M, 1:216M, [3:clk_miu]
    // }

    writew(0x0001, (volatile void *)misc_base + SATA_MISC_HOST_SWRST);//25
    writew(0x0000, (volatile void *)misc_base + SATA_MISC_HOST_SWRST);//25
    writew(0x0000, (volatile void *)misc_base + SATA_MISC_AMBA_MUXRST);//21
    writew(0x008C, (volatile void *)misc_base + SATA_MISC_AMBA_ARBIT);//2
    //writew(0x0003, (volatile void *)misc_base +(0x55<<2));//FPGA mode
    // AHB Data FIFO  Setting
    writew(0x0000, (volatile void *)misc_base + SATA_MISC_HBA_HADDR);
    writew(0x0000, (volatile void *)misc_base + SATA_MISC_HBA_LADDR);
    writew(0x1800, (volatile void *)misc_base + SATA_MISC_CMD_HADDR);
    writew(0x1000, (volatile void *)misc_base + SATA_MISC_CMD_LADDR);
    writew(0x0000, (volatile void *)misc_base + SATA_MISC_DATA_ADDR);
    writew(0x0001, (volatile void *)misc_base + SATA_MISC_ENRELOAD);
    writew(0x0001, (volatile void *)misc_base + SATA_MISC_ACCESS_MODE);

    //printk("check sata speed !!!!!!\n");
    u16Temp = readw((volatile void *)port_base + (0x2c << 1));
    u16Temp = u16Temp & (~E_PORT_SPEED_MASK);
    u16Temp = u16Temp | MHal_SATA_get_max_speed();
    writew(u16Temp, (volatile void *)port_base + (0x2c << 1));
    u16Temp = readw((volatile void *)port_base + (0x2c << 1));
    printk("MAC sata speed= 0x%x\n", u16Temp);


    //printk("sata hal PHY init !!!!!!\n");
#if 1
    //printk("Gen1 Tx swing\n");
    writew(0x2031, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x94) << 1));
    writew(0x3803, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x96) << 1));
    // EQ BW
    //printk("Gen1 EQ BW\n");
    u16Temp = readw((volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x92) << 1));
    u16Temp &= ~0x0060;
    u16Temp |= 0x0020;  // default is 0x01
    writew(u16Temp, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x92) << 1));
    // EQ STR
    //printk("Gen1 EQ STR\n");
    u16Temp = readw((volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x92) << 1));
    u16Temp &= ~0x001f;
    u16Temp |= 0x0003;  // default is 0x03
    writew(u16Temp, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x92) << 1));

    // Gen2
    // Tx swing
    //printk("Gen2 Tx swing\n");
    writew(0x3935, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x2c) << 1));
    writew(0x3920, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x2e) << 1));
    // EQ BW
    //printk("Gen2 EQ BW\n");
    u16Temp = readw((volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x50) << 1));
    u16Temp &= ~0x1800;
    u16Temp |= 0x0800;  // default is 0x01
    writew(u16Temp, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x50) << 1));
    // EQ STR
    //printk("Gen2 EQ STR\n");
    u16Temp = readw((volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x26) << 1));
    u16Temp &= ~0x001f;
    u16Temp |= 0x0003;  // default is 0x03
    writew(u16Temp, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x26) << 1));

    // Gen3
    // Tx swing
    //printk("Gen3 Tx swing\n");
    writew(0x3937, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x9a) << 1));
    writew(0x3921, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x9c) << 1));
    // EQ BW
    //printk("Gen3 EQ BW\n");
    u16Temp = readw((volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x98) << 1));
    u16Temp &= ~0x0060;
    u16Temp |= 0x0000;  // default is 0x00
    writew(u16Temp, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x98) << 1));
    // EQ STR
    //printk("Gen3 EQ EQ STR\n");
    //printk("EQ BW\n");
    u16Temp = readw((volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x98) << 1));
    u16Temp &= ~0x001f;
    u16Temp |= 0x0003;  // default is 0x03
    writew(u16Temp, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x98) << 1));

    // SSC setup
    writew(0x3007, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x64) << 1));
    writew(0xdf0a, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x60) << 1));
    writeb(0x24, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x62) << 1));
    writew(0x02e8, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x66) << 1));
#endif

#if 0
    //printk("sata hal PHY init done!!!!!!\n");
    writew(0x0001, (volatile void *)misc_base + SATA_MISC_HOST_SWRST);//25
    writew(0x0000, (volatile void *)misc_base + SATA_MISC_HOST_SWRST);//25
    writew(0x0000, (volatile void *)misc_base + SATA_MISC_AMBA_MUXRST);//21
    writew(0x008C, (volatile void *)misc_base + SATA_MISC_AMBA_ARBIT);//2
    //writew(0x0003, (volatile void *)misc_base +(0x55<<2));//FPGA mode
    // AHB Data FIFO  Setting
    writew(0x0000, (volatile void *)misc_base + SATA_MISC_HBA_HADDR);
    writew(0x0000, (volatile void *)misc_base + SATA_MISC_HBA_LADDR);
    writew(0x1800, (volatile void *)misc_base + SATA_MISC_CMD_HADDR);
    writew(0x1000, (volatile void *)misc_base + SATA_MISC_CMD_LADDR);
    writew(0x0000, (volatile void *)misc_base + SATA_MISC_DATA_ADDR);
    writew(0x0001, (volatile void *)misc_base + SATA_MISC_ENRELOAD);
    writew(0x0001, (volatile void *)misc_base + SATA_MISC_ACCESS_MODE);


    //#if 1
    u16Temp = readw((volatile void *)port_base + (0x2c << 1));
    u16Temp = u16Temp & (~E_PORT_SPEED_MASK);
    u16Temp = u16Temp | MHal_SATA_get_max_speed();
    writew(u16Temp, (volatile void *)port_base + (0x2c << 1));
    u16Temp = readw((volatile void *)port_base + (0x2c << 1));
    printk("MAC sata speed= 0x%x\n", u16Temp);
    //#endif
#endif

#if 0 //test
    printk("sata SATA PHY OOB setting ..\n");
    writew(0x96de, (volatile void *)MSTAR_RIU_BASE + (0x103902 << 1));
    //writew(0x031e, MSTAR_RIU_BASE + (0x103934 << 1));
    writew(0x0f20, (volatile void *)MSTAR_RIU_BASE + (0x103906 << 1));
    writew(0x341f, (volatile void *)MSTAR_RIU_BASE + (0x103908 << 1));

    printk("sata SATA PHY setting ..\n");
    writew(0x0000, (volatile void *)MSTAR_RIU_BASE + (0x103922 << 1));
    writew(0x0000, (volatile void *)MSTAR_RIU_BASE + (0x103924 << 1));
    writew(0x0a3d, (volatile void *)MSTAR_RIU_BASE + (0x103960 << 1));
    writeb(0x17, (volatile void *)MSTAR_RIU_BASE + (0x103962 << 1));
    writew(0x9103, (volatile void *)MSTAR_RIU_BASE + (0x10390c << 1));
    writeb(0x06, (volatile void *)MSTAR_RIU_BASE + (0x10393e << 1));
    writew(0x2200, (volatile void *)MSTAR_RIU_BASE + (0x10390e << 1));
    writew(0x0000, (volatile void *)MSTAR_RIU_BASE + (0x103910 << 1));
    writeb(0x0f, (volatile void *)MSTAR_RIU_BASE + (0x10390b << 1) - 1);
    writew(0x888f, (volatile void *)MSTAR_RIU_BASE + (0x103902 << 1));
    writew(0x888e, (volatile void *)MSTAR_RIU_BASE + (0x103902 << 1));
    writew(0x831c, (volatile void *)MSTAR_RIU_BASE + (0x103968 << 1));
    writew(0x031c, (volatile void *)MSTAR_RIU_BASE + (0x1039c0 << 1));
    writew(0x031c, (volatile void *)MSTAR_RIU_BASE + (0x1039c2 << 1));
    writeb(0x30, (volatile void *)MSTAR_RIU_BASE + (0x103919 << 1) - 1);
    writeb(0x80, (volatile void *)MSTAR_RIU_BASE + (0x10393c << 1));
    writeb(0x50, (volatile void *)MSTAR_RIU_BASE + (0x103933 << 1) - 1);
    writeb(0x03, (volatile void *)MSTAR_RIU_BASE + (0x103998 << 1));

#else //update new settings for indinity2

    printk("sata SATA PHY OOB setting ..\n");
    writew(0x96de, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x02) << 1));
    //writew(0x031e, MSTAR_RIU_BASE + (0x103934 << 1));
    writew(0x0f20, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x06) << 1));
    writew(0x341f, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x08) << 1));

    printk("sata SATA PHY setting ..\n");
    //asm volatile( "loop: b loop");

#if 1  // new setting on 2018/01/10
    writew(0x0000, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x22) << 1));
    writew(0x0000, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x24) << 1));
    writew(0x0a3d, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x60) << 1));
    writeb(0x17, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x62) << 1));
    writew(0x9103, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x0c) << 1));
    writeb(0x06, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x3e) << 1));
    writew(0x0000, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x10) << 1));  // Enable clocks
    writeb(0x0b, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x0b) << 1) - 1);  // [10]: OOB pattern select
    writew(0x9a8f, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x02) << 1));  // [0]: Reset
    writew(0x9a8e, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x02) << 1));
    writew(0x831f, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x68) << 1));
    writew(0x031f, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0xc0) << 1));
    writew(0x031f, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0xc2) << 1));
    writeb(0x30, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x19) << 1) - 1);
    writeb(0x80, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x3c) << 1));
    writeb(0x50, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x33) << 1) - 1);  // [12]: reg_force_tx_oob_pat
    writew(0x2200, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x0e) << 1));  // [9]: Enable host
#else
    writew(0x0000, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x22) << 1));
    writew(0x0000, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x24) << 1));
    writew(0x0a3d, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x60) << 1));
    writeb(0x17, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x62) << 1));
    writew(0x9103, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x0c) << 1));
    writeb(0x06, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x3e) << 1));
    writew(0x2200, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x0e) << 1));
    writew(0x0000, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x10) << 1));
    writeb(0x0f, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x0b) << 1) - 1);
    writew(0x888f, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x02) << 1));
    writew(0x888e, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x02) << 1));
    writew(0x831c, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x68) << 1));
    writew(0x031c, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0xc0) << 1));
    writew(0x031c, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0xc2) << 1));
    writeb(0x30, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x19) << 1) - 1);
    writeb(0x80, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x3c) << 1));
    writeb(0x50, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x33) << 1) - 1);
    writeb(0x03, (volatile void *)MSTAR_RIU_BASE + ((GHC_PHY | 0x98) << 1));
#endif

#endif

    writew(0x0001, (volatile void *) hba_base + MS_HOST_CTL); // reset HBA

}
EXPORT_SYMBOL_GPL(MHal_SATA_HW_Inital);

void MHal_SATA_Setup_Port_Implement(phys_addr_t misc_base, phys_addr_t port_base, phys_addr_t hba_base)
{
    //  Init FW to trigger controller
    writew(0x0000, (volatile void *)hba_base + (MS_HOST_CAP));
    writew(0x0000, (volatile void *)hba_base + (MS_HOST_CAP + 0x4));

    // Port Implement
    writew(0x0001, (volatile void *)hba_base + (MS_HOST_PORTS_IMPL));
    writew(0x0000, (volatile void *)port_base + (MS_PORT_CMD));
    writew(0x0000, (volatile void *)port_base + (MS_PORT_CMD + 0x4));
}
EXPORT_SYMBOL_GPL(MHal_SATA_Setup_Port_Implement);

phys_addr_t MHal_SATA_bus_address(phys_addr_t phy_address)
{
    //phys_addr_t bus_address;

    //printk("phy addr = 0x%llx\n", phy_address);
    if (phy_address >= MIU_INTERVAL_SATA)
    {
        //printk("select MIU1, bus addr = 0x%8.8x\n", phy_address + 0x20000000);
        //return phy_address + MIU_INTERVAL_SATA;
        return phy_address + 0x20000000;
    }
    else
    {
        //printk("select MIU0, bus addr = 0x%8.8x\n", phy_address - 0x20000000);
        //return phy_address - MIU_INTERVAL_SATA;
        return phy_address - 0x20000000;
    }
}
EXPORT_SYMBOL_GPL(MHal_SATA_bus_address);

u32 MHal_SATA_get_max_speed(void)
{
    return E_PORT_SPEED_GEN3;
}
EXPORT_SYMBOL_GPL(MHal_SATA_get_max_speed);
