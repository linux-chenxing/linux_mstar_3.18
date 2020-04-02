/**********************************************************************
 * copyright (C) 2012-2014 Cadence Design Systems
 * All rights reserved.
 ***********************************************************************
 * cpd_lnx_drv.c
 *
 * Cadence PCIe linux Driver
 * This code implements the PCIe "subsystem" for the Cadence Databahn
 * PCIe 3.0 Core. The PCIe 3.0 Core is directly attached to the "system"
 * bus, and the Core is memory-mapped to be directly addressable by the
 * main CPU.
 *
 * The PCIe subsystem is comprised of a Root Port that is directly
 * addressable by the CPU and the Root Port "exports" a PCIe peripheral
 * bus on the other side. The Root Port is a "platform" device and
 * is part of the board-hardware. In a traditional manner of integration,
 * the Root Port driver would be initialized and brought up as part
 * of the boot-up process of the "platform" as part of the OS initialization.
 *
 * In this case, the Root Port driver is made to be dynamically loadable
 * as a kernel module. Thus, even though the Root Port Driver is part
 * of the platform, it is brought up as a loadable module *after* the
 * Linux system is already up and running. The Driver has been implemented
 * in such a way for test, debug and diagnostic support for the Core.
 *
 * The Root Port Driver also exports a character-device interface for
 * test, debug and diagnostic functionality. This interface is not
 * to be used during normal operation.
 *
 * Cadence ships a "core-driver" that may be used as an abstraction and
 * low-level-driver layer when configuration of the core is required.
 *
 * The Cadence Core Driver architecture relies on an Object model to
 * provide access to "api" routines that can be used to perform different
 * functions required by the driver. For PCIe, this interface is
 * termed CPDI or the "Cadence PCIe Driver Interface".
 *
 * The CPDI_OBJ struct is used to contain a list of apis. Additional
 * details about this interface and associated routines may be found at
 * other places in the documentation set that ships with the Core.
 *
 * The CPDI_OBJ struct and associated programming model includes the
 * use of a "probe" and an "init" routine (in addition to others) which
 * may be used by the current driver to get information about the amount
 * of memory required by the Core driver, etc.
 *
 * For example:
 *      CPDI_InitParam rc_init_params;
 *      void * p_rc_data;
 *
 *      // Get the CPDI object
 *      CPDI_OBJ * obj = CPDI_GetInstance ();
 *
 *      // Call "probe"
 *      obj->probe (CPDI_RP_LM_BASE, & sysReq);
 *
 *      // Allocate the memory required by the Core driver
 *      p_rc_data = kmalloc (sysReq.pdataSize, GFP_KERNEL);
 *
 *      // Call "init"
 *      obj->init (p_rc_data, CPDI_RP_LM_BASE, & rc_init_params);
 *
 *      // Call required functionality (RP BAR Aperture settings for example)
 *      obj->accessRootPortBarApertureSetting (p_rc_data, BAR_01, READ,...);
 *
 * Additional documentation and sample code for this interface is
 * included in the documentation-set shipped by Cadence.
 *
 ***********************************************************************/


#include <linux/types.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>

#include <linux/fs.h>

#include <linux/module.h>

#include <asm/uaccess.h>
#include <asm/mach/pci.h>
#include <mach/io.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/ioport.h>
#include "chip_int.h"
#include "../../../drivers/pci/pci.h"

/*
#ifdef MODULE
extern struct module __this_module;
#define THIS_MODULE (&__this_module)
#else
#define THIS_MODULE ((struct module *)0)
#endif
*/
/* Define start addresses for PCIE registers */
//#include <mach/pcie_addr.h>

/* Defines the RP Vendor Device ID */
#include "pcie.h"
#include "cpdi.h"
#include "mstar_pcie_addrmap.h"
#include "mstar_pcie_axi2mi_bridge_reg.h"
#include "mstar_pcie_rc_reg.h"


/*-------------------------------------------------------------------------------*/
/* globals                                                                       */
/*-------------------------------------------------------------------------------*/
u32 g_rc_data [16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
CPDI_SysReq     sysReq;
struct task_struct *polling_localerr_task =  NULL;
extern int _mdrv_RC_ob_memory_init(void *pD, CPDI_BarControl control_val, CPDI_BarApertureSize aperture_val, CPDI_EnableOrDisable check_enable);
extern int _mdrv_RC_ob_mem_init_resume(void);

#ifdef DEBUG_RUNTIME_LOCAL_ERROR
struct Mstar_LocalError_Descriptor  localerror_count = {0};
#endif
/**
 * Driver name is "DATABAHN_PCIE"
 */
#define DRVNAME    "Mstar-pcie"


/**
 * "DATABAHN_PCIE" major number
 */
#define DATABAHN_PCIE_MAJOR_NUMBER     199


/**
 * "DATABAHN_PCIE" name of device for chrdev access
 */
#define DATABAHN_PCIE_CHRDEV_NAME        "cpd_lnx"

/**
 * Invalidate cache using Linux DMA sync routines
 * @param[in] address Virtual address of buffer to sync. Buffer must
 *    be allocated using dma_map_single() function
 * @param[in] size Size of buffer
 * @param[in] device Pointer to struct device* which owns buffer
 */
void CPS_CacheInvalidate(uintptr_t address, size_t size, uintptr_t device) {
    uintptr_t phys_address = __pa(address);
    struct device *dev = (struct device *) device;
    dma_sync_single_for_cpu(dev, phys_address, size, DMA_BIDIRECTIONAL);
}

/**
 * Flush cache using Linux DMA sync routines
 * @param[in] address Virtual address of buffer to sync. Buffer must
 *    be allocated using dma_map_single() function
 * @param[in] size Size of buffer
 * @param[in] device Pointer to struct device* which owns buffer
 */
void CPS_CacheFlush(uintptr_t address, size_t size, uintptr_t device) {
    uintptr_t phys_address = __pa(address);
    struct device *dev = (struct device *) device;
    dma_sync_single_for_device(dev, phys_address, size, DMA_BIDIRECTIONAL);
}

/**
 * Forward Declarations for function pointers populated into
 * the pci_ops structure.
 */
int cpdLnx_ReadConf  (struct pci_bus *bus, u32 devfn, int where, int size, u32 *val);
int cpdLnx_WriteConf (struct pci_bus *bus, u32 devfn, int where, int size, u32  val);

/**
 * prototype for print ioctl function
 */
void cpdLnx_PrintIoctlParam (void);

/**
 * PCIe OPS struct
 * The pci_ops struct contains function pointers for Linux to
 * be able to run config-cycles to devices connected on the PCIe
 * fabric. These routines are used for "pci_scan_bus" and by
 * "pciauto_assign_resources" to access the configuration space
 * exported by the device & its function(s).
 */
static struct pci_ops cpdLnx_Ops = {

    .read  = cpdLnx_ReadConf ,
    .write = cpdLnx_WriteConf,
};


static struct resource pci_mem_space = {
    .name  = "PCIE IO Memory Space",
    .start = MSTAR_PCIE_IOMEM_BASE,
    .end   = MSTAR_PCIE_IOMEM_BASE + MSTAR_PCIE_IOMEM_LEN - 1,
    .flags = IORESOURCE_MEM,

};

/**
 * Globals
 */
static struct pci_sys_data * cpdLnx_gPciSysPtr = NULL;
static struct pci_bus      * cpdLnx_gPciBusPtr = NULL;

#ifdef CONFIG_MSTAR_PCIE_OB_MSG_ENABLE
#define MESSAGE_CODE_USAGE_NUMBER 19
uint32_t MSG_Vendor_Define_Test_All(void *pD, uint32_t msg_code)
{
    uint32_t ret = 0;

    ret = _mdrv_RC_ob_message_write(pD, MSTAR_PCIE_OB_MSG_REG_START, msg_code, 0x80); // Routing => 000, 010, 011 or 100
    if(ret != 0)
    {
        pr_info ("     MSG_Vendor_Define_Test_All() Fail at msg_code = 0x7E, msg_routing = 0x80 !\n");
        return ret;
    }

    ret = _mdrv_RC_ob_message_write(pD, MSTAR_PCIE_OB_MSG_REG_START, msg_code, 0x60);
    if(ret != 0)
    {
        pr_info ("     MSG_Vendor_Define_Test_All() Fail at msg_code = 0x7E, msg_routing = 0x60 !\n");
        return ret;
    }

    ret = _mdrv_RC_ob_message_write(pD, MSTAR_PCIE_OB_MSG_REG_START, msg_code, 0x40);
    if(ret != 0)
    {
        pr_info ("     MSG_Vendor_Define_Test_All() Fail at msg_code = 0x7E, msg_routing = 0x40 !\n");
        return ret;
    }

    ret = _mdrv_RC_ob_message_write(pD, MSTAR_PCIE_OB_MSG_REG_START, msg_code, 0x00);
    if(ret != 0)
    {
        pr_info ("     MSG_Vendor_Define_Test_All() Fail at msg_code = 7E, msg_routing = 0x00 !\n");
        return ret;
    }

    return 0;
}

uint32_t MSG_OutBound_Test_All(void *pD)
{
    uint32_t i, retval, msg_region = 2;
    uint32_t msg_code[MESSAGE_CODE_USAGE_NUMBER]    = {0x00 , 0x10, 0x12, 0x14, 0x18, 0x19, 0x1B, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x30, 0x31, 0x33, 0x50};
    uint32_t msg_routing[MESSAGE_CODE_USAGE_NUMBER] = {0x60 , 0x80, 0x80, 0x80, 0x00, 0x60, 0xA0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x80};

    for(i=0 ; i<MESSAGE_CODE_USAGE_NUMBER ; i++)
    {
        retval = _mdrv_RC_ob_message_write(pD, msg_region, msg_code[i], msg_routing[i]);
        if(retval != 0)
        {
            pr_info ("     MSG_OutBound_Test_All() Fail at msg_code = %x, msg_routing = %x !\n", msg_code[i], msg_routing[i]);
            break;
        }
    }

    return 0;
}
#endif

/**
 * hw PCIe scan bus. This routine is called by Linux when Linux
 * wants to request a scan of the PCI bus. A pointer to this
 * routine is stored in the "hw_pci" structure and passed to
 * Linux by calling the "pci_common_init" function.
 * @param[in]  nr   PCI bus number for which the scan is to run
 * @param[in]  sys  pointer to the "pci_sys_data" struct
 * @return          pointer to the "pci_bus" struct returned by
 *                  "pci_scan_bus"
 */
static struct pci_bus * cpdLnx_ScanBus (int nr,
                                        struct pci_sys_data *sys) {

    /**
     * call Linux to scan the pci bus
     */

    cpdLnx_gPciBusPtr = pci_scan_root_bus(NULL, sys->busnr, &cpdLnx_Ops, sys,
				 &sys->resources);

    //cpdLnx_gPciBusPtr = pci_scan_root_bus(NULL, nr, &cpdLnx_Ops, sys,
    //     &sys->resources);
    //pr_info ("     Pci_scan_bus called\n");

    /**
     * save the pci sys pointer for later
     * This stored value may not be used but is stored
     * for legacy reasons.
     */
    cpdLnx_gPciSysPtr = sys;

    /**
     * return the pointer to the "pci_bus" struct
     * returned by "pci_scan_bus"
     */
    return cpdLnx_gPciBusPtr;
}


/**
 * hw PCIe setup pci hardware for access by Linux. This routine
 * doesn't do much as we do not rely on this driver to do much
 * setup for the PCIe hardware.
 * @param[in]  nr   PCI bus number for which the scan is to run
 * @param[in]  sys  pointer to the "pci_sys_data" struct
 */
static int cpdLnx_Setup (int nr,
                         struct pci_sys_data * sys)
{
	printk("[PCIE] setup iomem resource, 0x%x - 0x%x\n",
		pci_mem_space.start, pci_mem_space.end);
    if(request_resource(&iomem_resource, &pci_mem_space))
	{
		printk("cpdLnx_Setup() PCIE Request resouce Fail \n");
	}
	else
	{
    pci_add_resource_offset(&sys->resources , &pci_mem_space, sys->mem_offset);
	}

    return 1;
}


/**
 * hw PCIe mapping of the Irq to a slot and pin.
 * This routine doesn't do anything as we do not rely on this
 * driver to handle interrupts.
 * @param[in]  dev  Pointer to "pci_dev" structure
 * @param[in] slot  Slot for this device
 * @param[in] pin   Pin that the interrupt should be generated on
 */
static int cpdLnx_MapIrq (const struct pci_dev * dev,
                          u8 slot,
                          u8 pin)
{
    pr_info("     cpdLnx_MapIrq() Bus number = %d , slot number = %d , pin number = %d\n", dev->bus->number, slot, pin);
    if(dev->bus->number != 0)
    {

#ifdef PCIE_3LAYER_INTERRUPT
        if(pin == 1)
          return (PCIE_IRQ_INTA);
        else if(pin == 2)
          return (PCIE_IRQ_INTB);
        else if(pin == 3)
          return (PCIE_IRQ_INTC);
        else if(pin == 4)
          return (PCIE_IRQ_INTD);
#else
        return (E_IRQ_INT_PCIE_RC);
#endif
    }

    return 0;
}


/**
 * "hw_pci" struct. This structure is passed to Linux by calling
 * "pci_common_init". This structure tells contains pointers to
 * functions that tell Linux about the functions that may be called
 * to setup hardware or to perform a PCI bus scan for devices.
 */
static struct hw_pci cpdLnx_DatabahnPCIe = {
    .nr_controllers  = 1             ,
    .setup           = cpdLnx_Setup  ,
    .scan            = cpdLnx_ScanBus,
    .preinit         = NULL          ,
    .postinit        = NULL          ,
    .map_irq         = cpdLnx_MapIrq ,
};


/**
 * Platform driver probe function that is called by Linux to find out
 * if the Databahn Root Port Device exists within the platform. This
 * routine is part of the "platform_driver" structure that is passed
 * to Linux by called the "platform_driver_register" call. After the
 * registration process happens, Linux uses this routine to find out
 * if the actual Root Port hardware exists in the system.
 * @param[in]   pdev   Pointer to "platform_device" structure
 * @return        0    upon success
 * @return     -EINVAL If function was unable to find the Root Port
 *                     platform device as expected at a fixed virtual
 *                     address
 */
static int cpdLnx_Probe (struct platform_device * pdev) {

    u32 value;
    void __iomem * addr = (void __iomem *) CPDREF_RP_BASE;

    value = readl (addr);

    //pr_info("     Probe for Cadence Databahn PCIe Root Port\n");

    /**
     * Something is wrong if we weren't able to find
     * the Root Port with the system. The Root Port cannot
     * be "hot-plugged" into the system and must exist at the fixed
     * address. Existance is confirmed by comparing the Vendor-ID
     * for the PCIe Root Port.
     */
    if (value != CPDI_DEFAULT_VENDOR_DEVICE_ID)
        return -EINVAL;

    /**
     * The Root Port has been found
     */
    //pr_info("     Found Root Port; now initializing Linux PCI\n");


    /**
     * Call "pci_common_init" to tell Linux
     * to initialize PCI and about how to call into this
     * driver for functionality such as setting up hardware or
     * scanning the PCIe bus
     */
    pci_common_init (& cpdLnx_DatabahnPCIe);

    //pr_info("     Pci_common_init called\n");

    return 0;
}

static int cpdLnx_Suspend (struct platform_device * pdev, pm_message_t state){

    pr_info("[PCIE] Platform suspend...\n");

#if 0  //20161027, no need to remove pci bus
    pci_stop_root_bus(cpdLnx_gPciBusPtr);
    pci_remove_root_bus(cpdLnx_gPciBusPtr);
#endif

    return 0;
}

static int cpdLnx_Resume (struct platform_device * pdev){

    pr_info("[PCIE] Platform resume...\n");

#if 0  //20161027, do resume procedure in mstar_pci_set_power_state()
    uint32_t ret;

    ret = _mdrv_pcie_resume();
    if(ret != 0)
    {
        pr_info ("     cpdLnx_Resume() :_mdrv_pcie_resume () Fail !\n");
        return ret;
    }

    cpdLnx_gPciBusPtr = pci_scan_bus (0, & cpdLnx_Ops, cpdLnx_gPciSysPtr);
    pci_bus_add_devices(cpdLnx_gPciBusPtr);
    pci_assign_unassigned_bus_resources(cpdLnx_gPciBusPtr);
    pci_common_init (&cpdLnx_DatabahnPCIe);

    //After PCIe driver OK, we set up _mdrv_RC_ob_memory_init for BAR
    ret = _mdrv_RC_ob_memory_init(g_rc_data, CPDI_NON_PREFETCHABLE_32_BIT_MEM_BAR, CPDI_APERTURE_SIZE_1M, CPDI_ENABLE_PARAM);
    if(ret == 0)
      pr_info("     RC Outbound memory Init Done\n");
    else
      pr_info("     RC Outbound memory Init Fail !\n");

#endif
    return 0;
}

/**
 * The "platform_driver" structure. This structure tells Linux
 * how to find the PCIe Root Port within the system. The Root
 * Port is the platform device that is directly addressable by
 * the processor (i.e., it is not a peripheral device). Other
 * devices on the PCIe fabric are accessible through the PCIe
 * bus(fabric), and any accesses to those devices must go through
 * the Root Port. The main member of this structure is "probe"
 * which tells Linux if the Root Port is available to be
 * accessed within the system.
 */
static struct platform_driver cpdLnx_PCIePlatformDriver = {

    .driver = {
        .name  = DRVNAME,
        .owner = THIS_MODULE,
    },
    .probe = cpdLnx_Probe,
    .suspend = cpdLnx_Suspend,
    .resume  = cpdLnx_Resume,
};

static pci_power_t mstar_pci_choose_state(struct pci_dev *pdev)
{
	printk("%s\n", __func__);

	return PCI_D0;
}

static bool mstar_pci_power_manageable(struct pci_dev *dev)
{
	printk("%s, dev: %x, ven: %x\n", __func__, dev->device, dev->vendor);

	if ( ( ((u32) dev->device << 16) | (u32) dev->vendor ) != CPDI_DEFAULT_VENDOR_DEVICE_ID )
		return false;
	else
		return true;
}

static int mstar_pci_set_power_state(struct pci_dev *dev, pci_power_t state)
{
	uint32_t ret;

	printk("%s, state_saved %d, state: %d\n", __func__, dev->state_saved, state);
	if ( ( ((u32) dev->device << 16) | (u32) dev->vendor ) != CPDI_DEFAULT_VENDOR_DEVICE_ID )
		return 0;

	if ( dev->state_saved && state == PCI_D0 )
	{
		printk("[PCIE] Resume... \n");
		ret = _mdrv_pcie_resume();
		if(ret != 0)
		{
			printk("[PCIE] _mdrv_pcie_resume () Fail !\n");
			return ret;
		}

		ret = _mdrv_RC_ob_mem_init_resume();
		if(ret == 0)
		{
			printk("[PCIE] OB mem Init Done\n");
		}
		else
		{
			printk("[PCIE] OB mem Init Failed\n");
		}

		return 0;

	}

	return 0;
}

static int mstar_pci_sleep_wake(struct pci_dev *dev, bool enable)
{
	printk("%s, enable: %d\n", __func__, enable);

	return 0;
}

static struct pci_platform_pm_ops mstar_pci_platform_pm = {
	.is_manageable = mstar_pci_power_manageable,
	.set_state = mstar_pci_set_power_state,
	.choose_state = mstar_pci_choose_state,
	.sleep_wake = mstar_pci_sleep_wake,
};

/**
 * The "ioctl" parameter structure that is used to exchange
 * information and commands with the driver from a user application.
 * This structure is used by an application for test and diagnostic
 * functionality only and should not be used by the application during
 * normal operation. For test and diagnostics, this structure is used
 * to exchange information between user and kernel spaces.
 */
typedef struct {
    bool b_power_onoff;
    bool b_ASPM_onoff;
    bool b_legacy_onoff;
    bool b_thread_onoff;
    unsigned short vendor_id;
    unsigned short device_id;
    int irq;
    int num_instances;
    unsigned long dev_p;
    unsigned long pci_sys_p;
    unsigned long bar_start [5];
    unsigned long bar_length [5];
    unsigned long bar_flags [5];
    unsigned long write_addr;
    unsigned long write_data;
    unsigned long repeat_time;
    unsigned long time_slice;
    unsigned long ob_dma_offset;
}
cpdLnx_IoctlParam;


/**
 * Ioctl commands that can be involked by an application to get the
 * driver to perform certain operations for test, debug and
 * diagnostic functionality only. These commands are not designed
 * to be used during normal operation.
 */
enum {
    /**
     * This "ioctl" command is involked by an application to find
     * a device on the PCIe fabric. This command only allows for
     * visibility into what Linux sees. I.e., if the device is
     * part of the Linux PCI device tree, then this command will
     * be successful in finding the device, else it will not.
     */
    cpdLnx_IOCTL_FIND_DEVICE,
    /**
     * This "ioctl" command is used to read BAR registers.
     * This command is used purely for test and diagnostic purposes
     * and performs very specific functionality not to be generally
     * used
     */
    cpdLnx_IOCTL_READ_BARS,
    /**
     * Placeholder: does nothing
     */
    cpdLnx_DUMMY,
    /**
     * This "ioctl" command is used to force a "rescan" of the
     * PCI bus. This routine is also used for test and diagnostic
     * purposes only and not designed for normal operation of
     * the driver
     */
    cpdLnx_IOCTL_DO_RESCAN,
    /**
     * This "ioctl" command is used to force a "class code" to
     * function and enable BAR0 exported by a device. This routine
     * is also used for test and diagnostic purposes only and not
     * designed for normal operation of the driver
     */
    cpdLnx_ENABLE_EP_BAR0_MEM,
    /**
     * This "ioctl" command is used to print out the config
     * space of the device. This routine is also used for test and
     * diagnostic purposes only and not designed for normal
     * operation of the driver
     */
    cpdLnx_PRINT_CONFIG_SPACE,
    cpdLnx_OUTBOUND_CONFIG_READ_TEST_ALL,
    cpdLnx_OUTBOUND_CONFIG_WRITE,
    cpdLnx_INBOUND_DMA_READ,
    cpdLnx_INBOUND_DMA_WRITE,
    cpdLnx_OUTBOUND_DMA_READ,
    cpdLnx_OUTBOUND_DMA_WRITE,
    cpdLnx_POWER_ONOFF,
    cpdLnx_POLLING_LOCAL_ERROR,
    cpdLnx_POLLING_LINKING_STATE,
    cpdLnx_MSG_OUTBOUND_VENDOR_DEFINE,
    cpdLnx_MSG_OUTBOUND_TEST_ALL,
    cpdLnx_TEST_API_EXIT,
    cpdLnx_TEST_ASPM,
    cpdLnx_PCIE_LEGACY_INTERRUPT,
    cpdLnx_OUTBOUND_DIRECT_MEMORY_READ,
    cpdLnx_OUTBOUND_DIRECT_MEMORY_WRITE,

};

/**
 * Globally allocated "ioctl" parameter structure
 */
cpdLnx_IoctlParam cpdLnx_gParam;


/**
 * This routine prints out the "ioctl" structure
 * @param[in]  none
 * @param[out] none
 * @returns    none
 */
void cpdLnx_PrintIoctlParam () {

    pr_info ("\n");
    pr_info ("          vendor_id_____: %x'h\n", (unsigned) cpdLnx_gParam.vendor_id);
    pr_info ("          device_id_____: %x'h\n", (unsigned) cpdLnx_gParam.device_id);
    pr_info ("          irq___________: %d\n"   , cpdLnx_gParam.irq          );
    pr_info ("          num_instances_: %d\n"   , cpdLnx_gParam.num_instances);
    pr_info ("          dev_pointer___: %lx'h\n", cpdLnx_gParam.dev_p        );
    /* FUNCTION 0 / BAR 0 */
    pr_info ("          bar_start__[0]: %lx'h\n", cpdLnx_gParam.bar_start [0]);
    pr_info ("          bar_length_[0]: %lx'h\n", cpdLnx_gParam.bar_length[0]);
    pr_info ("          bar_flags__[0]: %lx'h\n", cpdLnx_gParam.bar_flags [0]);
    /* FUNCTION 0 / BAR 1 */
    pr_info ("          bar_start__[1]: %lx'h\n", cpdLnx_gParam.bar_start [1]);
    pr_info ("          bar_length_[1]: %lx'h\n", cpdLnx_gParam.bar_length[1]);
    pr_info ("          bar_flags__[1]: %lx'h\n", cpdLnx_gParam.bar_flags [1]);
}

static int supervise_localerr_thread(void* arg)
{
    cpdLnx_IoctlParam * ps = & cpdLnx_gParam;
    uint32_t ret = 0 , time_pr_info = 0;

    pr_info ("    Time Slice = %ld(ms) \n", (unsigned long) ps->time_slice);

    if(ps->time_slice == 0)
        ps->time_slice = 100;

    while(ps->b_thread_onoff == true)
    {
#ifdef DEBUG_RUNTIME_LOCAL_ERROR
        ret = _mdrv_pcie_trigger_error_callback(g_rc_data);
        if(ret != 0)
           pr_info ("    _mdrv_pcie_trigger_error_callback() Error !\n");
#endif

        mdelay(ps->time_slice);
        time_pr_info += ps->time_slice;

        if(time_pr_info >= 15000)
        {
            //pr_info ("    Supervise local err thread run successfully Device ID = 0x%x , Vendor ID = 0x%x, \n", ps->device_id, ps->vendor_id);
            pr_info ("    ===== PCIe Local Error Statistics  =====\n");
            pr_info ("    PNP_RX_FIFO_PARITY_ERROR_Count = %d \n", localerror_count.PNP_RX_FIFO_PARITY_ERROR_Count);
            pr_info ("    COMPLETION_RX_FIFO_PARITY_ERROR_Count = %d \n", localerror_count.COMPLETION_RX_FIFO_PARITY_ERROR_Count);
            pr_info ("    REPLAY_RAM_PARITY_ERROR_Count = %d \n", localerror_count.REPLAY_RAM_PARITY_ERROR_Count);
            pr_info ("    PNP_RX_FIFO_OVERFLOW_CONDITION_Count = %d \n", localerror_count.PNP_RX_FIFO_OVERFLOW_CONDITION_Count);
            pr_info ("    COMPLETION_RX_FIFO_OVERFLOW_CONDITION_Count = %d \n", localerror_count.COMPLETION_RX_FIFO_OVERFLOW_CONDITION_Count);
            pr_info ("    REPLAY_TIMEOUT_CONDITION_Count = %d \n", localerror_count.REPLAY_TIMEOUT_CONDITION_Count);
            pr_info ("    REPLAY_TIMEOUT_ROLLOVER_CONDITION_Count = %d \n", localerror_count.REPLAY_TIMEOUT_ROLLOVER_CONDITION_Count);
            pr_info ("    PHY_ERROR_Count = %d \n", localerror_count.PHY_ERROR_Count);
            pr_info ("    MALFORMED_TLP_RECEIVED_Count = %d \n", localerror_count.MALFORMED_TLP_RECEIVED_Count);
            pr_info ("    UNEXPECTED_COMPLETION_RECEIVED_Count = %d \n", localerror_count.UNEXPECTED_COMPLETION_RECEIVED_Count);
            pr_info ("    FLOW_CONTROL_ERROR_Count = %d \n", localerror_count.FLOW_CONTROL_ERROR_Count);
            pr_info ("    COMPLETION_TIMEOUT_CONDITION_Count = %d \n", localerror_count.COMPLETION_TIMEOUT_CONDITION_Count);
            pr_info ("    \n");
            time_pr_info = 0;
        }
    }

    pr_info ("    supervise_localerr_thread() Exit!\n");
    return 0;
}

/**
 * This routine implements the limited-set "ioctl" functionality required
 * for test, debug and diagnostic functionality only. This routine is called
 * by test application code and is not designed or intended for normal
 * operation.
 * @param[in]  filep   Pointer to the device-"file" structure that allows
 *                     access to this driver
 * @param[in] ioctl    This paramter indicates the ioctl command that is
 *                     being involked by the calling application
 * @param[in|out] arg  The ioctl struct that is exchanged between application
 *                     and kernel space
 * @return   -EFAULT   If copying ioctl struct from user space to kernel
 *                     space or vice-versa failed.
 * @return       0     If successful
 */
static long cpdLnx_Ioctl (struct file * filep,
                          unsigned int ioctl,
                          unsigned long arg) {

    cpdLnx_IoctlParam * ps = & cpdLnx_gParam;

    /**
     * copy "ioctl" structure from user space to
     * kernel space
     */
    if (copy_from_user (ps, (const void __user *) arg, sizeof (cpdLnx_IoctlParam))) {
        pr_info ("     cpdLnx_Ioctl(): copy from user failed!\n");
        return -EFAULT;
    }

    switch (ioctl) {

        /**
         * This "ioctl" command is involked by an application to find
         * a device on the PCIe fabric. This command only allows for
         * visibility into what Linux sees. I.e., if the device is
         * part of the Linux PCI device tree, then this command will
         * be successful in finding the device, else it will not.
         */
        case cpdLnx_IOCTL_FIND_DEVICE:
            {
                struct pci_dev * pd = NULL;
                struct pci_dev * tmp = NULL;
                ps->num_instances = 0;

                pr_info ("     Trying to find device with vendor-ID %x'h & device-ID %x'h\n",
                            (unsigned) ps->vendor_id, (unsigned) ps->device_id);
                /**
                 * Call "pci_find_device" to try and
                 * get Linux to find the device
                 */
                while ((pd = pci_get_device (ps->vendor_id, ps->device_id, pd))) {
                    if (! tmp) {
                        tmp = pd;
                    }
                    ps->irq = pd->irq;
                    ps->num_instances ++;

                    /**
                     * Disable the device since we do not
                     * have an attached function for this
                     * device
                     */
                    //pci_disable_device (pd);
                }

                /**
                 * Alert if more than one instance of the same device found
                 */
                if (ps->num_instances > 0)
                    pr_info ("     %d instances of device found\n", ps->num_instances);

                /*-- save the first instance found --*/
                ps->dev_p = (unsigned long) tmp;

                /*-- save sys ptr if we might need it again --*/
                ps->pci_sys_p = (unsigned long) cpdLnx_gPciSysPtr;

                /**
                 * try to enable the device. Not doing this now
                 * since we do not have an attached "function" to
                 * this device.
                 * ret_dev = pci_is_enabled ((struct pci_dev *) tmp);
                 */
                 //ret_dev = pci_enable_device ((struct pci_dev *) tmp);
            }
            break;

        /**
         * This "ioctl" command is used to read BAR registers.
         * This command is used purely for test and diagnostic purposes
         * and performs very specific functionality not to be generally
         * used
         */
        case cpdLnx_IOCTL_READ_BARS:
            {
                /**
                 * reads first two bars
                 */

                /**
                 * physical function 0, bar 0
                 */
                ps->bar_start  [0] = pci_resource_start ((struct pci_dev *) (ps->dev_p), 0);
                ps->bar_length [0] = pci_resource_len   ((struct pci_dev *) (ps->dev_p), 0);
                ps->bar_flags  [0] = pci_resource_flags ((struct pci_dev *) (ps->dev_p), 0);

                /**
                 * physical function 0, bar 1
                 */
                ps->bar_start  [1] = pci_resource_start ((struct pci_dev *) (ps->dev_p), 1);
                ps->bar_length [1] = pci_resource_len   ((struct pci_dev *) (ps->dev_p), 1);
                ps->bar_flags  [1] = pci_resource_flags ((struct pci_dev *) (ps->dev_p), 1);
            }
            break;

        /**
         * This "ioctl" command is used to force a "rescan" of the
         * PCI bus. This routine is also used for test and diagnostic
         * purposes only and not designed for normal operation of
         * the driver
         */
        case cpdLnx_IOCTL_DO_RESCAN:
            {
                /**
                 * force a pci scan by getting Linux
                 * to "rescan" the PCI bus
                 */
                pr_info("     ReScan PCI BUS \n");
                pci_rescan_bus (cpdLnx_gPciBusPtr);
            }
            break;

        /**
         * This "ioctl" command is used to force a "class code" to
         * function exported by a device. This routine is also used for
         * test and diagnostic purposes only and not designed for
         * normal operation of the driver
         */
        case cpdLnx_ENABLE_EP_BAR0_MEM:
            {
                unsigned v, reg, io, memtype, sz, v2, sz2, r, bar = 0, base64 = 3;

                reg = 0x10 + (bar << 2);
                cpdLnx_ReadConf (NULL, 0, reg, 4, & v);

                io      =  v       & 1;
                memtype = (v >> 1) & 3;

                if(io) {
                    pr_info("Error: Unable to handle IO access\n");
                    return -EFAULT;
                } else {
                    switch(memtype) {
                        case 0:
                            // get size
                            cpdLnx_WriteConf (NULL, 0, reg, 4, 0xfffffff0 | v);
                            cpdLnx_ReadConf  (NULL, 0, reg, 4, & r           );

                            if(r & 0xfffffff0)
                                 sz = ~(r & ~0xf) + 1;
                            else sz = 0;
                            // restore BAR
                            cpdLnx_WriteConf (NULL, 0, reg, 4, v);

                            if(sz) {
                                // map it and set BAR
                                unsigned tmp = 0x10 + (bar << 2);
                                cpdLnx_WriteConf (NULL, 0, tmp, 4, 0x400000 | (v & 0xf));
                            }
                            break;
                        case 2:
                            // get size
                            cpdLnx_ReadConf  (NULL, 0, reg+4, 4, & v2          );
                            cpdLnx_WriteConf (NULL, 0, reg  , 4, 0xfffffff0 | v);
                            cpdLnx_WriteConf (NULL, 0, reg+4, 4, 0xffffffff    );
                            cpdLnx_ReadConf  (NULL, 0, reg  , 4, & r           );
                            sz = ~(r & ~0xf) + 1;
                            cpdLnx_ReadConf  (NULL, 0, reg+4, 4, & r           );

                            if(r)
                                 sz2 = ~r;
                            else sz2 =  0;

                            // restore BARs
                            cpdLnx_WriteConf (NULL, 0, reg  , 4, v );
                            cpdLnx_WriteConf (NULL, 0, reg+4, 4, v2);

                            // map it
                            if(!sz2 && (sz < 0x00400000)) {

                                unsigned tmp = 0x10 + (bar << 2);
                                // map it as 32-bit
                                pr_info (" Mapping bar %d as 32 bits\n", bar);


                                cpdLnx_WriteConf (NULL, 0, tmp  , 4, (CPED_PCIE_AXI_PHYS_BASE+CPDI_AXI_REGION_0_SIZE) | (v & 0xf));

                                cpdLnx_WriteConf (NULL, 0, reg+4, 4, 0                    );
                            } else {
                                // full 64-bit
                                pr_info (" Mapping bar %d as 64 bits\n", bar);
                                cpdLnx_WriteConf (NULL, 0, reg+4, 4, base64);
                                cpdLnx_WriteConf (NULL, 0, reg  , 4, v     );
                            }
                            break;
                        default:
                            pr_info("Error: Undefined memory-type from Device\n");
                            return -EFAULT;
                    }
                    // Enable memory access
                    // Enable Bus Mastering (This doesn't seem to work
                    //    if done as a separate write to the command
                    //    register. todo: check why this is)
                    cpdLnx_WriteConf (NULL, 0, 4, 4, 6);
                    pr_info ("     Enabled Memory Access\n");
                }
            }
            break;
        case cpdLnx_PRINT_CONFIG_SPACE:
            {
                // Hardcoded to dev0, func0
                // For testing purposes only (cfg timeout testing)
                unsigned reg = 0, v;
                pr_info("Config Space is______________________:\n");
                for (reg=0;reg<0x40;reg++) {
                    cpdLnx_ReadConf (NULL, 0, reg*4, 4, & v);
                    pr_info(" bus_0/vip_1,dev_0,func_0,offset_%04x: %x\n", reg*4, v);
                }
            }
            break;

        case cpdLnx_OUTBOUND_CONFIG_READ_TEST_ALL:
            {
                uint32_t i , j,  ret_val, Config_size=64;
                uint32_t rdata;
                pr_info("     ===== OUTBOUND_CONFIG_READ_TEST_ALL =====\n");
for(j=0; j<ps->repeat_time; j++)
{
                for(i=0; i <Config_size; i++)
                {
                    ret_val = _mdrv_RC_ob_config_read(g_rc_data, /*REG_TYPE1_CS_BASE +*/ 4*i, &rdata);
                    if (ret_val)
                        pr_info ("     _mdrv_RC_ob_config_read()  Fail at addr = 0x%08x !\n", REG_TYPE1_CS_BASE + 4*i);
                    else
                   {
#if defined (CONFIG_ARM64)
                        pr_info ("     Config Read at addr = 0x%x , Data = 0x%llx \n", REG_TYPE1_CS_BASE + 4*i, (uint64_t) rdata);
#else
                        pr_info ("     Config Read at addr = 0x%08x , Data = 0x%08x \n", /*REG_TYPE1_CS_BASE +*/ 4*i, rdata);
#endif
                   }
                }
}
            }
            break;
        case cpdLnx_OUTBOUND_CONFIG_WRITE:
            {
                uint32_t ret_val;
                pr_info("     ===== OUTBOUND_CONFIG_READ_WRITE =====\n");

                ret_val = _mdrv_RC_ob_config_write(NULL, ps->write_addr, ps->write_data);
                if (ret_val)
                    pr_info ("     _mdrv_RC_ob_config_write()  Fail at addr = 0x%08lx !\n", ps->write_addr);
                else
                    pr_info ("     Config Write at addr = 0x%08lx , Data = 0x%08lx \n", ps->write_addr, ps->write_data);

            }
            break;
        case cpdLnx_INBOUND_DMA_READ:
            {
                int ret = 0;

                if(ret == 0)
                   pr_info ("     cpdLnx_INBOUND_DMA_READ Successfully !\n");
                else
                   pr_info ("     cpdLnx_INBOUND_DMA_READ Fail !\n");
            }
            break;
        case cpdLnx_INBOUND_DMA_WRITE:
            {
                int ret = 0;

                if(ret == 0)
                   pr_info ("     cpdLnx_INBOUND_DMA_WRITE Successfully !\n");
                else
                   pr_info ("     cpdLnx_INBOUND_DMA_WRITE Failed , error code = %d !\n", ret);
            }
            break;
        case cpdLnx_OUTBOUND_DMA_READ:
            {
                uint32_t rdata;
                int count = 1;//64*1024;
                int ret = 0, i = 0;// 0x8900;
                count = ps->repeat_time;

#ifdef K6_IoCtlTest
                int ret = _mdrv_RC_outbound_DMA_read(g_rc_data , CPDREF_RP_SRAM_LOCAL_OB_DESCRIPTOR, CPDREF_RP_SRAM_LOCAL_OB, 16, ps->ob_dma_offset);
#else
                for(i=0; i<count; i++)
                {
                  //writel (0xF + (i*0x10000), 0xFD3A4020);
                  ret = _mdrv_RC_ob_config_read(g_rc_data, (ps->ob_dma_offset + 0x2000000 + (0x100000*(CPDI_RP_OB_REGION_NUMBER-1))), &rdata);
                }
#endif
                pr_info ("     Config Read at addr = 0x%x , Data = 0x%08x \n", (unsigned int) ps->ob_dma_offset, rdata);

                if(ret == 0)
                   pr_info ("     cpdLnx_OUTBOUND_DMA_READ Successfully !\n");
                else
                   pr_info ("     cpdLnx_OUTBOUND_DMA_READ Failed !\n");
            }
            break;
        case cpdLnx_OUTBOUND_DMA_WRITE:
            {
#ifdef K6_IoCtlTest
                int ret = _mdrv_RC_outbound_DMA_write(g_rc_data , CPDREF_RP_SRAM_LOCAL_OB_DESCRIPTOR, CPDREF_RP_SRAM_LOCAL_OB, 16);
#else
                int ret = _mdrv_RC_ob_config_write(NULL, (ps->ob_dma_offset + 0x2000000 + (0x100000*(CPDI_RP_OB_REGION_NUMBER-1))), ps->write_data);
#endif
                if(ret == 0)
                   pr_info ("     cpdLnx_OUTBOUND_DMA_WRITE Successfully !\n");
                else
                   pr_info ("     cpdLnx_OUTBOUND_DMA_WRITE Failed !\n");
            }
            break;
        case cpdLnx_POWER_ONOFF:
            {
                int ret = _mdrv_pcie_power_onoff(ps->b_power_onoff);

                if(ps->b_power_onoff == 0 && ret == 0)
                   pr_info ("     cpdLnx_POWER_ONOFF POWER OFF !\n");
                else
                   pr_info ("     cpdLnx_POWER_ONOFF POWER ON !\n");
            }
            break;
        case cpdLnx_POLLING_LOCAL_ERROR:
            {
                if(polling_localerr_task == NULL)
                {
                    int ret = 0;
                    polling_localerr_task = kthread_create(supervise_localerr_thread, (void *)arg, "Supervise Local Error Task");
                    if (IS_ERR(polling_localerr_task)) {
                      pr_info("     cpdLnx_POLLING_LOCAL_ERROR: create kthread for polling error fail\n");
                      ret = PTR_ERR(polling_localerr_task);
                      polling_localerr_task = NULL;
                    }
                    else
                       wake_up_process(polling_localerr_task);

                   if(ret != 0)
                       pr_info ("     cpdLnx_POLLING_LOCAL_ERROR Create Fail , ErrorCode = %d !\n", ret);
                }
                else
                {
                   pr_info ("     cpdLnx_POLLING_LOCAL_ERROR has already created thread !\n");
                }
            }
            break;

        case cpdLnx_POLLING_LINKING_STATE:
            {

                int ret = _mdrv_check_LinkTrainingComplete();

                if(ret == 0)
                   pr_info ("     cpdLnx_POLLING_LINKING_STATE Successfully !\n");
                else
                   pr_info ("     cpdLnx_POLLING_LINKING_STATE Failed !\n");
            }
            break;

#ifdef CONFIG_MSTAR_PCIE_OB_MSG_ENABLE
        case cpdLnx_MSG_OUTBOUND_VENDOR_DEFINE:
            {
                uint32_t msg_code = 0x7E; //7E: Type 0,  7F:Type 1
                uint32_t ret = MSG_Vendor_Define_Test_All(g_rc_data, msg_code);
                if(ret == 0)
                   pr_info ("     cpdLnx_MSG_OUTBOUND_VENDOR_DEFINE Type 0 Successfully !\n");
                else
                   pr_info ("     cpdLnx_MSG_OUTBOUND_VENDOR_DEFINE Type 0 Failed !\n");

                msg_code = 0x7F;
                ret = MSG_Vendor_Define_Test_All(g_rc_data, msg_code);
                if(ret == 0)
                   pr_info ("     cpdLnx_MSG_OUTBOUND_VENDOR_DEFINE Type 1 Successfully !\n");
                else
                   pr_info ("     cpdLnx_MSG_OUTBOUND_VENDOR_DEFINE Type 1 Failed !\n");
            }
            break;

        case cpdLnx_MSG_OUTBOUND_TEST_ALL:
            {

                int ret = MSG_OutBound_Test_All(g_rc_data);

                if(ret == 0)
                   pr_info ("     cpdLnx_MSG_OUTBOUND_TEST_ALL Successfully !\n");
                else
                   pr_info ("     cpdLnx_MSG_OUTBOUND_TEST_ALL Failed !\n");
            }
            break;
#endif

        case cpdLnx_TEST_API_EXIT:
            {
                if(polling_localerr_task)
                {
                    kthread_stop(polling_localerr_task);
                    polling_localerr_task = NULL;
                    pr_info ("     polling_localerr_task free Successfully !\n");
                }
                else
                    pr_info ("     cpdLnx_TEST_API_EXIT Successfully !\n");
            }
            break;
        case cpdLnx_TEST_ASPM:
            {
                int ret = _mdrv_pcie_ASPM_onoff(ps->b_ASPM_onoff);

                if(ps->b_ASPM_onoff == 1 && ret == 0)
                   pr_info ("     cpdLnx ASPM TEST Successfully !\n");
                else
                   pr_info ("     cpdLnx ASPM TEST Failed !\n");
            }
            break;
        case cpdLnx_PCIE_LEGACY_INTERRUPT:
            {
                int ret = _mdrv_pcie_legacy_onoff(ps->b_legacy_onoff);

                if(ps->b_legacy_onoff == 1 && ret ==0)
                   pr_info ("     cpdLnx LEGACY INTERRUPT Enable!\n");
                else
                   pr_info ("     cpdLnx LEGACY INTERRUPT Disable!\n");
            }
            break;
        case cpdLnx_OUTBOUND_DIRECT_MEMORY_READ:
            {
                uint32_t rdata = 0;
#ifdef CONFIG_MSTAR_K6
                rdata = _mdrv_RC_outbound_DM_read(g_rc_data, ps->ob_dma_offset);
#endif
                pr_info ("     Outbound  Read at addr = 0x%x , Data = 0x%08x \n", (unsigned int) ps->ob_dma_offset, rdata);
            }
            break;
        case cpdLnx_OUTBOUND_DIRECT_MEMORY_WRITE:
            {
#ifdef CONFIG_MSTAR_K6
                _mdrv_RC_outbound_DM_write(g_rc_data, ps->ob_dma_offset, ps->write_data);
#endif
                pr_info ("     Outbound Write at addr = 0x%x , Data = 0x%08x \n", (unsigned int) ps->ob_dma_offset, (unsigned int) ps->write_data);
            }
            break;
        default: break;
    }

    /**
     * copy "ioctl" structure from kernel space to
     * user space
     */
    if (copy_to_user ((void __user *) arg, (const void *) ps, sizeof (cpdLnx_IoctlParam))) {
        pr_info ("copy from user failed!\n");
        return -EFAULT;
    }

    return 0;
}

/**
 * "Open" routine for the "file_operations" structure associated
 * with this driver. This routine just increments the reference count
 * to the module by calling a "try_module_get"
 * @param[in]   inode   pointer to the inode structure
 * @param[in]   filep   pointer to the file structure
 * @return        0     always returns success
 */
static int cpdLnx_Open (struct inode * inode, struct file  * filep) {

    try_module_get (THIS_MODULE);
    pr_info ("     cpdLnx_Open()\n");
    return 0;
}

/**
 * "Release" routine for the "file_operations" structure associated
 * with this driver. This routine just decrements the reference count
 * to the module by calling a "module_put"
 * @param[in]   inode   pointer to the inode structure
 * @param[in]   filep   pointer to the file structure
 * @return        0     always returns success
 */
static int cpdLnx_Release (struct inode * inode, struct file * filep) {

    module_put (THIS_MODULE);

    pr_info ("     cpdLnx_Release()\n");
    return 0;
}

/**
 * "Read" routine for the "file_operations" structure associated
 * with this driver. This routine performs a "custom-read" function
 * wherein it checks for the presence of an EP. This routine is
 * used only by test, debug and diagnostic code and must never be
 * invoked during normal operation.
 * @param[in]   filep   pointer to the file structure
 * @param[in]   buffer  pointer to buffer that hold data read
 * @param[in]   length  length of data to be read
 * @param[in]   offset  offset from which to read the data
 * @return        0     always returns success
 */
static ssize_t cpdLnx_Read (struct file * filep,
                            char * buffer,
                            size_t length,
                            loff_t * offset) {

    struct pci_dev * dev = NULL;
    int dev_cnt = 0;

    /**
     * check to see how many EPs with our vendor & device ids
     * we can find (This is a debug function with hardcoded values
     * for testing purposes ONLY). This function will work only
     * in default test cases and is not to be included for
     * normal operation.
     */
    while ((dev = (pci_get_device (0xDEDB, 0x100, dev)))) {
        dev_cnt ++;
    }
    pr_info ("     Note! found %d instances of the same device.\n", dev_cnt);

    return 0; /* should return number of bytes read */
}

/**
 * "Write" routine for the "file_operations" structure associated
 * with this driver. This routine currently does nothing and is
 * used only by test, debug and diagnostic code and must never be
 * invoked during normal operation.
 * @param[in]   filep   pointer to the file structure
 * @param[in]   buffer  pointer to buffer that hold data read
 * @param[in]   length  length of data to be read
 * @param[in]   offset  offset from which to read the data
 * @return    -EINVAL   always fails
 */
static ssize_t cpdLnx_Write (struct file * filep,
                             const char * buffer,
                             size_t length,
                             loff_t * offset) {

    return -EINVAL;
}

/**
 * "file_operations" structure associated with
 * this driver
 */
static const struct file_operations cpdLnx_fops = {

    .owner           = THIS_MODULE,
    .llseek          = noop_llseek,
    .unlocked_ioctl  = cpdLnx_Ioctl,
    .read            = cpdLnx_Read,
    .write           = cpdLnx_Write,
    .open            = cpdLnx_Open,
    .release         = cpdLnx_Release,
};


/**
 * Global pointer to the "platform_device' structure
 */
static struct platform_device * pdev;

/**
 * Personal data for the platform device. Not really being
 * used for any practical purpose right now. This structure
 * is reserved for future use.
 */
typedef struct pcDevData {
    u32   field_1;
    u32   field_2;
    u32   field_3;
    u32   field_4;
} cpdLnx_pcDevData;

static cpdLnx_pcDevData cpdLnx_PlatformDevData;


void Config_Read_Test_All(void *pD)
{
    uint32_t i , ret_val;
    uint32_t rdata;

    for(i=0; i<40; i++)
    {
        ret_val = _mdrv_RC_ob_config_read(pD, REG_TYPE1_CS_BASE + 4*i, &rdata);
        if (ret_val)
#ifdef ZEBU_ESL
        {uart_putc('Z'); uart_putc('\n');}
#else
        {
            pr_info ("_mdrv_RC_ob_config_write()  Fail !\n");
            break;
        }

        pr_info("Config_Read_Test_All() Addr=0x%x , rData=0x%x\n", REG_TYPE1_CS_BASE + 4*i, rdata);

#endif
    }
}

void Config_ReadWrite_BAR0(void *pD)
{
    uint32_t ret_val, wdata = 0xFFFFFFF0;
    uint32_t rdata;

    ret_val = _mdrv_RC_ob_config_write(pD, REG_TYPE1_CS_BASE + 0x10, wdata);
    ret_val = _mdrv_RC_ob_config_read(pD, REG_TYPE1_CS_BASE + 0x10, &rdata);

    pr_info("Config_ReadWrite_BAR0() Addr=0x%x , rData=0x%x\n", REG_TYPE1_CS_BASE + 0x10, rdata);
}

/**
 * PCIe Subsystem Init. This routine is used to initialize the pcie
 * subsystem which is part of the system platform.
 * @param[in]  none
 * @param[out] none
 * @return     0     if success
 * @return    int    Linux error code if error
 */
static int cpdLnx_subsystem_init (void) {

    int ret;
    struct resource res;

    //void __iomem *mstar_riu_base;
    //phys_addr_t address;

    CPDI_OBJ * drvptr = CPDI_GetInstance();

    //CPDI_InitParam rc_init_params;

    //memset (& rc_init_params, 0, sizeof (CPDI_InitParam));

    /**
     * Call core driver API - probe for RP
     */

    //mstar_riu_base = ioremap(IO_PHYS, IO_SIZE);
    //address = mstar_pm_base + CPDREF_RP_BASE;

    pr_info ("===============  PCIE Log   =======%s==== \n", PCIE_MSTAR_VERSION);

    ret = _mdrv_system_init();

    if(ret == 0)
       pr_info ("     PCIE HW Init Success !\n");
    else
      {
			pr_info ("     PCIE HW Init Fail !\n");
#if 1
			return -ECANCELED;
#else
			do
			{
				mdelay(100);
			}while(1);
#endif
      }

#if defined (CONFIG_ARM64)
    pr_info ("     Calling probe of core driver , RP base addr = 0x%llx'h \n", (uint64_t) CPDREF_RP_BASE);
#else
    pr_info ("     Calling probe of core driver , RP base addr = 0x%x'h \n", CPDREF_RP_BASE);
#endif

    ret = drvptr->probe (CPDREF_RP_BASE, & sysReq);

    if (ret) {
        pr_info ("    >> cpdLnx/Error! Unable to probe for core-driver\n");
        return -EINVAL;
    }

    /**
     * Initialize the callbacks for errors
     */
    //rc_init_params.initErrorCallback = 0;


    // We just use the default offset value
    // rc_init_params.initBaseAddresses.initparamLmBase.changeDefault = 1;
    // rc_init_params.initBaseAddresses.initparamLmBase.newValue = (CPDREF_RP_BASE + CPDI_REGS_OFFSET_LOCAL_MANAGEMENT);



    // We just use the default offset value
    // rc_init_params.initBaseAddresses.initparamAwBase.changeDefault = 1;
    // rc_init_params.initBaseAddresses.initparamAwBase.newValue = (CPDREF_RP_BASE + CPDI_REG_OFFSET_AXI_WRAPPER);

    //rc_init_params.initBaseAddresses.initparamAxBase.changeDefault = 1;     //Only 28~31bit can use
    //rc_init_params.initBaseAddresses.initparamAxBase.newValue = 0xF0000000; //CPDI_DEFAULT_ECAM_BASE_BUS_ADDRESS, Original is CPDI_RP_AX_BASE;



    /**
     * Call core driver API - init for RP
     */
    pr_info ("     Initializing core driver\n");

    //ret = drvptr->init (g_rc_data, CPDREF_RP_BASE, & rc_init_params);
    ret = CPDREF_ProbeAndInitRPDriver(g_rc_data, sizeof(g_rc_data));

    if (ret) {
        pr_info ("    >> cpdLnx/Error! Unable to initialize the core-driver\n");
        return -EINVAL;
    }


    /**
     * Announce the subsystem
     */
    //pr_info("     Initializing pcie subsystem\n");
    //pr_info("     Registering platform driver '%s'\n", cpdLnx_PCIePlatformDriver.driver.name);

    /**
     * register platform driver
     * for PCIe
     */
    ret = platform_driver_register (& cpdLnx_PCIePlatformDriver);

    if (ret) {
        pr_info("  >> cpdLnx/Error! Unable to register platform driver '%s': %d\n", cpdLnx_PCIePlatformDriver.driver.name, ret);
        return ret;
    }

    pci_set_platform_pm(&mstar_pci_platform_pm);
    /**
     * allocate platform device which is
     * the PCIe Root Port
     */
    pdev = platform_device_alloc (DRVNAME, 0);

    if (! pdev)
        pr_info("  >> cpdLnx/Error! Unable to allocate device '%s'\n", DRVNAME);


    /**
     * add data for the platform device. This data being added
     * is not really being used as of now but is reserved for
     * future use.
     */
    ret = platform_device_add_data (pdev, & cpdLnx_PlatformDevData, sizeof (cpdLnx_pcDevData));

    if (ret) {
       pr_info("  >> cpdLnx/Error! Platform data allocation failed\n");
       return ret;
    }

    /**
     * initialize resources
     */
    memset (& res, 0, sizeof(res));
    res.name  = "PCIE RP Base Bank";
    res.start = CPDREF_RP_BASE;
    res.end   = CPDREF_RP_BASE + 0xFFF;
    res.flags = IORESOURCE_MEM;

    /**
     * add resources for platform device
     */
    ret = platform_device_add_resources (pdev, &res, 1);
    if (ret) {
        pr_info("  >> cpdLnx/Error! Device resource addition failed (%d)\n", ret);
        return ret;
    }


    /**
     * add platform device to indirectly call probe
     */
    //pr_info("     PCIe platform_device_add()\n");
    ret = platform_device_add (pdev);

    if (ret) {
        pr_info("  >> cpdLnx/Error! Device addition failed (%d)\n", ret);
        return ret;
    }

    /**
     * register driver with a character device driver interface
     * for file operations. This provides an interface for access
     * to the driver.
     */
    //pr_info("     PCIe register_chrdev()\n");
    ret = register_chrdev (DATABAHN_PCIE_MAJOR_NUMBER, DATABAHN_PCIE_CHRDEV_NAME, & cpdLnx_fops);

    if (ret) {
        pr_info("  >> cpdLnx/Error! Unable to register Interface Driver\n");
        return ret;
    }

    /**
     * We finally got here!
     * We are all done with intialization
     */
    //pr_info("     PCIe subsystem initialized\n");

#ifdef PCIE_3LAYER_INTERRUPT
    ret = _mdrv_pcie_irq_init();
    if(ret == 0)
      pr_info ("     PCIE IRQ Init Success !\n");
    else
      pr_info ("     PCIE IRQ Init Success Fail !\n");
#endif


    return 0;
}


/**
 * subsystem exit
 * @param[in]    none
 * @param[out]   none
 * @return       none
 */
static void cpdLnx_subsystem_exit (void) {

    pr_info("cadence pcie subsystem exiting\n");

    /**
     *  unregister the device
     */
    platform_device_unregister (pdev);

    /**
     * unregister the driver
     */
    platform_driver_unregister (& cpdLnx_PCIePlatformDriver);

    /**
     * Unregister the interface character driver
     */
    unregister_chrdev (DATABAHN_PCIE_MAJOR_NUMBER, DATABAHN_PCIE_CHRDEV_NAME);
}


/**
 * Do PCIe config cycle - for read.
 * @param[in]   bus    pointer to "pci_bus" to run config cycle on
 * @param[in]   devfn  device and function
 * @param[in]   offset offset into the config space
 * @param[in]   size   size to read
 * @param[out]  val    value read back
 * @return    PCIBIOS_SUCCESSFUL  always returns success. it is left
 *                             to the calling function to device if
 *                             what was read makes sense.
 * @return    PCIBIOS_SET_FAILED  failure if the size to be written
 *                           didn't get passed down correctly
 */
int cpdLnx_ReadConf (struct pci_bus * bus,
                     u32 devfn,
                     int offset,
                     int size,
                     u32 * val) {

    int ret;
    u32 valRead;

    CPDI_OBJ * drvptr = CPDI_GetInstance();

    *val = 0;

    ret = drvptr->doConfigRead (g_rc_data,
#ifdef USE_VIP_EP
                                bus ? bus->number+1 : 1,
#else
                                bus ? bus->number : 0,
#endif
                                PCI_SLOT(devfn),
                                PCI_FUNC (devfn),
                                offset,
                                size,
                                & valRead);
    if (ret) {
        pr_info ("    >> cpdLnx/Error! Unable to perform config read cycle\n");
        return  PCIBIOS_SET_FAILED;
    }

    *val = valRead;

    /* -- debug message
    if (! devfn)
        pr_info("-- r %x @ %04x:%04x:%04x:%04x () %08lx ",
            size, bus ? bus->number : 0, PCI_SLOT(devfn), PCI_FUNC(devfn), offset, *val);
    */

    return PCIBIOS_SUCCESSFUL;
}

/**
 * Do PCIe config cycle - for write.
 * @param[in]   bus    pointer to "pci_bus" to run config cycle on
 * @param[in]   devfn  device and function
 * @param[in]   offset offset into the config space
 * @param[in]   size   size to read
 * @param[in]   data   data to be written through the config cycle
 * @return    PCIBIOS_SUCCESSFUL  returns success if the write
 *                           was successful. accuracy of what was actually
 *                           written is upto the calling function.
 * @return    PCIBIOS_SET_FAILED  failure if the size to be written
 *                           didn't get passed down correctly
 */
int cpdLnx_WriteConf (struct pci_bus * bus,
                      u32 devfn,
                      int offset,
                      int size,
                      u32 data) {
    int ret;
    CPDI_OBJ * drvptr = CPDI_GetInstance();

    /* --debug message
    if (! devfn)
        pr_info("++ w %x @ %04x:%04x:%04x:%04x -- %08lx\n",
            size, bus ? bus->number : 0, PCI_SLOT(devfn), PCI_FUNC(devfn), offset, data);
    */

    ret = drvptr->doConfigWrite (g_rc_data,
#ifdef USE_VIP_EP
                                 bus ? bus->number+1 : 1,
#else
                                 bus ? bus->number : 0,
#endif
                                 PCI_SLOT(devfn),
                                 PCI_FUNC (devfn),
                                 offset,
                                 size,
                                 data);

    if (ret) {
        pr_info ("    >> cpdLnx/Error! Unable to perform config write cycle\n");
        return  PCIBIOS_SET_FAILED;
    }

    return PCIBIOS_SUCCESSFUL;
}


/**
 * Module information
 */
MODULE_AUTHOR("Mstar PCIe Host FW");
MODULE_DESCRIPTION("Mstar PCIe Host PCIe 1s.0 Core");
MODULE_LICENSE("GPL");


/**
 * Module init and exit
 */
module_init(cpdLnx_subsystem_init);
module_exit(cpdLnx_subsystem_exit);
