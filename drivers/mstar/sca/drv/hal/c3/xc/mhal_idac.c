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
#if defined(MSOS_TYPE_LINUX_KERNEL)
#include <linux/wait.h>
#endif

#include "mhal_xc_chip_config.h"
#include "xc_hwreg_utility2.h"
#include "xc_Analog_Reg.h"


#include "mhal_idac.h"

void Hal_SC_iDAC_SetEnabled(MS_BOOL bEnabled)
{
	if (bEnabled)
	{
		MDrv_WriteByteMask(L_BK_iDAC(0x0), 0x11, 0x11);
	}
	else
	{
		MDrv_WriteByteMask(L_BK_iDAC(0x0), 0x0, 0x11);
	}
}
