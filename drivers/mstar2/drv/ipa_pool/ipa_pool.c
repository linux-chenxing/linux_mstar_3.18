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
/// @file   ipa_pool.c
/// @brief  manager ipa space
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include files
//-------------------------------------------------------------------------------------------------
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/gfp.h>
#include <linux/debugfs.h>
#include <linux/interval_tree_generic.h>
#include <mdrv_ipa_pool.h>
#include "ipa_pool.h"
#include "ipa_priv.h"

//-------------------------------------------------------------------------------------------------
// Macros and struct
//-------------------------------------------------------------------------------------------------
struct ipa_pool_allocation
{
    struct rb_node itrb_node;       //node in interval tree
    unsigned long offset_in_heap;   //offset in heap space, unit:BYTE
    unsigned long length;           //offset in heap space, unit:BYTE
    unsigned long rb_subtree_last;  //interval tree need this
    struct IPA_pool *pool;          // pool this allocation belongs to
    struct file *client_filp;       //pool client filp
};

struct ipa_hal_allocation
{
    struct rb_node itrb_node;       //node in interval tree
    unsigned long offset_in_heap;   //offset in heap space, unit:ipa heap (alignmask +1)
    unsigned long length;           //offset in heap space, unit:ipa heap (alignmask +1)
    unsigned long rb_subtree_last; 
};

struct IPA_space_heap_man
{
    struct mutex lock;
    struct list_head heaps;
    struct dentry * dbg_root;
};
static struct IPA_space_heap_man IPASpcHeapMan;

struct IPA_space_heap_pm_man
{
    struct mutex lock;
    struct list_head heaps_pm;
};
static struct IPA_space_heap_pm_man IPASpcHeapPMMan;

#define heap_pool_allocation_interval_tree_foreach(alloc, root, start, last)    \
    for (alloc = ipa_pool_allocation_tree_iter_first(root, start, last); \
         alloc; alloc = ipa_pool_allocation_tree_iter_next(alloc, start, last))

#define heap_pool_allocation_interval_tree_foreach_safe(alloc, n, root, start, last) \
        for (alloc = ipa_pool_allocation_tree_iter_first(root, start, last), \
            alloc? (n = ipa_pool_allocation_tree_iter_next(alloc, start, last)):(n = NULL);  \
            alloc;  \
            alloc = n, alloc? (n = ipa_pool_allocation_tree_iter_next(alloc, start, last)):(n = NULL))


#define heap_hal_allocation_interval_tree_foreach(alloc, root, start, last) \
    for (alloc = ipa_hal_allocation_tree_iter_first(root, start, last); \
        alloc; alloc = ipa_hal_allocation_tree_iter_next(alloc, start, last))

#define heap_hal_allocation_interval_tree_foreach_safe(alloc, n, root, start, last) \
        for (alloc = ipa_hal_allocation_tree_iter_first(root, start, last), \
            alloc? (n = ipa_hal_allocation_tree_iter_next(alloc, start, last)):(n = NULL);  \
            alloc;  \
            alloc = n, alloc? (n = ipa_hal_allocation_tree_iter_next(alloc, start, last)):(n = NULL))

#define IPA_ALLOCATION_BUG(fmt, args...) \
do { \
        printk(KERN_ERR "ipa pool error: should not been here: %s %d\n", __FUNCTION__,__LINE__);  \
        printk(KERN_ERR fmt,## args); \
        BUG(); \
   } while(0) 


//#define IPA_FREE 1
//#define IPA_ALLOC 1
#ifdef IPA_FREE
#define IPA_HEAP_FREE_DEBUG(fmt, args...)   \
    printk(KERN_ERR "%s %d: " fmt, __FUNCTION__, __LINE__, ## args);
#else
#define IPA_HEAP_FREE_DEBUG(fmt, args...)
#endif

#ifdef IPA_ALLOC
#define IPA_HEAP_ALLOC_DEBUG(fmt, args...)  \
    printk(KERN_ERR "%s %d: " fmt, __FUNCTION__, __LINE__, ## args);
#else
#define IPA_HEAP_ALLOC_DEBUG(fmt, args...)
#endif

#define IPA_HEAP_INVALID_PARA(fmt, args...)  \
printk(KERN_ERR "IPA_HEAP %s %d: parameter error " fmt, __FUNCTION__, __LINE__, ## args);
#define IPA_HEAP_ERROR(fmt, args...)  \
printk(KERN_ERR "IPA_HEAP error %s %d " fmt, __FUNCTION__, __LINE__, ## args);

//-------------------------------------------------------------------------------------------------
// internal fun
//-------------------------------------------------------------------------------------------------
static inline unsigned long ipa_pool_allocation_start(struct ipa_pool_allocation *v)
{
	return v->offset_in_heap;
}

static inline unsigned long ipa_pool_allocation_last(struct ipa_pool_allocation *v)
{
	return v->offset_in_heap + v->length - 1;
}

INTERVAL_TREE_DEFINE(struct ipa_pool_allocation, itrb_node,
		     unsigned long, rb_subtree_last,
		     ipa_pool_allocation_start, ipa_pool_allocation_last, static, ipa_pool_allocation_tree);


static inline unsigned long ipa_hal_allocation_start(struct ipa_hal_allocation *v)
{
	return v->offset_in_heap;
}

static inline unsigned long ipa_hal_allocation_last(struct ipa_hal_allocation *v)
{
	return v->offset_in_heap + v->length - 1;
}

INTERVAL_TREE_DEFINE(struct ipa_hal_allocation, itrb_node,
		     unsigned long, rb_subtree_last,
		     ipa_hal_allocation_start, ipa_hal_allocation_last, static, ipa_hal_allocation_tree);



static int heap_hal_tree_add_and_merge_range(struct IPA_heap *heap,
	                                                              unsigned long start_in_hal_unit, 
	                                                              unsigned long last_in_hal_unit, 
	                                                              int unit_shfit)
{
   struct ipa_hal_allocation *pre_allocation = NULL, *post_allocation = NULL, *n;
   unsigned long heap_end_in_hal_unit = (heap->len>>unit_shfit);

   if((start_in_hal_unit > last_in_hal_unit)
      || (last_in_hal_unit+1 > heap_end_in_hal_unit))
   {
        IPA_HEAP_INVALID_PARA("start_in_hal_unit %lu last_in_hal_unit %lu heap_end_in_hal_unit %lu\n",
            start_in_hal_unit, last_in_hal_unit, heap_end_in_hal_unit);
        return -EINVAL;   
   }
  
   if(start_in_hal_unit)
   {
       //check for possible just before pre-allocation     
       heap_hal_allocation_interval_tree_foreach_safe(pre_allocation, n, &heap->hal_allocation_tree_root, start_in_hal_unit-1, start_in_hal_unit-1)
       {
          BUG_ON(pre_allocation->offset_in_heap+pre_allocation->length != start_in_hal_unit);
       
	      ipa_hal_allocation_tree_remove(pre_allocation, &heap->hal_allocation_tree_root);
		  pre_allocation->length += last_in_hal_unit-start_in_hal_unit+1;
		  ipa_hal_allocation_tree_insert(pre_allocation, &heap->hal_allocation_tree_root);
		  break;
       }
   }
   
   if(last_in_hal_unit < (heap_end_in_hal_unit-1))
   {
       //check for possible just after post-allocation  
       heap_hal_allocation_interval_tree_foreach_safe(post_allocation, n, &heap->hal_allocation_tree_root, last_in_hal_unit+1, last_in_hal_unit+1)
       {
		  //since hal tree no overlay, bug check here
          BUG_ON(post_allocation->offset_in_heap != last_in_hal_unit+1);
       
	   	  ipa_hal_allocation_tree_remove(post_allocation, &heap->hal_allocation_tree_root);
	      if(pre_allocation)
	      {
	         BUG_ON(pre_allocation->offset_in_heap+pre_allocation->length != post_allocation->offset_in_heap);
             
			 ipa_hal_allocation_tree_remove(pre_allocation, &heap->hal_allocation_tree_root);			         
			 pre_allocation->length += post_allocation->length;
			 ipa_hal_allocation_tree_insert(pre_allocation, &heap->hal_allocation_tree_root);
             kfree(post_allocation);
	      }
		  else
		  {
			 post_allocation->offset_in_heap = start_in_hal_unit;
		     post_allocation->length += last_in_hal_unit-start_in_hal_unit+1;
			 ipa_hal_allocation_tree_insert(post_allocation, &heap->hal_allocation_tree_root);
		  }
		  break;
       }
   }
   
   if(!pre_allocation && !post_allocation)
   {
      pre_allocation = (struct ipa_hal_allocation*)kmalloc(sizeof(struct ipa_hal_allocation), GFP_KERNEL);
	  if(!pre_allocation)
	  	return -ENOMEM;
      
	  pre_allocation->offset_in_heap = start_in_hal_unit;
	  pre_allocation->length = last_in_hal_unit-start_in_hal_unit+1;
	  ipa_hal_allocation_tree_insert(pre_allocation, &heap->hal_allocation_tree_root);
   }

   return 0;
}

static int ipa_heap_hal_alloc_tree_alloc_range(struct IPA_heap *heap, unsigned long start, unsigned long last, 
                                                                                        int unit_shift)
{
    struct ipa_hal_allocation *hal_allocation;
    unsigned long start_in_hal_unit = (round_up(start, 1UL<<unit_shift)>>unit_shift);
    unsigned long last_in_hal_unit = (round_down(last+1, 1UL<<unit_shift)>>unit_shift);
    int ret = 0;

    if((last > heap->len-1) || (start > last))
    {
        IPA_HEAP_INVALID_PARA("start %lu last %lu (heap->len-1) %lu \n",
            start, last, heap->len-1);
        return -EINVAL;
    }

    if(last_in_hal_unit == 0)
    {
       //this case means -----last < (1UL<<unit_shift)-1
       //so nothing to alloc
       return 0;
    }
    last_in_hal_unit--;

    //check to see if other to free
    //  oooo|oxxo|oooo,     o means in used bytes, xx means empty space need to alloc
    if(start_in_hal_unit>last_in_hal_unit)
        return 0;

    IPA_HEAP_ALLOC_DEBUG("start_in_hal_unit %lu last_in_hal_unit %lu\n",
        start_in_hal_unit, last_in_hal_unit);        
    heap_hal_allocation_interval_tree_foreach(hal_allocation, &heap->hal_allocation_tree_root, start_in_hal_unit, last_in_hal_unit)
    {
        //conflict, already allocate, why been here
        BUG();
    }

    IPA_HEAP_ALLOC_DEBUG("alloc in_hal_unit start %lu length %lu\n", start_in_hal_unit<<unit_shift, 
        (last_in_hal_unit-start_in_hal_unit+1)<<unit_shift);  
    ret = heap->heap_op->heap_allocate(start_in_hal_unit<<unit_shift, (last_in_hal_unit-start_in_hal_unit+1)<<unit_shift, heap->priv);
    if(ret)
        return ret;

    ret = heap_hal_tree_add_and_merge_range(heap, start_in_hal_unit, last_in_hal_unit, unit_shift);
    if(ret)
    {
       heap->heap_op->heap_free(start_in_hal_unit<<unit_shift, (last_in_hal_unit-start_in_hal_unit+1)<<unit_shift, heap->priv);
    }

    return ret;
}

static int ipa_heap_hal_alloc_tree_free_range(struct IPA_heap *heap, unsigned long start, unsigned long last, 
    int unit_shift)
{
    struct ipa_hal_allocation *hal_allocation, *hal_allocation2, *n;
    unsigned long start_in_hal_unit = (round_up(start, 1UL<<unit_shift)>>unit_shift);
    unsigned long last_in_hal_unit = (round_down(last+1, 1UL<<unit_shift)>>unit_shift);
    int ret = 0;

    if(start > last)
    {
        IPA_HEAP_INVALID_PARA("start %lu last %lu\n", start, last);
        return -EINVAL;
    }   

    if(last_in_hal_unit == 0)
    {
      //this case means -----last < (1UL<<unit_shift)-1
      //so nothing to free
      return 0;
    }
    last_in_hal_unit--;

    //check to see if other to free
    //  oooo|oxxo|oooo,     o means in used bytes, xx means freed bytes
    if(start_in_hal_unit > last_in_hal_unit)
        return 0;

    IPA_HEAP_FREE_DEBUG("start_in_hal_unit %lu last_in_hal_unit %lu\n",
        start_in_hal_unit, last_in_hal_unit);
    heap_hal_allocation_interval_tree_foreach_safe(hal_allocation, n, &heap->hal_allocation_tree_root, start_in_hal_unit, last_in_hal_unit)
    {
      ipa_hal_allocation_tree_remove(hal_allocation, &heap->hal_allocation_tree_root);

      IPA_HEAP_FREE_DEBUG("hal_allocation->offset_in_heap %lu hal_allocation->length %lu\n",
          hal_allocation->offset_in_heap, hal_allocation->length);
      
      if(start_in_hal_unit<=hal_allocation->offset_in_heap 
            && last_in_hal_unit>=hal_allocation->offset_in_heap+hal_allocation->length-1)
      {
        /*contain	
               * start       last
               *  | ------ |
               */
         IPA_HEAP_FREE_DEBUG("free start 0x%lx len 0x%lx\n", hal_allocation->offset_in_heap<<unit_shift, 
         hal_allocation->length<<unit_shift); 
         ret = heap->heap_op->heap_free(hal_allocation->offset_in_heap<<unit_shift, hal_allocation->length<<unit_shift, heap->priv);
         BUG_ON(ret != 0);
         kfree(hal_allocation);
      }
      else if(start_in_hal_unit <= hal_allocation->offset_in_heap+hal_allocation->length-1 
        && last_in_hal_unit>=hal_allocation->offset_in_heap)
      {
          if(start_in_hal_unit>hal_allocation->offset_in_heap && last_in_hal_unit<hal_allocation->offset_in_heap+hal_allocation->length-1)
          { 
             /*intersect     
                        *   start           last
                        * ---|--------|---
                        */          
             hal_allocation2 = (struct ipa_hal_allocation*)kmalloc(sizeof(struct ipa_hal_allocation), GFP_KERNEL);
             if(!hal_allocation2)
             {
                 //must be first conject here
                 return -ENOMEM;
             }
         
             hal_allocation2->offset_in_heap = last_in_hal_unit+1;
             hal_allocation2->length = hal_allocation->offset_in_heap+hal_allocation->length-hal_allocation2->offset_in_heap;
             ipa_hal_allocation_tree_insert(hal_allocation2, &heap->hal_allocation_tree_root);
             

             hal_allocation->length = start_in_hal_unit - hal_allocation->offset_in_heap;
             ipa_hal_allocation_tree_insert(hal_allocation, &heap->hal_allocation_tree_root);				 

             IPA_HEAP_FREE_DEBUG("free start 0x%lx len 0x%lx\n", start_in_hal_unit<<unit_shift, 
                (last_in_hal_unit+1-start_in_hal_unit)<<unit_shift);
             ret = heap->heap_op->heap_free(start_in_hal_unit<<unit_shift, (last_in_hal_unit+1-start_in_hal_unit)<<unit_shift, heap->priv);
             BUG_ON(ret != 0);
             //since there is no conjection in hal allocation interval tree, return here
             return 0;
          }
          else if(start_in_hal_unit > hal_allocation->offset_in_heap)
          {
              /*       
                          *   start           last
                          * ---|-----     |
                          */
              unsigned long free_end_in_hal_unit = hal_allocation->offset_in_heap+hal_allocation->length;			  
              hal_allocation->length = start_in_hal_unit - hal_allocation->offset_in_heap;
              ipa_hal_allocation_tree_insert(hal_allocation, &heap->hal_allocation_tree_root);

              IPA_HEAP_FREE_DEBUG("free start 0x%lx len 0x%lx\n", start_in_hal_unit<<unit_shift, 
                (free_end_in_hal_unit-start_in_hal_unit)<<unit_shift);
              ret = heap->heap_op->heap_free(start_in_hal_unit<<unit_shift, (free_end_in_hal_unit-start_in_hal_unit)<<unit_shift, heap->priv);
              BUG_ON(ret != 0);
              start_in_hal_unit = free_end_in_hal_unit;              
          }
          else
          {
              /*     
                          *   start          last
                          *    |      ------|--
                          */
              if(hal_allocation->offset_in_heap+hal_allocation->length-1 > last_in_hal_unit)
              { 
                    unsigned long free_start_in_hal_unit = hal_allocation->offset_in_heap;
                    
                    hal_allocation->length = hal_allocation->offset_in_heap+hal_allocation->length-last_in_hal_unit-1;
                    hal_allocation->offset_in_heap = last_in_hal_unit+1;
                    ipa_hal_allocation_tree_insert(hal_allocation, &heap->hal_allocation_tree_root);

                    IPA_HEAP_FREE_DEBUG("free start 0x%lx len 0x%lx\n", free_start_in_hal_unit<<unit_shift, 
                        (last_in_hal_unit+1-free_start_in_hal_unit)<<unit_shift);
                    ret = heap->heap_op->heap_free(free_start_in_hal_unit<<unit_shift, 
                          (last_in_hal_unit+1-free_start_in_hal_unit)<<unit_shift, heap->priv);          
                    BUG_ON(ret != 0);
                    return 0;
              }
              else
              {
                  IPA_ALLOCATION_BUG("start %lx last %lx, hal_alloc start %lx last %lx\n",
                            start_in_hal_unit, last_in_hal_unit, hal_allocation->offset_in_heap, 
                            hal_allocation->offset_in_heap+hal_allocation->length-1);
              }
          }  
      }
      else
      {
         IPA_ALLOCATION_BUG("start %lx last %lx, hal_alloc start %lx last %lx\n",
                          start_in_hal_unit, last_in_hal_unit, hal_allocation->offset_in_heap, 
                          hal_allocation->offset_in_heap+hal_allocation->length-1);   
      }

    }
    return 0;
}


/* function to travel through heap pool allocation interval tree to find out each unused gap in range [start, last],
  * for each gap, call action to process it
  */
static int ipa_heap_pool_alloc_tree_gap_walker(struct IPA_heap *heap, unsigned long start, unsigned long last, 
	    int (*action)(struct IPA_heap *heap, unsigned long start, unsigned long last, int unit_shift))
{
	struct ipa_pool_allocation *pool_allocation;
	unsigned long gap_start = start;
	int unit_shift = ffs(~heap->alignmask)-1;
    int ret;
 
    if((start > last) || (last > heap->len-1))
    {
        IPA_HEAP_INVALID_PARA("start %lu last %lu (heap->len-1) %lu \n",
            start, last, heap->len-1);
        return -EINVAL;    
    }
	
	heap_pool_allocation_interval_tree_foreach(pool_allocation, &heap->pool_allocation_tree_root, start, last)
	{
	  if(pool_allocation->offset_in_heap > gap_start)
	  {
	     ret = action(heap, gap_start, pool_allocation->offset_in_heap-1, unit_shift);
		 if(ret)
			 return ret;
         
		 gap_start = pool_allocation->offset_in_heap + pool_allocation->length;
		 continue;
	  }
	  else if(pool_allocation->offset_in_heap+pool_allocation->length > gap_start)
	  {
		 gap_start = pool_allocation->offset_in_heap + pool_allocation->length;
	  }
	}
	
	if(gap_start <= last)  
	{
		ret = action(heap, gap_start, last, unit_shift);
		if(ret)
			return ret;
	}
    
	return 0;
}

extern struct IPA_pool *MSTAR_IPA_STR_POOL;

/*
like the follow picture show,we want find which area conflict with area B
//A1 and A2 is part of B,and A3 is beyond B.So the conflict begin is minimum start of A1/A2,
// and conflict end is maximum end of A1/A2,
//
//C1 is part of B.So the conflict begin is the start of C1,and conflict end is the end of C1,
//
//D1 is part of B,and D2 have overlap with B,and D3 is beyond B.So the conflict begin is minimum start of D1/D2,
// and conflict end is maximum end of D1/D2,
//
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

         A1            C1              A2             D1             D2          A3      D3
      -------     --------------  ------------    ------------  -------------- ------  ------

                     B
  -------------------------------------------------------------------

===>

                     A
      ----------------------------------------


                        C
                  --------------

                                                                D
                                                  ----------------------------

//////////////////////////////////////////////////////////////////////////////////////////////////////

*/
static int ipa_heap_get_conflict_areas(struct IPA_heap *heap,struct IPA_pool *pool,unsigned long start, unsigned long length, struct IPA_pool_conflict **p_conflict_info)
{
    struct ipa_pool_allocation *pool_allocation;
    unsigned long last = start+length-1;
    unsigned long new_conflict_end;
    int ret = 0;
    int i=0;
    struct IPA_pool_conflict *conflict_info = *p_conflict_info;
    
    if(pool->heap_off+pool->len > heap->len)
    {
         IPA_HEAP_ALLOC_DEBUG("%s  %d  pool->heap_off=%lu,pool->len=%lu,heap->len=%lu\n",__FUNCTION__,__LINE__,pool->heap_off,pool->len,heap->len);
         return -EINVAL;
    }
    if(start< pool->heap_off || last > (pool->heap_off+pool->len-1))
    {
         IPA_HEAP_ALLOC_DEBUG("%s  %d , pool->heap_off=%lu,pool->len=%lu,start=%lu,last=%lu\n",__FUNCTION__,__LINE__,pool->heap_off,pool->len,start,last);
         return -EINVAL;
    }

    
    IPA_HEAP_ALLOC_DEBUG("start 0x%lx last 0x%lx\n", start, last);
    //check confliction and determine merge range
    heap_pool_allocation_interval_tree_foreach(pool_allocation,
          &heap->pool_allocation_tree_root, start, last)
    {
       IPA_HEAP_ALLOC_DEBUG("=========>[%lx    %lx]\n",pool_allocation->offset_in_heap,pool_allocation->length);
       if((pool_allocation->pool != pool) 
         && (pool != MSTAR_IPA_STR_POOL))
       {
          //the required range is holded by other co-buffered pools, reject it
          if(pool_allocation->offset_in_heap <= last
             && pool_allocation->offset_in_heap+pool_allocation->length-1 >= start)
          {
             ret = -EBUSY;
             IPA_HEAP_ALLOC_DEBUG("*********>[%lx    %lx]\n",pool_allocation->offset_in_heap,pool_allocation->length);
             if(pool_allocation->pool !=  MSTAR_IPA_STR_POOL)
             {
                for(i=0;i<MAX_CONFLICT_WAKE_UP_COUNTS;i++)
                {
                    if(conflict_info->conflict_detail[i].conflict_client_filp
                        &&(0 != conflict_info->conflict_detail[i].length)
                        && (pool_allocation->client_filp == conflict_info->conflict_detail[i].conflict_client_filp))
                    {
                        struct IPA_pool_ref *pr;
                        pr=(struct IPA_pool_ref*)pool_allocation->client_filp->private_data;
                        if(pr->having_polling_thread)
                        {
                            //if alreay have,update(extend) offset_in_heap and length
                            if((pool_allocation->offset_in_heap+pool_allocation->length)
                                >=(conflict_info->conflict_detail[i].offset_in_heap  + conflict_info->conflict_detail[i].length))
                            {
                                new_conflict_end = pool_allocation->offset_in_heap+pool_allocation->length;
                            }
                            else
                            {
                                 new_conflict_end = conflict_info->conflict_detail[i].offset_in_heap  + conflict_info->conflict_detail[i].length;
                            }
                            if(pool_allocation->offset_in_heap <= conflict_info->conflict_detail[i].offset_in_heap)
                            {
                                conflict_info->conflict_detail[i].offset_in_heap = pool_allocation->offset_in_heap;
                            }
                            conflict_info->conflict_detail[i].length = new_conflict_end - conflict_info->conflict_detail[i].offset_in_heap;
                            break;//break from for.
                        }
                    }
                }
                if(MAX_CONFLICT_WAKE_UP_COUNTS == i)//if not find 
                {
                    conflict_info->conflict_counts ++ ;
                    if(conflict_info->conflict_counts > MAX_CONFLICT_WAKE_UP_COUNTS)
                    {
                        IPA_HEAP_ERROR("%s error ,conflict clients is toooooo much,will no longer get more areas,conflict_info->conflict_counts=%lu\n",__FUNCTION__,conflict_info->conflict_counts);

                        //break;//finish whole interval tree search
                        BUG();
                    }
                    atomic_long_inc_not_zero(&pool_allocation->client_filp->f_count);
                    conflict_info->conflict_detail[conflict_info->conflict_counts -1].conflict_client_filp = pool_allocation->client_filp;
                    conflict_info->conflict_detail[conflict_info->conflict_counts -1].offset_in_heap = pool_allocation->offset_in_heap;
                    conflict_info->conflict_detail[conflict_info->conflict_counts -1].length= pool_allocation->length;
                    IPA_HEAP_ALLOC_DEBUG("id=%d  client_filp=%p,offset_in_heap 0x%lx ,length=0x%lx  conflict_counts=0x%lx\n",pool_allocation->pool->id,pool_allocation->client_filp,pool_allocation->offset_in_heap,pool_allocation->length,conflict_info->conflict_counts);
                }
             }
          }
       }
    }
    return ret;
}

static int ipa_heap_alloc_buffer(struct IPA_heap *heap, struct IPA_pool *pool, struct file *client_filp, 
                                                  unsigned long start, unsigned long length, struct IPA_pool_conflict *conflict_info,  unsigned long timeout)
{
   struct ipa_pool_allocation *pool_allocation, *pool_allocation2, *n;
   unsigned long last = start+length-1;
   unsigned long merge_start = ULONG_MAX;
   unsigned long merge_last = 0;
   unsigned long merge_check_start;
   unsigned long merge_check_last;
   int ret = 0;
   bool need_get_conflict_areas =false;
   if(pool->heap_off+pool->len > heap->len)
        return -EINVAL;
   if(start< pool->heap_off || last > (pool->heap_off+pool->len-1))
        return -EINVAL;

   //check less  1 unit for possibility of merge with before allocaton
   merge_check_start = ((start > pool->heap_off)? start-1:start);
   //check more  1 unit for possibility of merge with after allocaton
   merge_check_last = ((last < (pool->heap_off + pool->len-1))?last+1:last);

   IPA_HEAP_ALLOC_DEBUG("start 0x%lx last 0x%lx\n", start, last);
   mutex_lock(&heap->lock);   
   //check confliction and determine merge range
   heap_pool_allocation_interval_tree_foreach(pool_allocation,
         &heap->pool_allocation_tree_root, merge_check_start, merge_check_last)
   {
      if((pool_allocation->pool != pool) 
        && (pool != MSTAR_IPA_STR_POOL))
      {
         //the required range is holded by other co-buffered pools, reject it
         if(pool_allocation->offset_in_heap <= last
            && pool_allocation->offset_in_heap+pool_allocation->length-1 >= start)
         {
            IPA_HEAP_ALLOC_DEBUG("%s  %d  ret is EBUSY\n",__FUNCTION__,__LINE__);
            ret = -EBUSY;
            if((timeout > 0)
                &&(pool_allocation->pool !=  MSTAR_IPA_STR_POOL))
            {
               IPA_HEAP_ALLOC_DEBUG("%s  %d  set need_get_conflict_areas\n",__FUNCTION__,__LINE__);
               need_get_conflict_areas =true; 
            }
            goto HEAP_ALLOC_DONE;
         }
         
         continue;
      }

      //for same process and same pool
      if(pool_allocation->client_filp == client_filp)
      {
         //already alloc the buffer
         if((start >= pool_allocation->offset_in_heap)
            &&((start+length) <= (pool_allocation->offset_in_heap+pool_allocation->length)))
         {
            ret = 0;            
            goto HEAP_ALLOC_DONE;
         }
         
         if(merge_start > pool_allocation->offset_in_heap)
            merge_start = pool_allocation->offset_in_heap;
         
         if(merge_last < pool_allocation->offset_in_heap + pool_allocation->length-1)
            merge_last = pool_allocation->offset_in_heap + pool_allocation->length-1;
        }
   }
   
   //merge allocations from same filp (process)
   pool_allocation2 = (struct ipa_pool_allocation*)kmalloc(sizeof(struct ipa_pool_allocation), GFP_KERNEL);
   if(!pool_allocation2)
   {
       ret = -ENOMEM;
       goto HEAP_ALLOC_DONE;
   }
   
   // allocate low level memory at first
   ret = ipa_heap_pool_alloc_tree_gap_walker(heap, start&~heap->alignmask, last|heap->alignmask, ipa_heap_hal_alloc_tree_alloc_range);
   if(ret)
   {
       kfree(pool_allocation2);
       //error during allocate low level mem, recovery
       goto ERROR_FREE_HEAP_MEM;
   }

   if(merge_last < merge_start)
   { 
       //for not find merged allocation
   	   merge_start = start;
	   merge_last = last;
   	   goto SKIP_MERGE;
   }
   
ALLOC_REMOVE_AGAIN:
   heap_pool_allocation_interval_tree_foreach_safe(pool_allocation, n,
	  &heap->pool_allocation_tree_root, merge_start, merge_last)
   {
       if(pool_allocation->client_filp == client_filp)
       {            
            BUG_ON(pool_allocation->pool != pool);
            
	      	if(merge_start<=pool_allocation->offset_in_heap && merge_last>=
				pool_allocation->offset_in_heap+pool_allocation->length-1)
	      	{
	           ipa_pool_allocation_tree_remove(pool_allocation, &heap->pool_allocation_tree_root);			   
               IPA_HEAP_ALLOC_DEBUG("pool allocation merge: remove alloc[0x%lx 0x%lx]\n", 
                pool_allocation->offset_in_heap, pool_allocation->length);
               kfree(pool_allocation);
			   goto ALLOC_REMOVE_AGAIN;
	      	}
			else
			{
			    //if still intersect, bug
			    BUG();
			}
       }
   }
    
   merge_start = min(merge_start, start);
   merge_last = max(merge_last, last);

SKIP_MERGE:
   pool_allocation2->client_filp = client_filp;
   pool_allocation2->offset_in_heap = merge_start;
   pool_allocation2->length = merge_last-merge_start+1;
   pool_allocation2->pool = pool;
   ipa_pool_allocation_tree_insert(pool_allocation2, &heap->pool_allocation_tree_root);
   mutex_unlock(&heap->lock);
   return ret;

ERROR_FREE_HEAP_MEM:
   ipa_heap_pool_alloc_tree_gap_walker(heap, start&~heap->alignmask, last|heap->alignmask, ipa_heap_hal_alloc_tree_free_range);
   mutex_unlock(&heap->lock);
   return ret;

HEAP_ALLOC_DONE:
   if(true == need_get_conflict_areas)
   {
       ipa_heap_get_conflict_areas(heap,pool,start, length, &conflict_info);
   }  
   mutex_unlock(&heap->lock);

   return ret;
}

static int ipa_heap_free_buffer(struct IPA_heap *heap, struct IPA_pool *pool, struct file *client_filp,
                                                              unsigned long start, unsigned long length)
{
   struct ipa_pool_allocation *pool_allocation, *pool_allocation2, *n;
   unsigned long last = start+length-1;
   bool need_check_mem_free = false;

   if(pool->heap_off+pool->len > heap->len)
      return -EINVAL;
   if(start < pool->heap_off || last > (pool->heap_off+pool->len-1))
      return -EINVAL;

   pool_allocation2 = (struct ipa_pool_allocation*)kmalloc(sizeof(struct ipa_pool_allocation), GFP_KERNEL);
   if(!pool_allocation2)
   {
      return -ENOMEM;
   }

   IPA_HEAP_FREE_DEBUG("start 0x%lx last 0x%lx\n", start, last);
   mutex_lock(&heap->lock);
   heap_pool_allocation_interval_tree_foreach_safe(pool_allocation, n, &heap->pool_allocation_tree_root, start, last)
   {
       if(pool_allocation->client_filp == client_filp)
       {     
            BUG_ON(pool_allocation->pool != pool);
            IPA_HEAP_FREE_DEBUG("pool_allocation->offset_in_heap 0x%lx pool_allocation->length 0x%lx\n",
                pool_allocation->offset_in_heap, pool_allocation->length);
            
            ipa_pool_allocation_tree_remove(pool_allocation, &heap->pool_allocation_tree_root);
            
            if(start<=pool_allocation->offset_in_heap 
                && last>=pool_allocation->offset_in_heap+pool_allocation->length-1)
            {
               /*contain []
                            * start       last
                            *  | ------ |
                            */
               IPA_HEAP_FREE_DEBUG("\n");               
               kfree(pool_allocation);
               need_check_mem_free = true;
            }
            //FIXME: no need if
            else if(start<= pool_allocation->offset_in_heap+pool_allocation->length-1 
                && last>=pool_allocation->offset_in_heap)
            {
                /*intersect
                              *   start           last
                              * ---|--------|---
                              */
                if(start>pool_allocation->offset_in_heap && last<pool_allocation->offset_in_heap+pool_allocation->length-1)
                {
                   //should be first time
                   BUG_ON(need_check_mem_free);
                   IPA_HEAP_FREE_DEBUG("\n");
                   
                   pool_allocation2->client_filp = client_filp;
                   pool_allocation2->offset_in_heap = last+1;
                   pool_allocation2->length = pool_allocation->offset_in_heap+pool_allocation->length-last-1;
                   pool_allocation2->pool = pool;
                   ipa_pool_allocation_tree_insert(pool_allocation2, &heap->pool_allocation_tree_root);
                   pool_allocation2 = NULL;
                   
                   pool_allocation->length = start - pool_allocation->offset_in_heap;
                   ipa_pool_allocation_tree_insert(pool_allocation, &heap->pool_allocation_tree_root);
                }
                else if(start > pool_allocation->offset_in_heap) // include ==last
                {
                   /*
                                  *   start           last
                                  * ---|-----     |
                                  */
                    IPA_HEAP_FREE_DEBUG("\n");                   
                    pool_allocation->length = start - pool_allocation->offset_in_heap;
                    ipa_pool_allocation_tree_insert(pool_allocation, &heap->pool_allocation_tree_root);
                }
                else // include ==start
                {
                    /*     
                                     * start          last
                                     *  |      ------|--
                                     */
                    if(pool_allocation->offset_in_heap+pool_allocation->length-1 > last)
                    {
                        IPA_HEAP_FREE_DEBUG("\n");                       
                        pool_allocation->length = pool_allocation->offset_in_heap+pool_allocation->length-last-1;
                        pool_allocation->offset_in_heap = last+1;
                        ipa_pool_allocation_tree_insert(pool_allocation, &heap->pool_allocation_tree_root);
                    }
                    else
                    {                        
                        IPA_ALLOCATION_BUG("start 0x%lx last 0x%lx, pool_alloc start 0x%lx last 0x%lx\n",
                            start, last, pool_allocation->offset_in_heap, 
                            pool_allocation->offset_in_heap+pool_allocation->length-1);                        
                    }
                }
                need_check_mem_free = true;             
            }
            else
            {                
                IPA_ALLOCATION_BUG("start 0x%lx last 0x%lx, pool_alloc start 0x%lx last 0x%lx\n",
                    start, last, pool_allocation->offset_in_heap, 
                    pool_allocation->offset_in_heap+pool_allocation->length-1);                
            }
            
       }
   }

   if(pool_allocation2)
       kfree(pool_allocation2);
  
   if(need_check_mem_free)
       ipa_heap_pool_alloc_tree_gap_walker(heap, start&~heap->alignmask, last|heap->alignmask, ipa_heap_hal_alloc_tree_free_range);
   
   mutex_unlock(&heap->lock);
   return 0;
}

/*  if all specified range allocated in client&pool, return 0 and give ipa address of start
  *  else return error
  */
static int ipa_heap_check_buffer(struct IPA_heap *heap, struct IPA_pool *pool, struct file *client_filp,
	                           unsigned long start, unsigned long length, IPA_ADDR *ipa)
{
	struct ipa_pool_allocation *pool_allocation;
	unsigned long gap_start = start;
    unsigned long last = start+length-1;
    int ret = 0;
 
    if((length == 0) || (start > last) || (last > heap->len-1))
    {
        IPA_HEAP_INVALID_PARA("start %lu last %lu (heap->len-1) %lu length %lu\n",
          start, last, heap->len-1, length);
        return -EINVAL;    
    }

	mutex_lock(&heap->lock);
	heap_pool_allocation_interval_tree_foreach(pool_allocation, &heap->pool_allocation_tree_root, start, last)
	{
          if((pool_allocation->client_filp != client_filp)
              || (pool_allocation->pool != pool))
              continue;
    
    	  if(pool_allocation->offset_in_heap > gap_start)
    	  {
              ret = -1;
              goto HEAP_CHECK_BUFFER_DOEN;
    	  }
    	  else if(pool_allocation->offset_in_heap+pool_allocation->length > gap_start)
    	  {
    		  gap_start = pool_allocation->offset_in_heap + pool_allocation->length;
    	  }
	}
	
	if(gap_start <= last)
	{
        ret = -1;
        goto HEAP_CHECK_BUFFER_DOEN;
	}

   heap->heap_op->heap_get_ipa(start, ipa, heap->priv);
   mutex_unlock(&heap->lock);
   return 0;

HEAP_CHECK_BUFFER_DOEN:
   mutex_unlock(&heap->lock);
   return ret;
}

static inline struct IPA_heap* _IPASpcMan_find_heap(int hid)
{
    struct IPA_heap* pos=NULL, *find = NULL;

    list_for_each_entry(pos, &IPASpcHeapMan.heaps, node)
    {    
        if(pos->hid == hid)
        {
            find = pos;
            break;
        }           
    }

    return find;
}

static int ipa_debug_heap_show(struct seq_file *s, void *unused)
{
	struct IPA_heap* heap = (struct IPA_heap*)s->private;
    struct ipa_pool_allocation *pool_allocation;
    struct ipa_hal_allocation *hal_allocation;
    unsigned long start, last;    
    struct IPAPool_device * dev = NULL;
    struct rb_node *n;    
    
    if(heap)
    {
        struct IPA_pool *pos;
        struct IPA_heap_attr info;       
     
        if(heap->heap_op->heap_get_attr(&info, heap->priv) == RET_OK)
        {
            seq_printf(s, "heapid %d name %s information---------------------------------------------\n", 
                heap->hid, info.name);
            seq_printf(s, "miu %d  miu_offset 0x%lx  heap_len 0x%lx  heap_type %d\n\n",
                info.miu, info.miu_offset, info.heap_len, info.type);
        }

        mutex_lock(&heap->lock);
        n = rb_first(&heap->pool_allocation_tree_root);
        if(!n)
        {
            seq_printf(s, "no allocation in this heap\n");
            mutex_unlock(&heap->lock);
            return 0;
        }

        pool_allocation = rb_entry(n, struct ipa_pool_allocation, itrb_node);
        dev = pool_allocation->pool->device;
        BUG_ON(!dev);       
        
        mutex_lock(&dev->lock);    
        //dump all pools in this heap
        list_for_each_entry(pos, &dev->pool, node)
        {
            if(pos->hid != heap->hid)
                continue;
            
            start = pos->heap_off;
            last = pos->heap_off + pos->len - 1;
            seq_printf(s, "dump pool name %s id %d: memory allocation range-------------------------\n", 
                pos->name, pos->id);
            
            heap_pool_allocation_interval_tree_foreach(pool_allocation,
                &heap->pool_allocation_tree_root, start, last)
            {
                if(pool_allocation->pool == pos)
                {
                    seq_printf(s, "alloc[0x%lx 0x%lx] ", pool_allocation->offset_in_heap, 
                        pool_allocation->length);
                }
            }
            seq_printf(s, "\n\n");
        }
        mutex_unlock(&dev->lock);

        //dump all hal alloc  in this heap
        seq_printf(s, "dump heap %d hal tree memory allocation range----------------------------------\n", 
            heap->hid);
        for (n = rb_first(&heap->hal_allocation_tree_root); n; n = rb_next(n)) 
        {
            hal_allocation = rb_entry(n, struct ipa_hal_allocation, itrb_node);
            seq_printf(s, "alloc[0x%lx 0x%lx] ", hal_allocation->offset_in_heap*(heap->alignmask+1), 
                hal_allocation->length*(heap->alignmask+1));
        }
        seq_printf(s, "\n\n");

        mutex_unlock(&heap->lock);
    }    
    
	return 0;
}

static int ipa_debug_heap_open(struct inode *inode, struct file *file)
{
	return single_open(file, ipa_debug_heap_show, inode->i_private);
}

static const struct file_operations ipa_debug_heap_fops = {
	.open = ipa_debug_heap_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

int IPASpcMan_register_heap(int hid,unsigned long len,
                                     unsigned long alignmask,
                                     struct IPA_heap_op *heap_op,
                                     void *data)
{
    int ret=-1;
    struct IPA_heap* heap=NULL;

    mutex_lock(&IPASpcHeapMan.lock);
    heap = _IPASpcMan_find_heap(hid);
    if(!heap)
    {
        heap = kzalloc(sizeof(struct IPA_heap),GFP_KERNEL);
        if(heap)
        {       
            char heap_name[256];
            heap->hid = hid;
            heap->len = len;
            heap->alignmask = alignmask;
            heap->heap_op = heap_op;
            heap->priv = data;
            list_add(&heap->node, &IPASpcHeapMan.heaps);
            mutex_init(&heap->lock);
            heap->hal_allocation_tree_root = RB_ROOT;
            heap->pool_allocation_tree_root = RB_ROOT;            
            ret = 0;  
            if(IPASpcHeapMan.dbg_root)
            {
                snprintf(heap_name, 256, "heap-%d",hid);
                heap->debug_file = debugfs_create_file(heap_name, 0664,
    					IPASpcHeapMan.dbg_root, heap,
    					&ipa_debug_heap_fops);                              
            }
            else
            {
                heap->debug_file = NULL;
                IPA_HEAP_ERROR("IPASpcHeapMan no debug root dir\n");
            }
        }
        else
        {
            IPA_HEAP_ERROR("inside IPASpcHeapMan kzalloc fail\n");
        }
    }
    else
    {
        IPA_HEAP_ERROR("heap %d already exists\n",hid);
    }
    mutex_unlock(&IPASpcHeapMan.lock);
    
    return ret;
}

int IPASpcMan_unregister_heap(int hid)
{
    int ret=-1;
    struct IPA_heap* heap=NULL;
    
    mutex_lock(&IPASpcHeapMan.lock);
    heap = _IPASpcMan_find_heap(hid);
    if(heap)
    {
        list_del(&heap->node);        
        kfree(heap);
        ret=0;
    }
    else
    {
        IPA_HEAP_ERROR("heapid %d not exist\n",hid);
    }
    mutex_unlock(&IPASpcHeapMan.lock);
    return ret;
}

int IPASpcMan_register_heap_pm(struct Heap_pm_op *ops, 
                                       enum IPASPCMAN_HEAP_TYPE type)
{
    struct IPA_heap_pm_op* heap_pm=NULL;

    mutex_lock(&IPASpcHeapPMMan.lock);
    list_for_each_entry(heap_pm, &IPASpcHeapPMMan.heaps_pm, node)
    {        
        if(heap_pm->type == type)
        {
            heap_pm->ref++;
            printk(KERN_ERR "111 %s %d\n", __FUNCTION__, __LINE__);
            goto REG_PM_DONE;
        }         
    }

    heap_pm = kzalloc(sizeof(struct IPA_heap_pm_op),GFP_KERNEL);
    if(heap_pm)
    {
        heap_pm->heap_pm_ops = ops;
        heap_pm->type = type;
        heap_pm->ref = 1;        
        list_add(&heap_pm->node, &IPASpcHeapPMMan.heaps_pm);     
    }
    else
        IPA_HEAP_ERROR("alloc IPA_heap_pm_op fail\n");    

REG_PM_DONE:
    mutex_unlock(&IPASpcHeapPMMan.lock);    
    return 0;
}


int IPASpcMan_unregister_heap_pm(enum IPASPCMAN_HEAP_TYPE type)
{
    struct IPA_heap_pm_op* heap_pm=NULL;

    mutex_lock(&IPASpcHeapPMMan.lock);
    list_for_each_entry(heap_pm, &IPASpcHeapPMMan.heaps_pm, node)
    {        
        if(heap_pm->type == type)
        {
            heap_pm->ref--;

            if(heap_pm->ref <= 0)
            {
                list_del(&heap_pm->node);        
                kfree(heap_pm);
            }            
            break;
        }         
    }
    
    mutex_unlock(&IPASpcHeapPMMan.lock);
    return 0;
}

#define CHECK_INPUT_PARA()  \
    if(!pool || (len == 0)  \
        || (pool_offset >= pool->len)   \
        || (pool_offset+len > pool->len)){  \
        IPA_HEAP_INVALID_PARA("pool %p len %lu pool_offset %lu pool->len %lu\n",pool,len,pool_offset,pool->len);    \
        return -EINVAL;}    \

int pool_alloc(struct IPA_pool *pool, struct file *client_filp, unsigned long pool_offset, unsigned long len,struct IPA_pool_conflict *conflict_info,  unsigned long timeout)
{
    struct IPA_heap *heap = NULL;
    unsigned long heap_start = 0;
    int ret = -1;
    int i=0;

    CHECK_INPUT_PARA();
    mutex_lock(&IPASpcHeapMan.lock);
    heap = _IPASpcMan_find_heap(pool->hid);
    mutex_unlock(&IPASpcHeapMan.lock);
    if(heap)
    {
        heap_start = pool->heap_off + pool_offset;
        IPA_HEAP_ALLOC_DEBUG("heap_start 0x%lx len 0x%lx, pool->heap_off 0x%lx pool_offset 0x%lx\n", 
            heap_start, len, pool->heap_off, pool_offset); 
        ret = ipa_heap_alloc_buffer(heap, pool, client_filp, heap_start, len,conflict_info, timeout);
        if(conflict_info->conflict_counts > 0)
        {
            IPA_HEAP_ALLOC_DEBUG("conflict_info->conflict_counts=%lu\n",conflict_info->conflict_counts);
            for(i=0;i<conflict_info->conflict_counts;i++)
                IPA_HEAP_ALLOC_DEBUG("[%p  0x%lx  0x%lx]\n",conflict_info->conflict_detail[i].conflict_client_filp,conflict_info->conflict_detail[i].offset_in_heap,conflict_info->conflict_detail[i].length);
        }
        else
        {
            IPA_HEAP_ALLOC_DEBUG("conflict_info->conflict_counts=%lu\n",conflict_info->conflict_counts);
        }
    }
    else
        IPA_HEAP_ERROR("heapid %d not exist\n", pool->hid);
    
    return ret;
}

int pool_free(struct IPA_pool *pool, struct file *client_filp, unsigned long pool_offset, unsigned long len)
{
    struct IPA_heap *heap = NULL;
    unsigned long heap_start = 0;
    int ret = -EEXIST;

    CHECK_INPUT_PARA();
    mutex_lock(&IPASpcHeapMan.lock);
    heap = _IPASpcMan_find_heap(pool->hid);
    mutex_unlock(&IPASpcHeapMan.lock);
    if(heap)
    {
        heap_start = pool->heap_off + pool_offset;
        IPA_HEAP_FREE_DEBUG("heap_start 0x%lx len 0x%lx, pool->heap_off 0x%lx pool_offset 0x%lx\n", 
            heap_start, len, pool->heap_off, pool_offset);
        ret = ipa_heap_free_buffer(heap, pool, client_filp, heap_start, len);
    }
    else
        IPA_HEAP_ERROR("heapid %d not exist\n", pool->hid);
    
    return ret;
}

int pool_check_mem(struct IPA_pool *pool, struct file *client_filp, unsigned long pool_offset, 
                unsigned long len, IPA_ADDR *ipa)
{
    struct IPA_heap *heap = NULL;
    unsigned long heap_start = 0;
    int ret = -1;

    CHECK_INPUT_PARA();
    mutex_lock(&IPASpcHeapMan.lock);
    heap = _IPASpcMan_find_heap(pool->hid);
    mutex_unlock(&IPASpcHeapMan.lock);
    if(heap)
    {        
        heap_start = pool->heap_off + pool_offset;
        ret = ipa_heap_check_buffer(heap, pool, client_filp, heap_start, len, ipa);
    }
    else
        IPA_HEAP_ERROR("heapid %d not exist\n", pool->hid);
    
    return ret;
}

void dump_pool_allocation(struct IPA_pool *pool, struct file * client_filp, struct seq_file *s)
{
    struct rb_node *n;
    struct IPA_heap *heap = NULL;
    struct ipa_pool_allocation *alloc = NULL;
    bool filp_is_null = true;
    
    if(!pool || !s)  
        return;

    if(client_filp)
        filp_is_null = false;
 
    mutex_lock(&IPASpcHeapMan.lock);
    heap = _IPASpcMan_find_heap(pool->hid);
    mutex_unlock(&IPASpcHeapMan.lock);
    if(heap)
    {     
        mutex_lock(&heap->lock);        
        for (n = rb_first(&heap->pool_allocation_tree_root); n != NULL; n = rb_next(n)) 
        {
             alloc = rb_entry(n, struct ipa_pool_allocation, itrb_node);
             
             if((pool == alloc->pool) && (filp_is_null? true:(client_filp == alloc->client_filp)))                        
                seq_printf(s, "alloc[0x%lx 0x%lx] ", alloc->offset_in_heap, alloc->length);
            
        }
        mutex_unlock(&heap->lock);
        seq_printf(s, "\n\n");
    }
}

int heap_get_ipa(int hid, unsigned int heap_off, IPA_ADDR *ipa)
{
    struct IPA_heap *heap = NULL;
    int ret = -EINVAL;

    mutex_lock(&IPASpcHeapMan.lock);
    heap = _IPASpcMan_find_heap(hid);
    mutex_unlock(&IPASpcHeapMan.lock);
    if(heap && (heap_off < heap->len))
    {        
        ret = heap->heap_op->heap_get_ipa(heap_off, ipa, heap->priv);
    }
    else
        IPA_HEAP_ERROR("heapid %d not exist  heap_off=%x, heap->len=%lx\n", hid, heap_off, heap->len);

    return ret;
}

int heap_get_attr(int hid, struct IPA_heap_attr *info)
{
    struct IPA_heap *heap = NULL;
    int ret = -EEXIST;

    mutex_lock(&IPASpcHeapMan.lock);
    heap = _IPASpcMan_find_heap(hid);
    mutex_unlock(&IPASpcHeapMan.lock);
    if(heap)
    {
        heap->heap_op->heap_get_attr(info, heap->priv);
        ret = 0;
    }
    else
        IPA_HEAP_ERROR("heapid %d not exist\n", hid);
    
    return ret;
}

int heap_suspend()
{
    struct IPA_heap_pm_op* heap_pm=NULL;

    mutex_lock(&IPASpcHeapPMMan.lock);
    list_for_each_entry(heap_pm, &IPASpcHeapPMMan.heaps_pm, node)
    {        
        if(heap_pm->heap_pm_ops->heap_suspend)
        {            
            heap_pm->heap_pm_ops->heap_suspend();
        }
    }
    mutex_unlock(&IPASpcHeapPMMan.lock);
    return 0;
}

int heap_resume()
{
    struct IPA_heap_pm_op* heap_pm=NULL;

    mutex_lock(&IPASpcHeapPMMan.lock);
    list_for_each_entry(heap_pm, &IPASpcHeapPMMan.heaps_pm, node)
    {        
        if(heap_pm->heap_pm_ops->heap_resume)
        {
           heap_pm->heap_pm_ops->heap_resume();
        }
    }
    mutex_unlock(&IPASpcHeapPMMan.lock);
    return 0;
}

int __init IPAHeap_init(void)
{
    mutex_init(&IPASpcHeapMan.lock);
    INIT_LIST_HEAD(&IPASpcHeapMan.heaps);
    IPASpcHeapMan.dbg_root = debugfs_create_dir("IPA_Heap", NULL);

    mutex_init(&IPASpcHeapPMMan.lock);
    INIT_LIST_HEAD(&IPASpcHeapPMMan.heaps_pm);
    return 0;
}
