#include <drv_mload.h>
#include <mdrv_mload.h>
#include <mload_common.h>
#include <linux/module.h>

#if LINUX_UNIT_TEST
#include <linux/kernel.h>
#include <linux/fs.h> // required for various structures related to files liked fops.
#include <linux/cdev.h>
#include <linux/ioctl.h>
#include <linux/device.h>

static int Major;
dev_t dev_no,dev;
struct cdev *kernel_cdev;
static struct class *c1;
#endif

static int _ginit=0;

MLOAD_HANDLE IspMLoadInit(MLOAD_ATTR attr){

    MLOAD_HANDLE hnd=0;

    DrvIsp_MLoadInit(&hnd,attr);
    MloadWarperInit();

    return hnd;
}
int IspMLoadDeInit(MLOAD_HANDLE handle){

    DrvIsp_MLoadFreeBuffer(handle);

    return 0;
}

int IspMLoadTableSet(MLOAD_HANDLE handle, IspMloadID_e id,void *table)
{

    DrvIsp_MLoadTableSet(handle,id,table);
    return 0;
}

int IspMLoadTableGet(MLOAD_HANDLE handle, IspMloadID_e id,void *table)
{
    return 0;
}

int IspMLoadApply(MLOAD_HANDLE handle)
{
    DrvIsp_MLoadApply(handle);

    return 0;
}

int SclMLoadApply(MLOAD_HANDLE handle)
{
    DrvScl_MLoadApply(handle);

    return 0;
}

void IspMloadUTSet(MLOAD_HANDLE handle)
{
    int i = 0;
    static u16 *buf;

    if(_ginit ==0){

        buf = MloadMallocNonCache(4208*8,16);

        //memset(buf,0x0B,4208*8);
	     //memset(buf+12,0x0B,6);
	
	   for(i=0; i<4208*4;i++)
		    *((u8 *)buf+i)=i;
	
        MLOAD_DEBUG("6 buf=0x%x\n",(int)buf);
        _ginit=1;
    }
    //IspMLoadTableSet(handle,MLOAD_ID_PIPE0_ALSC,buf);
	//MLOAD_DEBUG("7\n");
    //IspMLoadTableSet(handle,MLOAD_ID_PIPE0_FPN,buf);
    //IspMLoadTableSet(handle,MLOAD_ID_DPC,buf);
    //IspMLoadTableSet(handle,MLOAD_ID_PIPE0_GMA16TO16,buf);
    //IspMLoadTableSet(handle,MLOAD_ID_GMA10TO10,buf);
	MLOAD_DEBUG("8\n");

    //IspMLoadApply(handle);
    MLOAD_DEBUG("9\n");

    IspMLoadTableSet(handle,SCL_MLOAD_ID_RGB12TO12_PRE,buf);
	//MLOAD_DEBUG("10\n");
    SclMLoadApply(handle);

    MLOAD_DEBUG("11\n");
    return;
}

void IspMloadUT(MHAL_CMDQ_CmdqInterface_t *cmdq,FrameSyncMode mode)
{
    //short *buf;
    static MLOAD_HANDLE handle;
    static MLOAD_ATTR attr;

    attr.mode = mode;
    attr.pCmqInterface_t = cmdq;


    MLOAD_DEBUG("!!!!!!!!!!!!!!!!!! MloadUT !!!!!!!!!!!!!!!!!!!!!!!!\n");

    if(_ginit == 0){
        handle = IspMLoadInit(attr);
        MloadRegW(cmdq,mode,0x150A,0x05,0x0003,0x3); //gamma16to16 enable
    }
	//MloadRegW(cmdq,mode,0x1502,0x01,0x0001,0x01); //Mask VIF
	//MloadRegW(cmdq,mode,0x1502,0x00,0x0801,0x01); //disable VIF0
    //MloadRegW(cmdq,mode,0x1502,0x40,0x0001,0x01); //Mask VIF
	//MloadRegW(cmdq,mode,0x1502,0x40,0x0802,0xFFFF); //disable VIF1

    IspMloadUTSet(handle);
	//IspMloadUTSet(handle);
	//IspMloadUTSet(handle);
	//IspMloadUTSet(handle);

    //MloadSleep(3000);
    //buf = MloadMalloc(4208*8);
    //MLOAD_DEBUG("!!!!!! MLOAD_ID_PIPE0_ALSC buf:0x%x\n",(int)buf);
    //DrvIsp_MLoadDump(handle,MLOAD_ID_PIPE0_ALSC,buf);

    //buf = MloadMalloc(2816*2);
    //MLOAD_DEBUG("!!!!!! MLOAD_ID_PIPE0_FPN buf:0x%x\n",(int)buf);
    //DrvIsp_MLoadDump(handle,MLOAD_ID_PIPE0_FPN,buf);


    //buf = MloadMalloc(1024*4);
    //MLOAD_DEBUG("!!!!!! MLOAD_ID_DPC buf:0x%x\n",(int)buf);
    //DrvIsp_MLoadDump(handle,MLOAD_ID_DPC,buf);


    //buf = MloadMalloc(256*8);
    //MLOAD_DEBUG("!!!!!! MLOAD_ID_PIPE0_GMA16TO16 buf:0x%x\n",(int)buf);
    //DrvIsp_MLoadDump(handle,MLOAD_ID_PIPE0_GMA16TO16,buf);


    //buf = MloadMalloc(256*8);
    //MLOAD_DEBUG("!!!!!! MLOAD_ID_GMA10TO10 buf:0x%x\n",(int)buf);
    //DrvIsp_MLoadDump(handle,MLOAD_ID_GMA10TO10,buf);



    //buf = MloadMalloc(256*8);
   // MLOAD_DEBUG("!!!!!! SCL_MLOAD_ID_RGB10TO12 buf:0x%x\n",(int)buf);
    //DrvScl_MLoadDump(handle,SCL_MLOAD_ID_RGB12TO12_PRE,buf);



    MLOAD_DEBUG("!!!!!!!!!!!!!!!!!! Finish !!!!!!!!!!!!!!!!!!!!!!!!\n");

    //(*(volatile U16*)(0x1f000000 + (0x1312 * 0x200) + (0x01 * 4))) = 0x0000; //UnMask VIF


    return;
}

#if LINUX_UNIT_TEST
static long mload_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{

    return 0;
}

int mload_open(struct inode *inode, struct file *filp)
{

    IspMloadUT(NULL,1);
    return 0;
}

int mload_release(struct inode *inode, struct file *filp)
{

    return 0;
}


struct file_operations fops = {
 .unlocked_ioctl =  mload_ioctl,
 .open           =  mload_open,
 .release        = mload_release
};


int mload_init (void)
{
    int ret;
    kernel_cdev = cdev_alloc();
    kernel_cdev->ops = &fops;
    kernel_cdev->owner = THIS_MODULE;
    MLOAD_DEBUG (" ~~~~~~~~~~~ Mload Inside init module ~~~~~~~~~~~~~~~\n");
    ret = alloc_chrdev_region( &dev_no , 0, 1,"chr_arr_dev");
    if (ret < 0) {
        MLOAD_DEBUG("mload Major number allocation is failed\n");
    }

    Major = MAJOR(dev_no);
    dev = MKDEV(Major,0);
    ret = cdev_add( kernel_cdev,dev,1);

    c1 = class_create(THIS_MODULE, "chardrv");
    device_create(c1, NULL, dev, NULL, "mload");


    return ret;
}

void mload_cleanup(void)
{

    MLOAD_DEBUG("mload Inside cleanup_module\n");
    cdev_del(kernel_cdev);
    unregister_chrdev_region(Major, 1);


}

MODULE_LICENSE("GPL");
module_init(mload_init);
module_exit(mload_cleanup);

#endif

EXPORT_SYMBOL(IspMLoadInit);
EXPORT_SYMBOL(IspMLoadDeInit);
EXPORT_SYMBOL(IspMLoadTableSet);
EXPORT_SYMBOL(IspMLoadApply);
EXPORT_SYMBOL(SclMLoadApply);
EXPORT_SYMBOL(IspMloadUT);
