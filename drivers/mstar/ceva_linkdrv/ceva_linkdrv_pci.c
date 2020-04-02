/*
 * ceva_linkdrv_pci.c
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
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

#include "ceva_linkdrv-generic.h"
#include "protected_mem_db.h"
#include "ceva_linkdrv.h"
#include "pci_acl.h"
#include "ceva_linkdrv_pci.h"

MODULE_AUTHOR("Ido Reis <ido.reis@tandemg.com>");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0.1");
MODULE_DESCRIPTION("mapped pci character device driver");

static int ceva_pcidev_open(struct inode *inode, struct file *filp);
static int ceva_pcidev_release(struct inode *inode, struct file *filp);
static ssize_t ceva_pcidev_read(struct file *filp, char __user *buf,
		size_t count, loff_t *pos);
static ssize_t ceva_pcidev_write(struct file *filp, const char __user *buf,
		size_t count, loff_t *pos);
static long ceva_pcidev_ioctl(struct file *filp, unsigned int cmd,
		unsigned long arg);
static void ceva_pcidev_dma_vma_open(struct vm_area_struct *vma);
static void ceva_pcidev_dma_vma_close(struct vm_area_struct *vma);
static int ceva_pcidev_mmap_dma(struct ceva_pcidev *dev,
		struct vm_area_struct *vma);
static int ceva_pcidev_mmap(struct file *filp, struct vm_area_struct *vma);
static void ceva_pcidev_cleanup_module(struct ceva_pcidev* dev);
static int ceva_pcidev_setup_cdev(struct ceva_pcidev *dev);
static int ceva_linkdrv_init_module(struct ceva_pcidev* dev);
static u8 ceva_pcidev_get_revision(struct pci_dev *dev);
static u8 ceva_pcidev_get_irq(struct pci_dev *dev);
static struct pci_dev* find_upstream_dev(struct pci_dev *dev);
static void retrain_gen2(struct pci_dev *dev,
		struct ceva_pcidev* ceva_linkdrv);
static int probe_enable_pci_dev(struct pci_dev *pci_dev);
static void remove_disable_pci_dev(struct pci_dev *pci_dev);
static int probe_get_hw_prop(struct pci_dev* pci_dev, struct
		ceva_pcidev* dev);
static int ceva_link_generate_irq(struct ceva_pcidev *dev, ceva_event_t e);
static int scan_bars(struct pci_dev *dev,
		struct ceva_pcidev* ceva_linkdrv);
static int map_bars(struct pci_dev *dev,
		struct ceva_pcidev* ceva_linkdrv);
static void free_bars(struct pci_dev *dev, struct ceva_pcidev* ceva_linkdrv);
static int probe_map_bars(struct pci_dev *pci_dev,
		struct ceva_pcidev* dev);
static int probe_map_dma(struct ceva_pcidev* dev);
static int probe_enable_msi(struct pci_dev *pci_dev,
		struct ceva_pcidev* dev);
static irqreturn_t ceva_linkdrv_interrupt(int irq, void *dev_id);
static int probe_init_irq(struct pci_dev *pci_dev,
		struct ceva_pcidev* dev);
static void remove_deinit_irq(struct pci_dev *pci_dev,
		struct ceva_pcidev* dev);
static int probe(struct pci_dev *pci_dev,
		const struct pci_device_id *id);
static void remove(struct pci_dev *pci_dev);
int __init ceva_pcidev_init_module(void);
void __exit ceva_pcidev_exit_module(void);

static int ceva_pcidev_major;
static struct class *ceva_pcidev_class = NULL;

/*!
 * ops for dma mapped memory
 */
static struct vm_operations_struct ceva_pcidev_dma_vma_ops = {
		.open = ceva_pcidev_dma_vma_open,
		.close = ceva_pcidev_dma_vma_close
};

/*!
 * device ids supported by this driver
 */
static struct pci_device_id ids[] = {
	{ PCI_DEVICE(PCI_VENDOR_ID_CEVA, PCI_DEVICE_ID_CEVA), },
	{ 0, }
};
MODULE_DEVICE_TABLE(pci, ids);

/*!
 * fops for this driver's device
 */
static struct file_operations ceva_pcidev_fops = {
		.owner = THIS_MODULE,
		.read =	ceva_pcidev_read,
		.write = ceva_pcidev_write,
		.open = ceva_pcidev_open,
		.release = ceva_pcidev_release,
		.unlocked_ioctl = ceva_pcidev_ioctl,
		.llseek = no_llseek, /* This driver doesn't support llseek */
		.mmap = ceva_pcidev_mmap,
};

/*!
 * pci device driver fops
 */
static struct pci_driver ceva_pcidev_driver = {
		.name = DRIVER_NAME,
		.id_table =	ids,
		.probe = probe,
		.remove = remove,
};

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
static int ceva_pcidev_open(struct inode *inode, struct file *filp) {
	int ret = 0;
	unsigned int mj = imajor(inode);
	unsigned int mn = iminor(inode);
	struct ceva_pcidev *dev = NULL;

	if (mj != ceva_pcidev_major || mn < 0 ||
			mn >= CEVA_PCIDEV_NUMBER_OF_DEVICES) {
		LOG_I("No device found with minor=%d and major=%d",
				mj, mn);
		ret = -ENODEV; /* No such device */
		goto exit;
	}

	/* store a pointer to struct cfake_dev here for other methods */
	dev = container_of(inode->i_cdev, struct ceva_pcidev, cdev);
	filp->private_data = dev;

	if (inode->i_cdev != &dev->cdev) {
		LOG_W("open: internal error");
		ret = -ENODEV; /* No such device */
		goto exit;
	}
	/* This method can't fail, so no need to check for return value */
	nonseekable_open(inode, filp);

	if (mutex_lock_interruptible(&dev->dev_mutex)) {
		return -EINTR;
	}

	if (dev->count == 0) {
		ret = ceva_linkdrv_init(&dev->linkdrv, dev);
	}

	if (!ret) {
		ret = ceva_linkdrv_open_cb(&dev->linkdrv);
		if (!ret) {
			dev->count++;
		} else {
			if (dev->count == 0) {
				ceva_linkdrv_deinit(&dev->linkdrv);
			}
		}
	}

	mutex_unlock(&dev->dev_mutex);

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
static int ceva_pcidev_release(struct inode *inode, struct file *filp) {
	int ret;
	struct ceva_pcidev *dev = (struct ceva_pcidev *) filp->private_data;

	if (mutex_lock_interruptible(&dev->dev_mutex)) {
		return -EINTR;
	}
	ret = ceva_linkdrv_release_cb(&dev->linkdrv);
	if (dev->count)
		dev->count--;
	if (!dev->count) {
		ceva_linkdrv_deinit(&dev->linkdrv);
	}
	mutex_unlock(&dev->dev_mutex);

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
 * @see struct RWBuffer, ceva_pcidev_write()
 */
static ssize_t ceva_pcidev_read(struct file *filp, char __user *buf,
		size_t count, loff_t *pos) {
	struct ceva_pcidev *dev = (struct ceva_pcidev *) filp->private_data;
	struct RWBuffer rwbuf;
	ssize_t ret = 0;

	if (!count) {
		return 0;
	}

	if (!access_ok(VERIFY_WRITE, buf, count)) {
		LOG_E("access error");
		return -EFAULT;
	}

	if (mutex_lock_interruptible(&dev->dev_mutex)) {
		LOG_E("unable to lock mutex");
		return -EINTR;
	}

	// read offset from buf
	if (copy_from_user(&rwbuf, buf, sizeof(rwbuf)) != 0) {
		LOG_E("could not read from user buffer");
		ret = -EFAULT;
		goto out;
	}

	// read content of bar 0 to buf
	if (copy_to_user(rwbuf.buf,
			((char *)dev->bar[0]) + rwbuf.offset, count) != 0) {
		LOG_E("%s -> copy to user failed", __FUNCTION__);
		ret = -EFAULT;
		goto out;
	}
	*pos += ret;
	ret = count;
out:
	mutex_unlock(&dev->dev_mutex);
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
 * @see struct RWBuffer, ceva_pcidev_read()
 */
static ssize_t ceva_pcidev_write(struct file *filp, const char __user *buf,
		size_t count, loff_t *pos) {
	struct ceva_pcidev *dev = (struct ceva_pcidev *) filp->private_data;
	struct RWBuffer rwbuf;
	ssize_t ret = 0;

	if (!count) {
		return 0;
	}

	if (!access_ok(VERIFY_READ, buf, count)) {
		LOG_E("access error");
		return -EFAULT;
	}

	if (mutex_lock_interruptible(&dev->dev_mutex)) {
		LOG_E("unable to lock mutex");
		return -EINTR;
	}

	// read offset from buf
	if (copy_from_user(&rwbuf, buf, sizeof(rwbuf)) != 0) {
		LOG_E("could not read from user buffer");
		ret = -EFAULT;
		goto out;
	}

	// write content of buf to bar 0
	if (copy_from_user(((char *)dev->bar[0]) + rwbuf.offset,
			rwbuf.buf, count) != 0) {
		LOG_E("%s -> copy from user failed", __FUNCTION__);
		ret = -EFAULT;
		goto out;
	}

	*pos += count;
	ret = count;

out:
	mutex_unlock(&dev->dev_mutex);
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
static long ceva_pcidev_ioctl(struct file *filp, unsigned int cmd,
		unsigned long arg) {

	int ret = 0;
	struct ceva_pcidev *dev = (struct ceva_pcidev *) filp->private_data;
	struct ceva_linkdrv* linkdrv = &dev->linkdrv;

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
		LOG_D("generating interrupt %lx", arg);
		ret = ceva_link_generate_irq(dev, arg);
		if (!ret) {
			DEBUG_INFO_INC(linkdrv, generated_interrupts);
		} else {
			DEBUG_INFO_INC(linkdrv, generate_interrupt_failures);
		}
		break;

	case IOC_CEVADRV_PRINT_DMA: {
#ifdef ENABLE_DEBUG
			struct ceva_pcidev_dma_buf *buf = &dev->dma_buf;
			int off = (int)arg;
			int* addr = (int*)arg;
			int* p = buf->cpu_addr;
#endif // ENABLE_DEBUG

			LOG_I("IOC_CEVADRV_PRINT_DMA :: dma address @ 0x%p -> 0x%x",
					addr, p[off]);
		}
		break;

	default:
		ret = ceva_linkdrv_ioctl(linkdrv, cmd, arg);
	}

	return ret;
}

static void ceva_pcidev_dma_vma_open(struct vm_area_struct *vma) {
	struct ceva_pcidev_dma_buf* buf;
	struct ceva_pcidev* dev;

	LOG_I("ceva_pcidev_dma_vma_open: virt %lx, phys %lx",
	            vma->vm_start, vma->vm_pgoff << PAGE_SHIFT);

	buf = vma->vm_private_data;
	dev = buf->priv_data;
}

static void ceva_pcidev_dma_vma_close(struct vm_area_struct *vma) {
	struct ceva_pcidev_dma_buf* buf;
	struct ceva_pcidev* dev;

	buf = vma->vm_private_data;
	dev = buf->priv_data;
}

/*!
 * mmap handler for dma memory
 *
 * this function remap the internal dma memory that was mapped during
 * probe function to userland.
 * @param [in] dev device driver object
 * @param [in] vma vma object
 * @return zero for success, nonzero for failures
 * @see probe()
 */
static int ceva_pcidev_mmap_dma(struct ceva_pcidev *dev,
		struct vm_area_struct *vma) {
	struct ceva_pcidev_dma_buf *buf = &dev->dma_buf;

#ifdef ENABLE_DEBUG
	static int count = 0;
#endif // ENABLE_DEBUG

	LOG_I("ceva_pcidev_mmap_dma: setting vma");

	vma->vm_ops = &ceva_pcidev_dma_vma_ops;
	vma->vm_private_data = buf;
	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

	ceva_pcidev_dma_vma_open(vma);

	LOG_I("ceva_pcidev_mmap_dma: enter #%d", ++count);

	LOG_I("ceva_pcidev_mmap_dma: remap started, cpu_addr=%p, size=%lu",
			buf->cpu_addr, vma->vm_end - vma->vm_start);

	if (remap_pfn_range(vma,
			vma->vm_start,
			vmalloc_to_pfn(buf->cpu_addr),
			vma->vm_end - vma->vm_start,
			vma->vm_page_prot)) {
		LOG_E("ceva_pcidev_mmap_dma: remap failed");
		return -EAGAIN;
	}

	LOG_I("ceva_pcidev_mmap_dma: remap succeeded");
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
 * @see ceva_pcidev_mmap_dma()
 */
static int ceva_pcidev_mmap(struct file *filp, struct vm_area_struct *vma) {
	struct ceva_pcidev *dev = (struct ceva_pcidev *) filp->private_data;

	LOG_D("ceva_pcidev_mmap:");
	return ceva_pcidev_mmap_dma(dev, vma);
}

/*!
 * cleanup logical part of this module
 *
 * this function deletes any non-pci related driver's objects (the chardev
 * properties).
 * @param [in] dev device handler
 */
static void ceva_pcidev_cleanup_module(struct ceva_pcidev* dev) {

	dev_t dev_sn = MKDEV(ceva_pcidev_major, CEVA_PCIDEV_DEFAULT_MINOR_START);

	cdev_del(&dev->cdev);
	mutex_destroy(&dev->dev_mutex);
	device_destroy(ceva_pcidev_class, dev_sn);
	if (ceva_pcidev_class) {
		class_destroy(ceva_pcidev_class);
		ceva_pcidev_class = NULL;
	}
	unregister_chrdev_region(dev_sn, 1);
}

/*!
 * buildup the char device of this module
 *
 * this function handles the creation and setup of the char device
 * @param [in] dev device handler
 * @return negative error code is returned on failure
 * @see ceva_linkdrv_init_module()
 */
static int ceva_pcidev_setup_cdev(struct ceva_pcidev *dev) {

	int ret;
	struct device *device = NULL;
	int devno = MKDEV(ceva_pcidev_major, CEVA_PCIDEV_DEFAULT_MINOR_START);

	cdev_init(&dev->cdev, &ceva_pcidev_fops);
	dev->cdev.owner = THIS_MODULE;
	ret = cdev_add(&dev->cdev, devno, 1);
	/* Fail gracefully if need be */
	if (ret < 0) {
		LOG_E("setup_cdev: Error %d adding device", devno);
		goto exit;
	}

	device = device_create(
			ceva_pcidev_class,                /* class */
			NULL,                             /* no parent device */
			devno,
			NULL,                             /* no additional data */
			CEVA_PCIDEV_DEVICE_NAME "%d",
			CEVA_PCIDEV_DEFAULT_MINOR_START);

	if (IS_ERR(device)) {
		ret = PTR_ERR(device);
		LOG_E("setup_cdev: Error %d while trying to create %s%d",
				ret,
				CEVA_PCIDEV_DEVICE_NAME,
				CEVA_PCIDEV_DEFAULT_MINOR_START);
		cdev_del(&dev->cdev);
	}
	dev->count = 0;

exit:
	return ret;

}

/*!
 * initialize logical part of this module
 *
 * this function allocate and setup all non-pci related driver's objects
 * (the chardev properties).
 * @param [in] dev device handler
 * @return zero for success, nonzero for failures
 * @see ceva_pcidev_setup_cdev()
 */
static int ceva_linkdrv_init_module(struct ceva_pcidev* dev) {
	dev_t dev_sn;
	int ret;

#ifdef CEVA_LINKDRV_STATIC_MAJOR
	dev_sn = MKDEV(CEVA_LINKDRV_STATIC_MAJOR,
				   CEVA_PCIDEV_DEFAULT_MINOR_START);
	ret = register_chrdev_region(dev_sn, 1,
			CEVA_PCIDEV_DEVICE_NAME);

#else
	ret = alloc_chrdev_region(&dev_sn,
			CEVA_PCIDEV_DEFAULT_MINOR_START,
			1,
			CEVA_PCIDEV_DEVICE_NAME);
#endif

	if (ret < 0) {
		LOG_W("%s: failed to register dev number %d (%d)",
		__FUNCTION__, MAJOR(dev_sn), ret);
		goto exit;
	}

	ceva_pcidev_major = MAJOR(dev_sn);
	LOG_D("Device is up, major: %d of %d devices, starting with minor %d",
			ceva_pcidev_major,
			CEVA_PCIDEV_NUMBER_OF_DEVICES,
			CEVA_PCIDEV_DEFAULT_MINOR_START);

	ceva_pcidev_class = class_create(THIS_MODULE, CEVA_PCIDEV_DEVICE_NAME);
	if (IS_ERR(ceva_pcidev_class)) {
		ret = PTR_ERR(ceva_pcidev_class);
		goto fail;
	}

	mutex_init(&dev->dev_mutex);
	if (ceva_pcidev_setup_cdev(dev) < 0) {
		goto fail;
	}

exit:
	return 0;

fail:
	ceva_pcidev_cleanup_module(dev);
	return ret;
}

/*!
 * get pci device revision
 *
 * @param [in] dev device handelr
 * @return revision number
 */
static inline u8 ceva_pcidev_get_revision(struct pci_dev *dev) {
	u8 revision = 0;

	pci_read_config_byte(dev, PCI_REVISION_ID, &revision);
	return revision;
}

/*!
 * get pci device hw irq number
 *
 * @param [in] dev device handelr
 * @return irq id
 */
static inline u8 ceva_pcidev_get_irq(struct pci_dev *dev) {
	u8 irq = 0;
	pci_read_config_byte(dev, PCI_INTERRUPT_LINE, &irq);
	return irq;
}

/*!
 * find upstream PCIe root node
 *
 * Used for re-training and disabling AER
 * @note based of Altera's sample driver
 * @param [in] dev device handler
 * @return pointer for the root node in case it was found
 * @see probe(), retrain_gen2()
 */
static struct pci_dev* find_upstream_dev(struct pci_dev *dev) {
	struct pci_bus *bus = 0;
	struct pci_dev *bridge = 0;
	struct pci_dev *cur = 0;
	int found_dev = 0;

	bus = dev->bus;
	if (bus == 0) {
		LOG_W("Device doesn't have an associated bus!");
		return 0;
	}

	bridge = bus->self;
	if (bridge == 0) {
		LOG_W("Can't get the bridge for the bus!");
		return 0;
	}

	LOG_D("Upstream device %x/%x, bus:slot.func %02x:%02x.%02x",
			bridge->vendor, bridge->device,
			bridge->bus->number, PCI_SLOT(bridge->devfn),
			PCI_FUNC(bridge->devfn));

	LOG_D("List of downstream devices:");
	list_for_each_entry (cur, &bus->devices, bus_list) {
		if (cur != 0) {
			LOG_D("  %x/%x", cur->vendor, cur->device);
			if (cur == dev) {
				found_dev = 1;
			}
		}
	}

	if (found_dev) {
		return bridge;
	} else {
		LOG_W("Couldn't find upstream device!");
		return 0;
	}
}

/*!
 * check link speed and retrain it to gen2 speeds
 *
 * after reprogramming, the link defaults to gen1 speeds for some reason.
 * doing re-training by finding the upstream root device and telling it
 * to retrain itself. Doesn't seem to be a cleaner way to do this
 * @note based of Altera's sample driver
 * @param [in] dev PCIe device handler (allocated by the bsp)
 * @param [in] ceva_linkdrv device handler
 * @see probe()
 */
static inline void retrain_gen2(struct pci_dev *dev,
		struct ceva_pcidev* ceva_linkdrv) {

	u16 linkstat, speed, width;
	struct pci_dev *upstream;
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

	pos = pci_find_capability(dev, PCI_CAP_ID_EXP);
	if (!pos) {
		LOG_W("Can't find PCI Express capability!");
		return;
	}

	/* Find root node for this bus and tell it to retrain itself. */
	upstream = ceva_linkdrv->upstream;
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
	pci_read_config_word(dev, pos + PCI_EXP_LNKSTA, &linkstat);
	pci_read_config_dword (upstream, link_cap_reg, &link_cap);
	speed = linkstat & PCI_EXP_LNKSTA_CLS;
	width = (linkstat & PCI_EXP_LNKSTA_NLW) >> PCI_EXP_LNKSTA_NLW_SHIFT;

	ceva_linkdrv->pci_gen = (speed == LINKSPEED_5_0_GB) ? 2 : 1;
	ceva_linkdrv->pci_num_lanes = width;

	if (speed == LINKSPEED_2_5_GB) {
		LOG_D( "Link is operating at 2.5 GT/s with %d lanes. Need to retrain",
				width);
	} else if (speed == LINKSPEED_5_0_GB) {
		LOG_D( "Link is operating at 5.0 GT/s with %d lanes.", width);
		if (width == 4) {
			LOG_D( "  All is good!");
			return;
		} else {
			LOG_D( "  Need to retrain.");
		}
	} else {
		LOG_W("Not sure what's going on. Retraining.");
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
		LOG_D( "Error: Link training timed out.");
		LOG_D( "PCIe link not established.");
	}
	else {
		LOG_D( "Link training completed in %d ms.", timeout);
	}
	/* Verify that it's a 5 GT/s link now */
	pci_read_config_word (dev, pos + PCI_EXP_LNKSTA, &linkstat);
	pci_read_config_dword (upstream, link_cap_reg, &link_cap);
	speed = linkstat & PCI_EXP_LNKSTA_CLS;
	width = (linkstat & PCI_EXP_LNKSTA_NLW) >> PCI_EXP_LNKSTA_NLW_SHIFT;

	ceva_linkdrv->pci_gen = (speed == LINKSPEED_5_0_GB) ? 2 : 1;
	ceva_linkdrv->pci_num_lanes = width;

	if(speed == LINKSPEED_5_0_GB) {
		LOG_D("Link operating at 5 GT/s with %d lanes", width);
	} else {
		LOG_W("Link training failed. Link at 2.5 GT/s with %d lanes.", width);
	}
}

/*!
 * enable the PCIe device and gain access to it's memory regions
 *
 * @param [in] pci_dev PCIe device handler
 * @return zero for success, nonzero for failures
 * @see probe()
 */
static inline int probe_enable_pci_dev(struct pci_dev *pci_dev) {

	int ret;

	ret = pci_enable_device(pci_dev);
	if (unlikely(ret != 0)) {
		LOG_E("probe: failed to enable pci\n");
		goto out;
	}

	pci_set_master(pci_dev);
	// ret = pci_try_set_mwi(pci_dev);
	if (unlikely(ret != 0)) {
		LOG_E("probe: failed to set memory-write-invalidate, (err:%d)\n", ret);
		goto fail_set_mwi;
	}

	ret = pci_request_regions(pci_dev, DRIVER_NAME);
	if (ret != 0) {
		LOG_E("probe: failed to get regions, (err:%d)\n", ret);
		goto fail_regions;
	}

	LOG_D("probe: pci device enabled");

	return ret;

fail_regions:
fail_set_mwi:
	pci_disable_device(pci_dev);
out:
	return ret;
}

/*!
 * disable the PCIe device
 *
 * @param [in] pci_dev PCIe device handler
 * @see remove()
 */
static void remove_disable_pci_dev(struct pci_dev *pci_dev) {
	pci_release_regions(pci_dev);
	pci_disable_device(pci_dev);
}

/*!
 * get PCIe hardware properties (irq, revision)
 * @param [in] pci_dev PCIe device handler
 * @param [in] dev device handler
 * @return zero for success, nonzero for failures
 * @see probe()
 */
static inline int probe_get_hw_prop(struct pci_dev* pci_dev, struct
		ceva_pcidev* dev) {
	dev->hw_irq_id = ceva_pcidev_get_irq(pci_dev);
	dev->hw_revision = ceva_pcidev_get_revision(pci_dev);
	LOG_D("irq id: %d, pci revision: %d", dev->hw_irq_id, dev->hw_revision);
	return 0;
}

/*!
 * generates an interrupt over the device
 *
 * this function checks for device's availability to receive new interrupts
 * and then writes the new interrupt data to the predefined address in bar0
 * @param dev device handler
 * @param e interrupt id to generate
 * @return zero for success, -EBUSY if device's fifo is full
 * @see ceva_pcidev_ioctl(), IOC_CEVADRV_GENERATE_INT
 */
static int ceva_link_generate_irq(struct ceva_pcidev *dev, ceva_event_t e) {

	u32 fifo_stat = 0;
	//u32 fifo_stat = readl(dev->bar[0] + CEVA_HOST_INTERRUPT_FIFO);
	if (!fifo_stat) {
		writel((u32) e, dev->bar[0] + CEVA_HOST_INTERRUPT_FIFO);
		LOG_D("interrupt generated");
		return 0;
	}

	LOG_W("unable to generate interrupt, fifo_stat=%ul", fifo_stat);
	return -EBUSY;
}

/*!
 * scans the PCIe bars to check which is available (debug function)
 * @note available only when ENABLE_DEBUG is defined
 * @return always zero
 */
static inline int scan_bars(struct pci_dev *dev,
		struct ceva_pcidev* ceva_linkdrv)
{
#ifdef ENABLE_DEBUG
	int i;
	for (i = 0; i < ACL_PCI_NUM_BARS; i++) {
		struct resource *resource = &dev->resource[i];
		LOG_D("resource [%d] :: name: %s, flags: %lu, start: %p, end: %p\n"
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
 * @param [in] dev PCIe device handler
 * @param [in/out] device handler
 * @note additional debug warnings/errors are printed when ENABLE_DEBUG
 * is defined
 * @return always zero
 * @see free_bars()
 */
static inline int map_bars(struct pci_dev *dev,
		struct ceva_pcidev* ceva_linkdrv)
{
	int i;
	for (i = 0; i < ACL_PCI_NUM_BARS; i++){
#ifdef ENABLE_DEBUG
		unsigned long bar_start = pci_resource_start(dev, i);
		unsigned long bar_end = pci_resource_end(dev, i);
		unsigned long bar_length = bar_end - bar_start + 1;

		if (!bar_start || !bar_end) {
			LOG_W("could not obtain start/end addresses for BAR #%d(%lu/%lu)",
					i, bar_start, bar_end);
			continue;
		}
		if (bar_length < 1) {
			LOG_E("BAR #%d length is less than 1 byte", i);
			continue;
		}

		/*
		 * map the device memory or IO region to kernel virtual address space
		 */
		ceva_linkdrv->bar[i] = ioremap_nocache(bar_start, bar_length);
#else
		ceva_linkdrv->bar[i] = pci_ioremap_bar(dev, i);
#endif
		if (!ceva_linkdrv->bar[i]) {
		  LOG_W("Could not map BAR #%d.", i);
		  continue;
		}

		LOG_D("BAR[%d] mapped at 0x%p with length %lu.",
				i, ceva_linkdrv->bar[i], bar_length);
	}
	return 0;
}

/*!
 * frees the mapped PCIe memory space allocated by map_bars
 * @param [in] dev PCIe device handler
 * @param [in/out] device handler
 * @see map_bars()
 */
static void free_bars(struct pci_dev *dev, struct ceva_pcidev* ceva_linkdrv) {
	int i;
	for (i = 0; i < ACL_PCI_NUM_BARS; i++) {
		if (ceva_linkdrv->bar[i]) {
			pci_iounmap(dev, ceva_linkdrv->bar[i]);
			ceva_linkdrv->bar[i] = NULL;
		}
	}
}

/*!
 * scans and maps the PCIe memory and i/o regions
 * @param [in] dev PCIe device handler
 * @param [in/out] device handler
 * @return zero for success, nonzero for failure
 * @see probe(), map_bars()
 */
static inline int probe_map_bars(struct pci_dev *pci_dev,
		struct ceva_pcidev* dev) {
	int ret;
	scan_bars(pci_dev, dev);
	ret = map_bars(pci_dev, dev);
	if (!ret) {
		if (!dev->bar[0]) {
			LOG_E("unable to map PCI bar 0");
			ret |= BIT(0);
		}
		if (!dev->bar[1]) {
			LOG_E("unable to map PCI bar 1");
			ret |= BIT(1);
		}
	}

	return ret;
}

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
 * @ see probe(), CEVADEV_PCI_DMA_MEM_SIZE
 */
static inline int probe_map_dma(struct ceva_pcidev* dev) {
	struct ceva_pcidev_dma_buf* dmabuf;
#ifdef ENABLE_DEBUG
	int i, ret;
#endif // ENABLE_DEBUG
	unsigned long mask;
	char *pcie_cra_addr_trans;
	dma_addr_t dma_addr;

	if (!dev) {
		return -EINVAL;
	}
	dmabuf = &dev->dma_buf;

	dmabuf->size = CEVADEV_PCI_DMA_MEM_SIZE;
	dmabuf->priv_data = dev;

	/*
	 * Note: The DMA can not handle > 25bit addresses (32MB)
	 * Note: We couldn't get non-consistent mappings for scatterlists with
	 * 25bits, therefore we disable sg_mapper
	 */
	if (!pci_set_dma_mask(dev->pci_dev, DMA_BIT_MASK(64))) {
		LOG_D("dma mask set to 64 bit");
		if (pci_set_consistent_dma_mask(dev->pci_dev, DMA_BIT_MASK(64))) {
			LOG_E("dma consistent mask set failed (64 bit)");
			pci_set_dma_mask(dev->pci_dev, DMA_BIT_MASK(32));
			goto dma_mask_32;
		}
		LOG_D("dma consistent mask set to 64 bit");
	} else if (!pci_set_dma_mask(dev->pci_dev, DMA_BIT_MASK(32))) {
dma_mask_32:
		LOG_D("dma mask set to 32 bit");
		if (pci_set_consistent_dma_mask(dev->pci_dev, DMA_BIT_MASK(32))) {
			LOG_E("dma consistent mask set failed (32 bit)");
			goto dma_mask_error;
		}
		LOG_D("dma consistent mask set to 32 bit");
	} else {
dma_mask_error:
		LOG_E("probe: failed to set consistent dma mask");
		return -EIO;
	}


	LOG_D("probe_map_dma: dmabuf->size=%x", dmabuf->size);

	dmabuf->cpu_addr = pci_alloc_consistent(dev->pci_dev, dmabuf->size,
			&dmabuf->dma_addr);
	if (dmabuf->cpu_addr == NULL) {
		LOG_E("probe: failed to map dma buffer");
		return -ENOMEM;
	}

	LOG_D("forward dma address (%p) to device", (void*) dmabuf->dma_addr);
/*	for (i = 0; i < (dmabuf->size/CEVA_PCIE_CRA_PAGE_SIZE); i++) {
		unsigned long mask;
		char *pcie_cra_addr_trans = (char*) dev->bar[0] + GET_PCIE_CRA_ADDR(i);
		dma_addr_t dma_addr = dmabuf->dma_addr + (i * CEVA_PCIE_CRA_PAGE_SIZE);
		writel(0xfffffffc, pcie_cra_addr_trans);
		mask = readl(pcie_cra_addr_trans);
		writel(mask & dma_addr, pcie_cra_addr_trans);
		LOG_D("address %lx written to %p (bar0 with %u offset)",
				(long unsigned) dma_addr,
				pcie_cra_addr_trans,
				GET_PCIE_CRA_ADDR(i));
	}
*/

	pcie_cra_addr_trans = (char*) dev->bar[1] + CAVA_PCIE_CRA_ADDR_BASE;
	dma_addr = dmabuf->dma_addr;
	writel(0xfffffffc, pcie_cra_addr_trans);
	mask = readl(pcie_cra_addr_trans);
	writel(mask & dma_addr, pcie_cra_addr_trans);
	//writel(dma_addr, pcie_cra_addr_trans);

	LOG_D("address %lx written to %p (bar1 with %lx offset)",
			(long unsigned) dma_addr,
			pcie_cra_addr_trans,
			CAVA_PCIE_CRA_ADDR_BASE);

	LOG_D("probe: dma mapped, physical: %lxH, address: %pH (size: %zuH)",
			(long unsigned) dmabuf->dma_addr, dmabuf->cpu_addr, dmabuf->size);
	return 0;
}

/*!
 * enable PCIe message signaled interrupts (msi)
 *
 * pci allows up to 32 msi, we try to enable all, in case of failure we
 * update the msis to the allowed number and try again
 * @param [in] pci_dev PCIe device handler
 * @param [in/out] dev device handler
 * @return zero for success, nonzero for failures
 * @note in case this function suceeded, the irq value in pci_dev changes
 * to the number of the the first msi.
 * @see probe(), probe_init_irq(), probe_deinit_irq()
 */
static inline int probe_enable_msi(struct pci_dev *pci_dev,
		struct ceva_pcidev* dev) {

	int ret = 1;

	dev->msis = 32; // TODO remove this (number of msi should not be 32)
	// ret = pci_enable_msi_block(pci_dev, dev->msis);
	if (ret > 0) {
		LOG_W("probe: failed to enable %d msi, retrying with %d",
				dev->msis, ret);
		dev->msis = (unsigned int) ret;
		//ret = pci_enable_msi_block(pci_dev, dev->msis);
	}
	if (ret) {
		LOG_E("probe: failed to enable %d msi", dev->msis);
		return -EIO;
	}
	dev_info(&pci_dev->dev, "msi enabled, %d irqs available from irq %d",
			dev->msis, pci_dev->irq);
	LOG_D("msi enabled, %d irqs available from irq %d",
			dev->msis, pci_dev->irq);
	return 0;
}

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

	struct ceva_pcidev *dev = (struct ceva_pcidev *) dev_id;
	int i, cn;
	u32 irq_status;
	u32 irq_addr;
	u32 irq_data[16];

	LOG_D("interrupt (%d)", irq);

	if (!dev) {
		DEBUG_INFO_INC(&dev->linkdrv, unhandled_irq);
		return IRQ_NONE;
	}

	/*
	 * From this point on, this is our interrupt. So return IRQ_HANDLED
	 * no matter what (since nobody else in the system will handle this
	 * interrupt for us).
	 */
	DEBUG_INFO_INC(&dev->linkdrv, handled_irq);
	// read status to obtain which irq register to read
	irq_status = readl((char*) dev->bar[0] + CEVA_IRQ_STATUS_ADDR);
	irq_status &= 0x0000ffff;
	LOG_D("irq status=%lx", (long unsigned) irq_status);

	for (cn = 0, i = 0; i < 16; i++) {
		if (irq_status & 1<<i) {
			irq_addr = GET_IRQ_DATA_ADDR(i);
			LOG_D("interrupt #%02x is on, reading %p", i, (void *) irq_addr);
			irq_data[cn] = readl((char*) dev->bar[0] + irq_addr);
			LOG_D("interrupt #%02x, address offset %p, data = %lx",
					i,
					(char*) dev->bar[0] + irq_addr,
					(long unsigned int) irq_data[cn]);
			cn++;
		}
	}

	// send to linkdrv layer
	LOG_D("sending %d events to link layer", cn);
	ceva_linkdrv_broadcast_events(&dev->linkdrv, irq_data, cn);
	return IRQ_HANDLED;
}

/*!
 * enable PCIe irq
 * @param [in] pci_dev PCIe device handler
 * @param [in] dev device handler
 * @return zero for success, nonzero for failures
 * @see probe()
 */
static int probe_init_irq(struct pci_dev *pci_dev,
		struct ceva_pcidev* dev) {
	int ret = request_irq(pci_dev->irq,
						  ceva_linkdrv_interrupt,
						  0,
						  DRIVER_NAME, dev);
	if (ret != 0) {
		LOG_E("Could not request IRQ #%d, error %d", pci_dev->irq, ret);
		return -EIO;
	}
	pci_write_config_byte(pci_dev, PCI_INTERRUPT_LINE, pci_dev->irq);
	LOG_D("Successfully requested IRQ #%d", pci_dev->irq);
	return 0;
}

/*!
 * disable PCIe irq
 * @param [in] pci_dev PCIe device handler
 * @param [in] dev device handler
 * @see remove()
 */
static void remove_deinit_irq(struct pci_dev *pci_dev,
		struct ceva_pcidev* dev) {
	free_irq(pci_dev->irq, dev);
	LOG_D("irq freed");
}

/*!
 * PCIe probe function
 *
 * this function enables and starts CEVA PCIe device
 * @param [in/out] pci_dev PCIe device handler
 * @param [in] id not used
 * @return zero for success, nonzero for failures
 * @see find_upstream_dev(), retrain_gen2(), probe_get_hw_prop(),
 * probe_map_bars(), probe_map_dma(), probe_enable_msi(),probe_init_irq(),
 * ceva_linkdrv_init_module(), remove()
 */
static int probe(struct pci_dev *pci_dev,
		const struct pci_device_id *id) {
	int ret = 0;
	struct ceva_pcidev* dev = NULL;
	struct ceva_pcidev_dma_buf* dmabuf = NULL;

	LOG_D("device probed (dev = 0x%p, pciid = 0x%p)", pci_dev, id);
	LOG_D("vendor = 0x%x, device = 0x%x, class = 0x%x, "
			"bus:slot.func = %02x:%02x.%02x",
			pci_dev->vendor, pci_dev->device, pci_dev->class,
			pci_dev->bus->number, PCI_SLOT(pci_dev->devfn),
			PCI_FUNC(pci_dev->devfn));

	dev = kzalloc(sizeof(struct ceva_pcidev), GFP_KERNEL);
	if (unlikely(!dev)) {
		ret = -ENOMEM;
		LOG_E("probe: allocation error");
		goto alloc_fail;
	}

	dev->pci_dev = pci_dev;
	pci_set_drvdata(pci_dev, dev);
	dev->pci_gen = 0;
	dev->pci_num_lanes = 0;

	dev->upstream = find_upstream_dev(pci_dev);
	retrain_gen2(pci_dev, dev);

	/*
	 * enable pci device
	 */
	ret = probe_enable_pci_dev(pci_dev);
	if (unlikely(ret != 0)) {
		goto enable_fail;
	}

	/*
	 * get pci hardware properties
	 */
	probe_get_hw_prop(pci_dev, dev);
	/*
	 * map bars memory
	 */
	ret = probe_map_bars(pci_dev, dev);
	if (unlikely(ret != 0)) {
		goto map_fail;
	}
	/*
	 * map dma memory
	 */
	ret = probe_map_dma(dev);
	if (unlikely(ret != 0)) {
		goto map_dma_fail;
	}

	/*
	 * setup message signaled interrupt
	 */
	ret = probe_enable_msi(pci_dev, dev);
	if (unlikely(ret != 0)) {
		goto enable_msi_fail;
	}
	/*
	 * request irq
	 */
	ret = probe_init_irq(pci_dev, dev);
	if (unlikely(ret != 0)) {
		goto init_irq_fail;
	}

	/*
	 * logical module init
	 */
	ret = ceva_linkdrv_init_module(dev);
	if (unlikely(ret != 0)) {
		LOG_E("probe: failed init logic module");
		goto logic_fail;
	}

	/*
	 * probed successfully
	 */
	LOG_D("CEVA device 0x%x probed and enabled at bus %d dev %d func %d",
			pci_dev->device, pci_dev->bus->number, PCI_SLOT(pci_dev->devfn),
			PCI_FUNC(pci_dev->devfn));

	return ret;

logic_fail:
	remove_deinit_irq(pci_dev, dev);
init_irq_fail:
	// pci_disable_msi(pci_dev);
enable_msi_fail:
map_dma_fail:
	dmabuf = &dev->dma_buf;
	if (dmabuf->cpu_addr) {
		pci_free_consistent(dev->pci_dev, dmabuf->size, dmabuf->cpu_addr,
				dmabuf->dma_addr);
	}

map_fail:
	free_bars(pci_dev, dev);
	remove_disable_pci_dev(pci_dev);

enable_fail:
	kfree(dev);

alloc_fail:
	LOG_E("probe: failed (ret=%d)", ret);
	return ret;
}

/*!
 * PCIe remove function
 *
 * this function stops and disables the CEVA PCIe device
 * @param [in/out] pci_dev PCIe device handler
 * @see ceva_pcidev_cleanup_module(), remove_deinit_irq(), pci_disable_msi(),
 * free_bars(), remove_disable_pci_dev(), probe()
 */
static void remove(struct pci_dev *pci_dev) {
	struct ceva_pcidev* dev;
	struct ceva_pcidev_dma_buf* buf;

	dev = pci_get_drvdata(pci_dev);
	if (unlikely(!dev))
		goto pci_disable;

	ceva_pcidev_cleanup_module(dev);
	pci_set_drvdata(pci_dev, NULL);

	/*
	 * disable interrupt
	 */
	remove_deinit_irq(pci_dev, dev);

	/*
	 * disable msi
	 */
	// pci_disable_msi(pci_dev);

	/*
	 * free dma buffer
	 */
	buf = &dev->dma_buf;
	if (buf->cpu_addr) {
		pci_free_consistent(dev->pci_dev, buf->size, buf->cpu_addr,
				buf->dma_addr);
	}

	/*
	 * free mapped bars
	 */
	free_bars(pci_dev, dev);

	kfree(dev);

pci_disable:
	remove_disable_pci_dev(pci_dev);
	LOG_I("Removed device 0x%x at bus %d dev %d func %d.",
			pci_dev->device, pci_dev->bus->number, PCI_SLOT(pci_dev->devfn),
			PCI_FUNC(pci_dev->devfn));
}

/*!
 * driver't init function
 *
 * this function register the CEVA's supported PCI ids with
 * this driver.
 * it also inits the protected shared memory module.
 * @see ceva_pcidev_exit_module
 */
int __init ceva_pcidev_init_module(void) {

	LOG_D("module init");

	if (protected_mem_db_init() != 0) {
		LOG_E("failed to init protected memory db");
		return -ENODEV;
	}

	if (pci_register_driver(&ceva_pcidev_driver) != 0) {
		LOG_E("failed to register driver");
		protected_mem_db_deinit();
		return -ENODEV;
	}

	/* Load all board-related constants */
	LOG_D("device registered");
	return 0;
}

/*!
 * driver's deinit function
 *
 * this function unregister the CEVA's supported PCI ids with
 * this driver.
 * it also de-inits the protected shared memory module.
 * @see ceva_pcidev_init_module
 */
void __exit ceva_pcidev_exit_module(void) {
	protected_mem_db_deinit();
	pci_unregister_driver(&ceva_pcidev_driver);
	LOG_I("driver unregistered");
}

module_init(ceva_pcidev_init_module);
module_exit(ceva_pcidev_exit_module);
