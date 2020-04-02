
#ifndef DRV_AUDIO_TYPES_H_
#define DRV_AUDIO_TYPES_H_


struct AUD_I2sCfg_s
{
    int nTdmMode;
    int nMsMode;
    int nBitWidth;
    int nFmt;
    int u16Channels;
};

struct AUD_PcmCfg_s
{
    int nRate;
    int nBitWidth;
    int n16Channels;
    int nInterleaved;
    int n32PeriodSize; //bytes
    int n32StartThres;
    int nTimeMs;
    int nI2sConfig;
    struct AUD_I2sCfg_s sI2s;
};

#endif
