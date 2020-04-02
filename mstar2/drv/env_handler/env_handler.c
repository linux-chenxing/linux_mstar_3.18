#include <linux/autoconf.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/cdev.h>
#include <linux/time.h> 
#include <linux/device.h>
#include <asm/io.h>
#include <asm/types.h>
#include <asm/cacheflush.h>
#include <linux/vmalloc.h>
#include <linux/version.h>
#include <../block/partitions/check.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/page-flags.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <asm/io.h>
#include <linux/sched.h>
#include <generated/asm-offsets.h>
#include "env_handler.h"


#ifdef CONFIG_COMPAT  

#include <linux/compat.h>

typedef struct {

     compat_int_t                 block_id;      
     compat_int_t               offset;    
} compat_read_write_pos;

typedef struct {

     compat_read_write_pos                 pos;      
     compat_uptr_t               data;    
} compat_read_write_struct;

#define COMPAT_ENV_HANDLER_GET_DEVICE_INFO _IOR(ENV_HANDLER_MAGIC,1,device_info)
#define COMPAT_ENV_HANDLER_READ _IOR(ENV_HANDLER_MAGIC,2,compat_read_write_struct)
#define COMPAT_ENV_HANDLER_WRITE _IOW(ENV_HANDLER_MAGIC,3,compat_read_write_struct)
#define COMPAT_ENV_HANDLER_ERASE _IOW(ENV_HANDLER_MAGIC,4,int)

#endif

#define MOD_ENV_HANDLER_DEVICE_COUNT     2
#define MOD_ENV_HANDLER_NAME            "env_handler"
#define MOD_ENV_HANDLER_NAME_1             "env_handler_1"
#define MOD_ENV_HANDLER_NAME_2             "env_handler_2"

typedef struct
{
    int                         env_handler_major;
    int                         env_handler_minor;
    struct cdev                 env_handler_device;
    struct file_operations      env_handler_file_operations;
    device_info          info;	
} env_handler_mod;

static long env_handler_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
static int env_handler_open (struct inode *inode, struct file *filp);

#ifdef CONFIG_COMPAT  
static long compat_env_handler_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
static int compat_env_handler_read_allocation_data(compat_read_write_struct __user *data32, read_write_struct  __user *data);
#endif


static struct class *env_handler_class;

static env_handler_mod env_handler_dev=
{
    .env_handler_major=               199,
    .env_handler_minor=               1,
    .env_handler_device=
    {
        .kobj=                  {.name= MOD_ENV_HANDLER_NAME, },
        .owner  =               THIS_MODULE,
    },
    .env_handler_file_operations=
    {
    	 .open             =     env_handler_open,    
        .unlocked_ioctl=          env_handler_ioctl,
        #ifdef CONFIG_COMPAT        
        .compat_ioctl =         compat_env_handler_ioctl,
        #endif
    },
};

static void partion_end_bio(struct bio *bio, int err)
{
    const int uptodate = test_bit(BIO_UPTODATE, &bio->bi_flags);
	struct page *page = bio->bi_io_vec[0].bv_page;

	if (!uptodate) {
		SetPageError(page);
		ClearPageUptodate(page);
		printk(KERN_ALERT "Read-error on swap-device (%u:%u:%Lu)\n",
				imajor(bio->bi_bdev->bd_inode),
				iminor(bio->bi_bdev->bd_inode),
				(unsigned long long)bio->bi_sector);
	} else {
		SetPageUptodate(page);
	}
	unlock_page(page);
	bio_put(bio);
}

static int submit_page(struct block_device* bd, int rw, pgoff_t page_off,
                                            struct page *page)
{
      struct bio *bio;

	bio = bio_alloc(__GFP_WAIT | __GFP_HIGH, 1);
	if (!bio)
		return -ENOMEM;
	bio->bi_sector = page_off * (PAGE_SIZE >> 9);
	bio->bi_bdev = bd;
	bio->bi_end_io = partion_end_bio;

	if (bio_add_page(bio, page, PAGE_SIZE, 0) < PAGE_SIZE) {
		printk(KERN_ERR "ENV_HANDLER: Adding page to bio failed at %ld\n",page_off);
		bio_put(bio);
		return -EFAULT;
	}

	lock_page(page);
	bio_get(bio);

	submit_bio(rw | REQ_SYNC, bio);
	wait_on_page_locked(page);
	if (rw == READ)
		bio_set_pages_dirty(bio);
	bio_put(bio);
	return 0;
}

unsigned char *read_page(struct block_device *bdev, sector_t n, Sector *p)
{
      struct address_space *mapping = bdev->bd_inode->i_mapping;
      struct page *page;
      page = read_mapping_page(mapping, (pgoff_t)n, NULL);
      p->v = page;
      return (unsigned char *)page_address(page) ;
}


#ifdef CONFIG_COMPAT  

static int compat_env_handler_read_allocation_data(
			compat_read_write_struct __user *data32,
			read_write_struct  __user *data)
{
      int err;
      compat_int_t  s32;
      compat_uptr_t compat_ptr;
	err = get_user(s32, &data32->pos.block_id);
	err |= put_user(s32, &data->pos.block_id);
	err |= get_user(s32, &data32->pos.offset);
	err |= put_user(s32, &data->pos.offset);
	err |= get_user(compat_ptr, &data32->data);
	err |= put_user(compat_ptr, &data->data);    

	return err;  
}

static long compat_env_handler_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{    
    int err=0;
    if (!filp->f_op || !filp->f_op->unlocked_ioctl)
        return -ENOTTY;

      switch (cmd) 
	{
	    case COMPAT_ENV_HANDLER_GET_DEVICE_INFO:	
	    {
			      return filp->f_op->unlocked_ioctl(filp, ENV_HANDLER_GET_DEVICE_INFO, (unsigned long)compat_ptr(arg));
	    }
	    case COMPAT_ENV_HANDLER_READ:		
	    {
                         compat_read_write_struct __user *data32;
		            read_write_struct __user *data;
		            data32 = compat_ptr(arg);
		            data = compat_alloc_user_space(sizeof(*data));
		            
		            if (data == NULL)
		                return -EFAULT;               

		            err = compat_env_handler_read_allocation_data(data32, data);
		            if (err)
		                return err;            
		    		return  filp->f_op->unlocked_ioctl(filp, ENV_HANDLER_READ,  (unsigned long)data);
	      }
		case COMPAT_ENV_HANDLER_WRITE:
	      {

                         compat_read_write_struct __user *data32;
		            read_write_struct __user *data;
		            data32 = compat_ptr(arg);
		            data = compat_alloc_user_space(sizeof(*data));
		            
		            if (data == NULL)
		                return -EFAULT;               

		            err = compat_env_handler_read_allocation_data(data32, data);
		            if (err)
		                return err;            
		    		return  filp->f_op->unlocked_ioctl(filp, ENV_HANDLER_WRITE,  (unsigned long)data);
	      }	
		case COMPAT_ENV_HANDLER_ERASE:
		{
				return filp->f_op->unlocked_ioctl(filp, ENV_HANDLER_ERASE, (unsigned long)compat_ptr(arg));
		}  
	}	
	  return 	 -ENOSYS;
}

#endif

static unsigned short env_handler_logical_block_size(struct request_queue *q)
{
	int retval = 512;

	if (q && q->limits.logical_block_size)
		retval = q->limits.logical_block_size;

	return retval;
}

static int env_handler_open (struct inode *inode, struct file *filp)
{
	 struct block_device *bd;
	 char device_name[100];
        loff_t size = 0;
	 
	  if(MINOR(inode->i_rdev)==1)
	 {
	 	sprintf(device_name,"/dev/block/platform/mstar_mci.0/by-name/%s",CONFIG_MSTAR_ENV_HANDLER_1);
	 }
	 else if(MINOR(inode->i_rdev)==2)
	 {
	 	sprintf(device_name,"/dev/block/platform/mstar_mci.0/by-name/%s",CONFIG_MSTAR_ENV_HANDLER_2);	
	 }
	 
	 bd = blkdev_get_by_path(device_name, FMODE_READ|FMODE_WRITE, NULL);

       if(PAGE_SZ != bd->bd_block_size)
	  {
	         set_blocksize(bd,PAGE_SZ);
        }

        
 
	  env_handler_dev.info.rw_block_size=bd->bd_block_size;
	  env_handler_dev.info.erase_block_size=bd->bd_block_size;
	  //env_handler_dev.info.erase_block_count=i_size_read(bd->bd_inode)/env_handler_dev.info.erase_block_size;
	  size = i_size_read(bd->bd_inode);
	  do_div(size,env_handler_dev.info.erase_block_size);
	  env_handler_dev.info.erase_block_count= size;
	  filp->private_data = bd;
	  
	 return 0;
}

static long env_handler_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{


    Sector sector;
    char *data;
    read_write_struct m_read_write_struct;
    struct block_device *bd = filp->private_data;	

    switch(cmd)
    {
        case ENV_HANDLER_GET_DEVICE_INFO:
	 {
            if (copy_to_user(( struct device_info *) arg, &env_handler_dev.info, sizeof(env_handler_dev.info)))
			 return -EFAULT;
            break;
	 }
        case ENV_HANDLER_READ:
	 { 	
          if (copy_from_user (&m_read_write_struct, (struct read_write_struct *) arg,sizeof(m_read_write_struct)))
			return -EFAULT;
		 
	     data=read_page(bd, m_read_write_struct.pos.block_id, &sector);

           if (copy_to_user((char *)m_read_write_struct.data, data,env_handler_dev.info.rw_block_size))
	              return -EFAULT;

	      put_dev_sector(sector);
            break;
	 }
        case ENV_HANDLER_WRITE:
	 {		
            if (copy_from_user (&m_read_write_struct, (struct read_write_struct *) arg,sizeof(m_read_write_struct)))
			return -EFAULT;

	     data= read_page(bd, m_read_write_struct.pos.block_id, &sector);

	     if (copy_from_user (data, (char *) m_read_write_struct.data, env_handler_dev.info.rw_block_size))
			return -EFAULT;
		 
	     submit_page(bd,1,m_read_write_struct.pos.block_id,sector.v);
	     put_dev_sector(sector);
            break;
	 }
        case ENV_HANDLER_ERASE:
            return 0;
			
        default:
            printk("\nUnknown ioctl command %x\n", cmd);
			
        return -ENOTTY;
    }


    return 0;
}

 int __init mod_env_handler_init(void)
{
    int s32Ret;
    dev_t dev;

    env_handler_class = class_create(THIS_MODULE, "env_handler");
    if (IS_ERR(env_handler_class))
    {
        return PTR_ERR(env_handler_class);
    }

    if (env_handler_dev.env_handler_major)
    {
        dev = MKDEV(env_handler_dev.env_handler_major, env_handler_dev.env_handler_minor);
        s32Ret = register_chrdev_region(dev, MOD_ENV_HANDLER_DEVICE_COUNT, MOD_ENV_HANDLER_NAME);
    }
    else
    {
        s32Ret = alloc_chrdev_region(&dev, env_handler_dev.env_handler_minor, MOD_ENV_HANDLER_DEVICE_COUNT, MOD_ENV_HANDLER_NAME);
        env_handler_dev.env_handler_major = MAJOR(dev);
    }

    if ( 0 > s32Ret)
    {
        class_destroy(env_handler_class);
        return s32Ret;
    }

    cdev_init(&env_handler_dev.env_handler_device, &env_handler_dev.env_handler_file_operations);
	
    if (0!= (s32Ret= cdev_add(&env_handler_dev.env_handler_device, dev, MOD_ENV_HANDLER_DEVICE_COUNT)))
    {
        unregister_chrdev_region(dev, MOD_ENV_HANDLER_DEVICE_COUNT);
        class_destroy(env_handler_class);
        return s32Ret;
    }
	
    device_create(env_handler_class, NULL, MKDEV(env_handler_dev.env_handler_major, 1), NULL, CONFIG_MSTAR_ENV_HANDLER_1);
    device_create(env_handler_class, NULL, MKDEV(env_handler_dev.env_handler_major, 2), NULL, CONFIG_MSTAR_ENV_HANDLER_2);	
	
    return 0;
}


 void __exit mod_env_handler_exit(void)
{
    cdev_del(&env_handler_dev.env_handler_device);
    unregister_chrdev_region(MKDEV(env_handler_dev.env_handler_major, env_handler_dev.env_handler_minor), MOD_ENV_HANDLER_DEVICE_COUNT);

    device_destroy(env_handler_class, MKDEV(env_handler_dev.env_handler_major, env_handler_dev.env_handler_minor));
    class_destroy(env_handler_class);
}

module_init(mod_env_handler_init);
module_exit(mod_env_handler_exit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("Env handler driver");
MODULE_LICENSE("GPL");

