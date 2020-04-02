///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2008 - 2009 MStar Semiconductor, Inc.
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
/// file    color_format_driver.c
/// @brief  MStar XC Driver DDI HAL Level
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _HAL_COLOR_FORMAT_DRIVER_HARDWARE_C
#define _HAL_COLOR_FORMAT_DRIVER_HARDWARE_C
#endif

#include <linux/kernel.h>
#include <linux/interrupt.h>
#include "mdrv_mstypes.h"
#include "color_format_input.h"
#include "color_format_driver.h"
#include "color_format_driver_hardware.h"
#include "color_format_ip.h"
#include "mhal_dlc.h"
#include "mhal_xc.h"
#include "color_format_driver_hardware_gop.h"

MS_U32 Maserati22Gamma[513] = {
0x0, 0x3, 0x4, 0x5, 0x5, 0x6, 0x6, 0x7, 0x7, 0x7, 0x8, 0x8, 0x8, 0x9, 0x9, 0x9,
0xa, 0xa, 0xa, 0xa, 0xb, 0xb, 0xb, 0xb, 0xc, 0xc, 0xc, 0xc, 0xc, 0xd, 0xd, 0xd,
0xd, 0xe, 0xe, 0xe, 0xf, 0xf, 0xf, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11, 0x12, 0x12,
0x12, 0x13, 0x13, 0x14, 0x14, 0x15, 0x15, 0x15, 0x16, 0x16, 0x17, 0x17, 0x17, 0x18, 0x18, 0x19,
0x19, 0x1a, 0x1a, 0x1b, 0x1c, 0x1c, 0x1d, 0x1d, 0x1e, 0x1e, 0x1f, 0x20, 0x20, 0x21, 0x21, 0x22,
0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2b, 0x2c, 0x2d, 0x2d, 0x2e,
0x2f, 0x30, 0x31, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
0x40, 0x42, 0x44, 0x45, 0x47, 0x48, 0x4a, 0x4c, 0x4d, 0x4e, 0x50, 0x51, 0x53, 0x54, 0x55, 0x56,
0x58, 0x5a, 0x5d, 0x5f, 0x61, 0x63, 0x65, 0x67, 0x69, 0x6b, 0x6d, 0x6f, 0x71, 0x73, 0x75, 0x77,
0x78, 0x7c, 0x7f, 0x82, 0x85, 0x88, 0x8b, 0x8e, 0x91, 0x93, 0x96, 0x99, 0x9b, 0x9e, 0xa0, 0xa2,
0xa5, 0xa9, 0xae, 0xb2, 0xb6, 0xba, 0xbe, 0xc2, 0xc6, 0xca, 0xcd, 0xd1, 0xd4, 0xd8, 0xdb, 0xdf,
0xe2, 0xe8, 0xee, 0xf4, 0xfa, 0xff, 0x105, 0x10a, 0x10f, 0x115, 0x11a, 0x11e, 0x123, 0x128, 0x12c, 0x131,
0x135, 0x13e, 0x146, 0x14f, 0x156, 0x15e, 0x166, 0x16d, 0x174, 0x17b, 0x182, 0x188, 0x18f, 0x195, 0x19c, 0x1a2,
0x1a8, 0x1b4, 0x1bf, 0x1ca, 0x1d5, 0x1e0, 0x1ea, 0x1f4, 0x1fe, 0x207, 0x211, 0x21a, 0x223, 0x22c, 0x234, 0x23d,
0x245, 0x255, 0x265, 0x274, 0x283, 0x291, 0x2a0, 0x2ad, 0x2bb, 0x2c8, 0x2d4, 0x2e1, 0x2ed, 0x2f9, 0x305, 0x311,
0x31c, 0x332, 0x348, 0x35d, 0x371, 0x385, 0x398, 0x3ab, 0x3bd, 0x3cf, 0x3e1, 0x3f2, 0x403, 0x413, 0x424, 0x433,
0x443, 0x462, 0x47f, 0x49c, 0x4b8, 0x4d3, 0x4ed, 0x507, 0x520, 0x538, 0x551, 0x568, 0x57f, 0x596, 0x5ac, 0x5c2,
0x5d7, 0x601, 0x629, 0x651, 0x677, 0x69c, 0x6c0, 0x6e3, 0x706, 0x727, 0x748, 0x769, 0x788, 0x7a7, 0x7c6, 0x7e4,
0x801, 0x83a, 0x872, 0x8a7, 0x8dc, 0x90e, 0x940, 0x970, 0x9a0, 0x9ce, 0x9fb, 0xa27, 0xa52, 0xa7d, 0xaa7, 0xacf,
0xaf8, 0xb46, 0xb92, 0xbdc, 0xc23, 0xc69, 0xcad, 0xcef, 0xd30, 0xd6f, 0xdad, 0xdea, 0xe25, 0xe5f, 0xe98, 0xed0,
0xf08, 0xf73, 0xfdb, 0x1040, 0x10a2, 0x1102, 0x115f, 0x11ba, 0x1212, 0x1269, 0x12be, 0x1311, 0x1362, 0x13b2, 0x1400, 0x144d,
0x1499, 0x152c, 0x15ba, 0x1645, 0x16cb, 0x174e, 0x17ce, 0x184a, 0x18c4, 0x193a, 0x19ae, 0x1a20, 0x1a90, 0x1afd, 0x1b68, 0x1bd2,
0x1c39, 0x1d03, 0x1dc7, 0x1e84, 0x1f3d, 0x1ff0, 0x209f, 0x2149, 0x21ef, 0x2292, 0x2331, 0x23cd, 0x2466, 0x24fc, 0x258f, 0x261f,
0x26ad, 0x27c2, 0x28ce, 0x29d1, 0x2ace, 0x2bc4, 0x2cb3, 0x2d9d, 0x2e81, 0x2f60, 0x303a, 0x3110, 0x31e1, 0x32ae, 0x3378, 0x343e,
0x3500, 0x367b, 0x37ea, 0x394e, 0x3aa8, 0x3bf9, 0x3d41, 0x3e81, 0x3fba, 0x40eb, 0x4216, 0x433b, 0x445a, 0x4573, 0x4687, 0x4796,
0x48a1, 0x4aa8, 0x4c9f, 0x4e87, 0x5062, 0x522f, 0x53f1, 0x55a7, 0x5754, 0x58f6, 0x5a90, 0x5c21, 0x5daa, 0x5f2c, 0x60a6, 0x621a,
0x6387, 0x64ee, 0x664f, 0x67aa, 0x6900, 0x6a51, 0x6b9d, 0x6ce4, 0x6e27, 0x6f65, 0x709f, 0x71d5, 0x7307, 0x7435, 0x7560, 0x7687,
0x77ab, 0x78cc, 0x79e9, 0x7b03, 0x7c1a, 0x7d2e, 0x7e40, 0x7f4f, 0x805b, 0x8164, 0x826b, 0x836f, 0x8471, 0x8571, 0x866f, 0x876a,
0x8863, 0x8a4f, 0x8c32, 0x8e0e, 0x8fe3, 0x91b1, 0x9377, 0x9538, 0x96f2, 0x98a6, 0x9a55, 0x9bfd, 0x9da1, 0x9f3f, 0xa0d8, 0xa26d,
0xa3fd, 0xa588, 0xa70f, 0xa892, 0xaa10, 0xab8b, 0xad02, 0xae75, 0xafe4, 0xb150, 0xb2b8, 0xb41d, 0xb57e, 0xb6dd, 0xb838, 0xb990,
0xbae5, 0xbd87, 0xc01e, 0xc2ab, 0xc52d, 0xc7a5, 0xca15, 0xcc7b, 0xced9, 0xd12f, 0xd37d, 0xd5c3, 0xd802, 0xda39, 0xdc6a, 0xde95,
0xe0b8, 0xe2d6, 0xe4ee, 0xe700, 0xe90c, 0xeb13, 0xed14, 0xef11, 0xf108, 0xf2fa, 0xf4e8, 0xf6d1, 0xf8b6, 0xfa96, 0xfc72, 0xfe49,
0x1001d, };

//this table is from color_map_ip solution
//this table is only used for HDR PQ bypass
//pq degamma (matched case)
//deGamma(16,65280,0x40,0x40,0,g_au32DoViDegammaHdrLut,512,E_DEGAMMA_MASERATI_MD,NULL,2);
MS_U32 Maserati_DeGamma_PQ[513] = {
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
0x00000002, 0x00000002, 0x00000002, 0x00000002, 0x00000002, 0x00000003, 0x00000003, 0x00000003,
0x00000003, 0x00000003, 0x00000004, 0x00000004, 0x00000004, 0x00000005, 0x00000005, 0x00000005,
0x00000005, 0x00000006, 0x00000007, 0x00000007, 0x00000008, 0x00000009, 0x00000009, 0x0000000a,
0x0000000b, 0x0000000b, 0x0000000c, 0x0000000d, 0x0000000e, 0x0000000f, 0x00000010, 0x00000011,
0x00000011, 0x00000013, 0x00000015, 0x00000017, 0x00000019, 0x0000001c, 0x0000001e, 0x00000020,
0x00000023, 0x00000025, 0x00000028, 0x0000002a, 0x0000002d, 0x00000030, 0x00000033, 0x00000036,
0x00000039, 0x0000003c, 0x0000003f, 0x00000042, 0x00000045, 0x00000048, 0x0000004c, 0x0000004f,
0x00000053, 0x00000056, 0x0000005a, 0x0000005e, 0x00000062, 0x00000065, 0x00000069, 0x0000006d,
0x00000071, 0x00000075, 0x0000007a, 0x0000007e, 0x00000082, 0x00000086, 0x0000008b, 0x0000008f,
0x00000094, 0x00000098, 0x0000009d, 0x000000a2, 0x000000a7, 0x000000ab, 0x000000b0, 0x000000b5,
0x000000ba, 0x000000c5, 0x000000cf, 0x000000da, 0x000000e5, 0x000000f0, 0x000000fb, 0x00000107,
0x00000113, 0x0000011f, 0x0000012c, 0x00000139, 0x00000146, 0x00000153, 0x00000160, 0x0000016e,
0x0000017c, 0x0000018a, 0x00000198, 0x000001a7, 0x000001b6, 0x000001c5, 0x000001d5, 0x000001e4,
0x000001f4, 0x00000204, 0x00000215, 0x00000225, 0x00000236, 0x00000247, 0x00000259, 0x0000026a,
0x0000027c, 0x000002a1, 0x000002c6, 0x000002ed, 0x00000314, 0x0000033d, 0x00000366, 0x00000391,
0x000003bd, 0x000003e9, 0x00000417, 0x00000445, 0x00000475, 0x000004a6, 0x000004d8, 0x0000050a,
0x0000053e, 0x00000573, 0x000005a9, 0x000005e0, 0x00000618, 0x00000651, 0x0000068b, 0x000006c7,
0x00000703, 0x00000741, 0x0000077f, 0x000007bf, 0x00000800, 0x00000841, 0x00000884, 0x000008c8,
0x0000090e, 0x0000099b, 0x00000a2e, 0x00000ac5, 0x00000b60, 0x00000c00, 0x00000ca5, 0x00000d4f,
0x00000dfd, 0x00000eb1, 0x00000f69, 0x00001026, 0x000010e8, 0x000011af, 0x0000127b, 0x0000134c,
0x00001423, 0x000014fe, 0x000015df, 0x000016c5, 0x000017b1, 0x000018a1, 0x00001998, 0x00001a93,
0x00001b95, 0x00001c9b, 0x00001da8, 0x00001eba, 0x00001fd2, 0x000020ef, 0x00002213, 0x0000233c,
0x0000246b, 0x000026dc, 0x00002965, 0x00002c06, 0x00002ec1, 0x00003195, 0x00003483, 0x0000378c,
0x00003ab0, 0x00003def, 0x0000414a, 0x000044c0, 0x00004854, 0x00004c06, 0x00004fd4, 0x000053c2,
0x000057cc, 0x00005bf6, 0x00006040, 0x000064ac, 0x00006936, 0x00006de2, 0x000072b0, 0x0000779e,
0x00007cb0, 0x000081e8, 0x00008740, 0x00008cbc, 0x00009260, 0x00009828, 0x00009e14, 0x0000a428,
0x0000aa64, 0x0000b750, 0x0000c4e4, 0x0000d31c, 0x0000e200, 0x0000f198, 0x000101e8, 0x000112f0,
0x000124b8, 0x00013750, 0x00014aa8, 0x00015ed8, 0x000173e0, 0x000189c0, 0x0001a088, 0x0001b838,
0x0001d0d8, 0x0001ea68, 0x00020500, 0x00022090, 0x00023d30, 0x00025ad0, 0x00027990, 0x00029970,
0x0002ba80, 0x0002dcb0, 0x00030010, 0x000324b0, 0x00034a90, 0x000371c0, 0x00039a40, 0x0003c420,
0x0003ef60, 0x00041c00, 0x00044a20, 0x000479c0, 0x0004ab00, 0x0004dda0, 0x00051200, 0x000547e0,
0x00057f80, 0x0005b8e0, 0x0005f3e0, 0x000630c0, 0x00066f80, 0x0006b020, 0x0006f2a0, 0x00073700,
0x00077d80, 0x0007c600, 0x000810c0, 0x00085d80, 0x0008ac80, 0x0008fd80, 0x00095100, 0x0009a700,
0x0009ff00, 0x000a59c0, 0x000ab6c0, 0x000b1680, 0x000b78c0, 0x000bddc0, 0x000c4580, 0x000cb000,
0x000d1d40, 0x000d8d40, 0x000e0080, 0x000e7680, 0x000eefc0, 0x000f6c40, 0x000febc0, 0x00106e80,
0x0010f500, 0x00117e80, 0x00120b80, 0x00129c80, 0x00133080, 0x0013c880, 0x00146480, 0x00150400,
0x0015a780, 0x00164f80, 0x0016fb00, 0x0017ab00, 0x00185f00, 0x00191780, 0x0019d480, 0x001a9600,
0x001b5c80, 0x001c2700, 0x001cf700, 0x001dcb80, 0x001ea500, 0x001f8400, 0x00206800, 0x00215100,
0x00224000, 0x00242e00, 0x00263300, 0x00284f00, 0x002a8500, 0x002cd400, 0x002f3f00, 0x0031c500,
0x00346800, 0x00372900, 0x003a0900, 0x003d0900, 0x00402c00, 0x00437200, 0x0046da00, 0x004a6a00,
0x004e2000, 0x00520000, 0x00560800, 0x005a3c00, 0x005e9e00, 0x00633000, 0x0067f200, 0x006ce600,
0x00721000, 0x00776e00, 0x007d0600, 0x0082dc00, 0x0088e800, 0x008f3800, 0x0095c800, 0x009c9c00,
0x00a3b400, 0x00ab1800, 0x00b2c400, 0x00bac000, 0x00c31000, 0x00cbb000, 0x00d4a800, 0x00ddfc00,
0x00e7ac00, 0x00f1bc00, 0x00fc3400, 0x01071000, 0x01126000, 0x011e1800, 0x012a4000, 0x0136e800,
0x01440800, 0x0151b000, 0x015fd800, 0x016e8800, 0x017dc800, 0x018da000, 0x019e1000, 0x01af1800,
0x01c0d000, 0x01d33000, 0x01e64000, 0x01fa0000, 0x020e9000, 0x0223d000, 0x0239e000, 0x0250d000,
0x02689000, 0x0281a000, 0x029ac000, 0x02b5c000, 0x02d0c000, 0x02edc000, 0x030ac000, 0x0329f000,
0x03493000, 0x036ab000, 0x038c4000, 0x03b05000, 0x03d46000, 0x03fb2000, 0x0421e000, 0x044b8000,
0x04752000, 0x04a1e000, 0x04cea000, 0x04fea000, 0x052ea000, 0x05624000, 0x0595e000, 0x05cd4000,
0x0604c000, 0x06404000, 0x067bc000, 0x06bba000, 0x06fba000, 0x07404000, 0x0784e000, 0x07ce8000,
0x08184000, 0x08674000, 0x08b68000, 0x090b8000, 0x09608000, 0x09bb8000, 0x0a16c000, 0x0a788000,
0x0ada8000, 0x0b438000, 0x0bacc000, 0x0c1d8000, 0x0c8e8000, 0x0d078000, 0x0d80c000, 0x0e02c000,
0x0e850000, 0x0f108000, 0x0f9c4000, 0x10320000, 0x10c80000, 0x11690000, 0x120a0000, 0x12b68000,
0x13638000, 0x141d0000, 0x14d70000, 0x159e0000, 0x16658000, 0x173b8000, 0x18118000, 0x18f70000,
0x19dd0000, 0x1ad40000, 0x1bcb0000, 0x1cd40000, 0x1ddd8000, 0x1efa0000, 0x20170000, 0x21490000,
0x227c0000, 0x23c50000, 0x250e0000, 0x266f0000, 0x27d10000, 0x294c0000, 0x2ac80000, 0x2c600000,
0x2df90000, 0x2fb00000, 0x31670000, 0x333f0000, 0x35180000, 0x37130000, 0x390f0000, 0x3b310000,
0x3d530000, 0x3f9e0000, 0x41ea0000, 0x44620000, 0x46da0000, 0x49820000, 0x4c2c0000, 0x4f080000,
0x51e40000, 0x54f80000, 0x580c0000, 0x5b5c0000, 0x5eae0000, 0x62400000, 0x65d40000, 0x69ac0000,
0x6d840000, 0x71aa0000, 0x75d00000, 0x7a480000, 0x7ec00000, 0x83900000, 0x88640000, 0x8d940000,
0x92c80000, 0x98600000, 0x9dfc0000, 0xa4080000, 0xaa140000, 0xb0980000, 0xb7200000, 0xbe280000,
0xc5340000, 0xccd00000, 0xd46c0000, 0xdca00000, 0xe4d80000, 0xedb80000, 0xf6980000, 0xfff00000,
0x00000000
};

//this table is from color_map_ip solution
//this table is only used for HDR PQ bypass
//pq gamma (matched case)
//gamma(40,0,0,0,1,M_HDRIP_gamma_PQ,513,E_GAMMA_MASERATI_MD,NULL,2);

MS_U32 Maserati_gamma_PQ[513] =
{
0x00000, 0x0000c, 0x00012, 0x00017, 0x0001b, 0x0001f, 0x00022, 0x00025, 0x00028, 0x0002b, 0x0002e, 0x00031, 0x00033, 0x00036, 0x00038, 0x0003b,
0x0003d, 0x0003f, 0x00041, 0x00043, 0x00045, 0x00047, 0x00049, 0x0004b, 0x0004d, 0x0004f, 0x00051, 0x00053, 0x00055, 0x00056, 0x00058, 0x0005a,
0x0005c, 0x0005f, 0x00062, 0x00065, 0x00068, 0x0006b, 0x0006e, 0x00071, 0x00074, 0x00077, 0x0007a, 0x0007d, 0x0007f, 0x00082, 0x00084, 0x00087,
0x0008a, 0x0008f, 0x00093, 0x00098, 0x0009d, 0x000a1, 0x000a6, 0x000aa, 0x000ae, 0x000b3, 0x000b7, 0x000bb, 0x000bf, 0x000c3, 0x000c7, 0x000ca,
0x000ce, 0x000d6, 0x000dd, 0x000e4, 0x000eb, 0x000f1, 0x000f8, 0x000fe, 0x00105, 0x0010b, 0x00111, 0x00117, 0x0011d, 0x00122, 0x00128, 0x0012e,
0x00133, 0x0013e, 0x00148, 0x00153, 0x0015d, 0x00166, 0x00170, 0x00179, 0x00182, 0x0018b, 0x00194, 0x0019d, 0x001a5, 0x001ae, 0x001b6, 0x001be,
0x001c6, 0x001d5, 0x001e5, 0x001f3, 0x00202, 0x00210, 0x0021d, 0x0022b, 0x00238, 0x00245, 0x00251, 0x0025e, 0x0026a, 0x00276, 0x00281, 0x0028d,
0x00298, 0x002ae, 0x002c4, 0x002d9, 0x002ed, 0x00301, 0x00314, 0x00327, 0x00339, 0x0034b, 0x0035d, 0x0036e, 0x0037f, 0x00390, 0x003a0, 0x003b0,
0x003c0, 0x003df, 0x003fd, 0x0041a, 0x00436, 0x00452, 0x0046c, 0x00487, 0x004a0, 0x004b9, 0x004d1, 0x004e9, 0x00501, 0x00518, 0x0052e, 0x00544,
0x0055a, 0x00584, 0x005ad, 0x005d5, 0x005fb, 0x00620, 0x00645, 0x00668, 0x0068b, 0x006ad, 0x006ce, 0x006ee, 0x0070d, 0x0072c, 0x0074b, 0x00768,
0x00785, 0x007be, 0x007f5, 0x0082a, 0x0085d, 0x0088f, 0x008c0, 0x008ef, 0x0091d, 0x0094a, 0x00975, 0x009a0, 0x009ca, 0x009f2, 0x00a1a, 0x00a42,
0x00a68, 0x00ab3, 0x00afb, 0x00b40, 0x00b83, 0x00bc4, 0x00c04, 0x00c41, 0x00c7d, 0x00cb7, 0x00cf0, 0x00d27, 0x00d5d, 0x00d92, 0x00dc5, 0x00df8,
0x00e29, 0x00e89, 0x00ee6, 0x00f3f, 0x00f95, 0x00fe8, 0x01039, 0x01087, 0x010d3, 0x0111d, 0x01165, 0x011ab, 0x011f0, 0x01232, 0x01274, 0x012b4,
0x012f2, 0x0136b, 0x013df, 0x0144f, 0x014bb, 0x01523, 0x01588, 0x015e9, 0x01648, 0x016a4, 0x016fe, 0x01755, 0x017aa, 0x017fc, 0x0184d, 0x0189c,
0x018e9, 0x0197e, 0x01a0d, 0x01a97, 0x01b1b, 0x01b9a, 0x01c16, 0x01c8d, 0x01d01, 0x01d71, 0x01dde, 0x01e47, 0x01eae, 0x01f13, 0x01f74, 0x01fd4,
0x02031, 0x020e5, 0x02191, 0x02237, 0x022d5, 0x0236e, 0x02402, 0x02491, 0x0251b, 0x025a0, 0x02622, 0x026a0, 0x0271a, 0x02791, 0x02805, 0x02876,
0x028e5, 0x029b9, 0x02a85, 0x02b47, 0x02c02, 0x02cb6, 0x02d63, 0x02e0a, 0x02eac, 0x02f48, 0x02fdf, 0x03072, 0x03101, 0x0318b, 0x03212, 0x03295,
0x03315, 0x0340c, 0x034f7, 0x035d8, 0x036af, 0x0377e, 0x03845, 0x03905, 0x039be, 0x03a71, 0x03b1e, 0x03bc6, 0x03c69, 0x03d07, 0x03da0, 0x03e36,
0x03ec7, 0x03fdf, 0x040ea, 0x041e9, 0x042dd, 0x043c6, 0x044a7, 0x0457f, 0x0464f, 0x04718, 0x047db, 0x04897, 0x0494d, 0x049fe, 0x04aaa, 0x04b51,
0x04bf4, 0x04d2c, 0x04e55, 0x04f70, 0x0507f, 0x05182, 0x0527b, 0x0536a, 0x05450, 0x0552e, 0x05604, 0x056d4, 0x0579d, 0x0585f, 0x0591c, 0x059d3,
0x05a86, 0x05bdc, 0x05d21, 0x05e56, 0x05f7d, 0x06098, 0x061a6, 0x062aa, 0x063a5, 0x06495, 0x0657e, 0x0665f, 0x06738, 0x0680a, 0x068d6, 0x0699c,
0x06a5c, 0x06bcd, 0x06d2b, 0x06e77, 0x06fb4, 0x070e3, 0x07205, 0x0731b, 0x07426, 0x07528, 0x07620, 0x0770f, 0x077f6, 0x078d6, 0x079af, 0x07a81,
0x07b4d, 0x07cd5, 0x07e47, 0x07fa7, 0x080f6, 0x08235, 0x08367, 0x0848c, 0x085a6, 0x086b4, 0x087b9, 0x088b4, 0x089a7, 0x08a92, 0x08b75, 0x08c51,
0x08d27, 0x08ebf, 0x09042, 0x091b1, 0x0930e, 0x0945b, 0x09598, 0x096c9, 0x097ed, 0x09905, 0x09a14, 0x09b18, 0x09c13, 0x09d06, 0x09df0, 0x09ed4,
0x09fb0, 0x0a156, 0x0a2e4, 0x0a45d, 0x0a5c4, 0x0a71a, 0x0a860, 0x0a998, 0x0aac3, 0x0abe2, 0x0acf6, 0x0ae00, 0x0af01, 0x0aff9, 0x0b0e8, 0x0b1d0,
0x0b2b1, 0x0b45e, 0x0b5f3, 0x0b772, 0x0b8de, 0x0ba39, 0x0bb83, 0x0bcbf, 0x0bdee, 0x0bf10, 0x0c027, 0x0c134, 0x0c237, 0x0c331, 0x0c422, 0x0c50c,
0x0c5ee, 0x0c6c9, 0x0c79e, 0x0c86c, 0x0c935, 0x0c9f8, 0x0cab6, 0x0cb6f, 0x0cc23, 0x0ccd3, 0x0cd7f, 0x0ce26, 0x0ceca, 0x0cf6a, 0x0d006, 0x0d09f,
0x0d135, 0x0d1c8, 0x0d257, 0x0d2e4, 0x0d36e, 0x0d3f6, 0x0d47b, 0x0d4fd, 0x0d57d, 0x0d5fb, 0x0d676, 0x0d6f0, 0x0d767, 0x0d7dc, 0x0d850, 0x0d8c1,
0x0d931, 0x0da0b, 0x0dadf, 0x0dbac, 0x0dc74, 0x0dd36, 0x0ddf2, 0x0deaa, 0x0df5d, 0x0e00b, 0x0e0b6, 0x0e15c, 0x0e1fe, 0x0e29d, 0x0e338, 0x0e3cf,
0x0e463, 0x0e4f5, 0x0e583, 0x0e60e, 0x0e697, 0x0e71d, 0x0e7a0, 0x0e821, 0x0e89f, 0x0e91b, 0x0e995, 0x0ea0d, 0x0ea83, 0x0eaf7, 0x0eb69, 0x0ebd9,
0x0ec47, 0x0ed1e, 0x0edee, 0x0eeb9, 0x0ef7d, 0x0f03c, 0x0f0f6, 0x0f1aa, 0x0f25a, 0x0f306, 0x0f3ad, 0x0f450, 0x0f4ef, 0x0f58b, 0x0f623, 0x0f6b8,
0x0f749, 0x0f7d8, 0x0f863, 0x0f8eb, 0x0f971, 0x0f9f4, 0x0fa75, 0x0faf3, 0x0fb6f, 0x0fbe8, 0x0fc60, 0x0fcd5, 0x0fd48, 0x0fdb9, 0x0fe29, 0x0fe96,
0x0ff01,
};

//use Config_PQ() to get the entry of degamma fucntion
//this table comes from HLG_EOTF function in Manhattan HDR solution
MS_U32 Maserati_DeGamma_HLG[513] = {
0x00000000, 0x00000000, 0x00000001, 0x00000003, 0x00000005, 0x00000008, 0x0000000c, 0x00000010,
0x00000015, 0x0000001b, 0x00000022, 0x00000029, 0x00000030, 0x00000039, 0x00000042, 0x0000004c,
0x00000056, 0x00000061, 0x0000006d, 0x00000079, 0x00000086, 0x00000094, 0x000000a3, 0x000000b2,
0x000000c1, 0x000000d2, 0x000000e3, 0x000000f5, 0x00000107, 0x0000011a, 0x0000012e, 0x00000143,
0x00000158, 0x00000184, 0x000001b3, 0x000001e5, 0x00000219, 0x00000250, 0x0000028a, 0x000002c7,
0x00000306, 0x00000348, 0x0000038c, 0x000003d3, 0x0000041d, 0x0000046a, 0x000004b9, 0x0000050b,
0x00000560, 0x00000611, 0x000006cd, 0x00000794, 0x00000866, 0x00000942, 0x00000a29, 0x00000b1b,
0x00000c17, 0x00000d1f, 0x00000e31, 0x00000f4e, 0x00001075, 0x000011a7, 0x000012e5, 0x0000142c,
0x0000157f, 0x000016dc, 0x00001844, 0x000019b7, 0x00001b35, 0x00001cbd, 0x00001e50, 0x00001fee,
0x00002196, 0x0000234a, 0x00002508, 0x000026d0, 0x000028a4, 0x00002a82, 0x00002c6b, 0x00002e5f,
0x0000305e, 0x00003267, 0x0000347b, 0x0000369a, 0x000038c3, 0x00003af7, 0x00003d36, 0x00003f80,
0x000041d4, 0x00004434, 0x0000469e, 0x00004912, 0x00004b92, 0x00004e1c, 0x000050b0, 0x00005350,
0x000055fc, 0x00005b70, 0x00006110, 0x000066dc, 0x00006cd2, 0x000072f4, 0x00007940, 0x00007fb6,
0x00008658, 0x00008d24, 0x0000941c, 0x00009b40, 0x0000a290, 0x0000aa08, 0x0000b1ac, 0x0000b97c,
0x0000c174, 0x0000c998, 0x0000d1e8, 0x0000da64, 0x0000e30c, 0x0000ebdc, 0x0000f4d8, 0x0000fe00,
0x00010750, 0x000110d0, 0x00011a78, 0x00012448, 0x00012e48, 0x00013870, 0x000142c0, 0x00014d40,
0x000157e8, 0x00016dc0, 0x00018440, 0x00019b70, 0x0001b348, 0x0001cbc8, 0x0001e4f8, 0x0001fed8,
0x00021960, 0x00023490, 0x00025070, 0x00026d00, 0x00028a30, 0x0002a820, 0x0002c6b0, 0x0002e5f0,
0x000305d0, 0x00032660, 0x000347a0, 0x00036990, 0x00038c30, 0x0003af70, 0x0003d360, 0x0003f800,
0x00041d40, 0x00044320, 0x000469c0, 0x00049120, 0x0004b920, 0x0004e1c0, 0x00050b00, 0x00053500,
0x00055fa0, 0x0005b700, 0x00061100, 0x00066dc0, 0x0006cd20, 0x00072f20, 0x000793e0, 0x0007fb60,
0x00086580, 0x0008d240, 0x000941c0, 0x0009b400, 0x000a28c0, 0x000aa080, 0x000b1ac0, 0x000b97c0,
0x000c1740, 0x000c9980, 0x000d1e80, 0x000da640, 0x000e30c0, 0x000ebdc0, 0x000f4d80, 0x000fe000,
0x00107500, 0x00110c80, 0x0011a700, 0x00124480, 0x0012e480, 0x00138700, 0x00142c00, 0x0014d400,
0x00157e80, 0x0016dc00, 0x00184400, 0x0019b700, 0x001b3480, 0x001cbc80, 0x001e4f80, 0x001fed80,
0x00219600, 0x00234900, 0x00250700, 0x0026d000, 0x0028a300, 0x002a8200, 0x002c6b00, 0x002e5f00,
0x00305d00, 0x00326600, 0x00347a00, 0x00369900, 0x0038c300, 0x003af700, 0x003d3600, 0x003f8000,
0x0041d400, 0x00443200, 0x00469c00, 0x00491200, 0x004b9200, 0x004e1c00, 0x0050b000, 0x00535000,
0x0055fa00, 0x005b7000, 0x00611000, 0x0066dc00, 0x006cd200, 0x0072f200, 0x00793e00, 0x007fb600,
0x00865800, 0x008d2400, 0x00941c00, 0x009b4000, 0x00a28c00, 0x00aa0800, 0x00b1ac00, 0x00b97c00,
0x00c17400, 0x00c99800, 0x00d1e800, 0x00da6400, 0x00e30c00, 0x00ebdc00, 0x00f4d800, 0x00fe0000,
0x01075000, 0x0110c800, 0x011a7000, 0x01244800, 0x012e4800, 0x01387000, 0x0142c000, 0x014d4000,
0x0157e800, 0x0162c000, 0x016dc000, 0x0178e800, 0x01844000, 0x018fc000, 0x019b7000, 0x01a74000,
0x01b34800, 0x01bf7000, 0x01cbc800, 0x01d85000, 0x01e4f800, 0x01f1d800, 0x01fed800, 0x020c0000,
0x02196000, 0x0226e000, 0x02349000, 0x02427000, 0x02507000, 0x025ea000, 0x026d0000, 0x027b8000,
0x028a3000, 0x02991000, 0x02a82000, 0x02b75000, 0x02c6b000, 0x02d63000, 0x02e5f000, 0x02f5c000,
0x0305d000, 0x03160000, 0x03266000, 0x0336f000, 0x0347a000, 0x03588000, 0x03699000, 0x037ad000,
0x038c3000, 0x039db000, 0x03af7000, 0x03c15000, 0x03d36000, 0x03e59000, 0x03f80000, 0x040a8000,
0x041d4000, 0x04302000, 0x04432000, 0x04566000, 0x0469c000, 0x047d6000, 0x04912000, 0x04a50000,
0x04b92000, 0x04cd4000, 0x04e1c000, 0x04f64000, 0x050b0000, 0x051fe000, 0x05350000, 0x054a4000,
0x055fa000, 0x058b0000, 0x05b70000, 0x05e3a000, 0x06110000, 0x063f0000, 0x066dc000, 0x069d0000,
0x06cd2000, 0x06fdc000, 0x072f2000, 0x07614000, 0x0793e000, 0x07c76000, 0x07fb6000, 0x08300000,
0x08658000, 0x089b8000, 0x08d24000, 0x0909c000, 0x0941c000, 0x097a8000, 0x09b40000, 0x09ee0000,
0x0a28c000, 0x0a644000, 0x0aa08000, 0x0add4000, 0x0b1ac000, 0x0b58c000, 0x0b97c000, 0x0bd70000,
0x0c174000, 0x0c580000, 0x0c998000, 0x0cdbc000, 0x0d1e8000, 0x0d620000, 0x0da64000, 0x0deb4000,
0x0e30c000, 0x0e76c000, 0x0ebdc000, 0x0f054000, 0x0f4d8000, 0x0f964000, 0x0fe00000, 0x102a0000,
0x10750000, 0x10c08000, 0x110c8000, 0x11598000, 0x11a70000, 0x11f58000, 0x12448000, 0x12940000,
0x12e48000, 0x13350000, 0x13870000, 0x13d90000, 0x142c0000, 0x147f8000, 0x14d40000, 0x15290000,
0x157f0000, 0x15d68000, 0x16300000, 0x168b8000, 0x16e90000, 0x17488000, 0x17aa0000, 0x180e0000,
0x18740000, 0x18dc8000, 0x19470000, 0x19b40000, 0x1a238000, 0x1a958000, 0x1b0a0000, 0x1b810000,
0x1bfa8000, 0x1c770000, 0x1cf60000, 0x1d780000, 0x1dfc8000, 0x1e848000, 0x1f0f0000, 0x1f9d0000,
0x202e0000, 0x20c20000, 0x21590000, 0x21f40000, 0x22920000, 0x23340000, 0x23da0000, 0x24830000,
0x252f0000, 0x25e00000, 0x26940000, 0x274d0000, 0x280a0000, 0x28ca0000, 0x298f0000, 0x2a590000,
0x2b270000, 0x2bf90000, 0x2cd00000, 0x2dac0000, 0x2e8d0000, 0x2f730000, 0x305e0000, 0x314e0000,
0x32430000, 0x333e0000, 0x343e0000, 0x35440000, 0x36500000, 0x37620000, 0x387a0000, 0x39980000,
0x3abd0000, 0x3be70000, 0x3d190000, 0x3e510000, 0x3f910000, 0x40d60000, 0x42240000, 0x43780000,
0x44d60000, 0x463a0000, 0x47a60000, 0x491a0000, 0x4a960000, 0x4c1c0000, 0x4da80000, 0x4f400000,
0x50de0000, 0x52880000, 0x543a0000, 0x55f40000, 0x57ba0000, 0x598a0000, 0x5b640000, 0x5d480000,
0x5f360000, 0x61300000, 0x63360000, 0x65460000, 0x67620000, 0x698c0000, 0x6bc00000, 0x6e000000,
0x704e0000, 0x72aa0000, 0x75120000, 0x77880000, 0x7a0c0000, 0x7c9e0000, 0x7f400000, 0x81f00000,
0x84ac0000, 0x877c0000, 0x8a5c0000, 0x8d480000, 0x90480000, 0x93580000, 0x967c0000, 0x99b00000,
0x9cf40000, 0xa04c0000, 0xa3b80000, 0xa7380000, 0xaac80000, 0xae700000, 0xb22c0000, 0xb5fc0000,
0xb9e40000, 0xbde00000, 0xc1f40000, 0xc6200000, 0xca600000, 0xcebc0000, 0xd32c0000, 0xd7b80000,
0xdc600000, 0xe1200000, 0xe5fc0000, 0xeaf40000, 0xf0040000, 0xf5340000, 0xfa840000, 0xfff00000,
0x00000000,
};

//use Config_PQ() to get the entry of degamma fucntion
//this table comes from HLG_OETF function in Manhattan HDR solution
MS_U32 Maserati_gamma_HLG[513] =
{
0x00000, 0x00002, 0x00002, 0x00003, 0x00003, 0x00004, 0x00004, 0x00005, 0x00005, 0x00005, 0x00005, 0x00006, 0x00006, 0x00006, 0x00006, 0x00007,
0x00007, 0x00007, 0x00007, 0x00008, 0x00008, 0x00008, 0x00008, 0x00008, 0x00008, 0x00009, 0x00009, 0x00009, 0x00009, 0x00009, 0x00009, 0x0000a,
0x0000a, 0x0000a, 0x0000a, 0x0000b, 0x0000b, 0x0000b, 0x0000b, 0x0000c, 0x0000c, 0x0000c, 0x0000c, 0x0000d, 0x0000d, 0x0000d, 0x0000d, 0x0000e,
0x0000e, 0x0000e, 0x0000f, 0x0000f, 0x0000f, 0x00010, 0x00010, 0x00011, 0x00011, 0x00011, 0x00012, 0x00012, 0x00012, 0x00013, 0x00013, 0x00013,
0x00014, 0x00014, 0x00015, 0x00015, 0x00016, 0x00016, 0x00017, 0x00017, 0x00018, 0x00018, 0x00019, 0x00019, 0x0001a, 0x0001a, 0x0001b, 0x0001b,
0x0001c, 0x0001c, 0x0001d, 0x0001e, 0x0001f, 0x00020, 0x00020, 0x00021, 0x00022, 0x00023, 0x00023, 0x00024, 0x00025, 0x00025, 0x00026, 0x00026,
0x00027, 0x00028, 0x00029, 0x0002b, 0x0002c, 0x0002d, 0x0002e, 0x0002f, 0x00030, 0x00031, 0x00032, 0x00033, 0x00034, 0x00035, 0x00035, 0x00036,
0x00037, 0x00039, 0x0003b, 0x0003c, 0x0003e, 0x0003f, 0x00041, 0x00042, 0x00044, 0x00045, 0x00046, 0x00048, 0x00049, 0x0004a, 0x0004c, 0x0004d,
0x0004e, 0x00050, 0x00053, 0x00055, 0x00057, 0x00059, 0x0005c, 0x0005e, 0x00060, 0x00062, 0x00064, 0x00065, 0x00067, 0x00069, 0x0006b, 0x0006d,
0x0006e, 0x00072, 0x00075, 0x00078, 0x0007b, 0x0007f, 0x00081, 0x00084, 0x00087, 0x0008a, 0x0008d, 0x0008f, 0x00092, 0x00095, 0x00097, 0x0009a,
0x0009c, 0x000a1, 0x000a6, 0x000aa, 0x000af, 0x000b3, 0x000b7, 0x000bb, 0x000bf, 0x000c3, 0x000c7, 0x000cb, 0x000cf, 0x000d2, 0x000d6, 0x000d9,
0x000dd, 0x000e4, 0x000ea, 0x000f1, 0x000f7, 0x000fd, 0x00103, 0x00109, 0x0010f, 0x00114, 0x0011a, 0x0011f, 0x00124, 0x00129, 0x0012e, 0x00133,
0x00138, 0x00142, 0x0014b, 0x00154, 0x0015d, 0x00166, 0x0016e, 0x00176, 0x0017f, 0x00186, 0x0018e, 0x00196, 0x0019d, 0x001a5, 0x001ac, 0x001b3,
0x001ba, 0x001c7, 0x001d5, 0x001e1, 0x001ee, 0x001fa, 0x00206, 0x00212, 0x0021d, 0x00228, 0x00233, 0x0023e, 0x00248, 0x00253, 0x0025d, 0x00267,
0x00271, 0x00284, 0x00297, 0x002a9, 0x002ba, 0x002cc, 0x002dd, 0x002ed, 0x002fd, 0x0030d, 0x0031c, 0x0032c, 0x0033a, 0x00349, 0x00357, 0x00366,
0x00373, 0x0038f, 0x003a9, 0x003c3, 0x003dc, 0x003f4, 0x0040c, 0x00423, 0x0043a, 0x00450, 0x00466, 0x0047c, 0x00491, 0x004a5, 0x004ba, 0x004ce,
0x004e1, 0x00508, 0x0052d, 0x00551, 0x00575, 0x00597, 0x005b9, 0x005da, 0x005fa, 0x0061a, 0x00639, 0x00657, 0x00675, 0x00692, 0x006af, 0x006cb,
0x006e7, 0x0071d, 0x00752, 0x00785, 0x007b7, 0x007e8, 0x00818, 0x00846, 0x00874, 0x008a1, 0x008cc, 0x008f7, 0x00921, 0x0094b, 0x00973, 0x0099b,
0x009c3, 0x00a10, 0x00a5a, 0x00aa3, 0x00aea, 0x00b2f, 0x00b72, 0x00bb4, 0x00bf4, 0x00c33, 0x00c71, 0x00cae, 0x00cea, 0x00d24, 0x00d5e, 0x00d96,
0x00dce, 0x00e3b, 0x00ea4, 0x00f0b, 0x00f6f, 0x00fd0, 0x01030, 0x0108d, 0x010e8, 0x01141, 0x01199, 0x011ef, 0x01243, 0x01296, 0x012e7, 0x01337,
0x01386, 0x0141f, 0x014b5, 0x01546, 0x015d3, 0x0165d, 0x016e4, 0x01768, 0x017e9, 0x01867, 0x018e3, 0x0195c, 0x019d3, 0x01a48, 0x01abb, 0x01b2c,
0x01b9c, 0x01c75, 0x01d48, 0x01e16, 0x01ede, 0x01fa1, 0x02060, 0x0211a, 0x021d0, 0x02283, 0x02331, 0x023dd, 0x02486, 0x0252b, 0x025ce, 0x0266e,
0x0270b, 0x0283f, 0x02969, 0x02a8c, 0x02ba7, 0x02cbb, 0x02dc8, 0x02ed0, 0x02fd1, 0x030ce, 0x031c5, 0x032b8, 0x033a6, 0x03490, 0x03576, 0x03659,
0x03737, 0x038ea, 0x03a91, 0x03c2c, 0x03dbc, 0x03f42, 0x040bf, 0x04234, 0x043a0, 0x04505, 0x04663, 0x047ba, 0x0490b, 0x04a56, 0x04b9c, 0x04cdb,
0x04e16, 0x0507e, 0x052d3, 0x05518, 0x0574e, 0x05976, 0x05b91, 0x05d9f, 0x05fa3, 0x0619c, 0x0638b, 0x06570, 0x0674d, 0x06921, 0x06aed, 0x06cb1,
0x06e6f, 0x071d5, 0x07521, 0x07857, 0x07b77, 0x07e84, 0x08177, 0x0843e, 0x086dc, 0x08956, 0x08baf, 0x08dea, 0x0900b, 0x09214, 0x09406, 0x095e4,
0x097af, 0x09969, 0x09b13, 0x09cad, 0x09e3a, 0x09fba, 0x0a12d, 0x0a295, 0x0a3f2, 0x0a546, 0x0a68f, 0x0a7d0, 0x0a908, 0x0aa37, 0x0ab5f, 0x0ac80,
0x0ad9a, 0x0aead, 0x0afba, 0x0b0c1, 0x0b1c2, 0x0b2bd, 0x0b3b3, 0x0b4a4, 0x0b591, 0x0b678, 0x0b75b, 0x0b83a, 0x0b914, 0x0b9eb, 0x0babd, 0x0bb8c,
0x0bc58, 0x0bde4, 0x0bf63, 0x0c0d6, 0x0c23e, 0x0c39b, 0x0c4ed, 0x0c637, 0x0c777, 0x0c8ae, 0x0c9de, 0x0cb06, 0x0cc26, 0x0cd40, 0x0ce53, 0x0cf5f,
0x0d066, 0x0d167, 0x0d262, 0x0d358, 0x0d449, 0x0d535, 0x0d61c, 0x0d6ff, 0x0d7de, 0x0d8b8, 0x0d98e, 0x0da61, 0x0db30, 0x0dbfb, 0x0dcc2, 0x0dd87,
0x0de48, 0x0dfc1, 0x0e12e, 0x0e290, 0x0e3e7, 0x0e535, 0x0e67a, 0x0e7b5, 0x0e8e9, 0x0ea14, 0x0eb38, 0x0ec55, 0x0ed6b, 0x0ee7b, 0x0ef84, 0x0f088,
0x0f186, 0x0f27e, 0x0f371, 0x0f460, 0x0f549, 0x0f62e, 0x0f70f, 0x0f7eb, 0x0f8c3, 0x0f998, 0x0fa68, 0x0fb35, 0x0fbff, 0x0fcc4, 0x0fd87, 0x0fe47,
0x0ff02,
};



#if RealChip

extern MS_U8 u8DoHDRbypassInHDRIP_Flag;

extern StuDlc_HDRinit g_HDRinitParameters;
extern StuDlc_FinetuneParamaters g_DlcParameters;

extern MS_U8 u8Mhal_xc_TMO_mode;

#endif

#if (1 == CFD_debug)
#else
#define printf
#endif


#define FunctionMode(Mode) ((Mode) & 0x3F)
#define  HDR_DBG_HAL_CFD(x)  x
#define function1(var1,var2) var1 = (var1&MaskForMode_HB) | (var2&MaskForMode_LB);
#define WriteRegsiterFlag(Mode) (Mode & 0x40) >>6

#if (NowHW == Kastor)

//for K6
const MS_U32 OSD_ITMO_TCH[65] = {0x0000,0x04f9,0x058c,0x05ea,0x0630,0x0669,0x0699,0x06c2,0x06e7,0x0708,0x0726,0x0742,0x075c,0x0774,0x078a
,0x079f,0x07b3,0x07c6,0x07d8,0x07e9,0x07fa,0x080a,0x0819,0x0827,0x0835,0x0843,0x0850,0x085d,0x0869,0x0875,0x0881,0x088c,0x0897,0x08a2,0x08ac
,0x08b6,0x08c1,0x08ce,0x08dd,0x08ef,0x0903,0x091b,0x0935,0x0952,0x0972,0x0996,0x09bd,0x09e8,0x0a17,0x0a4a,0x0a81,0x0abe,0x0aff,0x0b45,0x0b91
,0x0be3,0x0c3b,0x0c9a,0x0d01,0x0d6f,0x0de6,0x0e67,0x0ef1,0x0f86,0x0ffc};

const MS_U32 OSD_GAMMA_SP_TCH[32] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1};

const MS_U32 OSD_GAMMA_EIDB_TCH[32] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xa,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2};

const MS_U32 OSD_GAMMA_OFFSET_TCH[32] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x8,0x10,0x18,0x20,0x28,0x30,0x38,0x40,0x48,0x50,0x58,0x60,0x68,0x70,0x78};

const MS_U32 OSD_GAMMA_TCH[129] = {0x0,0x8,0xc,0xf,0x11,0x13,0x15,0x17,0x18,0x1a,0x1b,0x1c,0x1e,0x1f,0x20,0x21,0x22,0x24,0x26,0x27,0x29,0x2b,0x2c,0x2d,0x2f,0x31,0x34,0x36
    ,0x38,0x3a,0x3c,0x3d,0x3f,0x42,0x45,0x48,0x4b,0x4d,0x50,0x52,0x54,0x58,0x5c,0x5f,0x62,0x66,0x68,0x6b,0x6e,0x73,0x77,0x7b,0x7f,0x83,0x87,0x8a,0x8d,0x93,0x98,0x9d,0xa2,0xa6
    ,0xaa,0xae,0xb2,0xb9,0xbf,0xc5,0xca,0xd0,0xd4,0xd9,0xdd,0xe5,0xec,0xf3,0xf9,0xff,0x104,0x109,0x10e,0x117,0x11f,0x127,0x12e,0x134,0x13a,0x140,0x145,0x14f,0x158,0x160,0x168
    ,0x16f,0x175,0x17c,0x181,0x18c,0x196,0x19f,0x1a7,0x1af,0x1b6,0x1bc,0x1c3,0x1ce,0x1d9,0x1e2,0x1eb,0x1f3,0x1fa,0x201,0x208,0x214,0x21f,0x229,0x232,0x23b,0x242,0x24a,0x251
    ,0x25d,0x269,0x273,0x27c,0x285,0x28d,0x295,0x8,};

const MS_U32 OSD_GAMMA_1000[129] ={0x0,0x11,0x18,0x1d,0x21,0x25,0x28,0x2b,0x24,0x26,0x28,0x2a,0x2b,0x2d,0x2e,0x30,0x31,0x33,0x36,0x38,0x3a,0x3c,0x3e,0x3f,0x41,0x44,0x47
                                                        ,0x4a,0x4c,0x4f,0x51,0x53,0x55,0x59,0x5d,0x60,0x63,0x66,0x69,0x6b,0x6e,0x72,0x76,0x7a,0x7e,0x81,0x85,0x88,0x8b,0x90,0x95,0x9a,0x9e,0xa2
                                                        ,0xa6,0xa9,0xac,0xb3,0xb8,0xbe,0xc2,0xc7,0xcb,0xcf,0xd3,0xda,0xe0,0xe6,0xec,0xf1,0xf6,0xfa,0xfe,0x106,0x10d,0x114,0x11a,0x11f,0x124,0x129
                                                        ,0x12e,0x136,0x13e,0x145,0x14c,0x152,0x157,0x15c,0x161,0x16b,0x173,0x17a,0x181,0x188,0x18e,0x193,0x198,0x1a2,0x1ab,0x1b2,0x1ba,0x1c0,0x1c7
                                                        ,0x1cc,0x1d2,0x1dc,0x1e5,0x1ed,0x1f5,0x1fc,0x202,0x208,0x20e,0x218,0x221,0x229,0x231,0x238,0x23f,0x245,0x24a,0x255,0x25e,0x267,0x26e,0x276,0x27c,0x282,0x1c};

const MS_U32 OSD_GAMMA_4000[129] ={0x0,0x1b,0x26,0x2d,0x34,0x39,0x3d,0x42,0x3f,0x42,0x45,0x47,0x4a,0x4c,0x4e,0x51,0x53,0x57,0x5a,0x5d,0x61,0x64,0x66,0x69,0x6b,0x70,0x75,0x79,0x7c
    ,0x80,0x83,0x86,0x89,0x8f,0x94,0x99,0x9d,0xa1,0xa5,0xa9,0xac,0xb3,0xb9,0xbe,0xc3,0xc8,0xcd,0xd1,0xd5,0xdc,0xe3,0xe9,0xef,0xf5,0xfa,0xfe,0x103,0x10b,0x113,0x11a,0x120,0x126,0x12c,0x131
    ,0x136,0x13f,0x148,0x14f,0x156,0x15d,0x163,0x169,0x16e,0x178,0x181,0x189,0x191,0x198,0x19e,0x1a4,0x1aa,0x1b5,0x1be,0x1c7,0x1cf,0x1d6,0x1dd,0x1e4,0x1ea,0x1f5,0x1ff,0x208,0x210,0x218
    ,0x21f,0x226,0x22c,0x238,0x242,0x24b,0x254,0x25c,0x263,0x26a,0x270,0x27c,0x287,0x290,0x299,0x2a1,0x2a9,0x2af,0x2b6,0x2c2,0x2cc,0x2d6,0x2df,0x2e7,0x2ee,0x2f5,0x2fc,0x308,0x312,0x31c,0x324
    ,0x32c,0x334,0x33b,0x60};


const MS_U32 OSD_ITMO_100_linear[65] = {0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029
    ,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029
    ,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029,0x0029};

const MS_U32 OSD_GAMMA_100_GAMMA[129] = {0x0,0x14,0x1c,0x23,0x28,0x2d,0x31,0x35,0x39,0x3c,0x3f,0x42,0x45,0x48,0x4b,0x4d,0x50,0x55,0x59,0x5e,0x62,0x66,0x6a,0x6d,0x71,0x78,0x7e,0x85,0x8a,0x90
    ,0x96,0x9b,0xa0,0xaa,0xb3,0xbb,0xc4,0xcc,0xd3,0xdb,0xe2,0xf0,0xfd,0x109,0x115,0x120,0x12b,0x136,0x140,0x153,0x165,0x177,0x188,0x198,0x1a7,0x1b6,0x1c4,0x1e0,0x1f9,0x212,0x22a,0x240,0x256,0x26b,0x27f
    ,0x2a6,0x2cb,0x2ee,0x30f,0x32f,0x34e,0x36c,0x388,0x3bf,0x3f3,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff
    ,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x0};



//GOP HDR/SDR data
const MS_U16 u16Mux_PipeOffset[OP_MUX_MAX] = {0x0, 0x4, 0x8, 0xC, 0x10};
const MS_BOOL bGOPVscalePipeDelay[GOP_MAX] = {FALSE, FALSE, FALSE, TRUE, FALSE, FALSE};

static MS_BOOL bHDR_LAST_STATUS=FALSE;
static MS_BOOL bGOP_OSDB[GOP_MAX];
static MS_U16 u16GOP_OPMux[GOP_MAX];
static MS_U8 u8GOP_HDR_OPMux_GOP=0;

void writeGamma_Kastor_OSD(U32* u32Lut,MS_U8 debugMode,MS_U8 transferCharacterstics)
{

    //return 0;

    U32 u16Idx;
    MS_U16 SramData;
    MS_U16 u16tmp=0;
    MS_U16 u16cnt=0;


    #ifdef AUTODOWNLOAD
    U16* u16LutData = NULL;
    #endif

    #if RealChip
    #else
    FILE *f;
    if(2 == debugMode)
    {
        f = fopen("GammaMD.txt", "w");
        if (f == NULL)
        {
            printk("Error opening GammaMD output file!\n");

        }
    }
    #endif


#ifdef CMODEL
    fprintf(fi,"wriu -b 0x100A80 0x1 0\n");
    fprintf(fi,"wriu -b 0x100A82 0x1 0\n");
    fprintf(fi,"wriu -b 0x100A84 0x1 0\n");
#else
    //writeByteMask(0x100A80, 0x1,0);
    //writeByteMask(0x100A82, 0x1,0);
    //writeByteMask(0x100A84, 0x1,0);
    MApi_GFLIP_XC_W2BYTE(0x137905,0x0000);//SRAM power
    MApi_GFLIP_XC_W2BYTE(0x137904,0x0000);//SRAM power
#endif//CMODEL

#if 0
//#ifdef AUTODOWNLOAD
    printk("AUTODOWNLOAD AUTODOWNLOAD AUTODOWNLOAD!\n");
    u16LutData = (U16*) kmalloc( 128 * sizeof(U16), GFP_KERNEL);
    for(u16Idx = 0; u16Idx<128;u16Idx++)
    {
        u16LutData[u16Idx] = min( u32Lut[u16Idx],0xFFFF);
    }

    ST_KDRV_XC_AUTODOWNLOAD_FORMAT_INFO stAutoDownloadFormat;
    stAutoDownloadFormat.enSubClient = E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_GAMMA;
    stAutoDownloadFormat.bEnableRange = 1;
    stAutoDownloadFormat.u16StartAddr = 0;
    stAutoDownloadFormat.u16EndAddr = 127;
    writeByteMask(DoVi_Addr_H(0x32, 0x70),0x80, 0x80); // AutoDownload on
    if(1 == MHal_XC_WriteAutoDownload(E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR, (MS_U8*)&(u16LutData[0]),128 * sizeof(MS_U16), (void*)&stAutoDownloadFormat) )
    {
        //if(1 == MHal_XC_FireAutoDownload(E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR))            //Fire at TMO, once is enough
        //     printk("M+D Gamma AutoDownload successful!\n");
        //else
        //  printk("M+D Gamma AutoDownload FAILED!\n");
    }
    else
    {
        printk("Write Gamma AutoDownload FAILED!\n");
    }
    kfree(u16LutData);
#else //RIU


#if 0
    writeByteMask(_PK_H_(0x32, 0x70),0x80, 0, E_OP); // AutoDownload off
    writeByteMask(_PK_L_(0x32, 0x60),  0x3, 0x1, E_OP); // lut_sel Gamma
    //writeByteMask(_PK_L_(0x32, 0x68),  0x0F, 0x0, E_OP); // lut_sel_ip
    //writeByteMask(_PK_H_(0x32, 0x60), 0x10, 0x0, E_OP); // lut_fast_md off
    writeByteMask(_PK_H_(0x32, 0x60), 0x10, 0x10, E_OP); // lut_fast_md on
    writeByteMask(_PK_H_(0x32, 0x60), 0x20, 0x20, E_OP); // lut_wd_dup_md
    writeByteMask(_PK_H_(0x32, 0x60), 0x80, 0x80, E_OP); // lut_load_en
    write2Bytes(_PK_L_(0x32, 0x69), 0, E_OP); // lut_addr base

#else

MApi_GFLIP_XC_W2BYTE(_PK_L_(0x32, 0x60),0xb001);//
MApi_GFLIP_XC_W2BYTE(_PK_L_(0x32, 0x69),0);//lut_addr base

#endif


    //write2Bytes(DoVi_Addr_L(0x7B, 0x69), 0); // lut_init_addr
    //MS_U32* pu32Lut = (MS_U32*) pstDataInfo->pu8Data;

    for (u16Idx  = 0; u16Idx < 128; u16Idx++)
    {
        SramData = min( u32Lut[u16Idx],0x3FF);
        if(2 == debugMode)
        {
                    #if RealChip
                    #else
            fprintf(f,"%d: %0#x\n", u16Idx,SramData);
                    #endif
        }


        // write2Bytes(_PK_L_(0x32, 0x61), u16Idx, E_OP); // lut_addr

#if 0
        write2Bytes(_PK_L_(0x32, 0x66), SramData, E_OP); // wd2
        write2Bytes(_PK_L_(0x32, 0x64), SramData, E_OP); // wd1
        write2Bytes(_PK_L_(0x32, 0x62), SramData, E_OP); // wd0
#else
    MApi_GFLIP_XC_W2BYTE(_PK_L_(0x32, 0x66),SramData);//wd2
    MApi_GFLIP_XC_W2BYTE(_PK_L_(0x32, 0x64),SramData);//wd1
    MApi_GFLIP_XC_W2BYTE(_PK_L_(0x32, 0x62),SramData);//wd0
#endif
        //write2Bytes(_PK_L_(0x32, 0x67), 0, E_OP); // wd2
        //write2Bytes(_PK_L_(0x32, 0x65), 0, E_OP); // wd1
        //write2Bytes(_PK_L_(0x32, 0x63), 0, E_OP); // wd0
        //writeByteMask(_PK_L_(0x32, 0x60), 0x10, 0x10, E_OP); // lut_write_pulse

#ifdef CMODEL
        //fprintf(fi,"wait 10\n") ;
#else
#if 0
        while  (MApi_GFLIP_XC_R2BYTEMSK( DoVi_Addr_L(0x7A, 0x60) , 0x0100 ) != 0x0100)
        {
            printk("wait a moment...\n");

        };//
#endif
#endif

#if 1
        u16cnt=0;
        while(1)
        {
            //u16tmp = MApi_GFLIP_XC_R2BYTEMSK(REG_SC_YAHAN_BK32_60_L, BIT8);
         u16tmp =   MApi_GFLIP_XC_R2BYTE(REG_SC_YAHAN_BK32_60_L);
           // if(u16tmp)
        if(u16tmp==0xb101)
            {
                //writeByteMask(_PK_H_(0x32, 0x60), 0x04, 0x00, E_OP); // lut_flag_clr
                //writeByteMask(_PK_H_(0x32, 0x60), 0x02, 0x00, E_OP); // clear write flag

         //MApi_GFLIP_XC_W2BYTE(_PK_L_(0x32, 0x60),0xb001);
          MApi_GFLIP_XC_W2BYTE(0x133260,0xb001);//
                break;
    }
            u16cnt++;
            if(u16cnt>20)
            {
                printk("\n[CFD][%s,%d]TIMEOUT,BREAK;u16Idx=%d,u16tmp=%d,u16cnt=0x%x !!!\n",__FUNCTION__,__LINE__,u16Idx,u16tmp,u16cnt);
                break;
    }
        }
#endif
        //MApi_GFLIP_XC_W2BYTE(_PK_L_(0x32, 0x60),0xB001);//

    }
    //writeByteMask(_PK_H_(0x32, 0x60), 0x80, 0x00, E_OP); // lut_load_en
    //MApi_GFLIP_XC_W2BYTE(0x133260,0x0);
    MApi_GFLIP_XC_W2BYTE(_PK_L_(0x32, 0x60),0x0);
#endif // autodownload or RIU

#ifdef AUTODOWNLOAD
        kfree(u16LutData);
#endif
    MS_U32 u32LastEntry = u32Lut[128];// << 3;

    if(2 == debugMode)
    {
#if RealChip
#else


        fprintf(f,"%d\n",u32Lut[128]);
#endif
    }
    if(1 == debugMode)
    {
        printf("%d: %0#x\n", 128,u32Lut[128]);
    }

    write4Bytes(_PK_L_(0x32, 0x04), u32LastEntry, E_OP);
    write4Bytes(_PK_L_(0x32, 0x08), u32LastEntry, E_OP);
    write4Bytes(_PK_L_(0x32, 0x0c), u32LastEntry, E_OP);

}

MS_BOOL _GetBnkOfstByGop(MS_U8 gop, MS_U32 *pBnkOfst)
{
    if(gop >= GOP_MAX)
    {
        printk("[%s][%d] Out of GOP support!!! GOP=%d\n",__FUNCTION__,__LINE__ ,gop);
        return FALSE;
    }
    if (gop==GOP_0)
        *pBnkOfst = 0x0;
    else if (gop==GOP_1)
        *pBnkOfst = 0x300;
    else if (gop==GOP_2)
        *pBnkOfst = 0x600;
    else if (gop==GOP_3)
        *pBnkOfst = 0x900;
    else if (gop==GOP_4)
        *pBnkOfst = 0x1500;
    else
        return FALSE;

    return TRUE;
}

void OSDB_BLENING_ENABLE(MS_U16 u16OP_Mux, MS_BOOL bEnable)
{
    switch(u16OP_Mux)
    {
        case OP_MUX_0:
        {
            MApi_GFLIP_XC_W2BYTEMSK(GOP_MUX0_BLENDING_REG, bEnable?GOP_MUX0_OSDB_MASK:0x0, GOP_MUX0_OSDB_MASK);
            break;
        }
        case OP_MUX_1:
        {
            MApi_GFLIP_XC_W2BYTEMSK(GOP_MUX0_BLENDING_REG, bEnable?GOP_MUX1_OSDB_MASK:0x0, GOP_MUX1_OSDB_MASK);
            break;
        }
        case OP_MUX_2:
        {
            MApi_GFLIP_XC_W2BYTEMSK(GOP_MUX2_BLENDING_REG, bEnable?GOP_MUX2_OSDB_MASK:0x0, GOP_MUX2_OSDB_MASK);
            break;
        }
        case OP_MUX_3:
        {
            MApi_GFLIP_XC_W2BYTEMSK(GOP_MUX2_BLENDING_REG, bEnable?GOP_MUX3_OSDB_MASK:0x0, GOP_MUX3_OSDB_MASK);
            break;
        }
        case OP_MUX_4:
        {
            MApi_GFLIP_XC_W2BYTEMSK(GOP_MUX4_BLENDING_REG, bEnable?GOP_MUX4_OSDB_MASK:0x0, GOP_MUX4_OSDB_MASK);
            break;
        }
        default:
        {
            printk("[%s][%d] Out of OP_MUX_MAX support!!! GOP=%d\n",__FUNCTION__,__LINE__ ,u16OP_Mux);
            break;
        }

    }
}

MS_U16 MS_Cfd_Kastor_OSD_process(STU_CFDAPI_Kastor_OSDIP  *pstu_Kastor_OSDIP_Param, STU_CFDAPI_OSD_PROCESS_Input  *pstu_Cfd_OSD_Process_Configs)
{
    MS_U8 u8index=0;
    MS_U16 u16GOPdst=0;
    MS_U16 u16GOP_OPMUX_tmp=0;
    MS_U16 u16GOP_OPMux_OSDB_tmp=0;
    MS_U16 u16GOPMixerPortEna=0;
    MS_U32 u32BankOffSet=0;
    MS_S32 u16pipe=0;

#if 1
//if (msReadByte(REG_SC_BK79_7C_L) == 0)
if (1 == pstu_Kastor_OSDIP_Param->u8Status)
{

//printk("\n  [CFD][ %s  , %d ]  do OSD SDR2HDR  !!!\n",__FUNCTION__,__LINE__);

//MApi_GFLIP_XC_W2BYTE(0x100a4a,0x0010);//clock
MApi_GFLIP_XC_W2BYTEMSK(0x100a4a, 0x0000, 0x000f ); //clock on

//OETF case : 1000/4000/400 (Technicolor)
if (0 == pstu_Kastor_OSDIP_Param->u8OSD_Gamma_SRAM_Mode)  //to PQ 400nits tch //default
{
    //if (msReadByte(REG_SC_BK79_7D_L)!=0x0001)
    {
    writeGamma_Kastor_OSD(OSD_GAMMA_TCH, 0, 1);
    //MApi_GFLIP_XC_W2BYTE(REG_SC_BK79_7D_L,0x0001);
    }
}
//else if(msReadByte(REG_SC_BK79_7C_H)==0x01) //to PQ 1000nits
else if(1 == pstu_Kastor_OSDIP_Param->u8OSD_Gamma_SRAM_Mode) //to PQ 1000nits
{
    //if (msReadByte(REG_SC_BK79_7D_L)!=0x0002)
    {
    writeGamma_Kastor_OSD(OSD_GAMMA_1000, 0, 1);
    //MApi_GFLIP_XC_W2BYTE(REG_SC_BK79_7D_L,0x0002);
    }
}
//else if(msReadByte(REG_SC_BK79_7C_H)==0x02)//to PQ 4000nits
else
{
    //if (msReadByte(REG_SC_BK79_7D_L)!=0x0003)
    {
    writeGamma_Kastor_OSD(OSD_GAMMA_4000, 0, 1);
    //MApi_GFLIP_XC_W2BYTE(REG_SC_BK79_7D_L,0x0003);
    }
}


if(pstu_Cfd_OSD_Process_Configs->u8OSD_IsFullRange == 0)
{
    //printk("[OSD HDR] fullR to LimitY\n");
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_6e_L,0xE010);//out_r2y (fullR to limitY)
}
else if (pstu_Cfd_OSD_Process_Configs->u8OSD_IsFullRange == 1)
{
    //printk("[OSD HDR] fullR to fullY\n");
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_6e_L,0x6010);//out_r2y (fullR to fullY)
}
else
{
    //printk("[OSD HDR] fullR to fullY(default)\n");
       MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_6e_L,0x6010);//out_r2y (fullR to fullY)
}


if(pstu_Cfd_OSD_Process_Configs->u8OSD_colorprimary == 1) //r2y 709
{
//printk("[OSD HDR] r2y 709\n");
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_04_L,0x367);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_05_L,0xb71);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_06_L,0x128);
}
else if(pstu_Cfd_OSD_Process_Configs->u8OSD_colorprimary == 9)  //r2y 2020
{
//printk("[OSD HDR] r2y 2020\n");
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_04_L,0x434);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_05_L,0xAD9);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_06_L,0x0F3);
}
else //default : r2y 709
{
//printk("[OSD HDR] default : r2y 709\n");
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_04_L,0x367);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_05_L,0xb71);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_06_L,0x128);
}


if(pstu_Cfd_OSD_Process_Configs->u8OSD_colorprimary == 1 && pstu_Cfd_OSD_Process_Configs->u8Video_colorprimary == 9) //r2r 709->2020
{
//printk("[OSD HDR] r2r 709->2020\n");

MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_07_L,0xa0a);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_08_L,0x545);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_09_L,0xb1);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0a_L,0x11b);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0b_L,0xeb6);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0c_L,0x2f);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0d_L,0x43);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0e_L,0x169);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0f_L,0xe54);
}
else if (pstu_Cfd_OSD_Process_Configs->u8OSD_colorprimary == 9 && pstu_Cfd_OSD_Process_Configs->u8Video_colorprimary == 1) //r2r 2020->709
{
//printk("[OSD HDR] r2r 2020->709\n");

    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_07_L,0x1A91);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_08_L,-0x967);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_09_L, -0x12A);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0a_L,-0x1FE);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0b_L,0x1220);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0c_L, -0x022);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0d_L,-0x04A);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0e_L,-0x19C);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0f_L,0x11E6);
}
else //r2r identity
{
    //printk("[OSD HDR] r2r 709->709 or 2020->2020\n");

    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_07_L,0x1000);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_08_L,0x0);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_09_L,0x0 );
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0a_L,0x0);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0b_L,0x1000);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0c_L, 0x0);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0d_L,0x0);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0e_L,0x0);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0f_L,0x1000);
}


//tmo case : linear/ technicolor
MS_U32 tmo_reg;
MS_U8 idx =0;


if (0 == pstu_Kastor_OSDIP_Param->u8OSD_Plut_Mode) //linear
{
    //MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_18_L,0x8000);
    for( idx =0; idx < 65 ; idx++)
    {
        tmo_reg = (0x130000 | ((MS_U16)(0x31) << 8) | (MS_U16)((0x20+idx)*2));
        MApi_GFLIP_XC_W2BYTE(tmo_reg,4095);
    }
   // MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_18_L,0x8001);
}
else //Technicolor
{

    //MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_18_L,0x8000);
    for( idx =0; idx < 65 ; idx++)
    {
        tmo_reg = (0x130000 | ((MS_U16)(0x31) << 8) | (MS_U16)((0x20+idx)*2));
        MApi_GFLIP_XC_W2BYTE(tmo_reg,OSD_ITMO_TCH[idx]);
    }
    //MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_18_L,0x8001);
}

//OETF
MS_U32 gamma_reg;
MS_U8 idx_gamma = 0;
MS_U32 gamma_sp = 0;

for( idx_gamma = 0; idx_gamma < 32 ; idx_gamma += 2)
{
gamma_reg = (0x130000 | ((MS_U16)(0x32) << 8) | (MS_U16)((0x10+idx_gamma/2)*2));
MApi_GFLIP_XC_W2BYTE(gamma_reg,OSD_GAMMA_EIDB_TCH[idx_gamma] + (OSD_GAMMA_EIDB_TCH[idx_gamma+1]<<8));
}

for( idx_gamma =0; idx_gamma < 32 ; idx_gamma++)
{
gamma_reg = (0x130000 | ((MS_U16)(0x32) << 8) | (MS_U16)((0x20+idx_gamma)*2));
MApi_GFLIP_XC_W2BYTE(gamma_reg,OSD_GAMMA_OFFSET_TCH[idx_gamma] );
}

for( idx_gamma =0; idx_gamma < 32 ; idx_gamma++)
{
gamma_sp += (OSD_GAMMA_SP_TCH[idx_gamma] << idx_gamma);
}

MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK32_02_L,gamma_sp&0x00ff);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK32_03_L,(gamma_sp>>16));

//r2y 2020
if(pstu_Cfd_OSD_Process_Configs->u8OSD_IsFullRange == 1)
{

if(pstu_Cfd_OSD_Process_Configs->u8Video_colorprimary == 9)//r2y full 2020
{
    //printk("[OSD HDR] r2y full 2020\n");

MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_6f_L,0x0200);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_70_L,0x1E29);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_71_L,0x1FD7);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_72_L,0x010D);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_73_L,0x02B6);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_74_L,0x003D);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_75_L,0x1F71);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_76_L,0x1E8F);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_77_L,0x0200);
}
else if(pstu_Cfd_OSD_Process_Configs->u8Video_colorprimary == 1)//r2y full 709
{
    //printk("[OSD HDR] r2y full 709\n");

MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_6f_L,0x0200);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_70_L,0x1E2F);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_71_L,0x1FD1);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_72_L,0x00DA);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_73_L,0x02DC);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_74_L,0x004A);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_75_L,0x1F8B);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_76_L,0x1E75);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_77_L,0x0200);
}
else //r2y full 2020
{
    //printk("[OSD HDR] r2y full 2020\n");
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_6f_L,0x0200);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_70_L,0x1E29);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_71_L,0x1FD7);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_72_L,0x010D);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_73_L,0x02B6);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_74_L,0x003D);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_75_L,0x1F71);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_76_L,0x1E8F);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_77_L,0x0200);

}
}
else if (pstu_Cfd_OSD_Process_Configs->u8OSD_IsFullRange == 0)
{
    if(pstu_Cfd_OSD_Process_Configs->u8Video_colorprimary == 9)//r2y limit 2020
    {
    //printk("[OSD HDR] r2y limit 2020\n");
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_6f_L,0x01C0);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_70_L,0x1E64);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_71_L,0x1FDC);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_72_L,0x00E6);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_73_L,0x0253);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_74_L,0x0034);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_75_L,0x1F83);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_76_L,0x1EBD);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_77_L,0x01C0);
       }
       else if(pstu_Cfd_OSD_Process_Configs->u8Video_colorprimary == 1)//r2y limit 709
       {
       //printk("[OSD HDR] r2y limit 709\n");
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_6f_L,0x01C0);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_70_L,0x1E69);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_71_L,0x1FD7);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_72_L,0x00BA);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_73_L,0x0273);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_74_L,0x003F);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_75_L,0x1F99);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_76_L,0x1EA6);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_77_L,0x01C0);
    }
    else //r2y limit 709
     {
     //printk("[OSD HDR] r2y limit 2020\n");
     MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_6f_L,0x01C0);
     MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_70_L,0x1E64);
     MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_71_L,0x1FDC);
     MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_72_L,0x00E6);
     MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_73_L,0x0253);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_74_L,0x0034);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_75_L,0x1F83);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_76_L,0x1EBD);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_77_L,0x01C0);
    }
    }
    else//r2y full 2020
    {
        //printk("[OSD HDR] r2y full 2020\n");
        MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_6f_L,0x0200);
        MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_70_L,0x1E29);
        MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_71_L,0x1FD7);
        MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_72_L,0x010D);
        MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_73_L,0x02B6);
        MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_74_L,0x003D);
        MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_75_L,0x1F71);
        MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_76_L,0x1E8F);
        MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_77_L,0x0200);

    }

/////////////////////////////////////////////////
//////////////GOP HDR setting START//////////////
/////////////////////////////////////////////////
if(bHDR_LAST_STATUS==FALSE)
{
    u16GOP_OPMUX_tmp=MApi_GFLIP_XC_R2BYTE(GOP_OPMUX_REG);
    for(u8index=0;u8index<GOP_MAX;u8index++)
    {
        u16GOPMixerPortEna =0;
        _GetBnkOfstByGop(u8index, &u32BankOffSet);
        u16GOPdst = MApi_GFLIP_XC_R2BYTEMSK( u32BankOffSet+ REG_GOP_DST , GOP_DST_MASK ) ;
        if(u16GOPdst==GOP_DST_OP)
        {
            //HDR need RGB domain
            MApi_GFLIP_XC_W2BYTEMSK(u32BankOffSet+REG_GOP_CTRL0, 0x0, GOP_COLOR);

            if(u8index == (u16GOP_OPMUX_tmp&GOP_OP_MUX0))
                u16GOP_OPMux[u8index] = 0;
            else if(u8index == ((u16GOP_OPMUX_tmp&GOP_OP_MUX1)>>GOP_OP_MUX1_SHIFT))
                u16GOP_OPMux[u8index] = 1;
            else if(u8index == ((u16GOP_OPMUX_tmp&GOP_OP_MUX2)>>GOP_OP_MUX2_SHIFT))
                u16GOP_OPMux[u8index] = 2;
            else if(u8index == ((u16GOP_OPMUX_tmp&GOP_OP_MUX3)>>GOP_OP_MUX3_SHIFT))
                u16GOP_OPMux[u8index] = 3;
            else if(u8index == ((u16GOP_OPMUX_tmp&GOP_OP_MUX4)>>GOP_OP_MUX4_SHIFT))
                u16GOP_OPMux[u8index] = 4;

            //SDR Blending disable
            switch(u16GOP_OPMux[u8index])
            {
                case 0:
                {
                    u16GOP_OPMux_OSDB_tmp=MApi_GFLIP_XC_R2BYTEMSK(GOP_MUX0_BLENDING_REG, 0xffff);
                    if(u16GOP_OPMux_OSDB_tmp&GOP_MUX0_OSDB_MASK)
                        bGOP_OSDB[u8index]=TRUE;
                    break;
                }
                case 1:
                {
                    u16GOP_OPMux_OSDB_tmp=MApi_GFLIP_XC_R2BYTEMSK(GOP_MUX0_BLENDING_REG, 0xffff);
                    if(u16GOP_OPMux_OSDB_tmp&GOP_MUX1_OSDB_MASK)
                        bGOP_OSDB[u8index]=TRUE;
                    break;
                }
                case 2:
                {
                    u16GOP_OPMux_OSDB_tmp=MApi_GFLIP_XC_R2BYTEMSK(GOP_MUX2_BLENDING_REG, 0xffff);
                    if(u16GOP_OPMux_OSDB_tmp&GOP_MUX2_OSDB_MASK)
                        bGOP_OSDB[u8index]=TRUE;
                    break;
                }
                case 3:
                {
                    u16GOP_OPMux_OSDB_tmp=MApi_GFLIP_XC_R2BYTEMSK(GOP_MUX2_BLENDING_REG, 0xffff);
                    if(u16GOP_OPMux_OSDB_tmp&GOP_MUX3_OSDB_MASK)
                        bGOP_OSDB[u8index]=TRUE;
                    break;
                }
                case 4:
                {
                    u16GOP_OPMux_OSDB_tmp=MApi_GFLIP_XC_R2BYTEMSK(GOP_MUX4_BLENDING_REG, 0xffff);
                    if(u16GOP_OPMux_OSDB_tmp&GOP_MUX4_OSDB_MASK)
                        bGOP_OSDB[u8index]=TRUE;
                    break;
                }
            }
        }
    }

    // Enable Mixer For HDR
    u16GOPMixerPortEna = 0;
    for(u8index=0;u8index<GOP_MAX;u8index++)
    {
        if(bGOP_OSDB[u8index]==TRUE)
        {
            u16GOPMixerPortEna |= 1 << u8index;
        }
    }

    //Save HDR Mux GOP information
    u8GOP_HDR_OPMux_GOP = (u16GOP_OPMUX_tmp&HDR_MUX_MASK)>>HDR_MUX_SHIFT;

    //Set one OP mux for Mixer HDR
    u16GOP_OPMUX_tmp=MApi_GFLIP_XC_R2BYTEMSK(GOP_OPMUX_REG, GOP_OP_MUX_MASK);
    if(HDR_MUX == OP_MUX_0)
        u16GOP_OPMUX_tmp = (u16GOP_OPMUX_tmp&(~GOP_OP_MUX0)) | GOP_MIXER_MUX << GOP_OP_MUX0_SHIFT;
    else if(HDR_MUX == OP_MUX_1)
        u16GOP_OPMUX_tmp = (u16GOP_OPMUX_tmp&(~GOP_OP_MUX1)) | GOP_MIXER_MUX << GOP_OP_MUX1_SHIFT;
    else if(HDR_MUX == OP_MUX_2)
        u16GOP_OPMUX_tmp = (u16GOP_OPMUX_tmp&(~GOP_OP_MUX2)) | GOP_MIXER_MUX << GOP_OP_MUX2_SHIFT;
    else if(HDR_MUX == OP_MUX_3)
        u16GOP_OPMUX_tmp = (u16GOP_OPMUX_tmp&(~GOP_OP_MUX3)) | GOP_MIXER_MUX << GOP_OP_MUX3_SHIFT;
    else if(HDR_MUX == OP_MUX_4)
        u16GOP_OPMUX_tmp = (u16GOP_OPMUX_tmp&(~GOP_OP_MUX4)) | GOP_MIXER_MUX << GOP_OP_MUX4_SHIFT;

    OSDB_BLENING_ENABLE(HDR_MUX,TRUE);

    MApi_GFLIP_XC_W2BYTEMSK(GOP_OPMUX_DB_REG, 0x0, OPMUX_DBF_EN); //disable opmux double buffer
    MApi_GFLIP_XC_W2BYTEMSK(GOP_OPMUX_REG, u16GOP_OPMUX_tmp, GOP_OP_MUX_MASK);

    MApi_GFLIP_XC_W2BYTEMSK(GOP_MIXER_L_REG, GOP_MIXER_FORCE_WRITE, GOP_MIXER_FORCE_WRITE);    //enable mixer force write
    MApi_GFLIP_XC_W2BYTEMSK(GOP_MIXER_H_REG, u16GOPMixerPortEna,GOP_HDR_BYPASS|GOP_VALID_EN);
    MApi_GFLIP_XC_W2BYTEMSK(GOP_MIXER_L_REG, GOP_MIXER_VDE_SEL|GOP_MIXER_ALPHA_INV, GOP_MIXER_VDE_SEL|GOP_MIXER_ALPHA_INV);
    MApi_GFLIP_XC_W2BYTEMSK(GOP_MIXER_L_REG, 0x0, GOP_MIXER_FORCE_WRITE);    //disable mixer force write

    for(u8index=0;u8index<GOP_MAX;u8index++)
    {
        if(bGOP_OSDB[u8index]==TRUE)
        {
            if(HDR_MUX == u16GOP_OPMux[u8index])
                continue;
            OSDB_BLENING_ENABLE(u16GOP_OPMux[u8index],FALSE);
        }

        _GetBnkOfstByGop(u8index, &u32BankOffSet);
        u16GOPdst = MApi_GFLIP_XC_R2BYTEMSK( u32BankOffSet+ REG_GOP_DST , GOP_DST_MASK ) ;
        if(u16GOPdst==GOP_DST_OP)
        {
            //enable bank force write
            MApi_GFLIP_XC_W2BYTEMSK(u32BankOffSet+GOP_BAK_FORCE_WRITE_REG, GOP_BAK_WRITE, GOP_BAK_WRITE);
            u16pipe=MApi_GFLIP_XC_R2BYTEMSK(GOP_HDE_START_REG, GOP_HDE_START_MASK);
            u16pipe += u16Mux_PipeOffset[HDR_MUX];
            if(bGOPVscalePipeDelay[u8index] == TRUE )
                u16pipe += GOP_NONVSCALE_PIPEDELAY;

            if(u16pipe > GOP_HDR_PIPEDELAY)
                u16pipe = u16pipe - GOP_HDR_PIPEDELAY;
            else
                u16pipe = 0;
            MApi_GFLIP_XC_W2BYTEMSK(u32BankOffSet+REG_GOP_HS_PIPE, u16pipe, GOP_HS_PIPE);//Set GOP HDR pipe delay

            //diable bank force write
            MApi_GFLIP_XC_W2BYTEMSK(u32BankOffSet+GOP_BAK_FORCE_WRITE_REG, 0x0, GOP_BAK_WRITE);
        }
    }

    bHDR_LAST_STATUS=TRUE;
}
/////////////////////////////////////////////////
//////////////GOP HDR setting END////////////////
/////////////////////////////////////////////////

#if 0
printk("[OSD HDR] Video_ full_range_en %x\n",pstu_Cfd_OSD_Process_Configs->u8Video_IsFullRange);
printk("[OSD HDR] Video_ colorprimary %x\n",pstu_Cfd_OSD_Process_Configs->u8Video_colorprimary);
printk("[OSD HDR] Video_ HDRMode %x\n",pstu_Cfd_OSD_Process_Configs->u8Video_HDRMode);
printk("[OSD HDR] Video_ MatrixCoefficients %x\n",pstu_Cfd_OSD_Process_Configs->u8Video_MatrixCoefficients);
printk("[OSD HDR] Video_ MaxLumInNits %x\n",pstu_Cfd_OSD_Process_Configs->u16Video_MaxLumInNits);

printk("[OSD HDR] OSD_ full_range_en %x\n",pstu_Cfd_OSD_Process_Configs->u8OSD_IsFullRange);
printk("[OSD HDR] OSD_ colorprimary %x\n",pstu_Cfd_OSD_Process_Configs->u8OSD_colorprimary);
printk("[OSD HDR] OSD_ HDRMode %x\n",pstu_Cfd_OSD_Process_Configs->u8OSD_HDRMode);
printk("[OSD HDR] OSD_ u8OSD_transferf %x\n",pstu_Cfd_OSD_Process_Configs->u8OSD_transferf);

printk("[OSD HDR] OSD_ u8OSD_Gamma_SRAM_Mode %x\n",pstu_Kastor_OSDIP_Param->u8OSD_Gamma_SRAM_Mode);
#endif

}
else //bypass
{

printf("\n  [CFD][ %s  , %d ]  HDR NOEN  !!!\n",__FUNCTION__,__LINE__);
printf("\n  [CFD][ %s  , %d ]   u8UserMode = %d!  !!!\n",__FUNCTION__,__LINE__,pstu_Kastor_OSDIP_Param->u8Status);


//bug
//MApi_GFLIP_XC_W2BYTE(0x100a4a,0x0010);//clock

//MApi_GFLIP_XC_W2BYTEMSK(0x100a4a, 0x0010, 0x0010); //clock
MApi_GFLIP_XC_W2BYTEMSK(0x100a4a, 0x0001, 0x000f ); //clock off

/////////////////////////////////////////////////
//////////////GOP SDR setting START//////////////
/////////////////////////////////////////////////
    if(bHDR_LAST_STATUS==TRUE)
    {
        for(u8index=0;u8index<GOP_MAX;u8index++)
        {
            _GetBnkOfstByGop(u8index, &u32BankOffSet);
            u16GOPdst = MApi_GFLIP_XC_R2BYTEMSK( u32BankOffSet+ REG_GOP_DST , GOP_DST_MASK ) ;
            if(u16GOPdst==GOP_DST_OP)
            {
                //enable bank force write
                MApi_GFLIP_XC_W2BYTEMSK(u32BankOffSet+GOP_BAK_FORCE_WRITE_REG, GOP_BAK_WRITE, GOP_BAK_WRITE);

                MApi_GFLIP_XC_W2BYTEMSK(u32BankOffSet+REG_GOP_CTRL0, GOP_COLOR, GOP_COLOR); //K6 OP color space always YUV domain
                //Set GOP SDR pipe delay
                u16pipe=MApi_GFLIP_XC_R2BYTEMSK(GOP_HDE_START_REG, GOP_HDE_START_MASK);
                u16pipe += u16Mux_PipeOffset[HDR_MUX];

                if(bGOPVscalePipeDelay[u8index] == TRUE )
                    u16pipe += GOP_NONVSCALE_PIPEDELAY;

                if(u16pipe > GOP_OP_PIPEDELAY)
                   u16pipe = u16pipe - GOP_OP_PIPEDELAY;
                else
                    u16pipe = 0;

                MApi_GFLIP_XC_W2BYTEMSK(u32BankOffSet+REG_GOP_HS_PIPE, u16pipe, GOP_HS_PIPE);

                //disable bank force write
                MApi_GFLIP_XC_W2BYTEMSK(u32BankOffSet+GOP_BAK_FORCE_WRITE_REG, 0x0, GOP_BAK_WRITE);
            }

        }
        //Resotre HDR Mux GOP information
        u16GOP_OPMUX_tmp=MApi_GFLIP_XC_R2BYTEMSK(GOP_OPMUX_REG, GOP_OP_MUX_MASK);
        if(HDR_MUX == OP_MUX_0)
            u16GOP_OPMUX_tmp = (u16GOP_OPMUX_tmp&(~GOP_OP_MUX0)) | u8GOP_HDR_OPMux_GOP << GOP_OP_MUX0_SHIFT;
        else if(HDR_MUX == OP_MUX_1)
            u16GOP_OPMUX_tmp = (u16GOP_OPMUX_tmp&(~GOP_OP_MUX1)) | u8GOP_HDR_OPMux_GOP << GOP_OP_MUX1_SHIFT;
        else if(HDR_MUX == OP_MUX_2)
            u16GOP_OPMUX_tmp = (u16GOP_OPMUX_tmp&(~GOP_OP_MUX2)) | u8GOP_HDR_OPMux_GOP << GOP_OP_MUX2_SHIFT;
        else if(HDR_MUX == OP_MUX_3)
            u16GOP_OPMUX_tmp = (u16GOP_OPMUX_tmp&(~GOP_OP_MUX3)) | u8GOP_HDR_OPMux_GOP << GOP_OP_MUX3_SHIFT;
        else if(HDR_MUX == OP_MUX_4)
            u16GOP_OPMUX_tmp = (u16GOP_OPMUX_tmp&(~GOP_OP_MUX4)) | u8GOP_HDR_OPMux_GOP << GOP_OP_MUX4_SHIFT;

        MApi_GFLIP_XC_W2BYTEMSK(GOP_OPMUX_DB_REG, 0x0, OPMUX_DBF_EN); //disable opmux double buffer
        MApi_GFLIP_XC_W2BYTE(GOP_OPMUX_REG,u16GOP_OPMUX_tmp);


        for(u8index=0;u8index<GOP_MAX;u8index++)
        {
            if(bGOP_OSDB[u8index]==TRUE)
                OSDB_BLENING_ENABLE(u16GOP_OPMux[u8index],TRUE);
        }

        OSDB_BLENING_ENABLE(HDR_MUX,FALSE);

        MApi_GFLIP_XC_W2BYTEMSK(GOP_MIXER_L_REG, GOP_MIXER_FORCE_WRITE, GOP_MIXER_FORCE_WRITE); //enable mixer force write
        MApi_GFLIP_XC_W2BYTEMSK(GOP_MIXER_H_REG, GOP_HDR_BYPASS, GOP_HDR_BYPASS); // [7]: HDR bypass
        MApi_GFLIP_XC_W2BYTEMSK(GOP_MIXER_L_REG, 0x0, GOP_MIXER_FORCE_WRITE); //disable mixer force write

        bHDR_LAST_STATUS=FALSE;
    }
/////////////////////////////////////////////////
//////////////GOP SDR setting END////////////////
/////////////////////////////////////////////////


}
#endif


    return 0;
}


MS_U16 MS_Cfd_Curry_CheckModes(MS_U8 *temp, MS_U8 mode_upbound)
{
    //E_CFD_MC_ERR
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_U8 u8temp = 0;

    u8temp = (*temp)&MaskForMode_LB;

    if (u8temp >= mode_upbound)
    {
            u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_OVERRANGE;
            (*temp) = ((*temp)&MaskForMode_HB);
            HDR_DBG_HAL_CFD(printk("\n  out of range [%s]\n",__func__));
    }

    return u16_check_status;
}

void MS_Cfd_Curry_HDRIP_Debug(STU_CFDAPI_Curry_TMOIP *pstu_Curry_TMO_Param, STU_CFDAPI_Curry_HDRIP *pstu_Curry_HDRIP_Param)
{
    printf("u8TMO_curve_enable_Mode                 :0x%02x\n", pstu_Curry_TMO_Param->u8HDR_TMO_curve_enable_Mode);
    printf("u8TMO_curve_Mode                        :0x%02x\n", pstu_Curry_TMO_Param->u8HDR_TMO_curve_Mode );
    printf("u8TMO_curve_setting_Mode                :0x%02x\n", pstu_Curry_TMO_Param->u8HDR_TMO_curve_setting_Mode );
    printf("u8HDR_UVC_setting_Mode                  :0x%02x\n", pstu_Curry_TMO_Param->u8HDR_UVC_setting_Mode );
    printf("\n");
    printf("u8HDR_enable_Mode                       :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_enable_Mode);
    printf("u8HDR_Composer_Mode                     :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_Composer_Mode);
    printf("u8HDR_Module1_enable_Mode               :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_Module1_enable_Mode);
    printf("\n");
    printf("u8HDR_InputCSC_Mode                     :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_InputCSC_Mode);
    printf("u8HDR_InputCSC_Ratio1                   :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_InputCSC_Ratio1);
    printf("u8HDR_InputCSC_Manual_Vars_en           :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_InputCSC_Manual_Vars_en);
    printf("u8HDR_InputCSC_MC                       :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_InputCSC_MC);
    printf("\n");
    printf("u8HDR_Degamma_SRAM_Mode                 :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_Degamma_SRAM_Mode);
    printf("u8HDR_Degamma_Ratio1                    :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_Degamma_Ratio1);
    printf("u16HDR_Degamma_Ratio2                   :0x%04x\n",pstu_Curry_HDRIP_Param->u16HDR_Degamma_Ratio2);
    printf("u8HDR_DeGamma_Manual_Vars_en            :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_DeGamma_Manual_Vars_en);
    printf("u8HDR_Degamma_TR                        :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_Degamma_TR);
    printf("u8HDR_Degamma_Lut_En                    :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_Degamma_Lut_En);
    printf("pu32HDR_Degamma_Lut_Address             :0x%08x\n",pstu_Curry_HDRIP_Param->pu32HDR_Degamma_Lut_Address);
    printf("u16HDR_Degamma_Lut_Length               :0x%04x\n",pstu_Curry_HDRIP_Param->u16HDR_Degamma_Lut_Length);
    printf("u8DHDR_Degamma_Max_Lum_En               :0x%02x\n",pstu_Curry_HDRIP_Param->u8DHDR_Degamma_Max_Lum_En);
    printf("u16HDR_Degamma_Max_Lum                  :0x%04x\n",pstu_Curry_HDRIP_Param->u16HDR_Degamma_Max_Lum);
    printf("\n");
    printf("u8HDR_3x3_Mode                          :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_3x3_Mode);
    printf("u16HDR_3x3_Ratio2                       :0x%02x\n",pstu_Curry_HDRIP_Param->u16HDR_3x3_Ratio2);
    printf("u8HDR_3x3_Manual_Vars_en                :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_3x3_Manual_Vars_en);
    printf("u8HDR_3x3_InputCP                       :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_3x3_InputCP);
    printf("u8HDR_3x3_OutputCP                      :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_3x3_OutputCP);
    printf("\n");
    printf("u8HDR_Gamma_SRAM_Mode                   :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_Gamma_SRAM_Mode);
    printf("u8HDR_Gamma_Manual_Vars_en              :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_Gamma_Manual_Vars_en);
    printf("u8HDR_Gamma_TR                          :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_Gamma_TR);
    printf("u8HDR_Gamma_Lut_En                      :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_Gamma_Lut_En);
    printf("pu32HDR_Gamma_Lut_Address               :0x%08x\n",pstu_Curry_HDRIP_Param->pu32HDR_Gamma_Lut_Address);
    printf("u16HDR_Gamma_Lut_Length                 :0x%04x\n",pstu_Curry_HDRIP_Param->u16HDR_Gamma_Lut_Length);
    printf("\n");
    printf("u8HDR_OutputCSC_Mode                    :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_OutputCSC_Mode);
    printf("u8HDR_OutputCSC_Ratio1                  :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_OutputCSC_Ratio1);
    printf("u8HDR_OutputCSC_Manual_Vars_en          :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_OutputCSC_Manual_Vars_en);
    printf("u8HDR_OutputCSC_MC                      :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_OutputCSC_MC);
    printf("\n");
    printf("u8HDR_Yoffset_Mode                      :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_Yoffset_Mode);
    printf("\n");
    printf("u8HDR_NLM_enable_Mode                   :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_NLM_enable_Mode);
    printf("u8HDR_NLM_setting_Mode                  :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_NLM_setting_Mode);
    printf("u8HDR_ACGain_enable_Mode                :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_ACGain_enable_Mode);
    printf("u8HDR_ACGain_setting_Mode               :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_ACGain_setting_Mode);
    printf("u8HDR_ACE_enable_Mode                   :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_ACE_enable_Mode);
    printf("u8HDR_ACE_setting_Mode                  :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_ACE_setting_Mode);
    printf("u8HDR_Dither1_setting_Mode              :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_Dither1_setting_Mode);
    printf("\n");
    printf("u8HDR_3DLUT_enable_Mode                 :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_3DLUT_enable_Mode);
    printf("u8HDR_3DLUT_SRAM_Mode                   :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_3DLUT_SRAM_Mode);
    printf("u8HDR_3DLUT_setting_Mode                :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_3DLUT_setting_Mode);
    printf("u8HDR_444to422_enable_Mode              :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_444to422_enable_Mode);
    printf("u8HDR_Dither2_enable_Mode               :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_Dither2_enable_Mode);
    printf("u8HDR_Dither2_setting_Mode              :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDR_Dither2_setting_Mode);
    printf("\n");
    printf("u8HDRIP_Patch(0 means no patch)         :0x%02x\n",pstu_Curry_HDRIP_Param->u8HDRIP_Patch);
    printf("\n");
}

void MS_Cfd_Curry_SDRIP_Debug(STU_CFDAPI_Curry_DLCIP *pstu_DLC_Param, STU_CFDAPI_Curry_SDRIP *pstu_SDRIP_Param)
{
    printf("\n");
    printf("\n");
    printf("u8SDR_IP2_CSC_Mode                       :0x%02x\n",pstu_SDRIP_Param->u8SDR_IP2_CSC_Mode);
    printf("u8SDR_IP2_CSC_Ratio1                     :0x%02x\n",pstu_SDRIP_Param->u8SDR_IP2_CSC_Ratio1);
    printf("u8SDR_IP2_CSC_Manual_Vars_en             :0x%02x\n",pstu_SDRIP_Param->u8SDR_IP2_CSC_Manual_Vars_en);
    printf("u8SDR_IP2_CSC_MC                         :0x%02x\n",pstu_SDRIP_Param->u8SDR_IP2_CSC_MC);
    printf("\n");
    printf("u8SDR_VIP_CM_Mode                        :0x%02x\n",pstu_SDRIP_Param->u8SDR_VIP_CM_Mode);
    printf("u8SDR_VIP_CM_Ratio1                      :0x%02x\n",pstu_SDRIP_Param->u8SDR_VIP_CM_Ratio1);
    printf("u8SDR_VIP_CM_Manual_Vars_en              :0x%02x\n",pstu_SDRIP_Param->u8SDR_VIP_CM_Manual_Vars_en);
    printf("u8SDR_VIP_CM_MC                          :0x%02x\n",pstu_SDRIP_Param->u8SDR_VIP_CM_MC);
    printf("\n");
    printf("u8SDR_Conv420_CM_Mode                    :0x%02x\n",pstu_SDRIP_Param->u8SDR_Conv420_CM_Mode);
    printf("u8SDR_Conv420_CM_Ratio1                  :0x%02x\n",pstu_SDRIP_Param->u8SDR_Conv420_CM_Ratio1);
    printf("u8SDR_Conv420_CM_Manual_Vars_en          :0x%02x\n",pstu_SDRIP_Param->u8SDR_Conv420_CM_Manual_Vars_en);
    printf("u8SDR_Conv420_CM_MC                      :0x%02x\n",pstu_SDRIP_Param->u8SDR_Conv420_CM_MC);
    printf("\n");
    printf("\n");
}

void Mapi_Cfd_Curry_HDRIP_Param_Init(STU_CFDAPI_Curry_HDRIP *pstu_Curry_HDRIP_Param)
{

    //pstu_Curry_HDRIP_Param->u8HDR_enable_Mode = 0xC0;

    //Composer
    //pstu_Curry_HDRIP_Param->u8HDR_Composer_Mode = 0xC0;

    //B01
    //pstu_Curry_HDRIP_Param->u8HDR_Module1_enable_Mode = 0xC0;

    //B01-02
    //pstu_Curry_HDRIP_Param->u8HDR_InputCSC_Mode;
    pstu_Curry_HDRIP_Param->u8HDR_InputCSC_Ratio1 = 0x40;
    pstu_Curry_HDRIP_Param->u8HDR_InputCSC_Manual_Vars_en = 0;
    pstu_Curry_HDRIP_Param->u8HDR_InputCSC_MC = E_CFD_CFIO_MC_BT709_XVYCC709;

    //B01-03
    //pstu_Curry_HDRIP_Param->u8HDR_Degamma_SRAM_Mode = 0;
    pstu_Curry_HDRIP_Param->u8HDR_Degamma_Ratio1 = 0x40;//0x40 = 1 Q2.6
    pstu_Curry_HDRIP_Param->u16HDR_Degamma_Ratio2 = 0x40;//0x40 = 1 Q2.6
    pstu_Curry_HDRIP_Param->u8HDR_DeGamma_Manual_Vars_en = 0;
    pstu_Curry_HDRIP_Param->u8HDR_Degamma_TR = E_CFD_CFIO_TR_BT709;
    pstu_Curry_HDRIP_Param->u8HDR_Degamma_Lut_En = 0;
    pstu_Curry_HDRIP_Param->pu32HDR_Degamma_Lut_Address = NULL;
    pstu_Curry_HDRIP_Param->u16HDR_Degamma_Lut_Length = 0x200;
    pstu_Curry_HDRIP_Param->u8DHDR_Degamma_Max_Lum_En = 0;
    pstu_Curry_HDRIP_Param->u16HDR_Degamma_Max_Lum = 100;

    //B01-04
    //pstu_Curry_HDRIP_Param->u8HDR_3x3_Mode;
    pstu_Curry_HDRIP_Param->u16HDR_3x3_Ratio2= 0x40;//0x40 = 1 Q2.6
    pstu_Curry_HDRIP_Param->u8HDR_3x3_Manual_Vars_en = 0;
    pstu_Curry_HDRIP_Param->u8HDR_3x3_InputCP = E_CFD_CFIO_TR_BT709;
    pstu_Curry_HDRIP_Param->u8HDR_3x3_OutputCP = E_CFD_CFIO_TR_BT709;

    //B01-05
    //pstu_Curry_HDRIP_Param->u8HDR_Gamma_SRAM_Mode = 0;
    pstu_Curry_HDRIP_Param->u8HDR_Gamma_Manual_Vars_en = 0;
    pstu_Curry_HDRIP_Param->u8HDR_Gamma_TR = E_CFD_CFIO_TR_BT709;
    pstu_Curry_HDRIP_Param->u8HDR_Gamma_Lut_En = 0;
    pstu_Curry_HDRIP_Param->pu32HDR_Gamma_Lut_Address = NULL;
    pstu_Curry_HDRIP_Param->u16HDR_Gamma_Lut_Length = 0x200;

    //B01-06
    //pstu_Curry_HDRIP_Param->u8HDR_OutputCSC_Mode;
    pstu_Curry_HDRIP_Param->u8HDR_OutputCSC_Ratio1 = 0x40;
    pstu_Curry_HDRIP_Param->u8HDR_OutputCSC_Manual_Vars_en = 0;
    pstu_Curry_HDRIP_Param->u8HDR_OutputCSC_MC = E_CFD_CFIO_MC_BT709_XVYCC709;

    //MaxRGB for B02
    pstu_Curry_HDRIP_Param->u8HDR_MAXRGB_CSC_Mode = 0xC7;
    pstu_Curry_HDRIP_Param->u8HDR_MAXRGB_Ratio1 = 0x40;
    pstu_Curry_HDRIP_Param->u8HDR_MAXRGB_Manual_Vars_en = 0;
    pstu_Curry_HDRIP_Param->u8HDR_MAXRGB_MC = E_CFD_CFIO_MC_BT709_XVYCC709;

    }

MS_U16 Mapi_Cfd_Curry_HDRIP_Param_Check(STU_CFDAPI_Curry_HDRIP *pstu_Curry_HDRIP_Param)
{

    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_U8 u8_check_status2 = E_CFD_MC_ERR_NOERR;
    //MS_U16 u16Temp;

    MS_U8 u8temp = 0;

    u8temp = FunctionMode(pstu_Curry_HDRIP_Param->u8HDR_enable_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_enable_Mode = function1(pstu_Curry_HDRIP_Param->u8HDR_enable_Mode,0);
    }

    u8temp = FunctionMode(pstu_Curry_HDRIP_Param->u8HDR_Composer_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Composer_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_Composer_Mode = function1(pstu_Curry_HDRIP_Param->u8HDR_Composer_Mode,0);
    }

    u8temp = FunctionMode(pstu_Curry_HDRIP_Param->u8HDR_Module1_enable_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Module1_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_Module1_enable_Mode = function1(pstu_Curry_HDRIP_Param->u8HDR_Module1_enable_Mode,0);
    }

    u8temp = FunctionMode(pstu_Curry_HDRIP_Param->u8HDR_InputCSC_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x0E)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_InputCSC_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_InputCSC_Mode = function1(pstu_Curry_HDRIP_Param->u8HDR_InputCSC_Mode,0);
    }

    //1x
    if (pstu_Curry_HDRIP_Param->u8HDR_InputCSC_Ratio1 > 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_InputCSC_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_InputCSC_Ratio1 = 0x40;
    }

    //1x
    if (pstu_Curry_HDRIP_Param->u8HDR_InputCSC_Ratio1 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_InputCSC_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_InputCSC_Ratio1 = 0x40;
    }

    if (pstu_Curry_HDRIP_Param->u8HDR_InputCSC_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_InputCSC_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_InputCSC_Manual_Vars_en = 0x00;
    }

    if (pstu_Curry_HDRIP_Param->u8HDR_InputCSC_MC >= E_CFD_CFIO_MC_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_InputCSC_MC is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_InputCSC_MC = E_CFD_CFIO_MC_BT709_XVYCC709;
    }

    u8temp = FunctionMode(pstu_Curry_HDRIP_Param->u8HDR_Degamma_SRAM_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x03)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Degamma_SRAM_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_Degamma_SRAM_Mode = function1(pstu_Curry_HDRIP_Param->u8HDR_Degamma_SRAM_Mode,0);
    }

    //2x
    if (pstu_Curry_HDRIP_Param->u8HDR_Degamma_Ratio1 > 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Degamma_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_Degamma_Ratio1 = 0x40;
    }

    //1x
    if (pstu_Curry_HDRIP_Param->u8HDR_Degamma_Ratio1 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Degamma_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_Degamma_Ratio1 = 0x40;
    }

    //2x
    if (pstu_Curry_HDRIP_Param->u16HDR_Degamma_Ratio2 > 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u16HDR_Degamma_Ratio2 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u16HDR_Degamma_Ratio2 = 0x40;
    }

    //1x
    if (pstu_Curry_HDRIP_Param->u16HDR_Degamma_Ratio2 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u16HDR_Degamma_Ratio2 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u16HDR_Degamma_Ratio2 = 0x40;
    }

    if (pstu_Curry_HDRIP_Param->u8HDR_DeGamma_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_DeGamma_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_DeGamma_Manual_Vars_en = 0x00;
    }

    if (pstu_Curry_HDRIP_Param->u8HDR_Degamma_TR >= E_CFD_CFIO_TR_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Degamma_TR is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_Degamma_TR = E_CFD_CFIO_TR_BT709;
    }

    if (pstu_Curry_HDRIP_Param->u8HDR_Degamma_Lut_En > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Degamma_Lut_En is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_Degamma_Lut_En = 0x00;
    }

    if (pstu_Curry_HDRIP_Param->u16HDR_Degamma_Lut_Length != 512)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u16HDR_Degamma_Lut_Length is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u16HDR_Degamma_Lut_Length = 512;
    }

    if (pstu_Curry_HDRIP_Param->u8DHDR_Degamma_Max_Lum_En != 0)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8DHDR_Degamma_Max_Lum_En is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8DHDR_Degamma_Max_Lum_En = 0x00;
    }

    if (pstu_Curry_HDRIP_Param->u16HDR_Degamma_Max_Lum > 0xff00)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u16HDR_Degamma_Max_Lum is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u16HDR_Degamma_Max_Lum = 0xff00;
    }

    u8temp = FunctionMode(pstu_Curry_HDRIP_Param->u8HDR_3x3_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x04)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_3x3_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_3x3_Mode = function1(pstu_Curry_HDRIP_Param->u8HDR_3x3_Mode,0);
    }

    //1x
    if (pstu_Curry_HDRIP_Param->u16HDR_3x3_Ratio2 > 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u16HDR_3x3_Ratio2 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u16HDR_3x3_Ratio2 = 0x40;
    }

    //1x
    if (pstu_Curry_HDRIP_Param->u16HDR_3x3_Ratio2 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u16HDR_3x3_Ratio2 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u16HDR_3x3_Ratio2 = 0x40;
    }

    if (pstu_Curry_HDRIP_Param->u8HDR_3x3_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_3x3_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_3x3_Manual_Vars_en = 0x00;
    }

    //if (pstu_Curry_HDRIP_Param->u8HDR_3x3_InputCP >= E_CFD_CFIO_CP_RESERVED_START)
    if (MS_Cfd_checkColorprimaryIsUndefined(&(pstu_Curry_HDRIP_Param->u8HDR_3x3_InputCP)))
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_3x3_InputCP is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_3x3_InputCP = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
    }

    //if (pstu_Curry_HDRIP_Param->u8HDR_3x3_OutputCP >= E_CFD_CFIO_CP_RESERVED_START)
    if (MS_Cfd_checkColorprimaryIsUndefined(&(pstu_Curry_HDRIP_Param->u8HDR_3x3_OutputCP)))
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_3x3_OutputCP is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_3x3_OutputCP = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
    }

    u8temp = FunctionMode(pstu_Curry_HDRIP_Param->u8HDR_Gamma_SRAM_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x03)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Gamma_SRAM_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_Gamma_SRAM_Mode = function1(pstu_Curry_HDRIP_Param->u8HDR_Gamma_SRAM_Mode,0);
    }

    if (pstu_Curry_HDRIP_Param->u8HDR_Gamma_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Gamma_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_Gamma_Manual_Vars_en = 0x00;
    }

    if (pstu_Curry_HDRIP_Param->u8HDR_Gamma_TR >= E_CFD_CFIO_TR_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Gamma_TR is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_Gamma_TR = E_CFD_CFIO_TR_BT709;
    }

    if (pstu_Curry_HDRIP_Param->u8HDR_Gamma_Lut_En > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Gamma_Lut_En is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_Gamma_Lut_En = 0x00;
    }

    if (pstu_Curry_HDRIP_Param->u16HDR_Gamma_Lut_Length != 512)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u16HDR_Gamma_Lut_Length is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u16HDR_Gamma_Lut_Length = 512;
    }

    u8temp = FunctionMode(pstu_Curry_HDRIP_Param->u8HDR_OutputCSC_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x0E)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_OutputCSC_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_OutputCSC_Mode = function1(pstu_Curry_HDRIP_Param->u8HDR_OutputCSC_Mode,0);
    }

    //1x
    if (pstu_Curry_HDRIP_Param->u8HDR_OutputCSC_Ratio1 > 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_OutputCSC_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_OutputCSC_Ratio1 = 0x40;
    }

    //1x
    if (pstu_Curry_HDRIP_Param->u8HDR_OutputCSC_Ratio1 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_OutputCSC_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_OutputCSC_Ratio1 = 0x40;
    }

    if (pstu_Curry_HDRIP_Param->u8HDR_OutputCSC_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_OutputCSC_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_OutputCSC_Manual_Vars_en = 0x00;
    }

    if (pstu_Curry_HDRIP_Param->u8HDR_OutputCSC_MC >= E_CFD_CFIO_MC_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_OutputCSC_MC is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_OutputCSC_MC = E_CFD_CFIO_MC_BT709_XVYCC709;
    }

    u8temp = FunctionMode(pstu_Curry_HDRIP_Param->u8HDR_Yoffset_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Yoffset_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_Yoffset_Mode = function1(pstu_Curry_HDRIP_Param->u8HDR_Yoffset_Mode,0);
    }

    u8temp = FunctionMode(pstu_Curry_HDRIP_Param->u8HDR_MAXRGB_CSC_Mode);
    if (u8temp != E_CFD_IP_CSC_YFULL_TO_RFULL)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_MAXRGB_CSC_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_MAXRGB_CSC_Mode = function1(pstu_Curry_HDRIP_Param->u8HDR_MAXRGB_CSC_Mode,E_CFD_IP_CSC_YFULL_TO_RFULL);
    }

    //1x
    if (pstu_Curry_HDRIP_Param->u8HDR_MAXRGB_Ratio1 > 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_MAXRGB_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_MAXRGB_Ratio1 = 0x40;
    }

    //1x
    if (pstu_Curry_HDRIP_Param->u8HDR_MAXRGB_Ratio1 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_MAXRGB_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_MAXRGB_Ratio1 = 0x40;
    }

    if (pstu_Curry_HDRIP_Param->u8HDR_MAXRGB_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_MAXRGB_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_MAXRGB_Manual_Vars_en = 0x00;
    }

    if (pstu_Curry_HDRIP_Param->u8HDR_MAXRGB_MC >= E_CFD_CFIO_MC_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_MAXRGB_MC is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_MAXRGB_MC = E_CFD_CFIO_MC_BT709_XVYCC709;
    }

    u8temp = FunctionMode(pstu_Curry_HDRIP_Param->u8HDR_NLM_enable_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_NLM_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_NLM_enable_Mode = function1(pstu_Curry_HDRIP_Param->u8HDR_NLM_enable_Mode,0x00);
    }

    u8temp = FunctionMode(pstu_Curry_HDRIP_Param->u8HDR_NLM_setting_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x03)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_NLM_setting_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_NLM_setting_Mode = function1(pstu_Curry_HDRIP_Param->u8HDR_NLM_setting_Mode,0x00);
    }

    u8temp = FunctionMode(pstu_Curry_HDRIP_Param->u8HDR_ACGain_enable_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_ACGain_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_ACGain_enable_Mode = function1(pstu_Curry_HDRIP_Param->u8HDR_ACGain_enable_Mode,0x00);
    }

    u8temp = FunctionMode(pstu_Curry_HDRIP_Param->u8HDR_ACGain_setting_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_ACGain_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_ACGain_setting_Mode = function1(pstu_Curry_HDRIP_Param->u8HDR_ACGain_setting_Mode,0x00);
    }

    u8temp = FunctionMode(pstu_Curry_HDRIP_Param->u8HDR_ACE_enable_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_ACE_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_ACE_enable_Mode = function1(pstu_Curry_HDRIP_Param->u8HDR_ACE_enable_Mode,0x00);
    }

    u8temp = FunctionMode(pstu_Curry_HDRIP_Param->u8HDR_ACE_setting_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_ACE_setting_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_ACE_setting_Mode = function1(pstu_Curry_HDRIP_Param->u8HDR_ACE_setting_Mode,0x00);
    }

    u8temp = FunctionMode(pstu_Curry_HDRIP_Param->u8HDR_Dither1_setting_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Dither1_setting_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_Dither1_setting_Mode = function1(pstu_Curry_HDRIP_Param->u8HDR_Dither1_setting_Mode,0x00);
    }

    u8temp = FunctionMode(pstu_Curry_HDRIP_Param->u8HDR_3DLUT_enable_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_3DLUT_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_3DLUT_enable_Mode = function1(pstu_Curry_HDRIP_Param->u8HDR_3DLUT_enable_Mode,0x00);
    }

    u8temp = FunctionMode(pstu_Curry_HDRIP_Param->u8HDR_3DLUT_SRAM_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_3DLUT_SRAM_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_3DLUT_SRAM_Mode = function1(pstu_Curry_HDRIP_Param->u8HDR_3DLUT_SRAM_Mode,0x00);
    }

    u8temp = FunctionMode(pstu_Curry_HDRIP_Param->u8HDR_3DLUT_setting_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x03)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_3DLUT_setting_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_3DLUT_setting_Mode = function1(pstu_Curry_HDRIP_Param->u8HDR_3DLUT_setting_Mode,0x00);
    }

    u8temp = FunctionMode(pstu_Curry_HDRIP_Param->u8HDR_444to422_enable_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_444to422_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_444to422_enable_Mode = function1(pstu_Curry_HDRIP_Param->u8HDR_444to422_enable_Mode,0x00);
    }

    u8temp = FunctionMode(pstu_Curry_HDRIP_Param->u8HDR_Dither2_enable_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Dither2_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_Dither2_enable_Mode = function1(pstu_Curry_HDRIP_Param->u8HDR_Dither2_enable_Mode,0x00);
    }

    u8temp = FunctionMode(pstu_Curry_HDRIP_Param->u8HDR_Dither2_setting_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Dither2_setting_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_Dither2_setting_Mode = function1(pstu_Curry_HDRIP_Param->u8HDR_Dither2_setting_Mode,0x00);
    }
#if (1 == RealChip)
    u16_check_status = E_CFD_MC_ERR_NOERR;//Easter_test
#endif
    return u16_check_status;
}

void Mapi_Cfd_Curry_TMOIP_Param_Init(STU_CFDAPI_Curry_TMOIP *pstu_Curry_TMOIP_Param)
{
    //pstu_Kano_TMOIP_Param->u8HDR_TMO_curve_Mode = 0;
    //for TMO algorithm ,  from user and driver
    //0: not set
    //1: set
    pstu_Curry_TMOIP_Param->u8HDR_TMO_param_SetbyDriver = 0;
}

MS_U16 Mapi_Cfd_Curry_TMOIP_Param_Check(STU_CFDAPI_Curry_TMOIP *pstu_Curry_TMOIP_Param)
{

    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_U8 u8_check_status2 = E_CFD_MC_ERR_NOERR;
    //MS_U16 u16Temp;

    MS_U8 u8temp = 0;
    MS_U16 u16temp = 0;

    u8temp = FunctionMode(pstu_Curry_TMOIP_Param->u8HDR_TMO_curve_enable_Mode);

    if (u8temp >= E_CFD_RESERVED_AT0x02)
{
        HDR_DBG_HAL_CFD(printk("\n TMO u8HDR_TMO_curve_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_TMO_OVERRANGE;
        pstu_Curry_TMOIP_Param->u8HDR_TMO_curve_enable_Mode = function1(pstu_Curry_TMOIP_Param->u8HDR_TMO_curve_enable_Mode,0);
}

    u8temp = FunctionMode(pstu_Curry_TMOIP_Param->u8HDR_TMO_curve_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x04)
{
        HDR_DBG_HAL_CFD(printk("\n  TMO u8HDR_TMO_curve_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_TMO_OVERRANGE;
        pstu_Curry_TMOIP_Param->u8HDR_TMO_curve_Mode = function1(pstu_Curry_TMOIP_Param->u8HDR_TMO_curve_Mode,0);
    }

    u8temp = FunctionMode(pstu_Curry_TMOIP_Param->u8HDR_TMO_curve_setting_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x03)
    {
        HDR_DBG_HAL_CFD(printk("\n  TMO u8HDR_TMO_curve_setting_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_TMO_OVERRANGE;
        pstu_Curry_TMOIP_Param->u8HDR_TMO_curve_setting_Mode = function1(pstu_Curry_TMOIP_Param->u8HDR_TMO_curve_setting_Mode,0);
    }

    u8temp = FunctionMode(pstu_Curry_TMOIP_Param->u8HDR_UVC_setting_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x04)
    {
        HDR_DBG_HAL_CFD(printk("\n  TMO u8HDR_UVC_setting_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_TMO_OVERRANGE;
        pstu_Curry_TMOIP_Param->u8HDR_UVC_setting_Mode = function1(pstu_Curry_TMOIP_Param->u8HDR_UVC_setting_Mode,0);
    }

    u8temp = FunctionMode(pstu_Curry_TMOIP_Param->u8HDR_TMO_param_SetbyDriver);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  TMO u8HDR_TMO_param_SetbyDriver is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_TMO_OVERRANGE;
        pstu_Curry_TMOIP_Param->u8HDR_TMO_param_SetbyDriver = 0;
    }

    return u16_check_status;
}

void Mapi_Cfd_Curry_SDRIP_Param_Init(STU_CFDAPI_Curry_SDRIP *pstu_Curry_SDRIP_Param)
{

    pstu_Curry_SDRIP_Param->u8SDR_IP2_CSC_Ratio1 = 0x40;
    pstu_Curry_SDRIP_Param->u8SDR_IP2_CSC_Manual_Vars_en = 0;
    pstu_Curry_SDRIP_Param->u8SDR_IP2_CSC_MC = E_CFD_CFIO_MC_BT709_XVYCC709;

    pstu_Curry_SDRIP_Param->u8SDR_VIP_CM_Ratio1 = 0x40;
    pstu_Curry_SDRIP_Param->u8SDR_VIP_CM_Manual_Vars_en = 0;
    pstu_Curry_SDRIP_Param->u8SDR_VIP_CM_MC = E_CFD_CFIO_MC_BT709_XVYCC709;

    pstu_Curry_SDRIP_Param->u8SDR_Conv420_CM_Ratio1 = 0x40;
    pstu_Curry_SDRIP_Param->u8SDR_Conv420_CM_Manual_Vars_en = 0;
    pstu_Curry_SDRIP_Param->u8SDR_Conv420_CM_MC = E_CFD_CFIO_MC_BT709_XVYCC709;

#if (Curry_Control_HDMITX_CSC == 1)
    pstu_Curry_SDRIP_Param->u8SDR_HDMITX_CSC_Ratio1 = 0x40;
    pstu_Curry_SDRIP_Param->u8SDR_HDMITX_CSC_Manual_Vars_en = 0;
    pstu_Curry_SDRIP_Param->u8SDR_HDMITX_CSC_MC = E_CFD_CFIO_MC_BT709_XVYCC709;
#endif
}

MS_U16 Mapi_Cfd_Curry_SDRIP_Param_Check(STU_CFDAPI_Curry_SDRIP *pstu_Curry_SDRIP_Param)
{
    MS_U8 u8_check_status = TRUE;

    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_U8 u8_check_status2 = E_CFD_MC_ERR_NOERR;
    //MS_U16 u16Temp;

    MS_U8 u8temp = 0;

    u8temp = FunctionMode(pstu_Curry_SDRIP_Param->u8SDR_IP2_CSC_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x0D)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_IP2_CSC_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Curry_SDRIP_Param->u8SDR_IP2_CSC_Mode = function1(pstu_Curry_SDRIP_Param->u8SDR_IP2_CSC_Mode,0);
    }

    //1x
    if (pstu_Curry_SDRIP_Param->u8SDR_IP2_CSC_Ratio1 > 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8IP2_CSC_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Curry_SDRIP_Param->u8SDR_IP2_CSC_Ratio1 = 0x40;
    }

    //1x
    if (pstu_Curry_SDRIP_Param->u8SDR_IP2_CSC_Ratio1 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8IP2_CSC_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Curry_SDRIP_Param->u8SDR_IP2_CSC_Ratio1 = 0x40;
    }

    if (pstu_Curry_SDRIP_Param->u8SDR_IP2_CSC_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_IP2_CSC_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Curry_SDRIP_Param->u8SDR_IP2_CSC_Manual_Vars_en = 0x00;
    }

    if (pstu_Curry_SDRIP_Param->u8SDR_IP2_CSC_MC >= E_CFD_CFIO_MC_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8SDR_IP2_CSC_MC is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Curry_SDRIP_Param->u8SDR_IP2_CSC_MC = E_CFD_CFIO_MC_BT709_XVYCC709;
    }


    u8temp = FunctionMode(pstu_Curry_SDRIP_Param->u8SDR_VIP_CM_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x0D)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_VIP_CM_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Curry_SDRIP_Param->u8SDR_VIP_CM_Mode = function1(pstu_Curry_SDRIP_Param->u8SDR_VIP_CM_Mode,0);
    }

    //not supported cases
    if ((u8temp == E_CFD_IP_CSC_RFULL_TO_RLIMIT) ||
        (u8temp == E_CFD_IP_CSC_RLIMIT_TO_RFULL) ||
        (u8temp == E_CFD_IP_CSC_RLIMIT_TO_YFULL) ||
        (u8temp == E_CFD_IP_CSC_RLIMIT_TO_YLIMIT) ||
        (u8temp == E_CFD_IP_CSC_YFULL_TO_RLIMIT) ||
        (u8temp == E_CFD_IP_CSC_YLIMIT_TO_RLIMIT))
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_VIP_CM_Mode %d is not supported now \n",u8temp));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_MODE_SDR_NOTSUPPORTED;

        switch(u8temp)
        {
            case E_CFD_IP_CSC_RFULL_TO_RLIMIT:
            u8temp = E_CFD_IP_CSC_OFF;
            break;

            case E_CFD_IP_CSC_RLIMIT_TO_RFULL:
            u8temp = E_CFD_IP_CSC_OFF;
            break;

            case E_CFD_IP_CSC_RLIMIT_TO_YFULL:
            u8temp = E_CFD_IP_CSC_RFULL_TO_YFULL;
            break;

            case E_CFD_IP_CSC_RLIMIT_TO_YLIMIT:
            u8temp = E_CFD_IP_CSC_RFULL_TO_YLIMIT;
            break;

            case E_CFD_IP_CSC_YFULL_TO_RLIMIT:
            u8temp = E_CFD_IP_CSC_YFULL_TO_RFULL;
            break;

            case E_CFD_IP_CSC_YLIMIT_TO_RLIMIT:
            u8temp = E_CFD_IP_CSC_YLIMIT_TO_RFULL;
            break;
        }

        pstu_Curry_SDRIP_Param->u8SDR_VIP_CM_Mode = function1(pstu_Curry_SDRIP_Param->u8SDR_VIP_CM_Mode,u8temp);
    }

    //1x
    if (pstu_Curry_SDRIP_Param->u8SDR_VIP_CM_Ratio1 > 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_VIP_CM_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Curry_SDRIP_Param->u8SDR_VIP_CM_Ratio1 = 0x40;
    }

    //1x
    if (pstu_Curry_SDRIP_Param->u8SDR_VIP_CM_Ratio1 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_VIP_CM_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Curry_SDRIP_Param->u8SDR_VIP_CM_Ratio1 = 0x40;
    }

    if (pstu_Curry_SDRIP_Param->u8SDR_VIP_CM_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_VIP_CM_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Curry_SDRIP_Param->u8SDR_VIP_CM_Manual_Vars_en = 0x00;
    }

    if (pstu_Curry_SDRIP_Param->u8SDR_VIP_CM_MC >= E_CFD_CFIO_MC_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8SDR_VIP_CM_MC is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Curry_SDRIP_Param->u8SDR_VIP_CM_MC = E_CFD_CFIO_MC_BT709_XVYCC709;
    }

    u8temp = FunctionMode(pstu_Curry_SDRIP_Param->u8SDR_Conv420_CM_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x0D)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_Conv420_CM_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Curry_SDRIP_Param->u8SDR_Conv420_CM_Mode = function1(pstu_Curry_SDRIP_Param->u8SDR_Conv420_CM_Mode,0);
    }

    //not supported cases
    if ((u8temp == E_CFD_IP_CSC_RFULL_TO_RLIMIT) ||
        (u8temp == E_CFD_IP_CSC_RLIMIT_TO_RFULL) ||
        (u8temp == E_CFD_IP_CSC_RLIMIT_TO_YFULL) ||
        (u8temp == E_CFD_IP_CSC_RLIMIT_TO_YLIMIT) ||
        (u8temp == E_CFD_IP_CSC_YFULL_TO_RLIMIT) ||
        (u8temp == E_CFD_IP_CSC_YLIMIT_TO_RLIMIT))
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_Conv420_CM_Mode %d is not supported now \n",u8temp));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_MODE_SDR_NOTSUPPORTED;

        switch(u8temp)
        {
            case E_CFD_IP_CSC_RFULL_TO_RLIMIT:
            u8temp = E_CFD_IP_CSC_OFF;
            break;

            case E_CFD_IP_CSC_RLIMIT_TO_RFULL:
            u8temp = E_CFD_IP_CSC_OFF;
            break;

            case E_CFD_IP_CSC_RLIMIT_TO_YFULL:
            u8temp = E_CFD_IP_CSC_RFULL_TO_YFULL;
            break;

            case E_CFD_IP_CSC_RLIMIT_TO_YLIMIT:
            u8temp = E_CFD_IP_CSC_RFULL_TO_YLIMIT;
            break;

            case E_CFD_IP_CSC_YFULL_TO_RLIMIT:
            u8temp = E_CFD_IP_CSC_YFULL_TO_RFULL;
            break;

            case E_CFD_IP_CSC_YLIMIT_TO_RLIMIT:
            u8temp = E_CFD_IP_CSC_YLIMIT_TO_RFULL;
            break;
        }

        pstu_Curry_SDRIP_Param->u8SDR_Conv420_CM_Mode = function1(pstu_Curry_SDRIP_Param->u8SDR_Conv420_CM_Mode,u8temp);
    }

    //1x
    if (pstu_Curry_SDRIP_Param->u8SDR_Conv420_CM_Ratio1 > 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_Conv420_CM_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Curry_SDRIP_Param->u8SDR_Conv420_CM_Ratio1 = 0x40;
    }

    //1x
    if (pstu_Curry_SDRIP_Param->u8SDR_Conv420_CM_Ratio1 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_Conv420_CM_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Curry_SDRIP_Param->u8SDR_Conv420_CM_Ratio1 = 0x40;
    }

    if (pstu_Curry_SDRIP_Param->u8SDR_Conv420_CM_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_Conv420_CM_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Curry_SDRIP_Param->u8SDR_Conv420_CM_Manual_Vars_en = 0x00;
    }

    if (pstu_Curry_SDRIP_Param->u8SDR_Conv420_CM_MC >= E_CFD_CFIO_MC_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8SDR_Conv420_CM_MC is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Curry_SDRIP_Param->u8SDR_Conv420_CM_MC = E_CFD_CFIO_MC_BT709_XVYCC709;
    }

#if (Curry_Control_HDMITX_CSC == 1)

    u8temp = FunctionMode(pstu_Curry_SDRIP_Param->u8SDR_HDMITX_CSC_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x0D)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_HDMITX_CSC_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Curry_SDRIP_Param->u8SDR_HDMITX_CSC_Mode = function1(pstu_Curry_SDRIP_Param->u8SDR_HDMITX_CSC_Mode,0);
    }

    //1x
    if (pstu_Curry_SDRIP_Param->u8SDR_HDMITX_CSC_Ratio1 > 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_HDMITX_CSC_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Curry_SDRIP_Param->u8SDR_HDMITX_CSC_Ratio1 = 0x40;
    }

    //1x
    if (pstu_Curry_SDRIP_Param->u8SDR_HDMITX_CSC_Ratio1 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_HDMITX_CSC_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Curry_SDRIP_Param->u8SDR_HDMITX_CSC_Ratio1 = 0x40;
    }

    if (pstu_Curry_SDRIP_Param->u8SDR_HDMITX_CSC_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_HDMITX_CSC_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Curry_SDRIP_Param->u8SDR_HDMITX_CSC_Manual_Vars_en = 0x00;
    }

    if (pstu_Curry_SDRIP_Param->u8SDR_HDMITX_CSC_MC >= E_CFD_CFIO_MC_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8SDR_HDMITX_CSC_MC is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Curry_SDRIP_Param->u8SDR_HDMITX_CSC_MC = E_CFD_CFIO_MC_BT709_XVYCC709;
    }

#endif

    return u16_check_status;
}

//start of Curry IP write register function =========================================================
MS_U8 MS_Cfd_Curry_HDR_TMO_Curve_enable_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{
    //0: OFF
    //1: ON

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_TMO_curve_enable_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //Dolby case
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        //open HDR case
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        else if ((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0]))
    {
            if (1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag)
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        else
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }
        }
        else
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }

        u8_mode = (pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_TMO_curve_enable_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
        else
        {
        pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_TMO_curve_enable_Mode), 0x02);

        u8_mode = pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_TMO_curve_enable_Mode;
    }

    return u8_mode;
}


MS_U8 MS_Cfd_Curry_HDR_TMO_Curve_Mode_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{
    MS_U8 u8_mode = 0;

    // 0: linear
    // 1: By TMO algorithm function
    // 2: by Dolby driver function
    // 3: Manual Mode

    //MS_U8  u8TMO_curve_Mode;
    //MS_U8  u8TMO_curve_Manual_Mode;
    pstu_Control_Param->u8DoDLCInHDRIP_Flag = 0;

    if(0x80 == (pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_TMO_curve_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 2;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            if (1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag)
        {
            u8_mode = 1;
        }
        else
        {
            u8_mode = 0;
        }
        }
        else
        {
            u8_mode = 0;
        }

        u8_mode = (pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_TMO_curve_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_TMO_curve_Mode), 0x04);

        u8_mode = pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_TMO_curve_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Curry_HDR_UVC_setting_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{
    MS_U8 u8_mode = 0;

    // 0: PQ0_bypass
    // 1: PQ1_Open_mode
    // 2: PQ2_openHDR_bypass
    // 3: Dolby_mode

    if(0x80 == (pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_UVC_setting_Mode&0x80))
    {
        //Dolby HDR
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 3;
        }
        //openHDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            if (1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag)
            {
                u8_mode = 1;
            }
            else
            {
                if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
                    u8_mode = 0;
                else
                    u8_mode = 2;
            }
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        else if ((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0]))
        {
            if (1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag)
        {
            u8_mode = 1;
        }
        else
        {
            if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
                u8_mode = 0;
            else
                u8_mode = 2;
        }
        }
        else
        {
            if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
                u8_mode = 0;
            else
                u8_mode = 2;
        }

        u8_mode = (pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_UVC_setting_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_UVC_setting_Mode), 0x03);
        u8_mode = pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_UVC_setting_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Curry_HDR_TMO_Curve_setting_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{
    MS_U8 u8_mode = 0;

    // 0: PQ0
    // 1: Open_mode
    // 2: Dolby_mode

    if(0x80 == (pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_TMO_curve_setting_Mode&0x80))
    {

        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 2;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            if (1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag)
        {
            u8_mode = 1;
        }
        else
        {
            u8_mode = 0;
        }
        }
        else
        {
            u8_mode = 0;
        }

        u8_mode = (pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_TMO_curve_setting_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_TMO_curve_setting_Mode), 0x03);
        u8_mode = pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_TMO_curve_setting_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Curry_HDR_enable_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    //0: OFF
    //1: ON
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_enable_Mode&0x80))
    {

        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        else if ((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0]))
        {
            if (((1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag) && (0x04 == (CFD_HDR_IP_CAPABILITY&0x04))) ||
            ((1 == pstu_Control_Param->u8DoGamutMappingInHDRIP_Flag) && (0x02 == (CFD_HDR_IP_CAPABILITY&0x02))) ||
            ((1 == pstu_Control_Param->u8DoBT2020CLPInHDRIP_Flag) && (0x08 == (CFD_HDR_IP_CAPABILITY&0x08))) ||
            ((1 == pstu_Control_Param->u8DoDLCInHDRIP_Flag) && (0x01 == (CFD_HDR_IP_CAPABILITY&0x01))))
            {
                u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
            }
            else if (((E_CFD_CFIO_XVYCC_601 == pstu_Control_Param->u8Input_Format) ||
                     (E_CFD_CFIO_XVYCC_709 == pstu_Control_Param->u8Input_Format) ||
                     (E_CFD_CFIO_SYCC601 == pstu_Control_Param->u8Input_Format)) &&
                     (0x10 == (CFD_HDR_IP_CAPABILITY&0x10)))
        {

            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
            else if (1 == pstu_Control_Param->u8DoForceEnterHDRIP_Flag)
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
            else
        {
                u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
            }
        }
        else
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }

        u8_mode = (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_enable_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_enable_Mode), 0x02);

        u8_mode = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_enable_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Curry_HDR_Composer_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    //0: OFF
    //1: dolby_driver()
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Composer_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 1;
        }
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 0;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        else if ((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0]))
        {
            u8_mode = 0;
        }
        else
        {
            u8_mode = 0;
        }

        u8_mode = (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Composer_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Composer_Mode), 0x02);
        u8_mode = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Composer_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Curry_HDR_Module1_enable_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    //0: OFF
    //1: ON
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Module1_enable_Mode&0x80))
    {

        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        else if ((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0]))
        {
            if (((1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag) && (0x04 == (CFD_HDR_IP_CAPABILITY&0x04))) ||
            ((1 == pstu_Control_Param->u8DoGamutMappingInHDRIP_Flag) && (0x02 == (CFD_HDR_IP_CAPABILITY&0x02))) ||
            ((1 == pstu_Control_Param->u8DoBT2020CLPInHDRIP_Flag) && (0x08 == (CFD_HDR_IP_CAPABILITY&0x08))) ||
            ((1 == pstu_Control_Param->u8DoDLCInHDRIP_Flag) && (0x01 == (CFD_HDR_IP_CAPABILITY&0x01))))
            {
                u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
            }
            else if (((E_CFD_CFIO_XVYCC_601 == pstu_Control_Param->u8Input_Format) ||
                     (E_CFD_CFIO_XVYCC_709 == pstu_Control_Param->u8Input_Format) ||
                     (E_CFD_CFIO_SYCC601 == pstu_Control_Param->u8Input_Format)) &&
                     (0x10 == (CFD_HDR_IP_CAPABILITY&0x10)))
            {
                u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
            }
            else if (1 == pstu_Control_Param->u8DoForceEnterHDRIP_Flag)
            {
                u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
            }
            else
            {
                u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
            }
        }
        else
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }

        u8_mode = (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Module1_enable_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Module1_enable_Mode), 0x02);

        u8_mode = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Module1_enable_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Curry_HDR_InputCSC_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    //0: OFF
    //1~12 : see definition in E_CFD_IP_CSC_PROCESS
    //13 : dolby_driver()

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_InputCSC_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 13;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            if (((1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag) && (0x04 == (CFD_HDR_IP_CAPABILITY&0x04))) ||
            ((1 == pstu_Control_Param->u8DoGamutMappingInHDRIP_Flag) && (0x02 == (CFD_HDR_IP_CAPABILITY&0x02))) ||
            ((1 == pstu_Control_Param->u8DoBT2020CLPInHDRIP_Flag) && (0x08 == (CFD_HDR_IP_CAPABILITY&0x08))) ||
            ((1 == pstu_Control_Param->u8DoDLCInHDRIP_Flag) && (0x01 == (CFD_HDR_IP_CAPABILITY&0x01)))||
            ((E_CFD_CFIO_XVYCC_601 == pstu_Control_Param->u8Input_Format) && (0x10 == (CFD_HDR_IP_CAPABILITY&0x10))) ||
            ((E_CFD_CFIO_XVYCC_709 == pstu_Control_Param->u8Input_Format) && (0x10 == (CFD_HDR_IP_CAPABILITY&0x10))) ||
            ((E_CFD_CFIO_SYCC601 == pstu_Control_Param->u8Input_Format) && (0x10 == (CFD_HDR_IP_CAPABILITY&0x10))) ||
            (1 == pstu_Control_Param->u8DoForceEnterHDRIP_Flag))
            {

                if (E_CFD_MC_FORMAT_RGB == pstu_Control_Param->u8Temp_DataFormat[0])
                {
                    if (0 == pstu_Control_Param->u8Input_IsRGBBypass)
                    {
                    //
                    if(E_CFD_CFIO_RANGE_LIMIT == pstu_Control_Param->u8Temp_IsFullRange[0])
                    {
                        u8_mode = E_CFD_IP_CSC_RLIMIT_TO_RFULL;
                    }
                    //RFULL to RFULL = OFF
                    else if(E_CFD_CFIO_RANGE_FULL== pstu_Control_Param->u8Temp_IsFullRange[0])
                    {
                        u8_mode = E_CFD_IP_CSC_OFF;
                    }
                    else
                    {
                        u8_mode = E_CFD_IP_CSC_OFF;
                    }
                }
                    else // u8Input_IsRGBBypass = 1
                    {
                        u8_mode = E_CFD_IP_CSC_OFF; //keep RGB
                    }
                }
                else if ( E_CFD_MC_FORMAT_RGB != pstu_Control_Param->u8Temp_DataFormat[0])
                {
                    if(E_CFD_CFIO_RANGE_LIMIT == pstu_Control_Param->u8Temp_IsFullRange[0])
                    {
                        u8_mode = E_CFD_IP_CSC_YLIMIT_TO_RFULL;
                    }
                    else if(E_CFD_CFIO_RANGE_FULL== pstu_Control_Param->u8Temp_IsFullRange[0])
                    {
                        u8_mode = E_CFD_IP_CSC_YFULL_TO_RFULL;
                    }
                    else
                    {
                        u8_mode = E_CFD_IP_CSC_OFF;
                    }
                }
            }
            else
        {
                u8_mode = E_CFD_IP_CSC_OFF;
            }
        }
        else
        {
            u8_mode = E_CFD_IP_CSC_OFF;
        }

        u8_mode = (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_InputCSC_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_InputCSC_Mode), 0x0e);

        u8_mode = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_InputCSC_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Curry_HDR_DeGamma_SRAM_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    //0: degamma(linear)
    //1: degamma(TR)
    //2 : dolby_driver()

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Degamma_SRAM_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 2;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
               ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            if (((1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag) && (0x04 == (CFD_HDR_IP_CAPABILITY&0x04))) ||
            ((1 == pstu_Control_Param->u8DoGamutMappingInHDRIP_Flag) && (0x02 == (CFD_HDR_IP_CAPABILITY&0x02))) ||
            ((1 == pstu_Control_Param->u8DoBT2020CLPInHDRIP_Flag) && (0x08 == (CFD_HDR_IP_CAPABILITY&0x08))) ||
            ((1 == pstu_Control_Param->u8DoDLCInHDRIP_Flag) && (0x01 == (CFD_HDR_IP_CAPABILITY&0x01)))||
            ((E_CFD_CFIO_XVYCC_601 == pstu_Control_Param->u8Input_Format) && (0x10 == (CFD_HDR_IP_CAPABILITY&0x10))) ||
            ((E_CFD_CFIO_XVYCC_709 == pstu_Control_Param->u8Input_Format) && (0x10 == (CFD_HDR_IP_CAPABILITY&0x10))) ||
            ((E_CFD_CFIO_SYCC601 == pstu_Control_Param->u8Input_Format) && (0x10 == (CFD_HDR_IP_CAPABILITY&0x10))))
            {
                u8_mode = 1;
            }
            else
            {
                //include pstu_Control_Param->u8DoFull2LimitInHDRIP_Flag = 1
                u8_mode = 0;
            }
        }
        else
        {
            u8_mode = 0;
        }

        u8_mode = (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Degamma_SRAM_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Degamma_SRAM_Mode), 0x03);

        u8_mode = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Degamma_SRAM_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Curry_HDR_3x3_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    //0:3x3(CP1=CP2)
    //1:3x3(CP1,CP2)
    //2:3x3(2020CLtoNCL)
    //3:dolby_driver()
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3x3_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 3;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            if ((1 == pstu_Control_Param->u8DoGamutMappingInHDRIP_Flag) && (0x02 == (CFD_HDR_IP_CAPABILITY&0x02)))
        {
            u8_mode = 1;
        }
            else if ((1 == pstu_Control_Param->u8DoBT2020CLPInHDRIP_Flag) && (0x08 == (CFD_HDR_IP_CAPABILITY&0x08)))
        {
            u8_mode = 2;
        }
        else
        {
            u8_mode = 0;
        }
        }
        else
        {
            u8_mode = 0;
        }

        u8_mode = (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3x3_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3x3_Mode), 0x04);

        u8_mode = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3x3_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Curry_HDR_Gamma_SRAM_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    //0:Gamma(linear)
    //1:Gamma(TR)
    //2:dolby_driver()

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Gamma_SRAM_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 2;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            if (((1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag) && (0x04 == (CFD_HDR_IP_CAPABILITY&0x04))) ||
            ((1 == pstu_Control_Param->u8DoGamutMappingInHDRIP_Flag) && (0x02 == (CFD_HDR_IP_CAPABILITY&0x02))) ||
            ((1 == pstu_Control_Param->u8DoBT2020CLPInHDRIP_Flag) && (0x08 == (CFD_HDR_IP_CAPABILITY&0x08))) ||
            ((1 == pstu_Control_Param->u8DoDLCInHDRIP_Flag) && (0x01 == (CFD_HDR_IP_CAPABILITY&0x01)))||
            ((E_CFD_CFIO_XVYCC_601 == pstu_Control_Param->u8Input_Format) && (0x10 == (CFD_HDR_IP_CAPABILITY&0x10))) ||
            ((E_CFD_CFIO_XVYCC_709 == pstu_Control_Param->u8Input_Format) && (0x10 == (CFD_HDR_IP_CAPABILITY&0x10))) ||
            ((E_CFD_CFIO_SYCC601 == pstu_Control_Param->u8Input_Format) && (0x10 == (CFD_HDR_IP_CAPABILITY&0x10))))
        {
            u8_mode = 1;
        }
            else
        {
                //include pstu_Control_Param->u8DoFull2LimitInHDRIP_Flag = 1
                u8_mode = 0;
            }
        }
        else
        {
            u8_mode = 0;
        }

        u8_mode = (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Gamma_SRAM_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Gamma_SRAM_Mode), 0x03);

        u8_mode = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Gamma_SRAM_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Curry_HDR_OutputCSC_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{

    //0: OFF
    //1~12 : see definition in E_CFD_IP_CSC_PROCESS
    //13 : dolby_driver()

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_OutputCSC_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 13;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            if (((1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag) && (0x04 == (CFD_HDR_IP_CAPABILITY&0x04))) ||
            ((1 == pstu_Control_Param->u8DoGamutMappingInHDRIP_Flag) && (0x02 == (CFD_HDR_IP_CAPABILITY&0x02))) ||
            ((1 == pstu_Control_Param->u8DoBT2020CLPInHDRIP_Flag) && (0x08 == (CFD_HDR_IP_CAPABILITY&0x08))) ||
            ((1 == pstu_Control_Param->u8DoDLCInHDRIP_Flag) && (0x01 == (CFD_HDR_IP_CAPABILITY&0x01)))||
            ((E_CFD_CFIO_XVYCC_601 == pstu_Control_Param->u8Input_Format) && (0x10 == (CFD_HDR_IP_CAPABILITY&0x10))) ||
            ((E_CFD_CFIO_XVYCC_709 == pstu_Control_Param->u8Input_Format) && (0x10 == (CFD_HDR_IP_CAPABILITY&0x10))) ||
            ((E_CFD_CFIO_SYCC601 == pstu_Control_Param->u8Input_Format) && (0x10 == (CFD_HDR_IP_CAPABILITY&0x10))) ||
            (1 == pstu_Control_Param->u8DoForceEnterHDRIP_Flag))
            {
                if (0 == pstu_Control_Param->u8Input_IsRGBBypass)
                {
                    if (0 == pstu_Control_Param->u8DoHDRbypassInHDRIP_Flag)
                    {
                        u8_mode = E_CFD_IP_CSC_RFULL_TO_YFULL;
                    }
                    else //HDRbypass happens
                    {
                        if (E_CFD_CFIO_RANGE_LIMIT == pstu_Control_Param->u8Temp_IsFullRange[1])
                        {
                            u8_mode = E_CFD_IP_CSC_RFULL_TO_YLIMIT;
                        }
                        else
                        {
                            u8_mode = E_CFD_IP_CSC_RFULL_TO_YFULL;
                        }
                    }
                }
                else
                {
                    u8_mode = E_CFD_IP_CSC_OFF;
                }
            }
            else
            {
                u8_mode = E_CFD_IP_CSC_OFF;
            }
        }
        else
        {
            u8_mode = E_CFD_IP_CSC_OFF;
        }

        u8_mode = (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_OutputCSC_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_OutputCSC_Mode), 0x0e);

        u8_mode = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_OutputCSC_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Curry_HDR_Yoffset_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    //0: PQ0_bypass
    //1: dolby_driver()
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Yoffset_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 1;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            u8_mode = 0;
        }
        else
        {
            u8_mode = 0;
        }

        u8_mode = (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Yoffset_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Yoffset_Mode), 0x02);
        u8_mode = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Yoffset_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Curry_HDR_MAXRGB_CSC_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    //0: PQ0_bypass
    //1: dolby_driver()
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_MAXRGB_CSC_Mode&0x80))
    {
        u8_mode = E_CFD_IP_CSC_YFULL_TO_RFULL;
        u8_mode = (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_MAXRGB_CSC_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {

        u8_mode = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_MAXRGB_CSC_Mode&MaskForMode_LB;

        if (u8_mode!=E_CFD_IP_CSC_YFULL_TO_RFULL)
        {
            u8_mode = (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_MAXRGB_CSC_Mode&MaskForMode_HB)|(E_CFD_IP_CSC_YFULL_TO_RFULL&MaskForMode_LB);
            pstu_Control_Param->u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_OVERRANGE;
        }

        //pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Yoffset_Mode), 0x02);
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Curry_HDR_NLM_enable_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    //0: OFF
    //1: ON

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_NLM_enable_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        else if ((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0]))
        {
            if ((1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag) && (0x04 == (CFD_HDR_IP_CAPABILITY&0x04)))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
                //u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();//temporally off ,wait for AC gain driver is ready
        }
        else
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }
        }
        else
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }

        u8_mode = (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_NLM_enable_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_NLM_enable_Mode), 0x02);

        u8_mode = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_NLM_enable_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Curry_HDR_NLM_setting_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    //0: PQ0
    //1: PQ1
    //2: PQ2

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_NLM_setting_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 0;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            if ((1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag) && (0x04 == (CFD_HDR_IP_CAPABILITY&0x04)))
            {
                u8_mode = 1;
            }
            else
            {
                u8_mode = 0;
            }
        }
        else
        {
        u8_mode = 0;
        }

        //force
        u8_mode = 2;

        u8_mode = (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_NLM_setting_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
    pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_NLM_setting_Mode), 0x03);

    u8_mode = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_NLM_setting_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Curry_HDR_ACgain_enable_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    //0: OFF
    //1: ON

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACGain_enable_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        else if ((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0]))
        {
            if ((1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag) && (0x04 == (CFD_HDR_IP_CAPABILITY&0x04)))
            {
                u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
                //u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();//temporally off ,wait for AC gain driver is ready
            }
            else
            {
                u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
            }
        }
        else
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }

        u8_mode = (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACGain_enable_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACGain_enable_Mode), 0x02);
        u8_mode = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACGain_enable_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Curry_HDR_ACgain_setting_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    //0: PQ0_bypass
    //1: ACgain_alg()

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACGain_setting_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 0;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            if ((1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag) && (0x04 == (CFD_HDR_IP_CAPABILITY&0x04)))
            {
                u8_mode = 1;
            }
            else
            {
                u8_mode = 0;
            }
        }
        else
        {
            u8_mode = 0;
        }

        //force
        u8_mode = 0;

        u8_mode = (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACGain_setting_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACGain_setting_Mode), 0x03);
        u8_mode = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACGain_setting_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Curry_HDR_ACE_enable_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    //0: OFF
    //1: ON

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACE_enable_Mode&0x80))
    {
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        else if ((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0]))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }
        else
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }

        u8_mode = (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACE_enable_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACE_enable_Mode), 0x02);

        u8_mode = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACE_enable_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Curry_HDR_ACE_setting_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    //0:PQ0_bypass
    //1:dolby_driver()

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACE_setting_Mode&0x80))
    {

        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 1;
        }
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 0;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        else if ((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0]))
        {
            u8_mode = 0;
        }
        else
        {
            u8_mode = 0;
        }

        u8_mode = (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACE_setting_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACE_setting_Mode), 0x02);

        u8_mode = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACE_setting_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Curry_HDR_Dither1_setting_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{

    //0: PQ0_bypass
    //1: Dolby_mode

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither1_setting_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 1;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            u8_mode = 0;
        }
        else
        {
            u8_mode = 0;
        }

        u8_mode = (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither1_setting_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither1_setting_Mode), 0x02);

        u8_mode = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither1_setting_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Curry_HDR_3DLUT_enable_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    //0: OFF
    //1: ON
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3DLUT_enable_Mode&0x80))
    {
        //Dolby HDR in
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
        ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        //Open HDR in
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
            {
                u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
            }
            else
            {
                if ((0 == pstu_Control_Param->u8Input_IsRGBBypass))
                {
                    u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
                }
                else
                {
                    u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
                }
            }
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        else if ((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0]))
        {
            if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
            {
                u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
            }
            else
            {
                if ((0 == pstu_Control_Param->u8Input_IsRGBBypass))
                {
                    if(E_CFD_INPUT_SOURCE_HDMI == pstu_Control_Param->u8Input_Source && E_CFIO_MODE_SDR == pstu_Control_Param->u8Input_HDRMode)
                    {
                        u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
                    }
                    else
                    {
                        u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
                    }
                }
                else
                {
                    u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
                }
            }
        }
        else
        {
            if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
            {
                u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
            }
            else
            {
                if ((0 == pstu_Control_Param->u8Input_IsRGBBypass))
                {
                    u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
                }
                else
                {
                    u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
                }
            }
        }

        //force off
        u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();

        u8_mode = (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3DLUT_enable_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }//end of if(0x80 == (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3DLUT_enable_Mode&0x80))
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3DLUT_enable_Mode), 0x02);
        u8_mode = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3DLUT_enable_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Curry_HDR_3DLUT_SRAM_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    //0: dolby_driver()

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3DLUT_SRAM_Mode&0x80))
    {

        if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
        u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        else
        u8_mode = 1; //full to limit

        u8_mode = (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3DLUT_SRAM_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3DLUT_SRAM_Mode), 0x02);

        u8_mode = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3DLUT_SRAM_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Curry_HDR_3DLUT_setting_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    //0: PQ0_bypass
    //1: Dolby_mode
    //2: full to limit

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3DLUT_setting_Mode&0x80))
    {

        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 1;
        }
        //
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
            u8_mode = 0;
            else
                u8_mode = 2; //full to limit
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        else if ((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0]))
        {
            if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
                u8_mode = 0;
            else
                u8_mode = 2; //full to limit
        }
        else
        {
            if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
                u8_mode = 0;
            else
                u8_mode = 2; //full to limit
        }

        //force 0
        u8_mode = 0;
        u8_mode = (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3DLUT_setting_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3DLUT_setting_Mode), 0x02);

        u8_mode = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3DLUT_setting_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Curry_HDR_444to422_enable_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    //0: OFF
    //1: ON
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_444to422_enable_Mode&0x80))
    {

        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 1;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            u8_mode = 0;
        }
        else
        {
            u8_mode = 0;
        }

        //force off
        u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        u8_mode = (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_444to422_enable_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_444to422_enable_Mode), 0x02);

        u8_mode = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_444to422_enable_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Curry_HDR_Dither2_enable_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    //0: OFF
    //1: ON
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither2_enable_Mode&0x80))
    {

        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 1;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
                u8_mode = 0; //off
            else
                u8_mode = 0; //on
        }
        else
        {
            if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
                u8_mode = 0; //off
            else
                u8_mode = 0; //on
        }

        //force off
        u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        u8_mode = (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither2_enable_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither2_enable_Mode), 0x02);

        u8_mode = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither2_enable_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Curry_HDR_Dither2_setting_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    //0: PQ0_bypass
    //1: Dolby_mode

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither2_setting_Mode&0x80))
    {

        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 1;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            u8_mode = 0;
        }
        else
        {
            u8_mode = 0;
        }

        //force off
        u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        u8_mode = (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither2_setting_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither2_setting_Mode), 0x02);

        u8_mode = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither2_setting_Mode;
    }

    return u8_mode;
}

//end of Curry IP write register function =========================================================

//Due to Curry's ygain_offset issue
MS_U16 MS_Cfd_CurryPatch_TMO_For_LimitIn(MS_U8 *TMO_curve, MS_U8 *TMO_curve_patch)
{
    MS_U16 status = 0;

    return status;
}

MS_U8 MS_Cfd_Curry_SDR_IP2_CSC_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
    MS_U8 u8_mode = 0;

    //MS_U8 u8curr_DataFormat = pstu_Control_Param->u8Input_DataFormat;
    //MS_U8 u8curr_IsFullRange = pstu_Control_Param->u8Input_IsFullRange;
    //MS_U8 u8curr_IsRGBBypass = pstu_Control_Param->u8Input_IsRGBBypass;
    //MS_U8 u8curr_DoPathFullRange_Flag = pstu_Control_Param->u8DoPathFullRange_Flag;

    MS_U8 u8curr_DataFormat = pstu_Control_Param->u8Temp_DataFormat[1];
    MS_U8 u8curr_IsFullRange = pstu_Control_Param->u8Temp_IsFullRange[1];
    MS_U8 u8curr_IsRGBBypass = pstu_Control_Param->u8Input_IsRGBBypass;
    MS_U8 u8curr_DoPathFullRange_Flag = pstu_Control_Param->u8DoPathFullRange_Flag;

#if 0
    if(E_CFD_CFIO_XVYCC_601 == pstu_Control_Param->u8Input_Format ||E_CFD_CFIO_XVYCC_709 == pstu_Control_Param->u8Input_Format)
    {
        u8_mode = E_CFD_IP_CSC_OFF;
    }
#endif

    if (E_CFD_MC_FORMAT_RGB == u8curr_DataFormat)
    {
        if(E_CFD_CFIO_RANGE_LIMIT == u8curr_IsFullRange && 0 == u8curr_IsRGBBypass)
        {
            if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
            u8_mode = E_CFD_IP_CSC_RLIMIT_TO_YFULL;
            else
            u8_mode = E_CFD_IP_CSC_RLIMIT_TO_YLIMIT;
        }
        else if(E_CFD_CFIO_RANGE_LIMIT == u8curr_IsFullRange && 1 == u8curr_IsRGBBypass)
        {
            if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
            u8_mode = E_CFD_IP_CSC_RLIMIT_TO_RFULL;
            else
            u8_mode = E_CFD_IP_CSC_OFF;
        }
        else if(E_CFD_CFIO_RANGE_FULL== u8curr_IsFullRange && 0 == u8curr_IsRGBBypass)
        {
            if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
            u8_mode = E_CFD_IP_CSC_RFULL_TO_YFULL;
            else
            u8_mode = E_CFD_IP_CSC_RFULL_TO_YLIMIT;
        }
        else if(E_CFD_CFIO_RANGE_FULL== u8curr_IsFullRange && 1 == u8curr_IsRGBBypass)
        {
            if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
            u8_mode = E_CFD_IP_CSC_OFF;
            else
            u8_mode = E_CFD_IP_CSC_RFULL_TO_RLIMIT;
        }
        else
        {
            u8_mode = E_CFD_IP_CSC_OFF;
        }
    }
    else if ( E_CFD_MC_FORMAT_RGB != u8curr_DataFormat)
    {
            if(E_CFD_CFIO_RANGE_LIMIT == u8curr_IsFullRange && 0 == u8curr_IsRGBBypass)
            {
                if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
                u8_mode = E_CFD_IP_CSC_YLIMIT_TO_YFULL;
                else
                u8_mode = E_CFD_IP_CSC_OFF;
            }
            else if(E_CFD_CFIO_RANGE_LIMIT == u8curr_IsFullRange && 1 == u8curr_IsRGBBypass)
            {
                if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
                u8_mode = E_CFD_IP_CSC_YLIMIT_TO_YFULL;
                else
                u8_mode = E_CFD_IP_CSC_OFF;
            }
            else if(E_CFD_CFIO_RANGE_FULL== u8curr_IsFullRange && 0 == u8curr_IsRGBBypass)
            {
                if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
                u8_mode = E_CFD_IP_CSC_OFF;
                else
                u8_mode = E_CFD_IP_CSC_YFULL_TO_YLIMIT;
            }
            else if(E_CFD_CFIO_RANGE_FULL== u8curr_IsFullRange && 1 == u8curr_IsRGBBypass)
            {
                if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
                u8_mode = E_CFD_IP_CSC_OFF;
                else
                u8_mode = E_CFD_IP_CSC_YFULL_TO_YLIMIT;
            }
            else
            {
                u8_mode = E_CFD_IP_CSC_OFF;
            }
    }
    else
    {
        u8_mode = E_CFD_IP_CSC_OFF;
    }

    if(0x80 == (pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_IP2_CSC_Mode&0x80))
    {
        u8_mode = (pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_IP2_CSC_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_IP2_CSC_Mode), 0x0D);

        u8_mode = pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_IP2_CSC_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Curry_SDR_VIP_CM_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{

    MS_U8 u8_mode = 0;

    MS_U8 u8curr_DataFormat = pstu_Control_Param->u8Output_DataFormat;
    MS_U8 u8curr_IsFullRange = pstu_Control_Param->u8Output_IsFullRange;
    MS_U8 u8curr_IsRGBBypass = pstu_Control_Param->u8Input_IsRGBBypass;
    MS_U8 u8curr_DoPathFullRange_Flag = pstu_Control_Param->u8DoPathFullRange_Flag;

    if(E_CFD_CFIO_RANGE_LIMIT == u8curr_IsFullRange && (pstu_Control_Param->u8Output_Format <= E_CFD_CFIO_RESERVED_START && pstu_Control_Param->u8Output_Format >= E_CFD_CFIO_XVYCC_601))
    {
        printf("Error code =%d!!! This ouput format can't support limit range output [ %s  , %d]\n",E_CFD_MC_ERR_WRONGOUTPUTSOURCE, __FUNCTION__,__LINE__);
        //return E_CFD_MC_ERR_WRONGOUTPUTSOURCE;
        u8_mode = E_CFD_IP_CSC_OFF;
    }
    //else if ((E_CFD_MC_FORMAT_RGB == pstu_Control_Param->u8Temp_DataFormat[1]) &&  (1== pstu_Control_Param->u8Input_IsRGBBypass))
    //{
    //  u8_mode = E_CFD_IP_CSC_OFF;
    //}
    //any input & RGB limit out
    else if (E_CFD_MC_FORMAT_RGB == u8curr_DataFormat && E_CFD_CFIO_RANGE_LIMIT == u8curr_IsFullRange)
    {
        if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
        u8_mode = E_CFD_IP_CSC_YFULL_TO_RLIMIT;
        else
        u8_mode = E_CFD_IP_CSC_YLIMIT_TO_RLIMIT;
    }
    //RGB full out
    else if (E_CFD_MC_FORMAT_RGB == u8curr_DataFormat && E_CFD_CFIO_RANGE_FULL == u8curr_IsFullRange)
    {
        if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
        u8_mode = E_CFD_IP_CSC_YFULL_TO_RFULL;
        else
        u8_mode = E_CFD_IP_CSC_YLIMIT_TO_RFULL;
    }
    //YUV limit
    else if (E_CFD_MC_FORMAT_RGB != u8curr_DataFormat && E_CFD_CFIO_RANGE_LIMIT == u8curr_IsFullRange)
    {
        if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
        u8_mode = E_CFD_IP_CSC_YFULL_TO_YLIMIT;
        else
        u8_mode = E_CFD_IP_CSC_OFF;
    }
    //YUV full
    else if (E_CFD_MC_FORMAT_RGB != u8curr_DataFormat && E_CFD_CFIO_RANGE_FULL == u8curr_IsFullRange)
    {
        if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
        u8_mode = E_CFD_IP_CSC_OFF;
        else
        u8_mode = E_CFD_IP_CSC_YLIMIT_TO_YFULL;
    }
    else
    {
        u8_mode = E_CFD_IP_CSC_OFF;
    }

    if(0x80 == (pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_VIP_CM_Mode&0x80))
    {
        u8_mode = (pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_VIP_CM_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_VIP_CM_Mode), 0x0D);

        u8_mode = pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_VIP_CM_Mode;

        }

    return u8_mode;
}

MS_U8 MS_Cfd_Curry_SDR_Conv420_CSC_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
    MS_U8 u8_mode = 0;

    MS_U8 u8curr_DataFormat = pstu_Control_Param->u8Output_DataFormat;
    MS_U8 u8curr_IsFullRange = pstu_Control_Param->u8Output_IsFullRange;
    MS_U8 u8curr_IsRGBBypass = pstu_Control_Param->u8Input_IsRGBBypass;
    MS_U8 u8curr_DoPathFullRange_Flag = pstu_Control_Param->u8DoPathFullRange_Flag;

    if(E_CFD_CFIO_RANGE_LIMIT == u8curr_IsFullRange && (pstu_Control_Param->u8Output_Format <= E_CFD_CFIO_RESERVED_START && pstu_Control_Param->u8Output_Format >= E_CFD_CFIO_XVYCC_601))
    {
        printf("Error code =%d!!! This ouput format can't support limit range output [ %s  , %d]\n",E_CFD_MC_ERR_WRONGOUTPUTSOURCE, __FUNCTION__,__LINE__);
        //return E_CFD_MC_ERR_WRONGOUTPUTSOURCE;
        u8_mode = E_CFD_IP_CSC_OFF;
    }
    //else if ((E_CFD_MC_FORMAT_RGB == pstu_Control_Param->u8Temp_DataFormat[1]) &&  (1== pstu_Control_Param->u8Input_IsRGBBypass))
    //{
    //  u8_mode = E_CFD_IP_CSC_OFF;
    //}
    //any input & RGB limit out
    else if (E_CFD_MC_FORMAT_RGB == u8curr_DataFormat && E_CFD_CFIO_RANGE_LIMIT == u8curr_IsFullRange)
    {
        if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
        u8_mode = E_CFD_IP_CSC_YFULL_TO_RLIMIT;
        else
        u8_mode = E_CFD_IP_CSC_YLIMIT_TO_RLIMIT;
    }
    //RGB full out
    else if (E_CFD_MC_FORMAT_RGB == u8curr_DataFormat && E_CFD_CFIO_RANGE_FULL == u8curr_IsFullRange)
    {
        if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
        u8_mode = E_CFD_IP_CSC_YFULL_TO_RFULL;
        else
        u8_mode = E_CFD_IP_CSC_YLIMIT_TO_RFULL;
    }
    //YUV limit
    else if (E_CFD_MC_FORMAT_RGB != u8curr_DataFormat && E_CFD_CFIO_RANGE_LIMIT == u8curr_IsFullRange)
    {
        if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
        u8_mode = E_CFD_IP_CSC_YFULL_TO_YLIMIT;
        else
        u8_mode = E_CFD_IP_CSC_OFF;
    }
    //YUV full
    else if (E_CFD_MC_FORMAT_RGB != u8curr_DataFormat && E_CFD_CFIO_RANGE_FULL == u8curr_IsFullRange)
    {
        if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
        u8_mode = E_CFD_IP_CSC_OFF;
        else
        u8_mode = E_CFD_IP_CSC_YLIMIT_TO_YFULL;
    }
    else
    {
        u8_mode = E_CFD_IP_CSC_OFF;
        }

    if(0x80 == (pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_Conv420_CM_Mode&0x80))
    {
        //force off
        u8_mode = 0;
        u8_mode = (pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_Conv420_CM_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Curry_CheckModes(&(pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_Conv420_CM_Mode), 0x0D);

        u8_mode = pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_Conv420_CM_Mode;

        }

    return u8_mode;
}

MS_U8 MS_Cfd_Curry_SDR_HDMIRX_CSC_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
    return 0;
}

//
//@Param
//input :
//STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param
//STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input

//Output :
//temp[0] : for max
//temp[1] : for med
//temp[2] : for min
//temp[3] : updating flag for max/min
              //need to update driver global variables or not

//temp[4] : updating flag for med
              //need to update driver global variables or not


void MS_Cfd_TMO_parameter_update_source(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param ,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input, MS_U16 *u16temp)
{
    MS_U8 u8FindFlag;

    //0: match done
    //1: continue to match
    u8FindFlag = 0;

    //for source Max/Min

    //check auto/force
    if (0 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceUserModeEn)
    {
        pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceUserMode = 0;
    }

    //check this first
    if ((0 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceStatus) && (0 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceUserMode))
    {
        pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceStatus = 0;
        u8FindFlag = 1;
    }
    else
    {
        if (0 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceUserMode)
        {
            pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceUserMode = 1;
        }
        u8FindFlag = 0;
    }

    if ((1 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceUserMode) && (0 == u8FindFlag))
    {
        if (1 == pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_TMO_param_SetbyDriver)
        {
             pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceStatus = 1;
             u8FindFlag = 1;
        }
        else
        {
             pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceUserMode = 2;
             u8FindFlag = 0;
        }
    }

    if ((2 <= pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceUserMode) && (0 == u8FindFlag))
    {
        pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceStatus = 2;
        u8FindFlag = 1;
    }

    //for source Med
    pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceUserMode = 1;
    u8FindFlag = 0;

    if ((1 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceUserMode) && (0 == u8FindFlag))
    {
        if (1 == pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_TMO_param_SetbyDriver)
        {
            pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceStatusM = 1;
            u8FindFlag = 1;
        }
        else
        {
            pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceUserMode = 2;
            u8FindFlag = 0;
        }
    }

    if ((2 <= pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceUserMode) && (0 == u8FindFlag))
    {
        pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceStatusM = 2;
        u8FindFlag = 1;
    }

    if ((0 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceStatus) || (2 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceStatus))
    {
        //g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmax      = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax;
        //g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmin      = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMin;

        u16temp[0] = pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMax;
        u16temp[2] = pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMin;
        u16temp[3] = 1;

    }
    else if (1 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceStatus)
    {
        //no need to update
        //pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMax = pstu_TMO_Input->stu_Kano_TMOIP_Param.u16HDR_TMO_Tmax;
        u16temp[3] = 0;
    }

    if (2 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceStatusM)
    {
        //g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmed      = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMin;
        u16temp[1] = pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMed;
        u16temp[4] = 1;
    }
    else
    {
        u16temp[4] = 0;
    }
    //no need to update for 1
}


void MS_Cfd_TMO_parameter_update_target(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param ,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input, MS_U16 *u16temp)
{
    MS_U8 u8FindFlag;

    //0: match done
    //1: continue to match
    u8FindFlag = 0;

    //for source Max/Min

    //check auto/force
    if (0 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetUserModeEn)
    {
        pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetUserMode = 0;
    }

    //check this first
    if ((0 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetStatus) && (0 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetUserMode))
    {
        pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetStatus = 0;
        u8FindFlag = 1;
    }
    else
    {
        if (0 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetUserMode)
        {
            pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetUserMode = 1;
        }
        u8FindFlag = 0;
    }

    if ((1 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetUserMode) && (0 == u8FindFlag))
    {
        if (1 == pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_TMO_param_SetbyDriver)
        {
             pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetStatus = 1;
             u8FindFlag = 1;
        }
        else
        {
             pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetUserMode = 2;
             u8FindFlag = 0;
        }
    }

    if ((2 <= pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetUserMode) && (0 == u8FindFlag))
    {
        pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetStatus = 2;
        u8FindFlag = 1;
    }

    //for Target Med
    pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetUserMode = 1;
    u8FindFlag = 0;

    if ((1 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetUserMode) && (0 == u8FindFlag))
    {
        if (1 == pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_TMO_param_SetbyDriver)
        {
            pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetStatusM = 1;
            u8FindFlag = 1;
        }
        else
        {
            pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetUserMode = 2;
            u8FindFlag = 0;
        }
    }

    if ((2 <= pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetUserMode) && (0 == u8FindFlag))
    {
        pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetStatusM = 2;
        u8FindFlag = 1;
    }

    if ((0 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetStatus) || (2 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetStatus))
    {
        //g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmax      = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax;
        //g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmin      = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMin;

        u16temp[0] = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax;
        if ((pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMin >= 1) && (pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMinFlag==0))
        {
            u16temp[2] = 10000;
        }
        else
        {
            u16temp[2] = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMin;
        }
        u16temp[3] = 1;

    }
    else if (1 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetStatus)
    {
        //no need to update
#if (NowHW == Kano)
        pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax = pstu_TMO_Input->stu_Kano_TMOIP_Param.u16HDR_TMO_Tmax;
#endif
        u16temp[3] = 0;
    }

    if (2 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetStatusM)
    {
        //g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmed      = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMin;
        u16temp[1] = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMed;
        u16temp[4] = 1;
    }
    else
    {
        u16temp[4] = 0;
    }
    //no need to update for 1
}



MS_U16 MS_Cfd_Curry_TMO_Control(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{
    //E_CFD_MC_ERR
    MS_U8 u8_check_status = E_CFD_MC_ERR_NOERR;
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_BOOL bFuncEn,bRegWriteEn;
    MS_U8 u8Mode;
    MS_U16 u16temp[5] = {0};
    ST_HDR_UVC stUVC;
    ST_HDR_TMO_SETTINGS stTMO;
    memset(&stUVC,0,sizeof(ST_HDR_UVC));
    memset(&stTMO,0,sizeof(ST_HDR_TMO_SETTINGS));
    //TMO_Decision_Tree_Start
    u8Mode = 0;
    //bypass mode
if (0 == pstu_Control_Param->u8Input_HDRIPMode)
{
        function1(pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_TMO_curve_enable_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_TMO_curve_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_TMO_curve_setting_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_UVC_setting_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        }
    //normal mode
        else
        {
        pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_TMO_curve_enable_Mode = MS_Cfd_Curry_HDR_TMO_Curve_enable_Decision(pstu_Control_Param,pstu_TMO_Input);
        pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_TMO_curve_Mode = MS_Cfd_Curry_HDR_TMO_Curve_Mode_Decision(pstu_Control_Param,pstu_TMO_Input);
        pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_TMO_curve_setting_Mode = MS_Cfd_Curry_HDR_TMO_Curve_setting_Decision(pstu_Control_Param,pstu_TMO_Input);
        pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_UVC_setting_Mode = MS_Cfd_Curry_HDR_UVC_setting_Decision(pstu_Control_Param,pstu_TMO_Input);
        }

#if ((CFD_SW_VERSION >= 0x00000014) && (CFD_SW_RETURN_ST_VERSION >= 1))

if (0x00 != FunctionMode(pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_TMO_curve_enable_Mode))
{
    pstu_Control_Param->u8VideoADFlag = 1;
}

#endif

    u16_check_status = Mapi_Cfd_Curry_TMOIP_Param_Check(&(pstu_TMO_Input->stu_Curry_TMOIP_Param));

    // Input_to_Physical_Layer_by_Tony_start
#if RealChip
    //g_DlcParameters.u8Dlc_Mode = pstu_TMO_Input->stu_Manhattan_TMOIP_Param.u8HDR_TMO_curve_Mode;
    if (1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag)
{
        //printf("1 TMO control TgtMax:%d\n",pstu_TMO_Input->stu_CFD_TMO_Param.u16TgtMax);
        //g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TgtMinFlag = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMinFlag;
        //g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TgtMaxFlag = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMaxFlag;
        }

    //if (1 == g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode)
    {
        //g_HDRinitParameters.DLC_HDRNewToneMappingData.u16TgtMin = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMin;  // 0.05
        //g_HDRinitParameters.DLC_HDRNewToneMappingData.u16TgtMax = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax; // 300
        //g_HDRinitParameters.DLC_HDRToneMappingData.u16Smin =  pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMin;
        //g_HDRinitParameters.DLC_HDRToneMappingData.u16Smax = pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMax;
        //printf("\033[1;35m###[Brian][%s][%d]### old u16TgtMax = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,g_HDRinitParameters.DLC_HDRToneMappingData.u16Tmax);
        //printf("\033[1;35m###[Brian][%s][%d]### old u16TgtMin = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,g_HDRinitParameters.DLC_HDRToneMappingData.u16Tmin);
        //backward compatibility
        if (2 > pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetRefer_Mode)
        {
        g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmax = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax;
        g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmin = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMin;
        g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TgtMinFlag = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMinFlag;
        }
        else if (2 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetRefer_Mode)
        {
            MS_Cfd_TMO_parameter_update_source(pstu_Control_Param ,pstu_TMO_Input, u16temp);

            if (1 == u16temp[3])
            {
                //only update Smin here
                //g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smax = u16temp[0];
                g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smin = u16temp[2];
            }

            if (1 == u16temp[4])
            {
                g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smed = u16temp[1];
            }

            //for target
            MS_Cfd_TMO_parameter_update_target(pstu_Control_Param ,pstu_TMO_Input, u16temp);

            g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TgtMinFlag = 1;
            if (1 == u16temp[3])
            {
                //only update Tmax here
                g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmax = u16temp[0];
                g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmin = u16temp[2];
            }

            if (1 == u16temp[4])
            {
                g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmed = u16temp[1];
            }
        } //else if (2 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetRefer_Mode)
        else
        {
            g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmax      = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax;
            g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmin      = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMin;
            g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TgtMinFlag = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMinFlag;
        }

        if (E_CFIO_MODE_HDR2 == pstu_Control_Param->u8Temp_HDRMode[0]) //PQ uses TMO configures from CFD values
        {
            g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode = 1;
        }
        else if (E_CFIO_MODE_HDR3 == pstu_Control_Param->u8Temp_HDRMode[0]) //HLG uses TMO configures in TMO
        {
            g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode = 2;
        }
        else //use TMO default values
        {
            g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode = 0;
        }

        //printf("\033[1;35m###[Brian][%s][%d]### new u16Tmax = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmax);
        //printf("\033[1;35m###[Brian][%s][%d]### new u16Tmin = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmin);
        //printf("\033[1;35m###[Brian][%s][%d]### pstu_Control_Param->u8Temp_HDRMode[0] = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,pstu_Control_Param->u8Temp_HDRMode[0]);
        //printf("\033[1;35m###[Brian][%s][%d]### g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode] = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode);
        //g_HDRinitParameters.DLC_HDRToneMappingData.u16Smed = pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMed;
        //g_HDRinitParameters.DLC_HDRToneMappingData.u16Tmed = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMed;
    }
#endif
    // Input_to_Physical_Layer_by_Tony_start
    //produce TMO curves...
    //B02 TMO start//

    u8Mode = FunctionMode(pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_TMO_curve_Mode);
#if RealChip
    g_DlcParameters.u8Tmo_Mode = u8Mode;
    u8Mhal_xc_TMO_mode = u8Mode;
    //printf("\033[1;35m###[Brian][%s][%d]### g_DlcParameters.u8Tmo_Mode = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,g_DlcParameters.u8Tmo_Mode);
    //printf("\033[1;35m###[Brian][%s][%d]### u8Mhal_xc_TMO_mode = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,u8Mhal_xc_TMO_mode);
#endif
    if(2 != u8Mode) //not Dolby
    {
        bFuncEn = FunctionMode(pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_TMO_curve_enable_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_TMO_curve_setting_Mode);
    u8Mode = FunctionMode(pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_TMO_curve_setting_Mode);
        HdrTmoSettings(         bFuncEn,bRegWriteEn,u8Mode, &stTMO);
    }
    else
    {
        //u8_check_status = DolbyDriver();
    }
    //B02 TMO End//

    //B04 UVC Start//
    bFuncEn = FunctionMode(pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_UVC_setting_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_UVC_setting_Mode);
    u8Mode = FunctionMode(pstu_TMO_Input->stu_Curry_TMOIP_Param.u8HDR_UVC_setting_Mode);
    if(3 != u8Mode)
    {
        HdrUvcWrite2Register(   bFuncEn,bRegWriteEn,u8Mode, &stUVC);
    }
    else
    {
        //u8_check_status = DolbyDriver();
    }
    //B04 UVC Edn//
    return u16_check_status ;
}

MS_U16 MS_Cfd_Curry_HDRIP_WriteRegister_DebugMode(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{
    MS_U8 u8CurrentIP_Vars,u8CurrentIP_Vars2,u8GammutMode;
    MS_U16 u16Max_luma_codes;
    MS_U8 u8_check_status = E_CFD_MC_ERR_NOERR;
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_U16 u16temp;
    MS_BOOL bFuncEn,bRegWriteEn;
    MS_U8 u8Mode;
    MS_U8 u8IPindex = 0,u8IPmode = 0;

    ST_HDR_B107 stYoffset;
    ST_HDR_NLM stNLM;
    ST_HDR_AC_GAIN stAcGain;
    ST_HDR_ACE stACE;
    ST_HDR_UVC stUVC;
    ST_HDR_DITHER1 stDither1;
    ST_HDR_3DLUT_SETTINGS st3DLIT;
    ST_HDR_DITHER2 stDither;
    memset(&stYoffset, 0, sizeof(ST_HDR_B107));
    memset(&stNLM, 0, sizeof(ST_HDR_NLM));
    memset(&stAcGain, 0, sizeof(ST_HDR_AC_GAIN));
    memset(&stACE, 0, sizeof(ST_HDR_ACE));
    memset(&stUVC, 0, sizeof(ST_HDR_UVC));
    memset(&stDither1, 0, sizeof(ST_HDR_DITHER1));
    memset(&st3DLIT, 0, sizeof(ST_HDR_3DLUT_SETTINGS));
    memset(&stDither, 0, sizeof(ST_HDR_DITHER2));
#if RealChip
    u8IPindex = msReadByte(REG_SC_BK30_0E_H);
    u8IPmode = msReadByte(REG_SC_BK30_0E_L);
#endif
//HDR enable start//
    if(u8IPindex == 17)
    {
        if(u8IPmode == 0x80)
        {
            bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_enable_Mode);
            bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_enable_Mode);
            u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_enable_Mode);
        }
        else
        {
            bFuncEn = FunctionMode(u8IPmode);
            bRegWriteEn = WriteRegsiterFlag(u8IPmode);
            u8Mode = FunctionMode(u8IPmode);
        }
        HdrEnable(bFuncEn,bRegWriteEn,u8Mode);
    }
//HDR enable end//

//Composer Mode start//
    pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Composer_Mode = MS_Cfd_Curry_HDR_Composer_Decision(pstu_Control_Param, pstu_HDRIP_Param);
//Composer Mode End//

//B01 Enable start//
    if(u8IPindex == 18)
    {
        if(0x80 == (u8IPmode &0x80))
        {
            bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Module1_enable_Mode);
            bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Module1_enable_Mode);
            u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Module1_enable_Mode);
        }
        else
        {
            bFuncEn = FunctionMode(u8IPmode);
            bRegWriteEn = WriteRegsiterFlag(u8IPmode);
            u8Mode = FunctionMode(u8IPmode);
        }
        HdrB01En(               bFuncEn,bRegWriteEn,u8Mode);
    }
//B01 Enable End//

//B01-01 Start //
    //MS_U8 HdrCupB0101(            bFuncEn,bRegWriteEn,u8Mode, ST_HDR_CUP* Struct);
//B01-01 End //

//B01-02 InCSC Start//
    if(u8IPindex == 19)
    {
    if (1 == pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_InputCSC_Manual_Vars_en)
    {
        u8CurrentIP_Vars = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_InputCSC_MC;
    }
    else
    {
        u8CurrentIP_Vars = pstu_Control_Param->u8TempMatrixCoefficients[0];
        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_InputCSC_MC = pstu_Control_Param->u8TempMatrixCoefficients[0];
    }

        if(0x80 == (u8IPmode &0x80))
        {

        }
        else
        {
            pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_InputCSC_Mode = u8IPmode;
        }

    u8_check_status = CSC((pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_InputCSC_Mode&MaskForMode_LB),u8CurrentIP_Vars,pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_InputCSC_Ratio1,3,1,E_CSC_MASERTATI_HDR_INPUT);
    if(0 == u8_check_status)
    {
            printf("Error code =%d!!! B01-02 InputCSC Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    }
//B01-02  InCSC End//

//B01-03 Degamma Start//
    if(u8IPindex == 20)
    {
    if(1 == pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_DeGamma_Manual_Vars_en)
    {
        u8CurrentIP_Vars = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Degamma_TR;
    }
    else
    {
        u8CurrentIP_Vars = pstu_Control_Param->u8TempTransferCharacterstics[0];
        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Degamma_TR = pstu_Control_Param->u8TempTransferCharacterstics[0];
    }

        if(0x80 == (u8IPmode &0x80))
        {

        }
        else
        {
            u8CurrentIP_Vars = u8IPmode;
        }

    u16Max_luma_codes = MS_Cfd_Luminance_To_PQCode(pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax, pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMaxFlag);

    u16Max_luma_codes = ((MS_U32)u16Max_luma_codes*0xff00)/(0x3ff);
    pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u16HDR_Degamma_Max_Lum = u16Max_luma_codes;


        ST_DEGAMMA_PARAMETERS_EXTENSION st_degamma_extension;
        ST_DEGAMMA_PARAMETERS_EXTENSION* pt_degamma_extension = &st_degamma_extension;
        pt_degamma_extension->dePQclamp_en = DePQClamp_EN;

#if (1 == DePQClamp_EN)
        u16temp = MS_Cfd_Luminance_To_PQCode(pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMax, 0);
        pt_degamma_extension->dePQclamp_value = ((MS_U32)u16temp*0xff00)/(0x3ff);
#else
        //give maximum values
        pt_degamma_extension->dePQclamp_value = 0xff00;
#endif

    u8_check_status = deGamma(u8CurrentIP_Vars, u16Max_luma_codes, pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Degamma_Ratio1
        ,pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u16HDR_Degamma_Ratio2
        , pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Degamma_Lut_En
        , pstu_HDRIP_Param->stu_Curry_HDRIP_Param.pu32HDR_Degamma_Lut_Address
        , pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u16HDR_Degamma_Lut_Length
        , 2,pstu_Control_Param->u8Temp_Format[0] , 1, pt_degamma_extension);
    if(0 == u8_check_status)
    {
            printf("Error code =%d!!!B01-03 Degamma Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    }
//B01-03 Degamma End//

//B01-04 HDR 3x3 Start//
    if(u8IPindex == 21)
    {
    if (1 == pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3x3_Manual_Vars_en)
    {
        u8CurrentIP_Vars = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3x3_InputCP;
        u8CurrentIP_Vars2 = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3x3_OutputCP;
    }
    else
    {
        u8CurrentIP_Vars = pstu_Control_Param->u8TempColorPriamries[0];
        u8CurrentIP_Vars2 =  pstu_Control_Param->u8TempColorPriamries[1];

        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3x3_InputCP = pstu_Control_Param->u8TempColorPriamries[0];
        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3x3_OutputCP = pstu_Control_Param->u8TempColorPriamries[1];
    }

    /*#if 0
    if (E_CFD_CFIO_CP_PANEL == u8CurrentIP_Vars2)
    {
        u8GammutMode = 0;
    }
    else
    {
        u8GammutMode = 1;
    }
    #endif */

    //for HDR
    u8GammutMode = 1;
    if(0x80 == (u8IPmode &0x80))
        {
        }
        else
        {
            u8CurrentIP_Vars2 = u8IPmode;
        }
    u8_check_status = gamutMapping(u8CurrentIP_Vars, u8GammutMode,
                               //&(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.pstu_Panel_Param->stu_Cfd_Panel_ColorMetry)
                               pstu_Control_Param->pstu_Panel_Param_Colorimetry,
                            u8CurrentIP_Vars2,pstu_Control_Param->u8Temp_Format[0],
                           pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u16HDR_3x3_Ratio2,1,1);

    if(0 == u8_check_status)
    {
            printf("Error code =%d!!!B01 -04 GamutMapping Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    }
//B01-04 HDR 3x3 End//

//B01-05 Gamma Start//
    if(u8IPindex == 22)
    {
    if (1 == pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Gamma_Manual_Vars_en)
    {
        u8CurrentIP_Vars = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Gamma_TR;
    }
    else
    {
        u8CurrentIP_Vars = pstu_Control_Param->u8TempTransferCharacterstics[1];
        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Gamma_TR = pstu_Control_Param->u8TempTransferCharacterstics[1];
    }

    u16Max_luma_codes = 0xff00;

    //if (u8CurrentIP_Vars == 16)
    //{
    //  u8CurrentIP_Vars = 6;
    //}
    if(0x80 == (u8IPmode &0x80))
        {
        }
        else
        {
            u8CurrentIP_Vars = u8IPmode;
        }
    u8_check_status = gamma(u8CurrentIP_Vars,u16Max_luma_codes,0,0,
                    pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Gamma_Lut_En,
                    pstu_HDRIP_Param->stu_Curry_HDRIP_Param.pu32HDR_Gamma_Lut_Address,
                    pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u16HDR_Gamma_Lut_Length,
                    1,pstu_Control_Param->u8Temp_Format[1],1);


    if(0 == u8_check_status)
    {
            printf("Error code =%d!!!B01-05 Gamma Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    }
//B01-05 Gamma End//

//B01-06 OutCSC start//
    if(u8IPindex == 23)
    {
    if (1 == pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_OutputCSC_Manual_Vars_en)
    {
        u8CurrentIP_Vars2 = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_OutputCSC_MC;
    }
    else
    {
        u8CurrentIP_Vars2 = pstu_Control_Param->u8TempMatrixCoefficients[1];
        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_OutputCSC_MC = pstu_Control_Param->u8TempMatrixCoefficients[1];
    }

    if(0x80 == (u8IPmode &0x80))
        {
        }
        else
        {
            pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_OutputCSC_Mode = u8IPmode;
        }
    u8_check_status = CSC((pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_OutputCSC_Mode&MaskForMode_LB)
                                        ,u8CurrentIP_Vars2,pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_OutputCSC_Ratio1
                                        ,3,1,E_CSC_MASERTATI_HDR_OUTPUT);
    if(0 == u8_check_status)
    {
            printf("Error code =%d!!! B01-06 OutCSC Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    }
//B01-06 OutCSC End//

//B01-07 Yoffset Start//
    if(u8IPindex == 24)
    {
    if(0x80 == (u8IPmode &0x80))
        {
    bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Yoffset_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Yoffset_Mode);
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Yoffset_Mode);
        }
        else
        {
            bFuncEn = FunctionMode(u8IPmode);
            bRegWriteEn = WriteRegsiterFlag(u8IPmode);
            u8Mode = FunctionMode(u8IPmode);
        }

    if(0 == u8Mode)
    {
        b107Write2Register(     bFuncEn,bRegWriteEn,u8Mode, &stYoffset);
    }
    else
    {
        //u8_check_status = DolbyDriver();
    }

    if(0 == u8_check_status)
    {
            printf("Error code =%d!!! B01-07 Yoffset Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    }
//B01-07 Yoffset End//

//M IP NLM start//
    if(u8IPindex == 25)
    {
        if(0x80 == (u8IPmode &0x80))
        {
    bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_NLM_enable_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_NLM_setting_Mode);
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_NLM_setting_Mode);
        }
        else
        {
            bFuncEn = FunctionMode(u8IPmode);
            bRegWriteEn = WriteRegsiterFlag(u8IPmode);
            u8Mode = FunctionMode(u8IPmode);
        }
    NlmWrite2Register(      bFuncEn,bRegWriteEn,u8Mode, &stNLM);
    }
//M IP NLM End//

//M IP AC Start //
    if(u8IPindex == 26)
    {
       if(0x80 == (u8IPmode &0x80))
        {
    bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACGain_enable_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACGain_setting_Mode);
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACGain_setting_Mode);
        }
        else
        {
            bFuncEn = FunctionMode(u8IPmode);
            bRegWriteEn = WriteRegsiterFlag(u8IPmode);
            u8Mode = FunctionMode(u8IPmode);
        }

    if(0 == u8Mode)
    {
        HdrAcGainWrite2Register(bFuncEn,bRegWriteEn,u8Mode, &stAcGain);
    }
    else
    {
        //u16_check_status = ACgain_algo();
    }
    }

//M IP AC End //

//B03 ACE Start //
    if(u8IPindex == 27)
    {
    if(0x80 == (u8IPmode &0x80))
        {
            bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACE_enable_Mode);
            bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACE_setting_Mode);
            u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACE_setting_Mode);
        }
        else
        {
        bFuncEn = FunctionMode(u8IPmode);
        bRegWriteEn = WriteRegsiterFlag(u8IPmode);
            u8Mode = FunctionMode(u8IPmode);
        }

    if(0 == u8Mode)
    {
        HdrAceWrite2Register(   bFuncEn,bRegWriteEn,u8Mode, &stACE);
    }
    else
    {
        //u16_check_status = DolbyDriver();
    }
    }


//B03 ACE End //

//B05-01 Dither Start//
    if(u8IPindex == 28)
    {
        if(0x80 == (u8IPmode &0x80))
        {
    bFuncEn = FunctionMode(0x01);
    bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither1_setting_Mode);
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither1_setting_Mode);
        }
        else
        {
            bFuncEn = FunctionMode(u8IPmode);
            bRegWriteEn = WriteRegsiterFlag(u8IPmode);
            u8Mode = FunctionMode(u8IPmode);
        }
        if(0 == u8Mode)
        {
            hdrDither1_b501(        bFuncEn,bRegWriteEn,u8Mode, &stDither1);
        }
        else
        {
            //u16_check_status = DolbyDriver();
        }
    }

//B05-01 Dither End//

//B05-02 3D Lut Start//
    if(u8IPindex == 29)
    {
        if(0x80 == (u8IPmode &0x80))
        {
    bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3DLUT_enable_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3DLUT_setting_Mode);
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3DLUT_setting_Mode);
        }
        else
        {
            bFuncEn = FunctionMode(u8IPmode);
            bRegWriteEn = WriteRegsiterFlag(u8IPmode);
            u8Mode = FunctionMode(u8IPmode);
        }
    hdr3dLutSettings_b502(  bFuncEn,bRegWriteEn,u8Mode, &st3DLIT);

    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3DLUT_SRAM_Mode);
    if(0 == u8Mode)
    {
        //u16_check_status = DolbyDriver();
    }
    }
//B05-02 3D Lut End//

//B06-01 444to422 start //
    if(u8IPindex == 30)
    {
        if(0x80 == (u8IPmode &0x80))
        {
    bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_444to422_enable_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_444to422_enable_Mode);
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_444to422_enable_Mode);
        }
        else
        {
            bFuncEn = FunctionMode(u8IPmode);
            bRegWriteEn = WriteRegsiterFlag(u8IPmode);
            u8Mode = FunctionMode(u8IPmode);
        }
    Hdr444to442_b601(       bFuncEn,bRegWriteEn,u8Mode);
    }
//B06-01 444to422 End //

//B06-02 Dither2 Start //
    if(u8IPindex == 31)
    {
        if(0x80 == (u8IPmode &0x80))
        {
    bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither2_enable_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither2_setting_Mode);
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither2_setting_Mode);
        }
        else
        {
            bFuncEn = FunctionMode(u8IPmode);
            bRegWriteEn = WriteRegsiterFlag(u8IPmode);
            u8Mode = FunctionMode(u8IPmode);
        }
    hdrDither2_b602(       bFuncEn,bRegWriteEn,u8Mode, &stDither);
    }
//B06-02 Dither2 End //
    //mD3dLutWrite2Register(u16Full2limit3dLut);
    return u16_check_status ;
}

MS_U16 MS_Cfd_Curry_HDRIP_WriteRegister(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{
    MS_U8 u8CurrentIP_Vars,u8CurrentIP_Vars2,u8GammutMode;
    MS_U16 u16Max_luma_codes;
    MS_U16 u16temp;
    MS_U8 u8_check_status = E_CFD_MC_ERR_NOERR;
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_BOOL bFuncEn,bRegWriteEn;
    MS_U8 u8Mode;
    ST_HDR_B107 stYoffset;
    ST_HDR_NLM stNLM;
    ST_HDR_AC_GAIN stAcGain;
    ST_HDR_ACE stACE;
    ST_HDR_UVC stUVC;
    ST_HDR_DITHER1 stDither1;
    ST_HDR_3DLUT_SETTINGS st3DLIT;
    ST_HDR_DITHER2 stDither;
    memset(&stYoffset, 0, sizeof(ST_HDR_B107));
    memset(&stNLM, 0, sizeof(ST_HDR_NLM));
    memset(&stAcGain, 0, sizeof(ST_HDR_AC_GAIN));
    memset(&stACE, 0, sizeof(ST_HDR_ACE));
    memset(&stUVC, 0, sizeof(ST_HDR_UVC));
    memset(&stDither1, 0, sizeof(ST_HDR_DITHER1));
    memset(&st3DLIT, 0, sizeof(ST_HDR_3DLUT_SETTINGS));
    memset(&stDither, 0, sizeof(ST_HDR_DITHER2));

//HDR enable start//
    bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_enable_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_enable_Mode);
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_enable_Mode);
    HdrEnable(bFuncEn,bRegWriteEn,u8Mode);
//HDR enable end//

//Composer Mode start//
    pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Composer_Mode = MS_Cfd_Curry_HDR_Composer_Decision(pstu_Control_Param, pstu_HDRIP_Param);
//Composer Mode End//

//B01 Enable start//
    bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Module1_enable_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Module1_enable_Mode);
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Module1_enable_Mode);
    HdrB01En(               bFuncEn,bRegWriteEn,u8Mode);
//B01 Enable End//

//B01-01 Start //
    //MS_U8 HdrCupB0101(            bFuncEn,bRegWriteEn,u8Mode, ST_HDR_CUP* Struct);
//B01-01 End //

//B01-02 InCSC Start//
    if (1 == pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_InputCSC_Manual_Vars_en)
    {
        u8CurrentIP_Vars = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_InputCSC_MC;
    }
    else
    {
        u8CurrentIP_Vars = pstu_Control_Param->u8TempMatrixCoefficients[0];
        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_InputCSC_MC = pstu_Control_Param->u8TempMatrixCoefficients[0];
    }

    u8_check_status = CSC((pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_InputCSC_Mode&MaskForMode_LB),u8CurrentIP_Vars,pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_InputCSC_Ratio1,3,1,E_CSC_MASERTATI_HDR_INPUT);
    if(0 == u8_check_status)
    {
        printf("Error code =%d!!! B01-02 InputCSC Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
//B01-02  InCSC End//

//B01-03 Degamma Start//
    if(1 == pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_DeGamma_Manual_Vars_en)
    {
        u8CurrentIP_Vars = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Degamma_TR;
    }
    else
    {
        u8CurrentIP_Vars = pstu_Control_Param->u8TempTransferCharacterstics[0];
        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Degamma_TR = pstu_Control_Param->u8TempTransferCharacterstics[0];
    }

#if 0 //this code is for R/D
    if(MApi_GFLIP_XC_R2BYTE(REG_SC_BK30_01_L) == 0x001C)
    {
        pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax = MApi_GFLIP_XC_R2BYTE(REG_SC_Ali_BK30_0E_L);
    }
    else
    {
        pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax = 4000;
    }
#else
    pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax = 4000;
#endif

    u16Max_luma_codes = MS_Cfd_Luminance_To_PQCode(pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax, pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMaxFlag);

    u16Max_luma_codes = ((MS_U32)u16Max_luma_codes*0xff00)/(0x3ff);
    pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u16HDR_Degamma_Max_Lum = u16Max_luma_codes;


    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Degamma_SRAM_Mode);

    //printf("\033[1;35m###[Brian][%s][%d]### pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMax = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMax);
    ST_DEGAMMA_PARAMETERS_EXTENSION st_degamma_extension;
    ST_DEGAMMA_PARAMETERS_EXTENSION* pt_degamma_extension = &st_degamma_extension;
    pt_degamma_extension->dePQclamp_en = DePQClamp_EN;

#if (1 == DePQClamp_EN)
    u16temp = MS_Cfd_Luminance_To_PQCode(pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMax, 0);
    pt_degamma_extension->dePQclamp_value = ((MS_U32)u16temp*0xff00)/(0x3ff);
#else
    //give maximum values
    pt_degamma_extension->dePQclamp_value = 0xff00;
#endif


    if(u8Mode == 0)//degamma = Linear
    {
        u8CurrentIP_Vars = 8;
        u8_check_status = deGamma(u8CurrentIP_Vars, u16Max_luma_codes, pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Degamma_Ratio1
        ,pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u16HDR_Degamma_Ratio2
        , pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Degamma_Lut_En
        , pstu_HDRIP_Param->stu_Curry_HDRIP_Param.pu32HDR_Degamma_Lut_Address
        , pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u16HDR_Degamma_Lut_Length
        , 2,pstu_Control_Param->u8Temp_Format[0] , 0, pt_degamma_extension);
    }
    else if(u8Mode == 1) //degamma = u8TempTransferCharacterstics
    {
        //when HDR bypass happens
        if (0 != pstu_Control_Param->u8DoHDRbypassInHDRIP_Flag)
        {
            pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Degamma_Lut_En = 1;

            if (1 == pstu_Control_Param->u8DoHDRbypassInHDRIP_Flag)
            {
                pstu_HDRIP_Param->stu_Curry_HDRIP_Param.pu32HDR_Degamma_Lut_Address = Maserati_DeGamma_PQ;
            }
            else if  (2 == pstu_Control_Param->u8DoHDRbypassInHDRIP_Flag)//HLG
            {
                //should modify here
                pstu_HDRIP_Param->stu_Curry_HDRIP_Param.pu32HDR_Degamma_Lut_Address = Maserati_DeGamma_HLG;
                u8DoHDRbypassInHDRIP_Flag = pstu_Control_Param->u8DoHDRbypassInHDRIP_Flag;
            }

            pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u16HDR_Degamma_Lut_Length = 513;
            //printk("LutAdd:%d :%d\n",pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.pu32HDR_Gamma_Lut_Address,&Maserati22Gamma);
        }
        else
        {
            pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Degamma_Lut_En = 0;
            pstu_HDRIP_Param->stu_Curry_HDRIP_Param.pu32HDR_Degamma_Lut_Address = NULL;
            pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u16HDR_Degamma_Lut_Length = 513;
        }

        u8CurrentIP_Vars = u8CurrentIP_Vars;
        u8_check_status = deGamma(u8CurrentIP_Vars, u16Max_luma_codes, pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Degamma_Ratio1
        ,pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u16HDR_Degamma_Ratio2
        , pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Degamma_Lut_En
        , pstu_HDRIP_Param->stu_Curry_HDRIP_Param.pu32HDR_Degamma_Lut_Address
        , pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u16HDR_Degamma_Lut_Length
        , 2,pstu_Control_Param->u8Temp_Format[0] , 0, pt_degamma_extension);
    }
    else //dolby driver
    {
        //dolby_driver();
    }


    if(0 == u8_check_status)
    {
        printf("Error code =%d!!!B01-03 Degamma Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
//B01-03 Degamma End//

//B01-04 HDR 3x3 Start//
    if (1 == pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3x3_Manual_Vars_en)
    {
        u8CurrentIP_Vars = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3x3_InputCP;
        u8CurrentIP_Vars2 = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3x3_OutputCP;
    }
    else
    {
        u8CurrentIP_Vars = pstu_Control_Param->u8TempColorPriamries[0];
        u8CurrentIP_Vars2 =  pstu_Control_Param->u8TempColorPriamries[1];

        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3x3_InputCP = pstu_Control_Param->u8TempColorPriamries[0];
        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3x3_OutputCP = pstu_Control_Param->u8TempColorPriamries[1];
    }

#if 0
    if (E_CFD_CFIO_CP_PANEL == u8CurrentIP_Vars2)
    {
        u8GammutMode = 0;
    }
    else
    {
        u8GammutMode = 1;
    }
#endif

    //for HDR
    u8GammutMode = 1;

    //hardcode
    //u8GammutMode = 0;
    //u8CurrentIP_Vars = 1;
    //u8CurrentIP_Vars2 = 1;

    u8_check_status = gamutMapping(u8CurrentIP_Vars, u8GammutMode,
    //&(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.pstu_Panel_Param->stu_Cfd_Panel_ColorMetry)
                                    pstu_Control_Param->pstu_Panel_Param_Colorimetry,
                                    u8CurrentIP_Vars2,pstu_Control_Param->u8Temp_Format[0],
                                    pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u16HDR_3x3_Ratio2,1,1);

    if(0 == u8_check_status)
    {
        printf("Error code =%d!!!B01 -04 GamutMapping Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
//B01-04 HDR 3x3 End//

//B01-05 Gamma Start//
    if (1 == pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Gamma_Manual_Vars_en)
    {
        u8CurrentIP_Vars = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Gamma_TR;
    }
    else
    {
        if((1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag)) //&& (0x01 == (pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDRIP_Patch&0x01)))
        {
            //gamma_patch force to gamma 2.2
            u8CurrentIP_Vars = E_CFD_CFIO_TR_GAMMA2P2;//E_CFD_CFIO_TR_BT709;
            pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Gamma_TR = E_CFD_CFIO_TR_GAMMA2P2;//E_CFD_CFIO_TR_BT709;
            pstu_Control_Param->u8TempTransferCharacterstics[1] = E_CFD_CFIO_TR_GAMMA2P2;//E_CFD_CFIO_TR_BT709;
            pstu_Control_Param->u8OutputTransferCharacterstics = E_CFD_CFIO_TR_GAMMA2P2;//E_CFD_CFIO_TR_BT709;
        }
        else
        {
            u8CurrentIP_Vars = pstu_Control_Param->u8TempTransferCharacterstics[1];
            pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Gamma_TR = pstu_Control_Param->u8TempTransferCharacterstics[1];
        }
    }

    u16Max_luma_codes = 0xff00;

//if (u8CurrentIP_Vars == 16)
//{
//  u8CurrentIP_Vars = 6;
//}
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Gamma_SRAM_Mode);
    if(u8Mode == 0)//gamma = linear
    {
        u8CurrentIP_Vars = 8;
        u8_check_status = gamma(u8CurrentIP_Vars,u16Max_luma_codes,0,0,
                                pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Gamma_Lut_En,
                                pstu_HDRIP_Param->stu_Curry_HDRIP_Param.pu32HDR_Gamma_Lut_Address,
                                pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u16HDR_Gamma_Lut_Length,
                                1,pstu_Control_Param->u8Temp_Format[1],0);
    }
    else if(u8Mode == 1)//gamma = u8TempTransferCharacterstics
    {
        u8CurrentIP_Vars = u8CurrentIP_Vars;
        if(E_CFD_CFIO_TR_GAMMA2P2 == u8CurrentIP_Vars)
        {
            pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Gamma_Lut_En = 1;
            pstu_HDRIP_Param->stu_Curry_HDRIP_Param.pu32HDR_Gamma_Lut_Address = Maserati22Gamma;
            pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u16HDR_Gamma_Lut_Length = 512;
            //printk("LutAdd:%d :%d\n",pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.pu32HDR_Gamma_Lut_Address,&Maserati22Gamma);
        }
        else
        {
            //when HDR bypass happens
            if (0 != pstu_Control_Param->u8DoHDRbypassInHDRIP_Flag)
            {
                pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Gamma_Lut_En = 1;

                if (1 == pstu_Control_Param->u8DoHDRbypassInHDRIP_Flag)
                {
                    pstu_HDRIP_Param->stu_Curry_HDRIP_Param.pu32HDR_Gamma_Lut_Address = Maserati_gamma_PQ;
                }
                else if (2 == pstu_Control_Param->u8DoHDRbypassInHDRIP_Flag)//HLG
                {
                    //should modify here, not done yet
                    pstu_HDRIP_Param->stu_Curry_HDRIP_Param.pu32HDR_Gamma_Lut_Address = Maserati_gamma_HLG;
                }

                pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u16HDR_Gamma_Lut_Length = 513;
                //printk("LutAdd:%d :%d\n",pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.pu32HDR_Gamma_Lut_Address,&Maserati22Gamma);
            }
            else
            {
            pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Gamma_Lut_En = 0;
            pstu_HDRIP_Param->stu_Curry_HDRIP_Param.pu32HDR_Gamma_Lut_Address = NULL;
            pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u16HDR_Gamma_Lut_Length = 512;
        }
        }

        u8_check_status = gamma(u8CurrentIP_Vars,u16Max_luma_codes,0,0,
                                pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Gamma_Lut_En,
                                pstu_HDRIP_Param->stu_Curry_HDRIP_Param.pu32HDR_Gamma_Lut_Address,
                                pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u16HDR_Gamma_Lut_Length,
                                1,pstu_Control_Param->u8Temp_Format[1],0);
    }
    else //dolby driver
    {
        //dolby_driver();
    }


    if(0 == u8_check_status)
    {
        printf("Error code =%d!!!B01-05 Gamma Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
//B01-05 Gamma End//

//B01-06 OutCSC start//
    if (1 == pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_OutputCSC_Manual_Vars_en)
    {
        u8CurrentIP_Vars2 = pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_OutputCSC_MC;
    }
    else
    {
        u8CurrentIP_Vars2 = pstu_Control_Param->u8TempMatrixCoefficients[1];
        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_OutputCSC_MC = pstu_Control_Param->u8TempMatrixCoefficients[1];
    }

    u8_check_status = CSC((pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_OutputCSC_Mode&MaskForMode_LB)
                                        ,u8CurrentIP_Vars2,pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_OutputCSC_Ratio1
                                        ,3,0,E_CSC_MASERTATI_HDR_OUTPUT);
    if(0 == u8_check_status)
    {
        printf("Error code =%d!!! B01-06 OutCSC Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    //Max RGB CSC Start//
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_MAXRGB_CSC_Mode);

    u8_check_status = CSC(u8Mode,u8CurrentIP_Vars2,0x40,3,1,E_CSC_MASERATI_HDR_MAXRGB);
    //Max RGB CSC End//
    if(0 == u8_check_status)
    {
        printf("Error code =%d!!! Max RGB CSC Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
//B01-06 OutCSC End//

//B01-07 Yoffset Start//
    bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Yoffset_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Yoffset_Mode);
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Yoffset_Mode);
    if(0 == u8Mode)
    {
        b107Write2Register(     bFuncEn,bRegWriteEn,u8Mode, &stYoffset);
    }
    else
    {
        //u8_check_status = DolbyDriver();
    }
    if(0 == u8_check_status)
    {
        printf("Error code =%d!!! B01-07 Yoffset Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
//B01-07 Yoffset End//

//M IP NLM start//
    bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_NLM_enable_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_NLM_setting_Mode);
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_NLM_setting_Mode);
    NlmWrite2Register(      bFuncEn,bRegWriteEn,u8Mode, &stNLM);
//M IP NLM End//

//M IP AC Start //
//control by MDrv_HDR_ACAdaptiveGainGen_lavend()
//not control here

    bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACGain_enable_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACGain_setting_Mode);
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACGain_setting_Mode);

    //printf("\033[1;35m###[Brian][%s][%d]### 0:bFuncEn = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,bFuncEn);
    //printf("\033[1;35m###[Brian][%s][%d]### 0:bRegWriteEn = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,bRegWriteEn);
    //printf("\033[1;35m###[Brian][%s][%d]### 0:u8Mode = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,u8Mode);
    //printf("\033[1;35m###[Brian][%s][%d]### 0:u8acGainLut[0] = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,stAcGain.u8acGainLut[0]);
    //printf("\033[1;35m###[Brian][%s][%d]### 0:u8acGainLut[8] = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,stAcGain.u8acGainLut[8]);


    HdrAcGainWrite2Register(bFuncEn,bRegWriteEn,u8Mode, &stAcGain);


//M IP AC End //

//B03 ACE Start //

    bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACE_enable_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACE_setting_Mode);
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACE_setting_Mode);

    if(0 == u8Mode)
    {
        HdrAceWrite2Register(   bFuncEn,bRegWriteEn,u8Mode, &stACE);
    }
    else
    {
        //u16_check_status = DolbyDriver();
    }


//B03 ACE End //

//B05-01 Dither Start//
    bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither1_setting_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither1_setting_Mode);
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither1_setting_Mode);
    if(0 == u8Mode)
    {
        hdrDither1_b501(        bFuncEn,bRegWriteEn,u8Mode, &stDither1);
    }
    else
    {
        //u16_check_status = DolbyDriver();
    }

//B05-01 Dither End//

//B05-02 3D Lut Start//
    bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3DLUT_enable_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3DLUT_setting_Mode);
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3DLUT_setting_Mode);
    hdr3dLutSettings_b502(  bFuncEn,bRegWriteEn,u8Mode, &st3DLIT);

    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3DLUT_SRAM_Mode);
    if(0 == u8Mode)
    {
        //u16_check_status = DolbyDriver();
    }
//B05-02 3D Lut End//

//B06-01 444to422 start //
    bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_444to422_enable_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_444to422_enable_Mode);
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_444to422_enable_Mode);
    Hdr444to442_b601(       bFuncEn,bRegWriteEn,u8Mode);
//B06-01 444to422 End //

//B06-02 Dither2 Start //
    bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither2_enable_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither2_setting_Mode);
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither2_setting_Mode);
    hdrDither2_b602(       bFuncEn,bRegWriteEn,u8Mode, &stDither);
//B06-02 Dither2 End //
    //mD3dLutWrite2Register(u16Full2limit3dLut);

    return u16_check_status ;
}

MS_U16 MS_Cfd_Curry_HDRIP_Control(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{
    //E_CFD_MC_ERR
    MS_U8 u8_check_status = 0;
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_U16 u16_check_status_tmp = E_CFD_MC_ERR_NOERR;
    MS_U8  u8ERR_Happens_Flag = 0;
    MS_U8 u8IPindex = 0xFF,u8IPmode;
#if 0//RealChip
   u8IPindex = msReadByte(REG_SC_BK30_0E_H);
    u8IPmode = msReadByte(REG_SC_BK30_0E_L);
#endif
    //4 cases of u8Input_HDRIPMode
    //if ()
    //bypass mode
    if (0 == pstu_Control_Param->u8Input_HDRIPMode)
    {
        function1(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_enable_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        //function1(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Composer_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Module1_enable_Mode,MS_Cfd_OnlyEnable_Decision_OFF());

        function1(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_InputCSC_Mode,E_CFD_IP_CSC_OFF);
        function1(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Degamma_SRAM_Mode,0);
        function1(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3x3_Mode,0);
        function1(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Gamma_SRAM_Mode,0);
        function1(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_OutputCSC_Mode,E_CFD_IP_CSC_OFF);
        function1(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Yoffset_Mode,0);

        function1(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_MAXRGB_CSC_Mode,E_CFD_IP_CSC_YFULL_TO_RFULL);

        function1(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_NLM_enable_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_NLM_setting_Mode,0);
        function1(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACGain_enable_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACGain_setting_Mode,0);
        function1(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACE_enable_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        //function1(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACE_setting_Mode,0);

        function1(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither1_setting_Mode,0);
        function1(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3DLUT_enable_Mode,0);
        //function1(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3DLUT_SRAM_Mode,0);
        function1(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3DLUT_setting_Mode,0);

        function1(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_444to422_enable_Mode,0);
        function1(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither2_enable_Mode,0);
        //function1(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither2_setting_Mode,0);
    }
    //normal mode
    else
    {
        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_enable_Mode = MS_Cfd_Curry_HDR_enable_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        //pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Composer_Mode = MS_Cfd_Curry_HDR_Composer_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Module1_enable_Mode = MS_Cfd_Curry_HDR_Module1_enable_Decision(pstu_Control_Param, pstu_HDRIP_Param);

        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_InputCSC_Mode = MS_Cfd_Curry_HDR_InputCSC_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Degamma_SRAM_Mode = MS_Cfd_Curry_HDR_DeGamma_SRAM_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3x3_Mode = MS_Cfd_Curry_HDR_3x3_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Gamma_SRAM_Mode = MS_Cfd_Curry_HDR_Gamma_SRAM_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_OutputCSC_Mode = MS_Cfd_Curry_HDR_OutputCSC_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Yoffset_Mode = MS_Cfd_Curry_HDR_Yoffset_Decision(pstu_Control_Param, pstu_HDRIP_Param);

        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_MAXRGB_CSC_Mode = MS_Cfd_Curry_HDR_MAXRGB_CSC_Decision(pstu_Control_Param, pstu_HDRIP_Param);

        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_NLM_enable_Mode = MS_Cfd_Curry_HDR_NLM_enable_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_NLM_setting_Mode = MS_Cfd_Curry_HDR_NLM_setting_Decision(pstu_Control_Param, pstu_HDRIP_Param);
           pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACGain_enable_Mode = MS_Cfd_Curry_HDR_ACgain_enable_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACGain_setting_Mode = MS_Cfd_Curry_HDR_ACgain_setting_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACE_enable_Mode = MS_Cfd_Curry_HDR_ACE_enable_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        //pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_ACE_setting_Mode = MS_Cfd_Curry_HDR_ACE_setting_Decision(pstu_Control_Param, pstu_HDRIP_Param);

        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither1_setting_Mode = MS_Cfd_Curry_HDR_Dither1_setting_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3DLUT_enable_Mode = MS_Cfd_Curry_HDR_3DLUT_enable_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3DLUT_SRAM_Mode = MS_Cfd_Curry_HDR_3DLUT_SRAM_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        //pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_3DLUT_setting_Mode = MS_Cfd_Curry_HDR_3DLUT_setting_Decision(pstu_Control_Param, pstu_HDRIP_Param);

        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_444to422_enable_Mode = MS_Cfd_Curry_HDR_444to422_enable_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither2_enable_Mode = MS_Cfd_Curry_HDR_Dither2_enable_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        //pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Dither2_setting_Mode = MS_Cfd_Curry_HDR_Dither2_setting_Decision(pstu_Control_Param, pstu_HDRIP_Param);
    }

#if ((CFD_SW_VERSION >= 0x00000014) && (CFD_SW_RETURN_ST_VERSION >= 1))

    if (0x00 != FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Degamma_SRAM_Mode))
    {
        pstu_Control_Param->u8VideoADFlag = 1;
    }

    if (0x00 != FunctionMode(pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Gamma_SRAM_Mode))
    {
        pstu_Control_Param->u8VideoADFlag = 1;
    }

#endif

    u16_check_status_tmp = Mapi_Cfd_Curry_HDRIP_Param_Check(&(pstu_HDRIP_Param->stu_Curry_HDRIP_Param));

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }

#if 0//RealChip
    u8IPindex = msReadByte(REG_SC_BK30_0E_H);
    u8IPmode = msReadByte(REG_SC_BK30_0E_L);
#endif

    //u8IPindex = msReadByte(REG_SC_BK79_7E_L);

    //printf("\033[1;35m###[Brian][%s][%d]### BK78_7E_L =%x!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,u8IPindex);

    //u8IPindex = msReadByte(REG_SC_BK79_02_L);

    //printf("\033[1;35m###[Brian][%s][%d]### BK78_02_L =%x!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,u8IPindex);

    //MHal_XC_W2BYTEMSK(REG_SC_BK42_50_L, 0, BIT(1));
    //MHal_XC_W2BYTEMSK(0x100a4a, 0, 0xF0);
    //write2Byte(REG_SC_BK79_02_L, 0x0000);
    //write2Byte(REG_SC_BK79_7E_L, 0xFFFF);

    //u8IPindex = msReadByte(REG_SC_BK79_7E_L);

    //printf("\033[1;35m###[Brian][%s][%d]### BK78_7E_L(2) =%x!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,u8IPindex);

    //u8IPindex = msReadByte(REG_SC_BK79_02_L);

    //printf("\033[1;35m###[Brian][%s][%d]### BK78_02_L(2) =%x!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,u8IPindex);

    u8IPindex = 0xff;

    // Input to Physical Layer by Ali Start
    if(u8IPindex ==0xFF)
    {
        //printf("\033[1;35m###[Brian][%s][%d]### u8IPindex ==0xFF!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__);
        u16_check_status_tmp = MS_Cfd_Curry_HDRIP_WriteRegister(pstu_Control_Param,pstu_HDRIP_Param,pstu_TMO_Input);
    }
    else
    {
        u16_check_status_tmp = MS_Cfd_Curry_HDRIP_WriteRegister_DebugMode(pstu_Control_Param,pstu_HDRIP_Param,pstu_TMO_Input);
    }
    // Input to Physical Layer by Ali End

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }

    return u16_check_status;
}

MS_U16 MS_Cfd_Curry_SDRIP_WriteRegister(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{

    MS_U8 u8CurrentIP_Vars = 0,u8CurrentIP_Vars2 = 0;
    MS_U16 u16Max_luma_codes = 0;
    MS_U8 u8GammutMode = 0;
    MS_U8 u8IPindex = 0,u8IPmode = 0;
#if 0//RealChip
    u8IPindex = msReadByte(REG_SC_BK30_0E_H);
    u8IPmode = msReadByte(REG_SC_BK30_0E_L);
#endif
    MS_U8 u8_check_status = 1;
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;


    if (1 == pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_IP2_CSC_Manual_Vars_en)
        {
        u8CurrentIP_Vars = pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_IP2_CSC_MC;
        }
        else
        {
        u8CurrentIP_Vars = pstu_Control_Param->u8TempMatrixCoefficients[1];
        pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_IP2_CSC_MC = pstu_Control_Param->u8TempMatrixCoefficients[1];

#if (CFD_CURRY_SPECIAL_HANDLE_3P3 == 1)

        if (pstu_Control_Param->u8TempMatrixCoefficients[1] == E_CFD_CFIO_MC_BT2020CL)
        {
            u8CurrentIP_Vars = E_CFD_CFIO_MC_BT709_XVYCC709;
            pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_IP2_CSC_MC = E_CFD_CFIO_MC_BT709_XVYCC709;
        }

#endif

        }


#if 0 //(CFD_CURRY_Force_Inside_MC709 == 1)
    u8CurrentIP_Vars = 1;
    pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_IP2_CSC_MC = 1;
#endif

       //not control now
    //u8_check_status = CSC((pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_IP2_CSC_Mode&MaskForMode_LB),u8CurrentIP_Vars,pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_IP2_CSC_Ratio1,3,1,E_CSC_MANHATTAN_INPUT);

        if(0 == u8_check_status)
        {
            printf("Error code =%d!!! InputCSC Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
            u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
        }
        else
        {
            u16_check_status = E_CFD_MC_ERR_NOERR;
        }
        //inputCSC End

    //VIP off
    //pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_VIP_CM_MC = pstu_Control_Param->u8TempMatrixCoefficients[1];
    //write2ByteMask(0x102F00, 0xFFFF, 0x002F);
    //write2ByteMask(_PK_L_kano(0x2F,0x70), 0x0001, 0x00);
    //default is off

#if 1 //not control now
    //outputCSC start
    if (1 == pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_VIP_CM_Manual_Vars_en)
        {
        u8CurrentIP_Vars2 = pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_VIP_CM_MC;
        }
        else
        {
        u8CurrentIP_Vars2 = pstu_Control_Param->u8TempMatrixCoefficients[1];
        pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_VIP_CM_MC = pstu_Control_Param->u8TempMatrixCoefficients[1];

#if (CFD_CURRY_SPECIAL_HANDLE_3P3 == 1)

        if (pstu_Control_Param->u8TempMatrixCoefficients[1] == E_CFD_CFIO_MC_BT2020CL)
        {
            u8CurrentIP_Vars2 = E_CFD_CFIO_MC_BT709_XVYCC709;
            pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_VIP_CM_MC = E_CFD_CFIO_MC_BT709_XVYCC709;
        }

#endif

        }

#if 0 //(CFD_CURRY_Force_Inside_MC709 == 1)
    //u8CurrentIP_Vars2 = 1;
    //pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_VIP_CM_MC = 1;
#endif
       //not control now
    //u8_check_status = CSC((pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_VIP_CM_Mode&MaskForMode_LB),u8CurrentIP_Vars2,pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_VIP_CM_Ratio1,3,1,E_CSC_KANO_VIP_CM);

        if(0 == u8_check_status)
        {
        printf("Error code =%d!!! OutCSC Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
            u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
        }
        else
        {
            u16_check_status = E_CFD_MC_ERR_NOERR;
        }
    //outputCSC end
#endif



#if 1 //not control now
    //outputCSC start
    if (1 == pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_Conv420_CM_Manual_Vars_en)
    {
        u8CurrentIP_Vars2 = pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_Conv420_CM_MC;
    }
    else
    {
        u8CurrentIP_Vars2 = pstu_Control_Param->u8TempMatrixCoefficients[1];
        pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_Conv420_CM_MC = pstu_Control_Param->u8TempMatrixCoefficients[1];

#if (CFD_CURRY_SPECIAL_HANDLE_3P3 == 1)

        if (pstu_Control_Param->u8TempMatrixCoefficients[1] == E_CFD_CFIO_MC_BT2020CL)
        {
            u8CurrentIP_Vars2 = E_CFD_CFIO_MC_BT709_XVYCC709;
            pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_Conv420_CM_MC = E_CFD_CFIO_MC_BT709_XVYCC709;
        }

#endif
     }

#if 0//(CFD_CURRY_Force_Inside_MC709 == 1)
    //u8CurrentIP_Vars = 2;
    //pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_Conv420_CM_MC = 1;
#endif

       //not control now
    //u8_check_status = CSC((pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_Conv420_CM_Mode&MaskForMode_LB),u8CurrentIP_Vars2,pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_Conv420_CM_Ratio1,3,1,E_CSC_KANO_CONV420_CM);

        if(0 == u8_check_status)
        {
        printf("Error code =%d!!! OutCSC Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
            u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
        }
        else
        {
            u16_check_status = E_CFD_MC_ERR_NOERR;
        }
    //outputCSC end
#endif

    return u16_check_status;
}

MS_U16 MS_Cfd_Curry_SDRIP_WriteRegister_Sub(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{
    return 0;
    }

MS_U16 MS_Cfd_Curry_SDRIP_WriteRegister_DebugMode(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input,MS_U8 u8MainSubMode)
    {

    MS_U8 u8CurrentIP_Vars = 0,u8CurrentIP_Vars2 = 0;
    MS_U16 u16Max_luma_codes = 0;
    MS_U8 u8GammutMode = 0;
    MS_U8 u8IPindex = 0,u8IPmode = 0;
#if 0 //RealChip
    u8IPindex = msReadByte(REG_SC_BK30_0E_H);
    u8IPmode = msReadByte(REG_SC_BK30_0E_L);
#endif
    MS_U8 u8_check_status = 0;
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;

    //VIP off
    pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_VIP_CM_MC = pstu_Control_Param->u8TempMatrixCoefficients[1];

    if(u8IPindex == 1)
    {
        //inputCSC start

        if (1 == pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_IP2_CSC_Manual_Vars_en)
    {
            u8CurrentIP_Vars = pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_IP2_CSC_MC;
    }
    else
    {
            u8CurrentIP_Vars = pstu_Control_Param->u8TempMatrixCoefficients[1];
            pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_IP2_CSC_MC = pstu_Control_Param->u8TempMatrixCoefficients[1];
    }

#if (CFD_CURRY_Force_Inside_MC709 == 1)
    u8CurrentIP_Vars = 1;
    pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_IP2_CSC_MC = 1;
#endif


        //if(u8IPmode == 0xFF)
        if(0x80 == (u8IPmode&0x80))
        {

    }
        else
    {
            pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_IP2_CSC_Mode = u8IPmode;
    }

        if(0 == u8MainSubMode)
    {
            u8_check_status = CSC((pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_IP2_CSC_Mode&MaskForMode_LB),u8CurrentIP_Vars,pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_IP2_CSC_Ratio1,3,1,E_CSC_MANHATTAN_INPUT);
    }
    else
    {
            u8_check_status = CSC((pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_IP2_CSC_Mode&MaskForMode_LB),u8CurrentIP_Vars,pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_IP2_CSC_Ratio1,3,1,E_CSC_KANO_IP2_OUTPUT_SUB);
    }

        if(0 == u8_check_status)
    {
            printf("Error code =%d!!! InputCSC Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
            u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
        else
    {
            u16_check_status = E_CFD_MC_ERR_NOERR;
    }
        //inputCSC End
    }
    else if(u8IPindex == 2)
        {
        //outputCSC start
        if (1 == pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_Conv420_CM_Manual_Vars_en)
    {
            u8CurrentIP_Vars2 = pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_Conv420_CM_MC;
    }
        else
    {
            u8CurrentIP_Vars2 = pstu_Control_Param->u8TempMatrixCoefficients[1];
            pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_Conv420_CM_MC = pstu_Control_Param->u8TempMatrixCoefficients[1];
            }

        if(0x80 == (u8IPmode&0x80))
        {

    }
    else
    {
            pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_Conv420_CM_Mode = u8IPmode;
    }

        //no sub for this IP
        //if(0 == u8MainSubMode)
{
            u8_check_status = CSC((pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_Conv420_CM_Mode&MaskForMode_LB),u8CurrentIP_Vars2,pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_Conv420_CM_Ratio1,3,1,E_CSC_MANHATTAN_OUTPUT);
    }
        //else
        //{
        //   u8_check_status = CSC((pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_Conv420_CM_Mode&MaskForMode_LB),u8CurrentIP_Vars2,pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_Conv420_CM_Ratio1,3,1,E_CSC_MASERATI_OUTPUT_SUB);
        //}

        if(0 == u8_check_status)
    {
            printf("Error code =%d!!! OutCSC Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
            u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
            u16_check_status = E_CFD_MC_ERR_NOERR;
        }
        //outputCSC end
    }

    return u16_check_status;
    }

MS_U16 MS_Cfd_Curry_SDRIP_Control(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param, STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input,MS_U8 u8MainSubMode)
{
    //E_CFD_MC_ERR
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_U16 u16_check_status_tmp = E_CFD_MC_ERR_NOERR;
    MS_U8  u8ERR_Happens_Flag = 0;
    MS_U8 u8IPindex = 0xFF,u8IPmode = 0;


    if (0 == pstu_Control_Param->u8Input_SDRIPMode)
    {
        function1(pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_IP2_CSC_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_VIP_CM_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_Conv420_CM_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
    }
    else
    {
        pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_IP2_CSC_Mode = MS_Cfd_Curry_SDR_IP2_CSC_Decision(pstu_Control_Param,pstu_SDRIP_Param);
        pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_VIP_CM_Mode = MS_Cfd_Curry_SDR_VIP_CM_Decision(pstu_Control_Param,pstu_SDRIP_Param);
        pstu_SDRIP_Param->stu_Curry_SDRIP_Param.u8SDR_Conv420_CM_Mode = MS_Cfd_Curry_SDR_Conv420_CSC_Decision(pstu_Control_Param,pstu_SDRIP_Param);
    }

#if 0
    u16_check_status_tmp = Mapi_Cfd_Curry_SDRIP_Param_Check(&(pstu_SDRIP_Param->stu_Curry_SDRIP_Param));
#endif

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }

#if 0 //RealChip
    u8IPindex = msReadByte(REG_SC_BK30_0E_H);
    u8IPmode = msReadByte(REG_SC_BK30_0E_L);
#endif

    // Input to Physical Layer by Ali Start
#if 1
    if(u8IPindex ==0xFF) //default path in windows
    {
        if(0 == u8MainSubMode)
        {
            u16_check_status_tmp = MS_Cfd_Curry_SDRIP_WriteRegister(pstu_Control_Param,pstu_SDRIP_Param,pstu_TMO_Input);
        }
        else
        {
            u16_check_status_tmp = MS_Cfd_Curry_SDRIP_WriteRegister_Sub(pstu_Control_Param,pstu_SDRIP_Param,pstu_TMO_Input);
        }
    }
    else
    {
        u16_check_status_tmp = MS_Cfd_Curry_SDRIP_WriteRegister_DebugMode(pstu_Control_Param,pstu_SDRIP_Param,pstu_TMO_Input,u8MainSubMode);
    }
    // Input to Physical Layer by Ali End
#endif

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }

    //not test mode
    return u16_check_status;
}

#endif



