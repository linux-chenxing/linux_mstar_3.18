///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2006 - 2007 MStar Semiconductor, Inc.
// This program is free software.
// You can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation;
// either version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program;
// if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   ipanic-emmc.c
/// @brief  dump all info into emmc device when kernel panic
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <linux/version.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <linux/genhd.h>
#include <linux/seq_file.h>
#include <linux/crc32.h>
#include <linux/kmsg_dump.h>
#include <linux/seq_file.h>
#include <linux/debugfs.h>
#include "ipanic.h"

/*  0: write to emmc by block device
  *  1: write to file by filesystem
  *  2: write to emmc by emmc hal API
  */
static int FILEWRITE = 2;
static int in_panic = 0;
static struct kmsg_dumper ipanic_kmsg_dumper;
static struct ipanic_header iheader;
struct mstar_ipanic_emmcdevice_info __section(.FCIE.module.rodata) ipanic_emmcdevice_info;
extern union IPANIC_MEM  ipanic_memory;

static char *emmc_allocate_and_read(int offset, int length)
{
    int size, memsize;
    char *buff = NULL;

    if (length == 0) 
    {
        return NULL;
    }

    size = ALIGN(length, EMMC_BLOCK_SIZE);
    memsize = size;
    //get more one bytes at least  for print to stdio parse
    if(memsize == length)
    {
       memsize += 1;
    }
    buff = kzalloc(memsize, GFP_KERNEL);
    if (buff != NULL)
    {        
        if (card_dump_func_read(buff, size, offset) < size) 
        {
            IPANIC_DPRINTK("read data fail\n");
            kfree(buff);
            buff = NULL;
        }
    }
    else
    {
        IPANIC_DPRINTK("allocate buffer fail(len:%d)\n", length);
    }
    return buff;
}

static int emmc_ipanic_write(void *buf, int off, int len, int filewrite)
{       
    int rem = len & (EMMC_BLOCK_SIZE - 1);
    len = len & ~(EMMC_BLOCK_SIZE - 1);

    if (len > 0)
    {
        if (card_dump_func_write((unsigned char *)buf, len, off, filewrite) < len)
        {
            IPANIC_DPRINTK("write data fail\n");
            return -1;
        }
    }

    if (rem != 0)
    {
        if(filewrite == 1) //write to file
        {
            if (card_dump_func_write(buf + len, rem, off + len, filewrite) < rem)
                return -1;         
        }
        else  //write to emmc
        {
            memcpy(ipanic_memory.emmc_bounce, buf + len, rem);
            memset(ipanic_memory.emmc_bounce + rem, 0, EMMC_BLOCK_SIZE - rem);
            if (card_dump_func_write(ipanic_memory.emmc_bounce, EMMC_BLOCK_SIZE, off + len, filewrite) < EMMC_BLOCK_SIZE)
                return -1;         
        }
    }
    
    return len + rem;
}


#define LOGGER_MAIN_DEV_TYPE    1
#define LOGGER_RADIO_DEV_TYPE   3
#define LOGGER_SYSTEM_DEV_TYPE  4

#define LOGGER_MAIN_DEV  "/dev/log/main"
#define LOGGER_SYSTEM_DEV  "/dev/log/system"
#define LOGGER_RADIO_DEV  "/dev/log/radio"

#define BLANKSPACE 32
#define NEWLINE    10
#ifdef DUMP_ANDROID_LOG
static int ipanic_write_android_buf(unsigned int off, int type, int filewrite)
{
    int ret = 0;
    unsigned int interBufWriteOff = 0;
    unsigned int copy_count = 0;    
    char * logger_dev_name = NULL;
    struct file *fp = NULL;    

    switch (type)
    {
        case LOGGER_MAIN_DEV_TYPE:
            logger_dev_name = LOGGER_MAIN_DEV;
            break;

        case LOGGER_RADIO_DEV_TYPE:
            logger_dev_name = LOGGER_RADIO_DEV;
            break;

        case LOGGER_SYSTEM_DEV_TYPE:
            logger_dev_name = LOGGER_SYSTEM_DEV;
            break; 

         default:
            IPANIC_DPRINTK("not support droid logger type %d\n", type);
            return 0;
    }
    
    fp = filp_open(logger_dev_name, O_RDONLY | O_NONBLOCK, 0);
    if(IS_ERR(fp))
    {
        IPANIC_DPRINTK("open logger device %s fail\n", logger_dev_name);
        return 0;
    }

    while (1) 
    {
        int bufWriteSize = 0;
        //IPANIC_DPRINTK("type %d interBufWriteOff %d\n",type, interBufWriteOff); 
         
        int rc = panic_dump_android_log(fp, ipanic_memory.emmc_bounce, interBufWriteOff, PAGE_SIZE, type, 2*PAGE_SIZE-interBufWriteOff);
        BUG_ON(rc < 0);
        if (rc <= 0)
            break;
        
        if (rc < PAGE_SIZE)
        {
            interBufWriteOff = 0;
            if(filewrite == 1) //write to file
            {
                bufWriteSize = rc;
            }
            else  //write to emmc
            {
                memset(ipanic_memory.emmc_bounce + rc, BLANKSPACE, PAGE_SIZE - rc);
                ipanic_memory.emmc_bounce[PAGE_SIZE-1] = NEWLINE;
                bufWriteSize = PAGE_SIZE;
            }
            
            ret = card_dump_func_write(ipanic_memory.emmc_bounce, bufWriteSize, off, FILEWRITE);            
            if (ret < bufWriteSize)
            {
                IPANIC_DPRINTK("write android log failed writeleng %d ret %d\n", bufWriteSize, ret);
                return -1;
            }
        }

        if(rc >= PAGE_SIZE)
        {
            int onePageExceed = 0;
            
            ret = card_dump_func_write(ipanic_memory.emmc_bounce, PAGE_SIZE, off, FILEWRITE);
            if (ret < PAGE_SIZE)
            {
                IPANIC_DPRINTK("write android log failed writeleng %lu ret %d\n", PAGE_SIZE, ret);
                return -1;
            }

            onePageExceed = rc - PAGE_SIZE;
            if(onePageExceed)
            {
                memcpy(ipanic_memory.emmc_bounce, ipanic_memory.emmc_bounce+PAGE_SIZE, onePageExceed);
                interBufWriteOff = onePageExceed;
                rc = PAGE_SIZE;
            }
        }
         
        copy_count += rc;
        off += PAGE_SIZE;        
    }
    //IPANIC_DPRINTK("dump droid log type %d, count %d\n", type, copy_count);

    filp_close(fp, NULL);  
    return copy_count;
}

static int ipanic_write_all_android_buf(int offset, struct ipanic_header *hdr, int filewrite)
{
    int rc;
    char * str = NULL;
    int length = 0;

    // main buffer:
    if(filewrite == 1) //write to file
    {
       str = "\n dump android log main buffer\n\n";
       length = strlen(str);
       emmc_ipanic_write(str, offset, length, FILEWRITE);           
    }
    
    offset = ALIGN(offset, EMMC_BLOCK_SIZE);
    rc = ipanic_write_android_buf(offset, 1, filewrite);
    if (rc > 0) 
    {
        hdr->android_main_offset = offset;
        hdr->android_main_length = rc;
        offset += rc;
    }
    else
    {
        IPANIC_DPRINTK("Failed to write android main buffer\n");
    }

    // radio buffer:
    if(filewrite == 1) //write to file
    {
       str = "\n dump android log radio buffer\n\n";
       length = strlen(str);
       emmc_ipanic_write(str, offset, length, FILEWRITE);           
    }
    offset = ALIGN(offset, EMMC_BLOCK_SIZE);
    rc = ipanic_write_android_buf(offset, 3, filewrite);
    if (rc > 0)
    {
        hdr->android_radio_offset = offset;
        hdr->android_radio_length = rc;
        offset += rc;
    }
    else
    {
        IPANIC_DPRINTK("Failed to write android radio buffer\n");
    }

    // system buffer:
    if(filewrite == 1) //write to file
    {
       str = "\n dump android log system buffer\n\n";
       length = strlen(str);
       emmc_ipanic_write(str, offset, length, FILEWRITE);           
    }
    offset = ALIGN(offset, EMMC_BLOCK_SIZE);
    rc = ipanic_write_android_buf(offset, 4, filewrite) ; // system buffer.
    if (rc > 0)
    {
        hdr->android_system_offset = offset;
        hdr->android_system_length = rc;
        offset += rc;
    }
    else 
    {
        IPANIC_DPRINTK("Failed to write android system buffer\n");
    }
    
    return offset;
}
#endif

static int ipanic_write_backtrace(unsigned int off)
{
   int ret = 0;
   ret = emmc_ipanic_write(ipanic_memory.oops_header.process_path, off, 
                           sizeof(struct ipanic_oops_header), FILEWRITE);
   
   return ret;
}

static int ipanic_write_userspace(unsigned int off)
{
    int ret = 0;
    int length = 0;
    
    DumpNativeInfo();

    length = strlen(ipanic_memory.NativeInfo);
    ret = emmc_ipanic_write(ipanic_memory.NativeInfo, off, length, FILEWRITE);
    
    return ret;
}

extern unsigned long ipanic_detail_start;
static DEFINE_SPINLOCK(ipanic_lock);
static u32 fcie_module_text_crc = 0, fcie_module_rodata_crc = 0;
#if (LINUX_VERSION_CODE == KERNEL_VERSION(3,10,40))
extern const char __mstar_fcie_text_start[];
extern const char __mstar_fcie_text_end[];
extern const char __mstar_fcie_rodata_start[];
extern const char __mstar_fcie_rodata_end[];
#else
char * const __mstar_fcie_text_start = NULL;
char * const __mstar_fcie_text_end = NULL;
char * const __mstar_fcie_rodata_start = NULL;
char * const __mstar_fcie_rodata_end = NULL;
#endif

int emmc_ipanic(struct notifier_block *this, unsigned long event, void *ptr)
{      
    int rc = 0;  
    char * loginfo = NULL;
    u32 text_crc, rodata_crc;
    size_t length = 0;    

    spin_lock(&ipanic_lock);
    /*In case many core run here concurrently*/
    if (in_panic)
    {     
        spin_unlock(&ipanic_lock);
        return NOTIFY_DONE;
    }
    
    in_panic = 1;
    spin_unlock(&ipanic_lock);

    //.text&.rodata section CRC check of fcie module
    if((unsigned long)__mstar_fcie_text_start >=  PAGE_OFFSET)
    {
        text_crc = ~(u32)0;
        length = __mstar_fcie_text_end - __mstar_fcie_text_start;
        if(length > 0)
            text_crc = crc32(text_crc, __mstar_fcie_text_start, length);
        else
            return NOTIFY_DONE;

        rodata_crc = ~(u32)0;
        length = __mstar_fcie_rodata_end - __mstar_fcie_rodata_start;
        if(length > 0)
            rodata_crc = crc32(rodata_crc, __mstar_fcie_rodata_start, length);
        else
            return NOTIFY_DONE;
        
        if(text_crc != fcie_module_text_crc)
        {
            printk(KERN_ERR "fcie module .text section check fail, .text section info\n");
            printk(KERN_ERR "start %lx end %lx, orig crc %08x, now crc %08x\n",
                (unsigned long)__mstar_fcie_text_start, (unsigned long)__mstar_fcie_text_end,
                fcie_module_text_crc, text_crc);
            return NOTIFY_DONE;
        }

        if(rodata_crc != fcie_module_rodata_crc)
        {
            printk(KERN_ERR "fcie module rodata section check fail, .rodata section info. check ipanic partition create or not\n");
            printk(KERN_ERR "start %lx end %08x, orig crc %08x, now crc %08x\n",
                (unsigned long)__mstar_fcie_rodata_start, (unsigned long)__mstar_fcie_rodata_end,
                fcie_module_rodata_crc, rodata_crc);
            return NOTIFY_DONE;
        }
    }  else {
        printk(KERN_ERR "[Ipanic Driver] fcie .text&.rodata section address invalid\n");
        return NOTIFY_DONE;
    } 
    
#ifdef CONFIG_PREEMPT
    /* Ensure that cond_resched() won't try to preempt anybody, important !!!*/
    add_preempt_count(PREEMPT_ACTIVE);
#endif 

    if (FILEWRITE != 2) //not triggered by kernel panic, ignore the kernel message
    {
        //printk(KERN_ERR "emmc_ipanic %d\n", __LINE__);
        memset(&iheader, 0, sizeof(struct ipanic_header));
    	iheader.magic = MSTAR_IPANIC_MAGIC;
    	iheader.version = MSTAR_IPANIC_PHDR_VERSION;
    	
    	iheader.console_length = 0;
    	iheader.console_length=ALIGN(iheader.console_length, EMMC_BLOCK_SIZE);
    	iheader.console_offset=ALIGN(sizeof(struct ipanic_header), EMMC_BLOCK_SIZE);        
    }    

    ipanic_oops_start();	
    /*
         * Write out the console
         * Section 0 is reserved for ipanic header, we start at section 1 with EMMC storage device
         * write all kernel panic message into file sequentially
         */
    //dump panic backtrace and process path
    if(FILEWRITE == 1)
    {         
        loginfo = "dump backtrace and process path\n";
        emmc_ipanic_write(loginfo, 0, strlen(loginfo), FILEWRITE);      
    }

    iheader.oops_header_offset=iheader.console_length+iheader.console_offset;
    iheader.oops_header_length = ipanic_write_backtrace(iheader.oops_header_offset);    
    if (iheader.oops_header_length < 0) {
        IPANIC_DPRINTK("Error writing oops header to panic log! (%d)\n", iheader.oops_header_length);
        iheader.oops_header_length = 0;
    }    
   
    //dump native information
    if(FILEWRITE == 1)
    {
        loginfo = "\ndump userspaceinfo\n";
        emmc_ipanic_write(loginfo, 0, strlen(loginfo), FILEWRITE);        
    }
	
    iheader.userspace_info_offset = ALIGN(iheader.oops_header_offset+ iheader.oops_header_length, EMMC_BLOCK_SIZE);    
    iheader.userspace_info_length = ipanic_write_userspace(iheader.userspace_info_offset);
    if (iheader.userspace_info_length < 0) {
            IPANIC_DPRINTK("Error writing user space process to panic log (%d)\n", iheader.console_length);
            iheader.userspace_info_length = 0;
    }  


#ifdef DUMP_ANDROID_LOG
    //dump android log
    if(FILEWRITE == 1)
    {
        emmc_ipanic_write("\n", iheader.oops_header_length, strlen("\n"), FILEWRITE);
    }
    
    ipanic_write_all_android_buf(iheader.userspace_info_offset + iheader.userspace_info_length, &iheader, FILEWRITE);   
#endif
    
    /*
         * Finally write the ipanic header
         * Section 0 is reserved for ipanic header with EMMC storage device
         */
    if(FILEWRITE != 1)
    {
        memset(ipanic_memory.emmc_bounce, 0, PAGE_SIZE);
        memcpy(ipanic_memory.emmc_bounce, &iheader, sizeof(struct ipanic_header));
    
        rc = emmc_ipanic_write(ipanic_memory.emmc_bounce, 0, ALIGN(sizeof(struct ipanic_header), EMMC_BLOCK_SIZE), FILEWRITE);
        if (rc <= 0) {
            IPANIC_DPRINTK("Header write failed (%d)\n", rc);
            goto IPANIC_OUT;
        }
    }

    IPANIC_DPRINTK("oops_header_len %u, userspace_info_len %d, console_len %u\n", 
                    iheader.oops_header_length, iheader.userspace_info_length, iheader.console_length);

#ifdef DUMP_ANDROID_LOG
    IPANIC_DPRINTK("android_main_len %u, android_event_len %u, android_radio_len %u, android_system_len %u)\n", 
                    iheader.android_main_length, iheader.android_event_length, iheader.android_radio_length, 
                    iheader.android_system_length);
#endif
    if(FILEWRITE != 1)
        IPANIC_DPRINTK("Panic dump sucessfully written to emmc\n");
  
IPANIC_OUT:
    in_panic = 0;
#ifdef CONFIG_PREEMPT
    sub_preempt_count(PREEMPT_ACTIVE);
#endif

    return NOTIFY_DONE;
}

static struct aee_oops *emmc_ipanic_oops_copy(void)
{
    struct aee_oops *oops = NULL;
    struct ipanic_header *hdr = NULL;
    int hdr_size = ALIGN(sizeof(struct ipanic_header), EMMC_BLOCK_SIZE);

    hdr = kzalloc(hdr_size, GFP_KERNEL);
    if (hdr == NULL) {
        IPANIC_DPRINTK("allocate ipanic header memory fail\n");
        return NULL;
    }

    if (card_dump_func_read((unsigned char *)hdr, hdr_size, 0) < 0) {
        IPANIC_DPRINTK("emmc panic log header read failed\n");
        return NULL;
    }
    
    if (ipanic_header_check(hdr) != 0)
        return NULL;    

    oops = ipanic_oops_create(IPANIC_MODULE_TAG);
    if (oops != NULL)
    {
        struct ipanic_oops_header *oops_header = (struct ipanic_oops_header *)emmc_allocate_and_read(hdr->oops_header_offset, hdr->oops_header_length);
        if (oops_header == NULL) {
            IPANIC_DPRINTK("read oops header (len:%d) fail\n", hdr->oops_header_length);
            goto error_return;
        }
        ipanic_oops_set_process_path(oops, oops_header->process_path);
        ipanic_oops_set_backtrace(oops, oops_header->backtrace);
        kfree(oops_header);

        oops->console = emmc_allocate_and_read(hdr->console_offset, hdr->console_length);
        oops->console_len = hdr->console_length;
        if (oops->console == NULL) {
            IPANIC_DPRINTK("read console message failed(len: %d)\n", oops->console_len);
            goto error_return;
        }

        /*If panic from kernel context, no user sapce info available. Shouldn't fail*/
        if (0 == hdr->userspace_info_length)
        {
            oops->userspace_info = NULL;
            oops->userspace_info_len = 0;
        }
        else
        {
            oops->userspace_info = emmc_allocate_and_read(hdr->userspace_info_offset, hdr->userspace_info_length);
            oops->userspace_info_len = hdr->userspace_info_length;
            if (oops->userspace_info == NULL) {
                IPANIC_DPRINTK("read usrespace info(len: %d) failed\n", oops->userspace_info_len);
                goto error_return;
            }
        }

#ifdef DUMP_ANDROID_LOG
        oops->android_main = emmc_allocate_and_read(hdr->android_main_offset, hdr->android_main_length);
        oops->android_main_len = hdr->android_main_length;
        if (oops->android_main == NULL) {
            IPANIC_DPRINTK("read android_main failed\n");
            goto error_return;
        }
        
        oops->android_radio = emmc_allocate_and_read(hdr->android_radio_offset, hdr->android_radio_length);
        oops->android_radio_len = hdr->android_radio_length;
        if (oops->android_radio == NULL) {
            IPANIC_DPRINTK("read android_radio failed\n");
            goto error_return;
        }               
        
        oops->android_system = emmc_allocate_and_read(hdr->android_system_offset, hdr->android_system_length);
        oops->android_system_len = hdr->android_system_length;
        if (oops->android_system == NULL) {
            IPANIC_DPRINTK("read android_system failed\n");
            goto error_return;
        } 
#endif
        //IPANIC_DPRINTK("ipanic_oops_copy OK\n");
        kfree(hdr);
        return oops;
    }
    else 
    {
        IPANIC_DPRINTK("kmalloc failed at header\n");
        kfree(hdr);
        return NULL;
    }
    
    error_return:
    kfree(hdr);
    ipanic_oops_free(oops);
    return NULL;
}

static void emmc_ipanic_oops_free(struct aee_oops *oops, int erase)
{
    if (oops) 
    {
        ipanic_oops_free(oops);
    }

    if (erase) 
    {
        char *zero = kzalloc(PAGE_SIZE, GFP_KERNEL);
        emmc_ipanic_write(zero, 0, PAGE_SIZE, FILEWRITE);
        kfree(zero);
    }
}

static struct notifier_block panic_blk = {
        .notifier_call        = emmc_ipanic,
};

int ipanic_proc_upload_log(struct seq_file *m, void *v)
{
    char * loginfo = NULL;
    unsigned int length = 0;
    struct aee_oops *oops = NULL;
 
    oops = emmc_ipanic_oops_copy();
    if(!oops)
    {
        IPANIC_DPRINTK("read dump log fail\n");
        return -EIO;
    }

    //write module name
    length = strlen(oops->module);
    seq_write(m, (const void *)oops->module, length);

    //write oops header to file
    loginfo = "\ndump process path\n";
    length = strlen(loginfo);
    seq_write(m, (const void *)loginfo, length); 
    length = strlen(oops->process_path);
    seq_write(m, (const void *)oops->process_path, length);

    loginfo = "\ndump backtrace\n";
    length = strlen(loginfo);
    seq_write(m, (const void *)loginfo, length);    
    length = strlen(oops->backtrace);
    seq_write(m, (const void *)oops->backtrace, length);

    loginfo = "\ndump userspaceinfo\n";
    length = strlen(loginfo);
    seq_write(m, (const void *)loginfo, length);    
    length = oops->userspace_info_len;
    seq_write(m, (const void *)oops->userspace_info, length);
    
    //write kernel message to file
    loginfo = "\ndump kernal message\n";
    length = strlen(loginfo);
    seq_write(m, (const void *)loginfo, length);
    length = oops->console_len;
    seq_write(m, (const void *)oops->console, length);

#ifdef DUMP_ANDROID_LOG
    //write android log to file
    loginfo = "\ndump android log main buffer\n";
    length = strlen(loginfo);
    seq_write(m, (const void *)loginfo, length);
    length = oops->android_main_len;
    seq_write(m, (const void *)oops->android_main, length);

    loginfo = "\ndump android log radio buffer\n";
    length = strlen(loginfo);
    seq_write(m, (const void *)loginfo, length);
    length = oops->android_radio_len;
    seq_write(m, (const void *)oops->android_radio, length);

    loginfo = "\ndump android log system buffer\n";
    length = strlen(loginfo);
    seq_write(m, (const void *)loginfo, length);
    length = oops->android_system_len;
    seq_write(m, (const void *)oops->android_system, length);
#endif    
    emmc_ipanic_oops_free(oops, 0);
    return 0;
}

static int ipanic_proc_upload_open(struct inode *inode, struct file *file)
{
	return single_open(file, ipanic_proc_upload_log, inode->i_private);
}

static const struct file_operations ipanic_upload_fops = {
	.open		= ipanic_proc_upload_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

int ipanic_debug_read(struct seq_file *m, void *v)
{   

    if(FILEWRITE == 2) //write by emmc hal API
    {
        panic("panic for debug");
    }
    else
    {        
        emmc_ipanic(NULL, 0, NULL);        
    }

    return 0;
}

int ipanic_debug_write(struct file *file, const char *buffer, size_t count, loff_t *data)
{    
    int len;
    char buf[2];

    if(count > 2)
	    len = 2;
    else
	    len = count;

    if(copy_from_user(buf, buffer, len))
    {
	    return -EFAULT;
    }

    if(buf[0]=='0')
    {
	    FILEWRITE = 0;        
    }
    else if(buf[0]=='1')
    {
	    FILEWRITE = 1;        
    }
    else
    {
        FILEWRITE = 2;        
    }

    IPANIC_DPRINTK("0: write to emmc by block device\n");   
    IPANIC_DPRINTK("1: write to file by filesystem\n");   
    IPANIC_DPRINTK("2: write to emmc by emmc hal API no schedule\n");
    IPANIC_DPRINTK("FILEWRITE %d\n", FILEWRITE);
    
    return len;
}

static int ipanic_debug_open(struct inode *inode, struct file *file)
{
	return single_open(file, ipanic_debug_read, inode->i_private);
}

static const struct file_operations ipanic_debug_fops = {
	.owner		= THIS_MODULE,
	.open		= ipanic_debug_open,
	.read		= seq_read,
	.write		= ipanic_debug_write,
	.release	= single_release,
};

#define IPANIC_ENTRY_DIR_NAME "ipanic"
#define IPANIC_ENTRY_DEBUG_NAME "debug"
#define IPANIC_ENTRY_UPLOAD_NAME "upload"
static int createIpanicDebugEntry(void)
{
    int ret = 0;

    struct dentry *ipanic_entry_parent = NULL;
    struct dentry *ipanic_debug = NULL;
    struct dentry *ipanic_upload = NULL;
    //add entry dir for debug
    ipanic_entry_parent = debugfs_create_dir(IPANIC_ENTRY_DIR_NAME, NULL);
    if (!ipanic_entry_parent)
    {
        IPANIC_DPRINTK("Unable to create dir %s in debug fs\n", IPANIC_ENTRY_DIR_NAME);
        return -ENODEV;
    }

    ipanic_debug = debugfs_create_file(IPANIC_ENTRY_DEBUG_NAME, 0664, ipanic_entry_parent,
                    NULL, &ipanic_debug_fops);
    if (!ipanic_debug)
    {
        printk(KERN_ERR "ipanic: failed to create debugfs file %s\n", IPANIC_ENTRY_DEBUG_NAME);
        ret = -ENODEV;
        goto CREAT_IPANIC_DEBUG_ERROR;
    }

    ipanic_upload = debugfs_create_file(IPANIC_ENTRY_UPLOAD_NAME, 0664, ipanic_entry_parent,
                    NULL, &ipanic_upload_fops);
    if (!ipanic_upload)
    {
        printk(KERN_ERR "ipanic: failed to create debugfs file %s\n", IPANIC_ENTRY_UPLOAD_NAME);
        ret = -ENODEV;
        goto CREAT_IPANIC_DEBUG_ERROR;
    }

    return ret;

CREAT_IPANIC_DEBUG_ERROR:
    debugfs_remove(ipanic_entry_parent);
    return ret;
}

static void getIpanicMMCDeviceInfo(struct work_struct *work)
{        
    struct file *file;
    struct mstar_ipanic_emmcdevice_info * pipanic_emmcdevice_info = container_of(work, struct mstar_ipanic_emmcdevice_info, asyncwork.work);
    unsigned int length;
    static int try_count = 0;
    ++try_count;

    file = filp_open(EMMC_IPANIC_DEVICE_NAME, O_RDWR, 0);
    if (!IS_ERR(file))
    {
        if(file->f_mapping && file->f_mapping->host && S_ISBLK(file->f_mapping->host->i_mode))
        {
            struct block_device *bdev = I_BDEV(file->f_mapping->host);            
            if(bdev->bd_part)
            {
               pipanic_emmcdevice_info->start_sect = bdev->bd_part->start_sect;
               pipanic_emmcdevice_info->nr_sects = bdev->bd_part->nr_sects;
               printk(KERN_ERR "[Ipanic Driver] ipanic log partition assgined %s: start_sect %llu nr_sects %llu\n", 
                                EMMC_IPANIC_DEVICE_NAME, bdev->bd_part->start_sect, bdev->bd_part->nr_sects);                                             
            }
            else
            {
               IPANIC_DPRINTK("emmc ipanic device partition invalid\n");
               goto GETSECTORINFO_ERROR;
            }
        }
        else
        {
            IPANIC_DPRINTK("emmc file handler is invalid\n");
            goto GETSECTORINFO_ERROR;
        }

        filp_close(file, 0);
    }
    else
    {
        IPANIC_DPRINTK("get ipanic emmc device partition info failed (no device %s)\n", EMMC_IPANIC_DEVICE_NAME); 
        if(try_count <= 3)
            schedule_delayed_work(&ipanic_emmcdevice_info.asyncwork, 2*HZ);
        else
            goto GETSECTORINFO_ERROR;
    }

    //.text&.rodata section CRC calculate
    if((unsigned long)__mstar_fcie_text_start >=  PAGE_OFFSET)
    {
        fcie_module_text_crc = ~(u32)0;
        length = __mstar_fcie_text_end - __mstar_fcie_text_start;
        if(length > 0)
            fcie_module_text_crc = crc32(fcie_module_text_crc, __mstar_fcie_text_start, length);
        else
            goto GETSECTORINFO_ERROR;
        //printk(KERN_ERR "fcie module text section info crc\n");
        //printk(KERN_ERR "start %08x end %08x crc %08x\n", 
        //    (unsigned int)__mstar_fcie_text_start, (unsigned int)__mstar_fcie_text_end, fcie_module_text_crc);

        fcie_module_rodata_crc = ~(u32)0;
        length = __mstar_fcie_rodata_end - __mstar_fcie_rodata_start;
        if(length > 0)
            fcie_module_rodata_crc = crc32(fcie_module_rodata_crc, __mstar_fcie_rodata_start, length);
        else
            goto GETSECTORINFO_ERROR;
        //printk(KERN_ERR "fcie module rodata section info crc\n");
        //printk(KERN_ERR "start %08x end %08x crc %08x\n", 
        //    (unsigned int)__mstar_fcie_rodata_start, (unsigned int)__mstar_fcie_rodata_end, fcie_module_rodata_crc);
    } else {
        printk(KERN_ERR "[Ipanic Driver] fcie .text&.rodata section address invalid\n");
        goto GETSECTORINFO_ERROR;
    }

    return;
    
    GETSECTORINFO_ERROR:    
    atomic_notifier_chain_unregister(&panic_notifier_list, &panic_blk);
}


static void ipanic_kmsg_dump_function(struct kmsg_dumper *dumper,
			    enum kmsg_dump_reason reason)
{	
    memset(&iheader, 0, sizeof(struct ipanic_header));
	iheader.magic = MSTAR_IPANIC_MAGIC;
	iheader.version = MSTAR_IPANIC_PHDR_VERSION;	
   
	kmsg_dump_get_buffer(dumper, true, ipanic_memory.NativeInfo, MAX_NATIVEINFO, &iheader.console_length);
	iheader.console_length=ALIGN(iheader.console_length, EMMC_BLOCK_SIZE);
	iheader.console_offset=ALIGN(sizeof(struct ipanic_header), EMMC_BLOCK_SIZE);
	emmc_ipanic_write(ipanic_memory.NativeInfo, iheader.console_offset,iheader.console_length, FILEWRITE);
}


int __init aee_emmc_ipanic_init(void)
{
    int ret = 0;    
    if(__mstar_fcie_text_start ==  NULL) {
        printk(KERN_ERR "[Ipanic Driver] not support fcie .text&.rodata section crc check, disable this feature\n");
        return ret;
    }
        
    spin_lock_init(&ipanic_lock);
    INIT_DELAYED_WORK(&ipanic_emmcdevice_info.asyncwork, getIpanicMMCDeviceInfo);
    ipanic_emmcdevice_info.start_sect = 0;
    ipanic_emmcdevice_info.nr_sects = 0;
    
    atomic_notifier_chain_register(&panic_notifier_list, &panic_blk);    
    ret = createIpanicDebugEntry();
    if(ret != 0)
    {
        IPANIC_DPRINTK("create ipanic entry fail\n");
        goto CREAT_IENTRY_FAIL;
    }
   
    schedule_delayed_work(&ipanic_emmcdevice_info.asyncwork, 2*HZ);
  
    ipanic_kmsg_dumper.dump = ipanic_kmsg_dump_function;
    /*ipanic_kmsg_dumper.max_reason = KMSG_DUMP_PANIC;*/
    kmsg_dump_register(&ipanic_kmsg_dumper);
    
    return ret;
  
CREAT_IENTRY_FAIL:
    atomic_notifier_chain_unregister(&panic_notifier_list, &panic_blk);
    return ret;
}

module_init(aee_emmc_ipanic_init);
