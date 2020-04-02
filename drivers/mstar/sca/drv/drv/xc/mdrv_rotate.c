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
#define  MDRV_ROTATE_C

#if defined(MSOS_TYPE_LINUX_KERNEL)
#include <linux/wait.h>
#endif

#include "mhal_xc_chip_config.h"
#include "xc_hwreg_utility2.h"
#include "xc_Analog_Reg.h"
#include "drvXC_IOPort.h"
#include "apiXC.h"
#include "mvideo_context.h"
#include "drv_sc_ip.h"
#include "mhal_sc.h"

#if defined(CHIP_C3)

#include "mhal_rotate.h"

void MApi_XC_Rotate_Config(XC_ROT_CONFIG_st stCfg)
{
    Hal_ROT_set_size(stCfg.u16Width, stCfg.u16Height);
    Hal_ROT_set_fmt(stCfg.eSrcFmt);
    Hal_ROT_set_degree(stCfg.eDegree);
    Hal_ROT_set_src_base(stCfg.u32SrcBaseAddr);
    Hal_ROT_set_dest_base(stCfg.u32DstBaseAddr);
    Hal_ROT_set_imi_base(stCfg.u32IMIBaseAddr0, stCfg.u32IMIBaseAddr1);
    Hal_ROT_set_src_pitch(stCfg.u16SrcPitch);
    Hal_ROT_set_dest_pitch(stCfg.u16DstPitch);

    MDrv_WriteByteMask(0x101E84, 0x0100, 0x0100);
    MDrv_WriteByteMask(0x101E8C, 0x0000, 0x0100);
    MDrv_WriteByteMask(0x101286, 0x0000, 0x0400);

}

void MApi_XC_Rotate_Trig(MS_BOOL bEn)
{
    Hal_ROT_set_trig(bEn);
}


void MApi_XC_Rotate_Set_EMI_WriteProtect(MS_BOOL bEn, MS_U32 u32Addr)
{
    Hal_ROT_set_emi_wirteprotect(bEn, u32Addr);
}

void MApi_XC_Rotate_Set_IMI_WriteProtect_Base0(MS_BOOL bEn, MS_U32 u32Addr)
{
    Hal_ROT_set_imi_wirteprotect_base0(bEn, u32Addr);
}

void MApi_XC_Rotate_Set_IMI_WriteProtect_Base1(MS_BOOL bEn, MS_U32 u32Addr)
{
    Hal_ROT_set_imi_wirteprotect_base1(bEn, u32Addr);
}
#else

void MApi_XC_Rotate_Config(XC_ROT_CONFIG_st stCfg)
{
    UNUSED(stCfg);
}

void MApi_XC_Rotate_Trig(MS_BOOL bEn)
{
    UNUSED(bEn);
}

void MApi_XC_Rotate_Set_EMI_WriteProtect(MS_BOOL bEn, MS_U32 u32Addr)
{
    UNUSED(bEn);
    UNUSED(u32Addr);
}
void MApi_XC_Rotate_Set_IMI_WriteProtect_Base0(MS_BOOL bEn, MS_U32 u32Addr)
{
    UNUSED(bEn);
    UNUSED(u32Addr);
}

void MApi_XC_Rotate_Set_IMI_WriteProtect_Base1(MS_BOOL bEn, MS_U32 u32Addr)
{
    UNUSED(bEn);
    UNUSED(u32Addr);
}

#endif

#undef  MDRV_ROTATE_C
