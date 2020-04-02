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

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define DRV_SCLVIP_HANDLER_PRE_FIX     0x12330000
#define DRV_SCLVIP_HANDLER_PRE_MASK    0xFFFF0000
#define DRV_SCLVIP_HANDLER_MAX         2



//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------
typedef struct
{
    s32 s32Handle;
    void *pPrivate_Data;
    DrvSclVipIoIdType_e enSclVipId;
} DrvSclVipHandleConfig_t;



//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------

#ifndef __DRV_SCL_VIP_IO_WRAPPER_C__
#define INTERFACE extern
#else
#define INTERFACE
#endif

INTERFACE void                 _DrvSclVipIoInitHandler(void);
INTERFACE s32                  _DrvSclVipIoOpen(DrvSclVipIoIdType_e enSlcVipId);
INTERFACE DrvSclVipIoErrType_e _DrvSclVipIoRelease(s32 s32Handler);
INTERFACE DrvSclVipIoErrType_e _DrvSclVipIoSetPeakingConfig(s32 s32Handler, DrvSclVipIoPeakingConfig_t *pstCfg);
INTERFACE DrvSclVipIoErrType_e _DrvSclVipIoSetDlcHistogramConfig(s32 s32Handler, DrvSclVipIoDlcHistogramConfig_t *pstCfg);
INTERFACE DrvSclVipIoErrType_e _DrvSclVipIoGetDlcHistogramConfig(s32 s32Handler, DrvSclVipIoDlcHistogramReport_t *pstCfg);
INTERFACE DrvSclVipIoErrType_e _DrvSclVipIoSetDlcConfig(s32 s32Handler, DrvSclVipIoDlcConfig_t *pstCfg);
INTERFACE DrvSclVipIoErrType_e _DrvSclVipIoSetLceConfig(s32 s32Handler, DrvSclVipIoLceConfig_t *pstCfg);
INTERFACE DrvSclVipIoErrType_e _DrvSclVipIoSetUvcConfig(s32 s32Handler, DrvSclVipIoUvcConfig_t *pstCfg);
INTERFACE DrvSclVipIoErrType_e _DrvSclVipIoSetIhcConfig(s32 s32Handler, DrvSclVipIoIhcConfig_t *pstCfg);
INTERFACE DrvSclVipIoErrType_e _DrvSclVipIoSetIccConfig(s32 s32Handler, DrvSclVipIoIccConfig_t *pstCfg);
INTERFACE DrvSclVipIoErrType_e _DrvSclVipIoSetIhcIceAdpYConfig(s32 s32Handler, DrvSclVipIoIhcIccConfig_t *pstCfg);
INTERFACE DrvSclVipIoErrType_e _DrvSclVipIoSetIbcConfig(s32 s32Handler, DrvSclVipIoIbcConfig_t *pstCfg);
INTERFACE DrvSclVipIoErrType_e _DrvSclVipIoSetFccConfig(s32 s32Handler, DrvSclVipIoFccConfig_t *pstCfg);
INTERFACE DrvSclVipIoErrType_e _DrvSclVipIoSetNlmConfig(s32 s32Handler, DrvSclVipIoNlmConfig_t *pstIoCfg);
INTERFACE DrvSclVipIoErrType_e _DrvSclVipIoSetAckConfig(s32 s32Handler, DrvSclVipIoAckConfig_t *pstCfg);
INTERFACE DrvSclVipIoErrType_e _DrvSclVipIoSetMcnrConfig(s32 s32Handler, DrvSclVipIoMcnrConfig_t *pstIoCfg);
INTERFACE DrvSclVipIoErrType_e _DrvSclVipIoSetAipConfig(s32 s32Handler, DrvSclVipIoAipConfig_t *pstIoConfig);
INTERFACE DrvSclVipIoErrType_e _DrvSclVipIoSetAipSramConfig(s32 s32Handler, DrvSclVipIoAipSramConfig_t *pstIoCfg);
INTERFACE DrvSclVipIoErrType_e _DrvSclVipIoCmdqWriteConfig(s32 s32Handler, DrvSclVipIoCmdqConfig_t *pstIoCfg);
INTERFACE DrvSclVipIoErrType_e _DrvSclVipIoSetLdcConfig(s32 s32Handler, DrvSclVipIoLdcConfig_t *pstCfg);
INTERFACE DrvSclVipIoErrType_e _DrvSclVipIoSetVipConfig(s32 s32Handler, DrvSclVipIoConfig_t *pstCfg);
INTERFACE DrvSclVipIoErrType_e _DrvSclVipIoSetLdcMdConfig(s32 s32Handler, DrvSclVipIoLdcMdConfig_t *pstCfg);
INTERFACE DrvSclVipIoErrType_e _DrvSclVipIoSetLdcDmapConfig(s32 s32Handler, DrvSclVipIoLdcDmapConfig_t *pstCfg);
INTERFACE DrvSclVipIoErrType_e _DrvSclVipIoSetLdcSramConfig(s32 s32Handler, DrvSclVipIoLdcSramConfig_t *pstCfg);
INTERFACE DrvSclVipIoErrType_e _DrvSclVipIoSetVtrackConfig(s32 s32Handler, DrvSclVipIoVtrackConfig_t *pstCfg);
INTERFACE DrvSclVipIoErrType_e _DrvSclVipIoSetVtrackOnOffConfig(s32 s32Handler, DrvSclVipIoVtrackOnOffConfig_t *pstCfg);
INTERFACE DrvSclVipIoErrType_e _DrvSclVipIoGetVersion(s32 s32Handler, DrvSclVipIoVersionConfig_t *pstCfg);
INTERFACE DrvSclVipIoErrType_e _DrvSclVipIoSetAllVip(s32 s32Handler, DrvSclVipIoAllSetConfig_t *pstIoCfg);


#undef INTERFACE
