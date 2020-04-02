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

#if RealChip
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

//for K6Lite
const MS_U32 K6L_OSD_GAMMA_TH_TCH[7] = {0x00008,0x00048,0x00448,0x01448,0x03448,0x0B448,0x02B448};
const MS_U32 K6L_OSD_GAMMA_STEP_TCH[8] = {0x0,0x3,0x6,0x8,0x9,0xB,0xD,0xF};
const MS_U32 K6L_OSD_GAMMA_OFFSET_TCH[8] = {0x00,0x08,0x10,0x20,0x30,0x40,0x50,0x60};
const MS_U32 K6L_OSD_GAMMA_DLUT_TCH[124] = {  0,   4,   6,   7,   8,   9,  10,  11,
 12,  17,  21,  24,  27,  30,  32,  34,
 36,  48,  57,  64,  70,  75,  80,  84,  88,  92,  96,  99, 102, 105, 107, 110,
113, 122, 129, 136, 143, 148, 154, 159, 163, 167, 171, 175, 179, 182, 186, 189,
192, 198, 203, 208, 213, 217, 222, 226, 229, 233, 237, 240, 243, 246, 249, 252,
255, 266, 275, 283, 291, 298, 305, 311, 317, 322, 328, 333, 337, 342, 346, 350,
354, 368, 381, 392, 402, 411, 420, 428, 435, 442, 448, 454, 460, 465, 470, 475,
480, 497, 512, 525, 537, 547, 557, 565, 574, 581, 588, 595, 601, 607, 613, 619, 624, 629, 633, 638, 642, 647, 651, 655, 658, 662, 666 ,669};


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

#else //RIU

    writeByteMask(_PK_H_(0x32, 0x70),0x80, 0, E_OP); // AutoDownload off
    writeByteMask(_PK_L_(0x32, 0x60),  0x3, 0x1, E_OP); // lut_sel Gamma
    writeByteMask(_PK_L_(0x32, 0x68),  0x0F, 0x0, E_OP); // lut_sel_ip
    writeByteMask(_PK_H_(0x32, 0x60), 0x10, 0x0, E_OP); // lut_fast_md off
    writeByteMask(_PK_H_(0x32, 0x60), 0x20, 0x20, E_OP); // lut_wd_dup_md
    writeByteMask(_PK_H_(0x32, 0x60), 0x80, 0x80, E_OP); // lut_load_en

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
        if(1 == debugMode)
        {
            printk("%d: %0#x\n", u16Idx,SramData);
        }
        write2Bytes(_PK_L_(0x32, 0x61), u16Idx, E_OP); // lut_addr
        write2Bytes(_PK_L_(0x32, 0x62), SramData, E_OP); // wd0
        write2Bytes(_PK_L_(0x32, 0x64), SramData, E_OP); // wd1
        write2Bytes(_PK_L_(0x32, 0x66), SramData, E_OP); // wd2
        write2Bytes(_PK_L_(0x32, 0x63), 0, E_OP); // wd0
        write2Bytes(_PK_L_(0x32, 0x65), 0, E_OP); // wd1
        write2Bytes(_PK_L_(0x32, 0x67), 0, E_OP); // wd2
        writeByteMask(_PK_L_(0x32, 0x60), 0x10, 0x10, E_OP); // lut_write_pulse

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
        writeByteMask(_PK_H_(0x32, 0x60), 0x0F, 0x00, E_OP); // lut_flag_clr
    }
    writeByteMask(_PK_H_(0x32, 0x60), 0xFF, 0x00, E_OP); // lut_load_en
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

printf("\n  [CFD][ %s  , %d ]  do OSD SDR2HDR  !!!\n",__FUNCTION__,__LINE__);

//MApi_GFLIP_XC_W2BYTE(0x100a4a,0x0010);//clock
MApi_GFLIP_XC_W2BYTEMSK(0x100a4a, 0x0000, 0x000f ); //clock on



if(pstu_Cfd_OSD_Process_Configs->u8Video_IsFullRange == 0)
{
//printk("[OSD HDR] fullR to LimitY\n");
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_6e_L,0xE010);//out_r2y (fullR to limitY)
}
else if (pstu_Cfd_OSD_Process_Configs->u8Video_IsFullRange == 1)
{
    //printk("[OSD HDR] fullR to fullY\n");
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_6e_L,0x6010);//out_r2y (fullR to fullY)
}
else
{
    //printk("[OSD HDR] fullR to limitY(default)\n");
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_6e_L,0xE010);//out_r2y (fullR to limitY)
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
//r2r 709->2020(K6Lite,1.15b)
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_07_L,0xa09d);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_08_L,0x544d);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_09_L,0x0b16);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0a_L,0x11aa);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0b_L,0xeb6b);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0c_L,0x02eb);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0d_L,0x0433);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0e_L,0x1687);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0f_L,0xe546);
}
else{
    //r2r identity
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_07_L,0x8000);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_08_L,0x0);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_09_L,0x0 );
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0a_L,0x0);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0b_L,0x8000);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0c_L, 0x0);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0d_L,0x0);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0e_L,0x0);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_0f_L,0x8000);
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
    //MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_18_L,0x8001);
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

for( idx_gamma = 0; idx_gamma < 7 ; idx_gamma ++)
{
gamma_reg = (0x130000 | ((MS_U16)(0x31) << 8) | (MS_U16)((0x62+idx_gamma*2)*2));
MApi_GFLIP_XC_W2BYTE(gamma_reg,K6L_OSD_GAMMA_TH_TCH[idx_gamma]&0xffff );
gamma_reg = (0x130000 | ((MS_U16)(0x31) << 8) | (MS_U16)((0x62+idx_gamma*2+1)*2));
MApi_GFLIP_XC_W2BYTEMSK(gamma_reg,(K6L_OSD_GAMMA_TH_TCH[idx_gamma]>>16)&0xf, 0xf);
}

for( idx_gamma =0; idx_gamma < 8 ; idx_gamma++)
{
gamma_reg = (0x130000 | ((MS_U16)(0x31) << 8) | (MS_U16)((0x70+idx_gamma)*2));
MApi_GFLIP_XC_W2BYTEMSK(gamma_reg,K6L_OSD_GAMMA_STEP_TCH[idx_gamma],0x1F);
}

for( idx_gamma =0; idx_gamma < 8 ; idx_gamma++)
{
gamma_reg = (0x130000 | ((MS_U16)(0x31) << 8) | (MS_U16)((0x78+idx_gamma)*2));
MApi_GFLIP_XC_W2BYTEMSK(gamma_reg,K6L_OSD_GAMMA_OFFSET_TCH[idx_gamma],0x7F);
}

//OETF case : 400 (Technicolor)

//MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_7d_L,0x8000);
for( idx =0; idx < 124 ; idx++)
{
    tmo_reg = (0x130000 | ((MS_U16)(0x32) << 8) | (MS_U16)((0x01+idx)*2));
    MApi_GFLIP_XC_W2BYTEMSK(tmo_reg,K6L_OSD_GAMMA_DLUT_TCH[idx],0x3ff);
}
//MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK31_7d_L,0x8001);

//sat process (K6Lite)
MApi_GFLIP_XC_W2BYTEMSK(REG_SC_YAHAN_BK31_12_L,0x0040,0x7F); //sat_base, default0x40
MApi_GFLIP_XC_W2BYTEMSK(REG_SC_YAHAN_BK31_61_L,0x0047,0x7F); //sat_adj, default 0x47

//r2y 2020
if(pstu_Cfd_OSD_Process_Configs->u8Video_IsFullRange == 1)
{

if(pstu_Cfd_OSD_Process_Configs->u8Video_colorprimary == 9)//r2y full 2020
{
//printk("[OSD HDR] r2y full 2020\n");
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_6f_L,0x0200);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_70_L,0x1E29);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_71_L,0x1FD7);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_72_L,0x010D);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_73_L,0x02B6);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_74_L,0x003D);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_75_L,0x1F71);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_76_L,0x1E8F);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_77_L,0x0200);
}
else if(pstu_Cfd_OSD_Process_Configs->u8Video_colorprimary == 1)//r2y full 709
{
    //printk("[OSD HDR] r2y full 709\n");
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_6f_L,0x0200);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_70_L,0x1E2F);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_71_L,0x1FD1);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_72_L,0x00DA);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_73_L,0x02DC);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_74_L,0x004A);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_75_L,0x1F8B);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_76_L,0x1E75);
MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_77_L,0x0200);
}
else //r2y full 2020
    {
    //printk("[OSD HDR] r2y full 2020\n");
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_6f_L,0x0200);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_70_L,0x1E29);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_71_L,0x1FD7);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_72_L,0x010D);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_73_L,0x02B6);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_74_L,0x003D);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_75_L,0x1F71);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_76_L,0x1E8F);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_77_L,0x0200);
    }
}
else if (pstu_Cfd_OSD_Process_Configs->u8Video_IsFullRange == 0)
{
    if(pstu_Cfd_OSD_Process_Configs->u8Video_colorprimary == 9)//r2y limit 2020
    {
    //printk("[OSD HDR] r2y limit 2020\n");
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_6f_L,0x01C0);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_70_L,0x1E64);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_71_L,0x1FDC);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_72_L,0x00E6);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_73_L,0x0253);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_74_L,0x0034);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_75_L,0x1F83);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_76_L,0x1EBD);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_77_L,0x01C0);
    }
       else if(pstu_Cfd_OSD_Process_Configs->u8Video_colorprimary == 1)//r2y limit 709
{
       //printk("[OSD HDR] r2y limit 709\n");
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_6f_L,0x01C0);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_70_L,0x1E69);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_71_L,0x1FD7);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_72_L,0x00BA);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_73_L,0x0273);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_74_L,0x003F);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_75_L,0x1F99);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_76_L,0x1EA6);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_77_L,0x01C0);
    }
    else //r2y limit 709
    {
     //printk("[OSD HDR] r2y limit 2020\n");
     MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_6f_L,0x01C0);
     MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_70_L,0x1E64);
     MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_71_L,0x1FDC);
     MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_72_L,0x00E6);
     MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_73_L,0x0253);
     MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_74_L,0x0034);
     MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_75_L,0x1F83);
     MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_76_L,0x1EBD);
     MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_77_L,0x01C0);
    }
}
else//r2y limit 2020
 {
        //printk("[OSD HDR] r2y full 2020\n");
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_6f_L,0x01C0);
      MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_70_L,0x1E64);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_71_L,0x1FDC);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_72_L,0x00E6);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_73_L,0x0253);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_74_L,0x0034);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_75_L,0x1F83);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_76_L,0x1EBD);
    MApi_GFLIP_XC_W2BYTE(REG_SC_YAHAN_BK33_77_L,0x01C0);

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

#if 0
void Restore_input_bank_and_log_Maserati(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param)
{

#if 0 //RealChip
    msDlc_FunctionEnter();
    if(1 == GetDlcFunctionControl())
    {
        //msWriteByte(REG_SC_Ali_BK30_01_L,pstApiControlParamInit->u8Process_Mode );//0x1     ;     //0:off 1:on - normal mode 2:on - test mode
        //pstControlParamInit->u8HW_Structure                             = ;//E_CFD_HWS_STB_TYPE1 ;     //assign by E_CFD_MC_HW_STRUCTURE
        //pstControlParamInit->u8HW_PatchEn                               = ;//0x1     ;     //0: patch off, 1:patch on
        //msWriteByte(REG_SC_Ali_BK30_01_H,pstApiControlParamInit->u8Input_Source   );// ;     //assign by E_CFD_MC_SOURCE
        //msWriteByte(REG_SC_Ali_BK30_02_L,pstApiControlParamInit->u8Input_AnalogIdx   );   //assign by E_CFD_INPUT_ANALOG_FORMAT
        //msWriteByte(REG_SC_Ali_BK30_02_H,pstApiControlParamInit->u8Input_Format);  //assign by E_CFD_CFIO
        //msWriteByte(REG_SC_Ali_BK30_03_L,pstApiControlParamInit->u8Input_DataFormat );//assign by E_CFD_MC_FORMAT
        //msWriteByte(REG_SC_Ali_BK30_03_H,pstApiControlParamInit->u8Input_IsFullRange);//assign by E_CFD_CFIO_RANGE
        //msWriteByte(REG_SC_Ali_BK30_04_L,pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_HDRMode);//assign by E_CFIO_HDR_STATUS
        msWriteByte(REG_SC_Ali_BK30_04_H,pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_IsRGBBypass);//0:no R2Y for RGB in;1:R2Y for RGB in
        msWriteByte(REG_SC_Ali_BK30_05_L,pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_Source);//assign by E_CFD_MC_SOURCE
        msWriteByte(REG_SC_Ali_BK30_05_H,pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_Format); //assign by E_CFD_CFIO
        msWriteByte(REG_SC_Ali_BK30_06_L,pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_DataFormat); //assign by E_CFD_MC_FORMAT
         msWriteByte(REG_SC_Ali_BK30_06_H,pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_IsFullRange);//assign by E_CFD_CFIO_RANGE
        msWriteByte(REG_SC_Ali_BK30_07_H,pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_HDRMode);//assign by E_CFIO_HDR_STATUS
        //pstApiControlParamInit->u8Input_Format_HDMI_CE_Flag             = ; //1: CE 0 : IT
        //pstApiControlParamInit->u8Output_Format_HDMI_CE_Flag            = ; //1: CE 0 : IT
        //pstApiControlParamInit->u8HDMIOutput_GammutMapping_En           = ;  //1: ON 0: OFF
        //pstApiControlParamInit->u8HDMIOutput_GammutMapping_MethodMode           = ;  //0: extension 1:compression
        /*
        msWriteByte(REG_SC_Ali_BK30_08_L,pstApiControlParamInit->u8MMInput_ColorimetryHandle_Mode);      //1: ON 0: OFF
        msWriteByte(REG_SC_Ali_BK30_08_H,pstApiControlParamInit->u8PanelOutput_GammutMapping_Mode);  //1:depend on output_source infor ON 0: use default values
        msWriteByte(REG_SC_Ali_BK30_09_H,pstApiControlParamInit->u8TMO_TargetRefer_Mode);   //for TMO, unit : 1 nits
        msWriteByte(REG_SC_Ali_BK30_0A_H,((pstApiControlParamInit->u16Target_Max_Luminance>>8)&0xFF));
        msWriteByte(REG_SC_Ali_BK30_0A_L,((pstApiControlParamInit->u16Target_Max_Luminance)&0xFF)); //for TMO, unit : 0.0001 nits
        msWriteByte(REG_SC_Ali_BK30_0B_H,((pstApiControlParamInit->u16Target_Min_Luminance>>8)&0xFF));
        msWriteByte(REG_SC_Ali_BK30_0B_L,(pstApiControlParamInit->u16Target_Min_Luminance&0xFF));
        */
        //msWriteByte(REG_SC_Ali_BK30_08_L,(pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_enable_Mode
        //                                                          |(pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_SDRIPMode<<4)
        //                                                          |(pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_HDRIPMode<<6)));      //1: ON 0: OFF
        //msWriteByte(REG_SC_Ali_BK30_08_H,pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_InputCSC_Manual_Vars_en);  //1:depend on output_source infor ON 0: use default values
        //msWriteByte(REG_SC_Ali_BK30_09_H,pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_InputCSC_MC);   //for TMO, unit : 1 nits
        msWriteByte(REG_SC_Ali_BK30_08_H,pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_DeGamma_Manual_Vars_en);
        msWriteByte(REG_SC_Ali_BK30_09_H,pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_Degamma_TR); //for TMO, unit : 0.0001 nits
        msWriteByte(REG_SC_Ali_BK30_0B_H,pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_Gamma_Manual_Vars_en);
        msWriteByte(REG_SC_Ali_BK30_0B_L,pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_Gamma_TR);
        msWriteByte(REG_SC_Ali_BK30_0A_H,((pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax>>8)&0xFF));
        msWriteByte(REG_SC_Ali_BK30_0A_L,((pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax)&0xFF)); //for TMO, unit : 0.0001 nits
        u8IP2_CSC_Ratio1   =  pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_SDRIP_Input->stu_Manhattan_SDRIP_Param.u8IP2_CSC_Ratio1;
        u8VOP_3x3_Ratio1   =  pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_SDRIP_Input->stu_Manhattan_SDRIP_Param.u8VOP_3x3_Ratio1;
        u8Degamma_Ratio1   =  pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_SDRIP_Input->stu_Manhattan_SDRIP_Param.u8Degamma_Ratio1;
        u16Degamma_Ratio2  = pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_SDRIP_Input->stu_Manhattan_SDRIP_Param.u16Degamma_Ratio2;
        u163x3_Ratio2      =     pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_SDRIP_Input->stu_Manhattan_SDRIP_Param.u163x3_Ratio2;
    }

    msDlc_FunctionExit();
#endif

    //printk("u8Process_Mode                        :%d REG_SC_Ali_BK30_01_L \n", pstApiControlParamInit->u8Process_Mode                     );
    printk("u8Input_Source                          :%d REG_SC_Ali_BK30_01_H \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_Source                       );
    //printk("u8Input_AnalogIdx                     :%d REG_SC_Ali_BK30_02_L \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_AnalogIdx                    );
    printk("u8Input_Format                          :%d REG_SC_Ali_BK30_02_H \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_Format                       );
    printk("u8Input_DataFormat                      :%d REG_SC_Ali_BK30_03_L \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_DataFormat                   );
    printk("u8Input_IsFullRange                     :%d REG_SC_Ali_BK30_03_H \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_IsFullRange                  );
    printk("u8Input_HDRMode                         :%d REG_SC_Ali_BK30_04_L \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_HDRMode                      );
    printk("u8Input_IsRGBBypass                     :%d REG_SC_Ali_BK30_04_H \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_IsRGBBypass                  );
    printk("u8Output_Source                         :%d REG_SC_Ali_BK30_05_L \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_Source                      );
    printk("u8Output_Format                         :%d REG_SC_Ali_BK30_05_H \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_Format                      );
    printk("u8Output_DataFormat                     :%d REG_SC_Ali_BK30_06_L \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_DataFormat                  );
    printk("u8Output_IsFullRange                    :%d REG_SC_Ali_BK30_06_H \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_IsFullRange                 );
    printk("u8Output_HDRMode                        :%d REG_SC_Ali_BK30_07_H \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_HDRMode                     );
    printk("u8HDR_enable_Mode            :%d REG_SC_Ali_BK30_08_L Bit[0]  \n",pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_enable_Mode             );
    printk("u8Input_SDRIPMode            :%d REG_SC_Ali_BK30_08_L Bit[5:4]\n",pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_SDRIPMode                     );
    printk("u8Input_HDRIPMode            :%d REG_SC_Ali_BK30_08_L Bit[7:6]\n",pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_HDRIPMode                     );
    //printk("u8HDR_InputCSC_Manual_Vars_en:%d REG_SC_Ali_BK30_08_H         \n",pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_InputCSC_Manual_Vars_en );
    //printk("u8HDR_InputCSC_MC            :%d REG_SC_Ali_BK30_09_H         \n",pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_InputCSC_MC             );
    printk("u8HDR_DeGamma_Manual_Vars_en :%d REG_SC_Ali_BK30_08_H         \n",pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_DeGamma_Manual_Vars_en  );
    printk("u8HDR_Degamma_TR             :%d REG_SC_Ali_BK30_09_H         \n",pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_Degamma_TR              );
    printk("u8HDR_Gamma_Manual_Vars_en   :%d REG_SC_Ali_BK30_0B_H         \n",pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_Gamma_Manual_Vars_en    );
    printk("u8HDR_Gamma_TR               :%d REG_SC_Ali_BK30_0B_L         \n",pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_Gamma_TR                );
}
#endif

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

    if (pstu_Curry_HDRIP_Param->u8HDR_3x3_InputCP >= E_CFD_CFIO_CP_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_3x3_InputCP is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Curry_HDRIP_Param->u8HDR_3x3_InputCP = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
    }

    if (pstu_Curry_HDRIP_Param->u8HDR_3x3_OutputCP >= E_CFD_CFIO_CP_RESERVED_START)
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
    u16_check_status = E_CFD_MC_ERR_NOERR;//Easter_test
    return u16_check_status;
}

void Mapi_Cfd_Curry_TMOIP_Param_Init(STU_CFDAPI_Curry_TMOIP *pstu_Curry_TMOIP_Param)
{
    //do nothing
}

MS_U16 Mapi_Cfd_Curry_TMOIP_Param_Check(STU_CFDAPI_Curry_TMOIP *pstu_Curry_TMOIP_Param)
{

    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_U8 u8_check_status2 = E_CFD_MC_ERR_NOERR;
    //MS_U16 u16Temp;

    MS_U8 u8temp = 0;

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
            u8_mode = E_CFD_IP_CSC_RFULL_TO_YFULL;
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

MS_U16 MS_Cfd_Curry_TMO_Control(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
        {
    //E_CFD_MC_ERR
    MS_U8 u8_check_status = E_CFD_MC_ERR_NOERR;
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_BOOL bFuncEn,bRegWriteEn;
    MS_U8 u8Mode;
    ST_HDR_UVC stUVC;
    ST_HDR_TMO_SETTINGS stTMO;
    memset(&stUVC,0,sizeof(ST_HDR_UVC));
    memset(&stTMO,0,sizeof(ST_HDR_TMO_SETTINGS));
    //TMO_Decision_Tree_Start

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

        g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmax = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax;
        g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmin = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMin;
        g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TgtMinFlag = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMinFlag;
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
#if 0 //RealChip
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
    u8_check_status = deGamma(u8CurrentIP_Vars, u16Max_luma_codes, pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Degamma_Ratio1
        ,pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u16HDR_Degamma_Ratio2
        , pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Degamma_Lut_En
        , pstu_HDRIP_Param->stu_Curry_HDRIP_Param.pu32HDR_Degamma_Lut_Address
        , pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u16HDR_Degamma_Lut_Length
        , 2,pstu_Control_Param->u8Temp_Format[0] , 1);
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
        u8Mode  = FunctionMode(u8IPmode);
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

    //printf("\033[1;35m###[Brian][%s][%d]### u8Mode = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,u8Mode);

    if(u8Mode == 0)//degamma = Linear
    {
        u8CurrentIP_Vars = 8;
        u8_check_status = deGamma(u8CurrentIP_Vars, u16Max_luma_codes, pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Degamma_Ratio1
        ,pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u16HDR_Degamma_Ratio2
        , pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Degamma_Lut_En
        , pstu_HDRIP_Param->stu_Curry_HDRIP_Param.pu32HDR_Degamma_Lut_Address
        , pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u16HDR_Degamma_Lut_Length
        , 2,pstu_Control_Param->u8Temp_Format[0] , 0);
    }
    else if(u8Mode == 1) //degamma = u8TempTransferCharacterstics
    {
        u8CurrentIP_Vars = u8CurrentIP_Vars;
        u8_check_status = deGamma(u8CurrentIP_Vars, u16Max_luma_codes, pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Degamma_Ratio1
        ,pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u16HDR_Degamma_Ratio2
        , pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Degamma_Lut_En
        , pstu_HDRIP_Param->stu_Curry_HDRIP_Param.pu32HDR_Degamma_Lut_Address
        , pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u16HDR_Degamma_Lut_Length
        , 2,pstu_Control_Param->u8Temp_Format[0] , 0);
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
            pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u8HDR_Gamma_Lut_En = 0;
            pstu_HDRIP_Param->stu_Curry_HDRIP_Param.pu32HDR_Gamma_Lut_Address = NULL;
            pstu_HDRIP_Param->stu_Curry_HDRIP_Param.u16HDR_Gamma_Lut_Length = 512;
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

    u16_check_status_tmp = Mapi_Cfd_Curry_SDRIP_Param_Check(&(pstu_SDRIP_Param->stu_Curry_SDRIP_Param));

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



