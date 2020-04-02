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
/// @file   ipa_cma.c
/// @brief  CMA & miu kernel protect interface
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <linux/version.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/debugfs.h>
#include <asm/uaccess.h>
#include <asm/dma-contiguous.h>
#include <linux/dma-mapping.h>
#include <linux/proc_fs.h>
#include <ion.h>
#include <ipa_priv.h>
#include <chip_setup.h>
#include <ipa_pool.h>
#include <ipa_bootargs.h>
#include <mdrv_miu.h>
#include "mdrv_types.h"
#include "mdrv_system.h"
#include "ipa_cma.h"

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define IPA_CMA_BUG_ON(cond)  \
do { \
      if(cond) \
        printk(CMA_ERR "IPA_CMA_BUG in %s @ %d \n", __FUNCTION__, __LINE__); \
      BUG_ON(cond); \
   } while(0)

#define IPA_CMA_ERR(fmt, args...) printk(CMA_ERR "IPA_CMA error %s:%d " fmt,__FUNCTION__,__LINE__,## args)
#define IPA_CMA_WARNING(fmt, args...) printk(CMA_ERR "IPA_CMA warning %s:%d " fmt,__FUNCTION__,__LINE__,## args)
//#define IPA_CMA_TEST       1
//#define IPA_CMA_DEBUG      1

//-------------------------------------------------------------------------------------------------
// Type and Structure Declaration
//-------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// Forward declaration
//--------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
// Local Variables
//-------------------------------------------------------------------------------------------------
static struct MIU_ProtectRanges glob_miu_kranges[MIU_NUM]; //all heaps share this struct
static atomic_t kprotect_enabled = ATOMIC_INIT(1);         //kernel protect default enabled
static MS_U16 *kernal_protect_client_id = NULL;     //miu protect info
static struct ipa_cma_para * para = NULL;                  //para register with ops

static inline void phy_to_MiuOffset(unsigned long phy_addr, unsigned int *miu, unsigned long *offset)
{
    *miu = INVALID_MIU;

    if(phy_addr >= ARM_MIU2_BUS_BASE)
    {
        *miu = 2;
        *offset = phy_addr - ARM_MIU2_BUS_BASE;
    }
    else if(phy_addr >= ARM_MIU1_BUS_BASE)
    {
        *miu = 1;
        *offset = phy_addr - ARM_MIU1_BUS_BASE;
    }
    else if(phy_addr >= ARM_MIU0_BUS_BASE)
    {
        *miu = 0;
        *offset = phy_addr - ARM_MIU0_BUS_BASE;
    }
    else
    {
        printk(CMA_ERR "\033[35mFunction = %s, Line = %d, Error, Unknown MIU, for phy_addr is 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, phy_addr);
        IPA_CMA_BUG_ON(true);
    }
}

//before call this API, lock heap info mutex firstly
static struct cma_buffer * SplitBuffer(unsigned long start_pa, unsigned long length, struct cma_buffer *find,
        struct cma_alloc_list *alloc_list, BUFF_OPS ops)
{
    struct cma_buffer *buffer_node = NULL;
    IPA_CMA_BUG_ON(!find || start_pa < find->start_pa || ((start_pa + length) > (find->start_pa + find->length)));
    if(ops == CMA_ALLOC)
        IPA_CMA_BUG_ON(find->freed == 0);
    else
        IPA_CMA_BUG_ON(find->freed == 1);

    //at begginning of found buffer
    if(start_pa == find->start_pa)
    {
        if(length == find->length)
        {
           /* bufffer_node(start_pa, length)  =   find
            *  |------------------------------------|
            */
            buffer_node = find;
            if(ops == CMA_ALLOC)
            {
                buffer_node->freed = 0;
                alloc_list->freed_count--;
                alloc_list->using_count++;
            }
            else
            {
                alloc_list->freed_count++;
                alloc_list->using_count--;
                buffer_node->freed = 1;
                //buffer_node->pid = INVALID_PID;
            }
        }
        else
        {
           /* bufffer_node(start_pa, length)  find
            *  |----------------------------|--------|
            */
            buffer_node = (struct cma_buffer *)kzalloc(sizeof(struct cma_buffer), GFP_KERNEL);
            IPA_CMA_BUG_ON(!buffer_node);
            buffer_node->start_pa = find->start_pa;	// buffer_noda specify the freed buffer
            buffer_node->length = length;
            buffer_node->page = find->page;
            INIT_LIST_HEAD(&buffer_node->list);

            find->start_pa = buffer_node->start_pa + buffer_node->length;	// adjust the freed buffer
            find->length -= buffer_node->length;
            find->page = pfn_to_page(page_to_pfn(buffer_node->page) + (buffer_node->length >> PAGE_SHIFT));
            list_add(&buffer_node->list, find->list.prev); //insert new node before find node (list_add(source, to))

            if(ops == CMA_ALLOC)
            {
                buffer_node->freed = 0;
                alloc_list->using_count++;
            }
            else
            {
                alloc_list->freed_count++;
                buffer_node->freed = 1;
                //buffer_node->pid = INVALID_PID;
            }
        }
    }
    else if((start_pa+length) == (find->start_pa+find->length)) //at end of found buffer
    {
        /* find         bufffer_node(start_pa, length)
         *  |-------|----------------------------|
         */
        find->length -= length;
        buffer_node = (struct cma_buffer *)kzalloc(sizeof(struct cma_buffer), GFP_KERNEL);
        IPA_CMA_BUG_ON(!buffer_node);
        buffer_node->start_pa = find->start_pa + find->length;
        buffer_node->length = length;
        buffer_node->page = pfn_to_page(page_to_pfn(find->page) + (find->length>>PAGE_SHIFT));
        INIT_LIST_HEAD(&buffer_node->list);
        list_add(&buffer_node->list, &find->list); //insert new node after find node

        if(ops == CMA_ALLOC)
        {
            buffer_node->freed = 0;
            alloc_list->using_count++;
        }
        else
        {
            alloc_list->freed_count++;
            buffer_node->freed = 1;
            //buffer_node->pid = INVALID_PID;
        }
    }
    else //in the middle(into 2 find buffers and one ready_to_be_allocated buffer)
    {
        /*    find   bufffer_node(start_pa, length) buffer_node1
              *  |-------|----------------------------|-------------|
              */
        struct cma_buffer *buffer_node1 = NULL;
        unsigned long total_len = find->length;

        find->length = start_pa - find->start_pa;	// leftest buffer

        buffer_node = (struct cma_buffer *)kzalloc(sizeof(struct cma_buffer), GFP_KERNEL);	// ready_to_be_allocated buffer
        IPA_CMA_BUG_ON(!buffer_node);
        buffer_node->start_pa = find->start_pa + find->length;
        buffer_node->length = length;
        buffer_node->page = pfn_to_page(page_to_pfn(find->page) + (find->length >> PAGE_SHIFT));
        INIT_LIST_HEAD(&buffer_node->list);
        list_add(&buffer_node->list, &find->list); //insert new node after find node

        buffer_node1 = (struct cma_buffer *)kzalloc(sizeof(struct cma_buffer), GFP_KERNEL);	// rightest buffer
        IPA_CMA_BUG_ON(!buffer_node1);
        buffer_node1->start_pa = buffer_node->start_pa + buffer_node->length;
        buffer_node1->length = total_len - buffer_node->length - find->length;
        buffer_node1->page = pfn_to_page(page_to_pfn(buffer_node->page) + (buffer_node->length >> PAGE_SHIFT));
        INIT_LIST_HEAD(&buffer_node1->list);
        list_add(&buffer_node1->list, &buffer_node->list); //insert new node after find node

        if(ops == CMA_ALLOC)
        {
            buffer_node->freed = 0;
            alloc_list->using_count++;
            buffer_node1->freed = 1;
            alloc_list->freed_count++;
        }
        else
        {
            alloc_list->freed_count++;
            buffer_node1->freed = 0;
            alloc_list->using_count++;

            buffer_node->freed = 1;
            //buffer_node->pid = INVALID_PID;
        }
    }

    return buffer_node;
}

//before call this API, lock heap info mutex firstly
static struct cma_buffer * _alloc_from_freelist(struct cma_alloc_list * cma_buf_list,
                                                unsigned long start_pa, size_t length)
{
    struct cma_buffer *buffer_node, *find = NULL;

    if(cma_buf_list->freed_count <= 0)
        return NULL;

    //find the start address in free list
    list_for_each_entry(buffer_node, &cma_buf_list->list_head, list)
    {
        //not freed buffer
        if(buffer_node->freed == 0)
            continue;

        // check if this allocation_range is located at this freed buffer
        if((buffer_node->start_pa<=start_pa)
            && ((start_pa - buffer_node->start_pa + length) <= buffer_node->length))
        {
            find = buffer_node;
            break;
        }
    }

    if(!find)
    {
        buffer_node = NULL;
    }
    else
    {
        buffer_node = SplitBuffer(start_pa, length, find, cma_buf_list, CMA_ALLOC);	// split the free buffer to "ready_to_be_allocated buffer" and "free_buffer"
    }

    return buffer_node;
}

//find idle miu protect block index
static int idleBlockIndx(struct MIU_ProtectRanges * pranges)
{
    int index = 0;

    for(index = 0; index < MIU_BLOCK_NUM; ++index)
    {
        if(pranges->miuBlockStatus[index] == MIU_BLOCK_IDLE)
        {
            return index;
        }
    }

    return -1;
}

static void dumpKRange(struct MIU_ProtectRanges *pranges)
{
#ifdef IPA_CMA_KERNEL_PROT_DUMP
    MIU_ProtectRange *range = NULL;

    mutex_lock(&pranges->lock);
    list_for_each_entry(range, &pranges->list_head, list_node)
    {
        printk(CMA_ERR "range start 0x%lX to 0x%lX, length 0x%lX miublock_index %d\n",
            range->start_pa, (range->start_pa+range->length), range->length, range->miuBlockIndex);
    }
    mutex_unlock(&pranges->lock);
#endif
}

static bool _miu_kernel_protect(unsigned char miuBlockIndex, unsigned char *pu8ProtectId,
    unsigned long start, unsigned long end, int flag)
{
    bool ret = true;

    if(atomic_read(&kprotect_enabled) > 0)
        ret = MDrv_MIU_Protect(miuBlockIndex, kernal_protect_client_id, start, end, flag);
    else
        printk(CMA_ERR "ignore kernel protect\n");

    return ret;
}

//when alloc from cma heap, call this API to deleteKRange of this allocted buffer
static int deleteKRange(struct ipa_cma_para * para, struct cma_buffer * buffer)
{
    struct MIU_ProtectRanges * pranges = para->miu_pro_sta;
    MIU_ProtectRange * range,  * r_front = NULL, * r_back= NULL;
    MIU_ProtectRange old;
    unsigned long r_front_len = 0, r_back_len = 0;
    int miuBlockIndex = -1;
    bool find = false, protect_ret = false;
    int ret = 0;

    if(buffer->length == 0)
        return 0;

    /*
         * kernel protect range( before allocate buffer)
         *
         * |--------------------------------|
         *
         * kernel protect range(buffer location in this range, after buffer allocated)
         *  r_front        allocated buffer    r_back
         *
         * |------|=============|-------|
         *
         * case: r_front = 0;
         *          r_back = 0;
         *          r_front=r_back=0;
         */
    mutex_lock(&pranges->lock);
    list_for_each_entry(range, &pranges->list_head, list_node)
    {
        if((buffer->start_pa >= range->start_pa)
            && ((buffer->start_pa+buffer->length) <= (range->start_pa+range->length)))
        {
            find = true;
            old.start_pa = range->start_pa;
            old.length = range->length;
            old.miuBlockIndex = range->miuBlockIndex;
            break;
        }
    }

    if(!find)
    {
       ret = Error_Unknown;
       IPA_CMA_ERR("not find the buffer: start_pa %lx length %lu\n", buffer->start_pa, buffer->length);
       goto DELETE_KRANGE_DONE;
    }

    r_front_len = buffer->start_pa - range->start_pa;
    r_back_len = range->start_pa + range->length - (buffer->start_pa + buffer->length);

    if((r_front_len != 0) && (r_back_len != 0))
    {
        miuBlockIndex = idleBlockIndx(pranges);
        if(miuBlockIndex < 0)
        {
           ret = Error_No_MIU_Prot_index;
           IPA_CMA_ERR("no idle miu protect block in miu %d\n", para->miu);
           IPA_CMA_BUG_ON(ret);
           goto DELETE_KRANGE_DONE;
        }

        r_back = (MIU_ProtectRange *)kzalloc(sizeof(MIU_ProtectRange), GFP_KERNEL);
        if(!r_back)
        {
           ret = Error_No_Mem;
           IPA_CMA_ERR("no memory\n");
           goto DELETE_KRANGE_DONE;
        }

        r_front = range;
        r_front->length = r_front_len;

        r_back->start_pa = buffer->start_pa + buffer->length;
        r_back->length = r_back_len;
        r_back->miuBlockIndex = miuBlockIndex;
        INIT_LIST_HEAD(&r_back->list_node);
        list_add(&r_back->list_node, &r_front->list_node); //add back node after front(found range node)
        pranges->krange_num++;
    }
    else if(r_front_len != 0) //and (r_back_len == 0)
    {
        r_front = range;
        r_front->length = r_front_len;
    }
    else if(r_back_len != 0) //and (r_front_len == 0)
    {
        r_back = range;
        r_back->start_pa = buffer->start_pa + buffer->length;
        r_back->length = r_back_len;
    }
    else //((r_front_len == 0) && (r_back_len == 0))
    {
        list_del(&range->list_node);
        kfree(range);
        pranges->krange_num--;
    }

    protect_ret = _miu_kernel_protect(old.miuBlockIndex, kernal_protect_client_id, old.start_pa,
        old.start_pa + old.length, MIU_PROTECT_DISABLE);
    IPA_CMA_BUG_ON(!protect_ret);
    pranges->miuBlockStatus[old.miuBlockIndex] = MIU_BLOCK_IDLE;

    if(r_front)
    {
        protect_ret = _miu_kernel_protect(r_front->miuBlockIndex, kernal_protect_client_id,
            r_front->start_pa, r_front->start_pa+r_front->length, MIU_PROTECT_ENABLE);
        IPA_CMA_BUG_ON(!protect_ret);
        pranges->miuBlockStatus[r_front->miuBlockIndex] = MIU_BLOCK_BUSY;
    }

    if(r_back)
    {
        protect_ret = _miu_kernel_protect(r_back->miuBlockIndex, kernal_protect_client_id,
            r_back->start_pa, r_back->start_pa+r_back->length, MIU_PROTECT_ENABLE);
        IPA_CMA_BUG_ON(!protect_ret);
        pranges->miuBlockStatus[r_back->miuBlockIndex] = MIU_BLOCK_BUSY;
    }

DELETE_KRANGE_DONE:
    mutex_unlock(&pranges->lock);
    return ret;
}

// when free to cma heap, call this API to add KRange of this allocted buffer
static int addKRange(struct MIU_ProtectRanges *pranges , struct cma_buffer *buffer)
{
    MIU_ProtectRange *r_prev = NULL, *r_next= NULL;
    MIU_ProtectRange *range;
    int miuBlockIndex = -1;
    bool protect_ret = false;
    int ret = 0;

    if(buffer->length == 0)
        return 0;

    /*
         * kernel protect range (before freed buffer)
         *      r_prev       allocated buffer                             r_next
         * |-------------|====================|------------|
         *
         * kernel protect range(freed buffer location in this range)
         *   r_prev         freed buffer          r_next
         * |--------|?-------------?|-------|
         *
         */
    mutex_lock(&pranges->lock);
    list_for_each_entry(range, &pranges->list_head, list_node)	// find this miu all kernel_protect setting(range)
    {
        if((range->start_pa + range->length) <= buffer->start_pa)
        {
            r_prev = range;
            continue;   // should be continue, we are going to find a nearest one k_range before this buffer
        }
    }

    if(r_prev)  // find a kernel_protect range before this buffer
    {
        if(!list_is_last(&r_prev->list_node,&pranges->list_head))
        {
            r_next = container_of(r_prev->list_node.next, MIU_ProtectRange, list_node);  // if prev_krange is not the last one, the next one krange will be r_next

        }
    }
    else    // no kernel_protect range before this buffer ==> all k_range is behind this buffer
    {
        if(list_empty(&pranges->list_head))
            r_next = NULL;
        else
            r_next = list_first_entry(&pranges->list_head, MIU_ProtectRange, list_node);	// r_next will be first krange
    }

    //till now, find the prev range and next range of buffer freed
    if(r_prev && r_next)
    {
        if(((r_prev->start_pa + r_prev->length) == buffer->start_pa)
            && ((buffer->start_pa + buffer->length) == r_next->start_pa))	// the buffer is just the hole between r_prev and r_next
        {
            // disable r_prev
            protect_ret = _miu_kernel_protect(r_prev->miuBlockIndex, kernal_protect_client_id,
                r_prev->start_pa, r_prev->start_pa + r_prev->length, MIU_PROTECT_DISABLE);
            IPA_CMA_BUG_ON(!protect_ret);

            // disable r_next
            protect_ret = _miu_kernel_protect(r_next->miuBlockIndex, kernal_protect_client_id,
                r_next->start_pa, r_next->start_pa + r_next->length, MIU_PROTECT_DISABLE);
            IPA_CMA_BUG_ON(!protect_ret);
            pranges->miuBlockStatus[r_next->miuBlockIndex] = MIU_BLOCK_IDLE;    // mark a k_range is available

            r_prev->length += (r_next->length + buffer->length);                // extend the r_prev length, and protect it
            protect_ret = _miu_kernel_protect(r_prev->miuBlockIndex, kernal_protect_client_id,
                r_prev->start_pa, r_prev->start_pa + r_prev->length, MIU_PROTECT_ENABLE);
            IPA_CMA_BUG_ON(!protect_ret);

            list_del(&r_next->list_node);
            kfree(r_next);
            pranges->krange_num--;

            goto ADD_KRANGE_DONE;
        }
    }

    if(r_prev)
    {
        if((r_prev->start_pa + r_prev->length) == buffer->start_pa)
        {
            protect_ret = _miu_kernel_protect(r_prev->miuBlockIndex, kernal_protect_client_id,
                r_prev->start_pa, r_prev->start_pa + r_prev->length, MIU_PROTECT_DISABLE);
            IPA_CMA_BUG_ON(!protect_ret);

            r_prev->length += buffer->length;
            protect_ret = _miu_kernel_protect(r_prev->miuBlockIndex, kernal_protect_client_id,
                r_prev->start_pa, r_prev->start_pa + r_prev->length, MIU_PROTECT_ENABLE);
            IPA_CMA_BUG_ON(!protect_ret);

            goto ADD_KRANGE_DONE;
        }
    }

    if(r_next)
    {
        if((buffer->start_pa + buffer->length) == r_next->start_pa)
        {
            protect_ret = _miu_kernel_protect(r_next->miuBlockIndex, kernal_protect_client_id,
                r_next->start_pa, r_next->start_pa + r_next->length, MIU_PROTECT_DISABLE);
            IPA_CMA_BUG_ON(!protect_ret);

            r_next->start_pa = buffer->start_pa;
            r_next->length += buffer->length;
            protect_ret = _miu_kernel_protect(r_next->miuBlockIndex, kernal_protect_client_id,
                r_next->start_pa, r_next->start_pa + r_next->length, MIU_PROTECT_ENABLE);
            IPA_CMA_BUG_ON(!protect_ret);

            goto ADD_KRANGE_DONE;
        }
    }

    // use a new k_range for this buffer
    miuBlockIndex = idleBlockIndx(pranges);
    if(miuBlockIndex < 0)
    {
       ret = Error_No_MIU_Prot_index;
       IPA_CMA_ERR("no idle miu protect block in miu %d\n", (int)pranges->miu);
       IPA_CMA_BUG_ON(ret);
       goto ADD_KRANGE_DONE;
    }
    printk(CMA_DEBUG "\033[35mFunction = %s, Line = %d, use a new k_range for this buffer, miu_protect %d for 0x%lX to 0x%lX\033[m\n", __PRETTY_FUNCTION__, __LINE__, miuBlockIndex, buffer->start_pa, (buffer->start_pa+buffer->length));

    range = (MIU_ProtectRange *)kzalloc(sizeof(MIU_ProtectRange), GFP_KERNEL);
    if(!range)
    {
       ret = Error_No_Mem;
       printk(CMA_ERR "no memory\n");
       goto ADD_KRANGE_DONE;
    }
    range->start_pa = buffer->start_pa;
    range->length = buffer->length;
    range->miuBlockIndex = miuBlockIndex;
    INIT_LIST_HEAD(&range->list_node);
    if(r_prev)
        list_add(&range->list_node, &r_prev->list_node);
    else
        list_add(&range->list_node, &pranges->list_head);

    protect_ret = _miu_kernel_protect(range->miuBlockIndex, kernal_protect_client_id,
        range->start_pa, range->start_pa + range->length, MIU_PROTECT_ENABLE);
    IPA_CMA_BUG_ON(!protect_ret);
    pranges->miuBlockStatus[range->miuBlockIndex] = MIU_BLOCK_BUSY;
    pranges->krange_num++;

ADD_KRANGE_DONE:
    mutex_unlock(&pranges->lock);
    return ret;
}

static void dump_CMA_Pool(struct ipa_cma_para *para, bool force_print)
{
    struct cma_alloc_list *buffer_list = &para->cma_buf_list;
    struct cma_buffer *buffer = NULL;

    if(!force_print)
    {
        #ifndef IPA_CMA_KERNEL_PROT_DUMP
        return;
        #endif
    }

    printk(CMA_ERR "dump ipa_id %d information: min_start 0x%lX, max_end 0x%lX\n",
            para->ipa_id, buffer_list->min_start, buffer_list->max_end);
    list_for_each_entry(buffer, &buffer_list->list_head, list)
    {
        printk(CMA_ERR "allocated info: phy_addr 0x%lX, length 0x%lX, freed %d\n",
            buffer->start_pa, buffer->length, buffer->freed);
    }
}

/*
  * merge if possible
  * in this list buffer->start_pa sorted as ascending order
  */
static int merge_cma_buffer(struct cma_alloc_list * alloc_list)
{
    struct cma_buffer *node = NULL, *next = NULL;

    IPA_CMA_BUG_ON(alloc_list == NULL);

    if(list_empty(&alloc_list->list_head))
        return 0;

    list_for_each_entry_safe(node, next, &alloc_list->list_head, list)
    {
       if(list_is_last(&node->list, &alloc_list->list_head)) //node is last
          break;

       if((node->freed == next->freed)
           && ((node->start_pa + node->length) == next->start_pa))
       {
           next->start_pa = node->start_pa;
           next->length += node->length;
           next->page = node->page;

           list_del(&node->list);
           kfree(node);

           if(next->freed > 0)
              alloc_list->freed_count--;
           else
              alloc_list->using_count--;
       }
    }

    return 0;
}

extern void drop_slab(void);
extern void drop_pagecache_sb(struct super_block *sb, void *unused);
static struct page * CMA_Pool_Alloc(unsigned long offset_in_heap,
                                unsigned long length, unsigned long align, struct ipa_cma_para * para)
{
    int try_num = 0, ret = 0;
    bool alloc_at_front = false;
    struct page * page = NULL, * alloc_page = NULL;
    unsigned long phy_addr = 0, start_pa = 0;
    struct cma_buffer *buffer = NULL, *buffer_hole = NULL;
    struct cma_alloc_list * alloc_list = &para->cma_buf_list;
    unsigned long length_buffer_hole = 0;
    bool find_in_freelist = false;
    struct cma * cma_area = NULL;
    signed long timeout;

    IPA_CMA_BUG_ON(!IS_ALIGNED(offset_in_heap, MIU_PROT_ALIGN_SIZE));
    IPA_CMA_BUG_ON(!IS_ALIGNED(length, MIU_PROT_ALIGN_SIZE));

    cma_area = para->dev->cma_area;
    start_pa = offset_in_heap + PFN_PHYS(para->dev->cma_area->base_pfn);  // the start_allocation_addr (cpu bus address)

    mutex_lock(&para->ipa_cma_mutex);
    //find firstly in free list(buffer means a cma_alloc_result => from allocation_start to allocation_end, and mark if freed or not)
    if(alloc_list->freed_count > 0)
    {
        buffer = _alloc_from_freelist(alloc_list, start_pa, length);
    }

    //if not find in free list, get from cma heap directly
    if(!buffer)
    {
        phy_addr = start_pa;
        //check if input parameters out of cma heap range
        if(((phy_addr + length) > PFN_PHYS(cma_area->base_pfn + cma_area->count))
            || (phy_addr < PFN_PHYS(cma_area->base_pfn)))
        {
            ret = Error_Invalid_Para;
            IPA_CMA_ERR("invalid parameter ipa id %d:offset_in_heap 0x%lX ,max end 0x%lX, base_phy 0x%lX\n",
                           para->ipa_id, offset_in_heap, alloc_list->max_end, (unsigned long)PFN_PHYS(para->dev->cma_area->base_pfn));
            goto ALLOCAT_BUFF_FAIL;
        }

        //if not find in freelist previously, here it [start, length] should not locate in the middle of allocation_list
        if (alloc_list->min_start != PHYSICAL_START_INIT)
        {
            if (((phy_addr >= alloc_list->min_start) && (phy_addr < alloc_list->max_end))
                || (((phy_addr+length) > alloc_list->min_start) && ((phy_addr+length) <= alloc_list->max_end)))
            {
                ret = Error_Invalid_Para;
                IPA_CMA_ERR("invalid start address or length:  ipa id %u, min_start 0x%lX , max_end 0x%lX, phy_addr 0x%lX, length 0x%lX\n\n",
                           para->ipa_id, alloc_list->min_start, alloc_list->max_end,
                           phy_addr, length);
                dump_CMA_Pool(para, true);
                goto ALLOCAT_BUFF_FAIL;
            }

            if(phy_addr < alloc_list->min_start)
                alloc_at_front = true;

            /*
                      *   new buffer       hole   min_start                                   max_end
                      * |xxxxxxxxx|---------|======================|
                      */
            if ((phy_addr+length) < alloc_list->min_start) // font hole
            {
                length_buffer_hole = alloc_list->min_start - (phy_addr + length);
                length = alloc_list->min_start - phy_addr;  // adjust allocation length to cover the hole

                buffer_hole = (struct cma_buffer *)kzalloc(sizeof(struct cma_buffer), GFP_KERNEL);
                IPA_CMA_BUG_ON(!buffer_hole);
                INIT_LIST_HEAD(&buffer_hole->list);

                buffer_hole->freed = 1;                                 // set this addition cma_buffer(hole) to be freed
                buffer_hole->start_pa = phy_addr + length - length_buffer_hole; // hole_start_pa
                buffer_hole->length = length_buffer_hole;               // hole_length
                // this hole(buffer_hole) will be allocated, but no one can use this(allcated in new buffer, the new_buffer will be prolonged to cover hole)
            }
            else if(phy_addr > alloc_list->max_end)
            {
            /*
                      * min_start                               max_end    hole        new buffer
                      * |======================|---------------|xxxxxxxxxxxxxx|
                      */
                length_buffer_hole = phy_addr - alloc_list->max_end;  // hole_length
                length += length_buffer_hole;                   // adjust length to cover this hole
                phy_addr = alloc_list->max_end;       // adjust alloc_start_addr to hole_start_addr

                buffer_hole = (struct cma_buffer *)kzalloc(sizeof(struct cma_buffer), GFP_KERNEL);
                IPA_CMA_BUG_ON(!buffer_hole);
                INIT_LIST_HEAD(&buffer_hole->list);

                buffer_hole->freed = 1;
                buffer_hole->start_pa = alloc_list->max_end;
                buffer_hole->length = length_buffer_hole;
            }
        }

ALLOC_RETRY:
        //page = dma_alloc_from_contiguous_addr(heap_info->dev, phy_addr, length>>PAGE_SHIFT, 1);
        page = dma_alloc_at_from_contiguous(para->dev, length>>PAGE_SHIFT, 1, phy_addr);
		__dma_flush_buffer(page, (length>>PAGE_SHIFT) << PAGE_SHIFT);
        if(!page)
        {
            if(try_num < MAX_ALLOC_TRY)
            {
                try_num++;
                iterate_supers(drop_pagecache_sb, NULL);
                drop_slab();
                timeout = schedule_timeout_interruptible(msecs_to_jiffies(1000));
                goto ALLOC_RETRY;
            }

            IPA_CMA_ERR("Fail to allocate buffer\n");
            printk(CMA_ERR "\033[35mFunction = %s, Line = %d, Error, Please Check WHY dma_alloc_at_from_contiguous failed (maybe no memory left)!! \033[m\n", __PRETTY_FUNCTION__, __LINE__);
            ret = Error_No_Mem;
            goto ALLOCAT_BUFF_FAIL;
        }
        phy_addr = pfn_to_dma(para->dev, page_to_pfn(page));
        buffer = (struct cma_buffer *)kzalloc(sizeof(struct cma_buffer), GFP_KERNEL);
        IPA_CMA_BUG_ON(!buffer);
        buffer->start_pa = phy_addr;
        buffer->length = length;
        buffer->page = page;
        INIT_LIST_HEAD(&buffer->list);
        buffer->freed = 0;

        //insert the buffer node into alloc_list(be in order)
        if(alloc_at_front)
            list_add(&buffer->list, &alloc_list->list_head);
        else
            list_add_tail(&buffer->list, &alloc_list->list_head);
        alloc_list->using_count++;
    }// if not find in free list, get from cma heap directly
    else  //find in free list
    {
        find_in_freelist = true;
    }

    //buffer->pid = task_pid_nr(current->group_leader);

    // find in freelist, so alloc_list max_end and min_start does't need to be changed
    // here to update max_end and min_start addr
    if(!find_in_freelist)
    {
        if(buffer->start_pa < alloc_list->min_start)
            alloc_list->min_start = buffer->start_pa;
        if((buffer->start_pa+buffer->length) > alloc_list->max_end)
            alloc_list->max_end = buffer->start_pa + buffer->length;
    }

    if(!find_in_freelist)
    {
        ret = deleteKRange(para, buffer);  // this buffer will contain hole_buffer(if hole_buffer exists)
        if(ret)
        {
            IPA_CMA_ERR("update kernal range fail\n");
            goto ALLOCAT_BUFF_FAIL;
        }

        if(length_buffer_hole > 0)  // this means we have holes during this allocation, we will reserve this hole(cma_alloc, done by above dma_alloc_from_contiguous_addr()), but we do not mapping this hole(CMA_Pool_Mapping)
        {
            if (alloc_at_front)
            {
                /* adjust the real addr and length for buffer and buffer_hole */
                buffer->length -= length_buffer_hole;
                //FIXME: test this
                buffer_hole->page = pfn_to_page(page_to_pfn(buffer->page) + (buffer->length >> PAGE_SHIFT)); // sparse memory module
                list_add(&buffer_hole->list, &buffer->list);    // add to buffer lists
            }
            else
            {
                /* adjust the real addr and length for buffer and buffer_hole */
                buffer_hole->page = buffer->page;
                buffer->start_pa += buffer_hole->length;
                buffer->page = pfn_to_page(page_to_pfn(buffer->page) + (buffer_hole->length >> PAGE_SHIFT)); //sparse memory module
                buffer->length -= buffer_hole->length;
                list_add(&buffer_hole->list, buffer->list.prev);    // add to buffer lists
            }
            alloc_list->freed_count++;            // add this for allocating freed hole_buffer
        }
    }

    if(buffer)
        alloc_page = buffer->page;
    merge_cma_buffer(alloc_list);

ALLOCAT_BUFF_FAIL:
    dump_CMA_Pool(para, true);
    dumpKRange(para->miu_pro_sta);
    mutex_unlock(&para->ipa_cma_mutex);

    return alloc_page;
}

//before call this API, lock heap info mutex firstly
static void free_buffer_list(struct cma_alloc_list * alloc_list, struct cma_buffer **release_buf_front , struct cma_buffer **release_buf_back)
{
    struct cma_buffer *buffer = NULL, *next = NULL;
    struct cma_buffer *front = NULL, *back = NULL;
    bool min_max_change = false;

    *release_buf_front = *release_buf_back = NULL;
    if(list_empty(&alloc_list->list_head))
        return;

    //merge adjacent node all are free
    list_for_each_entry_safe(buffer, next, &alloc_list->list_head, list)   // from this heap_info, search every buffer
    {
        if((buffer->freed == 1) && (next->freed == 1))                      // in current case, buffers must be neighbor, so, merge contiguous freed buffers
        {
            if(buffer->start_pa + buffer->length == next->start_pa)
            {
                list_del(&buffer->list);
                next->start_pa = buffer->start_pa;
                next->page = buffer->page;
                next->length += buffer->length;
                alloc_list->freed_count--;
                kfree(buffer);
            }
        }
    }

    /* case A: freed buffer locate at beginning of alloc list */
    if(list_empty(&alloc_list->list_head))
    {
        printk(CMA_DEBUG "\033[35mFunction = %s, Line = %d\033[m\n", __PRETTY_FUNCTION__, __LINE__);
        goto UPDATE_MIN_MAX;
    }

    front = list_entry(alloc_list->list_head.next, struct cma_buffer, list); // check if 1st buffer is free? we can only free 1st or last_one buffer because kernel protect limitation
    if(front->freed == 1)
    {
        list_del(&front->list); // remove form list
        alloc_list->freed_count--;
        min_max_change = true;
        *release_buf_front = front;
    }

    /*case B: freed buffer locate at end of kernel range*/
    if(list_empty(&alloc_list->list_head))
        goto UPDATE_MIN_MAX;

    back = list_entry(alloc_list->list_head.prev, struct cma_buffer, list);  // check if last_one buffer is free? we can only free 1st or last_one buffer because kernel protect limitation
    if(back->freed == 1)
    {
        list_del(&back->list);
        alloc_list->freed_count--;
        min_max_change = true;
        *release_buf_back = back;
    }

UPDATE_MIN_MAX:
    if(list_empty(&alloc_list->list_head)) // empty, no buffer
    {
        alloc_list->min_start = PHYSICAL_START_INIT;
        alloc_list->max_end = PHYSICAL_END_INIT;
    }
    else if(min_max_change)
    {
        buffer = list_entry(alloc_list->list_head.next, struct cma_buffer, list);
        alloc_list->min_start = buffer->start_pa;
        buffer = list_entry(alloc_list->list_head.prev, struct cma_buffer, list);
        alloc_list->max_end = buffer->start_pa + buffer->length;
    }
}

//before call this, get lock firstly
static struct cma_buffer * find_cma_buffer(struct ipa_cma_para * para, unsigned long start_pa,
                                            unsigned long length)
{
    struct cma_alloc_list * alloc_list = &para->cma_buf_list;
    struct cma_buffer * buffer = NULL, *find = NULL;

    if(list_empty(&alloc_list->list_head))
        goto FIND_CMA_BUFF_DONE;

    list_for_each_entry(buffer, &alloc_list->list_head, list)
    {
        if((start_pa >= buffer->start_pa) && ((start_pa + length) <= (buffer->start_pa + buffer->length)))
        {
            find = buffer;
            break;
        }
    }

FIND_CMA_BUFF_DONE:
    return find;
}

static int CMA_Pool_Free(unsigned long offset_in_heap, unsigned long length,
                            unsigned long align, struct ipa_cma_para * para)
{
    int ret = 0;
    struct cma_buffer * release_buf_front = NULL, * release_buf_back = NULL, * find = NULL;
    struct cma_alloc_list * alloc_list = &para->cma_buf_list;
    unsigned long start_pa = 0;

    IPA_CMA_BUG_ON(!IS_ALIGNED(offset_in_heap, MIU_PROT_ALIGN_SIZE));
    IPA_CMA_BUG_ON(!IS_ALIGNED(length, MIU_PROT_ALIGN_SIZE));

    start_pa = offset_in_heap + PFN_PHYS(para->dev->cma_area->base_pfn);  // the start_allocation_addr (cpu bus address)

    mutex_lock(&para->ipa_cma_mutex);
    find = find_cma_buffer(para, start_pa, length);
    if(!find)
    {
        printk(CMA_ERR "\033[35mFunction = %s, Line = %d, [Error] [%s] Strange ipa cma free\033[m\n", __PRETTY_FUNCTION__, __LINE__, current->comm);
        printk(CMA_WARNING "\033[35mFunction = %s, Line = %d, want to release from ipa_id %d, 0x%lX to 0x%lX\033[m\n",
                __PRETTY_FUNCTION__, __LINE__, para->ipa_id, start_pa, (start_pa+length));
        printk(CMA_ERR "\033[35mFunction = %s, Line = %d, Check this\033[m\n", __PRETTY_FUNCTION__, __LINE__);
    }
    IPA_CMA_BUG_ON(!find);

    if(find->freed)
    {
        IPA_CMA_WARNING("buffer already freed [offset %llX length %llX] ipa_id %u\n", (u64)offset_in_heap, (u64)length, para->ipa_id);
        ret = Error_Unknown;
        goto CMA_POOL_FREE_DONE;
    }
    IPA_CMA_BUG_ON(alloc_list->using_count == 0);

    SplitBuffer(start_pa, length, find, alloc_list, CMA_FREE);
    //be careful, update kernel protect, then free memory to kernel
    free_buffer_list(alloc_list, &release_buf_front, &release_buf_back);
    if(release_buf_front)
    {
        ret = addKRange(para->miu_pro_sta, release_buf_front);
        dma_release_from_contiguous(para->dev, release_buf_front->page, release_buf_front->length >> PAGE_SHIFT);
        kfree(release_buf_front);
    }

    if(release_buf_back)
    {
        ret = addKRange(para->miu_pro_sta, release_buf_back);
        dma_release_from_contiguous(para->dev, release_buf_back->page, release_buf_back->length >> PAGE_SHIFT);
        kfree(release_buf_back);
    }
    merge_cma_buffer(alloc_list);

    dump_CMA_Pool(para, true);
CMA_POOL_FREE_DONE:
    mutex_unlock(&para->ipa_cma_mutex);
    dumpKRange(para->miu_pro_sta);

    return ret;
}

//if prev krange conjoint with the input krange, merge as one krange
static int _insertKRange(int miu_index, unsigned long lx_addr, unsigned long lx_length)
{
    MIU_ProtectRange *krange = NULL;

    if(!list_empty(&glob_miu_kranges[miu_index].list_head))
    {
        krange= list_entry(glob_miu_kranges[miu_index].list_head.prev, MIU_ProtectRange, list_node);
        if((krange->start_pa+krange->length) == lx_addr)
        {
            _miu_kernel_protect(krange->miuBlockIndex, kernal_protect_client_id,
                krange->start_pa, krange->start_pa+krange->length, MIU_PROTECT_DISABLE);

            krange->length += lx_length;
            _miu_kernel_protect(krange->miuBlockIndex, kernal_protect_client_id,
                krange->start_pa, krange->start_pa+krange->length, MIU_PROTECT_ENABLE);
            return 0;
        }
    }
    return -1;
}

static void _protect_lx_memory(unsigned long lx_size, unsigned long lx_start)
{
    int miu_index = 0;
    unsigned long offset = 0;
    MIU_ProtectRange *krange = NULL;

    if(lx_size != INVALID_PHY_ADDR)
    {
        phy_to_MiuOffset(lx_start, &miu_index, &offset);

        if(_insertKRange(miu_index, lx_start, lx_size))
        {
            krange = (MIU_ProtectRange *)kzalloc(sizeof(MIU_ProtectRange), GFP_KERNEL);
            IPA_CMA_BUG_ON(!krange);
            INIT_LIST_HEAD(&krange->list_node);
            krange->start_pa = lx_start;
            krange->length = lx_size;
            krange->miuBlockIndex = glob_miu_kranges[miu_index].krange_num;	// use miu_index's kernel protect
            // kernel protect block index start with 0

            _miu_kernel_protect(krange->miuBlockIndex, kernal_protect_client_id,
                krange->start_pa, krange->start_pa+krange->length, MIU_PROTECT_ENABLE);
            glob_miu_kranges[miu_index].miuBlockStatus[krange->miuBlockIndex] = MIU_BLOCK_BUSY;

            glob_miu_kranges[miu_index].krange_num++;   // next miu_index's kernel protect id
            list_add_tail(&krange->list_node, &glob_miu_kranges[miu_index].list_head);
            dumpKRange(&glob_miu_kranges[miu_index]);
        }
    }
}

static void init_glob_miu_kranges(void)
{
    int i = 0;

    for(i = 0; i < MIU_NUM; ++i)
    {
        glob_miu_kranges[i].miu = i;
        memset(glob_miu_kranges[i].miuBlockStatus, 0, sizeof(unsigned char)*MIU_BLOCK_NUM);
        glob_miu_kranges[i].krange_num = 0;
        mutex_init(&glob_miu_kranges[i].lock);
        INIT_LIST_HEAD(&glob_miu_kranges[i].list_head);
    }

    _protect_lx_memory(lx_mem_size, PHYS_OFFSET);
    _protect_lx_memory(lx_mem2_size, lx_mem2_addr);
    _protect_lx_memory(lx_mem3_size, lx_mem3_addr);
}

static ssize_t kprotect_enable(struct file *file, const char __user *user_buf, size_t size, loff_t *ppos)
{
    int len = 2;
    char buf[2];

    if(size > 2)
        len = 2;

    if(__copy_from_user(buf, user_buf, len))
    {
        return -EFAULT;
    }

    if(buf[0]=='0')
    {
        atomic_set(&kprotect_enabled, 0);
    }
    else if(buf[0]=='1')
    {
        atomic_set(&kprotect_enabled, 1);
    }

    if(atomic_read(&kprotect_enabled) > 0)
        printk(CMA_ERR "kernel protect enabled\n");
    else
       printk(CMA_ERR "kernel protect disabled\n");

    return size;
}

static int kprotect_status(struct seq_file *m, void *v)
{
    int kprotect_value;
    char buf[2];

    kprotect_value = atomic_read(&kprotect_enabled);
    if(kprotect_value == 0)
        buf[0] = '0';
    else
        buf[0] = '1';

    //seq_write(m, (const void *)buf, 1);
    if(kprotect_value > 0)
        printk(CMA_ERR "kernel protect enabled\n");
    else
        printk(CMA_ERR "kernel protect disabled\n");

    return 0;
}

static int miu_kernel_protect_open(struct inode *inode, struct file *file)
{
    return single_open(file, kprotect_status, inode->i_private);
}

static const struct file_operations kprotect_fops = {
    .owner  = THIS_MODULE,
    .open   = miu_kernel_protect_open,
    .read   = seq_read,
    .write  = kprotect_enable,
    .llseek = seq_lseek,
    .release = single_release,
};

//-------------------------------------------------------------------------------------------------
//IPA heap operations
//-------------------------------------------------------------------------------------------------
int ipa_cma_allocate(unsigned long heap_off, unsigned long len, void *para)
{
    struct page * page = NULL;

    if(len == 0)
        return 0;

    if(!IS_ALIGNED(heap_off, MIU_PROT_ALIGN_SIZE) || !IS_ALIGNED(len, MIU_PROT_ALIGN_SIZE))
    {
       IPA_CMA_ERR("input parameter not align to 8K, offset_in_heap %lx, length %lx\n",
                    heap_off, len);
       return Error_Invalid_Para;
    }

#ifdef IPA_CMA_DEBUG
    printk(KERN_ERR "\n%s ipa_id %d heap_off %lx len %lx\n", __FUNCTION__,
           ((struct ipa_cma_para *)para)->ipa_id, heap_off, len);
#endif

    //allocate memory
    page = CMA_Pool_Alloc(heap_off, len, MIU_PROT_ALIGN_SIZE, para);
    if(!page)
    {
        IPA_CMA_ERR("ipa cma alloc off %lx len %lx fail\n", heap_off, len);
        return Error_Unknown;
    }

    return 0;
}

int ipa_cma_free(unsigned long heap_off, unsigned long len, void *para)
{
    int ret = 0;

    if(!IS_ALIGNED(heap_off, MIU_PROT_ALIGN_SIZE) || !IS_ALIGNED(len, MIU_PROT_ALIGN_SIZE))
    {
       IPA_CMA_ERR("input parameter not align to 8K, offset_in_heap %lx, length %lx\n",
                    heap_off, len);
       return Error_Invalid_Para;
    }

#ifdef IPA_CMA_DEBUG
    printk(KERN_ERR "%s ipa_id %d heap_off %lx len %lx\n", __FUNCTION__,
            ((struct ipa_cma_para *)para)->ipa_id, heap_off, len);
#endif

    ret = CMA_Pool_Free(heap_off, len, MIU_PROT_ALIGN_SIZE, para);
    if(ret != RET_OK)
    {
       IPA_CMA_ERR("ipa cma free error heap_off %ld len %ld\n", heap_off, len);
       return Error_Unknown;
    }

    return 0;
}

int ipa_cma_get_ipa(unsigned long heap_off, IPA_ADDR *addr, void *cma_para)
{
    unsigned long length = 0;
    struct ipa_cma_para * para = (struct ipa_cma_para *)cma_para;

    IPA_CMA_BUG_ON(!IS_ALIGNED(heap_off, PAGE_SIZE));

    *addr = 0;
    length = para->dev->cma_area->count * PAGE_SIZE;
    if(heap_off > length)
    {
        IPA_CMA_ERR("invalid parameter: ipa id %d heap off %ld, heap length %ld\n",
                            para->ipa_id, heap_off, length);
        return Error_Invalid_Para;
    }

    *addr = heap_off + PFN_PHYS(para->dev->cma_area->base_pfn);
    return 0;
}

int ipa_cma_get_attr(struct IPA_heap_attr *info, void *cma_para)
{
    struct ipa_cma_para * para = (struct ipa_cma_para *)cma_para;
    unsigned long start = 0, offset = 0;
    unsigned int miu = 0;

    if(!info) return -EINVAL;
    info->miu = para->miu;
    info->name = para->name;
    start = PFN_PHYS(para->dev->cma_area->base_pfn);
    phy_to_MiuOffset(start, &miu, &offset);
    info->miu_offset = offset;

    info->heap_len = para->dev->cma_area->count * PAGE_SIZE;
    info->type = IPASPCMAN_HEAP_TYPE_CMA;

    return 0;
}

static struct IPA_heap_op ipa_cma_ops = {
    .heap_allocate = ipa_cma_allocate,
    .heap_free = ipa_cma_free,
    .heap_get_ipa = ipa_cma_get_ipa,
    .heap_get_attr = ipa_cma_get_attr,
};

static int ipa_cma_suspend(void)
{
    int miu_index= 0;
    MIU_ProtectRange *krange = NULL;
    for (miu_index = 0; miu_index < MIU_NUM; miu_index++)
    {
        list_for_each_entry(krange, &glob_miu_kranges[miu_index].list_head, list_node)
        {
            printk(CMA_DEBUG "--cmapool suspend--[miu: %d][block: %d][start: %lx][length: %lx]---\n",
                    miu_index, krange->miuBlockIndex, krange->start_pa, krange->length);
        }
    }
    MDrv_MIU_Save();
    return 0;
}

static int ipa_cma_resume(void)
{
    int miu_index= 0;
    MIU_ProtectRange *krange = NULL;
    for (miu_index = 0; miu_index < MIU_NUM; miu_index++)
    {
        list_for_each_entry(krange, &glob_miu_kranges[miu_index].list_head, list_node)
        {
            printk(CMA_DEBUG "--cmapool resume--[miu: %d][block: %d][start: %lx][length: %lx]---\n",
                    miu_index, krange->miuBlockIndex, krange->start_pa, krange->length);
            _miu_kernel_protect(krange->miuBlockIndex, kernal_protect_client_id,
                    krange->start_pa, krange->start_pa + krange->length, MIU_PROTECT_ENABLE);
        }
    }
    MDrv_MIU_Restore();
    return 0;
}

static struct Heap_pm_op ipa_cma_pm_ops = {
    .heap_suspend = ipa_cma_suspend,
    .heap_resume = ipa_cma_resume,
};

static int init_regis_heap(int index, struct ipa_cma_para * para)
{
    int ret = 0;
    struct cma_alloc_list * alloc_list = &para->cma_buf_list;

    para->name = ipa_cma_config[index].name;
    para->miu = ipa_cma_config[index].miu;
    para->dev = &ipa_cma_device[index];
    para->miu_pro_sta = &glob_miu_kranges[para->miu];
    para->ipa_id = ipa_cma_config[index].ipa_id;
    mutex_init(&para->ipa_cma_mutex);
    IPA_CMA_BUG_ON(!IS_ALIGNED(ipa_cma_config[index].size, MIU_PROT_ALIGN_SIZE));

    alloc_list->freed_count = 0;
    alloc_list->using_count = 0;
    alloc_list->min_start = PHYSICAL_START_INIT;
    alloc_list->max_end = PHYSICAL_END_INIT;
    INIT_LIST_HEAD(&alloc_list->list_head);

    ret = IPASpcMan_register_heap(para->ipa_id,
                                    ipa_cma_config[index].size,
                                    MIU_PROT_ALIGN_SIZE-1,
                                    &ipa_cma_ops,
                                    (void *)para);
    IPA_CMA_BUG_ON(ret);

    ret = IPASpcMan_register_heap_pm(&ipa_cma_pm_ops, IPASPCMAN_HEAP_TYPE_CMA);
    IPA_CMA_BUG_ON(ret);
    return ret;
}

// =========================================================
/*
  * compatible to prefix of CMA0 ~ CMA5, add those heap to ipa cma
  * for example:
  *
  * CMA0=OTHERS,miu=0,hid=23,sz=0x4800000,st=0x6000000
  * ......
  */
#ifdef CONFIG_MP_CMA_PATCH_CMA_MSTAR_DRIVER_BUFFER
extern struct CMA_BootArgs_Config cma_config[MAX_CMA_AREAS];
extern struct device mstar_cma_device[MAX_CMA_AREAS];
extern int mstar_driver_boot_cma_buffer_num;

static int compa_init_regis_heap(int index, struct ipa_cma_para * para)
{
    int ret = 0;
    struct cma_alloc_list * alloc_list = &para->cma_buf_list;

    //skig mali CMA heap 16 17 18
    if((cma_config[index].pool_id == ION_MALI_MIU0_HEAP_ID)
       || (cma_config[index].pool_id == ION_MALI_MIU1_HEAP_ID)
       || (cma_config[index].pool_id == ION_MALI_MIU2_HEAP_ID))
       return -1;

    para->name = cma_config[index].name;
    para->miu = cma_config[index].miu;
    para->dev = &mstar_cma_device[index];
    para->miu_pro_sta = &glob_miu_kranges[para->miu];
    para->ipa_id = cma_config[index].pool_id;
    mutex_init(&para->ipa_cma_mutex);

    alloc_list->freed_count = 0;
    alloc_list->using_count = 0;
    alloc_list->min_start = PHYSICAL_START_INIT;
    alloc_list->max_end = PHYSICAL_END_INIT;
    INIT_LIST_HEAD(&alloc_list->list_head);

    ret = IPASpcMan_register_heap(para->ipa_id,
                                    cma_config[index].size,
                                    MIU_PROT_ALIGN_SIZE-1,
                                    &ipa_cma_ops,
                                    (void *)para);
    IPA_CMA_BUG_ON(ret);

    ret = IPASpcMan_register_heap_pm(&ipa_cma_pm_ops, IPASPCMAN_HEAP_TYPE_CMA);
    IPA_CMA_BUG_ON(ret);
    return ret;
}
#else
static int mstar_driver_boot_cma_buffer_num = 0;
#endif
// =========================================================


MSYSTEM_STATIC int __init ipa_cma_init(void)
{
#ifdef IPA_CMA_TEST
    unsigned long heap_off,len;
    struct sg_table table1, table2, table3;
    void * test_para = NULL;
    int test_ret = 0;
#endif
    int ret = 0, index = 0, config_count = 0, i = 0;
    struct dentry * debug_root, * read_file = NULL;

    if(atomic_read(&kprotect_enabled) > 0)
        MDrv_MIU_Init();

    kernal_protect_client_id = MDrv_MIU_GetDefaultClientID_KernelProtect();
    init_glob_miu_kranges();    // to set kernel_protect for each lxmem

    debug_root = debugfs_create_dir("ipa_cma_pool", NULL);
    if (!debug_root)
    {
        printk(CMA_ERR "ipa cma: failed to create debugfs root directory.\n");
    }
    else
    {
        read_file = debugfs_create_file("kprotect_state", 0664, debug_root,
                    NULL, &kprotect_fops);
        if (!read_file)
            printk(CMA_ERR "ipa cma: failed to create debugfs file kprotect_state\n");
    }

    //mstar_driver_boot_cma_buffer_num may be include mali cma 16 17 18
    config_count = mstar_driver_boot_ipa_cma_num + mstar_driver_boot_cma_buffer_num;
    para = kmalloc(sizeof(struct ipa_cma_para) * config_count, GFP_KERNEL);
    IPA_CMA_BUG_ON(!para);

    for(index = 0; index < mstar_driver_boot_ipa_cma_num; ++index)
    {
        ret = init_regis_heap(index, &para[index]);
        IPA_CMA_BUG_ON(ret);
    }

#ifdef CONFIG_MP_CMA_PATCH_CMA_MSTAR_DRIVER_BUFFER
    i = 0;
    config_count = mstar_driver_boot_cma_buffer_num;
    while(config_count > 0)
    {
        ret = compa_init_regis_heap(i, &para[index]);
        if(ret == 0)
            index++;

        i++;
        config_count--;
    }
#endif

#ifdef IPA_CMA_TEST
    //config parameter add: IPA_CMA=cma_heap0,miu=1,sz=0x4000000,heap_id=0x280
    #define size_1M (1<<20)
    test_para = (void *)para;
    printk(KERN_ERR "\n======ipa cma test (heap start %llx)================================\n",
                    __pfn_to_phys(para->dev->cma_area->base_pfn));

    /* test pattern
        *  CMA heap 64M
        * |---------------------------------------------------------------------|
        *      |----|                    |-------------|                   |--------|
        *         1M                                   4M                                   2M
        */
    //alloc
    heap_off = 24*size_1M;
    len = 4*size_1M;
    test_ret = ipa_cma_allocate(heap_off, len, &table1, test_para);
    printk(KERN_ERR "alloc heap_off %lx len %lx ret %d\n", heap_off, len, test_ret);

    heap_off = 1*size_1M;
    len = 1*size_1M;
    test_ret = ipa_cma_allocate(heap_off, len, &table2, test_para);
    printk(KERN_ERR "alloc heap_off %lx len %lx ret %d\n", heap_off, len, test_ret);

    heap_off = 32*size_1M;
    len = 2*size_1M;
    test_ret = ipa_cma_allocate(heap_off, len, &table3, test_para);
    printk(KERN_ERR "alloc heap_off %lx len %lx ret %d\n", heap_off, len, test_ret);

    //free
    heap_off = 1*size_1M;
    len = 1*size_1M;
    test_ret = ipa_cma_free(heap_off, len, &table1, test_para);
    sg_free_table(&table1);
    printk(KERN_ERR "free heap_off %lx len %lx ret %d\n", heap_off, len, test_ret);

    heap_off = 32*size_1M;
    len = 2*size_1M;
    test_ret = ipa_cma_free(heap_off, len, &table2, test_para);
    sg_free_table(&table2);
    printk(KERN_ERR "free heap_off %lx len %lx ret %d\n", heap_off, len, test_ret);

    heap_off = 24*size_1M;
    len = 4*size_1M;
    test_ret = ipa_cma_free(heap_off, len, &table3, test_para);
    sg_free_table(&table3);
    printk(KERN_ERR "free heap_off %lx len %lx ret %d\n", heap_off, len, test_ret);
#endif

    return 0;
}


MSYSTEM_STATIC void __exit ipa_cma_exit(void)
{

}

late_initcall(ipa_cma_init);
module_exit(ipa_cma_exit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("CMAPOOL driver");
MODULE_LICENSE("GPL");
