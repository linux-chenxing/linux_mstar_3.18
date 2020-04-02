////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2011 MStar Semiconductor, Inc.
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

#ifndef _MDRV_POLLSAMPLE_IO_H_
#define _MDRV_POLLSAMPLE_IO_H_

////////////////////////////////////////////////////////////////////////////////
// Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cam_os_util_ioctl.h>
////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

#define MAGIC_POLLSAMPLE              ('p')
#define IOCTL_POLLSAMPLE_START_TIMER  CAM_OS_IOR(MAGIC_POLLSAMPLE, 0, unsigned int)
#define IOCTL_POLLSAMPLE_GET_TIME     CAM_OS_IOR(MAGIC_POLLSAMPLE, 1, unsigned int)

#endif//_MDRV_POLLSAMPLE_IO_H_

