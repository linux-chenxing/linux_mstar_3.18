///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2006 - 2009 MStar Semiconductor, Inc.
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
/// file    mdrv_mpool.c
/// @brief  Memory Pool Control Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
//#include "MsCommon.h"
#include <linux/autoconf.h>
//#include <linux/undefconf.h> //unused header file now
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
#include <linux/time.h>  //added
#include <linux/timer.h> //added
#include <linux/device.h>
#include <linux/version.h>
#include <asm/io.h>
#include <asm/types.h>
#include <asm/cacheflush.h>
#if defined(CONFIG_COMPAT)
#include <linux/compat.h>
#endif

#ifdef CONFIG_HAVE_HW_BREAKPOINT
#include <linux/hw_breakpoint.h>
#endif

#if defined(CONFIG_MIPS)
#include <asm/mips-boards/prom.h>
#include "mdrv_cache.h"
#elif defined(CONFIG_ARM) || defined(CONFIG_ARM64)
#endif

#include "mst_devid.h"
#include "mdrv_mpool.h"
#include "mdrv_types.h"
#include "mst_platform.h"
#include "mdrv_system.h"
#include "mdrv_mstypes.h"

#if !(defined(CONFIG_MSTAR_TITANIA3) || defined(CONFIG_MSTAR_TITANIA10) )
#include "chip_setup.h"
#endif

#if defined(CONFIG_MIPS)
#include  <asm/tlbmisc.h>
#endif

#include <mdrv_dlmalloc.h>
#define DLMALLOC_LOG KERN_DEBUG
//#define DLMALLOC_LOG // if want to print_log, define as NOTHING

//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------
//#define MPOOL_DPRINTK(fmt, args...) printk(KERN_WARNING"%s:%d " fmt,__FUNCTION__,__LINE__,## args)
#define MPOOL_DPRINTK(fmt, args...)

//-------------------------------------------------------------------------------------------------
//  Global variables
//-------------------------------------------------------------------------------------------------
#if defined(CONFIG_MIPS)
unsigned int bMPoolInit = 0;
#endif
//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define MPOOL_VERSION 1
#define MOD_MPOOL_DEVICE_COUNT     1
#define MOD_MPOOL_NAME             "malloc"

#define KER_CACHEMODE_UNCACHE_NONBUFFERABLE 0
#define KER_CACHEMODE_CACHE   1
#define KER_CACHEMODE_UNCACHE_BUFFERABLE 2

#ifdef MP_MMAP_MMAP_BOUNDARY_PROTECT
#define KER_CACHEMODE_TRAP                          3

#define KER_CACHEMODE_UNCACHE_NONBUFFERABLE_PROTECT 4
#define KER_CACHEMODE_CACHE_PROTECT                 5
#define KER_CACHEMODE_UNCACHE_BUFFERABLE_PROTECT    6
#endif


// Define MPOOL Device
U32 linux_base;
U32 linux_size;
U32 linux2_base;
U32 linux2_size;
U32 emac_base;
U32 emac_size;

#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
typedef  struct
{
   phys_addr_t mpool_base;
   U32 mpool_size;
   phys_addr_t mmap_offset;
   U32 mmap_size;
   U32 mmap_interval;
   U8  mmap_miusel;
   unsigned int u8MapCached;
   bool setflag;
}MMAP_FileData;
#elif defined(CONFIG_ARM64)
typedef  struct
{
   u64 mpool_base;
   u64 mpool_size;
   u64 mmap_offset;
   u64 mmap_size;
   u64 mmap_interval;
   U8  mmap_miusel;
   unsigned int u8MapCached;
   bool setflag;
}MMAP_FileData;
#endif

#define DLMALLOC_CNT 100
typedef struct
{
	void *initted_bus_addr;						// buffer start ba
	void *initted_memorypool_bus_addr;			// memory pool start ba
	void *initted_va;							// buffer start va
	void __iomem *initted_memorypool_va;		// memory pool start va
	size_t initted_size;
	unsigned int ref_cnt;
} DLMALLOC_Init_History;

DLMALLOC_Init_History dlmalloc_init_check[DLMALLOC_CNT];
static DEFINE_MUTEX(dlmalloc_op_mutex);

//bool setflag;
struct mutex mpool_iomap_mutex = __MUTEX_INITIALIZER(mpool_iomap_mutex);

unsigned int mpool_version = MPOOL_VERSION;
#if defined(CONFIG_MSTAR_TITANIA3) || \
    defined(CONFIG_MSTAR_TITANIA10) || \
    defined(CONFIG_MSTAR_TITANIA8) || \
    defined(CONFIG_MSTAR_TITANIA12) || \
    defined(CONFIG_MSTAR_TITANIA9) || \
    defined(CONFIG_MSTAR_JANUS2) || \
    defined(CONFIG_MSTAR_KRONUS) || \
    defined(CONFIG_MSTAR_KELTIC) || \
    defined(CONFIG_MSTAR_TITANIA11) || \
    defined(CONFIG_MSTAR_TITANIA13) || \
    defined(CONFIG_MSTAR_AMBER1) || \
    defined(CONFIG_MSTAR_AMBER2) || \
    defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_AMBER6) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST) || \
    defined(CONFIG_MSTAR_KAISERIN) || \
    defined(CONFIG_MSTAR_AGATE)    || \
    defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EMERALD) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EIFFEL) || \
    defined(CONFIG_MSTAR_NIKE) || \
    defined(CONFIG_MSTAR_MADISON) || \
	defined(CONFIG_MSTAR_CLIPPERS) || \
    defined(CONFIG_MSTAR_MIAMI) || \
    defined(CONFIG_MSTAR_MONACO) || \
    defined(CONFIG_MSTAR_MALDIVES) || \
    defined(CONFIG_MSTAR_MACAN) || \
    defined(CONFIG_MSTAR_MUSTANG) || \
    defined(CONFIG_MSTAR_MESSI) || \
    defined(CONFIG_MSTAR_K6Lite) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_NAPOLI) || \
    defined(CONFIG_MSTAR_KAISER) || \
    defined(CONFIG_MSTAR_NUGGET) || \
	defined(CONFIG_MSTAR_NIKON) || \
    defined(CONFIG_MSTAR_KAISERS) || \
    defined(CONFIG_MSTAR_KENYA)   || \
    defined(CONFIG_MSTAR_KERES) || \
    defined(CONFIG_MSTAR_MUJI) || \
    defined(CONFIG_MSTAR_MONET) || \
    defined(CONFIG_MSTAR_KANO) || \
    defined(CONFIG_MSTAR_K6) || \
    defined(CONFIG_MSTAR_MANHATTAN) || \
    defined(CONFIG_MSTAR_CURRY) || \
    defined(CONFIG_MSTAR_C2P) || \
    defined(CONFIG_MSTAR_MASERATI) || \
    defined(CONFIG_MSTAR_MOONEY)

    #define HAL_MIU0_LEN 0x10000000
#elif defined(CONFIG_MSTAR_KERES) || \
      defined(CONFIG_MSTAR_KIRIN) || \
	  defined(CONFIG_MSTAR_KRIS) || \
	  defined(CONFIG_MSTAR_KAYLA) || \
	  defined(CONFIG_MSTAR_KIWI)
    #define HAL_MIU0_LEN 0x40000000
#else
    #define HAL_MIU0_LEN 0x08000000
#endif

//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------;

typedef struct
{
    int                         s32MPoolMajor;
    int                         s32MPoolMinor;
    void*                       dmaBuf;
    struct cdev                 cDevice;
    struct file_operations      MPoolFop;
} MPoolModHandle;

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// Forward declaration
//--------------------------------------------------------------------------------------------------
static int                      _MDrv_MPOOL_Open (struct inode *inode, struct file *filp);
static int                      _MDrv_MPOOL_Release(struct inode *inode, struct file *filp);
static int                      _MDrv_MPOOL_MMap(struct file *filp, struct vm_area_struct *vma);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
static long                     _MDrv_MPOOL_Ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
#else
static int                      _MDrv_MPOOL_Ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
#endif


#if defined(CONFIG_COMPAT)
static long Compat_MDrv_MPOOL_Ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
#endif

//Add by austin, remove linux page from pfn range
static inline int mpool_io_remap_range(struct vm_area_struct *vma, unsigned long addr,
                unsigned long pfn, unsigned long size, pgprot_t prot);

//-------------------------------------------------------------------------------------------------
// Local Variables
//-------------------------------------------------------------------------------------------------

static struct class *mpool_class;

static MPoolModHandle MPoolDev=
{
    .s32MPoolMajor=               MDRV_MAJOR_MPOOL,
    .s32MPoolMinor=               MDRV_MINOR_MPOOL,
    .cDevice=
    {
        .kobj=                  {.name= MOD_MPOOL_NAME, },
        .owner  =               THIS_MODULE,
    },
    .MPoolFop=
    {
        .open=                  _MDrv_MPOOL_Open,
        .release=               _MDrv_MPOOL_Release,
        .mmap=                  _MDrv_MPOOL_MMap,
        #if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
        .unlocked_ioctl=        _MDrv_MPOOL_Ioctl,
        #else
        .ioctl =  		_MDrv_MPOOL_Ioctl,
        #endif
		#if defined(CONFIG_COMPAT)
		.compat_ioctl = Compat_MDrv_MPOOL_Ioctl,
		#endif
    },
};



//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static inline int mpool_io_remap_range(struct vm_area_struct *vma, unsigned long addr,
		    unsigned long pfn, unsigned long size, pgprot_t prot)
{
	unsigned long end = addr + PAGE_ALIGN(size);
	int err;
    vma->vm_flags |= VM_PFNMAP;

    do {
		/* pfn_valid(pfn) means the page is in linux memory
		 * we will also map linux memory to user_space
		 */
        //if(!pfn_valid(pfn))
		{
    		err = vm_insert_pfn(vma, addr, pfn);
    		if (err)
    			break;
        }
    }while(pfn++, addr += PAGE_SIZE, addr != end);

    return 0;
}

static int _MDrv_MPOOL_Open (struct inode *inode, struct file *filp)
{
    MMAP_FileData *mmapData;

    mmapData = kzalloc(sizeof(*mmapData), GFP_KERNEL);
    if (mmapData == NULL)
          return -ENOMEM;

    filp->private_data = mmapData;
    mmapData->u8MapCached = 1;
    mmapData->setflag = false;

    return 0;
}

static int _MDrv_MPOOL_Release(struct inode *inode, struct file *filp)
{
    MMAP_FileData *mmapData = filp->private_data ;
    kfree(mmapData);

    // iounmap(dev->dmaBuf) ;
    return 0;
}


static int _MDrv_MPOOL_MMap(struct file *filp, struct vm_area_struct *vma)
{
    MMAP_FileData *mmapData = filp->private_data;
    u32 miu0_len = 0x10000000;
	phys_addr_t BUS_BASE = 0;

    mutex_lock(&mpool_iomap_mutex);
    if(!mmapData->setflag)
		vma->vm_pgoff = mmapData->mpool_base >> PAGE_SHIFT;
    else
    {
    #if defined(CONFIG_MIPS)
        if(mmapData->mmap_miusel == 0)
            vma->vm_pgoff = mmapData->mmap_offset >> PAGE_SHIFT;
        else
            vma->vm_pgoff = (mmapData->mmap_offset+mmapData->mmap_interval) >> PAGE_SHIFT;
    #elif defined(CONFIG_ARM) || defined(CONFIG_ARM64)
        if(mmapData->mmap_miusel == 0)
            vma->vm_pgoff = (mmapData->mmap_offset+ARM_MIU0_BASE_ADDR) >> PAGE_SHIFT;
        else if(mmapData->mmap_miusel == 1)
            vma->vm_pgoff = (mmapData->mmap_offset+ARM_MIU1_BASE_ADDR) >> PAGE_SHIFT;
        else if(mmapData->mmap_miusel == 2)
            vma->vm_pgoff = (mmapData->mmap_offset+ARM_MIU2_BASE_ADDR) >> PAGE_SHIFT;
        else if(mmapData->mmap_miusel == 3)
            vma->vm_pgoff = (mmapData->mmap_offset+ARM_MIU3_BASE_ADDR) >> PAGE_SHIFT;
        else
            panic("miu%d not support\n",mmapData->mmap_miusel);
    #endif
    }

    /* set page to no cache */
    if((mmapData->u8MapCached == KER_CACHEMODE_CACHE)
#ifdef MP_MMAP_MMAP_BOUNDARY_PROTECT
        || (mmapData->u8MapCached == KER_CACHEMODE_CACHE_PROTECT)
#endif
        )
    {
        #if defined(CONFIG_MIPS)
        pgprot_val(vma->vm_page_prot) &= ~_CACHE_MASK;
        pgprot_val(vma->vm_page_prot) |= _page_cachable_default;
        #elif defined(CONFIG_ARM)
        //vma->vm_page_prot=__pgprot_modify(vma->vm_page_prot, L_PTE_MT_MASK,L_PTE_MT_WRITEBACK);
        vma->vm_page_prot=__pgprot_modify(vma->vm_page_prot,L_PTE_MT_MASK,L_PTE_MT_DEV_CACHED);
		#elif defined(CONFIG_ARM64)
        pgprot_val(vma->vm_page_prot) = pgprot_cached(vma->vm_page_prot);
        #endif
    }
#ifdef MP_MMAP_MMAP_BOUNDARY_PROTECT
    else if (mmapData->u8MapCached == KER_CACHEMODE_TRAP)
    {
        #if defined(CONFIG_MIPS)
        pgprot_val(vma->vm_page_prot) = pgprot_val(PAGE_NONE);
        #elif defined(CONFIG_ARM) || defined(CONFIG_ARM64)
        pgprot_val(vma->vm_page_prot) = __PAGE_NONE;
        #endif
		printk("\033[35mUsing TRAP, vma->vm_flags is %lu, vma->vm_page_prot is %lu\033[m\n", vma->vm_flags, pgprot_val(vma->vm_page_prot));
    }
#endif
    else
    {
        #if defined(CONFIG_MIPS)
        pgprot_val(vma->vm_page_prot) &= ~_CACHE_MASK;
        pgprot_val(vma->vm_page_prot) |= _CACHE_UNCACHED;
        #elif defined(CONFIG_ARM) || defined(CONFIG_ARM64)
        if((mmapData->u8MapCached == KER_CACHEMODE_UNCACHE_BUFFERABLE)
#ifdef MP_MMAP_MMAP_BOUNDARY_PROTECT
            || (mmapData->u8MapCached == KER_CACHEMODE_UNCACHE_BUFFERABLE_PROTECT)
#endif
            )
        {
		#if defined(CONFIG_ARM) 
           // pgprot_val(vma->vm_page_prot) = pgprot_dmacoherent(vma->vm_page_prot);  //This solution is only to modify mpool mmaping rule,
//The Correct solution is to enable config_ARM_DMA_MEM_BUFFERABLE, but if enable config_ARM_DMA_MEM_BUFFERABLE , system will trigger the app to crash. 
	      pgprot_val(vma->vm_page_prot) = __pgprot_modify(vma->vm_page_prot, L_PTE_MT_MASK, L_PTE_MT_BUFFERABLE | L_PTE_XN);
		#else
		  pgprot_val(vma->vm_page_prot) = pgprot_dmacoherent(vma->vm_page_prot); 
        #endif		  
        }
        else
        {
		#if defined(CONFIG_ARM)
           // pgprot_val(vma->vm_page_prot) = pgprot_dmacoherent(vma->vm_page_prot); //This solution is only to modify mpool mmaping rule,
//The Correct solution is to enable config_ARM_DMA_MEM_BUFFERABLE, but if enable config_ARM_DMA_MEM_BUFFERABLE , system will trigger the app to crash. 
	      pgprot_val(vma->vm_page_prot)= __pgprot_modify(vma->vm_page_prot, L_PTE_MT_MASK, L_PTE_MT_BUFFERABLE | L_PTE_XN);
        #else
		  pgprot_val(vma->vm_page_prot) = pgprot_dmacoherent(vma->vm_page_prot);
        #endif		
		}
        #endif
    }

    if(mmapData->setflag)
    {
		if(mmapData->mmap_miusel == 0)
		{
			#if defined(CONFIG_MIPS)
			BUS_BASE = MIPS_MIU0_BUS_BASE;
			#if defined(CONFIG_MSTAR_KENYA) || defined(CONFIG_MSTAR_KERES)
			if(mmapData->mmap_offset >= 0x10000000)
				BUS_BASE += 0x40000000;
			#endif
			#elif defined(CONFIG_ARM) || defined(CONFIG_ARM64)
			BUS_BASE = ARM_MIU0_BUS_BASE;
			#endif
		}
		else if(mmapData->mmap_miusel == 1)
		{
			#if defined(CONFIG_MIPS)
			BUS_BASE = MIPS_MIU1_BUS_BASE;
			#elif defined(CONFIG_ARM) || defined(CONFIG_ARM64)
			BUS_BASE = ARM_MIU1_BUS_BASE;
			#endif
		}
		else if(mmapData->mmap_miusel == 2)
		{
			#if defined(CONFIG_ARM) || defined(CONFIG_ARM64)
			BUS_BASE = ARM_MIU2_BUS_BASE;
			#endif
		}
		else if(mmapData->mmap_miusel == 3)
		{
			#if defined(CONFIG_ARM) || defined(CONFIG_ARM64)
			BUS_BASE = ARM_MIU3_BUS_BASE;
			#endif
		}
        else
        {
            panic("miu%d not support\n",mmapData->mmap_miusel);
        }

#ifdef CONFIG_MP_MMAP_MMAP_BOUNDARY_PROTECT
		if ((mmapData->u8MapCached == KER_CACHEMODE_UNCACHE_NONBUFFERABLE_PROTECT) ||
			(mmapData->u8MapCached == KER_CACHEMODE_CACHE_PROTECT) ||
			(mmapData->u8MapCached == KER_CACHEMODE_UNCACHE_BUFFERABLE_PROTECT)) {
			pgprot_t temp_prot;

            #if defined(CONFIG_MIPS)
			printk("\033[35m[MMAP]Boundary Protect for MIPS\033[m\n");

			// we  divide into 3 sectios to do mpool_io_remap_range
            mmapData->mmap_size -= 0x2000;
            pgprot_val(temp_prot) = pgprot_val(vma->vm_page_prot);

            // 1. Upper Boundary
            pgprot_val(vma->vm_page_prot) = pgprot_val(PAGE_NONE);
			if(io_remap_pfn_range(vma, vma->vm_start,
                    (BUS_BASE+mmapData->mmap_offset) >> PAGE_SHIFT, 0x1000,
                            vma->vm_page_prot))
            {
                mutex_unlock(&mpool_iomap_mutex);
                return -EAGAIN;
            }

            // 2. Lower Boundary
            pgprot_val(vma->vm_page_prot) = pgprot_val(PAGE_NONE);
			if(io_remap_pfn_range(vma, vma->vm_start+mmapData->mmap_size+0x1000,
                    (BUS_BASE+mmapData->mmap_offset+mmapData->mmap_size-0x1000) >> PAGE_SHIFT, 0x1000,
                            vma->vm_page_prot))
            {
                mutex_unlock(&mpool_iomap_mutex);
                return -EAGAIN;
            }

            // 3. Main Area
            pgprot_val(vma->vm_page_prot) = pgprot_val(temp_prot);
			if(io_remap_pfn_range(vma, vma->vm_start+0x1000,
                    (BUS_BASE+mmapData->mmap_offset) >> PAGE_SHIFT, mmapData->mmap_size,
                            vma->vm_page_prot))
            {
                mutex_unlock(&mpool_iomap_mutex);
                return -EAGAIN;
            }
            #elif defined(CONFIG_ARM) || defined(CONFIG_ARM64)
			printk("\033[35m[MMAP]Boundary Protect for ARM\033[m\n");

            // we  divide into 3 sectios to do mpool_io_remap_range
            mmapData->mmap_size -= 0x2000;
            temp_prot = vma->vm_page_prot;

            // 1. Upper Boundary
            pgprot_val(vma->vm_page_prot) = __PAGE_NONE;
            if(mpool_io_remap_range(vma, vma->vm_start,
                            (BUS_BASE+mmapData->mmap_offset) >> PAGE_SHIFT, 0x1000,
                                    vma->vm_page_prot))
            {
            	mutex_unlock(&mpool_iomap_mutex);
                return -EAGAIN;
            }

            // 2. Lower Boundary
            pgprot_val(vma->vm_page_prot) = __PAGE_NONE;
            if(mpool_io_remap_range(vma, vma->vm_start+mmapData->mmap_size+0x1000,
            				(BUS_BASE+mmapData->mmap_offset+mmapData->mmap_size-0x1000) >> PAGE_SHIFT, 0x1000,
                            		vma->vm_page_prot))
            {
                mutex_unlock(&mpool_iomap_mutex);
                return -EAGAIN;
            }

            // 3. Main Area
            pgprot_val(vma->vm_page_prot) = temp_prot;
            if(mpool_io_remap_range(vma, vma->vm_start+0x1000,
                            (BUS_BASE+mmapData->mmap_offset) >> PAGE_SHIFT, mmapData->mmap_size,
            						vma->vm_page_prot))
            {
                mutex_unlock(&mpool_iomap_mutex);
                return -EAGAIN;
            }
            #endif
		}
        else
#endif
		{
            #if defined(CONFIG_MIPS)
            if(io_remap_pfn_range(vma, vma->vm_start,
                    (BUS_BASE+mmapData->mmap_offset) >> PAGE_SHIFT, mmapData->mmap_size,
                            vma->vm_page_prot))
            #elif defined(CONFIG_ARM) || defined(CONFIG_ARM64)
            if(mpool_io_remap_range(vma, vma->vm_start,
                    (BUS_BASE+mmapData->mmap_offset) >> PAGE_SHIFT, mmapData->mmap_size,
                            vma->vm_page_prot))
            #endif
            {
                mutex_unlock(&mpool_iomap_mutex);
                return -EAGAIN;
            }
		}
	}
	else
	{
#if defined(CONFIG_MSTAR_TITANIA3) || \
	defined(CONFIG_MSTAR_TITANIA10) || \
    defined(CONFIG_MSTAR_TITANIA8) || \
    defined(CONFIG_MSTAR_TITANIA12) || \
    defined(CONFIG_MSTAR_TITANIA9) || \
    defined(CONFIG_MSTAR_TITANIA4) || \
    defined(CONFIG_MSTAR_TITANIA7) || \
    defined(CONFIG_MSTAR_URANUS4) || \
    defined(CONFIG_MSTAR_JANUS2) || \
    defined(CONFIG_MSTAR_TITANIA11) || \
    defined(CONFIG_MSTAR_TITANIA13) || \
    defined(CONFIG_MSTAR_AMBER1) || \
	defined(CONFIG_MSTAR_AMBER2) || \
	defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_KRONUS) || \
    defined(CONFIG_MSTAR_KELTIC) || \
    defined(CONFIG_MSTAR_AMBER6) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST) || \
    defined(CONFIG_MSTAR_KAISERIN) || \
    defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EMERALD) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EIFFEL) || \
    defined(CONFIG_MSTAR_NIKE) || \
    defined(CONFIG_MSTAR_MADISON) || \
	defined(CONFIG_MSTAR_CLIPPERS) || \
    defined(CONFIG_MSTAR_MIAMI) || \
    defined(CONFIG_MSTAR_MONACO) || \
    defined(CONFIG_MSTAR_MALDIVES) || \
    defined(CONFIG_MSTAR_MACAN) || \
    defined(CONFIG_MSTAR_MUSTANG) || \
    defined(CONFIG_MSTAR_MESSI) || \
    defined(CONFIG_MSTAR_K6Lite) || \
    defined(CONFIG_MSTAR_KAISER) || \
    defined(CONFIG_MSTAR_NUGGET) || \
	defined(CONFIG_MSTAR_NIKON) || \
    defined(CONFIG_MSTAR_KAISERS) || \
    defined(CONFIG_MSTAR_KENYA)   || \
    defined(CONFIG_MSTAR_KERES) || \
    defined(CONFIG_MSTAR_KIWI) || \
    defined(CONFIG_MSTAR_MUJI) || \
    defined(CONFIG_MSTAR_MONET) || \
    defined(CONFIG_MSTAR_KANO) || \
    defined(CONFIG_MSTAR_K6) || \
	defined(CONFIG_MSTAR_MANHATTAN) || \
    defined(CONFIG_MSTAR_CURRY) || \
    defined(CONFIG_MSTAR_C2P) || \
	defined(CONFIG_MSTAR_MASERATI) || \
	defined(CONFIG_MSTAR_MOONEY)

#if (defined(CONFIG_MSTAR_TITANIA3) || defined(CONFIG_MSTAR_TITANIA10))
		//check MIU0 size from MAU
		u16 miu0_size_reg_val=0;
#endif

		unsigned long u32MIU0_MapStart = 0;
		unsigned long u32MIU0_MapSize = 0;
		phys_addr_t u32MIU1_MapStart = 0;
		unsigned long u32MIU1_MapSize = 0;

#if (defined(CONFIG_MSTAR_TITANIA3) || defined(CONFIG_MSTAR_TITANIA10))
		miu0_size_reg_val=REG_ADDR(REG_MAU0_MIU0_SIZE);
		if(miu0_size_reg_val&0x100)
		{
			miu0_len=0x10000000;
		}
#endif

        //calculate map size & start
        if(mmapData->mpool_base<miu0_len)
        {
            u32MIU0_MapStart=mmapData->mpool_base;

            if((mmapData->mpool_base+mmapData->mpool_size)>miu0_len)
            {
                u32MIU0_MapSize=(miu0_len-mmapData->mpool_base);
                u32MIU1_MapSize=mmapData->mpool_size-u32MIU0_MapSize;
                #if defined(CONFIG_MIPS)
                u32MIU1_MapStart=MIPS_MIU1_BUS_BASE;
                #elif defined(CONFIG_ARM) || defined(CONFIG_ARM64)
                u32MIU1_MapStart=ARM_MIU1_BUS_BASE;
                #endif
            }
	    	else
            {
                u32MIU0_MapSize=mmapData->mpool_size;
                u32MIU1_MapSize=0;
                #if defined(CONFIG_MIPS)
                u32MIU1_MapStart=MIPS_MIU1_BUS_BASE;
                #elif defined(CONFIG_ARM) || defined(CONFIG_ARM64)
                u32MIU1_MapStart=ARM_MIU1_BUS_BASE;
                #endif
            }
        }
        else
        {
            u32MIU0_MapStart=0;
            u32MIU0_MapSize=0;
            #if defined(CONFIG_MIPS)
            u32MIU1_MapStart=(mmapData->mpool_base-miu0_len)+MIPS_MIU1_BUS_BASE;
            #elif defined(CONFIG_ARM) || defined(CONFIG_ARM64)
            u32MIU1_MapStart=ARM_MIU1_BUS_BASE;
            #endif
            u32MIU1_MapSize=mmapData->mpool_size;
        }
        //printk("MPOOL MAP INFORMATION:\n");
        //printk("    MIU0 Length=0x%08X\n",miu0_len);
        //printk("    MIU0 MAP:0x%08lX,0x%08lX\n",u32MIU0_MapStart,u32MIU0_MapSize);
        //printk("    MIU1 MAP:0x%08lX,0x%08lX\n",u32MIU1_MapStart,u32MIU1_MapSize);

        if(u32MIU0_MapSize)
        {
            if (mpool_io_remap_range(vma, vma->vm_start+ mmapData->mpool_base,
                                        u32MIU0_MapStart >> PAGE_SHIFT, u32MIU0_MapSize,
                                        vma->vm_page_prot))
            {
              mutex_unlock(&mpool_iomap_mutex);
              return -EAGAIN;
            }
        }

        if(u32MIU1_MapSize)
        {
			#if defined(CONFIG_MIPS)
           	if(io_remap_pfn_range(vma, vma->vm_start+u32MIU0_MapSize,
                                       MIPS_MIU1_BUS_BASE >> PAGE_SHIFT, u32MIU1_MapSize,
                                       vma->vm_page_prot))
			#elif defined(CONFIG_ARM) || defined(CONFIG_ARM64)
			if(mpool_io_remap_range(vma, vma->vm_start+u32MIU0_MapSize,
                                       ARM_MIU1_BUS_BASE >> PAGE_SHIFT, u32MIU1_MapSize,
                                       vma->vm_page_prot))
            #endif
			{
				mutex_unlock(&mpool_iomap_mutex);
				return -EAGAIN;
			}
		}
#else
    	if(mpool_io_remap_range(vma, vma->vm_start+mmapData->mpool_base,
                           mmapData->mpool_base >> PAGE_SHIFT, mmapData->mpool_size,
                           vma->vm_page_prot))
		{
			mutex_unlock(&mpool_iomap_mutex);
			return -EAGAIN;
		}
#endif
	}

	mutex_unlock(&mpool_iomap_mutex);
    return 0;
}

#if defined(CONFIG_COMPAT)
static long Compat_MDrv_MPOOL_Ioctl(struct file *filp, unsigned int cmd, unsigned long arg){
	int	err= 0;
	long ret = 0;

	switch(cmd)
	{
		case MPOOL_IOC_INFO:
		case MPOOL_IOC_FLUSHDCACHE:
		case MPOOL_IOC_SET:
		case MPOOL_IOC_KERNEL_DETECT:
		case MPOOL_IOC_VERSION:
		case MPOOL_IOC_FLUSHDCACHE_ALL:
		case MPOOL_IOC_GET_BLOCK_OFFSET:
		case MPOOL_IOC_PA2BA:
		case MPOOL_IOC_BA2PA:
		case MPOOL_IOC_SET_MAP_CACHE:
#ifdef CONFIG_MSTAR_DLMALLOC
		case MPOOL_IOC_DLMALLOC_DELETE_POOL:
#endif
		{
			return filp->f_op->unlocked_ioctl(filp, cmd,(unsigned long)compat_ptr(arg));
		}
#ifdef CONFIG_MSTAR_DLMALLOC
		case COMPAT_MPOOL_IOC_DLMALLOC_CREATE_POOL:
		{
			compat_u64 bus_addr;
			compat_uptr_t ptr;
			compat_size_t u;
			compat_int_t i;

			DrvMPool_Dlmalloc_Info_t32 __user *data32;
			DrvMPool_Dlmalloc_Info_t __user *data;

			data32 = compat_ptr(arg);
			data = compat_alloc_user_space(sizeof(*data));
			if (data == NULL)
				return -EFAULT;

			err = get_user(bus_addr, &data32->bus_addr);
			err |= put_user(bus_addr, &data->bus_addr);

			err |= get_user(ptr, &data32->user_va);
			err |= put_user(ptr, &data->user_va);

			err |= get_user(u, &data32->capacity);
			err |= put_user(u, &data->capacity);

			err |= get_user(i, &data32->locked);
			err |= put_user(i, &data->locked);
			if(err)
			{
				printk("\033[35mFunction = %s, Line = %d, get data from user failed\033[m\n", __PRETTY_FUNCTION__, __LINE__);
				return err;
			}

			ret = filp->f_op->unlocked_ioctl(filp, MPOOL_IOC_DLMALLOC_CREATE_POOL, (unsigned long)data);

			err = get_user(ptr, &data->pool_bus_addr);
			err |= put_user(ptr, &data32->pool_bus_addr);
			if(err)
			{
				printk("\033[35mFunction = %s, Line = %d, copy data to user failed\033[m\n", __PRETTY_FUNCTION__, __LINE__);
				return err;
			}

			return ret;
		}
		case COMPAT_MPOOL_IOC_DLMALLOC_ALLOC_POOL_MEMORY:
		{
			compat_u64 pool_bus_addr;
			compat_u64 alloc_addr;
			compat_size_t alloc_size;

			DrvMPool_Dlmalloc_Alloc_Free_Info_t32 __user *data32;
			DrvMPool_Dlmalloc_Alloc_Free_Info_t __user *data;

			data32 = compat_ptr(arg);
			data = compat_alloc_user_space(sizeof(*data));
			if (data == NULL)
				return -EFAULT;

			err = get_user(pool_bus_addr, &data32->pool_bus_addr);
			err |= put_user(pool_bus_addr, &data->pool_bus_addr);

			err |= get_user(alloc_size, &data32->alloc_size);
			err |= put_user(alloc_size, &data->alloc_size);
			if(err)
			{
				printk("\033[35mFunction = %s, Line = %d, get data from user failed\033[m\n", __PRETTY_FUNCTION__, __LINE__);
				return err;
			}

			ret = filp->f_op->unlocked_ioctl(filp, MPOOL_IOC_DLMALLOC_ALLOC_POOL_MEMORY, (unsigned long)data);

			err = get_user(alloc_addr, &data->alloc_addr);
			err |= put_user(alloc_addr, &data32->alloc_addr);
			if(err)
			{
				printk("\033[35mFunction = %s, Line = %d, copy data to user failed\033[m\n", __PRETTY_FUNCTION__, __LINE__);
				return err;
			}

			return ret;
		}
		case COMPAT_MPOOL_IOC_DLMALLOC_FREE_POOL_MEMORY:
		{
			compat_u64 pool_bus_addr;
			compat_u64 alloc_addr;

			DrvMPool_Dlmalloc_Alloc_Free_Info_t32 __user *data32;
			DrvMPool_Dlmalloc_Alloc_Free_Info_t __user *data;

			data32 = compat_ptr(arg);
			data = compat_alloc_user_space(sizeof(*data));
			if (data == NULL)
				return -EFAULT;

			err = get_user(pool_bus_addr, &data32->pool_bus_addr);
			err |= put_user(pool_bus_addr, &data->pool_bus_addr);

			err |= get_user(alloc_addr, &data32->alloc_addr);
			err |= put_user(alloc_addr, &data->alloc_addr);
			if(err)
			{
				printk("\033[35mFunction = %s, Line = %d, get data from user failed\033[m\n", __PRETTY_FUNCTION__, __LINE__);
				return err;
			}

			ret = filp->f_op->unlocked_ioctl(filp, MPOOL_IOC_DLMALLOC_FREE_POOL_MEMORY, (unsigned long)data);

			return ret;
		}
		case COMPAT_MPOOL_IOC_DLMALLOC_REALLOC_POOL_MEMORY:
		{
			compat_u64 pool_bus_addr;
			compat_u64 ori_alloc_addr;
			compat_u64 re_alloc_addr;
			compat_size_t alloc_size;

			DrvMPool_Dlmalloc_Alloc_Free_Info_t32 __user *data32;
			DrvMPool_Dlmalloc_Alloc_Free_Info_t __user *data;

			data32 = compat_ptr(arg);
			data = compat_alloc_user_space(sizeof(*data));
			if (data == NULL)
				return -EFAULT;

			err = get_user(pool_bus_addr, &data32->pool_bus_addr);
			err |= put_user(pool_bus_addr, &data->pool_bus_addr);

			err |= get_user(ori_alloc_addr, &data32->alloc_addr);
			err |= put_user(ori_alloc_addr, &data->alloc_addr);

			err |= get_user(alloc_size, &data32->alloc_size);
			err |= put_user(alloc_size, &data->alloc_size);
			if(err)
			{
				printk("\033[35mFunction = %s, Line = %d, get data from user failed\033[m\n", __PRETTY_FUNCTION__, __LINE__);
				return err;
			}

			ret = filp->f_op->unlocked_ioctl(filp, MPOOL_IOC_DLMALLOC_REALLOC_POOL_MEMORY, (unsigned long)data);

			err = get_user(re_alloc_addr, &data->new_alloc_addr);
			err |= put_user(re_alloc_addr, &data32->new_alloc_addr);
			if(err)
			{
				printk("\033[35mFunction = %s, Line = %d, copy data to user failed\033[m\n", __PRETTY_FUNCTION__, __LINE__);
				return err;
			}

			return ret;
		}
#endif
		case COMPAT_MPOOL_IOC_FLUSHDCACHE_PAVA:
		{
			compat_u64 u64;
			compat_size_t u;

			DrvMPool_Flush_Info_t32 __user *data32;
			DrvMPool_Flush_Info_t __user *data;
			data = compat_alloc_user_space(sizeof(*data));
			if (data == NULL)
				return -EFAULT;

			data32 = compat_ptr(arg);
			err = get_user(u, &data32->u32AddrVirt);
			err |= put_user(u, &data->u32AddrVirt);
			err |= get_user(u64, &data32->u32AddrPhys);
			err |= put_user(u64, &data->u32AddrPhys);
			err |= get_user(u64, &data32->u32Size);
			err |= put_user(u64, &data->u32Size);
			if (err)
				return err;

			return filp->f_op->unlocked_ioctl(filp, MPOOL_IOC_FLUSHDCACHE_PAVA,(unsigned long)data);
		}
#ifdef CONFIG_HAVE_HW_BREAKPOINT
		case MPOOL_IOC_SETWATCHPT:
			break;
		case MPOOL_IOC_GETWATCHPT:
			break;
#endif
		default:
			printk("\033[35mFunction = %s, Line = %d, unknown command: %u\033[m\n", __PRETTY_FUNCTION__, __LINE__, cmd);
			return -ENOIOCTLCMD;
	}
	return -ENOIOCTLCMD;
}
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
static long _MDrv_MPOOL_Ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
#else
static int _MDrv_MPOOL_Ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
#endif
{
    int         err= 0;
    int         ret= 0;

    MMAP_FileData *mmapData = filp->private_data ;

    /*
     * extract the type and number bitfields, and don't decode
     * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
     */
    if (MPOOL_IOC_MAGIC!= _IOC_TYPE(cmd))
        return -ENOTTY;

    /*
     * the direction is a bitmask, and VERIFY_WRITE catches R/W
     * transfers. `Type' is user-oriented, while
     * access_ok is kernel-oriented, so the concept of "read" and
     * "write" is reversed
     */
    if (_IOC_DIR(cmd) & _IOC_READ)
    {
        err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
    }
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
    {
        err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
    }
    if (err)
        return -EFAULT;

    // @FIXME: Use a array of function pointer for program readable and code size later
    switch(cmd)
    {
    //------------------------------------------------------------------------------
    // Signal
    //------------------------------------------------------------------------------
    case MPOOL_IOC_INFO:
        {
            DrvMPool_Info_t i ;

            i.u32Addr = mmapData->mpool_base;
            i.u32Size = mmapData->mpool_size;
            MPOOL_DPRINTK("MPOOL_IOC_INFO i.u32Addr = %d\n", i.u32Addr);
            MPOOL_DPRINTK("MPOOL_IOC_INFO i.u32Size = %d\n", i.u32Size);

            ret= copy_to_user( (void *)arg, &i, sizeof(i) );
        }
        break;
    case MPOOL_IOC_FLUSHDCACHE:
        {
#if defined(CONFIG_MSTAR_TITANIA3) || \
	defined(CONFIG_MSTAR_TITANIA10) || \
    defined(CONFIG_MSTAR_TITANIA8) || \
    defined(CONFIG_MSTAR_TITANIA12) || \
    defined(CONFIG_MSTAR_TITANIA9) || \
    defined(CONFIG_MSTAR_TITANIA4) || \
    defined(CONFIG_MSTAR_TITANIA7) || \
    defined(CONFIG_MSTAR_URANUS4) || \
    defined(CONFIG_MSTAR_JANUS2) || \
    defined(CONFIG_MSTAR_TITANIA11) || \
    defined(CONFIG_MSTAR_TITANIA13) || \
    defined(CONFIG_MSTAR_AMBER1) || \
	defined(CONFIG_MSTAR_AMBER2)|| \
	defined(CONFIG_MSTAR_AMBER5)|| \
    defined(CONFIG_MSTAR_KRONUS) || \
    defined(CONFIG_MSTAR_KELTIC) || \
    defined(CONFIG_MSTAR_AMBER6) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST) || \
    defined(CONFIG_MSTAR_KAISERIN) || \
    defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_EMERALD) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EIFFEL) || \
    defined(CONFIG_MSTAR_NIKE) || \
    defined(CONFIG_MSTAR_MADISON) || \
	defined(CONFIG_MSTAR_CLIPPERS) || \
    defined(CONFIG_MSTAR_MIAMI) || \
    defined(CONFIG_MSTAR_MONACO) || \
    defined(CONFIG_MSTAR_MALDIVES) || \
    defined(CONFIG_MSTAR_MACAN) || \
    defined(CONFIG_MSTAR_MUSTANG) || \
    defined(CONFIG_MSTAR_MESSI) || \
    defined(CONFIG_MSTAR_K6Lite) || \
    defined(CONFIG_MSTAR_KAISER) || \
    defined(CONFIG_MSTAR_NUGGET) || \
	defined(CONFIG_MSTAR_NIKON) || \
    defined(CONFIG_MSTAR_KAISERS) || \
    defined(CONFIG_MSTAR_KENYA)   || \
    defined(CONFIG_MSTAR_KANO)   || \
    defined(CONFIG_MSTAR_K6) || \
    defined(CONFIG_MSTAR_KERES)  || \
    defined(CONFIG_MSTAR_KIWI)

            DrvMPool_Info_t i ;
            unsigned long miu0_len;
            ret= copy_from_user(&i, (void __user *)arg, sizeof(i));
            miu0_len=HAL_MIU0_LEN;

#if (defined(CONFIG_MSTAR_TITANIA3)||defined(CONFIG_MSTAR_TITANIA10))
            //check MIU0 size from MAU
            unsigned short miu0_size_reg_val;
            miu0_size_reg_val=REG_ADDR(REG_MAU0_MIU0_SIZE);
            if(miu0_size_reg_val&0x100)
            {
                miu0_len=0x10000000;
            }
            else
            {
                miu0_len=0x08000000;
            }
#endif//defined(CONFIG_MSTAR_TITANIA3)
            {
                #if defined(CONFIG_MIPS)
                _dma_cache_wback_inv((unsigned long)(i.u32Addr), i.u32Size);
                #elif defined(CONFIG_ARM) || defined(CONFIG_ARM64)
                //printk("i.u32Addr=0x%x,i.u32Size=0x%x,__va(i.u32Addr)=0x%x\n",i.u32Addr,i.u32Size,__va(i.u32Addr));
                Chip_Flush_Cache_Range((i.u32Addr), i.u32Size);
                #endif
            }
#else
            DrvMPool_Info_t i ;
            ret = copy_from_user(&i, (void __user *)arg, sizeof(i));
            #if defined(CONFIG_MIPS)
            _dma_cache_wback_inv((unsigned long)__va(i.u32Addr), i.u32Size);
            #elif defined(CONFIG_ARM) || defined(CONFIG_ARM64)
            //printk("i.u32Addr=0x%x,i.u32Size=0x%x,__va(i.u32Addr)=0x%x\n",i.u32Addr,i.u32Size,__va(i.u32Addr));
            Chip_Flush_Cache_Range((i.u32Addr), i.u32Size);
            #if 0
            __cpuc_flush_dcache_area((i.u32Addr),i.u32Size);
            outer_cache.flush_range(0x40400000,0xAA000000);
            outer_cache.inv_range(0x40400000,0xAA000000);
            dmac_map_area((i.u32Addr),i.u32Size,2);
            _chip_flush_miu_pipe();
            #endif

            #endif
#endif
        }
        break;
    case MPOOL_IOC_FLUSHDCACHE_PAVA:
        {
#if defined(CONFIG_MSTAR_AMBER3) || \
    defined(CONFIG_MSTAR_EAGLE) || \
    defined(CONFIG_MSTAR_AGATE) || \
    defined(CONFIG_MSTAR_EDISON) || \
    defined(CONFIG_MSTAR_EIFFEL) || \
    defined(CONFIG_MSTAR_EINSTEIN) || \
    defined(CONFIG_MSTAR_NAPOLI) || \
    defined(CONFIG_MSTAR_NIKE) || \
    defined(CONFIG_MSTAR_MADISON) || \
	defined(CONFIG_MSTAR_CLIPPERS) || \
    defined(CONFIG_MSTAR_MIAMI) || \
    defined(CONFIG_MSTAR_MONACO) || \
    defined(CONFIG_MSTAR_MALDIVES) || \
    defined(CONFIG_MSTAR_MACAN) || \
    defined(CONFIG_MSTAR_MUSTANG) || \
    defined(CONFIG_MSTAR_MESSI) || \
    defined(CONFIG_MSTAR_K6Lite) || \
    defined(CONFIG_MSTAR_KAISER) || \
    defined(CONFIG_MSTAR_KAISERS)|| \
	defined(CONFIG_MSTAR_MUJI) || \
    defined(CONFIG_MSTAR_MONET) || \
    defined(CONFIG_MSTAR_KANO) || \
    defined(CONFIG_MSTAR_K6) || \
	defined(CONFIG_MSTAR_MANHATTAN) || \
    defined(CONFIG_MSTAR_CURRY) || \
    defined(CONFIG_MSTAR_C2P) || \
	defined(CONFIG_MSTAR_MASERATI) || \
	defined(CONFIG_MSTAR_MOONEY)

            DrvMPool_Flush_Info_t i ;
            ret= copy_from_user(&i, (void __user *)arg, sizeof(i));

           /*Compare "u32AddrPhys" with "miu_base" to decide if which miu is located*/
           if(i.u32AddrPhys >= ARM_MIU3_BASE_ADDR)
               Chip_Flush_Cache_Range_VA_PA(i.u32AddrVirt, (i.u32AddrPhys - ARM_MIU3_BASE_ADDR) + ARM_MIU3_BUS_BASE , i.u32Size);
           if((i.u32AddrPhys >= ARM_MIU2_BASE_ADDR) && (i.u32AddrPhys < ARM_MIU3_BASE_ADDR))
               Chip_Flush_Cache_Range_VA_PA(i.u32AddrVirt, (i.u32AddrPhys - ARM_MIU2_BASE_ADDR) + ARM_MIU2_BUS_BASE , i.u32Size);
           if((i.u32AddrPhys >= ARM_MIU1_BASE_ADDR) && (i.u32AddrPhys < ARM_MIU2_BASE_ADDR))
               Chip_Flush_Cache_Range_VA_PA(i.u32AddrVirt, (i.u32AddrPhys - ARM_MIU1_BASE_ADDR) + ARM_MIU1_BUS_BASE , i.u32Size);
           else
               Chip_Flush_Cache_Range_VA_PA(i.u32AddrVirt, i.u32AddrPhys + ARM_MIU0_BUS_BASE , i.u32Size);
#endif
    	}
        break ;
    case MPOOL_IOC_GET_BLOCK_OFFSET:
        {
            DrvMPool_Info_t i ;
            ret= copy_from_user( &i, (void __user *)arg, sizeof(i) );
            MDrv_SYS_GetMMAP((int)i.u32Addr, &(i.u32Addr), &(i.u32Size)) ;
            ret= copy_to_user( (void __user *)arg, &i, sizeof(i) );
        }
        break ;
    case MPOOL_IOC_SET_MAP_CACHE:
        {
            ret= copy_from_user(&mmapData->u8MapCached, (void __user *)arg, sizeof(mmapData->u8MapCached));
        }
        break;
    case MPOOL_IOC_SET:
        {
           	DrvMPool_Info_t i = {0};
           	ret= copy_from_user(&i, (void __user *)arg, sizeof(i));
            mmapData->setflag = true;
            mmapData->mmap_offset = i.u32Addr;
            mmapData->mmap_size = i.u32Size;
            mmapData->mmap_interval = i.u32Interval;
            mmapData->mmap_miusel = i.u8MiuSel;
#if defined(CONFIG_MSTAR_KENYA) || defined(CONFIG_MSTAR_KERES)
			if(mmapData->mmap_offset < 0x10000000)
			{
				if(mmapData->mmap_offset + mmapData->mmap_size > 0x10000000)
				{
					printk("KENYA BA is Discontinuous, you can not map a PA across low_256MB and high_256MB, return -%d\n", EFAULT);
					return -EFAULT;
				}
			}
#endif
        }
        break;

	case MPOOL_IOC_KERNEL_DETECT:
			{
				DrvMPool_Kernel_Info_t i;
                		i.u32lxAddr = linux_base;
				i.u32lxSize = linux_size;
				i.u32lx2Addr = linux2_base;
				i.u32lx2Size = linux2_size;

                printk("lxaddr = %08llx, lxsize = %08llx\n", i.u32lxAddr, i.u32lxSize);
                printk("lx2addr = %08llx, lx2size = %08llx\n", i.u32lx2Addr, i.u32lx2Size);
				ret= copy_to_user( (void *)arg, &i, sizeof(i) );
			}
			break;
    case MPOOL_IOC_VERSION:
        {
            ret= copy_to_user( (void *)arg, &mpool_version, sizeof(mpool_version) );
        }
	    break;

    case MPOOL_IOC_FLUSHDCACHE_ALL:
    {
#if !(defined(CONFIG_MSTAR_TITANIA3) || defined(CONFIG_MSTAR_TITANIA10) )
         Chip_Flush_Cache_All();
#endif
    }
    break ;
#ifdef CONFIG_MSTAR_DLMALLOC
	case MPOOL_IOC_DLMALLOC_CREATE_POOL:
		{
			DrvMPool_Dlmalloc_Info_t dlmalloc_info;
			void __iomem *kernel_va;
			void *memorypool_va;
			int search_index = 0;
			int insert_index = 0;
			int get_index = 0;

			ret = copy_from_user(&dlmalloc_info, (void __user *)arg, sizeof(dlmalloc_info));
			mutex_lock(&dlmalloc_op_mutex);
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, get data from user: dlmalloc_info.bus_addr is 0x%llX\033[m\n", __PRETTY_FUNCTION__, __LINE__, dlmalloc_info.bus_addr);
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, get data from user: dlmalloc_info.capacity is 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, dlmalloc_info.capacity);
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, get data from user: dlmalloc_info.locked is %d\033[m\n", __PRETTY_FUNCTION__, __LINE__, dlmalloc_info.locked);

			/* search if pool is created, or we will need to find an empty element */
			for(search_index = 0; search_index < DLMALLOC_CNT; search_index++)
			{
				if(dlmalloc_init_check[search_index].initted_bus_addr == dlmalloc_info.bus_addr)
				{
					dlmalloc_init_check[search_index].ref_cnt++;
					printk(DLMALLOC_LOG "\033[31mFunction = %s, Line = %d, [index: %d, ref_cnt: %u] pool is already created!!\033[m\n", __PRETTY_FUNCTION__, __LINE__, search_index, dlmalloc_init_check[search_index].ref_cnt);

					printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, [index: %d] initted_bus_addr: 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, search_index, dlmalloc_init_check[search_index].initted_bus_addr);
					printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, [index: %d] kernel_va: 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, search_index, dlmalloc_init_check[search_index].initted_va);

					printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, [index: %d] initted_memorypool_bus: 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, search_index, dlmalloc_init_check[search_index].initted_memorypool_bus_addr);
					printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, [index: %d] initted_memorypool_va: 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, search_index, dlmalloc_init_check[search_index].initted_memorypool_va);

					printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, [index: %d] initted_size: 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, search_index, dlmalloc_init_check[search_index].initted_size);
					dlmalloc_info.pool_bus_addr = dlmalloc_init_check[search_index].initted_memorypool_bus_addr;
					ret |= copy_to_user((void __user *)arg, &dlmalloc_info, sizeof(dlmalloc_info));

					mutex_unlock(&dlmalloc_op_mutex);
					return 0;
				}

				if(!get_index)
				{
					if(dlmalloc_init_check[search_index].initted_bus_addr)	// not empty
						continue;
					else
					{
						get_index = 1;
						insert_index = search_index;
					}
				}
			}
			if(!get_index)
			{
				printk("\033[31mFunction = %s, Line = %d, search_index is %d, ERROR\033[m\n", __PRETTY_FUNCTION__, __LINE__, search_index);
				BUG_ON(1);
			}
			/* search if pool is created, or we will need to find an empty element */

			/* map bus_addr to kernel_va, then pass to mstar_create_mspace_with_base() */
			// change to cache memory mapping, due to this buffer will also be mapped to user_space_va, if it is mapped as cache, the user_space_va may flush old chunk_head data to kernel_space_va,
			// Thus, the chunk_head data will be wrong, and the dlmalloc ops will go to wrong flow
			// EX:
			//                (cached)                                   (non-cached)
			//         user_va chunk_head data                    kernel_va chunk_head data
			//                0x7F3E31                                     0x7F3E31
			//
			//     =============================== dlmalloc allocate 0x40 ===============================
			//                0x7F3E31                                       0x43
			//
			//     =============================== dlmalloc free     0x40 ===============================
			//                0x7F3E31                                       0x43    ==> OK case(user_space_va no flush), use   0x43   to do check "ok_address(fm, p) && ok_inuse(p)" ==> go dlmalloc free
			//                0x7F3E31                                     0x7F3E31  ==> NG case(user_space_va do flush), use 0x7F3E31 to do check "ok_address(fm, p) && ok_inuse(p)" ==> go page_fault
			//
			//kernel_va = ioremap(dlmalloc_info.bus_addr, dlmalloc_info.capacity);
			kernel_va = ioremap_cached(dlmalloc_info.bus_addr, dlmalloc_info.capacity);
			if(!kernel_va)
			{
				printk("\033[31m[Error] Can not map the dlmalloc region, from ba: 0x%lX, size: 0x%lX\033[m\n", dlmalloc_info.bus_addr, dlmalloc_info.capacity);
				BUG_ON(1);
			}
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, remap kernel_va: 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, kernel_va);
			/* map bus_addr to kernel_va, then pass to mstar_create_mspace_with_base() */

			/* go creating memory pool */
			memorypool_va = mstar_create_mspace_with_base(kernel_va, dlmalloc_info.capacity, dlmalloc_info.locked);
			dlmalloc_info.pool_bus_addr = memorypool_va - kernel_va + dlmalloc_info.bus_addr;
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, get a dlmalloc_info.MemoryPool @ va: 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, memorypool_va);
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, get a dlmalloc_info.MemoryPool @ ba: 0x%llX\033[m\n", __PRETTY_FUNCTION__, __LINE__, dlmalloc_info.pool_bus_addr);
			//iounmap(kernel_va); /* since pool management is using va_adrr, so we can not unmap it. If we unmap it, the next time we access the va_addr will cause page_fault */
			/* go creating memory pool */

			/* record dlmalloc_init_check[] */
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, [insert_index: %d] ref_cnt is %u\033[m\n", __PRETTY_FUNCTION__, __LINE__, insert_index, dlmalloc_init_check[insert_index].ref_cnt);
			dlmalloc_init_check[insert_index].initted_bus_addr = dlmalloc_info.bus_addr;
			dlmalloc_init_check[insert_index].initted_memorypool_bus_addr = dlmalloc_info.pool_bus_addr;
			dlmalloc_init_check[insert_index].initted_size = dlmalloc_info.capacity;
			dlmalloc_init_check[insert_index].initted_va = kernel_va;
			dlmalloc_init_check[insert_index].initted_memorypool_va = memorypool_va;
			dlmalloc_init_check[insert_index].ref_cnt++;
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, [insert_index: %d] ref_cnt is %u\033[m\n", __PRETTY_FUNCTION__, __LINE__, insert_index, dlmalloc_init_check[insert_index].ref_cnt);
			/* record dlmalloc_init_check[] */

			mutex_unlock(&dlmalloc_op_mutex);
			ret |= copy_to_user((void __user *)arg, &dlmalloc_info, sizeof(dlmalloc_info));
		}
		break;
	case MPOOL_IOC_DLMALLOC_DELETE_POOL:
		{
			DrvMPool_Dlmalloc_Delete_Info_t dlmalloc_delete_info;
			void __iomem *kernel_va;
			void *memorypool_va;
			int search_index = 0;
			size_t freed_size = 0;

			ret = copy_from_user(&dlmalloc_delete_info, (void __user *)arg, sizeof(dlmalloc_delete_info));
			mutex_lock(&dlmalloc_op_mutex);
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, get data from user: dlmalloc_delete_info.pool_bus_addr is 0x%llX\033[m\n", __PRETTY_FUNCTION__, __LINE__, dlmalloc_delete_info.pool_bus_addr);

			/* search if pool is created */
			for(search_index = 0; search_index < DLMALLOC_CNT; search_index++)
			{
				if(dlmalloc_init_check[search_index].initted_memorypool_bus_addr == dlmalloc_delete_info.pool_bus_addr)
					break;
			}
			if(search_index == DLMALLOC_CNT)
			{
				printk("\033[31mFunction = %s, Line = %d, search_index is %d, ERROR\033[m\n", __PRETTY_FUNCTION__, __LINE__, search_index);
				BUG_ON(1);
			}
			/* search if pool is created */

			/* check ref_cnt */
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, [search_index: %d] ref_cnt is %u\033[m\n", __PRETTY_FUNCTION__, __LINE__, search_index, dlmalloc_init_check[search_index].ref_cnt);
			dlmalloc_init_check[search_index].ref_cnt--;
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, [search_index: %d] ref_cnt is %u\033[m\n", __PRETTY_FUNCTION__, __LINE__, search_index, dlmalloc_init_check[search_index].ref_cnt);
			if(dlmalloc_init_check[search_index].ref_cnt != 0)
			{
				printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, search_index is %d, ref_cnt is %u\033[m\n", __PRETTY_FUNCTION__, __LINE__, search_index, dlmalloc_init_check[search_index].ref_cnt);
				mutex_unlock(&dlmalloc_op_mutex);
				return 0;
			}

			/* map bus_addr to kernel_va, then pass to mstar_destroy_mspace() */
			//kernel_va = ioremap(dlmalloc_init_check[search_index].initted_bus_addr, dlmalloc_init_check[search_index].initted_size);
			kernel_va = dlmalloc_init_check[search_index].initted_va;	// we did not unmap kernel_va while pool_create
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, [index: %d] remap kernel_va: 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, search_index, kernel_va);
			/* map bus_addr to kernel_va, then pass to mstar_destroy_mspace() */

			/* go deleting memory pool */
			//memorypool_va = dlmalloc_init_check[search_index].initted_memorypool_bus_addr - dlmalloc_init_check[search_index].initted_bus_addr + kernel_va;
			memorypool_va = dlmalloc_init_check[search_index].initted_memorypool_va;
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, get a dlmalloc_delete_info.MemoryPool @ va: 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, memorypool_va);
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, get a dlmalloc_delete_info.MemoryPool @ ba: 0x%llX\033[m\n", __PRETTY_FUNCTION__, __LINE__, dlmalloc_delete_info.pool_bus_addr);
			freed_size = mstar_destroy_mspace(memorypool_va);
			iounmap(kernel_va);	// since the pool is deleted, we unmap the kernel_va
			/* go deleting memory pool */

			/* clear dlmalloc_init_check[] */
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, initted_bus_addr = 0x%llX\033[m\n", __PRETTY_FUNCTION__, __LINE__, dlmalloc_init_check[search_index].initted_bus_addr);
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, initted_memorypool_bus_addr = 0x%llX\033[m\n", __PRETTY_FUNCTION__, __LINE__, dlmalloc_init_check[search_index].initted_memorypool_bus_addr);
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, initted_size = 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, dlmalloc_init_check[search_index].initted_size);
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, initted_va = 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, dlmalloc_init_check[search_index].initted_va);
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, initted_memorypool_va = 0x%lX\033[m\n\n\n", __PRETTY_FUNCTION__, __LINE__, dlmalloc_init_check[search_index].initted_memorypool_va);
			memset(&dlmalloc_init_check[search_index], 0, sizeof(DLMALLOC_Init_History));
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, initted_bus_addr = 0x%llX\033[m\n", __PRETTY_FUNCTION__, __LINE__, dlmalloc_init_check[search_index].initted_bus_addr);
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, initted_memorypool_bus_addr = 0x%llX\033[m\n", __PRETTY_FUNCTION__, __LINE__, dlmalloc_init_check[search_index].initted_memorypool_bus_addr);
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, initted_size = 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, dlmalloc_init_check[search_index].initted_size);
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, initted_va = 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, dlmalloc_init_check[search_index].initted_va);
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, initted_memorypool_va = 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, dlmalloc_init_check[search_index].initted_memorypool_va);
			/* clear dlmalloc_init_check[] */

			mutex_unlock(&dlmalloc_op_mutex);
		}
		break;
	case MPOOL_IOC_DLMALLOC_ALLOC_POOL_MEMORY:
		{
			DrvMPool_Dlmalloc_Alloc_Free_Info_t dlmalloc_alloc_info;
			void __iomem *kernel_va;
			void *memorypool_va;
			void *allocated_va;
			int search_index = 0;

			ret = copy_from_user(&dlmalloc_alloc_info, (void __user *)arg, sizeof(dlmalloc_alloc_info));
			mutex_lock(&dlmalloc_op_mutex);
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, get data from user: dlmalloc_alloc_info.pool_bus_addr is 0x%llX\033[m\n", __PRETTY_FUNCTION__, __LINE__, dlmalloc_alloc_info.pool_bus_addr);
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, get data from user: dlmalloc_alloc_info.alloc_size is 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, dlmalloc_alloc_info.alloc_size);

			/* search the memory_pool */
			for(search_index = 0; search_index < DLMALLOC_CNT; search_index++)
			{
				if(dlmalloc_init_check[search_index].initted_memorypool_bus_addr == dlmalloc_alloc_info.pool_bus_addr)
					break;
			}
			if(search_index == DLMALLOC_CNT)
			{
				printk("\033[31mFunction = %s, Line = %d, search_index is %d, ERROR\033[m\n", __PRETTY_FUNCTION__, __LINE__, search_index);
				BUG_ON(1);
			}
			/* search the memory_pool */

			/* map bus_addr to kernel_va, then pass to mstar_mspace_malloc() */
			//kernel_va = ioremap(dlmalloc_init_check[search_index].initted_bus_addr, dlmalloc_init_check[search_index].initted_size);
			kernel_va = dlmalloc_init_check[search_index].initted_va;
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, [index: %d] remap kernel_va: 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, search_index, kernel_va);
			/* map bus_addr to kernel_va, then pass to mstar_mspace_malloc() */

			/* go allocating memory pool */
			//memorypool_va = dlmalloc_init_check[search_index].initted_memorypool_bus_addr - dlmalloc_init_check[search_index].initted_bus_addr + kernel_va;
			memorypool_va = dlmalloc_init_check[search_index].initted_memorypool_va;
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, alloc from pool @ va: 0x%lX, size is 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, memorypool_va, dlmalloc_alloc_info.alloc_size);
			allocated_va = mstar_mspace_malloc(memorypool_va, dlmalloc_alloc_info.alloc_size);
			if(allocated_va)
			{
				dlmalloc_alloc_info.alloc_addr = allocated_va - kernel_va + dlmalloc_init_check[search_index].initted_bus_addr;
				printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, get an allocated start addr @ va: 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, allocated_va);
				//printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, kernel_va: 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, kernel_va);
				//printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, [%d] initted_bus_addr: 0x%llX\033[m\n", __PRETTY_FUNCTION__, __LINE__, search_index, dlmalloc_init_check[search_index].initted_bus_addr);
				//printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, get an allocated start addr @ ba: 0x%llX\033[m\n", __PRETTY_FUNCTION__, __LINE__, dlmalloc_alloc_info.alloc_addr);
			}
			else
			{
				printk("\033[31m[Error] Can not get memory from dlmalloc pool, size is 0x%lX\033[m\n", dlmalloc_alloc_info.alloc_size);
				dlmalloc_alloc_info.alloc_addr = 0;
			}
			//iounmap(kernel_va);
			/* go allocating memory pool */

			mutex_unlock(&dlmalloc_op_mutex);
			ret |= copy_to_user((void __user *)arg, &dlmalloc_alloc_info, sizeof(dlmalloc_alloc_info));
		}
		break;
	case MPOOL_IOC_DLMALLOC_FREE_POOL_MEMORY:
		{
			DrvMPool_Dlmalloc_Alloc_Free_Info_t dlmalloc_free_info;
			void __iomem *kernel_va;
			void *memorypool_va;
			void *free_va;
			int search_index = 0;

			ret = copy_from_user(&dlmalloc_free_info, (void __user *)arg, sizeof(dlmalloc_free_info));
			mutex_lock(&dlmalloc_op_mutex);
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, get data from user: dlmalloc_free_info.pool_bus_addr is 0x%llX\033[m\n", __PRETTY_FUNCTION__, __LINE__, dlmalloc_free_info.pool_bus_addr);
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, get data from user: dlmalloc_free_info.alloc_addr is 0x%llX\033[m\n", __PRETTY_FUNCTION__, __LINE__, dlmalloc_free_info.alloc_addr);

			/* search the memory_pool */
			for(search_index = 0; search_index < DLMALLOC_CNT; search_index++)
			{
				if(dlmalloc_init_check[search_index].initted_memorypool_bus_addr == dlmalloc_free_info.pool_bus_addr)
					break;
			}
			if(search_index == DLMALLOC_CNT)
			{
				printk("\033[31mFunction = %s, Line = %d, search_index is %d, ERROR\033[m\n", __PRETTY_FUNCTION__, __LINE__, search_index);
				BUG_ON(1);
			}
			/* search the memory_pool */

			/* map bus_addr to kernel_va, then pass to mstar_mspace_malloc() */
			//kernel_va = ioremap(dlmalloc_init_check[search_index].initted_bus_addr, dlmalloc_init_check[search_index].initted_size);
			kernel_va = dlmalloc_init_check[search_index].initted_va;
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, [index: %d] remap kernel_va: 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, search_index, kernel_va);
			/* map bus_addr to kernel_va, then pass to mstar_mspace_malloc() */

			/* go freeing memory pool */
			//memorypool_va = dlmalloc_init_check[search_index].initted_memorypool_bus_addr - dlmalloc_init_check[search_index].initted_bus_addr + kernel_va;
			memorypool_va = dlmalloc_init_check[search_index].initted_memorypool_va;
			free_va = dlmalloc_free_info.alloc_addr - (unsigned long)dlmalloc_init_check[search_index].initted_bus_addr + (unsigned long)dlmalloc_init_check[search_index].initted_va;
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, free from pool @ va: 0x%lX, free_start_addr is 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, memorypool_va, free_va);
			mstar_mspace_free(memorypool_va, free_va);
			/* go freeing memory pool */

			mutex_unlock(&dlmalloc_op_mutex);
			ret |= copy_to_user((void __user *)arg, &dlmalloc_free_info, sizeof(dlmalloc_free_info));
		}
		break;
	case MPOOL_IOC_DLMALLOC_REALLOC_POOL_MEMORY:
		{
			DrvMPool_Dlmalloc_Alloc_Free_Info_t dlmalloc_realloc_info;
			void __iomem *kernel_va;
			void *memorypool_va;
			void *new_allocated_va;
			void *ori_allocated_va;
			int search_index = 0;

			ret = copy_from_user(&dlmalloc_realloc_info, (void __user *)arg, sizeof(dlmalloc_realloc_info));
			mutex_lock(&dlmalloc_op_mutex);
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, get data from user: dlmalloc_realloc_info.pool_bus_addr is 0x%llX\033[m\n", __PRETTY_FUNCTION__, __LINE__, dlmalloc_realloc_info.pool_bus_addr);
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, get data from user: dlmalloc_realloc_info.ori_alloc_bus_addr is 0x%llX\033[m\n", __PRETTY_FUNCTION__, __LINE__, dlmalloc_realloc_info.alloc_addr);
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, get data from user: dlmalloc_realloc_info.alloc_size is 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, dlmalloc_realloc_info.alloc_size);

			/* search the memory_pool */
			for(search_index = 0; search_index < DLMALLOC_CNT; search_index++)
			{
				if(dlmalloc_init_check[search_index].initted_memorypool_bus_addr == dlmalloc_realloc_info.pool_bus_addr)
					break;
			}
			if(search_index == DLMALLOC_CNT)
			{
				printk("\033[31mFunction = %s, Line = %d, search_index is %d, ERROR\033[m\n", __PRETTY_FUNCTION__, __LINE__, search_index);
				BUG_ON(1);
			}
			/* search the memory_pool */

			/* map bus_addr to kernel_va, then pass to mstar_mspace_malloc() */
			//kernel_va = ioremap(dlmalloc_init_check[search_index].initted_bus_addr, dlmalloc_init_check[search_index].initted_size);
			kernel_va = dlmalloc_init_check[search_index].initted_va;
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, [index: %d] remap kernel_va: 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, search_index, kernel_va);
			/* map bus_addr to kernel_va, then pass to mstar_mspace_malloc() */

			/* go re-allocating memory pool */
			//memorypool_va = dlmalloc_init_check[search_index].initted_memorypool_bus_addr - dlmalloc_init_check[search_index].initted_bus_addr + kernel_va;
			memorypool_va = dlmalloc_init_check[search_index].initted_memorypool_va;
			ori_allocated_va = dlmalloc_realloc_info.alloc_addr - (unsigned long)dlmalloc_init_check[search_index].initted_bus_addr + (unsigned long)dlmalloc_init_check[search_index].initted_va;
			printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, re-alloc from pool @ va: 0x%lX, ori alloc addr @ va: 0x%lX, size is 0x%lX\033[m\n",
				__PRETTY_FUNCTION__, __LINE__, memorypool_va, ori_allocated_va, dlmalloc_realloc_info.alloc_size);
			new_allocated_va = mstar_mspace_realloc(memorypool_va, ori_allocated_va, dlmalloc_realloc_info.alloc_size);
			if(new_allocated_va)
			{
				dlmalloc_realloc_info.new_alloc_addr = new_allocated_va - kernel_va + dlmalloc_init_check[search_index].initted_bus_addr;
				printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, get an re-allocated start addr @ va: 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, new_allocated_va);
				printk(DLMALLOC_LOG "\033[35mFunction = %s, Line = %d, get an re-allocated start addr @ ba: 0x%llX\033[m\n", __PRETTY_FUNCTION__, __LINE__, dlmalloc_realloc_info.new_alloc_addr);
			}
			else
			{
				printk("\033[31m[Error] Can not get memory from dlmalloc pool, size is 0x%lX\033[m\n", dlmalloc_realloc_info.alloc_size);
				dlmalloc_realloc_info.new_alloc_addr = 0;
			}
			//iounmap(kernel_va);
			/* go re-allocating memory pool */

			mutex_unlock(&dlmalloc_op_mutex);
			ret |= copy_to_user((void __user *)arg, &dlmalloc_realloc_info, sizeof(dlmalloc_realloc_info));
		}
		break;
#endif
#ifdef CONFIG_HAVE_HW_BREAKPOINT
    //edit by york
    case MPOOL_IOC_SETWATCHPT:
    {
        DrvMPool_Watchpt_Info_t info;
        ret = copy_from_user(&info, (void __user *)arg, sizeof(info));
#ifdef CONFIG_ARM
{
	unsigned int tmp,WCR;

	if(info.rwx == 0)	/*read*/
		WCR = 0x1EF;
	else if(info.rwx == 1)	/*write*/
		WCR = 0x1F7;
        else			/*read,write*/
		WCR = 0x1FF;

	ARM_DBG_WRITE(c0, c0, 6, info.u32AddrVirt);
	tmp = (info.mask << 24)| WCR ;/*shift 24 is because the mask control bit is defined there*/
		 ARM_DBG_WRITE(c0, c0, 7, tmp);

	/*printk("The input 0 is:%#x and the mask bit is:%#x\n",tmp,info.mask);
	tmp = 0;
	tmp = info.u32AddrVirt | (1  << (info.mask * 4))
	asm volatile(
			input[0] = ;
        "mov	r1, %[i0]\n\t"									\
        "mov	%[o1], r1\n\t"									\
        : [o1] "=r"(out)									\
	: [i0] "g"(tmp)									\
        : "memory"                                                                              \
    	);
	printk("The input 0 is:%#x and  output[1] is :%#x, the size is:%#x\n",tmp,out,info.mask);*/
        printk("The register is written\n");
}
#elif defined(CONFIG_ARM64)
#else
        if(info.global == 1)
                 write_c0_watchhi0(0x40000000);
#endif

     }
     break ;
     case MPOOL_IOC_GETWATCHPT:
     {

	#ifdef CONFIG_ARM
	char str[200];
        DrvMPool_Wcvr_Info_t info;
	int m;
        ARM_DBG_READ(c0, c1, 6, info.wvr1);
	for(m = 0; m < 10000; m++);
        ARM_DBG_READ(c0, c0, 6, info.wvr0);
	for(m = 0; m < 10000; m++);
        ARM_DBG_READ(c0, c1, 7, info.wcr1);
	for(m = 0; m < 10000; m++);
        ARM_DBG_READ(c0, c0, 7, info.wcr0);
	for(m = 0; m < 10000; m++);
	sprintf(str,"ARM HW watchpoint register,the wvr0 is:%#x,wvr1 is:%#x,wcr0 is:%#x,wcr1 is:%#x",info.wvr0,info.wvr1,info.wcr0,info.wcr1);
	copy_to_user( (void *)arg, str, sizeof(str) );
	#endif
     }
     break;
#endif //CONFIG_HAVE_HW_BREAKPOINT

#if defined(CONFIG_ARM) || defined(CONFIG_ARM64)
	case MPOOL_IOC_PA2BA:
	{
		MS_PHY64 bus_address = 0;
		MS_PHY64 phy_address = 0;
		ret= copy_from_user(&phy_address, (void __user *)arg, sizeof(MS_PHY64));
		if( (phy_address >= ARM_MIU0_BASE_ADDR) && (phy_address < ARM_MIU1_BASE_ADDR) ) // MIU0
			bus_address = phy_address - ARM_MIU1_BASE_ADDR + ARM_MIU0_BUS_BASE;
		else if( (phy_address >= ARM_MIU1_BASE_ADDR) && (phy_address < ARM_MIU2_BASE_ADDR) )    // MIU1
			bus_address = phy_address - ARM_MIU1_BASE_ADDR + ARM_MIU1_BUS_BASE;
		else
			bus_address = phy_address - ARM_MIU2_BASE_ADDR + ARM_MIU2_BUS_BASE;    // MIU2
		ret |= copy_to_user( (void *)arg, bus_address, sizeof(MS_PHY64));
		break;
	}
	case MPOOL_IOC_BA2PA:
	{
		MS_PHY64 bus_address = 0;
		MS_PHY64 phy_address = 0;
		ret= copy_from_user(&bus_address, (void __user *)arg, sizeof(MS_PHY64));
		if( (bus_address >= ARM_MIU0_BUS_BASE) && (bus_address < ARM_MIU1_BUS_BASE) ) // MIU0
			phy_address = bus_address - ARM_MIU0_BUS_BASE + ARM_MIU0_BASE_ADDR;
		else if( (bus_address >= ARM_MIU1_BUS_BASE) && (bus_address < ARM_MIU2_BUS_BASE) ) // MIU1
			phy_address = bus_address - ARM_MIU1_BUS_BASE + ARM_MIU1_BASE_ADDR;
		else
			phy_address = bus_address - ARM_MIU2_BUS_BASE + ARM_MIU2_BASE_ADDR; // MIU2
		ret |= copy_to_user( (void *)arg, phy_address, sizeof(MS_PHY64) );
		break;
	}
#endif
    default:
		printk("\033[35mFunction = %s, Line = %d, unknown command: %u\033[m\n", __PRETTY_FUNCTION__, __LINE__, cmd);
        return -ENOTTY;
    }
    return 0;
}

/* chip with miu1 */
#if defined(CONFIG_MSTAR_TITANIA3) || \
	defined(CONFIG_MSTAR_TITANIA10) || \
    defined(CONFIG_MSTAR_TITANIA8) || \
    defined(CONFIG_MSTAR_TITANIA12) || \
    defined(CONFIG_MSTAR_TITANIA9) || \
    defined(CONFIG_MSTAR_TITANIA4) || \
    defined(CONFIG_MSTAR_TITANIA7) || \
    defined(CONFIG_MSTAR_URANUS4) || \
    defined(CONFIG_MSTAR_JANUS2) || \
    defined(CONFIG_MSTAR_TITANIA11) || \
    defined(CONFIG_MSTAR_TITANIA13) || \
    defined(CONFIG_MSTAR_AMBER1) || \
	defined(CONFIG_MSTAR_AMBER2) || \
	defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_AMBER6) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST) || \
    defined(CONFIG_MSTAR_EMERALD) || \
    defined(CONFIG_MSTAR_NUGGET) || \
    defined(CONFIG_MSTAR_NIKON) || \
    defined(CONFIG_MSTAR_KIWI)

static void _mod_mpool_map_miu1(void* p)
{
    unsigned long flags;
/* chip with miu1 at bus address 0x60000000 */
#if defined(CONFIG_MSTAR_TITANIA8) || \
    defined(CONFIG_MSTAR_TITANIA12) || \
    defined(CONFIG_MSTAR_JANUS2) || \
    defined(CONFIG_MSTAR_TITANIA9) || \
    defined(CONFIG_MSTAR_TITANIA13) || \
    defined(CONFIG_MSTAR_AMBER1) || \
	defined(CONFIG_MSTAR_AMBER2) || \
	defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_AMBER6) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST) || \
    defined(CONFIG_MSTAR_EMERALD) || \
    defined(CONFIG_MSTAR_NUGGET) || \
    defined(CONFIG_MSTAR_NIKON)
     unsigned long miu1_bus_offset = 0x60000000;

#elif  defined(CONFIG_MSTAR_KERES) || \
         defined(CONFIG_MSTAR_KIRIN) || \
	  defined(CONFIG_MSTAR_KRIS) || \
	  defined(CONFIG_MSTAR_KIWI)
    // no MIU1 for keres, here mapping second 256MB (256MB~512MB) in MIU0
   unsigned long miu1_bus_offset = 0x50000000;
#else
	unsigned long miu1_bus_offset = 0x40000000;
#endif
	/* cacheable */
	unsigned long entrylo0 = ((miu1_bus_offset >> 12) << 6) | \
	 						 ((_page_cachable_default >> _CACHE_SHIFT) << 3) | \
							 (1 << 2) | (1 << 1) | (1 << 0);
	/* un-cacheable or 0x7 */
	unsigned long entrylo1 = ((miu1_bus_offset >> 12) << 6) | \
							 (2 << 3) | (1 << 2) | (1 << 1) | (1 << 0);

	printk ("CPU%d: _mod_mpool_map_miu1: entrylo0 = %08lx entrylo1 = %08lx\n",
			smp_processor_id(), entrylo0, entrylo1);

    local_irq_save(flags);

    add_wired_entry(entrylo0, entrylo1, 0xC0000000, PM_256M);
    local_irq_restore(flags);
}
#endif // #if defined(CONFIG_MSTAR_XXXX)

//extern unsigned int MDrv_SYS_GetDRAMLength(void);

MSYSTEM_STATIC int __init mod_mpool_init(void)
{
    int s32Ret;
    dev_t dev = 0;

    //MDrv_SYS_GetMMAP(E_SYS_MMAP_LINUX_BASE, &mpool_size, &mpool_base);
    //mpool_size = MDrv_SYS_GetDRAMLength()-mpool_base ;

	printk(DLMALLOC_LOG "\033[31mFunction = %s, Line = %d, memset dlmalloc_init_check with size: %d\033[m\n", __PRETTY_FUNCTION__, __LINE__, sizeof(dlmalloc_init_check));
	memset(dlmalloc_init_check, 0, sizeof(dlmalloc_init_check));

    #if defined(CONFIG_MIPS)
    //get_boot_mem_info(LINUX_MEM, &linux_base, &linux_size);
    //get_boot_mem_info(LINUX_MEM2, &linux2_base, &linux2_size);
    //get_boot_mem_info(MPOOL_MEM, &mpool_base, &mpool_size);
    //get_boot_mem_info(EMAC_MEM, &emac_base, &emac_size);
    #elif defined(CONFIG_ARM) || defined(CONFIG_ARM64)
    //add here later

    #endif

    //printk( "\nMpool base=0x%08X\n", mpool_base );
    //printk( "\nMpool size=0x%08X\n", mpool_size );

    mpool_class = class_create(THIS_MODULE, "mpool");
    if (IS_ERR(mpool_class))
    {
        return PTR_ERR(mpool_class);
    }

    if (MPoolDev.s32MPoolMajor)
    {
        dev = MKDEV(MPoolDev.s32MPoolMajor, MPoolDev.s32MPoolMinor);
        s32Ret = register_chrdev_region(dev, MOD_MPOOL_DEVICE_COUNT, MOD_MPOOL_NAME);
    }
    else
    {
        s32Ret = alloc_chrdev_region(&dev, MPoolDev.s32MPoolMinor, MOD_MPOOL_DEVICE_COUNT, MOD_MPOOL_NAME);
        MPoolDev.s32MPoolMajor = MAJOR(dev);
    }

    if ( 0 > s32Ret)
    {
        MPOOL_DPRINTK("Unable to get major %d\n", MPoolDev.s32MPoolMajor);
        class_destroy(mpool_class);
        return s32Ret;
    }

    cdev_init(&MPoolDev.cDevice, &MPoolDev.MPoolFop);
    if (0!= (s32Ret= cdev_add(&MPoolDev.cDevice, dev, MOD_MPOOL_DEVICE_COUNT)))
    {
        MPOOL_DPRINTK("Unable add a character device\n");
        unregister_chrdev_region(dev, MOD_MPOOL_DEVICE_COUNT);
        class_destroy(mpool_class);
        return s32Ret;
    }

#if defined(CONFIG_MSTAR_TITANIA3) || \
	defined(CONFIG_MSTAR_TITANIA10) || \
    defined(CONFIG_MSTAR_TITANIA8) || \
    defined(CONFIG_MSTAR_TITANIA12) || \
    defined(CONFIG_MSTAR_TITANIA9) || \
    defined(CONFIG_MSTAR_TITANIA4) || \
    defined(CONFIG_MSTAR_TITANIA7) || \
    defined(CONFIG_MSTAR_URANUS4) || \
    defined(CONFIG_MSTAR_JANUS2) || \
    defined(CONFIG_MSTAR_TITANIA11) || \
    defined(CONFIG_MSTAR_TITANIA13) || \
    defined(CONFIG_MSTAR_AMBER1) || \
	defined(CONFIG_MSTAR_AMBER2) || \
	defined(CONFIG_MSTAR_AMBER5) || \
    defined(CONFIG_MSTAR_AMBER6) || \
    defined(CONFIG_MSTAR_AMBER7) || \
    defined(CONFIG_MSTAR_AMETHYST) || \
    defined(CONFIG_MSTAR_EMERALD) || \
    defined(CONFIG_MSTAR_NUGGET) || \
    defined(CONFIG_MSTAR_KANO) || \
    defined(CONFIG_MSTAR_K6) || \
    defined(CONFIG_MSTAR_NIKON) || \
    defined(CONFIG_MSTAR_KIWI)

# if defined(CONFIG_MIPS)
	on_each_cpu(_mod_mpool_map_miu1, NULL, 1);
# elif defined(CONFIG_ARM) || defined(CONFIG_ARM64)
	/* FIXME: add something here */
# endif

# if 0 /* only for testing */
	*((unsigned int *)0xC0000000) = 0x12345678;
	printk("[%s][%d] 0x%08x\n", __FILE__, __LINE__, *((unsigned int *)0xC0000000));
	printk("[%s][%d] 0x%08x\n", __FILE__, __LINE__, *((unsigned int *)0x80000000));
	printk("[%s][%d] 0x%08x\n", __FILE__, __LINE__, *((unsigned int *)0xA0000000));
# endif
#endif

    device_create(mpool_class, NULL, dev, NULL, MOD_MPOOL_NAME);
#ifdef CONFIG_MIPS
	bMPoolInit = 1;
#endif
    return 0;
}


MSYSTEM_STATIC void __exit mod_mpool_exit(void)
{
    cdev_del(&MPoolDev.cDevice);
    unregister_chrdev_region(MKDEV(MPoolDev.s32MPoolMajor, MPoolDev.s32MPoolMinor), MOD_MPOOL_DEVICE_COUNT);

    device_destroy(mpool_class, MKDEV(MPoolDev.s32MPoolMajor, MPoolDev.s32MPoolMinor));
    class_destroy(mpool_class);
}
#if defined(CONFIG_MSTAR_MPOOL) || defined(CONFIG_MSTAR_MPOOL_MODULE)

#if defined(CONFIG_MSTAR_KIWI)
  arch_initcall(mod_mpool_init);
#else
 module_init(mod_mpool_init);
#endif

module_exit(mod_mpool_exit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("MPOOL driver");
MODULE_LICENSE("GPL");
#endif//#if defined(CONFIG_MSTAR_MPOOL) || defined(CONFIG_MSTAR_MPOOL_MODULE)
