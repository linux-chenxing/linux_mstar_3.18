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

#ifndef _MSDBC_LIB_GROUP_DTV2_H_
#define _MSDBC_LIB_GROUP_DTV2_H_


#ifdef IOUTIL_C
#define INTERFACE
#else
#define    INTERFACE extern
#endif

#include "dlc_hwreg_utility2.h"
#include "drvDBC_Cus.h"


/////////////// Chakra2 define start

#ifndef BOOL
#define BOOL    MS_BOOL
#else
#undef  BOOL
#define BOOL    MS_BOOL
#endif

#ifndef BYTE
#define BYTE    MS_U8
#else
#undef  BYTE
#define BYTE    MS_U8

#endif

#ifndef WORD
#define WORD    MS_U16
#else
#undef  WORD
#define WORD    MS_U16
#endif

#ifndef DWORD
#define DWORD   MS_U32
#else
#undef  DWORD
#define DWORD   MS_U32
#endif

#ifndef U8
#define U8      MS_U8
#else
#undef  U8
#define U8      MS_U8
#endif

#define XDATA
#define code

#define _BIT0           0x0001
#define _BIT1           0x0002
#define _BIT2           0x0004
#define _BIT3           0x0008
#define _BIT4           0x0010
#define _BIT5           0x0020
#define _BIT6           0x0040
#define _BIT7           0x0080

#define SC1_REG_BASE                BK_SCALER_BASE
#define REG_BANK_DLC                0x1A
#define BK_REG_L( x, y )            ((x) | (((y) << 1)))
#define BK_REG_H( x, y )            (((x) | (((y) << 1))) + 1)
#define L_BK_DLC(x)                 BK_REG_L(BK_SCALER_BASE,x)
#define H_BK_DLC(x)                 BK_REG_H(BK_SCALER_BASE,x)

//////////////// Chakra2 define end


/////#define BOOL    BOOLEAN

#define msReadByte(_reg_)                   MDrv_ReadByte(_reg_)
#define msWriteByte(_reg_, _val_)           MDrv_WriteByte(_reg_, _val_)
#define msWriteBit(_reg_, _val_, _pos_)     MDrv_WriteRegBit(_reg_, _val_, _pos_)

#define REG_NULL        0xFF // empty register




//////////////////////////////////////////////////////////////////////////
// Common Definition
//typedef struct _DBCRegUnitType
//{
//    WORD wReg;
//    BYTE ucValue;
//}DBCRegUnitType;


//////////////////////////////////////////////////////////////////////////
#define REG_ADDR_DBC_Y_GAIN              L_BK_DLC(0x14) //SC1A_28 (8bit)
#define REG_ADDR_DBC_C_GAIN              H_BK_DLC(0x14) //SC1A_29 (8bit)


#define msDBC_FunctionEnter()               BYTE  u8Bank; \
                                            u8Bank = msReadByte(SC1_REG_BASE); \
                                            msWriteByte(SC1_REG_BASE, REG_BANK_DLC);// for register bank switch...

#define msDBC_FunctionExit()                msWriteByte(SC1_REG_BASE, u8Bank)// for register bank switch...


//code DBCRegUnitType tDBC_Initialize[] =
//{
//    {_END_OF_TBL_, 0x00}
//};
#endif

