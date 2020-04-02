///////////////////////////////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//
// @file   mdrv_sca_io.c
// @brief  SCA KMD Driver Interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////

//=============================================================================
// Include Files
//=============================================================================
#ifndef DEV_INFO_H
#define DEV_INFO_H

typedef enum
{
	DEVINFO_BOOT_TYPE_SPI=0x01,
	DEVINFO_BOOT_TYPE_EMMC=0x02,
	DEVINFO_BOOT_TYPE_8051=0x03,
}DEVINFO_BOOT_TYPE;

typedef enum
{
    DEVINFO_PANEL_070_DEFAULT    = 0x0,
    DEVINFO_PANEL_HSD_070I_DW1   = 0x01,
    DEVINFO_PANEL_HSD_070P_FW3   = 0x02,
    DEVINFO_PANEL_AT_102_03TT91  = 0x03,
    DEVINFO_PANEL_HSD_062I_DW1   = 0x04,
    DEVINFO_PANEL_HSD_070I_DW2   = 0x05,
    DEVINFO_PANEL_HSD_LVDS_800480= 0x06,
    DEVINFO_PANEL_N070ICG_LD1     =0x08,
    DEVINFO_PANEL_HSD_062I_DW2   = 0x09,
    DEVINFO_PANEL_HSD_070I_MODE2 = 0x0a,
    E_PANEL_UNKNOWN           = 0xFF
}DEVINFO_PANEL_TYPE;


typedef enum
{
    DEVINFO_BD_MST154A_D01A_S     = 0x0801,
    DEVINFO_BD_MST786_SZDEMO      = 0x0802,
    DEVINFO_BD_MST786_CUSTOM10    = 0x0810,
    DEVINFO_BD_MST786_CUSTOM20    = 0x0820,
    DEVINFO_BD_MST786_CUSTOM30    = 0x0830,
    E_BD_UNKNOWN           = 0xFFFF
}DEVINFO_BOARD_TYPE;

typedef enum
{
    DEVINFO_RTK_FLAG_0     = 0x0,
    DEVINFO_RTK_FLAG_1     = 0x01,
    E_RTK_UNKNOWN           = 0xFFFF
}DEVINFO_RTK_FLAG;

DEVINFO_BOOT_TYPE ms_devinfo_boot_type(void);
DEVINFO_PANEL_TYPE ms_devinfo_panel_type(void);
DEVINFO_BOARD_TYPE ms_devinfo_board_type(void);
unsigned char ms_devinfo_rtk_qst_cfg(void);
DEVINFO_RTK_FLAG ms_devinfo_rtk_flag(void);

#endif
