#include <pub/drv_isp_ioctls.h>
#include <inc/drv_isp_ctrl_dispatch.h>
#include <isp_os_wrapper/pub/isp_os_wrapper.h>
//#define copy_from_user(dest,src,size) memcpy(dest,src,size)
//#define MAX_ISP_CH 4
extern IspRegShd_t *g_ptIspRegShds[MAX_ISP_CH];

static long isp_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
    isp_dev_data *data =  (isp_dev_data*) fp->private_data;
    if (_IOC_TYPE(cmd) != ISP_IOCTL_MAGIC) return -ENOTTY;
    switch(cmd)
    {
        case IOCTL_ISP_OPEN:
        {
            DrvIspCtllDispatch(eDrvIspCtl_Open,NULL,0);
        }break;
        case IOCTL_ISP_CLOSE
        {
            DrvIspCtllDispatch(eDrvIspCtl_Close,NULL,0);
        }break;
        case IOCTL_ISP_APPLY_SHADOW_ALL
        {
            IspCfgRegShadow_t tParam
            if(copy_from_user((void*)&tParam, (void*)arg, sizeof(tParam)))
            {
                BUG();
            }
            DrvIspCtllDispatch( eDrvIspCtl_ApplyShadow,
                                &pParam,
                                tParam.nSize
                              );
        }break;
    }
    return 0;
}
