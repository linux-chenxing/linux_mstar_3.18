#include <isp_sys_api.h>
#include <cam_os_wrapper.h>

#if(_OS_SEL_ == _LINUX_KERNEL_)
#include <io.h>
#endif

void* IspSysGetRegBase()
{
    static unsigned long uRegBase = 0;
#if(_OS_SEL_ == _LINUX_USER_)
#elif(_OS_SEL_ == _LINUX_KERNEL_)
    if(!uRegBase)
    {
        uRegBase = (unsigned long)ioremap(0x1F000000,0x400000);
    }
#elif (_OS_SEL_ == _RTK_)
#endif

    return (void*)uRegBase;
}

int g_malloc_count = 0;
void* IspSysMalloc(int size)
{
    void* ptr = CamOsMemAlloc(size);
    if(ptr)
        g_malloc_count++;
    return ptr;
}

void* IspSysCalloc(int num,int size)
{
    void* ptr = CamOsMemCalloc(num,size);
    if(ptr)
        g_malloc_count++;
    return ptr;
}

void IspSysFree(void* ptr)
{
    g_malloc_count--;
    CamOsMemRelease(ptr);
}

void show_malloc_dbg_info(void)
{
    #if(_OS_SEL_ == _LINUX_USER_)
    printf("malloc count = %d\n",g_malloc_count);
    #elif(_OS_SEL_ == _LINUX_KERNEL_)
    #elif (_OS_SEL_ == _RTK_)
    UartSendTrace("malloc count = %d\n",g_malloc_count);
    #endif
}

#if 0
int g_noncache_malloc_count = 0;
void* IspSysMallocNonCache(int size)
{
    #if(_OS_SEL_ == _LINUX_USER_)
    void* ptr = malloc(size);
    #elif(_OS_SEL_ == _LINUX_KERNEL_)
    #elif(_OS_SEL_ == _RTK_)
    void* ptr = MsAllocateNonCacheMem(size);
    #endif
    if(ptr)
        g_noncache_malloc_count++;
    return ptr;
}

void IspSysNonCacheFree(void* ptr)
{
    g_noncache_malloc_count--;
    //TODO: free non-cached memory
    //CamOsMemRelease(ptr);
}
#endif
