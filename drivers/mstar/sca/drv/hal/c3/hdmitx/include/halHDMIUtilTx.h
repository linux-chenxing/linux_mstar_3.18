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
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    halHDMIUtilTx.h
/// @author MStar Semiconductor,Inc.
/// @brief  HDMITx Utility HAL
/////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _HAL_HDMIUTILTX_H_
#define _HAL_HDMIUTILTX_H_


#ifdef __cplusplus
extern "C"
{
#endif

//-------------------------------------------------------------------------------------------------
//  Hardware Capability
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#ifndef BIT0
#define BIT0  0x0001
#define BIT1  0x0002
#define BIT2  0x0004
#define BIT3  0x0008
#define BIT4  0x0010
#define BIT5  0x0020
#define BIT6  0x0040
#define BIT7  0x0080
#define BIT8  0x0100
#define BIT9  0x0200
#define BIT10 0x0400
#define BIT11 0x0800
#define BIT12 0x1000
#define BIT13 0x2000
#define BIT14 0x4000
#define BIT15 0x8000
#endif

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

typedef struct
{
    MS_U32      bank;
    MS_U16     address;
    MS_U16     mask;
    MS_U16     value;
} MSTHDMITX_REG_TYPE;

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

#ifdef MHAL_HDMIUTILTX_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

INTERFACE void MHal_HDMITx_SetIOMapBase(MS_U32 u32Base, MS_U32 u32PMBase);

INTERFACE MS_BOOL MHal_HDMITx_Rx74WriteByte(MS_U8 addr, MS_U8 value);

INTERFACE MS_BOOL MHal_HDMITx_Rx74ReadByte(MS_U8  addr, MS_U8 *pBuf);

INTERFACE MS_BOOL MHal_HDMITx_Rx74WriteBytes(MS_U8  addr, MS_U8  len, MS_U8  *buf);

INTERFACE MS_BOOL MHal_HDMITx_Rx74ReadBytes(MS_U8  addr, MS_U16  len, MS_U8  *buf);

INTERFACE MS_BOOL MHal_HDMITx_EdidReadBlock(MS_U8  num, MS_U8 *buf);

INTERFACE MS_U16 MHal_HDMITx_Read(MS_U32 bank, MS_U16 address);

INTERFACE void MHal_HDMITx_Write(MS_U32 bank, MS_U16 address, MS_U16 reg_data);

INTERFACE void MHal_HDMITx_Mask_Write(MS_U32 bank, MS_U16 address, MS_U16 reg_mask, MS_U16 reg_data);

INTERFACE void MHal_HDMITx_RegsTbl_Write(MSTHDMITX_REG_TYPE *pTable, MS_U8 num);

INTERFACE MS_U16 MHal_HDMITxPM_Read(MS_U32 bank, MS_U16 address);

INTERFACE void MHal_HDMITxPM_Write(MS_U32 bank, MS_U16 address, MS_U16 reg_data);

INTERFACE void MHal_HDMITxPM_Mask_Write(MS_U32 bank, MS_U16 address, MS_U16 reg_mask, MS_U16 reg_data);

// Read HDCP key from external EEPROM if not used internal HDCP key
INTERFACE MS_BOOL MHal_HDMITx_HDCPKeyReadByte(MS_U32 u32Addr, MS_U8 *pu8Buf, MS_U32 u32Size);

INTERFACE void MHal_HDMITx_UtilDebugEnable(MS_BOOL benable);

INTERFACE MS_U32 MHal_HDMITx_GetDDCDelayCount(void);
INTERFACE void MHal_HDMITx_SetDDCDelayCount(MS_U32 u32Delay);
INTERFACE MS_BOOL MHal_HDMITx_AdjustDDCFreq(MS_U32 u32Speed_K);


#ifdef __cplusplus
}
#endif

#endif // _HAL_HDMIUTILTX_H_

