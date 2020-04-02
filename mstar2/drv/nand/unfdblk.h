#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h> /* printk() */
#include <linux/fs.h>     /* everything... */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/vmalloc.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/hdreg.h>

#include <linux/bitops.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/scatterlist.h>
#include <linux/dma-mapping.h>
#include <asm/errno.h>
#include <asm/uaccess.h>
#include "drvNAND.h"
#include "drvNAND_ftl.h"

#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/version.h>
#include <linux/kthread.h>

/*
Each partition has a device for 
*/
struct unfdblk_queue{
	struct task_struct	*thread;
	struct semaphore	thread_sem;
	struct request_queue	*queue;
};

typedef struct _unfdblk_dev
{
	uint32_t u32_StartSector;
	uint32_t u32_SectorCount;
	struct gendisk *gd;
	struct list_head list;
//	struct request_queue *queue;
	struct scatterlist *sg;
	struct unfdblk_queue uq;
	int sg_len;
	struct device *dev;
	spinlock_t lock;
}unfdblk_dev_t;

struct mstar_nand_info{
	struct mtd_info nand_mtd;
	struct platform_device *pdev;
	struct nand_chip	nand;
	struct mtd_partition	*parts;
};

extern struct mstar_nand_info *info;
