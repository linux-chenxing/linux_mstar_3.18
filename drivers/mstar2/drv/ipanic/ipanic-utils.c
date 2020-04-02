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
///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   ipanic-utils.h
/// @brief  dump all info into emmc device when kernel panic
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////


#include <linux/module.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/stacktrace.h>
#include <linux/slab.h>
#include "ipanic.h"
#include "../mmc/inc/common/eMMC.h"

unsigned long ipanic_stack_entries[64];
union IPANIC_MEM  ipanic_memory;

extern struct mstar_ipanic_emmcdevice_info ipanic_emmcdevice_info;
#define KERNEL_PANIC_DUMP_PATH "/data/app/kernel_panic.txt"
#define MAX_STACK_TRACE_DEPTH 64

void ipanic_oops_free(struct aee_oops *oops)
{
    if (oops->console) {
        kfree(oops->console);
    }
    
#ifdef DUMP_ANDROID_LOG    
    if (oops->android_main) {
        kfree (oops->android_main);
    }
    if (oops->android_radio) {
        kfree (oops->android_radio);
    }
    if (oops->android_system) {
        kfree (oops->android_system);
    }
#endif

    if (oops->userspace_info) {
        kfree (oops->userspace_info);
    }
    kfree(oops);
}

struct aee_oops *ipanic_oops_create(const char *module)
{
    struct aee_oops *oops = kzalloc(sizeof(struct aee_oops), GFP_KERNEL | GFP_ATOMIC);

    if (module != NULL) {
        int ret = strlcpy(oops->module, module, sizeof(oops->module));
        oops->module[ret] = '\n';
    }
    else {
        strcpy(oops->module, "N/A");
    }
    strcpy(oops->backtrace, "N/A");
    strcpy(oops->process_path, "N/A");
    
    return oops;
}

void ipanic_oops_set_process_path(struct aee_oops *oops, const char *process_path)
{
    if (process_path != NULL) {
        strlcpy(oops->process_path, process_path, sizeof(oops->process_path));
    }
}

void ipanic_oops_set_backtrace(struct aee_oops *oops, const char *backtrace)
{
    if (backtrace != NULL) {
        strlcpy(oops->backtrace, backtrace, sizeof(oops->backtrace));
    }
}

#if 0
void ipanic_block_scramble(u8 *buf, int buflen)
{
    int i;
    u32 *p = (u32 *)buf;
    for (i = 0; i < buflen; i += 4, p++) {
        *p = *p ^ ipanic_iv;
    }
}
#endif

/*
* Check if valid header is legitimate
* return
* 0: contain good panic data
* 1: no panic data
* 2: contain bad panic data
*/
int ipanic_header_check(const struct ipanic_header *hdr)
{
    if (hdr->magic != MSTAR_IPANIC_MAGIC) {
        IPANIC_DPRINTK("ipanic: No panic data available[Magic header]\n");
        return 1;
    }

    if (hdr->version != MSTAR_IPANIC_PHDR_VERSION) {
        IPANIC_DPRINTK("ipanic: Version mismatch (%d != %d)\n",
        hdr->version, MSTAR_IPANIC_PHDR_VERSION);
        return 2;
    }

    if ((hdr->oops_header_length < 16) || (hdr->oops_header_length > IPANIC_DATALENGTH_MAX)) {
        IPANIC_DPRINTK("ipanic: No panic data available [Invalid oops header length - %d]\n", hdr->oops_header_length);
        return 2;
    }

    if ((hdr->console_length < 16) || (hdr->console_length > 128 * 1024)) {
        IPANIC_DPRINTK("ipanic: No panic data available [Invalid oops console length - %d]\n", hdr->console_length);
        return 2;
    }
    return 0;
}

void ipanic_header_dump(const struct ipanic_header *hdr)
{
    IPANIC_DPRINTK("ipanic: magic(%x) version(%d)\n", hdr->magic, hdr->version);
    IPANIC_DPRINTK("\theader(%u, %u) console(%u, %u)\n",
             hdr->oops_header_offset, hdr->oops_header_length,             
             hdr->console_offset, hdr->console_length);
    IPANIC_DPRINTK("\tprocess(%u, %u)\n",
             hdr->userspace_info_offset, hdr->userspace_info_length);
    
#ifdef DUMP_ANDROID_LOG     
    IPANIC_DPRINTK("\tandroid main(%u, %u), android system(%u, %u), android radio(%u, %u)\n",
             hdr->android_main_offset, hdr->android_main_length,
             hdr->android_system_offset, hdr->android_system_length,
             hdr->android_radio_offset, hdr->android_radio_length);
#endif
}

void ipanic_oops_start()
{
    struct stack_trace trace;
    struct task_struct *tsk = get_current();
    int i, plen;

    memset(&ipanic_memory.oops_header, 0, sizeof(struct ipanic_oops_header));

    //IPANIC_DPRINTK("enter ipanic_oops_start\n");    
    /* Grab kernel task stack trace */
    trace.nr_entries    = 0;
    trace.max_entries   = MAX_STACK_TRACE_DEPTH;
    trace.entries       = ipanic_stack_entries;
    trace.skip          = 0;
    save_stack_trace_tsk(tsk, &trace);

    /* Skip the entries - ipanic_oops_start/save_stack_trace_tsk */
    for (i = 2; i < trace.nr_entries; i++) {
        int off = strlen(ipanic_memory.oops_header.backtrace);
        int plen = IPANIC_OOPS_HEADER_BACKTRACE_LENGTH - off;
        if (plen > 16) {
            snprintf(ipanic_memory.oops_header.backtrace + off, plen, "[<%p>] %pS\n",
                     (void *)ipanic_stack_entries[i], (void *)ipanic_stack_entries[i]);                     
        }
    }
    
    /* Current panic user tasks */
    plen = 0;
    while (tsk && (tsk->pid != 0) && (tsk->pid != 1))
    {
        /* FIXME: Check overflow ? */
        plen += sprintf(ipanic_memory.oops_header.process_path + plen, "[%s, %d]", tsk->comm, tsk->pid);
        tsk = tsk->real_parent;
    }

    if(plen+1 <= IPANIC_OOPS_HEADER_PROCESS_NAME_LENGTH)
        sprintf(ipanic_memory.oops_header.process_path + plen, "\n");

    //IPANIC_DPRINTK("end of ipanic_oops_start\n");    
}

int card_dump_func_read(unsigned char* buf, unsigned int len, unsigned int offset)
{
    int ret = 0;
    mm_segment_t old_fs;
    struct file *fp = NULL;
    
    fp = filp_open(EMMC_IPANIC_DEVICE_NAME, O_RDWR, 0);    

    if(!IS_ERR(fp))
    {
        if(fp->f_op->llseek) 
        {            
            ret = fp->f_op->llseek(fp, offset, SEEK_SET);
            if(ret < 0)
            {
                IPANIC_DPRINTK("%s f_op llseek  offset %d fail\n", EMMC_IPANIC_DEVICE_NAME, offset);
                return 0;
            }
        }
        else
        {
            IPANIC_DPRINTK("%s  no llseek f_op\n", EMMC_IPANIC_DEVICE_NAME);
            return 0;        
        }
        
        old_fs = get_fs();        
        set_fs(KERNEL_DS);        
        ret = fp->f_op->read(fp, buf, len, &fp->f_pos);        
        set_fs(old_fs); 
        filp_close(fp, NULL);
    }
    else
    {
        IPANIC_DPRINTK("open %s fail\n", EMMC_IPANIC_DEVICE_NAME);
    }
    
    return ret;
}

int card_dump_func_write(unsigned char* buf, unsigned int len, unsigned int offset, int filewrite)
{
    int ret = 0; 
    mm_segment_t old_fs;
    struct file *fp = NULL;
    char * filepath = NULL;
    int flag = 0, mode = 0;    
    
    if(filewrite == 2)      //write to emmc dev by emmc hal API
    {
        int rem = 0; 
        unsigned long eMMCBlkAddr = 0; 
        unsigned long eMMCBlkCount = ipanic_emmcdevice_info.nr_sects;
        int blkCount = 0; 

        if(ipanic_emmcdevice_info.start_sect == 0)
        {
            IPANIC_DPRINTK("ipanic_emmcdevice_info.start_sect == 0\n");
            return 0;
        }
        rem = len & (EMMC_BLOCK_SIZE - 1); 
        BUG_ON(rem);
        rem = offset & (EMMC_BLOCK_SIZE - 1);
        BUG_ON(rem);

        eMMCBlkAddr = ipanic_emmcdevice_info.start_sect;
        blkCount = (len >> eMMC_SECTOR_512BYTE_BITS);
        eMMCBlkAddr += (offset >> eMMC_SECTOR_512BYTE_BITS);

        if(eMMCBlkAddr+blkCount-ipanic_emmcdevice_info.start_sect <= eMMCBlkCount)
        {
            int saved_oip = oops_in_progress;
            oops_in_progress = 1;
            //ret = eMMC_CMD25(eMMCBlkAddr, buf, blkCount);
            ret = eMMC_CMD25_NoSched(eMMCBlkAddr, buf, blkCount);
            oops_in_progress = saved_oip;

            if(ret != 0)
            {
               IPANIC_DPRINTK("%s write to emmc device error %d\n", __FUNCTION__, ret);
               return 0;
            }
        }
        else
        {
            IPANIC_DPRINTK("%s write to emmc device overflow\n", __FUNCTION__);
            return 0;
        }
        
        return len;
    }
    else if(filewrite == 0) // write to emmc dev by block device
    {
        filepath = EMMC_IPANIC_DEVICE_NAME;
        flag = O_RDWR;
        mode = 0;
    }
    else if(filewrite == 1) // write to file by filesystem 
    {
        filepath = KERNEL_PANIC_DUMP_PATH;
        flag = O_RDWR|O_APPEND|O_CREAT;
        mode = 0644;    
    }
    else
    {
        IPANIC_DPRINTK("%s not support filewrite %d\n", __FUNCTION__, filewrite);
        return 0;
    }

    fp = filp_open(filepath, flag, mode);       
    //printk(KERN_ERR "enter %s %d offset %d\n", __FUNCTION__, __LINE__, offset); 
    
    if(!IS_ERR(fp))
    {        
        if(!fp->f_op || !fp->f_op->llseek || !fp->f_op->write)
        {
            IPANIC_DPRINTK("f_op is null\n");
            filp_close(fp, NULL);
            return 0;
        }

        if(filewrite == 1) //write to file
        {
            ret = fp->f_op->llseek(fp, 0, SEEK_END); //offset
    
        }
        else
        {
            ret = fp->f_op->llseek(fp, offset, SEEK_SET);
        }       
                
        if(ret < 0)
        {            
            IPANIC_DPRINTK("%s f_op llseek fail %d\n", filepath, offset);
            return 0;
        } 
        
        old_fs = get_fs();        
        set_fs(KERNEL_DS);
        ret = fp->f_op->write(fp, buf, len, &fp->f_pos);
        set_fs(old_fs);       
        vfs_fsync(fp, 0);
        if(ret <= 0)        
            IPANIC_DPRINTK("write message to %s fail\n", filepath);
        filp_close(fp, NULL);
    }
    else
    {
        IPANIC_DPRINTK("open %s fail\n", filepath);
    }
    
    return ret;
}

#ifdef DUMP_ANDROID_LOG
int android_log_processLogBuffer(struct user_logger_entry_compat *buf,
                                 AndroidLogEntry *entry)
{
    int msgStart = -1;
    int msgEnd = -1;
    int i;

    entry->tv_sec = buf->sec;
    entry->tv_nsec = buf->nsec;
    entry->pid = buf->pid;
    entry->tid = buf->tid;

    /*
     * format: <priority:1><tag:N>\0<message:N>\0
     *
     * tag str
     *   starts at buf->msg+1
     * msg
     *   starts at buf->msg+1+len(tag)+1
     *
     * The message may have been truncated by the kernel log driver.
     * When that happens, we must null-terminate the message ourselves.
     */
    if (buf->len < 3) {
        // An well-formed entry must consist of at least a priority
        // and two null characters
        IPANIC_DPRINTK("+++ LOG: entry too small\n");
        return -1;
    }
    
    for (i = 1; i < buf->len; i++) {
        if (buf->msg[i] == '\0') {
            if (msgStart == -1) {
                msgStart = i + 1;
            } else {
                msgEnd = i;
                break;
            }
        }
    }

    if (msgStart == -1) {
        IPANIC_DPRINTK("+++ LOG: malformed log message\n");
        return -1;
    }
    if (msgEnd == -1) {
        // incoming message not null-terminated; force it
        msgEnd = buf->len - 1;
        buf->msg[msgEnd] = '\0';
    }

    entry->priority = buf->msg[0];
    entry->tag = buf->msg + 1;
    entry->message = buf->msg + msgStart;
    entry->messageLen = msgEnd - msgStart;

    return 0;
}

static char filterPriToChar (android_LogPriority pri)
{
    switch (pri) {
        case ANDROID_LOG_VERBOSE:       return 'V';
        case ANDROID_LOG_DEBUG:         return 'D';
        case ANDROID_LOG_INFO:          return 'I';
        case ANDROID_LOG_WARN:          return 'W';
        case ANDROID_LOG_ERROR:         return 'E';
        case ANDROID_LOG_FATAL:         return 'F';
        case ANDROID_LOG_SILENT:        return 'S';

        case ANDROID_LOG_DEFAULT:
        case ANDROID_LOG_UNKNOWN:
        default:                        return '?';
    }
}

/*
 * The maximum size of a log entry which can be read from the
 * kernel logger driver. An attempt to read less than this amount
 * may result in read() returning EINVAL.
 */
#define LOGGER_PREFIX_MAX_LEN   50
#define LOGGER_ENTRYS_MAX_LEN   4096
char ENTRYS_BUFF[LOGGER_ENTRYS_MAX_LEN];

int panic_dump_android_log(struct file *fp, char *buf, unsigned int interBufWriteOff,size_t dataLen, int type, size_t sparesize)
{
    long ret = 0; 
    char * debug = NULL;
    char priChar;
    int bufWriteSize = interBufWriteOff;   
    unsigned long prefixLen = 0;
    mm_segment_t old_fs;
    AndroidLogEntry entry;    
    
    struct user_logger_entry_compat * log_entry = (struct user_logger_entry_compat *)ENTRYS_BUFF;
    
    if(fp && fp->f_op->read && fp->f_op->unlocked_ioctl)
    {  
        old_fs = get_fs();      
        set_fs(KERNEL_DS);        
        
        while(1)
        {
            if(bufWriteSize >= PAGE_SIZE)
                break; 
            
            ret = fp->f_op->unlocked_ioctl(fp, LOGGER_GET_NEXT_ENTRY_LEN, 0);
            if(ret <= 0)
                break;

            ret = fp->f_op->read(fp, ENTRYS_BUFF, LOGGER_ENTRYS_MAX_LEN, &fp->f_pos);      
            if(ret && log_entry->len)
            {
                //log_entry->msg[log_entry->len] = '\0';                          
                priChar = filterPriToChar(entry.priority);
                ret = android_log_processLogBuffer((struct user_logger_entry_compat *)ENTRYS_BUFF, &entry);
                if(ret == 0)
                {
                    debug = buf+bufWriteSize;                   
                    prefixLen = snprintf(buf+bufWriteSize, sparesize,
                    "%010ld.%03ld %c/%-8s(%5d): ", entry.tv_sec, entry.tv_nsec / 1000000,
                    priChar, entry.tag, entry.pid);
                    sparesize -= prefixLen;
                    bufWriteSize += prefixLen;

                    memcpy(buf+bufWriteSize, entry.message, entry.messageLen);
                    sparesize -= entry.messageLen;
                    bufWriteSize += entry.messageLen;

                    //force terminate with '\n'
                    if(entry.message[entry.messageLen-1] != '\n')
                    {
                        buf[bufWriteSize] = '\n';
                        sparesize -= 1;
                        bufWriteSize += 1;
                    }

                    //IPANIC_DPRINTK("one log entry: %s\n", debug);
                }                
            }
        }

        set_fs(old_fs);         
        //IPANIC_DPRINTK("prinLogLength %d sparesize %d\n", prinLogLength, sparesize);        
    }
    else
    {
        IPANIC_DPRINTK("panic_dump_android_log fp is null\n");
    }

    return bufWriteSize;
}

int ipanic_print2stdout(unsigned char* buf, unsigned int len)
{
   char * front_ptr = buf;
   char * back_ptr = buf;
   char savech = 0;
   bool print_enable = true;
   unsigned int distance = 0;
   
   if(len <= 0)
      return 0;

   //force terminal char is '\n'
   if(buf[len-1] != '\n')
   {
      buf[len-1] = '\n';      
      IPANIC_DPRINTK("force terminal char is new line\n");
   }   

   while(print_enable)
   {  
      distance = (unsigned int)front_ptr - (unsigned int)buf;
      if(distance >= len)
        break;
   
      if(front_ptr[0] == '\n')
      {
        printk(KERN_ERR "\n");
        front_ptr = &front_ptr[1];
        back_ptr = front_ptr;
        continue;
      }        
   
      back_ptr = strchr(front_ptr, '\n');
      back_ptr += 1;  //move to the char after '\n'
      distance = (unsigned int)back_ptr - (unsigned int)buf;
      //=len, buffer size is bigger than len at least one bytes
      if(distance <= len)
      {
        savech = buf[distance];
        buf[distance] = '\0';
      }
      else
      {
        IPANIC_DPRINTK("should not been here\n");
        break; 
      }
   
      printk(KERN_ERR "%s", front_ptr);
      buf[distance] = savech;
      front_ptr = back_ptr;
   }

   return len;
}
#endif

