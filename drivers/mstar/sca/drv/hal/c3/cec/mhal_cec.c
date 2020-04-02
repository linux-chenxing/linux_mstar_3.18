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
// File Name: mhal_CEC.c
// Description: For CEC functions.
////////////////////////////////////////////////////////////////////////////////


#define _MHAL_CEC_C_

#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/string.h>
#else
#include <string.h>
#endif
#include "cec_hwreg_utility2.h"
#include "cec_Analog_Reg.h"
#include "MsOS.h"
#include "apiCEC.h"
#include "MsIRQ.h"
#include "mhal_CEC.h"

extern MS_U32 CEC_RIU_BASE;
extern InterruptNum gCECIRQ;

#define CEC_DPUTSTR(str)        //printf(str)
#define CEC_DPRINTF(str, x)     //printf(str, x)

#define PM_REG_WRITE    MDrv_WriteByte
#define PM_REG_READ     MDrv_ReadByte


#define MST_XTAL_CLOCK_HZ   (12000000UL)    /* Temp define */


void mhal_CEC_init_riu_base(MS_U32 u32riu_base, MS_U32 u32PMriu_base)
{
    CEC_RIU_BASE = u32PMriu_base;
}


MS_U8 mhal_CEC_HeaderSwap(MS_U8 value)
{
    return(((value&0x0f)<<4)+((value&0xf0)>>4));
}

MS_U8 mhal_CEC_SendFrame(MS_U8 header, MS_U8 opcode, MS_U8* operand, MS_U8 len)
{
    MS_U8 i, cnt, *ptr, res;
    MS_U8 u8waitcnt;
     // clear CEC TX INT status
    PM_REG_WRITE(H_BK_CEC(0x12), 0x0E);
    PM_REG_WRITE(H_BK_CEC(0x12), 0x00);
    PM_REG_WRITE( L_BK_CEC(0x18) , header );
    PM_REG_WRITE( H_BK_CEC(0x18), opcode );

    CEC_DPUTSTR("\r\n/********  CEC Tx **********/\r\n");
    CEC_DPRINTF("CEC Tx FIFO= 0x%x", (MS_U8)header);
    CEC_DPRINTF(" 0x%x", (MS_U8)opcode);

    if(len > 0)
    {
        ptr=operand;
        for(i=0;i<len;i++)
        {
            PM_REG_WRITE( L_BK_CEC(0x19) +i , *(ptr+i) );
            CEC_DPRINTF(" 0x%x", *(operand+i));
        }
        CEC_DPUTSTR("\r\n/**************************/\r\n");
    }


    // CEC transmit length
    if(header==mhal_CEC_HeaderSwap(header))
    {
        PM_REG_WRITE(L_BK_CEC(0x00), 0);                   //polling message
        u8waitcnt = 5;
    }
    else
    {
        PM_REG_WRITE(L_BK_CEC(0x00), (len+1));
        u8waitcnt = 30;
    }

//The total time,
//(1). successful, 4.5 ms + 10 * 2.4 ms * N = 4.5 ms + 24 * N
//              = 28.5 ms (1), or 52.5 ms (2), ....
//(2). NAK,        (4.5 ms + 10 * 2.4 ms) * 1 + (4.5 ms + 10 * 2.4 ms +7.2 ms(3 bit time)) * retry (3)
//              = 28.5 + 35.2 * 3 = 133.6 ms

    cnt=0;
    MsOS_DelayTask(20);
    do
    {
        MsOS_DelayTask(10);
        if(cnt++>=u8waitcnt)
            break;
    } while((PM_REG_READ(H_BK_CEC(0x11))&0x0E)==0);

    res = (PM_REG_READ(H_BK_CEC(0x11))&0x0E);

    if(cnt>=u8waitcnt)
        res |= E_CEC_SYSTEM_BUSY;

     // clear CEC TX INT status
    PM_REG_WRITE(H_BK_CEC(0x12), 0x0E);
    PM_REG_WRITE(H_BK_CEC(0x12), 0x00);

    return res;
}


void mhal_CEC_SetMyAddress(MS_U8 mylogicaladdress)
{
    PM_REG_WRITE(L_BK_CEC(0x02), (PM_REG_READ(L_BK_CEC(0x02)) & 0x0F) |(mylogicaladdress<<4));
}

void mhal_CEC_INTEn(MS_BOOL bflag)
{
    if(bflag) // unmask
        PM_REG_WRITE(L_BK_CEC(0x13), 0x1E);  //REG_HDMI_INT_MASK
    else // Mask CEC interrupt
        PM_REG_WRITE(L_BK_CEC(0x13), 0x1F);  //REG_HDMI_INT_MASK
}


void mhal_CEC_Init(MS_U32 u32XTAL_CLK_Hz)
{
    MS_U16 reg_val0, reg_val1;

#if ENABLE_CEC_INT
#if 0
    // Adcdvi irq clear control.
    MDrv_WriteByte( H_BK_ADC_ATOP(0x71), 0xFF);
    MDrv_WriteByte( H_BK_ADC_ATOP(0x71), 0);
    // Adcdvi irq mask control - disable all adcdvi irq.
    MDrv_WriteByte( H_BK_ADC_ATOP(0x70), 0xFF);

    // HDMI irq clear
    MDrv_Write2Byte( REG_HDMI_INT_CLEAR, 0xFFFF);
    MDrv_Write2Byte( REG_HDMI_INT_CLEAR, 0);
    // HDMI irq mask control -only enable CEC rx irq
    MDrv_Write2Byte( REG_HDMI_INT_MASK, 0xFEFF);
#endif
    // CEC irq clear
    PM_REG_WRITE(H_BK_CEC(0x12), 0x1F);
    PM_REG_WRITE(H_BK_CEC(0x12), 0x00);
    // CEC irq mask control -only enable CEC rx irq
    PM_REG_WRITE(L_BK_CEC(0x13), 0x1E);

#ifdef MSOS_TYPE_LINUX_KERNEL
    gCECIRQ = E_INT_IRQ_DVI_HDMI_HDCP; // IRQ52
#else
    gCECIRQ = E_INT_IRQ_CEC; // IRQ53
#endif
#endif

    PM_REG_WRITE(L_BK_CEC(0x14),0x01); // [1]: clock source from Xtal;[0]: Power down CEC controller select
    PM_REG_WRITE(H_BK_CEC(0x03),PM_REG_READ(H_BK_CEC(0x03))&(~ BIT(4))); // [4]: Standby mode;
    PM_REG_WRITE(H_BK_CEC(0x00),0x10|RETRY_CNT); // retry times
    PM_REG_WRITE(L_BK_CEC(0x01),0x80); // [5]:CEC clock no gate; [7]: Enable CEC controller
    PM_REG_WRITE(H_BK_CEC(0x01),0x53); // CNT1=3; CNT2 = 5;
    PM_REG_WRITE(L_BK_CEC(0x02),0x07); // CNT3=7; logical address: TV

    reg_val0=(MST_XTAL_CLOCK_HZ%100000l)*0.00016+0.5;
    PM_REG_WRITE(H_BK_CEC(0x02),(MST_XTAL_CLOCK_HZ/100000l)); // CEC time unit by Xtal(integer)
    reg_val1 = PM_REG_READ(L_BK_CEC(0x03));
    PM_REG_WRITE(L_BK_CEC(0x03), ((reg_val1 & 0xF0) | reg_val0)); // CEC time unit by Xtal(fractional)

    PM_REG_WRITE(L_BK_CEC(0x11), 0xFF); // clear CEC status
}

MS_BOOL mhal_CEC_IsMessageReceived(void)
{
    return (PM_REG_READ(H_BK_CEC(0x11)) & 0x01 ? TRUE : FALSE);
}

MS_U8 mhal_CEC_ReceivedMessageLen(void)
{
    return ((PM_REG_READ(L_BK_CEC(0x04)) & 0x1F) + 1);
}

MS_U8 mhal_CEC_GetMessageByte(MS_U8 idx)
{
    return (PM_REG_READ(L_BK_CEC(0x20) + idx));
}

void mhal_CEC_ClearRxStatus(void)
{
    // clear RX INT status
    PM_REG_WRITE(H_BK_CEC(0x12), 0x11);
    PM_REG_WRITE(H_BK_CEC(0x12), 0x00);
    // clear RX NACK status
    PM_REG_WRITE(L_BK_CEC(0x11), 0xFF);
}


/***************************************************************************************/
/// config cec wake up
/***************************************************************************************/
void mhal_CEC_ConfigWakeUp(void)
{
    //MS_U16 reg_val0,reg_val1;
    CEC_DPUTSTR("\r\n Here do the PM config cec wakeup \r\n");

    //XBYTE[0x250C] &= ~BIT0;
    //MDrv_WriteByte( L_BK_ADC_ATOP(0x06), MDrv_ReadByte(L_BK_ADC_ATOP(0x06)) &(~ BIT(0)));

    //(1) enable chiptop clk_mcu & clk_pram
    //XBYTE[0x0E00] = 0x03;
    //enable PM_Sleep's clk_mcu and _pram
//??    MDrv_WriteByte(L_BK_PMSLP(0x00), 0x03);
    //XBYTE[0x0E01] = 0x0F;
    //0x0C; Ken 20080916 for calibration to 1Mz
//??    MDrv_WriteByte(H_BK_PMSLP(0x00), 0x0F);

    //(2) HDMI CEC settings
#if 0
    PM_REG_WRITE(H_BK_CEC(0x00),0x10|RETRY_CNT); // retry times: 3
    PM_REG_WRITE(L_BK_CEC(0x01),0x80); // [5]:CEC clock no gate; [7]: Enable CEC controller
    PM_REG_WRITE(H_BK_CEC(0x01),0x53); // CNT1=3; CNT2 = 5;
    PM_REG_WRITE(L_BK_CEC(0x02),0x07); // CNT3=7; logical address: TV

    reg_val0=(MST_XTAL_CLOCK_HZ%100000l)*0.00016+0.5;
    PM_REG_WRITE(H_BK_CEC(0x02),(MST_XTAL_CLOCK_HZ/100000l)); // CEC time unit by Xtal(integer)
    reg_val1 = PM_REG_READ(L_BK_CEC(0x03));
    PM_REG_WRITE(L_BK_CEC(0x03), ((reg_val1 & 0xF0) | reg_val0)); // CEC time unit by Xtal(fractional)
#endif

    //(3) PM Sleep: wakeup enable sources
//??    PM_REG_WRITE(L_BK_PMMCU(0x00),0x01); // reg_cec_enw

    //(4) PM CEC power down controller settings
    // Mask CEC interrupt in standby mode
    PM_REG_WRITE(L_BK_CEC(0x13),0x1F);
    // select power down SW CEC controller
    PM_REG_WRITE(L_BK_CEC(0x14),0x03); // [1]: clock source from internal 4M clock;[0]: Power down CEC controller select
    PM_REG_WRITE(L_BK_CEC(0x01),0x00); // [5]:CEC clock no gate; [7]: Disable CEC controller
    PM_REG_WRITE(L_BK_CEC(0x01),0x80); // [5]:CEC clock no gate; [7]: Enable CEC controller
#if 0//ENABLE_SW_CEC_WAKEUP
    PM_REG_WRITE(H_BK_CEC(0x03),PM_REG_READ(H_BK_CEC(0x03))&(~ BIT(4))); // [4]: Standby mode;
#else
    PM_REG_WRITE(H_BK_CEC(0x03),PM_REG_READ(H_BK_CEC(0x03))|(BIT(4))); // [4]: sleep mode;
#endif
    //(5) PM CEC wakeup opcode settings
    // OPCODE0: 0x04(Image view on)
    // OPCODE1: 0x0D(Text view on)
    // OPCODE2: 0x44 0x40(Power)
    //          0x44 0x6D(Power ON Function)
    // OPCODE3: N/A
    // OPCODE4: 0x82(Active source) length = 2
    PM_REG_WRITE(L_BK_CEC(0x07), 0x37); // Enable OP0~2 and OP4
    PM_REG_WRITE(H_BK_CEC(0x07), 0x24); // Eanble OPCODE2's operand
    PM_REG_WRITE(L_BK_CEC(0x08), 0x04); // OPCODE0: Image View On
    PM_REG_WRITE(H_BK_CEC(0x08), 0x0D); // OPCODE1: Text View ON
    PM_REG_WRITE(L_BK_CEC(0x09), 0x44); // OPCODE2: User control
    PM_REG_WRITE(L_BK_CEC(0x0A), 0x82); // OPCODE4: Active source
    PM_REG_WRITE(H_BK_CEC(0x0B), 0x40); // User control - Power
    PM_REG_WRITE(L_BK_CEC(0x0C), 0x6D); // User control - Power ON Function
    PM_REG_WRITE(H_BK_CEC(0x0D), 0x84); // [2:0]: CEC version 1.3a; [7:3]: OP4 is broadcast message

    //(6) Device(TV) Vendor ID for customer (Big Endian)
    // It depends end-customer's vendor ID
    //MS_DEBUG_MSG(printf("!!!!!!!!!!!!!!!!!!!Change this Vendor ID according to customer!!!!!!!!!!!!!!!!\n"));
    PM_REG_WRITE(L_BK_CEC(0x0F),0x00); // Device Vendor ID
    PM_REG_WRITE(H_BK_CEC(0x0F),0x00); // Device Vendor ID
    PM_REG_WRITE(L_BK_CEC(0x10),0x00); // Device Vendor ID
    PM_REG_WRITE(H_BK_CEC(0x10),0x01); // [2:0]: Feature abort reason - "Not in correct mode to respond"

    //(7) Device Physical address: default is 0x00 0x00 0x00
    PM_REG_WRITE(L_BK_CEC(0x0E), 0x00); // Physical address 0.0
    PM_REG_WRITE(H_BK_CEC(0x0E), 0x00); // Physical address 0.0
    PM_REG_WRITE(H_BK_CEC(0x14), 0x00); // Device type: TV

    //(8) Clear CEC status
    PM_REG_WRITE(L_BK_CEC(0x11), 0x7F); // Clear CEC wakeup status
    PM_REG_WRITE(H_BK_CEC(0x12), 0x1F); // Clear RX/TX/RF/LA/NACK status status
    PM_REG_WRITE(H_BK_CEC(0x12), 0x00);
}

void mhal_CEC_Enabled(MS_BOOL bEnableFlag)
{
    if(bEnableFlag)
        PM_REG_WRITE(L_BK_CEC(0x01),0x80); //Enable PM CEC controller
    else
        PM_REG_WRITE(L_BK_CEC(0x01),0x00); //Disable PM CEC controller
}

MS_U8 mhal_CEC_TxStatus(void)
{
    return (PM_REG_READ(H_BK_CEC(0x11))&0x0E);
}

MS_BOOL mhal_CEC_Device_Is_Tx(void)
{
    return CEC_DEVICE_IS_SOURCE;
}
