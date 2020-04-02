#include <linux/types.h>
#include <linux/fdtable.h>
#include <linux/file.h>
#include <linux/anon_inodes.h>
#include <linux/debugfs.h>
#include <linux/slab.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/mman.h>
#include <linux/version.h>
#include <linux/device.h>
#include <asm/cacheflush.h>
#include <asm/outercache.h>
#include <linux/platform_device.h>
#include <linux/pm.h>
#if defined(CONFIG_COMPAT)
#include <linux/compat.h>
#endif
#include <mdrv_mstypes.h>
#include "mdrv_types.h"
#include <MsTypes.h>
#include "mst_devid.h"
#include <mdrv_ipa_pool.h>
#include "mdrv_ipa_pool_uapi.h"
#include "ipa_pool.h"
#define IPAPOOL_MOD_NAME "ipapool"
#define IPAPOOLERR(fmt,...) printk("IPAPool Error: " fmt,##__VA_ARGS__)
#if 0
#define IPAPOOLDEBUG(fmt,...) printk("IPAPool DEBUG: " fmt,##__VA_ARGS__)
#else
#define IPAPOOLDEBUG(fmt,...) 
#endif
#define IPAPOOLWARNING(fmt, args...) printk(KERN_ERR "%s %d IPAPool Warning: " fmt, __FUNCTION__, __LINE__, ## args);
#define IPAPOOL_NAME_MAX_LONG 128
#define IPAPOOL_MAX_REFPOOL_ID  UL(-1)
#define IPAPOOL_REGION_VMA(reg) ((struct vm_area_struct*)((reg)->data))
#define IPAPOOL_IS_PGALN(val) (((val)&(~PAGE_MASK))==0)
#define IPAPOOL_VMAOFF(vma) (vma->vm_pgoff<<PAGE_SHIFT)
#define IPAPOOL_VMALEN(vma) (vma->vm_end-vma->vm_start)
#define IPAPOOL_VMAEND(vma) (IPAPOOL_VMAOFF(vma)+IPAPOOL_VMALEN(vma))
#define USE(x) (x=x)
#define IPA_MAX(a,b) ((a)>=(b)?(a):(b))
#define IPA_MIN(a,b) ((a)<=(b)?(a):(b))
#define SET_RANGE_OFFLEN(range,off,len) do{(range)->l=(off);(range)->r=(off)+(len);}while(0)
#define SET_RANGE_LR(range,left,right) do{(range)->l=(left);(range)->r=(right);}while(0)
#define ASSIGN_RANGE(range_d,range_s) do{(range_d)->l=(range_s)->l;(range_d)->r=(range_s)->r;}while(0)
#define SHIFT_RANGE(range, val) do{(range)->l +=(long)(val);(range)->r+=(long)(val);}while(0)
#define IS_RANGE_EMPTY(range) ((range)->l>=(range)->r)
struct IPA_range
{
    unsigned long l,r;
};

struct IPA_region
{
    struct list_head node;
    unsigned long pool_off;
    unsigned long len;
    unsigned long flag;
    void *data;
};

struct IPA_POLL_EVENT_NODE
{
    struct IPA_Pool_Event_Args pool_event;//event array that need send.
    struct list_head list_node;
};

static int IPAPool_Open(struct inode *inode, struct file *filp);
static int IPAPool_Release(struct inode *inode, struct file *filp);
static int IPAPool_MMap(struct file *filp, struct vm_area_struct *vma);
static long IPAPool_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
static int IPAPoolDev_Open(struct inode *inode, struct file *filp);
static int IPAPoolDev_Release(struct inode *inode, struct file *filp);
static int IPAPoolDev_MMap(struct file *filp, struct vm_area_struct *vma);
static long IPAPoolDev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
#if defined(CONFIG_COMPAT)
static long Compat_IPAPool_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
static long Compat_IPAPoolDev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
#endif
static struct IPAPool_device IPAPoolDev=
{
    .major=MDRV_MAJOR_IPAPOOL,
    .minor=MDRV_MINOR_IPAPOOL,
    .dev=
    {
        .kobj=                  {.name= IPAPOOL_MOD_NAME, },
        .owner  =               THIS_MODULE,
    },
    .fop=
    {
        .open=                  IPAPoolDev_Open,
        .release=               IPAPoolDev_Release,
        .mmap=                  IPAPoolDev_MMap,
        .unlocked_ioctl=        IPAPoolDev_ioctl,
#if defined(CONFIG_COMPAT)
		.compat_ioctl 	= 		Compat_IPAPoolDev_ioctl,
#endif
    },
};

#ifndef CONFIG_MSTAR_CMAPOOL
unsigned int bootarg_cma_debug_level = 0;
extern void set_delay_free_min_mem(int min_mem)
{    
}
#endif

static inline int IPA_range_intersect(
    struct IPA_range *sr, struct IPA_range *dr,
    struct IPA_range *res)
{
    struct IPA_range r;
    int ret=0;
    r.l=IPA_MAX(sr->l, dr->l);
    r.r=IPA_MIN(sr->r, dr->r);
    if(res)SET_RANGE_LR(res,0,0);
    if(r.l<r.r)ret=1;
    if(ret&&res){
        res->l=r.l;
        res->r=r.r;
    }
    return ret;
}
static inline struct dentry * _IPAPool_create_dbgdir(
    const char *name, struct dentry *parent)
{
    struct dentry *entry=debugfs_create_dir(name, parent);
    if(IS_ERR(entry)){
        entry=NULL;
    }
    return entry;
}
static inline char *kstr_dup(const char *str)
{
    long len=strlen(str);
    char *buf;
    if(len<=0)return NULL;
    buf=(char*)kmalloc(len+1, GFP_KERNEL);
    if(buf){
        memcpy(buf, str, len+1);
    }
    return buf;
}
static inline char *ustr_dup_len(const char __user *str, long len)
{
    char *buf;
    
    if(len<=0)
        return NULL;
    
    buf=(char*)kmalloc(len+1, GFP_KERNEL);
    if(buf){
        len=strncpy_from_user(buf, str, len);
        if(len<0){
            kfree(buf);
            return NULL;            
        }
        buf[len]=0;
    }
    return buf;
}
static inline char *ustr_dup(const char __user *str, long n)
{
    long len=strnlen_user(str, n);
    char *buf;
    
    if(len<=0)
        return NULL;
    
    buf=(char*)kmalloc(len, GFP_KERNEL);
    if(buf){
        len=strncpy_from_user(buf, str, len);
        if(len<0){
            kfree(buf);
            buf=NULL;
        }
    }
    return buf;
}

typedef int (*SEARCH_FILE_FUN)(struct file *fp, void *data);
int _IPAPool_Alloc_Fd(struct file *file)
{
    int error, fd;
    error = get_unused_fd_flags(O_CLOEXEC);
    if (error < 0)
        return error;
    fd = error;
    get_file(file);
    fd_install(fd, file);
    return fd;
}
#if 0
static struct file * _IPAPool_Search_file(SEARCH_FILE_FUN fun,void *data)
{
    struct files_struct *files;
    unsigned int fd=3;
    struct fdtable *fdt;
    struct file *fp=NULL;
    files=current->files;
    spin_lock(&files->file_lock);
    fdt = files_fdtable(files);
    while((fd=find_next_bit(fdt->open_fds, fdt->max_fds, fd))<fdt->max_fds){
        if((*fun)(fdt->fd[fd], data)){
            fp=fdt->fd[fd];
            get_file(fp);
            break;
        }
        fd++;
    }
    spin_unlock(&files->file_lock);
    return fp;
}
static int _IPAPool_Is_Pool_File(struct file *fp, void *data)
{
    struct IPA_pool *pool=(struct IPA_pool*)data;
    struct IPA_pool_ref *pr=(struct IPA_pool_ref *)fp->private_data;
    if(_IPAPool_IsValidFile(fp)
        && (pool==pr->pool))
        return 1;
    return 0;
}
#endif
static struct file_operations pool_fop=
{
    .open=                  IPAPool_Open,
    .release=               IPAPool_Release,
    .mmap=                  IPAPool_MMap,
    .unlocked_ioctl=        IPAPool_ioctl,
#if defined(CONFIG_COMPAT)
    .compat_ioctl 	= 		Compat_IPAPool_ioctl,
#endif
};
static inline int _IPAPool_IsValidFile(struct file *fp)
{
    return (fp->f_op==&pool_fop);
}

static int _IPAPool_destroy_pool(struct IPA_pool *pool)
{
    BUG_ON(!list_empty(&pool->refs));
    if(pool->dbg_dir)
        debugfs_remove_recursive(pool->dbg_dir);
    idr_remove(&IPAPoolDev.idr_poolid, pool->id);
    mutex_destroy(&pool->lock);
    kfree(pool->name);
    kfree(pool);
    return 0;
}
static int _IPAPool_release_pool(struct IPA_pool *pool)
{
    struct IPA_pool *delp=NULL;
    mutex_lock(&IPAPoolDev.lock);    
    pool->count--;
    if(pool->count<=0){
        list_del(&pool->node);
        delp=pool;
    }
    mutex_unlock(&IPAPoolDev.lock);
    if(delp){        
        _IPAPool_destroy_pool(pool);
    }
    return 0;
}

static int ipa_debug_pool_show(struct seq_file *s, void *unused)
{
    struct IPA_pool* pool = (struct IPA_pool*)s->private;
    struct IPA_pool_ref * pr = NULL;   
    
    if(pool)
    {       
        mutex_lock(&pool->lock);      
        list_for_each_entry(pr, &pool->refs, link)
        {
           mutex_lock(&pr->lock); 
           seq_printf(s, "ref pool name %s, ref id %lu information---------------------------------------\n", 
            pool->name, pr->pool_ref_id);
           dump_pool_allocation(pool, pr->fp, s);        
           mutex_unlock(&pr->lock);
        }
        
        seq_printf(s, "pool name %s information----------------------------------------------\n", pool->name);
        seq_printf(s, "id %d  hid %d heap_off 0x%lx heap_len 0x%lx ref count %d\n\n",
            pool->id, pool->hid, pool->heap_off, pool->len, pool->count);
        dump_pool_allocation(pool, NULL, s);
        mutex_unlock(&pool->lock);
    }  
    
	return 0;
}

static int ipa_debug_pool_open(struct inode *inode, struct file *file)
{
	return single_open(file, ipa_debug_pool_show, inode->i_private);
}

static const struct file_operations ipa_debug_pool_fops = {
	.open = ipa_debug_pool_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int _IPAPool_create_pool(
                           int heap_id, unsigned int flags,
                           unsigned int heap_off,
                           int pool_len,char *name,
                           struct IPA_pool **pool)
{
    int ret=-1;
    struct IPA_pool *p=NULL;
    int pool_id;
    IPA_ADDR ipaaddr;
    char *newname;
    
    if(heap_get_ipa(heap_id, heap_off, &ipaaddr)<0)
        return -1;
    //check if exist
    mutex_lock(&IPAPoolDev.lock);
    list_for_each_entry(p, &IPAPoolDev.pool, node){
        if(strcmp(p->name,name)==0){
            if(p->heap_off==heap_off &&
                p->len==pool_len &&
                p->hid==heap_id){
                p->count++;
                *pool=p;
                ret=0;
            }
            goto exit;
        }
    }
    //create a new one
    newname=kstr_dup(name);
    if(!newname)
        goto exit;
    p=kzalloc(sizeof(struct IPA_pool),GFP_KERNEL);
    if(!p)
        goto free_name;
    pool_id=idr_alloc(&IPAPoolDev.idr_poolid, p, 0, 0x0FFFFFFF, GFP_KERNEL);
    if(pool_id<0)
        goto free_pool;
    INIT_LIST_HEAD(&p->refs);
    mutex_init(&p->lock);
    p->count=1;
    p->device=&IPAPoolDev;
    p->hid=heap_id;
    p->flags=0;
    p->id=pool_id;
    p->heap_off=heap_off;
    p->len=pool_len;
    p->phyaddr=ipaaddr;
    p->name=newname;
    if(p->device->debug_root){
        p->dbg_dir = _IPAPool_create_dbgdir(p->name, p->device->debug_root);
        if(p->dbg_dir)      
            p->dbg_entry = debugfs_create_file("self", 0664,
                                p->dbg_dir, p, &ipa_debug_pool_fops);
    }
    list_add(&p->node, &IPAPoolDev.pool);     
    *pool=p;
    ret=0;    
    goto exit;
free_pool:
    kfree(p);
free_name:
    kfree(newname);
exit:
    mutex_unlock(&IPAPoolDev.lock);
    return ret;
}
static inline struct IPA_pool_ref * _IPAPool_try_get_poolref(
    struct IPA_pool *pool)
{
#if 0
    struct IPA_pool_ref *pr=NULL;
    struct file *fp=NULL;
    fp=_IPAPool_Search_file(_IPAPool_Is_Pool_File, pool);
    if(fp)
        pr=(struct IPA_pool_ref *)fp->private_data;
    return pr;
#else
    //always new client for each pool init
    return NULL;
#endif
}
static inline struct IPA_pool_ref * _IPAPool_pr_get(struct IPA_pool_ref *pr)
{
    get_file(pr->fp);
    return pr;
}
static inline void _IPAPool_pr_put(struct IPA_pool_ref *pr)
{
    fput(pr->fp);
}

static int ipa_debug_poolref_show(struct seq_file *s, void *unused)
{
    struct IPA_pool_ref* pr = (struct IPA_pool_ref*)s->private; 
    
    if(pr)
    {
        mutex_lock(&pr->lock);
        mutex_lock(&pr->pool->lock);      
        seq_printf(s, "ref pool name %s, ref id %lu information---------------------------------------\n", 
            pr->pool->name, pr->pool_ref_id);
        dump_pool_allocation(pr->pool, pr->fp, s);
        mutex_unlock(&pr->pool->lock);   
        mutex_unlock(&pr->lock);
    } 
    
    return 0;
}

static int ipa_debug_poolref_open(struct inode *inode, struct file *file)
{
	return single_open(file, ipa_debug_poolref_show, inode->i_private);
}

static const struct file_operations ipa_debug_poolref_fops = {
	.open = ipa_debug_poolref_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int _IPAPool_attach_pool(
                        struct IPA_pool *pool,
                        unsigned int flags,
                        struct file **pfp)
{
    int ret=-1, i;
    struct IPA_pool_ref *pr=NULL;
    struct file *fp=NULL;    
    bool get_refpool_id_ok = true;
    char buf[128];
    
    *pfp=NULL;
    //check exist
    if((pr=_IPAPool_try_get_poolref(pool))){
        *pfp=pr->fp;
        ret=0;
        goto exit;
    }
    //create new one
    pr=kzalloc(sizeof(struct IPA_pool_ref), GFP_KERNEL);
    if(!pr)goto exit;

    { 
        snprintf(buf,128, "%s-%d-%p",pool->name,
            task_pid_nr(current->group_leader), pr);
        //file name: poolname-gid
        fp=anon_inode_getfile(buf,&pool_fop, pr, O_RDWR);        
        if(IS_ERR(fp))goto free_pr;
    }

    pr->fp=fp;
    pr->flags=0;
    pr->pool=pool;
    
    pr->having_polling_thread = FALSE;//default not have polling thread,so initial value is FALSE.
    init_waitqueue_head(&pr->poll_event.wait);
    INIT_LIST_HEAD(&pr->poll_event.event_head);
    mutex_init(&pr->poll_event.lock); 
    
    mutex_init(&pr->lock);
    for(i=0;i<ca_type_max;i++){
        INIT_LIST_HEAD(&pr->cpu_addr[i]);
    }

    mutex_lock(&IPAPoolDev.lock);
    pool->count++;    
    mutex_lock(&pool->lock);    
    list_add(&pr->link,&pool->refs);
    pr->pool_ref_id = pool->last_pool_ref_id;
    if(pr->pool_ref_id >= IPAPOOL_MAX_REFPOOL_ID)
        get_refpool_id_ok = false;
    else
        pool->last_pool_ref_id++;
    mutex_unlock(&pool->lock);
    mutex_unlock(&IPAPoolDev.lock);

    if(get_refpool_id_ok == false){
        IPAPOOLERR("create pool ref fail: pool_ref_id overflow\n");
        goto free_pr;
    }

    if(pool->dbg_dir){        
        snprintf(buf,128, "refid-%lu", pr->pool_ref_id);
        pr->dbg_entry = debugfs_create_file(buf, 0664, pool->dbg_dir, 
            pr, &ipa_debug_poolref_fops);
    }

    *pfp=fp;
    ret=0;
    goto exit;
free_pr:
    if(pr)kfree(pr);
exit:
    return ret;
}

static void _IPAPool_destroy_poolref(struct IPA_pool_ref *pool_ref)
{
    struct IPA_pool *pool=pool_ref->pool;    
    int i;    
    
    mutex_lock(&pool->lock);
    list_del(&pool_ref->link);
    mutex_unlock(&pool->lock);
    //destroy cpu addr
    for(i=0;i<ca_type_max;i++){
        BUG_ON(!list_empty(&pool_ref->cpu_addr[i]));
    }
    
    //destroy pool region
    pool_free(pool, pool_ref->fp, 0, pool->len);
    if(pool_ref->dbg_entry)
        debugfs_remove_recursive(pool_ref->dbg_entry);
    
    //release ipc handle idr
    if(pool_ref->ipc_handle.fp > 0) 
       idr_remove(&IPAPoolDev.idr_ipc_pool_handle, pool_ref->ipc_handle.ipc_pool_handle);

    //release pool
    _IPAPool_release_pool(pool);

    //release pool_ref itself
    mutex_destroy(&pool_ref->lock);
    kfree(pool_ref);
}
static int _IPAPool_can_mapping(
    struct IPA_pool_ref *pr,
    unsigned long pool_off, unsigned long len,
    enum cpuaddr_type addrtype, struct list_head **ppos)
{
    struct IPA_region *pos=NULL,*pre=NULL;
    int ret=0;
    list_for_each_entry(pos, &pr->cpu_addr[addrtype], node){
        if(pool_off<pos->pool_off)
            break;
        pre=pos;
    }
    ret=1;
    if(ppos){
        if(pre)*ppos=&pre->node;
        else *ppos=&pr->cpu_addr[addrtype];
    }
    return ret;
}
static unsigned long _IPAPool_create_vm(struct file *file,
       unsigned long pool_off,
	unsigned long len,
	struct vm_area_struct **vma,
	enum cpuaddr_type addrtype)
{
    unsigned long ret;
    struct mm_struct *mm = current->mm;
    struct IPA_pool_ref *pr=(struct IPA_pool_ref *)file->private_data;
    unsigned long populate;
    if(!vma)return -EINVAL;
    down_write(&mm->mmap_sem);
    pr->vma=vma;
    ret = do_mmap_pgoff(file, 0, len, PROT_READ|PROT_WRITE, MAP_SHARED, pool_off>>PAGE_SHIFT,
                &populate);    
    
    if(!IS_ERR_VALUE(ret))
    {
        if(addrtype==ca_type_uncached)
        {
            (*vma)->vm_page_prot = pgprot_noncached((*vma)->vm_page_prot);                     
        }
        else if(addrtype==ca_type_cached_writethrough)
        {
            IPAPOOLWARNING("using noache instead of writethrough!!!!!\n");
            (*vma)->vm_page_prot = pgprot_noncached((*vma)->vm_page_prot);
        }
    }
    
    pr->vma=NULL;
    up_write(&mm->mmap_sem);
    if (populate)
        mm_populate(ret, populate);
    return ret;
}
static int _IPAPool_insert_vm(struct IPA_pool_ref *pr,
       unsigned long pool_offset,
	unsigned long len,
	struct vm_area_struct *vma,
	unsigned long flags)
{
    struct list_head *phead;
    struct IPA_region *region=NULL;
    int ret=-1;
    mutex_lock(&pr->lock);
    if(!_IPAPool_can_mapping(pr, pool_offset, len, flags&IPAPOOLFLG_CPU_ADDR_TYPE_MASK, &phead)){
        IPAPOOLERR("can not map %lx,%lx\n", pool_offset,len);
        ret=-IPAERROR_CANT_MAP;
        goto unlock_pr;
    }
    region=kmalloc(sizeof(struct IPA_region),GFP_KERNEL);
    if(!region){
        ret=-IPAERROR_KERNEL_NOMEM;
        goto unlock_pr;
    }
    region->pool_off=pool_offset;
    region->len=len;
    region->flag=flags;
    region->data=vma;
    vma->vm_private_data=region;
    list_add(&region->node, phead);
    ret=0;
unlock_pr:
    mutex_unlock(&pr->lock);
    return ret;
}
static inline int _IPAPool_unmap(struct mm_struct *mm, unsigned long start, size_t len)
{
    int ret=0;
    if(!mm)return 0;
    down_write(&mm->mmap_sem);
    ret = do_munmap(mm, start, len);
    up_write(&mm->mmap_sem);
    return ret;
}
static int _IPAPool_del_vm(
    struct vm_area_struct *vma)
{
    unsigned long ret;
    struct mm_struct *mm = current->mm;
    if(!vma)return -EINVAL;
    ret = _IPAPool_unmap(mm, vma->vm_start, vma->vm_end-vma->vm_start);
    return ret;
}
static int _IPAPool_vma_flush(
    unsigned long va, unsigned long len,
    unsigned long flags)
{
    struct IPA_pool_ref *pr=NULL;
    struct IPA_pool *pool=NULL;
    struct file *fp=NULL;
    struct mm_struct *mm = current->active_mm;
    struct vm_area_struct *vma;
    unsigned long end=va+len;
    int ret=-EINVAL;

    if (va <= end)
        return -EINVAL;

    down_read(&mm->mmap_sem);
    vma = find_vma(mm, va);
    if (vma && vma->vm_start < end) {
        if (va < vma->vm_start)
            va = vma->vm_start;
        if (end > vma->vm_end)
            end = vma->vm_end;
        fp=vma->vm_file;
        if(!fp ||!_IPAPool_IsValidFile(fp))
            goto exit;
        pr=(struct IPA_pool_ref *)fp->private_data;
        pool=pr->pool;
        dmac_flush_range((void*)(va& PAGE_MASK), (void*)PAGE_ALIGN(end));
        outer_flush_range(va-vma->vm_start+IPAPOOL_VMAOFF(vma)+pool->phyaddr,
                    end-vma->vm_start+IPAPOOL_VMAOFF(vma)+pool->phyaddr);
#ifndef CONFIG_OUTER_CACHE
    {
        extern void Chip_Flush_Miu_Pipe(void);
        Chip_Flush_Miu_Pipe();
    }
#endif
        ret=0;
        goto exit;
    }
exit:
    up_read(&mm->mmap_sem);
    return -EINVAL;
}
static int _IPAPool_vma_zap(
    struct IPA_pool_ref *pr,
    unsigned long pool_offset,
    unsigned long len)
{
    struct IPA_region *pos=NULL;
    struct list_head *head;
    struct vm_area_struct *vma;
    int addrtype;
    for(addrtype=0;addrtype<ca_type_max;addrtype++)
    {
        head=&pr->cpu_addr[addrtype];
        list_for_each_entry(pos, head, node)
        {
            struct IPA_range s, d,r;
            vma=IPAPOOL_REGION_VMA(pos);
            SET_RANGE_OFFLEN(&s, pool_offset, len);
            SHIFT_RANGE(&s,vma->vm_start -IPAPOOL_VMAOFF(vma));
            SET_RANGE_LR(&d, vma->vm_start, vma->vm_end);
            if(IPA_range_intersect(&s, &d, &r))
            {
                down_write(&vma->vm_mm->mmap_sem);
                zap_vma_ptes(vma, r.l,r.r-r.l);
                up_write(&vma->vm_mm->mmap_sem);
            }
        }
    }
    return 0;
}

static int _IPAPool_detach_pool(struct IPA_pool_ref *pr)
{
    _IPAPool_destroy_poolref(pr);
    return 0;
}

static inline int _IPA_pool_ref(int pool_id, struct file **pfp, struct IPA_pool_ref **ppr)
{
    int ret=-1;
    struct file *fp;
    struct IPA_pool_ref *pr;

    fp=fget(pool_id);
    if(!fp){
        IPAPOOLERR("no pool %d\n", pool_id);
        ret=-IPAERROR_NO_POOL;
        return ret;
    }
    
    if(!_IPAPool_IsValidFile(fp)){
        IPAPOOLERR("no pool %d\n", pool_id);
        ret=-IPAERROR_NO_POOL;
        goto exit;
    }
    
    pr=(struct IPA_pool_ref*)fp->private_data;
    if(pfp) *pfp=fp;
    if(ppr) *ppr=pr;

    return 0;
exit:
    fput(fp);
    return ret;
}

static inline int _IPA_pool_entry_no_care_pool_offset_len(
                           int pool_id, struct file **pfp,struct IPA_pool_ref **ppr,struct IPA_pool **ppool)
{
    int ret=-1;
    struct file *fp;
    struct IPA_pool_ref *pr;
    struct IPA_pool *pool;

    fp=fget(pool_id);
    if(!fp){
        IPAPOOLERR("no pool %d\n", pool_id);
        ret=-IPAERROR_NO_POOL;
        return ret;
    }
    if(!_IPAPool_IsValidFile(fp)){
        IPAPOOLERR("no pool %d\n", pool_id);
        ret=-IPAERROR_NO_POOL;
        goto exit;
    }
    pr=(struct IPA_pool_ref*)fp->private_data;
    pool=pr->pool;
    
    if(pfp)*pfp=fp;
    if(ppr)*ppr=pr;
    if(ppool)*ppool=pool;
    return 0;
exit:
    fput(fp);
    return ret;
}


static inline int _IPA_pool_entry(
                           int pool_id, unsigned long pool_offset,  unsigned long len,
                           struct file **pfp,struct IPA_pool_ref **ppr,struct IPA_pool **ppool)
{
    int ret=-1;
    struct file *fp;
    struct IPA_pool_ref *pr;
    struct IPA_pool *pool;
    if(!IPAPOOL_IS_PGALN(pool_offset) || !IPAPOOL_IS_PGALN(len)){
        IPAPOOLERR("not aligned pool range (%lx,%lx)\n", pool_offset, len);
        ret=-IPAERROR_RANGE_NOALIGN;
        return ret;
    }
    fp=fget(pool_id);
    if(!fp){
        IPAPOOLERR("no pool %d\n", pool_id);
        ret=-IPAERROR_NO_POOL;
        return ret;
    }
    if(!_IPAPool_IsValidFile(fp)){
        IPAPOOLERR("no pool %d\n", pool_id);
        ret=-IPAERROR_NO_POOL;
        goto exit;
    }
    pr=(struct IPA_pool_ref*)fp->private_data;
    pool=pr->pool;
    if((pool_offset >= pool->len)
       ||(pool_offset+len) > pool->len){
       IPAPOOLERR("invalid pool range (%lx,%lx)\n", pool_offset, len);
       ret=-IPAERROR_RANGE_INVLAID;
       goto exit;
    }
    if(pfp)*pfp=fp;
    if(ppr)*ppr=pr;
    if(ppool)*ppool=pool;
    return 0;
exit:
    fput(fp);
    return ret;
}

static inline int _IPA_pool_exit(
                           struct file *fp,struct IPA_pool_ref *pr,struct IPA_pool *pool)
{
    fput(fp);
    USE(pr);
    USE(pool);
    return 0;
}
int IPA_pool_init(int heap_id, unsigned int flags,
                           unsigned long heap_off,
                           unsigned long pool_len,char *name,
                           int *pool_id)
{
    int ret=-1;
    struct IPA_pool *pool=NULL;
    struct file *fp=NULL;
    int fd;    
    if(!IPAPOOL_IS_PGALN(heap_off) || !IPAPOOL_IS_PGALN(pool_len)){
        IPAPOOLERR("not aligned heap range (%lx,%lx)\n", heap_off, pool_len);
        ret=-IPAERROR_RANGE_NOALIGN;
        return ret;
    }
    mutex_lock(&IPAPoolDev.apilock);
    ret=_IPAPool_create_pool(heap_id, flags, heap_off, pool_len, name, &pool);
    if(ret<0){
        IPAPOOLERR("create pool error (%d, %x, %s, %lx,%lx)\n",
            heap_id, flags, name, heap_off, pool_len);
        ret=-IPAERROR_CREATE_POOL;
        goto exit;
    }
    ret=_IPAPool_attach_pool(pool, flags, &fp);
    if(ret<0){
        IPAPOOLERR("attach pool error (%d, %x, %s, %lx,%lx)\n",
            heap_id, flags, name, heap_off, pool_len);
        ret=-IPAERROR_ATTACH_POOL;
        goto release_pool;
    }
    //install fd
    fd=_IPAPool_Alloc_Fd(fp);
    if(fd<0){
        IPAPOOLERR("alloc fd error (%d, %x, %s, %lx,%lx)\n",
            heap_id, flags, name, heap_off, pool_len);
        ret=-IPAERROR_ATTACH_POOL;
        goto release_file;
    }    
    *pool_id=fd;
    ret=0;
release_file:
    fput(fp);
release_pool:
    _IPAPool_release_pool(pool);
exit:
    mutex_unlock(&IPAPoolDev.apilock);
    return ret;
}

int IPA_pool_deinit(int pool_id)
{
    int ret;
    mutex_lock(&IPAPoolDev.apilock);

    struct IPA_pool_ref *pr=NULL;
    struct IPA_pool *pool=NULL;
    struct file *fp=NULL;
    
    ret=_IPA_pool_entry_no_care_pool_offset_len(pool_id, &fp, &pr, &pool);
    if(ret<0){
        IPAPOOLERR("Invalid para\n");
        return ret;
    }
    
//    if(pr->having_polling_thread)
    {
        IPAPOOLDEBUG("%s:%d  waitqueue_active %d \n",__FUNCTION__,__LINE__,waitqueue_active(&pr->poll_event.wait));
//        if ( waitqueue_active(&pr->poll_event.wait))//if active (active means not empty)
        {
            //step1 clear event count and event array.
            mutex_lock(&pr->poll_event.lock);    

            struct IPA_POLL_EVENT_NODE *event_node;
            struct IPA_POLL_EVENT_NODE *tmp;
            //delete all entries in list
            list_for_each_entry_safe(event_node,tmp,&pr->poll_event.event_head,list_node)
            {
                list_del(&event_node->list_node);
                kfree(event_node);
            }
            event_node = (struct IPA_POLL_EVENT_NODE *)kmalloc(sizeof(struct IPA_POLL_EVENT_NODE),GFP_KERNEL);
            if(!event_node)
            {
                mutex_unlock(&pr->poll_event.lock); 
                mutex_unlock(&IPAPoolDev.apilock);
                printk("%s:%d kmalloc fail , sizeof(struct IPA_POLL_EVENT_NODE)=%d\n",__FUNCTION__,__LINE__,sizeof(struct IPA_POLL_EVENT_NODE));
                return -IPAERROR_KERNEL_NOMEM;
            }
            event_node->pool_event.event = IPA_EVENT_NO_WAIT;//tell user that no need wait
            pr->poll_event.event_count =1;//set with only one event,and later only deal with this event
            list_add(&event_node->list_node,&pr->poll_event.event_head);
    
            //step2:give IPA_EVENT_NO_WAIT EVENT
            //only set event, for no care value of pool_handle_id and start and length
            
            IPAPOOLDEBUG("%s:%d  pr->poll_event.event_count=%lu,  event_node->pool_event.event=%d\n",__FUNCTION__,__LINE__
                   ,pr->poll_event.event_count,event_node->pool_event.event);
            mutex_unlock(&pr->poll_event.lock);     
            //step3: wake up
            wake_up(&pr->poll_event.wait);
        }
        //else
       // {
       //     IPAPOOLDEBUG("%s:%d  waitqueue not active\n",__FUNCTION__,__LINE__);
       // }
    }

    ret=__close_fd(current->files,pool_id);
    _IPA_pool_exit(fp, pr, pool);

    mutex_unlock(&IPAPoolDev.apilock);
    return ret;
}

int IPA_pool_get_mem(
                           int pool_id,
                           unsigned long pool_offset,  unsigned long len,  unsigned long timeout)
{
    struct IPA_pool_ref *pr=NULL;
    struct IPA_pool *pool=NULL;
    struct file *fp=NULL;
    int ret=-1;
    struct IPA_pool_conflict conflict_info;
    int i = 0,j=0;
    ret=_IPA_pool_entry(pool_id, pool_offset, len,
                                   &fp, &pr, &pool);
    if(ret<0){
        IPAPOOLERR("Invalid para\n");
        return ret;
    }

    if(timeout > 0)
    {
        memset(&conflict_info,0,sizeof(struct IPA_pool_conflict));
    }
    
    mutex_lock(&IPAPoolDev.apilock);
    
    //find each conflict areas for each client.And send event to them.
    ret=pool_alloc(pool,fp, pool_offset, len,&conflict_info, timeout);
    if(0 == ret){//alloc success
        goto exit;
    }

    IPAPOOLDEBUG("%s:%d pool_alloc ret=%d\n",__FUNCTION__,__LINE__,ret);
    if(conflict_info.conflict_counts > 0)
    {
        IPAPOOLDEBUG("%s conflict_info.conflict_counts=%lu\n",__FUNCTION__,conflict_info.conflict_counts);
        for(i=0;i<conflict_info.conflict_counts;i++)
           IPAPOOLDEBUG("%s [%p  0x%lx  0x%lx]\n",__FUNCTION__,conflict_info.conflict_detail[i].conflict_client_filp,conflict_info.conflict_detail[i].offset_in_heap,conflict_info.conflict_detail[i].length);
    }
    else
    {
        IPAPOOLDEBUG("%s conflict_info.conflict_counts=%lu\n",__FUNCTION__,conflict_info.conflict_counts);
    }

    if(timeout > 0)
    {
        IPAPOOLDEBUG("%s:%d,timeout=%lu\n",__FUNCTION__,__LINE__,timeout);

        if(conflict_info.conflict_counts > 0)
        {
            for(i=0;i<conflict_info.conflict_counts;i++)
            {
                //will_wake_client_filp = conflict_info.conflict_detail[i].conflict_client_filp;
                BUG_ON(!conflict_info.conflict_detail[i].conflict_client_filp);
                if(conflict_info.conflict_detail[i].conflict_client_filp)
                {
                    struct IPA_pool_ref *wake_pr;
                    wake_pr = (struct IPA_pool_ref*)conflict_info.conflict_detail[i].conflict_client_filp->private_data;
                    IPAPOOLDEBUG("%s:%d,wake_pr->pool->id=%d,  wake_pr=%p  conflict_info.conflict_detail[%d].conflict_client_filp=%p\n", __FUNCTION__, __LINE__, wake_pr->pool->id,wake_pr, i, conflict_info.conflict_detail[i].conflict_client_filp);

                    if(wake_pr->having_polling_thread)
                    {
                        if ( waitqueue_active(&wake_pr->poll_event.wait))
                        {  
                            mutex_lock(&wake_pr->poll_event.lock);
       
                            struct IPA_POLL_EVENT_NODE *event_node = (struct IPA_POLL_EVENT_NODE *)kmalloc(sizeof(struct IPA_POLL_EVENT_NODE),GFP_KERNEL);
                            if(!event_node)
                            {
                                ret = -IPAERROR_KERNEL_NOMEM;
                                printk("%s:%d kmalloc fail , sizeof(struct IPA_POLL_EVENT_NODE)=%d\n",__FUNCTION__,__LINE__,sizeof(struct IPA_POLL_EVENT_NODE));
                                mutex_unlock(&wake_pr->poll_event.lock); 
                                fput(&conflict_info.conflict_detail[i].conflict_client_filp->f_count);
                                goto exit;
                            }
                            event_node->pool_event.event = IPA_EVENT_CONFLICT;//tell user that need free
                            event_node->pool_event.pool_handle_id = wake_pr->pool->id;
                            event_node->pool_event.start = conflict_info.conflict_detail[i].offset_in_heap -wake_pr->pool->heap_off;
                            event_node->pool_event.length = conflict_info.conflict_detail[i].length;
                            IPAPOOLDEBUG("%s:%d  wake_pr=%p,wake_pr->pool->id=%d    event %d  start_length[0x%llx,   0x%llx]\n",__FUNCTION__,__LINE__,wake_pr,wake_pr->pool->id,event_node->pool_event.event,event_node->pool_event.start,event_node->pool_event.length);

#if 0
                            //before add ,debug code:
                            struct IPA_POLL_EVENT_NODE *event_node_debug;
                            struct IPA_POLL_EVENT_NODE *tmp_debug;
                            if(!list_empty(&wake_pr->poll_event.event_head))
                            {
                                list_for_each_entry_safe(event_node_debug,tmp_debug,&wake_pr->poll_event.event_head,list_node)
                                {
                                    IPAPOOLDEBUG(" before add pool_event_args %u  %d  [0x%llx 0x%llx]    wake_pr=%p \n",event_node_debug->pool_event.pool_handle_id,event_node_debug->pool_event.event,event_node_debug->pool_event.start,event_node_debug->pool_event.length,wake_pr);
                                }
                            }
                            else
                                IPAPOOLDEBUG(" before add pool_event_args list is empty wake_pr=%p \n",wake_pr);
#endif

                            list_add_tail(&event_node->list_node,&wake_pr->poll_event.event_head);
                            wake_pr->poll_event.event_count++;

#if 0
                            //after add ,debug code:
                            if(!list_empty(&wake_pr->poll_event.event_head))
                            {
                                list_for_each_entry_safe(event_node_debug,tmp_debug,&wake_pr->poll_event.event_head,list_node)
                                {
                                    IPAPOOLDEBUG(" after add pool_event_args %u  %d  [0x%llx 0x%llx]    wake_pr=%p \n",event_node_debug->pool_event.pool_handle_id,event_node_debug->pool_event.event,event_node_debug->pool_event.start,event_node_debug->pool_event.length,wake_pr);
                                }
                            }
                            else
                                IPAPOOLDEBUG(" after add pool_event_args list is empty wake_pr=%p \n",wake_pr);
#endif                            
                            IPAPOOLDEBUG("event_count=%lu wake_pr=%p\n",wake_pr->poll_event.event_count,wake_pr);
                            IPAPOOLDEBUG("event info %d , %llx %llx\n",event_node->pool_event.event
                                ,event_node->pool_event.start,event_node->pool_event.length);
                            mutex_unlock(&wake_pr->poll_event.lock); 

                            //step2:wake up this polling
                            wake_up(&wake_pr->poll_event.wait);
                        }
                    }
                    fput(&conflict_info.conflict_detail[i].conflict_client_filp->f_count);
                }
            }          
        }
    }
    if(ret<0){
        goto exit;
    }

    ret=0;
exit:
    mutex_unlock(&IPAPoolDev.apilock);
    _IPA_pool_exit(fp, pr, pool);
    return ret;
}
int IPA_pool_put_mem(
                           int pool_id,
                           unsigned long pool_offset,  unsigned long len)
{
    struct IPA_pool_ref *pr=NULL;
    struct IPA_pool *pool=NULL;
    struct file *fp=NULL;
    int ret=-1;
    ret=_IPA_pool_entry(pool_id, pool_offset, len,
                                   &fp, &pr, &pool);
    if(ret<0){
        IPAPOOLERR("Invalid para\n");
        return ret;
    }
    mutex_lock(&IPAPoolDev.apilock);
    mutex_lock(&pr->lock);
    _IPAPool_vma_zap(pr, pool_offset,len);

    if(pr->having_polling_thread)
    {
        IPAPOOLDEBUG("%s:%d   before pool_free waitqueue_active(&pr->poll_event.wait)=%d   pool_id=%d,pr=%p\n",__FUNCTION__,__LINE__,waitqueue_active(&pr->poll_event.wait),pool_id,pr);
    }
    else
        IPAPOOLDEBUG("%s:%d    before pool_free   not having_polling_thread pool_id=%d,pr=%p\n",__FUNCTION__,__LINE__,pool_id,pr);
    
    ret=pool_free(pool,fp, pool_offset, len);
    
    if(pr->having_polling_thread)
    {
        IPAPOOLDEBUG("%s:%d   after pool_free waitqueue_active(&pr->poll_event.wait)=%d pool_id=%d,pr=%p\n",__FUNCTION__,__LINE__,waitqueue_active(&pr->poll_event.wait),pool_id,pr);
    }
    else
        IPAPOOLDEBUG("%s:%d    after  pool_free   not having_polling_thread  pool_id=%d,pr=%p\n",__FUNCTION__,__LINE__,pool_id,pr);
    
    mutex_unlock(&pr->lock);
    if(ret<0){
        goto exit;
    }
    ret=0;
exit:
    mutex_unlock(&IPAPoolDev.apilock);
    _IPA_pool_exit(fp, pr, pool);
    return ret;
}
int IPA_pool_map(
                           int pool_id,
                           unsigned long pool_offset, unsigned long len,
                           unsigned long flags,unsigned long *paddr)
{
    struct IPA_pool_ref *pr=NULL;
    struct IPA_pool *pool=NULL;
    struct file *fp=NULL;
    struct vm_area_struct *vma;
    enum cpuaddr_type addrtype=flags&IPAPOOLFLG_CPU_ADDR_TYPE_MASK;
    unsigned long addr;
    int ret=-1;
    if(flags&IPAPOOLFLG_KERNEL_ADDR || addrtype>=ca_type_max){
        IPAPOOLERR("invalid addr type %lx\n", flags);
        return -IPAERROR_ADDR_TYPE_INV;
    }

    if(flags&ca_type_cached_writethrough)
        IPAPOOLWARNING("use noache instead of writethrough!!!!!\n");
    
    ret=_IPA_pool_entry(pool_id, pool_offset, len,
                                   &fp, &pr, &pool);
    if(ret<0){
        IPAPOOLERR("Invalid para\n");
        return ret;
    }
    mutex_lock(&IPAPoolDev.apilock);
    addr=_IPAPool_create_vm(fp,pool_offset, len, &vma, addrtype);
    if(IS_ERR_VALUE(addr)){
        IPAPOOLERR("create vm fail\n");
        ret=-IPAERROR_CREATE_VM;
        goto exit;
    }
    ret=_IPAPool_insert_vm(pr, pool_offset, len, vma, flags);
    if(ret<0){
        goto del_vma;
    }
    ret=0;
    *paddr=addr;    
    goto exit;
del_vma:
    _IPAPool_del_vm(vma);
exit:
    mutex_unlock(&IPAPoolDev.apilock);
    _IPA_pool_exit(fp, pr, pool);
    return ret;
}
int IPA_pool_unmap(
                           unsigned long va, unsigned long len)
{
    int ret=-1;
    mutex_lock(&IPAPoolDev.apilock);
    ret=_IPAPool_unmap(current->mm, va, len);
    mutex_unlock(&IPAPoolDev.apilock);
    return ret;
}
int IPA_pool_dcache_flush(
    unsigned long va, unsigned long len,
    unsigned long flags)
{

    int ret=-1;
    mutex_lock(&IPAPoolDev.apilock);
    ret=_IPAPool_vma_flush(va, len, flags);
    mutex_unlock(&IPAPoolDev.apilock);
    return ret;
}
int _ipapool_cpu_page_fault_handler(struct vm_area_struct *vma, struct vm_fault *vmf)
{
    void __user * address;
    struct file *fp=NULL;
    struct IPA_pool_ref *pr=NULL;
    int ret=VM_FAULT_SIGBUS;
    unsigned long pool_offset;
    IPA_ADDR ipaaddr;
    unsigned long adr;

    fp=vma->vm_file;
    address = vmf->virtual_address;
    if(!fp)
        return ret;
    
    adr=(((unsigned long)address)&PAGE_MASK);
    pool_offset=(vma->vm_pgoff<<PAGE_SHIFT)+(adr-vma->vm_start);
    pr=(struct IPA_pool_ref*)fp->private_data;

    mutex_lock(&pr->lock);   
    if(pool_check_mem(pr->pool,fp, pool_offset,PAGE_SIZE, &ipaaddr)<0){
        IPAPOOLERR("%s %d no allocation of pool ref id %lu pool_offset 0x%lx length 0x%lx\n", 
            __FUNCTION__, __LINE__, pr->pool_ref_id, pool_offset, PAGE_SIZE);
        goto exit; 
    }

    ret = vm_insert_pfn(vma, adr, __phys_to_pfn(ipaaddr));
    if (ret)
        ret = VM_FAULT_ERROR;
    else
        ret = VM_FAULT_NOPAGE;   
    
exit:
    mutex_unlock(&pr->lock);
    return ret;
}
void _ipapool_vma_open(struct vm_area_struct * vma)
{
    struct file *fp=NULL;
    struct IPA_pool_ref *pr;
    unsigned long pool_offset;
    unsigned long len;
    struct IPA_region *region;
    if (NULL == vma)
        return;
    
    pool_offset=(vma->vm_pgoff<<PAGE_SHIFT);
    len=vma->vm_end-vma->vm_start;
    fp=vma->vm_file;
    region=(struct IPA_region *)vma->vm_private_data;
    if(!fp || !region)
        return;
    
    pr=(struct IPA_pool_ref *)fp->private_data;
    if(!pr)
        return;
    
    _IPAPool_insert_vm(pr, pool_offset, len, vma, region->flag);    
}
void _ipapool_vma_close(struct vm_area_struct * vma)
{
    struct IPA_pool_ref *pr=NULL;
    struct file *fp=NULL;
    struct IPA_region *region=NULL;
    
    region=(struct IPA_region *)vma->vm_private_data;
    fp=vma->vm_file;
    if(!fp) return;
    
    pr=(struct IPA_pool_ref *)fp->private_data;
    if(!pr) return;
    
    mutex_lock(&pr->lock);
    list_del(&region->node);
    kfree(region);
    mutex_unlock(&pr->lock);
}
static struct vm_operations_struct ipapool_vm_ops =
{
    .open = _ipapool_vma_open,
    .close = _ipapool_vma_close,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
    .fault = _ipapool_cpu_page_fault_handler
#else
    .nopfn = _ipapool_cpu_page_fault_handler
#endif
};
static int IPAPool_Open(struct inode *inode, struct file *filp)
{
    return 0;
}
static int IPAPool_Release(struct inode *inode, struct file *filp)
{
    struct IPA_pool_ref *pr=filp->private_data; 
    _IPAPool_detach_pool(pr);
    return 0;
}
static int IPAPool_MMap(struct file *filp, struct vm_area_struct *vma)
{
    struct IPA_pool_ref *pr=(struct IPA_pool_ref *)filp->private_data;
    unsigned long pool_offset=0, len=0;
    
    if (NULL == vma)
        return -EINVAL; 
    pool_offset=(vma->vm_pgoff<<PAGE_SHIFT);
    len=vma->vm_end-vma->vm_start;
    
    vma->vm_flags |=  VM_SHARED | VM_MAYSHARE | VM_SPECIAL;
    vma->vm_page_prot = vm_get_page_prot(vma->vm_flags);
    //default set as writeback
    #ifdef CONFIG_ARM64
    vma->vm_page_prot = pgprot_cached(vma->vm_page_prot);
    #else    
    vma->vm_page_prot = __pgprot_modify(vma->vm_page_prot, L_PTE_MT_MASK, L_PTE_MT_WRITEBACK);
    #endif
    
    vma->vm_ops = &ipapool_vm_ops;
    if(!(pr->vma)){        
        if(0>_IPAPool_insert_vm(pr, pool_offset, len, vma, ca_type_cached_writeback))
            return -EINVAL;
    }else       
        *(pr->vma)=vma;
       
    return 0;
}

//if success, return >=0; else fail
int IPA_pool_getipchandle(int fd)
{
    int ret = -1;
    int ipc_pool_handle_id = 0;
    struct file * fp = NULL;
    struct IPA_pool_ref * pool_ref = NULL;

    if(_IPA_pool_ref(fd, &fp, &pool_ref))
        return -EINVAL;
    
    if(pool_ref->ipc_handle.fp > 0)
    {
        ret = pool_ref->ipc_handle.ipc_pool_handle;
        goto GETIPCHANDLE_DONE;
    }

    //set start parameter of idr_alloc() with not 0 but an other value,
    //to help users not to be confused.
    ipc_pool_handle_id = idr_alloc(&IPAPoolDev.idr_ipc_pool_handle, 
                        &pool_ref->ipc_handle, 0xff, 0, GFP_KERNEL);
    if(ipc_pool_handle_id < 0)
    {
        ret = -ENOMEM;
        goto GETIPCHANDLE_DONE;
    }

    pool_ref->ipc_handle.fp = fp;
    pool_ref->ipc_handle.ipc_pool_handle = ipc_pool_handle_id;
    ret = ipc_pool_handle_id;
    
GETIPCHANDLE_DONE:
    fput(fp);    
    return ret;
}

static int IPA_pool_file_install(int ipc_pool_handle_id)
{    
    int error = -1, fd = -1;
    struct IPA_ipc_handle * handle = NULL;

    if(ipc_pool_handle_id < 0)
        return -EINVAL;
   
    handle = idr_find(&IPAPoolDev.idr_ipc_pool_handle, ipc_pool_handle_id);
    if(!handle)
        return -EINVAL;

    if(handle->fp == NULL)
        return -EFAULT;

    if(!_IPAPool_IsValidFile(handle->fp))
    {
        IPAPOOLERR("invalid fp in %s\n", __FUNCTION__);
        return -IPAERROR_NO_POOL;        
    }

    //already released handle->fp
    if(!atomic_long_inc_not_zero(&handle->fp->f_count))
        return -EINVAL;        

    error = get_unused_fd_flags(O_CLOEXEC);
    if (error < 0)
    {
        fput(handle->fp);
        return error;
    }
    
    fd = error;
    fd_install(fd, handle->fp);    
    return fd;   
}

static int IPA_pool_poll_client(struct IPA_Pool_Event_Args *pool_event_args)
{
    int ret=-1;
    struct file *fp;        
    struct IPA_pool_ref *pr;

    //clear value except pool_handle_id with 0
    unsigned int pool_id = pool_event_args->pool_handle_id;
    memset(pool_event_args,0,sizeof(struct IPA_Pool_Event_Args));
    pool_event_args->pool_handle_id = pool_id;
    
    fp=fget(pool_id);
    if(!fp){
        IPAPOOLERR("no pool %d\n", pool_id);
        ret=-IPAERROR_NO_POOL;
        
        printk("%s:%d return pool_event_args->pool_handle_id=%u\n",__FUNCTION__,__LINE__,pool_event_args->pool_handle_id);
        return ret;
    }
    if(!_IPAPool_IsValidFile(fp)){
        IPAPOOLERR("no pool %d\n", pool_id);
        ret=-IPAERROR_NO_POOL;
        goto exit;
    }
    pr=(struct IPA_pool_ref*)fp->private_data;
    
    if(FALSE == pr->having_polling_thread)
    {
        mutex_lock(&pr->poll_event.lock); 
        pr->having_polling_thread = TRUE;        
        pr->poll_event.event_count =0;//default event count is 0
        IPAPOOLDEBUG("%s:%d  after set flag  pr->having_polling_thread=%d   pool_id=%u pr=%p",__FUNCTION__,__LINE__,pr->having_polling_thread,pool_id,pr);
        mutex_unlock(&pr->poll_event.lock);  
    }
    
    mutex_lock(&pr->poll_event.lock); 
    while(0 == pr->poll_event.event_count )//while evene count is 0,sleep
    {
        IPAPOOLDEBUG("%s:%d will interruptible_sleep_on.   pool_id=%u,fp=%p,pr=%p\n",__FUNCTION__,__LINE__,pool_id,fp,pr);
        mutex_unlock(&pr->poll_event.lock);
        interruptible_sleep_on(&pr->poll_event.wait);
        mutex_lock(&pr->poll_event.lock);
        IPAPOOLDEBUG("%s:%d after interruptible_sleep_on.   pool_id=%u,fp=%p,pr=%p\n",__FUNCTION__,__LINE__,pool_id,fp,pr);
    }

    //if event count is not 0, get first valid event and return .
    //If both clientB and clientC  conflict with clientA,then clientA has two events.
    if(pr->poll_event.event_count > 0)
    {
        struct IPA_POLL_EVENT_NODE *event_node;
        struct IPA_POLL_EVENT_NODE *tmp;
        
        IPAPOOLDEBUG("%s %d    pool_event_args->pool_handle_id=%u   pr=%p\n",__FUNCTION__,__LINE__,pool_event_args->pool_handle_id,pr);
        list_for_each_entry_safe(event_node,tmp,&pr->poll_event.event_head,list_node)
        {
            if(IPA_EVENT_CONFLICT == event_node->pool_event.event)
            {
                IPAPOOLDEBUG("%s %d   pool_event_args->pool_handle_id=%u   event_node->pool_event.pool_handle_id=%u   pr=%p\n",__FUNCTION__,__LINE__,pool_event_args->pool_handle_id,event_node->pool_event.pool_handle_id,pr);
                IPAPOOLDEBUG("%s %d   start=0x%llx   length=0x%llx\n",__FUNCTION__,__LINE__,event_node->pool_event.start,event_node->pool_event.length);

                BUG_ON(pool_event_args->pool_handle_id != event_node->pool_event.pool_handle_id);
            }
            pool_event_args->event = event_node->pool_event.event;
            pool_event_args->pool_handle_id = event_node->pool_event.pool_handle_id;
            pool_event_args->start = event_node->pool_event.start;
            pool_event_args->length = event_node->pool_event.length;
            IPAPOOLDEBUG("%s:%d pr=%p   fp=%p\n",__FUNCTION__,__LINE__,pr,fp);
            
            IPAPOOLDEBUG("   event_node %u  %d  [0x%llx 0x%llx]\n",event_node->pool_event.pool_handle_id,event_node->pool_event.event,event_node->pool_event.start,event_node->pool_event.length);
            pr->poll_event.event_count --;
            list_del(&event_node->list_node);
            kfree(event_node);
            break;// after find one event ,send it.And here no care other events,for this function will be invoked again later.
        }
        IPAPOOLDEBUG("%s %d \n",__FUNCTION__,__LINE__);
    }
    mutex_unlock(&pr->poll_event.lock); 
    
    ret = 0;
    USE(pr);
exit:
    fput(fp);
    return ret;
}

static long IPAPool_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    return -EINVAL;
}
static int IPAPoolDev_Open(struct inode *inode, struct file *filp)
{
    filp->private_data=&IPAPoolDev;
    return 0;
}
static int IPAPoolDev_Release(struct inode *inode, struct file *filp)
{
    filp->private_data=NULL;
    return 0;
}

static int IPAPoolDev_MMap(struct file *filp, struct vm_area_struct *vma)
{
    return -EINVAL;;
}
static inline unsigned int _ipapool_ioctl_dir(unsigned int cmd)
{
	switch (cmd) {
	default:
		return _IOC_DIR(cmd);
	}
}

//here name of str pool client  should be unique with any other pool client.
#define STR_POOL_CLIENT "str_pool_client"
struct IPA_pool *MSTAR_IPA_STR_POOL=NULL;
static bool ipa_str_pool_para_set_done=false;
#ifdef CONFIG_MP_CMA_PATCH_MBOOT_STR_USE_CMA
extern int mboot_str_heap_id;
#endif
void str_reserve_mboot_ipa_str_pool_buffer()
{
    int ret=0;
    int pool_id;
    struct file *client_filp = (struct file *)STR_POOL_CLIENT;
    struct IPA_heap_attr heap_attr;

    if(false == ipa_str_pool_para_set_done)
    {
        if(mstar_driver_boot_ipa_str_pool_num != 1 )
        {
            //do not have IPA_STR_POOL=XXX
            // (such as IPA_STR_POOL=str,miu=0,heap_id=0x14,sz=0x800000,st=0xac00000)
            //so we need to check whether have  VDEC_XC_STR_MBOOT
            //(such as CMA2=VDEC_XC_STR_MBOOT,miu=0,hid=20,sz=0x3000000,st=0xac00000)
            if(mboot_str_heap_id > 0 )
            {
                if(heap_get_attr(mboot_str_heap_id, &heap_attr)>=0){
                    //here set mstar_driver_boot_ipa_str_pool_num value,
                    //and set ipa_str_pool_config value,
                    //after str pool alloc and str pool free ,next time str pool alloc again,
                    //will no need to set ipa_str_pool_config value again,for it have already been set.
                    mstar_driver_boot_ipa_str_pool_num = 1;
                    ipa_str_pool_config.miu =heap_attr.miu;
                    ipa_str_pool_config.ipa_id =mboot_str_heap_id;
                    ipa_str_pool_config.start = 0;//this special pool should take up all this heap
                    ipa_str_pool_config.size = heap_attr.heap_len;
                    strcpy(ipa_str_pool_config.name,heap_attr.name);
                    ipa_str_pool_para_set_done = true;
                }
                else
                {
                    printk("%s:%d heap_get_attr fail error   mboot_str_heap_id=%d\n",__FUNCTION__,__LINE__,mboot_str_heap_id);
                    return;
                }
            }
            else
            {
                printk("%s:%d  both no str pool related  mstar_driver_boot_ipa_str_pool_num=%d ,mboot_str_heap_id=%d\n",__FUNCTION__,__LINE__,mstar_driver_boot_ipa_str_pool_num,mboot_str_heap_id);

                //for some projects,str may not co-buff with others.In that case ,
                //will and no need to call reserver/release ipa_str_pool,but if this function wrongly called ,
                //for code strong,here directly return.
                return;
            }
        }
        else
        {
             if(heap_get_attr(ipa_str_pool_config.ipa_id, &heap_attr)>=0){
                // CMA2=XXX,miu=0,hid=20,sz=0x3000000,st=0xac00000
                //IPA_STR_POOL=str,miu=0,heap_id=0x14,sz=0x800000,st=0xac00000
                BUG_ON(ipa_str_pool_config.start < heap_attr.miu_offset);
                ipa_str_pool_config.start  -= heap_attr.miu_offset;
                ipa_str_pool_para_set_done = true;
            }
            else
            {
                printk("%s:%d heap_get_attr fail error,ipa_str_pool_config.ipa_id=%d\n",__FUNCTION__,__LINE__,ipa_str_pool_config.ipa_id);
                return;
            }   
        }
    }
    //printk("%s:%d mstar_driver_boot_ipa_str_pool_num=%d  ipa_str_pool_config.ipa_id=%d,start=0x%lx,size=0x%lx\n",__FUNCTION__,__LINE__,mstar_driver_boot_ipa_str_pool_num,ipa_str_pool_config.ipa_id
       //           ,ipa_str_pool_config.start,ipa_str_pool_config.size);

    if(MSTAR_IPA_STR_POOL == NULL)
    {
        if(!IPAPOOL_IS_PGALN(ipa_str_pool_config.start) || !IPAPOOL_IS_PGALN(ipa_str_pool_config.size)){
            IPAPOOLERR("not aligned heap range (%lx,%lx)\n",  ipa_str_pool_config.start, ipa_str_pool_config.size);
            ret=-IPAERROR_RANGE_NOALIGN;
            return;
        }
        mutex_lock(&IPAPoolDev.apilock);
        //for ipa str use,after create this MSTAR_IPA_STR_POOL special pool ,will never destroy.
        printk("[%s:%d] will create pool heap_id=%d,heap_off=0x%x,pool_len=%x,name=%s\n"
                 ,__FUNCTION__,__LINE__,ipa_str_pool_config.ipa_id,ipa_str_pool_config.start,ipa_str_pool_config.size, ipa_str_pool_config.name);//notify reserve process.
        ret=_IPAPool_create_pool(ipa_str_pool_config.ipa_id, 0, ipa_str_pool_config.start, ipa_str_pool_config.size, ipa_str_pool_config.name, &MSTAR_IPA_STR_POOL);
        printk("[%s:%d] after create pool heap_id=%d,heap_off=0x%x,pool_len=%x,name=%s\n"
                 ,__FUNCTION__,__LINE__,ipa_str_pool_config.ipa_id,ipa_str_pool_config.start,ipa_str_pool_config.size, ipa_str_pool_config.name);//notify reserve process.
        mutex_unlock(&IPAPoolDev.apilock);
        if(ret <0)
        {
            printk("%s:%d _IPAPool_create_pool fail error [%d %lu %lu]\n",__FUNCTION__,__LINE__,ipa_str_pool_config.ipa_id, ipa_str_pool_config.start, ipa_str_pool_config.size);    
            return;
        }
    }

    
    printk("[%s:%d] will pool_alloc hid=%d,heap_off=%lu,len=%lu\n"
             ,__FUNCTION__,__LINE__,MSTAR_IPA_STR_POOL->hid,MSTAR_IPA_STR_POOL->heap_off,MSTAR_IPA_STR_POOL->len);//notify reserve process.
    ret = pool_alloc(MSTAR_IPA_STR_POOL,client_filp, 0, MSTAR_IPA_STR_POOL->len,NULL,0);
    printk("[%s:%d] after pool_alloc hid=%d,heap_off=%lu,len=%lu\n"
             ,__FUNCTION__,__LINE__,MSTAR_IPA_STR_POOL->hid,MSTAR_IPA_STR_POOL->heap_off,MSTAR_IPA_STR_POOL->len);//notify reserve process.
    if(ret <0)
    {
        printk("%s:%d pool_alloc fail [%p %p ][%lu %lu]\n",__FUNCTION__,__LINE__,MSTAR_IPA_STR_POOL , client_filp, ipa_str_pool_config.start, ipa_str_pool_config.size);
    }

    return;
}

void str_release_mboot_ipa_str_pool_buffer()
{
    struct file *client_filp = (struct file *)STR_POOL_CLIENT;
    int ret =0;

    //for some projects,str may not co-buff with others.In that case ,
    //will and no need to call reserver/release ipa_str_pool,but if this function wrongly called ,
    //for code strong,here directly return.
    if(false == ipa_str_pool_para_set_done)
        return;

    //if not create this special pool MSTAR_IPA_STR_POOL yet,return.
    if(MSTAR_IPA_STR_POOL == NULL)
        return;

    printk("[%s:%d] will pool_free hid=%d,heap_off=%lu,len=%lu\n"
             ,__FUNCTION__,__LINE__,MSTAR_IPA_STR_POOL->hid,MSTAR_IPA_STR_POOL->heap_off,MSTAR_IPA_STR_POOL->len);//notify reserve process.
    ret =pool_free(MSTAR_IPA_STR_POOL, client_filp, 0, MSTAR_IPA_STR_POOL->len);
    printk("[%s:%d] after pool_free hid=%d,heap_off=%lu,len=%lu\n"
             ,__FUNCTION__,__LINE__,MSTAR_IPA_STR_POOL->hid,MSTAR_IPA_STR_POOL->heap_off,MSTAR_IPA_STR_POOL->len);//notify reserve process.

    if(ret < 0)
        printk("%s:%d pool_free fail [ %p %p][%lu  %lu]\n",__FUNCTION__,__LINE__, MSTAR_IPA_STR_POOL, client_filp,MSTAR_IPA_STR_POOL->heap_off, MSTAR_IPA_STR_POOL->len);

}

static long IPAPoolDev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    unsigned int dir;
    int pool_id;
    unsigned long flags=0;
    unsigned long addr=0;

    union {
        struct IPA_Pool_Init_Args init_para;
        struct IPA_Pool_Deinit_Args deinit_para;
        struct IPA_Pool_Alloc_Args alloc_para;
        struct IPA_Pool_free_Args free_para;
        struct IPA_Pool_Map_Args map_para;
        struct IPA_Pool_Unmap_Args unmap_para;
        struct IPA_Pool_DCacheFlush_Args flush_para;
        struct IPA_Pool_Heap_Attr heap_attr;
        struct IPA_Pool_GetIpcHandle_Args getipchandle_para;
        struct IPA_Pool_InstallIpcHandle_Args instipchandle_para; 
        #ifdef STR_ALLOC_FREE_DEBUG//only debug use
        struct IPA_Pool_STR_Alloc_Args str_alloc_para;
        struct IPA_Pool_STR_free_Args str_free_para;
        #endif
        struct IPA_Pool_Event_Args event_args;
    } data;

    dir = _ipapool_ioctl_dir(cmd);

    if (_IOC_SIZE(cmd) > sizeof(data))
        return -EINVAL;

    if (dir & _IOC_WRITE)
        if (copy_from_user(&data, (void __user *)arg, _IOC_SIZE(cmd)))
            return -EFAULT;

    switch (cmd) {
    #ifdef STR_ALLOC_FREE_DEBUG //only  debug use
    case IPA_POOL_IOC_STR_ALLOC:
        //printk(" will ipa_heap_str_alloc_buffer   %s:%d\n",__FUNCTION__,__LINE__);
        str_reserve_mboot_ipa_str_pool_buffer();
         //printk("after ipa_heap_str_alloc_buffer   %s:%d\n",__FUNCTION__,__LINE__);
        break;
     case IPA_POOL_IOC_STR_FREE:
        // printk("will ipa_heap_str_free_buffer   %s:%d\n",__FUNCTION__,__LINE__);
        str_release_mboot_ipa_str_pool_buffer();
        //printk("after ipa_heap_str_free_buffer   %s:%d\n",__FUNCTION__,__LINE__);
        break;       
    #endif   
    case IPA_POOL_IOC_INIT:
        {
            struct IPA_heap_attr heap_attr;
            char *name=ustr_dup((char*)data.init_para.pool_name,IPAPOOL_NAME_MAX_LONG);
            data.init_para.error_code=-1;
            if(!name){
                break;
            }
            if(heap_get_attr(data.init_para.heap_id, &heap_attr)>=0){
                unsigned long flags=0;
                data.init_para.error_code=IPA_pool_init(data.init_para.heap_id,
                                            flags, data.init_para.offset, data.init_para.len,
                                            name, &pool_id);
                if(data.init_para.error_code<0){
                    IPAPOOLERR("IPA pool %s init fail(%u,"U64X_FMT","U64X_FMT"),ret=%d\n", name,
                        (unsigned int)data.init_para.heap_id,
                        data.init_para.offset,
                        data.init_para.len,
                        (int)data.init_para.error_code);
                }else{
                    data.init_para.pool_handle_id=pool_id;
                    data.init_para.miu=heap_attr.miu;
                    data.init_para.heap_miu_start_offset=heap_attr.miu_offset;
                    data.init_para.heap_length=heap_attr.heap_len;
                    data.init_para.heap_type=heap_attr.type;
                }
            }
            kfree(name);
        }
        break;
    case IPA_POOL_IOC_DEINIT:
        pool_id=data.deinit_para.pool_handle_id;
        if(IPA_pool_deinit(pool_id)<0){
            IPAPOOLERR("IPA pool %d deinit fail\n",pool_id);
        }
        break;
    case IPA_POOL_IOC_ALLOC:
        pool_id=data.alloc_para.pool_handle_id;
        IPAPOOLDEBUG("%s     data.alloc_para.timeout=:%u \n",__FUNCTION__,data.alloc_para.timeout);
        data.alloc_para.error_code=IPA_pool_get_mem(pool_id,
            data.alloc_para.offset_in_pool,data.alloc_para.length,data.alloc_para.timeout);
        if(data.alloc_para.error_code<0){
            IPAPOOLERR("IPA pool %d allcate mem fail("U64X_FMT","U64X_FMT")\n",pool_id,
                data.alloc_para.offset_in_pool,data.alloc_para.length);
        }
        break;
    case IPA_POOL_IOC_FREE:
        pool_id=data.free_para.pool_handle_id;
        if(IPA_pool_put_mem(pool_id,
                data.free_para.offset_in_pool,data.free_para.length)<0){
            IPAPOOLERR("IPA pool %d free mem fail("U64X_FMT","U64X_FMT")\n",pool_id,
                data.free_para.offset_in_pool,data.free_para.length);
        }
        break;
    case IPA_POOL_IOC_MAP:
        pool_id=data.map_para.pool_handle_id;        
        if(data.map_para.map_va_type==IPA_VA_CACHE_WRITE_BACK){
            flags |=ca_type_cached_writeback;
        }else if(data.map_para.map_va_type==IPA_VA_CACHE_WRITE_THROUGH){
            flags |=ca_type_cached_writethrough;
        }
        data.map_para.error_code=IPA_pool_map(pool_id,
            data.map_para.offset_in_pool, data.map_para.length, flags, &addr);
        data.map_para.virt_addr=addr;
        if(data.map_para.error_code<0){
            IPAPOOLERR("IPA pool %d map mem fail("U64X_FMT","U64X_FMT"),type=%d\n",pool_id,
                data.map_para.offset_in_pool,data.map_para.length,
                data.map_para.map_va_type);
        }
        break;
    case IPA_POOL_IOC_UNMAP:
        if(IPA_pool_unmap((unsigned long)data.unmap_para.virt_addr,(unsigned long)data.unmap_para.length)<0){
            IPAPOOLERR("IPA pool unmap mem fail("U64X_FMT","U64X_FMT")\n",
                data.unmap_para.virt_addr,data.unmap_para.length);
        }
        break;
    case IPA_POOL_IOC_FLUSH:
        if(data.flush_para.flush_type==IPA_DCACHE_INVALID){
            flags |=IPAPOOLFLG_INVALID_ONLY;
        }
        if(IPA_pool_dcache_flush(
                data.flush_para.virt_addr,data.flush_para.length,flags)<0){
            IPAPOOLERR("IPA pool flush fail("U64X_FMT","U64X_FMT"),type=%x\n",
                data.flush_para.virt_addr,data.flush_para.length,
                data.flush_para.flush_type);
        }
        break;
    case IPA_POOL_IOC_HEAP_ATTR:
        {
            struct IPA_heap_attr heap_attr;
            data.heap_attr.error_code=-1;            
            if(heap_get_attr(data.heap_attr.heap_id, &heap_attr)>=0){
                data.heap_attr.heap_miu_start_offset = heap_attr.miu_offset;
                data.heap_attr.heap_length = heap_attr.heap_len;
                data.heap_attr.heap_miu_start_offset = heap_attr.miu_offset;
                data.heap_attr.heap_type = heap_attr.type;
                data.heap_attr.miu = heap_attr.miu;
                memcpy(data.heap_attr.name, heap_attr.name, IPAPOOL_HEAP_NAME_MAX_LEN);
                data.heap_attr.error_code = 0;                
            }
        }
        break;
     case IPA_POOL_IOC_GETIPCHANDLE:
        {           
            int ipc_handle = -1;
            
            data.getipchandle_para.error_code = -1; 
            ipc_handle = IPA_pool_getipchandle( data.getipchandle_para.pool_handle_id);

            if(ipc_handle >= 0)
            {
                data.getipchandle_para.ipc_handle_id = ipc_handle;
                data.getipchandle_para.error_code = 0;
            }
        }
        break;
     case IPA_POOL_IOC_INSTALLIPCHANDLE:
        {
             int fd = -1;

             data.instipchandle_para.error_code = -1;
             fd = IPA_pool_file_install(data.instipchandle_para.ipc_handle_id);
             if(fd > 0) 
             {
                data.instipchandle_para.pool_handle_id = fd;
                data.instipchandle_para.error_code = 0;
             }
        }
        break;
     case IPA_POOL_IOC_POLL:
     {
        IPA_pool_poll_client(&data.event_args);
        break;
     }
     default:
        IPAPOOLERR("unknow command\n");
        return -ENOTTY;
    }

    if (dir & _IOC_READ) {
        if (copy_to_user((void __user *)arg, &data, _IOC_SIZE(cmd))) {
            IPAPOOLDEBUG("%s:%d\n",__FUNCTION__,__LINE__);
            return -EFAULT;
        }
    }
    return 0;
}
#if defined(CONFIG_COMPAT)
static long Compat_IPAPool_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    return -EINVAL;
}
static long Compat_IPAPoolDev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    return IPAPoolDev_ioctl(filp,cmd,arg);
}
#endif

//[STR]
static struct platform_device ipapool_dev = {
    .name   = IPAPOOL_MOD_NAME,
    .id     = 0,
};

static int IPAPool_Suspend(struct device *dev)
{    
    heap_suspend();
    return 0;
}

static int IPAPool_Resume(struct device *dev)
{    
    heap_resume();
    return 0;
}

static int IPAPool_Freeze(struct device *dev)
{
    return 0;
}

static int IPAPool_Thaw(struct device *dev)
{
    return 0;
}

static int IPAPool_Restore(struct device *dev)
{
    return 0;
}

static int IPAPool_Probe(struct platform_device *pdev)
{
    pdev->dev.platform_data = NULL;
    return 0;
}

static int IPAPool_Remove(struct platform_device *pdev)
{
    pdev->dev.platform_data = NULL;
    return 0;
}

static const struct dev_pm_ops ipa_pool_pm_ops =
{
    .suspend = IPAPool_Suspend,
    .resume  = IPAPool_Resume,
    .freeze  = IPAPool_Freeze,
    .thaw    = IPAPool_Thaw,
    .restore = IPAPool_Restore,
};

static struct platform_driver Mstar_ipapool_driver = {
    .probe   = IPAPool_Probe,
    .remove  = IPAPool_Remove,

    .driver = {
        .name   = IPAPOOL_MOD_NAME,
        .owner  = THIS_MODULE,
        .pm     = &ipa_pool_pm_ops,
    }
};

static int _IPA_pool_device_init(void)
{
    int ret;
    dev_t dev;
    mutex_init(&IPAPoolDev.apilock);
    mutex_init(&IPAPoolDev.lock);
    INIT_LIST_HEAD(&IPAPoolDev.pool);
    IPAPoolDev.debug_root =_IPAPool_create_dbgdir("IPA_Pool", NULL);
    idr_init(&IPAPoolDev.idr_poolid);
    idr_init(&IPAPoolDev.idr_ipc_pool_handle);

    IPAPoolDev.ipapool_class=class_create(THIS_MODULE, IPAPOOL_MOD_NAME);
    if (IS_ERR(IPAPoolDev.ipapool_class)){
        ret=PTR_ERR(IPAPoolDev.ipapool_class);
        goto fail_exit;
    }

    if(IPAPoolDev.major){
        dev = MKDEV(IPAPoolDev.major, IPAPoolDev.minor);
        ret = register_chrdev_region(dev, 1, IPAPOOL_MOD_NAME);
    }else{
        ret = alloc_chrdev_region(&dev,IPAPoolDev.minor, 1, IPAPOOL_MOD_NAME);
        IPAPoolDev.major=MAJOR(dev);
    }

    if (ret<0){
        printk(KERN_ERR "Unable to get major %d\n",IPAPoolDev.major);
        goto destroy_class;
    }

    cdev_init(&IPAPoolDev.dev, &IPAPoolDev.fop);
    if(0!= (ret= cdev_add(&IPAPoolDev.dev, dev, 1))){
        printk(KERN_ERR "Unable add a character device\n");
        goto unreg_dev_region;
    }
    IPAPoolDev.ipapool_dev=device_create(IPAPoolDev.ipapool_class, NULL, dev, NULL, IPAPOOL_MOD_NAME);
    if(IS_ERR(IPAPoolDev.ipapool_dev)){
        ret=PTR_ERR(IPAPoolDev.ipapool_dev);
        goto del_device;
    }

    platform_device_register(&ipapool_dev);
    platform_driver_register(&Mstar_ipapool_driver);
    return 0;
del_device:
    cdev_del(&IPAPoolDev.dev);
unreg_dev_region:
    unregister_chrdev_region(dev, 1);
destroy_class:
    class_destroy(IPAPoolDev.ipapool_class);
fail_exit:
    mutex_destroy(&IPAPoolDev.lock);
    mutex_destroy(&IPAPoolDev.apilock);
    if(IPAPoolDev.debug_root)
        debugfs_remove_recursive(IPAPoolDev.debug_root);
    idr_destroy(&IPAPoolDev.idr_poolid);
    return ret;
}
static int __init IPAPool_module_init(void)
{
    IPAHeap_init();
    return _IPA_pool_device_init();
}
static void __exit IPAPool_module_exit(void)
{
    BUG();
}
module_init(IPAPool_module_init);
module_exit(IPAPool_module_exit);

