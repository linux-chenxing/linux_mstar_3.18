#ifndef __HAL_AUD_API_H__
#define __HAL_AUD_API_H__
#include "hal_audio.h"
#include "hal_audio_types.h"
//#include <linux/types.h>
//#include "sys_datatype.h"

void HalAudSysSetBankBaseAddr(U32 nAddr);

BOOL HalAudSetMux(AudMux_e eMux, U8 nChoice);
BOOL HalAudSetMixer(AudMixer_e eMixer, U8 nChoice);
BOOL HalAudSetSynthRate(AudSynth_e eSynth, AudRate_e eRate);

BOOL HalAudI2sSetTdmMode(AudI2s_e eI2s, AudI2sMode_e eMode);
BOOL HalAudI2sSetMsMode(AudI2s_e eI2s, AudI2sMsMode_e eMsMode);
BOOL HalAudI2sSetFmt(AudI2s_e eI2s, AudI2sFmt_e eFmt);
BOOL HalAudI2sSetWidth(AudI2s_e eI2s, AudBitWidth_e eWidth);
BOOL HalAudI2sSetTdmChannel(AudI2s_e eI2s, U16 nChannel);

BOOL HalAudI2sSetSlotConfig(AudI2s_e eI2s, U16 nSlotMsk, AudTdmChnMap_e eMap);
BOOL HalAudI2sSetWckWidth(AudI2s_e eI2s, U16 nWidth);
BOOL HalAudI2sEnable(AudI2s_e eI2s, BOOL bEnable);

BOOL HalAudDmaReset(AudDmaChn_e eDmaChannel);
BOOL HalAudDmaEnable(AudDmaChn_e eDmaChannel, BOOL bEnable);
BOOL HalAudDmaPause(AudDmaChn_e eDmaChannel);
BOOL HalAudDmaResume(AudDmaChn_e eDmaChannel);
BOOL HalAudDmaIntEnable(AudDmaChn_e eDmaChannel, BOOL bDatatrigger, BOOL bDataboundary, BOOL bLocalData);
BOOL HalAudDmaClearInt(AudDmaChn_e eDmaChannel);
U32 HalAudDmaGetLevelCnt(AudDmaChn_e eDmaChannel);
U32 HalAudDmaTrigLevelCnt(AudDmaChn_e eDmaChannel, U32 nDataSize);
BOOL HalAudDmaSetPhyAddr(AudDmaChn_e eDmaChannel, U32 nBufAddrOffset, U32 nBufSize);
BOOL HalAudDmaSetBitMode(AudDmaChn_e eDmaChannel, U16 nSel);
BOOL HalAudDmaWrSetThreshold(AudDmaChn_e eDmaChannel, U32 nOverrunTh);
BOOL HalAudDmaRdSetThreshold(AudDmaChn_e eDmaChannel, U32 nUnderrunTh);
BOOL HalAudDmaGetFlags(AudDmaChn_e eDmaChannel, BOOL *pbDatatrigger, BOOL *pbDataboundary, BOOL *pbLocalData);
BOOL HalAudDmaGetStatus(AudDmaChn_e eDmaChannel, BOOL *pbDataboundary);
BOOL HalAudDmaRd5ConfigMch(U16 nCh);
BOOL HalAudDmaWrConfigMch(AudDmaChn_e eDmaChannel, U16 nCh);
BOOL HalAudDmaWr1SetClkRate(AudDmaClkSrc_e eSrc, AudRate_e eRate);
BOOL HalAudDmaWr2SetClkRate(AudDmaClkSrc_e eSrc, AudRate_e eRate);
BOOL HalAudDmaWr3SetClkRate(AudDmaClkSrc_e eSrc, AudRate_e eRate);
BOOL HalAudDmaWr4SetClkRate(AudDmaClkSrc_e eSrc, AudRate_e eRate);
BOOL HalAudDmaWr5SetClkRate(AudDmaClkSrc_e eSrc, AudRate_e eRate);
BOOL HalAudDmaRd1SetClkRate(AudDmaClkSrc_e eSrc, AudRate_e eRate);
BOOL HalAudDmaRd2SetClkRate(AudDmaClkSrc_e eSrc, AudRate_e eRate);
BOOL HalAudDmaRd3SetClkRate(AudDmaClkSrc_e eSrc, AudRate_e eRate);
BOOL HalAudDmaRd4SetClkRate(AudDmaClkSrc_e eSrc, AudRate_e eRate);
BOOL HalAudDmaRd5SetClkRate(AudDmaClkSrc_e eSrc, AudRate_e eRate);

void HalAudAtopInit(void);
BOOL HalAudAtopOpen(AudAtopPath_e path);
BOOL HalAudAtopClose(AudAtopPath_e path);
BOOL HalAudAtopMicAmpGain(U16 nSel);
BOOL HalAudAtopAdcGain(AudAdcPath_e eAdc, U16 nSel);
BOOL HalAudAtopAdcInmux(AudAdcPath_e eAdc);

BOOL HalAudDmicCicCtrl(BOOL bEnable);
BOOL HalAudDmicSetGain(U16 nGain);
BOOL HalAudDmicSetChannel(S16 nChannels);
BOOL HalAudDmicSetMode(U16 nMode);
BOOL HalAudDmicSetClkRate(AudPdmClkRate_e eClkRate);
U16 HalAudDmicFindMode(AudRate_e eRate);

BOOL HalAudIntEnable(AudInt_e eInt, U16 nEn);

void HalAudConfigInit(void);
void HalAudSysInit(void);

#endif //__HAL_AUD_API_H__