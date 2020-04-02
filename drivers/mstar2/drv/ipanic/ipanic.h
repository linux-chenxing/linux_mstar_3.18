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
/// @file   ipanic.h
/// @brief  dump all info into emmc device when kernel panic
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef _DRV_IPANIC_H_
#define _DRV_IPANIC_H_

#include <linux/autoconf.h>
#include <linux/kallsyms.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <logger.h>

#define IPANIC_DEBUG 1
#define EMMC_IPANIC_DEVICE_NAME  "/dev/block/platform/mstar_mci.0/by-name/ipanic"
#define IPANIC_MODULE_TAG "KERNEL-PANIC"
//#define DUMP_ANDROID_LOG 1  //andorid 5.1 and after, android log can not dump in kernel

#define IPANIC_OOPS_BLOCK_COUNT 320
#define MSTAR_IPANIC_MAGIC 0xaee0dead
#define MSTAR_IPANIC_PHDR_VERSION 0x01
#define IPANIC_MODULE_NAME_LENGTH 64
#define IPANIC_OOPS_HEADER_PROCESS_NAME_LENGTH 512
#define IPANIC_OOPS_HEADER_BACKTRACE_LENGTH 4096
#define IPANIC_DATALENGTH_MAX (IPANIC_OOPS_HEADER_PROCESS_NAME_LENGTH + IPANIC_OOPS_HEADER_BACKTRACE_LENGTH)

#define EMMC_BLOCK_SIZE 512
#define MAX_NATIVEINFO 32*1024
//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------
#ifdef IPANIC_DEBUG
#define IPANIC_DPRINTK(fmt, args...)      printk(KERN_ERR "[Ipanic Driver][%s %d] " fmt, __FUNCTION__,__LINE__, ## args)
#else
#define IPANIC_DPRINTK(fmt, args...)
#endif

struct mstar_ipanic_emmcdevice_info
{
    volatile u64    start_sect;
    volatile u64    nr_sects;
    struct delayed_work  asyncwork;
};

struct aee_oops
{
    char module[IPANIC_MODULE_NAME_LENGTH];
    char process_path[IPANIC_OOPS_HEADER_PROCESS_NAME_LENGTH];
    char backtrace[IPANIC_OOPS_HEADER_BACKTRACE_LENGTH];

    char *userspace_info;
    int userspace_info_len;

    char *console;
    int console_len;

#ifdef DUMP_ANDROID_LOG
    char *android_main;
    int android_main_len;
    char *android_radio;
    int android_radio_len;
    char *android_system;
    int android_system_len;
#endif
    
    int dump_option;
};

struct ipanic_header {
        /* The magic/version field cannot be moved or resize */
        u32 magic;
        u32 version;

        u32 console_offset;
        u32 console_length;
		
        u32 oops_header_offset;
        u32 oops_header_length;
		
        u32 userspace_info_offset;
        u32 userspace_info_length;

#ifdef DUMP_ANDROID_LOG
        u32 android_main_offset;
        u32 android_main_length;
        
        u32 android_event_offset;
        u32 android_event_length;
        
        u32 android_radio_offset;
        u32 android_radio_length;
        
        u32 android_system_offset;
        u32 android_system_length;
#endif
};

struct ipanic_oops_header
{
    char process_path[IPANIC_OOPS_HEADER_PROCESS_NAME_LENGTH];
    char backtrace[IPANIC_OOPS_HEADER_BACKTRACE_LENGTH];
}__attribute__  ((aligned (512)));

union IPANIC_MEM {
    struct ipanic_oops_header oops_header;
    char NativeInfo[MAX_NATIVEINFO];  //check that 32k is enought 
    char emmc_bounce[2*PAGE_SIZE];
};

typedef enum android_LogPriority {  
    ANDROID_LOG_UNKNOWN = 0,  
    ANDROID_LOG_DEFAULT,    
    ANDROID_LOG_VERBOSE,  
    ANDROID_LOG_DEBUG,  
    ANDROID_LOG_INFO,  
    ANDROID_LOG_WARN,  
    ANDROID_LOG_ERROR,  
    ANDROID_LOG_FATAL,  
    ANDROID_LOG_SILENT, 
} android_LogPriority;  

typedef struct AndroidLogEntry_t {
    time_t tv_sec;
    long tv_nsec;
    android_LogPriority priority;
    int32_t pid;
    int32_t tid;
    const char * tag;
    size_t messageLen;
    const char * message;
} AndroidLogEntry;

struct aee_oops *ipanic_oops_create(const char *module);
void ipanic_oops_set_backtrace(struct aee_oops *oops, const char *backtrace);
void ipanic_oops_set_process_path(struct aee_oops *oops, const char *process_path);
void ipanic_oops_free(struct aee_oops *oops);
//extern int log_buf_copy2(char *dest, int dest_len, int log_copy_start, int log_copy_end);

/*
* Check if valid header is legitimate
* return
* 0: contain good panic data
* 1: no panic data
* 2: contain bad panic data
*/
int ipanic_header_check(const struct ipanic_header *hdr);

void ipanic_header_dump(const struct ipanic_header *header);

void ipanic_block_scramble(u8 *buf, int buflen);

void ipanic_oops_start(void);

/* User space process support functions */

int DumpNativeInfo(void);

/* External ipanic support functions */

int card_dump_func_read(unsigned char* buf, unsigned int len, unsigned int offset);

int card_dump_func_write(unsigned char* buf, unsigned int len, unsigned int offset, int filewrite);

int panic_dump_android_log(struct file *fp, char *buf, unsigned int interBufWriteOff,size_t dataLen, int type, size_t sparesize);
#endif
