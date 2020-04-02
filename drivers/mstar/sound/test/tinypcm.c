#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>

#define AUDIO_IOCTL_MAGIC               'S'
#define AUDRV_PCM_TEST _IOW(AUDIO_IOCTL_MAGIC, 0x0, struct AUD_PcmCfg_s)
#define AUDRV_ADC_MUX  _IOW(AUDIO_IOCTL_MAGIC, 0x1,  int)
#define AUDRV_LINEIN_GAIN  _IOW(AUDIO_IOCTL_MAGIC, 0x2,  int)
#define AUDRV_MIC_GAIN  _IOW(AUDIO_IOCTL_MAGIC, 0x3,  int)
#define AUDRV_MICPRE_GAIN  _IOW(AUDIO_IOCTL_MAGIC, 0x4,  int)

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

int pcm_close(int fd)
{
    if(fd)
        close(fd);
    return 0;
}


int pcm_open(int dev, int flag, int *pFd)
{
    char fn[256];
    int card=0;
    int fd;

    snprintf(fn, sizeof(fn), "/dev/pcmC%uD%u%c", card, dev, (flag?'p':'c'));

    fd = open(fn, O_RDWR);
    if (fd < 0)
    {
        printf("open %s failed\n",fn);
        return -1;
    }

    *pFd = fd;

    return 0;
}

int pcm_ioctl(int fd, struct AUD_PcmCfg_s *pInfo)
{
    if (ioctl(fd, AUDRV_PCM_TEST, pInfo))
    {
        printf("cannot ioctl test info");
        return -1;
    }
    return 0;
}

int main(int argc, char **argv)
{
    unsigned int device = 0;
    unsigned int period_size = 1024;
    unsigned int channels = 2;
    unsigned int rate = 48000;
    unsigned int bits = 16;
    unsigned int interleaved = 1;
    unsigned int time = 10000;
    unsigned int mode = 1; //0:cap, 1:play
    unsigned int nI2sTdmMode = 0;
    unsigned int nI2sMsMode = 0;
    unsigned int nI2sBitWidth = 0;
    unsigned int nI2sFmt = 0;
    unsigned int nI2sChannels = 2;
    unsigned int nI2sConfig = 0;
    struct AUD_PcmCfg_s sPcmCfg;
    int fd;
    int nSel;

    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s [-d device] [-c channels] [-r rate] "
                "[-b bits] [-p period_size] [-i interleaved(0,1)] [-t time(ms)] "
                "[-m cap/play mode(0/1)] [-T TDM mode(off:0/on:1)] "
                "[-M master/slave mode(0,1)] [-B I2s bitwidth mode(16:0, 24:1, 32:2)] "
                "[-F enc format(0:I2s,1:left-just)] [-C I2s Channels]\n\n", argv[0]);
        fprintf(stderr, "Usage: %s [-adc sel]\n\n", argv[0]);
        fprintf(stderr, "Usage: %s [-pre pregain]\n\n", argv[0]);
        fprintf(stderr, "Usage: %s [-mgain mic gain]\n\n", argv[0]);
        fprintf(stderr, "Usage: %s [-lgain linein gain]\n\n", argv[0]);
        return 1;
    }
    else if(argc==3)
    {
        argv ++;

        if (strcmp(*argv, "-adc") == 0)
        {
            argv++;
            if (*argv)
            {
                nSel = atoi(*argv);
                pcm_open(device,0,&fd);
                if (ioctl(fd, AUDRV_ADC_MUX, &nSel))
                {
                    printf("cannot ioctl AUDRV_ADC_MUX");
                    return -1;
                }
                pcm_close(fd);
            }
        }
        else if (strcmp(*argv, "-pre") == 0)
        {
            argv++;
            if (*argv)
            {
                nSel = atoi(*argv);
                pcm_open(device,0,&fd);
                if (ioctl(fd, AUDRV_MICPRE_GAIN, &nSel))
                {
                    printf("cannot ioctl AUDRV_MICPRE_GAIN");
                    return -1;
                }
                pcm_close(fd);
            }
        }
        else if (strcmp(*argv, "-mgain") == 0)
        {
            argv++;
            if (*argv)
            {
                nSel = atoi(*argv);
                pcm_open(device,0,&fd);
                if (ioctl(fd, AUDRV_MIC_GAIN, &nSel))
                {
                    printf("cannot ioctl AUDRV_MIC_GAIN");
                    return -1;
                }
                pcm_close(fd);
            }
        }
        else if (strcmp(*argv, "-lgain") == 0)
        {
            argv++;
            if (*argv)
            {
                nSel = atoi(*argv);
                pcm_open(device,0,&fd);
                if (ioctl(fd, AUDRV_LINEIN_GAIN, &nSel))
                {
                    printf("cannot ioctl AUDRV_LINEIN_GAIN");
                    return -1;
                }
                pcm_close(fd);
            }
        }
        else
        {
            fprintf(stderr, "Usage: %s [-adc sel]\n\n", argv[0]);
            fprintf(stderr, "Usage: %s [-pre pregain]\n\n", argv[0]);
            fprintf(stderr, "Usage: %s [-mgain mic gain]\n\n", argv[0]);
            fprintf(stderr, "Usage: %s [-lgain linein gain]\n\n", argv[0]);
            return 1;
        }

        return 1;
    }

    memset(&sPcmCfg,0,sizeof(sPcmCfg));

    /* parse command line arguments */
    argv ++;
    while (*argv)
    {
        if (strcmp(*argv, "-d") == 0)
        {
            argv++;
            if (*argv)
                device = atoi(*argv);
        }
        else if (strcmp(*argv, "-p") == 0)
        {
            argv++;
            if (*argv)
                period_size = atoi(*argv);
        }
        else if (strcmp(*argv, "-c") == 0)
        {
            argv++;
            if (*argv)
                channels = atoi(*argv);
        }
        else if (strcmp(*argv, "-r") == 0)
        {
            argv++;
            if (*argv)
                rate = atoi(*argv);
        }
        else if (strcmp(*argv, "-b") == 0)
        {
            argv++;
            if (*argv)
                bits = atoi(*argv);
        }
        else if (strcmp(*argv, "-i") == 0)
        {
            argv++;
            if (*argv)
                interleaved = atoi(*argv);
        }
        else if (strcmp(*argv, "-t") == 0)
        {
            argv++;
            if (*argv)
                time = atoi(*argv);
        }
        else if (strcmp(*argv, "-m") == 0)
        {
            argv++;
            if (*argv)
                mode = atoi(*argv);
        }
        else if (strcmp(*argv, "-T") == 0)
        {
            argv++;
            if (*argv)
            {
                nI2sTdmMode = atoi(*argv);
                nI2sConfig = 1;
            }
        }
        else if (strcmp(*argv, "-M") == 0)
        {
            argv++;
            if (*argv)
            {
                nI2sMsMode = atoi(*argv);
                nI2sConfig = 1;
            }
        }
        else if (strcmp(*argv, "-B") == 0)
        {
            argv++;
            if (*argv)
            {
                nI2sBitWidth = atoi(*argv);
                nI2sConfig = 1;
            }
        }
        else if (strcmp(*argv, "-F") == 0)
        {
            argv++;
            if (*argv)
            {
                nI2sFmt = atoi(*argv);
                nI2sConfig = 1;
            }
        }
        else if (strcmp(*argv, "-C") == 0)
        {
            argv++;
            if (*argv)
            {
                nI2sChannels = atoi(*argv);
                nI2sConfig = 1;
            }
        }

        if (*argv)
            argv++;
    }

    sPcmCfg.nRate = rate;
    sPcmCfg.nBitWidth = bits;
    sPcmCfg.n16Channels = channels;
    sPcmCfg.nInterleaved = interleaved;
    sPcmCfg.n32PeriodSize = period_size;
    sPcmCfg.n32StartThres = 1<<30;
    sPcmCfg.nTimeMs = time;
    sPcmCfg.nI2sConfig = nI2sConfig;

    if(nI2sConfig)
    {
        sPcmCfg.sI2s.nTdmMode = nI2sTdmMode;
        sPcmCfg.sI2s.nMsMode = nI2sMsMode;
        sPcmCfg.sI2s.nBitWidth = nI2sBitWidth;
        sPcmCfg.sI2s.nFmt = nI2sFmt;
        sPcmCfg.sI2s.u16Channels = nI2sChannels;
    }

    pcm_open(device,mode,&fd);

    pcm_ioctl(fd, &sPcmCfg);

    pcm_close(fd);
    return 0;
}