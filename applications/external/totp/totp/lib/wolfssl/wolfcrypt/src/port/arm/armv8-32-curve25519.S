/* armv8-32-curve25519
 *
 * Copyright (C) 2006-2023 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

/* Generated using (from wolfssl):
 *   cd ../scripts
 *   ruby ./x25519/x25519.rb arm32 ../wolfssl/wolfcrypt/src/port/arm/armv8-32-curve25519.S
 */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif /* HAVE_CONFIG_H */
#include <wolfssl/wolfcrypt/settings.h>

#ifdef WOLFSSL_ARMASM
#ifndef __aarch64__
#ifdef HAVE_CURVE25519

	.text
	.align	4
	.globl	fe_init
	.type	fe_init, %function
fe_init:
	bx	lr
	.size	fe_init,.-fe_init
	.text
	.align	4
	.globl	fe_frombytes
	.type	fe_frombytes, %function
fe_frombytes:
	push	{r4, r5, r6, r7, r8, r9, lr}
	ldr	r2, [r1]
	ldr	r3, [r1, #4]
	ldr	r4, [r1, #8]
	ldr	r5, [r1, #12]
	ldr	r6, [r1, #16]
	ldr	r7, [r1, #20]
	ldr	r8, [r1, #24]
	ldr	r9, [r1, #28]
	and	r9, r9, #0x7fffffff
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r2, [r0]
	str	r3, [r0, #4]
#else
	strd	r2, r3, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #8]
	str	r5, [r0, #12]
#else
	strd	r4, r5, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #16]
	str	r7, [r0, #20]
#else
	strd	r6, r7, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #24]
	str	r9, [r0, #28]
#else
	strd	r8, r9, [r0, #24]
#endif
	pop	{r4, r5, r6, r7, r8, r9, pc}
	.size	fe_frombytes,.-fe_frombytes
	.text
	.align	4
	.globl	fe_tobytes
	.type	fe_tobytes, %function
fe_tobytes:
	push	{r4, r5, r6, r7, r8, r9, lr}
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r2, [r1]
	ldr	r3, [r1, #4]
#else
	ldrd	r2, r3, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #8]
	ldr	r5, [r1, #12]
#else
	ldrd	r4, r5, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #16]
	ldr	r7, [r1, #20]
#else
	ldrd	r6, r7, [r1, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r1, #24]
	ldr	r9, [r1, #28]
#else
	ldrd	r8, r9, [r1, #24]
#endif
	adds	r12, r2, #19
	adcs	r12, r3, #0
	adcs	r12, r4, #0
	adcs	r12, r5, #0
	adcs	r12, r6, #0
	adcs	r12, r7, #0
	adcs	r12, r8, #0
	adc	r12, r9, #0
	asr	r12, r12, #31
	and	r12, r12, #19
	adds	r2, r2, r12
	adcs	r3, r3, #0
	adcs	r4, r4, #0
	adcs	r5, r5, #0
	adcs	r6, r6, #0
	adcs	r7, r7, #0
	adcs	r8, r8, #0
	adc	r9, r9, #0
	and	r9, r9, #0x7fffffff
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r2, [r0]
	str	r3, [r0, #4]
#else
	strd	r2, r3, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #8]
	str	r5, [r0, #12]
#else
	strd	r4, r5, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #16]
	str	r7, [r0, #20]
#else
	strd	r6, r7, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #24]
	str	r9, [r0, #28]
#else
	strd	r8, r9, [r0, #24]
#endif
	pop	{r4, r5, r6, r7, r8, r9, pc}
	.size	fe_tobytes,.-fe_tobytes
	.text
	.align	4
	.globl	fe_1
	.type	fe_1, %function
fe_1:
	# Set one
	mov	r2, #1
	mov	r3, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r2, [r0]
	str	r3, [r0, #4]
#else
	strd	r2, r3, [r0]
#endif
	mov	r2, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r2, [r0, #8]
	str	r3, [r0, #12]
#else
	strd	r2, r3, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r2, [r0, #16]
	str	r3, [r0, #20]
#else
	strd	r2, r3, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r2, [r0, #24]
	str	r3, [r0, #28]
#else
	strd	r2, r3, [r0, #24]
#endif
	bx	lr
	.size	fe_1,.-fe_1
	.text
	.align	4
	.globl	fe_0
	.type	fe_0, %function
fe_0:
	# Set zero
	mov	r2, #0
	mov	r3, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r2, [r0]
	str	r3, [r0, #4]
#else
	strd	r2, r3, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r2, [r0, #8]
	str	r3, [r0, #12]
#else
	strd	r2, r3, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r2, [r0, #16]
	str	r3, [r0, #20]
#else
	strd	r2, r3, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r2, [r0, #24]
	str	r3, [r0, #28]
#else
	strd	r2, r3, [r0, #24]
#endif
	bx	lr
	.size	fe_0,.-fe_0
	.text
	.align	4
	.globl	fe_copy
	.type	fe_copy, %function
fe_copy:
	push	{r4, r5, lr}
	# Copy
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r2, [r1]
	ldr	r3, [r1, #4]
#else
	ldrd	r2, r3, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #8]
	ldr	r5, [r1, #12]
#else
	ldrd	r4, r5, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r2, [r0]
	str	r3, [r0, #4]
#else
	strd	r2, r3, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #8]
	str	r5, [r0, #12]
#else
	strd	r4, r5, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r2, [r1, #16]
	ldr	r3, [r1, #20]
#else
	ldrd	r2, r3, [r1, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #24]
	ldr	r5, [r1, #28]
#else
	ldrd	r4, r5, [r1, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r2, [r0, #16]
	str	r3, [r0, #20]
#else
	strd	r2, r3, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #24]
	str	r5, [r0, #28]
#else
	strd	r4, r5, [r0, #24]
#endif
	pop	{r4, r5, pc}
	.size	fe_copy,.-fe_copy
	.text
	.align	4
	.globl	fe_sub
	.type	fe_sub, %function
fe_sub:
	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
	# Sub
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1]
	ldr	r5, [r1, #4]
#else
	ldrd	r4, r5, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #8]
	ldr	r7, [r1, #12]
#else
	ldrd	r6, r7, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r2]
	ldr	r9, [r2, #4]
#else
	ldrd	r8, r9, [r2]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r2, #8]
	ldr	r11, [r2, #12]
#else
	ldrd	r10, r11, [r2, #8]
#endif
	subs	r8, r4, r8
	sbcs	r9, r5, r9
	sbcs	r10, r6, r10
	sbcs	r11, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0]
	str	r9, [r0, #4]
#else
	strd	r8, r9, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #8]
	str	r11, [r0, #12]
#else
	strd	r10, r11, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #16]
	ldr	r5, [r1, #20]
#else
	ldrd	r4, r5, [r1, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #24]
	ldr	r7, [r1, #28]
#else
	ldrd	r6, r7, [r1, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r2, #16]
	ldr	r9, [r2, #20]
#else
	ldrd	r8, r9, [r2, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r2, #24]
	ldr	r11, [r2, #28]
#else
	ldrd	r10, r11, [r2, #24]
#endif
	sbcs	r8, r4, r8
	sbcs	r9, r5, r9
	sbcs	r10, r6, r10
	sbc	r11, r7, r11
	mov	r12, #-19
	asr	r3, r11, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Add modulus (if underflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
#else
	ldrd	r6, r7, [r0, #8]
#endif
	adds	r4, r4, r12
	adcs	r5, r5, r3
	adcs	r6, r6, r3
	adcs	r7, r7, r3
	adcs	r8, r8, r3
	adcs	r9, r9, r3
	adcs	r10, r10, r3
	adc	r11, r11, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #8]
	str	r7, [r0, #12]
#else
	strd	r6, r7, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #24]
	str	r11, [r0, #28]
#else
	strd	r10, r11, [r0, #24]
#endif
	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
	.size	fe_sub,.-fe_sub
	.text
	.align	4
	.globl	fe_add
	.type	fe_add, %function
fe_add:
	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
	# Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1]
	ldr	r5, [r1, #4]
#else
	ldrd	r4, r5, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #8]
	ldr	r7, [r1, #12]
#else
	ldrd	r6, r7, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r2]
	ldr	r9, [r2, #4]
#else
	ldrd	r8, r9, [r2]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r2, #8]
	ldr	r11, [r2, #12]
#else
	ldrd	r10, r11, [r2, #8]
#endif
	adds	r8, r4, r8
	adcs	r9, r5, r9
	adcs	r10, r6, r10
	adcs	r11, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0]
	str	r9, [r0, #4]
#else
	strd	r8, r9, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #8]
	str	r11, [r0, #12]
#else
	strd	r10, r11, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #16]
	ldr	r5, [r1, #20]
#else
	ldrd	r4, r5, [r1, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #24]
	ldr	r7, [r1, #28]
#else
	ldrd	r6, r7, [r1, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r2, #16]
	ldr	r9, [r2, #20]
#else
	ldrd	r8, r9, [r2, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r2, #24]
	ldr	r11, [r2, #28]
#else
	ldrd	r10, r11, [r2, #24]
#endif
	adcs	r8, r4, r8
	adcs	r9, r5, r9
	adcs	r10, r6, r10
	adc	r11, r7, r11
	mov	r12, #-19
	asr	r3, r11, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Sub modulus (if overflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
#else
	ldrd	r6, r7, [r0, #8]
#endif
	subs	r4, r4, r12
	sbcs	r5, r5, r3
	sbcs	r6, r6, r3
	sbcs	r7, r7, r3
	sbcs	r8, r8, r3
	sbcs	r9, r9, r3
	sbcs	r10, r10, r3
	sbc	r11, r11, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #8]
	str	r7, [r0, #12]
#else
	strd	r6, r7, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #24]
	str	r11, [r0, #28]
#else
	strd	r10, r11, [r0, #24]
#endif
	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
	.size	fe_add,.-fe_add
	.text
	.align	4
	.globl	fe_neg
	.type	fe_neg, %function
fe_neg:
	push	{r4, r5, lr}
	mov	lr, #-1
	mov	r12, #-19
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r2, [r1]
	ldr	r3, [r1, #4]
#else
	ldrd	r2, r3, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #8]
	ldr	r5, [r1, #12]
#else
	ldrd	r4, r5, [r1, #8]
#endif
	subs	r2, r12, r2
	sbcs	r3, lr, r3
	sbcs	r4, lr, r4
	sbcs	r5, lr, r5
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r2, [r0]
	str	r3, [r0, #4]
#else
	strd	r2, r3, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #8]
	str	r5, [r0, #12]
#else
	strd	r4, r5, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r12, #0x7fffff
	lsl	r12, r12, #8
	add	r12, r12, #0xff
#else
	mov	r12, #0x7fffffff
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r2, [r1, #16]
	ldr	r3, [r1, #20]
#else
	ldrd	r2, r3, [r1, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #24]
	ldr	r5, [r1, #28]
#else
	ldrd	r4, r5, [r1, #24]
#endif
	sbcs	r2, lr, r2
	sbcs	r3, lr, r3
	sbcs	r4, lr, r4
	sbc	r5, r12, r5
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r2, [r0, #16]
	str	r3, [r0, #20]
#else
	strd	r2, r3, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #24]
	str	r5, [r0, #28]
#else
	strd	r4, r5, [r0, #24]
#endif
	pop	{r4, r5, pc}
	.size	fe_neg,.-fe_neg
	.text
	.align	4
	.globl	fe_isnonzero
	.type	fe_isnonzero, %function
fe_isnonzero:
	push	{r4, r5, r6, r7, r8, r9, lr}
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r2, [r0]
	ldr	r3, [r0, #4]
#else
	ldrd	r2, r3, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #8]
	ldr	r5, [r0, #12]
#else
	ldrd	r4, r5, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #16]
	ldr	r7, [r0, #20]
#else
	ldrd	r6, r7, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r0, #24]
	ldr	r9, [r0, #28]
#else
	ldrd	r8, r9, [r0, #24]
#endif
	adds	r1, r2, #19
	adcs	r1, r3, #0
	adcs	r1, r4, #0
	adcs	r1, r5, #0
	adcs	r1, r6, #0
	adcs	r1, r7, #0
	adcs	r1, r8, #0
	adc	r1, r9, #0
	asr	r1, r1, #31
	and	r1, r1, #19
	adds	r2, r2, r1
	adcs	r3, r3, #0
	adcs	r4, r4, #0
	adcs	r5, r5, #0
	adcs	r6, r6, #0
	adcs	r7, r7, #0
	adcs	r8, r8, #0
	adc	r9, r9, #0
	and	r9, r9, #0x7fffffff
	orr	r2, r2, r3
	orr	r4, r4, r5
	orr	r6, r6, r7
	orr	r8, r8, r9
	orr	r4, r4, r6
	orr	r2, r2, r8
	orr	r0, r2, r4
	pop	{r4, r5, r6, r7, r8, r9, pc}
	.size	fe_isnonzero,.-fe_isnonzero
	.text
	.align	4
	.globl	fe_isnegative
	.type	fe_isnegative, %function
fe_isnegative:
	push	{r4, r5, lr}
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r2, [r0]
	ldr	r3, [r0, #4]
#else
	ldrd	r2, r3, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #8]
	ldr	r5, [r0, #12]
#else
	ldrd	r4, r5, [r0, #8]
#endif
	adds	r1, r2, #19
	adcs	r1, r3, #0
	adcs	r1, r4, #0
	adcs	r1, r5, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r2, [r0, #16]
	ldr	r3, [r0, #20]
#else
	ldrd	r2, r3, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #24]
	ldr	r5, [r0, #28]
#else
	ldrd	r4, r5, [r0, #24]
#endif
	adcs	r1, r2, #0
	adcs	r1, r3, #0
	adcs	r1, r4, #0
	ldr	r2, [r0]
	adc	r1, r5, #0
	and	r0, r2, #1
	lsr	r1, r1, #31
	eor	r0, r0, r1
	pop	{r4, r5, pc}
	.size	fe_isnegative,.-fe_isnegative
	.text
	.align	4
	.globl	fe_cmov_table
	.type	fe_cmov_table, %function
fe_cmov_table:
	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
	sxtb	r2, r2
	sbfx	r3, r2, #7, #1
	eor	r12, r2, r3
	sub	r12, r12, r3
	mov	r4, #1
	mov	r5, #0
	mov	r6, #1
	mov	r7, #0
	mov	r8, #0
	mov	r9, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #31
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1]
	ldr	r11, [r1, #4]
#else
	ldrd	r10, r11, [r1]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #32]
	ldr	r11, [r1, #36]
#else
	ldrd	r10, r11, [r1, #32]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #64]
	ldr	r11, [r1, #68]
#else
	ldrd	r10, r11, [r1, #64]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	add	r1, r1, #0x60
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #30
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1]
	ldr	r11, [r1, #4]
#else
	ldrd	r10, r11, [r1]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #32]
	ldr	r11, [r1, #36]
#else
	ldrd	r10, r11, [r1, #32]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #64]
	ldr	r11, [r1, #68]
#else
	ldrd	r10, r11, [r1, #64]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	add	r1, r1, #0x60
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #29
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1]
	ldr	r11, [r1, #4]
#else
	ldrd	r10, r11, [r1]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #32]
	ldr	r11, [r1, #36]
#else
	ldrd	r10, r11, [r1, #32]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #64]
	ldr	r11, [r1, #68]
#else
	ldrd	r10, r11, [r1, #64]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	add	r1, r1, #0x60
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #28
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1]
	ldr	r11, [r1, #4]
#else
	ldrd	r10, r11, [r1]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #32]
	ldr	r11, [r1, #36]
#else
	ldrd	r10, r11, [r1, #32]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #64]
	ldr	r11, [r1, #68]
#else
	ldrd	r10, r11, [r1, #64]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	add	r1, r1, #0x60
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #27
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1]
	ldr	r11, [r1, #4]
#else
	ldrd	r10, r11, [r1]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #32]
	ldr	r11, [r1, #36]
#else
	ldrd	r10, r11, [r1, #32]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #64]
	ldr	r11, [r1, #68]
#else
	ldrd	r10, r11, [r1, #64]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	add	r1, r1, #0x60
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #26
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1]
	ldr	r11, [r1, #4]
#else
	ldrd	r10, r11, [r1]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #32]
	ldr	r11, [r1, #36]
#else
	ldrd	r10, r11, [r1, #32]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #64]
	ldr	r11, [r1, #68]
#else
	ldrd	r10, r11, [r1, #64]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	add	r1, r1, #0x60
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #25
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1]
	ldr	r11, [r1, #4]
#else
	ldrd	r10, r11, [r1]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #32]
	ldr	r11, [r1, #36]
#else
	ldrd	r10, r11, [r1, #32]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #64]
	ldr	r11, [r1, #68]
#else
	ldrd	r10, r11, [r1, #64]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	add	r1, r1, #0x60
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #24
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1]
	ldr	r11, [r1, #4]
#else
	ldrd	r10, r11, [r1]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #32]
	ldr	r11, [r1, #36]
#else
	ldrd	r10, r11, [r1, #32]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #64]
	ldr	r11, [r1, #68]
#else
	ldrd	r10, r11, [r1, #64]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	sub	r1, r1, #0x2a0
	mov	r10, #-19
	mov	r11, #-1
	subs	r10, r10, r8
	sbcs	r11, r11, r9
	sbc	lr, lr, lr
	asr	r12, r2, #31
	eor	r3, r4, r6
	and	r3, r3, r12
	eor	r4, r4, r3
	eor	r6, r6, r3
	eor	r3, r5, r7
	and	r3, r3, r12
	eor	r5, r5, r3
	eor	r7, r7, r3
	eor	r10, r10, r8
	and	r10, r10, r12
	eor	r8, r8, r10
	eor	r11, r11, r9
	and	r11, r11, r12
	eor	r9, r9, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #32]
	str	r7, [r0, #36]
#else
	strd	r6, r7, [r0, #32]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #64]
	str	r9, [r0, #68]
#else
	strd	r8, r9, [r0, #64]
#endif
	sbfx	r3, r2, #7, #1
	eor	r12, r2, r3
	sub	r12, r12, r3
	mov	r4, #0
	mov	r5, #0
	mov	r6, #0
	mov	r7, #0
	mov	r8, #0
	mov	r9, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #31
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #8]
	ldr	r11, [r1, #12]
#else
	ldrd	r10, r11, [r1, #8]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #40]
	ldr	r11, [r1, #44]
#else
	ldrd	r10, r11, [r1, #40]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #72]
	ldr	r11, [r1, #76]
#else
	ldrd	r10, r11, [r1, #72]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	add	r1, r1, #0x60
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #30
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #8]
	ldr	r11, [r1, #12]
#else
	ldrd	r10, r11, [r1, #8]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #40]
	ldr	r11, [r1, #44]
#else
	ldrd	r10, r11, [r1, #40]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #72]
	ldr	r11, [r1, #76]
#else
	ldrd	r10, r11, [r1, #72]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	add	r1, r1, #0x60
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #29
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #8]
	ldr	r11, [r1, #12]
#else
	ldrd	r10, r11, [r1, #8]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #40]
	ldr	r11, [r1, #44]
#else
	ldrd	r10, r11, [r1, #40]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #72]
	ldr	r11, [r1, #76]
#else
	ldrd	r10, r11, [r1, #72]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	add	r1, r1, #0x60
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #28
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #8]
	ldr	r11, [r1, #12]
#else
	ldrd	r10, r11, [r1, #8]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #40]
	ldr	r11, [r1, #44]
#else
	ldrd	r10, r11, [r1, #40]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #72]
	ldr	r11, [r1, #76]
#else
	ldrd	r10, r11, [r1, #72]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	add	r1, r1, #0x60
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #27
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #8]
	ldr	r11, [r1, #12]
#else
	ldrd	r10, r11, [r1, #8]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #40]
	ldr	r11, [r1, #44]
#else
	ldrd	r10, r11, [r1, #40]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #72]
	ldr	r11, [r1, #76]
#else
	ldrd	r10, r11, [r1, #72]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	add	r1, r1, #0x60
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #26
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #8]
	ldr	r11, [r1, #12]
#else
	ldrd	r10, r11, [r1, #8]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #40]
	ldr	r11, [r1, #44]
#else
	ldrd	r10, r11, [r1, #40]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #72]
	ldr	r11, [r1, #76]
#else
	ldrd	r10, r11, [r1, #72]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	add	r1, r1, #0x60
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #25
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #8]
	ldr	r11, [r1, #12]
#else
	ldrd	r10, r11, [r1, #8]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #40]
	ldr	r11, [r1, #44]
#else
	ldrd	r10, r11, [r1, #40]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #72]
	ldr	r11, [r1, #76]
#else
	ldrd	r10, r11, [r1, #72]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	add	r1, r1, #0x60
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #24
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #8]
	ldr	r11, [r1, #12]
#else
	ldrd	r10, r11, [r1, #8]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #40]
	ldr	r11, [r1, #44]
#else
	ldrd	r10, r11, [r1, #40]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #72]
	ldr	r11, [r1, #76]
#else
	ldrd	r10, r11, [r1, #72]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	sub	r1, r1, #0x2a0
	mov	r10, #-1
	mov	r11, #-1
	rsbs	lr, lr, #0
	sbcs	r10, r10, r8
	sbcs	r11, r11, r9
	sbc	lr, lr, lr
	asr	r12, r2, #31
	eor	r3, r4, r6
	and	r3, r3, r12
	eor	r4, r4, r3
	eor	r6, r6, r3
	eor	r3, r5, r7
	and	r3, r3, r12
	eor	r5, r5, r3
	eor	r7, r7, r3
	eor	r10, r10, r8
	and	r10, r10, r12
	eor	r8, r8, r10
	eor	r11, r11, r9
	and	r11, r11, r12
	eor	r9, r9, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #8]
	str	r5, [r0, #12]
#else
	strd	r4, r5, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #40]
	str	r7, [r0, #44]
#else
	strd	r6, r7, [r0, #40]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #72]
	str	r9, [r0, #76]
#else
	strd	r8, r9, [r0, #72]
#endif
	sbfx	r3, r2, #7, #1
	eor	r12, r2, r3
	sub	r12, r12, r3
	mov	r4, #0
	mov	r5, #0
	mov	r6, #0
	mov	r7, #0
	mov	r8, #0
	mov	r9, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #31
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #16]
	ldr	r11, [r1, #20]
#else
	ldrd	r10, r11, [r1, #16]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #48]
	ldr	r11, [r1, #52]
#else
	ldrd	r10, r11, [r1, #48]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #80]
	ldr	r11, [r1, #84]
#else
	ldrd	r10, r11, [r1, #80]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	add	r1, r1, #0x60
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #30
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #16]
	ldr	r11, [r1, #20]
#else
	ldrd	r10, r11, [r1, #16]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #48]
	ldr	r11, [r1, #52]
#else
	ldrd	r10, r11, [r1, #48]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #80]
	ldr	r11, [r1, #84]
#else
	ldrd	r10, r11, [r1, #80]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	add	r1, r1, #0x60
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #29
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #16]
	ldr	r11, [r1, #20]
#else
	ldrd	r10, r11, [r1, #16]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #48]
	ldr	r11, [r1, #52]
#else
	ldrd	r10, r11, [r1, #48]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #80]
	ldr	r11, [r1, #84]
#else
	ldrd	r10, r11, [r1, #80]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	add	r1, r1, #0x60
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #28
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #16]
	ldr	r11, [r1, #20]
#else
	ldrd	r10, r11, [r1, #16]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #48]
	ldr	r11, [r1, #52]
#else
	ldrd	r10, r11, [r1, #48]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #80]
	ldr	r11, [r1, #84]
#else
	ldrd	r10, r11, [r1, #80]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	add	r1, r1, #0x60
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #27
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #16]
	ldr	r11, [r1, #20]
#else
	ldrd	r10, r11, [r1, #16]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #48]
	ldr	r11, [r1, #52]
#else
	ldrd	r10, r11, [r1, #48]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #80]
	ldr	r11, [r1, #84]
#else
	ldrd	r10, r11, [r1, #80]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	add	r1, r1, #0x60
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #26
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #16]
	ldr	r11, [r1, #20]
#else
	ldrd	r10, r11, [r1, #16]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #48]
	ldr	r11, [r1, #52]
#else
	ldrd	r10, r11, [r1, #48]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #80]
	ldr	r11, [r1, #84]
#else
	ldrd	r10, r11, [r1, #80]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	add	r1, r1, #0x60
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #25
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #16]
	ldr	r11, [r1, #20]
#else
	ldrd	r10, r11, [r1, #16]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #48]
	ldr	r11, [r1, #52]
#else
	ldrd	r10, r11, [r1, #48]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #80]
	ldr	r11, [r1, #84]
#else
	ldrd	r10, r11, [r1, #80]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	add	r1, r1, #0x60
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #24
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #16]
	ldr	r11, [r1, #20]
#else
	ldrd	r10, r11, [r1, #16]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #48]
	ldr	r11, [r1, #52]
#else
	ldrd	r10, r11, [r1, #48]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #80]
	ldr	r11, [r1, #84]
#else
	ldrd	r10, r11, [r1, #80]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	sub	r1, r1, #0x2a0
	mov	r10, #-1
	mov	r11, #-1
	rsbs	lr, lr, #0
	sbcs	r10, r10, r8
	sbcs	r11, r11, r9
	sbc	lr, lr, lr
	asr	r12, r2, #31
	eor	r3, r4, r6
	and	r3, r3, r12
	eor	r4, r4, r3
	eor	r6, r6, r3
	eor	r3, r5, r7
	and	r3, r3, r12
	eor	r5, r5, r3
	eor	r7, r7, r3
	eor	r10, r10, r8
	and	r10, r10, r12
	eor	r8, r8, r10
	eor	r11, r11, r9
	and	r11, r11, r12
	eor	r9, r9, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #16]
	str	r5, [r0, #20]
#else
	strd	r4, r5, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #48]
	str	r7, [r0, #52]
#else
	strd	r6, r7, [r0, #48]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #80]
	str	r9, [r0, #84]
#else
	strd	r8, r9, [r0, #80]
#endif
	sbfx	r3, r2, #7, #1
	eor	r12, r2, r3
	sub	r12, r12, r3
	mov	r4, #0
	mov	r5, #0
	mov	r6, #0
	mov	r7, #0
	mov	r8, #0
	mov	r9, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #31
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #24]
	ldr	r11, [r1, #28]
#else
	ldrd	r10, r11, [r1, #24]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #56]
	ldr	r11, [r1, #60]
#else
	ldrd	r10, r11, [r1, #56]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #88]
	ldr	r11, [r1, #92]
#else
	ldrd	r10, r11, [r1, #88]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	add	r1, r1, #0x60
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #30
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #24]
	ldr	r11, [r1, #28]
#else
	ldrd	r10, r11, [r1, #24]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #56]
	ldr	r11, [r1, #60]
#else
	ldrd	r10, r11, [r1, #56]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #88]
	ldr	r11, [r1, #92]
#else
	ldrd	r10, r11, [r1, #88]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	add	r1, r1, #0x60
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #29
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #24]
	ldr	r11, [r1, #28]
#else
	ldrd	r10, r11, [r1, #24]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #56]
	ldr	r11, [r1, #60]
#else
	ldrd	r10, r11, [r1, #56]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #88]
	ldr	r11, [r1, #92]
#else
	ldrd	r10, r11, [r1, #88]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	add	r1, r1, #0x60
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #28
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #24]
	ldr	r11, [r1, #28]
#else
	ldrd	r10, r11, [r1, #24]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #56]
	ldr	r11, [r1, #60]
#else
	ldrd	r10, r11, [r1, #56]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #88]
	ldr	r11, [r1, #92]
#else
	ldrd	r10, r11, [r1, #88]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	add	r1, r1, #0x60
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #27
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #24]
	ldr	r11, [r1, #28]
#else
	ldrd	r10, r11, [r1, #24]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #56]
	ldr	r11, [r1, #60]
#else
	ldrd	r10, r11, [r1, #56]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #88]
	ldr	r11, [r1, #92]
#else
	ldrd	r10, r11, [r1, #88]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	add	r1, r1, #0x60
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #26
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #24]
	ldr	r11, [r1, #28]
#else
	ldrd	r10, r11, [r1, #24]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #56]
	ldr	r11, [r1, #60]
#else
	ldrd	r10, r11, [r1, #56]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #88]
	ldr	r11, [r1, #92]
#else
	ldrd	r10, r11, [r1, #88]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	add	r1, r1, #0x60
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #25
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #24]
	ldr	r11, [r1, #28]
#else
	ldrd	r10, r11, [r1, #24]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #56]
	ldr	r11, [r1, #60]
#else
	ldrd	r10, r11, [r1, #56]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #88]
	ldr	r11, [r1, #92]
#else
	ldrd	r10, r11, [r1, #88]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	add	r1, r1, #0x60
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r3, #0x800000
	lsl	r3, r3, #8
	add	r3, r3, #0x0
#else
	mov	r3, #0x80000000
#endif
	ror	r3, r3, #24
	ror	r3, r3, r12
	asr	r3, r3, #31
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #24]
	ldr	r11, [r1, #28]
#else
	ldrd	r10, r11, [r1, #24]
#endif
	eor	r10, r10, r4
	eor	r11, r11, r5
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r4, r4, r10
	eor	r5, r5, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #56]
	ldr	r11, [r1, #60]
#else
	ldrd	r10, r11, [r1, #56]
#endif
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r6, r6, r10
	eor	r7, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #88]
	ldr	r11, [r1, #92]
#else
	ldrd	r10, r11, [r1, #88]
#endif
	eor	r10, r10, r8
	eor	r11, r11, r9
	and	r10, r10, r3
	and	r11, r11, r3
	eor	r8, r8, r10
	eor	r9, r9, r11
	sub	r1, r1, #0x2a0
	mov	r10, #-1
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	mov	r11, #0x7fffff
	lsl	r11, r11, #8
	add	r11, r11, #0xff
#else
	mov	r11, #0x7fffffff
#endif
	rsbs	lr, lr, #0
	sbcs	r10, r10, r8
	sbc	r11, r11, r9
	asr	r12, r2, #31
	eor	r3, r4, r6
	and	r3, r3, r12
	eor	r4, r4, r3
	eor	r6, r6, r3
	eor	r3, r5, r7
	and	r3, r3, r12
	eor	r5, r5, r3
	eor	r7, r7, r3
	eor	r10, r10, r8
	and	r10, r10, r12
	eor	r8, r8, r10
	eor	r11, r11, r9
	and	r11, r11, r12
	eor	r9, r9, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #24]
	str	r5, [r0, #28]
#else
	strd	r4, r5, [r0, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #56]
	str	r7, [r0, #60]
#else
	strd	r6, r7, [r0, #56]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #88]
	str	r9, [r0, #92]
#else
	strd	r8, r9, [r0, #88]
#endif
	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
	.size	fe_cmov_table,.-fe_cmov_table
	.text
	.align	4
	.globl	fe_mul
	.type	fe_mul, %function
fe_mul:
	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
	sub	sp, sp, #0x40
	# Multiply
	ldr	r7, [r1]
	ldr	r8, [r1, #4]
	ldr	r9, [r2]
	ldr	lr, [r2, #4]
	#  A[0] * B[0] = 0
	umull	r4, r5, r7, r9
	str	r4, [sp]
	#  A[0] * B[1] = 1
	umull	r3, r6, r7, lr
	adds	r5, r5, r3
	adc	r6, r6, #0
	#  A[1] * B[0] = 1
	umull	r3, r12, r8, r9
	adds	r5, r5, r3
	mov	r4, #0
	adcs	r6, r6, r12
	adc	r4, r4, #0
	str	r5, [sp, #4]
	#  A[2] * B[0] = 2
	ldr	r10, [r1, #8]
	umull	r3, r12, r10, r9
	adds	r6, r6, r3
	adc	r4, r4, r12
	#  A[1] * B[1] = 2
	umull	r3, r12, r8, lr
	adds	r6, r6, r3
	mov	r5, #0
	adcs	r4, r4, r12
	adc	r5, r5, #0
	#  A[0] * B[2] = 2
	ldr	r11, [r2, #8]
	umull	r3, r12, r7, r11
	adds	r6, r6, r3
	adcs	r4, r4, r12
	adc	r5, r5, #0
	str	r6, [sp, #8]
	#  A[0] * B[3] = 3
	ldr	r11, [r2, #12]
	umull	r3, r12, r7, r11
	adds	r4, r4, r3
	mov	r6, #0
	adcs	r5, r5, r12
	adc	r6, r6, #0
	#  A[1] * B[2] = 3
	ldr	r11, [r2, #8]
	umull	r3, r12, r8, r11
	adds	r4, r4, r3
	adcs	r5, r5, r12
	adc	r6, r6, #0
	#  A[2] * B[1] = 3
	umull	r3, r12, r10, lr
	adds	r4, r4, r3
	adcs	r5, r5, r12
	adc	r6, r6, #0
	#  A[3] * B[0] = 3
	ldr	r10, [r1, #12]
	umull	r3, r12, r10, r9
	adds	r4, r4, r3
	adcs	r5, r5, r12
	adc	r6, r6, #0
	str	r4, [sp, #12]
	#  A[4] * B[0] = 4
	ldr	r10, [r1, #16]
	umull	r3, r12, r10, r9
	adds	r5, r5, r3
	mov	r4, #0
	adcs	r6, r6, r12
	adc	r4, r4, #0
	#  A[3] * B[1] = 4
	ldr	r10, [r1, #12]
	umull	r3, r12, r10, lr
	adds	r5, r5, r3
	adcs	r6, r6, r12
	adc	r4, r4, #0
	#  A[2] * B[2] = 4
	ldr	r10, [r1, #8]
	umull	r3, r12, r10, r11
	adds	r5, r5, r3
	adcs	r6, r6, r12
	adc	r4, r4, #0
	#  A[1] * B[3] = 4
	ldr	r11, [r2, #12]
	umull	r3, r12, r8, r11
	adds	r5, r5, r3
	adcs	r6, r6, r12
	adc	r4, r4, #0
	#  A[0] * B[4] = 4
	ldr	r11, [r2, #16]
	umull	r3, r12, r7, r11
	adds	r5, r5, r3
	adcs	r6, r6, r12
	adc	r4, r4, #0
	str	r5, [sp, #16]
	#  A[0] * B[5] = 5
	ldr	r11, [r2, #20]
	umull	r3, r12, r7, r11
	adds	r6, r6, r3
	mov	r5, #0
	adcs	r4, r4, r12
	adc	r5, r5, #0
	#  A[1] * B[4] = 5
	ldr	r11, [r2, #16]
	umull	r3, r12, r8, r11
	adds	r6, r6, r3
	adcs	r4, r4, r12
	adc	r5, r5, #0
	#  A[2] * B[3] = 5
	ldr	r11, [r2, #12]
	umull	r3, r12, r10, r11
	adds	r6, r6, r3
	adcs	r4, r4, r12
	adc	r5, r5, #0
	#  A[3] * B[2] = 5
	ldr	r10, [r1, #12]
	ldr	r11, [r2, #8]
	umull	r3, r12, r10, r11
	adds	r6, r6, r3
	adcs	r4, r4, r12
	adc	r5, r5, #0
	#  A[4] * B[1] = 5
	ldr	r10, [r1, #16]
	umull	r3, r12, r10, lr
	adds	r6, r6, r3
	adcs	r4, r4, r12
	adc	r5, r5, #0
	#  A[5] * B[0] = 5
	ldr	r10, [r1, #20]
	umull	r3, r12, r10, r9
	adds	r6, r6, r3
	adcs	r4, r4, r12
	adc	r5, r5, #0
	str	r6, [sp, #20]
	#  A[6] * B[0] = 6
	ldr	r10, [r1, #24]
	umull	r3, r12, r10, r9
	adds	r4, r4, r3
	mov	r6, #0
	adcs	r5, r5, r12
	adc	r6, r6, #0
	#  A[5] * B[1] = 6
	ldr	r10, [r1, #20]
	umull	r3, r12, r10, lr
	adds	r4, r4, r3
	adcs	r5, r5, r12
	adc	r6, r6, #0
	#  A[4] * B[2] = 6
	ldr	r10, [r1, #16]
	umull	r3, r12, r10, r11
	adds	r4, r4, r3
	adcs	r5, r5, r12
	adc	r6, r6, #0
	#  A[3] * B[3] = 6
	ldr	r10, [r1, #12]
	ldr	r11, [r2, #12]
	umull	r3, r12, r10, r11
	adds	r4, r4, r3
	adcs	r5, r5, r12
	adc	r6, r6, #0
	#  A[2] * B[4] = 6
	ldr	r10, [r1, #8]
	ldr	r11, [r2, #16]
	umull	r3, r12, r10, r11
	adds	r4, r4, r3
	adcs	r5, r5, r12
	adc	r6, r6, #0
	#  A[1] * B[5] = 6
	ldr	r11, [r2, #20]
	umull	r3, r12, r8, r11
	adds	r4, r4, r3
	adcs	r5, r5, r12
	adc	r6, r6, #0
	#  A[0] * B[6] = 6
	ldr	r11, [r2, #24]
	umull	r3, r12, r7, r11
	adds	r4, r4, r3
	adcs	r5, r5, r12
	adc	r6, r6, #0
	str	r4, [sp, #24]
	#  A[0] * B[7] = 7
	ldr	r11, [r2, #28]
	umull	r3, r12, r7, r11
	adds	r5, r5, r3
	mov	r4, #0
	adcs	r6, r6, r12
	adc	r4, r4, #0
	#  A[1] * B[6] = 7
	ldr	r11, [r2, #24]
	umull	r3, r12, r8, r11
	adds	r5, r5, r3
	adcs	r6, r6, r12
	adc	r4, r4, #0
	#  A[2] * B[5] = 7
	ldr	r11, [r2, #20]
	umull	r3, r12, r10, r11
	adds	r5, r5, r3
	adcs	r6, r6, r12
	adc	r4, r4, #0
	#  A[3] * B[4] = 7
	ldr	r10, [r1, #12]
	ldr	r11, [r2, #16]
	umull	r3, r12, r10, r11
	adds	r5, r5, r3
	adcs	r6, r6, r12
	adc	r4, r4, #0
	#  A[4] * B[3] = 7
	ldr	r10, [r1, #16]
	ldr	r11, [r2, #12]
	umull	r3, r12, r10, r11
	adds	r5, r5, r3
	adcs	r6, r6, r12
	adc	r4, r4, #0
	#  A[5] * B[2] = 7
	ldr	r10, [r1, #20]
	ldr	r11, [r2, #8]
	umull	r3, r12, r10, r11
	adds	r5, r5, r3
	adcs	r6, r6, r12
	adc	r4, r4, #0
	#  A[6] * B[1] = 7
	ldr	r10, [r1, #24]
	umull	r3, r12, r10, lr
	adds	r5, r5, r3
	adcs	r6, r6, r12
	adc	r4, r4, #0
	#  A[7] * B[0] = 7
	ldr	r10, [r1, #28]
	umull	r3, r12, r10, r9
	adds	r5, r5, r3
	adcs	r6, r6, r12
	adc	r4, r4, #0
	str	r5, [sp, #28]
	ldr	r7, [r1, #24]
	ldr	r9, [r2, #24]
	#  A[7] * B[1] = 8
	umull	r3, r12, r10, lr
	adds	r6, r6, r3
	mov	r5, #0
	adcs	r4, r4, r12
	adc	r5, r5, #0
	#  A[6] * B[2] = 8
	umull	r3, r12, r7, r11
	adds	r6, r6, r3
	adcs	r4, r4, r12
	adc	r5, r5, #0
	#  A[5] * B[3] = 8
	ldr	r10, [r1, #20]
	ldr	r11, [r2, #12]
	umull	r3, r12, r10, r11
	adds	r6, r6, r3
	adcs	r4, r4, r12
	adc	r5, r5, #0
	#  A[4] * B[4] = 8
	ldr	r10, [r1, #16]
	ldr	r11, [r2, #16]
	umull	r3, r12, r10, r11
	adds	r6, r6, r3
	adcs	r4, r4, r12
	adc	r5, r5, #0
	#  A[3] * B[5] = 8
	ldr	r10, [r1, #12]
	ldr	r11, [r2, #20]
	umull	r3, r12, r10, r11
	adds	r6, r6, r3
	adcs	r4, r4, r12
	adc	r5, r5, #0
	#  A[2] * B[6] = 8
	ldr	r10, [r1, #8]
	umull	r3, r12, r10, r9
	adds	r6, r6, r3
	adcs	r4, r4, r12
	adc	r5, r5, #0
	#  A[1] * B[7] = 8
	ldr	r11, [r2, #28]
	umull	r3, r12, r8, r11
	adds	r6, r6, r3
	adcs	r4, r4, r12
	adc	r5, r5, #0
	str	r6, [sp, #32]
	ldr	r8, [r1, #28]
	mov	lr, r11
	#  A[2] * B[7] = 9
	umull	r3, r12, r10, lr
	adds	r4, r4, r3
	mov	r6, #0
	adcs	r5, r5, r12
	adc	r6, r6, #0
	#  A[3] * B[6] = 9
	ldr	r10, [r1, #12]
	umull	r3, r12, r10, r9
	adds	r4, r4, r3
	adcs	r5, r5, r12
	adc	r6, r6, #0
	#  A[4] * B[5] = 9
	ldr	r10, [r1, #16]
	ldr	r11, [r2, #20]
	umull	r3, r12, r10, r11
	adds	r4, r4, r3
	adcs	r5, r5, r12
	adc	r6, r6, #0
	#  A[5] * B[4] = 9
	ldr	r10, [r1, #20]
	ldr	r11, [r2, #16]
	umull	r3, r12, r10, r11
	adds	r4, r4, r3
	adcs	r5, r5, r12
	adc	r6, r6, #0
	#  A[6] * B[3] = 9
	ldr	r11, [r2, #12]
	umull	r3, r12, r7, r11
	adds	r4, r4, r3
	adcs	r5, r5, r12
	adc	r6, r6, #0
	#  A[7] * B[2] = 9
	ldr	r11, [r2, #8]
	umull	r3, r12, r8, r11
	adds	r4, r4, r3
	adcs	r5, r5, r12
	adc	r6, r6, #0
	str	r4, [sp, #36]
	#  A[7] * B[3] = 10
	ldr	r11, [r2, #12]
	umull	r3, r12, r8, r11
	adds	r5, r5, r3
	mov	r4, #0
	adcs	r6, r6, r12
	adc	r4, r4, #0
	#  A[6] * B[4] = 10
	ldr	r11, [r2, #16]
	umull	r3, r12, r7, r11
	adds	r5, r5, r3
	adcs	r6, r6, r12
	adc	r4, r4, #0
	#  A[5] * B[5] = 10
	ldr	r11, [r2, #20]
	umull	r3, r12, r10, r11
	adds	r5, r5, r3
	adcs	r6, r6, r12
	adc	r4, r4, #0
	#  A[4] * B[6] = 10
	ldr	r10, [r1, #16]
	umull	r3, r12, r10, r9
	adds	r5, r5, r3
	adcs	r6, r6, r12
	adc	r4, r4, #0
	#  A[3] * B[7] = 10
	ldr	r10, [r1, #12]
	umull	r3, r12, r10, lr
	adds	r5, r5, r3
	adcs	r6, r6, r12
	adc	r4, r4, #0
	str	r5, [sp, #40]
	#  A[4] * B[7] = 11
	ldr	r10, [r1, #16]
	umull	r3, r12, r10, lr
	adds	r6, r6, r3
	mov	r5, #0
	adcs	r4, r4, r12
	adc	r5, r5, #0
	#  A[5] * B[6] = 11
	ldr	r10, [r1, #20]
	umull	r3, r12, r10, r9
	adds	r6, r6, r3
	adcs	r4, r4, r12
	adc	r5, r5, #0
	#  A[6] * B[5] = 11
	umull	r3, r12, r7, r11
	adds	r6, r6, r3
	adcs	r4, r4, r12
	adc	r5, r5, #0
	#  A[7] * B[4] = 11
	ldr	r11, [r2, #16]
	umull	r3, r12, r8, r11
	adds	r6, r6, r3
	adcs	r4, r4, r12
	adc	r5, r5, #0
	str	r6, [sp, #44]
	#  A[7] * B[5] = 12
	ldr	r11, [r2, #20]
	umull	r3, r12, r8, r11
	adds	r4, r4, r3
	mov	r6, #0
	adcs	r5, r5, r12
	adc	r6, r6, #0
	#  A[6] * B[6] = 12
	umull	r3, r12, r7, r9
	adds	r4, r4, r3
	adcs	r5, r5, r12
	adc	r6, r6, #0
	#  A[5] * B[7] = 12
	umull	r3, r12, r10, lr
	adds	r4, r4, r3
	adcs	r5, r5, r12
	adc	r6, r6, #0
	str	r4, [sp, #48]
	#  A[6] * B[7] = 13
	umull	r3, r12, r7, lr
	adds	r5, r5, r3
	mov	r4, #0
	adcs	r6, r6, r12
	adc	r4, r4, #0
	#  A[7] * B[6] = 13
	umull	r3, r12, r8, r9
	adds	r5, r5, r3
	adcs	r6, r6, r12
	adc	r4, r4, #0
	str	r5, [sp, #52]
	#  A[7] * B[7] = 14
	umull	r3, r12, r8, lr
	adds	r6, r6, r3
	adc	r4, r4, r12
	str	r6, [sp, #56]
	str	r4, [sp, #60]
	# Reduce
	#  Load bottom half
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp]
	ldr	r5, [sp, #4]
#else
	ldrd	r4, r5, [sp]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp, #8]
	ldr	r7, [sp, #12]
#else
	ldrd	r6, r7, [sp, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [sp, #16]
	ldr	r9, [sp, #20]
#else
	ldrd	r8, r9, [sp, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [sp, #24]
	ldr	r11, [sp, #28]
#else
	ldrd	r10, r11, [sp, #24]
#endif
	lsr	r3, r11, #31
	and	r11, r11, #0x7fffffff
	mov	lr, #19
	ldr	r1, [sp, #32]
	orr	r3, r3, r1, lsl #1
	umull	r3, r12, lr, r3
	adds	r4, r4, r3
	mov	r2, #0
	adcs	r5, r5, r12
	adc	r2, r2, #0
	lsr	r3, r1, #31
	ldr	r1, [sp, #36]
	orr	r3, r3, r1, lsl #1
	umull	r3, r12, lr, r3
	add	r12, r12, r2
	adds	r5, r5, r3
	mov	r2, #0
	adcs	r6, r6, r12
	adc	r2, r2, #0
	lsr	r3, r1, #31
	ldr	r1, [sp, #40]
	orr	r3, r3, r1, lsl #1
	umull	r3, r12, lr, r3
	add	r12, r12, r2
	adds	r6, r6, r3
	mov	r2, #0
	adcs	r7, r7, r12
	adc	r2, r2, #0
	lsr	r3, r1, #31
	ldr	r1, [sp, #44]
	orr	r3, r3, r1, lsl #1
	umull	r3, r12, lr, r3
	add	r12, r12, r2
	adds	r7, r7, r3
	mov	r2, #0
	adcs	r8, r8, r12
	adc	r2, r2, #0
	lsr	r3, r1, #31
	ldr	r1, [sp, #48]
	orr	r3, r3, r1, lsl #1
	umull	r3, r12, lr, r3
	add	r12, r12, r2
	adds	r8, r8, r3
	mov	r2, #0
	adcs	r9, r9, r12
	adc	r2, r2, #0
	lsr	r3, r1, #31
	ldr	r1, [sp, #52]
	orr	r3, r3, r1, lsl #1
	umull	r3, r12, lr, r3
	add	r12, r12, r2
	adds	r9, r9, r3
	mov	r2, #0
	adcs	r10, r10, r12
	adc	r2, r2, #0
	lsr	r3, r1, #31
	ldr	r1, [sp, #56]
	orr	r3, r3, r1, lsl #1
	umull	r3, r12, lr, r3
	add	r12, r12, r2
	adds	r10, r10, r3
	mov	r2, #0
	adcs	r11, r11, r12
	adc	r2, r2, #0
	lsr	r3, r1, #31
	ldr	r1, [sp, #60]
	orr	r3, r3, r1, lsl #1
	umull	r3, r12, lr, r3
	adds	r11, r11, r3
	adc	r3, r12, r2
	#  Overflow
	lsl	r3, r3, #1
	orr	r3, r3, r11, lsr #31
	mul	r3, r3, lr
	and	r11, r11, #0x7fffffff
	adds	r4, r4, r3
	adcs	r5, r5, #0
	adcs	r6, r6, #0
	adcs	r7, r7, #0
	adcs	r8, r8, #0
	adcs	r9, r9, #0
	adcs	r10, r10, #0
	adc	r11, r11, #0
	# Reduce if top bit set
	asr	r3, r11, #31
	and	r3, r3, lr
	and	r11, r11, #0x7fffffff
	adds	r4, r4, r3
	adcs	r5, r5, #0
	adcs	r6, r6, #0
	adcs	r7, r7, #0
	adcs	r8, r8, #0
	adcs	r9, r9, #0
	adcs	r10, r10, #0
	adc	r11, r11, #0
	# Store
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #8]
	str	r7, [r0, #12]
#else
	strd	r6, r7, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #24]
	str	r11, [r0, #28]
#else
	strd	r10, r11, [r0, #24]
#endif
	add	sp, sp, #0x40
	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
	.size	fe_mul,.-fe_mul
	.text
	.align	4
	.globl	fe_sq
	.type	fe_sq, %function
fe_sq:
	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
	sub	sp, sp, #0x40
	# Square
	ldr	r7, [r1]
	ldr	r8, [r1, #4]
	ldr	r9, [r1, #8]
	ldr	r10, [r1, #12]
	ldr	r12, [r1, #16]
	#  A[0] * A[0] = 0
	umull	r4, r5, r7, r7
	str	r4, [sp]
	#  A[0] * A[1] = 1
	umull	r2, r3, r7, r8
	mov	r6, #0
	adds	r5, r5, r2
	adc	r6, r6, r3
	adds	r5, r5, r2
	mov	r4, #0
	adcs	r6, r6, r3
	adc	r4, r4, #0
	str	r5, [sp, #4]
	#  A[1] * A[1] = 2
	umull	r2, r3, r8, r8
	adds	r6, r6, r2
	adc	r4, r4, r3
	#  A[0] * A[2] = 2
	umull	r2, r3, r7, r9
	adds	r6, r6, r2
	mov	r5, #0
	adcs	r4, r4, r3
	adc	r5, r5, #0
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	str	r6, [sp, #8]
	#  A[0] * A[3] = 3
	umull	r2, r3, r7, r10
	adds	r4, r4, r2
	adc	r5, r5, r3
	adds	r4, r4, r2
	mov	r6, #0
	adcs	r5, r5, r3
	adc	r6, r6, #0
	#  A[1] * A[2] = 3
	umull	r2, r3, r8, r9
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	str	r4, [sp, #12]
	#  A[2] * A[2] = 4
	umull	r2, r3, r9, r9
	adds	r5, r5, r2
	mov	r4, #0
	adcs	r6, r6, r3
	adc	r4, r4, #0
	#  A[1] * A[3] = 4
	umull	r2, r3, r8, r10
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	#  A[0] * A[4] = 4
	umull	r2, r3, r7, r12
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	str	r5, [sp, #16]
	#  A[0] * A[5] = 5
	ldr	r11, [r1, #20]
	umull	r2, r3, r7, r11
	adds	r6, r6, r2
	mov	r5, #0
	adcs	r4, r4, r3
	adc	r5, r5, #0
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	#  A[1] * A[4] = 5
	umull	r2, r3, r8, r12
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	#  A[2] * A[3] = 5
	umull	r2, r3, r9, r10
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	str	r6, [sp, #20]
	#  A[3] * A[3] = 6
	umull	r2, r3, r10, r10
	adds	r4, r4, r2
	mov	r6, #0
	adcs	r5, r5, r3
	adc	r6, r6, #0
	#  A[2] * A[4] = 6
	umull	r2, r3, r9, r12
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	#  A[1] * A[5] = 6
	umull	r2, r3, r8, r11
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	#  A[0] * A[6] = 6
	ldr	r11, [r1, #24]
	umull	r2, r3, r7, r11
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	str	r4, [sp, #24]
	#  A[0] * A[7] = 7
	ldr	r11, [r1, #28]
	umull	r2, r3, r7, r11
	adds	r5, r5, r2
	mov	r4, #0
	adcs	r6, r6, r3
	adc	r4, r4, #0
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	#  A[1] * A[6] = 7
	ldr	r11, [r1, #24]
	umull	r2, r3, r8, r11
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	#  A[2] * A[5] = 7
	ldr	r11, [r1, #20]
	umull	r2, r3, r9, r11
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	#  A[3] * A[4] = 7
	umull	r2, r3, r10, r12
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	str	r5, [sp, #28]
	#  A[4] * A[4] = 8
	umull	r2, r3, r12, r12
	adds	r6, r6, r2
	mov	r5, #0
	adcs	r4, r4, r3
	adc	r5, r5, #0
	#  A[3] * A[5] = 8
	umull	r2, r3, r10, r11
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	#  A[2] * A[6] = 8
	ldr	r11, [r1, #24]
	umull	r2, r3, r9, r11
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	#  A[1] * A[7] = 8
	ldr	r11, [r1, #28]
	umull	r2, r3, r8, r11
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	str	r6, [sp, #32]
	ldr	r7, [r1, #20]
	#  A[2] * A[7] = 9
	umull	r2, r3, r9, r11
	adds	r4, r4, r2
	mov	r6, #0
	adcs	r5, r5, r3
	adc	r6, r6, #0
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	#  A[3] * A[6] = 9
	ldr	r11, [r1, #24]
	umull	r2, r3, r10, r11
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	#  A[4] * A[5] = 9
	umull	r2, r3, r12, r7
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	str	r4, [sp, #36]
	mov	r8, r11
	#  A[5] * A[5] = 10
	umull	r2, r3, r7, r7
	adds	r5, r5, r2
	mov	r4, #0
	adcs	r6, r6, r3
	adc	r4, r4, #0
	#  A[4] * A[6] = 10
	umull	r2, r3, r12, r8
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	#  A[3] * A[7] = 10
	ldr	r11, [r1, #28]
	umull	r2, r3, r10, r11
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	str	r5, [sp, #40]
	mov	r9, r11
	#  A[4] * A[7] = 11
	umull	r2, r3, r12, r9
	adds	r6, r6, r2
	mov	r5, #0
	adcs	r4, r4, r3
	adc	r5, r5, #0
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	#  A[5] * A[6] = 11
	umull	r2, r3, r7, r8
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	str	r6, [sp, #44]
	#  A[6] * A[6] = 12
	umull	r2, r3, r8, r8
	adds	r4, r4, r2
	mov	r6, #0
	adcs	r5, r5, r3
	adc	r6, r6, #0
	#  A[5] * A[7] = 12
	umull	r2, r3, r7, r9
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	str	r4, [sp, #48]
	#  A[6] * A[7] = 13
	umull	r2, r3, r8, r9
	adds	r5, r5, r2
	mov	r4, #0
	adcs	r6, r6, r3
	adc	r4, r4, #0
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	str	r5, [sp, #52]
	#  A[7] * A[7] = 14
	umull	r2, r3, r9, r9
	adds	r6, r6, r2
	adc	r4, r4, r3
	str	r6, [sp, #56]
	str	r4, [sp, #60]
	# Reduce
	#  Load bottom half
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp]
	ldr	r5, [sp, #4]
#else
	ldrd	r4, r5, [sp]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp, #8]
	ldr	r7, [sp, #12]
#else
	ldrd	r6, r7, [sp, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [sp, #16]
	ldr	r9, [sp, #20]
#else
	ldrd	r8, r9, [sp, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [sp, #24]
	ldr	r11, [sp, #28]
#else
	ldrd	r10, r11, [sp, #24]
#endif
	lsr	r2, r11, #31
	and	r11, r11, #0x7fffffff
	mov	r12, #19
	ldr	r1, [sp, #32]
	orr	r2, r2, r1, lsl #1
	umull	r2, r3, r12, r2
	adds	r4, r4, r2
	mov	lr, #0
	adcs	r5, r5, r3
	adc	lr, lr, #0
	lsr	r2, r1, #31
	ldr	r1, [sp, #36]
	orr	r2, r2, r1, lsl #1
	umull	r2, r3, r12, r2
	add	r3, r3, lr
	adds	r5, r5, r2
	mov	lr, #0
	adcs	r6, r6, r3
	adc	lr, lr, #0
	lsr	r2, r1, #31
	ldr	r1, [sp, #40]
	orr	r2, r2, r1, lsl #1
	umull	r2, r3, r12, r2
	add	r3, r3, lr
	adds	r6, r6, r2
	mov	lr, #0
	adcs	r7, r7, r3
	adc	lr, lr, #0
	lsr	r2, r1, #31
	ldr	r1, [sp, #44]
	orr	r2, r2, r1, lsl #1
	umull	r2, r3, r12, r2
	add	r3, r3, lr
	adds	r7, r7, r2
	mov	lr, #0
	adcs	r8, r8, r3
	adc	lr, lr, #0
	lsr	r2, r1, #31
	ldr	r1, [sp, #48]
	orr	r2, r2, r1, lsl #1
	umull	r2, r3, r12, r2
	add	r3, r3, lr
	adds	r8, r8, r2
	mov	lr, #0
	adcs	r9, r9, r3
	adc	lr, lr, #0
	lsr	r2, r1, #31
	ldr	r1, [sp, #52]
	orr	r2, r2, r1, lsl #1
	umull	r2, r3, r12, r2
	add	r3, r3, lr
	adds	r9, r9, r2
	mov	lr, #0
	adcs	r10, r10, r3
	adc	lr, lr, #0
	lsr	r2, r1, #31
	ldr	r1, [sp, #56]
	orr	r2, r2, r1, lsl #1
	umull	r2, r3, r12, r2
	add	r3, r3, lr
	adds	r10, r10, r2
	mov	lr, #0
	adcs	r11, r11, r3
	adc	lr, lr, #0
	lsr	r2, r1, #31
	ldr	r1, [sp, #60]
	orr	r2, r2, r1, lsl #1
	umull	r2, r3, r12, r2
	adds	r11, r11, r2
	adc	r2, r3, lr
	#  Overflow
	lsl	r2, r2, #1
	orr	r2, r2, r11, lsr #31
	mul	r2, r2, r12
	and	r11, r11, #0x7fffffff
	adds	r4, r4, r2
	adcs	r5, r5, #0
	adcs	r6, r6, #0
	adcs	r7, r7, #0
	adcs	r8, r8, #0
	adcs	r9, r9, #0
	adcs	r10, r10, #0
	adc	r11, r11, #0
	# Reduce if top bit set
	asr	r2, r11, #31
	and	r2, r2, r12
	and	r11, r11, #0x7fffffff
	adds	r4, r4, r2
	adcs	r5, r5, #0
	adcs	r6, r6, #0
	adcs	r7, r7, #0
	adcs	r8, r8, #0
	adcs	r9, r9, #0
	adcs	r10, r10, #0
	adc	r11, r11, #0
	# Store
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #8]
	str	r7, [r0, #12]
#else
	strd	r6, r7, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #24]
	str	r11, [r0, #28]
#else
	strd	r10, r11, [r0, #24]
#endif
	add	sp, sp, #0x40
	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
	.size	fe_sq,.-fe_sq
	.text
	.align	4
	.globl	fe_mul121666
	.type	fe_mul121666, %function
fe_mul121666:
	push	{r4, r5, r6, r7, r8, r9, r10, lr}
	# Multiply by 121666
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r2, [r1]
	ldr	r3, [r1, #4]
#else
	ldrd	r2, r3, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #8]
	ldr	r5, [r1, #12]
#else
	ldrd	r4, r5, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #16]
	ldr	r7, [r1, #20]
#else
	ldrd	r6, r7, [r1, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r1, #24]
	ldr	r9, [r1, #28]
#else
	ldrd	r8, r9, [r1, #24]
#endif
	movw	lr, #0xdb42
	movt	lr, #1
	umull	r2, r10, r2, lr
	umull	r3, r12, r3, lr
	adds	r3, r3, r10
	adc	r10, r12, #0
	umull	r4, r12, r4, lr
	adds	r4, r4, r10
	adc	r10, r12, #0
	umull	r5, r12, r5, lr
	adds	r5, r5, r10
	adc	r10, r12, #0
	umull	r6, r12, r6, lr
	adds	r6, r6, r10
	adc	r10, r12, #0
	umull	r7, r12, r7, lr
	adds	r7, r7, r10
	adc	r10, r12, #0
	umull	r8, r12, r8, lr
	adds	r8, r8, r10
	adc	r10, r12, #0
	umull	r9, r12, r9, lr
	adds	r9, r9, r10
	adc	r10, r12, #0
	mov	lr, #19
	lsl	r10, r10, #1
	orr	r10, r10, r9, lsr #31
	mul	r10, r10, lr
	and	r9, r9, #0x7fffffff
	adds	r2, r2, r10
	adcs	r3, r3, #0
	adcs	r4, r4, #0
	adcs	r5, r5, #0
	adcs	r6, r6, #0
	adcs	r7, r7, #0
	adcs	r8, r8, #0
	adc	r9, r9, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r2, [r0]
	str	r3, [r0, #4]
#else
	strd	r2, r3, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #8]
	str	r5, [r0, #12]
#else
	strd	r4, r5, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #16]
	str	r7, [r0, #20]
#else
	strd	r6, r7, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #24]
	str	r9, [r0, #28]
#else
	strd	r8, r9, [r0, #24]
#endif
	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
	.size	fe_mul121666,.-fe_mul121666
	.text
	.align	4
	.globl	fe_sq2
	.type	fe_sq2, %function
fe_sq2:
	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
	sub	sp, sp, #0x40
	# Square * 2
	ldr	r7, [r1]
	ldr	r8, [r1, #4]
	ldr	r9, [r1, #8]
	ldr	r10, [r1, #12]
	ldr	r12, [r1, #16]
	#  A[0] * A[0] = 0
	umull	r4, r5, r7, r7
	str	r4, [sp]
	#  A[0] * A[1] = 1
	umull	r2, r3, r7, r8
	mov	r6, #0
	adds	r5, r5, r2
	adc	r6, r6, r3
	adds	r5, r5, r2
	mov	r4, #0
	adcs	r6, r6, r3
	adc	r4, r4, #0
	str	r5, [sp, #4]
	#  A[1] * A[1] = 2
	umull	r2, r3, r8, r8
	adds	r6, r6, r2
	adc	r4, r4, r3
	#  A[0] * A[2] = 2
	umull	r2, r3, r7, r9
	adds	r6, r6, r2
	mov	r5, #0
	adcs	r4, r4, r3
	adc	r5, r5, #0
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	str	r6, [sp, #8]
	#  A[0] * A[3] = 3
	umull	r2, r3, r7, r10
	adds	r4, r4, r2
	adc	r5, r5, r3
	adds	r4, r4, r2
	mov	r6, #0
	adcs	r5, r5, r3
	adc	r6, r6, #0
	#  A[1] * A[2] = 3
	umull	r2, r3, r8, r9
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	str	r4, [sp, #12]
	#  A[2] * A[2] = 4
	umull	r2, r3, r9, r9
	adds	r5, r5, r2
	mov	r4, #0
	adcs	r6, r6, r3
	adc	r4, r4, #0
	#  A[1] * A[3] = 4
	umull	r2, r3, r8, r10
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	#  A[0] * A[4] = 4
	umull	r2, r3, r7, r12
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	str	r5, [sp, #16]
	#  A[0] * A[5] = 5
	ldr	r11, [r1, #20]
	umull	r2, r3, r7, r11
	adds	r6, r6, r2
	mov	r5, #0
	adcs	r4, r4, r3
	adc	r5, r5, #0
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	#  A[1] * A[4] = 5
	umull	r2, r3, r8, r12
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	#  A[2] * A[3] = 5
	umull	r2, r3, r9, r10
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	str	r6, [sp, #20]
	#  A[3] * A[3] = 6
	umull	r2, r3, r10, r10
	adds	r4, r4, r2
	mov	r6, #0
	adcs	r5, r5, r3
	adc	r6, r6, #0
	#  A[2] * A[4] = 6
	umull	r2, r3, r9, r12
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	#  A[1] * A[5] = 6
	umull	r2, r3, r8, r11
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	#  A[0] * A[6] = 6
	ldr	r11, [r1, #24]
	umull	r2, r3, r7, r11
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	str	r4, [sp, #24]
	#  A[0] * A[7] = 7
	ldr	r11, [r1, #28]
	umull	r2, r3, r7, r11
	adds	r5, r5, r2
	mov	r4, #0
	adcs	r6, r6, r3
	adc	r4, r4, #0
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	#  A[1] * A[6] = 7
	ldr	r11, [r1, #24]
	umull	r2, r3, r8, r11
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	#  A[2] * A[5] = 7
	ldr	r11, [r1, #20]
	umull	r2, r3, r9, r11
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	#  A[3] * A[4] = 7
	umull	r2, r3, r10, r12
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	str	r5, [sp, #28]
	#  A[4] * A[4] = 8
	umull	r2, r3, r12, r12
	adds	r6, r6, r2
	mov	r5, #0
	adcs	r4, r4, r3
	adc	r5, r5, #0
	#  A[3] * A[5] = 8
	umull	r2, r3, r10, r11
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	#  A[2] * A[6] = 8
	ldr	r11, [r1, #24]
	umull	r2, r3, r9, r11
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	#  A[1] * A[7] = 8
	ldr	r11, [r1, #28]
	umull	r2, r3, r8, r11
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	str	r6, [sp, #32]
	ldr	r7, [r1, #20]
	#  A[2] * A[7] = 9
	umull	r2, r3, r9, r11
	adds	r4, r4, r2
	mov	r6, #0
	adcs	r5, r5, r3
	adc	r6, r6, #0
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	#  A[3] * A[6] = 9
	ldr	r11, [r1, #24]
	umull	r2, r3, r10, r11
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	#  A[4] * A[5] = 9
	umull	r2, r3, r12, r7
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	str	r4, [sp, #36]
	mov	r8, r11
	#  A[5] * A[5] = 10
	umull	r2, r3, r7, r7
	adds	r5, r5, r2
	mov	r4, #0
	adcs	r6, r6, r3
	adc	r4, r4, #0
	#  A[4] * A[6] = 10
	umull	r2, r3, r12, r8
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	#  A[3] * A[7] = 10
	ldr	r11, [r1, #28]
	umull	r2, r3, r10, r11
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	str	r5, [sp, #40]
	mov	r9, r11
	#  A[4] * A[7] = 11
	umull	r2, r3, r12, r9
	adds	r6, r6, r2
	mov	r5, #0
	adcs	r4, r4, r3
	adc	r5, r5, #0
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	#  A[5] * A[6] = 11
	umull	r2, r3, r7, r8
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	adds	r6, r6, r2
	adcs	r4, r4, r3
	adc	r5, r5, #0
	str	r6, [sp, #44]
	#  A[6] * A[6] = 12
	umull	r2, r3, r8, r8
	adds	r4, r4, r2
	mov	r6, #0
	adcs	r5, r5, r3
	adc	r6, r6, #0
	#  A[5] * A[7] = 12
	umull	r2, r3, r7, r9
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	adds	r4, r4, r2
	adcs	r5, r5, r3
	adc	r6, r6, #0
	str	r4, [sp, #48]
	#  A[6] * A[7] = 13
	umull	r2, r3, r8, r9
	adds	r5, r5, r2
	mov	r4, #0
	adcs	r6, r6, r3
	adc	r4, r4, #0
	adds	r5, r5, r2
	adcs	r6, r6, r3
	adc	r4, r4, #0
	str	r5, [sp, #52]
	#  A[7] * A[7] = 14
	umull	r2, r3, r9, r9
	adds	r6, r6, r2
	adc	r4, r4, r3
	str	r6, [sp, #56]
	str	r4, [sp, #60]
	# Double and Reduce
	#  Load bottom half
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp]
	ldr	r5, [sp, #4]
#else
	ldrd	r4, r5, [sp]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp, #8]
	ldr	r7, [sp, #12]
#else
	ldrd	r6, r7, [sp, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [sp, #16]
	ldr	r9, [sp, #20]
#else
	ldrd	r8, r9, [sp, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [sp, #24]
	ldr	r11, [sp, #28]
#else
	ldrd	r10, r11, [sp, #24]
#endif
	lsr	r2, r11, #30
	lsl	r11, r11, #1
	orr	r11, r11, r10, lsr #31
	lsl	r10, r10, #1
	orr	r10, r10, r9, lsr #31
	lsl	r9, r9, #1
	orr	r9, r9, r8, lsr #31
	lsl	r8, r8, #1
	orr	r8, r8, r7, lsr #31
	lsl	r7, r7, #1
	orr	r7, r7, r6, lsr #31
	lsl	r6, r6, #1
	orr	r6, r6, r5, lsr #31
	lsl	r5, r5, #1
	orr	r5, r5, r4, lsr #31
	lsl	r4, r4, #1
	and	r11, r11, #0x7fffffff
	mov	r12, #19
	ldr	r1, [sp, #32]
	orr	r2, r2, r1, lsl #2
	umull	r2, r3, r12, r2
	adds	r4, r4, r2
	mov	lr, #0
	adcs	r5, r5, r3
	adc	lr, lr, #0
	lsr	r2, r1, #30
	ldr	r1, [sp, #36]
	orr	r2, r2, r1, lsl #2
	umull	r2, r3, r12, r2
	add	r3, r3, lr
	adds	r5, r5, r2
	mov	lr, #0
	adcs	r6, r6, r3
	adc	lr, lr, #0
	lsr	r2, r1, #30
	ldr	r1, [sp, #40]
	orr	r2, r2, r1, lsl #2
	umull	r2, r3, r12, r2
	add	r3, r3, lr
	adds	r6, r6, r2
	mov	lr, #0
	adcs	r7, r7, r3
	adc	lr, lr, #0
	lsr	r2, r1, #30
	ldr	r1, [sp, #44]
	orr	r2, r2, r1, lsl #2
	umull	r2, r3, r12, r2
	add	r3, r3, lr
	adds	r7, r7, r2
	mov	lr, #0
	adcs	r8, r8, r3
	adc	lr, lr, #0
	lsr	r2, r1, #30
	ldr	r1, [sp, #48]
	orr	r2, r2, r1, lsl #2
	umull	r2, r3, r12, r2
	add	r3, r3, lr
	adds	r8, r8, r2
	mov	lr, #0
	adcs	r9, r9, r3
	adc	lr, lr, #0
	lsr	r2, r1, #30
	ldr	r1, [sp, #52]
	orr	r2, r2, r1, lsl #2
	umull	r2, r3, r12, r2
	add	r3, r3, lr
	adds	r9, r9, r2
	mov	lr, #0
	adcs	r10, r10, r3
	adc	lr, lr, #0
	lsr	r2, r1, #30
	ldr	r1, [sp, #56]
	orr	r2, r2, r1, lsl #2
	umull	r2, r3, r12, r2
	add	r3, r3, lr
	adds	r10, r10, r2
	mov	lr, #0
	adcs	r11, r11, r3
	adc	lr, lr, #0
	lsr	r2, r1, #30
	ldr	r1, [sp, #60]
	orr	r2, r2, r1, lsl #2
	umull	r2, r3, r12, r2
	adds	r11, r11, r2
	adc	r2, r3, lr
	#  Overflow
	lsl	r2, r2, #1
	orr	r2, r2, r11, lsr #31
	mul	r2, r2, r12
	and	r11, r11, #0x7fffffff
	adds	r4, r4, r2
	adcs	r5, r5, #0
	adcs	r6, r6, #0
	adcs	r7, r7, #0
	adcs	r8, r8, #0
	adcs	r9, r9, #0
	adcs	r10, r10, #0
	adc	r11, r11, #0
	# Reduce if top bit set
	asr	r2, r11, #31
	and	r2, r2, r12
	and	r11, r11, #0x7fffffff
	adds	r4, r4, r2
	adcs	r5, r5, #0
	adcs	r6, r6, #0
	adcs	r7, r7, #0
	adcs	r8, r8, #0
	adcs	r9, r9, #0
	adcs	r10, r10, #0
	adc	r11, r11, #0
	# Store
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #8]
	str	r7, [r0, #12]
#else
	strd	r6, r7, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #24]
	str	r11, [r0, #28]
#else
	strd	r10, r11, [r0, #24]
#endif
	add	sp, sp, #0x40
	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
	.size	fe_sq2,.-fe_sq2
	.text
	.align	4
	.globl	fe_invert
	.type	fe_invert, %function
fe_invert:
	push	{r4, lr}
	sub	sp, sp, #0x88
	# Invert
	str	r0, [sp, #128]
	str	r1, [sp, #132]
	mov	r0, sp
	ldr	r1, [sp, #132]
	bl	fe_sq
	add	r0, sp, #32
	mov	r1, sp
	bl	fe_sq
	add	r0, sp, #32
	add	r1, sp, #32
	bl	fe_sq
	add	r0, sp, #32
	ldr	r1, [sp, #132]
	add	r2, sp, #32
	bl	fe_mul
	mov	r0, sp
	mov	r1, sp
	add	r2, sp, #32
	bl	fe_mul
	add	r0, sp, #0x40
	mov	r1, sp
	bl	fe_sq
	add	r0, sp, #32
	add	r1, sp, #32
	add	r2, sp, #0x40
	bl	fe_mul
	add	r0, sp, #0x40
	add	r1, sp, #32
	bl	fe_sq
	mov	r4, #4
L_fe_invert1:
	add	r0, sp, #0x40
	add	r1, sp, #0x40
	bl	fe_sq
	sub	r4, r4, #1
	cmp	r4, #0
	bne	L_fe_invert1
	add	r0, sp, #32
	add	r1, sp, #0x40
	add	r2, sp, #32
	bl	fe_mul
	add	r0, sp, #0x40
	add	r1, sp, #32
	bl	fe_sq
	mov	r4, #9
L_fe_invert2:
	add	r0, sp, #0x40
	add	r1, sp, #0x40
	bl	fe_sq
	sub	r4, r4, #1
	cmp	r4, #0
	bne	L_fe_invert2
	add	r0, sp, #0x40
	add	r1, sp, #0x40
	add	r2, sp, #32
	bl	fe_mul
	add	r0, sp, #0x60
	add	r1, sp, #0x40
	bl	fe_sq
	mov	r4, #19
L_fe_invert3:
	add	r0, sp, #0x60
	add	r1, sp, #0x60
	bl	fe_sq
	sub	r4, r4, #1
	cmp	r4, #0
	bne	L_fe_invert3
	add	r0, sp, #0x40
	add	r1, sp, #0x60
	add	r2, sp, #0x40
	bl	fe_mul
	mov	r4, #10
L_fe_invert4:
	add	r0, sp, #0x40
	add	r1, sp, #0x40
	bl	fe_sq
	sub	r4, r4, #1
	cmp	r4, #0
	bne	L_fe_invert4
	add	r0, sp, #32
	add	r1, sp, #0x40
	add	r2, sp, #32
	bl	fe_mul
	add	r0, sp, #0x40
	add	r1, sp, #32
	bl	fe_sq
	mov	r4, #49
L_fe_invert5:
	add	r0, sp, #0x40
	add	r1, sp, #0x40
	bl	fe_sq
	sub	r4, r4, #1
	cmp	r4, #0
	bne	L_fe_invert5
	add	r0, sp, #0x40
	add	r1, sp, #0x40
	add	r2, sp, #32
	bl	fe_mul
	add	r0, sp, #0x60
	add	r1, sp, #0x40
	bl	fe_sq
	mov	r4, #0x63
L_fe_invert6:
	add	r0, sp, #0x60
	add	r1, sp, #0x60
	bl	fe_sq
	sub	r4, r4, #1
	cmp	r4, #0
	bne	L_fe_invert6
	add	r0, sp, #0x40
	add	r1, sp, #0x60
	add	r2, sp, #0x40
	bl	fe_mul
	mov	r4, #50
L_fe_invert7:
	add	r0, sp, #0x40
	add	r1, sp, #0x40
	bl	fe_sq
	sub	r4, r4, #1
	cmp	r4, #0
	bne	L_fe_invert7
	add	r0, sp, #32
	add	r1, sp, #0x40
	add	r2, sp, #32
	bl	fe_mul
	mov	r4, #5
L_fe_invert8:
	add	r0, sp, #32
	add	r1, sp, #32
	bl	fe_sq
	sub	r4, r4, #1
	cmp	r4, #0
	bne	L_fe_invert8
	ldr	r0, [sp, #128]
	add	r1, sp, #32
	mov	r2, sp
	bl	fe_mul
	ldr	r1, [sp, #132]
	ldr	r0, [sp, #128]
	add	sp, sp, #0x88
	pop	{r4, pc}
	.size	fe_invert,.-fe_invert
	.text
	.align	4
	.globl	curve25519
	.type	curve25519, %function
curve25519:
	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
	sub	sp, sp, #0xbc
	str	r0, [sp, #160]
	str	r1, [sp, #164]
	str	r2, [sp, #168]
	mov	r1, #0
	str	r1, [sp, #172]
	# Set one
	mov	r10, #1
	mov	r11, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0]
	str	r11, [r0, #4]
#else
	strd	r10, r11, [r0]
#endif
	mov	r10, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #8]
	str	r11, [r0, #12]
#else
	strd	r10, r11, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #16]
	str	r11, [r0, #20]
#else
	strd	r10, r11, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #24]
	str	r11, [r0, #28]
#else
	strd	r10, r11, [r0, #24]
#endif
	# Set zero
	mov	r10, #0
	mov	r11, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [sp]
	str	r11, [sp, #4]
#else
	strd	r10, r11, [sp]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [sp, #8]
	str	r11, [sp, #12]
#else
	strd	r10, r11, [sp, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [sp, #16]
	str	r11, [sp, #20]
#else
	strd	r10, r11, [sp, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [sp, #24]
	str	r11, [sp, #28]
#else
	strd	r10, r11, [sp, #24]
#endif
	# Set one
	mov	r10, #1
	mov	r11, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [sp, #32]
	str	r11, [sp, #36]
#else
	strd	r10, r11, [sp, #32]
#endif
	mov	r10, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [sp, #40]
	str	r11, [sp, #44]
#else
	strd	r10, r11, [sp, #40]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [sp, #48]
	str	r11, [sp, #52]
#else
	strd	r10, r11, [sp, #48]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [sp, #56]
	str	r11, [sp, #60]
#else
	strd	r10, r11, [sp, #56]
#endif
	# Copy
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r2]
	ldr	r5, [r2, #4]
#else
	ldrd	r4, r5, [r2]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r2, #8]
	ldr	r7, [r2, #12]
#else
	ldrd	r6, r7, [r2, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [sp, #64]
	str	r5, [sp, #68]
#else
	strd	r4, r5, [sp, #64]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [sp, #72]
	str	r7, [sp, #76]
#else
	strd	r6, r7, [sp, #72]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r2, #16]
	ldr	r5, [r2, #20]
#else
	ldrd	r4, r5, [r2, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r2, #24]
	ldr	r7, [r2, #28]
#else
	ldrd	r6, r7, [r2, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [sp, #80]
	str	r5, [sp, #84]
#else
	strd	r4, r5, [sp, #80]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [sp, #88]
	str	r7, [sp, #92]
#else
	strd	r6, r7, [sp, #88]
#endif
	mov	r1, #30
	str	r1, [sp, #180]
	mov	r2, #28
	str	r2, [sp, #176]
L_curve25519_words:
L_curve25519_bits:
	ldr	r1, [sp, #164]
	ldr	r2, [r1, r2]
	ldr	r1, [sp, #180]
	lsr	r2, r2, r1
	and	r2, r2, #1
	str	r2, [sp, #184]
	ldr	r1, [sp, #172]
	eor	r1, r1, r2
	str	r1, [sp, #172]
	ldr	r0, [sp, #160]
	# Conditional Swap
	neg	r1, r1
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp, #64]
	ldr	r7, [sp, #68]
#else
	ldrd	r6, r7, [sp, #64]
#endif
	eor	r8, r4, r6
	eor	r9, r5, r7
	and	r8, r8, r1
	and	r9, r9, r1
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r8
	eor	r7, r7, r9
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [sp, #64]
	str	r7, [sp, #68]
#else
	strd	r6, r7, [sp, #64]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #8]
	ldr	r5, [r0, #12]
#else
	ldrd	r4, r5, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp, #72]
	ldr	r7, [sp, #76]
#else
	ldrd	r6, r7, [sp, #72]
#endif
	eor	r8, r4, r6
	eor	r9, r5, r7
	and	r8, r8, r1
	and	r9, r9, r1
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r8
	eor	r7, r7, r9
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #8]
	str	r5, [r0, #12]
#else
	strd	r4, r5, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [sp, #72]
	str	r7, [sp, #76]
#else
	strd	r6, r7, [sp, #72]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #16]
	ldr	r5, [r0, #20]
#else
	ldrd	r4, r5, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp, #80]
	ldr	r7, [sp, #84]
#else
	ldrd	r6, r7, [sp, #80]
#endif
	eor	r8, r4, r6
	eor	r9, r5, r7
	and	r8, r8, r1
	and	r9, r9, r1
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r8
	eor	r7, r7, r9
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #16]
	str	r5, [r0, #20]
#else
	strd	r4, r5, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [sp, #80]
	str	r7, [sp, #84]
#else
	strd	r6, r7, [sp, #80]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #24]
	ldr	r5, [r0, #28]
#else
	ldrd	r4, r5, [r0, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp, #88]
	ldr	r7, [sp, #92]
#else
	ldrd	r6, r7, [sp, #88]
#endif
	eor	r8, r4, r6
	eor	r9, r5, r7
	and	r8, r8, r1
	and	r9, r9, r1
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r8
	eor	r7, r7, r9
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #24]
	str	r5, [r0, #28]
#else
	strd	r4, r5, [r0, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [sp, #88]
	str	r7, [sp, #92]
#else
	strd	r6, r7, [sp, #88]
#endif
	ldr	r1, [sp, #172]
	# Conditional Swap
	neg	r1, r1
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp]
	ldr	r5, [sp, #4]
#else
	ldrd	r4, r5, [sp]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp, #32]
	ldr	r7, [sp, #36]
#else
	ldrd	r6, r7, [sp, #32]
#endif
	eor	r8, r4, r6
	eor	r9, r5, r7
	and	r8, r8, r1
	and	r9, r9, r1
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r8
	eor	r7, r7, r9
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [sp]
	str	r5, [sp, #4]
#else
	strd	r4, r5, [sp]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [sp, #32]
	str	r7, [sp, #36]
#else
	strd	r6, r7, [sp, #32]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #8]
	ldr	r5, [sp, #12]
#else
	ldrd	r4, r5, [sp, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp, #40]
	ldr	r7, [sp, #44]
#else
	ldrd	r6, r7, [sp, #40]
#endif
	eor	r8, r4, r6
	eor	r9, r5, r7
	and	r8, r8, r1
	and	r9, r9, r1
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r8
	eor	r7, r7, r9
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [sp, #8]
	str	r5, [sp, #12]
#else
	strd	r4, r5, [sp, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [sp, #40]
	str	r7, [sp, #44]
#else
	strd	r6, r7, [sp, #40]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #16]
	ldr	r5, [sp, #20]
#else
	ldrd	r4, r5, [sp, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp, #48]
	ldr	r7, [sp, #52]
#else
	ldrd	r6, r7, [sp, #48]
#endif
	eor	r8, r4, r6
	eor	r9, r5, r7
	and	r8, r8, r1
	and	r9, r9, r1
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r8
	eor	r7, r7, r9
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [sp, #16]
	str	r5, [sp, #20]
#else
	strd	r4, r5, [sp, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [sp, #48]
	str	r7, [sp, #52]
#else
	strd	r6, r7, [sp, #48]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #24]
	ldr	r5, [sp, #28]
#else
	ldrd	r4, r5, [sp, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp, #56]
	ldr	r7, [sp, #60]
#else
	ldrd	r6, r7, [sp, #56]
#endif
	eor	r8, r4, r6
	eor	r9, r5, r7
	and	r8, r8, r1
	and	r9, r9, r1
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r8
	eor	r7, r7, r9
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [sp, #24]
	str	r5, [sp, #28]
#else
	strd	r4, r5, [sp, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [sp, #56]
	str	r7, [sp, #60]
#else
	strd	r6, r7, [sp, #56]
#endif
	ldr	r1, [sp, #184]
	str	r1, [sp, #172]
	# Add-Sub
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp]
	ldr	r7, [sp, #4]
#else
	ldrd	r6, r7, [sp]
#endif
	adds	r8, r4, r6
	mov	r3, #0
	adcs	r9, r5, r7
	adc	r3, r3, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0]
	str	r9, [r0, #4]
#else
	strd	r8, r9, [r0]
#endif
	#  Sub
	subs	r10, r4, r6
	mov	r12, #0
	sbcs	r11, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [sp, #128]
	str	r11, [sp, #132]
#else
	strd	r10, r11, [sp, #128]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #8]
	ldr	r5, [r0, #12]
#else
	ldrd	r4, r5, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp, #8]
	ldr	r7, [sp, #12]
#else
	ldrd	r6, r7, [sp, #8]
#endif
	adds	r3, r3, #-1
	adcs	r8, r4, r6
	mov	r3, #0
	adcs	r9, r5, r7
	adc	r3, r3, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #8]
	str	r9, [r0, #12]
#else
	strd	r8, r9, [r0, #8]
#endif
	#  Sub
	adds	r12, r12, #-1
	sbcs	r10, r4, r6
	mov	r12, #0
	sbcs	r11, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [sp, #136]
	str	r11, [sp, #140]
#else
	strd	r10, r11, [sp, #136]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #16]
	ldr	r5, [r0, #20]
#else
	ldrd	r4, r5, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp, #16]
	ldr	r7, [sp, #20]
#else
	ldrd	r6, r7, [sp, #16]
#endif
	adds	r3, r3, #-1
	adcs	r8, r4, r6
	mov	r3, #0
	adcs	r9, r5, r7
	adc	r3, r3, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
	#  Sub
	adds	r12, r12, #-1
	sbcs	r10, r4, r6
	mov	r12, #0
	sbcs	r11, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [sp, #144]
	str	r11, [sp, #148]
#else
	strd	r10, r11, [sp, #144]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #24]
	ldr	r5, [r0, #28]
#else
	ldrd	r4, r5, [r0, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp, #24]
	ldr	r7, [sp, #28]
#else
	ldrd	r6, r7, [sp, #24]
#endif
	adds	r3, r3, #-1
	adcs	r8, r4, r6
	adc	r9, r5, r7
	#  Sub
	adds	r12, r12, #-1
	sbcs	r10, r4, r6
	sbc	r11, r5, r7
	mov	r3, #-19
	asr	r2, r9, #31
	#   Mask the modulus
	and	r3, r2, r3
	and	r12, r2, #0x7fffffff
	#   Sub modulus (if overflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
	subs	r4, r4, r3
	sbcs	r5, r5, r2
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #8]
	ldr	r5, [r0, #12]
#else
	ldrd	r4, r5, [r0, #8]
#endif
	sbcs	r4, r4, r2
	sbcs	r5, r5, r2
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #8]
	str	r5, [r0, #12]
#else
	strd	r4, r5, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #16]
	ldr	r5, [r0, #20]
#else
	ldrd	r4, r5, [r0, #16]
#endif
	sbcs	r4, r4, r2
	sbcs	r5, r5, r2
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #16]
	str	r5, [r0, #20]
#else
	strd	r4, r5, [r0, #16]
#endif
	sbcs	r8, r8, r2
	sbc	r9, r9, r12
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #24]
	str	r9, [r0, #28]
#else
	strd	r8, r9, [r0, #24]
#endif
	mov	r3, #-19
	asr	r2, r11, #31
	#   Mask the modulus
	and	r3, r2, r3
	and	r12, r2, #0x7fffffff
	#   Add modulus (if underflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #128]
	ldr	r5, [sp, #132]
#else
	ldrd	r4, r5, [sp, #128]
#endif
	adds	r4, r4, r3
	adcs	r5, r5, r2
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [sp, #128]
	str	r5, [sp, #132]
#else
	strd	r4, r5, [sp, #128]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #136]
	ldr	r5, [sp, #140]
#else
	ldrd	r4, r5, [sp, #136]
#endif
	adcs	r4, r4, r2
	adcs	r5, r5, r2
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [sp, #136]
	str	r5, [sp, #140]
#else
	strd	r4, r5, [sp, #136]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #144]
	ldr	r5, [sp, #148]
#else
	ldrd	r4, r5, [sp, #144]
#endif
	adcs	r4, r4, r2
	adcs	r5, r5, r2
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [sp, #144]
	str	r5, [sp, #148]
#else
	strd	r4, r5, [sp, #144]
#endif
	adcs	r10, r10, r2
	adc	r11, r11, r12
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [sp, #152]
	str	r11, [sp, #156]
#else
	strd	r10, r11, [sp, #152]
#endif
	# Add-Sub
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #64]
	ldr	r5, [sp, #68]
#else
	ldrd	r4, r5, [sp, #64]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp, #32]
	ldr	r7, [sp, #36]
#else
	ldrd	r6, r7, [sp, #32]
#endif
	adds	r8, r4, r6
	mov	r3, #0
	adcs	r9, r5, r7
	adc	r3, r3, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [sp]
	str	r9, [sp, #4]
#else
	strd	r8, r9, [sp]
#endif
	#  Sub
	subs	r10, r4, r6
	mov	r12, #0
	sbcs	r11, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [sp, #96]
	str	r11, [sp, #100]
#else
	strd	r10, r11, [sp, #96]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #72]
	ldr	r5, [sp, #76]
#else
	ldrd	r4, r5, [sp, #72]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp, #40]
	ldr	r7, [sp, #44]
#else
	ldrd	r6, r7, [sp, #40]
#endif
	adds	r3, r3, #-1
	adcs	r8, r4, r6
	mov	r3, #0
	adcs	r9, r5, r7
	adc	r3, r3, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [sp, #8]
	str	r9, [sp, #12]
#else
	strd	r8, r9, [sp, #8]
#endif
	#  Sub
	adds	r12, r12, #-1
	sbcs	r10, r4, r6
	mov	r12, #0
	sbcs	r11, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [sp, #104]
	str	r11, [sp, #108]
#else
	strd	r10, r11, [sp, #104]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #80]
	ldr	r5, [sp, #84]
#else
	ldrd	r4, r5, [sp, #80]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp, #48]
	ldr	r7, [sp, #52]
#else
	ldrd	r6, r7, [sp, #48]
#endif
	adds	r3, r3, #-1
	adcs	r8, r4, r6
	mov	r3, #0
	adcs	r9, r5, r7
	adc	r3, r3, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [sp, #16]
	str	r9, [sp, #20]
#else
	strd	r8, r9, [sp, #16]
#endif
	#  Sub
	adds	r12, r12, #-1
	sbcs	r10, r4, r6
	mov	r12, #0
	sbcs	r11, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [sp, #112]
	str	r11, [sp, #116]
#else
	strd	r10, r11, [sp, #112]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #88]
	ldr	r5, [sp, #92]
#else
	ldrd	r4, r5, [sp, #88]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp, #56]
	ldr	r7, [sp, #60]
#else
	ldrd	r6, r7, [sp, #56]
#endif
	adds	r3, r3, #-1
	adcs	r8, r4, r6
	adc	r9, r5, r7
	#  Sub
	adds	r12, r12, #-1
	sbcs	r10, r4, r6
	sbc	r11, r5, r7
	mov	r3, #-19
	asr	r2, r9, #31
	#   Mask the modulus
	and	r3, r2, r3
	and	r12, r2, #0x7fffffff
	#   Sub modulus (if overflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp]
	ldr	r5, [sp, #4]
#else
	ldrd	r4, r5, [sp]
#endif
	subs	r4, r4, r3
	sbcs	r5, r5, r2
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [sp]
	str	r5, [sp, #4]
#else
	strd	r4, r5, [sp]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #8]
	ldr	r5, [sp, #12]
#else
	ldrd	r4, r5, [sp, #8]
#endif
	sbcs	r4, r4, r2
	sbcs	r5, r5, r2
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [sp, #8]
	str	r5, [sp, #12]
#else
	strd	r4, r5, [sp, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #16]
	ldr	r5, [sp, #20]
#else
	ldrd	r4, r5, [sp, #16]
#endif
	sbcs	r4, r4, r2
	sbcs	r5, r5, r2
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [sp, #16]
	str	r5, [sp, #20]
#else
	strd	r4, r5, [sp, #16]
#endif
	sbcs	r8, r8, r2
	sbc	r9, r9, r12
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [sp, #24]
	str	r9, [sp, #28]
#else
	strd	r8, r9, [sp, #24]
#endif
	mov	r3, #-19
	asr	r2, r11, #31
	#   Mask the modulus
	and	r3, r2, r3
	and	r12, r2, #0x7fffffff
	#   Add modulus (if underflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #96]
	ldr	r5, [sp, #100]
#else
	ldrd	r4, r5, [sp, #96]
#endif
	adds	r4, r4, r3
	adcs	r5, r5, r2
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [sp, #96]
	str	r5, [sp, #100]
#else
	strd	r4, r5, [sp, #96]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #104]
	ldr	r5, [sp, #108]
#else
	ldrd	r4, r5, [sp, #104]
#endif
	adcs	r4, r4, r2
	adcs	r5, r5, r2
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [sp, #104]
	str	r5, [sp, #108]
#else
	strd	r4, r5, [sp, #104]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #112]
	ldr	r5, [sp, #116]
#else
	ldrd	r4, r5, [sp, #112]
#endif
	adcs	r4, r4, r2
	adcs	r5, r5, r2
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [sp, #112]
	str	r5, [sp, #116]
#else
	strd	r4, r5, [sp, #112]
#endif
	adcs	r10, r10, r2
	adc	r11, r11, r12
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [sp, #120]
	str	r11, [sp, #124]
#else
	strd	r10, r11, [sp, #120]
#endif
	ldr	r2, [sp, #160]
	add	r1, sp, #0x60
	add	r0, sp, #32
	bl	fe_mul
	add	r2, sp, #0x80
	add	r1, sp, #0
	add	r0, sp, #0
	bl	fe_mul
	add	r1, sp, #0x80
	add	r0, sp, #0x60
	bl	fe_sq
	ldr	r1, [sp, #160]
	add	r0, sp, #0x80
	bl	fe_sq
	# Add-Sub
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #32]
	ldr	r5, [sp, #36]
#else
	ldrd	r4, r5, [sp, #32]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp]
	ldr	r7, [sp, #4]
#else
	ldrd	r6, r7, [sp]
#endif
	adds	r8, r4, r6
	mov	r3, #0
	adcs	r9, r5, r7
	adc	r3, r3, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [sp, #64]
	str	r9, [sp, #68]
#else
	strd	r8, r9, [sp, #64]
#endif
	#  Sub
	subs	r10, r4, r6
	mov	r12, #0
	sbcs	r11, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [sp]
	str	r11, [sp, #4]
#else
	strd	r10, r11, [sp]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #40]
	ldr	r5, [sp, #44]
#else
	ldrd	r4, r5, [sp, #40]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp, #8]
	ldr	r7, [sp, #12]
#else
	ldrd	r6, r7, [sp, #8]
#endif
	adds	r3, r3, #-1
	adcs	r8, r4, r6
	mov	r3, #0
	adcs	r9, r5, r7
	adc	r3, r3, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [sp, #72]
	str	r9, [sp, #76]
#else
	strd	r8, r9, [sp, #72]
#endif
	#  Sub
	adds	r12, r12, #-1
	sbcs	r10, r4, r6
	mov	r12, #0
	sbcs	r11, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [sp, #8]
	str	r11, [sp, #12]
#else
	strd	r10, r11, [sp, #8]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #48]
	ldr	r5, [sp, #52]
#else
	ldrd	r4, r5, [sp, #48]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp, #16]
	ldr	r7, [sp, #20]
#else
	ldrd	r6, r7, [sp, #16]
#endif
	adds	r3, r3, #-1
	adcs	r8, r4, r6
	mov	r3, #0
	adcs	r9, r5, r7
	adc	r3, r3, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [sp, #80]
	str	r9, [sp, #84]
#else
	strd	r8, r9, [sp, #80]
#endif
	#  Sub
	adds	r12, r12, #-1
	sbcs	r10, r4, r6
	mov	r12, #0
	sbcs	r11, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [sp, #16]
	str	r11, [sp, #20]
#else
	strd	r10, r11, [sp, #16]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #56]
	ldr	r5, [sp, #60]
#else
	ldrd	r4, r5, [sp, #56]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp, #24]
	ldr	r7, [sp, #28]
#else
	ldrd	r6, r7, [sp, #24]
#endif
	adds	r3, r3, #-1
	adcs	r8, r4, r6
	adc	r9, r5, r7
	#  Sub
	adds	r12, r12, #-1
	sbcs	r10, r4, r6
	sbc	r11, r5, r7
	mov	r3, #-19
	asr	r2, r9, #31
	#   Mask the modulus
	and	r3, r2, r3
	and	r12, r2, #0x7fffffff
	#   Sub modulus (if overflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #64]
	ldr	r5, [sp, #68]
#else
	ldrd	r4, r5, [sp, #64]
#endif
	subs	r4, r4, r3
	sbcs	r5, r5, r2
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [sp, #64]
	str	r5, [sp, #68]
#else
	strd	r4, r5, [sp, #64]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #72]
	ldr	r5, [sp, #76]
#else
	ldrd	r4, r5, [sp, #72]
#endif
	sbcs	r4, r4, r2
	sbcs	r5, r5, r2
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [sp, #72]
	str	r5, [sp, #76]
#else
	strd	r4, r5, [sp, #72]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #80]
	ldr	r5, [sp, #84]
#else
	ldrd	r4, r5, [sp, #80]
#endif
	sbcs	r4, r4, r2
	sbcs	r5, r5, r2
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [sp, #80]
	str	r5, [sp, #84]
#else
	strd	r4, r5, [sp, #80]
#endif
	sbcs	r8, r8, r2
	sbc	r9, r9, r12
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [sp, #88]
	str	r9, [sp, #92]
#else
	strd	r8, r9, [sp, #88]
#endif
	mov	r3, #-19
	asr	r2, r11, #31
	#   Mask the modulus
	and	r3, r2, r3
	and	r12, r2, #0x7fffffff
	#   Add modulus (if underflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp]
	ldr	r5, [sp, #4]
#else
	ldrd	r4, r5, [sp]
#endif
	adds	r4, r4, r3
	adcs	r5, r5, r2
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [sp]
	str	r5, [sp, #4]
#else
	strd	r4, r5, [sp]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #8]
	ldr	r5, [sp, #12]
#else
	ldrd	r4, r5, [sp, #8]
#endif
	adcs	r4, r4, r2
	adcs	r5, r5, r2
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [sp, #8]
	str	r5, [sp, #12]
#else
	strd	r4, r5, [sp, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #16]
	ldr	r5, [sp, #20]
#else
	ldrd	r4, r5, [sp, #16]
#endif
	adcs	r4, r4, r2
	adcs	r5, r5, r2
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [sp, #16]
	str	r5, [sp, #20]
#else
	strd	r4, r5, [sp, #16]
#endif
	adcs	r10, r10, r2
	adc	r11, r11, r12
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [sp, #24]
	str	r11, [sp, #28]
#else
	strd	r10, r11, [sp, #24]
#endif
	add	r2, sp, #0x60
	add	r1, sp, #0x80
	ldr	r0, [sp, #160]
	bl	fe_mul
	# Sub
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #128]
	ldr	r5, [sp, #132]
#else
	ldrd	r4, r5, [sp, #128]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp, #136]
	ldr	r7, [sp, #140]
#else
	ldrd	r6, r7, [sp, #136]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [sp, #96]
	ldr	r9, [sp, #100]
#else
	ldrd	r8, r9, [sp, #96]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [sp, #104]
	ldr	r11, [sp, #108]
#else
	ldrd	r10, r11, [sp, #104]
#endif
	subs	r8, r4, r8
	sbcs	r9, r5, r9
	sbcs	r10, r6, r10
	sbcs	r11, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [sp, #128]
	str	r9, [sp, #132]
#else
	strd	r8, r9, [sp, #128]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [sp, #136]
	str	r11, [sp, #140]
#else
	strd	r10, r11, [sp, #136]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #144]
	ldr	r5, [sp, #148]
#else
	ldrd	r4, r5, [sp, #144]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp, #152]
	ldr	r7, [sp, #156]
#else
	ldrd	r6, r7, [sp, #152]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [sp, #112]
	ldr	r9, [sp, #116]
#else
	ldrd	r8, r9, [sp, #112]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [sp, #120]
	ldr	r11, [sp, #124]
#else
	ldrd	r10, r11, [sp, #120]
#endif
	sbcs	r8, r4, r8
	sbcs	r9, r5, r9
	sbcs	r10, r6, r10
	sbc	r11, r7, r11
	mov	r3, #-19
	asr	r2, r11, #31
	#   Mask the modulus
	and	r3, r2, r3
	and	r12, r2, #0x7fffffff
	#   Add modulus (if underflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #128]
	ldr	r5, [sp, #132]
#else
	ldrd	r4, r5, [sp, #128]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp, #136]
	ldr	r7, [sp, #140]
#else
	ldrd	r6, r7, [sp, #136]
#endif
	adds	r4, r4, r3
	adcs	r5, r5, r2
	adcs	r6, r6, r2
	adcs	r7, r7, r2
	adcs	r8, r8, r2
	adcs	r9, r9, r2
	adcs	r10, r10, r2
	adc	r11, r11, r12
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [sp, #128]
	str	r5, [sp, #132]
#else
	strd	r4, r5, [sp, #128]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [sp, #136]
	str	r7, [sp, #140]
#else
	strd	r6, r7, [sp, #136]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [sp, #144]
	str	r9, [sp, #148]
#else
	strd	r8, r9, [sp, #144]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [sp, #152]
	str	r11, [sp, #156]
#else
	strd	r10, r11, [sp, #152]
#endif
	add	r1, sp, #0
	add	r0, sp, #0
	bl	fe_sq
	# Multiply by 121666
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #128]
	ldr	r5, [sp, #132]
#else
	ldrd	r4, r5, [sp, #128]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp, #136]
	ldr	r7, [sp, #140]
#else
	ldrd	r6, r7, [sp, #136]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [sp, #144]
	ldr	r9, [sp, #148]
#else
	ldrd	r8, r9, [sp, #144]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [sp, #152]
	ldr	r11, [sp, #156]
#else
	ldrd	r10, r11, [sp, #152]
#endif
	movw	r12, #0xdb42
	movt	r12, #1
	umull	r4, r2, r4, r12
	umull	r5, r3, r5, r12
	adds	r5, r5, r2
	adc	r2, r3, #0
	umull	r6, r3, r6, r12
	adds	r6, r6, r2
	adc	r2, r3, #0
	umull	r7, r3, r7, r12
	adds	r7, r7, r2
	adc	r2, r3, #0
	umull	r8, r3, r8, r12
	adds	r8, r8, r2
	adc	r2, r3, #0
	umull	r9, r3, r9, r12
	adds	r9, r9, r2
	adc	r2, r3, #0
	umull	r10, r3, r10, r12
	adds	r10, r10, r2
	adc	r2, r3, #0
	umull	r11, r3, r11, r12
	adds	r11, r11, r2
	adc	r2, r3, #0
	mov	r12, #19
	lsl	r2, r2, #1
	orr	r2, r2, r11, lsr #31
	mul	r2, r2, r12
	and	r11, r11, #0x7fffffff
	adds	r4, r4, r2
	adcs	r5, r5, #0
	adcs	r6, r6, #0
	adcs	r7, r7, #0
	adcs	r8, r8, #0
	adcs	r9, r9, #0
	adcs	r10, r10, #0
	adc	r11, r11, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [sp, #32]
	str	r5, [sp, #36]
#else
	strd	r4, r5, [sp, #32]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [sp, #40]
	str	r7, [sp, #44]
#else
	strd	r6, r7, [sp, #40]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [sp, #48]
	str	r9, [sp, #52]
#else
	strd	r8, r9, [sp, #48]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [sp, #56]
	str	r11, [sp, #60]
#else
	strd	r10, r11, [sp, #56]
#endif
	add	r1, sp, #0x40
	add	r0, sp, #0x40
	bl	fe_sq
	# Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #96]
	ldr	r5, [sp, #100]
#else
	ldrd	r4, r5, [sp, #96]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp, #104]
	ldr	r7, [sp, #108]
#else
	ldrd	r6, r7, [sp, #104]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [sp, #32]
	ldr	r9, [sp, #36]
#else
	ldrd	r8, r9, [sp, #32]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [sp, #40]
	ldr	r11, [sp, #44]
#else
	ldrd	r10, r11, [sp, #40]
#endif
	adds	r8, r4, r8
	adcs	r9, r5, r9
	adcs	r10, r6, r10
	adcs	r11, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [sp, #96]
	str	r9, [sp, #100]
#else
	strd	r8, r9, [sp, #96]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [sp, #104]
	str	r11, [sp, #108]
#else
	strd	r10, r11, [sp, #104]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #112]
	ldr	r5, [sp, #116]
#else
	ldrd	r4, r5, [sp, #112]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp, #120]
	ldr	r7, [sp, #124]
#else
	ldrd	r6, r7, [sp, #120]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [sp, #48]
	ldr	r9, [sp, #52]
#else
	ldrd	r8, r9, [sp, #48]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [sp, #56]
	ldr	r11, [sp, #60]
#else
	ldrd	r10, r11, [sp, #56]
#endif
	adcs	r8, r4, r8
	adcs	r9, r5, r9
	adcs	r10, r6, r10
	adc	r11, r7, r11
	mov	r3, #-19
	asr	r2, r11, #31
	#   Mask the modulus
	and	r3, r2, r3
	and	r12, r2, #0x7fffffff
	#   Sub modulus (if overflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [sp, #96]
	ldr	r5, [sp, #100]
#else
	ldrd	r4, r5, [sp, #96]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [sp, #104]
	ldr	r7, [sp, #108]
#else
	ldrd	r6, r7, [sp, #104]
#endif
	subs	r4, r4, r3
	sbcs	r5, r5, r2
	sbcs	r6, r6, r2
	sbcs	r7, r7, r2
	sbcs	r8, r8, r2
	sbcs	r9, r9, r2
	sbcs	r10, r10, r2
	sbc	r11, r11, r12
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [sp, #96]
	str	r5, [sp, #100]
#else
	strd	r4, r5, [sp, #96]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [sp, #104]
	str	r7, [sp, #108]
#else
	strd	r6, r7, [sp, #104]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [sp, #112]
	str	r9, [sp, #116]
#else
	strd	r8, r9, [sp, #112]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [sp, #120]
	str	r11, [sp, #124]
#else
	strd	r10, r11, [sp, #120]
#endif
	add	r2, sp, #0
	ldr	r1, [sp, #168]
	add	r0, sp, #32
	bl	fe_mul
	add	r2, sp, #0x60
	add	r1, sp, #0x80
	add	r0, sp, #0
	bl	fe_mul
	ldr	r2, [sp, #176]
	ldr	r1, [sp, #180]
	subs	r1, r1, #1
	str	r1, [sp, #180]
	bge	L_curve25519_bits
	mov	r1, #31
	str	r1, [sp, #180]
	subs	r2, r2, #4
	str	r2, [sp, #176]
	bge	L_curve25519_words
	# Invert
	add	r0, sp, #32
	add	r1, sp, #0
	bl	fe_sq
	add	r0, sp, #0x40
	add	r1, sp, #32
	bl	fe_sq
	add	r0, sp, #0x40
	add	r1, sp, #0x40
	bl	fe_sq
	add	r0, sp, #0x40
	add	r1, sp, #0
	add	r2, sp, #0x40
	bl	fe_mul
	add	r0, sp, #32
	add	r1, sp, #32
	add	r2, sp, #0x40
	bl	fe_mul
	add	r0, sp, #0x60
	add	r1, sp, #32
	bl	fe_sq
	add	r0, sp, #0x40
	add	r1, sp, #0x40
	add	r2, sp, #0x60
	bl	fe_mul
	add	r0, sp, #0x60
	add	r1, sp, #0x40
	bl	fe_sq
	mov	r4, #4
L_curve25519_inv_1:
	add	r0, sp, #0x60
	add	r1, sp, #0x60
	bl	fe_sq
	sub	r4, r4, #1
	cmp	r4, #0
	bne	L_curve25519_inv_1
	add	r0, sp, #0x40
	add	r1, sp, #0x60
	add	r2, sp, #0x40
	bl	fe_mul
	add	r0, sp, #0x60
	add	r1, sp, #0x40
	bl	fe_sq
	mov	r4, #9
L_curve25519_inv_2:
	add	r0, sp, #0x60
	add	r1, sp, #0x60
	bl	fe_sq
	sub	r4, r4, #1
	cmp	r4, #0
	bne	L_curve25519_inv_2
	add	r0, sp, #0x60
	add	r1, sp, #0x60
	add	r2, sp, #0x40
	bl	fe_mul
	add	r0, sp, #0x80
	add	r1, sp, #0x60
	bl	fe_sq
	mov	r4, #19
L_curve25519_inv_3:
	add	r0, sp, #0x80
	add	r1, sp, #0x80
	bl	fe_sq
	sub	r4, r4, #1
	cmp	r4, #0
	bne	L_curve25519_inv_3
	add	r0, sp, #0x60
	add	r1, sp, #0x80
	add	r2, sp, #0x60
	bl	fe_mul
	mov	r4, #10
L_curve25519_inv_4:
	add	r0, sp, #0x60
	add	r1, sp, #0x60
	bl	fe_sq
	sub	r4, r4, #1
	cmp	r4, #0
	bne	L_curve25519_inv_4
	add	r0, sp, #0x40
	add	r1, sp, #0x60
	add	r2, sp, #0x40
	bl	fe_mul
	add	r0, sp, #0x60
	add	r1, sp, #0x40
	bl	fe_sq
	mov	r4, #49
L_curve25519_inv_5:
	add	r0, sp, #0x60
	add	r1, sp, #0x60
	bl	fe_sq
	sub	r4, r4, #1
	cmp	r4, #0
	bne	L_curve25519_inv_5
	add	r0, sp, #0x60
	add	r1, sp, #0x60
	add	r2, sp, #0x40
	bl	fe_mul
	add	r0, sp, #0x80
	add	r1, sp, #0x60
	bl	fe_sq
	mov	r4, #0x63
L_curve25519_inv_6:
	add	r0, sp, #0x80
	add	r1, sp, #0x80
	bl	fe_sq
	sub	r4, r4, #1
	cmp	r4, #0
	bne	L_curve25519_inv_6
	add	r0, sp, #0x60
	add	r1, sp, #0x80
	add	r2, sp, #0x60
	bl	fe_mul
	mov	r4, #50
L_curve25519_inv_7:
	add	r0, sp, #0x60
	add	r1, sp, #0x60
	bl	fe_sq
	sub	r4, r4, #1
	cmp	r4, #0
	bne	L_curve25519_inv_7
	add	r0, sp, #0x40
	add	r1, sp, #0x60
	add	r2, sp, #0x40
	bl	fe_mul
	mov	r4, #5
L_curve25519_inv_8:
	add	r0, sp, #0x40
	add	r1, sp, #0x40
	bl	fe_sq
	sub	r4, r4, #1
	cmp	r4, #0
	bne	L_curve25519_inv_8
	add	r0, sp, #0
	add	r1, sp, #0x40
	add	r2, sp, #32
	bl	fe_mul
	add	r2, sp, #0
	ldr	r1, [sp, #160]
	ldr	r0, [sp, #160]
	bl	fe_mul
	mov	r0, #0
	add	sp, sp, #0xbc
	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
	.size	curve25519,.-curve25519
	.text
	.align	4
	.globl	fe_pow22523
	.type	fe_pow22523, %function
fe_pow22523:
	push	{r4, lr}
	sub	sp, sp, #0x68
	# pow22523
	str	r0, [sp, #96]
	str	r1, [sp, #100]
	mov	r0, sp
	ldr	r1, [sp, #100]
	bl	fe_sq
	add	r0, sp, #32
	mov	r1, sp
	bl	fe_sq
	add	r0, sp, #32
	add	r1, sp, #32
	bl	fe_sq
	add	r0, sp, #32
	ldr	r1, [sp, #100]
	add	r2, sp, #32
	bl	fe_mul
	mov	r0, sp
	mov	r1, sp
	add	r2, sp, #32
	bl	fe_mul
	mov	r0, sp
	mov	r1, sp
	bl	fe_sq
	mov	r0, sp
	add	r1, sp, #32
	mov	r2, sp
	bl	fe_mul
	add	r0, sp, #32
	mov	r1, sp
	bl	fe_sq
	mov	r4, #4
L_fe_pow22523_1:
	add	r0, sp, #32
	add	r1, sp, #32
	bl	fe_sq
	sub	r4, r4, #1
	cmp	r4, #0
	bne	L_fe_pow22523_1
	mov	r0, sp
	add	r1, sp, #32
	mov	r2, sp
	bl	fe_mul
	add	r0, sp, #32
	mov	r1, sp
	bl	fe_sq
	mov	r4, #9
L_fe_pow22523_2:
	add	r0, sp, #32
	add	r1, sp, #32
	bl	fe_sq
	sub	r4, r4, #1
	cmp	r4, #0
	bne	L_fe_pow22523_2
	add	r0, sp, #32
	add	r1, sp, #32
	mov	r2, sp
	bl	fe_mul
	add	r0, sp, #0x40
	add	r1, sp, #32
	bl	fe_sq
	mov	r4, #19
L_fe_pow22523_3:
	add	r0, sp, #0x40
	add	r1, sp, #0x40
	bl	fe_sq
	sub	r4, r4, #1
	cmp	r4, #0
	bne	L_fe_pow22523_3
	add	r0, sp, #32
	add	r1, sp, #0x40
	add	r2, sp, #32
	bl	fe_mul
	mov	r4, #10
L_fe_pow22523_4:
	add	r0, sp, #32
	add	r1, sp, #32
	bl	fe_sq
	sub	r4, r4, #1
	cmp	r4, #0
	bne	L_fe_pow22523_4
	mov	r0, sp
	add	r1, sp, #32
	mov	r2, sp
	bl	fe_mul
	add	r0, sp, #32
	mov	r1, sp
	bl	fe_sq
	mov	r4, #49
L_fe_pow22523_5:
	add	r0, sp, #32
	add	r1, sp, #32
	bl	fe_sq
	sub	r4, r4, #1
	cmp	r4, #0
	bne	L_fe_pow22523_5
	add	r0, sp, #32
	add	r1, sp, #32
	mov	r2, sp
	bl	fe_mul
	add	r0, sp, #0x40
	add	r1, sp, #32
	bl	fe_sq
	mov	r4, #0x63
L_fe_pow22523_6:
	add	r0, sp, #0x40
	add	r1, sp, #0x40
	bl	fe_sq
	sub	r4, r4, #1
	cmp	r4, #0
	bne	L_fe_pow22523_6
	add	r0, sp, #32
	add	r1, sp, #0x40
	add	r2, sp, #32
	bl	fe_mul
	mov	r4, #50
L_fe_pow22523_7:
	add	r0, sp, #32
	add	r1, sp, #32
	bl	fe_sq
	sub	r4, r4, #1
	cmp	r4, #0
	bne	L_fe_pow22523_7
	mov	r0, sp
	add	r1, sp, #32
	mov	r2, sp
	bl	fe_mul
	mov	r4, #2
L_fe_pow22523_8:
	mov	r0, sp
	mov	r1, sp
	bl	fe_sq
	sub	r4, r4, #1
	cmp	r4, #0
	bne	L_fe_pow22523_8
	ldr	r0, [sp, #96]
	mov	r1, sp
	ldr	r2, [sp, #100]
	bl	fe_mul
	ldr	r1, [sp, #100]
	ldr	r0, [sp, #96]
	add	sp, sp, #0x68
	pop	{r4, pc}
	.size	fe_pow22523,.-fe_pow22523
	.text
	.align	4
	.globl	fe_ge_to_p2
	.type	fe_ge_to_p2, %function
fe_ge_to_p2:
	push	{lr}
	sub	sp, sp, #16
	str	r0, [sp]
	str	r1, [sp, #4]
	str	r2, [sp, #8]
	str	r3, [sp, #12]
	ldr	r2, [sp, #28]
	ldr	r1, [sp, #12]
	ldr	r0, [sp]
	bl	fe_mul
	ldr	r2, [sp, #24]
	ldr	r1, [sp, #20]
	ldr	r0, [sp, #4]
	bl	fe_mul
	ldr	r2, [sp, #28]
	ldr	r1, [sp, #24]
	ldr	r0, [sp, #8]
	bl	fe_mul
	add	sp, sp, #16
	pop	{pc}
	.size	fe_ge_to_p2,.-fe_ge_to_p2
	.text
	.align	4
	.globl	fe_ge_to_p3
	.type	fe_ge_to_p3, %function
fe_ge_to_p3:
	push	{lr}
	sub	sp, sp, #16
	str	r0, [sp]
	str	r1, [sp, #4]
	str	r2, [sp, #8]
	str	r3, [sp, #12]
	ldr	r2, [sp, #32]
	ldr	r1, [sp, #20]
	ldr	r0, [sp]
	bl	fe_mul
	ldr	r2, [sp, #28]
	ldr	r1, [sp, #24]
	ldr	r0, [sp, #4]
	bl	fe_mul
	ldr	r2, [sp, #32]
	ldr	r1, [sp, #28]
	ldr	r0, [sp, #8]
	bl	fe_mul
	ldr	r2, [sp, #24]
	ldr	r1, [sp, #20]
	ldr	r0, [sp, #12]
	bl	fe_mul
	add	sp, sp, #16
	pop	{pc}
	.size	fe_ge_to_p3,.-fe_ge_to_p3
	.text
	.align	4
	.globl	fe_ge_dbl
	.type	fe_ge_dbl, %function
fe_ge_dbl:
	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
	sub	sp, sp, #16
	str	r0, [sp]
	str	r1, [sp, #4]
	str	r2, [sp, #8]
	str	r3, [sp, #12]
	ldr	r1, [sp, #52]
	ldr	r0, [sp]
	bl	fe_sq
	ldr	r1, [sp, #56]
	ldr	r0, [sp, #8]
	bl	fe_sq
	ldr	r0, [sp, #4]
	ldr	r1, [sp, #52]
	ldr	r2, [sp, #56]
	# Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1]
	ldr	r5, [r1, #4]
#else
	ldrd	r4, r5, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #8]
	ldr	r7, [r1, #12]
#else
	ldrd	r6, r7, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r2]
	ldr	r9, [r2, #4]
#else
	ldrd	r8, r9, [r2]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r2, #8]
	ldr	r11, [r2, #12]
#else
	ldrd	r10, r11, [r2, #8]
#endif
	adds	r8, r4, r8
	adcs	r9, r5, r9
	adcs	r10, r6, r10
	adcs	r11, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0]
	str	r9, [r0, #4]
#else
	strd	r8, r9, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #8]
	str	r11, [r0, #12]
#else
	strd	r10, r11, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #16]
	ldr	r5, [r1, #20]
#else
	ldrd	r4, r5, [r1, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #24]
	ldr	r7, [r1, #28]
#else
	ldrd	r6, r7, [r1, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r2, #16]
	ldr	r9, [r2, #20]
#else
	ldrd	r8, r9, [r2, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r2, #24]
	ldr	r11, [r2, #28]
#else
	ldrd	r10, r11, [r2, #24]
#endif
	adcs	r8, r4, r8
	adcs	r9, r5, r9
	adcs	r10, r6, r10
	adc	r11, r7, r11
	mov	r12, #-19
	asr	r3, r11, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Sub modulus (if overflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
#else
	ldrd	r6, r7, [r0, #8]
#endif
	subs	r4, r4, r12
	sbcs	r5, r5, r3
	sbcs	r6, r6, r3
	sbcs	r7, r7, r3
	sbcs	r8, r8, r3
	sbcs	r9, r9, r3
	sbcs	r10, r10, r3
	sbc	r11, r11, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #8]
	str	r7, [r0, #12]
#else
	strd	r6, r7, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #24]
	str	r11, [r0, #28]
#else
	strd	r10, r11, [r0, #24]
#endif
	ldr	r1, [sp, #4]
	ldr	r0, [sp, #12]
	bl	fe_sq
	ldr	r0, [sp, #4]
	ldr	r1, [sp, #8]
	ldr	r2, [sp]
	# Add-Sub
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1]
	ldr	r5, [r1, #4]
#else
	ldrd	r4, r5, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r2]
	ldr	r7, [r2, #4]
#else
	ldrd	r6, r7, [r2]
#endif
	adds	r8, r4, r6
	mov	r12, #0
	adcs	r9, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0]
	str	r9, [r0, #4]
#else
	strd	r8, r9, [r0]
#endif
	#  Sub
	subs	r10, r4, r6
	mov	lr, #0
	sbcs	r11, r5, r7
	adc	lr, lr, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1]
	str	r11, [r1, #4]
#else
	strd	r10, r11, [r1]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #8]
	ldr	r5, [r1, #12]
#else
	ldrd	r4, r5, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r2, #8]
	ldr	r7, [r2, #12]
#else
	ldrd	r6, r7, [r2, #8]
#endif
	adds	r12, r12, #-1
	adcs	r8, r4, r6
	mov	r12, #0
	adcs	r9, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #8]
	str	r9, [r0, #12]
#else
	strd	r8, r9, [r0, #8]
#endif
	#  Sub
	adds	lr, lr, #-1
	sbcs	r10, r4, r6
	mov	lr, #0
	sbcs	r11, r5, r7
	adc	lr, lr, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1, #8]
	str	r11, [r1, #12]
#else
	strd	r10, r11, [r1, #8]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #16]
	ldr	r5, [r1, #20]
#else
	ldrd	r4, r5, [r1, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r2, #16]
	ldr	r7, [r2, #20]
#else
	ldrd	r6, r7, [r2, #16]
#endif
	adds	r12, r12, #-1
	adcs	r8, r4, r6
	mov	r12, #0
	adcs	r9, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
	#  Sub
	adds	lr, lr, #-1
	sbcs	r10, r4, r6
	mov	lr, #0
	sbcs	r11, r5, r7
	adc	lr, lr, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1, #16]
	str	r11, [r1, #20]
#else
	strd	r10, r11, [r1, #16]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #24]
	ldr	r5, [r1, #28]
#else
	ldrd	r4, r5, [r1, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r2, #24]
	ldr	r7, [r2, #28]
#else
	ldrd	r6, r7, [r2, #24]
#endif
	adds	r12, r12, #-1
	adcs	r8, r4, r6
	adc	r9, r5, r7
	#  Sub
	adds	lr, lr, #-1
	sbcs	r10, r4, r6
	sbc	r11, r5, r7
	mov	r12, #-19
	asr	r3, r9, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Sub modulus (if overflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
	subs	r4, r4, r12
	sbcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #8]
	ldr	r5, [r0, #12]
#else
	ldrd	r4, r5, [r0, #8]
#endif
	sbcs	r4, r4, r3
	sbcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #8]
	str	r5, [r0, #12]
#else
	strd	r4, r5, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #16]
	ldr	r5, [r0, #20]
#else
	ldrd	r4, r5, [r0, #16]
#endif
	sbcs	r4, r4, r3
	sbcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #16]
	str	r5, [r0, #20]
#else
	strd	r4, r5, [r0, #16]
#endif
	sbcs	r8, r8, r3
	sbc	r9, r9, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #24]
	str	r9, [r0, #28]
#else
	strd	r8, r9, [r0, #24]
#endif
	mov	r12, #-19
	asr	r3, r11, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Add modulus (if underflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1]
	ldr	r5, [r1, #4]
#else
	ldrd	r4, r5, [r1]
#endif
	adds	r4, r4, r12
	adcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r1]
	str	r5, [r1, #4]
#else
	strd	r4, r5, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #8]
	ldr	r5, [r1, #12]
#else
	ldrd	r4, r5, [r1, #8]
#endif
	adcs	r4, r4, r3
	adcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r1, #8]
	str	r5, [r1, #12]
#else
	strd	r4, r5, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #16]
	ldr	r5, [r1, #20]
#else
	ldrd	r4, r5, [r1, #16]
#endif
	adcs	r4, r4, r3
	adcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r1, #16]
	str	r5, [r1, #20]
#else
	strd	r4, r5, [r1, #16]
#endif
	adcs	r10, r10, r3
	adc	r11, r11, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1, #24]
	str	r11, [r1, #28]
#else
	strd	r10, r11, [r1, #24]
#endif
	ldr	r0, [sp]
	ldr	r1, [sp, #12]
	ldr	r2, [sp, #4]
	# Sub
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1]
	ldr	r5, [r1, #4]
#else
	ldrd	r4, r5, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #8]
	ldr	r7, [r1, #12]
#else
	ldrd	r6, r7, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r2]
	ldr	r9, [r2, #4]
#else
	ldrd	r8, r9, [r2]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r2, #8]
	ldr	r11, [r2, #12]
#else
	ldrd	r10, r11, [r2, #8]
#endif
	subs	r8, r4, r8
	sbcs	r9, r5, r9
	sbcs	r10, r6, r10
	sbcs	r11, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0]
	str	r9, [r0, #4]
#else
	strd	r8, r9, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #8]
	str	r11, [r0, #12]
#else
	strd	r10, r11, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #16]
	ldr	r5, [r1, #20]
#else
	ldrd	r4, r5, [r1, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #24]
	ldr	r7, [r1, #28]
#else
	ldrd	r6, r7, [r1, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r2, #16]
	ldr	r9, [r2, #20]
#else
	ldrd	r8, r9, [r2, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r2, #24]
	ldr	r11, [r2, #28]
#else
	ldrd	r10, r11, [r2, #24]
#endif
	sbcs	r8, r4, r8
	sbcs	r9, r5, r9
	sbcs	r10, r6, r10
	sbc	r11, r7, r11
	mov	r12, #-19
	asr	r3, r11, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Add modulus (if underflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
#else
	ldrd	r6, r7, [r0, #8]
#endif
	adds	r4, r4, r12
	adcs	r5, r5, r3
	adcs	r6, r6, r3
	adcs	r7, r7, r3
	adcs	r8, r8, r3
	adcs	r9, r9, r3
	adcs	r10, r10, r3
	adc	r11, r11, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #8]
	str	r7, [r0, #12]
#else
	strd	r6, r7, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #24]
	str	r11, [r0, #28]
#else
	strd	r10, r11, [r0, #24]
#endif
	ldr	r1, [sp, #60]
	ldr	r0, [sp, #12]
	bl	fe_sq2
	ldr	r0, [sp, #12]
	ldr	r1, [sp, #8]
	# Sub
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
#else
	ldrd	r6, r7, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r1]
	ldr	r9, [r1, #4]
#else
	ldrd	r8, r9, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #8]
	ldr	r11, [r1, #12]
#else
	ldrd	r10, r11, [r1, #8]
#endif
	subs	r8, r4, r8
	sbcs	r9, r5, r9
	sbcs	r10, r6, r10
	sbcs	r11, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0]
	str	r9, [r0, #4]
#else
	strd	r8, r9, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #8]
	str	r11, [r0, #12]
#else
	strd	r10, r11, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #16]
	ldr	r5, [r0, #20]
#else
	ldrd	r4, r5, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #24]
	ldr	r7, [r0, #28]
#else
	ldrd	r6, r7, [r0, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r1, #16]
	ldr	r9, [r1, #20]
#else
	ldrd	r8, r9, [r1, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #24]
	ldr	r11, [r1, #28]
#else
	ldrd	r10, r11, [r1, #24]
#endif
	sbcs	r8, r4, r8
	sbcs	r9, r5, r9
	sbcs	r10, r6, r10
	sbc	r11, r7, r11
	mov	r12, #-19
	asr	r3, r11, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Add modulus (if underflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
#else
	ldrd	r6, r7, [r0, #8]
#endif
	adds	r4, r4, r12
	adcs	r5, r5, r3
	adcs	r6, r6, r3
	adcs	r7, r7, r3
	adcs	r8, r8, r3
	adcs	r9, r9, r3
	adcs	r10, r10, r3
	adc	r11, r11, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #8]
	str	r7, [r0, #12]
#else
	strd	r6, r7, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #24]
	str	r11, [r0, #28]
#else
	strd	r10, r11, [r0, #24]
#endif
	add	sp, sp, #16
	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
	.size	fe_ge_dbl,.-fe_ge_dbl
	.text
	.align	4
	.globl	fe_ge_madd
	.type	fe_ge_madd, %function
fe_ge_madd:
	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
	sub	sp, sp, #32
	str	r0, [sp]
	str	r1, [sp, #4]
	str	r2, [sp, #8]
	str	r3, [sp, #12]
	ldr	r0, [sp]
	ldr	r1, [sp, #72]
	ldr	r2, [sp, #68]
	# Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1]
	ldr	r5, [r1, #4]
#else
	ldrd	r4, r5, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #8]
	ldr	r7, [r1, #12]
#else
	ldrd	r6, r7, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r2]
	ldr	r9, [r2, #4]
#else
	ldrd	r8, r9, [r2]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r2, #8]
	ldr	r11, [r2, #12]
#else
	ldrd	r10, r11, [r2, #8]
#endif
	adds	r8, r4, r8
	adcs	r9, r5, r9
	adcs	r10, r6, r10
	adcs	r11, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0]
	str	r9, [r0, #4]
#else
	strd	r8, r9, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #8]
	str	r11, [r0, #12]
#else
	strd	r10, r11, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #16]
	ldr	r5, [r1, #20]
#else
	ldrd	r4, r5, [r1, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #24]
	ldr	r7, [r1, #28]
#else
	ldrd	r6, r7, [r1, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r2, #16]
	ldr	r9, [r2, #20]
#else
	ldrd	r8, r9, [r2, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r2, #24]
	ldr	r11, [r2, #28]
#else
	ldrd	r10, r11, [r2, #24]
#endif
	adcs	r8, r4, r8
	adcs	r9, r5, r9
	adcs	r10, r6, r10
	adc	r11, r7, r11
	mov	r12, #-19
	asr	r3, r11, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Sub modulus (if overflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
#else
	ldrd	r6, r7, [r0, #8]
#endif
	subs	r4, r4, r12
	sbcs	r5, r5, r3
	sbcs	r6, r6, r3
	sbcs	r7, r7, r3
	sbcs	r8, r8, r3
	sbcs	r9, r9, r3
	sbcs	r10, r10, r3
	sbc	r11, r11, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #8]
	str	r7, [r0, #12]
#else
	strd	r6, r7, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #24]
	str	r11, [r0, #28]
#else
	strd	r10, r11, [r0, #24]
#endif
	ldr	r0, [sp, #4]
	ldr	r1, [sp, #72]
	ldr	r2, [sp, #68]
	# Sub
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1]
	ldr	r5, [r1, #4]
#else
	ldrd	r4, r5, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #8]
	ldr	r7, [r1, #12]
#else
	ldrd	r6, r7, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r2]
	ldr	r9, [r2, #4]
#else
	ldrd	r8, r9, [r2]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r2, #8]
	ldr	r11, [r2, #12]
#else
	ldrd	r10, r11, [r2, #8]
#endif
	subs	r8, r4, r8
	sbcs	r9, r5, r9
	sbcs	r10, r6, r10
	sbcs	r11, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0]
	str	r9, [r0, #4]
#else
	strd	r8, r9, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #8]
	str	r11, [r0, #12]
#else
	strd	r10, r11, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #16]
	ldr	r5, [r1, #20]
#else
	ldrd	r4, r5, [r1, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #24]
	ldr	r7, [r1, #28]
#else
	ldrd	r6, r7, [r1, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r2, #16]
	ldr	r9, [r2, #20]
#else
	ldrd	r8, r9, [r2, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r2, #24]
	ldr	r11, [r2, #28]
#else
	ldrd	r10, r11, [r2, #24]
#endif
	sbcs	r8, r4, r8
	sbcs	r9, r5, r9
	sbcs	r10, r6, r10
	sbc	r11, r7, r11
	mov	r12, #-19
	asr	r3, r11, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Add modulus (if underflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
#else
	ldrd	r6, r7, [r0, #8]
#endif
	adds	r4, r4, r12
	adcs	r5, r5, r3
	adcs	r6, r6, r3
	adcs	r7, r7, r3
	adcs	r8, r8, r3
	adcs	r9, r9, r3
	adcs	r10, r10, r3
	adc	r11, r11, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #8]
	str	r7, [r0, #12]
#else
	strd	r6, r7, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #24]
	str	r11, [r0, #28]
#else
	strd	r10, r11, [r0, #24]
#endif
	ldr	r2, [sp, #88]
	ldr	r1, [sp]
	ldr	r0, [sp, #8]
	bl	fe_mul
	ldr	r2, [sp, #92]
	ldr	r1, [sp, #4]
	ldr	r0, [sp, #4]
	bl	fe_mul
	ldr	r2, [sp, #80]
	ldr	r1, [sp, #84]
	ldr	r0, [sp, #12]
	bl	fe_mul
	ldr	r0, [sp, #4]
	ldr	r1, [sp]
	ldr	r2, [sp, #8]
	# Add-Sub
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r2]
	ldr	r5, [r2, #4]
#else
	ldrd	r4, r5, [r2]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0]
	ldr	r7, [r0, #4]
#else
	ldrd	r6, r7, [r0]
#endif
	adds	r8, r4, r6
	mov	r12, #0
	adcs	r9, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0]
	str	r9, [r0, #4]
#else
	strd	r8, r9, [r0]
#endif
	#  Sub
	subs	r10, r4, r6
	mov	lr, #0
	sbcs	r11, r5, r7
	adc	lr, lr, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1]
	str	r11, [r1, #4]
#else
	strd	r10, r11, [r1]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r2, #8]
	ldr	r5, [r2, #12]
#else
	ldrd	r4, r5, [r2, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
#else
	ldrd	r6, r7, [r0, #8]
#endif
	adds	r12, r12, #-1
	adcs	r8, r4, r6
	mov	r12, #0
	adcs	r9, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #8]
	str	r9, [r0, #12]
#else
	strd	r8, r9, [r0, #8]
#endif
	#  Sub
	adds	lr, lr, #-1
	sbcs	r10, r4, r6
	mov	lr, #0
	sbcs	r11, r5, r7
	adc	lr, lr, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1, #8]
	str	r11, [r1, #12]
#else
	strd	r10, r11, [r1, #8]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r2, #16]
	ldr	r5, [r2, #20]
#else
	ldrd	r4, r5, [r2, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #16]
	ldr	r7, [r0, #20]
#else
	ldrd	r6, r7, [r0, #16]
#endif
	adds	r12, r12, #-1
	adcs	r8, r4, r6
	mov	r12, #0
	adcs	r9, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
	#  Sub
	adds	lr, lr, #-1
	sbcs	r10, r4, r6
	mov	lr, #0
	sbcs	r11, r5, r7
	adc	lr, lr, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1, #16]
	str	r11, [r1, #20]
#else
	strd	r10, r11, [r1, #16]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r2, #24]
	ldr	r5, [r2, #28]
#else
	ldrd	r4, r5, [r2, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #24]
	ldr	r7, [r0, #28]
#else
	ldrd	r6, r7, [r0, #24]
#endif
	adds	r12, r12, #-1
	adcs	r8, r4, r6
	adc	r9, r5, r7
	#  Sub
	adds	lr, lr, #-1
	sbcs	r10, r4, r6
	sbc	r11, r5, r7
	mov	r12, #-19
	asr	r3, r9, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Sub modulus (if overflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
	subs	r4, r4, r12
	sbcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #8]
	ldr	r5, [r0, #12]
#else
	ldrd	r4, r5, [r0, #8]
#endif
	sbcs	r4, r4, r3
	sbcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #8]
	str	r5, [r0, #12]
#else
	strd	r4, r5, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #16]
	ldr	r5, [r0, #20]
#else
	ldrd	r4, r5, [r0, #16]
#endif
	sbcs	r4, r4, r3
	sbcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #16]
	str	r5, [r0, #20]
#else
	strd	r4, r5, [r0, #16]
#endif
	sbcs	r8, r8, r3
	sbc	r9, r9, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #24]
	str	r9, [r0, #28]
#else
	strd	r8, r9, [r0, #24]
#endif
	mov	r12, #-19
	asr	r3, r11, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Add modulus (if underflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1]
	ldr	r5, [r1, #4]
#else
	ldrd	r4, r5, [r1]
#endif
	adds	r4, r4, r12
	adcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r1]
	str	r5, [r1, #4]
#else
	strd	r4, r5, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #8]
	ldr	r5, [r1, #12]
#else
	ldrd	r4, r5, [r1, #8]
#endif
	adcs	r4, r4, r3
	adcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r1, #8]
	str	r5, [r1, #12]
#else
	strd	r4, r5, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #16]
	ldr	r5, [r1, #20]
#else
	ldrd	r4, r5, [r1, #16]
#endif
	adcs	r4, r4, r3
	adcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r1, #16]
	str	r5, [r1, #20]
#else
	strd	r4, r5, [r1, #16]
#endif
	adcs	r10, r10, r3
	adc	r11, r11, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1, #24]
	str	r11, [r1, #28]
#else
	strd	r10, r11, [r1, #24]
#endif
	ldr	r0, [sp, #8]
	ldr	r1, [sp, #76]
	# Double
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1]
	ldr	r5, [r1, #4]
#else
	ldrd	r4, r5, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #8]
	ldr	r7, [r1, #12]
#else
	ldrd	r6, r7, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r1, #16]
	ldr	r9, [r1, #20]
#else
	ldrd	r8, r9, [r1, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #24]
	ldr	r11, [r1, #28]
#else
	ldrd	r10, r11, [r1, #24]
#endif
	adds	r4, r4, r4
	adcs	r5, r5, r5
	adcs	r6, r6, r6
	adcs	r7, r7, r7
	adcs	r8, r8, r8
	adcs	r9, r9, r9
	adcs	r10, r10, r10
	adc	r11, r11, r11
	mov	r12, #-19
	asr	r3, r11, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Sub modulus (if overflow)
	subs	r4, r4, r12
	sbcs	r5, r5, r3
	sbcs	r6, r6, r3
	sbcs	r7, r7, r3
	sbcs	r8, r8, r3
	sbcs	r9, r9, r3
	sbcs	r10, r10, r3
	sbc	r11, r11, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #8]
	str	r7, [r0, #12]
#else
	strd	r6, r7, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #24]
	str	r11, [r0, #28]
#else
	strd	r10, r11, [r0, #24]
#endif
	ldr	r0, [sp, #8]
	ldr	r1, [sp, #12]
	# Add-Sub
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1]
	ldr	r7, [r1, #4]
#else
	ldrd	r6, r7, [r1]
#endif
	adds	r8, r4, r6
	mov	r12, #0
	adcs	r9, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0]
	str	r9, [r0, #4]
#else
	strd	r8, r9, [r0]
#endif
	#  Sub
	subs	r10, r4, r6
	mov	lr, #0
	sbcs	r11, r5, r7
	adc	lr, lr, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1]
	str	r11, [r1, #4]
#else
	strd	r10, r11, [r1]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #8]
	ldr	r5, [r0, #12]
#else
	ldrd	r4, r5, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #8]
	ldr	r7, [r1, #12]
#else
	ldrd	r6, r7, [r1, #8]
#endif
	adds	r12, r12, #-1
	adcs	r8, r4, r6
	mov	r12, #0
	adcs	r9, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #8]
	str	r9, [r0, #12]
#else
	strd	r8, r9, [r0, #8]
#endif
	#  Sub
	adds	lr, lr, #-1
	sbcs	r10, r4, r6
	mov	lr, #0
	sbcs	r11, r5, r7
	adc	lr, lr, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1, #8]
	str	r11, [r1, #12]
#else
	strd	r10, r11, [r1, #8]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #16]
	ldr	r5, [r0, #20]
#else
	ldrd	r4, r5, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #16]
	ldr	r7, [r1, #20]
#else
	ldrd	r6, r7, [r1, #16]
#endif
	adds	r12, r12, #-1
	adcs	r8, r4, r6
	mov	r12, #0
	adcs	r9, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
	#  Sub
	adds	lr, lr, #-1
	sbcs	r10, r4, r6
	mov	lr, #0
	sbcs	r11, r5, r7
	adc	lr, lr, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1, #16]
	str	r11, [r1, #20]
#else
	strd	r10, r11, [r1, #16]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #24]
	ldr	r5, [r0, #28]
#else
	ldrd	r4, r5, [r0, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #24]
	ldr	r7, [r1, #28]
#else
	ldrd	r6, r7, [r1, #24]
#endif
	adds	r12, r12, #-1
	adcs	r8, r4, r6
	adc	r9, r5, r7
	#  Sub
	adds	lr, lr, #-1
	sbcs	r10, r4, r6
	sbc	r11, r5, r7
	mov	r12, #-19
	asr	r3, r9, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Sub modulus (if overflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
	subs	r4, r4, r12
	sbcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #8]
	ldr	r5, [r0, #12]
#else
	ldrd	r4, r5, [r0, #8]
#endif
	sbcs	r4, r4, r3
	sbcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #8]
	str	r5, [r0, #12]
#else
	strd	r4, r5, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #16]
	ldr	r5, [r0, #20]
#else
	ldrd	r4, r5, [r0, #16]
#endif
	sbcs	r4, r4, r3
	sbcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #16]
	str	r5, [r0, #20]
#else
	strd	r4, r5, [r0, #16]
#endif
	sbcs	r8, r8, r3
	sbc	r9, r9, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #24]
	str	r9, [r0, #28]
#else
	strd	r8, r9, [r0, #24]
#endif
	mov	r12, #-19
	asr	r3, r11, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Add modulus (if underflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1]
	ldr	r5, [r1, #4]
#else
	ldrd	r4, r5, [r1]
#endif
	adds	r4, r4, r12
	adcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r1]
	str	r5, [r1, #4]
#else
	strd	r4, r5, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #8]
	ldr	r5, [r1, #12]
#else
	ldrd	r4, r5, [r1, #8]
#endif
	adcs	r4, r4, r3
	adcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r1, #8]
	str	r5, [r1, #12]
#else
	strd	r4, r5, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #16]
	ldr	r5, [r1, #20]
#else
	ldrd	r4, r5, [r1, #16]
#endif
	adcs	r4, r4, r3
	adcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r1, #16]
	str	r5, [r1, #20]
#else
	strd	r4, r5, [r1, #16]
#endif
	adcs	r10, r10, r3
	adc	r11, r11, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1, #24]
	str	r11, [r1, #28]
#else
	strd	r10, r11, [r1, #24]
#endif
	add	sp, sp, #32
	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
	.size	fe_ge_madd,.-fe_ge_madd
	.text
	.align	4
	.globl	fe_ge_msub
	.type	fe_ge_msub, %function
fe_ge_msub:
	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
	sub	sp, sp, #32
	str	r0, [sp]
	str	r1, [sp, #4]
	str	r2, [sp, #8]
	str	r3, [sp, #12]
	ldr	r0, [sp]
	ldr	r1, [sp, #72]
	ldr	r2, [sp, #68]
	# Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1]
	ldr	r5, [r1, #4]
#else
	ldrd	r4, r5, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #8]
	ldr	r7, [r1, #12]
#else
	ldrd	r6, r7, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r2]
	ldr	r9, [r2, #4]
#else
	ldrd	r8, r9, [r2]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r2, #8]
	ldr	r11, [r2, #12]
#else
	ldrd	r10, r11, [r2, #8]
#endif
	adds	r8, r4, r8
	adcs	r9, r5, r9
	adcs	r10, r6, r10
	adcs	r11, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0]
	str	r9, [r0, #4]
#else
	strd	r8, r9, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #8]
	str	r11, [r0, #12]
#else
	strd	r10, r11, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #16]
	ldr	r5, [r1, #20]
#else
	ldrd	r4, r5, [r1, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #24]
	ldr	r7, [r1, #28]
#else
	ldrd	r6, r7, [r1, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r2, #16]
	ldr	r9, [r2, #20]
#else
	ldrd	r8, r9, [r2, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r2, #24]
	ldr	r11, [r2, #28]
#else
	ldrd	r10, r11, [r2, #24]
#endif
	adcs	r8, r4, r8
	adcs	r9, r5, r9
	adcs	r10, r6, r10
	adc	r11, r7, r11
	mov	r12, #-19
	asr	r3, r11, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Sub modulus (if overflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
#else
	ldrd	r6, r7, [r0, #8]
#endif
	subs	r4, r4, r12
	sbcs	r5, r5, r3
	sbcs	r6, r6, r3
	sbcs	r7, r7, r3
	sbcs	r8, r8, r3
	sbcs	r9, r9, r3
	sbcs	r10, r10, r3
	sbc	r11, r11, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #8]
	str	r7, [r0, #12]
#else
	strd	r6, r7, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #24]
	str	r11, [r0, #28]
#else
	strd	r10, r11, [r0, #24]
#endif
	ldr	r0, [sp, #4]
	ldr	r1, [sp, #72]
	ldr	r2, [sp, #68]
	# Sub
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1]
	ldr	r5, [r1, #4]
#else
	ldrd	r4, r5, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #8]
	ldr	r7, [r1, #12]
#else
	ldrd	r6, r7, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r2]
	ldr	r9, [r2, #4]
#else
	ldrd	r8, r9, [r2]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r2, #8]
	ldr	r11, [r2, #12]
#else
	ldrd	r10, r11, [r2, #8]
#endif
	subs	r8, r4, r8
	sbcs	r9, r5, r9
	sbcs	r10, r6, r10
	sbcs	r11, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0]
	str	r9, [r0, #4]
#else
	strd	r8, r9, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #8]
	str	r11, [r0, #12]
#else
	strd	r10, r11, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #16]
	ldr	r5, [r1, #20]
#else
	ldrd	r4, r5, [r1, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #24]
	ldr	r7, [r1, #28]
#else
	ldrd	r6, r7, [r1, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r2, #16]
	ldr	r9, [r2, #20]
#else
	ldrd	r8, r9, [r2, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r2, #24]
	ldr	r11, [r2, #28]
#else
	ldrd	r10, r11, [r2, #24]
#endif
	sbcs	r8, r4, r8
	sbcs	r9, r5, r9
	sbcs	r10, r6, r10
	sbc	r11, r7, r11
	mov	r12, #-19
	asr	r3, r11, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Add modulus (if underflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
#else
	ldrd	r6, r7, [r0, #8]
#endif
	adds	r4, r4, r12
	adcs	r5, r5, r3
	adcs	r6, r6, r3
	adcs	r7, r7, r3
	adcs	r8, r8, r3
	adcs	r9, r9, r3
	adcs	r10, r10, r3
	adc	r11, r11, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #8]
	str	r7, [r0, #12]
#else
	strd	r6, r7, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #24]
	str	r11, [r0, #28]
#else
	strd	r10, r11, [r0, #24]
#endif
	ldr	r2, [sp, #92]
	ldr	r1, [sp]
	ldr	r0, [sp, #8]
	bl	fe_mul
	ldr	r2, [sp, #88]
	ldr	r1, [sp, #4]
	ldr	r0, [sp, #4]
	bl	fe_mul
	ldr	r2, [sp, #80]
	ldr	r1, [sp, #84]
	ldr	r0, [sp, #12]
	bl	fe_mul
	ldr	r0, [sp, #4]
	ldr	r1, [sp]
	ldr	r2, [sp, #8]
	# Add-Sub
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r2]
	ldr	r5, [r2, #4]
#else
	ldrd	r4, r5, [r2]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0]
	ldr	r7, [r0, #4]
#else
	ldrd	r6, r7, [r0]
#endif
	adds	r8, r4, r6
	mov	r12, #0
	adcs	r9, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0]
	str	r9, [r0, #4]
#else
	strd	r8, r9, [r0]
#endif
	#  Sub
	subs	r10, r4, r6
	mov	lr, #0
	sbcs	r11, r5, r7
	adc	lr, lr, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1]
	str	r11, [r1, #4]
#else
	strd	r10, r11, [r1]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r2, #8]
	ldr	r5, [r2, #12]
#else
	ldrd	r4, r5, [r2, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
#else
	ldrd	r6, r7, [r0, #8]
#endif
	adds	r12, r12, #-1
	adcs	r8, r4, r6
	mov	r12, #0
	adcs	r9, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #8]
	str	r9, [r0, #12]
#else
	strd	r8, r9, [r0, #8]
#endif
	#  Sub
	adds	lr, lr, #-1
	sbcs	r10, r4, r6
	mov	lr, #0
	sbcs	r11, r5, r7
	adc	lr, lr, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1, #8]
	str	r11, [r1, #12]
#else
	strd	r10, r11, [r1, #8]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r2, #16]
	ldr	r5, [r2, #20]
#else
	ldrd	r4, r5, [r2, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #16]
	ldr	r7, [r0, #20]
#else
	ldrd	r6, r7, [r0, #16]
#endif
	adds	r12, r12, #-1
	adcs	r8, r4, r6
	mov	r12, #0
	adcs	r9, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
	#  Sub
	adds	lr, lr, #-1
	sbcs	r10, r4, r6
	mov	lr, #0
	sbcs	r11, r5, r7
	adc	lr, lr, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1, #16]
	str	r11, [r1, #20]
#else
	strd	r10, r11, [r1, #16]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r2, #24]
	ldr	r5, [r2, #28]
#else
	ldrd	r4, r5, [r2, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #24]
	ldr	r7, [r0, #28]
#else
	ldrd	r6, r7, [r0, #24]
#endif
	adds	r12, r12, #-1
	adcs	r8, r4, r6
	adc	r9, r5, r7
	#  Sub
	adds	lr, lr, #-1
	sbcs	r10, r4, r6
	sbc	r11, r5, r7
	mov	r12, #-19
	asr	r3, r9, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Sub modulus (if overflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
	subs	r4, r4, r12
	sbcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #8]
	ldr	r5, [r0, #12]
#else
	ldrd	r4, r5, [r0, #8]
#endif
	sbcs	r4, r4, r3
	sbcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #8]
	str	r5, [r0, #12]
#else
	strd	r4, r5, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #16]
	ldr	r5, [r0, #20]
#else
	ldrd	r4, r5, [r0, #16]
#endif
	sbcs	r4, r4, r3
	sbcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #16]
	str	r5, [r0, #20]
#else
	strd	r4, r5, [r0, #16]
#endif
	sbcs	r8, r8, r3
	sbc	r9, r9, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #24]
	str	r9, [r0, #28]
#else
	strd	r8, r9, [r0, #24]
#endif
	mov	r12, #-19
	asr	r3, r11, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Add modulus (if underflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1]
	ldr	r5, [r1, #4]
#else
	ldrd	r4, r5, [r1]
#endif
	adds	r4, r4, r12
	adcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r1]
	str	r5, [r1, #4]
#else
	strd	r4, r5, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #8]
	ldr	r5, [r1, #12]
#else
	ldrd	r4, r5, [r1, #8]
#endif
	adcs	r4, r4, r3
	adcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r1, #8]
	str	r5, [r1, #12]
#else
	strd	r4, r5, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #16]
	ldr	r5, [r1, #20]
#else
	ldrd	r4, r5, [r1, #16]
#endif
	adcs	r4, r4, r3
	adcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r1, #16]
	str	r5, [r1, #20]
#else
	strd	r4, r5, [r1, #16]
#endif
	adcs	r10, r10, r3
	adc	r11, r11, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1, #24]
	str	r11, [r1, #28]
#else
	strd	r10, r11, [r1, #24]
#endif
	ldr	r0, [sp, #8]
	ldr	r1, [sp, #76]
	# Double
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1]
	ldr	r5, [r1, #4]
#else
	ldrd	r4, r5, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #8]
	ldr	r7, [r1, #12]
#else
	ldrd	r6, r7, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r1, #16]
	ldr	r9, [r1, #20]
#else
	ldrd	r8, r9, [r1, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #24]
	ldr	r11, [r1, #28]
#else
	ldrd	r10, r11, [r1, #24]
#endif
	adds	r4, r4, r4
	adcs	r5, r5, r5
	adcs	r6, r6, r6
	adcs	r7, r7, r7
	adcs	r8, r8, r8
	adcs	r9, r9, r9
	adcs	r10, r10, r10
	adc	r11, r11, r11
	mov	r12, #-19
	asr	r3, r11, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Sub modulus (if overflow)
	subs	r4, r4, r12
	sbcs	r5, r5, r3
	sbcs	r6, r6, r3
	sbcs	r7, r7, r3
	sbcs	r8, r8, r3
	sbcs	r9, r9, r3
	sbcs	r10, r10, r3
	sbc	r11, r11, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #8]
	str	r7, [r0, #12]
#else
	strd	r6, r7, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #24]
	str	r11, [r0, #28]
#else
	strd	r10, r11, [r0, #24]
#endif
	ldr	r0, [sp, #12]
	ldr	r1, [sp, #8]
	# Add-Sub
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1]
	ldr	r5, [r1, #4]
#else
	ldrd	r4, r5, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0]
	ldr	r7, [r0, #4]
#else
	ldrd	r6, r7, [r0]
#endif
	adds	r8, r4, r6
	mov	r12, #0
	adcs	r9, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0]
	str	r9, [r0, #4]
#else
	strd	r8, r9, [r0]
#endif
	#  Sub
	subs	r10, r4, r6
	mov	lr, #0
	sbcs	r11, r5, r7
	adc	lr, lr, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1]
	str	r11, [r1, #4]
#else
	strd	r10, r11, [r1]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #8]
	ldr	r5, [r1, #12]
#else
	ldrd	r4, r5, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
#else
	ldrd	r6, r7, [r0, #8]
#endif
	adds	r12, r12, #-1
	adcs	r8, r4, r6
	mov	r12, #0
	adcs	r9, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #8]
	str	r9, [r0, #12]
#else
	strd	r8, r9, [r0, #8]
#endif
	#  Sub
	adds	lr, lr, #-1
	sbcs	r10, r4, r6
	mov	lr, #0
	sbcs	r11, r5, r7
	adc	lr, lr, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1, #8]
	str	r11, [r1, #12]
#else
	strd	r10, r11, [r1, #8]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #16]
	ldr	r5, [r1, #20]
#else
	ldrd	r4, r5, [r1, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #16]
	ldr	r7, [r0, #20]
#else
	ldrd	r6, r7, [r0, #16]
#endif
	adds	r12, r12, #-1
	adcs	r8, r4, r6
	mov	r12, #0
	adcs	r9, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
	#  Sub
	adds	lr, lr, #-1
	sbcs	r10, r4, r6
	mov	lr, #0
	sbcs	r11, r5, r7
	adc	lr, lr, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1, #16]
	str	r11, [r1, #20]
#else
	strd	r10, r11, [r1, #16]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #24]
	ldr	r5, [r1, #28]
#else
	ldrd	r4, r5, [r1, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #24]
	ldr	r7, [r0, #28]
#else
	ldrd	r6, r7, [r0, #24]
#endif
	adds	r12, r12, #-1
	adcs	r8, r4, r6
	adc	r9, r5, r7
	#  Sub
	adds	lr, lr, #-1
	sbcs	r10, r4, r6
	sbc	r11, r5, r7
	mov	r12, #-19
	asr	r3, r9, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Sub modulus (if overflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
	subs	r4, r4, r12
	sbcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #8]
	ldr	r5, [r0, #12]
#else
	ldrd	r4, r5, [r0, #8]
#endif
	sbcs	r4, r4, r3
	sbcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #8]
	str	r5, [r0, #12]
#else
	strd	r4, r5, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #16]
	ldr	r5, [r0, #20]
#else
	ldrd	r4, r5, [r0, #16]
#endif
	sbcs	r4, r4, r3
	sbcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #16]
	str	r5, [r0, #20]
#else
	strd	r4, r5, [r0, #16]
#endif
	sbcs	r8, r8, r3
	sbc	r9, r9, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #24]
	str	r9, [r0, #28]
#else
	strd	r8, r9, [r0, #24]
#endif
	mov	r12, #-19
	asr	r3, r11, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Add modulus (if underflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1]
	ldr	r5, [r1, #4]
#else
	ldrd	r4, r5, [r1]
#endif
	adds	r4, r4, r12
	adcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r1]
	str	r5, [r1, #4]
#else
	strd	r4, r5, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #8]
	ldr	r5, [r1, #12]
#else
	ldrd	r4, r5, [r1, #8]
#endif
	adcs	r4, r4, r3
	adcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r1, #8]
	str	r5, [r1, #12]
#else
	strd	r4, r5, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #16]
	ldr	r5, [r1, #20]
#else
	ldrd	r4, r5, [r1, #16]
#endif
	adcs	r4, r4, r3
	adcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r1, #16]
	str	r5, [r1, #20]
#else
	strd	r4, r5, [r1, #16]
#endif
	adcs	r10, r10, r3
	adc	r11, r11, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1, #24]
	str	r11, [r1, #28]
#else
	strd	r10, r11, [r1, #24]
#endif
	add	sp, sp, #32
	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
	.size	fe_ge_msub,.-fe_ge_msub
	.text
	.align	4
	.globl	fe_ge_add
	.type	fe_ge_add, %function
fe_ge_add:
	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
	sub	sp, sp, #0x60
	str	r0, [sp]
	str	r1, [sp, #4]
	str	r2, [sp, #8]
	str	r3, [sp, #12]
	ldr	r0, [sp]
	ldr	r1, [sp, #136]
	ldr	r2, [sp, #132]
	# Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1]
	ldr	r5, [r1, #4]
#else
	ldrd	r4, r5, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #8]
	ldr	r7, [r1, #12]
#else
	ldrd	r6, r7, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r2]
	ldr	r9, [r2, #4]
#else
	ldrd	r8, r9, [r2]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r2, #8]
	ldr	r11, [r2, #12]
#else
	ldrd	r10, r11, [r2, #8]
#endif
	adds	r8, r4, r8
	adcs	r9, r5, r9
	adcs	r10, r6, r10
	adcs	r11, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0]
	str	r9, [r0, #4]
#else
	strd	r8, r9, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #8]
	str	r11, [r0, #12]
#else
	strd	r10, r11, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #16]
	ldr	r5, [r1, #20]
#else
	ldrd	r4, r5, [r1, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #24]
	ldr	r7, [r1, #28]
#else
	ldrd	r6, r7, [r1, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r2, #16]
	ldr	r9, [r2, #20]
#else
	ldrd	r8, r9, [r2, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r2, #24]
	ldr	r11, [r2, #28]
#else
	ldrd	r10, r11, [r2, #24]
#endif
	adcs	r8, r4, r8
	adcs	r9, r5, r9
	adcs	r10, r6, r10
	adc	r11, r7, r11
	mov	r12, #-19
	asr	r3, r11, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Sub modulus (if overflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
#else
	ldrd	r6, r7, [r0, #8]
#endif
	subs	r4, r4, r12
	sbcs	r5, r5, r3
	sbcs	r6, r6, r3
	sbcs	r7, r7, r3
	sbcs	r8, r8, r3
	sbcs	r9, r9, r3
	sbcs	r10, r10, r3
	sbc	r11, r11, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #8]
	str	r7, [r0, #12]
#else
	strd	r6, r7, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #24]
	str	r11, [r0, #28]
#else
	strd	r10, r11, [r0, #24]
#endif
	ldr	r0, [sp, #4]
	ldr	r1, [sp, #136]
	ldr	r2, [sp, #132]
	# Sub
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1]
	ldr	r5, [r1, #4]
#else
	ldrd	r4, r5, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #8]
	ldr	r7, [r1, #12]
#else
	ldrd	r6, r7, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r2]
	ldr	r9, [r2, #4]
#else
	ldrd	r8, r9, [r2]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r2, #8]
	ldr	r11, [r2, #12]
#else
	ldrd	r10, r11, [r2, #8]
#endif
	subs	r8, r4, r8
	sbcs	r9, r5, r9
	sbcs	r10, r6, r10
	sbcs	r11, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0]
	str	r9, [r0, #4]
#else
	strd	r8, r9, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #8]
	str	r11, [r0, #12]
#else
	strd	r10, r11, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #16]
	ldr	r5, [r1, #20]
#else
	ldrd	r4, r5, [r1, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #24]
	ldr	r7, [r1, #28]
#else
	ldrd	r6, r7, [r1, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r2, #16]
	ldr	r9, [r2, #20]
#else
	ldrd	r8, r9, [r2, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r2, #24]
	ldr	r11, [r2, #28]
#else
	ldrd	r10, r11, [r2, #24]
#endif
	sbcs	r8, r4, r8
	sbcs	r9, r5, r9
	sbcs	r10, r6, r10
	sbc	r11, r7, r11
	mov	r12, #-19
	asr	r3, r11, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Add modulus (if underflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
#else
	ldrd	r6, r7, [r0, #8]
#endif
	adds	r4, r4, r12
	adcs	r5, r5, r3
	adcs	r6, r6, r3
	adcs	r7, r7, r3
	adcs	r8, r8, r3
	adcs	r9, r9, r3
	adcs	r10, r10, r3
	adc	r11, r11, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #8]
	str	r7, [r0, #12]
#else
	strd	r6, r7, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #24]
	str	r11, [r0, #28]
#else
	strd	r10, r11, [r0, #24]
#endif
	ldr	r2, [sp, #156]
	ldr	r1, [sp]
	ldr	r0, [sp, #8]
	bl	fe_mul
	ldr	r2, [sp, #160]
	ldr	r1, [sp, #4]
	ldr	r0, [sp, #4]
	bl	fe_mul
	ldr	r2, [sp, #144]
	ldr	r1, [sp, #152]
	ldr	r0, [sp, #12]
	bl	fe_mul
	ldr	r2, [sp, #148]
	ldr	r1, [sp, #140]
	ldr	r0, [sp]
	bl	fe_mul
	add	r0, sp, #16
	ldr	r1, [sp]
	# Double
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1]
	ldr	r5, [r1, #4]
#else
	ldrd	r4, r5, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #8]
	ldr	r7, [r1, #12]
#else
	ldrd	r6, r7, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r1, #16]
	ldr	r9, [r1, #20]
#else
	ldrd	r8, r9, [r1, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #24]
	ldr	r11, [r1, #28]
#else
	ldrd	r10, r11, [r1, #24]
#endif
	adds	r4, r4, r4
	adcs	r5, r5, r5
	adcs	r6, r6, r6
	adcs	r7, r7, r7
	adcs	r8, r8, r8
	adcs	r9, r9, r9
	adcs	r10, r10, r10
	adc	r11, r11, r11
	mov	r12, #-19
	asr	r3, r11, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Sub modulus (if overflow)
	subs	r4, r4, r12
	sbcs	r5, r5, r3
	sbcs	r6, r6, r3
	sbcs	r7, r7, r3
	sbcs	r8, r8, r3
	sbcs	r9, r9, r3
	sbcs	r10, r10, r3
	sbc	r11, r11, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #8]
	str	r7, [r0, #12]
#else
	strd	r6, r7, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #24]
	str	r11, [r0, #28]
#else
	strd	r10, r11, [r0, #24]
#endif
	ldr	r0, [sp, #4]
	ldr	r1, [sp]
	ldr	r2, [sp, #8]
	# Add-Sub
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r2]
	ldr	r5, [r2, #4]
#else
	ldrd	r4, r5, [r2]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0]
	ldr	r7, [r0, #4]
#else
	ldrd	r6, r7, [r0]
#endif
	adds	r8, r4, r6
	mov	r12, #0
	adcs	r9, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0]
	str	r9, [r0, #4]
#else
	strd	r8, r9, [r0]
#endif
	#  Sub
	subs	r10, r4, r6
	mov	lr, #0
	sbcs	r11, r5, r7
	adc	lr, lr, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1]
	str	r11, [r1, #4]
#else
	strd	r10, r11, [r1]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r2, #8]
	ldr	r5, [r2, #12]
#else
	ldrd	r4, r5, [r2, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
#else
	ldrd	r6, r7, [r0, #8]
#endif
	adds	r12, r12, #-1
	adcs	r8, r4, r6
	mov	r12, #0
	adcs	r9, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #8]
	str	r9, [r0, #12]
#else
	strd	r8, r9, [r0, #8]
#endif
	#  Sub
	adds	lr, lr, #-1
	sbcs	r10, r4, r6
	mov	lr, #0
	sbcs	r11, r5, r7
	adc	lr, lr, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1, #8]
	str	r11, [r1, #12]
#else
	strd	r10, r11, [r1, #8]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r2, #16]
	ldr	r5, [r2, #20]
#else
	ldrd	r4, r5, [r2, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #16]
	ldr	r7, [r0, #20]
#else
	ldrd	r6, r7, [r0, #16]
#endif
	adds	r12, r12, #-1
	adcs	r8, r4, r6
	mov	r12, #0
	adcs	r9, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
	#  Sub
	adds	lr, lr, #-1
	sbcs	r10, r4, r6
	mov	lr, #0
	sbcs	r11, r5, r7
	adc	lr, lr, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1, #16]
	str	r11, [r1, #20]
#else
	strd	r10, r11, [r1, #16]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r2, #24]
	ldr	r5, [r2, #28]
#else
	ldrd	r4, r5, [r2, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #24]
	ldr	r7, [r0, #28]
#else
	ldrd	r6, r7, [r0, #24]
#endif
	adds	r12, r12, #-1
	adcs	r8, r4, r6
	adc	r9, r5, r7
	#  Sub
	adds	lr, lr, #-1
	sbcs	r10, r4, r6
	sbc	r11, r5, r7
	mov	r12, #-19
	asr	r3, r9, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Sub modulus (if overflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
	subs	r4, r4, r12
	sbcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #8]
	ldr	r5, [r0, #12]
#else
	ldrd	r4, r5, [r0, #8]
#endif
	sbcs	r4, r4, r3
	sbcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #8]
	str	r5, [r0, #12]
#else
	strd	r4, r5, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #16]
	ldr	r5, [r0, #20]
#else
	ldrd	r4, r5, [r0, #16]
#endif
	sbcs	r4, r4, r3
	sbcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #16]
	str	r5, [r0, #20]
#else
	strd	r4, r5, [r0, #16]
#endif
	sbcs	r8, r8, r3
	sbc	r9, r9, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #24]
	str	r9, [r0, #28]
#else
	strd	r8, r9, [r0, #24]
#endif
	mov	r12, #-19
	asr	r3, r11, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Add modulus (if underflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1]
	ldr	r5, [r1, #4]
#else
	ldrd	r4, r5, [r1]
#endif
	adds	r4, r4, r12
	adcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r1]
	str	r5, [r1, #4]
#else
	strd	r4, r5, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #8]
	ldr	r5, [r1, #12]
#else
	ldrd	r4, r5, [r1, #8]
#endif
	adcs	r4, r4, r3
	adcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r1, #8]
	str	r5, [r1, #12]
#else
	strd	r4, r5, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #16]
	ldr	r5, [r1, #20]
#else
	ldrd	r4, r5, [r1, #16]
#endif
	adcs	r4, r4, r3
	adcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r1, #16]
	str	r5, [r1, #20]
#else
	strd	r4, r5, [r1, #16]
#endif
	adcs	r10, r10, r3
	adc	r11, r11, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1, #24]
	str	r11, [r1, #28]
#else
	strd	r10, r11, [r1, #24]
#endif
	ldr	r0, [sp, #8]
	ldr	r1, [sp, #12]
	add	r2, sp, #16
	# Add-Sub
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r2]
	ldr	r5, [r2, #4]
#else
	ldrd	r4, r5, [r2]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1]
	ldr	r7, [r1, #4]
#else
	ldrd	r6, r7, [r1]
#endif
	adds	r8, r4, r6
	mov	r12, #0
	adcs	r9, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0]
	str	r9, [r0, #4]
#else
	strd	r8, r9, [r0]
#endif
	#  Sub
	subs	r10, r4, r6
	mov	lr, #0
	sbcs	r11, r5, r7
	adc	lr, lr, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1]
	str	r11, [r1, #4]
#else
	strd	r10, r11, [r1]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r2, #8]
	ldr	r5, [r2, #12]
#else
	ldrd	r4, r5, [r2, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #8]
	ldr	r7, [r1, #12]
#else
	ldrd	r6, r7, [r1, #8]
#endif
	adds	r12, r12, #-1
	adcs	r8, r4, r6
	mov	r12, #0
	adcs	r9, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #8]
	str	r9, [r0, #12]
#else
	strd	r8, r9, [r0, #8]
#endif
	#  Sub
	adds	lr, lr, #-1
	sbcs	r10, r4, r6
	mov	lr, #0
	sbcs	r11, r5, r7
	adc	lr, lr, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1, #8]
	str	r11, [r1, #12]
#else
	strd	r10, r11, [r1, #8]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r2, #16]
	ldr	r5, [r2, #20]
#else
	ldrd	r4, r5, [r2, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #16]
	ldr	r7, [r1, #20]
#else
	ldrd	r6, r7, [r1, #16]
#endif
	adds	r12, r12, #-1
	adcs	r8, r4, r6
	mov	r12, #0
	adcs	r9, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
	#  Sub
	adds	lr, lr, #-1
	sbcs	r10, r4, r6
	mov	lr, #0
	sbcs	r11, r5, r7
	adc	lr, lr, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1, #16]
	str	r11, [r1, #20]
#else
	strd	r10, r11, [r1, #16]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r2, #24]
	ldr	r5, [r2, #28]
#else
	ldrd	r4, r5, [r2, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #24]
	ldr	r7, [r1, #28]
#else
	ldrd	r6, r7, [r1, #24]
#endif
	adds	r12, r12, #-1
	adcs	r8, r4, r6
	adc	r9, r5, r7
	#  Sub
	adds	lr, lr, #-1
	sbcs	r10, r4, r6
	sbc	r11, r5, r7
	mov	r12, #-19
	asr	r3, r9, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Sub modulus (if overflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
	subs	r4, r4, r12
	sbcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #8]
	ldr	r5, [r0, #12]
#else
	ldrd	r4, r5, [r0, #8]
#endif
	sbcs	r4, r4, r3
	sbcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #8]
	str	r5, [r0, #12]
#else
	strd	r4, r5, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #16]
	ldr	r5, [r0, #20]
#else
	ldrd	r4, r5, [r0, #16]
#endif
	sbcs	r4, r4, r3
	sbcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #16]
	str	r5, [r0, #20]
#else
	strd	r4, r5, [r0, #16]
#endif
	sbcs	r8, r8, r3
	sbc	r9, r9, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #24]
	str	r9, [r0, #28]
#else
	strd	r8, r9, [r0, #24]
#endif
	mov	r12, #-19
	asr	r3, r11, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Add modulus (if underflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1]
	ldr	r5, [r1, #4]
#else
	ldrd	r4, r5, [r1]
#endif
	adds	r4, r4, r12
	adcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r1]
	str	r5, [r1, #4]
#else
	strd	r4, r5, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #8]
	ldr	r5, [r1, #12]
#else
	ldrd	r4, r5, [r1, #8]
#endif
	adcs	r4, r4, r3
	adcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r1, #8]
	str	r5, [r1, #12]
#else
	strd	r4, r5, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #16]
	ldr	r5, [r1, #20]
#else
	ldrd	r4, r5, [r1, #16]
#endif
	adcs	r4, r4, r3
	adcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r1, #16]
	str	r5, [r1, #20]
#else
	strd	r4, r5, [r1, #16]
#endif
	adcs	r10, r10, r3
	adc	r11, r11, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1, #24]
	str	r11, [r1, #28]
#else
	strd	r10, r11, [r1, #24]
#endif
	add	sp, sp, #0x60
	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
	.size	fe_ge_add,.-fe_ge_add
	.text
	.align	4
	.globl	fe_ge_sub
	.type	fe_ge_sub, %function
fe_ge_sub:
	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
	sub	sp, sp, #0x60
	str	r0, [sp]
	str	r1, [sp, #4]
	str	r2, [sp, #8]
	str	r3, [sp, #12]
	ldr	r0, [sp]
	ldr	r1, [sp, #136]
	ldr	r2, [sp, #132]
	# Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1]
	ldr	r5, [r1, #4]
#else
	ldrd	r4, r5, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #8]
	ldr	r7, [r1, #12]
#else
	ldrd	r6, r7, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r2]
	ldr	r9, [r2, #4]
#else
	ldrd	r8, r9, [r2]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r2, #8]
	ldr	r11, [r2, #12]
#else
	ldrd	r10, r11, [r2, #8]
#endif
	adds	r8, r4, r8
	adcs	r9, r5, r9
	adcs	r10, r6, r10
	adcs	r11, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0]
	str	r9, [r0, #4]
#else
	strd	r8, r9, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #8]
	str	r11, [r0, #12]
#else
	strd	r10, r11, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #16]
	ldr	r5, [r1, #20]
#else
	ldrd	r4, r5, [r1, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #24]
	ldr	r7, [r1, #28]
#else
	ldrd	r6, r7, [r1, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r2, #16]
	ldr	r9, [r2, #20]
#else
	ldrd	r8, r9, [r2, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r2, #24]
	ldr	r11, [r2, #28]
#else
	ldrd	r10, r11, [r2, #24]
#endif
	adcs	r8, r4, r8
	adcs	r9, r5, r9
	adcs	r10, r6, r10
	adc	r11, r7, r11
	mov	r12, #-19
	asr	r3, r11, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Sub modulus (if overflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
#else
	ldrd	r6, r7, [r0, #8]
#endif
	subs	r4, r4, r12
	sbcs	r5, r5, r3
	sbcs	r6, r6, r3
	sbcs	r7, r7, r3
	sbcs	r8, r8, r3
	sbcs	r9, r9, r3
	sbcs	r10, r10, r3
	sbc	r11, r11, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #8]
	str	r7, [r0, #12]
#else
	strd	r6, r7, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #24]
	str	r11, [r0, #28]
#else
	strd	r10, r11, [r0, #24]
#endif
	ldr	r0, [sp, #4]
	ldr	r1, [sp, #136]
	ldr	r2, [sp, #132]
	# Sub
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1]
	ldr	r5, [r1, #4]
#else
	ldrd	r4, r5, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #8]
	ldr	r7, [r1, #12]
#else
	ldrd	r6, r7, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r2]
	ldr	r9, [r2, #4]
#else
	ldrd	r8, r9, [r2]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r2, #8]
	ldr	r11, [r2, #12]
#else
	ldrd	r10, r11, [r2, #8]
#endif
	subs	r8, r4, r8
	sbcs	r9, r5, r9
	sbcs	r10, r6, r10
	sbcs	r11, r7, r11
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0]
	str	r9, [r0, #4]
#else
	strd	r8, r9, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #8]
	str	r11, [r0, #12]
#else
	strd	r10, r11, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #16]
	ldr	r5, [r1, #20]
#else
	ldrd	r4, r5, [r1, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #24]
	ldr	r7, [r1, #28]
#else
	ldrd	r6, r7, [r1, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r2, #16]
	ldr	r9, [r2, #20]
#else
	ldrd	r8, r9, [r2, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r2, #24]
	ldr	r11, [r2, #28]
#else
	ldrd	r10, r11, [r2, #24]
#endif
	sbcs	r8, r4, r8
	sbcs	r9, r5, r9
	sbcs	r10, r6, r10
	sbc	r11, r7, r11
	mov	r12, #-19
	asr	r3, r11, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Add modulus (if underflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
#else
	ldrd	r6, r7, [r0, #8]
#endif
	adds	r4, r4, r12
	adcs	r5, r5, r3
	adcs	r6, r6, r3
	adcs	r7, r7, r3
	adcs	r8, r8, r3
	adcs	r9, r9, r3
	adcs	r10, r10, r3
	adc	r11, r11, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #8]
	str	r7, [r0, #12]
#else
	strd	r6, r7, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #24]
	str	r11, [r0, #28]
#else
	strd	r10, r11, [r0, #24]
#endif
	ldr	r2, [sp, #160]
	ldr	r1, [sp]
	ldr	r0, [sp, #8]
	bl	fe_mul
	ldr	r2, [sp, #156]
	ldr	r1, [sp, #4]
	ldr	r0, [sp, #4]
	bl	fe_mul
	ldr	r2, [sp, #144]
	ldr	r1, [sp, #152]
	ldr	r0, [sp, #12]
	bl	fe_mul
	ldr	r2, [sp, #148]
	ldr	r1, [sp, #140]
	ldr	r0, [sp]
	bl	fe_mul
	add	r0, sp, #16
	ldr	r1, [sp]
	# Double
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1]
	ldr	r5, [r1, #4]
#else
	ldrd	r4, r5, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r1, #8]
	ldr	r7, [r1, #12]
#else
	ldrd	r6, r7, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r1, #16]
	ldr	r9, [r1, #20]
#else
	ldrd	r8, r9, [r1, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r1, #24]
	ldr	r11, [r1, #28]
#else
	ldrd	r10, r11, [r1, #24]
#endif
	adds	r4, r4, r4
	adcs	r5, r5, r5
	adcs	r6, r6, r6
	adcs	r7, r7, r7
	adcs	r8, r8, r8
	adcs	r9, r9, r9
	adcs	r10, r10, r10
	adc	r11, r11, r11
	mov	r12, #-19
	asr	r3, r11, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Sub modulus (if overflow)
	subs	r4, r4, r12
	sbcs	r5, r5, r3
	sbcs	r6, r6, r3
	sbcs	r7, r7, r3
	sbcs	r8, r8, r3
	sbcs	r9, r9, r3
	sbcs	r10, r10, r3
	sbc	r11, r11, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #8]
	str	r7, [r0, #12]
#else
	strd	r6, r7, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r0, #24]
	str	r11, [r0, #28]
#else
	strd	r10, r11, [r0, #24]
#endif
	ldr	r0, [sp, #4]
	ldr	r1, [sp]
	ldr	r2, [sp, #8]
	# Add-Sub
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r2]
	ldr	r5, [r2, #4]
#else
	ldrd	r4, r5, [r2]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0]
	ldr	r7, [r0, #4]
#else
	ldrd	r6, r7, [r0]
#endif
	adds	r8, r4, r6
	mov	r12, #0
	adcs	r9, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0]
	str	r9, [r0, #4]
#else
	strd	r8, r9, [r0]
#endif
	#  Sub
	subs	r10, r4, r6
	mov	lr, #0
	sbcs	r11, r5, r7
	adc	lr, lr, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1]
	str	r11, [r1, #4]
#else
	strd	r10, r11, [r1]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r2, #8]
	ldr	r5, [r2, #12]
#else
	ldrd	r4, r5, [r2, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
#else
	ldrd	r6, r7, [r0, #8]
#endif
	adds	r12, r12, #-1
	adcs	r8, r4, r6
	mov	r12, #0
	adcs	r9, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #8]
	str	r9, [r0, #12]
#else
	strd	r8, r9, [r0, #8]
#endif
	#  Sub
	adds	lr, lr, #-1
	sbcs	r10, r4, r6
	mov	lr, #0
	sbcs	r11, r5, r7
	adc	lr, lr, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1, #8]
	str	r11, [r1, #12]
#else
	strd	r10, r11, [r1, #8]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r2, #16]
	ldr	r5, [r2, #20]
#else
	ldrd	r4, r5, [r2, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #16]
	ldr	r7, [r0, #20]
#else
	ldrd	r6, r7, [r0, #16]
#endif
	adds	r12, r12, #-1
	adcs	r8, r4, r6
	mov	r12, #0
	adcs	r9, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
	#  Sub
	adds	lr, lr, #-1
	sbcs	r10, r4, r6
	mov	lr, #0
	sbcs	r11, r5, r7
	adc	lr, lr, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1, #16]
	str	r11, [r1, #20]
#else
	strd	r10, r11, [r1, #16]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r2, #24]
	ldr	r5, [r2, #28]
#else
	ldrd	r4, r5, [r2, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #24]
	ldr	r7, [r0, #28]
#else
	ldrd	r6, r7, [r0, #24]
#endif
	adds	r12, r12, #-1
	adcs	r8, r4, r6
	adc	r9, r5, r7
	#  Sub
	adds	lr, lr, #-1
	sbcs	r10, r4, r6
	sbc	r11, r5, r7
	mov	r12, #-19
	asr	r3, r9, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Sub modulus (if overflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
	subs	r4, r4, r12
	sbcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #8]
	ldr	r5, [r0, #12]
#else
	ldrd	r4, r5, [r0, #8]
#endif
	sbcs	r4, r4, r3
	sbcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #8]
	str	r5, [r0, #12]
#else
	strd	r4, r5, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #16]
	ldr	r5, [r0, #20]
#else
	ldrd	r4, r5, [r0, #16]
#endif
	sbcs	r4, r4, r3
	sbcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #16]
	str	r5, [r0, #20]
#else
	strd	r4, r5, [r0, #16]
#endif
	sbcs	r8, r8, r3
	sbc	r9, r9, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #24]
	str	r9, [r0, #28]
#else
	strd	r8, r9, [r0, #24]
#endif
	mov	r12, #-19
	asr	r3, r11, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Add modulus (if underflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1]
	ldr	r5, [r1, #4]
#else
	ldrd	r4, r5, [r1]
#endif
	adds	r4, r4, r12
	adcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r1]
	str	r5, [r1, #4]
#else
	strd	r4, r5, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #8]
	ldr	r5, [r1, #12]
#else
	ldrd	r4, r5, [r1, #8]
#endif
	adcs	r4, r4, r3
	adcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r1, #8]
	str	r5, [r1, #12]
#else
	strd	r4, r5, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #16]
	ldr	r5, [r1, #20]
#else
	ldrd	r4, r5, [r1, #16]
#endif
	adcs	r4, r4, r3
	adcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r1, #16]
	str	r5, [r1, #20]
#else
	strd	r4, r5, [r1, #16]
#endif
	adcs	r10, r10, r3
	adc	r11, r11, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1, #24]
	str	r11, [r1, #28]
#else
	strd	r10, r11, [r1, #24]
#endif
	ldr	r0, [sp, #12]
	ldr	r1, [sp, #8]
	add	r2, sp, #16
	# Add-Sub
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r2]
	ldr	r5, [r2, #4]
#else
	ldrd	r4, r5, [r2]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0]
	ldr	r7, [r0, #4]
#else
	ldrd	r6, r7, [r0]
#endif
	adds	r8, r4, r6
	mov	r12, #0
	adcs	r9, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0]
	str	r9, [r0, #4]
#else
	strd	r8, r9, [r0]
#endif
	#  Sub
	subs	r10, r4, r6
	mov	lr, #0
	sbcs	r11, r5, r7
	adc	lr, lr, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1]
	str	r11, [r1, #4]
#else
	strd	r10, r11, [r1]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r2, #8]
	ldr	r5, [r2, #12]
#else
	ldrd	r4, r5, [r2, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
#else
	ldrd	r6, r7, [r0, #8]
#endif
	adds	r12, r12, #-1
	adcs	r8, r4, r6
	mov	r12, #0
	adcs	r9, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #8]
	str	r9, [r0, #12]
#else
	strd	r8, r9, [r0, #8]
#endif
	#  Sub
	adds	lr, lr, #-1
	sbcs	r10, r4, r6
	mov	lr, #0
	sbcs	r11, r5, r7
	adc	lr, lr, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1, #8]
	str	r11, [r1, #12]
#else
	strd	r10, r11, [r1, #8]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r2, #16]
	ldr	r5, [r2, #20]
#else
	ldrd	r4, r5, [r2, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #16]
	ldr	r7, [r0, #20]
#else
	ldrd	r6, r7, [r0, #16]
#endif
	adds	r12, r12, #-1
	adcs	r8, r4, r6
	mov	r12, #0
	adcs	r9, r5, r7
	adc	r12, r12, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #16]
	str	r9, [r0, #20]
#else
	strd	r8, r9, [r0, #16]
#endif
	#  Sub
	adds	lr, lr, #-1
	sbcs	r10, r4, r6
	mov	lr, #0
	sbcs	r11, r5, r7
	adc	lr, lr, #0
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1, #16]
	str	r11, [r1, #20]
#else
	strd	r10, r11, [r1, #16]
#endif
	#  Add
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r2, #24]
	ldr	r5, [r2, #28]
#else
	ldrd	r4, r5, [r2, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #24]
	ldr	r7, [r0, #28]
#else
	ldrd	r6, r7, [r0, #24]
#endif
	adds	r12, r12, #-1
	adcs	r8, r4, r6
	adc	r9, r5, r7
	#  Sub
	adds	lr, lr, #-1
	sbcs	r10, r4, r6
	sbc	r11, r5, r7
	mov	r12, #-19
	asr	r3, r9, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Sub modulus (if overflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
	subs	r4, r4, r12
	sbcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #8]
	ldr	r5, [r0, #12]
#else
	ldrd	r4, r5, [r0, #8]
#endif
	sbcs	r4, r4, r3
	sbcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #8]
	str	r5, [r0, #12]
#else
	strd	r4, r5, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #16]
	ldr	r5, [r0, #20]
#else
	ldrd	r4, r5, [r0, #16]
#endif
	sbcs	r4, r4, r3
	sbcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #16]
	str	r5, [r0, #20]
#else
	strd	r4, r5, [r0, #16]
#endif
	sbcs	r8, r8, r3
	sbc	r9, r9, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [r0, #24]
	str	r9, [r0, #28]
#else
	strd	r8, r9, [r0, #24]
#endif
	mov	r12, #-19
	asr	r3, r11, #31
	#   Mask the modulus
	and	r12, r3, r12
	and	lr, r3, #0x7fffffff
	#   Add modulus (if underflow)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1]
	ldr	r5, [r1, #4]
#else
	ldrd	r4, r5, [r1]
#endif
	adds	r4, r4, r12
	adcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r1]
	str	r5, [r1, #4]
#else
	strd	r4, r5, [r1]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #8]
	ldr	r5, [r1, #12]
#else
	ldrd	r4, r5, [r1, #8]
#endif
	adcs	r4, r4, r3
	adcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r1, #8]
	str	r5, [r1, #12]
#else
	strd	r4, r5, [r1, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r1, #16]
	ldr	r5, [r1, #20]
#else
	ldrd	r4, r5, [r1, #16]
#endif
	adcs	r4, r4, r3
	adcs	r5, r5, r3
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r1, #16]
	str	r5, [r1, #20]
#else
	strd	r4, r5, [r1, #16]
#endif
	adcs	r10, r10, r3
	adc	r11, r11, lr
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [r1, #24]
	str	r11, [r1, #28]
#else
	strd	r10, r11, [r1, #24]
#endif
	add	sp, sp, #0x60
	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
	.size	fe_ge_sub,.-fe_ge_sub

#endif /* HAVE_CURVE25519 */
#endif /* !__aarch64__ */
#endif /* WOLFSSL_ARMASM */

#if defined(__linux__) && defined(__ELF__)
.section	.note.GNU-stack,"",%progbits
#endif
