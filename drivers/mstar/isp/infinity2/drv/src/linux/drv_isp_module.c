#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/irq.h>
#include <linux/delay.h>
#include <linux/types.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/mutex.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/hrtimer.h>
#include <linux/input.h>
#include <linux/debugfs.h>
#include <linux/slab.h>
#include <linux/stringify.h>
#include <linux/kernel.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/fs.h>
#include <linux/kobject.h>
#include <linux/dcache.h>
#include <linux/namei.h>
#include <linux/sysfs.h>
#include <linux/kernel.h>

#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/io.h>

#include <cedric/irqs.h>
#include <mdrv_msys_io_st.h>

#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/clk.h>
#include <linux/miscdevice.h>
#include <linux/mod_devicetable.h>
#include <drv_isp_ioctls.h>
//#include <drv_isp_ctrl_dispatch.h>
#include <hal_isp_shadow.h>
#include <drv_isp.h>
#include <irqs.h>
#include <ms_msys.h>

ISP_DRV_HANDLE gIspDrvHandle = NULL;

#if 0
#define MAX_ISP_CH 4
typedef struct
{
    uint32_t nStatus; // 0:available ,1: occupied
    IspRegShd_t *ptIspRegsShd;
};
IspRegShd_t *g_ptIspRegShds[MAX_ISP_CH];
#endif

#define MAJOR_ISP_NUM               234
#define MINOR_ISP_NUM               128
#define MINOR_CSI_NUM               127

static int isp_probe(struct platform_device* pdev);
static int isp_suspend(struct platform_device *pdev, pm_message_t state);
static int isp_resume(struct platform_device *pdev);
static int isp_remove(struct platform_device* pdev);
static int isp_open(struct inode *inode, struct file *fp);
static int isp_release(struct inode *inode, struct file *fp);
static long isp_ioctl(struct file *fp, unsigned int cmd, unsigned long arg);
static unsigned int isp_poll(struct file *filp, poll_table *wait);

static ssize_t isp_fread(struct file *fp, char __user *buf, size_t size, loff_t *ppos);
static ssize_t isp_fwrite(struct file *fp,const char __user *buf, size_t size, loff_t *ppos);

static struct of_device_id isp_dt_ids[] = {
    {.compatible = "isp"},
    {}
};
MODULE_DEVICE_TABLE(of, isp_dt_ids);

static struct platform_device *isp_device;
static struct platform_driver isp_driver =
{
        .probe   = isp_probe,
        .remove = isp_remove,
        .suspend = isp_suspend,
        .resume  = isp_resume,
        .driver  = {
                .name = "isp",
                .owner = THIS_MODULE,
                .of_match_table = of_match_ptr(isp_dt_ids),
        },
};

struct file_operations isp_fops =
{
    .owner = THIS_MODULE,
    .open = isp_open,
    .release = isp_release,
    .read = isp_fread,
    .write = isp_fwrite,
    .unlocked_ioctl = isp_ioctl,
    .poll = isp_poll,
};

typedef struct _isp_dev_data
{
    struct miscdevice isp_dev;
    struct device * sysfs_dev; //for node /sys/class/mstar/
    //struct clk *clk_isp;                 //isp clock
    //struct clk *clk_sr;                  //pclk
    //struct clk *clk_sr_mclk;        //sensor mclk
    //struct clk *clk_csi_mac;
    //ISP_INT_DATA    *isp_int;
    //ISP_MEM_BUFF    *isp_mem;
    //char isp_info_str[ISP_INFO_ID_MAX][1024];
}isp_dev_data;


static int isp_open(struct inode *inode, struct file *fp)
{
    fp->private_data = isp_device->dev.platform_data;
    return 0;
}

static int isp_release(struct inode *inode, struct file *fp)
{
    fp->private_data = 0;
    return 0;
}

static ssize_t isp_fread(struct file *fp, char __user *buf, size_t size, loff_t *ppos)
{
    return 0;
}

static ssize_t isp_fwrite(struct file *fp,const char __user *buf, size_t size, loff_t *ppos)
{
    //pr_info("isp write:%s\n",buf);
    int cmd = 0;
    sscanf(buf, "%d", &cmd);
    switch(cmd)
    {
    case 1:
        pr_info("ISP ON\n");
        gIspDrvHandle = DrvIsp_Open();
        //DrvIsp_PowerOn(gIspDrvHandle);
        //DrvIsp_InitHw(gIspDrvHandle);
        break;
    case 2:
        pr_info("ISP OFF\n");
        DrvIsp_Close(gIspDrvHandle);
        gIspDrvHandle = 0;
        break;
    }
    return size;
}

unsigned int isp_poll(struct file *filp, poll_table *wait)
{
  unsigned int mask = 0;
#if 0
  poll_wait(filp, &isp_wq_epoll_event, wait);

  if(isp_int.epoll_event == true)
  {
      isp_int.epoll_event = false;
      mask |= POLLIN;
  }
#endif
  return mask;
}

static long isp_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    //isp_dev_data *data =  (isp_dev_data*) fp->private_data;
    if (_IOC_TYPE(cmd) != ISP_IOCTL_MAGIC) return -ENOTTY;
    switch(cmd)
    {
        case IOCTL_ISP_OPEN:
        {
            //DrvIspCtllDispatch(eDrvIspCtl_Open,NULL,0);
            //TODO: to initialize gIspDrvHandle
            //gIspDrvHandle = DrvIsp_Open();
            gIspDrvHandle = DrvIsp_Open();
        }break;
        case IOCTL_ISP_CLOSE:
        {
            //DrvIspCtrlDispatch(eDrvIspCtl_Close,NULL,0);
            //DrvIsp_Close(handle);
        }break;
        case IOCTL_ISP_NEW_SHADOW:
        {

        }break;
        case IOCTL_ISP_DEL_SHADOW:
        {

        }break;
        case IOCTL_ISP_APPLY_SHADOW_ALL:
        {
#if 0
            IspRegShadowApply_t tParam
            if(copy_from_user((void*)&tParam, (void __user *)arg, sizeof(tParam)))
            {
                BUG();
            }
            //DrvIspCtrlDispatch( eDrvIspCtl_ApplyShadow,
            //                    &pParam,
            //                    tParam.nSize
            //                  );
            ret = DrvIsp_ApplyShadow(handle,g_pstIspRegShds[tParam->nShdID],tParam->nBlkID);
#endif
        }break;
        case IOCTL_ISP_CAPTURE_OUTPUT:
        {
            ISP_WDMA_ATTR drv_wdma_attr;
            IspRawCaptureCfg_t tCfg;
            if(copy_from_user((void*)&tCfg, (void __user *)arg, sizeof(tCfg)))
            {
                BUG();
            }
            pr_info("ISP output wdma: W=%u , H=%u, BufAddr=0x%X\n",tCfg.nW,tCfg.nH,tCfg.nBufMiuAddr);
            drv_wdma_attr.x = tCfg.nX;
            drv_wdma_attr.y = tCfg.nY;
            drv_wdma_attr.height = tCfg.nH;
            drv_wdma_attr.width = tCfg.nW;
            drv_wdma_attr.wdma_path = ISP_WDMA_ISPSWAPYC;
            drv_wdma_attr.miu_addr = tCfg.nBufMiuAddr;

            ret = DrvIsp_WDMACapture(gIspDrvHandle, drv_wdma_attr);
        }break;
    }

    return ret;
}


#if 0
static int isp_probe(struct platform_device* pdev)
{
    //TODO: Allocate all driver resource
    int n=0;
    //allocate all reg shadows
    for(n=0;n<MAX_ISP_CH;++n)
    {
        g_ptIspRegShds[n] = kmalloc(sizeof(IspRegShd_t));
    }
}

static int isp_remove(struct platform_device* pdev)
{
    //TODO: Free all driver resource
    int n=0;
    for(n=0;n<MAX_ISP_CH;++n)
    {
        free(g_ptIspRegShds[n]);
    }
}
#endif

static int isp_suspend(struct platform_device *pdev, pm_message_t state)
{
    if (!pdev)
    {
        return -EINVAL;
        //pr_info("isp_suspend with NULL pdev %d", isp_int.frame_cnt);
    }
    return 0;
}

static int isp_resume(struct platform_device *pdev)
{
    //isp_dev_data *data;
    if(!pdev)
        return -EINVAL;
    return 0;
}

extern int DrvIspISR(void* pParam);
irqreturn_t isp_ISR(int num, void *priv)
{
    //ISP_DRV_HANDLE isp = (ISP_DRV_HANDLE*)priv;
    //pr_info("ISP ISR");
    if(!gIspDrvHandle)
        return IRQ_NONE;

    if(!DrvIspISR(gIspDrvHandle))
        return IRQ_HANDLED;
    else
        return IRQ_NONE;
}

static int isp_probe(struct platform_device* pdev)
{
    int err;
    //int ret;
    int irq = INT_IRQ_118_ISP_INT+32;
    //unsigned int i, u4IO_PHY_BASE;
    //unsigned int u4Banks[10];
    isp_dev_data *data;

    pr_info("*************  [ISP] = isp_probe  ***************\n");

    //memset(&isp_int,0,sizeof(isp_int));
    //isp_int.int_cnt = 0;

    data = kzalloc(sizeof(isp_dev_data),GFP_KERNEL);
    if(NULL == data)
        return -ENOENT;
    //ret = of_property_read_u32(pdev->dev.of_node, "io_phy_addr", &u4IO_PHY_BASE);
    //if(ret != 0)
    //    pr_err("[ISP] read node error!\n");
    //ret = of_property_read_u32_array(pdev->dev.of_node, "banks", (unsigned int*)u4Banks, 10);
    //if(ret != 0)
    //    pr_err("[ISP] read node error!\n");
    //for (i = 0; i < 10; i++) {
    //    isp_mem.pISPRegs[i] = (void*)ioremap(BANK_TO_ADDR32(u4Banks[i])+u4IO_PHY_BASE, BANK_SIZE);
    //}
    //ret = of_property_read_u32(pdev->dev.of_node, "isp-flag", &g_isp_flag);
    //if(ret != 0)
    //    pr_err("[ISP] read node error!\n");
    //ret = of_property_read_u32(pdev->dev.of_node, "isp-res", &g_isp_max_res);
    //if(ret != 0)
    //    pr_err("[ISP] read node error!\n");

    //enable clocks
    //ISP
    //data->clk_isp = of_clk_get(pdev->dev.of_node, 0);
    //clk_set_rate(data->clk_isp,123000000);
    //clk_prepare_enable(data->clk_isp);

    //data->clk_sr_mclk = of_clk_get(pdev->dev.of_node, 1);
    //clk_set_rate(data->clk_sr_mclk,12000000);
    //clk_prepare_enable(data->clk_sr_mclk);

    //data->clk_sr = of_clk_get(pdev->dev.of_node, 2);
    //clk_set_rate(data->clk_sr,86000000);
    //clk_prepare_enable(data->clk_sr);

    //data->clk_csi_mac = of_clk_get(pdev->dev.of_node, 3);

    //data->isp_int = &isp_int;
    //data->isp_mem = &isp_mem;
    //fps_init(&data->isp_int->fps);  //initial frame interval count

    // init register and disable interrupt
    //HalInitRegs((void**)isp_mem.pISPRegs);
    //HalISPDisableInt();

    //irq = irq_of_parse_and_map(pdev->dev.of_node, 0);
    pr_debug("[ISP] Request IRQ: %d\n", irq);
    //if (request_irq(irq, isp_ISR, 0, "isp interrupt", (void*)&isp_int) == 0)
    if (request_irq(irq, isp_ISR, 0, "isp interrupt", (void*)0) == 0)
        pr_debug("[ISP] isp interrupt registered\n");
    else
        pr_err("[ISP] isp interrupt failed");

    //SETREG8((u4IO_PHY_BASE+(0x112B00*2)+(0x16*4)),0x01);      //set GPIO28 to GPIO function
    //CLRREG16((u4IO_PHY_BASE+(0x101A00*2)+(0x12*4)),(0x0001<<12)); //set GPIO28 pad mux to GPIO
    //CLRREG16((u4IO_PHY_BASE+(0x102B00*2)+(0x12*4)),(0x0001<<9));  //GPIO28 OEN
    //SETREG16((u4IO_PHY_BASE+(0x102B00*2)+(0x12*4)),(0x0001<<8));  //GPIO28 pull ligh
    isp_device = pdev;

    //allocate statistics data and menload table memory
    //isp_init_buff();

    // enable interrupt
    //HalISPEnableInt();
    //isp_int.sysfs_int = true; // turn on IRQ handler
    //isp_int.yuv_ccm.dirty = 0;

    //isp_int.dgain.dirty = 0;
    //isp_int.dgain.enable = 0;
    //isp_int.dgain.gain = 1024;

    // setup kernel i2c
    data->isp_dev.minor = MISC_DYNAMIC_MINOR;
    data->isp_dev.name = "isp";
    data->isp_dev.fops = &isp_fops;
    data->isp_dev.parent = &pdev->dev;
    misc_register(&data->isp_dev);

    //AddFsNode();
    //AddAeNode();

    data->sysfs_dev = device_create(msys_get_sysfs_class(), NULL, MKDEV(MAJOR_ISP_NUM, MINOR_ISP_NUM), NULL, "isp0");
    //isp_create_bin_file(data->sysfs_dev);
    err = sysfs_create_link(&pdev->dev.parent->kobj,&data->sysfs_dev->kobj, "isp0"); //create symlink for older firmware version

    data->sysfs_dev->platform_data = pdev->dev.platform_data = (void*)data;

    return 0;
}

static int isp_remove(struct platform_device* pdev)
{
    isp_dev_data *data = dev_get_platdata(&pdev->dev);
    //clk_disable_unprepare(data->clk_isp);
    //clk_put(data->clk_isp);
    //clk_disable_unprepare(data->clk_sr);
    //clk_put(data->clk_sr);
    //clk_disable_unprepare(data->clk_sr_mclk);
    //clk_put(data->clk_sr_mclk);

    misc_deregister(&data->isp_dev);
    device_destroy(msys_get_sysfs_class(),MKDEV(MAJOR_ISP_NUM, MINOR_ISP_NUM));
    kfree(data);
    return 0;
}

static int __init mstar_isp_init(void)
{
    int ret;
    //memset((void*)&isp_int, 0, sizeof(isp_int));
    //isp_int.sysfs_int = false;
    pr_info("******** ISP driver init *************");
    ret = platform_driver_register(&isp_driver);
    if(!ret)
    {
        printk(KERN_INFO "[ISP] register driver success");
    }
    else
    {
        pr_err("[ISP] register driver fail");
        platform_driver_unregister(&isp_driver);
        return ret;
    }
#if 0
    ret = platform_driver_register(&csi_driver);
    if(!ret)
    {
        printk(KERN_INFO "[CSI] register driver success");
    }
    else
    {
        pr_err("[CSI] register driver fail");
        platform_driver_unregister(&csi_driver);
        return ret;
    }
#endif
    return 0;
}

static void __exit mstar_isp_exit(void)
{
    platform_driver_unregister(&isp_driver);
    //isp_remove_bin_file();
    pr_info("[ISP] exit");
}

module_init(mstar_isp_init);
module_exit(mstar_isp_exit);
MODULE_LICENSE("GPL");
