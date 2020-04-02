/*
 * spin.S - spin-table boot protocol implementation
 *
 * Copyright (C) 2013 ARM Limited. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE.txt file.
 */

#include "common.S"

	.text

	.globl start_no_el3
	.globl start_el3

start_el3:
	/*
	 * Prepare the switch to the EL2_SP1 mode from EL3
	 */
	ldr	x0, =start_no_el3		// Return after mode switch
	mov	x1, #SPSR_KERNEL
	drop_el	x1, x0

start_no_el3:
	/*
	 * Kernel parameters
	 */
	mov	x0, xzr
	mov	x1, xzr
	mov	x2, xzr
	mov	x3, xzr

	mrs	x4, mpidr_el1
	ldr	x5, =MPIDR_ID_BITS
	tst	x4, x5
	b.eq	2f

	/*
	 * Secondary CPUs
	 */
1:	wfe
	ldr	x4, =mbox
	ldr x4, [x4]
	cbz	x4, 1b
	br	x4				// branch to the given address

2:
	/*
	 * Primary CPU
	 */
	bl	ns_init_system
	ldr	x0, =dtb			// device tree blob
	ldr     x4, =kernel
	br	x4

	.ltorg

	.org	0x80
