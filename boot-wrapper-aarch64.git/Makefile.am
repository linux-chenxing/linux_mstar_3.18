#
# Makefile.am - build a kernel+filesystem image for stand-alone Linux
# booting
#
# Copyright (C) 2012 ARM Limited. All rights reserved.
#
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE.txt file.

# VE
if KERN
PHYS_OFFSET	:= $(shell $(top_srcdir)/findmem.pl $(KERNEL_DTB))
endif
UART_BASE	:= $(shell $(top_srcdir)/findbase.pl $(KERNEL_DTB) 0 'arm,pl011')
SYSREGS_BASE	:= $(shell $(top_srcdir)/findbase.pl $(KERNEL_DTB) 0 'arm,vexpress-sysreg')
CNTFRQ		:= 0x01800000	# 24Mhz

DEFINES		= -DCNTFRQ=$(CNTFRQ)
DEFINES		+= -DCPU_IDS=$(CPU_IDS)
DEFINES		+= -DSYSREGS_BASE=$(SYSREGS_BASE)
DEFINES		+= -DUART_BASE=$(UART_BASE)

BOOTLOADER	:= boot.S

if PSCI
BOOTMETHOD	:= psci.o
PSCI_NODE	:= psci {				\
			compatible = \"arm,psci\";	\
			method = \"smc\";		\
			cpu_on = <0x84000002>;		\
			cpu_off = <0x84000001>;		\
		   };
CPU_NODES	:= $(shell $(top_srcdir)/gen-cpu-nodes.sh $(CPU_IDS))
CPUS_NODE	:= cpus {		\
			$(CPU_NODES)	\
		   };
else
BOOTMETHOD	:= spin.o
PSCI_NODE	:=
CPUS_NODE	:=
endif

if GICV3
GIC_DIST_BASE	:= $(shell $(top_srcdir)/findbase.pl $(KERNEL_DTB) 0 'arm,gic-v3')
GIC_RDIST_BASE	:= $(shell $(top_srcdir)/findbase.pl $(KERNEL_DTB) 1 'arm,gic-v3')
DEFINES		+= -DGIC_DIST_BASE=$(GIC_DIST_BASE)
DEFINES		+= -DGIC_RDIST_BASE=$(GIC_RDIST_BASE)
GIC		:= gic-v3.o
else
GIC_DIST_BASE	:= $(shell $(top_srcdir)/findbase.pl $(KERNEL_DTB) 0 'arm,cortex-a15-gic')
GIC_CPU_BASE	:= $(shell $(top_srcdir)/findbase.pl $(KERNEL_DTB) 1 'arm,cortex-a15-gic')
DEFINES		+= -DGIC_CPU_BASE=$(GIC_CPU_BASE)
DEFINES		+= -DGIC_DIST_BASE=$(GIC_DIST_BASE)
GIC		:= gic.o
endif

MBOX_OFFSET	:= 0xfff8
KERNEL_OFFSET	:= 0x80000
LD_SCRIPT	:= model.lds.S
IMAGE		:= linux-system.axf
BIN		:= boot.bin

FS_OFFSET	:= 0x10000000
FILESYSTEM_START:= $(shell echo $$(($(PHYS_OFFSET) + $(FS_OFFSET))))
FILESYSTEM_SIZE	:= $(shell stat -Lc %s $(FILESYSTEM) 2>/dev/null || echo 0)
FILESYSTEM_END	:= $(shell echo $$(($(FILESYSTEM_START) + $(FILESYSTEM_SIZE))))

FDT_OFFSET	:= 0x08000000

if INITRD
INITRD_FLAGS	:= -DUSE_INITRD
CHOSEN_NODE	:= chosen {						\
			bootargs = \"$(CMDLINE)\";			\
			linux,initrd-start = <$(FILESYSTEM_START)>;	\
			linux,initrd-end = <$(FILESYSTEM_END)>;		\
		   };
else
INITRD_FLAGS	:=
CHOSEN_NODE	:= chosen {						\
			bootargs = \"$(CMDLINE)\";			\
		   };
endif


if KERN
KERN_FLAGS := -DUSE_KERN
else
PHYS_OFFSET	:= 0x0
DEFINES		+= -Dkernel=0x20080000
DEFINES		+= -Dmbox=0x2000fff8
endif

if DTB
DTB_FLAGS := -DUSE_DTB
else
DEFINES		+= -Ddtb=0x28000000
endif

CPPFLAGS	+= $(INITRD_FLAGS)
CPPFLAGS	+= $(KERN_FLAGS)
CPPFLAGS	+= $(DTB_FLAGS)


all: $(BIN)

CLEANFILES = $(IMAGE) boot.o cache.o $(GIC) mmu.o ns.o $(BOOTMETHOD) model.lds fdt.dtb

%.bin: $(IMAGE)
	aarch64-linux-gnu-objcopy -O binary $< $@

$(IMAGE): boot.o cache.o $(GIC) mmu.o ns.o $(BOOTMETHOD) model.lds fdt.dtb $(KERNEL_IMAGE) $(FILESYSTEM)
	$(LD) -o $@ --script=model.lds

%.o: %.S Makefile
	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEFINES) -c -o $@ $<

model.lds: $(LD_SCRIPT) Makefile
	$(CPP) $(CPPFLAGS) -ansi -DPHYS_OFFSET=$(PHYS_OFFSET) -DMBOX_OFFSET=$(MBOX_OFFSET) -DKERNEL_OFFSET=$(KERNEL_OFFSET) -DFDT_OFFSET=$(FDT_OFFSET) -DFS_OFFSET=$(FS_OFFSET) -DKERNEL=$(KERNEL_IMAGE) -DFILESYSTEM=$(FILESYSTEM) -DBOOTMETHOD=$(BOOTMETHOD) -DGIC=$(GIC) -P -C -o $@ $<

fdt.dtb: $(KERNEL_DTB) Makefile gen-cpu-nodes.sh
	( $(DTC) -O dts -I dtb $(KERNEL_DTB) ; echo "/ { $(CHOSEN_NODE) $(PSCI_NODE) $(CPUS_NODE) };" ) | $(DTC) -O dtb -o $@ -

# The filesystem archive might not exist if INITRD is not being used
.PHONY: all clean $(FILESYSTEM)

MAINTAINERCLEANFILES = aclocal.m4 compile config.* configure install-sh \
	Makefile.in missing
