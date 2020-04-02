///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2017 MStar Semiconductor, Inc.
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
// @file   mdrv_mipi_dsi_io_st.h
// @brief  MIPI TX DSI KMD Driver Interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_MIPI_DSI_IO_ST_H
#define _MDRV_MIPI_DSI_IO_ST_H

//=============================================================================
// enum
//=============================================================================


//=============================================================================
// struct
//=============================================================================

// Porting from hal_mipi_dsi.h
typedef enum
{
	DSI_CMD_MODE = 0,
	DSI_SYNC_PULSE_MODE,
	DSI_SYNC_EVENT_MODE,
	DSI_BURST_MODE
} DSI_MODE_CTL;

typedef enum
{
	DSI_LANE_DIS = 0,
	DSI_LANE_NUM_1 = 1,
	DSI_LANE_NUM_2,
	DSI_LANE_NUM_3,
	DSI_LANE_NUM_4
} DSI_TOTAL_LANE_NUM;

typedef enum
{
    DSI_FORMAT_RGB565,
    DSI_FORMAT_RGB666,
    DSI_FORMAT_LOOSELY_RGB666,
    DSI_FORMAT_RGB888
} DSI_FORMAT;

typedef struct
{
    u8 HS_TRAIL;
    u8 HS_PRPR;
    u8 HS_ZERO;
    u8 CLK_HS_PRPR;
    u8 CLK_HS_EXIT;
    u8 CLK_TRAIL;
    u8 CLK_ZERO;
    u8 CLK_HS_POST;
    u8 DA_HS_EXIT;
    u8 CONT_DET;

    u8 LPX;
    u8 TA_GET;
    u8 TA_SURE;
    u8 TA_GO;

    s8 pll_div2;
    s8 pll_div1;
    s8 fbk_sel;
    s8 fbk_div;

    u32 u32HActive;
    u32 u32HPW;
    u32 u32HBP;
    u32 u32HFP;
    u32 u32VActive;
    u32 u32VPW;
    u32 u32VBP;
    u32 u32VFP;
    u32 u32BLLP; // only for burst mode
    u8 u8FPS;
    u8 LANE_NUM;
    DSI_FORMAT format;
    DSI_MODE_CTL vdo_mode;
}LCM_PARAMS;

typedef struct
{
    LCM_PARAMS tLcmParams;
    u32 nPacketDataLen;
    u32 nPacketDataAddr;
} MipiDsiConfig_t, *pMipiDsiConfig;

typedef struct
{
    u32 nRegAddr;
    u32 nRegVal;
    u32 nRegMask;
} MipiDsiReg_t, *pMipiDsiReg;
//=============================================================================

//=============================================================================



#endif //_MDRV_MIPI_DSI_IO_ST_H



