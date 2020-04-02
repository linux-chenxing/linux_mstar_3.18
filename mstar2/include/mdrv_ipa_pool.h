#ifndef __MDRV_IPA_POOL_H_
#define __MDRV_IPA_POOL_H_
#include <linux/idr.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#ifdef CONFIG_ARM64
#define U64X_FMT "%lx"
#else
#define U64X_FMT "%llx"
#endif

enum miu_type
{
    MIU0=0,
    MIU1,
    MIU2,
    MIU_MAX
};

struct miu_addr
{
    int miu;
    unsigned long miu_offset;
};



struct IPA_pool{
    struct list_head node;
    struct list_head refs;
    struct IPAPool_device *device;
    struct mutex lock;
    int hid;
    int id;
    unsigned int flags;
    unsigned long heap_off;
    unsigned long len;
    unsigned long last_pool_ref_id;
    phys_addr_t phyaddr;
    int count;
    char *name;
    struct dentry * dbg_dir;
    struct dentry * dbg_entry;
};

#define MAX_CONFLICT_WAKE_UP_COUNTS (10)
struct IPA_pool_conflict_detail
{
    struct file *conflict_client_filp;
    unsigned long offset_in_heap;   //offset in heap space, unit:BYTE
    unsigned long length;   
};
struct IPA_pool_conflict 
{
    struct IPA_pool_conflict_detail  conflict_detail[MAX_CONFLICT_WAKE_UP_COUNTS];
    unsigned long conflict_counts;
};



#define IPAPOOLFLG_CPU_ADDR_TYPE_MASK 0x0F
#define IPAPOOLFLG_KERNEL_ADDR         0x10   //map to kernel address
#define IPAPOOLFLG_INVALID_ONLY        0x20   //invalid cache only

extern int IPA_pool_init(
                           int heap_id, unsigned int flags,
                           unsigned long heap_off,
                           unsigned long pool_len,char *name,
                           int *pool_id);
extern int IPA_pool_deinit(int pool_id);
extern int IPA_pool_get_mem(int pool_id,
                           unsigned long pool_offset,  unsigned long len,  unsigned long timeout);
extern int IPA_pool_put_mem(
                           int pool_id,
                           unsigned long pool_offset,  unsigned long len);
extern int IPA_pool_map(
                           int pool_id,
                           unsigned long pool_offset, unsigned long len,
                           unsigned long flags,unsigned long *paddr);
extern int IPA_pool_unmap(
                           unsigned long va, unsigned long len);
extern int IPA_pool_dcache_flush(
    unsigned long va, unsigned long len,
    unsigned long flags);

void str_reserve_mboot_ipa_str_pool_buffer();
void str_release_mboot_ipa_str_pool_buffer();

#endif