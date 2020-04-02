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

/**
* @file    drv_scl.c
* @version
* @brief   scl driver
*
*/

#define DRV_SCL_C__
/*=============================================================*/
// Include files
/*=============================================================*/
//#include "sys_traces.ho"
#include <string.h>
#include <stdarg.h>
#include "kernel.h"

#include "sys_sys_isw_uart.h"

#include "drv_scl_os.h"
#include "drv_scl_dbg.h"
#include "hal_scl.h"
#include "hal_int_ctrl_pub.h"
#include "drv_scl_irq_st.h"
#include "drv_scl_irq.h"
/*=============================================================*/
// Macro definition
/*=============================================================*/


/*=============================================================*/
// Data type definition
/*=============================================================*/

/*=============================================================*/
// Variable definition
/*=============================================================*/
u8 gbdbgmessage[EN_DBGMG_NUM_CONFIG];

/*=============================================================*/
// Local function definition
/*=============================================================*/

bool DrvSclInit(void)
{
    return TRUE;
}



// Test Function
u16 DrvSclRegisterRead(u32 u32Addr)
{
    return HalSclRegisterRead(u32Addr);
}

// Test Function
void DrvSclRegisterWrite(u32 u32Addr, u16 u16Val)
{
    HalSclRegisterWrite(u32Addr, u16Val);
}
