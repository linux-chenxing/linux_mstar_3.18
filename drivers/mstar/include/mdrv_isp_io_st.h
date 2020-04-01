#ifndef _MDRV_ISP_IO_ST_H
#define _MDRV_ISP_IO_ST_H

#define CLK_ID_ISP 0
#define CLK_ID_SR 1
#define CLK_ID_SR_MCLK 2
#define CLK_ID_CSI_MAC 3
typedef struct
{
    unsigned int id;
    unsigned int  rate;
    unsigned int enable;
}isp_ioctl_clock_ctl;

//// for ioctl IOCTL_ISP_IQ_YUVCCM, IOCTL_ISP_IQ_RGBCCM ////
typedef struct
{
  s16 ccm[9];
} __attribute__((packed, aligned(1))) isp_ioctl_ccm_coeff;


//// for ioctl ioctl IOCTL_ISP_GET_ISP_FLAG ////
#define ISP_FLAG_ENABLE_ROT 0x01
#define ISP_FLAG_ENABLE_DNR 0x02
typedef struct
{
  u32 flag;
} __attribute__((packed, aligned(1))) isp_ioctl_isp_flag;

#endif
