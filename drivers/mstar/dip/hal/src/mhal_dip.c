//<MStar Software>
//******************************************************************************
// MStar Software
// Copyright (c) 2010 - 2012 MStar Semiconductor, Inc. All rights reserved.
// All software, firmware and related documentation herein ("MStar Software") are
// intellectual property of MStar Semiconductor, Inc. ("MStar") and protected by
// law, including, but not limited to, copyright law and international treaties.
// Any use, modification, reproduction, retransmission, or republication of all
// or part of MStar Software is expressly prohibited, unless prior written
// permission has been granted by MStar.
//
// By accessing, browsing and/or using MStar Software, you acknowledge that you
// have read, understood, and agree, to be bound by below terms ("Terms") and to
// comply with all applicable laws and regulations:
//
// 1. MStar shall retain any and all right, ownership and interest to MStar
//    Software and any modification/derivatives thereof.
//    No right, ownership, or interest to MStar Software and any
//    modification/derivatives thereof is transferred to you under Terms.
//
// 2. You understand that MStar Software might include, incorporate or be
//    supplied together with third party`s software and the use of MStar
//    Software may require additional licenses from third parties.
//    Therefore, you hereby agree it is your sole responsibility to separately
//    obtain any and all third party right and license necessary for your use of
//    such third party`s software.
//
// 3. MStar Software and any modification/derivatives thereof shall be deemed as
//    MStar`s confidential information and you agree to keep MStar`s
//    confidential information in strictest confidence and not disclose to any
//    third party.
//
// 4. MStar Software is provided on an "AS IS" basis without warranties of any
//    kind. Any warranties are hereby expressly disclaimed by MStar, including
//    without limitation, any warranties of merchantability, non-infringement of
//    intellectual property rights, fitness for a particular purpose, error free
//    and in conformity with any international standard.  You agree to waive any
//    claim against MStar for any loss, damage, cost or expense that you may
//    incur related to your use of MStar Software.
//    In no event shall MStar be liable for any direct, indirect, incidental or
//    consequential damages, including without limitation, lost of profit or
//    revenues, lost or damage of data, and unauthorized system use.
//    You agree that this Section 4 shall still apply without being affected
//    even if MStar Software has been modified by MStar in accordance with your
//    request or instruction for your use, except otherwise agreed by both
//    parties in writing.
//
// 5. If requested, MStar may from time to time provide technical supports or
//    services in relation with MStar Software to you for your use of
//    MStar Software in conjunction with your or your customer`s product
//    ("Services").
//    You understand and agree that, except otherwise agreed by both parties in
//    writing, Services are provided on an "AS IS" basis and the warranty
//    disclaimer set forth in Section 4 above shall apply.
//
// 6. Nothing contained herein shall be construed as by implication, estoppels
//    or otherwise:
//    (a) conferring any license or right to use MStar name, trademark, service
//        mark, symbol or any other identification;
//    (b) obligating MStar or any of its affiliates to furnish any person,
//        including without limitation, you and your customers, any assistance
//        of any kind whatsoever, or any information; or
//    (c) conferring any license or right under any intellectual property right.
//
// 7. These terms shall be governed by and construed in accordance with the laws
//    of Taiwan, R.O.C., excluding its conflict of law rules.
//    Any and all dispute arising out hereof or related hereto shall be finally
//    settled by arbitration referred to the Chinese Arbitration Association,
//    Taipei in accordance with the ROC Arbitration Law and the Arbitration
//    Rules of the Association by three (3) arbitrators appointed in accordance
//    with the said Rules.
//    The place of arbitration shall be in Taipei, Taiwan and the language shall
//    be English.
//    The arbitration award shall be final and binding to both parties.
//
//******************************************************************************
//<MStar Software>
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
//==============================================================================
// [mhal_sc.c]
// Date: 20081203
// Descriptions: Add a new layer for HW setting
//==============================================================================
#define  MHAL_SC_C

#ifdef MSOS_TYPE_LINUX_KERNEL
    #include <asm/div64.h>
#else
    #define do_div(x,y) ((x)/=(y))
#endif

#include <linux/time.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/semaphore.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>


// Common Definition
#include "MsCommon.h"
#include "hwreg_sc.h"
#include "mhal_xc_chip_config.h"
#include "xc_hwreg_utility2.h"
#include "drvXC_IOPort.h"
#include "apiXC.h"
#include "apiXC_DWIN.h"
#include "halCHIP.h"
#include "mhal_dip.h"
#include "hwreg_dip.h"
#include "mhal_mux.h"
#include "hwreg_ipmux.h"
#include "mhal_divp_datatype.h"
//#include "mhal_cmdq.h"

#define SC_DBG(x)   //x
#define FPLL_DBG(x) //x
#define DI_DBG_MSG(x)  \
    if(debug_level >= 2)\
    {\
        x\
    }

//#define MFDEC_DEBUG 0
#define FHD_WIDTH 1920UL

XC_DIP_InternalStatus  g_DIPSrcInfo[MAX_DIP_WINDOW];

#define H_DIP_PostScalingRatio(Input, Output)        (Output!=0)?((MS_U32)( ((MS_U32)(Input)) * 1048576ul / (Output))) : ((MS_U32)(Input)) * 1048576ul
#define V_DIP_PostScalingRatio(Input, Output)        (Output!=0)?((MS_U32)( ((MS_U32)(Input)) * 1048576ul / (Output))) : ((MS_U32)(Input)) * 1048576ul

#define DIPR_TILE_FIFO_MAX  32

#define DI_NWAY_MAX              32
#define DI_FIELD_MAX              8
#define DI_INIT_H_SIZE  32
#define DI_INIT_V_SIZE  32
#define DI_INIT_PITCH  DI_INIT_H_SIZE
#define DI_INIT_RETRY_CNT  300
#define DI_INIT_DELAY_TIME  100  //unit us
#define DI_INIT_DI_QUEUE_COUNT  2

#define new_chakra
static MS_U8  DipWBufCnt = 0;
static SCALER_DIP_SOURCE_TYPE DipSource = SCALER_DIP_SOURCE_TYPE_OP_SUB;

static MS_BOOL bPIPmode = FALSE;
//static InterruptCb pDipIntCb;
//static InterruptCb pDwin0IntCb;
//static InterruptCb pDwin1IntCb;
//static MS_BOOL bDipIsr = FALSE, bDwin0Isr = FALSE, bDwin1Isr = FALSE, bDIPRotation = FALSE, bAttached = FALSE;
static MS_BOOL bDIPRotation = FALSE;
static EN_DRV_XC_DWIN_DATA_FMT DipRFmt;

static MS_U16 DipSrcWidth = 0;
static MS_BOOL bDipHMirror = FALSE;
static MS_BOOL bDipUVSwap = FALSE;
static MS_BOOL bDipYCSwap = FALSE;
static MS_BOOL bDipR2Y = FALSE;

static EN_DRV_XC_DWIN_DATA_FMT DipFmt; //,Dwin1Fmt;
static SCALER_DIP_WIN DIPRotation = MAX_DIP_WINDOW;

static MS_U32 phyDI_DIPBufAddr[DI_NWAY_MAX] = {}; //DI buf address
static MS_U32 u32DI_DIPBufSize[DI_NWAY_MAX] = {}; //DI buf size
static MS_U8 u8DI_DIPBufCnt[DI_NWAY_MAX] = {};
#if (CONFIG_NEW_3DDI_ALGO == 1)  // New DI algorithm
static MS_U8 u8DIBuf_WrTopIndex[DI_NWAY_MAX] = {};
static MS_U8 u8DIBuf_WrBotIndex[DI_NWAY_MAX] = {};
static MS_U8 u8DIBuf_RdTopIndex[DI_NWAY_MAX] = {};
static MS_U8 u8DIBuf_RdBotIndex[DI_NWAY_MAX] = {};
#else
static MS_U8 u8DI_DIPbuf_index[DI_NWAY_MAX] = {};
#endif
static EN_TB_FIELD enTB_DIPField[DI_FIELD_MAX][DI_NWAY_MAX] = {};

static MS_BOOL bDIPEnaDI = FALSE;
static MS_BOOL bDIPNwayInfoInit = FALSE;
static MS_BOOL bDIPEnaDIInit = FALSE;
static EN_DIP_DI_FIELD enTB_DIPCurrentField = E_DIP_3DDI_TOP_SINGLE_FIELD;
static MS_BOOL g_bDIPHVSPStatus = FALSE;
static MS_U16 g_u16DIWidth, g_u16DIHeight;
static MS_U32 g_u32H_PreScalingRatio;
static MS_U32 g_u32V_PreScalingRatio;
static MS_U16 g_u16H_CapStart;
static MS_U16 g_u16H_CapEnd;
static MS_U16 g_u16V_CapStart;
static MS_U16 g_u16V_CapEnd;
static MS_U16 g_u16DIPRWidth;
static MS_BOOL bDIPMfdecEnable = FALSE;
static MS_U8 g_u8REG_CKG_IDCLK3 = 0;

extern int debug_level;

DIP_CHIP_PROPERTY g_DIPChipPro =
{
    .bSourceSupport =                {TRUE, TRUE, FALSE},
    .bDIPBuildIn    =                {TRUE, TRUE, TRUE},
    .XCPathCount    =                XC_PATH_COUNT_SUPPT,
    .BusWordUnit    =                {DWIN_BYTE_PER_WORD, DWIN_BYTE_PER_WORD, 0},
    .bSourceSel =
    {
        .bSOURCE_TYPE_MVOP[0]    =      {TRUE, TRUE, FALSE},
        .bSOURCE_TYPE_MVOP[1]    =      {TRUE, TRUE, FALSE},

        .bSOURCE_TYPE_IP_MAIN[0] =      {TRUE, TRUE, FALSE},
        .bSOURCE_TYPE_IP_SUB[0]  =      {TRUE, TRUE, FALSE},
        .bSOURCE_TYPE_OP_MAIN[0] =      {FALSE, FALSE, FALSE},
        .bSOURCE_TYPE_OP_SUB[0]  =      {FALSE, FALSE, FALSE},
        .bSOURCE_TYPE_OP_CAPTURE[0] =    {TRUE, TRUE,  FALSE},

        .bSOURCE_TYPE_IP_MAIN[1] =      {FALSE, FALSE, FALSE}, //VE
        .bSOURCE_TYPE_IP_SUB[1]  =      {FALSE, FALSE, FALSE},
        .bSOURCE_TYPE_OP_MAIN[1] =      {FALSE, FALSE, FALSE},
        .bSOURCE_TYPE_OP_SUB[1]  =      {FALSE, FALSE, FALSE},
        .bSOURCE_TYPE_OP_CAPTURE[1] =    {TRUE, TRUE,  FALSE},

        .bSOURCE_TYPE_DRAM =            {FALSE, FALSE, FALSE},
        .bSOURCE_TYPE_OSD =             {FALSE, FALSE, FALSE},
    }
};

#define DIP_R2BYTEMSK(u32Id,u32Reg,u16Mask)\
    ({\
        SC_R2BYTEMSK(u32Id, u32Reg, u16Mask);\
    })

#if 0
#define  BANK_REG_MAX  0x80
static MS_U16 u16RegBK34Table[BANK_REG_MAX] = {0};
static MS_U16 u16RegBK36Table[BANK_REG_MAX] = {0};

#define DIP_W2BYTE(u32Id,u32Reg,u16Val)\
    ({\
        if((u32Reg&0xff00) == 0x3400)\
        {\
            u16RegBK34Table[((u32Reg&0xff)>>1)] = u16Val;\
        }\
        else if((u32Reg&0xff00) == 0x3600)\
        {\
            u16RegBK36Table[((u32Reg&0xff)>>1)] = u16Val;\
        }\
        SC_W2BYTE(u32Id, u32Reg, u16Val);\
    })

#define DIP_W4BYTE(u32Id,u32Reg,u32Val)\
    ({\
        if((u32Reg&0xff00) == 0x3400)\
        {\
            u16RegBK34Table[((u32Reg&0xff)>>1)] = u32Val&0x00FF;\
            u16RegBK34Table[(((u32Reg+2)&0xff)>>1)] = (u32Val&0xFF00)>>16;\
        }\
        else if((u32Reg&0xff00) == 0x3600)\
        {\
            u16RegBK36Table[((u32Reg&0xff)>>1)] = u32Val&0x00FF;\
            u16RegBK36Table[(((u32Reg+2)&0xff)>>1)] = (u32Val&0xFF00)>>16;\
        }\
        SC_W4BYTE(u32Id, u32Reg, u32Val);\
    })

#define DIP_W2BYTEMSK(u32Id,u32Reg,u16Val,u16Mask)\
    ({\
        if((u32Reg&0xff00) == 0x3400)\
        {\
            u16RegBK34Table[((u32Reg&0xff)>>1)] &= u16Mask;\
            u16RegBK34Table[((u32Reg&0xff)>>1)] |= u16Val;\
        }\
        else if((u32Reg&0xff00) == 0x3600)\
        {\
            u16RegBK36Table[((u32Reg&0xff)>>1)] &= u16Mask;\
            u16RegBK36Table[((u32Reg&0xff)>>1)] |= u16Val;\
        }\
        SC_W2BYTEMSK(u32Id, u32Reg, u16Val, u16Mask);\
    })
#endif

#if 0
static void HAL_XC_INFO_Dump(void *pstCmdQInfo, SCALER_DIP_WIN eWindow)
{
    P_XC_DIP_InternalStatus pDIPSrcInfo = &g_DIPSrcInfo[eWindow];

    printf("g_DIPSrcInfo[%d]: \n", eWindow);
    printf("========== Info Start ==========\n");
    //----------------------
    // Customer setting
    //----------------------
    //----------------------
    // Source type for DIP
    //----------------------
    //----------------------
    // Window
    //----------------------
    printf("enInputSourceType = %d\n" \
           "enDIPSourceType = %d\n" \
           "stCapWin = (%d, %d, %d, %d)\n",
           pDIPSrcInfo->enInputSourceType,
           pDIPSrcInfo->enDIPSourceType,
           pDIPSrcInfo->stCapWin.x,
           pDIPSrcInfo->stCapWin.y,
           pDIPSrcInfo->stCapWin.width,
           pDIPSrcInfo->stCapWin.height);

    //----------------------
    // Timing
    //----------------------
    printf("bInterlace = %u\n" \
           "bHDuplicate = %u\n",
           pDIPSrcInfo->bInterlace,
           pDIPSrcInfo->bHDuplicate);

    //----------------------
    // customized post scaling
    //----------------------
    printf("bHCusScaling = %u\n" \
           "u16HCusScalingSrc = %u\n" \
           "u16HCusScalingDst = %u\n" \
           "bVCusScaling = %u\n" \
           "u16VCusScalingSrc = %u\n" \
           "u16VCusScalingDst = %u\n",
           pDIPSrcInfo->bHCusScaling,
           pDIPSrcInfo->u16HCusScalingSrc,
           pDIPSrcInfo->u16HCusScalingDst,
           pDIPSrcInfo->bVCusScaling,
           pDIPSrcInfo->u16VCusScalingSrc,
           pDIPSrcInfo->u16VCusScalingDst);

    //-------------------------
    // customized pre scaling
    //-------------------------
    printf("bPreHCusScaling = %u\n" \
           "u16PreHCusScalingSrc = %u\n" \
           "u16PreHCusScalingDst = %u\n" \
           "bPreVCusScaling = %u\n" \
           "u16PreVCusScalingSrc = %u\n" \
           "u16PreVCusScalingDst = %u\n",
           pDIPSrcInfo->bPreHCusScaling,
           pDIPSrcInfo->u16PreHCusScalingSrc,
           pDIPSrcInfo->u16PreHCusScalingDst,
           pDIPSrcInfo->bPreVCusScaling,
           pDIPSrcInfo->u16PreVCusScalingSrc,
           pDIPSrcInfo->u16PreVCusScalingDst);

    //----------------------
    // XC internal setting
    //----------------------
    /* scaling ratio */
    printf("u16H_SizeAfterPreScaling = %u\n" \
           "u16V_SizeAfterPreScaling = %u\n" \
           "u8DelayLines = %u\n" \
           "bMemFmt422 = %u\n" \
           "bMemYUVFmt = %u\n",
           pDIPSrcInfo->u16H_SizeAfterPreScaling,
           pDIPSrcInfo->u16V_SizeAfterPreScaling,
           pDIPSrcInfo->u8DelayLines,
           pDIPSrcInfo->bMemFmt422,
           pDIPSrcInfo->bMemYUVFmt);

    printf("========== Info End ==========\n");
}
#endif

#ifdef DIP_REG_DUMP
void HAL_XC_INFO_dump_Reg(MS_U16 BankNo, MS_U16 RegOffset, MS_U32 len)
{
    MS_U32 addr, l_offset;
    MS_U8 l_buf[128], token_buf[8];
    MS_U32 start;

    start = _PK_L_(BankNo, RegOffset);

    for (addr = start, l_offset = 0; addr < (start + (len * 2)); addr += 2)
    {
        if ((addr & 0xFF) == 0)
        {
            printk(KERN_INFO "BK x%02X :", (addr >> 8));
        }
        if ((addr & 0xF) == 0)
        {
            l_offset = 0;
            snprintf(token_buf, 8, "%02X:", ((0xFF & addr) >> 1));
            strcpy(l_buf + l_offset, token_buf);
            l_offset += strlen(token_buf);
        }

        snprintf(token_buf, 8, " %04X", SC_R2BYTE(0, addr));
        strcpy(l_buf + l_offset, token_buf);
        l_offset += strlen(token_buf);

        if ((addr & 0xF) == 0xE)
        {
            printk(KERN_INFO "%s\n", l_buf);
        }
    }
    printk(KERN_INFO "Dump End\n");
}

void HAL_MFDEC_INFO_dump_Reg(void)
{
    MS_U32 addr, l_offset;
    MS_U8 l_buf[128], token_buf[8];
    MS_U32 start;

    start = REG_MFDEC_BASE;

    for (addr = start, l_offset = 0; addr < (start + (0x80 * 2)); addr += 2)
    {
        if ((addr & 0xFF) == 0)
        {
            printk(KERN_INFO "BK x%04X :", (addr >> 8));
        }
        if ((addr & 0xF) == 0)
        {
            l_offset = 0;
            snprintf(token_buf, 8, "%02X:", ((0xFF & addr) >> 1));
            strcpy(l_buf + l_offset, token_buf);
            l_offset += strlen(token_buf);
        }

        snprintf(token_buf, 8, " %04X", MDrv_Read2Byte(addr));
        strcpy(l_buf + l_offset, token_buf);
        l_offset += strlen(token_buf);

        if ((addr & 0xF) == 0xE)
        {
            printk(KERN_INFO "%s\n", l_buf);
        }
    }
    printk(KERN_INFO "Dump End\n");
}
#endif

void DIP_DelayTask (MS_U32 u32Ms)
{
    //sleep in spinlock will cause deadlock
    //#ifdef MSOS_MUTEX_USE_SEM
    msleep_interruptible((unsigned int)u32Ms);
    //#else
    //    mdelay(u32Ms);
    //#endif
}


void DIP_DelayTaskUs (MS_U32 u32Us)
{
    //sleep in spinlock will cause deadlock
    udelay(u32Us);
}

static MS_U16 HAL_XC_DIP_GetWidth(void *pstCmdQInfo, SCALER_DIP_SOURCE_TYPE eSource, SCALER_DIP_WIN eWindow)
{
    MS_U16 u16MaxWith = 0;

    if(eSource == SCALER_DIP_SOURCE_TYPE_MAIN)  // IP1_Main
    {
#if 1
        //if (MDrv_XC_GetDynamicScalingStatus(pu32XCInst) || MDrv_XC_Is_DSForceIndexEnabled(pu32XCInst, eWindow) )
        //{
        //    u16MaxWith = g_DIPSrcInfo[eWindow].stCapWin.width;
        //}
        //else
        {
            u16MaxWith = DIP_R2BYTEMSK(0, REG_SC_BK01_07_L, BMASK(13: 0));
        }
#endif
    }
    else if(eSource == SCALER_DIP_SOURCE_TYPE_SUB)   // IP1_Sub
    {
        u16MaxWith = DIP_R2BYTEMSK(0, REG_SC_BK03_07_L, BMASK(13: 0));
    }
    else if(eSource == SCALER_DIP_SOURCE_TYPE_OP_MAIN)  // SCMI OP
    {
        u16MaxWith = DIP_R2BYTEMSK(0, REG_SC_BK12_17_L, BMASK(13: 0));
    }
    else if(eSource == SCALER_DIP_SOURCE_TYPE_OP_SUB)  // SCMI_SUB OP
    {
        u16MaxWith = DIP_R2BYTEMSK(0, REG_SC_BK12_57_L, BMASK(13: 0));
    }
    else if(eSource == SCALER_DIP_SOURCE_TYPE_OP_CAPTURE)  // VOP
    {
        u16MaxWith = (DIP_R2BYTEMSK(0, REG_SC_BK10_05_L, BMASK(13: 0)) - DIP_R2BYTEMSK(0, REG_SC_BK10_04_L, BMASK(13: 0)) + 1);
    }
    else if(eSource == SCALER_DIP_SOURCE_TYPE_DRAM)
    {
        if (eWindow == DIP_WINDOW)
        {
#if 1  // for CmdQ
            u16MaxWith = g_u16DIPRWidth;
#else
            u16MaxWith = DIP_R2BYTEMSK(0, REG_SC_BK36_27_L, BMASK(13: 0));
#endif
        }
    }
    return u16MaxWith;
}

static MS_BOOL IsNeedAverageMode(SCALER_DIP_SOURCE_TYPE eSource, MS_U16 u16MaxWith)
{
    MS_BOOL bNeedAverageMode = FALSE;
    //k2k@60 averge to 2k2k@60
    if((eSource == SCALER_DIP_SOURCE_TYPE_OP_CAPTURE) && (u16MaxWith > FHD_WIDTH))
    {
        bNeedAverageMode = TRUE;
    }
    else if(( (eSource == SCALER_DIP_SOURCE_TYPE_OP_MAIN ) || (eSource == SCALER_DIP_SOURCE_TYPE_OP_SUB) ) && (u16MaxWith > FHD_WIDTH) )
    {
        bNeedAverageMode = TRUE;
    }
    //else if( (eSource == SCALER_DIP_SOURCE_TYPE_MAIN) && (DIP_R2BYTEMSK(0, REG_SC_BK02_0A_L, BIT(15)) != 0 ) )
    else if((eSource == SCALER_DIP_SOURCE_TYPE_MAIN) && (u16MaxWith > FHD_WIDTH))
    {
        bNeedAverageMode = TRUE;
    }
    else if( (eSource == SCALER_DIP_SOURCE_TYPE_SUB) && (DIP_R2BYTEMSK(0, REG_SC_BK04_0A_L, BIT(15)) != 0 ) )
    {
        bNeedAverageMode = TRUE;
    }
    else
    {
        bNeedAverageMode = FALSE;
    }
    return bNeedAverageMode;
}

void HAL_XC_DIP_DI_Read_FIFO(void *pstCmdQInfo, MS_U16 u16Width, SCALER_DIP_WIN eWindow)
{
    MS_U16 u16BusSize = 0;
    MS_U16 u16MiuCnt = 0;
    MS_U16 u16temp = 0;
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    u16BusSize = HAL_XC_DIP_GetBusSize(pstCmdQInfo, eWindow);
    u16MiuCnt = u16Width / u16BusSize;

    for(u16temp = DIPR_TILE_FIFO_MAX; u16temp > 0; u16temp--)
    {
        if((u16MiuCnt % u16temp) == 0)
        {
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_7F_L, (u16temp - 1), BMASK(6: 0));
            break;
        }
    }
}

//==============================================================================
MS_BOOL HAL_XC_DIP_CMDQBegin(void *pstCmdQInfo, SCALER_DIP_WIN eWindow)
{
    printf("[Warnning][%s], Line:%d, SCALER_DIP CMDQ not Support\n", __FUNCTION__, __LINE__);
    return FALSE;
}

MS_BOOL HAL_XC_DIP_CMDQEnd(void *pstCmdQInfo, SCALER_DIP_WIN eWindow)
{
    printf("[Warnning][%s], Line:%d, SCALER_DIP CMDQ not Support\n", __FUNCTION__, __LINE__);
    return FALSE;
}
//==============================================================================

//extern PQ_Function_Info    s_PQ_Function_Info;
//extern MS_BOOL             s_bKeepPixelPointerAppear;

void Hal_SC_DWIN_set_422_cbcr_swap(void *pstCmdQInfo, MS_BOOL bEnable, SCALER_DIP_WIN eWindow)
{
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_30_L, bEnable ? BIT(3) : 0, BIT(3));

}

void Hal_SC_DWIN_set_pre_align_pixel(void *pstCmdQInfo, MS_BOOL bEnable, MS_U16 pixels, SCALER_DIP_WIN eWindow)
{
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    if (bEnable)
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_60_L, BIT(1), BIT(1));
    else
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_60_L, 0x00, BIT(1));

}

#if 0
// This function will return 8/10/12/14/16 field mode or 8 frame mode.
// Otherwise it return IMAGE_STORE_2_FRAMES
XC_FRAME_STORE_NUMBER Hal_SC_DWIN_GetFrameStoreMode(void *pstCmdQInfo, SCALER_DIP_WIN eWindow, MS_BOOL bInterlace)
{
    if (bInterlace)
    {
        return IMAGE_STORE_4_FIELDS;
    }
    else
    {
        return IMAGE_STORE_2_FRAMES;
    }
}
#endif

void Hal_SC_DWIN_EnableR2YCSC(void *pstCmdQInfo, MS_BOOL bEnable, SCALER_DIP_WIN eWindow)
{
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    bDipR2Y = bEnable;
    if( ((DipSource == SCALER_DIP_SOURCE_TYPE_OP_MAIN) || (DipSource == SCALER_DIP_SOURCE_TYPE_OP_SUB)) && (bDipR2Y == FALSE) )
    {
        //enable  422 to 444
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_6C_L, BIT(9), BIT(9));
        //enable  source yc swap
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_7C_L, BIT(14), BIT(14));
    }
    else
    {
        //disable  422 to 444
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_6C_L, 0, BIT(9));
        //disable  source yc swap
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_7C_L, 0, BIT(14));
    }
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_31_L, (bEnable ? BIT(0) : 0), BIT(0));

}

void Hal_SC_DWIN_Set_vsd_output_line_count(void *pstCmdQInfo, MS_BOOL bEnable, MS_U32 u32LineCount, SCALER_DIP_WIN eWindow)
{
    MS_U16 u16OutputLineCount = 0x00;
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    if (bEnable)
    {
        u16OutputLineCount = BIT(15);
        u16OutputLineCount |= (MS_U16)(u32LineCount & 0x1FFF);
    }

    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_37_L, u16OutputLineCount, 0x81FF);

}

/*
 bEnable : Enable input line count.
 bUserMode : 1 -> the input line count will ref u32UserLineCount
             0 -> the input line count will ref V capture win
*/
void Hal_SC_DWIN_Set_vsd_input_line_count(void *pstCmdQInfo, MS_BOOL bEnable, MS_BOOL bUserMode, MS_U32 u32UserLineCount, SCALER_DIP_WIN eWindow)
{
    MS_U16 u16InputLineCount = 0x00;
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    if (bEnable)
    {
        u16InputLineCount = BIT(15);

        if (bUserMode)
        {
            u16InputLineCount |= BIT(14);
            u16InputLineCount |= (MS_U16)(u32UserLineCount & 0x1FFF);
        }
    }

    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK34_36_L, u16InputLineCount);

}

void Hal_SC_DWIN_sw_db(void *pstCmdQInfo, P_SC_DIP_SWDB_INFO pDBreg, SCALER_DIP_WIN eWindow)
{
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;
    MS_U16 u16H_CapStart = 0;
    MS_U16 u16H_CapEnd = 0;
    MS_U16 u16V_CapSize = 0;

    //    if(bDIPEnaDI == TRUE)
    //    {
    //        SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK34_04_L, (pDBreg->u32H_PreScalingRatio - 2)); // H pre-scaling
    //        g_u32H_PreScalingRatio = pDBreg->u32H_PreScalingRatio - 2;
    //    }
    //    else
    {
        SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK34_04_L, pDBreg->u32H_PreScalingRatio);   // H pre-scaling
        g_u32H_PreScalingRatio = pDBreg->u32H_PreScalingRatio;
    }

    SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK34_08_L, pDBreg->u32V_PreScalingRatio);   // V pre-scaling
    g_u32V_PreScalingRatio = pDBreg->u32V_PreScalingRatio;

    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_60_L, BIT(1) | BIT(0), BIT(1) | BIT(0));

    u16H_CapStart = pDBreg->u16H_CapStart + 1;
    u16H_CapEnd = pDBreg->u16H_CapStart + pDBreg->u16H_CapSize;
    if((bDIPEnaDI == TRUE) && (enTB_DIPCurrentField == E_DIP_3DDI_BOTTOM_BOTH_FIELD_STAGGER))
    {
        u16H_CapStart = u16H_CapStart + pDBreg->u16H_CapSize;
        u16H_CapEnd = u16H_CapEnd + pDBreg->u16H_CapSize;
    }
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK34_61_L, u16H_CapStart);
    g_u16H_CapStart = u16H_CapStart;
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK34_62_L, u16H_CapEnd);
    g_u16H_CapEnd = u16H_CapEnd;

    if((bDIPEnaDI == TRUE) && ((enTB_DIPCurrentField == E_DIP_3DDI_TOP_BOTH_FIELD_STAGGER) || (enTB_DIPCurrentField == E_DIP_3DDI_BOTTOM_BOTH_FIELD_STAGGER)))
    {
        u16V_CapSize = (pDBreg->u16V_CapSize) / 2;
    }
    else
        u16V_CapSize = pDBreg->u16V_CapSize;

    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK34_63_L, pDBreg->u16V_CapStart + 1);
    g_u16V_CapStart = pDBreg->u16V_CapStart + 1;
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK34_64_L, pDBreg->u16V_CapStart + u16V_CapSize);
    g_u16V_CapEnd = pDBreg->u16V_CapStart + u16V_CapSize;
    DipSrcWidth = pDBreg->u16H_CapStart + pDBreg->u16H_CapSize;

}

void Hal_SC_DWIN_get_sw_db(void *pstCmdQInfo, P_SC_DIP_SWDB_INFO pDBreg, SCALER_DIP_WIN eWindow)
{
#if 1  // for CmdQ
    pDBreg->u32H_PreScalingRatio =  g_u32H_PreScalingRatio;
    pDBreg->u32V_PreScalingRatio =  g_u32V_PreScalingRatio;

    pDBreg->u16H_CapStart = (g_u16H_CapStart - 1);
    pDBreg->u16H_CapSize = (g_u16H_CapEnd - pDBreg->u16H_CapStart);
    pDBreg->u16V_CapStart = (g_u16V_CapStart - 1);
    pDBreg->u16V_CapSize = (g_u16V_CapEnd - pDBreg->u16V_CapStart);
#else
    pDBreg->u32H_PreScalingRatio =  SC_R4BYTE(0, REG_SC_BK34_04_L);
    pDBreg->u32V_PreScalingRatio =  SC_R4BYTE(0, REG_SC_BK34_08_L);

    pDBreg->u16H_CapStart = (SC_R2BYTE(0, REG_SC_BK34_61_L) - 1);
    pDBreg->u16H_CapSize = (SC_R2BYTE(0, REG_SC_BK34_62_L) - pDBreg->u16H_CapStart);
    pDBreg->u16V_CapStart = (SC_R2BYTE(0, REG_SC_BK34_63_L) - 1);
    pDBreg->u16V_CapSize = (SC_R2BYTE(0, REG_SC_BK34_64_L) - pDBreg->u16V_CapStart);
#endif

}

void HAL_XC_DIP_Set444to422(void *pstCmdQInfo, EN_DRV_XC_DWIN_DATA_FMT fmt, MS_BOOL bSrcYUVFmt, MS_BOOL bSrcFmt422, SCALER_DIP_WIN eWindow)
{

}

void HAL_XC_DIP_SetFRC(void *pstCmdQInfo, MS_BOOL bEnable, MS_U16 u16In, MS_U16 u16Out, SCALER_DIP_WIN eWindow)
{
    MS_U16 u16Ratio = 0;
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    if(bEnable)
    {
        if(u16In > u16Out)
        {
            u16Ratio = (u16In - u16Out) * 64 / u16In;
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_40_L, u16Ratio, BMASK(5: 0));
        }
        else
        {
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_40_L, 0, BMASK(5: 0));
        }
    }
    else
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_40_L, 0, BMASK(5: 0));
    }

}
void Hal_SC_DWIN_set_input_vsync_inverse(void *pstCmdQInfo, MS_BOOL bEnable, SCALER_DIP_WIN eWindow)
{
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_01_L, (bEnable << 2), BIT(2) );

}

//=============== DIP =====================//
#define DWIN_CAPTURE_TIMEOUT_CNT 10 //consider 576i video, maximum time is 20+20x2 = 60 ms

MS_U16 HAL_XC_DIP_GetBusSize(void *pstCmdQInfo, SCALER_DIP_WIN eWindow)
{
    return g_DIPChipPro.BusWordUnit[eWindow];
}

void HAL_XC_DIP_SWReset(void *pstCmdQInfo, SCALER_DIP_WIN eWindow)
{
    MS_U16 u16HSrc = 0, u16HDst = 0, u16VSrc = 0, u16VDst = 0;
    MS_U32 u32HRatio_Ena = 0, u32VRatio_Ena = 0;
    MS_U16 u16EnaHVSP = 0;
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    if(bDIPEnaDIInit == TRUE)
        return;
    //store HVSP setting
    u16HSrc = SC_R2BYTE(0, REG_SC_BK49_22_L);
    u16VSrc = SC_R2BYTE(0, REG_SC_BK49_23_L);
    u16HDst = SC_R2BYTE(0, REG_SC_BK49_24_L);
    u16VDst = SC_R2BYTE(0, REG_SC_BK49_25_L);
    u32HRatio_Ena = SC_R4BYTE(0, REG_SC_BK49_07_L);
    u32VRatio_Ena = SC_R4BYTE(0, REG_SC_BK49_0A_L);
    u16EnaHVSP = SC_R2BYTEMSK(0, REG_SC_BK49_70_L, BMASK(2: 1));
    //reset DIP
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_7F_L, BIT(0), BIT(0));
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_7F_L, 0, BIT(0));
    //restore HVSP setting
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK49_22_L, u16HSrc);
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK49_23_L, u16VSrc);
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK49_24_L, u16HDst);
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK49_25_L, u16VDst);
    SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK49_07_L, u32HRatio_Ena);
    SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK49_0A_L, u32VRatio_Ena);
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK49_70_L, u16EnaHVSP, BMASK(2: 1));

}

void HAL_XC_DIP_Init(void *pstCmdQInfo, SCALER_DIP_WIN eWindow)
{
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    MDrv_WriteRegBit_CmdQ(pstCmdQ, REG_CKG_DIP_FCLK, DISABLE, CKG_DIP_FCLK_GATED);                    // Enable clock
    //MDrv_WriteByteMask_CmdQ(pstCmdQ, REG_CKG_DIP_FCLK, CKG_DIP_FCLK_216MHZ, CKG_DIP_FCLK_MASK);
    //MDrv_WriteByteMask_CmdQ(pstCmdQ, REG_CKG_DIP_FCLK, CKG_DIP_FCLK_345MHZ, CKG_DIP_FCLK_MASK);
    MDrv_WriteByteMask_CmdQ(pstCmdQ, REG_CKG_DIP_FCLK, CKG_DIP_FCLK_432MHZ, CKG_DIP_FCLK_MASK);

    //SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_7F_L, BIT(0), BIT(0));
    //SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_7F_L, 0, BIT(0));

    //[15]intlac. src;[12:8]422to420 ratio;[6]dwin_en;[5:4]format(rgb888);[3]intlac_w;[2:0]frm_buf_num
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_01_L, 0x0840);
    // [10]pdw_off;[9:8] reg_dip_pdw_src_sel;[7:0]alpha
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_02_L, 0x0400);
    //[8] write once; [9] write once trig
    if(HAL_XC_DIP_GetBusSize(pstCmdQInfo, eWindow) == DIP_BYTE_PER_WORD)
        SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_03_L, 0x0000);
    else
        SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_03_L, 0x0800);
    //When dip overstep memory next to dip,dip won't stop telling this wrong state with continuous interrupt.
    //The continuous interrupt will cause the high cpu possesion.
    //So we mask the related bit to ignore the wrong state.
    //Function HAL_XC_MIU_Protect will protect the memory next to dip being overstepped by dip in case "MIU_PROTECT == 1".
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_04_L, BIT(4), BIT(4));
    //clear mirror state
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_0B_L, 0, BIT(9));
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_0B_L, 0, BIT(10));
#if (DIP_PLATFORM == DIP_K6)
    //[15:8]wreq max ; [7:0]wreq threshold
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_6F_L, 0x200A);
    //tile request number
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_7E_L, 0x0010);
#else
    //[15:8]wreq max ; [7:0]wreq threshold
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_6F_L, 0x400A);
    //tile request number
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_7E_L, 0x0010);
#endif
    //Double buffer enable
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_7E_L, BIT(0), BIT(0));
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7F_L, BIT(7), BIT(7));
    //enable dip clk
    MDrv_WriteRegBit_CmdQ(pstCmdQ, REG_CKG_IDCLK3, DISABLE, CKG_IDCLK3_INVERT);                   // Not Invert
    MDrv_WriteRegBit_CmdQ(pstCmdQ, REG_CKG_IDCLK3, /*CKG_IDCLK3_GATED*/DISABLE, CKG_IDCLK3_GATED);                   // Disable clock
    g_u8REG_CKG_IDCLK3 = MDrv_ReadByte(REG_CKG_IDCLK3);

#if 0   //enable OSD blending
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_10_L, BIT(0), BIT(0));
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_10_L, 0, BIT(1));  // Disable DE mode with GOP, using GOP handshaking mode
    //SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_10_L, BIT(6), BIT(6));  // Vsync invert (to make vfde falling)
#endif
#if 0   //disable OSD blending
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_10_L, 0, BIT(0));
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_10_L, BIT(1), BIT(1));  // Enable DE mode with GOP if GOP is disable
    //SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_10_L, BIT(6), BIT(6));  // Vsync invert (to make vfde falling)
#endif

    //TEE clip
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_0F_L, 0x1000, BMASK(12: 0));
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_2F_L, 0x1000, BMASK(12: 0));

    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_1C_L, 0, BIT(14));

    //force OP1 ACK 1
    //SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_7F_L,BIT(1),BIT(1));
    //for DIP R
    //read request maximum length and 2 times tile 420 used
    // [14:8]: reg_dipr_req_max[6:0], [6:0]: reg_dipr_tile_req_num[6:0]
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_7F_L, 0x400F);
    //SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_7F_L, 0x4013);
    //read request threshold
#if 1  // for MFDEC
    // Reg7dh[5:0] = 0x18 reg_dipr_req_thrd[5:0], Reg7dh[12:8] = 0x0f reg_dipr_hi_pri_num[4:0]
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7D_L, 0x18, BMASK(5: 0));
    //SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7D_L, 0x0A, BMASK(5: 0));
    //SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7D_L, (0x0f << 8), BMASK(12: 8));
    //SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7D_L, (0x01 << 8), BMASK(12: 8));
#else
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7D_L, 0xA, BMASK(5: 0));
#endif
    //reg_dipr_tile_req_num_evd
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_1C_L, 0x7, BMASK(4: 0));

    // DI disable
#if (DIP_PLATFORM == DIP_K6)
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_0D_L, 0, BIT(0));
#else
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_1E_L, 0, BIT(0));
#endif

#if (DIP_PLATFORM == DIP_I2)
    // select which dip can return cmdq int
    //SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_5D_L, 0, BMASK(5: 4));
    // select which dip can osdb with gop
    //SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_5D_L, 0, BMASK(7: 6));
#endif

    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK6C_40_L, BIT(7), BIT(7));    //Disable clock
    //SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK6C_40_L, 0, BIT(7));    //Enable clock
#ifdef CONFIG_MSTAR_SRAMPD
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_7E_L, BMASK(7: 4), BMASK(7: 4));
#endif
    //DI force filed
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7C_L, 0, BIT(6));
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_3B_L, BIT(0), BIT(0));  // dipw force frame index enable
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_7C_L, BIT(8), BIT(8));  //dipr force frame index

#if (DIP_PLATFORM == DIP_I2)
    // Disable DIPR and DIPR CmdQ interrupt
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_38_L, 0xFF, BMASK(7: 0));  // DIPR interrupt mask
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_48_L, 0xFF, BMASK(7: 0));  // DIPR CmdQ interrupt mask
#endif

    //output capture setting
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK10_23_L, BIT(4), BIT(4));  // Set to no osd for DIP select
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK10_23_L, E_XC_DIP_VOP2 << 12, BMASK(13: 12));
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK10_50_L, BIT(14), BMASK(14: 13));

    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK10_50_L, BIT(12), BIT(12));  // Set to with osd for DIP select
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK10_50_L, E_XC_DIP_OP2 << 8, BMASK(9: 8));
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK10_6B_L, BIT(13), BMASK(13: 12));

    //recv2dip_cap_en_main
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK4A_05_L, BIT(1), BIT(1));
    //recv2dip_cap_en_sub
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK4A_04_L, BIT(1), BIT(1));

    //Sc2 op capture enable:
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK90_50_L, BIT(12), BIT(12));
    //Sc2 op capture stage 0: before osdb 1: after osdb
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK90_50_L, E_XC_DIP_OP2 << 8, BMASK(9: 8)); // Set to with osd for DIP select

    MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_MFDEC_FB_MIU(MFDEC_ID0), 3 << MFDEC_MIF0_BURST_SHIFT, MFDEC_MIF0_BURST_LEN);
    MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_MFDEC_FB_MIU(MFDEC_ID0), 1 << MFDEC_MIF1_BURST_SHIFT, MFDEC_MIF1_BURST_LEN);
    MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_MFDEC_FB_MIU(MFDEC_ID1), 3 << MFDEC_MIF0_BURST_SHIFT, MFDEC_MIF0_BURST_LEN);
    MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_MFDEC_FB_MIU(MFDEC_ID1), 1 << MFDEC_MIF1_BURST_SHIFT, MFDEC_MIF1_BURST_LEN);
    //MDrv_DIP_Write2ByteMask(REG_MFDEC_EX_FB_MIU(0), 3 << MFDEC_MIF0_BURST_SHIFT, MFDEC_MIF0_BURST_LEN);
    //MDrv_DIP_Write2ByteMask(REG_MFDEC_EX_FB_MIU(0), 1 << MFDEC_MIF1_BURST_SHIFT, MFDEC_MIF1_BURST_LEN);

}

void HAL_XC_DIP_EnableCaptureStream(void *pstCmdQInfo, MS_BOOL bEnable, SCALER_DIP_WIN eWindow)
{
    MS_U16 u16IntrStus = 0;
    MS_XC_DIP_SOURCE_TYPE eHalSource;
    SCALER_DIP_SOURCE_TYPE eWindowSource = SCALER_DIP_SOURCE_TYPE_MAIN;
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    eWindowSource = DipSource;
#ifdef CONFIG_MSTAR_SRAMPD
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_7E_L, 0, BMASK(7: 4));
#endif
    MDrv_WriteRegBit_CmdQ(pstCmdQ, REG_CKG_IDCLK3, DISABLE, CKG_IDCLK3_GATED);
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK6C_40_L, 0, BIT(7));    //Enable clock

    switch(eWindowSource)
    {
        case SCALER_DIP_SOURCE_TYPE_DRAM:
        case SCALER_DIP_SOURCE_TYPE_SUB2 :
        case SCALER_DIP_SOURCE_TYPE_OP_CAPTURE:
        case SCALER_DIP_SOURCE_TYPE_SC2_OP_CAPTURE:
        case SCALER_DIP_SOURCE_TYPE_OSD:
        case SCALER_DIP_SOURCE_TYPE_OP_SC1_CAPTURE:
        case SCALER_DIP_SOURCE_TYPE_MVOP0:
        case SCALER_DIP_SOURCE_TYPE_MVOP1:
            eHalSource = E_XC_DIP_SOURCE_TYPE_SUB2;
            break;
        case SCALER_DIP_SOURCE_TYPE_MAIN :
            eHalSource = E_XC_DIP_SOURCE_TYPE_MAIN;
            break;
        case SCALER_DIP_SOURCE_TYPE_SUB :
            eHalSource = E_XC_DIP_SOURCE_TYPE_SUB;
            break;
        case SCALER_DIP_SOURCE_TYPE_OP_MAIN:
        case SCALER_DIP_SOURCE_TYPE_OP_SUB:
            eHalSource = E_XC_DIP_SOURCE_TYPE_OP;
            break;
        default:
            printf("SCALER_DIP_SOURCE_TYPE does not support\n");
            return;
    }


    if(bEnable == TRUE)
    {
        //Control DIPR
        if(eWindowSource == SCALER_DIP_SOURCE_TYPE_DRAM)
        {
            // Because Kano DIP has DIP multi view(4-way), In order to achieve performace, so adjust fclk to 320M
            MDrv_WriteByteMask_CmdQ(pstCmdQ, REG_CKG_DIP_FCLK, CKG_DIP_FCLK_345MHZ, CKG_DIP_FCLK_MASK);
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7D_L, BIT(14), BIT(14));
        }
        else if( (eWindowSource == SCALER_DIP_SOURCE_TYPE_OP_MAIN) || (eWindowSource == SCALER_DIP_SOURCE_TYPE_OP_SUB) )
        {
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_7C_L, ((eWindowSource == SCALER_DIP_SOURCE_TYPE_OP_SUB) ? BIT(15) : 0), BIT(15));
            //new rotation enabel
            //SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_6E_L, bDIPRotation ? BIT(11) : 0, BIT(11));
        }

        //Enable last
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_02_L, BITS(9: 8, eHalSource), BMASK(10: 8));
    }
    else
    {
        //Disable first
        //new rotation disable
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_6E_L, 0, BIT(11));

        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_02_L, BIT(10), BMASK(10: 8));
        //disable write once
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_03_L, 0, BIT(8));

        if(eWindowSource == SCALER_DIP_SOURCE_TYPE_DRAM)
        {
            // Because Kano DIP has DIP multi view(4-way), In order to achieve performace, so adjust fclk to 320M
            // disable DIPR so adjust fclk to 216M
            MDrv_WriteByteMask_CmdQ(pstCmdQ, REG_CKG_DIP_FCLK, CKG_DIP_FCLK_345MHZ, CKG_DIP_FCLK_MASK);
            //Dipr disable
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7D_L, 0, BIT(14));
        }
    }

    if (!bEnable)
    {
        //Auto clear status to zero
        u16IntrStus = HAL_XC_DIP_GetIntrStatus(pstCmdQInfo, eWindow);
        HAL_XC_DIP_ClearIntr(pstCmdQInfo, u16IntrStus, eWindow);
        HAL_XC_DIP_SWReset(pstCmdQInfo, eWindow);
    }

}

void HAL_XC_DIP_EnableIntr(void *pstCmdQInfo, MS_U16 u8mask, MS_BOOL bEnable, SCALER_DIP_WIN eWindow)
{
    MS_U16 regval = 0;

    regval = SC_R2BYTE(0, REG_SC_BK36_08_L);

    if(bEnable)
        regval &= ~u8mask;
    else
        regval |= u8mask;

    SC_W2BYTEMSK(0, REG_SC_BK36_08_L, regval, BMASK(7: 0));

}

void HAL_XC_DIP_ClearIntr(void *pstCmdQInfo, MS_U16 u16mask, SCALER_DIP_WIN eWindow)
{
    SC_W2BYTEMSK(0, REG_SC_BK36_09_L, u16mask & BMASK(7: 0), BMASK(7: 0));
}

MS_U16 HAL_XC_DIP_GetIntrStatus(void *pstCmdQInfo, SCALER_DIP_WIN eWindow)
{
    MS_U16 u16IntrStus = 0;

    u16IntrStus = SC_R2BYTEMSK(0, REG_SC_BK36_0A_L, BMASK(7: 0));

    return u16IntrStus;
}

void HAL_XC_DIP_CMDQ_EnableIntr(void *pstCmdQInfo, MS_U16 u8mask, MS_BOOL bEnable, SCALER_DIP_WIN eWindow)
{
    MS_U16 regval = 0;

    regval = SC_R2BYTE(0, REG_SC_BK36_28_L);

    if(bEnable)
        regval &= ~u8mask;
    else
        regval |= u8mask;

    SC_W2BYTEMSK(0, REG_SC_BK36_28_L, regval, BMASK(7: 0));

}

void HAL_XC_DIP_CMDQ_ClearIntr(void *pstCmdQInfo, MS_U16 u16mask, SCALER_DIP_WIN eWindow)
{
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_29_L, u16mask & BMASK(7: 0), BMASK(7: 0));
}

MS_U16 HAL_XC_DIP_CMDQ_GetIntrStatus(void *pstCmdQInfo, SCALER_DIP_WIN eWindow)
{
    MS_U16 u16IntrStus = 0;

    u16IntrStus = SC_R2BYTEMSK(0, REG_SC_BK36_2A_L, BMASK(7: 0));

    return u16IntrStus;
}

void HAL_XC_DIP_MDWIN_EnableIntr(void *pstCmdQInfo, MS_BOOL bEnable, SCALER_DIP_WIN eWindow)
{
    MS_U16 regval = 0;
    MS_U16 u16mask = BIT(13);

    regval = SC_R2BYTE(0, REG_SC_BKF6_70_L);

    if(bEnable)
        regval &= ~u16mask;
    else
        regval |= u16mask;

    SC_W2BYTEMSK(0, REG_SC_BKF6_70_L, regval, u16mask);

}

void HAL_XC_DIP_MDWIN_ClearIntr(void *pstCmdQInfo, SCALER_DIP_WIN eWindow)
{
    SC_W2BYTEMSK(0, REG_SC_BKF6_70_L, BIT(14), BIT(14));
}

MS_U16 HAL_XC_DIP_MDWIN_GetIntrStatus(void *pstCmdQInfo, SCALER_DIP_WIN eWindow)
{
    MS_U16 u16IntrStus = 0;

    u16IntrStus = SC_R2BYTEMSK(0, REG_SC_BKF6_70_L, BIT(15));

    return u16IntrStus;
}

void HAL_XC_DIP_CpatureOneFrame(void *pstCmdQInfo, SCALER_DIP_WIN eWindow)//MS_BOOL benable, SCALER_DIP_WIN eWindow)
{
    MS_U16 regval = 0;
    MS_U16 u16Count = 0;

    HAL_XC_DIP_CpatureOneFrame2(pstCmdQInfo, eWindow);

    do
    {
        regval = HAL_XC_DIP_GetIntrStatus(pstCmdQInfo, eWindow);
        if(regval > 0x0)
            break;
        u16Count++;
        DIP_DelayTask(1);
    }
    while(u16Count < DWIN_CAPTURE_TIMEOUT_CNT);
    //if(u16Count >= DWIN_CAPTURE_TIMEOUT_CNT)
    //    printf("!!!Alert !!! DWIN Capture, wait ack time out!\n");

}

void HAL_XC_DIP_CpatureOneFrame2(void *pstCmdQInfo, SCALER_DIP_WIN eWindow)//MS_BOOL benable, SCALER_DIP_WIN eWindow)
{
    MS_XC_DIP_SOURCE_TYPE eHalSource;
    SCALER_DIP_SOURCE_TYPE eWindowSource = SCALER_DIP_SOURCE_TYPE_MAIN;
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    eWindowSource = DipSource;
#ifdef CONFIG_MSTAR_SRAMPD
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_7E_L, 0, BMASK(7: 4));
#endif

#if 1
    g_u8REG_CKG_IDCLK3 &= ~(CKG_IDCLK3_GATED);
    MDrv_WriteByteMask_CmdQ(pstCmdQ, REG_CKG_IDCLK3, g_u8REG_CKG_IDCLK3, (CKG_IDCLK3_MASK | CKG_IDCLK3_GATED));
#else
    MDrv_WriteRegBit_CmdQ(pstCmdQ, REG_CKG_IDCLK3, DISABLE, CKG_IDCLK3_GATED);  // Enable clock
#endif

    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK6C_40_L, 0, BIT(7));    //Enable clock

    switch(eWindowSource)
    {
        case SCALER_DIP_SOURCE_TYPE_DRAM:
        case SCALER_DIP_SOURCE_TYPE_SUB2 :
        case SCALER_DIP_SOURCE_TYPE_OP_CAPTURE:
        case SCALER_DIP_SOURCE_TYPE_SC2_OP_CAPTURE:
        case SCALER_DIP_SOURCE_TYPE_OSD:
        case SCALER_DIP_SOURCE_TYPE_OP_SC1_CAPTURE:
        case SCALER_DIP_SOURCE_TYPE_MVOP0:
        case SCALER_DIP_SOURCE_TYPE_MVOP1:
            eHalSource = E_XC_DIP_SOURCE_TYPE_SUB2;
            break;
        case SCALER_DIP_SOURCE_TYPE_MAIN :
            eHalSource = E_XC_DIP_SOURCE_TYPE_MAIN;
            break;
        case SCALER_DIP_SOURCE_TYPE_SUB :
            eHalSource = E_XC_DIP_SOURCE_TYPE_SUB;
            break;
        case SCALER_DIP_SOURCE_TYPE_OP_MAIN:
        case SCALER_DIP_SOURCE_TYPE_OP_SUB:
            eHalSource = E_XC_DIP_SOURCE_TYPE_OP;
            break;
        default:
            printf("SCALER_DIP_SOURCE_TYPE does not support\n");
            return;
    }

    //Control OP1 Bank and DIPR
    if(eWindowSource == SCALER_DIP_SOURCE_TYPE_DRAM)
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7D_L, BIT(14), BIT(14));
    }
    else if( (eWindowSource == SCALER_DIP_SOURCE_TYPE_OP_MAIN) || (eWindowSource == SCALER_DIP_SOURCE_TYPE_OP_SUB) )
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_7C_L, ((eWindowSource == SCALER_DIP_SOURCE_TYPE_OP_SUB) ? BIT(15) : 0), BIT(15));
    }

    //Enable last
#if 0 // mark for I2
    //new rotation enabel
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_6E_L, bDIPRotation ? BIT(11) : 0, BIT(11));
#endif

    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_02_L, BITS(9: 8, eHalSource), BMASK(10: 8));
    if(DipSource == SCALER_DIP_SOURCE_TYPE_DRAM)
    {
        //Dipr trigger
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7C_L, BIT(15), BIT(15));
    }
    else
    {
        // Set the dwin write once   (0x03 bit:8)
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_03_L, BIT(8), BIT(8));
        //trigger
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_03_L, BIT(9), BIT(9));
    }

}

void HAL_XC_DIP_SelectSourceScanType(void *pstCmdQInfo, EN_XC_DWIN_SCAN_TYPE enScan, SCALER_DIP_WIN eWindow)
{
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_01_L, enScan ? BIT(15) : 0, BIT(15));

}

EN_XC_DWIN_SCAN_TYPE HAL_XC_DIP_GetSourceScanType(void *pstCmdQInfo, SCALER_DIP_WIN eWindow)
{
    EN_XC_DWIN_SCAN_TYPE eSacnType = GOPDWIN_SCAN_MODE_MAX;
    eSacnType = (EN_XC_DWIN_SCAN_TYPE)(SC_R2BYTEMSK(0, REG_SC_BK36_01_L, BIT(15)) >> 15);

    return eSacnType;
}

void HAL_XC_DIP_SetInterlaceWrite(void *pstCmdQInfo, MS_BOOL bEnable, SCALER_DIP_WIN eWindow)
{
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_01_L, (bEnable ? BIT(3) : 0), BIT(3));

}

MS_BOOL HAL_XC_DIP_GetInterlaceWrite(void *pstCmdQInfo, SCALER_DIP_WIN eWindow)
{
    MS_BOOL bInterW = FALSE;
    bInterW = (MS_BOOL)(SC_R2BYTEMSK(0, REG_SC_BK36_01_L, BIT(3)) >> 3);

    return bInterW;
}

void HAL_XC_DIP_SetDataFmt(void *pstCmdQInfo, EN_DRV_XC_DWIN_DATA_FMT fmt, DIP_OUTPUT_PATH eOutPath, SCALER_DIP_WIN eWindow)
{
    MS_U16 u16Fmt = 0x0;
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    switch (fmt)
    {
        case XC_DWIN_DATA_FMT_YUV422 :
        case XC_DWIN_DATA_FMT_YC422 :
            u16Fmt = 0;
            break;
        case XC_DWIN_DATA_FMT_RGB565 :
            u16Fmt = 1;
            break;
        case XC_DWIN_DATA_FMT_ARGB8888 :
            u16Fmt = 2;
            break;
        case XC_DWIN_DATA_FMT_YUV420 :
        case XC_DWIN_DATA_FMT_YUV420_H265 :
        case XC_DWIN_DATA_FMT_YUV420_H265_10BITS:
        case XC_DWIN_DATA_FMT_YUV420_PLANER:
        case XC_DWIN_DATA_FMT_YUV420_SEMI_PLANER:
            u16Fmt = 3;
            break;
        default :
            u16Fmt = 0x0;
            break;
    }

    DipFmt = fmt;
    //set fmt
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_01_L, BITS(5: 4, u16Fmt), BMASK(5: 4));
    //select yc separate
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_03_L, ((fmt == XC_DWIN_DATA_FMT_YC422) ? BIT(6) : 0), BIT(6));
    //select de-tile for 420 semi planer
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_03_L, ((fmt == XC_DWIN_DATA_FMT_YUV420_SEMI_PLANER) ? BIT(14) : 0), BIT(14));
    //select flash mode for 420 planer
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7E_L, ((fmt == XC_DWIN_DATA_FMT_YUV420_PLANER) ? BIT(14) : 0), BIT(14));

#if (DIP_PLATFORM == DIP_K6)
    if(bDIPEnaDI)  // turn off YUV444 to YUV422 if 3DDI enable
    {
        //enable 444 to 422 block after DI
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_30_L, 0, BIT(0));
    }
    else
#endif
    {
        //if(bDIPMfdecEnable == FALSE)
        {
            //enable 444 to 422 block before DI(for I2)
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_30_L, BIT(0), BIT(0));
        }
    }

#if (DIP_PLATFORM == DIP_I2)
    //enable 422 to 444 block for osdb, 0: disable, 1: duplicate
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_68_L, BIT(0), BIT(0));
#endif

    if( (fmt == XC_DWIN_DATA_FMT_RGB565) || (fmt == XC_DWIN_DATA_FMT_ARGB8888) )
    {
        //disable yc swap setting
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_02_L, 0, BMASK(15: 14));
        //dip_pdw source data select
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_30_L, 0, BIT(5));
#if (DIP_PLATFORM == DIP_K6)
        //enable 422 to 444
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_68_L, BIT(0), BIT(0));
#endif
#if (DIP_PLATFORM == DIP_I2)
        //disable 444 to 422 for osdb
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_0B_L, 0, BIT(12));
#endif
    }
    else
    {
        //disable rgb swap setting
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7E_L, 0, BIT(13));
        if(eOutPath == DIP_OUTPUT_PATH_MDWIN)
        {
            //dip_pdw source data select
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_30_L, 0, BIT(5));  // [5]: 0 for MDWin YUV input order(VUY), 1: for DIPW YUV input order(VYU)
        }
        else
        {
            //dip_pdw source data select
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_30_L, BIT(5), BIT(5));  // [5]: 0 for MDWin YUV input order(VUY), 1: for DIPW YUV input order(VYU)
        }
#if (DIP_PLATFORM == DIP_K6)
        //enable 422 to 444
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_68_L, 0, BIT(0));
#endif
#if (DIP_PLATFORM == DIP_I2)
        if(eOutPath == DIP_OUTPUT_PATH_MDWIN)
        {
            //disable 444 to 422 for MDWin
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_0B_L, 0, BIT(12));  // [12]: 0 for MDWin YUV444 input request, 1: for DIPW YUV422 input request
            // Set the dwin write once   (0x03 bit:8)
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_03_L, BIT(8), BIT(8));
            //SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_01_L, BIT(7), BIT(7));  // [7]: disable DIP write byte for MDWin test
        }
        else
        {
            //enable 444 to 422 for osdb
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_0B_L, BIT(12), BIT(12));  // [12]: 0 for MDWin YUV444 input request, 1: for DIPW YUV422 input request
            // Set the dwin write once   (0x03 bit:8)
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_03_L, 0, BIT(8));
            //SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_01_L, 0, BIT(7));  // [7]: disable DIP write byte for MDWin test
        }
#endif
    }

}

EN_DRV_XC_DWIN_DATA_FMT HAL_XC_DIP_GetDataFmt(void *pstCmdQInfo, SCALER_DIP_WIN eWindow)
{
    return DipFmt;
}

MS_U16 HAL_XC_DIP_GetBPP(void *pstCmdQInfo, EN_DRV_XC_DWIN_DATA_FMT fbFmt, SCALER_DIP_WIN eWindow)
{
    MS_U16 bpp = 0;

    switch ( fbFmt )
    {
        case XC_DWIN_DATA_FMT_YUV422 :
        case XC_DWIN_DATA_FMT_YC422  :
        case XC_DWIN_DATA_FMT_RGB565 :
            bpp = 2;
            break;
        case XC_DWIN_DATA_FMT_ARGB8888 :
            bpp = 4;
            break;
        case XC_DWIN_DATA_FMT_YUV420 :
        case XC_DWIN_DATA_FMT_YUV420_H265 :
        case XC_DWIN_DATA_FMT_YUV420_H265_10BITS :
        case XC_DWIN_DATA_FMT_YUV420_PLANER :
        case XC_DWIN_DATA_FMT_YUV420_SEMI_PLANER :
            bpp = 1;
            break;
        default :
            bpp = 0xFF;
            break;
    }
    return bpp;
}

MS_U16 HAL_XC_DIP_WidthAlignCheck(void *pstCmdQInfo, MS_U16 u16Width, MS_U16 u16Bpp, SCALER_DIP_WIN eWindow)
{
    MS_U16 AlignFactor;
    MS_U16 u16BusSize = 0;

    u16BusSize = HAL_XC_DIP_GetBusSize(pstCmdQInfo, eWindow);
    AlignFactor = u16BusSize;
    return (u16Width + AlignFactor - 1) & (~(AlignFactor - 1));
}

void HAL_XC_DIP_DumpIPMUXReg(void)
{
    if(debug_level >= 1)
    {
        printf("DIP_Reg(REG_IPMUX_02_L) = x%x\n", MDrv_ReadByte(REG_IPMUX_02_L));
        printf("DIP_Reg(REG_CKG_IDCLK3) = x%x\n", MDrv_ReadByte(REG_CKG_IDCLK3));
    }
}

void HAL_XC_DIP_SetMux(void *pstCmdQInfo, MS_U8 u8Data_Mux, MS_U8 u8Clk_Mux, SCALER_DIP_WIN eWindow)
{
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

#if 1 //
    MDrv_WriteByteMask_CmdQ(pstCmdQ, REG_IPMUX_02_L, u8Data_Mux << 4, 0xF0);
    g_u8REG_CKG_IDCLK3 &= ~(CKG_IDCLK3_MASK);
    g_u8REG_CKG_IDCLK3 |= (u8Clk_Mux);
    //MDrv_WriteByteMask_CmdQ(pstCmdQ, REG_CKG_IDCLK3, u8Clk_Mux, CKG_IDCLK3_MASK);
#else
    MDrv_WriteByteMask_CmdQ(pstCmdQ, REG_IPMUX_02_L, u8Data_Mux << 4, 0xF0);
    MDrv_WriteByteMask_CmdQ(pstCmdQ, REG_CKG_IDCLK3, u8Clk_Mux, CKG_IDCLK3_MASK);
#endif

#if 0  // add cmdq timeout setting for delay function

    if(pstCmdQ != NULL)
    {
        printf("DIP_DummyReg = x%x\n", SC_R2BYTE(0, REG_SC_BK34_7D_L));

        //patch for I2 start :
        //add delay(0.1ms) for each frame. solution is from tommy.wang.
        pstCmdQ->MHAL_CMDQ_WriteRegCmdq(pstCmdQ, 0x123C50, 0xFFFF);
        pstCmdQ->MHAL_CMDQ_WriteRegCmdq(pstCmdQ, 0x123C52, 0x8080);
        pstCmdQ->MHAL_CMDQ_CmdqPollRegBits(pstCmdQ, 0x1334FA, 0x0001,  0xFFF, TRUE);
        pstCmdQ->MHAL_CMDQ_WriteRegCmdq(pstCmdQ, 0x123C50, 0x0);
        pstCmdQ->MHAL_CMDQ_WriteRegCmdq(pstCmdQ, 0x123C52, 0x8000);
        //patch for I2 end :

        printf("DIP_DummyReg = x%x\n", SC_R2BYTE(0, REG_SC_BK34_7D_L));
    }

#endif

}

MS_BOOL HAL_XC_DIP_GetCaptureSourceStatus(void *pstCmdQInfo, SCALER_DIP_SOURCE_TYPE eSource, SCALER_DIP_WIN eWindow, MS_BOOL* bstatus)
{
    switch(eSource)
    {
        case SCALER_DIP_SOURCE_TYPE_MVOP0:
        {
            *bstatus = g_DIPChipPro.bSourceSel.bSOURCE_TYPE_MVOP[0][eWindow];
        }
        break;
        case SCALER_DIP_SOURCE_TYPE_MVOP1:
        {
            *bstatus = g_DIPChipPro.bSourceSel.bSOURCE_TYPE_MVOP[1][eWindow];
        }
        break;
        case SCALER_DIP_SOURCE_TYPE_MAIN:
        {
            *bstatus = g_DIPChipPro.bSourceSel.bSOURCE_TYPE_IP_MAIN[0][eWindow];
        }
        break;
        case SCALER_DIP_SOURCE_TYPE_SUB:
        {
            *bstatus = g_DIPChipPro.bSourceSel.bSOURCE_TYPE_IP_SUB[0][eWindow];
        }
        break;
        case SCALER_DIP_SOURCE_TYPE_OP_MAIN:
        {
            *bstatus = g_DIPChipPro.bSourceSel.bSOURCE_TYPE_OP_MAIN[0][eWindow];
        }
        break;
        case SCALER_DIP_SOURCE_TYPE_OP_SUB:
        {
            *bstatus = g_DIPChipPro.bSourceSel.bSOURCE_TYPE_OP_SUB[0][eWindow];
        }
        break;
        case SCALER_DIP_SOURCE_TYPE_OP_CAPTURE:
        {
            *bstatus = g_DIPChipPro.bSourceSel.bSOURCE_TYPE_OP_CAPTURE[0][eWindow];
        }
        break;
        case SCALER_DIP_SOURCE_TYPE_OP_SC1_CAPTURE:
        {
            *bstatus = g_DIPChipPro.bSourceSel.bSOURCE_TYPE_OP_CAPTURE[1][eWindow];
        }
        break;
        case SCALER_DIP_SOURCE_TYPE_SC2_OP_CAPTURE :
        {
            if(XC_PATH_COUNT_SUPPT <= 2)
            {
                *bstatus = FALSE;
            }
            else
            {
                *bstatus = g_DIPChipPro.bSourceSel.bSOURCE_TYPE_OP_CAPTURE[XC_PATH_COUNT_SUPPT - 1][eWindow];
            }
        }
        break;
        case SCALER_DIP_SOURCE_TYPE_DRAM  :
        {
            *bstatus = g_DIPChipPro.bSourceSel.bSOURCE_TYPE_DRAM[eWindow];
        }
        break;
        default:
            printf("[%s][%d] eWindow =%d ---->eSource = %d is not Support\n", __FUNCTION__, __LINE__, eWindow, eSource);
            *bstatus = FALSE;
            return FALSE;
            break;

    }

    return TRUE;
}

void HAL_XC_DIP_MuxDispatch(void *pstCmdQInfo, SCALER_DIP_SOURCE_TYPE eSource, SCALER_DIP_WIN eWindow)
{
    MS_BOOL eRet = FALSE;
    //MS_U8 u8Clk = 0;
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    switch(eSource)
    {
        case SCALER_DIP_SOURCE_TYPE_MVOP0:  // MVOP
        {
            if(g_DIPChipPro.bSourceSel.bSOURCE_TYPE_MVOP[0][eWindow] != TRUE)
            {
                eRet = FALSE;
            }
            else
            {
                HAL_XC_DIP_SetMux(pstCmdQInfo, SC_DWIN_IPMUX_MVOP, CKG_IDCLK3_CLK_DC0, eWindow);
                eRet = TRUE;
            }
        }

        break;
        case SCALER_DIP_SOURCE_TYPE_MVOP1:  // --> MVOP_SUB
        {
            if(g_DIPChipPro.bSourceSel.bSOURCE_TYPE_MVOP[1][eWindow] != TRUE)
            {
                eRet = FALSE;
            }
            else
            {
                HAL_XC_DIP_SetMux(pstCmdQInfo, SC_DWIN_IPMUX_MVOP2, CKG_IDCLK3_CLK_DC1, eWindow);
                eRet = TRUE;
            }
        }
        break;

        case SCALER_DIP_SOURCE_TYPE_MAIN:  //--> IP2
        {
            if(g_DIPChipPro.bSourceSel.bSOURCE_TYPE_IP_MAIN[0][eWindow] != TRUE)
            {
                eRet = FALSE;
            }
            else
            {
                HAL_XC_DIP_SetMux(pstCmdQInfo, SC_DWIN_IPMUX_IP_MAIN, CKG_IDCLK3_ODCLK, eWindow);
                SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_50_L, 0, BIT(15));  // select avg 2pto1p output result
                eRet = TRUE;
            }
        }
        break;

#if (DIP_PLATFORM == DIP_K6)
        case SCALER_DIP_SOURCE_TYPE_SUB:
        {
            if(g_DIPChipPro.bSourceSel.bSOURCE_TYPE_IP_SUB[0][eWindow] != TRUE)
            {
                eRet = FALSE;
            }
            else
            {
                HAL_XC_DIP_SetMux(pstCmdQInfo, SC_DWIN_IPMUX_IP_SUB, CKG_IDCLK3_ODCLK, eWindow);
                SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_50_L, BIT(15), BIT(15));  // select avg 2pto1p output result
                eRet = TRUE;
            }
        }
        break;
#endif

        case SCALER_DIP_SOURCE_TYPE_OP_MAIN:
            if(g_DIPChipPro.bSourceSel.bSOURCE_TYPE_OP_MAIN[0][eWindow] != TRUE)
            {
                eRet = FALSE;
            }
            eRet = TRUE;
            break;
        case SCALER_DIP_SOURCE_TYPE_OP_SUB:
        {
            if(g_DIPChipPro.bSourceSel.bSOURCE_TYPE_OP_SUB[0][eWindow] != TRUE)
            {
                eRet = FALSE;
            }

            eRet = TRUE;
        }
        break;

        case SCALER_DIP_SOURCE_TYPE_DRAM:
        {
            if(g_DIPChipPro.bSourceSel.bSOURCE_TYPE_DRAM[eWindow] != TRUE)
            {
                eRet = FALSE;
            }
            else
            {
            }
            eRet = TRUE;
        }
        break;

        case SCALER_DIP_SOURCE_TYPE_OP_CAPTURE:  // --> VOP2
        {
            if(g_DIPChipPro.bSourceSel.bSOURCE_TYPE_OP_CAPTURE[0][eWindow] != TRUE)
            {
                eRet = FALSE;
            }
            else
            {
                HAL_XC_DIP_SetMux(pstCmdQInfo, SC_DWIN_IPMUX_OP1, CKG_IDCLK3_00, eWindow);
                eRet = TRUE;
            }

        }
        break;

        case SCALER_DIP_SOURCE_TYPE_OP_SC1_CAPTURE:  // --> SC1 OP
        {
            if(g_DIPChipPro.bSourceSel.bSOURCE_TYPE_OP_CAPTURE[1][eWindow] != TRUE)
            {
                eRet = FALSE;
            }
            else
            {
                HAL_XC_DIP_SetMux(pstCmdQInfo, SC_DWIN_IPMUX_SC1_CAPTURE, CKG_IDCLK3_XTAL, eWindow);
                eRet = TRUE;
            }
        }
        break;

        case SCALER_DIP_SOURCE_TYPE_SC2_OP_CAPTURE:
        {
            if(XC_PATH_COUNT_SUPPT <= 2)
            {
                eRet = FALSE;
            }
            else if( XC_PATH_COUNT_SUPPT <= 3)
            {
                eRet = FALSE;
            }
            else
            {
                eRet = TRUE;
            }
        }
        break;

        case SCALER_DIP_SOURCE_TYPE_OSD:
        {
            if(g_DIPChipPro.bSourceSel.bSOURCE_TYPE_OSD[eWindow] != TRUE)
            {
                eRet = FALSE;
            }
            else
            {
                eRet = TRUE;
            }

        }
        break;
        default:
            eRet = FALSE;
            break;
    }

    if(eRet == FALSE)
    {
        printf("[%s][%d] eWindow =%d ---->eSource = %d is not Support\n", __FUNCTION__, __LINE__, eWindow, eSource);
    }
}

void HAL_XC_DIP_Averge2PTo1P(void *pstCmdQInfo, MS_BOOL bAVG, SCALER_DIP_WIN eWindow)
{
    SC_DIP_SWDB_INFO DBreg;
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    if(bAVG == TRUE)
    {
        Hal_SC_DWIN_get_sw_db(pstCmdQInfo, &DBreg, eWindow);
        DBreg.u16H_CapStart /= 2;
        if (DBreg.u16H_CapSize % 2 )
            DBreg.u16H_CapSize += 1;
        DBreg.u16H_CapSize /= 2;
        if( (g_DIPSrcInfo[eWindow].u16PreHCusScalingSrc / 2) > g_DIPSrcInfo[eWindow].u16PreHCusScalingDst )
        {
            DBreg.u32H_PreScalingRatio = H_PreScalingDownRatioDIP(g_DIPSrcInfo[eWindow].u16PreHCusScalingSrc / 2, g_DIPSrcInfo[eWindow].u16PreHCusScalingDst);
            DBreg.u32H_PreScalingRatio &= 0x7FFFFFL;
            DBreg.u32H_PreScalingRatio |= (BIT(31));
        }
        else
            DBreg.u32H_PreScalingRatio = 0x100000L;
        Hal_SC_DWIN_sw_db(pstCmdQInfo, &DBreg, eWindow);
    }

    if(bAVG == TRUE)
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_50_L, BIT(15), BIT(15));
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_6D_L, BIT(14), BIT(14));
    }
    else
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_50_L, 0, BIT(15));
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_6D_L, 0, BIT(14));
    }

}

void HAL_XC_DIP_HSPScaling(void *pstCmdQInfo, MS_BOOL bEnable, MS_U16 ScalingSrc, MS_U16 ScalingDst, SCALER_DIP_WIN eWindow)
{
    MS_U32 u32Ratio = 0;
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    u32Ratio = (H_DIP_PostScalingRatio(ScalingSrc, ScalingDst));

    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK49_22_L, ScalingSrc);
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK49_24_L, ScalingDst);

    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK49_07_L, u32Ratio & 0xffff);
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK49_08_L, ((u32Ratio & 0xff0000) >> 16), BMASK(7: 0));

    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK49_08_L, bEnable ? BIT(8) : 0, BIT(8));

}

void HAL_XC_DIP_VSPScaling(void *pstCmdQInfo, MS_BOOL bEnable, MS_U16 ScalingSrc, MS_U16 ScalingDst, SCALER_DIP_WIN eWindow)
{
    MS_U32 u32Ratio = 0;
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    u32Ratio = (V_DIP_PostScalingRatio(ScalingSrc, ScalingDst));

    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK49_23_L, ScalingSrc);
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK49_25_L, ScalingDst);

    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK49_09_L, u32Ratio & 0xffff);
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK49_0A_L, ((u32Ratio & 0xff0000) >> 16), BMASK(7: 0));

    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK49_0A_L, bEnable ? BIT(8) : 0, BIT(8));

}

void HAL_XC_DIP_AdjustScaling(void *pstCmdQInfo, MS_BOOL bAVG, SCALER_DIP_WIN eWindow)
{
    SCALER_DIP_WIN eHVSPWindow = MAX_DIP_WINDOW;
    MS_U16 HCusScalingSrc = 0;
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    eHVSPWindow = HAL_XC_DIP_GetHVSP(pstCmdQInfo, eWindow);
    if(eHVSPWindow != MAX_DIP_WINDOW)
    {
#if (DIP_PLATFORM == DIP_I2)

        if(bAVG == TRUE)
            HCusScalingSrc = g_DIPSrcInfo[eWindow].u16HCusScalingSrc / 2;
        else
            HCusScalingSrc = g_DIPSrcInfo[eWindow].u16HCusScalingSrc;

        if(bDIPEnaDI == TRUE)
        {
            g_DIPSrcInfo[eWindow].u16VCusScalingSrc <<= 1;
        }

        if((HCusScalingSrc != g_DIPSrcInfo[eWindow].u16HCusScalingDst) || (g_DIPSrcInfo[eWindow].u16VCusScalingSrc != g_DIPSrcInfo[eWindow].u16VCusScalingDst))
        {
            //SC_DIP_SWDB_INFO DBreg;
            //Hal_SC_DWIN_get_sw_db(pstCmdQInfo, &DBreg, eWindow);
            //DBreg.u32H_PreScalingRatio = 0x100000L;
            //DBreg.u32V_PreScalingRatio = 0x100000L;
            //Hal_SC_DWIN_sw_db(pstCmdQInfo, &DBreg, eWindow);
            HAL_XC_DIP_HSPScaling(pstCmdQInfo, TRUE, HCusScalingSrc, g_DIPSrcInfo[eWindow].u16HCusScalingDst, eWindow);
            HAL_XC_DIP_VSPScaling(pstCmdQInfo, TRUE, g_DIPSrcInfo[eWindow].u16VCusScalingSrc, g_DIPSrcInfo[eWindow].u16VCusScalingDst, eWindow);
        }

#else

        if(bAVG == TRUE)
            HCusScalingSrc = g_DIPSrcInfo[eWindow].u16PreHCusScalingSrc / 2;
        else
            HCusScalingSrc = g_DIPSrcInfo[eWindow].u16PreHCusScalingSrc;

        if(bDIPEnaDI == TRUE)
        {
            g_DIPSrcInfo[eWindow].u16PreVCusScalingSrc <<= 1;
            g_DIPSrcInfo[eWindow].u16PreVCusScalingDst <<= 1;
        }

        if((HCusScalingSrc < g_DIPSrcInfo[eWindow].u16PreHCusScalingDst) || (g_DIPSrcInfo[eWindow].u16PreVCusScalingSrc < g_DIPSrcInfo[eWindow].u16PreVCusScalingDst))
        {
            SC_DIP_SWDB_INFO DBreg;
            Hal_SC_DWIN_get_sw_db(pstCmdQInfo, &DBreg, eWindow);
            DBreg.u32H_PreScalingRatio = 0x100000L;
            DBreg.u32V_PreScalingRatio = 0x100000L;
            Hal_SC_DWIN_sw_db(pstCmdQInfo, &DBreg, eWindow);
            HAL_XC_DIP_HSPScaling(pstCmdQInfo, TRUE, g_DIPSrcInfo[eWindow].u16PreHCusScalingSrc, g_DIPSrcInfo[eWindow].u16PreHCusScalingDst, eWindow);
            HAL_XC_DIP_VSPScaling(pstCmdQInfo, TRUE, g_DIPSrcInfo[eWindow].u16PreVCusScalingSrc, g_DIPSrcInfo[eWindow].u16PreVCusScalingDst, eWindow);
        }

#endif

        if(debug_level >= 1)
        {
            printf("== Enable HVSP ==\n");
        }
    }
    else
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK49_08_L, 0, BIT(8));
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK49_0A_L, 0, BIT(8));

        if(debug_level >= 1)
        {
            printf("== Disable HVSP ==\n");
        }
    }
}

void HAL_XC_DIP_SetWinProperty(void *pstCmdQInfo, MS_U8 u8BufCnt, MS_U16 u16Width, MS_U16 u16LineOft, MS_U16 u16Height, MS_U32 u64OffSet, SCALER_DIP_SOURCE_TYPE eSource, MS_BOOL bPIP, MS_BOOL b2P_Enable, SCALER_DIP_WIN eWindow)
{
    MS_U16 u16SrcWith = 0;
    MS_U16 u16MaxWith = 0;
    MS_BOOL bNeedAverageMode = FALSE;
    MS_U32 u32DIoffset = 0;
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    bPIPmode = bPIP;
    u16MaxWith = HAL_XC_DIP_GetWidth(pstCmdQInfo, eSource, eWindow);  // Get DIPR width

    bNeedAverageMode = IsNeedAverageMode(eSource, u16MaxWith);
    HAL_XC_DIP_Averge2PTo1P(pstCmdQInfo, bNeedAverageMode, eWindow);
    HAL_XC_DIP_AdjustScaling(pstCmdQInfo, bNeedAverageMode, eWindow);

    //printf("u16MaxWith = %d, bNeedAverageMode = %d\n", u16MaxWith, bNeedAverageMode);

    DipWBufCnt = u8BufCnt;
    DipSource = eSource;
    u16SrcWith = DipSrcWidth;
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_01_L, u8BufCnt - 1, BMASK(2: 0));

    if(eSource == SCALER_DIP_SOURCE_TYPE_DRAM)
    {
        //1: from dipr
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_7F_L, BIT(6), BIT(6));
        //    dipr to dip
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_7F_L, BIT(13), BMASK(13: 12));
        //disable  source yc swap
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_7C_L, 0, BIT(14));
    }
    else
    {
        //0: from IPMUX/OP1
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_7F_L, 0, BIT(6));
        //Monaco Mode DI input is 422
        if( ((eSource == SCALER_DIP_SOURCE_TYPE_OP_MAIN) || (eSource == SCALER_DIP_SOURCE_TYPE_OP_SUB)) && (bDipR2Y == FALSE) )
        {
            //enable  422 to 444
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_6C_L, BIT(9), BIT(9));
            //enable  source yc swap
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_7C_L, BIT(14), BIT(14));
        }
        else
        {
            //disable  422 to 444
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_6C_L, 0, BIT(9));
            //disable  source yc swap
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_7C_L, 0, BIT(14));
        }
    }

    if( (eSource == SCALER_DIP_SOURCE_TYPE_MAIN) || (eSource == SCALER_DIP_SOURCE_TYPE_SUB) )
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ,  REG_SC_BK34_7F_L, BIT(15), BMASK(15: 14));
    }

    //width
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_1F_L, u16Width);
    //height
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_2F_L, u16Height);

    //3DDI
    if(bDIPEnaDI == TRUE)
    {
        g_u16DIWidth = g_DIPSrcInfo[eWindow].u16PreHCusScalingDst;
        //write height
        g_u16DIHeight = g_DIPSrcInfo[eWindow].u16PreVCusScalingDst;
        SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK43_2F_L, g_u16DIHeight);
        //g_u16DIHeight = u16DIHeight;
        //read height
        SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK43_37_L, g_u16DIHeight);  //line count number of one frame for dipr
        //Write Line offset
        SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK43_3F_L, g_u16DIWidth);
        //Read Line offset
        SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK43_74_L, g_u16DIWidth);

        SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK43_1F_L, g_u16DIWidth);
        SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK43_27_L, g_u16DIWidth);  //fetch number of one line for dipr
        HAL_XC_DIP_DI_Read_FIFO(pstCmdQInfo, g_u16DIWidth, eWindow);

        u32DIoffset = g_u16DIWidth * g_u16DIHeight;

        SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK43_50_L, u32DIoffset);
        SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK43_60_L, u32DIoffset);
        SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK43_70_L, u32DIoffset);
        SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK43_72_L, u32DIoffset);
    }

    //pitch
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_3F_L, u16LineOft);
    //buffer offset
    SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK36_50_L, u64OffSet);

#if (DIP_PLATFORM == DIP_I2)
    // reg_dip_wreq_thrd, TODO: FIXEDME, less than (DIPW hsize/16) - 1
    //SC_W2BYTEMSK_CMDQ(0, REG_SC_BK36_6F_L, 0x0A, BMASK(5: 0));
    if(u16Width < 256)
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_6F_L, 0x04, BMASK(5: 0));
    }
    else
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_6F_L, 0x0A, BMASK(5: 0));
    }

    //reg_dip_tile_req_num,  TODO: FIXEDME, value should be even and smaller than (DIPW hsize/32)
    if(u16Width < 640)
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7E_L, 0x08, BMASK(6: 0));
    }
    else
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7E_L, 0x10, BMASK(6: 0));
    }

#endif

    if(eSource != SCALER_DIP_SOURCE_TYPE_SC2_OP_CAPTURE && eSource != SCALER_DIP_SOURCE_TYPE_OP_SC1_CAPTURE)
    {
        //reg_4k_h_size
        if((bDIPEnaDI == TRUE) && ((enTB_DIPCurrentField == E_DIP_3DDI_TOP_BOTH_FIELD_STAGGER) || (enTB_DIPCurrentField == E_DIP_3DDI_BOTTOM_BOTH_FIELD_STAGGER)))
        {
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_6D_L, u16MaxWith, BMASK(11: 0));
        }
        else
        {
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_6D_L, ((u16MaxWith + 1) / 2), BMASK(11: 0));
        }
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_6D_L, BIT(15), BIT(15));
        //dip_op_pre_sel (2: sc2 3: sc)
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_67_L, 0x3, BMASK(1: 0));
    }
    else
    {
        //reg_4k_h_size
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_6D_L, 0, BIT(15));
        //dip_op_pre_sel (2: sc2 3: sc)
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_67_L, 0x2, BMASK(1: 0));
    }
    //DIP Clk gating seletion and source select
    if(eSource == SCALER_DIP_SOURCE_TYPE_MAIN)
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_7F_L, BIT(8), BMASK(8: 7));
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_67_L, 0, BMASK(4: 2)); //0: ip_main    1: ip_sub    2: sc2_ip_main
    }
    else if(eSource == SCALER_DIP_SOURCE_TYPE_SUB)
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_7F_L, BIT(7), BMASK(8: 7));
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_67_L, BIT(2), BMASK(4: 2)); //0: ip_main    1: ip_sub    2: sc2_ip_main
    }
    else
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_7F_L, 0x0, BMASK(8: 7));
    }

#if 0 // mark for I2
    //Rotation
    if( (bDIPRotation == TRUE) && (DIPRotation == eWindow) )
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_6E_L, (u16Height / 0x10), BMASK(7: 0));
#endif

}

void HAL_XC_DIP_SetWinProperty1(void *pstCmdQInfo, MS_U32 u64OffSet, SCALER_DIP_WIN eWindow)
{
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK36_60_L, u64OffSet);

}

MS_U8 HAL_XC_DIP_GetBufCnt(void *pstCmdQInfo, SCALER_DIP_WIN eWindow)
{
    MS_U8 u8BufCnt = 0;
    u8BufCnt = DipWBufCnt;
    return (MS_U8) u8BufCnt;
}

void HAL_XC_DIP_SetBase0(void *pstCmdQInfo, MS_U32 u64BufStart, MS_U32 u64BufEnd, SCALER_DIP_WIN eWindow)
{
    MS_U16 u16BusSize = 0;
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    u16BusSize = HAL_XC_DIP_GetBusSize(pstCmdQInfo, eWindow);

    //low bound
    SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK36_10_L, u64BufStart / u16BusSize);     // input address0
    //high bound
    u64BufEnd /= u16BusSize;
    u64BufEnd |= (1 << DWIN_W_LIMITE_OFT);
    SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK36_30_L, u64BufEnd);     // input address0

}

void HAL_XC_DIP_SetBase1(void *pstCmdQInfo, MS_U32 u64BufStart, MS_U32 u64BufEnd, SCALER_DIP_WIN eWindow)
{
    MS_U16 u16BusSize = 0;
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    u16BusSize = HAL_XC_DIP_GetBusSize(pstCmdQInfo, eWindow);

    //low bound
    SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK36_20_L, u64BufStart / u16BusSize);     // input address0
    //high bound
    u64BufEnd /= u16BusSize;
    u64BufEnd |= (1 << DWIN_W_LIMITE_OFT);
    SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK36_40_L, u64BufEnd);     // input address0

}

void HAL_XC_DIP_SetMiuSel(void *pstCmdQInfo, MS_U8 u8MIUSel, SCALER_DIP_WIN eWindow)
{
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

#if (DIP_PLATFORM == DIP_K6)
    // Scaler control MIU by itself
    // So MIU1 IP-select is set to 1. At this moment, MIU0 Miu select is not working.
    MDrv_Write2ByteMask_CmdQ(pstCmdQ, (REG_MIU0_BASE + (0x7D * 2)), 0x00, BIT(9)); // MIU select (Group3 BIT1)
    MDrv_Write2ByteMask_CmdQ(pstCmdQ, (REG_MIU1_BASE + (0x7D * 2)), BIT(9), BIT(9)); // IP select
#endif
    if( ((CHIP_MIU_ID)u8MIUSel) == E_CHIP_MIU_0 )
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_02_L, 0, BIT(13));
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_07_L, 0, BIT(15));
    }
    else if( ((CHIP_MIU_ID)u8MIUSel) == E_CHIP_MIU_1 )
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_02_L, BIT(13), BIT(13));
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_07_L, 0, BIT(15));
    }
    else if( ((CHIP_MIU_ID)u8MIUSel) == E_CHIP_MIU_2 )
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_02_L, 0, BIT(13));
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_07_L, BIT(15), BIT(15));
    }
    else
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_02_L, BIT(13), BIT(13));
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_07_L, BIT(15), BIT(15));
    }

}

void HAL_XC_DIP_SetDIPRMiuSel(void *pstCmdQInfo, MS_U8 u8YBufMIUSel, MS_U8 u8CBufMIUSel, SCALER_DIP_WIN eWindow)
{
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    // Y/C miu separate setting
    if( u8YBufMIUSel == u8CBufMIUSel)
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_76_L, 0, BIT(15));
    else
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_76_L, BIT(15), BIT(15));

    //DIPR Y buffer
    if( ((CHIP_MIU_ID)u8YBufMIUSel) == E_CHIP_MIU_0 )
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7D_L, 0, BIT(15));
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7C_L, 0, BIT(14));
    }
    else if( ((CHIP_MIU_ID)u8YBufMIUSel) == E_CHIP_MIU_1 )
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7D_L, BIT(15), BIT(15));
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7C_L, 0, BIT(14));
    }
    else if( ((CHIP_MIU_ID)u8YBufMIUSel) == E_CHIP_MIU_2 )
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7D_L, 0, BIT(15));
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7C_L, BIT(14), BIT(14));
    }

    //DIPR C buffer
    if( ((CHIP_MIU_ID)u8CBufMIUSel) == E_CHIP_MIU_0 )
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_76_L, 0, BMASK(14: 13));
    }
    else if( ((CHIP_MIU_ID)u8CBufMIUSel) == E_CHIP_MIU_1 )
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_76_L, BIT(13), BMASK(14: 13));
    }
    else if( ((CHIP_MIU_ID)u8CBufMIUSel) == E_CHIP_MIU_2 )
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_76_L, BIT(14), BMASK(14: 13));
    }

}

void HAL_XC_DIP_SetY2R(void *pstCmdQInfo, MS_BOOL bEnable, SCALER_DIP_WIN eWindow)
{
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    //enable y2r
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7E_L, (bEnable ? BIT(11) : 0), BIT(11));

}

void HAL_XC_DIP_SetAlphaValue(void *pstCmdQInfo, MS_U8 u8AlphaVal, SCALER_DIP_WIN eWindow)
{
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_02_L, u8AlphaVal, BMASK(7: 0));

}

void HAL_XC_DIP_SetUVSwap(void *pstCmdQInfo, MS_BOOL bEnable, SCALER_DIP_WIN eWindow)
{
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    bDipUVSwap = bEnable;
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_02_L, (bDipHMirror ? (!bDipUVSwap ? BIT(14) : 0) : (bDipUVSwap ? BIT(14) : 0)), BIT(14));

}

void HAL_XC_DIP_SetYCSwap(void *pstCmdQInfo, MS_BOOL bEnable, SCALER_DIP_WIN eWindow)
{
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    bDipYCSwap = bEnable;
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_02_L, (bDipHMirror ? (!bDipYCSwap ? BIT(15) : 0) : (bDipYCSwap ? BIT(15) : 0)), BIT(15));

}

void HAL_XC_DIP_SetRGBSwap(void *pstCmdQInfo, MS_BOOL bEnable, SCALER_DIP_WIN eWindow)
{
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7E_L, (bEnable ? BIT(13) : 0), BIT(13));

}

void HAL_XC_DIP_SetOutputCapture(void *pstCmdQInfo, MS_BOOL bEnable, EN_XC_DIP_OP_CAPTURE eOpCapture, SCALER_DIP_WIN eWindow)
{
    MS_BOOL bCaptureEna;
    MS_U16 u16CaptureSrc;
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    bCaptureEna = bEnable;
    switch(eOpCapture)
    {
        case E_XC_DIP_VOP2:  // compatible to previous IC, overlap is before osd blending
            u16CaptureSrc = 0;
            break;
        case E_XC_DIP_OP2:
            u16CaptureSrc = 1;
            break;
        case E_XC_DIP_VIP:
            u16CaptureSrc = 2;
            break;
        case E_XC_DIP_BRI:
            u16CaptureSrc = 3;
            break;
        default:
            u16CaptureSrc = 0;
            bCaptureEna = FALSE;
            break;
    }

    if(bCaptureEna)
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK0F_57_L, BIT(11), BIT(11));  // Enable
        if(eOpCapture == E_XC_DIP_VOP2)
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK2F_35_L, 0 << 4, BMASK(6: 4));
        else if(eOpCapture == E_XC_DIP_OP2)
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK2F_35_L, 5 << 4, BMASK(6: 4));
        else
            printf("[DIP] OP capture source not support\n");
    }

}

void HAL_XC_DIP_SetMirror(void *pstCmdQInfo, MS_BOOL bHMirror, MS_BOOL bVMirror, SCALER_DIP_WIN eWindow)
{
    EN_DRV_XC_DWIN_DATA_FMT fmt = XC_DWIN_DATA_FMT_MAX;
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    // pixel format will swap if set Hmirror,so here do swap pixel format
    fmt = HAL_XC_DIP_GetDataFmt(pstCmdQInfo, eWindow);

    if (  (XC_DWIN_DATA_FMT_YUV422 == fmt)
            || (XC_DWIN_DATA_FMT_YUV420 == fmt))
    {
        bDipHMirror = bHMirror;
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_02_L, (bDipHMirror ? (!bDipUVSwap ? BIT(14) : 0) : (bDipUVSwap ? BIT(14) : 0)), BIT(14));
    }
    else if (XC_DWIN_DATA_FMT_YC422 == fmt)
    {
        bDipHMirror = bHMirror;
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_02_L, (bDipHMirror ? (!bDipYCSwap ? BIT(15) : 0) : (bDipYCSwap ? BIT(15) : 0)), BIT(15));
    }

    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_0B_L, (bHMirror ? BIT(9) : 0), BIT(9));
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_0B_L, (bVMirror ? BIT(10) : 0), BIT(10));

}
void HAL_XC_DIP_SetDIPRProperty(void *pstCmdQInfo, ST_XC_DIPR_PROPERTY *pstDIPRProperty, SCALER_DIP_WIN eWindow)
{
    MS_BOOL bYUV = TRUE;
    MS_U16 u16Fmt;
    MS_U16 u16BusSize = 0;
    MS_U16 u16MiuCnt = 0;
    MS_U16 u16temp = 0;
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    DipRFmt = (EN_DRV_XC_DWIN_DATA_FMT)pstDIPRProperty->enDataFmt;
    switch ((EN_DRV_XC_DWIN_DATA_FMT)pstDIPRProperty->enDataFmt)
    {
        case XC_DWIN_DATA_FMT_YUV422 :
        case XC_DWIN_DATA_FMT_YC422 :
            u16Fmt = 0;
            break;
        case XC_DWIN_DATA_FMT_RGB565 :
            u16Fmt = 1;
            break;
        case XC_DWIN_DATA_FMT_ARGB8888 :
            u16Fmt = 2;
            break;
        case XC_DWIN_DATA_FMT_YUV420 :
        case XC_DWIN_DATA_FMT_YUV420_H265 :
        case XC_DWIN_DATA_FMT_YUV420_H265_10BITS :
#if (DIP_PLATFORM == DIP_I2)
        case XC_DWIN_DATA_FMT_YUV420_SEMI_PLANER :
#endif
            u16Fmt = 3;
            u16BusSize = HAL_XC_DIP_GetBusSize(pstCmdQInfo, eWindow);
            u16MiuCnt = (pstDIPRProperty->u16Width) / u16BusSize;

            for(u16temp = DIPR_TILE_FIFO_MAX; u16temp > 0; u16temp--)
            {
                if((u16MiuCnt % u16temp) == 0)
                {
                    // reg_dipr_tile_req_num, value = (the factor of (DIPR hsize/32)) - 1
                    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7F_L, (u16temp - 1), BMASK(6: 0));
                    break;
                }
            }

            break;
        default :
            u16Fmt = 0x0;
            break;
    }

    //select 420 to 422
    if( (pstDIPRProperty->enDataFmt == DIP_DATA_FMT_YUV420) || (pstDIPRProperty->enDataFmt == DIP_DATA_FMT_YUV420_H265) || \
            (pstDIPRProperty->enDataFmt == DIP_DATA_FMT_YUV420_H265_10BITS) || (pstDIPRProperty->enDataFmt == DIP_DATA_FMT_YUV420_SEMI_PLANER))
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7C_L, BIT(3), BIT(3));
    else
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7C_L, 0, BIT(3));
    if( ((pstDIPRProperty->enDataFmt) == DIP_DATA_FMT_RGB565) || ((pstDIPRProperty->enDataFmt) == DIP_DATA_FMT_ARGB8888))
        bYUV = FALSE;

#if (DIP_PLATFORM == DIP_K6)
    if(bDIPEnaDI)  // don't convert to YUV444 if 3DDI enable, source must be YUV422
    {
        bYUV = FALSE;
    }
#endif
#if (DIP_PLATFORM == DIP_I2)
    if(bDIPRotation == TRUE)
    {
        bYUV = FALSE;
    }
#endif

    //select  422 to 444
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_6C_L, (bYUV ? BIT(9) : 0), BIT(9));

#if 0 // mark for I2
    if(bDIPRotation == FALSE)
#endif
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_6F_L, 0x3, BIT(1) | BIT(0));

    //Y Buffer
    SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK36_78_L, ((pstDIPRProperty->u32YBufAddr) / HAL_XC_DIP_GetBusSize(pstCmdQInfo, eWindow)));
    //C Buffer
    SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK36_7A_L, ((pstDIPRProperty->u32CBufAddr) / HAL_XC_DIP_GetBusSize(pstCmdQInfo, eWindow)));

    //width
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_27_L, (pstDIPRProperty->u16Width));
    g_u16DIPRWidth = pstDIPRProperty->u16Width;
    //height
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_37_L, (pstDIPRProperty->u16Height));
    //Line offset
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_74_L, (pstDIPRProperty->u16Pitch));
    //Data Fmt
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7D_L, (u16Fmt) << 6, BMASK(7: 6));
#if (DIP_PLATFORM == DIP_I2)
    if(DipRFmt == XC_DWIN_DATA_FMT_YUV420_SEMI_PLANER)
    {
        //Data Fmt: YUV420 SEMI_PLANER
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_07_L, BIT(14), BIT(14));
    }
    else
    {
        //Data Fmt
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_07_L, 0, BIT(14));
    }
#endif

    if(pstDIPRProperty->enDataFmt == DIP_DATA_FMT_YUV420_H265_10BITS)
    {
        //Y Buffer for EVD 10 bits
        SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK36_12_L, ((pstDIPRProperty->u32YBufAddr10Bits) / HAL_XC_DIP_GetBusSize(pstCmdQInfo, eWindow)));
        //C Buffer for EVD 10 bits
        SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK36_14_L, ((pstDIPRProperty->u32CBufAddr10Bits) / HAL_XC_DIP_GetBusSize(pstCmdQInfo, eWindow)));
        //Line offset for EVD 10 bits
        SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_1D_L, (pstDIPRProperty->u16Pitch));
        //Enable DIPR H265 10 bits
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_17_L, BIT(15), BIT(15));
    }
    else
    {
        //Disable DIPR H265 10 bits
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_17_L, 0x0, BIT(15));
    }
    if( (pstDIPRProperty->enDataFmt == DIP_DATA_FMT_YUV420_H265) || (pstDIPRProperty->enDataFmt == DIP_DATA_FMT_YUV420_H265_10BITS))
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_0B_L, BIT(13), BIT(13));
    }
    else
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_0B_L, 0, BIT(13));
    }
    if(pstDIPRProperty->enDataFmt == DIP_DATA_FMT_YUV420)
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_0C_L, BIT(6), BIT(6));
    }
    else
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_0C_L, 0, BIT(6));
    }

}

MS_U16 MFDec_MUF_Table[MFEDC_TABLE_INDEX_COUNT][4] =
{
    {0x0000, 0x0000, 0x0000, 0x8080},
    {0x0000, 0x0000, 0x0101, 0x8081},
    {0x0000, 0x0000, 0x0000, 0x8082},
    {0x0000, 0x0000, 0x0000, 0x8083},
    {0x0002, 0x0002, 0x0101, 0x8180},
    {0x0002, 0x0002, 0x0000, 0x8181},
    {0x0000, 0x0002, 0x0200, 0x8182},
    {0x0000, 0x0001, 0x0100, 0x8183},
    {0x0006, 0x0007, 0x0302, 0x8280},
    {0x0006, 0x0006, 0x0202, 0x8281},
    {0x0006, 0x0006, 0x0303, 0x8282},
    {0x0004, 0x0005, 0x0302, 0x8283},
    {0x0000, 0x0000, 0x0000, 0x8380},
    {0x000E, 0x000E, 0x0303, 0x8381},
    {0x000E, 0x000E, 0x0404, 0x8382},
    {0x000C, 0x000D, 0x0504, 0x8383},
    {0x0000, 0x0000, 0x0000, 0x8480},
    {0x001E, 0x001E, 0x0404, 0x8481},
    {0x001E, 0x001E, 0x0505, 0x8482},
    {0x001C, 0x001D, 0x0706, 0x8483},
    {0x0000, 0x0000, 0x0000, 0x8580},
    {0x003E, 0x003E, 0x0505, 0x8581},
    {0x003E, 0x003E, 0x0606, 0x8582},
    {0x003C, 0x003D, 0x0908, 0x8583},
    {0x0000, 0x0000, 0x0000, 0x8680},
    {0x007E, 0x007F, 0x0706, 0x8681},
    {0x007E, 0x007E, 0x0707, 0x8682},
    {0x007C, 0x007D, 0x0B0A, 0x8683},
    {0x0000, 0x0000, 0x0000, 0x8780},
    {0x0000, 0x0000, 0x0000, 0x8781},
    {0x00FE, 0x00FE, 0x0808, 0x8782},
    {0x00FC, 0x00FD, 0x0D0C, 0x8783},
    {0x0000, 0x0000, 0x0000, 0x8880},
    {0x0000, 0x0000, 0x0000, 0x8881},
    {0x01FE, 0x01FE, 0x0909, 0x8882},
    {0x01FC, 0x01FD, 0x0F0E, 0x8883},
    {0x0000, 0x0000, 0x0000, 0x8980},
    {0x0000, 0x0000, 0x0000, 0x8981},
    {0x0000, 0x0000, 0x0D00, 0x8982},
    {0x0000, 0x0000, 0x0000, 0x8983},
    {0x0000, 0x0000, 0x0000, 0x8A80},
    {0x0000, 0x0000, 0x0000, 0x8A81},
    {0x07FC, 0x07FD, 0x0B0A, 0x8A82},
    {0x0000, 0x0000, 0x0000, 0x8A83},
    {0x0000, 0x0000, 0x0000, 0x8B80},
    {0x0000, 0x0000, 0x0000, 0x8B81},
    {0x0FFC, 0x0FFF, 0x0F0C, 0x8B82},
    {0x0FF0, 0x0FFF, 0x1F10, 0x8B83},
};

void HAL_XC_DIP_LOAD_MFDEC_TABLE(void *pstCmdQInfo, MS_U8 mfdec_id, SCALER_DIP_WIN eWindow)
{
    MS_U8 u8index = 0;
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_MFDEC_CW_LEN2(mfdec_id), 0x0007, MFDEC_CW_LEN2_MASK);
    MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_MFDEC_CW_LEN3(mfdec_id), 0x007F, MFDEC_CW_LEN3_MASK);
    MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_MFDEC_CW_LEN4(mfdec_id), 0x0DFE, MFDEC_CW_LEN4_MASK);
    MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_MFDEC_CW_LEN5(mfdec_id), 0x09FE, MFDEC_CW_LEN5_MASK);

    for (u8index = 0; u8index < MFEDC_TABLE_INDEX_COUNT; u8index++)
    {
        MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_MFDEC_CW_BASE(mfdec_id), MFDec_MUF_Table[u8index][0], BMASK(15: 0));
        MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_MFDEC_CW_MAX(mfdec_id), MFDec_MUF_Table[u8index][1], BMASK(15: 0));
        MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_MFDEC_SYMB_BASE(mfdec_id), MFDec_MUF_Table[u8index][2], BMASK(15: 0));
        MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_MFDEC_HUF_TAB(mfdec_id), MFDec_MUF_Table[u8index][3], BMASK(15: 0));
    }

}

void HAL_XC_DIP_SetDIPRProperty_MFDEC(void *pstCmdQInfo, ST_DIP_MFDEC_INFO stDIPR_MFDecInfo, SCALER_DIP_WIN eWindow)
{
    MS_U32 phybitlen_base = 0;
    MS_U32 phyLuma_FB_Base = 0, phyChroma_FB_Base = 0;
    MS_U16 u16str_x = 0, u16hsize = 0;
    MS_U16 u16regtmp = 0;
    MS_U8 u8bitlen_miu_sel = 0, u8luma_miu_sel = 0, u8chroma_miu_sel = 0;
    MS_U8 u8mfdec_id = 0;
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    if(stDIPR_MFDecInfo.bMFDec_Enable == TRUE )
    {
        if(debug_level >= 2)
        {
            printf("\033[32m [%s][%d] ========== stMFDec_VerCtl.u32version = %d  ========== \033[0m\n", __FUNCTION__, __LINE__, stDIPR_MFDecInfo.stMFDec_VerCtl.u32version);
            printf("\033[32m [%s][%d] ========== stMFDec_VerCtl.u32size = %d  ========== \033[0m\n", __FUNCTION__, __LINE__, stDIPR_MFDecInfo.stMFDec_VerCtl.u32size);
            printf("\033[32m [%s][%d] ========== bMFDec_Enable = %d  ========== \033[0m\n", __FUNCTION__, __LINE__, stDIPR_MFDecInfo.bMFDec_Enable);
            printf("\033[32m [%s][%d] ========== u8MFDec_Select = %d  ========== \033[0m\n", __FUNCTION__, __LINE__, stDIPR_MFDecInfo.u8MFDec_Select);
            printf("\033[32m [%s][%d] ========== enMFDec_tile_mode = %d  ========== \033[0m\n", __FUNCTION__, __LINE__, stDIPR_MFDecInfo.enMFDec_tile_mode);
            printf("\033[32m [%s][%d] ========== bUncompress_mode = %d  ========== \033[0m\n", __FUNCTION__, __LINE__, stDIPR_MFDecInfo.bUncompress_mode);
            printf("\033[32m [%s][%d] ========== bBypass_codec_mode = %d  ========== \033[0m\n", __FUNCTION__, __LINE__, stDIPR_MFDecInfo.bBypass_codec_mode);
            printf("\033[32m [%s][%d] ========== en_MFDecVP9_mode = %d  ========== \033[0m\n", __FUNCTION__, __LINE__, stDIPR_MFDecInfo.en_MFDecVP9_mode);
            printf("\033[32m [%s][%d] ========== phyLuma_FB_Base = 0x%x  ========== \033[0m\n", __FUNCTION__, __LINE__, stDIPR_MFDecInfo.phyLuma_FB_Base);
            printf("\033[32m [%s][%d] ========== phyChroma_FB_Base = 0x%x  ========== \033[0m\n", __FUNCTION__, __LINE__, stDIPR_MFDecInfo.phyChroma_FB_Base);
            printf("\033[32m [%s][%d] ========== u16FB_Pitch = %d  ========== \033[0m\n", __FUNCTION__, __LINE__, stDIPR_MFDecInfo.u16FB_Pitch);
            printf("\033[32m [%s][%d] ========== u16StartX = %d  ========== \033[0m\n", __FUNCTION__, __LINE__, stDIPR_MFDecInfo.u16StartX);
            printf("\033[32m [%s][%d] ========== u16StartY = %d  ========== \033[0m\n", __FUNCTION__, __LINE__, stDIPR_MFDecInfo.u16StartY);
            printf("\033[32m [%s][%d] ========== u16HSize = %d  ========== \033[0m\n", __FUNCTION__, __LINE__, stDIPR_MFDecInfo.u16HSize);
            printf("\033[32m [%s][%d] ========== u16VSize = %d  ========== \033[0m\n", __FUNCTION__, __LINE__, stDIPR_MFDecInfo.u16VSize);
            printf("\033[32m [%s][%d] ========== phyBitlen_Base = 0x%x  ========== \033[0m\n", __FUNCTION__, __LINE__, stDIPR_MFDecInfo.phyBitlen_Base);
            printf("\033[32m [%s][%d] ========== u16Bitlen_Pitch = %d  ========== \033[0m\n", __FUNCTION__, __LINE__, stDIPR_MFDecInfo.u16Bitlen_Pitch);
        }

        if(stDIPR_MFDecInfo.u8MFDec_Select == 0 )
            u8mfdec_id = MFDEC_ID0;
        else if(stDIPR_MFDecInfo.u8MFDec_Select == 1 )
            u8mfdec_id = MFDEC_ID1;

        // Enable MFDEC and Set attribute
        u16regtmp = (MFDEC_RIU_MODE | MFDEC_RIU1_MODE) | MFDEC_ENABLE;
        if(stDIPR_MFDecInfo.bHMirror == TRUE)
            u16regtmp |= MFDEC_HMIRROR;

        if(stDIPR_MFDecInfo.bVMirror == TRUE)
            u16regtmp |= MFDEC_VMIRROR;

        if(stDIPR_MFDecInfo.en_MFDecVP9_mode == E_XC_DIP_VP9_MODE)
            u16regtmp |= MFDEC_VP9_MODE;

        if(stDIPR_MFDecInfo.bUncompress_mode == TRUE)
            u16regtmp |= MFDEC_UNCOMP_MODE;

        if(stDIPR_MFDecInfo.enMFDec_tile_mode == E_DIP_MFEDC_TILE_32_32)
            u16regtmp |= MFDEC_HVD_MODE;  // only use for bypass mode

        if(stDIPR_MFDecInfo.bBypass_codec_mode == TRUE)
            u16regtmp |= MFDEC_BYPASS_MODE;

        if(stDIPR_MFDecInfo.bBypass_codec_mode == FALSE)
        {
#ifdef DIP_UT_MFDEC_SIM_MODE  // test simulation mode
            u16regtmp |= MFDEC_SIM_MODE;
#endif
        }

        MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_MFDEC_ENA(u8mfdec_id), u16regtmp, REG_MFDEC_ENA_MASK);

        // Set MFDEC miu
        u16regtmp = 0;
        _phy_to_miu_offset(u8luma_miu_sel, stDIPR_MFDecInfo.phyLuma_FB_Base, stDIPR_MFDecInfo.phyLuma_FB_Base);
        u16regtmp |= (u8luma_miu_sel << MFDEC_LUMA_FB_MIU_SHIFT);
        _phy_to_miu_offset(u8chroma_miu_sel, stDIPR_MFDecInfo.phyChroma_FB_Base, stDIPR_MFDecInfo.phyChroma_FB_Base);
        u16regtmp |= (u8chroma_miu_sel << MFDEC_CHROMA_FB_SHIFT);
        _phy_to_miu_offset(u8bitlen_miu_sel, stDIPR_MFDecInfo.phyBitlen_Base, stDIPR_MFDecInfo.phyBitlen_Base);
        u16regtmp |= (u8bitlen_miu_sel << MFDEC_BITLEN_FB_MIU_SHIFT);
        MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_MFDEC_FB_MIU(u8mfdec_id), u16regtmp, (MFDEC_LUMA_FB_MIU | MFDEC_CHROMA_FB_MIU | MFDEC_BITLEN_FB_MIU));

        // Set MFDEC X/Y start
        u16str_x = (stDIPR_MFDecInfo.u16StartX) / MFDEC_X_START_ALIGN;
        MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_MFDEC_X_START(u8mfdec_id), u16str_x, MFDEC_X_START);
        MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_MFDEC_Y_START(u8mfdec_id), stDIPR_MFDecInfo.u16StartY, MFDEC_Y_START);

        // Set MFDEC X/Y Size
        u16hsize = (stDIPR_MFDecInfo.u16HSize) / MFDEC_HSIZE_ALIGN;
        MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_MFDEC_H_SIZE(u8mfdec_id), u16hsize, MFDEC_H_SIZE);
        MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_MFDEC_V_SIZE(u8mfdec_id), stDIPR_MFDecInfo.u16VSize, MFDEC_V_SIZE);

        // Set MFDEC FB pitch, reg_fb_pitch = ((src_pic_width + 31) >> 5 ) <<2
        if(stDIPR_MFDecInfo.u16FB_Pitch % MFDEC_FB_PITCH_ALIGN != 0)
            printf("ERROR[%s][%d] ==========FB Pitch not alignment %d , 8P alignment ========== \n", __FUNCTION__, __LINE__, stDIPR_MFDecInfo.u16FB_Pitch);
        u16regtmp = stDIPR_MFDecInfo.u16FB_Pitch / MFDEC_FB_PITCH_ALIGN;
        MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_MFDEC_FB_PITCH(u8mfdec_id), u16regtmp, MFDEC_FB_PITCH);

        // Set MFDEC bitlen pitch, (src_pic_width + 63) >> 6  --> align 64pixel
        MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_MFDEC_BITLEN_FB_PITCH(u8mfdec_id), stDIPR_MFDecInfo.u16Bitlen_Pitch, MFDEC_BITLEN_FB_PITCH_MASK);

        //if(stDIPR_MFDecInfo.bBypass_codec_mode == TRUE)
        {
            // Set MFDEC Luma FB address
            phyLuma_FB_Base = stDIPR_MFDecInfo.phyLuma_FB_Base / MFDEC_ADDR_ALIGN;
            MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_MFDEC_LUMA_FB_BASE_L(u8mfdec_id), phyLuma_FB_Base, MFDEC_LUMA_FB_BASE_L);
            MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_MFDEC_LUMA_FB_BASE_H(u8mfdec_id), phyLuma_FB_Base >> 16, MFDEC_LUMA_FB_BASE_H);

            // Set MFDEC Chroma FB address
            phyChroma_FB_Base = stDIPR_MFDecInfo.phyChroma_FB_Base / MFDEC_ADDR_ALIGN;
            MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_MFDEC_CHROMA_FB_BASE_L(u8mfdec_id), phyChroma_FB_Base, MFDEC_CHROMA_FB_BASE_L);
            MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_MFDEC_CHROMA_FB_BASE_H(u8mfdec_id), phyChroma_FB_Base >> 16, MFDEC_CHROMA_FB_BASE_H);
        }

        // Set MFDEC bitlen FB address
        phybitlen_base = stDIPR_MFDecInfo.phyBitlen_Base / MFDEC_ADDR_ALIGN;
        MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_MFDEC_BITLEN_FB_BASE_L(u8mfdec_id), phybitlen_base, MFDEC_BITLEN_FB_BASE_L);
        MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_MFDEC_BITLEN_FB_BASE_H(u8mfdec_id), phybitlen_base >> 16, MFDEC_BITLEN_FB_BASE_H);

        //load MFDEC table for DIP
        //HAL_XC_DIP_LOAD_MFDEC_TABLE(pstCmdQInfo, u8mfdec_id, eWindow);

        //Enable MFDEC line buffer
        if(u8mfdec_id == MFDEC_ID0)
        {
            MDrv_WriteRegBit_CmdQ(pstCmdQ, REG_MFDEC_LINEBUFFER, ENABLE_MFDEC_LB, MFDEC_MAIN_LB);
        }
        else if(u8mfdec_id == MFDEC_ID1)
        {
            MDrv_WriteRegBit_CmdQ(pstCmdQ, REG_MFDEC_LINEBUFFER, ENABLE_MFDEC_LB, MFDEC_SUB_LB);
        }

        if(eWindow == DIP_WINDOW)
        {
            // swap MFDEC
            if(u8mfdec_id == MFDEC_ID0)
                SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_5D_L, 0, BIT(0));
            else if(u8mfdec_id == MFDEC_ID1)
                SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_5D_L, BIT(0), BIT(0));

            // Enable DIP with MFDEC
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_75_L, BIT(6), BIT(6));
        }

        bDIPMfdecEnable = TRUE;
    }
    else
    {
        // Disable MFDEC
        MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_MFDEC_ENA(u8mfdec_id), 0, (MFDEC_RIU1_MODE | MFDEC_RIU_MODE | MFDEC_ENABLE));

        // Disable DIP with MFDEC
        if(eWindow == DIP_WINDOW)
        {
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_75_L, 0, BIT(6));
        }

        bDIPMfdecEnable = FALSE;
    }

}

void HAL_XC_DIP_DI_init(void *pstCmdQInfo, MS_U32 phyWriteAddr, MS_U32 phyReadAddr, SCALER_DIP_WIN eWindow)
{
#if 0
    MS_U16 u16WritewidthSave = 0;
    MS_U16 u16ReadwidthSave = 0;
    MS_U16 u16WriteHeigtSave = 0;
    MS_U16 u16ReadHeigtSave = 0;
    MS_U16 u16WritePitchSave = 0;
    MS_U16 u16ReadPitchSave = 0;
    MS_U32 u32WriteAddrSave = 0;
    MS_U16 u16MIUSel_W_lsb_Save = 0;
    MS_U32 u32ReadAddrSave = 0;
    MS_U16 u16MIUSel_R_lsb_Save = 0;
    MS_U16 u16WriteFmtSave = 0;
    MS_U16 u16ReadFmtSave = 0;
    MS_U16 u16DIPPathSave = 0;
    MS_U16 u16DIPHSDSizeSave = 0;

    MS_U8 u8MIUSel_R_W = 0;
    MS_U16 u16DIINIT_test_cnt = 0;
    MS_U8 u8DI_queue_count = 0;
    MS_U16 u16BusSize = 0;
#endif
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    DI_DBG_MSG(printf("[%s,%d] ======== 3DDI Initialization start ========\n", __func__, __LINE__);)
    //u16BusSize = HAL_XC_DIP_GetBusSize(pstCmdQInfo, eWindow);

#if 0
    //Save Info
    u16WritewidthSave = SC_R2BYTE(0, REG_SC_BK36_1F_L);
    u16ReadwidthSave = SC_R2BYTE(0, REG_SC_BK36_27_L);
    u16WriteHeigtSave = SC_R2BYTE(0, REG_SC_BK36_2F_L);
    u16ReadHeigtSave = SC_R2BYTE(0, REG_SC_BK36_37_L);
    u16WritePitchSave = SC_R2BYTE(0, REG_SC_BK36_3F_L);
    u16ReadPitchSave = SC_R2BYTE(0, REG_SC_BK36_74_L);
    u32WriteAddrSave = SC_R4BYTE(0, REG_SC_BK36_10_L);
    u16MIUSel_W_lsb_Save = SC_R2BYTE(0, REG_SC_BK36_02_L);

    u32ReadAddrSave = SC_R4BYTE(0, REG_SC_BK36_78_L);
    u16MIUSel_R_lsb_Save = SC_R2BYTE(0, REG_SC_BK36_7D_L);
    u16WriteFmtSave = SC_R2BYTE(0, REG_SC_BK36_01_L);
    u16ReadFmtSave = SC_R2BYTE(0, REG_SC_BK36_7C_L);
    u16DIPHSDSizeSave = SC_R2BYTE(0, REG_SC_BK34_6D_L);
    u16DIPPathSave = SC_R2BYTE(0, REG_SC_BK34_7F_L);
#endif

    //DI init default value
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK43_7D_L, 0x4F98);  //[5:0]:reg_dipr_req_thrd, [7:6]:reg_dipr_format, [12:8]:reg_dipr_hi_pri_num, [14]: reg_dipr_en
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK43_01_L, 0x0023); //set write fmt and write buf count
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_7C_L, BIT(6), BIT(6)); //DI field control, 0: force dipr field 1: toggle by self
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK43_14_L, 0x15AF);  // [0]:reg_cpack_en, [1]:reg_cpack_typ, [2]:interlace to progressive, [10]:reg_dnr_en, [12]:reg_madi_en
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_7E_L, 0x10, BMASK(5: 0));  //request number once for tile format
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK43_02_L, 0x00FF);
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK43_03_L, 0x000F);
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK43_6F_L, 0x4000);
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK43_7F_L, 0x4000);

#if (DIP_PLATFORM == DIP_I2)
    //read request threshold
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7D_L, 0x18, BMASK(5: 0));

    //SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_15_L, BIT(5), BIT(5));  // [3]:3D DI to 2.5D dI enable, [5]:reg_lb_ctrl_flag_force_en, [6]:reg_lb_ctrl_flag_force, [8]:reg_force_no_first_two_frm
#endif

#if 0
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_1F_L, DI_INIT_H_SIZE);
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_27_L, DI_INIT_H_SIZE);
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_2F_L, DI_INIT_V_SIZE);
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_37_L, DI_INIT_V_SIZE);
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_3F_L, DI_INIT_PITCH);
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_74_L, DI_INIT_PITCH);
    _phy_to_miu_offset(u8MIUSel_R_W, phyReadAddr, phyReadAddr);
    HAL_XC_DIP_SetDIPRMiuSel(pstCmdQInfo, u8MIUSel_R_W, u8MIUSel_R_W, eWindow);
    _phy_to_miu_offset(u8MIUSel_R_W, phyWriteAddr, phyWriteAddr);
    HAL_XC_DIP_SetMiuSel(pstCmdQInfo, u8MIUSel_R_W, eWindow);
    SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK36_10_L, (MS_U32)phyWriteAddr / u16BusSize);
    SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK36_78_L, (MS_U32)phyReadAddr / u16BusSize);
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_01_L, BIT(5), BMASK(5: 4));
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7C_L, BIT(6), BMASK(7: 6));
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7D_L, BIT(14), BIT(14));
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_02_L, 0, BIT(10));
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_7F_L, BIT(6), BIT(6));
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK34_6D_L, BIT(15) | (DI_INIT_H_SIZE / 2));

    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK43_1F_L, DI_INIT_H_SIZE);
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK43_27_L, DI_INIT_H_SIZE);  //fetch number of one line for dipr
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK43_2F_L, (DI_INIT_V_SIZE / 2));
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK43_37_L, (DI_INIT_V_SIZE / 2));  //line count number of one frame for dipr
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK43_3F_L, DI_INIT_PITCH);
    //DI pitch and read buf count
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK43_74_L, 0x6000 | DI_INIT_PITCH);

    SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK43_10_L, (MS_U32)phyWriteAddr / u16BusSize);
    SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK43_78_L, (MS_U32)phyReadAddr / u16BusSize);

    for(u8DI_queue_count = 0; u8DI_queue_count < DI_INIT_DI_QUEUE_COUNT; u8DI_queue_count++)
    {
        SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK43_09_L, 0xFF);
        SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK43_39_L, 0xFF);
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7C_L, BIT(15), BIT(15));
        u16DIINIT_test_cnt = 0;
        while(1)
        {
            if(((SC_R2BYTE(0, REG_SC_BK43_0A_L)) != 0) && ((SC_R2BYTE(0, REG_SC_BK43_3A_L)) != 0))
                break;
            u16DIINIT_test_cnt++;
            if(u16DIINIT_test_cnt > DI_INIT_RETRY_CNT)
            {
                printf("[%s,%d]DI INIT STEP%d TIMEOUT\n", __func__, __LINE__, u8DI_queue_count);
                break;
            }
            DIP_DelayTaskUs(DI_INIT_DELAY_TIME);
        }
    }

    //Restore Info
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_1F_L, u16WritewidthSave);
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_27_L, u16ReadwidthSave);
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_2F_L, u16WriteHeigtSave);
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_37_L, u16ReadHeigtSave);
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_3F_L, u16WritePitchSave);
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_74_L, u16ReadPitchSave);
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_02_L, u16MIUSel_W_lsb_Save);
    SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK36_10_L, u32WriteAddrSave);
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_7D_L, u16MIUSel_R_lsb_Save);
    SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK36_78_L, u32ReadAddrSave);
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_01_L, u16WriteFmtSave);
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK36_7C_L, u16ReadFmtSave);
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK34_6D_L, u16DIPHSDSizeSave);
    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK34_7F_L, u16DIPPathSave);
#endif

    SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BK43_6F_L, 0x400A);

}

void HAL_XC_DIP_SetDIPRProperty_DI(void *pstCmdQInfo, ST_XC_DIPR_PROPERTY *pstDIPRProperty, ST_DIP_DIPR_3DDI_SETTING *stDIPR_DIInfo, SCALER_DIP_WIN eWindow)
{
    ST_DIP_DI_SETTING stDIPRDISetting;
    MS_U8 u8Nwaycnt = 0;
    MS_U8 u8DINwayindex = 0;
    MS_U8 u8DIBufIndex = 0;
    MS_U8 u8DIBufTotal = 0;
    MS_U32 phyDIBufAddr = 0;
    MS_U32 u32DIBufSize = 0;

    MS_U16 u16BusSize = 0;
    MS_U16 u16loop_index = 0;
    MS_U16 u16check_index = 0;

    MS_U32 phyWriteAddr = 0;
    MS_U32 phyReadAddr = 0;

    MS_U32 u32DINeedBufSize = 0;
    MS_U8 u8i = 0;
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    u16BusSize = HAL_XC_DIP_GetBusSize(pstCmdQInfo, eWindow);

    memset(&stDIPRDISetting, 0x0, sizeof(ST_DIP_DI_SETTING));
    if(stDIPR_DIInfo->stDipr3DDI_VerCtl.u32version == 1)
    {
        memcpy(&stDIPRDISetting, stDIPR_DIInfo->pDIPR3DDISetting, sizeof(ST_DIP_DI_SETTING));
    }
    u8DINwayindex = stDIPRDISetting.u8NwayIndex;

    if(stDIPRDISetting.bEnaNwayInfoRefresh == TRUE)
    {
        u8Nwaycnt = stDIPRDISetting.u8NwayBufCnt;
        for(u16loop_index = 0; u16loop_index < u8Nwaycnt; u16loop_index++)
        {
            phyDI_DIPBufAddr[u16loop_index] = stDIPRDISetting.stDINwayBufInfo[u16loop_index].phyDI_BufAddr;
            u32DI_DIPBufSize[u16loop_index] = stDIPRDISetting.stDINwayBufInfo[u16loop_index].u32DI_BufSize;
            u8DI_DIPBufCnt[u16loop_index] = stDIPRDISetting.stDINwayBufInfo[u16loop_index].u8DIBufCnt;
            DI_DBG_MSG(printf("[%s]phyDI_DIPBufAddr[%d]=0x%x,u32DI_DIPBufSize[%d]=0x%x,u8DI_DIPBufCnt[%d]=%d \n", __func__, u16loop_index, phyDI_DIPBufAddr[u16loop_index], u16loop_index, u32DI_DIPBufSize[u16loop_index], u16loop_index, u8DI_DIPBufCnt[u16loop_index]);)
        }
        bDIPNwayInfoInit = TRUE;
    }

    if(stDIPRDISetting.bEnableDI == TRUE)
    {
#if (DIP_PLATFORM == DIP_K6)
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_0D_L, BIT(0), BIT(0));
#else
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_1E_L, BIT(0), BIT(0));
#endif
        bDIPEnaDI = TRUE;
        if(stDIPRDISetting.bInitDI == TRUE)
        {
            u32DINeedBufSize = DI_INIT_H_SIZE * DI_INIT_V_SIZE * 2 * 2;
            DI_DBG_MSG(printf("[%s,%d]phyDIInitAddr=0x%x,u32DIInitSize=0x%x\n", __func__, __LINE__, (MS_U32)stDIPRDISetting.stInitInfo.phyDIInitAddr, stDIPRDISetting.stInitInfo.u32DIInitSize);)
            if(stDIPRDISetting.stInitInfo.u32DIInitSize < u32DINeedBufSize)
            {
                printf("[ERROR]DI INIT Buffer Size is Not Enough, 0x%x < 0x%x \n", stDIPRDISetting.stInitInfo.u32DIInitSize, u32DINeedBufSize);
                return;
            }
            if(bDIPEnaDIInit == TRUE)
            {
                return;
            }
            phyWriteAddr = stDIPRDISetting.stInitInfo.phyDIInitAddr;
            phyReadAddr = stDIPRDISetting.stInitInfo.phyDIInitAddr + (stDIPRDISetting.stInitInfo.u32DIInitSize / 2) ;
            HAL_XC_DIP_DI_init(pstCmdQInfo, phyWriteAddr, phyReadAddr, eWindow);
            bDIPEnaDIInit = TRUE;
            return;
        }
        else
        {
            if(bDIPEnaDIInit == FALSE)
            {
                printf("[ERROR]NEED DI INIT FIRST\n");
                return;
            }
        }
    }
    else
    {
#if (DIP_PLATFORM == DIP_K6)
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_0D_L, 0, BIT(0));
#else
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_1E_L, 0, BIT(0));
#endif
        bDIPEnaDI = FALSE;
        return;
    }

    if(bDIPNwayInfoInit == FALSE)
    {
        printf("[ERROR]DIP DI Need Nway Info Init First\n");
        return;
    }

    // reg_dipw_force_frm_index, DDIW force frame index enable
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_3B_L, BIT(14), BIT(14));

    u8DIBufTotal = u8DI_DIPBufCnt[u8DINwayindex];
    phyDIBufAddr = phyDI_DIPBufAddr[u8DINwayindex];
    phyDIBufAddr = phyDI_DIPBufAddr[u8DINwayindex];
    u32DIBufSize = u32DI_DIPBufSize[u8DINwayindex];

    enTB_DIPCurrentField = stDIPRDISetting.enDI_Field;

#if (CONFIG_NEW_3DDI_ALGO == 1)  // New DI algorithm

    if((u8DIBuf_WrTopIndex[u8DINwayindex] != 0) && (u8DIBuf_WrTopIndex[u8DINwayindex] != 2))
        u8DIBuf_WrTopIndex[u8DINwayindex] = 0;
    if((u8DIBuf_WrBotIndex[u8DINwayindex] != 1) && (u8DIBuf_WrBotIndex[u8DINwayindex] != 3))
        u8DIBuf_WrBotIndex[u8DINwayindex] = 1;

    switch(stDIPRDISetting.enDI_Field)
    {
        case E_DIP_3DDI_TOP_SINGLE_FIELD:
        case E_DIP_3DDI_TOP_BOTH_FIELD_STAGGER:
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7C_L, 0, BIT(7));  //Forced dipr field, 0: top field  1: bottom field
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_7C_L, 0, BIT(7));  //Forced dipr field, 0: top field  1: bottom field
            u8DIBufIndex = u8DIBuf_WrTopIndex[u8DINwayindex];
            enTB_DIPField[u8DIBufIndex][u8DINwayindex] = E_DIP_TOP_FIELD ;
            u8DIBuf_WrTopIndex[u8DINwayindex] = (u8DIBufIndex == 0) ? 2 : 0;
            break;
        case E_DIP_3DDI_BOTTOM_SINGLE_FIELD:
        case E_DIP_3DDI_BOTTOM_BOTH_FIELD_STAGGER:
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7C_L, BIT(7), BIT(7));  //Forced dipr field, 0: top field  1: bottom field
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_7C_L, BIT(7), BIT(7));  //Forced dipr field, 0: top field  1: bottom field
            u8DIBufIndex = u8DIBuf_WrBotIndex[u8DINwayindex];
            enTB_DIPField[u8DIBufIndex][u8DINwayindex] = E_DIP_BOTTOM_FIELD ;
            u8DIBuf_WrBotIndex[u8DINwayindex] = (u8DIBufIndex == 1) ? 3 : 1;
            break;
        default:
            printf("[ERROR][%s,%d],Chip Not Support(%d)\n", __func__, __LINE__, stDIPRDISetting.enDI_Field);
            break;
    }

    //DI write index
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_3B_L, u8DIBufIndex << 1, BMASK(3: 1));

    DI_DBG_MSG(printf("[%d]phyDIBufAddr=0x%x,u32DIBufSize=0x%x\n", u8DIBufIndex, phyDIBufAddr, u32DIBufSize);)

    if(enTB_DIPField[u8DIBufIndex][u8DINwayindex]  == E_DIP_BOTTOM_FIELD)
    {
        //find DI read bottom field index
        u16check_index = u8DIBuf_WrBotIndex[u8DINwayindex];
        // ddir force bottom frame index
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_3B_L, u16check_index << 7, BMASK(9: 7));
        u8DIBuf_RdBotIndex[u8DINwayindex] = u16check_index;
    }
    else
    {
        //find DI read bottom field index
        u16check_index = (u8DIBuf_WrBotIndex[u8DINwayindex] == 1) ? 3 : 1;
        // ddir force bottom frame index
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_3B_L, u16check_index << 7, BMASK(9: 7));
        u8DIBuf_RdBotIndex[u8DINwayindex] = u16check_index;
    }

    if(enTB_DIPField[u8DIBufIndex][u8DINwayindex]  == E_DIP_TOP_FIELD)
    {
        //find DI read top field index
        u16check_index = u8DIBuf_WrTopIndex[u8DINwayindex];
        // ddir force top frame index
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_7C_L, u16check_index << 9, BMASK(11: 9));
        u8DIBuf_RdTopIndex[u8DINwayindex] = u16check_index;
    }
    else
    {
        //find DI read top field index
        u16check_index = (u8DIBuf_WrTopIndex[u8DINwayindex] == 0) ? 2 : 0;
        // ddir force top frame index
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_7C_L, u16check_index << 9, BMASK(11: 9));
        u8DIBuf_RdTopIndex[u8DINwayindex] = u16check_index;
    }

    DI_DBG_MSG(printf("WrTop = %d, WrBot = %d\n", u8DIBuf_WrTopIndex[u8DINwayindex], u8DIBuf_WrBotIndex[u8DINwayindex]);)
    DI_DBG_MSG(printf("RdTop = %d, RdBot = %d\n", u8DIBuf_RdTopIndex[u8DINwayindex], u8DIBuf_RdBotIndex[u8DINwayindex]);)

#else

    u8DIBufIndex = u8DI_DIPbuf_index[u8DINwayindex];
    DI_DBG_MSG(printf("[%d]phyDIBufAddr=0x%x,u32DIBufSize=0x%x\n", u8DIBufIndex, phyDIBufAddr, u32DIBufSize);)

    switch(stDIPRDISetting.enDI_Field)
    {
        case E_DIP_3DDI_TOP_SINGLE_FIELD:
        case E_DIP_3DDI_TOP_BOTH_FIELD_STAGGER:
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7C_L, 0, BIT(7));  //Forced dipr field, 0: top field  1: bottom field
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_7C_L, 0, BIT(7));  //Forced dipr field, 0: top field  1: bottom field
            enTB_DIPField[u8DIBufIndex][u8DINwayindex] = E_DIP_TOP_FIELD ;
            break;
        case E_DIP_3DDI_BOTTOM_SINGLE_FIELD:
        case E_DIP_3DDI_BOTTOM_BOTH_FIELD_STAGGER:
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_7C_L, BIT(7), BIT(7));  //Forced dipr field, 0: top field  1: bottom field
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_7C_L, BIT(7), BIT(7));  //Forced dipr field, 0: top field  1: bottom field
            enTB_DIPField[u8DIBufIndex][u8DINwayindex] = E_DIP_BOTTOM_FIELD ;
            break;
        default:
            printf("[ERROR][%s,%d],Chip Not Support(%d)\n", __func__, __LINE__, stDIPRDISetting.enDI_Field);
            break;
    }

    //DI write index
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_3B_L, u8DIBufIndex << 1, BMASK(3: 1));

    //find DI read bottom field index
    for(u16loop_index = 1; u16loop_index < u8DIBufTotal; u16loop_index++)
    {
        if((u8DIBufIndex - u16loop_index) >= 0)
            u16check_index = u8DIBufIndex - u16loop_index;
        else
            u16check_index = u8DIBufIndex + u8DIBufTotal - u16loop_index;
        if(enTB_DIPField[u16check_index][u8DINwayindex] == E_DIP_BOTTOM_FIELD)
        {
            // ddir force bottom frame index
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_3B_L, u16check_index << 7, BMASK(9: 7));
            break;
        }
        if(u16loop_index == u8DIBufTotal)
        {
            if(u8DIBufIndex > 0)
                u16check_index = u8DIBufIndex - 1;
            else
                u16check_index = u8DIBufIndex + u8DIBufTotal - 1;

            // ddir force bottom frame index
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_3B_L, u16check_index << 7, BMASK(9: 7));
            printf("[warning]No bottom field in DI buffer,DIBufIndex=%d\n", u8DIBufIndex);
            for(u8i = 0; u8i < u8DIBufTotal; u8i++)
            {
                printf(" enTB_DIPField=%d \n", enTB_DIPField[u8i][u8DINwayindex]);
            }
            printf("\n");
        }
    }

    //find DI read top field index
    for(u16loop_index = 1; u16loop_index < u8DIBufTotal; u16loop_index++)
    {
        if(u8DIBufIndex >= u16loop_index)
            u16check_index = u8DIBufIndex - u16loop_index;
        else
            u16check_index = u8DIBufIndex + u8DIBufTotal - u16loop_index;
        if(enTB_DIPField[u16check_index][u8DINwayindex] == E_DIP_TOP_FIELD)
        {
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_7C_L, u16check_index << 9, BMASK(11: 9));
            break;
        }
        if(u16loop_index == u8DIBufTotal)
        {
            if(u8DIBufIndex > 0)
                u16check_index = u8DIBufIndex - 1;
            else
                u16check_index = u8DIBufIndex + u8DIBufTotal - 1;
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_7C_L, u16check_index << 9, BMASK(11: 9));
            printf("[warning]No top field in DI buffer,DIBufIndex=%d\n", u8DIBufIndex);
        }
    }

    if((u8DIBufIndex + 1) >= u8DIBufTotal)
    {
        u8DI_DIPbuf_index[u8DINwayindex] = 0;
    }
    else
    {
        u8DI_DIPbuf_index[u8DINwayindex] = u8DI_DIPbuf_index[u8DINwayindex] + 1;
    }

#endif

    DI_DBG_MSG(printf("DIBufIndex = [%d]\n", u8DIBufIndex);)
    for(u8i = 0; u8i < u8DIBufTotal; u8i++)
    {
        DI_DBG_MSG(printf(" TBField[%d]=%d ", u8i, enTB_DIPField[u8i][u8DINwayindex]);)
    }
    DI_DBG_MSG(printf(" (1:TOP ; 2:bottom)\n");)

    //DIPR enable
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_7D_L, BIT(14), BIT(14));  //[5:0]:reg_dipr_req_thrd, [7:6]:reg_dipr_format, [12:8]:reg_dipr_hi_pri_num, [14]: reg_dipr_en
    //set write fmt
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_01_L, BIT(5), BMASK(5: 4));  //dip data format (00: yc422  01: rgb565  10: argb8888  11: yc420)
    //set read fmt
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_7D_L, BIT(7), BMASK(7: 6));  //[5:0]:reg_dipr_req_thrd, [7:6]:reg_dipr_format, [12:8]:reg_dipr_hi_pri_num, [14]: reg_dipr_en
    //DDIW Y low bound
    SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK43_10_L, phyDIBufAddr / u16BusSize);
    //DDIW Y high bound
    SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK43_30_L, (phyDIBufAddr + u32DIBufSize) / u16BusSize);
    //DDIW C low bound
    SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK43_20_L, phyDIBufAddr / u16BusSize);
    //DDIW C high bound
    SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK43_40_L, (phyDIBufAddr + u32DIBufSize) / u16BusSize);
    //DI DIPR Y buffer
    SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK43_78_L, phyDIBufAddr / u16BusSize);
    //DI DIPR C buffer
    SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK43_7A_L, phyDIBufAddr / u16BusSize);
    //Write count
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_01_L, (u8DIBufTotal - 1), BMASK(2: 0));
    //Read count
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_74_L, (u8DIBufTotal - 1) << 13, BMASK(15: 13));  //Dipr frame buffer number

#if 0  // motion debugging mode
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK47_7A_L, BIT(8), BIT(8));  // Motion debugging mode selection:
#endif
    if(stDIPRDISetting.bEnableBob == TRUE)
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK47_78_L, BIT(7) | BMASK(3: 0), BIT(7) | BMASK(3: 0));  // Weave mode:0x80, Bob mode:0x8f, 3DDI enable:0x00
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK47_79_L, BIT(7) | BMASK(3: 0), BIT(7) | BMASK(3: 0));  // Weave mode:0x80, Bob mode:0x8f, 3DDI enable:0x00
#if (DIP_PLATFORM == DIP_I2)
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_14_L, 0, BIT(10));  // disable DNR
        if((stDIPRDISetting.enDI_Field == E_DIP_3DDI_TOP_SINGLE_FIELD) || (stDIPRDISetting.enDI_Field == E_DIP_3DDI_TOP_BOTH_FIELD_STAGGER))
        {
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_15_L, BIT(5) | BIT(8), BIT(5) | BIT(6) | BIT(8));  // [5]:reg_lb_ctrl_flag_force_en, [6]:Top Field: 1'd0  Bot Field: 1'd1, [8]:reg_force_no_first_two_frm
        }
        else
        {
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_15_L, BIT(5) | BIT(6) | BIT(8), BIT(5) | BIT(6) | BIT(8));  // [5]:reg_lb_ctrl_flag_force_en, [6]:Top Field: 1'd0  Bot Field: 1'd1, [8]:reg_force_no_first_two_frm
        }
#endif
    }
    else
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK47_78_L, 0x0, BIT(7) | BMASK(3: 0));  // Weave mode:0x80, Bob mode:0x8f, 3DDI enable:0x00
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK47_79_L, 0x0, BIT(7) | BMASK(3: 0));  // Weave mode:0x80, Bob mode:0x8f, 3DDI enable:0x00
#if (DIP_PLATFORM == DIP_I2)
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_14_L, BIT(10), BIT(10));  // enable DNR

        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK43_15_L, BIT(8), BIT(5) | BIT(6) | BIT(8));  // [5]:reg_lb_ctrl_flag_force_en, [8]:reg_force_no_first_two_frm
#endif
    }

}

void HAL_XC_DIP_Rotation(void *pstCmdQInfo, MS_BOOL bRotation, EN_XC_DIP_ROTATION eRoDirection, MS_U32 u64StartAddr, SCALER_DIP_WIN eTmpWindow)
{
#if (DIP_PLATFORM == DIP_I2)
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;
    //MS_BOOL bRotateType = FALSE;
    bDIPRotation = bRotation;
    DIPRotation = eTmpWindow;

    if(debug_level >= 1)
    {
        printf("bDIPRotation=%d, RoDirection = %d\n", bDIPRotation, eRoDirection);
    }

    //reg_rotate_mode
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_10_L, eRoDirection << 14, BIT(14));  // 0: rotate 270, 1: rotate 90
    //continue mode
    //SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_6F_L,(bRotation?BIT(15):0),BIT(15));
    //reg_rotate_en
    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK34_10_L, (bRotation ? BIT(15) : 0), BIT(15));

#endif

}
void HAL_XC_DIP_SetPinpon(void *pstCmdQInfo, MS_BOOL bPinpon, MS_U32 u64PinponAddr, MS_U32 u64OffSet, SCALER_DIP_WIN eWindow)
{
    MS_U16 u16BusSize = 0;
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    u16BusSize = HAL_XC_DIP_GetBusSize(pstCmdQInfo, eWindow);

    SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK36_20_L, u64PinponAddr / u16BusSize);
    SC_W4BYTE_CMDQ(pstCmdQ, REG_SC_BK36_50_L, u64OffSet);

}

SCALER_DIP_WIN HAL_XC_DIP_GetHVSP(void* pstCmdQInfo, SCALER_DIP_WIN eWindow)
{
    MS_U16 u16Reg = 0;
#if 1  // for CmdQ timing

    u16Reg = (g_bDIPHVSPStatus) ? BIT(2) : BMASK(2: 1);

#else

    u16Reg = SC_R2BYTEMSK(0, REG_SC_BK49_70_L, BMASK(2: 1));

#endif

    if(u16Reg == BIT(2))
        return DIP_WINDOW;
    else if(u16Reg == 0x0)
        return DWIN0_WINDOW;
    else if(u16Reg == BIT(1))
        return DWIN1_WINDOW;
    else
        return MAX_DIP_WINDOW;

}

MS_BOOL HAL_XC_DIP_SetHVSP(void* pstCmdQInfo, MS_BOOL bSelect, SCALER_DIP_WIN eWindow)
{
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    if(bSelect == FALSE)
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK49_70_L, BMASK(2: 1), BMASK(2: 1));
    }
    else
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK49_70_L, BIT(2), BMASK(2: 1));
    }
    g_bDIPHVSPStatus = bSelect;

    return TRUE;
}

MS_BOOL HAL_XC_DIP_Set420TileBlock(void* pstCmdQInfo, EN_XC_DIP_TILE_BLOCK eTileBlock, SCALER_DIP_WIN eWindow)
{
    MS_U16 u16SAV_BW = 0;
    MS_BOOL bTILE_BLOCK_R = FALSE;
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    if(eTileBlock == DIP_TILE_BLOCK_R_16_32)
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_0B_L, 0x0, BIT(13));
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_0B_L, 0x0, BIT(15));
        u16SAV_BW = BIT(6);
        bTILE_BLOCK_R = TRUE;
    }
    else if(eTileBlock == DIP_TILE_BLOCK_R_32_16)
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_0B_L, BIT(13), BIT(13));
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_0B_L, 0x0, BIT(15));
        bTILE_BLOCK_R = TRUE;
    }
    else if(eTileBlock == DIP_TILE_BLOCK_R_32_32)
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_0B_L, 0x0, BIT(13));
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_0B_L, BIT(15), BIT(15));
        bTILE_BLOCK_R = TRUE;
    }
    else if( (eTileBlock == DIP_TILE_BLOCK_W_16_32))
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_0B_L, 0x0, BIT(12));
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_0B_L, 0x0, BIT(14));
    }
    else if( (eTileBlock == DIP_TILE_BLOCK_W_32_16))
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_0B_L, BIT(12), BIT(12));
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_0B_L, 0x0, BIT(14));
    }
    else if( (eTileBlock == DIP_TILE_BLOCK_W_32_32))
    {
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_0B_L, 0x0, BIT(12));
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_0B_L, BIT(14), BIT(14));
    }
    else
        return FALSE;

    if(eWindow == DIP_WINDOW && bTILE_BLOCK_R == TRUE)
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_0C_L, u16SAV_BW, BIT(6));

    return TRUE;
}

void HAL_XC_DIP_2P_Width_Check(void *pstCmdQInfo, XC_SETWIN_INFO *pstXC_SetWin_Info, SCALER_DIP_WIN eWindow)
{
    //MS_U8  u8bpp = 2;
    MS_U16 u16alignment = 16;
    MS_U16 u16tmp = 0;
    MS_U16 u16MaxWith = 0;
    MS_BOOL bNeedAverageMode = FALSE;

    u16MaxWith = HAL_XC_DIP_GetWidth(pstCmdQInfo, g_DIPSrcInfo[eWindow].enDIPSourceType, eWindow);
    bNeedAverageMode = IsNeedAverageMode(g_DIPSrcInfo[eWindow].enDIPSourceType, u16MaxWith);

    if(debug_level >= 1)
    {
        printf("u16MaxWith=%d, bNeedAverageMode=%d\n", u16MaxWith, bNeedAverageMode);
    }

    if(bNeedAverageMode)
    {
        if((pstXC_SetWin_Info->u16PreHCusScalingDst) > (pstXC_SetWin_Info->u16PreHCusScalingSrc / 2))
        {
            pstXC_SetWin_Info->u16PreHCusScalingDst = pstXC_SetWin_Info->u16PreHCusScalingSrc / 2;
        }
    }

#if 1  // modify alignment to 2
    u16alignment = 2;
#else

    if(HAL_XC_DIP_GetBusSize(pstCmdQInfo, eWindow) == DIP_BYTE_PER_WORD)
    {
        u16alignment = DIP_BYTE_PER_WORD / u8bpp;
    }
    else
    {
        u16alignment = DWIN_BYTE_PER_WORD / u8bpp;
    }
#endif

    u16tmp = (pstXC_SetWin_Info->u16PreHCusScalingDst) / u16alignment;
    pstXC_SetWin_Info->u16PreHCusScalingDst = u16tmp * u16alignment;
}

void HAL_XC_DIP_Check_Scale(MS_U64* u64ReqHdl, MS_U16 *u16H_Scaling_Enable, MS_U32 *u32H_Scaling_Ratio, MS_U16 *u16V_Scaling_Enable, MS_U32 *u32V_Scaling_Ratio, SCALER_DIP_WIN eWindow)
{
    //HV scaling enable/ratio
    *u16H_Scaling_Enable = (SC_R4BYTE(0, REG_SC_BK34_04_L) & BIT(31)) >> 31;
    *u32H_Scaling_Ratio =  SC_R4BYTE(0, REG_SC_BK34_04_L) & BMASK(22: 0);
    *u16V_Scaling_Enable = (SC_R4BYTE(0, REG_SC_BK34_08_L) & BIT(31)) >> 31;
    *u32V_Scaling_Ratio =  SC_R4BYTE(0, REG_SC_BK34_08_L) & BMASK(22: 0);
}

void HAL_XC_DIP_Enable(void *pstCmdQInfo, MS_BOOL bEnable)
{
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    if(bEnable)
    {
#ifdef MSOS_TYPE_OPTEE
        MDrv_WriteRegBit_CmdQ(pstCmdQ, REG_TZPC_NONPM_DIP, 0, BMASK(1: 0));
        MDrv_WriteRegBit_CmdQ(pstCmdQ, REG_TZPC_NONPM_DWIN0, 0, BMASK(9: 8));
#endif
    }
    else
    {
#ifdef MSOS_TYPE_OPTEE
        MDrv_WriteRegBit_CmdQ(pstCmdQ, REG_TZPC_NONPM_DIP, BMASK(1: 0), BMASK(1: 0));
        MDrv_WriteRegBit_CmdQ(pstCmdQ, REG_TZPC_NONPM_DWIN0, BMASK(9: 8), BMASK(9: 8));
#endif
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_02_L, BIT(10), BIT(10));
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK3B_02_L, BIT(10), BIT(10));
    }

}

void HAL_XC_DIP_SetPixelShift(void *pstCmdQInfo, MS_U16 u16OffSet, SCALER_DIP_WIN eWindow)
{
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_0D_L, u16OffSet, BMASK(4: 0));

}

void HAL_XC_DIP_MDWinInit(void *pstCmdQInfo, MS_U8 u8WinId)
{
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    if( u8WinId == 0 )
    {
        //SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BKF1_01_L, 1, BIT(0));  // MDWIN SW reset
        //SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BKF1_01_L, 0, BIT(0));  // MDWIN SW reset

        // Set DWIN input format: YUV
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BKF2_60_L, BIT(0), BIT(0));  // DWIN input format, 0:rgb, 1:yuv
        // Set DWIN on
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BKF2_72_L, 0, BIT(0));  // DWIN on/off
        // DWIN dbf off
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BKF2_72_L, 0, BIT(7));  // DWIN dbf_off
    }
    else
    {
        //SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BKF5_01_L, 1, BIT(0));
        //SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BKF5_01_L, 0, BIT(0));

        // Set DWIN input format: YUV
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BKF6_60_L, BIT(0), BIT(0));  // DWIN input format, 0:rgb, 1:yuv
        // Set DWIN on
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BKF6_72_L, 0, BIT(0));  // DWIN on/off
        // DWIN dbf off
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BKF6_72_L, 0, BIT(7));  // DWIN dbf_off
    }

}

void HAL_XC_DIP_MDWin_SetConfig(void *pstCmdQInfo, DIP_MDWIN_PROPERTY *stMDWinProperty)
{
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;
    MS_U32 u32BaseAddr = (stMDWinProperty->u32BaseAddr >> 5);

    if(stMDWinProperty->bEnable)
    {
        if( stMDWinProperty->u8WinId == 0 )
        {
            // Set Window id
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BKF1_37_L, stMDWinProperty->u8WinIndex, BMASK(3: 0));
            // Set mem config format, 0x30: YC422 8b, 0x32: YC420 8b, 0x20: YC444 8b, 0x00: ARGB, 0x1C: RGB565
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BKF1_01_L, (stMDWinProperty->u8Format << 8), BMASK(13: 8));
            // Set DWIN MIU line offset
            SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BKF1_02_L, stMDWinProperty->u16Lineoffset);
            // Set DWIN MIU Base address
            SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BKF1_03_L, u32BaseAddr & 0xffff);
            SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BKF1_04_L, ((u32BaseAddr >> 16) & 0xffff));
            // Set DWIN input hsize
            SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BKF1_2D_L, stMDWinProperty->u16Width);
            // Set DWIN input vsize
            SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BKF1_2F_L, stMDWinProperty->u16Height);
            // Set DWIN h mirror and v flip
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BKF2_78_L, (stMDWinProperty->bHmirror | (stMDWinProperty->bVflip << 1)), BMASK(1: 0));
        }
        else
        {
            // Set Window id
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BKF5_37_L, stMDWinProperty->u8WinIndex, BMASK(3: 0));
            // Set mem config format, 0x30: YC422 8b, 0x32: YC420 8b, 0x20: YC444 8b, 0x00: ARGB, 0x1C: RGB565
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BKF5_01_L, (stMDWinProperty->u8Format << 8), BMASK(13: 8));
            // Set DWIN MIU line offset
            SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BKF5_02_L, stMDWinProperty->u16Lineoffset);
            // Set DWIN MIU Base address
            SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BKF5_03_L, u32BaseAddr & 0xffff);
            SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BKF5_04_L, ((u32BaseAddr >> 16) & 0xffff));
            // Set DWIN input hsize
            SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BKF5_2D_L, stMDWinProperty->u16Width);
            // Set DWIN input vsize
            SC_W2BYTE_CMDQ(pstCmdQ, REG_SC_BKF5_2F_L, stMDWinProperty->u16Height);
            // Set DWIN h mirror and v flip
            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BKF6_78_L, (stMDWinProperty->bHmirror | (stMDWinProperty->bVflip << 1)), BMASK(1: 0));

            SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BKF6_70_L, 0, BIT(13));  // reg_mdw_irq_mask

        }

        // dip source to mdwin enable/disable
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_0C_L, BIT(12), BIT(12));  // dip source to mdwin enable

        //SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK00_55_L, 0, BIT(5) | BIT(6) | BIT(13) | BIT(14));  // mdw frame done irq mask

    }
    else
    {
        // dip source to mdwin enable/disable
        SC_W2BYTEMSK_CMDQ(pstCmdQ, REG_SC_BK36_0C_L, 0, BIT(12));  // dip source to mdwin enable
    }

}

void HAL_XC_DIP_COVER_SetConfig(void *pstCmdQInfo, DIP_COVER_PROPERTY *stDIPCoverProperty)
{
    MHAL_CMDQ_CmdqInterface_t *pstCmdQ = (MHAL_CMDQ_CmdqInterface_t *)pstCmdQInfo;

    if(stDIPCoverProperty->bEnable)
    {
        MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_COVER_ENABLE(0), 0x01, BIT(0));
        MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_COVER_BWIN_HSTART(0), (stDIPCoverProperty->u16WinHStart + 1), BMASK(12: 0));
        MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_COVER_BWIN_HEND(0), (stDIPCoverProperty->u16WinHStart + stDIPCoverProperty->u16WinHSize), BMASK(12: 0));
        MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_COVER_BWIN_VSTART(0), (stDIPCoverProperty->u16WinVStart + 1), BMASK(12: 0));
        MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_COVER_BWIN_VEND(0), (stDIPCoverProperty->u16WinVStart + stDIPCoverProperty->u16WinVSize), BMASK(12: 0));

        if(debug_level >= 1)
        {
            printf("[COVER] R=%d, G=%d, B=%d\n", stDIPCoverProperty->u16ColorR, stDIPCoverProperty->u16ColorG, stDIPCoverProperty->u16ColorB);
        }

        MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_COVER_BWIN_COLOR_GR(0), stDIPCoverProperty->u16ColorR, BMASK(7: 0)); // Fill color R
        MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_COVER_BWIN_COLOR_GR(0), (stDIPCoverProperty->u16ColorG << 8), BMASK(15: 8)); // Fill color G
        MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_COVER_BWIN_COLOR_B(0), stDIPCoverProperty->u16ColorB, BMASK(7: 0)); // Fill color B
    }
    else
    {
        MDrv_Write2ByteMask_CmdQ(pstCmdQ, REG_COVER_ENABLE(0), 0, BIT(0));
    }

}
//=============== DIP =====================//

#undef  MHAL_SC_C
