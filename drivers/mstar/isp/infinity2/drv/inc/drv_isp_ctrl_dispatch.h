#ifndef DRV_ISP_CTL_MAPPING_H
#define DRV_ISP_CTL_MAPPING_H

/* IOCTL_ISP_XXXX will be used for Linux and RTK*/
#define ISP_IOCTL_MAGIC 'I'
#define IOCTL_ISP_OPEN              _IO(ISP_IOCTL_MAGIC, 0x1)
#define IOCTL_ISP_CLOSE             _IO(ISP_IOCTL_MAGIC, 0x2)
#define IOCTL_ISP_APPLY_SHADOW_ALL  _IO(ISP_IOCTL_MAGIC, 0x3)
#define IOCTL_ISP_NEW_SHADOW        _IO(ISP_IOCTL_MAGIC, 0x4)
#define IOCTL_ISP_DEL_SHADOW        _IO(ISP_IOCTL_MAGIC, 0x5)
#define IOCTL_ISP_NEW_STATIS_BUFFER _IO(ISP_IOCTL_MAGIC, 0x6)
#define IOCTL_ISP_DEL_STATIS_BUFFER _IO(ISP_IOCTL_MAGIC, 0x7)

typedef enum
{
    eDrvIspCtl_Open,
    eDrvIspCtl_Close,
    eDrvIspCtl_PipeConfig,
    eDrvIspCtl_NewShadow,         //create new isp shadow buffer
    eDrvIspCtl_DelShadow,         //delete isp shadow buffer
    eDrvIspCtl_ApplyShadow,       //delete isp shadow buffer
    eDrvIspCtl_ApplyShadowAll,    //delete isp shadow buffer
    eDrvIspCtl_NewStatsBuffer,    //create statistics buffer
	eDrvIspCtl_DelStatsBuffer,    //delete statistics buffer
    eDrvIspCtl_ChgStatsBuffer,    //change statistics buffer
}DrvIspCtlId_e;
/*
 * sysfs list
 * mstar/isp/fe
 * mstar/isp/fs
 * mstar/isp/ae_done
 * mstar/isp/fifo_full
 * mstar/vif/snr0_fs
 * mstar/vif/snr0_fe
 * mstar/vif/snr0_cnt_done
 * mstar/vif/dma0_done
 * mstar/vif/snr1_fs
 * mstar/vif/snr1_fe
 * mstar/vif/snr1_cnt_done
 * mstar/vif/dma1_done
 * mstar/vif/snr2_fs
 * mstar/vif/snr2_fe
 * mstar/vif/snr2_cnt_done
 * mstar/vif/dma2_done
 * mstar/vif/snr3_fs
 * mstar/vif/snr3_fe
 * mstar/vif/snr3_cnt_done
 * mstar/vif/dma3_done
 */

#endif
