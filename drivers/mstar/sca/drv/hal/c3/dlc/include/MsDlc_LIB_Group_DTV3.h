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
#ifndef _MSDLC_LIB_GROUP_DTV2_H_
#define _MSDLC_LIB_GROUP_DTV2_H_


#ifdef IOUTIL_C
#define INTERFACE
#else
#define    INTERFACE extern
#endif

#include "drvDLC.h"
#include "dlc_hwreg_utility2.h"
#include "MsCommon.h"
#include "MsIRQ.h"
#include "MsOS.h"
#include "drvDLC_Cus.h"

/////////////// Chakra2 define start
#ifndef BOOL
#define BOOL    MS_BOOL
#else
#undef BOOL
#define BOOL    MS_BOOL
#endif

#ifndef BYTE
#define BYTE    MS_U8
#else
#undef BYTE
#define BYTE    MS_U8

#endif

#ifndef WORD
#define WORD    MS_U16
#else
#undef WORD
#define WORD    MS_U16
#endif

#ifndef DWORD
#define DWORD   MS_U32
#else
#undef DWORD
#define DWORD   MS_U32
#endif

#define XDATA
#define code

#define MAIN_WINDOW     0
#define SUB_WINDOW      1


#define REG_BANK_DLC                0x1A
#define BK_REG_L( x, y )            ((x) | (((y) << 1)))
#define BK_REG_H( x, y )            (((x) | (((y) << 1))) + 1)
#if SCALER_REGISTER_SPREAD
#define L_BK_DLC(x)                 BK_REG_L((REG_BANK_DLC << 8),x)
#define H_BK_DLC(x)                 BK_REG_H((REG_BANK_DLC << 8),x)
#else
#define L_BK_DLC(x)                 BK_REG_L(BK_SCALER_BASE,x)
#define H_BK_DLC(x)                 BK_REG_H(BK_SCALER_BASE,x)
#endif
//////////////// Chakra2 define end

#define _END_OF_TBL_    0xFFFF

#define _BIT0           0x0001
#define _BIT1           0x0002
#define _BIT2           0x0004
#define _BIT3           0x0008
#define _BIT4           0x0010
#define _BIT5           0x0020
#define _BIT6           0x0040
#define _BIT7           0x0080
#define _ENABLE             1
#define _DISABLE            0
#define BYTE            MS_U8
//////////////////////////////////////////////////////////////////////////
// Common Definition
typedef struct _DlcRegUnitType
{
    WORD wReg;
    BYTE ucValue;
}DlcRegUnitType;

#define ENABLE_HISTOGRAM_8                  1

//#define BOOL    BOOLEAN

#define msReadByte(_reg_)                   MDrv_ReadByte(_reg_)
#define msWriteByte(_reg_, _val_)           MDrv_WriteByte(_reg_, _val_)
#define msWriteBit(_reg_, _val_, _pos_)     MDrv_WriteRegBit(_reg_, _val_, _pos_)
#define msDlc_Delay1ms(x)                   MsOS_DelayTask(x)

#define REG_NULL        0xFF // empty register


//////////////////////////////////////////////////////////////////////////
#define REG_ADDR_HISTOGRAM_RANGE_M_HST      L_BK_DLC(0x1A)
#define REG_ADDR_HISTOGRAM_RANGE_M_HEN      H_BK_DLC(0x1A)
#define REG_ADDR_HISTOGRAM_RANGE_M_VST      L_BK_DLC(0x01)
#define REG_ADDR_HISTOGRAM_RANGE_M_VEN      H_BK_DLC(0x01)
#define REG_ADDR_HISTOGRAM_RANGE_S_HST      L_BK_DLC(0x1B)
#define REG_ADDR_HISTOGRAM_RANGE_S_HEN      H_BK_DLC(0x1B)
#define REG_ADDR_HISTOGRAM_RANGE_S_VST      L_BK_DLC(0x03)
#define REG_ADDR_HISTOGRAM_RANGE_S_VEN      H_BK_DLC(0x03)
#define REG_ADDR_DLC_HANDSHAKE              L_BK_DLC(0x04)
#define REG_ADDR_HISTOGRAM_TOTAL_SUM_L      L_BK_DLC(0x06)
#define REG_ADDR_HISTOGRAM_TOTAL_SUM_H      H_BK_DLC(0x06)
#define REG_ADDR_HISTOGRAM_TOTAL_COUNT_L    L_BK_DLC(0x07)
#define REG_ADDR_HISTOGRAM_TOTAL_COUNT_H    H_BK_DLC(0x07)
#define REG_ADDR_HISTOGRAM_RANGE_ENABLE     L_BK_DLC(0x08)
#define REG_ADDR_HISTOGRAM_8_RANGE_START    L_BK_DLC(0x1C)
#define REG_ADDR_BLE_UPPER_BOND             L_BK_DLC(0x09)
#define REG_ADDR_BLE_LOWER_BOND             H_BK_DLC(0x09)
#define REG_ADDR_WLE_UPPER_BOND             L_BK_DLC(0x0A)
#define REG_ADDR_WLE_LOWER_BOND             H_BK_DLC(0x0A)
#define REG_ADDR_MAIN_MAX_VALUE             L_BK_DLC(0x0B)
#define REG_ADDR_MAIN_MIN_VALUE             H_BK_DLC(0x0B)
#define REG_ADDR_SUB_MAX_VALUE              L_BK_DLC(0x0C)
#define REG_ADDR_SUB_MIN_VALUE              H_BK_DLC(0x0C)
#define REG_ADDR_DLC_DATA_START_MAIN        L_BK_DLC(0x30)
#define REG_ADDR_DLC_DATA_START_SUB         L_BK_DLC(0x38)
#define REG_ADDR_DLC_DATA_EXTEND_N0_MAIN    L_BK_DLC(0x76)
#define REG_ADDR_DLC_DATA_EXTEND_16_MAIN    L_BK_DLC(0x77)
#define REG_ADDR_DLC_DATA_EXTEND_N0_SUB     L_BK_DLC(0x7E)
#define REG_ADDR_DLC_DATA_EXTEND_16_SUB     L_BK_DLC(0x7F)
#define REG_ADDR_DLC_DATA_LSB_START_MAIN    L_BK_DLC(0x78)
#define REG_ADDR_DLC_DATA_LSB_START_SUB     L_BK_DLC(0x7B)
#define REG_ADDR_HISTOGRAM_DATA_32          L_BK_DLC(0x40)
#if ENABLE_HISTOGRAM_8
#define REG_ADDR_HISTOGRAM_DATA_8           L_BK_DLC(0x28)
#endif
#define REG_ADDR_DLC_Y_GAIN                 L_BK_DLC(0x14)
#define REG_ADDR_DLC_C_GAIN                 H_BK_DLC(0x14)

#if SCALER_REGISTER_SPREAD
#define msDlc_FunctionEnter()

#define msDlc_FunctionExit()
#else
#define msDlc_FunctionEnter()               BYTE  u8Bank; \
                                            u8Bank = msReadByte(BK_SCALER_BASE); \
                                            msWriteByte(BK_SCALER_BASE, REG_BANK_DLC);// for register bank switch...

#define msDlc_FunctionExit()                msWriteByte(BK_SCALER_BASE, u8Bank)// for register bank switch...
#endif

#ifdef _MSDLC_C_
code DlcRegUnitType tDLC_Initialize[] =
{
    {_END_OF_TBL_, 0x00}
};
#else
extern code DlcRegUnitType tDLC_Initialize[];
#endif

//////////////////////////////////////////////////////////////////////////
// Align to msdlc library
#ifdef _MSDLC_C_
    typedef void   (*MApi_XC_DLC_Print_Callback)(MS_U8 /*PWM_VALUE*/);


#if defined(MSOS_TYPE_CE)
    __declspec(dllimport)  MS_U16                       gu16PNL_Width;
    __declspec(dllimport)  MS_U16                       gu16PNL_Height;
    __declspec(dllimport)  MApi_XC_DLC_Print_Callback   gfnPutchar;
    __declspec(dllimport)  StuDbc_CAPTURE_Range         g_DlcCapRange;
#else
    extern  MS_U16                       gu16PNL_Width;
    extern  MS_U16                       gu16PNL_Height;
    extern  MApi_XC_DLC_Print_Callback   gfnPutchar;
    extern  StuDbc_CAPTURE_Range         g_DlcCapRange;
#endif

    #ifdef  putchar
    #undef  putchar
    #endif

    #define putchar(x)                   (gfnPutchar(x))
    #define msDlc_AP_ReinitWithoutPara() msDlcInitWithCurve( g_DlcCapRange.wHStart, g_DlcCapRange.wHEnd, gu16PNL_Height*1/8, gu16PNL_Height*7/8)//g_DlcCapRange.wVStart, g_DlcCapRange.wVEnd )
    #define Delay1ms(x)                  (MsOS_DelayTask(x))
#endif //_MSDLC_C_

#endif

