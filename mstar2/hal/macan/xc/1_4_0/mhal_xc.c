///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2006 - 2008 MStar Semiconductor, Inc.
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
#ifndef _MHAL_XC_C_
#define _MHAL_XC_C_

#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>
#include "mdrv_mstypes.h"
#include "mhal_xc_chip_config.h"
#include "mdrv_xc_st.h"
#include "mdrv_xc.h"
#include "mhal_xc.h"
#include "mhal_dlc.h"
#include "chip_int.h"
#include "mstar/mstar_chip.h"
#include <linux/time.h>
#include <linux/delay.h>
#include "dolby_vision_driver.h"
#if defined(CONFIG_MIPS)
#elif defined(CONFIG_ARM) || defined(CONFIG_ARM64)
#include <asm/io.h>
#endif
#include <linux/init.h>
#include <asm/types.h>
#include <asm/cacheflush.h>

extern MS_BOOL KApi_XC_MLoad_Fire(EN_MLOAD_CLIENT_TYPE _client_type, MS_BOOL bImmeidate);
extern MS_BOOL _bEnableHDR; // this is patch , needs to be removed after Easter.Jhan fixes issue.
extern struct mutex _cfd_mutex;
extern EN_KDRV_XC_HDR_TYPE _enHDRType;
extern EN_KDRV_XC_INPUT_SOURCE_TYPE _enInputSourceType;

#define     XC_DEBUG_ENABLE
#ifdef      XC_DEBUG_ENABLE
#define     XC_KDBG(_fmt, _args...)        printk("[XC (Hal)][%s:%05d] " _fmt, __FUNCTION__, __LINE__, ##_args)
#define     XC_ASSERT(_con)   do {\
                                                            if (!(_con)) {\
                                                                printk(KERN_CRIT "BUG at %s:%d assert(%s)\n",\
                                                                                                    __FILE__, __LINE__, #_con);\
                                                                BUG();\
                                                            }\
                                                          } while (0)
#else
#define     XC_KDBG(fmt, args...)
#define     XC_ASSERT(arg)
#endif

static DEFINE_MUTEX(_adl_mutex);
//static MS_BOOL bFireing3DLut = FALSE;
extern MS_BOOL KApi_XC_MLoad_WriteCmd(EN_MLOAD_CLIENT_TYPE _client_type,MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask);
extern MS_BOOL KApi_XC_MLoad_Fire(EN_MLOAD_CLIENT_TYPE _client_type, MS_BOOL bImmeidate);
extern ST_KDRV_XC_CFD_INIT _stCfdInit[2];


extern MS_BOOL _ISMMDolby;
extern MS_BOOL _ISHDMIDolby;
//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define _miu_offset_to_phy(MiuSel, Offset, PhysAddr) if (MiuSel == E_CHIP_MIU_0) \
                                                        {PhysAddr = Offset;} \
                                                     else if (MiuSel == E_CHIP_MIU_1) \
                                                         {PhysAddr = Offset + ARM_MIU1_BASE_ADDR;} \
                                                     else \
                                                         {PhysAddr = Offset + ARM_MIU2_BASE_ADDR;}

#define DEBUG_HDR 0
#define AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR     512
#define AUTO_DOWNLOAD_HDR_3DLUT_SRAM_MAX_ADDR   4944
#define AUTO_DOWNLOAD_HDR_GAMMA_BLANKING        224
#define DMA_STR_PROTECT_REGISTER_NUMBER         10
/*
---------------------
|    DeGamma Data   |
---------------------
| 168*Byte_Per_word |
|      blanking     |
---------------------
|     Gamma Data    |
---------------------
*/

#define XVYCC_GAMMA_ENTRY   256
#define XVYCC_DEGAMMA_ENTRY 600
#define XVYCC_GAMMA_BLANKING 168 //For HW design


MS_U8 u8HDMI_case;
// HDR client of auto download has these following formats, include format0 and format1.
//
// Format0:
//127      125          116 115         112                             60        57                                   0
// | [x[2]] | [addr(10)] | 0 | [sel0(3)] |           [data1]             | sel1(3) |              [data0]              |
//                               sel0 = 1                            x(52) sel1 = 1              wd0(19),wd1(19),wd2(19)
//                               sel0 = 2     x(4),wd0(16),wd1(16),wd2(16) sel1 = 2         x(9),wd0(16),wd1(16),wd2(16)
//                               sel0 = 3                    x(40),wd0(12) sel1 = 3                        x(45),wd0(12)
//                               sel0 = 4   idx(4),wd0(16),wd1(16),wd2(16) sel1 = 4  x(5),idx(4),wd0(16),wd1(16),wd2(16)
// sel:   0: disable
//        1: De-gamma
//        2: Gamma
//        3: TMO
//        4: 3D-lut

// Format1:
//127      125          116 115          112                             60        57                                   0
// | [x[2]] | [addr(10)] | 1 | [ctrl0(3)] |           [data1]             | sel1(3) |              [data0]              |
//                                                                    x(52) sel1 = 0
//                                           idx(4),wd0(16),wd1(16),wd2(16) sel1 = 4      x(10),degam(19),gam(16),tmo(16)
// ctrl:                 bit: 114  113 112
//                          |degam|gam|tmo|
// sel:   0: disable
//        4: 3D-lut
//
// HW designer recommand using format1.
// These following definitions write datas of tmo/gamma/de-gamma/3d-lut separately by format1,
// please refer to definition of WRITE_TMO_DATA_FORMAT_1, WRITE_GAMMA_DATA_FORMAT_1, WRITE_DEGAMMA_DATA_FORMAT_1 and WRITE_3DLUT_DATA_FORMAT_1.
#define WRITE_TMO_DATA_FORMAT_1(baseaddr, index, value) \
    *(baseaddr + 15) = (((*(baseaddr + 15)) & 0xC0) | (((index) >> 4) & 0x3F));\
    *(baseaddr + 14) = ((((index) << 4) & 0xF0) | ((*(baseaddr + 14)) & 0x0F) | 0x09);\
    *(baseaddr + 1) = ((*(baseaddr + 1) & 0xF0) | (((value) >> 8) & 0x0F));\
    *(baseaddr) = ((value) & 0xFF);

/*#define WRITE_XVYCC_GAMMA_DATA_FORMAT_1(baseaddr, index, value) \
*(baseaddr + 14) =0xFF;\
  *(baseaddr + 13) =0x80;\
  *(baseaddr + 10) =  0;(((value) >> 8) & 0x0F);\
  *(baseaddr + 9) = 0;((value) & 0xFF);\
  *(baseaddr + 5) =  (((value) >> 8) & 0x0F);\
  *(baseaddr + 4) = ((value) & 0xFF);\
  *(baseaddr + 1) =  (((value) >> 8) & 0x0F);\
  *(baseaddr) = ((value) & 0xFF);8*/


/*#define WRITE_XVYCC_GAMMA_DATA_FORMAT_1(baseaddr, index, value) \
        *(baseaddr + 15+ 16) = 0xFF;\
    *(baseaddr + 14 + 16) = 0xFF;\
     *(baseaddr + 13 + 16) =   0xFF;\
     *(baseaddr + 12 + 16) =  0xFF;\
    *(baseaddr + 11 + 16) =  0xFF;\
    *(baseaddr + 10 + 16) = 0xFF;\
     *(baseaddr + 9 + 16) =   0xFF;\
     *(baseaddr + 8 + 16) =  0xFF;\
     *(baseaddr + 7 + 16) =   0xFF;\
     *(baseaddr + 6 + 16) =  0xFF;\
    *(baseaddr + 5 + 16) =  0xFF;\
    *(baseaddr + 4 + 16) =  0xFF;\
     *(baseaddr + 3 + 16) =  0xFF;\
     *(baseaddr + 2 + 16) =   0xFF;\
    *(baseaddr + 1 + 16) =   0xFF;\
    *(baseaddr + 16) =  0xFF; \
    *(baseaddr + 15) = 0xFF;\
    *(baseaddr + 14) = 0xFF;\
     *(baseaddr + 13) =  0x00;\
     *(baseaddr + 12) = 0x00;\
    *(baseaddr + 11) = 0x00;\
    *(baseaddr + 10) =(((value) >> 8) & 0x0F);\
     *(baseaddr + 9) =  ((value) & 0xFF);\
     *(baseaddr + 8) = 0x00;\
     *(baseaddr + 7) =  0x00;\
     *(baseaddr + 6) = 0x00;\
    *(baseaddr + 5) = (((value) >> 4) & 0xFF);\
    *(baseaddr + 4) = ((value<<4) & 0xF0);\
     *(baseaddr + 3) = 0x00;\
     *(baseaddr + 2) =  0x00;\
    *(baseaddr + 1) =  (((value) >> 8) & 0x0F);\
    *(baseaddr) = ((value) & 0xFF); */


// XVYCC_GAMMA:
//----------------------------------------------------------//
// | 114     113      112     [83:72]     [47:36]     [11:0]
// | R_WR    G_WR    B_WR      R_in         G_in       B_in
//----------------------------------------------------------//


#define WRITE_XVYCC_GAMMA_DATA_FORMAT_1(baseaddr,  value) \
    *(baseaddr + 15) = 0xFF;\
    *(baseaddr + 14) = 0xFF;\
     *(baseaddr + 13) =  0x00;\
     *(baseaddr + 12) = 0xFF;\
    *(baseaddr + 11) = 0xFF;\
    *(baseaddr + 10) =(((value) >> 8) & 0x0F);\
     *(baseaddr + 9) =  ((value) & 0xFF);\
     *(baseaddr + 8) = 0xFF;\
     *(baseaddr + 7) =  0xFF;\
     *(baseaddr + 6) = 0xFF;\
    *(baseaddr + 5) = (((value) >> 4) & 0xFF);\
    *(baseaddr + 4) = ((value<<4) & 0xF0);\
     *(baseaddr + 3) = 0xFF;\
     *(baseaddr + 2) =  0xFF;\
    *(baseaddr + 1) =  (((value) >> 8) & 0x0F);\
    *(baseaddr) = ((value) & 0xFF);

/*      #define WRITE_XVYCC_DEGAMMA_DATA_FORMAT_1(baseaddr, index, value) \
      *(baseaddr + 15+ 16) = 0xFF;\
  *(baseaddr + 14 + 16) = 0xFF;\
   *(baseaddr + 13 + 16) =   0xFF;\
   *(baseaddr + 12 + 16) =  0xFF;\
  *(baseaddr + 11 + 16) =  0xFF;\
  *(baseaddr + 10 + 16) = 0xFF;\
   *(baseaddr + 9 + 16) =   0xFF;\
   *(baseaddr + 8 + 16) =  0xFF;\
   *(baseaddr + 7 + 16) =   0xFF;\
   *(baseaddr + 6 + 16) =  0xFF;\
  *(baseaddr + 5 + 16) =  0xFF;\
  *(baseaddr + 4 + 16) =  0xFF;\
   *(baseaddr + 3 + 16) =  0xFF;\
   *(baseaddr + 2 + 16) =   0xFF;\
  *(baseaddr + 1 + 16) =   0xFF;\
  *(baseaddr + 16) =  0xFF; \
  *(baseaddr + 15) = 0xFF;\
  *(baseaddr + 14) = 0x00;\
   *(baseaddr + 13) =  0xFF;\
   *(baseaddr + 12) = 0xFF;\
  *(baseaddr + 11) = (((value) >> 16) & 0x07);\
  *(baseaddr + 10) =(((value) >> 8) & 0xFF);\
   *(baseaddr + 9) =  ((value) & 0xFF);\
   *(baseaddr + 8) = 0xFF;\
   *(baseaddr + 7) =  0xFF;\
   *(baseaddr + 6) = (((value) >> 12) & 0x7F);\
  *(baseaddr + 5) = (((value) >> 4) & 0xFF);\
  *(baseaddr + 4) = ((value<<4) & 0xF0);\
   *(baseaddr + 3) = 0xFF;\
   *(baseaddr + 2) =  (((value) >> 16) & 0x07);\
  *(baseaddr + 1) =  (((value) >> 8) & 0xFF);\
  *(baseaddr) = ((value) & 0xFF);*/

// XVYCC_DEGAMMA:
//----------------------------------------------------------//
// | 110     109     108     [90:72]     [54:36]     [18:0]
// | R_WR    G_WR    B_WR      R_in       G_in        B_in
//----------------------------------------------------------//

#define WRITE_XVYCC_DEGAMMA_DATA_FORMAT_1(baseaddr, value) \
    *(baseaddr + 15) = 0xFF;\
    *(baseaddr + 14) = 0x00;\
     *(baseaddr + 13) =  0xFF;\
     *(baseaddr + 12) = 0xFF;\
    *(baseaddr + 11) = (((value) >> 16) & 0x07);\
    *(baseaddr + 10) =(((value) >> 8) & 0xFF);\
     *(baseaddr + 9) =  ((value) & 0xFF);\
     *(baseaddr + 8) = 0xFF;\
     *(baseaddr + 7) =  0xFF;\
     *(baseaddr + 6) = (((value) >> 12) & 0x7F);\
    *(baseaddr + 5) = (((value) >> 4) & 0xFF);\
    *(baseaddr + 4) = ((value<<4) & 0xF0);\
     *(baseaddr + 3) = 0xFF;\
     *(baseaddr + 2) =  (((value) >> 16) & 0x07);\
    *(baseaddr + 1) =  (((value) >> 8) & 0xFF);\
    *(baseaddr) = ((value) & 0xFF);

#define WRITE_GAMMA_DATA_FORMAT_1(baseaddr, index, value) \
    *(baseaddr + 15) = (((*(baseaddr + 15)) & 0xC0) | (((index) >> 4) & 0x3F));\
    *(baseaddr + 14) = ((((index) << 4) & 0xF0) | ((*(baseaddr + 14)) & 0x0F) | 0x0A);\
    *(baseaddr + 1) = ((*(baseaddr + 1) & 0x0F) | (((value) << 4) & 0xF0));\
    *(baseaddr + 2) = (((value) >> 4) & 0xFF);\
    *(baseaddr + 3) = ((*(baseaddr + 3) & 0xF0) | (((value) >> 12) & 0x0F));

#define WRITE_DEGAMMA_DATA_FORMAT_1(baseaddr, index, value) \
    *(baseaddr + 15) = (((*(baseaddr + 15)) & 0xC0) | (((index) >> 4) & 0x3F));\
    *(baseaddr + 14) = ((((index) << 4) & 0xF0) | ((*(baseaddr + 14)) & 0x0F) | 0x0C);\
    *(baseaddr + 3) = ((*(baseaddr + 3) & 0x0F) | (((value) << 4) & 0xF0));\
    *(baseaddr + 4) = (((value) >> 4) & 0xFF);\
    *(baseaddr + 5) = ((*(baseaddr + 5) & 0x80) | (((value) >> 12) & 0x7F));

#define WRITE_3DLUT_DATA_FORMAT_1(baseaddr, index, subindex, rval, gval, bval) \
    *(baseaddr + 15) = (((*(baseaddr + 15)) & 0xC0) | (((index) >> 4) & 0x3F));\
    *(baseaddr + 14) = ((((index) << 4) & 0xF0) | ((*(baseaddr + 14)) & 0x0F) | 0x08);\
    *(baseaddr + 7) = ((*(baseaddr + 7) & 0xF1) | 0x08);\
    *(baseaddr + 7) = ((*(baseaddr + 7) & 0x0F) | (((bval) << 4) & 0xF0));\
    *(baseaddr + 8) = (((bval) >> 4) & 0xFF);\
    *(baseaddr + 9) = ((*(baseaddr + 9) & 0xF0) | (((bval) >> 12) & 0x0F));\
    *(baseaddr + 9) = ((*(baseaddr + 9) & 0x0F) | (((gval) << 4) & 0xF0));\
    *(baseaddr + 10) = (((gval) >> 4) & 0xFF);\
    *(baseaddr + 11) = ((*(baseaddr + 11) & 0xF0) | (((gval) >> 12) & 0x0F));\
    *(baseaddr + 11) = ((*(baseaddr + 11) & 0x0F) | (((rval) << 4) & 0xF0));\
    *(baseaddr + 12) = (((rval) >> 4) & 0xFF);\
    *(baseaddr + 13) = ((*(baseaddr + 13) & 0xF0) | (((rval) >> 12) & 0x0F));\
    *(baseaddr + 13) = ((*(baseaddr + 13) & 0x0F) | (((subindex) << 4) & 0xF0));

// The following definition clear bits of format1's ctrl & sel.
#define CLEAR_HDR_DATA_FORMAT_1(baseaddr) \
    *(baseaddr + 14) = ((*(baseaddr + 14)) & 0xF8);\
    *(baseaddr + 7) = (*(baseaddr + 7) & 0xF1);


#define WRITE_XVYCC_GAMMA_RED_DATA_FORMAT(baseaddr,  value) \
     *(baseaddr + 14) = ((*(baseaddr + 14) & 0xF8) | ((*(baseaddr + 14)) & 0x07) | 0x04);\
     *(baseaddr + 10) = ((*(baseaddr + 10) & 0xF0) | (((value) >> 8) & 0x0F));\
     *(baseaddr + 9) = ((value) & 0xFF);

#define WRITE_XVYCC_GAMMA_GREEN_DATA_FORMAT(baseaddr,  value) \
     *(baseaddr + 14) = ((*(baseaddr + 14) & 0xF8) | ((*(baseaddr + 14)) & 0x07) | 0x02);\
     *(baseaddr + 5) = (((value) >> 4) & 0xFF);\
     *(baseaddr + 4) = ((*(baseaddr + 4) & 0x0F) | (((value) << 4) & 0xF0));

#define WRITE_XVYCC_GAMMA_BLUE_DATA_FORMAT(baseaddr,  value) \
     *(baseaddr + 14) = ((*(baseaddr + 14) & 0xF8) | ((*(baseaddr + 14)) & 0x07) | 0x01);\
     *(baseaddr + 1) = ((*(baseaddr + 1) & 0xF0) | (((value) >> 8) & 0x0F));\
     *(baseaddr) = ((value) & 0xFF);

#define WRITE_XVYCC_DEGAMMA_RED_DATA_FORMAT(baseaddr,  value) \
     *(baseaddr + 13) = ((*(baseaddr + 13) & 0x0F) | ((*(baseaddr + 13)) & 0xF0) | 0x40);\
     *(baseaddr + 11) = ((*(baseaddr + 11) & 0xF8) | (((value) >> 16) & 0x07));\
     *(baseaddr + 10) = (((value) >> 8) & 0xFF);\
     *(baseaddr + 9) = (value & 0xFF);

#define WRITE_XVYCC_DEGAMMA_GREEN_DATA_FORMAT(baseaddr,  value) \
     *(baseaddr + 13) = ((*(baseaddr + 13) & 0x0F) | ((*(baseaddr + 13)) & 0xF0) | 0x20);\
     *(baseaddr + 6) = ((*(baseaddr + 6) & 0x80) | (((value) >> 12) & 0x7F));\
     *(baseaddr + 5) = ((value >> 4) & 0xFF);\
     *(baseaddr + 4) = ((*(baseaddr + 4) & 0x0F) | (((value) << 4) & 0xF0));

#define WRITE_XVYCC_DEGAMMA_BLUE_DATA_FORMAT(baseaddr,  value) \
     *(baseaddr + 13) = ((*(baseaddr + 13) & 0x0F) | ((*(baseaddr + 13)) & 0xF0) | 0x10);\
     *(baseaddr + 2) = ((*(baseaddr + 2) & 0xF0) | (((value) >> 16) & 0x0F));\
     *(baseaddr + 1) = ((value >> 8) & 0xFF);\
     *(baseaddr) = ((value) & 0xFF);

static MS_U32 _au32_3dlut_entry_num[8] = {736, 656, 656, 576, 656, 576, 576, 512};
MS_BOOL bTmoFireEnable;
MS_BOOL _bEnableHDRCLK;
MS_BOOL _bEnableHDRCLK_for_STR = FALSE;
MS_BOOL _bAdlFired = FALSE;
MS_BOOL _bTimingChanged = FALSE;
extern MS_U32 _u32NoSignalCount;
static MS_U32 _u32SWDesrbErrCount = 0;
//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_KDRV_XC_AUTO_DOWNLOAD_NONE,
    E_KDRV_XC_AUTO_DOWNLOAD_CONFIGURED,
    E_KDRV_XC_AUTO_DOWNLOAD_WRITED,
    E_KDRV_XC_AUTO_DOWNLOAD_FIRED,
} EN_KDRV_XC_AUTO_DOWNLOAD_STATUS;

typedef struct
{
    MS_PHY phyBaseAddr;                 /// baseaddr
    MS_U32 u32Size;                     /// size
    MS_U32 u32MiuNo;                    /// miu no
    MS_BOOL bEnable;                    /// enable/disable the client
    EN_KDRV_XC_AUTODOWNLOAD_MODE enMode;/// work mode
    MS_U32 u32StartAddr;                /// sram start addr
    MS_U32 u32Depth;                    /// data length
} ST_KDRV_XC_AUTODOWNLOAD_CLIENT_INFO;

typedef enum
{
    E_CHIP_MIU_0 = 0,
    E_CHIP_MIU_1,
    E_CHIP_MIU_2,
    E_CHIP_MIU_3,
    E_CHIP_MIU_NUM,
} CHIP_MIU_ID;
//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static DoVi_Config_t* _pstDmConfig = NULL;
static DoVi_Comp_ExtConfig_t* _pstCompConfExt = NULL;
static MsHdr_RegTable_t* _pstDmRegTable = NULL;
static MsHdr_Comp_Regtable_t* _pstCompRegTable = NULL;
DoVi_TargetDisplayConfig_t*  _pstDoVi_TargetDisplayConfig = NULL;
static DoVi_PQ_control_t*   _pstDoVi_PQ_control = NULL;
static ST_KDRV_XC_AUTODOWNLOAD_CLIENT_INFO _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_MAX];
static EN_KDRV_XC_AUTO_DOWNLOAD_STATUS _enAutoDownloadStatus[E_KDRV_XC_AUTODOWNLOAD_CLIENT_MAX];
static MS_U8 *pu8AutoDownloadDRAMBaseAddr[E_KDRV_XC_AUTODOWNLOAD_CLIENT_MAX] = {NULL};
static MS_BOOL _bUnmapAutoDownloadDRAM[E_KDRV_XC_AUTODOWNLOAD_CLIENT_MAX] = {FALSE};
static STU_CFDAPI_MM_PARSER gstMMParam = {0};
static ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info gstDRAMFormatInfo[2] = {0};

// CFD local variables
ST_KDRV_XC_CFD_INIT _stCfdInit[2] = {{0, 0, 0, 12}, {0, 0, 1, 12}};

ST_KDRV_XC_CFD_PANEL _stCfdPanel = {0, 0, 0, 0, 0, {0, 0, 0}, {0, 0, 0}, 0, 0, 0, 0, 0, 0, 0, 0, 0};

ST_KDRV_XC_CFD_HDMI _stCfdHdmi[2] =
{
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {0, 0, 0}, {0, 0, 0}, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, {0, 0, 0}, {0, 0, 0}, 0, 0, 0, 0, 0, 0},
};


STU_CFDAPI_MM_PARSER _stCfdMm[2] =
{
    {0, 0,  0, 0, 0, 0, 0, 0, {{0, 0, 0}, {0, 0, 0}, 0, 0} ,0},
    {0, 0,  0, 0, 0, 0, 0, 0, {{0, 0, 0}, {0, 0, 0}, 0, 0} ,0},
};


ST_KDRV_XC_CFD_ANALOG _stCfdAnalog[2] =
{
    {0, 0, 0, 0, 0, 0, 2, 2, 2},
    {0, 0, 1, 0, 0, 0, 2, 2, 2}
};


ST_KDRV_XC_CFD_HDR _stCfdHdr[2] =
{
    {0, 0, 0, 0, 1},
    {0, 0, 1, 0, 1},
};

ST_KDRV_XC_CFD_OSD _stCfdOsd[2] =
{
    {CFD_OSD_VERSION, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 100, 100, 0, 0, 0, 0, 0, 0, 0},
    {CFD_OSD_VERSION, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 100, 100, 1, 0, 0, 0, 0, 0, 0},
};

ST_KDRV_XC_CFD_TMO _stCfdTmo = {0, 0, {1024, 1024, 1024}, {3072, 3072, 3072}, {10000, 10000, 10000}, {256, 256, 256}, {544, 576, 640}, {768, 768, 768},
    {1024, 1024, 1024}, {3072, 3072, 3072}, {10000, 10000, 10000}, {256, 256, 256}, {544, 576, 640}, {768, 768, 768}, 0, 0
};


ST_KDRV_XC_CFD_LINEAR_RGB _stCfdLinearRgb = {0, 0, 0, 0};

ST_KDRV_XC_CFD_FIRE _stCfdFire[2] = {{0, 0, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 0}};

E_CFD_OUTPUT_SOURCE _eCfdOutputType = E_CFD_OUTPUT_SOURCE_PANEL;

static ST_KDRV_XC_CFD_DLC _stCfdDlc = {0, 0, FALSE};
static ST_KDRV_XC_AUTODOWNLOAD_CLIENT_INFO _stHDRClientInfo;
static MS_U8 *pu8HDRAutoDownloadDRAMBaseAddr = NULL;
static MS_BOOL _bUnmapHDRAutoDownloadDRAM = FALSE;
static MS_U16 _au16DmaRegValue[DMA_STR_PROTECT_REGISTER_NUMBER]= {0};
static MS_S16 _u16DolbySWBondStatus = -1;
static MS_S16 _u16DolbyHWBondStatus = -1;
static MS_BOOL _bCfdInited = FALSE;
static STU_CFDAPI_MAIN_CONTROL _stCfdMainControl;

extern StuDlc_HDRinit g_HDRinitParameters;

typedef enum
{
    EN_DOLBY_PQ_BACKLIGHT,
} EN_DOLBY_PQ_TYPE;


//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
#ifdef CONFIG_MSTAR_XC_HDR_SUPPORT
static void _MHal_XC_CFD_Convert_XCDRAM_to_CFDMMParam(ST_KDRV_XC_CFD_FIRE *pstCfdFire, STU_CFDAPI_MM_PARSER *pstMMParam, ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info *pstFormatInfo)
{
    if ((pstFormatInfo == NULL) || (pstMMParam == NULL) || (pstCfdFire == NULL))
    {
        return;
    }

    if(!MHal_XC_ShareMemIsAvailable(pstFormatInfo))
    {
        printk("Share Memory isn't available\n");
        return;
    }

    pstMMParam->u32Version                                                  = 0;
    pstMMParam->u16Length                                                   = sizeof(STU_CFDAPI_MM_PARSER);
    pstMMParam->u8Video_Full_Range_Flag                                     = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u8Video_Full_Range_Flag;

    if (pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.bVUIValid == FALSE)
    {
        // Algorithm team's suggestion, when VUI is not valid, please assign them the value of following
        if(pstCfdFire->bIsHdMode == TRUE)
        {
            // HD default value
            pstMMParam->u8Colour_primaries         = 1;
            pstMMParam->u8Transfer_Characteristics = 1;
            pstMMParam->u8Matrix_Coeffs            = 1;
        }
        else
        {
            // SD default value
            pstMMParam->u8Colour_primaries         = 5;
            pstMMParam->u8Transfer_Characteristics = 6;
            pstMMParam->u8Matrix_Coeffs            = 5;
        }
    }
    else
    {
        pstMMParam->u8Colour_primaries         = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u8Colour_primaries;
        pstMMParam->u8Transfer_Characteristics = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u8Transfer_Characteristics;
        pstMMParam->u8Matrix_Coeffs            = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u8Matrix_Coeffs;
    }


    pstMMParam->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_x[0] = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_x[0];
    pstMMParam->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_x[1] = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_x[1];
    pstMMParam->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_x[2] = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_x[2];
    pstMMParam->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_y[0] = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_y[0];
    pstMMParam->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_y[1] = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_y[1];
    pstMMParam->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_y[2] = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_y[2];
    pstMMParam->stu_Cfd_MM_MasterPanel_ColorMetry.u16White_point_x          = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16White_point_x;
    pstMMParam->stu_Cfd_MM_MasterPanel_ColorMetry.u16White_point_y          = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16White_point_y;
    pstMMParam->u32Master_Panel_Max_Luminance                               = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u32Master_Panel_Max_Luminance;
    pstMMParam->u32Master_Panel_Min_Luminance                               = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u32Master_Panel_Min_Luminance;
    pstMMParam->u8Mastering_Display_Infor_Valid                             = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.bSEIValid;
    pstMMParam->u8MM_HDR_ContentLightMetaData_Valid                         = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.bContentLightLevelEnabled;
    pstMMParam->u16Max_content_light_level                                  = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16maxContentLightLevel;
    pstMMParam->u16Max_pic_average_light_level                              = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16maxPicAverageLightLevel;

    memcpy(&gstMMParam,pstMMParam,sizeof(STU_CFDAPI_MM_PARSER));
}

static void _MHal_XC_DolbyPatch(void)
{
    if (IS_OTT_DOLBY)
    {
        // IP CSC off
        MHal_XC_W2BYTEMSK(REG_SC_BK02_2B_L,0 ,BIT(4));
        MHal_XC_W2BYTEMSK(REG_SC_BK02_2B_L,0 ,BIT(5));
        MHal_XC_W2BYTEMSK(REG_SC_BK02_2B_L,0 ,BIT(3));
        MHal_XC_W2BYTEMSK(REG_SC_BK02_2B_L,0 ,BIT(9));
        MHal_XC_W2BYTEMSK(REG_SC_BK02_2B_L,0 ,BIT(2));
        MHal_XC_W2BYTEMSK(REG_SC_BK02_2B_L,0 ,BIT(1));
        MHal_XC_W2BYTEMSK(REG_SC_BK02_2B_L,0 ,BIT(0));
        MHal_XC_W2BYTEMSK(REG_SC_BK02_2B_L,0 ,BIT(12));
        MHal_XC_W2BYTEMSK(REG_SC_BK02_2B_L,0 ,BIT(15));
        MHal_XC_W2BYTEMSK(REG_SC_BK02_2B_L,0 ,BIT(11));
        MHal_XC_W2BYTEMSK(REG_SC_BK02_2B_L,0 ,BIT(13));
        MHal_XC_W2BYTEMSK(REG_SC_BK02_2B_L,0 ,BIT(14));

        MHal_XC_W2BYTEMSK(REG_SC_BK02_2D_L,0 ,0x1FFF);
        MHal_XC_W2BYTEMSK(REG_SC_BK02_2E_L,0 ,0x1FFF);
        MHal_XC_W2BYTEMSK(REG_SC_BK02_2F_L,0 ,0x1FFF);
        MHal_XC_W2BYTEMSK(REG_SC_BK02_30_L,0 ,0x1FFF);
        MHal_XC_W2BYTEMSK(REG_SC_BK02_31_L,0 ,0x1FFF);
        MHal_XC_W2BYTEMSK(REG_SC_BK02_32_L,0 ,0x1FFF);
        MHal_XC_W2BYTEMSK(REG_SC_BK02_33_L,0 ,0x1FFF);
        MHal_XC_W2BYTEMSK(REG_SC_BK02_34_L,0 ,0x1FFF);
        MHal_XC_W2BYTEMSK(REG_SC_BK02_35_L,0 ,0x1FFF);


        // VOP 3x3 default value
        MHal_XC_W2BYTEMSK(REG_SC_BK10_2F_L,BIT(4) ,BIT(4));
        MHal_XC_W2BYTEMSK(REG_SC_BK10_2F_L,BIT(5) ,BIT(5));
        MHal_XC_W2BYTEMSK(REG_SC_BK0F_18_L,0 ,BIT(9));
        MHal_XC_W2BYTEMSK(REG_SC_BK0F_57_L,BIT(6) ,BIT(6));
        MHal_XC_W2BYTEMSK(REG_SC_BK0F_18_L,0 ,BIT(3));
        MHal_XC_W2BYTEMSK(REG_SC_BK10_2F_L,BIT(2) ,BIT(2));
        MHal_XC_W2BYTEMSK(REG_SC_BK10_2F_L,0 ,BIT(1));
        MHal_XC_W2BYTEMSK(REG_SC_BK10_2F_L,BIT(0) ,BIT(0));
        MHal_XC_W2BYTEMSK(REG_SC_BK0F_6B_L,0 ,BIT(15));
        MHal_XC_W2BYTEMSK(REG_SC_BK10_50_L,0 ,BIT(7));
        MHal_XC_W2BYTEMSK(REG_SC_BK0F_6B_L,0 ,BIT(14));
        MHal_XC_W2BYTEMSK(REG_SC_BK10_50_L,0 ,BIT(5));
        MHal_XC_W2BYTEMSK(REG_SC_BK10_50_L,0 ,BIT(6));
        MHal_XC_W2BYTEMSK(REG_SC_BK10_26_L,0x0731 ,0x1FFF);
        MHal_XC_W2BYTEMSK(REG_SC_BK10_27_L,0x04AC ,0x1FFF);
        MHal_XC_W2BYTEMSK(REG_SC_BK10_28_L,0x0000 ,0x1FFF);
        MHal_XC_W2BYTEMSK(REG_SC_BK10_29_L,0x1DDD ,0x1FFF);
        MHal_XC_W2BYTEMSK(REG_SC_BK10_2A_L,0x04AC ,0x1FFF);
        MHal_XC_W2BYTEMSK(REG_SC_BK10_2B_L,0x1F25 ,0x1FFF);
        MHal_XC_W2BYTEMSK(REG_SC_BK10_2C_L,0x0000 ,0x1FFF);
        MHal_XC_W2BYTEMSK(REG_SC_BK10_2D_L,0x04AC ,0x1FFF);
        MHal_XC_W2BYTEMSK(REG_SC_BK10_2E_L,0x0879 ,0x1FFF);
    }
}


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
E_CFD_CFIO MHal_XC_HDMI_Color_Format(MS_U8 u8PixelFormat, MS_U8 u8Colorimetry, MS_U8 u8ExtendColorimetry)
{
    if (u8PixelFormat== 1 || u8PixelFormat == 2 || u8PixelFormat == 3)
    {
        if (u8Colorimetry == 0)
        {
            return E_CFD_CFIO_YUV_BT709;
        }
        else if (u8Colorimetry == 1)
        {
            return E_CFD_CFIO_YUV_BT601_525;
        }
        else if (u8Colorimetry == 2)
        {
            return E_CFD_CFIO_YUV_BT709;
        }
        else if (u8Colorimetry == 3)
        {
            if (u8ExtendColorimetry == 0)
            {
                return E_CFD_CFIO_XVYCC_601;
            }
            else if (u8ExtendColorimetry == 1)
            {
                return E_CFD_CFIO_XVYCC_709;
            }
            else if (u8ExtendColorimetry == 2)
            {
                return E_CFD_CFIO_SYCC601;
            }
            else if (u8ExtendColorimetry == 3)
            {
                return E_CFD_CFIO_ADOBE_YCC601;
            }
            else if (u8ExtendColorimetry == 5)
            {
                return E_CFD_CFIO_YUV_BT2020_CL;
            }
            else if (u8ExtendColorimetry == 6)
            {
                return E_CFD_CFIO_YUV_BT2020_NCL;
            }
            else
            {
                return E_CFD_CFIO_RESERVED_START;
            }
        }
    }
    else
    {
        if (u8Colorimetry == 0)
        {
            return E_CFD_CFIO_SRGB;
        }
        else if (u8Colorimetry == 1)
        {
            return E_CFD_CFIO_RGB_BT601_525;
        }
        else if (u8Colorimetry == 2)
        {
            return E_CFD_CFIO_RGB_BT709;
        }
        else if (u8Colorimetry == 3)
        {
            if (u8ExtendColorimetry == 0)
            {
                return E_CFD_CFIO_RGB_BT709;
            }
            else if (u8ExtendColorimetry == 1)
            {
                return E_CFD_CFIO_RGB_BT709;
            }
            else if (u8ExtendColorimetry == 2)
            {
                return E_CFD_CFIO_SRGB;
            }
            else if (u8ExtendColorimetry == 3)
            {
                return E_CFD_CFIO_ADOBE_RGB;
            }
            else if (u8ExtendColorimetry == 4)
            {
                return E_CFD_CFIO_ADOBE_RGB;
            }
            else if (u8ExtendColorimetry == 5)
            {
                return E_CFD_CFIO_RGB_BT2020;
            }
            else if (u8ExtendColorimetry == 6)
            {
                return E_CFD_CFIO_RGB_BT2020;
            }
            else
            {
                return E_CFD_CFIO_RESERVED_START;
            }
        }
    }

    return E_CFD_CFIO_RESERVED_START;
}

E_CFD_MC_FORMAT MHal_XC_HDMI_Color_Data_Format(MS_U8 u8PixelFormat)
{
    if (u8PixelFormat == 1)
    {
        return E_CFD_MC_FORMAT_YUV422;
    }
    else if (u8PixelFormat == 2)
    {
        return  E_CFD_MC_FORMAT_YUV444;
    }
    else if (u8PixelFormat == 3)
    {
        return E_CFD_MC_FORMAT_YUV420;
    }
    else
    {
        return E_CFD_MC_FORMAT_RGB;
    }
}

MS_U16 MHal_XC_CFD_GetInitParam(ST_KDRV_XC_CFD_INIT *pstCfdInit)
{
    if (pstCfdInit == NULL)
    {
        return E_CFD_MC_ERR_INPUT_MAIN_CONTROLS;
    }

    memcpy (pstCfdInit, &_stCfdInit[pstCfdInit->u8Win], sizeof(ST_KDRV_XC_CFD_INIT));
    return E_CFD_MC_ERR_NOERR;
}

MS_U16 MHal_XC_CFD_GetHdmiParam(ST_KDRV_XC_CFD_HDMI *pstCfdHdmi)
{
    if (pstCfdHdmi == NULL)
    {
        return E_CFD_MC_ERR_INPUT_MAIN_CONTROLS;
    }

    memcpy (pstCfdHdmi, &_stCfdHdmi[pstCfdHdmi->u8Win], sizeof(ST_KDRV_XC_CFD_HDMI));
    return E_CFD_MC_ERR_NOERR;
}

MS_U16 MHal_XC_CFD_SetPanelParam(ST_KDRV_XC_CFD_PANEL *pstCfdPanel)
{
    STU_CFDAPI_PANEL_FORMAT stPanelParam;
    MS_U16 u16RetVal = 0;
    memset(&stPanelParam, 0, sizeof(STU_CFDAPI_PANEL_FORMAT));
    stPanelParam.u32Version = 0;
    stPanelParam.u16Length = sizeof(STU_CFDAPI_PANEL_FORMAT);
    stPanelParam.u16Panel_Med_Luminance = pstCfdPanel->u16MedLuminance;          //data * 1 nits
    stPanelParam.u16Panel_Max_Luminance = pstCfdPanel->u16MaxLuminance;          //data * 1 nits
    stPanelParam.u16Panel_Min_Luminance = pstCfdPanel->u16MinLuminance;          //data * 0.0001 nits
    stPanelParam.stu_Cfd_Panel_ColorMetry.u16Display_Primaries_x[0] = pstCfdPanel->u16Display_Primaries_x[0];
    stPanelParam.stu_Cfd_Panel_ColorMetry.u16Display_Primaries_x[1] = pstCfdPanel->u16Display_Primaries_x[1];
    stPanelParam.stu_Cfd_Panel_ColorMetry.u16Display_Primaries_x[2] = pstCfdPanel->u16Display_Primaries_x[2];
    stPanelParam.stu_Cfd_Panel_ColorMetry.u16Display_Primaries_y[0] = pstCfdPanel->u16Display_Primaries_y[0];
    stPanelParam.stu_Cfd_Panel_ColorMetry.u16Display_Primaries_y[1] = pstCfdPanel->u16Display_Primaries_y[1];
    stPanelParam.stu_Cfd_Panel_ColorMetry.u16Display_Primaries_y[2] = pstCfdPanel->u16Display_Primaries_y[2];
    stPanelParam.stu_Cfd_Panel_ColorMetry.u16White_point_x = pstCfdPanel->u16White_point_x;
    stPanelParam.stu_Cfd_Panel_ColorMetry.u16White_point_y= pstCfdPanel->u16White_point_y;

    u16RetVal = Mapi_Cfd_inter_PANEL_Param_Check(&stPanelParam);
    if (u16RetVal == E_CFD_MC_ERR_NOERR)
    {
        Mapi_Cfd_inter_PANEL_Param_Set(&stPanelParam);
    }
    else
    {
        printk("Mapi_Cfd_inter_PANEL_Param_Check fail, errCode: %d\n", u16RetVal);
    }

    return u16RetVal;
}

MS_U16 MHal_XC_CFD_SetEdidParam(ST_KDRV_XC_CFD_EDID *pstCfdEdid)
{
    STU_CFDAPI_HDMI_EDID_PARSER stEdidParam;
    MS_U16 u16RetVal = 0;
    memset(&stEdidParam, 0, sizeof(STU_CFDAPI_HDMI_EDID_PARSER));

    stEdidParam.u32Version = CFD_HDMI_EDID_ST_VERSION;
    stEdidParam.u16Length = sizeof(STU_CFDAPI_HDMI_EDID_PARSER);
    stEdidParam.u8HDMISink_HDRData_Block_Valid = pstCfdEdid->u8HDMISinkHDRDataBlockValid;
    stEdidParam.u8HDMISink_EOTF = pstCfdEdid->u8HDMISinkEOTF;
    stEdidParam.u8HDMISink_SM = pstCfdEdid->u8HDMISinkSM;
    stEdidParam.u8HDMISink_Desired_Content_Max_Luminance = pstCfdEdid->u8HDMISinkDesiredContentMaxLuminance;
    stEdidParam.u8HDMISink_Desired_Content_Max_Frame_Avg_Luminance = pstCfdEdid->u8HDMISinkDesiredContentMaxFrameAvgLuminance;
    stEdidParam.u8HDMISink_Desired_Content_Min_Luminance = pstCfdEdid->u8HDMISinkDesiredContentMinLuminance;
    stEdidParam.u8HDMISink_HDRData_Block_Length = pstCfdEdid->u8HDMISinkHDRDataBlockLength;
    stEdidParam.stu_Cfd_HDMISink_Panel_ColorMetry.u16Display_Primaries_x[0] = pstCfdEdid->u16Display_Primaries_x[0];
    stEdidParam.stu_Cfd_HDMISink_Panel_ColorMetry.u16Display_Primaries_x[1] = pstCfdEdid->u16Display_Primaries_x[1];
    stEdidParam.stu_Cfd_HDMISink_Panel_ColorMetry.u16Display_Primaries_x[2] = pstCfdEdid->u16Display_Primaries_x[2];
    stEdidParam.stu_Cfd_HDMISink_Panel_ColorMetry.u16Display_Primaries_y[0] = pstCfdEdid->u16Display_Primaries_y[0];
    stEdidParam.stu_Cfd_HDMISink_Panel_ColorMetry.u16Display_Primaries_y[1] = pstCfdEdid->u16Display_Primaries_y[1];
    stEdidParam.stu_Cfd_HDMISink_Panel_ColorMetry.u16Display_Primaries_y[2] = pstCfdEdid->u16Display_Primaries_y[2];
    stEdidParam.stu_Cfd_HDMISink_Panel_ColorMetry.u16White_point_x = pstCfdEdid->u16White_point_x;
    stEdidParam.stu_Cfd_HDMISink_Panel_ColorMetry.u16White_point_y = pstCfdEdid->u16White_point_y;
    stEdidParam.u8HDMISink_EDID_base_block_version = pstCfdEdid->u8HDMISinkEDIDBaseBlockVersion;
    stEdidParam.u8HDMISink_EDID_base_block_reversion = pstCfdEdid->u8HDMISinkEDIDBaseBlockReversion;
    stEdidParam.u8HDMISink_EDID_CEA_block_reversion = pstCfdEdid->u8HDMISinkEDIDCEABlockReversion;
    stEdidParam.u8HDMISink_VCDB_Valid = pstCfdEdid->u8HDMISinkVCDBValid;
    stEdidParam.u8HDMISink_Support_YUVFormat = pstCfdEdid->u8HDMISinkSupportYUVFormat;
    stEdidParam.u8HDMISink_Extended_Colorspace = pstCfdEdid->u8HDMISinkExtendedColorspace;
    stEdidParam.u8HDMISink_EDID_Valid = pstCfdEdid->u8HDMISinkEDIDValid;

    u16RetVal = Mapi_Cfd_inter_HDMI_EDID_Param_Check(&stEdidParam);
    if (u16RetVal == E_CFD_MC_ERR_NOERR)
    {
        Mapi_Cfd_inter_HDMI_EDID_Param_Set(&stEdidParam);
    }
    else
    {
        printk("Mapi_Cfd_inter_HDMI_EDID_Param_Check fail, errCode: %d\n", u16RetVal);
    }

    return u16RetVal;
}

MS_U16 MHal_XC_CFD_SetOsdParam(ST_KDRV_XC_CFD_OSD *pstCfdOsd )
{
    STU_CFDAPI_OSD_CONTROL stOSDParam;
    MS_U16 u16RetVal = 0;
    memset(&stOSDParam, 0, sizeof(STU_CFDAPI_OSD_CONTROL));

    stOSDParam.u32Version = 0;
    stOSDParam.u16Length = sizeof(STU_CFDAPI_OSD_CONTROL);
    if (pstCfdOsd->bSkipPictureSetting == TRUE)
    {
        stOSDParam.u16Hue = 50;
        stOSDParam.u16Saturation = 128;
    }
    else
    {
        stOSDParam.u16Hue = pstCfdOsd->u16Hue;
        stOSDParam.u16Saturation = pstCfdOsd->u16Saturation;
    }
    stOSDParam.u16Contrast = pstCfdOsd->u16Contrast;
    stOSDParam.u8OSD_UI_En = pstCfdOsd->u8OSDUIEn;
    stOSDParam.u8OSD_UI_Mode = pstCfdOsd->u8OSDUIMode;

    u16RetVal = Mapi_Cfd_inter_OSD_Param_Check(&stOSDParam);
    if (u16RetVal == E_CFD_MC_ERR_NOERR)
    {
        Mapi_Cfd_inter_OSD_Param_Set(&stOSDParam);
    }
    else
    {
        printk("Mapi_Cfd_inter_OSD_Param_Check fail, errCode: %d\n", u16RetVal);
    }
    return u16RetVal;
}

MS_U16 MHal_XC_CFD_InitIp(ST_KDRV_XC_CFD_FIRE *pstCfdFire)
{
    MS_U16 u16RetVal = 0;
    MS_U8 u8HWMainSubMode = 0;
    ST_KDRV_XC_CFD_HW_IPS stHwIpsParam;
    memset(&stHwIpsParam, 0, sizeof(ST_KDRV_XC_CFD_HW_IPS));
    u8HWMainSubMode = pstCfdFire->u8Win;

    Mapi_Cfd_Maserati_DLCIP_Param_Init(&stHwIpsParam.stDLCInput.stMaseratiDLCParam);
    Mapi_Cfd_Maserati_TMOIP_Param_Init(&stHwIpsParam.stTMOInput.stMaseratiTMOParam);
    Mapi_Cfd_Maserati_SDRIP_Param_Init(&stHwIpsParam.stSDRIPInput.stMaseratiSDRIPParam);
    Mapi_Cfd_Maserati_HDRIP_Param_Init(&stHwIpsParam.stHDRIPInput.stMaseratiHDRIPParam);

    if(CFD_IS_MM(pstCfdFire->u8InputSource) || CFD_IS_DTV(pstCfdFire->u8InputSource)) // MM and DTV source
    {
        ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info stFormatInfo;
        MHal_XC_CFD_WithdrawMMParam(&stFormatInfo, pstCfdFire->u8Win);
        if (_bEnableHDR == TRUE && (_stCfdHdr[pstCfdFire->u8Win].u8HdrType & E_KDRV_XC_CFD_HDR_TYPE_DOLBY))
        {
            //stHwIpsParam.stHDRIPInput.stMaseratiHDRIPParam.u8HDR_enable_Mode = 0xC0;
            stHwIpsParam.stTMOInput.stMaseratiTMOParam.u8HDR_TMO_curve_enable_Mode = 0xC0;
        }
        else if (_bEnableHDR == FALSE && (_stCfdHdr[pstCfdFire->u8Win].u8HdrType & E_KDRV_XC_CFD_HDR_TYPE_OPEN)
            && ((stFormatInfo.HDRMemFormat.stHDRMemFormatCFD.bVUIValid == TRUE) && (stFormatInfo.HDRMemFormat.stHDRMemFormatCFD.u8Transfer_Characteristics == 16)))
        {
            //stHwIpsParam.stHDRIPInput.stMaseratiHDRIPParam.u8HDR_enable_Mode = 0xC0;
            stHwIpsParam.stTMOInput.stMaseratiTMOParam.u8HDR_TMO_curve_enable_Mode = 0xC0;
        }
        else if (_bEnableHDR == FALSE && (_stCfdHdr[pstCfdFire->u8Win].u8HdrType & E_KDRV_XC_CFD_HDR_TYPE_HLG)
            && ((stFormatInfo.HDRMemFormat.stHDRMemFormatCFD.bVUIValid == TRUE) && (stFormatInfo.HDRMemFormat.stHDRMemFormatCFD.u8Transfer_Characteristics == 18)))
        {
            //stHwIpsParam.stHDRIPInput.stMaseratiHDRIPParam.u8HDR_enable_Mode = 0xC0;
            stHwIpsParam.stTMOInput.stMaseratiTMOParam.u8HDR_TMO_curve_enable_Mode = 0xC0;
        }
        else
        {
            //stHwIpsParam.stHDRIPInput.stMaseratiHDRIPParam.u8HDR_enable_Mode = 0x40;
            stHwIpsParam.stTMOInput.stMaseratiTMOParam.u8HDR_TMO_curve_enable_Mode = 0x40;
        }
    }
    else if(CFD_IS_HDMI(pstCfdFire->u8InputSource)) // HDMI source
    {
        if (((_stCfdHdmi[pstCfdFire->u8Win].u8EOTF == 2) && (_stCfdHdr[pstCfdFire->u8Win].u8HdrType & E_KDRV_XC_CFD_HDR_TYPE_OPEN))  || // if HDMI content is HDR10, we nedd flow UI option to on/off.
            ((_stCfdHdmi[pstCfdFire->u8Win].u8EOTF == 3)  && (_stCfdHdr[pstCfdFire->u8Win].u8HdrType & E_KDRV_XC_CFD_HDR_TYPE_HLG)) || // if HDMI content is HLG, we nedd flow UI option to on/off.
            (_stCfdHdmi[pstCfdFire->u8Win].u8EOTF != 2)) // HDMI content is not HDR10,always set HDR ip enable (TMO bypass)
        {
            //stHwIpsParam.stHDRIPInput.stMaseratiHDRIPParam.u8HDR_enable_Mode = 0xC0;
            stHwIpsParam.stTMOInput.stMaseratiTMOParam.u8HDR_TMO_curve_enable_Mode = 0xC0;
        }
        else
        {
            //stHwIpsParam.stHDRIPInput.stMaseratiHDRIPParam.u8HDR_enable_Mode = 0x40;
            stHwIpsParam.stTMOInput.stMaseratiTMOParam.u8HDR_TMO_curve_enable_Mode = 0x40;
        }
    }
    else // otherwise input source
    {
        //stHwIpsParam.stHDRIPInput.stMaseratiHDRIPParam.u8HDR_enable_Mode = 0x40;
        stHwIpsParam.stTMOInput.stMaseratiTMOParam.u8HDR_TMO_curve_enable_Mode = 0x40;
    }

    u16RetVal = Mapi_Cfd_Maserati_TMOIP_Param_Check(&stHwIpsParam.stTMOInput.stMaseratiTMOParam);
    if (u16RetVal == E_CFD_MC_ERR_NOERR)
    {
        Mapi_Cfd_Maserati_TMOIP_Param_Set(u8HWMainSubMode, &stHwIpsParam.stTMOInput.stMaseratiTMOParam);
    }
    else
    {
        printk("Mapi_Cfd_Maserati_TMOIP_Param_Check fail, errCode: %d\n", u16RetVal);
        return u16RetVal;
    }

    u16RetVal = Mapi_Cfd_Maserati_DLCIP_Param_Check(&stHwIpsParam.stDLCInput.stMaseratiDLCParam);
    if (u16RetVal == E_CFD_MC_ERR_NOERR)
    {
        Mapi_Cfd_Maserati_DLCIP_Param_Set(u8HWMainSubMode, &stHwIpsParam.stDLCInput.stMaseratiDLCParam);
    }
    else
    {
        printk("Mapi_Cfd_Maserati_DLCIP_Param_Check fail, errCode: %d\n", u16RetVal);
        return u16RetVal;
    }

    u16RetVal = Mapi_Cfd_Maserati_SDRIP_Param_Check(&stHwIpsParam.stSDRIPInput.stMaseratiSDRIPParam);
    if (u16RetVal == E_CFD_MC_ERR_NOERR)
    {
        Mapi_Cfd_Maserati_SDRIP_Param_Set(u8HWMainSubMode, &stHwIpsParam.stSDRIPInput.stMaseratiSDRIPParam);
    }
    else
    {
        printk("Mapi_Cfd_Maserati_SDRIP_Param_Check fail, errCode: %d\n", u16RetVal);
        return u16RetVal;
    }

    u16RetVal = Mapi_Cfd_Maserati_HDRIP_Param_Check(&stHwIpsParam.stHDRIPInput.stMaseratiHDRIPParam);
    if (u16RetVal == E_CFD_MC_ERR_NOERR)
    {
        Mapi_Cfd_Maserati_HDRIP_Param_Set(u8HWMainSubMode, &stHwIpsParam.stHDRIPInput.stMaseratiHDRIPParam);
    }
    else
    {
        printk("Mapi_Cfd_Maserati_HDRIP_Param_Check fail, errCode: %d\n", u16RetVal);
        return u16RetVal;
    }

    return u16RetVal;
}

BOOL MHal_XC_EnableHDRCLK(MS_BOOL bEnable, MS_BOOL bImmediate)
{
    if (_bEnableHDRCLK == bEnable)
        return TRUE;

    _bEnableHDRCLK = bEnable;

    if (bEnable)
    {
        // hdr clk setting enable
        if (bImmediate)
        {
            if(IS_DOLBY_HDR(MAIN_WINDOW))
            {
                //MHal_XC_SetHDR_DMARequestOFF(DISABLE, bImmediate); //Move DMARequest control to init by sourcetype
            }

            MHal_XC_W2BYTE(REG_SC_BK79_02_L, 0);
            MHal_XC_W2BYTEMSK(REG_SC_BK79_7E_L, BIT(1)|BIT(0), BIT(1)|BIT(0));
        }
        else
        {
            if(IS_DOLBY_HDR(MAIN_WINDOW))
            {
                //MHal_XC_SetHDR_DMARequestOFF(DISABLE, bImmediate); //Move DMARequest control to init by sourcetype
            }
            KApi_XC_MLoad_WriteCmd(E_CLIENT_MAIN_HDR, REG_SC_BK79_02_L, 0, 0xFFFF);
            KApi_XC_MLoad_WriteCmd(E_CLIENT_MAIN_HDR, REG_SC_BK79_7E_L, BIT(1)|BIT(0), BIT(1)|BIT(0));
        }

        // because setting REG_SC_BK79_02_L to 0xFFFF will make SRAM reset
        // we have to tell our driver to redownload these settings
        DoVi_ResetDownloadProcedure();
    }
    else
    {
        // hdr clk setting disable
        if (bImmediate)
        {
            //MHal_XC_SetHDR_DMARequestOFF(ENABLE, bImmediate); //Move DMARequest control to init by sourcetype
            MHal_XC_W2BYTE(REG_SC_BK79_02_L, 0xFFFF);
            MHal_XC_W2BYTEMSK(REG_SC_BK79_7E_L, 0, BIT(1)|BIT(0));
        }
        else
        {
            //MHal_XC_SetHDR_DMARequestOFF(ENABLE, bImmediate); //Move DMARequest control to init by sourcetype
            KApi_XC_MLoad_WriteCmd(E_CLIENT_MAIN_HDR, REG_SC_BK79_02_L, 0xFFFF, 0xFFFF);
            KApi_XC_MLoad_WriteCmd(E_CLIENT_MAIN_HDR, REG_SC_BK79_7E_L, 0, BIT(1)|BIT(0));
        }
    }

    return TRUE;
}

void MHal_XC_SetManualTMOParemeter(MS_U8 *data)
{
    MHal_XC_W2BYTEMSK(REG_SC_BK5F_66_L,0x0003, 0x0003);

    MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_50_L,(*(data + 2) << 8) |  *(data + 3));
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_51_L,(*(data + 4) << 8) |  *(data + 5));
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_52_L,(*(data + 6) << 8) |  *(data + 7));
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_53_L,(*(data + 8) << 8) |  *(data + 9));
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_54_L,(*(data + 10) << 8) |  *(data + 11));
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_55_L,(*(data + 12) << 8) |  *(data + 13));
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_56_L,(*(data + 14) << 8) |  *(data + 15));
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_57_L,(*(data + 16) << 8) |  *(data + 17));
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_58_L,(*(data + 18) << 8) |  *(data + 19));
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_59_L,(*(data + 20) << 8) |  *(data + 21));
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_5A_L,(*(data + 22) << 8) |  *(data + 23));

    MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_5B_L,(*(data + 26) << 8) |  *(data + 27));
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_5C_L,(*(data + 28) << 8) |  *(data + 29));
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_5D_L,(*(data + 30) << 8) |  *(data + 31));
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_5E_L,(*(data + 32) << 8) |  *(data + 33));
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_5F_L,(*(data + 34) << 8) |  *(data + 35));
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_60_L,(*(data + 36) << 8) |  *(data + 37));
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_61_L,(*(data + 38) << 8) |  *(data + 39));
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_62_L,(*(data + 40) << 8) |  *(data + 41));
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_63_L,(*(data + 42) << 8) |  *(data + 43));
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_64_L,(*(data + 44) << 8) |  *(data + 45));
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_65_L,(*(data + 46) << 8) |  *(data + 47));

}

MS_U16 MHal_XC_CFD_SetMainCtrl(ST_KDRV_XC_CFD_FIRE *pstCfdFire)
{
    STU_CFDAPI_MAIN_CONTROL stMainControl;
    MS_U16 u16RetVal = 0;
    MS_U8 u8TmoLevel = 1;
    memset(&stMainControl, 0, sizeof(STU_CFDAPI_MAIN_CONTROL));

    stMainControl.u32Version = 0;
    stMainControl.u16Length = sizeof(STU_CFDAPI_MAIN_CONTROL);
    stMainControl.u8Input_Source = pstCfdFire->u8InputSource;
    u8HDMI_case = pstCfdFire->u8InputSource;
    if (CFD_IS_HDMI(pstCfdFire->u8InputSource))
    {
        // HDMI
        stMainControl.u16DolbySupportStatus = !MHal_XC_DolbyHWBonded();     //check is support DolbyHDR
        stMainControl.u8Input_Format = MHal_XC_HDMI_Color_Format(_stCfdHdmi[pstCfdFire->u8Win].u8PixelFormat, _stCfdHdmi[pstCfdFire->u8Win].u8Colorimetry, _stCfdHdmi[pstCfdFire->u8Win].u8ExtendedColorimetry);
        stMainControl.u8Input_DataFormat = MHal_XC_HDMI_Color_Data_Format(_stCfdHdmi[pstCfdFire->u8Win].u8PixelFormat);
        if ((_stCfdOsd[pstCfdFire->u8Win].u8UltraWhiteLevel == 0) && (_stCfdOsd[pstCfdFire->u8Win].u8UltraBlackLevel == 0))
        {
            if (_stCfdOsd[pstCfdFire->u8Win].u8ColorRange == 0)
            {
                stMainControl.u8Input_IsFullRange = _stCfdHdmi[pstCfdFire->u8Win].bIsFullRange;
            }
            else
            {
                stMainControl.u8Input_IsFullRange = (_stCfdOsd[pstCfdFire->u8Win].u8ColorRange==1)?(1):(0);
            }
        }
        else
        {
            stMainControl.u8Input_IsFullRange = 1;
        }

        if (_bEnableHDR == TRUE)
        {
            stMainControl.u8Input_HDRMode = E_CFIO_MODE_HDR1;
        }
        else
        {
            if (_stCfdHdmi[pstCfdFire->u8Win].u8EOTF == 2)
            {
                // HDR10
                stMainControl.u8Input_HDRMode = E_CFIO_MODE_HDR2;
            }
            else if (_stCfdHdmi[pstCfdFire->u8Win].u8EOTF == 3)
            {
                // HLG
                stMainControl.u8Input_HDRMode = E_CFIO_MODE_HDR3;
            }
            else
            {
                stMainControl.u8Input_HDRMode = E_CFIO_MODE_SDR;
            }
        }
    }
    else if (CFD_IS_MM(pstCfdFire->u8InputSource) || CFD_IS_DTV(pstCfdFire->u8InputSource))
    {
        // MM/DTV
        STU_CFDAPI_MM_PARSER stMMParam;
        ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info stFormatInfo;
        memset(&stMMParam, 0, sizeof(STU_CFDAPI_MM_PARSER));

        MHal_XC_CFD_WithdrawMMParam(&stFormatInfo, pstCfdFire->u8Win);
        _MHal_XC_CFD_Convert_XCDRAM_to_CFDMMParam(pstCfdFire, &stMMParam,&stFormatInfo);

        stMainControl.u8Input_Format = 7;
        stMainControl.u8Input_DataFormat = 2;
        if(MHal_XC_ShareMemIsAvailable(&stFormatInfo))
        {
            stMainControl.u8Input_IsFullRange = stMMParam.u8Video_Full_Range_Flag;
        }else{
            stMainControl.u8Input_IsFullRange = _stCfdAnalog[pstCfdFire->u8Win].bIsFullRange;
            stMainControl.u8Input_ext_Colour_primaries = _stCfdAnalog[pstCfdFire->u8Win].u8ColorPrimaries;
            stMainControl.u8Input_ext_Transfer_Characteristics = _stCfdAnalog[pstCfdFire->u8Win].u8TransferCharacteristics;
            stMainControl.u8Input_ext_Matrix_Coeffs = _stCfdAnalog[pstCfdFire->u8Win].u8MatrixCoefficients;
        }

        if (_bEnableHDR == TRUE)
        {
            stMainControl.u8Input_HDRMode = E_CFIO_MODE_HDR1;
        }
        else
        {
            if (stFormatInfo.HDRMemFormat.stHDRMemFormatCFD.bVUIValid == TRUE)
            {
                if (stMMParam.u8Transfer_Characteristics == 16)
                {
                    // HDR10
                    stMainControl.u8Input_HDRMode = E_CFIO_MODE_HDR2;
                }
                else if (stMMParam.u8Transfer_Characteristics == 18)
                {
                    // HLG
                    stMainControl.u8Input_HDRMode = E_CFIO_MODE_HDR3;
                }
                else
                {
                    stMainControl.u8Input_HDRMode = E_CFIO_MODE_SDR;
                }
            }
            else
            {
                stMainControl.u8Input_HDRMode = E_CFIO_MODE_SDR;
            }
        }
    }
    else
    {
        // Analog , wait Dixon modify
        stMainControl.u8Input_Format = _stCfdAnalog[pstCfdFire->u8Win].u8ColorFormat;
        stMainControl.u8Input_DataFormat = _stCfdAnalog[pstCfdFire->u8Win].u8ColorDataFormat;
        stMainControl.u8Input_IsFullRange = _stCfdAnalog[pstCfdFire->u8Win].bIsFullRange;
        stMainControl.u8Input_ext_Colour_primaries = _stCfdAnalog[pstCfdFire->u8Win].u8ColorPrimaries;
        stMainControl.u8Input_ext_Transfer_Characteristics = _stCfdAnalog[pstCfdFire->u8Win].u8TransferCharacteristics;
        stMainControl.u8Input_ext_Matrix_Coeffs = _stCfdAnalog[pstCfdFire->u8Win].u8MatrixCoefficients;
        stMainControl.u8Input_HDRMode = E_CFIO_MODE_SDR;
    }

    if (_stCfdTmo.u16Length > 0)
    {
        MS_U8 i;

        u8TmoLevel = _stCfdHdr[pstCfdFire->u8Win].u8TmoLevel;

        if(u8TmoLevel >= _stCfdTmo.u16LevelCount)
        {
            printk("[%s %d]ERROR level is invalid\n", __FUNCTION__,__LINE__);
        }

        if(stMainControl.u8Input_HDRMode == E_CFIO_MODE_HDR2)
        {
            for(i=0; i<_stCfdTmo.u16LevelCount; i++)
            {
                if((E_KDRV_XC_CFD_HDR_TYPE_OPEN == (_stCfdTmo.pstCfdTmoLevel + i)->u16HdrType) && u8TmoLevel == (_stCfdTmo.pstCfdTmoLevel + i)->u16Level)
                {
                    MS_U16 u16ControlSize = (_stCfdTmo.pstCfdTmoLevel + i)->u16ControlSize;
                    if(u16ControlSize == TMO_CONTROL_VERSION_V0)
                    {
                        MHal_XC_SetManualTMOParemeter((_stCfdTmo.pstCfdTmoLevel + i)->pu8data);
                    }
                    else if(u16ControlSize == TMO_CONTROL_VERSION_V1)
                    {
                        MS_BOOL bManual = (*((_stCfdTmo.pstCfdTmoLevel + i)->pu8data + 48) & BIT(0));    //manual or auto flag  0:auto  1: manual
                        MS_BOOL bHlgOotfEn = (*((_stCfdTmo.pstCfdTmoLevel + i)->pu8data + 48) & BIT(3)) >> 3;
                        MS_U8 u8OOTFGamma = *((_stCfdTmo.pstCfdTmoLevel + i)->pu8data + 49);
                        MS_U8 u8TMOHLGPow = *((_stCfdTmo.pstCfdTmoLevel + i)->pu8data + 50);
                        MS_U16 u16Alpha = (*((_stCfdTmo.pstCfdTmoLevel + i)->pu8data + 51) << 8) + (*((_stCfdTmo.pstCfdTmoLevel + i)->pu8data + 52));
                        MS_U16 u16Beta = (*((_stCfdTmo.pstCfdTmoLevel + i)->pu8data + 53) << 8) + (*((_stCfdTmo.pstCfdTmoLevel + i)->pu8data + 54));

                        MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK5F_66_L, bHlgOotfEn << 3, BIT(3));
                        MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK5F_67_L, u8TMOHLGPow, 0xFF);
                        MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK5F_67_L, u8OOTFGamma << 8, 0xFF00);
                        MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_68_L, u16Alpha);
                        MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_69_L, u16Beta);

                        if(bManual)
                        {
                            MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK5F_66_L, 0, BIT(0));
                            MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_5A_L, (*((_stCfdTmo.pstCfdTmoLevel + i)->pu8data + 22) << 8) |  *((_stCfdTmo.pstCfdTmoLevel + i)->pu8data + 23));
                            MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_65_L, (*((_stCfdTmo.pstCfdTmoLevel + i)->pu8data + 46) << 8) |  *((_stCfdTmo.pstCfdTmoLevel + i)->pu8data + 47));
                        }
                        else
                        {
                            MHal_XC_SetManualTMOParemeter((_stCfdTmo.pstCfdTmoLevel + i)->pu8data);
                        }
                    }
                    break;
                }
            }
        }
        else if(stMainControl.u8Input_HDRMode == E_CFIO_MODE_HDR3)
        {
            for(i=0; i<_stCfdTmo.u16LevelCount; i++)
            {
                if((E_KDRV_XC_CFD_HDR_TYPE_HLG == (_stCfdTmo.pstCfdTmoLevel + i)->u16HdrType) && u8TmoLevel == (_stCfdTmo.pstCfdTmoLevel + i)->u16Level)
                {
                    MS_U16 u16ControlSize = (_stCfdTmo.pstCfdTmoLevel + i)->u16ControlSize;
                    if(u16ControlSize == TMO_CONTROL_VERSION_V0)
                    {
                        MHal_XC_SetManualTMOParemeter((_stCfdTmo.pstCfdTmoLevel + i)->pu8data);
                    }
                    else if(u16ControlSize == TMO_CONTROL_VERSION_V1)
                    {
                        MS_BOOL bManual = (*((_stCfdTmo.pstCfdTmoLevel + i)->pu8data + 48) & BIT(0));    //manual or auto flag  0:auto  1: manual
                        MS_BOOL bHlgOotfEn = (*((_stCfdTmo.pstCfdTmoLevel + i)->pu8data + 48) & BIT(3)) >> 3;
                        MS_U8 u8TMOHLGPow = *((_stCfdTmo.pstCfdTmoLevel + i)->pu8data + 49);
                        MS_U8 u8OOTFGamma = *((_stCfdTmo.pstCfdTmoLevel + i)->pu8data + 50);
                        MS_U16 u16Alpha = (*((_stCfdTmo.pstCfdTmoLevel + i)->pu8data + 51)) + (*((_stCfdTmo.pstCfdTmoLevel + i)->pu8data + 52) << 8);
                        MS_U16 u16Beta = (*((_stCfdTmo.pstCfdTmoLevel + i)->pu8data + 53)) + (*((_stCfdTmo.pstCfdTmoLevel + i)->pu8data + 54) << 8);

                        MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK5F_66_L, bHlgOotfEn << 3, BIT(3));
                        MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK5F_67_L, u8TMOHLGPow, 0xFF);
                        MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK5F_67_L, u8OOTFGamma << 8, 0xFF00);
                        MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_68_L, u16Alpha);
                        MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_69_L, u16Beta);
                        if(bManual)
                        {
                            MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK5F_66_L, 0, BIT(0));
                            MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_5A_L, (*((_stCfdTmo.pstCfdTmoLevel + i)->pu8data + 22) << 8) |  *((_stCfdTmo.pstCfdTmoLevel + i)->pu8data + 23));
                            MApi_GFLIP_XC_W2BYTE(REG_SC_BK5F_65_L, (*((_stCfdTmo.pstCfdTmoLevel + i)->pu8data + 46) << 8) |  *((_stCfdTmo.pstCfdTmoLevel + i)->pu8data + 47));
                        }
                        else
                        {
                            MHal_XC_SetManualTMOParemeter((_stCfdTmo.pstCfdTmoLevel + i)->pu8data);
                        }
                    }
                    break;
                }
            }
        }
    }


    //stMainControl.u8Input_HDRMode = _stCfdHdr[pstCfdFire->u8Win].u8HdrType;
    stMainControl.u8Input_IsRGBBypass = pstCfdFire->bIsRgbBypass;
    stMainControl.u8Input_SDRIPMode = 1;
    stMainControl.u8Input_HDRIPMode = 1;
    /* No idea.
    stMainControl.stu_Middle_Format[0].u8Mid_Format_Mode = ;
    stMainControl.stu_Middle_Format[0].u8Mid_Format =;
    stMainControl.stu_Middle_Format[0].u8Mid_DataFormat = ;
    stMainControl.stu_Middle_Format[0].u8Mid_IsFullRange = ;
    stMainControl.stu_Middle_Format[0].u8Mid_HDRMode = ;
    stMainControl.stu_Middle_Format[0].u8Mid_Colour_primaries = ;
    stMainControl.stu_Middle_Format[0].u8Mid_Transfer_Characteristics = ;
    stMainControl.stu_Middle_Format[0].u8Mid_Matrix_Coeffs = ;
    */
    //_eCfdOutputType = E_CFD_OUTPUT_SOURCE_PANEL;
    if (_eCfdOutputType == E_CFD_OUTPUT_SOURCE_HDMI)
    {
        stMainControl.u8Output_Source = E_CFD_OUTPUT_SOURCE_HDMI;
        stMainControl.u8Output_Format = E_CFD_CFIO_RGB_BT709;
        stMainControl.u8Output_DataFormat = E_CFD_MC_FORMAT_RGB;
        stMainControl.u8Output_IsFullRange = 1;
        stMainControl.u8Output_HDRMode = E_CFIO_MODE_SDR;
        stMainControl.u8PanelOutput_GammutMapping_Mode = 0;
        stMainControl.u8HDMIOutput_GammutMapping_Mode = 1;
    }
    else
    {
        stMainControl.u8Output_Source = E_CFD_OUTPUT_SOURCE_PANEL;
        stMainControl.u8Output_Format = _stCfdPanel.u8ColorFormat;
        stMainControl.u8Output_DataFormat = _stCfdPanel.u8ColorDataFormat;
        stMainControl.u8Output_IsFullRange = _stCfdPanel.bIsFullRange;
        stMainControl.u8Output_HDRMode = E_CFIO_MODE_SDR;
        if (stMainControl.u8Input_HDRMode == E_CFIO_MODE_HDR1)
        {
            stMainControl.u8PanelOutput_GammutMapping_Mode = 0;
        }
        else if (stMainControl.u8Input_HDRMode == E_CFIO_MODE_HDR2)
        {
            stMainControl.u8PanelOutput_GammutMapping_Mode = _stCfdPanel.bLinearRgb;
        }
        else
        {
            stMainControl.u8PanelOutput_GammutMapping_Mode = _stCfdPanel.bLinearRgb & _stCfdLinearRgb.bEnable;
        }
        stMainControl.u8HDMIOutput_GammutMapping_Mode = 0;
    }

    stMainControl.u8HDMIOutput_GammutMapping_MethodMode = 0;
    stMainControl.u8MMInput_ColorimetryHandle_Mode = 0;
    stMainControl.u8TMO_TargetRefer_Mode = 1;
    stMainControl.u16Source_Max_Luminance = 0;    //data * 1 nits
    stMainControl.u16Source_Med_Luminance = 0;   //data * 1 nits
    stMainControl.u16Source_Min_Luminance = 0;    //data * 0.0001 nits
    stMainControl.u16Target_Med_Luminance = 0;   //data * 1 nits
    stMainControl.u16Target_Max_Luminance = 0;    //data * 1 nits
    stMainControl.u16Target_Min_Luminance = 0;    //data * 0.0001 nits

    u16RetVal = Mapi_Cfd_inter_Main_Control_Param_Check(&stMainControl);
    if (u16RetVal == E_CFD_MC_ERR_NOERR)
    {
        Mapi_Cfd_inter_Main_Control_Param_Set(&stMainControl);
        _stCfdMainControl = stMainControl;
    }
    else
    {
        printk("Mapi_Cfd_inter_Main_Control_Param_Check fail, errCode: %d\n", u16RetVal);
    }

    return u16RetVal;
}

MS_S32 MHal_XC_CFD_DepositeMMParam(ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info *pstFormatInfo, MS_U8 u8Win)
{
    if (pstFormatInfo==NULL)
    {
        return -1;
    }

    memcpy(&gstDRAMFormatInfo[u8Win], pstFormatInfo, sizeof(ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info));
    return E_CFD_MC_ERR_NOERR;
}

MS_S32 MHal_XC_CFD_WithdrawMMParam(ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info *pstFormatInfo, MS_U8 u8Win)
{
    if (pstFormatInfo == NULL)
    {
        return -1;
    }
    memcpy(pstFormatInfo, &gstDRAMFormatInfo[u8Win], sizeof(ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info));

    return E_CFD_MC_ERR_NOERR;
}

MS_U16 MHal_XC_CFD_SetMmParam(ST_KDRV_XC_CFD_FIRE *pstCfdFire, MS_BOOL *pbChanged)
{
    STU_CFDAPI_MM_PARSER stMMParam;
    ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info stFormatInfo;
    MS_U16 u16RetVal = 0;
    memset(&stMMParam, 0, sizeof(STU_CFDAPI_MM_PARSER));

    MHal_XC_CFD_WithdrawMMParam(&stFormatInfo, pstCfdFire->u8Win);
    _MHal_XC_CFD_Convert_XCDRAM_to_CFDMMParam(pstCfdFire, &stMMParam,&stFormatInfo);

    if(!MHal_XC_ShareMemIsAvailable(&stFormatInfo))
    {
        stMMParam.u8Video_Full_Range_Flag = _stCfdAnalog[pstCfdFire->u8Win].bIsFullRange;
        stMMParam.u8Colour_primaries = _stCfdAnalog[pstCfdFire->u8Win].u8ColorPrimaries;
        stMMParam.u8Transfer_Characteristics = _stCfdAnalog[pstCfdFire->u8Win].u8TransferCharacteristics;
        stMMParam.u8Matrix_Coeffs = _stCfdAnalog[pstCfdFire->u8Win].u8MatrixCoefficients;
    }

    *pbChanged = FALSE;
    if (memcmp(&stMMParam, &_stCfdMm[pstCfdFire->u8Win], sizeof(STU_CFDAPI_MM_PARSER)) != 0)
    {
        *pbChanged = TRUE;
    }
    memcpy(&_stCfdMm[pstCfdFire->u8Win], &stMMParam, sizeof(STU_CFDAPI_MM_PARSER));

    u16RetVal = Mapi_Cfd_inter_MM_Param_Check(&stMMParam);
    if (u16RetVal == E_CFD_MC_ERR_NOERR)
    {
        Mapi_Cfd_inter_MM_Param_Set(&stMMParam);
    }
    else
    {
        printk("Mapi_Cfd_inter_MM_Param_Check fail, errCode: %d\n", u16RetVal);
    }

    return u16RetVal;
}

MS_U16 MHal_XC_CFD_SetHdmiParam(ST_KDRV_XC_CFD_FIRE *pstCfdFire)
{
    STU_CFDAPI_HDMI_INFOFRAME_PARSER stHDMIInfoFrameParam;
    MS_U16 u16RetVal = 0;
    memset(&stHDMIInfoFrameParam, 0, sizeof(STU_CFDAPI_HDMI_INFOFRAME_PARSER));
    stHDMIInfoFrameParam.u32Version = 0;
    stHDMIInfoFrameParam.u16Length = sizeof(STU_CFDAPI_HDMI_INFOFRAME_PARSER);
    stHDMIInfoFrameParam.u8HDMISource_HDR_InfoFrame_Valid = _stCfdHdmi[pstCfdFire->u8Win].bHDRInfoFrameValid;
    stHDMIInfoFrameParam.u8HDMISource_EOTF = _stCfdHdmi[pstCfdFire->u8Win].u8EOTF;
    stHDMIInfoFrameParam.u8HDMISource_SMD_ID = _stCfdHdmi[pstCfdFire->u8Win].u8SMDID;
    stHDMIInfoFrameParam.u16Master_Panel_Max_Luminance = _stCfdHdmi[pstCfdFire->u8Win].u16MasterPanelMaxLuminance;    //data * 1 nits
    stHDMIInfoFrameParam.u16Master_Panel_Min_Luminance = _stCfdHdmi[pstCfdFire->u8Win].u16MasterPanelMinLuminance;    //data * 0.0001 nits
    stHDMIInfoFrameParam.u16Max_Content_Light_Level = _stCfdHdmi[pstCfdFire->u8Win].u16MaxContentLightLevel;       //data * 1 nits
    stHDMIInfoFrameParam.u16Max_Frame_Avg_Light_Level = _stCfdHdmi[pstCfdFire->u8Win].u16MaxFrameAvgLightLevel;     //data * 1 nits
    stHDMIInfoFrameParam.stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_x[0] = _stCfdHdmi[pstCfdFire->u8Win].u16Display_Primaries_x[0];
    stHDMIInfoFrameParam.stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_x[1] = _stCfdHdmi[pstCfdFire->u8Win].u16Display_Primaries_x[1];
    stHDMIInfoFrameParam.stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_x[2] = _stCfdHdmi[pstCfdFire->u8Win].u16Display_Primaries_x[2];
    stHDMIInfoFrameParam.stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_y[0] = _stCfdHdmi[pstCfdFire->u8Win].u16Display_Primaries_y[0];
    stHDMIInfoFrameParam.stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_y[1] = _stCfdHdmi[pstCfdFire->u8Win].u16Display_Primaries_y[1];
    stHDMIInfoFrameParam.stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_y[2] = _stCfdHdmi[pstCfdFire->u8Win].u16Display_Primaries_y[2];
    stHDMIInfoFrameParam.stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16White_point_x = _stCfdHdmi[pstCfdFire->u8Win].u16White_point_x;
    stHDMIInfoFrameParam.stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16White_point_y = _stCfdHdmi[pstCfdFire->u8Win].u16White_point_y;
    stHDMIInfoFrameParam.u8Mastering_Display_Infor_Valid = _stCfdHdmi[pstCfdFire->u8Win].u8SMDID==0?1:0;
    stHDMIInfoFrameParam.u8HDMISource_Support_Format = (_stCfdHdmi[pstCfdFire->u8Win].u8RgbQuantizationRange<<5) | (_stCfdHdmi[pstCfdFire->u8Win].u8YccQuantizationRange<<3) |(_stCfdHdmi[pstCfdFire->u8Win].u8PixelFormat);
    stHDMIInfoFrameParam.u8HDMISource_Colorspace = MHal_XC_HDMI_Color_Format(_stCfdHdmi[pstCfdFire->u8Win].u8PixelFormat, _stCfdHdmi[pstCfdFire->u8Win].u8Colorimetry, _stCfdHdmi[pstCfdFire->u8Win].u8ExtendedColorimetry);

    u16RetVal = Mapi_Cfd_inter_HDMI_InfoFrame_Param_Check(&stHDMIInfoFrameParam);
    if (u16RetVal == E_CFD_MC_ERR_NOERR)
    {
        Mapi_Cfd_inter_HDMI_InfoFrame_Param_Set(&stHDMIInfoFrameParam);
    }
    else
    {
        printk("Mapi_Cfd_inter_HDMI_InfoFrame_Param_Check fail, errCode: %d\n", u16RetVal);
    }

    return u16RetVal;
}

MS_U16 MHal_XC_CFD_SetHdrParam(ST_KDRV_XC_CFD_FIRE *pstCfdFire)
{
    MS_U16 u16RetVal = 0;
    STU_CFDAPI_HDR_METADATA_FORMAT stHDRMetadataFormatParam;
    memset(&stHDRMetadataFormatParam, 0, sizeof(STU_CFDAPI_HDR_METADATA_FORMAT));
    stHDRMetadataFormatParam.u32Version = CFD_HDMI_HDR_METADATA_VERSION;
    stHDRMetadataFormatParam.u16Length = sizeof(STU_CFDAPI_HDR_METADATA_FORMAT);
    if (_bEnableHDR == TRUE)
    {
        stHDRMetadataFormatParam.stu_Cfd_Dolby_HDR_Param.u8IsDolbyHDREn = 1;
    }
    else
    {
        stHDRMetadataFormatParam.stu_Cfd_Dolby_HDR_Param.u8IsDolbyHDREn = 0;
    }
    //stHDRMetadataFormatParam.stu_Cfd_Dolby_HDR_Param.u8IsDolbyHDREn = _stCfdHdr[pstCfdFire->u8Win].u8HdrType==1?1:0;

    u16RetVal = Mapi_Cfd_inter_HDR_Metadata_Param_Check(&stHDRMetadataFormatParam);
    if (u16RetVal == E_CFD_MC_ERR_NOERR)
    {
        Mapi_Cfd_inter_HDR_Metadata_Param_Set(&stHDRMetadataFormatParam);
    }
    else
    {
        printk("Mapi_Cfd_inter_HDR_Metadata_Param_Check fail, errCode: %d\n", u16RetVal);
    }

    return u16RetVal;
}
#endif
//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
void MHal_XC_W2BYTE(DWORD u32Reg, WORD u16Val )
{
    REG_W2B(u32Reg, u16Val);
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
WORD MHal_XC_R2BYTE(DWORD u32Reg )
{
    return REG_RR(u32Reg) ;
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
void MHal_XC_W2BYTEMSK(DWORD u32Reg, WORD u16Val, WORD u16Mask )
{
    WORD u16Data=0 ;
    u16Data = REG_RR(u32Reg);
    u16Data = (u16Data & (0xFFFF-u16Mask))|(u16Val &u16Mask);
    REG_W2B(u32Reg, u16Data);
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
WORD MHal_XC_R2BYTEMSK(DWORD u32Reg, WORD u16Mask )
{
    return (REG_RR(u32Reg) & u16Mask);
}

void MHal_XC_WriteByte(DWORD u32Reg, BYTE u8Val)
{
    if(u32Reg%2)
    {
        REG_WH((u32Reg-1), u8Val);
    }
    else
    {
        REG_WL(u32Reg, u8Val);
    }
}

void MHal_XC_WriteByteMask(DWORD u32Reg, BYTE u8Val, WORD u16Mask)
{
    if(u32Reg%2)
    {
        REG_WHMSK((u32Reg-1), u8Val, u16Mask);
    }
    else
    {
        REG_WLMSK(u32Reg, u8Val, u16Mask);
    }
}

BYTE MHal_XC_ReadByte(DWORD u32Reg)
{
    if(u32Reg%2)
    {
        u32Reg = u32Reg-1 ;
        return ((REG_RR(u32Reg) & 0xFF00)>>8);
    }
    else
    {
        return (REG_RR(u32Reg) & 0xFF);
    }
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
BOOL MHal_XC_IsBlackVideoEnable( MS_BOOL bWindow )
{
    MS_BOOL bReturn=FALSE;

    if( MAIN_WINDOW == bWindow)
    {
        if(MHal_XC_R2BYTEMSK(REG_SC_BK10_19_L,BIT1))
        {
            bReturn = TRUE;
        }
        else
        {
            bReturn = FALSE;
        }
    }

    return bReturn;
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
#if (CONFIG_MSTAR_FRC_SOFTWARE_TOGGLE == 1)
void MHal_XC_FRCR2SoftwareToggle( void )
{
    MHal_XC_W2BYTEMSK(REG_SC_BK00_14_L, 0x00, BIT4);

    if (MHal_XC_R2BYTEMSK(REG_SC_BK00_10_L, BIT4) == BIT4) // output Vsync case
    {
        //MHal_XC_W2BYTEMSK(REG_SC_BK00_12_L, BIT4, BIT4); // Clean job will be done in scaler
        FRC_CPU_INT_REG(REG_FRCINT_HKCPUFIRE) = INT_HKCPU_FRCR2_INPUT_SYNC;     //to FRC-R2
        FRC_CPU_INT_REG(REG_FRCINT_HKCPUFIRE) = INT_HKCPU_FRCR2_OUTPUT_SYNC;    //to FRC-R2
        mb();
        FRC_CPU_INT_REG(REG_FRCINT_HKCPUFIRE) = 0;
    }
}
#endif

#ifdef CONFIG_MSTAR_XC_HDR_SUPPORT
//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
static MS_U16 _HAL_XC_ReadMetadata2BYTE(MS_U16 u16Addr)
{
    MS_U16 u16FlagValue = 0;
    MS_U16 u16Value = 0;
    struct timespec startTs;
    struct timespec endTs;
    // set descrb read back addr
    MHal_XC_W2BYTEMSK(REG_SC_BK79_12_L, u16Addr, 0x00FF);
    // set check flag
    MHal_XC_W2BYTEMSK(REG_SC_BK79_12_L, (u16Addr % 16) << 12, 0xF000);
    // read check flag
    u16FlagValue = MHal_XC_R2BYTEMSK(REG_SC_BK79_14_L, 0xF000) >> 12;


    getnstimeofday(&startTs);
    // wait read-back flag == set flag, longest time = 4/350ms
    while (u16FlagValue != (u16Addr % 16))
    {
        long long diff = 0;
        // timeout is 1s
        getnstimeofday(&endTs);
        diff = endTs.tv_nsec + ((endTs.tv_sec - startTs.tv_sec) * 1000000000 - startTs.tv_nsec);
        if (diff > 1000000000)
        {
            printk("[XC][%s:%05d] wait read-back flag == set flag timeout!!!", __FUNCTION__, __LINE__);
            break;
        }
        u16FlagValue = MHal_XC_R2BYTEMSK(REG_SC_BK79_14_L, 0xF000) >> 12;
    }
    // read back metadata
    u16Value = MHal_XC_R2BYTE(REG_SC_BK79_13_L);

    return u16Value;
}

BOOL MHal_XC_GetMiuOffset(MS_U32 u32MiuNo, MS_U32 *pu32Offset)
{
    if (u32MiuNo == 0)
    {
        *pu32Offset = MSTAR_MIU0_BUS_BASE;
    }
    else if (u32MiuNo == 1)
    {
        *pu32Offset = MSTAR_MIU1_BUS_BASE;
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

#define DOLBY_VISION_SW_DESCRAMBLE_ENABLE 1
#if (DOLBY_VISION_SW_DESCRAMBLE_ENABLE == 1)
typedef struct
{
    MS_U8 metadata_length_hi;
    MS_U8 metadata_length_lo;
    MS_U8 metadata_body[119];
} FirstPacketBody;

typedef struct
{
    MS_U8 metadata_body[121];
} FollowingPacketBody;

typedef struct
{
    MS_U8 header0;
    MS_U8 header1;
    MS_U8 header2;
    union
    {
        /* this is used by packet type 0 or 1 */
        FirstPacketBody metadata_first;
        /* this is used by packet type 2 or 3 */
        FollowingPacketBody metadata_following;
    } metadata;
    MS_U8 crc3;
    MS_U8 crc2;
    MS_U8 crc1;
    MS_U8 crc0;
} EDRMetaDataPacket;

#define PACKET_TYPE_SINGLE      0
#define PACKET_TYPE_FIRST       1
#define PACKET_TYPE_INTERM      2
#define PACKET_TYPE_LAST        3

#define FIELD_MASK_PACKET_TYPE 0xc0 /*Metadata is hide in */
#define BitSet(c, mask, bit_value)  (((bit_value) * (mask)) | ((c) & ~(mask)))
#define input_frame_size (1920*1080*3)
#define PACKET_REPEAT    3
#define interval  (sizeof(EDRMetaDataPacket) * 8 * u8FrameNumber * 3)
#define MAX_DM_EXT_BLOCKS 255

typedef struct dm_metadata_base_s
{
    /* signal attributes */
    unsigned char dm_metadata_id; // affected_dm_metadata_id<<4|current_dm_metadata_id
    unsigned char scene_refresh_flag;
    unsigned char YCCtoRGB_coef0_hi;
    unsigned char YCCtoRGB_coef0_lo;
    unsigned char YCCtoRGB_coef1_hi;
    unsigned char YCCtoRGB_coef1_lo;
    unsigned char YCCtoRGB_coef2_hi;
    unsigned char YCCtoRGB_coef2_lo;
    unsigned char YCCtoRGB_coef3_hi;
    unsigned char YCCtoRGB_coef3_lo;
    unsigned char YCCtoRGB_coef4_hi;
    unsigned char YCCtoRGB_coef4_lo;
    unsigned char YCCtoRGB_coef5_hi;
    unsigned char YCCtoRGB_coef5_lo;
    unsigned char YCCtoRGB_coef6_hi;
    unsigned char YCCtoRGB_coef6_lo;
    unsigned char YCCtoRGB_coef7_hi;
    unsigned char YCCtoRGB_coef7_lo;
    unsigned char YCCtoRGB_coef8_hi;
    unsigned char YCCtoRGB_coef8_lo;
    unsigned char YCCtoRGB_offset0_byte3;
    unsigned char YCCtoRGB_offset0_byte2;
    unsigned char YCCtoRGB_offset0_byte1;
    unsigned char YCCtoRGB_offset0_byte0;
    unsigned char YCCtoRGB_offset1_byte3;
    unsigned char YCCtoRGB_offset1_byte2;
    unsigned char YCCtoRGB_offset1_byte1;
    unsigned char YCCtoRGB_offset1_byte0;
    unsigned char YCCtoRGB_offset2_byte3;
    unsigned char YCCtoRGB_offset2_byte2;
    unsigned char YCCtoRGB_offset2_byte1;
    unsigned char YCCtoRGB_offset2_byte0;
    unsigned char RGBtoLMS_coef0_hi;
    unsigned char RGBtoLMS_coef0_lo;
    unsigned char RGBtoLMS_coef1_hi;
    unsigned char RGBtoLMS_coef1_lo;
    unsigned char RGBtoLMS_coef2_hi;
    unsigned char RGBtoLMS_coef2_lo;
    unsigned char RGBtoLMS_coef3_hi;
    unsigned char RGBtoLMS_coef3_lo;
    unsigned char RGBtoLMS_coef4_hi;
    unsigned char RGBtoLMS_coef4_lo;
    unsigned char RGBtoLMS_coef5_hi;
    unsigned char RGBtoLMS_coef5_lo;
    unsigned char RGBtoLMS_coef6_hi;
    unsigned char RGBtoLMS_coef6_lo;
    unsigned char RGBtoLMS_coef7_hi;
    unsigned char RGBtoLMS_coef7_lo;
    unsigned char RGBtoLMS_coef8_hi;
    unsigned char RGBtoLMS_coef8_lo;
    unsigned char signal_eotf_hi;
    unsigned char signal_eotf_lo;
    unsigned char signal_eotf_param0_hi;
    unsigned char signal_eotf_param0_lo;
    unsigned char signal_eotf_param1_hi;
    unsigned char signal_eotf_param1_lo;
    unsigned char signal_eotf_param2_byte3;
    unsigned char signal_eotf_param2_byte2;
    unsigned char signal_eotf_param2_byte1;
    unsigned char signal_eotf_param2_byte0;
    unsigned char signal_bit_depth;
    unsigned char signal_color_space;
    unsigned char signal_chroma_format;
    unsigned char signal_full_range_flag;
    /* source display attributes */
    unsigned char source_min_PQ_hi;
    unsigned char source_min_PQ_lo;
    unsigned char source_max_PQ_hi;
    unsigned char source_max_PQ_lo;
    unsigned char source_diagonal_hi;
    unsigned char source_diagonal_lo;
    /* extended metadata */
    unsigned char num_ext_blocks;
} dm_metadata_base_t;

typedef struct ext_level_2_s
{
    unsigned char target_max_PQ_hi       ;
    unsigned char target_max_PQ_lo       ;
    unsigned char trim_slope_hi          ;
    unsigned char trim_slope_lo          ;
    unsigned char trim_offset_hi         ;
    unsigned char trim_offset_lo         ;
    unsigned char trim_power_hi          ;
    unsigned char trim_power_lo          ;
    unsigned char trim_chroma_weight_hi  ;
    unsigned char trim_chroma_weight_lo  ;
    unsigned char trim_saturation_gain_hi;
    unsigned char trim_saturation_gain_lo;
    unsigned char ms_weight_hi           ;
    unsigned char ms_weight_lo           ;
} ext_level_2_t;

typedef struct ext_level_1_s
{
    unsigned char min_PQ_hi;
    unsigned char min_PQ_lo;
    unsigned char max_PQ_hi;
    unsigned char max_PQ_lo;
    unsigned char avg_PQ_hi;
    unsigned char avg_PQ_lo;
} ext_level_1_t;


typedef struct dm_metadata_ext_s
{
    unsigned char ext_block_length_byte3;
    unsigned char ext_block_length_byte2;
    unsigned char ext_block_length_byte1;
    unsigned char ext_block_length_byte0;
    unsigned char ext_block_level;
    union
    {
        ext_level_1_t level_1;
        ext_level_2_t level_2;
    } l;
} dm_metadata_ext_t;

typedef struct dm_metadata_s
{
    dm_metadata_base_t base;
    dm_metadata_ext_t ext[MAX_DM_EXT_BLOCKS];
} dm_metadata_t;

static MS_U32 crc32_lut[256] =
{
    0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b,
    0x1a864db2, 0x1e475005, 0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
    0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd, 0x4c11db70, 0x48d0c6c7,
    0x4593e01e, 0x4152fda9, 0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
    0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3,
    0x709f7b7a, 0x745e66cd, 0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
    0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5, 0xbe2b5b58, 0xbaea46ef,
    0xb7a96036, 0xb3687d81, 0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
    0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49, 0xc7361b4c, 0xc3f706fb,
    0xceb42022, 0xca753d95, 0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
    0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d, 0x34867077, 0x30476dc0,
    0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
    0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16, 0x018aeb13, 0x054bf6a4,
    0x0808d07d, 0x0cc9cdca, 0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
    0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02, 0x5e9f46bf, 0x5a5e5b08,
    0x571d7dd1, 0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
    0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 0xbfa1b04b, 0xbb60adfc,
    0xb6238b25, 0xb2e29692, 0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
    0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a, 0xe0b41de7, 0xe4750050,
    0xe9362689, 0xedf73b3e, 0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
    0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34,
    0xdc3abded, 0xd8fba05a, 0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
    0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb, 0x4f040d56, 0x4bc510e1,
    0x46863638, 0x42472b8f, 0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
    0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5,
    0x3f9b762c, 0x3b5a6b9b, 0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
    0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623, 0xf12f560e, 0xf5ee4bb9,
    0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
    0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f, 0xc423cd6a, 0xc0e2d0dd,
    0xcda1f604, 0xc960ebb3, 0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
    0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b, 0x9b3660c6, 0x9ff77d71,
    0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
    0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640, 0x4e8ee645, 0x4a4ffbf2,
    0x470cdd2b, 0x43cdc09c, 0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
    0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24, 0x119b4be9, 0x155a565e,
    0x18197087, 0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
    0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d, 0x2056cd3a,
    0x2d15ebe3, 0x29d4f654, 0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
    0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c, 0xe3a1cbc1, 0xe760d676,
    0xea23f0af, 0xeee2ed18, 0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
    0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662,
    0x933eb0bb, 0x97ffad0c, 0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
    0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};


MS_U32 crc32_dolby(MS_U32 crc, const void *buf, size_t size)
{
    const MS_U8 *p = (MS_U8 *)buf;

    crc = ~crc;

    while(size)
    {
        crc = (crc << 8) ^ crc32_lut[((crc >> 24) ^ *p) & 0xff];
        p++;
        size--;
    }

    return crc;
}

/*
    SetNextBitRev : Set bit in a byte sequence in reverse order inside the byte, update the bit poition to next bit, and update
    byte position if necessary
    cur_pos: pointer to the current position in the byte where the bit will be retrived, should be in range [0, 7]. Advance one in this function.
    cur_byte: pointer to the current byte position where the bit needs to be retrieved, will advance one byte if cur_pos is the last bit
*/
void SetNextBitRev(MS_U8 **cur_byte, MS_U32 *cur_pos, MS_U8 bit)
{
    (**cur_byte) = BitSet((**cur_byte), (1 << (*cur_pos)), bit);

    if ((*cur_pos) > 0)
    {
        *cur_pos = (*cur_pos) - 1;
    }
    else
    {
        *cur_pos = 7;
        (*cur_byte) = (*cur_byte) + 1;
    }

}

/* precomputed lookup table for xor-ed bits in one byte */
MS_U8 XorMatrix[] =
{
    0,  1,  1,  0,  1,  0,  0,  1,  1,  0,  0,  1,  0,  1,  1,  0,
    1,  0,  0,  1,  0,  1,  1,  0,  0,  1,  1,  0,  1,  0,  0,  1,
    1,  0,  0,  1,  0,  1,  1,  0,  0,  1,  1,  0,  1,  0,  0,  1,
    0,  1,  1,  0,  1,  0,  0,  1,  1,  0,  0,  1,  0,  1,  1,  0,
    1,  0,  0,  1,  0,  1,  1,  0,  0,  1,  1,  0,  1,  0,  0,  1,
    0,  1,  1,  0,  1,  0,  0,  1,  1,  0,  0,  1,  0,  1,  1,  0,
    0,  1,  1,  0,  1,  0,  0,  1,  1,  0,  0,  1,  0,  1,  1,  0,
    1,  0,  0,  1,  0,  1,  1,  0,  0,  1,  1,  0,  1,  0,  0,  1,
    1,  0,  0,  1,  0,  1,  1,  0,  0,  1,  1,  0,  1,  0,  0,  1,
    0,  1,  1,  0,  1,  0,  0,  1,  1,  0,  0,  1,  0,  1,  1,  0,
    0,  1,  1,  0,  1,  0,  0,  1,  1,  0,  0,  1,  0,  1,  1,  0,
    1,  0,  0,  1,  0,  1,  1,  0,  0,  1,  1,  0,  1,  0,  0,  1,
    0,  1,  1,  0,  1,  0,  0,  1,  1,  0,  0,  1,  0,  1,  1,  0,
    1,  0,  0,  1,  0,  1,  1,  0,  0,  1,  1,  0,  1,  0,  0,  1,
    1,  0,  0,  1,  0,  1,  1,  0,  0,  1,  1,  0,  1,  0,  0,  1,
    0,  1,  1,  0,  1,  0,  0,  1,  1,  0,  0,  1,  0,  1,  1,  0
};

MS_U8 GetByteField(MS_U8 meta_byte, MS_U8 field_mask)
{
    MS_U8 field_lsb_pos = 0;
    MS_U8 fm = field_mask;

    /* find where the lsb of field is */
    while (!(fm & 1))
    {
        fm >>= 1;
        field_lsb_pos++;
    }

    return ((meta_byte & field_mask) >> field_lsb_pos);
}

MS_U8 GetPacketType(EDRMetaDataPacket *packet)
{
    return GetByteField(packet->header0, FIELD_MASK_PACKET_TYPE);
}

MS_U16 GetMetadataLength(EDRMetaDataPacket *packet)
{
    return (packet->metadata.metadata_first.metadata_length_hi << 8)
           + packet->metadata.metadata_first.metadata_length_lo;
}


MS_U32 DescrambleMetaDataImplMStar(const MS_U8 * const buf_scrambled
                                   , EDRMetaDataPacket * const output_metadata_packet
                                   , MS_U8 u8FrameNumber
                                   , MS_U8 byte_per_word)
{
    MS_U32 idx = 0;                                /* buffer index for buf_scrambled */
    MS_U8 *p_buf_metadata = (MS_U8 *)output_metadata_packet;
    MS_U8 **cur_metadata_byte = &p_buf_metadata;
    MS_U32 cur_metadata_pos = 7;
    MS_U16 metadata_bytes = sizeof(EDRMetaDataPacket);
    MS_U32 len_meta = metadata_bytes * 8;  /* how many bits in metadata packet */
    MS_U8 metabit;
    MS_U8 xored_bit = 0;
    MS_U32 crc;
    MS_U32 i32 = 0, i;

    const MS_U8 * const buf_in_r = buf_scrambled;
    const MS_U8 * const buf_in_g = buf_scrambled + u8FrameNumber * byte_per_word;
    const MS_U8 * const buf_in_b = buf_scrambled + u8FrameNumber * byte_per_word * 2;

    memset(output_metadata_packet, 0, metadata_bytes);

    /* process pixel data in multiples of 4 bytes, or 1 bit of metadata at a time */
    while (len_meta)
    {
        while ((idx < byte_per_word) && (len_meta--))
        {
            i = i32 + idx;
            xored_bit  = XorMatrix[buf_in_r[i]];
            xored_bit ^= XorMatrix[buf_in_g[i]];
            xored_bit ^= XorMatrix[buf_in_b[i]];
            metabit = xored_bit;
            SetNextBitRev(cur_metadata_byte, &cur_metadata_pos, metabit);
            idx++;
        }

        i32 += (u8FrameNumber * byte_per_word * 3);
        idx = 0;
    }

    /* crc check */
    crc = crc32_dolby(0, output_metadata_packet, metadata_bytes);

    if (crc != 0)
        return 0xFFFFFFFF;

    return 0;
}

MS_U32 DescrambleMetaDataFrame(
    const MS_U8 *buf_in_start,  // start point of frame
    MS_U8 *buf_metadata,        // buffer of storing metadata
    MS_U16 *p_metadata_len,     // size of metadata
    MS_U8 u8FrameNumber,        // total frame
    MS_U8 byte_per_word)
{
    MS_U32 i;
    int offset;
    EDRMetaDataPacket output_packet;
    MS_U8 packet_type;
    unsigned short metadata_len, cur_metadata_len, remain_metadata_len;
    const MS_U8 *buf_in = buf_in_start;

    /* first packet */
    for (i=0; i< PACKET_REPEAT; ++i)
    {
        if (!DescrambleMetaDataImplMStar(buf_in_start, &output_packet,u8FrameNumber, byte_per_word))
            break;
        buf_in_start += interval;
    }
    if (PACKET_REPEAT == i)
    {
        if (_u32SWDesrbErrCount > 1)
        {
            printk("Error: packet crc error, can not receover\n");
        }
        return 0xFFFFFFFF;
    }

    /* get packet type and metadata length */
    packet_type = GetPacketType(&output_packet);
    switch (packet_type)
    {
        case PACKET_TYPE_SINGLE:
            metadata_len = cur_metadata_len = GetMetadataLength(&output_packet);
            if (metadata_len > 119)
            {
                printk("Error: metadata type is %d but metadata length is %d\n", PACKET_TYPE_SINGLE, metadata_len);
                return 0xFFFFFFFF;
            }
            remain_metadata_len = 0;
            break;
        case PACKET_TYPE_FIRST:
            metadata_len = GetMetadataLength(&output_packet);
            if (metadata_len <= 119)
            {
                printk("Error: metadata type is %d but metadata length is %d\n", PACKET_TYPE_FIRST, metadata_len);
                return 0xFFFFFFFF;
            }
            cur_metadata_len = 119;
            remain_metadata_len = metadata_len - 119;
            break;
        default:
            printk("Error: unexpected packet type for first packet %d\n", packet_type);
            return 0xFFFFFFFF;
    }

    /* copy metadata from packet */
    memcpy(buf_metadata,
           output_packet.metadata.metadata_first.metadata_body,
           cur_metadata_len
          );
    offset = cur_metadata_len;

    /* handle more packets */
    if (packet_type == PACKET_TYPE_FIRST)
    {
        buf_in_start = buf_in + interval * PACKET_REPEAT;
        do
        {
            for (i=0; i<PACKET_REPEAT; ++i)
            {
                if (!DescrambleMetaDataImplMStar(buf_in_start + i*interval, &output_packet, u8FrameNumber, byte_per_word))
                    break;
            }
            if (PACKET_REPEAT == i)
            {
                if (_u32SWDesrbErrCount > 1)
                {
                    printk("Error: subsequent packet crc error, can not receover\n");
                }
                return 0xFFFFFFFF;
            }
            buf_in_start += PACKET_REPEAT* interval;

            /* get packet type and metadata length */
            packet_type = GetPacketType(&output_packet);
            switch (packet_type)
            {
                case PACKET_TYPE_INTERM:
                    cur_metadata_len = 121;
                    remain_metadata_len -= 121;
                    break;
                case PACKET_TYPE_LAST:
                    if (remain_metadata_len > 121)
                    {
                        printk("Error: unexpected packet end\n");
                        return 0xFFFFFFFF;
                    }
                    cur_metadata_len = remain_metadata_len;
                    remain_metadata_len = 0;
                    break;
                default:
                    printk("Error: unexpected packet type for subsequent packet %d\n", packet_type);
                    return 0xFFFFFFFF;
            }

            /* copy metadata from packet */
            memcpy(buf_metadata + offset,
                   output_packet.metadata.metadata_following.metadata_body,
                   cur_metadata_len
                  );
            offset += cur_metadata_len;
        }
        while (packet_type != PACKET_TYPE_LAST);
    }

    *p_metadata_len = metadata_len;
    return 0;
}

MS_U32 DescrambleMetaData(void* pRawData, void* pDescrambledData, MS_U8 u8FrameNumber, MS_U8 byte_per_word)//EDRHDMIMetaDataConfig *config)
{
    MS_U8 *buf_in;
    MS_U8 *buf_in_start;
    MS_U8 *buf_metadata;
    unsigned short metadata_len;
    int frame_nr = 0;
    //MS_U64 fs;
    unsigned int k;
    int more_frames = 1;

    // 1 step:
    // get start point & size & frame number of scaler dram
    /* open input scrambled frame file, get its size, read into buffer */

#if 0
    /* sanity check */
    if (config->interval*config->repeat > config->input_frame_size)
    {
        printk("Error: input frame size too small to even hold first metadata packet\n");
        return -1;
    }
#endif

    // 2 step:
    // define a variable, pointer to start point of scaler frame buffer

    //buf_in = (MS_U8 *)malloc(input_frame_size * fn_per_read_frame);
    // read dram
#if 0
    {
        MS_U32 u32DNRBase0 = pXCResourcePrivate->stdrvXC_MVideo_Context.g_XC_InitData.u32Main_FB_Start_Addr;
        u32DNRBase0 = u32DNRBase0
                      + 1920         //IPM offset
                      * 22           // 22 lines per Dolby request
                      * 3            // byte per pixel
                      * fn_per_read_frame;   // frame
        MS_U32 *pu32Addr;
        MS_PHYADDR DstAddr;
        DstAddr = u32DNRBase0;
        pu32Addr = (MS_U32 *)MS_PA2KSEG1(DstAddr);
        buf_in = (MS_U8 *) pu32Addr;
        printk("Dolby [%s,%5d] buf_in address(%p)\n",__FUNCTION__,__LINE__,buf_in);
        printk("Dolby [%s,%5d] buf_in[0] = %x, buf_in[1] = %x\n",__FUNCTION__,__LINE__, *buf_in,*(buf_in+1));
    }
#else
    {
        buf_in = (MS_U8 *) pRawData;
        //printk("Dolby [%s,%5d] buf_in address(%p)\n",__FUNCTION__,__LINE__,buf_in);
        //printk("Dolby [%s,%5d] buf_in[0] = %x, buf_in[1] = %x\n",__FUNCTION__,__LINE__, *buf_in,*(buf_in+1));
    }
#endif
    // 3 step:
    // define a variable, pointer to descrambled metadata from scaler frame buffer.


    // 4 step:(Not sure if necessary)
    // define a variable, pointer to size of descrambled metadata from scaler frame buffer.
    /* open other output files */


    // 5 step:
    // allocate buffer to store one metadata
    buf_metadata = (MS_U8 *)kmalloc(sizeof(dm_metadata_t), GFP_KERNEL);


    // 6 step:
    // start to descramble datas from frame buffer, read one frame buffer one time.
    while (more_frames)
    {
        for (k = 0; k <= u8FrameNumber; k++)
        {

            // if reading all frames done, jump out.
            if (k == u8FrameNumber)
            {
                more_frames = 0;
                break;
            }

            /* Descamble meta data from pixel data */
            buf_in_start = buf_in + k* byte_per_word;    /* for mstar, k=0 for other format */

            if (DescrambleMetaDataFrame(/*config,*/ buf_in_start, buf_metadata, &metadata_len, u8FrameNumber, byte_per_word) == 0xFFFFFFFF)
            {
                if (buf_metadata)
                    kfree(buf_metadata);

                return 0xFFFFFFFF;
            }
            //printk("Dolby [%s,%5d] metadata_len(%d)\n",__FUNCTION__,__LINE__,metadata_len);

            frame_nr++;
        } /* end for k */
    }

    //printk("Descrambled %u frames\n", frame_nr);

    /* deallocate buffers */
    if (buf_metadata)
        kfree(buf_metadata);

    return 0;
}
#endif // DOLBY_VISION_SW_DESCRAMBLE_ENABLE

BOOL MHal_XC_SwDescrambleMetadata(MS_U8 *pu8Metadata, MS_U16 *pu16MetadataLength)
{
    // F2 field number
    MS_U8 u8FrameNumber = MHal_XC_R2BYTEMSK(REG_SC_BK42_19_L, 0x001F);
    // F2 MIU select
    MS_U8 u8MiuSel = MHal_XC_R2BYTEMSK(REG_SC_BK42_05_L, 0x0030) >> 4;
    MS_U8 u8IPMCurFrameNum = MHal_XC_R2BYTEMSK(REG_SC_BK42_3A_L, 0x000F);
    // F2 IP frame buffer base address
    MS_U32 u32AddrL = MHal_XC_R2BYTE(REG_SC_BK42_08_L);
    MS_U32 u32AddrH = MHal_XC_R2BYTE(REG_SC_BK42_09_L) & 0x01FF;
    MS_U32 u32Addr = (u32AddrL | (u32AddrH << 16)) * BYTE_PER_WORD;
    MS_U32 u32Offset = 0;
    MS_U8 *pRawData = NULL;
    MS_U8 *pu8RawBufStart = NULL;
    //MS_U8 *pu8MetadataBuf = NULL;
    MS_BOOL bUnmapDRAM = FALSE;
    MS_BOOL bRetVal = TRUE;
    if (MHal_XC_GetMiuOffset(u8MiuSel, &u32Offset) != TRUE)
    {
        return FALSE;
    }

    if (pfn_valid(__phys_to_pfn(u32Addr + u32Offset)))
    {
        pRawData = __va(u32Addr + u32Offset);
        bUnmapDRAM = FALSE;
    }
    else
    {
        pRawData = (MS_U8 __iomem *)ioremap(u32Addr + u32Offset, 3840 * 2160 * 3);
        bUnmapDRAM = TRUE;
    }

    // start point of current frame from HDR DMA buffer.
    pu8RawBufStart = pRawData + u8IPMCurFrameNum * BYTE_PER_WORD;

    if (DescrambleMetaDataFrame(pu8RawBufStart, pu8Metadata, pu16MetadataLength, u8FrameNumber, BYTE_PER_WORD) == 0xFFFFFFFF)
    {
        _u32SWDesrbErrCount++;
        if (_u32SWDesrbErrCount > 1)
        {
            printk("Sw descramble metadata fail.\n");
        }
        bRetVal = FALSE;
    }
    else
    {
        _u32SWDesrbErrCount = 0;
    }

    if (bUnmapDRAM)
    {
        iounmap(pRawData);
        pRawData = NULL;
    }

    return bRetVal;
}

BOOL MHal_XC_IsCRCPass(MS_U8 u8PkgIdx)
{
    MS_BOOL bCRCPass = FALSE;
    MS_U32 au32CrcReg[6] = {REG_SC_BK79_1A_L, REG_SC_BK79_1B_L, REG_SC_BK79_1C_L, REG_SC_BK79_1D_L, REG_SC_BK79_1E_L, REG_SC_BK79_1F_L};

    // crc valid
    MS_U32 u32CrcLow = MHal_XC_R2BYTE(au32CrcReg[2 * u8PkgIdx]);
    MS_U32 u32CrcHigh = MHal_XC_R2BYTE(au32CrcReg[2 * u8PkgIdx+1]);
    if ((u32CrcLow == 0) && (u32CrcHigh == 0))
    {
        bCRCPass = TRUE;
    }

    return bCRCPass;
}

MS_BOOL MHal_XC_VSIF_Dolby_Status(void)
{
    // step 1: check length
    //if( DOLBY_VSIF_LEN != MHal_XC_R2BYTEMSK(REG_HDMI_DUAL_0_31_L,0x00FF) )
    //{
        //return FALSE;
    //}

    // step 2: check PB6 - PB24
    //if( DOLBY_VSIF_PB_VALUE != MHal_XC_R2BYTEMSK(REG_HDMI_DUAL_0_34_L,0xFF00) )
    //{
        //return FALSE;
    //}
    if( DOLBY_VSIF_PB_VALUE != MHal_XC_R2BYTE(REG_HDMI_DUAL_0_35_L) )
    {
        return FALSE;
    }
    if( DOLBY_VSIF_PB_VALUE != MHal_XC_R2BYTE(REG_HDMI_DUAL_0_36_L) )
    {
        return FALSE;
    }
    if( DOLBY_VSIF_PB_VALUE != MHal_XC_R2BYTE(REG_HDMI_DUAL_0_37_L) )
    {
        return FALSE;
    }
    if( DOLBY_VSIF_PB_VALUE != MHal_XC_R2BYTE(REG_HDMI_DUAL_0_38_L) )
    {
        return FALSE;
    }
    if( DOLBY_VSIF_PB_VALUE != MHal_XC_R2BYTE(REG_HDMI_DUAL_0_39_L) )
    {
        return FALSE;
    }
    if( DOLBY_VSIF_PB_VALUE != MHal_XC_R2BYTE(REG_HDMI_DUAL_0_3A_L) )
    {
        return FALSE;
    }
    if( DOLBY_VSIF_PB_VALUE != MHal_XC_R2BYTE(REG_HDMI_DUAL_0_3B_L) )
    {
        return FALSE;
    }
    if( DOLBY_VSIF_PB_VALUE != MHal_XC_R2BYTE(REG_HDMI_DUAL_0_3C_L) )
    {
        return FALSE;
    }
    if( DOLBY_VSIF_PB_VALUE != MHal_XC_R2BYTE(REG_HDMI_DUAL_0_3D_L) )
    {
        return FALSE;
    }

    return TRUE;
}


BOOL MHal_XC_GetHdmiMetadata(MS_U8 *pu8Metadata, MS_U16 *pu16MetadataLength)
{
    MS_BOOL bReturn = FALSE;
    int i = 0, j = 0;
    MS_U8 *pu8Data = pu8Metadata;
    MS_U16 u16MetadataLength = 0;
    MS_U8 u8MetadataLengthLow = 0;
    MS_U8 u8MetadataLengthHigh = 0;
    MS_U8 u8PacketType = 0;
    MS_U8 u8MetadataType = 0;

#define DOLBY_FIRST_PACKET_REMAIN_MAX_LENGTH (XC_HDR_DOLBY_PACKET_LENGTH - XC_HDR_DOLBY_PACKET_HEADER - XC_HDR_DOLBY_PACKET_TAIL - XC_HDR_DOLBY_METADATA_LENGTH_BIT)
#define DOLBY_OTHER_PACKET_REMAIN_MAX_LENGTH (XC_HDR_DOLBY_PACKET_LENGTH - XC_HDR_DOLBY_PACKET_HEADER - XC_HDR_DOLBY_PACKET_TAIL)
#if (DOLBY_VISION_SW_DESCRAMBLE_ENABLE == 1)
    struct timespec startTs;
    struct timespec endTs;
    getnstimeofday(&startTs);
#endif


    // enable descrb read back
    MHal_XC_W2BYTEMSK(REG_SC_BK79_12_L, 1 << 11, BIT(11));
    for (i = 0; i < 3; i++)
    {
        if (MHal_XC_IsCRCPass(i))
        {
            MS_U16 u16Value = _HAL_XC_ReadMetadata2BYTE(i * XC_HDR_DOLBY_PACKET_LENGTH / 2);
            u8PacketType = ((u16Value & 0xFF) >> 6) & 0x3;
            u8MetadataType = ((u16Value & 0xFF) >> 4) & 0x3;

            u8MetadataLengthHigh = _HAL_XC_ReadMetadata2BYTE(1 + i * XC_HDR_DOLBY_PACKET_LENGTH / 2) >> 8;
            u16Value = _HAL_XC_ReadMetadata2BYTE(2 + i * XC_HDR_DOLBY_PACKET_LENGTH / 2);
            u8MetadataLengthLow = u16Value & 0xFF;
            u16MetadataLength = (u8MetadataLengthHigh << 8) | u8MetadataLengthLow;
#if (DOLBY_VISION_SW_DESCRAMBLE_ENABLE == 1)
            if (u16MetadataLength > (DOLBY_FIRST_PACKET_REMAIN_MAX_LENGTH + DOLBY_OTHER_PACKET_REMAIN_MAX_LENGTH))
            {
                // exceed SRAM max length, go to sw descramble.
                break;
            }
#endif
            *pu8Data = (u16Value >> 8);
            pu8Data++;

            for (j = (XC_HDR_DOLBY_PACKET_HEADER + XC_HDR_DOLBY_METADATA_LENGTH_BIT) / 2 + 1; j < XC_HDR_DOLBY_PACKET_LENGTH / 2; j++)
            {
                *((MS_U16 *)pu8Data) = _HAL_XC_ReadMetadata2BYTE(j + i * XC_HDR_DOLBY_PACKET_LENGTH / 2);
                pu8Data = ((MS_U16 *)pu8Data) + 1;
            }
            break;
        }
    }

    // all crcs is fail
    if (i >= 3)
    {
        //printk("Get hdmi metadata, crc check fail!\n");
        bReturn = FALSE;
        goto EXIT;
    }

    // metadata_type = 0b00: Dolby Vision metadata
    // metadata_type = 0b01~0b11: Reserved for future use (for example, Dolby AS3D)
    if (u8MetadataType != 0)
    {
        printk("Get hdmi metadata, metadata type is incorrect!\n");
        bReturn = FALSE;
        goto EXIT;
    }

    // packet_type = 0b00: A single packet carries an entire Dolby Vision metadata structure.
    // packet_type = 0b01: The first packet if multiple packets carry one Dolby Vision metadata structure.
    // packet_type = 0b10: Intermediate packets if multiple packets carry one Dolby Vision metadata structure.
    // packet_type = 0b11: The last packet if multiple packets carry one Dolby Vision metadata structure.
    if ((u8PacketType != 0x00) && (u8PacketType != 0x01))
    {
        printk("Get hdmi metadata, packet type is incorrect!\n");
        bReturn = FALSE;
        goto EXIT;
    }

    if (u16MetadataLength <= DOLBY_FIRST_PACKET_REMAIN_MAX_LENGTH)
    {
        bReturn = TRUE;
    }
    else if (u16MetadataLength > (DOLBY_FIRST_PACKET_REMAIN_MAX_LENGTH + DOLBY_OTHER_PACKET_REMAIN_MAX_LENGTH))
    {
#if (DOLBY_VISION_SW_DESCRAMBLE_ENABLE == 1)
        long long diff = 0;
        MS_U16 u16SWMetadataLen = 0;
        getnstimeofday(&endTs);
        diff = endTs.tv_nsec + ((endTs.tv_sec - startTs.tv_sec) * 1000000000 - startTs.tv_nsec);
        // HW suggest that SW descramble wait 1.6ms after descrambling done.
        while (diff < 1600000)
        {
            getnstimeofday(&endTs);
            diff = endTs.tv_nsec + ((endTs.tv_sec - startTs.tv_sec) * 1000000000 - startTs.tv_nsec);
        }
        u16SWMetadataLen = 0;
        if (MHal_XC_SwDescrambleMetadata(pu8Metadata, &u16SWMetadataLen) != TRUE)
        {
            bReturn = FALSE;
            goto EXIT;
        }

        if (u16SWMetadataLen != u16MetadataLength)
        {
            printk("Metadata length of HW/SW descramble is inconsistent.\n");
            bReturn = FALSE;
            goto EXIT;
        }
        else
        {
            bReturn = TRUE;
        }
#else
        printk("Get hdmi metadata, exceed max length, hw don't support!\n");
        bReturn = FALSE;
        goto EXIT;
#endif
    }
    else
    {
        *pu8Data = _HAL_XC_ReadMetadata2BYTE(1 + 3 * XC_HDR_DOLBY_PACKET_LENGTH / 2) & 0xFF;
        pu8Data++;

        for (j = XC_HDR_DOLBY_PACKET_HEADER / 2 + 1; j < (u16MetadataLength - DOLBY_FIRST_PACKET_REMAIN_MAX_LENGTH) / 2; j++)
        {
            *((MS_U16 *)pu8Data) = _HAL_XC_ReadMetadata2BYTE(j + 3 * XC_HDR_DOLBY_PACKET_LENGTH / 2);
            pu8Data = ((MS_U16 *)pu8Data) + 1;
        }
        bReturn = TRUE;
    }

    *pu16MetadataLength = u16MetadataLength;

EXIT:

    // disable descrb read back
    MHal_XC_W2BYTEMSK(REG_SC_BK79_12_L, 0, BIT(11));

    return bReturn;
}

BOOL MHal_XC_SetInputSourceType(EN_KDRV_XC_INPUT_SOURCE_TYPE enInputSourceType)
{
    _enInputSourceType = enInputSourceType;

    return TRUE;
}

BOOL MHal_XC_SetHDR_DMARequestOFF(MS_BOOL bEnable, MS_BOOL bImmediately)
{
    if (bEnable == TRUE)
    {
        if (bImmediately)
        {
            MHal_XC_W2BYTEMSK(REG_SC_BK42_50_L, BIT(2) | BIT(1), BIT(2) | BIT(1));
        }
        else
        {
            KApi_XC_MLoad_WriteCmd(E_CLIENT_MAIN_HDR, REG_SC_BK42_50_L, BIT(2) | BIT(1), BIT(2) | BIT(1));
        }
    }
    else
    {
        if (bImmediately)
        {
            MHal_XC_W2BYTEMSK(REG_SC_BK42_50_L, 0, BIT(2) | BIT(1));
        }
        else
        {
            KApi_XC_MLoad_WriteCmd(E_CLIENT_MAIN_HDR, REG_SC_BK42_50_L, 0, BIT(2) | BIT(1));
        }
    }

    return TRUE;
}

BOOL MHal_XC_SetDMPQBypass(MS_BOOL bEnable)
{
    // BIT(0) = 1 --> SW DM PQ bypass
    //        = 0 --> SW DM PQ go-through
    // BIT(1) = 1 --> SW DM PQ bypass ENABLE
    //        = 0 --> SW DM PQ bypass DISABLE
    if (bEnable == TRUE)
    {
        MHal_XC_W2BYTEMSK(REG_SC_BK79_03_L, BIT(0)|BIT(1), BIT(0)|BIT(1));
    }
    else
    {
        MHal_XC_W2BYTEMSK(REG_SC_BK79_03_L, 0, BIT(0)|BIT(1));
    }

    return TRUE;
}

void MHal_XC_SetDMAPath(EN_KDRV_XC_HDR_DMA_PATH enDMAPath)
{
    if (enDMAPath == EN_KDRV_XC_HDR_DMA_BYPASS)
    {
        // dma path select
        MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(6));
        // dma path user mode
        MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(7));
    }
    else if (enDMAPath == EN_KDRV_XC_HDR_DMA_ENABLE)
    {
        // dma path select
        MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 1 << 6, BIT(6));
        // dma path user mode
        MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 1 << 7, BIT(7));
    }
    else
    {
        // impossible, should not go in here
        printk(" impossible setting, please check function variable = %d\n",enDMAPath);
    }
}

BOOL MHal_XC_SetHDRType(EN_KDRV_XC_HDR_TYPE enHDRType)
{
    if (_enHDRType == enHDRType)
    {
        return TRUE;
    }

    _enHDRType = enHDRType;

    if (IS_HDMI_DOLBY)
    {
        MHal_XC_UpdatePath(E_KDRV_XC_HDR_PATH_DOLBY_HDMI);
        MHal_XC_DisableHDMI422To444();
        MHal_XC_EnableAutoSeamless(TRUE);
        if (MHal_XC_SupportDolbyHDR() && IS_DOLBY_HDR(MAIN_WINDOW))
        {
            DoVi_Set_HDRInputType(_pstDmConfig, DOVI_DM_INPUT_TYPE_HDMI);
        }
    }
    else if (IS_OTT_DOLBY)
    {
        MHal_XC_UpdatePath(E_KDRV_XC_HDR_PATH_DOLBY_OTT_DUAL);
        MHal_XC_EnableAutoSeamless(FALSE);
        if (MHal_XC_SupportDolbyHDR() && IS_DOLBY_HDR(MAIN_WINDOW))
        {
            DoVi_Set_HDRInputType(_pstDmConfig, DOVI_DM_INPUT_TYPE_OTT);
        }
    }
    else if (IS_HDMI_OPEN)
    {
        MHal_XC_UpdatePath(E_KDRV_XC_HDR_PATH_OPEN_HDMI);
        MHal_XC_EnableAutoSeamless(FALSE);
    }
    else if (IS_OTT_OPEN)
    {
        MHal_XC_UpdatePath(E_KDRV_XC_HDR_PATH_OPEN_OTT);
        MHal_XC_EnableAutoSeamless(FALSE);
    }

    return TRUE;
}


BOOL MHal_XC_UpdatePath(EN_KDRV_XC_HDR_PATH enPath)
{
    switch (enPath)
    {
        case E_KDRV_XC_HDR_PATH_DOLBY_HDMI:
        {
            // hdmi422 to 14b for dm: 00: msb  01: lsb  11: lsb/C-signed
            MHal_XC_W2BYTEMSK(REG_SC_BK79_06_L, 0, 0x0300);
            // hdr to ip2 focre ack
            MHal_XC_W2BYTEMSK(REG_SC_BK79_06_L, 0, BIT(15));
            // dc path enable
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(0));
            // ip2 path enable
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 1 << 1, BIT(1));
            // ipsrc_out_pac
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(4));
            // ipsrc to dm data pac user mode
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 1 << 5, BIT(5));
            MHal_XC_SetDMAPath(EN_KDRV_XC_HDR_DMA_ENABLE);
            // EL / BL off
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 1 << 10, BIT(10));
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 1 << 11, BIT(11));
            // open hdr enable
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(15));
            // PQ bypass control
            MHal_XC_W2BYTEMSK(REG_SC_BK7A_01_L, 0, BIT(0));
            MHal_XC_W2BYTEMSK(REG_SC_BK7A_01_L, 0x8000, 0xF000);
            if (MHal_XC_SupportDolbyHDR() == TRUE)
            {
                MHal_XC_W2BYTEMSK(REG_SC_BK7A_02_L, 0x3, 0x007F);
            }
            break;
        }
        case E_KDRV_XC_HDR_PATH_DOLBY_OTT_SINGLE:
        {
            // hdmi422 to 14b for dm: 00: msb  01: lsb  11: lsb/C-signed
            MHal_XC_W2BYTEMSK(REG_SC_BK79_06_L, 0, 0x0300);
            // hdr to ip2 focre ack
            MHal_XC_W2BYTEMSK(REG_SC_BK79_06_L, 1 << 15, BIT(15));
            // dc path enable
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 1, BIT(0));
            // ip2 path enable
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 1 << 1, BIT(1));
            // ipsrc_out_pac
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(4));
            // ipsrc to dm data pac user mode
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 1 << 5, BIT(5));
            MHal_XC_SetDMAPath(EN_KDRV_XC_HDR_DMA_BYPASS);
            // EL / BL off
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 1 << 10, BIT(10));
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(11));
            // open hdr enable
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(15));
            // PQ bypass control
            MHal_XC_W2BYTEMSK(REG_SC_BK7A_01_L, 0, BIT(0));
            MHal_XC_W2BYTEMSK(REG_SC_BK7A_01_L, 0xB000, 0xF000);
            if (MHal_XC_SupportDolbyHDR() == TRUE)
            {
                MHal_XC_W2BYTEMSK(REG_SC_BK7A_02_L, 0x3, 0x007F);
            }
            break;
        }
        case E_KDRV_XC_HDR_PATH_DOLBY_OTT_DUAL:
        {
            // hdmi422 to 14b for dm: 00: msb  01: lsb  11: lsb/C-signed
            MHal_XC_W2BYTEMSK(REG_SC_BK79_06_L, 0, 0x0300);
            // hdr to ip2 focre ack
            MHal_XC_W2BYTEMSK(REG_SC_BK79_06_L, 1 << 15, BIT(15));
            // dc path enable
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 1, BIT(0));
            // ip2 path enable
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 1 << 1, BIT(1));
            // ipsrc_out_pac
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(4));
            // ipsrc to dm data pac user mode
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 1 << 5, BIT(5));
            MHal_XC_SetDMAPath(EN_KDRV_XC_HDR_DMA_BYPASS);
            // EL / BL off
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(10));
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(11));
            // open hdr enable
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(15));
            // PQ bypass control
            MHal_XC_W2BYTEMSK(REG_SC_BK7A_01_L, 0, BIT(0));
            MHal_XC_W2BYTEMSK(REG_SC_BK7A_01_L, 0xB000, 0xF000);
            if (MHal_XC_SupportDolbyHDR() == TRUE)
            {
                MHal_XC_W2BYTEMSK(REG_SC_BK7A_02_L, 0x3, 0x007F);
            }
            break;
        }
        case E_KDRV_XC_HDR_PATH_OPEN_HDMI:
        case E_KDRV_XC_HDR_PATH_OPEN_OTT:
        {
            // patch here, after CFD finishes we have to remove all these registers
            {
                //MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(5));
                //MHal_XC_W2BYTEMSK(REG_SC_BK7A_01_L, 0, BIT(0));
                if (MHal_XC_SupportDolbyHDR() == TRUE)
                {
                    //MHal_XC_W2BYTEMSK(REG_SC_BK7A_02_L, 0x18, 0x007F);
                    if (MHal_XC_HDMI_Color_Data_Format(_stCfdHdmi[0].u8PixelFormat) == E_CFD_MC_FORMAT_YUV422)
                    {
                        //MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(4));
                        //MHal_XC_W2BYTEMSK(REG_SC_BK7A_01_L, 0x8000, 0xF000);
                    }
                    else
                    {
                        //MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 1 << 4, BIT(4));
                        //MHal_XC_W2BYTEMSK(REG_SC_BK7A_01_L, 0x0, 0xF000);
                    }
                }
            }
            break;
        }
        case E_KDRV_XC_HDR_PATH_ORIGIN:
        {
            // hdr to ip2 focre ack
            MHal_XC_W2BYTEMSK(REG_SC_BK79_06_L, 1 << 15, BIT(15));
            // dc path enable
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(0));
            // ip2 path enable
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(1));
            MHal_XC_SetDMAPath(EN_KDRV_XC_HDR_DMA_BYPASS);
            // disable FRC mode
            MHal_XC_W2BYTEMSK(REG_SC_BK79_03_L, 0, BIT(14));
            // hdr dma seamless with rfbl mode
            MHal_XC_W2BYTEMSK(REG_SC_BK79_03_L, 0, BIT(15));
            // hdr auto seamless md
            MHal_XC_W2BYTEMSK(REG_SC_BK79_04_L, 0, BIT(15));
            break;
        }
        case E_KDRV_XC_HDR_PATH_NON_HDR_HDMI:
        {
            // hdr to ip2 focre ack
            MHal_XC_W2BYTEMSK(REG_SC_BK79_06_L, 1 << 15, BIT(15));
            // dc path enable
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(0));
            // disable FRC mode
            MHal_XC_W2BYTEMSK(REG_SC_BK79_03_L, 0, BIT(14));
            // hdr dma seamless with rfbl mode
            MHal_XC_W2BYTEMSK(REG_SC_BK79_03_L, 0, BIT(15));
            // hdr auto seamless md
            MHal_XC_W2BYTEMSK(REG_SC_BK79_04_L, 0, BIT(15));
            break;
        }
        default:
        {
            break;
        }
    }

    return TRUE;
}

BOOL MHal_XC_EnableEL(MS_BOOL bEnable)
{
    MS_U16 u16Value = (bEnable == TRUE) ? 0 : 1;
    MS_U16 u16CurVal = MHal_XC_R2BYTEMSK(REG_SC_BK79_07_L, BIT(10)) >> 10;
    if (u16CurVal != u16Value)
    {
        MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, u16Value << 10, BIT(10));
    }

    return TRUE;
}

BOOL MHal_XC_EnableAutoSeamless(MS_BOOL bEnable)
{
    // hdr auto seamless md
    if (bEnable)
    {
        MHal_XC_W2BYTEMSK(REG_SC_BK79_04_L, 1 << 15, BIT(15));
    }
    else
    {
        MHal_XC_W2BYTEMSK(REG_SC_BK79_04_L, 0, BIT(15));
    }

    return TRUE;
}

BOOL MHal_XC_DisableHDMI422To444()
{
    MHal_XC_W2BYTEMSK(REG_SC_BK01_61_L, 1, BIT(0));
    MHal_XC_W2BYTEMSK(REG_SC_BK01_61_L, 0, BIT(1));
    MHal_XC_W2BYTEMSK(REG_SC_BK01_61_L, 0, BIT(2));

    return TRUE;
}

BOOL MHal_XC_SetHDRWindow(MS_U16 u16Width, MS_U16 u16Height)
{
    // this control is now moved to utopia, which should be
    // set with DS
    //MHal_XC_W2BYTEMSK(REG_SC_BK79_08_L, u16Width, 0x1FFF);
    //MHal_XC_W2BYTEMSK(REG_SC_BK79_09_L, u16Height, 0x0FFF);
    return TRUE;
}

BOOL MHal_XC_Set3DLutInfo(MS_U8* pu8Data, MS_U32 u32Size)
{
    int iResult = DoVi_Replace_3DLut(&(_pstDmRegTable->st3D), pu8Data, u32Size);
    if (iResult < 0)
    {
        printk("3dLut has problems!\n");
        return FALSE;
    }

    return TRUE;
}

BOOL MHal_XC_UpdateDolbyPQSetting(MS_U32 enType, void* pParam)
{
    switch (enType)
    {
        case EN_DOLBY_PQ_BACKLIGHT:
        {
            MS_U16* pu16Backlight = (MS_U16*) pParam;
            DoVi_SetDisplayProperty( _pstDoVi_TargetDisplayConfig,  *pu16Backlight);
            DoVi_DumpPQSettings(_pstDoVi_TargetDisplayConfig);

            if (1 == _pstDoVi_PQ_control->bDovi_UsePQen)
            {
                DoVi_SetPQ2Configs( _pstDmConfig, _pstDoVi_TargetDisplayConfig);
            }

            break;
        }

        default:
            // do nothing
            break;
    }

    DoVi_DumpDmConfigs(_pstDmConfig);

    return TRUE;
}

BOOL MHal_XC_SetDolbyMetaData(MS_U8* pu8Data, MS_U32 u32Size)
{
    DoVi_MdsExt_t stMdsExt;
    int iResult = 0;

    memset((MS_U8*)&stMdsExt, 0, sizeof(DoVi_MdsExt_t));
    // Read metadata of this frame
    iResult = DoVi_DmReadMetadata(&stMdsExt, pu8Data, u32Size, _pstDmConfig);
    // Handle error if metadata is corrupted
    if (iResult < 0)
    {
        printk("Metadata has problems! iResult: %d\n", iResult);
        return FALSE;
    }
    else
    {
        //DoVi_DmDumpMetadata(&stMdsExt);
        // Calculate Registers and LUT contents during frame DE
        DoVi_DmFrameDeCalculate(_pstDmRegTable, _pstDmConfig, &stMdsExt, _pstCompConfExt);
        // Update Registers and LUT contents at blanking
        DoVi_DmBlankingUpdate(_pstDmRegTable);
    }
    return TRUE;
}

BOOL MHal_XC_SetDolbyCompData(MS_U8* pu8Data, MS_U32 u32Size)
{
    int iResult = DoVi_CompReadMetadata(_pstCompConfExt, pu8Data, u32Size);
    if (iResult < 0)
    {
        printk("Composer has problems!\n");
        return FALSE;
    }
    else
    {
        //DoVi_CompDumpConfig(_pstCompConfExt);
        //set compser into color-fomat-ctrl hw
        DoVi_CompFrameDeCalculate(_pstCompRegTable, _pstCompConfExt);
        DoVi_CompBlankingUpdate(_pstCompRegTable);
    }

    return TRUE;
}

MS_U32 MHal_XC_GetRegsetCnt()
{
    K_XC_DS_CMDCNT stXCDSCmdCnt;

    DoVi_GetCmdCnt(&stXCDSCmdCnt);

    return stXCDSCmdCnt.u16CMDCNT_IPM;
}

void MHal_XC_PrepareDolbyInfo()
{
    DoVi_Prepare_DS(DOLBY_DS_VERSION);
}

BOOL MHal_XC_EnableHDR(MS_BOOL bEnableHDR)
{
    _bEnableHDR = bEnableHDR;
    if (!bEnableHDR)
    {
        // hdr to ip2 focre ack
        MHal_XC_W2BYTEMSK(REG_SC_BK79_06_L, 1 << 15, BIT(15));
        // dc path enable
        MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(0));
        // ip2 path enable
        MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(1));
        // disable FRC mode
        MHal_XC_W2BYTEMSK(REG_SC_BK79_03_L, 0, BIT(14));
        // hdr dma seamless with rfbl mode
        MHal_XC_W2BYTEMSK(REG_SC_BK79_03_L, 0, BIT(15));
        // hdr auto seamless md
        MHal_XC_W2BYTEMSK(REG_SC_BK79_04_L, 0, BIT(15));

        MHal_XC_W2BYTE(REG_SC_BK79_7C_L, 0x0000);
        MHal_XC_W2BYTE(REG_SC_BK79_7D_L, 0x0000);

        // hdr clk setting enable
        //MHal_XC_W2BYTE(REG_SC_BK79_02_L, 0xFFFF);
        //MHal_XC_W2BYTEMSK(REG_SC_BK79_7E_L, 0, BIT(1)|BIT(0));

        _enHDRType = E_KDRV_XC_HDR_NONE;
        _enInputSourceType = E_KDRV_XC_INPUT_SOURCE_NONE;
    }
    else
    {
        // hdr clk setting disable
        //MHal_XC_W2BYTE(REG_SC_BK79_02_L, 0);
        //MHal_XC_W2BYTEMSK(REG_SC_BK79_7E_L, BIT(1)|BIT(0), BIT(1)|BIT(0));
        DoVi_Trigger3DLutUpdate();

        MHal_XC_W2BYTE(REG_SC_BK79_7C_L, 0xFFFF);
        MHal_XC_W2BYTE(REG_SC_BK79_7D_L, 0xFFFF);
    }

    return TRUE;
}

BOOL MHal_XC_MuteHDR()
{
    MHal_XC_W2BYTEMSK(REG_SC_BK7A_02_L, BIT(4), BIT(4));
    MHal_XC_W2BYTE(REG_SC_BK7A_42_L, 0);
    MHal_XC_W2BYTE(REG_SC_BK7A_54_L, 0);
    MHal_XC_W2BYTE(REG_SC_BK7A_55_L, 0);
    MHal_XC_W2BYTE(REG_SC_BK7A_59_L, 0x0800);

    return TRUE;
}

BOOL MHal_XC_SupportDolbyHDR()
{
    BOOL bSupportDolbyHdr = FALSE;

    if (!(MHal_XC_DolbySWBonded() || MHal_XC_DolbyHWBonded()))
    {
        bSupportDolbyHdr = TRUE;
    }
    else
    {
        bSupportDolbyHdr = FALSE;
    }

    return bSupportDolbyHdr;
}

BOOL MHal_XC_DolbySWBonded()
{
    MS_U16 u16OldVal = 0;
    MS_U16 u16Val = 0;
    struct timespec startTs;
    struct timespec endTs;
    if (_u16DolbySWBondStatus != -1)
        return _u16DolbySWBondStatus;

    u16OldVal = MHal_XC_R2BYTE(REG_BK20_28_L);
    MHal_XC_W2BYTEMSK(REG_BK20_28_L, 0x6f<<2, 0x1FFF);
    MHal_XC_W2BYTEMSK(REG_BK20_28_L, 0, BIT(15));
    MHal_XC_W2BYTEMSK(REG_BK20_28_L, BIT(13), BIT(13));
    u16Val = MHal_XC_R2BYTEMSK(REG_BK20_28_L, BIT(13));

    getnstimeofday(&startTs);
    while (u16Val & BIT(13))
    {
        long long diff = 0;
        getnstimeofday(&endTs);
        diff = endTs.tv_nsec + ((endTs.tv_sec - startTs.tv_sec) * 1000000000 - startTs.tv_nsec);
        if (diff > 100000000) // timeout is 100ms
        {
            printk("[XC][%s:%05d]read support dolby timeout!!!\n", __FUNCTION__, __LINE__);
            break;
        }
        u16Val = MHal_XC_R2BYTEMSK(REG_BK20_28_L, BIT(13));
    }
    _u16DolbySWBondStatus = MHal_XC_R2BYTE(REG_BK20_2C_L);
    MHal_XC_W2BYTE(REG_BK20_28_L, u16OldVal);

    if(_u16DolbySWBondStatus & BIT(0))
    {
        _u16DolbySWBondStatus = FALSE;
    }
    else
    {
        _u16DolbySWBondStatus = (_u16DolbySWBondStatus & BIT(4)) > 0? TRUE : FALSE;
    }

    return _u16DolbySWBondStatus;
}

BOOL MHal_XC_DolbyHWBonded()
{
    if (_u16DolbyHWBondStatus != -1)
        return _u16DolbyHWBondStatus;

    if (MHal_XC_R2BYTEMSK(REG_SC_BK79_3F_L, BIT(14)))  //BIT (14) = 1: HW bond or not support Dolby
    {
        _u16DolbyHWBondStatus = TRUE;
    }
    else    //BIT (14) = 0: HW support Dolby
    {
        _u16DolbyHWBondStatus = FALSE;
    }

    return _u16DolbyHWBondStatus;
}

BOOL MHal_XC_InitHDR()
{
    // descrb byte reverse
    MHal_XC_W2BYTEMSK(REG_SC_BK79_10_L, 1 << 5, BIT(5));
    // descrb metadata length, 0x180
    MHal_XC_W2BYTEMSK(REG_SC_BK79_11_L, 0x180, 0x1FFF);

    // metadata num
    MHal_XC_W2BYTEMSK(REG_SC_BK79_18_L, 0x3, 0x0003);
    // crc byte reverse
    MHal_XC_W2BYTEMSK(REG_SC_BK79_18_L, 1 << 5, BIT(5));
    // metadata package length, 0x80
    MHal_XC_W2BYTEMSK(REG_SC_BK79_19_L, 0x80, 0x1FFF);
    // force crc error cnt off
    MHal_XC_W2BYTEMSK(REG_SC_BK79_04_L, 0x6 << 8, 0x1F00);

    // enable lut use auto_dl
    MHal_XC_W2BYTEMSK(REG_SC_BK7A_70_L, 1 << 15, BIT(15));
    // clear error flag
    MHal_XC_W2BYTEMSK(REG_SC_BK7A_70_L, 0, BIT(9));
    // auto_dl protect
    MHal_XC_W2BYTEMSK(REG_SC_BK7A_70_L, 0x0, 0x000F);
    // auto_dl trigger mode0 delay
    // 0x600 is a experimental value
    // this value must set after DS trigger point, for
    // ADL to trigger at the right frame since ADL trigger is controlled by DS
    MHal_XC_W2BYTEMSK(REG_SC_BK7A_71_L, 0x600, 0x7FFF);
    // auto_dl trigger mode
    MHal_XC_W2BYTEMSK(REG_SC_BK7A_71_L, 0, BIT(15));
    // auto_dl clk select mask
    MHal_XC_W2BYTEMSK(REG_SC_BK7A_72_L, 0, 0x0003);
    // auto_dl trigger vsync select
    MHal_XC_W2BYTEMSK(REG_SC_BK7A_72_L, 1 << 15, BIT(15));

    // HDMI HDR auto seamless mode switch HW decision
    //  EDR and CRC
    //  0 : logic = OR  --> EDR || CRC
    //  1 : logic = AND --> EDR && CRC
    //  HW suggest default value 1
    MHal_XC_W2BYTEMSK(REG_SC_BK79_04_L, BIT(4), BIT(4));

    // disable FRC mode
    MHal_XC_W2BYTEMSK(REG_SC_BK79_03_L, 0, BIT(14));
    // F2 field number
    MHal_XC_W2BYTEMSK(REG_SC_BK42_19_L, 0x2, 0x001F);
    MHal_XC_W2BYTEMSK(REG_SC_BK42_07_L, 0x2000, 0xE000);
    // hdr dma miu request off
    MHal_XC_W2BYTEMSK(REG_SC_BK42_50_L, BIT(2) | BIT(1), BIT(2) | BIT(1));
    // hdr clk setting disable
    MHal_XC_W2BYTE(REG_SC_BK79_02_L, 0xFFFF);
    MHal_XC_W2BYTEMSK(REG_SC_BK79_7E_L, 0, BIT(1)|BIT(0));
    // enable irq
    MHal_XC_W2BYTEMSK(REG_SC_BK00_13_L, 0x0, 0x0060);
    MHal_XC_W2BYTEMSK(REG_SC_BK00_15_L, 0x0, 0x0060);
    _bEnableHDRCLK = FALSE;
    _enHDRType = E_KDRV_XC_HDR_NONE;
    _bEnableHDR = FALSE;

    // init color format hw
    if (_pstDoVi_PQ_control == NULL)
    {
        _pstDoVi_PQ_control = DoVi_PQSettingControlAllocConfig(); // configuration file
        _pstDoVi_PQ_control->bDovi_UsePQen = 1;
    }

    if (_pstDoVi_TargetDisplayConfig == NULL)
    {
        _pstDoVi_TargetDisplayConfig  = DoVi_PQSettingAllocConfig(); // configuration file
    }

    if (_pstDmRegTable == NULL)
    {
        _pstDmRegTable = MsHdr_DmAllocRegTable(); // register table
    }
    if (_pstDmConfig == NULL)
    {
        _pstDmConfig = DoVi_DmAllocConfig(); // configuration file
    }
    if (_pstDmConfig != NULL)
    {
        DoVi_DmSetDefaultConfig(_pstDmConfig);
    }
    if (_pstCompRegTable == NULL)
    {
        _pstCompRegTable = MsHdr_CompAllocRegTable(); // register table
    }
    if (_pstCompConfExt == NULL)
    {
        _pstCompConfExt = DoVi_CompAllocConfig(); // configuration file
    }
    if (_pstCompConfExt != NULL)
    {
        DoVi_CompSetDefaultConfig(_pstCompConfExt);
    }

    return TRUE;
}

BOOL MHal_XC_ExitHDR()
{
    if ((pu8AutoDownloadDRAMBaseAddr[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR] != NULL) && (_bUnmapAutoDownloadDRAM[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR] == TRUE))
    {
        iounmap(pu8AutoDownloadDRAMBaseAddr[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR]);
        pu8AutoDownloadDRAMBaseAddr[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR] = NULL;
    }

    // Release resource
    if (_pstDmRegTable != NULL)
    {
        MsHdr_DmFreeRegTable(_pstDmRegTable);
    }
    if (_pstDmConfig != NULL)
    {
        DoVi_DmFreeConfig(_pstDmConfig);
    }
    if (_pstCompRegTable != NULL)
    {
        MsHdr_CompFreeRegTable(_pstCompRegTable);
    }
    if (_pstCompConfExt != NULL)
    {
        DoVi_CompFreeConfig(_pstCompConfExt);
    }
    if (_pstDoVi_TargetDisplayConfig != NULL)
    {
        DoVi_PQSettingFreeConfig(_pstDoVi_TargetDisplayConfig);
    }
    if (_pstDoVi_PQ_control != NULL)
    {
        DoVi_PQSettingControlFreeConfig(_pstDoVi_PQ_control);
    }

    return TRUE;
}

static BOOL _Hal_XC_Set_Auto_Download_WorkMode(MS_U32 u32RegAddr, MS_U8 u8Bit, EN_KDRV_XC_AUTODOWNLOAD_MODE enMode)
{
    switch (enMode)
    {
        case E_KDRV_XC_AUTODOWNLOAD_TRIGGER_MODE:
        {
            MHal_XC_W2BYTEMSK(u32RegAddr, 0, BIT(u8Bit));
            break;
        }
        case E_KDRV_XC_AUTODOWNLOAD_ENABLE_MODE:
        {
            MHal_XC_W2BYTEMSK(u32RegAddr, 1 << u8Bit, BIT(u8Bit));
            break;
        }
        default:
            return FALSE;
    }

    return TRUE;
}

BOOL MHal_XC_ConfigAutoDownload(EN_KDRV_XC_AUTODOWNLOAD_CLIENT enClient, MS_BOOL bEnable,
                                EN_KDRV_XC_AUTODOWNLOAD_MODE enMode, MS_PHY phyBaseAddr, MS_U32 u32Size, MS_U32 u32MiuNo)
{
    MS_BOOL bCurrEnable[E_KDRV_XC_AUTODOWNLOAD_CLIENT_MAX] = {0};
    MS_BOOL bNextEnable[E_KDRV_XC_AUTODOWNLOAD_CLIENT_MAX] = {0};
    MS_U32 u32Offset = 0;
    mutex_lock(&_adl_mutex);

    bCurrEnable[enClient] = _stClientInfo[enClient].bEnable;

    switch(enClient)
    {
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR:
        {
            bNextEnable[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR] = bEnable;
            break;
        }
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC:
        {
            bNextEnable[E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC] = bEnable;
            break;
        }
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_OP2GAMMA:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_FRCOP2GAMMA:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_ODTABLE1:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_ODTABLE2:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_DEMURA:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_OP2LUT:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_T3D_0:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_T3D_1:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_FRCSPTPOPM:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_FOOPM:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_MAX:
        default:
            return FALSE;
    }

    if (MHal_XC_GetMiuOffset(u32MiuNo, &u32Offset) != TRUE)
    {
        mutex_unlock(&_adl_mutex);
        printk("Invalid miuno.\n");
        return FALSE;
    }

    if ((bNextEnable[enClient] == TRUE) && (bCurrEnable[enClient] != bNextEnable[enClient]))
    {
        _stClientInfo[enClient].phyBaseAddr = phyBaseAddr;
        _stClientInfo[enClient].u32Size = u32Size;
        _stClientInfo[enClient].bEnable = bNextEnable[enClient];
        _stClientInfo[enClient].u32MiuNo = u32MiuNo;

        // reset memory
        if (pfn_valid(__phys_to_pfn(phyBaseAddr + u32Offset)))
        {
            pu8AutoDownloadDRAMBaseAddr[enClient] = __va(phyBaseAddr + u32Offset);
            _bUnmapAutoDownloadDRAM[enClient] = FALSE;
        }
        else
        {
            pu8AutoDownloadDRAMBaseAddr[enClient] = (MS_U8 __iomem *)ioremap(phyBaseAddr + u32Offset, u32Size);
            _bUnmapAutoDownloadDRAM[enClient] = TRUE;
        }

        if (pu8AutoDownloadDRAMBaseAddr[enClient] != NULL)
        {
            memset(pu8AutoDownloadDRAMBaseAddr[enClient], 0, u32Size);
        }
        else
        {
            mutex_unlock(&_adl_mutex);
            printk("%s: %d, auto downlaod mmap pa fail.\n", __FUNCTION__, __LINE__);
            return FALSE;
        }
    }
    else if (bNextEnable[enClient] == FALSE)
    {
        mutex_unlock(&_adl_mutex);
        _stClientInfo[enClient].bEnable = bNextEnable[enClient];
        _enAutoDownloadStatus[enClient] = E_KDRV_XC_AUTO_DOWNLOAD_NONE;
        return TRUE;
    }

    switch(enClient)
    {
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR:
        {
            _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].u32StartAddr = 0xFFFFFFFF;
            _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].u32Depth = 0;
            _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].enMode = enMode;

            //set baseaddr
            MHal_XC_W2BYTE(REG_SC_BK67_29_L, (phyBaseAddr / BYTE_PER_WORD) & 0x0000FFFF);
            MHal_XC_W2BYTEMSK(REG_SC_BK67_2A_L, ((phyBaseAddr / BYTE_PER_WORD) >> 16 & 0x0000FFFF), 0x07FF);

            if (u32MiuNo)
            {
                MHal_XC_W2BYTEMSK(REG_SC_BK7F_10_L, 0x1000, 0x1000);
            }
            //set work mode
            _Hal_XC_Set_Auto_Download_WorkMode(REG_SC_BK67_28_L, 1, enMode);
            break;
        }
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC:
        {
            _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC].u32StartAddr = 0xFFFFFFFF;
            _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC].u32Depth = 0;
            _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC].enMode = enMode;

            //set baseaddr
            MHal_XC_W2BYTE(REG_SC_BK67_16_L, (phyBaseAddr / BYTE_PER_WORD) & 0x0000FFFF);
            MHal_XC_W2BYTEMSK(REG_SC_BK67_17_L, ((phyBaseAddr / BYTE_PER_WORD) >> 16 & 0x0000FFFF), 0x07FF);

            //set work mode
            _Hal_XC_Set_Auto_Download_WorkMode(REG_SC_BK67_11_L, 2, enMode);
            break;
        }
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_OP2GAMMA:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_FRCOP2GAMMA:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_ODTABLE1:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_ODTABLE2:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_DEMURA:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_OP2LUT:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_T3D_0:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_T3D_1:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_FRCSPTPOPM:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_FOOPM:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_MAX:
        default:
        {
            mutex_unlock(&_adl_mutex);
            return FALSE;
        }

    }

    _enAutoDownloadStatus[enClient] = E_KDRV_XC_AUTO_DOWNLOAD_CONFIGURED;
    mutex_unlock(&_adl_mutex);

    return TRUE;
}

static BOOL _Hal_XC_Auto_Download_Format_Hdr_Data(EN_KDRV_XC_AUTODOWNLOAD_SUB_CLIENT enSubClient,
        MS_U8 *pu8AutodownloadDRAMAddr, MS_U8* pu8Data, MS_U32 u32Size, MS_U16 u16StartAddr, MS_U32 *pu32Depth)
{
    MS_U32 u32Depth = *pu32Depth;
    MS_U8* pVirtBaseAddr = NULL;
    MS_U8 *pu8BaseAddr = NULL;
    unsigned int i = 0;
    MS_U32 u32Index = u16StartAddr;
    MS_U32 u32WriteNum = 0;
    MS_U32 u32DataSize = 0;

    if(pu8AutodownloadDRAMAddr == NULL)
    {
        printk("%s: %d pu8AutoDownloadDRAMBaseAddr is NULL.\n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    pVirtBaseAddr = pu8AutodownloadDRAMAddr;
    pu8BaseAddr = (MS_U8 *)pVirtBaseAddr;
    pu8BaseAddr += BYTE_PER_WORD * u16StartAddr;

    u32DataSize = 0;
    switch (enSubClient)
    {
        case E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_TMO:
        case E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_GAMMA:
        case E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_3DLUT:
        {
            u32DataSize = u32Size / sizeof(MS_U16);
            break;
        }
        case E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_DEGAMMA:
        {
            u32DataSize = u32Size / sizeof(MS_U32);
            break;
        }
        default:
        {
            printk("Vaild sub client.\n");
            return FALSE;
        }
    }

    //format&write datas into DRAM
    for (i = 0; i < u32DataSize; )
    {
        switch (enSubClient)
        {
            case E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_TMO:
            {
                MS_U16 u16Value = *(((MS_U16 *)pu8Data) + i);
                if (u16Value & 0xF000)
                {
                    printk("The %dth data is 0x%x, exceed max value, please check!!!\n", i + 1, u16Value);
                    return FALSE;
                }
                WRITE_TMO_DATA_FORMAT_1(pu8BaseAddr, u32Index, u16Value);
                i++;
                break;
            }
            case E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_GAMMA:
            {
                MS_U16 u16Value = *(((MS_U16 *)pu8Data) + i);
                WRITE_GAMMA_DATA_FORMAT_1(pu8BaseAddr, u32Index, u16Value);

                // patch for HW hdr_gamma table issue.
                // Write gamma/degamma simultaneously, gamma may be broken.
                WRITE_GAMMA_DATA_FORMAT_1(pu8BaseAddr + ((AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR + AUTO_DOWNLOAD_HDR_GAMMA_BLANKING) * BYTE_PER_WORD), u32Index, u16Value);
                i++;
                break;
            }
            case E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_DEGAMMA:
            {
                MS_U32 u32Value = *(((MS_U32 *)pu8Data) + i);
                if (u32Value & 0xFFF80000)
                {
                    printk("The %dth data is 0x%x, exceed max value, please check!!!\n", i + 1, (unsigned int)u32Value);
                    return FALSE;
                }
                WRITE_DEGAMMA_DATA_FORMAT_1(pu8BaseAddr, u32Index, u32Value);
                i++;
                break;
            }
            case E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_3DLUT:
            {
                MS_U16 u16BVal = *(((MS_U16 *)pu8Data) + i);
                MS_U16 u16GVal = *(((MS_U16 *)pu8Data) + i + 1);
                MS_U16 u16RVal = *(((MS_U16 *)pu8Data) + i + 2);
                MS_U16 u16Subindex = 0;
                if((i / 3 == _au32_3dlut_entry_num[0]) ||
                   (i / 3 == (_au32_3dlut_entry_num[0] + _au32_3dlut_entry_num[1])) ||
                   (i / 3 == (_au32_3dlut_entry_num[0] + _au32_3dlut_entry_num[1] + _au32_3dlut_entry_num[2])) ||
                   (i / 3 == (_au32_3dlut_entry_num[0] + _au32_3dlut_entry_num[1] + _au32_3dlut_entry_num[2] + _au32_3dlut_entry_num[3])) ||
                   (i / 3 == (_au32_3dlut_entry_num[0] + _au32_3dlut_entry_num[1] + _au32_3dlut_entry_num[2] + _au32_3dlut_entry_num[3] + _au32_3dlut_entry_num[4])) ||
                   (i / 3 == (_au32_3dlut_entry_num[0] + _au32_3dlut_entry_num[1] + _au32_3dlut_entry_num[2] + _au32_3dlut_entry_num[3] + _au32_3dlut_entry_num[4] + _au32_3dlut_entry_num[5])) ||
                   (i / 3 == (_au32_3dlut_entry_num[0] + _au32_3dlut_entry_num[1] + _au32_3dlut_entry_num[2] + _au32_3dlut_entry_num[3] + _au32_3dlut_entry_num[4] + _au32_3dlut_entry_num[5] + _au32_3dlut_entry_num[6])) ||
                   (i / 3 == (_au32_3dlut_entry_num[0] + _au32_3dlut_entry_num[1] + _au32_3dlut_entry_num[2] + _au32_3dlut_entry_num[3] + _au32_3dlut_entry_num[4] + _au32_3dlut_entry_num[5] + _au32_3dlut_entry_num[6] + _au32_3dlut_entry_num[7])))
                {
                    u32Index = 0;
                }

                if(i / 3 < _au32_3dlut_entry_num[0])
                {
                    u16Subindex = 0;
                }
                else if(i / 3 < (_au32_3dlut_entry_num[0] + _au32_3dlut_entry_num[1]))
                {
                    u16Subindex = 1;
                }
                else if(i / 3 < (_au32_3dlut_entry_num[0] + _au32_3dlut_entry_num[1] + _au32_3dlut_entry_num[2]))
                {
                    u16Subindex = 2;
                }
                else if(i / 3 < (_au32_3dlut_entry_num[0] + _au32_3dlut_entry_num[1] + _au32_3dlut_entry_num[2] + _au32_3dlut_entry_num[3]))
                {
                    u16Subindex = 3;
                }
                else if(i / 3 < (_au32_3dlut_entry_num[0] + _au32_3dlut_entry_num[1] + _au32_3dlut_entry_num[2] + _au32_3dlut_entry_num[3] + _au32_3dlut_entry_num[4]))
                {
                    u16Subindex = 4;
                }
                else if(i / 3 < (_au32_3dlut_entry_num[0] + _au32_3dlut_entry_num[1] + _au32_3dlut_entry_num[2] + _au32_3dlut_entry_num[3] + _au32_3dlut_entry_num[4] + _au32_3dlut_entry_num[5]))
                {
                    u16Subindex = 5;
                }
                else if(i / 3 < (_au32_3dlut_entry_num[0] + _au32_3dlut_entry_num[1] + _au32_3dlut_entry_num[2] + _au32_3dlut_entry_num[3] + _au32_3dlut_entry_num[4] + _au32_3dlut_entry_num[5] + _au32_3dlut_entry_num[6]))
                {
                    u16Subindex = 6;
                }
                else if(i / 3 < (_au32_3dlut_entry_num[0] + _au32_3dlut_entry_num[1] + _au32_3dlut_entry_num[2] + _au32_3dlut_entry_num[3] + _au32_3dlut_entry_num[4] + _au32_3dlut_entry_num[5] + _au32_3dlut_entry_num[6] + _au32_3dlut_entry_num[7]))
                {
                    u16Subindex = 7;
                }
                WRITE_3DLUT_DATA_FORMAT_1(pu8BaseAddr, u32Index, u16Subindex, u16RVal, u16GVal, u16BVal);
                i += 3;
                break;
            }
        }
        pu8BaseAddr += BYTE_PER_WORD;
        u32WriteNum++;
        u32Index++;
    }

    if (enSubClient == E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_GAMMA)
    {
        u32WriteNum += AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR + AUTO_DOWNLOAD_HDR_GAMMA_BLANKING;
    }

    u32Depth = u32Depth > u32WriteNum ? u32Depth : u32WriteNum;
    *pu32Depth = u32Depth;
#if DEBUG_HDR
    int j = 0;
    int l = 0;

    printk("\n#####################Dump Input Data####################\n");
    for (j = 0; j < u32DataSize;)
    {
        printk("The %04dth row: ", u16StartAddr++);
        switch (enSubClient)
        {
            case E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_TMO:
            {
                MS_U16 u16Value = *(((MS_U16 *)pu8Data) + j);
                printk("%02X \n", u16Value);
                j++;
                break;
            }
            case E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_GAMMA:
            {
                MS_U16 u16Value = *(((MS_U16 *)pu8Data) + j);
                printk("%02X \n", u16Value);
                j++;
                break;
            }
            case E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_DEGAMMA:
            {
                MS_U32 u32Value = *(((MS_U32 *)pu8Data) + j);
                printk("%04X \n", u32Value);
                j++;
                break;
            }
            case E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_3DLUT:
            {
                MS_U16 u16RVal = *(((MS_U16 *)pu8Data) + j);
                MS_U16 u16GVal = *(((MS_U16 *)pu8Data) + j + 1);
                MS_U16 u16BVal = *(((MS_U16 *)pu8Data) + j + 2);
                printk("%02X %02X %02X \n", u16RVal, u16GVal, u16BVal);
                j += 3;
                break;
            }
        }
        printk("\n");
    }
    printk("\n#####################Dump End####################\n\n");

    int k = AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR;
    printk("\n#####################Dump DRAM Buffer####################\n");
    for (j = 0; j < k; j++)
    {
        MS_U8 *pu8BaseAddr = (MS_U8 *)pVirtBaseAddr;
        printk("\nThe %04dth row: ", j);
        for (l = 0; l < BYTE_PER_WORD; l++)
        {
            switch (enSubClient)
            {
                case E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_TMO:
                case E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_GAMMA:
                case E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_DEGAMMA:
                {
                    printk("%02X ", *(pu8BaseAddr + BYTE_PER_WORD * j + l));
                    break;
                }
                case E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_3DLUT:
                {
                    printk("%02X ", *(pu8BaseAddr + BYTE_PER_WORD * j + l));
                    k = AUTO_DOWNLOAD_HDR_3DLUT_SRAM_MAX_ADDR;
                    break;
                }
            }
        }
    }
    printk("\n#####################Dump End####################\n");
#endif

    return TRUE;
}

static BOOL _Hal_XC_Auto_Download_Format_Xvycc_Data(EN_KDRV_XC_AUTODOWNLOAD_XVYCC_SUB_CLIENT enSubClient,
        MS_U8* pu8Data, MS_U32 u32Size)
{
    MS_U8* pVirtBaseAddr = NULL;
    MS_U8 *pu8BaseAddr = NULL;
    unsigned int i = 0;
    MS_U32 u32WriteNum = 0;
    MS_U32 u32DataSize = 0;
    MS_U32 u32Depth = _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC].u32Depth;

    if(pu8AutoDownloadDRAMBaseAddr[E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC] == NULL)
    {
        printk("%s: %d pu8AutoDownloadDRAMBaseAddr is NULL.\n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    pVirtBaseAddr = pu8AutoDownloadDRAMBaseAddr[E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC];
    pu8BaseAddr = (MS_U8 *)pVirtBaseAddr;

    switch (enSubClient)
    {
        case E_KDRV_XC_AUTODOWNLOAD_XVYCC_SUB_GAMMA:
        {
            u32DataSize = u32Size / sizeof(MS_U16);
            break;
        }
        case E_KDRV_XC_AUTODOWNLOAD_XVYCC_SUB_DEGAMMA:
        {
            u32DataSize = u32Size / sizeof(MS_U32);
            break;
        }
        default:
        {
            printk("InVaild sub client.\n");
            return FALSE;
        }
    }

    switch (enSubClient)
    {
        case E_KDRV_XC_AUTODOWNLOAD_XVYCC_SUB_GAMMA:
        {
            MS_U32 u32GammaDataStart = 0;

            u32GammaDataStart = (XVYCC_DEGAMMA_ENTRY + XVYCC_GAMMA_BLANKING);

            pu8BaseAddr += (u32GammaDataStart * BYTE_PER_WORD);

            for (i = 0; i < u32DataSize; i++)
            {
                MS_U16 u16Value = *((MS_U16 *)pu8Data + i);

                if (u16Value & 0xF000)
                {
                    printk("The %dth data is 0x%x, T exceed max value, please check!!!\n", i + 1, u16Value);
                    return FALSE;
                }

                WRITE_XVYCC_GAMMA_DATA_FORMAT_1(pu8BaseAddr, u16Value);

                pu8BaseAddr += BYTE_PER_WORD;
                u32WriteNum++;
            }
            break;
        }

        case E_KDRV_XC_AUTODOWNLOAD_XVYCC_SUB_DEGAMMA:
        {
            for (i = 0; i < u32DataSize; i++)
            {
                MS_U32 u32Value = *(((MS_U32 *)pu8Data) + i);

                if (u32Value & 0xFFF00000)
                {
                    printk("The %dth data is 0x%x, exceed max value, please check!!!\n", i + 1, u32Value);
                    return FALSE;
                }

                WRITE_XVYCC_DEGAMMA_DATA_FORMAT_1(pu8BaseAddr, u32Value);

                pu8BaseAddr += BYTE_PER_WORD;
                u32WriteNum++;
            }
            break;
        }

        default:
        {
            printk("InVaild sub client.\n");
            return FALSE;
        }

    }

    if (enSubClient == E_KDRV_XC_AUTODOWNLOAD_XVYCC_SUB_GAMMA)
    {
        u32WriteNum += XVYCC_GAMMA_BLANKING + XVYCC_DEGAMMA_ENTRY;
    }

    u32Depth = u32Depth > u32WriteNum ? u32Depth : u32WriteNum;
    _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC].u32Depth = u32Depth;

    return TRUE;
}


BOOL MHal_XC_WriteAutoDownload(EN_KDRV_XC_AUTODOWNLOAD_CLIENT enClient, MS_U8* pu8Data, MS_U32 u32Size, void* pParam)
{
    MS_U32 u32Depth = 0;
    MS_U8 *pu8BaseAddr = NULL;
    MS_U32 u32StartAddr = 0;
    MS_BOOL bFireing[E_KDRV_XC_AUTODOWNLOAD_CLIENT_MAX] = {0};
    mutex_lock(&_adl_mutex);

    u32Depth = _stClientInfo[enClient].u32Depth;
    u32StartAddr = _stClientInfo[enClient].u32StartAddr;

    if (_stClientInfo[enClient].bEnable == FALSE)
    {
        mutex_unlock(&_adl_mutex);
        printk("Auto downlaod client is disabled, please enable first.\n");
        return FALSE;
    }

    if(pu8AutoDownloadDRAMBaseAddr[enClient] == NULL)
    {
        mutex_unlock(&_adl_mutex);
        printk("%s: %d pu8AutoDownloadDRAMBaseAddr is NULL.\n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    pu8BaseAddr = pu8AutoDownloadDRAMBaseAddr[enClient];
    if (u32StartAddr != 0xFFFFFFFF)
    {
        pu8BaseAddr += BYTE_PER_WORD * u32StartAddr;
    }

    if (_stClientInfo[enClient].enMode == E_KDRV_XC_AUTODOWNLOAD_TRIGGER_MODE)
    {
        if(enClient == E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR)
        {
            bFireing[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR] = (MS_BOOL)(MHal_XC_R2BYTE(REG_SC_BK67_28_L) & 0x0001);
        }
        else if(enClient == E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC)
        {
            bFireing[E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC] = (MS_BOOL)(MHal_XC_R2BYTE(REG_SC_BK67_11_L) & 0x0010);
        }
        else
        {
            //printk("Need add other cases.\n");
        }

        // writing data now
        if (bFireing[enClient] == TRUE)
        {
            mutex_unlock(&_adl_mutex);
            printk("Wait a moment, the lastest datas is writing.\n");
            return FALSE;
        }
        else
        {
            // the lasteset datas is writing done.
            if (_enAutoDownloadStatus[enClient] == E_KDRV_XC_AUTO_DOWNLOAD_FIRED)
            {
                // clear buffer
                memset(pu8BaseAddr, 0, (u32Depth * BYTE_PER_WORD));
                _stClientInfo[enClient].u32StartAddr = 0xFFFFFFFF;
                _stClientInfo[enClient].u32Depth = 0;
                _enAutoDownloadStatus[enClient] = E_KDRV_XC_AUTO_DOWNLOAD_WRITED;
            }
            else
            {
                _enAutoDownloadStatus[enClient] = E_KDRV_XC_AUTO_DOWNLOAD_WRITED;
            }
        }
    }

    switch(enClient)
    {
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR:
        {
            ST_KDRV_XC_AUTODOWNLOAD_FORMAT_INFO* pstFormatInfo = (ST_KDRV_XC_AUTODOWNLOAD_FORMAT_INFO *)pParam;
            MS_U16 u16StartAddr = 0;
            MS_U32 u32MaxSize = 0;
            MS_U32 u32MaxAddr = 0;
            MS_U32 u32WriteNum = 0;
            MS_BOOL bRet = 0;
            switch (pstFormatInfo->enSubClient)
            {
                case E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_TMO:
                case E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_GAMMA:
                {
                    u32MaxSize = AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR * sizeof(MS_U16);
                    u32MaxAddr = AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR;
                    u32WriteNum = u32Size / sizeof(MS_U16);
                    break;
                }
                case E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_DEGAMMA:
                {
                    u32MaxSize = AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR * sizeof(MS_U32);
                    u32MaxAddr = AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR;
                    u32WriteNum = u32Size / sizeof(MS_U32);
                    break;
                }
                case E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_3DLUT:
                {
                    u32MaxSize = AUTO_DOWNLOAD_HDR_3DLUT_SRAM_MAX_ADDR * 3 * sizeof(MS_U16);
                    u32MaxAddr = AUTO_DOWNLOAD_HDR_3DLUT_SRAM_MAX_ADDR;
                    u32WriteNum = u32Size / (3 * sizeof(MS_U16));
                    break;
                }
                default:
                {
                    mutex_unlock(&_adl_mutex);
                    printk("Write auto download fail, invaild paramters, subClient: %d\n", pstFormatInfo->enSubClient);
                    return FALSE;
                }
            }
            if (pstFormatInfo->bEnableRange == TRUE)
            {
                if ((pstFormatInfo->u16StartAddr <= pstFormatInfo->u16EndAddr) && (pstFormatInfo->u16EndAddr < u32MaxAddr)
                    && (pstFormatInfo->u16StartAddr + u32WriteNum - 1) < u32MaxAddr)
                {
                    u16StartAddr = pstFormatInfo->u16StartAddr;
                }
                else
                {
                    mutex_unlock(&_adl_mutex);
                    printk("Write auto download fail, invaild paramters, subClient: %d, size: %d, addr range(enable, start, end) = (%d, %d, %d)\n",
                           pstFormatInfo->enSubClient, u32Size, pstFormatInfo->bEnableRange, pstFormatInfo->u16StartAddr, pstFormatInfo->u16EndAddr);
                    return FALSE;
                }
            }
            else
            {
                if (u32Size > u32MaxSize)
                {
                    mutex_unlock(&_adl_mutex);
                    printk("Write auto download fail, invaild paramters, subClient: %d, size: %d\n", pstFormatInfo->enSubClient, u32Size);
                    return FALSE;
                }
            }

            // update start addr of DRAM
            u32StartAddr = _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].u32StartAddr;
            if (u32StartAddr == 0xFFFFFFFF)
            {
                _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].u32StartAddr = u16StartAddr;
            }
            else
            {
                _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].u32StartAddr = u32StartAddr > u16StartAddr ? u16StartAddr : u32StartAddr;
            }

            bRet = _Hal_XC_Auto_Download_Format_Hdr_Data(pstFormatInfo->enSubClient, pu8AutoDownloadDRAMBaseAddr[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR], pu8Data, u32Size, u16StartAddr, &_stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].u32Depth);
            mutex_unlock(&_adl_mutex);

            return bRet;
        }
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC:
        {
            ST_KDRV_XC_AUTODOWNLOAD_XVYCC_FORMAT_INFO* pstFormatInfo = (ST_KDRV_XC_AUTODOWNLOAD_XVYCC_FORMAT_INFO *)pParam;
            MS_BOOL bRet = FALSE;
            MS_U32 u32MaxSize = 0;
            switch (pstFormatInfo->enSubClient)
            {
                case E_KDRV_XC_AUTODOWNLOAD_XVYCC_SUB_GAMMA:
                {
                    u32MaxSize = XVYCC_GAMMA_ENTRY * sizeof(MS_U16);
                    break;
                }
                case E_KDRV_XC_AUTODOWNLOAD_XVYCC_SUB_DEGAMMA:
                {
                    u32MaxSize = XVYCC_DEGAMMA_ENTRY * sizeof(MS_U32);
                    break;
                }
                default:
                {
                    mutex_unlock(&_adl_mutex);
                    printk("Write auto download fail, invaild paramters, subClient: %d\n", pstFormatInfo->enSubClient);
                    return FALSE;
                }

            }

            if (u32Size > u32MaxSize)
            {
                mutex_unlock(&_adl_mutex);
                printk("Write auto download fail, invaild paramters, subClient: %d, size: %d\n", pstFormatInfo->enSubClient, u32Size);
                return FALSE;
            }

            bRet = _Hal_XC_Auto_Download_Format_Xvycc_Data(pstFormatInfo->enSubClient, pu8Data, u32Size);
            mutex_unlock(&_adl_mutex);

            return bRet;
        }
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_OP2GAMMA:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_FRCOP2GAMMA:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_ODTABLE1:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_ODTABLE2:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_ODTABLE3:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_DEMURA:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_OP2LUT:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_T3D_0:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_T3D_1:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_FRCSPTPOPM:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_FOOPM:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_MAX:
        default:
        {
            mutex_unlock(&_adl_mutex);
            return FALSE;
        }

    }

    mutex_unlock(&_adl_mutex);
    return TRUE;
}

BOOL MHal_XC_FireAutoDownload(EN_KDRV_XC_AUTODOWNLOAD_CLIENT enClient)
{
    mutex_lock(&_adl_mutex);

    if (_stClientInfo[enClient].bEnable == FALSE)
    {
        mutex_unlock(&_adl_mutex);
        printk("Auto downlaod client is disabled, please enable first.\n");
        return FALSE;
    }

    switch(enClient)
    {
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR:
        {
            MS_PHY phyBaseAddr = 0;
            MS_U32 u32Depth = 0;
            MS_U32 u32StartAddr = 0;
            MS_U16 u16inputVTT = MHal_XC_R2BYTEMSK(REG_SC_BK01_1F_L,0x1FFF);
            MS_U16 u16inputHperiod = MHal_XC_R2BYTEMSK(REG_SC_BK01_20_L,0x3FFF);
            if(u16inputVTT==0 || u16inputHperiod==0 || u16inputHperiod==0x3FFF || u16inputVTT==0x1FFF)
            {
                if(!CFD_IS_MM(_stCfdInit[0].u8InputSource))    //To avoid dolby MM garbage
                {
                    mutex_unlock(&_adl_mutex);
                    printk("Auto downlaod HDR fail, because there is no input signal.\n");
                    return FALSE;
                }
            }
            phyBaseAddr = _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].phyBaseAddr;
            u32Depth = _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].u32Depth;
            u32StartAddr = _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].u32StartAddr;

            // set baseaddr
            MHal_XC_W2BYTE(REG_SC_BK67_29_L, ((phyBaseAddr + BYTE_PER_WORD * u32StartAddr) / BYTE_PER_WORD) & 0x0000FFFF);
            MHal_XC_W2BYTEMSK(REG_SC_BK67_2A_L, (((phyBaseAddr + BYTE_PER_WORD * u32StartAddr) / BYTE_PER_WORD) >> 16 & 0x0000FFFF), 0x07FF);

            // set depth
            MHal_XC_W2BYTE(REG_SC_BK67_2B_L, u32Depth);
            MHal_XC_W2BYTE(REG_SC_BK67_2C_L, u32Depth);

            __cpuc_flush_dcache_area(pu8AutoDownloadDRAMBaseAddr[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR], u32Depth * BYTE_PER_WORD);
            // enable auto download
            MHal_XC_W2BYTEMSK(REG_SC_BK67_28_L, 1, BIT(0));
            break;
        }
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC:
        {
            MS_PHY phyBaseAddr = 0;
            MS_U32 u32Depth = 0;

            phyBaseAddr = _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC].phyBaseAddr;
            u32Depth = _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC].u32Depth;

            // set baseaddr
            MHal_XC_W2BYTE(REG_SC_BK67_16_L, (phyBaseAddr / BYTE_PER_WORD) & 0x0000FFFF);
            MHal_XC_W2BYTEMSK(REG_SC_BK67_17_L, ((phyBaseAddr / BYTE_PER_WORD) >> 16 & 0x0000FFFF), 0x07FF);

            //reg_client6_depth
            MHal_XC_W2BYTE(REG_SC_BK67_1A_L, min(u32Depth,0xFFFF));

            //reg_client6_req_len
            MHal_XC_W2BYTE(REG_SC_BK67_1D_L, min(u32Depth,0xFFFF));

            __cpuc_flush_dcache_area(pu8AutoDownloadDRAMBaseAddr[E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC], u32Depth * BYTE_PER_WORD);

            // enable auto download
            MHal_XC_W2BYTEMSK(REG_SC_BK67_11_L, BIT(2), BIT(2));

            break;
        }
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_OP2GAMMA:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_FRCOP2GAMMA:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_ODTABLE1:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_ODTABLE2:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_ODTABLE3:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_DEMURA:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_OP2LUT:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_T3D_0:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_T3D_1:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_FRCSPTPOPM:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_FOOPM:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_MAX:
        default:
        {
            mutex_unlock(&_adl_mutex);
            return FALSE;
        }

    }

    _enAutoDownloadStatus[enClient] = E_KDRV_XC_AUTO_DOWNLOAD_FIRED;
    mutex_unlock(&_adl_mutex);

    return TRUE;
}

BOOL MHal_XC_ConfigHDRAutoDownloadStoredInfo(MS_PHY phyLutBaseAddr, MS_U8 *pu8VirtLutBaseAddr, MS_U32 u32Size)
{
    // init client info
    _stHDRClientInfo.phyBaseAddr = phyLutBaseAddr;
    _stHDRClientInfo.u32Size = u32Size;
    _stHDRClientInfo.u32StartAddr = 0xFFFFFFFF;
    _stHDRClientInfo.u32Depth = 0;

    pu8HDRAutoDownloadDRAMBaseAddr = pu8VirtLutBaseAddr;

    if (pu8HDRAutoDownloadDRAMBaseAddr != NULL)
    {
        memset(pu8HDRAutoDownloadDRAMBaseAddr, 0, u32Size);
    }
    else
    {
        printk("%s: %d, auto downlaod mmap pa fail.\n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    return TRUE;
}

BOOL MHal_XC_StoreHDRAutoDownload(MS_U8* pu8Data, MS_U32 u32Size, void* pParam)
{
    MS_U8 *pu8BaseAddr = NULL;
    MS_U32 u32StartAddr = 0;
    ST_KDRV_XC_AUTODOWNLOAD_FORMAT_INFO* pstFormatInfo;
    MS_U16 u16StartAddr = 0;
    MS_U32 u32MaxSize = 0;
    MS_U32 u32MaxAddr = 0;
    MS_U32 u32WriteNum = 0;
    MS_BOOL bRet = 0;

    u32StartAddr = _stHDRClientInfo.u32StartAddr;
    pu8BaseAddr = NULL;

    if(pu8HDRAutoDownloadDRAMBaseAddr == NULL)
    {
        printk("%s: %d pu8HDRAutoDownloadDRAMBaseAddr is NULL.\n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    pu8BaseAddr = pu8HDRAutoDownloadDRAMBaseAddr;
    if (u32StartAddr != 0xFFFFFFFF)
    {
        pu8BaseAddr += BYTE_PER_WORD * u32StartAddr;
    }

    pstFormatInfo = (ST_KDRV_XC_AUTODOWNLOAD_FORMAT_INFO *)pParam;
    u16StartAddr = 0;
    u32MaxSize = 0;
    u32MaxAddr = 0;
    u32WriteNum = 0;
    switch (pstFormatInfo->enSubClient)
    {
        case E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_TMO:
        case E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_GAMMA:
        {
            u32MaxSize = AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR * sizeof(MS_U16);
            u32MaxAddr = AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR;
            u32WriteNum = u32Size / sizeof(MS_U16);
            break;
        }
        case E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_DEGAMMA:
        {
            u32MaxSize = AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR * sizeof(MS_U32);
            u32MaxAddr = AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR;
            u32WriteNum = u32Size / sizeof(MS_U32);
            break;
        }
        case E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_3DLUT:
        {
            u32MaxSize = AUTO_DOWNLOAD_HDR_3DLUT_SRAM_MAX_ADDR * 3 * sizeof(MS_U16);
            u32MaxAddr = AUTO_DOWNLOAD_HDR_3DLUT_SRAM_MAX_ADDR;
            u32WriteNum = u32Size / (3 * sizeof(MS_U16));
            break;
        }
        default:
        {
            printk("Write auto download fail, invaild paramters, subClient: %d\n", pstFormatInfo->enSubClient);
            return FALSE;
        }
    }
    if (pstFormatInfo->bEnableRange == TRUE)
    {
        if ((pstFormatInfo->u16StartAddr <= pstFormatInfo->u16EndAddr) && (pstFormatInfo->u16EndAddr < u32MaxAddr)
            && (pstFormatInfo->u16StartAddr + u32WriteNum - 1) < u32MaxAddr)
        {
            u16StartAddr = pstFormatInfo->u16StartAddr;
        }
        else
        {
            printk("Write auto download fail, invaild paramters, subClient: %d, size: %d, addr range(enable, start, end) = (%d, %d, %d)\n",
                   pstFormatInfo->enSubClient, u32Size, pstFormatInfo->bEnableRange, pstFormatInfo->u16StartAddr, pstFormatInfo->u16EndAddr);
            return FALSE;
        }
    }
    else
    {
        if (u32Size > u32MaxSize)
        {
            printk("Write auto download fail, invaild paramters, subClient: %d, size: %d\n", pstFormatInfo->enSubClient, u32Size);
            return FALSE;
        }
    }

    // update start addr of DRAM
    u32StartAddr = _stHDRClientInfo.u32StartAddr;
    if (u32StartAddr == 0xFFFFFFFF)
    {
        _stHDRClientInfo.u32StartAddr = u16StartAddr;
    }
    else
    {
        _stHDRClientInfo.u32StartAddr = u32StartAddr > u16StartAddr ? u16StartAddr : u32StartAddr;
    }

    bRet = _Hal_XC_Auto_Download_Format_Hdr_Data(pstFormatInfo->enSubClient, pu8HDRAutoDownloadDRAMBaseAddr, pu8Data, u32Size, u16StartAddr, &_stHDRClientInfo.u32Depth);

    return bRet;
}

static MS_PHY _Hal_XC_GetAutoDownloadMemOffset(MS_U8 u8Index)
{
    return (AUTO_DOWNLOAD_HDR_3DLUT_SRAM_MAX_ADDR + u8Index * (AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR + AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR + AUTO_DOWNLOAD_HDR_GAMMA_BLANKING)) * BYTE_PER_WORD;
}

BOOL MHal_XC_WriteStoredHDRAutoDownload(MS_U8 *pu8LutData, MS_U32 u32Size, MS_U8 u8Index,
                                        MS_PHY *pphyFireAdlAddr, MS_U32 *pu32Depth)
{
    MS_PHY phyBaseAddr = 0;
    MS_U32 u32MiuNo = 0;
    mutex_lock(&_adl_mutex);

    phyBaseAddr = _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].phyBaseAddr;
    u32MiuNo = _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].u32MiuNo;
    // copy data into adl mem

    if ((pu8LutData != NULL) && (pu8AutoDownloadDRAMBaseAddr[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR] != NULL))
    {
#ifdef XC_REPLACE_MEMCPY_BY_BDMA
        MS_PHY phyDstAddr = 0;
        MS_U32 u32IdxOffset = _Hal_XC_GetAutoDownloadMemOffset(u8Index);
        _miu_offset_to_phy(u32MiuNo, phyBaseAddr, phyDstAddr);
        phyDstAddr = phyDstAddr + u32IdxOffset;
        MHal_XC_MemCpy_by_BDMA((MS_U32)pu8LutData, phyDstAddr, u32Size);
#else
        MS_U8 *pu8DstAddr = pu8AutoDownloadDRAMBaseAddr[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR] + _Hal_XC_GetAutoDownloadMemOffset(u8Index);
        memcpy(pu8DstAddr, pu8LutData, u32Size);
#endif
    }
    else
    {
        mutex_unlock(&_adl_mutex);
        printk("%s, malloc memory fail.\n", __FUNCTION__);
        return FALSE;
    }

    *pphyFireAdlAddr = phyBaseAddr + ((u8Index == 0) ? 0 : AUTO_DOWNLOAD_HDR_3DLUT_SRAM_MAX_ADDR + u8Index * (AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR + AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR + AUTO_DOWNLOAD_HDR_GAMMA_BLANKING)) * BYTE_PER_WORD;
    *pu32Depth = u32Size / BYTE_PER_WORD + ((u8Index == 0) ? AUTO_DOWNLOAD_HDR_3DLUT_SRAM_MAX_ADDR : 0);

    mutex_unlock(&_adl_mutex);

    return TRUE;
}

MS_U32 MHal_XC_GetHDRAutoDownloadStoredSize()
{
    return _stHDRClientInfo.u32Depth * BYTE_PER_WORD;
}

BOOL MHal_XC_GetAutoDownloadCaps(EN_KDRV_XC_AUTODOWNLOAD_CLIENT enClient, MS_BOOL *pbSupported)
{
    switch(enClient)
    {
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_OP2GAMMA:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_FRCOP2GAMMA:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_ODTABLE1:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_ODTABLE2:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_ODTABLE3:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_DEMURA:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_OP2LUT:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_T3D_0:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_T3D_1:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_FRCSPTPOPM:
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_FOOPM:
        {
            *pbSupported = TRUE;
            break;
        }
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_MAX:
        default:
        {
            *pbSupported = FALSE;
            break;
        }

    }

    return TRUE;
}

BOOL MHal_XC_SetColorFormat(MS_BOOL bHDMI422)
{
    MS_U16 u16Value = (bHDMI422 == TRUE) ? 0 : 1;

    MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, u16Value << 4, BIT(4));
    // PQ bypass control
    if (bHDMI422 == TRUE)
    {
        MHal_XC_W2BYTEMSK(REG_SC_BK7A_01_L, 0x8000, 0xF000);
    }
    else
    {
        MHal_XC_W2BYTEMSK(REG_SC_BK7A_01_L, 0x0, 0xF000);
    }

    return TRUE;
}

BOOL MHal_XC_CFDControl(ST_KDRV_XC_CFD_CONTROL_INFO *pstKdrvCFDCtrlInfo)
{
//#define     XC_KDBG(_fmt, _args...)        printk("%s:%d " _fmt, __FUNCTION__, __LINE__, ##_args)
    EN_KDRV_XC_CFD_CTRL_TYPE enCtrlType = pstKdrvCFDCtrlInfo->enCtrlType;
    //MS_U32 u32ParamLen = pstKdrvCFDCtrlInfo->u32ParamLen;
    static MS_BOOL bInitStatus[2] = {FALSE, FALSE};
    static MS_BOOL bHueChange[2] = {FALSE, FALSE};
    static MS_BOOL bContrastChange[2] = {FALSE, FALSE};
    static MS_BOOL bSaturationChange[2] = {FALSE, FALSE};
    static MS_BOOL bColorRangeChange[2] = {FALSE, FALSE};
    static MS_BOOL bLinearRgbChange[2] = {FALSE, FALSE};
    static MS_BOOL bHdrChange[2] = {FALSE, FALSE};
    static MS_BOOL bHdmiChange[2] = {FALSE, FALSE};
    static MS_BOOL bMmChange[2] = {FALSE, FALSE};
    static MS_BOOL bAnalogChange[2] = {FALSE, FALSE};
    static MS_BOOL bSkipPictureSettingChange[2] = {FALSE, FALSE};

    //MS_U16 u16RetVal = 0;

    switch(enCtrlType)
    {
        case E_KDRV_XC_CFD_CTRL_SET_INIT:
        {
            ST_KDRV_XC_CFD_INIT *pstXcCfdInit = (ST_KDRV_XC_CFD_INIT*)pstKdrvCFDCtrlInfo->pParam;
            mutex_lock(&_cfd_mutex);
            _u32NoSignalCount = 0;
            mutex_unlock(&_cfd_mutex);
            XC_KDBG("\033[31mCFD initalize \033[m\n");
            XC_KDBG("\033[31m  Window is %d \033[m\n", pstXcCfdInit->u8Win);
            XC_KDBG("\033[31m  Input source is %d \033[m\n", pstXcCfdInit->u8InputSource);
            memcpy(&_stCfdInit[pstXcCfdInit->u8Win], pstXcCfdInit, sizeof(ST_KDRV_XC_CFD_INIT));
            MHal_XC_EnableHDR(ENABLE);

            if(MAIN_WINDOW == pstXcCfdInit->u8Win)
            {
                // this is for first black frame is not black in dolby ott mode
                // but in normal pip case, we have to avoid sub win mute main win
                // if sub is no signal, then cfd code flow just call cfd_init
                MHal_XC_MuteHDR();
                _enHDRType = 0 ;
            }

            bTmoFireEnable = FALSE;
            _bCfdInited = TRUE;
            _bTimingChanged = TRUE;
            bInitStatus[pstXcCfdInit->u8Win] = TRUE;
            pstKdrvCFDCtrlInfo->u16ErrCode = E_CFD_MC_ERR_NOERR;
            bHueChange[pstXcCfdInit->u8Win] = TRUE;
            bContrastChange[pstXcCfdInit->u8Win] = TRUE;
            bSaturationChange[pstXcCfdInit->u8Win] = TRUE;
            bColorRangeChange[pstXcCfdInit->u8Win] = TRUE;
            bLinearRgbChange[pstXcCfdInit->u8Win] = TRUE;
            bHdrChange[pstXcCfdInit->u8Win] = TRUE;
            bHdmiChange[pstXcCfdInit->u8Win] = TRUE;
            bMmChange[pstXcCfdInit->u8Win] = TRUE;
            bAnalogChange[pstXcCfdInit->u8Win] = TRUE;
            bSkipPictureSettingChange[pstXcCfdInit->u8Win] = TRUE;

            // dolby HDMI to openHDR OTT, video display garbage
            // because dma does not be closed
            MHal_XC_SetDMAPath(EN_KDRV_XC_HDR_DMA_BYPASS);

            break;
        }

        case E_KDRV_XC_CFD_CTRL_SET_HDMI:
        {
            ST_KDRV_XC_CFD_HDMI *pstXcCfdHdmi = (ST_KDRV_XC_CFD_HDMI*)pstKdrvCFDCtrlInfo->pParam;
            XC_KDBG("\033[31mHDMI input source \033[m\n");
            XC_KDBG("\033[31m  Window is %d \033[m\n", pstXcCfdHdmi->u8Win);
            XC_KDBG("\033[31m  HDMI %s full range\033[m\n", pstXcCfdHdmi->bIsFullRange?"is":"isn't");
            XC_KDBG("\033[31m  AVI infoframe is (%d, %d, %d, %d, %d) \033[m\n", pstXcCfdHdmi->u8PixelFormat, pstXcCfdHdmi->u8Colorimetry, pstXcCfdHdmi->u8ExtendedColorimetry, pstXcCfdHdmi->u8RgbQuantizationRange, pstXcCfdHdmi->u8YccQuantizationRange);
            XC_KDBG("\033[31m  HDR infoframe %s exists\033[m\n", pstXcCfdHdmi->bHDRInfoFrameValid?"is":"isn't");

            if (memcmp(pstXcCfdHdmi, &_stCfdHdmi[pstXcCfdHdmi->u8Win], sizeof(ST_KDRV_XC_CFD_HDMI)) != 0)
            {
                bHdmiChange[pstXcCfdHdmi->u8Win] |= TRUE;
            }
            memcpy(&_stCfdHdmi[pstXcCfdHdmi->u8Win], pstXcCfdHdmi, sizeof(ST_KDRV_XC_CFD_HDMI));

            // for both Open and Dolby HDR
            // we must set intial auto seamless to FALSE
            // otherwise HDMI open HDR will be PQ bypssed due to HW auto seamless
            MHal_XC_EnableAutoSeamless(FALSE);
            MHal_XC_SetDMAPath(EN_KDRV_XC_HDR_DMA_BYPASS);
            pstKdrvCFDCtrlInfo->u16ErrCode = E_CFD_MC_ERR_NOERR;
            break;
        }

        case E_KDRV_XC_CFD_CTRL_SET_VGA:
        case E_KDRV_XC_CFD_CTRL_SET_TV:
        case E_KDRV_XC_CFD_CTRL_SET_CVBS:
        case E_KDRV_XC_CFD_CTRL_SET_SVIDEO:
        case E_KDRV_XC_CFD_CTRL_SET_YPBPR:
        case E_KDRV_XC_CFD_CTRL_SET_SCART:
        case E_KDRV_XC_CFD_CTRL_SET_MM:
        case E_KDRV_XC_CFD_CTRL_SET_DVI:
        {
            ST_KDRV_XC_CFD_ANALOG *pstXcCfdAnalog = (ST_KDRV_XC_CFD_ANALOG*)pstKdrvCFDCtrlInfo->pParam;
            XC_KDBG("\033[31mAnalog input source \033[m\n");
            XC_KDBG("\033[31m  Window is %d \033[m\n", pstXcCfdAnalog->u8Win);
            XC_KDBG("\033[31m  Color format is %d\033[m\n", pstXcCfdAnalog->u8ColorFormat);
            XC_KDBG("\033[31m  Color data format is %d\033[m\n", pstXcCfdAnalog->u8ColorDataFormat);
            XC_KDBG("\033[31m  Analog %s full range\033[m\n", pstXcCfdAnalog->bIsFullRange?"is":"isn't");
            XC_KDBG("\033[31m  Color primaries is %d\033[m\n", pstXcCfdAnalog->u8ColorPrimaries);
            XC_KDBG("\033[31m  Transfer characteristics  is %d\033[m\n", pstXcCfdAnalog->u8TransferCharacteristics);
            XC_KDBG("\033[31m  Matrix coefficients is %d\033[m\n", pstXcCfdAnalog->u8MatrixCoefficients);

            if (!CFD_IS_HDMI(_stCfdInit[pstXcCfdAnalog->u8Win].u8InputSource) &&
                !CFD_IS_MM(_stCfdInit[pstXcCfdAnalog->u8Win].u8InputSource) &&
                !CFD_IS_DTV(_stCfdInit[pstXcCfdAnalog->u8Win].u8InputSource))
            {
                if (memcmp(pstXcCfdAnalog, &_stCfdAnalog[pstXcCfdAnalog->u8Win], sizeof(ST_KDRV_XC_CFD_ANALOG)) != 0)
                {
                    bAnalogChange[pstXcCfdAnalog->u8Win] |= TRUE;
                }
            }
            memcpy(&_stCfdAnalog[pstXcCfdAnalog->u8Win], pstXcCfdAnalog, sizeof(ST_KDRV_XC_CFD_ANALOG));

            pstKdrvCFDCtrlInfo->u16ErrCode = E_CFD_MC_ERR_NOERR;
            break;
        }

        case E_KDRV_XC_CFD_CTRL_SET_PANEL:
        {
            //MS_U32 *pu32Version = (MS_U32*)pstKdrvCFDCtrlInfo->pParam;
            MS_U16 *pu16Length = (MS_U16*)(pstKdrvCFDCtrlInfo->pParam + sizeof(MS_U32));

            MS_U16 u16CpLength = sizeof(ST_KDRV_XC_CFD_PANEL);

            if ((*pu16Length) < u16CpLength)
            {
                u16CpLength = (*pu16Length) ;
            }

            memset(&_stCfdPanel, 0, sizeof(ST_KDRV_XC_CFD_PANEL));
            memcpy(&_stCfdPanel, pstKdrvCFDCtrlInfo->pParam, u16CpLength);

            XC_KDBG("\033[31mPanel info \033[m\n");
            XC_KDBG("\033[31m  R is (%d, %d)\033[m\n", _stCfdPanel.u16Display_Primaries_x[0], _stCfdPanel.u16Display_Primaries_y[0]);
            XC_KDBG("\033[31m  G is (%d, %d)\033[m\n", _stCfdPanel.u16Display_Primaries_x[1], _stCfdPanel.u16Display_Primaries_y[1]);
            XC_KDBG("\033[31m  B is (%d, %d)\033[m\n", _stCfdPanel.u16Display_Primaries_x[2], _stCfdPanel.u16Display_Primaries_y[2]);
            XC_KDBG("\033[31m  W is (%d, %d)\033[m\n", _stCfdPanel.u16White_point_x, _stCfdPanel.u16White_point_y);
            XC_KDBG("\033[31m  Luminanceis (%d, %d, %d)\033[m\n", _stCfdPanel.u16MaxLuminance, _stCfdPanel.u16MedLuminance, _stCfdPanel.u16MinLuminance);
            XC_KDBG("\033[31m  Linear RGB %d\033[m\n", _stCfdPanel.bLinearRgb);

            pstKdrvCFDCtrlInfo->u16ErrCode = MHal_XC_CFD_SetPanelParam(&_stCfdPanel);
            if (pstKdrvCFDCtrlInfo->u16ErrCode != E_CFD_MC_ERR_NOERR)
            {
                return FALSE;
            }
            break;
        }

        case E_KDRV_XC_CFD_CTRL_SET_HDR:
        {
            ST_KDRV_XC_CFD_HDR stCfdHdr;

            MS_U16 *pu16Length = (MS_U16*)(pstKdrvCFDCtrlInfo->pParam + sizeof(MS_U32));

            MS_U16 u16CpLength = sizeof(ST_KDRV_XC_CFD_HDR);

            if ((*pu16Length) < u16CpLength)
            {
                u16CpLength = (*pu16Length) ;
            }

            memset(&stCfdHdr, 0, sizeof(ST_KDRV_XC_CFD_HDR));
            memcpy(&stCfdHdr, pstKdrvCFDCtrlInfo->pParam, u16CpLength);

            XC_KDBG("\033[31mHDR info \033[m\n");
            XC_KDBG("\033[31m  Window is %d \033[m\n", stCfdHdr.u8Win);
            XC_KDBG("\033[31m  HDR type is %d \033[m\n", stCfdHdr.u8HdrType);
            XC_KDBG("\033[31m  HDR Level is %d \033[m\n", stCfdHdr.u8TmoLevel);

            if (stCfdHdr.u8HdrType != _stCfdHdr[stCfdHdr.u8Win].u8HdrType)
            {
                bHdrChange[stCfdHdr.u8Win] |= TRUE;
            }

            memcpy(&_stCfdHdr[stCfdHdr.u8Win], &stCfdHdr, sizeof(ST_KDRV_XC_CFD_HDR));
            // also enable HDR clk, auther: jonathan.yen
            MHal_XC_EnableHDRCLK(TRUE,TRUE);

            if (IS_DOLBY_HDR(stCfdHdr.u8Win))
            {
                if((CFD_IS_HDMI(_stCfdInit->u8InputSource)) || (CFD_IS_DVI(_stCfdInit->u8InputSource)))
                {
                    MHal_XC_SetHDR_DMARequestOFF(DISABLE, TRUE);
                }
                else
                {
                    MHal_XC_SetHDR_DMARequestOFF(ENABLE, TRUE);
                }
            }
            else
            {
                MHal_XC_SetHDR_DMARequestOFF(ENABLE, TRUE);
            }

            pstKdrvCFDCtrlInfo->u16ErrCode = E_CFD_MC_ERR_NOERR;
            break;
        }

        case E_KDRV_XC_CFD_CTRL_SET_EDID:
        {
            ST_KDRV_XC_CFD_EDID *pstCfdEdid = (ST_KDRV_XC_CFD_EDID*)pstKdrvCFDCtrlInfo->pParam;
            ST_KDRV_XC_CFD_FIRE *pstCfdFire;
            int timer = 0;
            MS_BOOL bEnable = 0;
            XC_KDBG("\033[31mEDID info \033[m\n");
            XC_KDBG("u8HDMISinkHDRDataBlockValid is %d\n", pstCfdEdid->u8HDMISinkHDRDataBlockValid);
            XC_KDBG("u8HDMISinkEOTF is %d\n", pstCfdEdid->u8HDMISinkEOTF);
            XC_KDBG("u8HDMISinkSM is %d\n", pstCfdEdid->u8HDMISinkSM);
            XC_KDBG("u8HDMISinkDesiredContentMaxLuminance is %d\n", pstCfdEdid->u8HDMISinkDesiredContentMaxLuminance);
            XC_KDBG("u8HDMISinkDesiredContentMaxFrameAvgLuminance is %d\n", pstCfdEdid->u8HDMISinkDesiredContentMaxFrameAvgLuminance);
            XC_KDBG("u8HDMISinkDesiredContentMinLuminance is %d\n", pstCfdEdid->u8HDMISinkDesiredContentMinLuminance);
            XC_KDBG("u8HDMISinkHDRDataBlockLength is %d\n", pstCfdEdid->u8HDMISinkHDRDataBlockLength);
            XC_KDBG("u16Display_Primaries_x[0] is %d\n", pstCfdEdid->u16Display_Primaries_x[0]);
            XC_KDBG("u16Display_Primaries_x[1] is %d\n", pstCfdEdid->u16Display_Primaries_x[1]);
            XC_KDBG("u16Display_Primaries_x[2] is %d\n", pstCfdEdid->u16Display_Primaries_x[2]);
            XC_KDBG("u16Display_Primaries_y[0] is %d\n", pstCfdEdid->u16Display_Primaries_y[0]);
            XC_KDBG("u16Display_Primaries_y[1] is %d\n", pstCfdEdid->u16Display_Primaries_y[1]);
            XC_KDBG("u16Display_Primaries_y[2] is %d\n", pstCfdEdid->u16Display_Primaries_y[2]);
            XC_KDBG("u16White_point_x is %d\n", pstCfdEdid->u16White_point_x);
            XC_KDBG("u16White_point_y is %d\n", pstCfdEdid->u16White_point_y);
            XC_KDBG("u8HDMISinkEDIDBaseBlockVersion is %d\n", pstCfdEdid->u8HDMISinkEDIDBaseBlockVersion);
            XC_KDBG("u8HDMISinkEDIDBaseBlockReversion is %d\n", pstCfdEdid->u8HDMISinkEDIDBaseBlockReversion);
            XC_KDBG("u8HDMISinkEDIDCEABlockReversion is %d\n", pstCfdEdid->u8HDMISinkEDIDCEABlockReversion);
            XC_KDBG("u8HDMISinkVCDBValid is %d\n", pstCfdEdid->u8HDMISinkVCDBValid);
            XC_KDBG("u8HDMISinkSupportYUVFormat is %d\n", pstCfdEdid->u8HDMISinkSupportYUVFormat);
            XC_KDBG("u8HDMISinkExtendedColorspace is %d\n", pstCfdEdid->u8HDMISinkExtendedColorspace);
            XC_KDBG("u8HDMISinkEDIDValid is %d\n", pstCfdEdid->u8HDMISinkEDIDValid);

            _eCfdOutputType = E_CFD_OUTPUT_SOURCE_HDMI;

            pstKdrvCFDCtrlInfo->u16ErrCode = MHal_XC_CFD_SetEdidParam(pstCfdEdid);
            if (pstKdrvCFDCtrlInfo->u16ErrCode != E_CFD_MC_ERR_NOERR)
            {
                return FALSE;
            }

            pstCfdFire = &_stCfdFire[0];

            pstKdrvCFDCtrlInfo->u16ErrCode = MHal_XC_CFD_SetMainCtrl(pstCfdFire);
            if (pstKdrvCFDCtrlInfo->u16ErrCode != E_CFD_MC_ERR_NOERR)
            {
                return FALSE;
            }

            timer = 0;
            bTmoFireEnable = FALSE;
            while ((MHal_XC_R2BYTE(REG_SC_BK67_28_L) & 0x0001) && timer <100)
            {
                timer++;
                msleep(50);
            }
            if (timer >= 100)
            {
                printk("[%s %d] time out\n",__FUNCTION__,__LINE__);
            }

            mutex_lock(&_cfd_mutex);
            bEnable = _bEnableHDR;
            mutex_unlock(&_cfd_mutex);
            // Fire
            // THIS IS A PATCH
            if (bEnable == FALSE)
            {
                Color_Format_Driver();
                KApi_XC_MLoad_Fire(E_CLIENT_MAIN_HDR, TRUE);
                MHal_XC_FireAutoDownload(E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC);
                bTmoFireEnable = TRUE;
            }
            else
            {
                //MHal_XC_W2BYTEMSK(_PK_H_(0x7a,0x4a),0x80,0x00); //reg_b04_out_clamp_en
                MHal_XC_W2BYTEMSK(0x137A94,0,0x3FFF);//reg_b04_uv_offs_0
                bTmoFireEnable = FALSE;
            }

            pstKdrvCFDCtrlInfo->u16ErrCode = E_CFD_MC_ERR_NOERR;
            break;
        }

        case E_KDRV_XC_CFD_CTRL_SET_OSD:
        {
            MS_U8 u8Win = 0;
            MS_U16 *pu16Length = (MS_U16*)(pstKdrvCFDCtrlInfo->pParam + sizeof(MS_U32));

            MS_U16 u16CpLength = sizeof(ST_KDRV_XC_CFD_OSD);
            ST_KDRV_XC_CFD_OSD stCfdOsd;

            if ((*pu16Length) < u16CpLength)
            {
                u16CpLength = (*pu16Length) ;
            }

            memset(&stCfdOsd, 0, sizeof(ST_KDRV_XC_CFD_OSD));
            memcpy(&stCfdOsd, pstKdrvCFDCtrlInfo->pParam, u16CpLength);

            u8Win = stCfdOsd.u32Version>=2?stCfdOsd.u8Win:0;

            if (stCfdOsd.u32Version == 0)
            {
                if (stCfdOsd.u16Hue != _stCfdOsd[u8Win].u16Hue)
                {
                    bHueChange[u8Win] |= TRUE;
                }

                if (stCfdOsd.u16Saturation != _stCfdOsd[u8Win].u16Saturation)
                {
                    bSaturationChange[u8Win] |= TRUE;
                }

                if (stCfdOsd.u16Contrast != _stCfdOsd[u8Win].u16Contrast)
                {
                    bContrastChange[u8Win] |= TRUE;
                }

                _stCfdOsd[u8Win].u16Hue = stCfdOsd.u16Hue;
                _stCfdOsd[u8Win].u16Saturation = stCfdOsd.u16Saturation;
                _stCfdOsd[u8Win].u16Contrast = stCfdOsd.u16Contrast;
            }
            else if (stCfdOsd.u32Version >= 1)
            {

                if (stCfdOsd.bHueValid)
                {
                    if (stCfdOsd.u16Hue != _stCfdOsd[u8Win].u16Hue)
                    {
                        // u8Win start at version 2.
                        bHueChange[u8Win] |= TRUE;
                    }
                    _stCfdOsd[u8Win].u16Hue = stCfdOsd.u16Hue;
                }

                if (stCfdOsd.bSaturationValid)
                {
                    if (stCfdOsd.u16Saturation != _stCfdOsd[u8Win].u16Saturation)
                    {
                        // u8Win start at version 2.
                        bSaturationChange[u8Win] |= TRUE;
                    }
                    _stCfdOsd[u8Win].u16Saturation = stCfdOsd.u16Saturation;
                }

                if (stCfdOsd.bContrastValid)
                {
                    if (stCfdOsd.u16Contrast != _stCfdOsd[u8Win].u16Contrast)
                    {
                        // u8Win start at version 2.
                        bContrastChange[u8Win] |= TRUE;
                    }
                    _stCfdOsd[u8Win].u16Contrast = stCfdOsd.u16Contrast;
                }

                if (stCfdOsd.bBacklightValid)
                {
                    _stCfdOsd[u8Win].u32Backlight = stCfdOsd.u32Backlight;
                    _stCfdOsd[u8Win].u32MinBacklight = stCfdOsd.u32MinBacklight;
                    _stCfdOsd[u8Win].u32MaxBacklight = stCfdOsd.u32MaxBacklight;

                    if (_bEnableHDR == TRUE)
                    {
                        MHal_XC_UpdateDolbyPQSetting(EN_DOLBY_PQ_BACKLIGHT, &_stCfdOsd[u8Win].u32Backlight);
                    }
                }

                if ((stCfdOsd.bColorRangeValid) && ((_stCfdOsd[u8Win].u8UltraBlackLevel == 0) && (_stCfdOsd[u8Win].u8UltraWhiteLevel == 0)))
                {
                    if (stCfdOsd.u8ColorRange != _stCfdOsd[u8Win].u8ColorRange)
                    {
                        bColorRangeChange[u8Win] |= TRUE;
                    }
                    _stCfdOsd[u8Win].u8ColorRange = stCfdOsd.u8ColorRange;
                }

                if ((stCfdOsd.bUltraBlackLevelValid || stCfdOsd.bUltraWhiteLevelValid) &&
                    ((CFD_IS_HDMI(_stCfdInit[u8Win].u8InputSource)) && (_stCfdHdmi[u8Win].bIsFullRange == false) && (_stCfdOsd[u8Win].u8ColorRange == 0)))
                {
                    if (stCfdOsd.bUltraBlackLevelValid)
                    {
                        _stCfdOsd[u8Win].u8UltraBlackLevel = stCfdOsd.u8UltraBlackLevel;
                    }

                    if (stCfdOsd.bUltraWhiteLevelValid)
                    {
                        _stCfdOsd[u8Win].u8UltraWhiteLevel = stCfdOsd.u8UltraWhiteLevel;
                    }

                    Api_UltraBlackAndWhite(_stCfdOsd[u8Win].u8UltraBlackLevel, _stCfdOsd[u8Win].u8UltraWhiteLevel);
                }

                if (stCfdOsd.bSkipPictureSettingValid)
                {
                    if (stCfdOsd.bSkipPictureSetting!= _stCfdOsd[u8Win].bSkipPictureSetting)
                    {
                        bSkipPictureSettingChange[u8Win] |= TRUE;
                    }
                    _stCfdOsd[u8Win].bSkipPictureSetting = stCfdOsd.bSkipPictureSetting;
                }
            }

            XC_KDBG("  u8Win is %d\n", _stCfdOsd[u8Win].u8Win);
            XC_KDBG("  u16Hue is %d\n", _stCfdOsd[u8Win].u16Hue);
            XC_KDBG("  u16Saturation is %d\n", _stCfdOsd[u8Win].u16Saturation);
            XC_KDBG("  u16Contrast is %d\n", _stCfdOsd[u8Win].u16Contrast);
            XC_KDBG("  u32Backlight is %d\n", _stCfdOsd[u8Win].u32Backlight);
            XC_KDBG("  u32MinBacklight is %d\n", _stCfdOsd[u8Win].u32MinBacklight);
            XC_KDBG("  u32MaxBacklight is %d\n", _stCfdOsd[u8Win].u32MaxBacklight);
            XC_KDBG("  u8ColorRange is %d\n", _stCfdOsd[u8Win].u8ColorRange);
            XC_KDBG("  u8UltraBlackLevel is %d\n", _stCfdOsd[u8Win].u8UltraBlackLevel);
            XC_KDBG("  u8UltraWhiteLevel is %d\n", _stCfdOsd[u8Win].u8UltraWhiteLevel);
            XC_KDBG("  bSkipPictureSetting is %d\n", _stCfdOsd[u8Win].bSkipPictureSetting);

            if (((stCfdOsd.bColorRangeValid) && (bInitStatus[u8Win] == FALSE) && (bColorRangeChange[u8Win] == TRUE)) ||
                ((stCfdOsd.bSkipPictureSettingValid) && (bInitStatus[u8Win] == FALSE) && (bSkipPictureSettingChange[u8Win] == TRUE)))
            {
                int timer = 0;
                MS_BOOL bEnable = FALSE;
                ST_KDRV_XC_CFD_FIRE *pstCfdFire = &_stCfdFire[u8Win];

                _stCfdOsd[u8Win].u8OSDUIEn = 1;
                _stCfdOsd[u8Win].u8OSDUIMode = (bColorRangeChange[u8Win]==TRUE)?0:1;

                // Main Control
                pstKdrvCFDCtrlInfo->u16ErrCode = MHal_XC_CFD_SetMainCtrl(pstCfdFire);
                if (pstKdrvCFDCtrlInfo->u16ErrCode != E_CFD_MC_ERR_NOERR)
                {
                }

                // OSD
                pstKdrvCFDCtrlInfo->u16ErrCode = MHal_XC_CFD_SetOsdParam(&_stCfdOsd[u8Win]);
                if (pstKdrvCFDCtrlInfo->u16ErrCode != E_CFD_MC_ERR_NOERR)
                {
                    return FALSE;
                }
                Mapi_Cfd_Maserati_DLCIP_CurveMode_Set(pstCfdFire->u8Win, _stCfdDlc.bUseCustomerDlcCurve);

                bTmoFireEnable = FALSE;
                while ((MHal_XC_R2BYTE(REG_SC_BK67_28_L) & 0x0001) && timer <100)
                {
                    timer++;
                    msleep(50);
                }
                if (timer >= 100)
                {
                    printk("[%s %d] time out\n",__FUNCTION__,__LINE__);
                }
                _bAdlFired = FALSE;

                mutex_lock(&_cfd_mutex);
                bEnable = _bEnableHDR;
                mutex_unlock(&_cfd_mutex);
                // Fire
                // THIS IS A PATCH
                if (bEnable == FALSE)
                {
                    Color_Format_Driver();
                    KApi_XC_MLoad_Fire(E_CLIENT_MAIN_HDR, TRUE);
                    MHal_XC_FireAutoDownload(E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC);
                    bTmoFireEnable = TRUE;
                }
                else
                {
                    //MHal_XC_W2BYTEMSK(_PK_H_(0x7a,0x4a),0x80,0x00); //reg_b04_out_clamp_en
                    MHal_XC_W2BYTEMSK(0x137A94,0,0x3FFF);//reg_b04_uv_offs_0
                    bTmoFireEnable = FALSE;
                }

                //waiting adl done.
                if (_bCfdInited && (_stCfdMainControl.u8Input_HDRMode == E_CFIO_MODE_HDR2))
                {
                    //waiting adl fire, timeout 100ms
                    timer = 0;
                    while (_bAdlFired == FALSE)
                    {
                        timer++;
                        if (timer > 10)
                        {
                            break;
                        }
                        msleep(10);
                    }

                    if (timer >= 10)
                    {
                        printk("[%s %d]waiting adl fire time out\n", __FUNCTION__,__LINE__);
                    }

                    // waiting adl firing done, timeout 100ms
                    timer = 0;
                    while ((MHal_XC_R2BYTE(REG_SC_BK67_28_L) & 0x0001))
                    {
                        timer++;
                        if (timer > 10)
                        {
                            break;
                        }
                        msleep(10);
                    }

                    if (timer >= 10)
                    {
                        printk("[%s %d]waiting adl firing done time out\n", __FUNCTION__,__LINE__);
                    }
                    _bCfdInited = FALSE;
                }

                bColorRangeChange[u8Win] = FALSE;
                bSkipPictureSettingChange[u8Win] = FALSE;

            }

            pstKdrvCFDCtrlInfo->u16ErrCode = E_CFD_MC_ERR_NOERR;
            break;
        }

        case E_KDRV_XC_CFD_CTRL_SET_DLC:
        {
            ST_KDRV_XC_CFD_DLC *pstCfdDlc = (ST_KDRV_XC_CFD_DLC*)pstKdrvCFDCtrlInfo->pParam;
            XC_KDBG("\033[31mDLC info \033[m\n");
            XC_KDBG("\033[31m  Customer DLC is %d \033[m\n", pstCfdDlc->bUseCustomerDlcCurve);
            memcpy(&_stCfdDlc, pstCfdDlc, sizeof(ST_KDRV_XC_CFD_DLC));

            pstKdrvCFDCtrlInfo->u16ErrCode = E_CFD_MC_ERR_NOERR;
            break;
        }

        case E_KDRV_XC_CFD_CTRL_SET_LINEAR_RGB:
        {
            int timer = 0;
            MS_BOOL bEnable = 0;
            ST_KDRV_XC_CFD_LINEAR_RGB *pstCfdLinearRgb = (ST_KDRV_XC_CFD_LINEAR_RGB*)pstKdrvCFDCtrlInfo->pParam;
            XC_KDBG("\033[31mLinear RGB info \033[m\n");
            XC_KDBG("\033[31m  Window is %d \033[m\n", pstCfdLinearRgb->u8Win);
            XC_KDBG("\033[31m  Enable is %d\033[m\n", pstCfdLinearRgb->bEnable);
            if (pstCfdLinearRgb->bEnable != _stCfdLinearRgb.bEnable)
            {
                bLinearRgbChange[pstCfdLinearRgb->u8Win] |= TRUE;
            }
            memcpy(&_stCfdLinearRgb, pstCfdLinearRgb, sizeof(ST_KDRV_XC_CFD_LINEAR_RGB));

            if (bInitStatus[pstCfdLinearRgb->u8Win] == TRUE)
            {
                pstKdrvCFDCtrlInfo->u16ErrCode = E_CFD_MC_ERR_NOERR;
                break;
            }

            if (bLinearRgbChange[pstCfdLinearRgb->u8Win] == TRUE)
            {
                ST_KDRV_XC_CFD_FIRE *pstCfdFire = &_stCfdFire[pstCfdLinearRgb->u8Win];

                _stCfdOsd[pstCfdLinearRgb->u8Win].u8OSDUIEn = 1;
                _stCfdOsd[pstCfdLinearRgb->u8Win].u8OSDUIMode = 0;
                MHal_XC_CFD_SetOsdParam(&_stCfdOsd[pstCfdLinearRgb->u8Win]);

                pstKdrvCFDCtrlInfo->u16ErrCode = MHal_XC_CFD_SetMainCtrl(pstCfdFire);
                if (pstKdrvCFDCtrlInfo->u16ErrCode != E_CFD_MC_ERR_NOERR)
                {
                    return FALSE;
                }

                bTmoFireEnable = FALSE;
                while ((MHal_XC_R2BYTE(REG_SC_BK67_28_L) & 0x0001) && timer <100)
                {
                    timer++;
                    msleep(50);
                }
                if (timer >= 100)
                {
                    printk("[%s %d] time out\n",__FUNCTION__,__LINE__);
                }

                mutex_lock(&_cfd_mutex);
                bEnable = _bEnableHDR;
                mutex_unlock(&_cfd_mutex);

                // Fire
                // THIS IS A PATCH
                if (_bEnableHDR == FALSE)
                {
                    Color_Format_Driver();
                    KApi_XC_MLoad_Fire(E_CLIENT_MAIN_HDR, TRUE);
                    MHal_XC_FireAutoDownload(E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC);
                    bTmoFireEnable = TRUE;
                }
                else
                {
                    //MHal_XC_W2BYTEMSK(_PK_H_(0x7a,0x4a),0x80,0x00); //reg_b04_out_clamp_en
                    MHal_XC_W2BYTEMSK(0x137A94,0,0x3FFF);//reg_b04_uv_offs_0
                    bTmoFireEnable = FALSE;
                }
            }
            bLinearRgbChange[pstCfdLinearRgb->u8Win] = FALSE;

            pstKdrvCFDCtrlInfo->u16ErrCode = E_CFD_MC_ERR_NOERR;
            break;
        }

        case E_KDRV_XC_CFD_CTRL_GET_STATUS:
        {
            ST_KDRV_XC_CFD_STATUS *pstCfdStatus = (ST_KDRV_XC_CFD_STATUS*)pstKdrvCFDCtrlInfo->pParam;
            if (_bEnableHDR)
            {
                pstCfdStatus->u8VideoHdrType = E_KDRV_XC_CFD_HDR_TYPE_DOLBY;
            }
            else
            {
                if (CFD_IS_MM(_stCfdInit[pstCfdStatus->u8Win].u8InputSource))
                {
                    ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info stFormatInfo;
                    MHal_XC_CFD_WithdrawMMParam(&stFormatInfo, pstCfdStatus->u8Win);
                    if ((stFormatInfo.HDRMemFormat.stHDRMemFormatCFD.bVUIValid == TRUE) && (stFormatInfo.HDRMemFormat.stHDRMemFormatCFD.u8Transfer_Characteristics == 16))
                    {
                        // MM Open HDR
                        pstCfdStatus->u8VideoHdrType = E_KDRV_XC_CFD_HDR_TYPE_OPEN;

                        if(pstCfdStatus->u32Version >= 3)
                        {
                            // get MM open HDR metadata
                            memcpy(&pstCfdStatus->HDRMetadata.stHDRMemFormatCFD, &stFormatInfo.HDRMemFormat.stHDRMemFormatCFD, sizeof(ST_KDRV_XC_HDR_CFD_MEMORY_FORMAT));
                        }
                    }
                    else if ((stFormatInfo.HDRMemFormat.stHDRMemFormatCFD.bVUIValid == TRUE) && (stFormatInfo.HDRMemFormat.stHDRMemFormatCFD.u8Transfer_Characteristics == 18))
                    {
                        // MM HLG
                        pstCfdStatus->u8VideoHdrType = E_KDRV_XC_CFD_HDR_TYPE_HLG;
                    }
                    else
                    {
                        // SDR
                        pstCfdStatus->u8VideoHdrType = E_KDRV_XC_CFD_HDR_TYPE_NONE;
                    }
                }
                else if (CFD_IS_HDMI(_stCfdInit[pstCfdStatus->u8Win].u8InputSource) || CFD_IS_DTV(_stCfdInit[pstCfdStatus->u8Win].u8InputSource))
                {
                    if ((_stCfdHdmi[pstCfdStatus->u8Win].bHDRInfoFrameValid == TRUE) && (_stCfdHdmi[pstCfdStatus->u8Win].u8EOTF == 2))
                    {
                        // HDMI Open HDR
                        pstCfdStatus->u8VideoHdrType = E_KDRV_XC_CFD_HDR_TYPE_OPEN;

                        if(pstCfdStatus->u32Version >= 3)
                        {
                            memcpy(&pstCfdStatus->HDRMetadata.stHdmiInfoCFD, &_stCfdHdmi[pstCfdStatus->u8Win], sizeof(ST_KDRV_XC_CFD_HDMI));
                        }
                    }
                    else if ((_stCfdHdmi[pstCfdStatus->u8Win].bHDRInfoFrameValid == TRUE) && (_stCfdHdmi[pstCfdStatus->u8Win].u8EOTF == 3))
                    {
                        // HDMI HLG
                        pstCfdStatus->u8VideoHdrType = E_KDRV_XC_CFD_HDR_TYPE_HLG;
                    }
                    else
                    {
                        // SDR
                        pstCfdStatus->u8VideoHdrType = E_KDRV_XC_CFD_HDR_TYPE_NONE;
                    }
                }
                else
                {
                    // SDR
                    pstCfdStatus->u8VideoHdrType = E_KDRV_XC_CFD_HDR_TYPE_NONE;
                }
            }

            if ((pstCfdStatus->u8VideoHdrType == E_KDRV_XC_CFD_HDR_TYPE_DOLBY) && (_stCfdHdr[pstCfdStatus->u8Win].u8HdrType & E_KDRV_XC_CFD_HDR_TYPE_DOLBY))
            {
                // Dolby HDR running
                pstCfdStatus->bIsHdrRunning = TRUE;
            }
            else if ((pstCfdStatus->u8VideoHdrType == E_KDRV_XC_CFD_HDR_TYPE_OPEN) && (_stCfdHdr[pstCfdStatus->u8Win].u8HdrType & E_KDRV_XC_CFD_HDR_TYPE_OPEN))
            {
                // Open HDR running
                pstCfdStatus->bIsHdrRunning = TRUE;
            }
            else if ((pstCfdStatus->u8VideoHdrType == E_KDRV_XC_CFD_HDR_TYPE_HLG) && (_stCfdHdr[pstCfdStatus->u8Win].u8HdrType & E_KDRV_XC_CFD_HDR_TYPE_HLG))
            {
                // HLG running
                pstCfdStatus->bIsHdrRunning = TRUE;
            }
            else
            {
                pstCfdStatus->bIsHdrRunning = FALSE;
            }

            if (pstCfdStatus->u32Version >= 1)
            {
                if (CFD_IS_HDMI(_stCfdInit[pstCfdStatus->u8Win].u8InputSource))
                {
                    // HDMI
                    pstCfdStatus->bIsFullRange = _stCfdHdmi[pstCfdStatus->u8Win].bIsFullRange;
                }
                else if (CFD_IS_MM(_stCfdInit[pstCfdStatus->u8Win].u8InputSource) || CFD_IS_DTV(_stCfdInit[pstCfdStatus->u8Win].u8InputSource))
                {
                    // MM/DTV
                    if ((IS_HDMI_DOLBY) || (IS_OTT_DOLBY))
                    {
                        pstCfdStatus->bIsFullRange = 0;
                    }
                    else
                    {
                        ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info stFormatInfo;
                        MHal_XC_CFD_WithdrawMMParam(&stFormatInfo, pstCfdStatus->u8Win);
                        pstCfdStatus->bIsFullRange = stFormatInfo.HDRMemFormat.stHDRMemFormatCFD.u8Video_Full_Range_Flag;
                        if(MHal_XC_ShareMemIsAvailable(&stFormatInfo))
                        {
                            pstCfdStatus->bIsFullRange = stFormatInfo.HDRMemFormat.stHDRMemFormatCFD.u8Video_Full_Range_Flag;
                        }
                        else
                        {
                            pstCfdStatus->bIsFullRange = _stCfdAnalog[pstCfdStatus->u8Win].bIsFullRange;
                        }
                    }
                }
                else
                {
                    // Analog
                    pstCfdStatus->bIsFullRange = _stCfdAnalog[pstCfdStatus->u8Win].bIsFullRange;
                }

                if (_stCfdOsd[pstCfdStatus->u8Win].u8UltraBlackLevel != 0 || _stCfdOsd[pstCfdStatus->u8Win].u8UltraWhiteLevel != 0)
                {
                    pstCfdStatus->bUltraBlackWhiteActive = true;
                }
                else
                {
                    pstCfdStatus->bUltraBlackWhiteActive = false;
                }
            }
            pstKdrvCFDCtrlInfo->u16ErrCode = E_CFD_MC_ERR_NOERR;

            break;
        }

        case E_KDRV_XC_CFD_CTRL_SET_TMO:
        {
            MS_U8 *pu8Data = NULL;
            MS_U8 i=0;
            MS_U16 u16CpLength = sizeof(ST_KDRV_XC_CFD_TMO);
            MS_U16 *pu16Length = (MS_U16*)(pstKdrvCFDCtrlInfo->pParam + sizeof(MS_U32));
            ST_KDRV_XC_CFD_TMO *pstKdrvCFDTmo = (ST_KDRV_XC_CFD_TMO*)pstKdrvCFDCtrlInfo->pParam;
            ST_KDRV_XC_CFD_TMO_LEVEL *pstKdrvTmoLevel = NULL;

            if(pstKdrvCFDTmo->u32Version == 0)
            {
                printk("[%s %d]ERROR MI/SN vsrsion is old, please merge corresponding MI/SN code\n", __FUNCTION__,__LINE__);
                break;
            }

            if(NULL != _stCfdTmo.pstCfdTmoLevel)
            {
                for(i=0; i<_stCfdTmo.u16LevelCount; i++)
                {
                    if(NULL != (_stCfdTmo.pstCfdTmoLevel + i)->pu8data)
                    {
                        vfree((_stCfdTmo.pstCfdTmoLevel + i)->pu8data);
                    }
                }
                vfree(_stCfdTmo.pstCfdTmoLevel);
            }

            if ((*pu16Length) < u16CpLength)
            {
                u16CpLength = (*pu16Length) ;
            }
            memset(&_stCfdTmo, 0, sizeof(ST_KDRV_XC_CFD_TMO));
            memcpy(&_stCfdTmo, pstKdrvCFDTmo, u16CpLength);

            if((0 == pstKdrvCFDTmo->u16LevelCount) || (NULL == pstKdrvCFDTmo->pstCfdTmoLevel))
            {
                printk("[%s %d]ERROR input TMO parameter\n",__FUNCTION__,__LINE__);
                return;
            }
            pu8Data = vmalloc(pstKdrvCFDTmo->u16LevelCount * sizeof(ST_KDRV_XC_CFD_TMO_LEVEL));
            if(NULL == pu8Data)
            {
                printk("[%s %d]Error malloc fail\n", __FUNCTION__,__LINE__);
                return;
            }

            memset(pu8Data, 0, pstKdrvCFDTmo->u16LevelCount * sizeof(ST_KDRV_XC_CFD_TMO_LEVEL));
            copy_from_user(pu8Data, (MS_U8 __user *)pstKdrvCFDTmo->pstCfdTmoLevel, pstKdrvCFDTmo->u16LevelCount * sizeof(ST_KDRV_XC_CFD_TMO_LEVEL));
            _stCfdTmo.pstCfdTmoLevel = pu8Data;

            for(i=0; i<_stCfdTmo.u16LevelCount; i++)
            {
                if(0 == (_stCfdTmo.pstCfdTmoLevel + i)->u16ControlSize)
                {
                    printk("[%s %d]ERROR Level %d controlsize is error\n", __FUNCTION__,__LINE__, i);
                    continue;
                }

                pu8Data = vmalloc((_stCfdTmo.pstCfdTmoLevel + i)->u16ControlSize);
                copy_from_user(pu8Data, (MS_U8 __user *)(_stCfdTmo.pstCfdTmoLevel + i)->pu8data, (_stCfdTmo.pstCfdTmoLevel + i)->u16ControlSize);
                (_stCfdTmo.pstCfdTmoLevel + i)->pu8data = pu8Data;
            }
        }
        break;

        case E_KDRV_XC_CFD_CTRL_SET_FIRE:
        {
            MS_BOOL bEnable = 0;
            MS_BOOL bFireChange = FALSE;
            ST_KDRV_XC_CFD_FIRE *pstCfdFire = (ST_KDRV_XC_CFD_FIRE*)pstKdrvCFDCtrlInfo->pParam;
            XC_KDBG("\033[31mFire info \033[m\n");
            XC_KDBG("\033[31m  Window is %d \033[m\n", pstCfdFire->u8Win);
            XC_KDBG("\033[31m  Input source is %d \033[m\n", pstCfdFire->u8InputSource);
            XC_KDBG("\033[31m  Is RGB bypass is %d \033[m\n", pstCfdFire->bIsRgbBypass);
            XC_KDBG("\033[31m  Is HD mode is %d \033[m\n", pstCfdFire->bIsHdMode);
            XC_KDBG("\033[31m  Update type is %d \033[m\n", pstCfdFire->u8UpdateType);

            if ((pstCfdFire->u8UpdateType == 1) && (bInitStatus[pstCfdFire->u8Win] == TRUE))
            {
                // In init status, skip OSD update action
                pstKdrvCFDCtrlInfo->u16ErrCode = E_CFD_MC_ERR_NOERR;
                XC_KDBG("\033[31m  In init status, skip OSD update action \033[m\n");
                break;
            }

            if ((pstCfdFire->u8UpdateType == 1) &&
                (bHueChange[pstCfdFire->u8Win] == FALSE) &&
                (bContrastChange[pstCfdFire->u8Win] == FALSE) &&
                (bSaturationChange[pstCfdFire->u8Win] == FALSE))
            {
                // OSD doesn't change
                pstKdrvCFDCtrlInfo->u16ErrCode = E_CFD_MC_ERR_NOERR;
                XC_KDBG("\033[31m Hue / Contrast / Saturation doesn't change, skip CFD fire \033[m\n");
                break;
            }

            if (pstCfdFire->u8UpdateType == 1)
            {
                pstCfdFire = &_stCfdFire[pstCfdFire->u8Win];
                pstCfdFire->u8UpdateType = 1;
                _stCfdOsd[pstCfdFire->u8Win].u8OSDUIEn = 1;
                _stCfdOsd[pstCfdFire->u8Win].u8OSDUIMode = 1;
            }
            else
            {
                if (_stCfdFire[pstCfdFire->u8Win].u8InputSource != pstCfdFire->u8InputSource)
                {
                    bFireChange |= TRUE;
                }

                if (_stCfdFire[pstCfdFire->u8Win].bIsRgbBypass!= pstCfdFire->bIsRgbBypass)
                {
                    bFireChange |= TRUE;
                }

                if (_stCfdFire[pstCfdFire->u8Win].bIsHdMode!= pstCfdFire->bIsHdMode)
                {
                    bFireChange |= TRUE;
                }

                memcpy(&_stCfdFire[pstCfdFire->u8Win], pstCfdFire, sizeof(ST_KDRV_XC_CFD_FIRE));
                _stCfdOsd[pstCfdFire->u8Win].u8OSDUIEn = 1;
                _stCfdOsd[pstCfdFire->u8Win].u8OSDUIMode = 0;
            }

            // Init IP
            pstKdrvCFDCtrlInfo->u16ErrCode = MHal_XC_CFD_InitIp(pstCfdFire);
            if (pstKdrvCFDCtrlInfo->u16ErrCode != E_CFD_MC_ERR_NOERR)
            {
                return FALSE;
            }

            // Main Control
            pstKdrvCFDCtrlInfo->u16ErrCode = MHal_XC_CFD_SetMainCtrl(pstCfdFire);
            if (pstKdrvCFDCtrlInfo->u16ErrCode != E_CFD_MC_ERR_NOERR)
            {
                return FALSE;
            }

            // MM/DTV
            if (CFD_IS_MM(pstCfdFire->u8InputSource) || CFD_IS_DTV(pstCfdFire->u8InputSource))
            {
                pstKdrvCFDCtrlInfo->u16ErrCode = MHal_XC_CFD_SetMmParam(pstCfdFire, &bMmChange[pstCfdFire->u8Win]);
                if (pstKdrvCFDCtrlInfo->u16ErrCode != E_CFD_MC_ERR_NOERR)
                {
                    //return FALSE;
                }
            }

            // HDMI
            if (CFD_IS_HDMI(pstCfdFire->u8InputSource))
            {
                pstKdrvCFDCtrlInfo->u16ErrCode = MHal_XC_CFD_SetHdmiParam(pstCfdFire);
                if (pstKdrvCFDCtrlInfo->u16ErrCode != E_CFD_MC_ERR_NOERR)
                {
                    //return FALSE;
                }
            }

            // HDR
            pstKdrvCFDCtrlInfo->u16ErrCode = MHal_XC_CFD_SetHdrParam(pstCfdFire);
            if (pstKdrvCFDCtrlInfo->u16ErrCode != E_CFD_MC_ERR_NOERR)
            {
                //return FALSE;
            }

            // OSD
            pstKdrvCFDCtrlInfo->u16ErrCode = MHal_XC_CFD_SetOsdParam(&_stCfdOsd[pstCfdFire->u8Win]);
            if (pstKdrvCFDCtrlInfo->u16ErrCode != E_CFD_MC_ERR_NOERR)
            {
                return FALSE;
            }
            Mapi_Cfd_Maserati_DLCIP_CurveMode_Set(pstCfdFire->u8Win, _stCfdDlc.bUseCustomerDlcCurve);

            if (bHueChange[pstCfdFire->u8Win] || bContrastChange[pstCfdFire->u8Win] || bSaturationChange[pstCfdFire->u8Win] || bLinearRgbChange[pstCfdFire->u8Win] ||
                bHdrChange[pstCfdFire->u8Win] || bHdmiChange[pstCfdFire->u8Win] || bMmChange[pstCfdFire->u8Win] || bAnalogChange[pstCfdFire->u8Win] || bFireChange)
            {
                int timer = 0;
                bTmoFireEnable = FALSE;
                while ((MHal_XC_R2BYTE(REG_SC_BK67_28_L) & 0x0001) && timer <100)
                {
                    timer++;
                    msleep(50);
                }
                if (timer >= 100)
                {
                    printk("[%s %d] time out\n",__FUNCTION__,__LINE__);
                }

                _bAdlFired = FALSE;

                mutex_lock(&_cfd_mutex);
                bEnable = _bEnableHDR;
                mutex_unlock(&_cfd_mutex);

                // Fire
                // THIS IS A PATCH
                if (_bEnableHDR == FALSE)
                {
                    Color_Format_Driver();
                    KApi_XC_MLoad_Fire(E_CLIENT_MAIN_HDR, TRUE);
                    MHal_XC_FireAutoDownload(E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC);
                    if(CFD_IS_HDMI(u8HDMI_case)  & (pstCfdFire->u8UpdateType == 0))
                    {
                        MHal_XC_FireAutoDownload(E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR);
                    }
                    bTmoFireEnable = TRUE;
                    _enHDRType = E_KDRV_XC_HDR_NONE;
                }
                else
                {
                    //MHal_XC_W2BYTEMSK(_PK_H_(0x7a,0x4a),0x80,0x00); //reg_b04_out_clamp_en
                    MHal_XC_W2BYTEMSK(0x137A94,0,0x3FFF);//reg_b04_uv_offs_0
                    _MHal_XC_DolbyPatch();
                    bTmoFireEnable = FALSE;
                }

                //waiting adl done.
                if (_bCfdInited && (_stCfdMainControl.u8Input_HDRMode == E_CFIO_MODE_HDR2))
                {
                    //waiting adl fire, timeout 100ms
                    timer = 0;
                    while (_bAdlFired == FALSE)
                    {
                        timer++;
                        if (timer > 10)
                        {
                            break;
                        }
                        msleep(10);
                    }

                    if (timer >= 10)
                    {
                        printk("[%s %d]waiting adl fire time out\n", __FUNCTION__,__LINE__);
                    }

                    // waiting adl firing done, timeout 100ms
                    timer = 0;
                    while ((MHal_XC_R2BYTE(REG_SC_BK67_28_L) & 0x0001))
                    {
                        timer++;
                        if (timer > 10)
                        {
                            break;
                        }
                        msleep(10);
                    }

                    if (timer >= 10)
                    {
                        printk("[%s %d]waiting adl firing done time out\n", __FUNCTION__,__LINE__);
                    }
                    _bCfdInited = FALSE;
                }
            }
            bInitStatus[pstCfdFire->u8Win] = FALSE;
            bHueChange[pstCfdFire->u8Win] = FALSE;
            bContrastChange[pstCfdFire->u8Win] = FALSE;
            bSaturationChange[pstCfdFire->u8Win] = FALSE;
            bColorRangeChange[pstCfdFire->u8Win] = FALSE;
            bLinearRgbChange[pstCfdFire->u8Win] = FALSE;
            bHdrChange[pstCfdFire->u8Win] = FALSE;
            bHdmiChange[pstCfdFire->u8Win] = FALSE;
            bMmChange[pstCfdFire->u8Win] = FALSE;
            bAnalogChange[pstCfdFire->u8Win] = FALSE;
            bSkipPictureSettingChange[pstCfdFire->u8Win] = FALSE;
            pstKdrvCFDCtrlInfo->u16ErrCode = E_CFD_MC_ERR_NOERR;

            break;
        }

        default:
            break;
    }
#undef XC_KDBG

    return TRUE;
}

MS_BOOL MHal_XC_ShareMemIsAvailable(ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info *pstFormatInfo)
{
    ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info tmp;
    memset(&tmp,0,sizeof(ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info));
    tmp.u8Version = 0xFF;
    if(memcmp(&tmp, pstFormatInfo, sizeof(ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info))==0)
    {
        return FALSE;
    }
    return TRUE;
}

MS_BOOL MHal_XC_CheckMuteStatusByRegister(MS_U8 u8Window)
{
    MS_BOOL bRet = FALSE;
    ///TODO: check need consider device 1
#if (1)//PIP_PATCH_USING_SC1_MAIN_AS_SC0_SUB)
    if (SUB_WINDOW == u8Window)
    {
        bRet = (MHal_XC_R2BYTEMSK(REG_SC_BK10_19_L, BIT(5)) ? TRUE: FALSE);
    }

#else
    if(SUB_WINDOW == eWindow)
    {
        bRet = (SC_R2BYTEMSK(psXCInstPri->u32DeviceID, REG_SC_BK10_19_L, BIT(5)) ? TRUE: FALSE);
    }
#endif
    else if(MAIN_WINDOW == u8Window)
    {
        bRet = (MHal_XC_R2BYTEMSK(REG_SC_BK10_19_L, BIT(1)) ? TRUE: FALSE);
    }
    return bRet;
}

void MHal_XC_Suspend(void)
{
    MS_U8 u8RegIndex = 0;

    _au16DmaRegValue[u8RegIndex++] = MHal_XC_R2BYTE(REG_SC_BK42_08_L);
    _au16DmaRegValue[u8RegIndex++] = MHal_XC_R2BYTE(REG_SC_BK42_09_L);
    _au16DmaRegValue[u8RegIndex++] = MHal_XC_R2BYTE(REG_SC_BK42_10_L);
    _au16DmaRegValue[u8RegIndex++] = MHal_XC_R2BYTE(REG_SC_BK42_11_L);
    _au16DmaRegValue[u8RegIndex++] = MHal_XC_R2BYTE(REG_SC_BK42_05_L);
    _au16DmaRegValue[u8RegIndex++] = MHal_XC_R2BYTE(REG_SC_BK42_50_L);
    _au16DmaRegValue[u8RegIndex++] = MHal_XC_R2BYTE(REG_SC_BK42_52_L);
    _au16DmaRegValue[u8RegIndex++] = MHal_XC_R2BYTE(REG_SC_BK42_53_L);
    _au16DmaRegValue[u8RegIndex++] = MHal_XC_R2BYTE(REG_SC_BK42_54_L);
    _au16DmaRegValue[u8RegIndex++] = MHal_XC_R2BYTE(REG_SC_BK42_55_L);
    if (u8RegIndex != DMA_STR_PROTECT_REGISTER_NUMBER)
    {
        printk("\033[31m[%s %d]Warning:DMA need protect %d registers, actually protect %d\033[m\n",__FUNCTION__,__LINE__,DMA_STR_PROTECT_REGISTER_NUMBER,u8RegIndex);
    }

    // DC off when MM playing dolby video, it shows MM UI after STR resume
    _ISMMDolby = FALSE;

    // dolbyHDR HDMI -> DC OFF/ON -> SDR ( purple, cause sdr does not run cfd )
    _ISHDMIDolby = FALSE;

    // store status for future STR
    _bEnableHDRCLK_for_STR = _bEnableHDRCLK;
}

void MHal_XC_Resume(void)
{
    MHal_XC_Init();
    ///HDR----
    MHal_XC_InitHDR();
    MHal_XC_EnableHDR(ENABLE);
    if (_bEnableHDRCLK_for_STR == ENABLE)
    {
        MHal_XC_EnableHDRCLK(TRUE,TRUE);
    }
    //----

    ///hdr auto download----
    ///TODO:need refine: xvYcc need  resume if it use autodownload in the future.
    //set baseaddr
    MHal_XC_W2BYTE(REG_SC_BK67_29_L, (_stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].phyBaseAddr / BYTE_PER_WORD) & 0x0000FFFF);
    MHal_XC_W2BYTEMSK(REG_SC_BK67_2A_L, ((_stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].phyBaseAddr / BYTE_PER_WORD) >> 16 & 0x0000FFFF), 0x07FF);
    if (_stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].u32MiuNo == 1)
    {
        MHal_XC_W2BYTEMSK(REG_SC_BK7F_10_L, 0x1000, 0x1000);
    }
    //set work mode
    _Hal_XC_Set_Auto_Download_WorkMode(REG_SC_BK67_28_L, 1, _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].enMode);
    //----

    ///DMA----
    {
        MS_U8 u8RegIndex = 0;
        // set IPM/OPM addr
        MHal_XC_W2BYTE(REG_SC_BK42_08_L, _au16DmaRegValue[u8RegIndex++]);
        MHal_XC_W2BYTEMSK(REG_SC_BK42_09_L, _au16DmaRegValue[u8RegIndex++], 0x01FF);
        MHal_XC_W2BYTE(REG_SC_BK42_10_L, _au16DmaRegValue[u8RegIndex++]);
        MHal_XC_W2BYTEMSK(REG_SC_BK42_11_L, _au16DmaRegValue[u8RegIndex++], 0x01FF);
        // set miu
        MHal_XC_W2BYTEMSK(REG_SC_BK42_05_L, _au16DmaRegValue[u8RegIndex++], 0x0030);
        // set limit addr
        // enable limit
        MHal_XC_W2BYTEMSK(REG_SC_BK42_50_L, _au16DmaRegValue[u8RegIndex++], 0x0001);
        // min addr
        MHal_XC_W2BYTE(REG_SC_BK42_52_L, _au16DmaRegValue[u8RegIndex++]);
        MHal_XC_W2BYTEMSK(REG_SC_BK42_53_L, _au16DmaRegValue[u8RegIndex++], 0x01FF);
        // max addr
        MHal_XC_W2BYTE(REG_SC_BK42_54_L, _au16DmaRegValue[u8RegIndex++]);
        MHal_XC_W2BYTEMSK(REG_SC_BK42_55_L, _au16DmaRegValue[u8RegIndex++], 0x01FF);
        if (u8RegIndex != DMA_STR_PROTECT_REGISTER_NUMBER)
        {
            printk("\033[31m[%s %d]Warning:DMA need restore %d registers, actually restore %d\033[m\n",__FUNCTION__,__LINE__,DMA_STR_PROTECT_REGISTER_NUMBER,u8RegIndex);
        }
    }
    //----
}

void MHal_XC_Init(void)
{
    // Do nothing
}
// dwsrc_addr , dwdes_addr must be physical address
// bymode = [7:4] = sel des MIU
//          [3:0] = sel src MIU
static void _bdma(MS_U32 dwsrc_addr, MS_U32 dwdes_addr, MS_U32 dwsize, MS_U8 bymode)
{
    MS_U16 temp;
    if (((bymode & 0xf0)>>4) == 0)
    {
        temp = *(volatile MS_U16 *)(mstar_pm_base + (0x100924 << 1));
        temp &= ~0xFF00;
        temp |= 0x4000;
        *(volatile MS_U16 *)(mstar_pm_base + (0x100924 << 1)) = temp;
    }
    else
    {
        temp = *(volatile MS_U16 *)(mstar_pm_base + (0x100924 << 1));
        temp &= ~0xFF00;
        temp |= 0x4100;
        *(volatile MS_U16 *)(mstar_pm_base + (0x100924 << 1)) = temp;
    }
    if ((bymode & 0x0f) == 0)
    {
        temp = *(volatile MS_U16 *)(mstar_pm_base + (0x100924 << 1));
        temp &= ~0x00ff;
        temp |= 0x0040;
        *(volatile MS_U16 *)(mstar_pm_base + (0x100924 << 1)) = temp;
    }
    else
    {
        temp = *(volatile MS_U16 *)(mstar_pm_base + (0x100924 << 1));
        temp &= ~0x00ff;
        temp |= 0x0041;
        *(volatile MS_U16 *)(mstar_pm_base + (0x100924 << 1)) = temp;
    }

    // destination address
    *(volatile MS_U16 *)(mstar_pm_base + (0x10092c << 1)) = dwdes_addr & 0xffff;
    *(volatile MS_U16 *)(mstar_pm_base + (0x10092e << 1)) = (dwdes_addr >> 16) & 0xffff;

    // source address
    *(volatile MS_U16 *)(mstar_pm_base + (0x100928 << 1)) = dwsrc_addr& 0xffff;
    *(volatile MS_U16 *)(mstar_pm_base + (0x10092a << 1)) = (dwsrc_addr >> 16) & 0xffff;

    // size
    *(volatile MS_U16 *)(mstar_pm_base + (0x100930 << 1)) = dwsize & 0xffff;
    *(volatile MS_U16 *)(mstar_pm_base + (0x100932 << 1)) = (dwsize >> 16) & 0xffff;

    // trigger
    *(volatile MS_U16 *)(mstar_pm_base + (0x100920 << 1)) = 1;

    // make sure it is finished.
    while ( *(volatile MS_U16 *) (mstar_pm_base + (0x100920 << 1)) != 0  );
}

MS_PHY _GETPA_WITHOUT_BASE(phys_addr_t phy, MS_U8 * u8MIU_No)
{
    phys_addr_t tmp_phy = phy;

    if(phy<ARM_MIU1_BASE_ADDR)
    {
        *u8MIU_No = 0;
    }
    else if((phy >= ARM_MIU1_BASE_ADDR) && (phy<ARM_MIU2_BASE_ADDR))
    {
        tmp_phy = tmp_phy - ARM_MIU1_BASE_ADDR;
        *u8MIU_No = 1;
    }
    else
    {
        tmp_phy = tmp_phy - ARM_MIU2_BASE_ADDR;
        *u8MIU_No = 2;
    }

    return tmp_phy;
}

void MHal_XC_MemCpy_by_BDMA(MS_U32 u32src_addr, MS_U32 u32dest_addr, MS_U32 u32Size)
{
    MS_U8 src_MIU_NO = 0;
    MS_U8 dest_MIU_NO = 0;
    MS_U8 u8MIU_Sel = 0;
    MS_U32 src_addr_without_base = 0;
    MS_U32 dest_addr_without_base = 0;
    ST_KDRV_XC_SHARE_MEMORY_INFO stShmemInfo;
    MDrv_XC_GetShareMemInfo(&stShmemInfo);

    src_addr_without_base = u32src_addr;
    src_MIU_NO = stShmemInfo.u32MiuNo;
    dest_addr_without_base = _GETPA_WITHOUT_BASE(u32dest_addr, &dest_MIU_NO);

    u8MIU_Sel = ((src_MIU_NO & 0x0F) | ((dest_MIU_NO&0x0F) << 4));

    _bdma(src_addr_without_base,
          dest_addr_without_base,
          u32Size,
          u8MIU_Sel);
}
#endif
#endif
