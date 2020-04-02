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
////////////////////////////////////////////////////////////////////////////////

#ifndef _REGBDMA_H_
#define _REGBDMA_H_

////////////////////////////////////////////////////////////////////////////////
// Header Files
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Define & data type
////////////////////////////////////////////////////////////////////////////////
//v: value n: shift n bits
#define __BIT(x)    ((U8)(1UL << (x)))
#define __BIT0       __BIT(0UL)
#define __BIT1       __BIT(1UL)
#define __BIT2       __BIT(2UL)
#define __BIT3       __BIT(3UL)
#define __BIT4       __BIT(4UL)
#define __BIT5       __BIT(5UL)
#define __BIT6       __BIT(6UL)
#define __BIT7       __BIT(7UL)

#if defined(CONFIG_ARM) 
#define mstar_pm_base                0xFD000000//Use 8 bit addressing
#elif defined(CONFIG_ARM64)
extern ptrdiff_t mstar_pm_base;
#endif

//#define mstar_pm_base               0xFD000000
//#define REG_ADDR_BASE               mstar_pm_base + (0x122000UL<<1)

#define BDMA_REG_BASE               mstar_pm_base + (0x100900UL<<1)
#define BDMA_REG_CH0_BASE           BDMA_REG_BASE
#define BDMA_CH_REG_OFFSET          (0x40UL)
#define BDMA_SET_CH0_REG(x)         BDMA_REG_CH0_BASE+(x)
#define BDMA_SET_CH1_REG(x)         BDMA_SET_CH0_REG(x)+BDMA_CH_REG_OFFSET



/*#define BDMA_REG_CH0_CTRL           BDMA_SET_CH0_REG(0UL)
#define BDMA_REG_CH0_STATUS         BDMA_SET_CH0_REG(0x02UL)
#define BDMA_REG_CH0_SRC_SEL        BDMA_SET_CH0_REG(0x04UL)
#define BDMA_REG_CH0_DST_SEL        BDMA_SET_CH0_REG(0x05UL)
#define BDMA_REG_CH0_MISC           BDMA_SET_CH0_REG(0x06UL)
#define BDMA_REG_CH0_DWUM_CNT       BDMA_SET_CH0_REG(0x07UL)
#define BDMA_REG_CH0_SRC_ADDR_L     BDMA_SET_CH0_REG(0x08UL)
#define BDMA_REG_CH0_SRC_ADDR_H     BDMA_SET_CH0_REG(0x0AUL)
#define BDMA_REG_CH0_DST_ADDR_L     BDMA_SET_CH0_REG(0x0CUL)
#define BDMA_REG_CH0_DST_ADDR_H     BDMA_SET_CH0_REG(0x0EUL)
#define BDMA_REG_CH0_SIZE_L         BDMA_SET_CH0_REG(0x10UL)
#define BDMA_REG_CH0_SIZE_H         BDMA_SET_CH0_REG(0x12UL)
//Ch0 Special command 0
#define BDMA_REG_CH0_CMD0_L         BDMA_SET_CH0_REG(0x14UL)
#define BDMA_REG_CH0_CMD0_H         BDMA_SET_CH0_REG(0x16UL)
//Ch0 Special command 1
#define BDMA_REG_CH0_CMD1_L         BDMA_SET_CH0_REG(0x18UL)
#define BDMA_REG_CH0_CMD1_H         BDMA_SET_CH0_REG(0x1AUL)*/

#define BDMA_REG_CH0_CTRL           BDMA_SET_CH0_REG(0UL)
#define BDMA_REG_CH0_STATUS         BDMA_SET_CH0_REG(0x04UL)
#define BDMA_REG_CH0_SRC_SEL        BDMA_SET_CH0_REG(0x08UL)
#define BDMA_REG_CH0_DST_SEL        BDMA_SET_CH0_REG(0x09UL)
#define BDMA_REG_CH0_MISC           BDMA_SET_CH0_REG(0x0CUL)
#define BDMA_REG_CH0_DWUM_CNT       BDMA_SET_CH0_REG(0x0DUL)
//#define BDMA_REG_CH0_DWUM_CNT       BDMA_SET_CH0_REG(0x0EUL)
#define BDMA_REG_CH0_SRC_ADDR_L     BDMA_SET_CH0_REG(0x10UL)
#define BDMA_REG_CH0_SRC_ADDR_H     BDMA_SET_CH0_REG(0x14UL)
#define BDMA_REG_CH0_DST_ADDR_L     BDMA_SET_CH0_REG(0x18UL)
#define BDMA_REG_CH0_DST_ADDR_H     BDMA_SET_CH0_REG(0x1CUL)
#define BDMA_REG_CH0_SIZE_L         BDMA_SET_CH0_REG(0x20UL)
#define BDMA_REG_CH0_SIZE_H         BDMA_SET_CH0_REG(0x24UL)
//Ch0 Special command 0                                
#define BDMA_REG_CH0_CMD0_L         BDMA_SET_CH0_REG(0x28UL)
#define BDMA_REG_CH0_CMD0_H         BDMA_SET_CH0_REG(0x2CUL)
//Ch0 Special command 1                                
#define BDMA_REG_CH0_CMD1_L         BDMA_SET_CH0_REG(0x30UL)
#define BDMA_REG_CH0_CMD1_H         BDMA_SET_CH0_REG(0x34UL)



#define BDMA_REG_CH1_CTRL           BDMA_SET_CH1_REG(0UL)
#define BDMA_REG_CH1_STATUS         BDMA_SET_CH1_REG(0x04UL)
#define BDMA_REG_CH1_SRC_SEL        BDMA_SET_CH1_REG(0x08UL)
#define BDMA_REG_CH1_DST_SEL        BDMA_SET_CH1_REG(0x09UL)
#define BDMA_REG_CH1_MISC           BDMA_SET_CH1_REG(0x0CUL)
#define BDMA_REG_CH1_DWUM_CNT       BDMA_SET_CH1_REG(0x0EUL)
#define BDMA_REG_CH1_SRC_ADDR_L     BDMA_SET_CH1_REG(0x10UL)
#define BDMA_REG_CH1_SRC_ADDR_H     BDMA_SET_CH1_REG(0x14UL)
#define BDMA_REG_CH1_DST_ADDR_L     BDMA_SET_CH1_REG(0x18UL)
#define BDMA_REG_CH1_DST_ADDR_H     BDMA_SET_CH1_REG(0x1CUL)
#define BDMA_REG_CH1_SIZE_L         BDMA_SET_CH1_REG(0x20UL)
#define BDMA_REG_CH1_SIZE_H         BDMA_SET_CH1_REG(0x24UL)
//CH1 Special command 0
#define BDMA_REG_CH1_CMD0_L         BDMA_SET_CH1_REG(0x28UL)
#define BDMA_REG_CH1_CMD0_H         BDMA_SET_CH1_REG(0x2CUL)
//CH1 Special command 1
#define BDMA_REG_CH1_CMD1_L         BDMA_SET_CH1_REG(0x30UL)
#define BDMA_REG_CH1_CMD1_H         BDMA_SET_CH1_REG(0x34UL)
//---------------------------------------------
// definition for BDMA_REG_CH0_CTRL/BDMA_REG_CH1_CTRL
//---------------------------------------------
#define BDMA_CH_TRIGGER             __BIT0
#define BDMA_CH_STOP                __BIT4

//---------------------------------------------
// definition for REG_BDMA_CH0_STATUS/REG_BDMA_CH1_STATUS
//---------------------------------------------
#define BDMA_CH_QUEUED              __BIT0
#define BDMA_CH_BUSY                __BIT1
#define BDMA_CH_INT                 __BIT2
#define BDMA_CH_DONE                __BIT3
#define BDMA_CH_RESULT              __BIT4
#define BDMA_CH_CLEAR_STATUS        (BDMA_CH_INT|BDMA_CH_DONE|BDMA_CH_RESULT)
//---------------------------------------------
// definition for REG_BDMA_CH0_MISC/REG_BDMA_CH1_MISC
//---------------------------------------------
#define BDMA_CH_ADDR_DECDIR         __BIT0
#define BDMA_CH_DONE_INT_EN         __BIT1
#define BDMA_CH_CRC_REFLECTION      __BIT4

#endif
