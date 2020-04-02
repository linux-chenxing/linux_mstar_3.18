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
/// @file   pa_space.c
/// @brief  manage physical address space not in linux kernel memory
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include files
//-------------------------------------------------------------------------------------------------
#include <linux/module.h>
#include <linux/list.h>
#include <linux/rbtree.h>
#include <linux/err.h>
#include <linux/scatterlist.h>
#include <linux/highmem.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/vmalloc.h>
#include <linux/sched.h> 
#include <linux/fdtable.h>
#include <linux/pid.h>
#include <linux/file.h>
#include <asm/uaccess.h>

#include <ipa_priv.h>
#include <chip_setup.h>
#include <ipa_bootargs.h>
#include <ipa_pool.h>
#include "pa_space.h"

//-------------------------------------------------------------------------------------------------
// Macros and struct
//-------------------------------------------------------------------------------------------------
struct pa_space_para {
    int miu;
    char * name;
    int ipa_id;
    struct pa_using_addr_space using_addr_space;
    unsigned long start;
    unsigned long length;    
    struct mutex pa_mutex;
};

//#define PA_SPACE_TEST       1
//#define PA_DEBUG            1
#define ALIGN_TO_PAGESIZE     PAGE_SIZE
static struct pa_space_para * para = NULL;
//-------------------------------------------------------------------------------------------------
//internal fun
//-------------------------------------------------------------------------------------------------
/*   find [heap_off, heap_off+len) in specified space list
  *   node retured is partly coverd or cover all
  */
static struct pa_using_addr_space_node * _find_in_pa_space(unsigned long heap_off, unsigned long len, 
    struct pa_using_addr_space *using_addr_space, unsigned long align)
{
    struct pa_using_addr_space_node * node;

    PA_SPACE_BUG_ON(using_addr_space == NULL);
    if(list_empty(&using_addr_space->list_head) || len <= 0)
        return NULL;

    heap_off /= align;
    len /= align;

    list_for_each_entry(node, &using_addr_space->list_head, node)
    {
        if((heap_off >= node->start && heap_off < (node->start + node->size))
            || (heap_off+len > node->start && heap_off+len <= (node->start + node->size)))
            return node;        
    }

    return NULL;
}

/*  dump all node in pa address space
  *
  */
static void  _dump_pa_space( struct pa_using_addr_space *using_addr_space)
{
    struct pa_using_addr_space_node * node;

    PA_SPACE_BUG_ON(using_addr_space == NULL);
    if(list_empty(&using_addr_space->list_head))
        return;

    printk(KERN_INFO "dump pa using address space\n");       
    list_for_each_entry(node, &using_addr_space->list_head, node)
    {
        printk(KERN_INFO "heap_off %ld len %ld\n", node->start, node->size);      
    }
    printk(KERN_INFO "\n\n\n");
}

 /* insert new node into addr space
  *  the addr space list is sorted as descending order
  */
static int  _insert_pa_space( struct pa_using_addr_space *using_addr_space,
    struct pa_using_addr_space_node *new_node)
{
    struct pa_using_addr_space_node *front = NULL, *back = NULL, *node;
    
    PA_SPACE_BUG_ON(using_addr_space == NULL);
    PA_SPACE_BUG_ON(new_node == NULL);

    if(list_empty(&using_addr_space->list_head))
    {
        list_add(&new_node->node, &using_addr_space->list_head);
        using_addr_space->count++;
        return 0;
    }

    //find front node and back node
    //front->start > new_node->start > back->start
    list_for_each_entry(node, &using_addr_space->list_head, node)
    {
        if(node->start < new_node->start) 
            break;       
 
        front = node;    
    }    

    if(!front)
        back = list_first_entry(&using_addr_space->list_head, 
                struct pa_using_addr_space_node, node);
    else
    {
        if(list_is_last(&front->node, &using_addr_space->list_head))
            back = NULL;
        else
            back = list_entry(front->node.next, struct pa_using_addr_space_node, node);        
    }

    //merge with front & back node
    if(front && back 
        && ((back->start + back->size) == new_node->start)    //can merge with back
        && ((new_node->start + new_node->size) == front->start)) //can merge with front
    {        
        back->size += new_node->size + front->size;
        kfree(new_node);
        list_del(&front->node);
        kfree(front);
        using_addr_space->count--;
        
        return 0;
    }

    //merge with front
    if(front && ((new_node->start + new_node->size) == front->start))
    {
        front->start = new_node->start;
        front->size += new_node->size;
        kfree(new_node);
        return 0;        
    }
    
    //merge with back
    if(back && ((back->start + back->size) == new_node->start))
    {
        back->size += new_node->size;        
        kfree(new_node);
        return 0;        
    }

    //can't merge with front or back
    if(front)
        list_add(&new_node->node, &front->node);  //insert after front
    else
        list_add(&new_node->node, &using_addr_space->list_head); //insert at list head
    using_addr_space->count++; 

    return 0;
}

/* free specified range[heap_off, heap_off+len) node in list
  * 
  */
static int _free_pa_space(unsigned long heap_off, unsigned long len, 
    struct pa_using_addr_space *using_addr_space, unsigned long align)
{    
    struct pa_using_addr_space_node * node = NULL, *new_node = NULL;
    unsigned long start, size;

    PA_SPACE_BUG_ON(using_addr_space == NULL);
    if(list_empty(&using_addr_space->list_head) || len <= 0)
        return Error_Invalid_Para;

    node = _find_in_pa_space(heap_off, len, using_addr_space, align);
    if(!node)
        return Error_Invalid_Para;

    start = heap_off/align;
    size = len/align;
    if(!((start >= node->start) && ((start+size) <= (node->start+node->size))))
        return Error_Invalid_Para;

    //[heap_off, off+len) cover a integrated node
    if((start == node->start) && ((start+size) == (node->start+node->size)))
    {
        list_del(&node->node);
        kfree(node);
        using_addr_space->count--;
        return 0;
    }

    //at the node start
    if(start == node->start)
    {
        node->start = start + size;
        node->size -= size;
    }
    else
    {
        //at the node end
        if((start + size) == (node->start + node->size))
        {             
            node->size -= size;
        }
        else
        {
            //in the middle of node
            new_node = kmalloc(sizeof(struct pa_using_addr_space_node), GFP_KERNEL);
            if(!new_node)
                return Error_No_Mem;
            new_node->start = start + size;
            new_node->size = node->start + node->size - (start + size);
            INIT_LIST_HEAD(&new_node->node);

            node->size = start - node->start;

            list_add(&new_node->node, &node->node); //add new node after node 
            using_addr_space->count++;
        }    
    }
        
    return 0;    
}


//-------------------------------------------------------------------------------------------------
//IPA heap operations
//-------------------------------------------------------------------------------------------------
int pa_space_allocate(unsigned long heap_off, unsigned long len, void *para)
{
    int ret = 0;  
    struct pa_space_para * pa_para = (struct pa_space_para *)para;
    struct pa_using_addr_space * using_addr_space = &pa_para->using_addr_space;
    struct pa_using_addr_space_node *space_node = NULL;

    if((len < ALIGN_TO_PAGESIZE)
        || !(IS_ALIGNED(heap_off, ALIGN_TO_PAGESIZE))
        || !(IS_ALIGNED(len, ALIGN_TO_PAGESIZE)))
    {
        PA_SPACE_ERR("invalid parameter heap off %ld, len %ld\n", heap_off, len);
        return Error_Invalid_Para;
    }

    if ((heap_off >= (pa_para->start + pa_para->length))
        || ((heap_off+len) > (pa_para->start + pa_para->length)))
    {
        PA_SPACE_ERR("invalid parameter heap off %ld, len %ld\n", heap_off, len);
        return Error_Invalid_Para;
    }

    #ifdef PA_DEBUG
    printk(KERN_ERR "\n%s ipa_id %d heap_off %lx len %lx\n", __FUNCTION__, 
            pa_para->ipa_id, heap_off, len);
    #endif

    mutex_lock(&pa_para->pa_mutex);
    //find in pa using addr space list
    space_node = _find_in_pa_space(heap_off, len, using_addr_space, ALIGN_TO_PAGESIZE);
    if(space_node)
    {
        _dump_pa_space(using_addr_space);
        PA_SPACE_BUG_ON(space_node);
        ret = Error_Node_Exist;
        goto ALLOC_ERROR;
    }

    space_node = kmalloc(sizeof(struct pa_using_addr_space_node), GFP_KERNEL);
    if(!space_node)
    {   
        ret = Error_No_Mem;
        goto ALLOC_ERROR;
    }

    //update pa using addr space list
    INIT_LIST_HEAD(&space_node->node);
    
    space_node->start = heap_off/ALIGN_TO_PAGESIZE;  //page size unit
    space_node->size = len/ALIGN_TO_PAGESIZE;
    _insert_pa_space(using_addr_space, space_node);
    mutex_unlock(&pa_para->pa_mutex);

    return  0;
    
ALLOC_ERROR:    
    mutex_unlock(&pa_para->pa_mutex);
    return ret;
}

int pa_space_free(unsigned long heap_off, unsigned long len, void *para)
{
    int ret = 0;
    
    struct pa_space_para * pa_para = (struct pa_space_para *)para;
    struct pa_using_addr_space * using_addr_space = &pa_para->using_addr_space;

    if((len < ALIGN_TO_PAGESIZE)
        || !(IS_ALIGNED(heap_off, ALIGN_TO_PAGESIZE))
        || !(IS_ALIGNED(len, ALIGN_TO_PAGESIZE)))
    {
        PA_SPACE_ERR("invalid parameter heap off %ld, len %ld\n", heap_off, len);
        return Error_Invalid_Para;
    }

    if ((heap_off >= (pa_para->start + pa_para->length))
        || ((heap_off+len) > (pa_para->start + pa_para->length)))
    {
        PA_SPACE_ERR("invalid parameter heap off %ld, len %ld\n", heap_off, len);
        return Error_Invalid_Para;
    }

    #ifdef PA_DEBUG
    printk(KERN_ERR "\n%s ipa_id %d heap_off %lx len %lx\n", __FUNCTION__, 
            pa_para->ipa_id, heap_off, len);
    #endif

    mutex_lock(&pa_para->pa_mutex);
    ret = _free_pa_space(heap_off, len, using_addr_space, ALIGN_TO_PAGESIZE);
    if(ret)
    {
        ret = Error_Invalid_Para;        
        goto FREE_ERROR;
    }

    ret = 0;
    
FREE_ERROR:
    mutex_unlock(&pa_para->pa_mutex);
    return ret;
}

int pa_space_get_ipa(unsigned long heap_off, IPA_ADDR *addr, void *para)
{
    struct pa_space_para * pa_para = (struct pa_space_para *)para;
    PA_SPACE_BUG_ON(!IS_ALIGNED(heap_off, ALIGN_TO_PAGESIZE));
    
    if(heap_off >= pa_para->length)        
    {
        *addr = 0;
        PA_SPACE_ERR("invalid parameter heap off %ld\n", heap_off);
        return Error_Invalid_Para;
    }

    *addr = heap_off + pa_para->start;
    return 0;
}

int pa_space_get_attr(struct IPA_heap_attr *info, void *para)
{
    struct pa_space_para * pa_para = (struct pa_space_para *)para;
    char miu = pa_para->miu;    
   
    PA_SPACE_BUG_ON(!info);
    info->name = pa_para->name;
    info->miu = miu;
    info->miu_offset = pa_para->start;
    info->heap_len = pa_para->length;
    info->type = IPASPCMAN_HEAP_TYPE_PA;
    
    return 0;
}

static struct IPA_heap_op pa_space_ops = {
    .heap_allocate = pa_space_allocate,
    .heap_free = pa_space_free,
    .heap_get_ipa = pa_space_get_ipa,
    .heap_get_attr = pa_space_get_attr,
};

static int init_pa_heap(struct pa_space_para * pa_para, int index)
{
    int ret = 0;

    pa_para->ipa_id = pa_config[index].ipa_id;
    pa_para->miu = pa_config[index].miu;        
    pa_para->start = pa_config[index].start;
    pa_para->length = pa_config[index].size;
    pa_para->name = pa_config[index].name;
    PA_SPACE_BUG_ON(!IS_ALIGNED(pa_para->length, ALIGN_TO_PAGESIZE));

    INIT_LIST_HEAD(&pa_para->using_addr_space.list_head);
    pa_para->using_addr_space.count = 0;
    mutex_init(&pa_para->pa_mutex);
    
    ret = IPASpcMan_register_heap(pa_para->ipa_id, 
                                    pa_config[index].size,
                                    ALIGN_TO_PAGESIZE-1,                                            
                                    &pa_space_ops,
                                    (void *)pa_para);
    PA_SPACE_BUG_ON(ret);
    
    return ret;
}

static int __init pa_space_init (void)
{
#ifdef PA_SPACE_TEST
    unsigned long heap_off,len;
    struct sg_table table1, table2, table3;
    void * test_para = NULL;
    int test_ret = 0;
#endif
    int index = 0, ret = 0;
    
    PA_SPACE_BUG_ON(mstar_driver_boot_pa_num > MIU_NUM);
    para = kmalloc(sizeof(struct pa_space_para) * mstar_driver_boot_pa_num, GFP_KERNEL);
    PA_SPACE_BUG_ON(!para);

    for(index = 0; index < mstar_driver_boot_pa_num; ++index)
    {        
        ret = init_pa_heap(&para[index], index);        
        PA_SPACE_BUG_ON(ret);
    }

#ifdef PA_SPACE_TEST
    #define size_1M (1<<20)
    test_para = (void *)para;
    printk(KERN_ERR "\n pa space test ==================================\n");
    //alloc
    heap_off = 0; 
    len = 1*size_1M;
    test_ret = pa_space_allocate(heap_off, len, &table1, test_para);   
    printk(KERN_ERR "alloc heap_off %lx len %lx ret %d\n", heap_off, len, test_ret);
    _dump_pa_space(&para->using_addr_space);

    heap_off = 1*size_1M; 
    len = 4*size_1M;
    test_ret = pa_space_allocate(heap_off, len, &table2, test_para);   
    printk(KERN_ERR "alloc heap_off %lx len %lx ret %d\n", heap_off, len, test_ret);
    _dump_pa_space(&para->using_addr_space);    

    heap_off = 12*size_1M; 
    len = 12*size_1M;
    test_ret = pa_space_allocate(heap_off, len, &table3, test_para); 
    printk(KERN_ERR "alloc heap_off %lx len %lx ret %d\n", heap_off, len, test_ret);
    _dump_pa_space(&para->using_addr_space);

    //free
    heap_off = 0*size_1M;
    len = 1*size_1M;
    test_ret = pa_space_free(heap_off, len, &table1, test_para);
    sg_free_table(&table1);
    printk(KERN_ERR "free heap_off %lx len %lx ret %d\n", heap_off, len, test_ret);
    _dump_pa_space(&para->using_addr_space);

    heap_off = 1*size_1M;
    len = 4*size_1M;
    test_ret = pa_space_free(heap_off, len, &table2, test_para);
    sg_free_table(&table2);
    printk(KERN_ERR "free heap_off %lx len %lx ret %d\n", heap_off, len, test_ret);
    _dump_pa_space(&para->using_addr_space);    

    heap_off = 12*size_1M;
    len = 12*size_1M;
    test_ret = pa_space_free(heap_off, len, &table3, test_para);
    sg_free_table(&table3);
    printk(KERN_ERR "free heap_off %lx len %lx ret %d\n", heap_off, len, test_ret);
    _dump_pa_space(&para->using_addr_space);    
#endif

    
    return 0;
}

static void __exit pa_space_exit(void)
{
    int index = 0;
    for(index = 0; index < mstar_driver_boot_pa_num; ++index)
    {
        IPASpcMan_unregister_heap(pa_config[index].ipa_id);
    }
}


late_initcall(pa_space_init);
module_exit(pa_space_exit);
