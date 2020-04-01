/*------------------------------------------------------------------------------
 *   Copyright (c) 2008 MStar Semiconductor, Inc.  All rights reserved.
 *------------------------------------------------------------------------------*/
//------------------------------------------------------------------------------
//  Include files
//------------------------------------------------------------------------------
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/cdev.h>


#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>

//#include <sound/driver.h>  remove it in kernel 2.6.29.1
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/jack.h>


#include <asm/mach-types.h>
#include <asm/hardware/scoop.h>
//#include <mach/hardware.h>
//#include <mach/platform.h>
#include <asm/io.h>
#include <asm/uaccess.h>


#include "ms_platform.h"

#include "infinity_pcm.h"
#include "infinity_codec.h"
#include "infinity_dai.h"
#include "bach_audio_debug.h"
#include "infinity.h"
#include "mdrv_sound_io.h"

static int infinity_soc_dai_link_init(struct snd_soc_pcm_runtime *rtd)
{
  //struct snd_soc_codec *codec = rtd->codec;
  //struct snd_soc_dapm_context *dapm = &codec->dapm;

  AUD_PRINTF(TRACE_LEVEL, "%s: rtd = %s\r\n", __FUNCTION__, rtd->dai_link->name);

  //snd_soc_dapm_disable_pin(dapm, "DBB I2S TRX");
  //snd_soc_dapm_enable_pin(dapm,  "BT I2S RX");

  return 0;
}

static int infinity_soc_dai_link_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *params)
{

  AUD_PRINTF(TRACE_LEVEL, "%s\r\n", __FUNCTION__);
/*
  struct snd_soc_pcm_runtime *rtd = substream->private_data;
  struct infinity_pcm_dma_data *dma_data;

  dma_data = snd_soc_dai_get_dma_data(rtd->codec_dai, substream);


  if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
  {
    if (BACH_DMA_READER1 == dma_data->channel)
    {
      AUD_PRINTF(TRACE_LEVEL, "%s: enable DMARD1\r\n", __FUNCTION__);
      snd_soc_dapm_enable_pin(&rtd->card->dapm, "DMARD1");
    }
    else
    {
      AUD_PRINTF(TRACE_LEVEL, "%s: enable DMARD2\r\n", __FUNCTION__);
      snd_soc_dapm_force_enable_pin(&rtd->card->dapm, "DMARD2");
    }

    snd_soc_dapm_sync(&rtd->card->dapm);
  }*/

  return 0;
}

//unsigned int BACH_BASE_ADDR = 0;

static int infinity_soc_card_probe(struct snd_soc_card *card)
{

  AUD_PRINTF(TRACE_LEVEL, "%s\r\n", __FUNCTION__);


  InfinitySetBankBaseAddr(IO_ADDRESS(BACH_RIU_BASE_ADDR));
  //BachSetBankBaseAddr(BACH_BASE_ADDR);
  AUD_PRINTF(TRACE_LEVEL, "Setup AudioSetBankBaseAddr = 0x%x\n", IO_ADDRESS(BACH_RIU_BASE_ADDR));

  return 0;
}

#if 0
static int infinity_soc_card_late_probe(struct snd_soc_card *card)
{
  snd_soc_dapm_enable_pin(&card->dapm, "DMA RD2");
  snd_soc_dapm_enable_pin(&card->dapm, "DMA RD1");
  snd_soc_dapm_enable_pin(&card->dapm, "DBB I2S TRX");
  snd_soc_dapm_enable_pin(&card->dapm, "BT I2S RX");
  snd_soc_dapm_sync(&card->dapm);

  return 0;
}
#endif

static unsigned int stream_playback_active = 0;
static unsigned int stream_capture_active = 0;

static int infinity_soc_card_suspend_pre(struct snd_soc_card *card)
{
  int i;

  struct snd_soc_dapm_widget *w;

  AUD_PRINTF(TRACE_LEVEL, "%s\r\n", __FUNCTION__);

#if 1
  for (i = 0; i < card->num_rtd; i++)
  {
    struct snd_soc_dai *codec_dai = card->rtd[i].codec_dai;

    if (card->rtd[i].dai_link->ignore_suspend)
      continue;

    w = codec_dai->playback_widget;
    if (w->active)
    {
      AUD_PRINTF(TRACE_LEVEL, "snd_soc_dapm_stream_event(): stop PLAYBACK %d\n", i);
      snd_soc_dapm_stream_event(&card->rtd[i],
                                SNDRV_PCM_STREAM_PLAYBACK,
                                SND_SOC_DAPM_STREAM_STOP);

      stream_playback_active |= 1 << i;

    }
    w = codec_dai->capture_widget;
    if (w->active)
    {
      AUD_PRINTF(TRACE_LEVEL, "snd_soc_dapm_stream_event(): stop CAPTURE %d\n", i);
      snd_soc_dapm_stream_event(&card->rtd[i],
                                SNDRV_PCM_STREAM_CAPTURE,
                                SND_SOC_DAPM_STREAM_STOP);

      stream_capture_active |= 1 << i;

    }
  }

#endif


#if 1
  snd_soc_dapm_disable_pin(&card->dapm, "DMARD");
  snd_soc_dapm_disable_pin(&card->dapm, "LINEIN");
  snd_soc_dapm_sync(&card->dapm);
#endif

  return 0;
}

static int infinity_soc_card_suspend_post(struct snd_soc_card *card)
{
  AUD_PRINTF(TRACE_LEVEL, "%s\r\n", __FUNCTION__);

  return 0;

}

static int infinity_soc_card_resume_pre(struct snd_soc_card *card)
{
  AUD_PRINTF(TRACE_LEVEL, "%s\r\n", __FUNCTION__);

#if 0
  BachSysInit();

  BachCfgInit();

  BachDpgaGainInit();

  BachSetInputPathGain(BACH_INPUT_PATH_ASRC1, (S8)(BACH_DPGA_GAIN_MIN_DB + CONFIG_MS_SOUND_SND_DEFAULT_VOLUME), BACH_CHMODE_BOTH);
  BachSetInputPathGain(BACH_INPUT_PATH_ASRC2, (S8)(BACH_DPGA_GAIN_MIN_DB + CONFIG_MS_SOUND_SND_DEFAULT_VOLUME), BACH_CHMODE_BOTH);
  BachSetRecPathGain(BACH_REC_PATH_DMA1, (S8)(BACH_DPGA_GAIN_MIN_DB + CONFIG_MS_SOUND_SND_DEFAULT_C_VOLUME));
  BachSetRecPathGain(BACH_REC_PATH_DMA2, (S8)(BACH_DPGA_GAIN_MIN_DB + CONFIG_MS_SOUND_SND_DEFAULT_C_VOLUME));

  //Initialize Atop register
  BachAtopMicGain(CONFIG_MS_SOUND_SND_DEFAULT_MIC_GAIN);
  BachAtopLineOutGain(BACH_ATOP_LINEOUT0, CONFIG_MS_SOUND_SND_DEFAULT_LINEOUT_GAIN);
  BachAtopLineOutGain(BACH_ATOP_LINEOUT1, CONFIG_MS_SOUND_SND_DEFAULT_LINEOUT_GAIN);
  BachAtopLineInGain(BACH_ATOP_LINEIN0, CONFIG_MS_SOUND_SND_DEFAULT_ADC_GAIN);
  BachAtopLineInGain(BACH_ATOP_LINEIN1, CONFIG_MS_SOUND_SND_DEFAULT_ADC_GAIN);
  //BachAtopInit();
  //BachAtopIdle();

  BachSetInputPathOnOff(BACH_INPUT_PATH_ASRC1, FALSE);
  BachSetInputPathOnOff(BACH_INPUT_PATH_ASRC2, FALSE);
  //BachOpenAtop(BACH_ATOP_EAR);
  //BachSetOutputPathOnOff(BACH_OUTPUT_PATH_DAC1, TRUE);
  //BachSetOutputPathOnOff(BACH_OUTPUT_PATH_DAC2, TRUE);
  //BachSetInputPathOnOff(BACH_INPUT_PATH_ASRC1, TRUE);
  //BachSetInputPathOnOff(BACH_INPUT_PATH_ASRC2, TRUE);

  BachSetOutputPathOnOff(BACH_OUTPUT_PATH_DMA1_WRITER, FALSE);
  BachSetOutputPathOnOff(BACH_OUTPUT_PATH_DMA2_WRITER, FALSE);

  //BachFuncEnable(BACH_FUNC_FM, TRUE);
  //BachFuncEnable(BACH_FUNC_GPS, TRUE);
  //BachFuncEnable(BACH_FUNC_TEST_RECORDER2, TRUE);
  //BachFuncEnable(BACH_FUNC_FM_RECORDER, TRUE);
  //BachOpenAtop(BACH_ATOP_LINEIN0);
  //BachOpenAtop(BACH_ATOP_LINEIN1);

  /*MUST sync to codec_reg[] */
  BachAuxSetCfg(BACH_RATE_8K, TRUE);
  BachSinkSelectChannel(BACH_SINK_DMA1_WRITER, BACH_CHAN_LINEIN);

  BachAdcSetRate(BACH_RATE_8K);
  BachSinkSelectChannel(BACH_SINK_DMA2_WRITER, BACH_CHAN_MICIN);

  BachSinkSelectChannel(BACH_SINK_CODEC_I2S,  BACH_CHAN_ASRC1);
  BachSinkSelectChannel(BACH_SINK_SPDIF_HDMI, BACH_CHAN_ASRC1);
#endif

  return 0;

}

static int infinity_soc_card_resume_post(struct snd_soc_card *card)
{
  int i;

  AUD_PRINTF(TRACE_LEVEL, "%s\n", __FUNCTION__);

#if 1
  snd_soc_dapm_enable_pin(&card->dapm, "DMARD");
  snd_soc_dapm_enable_pin(&card->dapm, "LINEIN");
  snd_soc_dapm_sync(&card->dapm);
#endif


#if 1
  for (i = 0; i < card->num_rtd; i++)
  {
    //struct snd_soc_dai *codec_dai = card->rtd[i].codec_dai;

    if (stream_playback_active & (1 << i))
    {
      AUD_PRINTF(TRACE_LEVEL, "snd_soc_dapm_stream_event(): start PLAYBACK %d\n", i);

      snd_soc_dapm_stream_event(&card->rtd[i],
                                SNDRV_PCM_STREAM_PLAYBACK,
                                SND_SOC_DAPM_STREAM_START);

      stream_playback_active &= ~(1 << i);

    }

    if (stream_capture_active & (1 << i))
    {
      AUD_PRINTF(TRACE_LEVEL, "snd_soc_dapm_stream_event(): start CAPTURE %d\n", i);
      snd_soc_dapm_stream_event(&card->rtd[i],
                                SNDRV_PCM_STREAM_CAPTURE,
                                SND_SOC_DAPM_STREAM_START);

      stream_capture_active &= ~(1 << i);

    }
  }
#endif

  return 0;

}

static struct snd_soc_ops infinity_soc_ops =
{
  .hw_params	= infinity_soc_dai_link_hw_params,
};

static struct snd_soc_dai_link infinity_soc_dais[] =
{
  {
    .name           = "Infinity Soc Dai Link",
    //.stream_name	  = "msb2501_dai_stream",
    .codec_name     = "infinity-codec",
    .codec_dai_name = "infinity-codec-dai-main",
    .cpu_dai_name   = "infinity-cpu-dai",
    .platform_name  = "infinity-platform",

    .init			= infinity_soc_dai_link_init,

    .ops			= &infinity_soc_ops,
  },
};

static const struct snd_soc_dapm_widget infinity_card_dapm_widgets[] =
{
  SND_SOC_DAPM_LINE("DMA RD1", NULL),
  //SND_SOC_DAPM_LINE("DMA RD2", NULL),
};

static const struct snd_soc_dapm_route infinity_card_dapm_routes[] =
{
  /* DMA Module */
  {"DMARD1", NULL, "DMA RD1"},
  //{"DMARD2", NULL, "DMA RD2"},

};


static struct snd_soc_card infinity_soc_card =
{
  .name       = "infinity_snd_machine",
  .owner      = THIS_MODULE,
  .dai_link	  = infinity_soc_dais,
  .num_links  = ARRAY_SIZE(infinity_soc_dais),
  //.dapm_widgets = infinity_card_dapm_widgets,
  //.num_dapm_widgets = ARRAY_SIZE(infinity_card_dapm_widgets),
  //.dapm_routes = infinity_card_dapm_routes,
  //.num_dapm_routes = ARRAY_SIZE(infinity_card_dapm_routes),
  .probe      = infinity_soc_card_probe,
  //.late_probe = infinity_soc_card_late_probe,

  .suspend_pre = infinity_soc_card_suspend_pre,
  .suspend_post = infinity_soc_card_suspend_post,
  .resume_pre = infinity_soc_card_resume_pre,
  .resume_post = infinity_soc_card_resume_post,
};


#ifndef CONFIG_OF

static struct platform_device *infinity_snd_device = NULL;
static struct platform_device *infinity_cpu_dai_device = NULL;
static struct platform_device *infinity_platform_device = NULL;
static struct platform_device *infinity_codec_device = NULL;

static int __init infinity_snd_init(void)
{
  int ret = 0;

  AUD_PRINTF(TRACE_LEVEL, "%s\r\n", __FUNCTION__);

  //----------------------------------------------------------------
  infinity_codec_device = platform_device_alloc("infinity-codec", -1);
  if (!infinity_codec_device)
  {
    AUD_PRINTF(ERROR_LEVEL, "%s: platform_device_alloc infinity-codec error\r\n", __FUNCTION__);
    return -ENOMEM;
  }
  ret = platform_device_add(infinity_codec_device);
  if (ret)
  {
    AUD_PRINTF(ERROR_LEVEL, "%s: platform_device_add infinity_codec_device error\r\n", __FUNCTION__);
    platform_device_put(infinity_codec_device);
  }

  //----------------------------------------------------------------
  infinity_platform_device = platform_device_alloc("infinity-platform", -1);
  if (!infinity_platform_device)
  {
    AUD_PRINTF(ERROR_LEVEL, "%s: platform_device_alloc infinity-platform error\r\n", __FUNCTION__);
    return -ENOMEM;
  }
  ret = platform_device_add(infinity_platform_device);
  if (ret)
  {
    AUD_PRINTF(ERROR_LEVEL, "%s: platform_device_add infinity_platform_device error\r\n", __FUNCTION__);
    platform_device_put(infinity_platform_device);
  }

  //----------------------------------------------------------------
  infinity_cpu_dai_device = platform_device_alloc("infinity-cpu-dai", -1);
  if (!infinity_cpu_dai_device)
  {
    AUD_PRINTF(ERROR_LEVEL, "%s: platform_device_alloc infinity-cpu-dai error\r\n", __FUNCTION__);
    return -ENOMEM;
  }
  ret = platform_device_add(infinity_cpu_dai_device);
  if (ret)
  {
    AUD_PRINTF(ERROR_LEVEL, "%s: platform_device_add infinity_cpu_dai_device error\r\n", __FUNCTION__);
    platform_device_put(infinity_cpu_dai_device);
  }

  //----------------------------------------------------------------
  infinity_snd_device = platform_device_alloc("soc-audio", -1);
  if (!infinity_snd_device)
  {
    AUD_PRINTF(ERROR_LEVEL, "%s: platform_device_alloc soc-audio error\r\n", __FUNCTION__);
    return -ENOMEM;
  }
  platform_set_drvdata(infinity_snd_device, &infinity_soc_card);
  ret = platform_device_add(infinity_snd_device);
  if (ret)
  {
    AUD_PRINTF(ERROR_LEVEL, "%s: platform_device_add infinity_snd_device error\r\n", __FUNCTION__);
    platform_device_put(infinity_snd_device);
  }

  return ret;
}

static void __exit infinity_snd_exit(void)
{
  AUD_PRINTF(TRACE_LEVEL, "%s\r\n", __FUNCTION__);
  platform_device_unregister(infinity_snd_device);
  platform_device_unregister(infinity_cpu_dai_device);
  platform_device_unregister(infinity_platform_device);
  platform_device_unregister(infinity_codec_device);
}

module_init(infinity_snd_init);
module_exit(infinity_snd_exit);

#else
static int infinity_audio_probe(struct platform_device *pdev)
{
  //int ret = 0;

	//struct resource *res_mem;
	//struct resource *res_irq;
  //struct device_node *np = pdev->dev.of_node;

  AUD_PRINTF(TRACE_LEVEL, "%s\r\n", __FUNCTION__);

/*
	ret = of_property_read_u32(np, "debug", &val);
	if (ret == 0)
		priv->mclk_fs = val;

  if (of_get_property(np, "fiq-merged", NULL) != NULL)
  {
    pr_info(" ms_irq_of_init->fiq-merged !\n");
    mic->fiq_merged = TRUE;

  } else
    mic->fiq_merged = FALSE;


	//res_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
  BACH_BASE_ADDR = (unsigned int)of_iomap(np, 0);
	if (BACH_BASE_ADDR)
	{
		//BACH_BASE_ADDR = (unsigned int)res_mem->start;
		AUD_PRINTF(TRACE_LEVEL, "Get resource IORESOURCE_MEM and IO mapping = 0x%x\n", (unsigned int)BACH_BASE_ADDR);
	}
  else
		return -EINVAL;
*/

  infinity_soc_card.dev = &pdev->dev;

  //return devm_snd_soc_register_card(&pdev->dev, &infinity_soc_card);

  return snd_soc_register_card(&infinity_soc_card);
}

int infinity_audio_remove(struct platform_device *pdev)
{
  AUD_PRINTF(TRACE_LEVEL, "%s\r\n", __FUNCTION__);
  return snd_soc_unregister_card(&infinity_soc_card);
}

static const struct of_device_id infinity_audio_of_match[] = {
	{ .compatible = "mstar,infinity-audio", },
	{},
};
MODULE_DEVICE_TABLE(of, infinity_audio_of_match);

static struct platform_driver infinity_audio = {
	.driver = {
		.name	= "infinity-audio",
		.owner = THIS_MODULE,
		.pm     = &snd_soc_pm_ops,
		.of_match_table = infinity_audio_of_match,
	},
	.probe		= infinity_audio_probe,
	.remove   = infinity_audio_remove,
};

module_platform_driver(infinity_audio);
#endif

#if 1
static int infinity_dma_open(struct inode *inode, struct file *filp)
{

    int ret = 0;
    struct snd_pcm_substream *substream = NULL;
    struct snd_pcm_runtime *runtime= NULL;
    //struct bach_pcm_runtime_data *prtd = NULL;
    struct snd_soc_pcm_runtime *rtd = NULL;

    AUD_PRINTF(TRACE_LEVEL, "%s\n", __FUNCTION__);

    rtd = snd_soc_get_pcm_runtime(&infinity_soc_card, infinity_soc_dais[iminor(inode)].name);
    if (!rtd)
    {
      AUD_PRINTF(ERROR_LEVEL, "%s: snd_soc_get_pcm_runtime failed = %s\n", __FUNCTION__, infinity_soc_dais[iminor(inode)].name);
      ret = -ENODEV;
    }
    else
    {
      substream = rtd->pcm->streams[SNDRV_PCM_STREAM_PLAYBACK].substream;

      if (substream)
      {
        snd_pcm_stream_lock_irq(substream);

        filp->private_data = (void *)substream;

        runtime = substream->runtime;
        if (!runtime)
        {
          AUD_PRINTF(ERROR_LEVEL, "%s: runtime NULL failed\n", __FUNCTION__);
          ret = -ENXIO;
        }

        snd_pcm_stream_unlock_irq(substream);
  /*
        prtd = runtime->private_data;
        if (prtd)
        {
          AUD_PRINTF(TRACE_LEVEL, "%s: open %s, addr = 0x%x, size = 0x%zx\n", __FUNCTION__, prtd->dma_data->name, (u32)runtime->dma_area, runtime->dma_bytes);
        }
  */
      }
      else
      {
         AUD_PRINTF(ERROR_LEVEL, "%s: substream NULL failed\n", __FUNCTION__);
         ret = -ENODEV;
      }
    }

    return ret;
}

static int infinity_dma_release(struct inode *inode, struct file *filp)
{

    AUD_PRINTF(TRACE_LEVEL, "%s\n", __FUNCTION__);

    filp->private_data = NULL;

    return 0;
}

static ssize_t infinity_dma_read(struct file *filp, char __user *buf,
      size_t count, loff_t *ppos)
{
    u32 offset, frame_size;
    u64 hw_ptr, appl_ptr;
    ssize_t ret;

    snd_pcm_sframes_t avail;

    struct snd_pcm_substream *substream = (struct snd_pcm_substream *)filp->private_data;
    struct snd_pcm_runtime *runtime;

    //AUD_PRINTF(TRACE_LEVEL, "loff = 0x%llx, f_pos = 0x%llx\n", *ppos, filp->f_pos);

    snd_pcm_stream_lock_irq(substream);

    runtime = substream->runtime;
    if (!runtime)
    {
      ret = -ENXIO;
    }
    else
    {

      if (runtime->status->state == SNDRV_PCM_STATE_RUNNING ||
          runtime->status->state == SNDRV_PCM_STATE_DRAINING)
      {

        frame_size  = runtime->frame_bits/8;
        avail = runtime->buffer_size - snd_pcm_playback_avail(runtime);
        hw_ptr = runtime->hw_ptr_wrap + runtime->status->hw_ptr;
        appl_ptr = hw_ptr + avail;

        //snd_pcm_stream_unlock_irq(substream);
        //AUD_PRINTF(TRACE_LEVEL, "appl_ptr = 0x%llx, hw_ptr = 0x%llx\n", ((hw_ptr + avail) * frame_size), (hw_ptr * frame_size));
        //snd_pcm_stream_lock_irq(substream);

        if ((appl_ptr * frame_size) <=  filp->f_pos)
        {
          //AUD_PRINTF(ERROR_LEVEL, "appl_ptr = 0x%llx, hw_ptr = 0x%llx f_pos = 0x%llx\n", ((hw_ptr + avail) * frame_size), (hw_ptr * frame_size),filp->f_pos);
          ret = 0;
        }
        else if (((appl_ptr * frame_size) - runtime->dma_bytes) > filp->f_pos)
        {
          ret = -EOVERFLOW;
        }
        else
        {
          if (count > ((appl_ptr * frame_size) - filp->f_pos))
            count = (appl_ptr * frame_size) - filp->f_pos;

          ret = count;

          div_u64_rem(filp->f_pos, runtime->dma_bytes, &offset);

          snd_pcm_stream_unlock_irq(substream);

          if (offset + count > runtime->dma_bytes)
          {
            if (copy_to_user((buf + runtime->dma_bytes - offset), runtime->dma_area, ((count + offset) - runtime->dma_bytes)))
              return -EFAULT;

            count = (runtime->dma_bytes - offset);
          }

          if (copy_to_user(buf, (runtime->dma_area + offset), count))
            return -EFAULT;

          snd_pcm_stream_lock_irq(substream);

          *ppos += ret;
        }

      }
      else
        ret = -EPERM;

    }/*!runtime*/

    snd_pcm_stream_unlock_irq(substream);

    return ret;

}

static loff_t infinity_dma_lseek(struct file *file, loff_t offset, int orig)
{
  loff_t ret;
  u32 frame_size;
  u64 hw_ptr;
  snd_pcm_sframes_t avail;
  struct snd_pcm_substream *substream = (struct snd_pcm_substream *)file->private_data;
  struct snd_pcm_runtime *runtime = substream->runtime;

  snd_pcm_stream_lock_irq(substream);

  runtime = substream->runtime;
  if (!runtime)
  {
    return -ENXIO;
  }
  else
  {

    frame_size  = runtime->frame_bits/8;
    avail = runtime->buffer_size - snd_pcm_playback_avail(runtime);
    hw_ptr = runtime->hw_ptr_wrap + runtime->status->hw_ptr;

    //mutex_lock(&file_inode(file)->i_mutex);
    switch (orig) {
    case SEEK_CUR:
      offset += file->f_pos;
    case SEEK_SET:
      file->f_pos = offset;
      ret = file->f_pos;
      break;
    case SEEK_END:
      offset += ((hw_ptr + avail) * frame_size);
      file->f_pos = offset;
      ret = file->f_pos;
      break;
/*
    case SEEK_DATA:
      offset += (hw_ptr * frame_size);
      file->f_pos = offset;
      ret = file->f_pos;
      break;
*/
    default:
      ret = -EINVAL;
    }
    //mutex_unlock(&file_inode(file)->i_mutex);
  }
  snd_pcm_stream_unlock_irq(substream);

  return ret;
}

static long infinity_dma_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int err= 0;
    AUD_PRINTF(TRACE_LEVEL, "%s 0x%x!\n",__FUNCTION__,cmd);
    // wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
    if (_IOC_TYPE(cmd) != AUDIO_IOCTL_MAGIC) return -ENOTTY;
    if (_IOC_NR(cmd) > IOCTL_AUDIO_MAXNR) return -ENOTTY;

    if (_IOC_DIR(cmd) & _IOC_READ)
    {
        err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
    }
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
    {
        err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
    }
    if (err)
    {
        return -EFAULT;
    }


    switch(cmd)
    {
        case MDRV_SOUND_STARTTIME_READ:
		{
			AUD_PRINTF(TRACE_LEVEL, "MDRV_SOUND_STARTTIME_READ!\n");
			if(copy_to_user((void __user *)arg, (void*)&g_nPlayStartTime, sizeof(unsigned long long)))
			{
				return -EFAULT;
			}

		}
		break;

        default:
            AUD_PRINTF(ERROR_LEVEL,"Unknown IOCTL Command 0x%08X\n", cmd);
            return -ENOTTY;
    }

    return err;
}

//static struct class * dma_class;

static struct cdev infinity_dma_cdev =
{
  .kobj = {.name= "infinity_dma", },
  .owner = THIS_MODULE,
};

static dev_t dma_dev;

static struct file_operations infinity_dma_fops =
{
    .open = infinity_dma_open,
    .release = infinity_dma_release,
    .read = infinity_dma_read,
    .llseek = infinity_dma_lseek,
    .unlocked_ioctl = infinity_dma_ioctl
};

static ssize_t play_time_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    char *str = buf;
    char *end = buf + PAGE_SIZE;

    str += scnprintf(str, end - str, "%llu\n",g_nPlayStartTime);

    return (str - buf);
}

DEVICE_ATTR(play_time, 0444, play_time_show, NULL);

static ssize_t cap_time_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    char *str = buf;
    char *end = buf + PAGE_SIZE;

    str += scnprintf(str, end - str, "%llu\n",g_nCapStartTime);

    return (str - buf);
}

DEVICE_ATTR(cap_time, 0444, cap_time_show, NULL);


static int __init infinity_mem_init(void)
{
  int ret = 0;
  struct device *dev;

  AUD_PRINTF(TRACE_LEVEL, "%s\r\n", __FUNCTION__);

  ret = alloc_chrdev_region(&dma_dev, 0, 1, "infinity_dma");
  if (ret) {
      AUD_PRINTF(ERROR_LEVEL,"can't alloc chrdev\n");
      return ret;
  }

  cdev_init(&infinity_dma_cdev, &infinity_dma_fops);
  if (0 != (ret= cdev_add(&infinity_dma_cdev, dma_dev, 1)))
  {
      AUD_PRINTF(ERROR_LEVEL, "%s:Unable add a character device\n", __FUNCTION__);
      unregister_chrdev_region(dma_dev, 1);
      return ret;
  }

/*
  dma_class = class_create(THIS_MODULE, "bach_dma_class");
  if(IS_ERR(dma_class))
  {
      AUD_PRINTF(ERROR_LEVEL, "%s:Failed at class_create().Please exec [mknod] before operate the device\n", __FUNCTION__);
  }
  else
*/
  {
      dev = device_create(sound_class, NULL, dma_dev, NULL, "dmaC0D0p");
  }
  ret = device_create_file(dev, &dev_attr_play_time);
  if(ret)
	  AUD_PRINTF(ERROR_LEVEL, "%s:add attibute failed\n", __FUNCTION__);

  ret = device_create_file(dev, &dev_attr_cap_time);
  if(ret)
      AUD_PRINTF(ERROR_LEVEL, "%s:add attibute failed\n", __FUNCTION__);

  return ret;
}

static void __exit infinity_mem_exit(void)
{
  AUD_PRINTF(TRACE_LEVEL, "%s\r\n", __FUNCTION__);

  cdev_del(&infinity_dma_cdev);

  unregister_chrdev_region(dma_dev, 1);
}

module_init(infinity_mem_init);
module_exit(infinity_mem_exit);
#endif


/* Module information */
MODULE_AUTHOR("Roger Lai, roger.lai@mstarsemi.com");
MODULE_DESCRIPTION("iNfinity Bach Audio ASLA SoC Machine");

