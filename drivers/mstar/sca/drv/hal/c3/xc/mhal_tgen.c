////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
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
#define MHAL_TGEN_C
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#if defined(MSOS_TYPE_LINUX_KERNEL)
#include <linux/wait.h>
#include <linux/string.h>
#endif

// Common Definition
#include "MsCommon.h"
#include "MsIRQ.h"
#include "MsOS.h"
#include "mhal_xc_chip_config.h"

// Internal Definition
#include "drvXC_IOPort.h"
#include "xc_Analog_Reg.h"
#include "xc_hwreg_utility2.h"
#include "hwreg_dvi_atop.h"//alex_tung
#include "hwreg_adc_atop.h"
#include "hwreg_adc_dtop.h"
#include "hwreg_adc_dtopb.h"
#include "hwreg_hdmi.h"
#include "apiXC.h"
#include "hwreg_sc.h"

#include "mhal_tgen_tbl.h"
#include "mhal_tgen_tbl.c"
#include "mhal_tgen.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define MS_TGEN_DBG(x)  // x

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------
typedef enum
{
    EN_TGEN_CLK_GEN,
    EN_TGEN_SC,
    EN_TGEN_HDMITX_MISC,
}TGEN_TABLE_TYPE;

typedef struct
{
    MS_U8 *pTable;
    MS_U8 u8TabCols;
    MS_U8 u8TabRows;
    TGEN_TABLE_TYPE enType;
    SCALER_WIN eWindow;
} TGEN_TAB_Info;

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
TGEN_TAB_Info gSCx_TabInfo[MAX_WINDOW] =
{
    {0, 0, 0, 0, MAX_WINDOW},
    {0, 0, 0, 0, MAX_WINDOW},
#ifdef MULTI_SCALER_SUPPORTED
    {0, 0, 0, 0, MAX_WINDOW},
    {0, 0, 0, 0, MAX_WINDOW},
    {0, 0, 0, 0, MAX_WINDOW},
    {0, 0, 0, 0, MAX_WINDOW},
#endif
};

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static void Hal_SC_TGen_LoadTable(TGEN_TAB_Info *pTab_info)
{
    MS_U8 i;
    MS_U32 u32Addr;
    MS_U8  u8Mask, u8Val;
    MS_U8 *pTable;

    if(pTab_info->pTable == NULL)
        return;

    pTable = pTab_info->pTable;
    for(i=0; i<pTab_info->u8TabRows; i++)
    {
        u32Addr =  (MS_U32)( (pTable[0]<<8) + pTable[1] );
        u8Mask   = pTable[2];
        u8Val   =  pTable[3];

        if(pTab_info->enType == EN_TGEN_CLK_GEN)
        {
            if(pTab_info->eWindow == SC2_MAIN_WINDOW)
            {
                continue;
            }
            u32Addr += 0x100000; // 0x100B00
        }
        else if(pTab_info->enType == EN_TGEN_SC)
        {
            if(pTab_info->eWindow == SC1_MAIN_WINDOW)
                u32Addr += 0x130000;
            else if(pTab_info->eWindow == SC2_MAIN_WINDOW)
                u32Addr = 0x130000 + u32Addr - 0x8000 + 0x4000;
            else
                u32Addr = 0x130000 + u32Addr - 0x8000;
        }
        else if(pTab_info->enType == EN_TGEN_HDMITX_MISC)
        {
            u32Addr += 0x150000; //0x151400
        }

        MDrv_WriteByteMask(u32Addr, u8Val, u8Mask);

        //printf("Addr:%08x, Mask:%02x, vla:%02x \r\n", u32Addr, u8Mask, u8Val);
        pTable += pTab_info->u8TabCols; // next
    }

}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
MS_BOOL Hal_SC_Get_TGen_SC_Tbl_H_Bacporch(SCALER_WIN eWindow, MS_U16 *u16Hbackporch )
{
    MS_U16 i;
    MS_U32 u32Addr;
    MS_U8  u8Mask, u8Val;
    MS_U8  *pTable;
    TGEN_TAB_Info *pTab_info;
    MS_U16 u16HDE_Start = 0;
    MS_U16 u16HSync_End = 0;

    *u16Hbackporch = 0;

    if(gSCx_TabInfo[eWindow].eWindow == eWindow && gSCx_TabInfo[eWindow].pTable)
    {
        pTab_info = &gSCx_TabInfo[eWindow];
        pTable = pTab_info->pTable;

        for(i=0; i<pTab_info->u8TabRows; i++)
        {
            u32Addr =  (MS_U32)( (pTable[0]<<8) + pTable[1] );
            u8Mask   = pTable[2];
            u8Val   =  pTable[3];

            if(u32Addr == REG_TGEN_SC1_VOP_BK90_01_L)
            {
                u16HSync_End = (MS_U16)u8Val;
            }
            else if(u32Addr == REG_TGEN_SC1_VOP_BK90_01_H)
            {
                u16HSync_End |= (((MS_U16)u8Val) << 8);
            }
            else if(u32Addr == REG_TGEN_SC1_VOP_BK90_04_L)
            {
                u16HDE_Start = (MS_U16)u8Val;
            }
            else if(u32Addr == REG_TGEN_SC1_VOP_BK90_04_H)
            {
                u16HDE_Start |= (((MS_U16)u8Val) << 8);
            }

            pTable += pTab_info->u8TabCols; // next
        }

        *u16Hbackporch = u16HDE_Start - u16HSync_End + 1;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}



MS_BOOL Hal_SC_Get_TGen_SC_Tbl_HV_Start(SCALER_WIN eWindow, MS_U16 *u16Hstart, MS_U16 *u16Vstart )
{
    MS_BOOL bret = FALSE;
    TGEN_TAB_Info *pTab_info;
    MS_U16 i;
    MS_U32 u32Addr;
    MS_U8  u8Mask, u8Val;
    MS_U8  *pTable;

    *u16Hstart = 0;
    *u16Vstart = 0;

    if(gSCx_TabInfo[eWindow].eWindow == eWindow)
    {
        pTab_info = &gSCx_TabInfo[eWindow];
        pTable = pTab_info->pTable;

        for(i=0; i<pTab_info->u8TabRows; i++)
        {
            u32Addr =  (MS_U32)( (pTable[0]<<8) + pTable[1] );
            u8Mask   = pTable[2];
            u8Val   =  pTable[3];

            //printf("Addr:%08x, Mask:%02x, vla:%02x \n", (int)u32Addr, (int)u8Mask, (int)u8Val);

            if(u32Addr == REG_TGEN_SC1_VOP_BK90_0A_L)
            {
                *u16Vstart = (MS_U16)u8Val;
            }
            else if(u32Addr == REG_TGEN_SC1_VOP_BK90_0A_H)
            {
                *u16Vstart |= (((MS_U16)u8Val) << 8);
            }

            else if(u32Addr == REG_TGEN_SC1_VOP_BK90_08_L)
            {
                *u16Hstart = (MS_U16)u8Val;
            }
            else if(u32Addr == REG_TGEN_SC1_VOP_BK90_08_H)
            {
                *u16Hstart |= (((MS_U16)u8Val) << 8);
            }

            pTable += pTab_info->u8TabCols; // next
        }

        bret = TRUE;
        MS_TGEN_DBG(printf("eWin:%d Hstar:%x, Vstar:%x \n",eWindow, *u16Hstart, *u16Vstart));
    }
    else
    {
        bret = FALSE;
    }

    return bret;
}

void Hal_SC_TGen_SetTiming(SCALER_WIN eWindow, XC_TGEN_TIMING_TYPE enType, MS_BOOL b10Bits)
{
    TGEN_TAB_Info stCLKGen_Tbl;
    TGEN_TAB_Info stSC_Tbl;
    TGEN_TAB_Info stHDMITx_MISC_Tbl;
    TGEN_TAB_Info stSynth_Tbl;

    memset(&stCLKGen_Tbl, 0, sizeof(TGEN_TAB_Info));
    memset(&stSC_Tbl, 0, sizeof(TGEN_TAB_Info));
    memset(&stHDMITx_MISC_Tbl, 0, sizeof(TGEN_TAB_Info));

    //printf("Hal_SC_TGen_SetTiming: %x %x \r\n", eWindow, enType);
    stSC_Tbl.eWindow = eWindow;
    stCLKGen_Tbl.eWindow = eWindow;
    stHDMITx_MISC_Tbl.eWindow = eWindow;
    stSynth_Tbl.eWindow = eWindow;
    switch(enType)
    {
    case E_XC_TGEN_VE_480_I_60:
        stCLKGen_Tbl.pTable = (void *)MST_TGEN_VE_480I_60_INIT_CLKGEN_TBL;
        stCLKGen_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_VE_480I_60_INIT_CLKGEN_NUMS*REG_DATA_SIZE;
        stCLKGen_Tbl.u8TabRows = sizeof(MST_TGEN_VE_480I_60_INIT_CLKGEN_TBL)/stCLKGen_Tbl.u8TabCols;
        stCLKGen_Tbl.enType = EN_TGEN_CLK_GEN;

        stSC_Tbl.pTable = (void *)MST_TGEN_VE_480I_60_INIT_SC1_TBL;
        stSC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_VE_480I_60_INIT_SC1_NUMS*REG_DATA_SIZE;
        stSC_Tbl.u8TabRows = sizeof(MST_TGEN_VE_480I_60_INIT_SC1_TBL)/stCLKGen_Tbl.u8TabCols;
        stSC_Tbl.enType = EN_TGEN_SC;

        stHDMITx_MISC_Tbl.pTable = (void *)MST_TGEN_VE_480I_60_INIT_HDMITx_MISC_TBL;
        stHDMITx_MISC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_VE_480I_60_INIT_HDMITx_MISC_NUMS*REG_DATA_SIZE;
        stHDMITx_MISC_Tbl.u8TabRows = sizeof(MST_TGEN_VE_480I_60_INIT_HDMITx_MISC_TBL)/stCLKGen_Tbl.u8TabCols;
        stHDMITx_MISC_Tbl.enType = EN_TGEN_HDMITX_MISC;

        if(b10Bits)
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_VE_480I_60_INIT_HDMITx_SYNTH_10_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_VE_480I_60_INIT_HDMITx_SYNTH_10_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_VE_480I_60_INIT_HDMITx_SYNTH_10_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }
        else
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_VE_480I_60_INIT_HDMITx_SYNTH_8_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_VE_480I_60_INIT_HDMITx_SYNTH_8_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_VE_480I_60_INIT_HDMITx_SYNTH_8_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }

        break;

    case E_XC_TGEN_VE_576_I_50:
        stCLKGen_Tbl.pTable = (void *)MST_TGEN_VE_576I_50_INIT_CLKGEN_TBL;
        stCLKGen_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_VE_576I_50_INIT_CLKGEN_NUMS*REG_DATA_SIZE;
        stCLKGen_Tbl.u8TabRows = sizeof(MST_TGEN_VE_576I_50_INIT_CLKGEN_TBL)/stCLKGen_Tbl.u8TabCols;
        stCLKGen_Tbl.enType = EN_TGEN_CLK_GEN;

        stSC_Tbl.pTable = (void *)MST_TGEN_VE_576I_50_INIT_SC1_TBL;
        stSC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_VE_576I_50_INIT_SC1_NUMS*REG_DATA_SIZE;
        stSC_Tbl.u8TabRows = sizeof(MST_TGEN_VE_576I_50_INIT_SC1_TBL)/stCLKGen_Tbl.u8TabCols;
        stSC_Tbl.enType = EN_TGEN_SC;

        stHDMITx_MISC_Tbl.pTable = (void *)MST_TGEN_VE_576I_50_INIT_HDMITx_MISC_TBL;
        stHDMITx_MISC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_VE_576I_50_INIT_HDMITx_MISC_NUMS*REG_DATA_SIZE;
        stHDMITx_MISC_Tbl.u8TabRows = sizeof(MST_TGEN_VE_576I_50_INIT_HDMITx_MISC_TBL)/stCLKGen_Tbl.u8TabCols;
        stHDMITx_MISC_Tbl.enType = EN_TGEN_HDMITX_MISC;

        if(b10Bits)
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_VE_576I_50_INIT_HDMITx_SYNTH_10_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_VE_576I_50_INIT_HDMITx_SYNTH_10_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_VE_576I_50_INIT_HDMITx_SYNTH_10_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }
        else
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_VE_576I_50_INIT_HDMITx_SYNTH_8_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_VE_576I_50_INIT_HDMITx_SYNTH_10_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_VE_576I_50_INIT_HDMITx_SYNTH_8_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }

        break;

    case E_XC_TGEN_HDMI_480_I_60:
        stCLKGen_Tbl.pTable = (void *)MST_TGEN_480I_60_INIT_CLKGEN_TBL;
        stCLKGen_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_480I_60_INIT_CLKGEN_NUMS*REG_DATA_SIZE;
        stCLKGen_Tbl.u8TabRows = sizeof(MST_TGEN_480I_60_INIT_CLKGEN_TBL)/stCLKGen_Tbl.u8TabCols;
        stCLKGen_Tbl.enType = EN_TGEN_CLK_GEN;

        stSC_Tbl.pTable = (void *)MST_TGEN_480I_60_INIT_SC1_TBL;
        stSC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_480I_60_INIT_SC1_NUMS*REG_DATA_SIZE;
        stSC_Tbl.u8TabRows = sizeof(MST_TGEN_480I_60_INIT_SC1_TBL)/stCLKGen_Tbl.u8TabCols;
        stSC_Tbl.enType = EN_TGEN_SC;

        stHDMITx_MISC_Tbl.pTable = (void *)MST_TGEN_480I_60_INIT_HDMITx_MISC_TBL;
        stHDMITx_MISC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_480I_60_INIT_HDMITx_MISC_NUMS*REG_DATA_SIZE;
        stHDMITx_MISC_Tbl.u8TabRows = sizeof(MST_TGEN_480I_60_INIT_HDMITx_MISC_TBL)/stCLKGen_Tbl.u8TabCols;
        stHDMITx_MISC_Tbl.enType = EN_TGEN_HDMITX_MISC;

        if(b10Bits)
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_480I_60_INIT_HDMITx_SYNTH_10_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_480I_60_INIT_HDMITx_SYNTH_10_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_480I_60_INIT_HDMITx_SYNTH_10_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }
        else
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_480I_60_INIT_HDMITx_SYNTH_8_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_480I_60_INIT_HDMITx_SYNTH_8_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_480I_60_INIT_HDMITx_SYNTH_8_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }

        break;

    case E_XC_TGEN_HDMI_576_I_50:
        stCLKGen_Tbl.pTable = (void *)MST_TGEN_576I_50_INIT_CLKGEN_TBL;
        stCLKGen_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_576I_50_INIT_CLKGEN_NUMS*REG_DATA_SIZE;
        stCLKGen_Tbl.u8TabRows = sizeof(MST_TGEN_576I_50_INIT_CLKGEN_TBL)/stCLKGen_Tbl.u8TabCols;
        stCLKGen_Tbl.enType = EN_TGEN_CLK_GEN;

        stSC_Tbl.pTable = (void *)MST_TGEN_576I_50_INIT_SC1_TBL;
        stSC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_576I_50_INIT_SC1_NUMS*REG_DATA_SIZE;
        stSC_Tbl.u8TabRows = sizeof(MST_TGEN_576I_50_INIT_SC1_TBL)/stCLKGen_Tbl.u8TabCols;
        stSC_Tbl.enType = EN_TGEN_SC;

        stHDMITx_MISC_Tbl.pTable = (void *)MST_TGEN_576I_50_INIT_HDMITx_MISC_TBL;
        stHDMITx_MISC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_576I_50_INIT_HDMITx_MISC_NUMS*REG_DATA_SIZE;
        stHDMITx_MISC_Tbl.u8TabRows = sizeof(MST_TGEN_576I_50_INIT_HDMITx_MISC_TBL)/stCLKGen_Tbl.u8TabCols;
        stHDMITx_MISC_Tbl.enType = EN_TGEN_HDMITX_MISC;

        if(b10Bits)
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_576I_50_INIT_HDMITx_SYNTH_10_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_576I_50_INIT_HDMITx_SYNTH_10_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_576I_50_INIT_HDMITx_SYNTH_10_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }
        else
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_576I_50_INIT_HDMITx_SYNTH_8_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_576I_50_INIT_HDMITx_SYNTH_8_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_576I_50_INIT_HDMITx_SYNTH_8_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }

        break;

    case E_XC_TGEN_HDMI_480_P_60:
        stCLKGen_Tbl.pTable = (void *)MST_TGEN_480P_60_INIT_CLKGEN_TBL;
        stCLKGen_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_480P_60_INIT_CLKGEN_NUMS*REG_DATA_SIZE;
        stCLKGen_Tbl.u8TabRows = sizeof(MST_TGEN_480P_60_INIT_CLKGEN_TBL)/stCLKGen_Tbl.u8TabCols;
        stCLKGen_Tbl.enType = EN_TGEN_CLK_GEN;

        stSC_Tbl.pTable = (void *)MST_TGEN_480P_60_INIT_SC1_TBL;
        stSC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_480P_60_INIT_SC1_NUMS*REG_DATA_SIZE;
        stSC_Tbl.u8TabRows = sizeof(MST_TGEN_480P_60_INIT_SC1_TBL)/stCLKGen_Tbl.u8TabCols;
        stSC_Tbl.enType = EN_TGEN_SC;

        stHDMITx_MISC_Tbl.pTable = (void *)MST_TGEN_480P_60_INIT_HDMITx_MISC_TBL;
        stHDMITx_MISC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_480P_60_INIT_HDMITx_MISC_NUMS*REG_DATA_SIZE;
        stHDMITx_MISC_Tbl.u8TabRows = sizeof(MST_TGEN_480P_60_INIT_HDMITx_MISC_TBL)/stCLKGen_Tbl.u8TabCols;
        stHDMITx_MISC_Tbl.enType = EN_TGEN_HDMITX_MISC;

        if(b10Bits)
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_480P_60_INIT_HDMITx_SYNTH_10_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_480P_60_INIT_HDMITx_SYNTH_10_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_480P_60_INIT_HDMITx_SYNTH_10_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }
        else
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_480P_60_INIT_HDMITx_SYNTH_8_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_480P_60_INIT_HDMITx_SYNTH_8_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_480P_60_INIT_HDMITx_SYNTH_8_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }

        break;

    case E_XC_TGEN_HDMI_576_P_50:
        stCLKGen_Tbl.pTable = (void *)MST_TGEN_576P_50_INIT_CLKGEN_TBL;
        stCLKGen_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_576P_50_INIT_CLKGEN_NUMS*REG_DATA_SIZE;
        stCLKGen_Tbl.u8TabRows = sizeof(MST_TGEN_576P_50_INIT_CLKGEN_TBL)/stCLKGen_Tbl.u8TabCols;
        stCLKGen_Tbl.enType = EN_TGEN_CLK_GEN;

        stSC_Tbl.pTable = (void *)MST_TGEN_576P_50_INIT_SC1_TBL;
        stSC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_576P_50_INIT_SC1_NUMS*REG_DATA_SIZE;
        stSC_Tbl.u8TabRows = sizeof(MST_TGEN_576P_50_INIT_SC1_TBL)/stCLKGen_Tbl.u8TabCols;
        stSC_Tbl.enType = EN_TGEN_SC;

        stHDMITx_MISC_Tbl.pTable = (void *)MST_TGEN_576P_50_INIT_HDMITx_MISC_TBL;
        stHDMITx_MISC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_576P_50_INIT_HDMITx_MISC_NUMS*REG_DATA_SIZE;
        stHDMITx_MISC_Tbl.u8TabRows = sizeof(MST_TGEN_576P_50_INIT_HDMITx_MISC_TBL)/stCLKGen_Tbl.u8TabCols;
        stHDMITx_MISC_Tbl.enType = EN_TGEN_HDMITX_MISC;

        if(b10Bits)
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_576P_50_INIT_HDMITx_SYNTH_10_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_576P_50_INIT_HDMITx_SYNTH_10_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_576P_50_INIT_HDMITx_SYNTH_10_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }
        else
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_576P_50_INIT_HDMITx_SYNTH_8_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_576P_50_INIT_HDMITx_SYNTH_8_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_576P_50_INIT_HDMITx_SYNTH_8_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }

        break;


    case E_XC_TGEN_HDMI_720_P_50:
        stCLKGen_Tbl.pTable = (void *)MST_TGEN_720P_50_INIT_CLKGEN_TBL;
        stCLKGen_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_720P_50_INIT_CLKGEN_NUMS*REG_DATA_SIZE;
        stCLKGen_Tbl.u8TabRows = sizeof(MST_TGEN_720P_50_INIT_CLKGEN_TBL)/stCLKGen_Tbl.u8TabCols;
        stCLKGen_Tbl.enType = EN_TGEN_CLK_GEN;

        stSC_Tbl.pTable = (void *)MST_TGEN_720P_50_INIT_SC1_TBL;
        stSC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_720P_50_INIT_SC1_NUMS*REG_DATA_SIZE;
        stSC_Tbl.u8TabRows = sizeof(MST_TGEN_720P_50_INIT_SC1_TBL)/stCLKGen_Tbl.u8TabCols;
        stSC_Tbl.enType = EN_TGEN_SC;

        stHDMITx_MISC_Tbl.pTable = (void *)MST_TGEN_720P_50_INIT_HDMITx_MISC_TBL;
        stHDMITx_MISC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_720P_50_INIT_HDMITx_MISC_NUMS*REG_DATA_SIZE;
        stHDMITx_MISC_Tbl.u8TabRows = sizeof(MST_TGEN_720P_50_INIT_HDMITx_MISC_TBL)/stCLKGen_Tbl.u8TabCols;
        stHDMITx_MISC_Tbl.enType = EN_TGEN_HDMITX_MISC;

        if(b10Bits)
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_720P_50_INIT_HDMITx_SYNTH_10_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_720P_50_INIT_HDMITx_SYNTH_10_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_720P_50_INIT_HDMITx_SYNTH_10_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }
        else
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_720P_50_INIT_HDMITx_SYNTH_8_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_720P_50_INIT_HDMITx_SYNTH_8_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_720P_50_INIT_HDMITx_SYNTH_8_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }
        break;



    case E_XC_TGEN_HDMI_720_P_60:
        stCLKGen_Tbl.pTable = (void *)MST_TGEN_720P_60_INIT_CLKGEN_TBL;
        stCLKGen_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_720P_60_INIT_CLKGEN_NUMS*REG_DATA_SIZE;
        stCLKGen_Tbl.u8TabRows = sizeof(MST_TGEN_720P_60_INIT_CLKGEN_TBL)/stCLKGen_Tbl.u8TabCols;
        stCLKGen_Tbl.enType = EN_TGEN_CLK_GEN;

        stSC_Tbl.pTable = (void *)MST_TGEN_720P_60_INIT_SC1_TBL;
        stSC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_720P_60_INIT_SC1_NUMS*REG_DATA_SIZE;
        stSC_Tbl.u8TabRows = sizeof(MST_TGEN_720P_60_INIT_SC1_TBL)/stCLKGen_Tbl.u8TabCols;
        stSC_Tbl.enType = EN_TGEN_SC;

        stHDMITx_MISC_Tbl.pTable = (void *)MST_TGEN_720P_60_INIT_HDMITx_MISC_TBL;
        stHDMITx_MISC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_720P_60_INIT_HDMITx_MISC_NUMS*REG_DATA_SIZE;
        stHDMITx_MISC_Tbl.u8TabRows = sizeof(MST_TGEN_720P_60_INIT_HDMITx_MISC_TBL)/stCLKGen_Tbl.u8TabCols;
        stHDMITx_MISC_Tbl.enType = EN_TGEN_HDMITX_MISC;

        if(b10Bits)
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_720P_60_INIT_HDMITx_SYNTH_10_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_720P_60_INIT_HDMITx_SYNTH_10_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_720P_60_INIT_HDMITx_SYNTH_10_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }
        else
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_720P_60_INIT_HDMITx_SYNTH_8_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_720P_60_INIT_HDMITx_SYNTH_8_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_720P_60_INIT_HDMITx_SYNTH_8_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }
        break;


    case E_XC_TGEN_HDMI_1080_I_50:
        stCLKGen_Tbl.pTable = (void *)MST_TGEN_1080I_50_INIT_CLKGEN_TBL;
        stCLKGen_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080I_50_INIT_CLKGEN_NUMS*REG_DATA_SIZE;
        stCLKGen_Tbl.u8TabRows = sizeof(MST_TGEN_1080I_50_INIT_CLKGEN_TBL)/stCLKGen_Tbl.u8TabCols;
        stCLKGen_Tbl.enType = EN_TGEN_CLK_GEN;

        stSC_Tbl.pTable = (void *)MST_TGEN_1080I_50_INIT_SC1_TBL;
        stSC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080I_50_INIT_SC1_NUMS*REG_DATA_SIZE;
        stSC_Tbl.u8TabRows = sizeof(MST_TGEN_1080I_50_INIT_SC1_TBL)/stCLKGen_Tbl.u8TabCols;
        stSC_Tbl.enType = EN_TGEN_SC;

        stHDMITx_MISC_Tbl.pTable = (void *)MST_TGEN_1080I_50_INIT_HDMITx_MISC_TBL;
        stHDMITx_MISC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080I_50_INIT_HDMITx_MISC_NUMS*REG_DATA_SIZE;
        stHDMITx_MISC_Tbl.u8TabRows = sizeof(MST_TGEN_1080I_50_INIT_HDMITx_MISC_TBL)/stCLKGen_Tbl.u8TabCols;
        stHDMITx_MISC_Tbl.enType = EN_TGEN_HDMITX_MISC;

        if(b10Bits)
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_1080I_50_INIT_HDMITx_SYNTH_10_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080I_50_INIT_HDMITx_SYNTH_10_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_1080I_50_INIT_HDMITx_SYNTH_10_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }
        else
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_1080I_50_INIT_HDMITx_SYNTH_8_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080I_50_INIT_HDMITx_SYNTH_8_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_1080I_50_INIT_HDMITx_SYNTH_8_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }
        break;



    case E_XC_TGEN_HDMI_1080_I_60:
        stCLKGen_Tbl.pTable = (void *)MST_TGEN_1080I_60_INIT_CLKGEN_TBL;
        stCLKGen_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080I_60_INIT_CLKGEN_NUMS*REG_DATA_SIZE;
        stCLKGen_Tbl.u8TabRows = sizeof(MST_TGEN_1080I_60_INIT_CLKGEN_TBL)/stCLKGen_Tbl.u8TabCols;
        stCLKGen_Tbl.enType = EN_TGEN_CLK_GEN;

        stSC_Tbl.pTable = (void *)MST_TGEN_1080I_60_INIT_SC1_TBL;
        stSC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080I_60_INIT_SC1_NUMS*REG_DATA_SIZE;
        stSC_Tbl.u8TabRows = sizeof(MST_TGEN_1080I_60_INIT_SC1_TBL)/stCLKGen_Tbl.u8TabCols;
        stSC_Tbl.enType = EN_TGEN_SC;

        stHDMITx_MISC_Tbl.pTable = (void *)MST_TGEN_1080I_60_INIT_HDMITx_MISC_TBL;
        stHDMITx_MISC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080I_60_INIT_HDMITx_MISC_NUMS*REG_DATA_SIZE;
        stHDMITx_MISC_Tbl.u8TabRows = sizeof(MST_TGEN_1080I_60_INIT_HDMITx_MISC_TBL)/stCLKGen_Tbl.u8TabCols;
        stHDMITx_MISC_Tbl.enType = EN_TGEN_HDMITX_MISC;

        if(b10Bits)
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_1080I_60_INIT_HDMITx_SYNTH_10_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080I_60_INIT_HDMITx_SYNTH_10_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_1080I_60_INIT_HDMITx_SYNTH_10_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }
        else
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_1080I_60_INIT_HDMITx_SYNTH_8_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080I_60_INIT_HDMITx_SYNTH_8_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_1080I_60_INIT_HDMITx_SYNTH_8_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }
        break;

    case E_XC_TGEN_HDMI_1080_P_50:
        stCLKGen_Tbl.pTable = (void *)MST_TGEN_1080P_50_INIT_CLKGEN_TBL;
        stCLKGen_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080P_50_INIT_CLKGEN_NUMS*REG_DATA_SIZE;
        stCLKGen_Tbl.u8TabRows = sizeof(MST_TGEN_1080P_50_INIT_CLKGEN_TBL)/stCLKGen_Tbl.u8TabCols;
        stCLKGen_Tbl.enType = EN_TGEN_CLK_GEN;

        stSC_Tbl.pTable = (void *)MST_TGEN_1080P_50_INIT_SC1_TBL;
        stSC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080P_50_INIT_SC1_NUMS*REG_DATA_SIZE;
        stSC_Tbl.u8TabRows = sizeof(MST_TGEN_1080P_50_INIT_SC1_TBL)/stCLKGen_Tbl.u8TabCols;
        stSC_Tbl.enType = EN_TGEN_SC;

        stHDMITx_MISC_Tbl.pTable = (void *)MST_TGEN_1080P_50_INIT_HDMITx_MISC_TBL;
        stHDMITx_MISC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080P_50_INIT_HDMITx_MISC_NUMS*REG_DATA_SIZE;
        stHDMITx_MISC_Tbl.u8TabRows = sizeof(MST_TGEN_1080P_50_INIT_HDMITx_MISC_TBL)/stCLKGen_Tbl.u8TabCols;
        stHDMITx_MISC_Tbl.enType = EN_TGEN_HDMITX_MISC;

        if(b10Bits)
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_1080P_50_INIT_HDMITx_SYNTH_10_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080P_50_INIT_HDMITx_SYNTH_10_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_1080P_50_INIT_HDMITx_SYNTH_10_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }
        else
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_1080P_50_INIT_HDMITx_SYNTH_8_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080P_50_INIT_HDMITx_SYNTH_8_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_1080P_50_INIT_HDMITx_SYNTH_8_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }
        break;

    case E_XC_TGEN_HDMI_1080_P_60:
        stCLKGen_Tbl.pTable = (void *)MST_TGEN_1080P_60_INIT_CLKGEN_TBL;
        stCLKGen_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080P_60_INIT_CLKGEN_NUMS*REG_DATA_SIZE;
        stCLKGen_Tbl.u8TabRows = sizeof(MST_TGEN_1080P_60_INIT_CLKGEN_TBL)/stCLKGen_Tbl.u8TabCols;
        stCLKGen_Tbl.enType = EN_TGEN_CLK_GEN;

        stSC_Tbl.pTable = (void *)MST_TGEN_1080P_60_INIT_SC1_TBL;
        stSC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080P_60_INIT_SC1_NUMS*REG_DATA_SIZE;
        stSC_Tbl.u8TabRows = sizeof(MST_TGEN_1080P_60_INIT_SC1_TBL)/stCLKGen_Tbl.u8TabCols;
        stSC_Tbl.enType = EN_TGEN_SC;

        stHDMITx_MISC_Tbl.pTable = (void *)MST_TGEN_1080P_60_INIT_HDMITx_MISC_TBL;
        stHDMITx_MISC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080P_60_INIT_HDMITx_MISC_NUMS*REG_DATA_SIZE;
        stHDMITx_MISC_Tbl.u8TabRows = sizeof(MST_TGEN_1080P_60_INIT_HDMITx_MISC_TBL)/stCLKGen_Tbl.u8TabCols;
        stHDMITx_MISC_Tbl.enType = EN_TGEN_HDMITX_MISC;
        if(b10Bits)
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_1080P_60_INIT_HDMITx_SYNTH_10_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080P_60_INIT_HDMITx_SYNTH_10_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_1080P_60_INIT_HDMITx_SYNTH_10_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }
        else
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_1080P_60_INIT_HDMITx_SYNTH_8_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080P_60_INIT_HDMITx_SYNTH_8_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_1080P_60_INIT_HDMITx_SYNTH_8_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }
        break;


    case E_XC_TGEN_HDMI_1080_P_30:
        stCLKGen_Tbl.pTable = (void *)MST_TGEN_1080P_30_INIT_CLKGEN_TBL;
        stCLKGen_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080P_30_INIT_CLKGEN_NUMS*REG_DATA_SIZE;
        stCLKGen_Tbl.u8TabRows = sizeof(MST_TGEN_1080P_30_INIT_CLKGEN_TBL)/stCLKGen_Tbl.u8TabCols;
        stCLKGen_Tbl.enType = EN_TGEN_CLK_GEN;

        stSC_Tbl.pTable = (void *)MST_TGEN_1080P_30_INIT_SC1_TBL;
        stSC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080P_30_INIT_SC1_NUMS*REG_DATA_SIZE;
        stSC_Tbl.u8TabRows = sizeof(MST_TGEN_1080P_30_INIT_SC1_TBL)/stCLKGen_Tbl.u8TabCols;
        stSC_Tbl.enType = EN_TGEN_SC;

        stHDMITx_MISC_Tbl.pTable = (void *)MST_TGEN_1080P_30_INIT_HDMITx_MISC_TBL;
        stHDMITx_MISC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080P_30_INIT_HDMITx_MISC_NUMS*REG_DATA_SIZE;
        stHDMITx_MISC_Tbl.u8TabRows = sizeof(MST_TGEN_1080P_30_INIT_HDMITx_MISC_TBL)/stCLKGen_Tbl.u8TabCols;
        stHDMITx_MISC_Tbl.enType = EN_TGEN_HDMITX_MISC;
        if(b10Bits)
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_1080P_30_INIT_HDMITx_SYNTH_10_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080P_30_INIT_HDMITx_SYNTH_10_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_1080P_30_INIT_HDMITx_SYNTH_10_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }
        else
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_1080P_30_INIT_HDMITx_SYNTH_8_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080P_30_INIT_HDMITx_SYNTH_8_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_1080P_30_INIT_HDMITx_SYNTH_8_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }
        break;

    case E_XC_TGEN_HDMI_1080_P_25:
        stCLKGen_Tbl.pTable = (void *)MST_TGEN_1080P_25_INIT_CLKGEN_TBL;
        stCLKGen_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080P_25_INIT_CLKGEN_NUMS*REG_DATA_SIZE;
        stCLKGen_Tbl.u8TabRows = sizeof(MST_TGEN_1080P_25_INIT_CLKGEN_TBL)/stCLKGen_Tbl.u8TabCols;
        stCLKGen_Tbl.enType = EN_TGEN_CLK_GEN;

        stSC_Tbl.pTable = (void *)MST_TGEN_1080P_25_INIT_SC1_TBL;
        stSC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080P_25_INIT_SC1_NUMS*REG_DATA_SIZE;
        stSC_Tbl.u8TabRows = sizeof(MST_TGEN_1080P_25_INIT_SC1_TBL)/stCLKGen_Tbl.u8TabCols;
        stSC_Tbl.enType = EN_TGEN_SC;

        stHDMITx_MISC_Tbl.pTable = (void *)MST_TGEN_1080P_25_INIT_HDMITx_MISC_TBL;
        stHDMITx_MISC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080P_25_INIT_HDMITx_MISC_NUMS*REG_DATA_SIZE;
        stHDMITx_MISC_Tbl.u8TabRows = sizeof(MST_TGEN_1080P_25_INIT_HDMITx_MISC_TBL)/stCLKGen_Tbl.u8TabCols;
        stHDMITx_MISC_Tbl.enType = EN_TGEN_HDMITX_MISC;
        if(b10Bits)
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_1080P_25_INIT_HDMITx_SYNTH_10_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080P_25_INIT_HDMITx_SYNTH_10_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_1080P_25_INIT_HDMITx_SYNTH_10_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }
        else
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_1080P_25_INIT_HDMITx_SYNTH_8_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080P_25_INIT_HDMITx_SYNTH_8_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_1080P_25_INIT_HDMITx_SYNTH_8_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }
        break;

    case E_XC_TGEN_HDMI_1080_P_24:
        stCLKGen_Tbl.pTable = (void *)MST_TGEN_1080P_24_INIT_CLKGEN_TBL;
        stCLKGen_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080P_24_INIT_CLKGEN_NUMS*REG_DATA_SIZE;
        stCLKGen_Tbl.u8TabRows = sizeof(MST_TGEN_1080P_24_INIT_CLKGEN_TBL)/stCLKGen_Tbl.u8TabCols;
        stCLKGen_Tbl.enType = EN_TGEN_CLK_GEN;

        stSC_Tbl.pTable = (void *)MST_TGEN_1080P_24_INIT_SC1_TBL;
        stSC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080P_24_INIT_SC1_NUMS*REG_DATA_SIZE;
        stSC_Tbl.u8TabRows = sizeof(MST_TGEN_1080P_24_INIT_SC1_TBL)/stCLKGen_Tbl.u8TabCols;
        stSC_Tbl.enType = EN_TGEN_SC;

        stHDMITx_MISC_Tbl.pTable = (void *)MST_TGEN_1080P_24_INIT_HDMITx_MISC_TBL;
        stHDMITx_MISC_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080P_24_INIT_HDMITx_MISC_NUMS*REG_DATA_SIZE;
        stHDMITx_MISC_Tbl.u8TabRows = sizeof(MST_TGEN_1080P_24_INIT_HDMITx_MISC_TBL)/stCLKGen_Tbl.u8TabCols;
        stHDMITx_MISC_Tbl.enType = EN_TGEN_HDMITX_MISC;
        if(b10Bits)
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_1080P_24_INIT_HDMITx_SYNTH_10_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080P_24_INIT_HDMITx_SYNTH_10_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_1080P_24_INIT_HDMITx_SYNTH_10_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }
        else
        {
            stSynth_Tbl.pTable = (void *)MST_TGEN_1080P_24_INIT_HDMITx_SYNTH_8_TBL;
            stSynth_Tbl.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+TGEN_TAB_1080P_24_INIT_HDMITx_SYNTH_8_NUMS*REG_DATA_SIZE;
            stSynth_Tbl.u8TabRows = sizeof(MST_TGEN_1080P_24_INIT_HDMITx_SYNTH_8_TBL)/stCLKGen_Tbl.u8TabCols;
            stSynth_Tbl.enType = EN_TGEN_HDMITX_MISC;
        }
        break;

    default:
        stCLKGen_Tbl.pTable = NULL;
        stSC_Tbl.pTable = NULL;
        MS_TGEN_DBG(printf("Unknown TGen Timing Type %x \r\n", enType));
        break;
    }

    memcpy(&gSCx_TabInfo[eWindow], &stSC_Tbl, sizeof(TGEN_TAB_Info));


    if(eWindow == SC1_MAIN_WINDOW)
    {
        Hal_SC_TGen_LoadTable(&stCLKGen_Tbl);
        Hal_SC_TGen_LoadTable(&stHDMITx_MISC_Tbl);
        Hal_SC_TGen_LoadTable(&stSynth_Tbl);
    }

    MS_TGEN_DBG(printf("Load SC Table :%x \r\n", stSC_Tbl.eWindow));
    Hal_SC_TGen_LoadTable(&stSC_Tbl);

}



#undef MHAL_TGEN_C

