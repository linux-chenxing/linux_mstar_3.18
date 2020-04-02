#ifndef DRV_ISP_IOCTLS_H
#define DRV_ISP_IOCTLS_H

#include <drv_isp_shadow.h>

/* IOCTL_ISP_XXXX will be used for Linux and RTK*/
#define ISP_IOCTL_MAGIC 'I'
#define IOCTL_ISP_OPEN              _IO(ISP_IOCTL_MAGIC, 0x1)
#define IOCTL_ISP_CLOSE             _IO(ISP_IOCTL_MAGIC, 0x2)
#define IOCTL_ISP_APPLY_SHADOW_ALL  _IO(ISP_IOCTL_MAGIC, 0x3)
#define IOCTL_ISP_NEW_SHADOW        _IO(ISP_IOCTL_MAGIC, 0x4)
#define IOCTL_ISP_DEL_SHADOW        _IO(ISP_IOCTL_MAGIC, 0x5)
#define IOCTL_ISP_NEW_STATIS_BUFFER _IO(ISP_IOCTL_MAGIC, 0x6)
#define IOCTL_ISP_DEL_STATIS_BUFFER _IO(ISP_IOCTL_MAGIC, 0x7)
#define IOCTL_ISP_CAPTURE_OUTPUT    _IO(ISP_IOCTL_MAGIC, 0x8)

/*Apply register shaodw to HW*/
typedef struct
{
    uint32_t  nSize;   //size of this struct
    uint32_t  nShdID;  //Register shadow ID
    IQ_SHAD_ENUM  nBlkID;  //Which ISP IP need to apply
}IspRegShadowApply_t;

/* Raw capture control*/
typedef struct
{
    u32 nX;
    u32 nY;
    u32 nH;
    u32 nW;
    u32 nBufMiuAddr;
}IspRawCaptureCfg_t;
#endif
