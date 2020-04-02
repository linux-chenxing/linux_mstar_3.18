# configure.ac - autoconf script for the AArch64 bootwrapper
#
# Copyright (c) 2014 ARM Limited. All rights reserved.
#
# Use of this source code is governed by a BSD-style license that can
# be found in the LICENSE.txt file.

AC_INIT([aarch64-boot-wrapper], [v0.1])

# Ensure that we're using an AArch64 compiler
AC_CANONICAL_SYSTEM

if test "x$host_cpu" != "xaarch64"; then
	AC_MSG_ERROR([The boot-wrapper can only be used with an AArch64 compiler.])
fi

AM_INIT_AUTOMAKE([foreign])

# Allow a user to pass --with-kernel-dir
AC_ARG_WITH([kernel-dir],
	AS_HELP_STRING([--with-kernel-dir], [specify the root Linux kernel build directory (modified)]),
	[KERN_DIR="$withval"])
AM_CONDITIONAL([KERN], [test "x$KERN_DIR" != "x"])

if test "x$KERN_DIR" != "x"; then
KERN_IMAGE=$KERN_DIR/arch/arm64/boot/Image
m4_define([CHECKFILES1], [KERN_DIR,
	KERN_IMAGE])
	
m4_foreach([checkfile1], [CHECKFILES1],
	[AC_CHECK_FILE([$checkfile1], [], AC_MSG_ERROR([No such file or directory: $checkfile1]))])
	
AC_SUBST([KERNEL_IMAGE], [$KERN_IMAGE])	
fi

# Allow the user to override the default DTB
AC_ARG_WITH([dtb],
	AS_HELP_STRING([--with-dtb], [Specify a particular DTB to use]),
	[KERN_DTB="$withval"])
AM_CONDITIONAL([DTB], [test "x$KERN_DTB" != "x"])	
if test "x$KERN_DTB" = "x"; then
KERN_DTB=./esl_v8.dtb
fi

# Ensure that the user has provided us with a sane kernel dir.
m4_define([CHECKFILES], [KERN_DTB])

m4_foreach([checkfile], [CHECKFILES],
	[AC_CHECK_FILE([$checkfile], [], AC_MSG_ERROR([No such file or directory: $checkfile]))])

AC_SUBST([KERNEL_DTB], [$KERN_DTB])

# Allow a user to pass --enable-psci
USE_PSCI=no
AC_ARG_ENABLE([psci],
	AS_HELP_STRING([--enable-psci], [enable the psci boot method]),
	[USE_PSCI=yes],
	[USE_PSCI=no])
AM_CONDITIONAL([PSCI], [test "x$USE_PSCI" = "xyes"])

# Allow a user to pass --with-cpu-ids
C_CPU_IDS="0x0,0x1,0x2,0x3,0x100,0x101,0x102,0x103"
AC_ARG_WITH(cpu-ids,
	AS_HELP_STRING([--with-cpu-ids], [Specify a comma seperated list of CPU IDs]),
	[C_CPU_IDS="$withval"])
AC_SUBST([CPU_IDS], [$C_CPU_IDS])

# Allow a user to pass --with-initrd
AC_ARG_WITH([initrd],
	AS_HELP_STRING([--with-initrd], [embed an initrd in the kernel image]),
	USE_INITRD=$withval)
AC_SUBST([FILESYSTEM], [$USE_INITRD])
AM_CONDITIONAL([INITRD], [test "x$USE_INITRD" != "x"])

#C_CMDLINE="console=ttySY0 mem=1024M selinux=0 maxcpus=4 devtmpfs.mount=1 earlyprintk=synopsys,0x1f001000 debug"
C_CMDLINE="console=ttySY0 selinux=0 maxcpus=4 earlycon=mstar,0x1f220d00 debug"
AC_ARG_WITH([cmdline],
	AS_HELP_STRING([--with-cmdline], [set a command line for the kernel]),
	[C_CMDLINE=$withval])
AC_SUBST([CMDLINE], [$C_CMDLINE])

# Allow a user to pass --enable-gicv3
USE_GICV3=no
AC_ARG_ENABLE([gicv3],
	AS_HELP_STRING([--enable-gicv3], [enable GICv3 instead of GICv2]),
	[USE_GICV3=yes],
	[USE_GICV3=no])
AM_CONDITIONAL([GICV3], [test "x$USE_GICV3" = "xyes"])

# Ensure that we have all the needed programs
AC_PROG_CC
AC_PROG_CPP
AM_PROG_AS
AC_PROG_SED
AC_PROG_LN_S
AC_PATH_PROG([DTC], dtc, error, [$PATH$PATH_SEPARATOR$KERN_DIR/scripts/dtc, ../scripts/dtc/])
if test "x$DTC" = "xerror"; then
	AC_MSG_ERROR([cannot find the device tree compiler (dtc)])
fi
AC_CHECK_TOOL(LD, ld)

AC_CONFIG_FILES([Makefile])

AC_OUTPUT

# Print the final config to the user.
echo ""
echo "  Boot wrapper configuration"
echo "  =========================="
echo ""
echo "  Linux kernel build dir:            ${KERN_DIR}"
echo "  Device tree blob:                  ${KERN_DTB}"
echo "  Linux kernel command line:         ${CMDLINE}"
echo "  Embedded initrd:                   ${FILESYSTEM:-NONE}"
echo "  Use PSCI?                          ${USE_PSCI}"
echo "  CPU IDs:                           ${CPU_IDS}"
echo "  Use GICv3?                         ${USE_GICV3}"
echo ""
