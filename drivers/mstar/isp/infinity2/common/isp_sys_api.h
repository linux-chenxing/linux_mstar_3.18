#ifndef ISP_SYS_API_H
#define ISP_SYS_API_H
#include <isp_porting_cfg.h>

#if _OS_SEL_ == _LINUX_KERNEL_
#include <delay.h>
#define usleep(a) udelay(a)
#elif _OS_SEL_ == _RTK_
#endif

void* IspSysGetRegBase(void);

void* IspSysMalloc(int size);
void* IspSysCalloc(int num,int size);
void IspSysFree(void* ptr);
void show_malloc_dbg_info(void);

#if _OS_SEL_ == _LINUX_KERNEL_
#define BANK_TO_ADDR32(b) (b<<9)
#define BANK_BASE_ADDR(bank) (((unsigned long)IspSysGetRegBase())+BANK_TO_ADDR32(bank))
#endif

#endif
