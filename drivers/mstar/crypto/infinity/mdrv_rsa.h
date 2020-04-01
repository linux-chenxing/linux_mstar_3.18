/*
 *#############################################################################
 *
 * Copyright (c) 2006-2011 MStar Semiconductor, Inc.
 * All rights reserved.
 *
 * Unless otherwise stipulated in writing, any and all information contained
 * herein regardless in any format shall remain the sole proprietary of
 * MStar Semiconductor Inc. and be kept in strict confidence
 * (“MStar Confidential Information”) by the recipient.
 * Any unauthorized act including without limitation unauthorized disclosure,
 * copying, use, reproduction, sale, distribution, modification, disassembling,
 * reverse engineering and compiling of the contents of MStar Confidential
 * Information is unlawful and strictly prohibited. MStar hereby reserves the
 * rights to any and all damages, losses, costs and expenses resulting therefrom.
 *
 *#############################################################################
*/


#ifndef _IOCTL_TEST_H
#define _IOCTL_TEST_H

#include <linux/ioctl.h>

struct ioctl_arg {
        unsigned int reg;
        unsigned short val;
        unsigned int *pu32RSA_Sig;
        unsigned int *pu32RSA_KeyN;
        unsigned int *pu32RSA_KeyE;
        unsigned int *pu32RSA_Output;
        unsigned int u32RSA_KeyNLen;
        unsigned int u32RSA_SigLen;
        unsigned int u32RSA_KeyELen;
        bool bRSAHwKey;
        bool bRSAPublicKey;


        unsigned int *pu32aesKey;
};




/* 這裡要找一個沒用到的號碼，請參考 Documentation/ioctl/ioctl-number.txt */
#define IOC_MAGIC '\x66'
/* 您要的動作 */
#define MDrv_RSA_Reset                _IOWR(IOC_MAGIC, 1, struct ioctl_arg)
#define MDrv_RSA_Setkeylen            _IOWR(IOC_MAGIC, 2, struct ioctl_arg)
#define MDrv_RSA_Calculate            _IOWR(IOC_MAGIC, 3, struct ioctl_arg)
#define MDrv_AESDMA_SetKey            _IOWR(IOC_MAGIC, 4, struct ioctl_arg)
#define MDrv_AESDMA_SelEng            _IOWR(IOC_MAGIC, 5, struct ioctl_arg)
#define MDrv_AESDMA_Init              _IOWR(IOC_MAGIC, 6, struct ioctl_arg)
#define MDrv_AESDMA_SetFileInOut      _IOWR(IOC_MAGIC, 7, struct ioctl_arg)
#define MDrv_AESDMA_SetIV             _IOWR(IOC_MAGIC, 8, struct ioctl_arg)
#define MDrv_AESDMA_Start             _IOWR(IOC_MAGIC, 9, struct ioctl_arg)//enable
#define MDrv_AESDMA_IsFinished        _IOWR(IOC_MAGIC, 10, struct ioctl_arg)
#define MDrv_AESDMA_GetStatus         _IOWR(IOC_MAGIC, 11, struct ioctl_arg)
#define IOCTL_VAL_MAXNR 11

#endif