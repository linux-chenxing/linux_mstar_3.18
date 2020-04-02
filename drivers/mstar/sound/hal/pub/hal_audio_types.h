#ifndef __HAL_AUD_TYPES_H__
#define __HAL_AUD_TYPES_H__

#define TDM_ACTIVE_SLOT_0_1 (0x3 << 0)
#define TDM_ACTIVE_SLOT_2_3 (0x3 << 2)
#define TDM_ACTIVE_SLOT_4_5 (0x3 << 4)
#define TDM_ACTIVE_SLOT_6_7 (0x3 << 6)

typedef enum
{
    AUD_RATE_SLAVE,
    AUD_RATE_8K,
    AUD_RATE_16K,
    AUD_RATE_32K,
    AUD_RATE_44K,
    AUD_RATE_48K,
    AUD_RATE_96K,
    AUD_RATE_NUM,
    AUD_RATE_NULL = 0xff
}AudRate_e;

typedef enum
{
    AUD_BITWIDTH_16,
    AUD_BITWIDTH_24,
    AUD_BITWIDTH_32
}AudBitWidth_e;

typedef enum
{
    AUD_I2S_MISC_RX,
    AUD_I2S_MISC_TX,
    AUD_I2S_CODEC_RX,
    AUD_I2S_CODEC_TX,
    AUD_I2S_BT_TX,
    AUD_I2S_BT_RX,
    AUD_I2S_NUM
}AudI2s_e;


typedef enum
{
    AUD_I2S_MODE_I2S,
    AUD_I2S_MODE_TDM,
}AudI2sMode_e;


typedef enum
{
    AUD_I2S_MSMODE_MASTER,
    AUD_I2S_MSMODE_SLAVE,
}AudI2sMsMode_e;


typedef enum
{
    AUD_I2S_FMT_I2S,
    AUD_I2S_FMT_LEFT_JUSTIFY
}AudI2sFmt_e;

typedef enum
{
    AUD_TDM_CHN_MAP0,  //0,1,2,3,4,5,6,7
    AUD_TDM_CHN_MAP1,  //2,3,0,1,6,7,4,5
    AUD_TDM_CHN_MAP2,  //4,5,6,7,0,1,2,3
    AUD_TDM_CHN_MAP3,  //6,7,4,5,2,3,0,1
}AudTdmChnMap_e;


typedef enum
{
    AUD_I2S_BCK_32FS,
    AUD_I2S_BCK_64FS,
    AUD_I2S_BCK_128FS,
    AUD_I2S_BCK_256FS,
}AudI2sBck_e;

typedef struct
{
    AudI2sMode_e eMode;
    AudI2sMsMode_e eMsMode;
    AudI2sFmt_e eFormat;
    AudBitWidth_e eWidth;
    U16 nTdmChannel;
}AudI2sCfg_t;


typedef enum
{
    AUD_DMA_WRITER1,
    AUD_DMA_WRITER2,
    AUD_DMA_WRITER3,
    AUD_DMA_WRITER4,
    AUD_DMA_WRITER5,
    AUD_DMA_READER1,
    AUD_DMA_READER2,
    AUD_DMA_READER3,
    AUD_DMA_READER4,
    AUD_DMA_READER5,
    AUD_DMA_NUM,
    AUD_DMA_NULL = 0xff
}AudDmaChn_e;

typedef enum
{
    AUD_REG_BANK0,
    AUD_REG_BANK1,
    AUD_REG_BANK2,
    AUD_REG_BANK3,
    AUD_REG_BANK4,
    AUD_REG_BANK5,
}AudRegBank_e;


typedef enum
{
    AUD_ATOP_ADC,
    AUD_ATOP_DAC
} AudAtopPath_e;

typedef enum
{
    AUD_ADC_LINEIN,
    AUD_ADC_MICIN
} AudAdcPath_e;

/**
 * \brief Audio Synthesizer
 */
typedef enum
{
    AUD_SYNTH_CODEC_RX,
    AUD_SYNTH_CODEC_TX,
    AUD_SYNTH_BT_RX,
    AUD_SYNTH_BT_TX,
    AUD_SYNTH_USB,
    AUD_SYNTH_HDMI,
    AUD_SYNTH_NUM,
    AUD_SYNTH_NULL = 0xff,
}AudSynth_e;


/**
 * \brief Audio MUX
 */
typedef enum
{
    AUD_MUX2_DMA_W1,
    AUD_MUX2_DMA_W2,
    AUD_MUX2_DMA_W3,
    AUD_MUX2_DMA_W4,
    AUD_MUX4_DMA_W5,
    AUD_MUX2_SRC_B,
    AUD_MUX2_SRC_C,
    AUD_MUX2_CODEC_I2STX,
    AUD_MUX2_BT_I2STX,
    AUD_MUX_NUM,
    AUD_MUX_NULL = 0xff
}AudMux_e;


/**
 * \brief Audio Mixer
 */
typedef enum
{
    AUD_MIXER_DAC_L,
    AUD_MIXER_DAC_R,
    AUD_MIXER_CODEC_L,
    AUD_MIXER_CODEC_R,
    AUD_MIXER_NUM,
    AUD_MIXER_NULL = 0xff
}AudMixer_e;

typedef enum
{
    AUD_DMA_INT_UNDERRUN = 0,
    AUD_DMA_INT_OVERRUN,
    AUD_DMA_INT_EMPTY,
    AUD_DMA_INT_FULL,
    AUD_DMA_INT_LOCALEMPTY,
    AUD_DMA_INT_LOCALFULL,
    AUD_DMA_INT_NUM
}AudDmaInt_e;


typedef enum
{
    AUD_DMA_CLK_GPA,
    AUD_DMA_CLK_PDM,
    AUD_DMA_CLK_ADC,
    AUD_DMA_CLK_USB240FS,
    AUD_DMA_CLK_USB256FS,
    AUD_DMA_CLK_SYNTH,
    AUD_DMA_CLK_NUM
}AudDmaClkSrc_e;

typedef enum
{
    AUD_PDM_CLK_RATE_400K,
    AUD_PDM_CLK_RATE_800K,
    AUD_PDM_CLK_RATE_1000K,
    AUD_PDM_CLK_RATE_1200K,
    AUD_PDM_CLK_RATE_1600K,
    AUD_PDM_CLK_RATE_2000K,
    AUD_PDM_CLK_RATE_2400K,
    AUD_PDM_CLK_RATE_4000K,
    AUD_PDM_CLK_RATE_4800K,
    AUD_PDM_CLK_RATE_8000K,
    AUD_PDM_CLK_RATE_NUM,
    AUD_PDM_CLK_RATE_NULL = 0xff
}AudPdmClkRate_e;

typedef enum
{
    AUD_AU_INT,
    AUD_AU_INT_GEN,
    AUD_AU_INT_NUM,
}AudInt_e;

typedef struct
{
    AudDmaClkSrc_e eSrc;
    AudRate_e eRate;
}AudDmaClk_t;

#endif //__HAL_AUD_TYPES_H__