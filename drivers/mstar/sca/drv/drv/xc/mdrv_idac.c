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
#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/wait.h>
#endif

#include "mdrv_idac.h"
#include "MsCommon.h"

#if defined(CHIP_C3)
#include "mhal_idac.h"
#endif

void MDrv_SC_iDAC_SetEnabled(MS_BOOL bEnabled)
{
	#if defined(CHIP_C3)
	Hal_SC_iDAC_SetEnabled(bEnabled);
	#endif
}