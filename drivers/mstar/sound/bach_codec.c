/*------------------------------------------------------------------------------
 *   Copyright (c) 2008 MStar Semiconductor, Inc.  All rights reserved.
 *  ------------------------------------------------------------------------------*/
//------------------------------------------------------------------------------
//  Include files
//------------------------------------------------------------------------------
#include <linux/module.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/workqueue.h>
#include <linux/moduleparam.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
//#include <sound/driver.h>   remove it for kernel 2.6.29.1
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/initval.h>
#include <asm/div64.h>
#include <sound/tlv.h>
#include <sound/jack.h>

//#include <mach/irqs.h>

#include "bach_codec.h"
#include "bach_pcm.h"
#include "bach_audio_debug.h"
#include "bach.h"

//------------------------------------------------------------------------------
//  Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//  Variables
//------------------------------------------------------------------------------
static u16 codec_reg_backup[AUD_REG_LEN] =
{
  0x0,    //AUD_ASRC_MUX1
  0x0,    //AUD_ASRC_MUX2
  0x1,    //AUD_ASRC_MIXER11
  0x1,    //AUD_ASRC_MIXER12
  0x1,    //AUD_ASRC_MIXER21
  0x1,    //AUD_ASRC_MIXER22
  0x1,    //AUD_OUT_MIXER1
  0x2,    //AUD_OUT_MIXER2
  0x0,    //AUD_ASRCDEC_MUX11
  0x3,    //AUD_ASRCDEC_MUX12
  0x0,    //AUD_ASRCDEC_MUX21
  0x3,    //AUD_ASRCDEC_MUX22
  0x1,    //AUD_BT_RATE
  0x2,    //AUD_BT_CHIP --> MTK
  0x2,    //AUD_DMAWR_SWAP
  0x1,    //AUD_ADC_MUX1
  0x1,    //AUD_ADC_MUX2
  0x0,    //AUD_ATOP_PWR
  0x0,    //AUD_DPGA_PWR
  0, //CONFIG_MS_SOUND_SND_DEFAULT_VOLUME,      //AUD_ASRC1_DPGA
  0, //CONFIG_MS_SOUND_SND_DEFAULT_VOLUME,      //AUD_ASRC2_DPGA
  0, //CONFIG_MS_SOUND_SND_DEFAULT_C_VOLUME,    //AUD_DMAWR12_DPGA
  0, //CONFIG_MS_SOUND_SND_DEFAULT_C_VOLUME,    //AUD_DMAWR34_DPGA
  0, //CONFIG_MS_SOUND_SND_DEFAULT_MIC_GAIN,    //AUD_MIC_GAIN
  0, //CONFIG_MS_SOUND_SND_DEFAULT_LINEOUT_GAIN,//AUD_LINEOUT1_GAIN
  0, //CONFIG_MS_SOUND_SND_DEFAULT_LINEOUT_GAIN,//AUD_LINEOUT2_GAIN
  0,                                            //AUD_CODEC_I2S_DISABLE
  0, //CONFIG_MS_SOUND_SND_DEFAULT_ADC_GAIN,    //AUD_ADC0_GAIN
  0, //CONFIG_MS_SOUND_SND_DEFAULT_ADC_GAIN     //AUD_ADC1_GAIN
};

static u16 codec_reg[AUD_REG_LEN] =
{
  0x0,    //AUD_ASRC_MUX1
  0x0,    //AUD_ASRC_MUX2
  0x1,    //AUD_ASRC_MIXER11
  0x1,    //AUD_ASRC_MIXER12
  0x1,    //AUD_ASRC_MIXER21
  0x1,    //AUD_ASRC_MIXER22
  0x1,    //AUD_OUT_MIXER1
  0x2,    //AUD_OUT_MIXER2
  0x0,    //AUD_ASRCDEC_MUX11
  0x3,    //AUD_ASRCDEC_MUX12
  0x0,    //AUD_ASRCDEC_MUX21
  0x3,    //AUD_ASRCDEC_MUX22
  0x1,    //AUD_BT_RATE
  0x2,    //AUD_BT_CHIP --> MTK
  0x2,    //AUD_DMAWR_SWAP
  0x1,    //AUD_ADC_MUX1
  0x1,    //AUD_ADC_MUX2
  0x0,    //AUD_ATOP_PWR
  0x0,    //AUD_DPGA_PWR
  0, //CONFIG_MS_SOUND_SND_DEFAULT_VOLUME,        //AUD_ASRC1_DPGA
  0, //CONFIG_MS_SOUND_SND_DEFAULT_VOLUME,        //AUD_ASRC2_DPGA
  0, //CONFIG_MS_SOUND_SND_DEFAULT_C_VOLUME,      //AUD_DMAWR12_DPGA
  0, //CONFIG_MS_SOUND_SND_DEFAULT_C_VOLUME,      //AUD_DMAWR34_DPGA
  0, //CONFIG_MS_SOUND_SND_DEFAULT_MIC_GAIN,      //AUD_MIC_GAIN
  0, //CONFIG_MS_SOUND_SND_DEFAULT_LINEOUT_GAIN,  //AUD_LINEOUT1_GAIN
  0, //CONFIG_MS_SOUND_SND_DEFAULT_LINEOUT_GAIN,  //AUD_LINEOUT2_GAIN
  0,                                              //AUD_CODEC_I2S_DISABLE
  0, //CONFIG_MS_SOUND_SND_DEFAULT_ADC_GAIN,      //AUD_ADC0_GAIN
  0, //CONFIG_MS_SOUND_SND_DEFAULT_ADC_GAIN       //AUD_ADC1_GAIN
};

static struct bach_pcm_dma_data bach_pcm_dma_wr[] =
{
  {
    .name		= "DMA 2 writer",
    .channel	= BACH_DMA_WRITER2,
    //.dma_addr	= NULL,
    //.dma_size	= 0,
  },
  {
    .name		= "DMA 1 writer",
    .channel	= BACH_DMA_WRITER1,
    //.dma_addr	= NULL,
    //.dma_size	= 0,
  },
};

static struct bach_pcm_dma_data bach_pcm_dma_rd[] =
{
  {
    .name		= "DMA 1 reader",
    .channel	= BACH_DMA_READER1,
    //.dma_addr	= NULL,
    //.dma_size	= 0,
  },
  {
    .name		= "DMA 2 reader",
    .channel	= BACH_DMA_READER2,
    //.dma_addr	= NULL,
    //.dma_size	= 0,
  },
};

#if(EAR_DET)
struct bach_jack_data {
	struct snd_soc_jack *jack;
	struct delayed_work work;
	int report;
	int short_report;
};
#endif


static int snd_soc_codec_update_bits(
	struct snd_soc_codec *codec, unsigned int reg,
	unsigned int mask, unsigned int val)
{
  bool change;
	unsigned int old, new;
	int ret;

  struct snd_soc_component *component = &codec->component;

	ret = component->read(component, reg, &old);
	if (ret < 0)
		return ret;

	new = (old & ~mask) | (val & mask);
	change = old != new;
	if (change)
	{
		ret = component->write(component, reg, new);
  	if (ret < 0)
  		return ret;
	}
	return change;
}


#if DAI_LEVEL_TAG
static int bach_soc_dai_ops_trigger(struct snd_pcm_substream *substream, int cmd, struct snd_soc_dai *dai)
{
  AUD_PRINTF(DAI_LEVEL, "%s: dai = %s\n", __FUNCTION__, dai->name);
  return 0;
}

static int bach_soc_dai_ops_prepare(struct snd_pcm_substream *substream, struct snd_soc_dai *dai)
{
  AUD_PRINTF(DAI_LEVEL, "%s: dai = %s\n", __FUNCTION__, dai->name);
  return 0;
}

static int bach_soc_dai_ops_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *params, struct snd_soc_dai *dai)
{
  AUD_PRINTF(DAI_LEVEL, "%s: dai = %s\n", __FUNCTION__, dai->name);
  return 0;
}

static int bach_soc_dai_ops_hw_free(struct snd_pcm_substream *substream, struct snd_soc_dai *dai)
{
  AUD_PRINTF(DAI_LEVEL, "%s: dai = %s\n", __FUNCTION__, dai->name);
  return 0;
}

static int bach_soc_dai_ops_set_fmt(struct snd_soc_dai *dai, unsigned int fmt)
{
  AUD_PRINTF(DAI_LEVEL, "%s: dai = %s\n", __FUNCTION__, dai->name);
  return 0;
}

static int bach_soc_dai_ops_set_sysclk(struct snd_soc_dai *dai, int clk_id,  unsigned int freq, int dir)
{
  AUD_PRINTF(DAI_LEVEL, "%s: dai = %s\n", __FUNCTION__, dai->name);
  return 0;
}

static int bach_soc_dai_ops_set_pll(struct snd_soc_dai *dai, int pll_id, int source,
                                    unsigned int freq_in, unsigned int freq_out)
{
  AUD_PRINTF(DAI_LEVEL, "%s: dai = %s\n", __FUNCTION__, dai->name);
  return 0;
}

static int bach_soc_dai_ops_set_clkdiv(struct snd_soc_dai *dai, int div_id, int div)
{
  AUD_PRINTF(DAI_LEVEL, "%s: dai = %s\n", __FUNCTION__, dai->name);
  return 0;
}

static int bach_soc_dai_ops_startup(struct snd_pcm_substream *substream, struct snd_soc_dai *dai)
{
  AUD_PRINTF(DAI_LEVEL, "%s: dai = %s\n", __FUNCTION__, dai->name);
  return 0;
}

static void bach_soc_dai_ops_shutdown(struct snd_pcm_substream *substream, struct snd_soc_dai *dai)
{
  AUD_PRINTF(DAI_LEVEL, "%s: dai = %s\n", __FUNCTION__, dai->name);
}

static int bach_soc_dai_ops_digital_mute(struct snd_soc_dai *dai, int mute)
{
  AUD_PRINTF(DAI_LEVEL, "%s: dai = %s, mute = %d\n", __FUNCTION__, dai->name, mute);
  return 0;
}
#endif

static struct snd_soc_dai_ops bach_soc_codec_dai_ops =
{
#if DAI_LEVEL_TAG
  .set_sysclk = bach_soc_dai_ops_set_sysclk,
  .set_pll    = bach_soc_dai_ops_set_pll,
  .set_clkdiv = bach_soc_dai_ops_set_clkdiv,

  .set_fmt    = bach_soc_dai_ops_set_fmt,

  .startup		= bach_soc_dai_ops_startup,
  .shutdown		= bach_soc_dai_ops_shutdown,
  .trigger		= bach_soc_dai_ops_trigger,
  .prepare    = bach_soc_dai_ops_prepare,
  .hw_params	= bach_soc_dai_ops_hw_params,
  .hw_free    = bach_soc_dai_ops_hw_free,
  .digital_mute = bach_soc_dai_ops_digital_mute,
#endif
};

static int bach_soc_dai_probe(struct snd_soc_dai *dai)
{
  AUD_PRINTF(DAI_LEVEL, "%s: dai = %s\n", __FUNCTION__, dai->name);

  dai->playback_dma_data = (void *)&bach_pcm_dma_rd[dai->id];
  dai->capture_dma_data = (void *)&bach_pcm_dma_wr[dai->id];

  AUD_PRINTF(DAI_LEVEL, "setup playback_dma_data = %s, channel = %d\n", bach_pcm_dma_rd[dai->id].name, bach_pcm_dma_rd[dai->id].channel);
  AUD_PRINTF(DAI_LEVEL, "setup capture_dma_data  = %s, channel = %d\n", bach_pcm_dma_wr[dai->id].name, bach_pcm_dma_wr[dai->id].channel);

  return 0;
}

static int bach_soc_dai_remove(struct snd_soc_dai *dai)
{
  AUD_PRINTF(DAI_LEVEL, "%s: dai = %s\n", __FUNCTION__, dai->name);
  return 0;
}

static int bach_soc_dai_suspend(struct snd_soc_dai *dai)
{
  AUD_PRINTF(DAI_LEVEL, "%s: dai = %s\n", __FUNCTION__, dai->name);
  return 0;
}

static int bach_soc_dai_resume(struct snd_soc_dai *dai)
{
  AUD_PRINTF(DAI_LEVEL, "%s: dai = %s\n", __FUNCTION__, dai->name);
  return 0;
}


struct snd_soc_dai_driver bach_soc_codec_dai_drv[] =
{
  {
    .name				= "bach-codec-dai-main",
    .probe				= bach_soc_dai_probe,
    .remove       = bach_soc_dai_remove,
    .suspend			= bach_soc_dai_suspend,
    .resume				= bach_soc_dai_resume,

    .playback			=
    {
      .stream_name	= "Main Playback",
      .channels_min	= 2,
      .channels_max	= 2,
      .rates			= SNDRV_PCM_RATE_8000_48000,
      .formats		= SNDRV_PCM_FMTBIT_S16_LE |
      SNDRV_PCM_FMTBIT_S24_LE |
      SNDRV_PCM_FMTBIT_S32_LE,
    },
    .capture			=
    {
      .stream_name	= "Main Capture",
      .channels_min	= 1,
      .channels_max	= 2,
      .rates			= SNDRV_PCM_RATE_8000_48000,
      .formats		= SNDRV_PCM_FMTBIT_S16_LE,
    },
    .ops				= &bach_soc_codec_dai_ops,
  },
  {
    .name				= "bach-codec-dai-gps",
    .probe				= bach_soc_dai_probe,
    .remove       = bach_soc_dai_remove,
    .suspend			= bach_soc_dai_suspend,
    .resume				= bach_soc_dai_resume,

    .playback			=
    {
      .stream_name	= "Sub Playback",
      .channels_min	= 2,
      .channels_max	= 2,
      .rates			  = SNDRV_PCM_RATE_8000_48000,
      .formats		  = SNDRV_PCM_FMTBIT_S16_LE |
      SNDRV_PCM_FMTBIT_S24_LE |
      SNDRV_PCM_FMTBIT_S32_LE,
    },
    .capture			=
    {
      .stream_name	= "Sub Capture",
      .channels_min	= 1,
      .channels_max	= 2,
      .rates			  = SNDRV_PCM_RATE_8000_48000,
      .formats		  = SNDRV_PCM_FMTBIT_S16_LE,
    },
    .ops				= &bach_soc_codec_dai_ops,
  },

};

#if(EAR_DET)
static irqreturn_t bach_hp_jack_handler(int irq, void *data)
{
  struct bach_jack_data *jack = (struct bach_jack_data *)data;
  AUD_PRINTF(CODEC_LEVEL, "%s\n", __FUNCTION__);

  schedule_delayed_work(&jack->work, 50);

  return IRQ_HANDLED;
}


int bach_init_hp_jack_detect(struct snd_soc_codec *codec, struct snd_soc_jack *jack, int report)
{
  struct bach_jack_data *priv = snd_soc_codec_get_drvdata(codec);
  AUD_PRINTF(CODEC_LEVEL, "%s\n", __FUNCTION__);

  priv->jack = jack;
  priv->report = report;

  /* Sync status */
  bach_hp_jack_handler(INT_PMU_GPIO01,priv);

  return 0;
}
EXPORT_SYMBOL_GPL(bach_init_hp_jack_detect);

int bach_hp_jack_detect(struct snd_soc_codec *codec)
{
  struct bach_jack_data *priv = snd_soc_codec_get_drvdata(codec);
  AUD_PRINTF(CODEC_LEVEL, "%s\n", __FUNCTION__);

  /* Sync status */
  bach_hp_jack_handler(INT_PMU_GPIO01,priv);

  return 0;
}
EXPORT_SYMBOL_GPL(bach_hp_jack_detect);

static void bach_hp_work(struct bach_jack_data *jack)
{
  BOOL bPlugIn;
  int report;
  bPlugIn = BachEarphoneIsInserted();
  AUD_PRINTF(CODEC_LEVEL, "%s - PlugIn:%d\n", __FUNCTION__,bPlugIn);
  if (bPlugIn)
  {
    BachEarDetMode(0); //rising -> falling
    report = jack->report;
  }
  else
  {
    BachEarDetMode(1); //falling -> rising
    report = 0;
  }

  snd_soc_jack_report(jack->jack, report, jack->report);

}

static void bach_hp_jack_work(struct work_struct *work)
{
  struct bach_jack_data *priv = container_of(work, struct bach_jack_data, work.work);

  bach_hp_work(priv);
}

static int bach_soc_codec_probe(struct snd_soc_codec *codec)
{

  struct bach_jack_data *priv;
  int err = 0;
  AUD_PRINTF(CODEC_LEVEL, "%s: codec = %s\n", __FUNCTION__, dev_name(codec->dev));

  priv = devm_kzalloc(codec->dev, sizeof(struct bach_jack_data), GFP_KERNEL);
  if (priv == NULL)
    return -ENOMEM;
  snd_soc_codec_set_drvdata(codec, priv);

  INIT_DELAYED_WORK(&priv->work, bach_hp_jack_work);

  err = request_irq(INT_PMU_GPIO01, //INT_MS_AUDIO_1,
                    bach_hp_jack_handler,
                    0,
                    "HP jack detect",
                    (void*)priv);
  //snd_soc_dapm_disable_pin(&codec->dapm, "DMARD1");
  //snd_soc_dapm_disable_pin(&codec->dapm, "DMARD2");
  //snd_soc_dapm_sync(&codec->dapm);
  //memcpy(codec_reg_backup, codec_reg, sizeof(codec_reg));

  return 0;
}

static int bach_soc_codec_remove(struct snd_soc_codec *codec)
{
  struct bach_jack_data *priv = snd_soc_codec_get_drvdata(codec);
  AUD_PRINTF(CODEC_LEVEL, "%s: codec = %s\n", __FUNCTION__, dev_name(codec->dev));
  free_irq(INT_PMU_GPIO01,(void*)priv);
  priv->jack = NULL;

  cancel_delayed_work_sync(&priv->work);

  flush_delayed_work_sync(&codec->dapm.delayed_work);

  return 0;
}
#else
static int bach_soc_codec_probe(struct snd_soc_codec *codec)
{

  AUD_PRINTF(CODEC_LEVEL, "%s: codec = %s\n", __FUNCTION__, dev_name(codec->dev));

  BachSysInit();
  AUD_PRINTF(TRACE_LEVEL, "Init system register\n");

  BachCfgInit();

  BachDpgaGainInit();

  //BachSetInputPathGain(BACH_INPUT_PATH_ASRC1, (S8)(BACH_DPGA_GAIN_MIN_DB + CONFIG_MS_SOUND_SND_DEFAULT_VOLUME), BACH_CHMODE_BOTH);
  //BachSetInputPathGain(BACH_INPUT_PATH_ASRC2, (S8)(BACH_DPGA_GAIN_MIN_DB + CONFIG_MS_SOUND_SND_DEFAULT_VOLUME), BACH_CHMODE_BOTH);
  //BachSetRecPathGain(BACH_REC_PATH_DMA1, (S8)(BACH_DPGA_GAIN_MIN_DB + CONFIG_MS_SOUND_SND_DEFAULT_C_VOLUME));
  //BachSetRecPathGain(BACH_REC_PATH_DMA2, (S8)(BACH_DPGA_GAIN_MIN_DB + CONFIG_MS_SOUND_SND_DEFAULT_C_VOLUME));
  snd_soc_write(codec, AUD_ASRC1_DPGA, codec_reg_backup[AUD_ASRC1_DPGA]);
  snd_soc_write(codec, AUD_ASRC2_DPGA, codec_reg_backup[AUD_ASRC2_DPGA]);
  snd_soc_write(codec, AUD_DMAWR12_DPGA, codec_reg_backup[AUD_DMAWR12_DPGA]);
  snd_soc_write(codec, AUD_DMAWR34_DPGA, codec_reg_backup[AUD_DMAWR34_DPGA]);


  //BachAtopMicGain(CONFIG_MS_SOUND_SND_DEFAULT_MIC_GAIN);
  //BachAtopLineOutGain(BACH_ATOP_LINEOUT0, CONFIG_MS_SOUND_SND_DEFAULT_LINEOUT_GAIN);
  //BachAtopLineOutGain(BACH_ATOP_LINEOUT1, CONFIG_MS_SOUND_SND_DEFAULT_LINEOUT_GAIN);
  //BachAtopLineInGain(BACH_ATOP_LINEIN0, CONFIG_MS_SOUND_SND_DEFAULT_ADC_GAIN);
  //BachAtopLineInGain(BACH_ATOP_LINEIN1, CONFIG_MS_SOUND_SND_DEFAULT_ADC_GAIN);
  snd_soc_write(codec, AUD_MIC_GAIN, codec_reg_backup[AUD_MIC_GAIN]);
  snd_soc_write(codec, AUD_LINEOUT1_GAIN, codec_reg_backup[AUD_LINEOUT1_GAIN]);
  snd_soc_write(codec, AUD_LINEOUT2_GAIN, codec_reg_backup[AUD_LINEOUT2_GAIN]);
  snd_soc_write(codec, AUD_ADC0_GAIN, codec_reg_backup[AUD_ADC0_GAIN]);
  snd_soc_write(codec, AUD_ADC1_GAIN, codec_reg_backup[AUD_ADC1_GAIN]);

  codec_reg_backup[AUD_CODEC_I2S_DISABLE] = 1;
  snd_soc_write(codec, AUD_CODEC_I2S_DISABLE, codec_reg_backup[AUD_CODEC_I2S_DISABLE]);

  BachSetInputPathOnOff(BACH_INPUT_PATH_ASRC1, FALSE);
  BachSetInputPathOnOff(BACH_INPUT_PATH_ASRC2, FALSE);
  BachSetOutputPathOnOff(BACH_OUTPUT_PATH_DMA1_WRITER, FALSE);
  BachSetOutputPathOnOff(BACH_OUTPUT_PATH_DMA2_WRITER, FALSE);

  /*MUST sync to codec_reg[] */
  BachAuxSetCfg(BACH_RATE_8K, TRUE);
  BachSinkSelectChannel(BACH_SINK_DMA1_WRITER, BACH_CHAN_LINEIN);

  BachAdcSetRate(BACH_RATE_8K);
  BachSinkSelectChannel(BACH_SINK_DMA2_WRITER, BACH_CHAN_MICIN);

  BachSinkSelectChannel(BACH_SINK_CODEC_I2S,  BACH_CHAN_ASRC1);
  BachSinkSelectChannel(BACH_SINK_SPDIF_HDMI, BACH_CHAN_ASRC1);

  //snd_soc_dapm_disable_pin(&codec->dapm, "DMARD1");
  //snd_soc_dapm_disable_pin(&codec->dapm, "DMARD2");
  //snd_soc_dapm_sync(&codec->dapm);
  //memcpy(codec_reg_backup, codec_reg, sizeof(codec_reg));

  return 0;
}

static int bach_soc_codec_remove(struct snd_soc_codec *codec)
{
  AUD_PRINTF(CODEC_LEVEL, "%s: codec = %s\n", __FUNCTION__, dev_name(codec->dev));
  return 0;
}

#endif

static int bach_soc_codec_resume(struct snd_soc_codec *codec)
{
  int i = 0;
  u16 tmp = 0;

  AUD_PRINTF(CODEC_LEVEL, "%s: codec = %s\n", __FUNCTION__, dev_name(codec->dev));

  for (i = 0; i < AUD_REG_LEN; i++)
  {
    tmp = codec_reg[i];
    snd_soc_codec_update_bits(codec, i, 0xff, codec_reg_backup[i]);
    codec_reg_backup[i] = tmp;
  }
#if 0
  snd_soc_dapm_enable_pin(&codec->dapm, "DMAWR1");
  snd_soc_dapm_enable_pin(&codec->dapm, "DMAWR2");
  snd_soc_dapm_enable_pin(&codec->dapm, "DMARD1");
  snd_soc_dapm_enable_pin(&codec->dapm, "DMARD2");
  snd_soc_dapm_enable_pin(&codec->dapm, "BTRX");
  snd_soc_dapm_enable_pin(&codec->dapm, "LINEIN");
  snd_soc_dapm_sync(&codec->dapm);
#endif

  return 0;
}

static int bach_soc_codec_suspend(struct snd_soc_codec *codec)
{

  int i = 0;
  u16 tmp = 0;

  AUD_PRINTF(CODEC_LEVEL, "%s: codec = %s\n", __FUNCTION__, dev_name(codec->dev));

  for (i = 0; i < AUD_REG_LEN; i++)
  {
    tmp = codec_reg_backup[i];
    codec_reg_backup[i] = codec_reg[i];;
    codec_reg[i] = tmp;
  }

#if 0
  //snd_soc_dapm_disable_pin(&codec->dapm, "DMAWR1");
  //snd_soc_dapm_disable_pin(&codec->dapm, "DMAWR2");
  snd_soc_dapm_disable_pin(&codec->card->dapm, "DMARD1");
  snd_soc_dapm_disable_pin(&codec->card->dapm, "DMARD2");
  snd_soc_dapm_disable_pin(&codec->card->dapm, "BTRX");
  snd_soc_dapm_disable_pin(&codec->card->dapm, "LINEIN1");
  snd_soc_dapm_sync(&codec->card->dapm);
#endif

  return 0;
}

unsigned int bach_codec_read(struct snd_soc_codec *codec, unsigned int reg)
{

  AUD_PRINTF(CODEC_LEVEL, "%s: codec = %s, reg = 0x%x, val = 0x%x\n", __FUNCTION__, dev_name(codec->dev), reg, codec_reg[reg]);
  return codec_reg[reg];
}



int bach_codec_write(struct snd_soc_codec *codec, unsigned int reg, unsigned int value)
{
  int ret = 0;

  AUD_PRINTF(CODEC_LEVEL, "%s: codec = %s, reg = 0x%x, val = 0x%x\n", __FUNCTION__, dev_name(codec->dev), reg, value);

  switch(reg)
  {
  case AUD_ASRC_MUX1:
    if (value == 0)
    {
      //BachDmaSetRate(BACH_DMA_READER1, BACH_RATE_48K);
      BachChannelSelectSrc(BACH_CHAN_ASRC1, BACH_SRC_DMA1_READER);
      BachDacSelectChannel(BACH_DAC1, BACH_CHMODE_BOTH);

      BachSelectSinkTxInput(BACH_SINKTX_CODEC_I2S, BACH_CHMODE_BOTH);
      BachSelectSinkTxInput(BACH_SINKTX_SPDIF, BACH_CHMODE_BOTH);
      BachSelectSinkTxInput(BACH_SINKTX_HDMI, BACH_CHMODE_BOTH);

      BachBtPadEnable(FALSE);
    }
    else if (value == 1)
    {
      //BachDmaSetRate(BACH_DMA_READER1, BACH_RATE_48K);  //R
      //BachBtSetCfg //L
      if (BachBtGetRate() != BachDmaGetRate(BACH_DMA_READER1)
        && BACH_RATE_SLAVE != BachBtGetRate()
        && 0x3 == codec_reg[AUD_ASRCDEC_MUX11])
      {
        snd_soc_update_bits(codec, AUD_ASRCDEC_MUX11, 0xff, 0);
      }
      BachChannelSelectSrc(BACH_CHAN_ASRC1, BACH_SRC_BT_I2S);
      BachDacSelectChannel(BACH_DAC1, BACH_CHMODE_LEFT);

      BachSelectSinkTxInput(BACH_SINKTX_CODEC_I2S, BACH_CHMODE_LEFT);
      BachSelectSinkTxInput(BACH_SINKTX_SPDIF, BACH_CHMODE_LEFT);
      BachSelectSinkTxInput(BACH_SINKTX_HDMI, BACH_CHMODE_LEFT);

      BachBtPadEnable(TRUE);
    }
    else if (value == 2)
    {
      BachChannelSelectSrc(BACH_CHAN_ASRC1, BACH_SRC_AUX);  //default 32k
      BachDacSelectChannel(BACH_DAC1, BACH_CHMODE_BOTH);

      BachSelectSinkTxInput(BACH_SINKTX_CODEC_I2S, BACH_CHMODE_BOTH);
      BachSelectSinkTxInput(BACH_SINKTX_SPDIF, BACH_CHMODE_BOTH);
      BachSelectSinkTxInput(BACH_SINKTX_HDMI, BACH_CHMODE_BOTH);

      BachBtPadEnable(FALSE);
    }
    else if (value == 3)
    {
      BachChannelSelectSrc(BACH_CHAN_ASRC1, BACH_SRC_DBB_I2S);
    }
    else
      AUD_PRINTF(ERROR_LEVEL, "%s error parameter, reg = 0x%x, val = 0x%x\n",
                 __FUNCTION__, reg, value);

    //snd_soc_dapm_sync(&codec->dapm);
    //snd_soc_dapm_sync_unlocked(&codec->dapm);

    break;
  case AUD_ASRC_MUX2:
    if (value == 0)
    {
      BachChannelSelectSrc(BACH_CHAN_ASRC2, BACH_SRC_DMA2_READER);
    }
    else if (value == 1)
    {
      BachChannelSelectSrc(BACH_CHAN_ASRC2, BACH_SRC_DMA1_READER);
    }
    else
      AUD_PRINTF(ERROR_LEVEL, "%s error parameter, reg = 0x%x, val = 0x%x\n",
                 __FUNCTION__, reg, value);

    break;
  case AUD_ASRC_MIXER11:
    if (codec_reg[AUD_ASRCDEC_MUX11] == 0x3)
    {
      //BachDmaSetRate(BACH_DMA_WRITER2, BACH_RATE_48K);
      if (value == 0x1)
        BachSinkSelectChannel(BACH_SINK_DMA2_WRITER, BACH_CHAN_ASRC1);
      else if(value == 0x2)
        BachSinkSelectChannel(BACH_SINK_DMA2_WRITER, BACH_CHAN_ASRC2);
      else if(value == 0x3)
        BachSinkSelectChannel(BACH_SINK_DMA2_WRITER, BACH_CHAN_MIX);
      else
        AUD_PRINTF(ERROR_LEVEL, "%s error parameter, reg = 0x%x, val = 0x%x\n",
                   __FUNCTION__, reg, value);
    }
    break;
  case AUD_ASRC_MIXER12:
    //if (codec_reg[AUD_ASRCDEC_MUX12] == 0x3)
  {
    //BachDmaSetRate(BACH_DMA_WRITER2, BACH_RATE_48K);
    if (value == 0x1)
      BachSinkSelectChannel(BACH_SINK_CODEC_I2S, BACH_CHAN_ASRC1);
    else if(value == 0x2)
      BachSinkSelectChannel(BACH_SINK_CODEC_I2S, BACH_CHAN_ASRC2);
    else if(value == 0x3)
      BachSinkSelectChannel(BACH_SINK_CODEC_I2S, BACH_CHAN_MIX);
    else
      AUD_PRINTF(ERROR_LEVEL, "%s error parameter, reg = 0x%x, val = 0x%x\n",
                 __FUNCTION__, reg, value);
  }
  break;
  case AUD_ASRC_MIXER21:
    if (codec_reg[AUD_ASRCDEC_MUX21] == 0x3)
    {
      //BachDmaSetRate(BACH_DMA_WRITER1, BACH_RATE_48K);
      if (value == 0x1)
        BachSinkSelectChannel(BACH_SINK_DMA1_WRITER, BACH_CHAN_ASRC1);
      else if(value == 0x2)
        BachSinkSelectChannel(BACH_SINK_DMA1_WRITER, BACH_CHAN_ASRC2);
      else if(value == 0x3)
        BachSinkSelectChannel(BACH_SINK_DMA1_WRITER, BACH_CHAN_MIX);
      else
        AUD_PRINTF(ERROR_LEVEL, "%s error parameter, reg = 0x%x, val = 0x%x\n",
                   __FUNCTION__, reg, value);
    }
    break;
  case AUD_ASRC_MIXER22:
    //if (codec_reg[AUD_ASRCDEC_MUX22] == 0x3)
  {
    //BachDmaSetRate(BACH_DMA_WRITER2, BACH_RATE_48K);
    if (value == 0x1)
      BachSinkSelectChannel(BACH_SINK_SPDIF_HDMI, BACH_CHAN_ASRC1);
    else if(value == 0x2)
      BachSinkSelectChannel(BACH_SINK_SPDIF_HDMI, BACH_CHAN_ASRC2);
    else if(value == 0x3)
      BachSinkSelectChannel(BACH_SINK_SPDIF_HDMI, BACH_CHAN_MIX);
    else
      AUD_PRINTF(ERROR_LEVEL, "%s error parameter, reg = 0x%x, val = 0x%x\n", __FUNCTION__, reg, value);
  }
  break;
  case AUD_OUT_MIXER1:
    if (value == 0x1)
    {
      BachDacSetSrc(BACH_DAC1, BACH_DAC_ASRC1);	//speaker1
    }
    else if (value == 0x2)
    {
      BachDacSetSrc(BACH_DAC1, BACH_DAC_ASRC2);	//speaker1
    }
    else if (value == 0x3)
    {
      BachDacSetSrc(BACH_DAC1, BACH_DAC_MIX);	//speaker1
    }
    else
      AUD_PRINTF(ERROR_LEVEL, "%s error parameter, reg = 0x%x, val = 0x%x\n", __FUNCTION__, reg, value);
    break;
  case AUD_OUT_MIXER2:
    if (value == 0x1)
    {
      BachDacSetSrc(BACH_DAC2, BACH_DAC_ASRC1);	//speaker2
    }
    else if (value == 0x2)
    {
      BachDacSetSrc(BACH_DAC2, BACH_DAC_ASRC2);	//speaker2
    }
    else if (value == 0x3)
    {
      BachDacSetSrc(BACH_DAC2, BACH_DAC_MIX);	//speaker2
    }
    else
      AUD_PRINTF(ERROR_LEVEL, "%s error parameter, reg = 0x%x, val = 0x%x\n", __FUNCTION__, reg, value);
    break;
  case AUD_ASRCDEC_MUX11:
    if (value == 0)
    {
      BachAdcSetRate(BACH_RATE_8K);
      BachSinkSelectChannel(BACH_SINK_DMA2_WRITER, BACH_CHAN_MICIN);
    }
    else if (value == 0x1)
    {
      BachAdcSetRate(BACH_RATE_16K);
      BachSinkSelectChannel(BACH_SINK_DMA2_WRITER, BACH_CHAN_MICIN);
    }
    else if (value == 0x2)
    {
      BachAdcSetRate(BACH_RATE_32K);
      BachSinkSelectChannel(BACH_SINK_DMA2_WRITER, BACH_CHAN_MICIN);
    }
    else if (value == 0x3)
    {
      AUD_PRINTF(CODEC_LEVEL, "AUD_ASRCDEC_MUX11, DMA WR2 rate = %d\n", BachRateToU32(BachDmaGetRate(BACH_DMA_WRITER2)));
      //BachDmaSetRate(BACH_DMA_WRITER2, BACH_RATE_48K);    //set by bach_pcm_hw_params
      if (codec_reg[AUD_ASRC_MIXER11] == 0x1)
        BachSinkSelectChannel(BACH_SINK_DMA2_WRITER, BACH_CHAN_ASRC1);
      else if(codec_reg[AUD_ASRC_MIXER11] == 0x2)
        BachSinkSelectChannel(BACH_SINK_DMA2_WRITER, BACH_CHAN_ASRC2);
      else if(codec_reg[AUD_ASRC_MIXER11] == 0x3)
        BachSinkSelectChannel(BACH_SINK_DMA2_WRITER, BACH_CHAN_MIX);
      else
        AUD_PRINTF(ERROR_LEVEL, "%s error parameter, reg = 0x%x, val = 0x%x\n",
                   __FUNCTION__, reg, value);
    }
    break;
  case AUD_ASRCDEC_MUX12:
    break;
  case AUD_ASRCDEC_MUX21:
    if (value == 0x0)
    {
      BachAuxSetCfg(BACH_RATE_8K, TRUE);
      BachSinkSelectChannel(BACH_SINK_DMA1_WRITER, BACH_CHAN_LINEIN);
    }
    else if (value == 0x1)
    {
      BachAuxSetCfg(BACH_RATE_16K, TRUE);
      BachSinkSelectChannel(BACH_SINK_DMA1_WRITER, BACH_CHAN_LINEIN);
    }
    else if (value == 0x2)
    {
      BachAuxSetCfg(BACH_RATE_32K, TRUE);
      BachSinkSelectChannel(BACH_SINK_DMA1_WRITER, BACH_CHAN_LINEIN);
    }
    else if (value == 0x3)
    {
      AUD_PRINTF(CODEC_LEVEL, "AUD_ASRCDEC_MUX21, DMA WR1 rate = %d\n", BachRateToU32(BachDmaGetRate(BACH_DMA_WRITER1)));
      //BachDmaSetRate(BACH_DMA_WRITER1, BACH_RATE_48K);
      if (codec_reg[AUD_ASRC_MIXER21] == 0x1)
        BachSinkSelectChannel(BACH_SINK_DMA1_WRITER, BACH_CHAN_ASRC1);
      else if(codec_reg[AUD_ASRC_MIXER21] == 0x2)
        BachSinkSelectChannel(BACH_SINK_DMA1_WRITER, BACH_CHAN_ASRC2);
      else if(codec_reg[AUD_ASRC_MIXER21] == 0x3)
        BachSinkSelectChannel(BACH_SINK_DMA1_WRITER, BACH_CHAN_MIX);
      else
        AUD_PRINTF(ERROR_LEVEL, "%s error parameter, reg = 0x%x, val = 0x%x\n",
                   __FUNCTION__, reg, value);

      //BachAuxSetCfg(BACH_RATE_48K, TRUE);
    }

    if (codec_reg[AUD_ASRC_MUX1] == 2)
    {
      BachChannelSelectSrc(BACH_CHAN_ASRC1, BACH_SRC_AUX);
    }

    break;
  case AUD_ASRCDEC_MUX22:
    break;

  case AUD_BT_RATE:

    if (value <= 0x3)
      BachBtSetRate((BachRate_e)value);
    else if (value == 0x4)
      BachBtSetRate(BACH_RATE_48K);
    else
      AUD_PRINTF(ERROR_LEVEL, "%s error parameter, reg = 0x%x, val = 0x%x\n",
                 __FUNCTION__, reg, value);

    if (codec_reg[AUD_ASRC_MUX1] == 1)
    {
      //BachDmaSetRate(BACH_DMA_READER1, BACH_RATE_48K);  //R
      //BachBtSetCfg //L
      BachChannelSelectSrc(BACH_CHAN_ASRC1, BACH_SRC_BT_I2S);
      BachBtPadEnable(TRUE);
    }
    break;

  case AUD_BT_CHIP:

    if (value == 0x0)
      BachBtSetChip(BACH_BT_MSTAR);
    else if (value == 0x1)
      BachBtSetChip(BACH_BT_EXT);
    else if (value == 0x2)
      BachBtSetChip(BACH_BT_MXK);
    else
      AUD_PRINTF(ERROR_LEVEL, "%s error parameter, reg = 0x%x, val = 0x%x\n",
                 __FUNCTION__, reg, value);

    if (codec_reg[AUD_ASRC_MUX1] == 1)
    {
      BachBtPadEnable(TRUE);
    }
    break;

  case AUD_DMAWR_SWAP:

    //if (codec_reg[AUD_ASRCDEC_MUX11] != 0x3)
    ret = snd_soc_codec_update_bits(codec, AUD_ATOP_PWR, 0x8, 0);
    {
      if (value == 0x0)
      {
        BachSetMux2(BACH_MUX2_DMAWR1, 0);
        BachSetMux2(BACH_MUX2_DMAWR2, 1);
      }
      else if (value == 0x1)
      {
        BachSetMux2(BACH_MUX2_DMAWR1, 1);
        BachSetMux2(BACH_MUX2_DMAWR2, 0);
      }
      else if (value == 0x2)
      {
        BachSetMux2(BACH_MUX2_DMAWR1, 0);
        BachSetMux2(BACH_MUX2_DMAWR2, 0);
      }
      else if (value == 0x3)
      {
        BachSetMux2(BACH_MUX2_DMAWR1, 1);
        BachSetMux2(BACH_MUX2_DMAWR2, 1);
      }

      codec_reg[reg] = value;

    }
    if (ret > 0)
      snd_soc_codec_update_bits(codec, AUD_ATOP_PWR, 0x8, 0x8);

  break;

  case AUD_ADC_MUX1:

    ret = snd_soc_codec_update_bits(codec, AUD_ATOP_PWR, 0x8, 0);
    codec_reg[reg] = value;
    if (ret > 0)
      snd_soc_codec_update_bits(codec, AUD_ATOP_PWR, 0x8, 0x8);
    break;

  case AUD_ADC_MUX2:

    BachAtopLineInMux(BACH_ATOP_LINEIN1, value);

    break;

  case AUD_ATOP_PWR:
    if ((codec_reg[reg] ^ value) & 0x1)
    {
      (value & 0x1) ? BachOpenAtop(BACH_ATOP_LINEOUT0) : BachCloseAtop(BACH_ATOP_LINEOUT0);
    }
    if ((codec_reg[reg] ^ value) & 0x2)
    {
      (value & 0x2) ? BachOpenAtop(BACH_ATOP_LINEOUT1) : BachCloseAtop(BACH_ATOP_LINEOUT1);
    }
    if ((codec_reg[reg] ^ value) & 0x4)
    {
      (value & 0x4) ? BachOpenAtop(BACH_ATOP_EAR) : BachCloseAtop(BACH_ATOP_EAR);
    }
    if ((codec_reg[reg] ^ value) & 0x8)
    {
      if (codec_reg[AUD_ADC_MUX1] == 0x1)
      {
        if (codec_reg[AUD_DMAWR_SWAP] == 0x0)
        {
          (value & 0x8) ? BachOpenAtop(BACH_ATOP_HANDSET) : BachCloseAtop(BACH_ATOP_HANDSET);
        }
        else if (codec_reg[AUD_DMAWR_SWAP] == 0x1)
        {
          (value & 0x8) ? BachOpenAtop(BACH_ATOP_AUXMIC) : BachCloseAtop(BACH_ATOP_AUXMIC);
        }
        else
        {
          (value & 0x8) ? BachOpenAtop(BACH_ATOP_HANDSET) : BachCloseAtop(BACH_ATOP_HANDSET);
          (value & 0x8) ? BachOpenAtop(BACH_ATOP_AUXMIC) : BachCloseAtop(BACH_ATOP_AUXMIC);
        }
      }
      else
        (value & 0x8) ? BachOpenAtop(BACH_ATOP_LINEIN0) : BachCloseAtop(BACH_ATOP_LINEIN0);
    }
    if ((codec_reg[reg] ^ value) & 0x10)
    {
      if (codec_reg[AUD_ASRC_MUX1] == 0x2)
         ret = snd_soc_codec_update_bits(codec, AUD_DPGA_PWR, 0x1, 0);

      (value & 0x10) ? BachOpenAtop(BACH_ATOP_LINEIN1) : BachCloseAtop(BACH_ATOP_LINEIN1);

      //schedule_timeout(msecs_to_jiffies(1000 * 1000));
      if (ret > 0)
          snd_soc_codec_update_bits(codec, AUD_DPGA_PWR, 0x1, 0x1);
    }

    break;

  case AUD_DPGA_PWR:
    if ((codec_reg[reg] ^ value) & 0x1)
    {
      BachSetInputPathOnOff(BACH_INPUT_PATH_ASRC1, (value & 0x1) ? TRUE : FALSE);
    }
    if ((codec_reg[reg] ^ value) & 0x2)
    {
      BachSetInputPathOnOff(BACH_INPUT_PATH_ASRC2, (value & 0x2) ? TRUE : FALSE);
    }
    if ((codec_reg[reg] ^ value) & 0x4)
    {
      BachSetOutputPathOnOff(BACH_OUTPUT_PATH_DMA2_WRITER, (value & 0x4) ? TRUE : FALSE);
    }
    if ((codec_reg[reg] ^ value) & 0x8)
    {
      BachSetOutputPathOnOff(BACH_OUTPUT_PATH_DMA1_WRITER, (value & 0x8) ? TRUE : FALSE);
    }
    break;

  case AUD_ASRC1_DPGA:
    BachSetInputPathGain(BACH_INPUT_PATH_ASRC1, (S8)(BACH_DPGA_GAIN_MIN_DB + value), BACH_CHMODE_BOTH);
    break;
  case AUD_ASRC2_DPGA:
    BachSetInputPathGain(BACH_INPUT_PATH_ASRC2, (S8)(BACH_DPGA_GAIN_MIN_DB + value), BACH_CHMODE_BOTH);
    break;
  case AUD_DMAWR12_DPGA:
    BachSetRecPathGain(BACH_REC_PATH_DMA2, (S8)(BACH_DPGA_GAIN_MIN_DB + value));
    break;
  case AUD_DMAWR34_DPGA:
    BachSetRecPathGain(BACH_REC_PATH_DMA1, (S8)(BACH_DPGA_GAIN_MIN_DB + value));
    break;
  case AUD_MIC_GAIN:
    ret = snd_soc_codec_update_bits(codec, AUD_ATOP_PWR, 0x8, 0);
    BachAtopMicGain(value);
    if (ret > 0)
      snd_soc_codec_update_bits(codec, AUD_ATOP_PWR, 0x8, 0x8);
    break;
  case AUD_LINEOUT1_GAIN:
    ret = snd_soc_codec_update_bits(codec, AUD_ATOP_PWR, 0x1, 0);
    BachAtopLineOutGain(BACH_ATOP_LINEOUT0, value);
    if (ret > 0)
      snd_soc_codec_update_bits(codec, AUD_ATOP_PWR, 0x1, 0x1);
    break;
  case AUD_LINEOUT2_GAIN:
    ret = snd_soc_codec_update_bits(codec, AUD_ATOP_PWR, 0x2, 0);
    BachAtopLineOutGain(BACH_ATOP_LINEOUT1, value);
    if (ret > 0)
      snd_soc_codec_update_bits(codec, AUD_ATOP_PWR, 0x2, 0x2);
    break;
  case AUD_CODEC_I2S_DISABLE:
    if (value)
    {
      BachI2sEnable(BACH_I2S_CODEC_TX, FALSE);
      AUD_PRINTF(TRACE_LEVEL, "Switch gpio28 pad to gpio mode\n");
    }
    else
    {
      BachI2sEnable(BACH_I2S_CODEC_TX, TRUE);
      AUD_PRINTF(TRACE_LEVEL, "switch gpio28 pad to mclk mode\n");
    }
    break;
  case AUD_ADC0_GAIN:
    ret = snd_soc_codec_update_bits(codec, AUD_ATOP_PWR, 0x8, 0);
    BachAtopLineInGain(BACH_ATOP_LINEIN0, value);
    if (ret > 0)
      snd_soc_codec_update_bits(codec, AUD_ATOP_PWR, 0x8, 0x8);
    break;
  case AUD_ADC1_GAIN:
    ret = snd_soc_codec_update_bits(codec, AUD_ATOP_PWR, 0x10, 0);
    BachAtopLineInGain(BACH_ATOP_LINEIN1, value);
    if (ret > 0)
      snd_soc_codec_update_bits(codec, AUD_ATOP_PWR, 0x10, 0x10);
    break;

  default:
    AUD_PRINTF(ERROR_LEVEL, "%s error parameter, reg = 0x%x, val = 0x%x\n",
               __FUNCTION__, reg, value);
    break;
  }

  codec_reg[reg] = value;

  return 0;
}

static const unsigned int bach_dpga_tlv[] =
{
  TLV_DB_RANGE_HEAD(1),
  0, 76, TLV_DB_LINEAR_ITEM(-64, 12),
};

static const char *bach_bt_rate[]  = {"Slave", "Master 8k", "Master 16k",  "Master 32k", "Master 48k"};
static const char *bach_bt_chip[]  = {"Mstar", "Ext", "Mxk"};
static const char *bach_dmawr_chan[]  = {"Left/Left", "Right/Right", "Left/Right", "Left/Right Swap"};

static const char *bach_codeci2s_switch[] = {"Enable", "Disable"};

static const struct soc_enum bach_btrate_enum =
  SOC_ENUM_SINGLE(AUD_BT_RATE, 0, 5, bach_bt_rate);
static const struct soc_enum bach_btchip_enum =
  SOC_ENUM_SINGLE(AUD_BT_CHIP, 0, 3, bach_bt_chip);

static const struct soc_enum bach_chan_enum =
  SOC_ENUM_SINGLE(AUD_DMAWR_SWAP, 0, 4, bach_dmawr_chan);

static const struct soc_enum bach_codeci2s_enum =
  SOC_ENUM_SINGLE(AUD_CODEC_I2S_DISABLE, 0, 2, bach_codeci2s_switch);

static const char *bach_chip[]     = {"Cedric"};

static const struct soc_enum bach_chip_enum =
  SOC_ENUM_SINGLE(SND_SOC_NOPM, 0, 1, bach_chip);


static const struct snd_kcontrol_new bach_snd_controls[] =
{
  SOC_ENUM("Chip Name", bach_chip_enum),
  SOC_ENUM("BT Rate Select", bach_btrate_enum),
  SOC_ENUM("BT Chip Select", bach_btchip_enum),
  SOC_ENUM("Main Capture Channel Select", bach_chan_enum),
  SOC_SINGLE_TLV("Main Playback Volume", AUD_ASRC1_DPGA, 0, 76, 0, bach_dpga_tlv),
  SOC_SINGLE_TLV("Sub Playback Volume", AUD_ASRC2_DPGA, 0, 76, 0, bach_dpga_tlv),
  SOC_SINGLE_TLV("Main Capture Volume", AUD_DMAWR12_DPGA, 0, 76, 0, bach_dpga_tlv),
  SOC_SINGLE_TLV("Sub Capture Volume", AUD_DMAWR34_DPGA, 0, 76, 0, bach_dpga_tlv),
  SOC_SINGLE_TLV("Mic Gain Level", AUD_MIC_GAIN, 0, 5, 0, NULL),
  SOC_SINGLE_TLV("Line Out1 Gain Level", AUD_LINEOUT1_GAIN, 0, 2, 0, NULL),   //mute+gain
  SOC_SINGLE_TLV("Line Out2 Gain Level", AUD_LINEOUT2_GAIN, 0, 2, 0, NULL),   //mute+gain
  SOC_SINGLE_TLV("ADC0 Gain Level", AUD_ADC0_GAIN, 0, 6, 0, NULL),  //mute+gain
  SOC_SINGLE_TLV("ADC1 Gain Level", AUD_ADC1_GAIN, 0, 6, 0, NULL),  //mute+gain
  SOC_ENUM("Codec I2S Switch", bach_codeci2s_enum),
};

static const char *bach_output_select[]  = {"DMA1 Reader", "BT Rx", "Line In",  "DBB Rx"};
static const char *bach_output2_select[]  = {"DMA2 Reader", "DMA1 Reader"};
static const char *bach_input_select[]   = {"ADC0 8k", "ADC0 16k", "ADC0 32k", "Mixer"};
static const char *bach_input2_select[]  = {"ADC1 8k", "ADC1 16k", "ADC1 32k", "Mixer"};
static const char *bach_adc_select[]     = {"Line-in 0", "Mic-in"};
static const char *bach_adc2_select[]    = {"Line-in 0", "Line-in 1"};

static const struct soc_enum bach_outsel_enum =
  SOC_ENUM_SINGLE(AUD_ASRC_MUX1, 0, 4, bach_output_select);
static const struct soc_enum bach_outsel2_enum =
  SOC_ENUM_SINGLE(AUD_ASRC_MUX2, 0, 2, bach_output2_select);
static const struct soc_enum bach_insel_enum =
  SOC_ENUM_SINGLE(AUD_ASRCDEC_MUX11, 0, 4, bach_input_select);
static const struct soc_enum bach_insel2_enum =
  SOC_ENUM_SINGLE(AUD_ASRCDEC_MUX21, 0, 4, bach_input2_select);
static const struct soc_enum bach_adcsel_enum =
  SOC_ENUM_SINGLE(AUD_ADC_MUX1, 0, 2, bach_adc_select);
static const struct soc_enum bach_adcsel2_enum =
  SOC_ENUM_SINGLE(AUD_ADC_MUX2, 0, 2, bach_adc2_select);

static const struct snd_kcontrol_new bach_output_mux_controls =
  SOC_DAPM_ENUM("Output Select", bach_outsel_enum);
static const struct snd_kcontrol_new bach_output_mux2_controls =
  SOC_DAPM_ENUM("Output2 Select", bach_outsel2_enum);
static const struct snd_kcontrol_new bach_capture_mux_controls =
  SOC_DAPM_ENUM("Input Select", bach_insel_enum);
static const struct snd_kcontrol_new bach_capture_mux2_controls =
  SOC_DAPM_ENUM("Input Select", bach_insel2_enum);
static const struct snd_kcontrol_new bach_adc_mux_controls =
  SOC_DAPM_ENUM("ADC Select", bach_adcsel_enum);
static const struct snd_kcontrol_new bach_adc2_mux_controls =
  SOC_DAPM_ENUM("ADC2 Select", bach_adcsel2_enum);

/* Output Mixer */
static const struct snd_kcontrol_new bach_output1_mixer_controls[] =
{
  SOC_DAPM_SINGLE("Main Playback Switch", AUD_OUT_MIXER1, 0, 1, 0),
  SOC_DAPM_SINGLE("Sub Playback Switch",  AUD_OUT_MIXER1, 1, 1, 0),
};
static const struct snd_kcontrol_new bach_output2_mixer_controls[] =
{
  SOC_DAPM_SINGLE("Main Playback Switch", AUD_OUT_MIXER2, 0, 1, 0),
  SOC_DAPM_SINGLE("Sub Playback Switch",  AUD_OUT_MIXER2, 1, 1, 0),
};
static const struct snd_kcontrol_new bach_mixer11_controls[] =
{
  SOC_DAPM_SINGLE("Main Playback Switch", AUD_ASRC_MIXER11, 0, 1, 0),
  SOC_DAPM_SINGLE("Sub Playback Switch",  AUD_ASRC_MIXER11, 1, 1, 0),
};
static const struct snd_kcontrol_new bach_mixer12_controls[] =
{
  SOC_DAPM_SINGLE("Main Playback Switch", AUD_ASRC_MIXER12, 0, 1, 0),
  SOC_DAPM_SINGLE("Sub Playback Switch",  AUD_ASRC_MIXER12, 1, 1, 0),
};
static const struct snd_kcontrol_new bach_mixer21_controls[] =
{
  SOC_DAPM_SINGLE("Main Playback Switch", AUD_ASRC_MIXER21, 0, 1, 0),
  SOC_DAPM_SINGLE("Sub Playback Switch",  AUD_ASRC_MIXER21, 1, 1, 0),
};
static const struct snd_kcontrol_new bach_mixer22_controls[] =
{
  SOC_DAPM_SINGLE("Main Playback Switch", AUD_ASRC_MIXER22, 0, 1, 0),
  SOC_DAPM_SINGLE("Sub Playback Switch",  AUD_ASRC_MIXER22, 1, 1, 0),
};

static const struct snd_soc_dapm_widget bach_dapm_widgets[] =
{

  SND_SOC_DAPM_MIXER("Line Out 1 Mixer", SND_SOC_NOPM, 0, 0,
  &bach_output1_mixer_controls[0], ARRAY_SIZE(bach_output1_mixer_controls)),

  SND_SOC_DAPM_MIXER("Line Out 2 Mixer", SND_SOC_NOPM, 0, 0,
  &bach_output2_mixer_controls[0], ARRAY_SIZE(bach_output2_mixer_controls)),

  SND_SOC_DAPM_MUX("Main Playback Mux", SND_SOC_NOPM, 0, 0, &bach_output_mux_controls),
  SND_SOC_DAPM_MUX("Sub Playback Mux", SND_SOC_NOPM, 0, 0, &bach_output_mux2_controls),

  SND_SOC_DAPM_OUTPUT("LINEOUT0"),
  SND_SOC_DAPM_OUTPUT("LINEOUT1"),
  SND_SOC_DAPM_OUTPUT("HPOUT"),
  SND_SOC_DAPM_OUTPUT("CODECI2S"),
  SND_SOC_DAPM_OUTPUT("SPDIFHDMI"),

  SND_SOC_DAPM_AIF_OUT("DMAWR1", "Sub Capture",  0, SND_SOC_NOPM, 0, 0),
  SND_SOC_DAPM_AIF_OUT("DMAWR2", "Main Capture",   0, SND_SOC_NOPM, 0, 0),
  //SND_SOC_DAPM_AIF_IN("DMARD1",  "Main Playback", 0, SND_SOC_NOPM, 0, 0),
  //SND_SOC_DAPM_AIF_IN("DMARD2",  "Sub Playback",  0, SND_SOC_NOPM, 0, 0),

  SND_SOC_DAPM_INPUT("DMARD2"),
  SND_SOC_DAPM_INPUT("DMARD1"),
  SND_SOC_DAPM_INPUT("BTRX"),

  SND_SOC_DAPM_DAC("DAC0", NULL, AUD_ATOP_PWR, 0, 0),
  SND_SOC_DAPM_DAC("DAC1", NULL, AUD_ATOP_PWR, 1, 0),
  SND_SOC_DAPM_ADC("ADC0", NULL, AUD_ATOP_PWR, 3, 0),
  SND_SOC_DAPM_ADC("ADC1", NULL, AUD_ATOP_PWR, 4, 0),
  //SND_SOC_DAPM_MICBIAS("Mic Bias", AUD_ATOP_PWR, 5, 0),

  SND_SOC_DAPM_PGA("Hp Amp",             AUD_ATOP_PWR, 2, 0, NULL, 0),
  SND_SOC_DAPM_PGA("Main Playback DPGA", AUD_DPGA_PWR, 0, 0, NULL, 0),
  SND_SOC_DAPM_PGA("Sub Playback DPGA",  AUD_DPGA_PWR, 1, 0, NULL, 0),
  SND_SOC_DAPM_PGA("Main Capture DPGA",  AUD_DPGA_PWR, 2, 0, NULL, 0),
  SND_SOC_DAPM_PGA("Sub Capture DPGA",   AUD_DPGA_PWR, 3, 0, NULL, 0),

  SND_SOC_DAPM_INPUT("LINEIN0"),
  SND_SOC_DAPM_INPUT("LINEIN1"),
  SND_SOC_DAPM_INPUT("DBBRX"),
  SND_SOC_DAPM_INPUT("MICIN"),

  SND_SOC_DAPM_MIXER("Mixer11", SND_SOC_NOPM, 0, 0,
  &bach_mixer11_controls[0], ARRAY_SIZE(bach_mixer11_controls)),
  SND_SOC_DAPM_MIXER("Mixer12", SND_SOC_NOPM, 0, 0,
  &bach_mixer12_controls[0], ARRAY_SIZE(bach_mixer12_controls)),
  SND_SOC_DAPM_MIXER("Mixer21", SND_SOC_NOPM, 0, 0,
  &bach_mixer21_controls[0], ARRAY_SIZE(bach_mixer21_controls)),
  SND_SOC_DAPM_MIXER("Mixer22", SND_SOC_NOPM, 0, 0,
  &bach_mixer22_controls[0], ARRAY_SIZE(bach_mixer22_controls)),

  SND_SOC_DAPM_MUX("Main Capture Mux", SND_SOC_NOPM, 0, 0, &bach_capture_mux_controls),
  SND_SOC_DAPM_MUX("Sub Capture Mux" , SND_SOC_NOPM, 0, 0, &bach_capture_mux2_controls),
  SND_SOC_DAPM_MUX("ADC0 Mux", SND_SOC_NOPM, 0, 0, &bach_adc_mux_controls),
  SND_SOC_DAPM_MUX("ADC1 Mux", SND_SOC_NOPM, 0, 0, &bach_adc2_mux_controls),

};

static const struct snd_soc_dapm_route bach_codec_routes[] =
{

  {"Main Playback Mux", "DMA1 Reader", "DMARD1"},
  {"Main Playback Mux", "BT Rx",       "BTRX"},
  {"Main Playback Mux", "Line In",     "Sub Capture Mux"},
  {"Main Playback Mux", "DBB Rx",      "DBBRX"},

  {"Sub Playback Mux", "DMA2 Reader", "DMARD2"},
  {"Sub Playback Mux", "DMA1 Reader", "DMARD1"},

  {"Main Playback DPGA", NULL, "Main Playback Mux"},
  {"Sub Playback DPGA",  NULL, "Sub Playback Mux"},

  /* line out mixer */
  {"Line Out 1 Mixer", "Main Playback Switch", "Main Playback DPGA"},
  {"Line Out 1 Mixer", "Sub Playback Switch",  "Sub Playback DPGA"},

  {"Line Out 2 Mixer", "Main Playback Switch", "Main Playback DPGA"},
  {"Line Out 2 Mixer", "Sub Playback Switch",  "Sub Playback DPGA"},

  /* outputs */
  {"DAC0",     NULL, "Line Out 1 Mixer"},
  {"DAC1",     NULL, "Line Out 2 Mixer"},
  {"Hp Amp",   NULL, "Line Out 1 Mixer"},
  //{"Mic Bias", NULL, "Line Out 1 Mixer"},

  {"LINEOUT0", NULL, "DAC0"},
  {"LINEOUT1", NULL, "DAC1"},
  //{"Hp Amp",   NULL, "DAC0"},
  {"HPOUT",    NULL, "Hp Amp"},
  //{"HPOUT",    NULL, "Mic Bias"},

  /* output interface mixer */
  {"Mixer12", "Main Playback Switch", "Main Playback DPGA"},
  {"Mixer12", "Sub Playback Switch",  "Sub Playback DPGA"},
  {"Mixer22", "Main Playback Switch", "Main Playback DPGA"},
  {"Mixer22", "Sub Playback Switch",  "Sub Playback DPGA"},

  {"CODECI2S", NULL, "Mixer12"},
  {"SPDIFHDMI", NULL, "Mixer22"},

  /* capture mixer */
  {"Mixer11", "Main Playback Switch", "Main Playback DPGA"},
  {"Mixer11", "Sub Playback Switch",  "Sub Playback DPGA"},
  {"Mixer21", "Main Playback Switch", "Main Playback DPGA"},
  {"Mixer21", "Sub Playback Switch",  "Sub Playback DPGA"},

  {"Main Capture Mux", "ADC0 8k",    "ADC0"},
  {"Main Capture Mux", "ADC0 16k",   "ADC0"},
  {"Main Capture Mux", "ADC0 32k",   "ADC0"},
  {"Main Capture Mux", "Mixer",     "Mixer11"},

  {"Sub Capture Mux", "ADC1 8k",   "ADC1"},
  {"Sub Capture Mux", "ADC1 16k",  "ADC1"},
  {"Sub Capture Mux", "ADC1 32k",  "ADC1"},
  {"Sub Capture Mux", "Mixer",     "Mixer21"},

  {"ADC0 Mux", "Line-in 0", "LINEIN0"},
  {"ADC0 Mux", "Mic-in",    "MICIN"},

  {"ADC1 Mux", "Line-in 0", "LINEIN0"},
  {"ADC1 Mux", "Line-in 1", "LINEIN1"},

  {"ADC0", NULL, "ADC0 Mux"},
  {"ADC1", NULL, "ADC1 Mux"},

  /* inputs */
  {"Main Capture DPGA", NULL, "Main Capture Mux"},
  {"Sub Capture DPGA", NULL, "Sub Capture Mux"},

  {"DMAWR1", NULL, "Sub Capture DPGA"},
  {"DMAWR2", NULL, "Main Capture DPGA"},

};

static struct snd_soc_codec_driver bach_soc_codec_drv =
{
  .probe =    bach_soc_codec_probe,
  .remove =   bach_soc_codec_remove,
  .suspend =  bach_soc_codec_suspend,
  .resume =   bach_soc_codec_resume,

  //.idle_bias_off = TRUE,

  .write = bach_codec_write,
  .read  = bach_codec_read,
  //.reg_cache_size = sizeof(codec_reg);
  //.reg_word_size = 1,
  .dapm_widgets = bach_dapm_widgets,
  .num_dapm_widgets = ARRAY_SIZE(bach_dapm_widgets),
  .dapm_routes = bach_codec_routes,
  .num_dapm_routes = ARRAY_SIZE(bach_codec_routes),
  .controls =	bach_snd_controls,
  .num_controls = ARRAY_SIZE(bach_snd_controls),
  //.reg_cache_size = WM8994_MAX_REGISTER,
  //.volatile_register = wm8994_soc_volatile,
};

static int bach_codec_probe(struct platform_device *pdev)
{
  u32 val;
	int ret;
  struct device_node *node = pdev->dev.of_node;

  AUD_PRINTF(TRACE_LEVEL, "%s enter\r\n", __FUNCTION__);

	ret = of_property_read_u32(node, "playback-volume-level", &val);
	if (ret == 0)
	{
    AUD_PRINTF(TRACE_LEVEL, "Get playback-volume-level = %d\n", val);
    codec_reg_backup[AUD_ASRC1_DPGA] = val;
    codec_reg_backup[AUD_ASRC2_DPGA] = val;
	}

	ret = of_property_read_u32(node, "capture-volume-level", &val);
	if (ret == 0)
	{
    AUD_PRINTF(TRACE_LEVEL, "Get capture-volume-level = %d\n", val);
    codec_reg_backup[AUD_DMAWR12_DPGA] = val;
    codec_reg_backup[AUD_DMAWR34_DPGA] = val;
	}

	ret = of_property_read_u32(node, "micin-gain-level", &val);
	if (ret == 0)
	{
    AUD_PRINTF(TRACE_LEVEL, "Get micin-gain-level = %d\n", val);
    codec_reg_backup[AUD_MIC_GAIN] = val;
	}

	ret = of_property_read_u32(node, "linein-gain-level", &val);
	if (ret == 0)
	{
    AUD_PRINTF(TRACE_LEVEL, "Get linein-gain-level = %d\n", val);
    codec_reg_backup[AUD_ADC0_GAIN] = val;
    codec_reg_backup[AUD_ADC1_GAIN] = val;
	}

	ret = of_property_read_u32(node, "lineout-gain-level", &val);
	if (ret == 0)
	{
    AUD_PRINTF(TRACE_LEVEL, "Get lineout-gain-level = %d\n", val);
    codec_reg_backup[AUD_LINEOUT1_GAIN] = val;
    codec_reg_backup[AUD_LINEOUT2_GAIN] = val;
	}

  return snd_soc_register_codec(&pdev->dev, &bach_soc_codec_drv,
                                bach_soc_codec_dai_drv, ARRAY_SIZE(bach_soc_codec_dai_drv));
}

static int bach_codec_remove(struct platform_device *pdev)
{
  AUD_PRINTF(TRACE_LEVEL, "%s enter\r\n", __FUNCTION__);
  snd_soc_unregister_codec(&pdev->dev);
  return 0;
}

static struct platform_driver bach_codec_driver =
{
  .probe = bach_codec_probe,
  .remove = bach_codec_remove,

  .driver = {
    .name = "bach-codec",
    .owner = THIS_MODULE,
  },
};

#ifndef CONFIG_OF
module_platform_driver(bach_codec_driver);
#else
static struct platform_device *bach_codec_device = NULL;
static int __init bach_codec_init(void)
{

  int ret = 0;

  struct device_node *np;

  AUD_PRINTF(TRACE_LEVEL, "%s\r\n", __FUNCTION__)

  bach_codec_device = platform_device_alloc("bach-codec", -1);
  if (!bach_codec_device)
  {
    AUD_PRINTF(ERROR_LEVEL, "%s: platform_device_alloc bach-codec error\r\n", __FUNCTION__);
    return -ENOMEM;
  }

  np = of_find_compatible_node(NULL, NULL, "mstar,bach-audio");
  if (np)
  {
    AUD_PRINTF(TRACE_LEVEL, "Find sound dts node\n");
    bach_codec_device->dev.of_node = of_node_get(np);
    of_node_put(np);
  }

  ret = platform_device_add(bach_codec_device);
  if (ret)
  {
    AUD_PRINTF(ERROR_LEVEL, "%s: platform_device_add bach_codec_device error\r\n", __FUNCTION__);
    platform_device_put(bach_codec_device);
    return ret;
  }

  ret = platform_driver_register(&bach_codec_driver);
  if (ret)
  {
    AUD_PRINTF(ERROR_LEVEL, "%s: platform_driver_register bach_codec_driver error\r\n", __FUNCTION__);
    platform_device_unregister(bach_codec_device);
    return ret;
  }

  return ret;
}

static void __exit bach_codec_exit(void)
{
  AUD_PRINTF(TRACE_LEVEL, "%s\r\n", __FUNCTION__);
  platform_device_unregister(bach_codec_device);
  platform_driver_unregister(&bach_codec_driver);
}

module_init(bach_codec_init);
module_exit(bach_codec_exit);

#endif

/* Module information */
MODULE_AUTHOR("Roger Lai, roger.lai@mstarsemi.com");
MODULE_DESCRIPTION("Cedric Bach Audio ALSA SoC Codec");

