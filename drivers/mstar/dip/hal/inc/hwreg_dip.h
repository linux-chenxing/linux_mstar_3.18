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

#ifndef _HWREG_DIP_H_
#define _HWREG_DIP_H_

//TrustZone
#define REG_TZPC_NONPM_BASE      0x123900UL
#define REG_TZPC_NONPM_DIP       (REG_TZPC_NONPM_BASE + (0x74<<1) )
#define REG_TZPC_NONPM_DWIN0     (REG_TZPC_NONPM_BASE + (0x79<<1) )

//CLOCK
#define REG_CLOCK_BASE                      (0x100B00UL)

#define REG_MFDEC_LINEBUFFER                (REG_CLOCK_BASE + (0x57<<1) )
#define MFDEC_MAIN_LB                       (BMASK(3:0))
#define ENABLE_MFDEC_LB                     (0)
#define MFDEC_SUB_LB                        (BMASK(7:4))


//MFDec
#define REG_MFDEC_BASE                      (0x112200UL)

#define MFDEC_ID0                           (0)
#define MFDEC_ID1                           (1)

#define REG_MFDEC_ENA(id)                   (REG_MFDEC_BASE + ((0x40<<1)*id) + (0x00<<1) )
#define MFDEC_ENABLE                        (BIT(0))
#define MFDEC_HMIRROR                       (BIT(1))
#define MFDEC_VMIRROR                       (BIT(2))
#define MFDEC_VP9_MODE                      (BIT(3))
#define MFDEC_HVD_MODE                      (BIT(4))
#define MFDEC_UNCOMP_MODE                   (BIT(5))
#define MFDEC_BYPASS_MODE                   (BIT(6))  // fixed
#define MFDEC_RIU_MODE                      (BIT(12))
#define MFDEC_RIU1_MODE                     (BIT(13))
#define MFDEC_SIM_MODE                     (BIT(14))
#define REG_MFDEC_ENA_MASK                  (MFDEC_RIU1_MODE|MFDEC_RIU_MODE|MFDEC_BYPASS_MODE|MFDEC_UNCOMP_MODE|MFDEC_HVD_MODE|MFDEC_VP9_MODE|MFDEC_VMIRROR|MFDEC_HMIRROR|MFDEC_ENABLE|MFDEC_SIM_MODE)

#define REG_MFDEC_FB_MIU(id)                (REG_MFDEC_BASE + ((0x40<<1)*id) + (0x01<<1) )
#define MFDEC_LUMA_FB_MIU                   (BMASK(1:0))
#define MFDEC_LUMA_FB_MIU_SHIFT             (0)
#define MFDEC_CHROMA_FB_MIU                 (BMASK(3:2))
#define MFDEC_CHROMA_FB_SHIFT               (2)
#define MFDEC_BITLEN_FB_MIU                 (BMASK(5:4))
#define MFDEC_BITLEN_FB_MIU_SHIFT           (4)
#define MFDEC_MIF0_BURST_LEN                (BMASK(11:8))
#define MFDEC_MIF0_BURST_SHIFT              (8)
#define MFDEC_MIF1_BURST_LEN                (BMASK(15:12))
#define MFDEC_MIF1_BURST_SHIFT              (12)

#define REG_MFDEC_X_START(id)               (REG_MFDEC_BASE + ((0x40<<1)*id) + (0x02<<1) )
#define MFDEC_X_START                       (BMASK(7:0))

#define REG_MFDEC_Y_START(id)               (REG_MFDEC_BASE + ((0x40<<1)*id) + (0x03<<1) )
#define MFDEC_Y_START                       (BMASK(11:0))

#define REG_MFDEC_H_SIZE(id)                (REG_MFDEC_BASE + ((0x40<<1)*id) + (0x04<<1) )
#define MFDEC_H_SIZE                        (BMASK(8:0))

#define REG_MFDEC_V_SIZE(id)                (REG_MFDEC_BASE + ((0x40<<1)*id) + (0x05<<1) )
#define MFDEC_V_SIZE                        (BMASK(11:0))

#define REG_MFDEC_FB_PITCH(id)              (REG_MFDEC_BASE + ((0x40<<1)*id) + (0x06<<1) )
#define MFDEC_FB_PITCH                      (BMASK(9:0))

#define REG_MFDEC_BITLEN_FB_PITCH(id)       (REG_MFDEC_BASE + ((0x40<<1)*id) + (0x07<<1) )
#define MFDEC_BITLEN_FB_FIXED_PAT_MASK      (BMASK(15:8))
#define MFDEC_BITLEN_FB_PITCH_MASK          (BMASK(7:0))

#define REG_MFDEC_LUMA_FB_BASE_L(id)        (REG_MFDEC_BASE + ((0x40<<1)*id) + (0x08<<1) )
#define MFDEC_LUMA_FB_BASE_L                (BMASK(15:0))
#define REG_MFDEC_LUMA_FB_BASE_H(id)        (REG_MFDEC_BASE + ((0x40<<1)*id) + (0x09<<1) )
#define MFDEC_LUMA_FB_BASE_H                (BMASK(12:0))
#define MFDEC_LUMA_FB_BASE_H_SHIFT          (12)

#define REG_MFDEC_CHROMA_FB_BASE_L(id)      (REG_MFDEC_BASE + ((0x40<<1)*id) + (0x0A<<1) )
#define MFDEC_CHROMA_FB_BASE_L              (BMASK(15:0))
#define REG_MFDEC_CHROMA_FB_BASE_H(id)      (REG_MFDEC_BASE + ((0x40<<1)*id) + (0x0B<<1) )
#define MFDEC_CHROMA_FB_BASE_H              (BMASK(12:0))

#define REG_MFDEC_BITLEN_FB_BASE_L(id)      (REG_MFDEC_BASE + ((0x40<<1)*id) + (0x0C<<1) )
#define MFDEC_BITLEN_FB_BASE_L              (BMASK(15:0))
#define REG_MFDEC_BITLEN_FB_BASE_H(id)      (REG_MFDEC_BASE + ((0x40<<1)*id) + (0x0D<<1) )
#define MFDEC_BITLEN_FB_BASE_H              (BMASK(12:0))

#define REG_MFDEC_CW_LEN2(id)               (REG_MFDEC_BASE + ((0x40<<1)*id) + (0x20<<1) )
#define MFDEC_CW_LEN2_MASK                  (BMASK(11:0))

#define REG_MFDEC_CW_LEN3(id)               (REG_MFDEC_BASE + ((0x40<<1)*id) + (0x21<<1) )
#define MFDEC_CW_LEN3_MASK                  (BMASK(11:0))

#define REG_MFDEC_CW_LEN4(id)               (REG_MFDEC_BASE + ((0x40<<1)*id) + (0x22<<1) )
#define MFDEC_CW_LEN4_MASK                  (BMASK(11:0))

#define REG_MFDEC_CW_LEN5(id)               (REG_MFDEC_BASE + ((0x40<<1)*id) + (0x23<<1) )
#define MFDEC_CW_LEN5_MASK                  (BMASK(11:0))

#define REG_MFDEC_CW_BASE(id)               (REG_MFDEC_BASE + ((0x40<<1)*id) + (0x24<<1) )
#define MFDEC_CW_BASE                       (BMASK(11:0))

#define REG_MFDEC_CW_MAX(id)                (REG_MFDEC_BASE + ((0x40<<1)*id) + (0x25<<1) )
#define MFDEC_CW_MAX                        (BMASK(11:0))

#define REG_MFDEC_SYMB_BASE(id)             (REG_MFDEC_BASE + ((0x40<<1)*id) + (0x26<<1) )
#define MFDEC_SYMB_BASE                     (BMASK(4:0))
#define MFDEC_SYMB_MAX                      (BMASK(12:8))

#define REG_MFDEC_HUF_TAB(id)               (REG_MFDEC_BASE + ((0x40<<1)*id) + (0x27<<1) )

#define REG_MFDEC_DBUS_L(id)                (REG_MFDEC_BASE + ((0x40<<1)*id) + (0x12<<1) )
#define REG_MFDEC_DBUS_H(id)                (REG_MFDEC_BASE + ((0x40<<1)*id) + (0x13<<1) )

#define MFDEC_X_START_ALIGN                 (16)
#define MFDEC_HSIZE_ALIGN                   (16)
#define MFDEC_ADDR_ALIGN                    (8)
#define MFDEC_FB_PITCH_ALIGN                (8)
#define MFEDC_TABLE_INDEX_COUNT             (48)


//COVER
#define REG_COVER_BASE                      (0x161800UL)

#define COVER_BWIN0                           (0)
#define COVER_BWIN1                           (1)
#define COVER_BWIN2                           (2)
#define COVER_BWIN3                           (3)

#define REG_COVER_ENABLE(id)             (REG_COVER_BASE + (0x10*id) + (0x08<<1) )

#define REG_COVER_BWIN_HSTART(id)              (REG_COVER_BASE + (0x10*id) + (0x09<<1) )
#define REG_COVER_BWIN_HEND(id)                 (REG_COVER_BASE + (0x10*id) + (0x0A<<1) )
#define REG_COVER_BWIN_VSTART(id)              (REG_COVER_BASE + (0x10*id) + (0x0B<<1) )
#define REG_COVER_BWIN_VEND(id)                 (REG_COVER_BASE + (0x10*id) + (0x0C<<1) )
#define REG_COVER_BWIN_COLOR_GR(id)         (REG_COVER_BASE + (0x10*id) + (0x0D<<1) )
#define REG_COVER_BWIN_COLOR_B(id)           (REG_COVER_BASE + (0x10*id) + (0x0E<<1) )


#endif
