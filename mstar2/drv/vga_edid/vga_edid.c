#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <asm/irq.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/list.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <asm/unistd.h>

#include <linux/err.h>
#include <linux/stringify.h>
#include <linux/namei.h>
#include <linux/stat.h>
#include <linux/log2.h>
#include "mdrv_iic1.h"

extern void MDrv_HW_IIC1_Init(void);
extern BOOL MDrv_HWI2C1_ReadBytes(U16 u16SlaveCfg, U32 uAddrCnt, U8 *pRegAddr, U32 uSize, U8 *pData);

#define VGA_EDID_DBG_ENABLE              0

#if VGA_EDID_DBG_ENABLE
#define VGA_EDID_DBG(_f)                 (_f)
#else
#define VGA_EDID_DBG(_f)
#endif

#define VGA_EDID_DEV_NAME "vga_edid"

typedef struct
{
	U8 u8RegAddr;
	U8 u8Value[128];
	U8 u8ret;
} EdidRead;



#define VGA_EDID_IOCTL_MAGIC             'V'
#define IOCTL_VGA_EDID_READ                _IOWR(VGA_EDID_IOCTL_MAGIC, 0x00, EdidRead)
#define IOCTL_VGA_EDID_MAXNR    0x02




int vga_edid_open(struct inode *inode, struct file *fp)
{
   MDrv_HW_IIC1_Init();
   VGA_EDID_DBG(("vga_edid_open \n"));

    return 0;
}

long vga_edid_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    S32 s32Err= 0;
	U8 addr[1];
	U8 buf[128];
	U8 i;
	EdidRead edidReadvalue;
    /*
     * extract the type and number bitfields, and don't decode
     * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
     */
    if ((VGA_EDID_IOCTL_MAGIC!= _IOC_TYPE(cmd)) || (_IOC_NR(cmd)> IOCTL_VGA_EDID_MAXNR))
    {
        return -ENOTTY;
    }
	
    if (_IOC_DIR(cmd) & _IOC_READ)
    {
        s32Err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
    }
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
    {
        s32Err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
    }
    if (s32Err)
    {
        return -EFAULT;
    }	
	
    switch(cmd){
        case IOCTL_VGA_EDID_READ:		
               addr[0]=0x00;
               edidReadvalue.u8ret=MDrv_HWI2C1_ReadBytes(0xa0,1,addr,2,edidReadvalue.u8Value);
               for(i=0;i<128;i++)
               {
               	   if(i%16==0&&i!=0)
            	   	   VGA_EDID_DBG(("\n"));
                   VGA_EDID_DBG(printk("0x%02x,",edidReadvalue.u8Value[i]));
            
               } 
		    if(copy_to_user( (void *)arg, &edidReadvalue, sizeof(edidReadvalue) ))	
			{			
			    printk( "copy_to_user error\n" );			
			    return -EFAULT;		
			}
			   
            break;  
			
        default:
            return EINVAL;
    }
    
    return ret;
}

static struct file_operations dev_fops = {
	.owner			= THIS_MODULE,
	.open	        = vga_edid_open,
	.unlocked_ioctl = vga_edid_ioctl,
};

static struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name  = VGA_EDID_DEV_NAME,
	.fops  = &dev_fops,
};

static int __init dev_init(void)
{
	int ret;

	ret = misc_register(&misc);	
	return ret;
}

static void __exit dev_exit(void)
{
	misc_deregister(&misc);
}

module_init(dev_init);
module_exit(dev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mstar");
