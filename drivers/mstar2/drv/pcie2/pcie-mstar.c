/*

 * Copyright (c) 2010 - 2014 MStar Semiconductor, Inc.
 This program is free software.
 You can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation;
 either version 2 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program;
 if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

*/



#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/msi.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/of_pci.h>
#include <linux/of_platform.h>
#include <linux/pci.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/resource.h>
#include <linux/signal.h>
#include <linux/types.h>
#include <asm/mach/pci.h>
/* #include <linux/phy/phy.h> */
#include "pcie-mstar.h"
#include "mstar/mstar_chip.h"

#ifdef CONFIG_MSTAR_ARM_BD_FPGA
#include "pcie-iic.h"
#endif


#define pcie_dbg(dev, fmt, args...) \
	do { if (PCIE_DEBUG) dev_err(dev , fmt , ## args); } while (0)
#define pcie_info(dev, fmt, args...) \
	dev_err(dev , fmt , ## args)
#define pcie_err(dev, fmt, args...) \
	dev_err(dev , fmt , ## args)


#if defined(MSTAR_MIU1_BUS_BASE)
/* #define MIU1_BUS_BASE_ADDR	MSTAR_MIU1_BUS_BASE */
#endif
#if defined(MSTAR_MIU2_BUS_BASE)
/* #define MIU2_BUS_BASE_ADDR	MSTAR_MIU2_BUS_BASE */
#endif


/* PCI CfgWr/CfgRd registers */
#define CFG_HEADER_0 					0x460
#define CFG_HEADER_1					0x464
#define CFG_HEADER_2					0x468
#define CFG_HEADER_3 					0x46c
#define CFG_WDATA						0x470
#define APP_TLP_REQ						0x488
#define APP_CFG_REQ						BIT(0)
#define APP_MSG_REQ						BIT(1)
#define APP_CPL_STATUS					GENMASK(7, 5)
#define CFG_RDATA						0x48c

/* PCI Interrupt registers */
#define INT_MASK 						0x420
#define INTA_MASK						BIT(16)
#define INTB_MASK						BIT(17)
#define INTC_MASK						BIT(18)
#define	INTD_MASK						BIT(19)
#define INTX_MASK						(INTA_MASK | \
						INTB_MASK | INTC_MASK | INTD_MASK)
#define MSTAR_PCIE_INTX_SHIFT			16
#define	MSI_MASK						BIT(23)
#define PCIE_L2_ENTRY_WAKE_MASK			BIT(30)
#define INT_STATUS						0x424
#define INTA_STATUS						BIT(16)
#define INTB_STATUS						BIT(17)
#define INTC_STATUS						BIT(18)
#define INTD_STATUS						BIT(19)
#define PM_HP_EVENT_STATUS				BIT(21)
#define AER_EVENT_STATUS				BIT(22)
#define MSI_STATUS						BIT(23)
#define PCIE_L2_ENTRY_WAKE_STATUS		BIT(30)
#define IMSI_STATUS						0x42c

/* IP Configuration registers */
#define K_GBL_1 						0x000
#define PCIE_IS_RC_ROOT_PORT			(0x04UL<<12)
#define K_CONF_FUNC0_0					0x100
#define K_CONF_FUNC0_1					0x104
#define K_CONF_FUNC0_2					0x108
#define K_CONF_FUNC0_3					0x10C
#define K_CONF_FUNC0_3_MASK				0x0001FE00UL
#define _K_CONF_D1_SUP					BIT(9)
#define _K_CONF_D2_SUP					BIT(10)
#define _K_CONF_PME_SUP					(0x1FUL<<11)
#define _K_CONF_AER_EN					BIT(16)
#define SET_K_CONF_FUNC0_3				(_K_CONF_D1_SUP | \
					_K_CONF_D2_SUP | _K_CONF_PME_SUP | _K_CONF_AER_EN)

#define K_CONF_FUNC0_4					0x110
#define K_CONF_FUNC0_4_MASK				0xFE000000UL
#define _K_CONF_L1S_SUP					BIT(25)
#define _K_CONF_LS_EXIT_LAT_COM			(0x01UL<<26)
#define _K_CONF_LS_EXIT_LAT_SEP			(0x01UL<<29)
#define SET_K_CONF_FUNC0_4 				(_K_CONF_L1S_SUP | \
					_K_CONF_LS_EXIT_LAT_COM | _K_CONF_LS_EXIT_LAT_SEP)

#define K_CONF_FUNC0_12					0x130
#define K_RXL0S_EXIT_TIME_MASK			(0x3FUL << 8)
#define K_RXL0S_EXIT_TIME				(0x08UL << 8)

#define K_FC_VC0_0						0x030
#define SET_K_FC_VC0_0					0x02008020UL

#define K_FC_VC0_1						0x034
#define SET_K_FC_VC0_1					0x00000002UL

#define K_CNT_2							0x014
#define K_CNT_2_MASK					0x00FFF800UL
#define K_CNT_L0S_ENTRY_LAT				0x00
#define K_CNT_L1_ENTRY_LAT				0x00
#define SET_K_CNT_2						(K_CNT_L0S_ENTRY_LAT | K_CNT_L1_ENTRY_LAT)

#define MSTAR_ROOT_PORT					((4 << 12) & 0xf000)
#define MSTAR_VEND_ID					(0x14c3)
#define MSTAR_DEV_ID					(0x5396)
#define MSTAR_SS_VEND_ID				(0x14c3)
#define MSTAR_SS_DEV_ID					(0x5396)
#define MSTAR_RC_CLASS					(0x0604)
#define MSTAR_RC_REVISION				(0x0000)

#define K_CONF_IDS(vend, dev) \
	((vend & 0xffff) | ((dev << 16) \
	& 0xffff0000))

/* PCI MAC registers */
#define PCI_RSTCR 						0x510
#define PCI_PHY_RSTB					BIT(0)
#define PCI_PIPE_SRSTB					BIT(1)
#define PCI_MAC_SRSTB					BIT(2)
#define PCI_CRSTB						BIT(3)
#define PCI_PERSTB						BIT(8)
#define PCI_PIPE_RST_EN					BIT(13)
#define PCI_MAC_RST_EN					BIT(14)
#define PCI_CONF_RST_EN					BIT(15)
#define PCI_RST_DEASSERTED			    (PCI_PHY_RSTB | \
						PCI_PIPE_SRSTB | PCI_MAC_SRSTB | PCI_CRSTB)
#define PCI_LINKDOWN_RST_EN				(PCI_PIPE_RST_EN | \
								PCI_MAC_RST_EN | PCI_CONF_RST_EN)
#define PCI_WAKE_CONTROL				0x52c
#define CLKREQ_N_ENABLE					BIT(0)
#define PCI_LINK_STATUS					0x804
#define PCI_LINKUP						BIT(10)
#define IMSI_ADDR						0x430
#define MSI_VECTOR						0x0c0
#define MSI_IRQS						32
#define PCIE_PORT_MSI_BIT				32
#define MAX_MSI_IRQS					(MSI_IRQS + 1) /* 32 msi irqs for endpoint(b[31:0]) & 1 msi irq for pcie port(b32) */
#define INTX_IRQ_NUM					5
#define AHB2PCIE_BASE0_L				0x438
#define AHB2PCIE_BASE0_H				0x43c
#define AHB2PCIE_BASE1_L				0x440
#define AHB2PCIE_BASE1_H				0x444
#define PCIE2AXI_WIN					0x448
#define WIN_NOPREFETCH					BIT(6)
#define WIN_ENABLE						BIT(7)

#define CFG_BAR0						0x490
#define CFG_BAR1						0x494
#define CFG_BAR2						0x498
#define CFG_BAR3						0x49C
#define CFG_BAR4						0x4A0
#define CFG_BAR5						0x4A4


#define DBG_MODE_SEL					0x518

/* REG_PHYMAC_CONF */
#define REG_PHYMAC_CONF					0x528
#define REC_RXCFG_TXTS2_NUM_MASK		(0x7FUL << 16)
#define REC_RXCFG_TXTS2_NUM				(0x40UL << 16)


#define AHB2PCIE_BASEL(base)		    (base & 0xffffffff)
#define AHB2PCIE_BASEH(base)		    (base >> 32)
#define NOPREFETCH(flag)				((flag & IORESOURCE_PREFETCH) \
											? 0 : 1 << 6)
#define BASE_SIZE(sz)					(sz & 0x1f)

#define PCIE2AXI_SIZE					0x000FFFFFFFFF /* Translated AXI base address (36 bits) */

/* PCI Configuration Transaction Header */
#define CFG_DW0_LENGTH(length)		(length & 0x3ff)
#define CFG_DW0_ATTR(attr)			((attr << 12) & 0x3000)
#define CFG_DW0_EP(ep)		        ((ep << 14) & 0x4000)
#define CFG_DW0_TD(td)				((td << 15) & 0x8000)
#define CFG_DW0_TC(tc)				((tc << 20) & 0x700000)
#define CFG_DW0_TYPE(type)			((type << 24) & 0x1f000000)
#define CFG_DW0_FMT(fmt)			((fmt << 29) & 0xe0000000)
#define CFG_DW1_FBE(fbe)			(fbe & 0x0f)
#define CFG_DW1_LBE(lbe)			((lbe << 4) & 0xf0)
#define CFG_DW1_TAG(tag)		    ((tag << 8) & 0xff00)
#define CFG_DW1_RID(rid)			((rid << 16) & 0xffff0000)
#define CFG_DW2_REGN(regn)			((((regn >> 2) & 0x3f) << 2) | \
								(((regn >> 8) & 0x0f) << 8))
#define CFG_DW2_FUN(fun)		    ((fun << 16) & 0x070000)
#define CFG_DW2_DEV(dev)		    ((dev << 19) & 0xf80000)
#define CFG_DW2_BUS(bus)		    ((bus << 24) & 0xff000000)

#define  CFG_HEADER_DW0(ep, td, type, fmt) \
	(CFG_DW0_LENGTH(1) | CFG_DW0_ATTR(0) | \
	 CFG_DW0_EP(ep) | CFG_DW0_TD(td) | CFG_DW0_TC(0) | \
	  CFG_DW0_TYPE(type) | CFG_DW0_FMT(fmt))

#define  CFG_HEADER_DW1(fbe, tag, rid) \
	(CFG_DW1_FBE(fbe) | CFG_DW1_LBE(0) | \
	 CFG_DW1_TAG(tag) | CFG_DW1_RID(rid))

#define  CFG_HEADER_DW2(regn, fun, dev, bus) \
	(CFG_DW2_REGN(regn) | CFG_DW2_FUN(fun) | \
	 CFG_DW2_DEV(dev) | CFG_DW2_BUS(bus))

/**
 * struct mstar_pcie_port - PCIe port information
 * @name: PCIe port name
 * @base: IO Mapped Register Base
 * @irq: Interrupt number
 * @port: Port number
 * @lane: Lane count
 * @devfn: Device/Function number
 * @pcie: PCIe host info pointer
 * @irq_domain:  IRQ domain pointer
 * @msi_irq_domain:  MSI IRQ domain pointer
 */
struct mstar_pcie_port {
	char *name;
	int 	portnum;
	void __iomem *base;
	void __iomem *riu_base;
	unsigned int irq;
	unsigned int pin_irqs[4];
	u32 busnum;
	u32 lane;
	int devfn;
	int Cfg1_18h;

	uintptr_t	iomem_base;
	uintptr_t	iomem_len;

	struct device *dev;
	struct resource *bus_range;
	struct resource *io_res;
	struct resource pref_mem;
	struct resource npref_mem;
	struct list_head resources;

#if 0 /* MSTAR_PORTING */
	struct irq_domain *irq_domain;
	struct irq_domain *msi_irq_domain;
	DECLARE_BITMAP(msi_irq_in_use, MSI_IRQS);
#endif
#ifdef PATCH_WED_XIU_BUS_COLLISION
	int	no_xiu_access;
#endif
};

struct mstar_pcie_port g_pcie_ports[] = {
	[0] = {
		.name = MSTAR_PCIE_PORT0_NAME,
		.portnum = 0,
		.irq = MSTAR_PCIE_PORT0_IRQ,
		.pin_irqs = {
			MSTAR_PCIE_PORT0_INTA,
			MSTAR_PCIE_PORT0_INTB,
			MSTAR_PCIE_PORT0_INTC,
			MSTAR_PCIE_PORT0_INTD},
		.lane = MSTAR_PCIE_PORT0_LANE,
		.iomem_base = MSTAR_PCIE_PORT0_IOMEM_BASE,
		.iomem_len = MSTAR_PCIE_PORT0_IOMEM_LEN,
#ifdef PATCH_SINGLE_DEVFN
		.devfn = PCI_DEVFN(0, 0),				/* port 0, dev=0, fn=0 */
#endif
		.Cfg1_18h = 0x00000000,				/* default bus address 0 */
	},
#ifdef ENABLE_PCIE_PORT1
	[1] = {
		.name = MSTAR_PCIE_PORT1_NAME,
		.portnum = 1,
		.irq = MSTAR_PCIE_PORT1_IRQ,
		.pin_irqs = {
			MSTAR_PCIE_PORT1_INTA,
			MSTAR_PCIE_PORT1_INTB,
			MSTAR_PCIE_PORT1_INTC,
			MSTAR_PCIE_PORT1_INTD},
		.lane = MSTAR_PCIE_PORT1_LANE,
		.iomem_base = MSTAR_PCIE_PORT1_IOMEM_BASE,
		.iomem_len = MSTAR_PCIE_PORT1_IOMEM_LEN,
#ifdef PATCH_SINGLE_DEVFN
		.devfn = PCI_DEVFN(1, 0),				/* port 1, dev=1, fn=0 */
#endif
		.Cfg1_18h = 0x00000000,				/* default bus address 0 */
	},
#endif
};
#define PCIE_PORT_COUNT	ARRAY_SIZE(g_pcie_ports)

static struct platform_device Mstar_pcie_0_device = {
	.name = MSTAR_PCIE_PORT0_NAME,
	.id = 0,
};

#ifdef ENABLE_PCIE_PORT1
static struct platform_device Mstar_pcie_1_device = {
	.name = MSTAR_PCIE_PORT1_NAME,
	.id = 1,
};
#endif

static struct platform_device *Mstar_pcie_platform_devices[] = {
	&Mstar_pcie_0_device,
#ifdef ENABLE_PCIE_PORT1
	&Mstar_pcie_1_device,
#endif
};



/* MStar PCIe RC TOP registers */
#define REG_SRST_TO_MAC_TOP				0x00
#define REG_DESIGN_OPTION				0x01
#define REG_BIT_LTSSM_EN				BIT(2)
#define REG_BIT_L3_FLUSH_ENABLE			BIT(7)
#define REG_BIT_MASK_INBWRT_AXIBVLD_EN  BIT(8)
#define REG_BIT_MI_DYNAMIC_REQUEST		BIT(9)

#define REG_05							0x05
#define REG_TEST_BUS_1B					0x1B
#define REG_BIT_DEBUG_MASK_DISABLE		BIT(5)

#define REG_36							0x36
#define REG_BIT_DELAY_FLUSH_REQ			BIT(13)
#define REG_BIT_MAC_SRAM_SD_EN			BIT(14)
#define REG_BIT_BRG_SRAM_SD_EN			BIT(15)

#define REG_64BIT_WFUL_ON				0x39
#define REG_BIT_64BIT_WFUL_ON			BIT(1)

#define REG_MIU0_START_ADDR0			0x47
#define REG_MIU0_START_ADDR1			0x48
#define REG_MIU0_START_ADDR2			0x49
#define REG_MIU0_END_ADDR0				0x4A
#define REG_MIU0_END_ADDR1				0x4B
#define REG_MIU0_END_ADDR2				0x4C

#define REG_MIU1_START_ADDR0			0x4D
#define REG_MIU1_START_ADDR1			0x4E
#define REG_MIU1_START_ADDR2			0x4F
#define REG_MIU1_END_ADDR0				0x50
#define REG_MIU1_END_ADDR1				0x51
#define REG_MIU1_END_ADDR2				0x52

#define REG_MIU2_START_ADDR0			0x53
#define REG_MIU2_START_ADDR1			0x54
#define REG_MIU2_START_ADDR2			0x55
#define REG_MIU2_END_ADDR0				0x56
#define REG_MIU2_END_ADDR1				0x57
#define REG_MIU2_END_ADDR2				0x58

#define REG_MIU0_TLB_START_ADDR0		0x59
#define REG_MIU0_TLB_START_ADDR1		0x5A
#define REG_MIU0_TLB_START_ADDR2		0x5B
#define REG_MIU0_TLB_END_ADDR0			0x5C
#define REG_MIU0_TLB_END_ADDR1			0x5D
#define REG_MIU0_TLB_END_ADDR2			0x5E

#define REG_MIU1_TLB_START_ADDR0		0x5F
#define REG_MIU1_TLB_START_ADDR1		0x60
#define REG_MIU1_TLB_START_ADDR2		0x61
#define REG_MIU1_TLB_END_ADDR0			0x62
#define REG_MIU1_TLB_END_ADDR1			0x63
#define REG_MIU1_TLB_END_ADDR2			0x64

#define REG_MIU2_TLB_START_ADDR0		0x65
#define REG_MIU2_TLB_START_ADDR1		0x66
#define REG_MIU2_TLB_START_ADDR2		0x67
#define REG_MIU2_TLB_END_ADDR0			0x68
#define REG_MIU2_TLB_END_ADDR1			0x69
#define REG_MIU2_TLB_END_ADDR2			0x6A


#define REG_AXI2MIU_SETTING				0x74
#define REG_BVALID_LDZ_OPT_MASK			0x03
#define REG_BVALID_DEPENDS_ON_LDZ		0x01
#define REG_MIU_ENABLE_MASK				0x1C
#define REG_BIT_MIU0_ENABLE				BIT(2)
#define REG_BIT_MIU1_ENABLE				BIT(3)
#define REG_BIT_MIU2_ENABLE				BIT(4)
#define REG_BIT_AXI2MI_SRST				BIT(7)

#define REG_CPU2PCIE_ADDR_SEG             0x76
#define REG_ACPU_TO_PCIE_ADR_31_28_MASK   0x0000000F
#define REG_SCPU_TO_PCIE_ADR_31_28_MASK   0x000000F0
#define REG_NOE0_TO_PCIE_ADR_31_28_MASK   0x00000F00
#define REG_NOE1_TO_PCIE_ADR_31_28_MASK   0x0000F000




#ifdef CONFIG_PCI_MSI
struct msi_map_entry {
	bool used;
	u8 index;
	unsigned int irq;
};

#define MSI_MAP_SIZE  (MAX_MSI_IRQS)
static struct msi_map_entry msi_map[MSI_MAP_SIZE];

static void msi_map_init(void)
{
	int i;

	for (i = 0; i < MSI_MAP_SIZE; i++) {
		msi_map[i].used = false;
		msi_map[i].index = i;
		msi_map[i].irq = 0;
	}
}

static struct msi_map_entry *msi_map_get(void)
{
	struct msi_map_entry *retval = NULL;
	int i;

	for (i = 0; i < MSI_MAP_SIZE; i++) {
		if (!msi_map[i].used) {
			retval = msi_map + i;
			retval->irq = MSTAR_PCIE_MSI_BASE + i;
			retval->used = true;
			break;
		}
	}

	return retval;
}

void msi_map_release(struct msi_map_entry *entry)
{
	if (entry) {
		entry->used = false;
		entry->irq = 0;
	}
}

static unsigned int msi_map_get_irq(int index)
{
	int i;

	for (i = 0; i < MSI_MAP_SIZE; i++) {
		if (msi_map[i].used && (msi_map[i].index == index)) {
			return msi_map[i].irq;
		}
	}

	return 0;
}
#endif

static inline u32 pcie_read(struct mstar_pcie_port *port, u32 reg)
{
	return readl(port->base + reg);
}

static inline void pcie_write(struct mstar_pcie_port *port, u32 val, u32 reg)
{
	/* pcie_info(port->dev, "pcie xiu write: offset: %x, val: %x\n", reg, val); */
	writel(val, port->base + reg);
}

static inline u16 pcie_riu_readw(struct mstar_pcie_port *port, u16 reg)
{
	/* reg is 16bits offset value. */
	return readw(port->riu_base + reg*4);
}

static inline void pcie_riu_writew(struct mstar_pcie_port *port, u16 val, u16 reg)
{
	/* pcie_info(port->dev, "pcie riu write: offset: %x, val: %x\n", reg, val); */
	writew(val, port->riu_base + reg*4);
}

static inline bool mstar_pcie_link_is_up(struct mstar_pcie_port *port)
{
	return (pcie_read(port, PCI_LINK_STATUS) &
		PCI_LINKUP) ? 1 : 0;
}

static int mstar_pcie_mapping_sz(u64 size)
{
	int i = 0;

	while (size) {
		size >>= 1;
		i++;
	}
	return i;
}

/**
 * mstar_pcie_check_size - Check if cfg size is valid
 * @szie: PCI cfgrd/cfgwr size
 * @where: PCI cfgrd/cfgwr register number
 * @ByteEn: byte enable field in PCI cfgrd/cfgwr header
 *
 * Return: 'true' on success and 'false' if cfg size is invalid
 */
static bool mstar_pcie_check_size(u32 size, u32 where, u32 *ByteEn)
{
	int i = 0;

	/* cfgrd/cfgwr must be on a DWORD boundary */
	if (size + (where & 3) > 4)
		return false;

	/* find the value for the 1st DW Byte Enables */
	*ByteEn = 0;
	while (i < size) {
		*ByteEn |= 1 << i;
		i++;
	}
	*ByteEn <<= (where & 0x3);

	return true;
}

static int mstar_pcie_check_cfg_cpld(struct mstar_pcie_port *port)
{
	int 	count = 10;
	u32	val;

	while (1) {
		/* 000: Success ,001: UR ,010: CRS ,100: CA ,111: Completion Timeout */
		val = pcie_read(port, APP_TLP_REQ);

		if (!(val & APP_CFG_REQ)) {
			if (val & APP_CPL_STATUS) {
				pcie_err(port->dev, "check config error %d\n", val);
				return PCIBIOS_SET_FAILED;
			}
			return PCIBIOS_SUCCESSFUL;
		}
		if (!count--) {
			pcie_err(port->dev, "check config timeout\n");
			return PCIBIOS_SET_FAILED;
		}
		mdelay(10);
	}
}

static int mstar_pcie_hw_rd_cfg(struct mstar_pcie_port *port, u32 bus, u32 devfn,
	int where, int size, u32 *val)
{
	int byte_enable = 0, reg, shift = 8 * (where & 3);

	*val = 0;
	/* check if cfgwr data is on a DWORD boundary */
	if (!mstar_pcie_check_size(size, where, &byte_enable))
		return PCIBIOS_BAD_REGISTER_NUMBER;

	/* write PCI Configuration Transaction Header for cfgrd */
	pcie_write(port, CFG_HEADER_DW0(0, 0, 4, 0), CFG_HEADER_0);
	pcie_write(port, CFG_HEADER_DW1(byte_enable, 0, 0), CFG_HEADER_1);
	pcie_write(port, CFG_HEADER_DW2(where, PCI_FUNC(devfn),
		PCI_SLOT(devfn), bus), CFG_HEADER_2);
	/* triget h/w to transmit cfgrd TLP */
	reg = pcie_read(port, APP_TLP_REQ);
	pcie_write(port, reg | APP_CFG_REQ, APP_TLP_REQ);
	/* Check complete condition */
	if (mstar_pcie_check_cfg_cpld(port))
		return PCIBIOS_SET_FAILED;
	/* read cpld payload of cfgrd */
	*val = pcie_read(port, CFG_RDATA);

	if (size == 1)
		*val = (*val >> shift) & 0xff;
	else if (size == 2)
		*val = (*val >> shift) & 0xffff;
	else if (size == 3)
		*val = (*val >> shift) & 0xffffff;

	return PCIBIOS_SUCCESSFUL;
}

static int mstar_pcie_hw_wr_cfg(struct mstar_pcie_port *port, u32 bus, u32 devfn,
	int where, int size, u32 val)

{
    int byte_enable, reg, _val, shift = 8 * (where & 3);

	/* check if cfgwr data is on a DWORD boundary */
	if (!mstar_pcie_check_size(size, where, &byte_enable))
		return PCIBIOS_BAD_REGISTER_NUMBER;

	/* write PCI Configuration Transaction Header for cfgwr */
	pcie_write(port, CFG_HEADER_DW0(0, 0, 4, 2), CFG_HEADER_0);
	pcie_write(port, CFG_HEADER_DW1(byte_enable, 0, 0), CFG_HEADER_1);
	pcie_write(port, CFG_HEADER_DW2(where, PCI_FUNC(devfn),
		PCI_SLOT(devfn), bus), CFG_HEADER_2);

	/* write cfgwr data */
	_val = val << shift;
	pcie_write(port, _val, CFG_WDATA);
	/* triget h/w to transmit cfgwr TLP */
	reg = pcie_read(port, APP_TLP_REQ);
	pcie_write(port, reg | APP_CFG_REQ, APP_TLP_REQ);
	/* Check complete condition */
	return mstar_pcie_check_cfg_cpld(port);
}

#ifdef LEGACY_INDIRECT_MMIO_MODE
static int mstar_pcie_hw_rd_mmio(struct mstar_pcie_port *port, u32 bus, u32 devfn,
	int where, int size, u32 *val)
{
	int byte_enable = 0, reg, shift = 8 * (where & 3);

	*val = 0;
	/* check if cfgwr data is on a DWORD boundary */
	if (!mstar_pcie_check_size(size, where, &byte_enable))
		return PCIBIOS_BAD_REGISTER_NUMBER;

	/* write PCI Configuration Transaction Header for cfgrd */
	pcie_write(port, CFG_HEADER_DW0(0, 0, 0, 0), CFG_HEADER_0);			/*MMIO read 1 DWORD*/
	pcie_write(port, CFG_HEADER_DW1(byte_enable, 0, 0), CFG_HEADER_1);
	pcie_write(port, where, CFG_HEADER_2);
	/* triget h/w to transmit cfgrd TLP */
	reg = pcie_read(port, APP_TLP_REQ);
	pcie_write(port, reg | APP_CFG_REQ, APP_TLP_REQ);
	/* Check complete condition */
	if (mstar_pcie_check_cfg_cpld(port))
		return PCIBIOS_SET_FAILED;
	/* read cpld payload of cfgrd */
	*val = pcie_read(port, CFG_RDATA);

	if (size == 1)
		*val = (*val >> shift) & 0xff;
	else if (size == 2)
		*val = (*val >> shift) & 0xffff;
	else if (size == 3)
		*val = (*val >> shift) & 0xffffff;

	return PCIBIOS_SUCCESSFUL;
}

static int mstar_pcie_hw_wr_mmio(struct mstar_pcie_port *port, u32 bus, u32 devfn,
	int where, int size, u32 val)

{
    int byte_enable, reg, _val, shift = 8 * (where & 3);

	/* check if cfgwr data is on a DWORD boundary */
	if (!mstar_pcie_check_size(size, where, &byte_enable))
		return PCIBIOS_BAD_REGISTER_NUMBER;

	/* write PCI Configuration Transaction Header for cfgwr */
	pcie_write(port, CFG_HEADER_DW0(0, 0, 0, 2), CFG_HEADER_0);			/*MMIO write 1 DWORD*/
	pcie_write(port, CFG_HEADER_DW1(byte_enable, 0, 0), CFG_HEADER_1);
	pcie_write(port, where, CFG_HEADER_2);

	/* write cfgwr data */
	_val = val << shift;
	pcie_write(port, _val, CFG_WDATA);
	/* triget h/w to transmit cfgwr TLP */
	reg = pcie_read(port, APP_TLP_REQ);
	pcie_write(port, reg | APP_CFG_REQ, APP_TLP_REQ);
	/* Check complete condition */
	return mstar_pcie_check_cfg_cpld(port);
}

int mstar_ob_mmio_read(struct pci_bus *bus, u32 bn, unsigned int devfn,
				   int where, int size, u32 *val)
{
	struct mstar_pcie_port *port = (struct mstar_pcie_port *) ((struct pci_sys_data *)bus->sysdata)->private_data;
	u32 ioBase, ioLen = MSTAR_PCIE_PORT0_IOMEM_LEN;

	if (port->portnum == 0)
		ioBase = MSTAR_PCIE_PORT0_IOMEM_BASE;
	else
#ifdef ENABLE_PCIE_PORT1
	if (port->portnum == 1)
		ioBase = MSTAR_PCIE_PORT1_IOMEM_BASE;
	else
#endif
	{
		ioBase = 0; 	/* illeagle base address */
		*val = 0xFFFFFFFF;
		return PCIBIOS_BAD_REGISTER_NUMBER;
	}

	/* For compatible with previous interface, use bit31 for enable memory IO and offset is not full address */
	if ((where >= INDIRECT_FLAG) && (where < ioBase))
		where = (where & 0x7FFFFFFF) + ioBase;

	if ((where < ioBase) || (where >= (ioBase+ioLen))) {
		pcie_dbg(port->dev, "[PCIE] ob mmio read with illegal offset 0x%08x !\n", where);
		*val = 0xFFFFFFFF;
		return PCIBIOS_BAD_REGISTER_NUMBER;
	}

	return mstar_pcie_hw_rd_mmio(port, bn, devfn, where, size, val);

	/*
	if (size == 1)
		*val = readb((void *)where);
	else
	if (size == 2)
		*val = readw((void *)where);
	else
		*val = readl((void *)where);

	return PCIBIOS_SUCCESSFUL;
	*/
}

int mstar_ob_mmio_write(struct pci_bus *bus, u32 bn, unsigned int devfn,
				   int where, int size, u32 val)
{
	struct mstar_pcie_port *port = (struct mstar_pcie_port *) ((struct pci_sys_data *)bus->sysdata)->private_data;
	u32 ioBase, ioLen = MSTAR_PCIE_PORT0_IOMEM_LEN;

	if (port->portnum == 0)
		ioBase = MSTAR_PCIE_PORT0_IOMEM_BASE;
	else
#ifdef ENABLE_PCIE_PORT1
	if (port->portnum == 1)
		ioBase = MSTAR_PCIE_PORT1_IOMEM_BASE;
	else
#endif
	{
		ioBase = 0; 	/* illeagle base address */
		return PCIBIOS_BAD_REGISTER_NUMBER;
	}

	/* For compatible with previous interface, use bit31 for enable memory IO and offset is not full address */
	if ((where >= INDIRECT_FLAG) && (where < ioBase))
		where = (where & 0x7FFFFFFF) + ioBase;

	if ((where < ioBase) || (where >= (ioBase+ioLen))) {
		pcie_dbg(port->dev, "[PCIE] ob mmio write with illegal offset 0x%08x !\n", where);
		return PCIBIOS_BAD_REGISTER_NUMBER;
	}

	return mstar_pcie_hw_wr_mmio(port, bn, devfn, where, size, val);

	/*
	if (size == 1)
		writeb((u8)val, (void *)where);
	else
	if (size == 2)
		writew((u16)val, (void *)where);
	else
		writel(val, (void *)where);

	return PCIBIOS_SUCCESSFUL;
	*/
}
#endif

/**
 * mstar_pcie_read_config - Read configuration space
 * @bus: PCI Bus structure
 * @devfn: Device/function
 * @where: Offset from base
 * @size: Byte/word/dword
 * @val: Value to be read
 *
 * Return: PCIBIOS_SUCCESSFUL on success
 *	   PCIBIOS_DEVICE_NOT_FOUND on failure
 */
static int mstar_pcie_read_config(struct pci_bus *bus, unsigned int devfn,
				   int where, int size, u32 *val)
{
	u32 bn = bus->number;
	struct mstar_pcie_port *port = (struct mstar_pcie_port *) ((struct pci_sys_data *)bus->sysdata)->private_data;
	int ret;
#ifdef PATCH_RC_BUSNUM_00
	u32 bn_tmp;
#endif

#ifdef PATCH_SINGLE_DEVFN
	/* exclude all other null functions, */
	/* this IP will return dirty data if pci is scan other slot(dev), fn. */
	/* this path will */
	if ((bn == port->busnum) && (devfn != port->devfn)) {
		*val = 0xFFFFFFFF;
		pcie_dbg(port->dev, "[PCIE] Read Cfg error, bn %x, port->busnum %x, devfn %x\n", bn, port->busnum, devfn);
		return PCIBIOS_DEVICE_NOT_FOUND;
	}
#endif

#ifdef LEGACY_INDIRECT_MMIO_MODE
	if ((u32)where > 0x1000) {		/* indirect mode, address > 4096 */
		ret = mstar_ob_mmio_read(bus, bn, devfn, where, size, val);
		pcie_dbg(port->dev, "MMIO Read [%d:%d:%d] offset: 0x%x, size: 0x%x, val: 0x%x\n",
					bn, PCI_SLOT(devfn), PCI_FUNC(devfn), where, size, *val);
		return ret;
	}
#endif	/* LEGACY_INDIRECT_MMIO_MODE */

#ifdef PATCH_RC_BUSNUM_00
	/*
		RC power on default bus address 0,
		PRINTK_WITHOUT_KERN_LEVELif primary bus # is not being changed,
		use bus0 as default.
	*/
	bn_tmp = bn;
	if ((bn == port->busnum) && (bn != 0))
		bn = 0;
#endif


#ifdef PATCH_RC_BUSNUM_POWER_00
	/*
		RC power on default bus address 0,
		if primary bus # is not being changed,
		use bus0 as default.
	*/
	if ((bn == port->busnum) && (bn != 0) && (port->Cfg1_18h & 0xFF) == 0x00) {
		pcie_dbg(port->dev, "[PCIE] port%d rd CfgSpace[18h] = %x\n", port->portnum, port->Cfg1_18h);
		bn = 0;
	}
#endif

	ret = mstar_pcie_hw_rd_cfg(port, bn, devfn, where, size,  val);

#ifdef PATCH_RC_BUSNUM_00
	bn = bn_tmp;
	if ((bn == port->busnum) && (bn != 0) && (where == 0x18))
		*val = port->Cfg1_18h;
#endif

	if (ret)
		*val = 0xffffffff;

#ifdef PATCH_BAR
	/*  BAR0, BAR1 is related to PCI2AXI_WIN setting, only EP use this function.
		in RC mode, needs to be patched by software.
		patch BAR 0 to return no use any io memory.	*/
	if ((bn == port->busnum) && (devfn == port->devfn) && (where == 0x10)) {
		pcie_dbg(port->dev, "[PCIE] before Patch BAR 0:%08X\n", *val);
		ret = PCIBIOS_SUCCESSFUL;
		*val = 0xFFFFFFFF;	/* 64bits BAR, non-prefetchable memory */
	}
	/* PATCH BAR 1 to return no use any io memory */
	if ((bn == port->busnum) && (devfn == port->devfn) && (where == 0x14)) {
		pcie_dbg(port->dev, "[PCIE] before Patch BAR 1:%08X\n", *val);
		ret = PCIBIOS_SUCCESSFUL;
		*val = 0xFFFFFFFF;
	}

#endif

	pcie_dbg(port->dev, "ConfigRead [%d:%d:%d] offset: 0x%x, size: 0x%x, val: 0x%x\n",
		bn, PCI_SLOT(devfn), PCI_FUNC(devfn), where, size, *val);

	return ret;
}

/**
 * mstar_pcie_write_config - Write configuration space
 * @bus: PCI Bus structure
 * @devfn: Device/function
 * @where: Offset from base
 * @size: Byte/word/dword
 * @val: Value to be written to device
 *
 * Return: PCIBIOS_SUCCESSFUL on success
 *	   PCIBIOS_DEVICE_NOT_FOUND on failure
 */
static int mstar_pcie_write_config(struct pci_bus *bus, unsigned int devfn,
				    int where, int size, u32 val)
{
	u32 bn = bus->number;
	struct mstar_pcie_port *port = (struct mstar_pcie_port *) ((struct pci_sys_data *)bus->sysdata)->private_data;
	int ret;
#if defined(PATCH_RC_BUSNUM_POWER_00) || defined(PATCH_RC_BUSNUM_00)
	u32 bn_tmp;
#ifdef PATCH_RC_BUSNUM_00
	u32 val_tmp;
#endif
#endif

#ifdef LEGACY_INDIRECT_MMIO_MODE
	if ((u32)where > 0x1000) {		/* indirect mode, address > 4096 */
		ret = mstar_ob_mmio_write(bus, bn, devfn, where, size, val);
		pcie_dbg(port->dev, "MMIO Write [%d:%d:%d] offset: 0x%x, size: 0x%x, val: 0x%x\n",
						bn, PCI_SLOT(devfn), PCI_FUNC(devfn), where, size, val);
		return ret;
	}
#endif	/* LEGACY_INDIRECT_MMIO_MODE */


#ifdef PATCH_BAR
	/*  BAR0, BAR1 is related to PCI2AXI_WIN setting, only EP use this function.
		in RC mode, needs to be patched by software.
		patch BAR 0 to return no use any io memory. */
	if ((bn == port->busnum) && (devfn == port->devfn) && (where == 0x10)) {
		pcie_dbg(port->dev, "[PCIE]Write 0 before Patch BAR0:%08X\n", val);
		val = 0x00000000;
	}
	/* PATCH BAR 1 to return no use any io memory */
	if ((bn == port->busnum) && (devfn == port->devfn) && (where == 0x14)) {
		pcie_dbg(port->dev, "[PCIE]Write 0 before Patch BAR1:%08X\n", val);
		val = 0x00000000;
	}

#endif

#ifdef PATCH_RC_BUSNUM_00
	/*
		use bus0 as default.
	*/
	bn_tmp = bn;
	val_tmp = val;
	if ((bn == port->busnum) && (bn != 0)) {
		bn = 0;
		if (where == 0x18)
			val &= 0xFFFFFF00;
	}
#endif

#ifdef PATCH_RC_BUSNUM_POWER_00
	/*
		RC power on default bus address 0,
		if primary bus # is not being changed,
		use bus0 as default.
	*/
	bn_tmp = bn;	/* backup bus number */
	if ((bn == port->busnum) && (bn != 0) && (port->Cfg1_18h & 0xFF) ==  0x00) {
		pcie_dbg(port->dev, "[PCIE] port%d wr CfgSpace[18h] = %x\n", port->portnum, port->Cfg1_18h);
		if (where == 0x18) {
			pcie_dbg(port->dev, "[PCIE] 18h write %x, force RC bus = 0\n", val);
			val &= 0xFFFFFF00;			/* force RC bus # = 0 */
		}
		bn = 0;
	}
#endif

	pcie_dbg(port->dev, "ConfigWrite [%d:%d:%d] offset: 0x%x, size: 0x%x, val: 0x%x\n",
		bn, PCI_SLOT(devfn), PCI_FUNC(devfn), where, size, val);

	ret = mstar_pcie_hw_wr_cfg(port, bn, devfn, where, size, val);

#ifdef PATCH_RC_BUSNUM_00
	/*
		if RC->primary bus # is being changed, update that bus primary #..etc
	*/
	bn = bn_tmp;		/* restore bus number */
	if ((bn == port->busnum) && (where == 0x18) && (ret == PCIBIOS_SUCCESSFUL))
		port->Cfg1_18h = val_tmp;
#endif

#ifdef PATCH_RC_BUSNUM_POWER_00
	/*
		if RC->primary bus # is being changed, update that bus primary #..etc
	*/
	bn = bn_tmp;		/* restore bus number */
	if ((bn == port->busnum) && (where == 0x18) && (ret == PCIBIOS_SUCCESSFUL)) {
		port->Cfg1_18h = val;
		pcie_dbg(port->dev, "[PCIE] port%d update CfgSpace[18h] = %x\n", port->portnum, val);
	}
#endif

	return ret;
}


/* PCIe operations */
static struct pci_ops mstar_pcie_ops = {
	.read  = mstar_pcie_read_config,
	.write = mstar_pcie_write_config,
};

#ifdef CONFIG_PCI_MSI
/* HW Interrupt Chip Descriptor */
static struct irq_chip mstar_msi_irq_chip = {
	.name = "MStar PCIe MSI",
	.irq_enable = unmask_msi_irq,
	.irq_disable = mask_msi_irq,
	.irq_mask = mask_msi_irq,
	.irq_unmask = unmask_msi_irq,
};

/* called by arch_setup_msi_irqs in drivers/pci/msi.c */
int arch_setup_msi_irq(struct pci_dev *pdev, struct msi_desc *desc)
{
	int retval = -EINVAL;
	struct msi_msg msg;
	struct msi_map_entry *map_entry = NULL;
	struct mstar_pcie_port *port = (struct mstar_pcie_port *) ((struct pci_sys_data *)pdev->bus->sysdata)->private_data;

	map_entry = msi_map_get();
	if (map_entry == NULL)
		goto exit;

	retval = irq_alloc_desc(map_entry->irq);
	if (retval < 0)
		goto exit;
	irq_set_chip_and_handler(map_entry->irq,
				&mstar_msi_irq_chip,
				handle_simple_irq);

	retval = irq_set_msi_desc(map_entry->irq, desc);
	if (retval < 0)
		goto exit;
	set_irq_flags(map_entry->irq, IRQF_VALID);

	msg.address_lo = (u32)(port->base + MSI_VECTOR);
	/* 32 bit address only */
	msg.address_hi = 0;
	msg.data = map_entry->index;

	write_msi_msg(map_entry->irq, &msg);

	retval = 0;
exit:
	if (retval != 0) {
		if (map_entry) {
			irq_free_desc(map_entry->irq);
			msi_map_release(map_entry);
		}
	}

	return retval;
}

void arch_teardown_msi_irq(unsigned int irq)
{
	int i;

	for (i = 0; i < MSI_MAP_SIZE; i++) {
		if ((msi_map[i].used) && (msi_map[i].irq == irq)) {
			irq_free_desc(msi_map[i].irq);
			msi_map_release(msi_map + i);
			break;
		}
	}
}
#endif

#ifdef CONFIG_PCI_MSI
/**
 * mstar_pcie_enable_msi - Enable MSI support
 * @port: PCIe port information
 */
static void mstar_pcie_enable_msi(struct mstar_pcie_port *port)
{
#ifdef CONFIG_PCI_MSI
	u32 mask;

	pcie_write(port, (u32)(port->base + MSI_VECTOR), IMSI_ADDR);
	mask = pcie_read(port, INT_MASK);
	mask &= ~MSI_MASK;
	pcie_write(port, mask, INT_MASK);
#endif
}
#endif

#ifdef PATCH_WED_XIU_BUS_COLLISION
/**
	\fn void pcie_set_no_xiu_access(u32 val)
*/
void pcie_set_no_xiu_access(u32 PortIdx, u32 val)
{
	/* setting xiu access action during INT */
	/* PortIdx == 0 for port0 */
	/* PortIdx != 0 for port1 */
	/* true - disable, false - enable */

	if (PortIdx == 0)
		g_pcie_ports[0].no_xiu_access = val;
	else {
#ifdef ENABLE_PCIE_PORT1
	g_pcie_ports[1].no_xiu_access = val;
#endif
	}
}
EXPORT_SYMBOL(pcie_set_no_xiu_access);
#endif
/**
 * mstar_pcie_intr_handler - Interrupt Service Handler
 * @irq: IRQ number
 * @data: PCIe port information
 *
 * Return: IRQ_HANDLED on success and IRQ_NONE on failure
 */
static irqreturn_t mstar_pcie_intr_handler(int irq, void *data)
{
	struct mstar_pcie_port *port = (struct mstar_pcie_port *)data;
	u32 mask, status;
	int index = 0;
	unsigned long val;
#ifdef CONFIG_PCI_MSI
	int pos, msi;
#endif


#ifdef PATCH_WED_XIU_BUS_COLLISION

	/* !!! special patch of no access xiu during INT only for mt7615 !!! */

	if (port->no_xiu_access) {
#ifdef PCIE_DEBUG_IRQ
		pcie_dbg(port->dev, "got a INTx with index %d - xiu patch walk around\n", index);
#endif
		generic_handle_irq(port->pin_irqs[0]);  /* mt7615 INTA */
		return IRQ_HANDLED;
	}
#endif


	/* read interrupt decode and mask registers */
	val = pcie_read(port, INT_STATUS);
	mask = pcie_read(port, INT_MASK);

#ifdef PCIE_DEBUG_IRQ
	pcie_dbg(port->dev, "pcie isr, val: 0x%x, mask: 0x%x\n", (u32)val, mask);
#endif

	status = val & ~mask;
	if (!status)
		return IRQ_NONE;

	if (status & INTX_MASK) {
		/* handle INTx Interrupt */
		val = ((val & INTX_MASK) >>
			MSTAR_PCIE_INTX_SHIFT);
		while (val) {
			val >>= 1;
			index++;
		}

#ifdef PCIE_DEBUG_IRQ
		pcie_dbg(port->dev, "got a INTx with index %d\n", index);
#endif
		generic_handle_irq(port->pin_irqs[index-1]);
	}
	if (status & MSI_STATUS) {
		val = pcie_read(port, IMSI_STATUS);
#ifdef CONFIG_PCI_MSI
		while (val > 0) {
			pos = find_first_bit(&val, 32);
			msi = msi_map_get_irq(pos);
			/* write back to clear bit (w1c)*/
			pcie_write(port, 1 << pos, IMSI_STATUS);
			val &= ~(1 << pos);
			generic_handle_irq(msi);
		}
#else
		pcie_err(port->dev, "[PCIE] get a msi intr in msi disable mode\n");
		pcie_write(port, val, IMSI_STATUS);
#endif
	}
#if 0 /* MSTAR_PORTING FIXME */
	/* PCIe port uses the same msi irq for PME, AER and HP*/
	if ((status & PM_HP_EVENT_STATUS) ||
				(status & AER_EVENT_STATUS)) {
		msi = irq_find_mapping(port->msi_irq_domain, PCIE_PORT_MSI_BIT);
		generic_handle_irq(msi);
	}
#endif
	if (status & PCIE_L2_ENTRY_WAKE_STATUS) {
		/* de-assert pe reset */
		if (!(pcie_read(port, PCI_RSTCR) & PCI_PERSTB))
			pcie_write(port, pcie_read(port, PCI_RSTCR) |
				PCI_PERSTB, PCI_RSTCR);
		/* wake up system from L2 state */
		val = pcie_read(port, PCI_WAKE_CONTROL);
		pcie_write(port, val | CLKREQ_N_ENABLE, PCI_WAKE_CONTROL);
		pcie_write(port, val & ~CLKREQ_N_ENABLE, PCI_WAKE_CONTROL);
	}
	/* clear the Interrupt Decode register */
	pcie_write(port, status, INT_STATUS);

	return IRQ_HANDLED;
}

/**
 * mstar_pcie_config_init
 * @port: pcie port information
 * return
*/
void mstar_pcie_config_init(struct mstar_pcie_port *port)
{
	struct device *dev = port->dev;

	pcie_info(dev, "PCIE configuration init...\n");
	/* set PCI_K_CONF_FUNC0_0 VID/PID */
	pcie_write(port, (MSTAR_DEV_ID << 16) + MSTAR_VEND_ID, K_CONF_FUNC0_0);
	/* set PCI_K_CONF_FUNC0_1 Class/Revision */
	pcie_write(port, (MSTAR_RC_CLASS << 16) + MSTAR_RC_REVISION, K_CONF_FUNC0_1);
	/* set PCI_K_CONF_FUNC0_2 susbys VID/PID */
	pcie_write(port, (MSTAR_DEV_ID << 16) + MSTAR_VEND_ID, K_CONF_FUNC0_2);
	/* set PCI_K_CONF_FUNC0_3 */
	pcie_write(port, (pcie_read(port, K_CONF_FUNC0_3) & ~K_CONF_FUNC0_3_MASK) \
						| SET_K_CONF_FUNC0_3, K_CONF_FUNC0_3);
	/* set PCI_K_CONF_FUNC0_4 1 lane */
	pcie_write(port, (pcie_read(port, K_CONF_FUNC0_4) & ~K_CONF_FUNC0_4_MASK) \
						| SET_K_CONF_FUNC0_4, K_CONF_FUNC0_4);
	/* Set PCI_K_CONF_FUNC0_12 ? */
	pcie_write(port, (pcie_read(port, K_CONF_FUNC0_12) & ~K_RXL0S_EXIT_TIME_MASK) \
						| K_RXL0S_EXIT_TIME, K_CONF_FUNC0_12);
	/* set PCI_K_FC_VC0_0 */
	pcie_write(port, SET_K_FC_VC0_0, K_FC_VC0_0);
	/* set PCI_K_FC_VC0_1 */
	pcie_write(port, SET_K_FC_VC0_1, K_FC_VC0_1);
	/* set PCI_K_CNT_2 */
	pcie_write(port, (pcie_read(port, K_CNT_2) & ~K_CNT_2_MASK) | SET_K_CNT_2, K_CNT_2);
	/* Set PCI_REG_PHYMAC_CONF ? */
	pcie_write(port, (pcie_read(port, REG_PHYMAC_CONF) & ~REC_RXCFG_TXTS2_NUM_MASK) | REC_RXCFG_TXTS2_NUM, REG_PHYMAC_CONF);
}




static void mstar_pcie_rc_init(struct mstar_pcie_port *port)
{
	u64   tVal;
/*
	// init cpu to pcie base addr
	regVal = readw(port->riu_base +  REG_CPU2PCIE_ADDR_SEG*4);
	printk("[PCIE] cpu to pcie address MSB[31:28](default): %04X\n", regVal & 0xFFFF);
	writew(0x00EE, port->riu_base +  REG_CPU2PCIE_ADDR_SEG*4);
	printk("[PCIE] cpu to pcie address MSB[31:28]         : %04X\n", readw(port->riu_base +  REG_CPU2PCIE_ADDR_SEG*4) & 0xFFFF);
*/
	/* init miu0 address */
	tVal = MIU0_BUS_BASE_ADDR >> 20;
	pcie_riu_writew(port, tVal, REG_MIU0_START_ADDR0);
	pcie_riu_writew(port, tVal, REG_MIU0_TLB_START_ADDR0);
	tVal >>= 16;
	pcie_riu_writew(port, tVal, REG_MIU0_START_ADDR1);
	pcie_riu_writew(port, tVal, REG_MIU0_TLB_START_ADDR1);
	tVal >>= 16;
	pcie_riu_writew(port, tVal, REG_MIU0_START_ADDR2);
	pcie_riu_writew(port, tVal, REG_MIU0_TLB_START_ADDR2);

	tVal = (MIU0_BUS_BASE_ADDR + MIU0_BUS_LENGTH) >> 20;
	pcie_riu_writew(port, tVal, REG_MIU0_END_ADDR0);
	pcie_riu_writew(port, tVal, REG_MIU0_TLB_END_ADDR0);
	tVal >>= 16;
	pcie_riu_writew(port, tVal, REG_MIU0_END_ADDR1);
	pcie_riu_writew(port, tVal, REG_MIU0_TLB_END_ADDR1);
	tVal >>= 16;
	pcie_riu_writew(port, tVal, REG_MIU0_END_ADDR2);
	pcie_riu_writew(port, tVal, REG_MIU0_TLB_END_ADDR2);


	pcie_dbg(port->dev, "[PCIE]: MIU0 base =%lx\n", MIU0_BUS_BASE_ADDR);
	/* disable miu1/2 */
	/* tVal=pcie_riu_readw(port, REG_AXI2MIU_SETTING) & ~REG_MIU_ENABLE_MASK; */
	/* pcie_riu_writew(port, tVal | REG_BIT_MIU0_ENABLE, REG_AXI2MIU_SETTING); */
	pcie_dbg(port->dev, "[PCIE] AXI2MIU_SETTING=%08X\n", pcie_riu_readw(port, REG_AXI2MIU_SETTING));

#ifdef PCIE_DEBUG_RC_TOP
	pcie_dbg(port->dev, "[PCIE] MIU0_START_ADDR0 %x\n", pcie_riu_readw(port, REG_MIU0_START_ADDR0));
	pcie_dbg(port->dev, "[PCIE] MIU0_START_ADDR1 %x\n", pcie_riu_readw(port, REG_MIU0_START_ADDR1));
	pcie_dbg(port->dev, "[PCIE] MIU0_START_ADDR2 %x\n", pcie_riu_readw(port, REG_MIU0_START_ADDR2));
	pcie_dbg(port->dev, "[PCIE] MIU0_END_ADDR0 %x\n", pcie_riu_readw(port, REG_MIU0_END_ADDR0));
	pcie_dbg(port->dev, "[PCIE] MIU0_END_ADDR1 %x\n", pcie_riu_readw(port, REG_MIU0_END_ADDR1));
	pcie_dbg(port->dev, "[PCIE] MIU0_END_ADDR2 %x\n", pcie_riu_readw(port, REG_MIU0_END_ADDR2));

	pcie_dbg(port->dev, "[PCIE] MIU0_TLB_START_ADDR0 %x\n", pcie_riu_readw(port, REG_MIU0_TLB_START_ADDR0));
	pcie_dbg(port->dev, "[PCIE] MIU0_TLB_START_ADDR1 %x\n", pcie_riu_readw(port, REG_MIU0_TLB_START_ADDR1));
	pcie_dbg(port->dev, "[PCIE] MIU0_TLB_START_ADDR2 %x\n", pcie_riu_readw(port, REG_MIU0_TLB_START_ADDR2));
	pcie_dbg(port->dev, "[PCIE] MIU0_TLB_END_ADDR0 %x\n", pcie_riu_readw(port, REG_MIU0_TLB_END_ADDR0));
	pcie_dbg(port->dev, "[PCIE] MIU0_TLB_END_ADDR1 %x\n", pcie_riu_readw(port, REG_MIU0_TLB_END_ADDR1));
	pcie_dbg(port->dev, "[PCIE] MIU0_TLB_END_ADDR2 %x\n", pcie_riu_readw(port, REG_MIU0_TLB_END_ADDR2));
#endif

	/* init miu1 address */
/* #if defined(MIU1_BUS_BASE_ADDR) */
	tVal = MIU1_BUS_BASE_ADDR>>20;
	pcie_riu_writew(port, tVal, REG_MIU1_START_ADDR0);
	pcie_riu_writew(port, tVal, REG_MIU1_TLB_START_ADDR0);
	tVal >>= 16;
	pcie_riu_writew(port, tVal, REG_MIU1_START_ADDR1);
	pcie_riu_writew(port, tVal, REG_MIU1_TLB_START_ADDR1);
	tVal >>= 16;
	pcie_riu_writew(port, tVal, REG_MIU1_START_ADDR2);
	pcie_riu_writew(port, tVal, REG_MIU1_TLB_START_ADDR2);


	tVal = (MIU1_BUS_BASE_ADDR + MIU1_BUS_LENGTH)>>20;
	pcie_riu_writew(port, tVal, REG_MIU1_END_ADDR0);
	pcie_riu_writew(port, tVal, REG_MIU1_TLB_END_ADDR0);
	tVal >>= 16;
	pcie_riu_writew(port, tVal, REG_MIU1_END_ADDR1);
	pcie_riu_writew(port, tVal, REG_MIU1_TLB_END_ADDR1);
	tVal >>= 16;
	pcie_riu_writew(port, tVal, REG_MIU1_END_ADDR2);
	pcie_riu_writew(port, tVal, REG_MIU1_TLB_END_ADDR2);

	/* enable miu1 */
	pcie_riu_writew(port, pcie_riu_readw(port, REG_AXI2MIU_SETTING) | REG_BIT_MIU1_ENABLE, \
			REG_AXI2MIU_SETTING);
/* #endif */

#ifdef PCIE_DEBUG_RC_TOP
	pcie_dbg(port->dev, "[PCIE] MIU1_START_ADDR0 %x\n", pcie_riu_readw(port, REG_MIU1_START_ADDR0));
	pcie_dbg(port->dev, "[PCIE] MIU1_START_ADDR1 %x\n", pcie_riu_readw(port, REG_MIU1_START_ADDR1));
	pcie_dbg(port->dev, "[PCIE] MIU1_START_ADDR2 %x\n", pcie_riu_readw(port, REG_MIU1_START_ADDR2));
	pcie_dbg(port->dev, "[PCIE] MIU1_END_ADDR0 %x\n", pcie_riu_readw(port, REG_MIU1_END_ADDR0));
	pcie_dbg(port->dev, "[PCIE] MIU1_END_ADDR1 %x\n", pcie_riu_readw(port, REG_MIU1_END_ADDR1));
	pcie_dbg(port->dev, "[PCIE] MIU1_END_ADDR2 %x\n", pcie_riu_readw(port, REG_MIU1_END_ADDR2));

	pcie_dbg(port->dev, "[PCIE] MIU1_TLB_START_ADDR0 %x\n", pcie_riu_readw(port, REG_MIU1_TLB_START_ADDR0));
	pcie_dbg(port->dev, "[PCIE] MIU1_TLB_START_ADDR1 %x\n", pcie_riu_readw(port, REG_MIU1_TLB_START_ADDR1));
	pcie_dbg(port->dev, "[PCIE] MIU1_TLB_START_ADDR2 %x\n", pcie_riu_readw(port, REG_MIU1_TLB_START_ADDR2));
	pcie_dbg(port->dev, "[PCIE] MIU1_TLB_END_ADDR0 %x\n", pcie_riu_readw(port, REG_MIU1_TLB_END_ADDR0));
	pcie_dbg(port->dev, "[PCIE] MIU1_TLB_END_ADDR1 %x\n", pcie_riu_readw(port, REG_MIU1_TLB_END_ADDR1));
	pcie_dbg(port->dev, "[PCIE] MIU1_TLB_END_ADDR2 %x\n", pcie_riu_readw(port, REG_MIU1_TLB_END_ADDR2));
#endif



	/* init miu2 address */
/* #if defined(MIU2_BUS_BASE_ADDR) */
	tVal = MIU2_BUS_BASE_ADDR >> 20;
	pcie_riu_writew(port, tVal, REG_MIU2_START_ADDR0);
	pcie_riu_writew(port, tVal, REG_MIU2_TLB_START_ADDR0);
	tVal >>= 16;
	pcie_riu_writew(port, tVal, REG_MIU2_START_ADDR1);
	pcie_riu_writew(port, tVal, REG_MIU2_TLB_START_ADDR1);
	tVal >>= 16;
	pcie_riu_writew(port, tVal, REG_MIU2_START_ADDR2);
	pcie_riu_writew(port, tVal, REG_MIU2_TLB_START_ADDR2);

	tVal = (MIU2_BUS_BASE_ADDR + MIU2_BUS_LENGTH)>>20;
	pcie_riu_writew(port, tVal, REG_MIU2_END_ADDR0);
	pcie_riu_writew(port, tVal, REG_MIU2_TLB_END_ADDR0);
	tVal >>= 16;
	pcie_riu_writew(port, tVal, REG_MIU2_END_ADDR1);
	pcie_riu_writew(port, tVal, REG_MIU2_TLB_END_ADDR1);
	tVal >>= 16;
	pcie_riu_writew(port, tVal, REG_MIU2_END_ADDR2);
	pcie_riu_writew(port, tVal, REG_MIU2_TLB_END_ADDR2);

	/* enable miu2 */
	pcie_riu_writew(port, pcie_riu_readw(port, REG_AXI2MIU_SETTING) | REG_BIT_MIU2_ENABLE, \
			REG_AXI2MIU_SETTING);
/* #endif */

#ifdef PCIE_DEBUG_RC_TOP
	pcie_dbg(port->dev, "[PCIE] MIU2_START_ADDR0 %x\n", pcie_riu_readw(port, REG_MIU2_START_ADDR0));
	pcie_dbg(port->dev, "[PCIE] MIU2_START_ADDR1 %x\n", pcie_riu_readw(port, REG_MIU2_START_ADDR1));
	pcie_dbg(port->dev, "[PCIE] MIU2_START_ADDR2 %x\n", pcie_riu_readw(port, REG_MIU2_START_ADDR2));
	pcie_dbg(port->dev, "[PCIE] MIU2_END_ADDR0 %x\n", pcie_riu_readw(port, REG_MIU2_END_ADDR0));
	pcie_dbg(port->dev, "[PCIE] MIU2_END_ADDR1 %x\n", pcie_riu_readw(port, REG_MIU2_END_ADDR1));
	pcie_dbg(port->dev, "[PCIE] MIU2_END_ADDR2 %x\n", pcie_riu_readw(port, REG_MIU2_END_ADDR2));

	pcie_dbg(port->dev, "[PCIE] MIU2_TLB_START_ADDR0 %x\n", pcie_riu_readw(port, REG_MIU2_TLB_START_ADDR0));
	pcie_dbg(port->dev, "[PCIE] MIU2_TLB_START_ADDR1 %x\n", pcie_riu_readw(port, REG_MIU2_TLB_START_ADDR1));
	pcie_dbg(port->dev, "[PCIE] MIU2_TLB_START_ADDR2 %x\n", pcie_riu_readw(port, REG_MIU2_TLB_START_ADDR2));
	pcie_dbg(port->dev, "[PCIE] MIU2_TLB_END_ADDR0 %x\n", pcie_riu_readw(port, REG_MIU2_TLB_END_ADDR0));
	pcie_dbg(port->dev, "[PCIE] MIU2_TLB_END_ADDR1 %x\n", pcie_riu_readw(port, REG_MIU2_TLB_END_ADDR1));
	pcie_dbg(port->dev, "[PCIE] MIU2_TLB_END_ADDR2 %x\n", pcie_riu_readw(port, REG_MIU2_TLB_END_ADDR2));
#endif

	/*pcie_riu_writew(port, pcie_riu_readw(port, REG_AXI2MIU_SETTING)
						| REG_BIT_AXI2MI_SRST, REG_AXI2MIU_SETTING);
	  printk("[PCIE] AXI2MIU_SETTING %08x\n", pcie_riu_readw(port, REG_AXI2MIU_SETTING));
	*/

	/* set 64bit address */
	pcie_riu_writew(port, REG_BIT_64BIT_WFUL_ON, REG_64BIT_WFUL_ON);

	pcie_dbg(port->dev, "[PCIE] 64bit wful on %x\n", pcie_riu_readw(port, REG_64BIT_WFUL_ON));

#ifdef ECO01_ON
	pcie_info(port->dev, "[PCIE]Mask InBWr to AXIBVLID\n");
	pcie_riu_writew(port, pcie_riu_readw(port, REG_DESIGN_OPTION) | REG_BIT_MASK_INBWRT_AXIBVLD_EN, REG_DESIGN_OPTION);
	/*pcie_info(port->dev, "[PCIE]BValid depends on LDZ\n");*/
	/*pcie_riu_writew(port, (pcie_riu_readw(port, REG_AXI2MIU_SETTING) & ~REG_BVALID_LDZ_OPT_MASK) | REG_BVALID_DEPENDS_ON_LDZ, REG_AXI2MIU_SETTING);*/
#endif


#ifdef CONFIG_MSTAR_ARM_BD_FPGA
	/* for debug purpose, set debug mask on, testbus select 0x01 */
	pcie_riu_writew(port, (!REG_BIT_DEBUG_MASK_DISABLE)+REG_TEST_BUS_1B, REG_05);

	/* if cpu clock is slow, enable this bit to accomodate the cpu clock. */
	pcie_riu_writew(port, pcie_riu_readw(port, REG_36) | REG_BIT_DELAY_FLUSH_REQ, REG_36);
#endif /* CONFIG_MSTAR_ARM_BD_FPGA */

#ifdef ENABLE_L3_CACHE
	/* ltssem, L3, MI Dynamic */
	pcie_dbg(port->dev, "[PCIE] ltssm enabled, MI Dynamic request, L3 cache enable\n");

	/* acpu pcie flush enable, 16bit address, bit 8 */
	writeb(0x01 | readb((void *)((MSTAR_PM_BASE+0x101880*2)+0x3D*4+1)), (void *)((MSTAR_PM_BASE+0x101880*2)+0x3D*4+1));
	printk("[PCIE] acpu L3 flush enabled\n");

	/* scpu pcie flush enable, 16bit address, bit 1 */
	/*
	writeb(0x02 | readb((void *)((MSTAR_PM_BASE+0x113C80*2)+0x37*4)), (void *)((MSTAR_PM_BASE+0x113C80*2)+0x37*4));
	printk("[PCIE] scpu L3 flush enabled\n");
	*/
	/* NOE pcie flush enable, 16bit address */
	/*
	writew(0,      (void *)((MSTAR_PM_BASE+0x100A00*2)+0x72*4));
	writew(0,      (void *)((MSTAR_PM_BASE+0x100A00*2)+0x73*4));
	writew(0x4000, (void *)((MSTAR_PM_BASE+0x121200*2)+0x0A*4));
	writew(0x4000, (void *)((MSTAR_PM_BASE+0x121280*2)+0x0A*4));
	*/
	pcie_riu_writew(port, pcie_riu_readw(port, REG_DESIGN_OPTION) \
		| (REG_BIT_LTSSM_EN + REG_BIT_L3_FLUSH_ENABLE + REG_BIT_MI_DYNAMIC_REQUEST), REG_DESIGN_OPTION);
#else
	/* ltssem MI Dynamic */
	pcie_dbg(port->dev, "[PCIE] ltssm enabled & MI Dynamic request\n");

	/*  apmcu pcie flush enable, 16bit address
		writeb(0x01 | readb((MSTAR_PM_BASE+0x101880*2)+0x3D*4+1), (MSTAR_PM_BASE+0x101880*2)+0x3D*4+1);
	*/

	pcie_riu_writew(port, pcie_riu_readw(port, REG_DESIGN_OPTION) \
		| (REG_BIT_LTSSM_EN + REG_BIT_MI_DYNAMIC_REQUEST), REG_DESIGN_OPTION);
#endif

	/*  XIU
		pcie_write(port, (0xE0000000 + 0x40 + 0x1C), AHB2PCIE_BASE0_L);
		pcie_write(port, 0, AHB2PCIE_BASE0_H);
	*/

}


/**
 * mstar_pcie_init_hw - Initialize mstar PCIe host hardware
 * @port: PCIe port information
 * @res: Bus Resources
 *
 * Return: '0' on success and error value on failure
 */
static int mstar_pcie_init_hw(struct mstar_pcie_port *port)
{
	int size, count = 0, Err_Return = 0;
	struct device *dev = port->dev;
	struct resource *pref = &port->pref_mem;
	struct resource *npref = &port->npref_mem;

#ifdef FPGA_PCIE_PHY_IIC
	pcie_dbg(port->dev, "[PCIE] MDrv_IIC_init\n");

	if (PCIE_PHY_INIT(port->portnum)) {
		pr_info("[PCIE] FPGA phy lan[0:3] init return non-00h!!\n");
	}

	/* PCIE_PHY_LoopBack(); */
#endif


	/* set pcie as RC root port*/
	pcie_write(port, PCIE_IS_RC_ROOT_PORT, K_GBL_1);

	/* Add MStar PCI MAC top init function*/
	mstar_pcie_config_init(port);

	pcie_info(dev, "PCIE init hw...\n");
	/* rc internal reset will work when Link state is from link up to link down. */
	pcie_write(port, PCI_LINKDOWN_RST_EN, PCI_RSTCR);
	/* de-assert reset  signals */
	pcie_write(port, pcie_read(port, PCI_RSTCR) |
		PCI_RST_DEASSERTED, PCI_RSTCR);
	mdelay(100);
	/* de-assert pe reset */
	pcie_write(port, pcie_read(port, PCI_RSTCR) |
		PCI_PERSTB, PCI_RSTCR);

	/* Add MStar RC top init function */
	mstar_pcie_rc_init(port);

	/*  init mac top
		set PCI_INT_MASK
		set PCI_IMSI_ADDR ?
		set PCI_1LANE_CAP
		set PCI_REG_WAKE_CONTROL
		set LTSSM enable
	*/




	/* check if the link is up or not */
	while (!mstar_pcie_link_is_up(port)) {
		mdelay(10);
		count++;
		if (count == 10) {
			pcie_err(dev, "PCIe link fail\n");
			Err_Return = -EINVAL;
			break;
			/* return -EINVAL; */
		}
	}

	if (!Err_Return)
		pcie_info(dev, "PCIE link is up\n");
	/* set INT mask */
	pcie_write(port, pcie_read(port, INT_MASK) & ~(INTX_MASK |
		PCIE_L2_ENTRY_WAKE_MASK), INT_MASK);
	/* set AHB to PCIe translation windows for prefetchable and non-prefetchable memory */
	if (pref->flags & IORESOURCE_MEM) {
		size = pref->end - pref->start;
		pcie_write(port, (AHB2PCIE_BASEL(pref->start) |
				NOPREFETCH(IORESOURCE_PREFETCH) | BASE_SIZE(mstar_pcie_mapping_sz(size))),
		AHB2PCIE_BASE1_L);
	#ifdef CONFIG_ARM64  /* Fix compile error in 32bit env.*/
		pcie_write(port, AHB2PCIE_BASEH(pref->start), AHB2PCIE_BASE1_H);
	#else
		pcie_write(port, 0, AHB2PCIE_BASE1_H);
	#endif
	}

	if (npref->flags & IORESOURCE_MEM) {
		size = npref->end - npref->start;
		pcie_write(port, (AHB2PCIE_BASEL(npref->start) |
			NOPREFETCH(0) | BASE_SIZE(mstar_pcie_mapping_sz(size))),
		AHB2PCIE_BASE0_L);

	pcie_dbg(port->dev, "AHB2PCIE BASE = %08X\n", pcie_read(port, AHB2PCIE_BASE0_L));

	#ifdef CONFIG_ARM64  /* Fix compile error in 32bit env. */
		pcie_write(port, AHB2PCIE_BASEH(npref->start), AHB2PCIE_BASE0_H);
	#else
		pcie_write(port, 0, AHB2PCIE_BASE0_H);
	#endif
	}

	/* set PCIe to axi translation memory space.*/
	pcie_write(port, mstar_pcie_mapping_sz(PCIE2AXI_SIZE) | WIN_ENABLE, PCIE2AXI_WIN);
	pcie_dbg(port->dev, "[PCIE] PCIE2AXI_WIN=%08X\n", pcie_read(port, PCIE2AXI_WIN));

#if 0
	/* set debug mode registers */
	pcie_write(port, 0x03030303, DBG_MODE_SEL);
	printk("[PCIE] Set Debug mode register=%08X\n", pcie_read(port, DBG_MODE_SEL));
#endif

#if 0
	pcie_dbg(port->dev, "[PCIE] BAR0=%x", pcie_read(port, CFG_BAR0));
	pcie_dbg(port->dev, "[PCIE] BAR1=%x", pcie_read(port, CFG_BAR1));
	pcie_dbg(port->dev, "[PCIE] BAR2=%x", pcie_read(port, CFG_BAR2));
	pcie_dbg(port->dev, "[PCIE] BAR3=%x", pcie_read(port, CFG_BAR3));
	pcie_dbg(port->dev, "[PCIE] BAR4=%x", pcie_read(port, CFG_BAR4));
	pcie_dbg(port->dev, "[PCIE] BAR5=%x", pcie_read(port, CFG_BAR5));
#endif

	/* if Err_return is true, means link is fail..so far only this condition meet. */
	if (Err_Return)
		return Err_Return;
	return 0;
}

static int mstar_pcie_setup(int nr, struct pci_sys_data *sys)
{
	struct pci_host_bridge_window *win;
	struct mstar_pcie_port *port = (struct mstar_pcie_port *) sys->private_data;

	port->busnum = sys->busnr;
	pcie_info(port->dev, "setup for busnum: %d\n", sys->busnr);

	list_for_each_entry(win, &port->resources, list)
	{
		pci_add_resource_offset(
			&sys->resources, win->res, sys->mem_offset);
	}

	return 1;
}

static int mstar_pcie_map_irq(const struct pci_dev *dev, u8 slot, u8 pin)
{
	struct mstar_pcie_port *port = (struct mstar_pcie_port *) ((struct pci_sys_data *)dev->bus->sysdata)->private_data;

	pcie_info(port->dev, "map [slot: %d: pin %d] to irq %d\n", slot, pin, port->pin_irqs[pin-1]);

	return port->pin_irqs[pin-1];
}

static struct pci_bus *__init mstar_pcie_scan_bus(int nr,
						  struct pci_sys_data *sys)
{
	return pci_scan_root_bus(NULL, sys->busnr, &mstar_pcie_ops, sys,
				 &sys->resources);
}

void *mstar_hw_private[PCIE_PORT_COUNT];

static struct hw_pci __initdata mstar_pcie_hw = {
	.setup		= mstar_pcie_setup,
	.scan		= mstar_pcie_scan_bus,
	.map_irq	= mstar_pcie_map_irq,
};


static int mstar_pcie_port_init(struct mstar_pcie_port *port)
{
	struct device *dev = port->dev;
	struct resource *mem, *io_res;
	struct pci_host_bridge_window *win;
	int err = 0;


	/* register memory resource */
	mem = (struct resource *)devm_kzalloc(dev, sizeof(*mem), GFP_KERNEL);
	mem->name = "Mstar pcie rc memory";
	mem->start = port->iomem_base;
	mem->end = port->iomem_base + port->iomem_len - 1;
	mem->flags = IORESOURCE_MEM;
	memcpy(&port->npref_mem, mem, sizeof(*mem));

	pcie_info(port->dev, "request iomem 0x%x, len: 0x%x\n",
		(unsigned int)port->iomem_base, (unsigned int)port->iomem_len);

	err = request_resource(&iomem_resource, mem);
	if (err) {
		pcie_err(port->dev, "[PCIE] request PCI memory failed(start: 0x%lx, len: 0x%lx)\n",
			port->iomem_base, port->iomem_len);
		goto free_list_entry;
	}
	pci_add_resource(&port->resources, mem);

	/* register io resource */
	io_res = (struct resource *)devm_kzalloc(dev, sizeof(*io_res), GFP_KERNEL);
	*io_res = (struct resource) {
		.name	= "Mstar pcie io",
		.start	= (port->portnum+1) * 0x1000,
		.end	= (port->portnum+2) * 0x1000 - 1,
		.flags	= IORESOURCE_IO,
	};
	port->io_res = io_res;

	pcie_info(port->dev, "request io 0x%x, len: 0x%x\n", (unsigned int)port->io_res->start, (unsigned int)port->io_res->end);

	pci_add_resource(&port->resources, io_res);

#if 1
	/* register irq */
	pcie_info(port->dev, "request irq %d\n", port->irq);
	err = request_irq(port->irq, mstar_pcie_intr_handler, IRQF_SHARED, "PCIe", port);
	if (err) {
		pcie_err(dev, "failed to register IRQ: %d\n", err);
		goto free_resources;
	}
#endif

#ifdef CONFIG_PCI_MSI
	mstar_pcie_enable_msi(port);
#endif

#if 1
	err = mstar_pcie_init_hw(port);
	if (err) {
		pcie_err(dev, "failed to init pcie h/w\n");
		/* return err; */			/* don't report error if link failed. */
	}
#endif


	return 0;

free_resources:
free_list_entry:
	list_for_each_entry(win, &port->resources, list)
		devm_kfree(dev, win->res);
	pci_free_resource_list(&port->resources);

	return err;
}

/**
 * mstar_pcie_probe - Probe function
 * @pdev: Platform device pointer
 *
 * Return: '0' on success and error value on failure
 */
static int __init mstar_pcie_probe(struct platform_device *pdev)
{
	struct mstar_pcie_port *port = NULL;
	int err;

	pcie_info(&pdev->dev, "pcie probe...\n");
#ifdef ENABLE_PCIE_PORT0
	if (strncmp(pdev->name, MSTAR_PCIE_PORT0_NAME, strlen(MSTAR_PCIE_PORT0_NAME)) == 0) {
		port = &g_pcie_ports[0];
	} else
#endif
#ifdef ENABLE_PCIE_PORT1
	if (strncmp(pdev->name, MSTAR_PCIE_PORT1_NAME, strlen(MSTAR_PCIE_PORT1_NAME)) == 0) {
		port = &g_pcie_ports[1];
	} else
#endif
	{
		pr_info("[PCIE] No PCIE driver found\n");
		return -ENODEV;
	}

	if (port == NULL) {
		pcie_err(&pdev->dev, "can't not found %s\n", pdev->name);
		return -ENODEV;
	}

	port->dev = &pdev->dev;

	/*
	 * parse PCI ranges, configuration bus range and
	 * request their resources
	 */
	INIT_LIST_HEAD(&port->resources);
	err = mstar_pcie_port_init(port);
	if (err) {
		pcie_err(&pdev->dev, "failed to init port (err=%d)\n", err);
		return err;
	}

	platform_set_drvdata(pdev, port);

	mstar_hw_private[mstar_pcie_hw.nr_controllers++] = (void *) port;

	return 0;
}
/**
 * mstar_pcie_remove - Remove function
 * @pdev: Platform device pointer
 *
 * Return: '0' always
 */
static int mstar_pcie_remove(struct platform_device *pdev)
{
	/* struct mstar_pcie_port *port = (struct mstar_pcie_port *) platform_get_drvdata(pdev); */

	pcie_info(&pdev->dev, "PCIE remove\n");
	return 0;
}

static struct platform_driver mstar_pcie_0_driver = {
	.driver = {
		.name = MSTAR_PCIE_PORT0_NAME,
		.owner = THIS_MODULE,
	},
	.probe = mstar_pcie_probe,
	.remove = mstar_pcie_remove,
};

#ifdef ENABLE_PCIE_PORT1
static struct platform_driver mstar_pcie_1_driver = {
	.driver = {
		.name = MSTAR_PCIE_PORT1_NAME,
		.owner = THIS_MODULE,
	},
	.probe = mstar_pcie_probe,
	.remove = mstar_pcie_remove,
};
#endif

#ifdef PCIE_TEST_MMIO_REGISTERS
void __iomem *pcie_mmio_start_addr;

void pcie_mmio_read_test(void)
{
	u8 	*u8_addr;
	unsigned int 	len, ii;
	unsigned int mt7615reg[] = {0x41F0, 0x1000, 0x1004, 0x1008, 0x4000};

	u32 val;
	u8_addr = (u8 *)pcie_mmio_start_addr;
	len = ARRAY_SIZE(mt7615reg);

	val = 0x02000000;
	pr_info("PCIE MMIO Write DWORD:%08X to mmio %08X\n", val, (u32)u8_addr+mt7615reg[0]);
	writel(val, (u8_addr+mt7615reg[0]));

	pr_info("PCIE MMIO Read DWORD test...%08X\n", (u32)u8_addr);

	for (ii = 0; ii < len; ii++) {
		/* val = *(u8_addr + mt7615reg[ii]); */
		val = readl((void *) (u8_addr + mt7615reg[ii]));

		pr_info("%08x: %08x\n", (unsigned int)mt7615reg[ii], val);
	}
}

void pcie_mmio_write_test(void)
{
	u8 	*u8_addr;
	unsigned int	len, ii;

	u8_addr = (u8 *)pcie_mmio_start_addr;
	len = 0x100;

	pr_info("PCIE MMIO Write byte test...\n");

	for (ii = 0; ii < len; ii++) {
		*(u8_addr + ii) = 0x5A;
	}
	pr_info("write len %x done\n", len);
}
#endif

#ifdef PCIE_ENABLE_5G_LINK
#if 0
void mstar_pcie_change_speed(struct mstar_pcie_port *port)
{

	u32 tmp = 0;
	u32 count = 0;

	mstar_pcie_hw_rd_cfg(port, 0, 0, 0x90, 4, &tmp);
	tmp |= 0x00000020;
	mstar_pcie_hw_wr_cfg(port, 0, 0, 0x90, 4, tmp);

	do {
		msleep(1);
		tmp = mstar_pcie_hw_rd_cfg(port, 0, 0, 0x90, 4, &tmp);

		tmp &= 0x000F0000;
		pcie_dbg(port->dev, "[PCIE] pcie_change_speed(), tmp = %08X\n", tmp);
		count++;

		if (count == 30)
			break;
    } while (tmp != 0x00020000);

	if (tmp & 0x00020000)
		pcie_dbg(port->dev, "[PCIE] link 5G\n");
	else
		pcie_dbg(port->dev, "[PCIE] link 5G failed\n");
}
#endif

static bool mstar_pcie_change_link_speed(struct pci_dev *pdev, bool isGen2)
{
	u16 val, link_up_spd, link_dn_spd, count;
	struct pci_dev *up_dev, *dn_dev;

	/* skip if current device is not PCI express capable */
	/* or is either a root port or downstream port */
	if (!pci_is_pcie(pdev))
		goto skip;

	if ((pci_pcie_type(pdev) == PCI_EXP_TYPE_DOWNSTREAM) ||
		(pci_pcie_type(pdev) == PCI_EXP_TYPE_ROOT_PORT))
		goto skip;

	pcie_info(&pdev->dev, "try to change link speed...\n");

	/* initialize upstream/endpoint and downstream/root port device ptr */
	up_dev = pdev;
	dn_dev = pdev->bus->self;

	/* read link status register to find current speed */
	pcie_capability_read_word(up_dev, PCI_EXP_LNKSTA, &link_up_spd);
	link_up_spd &= PCI_EXP_LNKSTA_CLS;
	pcie_info(&pdev->dev, "Endpoint current speed: 0x%x\n", link_up_spd);
	pcie_capability_read_word(dn_dev, PCI_EXP_LNKSTA, &link_dn_spd);
	link_dn_spd &= PCI_EXP_LNKSTA_CLS;
	pcie_info(&pdev->dev, "RC current speed: 0x%x\n", link_dn_spd);

	/* skip if both devices across the link are already trained to gen2 */
	if (isGen2 &&
		(link_up_spd == PCI_EXP_LNKSTA_CLS_5_0GB) &&
		(link_dn_spd == PCI_EXP_LNKSTA_CLS_5_0GB))
		goto skip;
	/* skip if both devices across the link are already trained to gen1 */
	else if (!isGen2 &&
		((link_up_spd == PCI_EXP_LNKSTA_CLS_2_5GB) ||
		(link_dn_spd == PCI_EXP_LNKSTA_CLS_2_5GB)))
		goto skip;

	/* read link capability register to find max speed supported */
	pcie_capability_read_word(up_dev, PCI_EXP_LNKCAP, &link_up_spd);
	link_up_spd &= PCI_EXP_LNKCAP_SLS;
	pcie_info(&pdev->dev, "Endpoint link capability: 0x%x\n", link_up_spd);
	pcie_capability_read_word(dn_dev, PCI_EXP_LNKCAP, &link_dn_spd);
	link_dn_spd &= PCI_EXP_LNKCAP_SLS;
	pcie_info(&pdev->dev, "RC link capability: 0x%x\n", link_dn_spd);

	/* skip if any device across the link is not supporting gen2 speed */
	if (isGen2 &&
		((link_up_spd < PCI_EXP_LNKCAP_SLS_5_0GB) ||
		(link_dn_spd < PCI_EXP_LNKCAP_SLS_5_0GB)))
		goto skip;
	/* skip if any device across the link is not supporting gen1 speed */
	else if (!isGen2 &&
		((link_up_spd < PCI_EXP_LNKCAP_SLS_2_5GB) ||
		(link_dn_spd < PCI_EXP_LNKCAP_SLS_2_5GB)))
		goto skip;

	/* Set Link Speed */
	pcie_capability_read_word(dn_dev, PCI_EXP_LNKCTL2, &val);
	val &= ~PCI_EXP_LNKSTA_CLS;
	if (isGen2)
		val |= PCI_EXP_LNKSTA_CLS_5_0GB;
	else
		val |= PCI_EXP_LNKSTA_CLS_2_5GB;
	pcie_capability_write_word(dn_dev, PCI_EXP_LNKCTL2, val);
	pcie_info(&pdev->dev, "set link speed: 0x%x\n", val);


	/* Retrain the link */
	pcie_capability_read_word(dn_dev, PCI_EXP_LNKCTL, &val);
	val |= PCI_EXP_LNKCTL_RL;
	pcie_capability_write_word(dn_dev, PCI_EXP_LNKCTL, val);
	pcie_info(&pdev->dev, "retrain link: 0x%x done\n", val);

	count = 30;
	val = 0;
	while (count--) {
		mdelay(1);
		pcie_capability_read_word(dn_dev, PCI_EXP_LNKSTA, &link_dn_spd);
		if (val != (link_dn_spd))
			pr_info("[PCIE] read link status %08x\n", link_dn_spd);
		val = link_dn_spd;
	}

	return true;
skip:
	return false;
}


bool mstar_pcie_link_speed(bool isGen2)
{
	struct pci_dev *pdev = NULL;
	bool ret = false;

	for_each_pci_dev(pdev) {
		if (mstar_pcie_change_link_speed(pdev, isGen2))
			ret = true;
	}

	return ret;
}

static void mstar_pcie_enable_features(void)
{
	if (!mstar_pcie_link_speed(true))
		pr_info("[PCIE]: Link speed no change...\n");
}
#endif


#ifdef PCIE_DUMP_REGISTERS
#define REG_MAC_K_PARA_SIZE 0x60
#define REG_MAC_TOP_SIZE	0x80
#define REG_MAC_TEST_SIZE	0x04
#define REG_RC_TOP_SIZE		0x80
static	u32 reg_mac_k[REG_MAC_K_PARA_SIZE];
static	u32 reg_mac_top[REG_MAC_TOP_SIZE];
static	u32 reg_mac_test[REG_MAC_TEST_SIZE];
static	u32 reg_rc_top[REG_RC_TOP_SIZE];

static void mstar_pcie_dump_registers(struct mstar_pcie_port *port)
{
	u32 i;


	/* init arrary */
	memset((void *)reg_mac_k, 0, sizeof(u32) * ARRAY_SIZE(reg_mac_k));
	memset((void *)reg_mac_top, 0, sizeof(u32) * ARRAY_SIZE(reg_mac_top));
	memset((void *)reg_mac_test, 0, sizeof(u32) * ARRAY_SIZE(reg_mac_test));
	memset((void *)reg_rc_top, 0, sizeof(u32) * ARRAY_SIZE(reg_rc_top));

	/* read MAC k para register */
	for (i = 0; i < REG_MAC_K_PARA_SIZE; i++)
		reg_mac_k[i] = pcie_read(port, i*4);

	/* read MAC TOP register */
	for (i = 0; i < REG_MAC_TOP_SIZE; i++)
		reg_mac_top[i] = pcie_read(port, 0x400+i*4);

	/* read MAC test register */
	for (i = 0; i < REG_MAC_TEST_SIZE; i++)
		reg_mac_test[i] = pcie_read(port, 0x800+i*4);


	/* read RC TOP register */
	for (i = 0; i < REG_RC_TOP_SIZE; i++)
		reg_rc_top[i] = pcie_riu_readw(port, i);

	/* dump registers */

	pr_info("[PCIE] dump MAC k para registers:\n");
	for (i = 0; i < REG_MAC_K_PARA_SIZE; i += 4) {
		/* offset */
		pr_info("%08X:", i*4);
		/* show value */
		pr_info("%08X %08X %08X %08X\n", reg_mac_k[i], reg_mac_k[i+1], reg_mac_k[i+2], reg_mac_k[i+3]);
	}

	pr_info("[PCIE] dump MAC top registers:\n");
	for (i = 0; i < REG_MAC_TOP_SIZE; i += 4) {
		/* offset */
		pr_info("%08X:", 0x400+i*4);
		/* show value */
		pr_info("%08X %08X %08X %08X\n", reg_mac_top[i], reg_mac_top[i+1], reg_mac_top[i+2], reg_mac_top[i+3]);
	}

	pr_info("[PCIE] dump MAC test registers:\n");
	for (i = 0; i < REG_MAC_TEST_SIZE; i += 4) {
		/* offset */
		pr_info("%08X:", 0x800+i*4);
		/* show value */
		pr_info("%08X %08X %08X %08X\n", reg_mac_test[i], reg_mac_test[i+1], reg_mac_test[i+2], reg_mac_test[i+3]);
	}


	pr_info("[PCIE] dump RC top registers:\n");
	for (i = 0; i < REG_RC_TOP_SIZE; i += 4) {
		/* offset*/
		pr_info("%08X:", i*2);
		/* show value */
		pr_info("%04X %04X %04X %04X\n", reg_rc_top[i], reg_rc_top[i+1], reg_rc_top[i+2], reg_rc_top[i+3]);
	}
}
#endif


/* PCIe driver does not allow module unload */
static int __init pcie_init(void)
{
	int 	ret;
#ifdef PCIE_TEST_MMIO_REGISTERS
	u32 	val;
#endif

	pr_info("[PCIE] driver init start...\n");

#ifdef FPGA_PCIE_PHY_IIC
	PCIE_IIC_INIT();
#endif

	mstar_pcie_hw.nr_controllers = 0;
	mstar_pcie_hw.private_data = mstar_hw_private;

	g_pcie_ports[0].base = (void *) MSTAR_PCIE_PORT0_BASE;
	g_pcie_ports[0].riu_base = (void *) MSTAR_PCIE_PORT0_RIU0_BASE;
#ifdef ENABLE_PCIE_PORT1
	g_pcie_ports[1].base = (void *) MSTAR_PCIE_PORT1_BASE;
	g_pcie_ports[1].riu_base = (void *) MSTAR_PCIE_PORT1_RIU1_BASE;
#endif

#ifdef PATCH_WED_XIU_BUS_COLLISION
	g_pcie_ports[0].no_xiu_access = false;				/* enable xiu accesss in interrupt */
#ifdef ENABLE_PCIE_PORT1
	g_pcie_ports[1].no_xiu_access = false;				/* enable xiu accesss in interrupt */
#endif
#endif

	ret = platform_add_devices(Mstar_pcie_platform_devices, ARRAY_SIZE(Mstar_pcie_platform_devices));
	if (ret) {
		pr_info("[PCIE] Fail to add the platform device %d\n", ret);
		return ret;
	}

	ret = platform_driver_register(&mstar_pcie_0_driver);
	if (ret) {
		pr_info("[PCIE] Fail to add pcie-0 platform driver %d\n", ret);
		return ret;
	}

#ifdef ENABLE_PCIE_PORT1
	ret = platform_driver_register(&mstar_pcie_1_driver);
	if (ret) {
		pr_info("[PCIE] Fail to add pcie-1 platform driver %d\n", ret);
		return ret;
	}
#endif

	if (mstar_pcie_hw.nr_controllers > 0) {
#ifdef CONFIG_PCI_MSI
		msi_map_init();
#endif
		pci_common_init(&mstar_pcie_hw);
	}

	pr_info("pcie init done\n");

#ifdef PCIE_ENABLE_5G_LINK
	pr_info("[PCIE] change speed 5G\n");
	/* mstar_pcie_change_speed(&g_pcie_ports[0]); */

	mstar_pcie_enable_features();
#endif

#ifdef PCIE_DUMP_REGISTERS
	mstar_pcie_dump_registers(&g_pcie_ports[0]);
#endif

#ifdef PCIE_TEST_MMIO_REGISTERS
	pr_info("[PCIE] Read Command register\n");
	mstar_pcie_hw_rd_cfg(&g_pcie_ports[0], 0x01, 0, 0x04, 4, &val);
	val |= 0x06;
	pr_info("[PCIE] Command register = %08X\n", val);
	mstar_pcie_hw_wr_cfg(&g_pcie_ports[0], 0x01, 0, 0x04, 4, val);

	pcie_mmio_start_addr = ioremap(0xE0000000, 0x2000000);
	pr_info("pcie_mmio_start_addr: %x\n", (unsigned int) pcie_mmio_start_addr);
	pcie_mmio_read_test();
	iounmap(0xE0000000);					/*unmap iomem*/
#endif

#if 0
	pr_info("[PCIE] Call user function\n");
	char *argv[] = {"/bin/sh", "-c", "/bin/busybox insmod /lib/modules/3.10.40/mt_wifi.ko", NULL};
	static char *envp[] = {"HOME=/", "TERM=linux", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };
	call_usermodehelper(argv[0], argv, envp, UMH_NO_WAIT);

	char *argv1[] = {"/bin/sh", "-c", "/bin/busybox ifconfig ra0 up", NULL};
	call_usermodehelper(argv1[0], argv1, envp, UMH_NO_WAIT);
#endif

	return 0;
}

MODULE_AUTHOR("Mstar PCIe host driver");
MODULE_DESCRIPTION("MStar PCIe host controller driver");
MODULE_LICENSE("GPL v2");

/**
 * Module init and exit
 */
module_init(pcie_init);
