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

#ifndef _MDRV_VIF_IO_H
#define _MDRV_VIF_IO_H

//=============================================================================
// Includs
//=============================================================================

//=============================================================================
// Defines
//=============================================================================
#define MAX_LEN (64)

/* TVFE ioctl command structure */
typedef union VIF_IOCTL_CMD {
//DEV control
        struct {
            uint32_t u32VifDev;
            MHal_VIF_DevCfg_t pstDevAttr;
        }MCU_DevSetConfig;

        struct {
            uint32_t u32VifDev;
        }MCU_DevEnable;

        struct {
            uint32_t u32VifDev;
        }MCU_DevDisable;

//Channel control
        struct {
            uint32_t u32VifChn;
            MHal_VIF_ChnCfg_t pstAttr;
        }MCU_ChnSetConfig;

        struct {
            uint32_t u32VifChn;
        }MCU_ChnEnable;

        struct {
             uint32_t u32VifChn;
        }MCU_ChnDisable;

        struct {
            uint32_t u32VifChn;
            MHal_VIF_ChnStat_t pstStat;
        }MCU_ChnQuery;

//Sub Channel control
        struct {
            uint32_t u32VifChn;
            MHal_VIF_SubChnCfg_t pstAttr;
        }MCU_SubChnSetConfig;

        struct {
            uint32_t u32VifChn;
        }MCU_SubChnEnable;

        struct {
            uint32_t u32VifChn;
        }MCU_SubChnDisable;

        struct {
            uint32_t u32VifChn;
            MHal_VIF_ChnStat_t pstStat;
        }MCU_SubChnQuery;

//Ring buffer Control
        struct {
            uint32_t u32VifChn;
            MHal_VIF_PORT u32ChnPort;
            MHal_VIF_RingBufElm_t ptFbInfo;
        }MCU_Queue_Frame;

        struct {
            uint32_t u32VifChn;
            MHal_VIF_PORT u32ChnPort;
            uint32_t pNumBuf;
        }MCU_Query_Frame;

        struct {
            uint32_t u32VifChn;
            MHal_VIF_PORT u32ChnPort;
            MHal_VIF_RingBufElm_t ptFbInfo;
        }MCU_DeQueue_Frame;

}__attribute__((packed)) VIF_CMD_st;


#define VIF_IOCTL_MAGIC 'V'

#define IOCTL_VIF_MCU_DEV_SET_CFG       _IOW(VIF_IOCTL_MAGIC, 0x1, VIF_CMD_st)
#define IOCTL_VIF_MCU_DEV_ENABLE        _IOW(VIF_IOCTL_MAGIC, 0x2, VIF_CMD_st)
#define IOCTL_VIF_MCU_DEV_DISABLE       _IOW(VIF_IOCTL_MAGIC, 0x3, VIF_CMD_st)

#define IOCTL_VIF_MCU_CHN_SET_CFG       _IOW(VIF_IOCTL_MAGIC, 0x4, VIF_CMD_st)
#define IOCTL_VIF_MCU_CHN_ENABLE        _IOW(VIF_IOCTL_MAGIC, 0x5, VIF_CMD_st)
#define IOCTL_VIF_MCU_CHN_DISABLE       _IOW(VIF_IOCTL_MAGIC, 0x6, VIF_CMD_st)
#define IOCTL_VIF_MCU_CHN_QUERY         _IOW(VIF_IOCTL_MAGIC, 0x7, VIF_CMD_st)

#define IOCTL_VIF_SUB_MCU_CHN_SET_CFG   _IOW(VIF_IOCTL_MAGIC, 0x8, VIF_CMD_st)
#define IOCTL_VIF_SUB_MCU_CHN_ENABLE    _IOW(VIF_IOCTL_MAGIC, 0x9, VIF_CMD_st)
#define IOCTL_VIF_SUB_MCU_CHN_DISABLE   _IOW(VIF_IOCTL_MAGIC, 0xa, VIF_CMD_st)
#define IOCTL_VIF_SUB_MCU_CHN_QUERY     _IOW(VIF_IOCTL_MAGIC, 0xb, VIF_CMD_st)

#define IOCTL_VIF_MCU_QUEUE_FRAME       _IOW(VIF_IOCTL_MAGIC, 0xc, VIF_CMD_st)
#define IOCTL_VIF_MCU_QUERY_FRAME       _IOR(VIF_IOCTL_MAGIC, 0xd, VIF_CMD_st)
#define IOCTL_VIF_MCU_DEQUEUE_FRAME     _IOR(VIF_IOCTL_MAGIC, 0xe, VIF_CMD_st)

#define IOCTL_VIF_MCU_TEST_FLOW         _IOR(VIF_IOCTL_MAGIC, 0xff, VIF_CMD_st)
#endif 
