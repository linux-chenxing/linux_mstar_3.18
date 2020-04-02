#ifndef __DRV_AUDIO_H__
#define __DRV_AUDIO_H__

#include "cam_os_wrapper.h"
#include "hal_audio.h"
#include "hal_audio_types.h"


typedef struct
{
    u8 *pDmaBuf;
    u32 nPhysDmaAddr;
    u32 nBufferSize;
    u16 nChannels;
    u16 nBitWidth;
    u32 nSampleRate;
    u32 nPeriodSize;
    u32 nStartThres;
    u16 nInterleaved;
} DmaParam_t;

typedef enum
{
    AUD_I2S_IF_MISC_RX,
    AUD_I2S_IF_MISC_TX,
    AUD_I2S_IF_CODEC_RX,
    AUD_I2S_IF_CODEC_TX,
    AUD_I2S_IF_BT_TRX,
    AUD_I2S_IF_NUM
}AudI2sIf_e;

typedef long PcmFrames_t;


void DrvAudInit(void);
BOOL DrvAudConfigI2s(AudI2sIf_e eI2sIf, AudI2sCfg_t *ptI2sCfg);
BOOL DrvAudDmaInitChannel(AudDmaChn_e eDma,
                                 u32 nPhysDmaAddr,
                                 u32 nBufferSize,
                                 u32 nChannels,
                                 AudBitWidth_e eBitWidth,
                                 u32 nThreshold);

BOOL DrvAudSetMux(AudMux_e eMux, u8 nChoice);
BOOL DrvAudSetMixer(AudMixer_e eMixer, u8 nChoice);
BOOL DrvAudEnableI2s(AudI2sIf_e eI2sIf, BOOL bEnable);
BOOL DrvAudEnableAtop(AudAtopPath_e eAtop, BOOL bEnable);

BOOL DrvAudConfigDmaParam(AudDmaChn_e eDmaChannel, DmaParam_t *ptParam);
BOOL DrvAudOpenDma(AudDmaChn_e eDmaChannel);
BOOL DrvAudPrepareDma(AudDmaChn_e eDmaChannel);
BOOL DrvAudCloseDma(AudDmaChn_e eDmaChannel);
BOOL DrvAudStartDma(AudDmaChn_e eDmaChannel);
BOOL DrvAudStopDma(AudDmaChn_e eDmaChannel);
BOOL DrvAudPauseDma(AudDmaChn_e eDmaChannel);
BOOL DrvAudResumeDma(AudDmaChn_e eDmaChannel);
BOOL DrvAudIsXrun(AudDmaChn_e eDmaChannel);
BOOL DrvAudSetGain(AudDmaChn_e eDmaChannel, s16 nGain);
BOOL DrvAudSetAdcGain(AudAdcPath_e eAdc, U16 nSel);
BOOL DrvAudSetMicAmpGain(U16 nSel);
BOOL DrvAudAdcSetMux(AudAdcPath_e eAdcPath);

s32 DrvAudWritePcm(AudDmaChn_e eDmaChannel, void *pWrBuffer, u32 nSize, BOOL bBlock);
s32 DrvAudWritePcmAll(AudDmaChn_e eDmaChannel, void *pWrBuffer, u32 nSize, BOOL bBlock);
s32 DrvAudReadPcm(AudDmaChn_e eDmaChannel, void *pRdBuffer, u32 nSize, BOOL bBlock);

#endif //__DRV_AUDIO_H__