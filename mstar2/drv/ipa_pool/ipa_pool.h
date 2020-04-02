#ifndef _IPA_POOL_H_
#define _IPA_POOL_H_

#include <linux/types.h>
#include <linux/rbtree.h>
#include <linux/scatterlist.h>
#include <mdrv_ipa_pool.h>
#include <chip_setup.h>
#include "ipa_bootargs.h"
#include <linux/atomic.h>

typedef unsigned long IPA_ADDR;
enum IPASPCMAN_HEAP_TYPE
{
    IPASPCMAN_HEAP_TYPE_PA = 0,  //directly PA space
    IPASPCMAN_HEAP_TYPE_CMA = 1, //CMA heap
    IPASPCMAN_HEAP_TYPE_MAX
};

struct IPA_heap_attr
{
    int miu;
    char * name;
    unsigned long miu_offset;
    unsigned long heap_len;
    enum IPASPCMAN_HEAP_TYPE type;
};
struct IPA_heap_op
{
    int (*heap_allocate)(unsigned long heap_off, unsigned long len, void *para);
    int (*heap_free)(unsigned long heap_off, unsigned long len, void *para);
    int (*heap_get_ipa)(unsigned long heap_off,IPA_ADDR *addr, void *para);
    int (*heap_get_attr)(struct IPA_heap_attr *info, void *para); 
};

struct Heap_pm_op
{
    int (*heap_suspend)(void);
    int (*heap_resume)(void);
};

struct IPA_heap_pm_op
{   //one heap type to one pm ops
    struct Heap_pm_op *heap_pm_ops;
    enum IPASPCMAN_HEAP_TYPE type;
    struct list_head node;
    int ref;
};

struct IPA_heap
{
    struct list_head node;
    struct rb_root pool_allocation_tree_root; //interval tree root for pool allocations
    struct rb_root hal_allocation_tree_root;  //interval tree root for hal allocations
        
    int hid;
    unsigned long len;
    unsigned long alignmask;
    struct IPA_heap_op *heap_op;
    void * priv;
    struct dentry *debug_file;
    struct mutex lock;
};

enum cpuaddr_type
{
    ca_type_uncached=0,
    ca_type_cached_writeback,
    ca_type_cached_writethrough,
    ca_type_max
};

struct IPAPool_device{
    int major;
    int minor;
    struct cdev dev;
    struct file_operations fop;
    struct mutex apilock;
    struct mutex lock;
    struct idr idr_poolid;
    struct idr idr_ipc_pool_handle;
    struct list_head pool;
    struct dentry *debug_root;
    struct class *ipapool_class;
    struct device *ipapool_dev;
};

struct IPA_ipc_handle{
    struct file * fp;
    int ipc_pool_handle;
};

struct IPA_POLL_EVENT
{
    wait_queue_head_t wait;
    unsigned long event_count;
    struct list_head   event_head;
    struct mutex lock;
};

typedef unsigned char	                        MS_BOOL;

struct IPA_pool_ref
{
    unsigned long pool_ref_id;
    struct list_head link;  //link to pool    
    struct file *fp;
    struct IPA_pool *pool;
    struct mutex lock;
    struct list_head cpu_addr[ca_type_max];
    unsigned int flags;
    struct vm_area_struct **vma;
    struct IPA_ipc_handle ipc_handle;
    struct dentry * dbg_entry;
    
    struct IPA_POLL_EVENT poll_event;
    MS_BOOL having_polling_thread;//flag for whether having polling thread    
    
};


int IPASpcMan_register_heap(int hid,unsigned long len,
       unsigned long alignmask, struct IPA_heap_op *heap_op, void *data);        
int IPASpcMan_unregister_heap(int hid);
int IPASpcMan_register_heap_pm(struct Heap_pm_op *ops, enum IPASPCMAN_HEAP_TYPE type);
int IPASpcMan_unregister_heap_pm(enum IPASPCMAN_HEAP_TYPE type);

int pool_alloc(struct IPA_pool *pool, struct file *client_filp, unsigned long pool_offset, unsigned long len,struct IPA_pool_conflict *conflict_info,  unsigned long timeout);
int pool_free(struct IPA_pool *pool, struct file *client_filp, unsigned long pool_offset, unsigned long len);
int pool_check_mem(struct IPA_pool *pool, struct file *client_filp, unsigned long pool_offset,
                unsigned long len, IPA_ADDR *ipa);
int heap_get_ipa(int hid, unsigned int heap_off, IPA_ADDR *ipa);
int heap_get_attr(int hid, struct IPA_heap_attr *info);
int heap_suspend(void);
int heap_resume(void);
void dump_pool_allocation(struct IPA_pool *pool, struct file * client_filp, struct seq_file *s);
int IPAHeap_init(void);

#endif
