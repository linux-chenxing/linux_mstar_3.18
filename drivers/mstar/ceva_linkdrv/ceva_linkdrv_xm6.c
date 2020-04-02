/*
 * ceva_linkdrv_xm6.c
 *
 * Created on: Sep 12, 2013
 * Author: Ido Reis <ido.reis@tandemg.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59
 * Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/dma-mapping.h>
#include <asm/page.h>

#include "ceva_linkdrv-generic.h"
#include "protected_mem_db.h"
#include "ceva_linkdrv.h"
#include "ceva_linkdrv_xm6.h"
#include "drv_debug.h"

#include "pci_acl.h"

#include "hal_clk.c"
#include "boot_loader.h"


#define XM6_DRV_DEVICE_COUNT    (1) // How many device will be installed
#define XM6_DRV_NAME            "mstar_ive"
#define XM6_DRV_MINOR           (0)
#define XM6_DRV_CLASS_NAME      "mstar_ive_class"

#define DSP_CODE_SIZE           (5*1024*1024)
#define E_EMAC_ID_CEVA_ADR      (0x5FB00000)


MODULE_AUTHOR("Ido Reis <ido.reis@tandemg.com>");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0.1");
MODULE_DESCRIPTION("mapped pci character device driver");

//#define FUNC_SHOULD_REMOVE

static int xm6_drv_open(struct inode *inode, struct file *filp);
static int xm6_drv_release(struct inode *inode, struct file *filp);
static ssize_t xm6_drv_read(struct file *filp, char __user *buf, size_t count, loff_t *pos);
static ssize_t xm6_drv_write(struct file *filp, const char __user *buf, size_t count, loff_t *pos);
static long xm6_drv_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
static void xm6_drv_dma_vma_open(struct vm_area_struct *vma);
static void xm6_drv_dma_vma_close(struct vm_area_struct *vma);
static int xm6_drv_mmap_dma(struct xm6_dev_data *dev_data, struct vm_area_struct *vma);
static int xm6_drv_mmap(struct file *filp, struct vm_area_struct *vma);
static void xm6_drv_cleanup_module(struct platform_device *plat_dev, struct xm6_dev_data *dev_data);
static int xm6_drv_setup_cdev(struct platform_device *plat_dev, struct xm6_dev_data *dev_data);
static int ceva_linkdrv_init_module(struct platform_device *plat_dev, struct xm6_dev_data *dev_data);
//static u8 xm6_drv_get_revision(struct platform_device *plat_dev);
//static u8 xm6_drv_get_irq(struct platform_device *plat_dev);
//static struct platform_device* find_upstream_dev(struct platform_device *plat_dev);
//static void retrain_gen2(struct platform_device *plat_dev, struct xm6_dev_data *dev_data);
//static int probe_enable_pci_dev(struct platform_device *plat_dev);
//static void remove_disable_pci_dev(struct platform_device *plat_dev);
static int probe_get_hw_prop(struct platform_device* plat_dev, struct xm6_dev_data *dev_data);
static int ceva_link_generate_irq(struct xm6_dev_data *dev_data, ceva_event_t e);
//static int scan_bars(struct platform_device *plat_dev, struct xm6_dev_data *dev_data);
//static int map_bars(struct platform_device *plat_dev, struct xm6_dev_data *dev_data);
//static void free_bars(struct platform_device *plat_dev, struct xm6_dev_data *dev_data);
//static int probe_map_bars(struct platform_device *plat_dev, struct xm6_dev_data *dev_data);
static int probe_map_dma(struct xm6_dev_data *dev_data);
#if defined(FUNC_SHOULD_REMOVE)
static int probe_enable_msi(struct platform_device *plat_dev, struct xm6_dev_data *dev_data);
#endif
static irqreturn_t ceva_linkdrv_interrupt(int irq, void *dev_id);
static int probe_init_irq(struct platform_device *plat_dev, struct xm6_dev_data *dev_data);
static void remove_deinit_irq(struct platform_device *plat_dev, struct xm6_dev_data *dev_data);
static int xm6_drv_probe(struct platform_device *plat_dev);
static s32 xm6_drv_remove(struct platform_device *plat_dev);
int __init xm6_drv_init_module(void);
void __exit xm6_drv_exit_module(void);

#if 0
static int xm6_drv_major;
static struct class *xm6_drv_class = NULL;
#endif

/*!
 * ops for dma mapped memory
 */
static struct vm_operations_struct xm6_drv_dma_vma_ops = {
        .open = xm6_drv_dma_vma_open,
        .close = xm6_drv_dma_vma_close
};

/*!
 * fops for this driver's device
 */
static struct file_operations xm6_drv_fops = {
        .owner = THIS_MODULE,
        .read = xm6_drv_read,
        .write = xm6_drv_write,
        .open = xm6_drv_open,
        .release = xm6_drv_release,
        .unlocked_ioctl = xm6_drv_ioctl,
        .llseek = no_llseek, /* This driver doesn't support llseek */
        .mmap = xm6_drv_mmap,
};

/*!
 * xm6 driver module fops
 */
static const struct of_device_id xm6_drv_match[] = {
    {
        .compatible = "mstar,infinity2-xm6",
        /*.data = NULL,*/
    },
    {},
};

static struct platform_driver xm6_drv_driver = {
    .probe      = xm6_drv_probe,
    .remove     = xm6_drv_remove,
    // .suspend    = xm6_drv_suspend,
    // .resume     = xm6_drv_resume,

    .driver = {
        .of_match_table = of_match_ptr(xm6_drv_match),
        .name   = "mstar_xm6",
        .owner  = THIS_MODULE,
    }
};

/* Use a struct to gather all global variable when registering a char device*/
static struct
{
    int major;              // cdev major number
    int minor_star;         // begining of cdev minor number
    int reg_count;          // registered count
    struct class *class;    // class pointer
} xm6_drv_data = {0, 0, 0, NULL};

/*!
 * open handler
 *
 * called when user opens the device.
 * each open call increment the ref_count variable.
 * when opened for the first time (e.g ref_count = 1) it initializes the
 * ceva_linkdrv logic module.
 * for each call, it also executes the ceva_linkdrv_open_cb to notify the
 * logical module of the user operation.
 * @param [in] inode
 * @param [in] filp
 * @return zero for success, nonzero for failure
 * @see ceva_linkdrv_init(), ceva_linkdrv_open_cb()
 */
static int xm6_drv_open(struct inode *inode, struct file *filp) {
    int ret = 0;
#if 0
    unsigned int mj = imajor(inode);
    unsigned int mn = iminor(inode);
#endif
    struct xm6_dev_data *dev_data = NULL;

#if 0
    if (mj != xm6_drv_major || mn < 0 ||
            mn >= CEVA_PCIDEV_NUMBER_OF_DEVICES) {
        XM6_MSG(XM6_MSG_DBG, "No device found with minor=%d and major=%d",
                mj, mn);
        ret = -ENODEV; /* No such device */
        goto exit;
    }
#endif

    XM6_MSG(XM6_MSG_DBG, "open device %p\n", inode);

    /* store a pointer to struct cfake_dev here for other methods */
    dev_data = container_of(inode->i_cdev, struct xm6_dev_data, cdev);
    filp->private_data = dev_data;

    if (inode->i_cdev != &dev_data->cdev) {
        XM6_MSG(XM6_MSG_WRN, "open: internal error");
        ret = -ENODEV; /* No such device */
        goto exit;
    }
    /* This method can't fail, so no need to check for return value */
    nonseekable_open(inode, filp);

    if (mutex_lock_interruptible(&dev_data->dev_mutex)) {
        return -EINTR;
    }

    if (dev_data->count == 0) {
        ret = ceva_linkdrv_init(&dev_data->linkdrv, dev_data);
    }

    if (!ret) {
        ret = ceva_linkdrv_open_cb(&dev_data->linkdrv);
        if (!ret) {
            dev_data->count++;
        } else {
            if (dev_data->count == 0) {
                ceva_linkdrv_deinit(&dev_data->linkdrv);
            }
        }
    }

    mutex_unlock(&dev_data->dev_mutex);

exit:
    return ret;
}

/*!
 * close handler
 *
 * called when user closes the device.
 * each open call decrement the ref_count variable.
 * for each call, it executes the ceva_linkdrv_release_cb to notify the
 * logical module of the user operation.
 * when closed for the last time (e.g ref_count = 0) it deinitializes the
 * ceva_linkdrv logic module.
 * @param [in] inode
 * @param [in] filp
 * @return zero for success, nonzero for failure
 * @see ceva_linkdrv_deinit(), ceva_linkdrv_release_cb()
 */
static int xm6_drv_release(struct inode *inode, struct file *filp) {
    int ret;
    struct xm6_dev_data *dev_data = (struct xm6_dev_data *) filp->private_data;

    if (mutex_lock_interruptible(&dev_data->dev_mutex)) {
        return -EINTR;
    }
    ret = ceva_linkdrv_release_cb(&dev_data->linkdrv);
    if (dev_data->count)
        dev_data->count--;
    if (!dev_data->count) {
        ceva_linkdrv_deinit(&dev_data->linkdrv);
    }
    mutex_unlock(&dev_data->dev_mutex);

    return ret;
}

/*!
 * read handler
 *
 * reads data from bar0 of the pci device, from a relative offset.
 * as the user interface does not include an offset argument, the user
 * has to write the offset value into the buf. the driver first read from
 * buf the offset value and user actual buffer address, and then reads
 * the data into the forwarded user supplied address.
 * @param [in] filp device handler
 * @param [in/out] buf output buffer to read data to
 * @param [in] count number of bytes to read
 * @param [in] pos offset (not used)
 * @return number of bytes read from bar0, negative values for errors
 * @see struct RWBuffer, xm6_drv_write()
 */
static ssize_t xm6_drv_read(struct file *filp, char __user *buf,
        size_t count, loff_t *pos) {
    struct xm6_dev_data *dev_data = (struct xm6_dev_data *) filp->private_data;
    struct RWBuffer rwbuf;
    ssize_t ret = 0;

    if (!count) {
        return 0;
    }

    if (!access_ok(VERIFY_WRITE, buf, count)) {
        XM6_MSG(XM6_MSG_ERR, "access error");
        return -EFAULT;
    }

    if (mutex_lock_interruptible(&dev_data->dev_mutex)) {
        XM6_MSG(XM6_MSG_ERR, "unable to lock mutex");
        return -EINTR;
    }

    // read offset from buf
    if (copy_from_user(&rwbuf, buf, sizeof(rwbuf)) != 0) {
        XM6_MSG(XM6_MSG_ERR, "could not read from user buffer");
        ret = -EFAULT;
        goto out;
    }

#if 0 // ORIGINAL_PCI_BAR
    // read content of bar 0 to buf
    if (copy_to_user(rwbuf.buf,
            ((char *)dev_data->bar[0]) + rwbuf.offset, count) != 0) {
        XM6_MSG(XM6_MSG_ERR, "%s -> copy to user failed", __FUNCTION__);
        ret = -EFAULT;
        goto out;
    }
#else
    XM6_MSG(XM6_MSG_ERR, "copy_to_user bar[0], offset %lu, size %d\n", rwbuf.offset, count);
#endif
    *pos += ret;
    ret = count;
out:
    mutex_unlock(&dev_data->dev_mutex);
    return ret;
}

/*!
 * write handler
 *
 * writes data to bar0 of the pci device, to a relative offset.
 * as the user interface does not include an offset argument, the user
 * has to write the offset value into the buf. the driver first read from
 * buf the offset value and user actual buffer address, and then writes
 * the data from the user buffer to bar0 base address plus the offset.
 * @param [in] filp device handler
 * @param [in] buf output buffer to read data to
 * @param [in] count number of bytes to read
 * @param [in] pos offset (not used)
 * @return number of bytes read from bar0, negative values for errors
 * @see struct RWBuffer, xm6_drv_read()
 */
static ssize_t xm6_drv_write(struct file *filp, const char __user *buf,
        size_t count, loff_t *pos) {
    struct xm6_dev_data *dev_data = (struct xm6_dev_data *) filp->private_data;
    struct RWBuffer rwbuf;
    ssize_t ret = 0;

    if (!count) {
        return 0;
    }

    if (!access_ok(VERIFY_READ, buf, count)) {
        XM6_MSG(XM6_MSG_ERR, "access error");
        return -EFAULT;
    }

    if (mutex_lock_interruptible(&dev_data->dev_mutex)) {
        XM6_MSG(XM6_MSG_ERR, "unable to lock mutex");
        return -EINTR;
    }

    // read offset from buf
    if (copy_from_user(&rwbuf, buf, sizeof(rwbuf)) != 0) {
        XM6_MSG(XM6_MSG_ERR, "could not read from user buffer");
        ret = -EFAULT;
        goto out;
    }

#if 0 // ORIGINAL_PCI_BAR
    // write content of buf to bar 0
    if (copy_from_user(((char *)dev_data->bar[0]) + rwbuf.offset,
            rwbuf.buf, count) != 0) {
        XM6_MSG(XM6_MSG_ERR, "%s -> copy from user failed", __FUNCTION__);
        ret = -EFAULT;
        goto out;
    }
#else
    XM6_MSG(XM6_MSG_ERR, "copy_from_user bar[0], offset %lu, size %d\n", rwbuf.offset, count);
#endif

    *pos += count;
    ret = count;

out:
    mutex_unlock(&dev_data->dev_mutex);
    return ret;
}

/*!
 * ioctl handler
 *
 * special commands executer.
 * in case the command is not supported by this module, it forwards it
 * to the ceva_linkdrv module to handle.
 * @param [in] filp device handler
 * @param [in] cmd command to execute
 * @param [in] arg additional argument (differ for each command)
 * @return zero for success, nonzero for failures
 * @see IOC_CEVADRV_GENERATE_INT, IOC_CEVADRV_PRINT_DMA, ceva_linkdrv_ioctl()
 */
static long xm6_drv_ioctl(struct file *filp, unsigned int cmd,
        unsigned long arg) {

    int ret = 0;
    struct xm6_dev_data *dev_data = (struct xm6_dev_data *) filp->private_data;
    struct ceva_linkdrv* linkdrv = &dev_data->linkdrv;

    /* don't even decode wrong cmds: better returning  ENOTTY than EFAULT */
    if (_IOC_TYPE(cmd) != IOC_CEVADRV_MAGIC) {
        return -ENOTTY;
    }
    if (_IOC_NR(cmd) > IOC_CEVADRV_MAXNR) {
        return -ENOTTY;
    }

    switch (cmd) {
    case IOC_CEVADRV_GENERATE_INT:
        /* use arg as raw data for interrupt */
        XM6_MSG(XM6_MSG_DBG, "generating interrupt %lx", arg);
        ret = ceva_link_generate_irq(dev_data, arg);
        if (!ret) {
            DEBUG_INFO_INC(linkdrv, generated_interrupts);
        } else {
            DEBUG_INFO_INC(linkdrv, generate_interrupt_failures);
        }
        break;

    case IOC_CEVADRV_PRINT_DMA: {
#ifdef ENABLE_DEBUG
            struct xm6_dev_dma_buf *dma_buf = &dev_data->dma_buf;
            int off = (int)arg;
            int* addr = (int*)arg;
            int* p = dma_buf->cpu_addr;

            XM6_MSG(XM6_MSG_DBG, "IOC_CEVADRV_PRINT_DMA :: dma address @ 0x%p -> 0x%x", addr, p[off]);
#endif // ENABLE_DEBUG
        }
        break;

    case IOC_CEVADRV_BOOT_UP: {
            struct dsp_image image;

            ret = copy_from_user(&image, (void*)arg, sizeof(image));
            if (ret != 0) {
                XM6_MSG(XM6_MSG_ERR, "can't copy data from user space (%d copied %d)\n", sizeof(image), ret);
                return -ENOMEM;
            }

            ret = boot_loader(&dev_data->ceva_hal, dev_data->dsp_code_phys, dev_data->dsp_code_virt, DSP_CODE_SIZE, image.data, image.size);
            if (ret != 0) {
                XM6_MSG(XM6_MSG_ERR, "can't trigger DSP\n");
                return ret;
            }
            break;
        }

    default:
        ret = ceva_linkdrv_ioctl(linkdrv, cmd, arg);
    }

    return ret;
}

static void xm6_drv_dma_vma_open(struct vm_area_struct *vma) {
    struct xm6_dev_dma_buf* dma_buf;
    struct xm6_dev_data *dev_data;

    XM6_MSG(XM6_MSG_DBG, "xm6_drv_dma_vma_open: virt %lx, phys %lx",
                vma->vm_start, vma->vm_pgoff << PAGE_SHIFT);

    dma_buf = vma->vm_private_data;
    dev_data = dma_buf->priv_data;
}

static void xm6_drv_dma_vma_close(struct vm_area_struct *vma) {
    struct xm6_dev_dma_buf* dma_buf;
    struct xm6_dev_data *dev_data;

    dma_buf = vma->vm_private_data;
    dev_data = dma_buf->priv_data;
}

/*!
 * mmap handler for dma memory
 *
 * this function remap the internal dma memory that was mapped during
 * probe function to userland.
 * @param [in] dev_data device driver object
 * @param [in] vma vma object
 * @return zero for success, nonzero for failures
 * @see xm6_drv_probe()
 */
static int xm6_drv_mmap_dma(struct xm6_dev_data *dev_data,
        struct vm_area_struct *vma) {
    struct xm6_dev_dma_buf *dma_buf = &dev_data->dma_buf;

#ifdef ENABLE_DEBUG
    static int count = 0;
#endif // ENABLE_DEBUG

    XM6_MSG(XM6_MSG_DBG, "xm6_drv_mmap_dma: setting vma");

    vma->vm_ops = &xm6_drv_dma_vma_ops;
    vma->vm_private_data = dma_buf;
    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

    xm6_drv_dma_vma_open(vma);

#ifdef ENABLE_DEBUG
    XM6_MSG(XM6_MSG_DBG, "xm6_drv_mmap_dma: enter #%d", ++count);
#endif

    XM6_MSG(XM6_MSG_DBG, "xm6_drv_mmap_dma: remap started, cpu_addr=%p, size=%lu",
            dma_buf->cpu_addr, vma->vm_end - vma->vm_start);

    if (remap_pfn_range(vma,
            vma->vm_start,
            vmalloc_to_pfn(dma_buf->cpu_addr),
            vma->vm_end - vma->vm_start,
            vma->vm_page_prot)) {
        XM6_MSG(XM6_MSG_ERR, "xm6_drv_mmap_dma: remap failed");
        return -EAGAIN;
    }

    XM6_MSG(XM6_MSG_DBG, "xm6_drv_mmap_dma: remap succeeded");
    return 0;
}

/*!
 * mmap handler
 *
 * wrapper function for mapping dma to user.
 * @param [in] filp device handler
 * @param [in] vma vma object
 * @return zero for success, nonzero for failures
 * @note the wrapper was implemented to allow mapping of pci memory in
 * future versions (if needed)
 * @see xm6_drv_mmap_dma()
 */
static int xm6_drv_mmap(struct file *filp, struct vm_area_struct *vma) {
    struct xm6_dev_data *dev_data = (struct xm6_dev_data *) filp->private_data;

    XM6_MSG(XM6_MSG_DBG, "xm6_drv_mmap:");
    return xm6_drv_mmap_dma(dev_data, vma);
}

/*!
 * cleanup logical part of this module
 *
 * this function deletes any non-pci related driver's objects (the chardev
 * properties).
 * @param [in] dev_data device handler
 */
static void xm6_drv_cleanup_module(struct platform_device *plat_dev, struct xm6_dev_data *dev_data) {
    device_destroy(xm6_drv_data.class, dev_data->cdev.dev);
    cdev_del(&dev_data->cdev);

    mutex_destroy(&dev_data->dev_mutex);
}

/*!
 * buildup the char device of this module
 *
 * this function handles the creation and setup of the char device
 * @param [in] dev_data device handler
 * @return negative error code is returned on failure
 * @see ceva_linkdrv_init_module()
 */
static int xm6_drv_setup_cdev(struct platform_device *plat_dev, struct xm6_dev_data *dev_data) {
    int ret;
    struct device *device = NULL;
    int dev_count, dev_no;

    dev_count = xm6_drv_data.minor_star + xm6_drv_data.reg_count;
    dev_no = MKDEV(xm6_drv_data.major, dev_count);

    cdev_init(&dev_data->cdev, &xm6_drv_fops);
    dev_data->cdev.owner = THIS_MODULE;
    ret = cdev_add(&dev_data->cdev, dev_no, 1);
    /* Fail gracefully if need be */
    if (ret < 0) {
        XM6_MSG(XM6_MSG_ERR, "setup_cdev: Error %d adding device", dev_no);
        goto exit;
    }

    device = device_create(
            xm6_drv_data.class,                 /* class */
            NULL,                               /* no parent device */
            dev_no,
            dev_data,
            CEVA_PCIDEV_DEVICE_NAME "%d",
            dev_count);

    if (IS_ERR(device)) {
        ret = PTR_ERR(device);
        XM6_MSG(XM6_MSG_ERR, "setup_cdev: Error %d while trying to create %s%d",
                ret,
                CEVA_PCIDEV_DEVICE_NAME,
                dev_count);
        cdev_del(&dev_data->cdev);
    }

    dev_data->count = 0;

    // Increase registered count
    xm6_drv_data.reg_count++;

    dev_set_drvdata(&plat_dev->dev, dev_data);

exit:
    return ret;

}

/*!
 * initialize logical part of this module
 *
 * this function allocate and setup all non-pci related driver's objects
 * (the chardev properties).
 * @param [in] dev_data device handler
 * @return zero for success, nonzero for failures
 * @see xm6_drv_setup_cdev()
 */
static int ceva_linkdrv_init_module(struct platform_device *plat_dev, struct xm6_dev_data *dev_data) {
    int ret;

    mutex_init(&dev_data->dev_mutex);

    ret = xm6_drv_setup_cdev(plat_dev, dev_data);
    if (ret < 0) {
        goto fail;
    }

    return 0;

fail:
    xm6_drv_cleanup_module(plat_dev, dev_data);
    return ret;
}

#if 0
/*!
 * get pci device revision
 *
 * @param [in] plat_dev device handelr
 * @return revision number
 */
static inline u8 xm6_drv_get_revision(struct platform_device *plat_dev) {
    u8 revision = 0;

    pci_read_config_byte(plat_dev, PCI_REVISION_ID, &revision);
    return revision;
}

/*!
 * get pci device hw irq number
 *
 * @param [in] plat_dev device handelr
 * @return irq id
 */
static inline u8 xm6_drv_get_irq(struct platform_device *plat_dev) {
    u8 irq = 0;
    pci_read_config_byte(plat_dev, PCI_INTERRUPT_LINE, &irq);
    return irq;
}

/*!
 * find upstream PCIe root node
 *
 * Used for re-training and disabling AER
 * @note based of Altera's sample driver
 * @param [in] plat_dev device handler
 * @return pointer for the root node in case it was found
 * @see xm6_drv_probe(), retrain_gen2()
 */

static struct platform_device* find_upstream_dev(struct platform_device *plat_dev) {
    struct pci_bus *bus = 0;
    struct platform_device *bridge = 0;
    struct platform_device *cur = 0;
    int found_dev = 0;

    bus = plat_dev->bus;
    if (bus == 0) {
        XM6_MSG(XM6_MSG_WRN, "Device doesn't have an associated bus!");
        return 0;
    }

    bridge = bus->self;
    if (bridge == 0) {
        XM6_MSG(XM6_MSG_WRN, "Can't get the bridge for the bus!");
        return 0;
    }

    XM6_MSG(XM6_MSG_DBG, "Upstream device %x/%x, bus:slot.func %02x:%02x.%02x",
            bridge->vendor, bridge->device,
            bridge->bus->number, PCI_SLOT(bridge->devfn),
            PCI_FUNC(bridge->devfn));

    XM6_MSG(XM6_MSG_DBG, "List of downstream devices:");
    list_for_each_entry (cur, &bus->devices, bus_list) {
        if (cur != 0) {
            XM6_MSG(XM6_MSG_DBG, "  %x/%x", cur->vendor, cur->device);
            if (cur == plat_dev) {
                found_dev = 1;
            }
        }
    }

    if (found_dev) {
        return bridge;
    } else {
        XM6_MSG(XM6_MSG_WRN, "Couldn't find upstream device!");
        return 0;
    }
}
#endif
/*!
 * check link speed and retrain it to gen2 speeds
 *
 * after reprogramming, the link defaults to gen1 speeds for some reason.
 * doing re-training by finding the upstream root device and telling it
 * to retrain itself. Doesn't seem to be a cleaner way to do this
 * @note based of Altera's sample driver
 * @param [in] plat_dev PCIe device handler (allocated by the bsp)
 * @param [in] dev_data device handler
 * @see xm6_drv_probe()
 */
 #if 0
static inline void retrain_gen2(struct platform_device *plat_dev,
        struct xm6_dev_data *dev_data) {

    u16 linkstat, speed, width;
    struct platform_device *upstream;
    int pos, upos;
    u16 status_reg, control_reg, link_cap_reg;
    u16 status, control;
    u32 link_cap;
    int training, timeout;

    /* Defines for some special PCIe control bits */
    #define DISABLE_LINK_BIT         (1 << 4)
    #define RETRAIN_LINK_BIT         (1 << 5)
    #define TRAINING_IN_PROGRESS_BIT (1 << 11)
    #define LINKSPEED_2_5_GB         (0x1)
    #define LINKSPEED_5_0_GB         (0x2)

    pos = pci_find_capability(plat_dev, PCI_CAP_ID_EXP);
    if (!pos) {
        XM6_MSG(XM6_MSG_WRN, "Can't find PCI Express capability!");
        return;
    }

    /* Find root node for this bus and tell it to retrain itself. */
    upstream = dev_data->upstream;
    if (upstream == NULL) {
        return;
    }

    upos = pci_find_capability(upstream, PCI_CAP_ID_EXP);
    status_reg = upos + PCI_EXP_LNKSTA;
    control_reg = upos + PCI_EXP_LNKCTL;
    link_cap_reg = upos + PCI_EXP_LNKCAP;
    pci_read_config_word(upstream, status_reg, &status);
    pci_read_config_word(upstream, control_reg, &control);
    pci_read_config_dword(upstream, link_cap_reg, &link_cap);
    pci_read_config_word(plat_dev, pos + PCI_EXP_LNKSTA, &linkstat);
    pci_read_config_dword (upstream, link_cap_reg, &link_cap);
    speed = linkstat & PCI_EXP_LNKSTA_CLS;
    width = (linkstat & PCI_EXP_LNKSTA_NLW) >> PCI_EXP_LNKSTA_NLW_SHIFT;

    dev_data->pci_gen = (speed == LINKSPEED_5_0_GB) ? 2 : 1;
    dev_data->pci_num_lanes = width;

    if (speed == LINKSPEED_2_5_GB) {
        XM6_MSG(XM6_MSG_DBG,  "Link is operating at 2.5 GT/s with %d lanes. Need to retrain",
                width);
    } else if (speed == LINKSPEED_5_0_GB) {
        XM6_MSG(XM6_MSG_DBG,  "Link is operating at 5.0 GT/s with %d lanes.", width);
        if (width == 4) {
            XM6_MSG(XM6_MSG_DBG,  "  All is good!");
            return;
        } else {
            XM6_MSG(XM6_MSG_DBG,  "  Need to retrain.");
        }
    } else {
        XM6_MSG(XM6_MSG_WRN, "Not sure what's going on. Retraining.");
    }

    /* Perform the training. */
    training = 1;
    timeout = 0;
    pci_read_config_word(upstream, control_reg, &control);
    pci_write_config_word(upstream, control_reg, control | RETRAIN_LINK_BIT);

    while (training && timeout < 50) {
        pci_read_config_word(upstream, status_reg, &status);
        training = (status & TRAINING_IN_PROGRESS_BIT);
        msleep (1); /* 1 ms */
        ++timeout;
    }
    if(training) {
        XM6_MSG(XM6_MSG_DBG,  "Error: Link training timed out.");
        XM6_MSG(XM6_MSG_DBG,  "PCIe link not established.");
    }
    else {
        XM6_MSG(XM6_MSG_DBG,  "Link training completed in %d ms.", timeout);
    }
    /* Verify that it's a 5 GT/s link now */
    pci_read_config_word (plat_dev, pos + PCI_EXP_LNKSTA, &linkstat);
    pci_read_config_dword (upstream, link_cap_reg, &link_cap);
    speed = linkstat & PCI_EXP_LNKSTA_CLS;
    width = (linkstat & PCI_EXP_LNKSTA_NLW) >> PCI_EXP_LNKSTA_NLW_SHIFT;

    dev_data->pci_gen = (speed == LINKSPEED_5_0_GB) ? 2 : 1;
    dev_data->pci_num_lanes = width;

    if(speed == LINKSPEED_5_0_GB) {
        XM6_MSG(XM6_MSG_DBG, "Link operating at 5 GT/s with %d lanes", width);
    } else {
        XM6_MSG(XM6_MSG_WRN, "Link training failed. Link at 2.5 GT/s with %d lanes.", width);
    }
}

/*!
 * enable the PCIe device and gain access to it's memory regions
 *
 * @param [in] plat_dev PCIe device handler
 * @return zero for success, nonzero for failures
 * @see xm6_drv_probe()
 */
static inline int probe_enable_pci_dev(struct platform_device *plat_dev) {

    int ret;

    ret = pci_enable_device(plat_dev);
    if (unlikely(ret != 0)) {
        XM6_MSG(XM6_MSG_ERR, "probe: failed to enable pci\n");
        goto out;
    }

    pci_set_master(plat_dev);
    // ret = pci_try_set_mwi(plat_dev);
    if (unlikely(ret != 0)) {
        XM6_MSG(XM6_MSG_ERR, "probe: failed to set memory-write-invalidate, (err:%d)\n", ret);
        goto fail_set_mwi;
    }

    ret = pci_request_regions(plat_dev, DRIVER_NAME);
    if (ret != 0) {
        XM6_MSG(XM6_MSG_ERR, "probe: failed to get regions, (err:%d)\n", ret);
        goto fail_regions;
    }

    XM6_MSG(XM6_MSG_DBG, "probe: pci device enabled");

    return ret;

fail_regions:
fail_set_mwi:
    pci_disable_device(plat_dev);
out:
    return ret;
}
#endif


/*!
 * get PCIe hardware properties (irq, revision)
 * @param [in] plat_dev PCIe device handler
 * @param [in] dev_data device handler
 * @return zero for success, nonzero for failures
 * @see xm6_drv_probe()
 */
static inline int probe_get_hw_prop(struct platform_device* plat_dev, struct xm6_dev_data *dev_data) {
    struct resource *res = NULL;

    dev_data->hw_irq_id = irq_of_parse_and_map(plat_dev->dev.of_node, 0);
    if (dev_data->hw_irq_id == 0) {
        XM6_MSG(XM6_MSG_ERR, "Can't get IRQ ID");
        return -ENODEV;
    }

    res = platform_get_resource(plat_dev, IORESOURCE_MEM, 0);
    if (res == NULL) {
        XM6_MSG(XM6_MSG_ERR, "Can't get IO addr 0");
        return -ENODEV;
    }
    dev_data->hw_addr_xm6 = res->start;

    res = platform_get_resource(plat_dev, IORESOURCE_MEM, 1);
    if (res == NULL) {
        XM6_MSG(XM6_MSG_ERR, "Can't get IO addr 0");
        return -ENODEV;
    }
    dev_data->hw_addr_sys = res->start;

    res = platform_get_resource(plat_dev, IORESOURCE_MEM, 2);
    if (res == NULL) {
        XM6_MSG(XM6_MSG_ERR, "Can't get IO addr 1");
        return -ENODEV;
    }
    dev_data->hw_addr_axi2miu0 = res->start;

    res = platform_get_resource(plat_dev, IORESOURCE_MEM, 3);
    if (res == NULL) {
        XM6_MSG(XM6_MSG_ERR, "Can't get IO addr 2");
        return -ENODEV;
    }
    dev_data->hw_addr_axi2miu1 = res->start;

    res = platform_get_resource(plat_dev, IORESOURCE_MEM, 4);
    if (res == NULL) {
        XM6_MSG(XM6_MSG_ERR, "Can't get IO addr 3");
        return -ENODEV;
    }
    dev_data->hw_addr_axi2miu2 = res->start;

    res = platform_get_resource(plat_dev, IORESOURCE_MEM, 5);
    if (res == NULL) {
        XM6_MSG(XM6_MSG_ERR, "Can't get IO addr 4");
        return -ENODEV;
    }
    dev_data->hw_addr_axi2miu3 = res->start;


    XM6_MSG(XM6_MSG_DBG, "irq id: %d, hw addr: %x, %x %x, %x, %x", dev_data->hw_irq_id, dev_data->hw_addr_sys,
                                                    dev_data->hw_addr_axi2miu0, dev_data->hw_addr_axi2miu1,
                                                    dev_data->hw_addr_axi2miu2, dev_data->hw_addr_axi2miu3);
    return 0;
}

/*!
 * generates an interrupt over the device
 *
 * this function checks for device's availability to receive new interrupts
 * and then writes the new interrupt data to the predefined address in bar0
 * @param dev_data device handler
 * @param e interrupt id to generate
 * @return zero for success, -EBUSY if device's fifo is full
 * @see xm6_drv_ioctl(), IOC_CEVADRV_GENERATE_INT
 */
static int ceva_link_generate_irq(struct xm6_dev_data *dev_data, ceva_event_t e) {

#if 0 // Original implementation is disabled, we may remove it latter
    u32 fifo_stat = 0;
    //u32 fifo_stat = readl(dev_data->bar[0] + CEVA_HOST_INTERRUPT_FIFO);
    if (!fifo_stat) {
        writel((u32) e, dev_data->bar[0] + CEVA_HOST_INTERRUPT_FIFO);
        XM6_MSG(XM6_MSG_DBG, "interrupt generated");
        return 0;
    }
#endif

    XM6_MSG(XM6_MSG_WRN, "unable to generate interrupt\n");

    return -EBUSY;
}

#if 0
/*!
 * scans the PCIe bars to check which is available (debug function)
 * @note available only when ENABLE_DEBUG is defined
 * @return always zero
 */
static inline int scan_bars(struct platform_device *plat_dev, struct xm6_dev_data *dev_data)
{
#ifdef ENABLE_DEBUG
    int i;
    for (i = 0; i < ACL_PCI_NUM_BARS; i++) {
        struct resource *resource = &plat_dev->resource[i];
        XM6_MSG(XM6_MSG_DBG, "resource [%d] :: name: %s, flags: %lu, start: %p, end: %p\n"
              "parent: %p, child: %p, sibling: %p",
              i,
              resource->name,
              resource->flags,
              (void*)resource->start,
              (void*)resource->end,
              resource->parent,
              resource->child,
              resource->sibling);
    }
#endif
    return 0;
}

/*!
 * maps the PCIe memory bars to internal device's virtual space *
 * @param [in] plat_dev PCIe device handler
 * @param [in/out] device handler
 * @note additional debug warnings/errors are printed when ENABLE_DEBUG
 * is defined
 * @return always zero
 * @see free_bars()
 */
static inline int map_bars(struct platform_device *plat_dev,
        struct xm6_dev_data *dev_data)
{
    int i;
    for (i = 0; i < ACL_PCI_NUM_BARS; i++){
#ifdef ENABLE_DEBUG
        unsigned long bar_start = pci_resource_start(plat_dev, i);
        unsigned long bar_end = pci_resource_end(plat_dev, i);
        unsigned long bar_length = bar_end - bar_start + 1;

        if (!bar_start || !bar_end) {
            XM6_MSG(XM6_MSG_WRN, "could not obtain start/end addresses for BAR #%d(%lu/%lu)",
                    i, bar_start, bar_end);
            continue;
        }
        if (bar_length < 1) {
            XM6_MSG(XM6_MSG_ERR, "BAR #%d length is less than 1 byte", i);
            continue;
        }

        /*
         * map the device memory or IO region to kernel virtual address space
         */
        dev_data->bar[i] = ioremap_nocache(bar_start, bar_length);
#else
        dev_data->bar[i] = pci_ioremap_bar(plat_dev, i);
#endif
        if (!dev_data->bar[i]) {
          XM6_MSG(XM6_MSG_WRN, "Could not map BAR #%d.", i);
          continue;
        }

        XM6_MSG(XM6_MSG_DBG, "BAR[%d] mapped at 0x%p with length %lu.",
                i, dev_data->bar[i], bar_length);
    }
    return 0;
}

/*!
 * frees the mapped PCIe memory space allocated by map_bars
 * @param [in] plat_dev PCIe device handler
 * @param [in/out] device handler
 * @see map_bars()
 */
static void free_bars(struct platform_device *plat_dev, struct xm6_dev_data *dev_data) {
    int i;
    for (i = 0; i < ACL_PCI_NUM_BARS; i++) {
        if (dev_data->bar[i]) {
            pci_iounmap(plat_dev, dev_data->bar[i]);
            dev_data->bar[i] = NULL;
        }
    }
}

/*!
 * scans and maps the PCIe memory and i/o regions
 * @param [in] plat_dev PCIe device handler
 * @param [in/out] dev_data device handler
 * @return zero for success, nonzero for failure
 * @see xm6_drv_probe(), map_bars()
 */
static inline int probe_map_bars(struct platform_device *plat_dev,
        struct xm6_dev_data *dev_data) {
    int ret;
    scan_bars(plat_dev, dev_data);
    ret = map_bars(plat_dev, dev_data);
    if (!ret) {
        if (!dev_data->bar[0]) {
            XM6_MSG(XM6_MSG_ERR, "unable to map PCI bar 0");
            ret |= BIT(0);
        }
        if (!dev_data->bar[1]) {
            XM6_MSG(XM6_MSG_ERR, "unable to map PCI bar 1");
            ret |= BIT(1);
        }
    }

    return ret;
}
#endif

/*!
 * maps the dma region of the PCIe device
 *
 * this function allocate and maps the dma region.
 * the size of the dma region is predefined with CEVADEV_PCI_DMA_MEM_SIZE
 * after the region is allocated and mapped to the driver, it tells the
 * device the physical memory address that was mapped in order to let it
 * gain control over it for r/w operations.
 * @param [in/out] device handler
 * @return zero for success, nonzero for failures
 * @ see xm6_drv_probe(), CEVADEV_PCI_DMA_MEM_SIZE
 */
static inline int probe_map_dma(struct xm6_dev_data *dev_data) {
    struct xm6_dev_dma_buf *dma_buf;
    char *pcie_cra_addr_trans;
    dma_addr_t dma_addr;

    if (!dev_data) {
        return -EINVAL;
    }
    dma_buf = &dev_data->dma_buf;

    dma_buf->size = CEVADEV_PCI_DMA_MEM_SIZE;
    dma_buf->priv_data = dev_data;

    XM6_MSG(XM6_MSG_DBG, "probe_map_dma: dma_buf->size=%x", dma_buf->size);

    // dma_buf->cpu_addr = pci_alloc_consistent(dev_data->plat_dev, dma_buf->size, &dma_buf->dma_addr);
    // dma_buf->cpu_addr = __get_free_pages(GFP_KERNEL | __GFP_DMA, CEVADEV_PCI_DMA_MEM_SIZE_ORDER);
    dma_buf->cpu_addr = dma_alloc_coherent(NULL, dma_buf->size, &dma_buf->dma_addr, GFP_KERNEL|GFP_DMA);
    if (dma_buf->cpu_addr == NULL) {
        XM6_MSG(XM6_MSG_ERR, "probe: failed to map dma buffer");
        return -ENOMEM;
    }

    XM6_MSG(XM6_MSG_DBG, "forward dma address (%p) to device", (void*) dma_buf->dma_addr);

    ceva_hal_write_dummy_data(&dev_data->ceva_hal, dma_addr);

    XM6_MSG(XM6_MSG_DBG, "address %lx written to %p (bar1 with %x offset)",
            (long unsigned) dma_addr,
            pcie_cra_addr_trans,
            CAVA_PCIE_CRA_ADDR_BASE);

    XM6_MSG(XM6_MSG_DBG, "probe: dma mapped, physical: %lxH, address: %pH (size: %zuH)",
            (long unsigned) dma_buf->dma_addr, dma_buf->cpu_addr, dma_buf->size);

    return 0;
}

/*!
 * enable PCIe message signaled interrupts (msi)
 *
 * pci allows up to 32 msi, we try to enable all, in case of failure we
 * update the msis to the allowed number and try again
 * @param [in] plat_dev PCIe device handler
 * @param [in/out] dev_data device handler
 * @return zero for success, nonzero for failures
 * @note in case this function suceeded, the irq value in plat_dev changes
 * to the number of the the first msi.
 * @see xm6_drv_probe(), probe_init_irq(), probe_deinit_irq()
 */
#if defined(FUNC_SHOULD_REMOVE)
static inline int probe_enable_msi(struct platform_device *plat_dev, struct xm6_dev_data *dev_data) {

    int ret = 1;

    dev_data->msis = 32; // TODO remove this (number of msi should not be 32)
    // ret = pci_enable_msi_block(plat_dev, dev_data->msis);
    if (ret > 0) {
        XM6_MSG(XM6_MSG_WRN, "probe: failed to enable %d msi, retrying with %d",
                dev_data->msis, ret);
        dev_data->msis = (unsigned int) ret;
        //ret = pci_enable_msi_block(plat_dev, dev_data->msis);
    }
    if (ret) {
        XM6_MSG(XM6_MSG_ERR, "probe: failed to enable %d msi", dev_data->msis);
        return -EIO;
    }

    XM6_MSG(XM6_MSG_DBG, "msi enabled, %d irqs available from irq %d",
            dev_data->msis, plat_dev->irq);
    return 0;
}
#endif

/*!
 * interrupt routine
 *
 * this function handles the device's interrupts.
 * it first reads from the device irq status register in order to tell
 * which interrupts where sent to the device (there up to 16), and then
 * reads the interrupts data, and forward it to the logical module to
 * handle.
 * @param [in] irq irq id
 * @param [in] dev_id device handler
 * @return IRQ_HANDLED for success, IRQ_NONE for error
 * @see ceva_linkdrv_broadcast_events()
 */
static irqreturn_t ceva_linkdrv_interrupt(int irq, void *dev_id) {

    struct xm6_dev_data *dev_data = (struct xm6_dev_data *) dev_id;
#if 0 // ORIGINAL_PCI_BAR
    int i, cn;
    u32 irq_addr;
    u32 irq_data[16];
#endif
    u32 irq_status = 0;

    XM6_MSG(XM6_MSG_DBG, "interrupt (%d)", irq);

    if (!dev_data) {
        DEBUG_INFO_INC(&dev_data->linkdrv, unhandled_irq);
        return IRQ_NONE;
    }

    /*
     * From this point on, this is our interrupt. So return IRQ_HANDLED
     * no matter what (since nobody else in the system will handle this
     * interrupt for us).
     */
    DEBUG_INFO_INC(&dev_data->linkdrv, handled_irq);
    // read status to obtain which irq register to read
#if 0 // ORIGINAL_PCI_BAR
    irq_status = readl((char*) dev_data->bar[0] + CEVA_IRQ_STATUS_ADDR);
#else
    XM6_MSG(XM6_MSG_ERR, "irq_status is read from bar 0, offset %d (CEVA_IRQ_STATUS_ADDR)\n", CEVA_IRQ_STATUS_ADDR);
#endif
    irq_status &= 0x0000ffff;
    XM6_MSG(XM6_MSG_DBG, "irq status=%lx", (long unsigned) irq_status);

#if 0 // ORIGINAL_PCI_BAR
    for (cn = 0, i = 0; i < 16; i++) {
        if (irq_status & 1<<i) {
            irq_addr = GET_IRQ_DATA_ADDR(i);
            XM6_MSG(XM6_MSG_DBG, "interrupt #%02x is on, reading %p", i, (void *) irq_addr);
            irq_data[cn] = readl((char*) dev_data->bar[0] + irq_addr);
            XM6_MSG(XM6_MSG_DBG, "interrupt #%02x, address offset %p, data = %lx",
                    i,
                    (char*) dev_data->bar[0] + irq_addr,
                    (long unsigned int) irq_data[cn]);
            cn++;
        }
    }

    // send to linkdrv layer
    XM6_MSG(XM6_MSG_DBG, "sending %d events to link layer", cn);
    ceva_linkdrv_broadcast_events(&dev_data->linkdrv, irq_data, cn);
#else
    XM6_MSG(XM6_MSG_ERR, "read irq_data[16] from bar 0, offset %d (CEVA_IRQ_STATUS_ADDR)\n", CEVA_IRQ_STATUS_ADDR);
#endif

    return IRQ_HANDLED;
}

/*!
 * enable PCIe irq
 * @param [in] plat_dev PCIe device handler
 * @param [in] dev_data device handler
 * @return zero for success, nonzero for failures
 * @see xm6_drv_probe()
 */
static int probe_init_irq(struct platform_device *plat_dev, struct xm6_dev_data *dev_data) {
    int ret = request_irq(dev_data->hw_irq_id,
                          ceva_linkdrv_interrupt,
                          0,
                          DRIVER_NAME, dev_data);
    if (ret != 0) {
        XM6_MSG(XM6_MSG_ERR, "Could not request IRQ #%d, error %d", dev_data->hw_irq_id, ret);
        return -EIO;
    }

    XM6_MSG(XM6_MSG_DBG, "Successfully requested IRQ #%d", dev_data->hw_irq_id);
    return 0;
}

/*!
 * disable PCIe irq
 * @param [in] plat_dev PCIe device handler
 * @param [in] dev_data device handler
 * @see xm6_drv_remove()
 */
static void remove_deinit_irq(struct platform_device *plat_dev, struct xm6_dev_data *dev_data) {
    free_irq(dev_data->hw_irq_id, dev_data);
    XM6_MSG(XM6_MSG_DBG, "irq freed");
}

/*!
 * XM6 probe function
 *
 * this function enables and starts CEVA PCIe device
 * @param [in/out] plat_dev PCIe device handler
 * @param [in] id not used
 * @return zero for success, nonzero for failures
 * @see find_upstream_dev(), retrain_gen2(), probe_get_hw_prop(),
 * probe_map_bars(), probe_map_dma(), probe_enable_msi(),probe_init_irq(),
 * ceva_linkdrv_init_module(), xm6_drv_remove()
 */
static int xm6_drv_probe(struct platform_device *plat_dev) {
    struct xm6_dev_data *dev_data = NULL;
    struct xm6_dev_dma_buf *dma_buf = NULL;
    int ret = 0;

    XM6_MSG(XM6_MSG_DBG, "device probed (dev_data = 0x%p)", plat_dev);
#if 0
    XM6_MSG(XM6_MSG_DBG, "vendor = 0x%x, device = 0x%x, class = 0x%x, "
            "bus:slot.func = %02x:%02x.%02x",
            plat_dev->vendor, plat_dev->device, plat_dev->class,
            plat_dev->bus->number, PCI_SLOT(plat_dev->devfn),
            PCI_FUNC(plat_dev->devfn));
#endif

    dev_data = devm_kcalloc(&plat_dev->dev, 1, sizeof(struct xm6_dev_data), GFP_KERNEL);
    if (unlikely(!dev_data)) {
        ret = -ENOMEM;
        XM6_MSG(XM6_MSG_ERR, "probe: allocation error");
        goto alloc_fail;
    }

    dev_data->plat_dev = plat_dev;
#if 0
    pci_set_drvdata(plat_dev, dev_data);
    dev_data->pci_gen = 0;
    dev_data->pci_num_lanes = 0;

    dev_data->upstream = find_upstream_dev(plat_dev);
    retrain_gen2(plat_dev, dev_data);

    /*
     * enable pci device
     */
    ret = probe_enable_pci_dev(plat_dev);
    if (unlikely(ret != 0)) {
        goto enable_fail;
    }
#endif

    /*
     * get hardware properties
     */
    ret = probe_get_hw_prop(plat_dev, dev_data);
    if (unlikely(ret != 0)) {
        goto enable_fail;
    }

    /* Init ceva wrapper hal */
    cava_hal_init(&dev_data->ceva_hal, dev_data->hw_addr_sys,  dev_data->hw_addr_axi2miu0,  dev_data->hw_addr_axi2miu1,  dev_data->hw_addr_axi2miu2, dev_data->hw_addr_axi2miu3);

    /* Enable  ceva clock & power */
    clk_hal_init();

    /*
     * remap DSP code buffer
     */
#if 1
    dev_data->dsp_code_phys = E_EMAC_ID_CEVA_ADR;
#else
    dev_data->dsp_code_phys = __get_free_pages(GFP_KERNEL | GFP_DMA, get_order(DSP_CODE_SIZE));
    if (dev_data->dsp_code_phys == 0) {
        XM6_MSG(XM6_MSG_ERR, "can't allocate buffer for dsp code");
        goto enable_fail;
    }
#endif

    dev_data->dsp_code_virt = ioremap_nocache(dev_data->dsp_code_phys, DSP_CODE_SIZE);
    if (dev_data->dsp_code_virt == NULL) {
        XM6_MSG(XM6_MSG_ERR, "can't ioremap buffer for dsp code");
        goto enable_fail;
    }

    XM6_MSG(XM6_MSG_DBG, "ioremap dsp code buffer, phys: 0x%x, virt = 0x%p, size %d\n", dev_data->dsp_code_phys, dev_data->dsp_code_virt, DSP_CODE_SIZE);

    /*
     * map dma memory
     */
    ret = probe_map_dma(dev_data);
    if (unlikely(ret != 0)) {
        goto map_dma_fail;
    }

    /*
     * setup message signaled interrupt
     */
    #if defined(FUNC_SHOULD_REMOVE)
    ret = probe_enable_msi(plat_dev, dev_data);
    if (unlikely(ret != 0)) {
        goto enable_msi_fail;
    }
    #endif

    /*
     * request irq
     */
    ret = probe_init_irq(plat_dev, dev_data);
    if (unlikely(ret != 0)) {
        goto init_irq_fail;
    }

    /*
     * logical module init
     */
    ret = ceva_linkdrv_init_module(plat_dev, dev_data);
    if (unlikely(ret != 0)) {
        XM6_MSG(XM6_MSG_ERR, "probe: failed init logic module");
        goto logic_fail;
    }

    // Increase registered count
    xm6_drv_data.reg_count++;

    /*
     * probed successfully
     */

    XM6_MSG(XM6_MSG_DBG, "CEVA device probed\n");

    return 0;

logic_fail:
    remove_deinit_irq(plat_dev, dev_data);

init_irq_fail:
    // pci_disable_msi(plat_dev);

map_dma_fail:
    dma_buf = &dev_data->dma_buf;
    if (dma_buf->cpu_addr) {
        dma_free_coherent(NULL, dma_buf->size, dma_buf->cpu_addr, dma_buf->dma_addr);
    }

    iounmap(dev_data->dsp_code_virt);

#if 0
    free_bars(plat_dev, dev_data);
    remove_disable_pci_dev(plat_dev);
#endif

enable_fail:
    devm_kfree(&dev_data->plat_dev->dev, dev_data);

alloc_fail:
    XM6_MSG(XM6_MSG_ERR, "probe: failed (ret=%d)", ret);

    return ret;
}

/*!
 * XM6 remove function
 *
 * this function stops and disables the CEVA PCIe device
 * @param [in/out] plat_dev PCIe device handler
 * @see xm6_drv_cleanup_module(), remove_deinit_irq(), pci_disable_msi(),
 * free_bars(), remove_disable_pci_dev(), xm6_drv_probe()
 */
static s32 xm6_drv_remove(struct platform_device *plat_dev) {
    struct xm6_dev_data *dev_data;
    struct xm6_dev_dma_buf *dma_buf;

    dev_data = (struct xm6_dev_data *)dev_get_drvdata(&plat_dev->dev);
    if (unlikely(!dev_data)) {
        XM6_MSG(XM6_MSG_DBG, "device data is NULL.");
        return -ENODEV;
    }

    xm6_drv_cleanup_module(plat_dev, dev_data);
    dev_set_drvdata(&plat_dev->dev, NULL);

    /*
     * disable interrupt
     */
    remove_deinit_irq(plat_dev, dev_data);

    /*
     * disable msi
     */
    // pci_disable_msi(plat_dev);

    /*
     * free dma buffer
     */
    dma_buf = &dev_data->dma_buf;
    if (dma_buf->cpu_addr) {
        dma_free_coherent(NULL, dma_buf->size, dma_buf->cpu_addr, dma_buf->dma_addr);
    }

    iounmap(dev_data->dsp_code_virt);

    devm_kfree(&plat_dev->dev, dev_data);

    XM6_MSG(XM6_MSG_DBG, "Removed device\n");

    return 0;
}

/*!
 * driver't init function
 *
 * this function register the CEVA's supported PCI ids with
 * this driver.
 * it also inits the protected shared memory module.
 * @see xm6_drv_exit_module
 */
int __init xm6_drv_init_module(void) {
    int err;
    dev_t dev;

    XM6_MSG(XM6_MSG_DBG, "Moudle Init\n");

    if (protected_mem_db_init() != 0) {
        XM6_MSG(XM6_MSG_ERR, "failed to init protected memory db");
        return -ENODEV;
    }

    // Allocate cdev id
    err = alloc_chrdev_region(&dev, XM6_DRV_MINOR, XM6_DRV_DEVICE_COUNT, XM6_DRV_NAME);
    if (err) {
        XM6_MSG(XM6_MSG_ERR, "Unable allocate cdev id\n");
        return err;
    }

    xm6_drv_data.major = MAJOR(dev);
    xm6_drv_data.minor_star = MINOR(dev);
    xm6_drv_data.reg_count = 0;

    XM6_MSG(XM6_MSG_DBG, "allocate char dev: %d, %d\n", xm6_drv_data.major, xm6_drv_data.minor_star);

    // Register device class
    xm6_drv_data.class = class_create(THIS_MODULE, XM6_DRV_CLASS_NAME);
    if (IS_ERR(xm6_drv_data.class)) {
        XM6_MSG(XM6_MSG_ERR, "Failed at class_create().Please exec [mknod] before operate the device/n");
        err = PTR_ERR(xm6_drv_data.class);
        goto ERR_RETURN_1;
    }

    // Register platform driver
    err = platform_driver_register(&xm6_drv_driver);
    if (err != 0) {
        goto ERR_RETURN_2;
    }

    return 0;

ERR_RETURN_2:
    class_destroy(xm6_drv_data.class);

ERR_RETURN_1:
    unregister_chrdev_region(MKDEV(xm6_drv_data.major, xm6_drv_data.minor_star), XM6_DRV_DEVICE_COUNT);

    return err;
}

/*!
 * driver's deinit function
 *
 * this function unregister the CEVA's supported PCI ids with
 * this driver.
 * it also de-inits the protected shared memory module.
 * @see xm6_drv_init_module
 */
void __exit xm6_drv_exit_module(void) {
    XM6_MSG(XM6_MSG_DBG, "Modules Exit\n");

    protected_mem_db_deinit();

    platform_driver_unregister(&xm6_drv_driver);
    class_destroy(xm6_drv_data.class);
    unregister_chrdev_region(MKDEV(xm6_drv_data.major, xm6_drv_data.minor_star), XM6_DRV_DEVICE_COUNT);
}

module_init(xm6_drv_init_module);
module_exit(xm6_drv_exit_module);
