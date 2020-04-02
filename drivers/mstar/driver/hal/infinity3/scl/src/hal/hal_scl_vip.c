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
#define HAL_VIP_C


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition
#include "drv_scl_os.h"
#include "drv_scl_dbg.h"
#include "hal_utility.h"
// Internal Definition
#include "hal_scl_reg.h"
#include "drv_scl_vip.h"
#include "hal_scl_util.h"
#include "hal_scl_vip.h"
#include "drv_scl_cmdq.h"
#include "drv_scl_irq_st.h"
#include "drv_scl_irq.h"
#include "drv_scl_pq_define.h"
#include "Infinity3e_Main.h"             // table config parameter
//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define HAL_SCLVIP_DBG(x)
#define HAL_SCLVIP_MUTEX_LOCK()            DrvSclOsObtainMutex(_HalVIP_Mutex,SCLOS_WAIT_FOREVER)
#define HAL_SCLVIP_MUTEX_UNLOCK()          DrvSclOsReleaseMutex(_HalVIP_Mutex)
#define WDR_SRAM_NUM 8
#define WDR_SRAM_BYTENUM (PQ_IP_WDR_Loc_TBL_0_SRAM_SIZE_Main/2)
#define WDR_SRAM_USERBYTENUM (81)
#define GAMMAY_SRAM_BYTENUM (PQ_IP_YUV_Gamma_tblY_SRAM_SIZE_Main/4)
#define GAMMAU_SRAM_BYTENUM (PQ_IP_YUV_Gamma_tblU_SRAM_SIZE_Main/4)
#define GAMMAV_SRAM_BYTENUM (PQ_IP_YUV_Gamma_tblV_SRAM_SIZE_Main/4)
#define GAMMA10to12R_SRAM_BYTENUM (PQ_IP_ColorEng_GM10to12_Tbl_R_SRAM_SIZE_Main/4)
#define GAMMA10to12G_SRAM_BYTENUM (PQ_IP_ColorEng_GM10to12_Tbl_G_SRAM_SIZE_Main/4)
#define GAMMA10to12B_SRAM_BYTENUM (PQ_IP_ColorEng_GM10to12_Tbl_B_SRAM_SIZE_Main/4)
#define GAMMA12to10R_SRAM_BYTENUM (PQ_IP_ColorEng_GM12to10_CrcTbl_R_SRAM_SIZE_Main/4)
#define GAMMA12to10G_SRAM_BYTENUM (PQ_IP_ColorEng_GM12to10_CrcTbl_G_SRAM_SIZE_Main/4)
#define GAMMA12to10B_SRAM_BYTENUM (PQ_IP_ColorEng_GM12to10_CrcTbl_B_SRAM_SIZE_Main/4)
//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
u32 VIP_RIU_BASE = 0;
bool gbCMDQ = 0;
s32 _HalVIP_Mutex = -1;

//-------------------------------------------------------------------------------------------------
//  Functions
//-------------------------------------------------------------------------------------------------
void HalSclVipExit(void)
{
    if(_HalVIP_Mutex != -1)
    {
        DrvSclOsDeleteMutex(_HalVIP_Mutex);
        _HalVIP_Mutex = -1;
    }
}

void HalSclVipSetRiuBase(u32 u32riubase)
{
    char word[] = {"_HalVIP_Mutex"};
    VIP_RIU_BASE = u32riubase;
    _HalVIP_Mutex = DrvSclOsCreateMutex(E_DRV_SCLOS_FIFO, word, SCLOS_PROCESS_SHARED);

    if (_HalVIP_Mutex == -1)
    {
        SCL_ERR("[DRVHVSP]%s(%d): create mutex fail\n", __FUNCTION__, __LINE__);
    }
}
void HalSclVipSeMcnrIPMRead(bool bEn)
{
    HalUtilityW2BYTEMSK(REG_SCL_DNR1_01_L, bEn ? BIT0 : 0, BIT0);//IOenable
}
void HalSclVipSetIPMConpress(bool bEn)
{
    HalUtilityW2BYTEMSK(REG_SCL_DNR1_30_L, bEn ? BIT0 : 0, BIT0);//conpress
    HalUtilityW2BYTEMSK(REG_SCL_DNR1_30_L, bEn ? BIT1 : 0, BIT1);//decon
}
void HalSclVipSeCiirRead(bool bEn)
{
    HalUtilityW2BYTEMSK(REG_SCL_DNR1_01_L, bEn ? BIT2 : 0, BIT2);//IOenable
}
void HalSclVipSeCiirWrite(bool bEn)
{
    HalUtilityW2BYTEMSK(REG_SCL_DNR1_01_L, bEn ? BIT3 : 0, BIT3);//IOenable
}
void _HalSclVipSramDumpIhcIcc(HalSclVipSramDumpType_e endump,u32 u32reg)
{
    u8  u8sec = 0;
    HalSclVipSramSecNum_e enSecNum;
    u16 u16addr = 0,u16tvalue = 0,u16tcount = 0,u16readdata;
    HalUtilityW2BYTEMSK(u32reg, BIT0, BIT0);//IOenable
    for(u8sec=0;u8sec<4;u8sec++)
    {
        switch(u8sec)
        {
            case 0:
                enSecNum = E_HAL_SCLVIP_SRAM_SEC_0;
                break;
            case 1:
                enSecNum = E_HAL_SCLVIP_SRAM_SEC_1;
                break;
            case 2:
                enSecNum = E_HAL_SCLVIP_SRAM_SEC_2;
                break;
            case 3:
                enSecNum = E_HAL_SCLVIP_SRAM_SEC_3;
                break;
            default:
                break;

        }
        HalUtilityW2BYTEMSK(u32reg, u8sec<<1, BIT1|BIT2);//sec
        for(u16addr=0;u16addr<enSecNum;u16addr++)
        {
            if(endump == E_HAL_SCLVIP_SRAM_DUMP_IHC)
            {
                u16tvalue = MST_VIP_IHC_CRD_SRAM_Main[0][u16tcount] | (MST_VIP_IHC_CRD_SRAM_Main[0][u16tcount+1]<<8);
            }
            else
            {
                u16tvalue = MST_VIP_ICC_CRD_SRAM_Main[0][u16tcount] | (MST_VIP_ICC_CRD_SRAM_Main[0][u16tcount+1]<<8);
            }
            HalUtilityW2BYTEMSK(u32reg+2, u16addr, 0x01FF);//addr
            HalUtilityW2BYTEMSK(u32reg+4, (u16)u16tvalue, 0x3FF);//data
            HalUtilityW2BYTEMSK(u32reg+4, BIT15, BIT15);//wen
            //if(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG)
            {
                HalUtilityW2BYTEMSK(u32reg+2, BIT15, BIT15);//ren
                SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[ICCIHC]tval:%hx\n", u16tvalue);
                u16readdata=HalUtilityR2BYTEDirect(u32reg+6);
                SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[ICCIHC]reg tval:%hx\n", u16readdata);
            }
            u16tcount+=2;
        }
    }

    HalUtilityW2BYTEMSK(u32reg, 0, BIT0);//IOenable
}
void _HalSclVipSramDumpHsp(u32 u32Sram,u32 u32reg)
{
    u16 u16addr = 0,u16tvalue = 0,u16tcount = 0,u16readdata;
    //clear
    for(u16addr = 64;u16addr<128;u16addr++)
    {
        HalUtilityW2BYTEMSK(u32reg, BIT0, BIT0);//Yenable
        HalUtilityW2BYTEMSK(u32reg+2, u16addr, 0xFF);
        HalUtilityW2BYTEMSK(u32reg+4, 0, 0xFFFF);
        HalUtilityW2BYTEMSK(u32reg+6, 0, 0xFFFF);
        HalUtilityW2BYTEMSK(u32reg+8, 0, 0xFF);
        HalUtilityW2BYTEMSK(u32reg, BIT8, BIT8);//W pulse
        HalUtilityW2BYTEMSK(u32reg, 0, BIT0);//Yenable

        HalUtilityW2BYTEMSK(u32reg, BIT1, BIT1);//Cenable
        HalUtilityW2BYTEMSK(u32reg+4, 0, 0xFFFF);
        HalUtilityW2BYTEMSK(u32reg+6, 0, 0xFFFF);
        HalUtilityW2BYTEMSK(u32reg+8, 0, 0xFF);
        HalUtilityW2BYTEMSK(u32reg, BIT8, BIT8);//W pulse
        HalUtilityW2BYTEMSK(u32reg, 0, BIT1);//Cenable
    }
    //SRAM 0 //0~64 entry is V  65~127 is H

    for(u16addr=0;u16addr<(PQ_IP_SRAM1_SIZE_Main/5);u16addr++)
    {
           // 64~127
        HalUtilityW2BYTEMSK(u32reg, BIT0, BIT0);//Yenable
        HalUtilityW2BYTEMSK(u32reg+2, u16addr+64, 0xFF);
        if(u32Sram&0x1)
        {
            u16tvalue = MST_SRAM1_Main[((u32Sram&0xF0)>>4)][u16tcount] |
                (MST_SRAM1_Main[((u32Sram&0xF0)>>4)][u16tcount+1]<<8);
        }
        else
        {
            u16tvalue = MST_SRAM2_Main[((u32Sram&0xF0)>>4)][u16tcount] |
                (MST_SRAM2_Main[((u32Sram&0xF0)>>4)][u16tcount+1]<<8);
        }
        SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[HVSP]VYtval12:%hx\n", u16tvalue);
        HalUtilityW2BYTEMSK(u32reg+4, u16tvalue, 0xFFFF);
        if(u32Sram&0x1)
        {
            u16tvalue = MST_SRAM1_Main[((u32Sram&0xF0)>>4)][u16tcount+2] |
                (MST_SRAM1_Main[((u32Sram&0xF0)>>4)][u16tcount+3]<<8);
        }
        else
        {
            u16tvalue = MST_SRAM2_Main[((u32Sram&0xF0)>>4)][u16tcount+2] |
                (MST_SRAM2_Main[((u32Sram&0xF0)>>4)][u16tcount+3]<<8);
        }
        HalUtilityW2BYTEMSK(u32reg+6, u16tvalue, 0xFFFF);
        SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[HVSP]VYtval34:%hx\n", u16tvalue);
        if(u32Sram&0x1)
        {
            u16tvalue = MST_SRAM1_Main[((u32Sram&0xF0)>>4)][u16tcount+4];
        }
        else
        {
            u16tvalue = MST_SRAM2_Main[((u32Sram&0xF0)>>4)][u16tcount+4];
        }
        SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[HVSP]Vtval5:%hx\n", u16tvalue);
        HalUtilityW2BYTEMSK(u32reg+8, u16tvalue, 0xFF);
        HalUtilityW2BYTEMSK(u32reg, BIT8, BIT8);//W pulse
        //if(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG)
        {
            HalUtilityW2BYTEMSK(u32reg, BIT9, BIT9);//R pulse
            u16readdata = HalUtilityR2BYTEDirect(u32reg+10);
            SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[HVSP]Vread Yval 12:%hx\n", u16readdata);
            u16readdata = HalUtilityR2BYTEDirect(u32reg+12);
            SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[HVSP]Vread Yval 34:%hx\n", u16readdata);
            u16readdata = HalUtilityR2BYTEDirect(u32reg+14);
            SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[HVSP]Vread Yval 5:%hx\n", u16readdata);
        }
        HalUtilityW2BYTEMSK(u32reg, 0, BIT0);//Yenable

        HalUtilityW2BYTEMSK(u32reg, BIT1, BIT1);//Cenable
        u16tvalue = MST_C_SRAM1_Main[0][u16tcount] | (MST_C_SRAM1_Main[0][u16tcount+1]<<8);
        HalUtilityW2BYTEMSK(u32reg+4, u16tvalue, 0xFFFF);
        u16tvalue = MST_C_SRAM1_Main[0][u16tcount+2] | (MST_C_SRAM1_Main[0][u16tcount+3]<<8);
        HalUtilityW2BYTEMSK(u32reg+6, u16tvalue, 0xFFFF);
        u16tvalue = MST_C_SRAM1_Main[0][u16tcount+4];
        HalUtilityW2BYTEMSK(u32reg+8, u16tvalue, 0xFF);
        HalUtilityW2BYTEMSK(u32reg, BIT8, BIT8);//W pulse
        //if(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG)
        {
            HalUtilityW2BYTEMSK(u32reg, BIT9, BIT9);//R pulse
            u16readdata = HalUtilityR2BYTEDirect(u32reg+10);
            SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[HVSP]Vread Cval 12:%hx\n", u16readdata);
            u16readdata = HalUtilityR2BYTEDirect(u32reg+12);
            SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[HVSP]Vread Cval 34:%hx\n", u16readdata);
            u16readdata = HalUtilityR2BYTEDirect(u32reg+14);
            SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[HVSP]Vread Cval 5:%hx\n", u16readdata);
        }
        HalUtilityW2BYTEMSK(u32reg, 0, BIT1);//Cenable
        u16tcount += 5;
    }
    //SRAM 1 //128~256 entry  link to 0~127 , so do not use
    u16tcount = 0;
}
void _HalSclVipSramDumpVsp(u32 u32Sram,u32 u32reg)
{
    u16 u16addr = 0,u16tvalue = 0,u16tcount = 0,u16readdata;
    //clear
    for(u16addr = 0;u16addr<64;u16addr++)
    {
        HalUtilityW2BYTEMSK(u32reg, BIT0, BIT0);//Yenable
        HalUtilityW2BYTEMSK(u32reg+2, u16addr, 0xFF);
        HalUtilityW2BYTEMSK(u32reg+4, 0, 0xFFFF);
        HalUtilityW2BYTEMSK(u32reg+6, 0, 0xFFFF);
        HalUtilityW2BYTEMSK(u32reg+8, 0, 0xFF);
        HalUtilityW2BYTEMSK(u32reg, BIT8, BIT8);//W pulse
        HalUtilityW2BYTEMSK(u32reg, 0, BIT0);//Yenable

        HalUtilityW2BYTEMSK(u32reg, BIT1, BIT1);//Cenable
        HalUtilityW2BYTEMSK(u32reg+4, 0, 0xFFFF);
        HalUtilityW2BYTEMSK(u32reg+6, 0, 0xFFFF);
        HalUtilityW2BYTEMSK(u32reg+8, 0, 0xFF);
        HalUtilityW2BYTEMSK(u32reg, BIT8, BIT8);//W pulse
        HalUtilityW2BYTEMSK(u32reg, 0, BIT1);//Cenable
    }
    //SRAM 0 ///0~64 entry is V  65~127 is H

    for(u16addr=0;u16addr<(PQ_IP_SRAM1_SIZE_Main/5);u16addr++)
    {
        HalUtilityW2BYTEMSK(u32reg, BIT0, BIT0);//Yenable
        HalUtilityW2BYTEMSK(u32reg+2, u16addr, 0xFF);
        if(u32Sram&0x1)
        {
            u16tvalue = MST_SRAM1_Main[((u32Sram&0xF0)>>4)][u16tcount] |
                (MST_SRAM1_Main[((u32Sram&0xF0)>>4)][u16tcount+1]<<8);
        }
        else
        {
            u16tvalue = MST_SRAM2_Main[((u32Sram&0xF0)>>4)][u16tcount] |
                (MST_SRAM2_Main[((u32Sram&0xF0)>>4)][u16tcount+1]<<8);
        }
        HalUtilityW2BYTEMSK(u32reg+4, u16tvalue, 0xFFFF);
        SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[HVSP]Ytval12:%hx\n", u16tvalue);
        if(u32Sram&0x1)
        {
            u16tvalue = MST_SRAM1_Main[((u32Sram&0xF0)>>4)][u16tcount+2] |
                (MST_SRAM1_Main[((u32Sram&0xF0)>>4)][u16tcount+3]<<8);
        }
        else
        {
            u16tvalue = MST_SRAM2_Main[((u32Sram&0xF0)>>4)][u16tcount+2] |
                (MST_SRAM2_Main[((u32Sram&0xF0)>>4)][u16tcount+3]<<8);
        }
        HalUtilityW2BYTEMSK(u32reg+6, u16tvalue, 0xFFFF);
        SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[HVSP]Ytval34:%hx\n", u16tvalue);
        if(u32Sram&0x1)
        {
            u16tvalue = MST_SRAM1_Main[((u32Sram&0xF0)>>4)][u16tcount+4];
        }
        else
        {
            u16tvalue = MST_SRAM2_Main[((u32Sram&0xF0)>>4)][u16tcount+4];
        }
        HalUtilityW2BYTEMSK(u32reg+8, u16tvalue, 0xFF);
        SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[HVSP]Ytval5:%hx\n", u16tvalue);
        HalUtilityW2BYTEMSK(u32reg, BIT8, BIT8);//W pulse
        //if(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG)
        {
            HalUtilityW2BYTEMSK(u32reg, BIT9, BIT9);//R pulse
            u16readdata = HalUtilityR2BYTEDirect(u32reg+10);
            SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[HVSP]read Yval 12:%hx\n", u16readdata);
            u16readdata = HalUtilityR2BYTEDirect(u32reg+12);
            SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[HVSP]read Yval 34:%hx\n", u16readdata);
            u16readdata = HalUtilityR2BYTEDirect(u32reg+14);
            SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[HVSP]read Yval 5:%hx\n", u16readdata);
        }
        HalUtilityW2BYTEMSK(u32reg, 0, BIT0);//Yenable

        HalUtilityW2BYTEMSK(u32reg, BIT1, BIT1);//Cenable
        u16tvalue = MST_C_SRAM1_Main[0][u16tcount] | (MST_C_SRAM1_Main[0][u16tcount+1]<<8);
        SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[HVSP]Ctval12:%hx\n", u16tvalue);
        HalUtilityW2BYTEMSK(u32reg+4, u16tvalue, 0xFFFF);
        u16tvalue = MST_C_SRAM1_Main[0][u16tcount+2] | (MST_C_SRAM1_Main[0][u16tcount+3]<<8);
        SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[HVSP]Ctval34:%hx\n", u16tvalue);
        HalUtilityW2BYTEMSK(u32reg+6, u16tvalue, 0xFFFF);
        u16tvalue = MST_C_SRAM1_Main[0][u16tcount+4];
        SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[HVSP]Ctval5:%hx\n", u16tvalue);
        HalUtilityW2BYTEMSK(u32reg+8, u16tvalue, 0xFF);
        HalUtilityW2BYTEMSK(u32reg, BIT8, BIT8);//W pulse

        //if(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG)
        {
            HalUtilityW2BYTEMSK(u32reg, BIT9, BIT9);//R pulse
            u16readdata = HalUtilityR2BYTEDirect(u32reg+10);
            SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[HVSP]read Cval 12:%hx\n", u16readdata);
            u16readdata = HalUtilityR2BYTEDirect(u32reg+12);
            SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[HVSP]read Cval 34:%hx\n", u16readdata);
            u16readdata = HalUtilityR2BYTEDirect(u32reg+14);
            SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[HVSP]read Cval 5:%hx\n", u16readdata);
        }
        HalUtilityW2BYTEMSK(u32reg, 0, BIT1);//Cenable
        u16tcount += 5;
    }
    //SRAM 1 //128~256 entry  link to 0~127 , so do not use
    u16tcount = 0;
}
u16 _HalSclVipGetWdrTvalue(u8 u8sec, u16 u16tcount, u32 u32Sram)
{
    u16 u16tvalue;
    u8 *p8buffer;
    u32 u32Val = 0;
    if(u32Sram)
    {
        p8buffer= (u8 *)u32Sram;
        u32Val = WDR_SRAM_USERBYTENUM*2 *u8sec ;
        u16tvalue = (u16)(*(p8buffer+u32Val+u16tcount) | (*(p8buffer+u32Val+u16tcount+1)<<8));
        SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[WDRTvalue]val:%ld\n",u32Val+u16tcount);
    }
    else
    {
        switch(u8sec)
        {
            case 0:
                u16tvalue = MST_WDR_Loc_TBL_0_SRAM_Main[0][u16tcount] | (MST_WDR_Loc_TBL_0_SRAM_Main[0][u16tcount+1]<<8);
                break;
            case 1:
                u16tvalue = MST_WDR_Loc_TBL_1_SRAM_Main[0][u16tcount] | (MST_WDR_Loc_TBL_1_SRAM_Main[0][u16tcount+1]<<8);
                break;
            case 2:
                u16tvalue = MST_WDR_Loc_TBL_2_SRAM_Main[0][u16tcount] | (MST_WDR_Loc_TBL_2_SRAM_Main[0][u16tcount+1]<<8);
                break;
            case 3:
                u16tvalue = MST_WDR_Loc_TBL_3_SRAM_Main[0][u16tcount] | (MST_WDR_Loc_TBL_3_SRAM_Main[0][u16tcount+1]<<8);
                break;
            case 4:
                u16tvalue = MST_WDR_Loc_TBL_4_SRAM_Main[0][u16tcount] | (MST_WDR_Loc_TBL_4_SRAM_Main[0][u16tcount+1]<<8);
                break;
            case 5:
                u16tvalue = MST_WDR_Loc_TBL_5_SRAM_Main[0][u16tcount] | (MST_WDR_Loc_TBL_5_SRAM_Main[0][u16tcount+1]<<8);
                break;
            case 6:
                u16tvalue = MST_WDR_Loc_TBL_6_SRAM_Main[0][u16tcount] | (MST_WDR_Loc_TBL_6_SRAM_Main[0][u16tcount+1]<<8);
                break;
            case 7:
                u16tvalue = MST_WDR_Loc_TBL_7_SRAM_Main[0][u16tcount] | (MST_WDR_Loc_TBL_7_SRAM_Main[0][u16tcount+1]<<8);
                break;
            default:
                u16tvalue = 0;
                break;
        }
    }
    return u16tvalue;
}
void _HalSclVipSetWriteRegType(bool bEn)
{
    if(bEn && (VIPSETRULE()==E_DRV_SCLOS_VIPSETRUle_CMDQALL || VIPSETRULE()==E_DRV_SCLOS_VIPSETRUle_CMDQALLCheck
        ||(VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQALLONLYSRAMCheck)))
    {
        DrvSclCmdqGetModuleMutex(E_DRV_SCLCMDQ_TYPE_IP0,1);
        if(DrvSclVipGetEachDmaEn())
        {
            gbCMDQ = 1;
        }
        else
        {
            gbCMDQ = 0;
        }
    }
    else if(bEn && VIPSETRULE())
    {
        DrvSclCmdqGetModuleMutex(E_DRV_SCLCMDQ_TYPE_IP0,1);
        if(DrvSclVipGetEachDmaEn()&& !DrvSclVipGetIsBlankingRegion())
        {
            gbCMDQ = 1;
        }
        else
        {
            gbCMDQ = 0;
        }
    }
    else
    {
        gbCMDQ = 0;
        DrvSclCmdqGetModuleMutex(E_DRV_SCLCMDQ_TYPE_IP0,0);
    }
}
void _HalSclVipWriteReg(u32 u32Reg,u16 u16Val,u16 u16Mask)
{
    DrvSclCmdqCmdReg_t stCfg;
    if(gbCMDQ)
    {
        DrvSclCmdqFillCmd(&stCfg, u32Reg, u16Val, u16Mask);
        DrvSclCmdqWriteSramCmd(E_DRV_SCLCMDQ_TYPE_IP0, stCfg, 1);
    }
    else
    {
        HalUtilityW2BYTEMSK(u32Reg, u16Val, u16Mask);//sec
    }
}
bool _HalSclVipCheckMonotonicallyIncreasing
    (u16 u16tvalueeven,u16 u16tvalueodd,u16 u16chkodd)
{
    if((u16tvalueodd < u16tvalueeven)||(u16tvalueeven < u16chkodd))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
void _HalSclVipSramDumpWdr(u32 u32reg, u32 u32Sram)
{
    u8  u8sec = 0;
    u16 u16addr = 0,u16tvalue = 0,u16tcount = 0;
    u16 u16addrmax;
    if(u32Sram==0)
    {
        u16addrmax = WDR_SRAM_BYTENUM;
    }
    else
    {
        u16addrmax = WDR_SRAM_USERBYTENUM;
    }
    HAL_SCLVIP_MUTEX_LOCK();
    _HalSclVipSetWriteRegType(1);
    DrvSclCmdqSramReset();
    for(u8sec=0;u8sec<WDR_SRAM_NUM;u8sec++)
    {
        if(!DrvSclVipGetIsBlankingRegion() && !gbCMDQ)
        {
            _HalSclVipSetWriteRegType(0);
            _HalSclVipSetWriteRegType(1);
            SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISR, "bCMDQ Open WDR\n");
        }
        SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "WDR:%hhx\n", u8sec);
        _HalSclVipWriteReg(u32reg, u8sec, 0xF);//sec
        u16tcount = 0;
        for(u16addr=0;u16addr<u16addrmax;u16addr++)
        {
            u16tvalue = _HalSclVipGetWdrTvalue(u8sec, u16tcount,u32Sram);
            //_HalSclVipWriteReg(u32reg+4, u16addr, 0x007F);//addr
            _HalSclVipWriteReg(u32reg+2, (u16)u16tvalue, 0x3FF);//data
            _HalSclVipWriteReg(u32reg+4, u16addr|BIT8|BIT9, 0xFFFF);//wen
            SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "tval:%hx addr:%hd\n",u16tvalue,u16addr);
            //for i3e can't read back
            //_HalSclVipWriteReg(u32reg+4, u16addr|BIT9, 0xFFFF);//ren
            //u16readdata=HalUtilityR2BYTEDirect(u32reg+6);
            //HAL_SCLVIP_DBG(sclprintf("reg tval:%hx\n",u16readdata));
            u16tcount+=2;
        }
    }
    if(gbCMDQ)
    {
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISR, "gbCMDQ WDR \n");
        DrvSclCmdqFireSram(E_DRV_SCLCMDQ_TYPE_IP0,1);
    }
    _HalSclVipSetWriteRegType(0);
    HAL_SCLVIP_MUTEX_UNLOCK();
}
u16 _HalSclVipGetGammaYuvTvalue(u16 u16sec, u16 u16tcount,u32 u32Sram)
{
    u16 u16tvalue;
    u8 *p8buffer;
    if(u32Sram)
    {
        p8buffer= (u8 *)u32Sram;
        u16tvalue = (u16)(*(p8buffer+u16tcount) | (*(p8buffer+u16tcount+1)<<8));
    }
    else
    {
        switch(u16sec)
        {
            case 0:
                u16tvalue = MST_YUV_Gamma_tblY_SRAM_Main[0][u16tcount] | (MST_YUV_Gamma_tblY_SRAM_Main[0][u16tcount+1]<<8);
                break;
            case 2:
                u16tvalue = MST_YUV_Gamma_tblU_SRAM_Main[0][u16tcount] | (MST_YUV_Gamma_tblU_SRAM_Main[0][u16tcount+1]<<8);
                break;
            case 4:
                u16tvalue = MST_YUV_Gamma_tblV_SRAM_Main[0][u16tcount] | (MST_YUV_Gamma_tblV_SRAM_Main[0][u16tcount+1]<<8);
                break;
            default:
                u16tvalue = 0;
                break;
        }
    }
    return u16tvalue;
}

bool _HalSclVipSramDumpGammaYuv(HalSclVipSramDumpType_e endump,u32 u32Sram,u32 u32reg)
{
    u16  u16sec = 0;
    u8  bRet = 1;
    u16  u16chkodd = 0;
    u16 u16addr = 0,u16tvalueodd = 0,u16tvalueeven = 0,u16tcount = 0,u16readdata,u16RegMask,u16size;
    u32 u32rega, u32regd ,u32Events= 0;
    switch(endump)
    {
        case E_HAL_SCLVIP_SRAM_DUMP_GAMMA_Y:
            u16RegMask = 0x3;
            u32rega = u32reg+2;
            u32regd = u32reg+8;
            u16size = GAMMAY_SRAM_BYTENUM;
            u16sec = 0;
            break;
        case E_HAL_SCLVIP_SRAM_DUMP_GAMMA_U:
            u16RegMask = 0xC;
            u32rega = u32reg+4;
            u32regd = u32reg+12;
            u16size = GAMMAU_SRAM_BYTENUM;
            u16sec = 2;
            break;
        case E_HAL_SCLVIP_SRAM_DUMP_GAMMA_V:
            u16RegMask = 0x30;
            u32rega = u32reg+6;
            u32regd = u32reg+16;
            u16size = GAMMAV_SRAM_BYTENUM;
            u16sec = 4;
            break;
        default:
            u16RegMask = 0;
            u32rega = 0;
            u32regd = 0;
            u16size = 0;
            break;
    }
    SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "YUVGamma:%hhx\n", u16sec);
    HAL_SCLVIP_MUTEX_LOCK();
    DrvSclCmdqSramReset();
    _HalSclVipSetWriteRegType(1);
    for(u16addr=0;u16addr<u16size;u16addr++)
    {
        if(!DrvSclVipGetIsBlankingRegion() && !gbCMDQ)
        {
            _HalSclVipSetWriteRegType(0);
            _HalSclVipSetWriteRegType(1);
            if(VIPSETRULE())
            {
            SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISR, "bCMDQ Open YUVGamma:%hhx\n",u16sec);
            }
            else
            {
                _HalSclVipSetWriteRegType(0);
                HAL_SCLVIP_MUTEX_UNLOCK();
                return 0;
            }
        }
        u16tvalueeven = _HalSclVipGetGammaYuvTvalue(u16sec, u16tcount,u32Sram);
        u16tvalueodd = _HalSclVipGetGammaYuvTvalue(u16sec, u16tcount+2,u32Sram);
        if(bRet)
        {
            bRet = _HalSclVipCheckMonotonicallyIncreasing(u16tvalueeven,u16tvalueodd,u16chkodd);
            if(!bRet)
            {
                //SCL_ERR("[HALVIP]YUVGamma:%hhx @:%hx NOT Monotonically Increasing  (%hx,%hx,%hx)\n"
                //    ,u16sec,u16addr,u16chkodd,u16tvalueeven,u16tvalueodd);
            }
        }
        u16chkodd = u16tvalueodd;
        _HalSclVipWriteReg(u32rega, (((u16addr)<<8)|u16addr), 0x7F7F);//addr
        _HalSclVipWriteReg(u32regd, (u16)u16tvalueeven, 0xFFF);//data
        _HalSclVipWriteReg(u32regd+2, (u16)u16tvalueodd, 0xFFF);//data
        _HalSclVipWriteReg(u32reg, u16RegMask, u16RegMask);//wen
        //if(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG)
        {
            _HalSclVipWriteReg(u32reg-2, (u16sec<<3)|BIT2, BIT2|BIT3|BIT4|BIT5);//ridx //ren bit2
            //_HalSclVipWriteReg(u32reg-2,BIT2 ,BIT2 );
            SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "even val:%hx\n", u16tvalueeven);
            u16readdata=HalUtilityR2BYTEDirect(u32reg+28);
            SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "read val:%hx\n", u16readdata);
            if(!gbCMDQ)
            {
                while(u16readdata!=u16tvalueeven)
                {
                    SCL_DBGERR("[HALVIP]EVEN YUVGamma:%hhx\n",u16sec);
                    SCL_DBGERR( "[HALVIP]EVEN val:%hx\n",u16tvalueodd);
                    SCL_DBGERR( "[HALVIP]read val:%hx\n",u16readdata);
                    if(!DrvSclVipGetIsBlankingRegion())
                    {
                        DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMENDSYNC, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
                    }
                    _HalSclVipWriteReg(u32rega, (((u16addr)<<8)|u16addr), 0x7F7F);//addr
                    _HalSclVipWriteReg(u32regd, (u16)u16tvalueeven, 0xFFF);//data
                    _HalSclVipWriteReg(u32regd+2, (u16)u16tvalueodd, 0xFFF);//data
                    _HalSclVipWriteReg(u32reg, u16RegMask, u16RegMask);//wen
                    _HalSclVipWriteReg(u32reg-2, (((u16sec+1)<<3)|(u16sec<<3)|BIT2), BIT2|BIT3|BIT4|BIT5);//ridx //ren bit2
                    u16readdata=HalUtilityR2BYTEDirect(u32reg+28);
                    bRet++;
                    if(bRet>10)
                    {
                        bRet = 0;
                        break;
                    }
                }
            }
            _HalSclVipWriteReg(u32reg-2, ((u16sec+1)<<3)|BIT2, BIT2|BIT3|BIT4|BIT5);//ridx
            //_HalSclVipWriteReg(u32reg-2, BIT2, BIT2);//ren
            SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "odd val:%hx\n", u16tvalueodd);
            u16readdata=HalUtilityR2BYTEDirect(u32reg+28);
            SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "read val:%hx\n", u16readdata);
            if(!gbCMDQ)
            {
                while(u16readdata!=u16tvalueodd)
                {
                    SCL_DBGERR("[HALVIP]ODD YUVGamma:%hhx\n",u16sec);
                    SCL_DBGERR( "[HALVIP]odd val:%hx\n",u16tvalueodd);
                    SCL_DBGERR( "[HALVIP]read val:%hx\n",u16readdata);
                    if(!DrvSclVipGetIsBlankingRegion())
                    {
                        DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMENDSYNC, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
                    }
                    _HalSclVipWriteReg(u32rega, (((u16addr)<<8)|u16addr), 0x7F7F);//addr
                    _HalSclVipWriteReg(u32regd+2, (u16)u16tvalueodd, 0xFFF);//data
                    _HalSclVipWriteReg(u32regd, (u16)u16tvalueeven, 0xFFF);//data
                    _HalSclVipWriteReg(u32reg, u16RegMask, u16RegMask);//wen
                    _HalSclVipWriteReg(u32reg-2, (((u16sec+1)<<3)|(u16sec<<3)|BIT2), BIT2|BIT3|BIT4|BIT5);//ridx
                    u16readdata=HalUtilityR2BYTEDirect(u32reg+28);
                    bRet++;
                    if(bRet>10)
                    {
                        bRet = 0;
                        break;
                    }
                }
            }
        }
        u16tcount+=4;
    }
    if(gbCMDQ)
    {
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISR, "gbCMDQ YUVGamma:%hhx\n",u16sec);
        DrvSclCmdqFireSram(E_DRV_SCLCMDQ_TYPE_IP0,1);
        switch(endump)
        {
            case E_HAL_SCLVIP_SRAM_DUMP_GAMMA_Y:
                DrvSclOsSetEventIrq(DrvSclIrqGetIrqSYNCEventID(),  E_DRV_SCLIRQ_EVENT_GAMMA_Y);
                break;
            case E_HAL_SCLVIP_SRAM_DUMP_GAMMA_U:
                DrvSclOsSetEventIrq(DrvSclIrqGetIrqSYNCEventID(),  E_DRV_SCLIRQ_EVENT_GAMMA_U);
                break;
            case E_HAL_SCLVIP_SRAM_DUMP_GAMMA_V:
                DrvSclOsSetEventIrq(DrvSclIrqGetIrqSYNCEventID(),  E_DRV_SCLIRQ_EVENT_GAMMA_V);
                break;
            default:
                break;
        }
    }
    _HalSclVipSetWriteRegType(0);
    HAL_SCLVIP_MUTEX_UNLOCK();
    return bRet;
}
bool HalSclVipGetSramDumpGammaYUVCallback(HalSclVipSramDumpType_e endump,u32 u32Sram,u32 u32reg)
{
    u16  u16sec = 0;
    u8 flag = 0;
    u16 u16addr = 0,u16tcount = 0,u16readdata = 0,u16readdataodd= 0,u16tvalueodd = 0,u16tvalueeven = 0,u16RegMask,u16size;
    u32 u32rega, u32regd;
    switch(endump)
    {
        case E_HAL_SCLVIP_SRAM_DUMP_GAMMA_Y:
            u16RegMask = 0x3;
            u32rega = u32reg+2;
            u32regd = u32reg+8;
            u16size = GAMMAY_SRAM_BYTENUM;
            u16sec = 0;
            break;
        case E_HAL_SCLVIP_SRAM_DUMP_GAMMA_U:
            u16RegMask = 0xC;
            u32rega = u32reg+4;
            u32regd = u32reg+12;
            u16size = GAMMAU_SRAM_BYTENUM;
            u16sec = 2;
            break;
        case E_HAL_SCLVIP_SRAM_DUMP_GAMMA_V:
            u16RegMask = 0x30;
            u32rega = u32reg+6;
            u32regd = u32reg+16;
            u16size = GAMMAV_SRAM_BYTENUM;
            u16sec = 4;
            break;
        default:
            u16RegMask = 0;
            u32rega = 0;
            u32regd = 0;
            u16size = 0;
            break;
    }
    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISR, "YUVGamma:%hhx\n", u16sec);
    HAL_SCLVIP_MUTEX_LOCK();
    DrvSclCmdqSramReset();
    for(u16addr=0;u16addr<u16size;u16addr++)
    {
        if(!DrvSclVipGetIsBlankingRegion())
        {
            HAL_SCLVIP_MUTEX_UNLOCK();
            return 0;
        }
        u16tvalueeven = _HalSclVipGetGammaYuvTvalue(u16sec, u16tcount,u32Sram);
        u16tvalueodd = _HalSclVipGetGammaYuvTvalue(u16sec, u16tcount+2,u32Sram);
        _HalSclVipWriteReg(u32rega, (((u16addr)<<8)|u16addr), 0x7F7F);//addr
        _HalSclVipWriteReg(u32reg-2, (u16sec<<3)|BIT2, BIT2|BIT3|BIT4|BIT5);//ridx //ren bit2
        u16readdata=HalUtilityR2BYTEDirect(u32reg+28);
        _HalSclVipWriteReg(u32reg-2, ((u16sec+1)<<3)|BIT2, BIT2|BIT3|BIT4|BIT5);//ridx //ren bit2
        u16readdataodd=HalUtilityR2BYTEDirect(u32reg+28);
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_SRAMCheck, "YUVGamma:%hhx", u16sec);
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_SRAMCheck, "addr:%hx even val:%hx even read val:%hx\n",u16addr,u16tvalueeven, u16readdata);
        if(u16readdata != u16tvalueeven || u16readdataodd != u16tvalueodd)
        {
            _HalSclVipSetWriteRegType(1);
            flag = gbCMDQ;
            if(DrvSclVipGetIsBlankingRegion())
            {
                SCL_DBGERR("YUVGamma:%hhx", u16sec);
                SCL_DBGERR("addr:%hx even val:%hx even read val:%hx\n",u16addr,u16tvalueeven, u16readdata);
                SCL_DBGERR("addr:%hx odd val:%hx odd read val:%hx\n",u16addr,u16tvalueodd, u16readdata);
            }
            _HalSclVipWriteReg(u32rega, (((u16addr)<<8)|u16addr), 0x7F7F);//addr
            _HalSclVipWriteReg(u32regd, (u16)u16tvalueeven, 0xFFF);//data
            _HalSclVipWriteReg(u32regd+2, (u16)u16tvalueodd, 0xFFF);//data
            _HalSclVipWriteReg(u32reg, u16RegMask, u16RegMask);//wen
            _HalSclVipWriteReg(u32reg-2, (((u16sec+1)<<3)|(u16sec<<3)|BIT2), BIT2|BIT3|BIT4|BIT5);//ridx
            _HalSclVipSetWriteRegType(0);
        }
        u16tcount+=4;
    }
    if(flag)
    {
        DrvSclCmdqFireSram(E_DRV_SCLCMDQ_TYPE_IP0,1);
    }
    HAL_SCLVIP_MUTEX_UNLOCK();
    return 1;
}
bool HalSclVipGetSramDumpGammaYUV(HalSclVipSramDumpType_e endump,u32 u32Sram,u32 u32reg)
{
    u16  u16sec = 0;
    u8 flag = 0;
    u16 u16addr = 0,u16tcount = 0,u16readdata = 0,u16readdataodd = 0,u16tvalueodd = 0,u16tvalueeven = 0,u16RegMask,u16size;
    u32 u32rega, u32regd;
    switch(endump)
    {
        case E_HAL_SCLVIP_SRAM_DUMP_GAMMA_Y:
            u16RegMask = 0x3;
            u32rega = u32reg+2;
            u32regd = u32reg+8;
            u16size = GAMMAY_SRAM_BYTENUM;
            u16sec = 0;
            break;
        case E_HAL_SCLVIP_SRAM_DUMP_GAMMA_U:
            u16RegMask = 0xC;
            u32rega = u32reg+4;
            u32regd = u32reg+12;
            u16size = GAMMAU_SRAM_BYTENUM;
            u16sec = 2;
            break;
        case E_HAL_SCLVIP_SRAM_DUMP_GAMMA_V:
            u16RegMask = 0x30;
            u32rega = u32reg+6;
            u32regd = u32reg+16;
            u16size = GAMMAV_SRAM_BYTENUM;
            u16sec = 4;
            break;
        default:
            u16RegMask = 0;
            u32rega = 0;
            u32regd = 0;
            u16size = 0;
            break;
    }
    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISR, "YUVGamma:%hhx\n", u16sec);
    HAL_SCLVIP_MUTEX_LOCK();
    DrvSclCmdqSramReset();
    for(u16addr=0;u16addr<u16size;u16addr++)
    {
        if(!DrvSclVipGetIsBlankingRegion())
        {
            HAL_SCLVIP_MUTEX_UNLOCK();
            return 0;
        }
        u16tvalueeven = _HalSclVipGetGammaYuvTvalue(u16sec, u16tcount,u32Sram);
        u16tvalueodd = _HalSclVipGetGammaYuvTvalue(u16sec, u16tcount+2,u32Sram);
        _HalSclVipWriteReg(u32rega, (((u16addr)<<8)|u16addr), 0x7F7F);//addr
        _HalSclVipWriteReg(u32reg-2, (u16sec<<3)|BIT2, BIT2|BIT3|BIT4|BIT5);//ridx //ren bit2
        u16readdata=HalUtilityR2BYTEDirect(u32reg+28);
        _HalSclVipWriteReg(u32reg-2, ((u16sec+1)<<3)|BIT2, BIT2|BIT3|BIT4|BIT5);//ridx //ren bit2
        u16readdataodd=HalUtilityR2BYTEDirect(u32reg+28);
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_SRAMCheck, "YUVGamma:%hhx", u16sec);
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_SRAMCheck, "addr:%hx even val:%hx even read val:%hx\n",u16addr,u16tvalueeven, u16readdata);
        if(u16readdata != u16tvalueeven || u16readdataodd != u16tvalueodd)
        {
            _HalSclVipSetWriteRegType(1);
            flag = gbCMDQ;
            if(DrvSclVipGetIsBlankingRegion())
            {
                SCL_DBGERR("YUVGamma:%hhx", u16sec);
                SCL_DBGERR("addr:%hx even val:%hx even read val:%hx\n",u16addr,u16tvalueeven, u16readdata);
                SCL_DBGERR("addr:%hx odd val:%hx odd read val:%hx\n",u16addr,u16tvalueodd, u16readdata);
            }
            _HalSclVipWriteReg(u32rega, (((u16addr)<<8)|u16addr), 0x7F7F);//addr
            _HalSclVipWriteReg(u32regd, (u16)u16tvalueeven, 0xFFF);//data
            _HalSclVipWriteReg(u32regd+2, (u16)u16tvalueodd, 0xFFF);//data
            _HalSclVipWriteReg(u32reg, u16RegMask, u16RegMask);//wen
            _HalSclVipWriteReg(u32reg-2, (((u16sec+1)<<3)|(u16sec<<3)|BIT2), BIT2|BIT3|BIT4|BIT5);//ridx
            _HalSclVipSetWriteRegType(0);
        }
        u16tcount+=4;
    }
    if(flag)
    {
        DrvSclCmdqFireSram(E_DRV_SCLCMDQ_TYPE_IP0,1);
    }
    HAL_SCLVIP_MUTEX_UNLOCK();
    return 1;
}
u16 Hal_VIP_GetGammaRGBTvalue(u16 u16sec, u16 u16tcount,u32 u32Sram,u8  u8type)
{
    u16 u16tvalue;
    u8 *p8buffer;
    if(u32Sram)
    {
        p8buffer= (u8 *)u32Sram;
        u16tvalue = (u16)(*(p8buffer+u16tcount) | (*(p8buffer+u16tcount+1)<<8));
    }
    else
    {
        switch(u16sec+u8type)
        {
            case 0:
                u16tvalue = MST_ColorEng_GM10to12_Tbl_R_SRAM_Main[0][u16tcount] | (MST_ColorEng_GM10to12_Tbl_R_SRAM_Main[0][u16tcount+1]<<8);
                break;
            case 2:
                u16tvalue = MST_ColorEng_GM10to12_Tbl_G_SRAM_Main[0][u16tcount] | (MST_ColorEng_GM10to12_Tbl_G_SRAM_Main[0][u16tcount+1]<<8);
                break;
            case 4:
                u16tvalue = MST_ColorEng_GM10to12_Tbl_B_SRAM_Main[0][u16tcount] | (MST_ColorEng_GM10to12_Tbl_B_SRAM_Main[0][u16tcount+1]<<8);
                break;
            case 1:
                u16tvalue = MST_ColorEng_GM12to10_CrcTbl_R_SRAM_Main[0][u16tcount] | (MST_ColorEng_GM12to10_CrcTbl_R_SRAM_Main[0][u16tcount+1]<<8);
                break;
            case 3:
                u16tvalue = MST_ColorEng_GM12to10_CrcTbl_G_SRAM_Main[0][u16tcount] | (MST_ColorEng_GM12to10_CrcTbl_G_SRAM_Main[0][u16tcount+1]<<8);
                break;
            case 5:
                u16tvalue = MST_ColorEng_GM12to10_CrcTbl_B_SRAM_Main[0][u16tcount] | (MST_ColorEng_GM12to10_CrcTbl_B_SRAM_Main[0][u16tcount+1]<<8);
                break;
            default:
                u16tvalue = 0;
                break;
        }
    }
    return u16tvalue;
}

u8 Hal_VIP_SRAMDumpGammaRGB(HalSclVipSramDumpType_e endump,u32 u32Sram,u32 u32reg)
{
    u16  u16sec = 0;
    u8  u8type = 0;
    u8  bRet = 1;
    u16 u16addr = 0,u16tvalueodd = 0,u16tvalueeven = 0,u16tcount = 0,u16readdata,u16size;
    u16 u16chkodd = 0;
    u32 u32Events = 0;
    switch(endump)
    {
        case E_HAL_SCLVIP_SRAM_DUMP_GM10to12_R:
            u16size = GAMMA10to12R_SRAM_BYTENUM;
            u16sec = 0;
            u8type = 0;
            SCL_DBG(SCL_DBG_LV_IOCTL()&EN_DBGMG_IOCTLEVEL_VIP, "GM10to12_R:%lx\n",u32reg);
            break;
        case E_HAL_SCLVIP_SRAM_DUMP_GM10to12_G:
            u16size = GAMMA10to12G_SRAM_BYTENUM;
            u16sec = 2;
            u8type = 0;
            SCL_DBG(SCL_DBG_LV_IOCTL()&EN_DBGMG_IOCTLEVEL_VIP, "GM10to12_G:%lx\n",u32reg);
            break;
        case E_HAL_SCLVIP_SRAM_DUMP_GM10to12_B:
            u16size = GAMMA10to12B_SRAM_BYTENUM;
            u16sec = 4;
            u8type = 0;
            SCL_DBG(SCL_DBG_LV_IOCTL()&EN_DBGMG_IOCTLEVEL_VIP, "GM10to12_B:%lx\n",u32reg);
            break;
        case E_HAL_SCLVIP_SRAM_DUMP_GM12to10_R:
            u16size = GAMMA12to10R_SRAM_BYTENUM;
            u16sec = 0;
            u8type = 1;
            SCL_DBG(SCL_DBG_LV_IOCTL()&EN_DBGMG_IOCTLEVEL_VIP, "GM12to10R:%lx\n",u32reg);
            break;
        case E_HAL_SCLVIP_SRAM_DUMP_GM12to10_G:
            u16size = GAMMA12to10G_SRAM_BYTENUM;
            u16sec = 2;
            u8type = 1;
            SCL_DBG(SCL_DBG_LV_IOCTL()&EN_DBGMG_IOCTLEVEL_VIP, "GM12to10G:%lx\n",u32reg);
            break;
        case E_HAL_SCLVIP_SRAM_DUMP_GM12to10_B:
            u16size = GAMMA12to10B_SRAM_BYTENUM;
            u16sec = 4;
            u8type = 1;
            SCL_DBG(SCL_DBG_LV_IOCTL()&EN_DBGMG_IOCTLEVEL_VIP, "GM12to10B:%lx\n",u32reg);
            break;
        default:
            u16size = 0;
            u16sec = 0;
            u8type = 0;
            break;
    }
    SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "RGBGamma:%hhx\n",u16sec+u8type);
    HAL_SCLVIP_MUTEX_LOCK();
    DrvSclCmdqSramReset();
    _HalSclVipSetWriteRegType(1);
    for(u16addr=0;u16addr<u16size;u16addr++)
    {
        if(!DrvSclVipGetIsBlankingRegion() && !gbCMDQ)
        {
            _HalSclVipSetWriteRegType(0);
            _HalSclVipSetWriteRegType(1);
            if(VIPSETRULE())
            {
                SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISR, "bCMDQ Open RGBGamma:%hhx\n",u16sec+u8type);
            }
            else
            {
                _HalSclVipSetWriteRegType(0);
                HAL_SCLVIP_MUTEX_UNLOCK();
                return 0;
            }
        }
        u16tvalueeven = Hal_VIP_GetGammaRGBTvalue(u16sec, u16tcount,u32Sram,u8type);
        u16tvalueodd = Hal_VIP_GetGammaRGBTvalue(u16sec, u16tcount+2,u32Sram,u8type);
        if(bRet)
        {
            bRet = _HalSclVipCheckMonotonicallyIncreasing(u16tvalueeven,u16tvalueodd,u16chkodd);
            if(!bRet)
            {
                //SCL_ERR("[HALVIP]RGBGamma:%hhx @:%hx NOT Monotonically Increasing  (%hx,%hx,%hx)\n"
                //    ,u16sec+u8type,u16addr,u16chkodd,u16tvalueeven,u16tvalueodd);
            }
        }
        u16chkodd = u16tvalueodd;
        //_HalSclVipWriteReg(u32reg, u16addr, 0x007F);//addr
        //_HalSclVipWriteReg(u32reg, u16sec<<8, BIT8|BIT9|BIT10);//sec
        _HalSclVipWriteReg(u32reg+2, (u16)u16tvalueeven, 0xFFF);//data
        _HalSclVipWriteReg(u32reg, ((u16addr)|((u16)u16sec<<8)|BIT11), 0xFFFF);//wen
        _HalSclVipWriteReg(u32reg, ((u16addr)|((u16)u16sec<<8)|BIT12), 0xFFFF);//ren
        //if(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG)
        {
            //_HalSclVipWriteReg(u32reg, BIT12, BIT12);//ren
            SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "even val:%hx\n",u16tvalueeven);
            u16readdata=HalUtilityR2BYTEDirect(u32reg+4);
            SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "read val:%hx\n",u16readdata);
        }

        if(!gbCMDQ)
        {
            while(u16readdata!=u16tvalueeven)
            {
                SCL_DBGERR("[HALVIP]EvenRGBGamma:%hhx\n",u16sec+u8type);
                SCL_DBGERR( "[HALVIP]even val:%hx\n",u16tvalueeven);
                SCL_DBGERR( "[HALVIP]read val:%hx\n",u16readdata);
                if(!DrvSclVipGetIsBlankingRegion())
                {
                    DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMENDSYNC, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
                }
                _HalSclVipWriteReg(u32reg+2, (u16)u16tvalueeven, 0xFFF);//data
                _HalSclVipWriteReg(u32reg, ((u16addr)|((u16)u16sec<<8)|BIT11), 0xFFFF);//wen
                _HalSclVipWriteReg(u32reg, ((u16addr)|((u16)u16sec<<8)|BIT12), 0xFFFF);//ren
                u16readdata=HalUtilityR2BYTEDirect(u32reg+4);
                bRet++;
                if(bRet>10)
                {
                    bRet = 0;
                    break;
                }
            }
        }
        //Hal_VIP_WriteReg(u32reg, u16addr, 0x007F);//addr
        //Hal_VIP_WriteReg(u32reg, (u16sec+1)<<8, BIT8|BIT9|BIT10);//sec
        _HalSclVipWriteReg(u32reg+2, (u16)u16tvalueodd, 0xFFF);//data
        //Hal_VIP_WriteReg(u32reg, BIT11, BIT11);//wen
        _HalSclVipWriteReg(u32reg, ((u16addr)|(((u16)u16sec+1)<<8)|BIT11), 0xFFFF);//wen
        _HalSclVipWriteReg(u32reg, ((u16addr)|(((u16)u16sec+1)<<8)|BIT12), 0xFFFF);//ren
        //if(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG)
        {
            //_HalSclVipWriteReg(u32reg, BIT12, BIT12);//ren
            SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "odd val:%hx\n",u16tvalueodd);
            u16readdata=HalUtilityR2BYTEDirect(u32reg+4);
            SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "read val:%hx\n",u16readdata);
        }
        if(!gbCMDQ)
        {
            while(u16readdata!=u16tvalueodd)
            {
                SCL_DBGERR("[HALVIP]odd RGBGamma:%hhx\n",u16sec+u8type);
                SCL_DBGERR( "[HALVIP]odd val:%hx\n",u16tvalueodd);
                SCL_DBGERR( "[HALVIP]read val:%hx\n",u16readdata);
                if(!DrvSclVipGetIsBlankingRegion())
                {
                    DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMENDSYNC, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
                }
                _HalSclVipWriteReg(u32reg+2, (u16)u16tvalueodd, 0xFFF);//data
                _HalSclVipWriteReg(u32reg, ((u16addr)|(((u16)u16sec+1)<<8)|BIT11), 0xFFFF);//wen
                _HalSclVipWriteReg(u32reg, ((u16addr)|(((u16)u16sec+1)<<8)|BIT12), 0xFFFF);//ren
                u16readdata=HalUtilityR2BYTEDirect(u32reg+4);
                bRet++;
                if(bRet>10)
                {
                    bRet = 0;
                    break;
                }
            }
        }
        u16tcount+=4;
    }
    if(gbCMDQ)
    {
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISR, "gbCMDQ RGBGamma:%hhx\n",u16sec+u8type);
        DrvSclCmdqFireSram(E_DRV_SCLCMDQ_TYPE_IP0,1);
        switch(endump)
        {
            case E_HAL_SCLVIP_SRAM_DUMP_GM10to12_R:
                DrvSclOsSetEventIrq(DrvSclIrqGetIrqSYNCEventID(),  E_DRV_SCLIRQ_EVENT_GM10to12_R);
                break;
            case E_HAL_SCLVIP_SRAM_DUMP_GM10to12_G:
                DrvSclOsSetEventIrq(DrvSclIrqGetIrqSYNCEventID(),  E_DRV_SCLIRQ_EVENT_GM10to12_G);
                break;
            case E_HAL_SCLVIP_SRAM_DUMP_GM10to12_B:
                DrvSclOsSetEventIrq(DrvSclIrqGetIrqSYNCEventID(),  E_DRV_SCLIRQ_EVENT_GM10to12_B);
                break;
            case E_HAL_SCLVIP_SRAM_DUMP_GM12to10_R:
                DrvSclOsSetEventIrq(DrvSclIrqGetIrqSYNCEventID(),  E_DRV_SCLIRQ_EVENT_GM12to10_R);
                break;
            case E_HAL_SCLVIP_SRAM_DUMP_GM12to10_G:
                DrvSclOsSetEventIrq(DrvSclIrqGetIrqSYNCEventID(),  E_DRV_SCLIRQ_EVENT_GM12to10_G);
                break;
            case E_HAL_SCLVIP_SRAM_DUMP_GM12to10_B:
                DrvSclOsSetEventIrq(DrvSclIrqGetIrqSYNCEventID(),  E_DRV_SCLIRQ_EVENT_GM12to10_B);
                break;
            default:
                break;
        }
    }
    _HalSclVipSetWriteRegType(0);
    HAL_SCLVIP_MUTEX_UNLOCK();
    return bRet;
}
bool HalSclVipGetSramDumpGammaRGBCallback(HalSclVipSramDumpType_e endump,u32 u32Sram,u32 u32reg)
{
    u16  u16sec = 0;
    u8  u8type = 0;
    u8 flag = 0;
    u8  bRet = 0;
    u16 u16chkodd = 0;
    u16 u16addr = 0,u16tvalueodd = 0,u16tvalueeven = 0,u16tcount = 0,u16readdata,u16size;
    switch(endump)
    {
        case E_HAL_SCLVIP_SRAM_DUMP_GM10to12_R:
            u16size = GAMMA10to12R_SRAM_BYTENUM;
            u16sec = 0;
            u8type = 0;
            break;
        case E_HAL_SCLVIP_SRAM_DUMP_GM10to12_G:
            u16size = GAMMA10to12G_SRAM_BYTENUM;
            u16sec = 2;
            u8type = 0;
            break;
        case E_HAL_SCLVIP_SRAM_DUMP_GM10to12_B:
            u16size = GAMMA10to12B_SRAM_BYTENUM;
            u16sec = 4;
            u8type = 0;
            break;
        case E_HAL_SCLVIP_SRAM_DUMP_GM12to10_R:
            u16size = GAMMA12to10R_SRAM_BYTENUM;
            u16sec = 0;
            u8type = 1;
            break;
        case E_HAL_SCLVIP_SRAM_DUMP_GM12to10_G:
            u16size = GAMMA12to10G_SRAM_BYTENUM;
            u16sec = 2;
            u8type = 1;
            break;
        case E_HAL_SCLVIP_SRAM_DUMP_GM12to10_B:
            u16size = GAMMA12to10B_SRAM_BYTENUM;
            u16sec = 4;
            u8type = 1;
            break;
        default:
            u16size = 0;
            u16sec = 0;
            u8type = 0;
            break;
    }
    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISR, "RGBGamma:%hhx\n",u16sec+u8type);
    HAL_SCLVIP_MUTEX_LOCK();
    DrvSclCmdqSramReset();
    for(u16addr=0;u16addr<u16size;u16addr++)
    {
        if(!DrvSclVipGetIsBlankingRegion())
        {
            HAL_SCLVIP_MUTEX_UNLOCK();
            return 0;
        }
        u16tvalueeven = Hal_VIP_GetGammaRGBTvalue(u16sec, u16tcount,u32Sram,u8type);
        u16tvalueodd = Hal_VIP_GetGammaRGBTvalue(u16sec, u16tcount+2,u32Sram,u8type);
        _HalSclVipWriteReg(u32reg, u16addr, 0x007F);//addr
        _HalSclVipWriteReg(u32reg, u16sec<<8, BIT8|BIT9|BIT10);//sec
        _HalSclVipWriteReg(u32reg, BIT12, BIT12);//ren
        u16readdata=HalUtilityR2BYTEDirect(u32reg+4);
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_SRAMCheck, "RGBGamma:%hhx ",u16sec+u8type);
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_SRAMCheck, "addr:%hx ,even val:%hx ",u16addr,u16tvalueeven);
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_SRAMCheck, "read val:%hx\n",u16readdata);
        if(u16readdata != u16tvalueeven)
        {
            if(u16tcount>= 2)
            {
                u16chkodd = Hal_VIP_GetGammaRGBTvalue(u16sec, u16tcount-2,u32Sram,u8type);
            }
            else
            {
                u16chkodd = Hal_VIP_GetGammaRGBTvalue(u16sec, u16tcount,u32Sram,u8type);
            }
            bRet = _HalSclVipCheckMonotonicallyIncreasing(u16readdata,u16tvalueodd,u16chkodd);
            SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_SRAMCheck, "read val:%hx odd val:%hx ckodd val:%hx\n"
                ,u16readdata,u16tvalueodd,u16chkodd);
            if(!bRet)
            {
                _HalSclVipSetWriteRegType(1);
                flag = gbCMDQ;
                u16readdata=HalUtilityR2BYTEDirect(u32reg+4);
                if(DrvSclVipGetIsBlankingRegion())
                {
                    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISRCheck, "[ISRCheck]RGBGamma:%hhx ",u16sec+u8type);
                    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISRCheck, "addr:%hx ,even val:%hx ",u16addr,u16tvalueeven);
                    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISRCheck, "read val:%hx\n",u16readdata);
                }
                _HalSclVipWriteReg(u32reg+2, (u16)u16tvalueeven, 0xFFF);//data
                _HalSclVipWriteReg(u32reg, ((u16addr)|((u16)u16sec<<8)|BIT11), 0xFFFF);//wen
                _HalSclVipWriteReg(u32reg, ((u16addr)|((u16)u16sec<<8)|BIT12), 0xFFFF);//ren
                _HalSclVipSetWriteRegType(0);
            }
        }
        _HalSclVipWriteReg(u32reg, u16addr, 0x007F);//addr
        _HalSclVipWriteReg(u32reg, (u16sec+1)<<8, BIT8|BIT9|BIT10);//sec
        _HalSclVipWriteReg(u32reg, BIT12, BIT12);//ren
        u16readdata=HalUtilityR2BYTEDirect(u32reg+4);
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_SRAMCheck, "RGBGamma:%hhx ",u16sec+u8type);
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_SRAMCheck, "addr:%hx ,odd val:%hx ",u16addr,u16tvalueodd);
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_SRAMCheck, "read val:%hx\n",u16readdata);
        if(u16readdata != u16tvalueodd)
        {
            if(u16tcount< 4*(u16size-1))
            {
                u16chkodd = Hal_VIP_GetGammaRGBTvalue(u16sec, u16tcount+4,u32Sram,u8type);
            }
            else
            {
                u16chkodd = Hal_VIP_GetGammaRGBTvalue(u16sec, u16tcount+2,u32Sram,u8type);
            }
            bRet = _HalSclVipCheckMonotonicallyIncreasing(u16readdata,u16chkodd,u16tvalueeven);
            SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_SRAMCheck, "read val:%hx odd val:%hx ckeven val:%hx\n"
                ,u16readdata,u16chkodd,u16tvalueeven);
            if(!bRet)
            {
                _HalSclVipSetWriteRegType(1);
                u16readdata=HalUtilityR2BYTEDirect(u32reg+4);
                flag = gbCMDQ;
                if(DrvSclVipGetIsBlankingRegion())
                {
                    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISRCheck, "[ISRCheck]RGBGamma:%hhx ",u16sec+u8type);
                    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISRCheck, "addr:%hx ,odd val:%hx ",u16addr,u16tvalueodd);
                    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISRCheck, "read val:%hx\n",u16readdata);
                }
                _HalSclVipWriteReg(u32reg+2, (u16)u16tvalueodd, 0xFFF);//data
                _HalSclVipWriteReg(u32reg, ((u16addr)|(((u16)u16sec+1)<<8)|BIT11), 0xFFFF);//wen
                _HalSclVipWriteReg(u32reg, ((u16addr)|(((u16)u16sec+1)<<8)|BIT12), 0xFFFF);//ren
                _HalSclVipSetWriteRegType(0);
            }
        }
        u16tcount+=4;
    }
    if(flag)
    {
        DrvSclCmdqFireSram(E_DRV_SCLCMDQ_TYPE_IP0,1);
    }
    HAL_SCLVIP_MUTEX_UNLOCK();
    return 1;
}
bool HalSclVipGetSramDumpGammaRGB(HalSclVipSramDumpType_e endump,u32 u32Sram,u32 u32reg)
{
    u16  u16sec = 0;
    u8  u8type = 0;
    u8 flag = 0;
    u16 u16addr = 0,u16tvalueodd = 0,u16tvalueeven = 0,u16tcount = 0,u16readdata,u16size;
    switch(endump)
    {
        case E_HAL_SCLVIP_SRAM_DUMP_GM10to12_R:
            u16size = GAMMA10to12R_SRAM_BYTENUM;
            u16sec = 0;
            u8type = 0;
            break;
        case E_HAL_SCLVIP_SRAM_DUMP_GM10to12_G:
            u16size = GAMMA10to12G_SRAM_BYTENUM;
            u16sec = 2;
            u8type = 0;
            break;
        case E_HAL_SCLVIP_SRAM_DUMP_GM10to12_B:
            u16size = GAMMA10to12B_SRAM_BYTENUM;
            u16sec = 4;
            u8type = 0;
            break;
        case E_HAL_SCLVIP_SRAM_DUMP_GM12to10_R:
            u16size = GAMMA12to10R_SRAM_BYTENUM;
            u16sec = 0;
            u8type = 1;
            break;
        case E_HAL_SCLVIP_SRAM_DUMP_GM12to10_G:
            u16size = GAMMA12to10G_SRAM_BYTENUM;
            u16sec = 2;
            u8type = 1;
            break;
        case E_HAL_SCLVIP_SRAM_DUMP_GM12to10_B:
            u16size = GAMMA12to10B_SRAM_BYTENUM;
            u16sec = 4;
            u8type = 1;
            break;
        default:
            u16size = 0;
            u16sec = 0;
            u8type = 0;
            break;
    }
    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISR, "RGBGamma:%hhx\n",u16sec+u8type);
    HAL_SCLVIP_MUTEX_LOCK();
    DrvSclCmdqSramReset();
    for(u16addr=0;u16addr<u16size;u16addr++)
    {
        if(!DrvSclVipGetIsBlankingRegion())
        {
            HAL_SCLVIP_MUTEX_UNLOCK();
            return 0;
        }
        u16tvalueeven = Hal_VIP_GetGammaRGBTvalue(u16sec, u16tcount,u32Sram,u8type);
        u16tvalueodd = Hal_VIP_GetGammaRGBTvalue(u16sec, u16tcount+2,u32Sram,u8type);
        _HalSclVipWriteReg(u32reg, u16addr, 0x007F);//addr
        _HalSclVipWriteReg(u32reg, u16sec<<8, BIT8|BIT9|BIT10);//sec
        _HalSclVipWriteReg(u32reg, BIT12, BIT12);//ren
        u16readdata=HalUtilityR2BYTEDirect(u32reg+4);
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_SRAMCheck, "RGBGamma:%hhx ",u16sec+u8type);
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_SRAMCheck, "addr:%hx ,even val:%hx ",u16addr,u16tvalueeven);
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_SRAMCheck, "read val:%hx\n",u16readdata);
        if(u16readdata != u16tvalueeven)
        {
            _HalSclVipSetWriteRegType(1);
            flag = gbCMDQ;
            u16readdata=HalUtilityR2BYTEDirect(u32reg+4);
            if(DrvSclVipGetIsBlankingRegion())
            {
                SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISRCheck, "RGBGamma:%hhx ",u16sec+u8type);
                SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISRCheck, "addr:%hx ,even val:%hx ",u16addr,u16tvalueeven);
                SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISRCheck, "read val:%hx\n",u16readdata);
            }
            _HalSclVipWriteReg(u32reg+2, (u16)u16tvalueeven, 0xFFF);//data
            _HalSclVipWriteReg(u32reg, ((u16addr)|((u16)u16sec<<8)|BIT11), 0xFFFF);//wen
            _HalSclVipWriteReg(u32reg, ((u16addr)|((u16)u16sec<<8)|BIT12), 0xFFFF);//ren
            _HalSclVipSetWriteRegType(0);
        }
        _HalSclVipWriteReg(u32reg, u16addr, 0x007F);//addr
        _HalSclVipWriteReg(u32reg, (u16sec+1)<<8, BIT8|BIT9|BIT10);//sec
        _HalSclVipWriteReg(u32reg, BIT12, BIT12);//ren
        u16readdata=HalUtilityR2BYTEDirect(u32reg+4);
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_SRAMCheck, "RGBGamma:%hhx ",u16sec+u8type);
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_SRAMCheck, "addr:%hx ,odd val:%hx ",u16addr,u16tvalueodd);
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_SRAMCheck, "read val:%hx\n",u16readdata);
        if(u16readdata != u16tvalueodd)
        {
            _HalSclVipSetWriteRegType(1);
            u16readdata=HalUtilityR2BYTEDirect(u32reg+4);
            flag = gbCMDQ;
            if(DrvSclVipGetIsBlankingRegion())
            {
                SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISRCheck, "RGBGamma:%hhx ",u16sec+u8type);
                SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISRCheck, "addr:%hx ,odd val:%hx ",u16addr,u16tvalueodd);
                SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISRCheck, "read val:%hx\n",u16readdata);
            }
            _HalSclVipWriteReg(u32reg+2, (u16)u16tvalueodd, 0xFFF);//data
            _HalSclVipWriteReg(u32reg, ((u16addr)|(((u16)u16sec+1)<<8)|BIT11), 0xFFFF);//wen
            _HalSclVipWriteReg(u32reg, ((u16addr)|(((u16)u16sec+1)<<8)|BIT12), 0xFFFF);//ren
            _HalSclVipSetWriteRegType(0);
        }
        u16tcount+=4;
    }
    if(flag)
    {
        DrvSclCmdqFireSram(E_DRV_SCLCMDQ_TYPE_IP0,1);
    }
    HAL_SCLVIP_MUTEX_UNLOCK();
    return 1;
}

bool _HalSclVipSetSramDump(HalSclVipSramDumpType_e endump,u32 u32Sram,u32 u32reg)
{
    if(endump == E_HAL_SCLVIP_SRAM_DUMP_IHC || endump == E_HAL_SCLVIP_SRAM_DUMP_ICC)
    {
        _HalSclVipSramDumpIhcIcc(endump,u32reg);
    }
    else if(endump == E_HAL_SCLVIP_SRAM_DUMP_WDR)
    {
        _HalSclVipSramDumpWdr(u32reg,u32Sram);
    }
    else if(endump == E_HAL_SCLVIP_SRAM_DUMP_HVSP_V || endump == E_HAL_SCLVIP_SRAM_DUMP_HVSP_V_1||
        endump == E_HAL_SCLVIP_SRAM_DUMP_HVSP_V_2)
    {
        _HalSclVipSramDumpVsp(u32Sram,u32reg);
    }
    else if(endump == E_HAL_SCLVIP_SRAM_DUMP_HVSP_H || endump == E_HAL_SCLVIP_SRAM_DUMP_HVSP_H_1||
        endump == E_HAL_SCLVIP_SRAM_DUMP_HVSP_H_2)
    {
        _HalSclVipSramDumpHsp(u32Sram,u32reg);
    }
    else if(endump == E_HAL_SCLVIP_SRAM_DUMP_GAMMA_Y || endump == E_HAL_SCLVIP_SRAM_DUMP_GAMMA_U||
        endump == E_HAL_SCLVIP_SRAM_DUMP_GAMMA_V)
    {
        return _HalSclVipSramDumpGammaYuv(endump,u32Sram,u32reg);
    }
    else
    {
        return Hal_VIP_SRAMDumpGammaRGB(endump,u32Sram,u32reg);
    }
    return 1;
}
bool HalSclVipSramDump(HalSclVipSramDumpType_e endump,u32 u32Sram)
{
    u16 u16clkreg;
    bool bRet;
    u32 u32reg = endump == E_HAL_SCLVIP_SRAM_DUMP_IHC ? REG_VIP_ACE2_7C_L :
                    endump == E_HAL_SCLVIP_SRAM_DUMP_ICC ?  REG_VIP_ACE2_78_L :
                    endump == E_HAL_SCLVIP_SRAM_DUMP_HVSP_V ? REG_SCL_HVSP0_41_L:
                    endump == E_HAL_SCLVIP_SRAM_DUMP_HVSP_V_1 ? REG_SCL_HVSP1_41_L:
                    endump == E_HAL_SCLVIP_SRAM_DUMP_HVSP_V_2 ? REG_SCL_HVSP2_41_L:
                    endump == E_HAL_SCLVIP_SRAM_DUMP_HVSP_H ? REG_SCL_HVSP0_41_L:
                    endump == E_HAL_SCLVIP_SRAM_DUMP_HVSP_H_1 ? REG_SCL_HVSP1_41_L:
                    endump == E_HAL_SCLVIP_SRAM_DUMP_HVSP_H_2 ? REG_SCL_HVSP2_41_L:
                    endump == E_HAL_SCLVIP_SRAM_DUMP_WDR ?  REG_VIP_WDR_78_L :
                    endump == E_HAL_SCLVIP_SRAM_DUMP_GAMMA_Y ?  REG_VIP_SCNR_41_L :
                    endump == E_HAL_SCLVIP_SRAM_DUMP_GAMMA_U ?  REG_VIP_SCNR_41_L :
                    endump == E_HAL_SCLVIP_SRAM_DUMP_GAMMA_V ?  REG_VIP_SCNR_41_L :
                    endump == E_HAL_SCLVIP_SRAM_DUMP_GM10to12_R ?  REG_SCL_HVSP1_7A_L :
                    endump == E_HAL_SCLVIP_SRAM_DUMP_GM10to12_G ?  REG_SCL_HVSP1_7A_L :
                    endump == E_HAL_SCLVIP_SRAM_DUMP_GM10to12_B ?  REG_SCL_HVSP1_7A_L :
                    endump == E_HAL_SCLVIP_SRAM_DUMP_GM12to10_R ?  REG_SCL_HVSP1_7D_L :
                    endump == E_HAL_SCLVIP_SRAM_DUMP_GM12to10_G ?  REG_SCL_HVSP1_7D_L :
                    endump == E_HAL_SCLVIP_SRAM_DUMP_GM12to10_B ?  REG_SCL_HVSP1_7D_L :
                        0;
    //clk open
    if(endump == E_HAL_SCLVIP_SRAM_DUMP_HVSP_V_2 || endump == E_HAL_SCLVIP_SRAM_DUMP_HVSP_H_2)
    {
        u16clkreg = HalUtilityR2BYTEDirect(REG_SCL_CLK_65_L);
        HalUtilityW2BYTEMSK(REG_SCL_CLK_65_L,0x0,0xFFFF);
    }
    else
    {
        u16clkreg = HalUtilityR2BYTEDirect(REG_SCL_CLK_64_L);
        HalUtilityW2BYTEMSK(REG_SCL_CLK_64_L,0x0,0xFFFF);
    }
    bRet = _HalSclVipSetSramDump(endump,u32Sram,u32reg);

    //clk close
    if(endump == E_HAL_SCLVIP_SRAM_DUMP_HVSP_V_2|| endump == E_HAL_SCLVIP_SRAM_DUMP_HVSP_H_2)
    {
        HalUtilityW2BYTEMSK(REG_SCL_CLK_65_L,u16clkreg,0xFFFF);
    }
    else
    {
        HalUtilityW2BYTEMSK(REG_SCL_CLK_64_L,u16clkreg,0xFFFF);
    }
    return bRet;
}

//-----------------------DLC
void HalSclVipDlcHistVarOnOff(u16 u16var)
{
    HalUtilityW2BYTEMSK(REG_VIP_DLC_04_L,u16var,0x25a2);
}

void HalSclVipSetDlcstatMIU(u8 u8value,u32 u32addr1,u32 u32addr2)
{
    HalUtilityW2BYTEMSK(REG_VIP_MWE_15_L,u8value,0x0001);
    HalUtilityW4BYTE(REG_VIP_MWE_18_L,u32addr1>>4);
    HalUtilityW4BYTE(REG_VIP_MWE_1A_L,u32addr2>>4);
}
void HalSclVipSetDlcShift(u8 u8value)
{
    HalUtilityW2BYTEMSK(REG_VIP_DLC_03_L,u8value,0x0007);
}
void HAlSclVipSetDlcMode(u8 u8value)
{
    HalUtilityW2BYTEMSK(REG_VIP_MWE_1C_L,u8value<<2,0x0004);
}
void HalSclVipSetDlcActWin(bool bEn,u16 u16Vst,u16 u16Hst,u16 u16Vnd,u16 u16Hnd)
{
    HalUtilityW2BYTEMSK(REG_VIP_DLC_08_L,bEn<<7,0x0080);
    HalUtilityW2BYTEMSK(REG_VIP_MWE_01_L,u16Vst,0x03FF);
    HalUtilityW2BYTEMSK(REG_VIP_MWE_02_L,u16Vnd,0x03FF);
    HalUtilityW2BYTEMSK(REG_VIP_MWE_03_L,u16Hst,0x01FF);
    HalUtilityW2BYTEMSK(REG_VIP_MWE_04_L,u16Hnd,0x01FF);
}
void HalSclVipDlcHistSetRange(u8 u8value,u8 u8range)
{
    u16 u16tvalue;
    u16 u16Mask;
    u32 u32Reg;
    u8range = u8range-1;
    u32Reg = REG_VIP_DLC_0C_L+(((u8range)/2)*2);
    if((u8range%2) == 0)
    {
        u16Mask     = 0x00FF;
        u16tvalue   = ((u16)u8value);

    }
    else
    {
        u16Mask     = 0xFF00;
        u16tvalue   = ((u16)u8value)<<8;
    }

    HalUtilityW2BYTEMSK(u32Reg,(u16tvalue),u16Mask);

}
u32 HalSclVipDlcHistGetRange(u8 u8range)
{
    u32 u32tvalue;
    u32 u32Reg;
    u32Reg      = REG_VIP_MWE_20_L+(((u8range)*2)*2);
    u32tvalue   = HalUtilityR4BYTEDirect(u32Reg);
    return u32tvalue;

}

u8 HalSclVipDlcGetBaseIdx(void)
{
    u8 u8tvalue;
    u8tvalue = HalUtilityR2BYTEDirect(REG_VIP_MWE_15_L);
    u8tvalue = (u8)(u8tvalue&0x80)>>7;
    return u8tvalue;
}

u32 HalSclVipDlcGetPC(void)
{
    u32 u32tvalue;
    u32tvalue = HalUtilityR4BYTEDirect(REG_VIP_MWE_08_L);
    return u32tvalue;
}

u32 HalSclVipDlcGetPW(void)
{
    u32 u32tvalue;
    u32tvalue = HalUtilityR4BYTEDirect(REG_VIP_MWE_0A_L);
    return u32tvalue;
}

u8 HalSclVipDlcGetMinP(void)
{
    u16 u16tvalue;
    u16tvalue = HalUtilityR2BYTEDirect(REG_VIP_DLC_62_L);
    u16tvalue = (u16tvalue>>8);
    return (u8)u16tvalue;
}

u8 HalSclVipDlcGetMaxP(void)
{
    u16 u16tvalue;
    u16tvalue = HalUtilityR2BYTEDirect(REG_VIP_DLC_62_L);
    return (u8)u16tvalue;
}
// LDC frame control
void HalSclVipSetLdcBypass(bool bEn)
{
    HalUtilityW2BYTEMSK(REG_SCL_LDC_0F_L, bEn ? BIT0 : 0, BIT0);
}


void HalSclVipSetLdcHwRwDiff(u16 u8Val)
{
    HalUtilityW2BYTEMSK(REG_SCL_LDC_11_L, ((u16)u8Val), BIT1|BIT0);
}
void HalSclVipSetLdcSwIdx(u8 idx)
{
    HalUtilityW2BYTEMSK(REG_SCL_LDC_10_L, ((u16)idx), BIT1|BIT0);
}
void HalSclVipSetLdcSwMode(u8 bEn)
{
    HalUtilityW2BYTEMSK(REG_SCL_LDC_12_L, ((u16)bEn)? BIT0 :0, BIT0);
}
void HalSclVipSetLdcBankMode(DrvSclVipLdcLcBankModeType_e enType)
{
    HalUtilityW2BYTEMSK(REG_SCL_LDC_09_L, ((u16)enType == E_DRV_SCLVIP_LDCLCBANKMODE_64)? BIT0 :0, BIT0);
}

void HalSclVipSetLdcDmapBase(u32 u32Base)
{
    HalUtilityW4BYTE(REG_SCL_LDC_0A_L, u32Base);
}

void HalSclVipSetLdcDmapPitch(u32 u32Pitch)
{
    HalUtilityW4BYTE(REG_SCL_LDC_0C_L, u32Pitch);
}
void HalSclVipMcnrInit(void)
{
    HalUtilityW2BYTEMSK(REG_SCL_DNR1_7F_L, 0x0007, BIT2|BIT1|BIT0); //for I3e ECO
}
void HalSclVipLdcEco(void)
{
    HalUtilityW2BYTEMSK(REG_SCL_LDC_31_L, BIT0, BIT0); //for I3e ECO
}
void HalSclVipAipDB(u8 u8En)
{
    HalUtilityW2BYTEMSK(REG_VIP_SCNR_7F_L, u8En ? 0 : BIT0, BIT0);
}
void HalSclVipSetLdc422To444Md(u8 u8md)
{
    HalUtilityW2BYTEMSK(REG_SCL_LDC_1D_L, (u16)u8md, BIT1|BIT0);
}
void HalSclVipSetLdc444To422Md(u8 u8md)
{
    HalUtilityW2BYTEMSK(REG_SCL_LDC_1F_L, (u16)u8md, BIT1|BIT0);
}

void HalSclVipSetLdcDmapOffset(u8 u8offset)
{
    HalUtilityW2BYTEMSK(REG_SCL_LDC_0E_L, (u8offset), 0x003F);
}

void HalSclVipSetLdcDmapPS(u8 bEn)
{
    HalUtilityW2BYTEMSK(REG_SCL_LDC_0E_L, (bEn<<6), BIT6);
}
void HalSclVipSetLdcSramAmount(u16 u16hor,u16 u16ver)
{
    HalUtilityW2BYTEMSK(REG_SCL_LDC_22_L, ((u16)u16hor), 0xFFFF);
    HalUtilityW2BYTEMSK(REG_SCL_LDC_2A_L, ((u16)u16ver), 0xFFFF);
}
void HalSclVipSetLdcSramBase(u32 u32hor,u32 u32ver)
{
    HalUtilityW4BYTE(REG_SCL_LDC_24_L, (u32hor>>4));
    HalUtilityW4BYTE(REG_SCL_LDC_2C_L, (u32ver>>4));
}
void HalSclVipSetLdcSramStr(u16 u16hor,u16 u16ver)
{

    HalUtilityW2BYTEMSK(REG_SCL_LDC_23_L, ((u16)u16hor), 0xFFFF);
    HalUtilityW2BYTEMSK(REG_SCL_LDC_2B_L, ((u16)u16ver), 0xFFFF);
}

void HalSclVipSetAutoDownloadAddr(u32 u32baseadr,u16 u16iniaddr,u8 u8cli)
{
    switch(u8cli)
    {
        case 9:
            HalUtilityW2BYTEMSK(REG_SCL1_73_L, (u16)(u32baseadr>>4), 0xFFFF);
            HalUtilityW2BYTEMSK(REG_SCL1_74_L, (u16)(u32baseadr>>20), 0x01FF);
            HalUtilityW2BYTEMSK(REG_SCL1_77_L, ((u16)u16iniaddr), 0xFFFF);
            break;
        default:
            break;
    }
}

void HalSclVipSetAutoDownloadReq(u16 u16depth,u16 u16reqlen,u8 u8cli)
{
    switch(u8cli)
    {
        case 9:
            HalUtilityW2BYTEMSK(REG_SCL1_76_L, ((u16)u16reqlen), 0xFFFF);
            HalUtilityW2BYTEMSK(REG_SCL1_75_L, ((u16)u16depth), 0xFFFF);
            break;
        default:
            break;
    }
}

void HalSclVipSetAutoDownload(u8 bCLientEn,u8 btrigContinue,u8 u8cli)
{
    switch(u8cli)
    {
        case 9:
            HalUtilityW2BYTEMSK(REG_SCL1_72_L, bCLientEn|(btrigContinue<<1), 0x0003);
            break;
        default:
            break;
    }
}

void HalSclVipSetAutoDownloadTimer(u8 bCLientEn)
{
    HalUtilityW2BYTEMSK(REG_SCL1_78_L, bCLientEn<<15, 0x8000);
}
void HalSclVipGetNlmSram(u16 u16entry)
{
    u32 u32tvalue1,u32tvalue2;
    HalUtilityW2BYTEMSK(REG_SCL_NLM0_62_L, u16entry, 0x07FF);
    HalUtilityW2BYTEMSK(REG_SCL_NLM0_62_L, 0x8000, 0x8000);
    HalUtilityW2BYTEMSK(REG_SCL_NLM0_62_L, 0x2000, 0x2000);
    u32tvalue1 = HalUtilityR2BYTEDirect(REG_SCL_NLM0_64_L);
    u32tvalue2 = HalUtilityR2BYTEDirect(REG_SCL_NLM0_65_L);
    u32tvalue1 |= ((u32tvalue2&0x00F0)<<12);
    HalUtilityW2BYTEMSK(REG_SCL_NLM0_62_L, 0x0000, 0x8000);
    SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[Get_SRAM]entry%hx :%lx\n",u16entry,u32tvalue1);
}

void HalSclVipSetNlmSrambyCPU(u16 u16entry,u32 u32tvalue)
{
    HalUtilityW2BYTEMSK(REG_SCL_NLM0_62_L, u16entry, 0x07FF);
    HalUtilityW2BYTEMSK(REG_SCL_NLM0_62_L, 0x8000, 0x8000);
    HalUtilityW2BYTEMSK(REG_SCL_NLM0_63_L, (u16)u32tvalue, 0xFFFF);
    HalUtilityW2BYTEMSK(REG_SCL_NLM0_65_L, (u16)(u32tvalue>>16), 0x000F);
    HalUtilityW2BYTEMSK(REG_SCL_NLM0_62_L, 0x4000, 0x4000);
    HalUtilityW2BYTEMSK(REG_SCL_NLM0_62_L, 0x0000, 0x8000);
    SCL_DBG(SCL_DBG_LV_DRVVIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[Set_SRAM]entry%hx :%lx\n",u16entry,u32tvalue);
}

#define  VTRACK_KEY_SETTING_LENGTH 8

static u8 u8VtrackKey[VTRACK_KEY_SETTING_LENGTH]=
{
    0xaa,0x13,0x46,0x90,0x28,0x35,0x29,0xFE,
};

void HalSclVipVtrackSetKey(bool bUserDefinded, u8 *pu8Setting)
{
    /*
     * Default Setting:
     * setting1 [0e]              => 8'h00
     * setting2 [0f]              => 8'h00
     * setting3 [1a:10]           => 165'h04 21 08 418c6318c4 21084210842108421086318c53
     */

    u8 *pu8Data = NULL;
    u16 u16Index = 0;
    u16 u16Data = 0;

    if (bUserDefinded == TRUE)
    {
        pu8Data = pu8Setting;
    }
    else
    {
        pu8Data = &u8VtrackKey[0];
    }
    for (u16Index = 0; u16Index < VTRACK_KEY_SETTING_LENGTH; u16Index = u16Index+2)
    {
        u8 u8Offset = (( (u16Index) / 2) *2 );
        u32 u32Addr = REG_SCL1_24_L + u8Offset;

        if ( (VTRACK_KEY_SETTING_LENGTH - u16Index) >= 2)
        {
            u16Data = (u16)pu8Data[u16Index +1];
            u16Data = (u16Data << 8) + (u16) pu8Data[u16Index];
            HalUtilityW2BYTEMSK(u32Addr, u16Data, 0xffff);
        }
    }

}

void HalSclVipVtrackSetPayloadData(u16 u16Timecode, u8 u8OperatorID)
{
    /*
     * reg_payload_use from 0x28 to 0x2C (1B - 1F)
     * {14'h0, TimeCode[55:40], Unique ID (OTP)[39:8], Operator ID[7:0]}
     */

    HalUtilityW2BYTEMSK(REG_SCL1_28_L, (u16)u8OperatorID, 0x00ff);
    HalUtilityW2BYTEMSK(REG_SCL1_2A_L, (u16Timecode << 8) , 0xff00);
    HalUtilityW2BYTEMSK(REG_SCL1_2B_L, (u16Timecode >> 8) , 0x00ff);

}

#define  VTRACK_SETTING_LENGTH 23

static u8 u8VtrackSetting[VTRACK_SETTING_LENGTH]=
{
    0x00, 0x00, 0xe8, 0x18, 0x32,
    0x86, 0x10, 0x42, 0x08, 0x21,
    0x84, 0x10, 0x42, 0x08, 0x21,
    0xc4, 0x18, 0x63, 0x8c, 0x41,
    0x08, 0x21, 0x04,
};

void HalSclVipVtrackSetUserDefindedSetting(bool bUserDefinded, u8 *pu8Setting)
{
    /*
     * Default Setting:
     * setting1 [0e]              => 8'h00
     * setting2 [0f]              => 8'h00
     * setting3 [1a:10]           => 165'h04 21 08 418c6318c4 21084210842108421086318c53
     */

    u8 *pu8Data = NULL;
    u16 u16Index = 0;
    u16 u16Data = 0;

    if (bUserDefinded == TRUE)
    {
        pu8Data = pu8Setting;
    }
    else
    {
        pu8Data = &u8VtrackSetting[0];
    }
    //MenuLoad enable
    HalUtilityW2BYTEMSK(REG_SCL1_2E_L, pu8Data[0], 0xff);
    HalUtilityW2BYTEMSK(REG_SCL1_2F_L, pu8Data[1], 0xff);
    for (u16Index = 2; u16Index < VTRACK_SETTING_LENGTH; u16Index = u16Index+2)
    {
        u8 u8Offset = (( (u16Index - 2) / 2) *2 );
        u32 u32Addr = REG_SCL1_30_L + u8Offset;

        if ( (VTRACK_SETTING_LENGTH - u16Index) >= 2)
        {
            u16Data = (u16)pu8Data[u16Index +1];
            u16Data = (u16Data << 8) + (u16) pu8Data[u16Index];
            HalUtilityW2BYTEMSK(u32Addr, u16Data, 0xffff);
        }
        else
        {
            u16Data = (u16) pu8Data[u16Index];
            HalUtilityW2BYTEMSK(u32Addr, u16Data, 0x00ff);
        }
    }

}

#define VIP_VTRACK_MODE 0x1c   //[3]:v_sync_inv_en ;[2]:h_sync_inv_en [4]mux
void HalSclVipVtrackEnable(u8 u8FrameRate, HalSclVipVtrackEnableType_e bEnable)
{
    //FrameRateIn     => 8'h1E
    HalUtilityW2BYTEMSK(REG_SCL1_21_L , u8FrameRate, 0xFF);

    //EnableIn              => 1
    //DebugEn               => 0
    if (bEnable == E_HAL_SCLVIP_VTRACK_ENABLE_ON)
    {
        HalUtilityW2BYTEMSK(REG_SCL1_20_L , BIT(0), BIT(1)|BIT(0) );
        HalUtilityW2BYTEMSK(REG_SCL1_22_L , VIP_VTRACK_MODE, 0x3F );
    }
    else if(bEnable == E_HAL_SCLVIP_VTRACK_ENABLE_DEBUG)
    {
        HalUtilityW2BYTEMSK(REG_SCL1_20_L , BIT(1)|BIT(0), BIT(1)|BIT(0) );
        HalUtilityW2BYTEMSK(REG_SCL1_22_L , VIP_VTRACK_MODE, 0x3F );
    }
    else
    {
        HalUtilityW2BYTEMSK(REG_SCL1_20_L , 0x00, BIT(1)|BIT(0) );
        HalUtilityW2BYTEMSK(REG_SCL1_22_L , VIP_VTRACK_MODE, 0x3F );
    }
}
void HalSclVipInitY2R(void)
{
    HalUtilityW2BYTEMSK(REG_SCL_HVSP2_60_L, 0x0A01, 0x1FFF);
    HalUtilityW2BYTEMSK(REG_SCL_HVSP2_61_L, 0x59E, 0x1FFF);
    HalUtilityW2BYTEMSK(REG_SCL_HVSP2_62_L, 0x401, 0x1FFF);
    HalUtilityW2BYTEMSK(REG_SCL_HVSP2_63_L, 0x1FFF, 0x1FFF);
    HalUtilityW2BYTEMSK(REG_SCL_HVSP2_64_L, 0x1D24, 0x1FFF);
    HalUtilityW2BYTEMSK(REG_SCL_HVSP2_65_L, 0x400, 0x1FFF);
    HalUtilityW2BYTEMSK(REG_SCL_HVSP2_66_L, 0x1E9F, 0x1FFF);
    HalUtilityW2BYTEMSK(REG_SCL_HVSP2_67_L, 0x1FFF, 0x1FFF);
    HalUtilityW2BYTEMSK(REG_SCL_HVSP2_68_L, 0x400, 0x1FFF);
    HalUtilityW2BYTEMSK(REG_SCL_HVSP2_69_L, 0x719, 0x1FFF);
    HalUtilityW2BYTEMSK(REG_SCL_HVSP1_6C_L, 0x181, 0x1FFF);
    HalUtilityW2BYTEMSK(REG_SCL_HVSP1_6D_L, 0x1FF, 0x1FFF);
    HalUtilityW2BYTEMSK(REG_SCL_HVSP1_6E_L, 0x1E54, 0x1FFF);
    HalUtilityW2BYTEMSK(REG_SCL_HVSP1_6F_L, 0x1FAD, 0x1FFF);
    HalUtilityW2BYTEMSK(REG_SCL_HVSP1_70_L, 0x132, 0x1FFF);
    HalUtilityW2BYTEMSK(REG_SCL_HVSP1_71_L, 0x259, 0x1FFF);
    HalUtilityW2BYTEMSK(REG_SCL_HVSP1_72_L, 0x75, 0x1FFF);
    HalUtilityW2BYTEMSK(REG_SCL_HVSP1_73_L, 0x1F53, 0x1FFF);
    HalUtilityW2BYTEMSK(REG_SCL_HVSP1_74_L, 0x1EAD, 0x1FFF);
    HalUtilityW2BYTEMSK(REG_SCL_HVSP1_75_L, 0x1FF, 0x1FFF);
}

bool HalSclVipGetVipBypass(void)
{
    bool bRet;
    bRet = HalUtilityR2BYTE(REG_VIP_LCE_70_L)&BIT0;
    return bRet;
}
bool HalSclVipGetMcnrBypass(void)
{
    bool bRet;
    bRet = HalUtilityR2BYTE(REG_VIP_MCNR_01_L)&(BIT0|BIT1);
    bRet = (((bRet)>>1))? 0: 1;
    return bRet;
}
bool HalSclVipGetLdcBypass(void)
{
    bool bRet;
    bRet = (HalUtilityR2BYTE(REG_SCL_LDC_0F_L)&BIT0)? 1: 0;
    return bRet;
}
bool HalSclVipGetNlmBypass(void)
{
    bool bRet;
    bRet = (HalUtilityR2BYTE(REG_SCL_NLM0_01_L)&BIT0)? 0: 1;
    return bRet;
}
