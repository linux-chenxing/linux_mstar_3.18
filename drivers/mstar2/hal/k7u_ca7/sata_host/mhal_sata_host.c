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

void MHal_SATA_HW_Inital(phys_addr_t misc_base, phys_addr_t port_base, phys_addr_t hba_base)
{
    u16 u16Temp;
    u8 u8Temp;

    printk("sata hal init\n");

    // ECO bit, to fix 1k boundary issue.
    //u16Temp = readw(MSTAR_RIU_BASE + (0x2d35 << 1));
    u16Temp = readw(misc_base + SATA_MISC_AGEN_F_VAL);
    u16Temp |= 0x8000;
    writew(u16Temp, misc_base + SATA_MISC_AGEN_F_VAL);

    // Enable MAC Clock, bank is chip dependent
    writew(0x000c, MSTAR_RIU_BASE + (0x100b64 << 1)); // [0] gate
                                                      // [1] clk invert
                                                      // [4:2] 0:240M, 1:216M, [3:clk_miu]


    writew(0x0001, misc_base + SATA_MISC_HOST_SWRST);
    writew(0x0000, misc_base + SATA_MISC_HOST_SWRST);
    writew(0x0000, misc_base + SATA_MISC_AMBA_MUXRST);
    writew(0x008C, misc_base + SATA_MISC_AMBA_ARBIT);

    // AHB Data FIFO  Setting
    writew(0x0000, misc_base + SATA_MISC_HBA_HADDR);
    writew(0x0000, misc_base + SATA_MISC_HBA_LADDR);
    writew(0x1800, misc_base + SATA_MISC_CMD_HADDR);
    writew(0x1000, misc_base + SATA_MISC_CMD_LADDR);
    writew(0x0000, misc_base + SATA_MISC_DATA_ADDR);
    writew(0x0001, misc_base + SATA_MISC_ENRELOAD);
    writew(0x0001, misc_base + SATA_MISC_ACCESS_MODE);

    // SATA Phy Setting, based on 20160701 21:05 script
    writew(0x0000, MSTAR_RIU_BASE + (0x103922 << 1));
    writew(0x0000, MSTAR_RIU_BASE + (0x103924 << 1));
    writew(0x8304, MSTAR_RIU_BASE + (0x10390c << 1));
    writew(0x0200, MSTAR_RIU_BASE + (0x10390e << 1));
    writew(0x0000, MSTAR_RIU_BASE + (0x103910 << 1));
    writew(0x08b8, MSTAR_RIU_BASE + (0x10390a << 1));
    writew(0x8087, MSTAR_RIU_BASE + (0x103902 << 1));
    writew(0x808e, MSTAR_RIU_BASE + (0x103902 << 1));

    // for Gen2 connection
    u16Temp = readw(MSTAR_RIU_BASE + (0x103902 << 1));
    u16Temp &= 0xF3FF;
    u16Temp |= (0x1 << 10);
    writew(u16Temp, MSTAR_RIU_BASE + ((0x103902 << 1)));

    u16Temp = readw(MSTAR_RIU_BASE + (0x10390c << 1));
    u16Temp &= 0xF0FF;
    u16Temp |= (0x1 << 8);
    writew(u16Temp, MSTAR_RIU_BASE + ((0x10390c << 1)));

    writew(0x2320, MSTAR_RIU_BASE + (0x103918 << 1)); // ?
    writew(0x0040, MSTAR_RIU_BASE + (0x10397c << 1));

    // Gen1
    // Tx swing
    writew(0x2031, MSTAR_RIU_BASE + (0x103994 << 1));
    writew(0x3803, MSTAR_RIU_BASE + (0x103996 << 1));
    // EQ BW
    u16Temp = readw(MSTAR_RIU_BASE + (0x103992 << 1));
    u16Temp &= ~0x0006;
    u16Temp |= (0x0006 & 0x0020);
    writew(u16Temp, MSTAR_RIU_BASE + ((0x103992 << 1)));
    // EQ STR
    u16Temp = readw(MSTAR_RIU_BASE + (0x103992 << 1));
    u16Temp &= ~0x001f;
    u16Temp |= (0x001f & 0x0000);
    writew(u16Temp, MSTAR_RIU_BASE + ((0x103992 << 1)));

    // Gen2
    // Tx swing
    writew(0x3935, MSTAR_RIU_BASE + (0x10392c << 1));
    writew(0x3920, MSTAR_RIU_BASE + (0x10392e << 1));
    // EQ BW
    u16Temp = readw(MSTAR_RIU_BASE + (0x103950 << 1));
    u16Temp &= ~0x1800;
    u16Temp |= (0x1800 & 0x0800);
    writew(u16Temp, MSTAR_RIU_BASE + ((0x103950 << 1)));
    // EQ STR
    u16Temp = readw(MSTAR_RIU_BASE + (0x103926 << 1));
    u16Temp &= ~0x001f;
    u16Temp |= (0x001f & 0x0000);
    writew(u16Temp, MSTAR_RIU_BASE + ((0x103926 << 1)));

    // Gen3
    // Tx swing
    writew(0x3937, MSTAR_RIU_BASE + (0x10399a << 1));
    writew(0x3921, MSTAR_RIU_BASE + (0x10399c << 1));
    // EQ BW
    u16Temp = readw(MSTAR_RIU_BASE + (0x103998 << 1));
    u16Temp &= ~0x0006;
    u16Temp |= (0x0006 & 0x0000);
    writew(u16Temp, MSTAR_RIU_BASE + ((0x103998 << 1)));
    // EQ STR
    u16Temp = readw(MSTAR_RIU_BASE + (0x103998 << 1));
    u16Temp &= ~0x001f;
    u16Temp |= (0x001f & 0x0000);
    writew(u16Temp, MSTAR_RIU_BASE + ((0x103998 << 1)));

    // SSC setup
    writew(0x3007, MSTAR_RIU_BASE + (0x103964 << 1));
    writew(0xdf0a, MSTAR_RIU_BASE + (0x103960 << 1));
    writeb(0x24, MSTAR_RIU_BASE + (0x103962 << 1));
    writew(0x02e8, MSTAR_RIU_BASE + (0x103966 << 1));

    writew(0x0001, hba_base + HOST_CTL); // reset HBA
}

void MHal_SATA_Setup_Port_Implement(phys_addr_t misc_base, phys_addr_t port_base, phys_addr_t hba_base)
{
    //  Init FW to trigger controller
    writew(0x0000, hba_base + (HOST_CAP));
    writew(0x0000, hba_base + (HOST_CAP + 0x4));

    // Port Implement
    writew(0x0001, hba_base + (HOST_PORTS_IMPL));
    writew(0x0000, port_base + (PORT_CMD));
    writew(0x0000, port_base + (PORT_CMD + 0x4));
}

phys_addr_t MHal_SATA_bus_address(phys_addr_t phy_address)
{
    //phys_addr_t bus_address;

    //printk("phy addr = 0x%llx\n", phy_address);
    if (phy_address >= MIU_INTERVAL_SATA) {
        //printk("select MIU1, bus addr = 0x%8.8x\n", phy_address + 0x20000000);
        //return phy_address + MIU_INTERVAL_SATA;
        return phy_address + 0x20000000;
    }
    else {
        //printk("select MIU0, bus addr = 0x%8.8x\n", phy_address - 0x20000000);
        //return phy_address - MIU_INTERVAL_SATA;
        return phy_address - 0x20000000;
    }
}
