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
/// @file   ipa_bootargs.c
/// @brief  parse ipa pool boot arguments[
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <linux/types.h>
#include <asm/dma-contiguous.h>
#include <chip_setup.h>
#if defined(CONFIG_COMPAT)
#include <linux/compat.h>
#endif
#include <linux/memblock.h>
#include <mdrv_mstypes.h>
#include "mdrv_types.h"
#include <MsTypes.h>
#include <mdrv_system.h>

#include "ipa_pool.h"
#include "ipa_priv.h"
#include "ipa_bootargs.h"

/* ipa cma array */
struct IPA_CMA_BootArgs_Config ipa_cma_config[IPA_CMA_DEFAULT_NUM];
struct device ipa_cma_device[IPA_CMA_DEFAULT_NUM];
int mstar_driver_boot_ipa_cma_num = 0;

/*pa para array*/
struct PA_BootArgs_Config pa_config[MIU_NUM];
struct device pa_device[MIU_NUM];
int mstar_driver_boot_pa_num = 0;

/*ipa str pool para*/
/*this ipa_str_pool is just a special pool for str use,notice that the meaning of ipa_str_pool_config is 
not related to ipa_cma_config or pa_config.
*/
struct IPA_STR_POOL_BootArgs_Config ipa_str_pool_config;
int mstar_driver_boot_ipa_str_pool_num = 0;

static int get_ipa_id(enum IPASPCMAN_HEAP_TYPE type, int miu, int hid)
{
    int ipa_id = -1;
    
    switch (type) 
    {            
        case IPASPCMAN_HEAP_TYPE_CMA:
            {
                switch(miu)
                {
                    case 0:
                        ipa_id = (IPA_CMA_MIU0 << HEAP_TYPE_SHIFT) + hid;
                        break;
                    case 1:
                        ipa_id = (IPA_CMA_MIU1 << HEAP_TYPE_SHIFT) + hid;
                        break;                        
                    case 2:
                        ipa_id = (IPA_CMA_MIU2 << HEAP_TYPE_SHIFT) + hid;
                        break;
                    default:
                        ;
                }
            }
            break; 
            
        case IPASPCMAN_HEAP_TYPE_PA:
            {
                BUG_ON(hid != 0);
                switch(miu)
                {
                    case 0:
                        ipa_id = PA_SPACE_MIU0 << HEAP_TYPE_SHIFT;
                        break;
                    case 1:
                        ipa_id = PA_SPACE_MIU1 << HEAP_TYPE_SHIFT;
                        break;                        
                    case 2:
                        ipa_id = PA_SPACE_MIU2 << HEAP_TYPE_SHIFT;                      
                        break;
                    default:
                        ;
                }
            }
            break;
            
        default:
            ;
    }

    if(ipa_id == -1)
    {
        printk(KERN_ERR "%s input para error: type %d miu %d hid %d\n", __FUNCTION__, (int)type, miu, hid);
        BUG_ON(true);
    }
    
    return ipa_id;
}


/*
  * if  start address has been specified, convert it to cpu bus address (+ miu bus base address);
  * else, check it alignment 
  */
static bool GetReservedPhysicalAddr(unsigned char miu, unsigned long *start, 
            unsigned long *size, unsigned long alignfactor)

{
    unsigned long alignstart = 0;

    if(miu < 0 || miu > 3 || *size == 0)
        goto GetReservedPhysicalAddr_Fail;
    
    if(*start != CMA_HEAP_MIUOFFSET_NOCARE)
    {
        if(!IS_ALIGNED(*start, alignfactor))
        {
            printk(KERN_ERR "parameters start %lx not align to %lx\n", *start, alignfactor);
            goto GetReservedPhysicalAddr_Fail;
        }
    }

    if(!IS_ALIGNED(*size, alignfactor))
    {
        printk(KERN_ERR "parameters size %lx not align to %lx\n", *size, alignfactor);
        goto GetReservedPhysicalAddr_Fail;
    }
    
    if(*start == CMA_HEAP_MIUOFFSET_NOCARE)
        return true;
    
    switch (miu)
    {
        case 0:
        {        
            alignstart = *start + ARM_MIU0_BUS_BASE;          
            break;
        }
        case 1:
        {
            alignstart = *start + ARM_MIU1_BUS_BASE;           
            break;
        }
        case 2:
        {            
            alignstart = *start + ARM_MIU2_BUS_BASE;         
            break;
        }
        case 3:
        {            
            alignstart = *start + ARM_MIU3_BUS_BASE;           
            break;
        }
        default:
            goto GetReservedPhysicalAddr_Fail;
    }    

    *start = alignstart;
    return true;

GetReservedPhysicalAddr_Fail:
    printk(KERN_ERR "error: invalid parameters\n");
    *start = 0;
    *size = 0;
    return false;
}

/* ipa cma boot args parse
  *
  * IPA_CMA=vdec0 ...
  * IPA_CMA=vdec1 ...
  * IPA_CMA=xc ...
  * ...
  */
static bool ipa_cma_para_config(char *cmdline, struct IPA_CMA_BootArgs_Config * heapconfig) 
{
    char * option;
    int leng = 0;

    //printk(KERN_ERR "roger cmdline: %s, mstar_driver_boot_ipa_cma_num %d\n", cmdline, mstar_driver_boot_ipa_cma_num);

    option = strstr(cmdline, ",");
    leng = (int)(option - cmdline);
    if(leng > (IPA_NAME_LENG-1))
        leng = IPA_NAME_LENG - 1;

    strncpy(heapconfig->name, cmdline, leng);
    heapconfig->name[leng] = '\0';

    option = strstr(cmdline, "heap_id=");
    if(option == NULL)
        goto INVALID_HEAP_CONFIG;

    option = strstr(cmdline, "sz=");
    if(option == NULL)
        goto INVALID_HEAP_CONFIG;

    option = strstr(cmdline, "miu=");
    if(option == NULL)
        goto INVALID_HEAP_CONFIG;

    option = strstr(cmdline, "st=");
    if(option == NULL)
    {    
        sscanf(option, "miu=%d,sz=%lx,heap_id=%x", &heapconfig->miu, &heapconfig->size, &heapconfig->ipa_id);
        heapconfig->start = CMA_HEAP_MIUOFFSET_NOCARE;        
    }
    else
    {
        option = strstr(cmdline, "miu="); 
        sscanf(option, "miu=%d,sz=%lx,st=%lx,heap_id=%x", &heapconfig->miu, &heapconfig->size, 
                &heapconfig->start, &heapconfig->ipa_id);
    }
    
    BUG_ON(heapconfig->miu < 0 || heapconfig->miu > MAX_MIU_NUM);
    if(!GetReservedPhysicalAddr(heapconfig->miu, &heapconfig->start, &heapconfig->size, CMA_MEM_ALIGN))
        goto INVALID_HEAP_CONFIG;

    //printk(KERN_ERR "name %s ipa_id %d miu %d sz %lx\n", heapconfig->name, heapconfig->ipa_id,
    //    heapconfig->miu, heapconfig->size);

    return true;

INVALID_HEAP_CONFIG:
    heapconfig->size = 0;    
    return false;
}

/* pa config parse
  * pa args number maximux is 2, so it's name as below:
  *
  * PA_HEAP=pa_heap0,miu=0,sz=0x4000000,st=0x6800000
  * PA_HEAP=pa_heap1,...
  * PA_HEAP=pa_heap2,...
  */
static bool pa_para_config(char *cmdline, struct PA_BootArgs_Config * heapconfig) 
{
    char * option;
    int leng = 0;

    //printk(KERN_ERR "roger cmdline: %s, mstar_driver_boot_pa_num %d\n", cmdline, mstar_driver_boot_pa_num);

    option = strstr(cmdline, ",");
    leng = (int)(option - cmdline);
    if(leng > (IPA_NAME_LENG-1))
        leng = IPA_NAME_LENG - 1;

    strncpy(heapconfig->name, cmdline, leng);
    heapconfig->name[leng] = '\0';

    option = strstr(cmdline, "st=");
    if(option == NULL)
        goto INVALID_HEAP_CONFIG;

    option = strstr(cmdline, "sz=");
    if(option == NULL)
        goto INVALID_HEAP_CONFIG;

    option = strstr(cmdline, "miu=");
    if(option == NULL)
        goto INVALID_HEAP_CONFIG;

    sscanf(option, "miu=%d,sz=%lx,st=%lx", &heapconfig->miu, &heapconfig->size, &heapconfig->start);
    heapconfig->ipa_id = get_ipa_id(IPASPCMAN_HEAP_TYPE_PA, heapconfig->miu, 0);
    BUG_ON(heapconfig->miu < 0 || heapconfig->miu > MAX_MIU_NUM);
    
    if(!GetReservedPhysicalAddr(heapconfig->miu, &heapconfig->start, &heapconfig->size, PAGE_SIZE))
        goto INVALID_HEAP_CONFIG;

    //printk(KERN_ERR "name %s ipa_id %d miu %d start %lx sz %lx\n", heapconfig->name, heapconfig->ipa_id,
    //    heapconfig->miu, heapconfig->start, heapconfig->size);

    return true;

INVALID_HEAP_CONFIG:
    heapconfig->size = 0;
    return false;
}

/* ipa str pool config parse
  * ipa str pool args number maximux is 1, so it's name as below:
  *
  * IPA_STR_POOL=str,miu=1,heap_id=0x25,sz=0x50000, st=0xac00000
  * st means miu bus address.
  *
  */
static bool ipa_str_pool_para_config(char *cmdline, struct IPA_STR_POOL_BootArgs_Config * heapconfig) 
{
    char * option;
    int leng = 0;
    
    //printk(KERN_ERR "roger cmdline: %s, mstar_driver_boot_ipa_str_pool_num %d\n", cmdline, mstar_driver_boot_ipa_str_pool_num);
    
    option = strstr(cmdline, ",");
    leng = (int)(option - cmdline);
    if(leng > (IPA_NAME_LENG-1))
        leng = IPA_NAME_LENG - 1;
    
    strncpy(heapconfig->name, cmdline, leng);
    heapconfig->name[leng] = '\0';
    
    /*option = strstr(cmdline, "st=");
    if(option == NULL)
        goto INVALID_HEAP_CONFIG;
    
    option = strstr(cmdline, "sz=");
    if(option == NULL)
        goto INVALID_HEAP_CONFIG;
    
    option = strstr(cmdline, "heap_id=");
    if(option == NULL)
        goto INVALID_HEAP_CONFIG;
    
    option = strstr(cmdline, "miu=");
    if(option == NULL)
        goto INVALID_HEAP_CONFIG;*/

    if(sscanf(cmdline, "miu=%d,heap_id=%x,sz=%lx,st=%lx", &heapconfig->miu, &heapconfig->ipa_id, &heapconfig->size, 
        &heapconfig->start) !=4)
    {
         printk("%s:%d  para config fail :maybe parameters are not enough or may be sequence of parameters is not right\n",__FUNCTION__,__LINE__);
         goto INVALID_HEAP_CONFIG;
    }

    BUG_ON(heapconfig->miu < 0 || heapconfig->miu > MAX_MIU_NUM);
    
    //printk(KERN_ERR "name %s ipa_id %d miu %d sz %lx\n", heapconfig->name, heapconfig->ipa_id,
    //    heapconfig->miu, heapconfig->size);
    return true;
    
INVALID_HEAP_CONFIG:
    heapconfig->size = 0;
    return false;
}

static bool parse_heap_config(char *cmdline, void * heapconfig, 
                                enum IPASPCMAN_HEAP_TYPE type)
{
    bool ret = false;

    if(cmdline == NULL)
        return false;

    switch (type) 
    {            
        case IPASPCMAN_HEAP_TYPE_CMA:
            ret = ipa_cma_para_config(cmdline, heapconfig);
            break;
            
        case IPASPCMAN_HEAP_TYPE_PA:
            ret = pa_para_config(cmdline, heapconfig);
            break;
            
        default:
            ret = false;
    }
    
    BUG_ON(!ret);
    return ret;
}

int __init setup_ipa_cma_info(char *cmdline)
{
    if(!parse_heap_config(cmdline, (void *)(&ipa_cma_config[mstar_driver_boot_ipa_cma_num]), IPASPCMAN_HEAP_TYPE_CMA))        
        printk(KERN_ERR "error: ipa cma%d args invalid\n", mstar_driver_boot_ipa_cma_num);    
    else
        mstar_driver_boot_ipa_cma_num++;

    return 0;
}

early_param("IPA_CMA", setup_ipa_cma_info);

int __init setup_pa_info(char *cmdline)
{
    if(!parse_heap_config(cmdline, (void *)(&pa_config[mstar_driver_boot_pa_num]), IPASPCMAN_HEAP_TYPE_PA))        
        printk(KERN_ERR "error: pa %d args invalid\n", mstar_driver_boot_pa_num);    
    else
        mstar_driver_boot_pa_num++;

    return 0;
}

early_param("PA_HEAP", setup_pa_info);

int __init setup_ipa_str_pool_info(char *cmdline)
{
    if(!ipa_str_pool_para_config(cmdline, (void *)(&ipa_str_pool_config)))        
        printk(KERN_ERR "error: ipa_str_pool %d args invalid\n", mstar_driver_boot_ipa_str_pool_num);    
    else
    {
        mstar_driver_boot_ipa_str_pool_num++;
        if(mstar_driver_boot_ipa_str_pool_num > 1 )//total only have one "IPA_STR_POOL"
        {
            BUG();
        }
    }

    return 0;
}

early_param("IPA_STR_POOL", setup_ipa_str_pool_info);

extern phys_addr_t arm_lowmem_limit;
unsigned long __init _find_in_range(phys_addr_t start, phys_addr_t end, phys_addr_t size, phys_addr_t alignfactor)
{
    phys_addr_t ret = 0;

#ifdef CONFIG_64BIT
    BUG_ON(arm_lowmem_limit != 0);
#endif

    //for CONFIG_64BIT, arm_lowmem_limit = 0
    if((arm_lowmem_limit > start) && (arm_lowmem_limit < end))
    {
        /* find from lowmem first*/
        ret = memblock_find_in_range(start, arm_lowmem_limit, size, alignfactor);
        if(ret > 0)
        {
            //printk("\033[35mFunction = %s, Line = %d, find cma_buffer range from 0x%X to 0x%X\033[m\n", __PRETTY_FUNCTION__, __LINE__, start, arm_lowmem_limit);
            return ret;
        }

        /* find from highmem */
        ret = memblock_find_in_range(arm_lowmem_limit, end, size, alignfactor);
        if(ret > 0)
        {
            //printk("\033[35mFunction = %s, Line = %d, find cma_buffer range from 0x%X to 0x%X\033[m\n", __PRETTY_FUNCTION__, __LINE__, arm_lowmem_limit, end);
            return ret;      
        }
    }
    else if(end > start)
    {
        ret = memblock_find_in_range(start, end, size, alignfactor);
        if(ret > 0)
        {
            //printk("\033[35mFunction = %s, Line = %d, find cma_buffer range from 0x%X to 0x%X\033[m\n", __PRETTY_FUNCTION__, __LINE__, start, end);
            return ret;
        }
    }

    printk(CMA_ERR "\033[35mFunction = %s, Line = %d, ERROR!!\033[m\n", __PRETTY_FUNCTION__, __LINE__);
    return ret;
}

unsigned long __init find_start_addr(unsigned char miu, unsigned long size)
{
    unsigned long ret = 0;
    unsigned long alignfactor = pageblock_nr_pages*PAGE_SIZE;

    if(miu < 0 || miu > 3)
        return 0;

    switch (miu)
    {
        case 0:
        {
            ret = _find_in_range(ARM_MIU0_BUS_BASE, ARM_MIU1_BUS_BASE, size, alignfactor); 
            break;
        }
        case 1:
        {
            ret = _find_in_range(ARM_MIU1_BUS_BASE, ARM_MIU2_BUS_BASE, size, alignfactor);        
            break;
        }
        case 2:
        {
            ret = _find_in_range(ARM_MIU2_BUS_BASE, ARM_MIU3_BUS_BASE, size, alignfactor);       
            break;
        }
        case 3:
        {
            ret = _find_in_range(ARM_MIU3_BUS_BASE, CMA_HEAP_MIUOFFSET_NOCARE, size, alignfactor);        
            break;
        }
        default:
            return 0;
    }  

    return ret;
}


int __init get_cma_info(enum IPASPCMAN_HEAP_TYPE type, int index, int *miu, unsigned long **start, 
                            unsigned long *size, char ** name)
{
    int ret = 0;
    
    switch(type)
    {         
            case IPASPCMAN_HEAP_TYPE_CMA:
                {
                    *miu = ipa_cma_config[index].miu;
                    *start = &(ipa_cma_config[index].start);
                    *size = ipa_cma_config[index].size;  
                    *name = ipa_cma_config[index].name;
                }
                break; 
                
            case IPASPCMAN_HEAP_TYPE_PA:                    
            default:
                ret = -1;
     }

     return ret;
}

/**
 * mem_contiguous_reserve() - reserve area for contiguous memory handling
 * @limit: End address of the reserved memory (optional, 0 for any), in this fun
 * limit has no sense (with start address)
 *
 * This function reserves memory from early allocator. It should be
 * called by arch specific code once the early allocator (memblock or bootmem)
 * has been activated and all other subsystems have already allocated/reserved
 * memory.
 */
void __init mem_contiguous_reserve(enum IPASPCMAN_HEAP_TYPE type)
{
    int ret = 0, index = 0;
    phys_addr_t limit = ARM_MIU3_BUS_BASE; //no limit for reserve memory with start address  

    int miu = 0, boot_cma_num = 0;
    unsigned long * start = NULL, size = 0;
    struct device *declare_mstar_cma_device = NULL;
    char * name = NULL;
    
    BUG_ON(type != IPASPCMAN_HEAP_TYPE_CMA);
    switch(type)
    {                
            case IPASPCMAN_HEAP_TYPE_CMA:
                boot_cma_num = mstar_driver_boot_ipa_cma_num;
                declare_mstar_cma_device = &ipa_cma_device[0];
                break; 
                
            case IPASPCMAN_HEAP_TYPE_PA:   //no cma associated with it               
            default:
                BUG_ON(1);
    }

    /*
         * add cma buffer from bootargs, and assigne it to the specific device
         * cma buffer is not limited in low memory, also can locate in high memory 
         */  
    index = 0;
    while(index < boot_cma_num)
    {
        
        ret = get_cma_info(type, index, &miu, &start, &size, &name);
        //printk(KERN_ERR "index %d miu %d, start %ld, size %lx, name %s\n", index, miu, (long)(*start), size, name);
        BUG_ON(ret || (size == 0));
        
        if(CMA_HEAP_MIUOFFSET_NOCARE == *start)
        {
            *start = find_start_addr(miu, size);
            BUG_ON(*start == 0);
        }

        
        /* for 64BIT arm_lowmem_limit = 0
         * check if the reserved memory allocated across 2 memory zones: a part of it in normal zone, the other in high memory
         */
        #ifdef CONFIG_64BIT
        BUG_ON(arm_lowmem_limit != 0);
        #endif
        if(*start > 0)
        {
            if((*start < arm_lowmem_limit)
                && (*start + size > arm_lowmem_limit))
            {
                printk(CMA_ERR "Warning: reserved memory allocated across 2 memory zones!!!=========\n");
                printk(CMA_ERR "start %lx size %lx arm_low_limit %llx\n", *start, size, (u64)arm_lowmem_limit);
                BUG_ON(1);
            }
        }

        ret = dma_declare_contiguous(declare_mstar_cma_device, size, *start, limit);
        declare_mstar_cma_device->coherent_dma_mask = ~0;	// not sure, this mask will be used in __dma_alloc while doing cma_alloc, 0xFFFFFFFF is for NULL device

        if(ret)
        {
            printk(CMA_ERR "error: reserve memory fail, start %lu size %lu\n", *start, size);
            BUG_ON(ret);
        }

        declare_mstar_cma_device->init_name = name;        
        index++;
    }    
}

/*
  * reserve memory for ipa 
  * include: ipa_cma
  */
void __init ipa_contiguous_reserve(void)
{
    if(mstar_driver_boot_ipa_cma_num > 0)
        mem_contiguous_reserve(IPASPCMAN_HEAP_TYPE_CMA);
}

