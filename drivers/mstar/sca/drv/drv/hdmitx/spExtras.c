//<MStar Software>
//******************************************************************************
// MStar Software
// Copyright (c) 2010 - 2012 MStar Semiconductor, Inc. All rights reserved.
// All software, firmware and related documentation herein ("MStar Software") are
// intellectual property of MStar Semiconductor, Inc. ("MStar") and protected by
// law, including, but not limited to, copyright law and international treaties.
// Any use, modification, reproduction, retransmission, or republication of all 
// or part of MStar Software is expressly prohibited, unless prior written 
// permission has been granted by MStar. 
//
// By accessing, browsing and/or using MStar Software, you acknowledge that you
// have read, understood, and agree, to be bound by below terms ("Terms") and to
// comply with all applicable laws and regulations:
//
// 1. MStar shall retain any and all right, ownership and interest to MStar
//    Software and any modification/derivatives thereof.
//    No right, ownership, or interest to MStar Software and any
//    modification/derivatives thereof is transferred to you under Terms.
//
// 2. You understand that MStar Software might include, incorporate or be
//    supplied together with third party`s software and the use of MStar
//    Software may require additional licenses from third parties.  
//    Therefore, you hereby agree it is your sole responsibility to separately
//    obtain any and all third party right and license necessary for your use of
//    such third party`s software. 
//
// 3. MStar Software and any modification/derivatives thereof shall be deemed as
//    MStar`s confidential information and you agree to keep MStar`s 
//    confidential information in strictest confidence and not disclose to any
//    third party.  
//
// 4. MStar Software is provided on an "AS IS" basis without warranties of any
//    kind. Any warranties are hereby expressly disclaimed by MStar, including
//    without limitation, any warranties of merchantability, non-infringement of
//    intellectual property rights, fitness for a particular purpose, error free
//    and in conformity with any international standard.  You agree to waive any
//    claim against MStar for any loss, damage, cost or expense that you may
//    incur related to your use of MStar Software.
//    In no event shall MStar be liable for any direct, indirect, incidental or
//    consequential damages, including without limitation, lost of profit or
//    revenues, lost or damage of data, and unauthorized system use.
//    You agree that this Section 4 shall still apply without being affected
//    even if MStar Software has been modified by MStar in accordance with your
//    request or instruction for your use, except otherwise agreed by both
//    parties in writing.
//
// 5. If requested, MStar may from time to time provide technical supports or
//    services in relation with MStar Software to you for your use of
//    MStar Software in conjunction with your or your customer`s product
//    ("Services").
//    You understand and agree that, except otherwise agreed by both parties in
//    writing, Services are provided on an "AS IS" basis and the warranty
//    disclaimer set forth in Section 4 above shall apply.  
//
// 6. Nothing contained herein shall be construed as by implication, estoppels
//    or otherwise:
//    (a) conferring any license or right to use MStar name, trademark, service
//        mark, symbol or any other identification;
//    (b) obligating MStar or any of its affiliates to furnish any person,
//        including without limitation, you and your customers, any assistance
//        of any kind whatsoever, or any information; or 
//    (c) conferring any license or right under any intellectual property right.
//
// 7. These terms shall be governed by and construed in accordance with the laws
//    of Taiwan, R.O.C., excluding its conflict of law rules.
//    Any and all dispute arising out hereof or related hereto shall be finally
//    settled by arbitration referred to the Chinese Arbitration Association,
//    Taipei in accordance with the ROC Arbitration Law and the Arbitration
//    Rules of the Association by three (3) arbitrators appointed in accordance
//    with the said Rules.
//    The place of arbitration shall be in Taipei, Taiwan and the language shall
//    be English.  
//    The arbitration award shall be final and binding to both parties.
//
//******************************************************************************
//<MStar Software>
/* spExtras.c */

/******************** SHORT COPYRIGHT NOTICE**************************
This source code is part of the BigDigits multiple-precision
arithmetic library Version 2.2 originally written by David Ireland,
copyright (c) 2001-8 D.I. Management Services Pty Limited, all rights
reserved. It is provided "as is" with no warranties. You may use
this software under the terms of the full copyright notice
"bigdigitsCopyright.txt" that should have been included with this
library or can be obtained from <www.di-mgt.com.au/bigdigits.html>.
This notice must always be retained in any copy.
******************* END OF COPYRIGHT NOTICE***************************/
/*
	Last updated:
	$Date: 2008-07-31 12:54:00 $
	$Revision: 2.2.0 $
	$Author: dai $
*/


//#include <assert.h>
#include "bigdigits.h"
#include "spExtras.h"

int spModMult(DIGIT_T *a, DIGIT_T x, DIGIT_T y, DIGIT_T m)
{	/*	Computes a = (x * y) mod m */
	
	/* Calc p[2] = x * y */
	DIGIT_T p[2];
	spMultiply(p, x, y);

	/* Then modulo */
	*a = mpShortMod(p, m, 2);
	return 0;
}

DIGIT_T spGcd(DIGIT_T x, DIGIT_T y)
{	/*	Returns gcd(x, y) */

	/* Ref: Schneier 2nd ed, p245 */
	
	DIGIT_T g;

	if (x + y == 0)
		return 0;	/* Error */

	g = y;
	while (x > 0)
	{
		g = x;
		x = y % x;
		y = g;
	}
	return g;
}

/* spIsPrime */

static DIGIT_T SMALL_PRIMES[] = { 2, 3, 5, 7, 11, 13, 17, 19 };
#define N_SMALL_PRIMES sizeof(SMALL_PRIMES)/sizeof(DIGIT_T)

int spIsPrime(DIGIT_T w, size_t t)
{	/*	Returns true if w is a probable prime 
		Carries out t iterations
		(Use t = 50 for DSS Standard) 
	*/
	/*	Uses Rabin-Miller Probabilistic Primality Test,
		Ref: FIPS-186-2 Appendix 2.
		Also Schneier 2nd ed p 260 & Knuth Vol 2, p 379
		and ANSI 9.42-2003 Annex B.1.1.
	*/

	unsigned int i, j;
	DIGIT_T m, a, b, z;
	int failed;

	/*	First check for small primes */
	for (i = 0; i < N_SMALL_PRIMES; i++)
	{
		if (w % SMALL_PRIMES[i] == 0)
			return 0;	/* Failed */
	}

	/*	Now do Rabin-Miller  */
	/*	Step 2. Find a and m where w = 1 + (2^a)m
		m is odd and 2^a is largest power of 2 dividing w - 1 */

	m = w - 1;
	for (a = 0; ISEVEN(m); a++)
		m >>= 1;	/* Divide by 2 until m is odd */

	/*
	assert((1 << a) * m + 1 == w);
	*/

	for (i = 0; i < t; i++)
	{
		failed = 1;	/* Assume fail unless passed in loop */
		/* Step 3. Generate a random integer 1 < b < w */
		/* [v2.1] changed to 1 < b < w-1 */
		b = spSimpleRand(2, w - 2);

		/*
		assert(1 < b && b < w-1);
		*/

		/* Step 4. Set j = 0 and z = b^m mod w */
		j = 0;
		spModExp(&z, b, m, w);
		do
		{
			/* Step 5. If j = 0 and z = 1, or if z = w - 1 */
			if ((j == 0 && z == 1) || (z == w - 1))
			{	/* Passes on this loop  - go to Step 9 */
				failed = 0;
				break;
			}

			/* Step 6. If j > 0 and z = 1 */
			if (j > 0 && z == 1)
			{	/* Fails - go to Step 8 */
				failed = 1;
				break;
			}

			/* Step 7. j = j + 1. If j < a set z = z^2 mod w */
			j++;
			if (j < a)
				spModMult(&z, z, z, w);
			/* Loop: if j < a go to Step 5 */
		} while (j < a);

		if (failed)
		{	/* Step 8. Not a prime - stop */
			return 0;
		}
	}	/* Step 9. Go to Step 3 until i >= n */
	/* If got here, probably prime => success */
	return 1;
}

/* spModExp */

/* Two alternative functions for spModExp */

int spModExpK(DIGIT_T *exp, DIGIT_T x, 
			DIGIT_T n, DIGIT_T d)
{	/*	Computes exp = x^n mod d */
	/*	Ref: Knuth Vol 2 Ch 4.6.3 p 462 Algorithm A
	*/
	DIGIT_T y = 1;		/* Step A1. Initialise */

	while (n > 0)
	{							/* Step A2. Halve N */
		if (n & 0x1)			/* If odd */
			spModMult(&y, y, x, d);	/*   Step A3. Multiply Y by Z */	
		
		n >>= 1;					/* Halve N */
		if (n > 0)				/* Step A4. N = 0? Y is answer */
			spModMult(&x, x, x, d);	/* Step A5. Square Z */
	}

	*exp = y;
	return 0;
}

int spModExpB(DIGIT_T *exp, DIGIT_T x, 
			DIGIT_T e, DIGIT_T m)
{	/*	Computes exp = x^e mod m */
	/*	Binary left-to-right method
	*/
	DIGIT_T mask;
	DIGIT_T y;	/* Temp variable */

	/* Find most significant bit in e */
	for (mask = HIBITMASK; mask > 0; mask >>= 1)
	{
		if (e & mask)
			break;
	}

	y = x;
	/* For j = k-2 downto 0 step -1 */
	for (mask >>= 1; mask > 0; mask >>= 1)
	{
		spModMult(&y, y, y, m);		/* Square */
		if (e & mask)
			spModMult(&y, y, x, m);	/* Multiply */
	}

	*exp = y;
	return 0;
}

int spModInv(DIGIT_T *inv, DIGIT_T u, DIGIT_T v)
{	/*	Computes inv = u^(-1) mod v */
	/*	Ref: Knuth Algorithm X Vol 2 p 342 
		ignoring u2, v2, t2
		and avoiding negative numbers
	*/
	DIGIT_T u1, u3, v1, v3, t1, t3, q, w;
	int bIterations = 1;
	int result;
	
	/* Step X1. Initialise */
	u1 = 1;
	u3 = u;
	v1 = 0;
	v3 = v;

	while (v3 != 0)	/* Step X2. */
	{	/* Step X3. */
		q = u3 / v3;	/* Divide and */
		t3 = u3 % v3;
		w = q * v1;		/* "Subtract" */
		t1 = u1 + w;
		/* Swap */
		u1 = v1;
		v1 = t1;
		u3 = v3;
		v3 = t3;
		bIterations = -bIterations;
	}

	if (bIterations < 0)
		*inv = v - u1;
	else
		*inv = u1;

	/* Make sure u3 = gcd(u,v) == 1 */
	if (u3 != 1)
	{
		result = 1;
		*inv = 0;
	}
	else
		result = 0;

	return result;
}

