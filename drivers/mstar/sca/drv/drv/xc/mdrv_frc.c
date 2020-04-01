////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2010 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    mdrv_frc.c
/// @brief  Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////
#define MDRV_FRC_C

#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/string.h>
#include <linux/wait.h>
#else
#include <string.h>
#endif
// Common Definition
#include "MsCommon.h"
#include "MsOS.h"
#include "mhal_xc_chip_config.h"
#include "xc_hwreg_utility2.h"
#include "drvXC_IOPort.h"
#include "apiXC.h"
#include "drvXC_HDMI_if.h"
#include "drv_sc_ip.h"
#include "mvideo_context.h"

#include "mhal_sc.h"
#include "mhal_frc.h"
#include "mdrv_frc.h"
#if FRC_INSIDE
#include "MHal_FRC_3D_QualityMap.h"
#endif

/******************************************************************************/
/*                      Debug information                                      */
/******************************************************************************/
#define _u16FRCDbgSwitch TRUE

#if defined (MSOS_TYPE_NOS) || defined(MSOS_TYPE_LINUX_KERNEL)

	#define FRC_DBG_MSG(fmt,...) 	//
	#define FRC_PRINT_VAR(var)   	//

#else

	#define FRC_DBG_MSG(fmt,...)                                                                 \
    MS_DEBUG_MSG(do{                                                                                      \
        if(_u16FRCDbgSwitch){                                                                   \
           printf( "[PNL_DBG_MSG]: %s: %d \n"  fmt, __FUNCTION__, __LINE__, ## __VA_ARGS__); \
          }                                                                                  \
      } while(0))

	#define FRC_PRINT_VAR(var)                                                                   \
    do{                                                                                      \
        if(_u16FRCDbgSwitch){                                                                   \
          FRC_DBG_MSG("%30s: %d\n", #var, (var));                                            \
          }                                                                                  \
      }while(0)

#endif

#ifndef UNUSED
	#if defined(MSOS_TYPE_NOS)
	#define UNUSED(x) ((x)=(x))
	#else
	#define UNUSED(x)
	#endif
#endif

#if FRC_INSIDE
MS_U8 tFRCNormalGammaR[] =
{
    0x00,0x00,0x01,0x00,0x02,0x03,0x00,0x04,0x05,
    0x00,0x06,0x07,0x00,0x08,0x09,0x00,0x0A,0x0B,
    0x00,0x0C,0x0D,0x00,0x0E,0x0F,0x00,0x10,0x11,
    0x00,0x12,0x13,0x00,0x14,0x15,0x00,0x16,0x17,
    0x00,0x18,0x19,0x00,0x1A,0x1B,0x00,0x1C,0x1D,
    0x00,0x1E,0x1F,0x00,0x20,0x21,0x00,0x22,0x23,
    0x00,0x24,0x25,0x00,0x26,0x27,0x00,0x28,0x29,
    0x00,0x2A,0x2B,0x00,0x2C,0x2D,0x00,0x2E,0x2F,
    0x00,0x30,0x31,0x00,0x32,0x33,0x00,0x34,0x35,
    0x00,0x36,0x37,0x00,0x38,0x39,0x00,0x3A,0x3B,
    0x00,0x3C,0x3D,0x00,0x3E,0x3F,0x00,0x40,0x41,
    0x00,0x42,0x43,0x00,0x44,0x45,0x00,0x46,0x47,
    0x00,0x48,0x49,0x00,0x4A,0x4B,0x00,0x4C,0x4D,
    0x00,0x4E,0x4F,0x00,0x50,0x51,0x00,0x52,0x53,
    0x00,0x54,0x55,0x00,0x56,0x57,0x00,0x58,0x59,
    0x00,0x5A,0x5B,0x00,0x5C,0x5D,0x00,0x5E,0x5F,
    0x00,0x60,0x61,0x00,0x62,0x63,0x00,0x64,0x65,
    0x00,0x66,0x67,0x00,0x68,0x69,0x00,0x6A,0x6B,
    0x00,0x6C,0x6D,0x00,0x6E,0x6F,0x00,0x70,0x71,
    0x00,0x72,0x73,0x00,0x74,0x75,0x00,0x76,0x77,
    0x00,0x78,0x79,0x00,0x7A,0x7B,0x00,0x7C,0x7D,
    0x00,0x7E,0x7F,0x00,0x80,0x81,0x00,0x82,0x83,
    0x00,0x84,0x85,0x00,0x86,0x87,0x00,0x88,0x89,
    0x00,0x8A,0x8B,0x00,0x8C,0x8D,0x00,0x8E,0x8F,
    0x00,0x90,0x91,0x00,0x92,0x93,0x00,0x94,0x95,
    0x00,0x96,0x97,0x00,0x98,0x99,0x00,0x9A,0x9B,
    0x00,0x9C,0x9D,0x00,0x9E,0x9F,0x00,0xA0,0xA1,
    0x00,0xA2,0xA3,0x00,0xA4,0xA5,0x00,0xA6,0xA7,
    0x00,0xA8,0xA9,0x00,0xAA,0xAB,0x00,0xAC,0xAD,
    0x00,0xAE,0xAF,0x00,0xB0,0xB1,0x00,0xB2,0xB3,
    0x00,0xB4,0xB5,0x00,0xB6,0xB7,0x00,0xB8,0xB9,
    0x00,0xBA,0xBB,0x00,0xBC,0xBD,0x00,0xBE,0xBF,
    0x00,0xC0,0xC1,0x00,0xC2,0xC3,0x00,0xC4,0xC5,
    0x00,0xC6,0xC7,0x00,0xC8,0xC9,0x00,0xCA,0xCB,
    0x00,0xCC,0xCD,0x00,0xCE,0xCF,0x00,0xD0,0xD1,
    0x00,0xD2,0xD3,0x00,0xD4,0xD5,0x00,0xD6,0xD7,
    0x00,0xD8,0xD9,0x00,0xDA,0xDB,0x00,0xDC,0xDD,
    0x00,0xDE,0xDF,0x00,0xE0,0xE1,0x00,0xE2,0xE3,
    0x00,0xE4,0xE5,0x00,0xE6,0xE7,0x00,0xE8,0xE9,
    0x00,0xEA,0xEB,0x00,0xEC,0xED,0x00,0xEE,0xEF,
    0x00,0xF0,0xF1,0x00,0xF2,0xF3,0x00,0xF4,0xF5,
    0x00,0xF6,0xF7,0x00,0xF8,0xF9,0x00,0xFA,0xFB,
    0x00,0xFC,0xFD,0x00,0xFE,0xFF,0x00,0xFF
};

MS_U8 tFRCNormalGammaG[] =
{
    0x00,0x00,0x01,0x00,0x02,0x03,0x00,0x04,0x05,
    0x00,0x06,0x07,0x00,0x08,0x09,0x00,0x0A,0x0B,
    0x00,0x0C,0x0D,0x00,0x0E,0x0F,0x00,0x10,0x11,
    0x00,0x12,0x13,0x00,0x14,0x15,0x00,0x16,0x17,
    0x00,0x18,0x19,0x00,0x1A,0x1B,0x00,0x1C,0x1D,
    0x00,0x1E,0x1F,0x00,0x20,0x21,0x00,0x22,0x23,
    0x00,0x24,0x25,0x00,0x26,0x27,0x00,0x28,0x29,
    0x00,0x2A,0x2B,0x00,0x2C,0x2D,0x00,0x2E,0x2F,
    0x00,0x30,0x31,0x00,0x32,0x33,0x00,0x34,0x35,
    0x00,0x36,0x37,0x00,0x38,0x39,0x00,0x3A,0x3B,
    0x00,0x3C,0x3D,0x00,0x3E,0x3F,0x00,0x40,0x41,
    0x00,0x42,0x43,0x00,0x44,0x45,0x00,0x46,0x47,
    0x00,0x48,0x49,0x00,0x4A,0x4B,0x00,0x4C,0x4D,
    0x00,0x4E,0x4F,0x00,0x50,0x51,0x00,0x52,0x53,
    0x00,0x54,0x55,0x00,0x56,0x57,0x00,0x58,0x59,
    0x00,0x5A,0x5B,0x00,0x5C,0x5D,0x00,0x5E,0x5F,
    0x00,0x60,0x61,0x00,0x62,0x63,0x00,0x64,0x65,
    0x00,0x66,0x67,0x00,0x68,0x69,0x00,0x6A,0x6B,
    0x00,0x6C,0x6D,0x00,0x6E,0x6F,0x00,0x70,0x71,
    0x00,0x72,0x73,0x00,0x74,0x75,0x00,0x76,0x77,
    0x00,0x78,0x79,0x00,0x7A,0x7B,0x00,0x7C,0x7D,
    0x00,0x7E,0x7F,0x00,0x80,0x81,0x00,0x82,0x83,
    0x00,0x84,0x85,0x00,0x86,0x87,0x00,0x88,0x89,
    0x00,0x8A,0x8B,0x00,0x8C,0x8D,0x00,0x8E,0x8F,
    0x00,0x90,0x91,0x00,0x92,0x93,0x00,0x94,0x95,
    0x00,0x96,0x97,0x00,0x98,0x99,0x00,0x9A,0x9B,
    0x00,0x9C,0x9D,0x00,0x9E,0x9F,0x00,0xA0,0xA1,
    0x00,0xA2,0xA3,0x00,0xA4,0xA5,0x00,0xA6,0xA7,
    0x00,0xA8,0xA9,0x00,0xAA,0xAB,0x00,0xAC,0xAD,
    0x00,0xAE,0xAF,0x00,0xB0,0xB1,0x00,0xB2,0xB3,
    0x00,0xB4,0xB5,0x00,0xB6,0xB7,0x00,0xB8,0xB9,
    0x00,0xBA,0xBB,0x00,0xBC,0xBD,0x00,0xBE,0xBF,
    0x00,0xC0,0xC1,0x00,0xC2,0xC3,0x00,0xC4,0xC5,
    0x00,0xC6,0xC7,0x00,0xC8,0xC9,0x00,0xCA,0xCB,
    0x00,0xCC,0xCD,0x00,0xCE,0xCF,0x00,0xD0,0xD1,
    0x00,0xD2,0xD3,0x00,0xD4,0xD5,0x00,0xD6,0xD7,
    0x00,0xD8,0xD9,0x00,0xDA,0xDB,0x00,0xDC,0xDD,
    0x00,0xDE,0xDF,0x00,0xE0,0xE1,0x00,0xE2,0xE3,
    0x00,0xE4,0xE5,0x00,0xE6,0xE7,0x00,0xE8,0xE9,
    0x00,0xEA,0xEB,0x00,0xEC,0xED,0x00,0xEE,0xEF,
    0x00,0xF0,0xF1,0x00,0xF2,0xF3,0x00,0xF4,0xF5,
    0x00,0xF6,0xF7,0x00,0xF8,0xF9,0x00,0xFA,0xFB,
    0x00,0xFC,0xFD,0x00,0xFE,0xFF,0x00,0xFF
};

MS_U8 tFRCNormalGammaB[] =
{
    0x00,0x00,0x01,0x00,0x02,0x03,0x00,0x04,0x05,
    0x00,0x06,0x07,0x00,0x08,0x09,0x00,0x0A,0x0B,
    0x00,0x0C,0x0D,0x00,0x0E,0x0F,0x00,0x10,0x11,
    0x00,0x12,0x13,0x00,0x14,0x15,0x00,0x16,0x17,
    0x00,0x18,0x19,0x00,0x1A,0x1B,0x00,0x1C,0x1D,
    0x00,0x1E,0x1F,0x00,0x20,0x21,0x00,0x22,0x23,
    0x00,0x24,0x25,0x00,0x26,0x27,0x00,0x28,0x29,
    0x00,0x2A,0x2B,0x00,0x2C,0x2D,0x00,0x2E,0x2F,
    0x00,0x30,0x31,0x00,0x32,0x33,0x00,0x34,0x35,
    0x00,0x36,0x37,0x00,0x38,0x39,0x00,0x3A,0x3B,
    0x00,0x3C,0x3D,0x00,0x3E,0x3F,0x00,0x40,0x41,
    0x00,0x42,0x43,0x00,0x44,0x45,0x00,0x46,0x47,
    0x00,0x48,0x49,0x00,0x4A,0x4B,0x00,0x4C,0x4D,
    0x00,0x4E,0x4F,0x00,0x50,0x51,0x00,0x52,0x53,
    0x00,0x54,0x55,0x00,0x56,0x57,0x00,0x58,0x59,
    0x00,0x5A,0x5B,0x00,0x5C,0x5D,0x00,0x5E,0x5F,
    0x00,0x60,0x61,0x00,0x62,0x63,0x00,0x64,0x65,
    0x00,0x66,0x67,0x00,0x68,0x69,0x00,0x6A,0x6B,
    0x00,0x6C,0x6D,0x00,0x6E,0x6F,0x00,0x70,0x71,
    0x00,0x72,0x73,0x00,0x74,0x75,0x00,0x76,0x77,
    0x00,0x78,0x79,0x00,0x7A,0x7B,0x00,0x7C,0x7D,
    0x00,0x7E,0x7F,0x00,0x80,0x81,0x00,0x82,0x83,
    0x00,0x84,0x85,0x00,0x86,0x87,0x00,0x88,0x89,
    0x00,0x8A,0x8B,0x00,0x8C,0x8D,0x00,0x8E,0x8F,
    0x00,0x90,0x91,0x00,0x92,0x93,0x00,0x94,0x95,
    0x00,0x96,0x97,0x00,0x98,0x99,0x00,0x9A,0x9B,
    0x00,0x9C,0x9D,0x00,0x9E,0x9F,0x00,0xA0,0xA1,
    0x00,0xA2,0xA3,0x00,0xA4,0xA5,0x00,0xA6,0xA7,
    0x00,0xA8,0xA9,0x00,0xAA,0xAB,0x00,0xAC,0xAD,
    0x00,0xAE,0xAF,0x00,0xB0,0xB1,0x00,0xB2,0xB3,
    0x00,0xB4,0xB5,0x00,0xB6,0xB7,0x00,0xB8,0xB9,
    0x00,0xBA,0xBB,0x00,0xBC,0xBD,0x00,0xBE,0xBF,
    0x00,0xC0,0xC1,0x00,0xC2,0xC3,0x00,0xC4,0xC5,
    0x00,0xC6,0xC7,0x00,0xC8,0xC9,0x00,0xCA,0xCB,
    0x00,0xCC,0xCD,0x00,0xCE,0xCF,0x00,0xD0,0xD1,
    0x00,0xD2,0xD3,0x00,0xD4,0xD5,0x00,0xD6,0xD7,
    0x00,0xD8,0xD9,0x00,0xDA,0xDB,0x00,0xDC,0xDD,
    0x00,0xDE,0xDF,0x00,0xE0,0xE1,0x00,0xE2,0xE3,
    0x00,0xE4,0xE5,0x00,0xE6,0xE7,0x00,0xE8,0xE9,
    0x00,0xEA,0xEB,0x00,0xEC,0xED,0x00,0xEE,0xEF,
    0x00,0xF0,0xF1,0x00,0xF2,0xF3,0x00,0xF4,0xF5,
    0x00,0xF6,0xF7,0x00,0xF8,0xF9,0x00,0xFA,0xFB,
    0x00,0xFC,0xFD,0x00,0xFE,0xFF,0x00,0xFF
};

MS_U8  *tFRCGammaTab[3] =
{
    // COLOR_TEMP_NORMAL/COLOR_TEMP_USER
    tFRCNormalGammaR,
    tFRCNormalGammaG,
    tFRCNormalGammaB,
};

MS_AutoDownLoad_Info stADLGInfo   = {0,0,FALSE};

static FRCTABLE_INFO _FRCTableInfo;
MS_U8 _u8FRCTabIdx[FRC_IP_NUM_Passive]; // store all TabIdx of all IPs

static void _MDrv_FRC_AddTable(FRCTABLE_INFO *pFRCTableInfo)
{
    _FRCTableInfo = *pFRCTableInfo;
}
EN_FRC_IP_Info _MDrv_FRC_GetTable(MS_U8 u8TabIdx, MS_U8 u8FRCIPIdx)
{
    EN_FRC_IP_Info ip_Info;

    _u8FRCTabIdx[u8FRCIPIdx] = u8TabIdx;
    if (u8TabIdx != FRC_IP_NULL) {
        ip_Info.pIPTable  = _FRCTableInfo.pIPTAB_Info[u8FRCIPIdx].pIPTable;
        ip_Info.u8TabNums = _FRCTableInfo.pIPTAB_Info[u8FRCIPIdx].u8TabNums;
        ip_Info.u8TabIdx = u8TabIdx;
        FRC_DBG_MSG("ip_Info.u8TabNums =0x%x\n",ip_Info.u8TabNums);
    }
    else {
        ip_Info.pIPTable  = 0;
        ip_Info.u8TabNums = 0;
        ip_Info.u8TabIdx = 0;
    }
    return ip_Info;
}
static MS_U8 _MDrv_FRC_GetTableIndex(MS_U16 u16FRCSrcType, MS_U8 u8FRCIPIdx)
{
    if (u16FRCSrcType >=_FRCTableInfo.u8FRC_InputType_Num){
        FRC_DBG_MSG("[FRC]invalid input type\n");
        return FRC_IP_NULL;
    }
    if (u8FRCIPIdx >= _FRCTableInfo.u8FRC_IP_Num){
        FRC_DBG_MSG("[FRC]invalid ip type\n");
        return FRC_IP_NULL;
    }
    return (MS_U8)_FRCTableInfo.pFRC_Map_Aray[u16FRCSrcType * _FRCTableInfo.u8FRC_IP_Num + u8FRCIPIdx];
}
static void _MDrv_FRC_DumpRegTable(EN_FRC_IP_Info* pIP_Info)
{
    MS_U32 u32Addr;
    MS_U8 u8Mask;
    MS_U8 u8Value;


    if (pIP_Info->pIPTable == NULL)
    {
        FRC_DBG_MSG("NULL Table\n");
        return;
    }
    //printf("pIP_Info->u8TabNums =%d\n",pIP_Info->u8TabNums);
    while (1)
    {
        u32Addr = (pIP_Info->pIPTable[0]<<8) + pIP_Info->pIPTable[1];
        u8Mask  = pIP_Info->pIPTable[2];
        u8Value = pIP_Info->pIPTable[REG_ADDR_SIZE+REG_MASK_SIZE+pIP_Info->u8TabIdx];

        if (u32Addr == _END_OF_TBL_) // check end of table
            break;

        u32Addr = REG_FRC_BANK_BASE | (u32Addr & 0xFFFF);
        MDrv_WriteByteMask(u32Addr, u8Value, u8Mask);

        pIP_Info->pIPTable+=(REG_ADDR_SIZE+REG_MASK_SIZE+pIP_Info->u8TabNums);
    }
}
static void _MDrv_FRC_LoadTabelbySrcType(MS_U16 u16FRCSrcType, MS_U8 u8FRCIPIdx)
{
    EN_FRC_IP_Info ip_Info;
    MS_U8 FRCIPtype_size,i;
    MS_U8 u8TabIdx;

    if (u8FRCIPIdx==FRC_IP_ALL)
    {
        FRCIPtype_size=_FRCTableInfo.u8FRC_IP_Num;
        u8FRCIPIdx=0;
    }
    else
        FRCIPtype_size =1;

    for(i=0; i<FRCIPtype_size; i++, u8FRCIPIdx++)
    {
        u8TabIdx = _MDrv_FRC_GetTableIndex(u16FRCSrcType, u8FRCIPIdx);
        ip_Info = _MDrv_FRC_GetTable(u8TabIdx, u8FRCIPIdx);
        _MDrv_FRC_DumpRegTable(&ip_Info);
    }

}
#endif

void MDrv_FRC_ByPass_Enable(MS_BOOL bEnable)
{


    MHal_FRC_IPM_SoftwareReset(ENABLE);
    MHal_FRC_IPM_SoftwareReset(DISABLE);
    MHal_FRC_OPM_SwReset(BIT(9)|BIT(8), 0x00);
    MHal_FRC_OPM_SwReset(0x00, BIT(9)|BIT(8));

    MHal_FRC_SCTOP_FRC_Bypass_Enable(bEnable);
    MHal_CLKGEN_FRC_Bypass_Enable(bEnable);
    MHal_FRC_IPM_RWEn(!bEnable, !bEnable);
    MHal_FRC_IPM_RW_CEN_Select(!bEnable, !bEnable);

    if(!bEnable)
    {
        FRC_DBG_MSG("\\*********************************\n");
        FRC_DBG_MSG("\\Load FRC Normal mode table\n");
        FRC_DBG_MSG("\\*********************************\n");

        //_FRC_NORMAL_ // for Normal case
#if 1
        //TGEN
        MDrv_WriteByte(0x302f00, 0xe9);
        MDrv_WriteByte(0x302f01, 0x01);
        MDrv_WriteByte(0x302f02, 0x64);
        MDrv_WriteByte(0x302f03, 0x04);
        MDrv_WriteByte(0x302f04, 0x97);
        MDrv_WriteByte(0x302f05, 0x08);
        MDrv_WriteByte(0x302f06, 0x5b);
        MDrv_WriteByte(0x302f07, 0x04);
        MDrv_WriteByte(0x302f08, 0x2f);
        MDrv_WriteByte(0x302f09, 0x08);
        MDrv_WriteByte(0x302f0a, 0x00);
        MDrv_WriteByte(0x302f0b, 0x00);
        MDrv_WriteByte(0x302f0c, 0x03);
        MDrv_WriteByte(0x302f0d, 0x00);
        MDrv_WriteByte(0x302f0e, 0x00);
        MDrv_WriteByte(0x302f0f, 0x00);
        MDrv_WriteByte(0x302f10, 0x00);
        MDrv_WriteByte(0x302f11, 0x00);
        MDrv_WriteByte(0x302f12, 0x09);
        MDrv_WriteByte(0x302f13, 0x00);
        MDrv_WriteByte(0x302f14, 0x14);
        MDrv_WriteByte(0x302f15, 0x00);
        MDrv_WriteByte(0x302f16, 0x4b);
        MDrv_WriteByte(0x302f17, 0x04);
        MDrv_WriteByte(0x302f18, 0xa0);
        MDrv_WriteByte(0x302f19, 0x00);
        MDrv_WriteByte(0x302f1a, 0x1f);
        MDrv_WriteByte(0x302f1b, 0x08);
        MDrv_WriteByte(0x302f1c, 0x14);
        MDrv_WriteByte(0x302f1d, 0x00);
        MDrv_WriteByte(0x302f1e, 0x4b);
        MDrv_WriteByte(0x302f1f, 0x04);
        MDrv_WriteByte(0x302f20, 0xa0);
        MDrv_WriteByte(0x302f21, 0x00);
        MDrv_WriteByte(0x302f22, 0x1f);
        MDrv_WriteByte(0x302f23, 0x08);
        MDrv_WriteByte(0x302f2e, 0x00);
        MDrv_WriteByte(0x302f2f, 0x00);
#endif

#if 1
        //OD init
        MDrv_WriteByte(0x302820, 0x2c); // ## od_en [0],  v_en [7], h_en [6]
        MDrv_WriteByte(0x302821, 0x04); // ## od_en [0],  v_en [7], h_en [6]
        MDrv_WriteByte(0x302822, 0x56); //
        MDrv_WriteByte(0x302823, 0x5f); //
        MDrv_WriteByte(0x302824, 0xff); // ## [7:0]  od_act_thrd
        MDrv_WriteByte(0x302825, 0x60); // ## [7:0]  od_act_thrd
#endif

    }
}


void MDrv_FRC_Tx_SetTgen(PMST_PANEL_INFO_t pPanelInfo)
{
    MHal_FRC_TGEN_SetVTotal(pPanelInfo->u16VTotal);
    MHal_FRC_TGEN_SetHTotal(pPanelInfo->u16HTotal);
    MHal_FRC_TGEN_SetFdeStartEndY(pPanelInfo->u16DE_VStart, (pPanelInfo->u16DE_VStart + pPanelInfo->u16Height - 1));
    MHal_FRC_TGEN_SetFdeStartEndX(pPanelInfo->u16HStart, (pPanelInfo->u16HStart + pPanelInfo->u16Width - 1));
    MHal_FRC_TGEN_SetMdeStartEndY(pPanelInfo->u16DE_VStart, (pPanelInfo->u16DE_VStart + pPanelInfo->u16Height - 1));
    MHal_FRC_TGEN_SetMdeStartEndX(pPanelInfo->u16HStart, (pPanelInfo->u16HStart + pPanelInfo->u16Width - 1));
    MHal_FRC_TGEN_SetHSyncStartEndX(pPanelInfo->u8HSyncStart, pPanelInfo->u8HSyncEnd);
    MHal_FRC_TGEN_SetVSyncStartEndY(pPanelInfo->u8VSyncStart, pPanelInfo->u8VSyncEnd);
    MHal_FRC_TGEN_SetVTrigY(pPanelInfo->u16VTrigY);
    MHal_FRC_TGEN_SetVTrigX(pPanelInfo->u16VTrigX);
}

void MDrv_FRC_SetCSC(MS_BOOL bEnable)
{
    MHal_FRC_IPM_Csc(bEnable);
    MHal_FRC_OP2_ColorMatrixEn(bEnable);
}

void MDrv_FRC_SetMemFormat(PFRC_INFO_t pFRCInfo)
{
    MS_U16 u16LineLimit;
    UNUSED(u16LineLimit);

    u16LineLimit = MHal_FRC_IPM_GetLineLimit(pFRCInfo->u8IpMode, pFRCInfo->u16PanelWidth, pFRCInfo->u8MirrorMode);
    pFRCInfo->u16FB_YcountLinePitch = MHal_FRC_IPM_GetYcoutLinePitch(pFRCInfo->u8IpMode, u16LineLimit);

    MHal_FRC_SetMemMode(pFRCInfo->u8MirrorMode, pFRCInfo->u8IpMode);
#if 0

    MHal_FRC_IPM_SetHTotalPixellimit(u16LineLimit);
    MHal_FRC_IPM_SetYCoutLinePitch(pFRCInfo->u16FB_YcountLinePitch);
    MHal_FRC_IPM_SetReadFetchNumber(pFRCInfo->u8IpMode, u16LineLimit);
    MHal_FRC_IPM_SetWriteFetchNumber(pFRCInfo->u8IpMode, u16LineLimit);
    MHal_FRC_IPM_SetVTotalPixellimit(pFRCInfo->u16PanelHeigh);
    MHal_FRC_OPM_enableBaseAdrMr(ENABLE);
    MHal_FRC_IPM_SetMirrorMode(pFRCInfo->u8MirrorMode);
#endif
//=-------------------
///  set frame buffer.
    MHal_FRC_IPM_PacketInitCnt(pFRCInfo->u8IpMode, u16LineLimit);
    MHal_FRC_IPM_SetFrameBufferNum(10);
    pFRCInfo->u32FRC_FrameSize = MHal_FRC_IPM_GetFrameYcout(pFRCInfo->u16PanelHeigh, pFRCInfo->u16FB_YcountLinePitch);
    MHal_FRC_IPM_SetBaseAddr(pFRCInfo->u32FRC_MEMC_MemAddr, pFRCInfo->u32FRC_FrameSize, pFRCInfo->u16FB_YcountLinePitch, pFRCInfo->u8MirrorMode);
    MHal_FRC_OPM_SetBaseAddr(pFRCInfo->u32FRC_MEMC_MemAddr, pFRCInfo->u32FRC_FrameSize);

}

void MDrv_FRC_IPM_Init(PMST_PANEL_INFO_t pPanelInfo, PFRC_INFO_t pFRCInfo)
{

    MHal_FRC_IPM_TestPattern(FALSE, 0);
    MHal_FRC_IPM_SetHTotal(pPanelInfo->u16HTotal);
    MHal_FRC_IPM_SetVTotal(pPanelInfo->u16VTotal);
    MHal_FRC_IPM_HActive(0, pPanelInfo->u16Width);
    MHal_FRC_IPM_VActive(0, pPanelInfo->u16Height);

    MHal_FRC_IPM_SetIp2Mc(ENABLE);
    MHal_FRC_IPM_SetRfifoThr(0x1F10); //0x1F2F for Film 32 mode
    MHal_FRC_IPM_SetWfifoThr(0x4020);
    MHal_FRC_IPM_SetReadLength(0x0820);
    MHal_FRC_IPM_SetWriteLength(0x0040);
    MHal_FRC_IPM_SetRmaskNum(0x00);
    MHal_FRC_IPM_SetWmaskNum(0x00);
    MHal_FRC_IPM_SetLvdsInputMode(0x03);

    MHal_FRC_IPM_SetOsdWinIdx(0x00);
    MHal_FRC_IPM_SetVPulseLineRst(0x0003);
    MHal_FRC_IPM_SetVPulseLoc(0x0001);
    MHal_FRC_IPM_SetLockIntCtrl(0x10);
    MHal_FRC_IPM_SetBlankBoundary(0x00DB);
    MHal_FRC_IPM_SetIpCtrl(ENABLE, 0x03, 0x00);
    MHal_FRC_IPM_SetHRefLock(0x05);

    if(MHal_FRC_IsRGB(pFRCInfo->u8IpMode))
    {
        MHal_FRC_IPM_RWEn(DISABLE,ENABLE);
    }
    else
    {
        MHal_FRC_IPM_RWEn(ENABLE, ENABLE);
    }
    MHal_FRC_IPM_CscDither(FALSE);
    MHal_FRC_IPM_CscRound(TRUE);
    MHal_FRC_IPM_CscSaturation(TRUE);
}

void MDrv_FRC_OPM_Init(void)
{

    MHal_FRC_OPM_SetMlbOutRstCycle(0x08);
    MHal_FRC_OPM_SetFifoTrigThr(0x6060);


    // 0x40, fine tune BW at frameinterleave @ 120Hz.
    // 0x20, decrease BW at 720P frameinterleave
    MHal_FRC_OPM_SetFifoMaxReadReq(0x60);//40);   // 0x50 //0x7f

    MHal_FRC_OPM_FuncEn(0x01, 0x02);
    MHal_FRC_OPM_SetGatingClk(0x20, 0);
}

void MDrv_FRC_Init(PMST_PANEL_INFO_t pPanelInfo, PFRC_INFO_t pFRCInfo)
{
    MS_BOOL isYuv = FALSE;
    MS_U32 u32FrameYcout;
    MS_U16 u16FB_YcountLinePitch, u16LineLimit;
    #if FRC_INSIDE
    FRCTABLE_INFO FRCTableInfo;
    #endif
    MS_U8 u8FRC_PnlType = E_FRC_PNL_TYPE_PASSIVE;

    UNUSED(u32FrameYcout);
    UNUSED(u16FB_YcountLinePitch);
    UNUSED(u16LineLimit);
    UNUSED(u8FRC_PnlType);
    UNUSED(isYuv);

    pFRCInfo->u16PanelWidth = pPanelInfo->u16Width;
    pFRCInfo->u16PanelHeigh = pPanelInfo->u16Height;
    pFRCInfo->u83Dmode = 0;
    pFRCInfo->u8IpMode = FRC_IP_MEM_IP_YC_10_SPECIAL;
    pFRCInfo->u8MirrorMode = 0;

    if(pFRCInfo->u8FRC3DPanelType == E_XC_3D_PANEL_SHUTTER)
        u8FRC_PnlType = E_FRC_PNL_TYPE_ACTIVE;

    //printf("[%s]FRC 3D panel type=%x\n", __FUNCTION__, pFRCInfo->u8FRC3DPanelType);
    //printf("[%s]R2 3D panel type=%x\n", __FUNCTION__, u8FRC_PnlType);

    u16LineLimit = MHal_FRC_IPM_GetLineLimit(pFRCInfo->u8IpMode, pFRCInfo->u16PanelWidth, 0);
    u16FB_YcountLinePitch = MHal_FRC_IPM_GetYcoutLinePitch(pFRCInfo->u8IpMode, u16LineLimit);
    u32FrameYcout = MHal_FRC_IPM_GetFrameYcout(pFRCInfo->u16PanelHeigh, u16FB_YcountLinePitch);
    MHal_FRC_IPM_SetBaseAddr(pFRCInfo->u32FRC_MEMC_MemAddr, u32FrameYcout, u16FB_YcountLinePitch, 0);
    MHal_FRC_OPM_SetBaseAddr(pFRCInfo->u32FRC_MEMC_MemAddr, u32FrameYcout);

    MHal_FRC_LD_SetBaseAddr(pFRCInfo->u32FRC_LD_MemAddr, pFRCInfo->u32FRC_LD_MemSize);
    MHal_FRC_LD_Edge2D_SetBaseAddr(pFRCInfo->u32FRC_LD_MemAddr, pFRCInfo->u32FRC_LD_MemSize);
    MHal_FRC_ME1_SetBaseAddr(pFRCInfo->u32FRC_ME1_MemAddr, pFRCInfo->u32FRC_ME1_MemSize);
    MHal_FRC_ME2_SetBaseAddr(pFRCInfo->u32FRC_ME2_MemAddr, pFRCInfo->u32FRC_ME2_MemSize);
    MHal_FRC_2D3D_Render_SetBaseAddr(pFRCInfo->u32FRC_2D3D_Render_MemAddr, pFRCInfo->u32FRC_2D3D_Render_MemSize);
    MHal_FRC_2D3D_Render_Detection_SetBaseAddr(pFRCInfo->u32FRC_2D3D_Render_Detection_MemAddr, pFRCInfo->u32FRC_2D3D_Render_Detection_MemSize);
    MHal_FRC_Halo_SetBaseAddr(pFRCInfo->u32FRC_Halo_MemAddr, pFRCInfo->u32FRC_Halo_MemSize);

#if FRC_INSIDE
    // R2 command init
    Hal_XC_FRC_R2_Init(u8FRC_PnlType, (pPanelInfo->u16DefaultVFreq/10), 0x00, 0x00);
    Hal_XC_FRC_R2_Set_Timing(60, (pPanelInfo->u16DefaultVFreq/10));
    Hal_XC_FRC_R2_Set_InputFrameSize(pFRCInfo->u16PanelWidth, pFRCInfo->u16PanelHeigh );
    Hal_XC_FRC_R2_Set_OutputFrameSize(pFRCInfo->u16PanelWidth, pFRCInfo->u16PanelHeigh);
    Hal_XC_FRC_R2_Set_Input3DFormat(E_FRC_INPUT_2D, E_FRC_3D_CFG_NORMAL, DISABLE, 0x80);
#endif

    MDrv_FRC_IPM_Init(pPanelInfo, pFRCInfo);

    MHal_FRC_IPM_CheckBoardEn(0); //Check board

    MHal_FRC_IPM_CeLineCountEn(ENABLE);
    MHal_FRC_OPM_CeLineCountEn(ENABLE);

    MDrv_FRC_OPM_Init();

    MHal_FRC_OPM_3dFuncEn(0, (BIT(0) | BIT(3)));
    MHal_FRC_OPM_SetSrcPixNum(pPanelInfo->u16Width);
    MHal_FRC_OPM_SetSrcLineNum(pPanelInfo->u16Height);

    MDrv_FRC_SetMemFormat(pFRCInfo);

    isYuv = !MHal_FRC_IsRGB(pFRCInfo->u8IpMode);
    MHal_FRC_IPM_Csc(isYuv);

    // SNR
    MHal_FRC_SNR_SetPixelHorixontalNum(pPanelInfo->u16Width);
    MHal_FRC_SNR_SetLineVerticalNum(pPanelInfo->u16Height);

    // SCP1
    MHal_FRC_SCP_HSU1_HSP(0, 0);
    MHal_FRC_SCP_HSU1_Init_Position(0);
    MHal_FRC_SCP_HSU1_Scaling_Mode(0);
    MHal_FRC_SCP_HSU1_444to422_Mode(0);
    MHal_FRC_SCP_HSU1_VSU_Scaling_Mode(0);
    MHal_FRC_SCP_HSU1_VSU_Scaling_coef(0);

    // SCP2
    MHal_FRC_SCP_HSU2_HSP(0, 0);
    MHal_FRC_SCP_HSU2_Init_Position(0);
    MHal_FRC_SCP_HSU2_Scaling_Mode(0);

    // TGEN
    MHal_FRC_TGEN_1Clk2PixOut(TRUE);
    MDrv_FRC_Tx_SetTgen(pPanelInfo);

    // OP2
    MHal_FRC_OP2_ColorMatrixEn(isYuv);
    MHal_FRC_OP2_CscDitherEn(FALSE);
    MHal_FRC_OP2_ColorMatrixEn(ENABLE);
    MHal_FRC_OP2_DebugFuncEn((BIT(3) | BIT(5) | BIT(6)) , BIT(5));
    MHal_FRC_OP2_SetDither(0x2d00);

    // SCTOP
    MHal_FRC_SCTOP_SCMI_Bypass_Enable(DISABLE);
    MHal_FRC_SCTOP_FRC_Bypass_Enable(DISABLE);

#if FRC_INSIDE
    if(u8FRC_PnlType == E_FRC_PNL_TYPE_PASSIVE)
    {
        FRCTableInfo.u8FRC_InputType_Num = FRC_INPUTTYPE_NUM_Passive;
        FRCTableInfo.u8FRC_IP_Num =FRC_IP_NUM_Passive;
        FRCTableInfo.pFRC_Map_Aray = (void*)FRC_120_Ln_alt_PASSIVE_1chip_Passive;
        FRCTableInfo.pIPTAB_Info = (void*) FRC_IPTAB_INFO_Passive;
    }
    else //if(u8FRC_PnlType == E_FRC_PNL_TYPE_ACTIVE)
    {
        FRCTableInfo.u8FRC_InputType_Num = FRC_INPUTTYPE_NUM_Active;
        FRCTableInfo.u8FRC_IP_Num =FRC_IP_NUM_Active;
        FRCTableInfo.pFRC_Map_Aray = (void*)FRC_FHD_120_ACTIVE_1Chip_Active;
        FRCTableInfo.pIPTAB_Info = (void*) FRC_IPTAB_INFO_Active;
    }

    _MDrv_FRC_AddTable(&FRCTableInfo);
    MDrv_FRC_Set_3D_QMap(u8FRC_PnlType, E_XC_3D_INPUT_MODE_NONE, E_XC_3D_OUTPUT_MODE_NONE, E_XC_3D_OUTPUT_FI_MODE_NONE);
    //MDrv_FRC_SetGammaTbl(tFRCGammaTab);
 #endif
}


//FRC_3D_FHD_FI_2x_Passive,       // 3,  1920x1080
//FRC_3D_FHD_HH_FI_2x_Passive, // 4,  960x1080
//FRC_3D_FHD_HV_FI_2x_Passive, // 5,  1920x540

void MDrv_FRC_Set_3D_QMap(MS_U8 u8FRC3DPanelType, MS_U16 u16sc_in_3dformat, MS_U16 u16sc_out_3dformat, MS_U8 u83D_FI_out)
{
#if FRC_INSIDE
    MS_U16 u16R2_Hsize = 1920;
    MS_U16 u16R2_Vsize = 1080;
    //g_XC_Pnl_Misc.FRCInfo.u83D_FI_out = u83D_FI_out;

    if (u8FRC3DPanelType == E_FRC_PNL_TYPE_PASSIVE)
    {
        FRC_DBG_MSG("##[%s]FRC Passive Panel\n", __FUNCTION__);
        switch(u16sc_out_3dformat)
        {
            case E_XC_3D_OUTPUT_MODE_NONE:
            {
                if(g_XC_Pnl_Misc.FRCInfo.u8IpMode == FRC_IP_MEM_IP_RGB_10_SPECIAL)
                {
                    FRC_DBG_MSG("##[%s]FRC RGB color space\n", __FUNCTION__);
                    _MDrv_FRC_LoadTabelbySrcType(FRC_2D_RGB_BYPASS_Passive, FRC_IP_ALL);
                }
                else
                {
                    FRC_DBG_MSG("##[%s]FRC YUV color space\n", __FUNCTION__);
                    _MDrv_FRC_LoadTabelbySrcType(FRC_2D_MFC_OFF_YUV_Passive,FRC_IP_ALL);
                }

                if(g_XC_Pnl_Misc.FRCInfo.bFRC)
                {
                    Hal_XC_FRC_R2_Set_InputFrameSize(u16R2_Hsize, u16R2_Vsize);
                    Hal_XC_FRC_R2_Set_Input3DFormat(E_FRC_INPUT_2D, E_FRC_3D_CFG_NORMAL, DISABLE, 0x80);
                }
            }
            break;
            case E_XC_3D_OUTPUT_FRAME_ALTERNATIVE:
            {
                switch(u83D_FI_out)
                {
                    default:
                    case E_XC_3D_OUTPUT_FI_1920x1080:
                        FRC_DBG_MSG("##[%s] FRC_3D_FHD_FI_2x_Passive\n", __FUNCTION__);
                        _MDrv_FRC_LoadTabelbySrcType(FRC_3D_FHD_FI_2x_Passive,FRC_IP_ALL);

                        u16R2_Hsize = 1920;
                        u16R2_Vsize = 1080;
                        //XC_Pnl_Misc.u8IpMode = FRC_IP_MEM_IP_YC_10_SPECIAL;
                    break;
                    case E_XC_3D_OUTPUT_FI_960x1080:
                        FRC_DBG_MSG("##[%s] FRC_3D_FHD_HH_FI_2x_Passive\n", __FUNCTION__);
                        _MDrv_FRC_LoadTabelbySrcType(FRC_3D_FHD_HH_FI_2x_Passive,FRC_IP_ALL);

                        u16R2_Hsize = 960;
                        u16R2_Vsize = 1080;
                        //XC_Pnl_Misc.u8IpMode = FRC_IP_MEM_IP_YUV_10_SPECIAL;
                    break;
                    case E_XC_3D_OUTPUT_FI_1920x540:
                        FRC_DBG_MSG("##[%s] FRC_3D_FHD_HV_FI_2x_Passive\n", __FUNCTION__);
                        _MDrv_FRC_LoadTabelbySrcType(FRC_3D_FHD_HV_FI_2x_Passive,FRC_IP_ALL);

                        u16R2_Hsize = 1920;
                        u16R2_Vsize = 540;
                        //XC_Pnl_Misc.u8IpMode = FRC_IP_MEM_IP_YC_10_SPECIAL;
                    break;
                }

                if(g_XC_Pnl_Misc.FRCInfo.bFRC)
                {
                    Hal_XC_FRC_R2_Set_InputFrameSize(u16R2_Hsize, u16R2_Vsize);
                    Hal_XC_FRC_R2_Set_Input3DFormat(E_FRC_INPUT_3D_FI_1080p, E_FRC_3D_CFG_NORMAL, DISABLE, 0x80);
                }
                //MHal_FRC_SetMemMode(g_XC_Pnl_Misc.FRCInfo.u8MirrorMode, g_XC_Pnl_Misc.FRCInfo.u8IpMode);
            }
            break;

            default:
            case E_XC_3D_OUTPUT_TOP_BOTTOM:
            {
                FRC_DBG_MSG("##[%s] E_FRC_INPUT_3D_TD\n", __FUNCTION__);
                _MDrv_FRC_LoadTabelbySrcType(FRC_3D_UD_2x_Passive,FRC_IP_ALL);

                if(g_XC_Pnl_Misc.FRCInfo.bFRC)
                {
                    Hal_XC_FRC_R2_Set_InputFrameSize(u16R2_Hsize, u16R2_Vsize);
                    Hal_XC_FRC_R2_Set_Input3DFormat(E_FRC_INPUT_3D_TD, E_FRC_3D_CFG_NORMAL, DISABLE, 0x80);
                }

            }
            break;
        }
    }
    else if (u8FRC3DPanelType == E_FRC_PNL_TYPE_ACTIVE)
    {
        FRC_DBG_MSG("##[%s]FRC Active Panel\n", __FUNCTION__);
        switch(u16sc_out_3dformat)
        {
            case E_XC_3D_OUTPUT_MODE_NONE:
            {
                if(g_XC_Pnl_Misc.FRCInfo.u8IpMode == FRC_IP_MEM_IP_RGB_10_SPECIAL)
                {
                    FRC_DBG_MSG("##[%s]FRC RGB color space\n", __FUNCTION__);
                    _MDrv_FRC_LoadTabelbySrcType(FRC_2D_RGB_BYPASS_Active, FRC_IP_ALL);
                }
                else
                {
                    FRC_DBG_MSG("##[%s]FRC YUV color space\n", __FUNCTION__);
                    _MDrv_FRC_LoadTabelbySrcType(FRC_2D_MFC_OFF_YUV_Active,FRC_IP_ALL);
                }

                if(g_XC_Pnl_Misc.FRCInfo.bFRC)
                {
                    Hal_XC_FRC_R2_Set_InputFrameSize(u16R2_Hsize, u16R2_Vsize);
                    Hal_XC_FRC_R2_Set_Input3DFormat(E_FRC_INPUT_2D, E_FRC_3D_CFG_NORMAL, DISABLE, 0x80);
                }
            }
            break;
            case E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF:
            {
                FRC_DBG_MSG("##[%s] E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF\n", __FUNCTION__);

#if 0 // for test
                _MDrv_FRC_LoadTabelbySrcType(FRC_2D_MFC_OFF_YUV_Passive,FRC_IP_ALL);
#else
                _MDrv_FRC_LoadTabelbySrcType(FRC_3D_LR_2x_Active, FRC_IP_ALL);
#endif
                if(g_XC_Pnl_Misc.FRCInfo.bFRC)
                {
                    Hal_XC_FRC_R2_Set_InputFrameSize(u16R2_Hsize, u16R2_Vsize);
                    Hal_XC_FRC_R2_Set_Input3DFormat(E_FRC_INPUT_3D_SBS, E_FRC_3D_CFG_NORMAL, DISABLE, 0x80);
                }
            }
            break;
            case E_XC_3D_OUTPUT_TOP_BOTTOM:
            {
                FRC_DBG_MSG("##[%s] E_XC_3D_OUTPUT_TOP_BOTTOM\n", __FUNCTION__);
                _MDrv_FRC_LoadTabelbySrcType(FRC_3D_UD_2x_Active, FRC_IP_ALL);

                if(g_XC_Pnl_Misc.FRCInfo.bFRC)
                {
                    Hal_XC_FRC_R2_Set_InputFrameSize(u16R2_Hsize, u16R2_Vsize);
                    Hal_XC_FRC_R2_Set_Input3DFormat(E_FRC_INPUT_3D_TD, E_FRC_3D_CFG_NORMAL, DISABLE, 0x80);
                }
            }
            break;
            case E_XC_3D_OUTPUT_FRAME_ALTERNATIVE:
            {
                FRC_DBG_MSG("##[%s] E_XC_3D_OUTPUT_FRAME_ALTERNATIVE\n", __FUNCTION__);
                switch(u83D_FI_out)
                {
                 default:
                 case E_XC_3D_OUTPUT_FI_1920x1080:
                 // TODO: 1. 1920x1080@60 should be FRC_3D_FI_2x_Active
                 // TODO: 2. 1920x1080@24 should be FRC_3D_FI_25x_Active

                     FRC_DBG_MSG("##[%s] FRC_3D_FI_2x_Active\n", __FUNCTION__);
                     _MDrv_FRC_LoadTabelbySrcType(FRC_3D_FI_2x_Active,FRC_IP_ALL);

                     u16R2_Hsize = 1920;
                     u16R2_Vsize = 1080;

                     if(g_XC_Pnl_Misc.FRCInfo.bFRC)
                     {
                         Hal_XC_FRC_R2_Set_InputFrameSize(u16R2_Hsize, u16R2_Vsize);
                         Hal_XC_FRC_R2_Set_Input3DFormat(E_FRC_INPUT_3D_FI_1080p, E_FRC_3D_CFG_NORMAL, DISABLE, 0x80);
                     }
                 break;
                 case E_XC_3D_OUTPUT_FI_1280x720:
                     FRC_DBG_MSG("##[%s] FRC_3D_720p_1x_Active\n", __FUNCTION__);
                     _MDrv_FRC_LoadTabelbySrcType(FRC_3D_720p_1x_Active, FRC_IP_ALL);

                     u16R2_Hsize = 1280;
                     u16R2_Vsize = 720;

                     if(g_XC_Pnl_Misc.FRCInfo.bFRC)
                     {
                         Hal_XC_FRC_R2_Set_InputFrameSize(u16R2_Hsize, u16R2_Vsize);
                         Hal_XC_FRC_R2_Set_Input3DFormat(E_FRC_INPUT_3D_FI_1080p, E_FRC_3D_CFG_NORMAL, DISABLE, 0x80);
                     }
                 break;
                }
            }
            break;
            case E_XC_3D_OUTPUT_LINE_ALTERNATIVE:
            {
                FRC_DBG_MSG("##[%s] E_XC_3D_OUTPUT_LINE_ALTERNATIVE\n", __FUNCTION__);

                _MDrv_FRC_LoadTabelbySrcType(FRC_3D_LnAlt_2x_Active, FRC_IP_ALL);

                if(g_XC_Pnl_Misc.FRCInfo.bFRC)
                {
                    Hal_XC_FRC_R2_Set_InputFrameSize(u16R2_Hsize, u16R2_Vsize);
                    Hal_XC_FRC_R2_Set_Input3DFormat(E_FRC_INPUT_3D_LA, E_FRC_3D_CFG_NORMAL, DISABLE, 0x80);
                }
            }
            break;

            default:
            break;
        }
    }
    else
    {
        ;
    }
#else
    UNUSED(u8FRC3DPanelType); UNUSED(u16sc_in_3dformat); UNUSED(u16sc_out_3dformat); UNUSED(u83D_FI_out);
#endif
}

#if FRC_INSIDE
void MDrv_FRC_AutoDownLoad_init(MS_PHYADDR PhyAddr, MS_U32 u32BufByteLen)
{
    FRC_DBG_MSG("ADL start addr:0x%lx, Length:0x%lx\n", PhyAddr, u32BufByteLen);
    W2BYTEMSK(L_FRC_SC30(0x0B), 0x0, BIT(12));
    stADLGInfo.PhyAddr = PhyAddr;
    stADLGInfo.u16MaxCmdCnt = (MS_U16)(u32BufByteLen/MS_ALG_CMD_LEN);
}

void MDrv_FRC_ADLG_Trigger(FRC_CLIENT_TABLE client,MS_PHYADDR startAddr, MS_U16 u16CmdCnt)
{
    FRC_DBG_MSG("start Addr:0x%lx, cmd cnt:%u\n",startAddr, u16CmdCnt);

    Hal_FRC_ADLG_set_base_addr(client,startAddr);
    Hal_FRC_ADLG_set_depth(client,u16CmdCnt);
    Hal_FRC_ADLG_set_dma(client,MS_ALG_CMD_LEN);
    Hal_FRC_ADLG_set_on_off(client,ENABLE);
}
void MDrv_FRC_ADLG_WritetoDRAM(FRC_CLIENT_TABLE client,MS_U8 *pR, MS_U8 *pG, MS_U8 *pB, MS_U16 u16Count, MS_U16 *pMaxGammaValue)
{
    MS_FRC_ADLG_TBL *pADLG_Addr;
    MS_PHYADDR DstAddr;
    MS_U16 i,j;
    pMaxGammaValue[0] = pMaxGammaValue [1] = pMaxGammaValue [2] = 0;
    DstAddr = stADLGInfo.PhyAddr;
    pADLG_Addr = (MS_FRC_ADLG_TBL *)MS_PA2KSEG1(DstAddr);
    switch(client)
    {
        case E_FRC_op2_gamma_table:
            for (i = 0, j = 0; i < u16Count; i+=2 , j+=3)
            {
                    pADLG_Addr[i].u32R = (pR[j] & 0x0F) | (pR[j+1] << 4);
                    pADLG_Addr[i].u32G = (pG[j] & 0x0F) | (pG[j+1] << 4);
                    pADLG_Addr[i].u32B = (pB[j] & 0x0F) | (pB[j+1] << 4);
                    pADLG_Addr[i].u32Enable = BIT(0) | BIT(1) | BIT(2);

                    if(pMaxGammaValue[0] < (MS_U16)pADLG_Addr[i].u32R)
                    {
                        pMaxGammaValue[0] = (MS_U16)pADLG_Addr[i].u32R;
                    }
                    if(pMaxGammaValue[1] < (MS_U16)pADLG_Addr[i].u32G)
                    {
                        pMaxGammaValue[1] = (MS_U16)pADLG_Addr[i].u32G;
                    }
                    if(pMaxGammaValue[2] < (MS_U16)pADLG_Addr[i].u32B)
                    {
                        pMaxGammaValue[2] = (MS_U16)pADLG_Addr[i].u32B;
                    }
                    pADLG_Addr[i+1].u32R = (pR[j] >>4) | (pR[j+2] << 4);
                    pADLG_Addr[i+1].u32G = (pG[j] >>4) | (pG[j+2] << 4);
                    pADLG_Addr[i+1].u32B = (pB[j] >>4) | (pB[j+2] << 4);
                    pADLG_Addr[i+1].u32Enable = BIT(0) | BIT(1) | BIT(2);

                    if(pMaxGammaValue[0] <(MS_U16) pADLG_Addr[i+1].u32R)
                    {
                        pMaxGammaValue[0] = (MS_U16)pADLG_Addr[i+1].u32R;
                    }
                    if(pMaxGammaValue[1] < (MS_U16)pADLG_Addr[i+1].u32G)
                    {
                        pMaxGammaValue[1] = (MS_U16)pADLG_Addr[i+1].u32G;
                    }
                    if(pMaxGammaValue[2] < (MS_U16)pADLG_Addr[i+1].u32B)
                    {
                        pMaxGammaValue[2] = (MS_U16)pADLG_Addr[i+1].u32B;
                    }

            }
        break;
        case E_FRC_od_table:
            break;
        case E_FRC_ld_table:
            break;
        }
}
void MDrv_FRC_ADLG_Fire(FRC_CLIENT_TABLE client,MS_U8 *pR, MS_U8 *pG, MS_U8 *pB, MS_U16 u16Count, MS_U16 *pMaxGammaValue)
{
    MS_PHYADDR CmdBufAddr;
    MDrv_FRC_ADLG_WritetoDRAM(client,pR, pG, pB, u16Count, pMaxGammaValue);
    MsOS_FlushMemory();
    CmdBufAddr = stADLGInfo.PhyAddr;
    MDrv_FRC_ADLG_Trigger(client,stADLGInfo.PhyAddr,u16Count);
}
void _MDrv_FRC_SetMaxGammaValue(MS_U8 u8Channel, MS_U16 u16MaxGammaValue)
{
    W2BYTEMSK((L_FRC_SC30(0x2C)+ 4 * u8Channel), u16MaxGammaValue, 0xFFF);
    W2BYTEMSK((L_FRC_SC30(0x2D)+ 4 * u8Channel), u16MaxGammaValue, 0xFFF);
}
void MDrv_FRC_SetGammaTbl(MS_U8* pu8GammaTab[3])
{
    //MS_U8 u8Channel;
    MS_U16 pMaxGammaValue[3] ;
    MS_U16 u16NumOfLevel = 256; //FRC gamma only support 256 entry
    MS_U8 u8Channel;
    MDrv_FRC_ADLG_Fire(E_FRC_op2_gamma_table,pu8GammaTab[0], pu8GammaTab[1], pu8GammaTab[2], u16NumOfLevel, pMaxGammaValue);

    for(u8Channel = 0; u8Channel < 3; u8Channel ++)
    {
        _MDrv_FRC_SetMaxGammaValue(u8Channel, pMaxGammaValue[u8Channel]);
    }
}
#endif

void MDrv_FRC_OPM_SetBaseOfset(FRC_INFO_t *FRCInfo, E_XC_3D_OUTPUT_MODE u16out_3dformat)
{
    if ((u16out_3dformat == E_XC_3D_OUTPUT_TOP_BOTTOM) ||
        (u16out_3dformat == E_XC_3D_OUTPUT_MODE_NONE))
    {
        MHal_FRC_OPM_SetBaseOffset_Data0(FRCInfo->u32FRC_FrameSize/2);
        MHal_FRC_OPM_SetBaseOffset_Data1(FRCInfo->u32FRC_FrameSize/2);
        MHal_FRC_OPM_SetBaseOffset_Me0(FRCInfo->u32FRC_FrameSize/2);
        MHal_FRC_OPM_SetBaseOffset_Me1(FRCInfo->u32FRC_FrameSize/2);
        MHal_FRC_OPM_SetBaseOffset_Me3(FRCInfo->u32FRC_FrameSize/2);
        MHal_FRC_OPM_SetBaseOffset_Mr1(FRCInfo->u32FRC_FrameSize/2);
    }
    else if (u16out_3dformat == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE)
    {
        MHal_FRC_OPM_SetBaseOffset_Data0(FRCInfo->u32FRC_FrameSize);
        MHal_FRC_OPM_SetBaseOffset_Data1(FRCInfo->u32FRC_FrameSize);
        MHal_FRC_OPM_SetBaseOffset_Me0(FRCInfo->u32FRC_FrameSize);
        MHal_FRC_OPM_SetBaseOffset_Me1(FRCInfo->u32FRC_FrameSize);
        MHal_FRC_OPM_SetBaseOffset_Me3(FRCInfo->u32FRC_FrameSize);
        MHal_FRC_OPM_SetBaseOffset_Mr1(FRCInfo->u32FRC_FrameSize);

        if ((FRCInfo->u83D_FI_out== E_XC_3D_OUTPUT_FI_1920x1080) ||
           (FRCInfo->u83D_FI_out == E_XC_3D_OUTPUT_FI_960x1080))
        {
            MHal_FRC_OPM_SetBaseOffset_Da0_L((FRCInfo->u32FRC_FrameSize + FRCInfo->u16FB_YcountLinePitch));
            MHal_FRC_OPM_SetBaseOffset_Da1_L((FRCInfo->u32FRC_FrameSize + FRCInfo->u16FB_YcountLinePitch));
            MHal_FRC_OPM_SetBaseOffset_Me0_L((FRCInfo->u32FRC_FrameSize + FRCInfo->u16FB_YcountLinePitch));
            MHal_FRC_OPM_SetBaseOffset_Me1_L((FRCInfo->u32FRC_FrameSize + FRCInfo->u16FB_YcountLinePitch));
            MHal_FRC_OPM_SetBaseOffset_Me3_L((FRCInfo->u32FRC_FrameSize + FRCInfo->u16FB_YcountLinePitch));
            MHal_FRC_OPM_SetBaseOffset_Mr1_L((FRCInfo->u32FRC_FrameSize + FRCInfo->u16FB_YcountLinePitch));
        }
        else if(FRCInfo->u83D_FI_out == E_XC_3D_OUTPUT_FI_1920x540)
        {
            MHal_FRC_OPM_SetBaseOffset_Da0_L(0);
            MHal_FRC_OPM_SetBaseOffset_Da1_L(0);
            MHal_FRC_OPM_SetBaseOffset_Me0_L(0);
            MHal_FRC_OPM_SetBaseOffset_Me1_L(0);
            MHal_FRC_OPM_SetBaseOffset_Me3_L(0);
            MHal_FRC_OPM_SetBaseOffset_Mr1_L(0);
        }
    }
    else if(u16out_3dformat == E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF)
    {
        MHal_FRC_OPM_SetBaseOffset_Data0(FRCInfo->u16FB_YcountLinePitch/2);
        MHal_FRC_OPM_SetBaseOffset_Data1(FRCInfo->u16FB_YcountLinePitch/2);
        MHal_FRC_OPM_SetBaseOffset_Me0(FRCInfo->u16FB_YcountLinePitch/2);
        MHal_FRC_OPM_SetBaseOffset_Me1(FRCInfo->u16FB_YcountLinePitch/2);
        MHal_FRC_OPM_SetBaseOffset_Me3(FRCInfo->u16FB_YcountLinePitch/2);
        MHal_FRC_OPM_SetBaseOffset_Mr1(FRCInfo->u16FB_YcountLinePitch/2);
    }
}





