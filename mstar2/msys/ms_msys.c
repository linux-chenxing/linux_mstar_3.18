/*
 * mdrv_system.c
 *
 *  Created on: 2012/9/21
 *      Author: Administrator
 */
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/errno.h>
#include <linux/dma-mapping.h>      /* for dma_alloc_coherent */
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/ctype.h>
#include <linux/swap.h>
#include <linux/vmalloc.h>
#include <linux/delay.h>
#include <linux/seq_file.h>
#include <linux/compaction.h>
#include <asm/cacheflush.h>

#include "ms_platform.h"
//#include "registers.h"
#include "mdrv_msys_io_st.h"
// #include "mdrv_msys_io.h"
//#include "mdrv_verchk.h"


#define BENCH_MEMORY_FUNC            0
#define MSYS_DEBUG                   0
#define MINOR_SYS_NUM               128
#define MAJOR_SYS_NUM               233

// keep
#if MSYS_DEBUG
#define MSYS_PRINT(fmt, args...)    printk("[MSYS] " fmt, ## args)
#else
#define MSYS_PRINT(fmt, args...)
#endif

// keep
#define MSYS_ERROR(fmt, args...)    printk(KERN_ERR"MSYS: " fmt, ## args)
#define MSYS_WARN(fmt, args...)     printk(KERN_WARNING"MSYS: " fmt, ## args)


struct ms_chip
{
    void (*chip_flush_miu_pipe)(void);
    void (*chip_flush_memory)(void);
    void (*chip_read_memory)(void);
    int  (*cache_outer_is_enabled)(void);
    void (*cache_flush_all)(void);
    void (*cache_clean_range_va_pa)(unsigned long, unsigned long,unsigned long);
    void (*cache_flush_range_va_pa)(unsigned long, unsigned long,unsigned long);
    void (*cache_clean_range)(unsigned long, unsigned long);
    void (*cache_flush_range)(unsigned long, unsigned long);
    void (*cache_invalidate_range)(unsigned long, unsigned long);
    u64 (*phys_to_miu)(u64);
    u64 (*miu_to_phys)(u64);

    int   (*chip_get_device_id)(void);
    char* (*chip_get_platform_name)(void);
    int   (*chip_get_revision)(void);

    const char* (*chip_get_API_version)(void);

    int  (*chip_get_boot_dev_type)(void);
    unsigned long long  (*chip_get_riu_phys)(void);
    int  (*chip_get_riu_size)(void);
    int  (*chip_get_storage_type)(void);
    int  (*chip_get_package_type)(void);

    int (*chip_function_set)(int functionId, int param);

    u64 (*chip_get_us_ticks)(void);
};



extern void Chip_Flush_Memory(void);

static int msys_open(struct inode *inode, struct file *filp);
static int msys_release(struct inode *inode, struct file *filp);
static long msys_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

typedef struct
{
  MSYS_PROC_DEVICE proc_dev;
  void *proc_addr;
  struct proc_dir_entry* proc_entry;
  struct list_head list;
} PROC_INFO_LIST;

// static int msys_request_proc_attr(MSYS_PROC_ATTRIBUTE* proc_attr);
// static int msys_release_proc_attr(MSYS_PROC_ATTRIBUTE* proc_attr);
// static int msys_request_proc_dev(MSYS_PROC_DEVICE* proc_dev);
// static int msys_release_proc_dev(MSYS_PROC_DEVICE* proc_dev);

#if BENCH_MEMORY_FUNC==1
static void msys_bench_memory(unsigned int);
#endif

static struct file_operations msys_fops = {
    .owner = THIS_MODULE,
    .open = msys_open,
    .release = msys_release,
    .unlocked_ioctl=msys_ioctl,
};

// keep
static struct miscdevice sys_dev = {
    .minor      = MINOR_SYS_NUM,
    .name       = "msys",
    .fops       = &msys_fops,
};

static struct ms_chip chip_funcs;

//static unsigned char data_part_string[32]={0};
//static unsigned char system_part_string[32]={0};

//static unsigned char mstar_property_path[32]="/data";

static u64 sys_dma_mask = 0xffffffffUL; //keep
struct list_head kept_mem_head;     // keep
struct list_head fixed_mem_head;    // keep
static struct mutex dmem_mutex;     // keep
static unsigned char fixed_dmem_enabled=0;  // keep
static unsigned char dmem_realloc_enabled=0;
//static unsigned long dmem_lock_flags;

//static unsigned int dmem_retry_interval=100; //(ms)
static unsigned int dmem_retry_count=16;

struct DMEM_INFO_LIST
{
  MSYS_DMEM_INFO dmem_info;
  struct list_head list;
};

//port from fs/proc/meminfo.c
unsigned int meminfo_free_in_K(void)
{
    return 0;
}
EXPORT_SYMBOL(meminfo_free_in_K);

//static void *mm_mem_virt = NULL; /* virtual address of frame buffer 1 */

static int msys_open(struct inode *inode, struct file *filp)
{
//    printk(KERN_WARNING"%s():\n", __FUNCTION__);
    return 0;
}

static int msys_release(struct inode *inode, struct file *filp)
{
//    MSYS_PRINT(KERN_WARNING"%s():\n", __FUNCTION__);
    return 0;
}

int msys_fix_dmem(char* name)
{
    return 0;
}

int msys_unfix_dmem(char* name)
{
    return 0;
}

//keep
int msys_find_dmem_by_phys(unsigned long long phys,MSYS_DMEM_INFO *mem_info)
{
    struct list_head *ptr;
    struct DMEM_INFO_LIST *entry;

    int res=-EINVAL;

    mutex_lock(&dmem_mutex);

    if(0!=phys)
    {
        list_for_each(ptr, &kept_mem_head)
        {
            entry = list_entry(ptr, struct DMEM_INFO_LIST, list);
            if ((entry->dmem_info.phys<=phys) && phys<(entry->dmem_info.phys+entry->dmem_info.length))
            {
                memcpy(mem_info,&entry->dmem_info,sizeof(MSYS_DMEM_INFO));
                res=0;
                goto BEACH;
                ;
            }
        }
    }

BEACH:
    mutex_unlock(&dmem_mutex);
    return res;
}

int msys_find_dmem_by_name(const char *name, MSYS_DMEM_INFO *mem_info)
{
    return 0;
}

//keep
int msys_release_dmem(MSYS_DMEM_INFO *mem_info)
{
    struct list_head *ptr;
    struct DMEM_INFO_LIST *entry,*match_entry;

    int dmem_fixed=0;

    mutex_lock(&dmem_mutex);
    match_entry=NULL;

    if(mem_info->name[0]!=0)
    {
        list_for_each(ptr, &kept_mem_head)
        {
            int res=0;
            entry = list_entry(ptr, struct DMEM_INFO_LIST, list);
            res=strncmp(entry->dmem_info.name, mem_info->name,strnlen(mem_info->name,15));

            if (0==res)
            {
                match_entry=entry;
                break;
            }
        }
    }

    if(match_entry==NULL && (0!=mem_info->phys))
    {
        MSYS_ERROR("WARNING!! DMEM [%s]@0x%08X can not be found by name, try to find by phys address\n",mem_info->name, (unsigned int)mem_info->phys);
        list_for_each(ptr, &kept_mem_head)
        {
            entry = list_entry(ptr, struct DMEM_INFO_LIST, list);
            if (entry->dmem_info.phys==mem_info->phys)
            {
                match_entry=entry;
                break;
            }
        }
    }

    if(match_entry==NULL)
    {
        MSYS_ERROR("DMEM [%s]@0x%08X not found, skipping release...\n",mem_info->name, (unsigned int)mem_info->phys);
        goto BEACH;
    }

    if(fixed_dmem_enabled)
    {
        //check if entry is fixed
        list_for_each(ptr, &fixed_mem_head)
        {
            int res=0;
            entry = list_entry(ptr, struct DMEM_INFO_LIST, list);
            res=strcmp(entry->dmem_info.name, match_entry->dmem_info.name);
            if (0==res)
            {
                dmem_fixed=1;
                MSYS_PRINT("DMEM [%s]@0x%08X is fixed, skipping release...\n",match_entry->dmem_info.name,(unsigned int)match_entry->dmem_info.phys);
                goto BEACH;
            }
        }
    }

    dma_free_coherent(sys_dev.this_device, PAGE_ALIGN(match_entry->dmem_info.length),(void *)(uintptr_t)match_entry->dmem_info.kvirt,match_entry->dmem_info.phys);

    MSYS_PRINT("DMEM [%s]@0x%08X successfully released\n",match_entry->dmem_info.name,(unsigned int)match_entry->dmem_info.phys);

    list_del_init(&match_entry->list);
    kfree(match_entry);

BEACH:
    mutex_unlock(&dmem_mutex);
    return 0;
}

int msys_request_dmem(MSYS_DMEM_INFO *mem_info)
{
    dma_addr_t phys_addr;
    int err=0;
    int retry=0;

    if(mem_info->name[0]==0||strlen(mem_info->name)>15)
    {
        MSYS_ERROR( "Invalid DMEM name!! Either garbage or empty name!!\n");
        return -EINVAL;
    }

    MSYS_ERROR("DMEM request: [%s]:0x%08X\n",mem_info->name,(unsigned int)mem_info->length);

    mutex_lock(&dmem_mutex);

    {
        struct list_head *ptr;
        struct DMEM_INFO_LIST *entry;

        list_for_each(ptr, &kept_mem_head)
        {
            entry = list_entry(ptr, struct DMEM_INFO_LIST, list);
            if (0==strncmp(entry->dmem_info.name, mem_info->name,strnlen(mem_info->name,15)))
            {
                if(dmem_realloc_enabled && (entry->dmem_info.length != mem_info->length))
                {
                    MSYS_ERROR("dmem realloc %s", entry->dmem_info.name);
                    dma_free_coherent(sys_dev.this_device, PAGE_ALIGN(entry->dmem_info.length),(void *)(uintptr_t)entry->dmem_info.kvirt,entry->dmem_info.phys);
                    MSYS_ERROR("DMEM [%s]@0x%08X successfully released\n",entry->dmem_info.name,(unsigned int)entry->dmem_info.phys);
                    list_del_init(&entry->list);
                    break;
                }
                else
                {
                    memcpy(mem_info,&entry->dmem_info,sizeof(MSYS_DMEM_INFO));
                    MSYS_ERROR("DMEM kept entry found: name=%s, phys=0x%08X, length=0x%08X\n",mem_info->name,(unsigned int)mem_info->phys,(unsigned int)mem_info->length);
                    goto BEACH_ENTRY_FOUND;
                }
            }
        }
    }

    while( !(mem_info->kvirt = (u64)(uintptr_t)dma_alloc_coherent(sys_dev.this_device, PAGE_ALIGN(mem_info->length), &phys_addr, GFP_KERNEL)) )
    {
        if(retry >= dmem_retry_count)
        {
            MSYS_ERROR( "unable to allocate direct memory\n");
            err = -ENOMEM;
            goto BEACH_ALLOCATE_FAILED;
        }
        MSYS_ERROR( "retry ALLOC_DMEM %d\n",retry);
        sysctl_compaction_handler(NULL, 1, NULL, NULL, NULL);
        msleep(1000);
        retry++;
    }

    mem_info->phys=(u64)phys_addr;

    {
        struct DMEM_INFO_LIST *new=(struct DMEM_INFO_LIST *)kmalloc(sizeof(struct DMEM_INFO_LIST),GFP_KERNEL);
        if(new==NULL)
        {
            MSYS_ERROR("allocate memory for mem_list entry error\n" ) ;
            err = -ENOMEM;
            goto BEACH;

        }

        memset(new->dmem_info.name,0,16);
        memcpy(&new->dmem_info,mem_info,sizeof(MSYS_DMEM_INFO));

        list_add(&new->list, &kept_mem_head);
    }

    if(retry)
        MSYS_ERROR("DMEM request: [%s]:0x%08X success, @0x%08X (retry=%d)\n",mem_info->name,(unsigned int)mem_info->length, (unsigned int)mem_info->phys, retry);
    else
        MSYS_PRINT("DMEM request: [%s]:0x%08X success, @0x%08X\n",mem_info->name,(unsigned int)mem_info->length, (unsigned int)mem_info->phys, retry);

BEACH:
    if(err==-ENOMEM)
    {
        msys_release_dmem(mem_info);
    }

BEACH_ALLOCATE_FAILED:
BEACH_ENTRY_FOUND:
    if(err)
    {
        MSYS_ERROR("DMEM request: [%s]:0x%08X FAILED!! (retry=%d)\n",mem_info->name,(unsigned int)mem_info->length, retry);
    }

    mutex_unlock(&dmem_mutex);
    return err;
}

unsigned int get_PIU_tick_count(void)
{
    return 0;
}
EXPORT_SYMBOL(get_PIU_tick_count);

int msys_user_to_physical(unsigned long addr,unsigned long *phys)
{
    return 0;
}

int msys_flush_cache(unsigned long arg)
{
    return 0;
}

int msys_addr_translation_verchk(unsigned long arg, bool direction)
{
    return 0;
}

int msys_get_riu_map_verchk(unsigned long arg)
{
    return 0;
}


int msys_fix_dmem_verchk(unsigned long arg)
{
    return 0;
}

int msys_unfix_dmem_verchk(unsigned long arg)
{
    return 0;
}

int msys_miu_protect_verchk(unsigned long arg)
{
    return 0;
}

int msys_string_verchk(unsigned long arg, unsigned int op)
{
    return 0;
}

extern int g_sCurrentTemp;

int msys_get_temp_verchk(unsigned long arg)
{
    return 0;
}

int msys_get_udid_verchk(unsigned long arg)
{
    return 0;
}

static long msys_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    return 0;
}

static int __init setup_system_part_string(char *arg)
{
    return 0;
}
static int __init setup_data_part_string(char *arg)
{
    return 0;
}

__setup("sysp",setup_system_part_string);
__setup("datap",setup_data_part_string);


static ssize_t unfix_dmem_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t n)
{
    return 0;
}

DEVICE_ATTR(unfix_dmem, 0200, NULL, unfix_dmem_store);

static ssize_t fixed_dmem_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t n)
{
    return 0;
}

static ssize_t fixed_dmem_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    return 0;
}

DEVICE_ATTR(fixed_dmem, 0644, fixed_dmem_show, fixed_dmem_store);

// keep
struct list_head proc_info_head;
static struct mutex proc_info_mutex;
static struct proc_dir_entry* proc_class=NULL;
static struct proc_dir_entry* proc_zen_kernel=NULL;

struct proc_dir_entry* msys_get_proc_class(void)
{
    return proc_class;
}

struct proc_dir_entry* msys_get_proc_zen_kernel(void)
{
    return proc_zen_kernel;
}

static int msys_seq_show(struct seq_file*m, void *p)
{
    return 0;
}

static int msys_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, msys_seq_show, inode);
}

static int msys_proc_mmap(struct file *file, struct vm_area_struct *vma)
{
    return 0 ;
}

static const struct file_operations msys_proc_fops = {
    .owner = THIS_MODULE,
    .open  = msys_proc_open,
    .read  = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

static const struct file_operations msys_proc_mmap_fops = {
    .owner = THIS_MODULE,
    .open = msys_proc_open,
    .mmap = msys_proc_mmap,
    .release = single_release,
};

static struct class *msys_sysfs_class = NULL;

struct class *msys_get_sysfs_class(void)
{
    return msys_sysfs_class;
}

static int __init msys_init(void)
{
    int ret;

    //ret = misc_register(&sys_dev);
    ret = register_chrdev(MAJOR_SYS_NUM, "msys", &msys_fops);
    if (ret != 0) {
        MSYS_ERROR("cannot register msys on minor=11 (err=%d)\n", ret);
    }

    // sys_dev.this_device = device_create(msys_get_sysfs_class(), NULL,
        // MKDEV(MAJOR_SYS_NUM, MINOR_SYS_NUM), NULL, "msys");

    msys_sysfs_class = class_create(THIS_MODULE, "msys");
    if (IS_ERR(msys_sysfs_class))
        return 0;
    sys_dev.this_device = device_create(msys_sysfs_class, NULL,
        MKDEV(MAJOR_SYS_NUM, MINOR_SYS_NUM), NULL, "msys");

    sys_dev.this_device->dma_mask=&sys_dma_mask;
    sys_dev.this_device->coherent_dma_mask=sys_dma_mask;
    mutex_init(&dmem_mutex);

    INIT_LIST_HEAD(&kept_mem_head);
    INIT_LIST_HEAD(&fixed_mem_head);

    // device_create_file(sys_dev.this_device, &dev_attr_dmem);
    // device_create_file(sys_dev.this_device, &dev_attr_fixed_dmem);
    // device_create_file(sys_dev.this_device, &dev_attr_unfix_dmem);
    // device_create_file(sys_dev.this_device, &dev_attr_release_dmem);
    // device_create_file(sys_dev.this_device, &dev_attr_PIU_T);
    // device_create_file(sys_dev.this_device, &dev_attr_dmem_retry_interval);
    // device_create_file(sys_dev.this_device, &dev_attr_dmem_retry_count);
    // device_create_file(sys_dev.this_device, &dev_attr_us_ticks);
    // device_create_file(sys_dev.this_device, &dev_attr_dmem_realloc);

#if defined(CONFIG_PROC_FS)
    mutex_init(&proc_info_mutex);
    INIT_LIST_HEAD(&proc_info_head);
    proc_class=proc_mkdir("mstar",NULL);
    proc_zen_kernel=proc_mkdir("kernel",proc_class);
#endif

    MSYS_WARN(" INIT DONE. TICK=0x%08X\n",get_PIU_tick_count());

    return 0;
}

// void Chip_Flush_Memory(void)
// {
    // chip_funcs.chip_flush_memory();
// }

u64 Chip_Phys_to_MIU(u64 phys)
{
    return chip_funcs.phys_to_miu(phys);
}

u64 Chip_MIU_to_Phys(u64 miu)
{
    return chip_funcs.miu_to_phys(miu);
}

subsys_initcall(msys_init);
EXPORT_SYMBOL(msys_user_to_physical);
EXPORT_SYMBOL(msys_request_dmem);
EXPORT_SYMBOL(msys_release_dmem);
EXPORT_SYMBOL(Chip_Flush_Memory);
EXPORT_SYMBOL(Chip_Phys_to_MIU);
EXPORT_SYMBOL(Chip_MIU_to_Phys);
MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("SYSTEM driver");
MODULE_LICENSE("MSTAR");
