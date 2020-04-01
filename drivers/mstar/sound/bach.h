// $Change: 548282 $
//
// bach.h
//
// defines for the registers in the C3 BACH chip
//
//
#ifndef _BACH_H
#define _BACH_H

#include "bach_reg.h"
#if defined(__linux__)
#include <linux/types.h>
#include <linux/kernel.h>
typedef unsigned int    U32;
typedef int             S32;
typedef bool            BOOL;

#define TRUE 1
#define FALSE 0

//#define ERRMSG printk
#define ERRMSG(fmt, arg...)		printk(KERN_ERR "<Audio Error>" fmt, ##arg);
#define TRACE(fmt, arg...)		printk(KERN_DEBUG "<Audio Trace>" fmt, ##arg);
#else
#include <wtypes.h>
#include <ceddk.h>

typedef unsigned long    U32;
typedef long             S32;

#endif


typedef unsigned char   U8;
typedef signed char     S8;
typedef unsigned short  U16;
typedef short           S16;


#define TYPE_CAST(OldType, NewType, Var)    ((NewType)(Var))

//#define BACH_RIU_BASE_ADDR        0x25000000
#define BACH_RIU_BASE_ADDR        0x1f000000
//#define BACH_REG_BANK_1           0x1a00
#define BACH_REG_BANK_1           0x112a00
//#define BACH_REG_BANK_2           0x1b00
#define BACH_REG_BANK_2           0x112b00
//#define BACH_REG_BANK_3           0x1c00
#define BACH_REG_BANK_3           0x112c00
#define BACH_REG_BANK_4           0x112d00

#define MIU_WORD_BYTE_SIZE          (8)
#define BACH_ARM_CACHE_LINE_ALIGN   (32)

/* DMA Channel State defines */
#define DMA_UNINIT          0
#define DMA_INIT            1
#define DMA_STOPPED         2
#define DMA_RUNNING         3

#define BACH_DPGA_GAIN_MAX_DB 12
#define BACH_DPGA_GAIN_MIN_DB -64 //actually -63.5 dB
#define BACH_DPGA_GAIN_MIN_IDX 0x7F

#define BACH_PGA_GAIN_MAX_DB 6
#define BACH_PGA_GAIN_MIN_DB -45

#define WRITE_BYTE(_reg, _val)      (*((volatile U8*)(_reg)))  = (U8)(_val)
#define WRITE_WORD(_reg, _val)      (*((volatile U16*)(_reg))) = (U16)(_val)
#define WRITE_LONG(_reg, _val)      (*((volatile U32*)(_reg))) = (U32)(_val)
#define READ_BYTE(_reg)             (*(volatile U8*)(_reg))
#define READ_WORD(_reg)             (*(volatile U16*)(_reg))
#define READ_LONG(_reg)             (*(volatile U32*)(_reg))

typedef struct DMAChannelTag
{
    U32 nDMAChannelState;

    // system things
    U32 nPhysDMAAddr;         // physical RAM address of the buffer
    U32 nBufferSize;          // lenght of contiguous physical RAM

    // internal things
    U32 nChannels;            // number of channels (1 or 2)
    U32 nSampleSize;          // sample word size
    U32 nSampleRate;          // sample rate in samples/second
    U32 nBytesPerInt;       // number of samples to play before interrupting

} DMACHANNEL;

/*
typedef struct InterfaceConfigTag
{
    U32 nDMAChannelState;

    // system things
    U32 nPhysDMAAddr;         // physical RAM address of the buffer
    U32 nBufferSize;          // lenght of contiguous physical RAM

    // internal things
    U32 nChannels;            // number of channels (1 or 2)
    U32 nSampleSize;          // sample word size
    U32 nSampleRate;          // sample rate in samples/second
    U32 nBytesPerInt;       // number of samples to play before interrupting

} IFConfig_t;
*/

typedef enum
{
    BACH_CHAN_ASRC1,      //for mm
    BACH_CHAN_ASRC2,      //for gps
    BACH_CHAN_MIX,      //for mix
    BACH_CHAN_MICIN,
    BACH_CHAN_LINEIN,
    BACH_CHAN_NUM,
} BachChannel_e;

typedef enum
{
    BACH_FUNC_DMA,     //DMA 1
    BACH_FUNC_GPS,    //DMA 2
    BACH_FUNC_BT_I2S,
    BACH_FUNC_BT_A2DP,
    BACH_FUNC_DBB,
    BACH_FUNC_DBB_AEC,
    BACH_FUNC_FM,     //Line-in
    BACH_FUNC_RECORDER,
    BACH_FUNC_FM_RECORDER,
    BACH_FUNC_DMA_RECORDER,
    BACH_FUNC_EXTBT_PCMMTK_M,
    BACH_FUNC_EXTBT_I2S_M,
    BACH_FUNC_DMA_SUBCH,
    BACH_FUNC_NUM,

} BachFunction_e;

typedef enum
{
    BACH_BT_MSTAR,
    BACH_BT_EXT,
    BACH_BT_MXK,
    BACH_BT_NUM,

} BachBtChip_e;


typedef enum
{
    BACH_SRC_DMA1_READER,
    BACH_SRC_DMA2_READER,
    BACH_SRC_EXT_I2S,
    BACH_SRC_BT_I2S,
    BACH_SRC_DBB_I2S,
    BACH_SRC_DBB_AEC_I2S,
    BACH_SRC_SPDIF,
    BACH_SRC_AUX,
    BACH_SRC_NUM,
    BACH_SRC_NULL = 0xff,

} BachSource_e;

typedef enum
{
    BACH_SINK_DMA1_WRITER,
    BACH_SINK_DMA2_WRITER,
    BACH_SINK_CODEC_I2S,
    BACH_SINK_SPDIF_HDMI,
    BACH_SINK_NUM,
    BACH_SINK_NULL = 0xff,

} BachSink_e;

typedef enum
{
    BACH_SINKTX_CODEC_I2S,
    BACH_SINKTX_SPDIF,
    BACH_SINKTX_HDMI,
    BACH_SINKTX_NUM,
    BACH_SINKTX_NULL = 0xff,

} BachSinkTx_e;


typedef enum
{
    BACH_OUTPUT_PATH_DMA1_WRITER ,
    BACH_OUTPUT_PATH_DMA2_WRITER ,
    BACH_OUTPUT_PATH_CODEC_I2S   ,
    BACH_OUTPUT_PATH_DBB_I2S     ,
    BACH_OUTPUT_PATH_DBB_AEC_I2S ,
    BACH_OUTPUT_PATH_BT_I2S      ,
    BACH_OUTPUT_PATH_SPDIF_HDMI  ,
    BACH_OUTPUT_PATH_NUM
} BachOutputPath_e;

typedef enum
{
    BACH_INPUT_PATH_ASRC1 ,
    BACH_INPUT_PATH_ASRC2
} BachInputPath_e;

typedef enum
{
    BACH_REC_PATH_DMA1 ,
    BACH_REC_PATH_DMA2
} BachRecPath_e;


/**
 * \brief Audio DMA
 */
typedef enum
{
    BACH_DMA_WRITER1 = 0,
    BACH_DMA_WRITER2,
    BACH_DMA_READER1,
    BACH_DMA_READER2,
    BACH_DMA_NUM,
    BACH_DMA_NULL = 0xff,

} BachDmaChannel_e;

typedef enum
{
    BACH_DMA_INT_UNDERRUN = 0,
    BACH_DMA_INT_OVERRUN,
    BACH_DMA_INT_EMPTY,
    BACH_DMA_INT_FULL,
    BACH_DMA_INT_NUM

} BachDmaInterrupt_e;

typedef enum
{
    BACH_REG_BANK1,
    BACH_REG_BANK2,
    BACH_REG_BANK3,
    BACH_REG_BANK4
} BachRegBank_e;

typedef enum
{
    BACH_ASRC_INT1,
    BACH_ASRC_INT2,
    BACH_ASRC_DEC1_2,
    BACH_ASRC_DEC3_4,
    BACH_ASRC_ASRC2_INT1_2,
    BACH_ASRC_ASRC2_DEC1_2,
    BACH_ASRC_ASRC2_DEC3_4,
    BACH_ASRC_SPIDF_HDMI,
    BACH_ASRC_NUM,
    BACH_ASRC_NULL = 0xff,

} BachAsrc_e;


typedef enum
{
    BACH_ASRC_INT_ASRC1_1,
    BACH_ASRC_INT_ASRC1_2,
    BACH_ASRC_INT_ASRC2_1_2,
    BACH_ASRC_INT_NUM,
    BACH_ASRC_INT_NULL = 0xff,
} BachAsrcInt_e;

typedef enum
{
    BACH_ASRC_DEC_ASRC1_DEC1_2,
    BACH_ASRC_DEC_ASRC1_DEC3_4,
    BACH_ASRC_DEC_ASRC2_DEC1_2,
    BACH_ASRC_DEC_ASRC2_DEC3_4,
    BACH_ASRC_DEC_NUM,
    BACH_ASRC_DEC_NULL = 0xff,
} BachAsrcDec_e;


typedef enum
{
    BACH_FS_SYNTH,
    BACH_NF_SYNTH_1,
    BACH_NF_SYNTH_2,
    BACH_NF_SYNTH_3,
    BACH_NF_SYNTH_4,
    BACH_NF_SYNTH_5,
    BACH_NF_SYNTH_6,
    BACH_NF_SYNTH_7,
    BACH_SYNTH_NUM,
    BACH_SYNTH_NULL = 0xff,

} BachSynth_e;

typedef enum
{
    BACH_RATE_SLAVE,
    BACH_RATE_8K,
    BACH_RATE_16K,
    BACH_RATE_32K,
    BACH_RATE_44K,
    BACH_RATE_48K,
    BACH_RATE_96K,
    BACH_RATE_NUM,
    BACH_RATE_NULL = 0xff,
} BachRate_e;

typedef enum
{
    BACH_ATOP_LINEIN0,
    BACH_ATOP_LINEIN1,
    BACH_ATOP_HANDSET,
    BACH_ATOP_AUXMIC,
    BACH_ATOP_LINEOUT0,
    BACH_ATOP_LINEOUT1,
    BACH_ATOP_EAR,
    BACH_ATOP_NUM
} BachAtopPath_e;


typedef enum
{
    BACH_I2S_EXT_RX,
    BACH_I2S_BT_TRX,
    BACH_I2S_CODEC_TX,
    BACH_I2S_DBB_TRX,
    BACH_I2S_NUM
} BachI2s_e;

typedef enum
{
    BACH_PCM_MODE_I2S,
    BACH_PCM_MODE_PCM,
} BachI2sPcmMode_e;

typedef enum
{
    BACH_I2S_WIDTH_16,
    BACH_I2S_WIDTH_24,
    BACH_I2S_WIDTH_32

} BachI2sWidth_e;

typedef enum
{
    BACH_I2S_FMT_I2S,
    BACH_I2S_FMT_LEFT_JUSTIFY,
} BachI2sFmt_e;


typedef enum
{
    BACH_SPIDF_RX,
    BACH_SPIDF_TX,
} BachSpidf_e;

typedef struct
{
    //BachI2s_e eI2s;
    BachI2sPcmMode_e eMode;
    BachI2sFmt_e eFormat;
    BachI2sWidth_e eWidth;
    BachRate_e eRate;
} BachI2sCfg_t;

typedef struct
{
    //U32 nDMAChannelState;
    //BachI2sFmt_e eFormat;
    BachRate_e eRate;
} BachDmaCfg_t;

typedef struct
{
    BachRate_e eRate;
} BachSpidfHdmiCfg_t;

typedef struct
{
    BachRate_e eRate;
} BachAdcCfg_t;

typedef struct
{
    BachRate_e eRate;
    BOOL    bIsStereo;
} BachAuxCfg_t;


typedef struct
{
    BachDmaCfg_t        	tDma1ReaderCfg;
    BachDmaCfg_t        	tDma2ReaderCfg;
    BachI2sCfg_t        	tBtI2sCfg;
    BachI2sCfg_t        	tDbbI2sCfg;
    BachI2sCfg_t        	tExtI2sCfg;
    BachSpidfHdmiCfg_t    tSpidfCfg;
    BachAdcCfg_t        	tAdcCfg;
    BachAuxCfg_t        	tAuxCfg;
} BachSrcCfg_t;

typedef struct
{
    BachDmaCfg_t        	tDma1WriterCfg;
    BachDmaCfg_t        	tDma2WriterCfg;
    BachI2sCfg_t            tCodecI2sCfg;
    BachSpidfHdmiCfg_t      tSpidfHdmiCfg;
} BachSinkCfg_t;




typedef enum
{
    BACH_DPGA_MMP1,
    BACH_DPGA_MMP2,
    BACH_DPGA_CODEC1,
    BACH_DPGA_CODEC2,
    BACH_DPGA_DBB_1,
    BACH_DPGA_DEC1_1,
    BACH_DPGA_DMARD_L,
    BACH_DPGA_DEC1_2,
    BACH_DPGA_DMARD_R,
    BACH_DPGA_ASRC2MMP1,
    BACH_DPGA_ASRC2MMP2,
    BACH_DPGA_SPDIF1,
    BACH_DPGA_SPDIF2,
    BACH_DPGA_DMAWR3,
    BACH_DPGA_DMAWR4,
    BACH_DPGA_DEC1_3,
    BACH_DPGA_DBB_2,
    BACH_DPGA_DMAWR1,
    BACH_DPGA_DMAWR2,
    BACH_DPGA_NUM,
    BACH_DPGA_NULL = 0xff,

} BachDpga_e;

typedef enum
{
    BACH_PGA1,
    BACH_PGA2,
    BACH_PGA3,
    BACH_PGA4
} BachPga_e;

typedef enum
{
    BACH_MUX4_MMP1_2_CH1,
    BACH_MUX4_MMP1_2_CH2,
    BACH_MUX4_DEC1_2,
    BACH_MUX4_DEC3_4,
    BACH_MUX4_ASRC2MMP1_2,
    BACH_MUX4_ASRC2DEC1_2,
    BACH_MUX4_ASRC2DEC3_4,
    BACH_MUX4_DMA1_WRITER,
    BACH_MUX4_DMA2_WRITER,
    BACH_MUX4_NULL = 0xff
} BachMux4_e;

typedef enum
{
    BACH_MUX2_MUX,
    BACH_MUX2_CODEC_I2S_TX,
    BACH_MUX2_CODEC_TX_CH1,
    BACH_MUX2_CODEC_TX_CH2,
    BACH_MUX2_DMARD_L,
    BACH_MUX2_DMARD_R,
    BACH_MUX2_SPDIF_TX_CH1,
    BACH_MUX2_SPDIF_TX_CH2,
    BACH_MUX2_HDMI_TX_CH1,
    BACH_MUX2_HDMI_TX_CH2,
    BACH_MUX2_SPDIF_HDMI_TX,
    BACH_MUX2_SDM_DWA1_CH1,
    BACH_MUX2_SDM_DWA1_CH2,
    BACH_MUX2_SDM_DWA2_CH1,
    BACH_MUX2_SDM_DWA2_CH2,
    BACH_MUX2_DMAWR1,
    BACH_MUX2_DMAWR2,
    BACH_MUX2_NULL = 0xff
} BachMux2_e;

typedef enum
{
    BACH_MIXER_DEC1_2,
    BACH_MIXER_DEC3_4,
    BACH_MIXER_ASRC2_DEC1_2,
    BACH_MIXER_ASRC2_DEC3_4,
    BACH_MIXER_SDM_DWA1,
    BACH_MIXER_SDM_DWA2,
    BACH_MIXER_AMR_MIX,
    BACH_MIXER_DBB_UL_MIX,
    BACH_MIXER_ST_MIX,
    BACH_MIXER_AV_MIX,
    BACH_MIXER_NULL = 0xff
} BachMixer_e;

typedef enum
{
    BACH_ADC_MICIN_8K,
    BACH_ADC_MICIN_16K,
    BACH_ADC_MICIN_32K
} BachADCMicIn_e;

typedef enum
{
    BACH_ADC_LINEIN_8K,
    BACH_ADC_LINEIN_16K,
    BACH_ADC_LINEIN_32K
} BachADCLineIn_e;


typedef enum
{
    BACH_DBB_UPLINK_DEC1,
    BACH_DBB_UPLINK_DMA1_RD_RIGHT
} BachDbbUplink_e;


/*typedef enum
{
	BACH_DAC_TYPE_LEFT,//ch1
	BACH_DAC_TYPE_RIGHT,//ch2
	BACH_DAC_TYPE_BOTH
}BachDacType_e;*/


typedef enum
{
    BACH_DAC1,//ch1
    BACH_DAC2,
} BachDac_e;

typedef struct
{
    BachRate_e  eNowRate;
    BachSynth_e eUsedSynth;
    BachSynth_e ePreferSynth;
} BachAsrcCfg_e;

typedef enum
{
    BACH_DAC_MIX,
    BACH_DAC_ASRC1,
    BACH_DAC_ASRC2
} BachDacSrc_e;


typedef enum
{
    BACH_I2S_PAD_CODEC,
    BACH_I2S_PAD_EXT,
    BACH_I2S_PAD_DBB,
    BACH_I2S_PAD_BT,
    BACH_I2S_PAD_NUM
} BachI2sPad_e;

typedef enum
{
    BACH_I2S_OUTPAD_CODEC,
    BACH_I2S_OUTPAD_EXT,
    BACH_I2S_OUTPAD_BT,
    BACH_I2S_OUTPAD_NUM
} BachI2sOutPad_e;


typedef enum
{
    BACH_CHMODE_LEFT = 0x1,
    BACH_CHMODE_RIGHT = 0x2,
    BACH_CHMODE_BOTH = 0x3,
    BACH_CHMODE_BOTH_INV = 0x4
} BachChannelMode_e;


#ifdef __cplusplus
extern "C" {
#endif


    U16 BachGetMaskReg(BachRegBank_e nBank, U8 nAddr);
    void BachSetWakeUpMode(BOOL bWakeUp);
    void BachSysInit(void);

//void BachU02AtopInit(void);
    void BachAtopInit(void);
    void BachAtopEnDigClk(void);
    void BachAtopIdle(void);
    void BachAtopEarphoneMute(void);
    void BachAtopEarphoneEnable(void);
    BOOL BachOpenAtop(BachAtopPath_e path);
    BOOL BachCloseAtop(BachAtopPath_e path);

    void BachDigMic(BOOL bEnable);
    void BachSetOutputPathOnOff(BachOutputPath_e eOutput, BOOL bOn);
    void BachSetInputPathOnOff(BachInputPath_e eOutput, BOOL bOn);
    void BachSetInputPathGain(BachInputPath_e eInput, S8 s8Gain, BachChannelMode_e eChannel);
    S8 BachGetInputPathGain(BachInputPath_e eInput);
    void BachSetRecPathGain(BachRecPath_e eInput, S8 s8Gain);
    S8 BachGetRecPathGain(BachRecPath_e eRec);



    void BachDmaReset(void);

    void BachDmaInitChannel( U32 nChannelIndex,
                             U32 nPhysDMAAddr,
                             U32 nBufferSize,
                             U32 nChannels,
                             U32 nSampleSize,
                             U32 nSampleRate,
                             U32 nOverrunTh,
                             U32 nUnderrunTh
                           );

    U32  BachDmaGetLevelCnt(BachDmaChannel_e eDmaChannel);
    BOOL BachDmaMaskInt(BachDmaChannel_e eDmaChan, BachDmaInterrupt_e eDmaInt, BOOL bEnable);
    void BachDmaClearInt(BachDmaChannel_e eDmaChannel);
    void BachDmaReInit(BachDmaChannel_e eDmaChannel);
    void BachDmaEnable(BachDmaChannel_e eDmaChannel, BOOL bEnable);
    void BachDmaStartChannel(BachDmaChannel_e eDmaChannel);
    void BachDmaStopChannel(BachDmaChannel_e eDmaChannel);
    void BachDmaSetThreshold(BachDmaChannel_e eDmaChannel, U32 nOverrunTh, U32 nUnderrunTh);
    void BachDmaSetPhyAddr(BachDmaChannel_e eDmaChannel, U32 nBufAddr, U32 nBufSize);
    void BachDmaWriterSetMode(BachDmaChannel_e eDma, BOOL bMono);

    U32 BachGetRecorderRate(void);
    U32 BachGetPlayRecorderRate(void);

    BOOL BachAsrcConfigSynth(BachAsrc_e eAsrc, BachRate_e eRate);
    BOOL BachAsrcIntSetMux(BachAsrcInt_e eAsrc, BachSource_e eSrc);
    BachSynth_e BachAsrcGetUsedSynth(BachAsrc_e eAsrc);
    U32 BachAsrcGetSynthRate(BachAsrc_e eAsrc);

    BOOL BachDacSetSrc(BachDac_e eDac, BachDacSrc_e eSrc);
    U16 BachDacGetSrc(BachDac_e eDac);

    BOOL BachDacSelectChannel(BachDac_e eDac, BachChannelMode_e eChannel);

    BOOL BachSinkSelectChannel(BachSink_e eSink, BachChannel_e eChan);
    BOOL BachChannelSelectSrc(BachChannel_e eChan, BachSource_e eSrc);

    BachRate_e BachAdcGetRate(void);
    BachRate_e BachAuxGetRate(void);
    BachRate_e BachBtGetRate(void);
    BachRate_e BachDmaGetRate(BachDmaChannel_e eDmaChannel);

    BOOL BachDmaSetRate(BachDmaChannel_e eDmaChannel, BachRate_e eRate);
    BOOL BachAdcSetRate(BachRate_e eRate);
    BOOL BachAuxSetCfg(BachRate_e eRate, BOOL bStereo);
    BOOL BachBtSetCfg(BachI2sFmt_e eFormat, BachI2sWidth_e eWidth, BachRate_e eRate);
    BOOL BachBtSetRate(BachRate_e eRate);
    void BachBtSetWidth(BachI2sWidth_e eWidth);
    void BachBtSetChip(BachBtChip_e eChip);
    void BachBtPadEnable(BOOL bEn);
    BOOL BachCodecSetCfg(BachI2sFmt_e eFormat, BachI2sWidth_e eWidth, BachRate_e eRate);
    void BachCfgInit(void);

    BachRate_e BachRateFromU32(U32 nRate);
    U32 BachRateToU32(BachRate_e eRate);

    void BachFuncEnable(BachFunction_e eFunc, BOOL bEnable);
    BOOL BachI2sConfig(BachI2s_e eI2s, BachI2sCfg_t tCfg);
    BOOL BachI2sEnable(BachI2s_e eI2s, BOOL bEn);

    void BachDpgaGainInit(void);
    void BachDpgaInit(BachDpga_e eDpga, S8 nGain);
    void BachDpgaMute(BachDpga_e eDpga);
    void BachDpgaEnable(BachDpga_e eDpga, BOOL nEnable);
    S8   BachDpgaGetGain(BachDpga_e eDpga);
    void BachDpgaSetGain(BachDpga_e eDpga, S8 s8Gain);
    void BachDpgaFadingSize(BachDpga_e eDpga, U8 nStepIdx);


    void BachPgaInit(BachPga_e ePga);
    void BachPgaMute(BachPga_e ePga);
    void BachPgaEnable(BachPga_e ePga, BOOL bEn);
    S8   BachPgaGetGain(BachPga_e ePga);
    void BachPgaSetGain(BachPga_e ePga, S8 s8Gain);

    BOOL BachDmaIsFull(BachDmaChannel_e eDmaChannel);
    BOOL BachDmaIsEmpty(BachDmaChannel_e eDmaChannel);
    BOOL BachDmaIsLocalEmpty(BachDmaChannel_e eDmaChannel);
    BOOL BachDmaIsUnderrun(BachDmaChannel_e eDmaChannel);
    BOOL BachDmaIsOverrun(BachDmaChannel_e eDmaChannel);
    U32  BachDmaTrigLevelCnt(BachDmaChannel_e eDmaChannel, U32 nDataSize);

    void BachSelectDbbUplinkSource(BachDbbUplink_e eSource);

    void BachSetBankBaseAddr(U32 addr);


    // DMA

    U16 BachReadReg(BachRegBank_e nBank, U8 nAddr);
    void BachWriteReg(BachRegBank_e nBank, U8 nAddr, U16 regMsk, U16 nValue);
    void BachWriteReg2Byte(U32 nAddr, U16 nValue);
    void BachWriteRegByte(U32 nAddr, U8 nValue);
    U16 BachReadReg2Byte(U32 nAddr);
    U8 BachReadRegByte(U32 nAddr);

    U8 BachGetMux4(BachMux4_e eMux);
    void BachSetMux4(BachMux4_e eMux, U8 u8Choice);
    void BachSetMux2(BachMux2_e eMux, U8 u8Choice);
    void BachSetMixer(BachMixer_e eMixer, U8 nCh1, U8 nCh2);
    void BachSetDmaValidSel(BachDmaChannel_e eDma, U8 u8Choice);
    void BachSetSynthRate(BachSynth_e eSynth, U32 nRate);
    BOOL BachSetAsrcSynth(BachAsrc_e eAsrc, BachSynth_e eSynth);
    void BachSetAsrcConfig(BachAsrc_e eAsrc, BachSynth_e eSynth, BachRate_e eRate);

    BOOL BachChangeAsrcSynth(BachAsrc_e eAsrc);
    void BachResetAsrcByTable(BachAsrcCfg_e eTable[]);

    BOOL BachSetI2sRate(BachI2s_e eI2S, BachRate_e eRate);
    BOOL BachSetI2sMode(BachI2s_e eI2S, BachI2sPcmMode_e eMode);
    BOOL BachSetI2sFmt(BachI2s_e eI2S, BachI2sFmt_e eFmt);
    BOOL BachSetI2sWidth(BachI2s_e eI2S, BachI2sWidth_e eWidth);

    void BachDpgaTrigger(U8 nAddr, U16 trigBit);
    void BachDpgaCalGain(S8 s8Gain, U8 *pu8GainIdx);
    void BachDpgaCtrl(BachDpga_e eDpga, BOOL bEnable, BOOL bMute, BOOL bFade);
    void BachPgaCalGain(S8 s8Gain, U8 *pu8GainIdx);
    void BachPgaCtrl(BachPga_e ePga, BOOL bEnable, BOOL bMute);

    void BachSelectDAC1Input(BachChannelMode_e eChannel);
    void BachSelectDAC2Input(BachChannelMode_e eChannel);
    void BachSelectSinkTxInput(BachSinkTx_e eSink, BachChannelMode_e eChannel);
    void BachSelectDmaWR12Input(BachChannelMode_e eChannel);

    void BachAtopEnableRef(BOOL bEnable);
    void BachAtopADCOn(BOOL bEnable);
    void BachAtopHandsetMic(BOOL bEnable);
    void BachAtopAuxMic(BOOL bEnable);
    void BachAtopLineIn0(BOOL bEnable);
    void BachAtopLineIn1(BOOL bEnable);
    void BachAtopDACOn(BOOL bEnable);
    void BachAtopEarphone(BOOL bEnable);
    void BachAtopLineOut0(BOOL bEnable);
    void BachAtopLineOut1(BOOL bEnable);
    BOOL BachAtopIsTrimOk(void);
    BOOL BachAtopMicGain(U16 nLevel);
    BOOL BachAtopLineOutGain(BachAtopPath_e eLineOut, U16 nLevel);
    BOOL BachAtopLineInGain(BachAtopPath_e eLineIn, U16 nLevel);
    void BachAtopLineInMux(BachAtopPath_e eAtop,U16 nLineIn);

//void BachRestoreFunc();
//void BachRestoreAtop();
//void BachHaltFunc();
    void BachHaltAtop(void);

    void BachResetAsrcCfgTable(void);
    void BachI2sPadMux(BachI2sPad_e eI2s, BachI2sOutPad_e eOutI2s, BOOL bMaster);

    BOOL BachDualMicSetting(void);
    BOOL BachLineOutGainSetting(U16 nChoice);
    BachRate_e BachRateMapping(U32 nRate);

    void BachAdcMuxGuard(BachMux4_e eMux4,BOOL bOn);

    BOOL BachEarphoneIsInserted(void);
    void BachEarDetMode(U16 nTrigMode);


#ifdef __cplusplus
}
#endif


#endif  // BACH_H
