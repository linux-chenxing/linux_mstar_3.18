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
#ifdef CONFIG_MSTAR_XC_HDR_SUPPORT
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/poll.h>
#include <linux/wait.h>
#if defined(CONFIG_MIPS)
#elif defined(CONFIG_ARM) || defined(CONFIG_ARM64)
#include <asm/io.h>
#endif
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/version.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/jiffies.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>
#include <linux/kthread.h>

#include "mdrv_mstypes.h"
#include "mdrv_xc_st.h"
#include "mhal_dlc.h"
#include "mhal_xc.h"
#include "chip_int.h"
#include "mstar/mstar_chip.h"
#include "dolby_vision_driver.h"

extern MS_BOOL KApi_XC_MLoad_Fire(EN_MLOAD_CLIENT_TYPE _client_type, MS_BOOL bImmeidate);
extern MS_BOOL _bEnableHDR; // this is patch , needs to be removed after Easter.Jhan fixes issue.
extern struct mutex _cfd_mutex;
extern EN_KDRV_XC_HDR_TYPE _enHDRType;
extern EN_KDRV_XC_INPUT_SOURCE_TYPE _enInputSourceType;

extern StuDlc_HDRinit g_HDRinitParameters;


MS_U8 u8Mhal_xc_TMO_mode;

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
static MS_BOOL bFireing3DLut = FALSE;
extern MS_BOOL KApi_XC_MLoad_WriteCmd(EN_MLOAD_CLIENT_TYPE _client_type,MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask);
extern MS_BOOL KApi_XC_MLoad_Fire(EN_MLOAD_CLIENT_TYPE _client_type, MS_BOOL bImmeidate);

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define K6_AUTO_DOWNLOADHDR_GAMMA_PATCH
#define DEBUG_HDR 0
#define  AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR     512
#define  AUTO_DOWNLOAD_HDR_3DLUT_SRAM_MAX_ADDR     4944
#define DMA_STR_PROTECT_REGISTER_NUMBER 10
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

static MS_U32 _au32_3dlut_entry_num[8] = {736, 656, 656, 576, 656, 576, 576, 512};
MS_BOOL bTmoFireEnable;
MS_BOOL _bEnableHDRCLK;
MS_BOOL _bAdlFired = FALSE;
MS_BOOL _bTimingChanged = FALSE;
extern MS_U32 _u32NoSignalCount;
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

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static DoVi_Config_t* _pstDmConfig;
static DoVi_Comp_ExtConfig_t* _pstCompConfExt;
static MsHdr_RegTable_t* _pstDmRegTable;
static MsHdr_Comp_Regtable_t* _pstCompRegTable;
static ST_KDRV_XC_AUTODOWNLOAD_CLIENT_INFO _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_MAX];
static EN_KDRV_XC_AUTO_DOWNLOAD_STATUS _enAutoDownloadStatus[E_KDRV_XC_AUTODOWNLOAD_CLIENT_MAX];
static MS_U8 *pu8AutoDownloadDRAMBaseAddr = NULL;
static MS_BOOL _bUnmapAutoDownloadDRAM = FALSE;
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

ST_KDRV_XC_CFD_HDMI _stCfdHdmi_Out[2] =
{
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {0, 0, 0}, {0, 0, 0}, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {0, 0, 0}, {0, 0, 0}, 0, 0, 0, 0, 0, 0},
};

ST_KDRV_XC_CFD_ANALOG _stCfdAnalog[2] =
{
    {0, 0, 0, 0, 0, 0, 2, 2, 2},
    {0, 0, 0, 0, 0, 0, 2, 2, 2}
};


ST_KDRV_XC_CFD_HDR _stCfdHdr[2] =
{
    {0, 0, 0, 0},
    {0, 0, 1, 0},
};

ST_KDRV_XC_CFD_OSD _stCfdOsd = {0, 0, 0, 0, 0, 0, 0, 0};

ST_KDRV_XC_CFD_LINEAR_RGB _stCfdLinearRgb = {0, 0, 0, 0};

ST_KDRV_XC_CFD_FIRE _stCfdFire[2] = {{0, 0, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 0}};

E_CFD_OUTPUT_SOURCE _eCfdOutputType = E_CFD_OUTPUT_SOURCE_HDMI;

static ST_KDRV_XC_CFD_DLC _stCfdDlc = {0, 0, FALSE};
static ST_KDRV_XC_AUTODOWNLOAD_CLIENT_INFO _stHDRClientInfo;
static MS_U8 *pu8HDRAutoDownloadDRAMBaseAddr = NULL;
static MS_BOOL _bUnmapHDRAutoDownloadDRAM = FALSE;
static MS_U16 _au16DmaRegValue[DMA_STR_PROTECT_REGISTER_NUMBER]={0};
static MS_S16 _u16DolbySupportStatus = -1;
static MS_BOOL _bCfdInited = FALSE;
static STU_CFDAPI_MAIN_CONTROL _stCfdMainControl;
#else
#include <linux/kernel.h>
#include "mdrv_mstypes.h"
#include "mhal_xc.h"
#endif
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
        return -1;
    }

    pstMMParam->u32Version                            = CFD_MM_ST_VERSION;
    pstMMParam->u16Length                             = sizeof(STU_CFDAPI_MM_PARSER);
    pstMMParam->u8Video_Full_Range_Flag    = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u8Video_Full_Range_Flag;

    if (pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.bVUIValid == FALSE)
    {
        // Algorithm team's suggestion, when VUI is not valid, please assign them the value of following
        if(pstCfdFire->bIsHdMode == TRUE)
        {
            // HD default value
            pstMMParam->u8Colour_primaries          = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
            pstMMParam->u8Transfer_Characteristics  = E_CFD_CFIO_TR_BT709;
            pstMMParam->u8Matrix_Coeffs             = E_CFD_CFIO_MC_BT709_XVYCC709;
        }
        else
        {
            // SD default value
            pstMMParam->u8Colour_primaries          = E_CFD_CFIO_CP_BT601625;
            pstMMParam->u8Transfer_Characteristics  = E_CFD_CFIO_TR_BT601525_601625;
            pstMMParam->u8Matrix_Coeffs             = E_CFD_CFIO_MC_BT601625_XVYCC601_SYCC;
        }
    }
    else
    {
        pstMMParam->u8Colour_primaries            = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u8Colour_primaries;
        pstMMParam->u8Transfer_Characteristics    = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u8Transfer_Characteristics;
        pstMMParam->u8Matrix_Coeffs               = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u8Matrix_Coeffs;

        //test for HLG case
        //pstMMParam->u8Colour_primaries = 9;
        //pstMMParam->u8Transfer_Characteristics = 18;
        //pstMMParam->u8Matrix_Coeffs = 9;
    }

    //for mastering SEI
    //The order in MM is G->B->R
    //the order in CFD mm is R->G->B
    pstMMParam->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_x[0] = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_x[2];
    pstMMParam->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_x[1] = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_x[0];
    pstMMParam->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_x[2] = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_x[1];
    pstMMParam->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_y[0] = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_y[2];
    pstMMParam->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_y[1] = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_y[0];
    pstMMParam->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_y[2] = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_y[1];
    pstMMParam->stu_Cfd_MM_MasterPanel_ColorMetry.u16White_point_x          = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16White_point_x;
    pstMMParam->stu_Cfd_MM_MasterPanel_ColorMetry.u16White_point_y          = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16White_point_y;
    pstMMParam->u32Master_Panel_Max_Luminance                               = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u32Master_Panel_Max_Luminance;
    pstMMParam->u32Master_Panel_Min_Luminance                               = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u32Master_Panel_Min_Luminance;
    pstMMParam->u8Mastering_Display_Infor_Valid                             = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.bSEIValid;


    //for content light level SEI
    //MS_BOOL bContentLightLevelEnabled;
    //MS_U16  u16maxContentLightLevel;
    //MS_U16  u16maxPicAverageLightLevel;

    //printk("\033[1;35m###[Brian][%s][%d]### (sharememory) bContentLightLevelEnabled = %d\033[0m\n",__FUNCTION__,__LINE__,pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.bContentLightLevelEnabled);
    //printk("\033[1;35m###[Brian][%s][%d]### (sharememory) u16maxContentLightLevel =%d \033[0m\n",__FUNCTION__,__LINE__,pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16maxContentLightLevel);
    //printk("\033[1;35m###[Brian][%s][%d]### (sharememory) u16maxPicAverageLightLevel =%d \033[0m\n",__FUNCTION__,__LINE__,pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16maxPicAverageLightLevel);

    pstMMParam->u8MM_HDR_ContentLightMetaData_Valid     = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.bContentLightLevelEnabled;
    pstMMParam->u16Max_content_light_level        = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16maxContentLightLevel;
    pstMMParam->u16Max_pic_average_light_level     = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16maxPicAverageLightLevel;

    //treat default value as not valid
    if (0xff == pstMMParam->u8MM_HDR_ContentLightMetaData_Valid)
    {
        pstMMParam->u8MM_HDR_ContentLightMetaData_Valid = 0;
        pstMMParam->u16Max_content_light_level = 0;
        pstMMParam->u16Max_pic_average_light_level = 0;
    }

    if ((0 != pstMMParam->u16Max_content_light_level) ||(0 != pstMMParam->u16Max_pic_average_light_level))
    {
        pstMMParam->u8MM_HDR_ContentLightMetaData_Valid = 1;
    }

#if 0
    //for test
    MS_U8 u8byte0;
    u8byte0 = msReadByte( REG_SC_BK79_7C_L);
    printf("\n test : u8byte0 = %x\n",u8byte0);

    if (u8byte0 == 1)
    Mapi_Cfd_Set_MM_SDR_Rec709(pstMMParam);
    else if (u8byte0 == 2)
    Mapi_Cfd_Set_MM_SDR_Rec2020(pstMMParam);
    else if (u8byte0 == 3)
    Mapi_Cfd_Set_MM_HDRPQ_Rec709(pstMMParam);
    else if (u8byte0 == 4)
    Mapi_Cfd_Set_MM_HDRPQ_Rec2020(pstMMParam);
#endif

    memcpy(&gstMMParam,pstMMParam,sizeof(STU_CFDAPI_MM_PARSER));
    return E_CFD_MC_ERR_NOERR;
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

#if 0
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
            if (u8ExtendColorimetry == 4)
            {
                return E_CFD_CFIO_ADOBE_RGB;
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
#else
EN_KDRV_XC_CFD_COLOR_FORMAT MHal_XC_HDMI_Color_Format(MS_U8 u8PixelFormat, MS_U8 u8Colorimetry, MS_U8 u8ExtendColorimetry)
{
    if (u8PixelFormat== 1 || u8PixelFormat == 2 || u8PixelFormat == 3)
    {
        if (u8Colorimetry == 0)
        {
            return E_KDRV_XC_CFD_FORMAT_YUV_BT709;
        }
        else if (u8Colorimetry == 1)
        {
            return E_KDRV_XC_CFD_FORMAT_YUV_BT601_525;
        }
        else if (u8Colorimetry == 2)
        {
            return E_KDRV_XC_CFD_FORMAT_YUV_BT709;
        }
        else if (u8Colorimetry == 3)
        {
            if (u8ExtendColorimetry == 0)
            {
                return E_KDRV_XC_CFD_FORMAT_XVYCC_601;
            }
            else if (u8ExtendColorimetry == 1)
            {
                return E_KDRV_XC_CFD_FORMAT_XVYCC_709;
            }
            else if (u8ExtendColorimetry == 2)
            {
                return E_KDRV_XC_CFD_FORMAT_SYCC_601;
            }
            else if (u8ExtendColorimetry == 3)
            {
                return E_KDRV_XC_CFD_FORMAT_ADOBE_YCC601;
            }
            else if (u8ExtendColorimetry == 5)
            {
                return E_KDRV_XC_CFD_FORMAT_YUV_BT2020CL;
            }
            else if (u8ExtendColorimetry == 6)
            {
                return E_KDRV_XC_CFD_FORMAT_YUV_BT2020NCL;
            }
            else
            {
                return E_KDRV_XC_CFD_FORMAT_RESERVED_START;
            }
        }
    }
    else
    {
        if (u8Colorimetry == 0)
        {
            return E_KDRV_XC_CFD_FORMAT_SRGB;
        }
        else if (u8Colorimetry == 1)
        {
            return E_KDRV_XC_CFD_FORMAT_RGB_BT601_525;
        }
        else if (u8Colorimetry == 2)
        {
            return E_KDRV_XC_CFD_FORMAT_RGB_BT709;
        }
        else if (u8Colorimetry == 3)
        {
            if (u8ExtendColorimetry == 4)
            {
                return E_KDRV_XC_CFD_FORMAT_ADOBE_RGB;
            }
            else if (u8ExtendColorimetry == 6)
            {
                return E_KDRV_XC_CFD_FORMAT_RGB_BT2020;
            }
            else
            {
                return E_KDRV_XC_CFD_FORMAT_RESERVED_START;
            }
        }
    }

    return E_KDRV_XC_CFD_FORMAT_RESERVED_START;
}

#endif

//eCFIO ref to E_CFD_CFIO, u8MCFormat ref to E_CFD_MC_FORMAT
void MHal_XC_HDMI_Color_Format_Out(MS_U8 eCFIO, MS_U8 u8MCFormat, MS_U8 *u8PixelFormat, MS_U8 *u8Colorimetry, MS_U8 *u8ExtendColorimetry)
{
    //printk("\033[1;35m###[River][%s][%d]### %d %d\033[0m\n",__FUNCTION__,__LINE__,eCFIO,u8MCFormat);
    switch(eCFIO)
    {
        case E_CFD_CFIO_RGB_BT2020:
            *u8PixelFormat = 0;
            *u8Colorimetry = 3;
            *u8ExtendColorimetry = 6;
            break;
        case E_CFD_CFIO_ADOBE_RGB:
            *u8PixelFormat = 0;
            *u8Colorimetry = 3;
            *u8ExtendColorimetry = 4;
            break;
        case E_CFD_CFIO_YUV_BT601_625:
            *u8PixelFormat = u8MCFormat;
            *u8Colorimetry = 1;
            *u8ExtendColorimetry = 0;
            break;
        case E_CFD_CFIO_YUV_BT709:
            *u8PixelFormat = u8MCFormat;
            *u8Colorimetry = 0;
            *u8ExtendColorimetry = 0;
            break;
        case E_CFD_CFIO_YUV_BT2020_NCL:
            *u8PixelFormat = u8MCFormat;
            *u8Colorimetry = 3;
            *u8ExtendColorimetry = 6;
            break;
        case E_CFD_CFIO_YUV_BT2020_CL:
            *u8PixelFormat = u8MCFormat;
            *u8Colorimetry = 3;
            *u8ExtendColorimetry = 5;
            break;
        case E_CFD_CFIO_XVYCC_601:
            *u8PixelFormat = u8MCFormat;
            *u8Colorimetry = 3;
            *u8ExtendColorimetry = 0;
            break;
        case E_CFD_CFIO_XVYCC_709:
            *u8PixelFormat = u8MCFormat;
            *u8Colorimetry = 3;
            *u8ExtendColorimetry = 1;
            break;
        case E_CFD_CFIO_SYCC601:
            *u8PixelFormat = u8MCFormat;
            *u8Colorimetry = 3;
            *u8ExtendColorimetry = 2;
            break;
        case E_CFD_CFIO_ADOBE_YCC601:
            *u8PixelFormat = u8MCFormat;
            *u8Colorimetry = 3;
            *u8ExtendColorimetry = 3;
            break;
        case E_CFD_CFIO_SRGB:
        case E_CFD_CFIO_RGB_BT709:
        case E_CFD_CFIO_YUV_BT601_525:
        default:
            *u8PixelFormat = 0;
            *u8Colorimetry = 0;
            *u8ExtendColorimetry = 0;
            break;
    }
    return;
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

    //only for test
    //set SDR/HDR in EDID
    //Mapi_Cfd_inter_HDMI_EDID_Param_SetSDR(&stEdidParam);
    //Mapi_Cfd_inter_HDMI_EDID_Param_SetHDRPQ(&stEdidParam);

    //set colorspace in EDID
    //Mapi_Cfd_inter_HDMI_EDID_Param_SetRec709Only(&stEdidParam);
    //Mapi_Cfd_inter_HDMI_EDID_Param_SetRec2020ncl(&stEdidParam);


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
    stOSDParam.u16Hue = pstCfdOsd->u16Hue;
    stOSDParam.u16Saturation = pstCfdOsd->u16Saturation;
    stOSDParam.u16Contrast = pstCfdOsd->u16Contrast;
    stOSDParam.u8OSD_UI_En = pstCfdOsd->u8OSDUIEn;
    stOSDParam.u8OSD_UI_Mode = pstCfdOsd->u8OSDUIMode;
    stOSDParam.u8HDR_UI_H2SMode =   pstCfdOsd->u8HDR_UI_H2SMode;

    //printk("!!!stOSDParam.u8HDR_UI_H2SMode: %d\n", stOSDParam.u8HDR_UI_H2SMode);

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

    //printk("\033[1;35m###[Brian][%s][%d]### Into MHal_XC_CFD_InitIp!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__);

    //Mapi_Cfd_Curry_DLCIP_Param_Init(&stHwIpsParam.stDLCInput.stCurryDLCParam);
    Mapi_Cfd_Kano_TMOIP_Param_Init(&stHwIpsParam.stTMOInput.stKanoTMOParam);
    Mapi_Cfd_Kano_SDRIP_Param_Init(&stHwIpsParam.stSDRIPInput.stKanoSDRIPParam);
    Mapi_Cfd_Kano_HDRIP_Param_Init(&stHwIpsParam.stHDRIPInput.stKanoHDRIPParam);

    if (_bEnableHDR == TRUE && (_stCfdHdr[pstCfdFire->u8Win].u8HdrType & E_KDRV_XC_CFD_HDR_TYPE_DOLBY))
    {
        stHwIpsParam.stHDRIPInput.stKanoHDRIPParam.u8HDR_IP_enable_Mode = 0xC0;
    }
    else if (_bEnableHDR == FALSE && (_stCfdHdr[pstCfdFire->u8Win].u8HdrType & E_KDRV_XC_CFD_HDR_TYPE_OPEN))
    {
        stHwIpsParam.stHDRIPInput.stKanoHDRIPParam.u8HDR_IP_enable_Mode = 0xC0;
    }
    else if (_bEnableHDR == FALSE && (_stCfdHdr[pstCfdFire->u8Win].u8HdrType & E_KDRV_XC_CFD_HDR_TYPE_HLG))
    {
        stHwIpsParam.stHDRIPInput.stKanoHDRIPParam.u8HDR_IP_enable_Mode = 0xC0;
    }
    else
    {
        stHwIpsParam.stHDRIPInput.stKanoHDRIPParam.u8HDR_IP_enable_Mode = 0x40;
    }

    ////simulation for TMO set happens
    //g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_intoTMOset = 1;

    //inform CFD that TMO parameters from user/ini file
    //printk("\033[1;35m###[Brian][%s][%d]### u8TMO_intoTMOset = %d !!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_intoTMOset);
    if (1 == g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_intoTMOset)
    {

//only for test
#if 0
        g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smin = 0;
        g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smed = 200;
        g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smax = 200;
#endif

        stHwIpsParam.stTMOInput.stKanoTMOParam.u8HDR_TMO_param_SetbyDriver = 1;
        stHwIpsParam.stTMOInput.stKanoTMOParam.u16HDR_TMO_Tmin = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmin;
        stHwIpsParam.stTMOInput.stKanoTMOParam.u16HDR_TMO_Tmed = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmed;
        stHwIpsParam.stTMOInput.stKanoTMOParam.u16HDR_TMO_Tmax = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmax;
        //printk("\033[1;35m###[Brian][%s][%d]### u8HDR_TMO_param_SetbyDriver = %d !!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,stHwIpsParam.stTMOInput.stKanoTMOParam.u8HDR_TMO_param_SetbyDriver);
        //printk("\033[1;35m###[Brian][%s][%d]### u16HDR_TMO_Tmin = %d !!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,stHwIpsParam.stTMOInput.stKanoTMOParam.u16HDR_TMO_Tmin);
        //printk("\033[1;35m###[Brian][%s][%d]### u16HDR_TMO_Tmed = %d !!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,stHwIpsParam.stTMOInput.stKanoTMOParam.u16HDR_TMO_Tmed);
        //printk("\033[1;35m###[Brian][%s][%d]### u16HDR_TMO_Tmax = %d !!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,stHwIpsParam.stTMOInput.stKanoTMOParam.u16HDR_TMO_Tmax);
    }

    u16RetVal = Mapi_Cfd_Kano_TMOIP_Param_Check(&stHwIpsParam.stTMOInput.stKanoTMOParam);

    if (u16RetVal == E_CFD_MC_ERR_NOERR)
    {
        Mapi_Cfd_Kano_TMOIP_Param_Set(u8HWMainSubMode, &stHwIpsParam.stTMOInput.stKanoTMOParam);
    }
    else
    {
        printk("Mapi_Cfd_Kano_TMOIP_Param_Check fail, errCode: %d\n", u16RetVal);
        return u16RetVal;
    }

#if 0
    u16RetVal = Mapi_Cfd_Kano_DLCIP_Param_Check(&stHwIpsParam.stDLCInput.stKanoDLCParam);
    if (u16RetVal == E_CFD_MC_ERR_NOERR)
    {
        Mapi_Cfd_Kano_DLCIP_Param_Set(u8HWMainSubMode, &stHwIpsParam.stDLCInput.stKanoDLCParam);
    }
    else
    {
        printk("Mapi_Cfd_Kano_DLCIP_Param_Check fail, errCode: %d\n", u16RetVal);
        return u16RetVal;
    }
#endif

    u16RetVal = Mapi_Cfd_Kano_SDRIP_Param_Check(&stHwIpsParam.stSDRIPInput.stKanoSDRIPParam);
    if (u16RetVal == E_CFD_MC_ERR_NOERR)
    {
        Mapi_Cfd_Kano_SDRIP_Param_Set(u8HWMainSubMode, &stHwIpsParam.stSDRIPInput.stKanoSDRIPParam);
    }
    else
    {
        printk("Mapi_Cfd_Kano_SDRIP_Param_Check fail, errCode: %d\n", u16RetVal);
        return u16RetVal;
    }

    u16RetVal = Mapi_Cfd_Kano_HDRIP_Param_Check(&stHwIpsParam.stHDRIPInput.stKanoHDRIPParam);
    if (u16RetVal == E_CFD_MC_ERR_NOERR)
    {
        Mapi_Cfd_Kano_HDRIP_Param_Set(u8HWMainSubMode, &stHwIpsParam.stHDRIPInput.stKanoHDRIPParam);
    }
    else
    {
        printk("Mapi_Cfd_Kano_HDRIP_Param_Check fail, errCode: %d\n", u16RetVal);
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
        MHal_XC_W2BYTEMSK(0x100a4a, 0x0, 0x100);
        MHal_XC_W2BYTEMSK(0x100a4a, 0, 0xF0);
        // hdr clk setting enable
        if (bImmediate)
        {
            MHal_XC_W2BYTE(REG_SC_BK79_02_L, 0);
            MHal_XC_W2BYTEMSK(REG_SC_BK79_7E_L, BIT(1)|BIT(0), BIT(1)|BIT(0));
        }
        else
        {
            KApi_XC_MLoad_WriteCmd(E_CLIENT_MAIN_HDR, REG_SC_BK79_02_L, 0, 0xFFFF);
            KApi_XC_MLoad_WriteCmd(E_CLIENT_MAIN_HDR, REG_SC_BK79_7E_L, BIT(1)|BIT(0), BIT(1)|BIT(0));
        }
    }
    else
    {
        MHal_XC_W2BYTEMSK(0x100a4a, 0x100, 0x100);
        MHal_XC_W2BYTEMSK(0x100a4a, 0x10, 0xF0);
        // hdr clk setting disable
        if (bImmediate)
        {
            MHal_XC_W2BYTE(REG_SC_BK79_02_L, 0xFFFF);
            MHal_XC_W2BYTEMSK(REG_SC_BK79_7E_L, 0, BIT(1)|BIT(0));
        }
        else
        {
            KApi_XC_MLoad_WriteCmd(E_CLIENT_MAIN_HDR, REG_SC_BK79_02_L, 0xFFFF, 0xFFFF);
            KApi_XC_MLoad_WriteCmd(E_CLIENT_MAIN_HDR, REG_SC_BK79_7E_L, 0, BIT(1)|BIT(0));
        }
    }
    return TRUE;
}

MS_U16 MHal_XC_CFD_SetMainCtrl(ST_KDRV_XC_CFD_FIRE *pstCfdFire)
{
    STU_CFDAPI_MAIN_CONTROL stMainControl;
    MS_U16 u16RetVal = 0;
    memset(&stMainControl, 0, sizeof(STU_CFDAPI_MAIN_CONTROL));

    stMainControl.u32Version = CFD_MAIN_CONTROL_ST_VERSION;
    stMainControl.u16Length = sizeof(STU_CFDAPI_MAIN_CONTROL);
    stMainControl.u8Input_Source = pstCfdFire->u8InputSource;

    if (CFD_IS_HDMI(pstCfdFire->u8InputSource))
    {
        // HDMI
        stMainControl.u8Input_Format = MHal_XC_HDMI_Color_Format(_stCfdHdmi[pstCfdFire->u8Win].u8PixelFormat, _stCfdHdmi[pstCfdFire->u8Win].u8Colorimetry, _stCfdHdmi[pstCfdFire->u8Win].u8ExtendedColorimetry);
        stMainControl.u8Input_DataFormat = MHal_XC_HDMI_Color_Data_Format(_stCfdHdmi[pstCfdFire->u8Win].u8PixelFormat);
        stMainControl.u8Input_IsFullRange = _stCfdHdmi[pstCfdFire->u8Win].bIsFullRange;
        if (_bEnableHDR == TRUE)
        {
            stMainControl.u8Input_HDRMode = E_CFIO_MODE_HDR1;
        }
        else
        {
            stMainControl.u8Input_HDRMode = (_stCfdHdmi[pstCfdFire->u8Win].u8EOTF == 2)?E_CFIO_MODE_HDR2:E_CFIO_MODE_SDR;
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

        //can not use E_CFD_CFIO_YUV_NOTSPECIFIED here
        stMainControl.u8Input_Format = E_KDRV_XC_CFD_FORMAT_YUV_NOT_SPECIFIED;
        //stMainControl.u8Input_Format = E_CFD_CFIO_YUV_NOTSPECIFIED;

        //stMainControl.u8Input_DataFormat = 2;
        stMainControl.u8Input_DataFormat = E_CFD_MC_FORMAT_YUV444;

        //stMainControl.u8Input_IsFullRange = stMMParam.u8Mastering_Display_Infor_Valid;
        stMainControl.u8Input_IsFullRange = stMMParam.u8Video_Full_Range_Flag;

        if (_bEnableHDR == TRUE)
        {
            stMainControl.u8Input_HDRMode = E_CFIO_MODE_HDR1;
        }
        else
        {
            if (stFormatInfo.HDRMemFormat.stHDRMemFormatCFD.bVUIValid == TRUE)
            {
                //stMainControl.u8Input_HDRMode = (stMMParam.u8Transfer_Characteristics == 16)?E_CFIO_MODE_HDR2:E_CFIO_MODE_SDR;
                if (16 == stMMParam.u8Transfer_Characteristics )
                {
                    stMainControl.u8Input_HDRMode = E_CFIO_MODE_HDR2;
                }
                else if (18 == stMMParam.u8Transfer_Characteristics )
                {
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

    //stMainControl.u8Input_HDRMode = _stCfdHdr[pstCfdFire->u8Win].u8HdrType;
    stMainControl.u8Input_IsRGBBypass = pstCfdFire->bIsRgbBypass;
    stMainControl.u8Input_SDRIPMode = 1;
    stMainControl.u8Input_HDRIPMode = 1;

    stMainControl.stu_Middle_Format[0].u8Mid_Format_Mode = 0;
    //can not use E_CFD_CFIO_YUV_NOTSPECIFIED here
    stMainControl.stu_Middle_Format[0].u8Mid_Format = E_KDRV_XC_CFD_FORMAT_YUV_NOT_SPECIFIED;
    stMainControl.stu_Middle_Format[0].u8Mid_DataFormat = E_CFD_MC_FORMAT_YUV444;
    stMainControl.stu_Middle_Format[0].u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
    stMainControl.stu_Middle_Format[0].u8Mid_HDRMode = E_CFIO_MODE_SDR;
    stMainControl.stu_Middle_Format[0].u8Mid_Colour_primaries = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
    stMainControl.stu_Middle_Format[0].u8Mid_Transfer_Characteristics = E_CFD_CFIO_TR_BT709;
    stMainControl.stu_Middle_Format[0].u8Mid_Matrix_Coeffs = E_CFD_CFIO_MC_BT709_XVYCC709;
    //*/
    //stMainControl.u8Output_Source = E_CFD_OUTPUT_SOURCE_PANEL;
    stMainControl.u8Output_Source = E_CFD_OUTPUT_SOURCE_HDMI;
    //stMainControl.u8Output_Format = _stCfdPanel.u8ColorFormat;
    //can not use E_CFD_CFIO_YUV_NOTSPECIFIED here
    stMainControl.u8Output_Format = E_KDRV_XC_CFD_FORMAT_YUV_NOT_SPECIFIED;
    stMainControl.u8Output_DataFormat = E_CFD_MC_FORMAT_YUV422;
    stMainControl.u8Output_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
    stMainControl.u8Output_HDRMode = E_CFIO_MODE_SDR;
    stMainControl.u8HDMIOutput_GammutMapping_Mode = 1;
    stMainControl.u8HDMIOutput_GammutMapping_MethodMode = 0;
    stMainControl.u8MMInput_ColorimetryHandle_Mode = 1;
    stMainControl.u8PanelOutput_GammutMapping_Mode = _stCfdPanel.bLinearRgb & _stCfdLinearRgb.bEnable;

    stMainControl.u8TMO_TargetRefer_Mode = 2;

    stMainControl.u16Source_Min_Luminance = 60;    //data * 0.0001 nits
    stMainControl.u16Source_Med_Luminance = 120;   //data * 1 nits
    stMainControl.u16Source_Max_Luminance = 4000;  //data * 1 nits

    //stMainControl.u16Target_Min_Luminance = 4000;    //data * 0.0001 nits
    //stMainControl.u16Target_Med_Luminance = 240;   //data * 1 nits
    //stMainControl.u16Target_Max_Luminance = 600;    //data * 1 nits

    //for new TMO in gamma domain

    stMainControl.u16Target_Min_Luminance = 3000;   //data * 0.0001 nits
    stMainControl.u16Target_Med_Luminance = 120;    //data * 1 nits
    stMainControl.u16Target_Max_Luminance = 600;    //data * 1 nits
    //stMainControl.u16Target_Max_Luminance = 100;    //data * 1 nits

    //for TMO parameter control method extension
    stMainControl.u8TMO_SourceUserModeEn = 1;
    //0: auto
    //priority : infoFrame > driver > CFD default
    //1: force, refer to u8SourceUserMode

    //write
    stMainControl.u8TMO_SourceUserMode = 1;
    //0: refer to infoFrame
    //1: refer to driver
    //2: refer to CFD default

    stMainControl.u8TMO_TargetUserModeEn = 1;
    //0: auto
    //priority : EDID > driver > CFD default
    //1: force, refer to u8SourceUserMode

    //write
    stMainControl.u8TMO_TargetUserMode = 1;
    //0: refer to EDID
    //1: refer to driver
    //2: refer to CFD default

    //for user-defined control
    //stMainControl.u8PredefinedProcess = 0;

    memset(&(stMainControl.stu_seamless),0x00,sizeof(STU_CFDAPI_SEAMLESS));
    //printk("MHal_XC_CFD_SetMainCtrl:stMainControl.u8PredefinedProcess: %d\n", stMainControl.u8PredefinedProcess);

    //printf("\033[1;35m###[Brian][%s][%d]### 0a u8Output_DataFormat = %x \033[0m\n",__FUNCTION__,__LINE__, stMainControl.u8Output_DataFormat);

    u16RetVal = Mapi_Cfd_inter_Main_Control_Param_Check(&stMainControl,0);

    //printf("\033[1;35m###[Brian][%s][%d]### 0b u8Output_DataFormat = %x \033[0m\n",__FUNCTION__,__LINE__, stMainControl.u8Output_DataFormat);

    Mapi_Cfd_inter_Main_Control_Param_Set(&stMainControl);
    if (u16RetVal == E_CFD_MC_ERR_NOERR)
    {

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

MS_U16 MHal_XC_CFD_SetMmParam(ST_KDRV_XC_CFD_FIRE *pstCfdFire)
{
    STU_CFDAPI_MM_PARSER stMMParam;
    MS_U16 u16RetVal = 0;
    ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info stFormatInfo;
    memset(&stMMParam, 0, sizeof(STU_CFDAPI_MM_PARSER));

    MHal_XC_CFD_WithdrawMMParam(&stFormatInfo, pstCfdFire->u8Win);
    _MHal_XC_CFD_Convert_XCDRAM_to_CFDMMParam(pstCfdFire, &stMMParam,&stFormatInfo);

    u16RetVal = Mapi_Cfd_inter_MM_Param_Check(&stMMParam);
    Mapi_Cfd_inter_MM_Param_Set(&stMMParam);
    if (u16RetVal == E_CFD_MC_ERR_NOERR)
    {

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
    Mapi_Cfd_inter_HDMI_InfoFrame_Param_Set(&stHDMIInfoFrameParam);
    if (u16RetVal == E_CFD_MC_ERR_NOERR)
    {

    }
    else
    {
        printk("Mapi_Cfd_inter_HDMI_InfoFrame_Param_Check fail, errCode: %d\n", u16RetVal);
    }

    return u16RetVal;
}

static MS_U16 MHal_XC_CFD_GetHdmiOutParam(MS_U8 u8win)
{
    STU_CFDAPI_HDMI_INFOFRAME_PARSER_OUT stHDMIInfoFrameParam_out;
    memset(&stHDMIInfoFrameParam_out, 0, sizeof(STU_CFDAPI_HDMI_INFOFRAME_PARSER_OUT));
    stHDMIInfoFrameParam_out.u32Version = 0;
    stHDMIInfoFrameParam_out.u16Length = sizeof(STU_CFDAPI_HDMI_INFOFRAME_PARSER);
    Mapi_Cfd_inter_HDMI_InfoFrame_Param_Out_Get(&stHDMIInfoFrameParam_out);

    _stCfdHdmi_Out[u8win].bHDRInfoFrameValid = stHDMIInfoFrameParam_out.u8HDMISource_HDR_InfoFrame_Valid;
    _stCfdHdmi_Out[u8win].u8EOTF = stHDMIInfoFrameParam_out.u8HDMISource_EOTF;
    _stCfdHdmi_Out[u8win].u8SMDID = stHDMIInfoFrameParam_out.u8HDMISource_SMD_ID;
    _stCfdHdmi_Out[u8win].u16MasterPanelMaxLuminance = stHDMIInfoFrameParam_out.u16Master_Panel_Max_Luminance;    //data * 1 nits
    _stCfdHdmi_Out[u8win].u16MasterPanelMinLuminance = stHDMIInfoFrameParam_out.u16Master_Panel_Min_Luminance;    //data * 0.0001 nits
    _stCfdHdmi_Out[u8win].u16MaxContentLightLevel = stHDMIInfoFrameParam_out.u16Max_Content_Light_Level;       //data * 1 nits
    _stCfdHdmi_Out[u8win].u16MaxFrameAvgLightLevel = stHDMIInfoFrameParam_out.u16Max_Frame_Avg_Light_Level;     //data * 1 nits
    _stCfdHdmi_Out[u8win].u16Display_Primaries_x[0] = stHDMIInfoFrameParam_out.stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_x[0];
    _stCfdHdmi_Out[u8win].u16Display_Primaries_x[1] = stHDMIInfoFrameParam_out.stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_x[1];
    _stCfdHdmi_Out[u8win].u16Display_Primaries_x[2] = stHDMIInfoFrameParam_out.stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_x[2];
    _stCfdHdmi_Out[u8win].u16Display_Primaries_y[0] = stHDMIInfoFrameParam_out.stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_y[0];
    _stCfdHdmi_Out[u8win].u16Display_Primaries_y[1] = stHDMIInfoFrameParam_out.stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_y[1];
    _stCfdHdmi_Out[u8win].u16Display_Primaries_y[2] = stHDMIInfoFrameParam_out.stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_y[2];
    _stCfdHdmi_Out[u8win].u16White_point_x = stHDMIInfoFrameParam_out.stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16White_point_x;
    _stCfdHdmi_Out[u8win].u16White_point_y = stHDMIInfoFrameParam_out.stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16White_point_y;
    _stCfdHdmi_Out[u8win].u8SMDID = stHDMIInfoFrameParam_out.u8Mastering_Display_Infor_Valid==1? 0 : 1;
    _stCfdHdmi_Out[u8win].u8RgbQuantizationRange = (stHDMIInfoFrameParam_out.u8HDMISource_Support_Format >> 5) & 0x3;
    _stCfdHdmi_Out[u8win].u8YccQuantizationRange = (stHDMIInfoFrameParam_out.u8HDMISource_Support_Format >> 3) & 0x3;
    _stCfdHdmi_Out[u8win].u8PixelFormat = (stHDMIInfoFrameParam_out.u8HDMISource_Support_Format) & 0x7;
    MHal_XC_HDMI_Color_Format_Out(stHDMIInfoFrameParam_out.u8HDMISource_Colorspace,stHDMIInfoFrameParam_out.u8Output_DataFormat,&_stCfdHdmi_Out[u8win].u8PixelFormat, &_stCfdHdmi_Out[u8win].u8Colorimetry, &_stCfdHdmi_Out[u8win].u8ExtendedColorimetry);
    return E_CFD_MC_ERR_NOERR;
}


MS_U16 MHal_XC_CFD_SetHdrParam(ST_KDRV_XC_CFD_FIRE *pstCfdFire)
{
    STU_CFDAPI_HDR_METADATA_FORMAT stHDRMetadataFormatParam;
    MS_U16 u16RetVal = 0;
    memset(&stHDRMetadataFormatParam, 0, sizeof(STU_CFDAPI_HDR_METADATA_FORMAT));
    stHDRMetadataFormatParam.u32Version = CFD_HDR_METADATA_VERSION;
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

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
#ifdef CONFIG_MSTAR_XC_HDR_SUPPORT
static MS_U16 _HAL_XC_ReadMetadata2BYTE(MS_U16 u16Addr)
{
    MS_U16 u16FlagValue = 0;
    MS_U16 u16Value = 0;
    // set descrb read back addr
    MHal_XC_W2BYTEMSK(REG_SC_BK79_12_L, u16Addr, 0x00FF);
    // set check flag
    MHal_XC_W2BYTEMSK(REG_SC_BK79_12_L, (u16Addr % 16) << 12, 0xF000);
    // read check flag
    u16FlagValue = MHal_XC_R2BYTEMSK(REG_SC_BK79_14_L, 0xF000) >> 12;

    struct timespec startTs;
    struct timespec endTs;
    getnstimeofday(&startTs);
    // wait read-back flag == set flag, longest time = 4/350ms
    while (u16FlagValue != (u16Addr % 16))
    {
        // timeout is 1s
        getnstimeofday(&endTs);
        long diff = endTs.tv_sec * 1000000 + endTs.tv_nsec - startTs.tv_sec * 1000000 - startTs.tv_nsec;
        if (diff > 1000000)
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
    if( DOLBY_VSIF_LEN != MHal_XC_R2BYTEMSK(REG_HDMI_DUAL_0_31_L,0x00FF) )
    {
        return FALSE;
    }

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

#define DOLBY_FIRST_PACKET_REMAIN_MAX_LENGTH (XC_HDR_DOLBY_PACKET_LENGTH - XC_HDR_DOLBY_PACKET_HEADER - XC_HDR_DOLBY_PACKET_TAIL - XC_HDR_DOLBY_METADATA_LENGTH_BIT)
#define DOLBY_OTHER_PACKET_REMAIN_MAX_LENGTH (XC_HDR_DOLBY_PACKET_LENGTH - XC_HDR_DOLBY_PACKET_HEADER - XC_HDR_DOLBY_PACKET_TAIL)

    MS_BOOL bReturn = FALSE;
    int i = 0, j = 0;
    MS_U8 *pu8Data = pu8Metadata;
    MS_U32 au32CrcReg[6] = {REG_SC_BK79_1A_L, REG_SC_BK79_1B_L, REG_SC_BK79_1C_L, REG_SC_BK79_1D_L, REG_SC_BK79_1E_L, REG_SC_BK79_1F_L};
    MS_U16 u16MetadataLength = 0;
    MS_U8 u8MetadataLengthLow = 0;
    MS_U8 u8MetadataLengthHigh = 0;
    MS_U8 u8PacketType = 0;
    MS_U8 u8MetadataType = 0;

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

    if (u8PacketType != 0x00)
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
        printk("Get hdmi metadata, exceed max length, hw don't support!\n");
        bReturn = FALSE;
        goto EXIT;
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
    //printf("\033[1;35m###[River][%s][%d]### %d\033[0m\n",__FUNCTION__,__LINE__,enInputSourceType);

    return TRUE;
}

BOOL MHal_XC_SetHDRType(EN_KDRV_XC_HDR_TYPE enHDRType)
{
    _enHDRType = enHDRType;
    //printf("\033[1;35m###[River][%s][%d]### %d\033[0m\n",__FUNCTION__,__LINE__,enHDRType);
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
    }
    else if (IS_OTT_DOLBY)
    {
        MHal_XC_UpdatePath(E_KDRV_XC_HDR_PATH_DOLBY_OTT_DUAL);
        MHal_XC_EnableAutoSeamless(FALSE);
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
    //printk("\033[1;35m###[River][%s][%d]### %d\033[0m\n",__FUNCTION__,__LINE__,enPath);
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
            // dma path select
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 1 << 6, BIT(6));
            // dma path user mode
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 1 << 7, BIT(7));
            // EL / BL off
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 1 << 10, BIT(10));
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 1 << 11, BIT(11));
            // open hdr enable
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(15));
            // PQ bypass control
            MHal_XC_W2BYTEMSK(REG_SC_BK7A_01_L, 0, BIT(0));
            MHal_XC_W2BYTEMSK(REG_SC_BK7A_01_L, 0x8000, 0xF000);
            MHal_XC_W2BYTEMSK(REG_SC_BK7A_02_L, 0x3, 0x007F);
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
            // dma path select
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(6));
            // dma path user mode
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(7));
            // EL / BL off
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 1 << 10, BIT(10));
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(11));
            // open hdr enable
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(15));
            // PQ bypass control
            MHal_XC_W2BYTEMSK(REG_SC_BK7A_01_L, 0, BIT(0));
            MHal_XC_W2BYTEMSK(REG_SC_BK7A_01_L, 0xB000, 0xF000);
            MHal_XC_W2BYTEMSK(REG_SC_BK7A_02_L, 0x3, 0x007F);
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
            // dma path select
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(6));
            // dma path user mode
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(7));
            // EL / BL off
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(10));
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(11));
            // open hdr enable
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(15));
            // PQ bypass control
            MHal_XC_W2BYTEMSK(REG_SC_BK7A_01_L, 0, BIT(0));
            MHal_XC_W2BYTEMSK(REG_SC_BK7A_01_L, 0xB000, 0xF000);
            MHal_XC_W2BYTEMSK(REG_SC_BK7A_02_L, 0x3, 0x007F);
            break;
        }
        case E_KDRV_XC_HDR_PATH_OPEN_HDMI:
        case E_KDRV_XC_HDR_PATH_OPEN_OTT:
        {
            // patch here, after CFD finishes we have to remove all these registers
            {
                MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(5));
                MHal_XC_W2BYTEMSK(REG_SC_BK7A_01_L, 0, BIT(0));
                MHal_XC_W2BYTEMSK(REG_SC_BK7A_01_L, 0x0000, 0xF000);
            }
#if 0
            // disable FRC mode
            MHal_XC_W2BYTEMSK(REG_SC_BK79_03_L, 0, BIT(14));
            // hdr dma seamless with rfbl mode
            MHal_XC_W2BYTEMSK(REG_SC_BK79_03_L, 0, BIT(15));
            // hdr auto seamless md
            MHal_XC_W2BYTEMSK(REG_SC_BK79_04_L, 0, BIT(15));
            // hdmi422 to 14b for dm: 00: msb  01: lsb  11: lsb/C-signed
            MHal_XC_W2BYTEMSK(REG_SC_BK79_06_L, 0, 0x0300);
            // hdr to ip2 focre ack
            MHal_XC_W2BYTEMSK(REG_SC_BK79_06_L, 0, BIT(15));
            // dc path enable
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(0));
            // ip2 path enable
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 1 << 1, BIT(1));
            // ipsrc_out_pac
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 1 << 4 , BIT(4));
            // ipsrc to dm data pac user mode
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 1 << 5, BIT(5));
            // dma path select
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(6));
            // dma path user mode
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(7));
            // EL / BL off
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(10));
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(11));
            // open hdr enable
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 1 << 15, BIT(15));
            // PQ bypass control
            MHal_XC_W2BYTEMSK(REG_SC_BK7A_01_L, 0, BIT(0));
            MHal_XC_W2BYTEMSK(REG_SC_BK7A_02_L, 0x18, 0x007F);
#endif
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
            // disable FRC mode
            MHal_XC_W2BYTEMSK(REG_SC_BK79_03_L, 0, BIT(14));
            // hdr dma seamless with rfbl mode
            MHal_XC_W2BYTEMSK(REG_SC_BK79_03_L, 0, BIT(15));
            // hdr auto seamless md
            MHal_XC_W2BYTEMSK(REG_SC_BK79_04_L, 0, BIT(15));
            // disable irq
            MHal_XC_W2BYTEMSK(REG_SC_BK00_14_L, 0x3 << 5, 0x0060);
            MHal_XC_W2BYTEMSK(REG_SC_BK00_12_L, 0x0, 0x0060);
            break;
        }
        case E_KDRV_XC_HDR_PATH_DOLBY_OTT_NONE:
        {
            // enable scaler to mvop hand shaking mode
            MHal_XC_W2BYTEMSK(REG_SC_BK02_10_L, 0x25, 0x25);
            // dc path disable
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(0));
            // ip2 path disable
            MHal_XC_W2BYTEMSK(REG_SC_BK79_07_L, 0, BIT(1));
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
            // diable irq
            MHal_XC_W2BYTEMSK(REG_SC_BK00_15_L, 0x3 << 5, 0x0060);
            MHal_XC_W2BYTEMSK(REG_SC_BK00_13_L, 0x0, 0x0060);
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
    MHal_XC_W2BYTEMSK(REG_SC_BK79_08_L, u16Width, 0x1FFF);
    MHal_XC_W2BYTEMSK(REG_SC_BK79_09_L, u16Height, 0x0FFF);

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
        // disable irq
        MHal_XC_W2BYTEMSK(REG_SC_BK00_14_L, 0x3 << 5, 0x0060);
        MHal_XC_W2BYTEMSK(REG_SC_BK00_12_L, 0x0, 0x0060);

        //TV can not mark these code ++++++++++++++++++
        //MHal_XC_W2BYTE(REG_SC_BK79_7C_L, 0x0000);
        //MHal_XC_W2BYTE(REG_SC_BK79_7D_L, 0x0000);
        //+++++++++++++++++++++++++++++++++++++

        // hdr clk setting enable
        //MHal_XC_W2BYTE(REG_SC_BK79_02_L, 0xFFFF);
        //MHal_XC_W2BYTEMSK(REG_SC_BK79_7E_L, 0, BIT(1)|BIT(0));

        // close Histogram report
        MHal_XC_W2BYTEMSK(0x135C02, 0x0, 0x3);

        _enHDRType = E_KDRV_XC_HDR_NONE;
        _enInputSourceType = E_KDRV_XC_INPUT_SOURCE_NONE;
    }
    else
    {
        // hdr clk setting disable
        //MHal_XC_W2BYTE(REG_SC_BK79_02_L, 0);
        //MHal_XC_W2BYTEMSK(REG_SC_BK79_7E_L, BIT(1)|BIT(0), BIT(1)|BIT(0));
        DoVi_Trigger3DLutUpdate();

        // enable irq
        MHal_XC_W2BYTEMSK(REG_SC_BK00_12_L, 0x0, 0x0060);
        MHal_XC_W2BYTEMSK(REG_SC_BK00_14_L, 0x0, 0x0060);

        //TV can not mark these code ++++++++++++++++++
        //MHal_XC_W2BYTE(REG_SC_BK79_7C_L, 0xFFFF);
        //MHal_XC_W2BYTE(REG_SC_BK79_7D_L, 0xFFFF);
        //+++++++++++++++++++++++++++++++++++++

        // Open Histogram report
        MHal_XC_W2BYTEMSK(0x135C02, 0x3, 0x3);
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
    return FALSE;
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
    MHal_XC_W2BYTEMSK(REG_SC_BK79_04_L, 0x5 << 8, 0x1F00);

    // enable lut use auto_dl
    MHal_XC_W2BYTEMSK(REG_SC_BK7A_70_L, 1 << 15, BIT(15));
    // clear error flag
    MHal_XC_W2BYTEMSK(REG_SC_BK7A_70_L, 0, BIT(9));
    // auto_dl protect
    MHal_XC_W2BYTEMSK(REG_SC_BK7A_70_L, 0x0, 0x000F);
    // auto_dl trigger mode0 delay
    MHal_XC_W2BYTEMSK(REG_SC_BK7A_71_L, 0x30, 0x7FFF);
    // auto_dl trigger mode
    MHal_XC_W2BYTEMSK(REG_SC_BK7A_71_L, 0, BIT(15));
    // auto_dl clk select mask
    MHal_XC_W2BYTEMSK(REG_SC_BK7A_72_L, 0, 0x0003);
    // auto_dl trigger vsync select
    MHal_XC_W2BYTEMSK(REG_SC_BK7A_72_L, 1 << 15, BIT(15));

    // disable FRC mode
    MHal_XC_W2BYTEMSK(REG_SC_BK79_03_L, 0, BIT(14));
    // F2 field number
    MHal_XC_W2BYTEMSK(REG_SC_BK42_19_L, 0x2, 0x001F);
    // hdr clk setting disable
    MHal_XC_W2BYTE(REG_SC_BK79_02_L, 0xFFFF);
    MHal_XC_W2BYTEMSK(REG_SC_BK79_7E_L, 0, BIT(1)|BIT(0));
    _bEnableHDRCLK = FALSE;

    // init color format hw
    _pstDmRegTable = MsHdr_DmAllocRegTable(); // register table
    _pstDmConfig = DoVi_DmAllocConfig(); // configuration file
    DoVi_DmSetDefaultConfig(_pstDmConfig);
    _pstCompRegTable = MsHdr_CompAllocRegTable(); // register table
    _pstCompConfExt = DoVi_CompAllocConfig(); // configuration file
    DoVi_CompSetDefaultConfig(_pstCompConfExt);

    return TRUE;
}

BOOL MHal_XC_ExitHDR()
{
    if ((pu8AutoDownloadDRAMBaseAddr != NULL) && (_bUnmapAutoDownloadDRAM == TRUE))
    {
        iounmap(pu8AutoDownloadDRAMBaseAddr);
        pu8AutoDownloadDRAMBaseAddr = NULL;
    }

    // Release resource
    MsHdr_DmFreeRegTable(_pstDmRegTable);
    DoVi_DmFreeConfig(_pstDmConfig);
    MsHdr_CompFreeRegTable(_pstCompRegTable);
    DoVi_CompFreeConfig(_pstCompConfExt);

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
    MS_BOOL bCurrEnable = FALSE;
    mutex_lock(&_adl_mutex);

    bCurrEnable = _stClientInfo[enClient].bEnable;
    MS_U32 u32Offset = 0;

    if (MHal_XC_GetMiuOffset(u32MiuNo, &u32Offset) != TRUE)
    {
        mutex_unlock(&_adl_mutex);
        printk("Invalid miuno.\n");
        return FALSE;
    }

    if ((bEnable == TRUE) && (bCurrEnable != bEnable))
    {
        _stClientInfo[enClient].phyBaseAddr = phyBaseAddr;
        _stClientInfo[enClient].u32Size = u32Size;
        _stClientInfo[enClient].bEnable = bEnable;

        // reset memory
        if (pfn_valid(__phys_to_pfn(phyBaseAddr + u32Offset)))
        {
            pu8AutoDownloadDRAMBaseAddr = __va(phyBaseAddr + u32Offset);
            _bUnmapAutoDownloadDRAM = FALSE;
        }
        else
        {
            pu8AutoDownloadDRAMBaseAddr = (MS_U8 __iomem *)ioremap(phyBaseAddr + u32Offset, u32Size);
            _bUnmapAutoDownloadDRAM = TRUE;
        }

        if (pu8AutoDownloadDRAMBaseAddr != NULL)
        {
            memset(pu8AutoDownloadDRAMBaseAddr, 0, u32Size);
        }
        else
        {
            mutex_unlock(&_adl_mutex);
            printk("%s: %d, auto downlaod mmap pa fail.\n", __FUNCTION__, __LINE__);
            return FALSE;
        }
    }
    else if (bEnable == FALSE)
    {
        mutex_unlock(&_adl_mutex);
        _stClientInfo[enClient].bEnable = bEnable;
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

            //set work mode
            _Hal_XC_Set_Auto_Download_WorkMode(REG_SC_BK67_28_L, 1, enMode);
            break;
        }
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC:
        {
            _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC].phyBaseAddr = phyBaseAddr;
            _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC].u32Size = u32Size;
            _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC].bEnable = bEnable;
            _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC].u32StartAddr = 0xFFFFFFFF;
            _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC].u32Depth = 0;
            _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC].enMode = enMode;

            //set baseaddr
            //MHal_XC_W2BYTE(REG_SC_BK67_29_L, (phyBaseAddr / BYTE_PER_WORD) & 0x0000FFFF);
            // MHal_XC_W2BYTEMSK(REG_SC_BK67_2A_L, ((phyBaseAddr / BYTE_PER_WORD) >> 16 & 0x0000FFFF), 0x07FF);

            ////set work mode
            _Hal_XC_Set_Auto_Download_WorkMode(REG_SC_BK67_28_L, 1, enMode);
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
#ifdef K6_AUTO_DOWNLOADHDR_GAMMA_PATCH
                WRITE_GAMMA_DATA_FORMAT_1(pu8BaseAddr+(u32DataSize*BYTE_PER_WORD), u32Index, u16Value);
#else
                WRITE_GAMMA_DATA_FORMAT_1(pu8BaseAddr, u32Index, u16Value);
#endif
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

    u32Depth = u32Depth > u32WriteNum ? u32Depth : u32WriteNum;
    *pu32Depth = u32Depth;

#if DEBUG_HDR

int j = 0;
int l = 0;
int k = 0x10;//AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR;
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

    int k = 0x10//AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR;
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


static BOOL _Hal_XC_Auto_Download_Format_Xvycc_Data(
    EN_KDRV_XC_AUTODOWNLOAD_SUB_CLIENT enSubClient, MS_U8* pu8Data, MS_U32 u32Size, MS_U16 u16StartAddr)
{
    MS_PHY phyBaseAddr = /*0x003FF80000;*/_stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].phyBaseAddr;
    MS_U32 u32Depth = _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].u32Depth;
    MS_U8* pVirtBaseAddr = NULL;
    MS_U8 *pu8BaseAddr = NULL;
    unsigned int i = 0;
    MS_U32 u32Index = u16StartAddr;
    MS_U32 u32WriteNum = 0;
    MS_U32 u32StartAddr = 0;
    MS_U32 u32DataSize = 0;
    printf("\033[1;35m###[River][%s][%d]### \033[0m\n",__FUNCTION__,__LINE__);

    if(pu8AutoDownloadDRAMBaseAddr == NULL)
    {
        printk("%s: %d pu8AutoDownloadDRAMBaseAddr is NULL.\n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    pVirtBaseAddr = pu8AutoDownloadDRAMBaseAddr;
    pu8BaseAddr = (MS_U8 *)pVirtBaseAddr;
    pu8BaseAddr += 16 * u16StartAddr;

    // update start addr of DRAM
    u32StartAddr = _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC].u32StartAddr;
    if (u32StartAddr == 0xFFFFFFFF)
    {
        _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC].u32StartAddr = u16StartAddr;
    }
    else
    {
        _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC].u32StartAddr = u32StartAddr > u16StartAddr ? u16StartAddr : u32StartAddr;
    }

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
            pu8BaseAddr+=(768*BYTE_PER_WORD);
            for (i = (768); i < (768+256); )
            {
                MS_U16 u16Value = (*(((MS_U16 *)pu8Data) +  (i%256)));
                if (u16Value & 0xF000)
                {
                    printk("The %dth data is 0x%x, T exceed max value, please check!!!\n", i + 1, u16Value);
                    return FALSE;
                }
                WRITE_XVYCC_GAMMA_DATA_FORMAT_1(pu8BaseAddr,  u16Value);
                i++;
                pu8BaseAddr += BYTE_PER_WORD;
                u32WriteNum++;
                u32Index++;
            }
            break;
        }

        case E_KDRV_XC_AUTODOWNLOAD_XVYCC_SUB_DEGAMMA:
        {
            for (i = (0); i < (600); )
            {
                MS_U32 u32Value = *(((MS_U32 *)pu8Data) + (i%600));
                if (u32Value & 0xFFF00000)
                {
                    printk("The %dth data is 0x%x, exceed max value, please check!!!\n", i + 1, u32Value);
                    return FALSE;
                }
                //printk("Degamma value being downloaded is %d: %d\n",i,u32Value);
                WRITE_XVYCC_DEGAMMA_DATA_FORMAT_1(pu8BaseAddr, u32Value);
                i++;
                pu8BaseAddr += BYTE_PER_WORD;
                u32WriteNum++;
                u32Index++;
            }
            break;

        }

    }




    printk("Bytes per word = %d\n",BYTE_PER_WORD);
    u32Depth = u32Depth > u32WriteNum ? u32Depth : u32WriteNum;
    _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].u32Depth = u32Depth;


    return TRUE;
}


BOOL MHal_XC_WriteAutoDownload(EN_KDRV_XC_AUTODOWNLOAD_CLIENT enClient, MS_U8* pu8Data, MS_U32 u32Size, void* pParam)
{
    mutex_lock(&_adl_mutex);
    //printk("\033[1;35m###[River][%s][%d]### %d %d %d\033[0m\n",__FUNCTION__,__LINE__,enClient,u32Size,_stClientInfo[enClient].enMode);

    MS_U32 u32Depth = _stClientInfo[enClient].u32Depth;
    EN_KDRV_XC_AUTODOWNLOAD_MODE enMode = _stClientInfo[enClient].enMode;
    EN_KDRV_XC_AUTO_DOWNLOAD_STATUS enStatus = _enAutoDownloadStatus[enClient];
    MS_U8 *pu8BaseAddr = NULL;
    MS_U32 u32StartAddr = _stClientInfo[enClient].u32StartAddr;

    if (_stClientInfo[enClient].bEnable == FALSE)
    {
        mutex_unlock(&_adl_mutex);
        printk("Auto downlaod client is disabled, please enable first.\n");
        return FALSE;
    }

    if(pu8AutoDownloadDRAMBaseAddr == NULL)
    {
        mutex_unlock(&_adl_mutex);
        printk("%s: %d pu8AutoDownloadDRAMBaseAddr is NULL.\n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    pu8BaseAddr = pu8AutoDownloadDRAMBaseAddr;
    if (u32StartAddr != 0xFFFFFFFF)
    {
        pu8BaseAddr += BYTE_PER_WORD * u32StartAddr;
    }

    if (enMode == E_KDRV_XC_AUTODOWNLOAD_TRIGGER_MODE)
    {
        MS_BOOL bFireing = (MS_BOOL)(MHal_XC_R2BYTE(REG_SC_BK67_28_L) & 0x0001);

        // writing data now
        if (bFireing == TRUE)
        {
            mutex_unlock(&_adl_mutex);
            printk("Wait a moment, the lastest datas is writing.\n");
            return FALSE;
        }
        else
        {
            // the lasteset datas is writing done.
            if ((enStatus == E_KDRV_XC_AUTO_DOWNLOAD_FIRED))
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
            MS_BOOL bRet = TRUE;
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
            u32StartAddr = _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].u32StartAddr;
            if (u32StartAddr == 0xFFFFFFFF)
            {
                _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].u32StartAddr = u16StartAddr;
            }
            else
            {
                _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].u32StartAddr = u32StartAddr > u16StartAddr ? u16StartAddr : u32StartAddr;
            }
            //printk("\033[1;35m###[River][%s][%d]### %d %d %lx %d\033[0m\n",__FUNCTION__,__LINE__,pstFormatInfo->enSubClient,pstFormatInfo->bEnableRange,u16StartAddr,u32WriteNum);

            bRet = _Hal_XC_Auto_Download_Format_Hdr_Data(pstFormatInfo->enSubClient, pu8AutoDownloadDRAMBaseAddr, pu8Data, u32Size, u16StartAddr, &_stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].u32Depth);
            mutex_unlock(&_adl_mutex);

            return bRet;
        }
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC:
        {
            MS_PHY phyBaseAddr = 0;
            ST_KDRV_XC_AUTODOWNLOAD_FORMAT_INFO* pstFormatInfo = NULL;
            u32Depth = _stClientInfo[enClient].u32Depth;
            enMode = _stClientInfo[enClient].enMode;
            enStatus = _enAutoDownloadStatus[enClient];
            phyBaseAddr = _stClientInfo[enClient].phyBaseAddr;
            *pu8BaseAddr = NULL;
            u32StartAddr = _stClientInfo[enClient].u32StartAddr;

            pstFormatInfo = (ST_KDRV_XC_AUTODOWNLOAD_FORMAT_INFO *)pParam;
            MS_U16 u16StartAddr = 0;
            MS_U32 u32MaxSize = 0;
            MS_U32 u32MaxAddr = 0;
            MS_U32 u32WriteNum = 0;

            return _Hal_XC_Auto_Download_Format_Xvycc_Data(pstFormatInfo->enSubClient, pu8Data, u32Size, u16StartAddr);
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
    //printf("\033[1;35m###[River][%s][%d]### \033[0m\n",__FUNCTION__,__LINE__);

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
            MS_PHY phyBaseAddr = _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].phyBaseAddr;
            MS_U32 u32Depth = _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].u32Depth;
            MS_U32 u32StartAddr = _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].u32StartAddr;

#ifdef K6_AUTO_DOWNLOADHDR_GAMMA_PATCH
            u32Depth *=2;
#endif
            // set baseaddr
            MHal_XC_W2BYTE(REG_SC_BK67_29_L, ((phyBaseAddr + BYTE_PER_WORD * u32StartAddr) / BYTE_PER_WORD) & 0x0000FFFF);
            MHal_XC_W2BYTEMSK(REG_SC_BK67_2A_L, (((phyBaseAddr + BYTE_PER_WORD * u32StartAddr) / BYTE_PER_WORD) >> 16 & 0x0000FFFF), 0x07FF);

            // set depth
            MHal_XC_W2BYTE(REG_SC_BK67_2B_L, u32Depth);
            MHal_XC_W2BYTE(REG_SC_BK67_2C_L, u32Depth);

            // enable auto download
            MHal_XC_W2BYTEMSK(REG_SC_BK67_28_L, 1, BIT(0));
            break;
        }
        case E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC:
        {
            MS_PHY phyBaseAddr = /*0x003FF80000;*/_stClientInfo[enClient].phyBaseAddr;
            MS_U32 u32Depth = 1024;//_stClientInfo[enClient].u32Depth;
            MS_U32 u32StartAddr =0; _stClientInfo[enClient].u32StartAddr;

            // set baseaddr   _PK_L_SC(0x64,0x16)
            //MHal_XC_W2BYTE(_PK_L_SC(0x64,0x16), ((phyBaseAddr + 16 * u32StartAddr) / BYTE_PER_WORD) & 0x0000FFFF);
            //MHal_XC_W2BYTEMSK(_PK_L_SC(0x64,0x17), (((phyBaseAddr + 16 * u32StartAddr) / BYTE_PER_WORD) >> 16 & 0x0000FFFF), 0x07FF);
            //MHal_XC_W2BYTE(_PK_L_SC(0x67,0x16), ((phyBaseAddr + 16 * u32StartAddr) / BYTE_PER_WORD) & 0x0000FFFF);
            //MHal_XC_W2BYTEMSK(_PK_L_SC(0x67,0x17), (((phyBaseAddr + 16 * u32StartAddr) / BYTE_PER_WORD) >> 16 & 0x0000FFFF), 0x07FF);

            MHal_XC_W2BYTE(REG_SC_BK67_16_L, ((phyBaseAddr + 16 * u32StartAddr) / BYTE_PER_WORD) & 0x0000FFFF);
            MHal_XC_W2BYTEMSK(REG_SC_BK67_17_L, (((phyBaseAddr + 16 * u32StartAddr) / BYTE_PER_WORD) >> 16 & 0x0000FFFF), 0x07FF);




            // set depth
            //MHal_XC_W2BYTE(_PK_L_SC(0x67,0x1A), min(u32Depth,0xFFFF));
            //MHal_XC_W2BYTE(_PK_L_SC(0x67,0x1D), min(u32Depth,0xFFFF)); // length
            MHal_XC_W2BYTE(REG_SC_BK67_1A_L, min(u32Depth,0xFFFF));
            MHal_XC_W2BYTE(REG_SC_BK67_1D_L, min(u32Depth,0xFFFF)); // length
            //MHal_XC_W2BYTE(REG_SC_BK67_2C_L, u32Depth);

            // start address
            //MHal_XC_W2BYTE(_PK_L_SC(0x67,0x20), u32StartAddr);
            MHal_XC_W2BYTE(REG_SC_BK67_20_L, u32StartAddr);

            // enable auto download
            //MHal_XC_W2BYTEMSK(_PK_L_SC(0x67,0x11), BIT(2), BIT(2));
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

static MS_PHY _Hal_XC_GetAutoDownlaodMemOffset(MS_U8 u8Index)
{
    return (AUTO_DOWNLOAD_HDR_3DLUT_SRAM_MAX_ADDR + u8Index * AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR) * BYTE_PER_WORD;
}

BOOL MHal_XC_WriteStoredHDRAutoDownload(MS_U8 *pu8LutData, MS_U32 u32Size, MS_U8 u8Index,
    MS_PHY *pphyFireAdlAddr, MS_U32 *pu32Depth)
{
    MS_PHY phyBaseAddr = 0;

    mutex_lock(&_adl_mutex);

    phyBaseAddr = _stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].phyBaseAddr;

    // copy data into adl mem

    if ((pu8LutData != NULL) && (pu8AutoDownloadDRAMBaseAddr != NULL))
    {
        MS_U8 *pu8DstAddr = pu8AutoDownloadDRAMBaseAddr + _Hal_XC_GetAutoDownlaodMemOffset(u8Index);
        memcpy(pu8DstAddr, pu8LutData, u32Size);
    }
    else
    {
        mutex_unlock(&_adl_mutex);
        printk("%s, malloc memory fail.\n", __FUNCTION__);
        return FALSE;
    }

    *pphyFireAdlAddr = phyBaseAddr + ((u8Index == 0) ? 0 : AUTO_DOWNLOAD_HDR_3DLUT_SRAM_MAX_ADDR + u8Index * AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR) * BYTE_PER_WORD;
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

MS_U8 MHal_XC_CFD_CheckEDIDHasRec2020()
{
    STU_CFDAPI_HDMI_EDID_PARSER stEdidParam;
    MS_U8 u8supportBT2020;

    Mapi_Cfd_inter_HDMI_EDID_Param_Get(&stEdidParam);
    u8supportBT2020 = Mapi_Cfd_inter_HDMI_EDID_Param_CheckSupportBT2020(&stEdidParam);

    return u8supportBT2020;
}

//get the current structure and update it
//Param
//EDIDhasBT2020:
//0: has no BT2020 flags in EDID
//1: has BT2020 flags in EDID

MS_U16 MHal_XC_CFD_SetMCMiddle(MS_U8 u8EDIDhasBT2020, STU_CFDAPI_CFDCall_MAIN_CONTROL *pst_cfdcall_mc)
{
    MS_U16 u16RetVal;
    MS_U16 u16RetValtemp;

    STU_CFDAPI_MAIN_CONTROL stMainControl;
    //STU_CFDAPI_MM_PARSER stMMParam;

     u16RetVal = 0;
     u16RetValtemp = 0;

    //memset(&stMainControl, 0, sizeof(STU_CFDAPI_MAIN_CONTROL));
    //memcpy(&stMainControl,&_stu_Cfd_HDMI_infoFrame_Paramters_out,sizeof(STU_CFDAPI_HDMI_INFOFRAME_PARSER_OUT));

    //get current structures
    Mapi_Cfd_inter_Main_Control_Param_Get(&stMainControl);
    //Mapi_Cfd_inter_MM_Param_Get(&stMMParam);

    //set initial values
    //Mapi_Cfd_Set_Main_Input(&stMainControl);

    //set SDR, BT2020 or BT709
    if ((1 == pst_cfdcall_mc->u8seamless_en) && (0 != pst_cfdcall_mc->u8seamless_colorspace_mode))
    {
        if ((E_CFD_SL_CS_REC2020 == pst_cfdcall_mc->u8seamless_colorspace) && (1 == u8EDIDhasBT2020))
        {
            //printk("\033[1;35m###[%s][%d]### do this !!!!!\033[0m\n",__FUNCTION__,__LINE__);
            Mapi_Cfd_Set_Main_MiddleControl(&stMainControl,u8EDIDhasBT2020);
            Mapi_Cfd_Set_Main_Gamutmapping(&stMainControl,3);
        }
    }

    u16RetValtemp = Mapi_Cfd_inter_Main_Control_Param_Check(&stMainControl,0);
    if (u16RetValtemp == E_CFD_MC_ERR_NOERR)
    {
    }
    else
    {
        printk("\033[1;35m###[%s][%d]### Main_Control_Param_Check fail , errCode: %d !!!!!\033[0m\n",__FUNCTION__,__LINE__,u16RetVal);
    }
    Mapi_Cfd_inter_Main_Control_Param_Set(&stMainControl);

    if ((u16RetVal == 0) && (u16RetValtemp))
    {
        u16RetVal = u16RetValtemp;
    }

    return u16RetVal;
}

//get the current structure and update it
//Param
//EDIDhasBT2020:
//0: has no BT2020 flags in EDID
//1: has BT2020 flags in EDID

MS_U16 MHal_XC_CFD_SetDefaultInput(MS_U8 u8EDIDhasBT2020, STU_CFDAPI_CFDCall_MAIN_CONTROL *pst_cfdcall_mc)
{
    MS_U16 u16RetVal;
    MS_U16 u16RetValtemp;

    STU_CFDAPI_MAIN_CONTROL stMainControl;
    STU_CFDAPI_MM_PARSER stMMParam;

    u16RetVal = 0;
    u16RetValtemp = 0;

    //memset(&stMainControl, 0, sizeof(STU_CFDAPI_MAIN_CONTROL));
    //memcpy(&stMainControl,&_stu_Cfd_HDMI_infoFrame_Paramters_out,sizeof(STU_CFDAPI_HDMI_INFOFRAME_PARSER_OUT));

    //get current structures
    Mapi_Cfd_inter_Main_Control_Param_Get(&stMainControl);
    Mapi_Cfd_inter_MM_Param_Get(&stMMParam);

    //set initial values
    Mapi_Cfd_Set_Main_Input(&stMainControl);

    //set gamut mapping mode
    if ((1 == pst_cfdcall_mc->u8seamless_en) && (0 != pst_cfdcall_mc->u8seamless_colorspace_mode))
    {
        if ((E_CFD_SL_CS_REC2020 == pst_cfdcall_mc->u8seamless_colorspace_mode) && (1 == u8EDIDhasBT2020))
        {
            //set SDR, BT2020 or BT709
            Mapi_Cfd_Set_Main_MiddleControl(&stMainControl,u8EDIDhasBT2020);
            Mapi_Cfd_Set_Main_Gamutmapping(&stMainControl,3);
        }
    }

    if (1 == u8EDIDhasBT2020)
    {
        Mapi_Cfd_Set_MM_SDR_Rec2020(&stMMParam);
    }
    else
    {
        Mapi_Cfd_Set_MM_SDR_Rec709(&stMMParam);
    }

    u16RetValtemp = Mapi_Cfd_inter_Main_Control_Param_Check(&stMainControl,0);
    if (u16RetValtemp == E_CFD_MC_ERR_NOERR)
    {
    }
    else
    {
        printk("\033[1;35m###[%s][%d]### Main_Control_Param_Check fail , errCode: %d !!!!!\033[0m\n",__FUNCTION__,__LINE__,u16RetVal);
    }
    Mapi_Cfd_inter_Main_Control_Param_Set(&stMainControl);

    if ((u16RetVal == 0) && (u16RetValtemp))
    {
        u16RetVal = u16RetValtemp;
    }

    u16RetVal = Mapi_Cfd_inter_MM_Param_Check(&stMMParam);
    if (u16RetVal == E_CFD_MC_ERR_NOERR)
    {
    }
    else
    {
        printk("Mapi_Cfd_inter_MM_Param_Check fail, errCode: %d\n", u16RetVal);
    }
    Mapi_Cfd_inter_MM_Param_Set(&stMMParam);

    if ((u16RetVal == 0) && (u16RetValtemp))
    {
        u16RetVal = u16RetValtemp;
    }

    return u16RetVal;
}


extern STU_CFDAPI_MAIN_CONTROL _stu_CFD_Main_Control;
BOOL MHal_XC_CFDControl(ST_KDRV_XC_CFD_CONTROL_INFO *pstKdrvCFDCtrlInfo)
{

    EN_KDRV_XC_CFD_CTRL_TYPE enCtrlType = pstKdrvCFDCtrlInfo->enCtrlType;
    MS_U32 u32ParamLen = pstKdrvCFDCtrlInfo->u32ParamLen;
    static MS_BOOL bInitStatus[2] = {FALSE, FALSE};

    MS_U16 u16RetVal = 0;
    MS_U8 u8supportBT2020;

    //for seamless
    STU_CFDAPI_CFDCall_MAIN_CONTROL stu_callcfd_mc;

    stu_callcfd_mc.u8seamless_en = 0;
    stu_callcfd_mc.u8seamless_colorspace_mode = 1;
    stu_callcfd_mc.u8seamless_colorspace = E_CFD_SL_CS_REC2020;
    stu_callcfd_mc.u8seamless_hdrmode_mode = 0;

    //printk("\033[31mMHal_XC_CFDControl %d \033[m\n",enCtrlType);
    switch(enCtrlType)
    {
        case E_KDRV_XC_CFD_CTRL_SET_INIT:
        {
            ST_KDRV_XC_CFD_INIT *pstXcCfdInit = (ST_KDRV_XC_CFD_INIT*)pstKdrvCFDCtrlInfo->pParam;
            mutex_lock(&_cfd_mutex);
            _u32NoSignalCount = 0;
            mutex_unlock(&_cfd_mutex);
            //printk("\033[31mCFD initalize \033[m\n");
            //printk("\033[31m  Window is %d \033[m\n", pstXcCfdInit->u8Win);
            //printk("\033[31m  Input source is %d \033[m\n", pstXcCfdInit->u8InputSource);
            memcpy(&_stCfdInit[pstXcCfdInit->u8Win], pstXcCfdInit, sizeof(ST_KDRV_XC_CFD_INIT));

            MHal_XC_EnableHDR(ENABLE);
            bTmoFireEnable = FALSE;
            _bCfdInited = TRUE;
            bInitStatus[pstXcCfdInit->u8Win] = TRUE;
            pstKdrvCFDCtrlInfo->u16ErrCode = E_CFD_MC_ERR_NOERR;
            break;
        }

        case E_KDRV_XC_CFD_CTRL_SET_HDMI:
        {
            ST_KDRV_XC_CFD_HDMI *pstXcCfdHdmi = (ST_KDRV_XC_CFD_HDMI*)pstKdrvCFDCtrlInfo->pParam;
            //printk("\033[31mHDMI input source \033[m\n");
            //printk("\033[31m  Window is %d \033[m\n", pstXcCfdHdmi->u8Win);
            //printk("\033[31m  HDMI %s full range\033[m\n", pstXcCfdHdmi->bIsFullRange?"is":"isn't");
            //printk("\033[31m  AVI infoframe is (%d, %d, %d, %d, %d) \033[m\n", pstXcCfdHdmi->u8PixelFormat, pstXcCfdHdmi->u8Colorimetry, pstXcCfdHdmi->u8ExtendedColorimetry, pstXcCfdHdmi->u8RgbQuantizationRange, pstXcCfdHdmi->u8YccQuantizationRange);
            //printk("\033[31m  HDR infoframe %s exists\033[m\n", pstXcCfdHdmi->bHDRInfoFrameValid?"is":"isn't");
            memcpy(&_stCfdHdmi[pstXcCfdHdmi->u8Win], pstXcCfdHdmi, sizeof(ST_KDRV_XC_CFD_HDMI));

            pstKdrvCFDCtrlInfo->u16ErrCode = E_CFD_MC_ERR_NOERR;
            break;
        }

        case E_KDRV_XC_CFD_CTRL_SET_VGA:
        case E_KDRV_XC_CFD_CTRL_SET_TV:
        case E_KDRV_XC_CFD_CTRL_SET_CVBS:
        case E_KDRV_XC_CFD_CTRL_SET_SVIDEO:
        case E_KDRV_XC_CFD_CTRL_SET_YPBPR:
        case E_KDRV_XC_CFD_CTRL_SET_SCART:
        case E_KDRV_XC_CFD_CTRL_SET_DVI:
        {
            ST_KDRV_XC_CFD_ANALOG *pstXcCfdAnalog = (ST_KDRV_XC_CFD_ANALOG*)pstKdrvCFDCtrlInfo->pParam;
            //printk("\033[31mAnalog input source \033[m\n");
            //printk("\033[31m  Window is %d \033[m\n", pstXcCfdAnalog->u8Win);
            //printk("\033[31m  Color format is %d\033[m\n", pstXcCfdAnalog->u8ColorFormat);
            //printk("\033[31m  Color data format is %d\033[m\n", pstXcCfdAnalog->u8ColorDataFormat);
            //printk("\033[31m  Analog %s full range\033[m\n", pstXcCfdAnalog->bIsFullRange?"is":"isn't");
            //printk("\033[31m  Color primaries is %d\033[m\n", pstXcCfdAnalog->u8ColorPrimaries);
            //printk("\033[31m  Transfer characteristics  is %d\033[m\n", pstXcCfdAnalog->u8TransferCharacteristics);
            //printk("\033[31m  Matrix coefficients is %d\033[m\n", pstXcCfdAnalog->u8MatrixCoefficients);
            memcpy(&_stCfdAnalog[pstXcCfdAnalog->u8Win], pstXcCfdAnalog, sizeof(ST_KDRV_XC_CFD_ANALOG));

            pstKdrvCFDCtrlInfo->u16ErrCode = E_CFD_MC_ERR_NOERR;
            break;
        }

        case E_KDRV_XC_CFD_CTRL_SET_PANEL:
        {
            MS_U16 u16CopiedLength = sizeof(ST_KDRV_XC_CFD_PANEL);
            ST_KDRV_XC_CFD_PANEL *pstCfdPanel = (ST_KDRV_XC_CFD_PANEL*)pstKdrvCFDCtrlInfo->pParam;
            //printk("\033[31mPanel info \033[m\n");
            //printk("\033[31m  R is (%d, %d)\033[m\n", pstCfdPanel->u16Display_Primaries_x[0], pstCfdPanel->u16Display_Primaries_y[0]);
            //printk("\033[31m  G is (%d, %d)\033[m\n", pstCfdPanel->u16Display_Primaries_x[1], pstCfdPanel->u16Display_Primaries_y[1]);
            //printk("\033[31m  B is (%d, %d)\033[m\n", pstCfdPanel->u16Display_Primaries_x[2], pstCfdPanel->u16Display_Primaries_y[2]);
            //printk("\033[31m  W is (%d, %d)\033[m\n", pstCfdPanel->u16White_point_x, pstCfdPanel->u16White_point_y);
            //printk("\033[31m  Luminanceis (%d, %d, %d)\033[m\n", pstCfdPanel->u16MaxLuminance, pstCfdPanel->u16MedLuminance, pstCfdPanel->u16MinLuminance);
            //printk("\033[31m  Linear RGB %d\033[m\n", pstCfdPanel->bLinearRgb);
            if (pstCfdPanel->u32Version > KDRV_XC_CFD_PANEL_VERSION)
            {
                u16CopiedLength = sizeof(ST_KDRV_XC_CFD_PANEL);
            }

            if ((pstCfdPanel->u32Version < KDRV_XC_CFD_PANEL_VERSION) || (pstCfdPanel->u16Length < sizeof(ST_KDRV_XC_CFD_PANEL)))
            {
                u16CopiedLength = pstCfdPanel->u16Length;
            }

            memset(&_stCfdPanel, 0, sizeof(ST_KDRV_XC_CFD_PANEL));
            memcpy(&_stCfdPanel, pstCfdPanel, u16CopiedLength);

            pstKdrvCFDCtrlInfo->u16ErrCode = MHal_XC_CFD_SetPanelParam(pstCfdPanel);
            if (pstKdrvCFDCtrlInfo->u16ErrCode != E_CFD_MC_ERR_NOERR)
            {
                return FALSE;
            }
            break;
        }

        case E_KDRV_XC_CFD_CTRL_SET_HDR:
        {
            ST_KDRV_XC_CFD_HDR *pstCfdHdr = (ST_KDRV_XC_CFD_HDR*)pstKdrvCFDCtrlInfo->pParam;
            //printk("\033[31m SET HDR info \033[m\n");
            //printk("\033[31m  Window is %d \033[m\n", pstCfdHdr->u8Win);
            //printk("\033[31m  HDR type is %d \033[m\n", pstCfdHdr->u8HdrType);
            memcpy(&_stCfdHdr[pstCfdHdr->u8Win], pstCfdHdr, sizeof(ST_KDRV_XC_CFD_HDR));

            if (IS_DOLBY_HDR(pstCfdHdr->u8Win) || IS_OPEN_HDR(pstCfdHdr->u8Win))
            {
                MHal_XC_EnableHDRCLK(TRUE,TRUE);
            }
            else
            {
                //MHal_XC_EnableHDRCLK(FALSE,TRUE);
            }

            pstKdrvCFDCtrlInfo->u16ErrCode = E_CFD_MC_ERR_NOERR;
            break;
        }

        case E_KDRV_XC_CFD_CTRL_SET_EDID:
        {
            int timer = 0;
            MS_BOOL bEnable = TRUE;
            ST_KDRV_XC_CFD_FIRE *pstCfdFire = NULL;
            ST_KDRV_XC_CFD_EDID *pstCfdEdid = (ST_KDRV_XC_CFD_EDID*)pstKdrvCFDCtrlInfo->pParam;
            //printk("u8HDMISinkHDRDataBlockValid is %d\n", pstCfdEdid->u8HDMISinkHDRDataBlockValid);
            //printk("u8HDMISinkEOTF is %d\n", pstCfdEdid->u8HDMISinkEOTF);
            //printk("u8HDMISinkSM is %d\n", pstCfdEdid->u8HDMISinkSM);
            //printk("u8HDMISinkDesiredContentMaxLuminance is %d\n", pstCfdEdid->u8HDMISinkDesiredContentMaxLuminance);
            //printk("u8HDMISinkDesiredContentMaxFrameAvgLuminance is %d\n", pstCfdEdid->u8HDMISinkDesiredContentMaxFrameAvgLuminance);
            //printk("u8HDMISinkDesiredContentMinLuminance is %d\n", pstCfdEdid->u8HDMISinkDesiredContentMinLuminance);
            //printk("u8HDMISinkHDRDataBlockLength is %d\n", pstCfdEdid->u8HDMISinkHDRDataBlockLength);
            //printk("u16Display_Primaries_x[0] is %d\n", pstCfdEdid->u16Display_Primaries_x[0]);
            //printk("u16Display_Primaries_x[1] is %d\n", pstCfdEdid->u16Display_Primaries_x[1]);
            //printk("u16Display_Primaries_x[2] is %d\n", pstCfdEdid->u16Display_Primaries_x[2]);
            //printk("u16Display_Primaries_y[0] is %d\n", pstCfdEdid->u16Display_Primaries_y[0]);
            //printk("u16Display_Primaries_y[1] is %d\n", pstCfdEdid->u16Display_Primaries_y[1]);
            //printk("u16Display_Primaries_y[2] is %d\n", pstCfdEdid->u16Display_Primaries_y[2]);
            //printk("u16White_point_x is %d\n", pstCfdEdid->u16White_point_x);
            //printk("u16White_point_y is %d\n", pstCfdEdid->u16White_point_y);
            //printk("u8HDMISinkEDIDBaseBlockVersion is %d\n", pstCfdEdid->u8HDMISinkEDIDBaseBlockVersion);
            //printk("u8HDMISinkEDIDBaseBlockReversion is %d\n", pstCfdEdid->u8HDMISinkEDIDBaseBlockReversion);
            //printk("u8HDMISinkEDIDCEABlockReversion is %d\n", pstCfdEdid->u8HDMISinkEDIDCEABlockReversion);
            //printk("u8HDMISinkVCDBValid is %d\n", pstCfdEdid->u8HDMISinkVCDBValid);
            //printk("u8HDMISinkSupportYUVFormat is %d\n", pstCfdEdid->u8HDMISinkSupportYUVFormat);
            //printk("u8HDMISinkExtendedColorspace is %d\n", pstCfdEdid->u8HDMISinkExtendedColorspace);
            //printk("u8HDMISinkEDIDValid is %d\n", pstCfdEdid->u8HDMISinkEDIDValid);

            _eCfdOutputType = E_CFD_OUTPUT_SOURCE_HDMI;

            pstKdrvCFDCtrlInfo->u16ErrCode = MHal_XC_CFD_SetEdidParam(pstCfdEdid);
            if (pstKdrvCFDCtrlInfo->u16ErrCode != E_CFD_MC_ERR_NOERR)
            {
                return FALSE;
            }

            pstCfdFire = &_stCfdFire[0];

            // Init IP
            pstKdrvCFDCtrlInfo->u16ErrCode = MHal_XC_CFD_InitIp(pstCfdFire);

            pstKdrvCFDCtrlInfo->u16ErrCode = MHal_XC_CFD_SetMainCtrl(pstCfdFire);

            if (1 == stu_callcfd_mc.u8seamless_en)
            {
                //check the current EDID support BT2020
                //Mapi_Cfd_inter_HDMI_EDID_Param_CheckSupportBT2020
                u8supportBT2020 = MHal_XC_CFD_CheckEDIDHasRec2020();

                //u8supportBT2020 = 1;

                printf("\033[1;35m###[%s][%d]### u8supportBT2020 = %d!!!!\033[0m\n",__FUNCTION__,__LINE__,u8supportBT2020);

                pstKdrvCFDCtrlInfo->u16ErrCode = MHal_XC_CFD_SetDefaultInput(u8supportBT2020,&stu_callcfd_mc);

                if (pstKdrvCFDCtrlInfo->u16ErrCode != E_CFD_MC_ERR_NOERR)
                {
                    return FALSE;
                }
            }

            bTmoFireEnable = FALSE;

            //We move ADL fire to msTmoHandler and check if(stCFD_API_Ctrl.u8AutodownloadFirePermission)
            //while ((MHal_XC_R2BYTE(REG_SC_BK67_28_L) & 0x0001) && timer <100)
            //{
            //    timer++;msleep(50);
            //}
            //if (timer >= 100){
            //    printk("[%s %d] time out\n",__FUNCTION__,__LINE__);
            //}

            mutex_lock(&_cfd_mutex);
            bEnable = _bEnableHDR;
            mutex_unlock(&_cfd_mutex);
            // Fire
            // THIS IS A PATCH
            if (bEnable == FALSE)
            {
                Color_Format_Driver();
                //We move ADL fire to msTmoHandler and check if(stCFD_API_Ctrl.u8AutodownloadFirePermission)
                //if ( 0 == g_DlcParameters.u8Tmo_Mode)
                //if (0 == u8Mhal_xc_TMO_mode)
                //{
                //    MHal_XC_FireAutoDownload(E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR);
                //}

                KApi_XC_MLoad_Fire(E_CLIENT_MAIN_HDR, TRUE);
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
            ST_KDRV_XC_CFD_OSD *pstCfdOsd = (ST_KDRV_XC_CFD_OSD*)pstKdrvCFDCtrlInfo->pParam;
            //printk("\033[31mOSD info \033[m\n");
            //printk("\033[31m  Hue is %d, Saturation is %d, Contrast is %d\033[m\n", pstCfdOsd->u16Hue, pstCfdOsd->u16Saturation, pstCfdOsd->u16Contrast);
            memcpy(&_stCfdOsd, pstCfdOsd, sizeof(ST_KDRV_XC_CFD_OSD));

            pstKdrvCFDCtrlInfo->u16ErrCode = E_CFD_MC_ERR_NOERR;
            break;
        }

        case E_KDRV_XC_CFD_CTRL_SET_DLC:
        {
            ST_KDRV_XC_CFD_DLC *pstCfdDlc = (ST_KDRV_XC_CFD_DLC*)pstKdrvCFDCtrlInfo->pParam;
            //printk("\033[31mDLC info \033[m\n");
            //printk("\033[31m  Customer DLC is %d \033[m\n", pstCfdDlc->bUseCustomerDlcCurve);
            memcpy(&_stCfdDlc, pstCfdDlc, sizeof(ST_KDRV_XC_CFD_DLC));

            pstKdrvCFDCtrlInfo->u16ErrCode = E_CFD_MC_ERR_NOERR;
            break;
        }

        case E_KDRV_XC_CFD_CTRL_SET_LINEAR_RGB:
        {
            int timer = 0;
            MS_BOOL bEnable = TRUE;
            ST_KDRV_XC_CFD_FIRE *pstCfdFire = NULL;
            ST_KDRV_XC_CFD_LINEAR_RGB *pstCfdLinearRgb = (ST_KDRV_XC_CFD_LINEAR_RGB*)pstKdrvCFDCtrlInfo->pParam;
            //printk("\033[31mLinear RGB info \033[m\n");
            //printk("\033[31m  Window is %d \033[m\n", pstCfdLinearRgb->u8Win);
            //printk("\033[31m  Enable is %d\033[m\n", pstCfdLinearRgb->bEnable);
            memcpy(&_stCfdLinearRgb, pstCfdLinearRgb, sizeof(ST_KDRV_XC_CFD_LINEAR_RGB));

            pstCfdFire = &_stCfdFire[pstCfdLinearRgb->u8Win];

            _stCfdOsd.u8OSDUIMode = 0;
            MHal_XC_CFD_SetOsdParam(&_stCfdOsd);

            pstKdrvCFDCtrlInfo->u16ErrCode = MHal_XC_CFD_SetMainCtrl(pstCfdFire);
            if (pstKdrvCFDCtrlInfo->u16ErrCode != E_CFD_MC_ERR_NOERR)
            {
                return FALSE;
            }

            bTmoFireEnable = FALSE;
#if 0
            //We move ADL fire to msTmoHandler and check if(stCFD_API_Ctrl.u8AutodownloadFirePermission)
            //so we do not wait here
            //while ((MHal_XC_R2BYTE(REG_SC_BK67_28_L) & 0x0001) && timer <100)
            //{
            //    timer++;
            //    msleep(50);
            //}
            //if (timer >= 100)
            //{
            //    printk("[%s %d] time out\n",__FUNCTION__,__LINE__);
            //}
#endif
            mutex_lock(&_cfd_mutex);
            bEnable = _bEnableHDR;
            mutex_unlock(&_cfd_mutex);
            // Fire
            // THIS IS A PATCH
            if (bEnable == FALSE)
            {
                //printk("\033[1;35m###[River][%s][%d]### entry open\033[0m\n",__FUNCTION__,__LINE__);
                Color_Format_Driver();
                //We move ADL fire to msTmoHandler and check if(stCFD_API_Ctrl.u8AutodownloadFirePermission)
                //if (0 == u8Mhal_xc_TMO_mode)
                //{
                //    MHal_XC_FireAutoDownload(E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR);
                //}

                KApi_XC_MLoad_Fire(E_CLIENT_MAIN_HDR, TRUE);
                bTmoFireEnable = TRUE;
            }
            else
            {
                //printk("\033[1;35m###[River][%s][%d]### entry dolby\033[0m\n",__FUNCTION__,__LINE__);
                //MHal_XC_W2BYTEMSK(_PK_H_(0x7a,0x4a),0x80,0x00); //reg_b04_out_clamp_en
                MHal_XC_W2BYTEMSK(0x137A94,0,0x3FFF);//reg_b04_uv_offs_0
                bTmoFireEnable = FALSE;
            }

            pstKdrvCFDCtrlInfo->u16ErrCode = E_CFD_MC_ERR_NOERR;
            break;
        }

        case E_KDRV_XC_CFD_CTRL_SET_FIRE:
        {
            int timer = 0;
            MS_BOOL bEnable = TRUE;
            ST_KDRV_XC_CFD_FIRE *pstCfdFire = (ST_KDRV_XC_CFD_FIRE*)pstKdrvCFDCtrlInfo->pParam;
            printk("\033[31mFire info \033[m\n");
            printk("\033[31m  Window is %d \033[m\n", pstCfdFire->u8Win);
            printk("\033[31m  Input source is %d \033[m\n", pstCfdFire->u8InputSource);
            printk("\033[31m  Is RGB bypass is %d \033[m\n", pstCfdFire->bIsRgbBypass);
            printk("\033[31m  Is HD mode is %d, _bEnableHDR %d \033[m\n", pstCfdFire->bIsHdMode,_bEnableHDR);
            XC_KDBG("\033[31m  Update type is %d \033[m\n", pstCfdFire->u8UpdateType);

            if ((pstCfdFire->u8UpdateType == 1) && (bInitStatus[pstCfdFire->u8Win] == TRUE))
            {
                // In init status, skip OSD update action
                pstKdrvCFDCtrlInfo->u16ErrCode = E_CFD_MC_ERR_NOERR;
                XC_KDBG("\033[31m  In init status, skip OSD update action \033[m\n");
                break;
            }

            MS_BOOL bFireChange = FALSE;

            if (pstCfdFire->u8UpdateType == 1)
            {
                pstCfdFire = &_stCfdFire[pstCfdFire->u8Win];
                pstCfdFire->u8UpdateType = 1;
                _stCfdOsd.u8OSDUIMode = 1;
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
                _stCfdOsd.u8OSDUIMode = 0;
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
                pstKdrvCFDCtrlInfo->u16ErrCode = MHal_XC_CFD_SetMmParam(pstCfdFire);
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
            pstKdrvCFDCtrlInfo->u16ErrCode = MHal_XC_CFD_SetOsdParam(&_stCfdOsd);
            if (pstKdrvCFDCtrlInfo->u16ErrCode != E_CFD_MC_ERR_NOERR)
            {
                return FALSE;
            }
            Mapi_Cfd_Kano_DLCIP_CurveMode_Set(pstCfdFire->u8Win, _stCfdDlc.bUseCustomerDlcCurve);

            bTmoFireEnable = FALSE;

            #if 0
            //We move ADL fire to msTmoHandler and check if(stCFD_API_Ctrl.u8AutodownloadFirePermission)
            //so we do not wait here
            //while ((MHal_XC_R2BYTE(REG_SC_BK67_28_L) & 0x0001) && timer <100)
            //{
            //    timer++;
            //    msleep(50);
            //}
            //if (timer >= 100)
            //{
            //    printk("[%s %d] time out\n",__FUNCTION__,__LINE__);
            //}
            #endif
            _bAdlFired = FALSE;

            mutex_lock(&_cfd_mutex);
            bEnable = _bEnableHDR;
            mutex_unlock(&_cfd_mutex);
            // Fire
            // THIS IS A PATCH
            if (bEnable == FALSE)
            {
                if ((1 == stu_callcfd_mc.u8seamless_en) &&  ((0 != stu_callcfd_mc.u8seamless_colorspace_mode) || (0 != stu_callcfd_mc.u8seamless_hdrmode_mode)) )
                {
                    //seamless for BT2020 output
                    //check the current EDID support BT2020
                    //Mapi_Cfd_inter_HDMI_EDID_Param_CheckSupportBT2020
                    u8supportBT2020 = MHal_XC_CFD_CheckEDIDHasRec2020();

                    //u8supportBT2020 = 1;
                    printf("\033[1;35m###[%s][%d]### u8supportBT2020 = %d!!!!\033[0m\n",__FUNCTION__,__LINE__,u8supportBT2020);

                    pstKdrvCFDCtrlInfo->u16ErrCode = MHal_XC_CFD_SetMCMiddle(u8supportBT2020, &stu_callcfd_mc);
                }
                Color_Format_Driver();
                //We move ADL fire to msTmoHandler and check if(stCFD_API_Ctrl.u8AutodownloadFirePermission)
                //if (0 == u8Mhal_xc_TMO_mode)
                //{
                //    MHal_XC_FireAutoDownload(E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR);
                //}

                STU_CFDAPI_Kano_HDRIP stKanoHDRIPParam;
                memset(&stKanoHDRIPParam,0,sizeof(STU_CFDAPI_Kano_HDRIP));
                MS_BOOL bRet;
                bRet = Mhal_Cfd_HDRIP_copy(0, &stKanoHDRIPParam);
                //MHal_XC_EnableHDRCLK((stCurryHDRIPParam.u8HDR_enable_Mode & 0x01),TRUE);

                KApi_XC_MLoad_Fire(E_CLIENT_MAIN_HDR, TRUE);
                //We move ADL fire to msTmoHandler and check if(stCFD_API_Ctrl.u8AutodownloadFirePermission)
                //if(CFD_IS_HDMI(u8HDMI_case)  & (pstCfdFire->u8UpdateType == 0))
                //{
                //    MHal_XC_FireAutoDownload(E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR);
                //}
                //[River]
                //MHal_XC_W2BYTEMSK(0x137A0C,0x9,0xF);
                bTmoFireEnable = TRUE;
            }
            else
            {
                //printk("\033[1;35m###[River][%s][%d]### entry dolby\033[0m\n",__FUNCTION__,__LINE__);
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

            bInitStatus[pstCfdFire->u8Win] = FALSE;
            pstKdrvCFDCtrlInfo->u16ErrCode = E_CFD_MC_ERR_NOERR;

            break;
        }

        case E_KDRV_XC_CFD_CTRL_GET_HDMI_STATUS:
        {
            ST_KDRV_XC_CFD_HDMI *pstXcCfdHdmi = (ST_KDRV_XC_CFD_HDMI*)pstKdrvCFDCtrlInfo->pParam;
            memcpy(&_stCfdHdmi_Out[pstXcCfdHdmi->u8Win], pstXcCfdHdmi, sizeof(ST_KDRV_XC_CFD_HDMI));
            MHal_XC_CFD_GetHdmiOutParam(pstXcCfdHdmi->u8Win);
            memcpy(pstXcCfdHdmi, &_stCfdHdmi_Out[pstXcCfdHdmi->u8Win], sizeof(ST_KDRV_XC_CFD_HDMI));
            //printk("\033[31mHDMI out status \033[m\n");
            //printk("\033[31mHDMI input source \033[m\n");
            //printk("\033[31m  Window is %d \033[m\n", pstXcCfdHdmi->u8Win);
            //printk("\033[31m  HDMI %s full range\033[m\n", pstXcCfdHdmi->bIsFullRange?"is":"isn't");
            //printk("\033[31m  AVI infoframe is (%d, %d, %d, %d, %d) \033[m\n", pstXcCfdHdmi->u8PixelFormat, pstXcCfdHdmi->u8Colorimetry, pstXcCfdHdmi->u8ExtendedColorimetry, pstXcCfdHdmi->u8RgbQuantizationRange, pstXcCfdHdmi->u8YccQuantizationRange);
            //printk("\033[31m  HDR infoframe %s exists\033[m\n", pstXcCfdHdmi->bHDRInfoFrameValid?"is":"isn't");
            //printk("\033[31m  HDR EOTF %d\033[m\n", pstXcCfdHdmi->u8EOTF);
            pstKdrvCFDCtrlInfo->u16ErrCode = E_CFD_MC_ERR_NOERR;
            break;
        }

        case E_KDRV_XC_CFD_CTRL_SET_HDR_ONOFF_SETTING:
        {
            MS_U32* pValue = (MS_U32*)pstKdrvCFDCtrlInfo->pParam;
            MS_U8 u8SetValue = (*pValue)&0xFF;
            Mhal_Cfd_OSD_H2SUI_Set(u8SetValue);
            _stCfdOsd.u8HDR_UI_H2SMode = u8SetValue;
            break;
        }

        default:
            break;
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
        bRet = (MHal_XC_R2BYTEMSK(REG_SC_BK2F_20_L, BIT(2)) ? TRUE: FALSE);
    }

#else
    if(SUB_WINDOW == eWindow)
    {
        bRet = (SC_R2BYTEMSK(psXCInstPri->u32DeviceID, REG_SC_BK10_19_L, BIT(5)) ? TRUE: FALSE);
    }
#endif
    else if(MAIN_WINDOW == u8Window)
    {
        bRet = (MHal_XC_R2BYTEMSK(REG_SC_BK2F_20_L, BIT(1)) ? TRUE: FALSE);
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
}

void MHal_XC_Resume(void)
{
    ///HDR----
    MHal_XC_InitHDR();
    //----

    ///hdr auto download----
    ///TODO:need refine: xvYcc need  resume if it use autodownload in the future.
    //set baseaddr
    MHal_XC_W2BYTE(REG_SC_BK67_29_L, (_stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].phyBaseAddr / BYTE_PER_WORD) & 0x0000FFFF);
    MHal_XC_W2BYTEMSK(REG_SC_BK67_2A_L, ((_stClientInfo[E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR].phyBaseAddr / BYTE_PER_WORD) >> 16 & 0x0000FFFF), 0x07FF);
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
#endif

