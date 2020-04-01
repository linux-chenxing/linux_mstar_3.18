/*------------------------------------------------------------------------------
	Copyright (c) 2008 MStar Semiconductor, Inc.  All rights reserved.
------------------------------------------------------------------------------*/
#ifndef _BACH_CODEC_H_
#define _BACH_CODEC_H_


#define EAR_DET 0
//------------------------------------------------------------------------------
//  Macros
//------------------------------------------------------------------------------

enum
{
AUD_ASRC_MUX1 = 0,
AUD_ASRC_MUX2,
AUD_ASRC_MIXER11,
AUD_ASRC_MIXER12,
AUD_ASRC_MIXER21,
AUD_ASRC_MIXER22,
AUD_OUT_MIXER1,
AUD_OUT_MIXER2,
AUD_ASRCDEC_MUX11,  //0x8
AUD_ASRCDEC_MUX12,
AUD_ASRCDEC_MUX21,
AUD_ASRCDEC_MUX22,
AUD_BT_RATE,
AUD_BT_CHIP,
AUD_DMAWR_SWAP,
AUD_ADC_MUX1,
AUD_ADC_MUX2, //0x10
AUD_ATOP_PWR, //0x11
AUD_DPGA_PWR, //0x12
AUD_ASRC1_DPGA,
AUD_ASRC2_DPGA,
AUD_DMAWR12_DPGA,
AUD_DMAWR34_DPGA,
AUD_MIC_GAIN,
AUD_LINEOUT1_GAIN,  //0x18
AUD_LINEOUT2_GAIN,
AUD_CODEC_I2S_DISABLE,
AUD_ADC0_GAIN,
AUD_ADC1_GAIN,
AUD_REG_LEN,
};

#if(EAR_DET)
int bach_init_hp_jack_detect(struct snd_soc_codec *codec, struct snd_soc_jack *jack, int report);
int bach_hp_jack_detect(struct snd_soc_codec *codec);
#endif

#endif /* _BACH_CODEC_H_ */
