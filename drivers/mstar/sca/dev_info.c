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

#define DEV_INFO_C


#include "dev_info.h"


DEVINFO_BOOT_TYPE ms_devinfo_boot_type(void)
{
	return DEVINFO_BOOT_TYPE_SPI;
}

DEVINFO_PANEL_TYPE ms_devinfo_panel_type(void)
{
	return DEVINFO_PANEL_070_DEFAULT;
}

DEVINFO_BOARD_TYPE ms_devinfo_board_type(void)
{
    return DEVINFO_BD_MST154A_D01A_S;
}


unsigned char ms_devinfo_rtk_qst_cfg(void)
{
    return DEVINFO_RTK_FLAG_0;
}

DEVINFO_RTK_FLAG ms_devinfo_rtk_flag(void)
{
    return DEVINFO_RTK_FLAG_0;
}

