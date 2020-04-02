#include "mhal_common.h"
#include "mhal_audio.h"
#include "mhal_audio_datatype.h"
//#include "hal_aud_api.h"
//#include "hal_aud_types.h"
#include "drv_audio.h"
#include "drv_audio_dbg.h"
#include <linux/kernel.h>
#include <linux/slab.h>

#define MHAL_AUDIO_MAX_OUT_DEV 4
#define MHAL_AUDIO_MAX_IN_DEV 3

//#define MHAL_AUDIO_I2S_PAD_INV

static MHAL_AUDIO_PcmCfg_t _astOutPcmCfg[MHAL_AUDIO_MAX_OUT_DEV];
static MHAL_AUDIO_PcmCfg_t _astInPcmCfg[MHAL_AUDIO_MAX_IN_DEV];
static MS_S16 _gnInitCount=0;

enum
{
    DMA_R1,
    DMA_R2,
    DMA_R5,
    DMA_RALL
};
// AoutDevId 0: DMAR1->lineout
//                1: DMAR2->CodexTx
//                2: DMAR5->HDMI
//                3: all device out
//
// AinDevId  0: I2S Rx->DMAW1 &DMAW2
//               1: MIC->DMAW
//

/**
* \brief Init audio HW
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_Init(void)
{
    AUD_PRINTF(MHAL_LEVEL,"in %s\n",__FUNCTION__);
    if(_gnInitCount++ == 0)
    {
        DrvAudInit();

        //init mux
        DrvAudSetMux(AUD_MUX2_DMA_W1, 0x1);
        DrvAudSetMux(AUD_MUX4_DMA_W5, 0x2);

        //init mixer
        DrvAudSetMixer(AUD_MIXER_CODEC_L, 0x2);
        DrvAudSetMixer(AUD_MIXER_CODEC_R, 0x2);
        DrvAudSetMixer(AUD_MIXER_DAC_L, 0x2);
        DrvAudSetMixer(AUD_MIXER_DAC_R, 0x2);
    }
    return MHAL_SUCCESS;
}

/**
* \brief Config audio I2S output device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_ConfigI2sOut(MHAL_AUDIO_DEV AoutDevId, MHAL_AUDIO_I2sCfg_t *pstI2sConfig)
{
    AudI2sCfg_t tI2sCfg;
    if(AoutDevId==1 || AoutDevId==3)// codex tx
    {
        tI2sCfg.nTdmChannel = 4;

        switch(pstI2sConfig->eMode)
        {
            case E_MHAL_AUDIO_MODE_I2S_MASTER:
                tI2sCfg.eMode = AUD_I2S_MODE_I2S;
                tI2sCfg.eMsMode = AUD_I2S_MSMODE_MASTER;
                if(pstI2sConfig->u16Channels!=2)
                {
                    AUD_PRINTF(ERROR_LEVEL,"in %s, channel %u  error\n",__FUNCTION__,pstI2sConfig->u16Channels);
                    return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
                }
                break;
            case E_MHAL_AUDIO_MODE_I2S_SLAVE:
                tI2sCfg.eMode = AUD_I2S_MODE_I2S;
                tI2sCfg.eMsMode = AUD_I2S_MSMODE_SLAVE;
                if(pstI2sConfig->u16Channels!=2)
                {
                    AUD_PRINTF(ERROR_LEVEL,"in %s, channel %u  error\n",__FUNCTION__,pstI2sConfig->u16Channels);AUD_PRINTF(ERROR_LEVEL,"in %s, channel error\n",__FUNCTION__);
                    return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
                }
                break;
            case E_MHAL_AUDIO_MODE_TDM_MASTER:
                tI2sCfg.eMode = AUD_I2S_MODE_TDM;
                tI2sCfg.eMsMode = AUD_I2S_MSMODE_MASTER;
                tI2sCfg.nTdmChannel = pstI2sConfig->u16Channels;
                if(pstI2sConfig->u16Channels!=4 && pstI2sConfig->u16Channels!=8)
                {
                    AUD_PRINTF(ERROR_LEVEL,"in %s, channel %u  error\n",__FUNCTION__,pstI2sConfig->u16Channels);
                    return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
                }
                break;
            default:
                return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
        }

        switch(pstI2sConfig->eWidth)
        {
            case E_MHAL_AUDIO_BITWIDTH_16:
                tI2sCfg.eWidth = AUD_BITWIDTH_16;
                break;
            case E_MHAL_AUDIO_BITWIDTH_24:
            case E_MHAL_AUDIO_BITWIDTH_32:
                tI2sCfg.eWidth = AUD_BITWIDTH_32;
                break;
            default:
                AUD_PRINTF(ERROR_LEVEL,"in %s, bitwidth %d  error\n",__FUNCTION__,pstI2sConfig->eWidth);
                return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
        }


        switch(pstI2sConfig->eFmt)
        {
            case E_MHAL_AUDIO_I2S_FMT_I2S:
                tI2sCfg.eFormat= AUD_I2S_FMT_I2S;
                break;
            case E_MHAL_AUDIO_I2S_FMT_LEFT_JUSTIFY:
                tI2sCfg.eFormat= AUD_I2S_FMT_LEFT_JUSTIFY;
                break;
            default:
                AUD_PRINTF(ERROR_LEVEL,"in %s, I2S format %d  error\n",__FUNCTION__,pstI2sConfig->eFmt);
                return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
        }

        if(!DrvAudConfigI2s(AUD_I2S_IF_CODEC_TX, &tI2sCfg))
        {
            return MHAL_FAILURE;//E_MHAL_ERR_NOT_PERM;
        }

    }
    else if(AoutDevId < MHAL_AUDIO_MAX_OUT_DEV)
    {
        return MHAL_SUCCESS;
    }
    else
    {
        return MHAL_FAILURE;//E_MHAL_ERR_UNEXIST;
    }

    return MHAL_SUCCESS;
}

/**
* \brief Config audio I2S input device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_ConfigI2sIn(MHAL_AUDIO_DEV AinDevId, MHAL_AUDIO_I2sCfg_t *pstI2sConfig)
{
    AudI2sCfg_t tI2sCfg;
    MS_BOOL bRet;
    if(AinDevId==0)// codex rx & misc rx
    {
        tI2sCfg.nTdmChannel = 4;

        switch(pstI2sConfig->eWidth)
        {
            case E_MHAL_AUDIO_BITWIDTH_16:
                tI2sCfg.eWidth = AUD_BITWIDTH_16;
                break;
            case E_MHAL_AUDIO_BITWIDTH_24:
            case E_MHAL_AUDIO_BITWIDTH_32:
                tI2sCfg.eWidth = AUD_BITWIDTH_32;
                break;
            default:
                AUD_PRINTF(ERROR_LEVEL,"in %s, bitwidth %d  error\n",__FUNCTION__,pstI2sConfig->eWidth);
                return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
        }

        switch(pstI2sConfig->eFmt)
        {
            case E_MHAL_AUDIO_I2S_FMT_I2S:
                tI2sCfg.eFormat= AUD_I2S_FMT_I2S;
                break;
            case E_MHAL_AUDIO_I2S_FMT_LEFT_JUSTIFY:
                tI2sCfg.eFormat= AUD_I2S_FMT_LEFT_JUSTIFY;
                break;
            default:
                AUD_PRINTF(ERROR_LEVEL,"in %s, I2S format %d  error\n",__FUNCTION__,pstI2sConfig->eFmt);
                return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
        }

        switch(pstI2sConfig->eMode)
        {
            case E_MHAL_AUDIO_MODE_I2S_MASTER:
                tI2sCfg.eMode = AUD_I2S_MODE_I2S;
                tI2sCfg.eMsMode = AUD_I2S_MSMODE_MASTER;

                if(pstI2sConfig->u16Channels==2)
                {
#ifndef MHAL_AUDIO_I2S_PAD_INV
                    bRet = DrvAudSetMux(AUD_MUX2_DMA_W2,0);
                    bRet &= DrvAudConfigI2s(AUD_I2S_IF_CODEC_RX, &tI2sCfg);
#else
                    bRet = DrvAudConfigI2s(AUD_I2S_IF_MISC_RX, &tI2sCfg);
#endif
                }
                else if(pstI2sConfig->u16Channels==4)
                {
                    bRet = DrvAudSetMux(AUD_MUX2_DMA_W2,0);
                    bRet &= DrvAudConfigI2s(AUD_I2S_IF_CODEC_RX, &tI2sCfg);
                    bRet &= DrvAudConfigI2s(AUD_I2S_IF_MISC_RX, &tI2sCfg);
                }
                else
                {
                    AUD_PRINTF(ERROR_LEVEL,"in %s, channel %u  error\n",__FUNCTION__,pstI2sConfig->u16Channels);
                    return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
                }
                break;
                
            case E_MHAL_AUDIO_MODE_I2S_SLAVE:
                tI2sCfg.eMode = AUD_I2S_MODE_I2S;
                tI2sCfg.eMsMode = AUD_I2S_MSMODE_SLAVE;
#ifndef MHAL_AUDIO_I2S_PAD_INV
                if(pstI2sConfig->u16Channels!=2)
                {
                    AUD_PRINTF(ERROR_LEVEL,"in %s, channel %u  error\n",__FUNCTION__,pstI2sConfig->u16Channels);
                    return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
                }
#else
                AUD_PRINTF(ERROR_LEVEL,"in %s, MISC I2S slave mode not support\n",__FUNCTION__);
                return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
#endif
                bRet = DrvAudSetMux(AUD_MUX2_DMA_W2,0);
                bRet &= DrvAudConfigI2s(AUD_I2S_IF_CODEC_RX, &tI2sCfg);
                break;
                
            case E_MHAL_AUDIO_MODE_TDM_MASTER:
                tI2sCfg.eMode = AUD_I2S_MODE_TDM;
                tI2sCfg.eMsMode = AUD_I2S_MSMODE_MASTER;
                if(pstI2sConfig->u16Channels==4 || pstI2sConfig->u16Channels==8)
                {
                    tI2sCfg.nTdmChannel = pstI2sConfig->u16Channels;
#ifndef MHAL_AUDIO_I2S_PAD_INV
                    bRet = DrvAudSetMux(AUD_MUX2_DMA_W2,1);
                    bRet &= DrvAudConfigI2s(AUD_I2S_IF_CODEC_RX, &tI2sCfg);
#else
                    bRet = DrvAudConfigI2s(AUD_I2S_IF_MISC_RX, &tI2sCfg);
#endif
                }
                else if(pstI2sConfig->u16Channels==16)
                {
                    tI2sCfg.nTdmChannel = 8;
                    bRet = DrvAudSetMux(AUD_MUX2_DMA_W2,1);
                    bRet &= DrvAudConfigI2s(AUD_I2S_IF_CODEC_RX, &tI2sCfg);
                    bRet &= DrvAudConfigI2s(AUD_I2S_IF_MISC_RX, &tI2sCfg);
                }
                else
                {
                    AUD_PRINTF(ERROR_LEVEL,"in %s, channel %u  error\n",__FUNCTION__,pstI2sConfig->u16Channels);
                    return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
                }
                break;
                
            default:
                return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
        }

        if(!bRet)
        {
            return MHAL_FAILURE;//E_MHAL_ERR_NOT_PERM;
        }

    }
    else if(AinDevId == 2)// bt rx 
    {
         tI2sCfg.nTdmChannel = 4;

        switch(pstI2sConfig->eWidth)
        {
            case E_MHAL_AUDIO_BITWIDTH_16:
                tI2sCfg.eWidth = AUD_BITWIDTH_16;
                break;
            case E_MHAL_AUDIO_BITWIDTH_24:
            case E_MHAL_AUDIO_BITWIDTH_32:
                tI2sCfg.eWidth = AUD_BITWIDTH_32;
                break;
            default:
                AUD_PRINTF(ERROR_LEVEL,"in %s, bitwidth %d  error\n",__FUNCTION__,pstI2sConfig->eWidth);
                return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
        }

        switch(pstI2sConfig->eFmt)
        {
            case E_MHAL_AUDIO_I2S_FMT_I2S:
                tI2sCfg.eFormat= AUD_I2S_FMT_I2S;
                break;
            case E_MHAL_AUDIO_I2S_FMT_LEFT_JUSTIFY:
                tI2sCfg.eFormat= AUD_I2S_FMT_LEFT_JUSTIFY;
                break;
            default:
                AUD_PRINTF(ERROR_LEVEL,"in %s, I2S format %d  error\n",__FUNCTION__,pstI2sConfig->eFmt);
                return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
        }
        
        switch(pstI2sConfig->eMode)
        {
            case E_MHAL_AUDIO_MODE_I2S_MASTER:
                tI2sCfg.eMode = AUD_I2S_MODE_I2S;
                tI2sCfg.eMsMode = AUD_I2S_MSMODE_MASTER;

                if(pstI2sConfig->u16Channels==2)
                {
                    bRet = DrvAudSetMux(AUD_MUX2_DMA_W3,0);
                    bRet &= DrvAudConfigI2s(AUD_I2S_IF_BT_TRX, &tI2sCfg);
                }
                else
                {
                    AUD_PRINTF(ERROR_LEVEL,"in %s, channel %u  error\n",__FUNCTION__,pstI2sConfig->u16Channels);
                    return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
                }
                break;

            case E_MHAL_AUDIO_MODE_I2S_SLAVE:
                tI2sCfg.eMode = AUD_I2S_MODE_I2S;
                tI2sCfg.eMsMode = AUD_I2S_MSMODE_SLAVE;

                if(pstI2sConfig->u16Channels!=2)
                {
                    AUD_PRINTF(ERROR_LEVEL,"in %s, channel %u  error\n",__FUNCTION__,pstI2sConfig->u16Channels);
                    return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
                }
                //AUD_PRINTF(ERROR_LEVEL,"in %s, MISC I2S slave mode not support\n",__FUNCTION__);
                //return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
                bRet = DrvAudSetMux(AUD_MUX2_DMA_W3,0);
                bRet &= DrvAudConfigI2s(AUD_I2S_IF_BT_TRX, &tI2sCfg);
                break;

            case E_MHAL_AUDIO_MODE_TDM_MASTER:
                tI2sCfg.eMode = AUD_I2S_MODE_TDM;
                tI2sCfg.eMsMode = AUD_I2S_MSMODE_MASTER;
                //I2S BT TRX support TDM, but DMA writer3 only 2 ch, so maybe return error
                if(pstI2sConfig->u16Channels==4 || pstI2sConfig->u16Channels==8)
                {
                    tI2sCfg.nTdmChannel = pstI2sConfig->u16Channels;
                    bRet = DrvAudSetMux(AUD_MUX2_DMA_W3,1);
                    bRet &= DrvAudConfigI2s(AUD_I2S_IF_BT_TRX, &tI2sCfg);
                }
                else
                {
                    AUD_PRINTF(ERROR_LEVEL,"in %s, channel %u  error\n",__FUNCTION__,pstI2sConfig->u16Channels);
                    return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
                }
                break;

            default:
                return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
        }        
        if(!bRet)
        {
            return MHAL_FAILURE;//E_MHAL_ERR_NOT_PERM;
        }

    }
    else if(AinDevId < MHAL_AUDIO_MAX_IN_DEV)
    {
        return MHAL_SUCCESS;
    }
    else
    {
        return MHAL_FAILURE;//E_MHAL_ERR_UNEXIST;
    }

    return MHAL_SUCCESS;
}

/**
* \brief Config audio PCM output device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_ConfigPcmOut(MHAL_AUDIO_DEV AoutDevId, MHAL_AUDIO_PcmCfg_t *pstDmaConfig)
{
    MS_BOOL bRet;
    u16 nBitWidth;
    DmaParam_t tParam;
    switch(pstDmaConfig->eWidth)
    {
        case E_MHAL_AUDIO_BITWIDTH_16:
            nBitWidth = 16;
            break;
        case E_MHAL_AUDIO_BITWIDTH_24:
            nBitWidth = 24;
            break;
        default:
            return E_MHAL_ERR_ILLEGAL_PARAM;
    }

    tParam.pDmaBuf = pstDmaConfig->pu8DmaArea;
    tParam.nPhysDmaAddr = pstDmaConfig->phyDmaAddr;//!!!MIU_OFFSET
    tParam.nBufferSize = pstDmaConfig->u32BufferSize;
    tParam.nChannels = pstDmaConfig->u16Channels;
    tParam.nBitWidth = nBitWidth;
    tParam.nSampleRate = pstDmaConfig->eRate;
    tParam.nPeriodSize = pstDmaConfig->u32PeriodSize;
    tParam.nStartThres = pstDmaConfig->u32StartThres;
    tParam.nInterleaved = pstDmaConfig->bInterleaved;
    /*if(tParam.nChannels==1)
    {
        tParam.nPeriodSize*=2;
        tParam.nStartThres*=2;
    }*/

    if(AoutDevId==0)
    {
        bRet = DrvAudConfigDmaParam(AUD_DMA_READER1, &tParam);
    }
    else if(AoutDevId==1)
    {
        bRet = DrvAudConfigDmaParam(AUD_DMA_READER2, &tParam);
    }
    else if(AoutDevId==2)
    {
        bRet = DrvAudConfigDmaParam(AUD_DMA_READER5, &tParam);
    }
    else if(AoutDevId==3)
    {
        bRet = DrvAudConfigDmaParam(AUD_DMA_READER1, &tParam);
        tParam.nPeriodSize = 0; //only one interrupt, config period_size=0 to disable interrupt
        bRet &= DrvAudConfigDmaParam(AUD_DMA_READER2, &tParam);
        bRet &= DrvAudConfigDmaParam(AUD_DMA_READER5, &tParam);
    }
    else
    {
        return MHAL_FAILURE;//E_MHAL_ERR_UNEXIST;
    }

    if(bRet)
    {
        memcpy(&_astOutPcmCfg[AoutDevId],pstDmaConfig,sizeof(MHAL_AUDIO_PcmCfg_t));
        return MHAL_SUCCESS;
    }
    else
    {
        return MHAL_FAILURE;//E_MHAL_ERR_NOT_SUPPORT;
    }

}

/**
* \brief Config audio PCM input device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_ConfigPcmIn(MHAL_AUDIO_DEV AinDevId, MHAL_AUDIO_PcmCfg_t *pstDmaConfig)
{
    u16 nBitWidth;
    MS_BOOL bRet;
    u32 nTmpBufSize;
    u32 nTmpPeriodSize;
    DmaParam_t tParam;

    switch(pstDmaConfig->eWidth)
    {
        case E_MHAL_AUDIO_BITWIDTH_16:
            nBitWidth = 16;
            break;
        case E_MHAL_AUDIO_BITWIDTH_24:
            nBitWidth = 24;
            break;
        default:
            return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
    }

    tParam.nBitWidth = nBitWidth;
    tParam.nSampleRate = pstDmaConfig->eRate;
    tParam.nStartThres = 1;
    tParam.nInterleaved = pstDmaConfig->bInterleaved;

    if(AinDevId==0)
    {
        if(pstDmaConfig->u16Channels==0 || pstDmaConfig->u16Channels%2)
        {
            return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
        }

        if(pstDmaConfig->u16Channels<=8)
        {
            tParam.pDmaBuf = pstDmaConfig->pu8DmaArea;
            tParam.nPhysDmaAddr = pstDmaConfig->phyDmaAddr;//!!!MIU_OFFSET
            tParam.nBufferSize = pstDmaConfig->u32BufferSize;
            tParam.nChannels = pstDmaConfig->u16Channels;
            //tParam.nPeriodSize = pstDmaConfig->u32PeriodSize+DMA_LOCALBUF_SIZE;
            tParam.nPeriodSize = pstDmaConfig->u32PeriodSize;
#ifndef MHAL_AUDIO_I2S_PAD_INV
            bRet = DrvAudConfigDmaParam(AUD_DMA_WRITER2, &tParam);
#else
            bRet = DrvAudConfigDmaParam(AUD_DMA_WRITER1, &tParam);
#endif
        }
        else
        {
            //if(pstDmaConfig->bInterleaved==TRUE)
               // return MHAL_FAILURE;//E_MHAL_ERR_NOT_SUPPORT;

            nTmpBufSize=(pstDmaConfig->u32BufferSize)/pstDmaConfig->u16Channels*8;
            nTmpPeriodSize = (pstDmaConfig->u32PeriodSize)/pstDmaConfig->u16Channels*8;
#ifndef MHAL_AUDIO_I2S_PAD_INV
            tParam.pDmaBuf = pstDmaConfig->pu8DmaArea;
            tParam.nPhysDmaAddr = pstDmaConfig->phyDmaAddr;//!!!MIU_OFFSET
            tParam.nBufferSize = nTmpBufSize;
            tParam.nChannels = 8;
            //tParam.PeriodSize = nTmpBufSize+DMA_LOCALBUF_SIZE;
            tParam.nPeriodSize = nTmpPeriodSize;
            bRet = DrvAudConfigDmaParam(AUD_DMA_WRITER2, &tParam);

            tParam.pDmaBuf = pstDmaConfig->pu8DmaArea + nTmpBufSize;
            tParam.nPhysDmaAddr = pstDmaConfig->phyDmaAddr + nTmpBufSize;//!!!MIU_OFFSET
            tParam.nBufferSize = pstDmaConfig->u32BufferSize- nTmpBufSize;
            tParam.nChannels = pstDmaConfig->u16Channels-8;
            tParam.nPeriodSize = 0;
            bRet &= DrvAudConfigDmaParam(AUD_DMA_WRITER1, &tParam);

#else
            tParam.pDmaBuf = pstDmaConfig->pu8DmaArea;
            tParam.nPhysDmaAddr = pstDmaConfig->phyDmaAddr;//!!!MIU_OFFSET
            tParam.nBufferSize = nTmpBufSize;
            tParam.nChannels = 8;
            //tParam.nPeriodSize = nTmpBufSize+DMA_LOCALBUF_SIZE;
            tParam.nPeriodSize = nTmpPeriodSize;
            bRet = DrvAudConfigDmaParam(AUD_DMA_WRITER1, &tParam);

            tParam.pDmaBuf = pstDmaConfig->pu8DmaArea + nTmpBufSize;
            tParam.nPhysDmaAddr = pstDmaConfig->phyDmaAddr + nTmpBufSize;//!!!MIU_OFFSET
            tParam.nBufferSize = pstDmaConfig->u32BufferSize- nTmpBufSize;
            tParam.nChannels = pstDmaConfig->u16Channels-8;
            tParam.nPeriodSize = 0;
            bRet &= DrvAudConfigDmaParam(AUD_DMA_WRITER2, &tParam);

#endif
        }
    }
    else if(AinDevId==1)
    {
        if(pstDmaConfig->u16Channels!=2)
        {
            return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
        }
        tParam.pDmaBuf = pstDmaConfig->pu8DmaArea;
        tParam.nPhysDmaAddr = pstDmaConfig->phyDmaAddr;//!!!MIU_OFFSET
        tParam.nBufferSize = pstDmaConfig->u32BufferSize;
        tParam.nChannels = pstDmaConfig->u16Channels;
        //tParam.nPeriodSize = pstDmaConfig->u32PeriodSize+DMA_LOCALBUF_SIZE;
        tParam.nPeriodSize = pstDmaConfig->u32PeriodSize;
        bRet = DrvAudConfigDmaParam(AUD_DMA_WRITER5, &tParam);
    }
    else if(AinDevId==2)
    {
        if(pstDmaConfig->u16Channels!=2)
        {
            return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
        }
        tParam.pDmaBuf = pstDmaConfig->pu8DmaArea;
        tParam.nPhysDmaAddr = pstDmaConfig->phyDmaAddr;//!!!MIU_OFFSET
        tParam.nBufferSize = pstDmaConfig->u32BufferSize;
        tParam.nChannels = pstDmaConfig->u16Channels;
        //tParam.nPeriodSize = pstDmaConfig->u32PeriodSize+DMA_LOCALBUF_SIZE;
        tParam.nPeriodSize = pstDmaConfig->u32PeriodSize;
        bRet = DrvAudConfigDmaParam(AUD_DMA_WRITER3, &tParam);
    }
    else
    {
        return MHAL_FAILURE;//E_MHAL_ERR_UNEXIST;
    }

    if(bRet)
    {
        memcpy(&_astInPcmCfg[AinDevId],pstDmaConfig,sizeof(MHAL_AUDIO_PcmCfg_t));
        return MHAL_SUCCESS;
    }
    else
    {
        return MHAL_FAILURE;//E_MHAL_ERR_NOT_SUPPORT;
    }
}

/**
* \brief Open audio PCM output device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_OpenPcmOut(MHAL_AUDIO_DEV AoutDevId)
{
    //dma
    //irq
    MS_BOOL bRet;

    if(AoutDevId==0)
    {
        bRet = DrvAudOpenDma(AUD_DMA_READER1);
        bRet &= DrvAudEnableAtop(AUD_ATOP_DAC,TRUE);
        bRet &= DrvAudPrepareDma(AUD_DMA_READER1);
    }
    else if(AoutDevId==1)
    {
        bRet = DrvAudOpenDma(AUD_DMA_READER2);
        bRet &= DrvAudEnableI2s(AUD_I2S_IF_CODEC_TX,TRUE);
        bRet &= DrvAudPrepareDma(AUD_DMA_READER2);
    }
    else if(AoutDevId==2)
    {
        bRet = DrvAudOpenDma(AUD_DMA_READER5);
        // enable HDMI
        bRet &= DrvAudPrepareDma(AUD_DMA_READER5);
    }
    else if(AoutDevId==3)
    {
        bRet = DrvAudOpenDma(AUD_DMA_READER1);
        bRet &= DrvAudEnableAtop(AUD_ATOP_DAC,TRUE);
        bRet &= DrvAudOpenDma(AUD_DMA_READER2);
        bRet &= DrvAudEnableI2s(AUD_I2S_IF_CODEC_TX,TRUE);
        bRet &= DrvAudOpenDma(AUD_DMA_READER5);
        // enable HDMI
        bRet &= DrvAudPrepareDma(AUD_DMA_READER1);
        bRet &= DrvAudPrepareDma(AUD_DMA_READER2);
        bRet &= DrvAudPrepareDma(AUD_DMA_READER5);
    }
    else
    {
        return MHAL_FAILURE;//E_MHAL_ERR_UNEXIST;
    }

    if(bRet)
    {
        return MHAL_SUCCESS;
    }
    else
    {
        return MHAL_FAILURE;//E_MHAL_ERR_NOT_SUPPORT;
    }
}

/**
* \brief Open audio PCM input device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_OpenPcmIn(MHAL_AUDIO_DEV AinDevId)
{
    MS_BOOL bRet;
    if(AinDevId==0)
    {
        if(_astInPcmCfg[AinDevId].u16Channels<=8)
        {

#ifndef MHAL_AUDIO_I2S_PAD_INV
            bRet = DrvAudOpenDma(AUD_DMA_WRITER2);
            bRet &= DrvAudEnableI2s(AUD_I2S_IF_CODEC_RX,TRUE);
            bRet &= DrvAudPrepareDma(AUD_DMA_WRITER2);
#else
            bRet = DrvAudOpenDma(AUD_DMA_WRITER1);
            bRet &= DrvAudEnableI2s(AUD_I2S_IF_MISC_RX,TRUE);
            bRet &= DrvAudPrepareDma(AUD_DMA_WRITER1);
#endif
        }
        else
        {
            bRet = DrvAudOpenDma(AUD_DMA_WRITER1);
            bRet &= DrvAudOpenDma(AUD_DMA_WRITER2);
            bRet &= DrvAudEnableI2s(AUD_I2S_IF_CODEC_RX,TRUE);
            bRet &= DrvAudEnableI2s(AUD_I2S_IF_MISC_RX,TRUE);
            bRet &= DrvAudPrepareDma(AUD_DMA_WRITER1);
            bRet &= DrvAudPrepareDma(AUD_DMA_WRITER2);
        }
    }
    else if(AinDevId==1)
    {
        bRet = DrvAudOpenDma(AUD_DMA_WRITER5);
        bRet &= DrvAudEnableAtop(AUD_ATOP_ADC,TRUE);
        bRet &= DrvAudPrepareDma(AUD_DMA_WRITER5);
    }
    else if(AinDevId == 2)
    {
        bRet = DrvAudOpenDma(AUD_DMA_WRITER3);
        bRet &= DrvAudEnableI2s(AUD_I2S_IF_BT_TRX,TRUE);
        bRet &= DrvAudPrepareDma(AUD_DMA_WRITER3);
    }
    else
    {
        return MHAL_FAILURE;//E_MHAL_ERR_UNEXIST;
    }

    if(bRet)
    {
        return MHAL_SUCCESS;
    }
    else
    {
        return MHAL_FAILURE;//E_MHAL_ERR_NOT_SUPPORT;
    }
}


/**
* \brief Close audio PCM output device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_ClosePcmOut(MHAL_AUDIO_DEV AoutDevId)
{
    MS_BOOL bRet;

    if(AoutDevId==0)
    {
        bRet = DrvAudCloseDma(AUD_DMA_READER1);
        bRet &= DrvAudEnableAtop(AUD_ATOP_DAC,FALSE);
    }
    else if(AoutDevId==1)
    {
        bRet = DrvAudCloseDma(AUD_DMA_READER2);
        bRet &= DrvAudEnableI2s(AUD_I2S_IF_CODEC_TX,FALSE);
    }
    else if(AoutDevId==2)
    {
        bRet = DrvAudCloseDma(AUD_DMA_READER5);
         // disable HDMI
    }
    else if(AoutDevId==3)
    {
        bRet = DrvAudCloseDma(AUD_DMA_READER1);
        bRet &= DrvAudEnableAtop(AUD_ATOP_DAC,FALSE);
        bRet &= DrvAudCloseDma(AUD_DMA_READER2);
        bRet &= DrvAudEnableI2s(AUD_I2S_IF_CODEC_TX,FALSE);
        bRet &= DrvAudCloseDma(AUD_DMA_READER5);
         // disable HDMI
    }
    else
    {
        return MHAL_FAILURE;//E_MHAL_ERR_UNEXIST;
    }

    if(bRet)
    {
        return MHAL_SUCCESS;
    }
    else
    {
        return MHAL_FAILURE;//E_MHAL_ERR_NOT_SUPPORT;
    }
}

/**
* \brief Close audio PCM input device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_ClosePcmIn(MHAL_AUDIO_DEV AinDevId)
{
    MS_BOOL bRet;
    if(AinDevId==0)
    {
        if(_astInPcmCfg[AinDevId].u16Channels<=8)
        {

#ifndef MHAL_AUDIO_I2S_PAD_INV
            bRet = DrvAudCloseDma(AUD_DMA_WRITER2);
            bRet &= DrvAudEnableI2s(AUD_I2S_IF_CODEC_RX,FALSE);
#else
            bRet = DrvAudCloseDma(AUD_DMA_WRITER1);
            bRet &= DrvAudEnableI2s(AUD_I2S_IF_MISC_RX,FALSE);
#endif
        }
        else
        {
            bRet = DrvAudCloseDma(AUD_DMA_WRITER1);
            bRet &= DrvAudCloseDma(AUD_DMA_WRITER2);
            bRet &= DrvAudEnableI2s(AUD_I2S_IF_CODEC_RX,FALSE);
            bRet &= DrvAudEnableI2s(AUD_I2S_IF_MISC_RX,FALSE);
        }
    }
    else if(AinDevId==1)
    {
        bRet = DrvAudCloseDma(AUD_DMA_WRITER5);
        bRet &= DrvAudEnableAtop(AUD_ATOP_ADC,FALSE);
    }
    else if(AinDevId == 2)
    {
         bRet = DrvAudCloseDma(AUD_DMA_WRITER3);
         bRet &= DrvAudEnableI2s(AUD_I2S_IF_BT_TRX,FALSE);
    }
    else
    {
        return MHAL_FAILURE;//E_MHAL_ERR_UNEXIST;
    }

    if(bRet)
    {
        return MHAL_SUCCESS;
    }
    else
    {
        return MHAL_FAILURE;//E_MHAL_ERR_NOT_SUPPORT;
    }
}

/**
* \brief Start audio PCM output device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_StartPcmOut(MHAL_AUDIO_DEV AoutDevId)
{
    MS_BOOL bRet;
    if(AoutDevId==0)//lineout
    {
        bRet = DrvAudStartDma(AUD_DMA_READER1);
    }
    else if(AoutDevId==1)
    {
        bRet = DrvAudStartDma(AUD_DMA_READER2);
    }
    else if(AoutDevId==2)
    {

        bRet = DrvAudStartDma(AUD_DMA_READER5);
    }
    else if(AoutDevId==3)
    {
        bRet = DrvAudStartDma(AUD_DMA_READER2);
        bRet &= DrvAudStartDma(AUD_DMA_READER5);
        bRet &= DrvAudStartDma(AUD_DMA_READER1); //irq, open it in the end
    }
    else
    {
        return MHAL_FAILURE;//E_MHAL_ERR_UNEXIST;
    }
    return (bRet?MHAL_SUCCESS:MHAL_FAILURE);
}

/**
* \brief Start audio PCM input device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_StartPcmIn(MHAL_AUDIO_DEV AinDevId)
{
    MS_BOOL bRet;
    if(AinDevId==0)
    {
        if(_astInPcmCfg[AinDevId].u16Channels<=8)
        {
#ifndef MHAL_AUDIO_I2S_PAD_INV

            bRet = DrvAudStartDma(AUD_DMA_WRITER2);
#else
            bRet = DrvAudStartDma(AUD_DMA_WRITER1);
#endif
        }
        else
        {
#ifndef MHAL_AUDIO_I2S_PAD_INV
            bRet = DrvAudStartDma(AUD_DMA_WRITER1);
            bRet &= DrvAudStartDma(AUD_DMA_WRITER2);
#else
            bRet = DrvAudStartDma(AUD_DMA_WRITER2);
            bRet &= DrvAudStartDma(AUD_DMA_WRITER1);
#endif
        }
    }
    else if(AinDevId==1)
    {
        bRet = DrvAudStartDma(AUD_DMA_WRITER5);
    }
    else if(AinDevId == 2)
    {
        bRet = DrvAudStartDma(AUD_DMA_WRITER3);
    }
    else
    {
        return MHAL_FAILURE;//E_MHAL_ERR_UNEXIST;
    }
    return (bRet?MHAL_SUCCESS:MHAL_FAILURE);
}

/**
* \brief Stop audio PCM output device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_StopPcmOut(MHAL_AUDIO_DEV AoutDevId)
{
    MS_BOOL bRet;
    if(AoutDevId==0)//lineout
    {
        bRet = DrvAudStopDma(AUD_DMA_READER1);

    }
    else if(AoutDevId==1)
    {
        bRet = DrvAudStopDma(AUD_DMA_READER2);

    }
    else if(AoutDevId==2)
    {
        bRet = DrvAudStopDma(AUD_DMA_READER5);
    }
    else if(AoutDevId==3)
    {
        bRet = DrvAudStopDma(AUD_DMA_READER1);
        bRet &= DrvAudStopDma(AUD_DMA_READER2);
        bRet &= DrvAudStopDma(AUD_DMA_READER5);
    }
    else
    {
        return MHAL_FAILURE;//E_MHAL_ERR_UNEXIST;
    }
    return (bRet?MHAL_SUCCESS:MHAL_FAILURE);
}

/**
* \brief Stop audio PCM input device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_StopPcmIn(MHAL_AUDIO_DEV AinDevId)
{
    MS_BOOL bRet;
    if(AinDevId==0)
    {
        if(_astInPcmCfg[AinDevId].u16Channels<=8)
        {
#ifndef MHAL_AUDIO_I2S_PAD_INV
            bRet = DrvAudStopDma(AUD_DMA_WRITER2);
#else
            bRet = DrvAudStopDma(AUD_DMA_WRITER1);

#endif
        }
        else
        {
#ifndef MHAL_AUDIO_I2S_PAD_INV
            bRet = DrvAudStopDma(AUD_DMA_WRITER2);
            bRet &= DrvAudStopDma(AUD_DMA_WRITER1);
#else
            bRet = DrvAudStopDma(AUD_DMA_WRITER1);
            bRet &= DrvAudStopDma(AUD_DMA_WRITER2);
#endif
        }
    }
    else if(AinDevId==1)
    {
        bRet = DrvAudStopDma(AUD_DMA_WRITER5);

    }
    else if(AinDevId == 2)
    {
        bRet = DrvAudStopDma(AUD_DMA_WRITER3);
    }
    else
    {
        return MHAL_FAILURE;//E_MHAL_ERR_UNEXIST;
    }

    return (bRet?MHAL_SUCCESS:MHAL_FAILURE);

}

/**
* \brief Pause audio PCM output device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_PausePcmOut(MHAL_AUDIO_DEV AoutDevId)
{
    MS_BOOL bRet;
    if(AoutDevId==0)
    {
        bRet = DrvAudPauseDma(AUD_DMA_READER1);
    }
    else if(AoutDevId==1)
    {
        bRet = DrvAudPauseDma(AUD_DMA_READER2);
    }
    else if(AoutDevId==2)
    {
        bRet = DrvAudPauseDma(AUD_DMA_READER5);
    }
    else if(AoutDevId==3)
    {
        bRet = DrvAudPauseDma(AUD_DMA_READER1);
        bRet &= DrvAudPauseDma(AUD_DMA_READER2);
        bRet &= DrvAudPauseDma(AUD_DMA_READER5);
    }
    else
    {
        return MHAL_FAILURE;//E_MHAL_ERR_UNEXIST;
    }

    return (bRet?MHAL_SUCCESS:MHAL_FAILURE);
}


/**
* \brief Resume audio PCM output device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_ResumePcmOut(MHAL_AUDIO_DEV AoutDevId)
{
    MS_BOOL bRet;
    if(AoutDevId==0)
    {
        bRet = DrvAudResumeDma(AUD_DMA_READER1);
    }
    else if(AoutDevId==1)
    {
        bRet = DrvAudResumeDma(AUD_DMA_READER2);
    }
    else if(AoutDevId==2)
    {
        bRet = DrvAudResumeDma(AUD_DMA_READER5);
    }
    else if(AoutDevId==3)
    {
        bRet = DrvAudResumeDma(AUD_DMA_READER2);
        bRet &= DrvAudResumeDma(AUD_DMA_READER5);
        bRet &= DrvAudResumeDma(AUD_DMA_READER1);
    }
    else
    {
        return MHAL_FAILURE;//E_MHAL_ERR_UNEXIST;
    }

    return (bRet?MHAL_SUCCESS:MHAL_FAILURE);
}


/**
* \brief Read audio raw data from PCM input device(period unit)
* \return value>0 => read data bytes, value<0 => error number
*/
MS_S32 MHAL_AUDIO_ReadDataIn(MHAL_AUDIO_DEV AinDevId, VOID *pRdBuffer, MS_U32 u32Size, MS_BOOL bBlock)
{
    MS_S32 s32Err;
    char *pBuffer1,*pBuffer2;
    MS_U32 u32BufSize;

    MS_U8* pu8TmpBuf;    
    MS_U32 u32SmpIdx;
    MS_U32 u32Idx;
    MS_U16 u16Channels;
    MS_U16 u16HalfChanl;

    if(AinDevId==0)
    {
        if(_astInPcmCfg[AinDevId].u16Channels<=8)
        {
#ifndef MHAL_AUDIO_I2S_PAD_INV
            s32Err = DrvAudReadPcm(AUD_DMA_WRITER2, pRdBuffer, u32Size, bBlock);
#else
            s32Err = DrvAudReadPcm(AUD_DMA_WRITER1, pRdBuffer, u32Size, bBlock);
#endif
        }
        else
        {
            MS_S32 s32Err1,s32Err2;
            u32BufSize = u32Size/2;
            pBuffer1 = (char*)pRdBuffer;
            pBuffer2 = (char*)(pBuffer1 + (s32)u32BufSize);
#ifndef MHAL_AUDIO_I2S_PAD_INV
            s32Err1 = DrvAudReadPcm(AUD_DMA_WRITER2, pBuffer1, u32BufSize, bBlock);
            s32Err2 = DrvAudReadPcm(AUD_DMA_WRITER1, pBuffer2, u32BufSize, FALSE);

#else
            s32Err1 = DrvAudReadPcm(AUD_DMA_WRITER1, pBuffer1, u32BufSize, bBlock);
            s32Err2 = DrvAudReadPcm(AUD_DMA_WRITER2, pBuffer2, u32BufSize, FALSE);
#endif

	     // maybe need to optimize
            if(_astInPcmCfg[AinDevId].bInterleaved == TRUE)
            {
                u16Channels = _astInPcmCfg[AinDevId].u16Channels ; //u16Channels = 16;
                u16HalfChanl = _astInPcmCfg[AinDevId].u16Channels /2 ; //u16HalfChanl = 8; 
                pu8TmpBuf = kmalloc(u32Size, GFP_KERNEL);
                if(NULL != pu8TmpBuf)
                {
                    memcpy(pu8TmpBuf,  pBuffer1 ,u32BufSize);
                    memcpy(pu8TmpBuf+u32BufSize,  pBuffer2 ,u32BufSize);
                  
                    for(u32Idx=0 ; u32Idx<(u32BufSize/(2*u16HalfChanl)) ; u32Idx++)
                    {
                        memcpy(pRdBuffer + u32SmpIdx*u16HalfChanl*2, pu8TmpBuf +u32Idx*u16HalfChanl*2, u16HalfChanl*2);
                        u32SmpIdx++; 
                        memcpy(pRdBuffer + u32SmpIdx*(u16HalfChanl)*2, (pu8TmpBuf +u32BufSize) +u32Idx*u16HalfChanl*2, u16HalfChanl*2);
                        u32SmpIdx++;                     
                    }
                    kfree(pu8TmpBuf);
                }
            }
            
            if(s32Err1>0 && s32Err2>0)
            {
                s32Err = s32Err1 + s32Err2;
            }
            else if(s32Err1<0)
            {
                s32Err = s32Err1;
                AUD_PRINTF(ERROR_LEVEL,"in %s, Err1  error = %d\n",__FUNCTION__,s32Err);
            }
            else
            {
                s32Err = s32Err2;
                AUD_PRINTF(ERROR_LEVEL,"in %s, Err2  error = %d\n",__FUNCTION__,s32Err);
            }
        }
    }
    else if(AinDevId==1)
    {
        s32Err = DrvAudReadPcm(AUD_DMA_WRITER5, pRdBuffer, u32Size, bBlock);

    }
    else if(AinDevId == 2)
    {
        s32Err = DrvAudReadPcm(AUD_DMA_WRITER3, pRdBuffer, u32Size, bBlock);
    }
    else
    {
        return MHAL_FAILURE;//E_MHAL_ERR_UNEXIST;
    }

    if(s32Err>=0)
    {
        return s32Err;
    }
    else
    {
        return MHAL_FAILURE;
    }



}

/**
* \brief Write audio raw data to PCM output device(period unit)
* \return value>0 => write data bytes, value<0 => error number
*/
MS_S32 MHAL_AUDIO_WriteDataOut(MHAL_AUDIO_DEV AoutDevId, VOID *pWrBuffer, MS_U32 u32Size, MS_BOOL bBlock)
{
    MS_S32 s32Err;
    if(AoutDevId==0)//lineout
    {
        s32Err = DrvAudWritePcm(AUD_DMA_READER1, pWrBuffer, u32Size, bBlock);

    }
    else if(AoutDevId==1)
    {
        s32Err = DrvAudWritePcm(AUD_DMA_READER2, pWrBuffer, u32Size, bBlock);

    }
    else if(AoutDevId==2)
    {
        s32Err = DrvAudWritePcm(AUD_DMA_READER5, pWrBuffer, u32Size, bBlock);
    }
    else if(AoutDevId==3)
    {
        s32Err = DrvAudWritePcmAll(AUD_DMA_READER1, pWrBuffer, u32Size, bBlock);
       // s32Err = DrvAudWritePcm(AUD_DMA_READER2, pWrBuffer, u32Size, bBlock);
       // s32Err &= DrvAudWritePcm(AUD_DMA_READER5, pWrBuffer, u32Size, bBlock);
      //  s32Err &= DrvAudWritePcm(AUD_DMA_READER1, pWrBuffer, u32Size, bBlock);
    }
    else
    {
        return MHAL_FAILURE;//E_MHAL_ERR_UNEXIST;
    }

    if(s32Err>=0)
    {
        return s32Err;
    }
    else
    {
        return MHAL_FAILURE;
    }
}

MS_BOOL MHAL_AUDIO_IsPcmOutXrun(MHAL_AUDIO_DEV AoutDevId)
{
    MS_BOOL bRet=FALSE;

    if(AoutDevId==0)//lineout
    {
        bRet = (DrvAudIsXrun(AUD_DMA_READER1)?TRUE:FALSE);
        if(bRet)
            DrvAudPrepareDma(AUD_DMA_READER1);

    }
    else if(AoutDevId==1)
    {
        bRet = (DrvAudIsXrun(AUD_DMA_READER2)?TRUE:FALSE);
        if(bRet)
            DrvAudPrepareDma(AUD_DMA_READER2);
    }
    else if(AoutDevId==2)
    {
        bRet = (DrvAudIsXrun(AUD_DMA_READER5)?TRUE:FALSE);
        if(bRet)
            DrvAudPrepareDma(AUD_DMA_READER5);
    }
    else if(AoutDevId==3)//lineout
    {
        bRet = (DrvAudIsXrun(AUD_DMA_READER1)?TRUE:FALSE);
        if(bRet)
        {
            DrvAudPrepareDma(AUD_DMA_READER1);
            DrvAudStopDma(AUD_DMA_READER2);
            DrvAudPrepareDma(AUD_DMA_READER2);
            DrvAudStopDma(AUD_DMA_READER5);
            DrvAudPrepareDma(AUD_DMA_READER5);
        }
    }
    else
    {
        AUD_PRINTF(ERROR_LEVEL,"%s unsupported device\n",__FUNCTION__);
    }
    return bRet;
}

MS_BOOL MHAL_AUDIO_IsPcmInXrun(MHAL_AUDIO_DEV AinDevId)
{
    MS_BOOL bRet=FALSE;
    if(AinDevId==0)
    {

#ifndef MHAL_AUDIO_I2S_PAD_INV
        bRet = (DrvAudIsXrun(AUD_DMA_WRITER2)?TRUE:FALSE);
        if(bRet)
        {
            DrvAudPrepareDma(AUD_DMA_WRITER2);
            if(_astInPcmCfg[AinDevId].u16Channels>8)
            {
                DrvAudStopDma(AUD_DMA_WRITER1);
                DrvAudPrepareDma(AUD_DMA_WRITER1);
            }
        }
#else
        bRet = (DrvAudIsXrun(AUD_DMA_WRITER1)?TRUE:FALSE);
        if(bRet)
        {
            DrvAudPrepareDma(AUD_DMA_WRITER1);
            if(_astInPcmCfg[AinDevId].u16Channels>8)
            {
                DrvAudStopDma(AUD_DMA_WRITER2);
                DrvAudPrepareDma(AUD_DMA_WRITER2);
            }
        }
#endif
        }
    else if(AinDevId==1)
    {
        bRet = (DrvAudIsXrun(AUD_DMA_WRITER5)?TRUE:FALSE);
        if(bRet)
            DrvAudPrepareDma(AUD_DMA_WRITER5);
    }
    else if(AinDevId == 2)
    {
        bRet = (DrvAudIsXrun(AUD_DMA_WRITER3)?TRUE:FALSE);
        if(bRet)
            DrvAudPrepareDma(AUD_DMA_WRITER3);
    }
    else
    {
        AUD_PRINTF(ERROR_LEVEL,"%s unsupported device\n",__FUNCTION__);
    }

    return bRet;
}


/**
* \brief Set output path gain
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_SetGainOut(MHAL_AUDIO_DEV AoutDevId, MS_S16 s16Gain)
{
    if(AoutDevId==0)//lineout
    {
        if(!DrvAudSetGain(AUD_DMA_READER1,s16Gain))
            return MHAL_FAILURE;

    }
    else if(AoutDevId==1)
    {
        if(!DrvAudSetGain(AUD_DMA_READER2,s16Gain))
            return MHAL_FAILURE;

    }
    else if(AoutDevId==2)
    {
        if(!DrvAudSetGain(AUD_DMA_READER5,s16Gain))
            return MHAL_FAILURE;
    }
    else if(AoutDevId==3)
    {
        if(!DrvAudSetGain(AUD_DMA_READER1,s16Gain)
            || !DrvAudSetGain(AUD_DMA_READER2,s16Gain)
            || !DrvAudSetGain(AUD_DMA_READER5,s16Gain))
            return MHAL_FAILURE;

    }
    else
    {
        return MHAL_FAILURE;//E_MHAL_ERR_UNEXIST;
    }

    return MHAL_SUCCESS;
}
EXPORT_SYMBOL(MHAL_AUDIO_Init);
EXPORT_SYMBOL(MHAL_AUDIO_ConfigI2sOut);
EXPORT_SYMBOL(MHAL_AUDIO_ConfigI2sIn);
EXPORT_SYMBOL(MHAL_AUDIO_ConfigPcmOut);
EXPORT_SYMBOL(MHAL_AUDIO_ConfigPcmIn);
EXPORT_SYMBOL(MHAL_AUDIO_OpenPcmOut);
EXPORT_SYMBOL(MHAL_AUDIO_OpenPcmIn);
EXPORT_SYMBOL(MHAL_AUDIO_ClosePcmOut);
EXPORT_SYMBOL(MHAL_AUDIO_ClosePcmIn);
EXPORT_SYMBOL(MHAL_AUDIO_StartPcmOut);
EXPORT_SYMBOL(MHAL_AUDIO_StartPcmIn);
EXPORT_SYMBOL(MHAL_AUDIO_StopPcmOut);
EXPORT_SYMBOL(MHAL_AUDIO_StopPcmIn);
EXPORT_SYMBOL(MHAL_AUDIO_PausePcmOut);
EXPORT_SYMBOL(MHAL_AUDIO_ResumePcmOut);
EXPORT_SYMBOL(MHAL_AUDIO_ReadDataIn);
EXPORT_SYMBOL(MHAL_AUDIO_WriteDataOut);
EXPORT_SYMBOL(MHAL_AUDIO_SetGainOut);
EXPORT_SYMBOL(MHAL_AUDIO_IsPcmOutXrun);
EXPORT_SYMBOL(MHAL_AUDIO_IsPcmInXrun);

