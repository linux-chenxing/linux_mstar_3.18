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
//////////////////////////////////////////////////////////////////////////

// ATV Chips
#define CHIP_PAULO      100
#define CHIP_PAULO2     101
#define CHIP_LOLA       102
#define CHIP_LOMEO      103
#define CHIP_LATTE      104
#define CHIP_LASER      105
#define CHIP_LOPEZ      106
#define CHIP_RAPHAEL    107
#define CHIP_MUSE       108
#define CHIP_METIS      109

// DTV Chips
#define CHIP_NEPTUNE    302
#define CHIP_PLUTO      303
#define CHIP_TITANIA1   304
#define CHIP_TITANIA2   305
#define CHIP_TITANIA3   306
#define CHIP_TITANIA7   307

// Monitor Chips
#define CHIP_OMEGA      502

// Others
#define CHIP_CERAMAL    702
#define CHIP_MOSES      703

#define CHIP_TYPE       CHIP_TITANIA3

//////////////////////////////////////////////////////////////////////////

#define CHIP_GROUP_ATV1 0
#define CHIP_GROUP_ATV2 1
#define CHIP_GROUP_ATV3     2
#define CHIP_GROUP_DTV1     3
#define CHIP_GROUP_DTV2     4
#define CHIP_GROUP_DTV3     5
#define CHIP_GROUP_MONITOR1 6
#define CHIP_GROUP_CERAMAL  7
#define CHIP_GROUP_MOSES    8
#define CHIO_GROUP_DTV7      9


#if (CHIP_TYPE == CHIP_LOPEZ || CHIP_TYPE == CHIP_RAPHAEL || CHIP_TYPE == CHIP_MUSE)
    #define CHIP_GROUP      CHIP_GROUP_ATV1
#elif (CHIP_TYPE == CHIP_PAULO || CHIP_TYPE == CHIP_PAULO2 || CHIP_TYPE == CHIP_LOLA || CHIP_TYPE == CHIP_LOMEO || CHIP_TYPE == CHIP_LATTE || CHIP_TYPE == CHIP_LASER)
    #define CHIP_GROUP      CHIP_GROUP_ATV2
#elif (CHIP_TYPE == CHIP_METIS)
    #define CHIP_GROUP      CHIP_GROUP_ATV3
#elif (CHIP_TYPE == CHIP_PLUTO || CHIP_TYPE == CHIP_TITANIA1 || CHIP_TYPE == CHIP_NEPTUNE )
    #define CHIP_GROUP      CHIP_GROUP_DTV1
#elif (CHIP_TYPE == CHIP_TITANIA2)
    #define CHIP_GROUP      CHIP_GROUP_DTV2
#elif (CHIP_TYPE == CHIP_TITANIA3)
    #define CHIP_GROUP      CHIP_GROUP_DTV3
#elif (CHIP_TYPE == CHIP_OMEGA)
    #define CHIP_GROUP      CHIP_GROUP_MONITOR1
#elif (CHIP_TYPE == CHIP_CERAMAL)
    #define CHIP_GROUP      CHIP_GROUP_CERAMAL
#elif (CHIP_TYPE == CHIP_MOSES)
    #define CHIP_GROUP      CHIP_GROUP_MOSES
#elif (CHIP_TYPE == CHIP_TITANIA7 )
    #define CHIP_GROUP  CHIO_GROUP_DTV7
#else
#endif

//////////////////////////////////////////////////////////////////////////

#if (CHIP_GROUP == CHIP_GROUP_ATV1)
    #include "MsDlc_LIB_Group_ATV1.h"
#elif (CHIP_GROUP == CHIP_GROUP_ATV2)
    #include "MsDlc_LIB_Group_ATV2.h"
#elif (CHIP_GROUP == CHIP_GROUP_ATV3)
    #include "MsDlc_LIB_Group_ATV3.h"
#elif (CHIP_GROUP == CHIP_GROUP_DTV1)
    #include "MsDlc_LIB_Group_DTV1.h"
#elif (CHIP_GROUP == CHIP_GROUP_DTV2)
    #include "MsDlc_LIB_Group_DTV2.h"
#elif (CHIP_GROUP == CHIP_GROUP_DTV3)
    #include "MsDlc_LIB_Group_DTV3.h"
#elif (CHIP_GROUP == CHIP_GROUP_MONITOR1)
    #include "MsDlc_LIB_Group_MONITOR1.h"
#elif (CHIP_GROUP == CHIP_GROUP_CERAMAL)
    #include "MsDlc_LIB_Group_Ceramal.h"
#elif (CHIP_GROUP == CHIP_GROUP_MOSES)
    #include "MsDlc_LIB_Group_Moses.h"
#else
#endif
