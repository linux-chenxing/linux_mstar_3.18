#include "unfdblk.h"

#if (defined(__VER_UNFD_FTL__) && __VER_UNFD_FTL__) && defined(CONFIG_MSTAR_UNFD_BLK)

#define DEVICE_NAME     "unfdblk"
#define MAJOR_NUMBER    210

#define USING_MTD_COMLINE_PARTITION     0

#define BGTHREAD_HANDLE_REQUEST 1

#define SCATTERLIST_SUPPORT     1

#if defined(SCATTERLIST_SUPPORT) && SCATTERLIST_SUPPORT
#define UNFD_MAX_SG_SUPPORT     128
#else
#define UNFD_MAX_SG_SUPPORT     1
#endif

extern struct semaphore                 PfModeSem;

extern void nand_lock_fcie(void);
extern void nand_unlock_fcie(void);

static LIST_HEAD(unfdblk_devices);
static LIST_HEAD(unfdblk_ctrl_devices);
static DEFINE_MUTEX(unfdblk_devices_mutex);
struct class *unfdblk_class;

static int unfdblk_getgeo(struct block_device *dev, struct hd_geometry *geo)
{
    geo->cylinders = get_capacity(dev->bd_disk) / (4 * 16);
    geo->heads = 4;
    geo->sectors = 16;
    return 0;
}

static int unfdblk_ioctl(struct block_device *dev, fmode_t file,unsigned cmd, unsigned long arg)
{
    return 0;
}

static struct block_device_operations unfdblk_fops ={
    .owner      =   THIS_MODULE,
    .ioctl      = unfdblk_ioctl,
    .getgeo     = unfdblk_getgeo,
};
static int unfdblk_transfer(unfdblk_dev_t *dev, struct request *req)
{
    uint8_t *pu8_Buffer;
    uint32_t u32_SectIdx, u32_SectCnt;
    uint32_t u32_TransferedSectCnt = 0;

    #if defined(SCATTERLIST_SUPPORT) && SCATTERLIST_SUPPORT
    uint32_t u32_SgCnt;
    struct scatterlist *pSg;
    #endif
    int ret = 0, i;
    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
    u32_SectIdx = blk_rq_pos(req);
    u32_SectCnt = blk_rq_cur_sectors(req);
    #else
    u32_SectIdx = req->sector;
    u32_SectCnt = req->current_nr_sectors;
    #endif
    pu8_Buffer = req->buffer;
    u32_TransferedSectCnt = 0;

    nand_lock_fcie();
    
    #if defined(SCATTERLIST_SUPPORT) && SCATTERLIST_SUPPORT

    u32_SgCnt = dma_map_sg(dev->dev, dev->sg, dev->sg_len, (rq_data_dir(req) == READ) ? DMA_FROM_DEVICE : DMA_TO_DEVICE);

    pSg = &(dev->sg[0]);
    for(i = 0; i < u32_SgCnt; i ++)
    {
        pu8_Buffer = sg_virt(pSg);
        u32_SectCnt = sg_dma_len(pSg) >> 9;

        switch(rq_data_dir(req))
        {
            case READ:
            //  printk("Read Sectors from %lX length %lX, buffer 0x%X ++\n", dev->u32_StartSector + u32_SectIdx, u32_SectCnt, pu8_Buffer);
                ret = nand_ReadFlow(dev->u32_StartSector + u32_SectIdx, u32_SectCnt, (uintptr_t)pu8_Buffer);
                break;
            case WRITE:
            //  printk("Write Sectors from %lX length %lX, buffer 0x%X ++\n", dev->u32_StartSector + u32_SectIdx, u32_SectCnt, pu8_Buffer);
                ret = nand_WriteFlow(dev->u32_StartSector + u32_SectIdx, u32_SectCnt, (uintptr_t)pu8_Buffer);
                break;
            default:
                printk("[%s] Unknown request 0x%X\n", __func__, rq_data_dir(req));
                goto ERRUNMAPSG;
        }

        if(ret != UNFD_ST_SUCCESS)
        {
            printk("[%s]transfer error 0x%X\n", __func__, ret);
            goto ERRUNMAPSG;
        }

        u32_TransferedSectCnt += u32_SectCnt;
        u32_SectIdx += u32_SectCnt;
        pSg = sg_next(pSg);
    }
    /*
    * Reason for Using BIDIRECTIONAL is that Using DMA_FROM_DEVICE would lead dma data fail when sector count  == 1
    */
    dma_unmap_sg(dev->dev, dev->sg, dev->sg_len, (rq_data_dir(req) == READ) ? DMA_FROM_DEVICE : DMA_TO_DEVICE);
    #else
    switch(rq_data_dir(req))
    {
        case READ:
    //      printk("Read Sectors from %lX length %lX, buffer 0x%X\n",dev->u32_StartSector + u32_SectIdx, u32_SectCnt, pu8_Buffer);
            ret = nand_ReadFlow(dev->u32_StartSector + u32_SectIdx, u32_SectCnt, (uintptr_t)pu8_Buffer);
            break;
        case WRITE:
    //      printk("Write Sectors from %lX length %lX, buffer 0x%X\n", dev->u32_StartSector + u32_SectIdx, u32_SectCnt, pu8_Buffer);
            ret = nand_WriteFlow(dev->u32_StartSector + u32_SectIdx, u32_SectCnt, (uintptr_t)pu8_Buffer);
            break;
        default:
            printk("[%s] Unknown request 0x%X\n", __func__, rq_data_dir(req));
            goto ERRINVAL;
    }
    if(ret != UNFD_ST_SUCCESS)
    {
        printk("[%s]transfer error 0x%X\n", __func__, ret);
        goto ERRINVAL;
    }

    u32_TransferedSectCnt += u32_SectCnt;
    #endif

    //printk("transfer sector %lX\n", u32_TransferedSectCnt);
    nand_unlock_fcie();

    
    spin_lock_irq(&dev->lock);      //must call queue lock before issue blk_end_request
    __blk_end_request(req, (ret!= 0), u32_TransferedSectCnt << 9);
    spin_unlock_irq(&dev->lock);

    //printk("transfer sector %lX\n", u32_TransferedSectCnt);

    return 0;
#if defined(SCATTERLIST_SUPPORT) && SCATTERLIST_SUPPORT
ERRUNMAPSG:
    dma_unmap_sg(dev->dev, dev->sg, dev->sg_len, DMA_BIDIRECTIONAL);
#else
ERRINVAL:
#endif
    nand_unlock_fcie();
    spin_lock_irq(&dev->lock);
    __blk_end_request(req, (ret!= 0), u32_TransferedSectCnt << 9);
    spin_unlock_irq(&dev->lock);

    return -EINVAL;
}

static void unfdblk_request(struct request_queue *rq)
{
    unfdblk_dev_t *dev = rq->queuedata;
    #if defined(BGTHREAD_HANDLE_REQUEST) && BGTHREAD_HANDLE_REQUEST
    wake_up_process(dev->uq.thread);
    #else
    struct request *req;
    while((req = elv_next_request(rq)) != NULL)
    {
        if(!blk_fs_request(req))
        {
            blk_end_request(req, 1, req->nr_sectors << 9);
            continue;
        }

        #if defined(SCATTERLIST_SUPPORT) && SCATTERLIST_SUPPORT
        dev->sg_len = blk_rq_map_sg(rq, req, dev->sg);
        #endif

        unfdblk_transfer(dev, req);
    }
    #endif
}

#if defined(BGTHREAD_HANDLE_REQUEST) && BGTHREAD_HANDLE_REQUEST

static int unfdblk_queue_thread(void *d)
{
    unfdblk_dev_t *dev = d;
    struct request_queue *q = dev->uq.queue;

    current->flags |= PF_MEMALLOC;

    down(&dev->uq.thread_sem);
    do{
        struct request *req = NULL;
    
        spin_lock_irq(q->queue_lock);
        set_current_state(TASK_INTERRUPTIBLE);
        #if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
        req = blk_fetch_request(q);
        #else
        req = elv_next_request(q);
        #endif
        spin_unlock_irq(q->queue_lock);
        
        if (req != NULL) {

            set_current_state(TASK_RUNNING);
            #if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,36)
            if(!blk_fs_request(req))
            {
                spin_lock_irq(&dev->lock);
                __blk_end_request(req, 1, req->nr_sectors << 9);
                spin_unlock_irq(&dev->lock);
            }
            else
            #endif
            {
                #if defined(SCATTERLIST_SUPPORT) && SCATTERLIST_SUPPORT
                dev->sg_len = blk_rq_map_sg(q, req, dev->sg);
                #endif
                unfdblk_transfer(dev, req);
            }
        } else {
            if (kthread_should_stop()) {
                set_current_state(TASK_RUNNING);
                break;
            }
            up(&dev->uq.thread_sem);
            schedule();
            down(&dev->uq.thread_sem);
        }
    }while(1);
    up(&dev->uq.thread_sem);
    return 0;
}
#endif

static void unfdblk_delete_disk(unfdblk_dev_t *dev)
{
    if(dev->gd)
    {
        del_gendisk(dev->gd);
        put_disk(dev->gd);
    }

    if(dev->uq.queue)
        blk_cleanup_queue(dev->uq.queue);

    kfree(dev);
}

static int unfdblk_create_disk(int part_num, DISK_INFO_t DiskInfo_t)
{
    unfdblk_dev_t *dev;
    uint32_t u32_TotalSectCnt;

    dev = kzalloc(sizeof(unfdblk_dev_t), GFP_KERNEL);
    if(!dev)
    {
        printk(KERN_CRIT"[%s] unfdblk_dev malloc fail\n", __func__);
        return -ENOMEM;
    }
    spin_lock_init(&dev->lock);
    INIT_LIST_HEAD(&dev->list);

    /*
        get partition info here
    */
    dev->u32_StartSector = DiskInfo_t.u32_StartSector;  
    dev->u32_SectorCount = u32_TotalSectCnt = DiskInfo_t.u32_SectorCnt;
    
    list_add_tail(&dev->list, &unfdblk_devices);
    //initial request queue
    dev->uq.queue = blk_init_queue(unfdblk_request, &dev->lock);
    dev->uq.queue->queuedata = dev;

    blk_queue_bounce_limit(dev->uq.queue, BLK_BOUNCE_HIGH);
    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
//  blk_queue_logical_block_size(dev->uq.queue, 2048);      //if block size is set to 2048, parameter "-b" of mkfs.ext2 should be set to 2048 
//  blk_queue_physical_block_size(dev->uq.queue, 2048);     //better write performance but poorer read performance
//  blk_queue_io_min(dev->uq.queue, 2048);
    blk_queue_max_hw_sectors(dev->uq.queue, 128);
    blk_queue_max_segments(dev->uq.queue, UNFD_MAX_SG_SUPPORT);
    blk_queue_max_segment_size(dev->uq.queue, UNFD_MAX_SG_SUPPORT << 9);
    #else
    //hardsect size:    set for minimum size that upper layer passby; can be set as 512, 1024, 2048
    blk_queue_hardsect_size(dev->uq.queue, 512);
    //max_sectors <-- unit is 512 Byte
    blk_queue_max_sectors(dev->uq.queue, 128);
    blk_queue_max_phys_segments(dev->uq.queue, UNFD_MAX_SG_SUPPORT);
    blk_queue_max_hw_segments(dev->uq.queue, UNFD_MAX_SG_SUPPORT);
    #endif

    /*allocate scatterlist*/

    dev->sg = kmalloc(sizeof(struct scatterlist) * UNFD_MAX_SG_SUPPORT, GFP_KERNEL);
    if(!dev->sg)
    {
        list_del(&dev->list);
        blk_cleanup_queue(dev->uq.queue);
        kfree(dev);
        printk(KERN_CRIT"[%s] scatterlist malloc fail\n",__func__);
        return -ENOMEM;
    }

    sg_init_table(dev->sg, UNFD_MAX_SG_SUPPORT);

    #if defined(BGTHREAD_HANDLE_REQUEST) && BGTHREAD_HANDLE_REQUEST
    sema_init(&dev->uq.thread_sem, 1);

    dev->uq.thread = kthread_run(unfdblk_queue_thread, dev, "unfdblkqd/%d",part_num);
    if (IS_ERR(dev->uq.thread)) {
        kfree(dev->sg);
        list_del(&dev->list);
        blk_cleanup_queue(dev->uq.queue);
        kfree(dev);
        return PTR_ERR(dev->uq.thread);
    }
    #endif

    //initial gendisk
    dev->gd = alloc_disk(1);
    if(!dev->gd)
    {
        kfree(dev->sg);
        list_del(&dev->list);
        blk_cleanup_queue(dev->uq.queue);
        kfree(dev);
        printk(KERN_CRIT"[%s] gendisk malloc fail\n",__func__);
        return -ENOMEM;
    }

    dev->gd->major = MAJOR_NUMBER;
    dev->gd->first_minor = part_num * 128;
    dev->gd->fops = &unfdblk_fops;
    dev->gd->queue = dev->uq.queue;
    dev->gd->private_data = dev;
    dev->gd->minors = 128;                  //max partition is 128
    dev->gd->flags = GENHD_FL_EXT_DEVT;     //allow partitions

    sprintf(dev->gd->disk_name,"unfdblk%d", part_num);

    set_capacity(dev->gd, u32_TotalSectCnt);
    
    add_disk(dev->gd);

    device_create(unfdblk_class, NULL, MKDEV(MAJOR_NUMBER, part_num), NULL, dev->gd->disk_name);

    return 0;
}

#if (defined(CONFIG_MTD_CMDLINE_PARTS) && (defined(USING_MTD_COMLINE_PARTITION) && USING_MTD_COMLINE_PARTITION))
extern int parse_cmdline_partitions(struct mtd_info *master, struct mtd_partition **pparts, char *);
#endif

#if defined(__VER_UNFD_FTL__) && __VER_UNFD_FTL__
static uint8_t gu8_ftl_ready = 0;
U32 drvNAND_ChkRdy(U32 u32_PartNo)
{
    return gu8_ftl_ready;
}
#endif


static int unfdblk_blkdev_create(void)
{
    int iRet = 0;
    NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();

    printk("Mstar unfdblk:");

    #if (defined(CONFIG_MTD_CMDLINE_PARTS) && (defined(USING_MTD_COMLINE_PARTITION) && USING_MTD_COMLINE_PARTITION))
    {
        int i;
        int logicalidx = 0;
        int mtd_parts_nb = 0;
        struct mtd_partition *mtd_parts = 0;
        struct mtd_partition *part = 0;
        struct mtd_info* nand_mtd = &info->nand_mtd;

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,1)
        mtd_parts_nb = parse_cmdline_partitions(nand_mtd, &mtd_parts, NULL);
        #else
        mtd_parts_nb = parse_cmdline_partitions(nand_mtd, &mtd_parts, "nand");
        #endif

        if(mtd_parts_nb)
        {
            //ftl initial
            /*
            down(&PfModeSem);
            iRet = nand_Init_FTL(); 
            up(&PfModeSem);
            if(iRet != UNFD_ST_SUCCESS)
            {

                printk("[%s]\t UNFD FTL initial fail: 0x%X\n", iRet);
                goto end;
            }
            */
        }
        printk("\n[%s] parts number %d\n", __func__, mtd_parts_nb);

        for(i = 0; i < mtd_parts_nb; i ++)
        {
            part = mtd_parts + i;
            
            //check partition name with prefix, "UNFDBLK" or "unfdblk"
            if(strncmp(part->name, "UNFDBLK", 7) == 0 || strncmp(part->name, "unfdblk", 7) == 0)
            {
                printk("unfdblk partition %s is found\n", part->name);
                //unfdblk_partition_create_disk(logicalidx,  pNandDrv->DiskInfo_t[0]);
                logicalidx ++;
            }
            
        }
    }
    #else
    //using pni ????
    if(pNandDrv->u8_HasPNI)
    {
        nand_lock_fcie();
        gu8_ftl_ready = 0;
        iRet = nand_Init_FTL();
        if(iRet != UNFD_ST_SUCCESS)
        {
            iRet = nand_Low_Level_Format();
            //memset 0 to ftl parameter
            memset(pNandDrv->DiskInfo_t, 0, sizeof(NAND_DRIVER) - ((U32)(pNandDrv->DiskInfo_t) - (U32)(pNandDrv)));
            iRet = nand_Init_FTL();
            if(iRet != UNFD_ST_SUCCESS)
            {
                nand_unlock_fcie();
                printk("[%s]\t UNFD FTL initial fail: 0x%X\n", __func__, iRet);
                goto end;
            }
        }
        
        iRet = drvNAND_TaskBGT_Init();
        if(iRet != UNFD_ST_SUCCESS)
        {
            nand_unlock_fcie();
            goto end;
        }
        gu8_ftl_ready = 1;
        nand_unlock_fcie();

        unfdblk_create_disk(0, pNandDrv->DiskInfo_t[0]);        //how to take other logical partition??? or just one logical and partitioned by emmc partition descriptor?
        
    }
    else
        printk("\nNo Partition infomation found in nand flash, abort initialization of unfdblk\n");

    #endif


end:
    printk("\n");
    return iRet;
}

static void unfdblk_blkdev_free(void)
{
    unfdblk_dev_t *dev;
    struct list_head *this, *next;

    list_for_each_safe(this, next, &unfdblk_devices) {
        dev = list_entry(this, unfdblk_dev_t, list);
        unfdblk_delete_disk(dev);
        list_del(&dev->list);
    }
}


static int __init mstar_unfdblk_init(void)
{
    if(register_blkdev(MAJOR_NUMBER, DEVICE_NAME))
    {
        printk(KERN_CRIT"[%s] unable to get major %d\n", __func__, MAJOR_NUMBER);
        return -EAGAIN;
    }

    unfdblk_class = class_create(THIS_MODULE, "unfdblk_device");
    if(IS_ERR(unfdblk_class))
    {
        printk(KERN_CRIT"[%s] create unfdblk class fail\n", __func__);
        return -EAGAIN;
    }

    if(unfdblk_blkdev_create())
    {
        printk(KERN_CRIT"[%s] create disk fail\n", __func__);
        unregister_blkdev(MAJOR_NUMBER, DEVICE_NAME);
        return -EINVAL;
    }
    return 0;
}

static void __exit mstar_unfdblk_exit(void)
{
    unfdblk_blkdev_free();
    unregister_blkdev(MAJOR_NUMBER, DEVICE_NAME);
}

//module_init(mstar_unfd_block_init);
late_initcall(mstar_unfdblk_init);
module_exit(mstar_unfdblk_exit);

MODULE_LICENSE("MSTAR");
MODULE_AUTHOR("mstarsemi");
MODULE_DESCRIPTION("The block device interface for mstar unfd");

#endif
