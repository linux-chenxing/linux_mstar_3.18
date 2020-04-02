#include <linux/spinlock.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/math64.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/delay.h>
#include "ms_platform.h"
#include "hal_audio_api.h"
#include "drv_audio.h"
#include "drv_audio_dbg.h"
#include "infinity2/irqs.h"

#define MIN_GAIN -60
#define MAX_GAIN 12
#define GAIN_STEP 2;
#define MAX_S16 32767
#define MIN_S16 -32768


#define INFINITY2_AUDIO_IRQ_ID INT_IRQ_110_AU_INT+32
#define DMA_EMPTY 0x0
#define DMA_UNDERRUN 0x1
#define DMA_OVERRUN 0x2
#define DMA_FULL 0x3
#define DMA_NORMAL 0x4

#define SND_PCM_STATE_INIT     (1<<0)
#define SND_PCM_STATE_SETUP    (1<<1)
#define SND_PCM_STATE_OPEN     (1<<2)
#define SND_PCM_STATE_PREPARED (1<<3)
#define SND_PCM_STATE_RUNNING  (1<<4)
#define SND_PCM_STATE_XRUN     (1<<5)
#define SND_PCM_STATE_PAUSED   (1<<6)

//Q13
const static u32 _gaGainTable[]=
{
    0x0008, //-60dB
    0x0009,
    0x000A,
    0x000C,
    0x000D,
    0x000F,
    0x0010,
    0x0012,
    0x0015,
    0x0017,
    0x001A,
    0x001D,
    0x0021,
    0x0025,
    0x0029,
    0x002E,
    0x0034,
    0x003A,
    0x0041,
    0x0049,
    0x0052,
    0x005C,
    0x0067,
    0x0074,
    0x0082,
    0x0092,
    0x00A3,
    0x00B7,
    0x00CE,
    0x00E7,
    0x0103,
    0x0123,
    0x0146,
    0x016E,
    0x019B,
    0x01CD,
    0x0205,
    0x0244,
    0x028B,
    0x02DA,
    0x0333,
    0x0397,
    0x0407,
    0x0485,
    0x0512,
    0x05B1,
    0x0663,
    0x072A,
    0x080A,
    0x0905,
    0x0A1F,
    0x0B5B,
    0x0CBD,
    0x0E4B,
    0x100A,
    0x11FF,
    0x1431,
    0x16A7,
    0x196B,
    0x1C85,
    0x2000,
    0x23E8,
    0x2849,
    0x2D34,
    0x32B7,
    0x38E8,
    0x3FD9,
    0x47A4,
    0x5061,
    0x5A30,
    0x6531,
    0x718A,
    0x7F65 //12dB
};


typedef enum
{
    PCM_STREAM_PLAYBACK,
    PCM_STREAM_CAPTURE,
} PcmStream_e;

typedef struct
{
    u8  szName[20];    /* stream identifier */
    u16 nChnId;    /* Channel ID */
    u8 *pDmaArea;	/* DMA area */
    dma_addr_t tDmaAddr; /* physical bus address (not accessible from main CPU) */
    u32 nBufBytes; /* Size of the DMA transfer */
    PcmFrames_t tBufSize;    /* Size of the DMA transfer */
    PcmFrames_t tPeriodSize;
    PcmFrames_t tApplPtr;
#ifdef __HWPTR__
    PcmFrames_t tHwPtr;
    PcmFrames_t tBoundary;
#endif
    spinlock_t tLock;
} PcmDmaData_t;


typedef struct PcmRuntimeData
{
    spinlock_t            tPcmLock;
    wait_queue_head_t     tSleep;
    volatile u16          nState;
#ifdef __HWPTR__
    u32                   nDmaLevelCount;
#endif
    u32                   nRemainCount;
    BOOL                  bEnableIrq;
    PcmStream_e           eStream;
    void                  *pPrivateData;
    u16                   nFrameBits;
    u16                   nHwFrameBits;
    u16                   nStatus;
    s32 (*copy)(struct PcmRuntimeData *ptRuntimeData, u32 nHwoff, u8* pBuf, u32 nBufSize, u32 nPos, PcmFrames_t tFrames);
} PcmRuntimeData_t;

typedef struct
{
    u16 nCurrentGainLevel;
    u16 nTargetGainLevel;
    u16 nGainStep;
} GainData_t;


PcmRuntimeData_t _gaRuntimeData[AUD_DMA_NUM];
DmaParam_t _gaDmaParam[AUD_DMA_NUM];
GainData_t _gaGainData[AUD_DMA_NUM];
static u8 _gaMuxSel[AUD_MUX_NUM]= {0};
static BOOL _gbI2sMaster[AUD_I2S_IF_NUM];
unsigned int INFINITY_IRQ_ID = INFINITY2_AUDIO_IRQ_ID; //INT_IRQ_110_AU_INT;


/**
 * bytes_to_frames - Unit conversion of the size from bytes to frames
 * @runtime: PCM runtime instance
 * @size: size in bytes
 */
static inline PcmFrames_t _BytesToFrames(PcmRuntimeData_t *pRtd, u32 size)
{
    return size / (pRtd->nFrameBits >> 3);
}

static inline PcmFrames_t _BytesToHwFrames(PcmRuntimeData_t *pRtd, u32 size)
{
    return size / (pRtd->nHwFrameBits >> 3);
}

/**
 * frames_to_bytes - Unit conversion of the size from frames to bytes
 * @runtime: PCM runtime instance
 * @size: size in frames
 */
static inline u32 _FramesToBytes(PcmRuntimeData_t *pRtd, PcmFrames_t size)
{
    return size * (pRtd->nFrameBits >> 3);
}

static inline u32 _HwFramesToBytes(PcmRuntimeData_t *pRtd, PcmFrames_t size)
{
    return size * (pRtd->nHwFrameBits >> 3);
}

#ifdef __HWPTR__
/**
 * _AudPcmPlaybackAvail - Get the available (writable) space for playback
 * Result is between 0 ... (boundary - 1)
 */
static inline PcmFrames_t _AudPcmPlaybackAvail(PcmDmaData_t *pDmaData)
{
    PcmFrames_t tAvail = pDmaData->tHwPtr + pDmaData->tBufSize - pDmaData->tApplPtr;

    if ( tAvail >= pDmaData->tBoundary)
        tAvail -= pDmaData->tBoundary;
    return tAvail;
}

/**
 * _AudPcmCaptureAvail - Get the available (readable) space for capture
 * Result is between 0 ... (boundary - 1)
 */
static inline PcmFrames_t _AudPcmCaptureAvail(PcmDmaData_t *pDmaData)
{
    PcmFrames_t tAvail = pDmaData->tHwPtr - pDmaData->tApplPtr;
    if (tAvail < 0)
        tAvail += pDmaData->tBoundary;
    return tAvail;
}

/**
 * _AudPcmPlaybackAvail - Get the queued space for playback
 */
static inline PcmFrames_t _AudPcmPlaybackHwAvail(PcmDmaData_t *pDmaData)
{
    return pDmaData->tBufSize - _AudPcmPlaybackAvail(pDmaData);
}

/**
 * _AudPcmCaptureHwAvail - Get the free space for capture
 */
static inline PcmFrames_t _AuddPcmCaptureHwAvail(PcmDmaData_t *pDmaData)
{
    return pDmaData->tBufSize - _AudPcmCaptureAvail(pDmaData);
}
#else
/**
 * _AudPcmPlaybackAvail - Get the available (writable) space for playback
 */
static inline PcmFrames_t _AudPcmPlaybackAvail(PcmDmaData_t *pDmaData)
{
    unsigned long nFlags;
    PcmFrames_t tAvail;
    u32 nCurDmaLevelCount = 0;

    spin_lock_irqsave(&pDmaData->tLock, nFlags);
    nCurDmaLevelCount = HalAudDmaGetLevelCnt(pDmaData->nChnId);
    tAvail = pDmaData->tBufSize - _BytesToHwFrames(&_gaRuntimeData[pDmaData->nChnId],nCurDmaLevelCount+_gaRuntimeData[pDmaData->nChnId].nRemainCount);
    spin_unlock_irqrestore(&pDmaData->tLock, nFlags);

    return tAvail;
}

/**
 * _AudPcmCaptureAvail - Get the available (readable) space for capture
 */
static inline PcmFrames_t _AudPcmCaptureAvail(PcmDmaData_t *pDmaData)
{
    unsigned long nFlags;
    PcmFrames_t tAvail;
    u32 nCurDmaLevelCount = 0;
    spin_lock_irqsave(&pDmaData->tLock, nFlags);

    nCurDmaLevelCount = HalAudDmaGetLevelCnt(pDmaData->nChnId);
    tAvail = _BytesToHwFrames(&_gaRuntimeData[pDmaData->nChnId],nCurDmaLevelCount-_gaRuntimeData[pDmaData->nChnId].nRemainCount);
    spin_unlock_irqrestore(&pDmaData->tLock, nFlags);

    //AUD_PRINTF(ERROR_LEVEL,"buf %ld, count %u,remain %u\n",pDmaData->tBufSize,nCurDmaLevelCount,_gaRuntimeData[pDmaData->nChnId].nRemainCount);

    if(_gaRuntimeData[pDmaData->nChnId].nRemainCount>0)
        AUD_PRINTF(ERROR_LEVEL,"remain %u\n",_gaRuntimeData[pDmaData->nChnId].nRemainCount);
    return tAvail;
}
#endif

static BOOL _AudDmaInitChannel(AudDmaChn_e eDmaChannel,
                               u8 *pDmaBuf,
                               u32 nPhysDmaAddr,
                               u32 nBufferSize,
                               u32 nChannels,
                               AudBitWidth_e eBitWidth,
                               u32 nPeriodSize)
{
    BOOL bRet = TRUE;
    AUD_PRINTF(TRACE_LEVEL,"%s chn = %d\n",__FUNCTION__,eDmaChannel);

    // Set up the physical DMA buffer address
    bRet &= HalAudDmaSetPhyAddr(eDmaChannel, nPhysDmaAddr, nBufferSize);

    switch(eBitWidth)
    {
        case AUD_BITWIDTH_16:
            bRet &= HalAudDmaSetBitMode(eDmaChannel, 0);
            break;
        case AUD_BITWIDTH_24:
        case AUD_BITWIDTH_32:
            bRet &= HalAudDmaSetBitMode(eDmaChannel, 1);
            break;
        default:
            return FALSE;
    }


    switch(eDmaChannel)
    {
        case AUD_DMA_WRITER1:
        case AUD_DMA_WRITER2:
        case AUD_DMA_WRITER3:
        case AUD_DMA_WRITER4:
        case AUD_DMA_WRITER5:
            bRet &= HalAudDmaWrConfigMch(eDmaChannel, nChannels);
            bRet &= HalAudDmaWrSetThreshold(eDmaChannel, nPeriodSize);
            if(nPeriodSize==0)
            {
                _gaRuntimeData[eDmaChannel].bEnableIrq = FALSE;
            }
            else
            {
                //bRet &= HalAudDmaWrSetThreshold(eDmaChannel, nPeriodSize);
                _gaRuntimeData[eDmaChannel].bEnableIrq = TRUE;
            }
            break;
        case AUD_DMA_READER1:
        case AUD_DMA_READER2:
        case AUD_DMA_READER3:
        case AUD_DMA_READER4:
            if(nChannels!=2) //only support stereo?
            {
                return FALSE;
            }

            bRet &= HalAudDmaRdSetThreshold(eDmaChannel, nBufferSize - nPeriodSize);

            if(nPeriodSize==0)
            {
                _gaRuntimeData[eDmaChannel].bEnableIrq = FALSE;
            }
            else
            {
                //bRet &= HalAudDmaRdSetThreshold(eDmaChannel, nBufferSize - nPeriodSize);
                _gaRuntimeData[eDmaChannel].bEnableIrq = TRUE;
            }
            break;
        case AUD_DMA_READER5:
            bRet &= HalAudDmaRd5ConfigMch(nChannels);
            bRet &= HalAudDmaRdSetThreshold(eDmaChannel, nBufferSize - nPeriodSize);

            if(nPeriodSize==0)
            {
                _gaRuntimeData[eDmaChannel].bEnableIrq = FALSE;
            }
            else
            {
                //bRet &= HalAudDmaRdSetThreshold(eDmaChannel, nBufferSize - nPeriodSize);
                _gaRuntimeData[eDmaChannel].bEnableIrq = TRUE;
            }
            break;
        default:
            return FALSE;
    }

    return bRet;
}

//To-Do : BT I2S TRx reader/writer with different sampling rate
static BOOL _AudDmaSetRate(AudDmaChn_e eDma, AudDmaClkSrc_e eClkSrc, AudRate_e eRate)
{
    BOOL bRet = TRUE;
    AUD_PRINTF(TRACE_LEVEL,"%s chn = %d, src %d, rate %d\n",__FUNCTION__,eDma,eClkSrc,eRate);
    switch(eDma)
    {
        case AUD_DMA_WRITER1:
            bRet=HalAudDmaWr1SetClkRate(eClkSrc, eRate);
            break;
        case AUD_DMA_WRITER2:
            bRet=HalAudDmaWr2SetClkRate(eClkSrc, eRate);
            if(bRet)
            {
                if(_gbI2sMaster[AUD_I2S_IF_CODEC_RX])
                {
                    bRet = HalAudSetSynthRate(AUD_SYNTH_CODEC_RX,eRate);
                }
                else
                {
                    bRet = HalAudSetSynthRate(AUD_SYNTH_CODEC_RX,AUD_RATE_SLAVE);
                }
            }
            break;
        case AUD_DMA_WRITER3:
            bRet=HalAudDmaWr3SetClkRate(eClkSrc, eRate);
            if(bRet)
            {
                if(_gbI2sMaster[AUD_I2S_IF_BT_TRX])
                {
                    bRet = HalAudSetSynthRate(AUD_SYNTH_BT_RX,eRate);
                }
                else
                {
                    bRet = HalAudSetSynthRate(AUD_SYNTH_BT_RX,AUD_RATE_SLAVE);
                }
            }
            break;
        case AUD_DMA_WRITER4:
            bRet=HalAudDmaWr4SetClkRate(eClkSrc,eRate);
            break;
        case AUD_DMA_WRITER5:
            bRet=HalAudDmaWr5SetClkRate(eClkSrc,eRate);
            break;
        case AUD_DMA_READER1:
            bRet=HalAudDmaRd1SetClkRate(eClkSrc,eRate);
            break;
        case AUD_DMA_READER2:
            bRet=HalAudDmaRd2SetClkRate(eClkSrc,eRate);
            if(bRet)
            {
                if(_gbI2sMaster[AUD_I2S_IF_CODEC_TX])
                {
                    bRet = HalAudSetSynthRate(AUD_SYNTH_CODEC_TX,eRate);
                }
                else
                {
                    bRet = HalAudSetSynthRate(AUD_SYNTH_CODEC_TX,AUD_RATE_SLAVE);
                }
            }
            break;
        case AUD_DMA_READER3:
            bRet=HalAudDmaRd3SetClkRate(eClkSrc,eRate);
            if(bRet)
            {
                if(_gbI2sMaster[AUD_I2S_IF_BT_TRX])
                {
                    bRet = HalAudSetSynthRate(AUD_SYNTH_BT_RX,eRate);
                }
                else
                {
                    bRet = HalAudSetSynthRate(AUD_SYNTH_BT_RX,AUD_RATE_SLAVE);
                }
            }
            break;
        case AUD_DMA_READER4:
            bRet=HalAudDmaRd4SetClkRate(eClkSrc,eRate);
            break;
        case AUD_DMA_READER5:
            bRet=HalAudDmaRd5SetClkRate(eClkSrc,eRate);
            break;
        default:
            return FALSE;
    }
    return bRet;
}

static BOOL _AudDmaStartChannel(AudDmaChn_e eDmaChannel)
{
    BOOL bRet = TRUE;
    AUD_PRINTF(TRACE_LEVEL,"%s chn = %d, level %u\n",__FUNCTION__,eDmaChannel,HalAudDmaGetLevelCnt(eDmaChannel));
    bRet &= HalAudDmaEnable(eDmaChannel, TRUE);
    bRet &= HalAudDmaClearInt(eDmaChannel);

    return bRet;
}


static BOOL _AudDmaStopChannel(AudDmaChn_e eDmaChannel)
{
    BOOL bRet = TRUE;
    AUD_PRINTF(TRACE_LEVEL,"%s chn = %d\n",__FUNCTION__,eDmaChannel);
    bRet &= HalAudDmaEnable(eDmaChannel, FALSE);
    bRet &= HalAudDmaReset(eDmaChannel);

    return bRet;
}

#ifdef __HWPTR__
static s32 _AudDmaPointer(PcmRuntimeData_t *ptRuntimeData)
{
    PcmDmaData_t *ptDmaData = (PcmDmaData_t*)ptRuntimeData->pPrivateData;
    s32 nOffset = 0;
    unsigned long nFlags;
    u32 nCurDmaLevelCount = 0;
    if (PCM_STREAM_PLAYBACK == ptRuntimeData->eStream)
    {
        spin_lock_irqsave(&ptDmaData->tLock, nFlags);

        nCurDmaLevelCount = HalAudDmaGetLevelCnt(ptDmaData->nChnId);
        spin_unlock_irqrestore(&ptDmaData->tLock, nFlags);

        // if (ptRuntimeData->nDmaLevelCount > ptDmaData->nBufBytes* 2)
        //     ptRuntimeData->nDmaLevelCount -= ptDmaData->nBufBytes;
        //  nOffset = _BytesToHwFrames(ptRuntimeData, (ptRuntimeData->nDmaLevelCount  - nCurDmaLevelCount - ptRuntimeData->nRemainCount)% ptDmaData->nBufBytes);
        if (ptRuntimeData->nDmaLevelCount > _HwFramesToBytes(ptRuntimeData,ptDmaData->tBoundary))
            ptRuntimeData->nDmaLevelCount -= _HwFramesToBytes(ptRuntimeData,ptDmaData->tBoundary);
        nOffset = ptRuntimeData->nDmaLevelCount  - nCurDmaLevelCount - ptRuntimeData->nRemainCount;
        if(nOffset<0) //equal case?
            nOffset += _HwFramesToBytes(ptRuntimeData,ptDmaData->tBoundary);
    }
    else if (PCM_STREAM_CAPTURE == ptRuntimeData->eStream)
    {
        spin_lock_irqsave(&ptDmaData->tLock, nFlags);

        nCurDmaLevelCount = HalAudDmaGetLevelCnt(ptDmaData->nChnId);
        spin_unlock_irqrestore(&ptDmaData->tLock, nFlags);

        //if (ptRuntimeData->nDmaLevelCount > ptDmaData->nBufBytes* 2)
        //    ptRuntimeData->nDmaLevelCount -= ptDmaData->nBufBytes;
        //nOffset = _BytesToHwFrames(ptRuntimeData,(ptRuntimeData->nDmaLevelCount  + nCurDmaLevelCount - ptRuntimeData->nRemainCount)% ptDmaData->nBufBytes);

        if (ptRuntimeData->nDmaLevelCount > _HwFramesToBytes(ptRuntimeData,ptDmaData->tBoundary))
            ptRuntimeData->nDmaLevelCount -= _HwFramesToBytes(ptRuntimeData,ptDmaData->tBoundary);
        nOffset = ptRuntimeData->nDmaLevelCount  + nCurDmaLevelCount - ptRuntimeData->nRemainCount;
        if(nOffset>_HwFramesToBytes(ptRuntimeData,ptDmaData->tBoundary)) //equal case?
            nOffset -= _HwFramesToBytes(ptRuntimeData,ptDmaData->tBoundary);

    }
    return nOffset;
}

static BOOL _AudDmaUpdateHwPointer(PcmRuntimeData_t *ptRuntimeData)
{
    PcmDmaData_t *ptDmaData = (PcmDmaData_t*)ptRuntimeData->pPrivateData;
    s32 nOffset = _AudDmaPointer(ptRuntimeData);
    AUD_PRINTF(ERROR_LEVEL,"%s chn = %d, ptr %d\n",__FUNCTION__,ptDmaData->nChnId,nOffset);
    //if(nOffset > ptDmaData->tBufSize)
    if(_BytesToHwFrames(ptRuntimeData,nOffset) > ptDmaData->tBoundary)
    {
        return FALSE;
    }
    else
    {
        ptDmaData->tHwPtr = _BytesToHwFrames(ptRuntimeData,nOffset);
        AUD_PRINTF(DEBUG_LEVEL,"%s chn = %d, ptr %d\n",__FUNCTION__,ptDmaData->nChnId,nOffset);
        return TRUE;
    }
}
#endif

static irqreturn_t  _AudDmaIrq(int irq, void *dev_id)
{
    PcmRuntimeData_t *ptRuntimeData = (PcmRuntimeData_t*)dev_id;
    PcmDmaData_t *ptDmaData = (PcmDmaData_t *)ptRuntimeData->pPrivateData;
    unsigned long nFlags;
    BOOL bTrigger,bBoundary,bLocalData;

    AUD_PRINTF(IRQ_LEVEL, "in %s,Id %d, previous state %d\n",  __func__, ptDmaData->nChnId,ptRuntimeData->nState);
    spin_lock_irqsave(&ptDmaData->tLock, nFlags);
    if ((PCM_STREAM_PLAYBACK == ptRuntimeData->eStream) && ptRuntimeData->nStatus == SND_PCM_STATE_RUNNING)
    {
        HalAudDmaGetFlags(ptDmaData->nChnId, &bTrigger, &bBoundary, &bLocalData);
        if (ptRuntimeData->nState != DMA_EMPTY)
        {
            if (bBoundary==TRUE)
            {
                HalAudDmaIntEnable(ptDmaData->nChnId, FALSE, FALSE, FALSE);
                _AudDmaStopChannel(ptDmaData->nChnId);
                ptRuntimeData->nStatus = SND_PCM_STATE_XRUN;
                ptRuntimeData->nState = DMA_EMPTY;

                AUD_PRINTF(IRQ_LEVEL, "EMPTY: chanId = %d\n",  ptDmaData->nChnId);


            }
            else if ((ptRuntimeData->nState != DMA_UNDERRUN) && bTrigger==TRUE)
            {
                HalAudDmaIntEnable(ptDmaData->nChnId, FALSE, TRUE, FALSE);

                ptRuntimeData->nState  = DMA_UNDERRUN;
                spin_unlock_irqrestore(&ptDmaData->tLock, nFlags);
#ifdef __HWPTR__
                _AudDmaUpdateHwPointer(ptRuntimeData);
#endif
                //_AudDmaUpdateHwLevel(ptRuntimeData);
                wake_up(&ptRuntimeData->tSleep);
                spin_lock_irqsave(&ptDmaData->tLock, nFlags);
                AUD_PRINTF(IRQ_LEVEL, "UNDER: chanId = %d,levelcount %u\n",  ptDmaData->nChnId,HalAudDmaGetLevelCnt(ptDmaData->nChnId));

            }
            else
            {
                AUD_PRINTF(ERROR_LEVEL, "UNKNOWN interrupt %d %d %d\n",bTrigger,bBoundary,bLocalData);
            }
        }
    }
    else if ((PCM_STREAM_CAPTURE == ptRuntimeData->eStream) && (ptRuntimeData->nStatus == SND_PCM_STATE_RUNNING))  // CAPTURE device
    {
        HalAudDmaGetFlags(ptDmaData->nChnId, &bTrigger, &bBoundary, &bLocalData);
        if (ptRuntimeData->nState != DMA_FULL)
        {
            if (bBoundary==TRUE)
            {
                HalAudDmaIntEnable(ptDmaData->nChnId, FALSE, FALSE, FALSE);
                _AudDmaStopChannel(ptDmaData->nChnId);
                ptRuntimeData->nStatus = SND_PCM_STATE_XRUN;

                AUD_PRINTF(IRQ_LEVEL, "FULL: chanId = %d\n", ptDmaData->nChnId);

                ptRuntimeData->nState = DMA_FULL;
            }
            else if ((ptRuntimeData->nState != DMA_OVERRUN) && bTrigger==TRUE)
            {
                HalAudDmaIntEnable(ptDmaData->nChnId, FALSE, TRUE, FALSE);

                ptRuntimeData->nState = DMA_OVERRUN;
                spin_unlock_irqrestore(&ptDmaData->tLock, nFlags);
                // _AudDmaUpdateHwLevel(ptRuntimeData);
#ifdef __HWPTR__
                _AudDmaUpdateHwPointer(ptRuntimeData);
#endif
                wake_up(&ptRuntimeData->tSleep);
                spin_lock_irqsave(&ptDmaData->tLock, nFlags);

                AUD_PRINTF(IRQ_LEVEL, "OVER: chanId = %d,levelcount %u\n",  ptDmaData->nChnId,HalAudDmaGetLevelCnt(ptDmaData->nChnId));
            }
            else
                AUD_PRINTF(ERROR_LEVEL, "UNKNOWN interrupt %d %d %d\n",bTrigger,bBoundary,bLocalData);
        }
    }
    spin_unlock_irqrestore(&ptDmaData->tLock, nFlags);

    return IRQ_HANDLED;
}

static int _WaitForAvail(PcmRuntimeData_t *ptRuntimeData, PcmFrames_t *ptAvail)
{
    PcmDmaData_t *ptDmaData = (PcmDmaData_t *)(ptRuntimeData->pPrivateData);
    BOOL bIsPlayback = (ptRuntimeData->eStream == PCM_STREAM_PLAYBACK);
    wait_queue_t wait;
    int err = 0;
    long nWaitTime, nTout;
    PcmFrames_t tAvail;
    init_waitqueue_entry(&wait, current);
    set_current_state(TASK_INTERRUPTIBLE);
    add_wait_queue(&ptRuntimeData->tSleep, &wait);


    if (_gaDmaParam[ptDmaData->nChnId].nSampleRate)
    {
        nWaitTime = ptDmaData->tPeriodSize*2*1000/ _gaDmaParam[ptDmaData->nChnId].nSampleRate;
    }

    nWaitTime = max((long)5000, nWaitTime);//min 5s wait
    nWaitTime = msecs_to_jiffies(nWaitTime);

    for (;;)
    {
        if (signal_pending(current))
        {
            err = -ERESTARTSYS;
            break;
        }

        /*
         * We need to check if space became available already
         * (and thus the wakeup happened already) first to close
         * the race of space already having become available.
         * This check must happen after been added to the waitqueue
         * and having current state be INTERRUPTIBLE.
         */
        // _AudDmaUpdateHwLevel(ptRuntimeData);
        if (bIsPlayback)
            tAvail = _AudPcmPlaybackAvail(ptDmaData);
        else
            tAvail = _AudPcmCaptureAvail(ptDmaData);

        if (tAvail >=ptDmaData->tPeriodSize) //any available data
            break;

        spin_unlock_irq(&ptRuntimeData->tPcmLock);

        nTout = schedule_timeout(nWaitTime);

        spin_lock_irq(&ptRuntimeData->tPcmLock);
        set_current_state(TASK_INTERRUPTIBLE);
        switch (ptRuntimeData->nStatus)
        {
            case SND_PCM_STATE_XRUN:
                err = -EPIPE;
                goto _endloop;
            case SND_PCM_STATE_OPEN:
            case SND_PCM_STATE_SETUP:
                err = -EBADFD;
                goto _endloop;
            case SND_PCM_STATE_PAUSED:
                continue;
        }
        if (!nTout)
        {
            AUD_PRINTF(DEBUG_LEVEL, "timeout\n");
            err = -EIO;
            break;
        }
    }
_endloop:
    set_current_state(TASK_RUNNING);
    remove_wait_queue(&ptRuntimeData->tSleep, &wait);
    *ptAvail = tAvail;
    return err;
}

u32 _AudUpdateGain(AudDmaChn_e eDmaChannel)
{
    if(_gaGainData[eDmaChannel].nGainStep)
    {
        _gaGainData[eDmaChannel].nGainStep--;
    }
    else
    {
        if(_gaGainData[eDmaChannel].nCurrentGainLevel>_gaGainData[eDmaChannel].nTargetGainLevel)
        {
            _gaGainData[eDmaChannel].nCurrentGainLevel--;
            _gaGainData[eDmaChannel].nGainStep = GAIN_STEP;
        }
        else if(_gaGainData[eDmaChannel].nCurrentGainLevel<_gaGainData[eDmaChannel].nTargetGainLevel)
        {
            _gaGainData[eDmaChannel].nCurrentGainLevel++;
            _gaGainData[eDmaChannel].nGainStep = GAIN_STEP;
        }
    }

    return _gaGainTable[_gaGainData[eDmaChannel].nCurrentGainLevel];
}

//writer HW frames should be the same as user's
s32 _AudDmaWrCopyI16(struct PcmRuntimeData *ptRuntimeData, u32 nHwoff, u8* pBuf, u32 nBufSize, u32 nPos, PcmFrames_t tFrames)
{
    PcmDmaData_t *ptDmaData = (PcmDmaData_t*)ptRuntimeData->pPrivateData;
    u8 *pHwbufPtr = ptDmaData->pDmaArea + _HwFramesToBytes(ptRuntimeData,nHwoff);
    u8 *pBufPtr = pBuf + _FramesToBytes(ptRuntimeData,nPos);

    memcpy(pBufPtr, pHwbufPtr, _FramesToBytes(ptRuntimeData,tFrames));
    return 0;
}

s32 _AudDmaWrCopyI32(struct PcmRuntimeData *ptRuntimeData, u32 nHwoff, u8* pBuf, u32 nBufSize, u32 nPos, PcmFrames_t tFrames)
{
    return 0;
}

//output noninterleave data, writer HW frames should be the same as user's
s32 _AudDmaWrCopyN16(struct PcmRuntimeData *ptRuntimeData, u32 nHwoff, u8* pBuf, u32 nBufSize, u32 nPos, PcmFrames_t tFrames)
{
    PcmDmaData_t *ptDmaData = (PcmDmaData_t*)ptRuntimeData->pPrivateData;
    u16 nChannels = _gaDmaParam[ptDmaData->nChnId].nChannels;
    u32 i;
    s16 *pnInSample, *pnOutSample;
    u32 nInterval = nBufSize/nChannels;

    pnInSample = (s16*)(ptDmaData->pDmaArea + _HwFramesToBytes(ptRuntimeData,nHwoff));
    while(tFrames>0)
    {
        for(i=0; i<nChannels; i++)
        {
            pnOutSample = (s16*)(pBuf + nInterval*i)+nPos;
            *pnOutSample = *pnInSample;
            pnInSample++;
        }
        tFrames --;
        nPos ++;
    }
    return 0;
}

s32 _AudDmaWrCopyN32(struct PcmRuntimeData *ptRuntimeData, u32 nHwoff, u8* pBuf, u32 nBufSize, u32 nPos, PcmFrames_t tFrames)
{
    return 0;
}


s32 _AudDmaRdCopyI16(struct PcmRuntimeData *ptRuntimeData, u32 nHwoff, u8* pBuf, u32 nBufSize, u32 nPos, PcmFrames_t tFrames)
{
    PcmDmaData_t *ptDmaData = (PcmDmaData_t*)ptRuntimeData->pPrivateData;
    u16 nChannels = _gaDmaParam[ptDmaData->nChnId].nChannels;
    s16 *pnInSample;
    s16 *pnOutSample;
    u32 i;
    s32 nGain;
    s32 nSampleValue;

    pnOutSample = (s16*)(ptDmaData->pDmaArea + _HwFramesToBytes(ptRuntimeData,nHwoff));
    pnInSample = (s16*)(pBuf + _FramesToBytes(ptRuntimeData,nPos));
    if(nChannels==1 && ptDmaData->nChnId!=AUD_DMA_READER5)//hw only supports stereo
    {
        while(tFrames>0)
        {
            nGain = (s32)_AudUpdateGain(ptDmaData->nChnId);

            nSampleValue = (s32)(*pnInSample);
            nSampleValue = (nSampleValue * nGain)>>13;
            if(nSampleValue>MAX_S16)
                nSampleValue = MAX_S16;
            else if(nSampleValue<MIN_S16)
                nSampleValue = MIN_S16;
            *pnOutSample = (s16)nSampleValue;
            pnOutSample++;
            *pnOutSample = (s16)nSampleValue;
            pnOutSample++;
            pnInSample++;

            tFrames--;
        }
    }
    else
    {
        while(tFrames>0)
        {
            nGain = (s32)_AudUpdateGain(ptDmaData->nChnId);
            for(i=0; i<nChannels; i++)
            {
                //pnOutSample = pnInSample;
                nSampleValue = (s32)(*pnInSample);
                nSampleValue = (nSampleValue * nGain)>>13;
                if(nSampleValue>MAX_S16)
                    nSampleValue = MAX_S16;
                else if(nSampleValue<MIN_S16)
                    nSampleValue = MIN_S16;
                *pnOutSample = (s16)nSampleValue;

                pnOutSample++;
                pnInSample++;
            }
            tFrames--;
        }
    }

    return 0;
}

s32 _AudDmaRdCopyI32(struct PcmRuntimeData *ptRuntimeData, u32 nHwoff, u8* pBuf, u32 nBufSize, u32 nPos, PcmFrames_t tFrames)
{
    return 0;
}

//input noninterleave data
s32 _AudDmaRdCopyN16(struct PcmRuntimeData *ptRuntimeData, u32 nHwoff, u8* pBuf, u32 nBufSize, u32 nPos, PcmFrames_t tFrames)
{
    PcmDmaData_t *ptDmaData = (PcmDmaData_t*)ptRuntimeData->pPrivateData;
    u16 nChannels = _gaDmaParam[ptDmaData->nChnId].nChannels;
    s16 *pnInSample;
    s16 *pnOutSample;
    u32 nInterval = nBufSize/nChannels;
    u32 i;
    s32 nGain;
    s32 nSampleValue;

    pnOutSample = (s16*)(ptDmaData->pDmaArea + _HwFramesToBytes(ptRuntimeData,nHwoff));

    if(nChannels==1 && ptDmaData->nChnId!=AUD_DMA_READER5)//hw only supports stereo
    {
        pnInSample = (s16*)(pBuf + _FramesToBytes(ptRuntimeData,nPos));
        while(tFrames>0)
        {
            nGain = (s32)_AudUpdateGain(ptDmaData->nChnId);

            nSampleValue = (s32)(*pnInSample);
            nSampleValue = (nSampleValue * nGain)>>13;
            if(nSampleValue>MAX_S16)
                nSampleValue = MAX_S16;
            else if(nSampleValue<MIN_S16)
                nSampleValue = MIN_S16;
            *pnOutSample = (s16)nSampleValue;
            pnOutSample++;
            *pnOutSample = (s16)nSampleValue;
            pnOutSample++;
            pnInSample++;

            tFrames--;
        }
    }
    else
    {

        while(tFrames>0)
        {
            nGain = (s32)_AudUpdateGain(ptDmaData->nChnId);
            for(i=0; i<nChannels; i++)
            {
                pnInSample = (s16*)(pBuf + nInterval*i)+nPos;
                //pnOutSample = pnInSample;
                nSampleValue = (s32)(*pnInSample);
                nSampleValue = (nSampleValue * nGain)>>13;
                if(nSampleValue>MAX_S16)
                    nSampleValue = MAX_S16;
                else if(nSampleValue<MIN_S16)
                    nSampleValue = MIN_S16;
                *pnOutSample = (s16)nSampleValue;
                pnOutSample++;
            }
            tFrames --;
            nPos ++;
        }
    }

    return 0;
}

s32 _AudDmaRdCopyN32(struct PcmRuntimeData *ptRuntimeData, u32 nHwoff, u8* pBuf, u32 nBufSize, u32 nPos, PcmFrames_t tFrames)
{
    return 0;
}

//just copy(not handle wrap-around), function call should check enough size,
s32 _AudDmaKernelCopy(struct PcmRuntimeData *ptRuntimeData, u32 nHwoff, u8* pBuf, u32 nBufSize, u32 nPos, PcmFrames_t tFrames)
{
    PcmDmaData_t *ptDmaData = (PcmDmaData_t*)ptRuntimeData->pPrivateData;
    unsigned long nFlags;
    u32 nLevelCount = 0;
    u32 nTrigCount;
    u32 nOldState = ptRuntimeData->nState;
    u16 nRetry = 0;
    if (PCM_STREAM_PLAYBACK == ptRuntimeData->eStream)
    {
        AUD_PRINTF(DEBUG_LEVEL,"%s pos=%u, frames %lu\n",__FUNCTION__,nPos,tFrames);
        ptRuntimeData->copy(ptRuntimeData, nHwoff, pBuf, nBufSize, nPos, tFrames);


        spin_lock_irqsave(&ptDmaData->tLock, nFlags);
        if(nOldState != ptRuntimeData->nState && ptRuntimeData->nState == DMA_EMPTY)
        {
            spin_unlock_irqrestore(&ptDmaData->tLock, nFlags);
            AUD_PRINTF(DEBUG_LEVEL,"%s state changing!!!\n",__FUNCTION__);
            return 0;
        }
        Chip_Flush_MIU_Pipe();
        nLevelCount = HalAudDmaGetLevelCnt(ptDmaData->nChnId);

        nTrigCount = HalAudDmaTrigLevelCnt(ptDmaData->nChnId, (_HwFramesToBytes(ptRuntimeData,tFrames) + ptRuntimeData->nRemainCount));
        if (ptRuntimeData->nState == DMA_EMPTY)
        {
            HalAudDmaClearInt(ptDmaData->nChnId);
            ptRuntimeData->nState = DMA_NORMAL;
        }

        /* Be careful!! check level count updated*/
        while(nRetry<20)
        {
            if(HalAudDmaGetLevelCnt(ptDmaData->nChnId)>nLevelCount)
                break;
            nRetry++;
        }

        if(nRetry==20)
            AUD_PRINTF(ERROR_LEVEL,"update level count too slow!!!!!!!!\n");

        if (((nLevelCount + nTrigCount)  >= (ptDmaData->nBufBytes - _HwFramesToBytes(ptRuntimeData,ptDmaData->tPeriodSize))) && ptRuntimeData->nStatus == SND_PCM_STATE_RUNNING)
        {
            //udelay(1); //wait level count ready
            HalAudDmaIntEnable(ptDmaData->nChnId, TRUE, FALSE, FALSE);
            ptRuntimeData->nState = DMA_NORMAL;
        }

        if ((nLevelCount + nTrigCount)  > ptDmaData->nBufBytes )
            AUD_PRINTF(ERROR_LEVEL,"l:%u, t:%u, size %u!!!\n",nLevelCount,nTrigCount,_HwFramesToBytes(ptRuntimeData,tFrames));
#ifdef __HWPTR__
        ptRuntimeData->nDmaLevelCount += _HwFramesToBytes(ptRuntimeData,tFrames);
#endif
        ptRuntimeData->nRemainCount = (_HwFramesToBytes(ptRuntimeData,tFrames) + ptRuntimeData->nRemainCount) - nTrigCount;

        spin_unlock_irqrestore(&ptDmaData->tLock, nFlags);
    }
    else if (PCM_STREAM_CAPTURE == ptRuntimeData->eStream)
    {

        ptRuntimeData->copy(ptRuntimeData, nHwoff, pBuf, nBufSize, nPos, tFrames);

        spin_lock_irqsave(&ptDmaData->tLock, nFlags);

        if(nOldState != ptRuntimeData->nState && ptRuntimeData->nState == DMA_FULL)
        {
            spin_unlock_irqrestore(&ptDmaData->tLock, nFlags);
            return 0;
        }

        nLevelCount = HalAudDmaGetLevelCnt(ptDmaData->nChnId);
        nTrigCount = HalAudDmaTrigLevelCnt(ptDmaData->nChnId, (_HwFramesToBytes(ptRuntimeData,tFrames) + ptRuntimeData->nRemainCount));
        if (ptRuntimeData->nState == DMA_FULL)
        {
            HalAudDmaClearInt(ptDmaData->nChnId);
            ptRuntimeData->nState = DMA_NORMAL;
        }

        while(nRetry<20)
        {
            if(HalAudDmaGetLevelCnt(ptDmaData->nChnId)<nLevelCount)
                break;
            nRetry++;
        }

         if(nRetry==20)
            AUD_PRINTF(ERROR_LEVEL,"update level count too slow!!!!!!!!\n");


        if (((nLevelCount - nTrigCount)  < _HwFramesToBytes(ptRuntimeData,ptDmaData->tPeriodSize)) && _gaRuntimeData[ptDmaData->nChnId].nStatus == SND_PCM_STATE_RUNNING)
        {
            //udelay(1); //wait level count ready
            HalAudDmaIntEnable(ptDmaData->nChnId, TRUE, FALSE, FALSE);
            ptRuntimeData->nState = DMA_NORMAL;
        }

        if (nLevelCount < nTrigCount )
            AUD_PRINTF(ERROR_LEVEL,"l:%u, t:%u, size %u!!!\n",nLevelCount,nTrigCount,_HwFramesToBytes(ptRuntimeData,tFrames));
#ifdef __HWPTR__
        ptRuntimeData->nDmaLevelCount += _HwFramesToBytes(ptRuntimeData,tFrames);
#endif
        ptRuntimeData->nRemainCount = (_HwFramesToBytes(ptRuntimeData,tFrames) + ptRuntimeData->nRemainCount) - nTrigCount;
        if(ptRuntimeData->nRemainCount)
        {
            AUD_PRINTF(DEBUG_LEVEL,"%s remain count = %ul\n",__FUNCTION__,ptRuntimeData->nRemainCount);
        }

        spin_unlock_irqrestore(&ptDmaData->tLock, nFlags);
    }
    //AUD_PRINTF(DEBUG_LEVEL,"%s chn=%d, frames size %d bytes, Hw pos %d , level count %u\n",__FUNCTION__,ptDmaData->nChnId,_HwFramesToBytes(ptRuntimeData,tFrames),_HwFramesToBytes(ptRuntimeData,nHwoff),ptRuntimeData->nDmaLevelCount);
    AUD_PRINTF(DEBUG_LEVEL,"%s chn=%d, frames size %d bytes, Hw pos %d \n",__FUNCTION__,ptDmaData->nChnId,_HwFramesToBytes(ptRuntimeData,tFrames),_HwFramesToBytes(ptRuntimeData,nHwoff));
    return 0;
}

/* To-Do : Only for special case?*/
U32 _AudDmaRdTrigLevelCountAll(U32 nDataSize)
{
    AudDmaChn_e eDmaChannel;
    u32 nTrigCount = 0;

    for(eDmaChannel = AUD_DMA_READER1; eDmaChannel<=AUD_DMA_READER5; eDmaChannel++)
    {
        if(_gaRuntimeData[eDmaChannel].nStatus == SND_PCM_STATE_RUNNING
            || _gaRuntimeData[eDmaChannel].nStatus == SND_PCM_STATE_PREPARED
            ||_gaRuntimeData[eDmaChannel].nStatus == SND_PCM_STATE_PAUSED)
        {
            nTrigCount = HalAudDmaTrigLevelCnt(eDmaChannel, nDataSize);
        }
    }

    return nTrigCount;
}

// for DrvAudWritePcmAll
s32 _AudDmaRdKernelCopyAll(struct PcmRuntimeData *ptRuntimeData, u32 nHwoff, u8* pBuf, u32 nBufSize, u32 nPos, PcmFrames_t tFrames)
{
    PcmDmaData_t *ptDmaData = (PcmDmaData_t*)ptRuntimeData->pPrivateData;
    unsigned long nFlags;
    u32 nLevelCount = 0;
    u32 nTrigCount;
    u32 nOldState = ptRuntimeData->nState;
    u16 nRetry = 0;
    if (PCM_STREAM_PLAYBACK == ptRuntimeData->eStream)
    {
        AUD_PRINTF(DEBUG_LEVEL,"%s pos=%u, frames %lu\n",__FUNCTION__,nPos,tFrames);
        ptRuntimeData->copy(ptRuntimeData, nHwoff, pBuf, nBufSize, nPos, tFrames);


        spin_lock_irqsave(&ptDmaData->tLock, nFlags);
        if(nOldState != ptRuntimeData->nState && ptRuntimeData->nState == DMA_EMPTY)
        {
            spin_unlock_irqrestore(&ptDmaData->tLock, nFlags);
            AUD_PRINTF(DEBUG_LEVEL,"%s state changing!!!\n",__FUNCTION__);
            return 0;
        }
        Chip_Flush_MIU_Pipe();
        nLevelCount = HalAudDmaGetLevelCnt(ptDmaData->nChnId);

        nTrigCount = _AudDmaRdTrigLevelCountAll(_HwFramesToBytes(ptRuntimeData,tFrames) + ptRuntimeData->nRemainCount);
        if (ptRuntimeData->nState == DMA_EMPTY)
        {
            HalAudDmaClearInt(ptDmaData->nChnId);
            ptRuntimeData->nState = DMA_NORMAL;
        }

        /* Be careful!! check level count updated*/
        while(nRetry<20)
        {
            if(HalAudDmaGetLevelCnt(ptDmaData->nChnId)>nLevelCount)
                break;
            nRetry++;
        }

        if(nRetry==20)
            AUD_PRINTF(ERROR_LEVEL,"update level count too slow!!!!!!!!\n");


        if (((nLevelCount + nTrigCount)  >= (ptDmaData->nBufBytes - _HwFramesToBytes(ptRuntimeData,ptDmaData->tPeriodSize))) && ptRuntimeData->nStatus == SND_PCM_STATE_RUNNING)
        {
            //udelay(1); //wait level count ready
            HalAudDmaIntEnable(ptDmaData->nChnId, TRUE, FALSE, FALSE);
            ptRuntimeData->nState = DMA_NORMAL;
        }

        if ((nLevelCount + nTrigCount)  > ptDmaData->nBufBytes )
            AUD_PRINTF(ERROR_LEVEL,"l:%u, t:%u, size %u!!!\n",nLevelCount,nTrigCount,_HwFramesToBytes(ptRuntimeData,tFrames));
#ifdef __HWPTR__
        ptRuntimeData->nDmaLevelCount += _HwFramesToBytes(ptRuntimeData,tFrames);
#endif
        ptRuntimeData->nRemainCount = (_HwFramesToBytes(ptRuntimeData,tFrames) + ptRuntimeData->nRemainCount) - nTrigCount;

        spin_unlock_irqrestore(&ptDmaData->tLock, nFlags);
    }
   /* else if (PCM_STREAM_CAPTURE == ptRuntimeData->eStream)
    {

        ptRuntimeData->copy(ptRuntimeData, nHwoff, pBuf, nBufSize, nPos, tFrames);

        spin_lock_irqsave(&ptDmaData->tLock, nFlags);

        if(nOldState != ptRuntimeData->nState && ptRuntimeData->nState == DMA_FULL)
        {
            spin_unlock_irqrestore(&ptDmaData->tLock, nFlags);
            return 0;
        }

        nLevelCount = HalAudDmaGetLevelCnt(ptDmaData->nChnId);
        nTrigCount = HalAudDmaTrigLevelCnt(ptDmaData->nChnId, (_HwFramesToBytes(ptRuntimeData,tFrames) + ptRuntimeData->nRemainCount));
        if (ptRuntimeData->nState == DMA_FULL)
        {
            HalAudDmaClearInt(ptDmaData->nChnId);
            ptRuntimeData->nState = DMA_NORMAL;
        }

        while(nRetry<16)
        {
            if(HalAudDmaGetLevelCnt(ptDmaData->nChnId)<nLevelCount)
                break;
            nRetry++;
        }

        if (((nLevelCount - nTrigCount)  < _HwFramesToBytes(ptRuntimeData,ptDmaData->tPeriodSize)) && _gaRuntimeData[ptDmaData->nChnId].nStatus == SND_PCM_STATE_RUNNING)
        {
            //udelay(1); //wait level count ready
            HalAudDmaIntEnable(ptDmaData->nChnId, TRUE, FALSE, FALSE);
            ptRuntimeData->nState = DMA_NORMAL;
        }

        if (nLevelCount < nTrigCount )
            AUD_PRINTF(ERROR_LEVEL,"l:%u, t:%u, size %u!!!\n",nLevelCount,nTrigCount,_HwFramesToBytes(ptRuntimeData,tFrames));
#ifdef __HWPTR__
        ptRuntimeData->nDmaLevelCount += _HwFramesToBytes(ptRuntimeData,tFrames);
#endif
        ptRuntimeData->nRemainCount = (_HwFramesToBytes(ptRuntimeData,tFrames) + ptRuntimeData->nRemainCount) - nTrigCount;
        if(ptRuntimeData->nRemainCount)
        {
            AUD_PRINTF(DEBUG_LEVEL,"%s remain count = %ul\n",__FUNCTION__,ptRuntimeData->nRemainCount);
        }

        spin_unlock_irqrestore(&ptDmaData->tLock, nFlags);
    }*/
    //AUD_PRINTF(DEBUG_LEVEL,"%s chn=%d, frames size %d bytes, Hw pos %d , level count %u\n",__FUNCTION__,ptDmaData->nChnId,_HwFramesToBytes(ptRuntimeData,tFrames),_HwFramesToBytes(ptRuntimeData,nHwoff),ptRuntimeData->nDmaLevelCount);
    AUD_PRINTF(DEBUG_LEVEL,"%s chn=%d, frames size %d bytes, Hw pos %d \n",__FUNCTION__,ptDmaData->nChnId,_HwFramesToBytes(ptRuntimeData,tFrames),_HwFramesToBytes(ptRuntimeData,nHwoff));
    return 0;
}

void DrvAudInit(void)
{
    u16 n;
//    struct device_node *pDevNode;

    AUD_PRINTF(TRACE_LEVEL,"%s\n",__FUNCTION__);
    HalAudSysSetBankBaseAddr(IO_ADDRESS(0x1F000000));
    HalAudSysInit();
    HalAudConfigInit();

    HalAudIntEnable(AUD_AU_INT, AUD_INT_EN_MSK);

    for(n=0; n<AUD_I2S_NUM; n++)
    {
        HalAudI2sSetFmt((AudI2s_e)n, AUD_I2S_FMT_LEFT_JUSTIFY);
        HalAudI2sSetTdmMode((AudI2s_e)n, AUD_I2S_MODE_I2S);
        if(n==AUD_I2S_BT_TX)
            HalAudI2sSetMsMode((AudI2s_e)n, AUD_I2S_MSMODE_SLAVE);
        else
            HalAudI2sSetMsMode((AudI2s_e)n, AUD_I2S_MSMODE_MASTER);
        HalAudI2sSetWidth((AudI2s_e)n, AUD_BITWIDTH_16);
        HalAudI2sSetTdmChannel((AudI2s_e)n, 4);
    }

    for(n=0; n<AUD_I2S_IF_NUM; n++)
        _gbI2sMaster[n]=TRUE;

    for(n=0; n<AUD_DMA_NUM; n++)
    {
        memset(&_gaRuntimeData[n], 0, sizeof(PcmRuntimeData_t));
        _gaRuntimeData[n].nStatus= SND_PCM_STATE_INIT;
        spin_lock_init(&_gaRuntimeData[n].tPcmLock);
        init_waitqueue_head(&_gaRuntimeData[n].tSleep);
        if(n<AUD_DMA_READER1)
        {
            _gaRuntimeData[n].eStream = PCM_STREAM_CAPTURE;
        }
        else
        {
            _gaRuntimeData[n].eStream = PCM_STREAM_PLAYBACK;
        }
        _gaGainData[n].nCurrentGainLevel = (0-MIN_GAIN);
        _gaGainData[n].nTargetGainLevel = (0-MIN_GAIN);
        _gaGainData[n].nGainStep = GAIN_STEP;
    }
#if 0
    pDevNode = of_find_compatible_node(NULL, NULL, "mstar,audio");
    if(pDevNode)
    {
        INFINITY_IRQ_ID = of_irq_to_resource(of_node_get(pDevNode), 0, NULL);
        of_node_put(pDevNode);
        AUD_PRINTF(TRACE_LEVEL,"%s irq ID = %d\n",__FUNCTION__,INFINITY_IRQ_ID);
    }
    else
    {
        AUD_PRINTF(ERROR_LEVEL,"failed to find device node\n");
    }
#endif
}

//To-Do : MISC I2s slave mode with MCLK
BOOL DrvAudConfigI2s(AudI2sIf_e eI2sIf, AudI2sCfg_t *ptI2sCfg)
{
    BOOL bRet;
    AudI2s_e eI2s;
    AUD_PRINTF(TRACE_LEVEL,"%s I2s=%d\n",__FUNCTION__,eI2sIf);
    switch(eI2sIf)
    {
        case AUD_I2S_IF_MISC_RX:
            eI2s = AUD_I2S_MISC_RX;
            if(ptI2sCfg->eMsMode == AUD_I2S_MSMODE_SLAVE)
                return FALSE;
            break;
        case AUD_I2S_IF_MISC_TX:
            eI2s = AUD_I2S_MISC_TX;
            if(ptI2sCfg->eMsMode == AUD_I2S_MSMODE_SLAVE)
                return FALSE;
            break;
        case AUD_I2S_IF_CODEC_RX:
            eI2s = AUD_I2S_CODEC_RX;
            break;
        case AUD_I2S_IF_CODEC_TX:
            eI2s = AUD_I2S_CODEC_TX;
            break;
        case AUD_I2S_IF_BT_TRX://TRX 4 wire mode, Tx should be slave mode
            eI2s = AUD_I2S_BT_RX;
            bRet = HalAudI2sSetFmt(AUD_I2S_BT_TX, ptI2sCfg->eFormat);
            bRet &= HalAudI2sSetTdmMode(AUD_I2S_BT_TX, ptI2sCfg->eMode);
            bRet &= HalAudI2sSetWidth(AUD_I2S_BT_TX, ptI2sCfg->eWidth);
            bRet &= HalAudI2sSetTdmChannel(AUD_I2S_BT_TX, ptI2sCfg->nTdmChannel);
            if(bRet == FALSE)
                return bRet;
            break;
        default:
            return FALSE;
    }
    bRet = HalAudI2sSetFmt(eI2s, ptI2sCfg->eFormat);
    bRet &= HalAudI2sSetTdmMode(eI2s, ptI2sCfg->eMode);
    bRet &= HalAudI2sSetMsMode(eI2s,ptI2sCfg->eMsMode);
    bRet &= HalAudI2sSetWidth(eI2s, ptI2sCfg->eWidth);
    bRet &= HalAudI2sSetTdmChannel(eI2s, ptI2sCfg->nTdmChannel);

    _gbI2sMaster[eI2sIf] = (ptI2sCfg->eMsMode==AUD_I2S_MSMODE_MASTER)? TRUE:FALSE;

    return bRet;
}

BOOL DrvAudEnableI2s(AudI2sIf_e eI2sIf, BOOL bEnable)
{
    AudI2s_e eI2s;
    if(_gbI2sMaster[eI2sIf]==FALSE) //only master mode should enable clock
        return TRUE;
    AUD_PRINTF(TRACE_LEVEL,"%s I2s[%d] = %d\n",__FUNCTION__,eI2sIf,bEnable);
    switch(eI2sIf)
    {
        case AUD_I2S_IF_MISC_RX:
            eI2s = AUD_I2S_MISC_RX;
            break;
        case AUD_I2S_IF_MISC_TX:
            eI2s = AUD_I2S_MISC_TX;
            break;
        case AUD_I2S_IF_CODEC_RX:
            eI2s = AUD_I2S_CODEC_RX;
            break;
        case AUD_I2S_IF_CODEC_TX:
            eI2s = AUD_I2S_CODEC_TX;
            break;
        case AUD_I2S_IF_BT_TRX:
            eI2s = AUD_I2S_BT_RX;
            break;
        default:
            AUD_PRINTF(ERROR_LEVEL,"%s I2s not exist\n",__FUNCTION__);
            return FALSE;

    }
    return HalAudI2sEnable(eI2s, bEnable);
}

//mux have to be configured before dma rate
BOOL DrvAudSetMux(AudMux_e eMux, u8 nChoice)
{
    AUD_PRINTF(TRACE_LEVEL,"%s Mux[%d] = %d\n",__FUNCTION__,eMux,nChoice);
    //DMA_W5 only support AMIC & DMIC in SW use case
    if(eMux==AUD_MUX4_DMA_W5 && nChoice==3)
        return FALSE;

    if(HalAudSetMux(eMux, nChoice))
    {
        _gaMuxSel[eMux]=nChoice;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


BOOL DrvAudSetMixer(AudMixer_e eMixer, u8 nChoice)
{
    AUD_PRINTF(TRACE_LEVEL,"%s Mixer[%d] = %d\n",__FUNCTION__,eMixer,nChoice);
    return HalAudSetMixer(eMixer, nChoice);
}


BOOL DrvAudEnableAtop(AudAtopPath_e eAtop, BOOL bEnable)
{
    AUD_PRINTF(TRACE_LEVEL,"%s Atop[%d] = %d\n",__FUNCTION__,eAtop,bEnable);
    if(bEnable)
    {
        return HalAudAtopOpen(eAtop);
    }
    else
    {
        return HalAudAtopClose(eAtop);
    }
}

BOOL DrvAudConfigDmaParam(AudDmaChn_e eDmaChannel, DmaParam_t *ptParam)
{
    AudBitWidth_e eBitWidth;
    BOOL bRet =TRUE;
    AUD_PRINTF(TRACE_LEVEL,"%s chn = %d,status %d\n",__FUNCTION__,eDmaChannel,_gaRuntimeData[eDmaChannel].nStatus);
    if(_gaRuntimeData[eDmaChannel].nStatus!= SND_PCM_STATE_INIT
            && _gaRuntimeData[eDmaChannel].nStatus != SND_PCM_STATE_SETUP)
    {
        AUD_PRINTF(ERROR_LEVEL,"%s error status\n",__FUNCTION__);
        return FALSE;
    }

    switch(ptParam->nBitWidth)
    {
        case 16:
            eBitWidth=AUD_BITWIDTH_16;
            break;
        case 24:
            eBitWidth=AUD_BITWIDTH_24;
            break;
        case 32:
            eBitWidth=AUD_BITWIDTH_32;
            break;
        default:
            AUD_PRINTF(ERROR_LEVEL,"%s bitwidth not support\n",__FUNCTION__);
            return FALSE;
    }

    switch(ptParam->nSampleRate)
    {
        case 8000:
        case 16000:
        case 32000:
        case 48000:
            break;
        default:
            AUD_PRINTF(ERROR_LEVEL,"%s sample rate not support\n",__FUNCTION__);
            return FALSE;
    }


    if(eDmaChannel>=AUD_DMA_READER1)
    {
        if(ptParam->nChannels<2 && eDmaChannel!=AUD_DMA_READER5)//SW should support mono playback, but HW only support stereo
        {
            AUD_PRINTF(TRACE_LEVEL,"%s reset to stereo\n",__FUNCTION__);
            bRet = _AudDmaInitChannel(eDmaChannel,ptParam->pDmaBuf,ptParam->nPhysDmaAddr,ptParam->nBufferSize,2,eBitWidth,ptParam->nPeriodSize*2);
        }
        else
            bRet = _AudDmaInitChannel(eDmaChannel,ptParam->pDmaBuf,ptParam->nPhysDmaAddr,ptParam->nBufferSize,ptParam->nChannels,eBitWidth,ptParam->nPeriodSize);
    }
    else //writer period size should add dma local buffer size
    {
        bRet = _AudDmaInitChannel(eDmaChannel,ptParam->pDmaBuf,ptParam->nPhysDmaAddr,ptParam->nBufferSize,ptParam->nChannels,eBitWidth,(ptParam->nPeriodSize)?ptParam->nPeriodSize+DMA_LOCALBUF_SIZE:ptParam->nPeriodSize);
    }



    if(bRet)
    {
        _gaRuntimeData[eDmaChannel].nFrameBits = ptParam->nChannels * ptParam->nBitWidth;

        memcpy(&_gaDmaParam[eDmaChannel],ptParam,sizeof(DmaParam_t));
        if(ptParam->nChannels<2 && eDmaChannel!= AUD_DMA_READER5)//writer does not support mono, so only reader 1~4 should support sw mono
        {
            _gaDmaParam[eDmaChannel].nStartThres *= 2;
            _gaDmaParam[eDmaChannel].nPeriodSize *= 2;
            _gaRuntimeData[eDmaChannel].nHwFrameBits = _gaRuntimeData[eDmaChannel].nFrameBits*2;
        }
        else
            _gaRuntimeData[eDmaChannel].nHwFrameBits = _gaRuntimeData[eDmaChannel].nFrameBits;
        _gaRuntimeData[eDmaChannel].nStatus = SND_PCM_STATE_SETUP;
    }

    return bRet;

}


// To-do : bypass mode
BOOL DrvAudOpenDma(AudDmaChn_e eDmaChannel)
{
    s32 err = 0;
    PcmDmaData_t *ptPcmData;
    u8 szName[20];
    AudDmaClkSrc_e eClkSrc = AUD_DMA_CLK_GPA;
    AudRate_e eSampleRate;
    BOOL bRet =TRUE;
    AUD_PRINTF(TRACE_LEVEL,"in %s\n",__FUNCTION__);
    if(_gaRuntimeData[eDmaChannel].nStatus != SND_PCM_STATE_SETUP)
    {
        AUD_PRINTF(ERROR_LEVEL,"%s status wrong\n",__FUNCTION__);
        return FALSE;
    }

    ptPcmData = kzalloc(sizeof(PcmDmaData_t), GFP_KERNEL);
    if (ptPcmData== NULL)
    {
        AUD_PRINTF(ERROR_LEVEL,"%s insufficient resource\n",__FUNCTION__);
        return FALSE;
    }
    sprintf(szName,"dma%d",(s16)eDmaChannel);
    memcpy(ptPcmData->szName, szName, strlen(szName));
    ptPcmData->nChnId = eDmaChannel;
    ptPcmData->pDmaArea = _gaDmaParam[eDmaChannel].pDmaBuf;
    ptPcmData->tDmaAddr = _gaDmaParam[eDmaChannel].nPhysDmaAddr;
    ptPcmData->nBufBytes = _gaDmaParam[eDmaChannel].nBufferSize;
    ptPcmData->tBufSize = _BytesToHwFrames(&_gaRuntimeData[eDmaChannel], _gaDmaParam[eDmaChannel].nBufferSize);
    ptPcmData->tPeriodSize = _BytesToHwFrames(&_gaRuntimeData[eDmaChannel] ,_gaDmaParam[eDmaChannel].nPeriodSize);
#ifndef __HWPTR__
    ptPcmData->tApplPtr = 0;
#else
    ptPcmData->tHwPtr = ptPcmData->tApplPtr = 0;
    ptPcmData->tBoundary = 2*ptPcmData->tBufSize;
#endif
    memset(ptPcmData->pDmaArea,0 ,ptPcmData->nBufBytes);
    spin_lock_init(&ptPcmData->tLock);
    _gaRuntimeData[eDmaChannel].pPrivateData = ptPcmData;
    AUD_PRINTF(TRACE_LEVEL, "IRQ_ID = 0x%x\n", INFINITY_IRQ_ID);
    if(_gaRuntimeData[eDmaChannel].bEnableIrq)
    {
        err = request_irq(INFINITY_IRQ_ID, //INT_MS_AUDIO_1,
                          _AudDmaIrq,
                          IRQF_SHARED,
                          szName,
                          (void *)&_gaRuntimeData[eDmaChannel]);
        if (err)
        {
            AUD_PRINTF(ERROR_LEVEL,"%s request irq err=%d\n",__FUNCTION__,err);
            return FALSE;
        }
    }

    switch(_gaDmaParam[eDmaChannel].nSampleRate)
    {
        case 8000:
            eSampleRate=AUD_RATE_8K;
            break;
        case 16000:
            eSampleRate=AUD_RATE_16K;
            break;
        case 32000:
            eSampleRate=AUD_RATE_32K;
            break;
        case 48000:
            eSampleRate=AUD_RATE_48K;
            break;
    }


    if(eDmaChannel==AUD_DMA_WRITER1)
    {
        if(_gaMuxSel[AUD_MUX2_DMA_W1]==0)
        {
            eClkSrc = AUD_DMA_CLK_ADC;
        }
    }
    else if(eDmaChannel==AUD_DMA_WRITER5)
    {
        if(_gaMuxSel[AUD_MUX4_DMA_W5]==0x2) // no I2S case
        {
            eClkSrc = AUD_DMA_CLK_ADC;
        }
        else
        {
            eClkSrc = AUD_DMA_CLK_PDM;
        }
    }
    else if(eDmaChannel==AUD_DMA_READER5)
    {
        eClkSrc = AUD_DMA_CLK_SYNTH;
    }
    bRet = _AudDmaSetRate(eDmaChannel,eClkSrc,eSampleRate);

    if(!bRet)
        return bRet;



    switch(_gaDmaParam[eDmaChannel].nBitWidth)
    {
        case 16:
            if(_gaDmaParam[eDmaChannel].nInterleaved)
            {
                if(_gaRuntimeData[eDmaChannel].eStream == PCM_STREAM_PLAYBACK)
                    _gaRuntimeData[eDmaChannel].copy = _AudDmaRdCopyI16;
                else
                    _gaRuntimeData[eDmaChannel].copy = _AudDmaWrCopyI16;
            }
            else
            {
                if(_gaRuntimeData[eDmaChannel].eStream == PCM_STREAM_PLAYBACK)
                    _gaRuntimeData[eDmaChannel].copy = _AudDmaRdCopyN16;
                else
                    _gaRuntimeData[eDmaChannel].copy = _AudDmaWrCopyN16;
            }
            break;
        case 32:
        default:
            if(_gaDmaParam[eDmaChannel].nInterleaved)
            {
                if(_gaRuntimeData[eDmaChannel].eStream == PCM_STREAM_PLAYBACK)
                    _gaRuntimeData[eDmaChannel].copy = _AudDmaRdCopyI32;
                else
                    _gaRuntimeData[eDmaChannel].copy = _AudDmaWrCopyI32;
            }
            else
            {
                if(_gaRuntimeData[eDmaChannel].eStream == PCM_STREAM_PLAYBACK)
                    _gaRuntimeData[eDmaChannel].copy = _AudDmaRdCopyN32;
                else
                    _gaRuntimeData[eDmaChannel].copy = _AudDmaWrCopyN32;
            }
            break;

    }

    _gaRuntimeData[eDmaChannel].nStatus = SND_PCM_STATE_OPEN;

    return TRUE;
}


BOOL DrvAudPrepareDma(AudDmaChn_e eDmaChannel)
{
    PcmDmaData_t *ptPcmData = (PcmDmaData_t*)_gaRuntimeData[eDmaChannel].pPrivateData;
    AUD_PRINTF(TRACE_LEVEL,"in %s\n",__FUNCTION__);

    if(_gaRuntimeData[eDmaChannel].nStatus != SND_PCM_STATE_OPEN
            && _gaRuntimeData[eDmaChannel].nStatus != SND_PCM_STATE_XRUN )
    {
        AUD_PRINTF(ERROR_LEVEL,"%s status error!\n",__FUNCTION__);
        return FALSE;
    }

    //_gaRuntimeData[eDmaChannel].nDmaLevelCount = 0;
    _gaRuntimeData[eDmaChannel].nRemainCount = 0;
    _gaRuntimeData[eDmaChannel].nState = DMA_EMPTY;

    //ptPcmData->tHwPtr = ptPcmData->tApplPtr = 0;
    ptPcmData->tApplPtr = 0;
    memset(ptPcmData->pDmaArea,0 ,ptPcmData->nBufBytes);

    _gaRuntimeData[eDmaChannel].nStatus = SND_PCM_STATE_PREPARED;
    return TRUE;

}



BOOL DrvAudStartDma(AudDmaChn_e eDmaChannel)
{
    BOOL bRet;
    AUD_PRINTF(TRACE_LEVEL,"in %s\n",__FUNCTION__);
    if(_gaRuntimeData[eDmaChannel].nStatus != SND_PCM_STATE_PREPARED)
    {
        AUD_PRINTF(ERROR_LEVEL,"%s error status %d\n",__FUNCTION__,_gaRuntimeData[eDmaChannel].nStatus);
        return FALSE;
    }

    bRet = _AudDmaStartChannel(eDmaChannel);
    if(bRet)
    {
        _gaRuntimeData[eDmaChannel].nStatus = SND_PCM_STATE_RUNNING;
    }


    if(_gaRuntimeData[eDmaChannel].bEnableIrq)
    {
        HalAudDmaIntEnable(eDmaChannel,TRUE,FALSE,FALSE);
    }


    return bRet;
}

BOOL DrvAudStopDma(AudDmaChn_e eDmaChannel)
{
    BOOL bRet = TRUE;
    AUD_PRINTF(TRACE_LEVEL,"in %s\n",__FUNCTION__);
    if(_gaRuntimeData[eDmaChannel].bEnableIrq)
    {
        bRet = HalAudDmaIntEnable(eDmaChannel,FALSE,FALSE,FALSE);
    }
    bRet &= _AudDmaStopChannel(eDmaChannel);


    if(bRet)
    {
        _gaRuntimeData[eDmaChannel].nStatus = SND_PCM_STATE_OPEN;
    }
    return bRet;
}

BOOL DrvAudPauseDma(AudDmaChn_e eDmaChannel)
{
    BOOL bRet = TRUE;
    AUD_PRINTF(TRACE_LEVEL,"in %s\n",__FUNCTION__);
    if(_gaRuntimeData[eDmaChannel].nStatus != SND_PCM_STATE_RUNNING)
    {
        return FALSE;
    }
    else
    {
        if((bRet = HalAudDmaPause(eDmaChannel)))
        {
            _gaRuntimeData[eDmaChannel].nStatus = SND_PCM_STATE_PAUSED;
            return TRUE;
        }
        return bRet;
    }

}

BOOL DrvAudResumeDma(AudDmaChn_e eDmaChannel)
{
    BOOL bRet;
    AUD_PRINTF(TRACE_LEVEL,"in %s\n",__FUNCTION__);
    if(_gaRuntimeData[eDmaChannel].nStatus != SND_PCM_STATE_PAUSED)
    {
        return FALSE;
    }
    else
    {
        if((bRet = HalAudDmaResume(eDmaChannel)))
        {
            _gaRuntimeData[eDmaChannel].nStatus = SND_PCM_STATE_RUNNING;
            return TRUE;
        }
        return bRet;
    }
}

BOOL DrvAudCloseDma(AudDmaChn_e eDmaChannel)
{
    AUD_PRINTF(TRACE_LEVEL,"in %s\n",__FUNCTION__);
    if(_gaRuntimeData[eDmaChannel].bEnableIrq)
        free_irq(INFINITY2_AUDIO_IRQ_ID, (void *)&_gaRuntimeData[eDmaChannel]);

    if(_gaRuntimeData[eDmaChannel].pPrivateData)
    {
        kfree(_gaRuntimeData[eDmaChannel].pPrivateData);
        _gaRuntimeData[eDmaChannel].pPrivateData = NULL;
    }
    _gaRuntimeData[eDmaChannel].nStatus = SND_PCM_STATE_INIT;
    return TRUE;
}

BOOL DrvAudIsXrun(AudDmaChn_e eDmaChannel)
{
    return (_gaRuntimeData[eDmaChannel].nStatus==SND_PCM_STATE_XRUN);
}


s32 DrvAudWritePcm(AudDmaChn_e eDmaChannel, void *pWrBuffer, u32 nSize, BOOL bBlock)
{
    PcmDmaData_t *ptDmaData = (PcmDmaData_t *)(_gaRuntimeData[eDmaChannel].pPrivateData);
    s32 nErr=0;
    PcmFrames_t tAvail;
    PcmFrames_t tFreeSize;
    PcmFrames_t tWorkSize;
    PcmFrames_t tOffset=0;
    PcmFrames_t tApplptr;
    PcmFrames_t tBufSize;
    AUD_PRINTF(DEBUG_LEVEL,"%s chn=%d\n",__FUNCTION__,eDmaChannel);
    if (nSize == 0)
        return 0;
    else
        tBufSize = _BytesToFrames(&_gaRuntimeData[eDmaChannel],nSize);

    spin_lock_irq(&_gaRuntimeData[eDmaChannel].tPcmLock);
    switch (_gaRuntimeData[eDmaChannel].nStatus)
    {
        case SND_PCM_STATE_PREPARED:
        case SND_PCM_STATE_RUNNING:
        case SND_PCM_STATE_PAUSED:
            break;
        case SND_PCM_STATE_XRUN:
            nErr = -EPIPE;
            goto _end_unlock;
        default:
            nErr = -EBADFD;
            goto _end_unlock;
    }

#ifdef __HWPTR__
    if (_gaRuntimeData[eDmaChannel].nStatus == SND_PCM_STATE_RUNNING)
        _AudDmaUpdateHwPointer(&_gaRuntimeData[eDmaChannel]);
#endif

    //_AudDmaUpdateHwLevel(&_gaRuntimeData[eDmaChannel]);
    tAvail = _AudPcmPlaybackAvail(ptDmaData);
    AUD_PRINTF(DEBUG_LEVEL,"%s tAvail %lu,tBufSize %lu\n",__FUNCTION__,tAvail,tBufSize);
    while(tBufSize>0)
    {
        if(tAvail==0)
        {
            if (!bBlock)
            {
                nErr = -EAGAIN;
                goto _end_unlock;
            }
            AUD_PRINTF(DEBUG_LEVEL,"_WaitForAvail\n");
            nErr = _WaitForAvail(&_gaRuntimeData[eDmaChannel], &tAvail);
            if (nErr < 0)
                goto _end_unlock;
            AUD_PRINTF(DEBUG_LEVEL,"_WaitForAvail avail %lu\n",tAvail);
        }

        tWorkSize = (tBufSize > tAvail? tAvail : tBufSize);
        tFreeSize = ptDmaData->tBufSize - (ptDmaData->tApplPtr%ptDmaData->tBufSize);

        if(tWorkSize > tFreeSize)
        {
            tWorkSize = tFreeSize;
        }

        //AUD_PRINTF(ERROR_LEVEL,"tOffset %ld, tWorkSize %ld, tAvail %ld\n",tOffset,tWorkSize,tAvail);

        if(tWorkSize == 0)
        {
            nErr = -EINVAL;
            goto _end_unlock;
        }

        tApplptr = ptDmaData->tApplPtr%ptDmaData->tBufSize;

        spin_unlock_irq(&_gaRuntimeData[eDmaChannel].tPcmLock);
        //if(tOffset)
        //    AUD_PRINTF(ERROR_LEVEL,"tOffset %ld!!!!!\n",tOffset);
        nErr = _AudDmaKernelCopy(&_gaRuntimeData[eDmaChannel], tApplptr, pWrBuffer, nSize, tOffset, tWorkSize);
        spin_lock_irq(&_gaRuntimeData[eDmaChannel].tPcmLock);

        if(nErr<0)
            goto _end_unlock;

        switch (_gaRuntimeData[eDmaChannel].nStatus)
        {
            case SND_PCM_STATE_XRUN:
                nErr = -EPIPE;
                goto _end_unlock;

            default:
                break;
        }
        tApplptr += tWorkSize;
        if (tApplptr >= ptDmaData->tBufSize)
        {
            tApplptr -= ptDmaData->tBufSize;
        }
        ptDmaData->tApplPtr = tApplptr;
        tOffset += tWorkSize;
        tBufSize -= tWorkSize;
        tAvail -= tWorkSize;

        //_AudDmaUpdateHwLevel(&_gaRuntimeData[eDmaChannel]);
#ifdef __EN_START_TH__
        if (_gaRuntimeData[eDmaChannel].nStatus == SND_PCM_STATE_PREPARED &&
                _AudPcmPlaybackAvail(ptDmaData) >= _BytesToFrames(&_gaRuntimeData[eDmaChannel],_gaDmaParam[eDmaChannel].nStartThres))
        {
            AUD_PRINTF(TRACE_LEVEL,"%s, DrvAudStartDma %ld\n",__func__,_AudPcmPlaybackAvail(ptDmaData));
            nErr = DrvAudStartDma(eDmaChannel);
            if (nErr < 0)
                goto _end_unlock;
        }
#endif
    }

_end_unlock:

    spin_unlock_irq(&_gaRuntimeData[eDmaChannel].tPcmLock);
    return nErr < 0 ? nErr:_FramesToBytes(&_gaRuntimeData[eDmaChannel],tOffset);//nSize;
}

//for special case, all running pcm reader works on the same dma buffer
s32 DrvAudWritePcmAll(AudDmaChn_e eDmaChannel, void *pWrBuffer, u32 nSize, BOOL bBlock)
{
    PcmDmaData_t *ptDmaData = (PcmDmaData_t *)(_gaRuntimeData[eDmaChannel].pPrivateData);
    s32 nErr=0;
    PcmFrames_t tAvail;
    PcmFrames_t tFreeSize;
    PcmFrames_t tWorkSize;
    PcmFrames_t tOffset=0;
    PcmFrames_t tApplptr;
    PcmFrames_t tBufSize;
    AUD_PRINTF(DEBUG_LEVEL,"%s chn=%d\n",__FUNCTION__,eDmaChannel);
    if (nSize == 0)
        return 0;
    else
        tBufSize = _BytesToFrames(&_gaRuntimeData[eDmaChannel],nSize);

    spin_lock_irq(&_gaRuntimeData[eDmaChannel].tPcmLock);
    switch (_gaRuntimeData[eDmaChannel].nStatus)
    {
        case SND_PCM_STATE_PREPARED:
        case SND_PCM_STATE_RUNNING:
        case SND_PCM_STATE_PAUSED:
            break;
        case SND_PCM_STATE_XRUN:
            nErr = -EPIPE;
            goto _end_unlock;
        default:
            nErr = -EBADFD;
            goto _end_unlock;
    }

#ifdef __HWPTR__
    if (_gaRuntimeData[eDmaChannel].nStatus == SND_PCM_STATE_RUNNING)
        _AudDmaUpdateHwPointer(&_gaRuntimeData[eDmaChannel]);
#endif

    //_AudDmaUpdateHwLevel(&_gaRuntimeData[eDmaChannel]);
    tAvail = _AudPcmPlaybackAvail(ptDmaData);
    AUD_PRINTF(DEBUG_LEVEL,"%s tAvail %lu,tBufSize %lu\n",__FUNCTION__,tAvail,tBufSize);
    while(tBufSize>0)
    {
        if(tAvail==0)
        {
            if (!bBlock)
            {
                nErr = -EAGAIN;
                goto _end_unlock;
            }
            AUD_PRINTF(DEBUG_LEVEL,"_WaitForAvail\n");
            nErr = _WaitForAvail(&_gaRuntimeData[eDmaChannel], &tAvail);
            if (nErr < 0)
                goto _end_unlock;
            AUD_PRINTF(DEBUG_LEVEL,"_WaitForAvail avail %lu\n",tAvail);
        }

        tWorkSize = (tBufSize > tAvail? tAvail : tBufSize);
        tFreeSize = ptDmaData->tBufSize - (ptDmaData->tApplPtr%ptDmaData->tBufSize);

        if(tWorkSize > tFreeSize)
        {
            tWorkSize = tFreeSize;
        }

        //AUD_PRINTF(ERROR_LEVEL,"tOffset %ld, tWorkSize %ld, tAvail %ld\n",tOffset,tWorkSize,tAvail);

        if(tWorkSize == 0)
        {
            nErr = -EINVAL;
            goto _end_unlock;
        }

        tApplptr = ptDmaData->tApplPtr%ptDmaData->tBufSize;

        spin_unlock_irq(&_gaRuntimeData[eDmaChannel].tPcmLock);
        //if(tOffset)
        //    AUD_PRINTF(ERROR_LEVEL,"tOffset %ld!!!!!\n",tOffset);
        nErr = _AudDmaRdKernelCopyAll(&_gaRuntimeData[eDmaChannel], tApplptr, pWrBuffer, nSize, tOffset, tWorkSize);
        spin_lock_irq(&_gaRuntimeData[eDmaChannel].tPcmLock);

        if(nErr<0)
            goto _end_unlock;

        switch (_gaRuntimeData[eDmaChannel].nStatus)
        {
            case SND_PCM_STATE_XRUN:
                nErr = -EPIPE;
                goto _end_unlock;

            default:
                break;
        }
        tApplptr += tWorkSize;
        if (tApplptr >= ptDmaData->tBufSize)
        {
            tApplptr -= ptDmaData->tBufSize;
        }
        ptDmaData->tApplPtr = tApplptr;
        tOffset += tWorkSize;
        tBufSize -= tWorkSize;
        tAvail -= tWorkSize;

        //_AudDmaUpdateHwLevel(&_gaRuntimeData[eDmaChannel]);
#ifdef __EN_START_TH__
        if (_gaRuntimeData[eDmaChannel].nStatus == SND_PCM_STATE_PREPARED &&
                _AudPcmPlaybackAvail(ptDmaData) >= _BytesToFrames(&_gaRuntimeData[eDmaChannel],_gaDmaParam[eDmaChannel].nStartThres))
        {
            AUD_PRINTF(TRACE_LEVEL,"%s, DrvAudStartDma %ld\n",__func__,_AudPcmPlaybackAvail(ptDmaData));
            nErr = DrvAudStartDma(eDmaChannel);
            if (nErr < 0)
                goto _end_unlock;
        }
#endif
    }

_end_unlock:

    spin_unlock_irq(&_gaRuntimeData[eDmaChannel].tPcmLock);
    return nErr < 0 ? nErr:_FramesToBytes(&_gaRuntimeData[eDmaChannel],tOffset);//nSize;
}


s32 DrvAudReadPcm(AudDmaChn_e eDmaChannel, void *pRdBuffer, u32 nSize, BOOL bBlock)
{
    PcmDmaData_t *ptDmaData = (PcmDmaData_t *)(_gaRuntimeData[eDmaChannel].pPrivateData);
    s32 nErr=0;
    PcmFrames_t tAvail;
    PcmFrames_t tFreeSize;
    PcmFrames_t tWorkSize;
    PcmFrames_t tOffset=0;
    PcmFrames_t tApplptr;
    PcmFrames_t tBufSize;
    AUD_PRINTF(DEBUG_LEVEL,"%s chn=%d\n",__FUNCTION__,eDmaChannel);
    if (nSize == 0)
        return 0;
    else
        tBufSize = _BytesToFrames(&_gaRuntimeData[eDmaChannel],nSize);

    spin_lock_irq(&_gaRuntimeData[eDmaChannel].tPcmLock);
    switch (_gaRuntimeData[eDmaChannel].nStatus)
    {
        case SND_PCM_STATE_PREPARED:
#ifdef __EN_START_TH__
            if (tBufSize >= _HwFramesToBytes(&_gaRuntimeData[eDmaChannel],_gaDmaParam[eDmaChannel].nStartThres))
            {
                if (DrvAudStartDma(eDmaChannel) == FALSE)
                    goto _end_unlock;
            }
#endif
            break;
        case SND_PCM_STATE_RUNNING:
        case SND_PCM_STATE_PAUSED:
            break;
        case SND_PCM_STATE_XRUN:
            nErr = -EPIPE;
            goto _end_unlock;
        default:
            nErr = -EBADFD;
            goto _end_unlock;
    }


#ifdef __HWPTR__
    if (_gaRuntimeData[eDmaChannel].nStatus == SND_PCM_STATE_RUNNING)
        _AudDmaUpdateHwPointer(&_gaRuntimeData[eDmaChannel]);
#endif
    //_AudDmaUpdateHwLevel(&_gaRuntimeData[eDmaChannel]);
    tAvail = _AudPcmCaptureAvail(ptDmaData);
    AUD_PRINTF(DEBUG_LEVEL,"%s tAvail %lu,tBufSize %lu\n",__FUNCTION__,tAvail,tBufSize);
    while(tBufSize>0)
    {
        if(tAvail==0)
        {
            if (!bBlock)
            {
                nErr = -EAGAIN;
                goto _end_unlock;
            }

            AUD_PRINTF(DEBUG_LEVEL,"_WaitForAvail\n");
            nErr = _WaitForAvail(&_gaRuntimeData[eDmaChannel], &tAvail);
            if (nErr < 0)
                goto _end_unlock;
            AUD_PRINTF(DEBUG_LEVEL,"_WaitForAvail avail %lu\n",tAvail);
        }

        tWorkSize = (tBufSize > tAvail? tAvail : tBufSize);
        tFreeSize = ptDmaData->tBufSize - (ptDmaData->tApplPtr % ptDmaData->tBufSize);

        if(tWorkSize > tFreeSize)
        {
            tWorkSize = tFreeSize;
        }

        if(tWorkSize == 0)
        {
            nErr = -EINVAL;
            goto _end_unlock;
        }

        tApplptr = ptDmaData->tApplPtr%ptDmaData->tBufSize;

        spin_unlock_irq(&_gaRuntimeData[eDmaChannel].tPcmLock);
        nErr = _AudDmaKernelCopy(&_gaRuntimeData[eDmaChannel], tApplptr, pRdBuffer, nSize, tOffset, tWorkSize);
        spin_lock_irq(&_gaRuntimeData[eDmaChannel].tPcmLock);

        if(nErr<0)
            goto _end_unlock;

        switch (_gaRuntimeData[eDmaChannel].nStatus)
        {
            case SND_PCM_STATE_XRUN:
                nErr = -EPIPE;
                goto _end_unlock;

            default:
                break;
        }
        tApplptr += tWorkSize;
        if (tApplptr >= ptDmaData->tBufSize)
        {
            tApplptr -= ptDmaData->tBufSize;
        }
        ptDmaData->tApplPtr = tApplptr;
        tOffset += tWorkSize;
        tBufSize -= tWorkSize;
        tAvail -= tWorkSize;
    }

_end_unlock:

    spin_unlock_irq(&_gaRuntimeData[eDmaChannel].tPcmLock);
    return nErr < 0 ? nErr:_FramesToBytes(&_gaRuntimeData[eDmaChannel],tOffset);//nSize;
}


BOOL DrvAudSetGain(AudDmaChn_e eDmaChannel, s16 nGain)
{
    AUD_PRINTF(TRACE_LEVEL,"%s gain=%d\n",__FUNCTION__,nGain);

    if(eDmaChannel<=AUD_DMA_WRITER5)
    {
        AUD_PRINTF(ERROR_LEVEL,"%s, Gain only in DMA reader\n",__FUNCTION__);
        return FALSE;
    }

    if(nGain>MAX_GAIN || nGain<MIN_GAIN)
    {
        AUD_PRINTF(ERROR_LEVEL,"%s, Gain level should be between %d and %d\n",__FUNCTION__,MIN_GAIN,MAX_GAIN);
        return FALSE;
    }



    _gaGainData[eDmaChannel].nTargetGainLevel = nGain - MIN_GAIN;

    return TRUE;
}

BOOL DrvAudSetAdcGain(AudAdcPath_e eAdc, U16 nSel)
{
    return HalAudAtopAdcGain(eAdc, nSel);
}

BOOL DrvAudSetMicAmpGain(U16 nSel)
{
    return HalAudAtopMicAmpGain(nSel);
}

BOOL DrvAudAdcSetMux(AudAdcPath_e eAdcPath)
{
    return HalAudAtopAdcInmux(eAdcPath);
}

