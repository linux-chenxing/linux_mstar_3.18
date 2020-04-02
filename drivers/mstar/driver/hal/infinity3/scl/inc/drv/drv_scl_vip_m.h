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

#ifndef _MDRV_VIP_H
#define _MDRV_VIP_H


//-------------------------------------------------------------------------------------------------
//  Defines & Structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------

#ifndef _MDRV_VIP_C
#define INTERFACE extern
#else
#define INTERFACE
#endif
INTERFACE bool MDrvSclVipInit(MDrvSclVipInitConfig_t *pCfg);
INTERFACE void MDrvSclVipResume(MDrvSclVipInitConfig_t *pCfg);
INTERFACE void MDrvSclVipFreeMemory(void);
INTERFACE MDrvSclVipSetPqConfig_t MDrvSclVipFillBasicStructSetPqCfg(MDrvSclVipConfigType_e enVIPtype,void *pPointToCfg);
INTERFACE void MDrvSclVipCheckEachIPByCmdqIst(void);
INTERFACE bool MDrvSclVipGetIsBlankingRegion(void);
INTERFACE bool MDrvSclVipSysInit(MDrvSclVipInitConfig_t *pCfg);
INTERFACE void MDrvSclVipDelete(bool bCloseISR);
INTERFACE void MDrvSclVipRelease(void);
INTERFACE void MDrvSclVipSupend(void);
INTERFACE bool MDrvSclVipGetCmdqHwDone(void);
INTERFACE void * MDrvSclVipGetWaitQueueHead(void);

INTERFACE bool MDrvSclVipSetPollWait(DrvSclOsPollWaitConfig_t stPollWait);

INTERFACE void MDrvSclVipResetEachIP(void);
INTERFACE void MDrvSclVipSetAllVipOneShot(MDrvSclVipSuspendConfig_t *stvipCfg);
INTERFACE void MDrvSclVipSuspendResetFlagInit(void);
INTERFACE bool MDrvSclVipSetPeakingConfig(void *pCfg);
INTERFACE bool MDrvSclVipSetHistogramConfig(void *pCfg);
INTERFACE bool MDrvSclVipGetDlcHistogramReport(void *pCfg);
INTERFACE bool MDrvSclVipSetDlcConfig(void *pCfg);
INTERFACE bool MDrvSclVipSetLceConfig(void *pCfg);
INTERFACE bool MDrvSclVipSetUvcConfig(void *pCfg);
INTERFACE bool MDrvSclVipSetIhcConfig(void *pCfg);
INTERFACE bool MDrvSclVipSetICEConfig(void *pCfg);
INTERFACE bool MDrvSclVipSetIhcICCADPYConfig(void *pCfg);
INTERFACE bool MDrvSclVipSetIbcConfig(void *pCfg);
INTERFACE bool MDrvSclVipSetFccConfig(void *pCfg);
INTERFACE bool MDrvSclVipCmdqWriteConfig(MDrvSclVipCmdqConfig_t *pCfg);
INTERFACE bool MDrvSclVipSetLdcConfig(void *pCfg);
INTERFACE bool MDrvSclVipSetLdcmdConfig(void *pCfg);
INTERFACE bool MDrvSclVipSetLdcDmapConfig(void *pCfg);
INTERFACE bool MDrvSclVipSetLdcSramConfig(void *pCfg);
INTERFACE bool MDrvSclVipSetAckConfig(void *pCfg);
INTERFACE bool MDrvSclVipSetNlmConfig(void *pCfg);
INTERFACE bool MDrvSclVipSetVipOtherConfig(void *pCfg);
INTERFACE bool MDrvSclVipSetVipBypassConfig(MDrvSclVipConfigType_e enVIPtype);
INTERFACE bool MDrvSclVipSetAipBypassConfig(MDrvSclVipAipType_e enAIPtype);
INTERFACE bool MDrvSclVipSetNlmSramConfig(MDrvSclVipNlmSramConfig_t stSRAM);
INTERFACE void MDrvSclVipPrepareStructToCheckRegister(MDrvSclVipCmdqCheckType_e enCheckType);
INTERFACE void MDrvSclVipSetCheckMode(MDrvSclVipCmdqCheckType_e enCheckType);
INTERFACE void MDrvSclVipCheckRegister(void);
INTERFACE void MDrvSclVipCheckConsist(void);
INTERFACE bool MDrvSclVipVtrackSetUserDefindedSetting(bool bUserDefinded, u8 *pu8Setting);
INTERFACE bool MDrvSclVipVtrackEnable( u8 u8FrameRate, MDrvSclVipVtrackEnableType_e bEnable);
INTERFACE bool MDrvSclVipVtrackSetPayloadData(u16 u16Timecode, u8 u8OperatorID);
INTERFACE bool MDrvSclVipVtrackSetKey(bool bUserDefinded, u8 *pu8Setting);
INTERFACE bool MDrvSclVipVtrackInit(void);
INTERFACE u32 MDrvSclVipGetSCLFrameEndCount(void);
INTERFACE bool MDrvSclVipSetAipConfig(MDrvSclVipAipConfig_t stCfg);
INTERFACE bool MDrvSclVipSetAipSramConfig(MDrvSclVipAipSramConfig_t stCfg);
INTERFACE bool MDrvSclVipSetMcnrConfig(void *pstCfg);

#if defined (SCLOS_TYPE_LINUX_KERNEL)
INTERFACE ssize_t MDrvSclVipProcShow(char *buf);
INTERFACE ssize_t MDrvSclVipVipShow(char *buf);
INTERFACE void MDrvSclVipVipStore(const char *buf);
INTERFACE ssize_t MDrvSclVipCmdqShow(char *buf);
INTERFACE ssize_t MDrvSclVipVipSetRuleShow(char *buf);
#endif

#undef INTERFACE


#endif
