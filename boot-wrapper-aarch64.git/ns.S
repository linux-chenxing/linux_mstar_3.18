/*
 * ns.S - code to initialise everything required when first booting non-secure.
 *
 * Copyright (C) 2013 ARM Limited. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE.txt file.
 */

#define PL011_UARTIBRD	0x24
#define PL011_UARTCR	0x30

#define V2M_SYS_CFGDATA	0xa0
#define V2M_SYS_CFGCTRL	0xa4

	.text
	.globl ns_init_system

ns_init_system:
	/*
	 * UART initialisation (38400 8N1)
	 */
/*	 
	ldr	x4, =UART_BASE
	mov	w5, #0x10
	str	w5, [x4, #PL011_UARTIBRD]
	mov	w5, #0xc300
	orr	w5, w5, #0x0001	
	str	w5, [x4, #PL011_UARTCR]
*/
	/*
	 * CLCD output site MB
	 */
/*
	ldr	x4, =SYSREGS_BASE
	ldr	w5, =(1 << 31) | (1 << 30) | (7 << 20) | (0 << 16)	// START|WRITE|MUXFPGA|SITE_MB
	str	wzr, [x4, #V2M_SYS_CFGDATA]
	str	w5, [x4, #V2M_SYS_CFGCTRL]
*/
	ret

	.ltorg
	.org 0x40
