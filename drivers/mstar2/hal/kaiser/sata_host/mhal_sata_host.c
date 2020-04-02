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

void MHal_SATA_HW_Inital(u32 misc_base, u32 port_base, u32 hba_base)
{
    u16 u16Temp;
    u8 u8Temp;

    // Open SATA PHY Clock
    writew(0x0030, 0xfd220000 + (0x28a2 << 1));

    // Set MIU1 Access for SATA
    u16Temp = readw(0xfd200000 + (0x06f6 << 1));
    u16Temp |= 0x8000;
    writew(u16Temp, 0xfd200000 + (0x06f6 << 1));

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

    // SATA Phy Setting
    writew(0x0000, 0xfd240000 + (0x2322 << 1));
    writew(0x2514, 0xfd240000 + (0x2324 << 1));
    writew(0x0302, 0xfd240000 + (0x230c << 1));
    writew(0x2200, 0xfd240000 + (0x230e << 1));
    writew(0x0300, 0xfd240000 + (0x2310 << 1));
    writew(0x2190, 0xfd240000 + (0x2368 << 1));
    writew(0x0000, 0xfd240000 + (0x237e << 1));
    writew(0x0a3d, 0xfd240000 + (0x2360 << 1));
    writew(0x0017, 0xfd240000 + (0x2362 << 1));
    writew(0x8087, 0xfd240000 + (0x2302 << 1));
    writew(0x8086, 0xfd240000 + (0x2302 << 1));

    // rterm setup
    u16Temp = readw(0xfd240000 + (0x2330 << 1));
    u16Temp &= ~0xffc0;
    u16Temp |= 0x83c0;
    writew(u16Temp, 0xfd240000 + (0x2330 << 1));
    writew(0x6c29, 0xfd240000 + (0x232c << 1));

    // cominit comwake time setup
    u8Temp = readb(0xfd240000 + ((0x2303 << 1)-1));
    u8Temp &= ~0x02;
    u8Temp |= 0x02;
    writeb(u8Temp, 0xfd240000 + ((0x2303 << 1)-1));
    u8Temp = readb(0xfd240000 + ((0x2303 << 1)-1));
    u8Temp &= ~0x10;
    u8Temp |= 0x10;
    writeb(u8Temp, 0xfd240000 + ((0x2303 << 1)-1));
    writeb(0x2a, 0xfd240000 + (0x2306 << 1));
    writeb(0x0b, 0xfd240000 + ((0x2307 << 1)-1));
    writeb(0x1c, 0xfd240000 + (0x2308 << 1));
    writeb(0x30, 0xfd240000 + ((0x2309 << 1)-1));
    writeb(0x5a, 0xfd240000 + (0x230a << 1));

    // enable SSC
    u8Temp = readb(0xfd240000 + ((0x2365 << 1)-1));
    u8Temp &= ~0x20;
    u8Temp |= 0x20;
    writeb(u8Temp, 0xfd240000 + ((0x2365 << 1)-1));
    writeb(0x17, 0xfd240000 + (0x2362 << 1));
    writew(0x0a3d, 0xfd240000 + (0x2360 << 1));
    u8Temp = readb(0xfd240000 + ((0x2367 << 1)-1));
    u8Temp &= ~0x7f;
    u8Temp |= 0x04;
    writeb(u8Temp, 0xfd240000 + ((0x2367 << 1)-1));
    writeb(0x70, 0xfd240000 + (0x2366 << 1));
    u8Temp = readb(0xfd240000 + ((0x2365 << 1)-1));
    u8Temp &= ~0x07;
    u8Temp |= 0x00;
    writeb(u8Temp, 0xfd240000 + ((0x2365 << 1)-1));
    writeb(0x02, 0xfd240000 + (0x2364 << 1));
}

void MHal_SATA_Setup_Port_Implement(u32 misc_base, u32 port_base, u32 hba_base)
{
    //  Init FW to trigger controller
    writew(0x0000, hba_base + (HOST_CAP));
    writew(0x0000, hba_base + (HOST_CAP + 0x4));

    // Port Implement
    writew(0x0001, hba_base + (HOST_PORTS_IMPL));
    writew(0x0000, port_base + (PORT_CMD));
    writew(0x0000, port_base + (PORT_CMD + 0x4));
}
