/* armv8-curve25519
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

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif /* HAVE_CONFIG_H */
#include <wolfssl/wolfcrypt/settings.h>

/* Generated using (from wolfssl):
 *   cd ../scripts
 *   ruby ./x25519/x25519.rb arm64 ../wolfssl/wolfcrypt/src/port/arm/armv8-curve25519.S
 */
#ifdef WOLFSSL_ARMASM
#ifdef __aarch64__
#ifdef HAVE_CURVE25519
#ifndef __APPLE__
.text
.globl	fe_init
.type	fe_init,@function
.align	2
fe_init:
#else
.section	__TEXT,__text
.globl	_fe_init
.p2align	2
_fe_init:
#endif /* __APPLE__ */
	ret
#ifndef __APPLE__
	.size	fe_init,.-fe_init
#endif /* __APPLE__ */
#ifndef __APPLE__
.text
.globl	fe_frombytes
.type	fe_frombytes,@function
.align	2
fe_frombytes:
#else
.section	__TEXT,__text
.globl	_fe_frombytes
.p2align	2
_fe_frombytes:
#endif /* __APPLE__ */
	ldp	x2, x3, [x1]
	ldp	x4, x5, [x1, #16]
	and	x5, x5, #0x7fffffffffffffff
	stp	x2, x3, [x0]
	stp	x4, x5, [x0, #16]
	ret
#ifndef __APPLE__
	.size	fe_frombytes,.-fe_frombytes
#endif /* __APPLE__ */
#ifndef __APPLE__
.text
.globl	fe_tobytes
.type	fe_tobytes,@function
.align	2
fe_tobytes:
#else
.section	__TEXT,__text
.globl	_fe_tobytes
.p2align	2
_fe_tobytes:
#endif /* __APPLE__ */
	mov	x7, #19
	ldp	x2, x3, [x1]
	ldp	x4, x5, [x1, #16]
	adds	x6, x2, x7
	adcs	x6, x3, xzr
	adcs	x6, x4, xzr
	adc	x6, x5, xzr
	and	x6, x7, x6, asr 63
	adds	x2, x2, x6
	adcs	x3, x3, xzr
	adcs	x4, x4, xzr
	adc	x5, x5, xzr
	and	x5, x5, #0x7fffffffffffffff
	stp	x2, x3, [x0]
	stp	x4, x5, [x0, #16]
	ret
#ifndef __APPLE__
	.size	fe_tobytes,.-fe_tobytes
#endif /* __APPLE__ */
#ifndef __APPLE__
.text
.globl	fe_1
.type	fe_1,@function
.align	2
fe_1:
#else
.section	__TEXT,__text
.globl	_fe_1
.p2align	2
_fe_1:
#endif /* __APPLE__ */
	# Set one
	mov	x1, #1
	stp	x1, xzr, [x0]
	stp	xzr, xzr, [x0, #16]
	ret
#ifndef __APPLE__
	.size	fe_1,.-fe_1
#endif /* __APPLE__ */
#ifndef __APPLE__
.text
.globl	fe_0
.type	fe_0,@function
.align	2
fe_0:
#else
.section	__TEXT,__text
.globl	_fe_0
.p2align	2
_fe_0:
#endif /* __APPLE__ */
	# Set zero
	stp	xzr, xzr, [x0]
	stp	xzr, xzr, [x0, #16]
	ret
#ifndef __APPLE__
	.size	fe_0,.-fe_0
#endif /* __APPLE__ */
#ifndef __APPLE__
.text
.globl	fe_copy
.type	fe_copy,@function
.align	2
fe_copy:
#else
.section	__TEXT,__text
.globl	_fe_copy
.p2align	2
_fe_copy:
#endif /* __APPLE__ */
	# Copy
	ldp	x2, x3, [x1]
	ldp	x4, x5, [x1, #16]
	stp	x2, x3, [x0]
	stp	x4, x5, [x0, #16]
	ret
#ifndef __APPLE__
	.size	fe_copy,.-fe_copy
#endif /* __APPLE__ */
#ifndef __APPLE__
.text
.globl	fe_sub
.type	fe_sub,@function
.align	2
fe_sub:
#else
.section	__TEXT,__text
.globl	_fe_sub
.p2align	2
_fe_sub:
#endif /* __APPLE__ */
	# Sub
	ldp	x3, x4, [x1]
	ldp	x5, x6, [x1, #16]
	ldp	x7, x8, [x2]
	ldp	x9, x10, [x2, #16]
	subs	x3, x3, x7
	sbcs	x4, x4, x8
	sbcs	x5, x5, x9
	sbcs	x6, x6, x10
	mov	x12, #-19
	csetm	x11, cc
	#   Mask the modulus
	and	x12, x11, x12
	and	x13, x11, #0x7fffffffffffffff
	#   Add modulus (if underflow)
	adds	x3, x3, x12
	adcs	x4, x4, x11
	adcs	x5, x5, x11
	adc	x6, x6, x13
	stp	x3, x4, [x0]
	stp	x5, x6, [x0, #16]
	ret
#ifndef __APPLE__
	.size	fe_sub,.-fe_sub
#endif /* __APPLE__ */
#ifndef __APPLE__
.text
.globl	fe_add
.type	fe_add,@function
.align	2
fe_add:
#else
.section	__TEXT,__text
.globl	_fe_add
.p2align	2
_fe_add:
#endif /* __APPLE__ */
	# Add
	ldp	x3, x4, [x1]
	ldp	x5, x6, [x1, #16]
	ldp	x7, x8, [x2]
	ldp	x9, x10, [x2, #16]
	adds	x3, x3, x7
	adcs	x4, x4, x8
	adcs	x5, x5, x9
	adc	x6, x6, x10
	mov	x12, #-19
	asr	x11, x6, #63
	#   Mask the modulus
	and	x12, x11, x12
	and	x13, x11, #0x7fffffffffffffff
	#   Sub modulus (if overflow)
	subs	x3, x3, x12
	sbcs	x4, x4, x11
	sbcs	x5, x5, x11
	sbc	x6, x6, x13
	stp	x3, x4, [x0]
	stp	x5, x6, [x0, #16]
	ret
#ifndef __APPLE__
	.size	fe_add,.-fe_add
#endif /* __APPLE__ */
#ifndef __APPLE__
.text
.globl	fe_neg
.type	fe_neg,@function
.align	2
fe_neg:
#else
.section	__TEXT,__text
.globl	_fe_neg
.p2align	2
_fe_neg:
#endif /* __APPLE__ */
	ldp	x2, x3, [x1]
	ldp	x4, x5, [x1, #16]
	mov	x6, #-19
	mov	x7, #-1
	mov	x8, #-1
	mov	x9, #0x7fffffffffffffff
	subs	x6, x6, x2
	sbcs	x7, x7, x3
	sbcs	x8, x8, x4
	sbc	x9, x9, x5
	stp	x6, x7, [x0]
	stp	x8, x9, [x0, #16]
	ret
#ifndef __APPLE__
	.size	fe_neg,.-fe_neg
#endif /* __APPLE__ */
#ifndef __APPLE__
.text
.globl	fe_isnonzero
.type	fe_isnonzero,@function
.align	2
fe_isnonzero:
#else
.section	__TEXT,__text
.globl	_fe_isnonzero
.p2align	2
_fe_isnonzero:
#endif /* __APPLE__ */
	mov	x6, #19
	ldp	x1, x2, [x0]
	ldp	x3, x4, [x0, #16]
	adds	x5, x1, x6
	adcs	x5, x2, xzr
	adcs	x5, x3, xzr
	adc	x5, x4, xzr
	and	x5, x6, x5, asr 63
	adds	x1, x1, x5
	adcs	x2, x2, xzr
	adcs	x3, x3, xzr
	adc	x4, x4, xzr
	and	x4, x4, #0x7fffffffffffffff
	orr	x0, x1, x2
	orr	x3, x3, x4
	orr	x0, x0, x3
	ret
#ifndef __APPLE__
	.size	fe_isnonzero,.-fe_isnonzero
#endif /* __APPLE__ */
#ifndef __APPLE__
.text
.globl	fe_isnegative
.type	fe_isnegative,@function
.align	2
fe_isnegative:
#else
.section	__TEXT,__text
.globl	_fe_isnegative
.p2align	2
_fe_isnegative:
#endif /* __APPLE__ */
	mov	x6, #19
	ldp	x1, x2, [x0]
	ldp	x3, x4, [x0, #16]
	adds	x5, x1, x6
	adcs	x5, x2, xzr
	adcs	x5, x3, xzr
	adc	x5, x4, xzr
	and	x0, x1, #1
	eor	x0, x0, x5, lsr 63
	ret
#ifndef __APPLE__
	.size	fe_isnegative,.-fe_isnegative
#endif /* __APPLE__ */
#ifndef __APPLE__
.text
.globl	fe_cmov_table
.type	fe_cmov_table,@function
.align	2
fe_cmov_table:
#else
.section	__TEXT,__text
.globl	_fe_cmov_table
.p2align	2
_fe_cmov_table:
#endif /* __APPLE__ */
	stp	x29, x30, [sp, #-128]!
	add	x29, sp, #0
	str	x17, [x29, #40]
	str	x19, [x29, #48]
	stp	x20, x21, [x29, #56]
	stp	x22, x23, [x29, #72]
	stp	x24, x25, [x29, #88]
	stp	x26, x27, [x29, #104]
	str	x28, [x29, #120]
	str	x0, [x29, #16]
	sxtb	x2, w2
	sbfx	x3, x2, #7, #1
	eor	x0, x2, x3
	sub	x0, x0, x3
	mov	x4, #1
	mov	x5, xzr
	mov	x6, xzr
	mov	x7, xzr
	mov	x8, #1
	mov	x9, xzr
	mov	x10, xzr
	mov	x11, xzr
	mov	x12, xzr
	mov	x13, xzr
	mov	x14, xzr
	mov	x15, xzr
	cmp	x0, #1
	ldp	x16, x17, [x1]
	ldp	x19, x20, [x1, #16]
	ldp	x21, x22, [x1, #32]
	ldp	x23, x24, [x1, #48]
	ldp	x25, x26, [x1, #64]
	ldp	x27, x28, [x1, #80]
	csel	x4, x16, x4, eq
	csel	x5, x17, x5, eq
	csel	x6, x19, x6, eq
	csel	x7, x20, x7, eq
	csel	x8, x21, x8, eq
	csel	x9, x22, x9, eq
	csel	x10, x23, x10, eq
	csel	x11, x24, x11, eq
	csel	x12, x25, x12, eq
	csel	x13, x26, x13, eq
	csel	x14, x27, x14, eq
	csel	x15, x28, x15, eq
	cmp	x0, #2
	ldp	x16, x17, [x1, #96]
	ldp	x19, x20, [x1, #112]
	ldp	x21, x22, [x1, #128]
	ldp	x23, x24, [x1, #144]
	ldp	x25, x26, [x1, #160]
	ldp	x27, x28, [x1, #176]
	csel	x4, x16, x4, eq
	csel	x5, x17, x5, eq
	csel	x6, x19, x6, eq
	csel	x7, x20, x7, eq
	csel	x8, x21, x8, eq
	csel	x9, x22, x9, eq
	csel	x10, x23, x10, eq
	csel	x11, x24, x11, eq
	csel	x12, x25, x12, eq
	csel	x13, x26, x13, eq
	csel	x14, x27, x14, eq
	csel	x15, x28, x15, eq
	cmp	x0, #3
	ldp	x16, x17, [x1, #192]
	ldp	x19, x20, [x1, #208]
	ldp	x21, x22, [x1, #224]
	ldp	x23, x24, [x1, #240]
	ldp	x25, x26, [x1, #256]
	ldp	x27, x28, [x1, #272]
	csel	x4, x16, x4, eq
	csel	x5, x17, x5, eq
	csel	x6, x19, x6, eq
	csel	x7, x20, x7, eq
	csel	x8, x21, x8, eq
	csel	x9, x22, x9, eq
	csel	x10, x23, x10, eq
	csel	x11, x24, x11, eq
	csel	x12, x25, x12, eq
	csel	x13, x26, x13, eq
	csel	x14, x27, x14, eq
	csel	x15, x28, x15, eq
	cmp	x0, #4
	ldp	x16, x17, [x1, #288]
	ldp	x19, x20, [x1, #304]
	ldp	x21, x22, [x1, #320]
	ldp	x23, x24, [x1, #336]
	ldp	x25, x26, [x1, #352]
	ldp	x27, x28, [x1, #368]
	csel	x4, x16, x4, eq
	csel	x5, x17, x5, eq
	csel	x6, x19, x6, eq
	csel	x7, x20, x7, eq
	csel	x8, x21, x8, eq
	csel	x9, x22, x9, eq
	csel	x10, x23, x10, eq
	csel	x11, x24, x11, eq
	csel	x12, x25, x12, eq
	csel	x13, x26, x13, eq
	csel	x14, x27, x14, eq
	csel	x15, x28, x15, eq
	add	x1, x1, #0x180
	cmp	x0, #5
	ldp	x16, x17, [x1]
	ldp	x19, x20, [x1, #16]
	ldp	x21, x22, [x1, #32]
	ldp	x23, x24, [x1, #48]
	ldp	x25, x26, [x1, #64]
	ldp	x27, x28, [x1, #80]
	csel	x4, x16, x4, eq
	csel	x5, x17, x5, eq
	csel	x6, x19, x6, eq
	csel	x7, x20, x7, eq
	csel	x8, x21, x8, eq
	csel	x9, x22, x9, eq
	csel	x10, x23, x10, eq
	csel	x11, x24, x11, eq
	csel	x12, x25, x12, eq
	csel	x13, x26, x13, eq
	csel	x14, x27, x14, eq
	csel	x15, x28, x15, eq
	cmp	x0, #6
	ldp	x16, x17, [x1, #96]
	ldp	x19, x20, [x1, #112]
	ldp	x21, x22, [x1, #128]
	ldp	x23, x24, [x1, #144]
	ldp	x25, x26, [x1, #160]
	ldp	x27, x28, [x1, #176]
	csel	x4, x16, x4, eq
	csel	x5, x17, x5, eq
	csel	x6, x19, x6, eq
	csel	x7, x20, x7, eq
	csel	x8, x21, x8, eq
	csel	x9, x22, x9, eq
	csel	x10, x23, x10, eq
	csel	x11, x24, x11, eq
	csel	x12, x25, x12, eq
	csel	x13, x26, x13, eq
	csel	x14, x27, x14, eq
	csel	x15, x28, x15, eq
	cmp	x0, #7
	ldp	x16, x17, [x1, #192]
	ldp	x19, x20, [x1, #208]
	ldp	x21, x22, [x1, #224]
	ldp	x23, x24, [x1, #240]
	ldp	x25, x26, [x1, #256]
	ldp	x27, x28, [x1, #272]
	csel	x4, x16, x4, eq
	csel	x5, x17, x5, eq
	csel	x6, x19, x6, eq
	csel	x7, x20, x7, eq
	csel	x8, x21, x8, eq
	csel	x9, x22, x9, eq
	csel	x10, x23, x10, eq
	csel	x11, x24, x11, eq
	csel	x12, x25, x12, eq
	csel	x13, x26, x13, eq
	csel	x14, x27, x14, eq
	csel	x15, x28, x15, eq
	cmp	x0, #8
	ldp	x16, x17, [x1, #288]
	ldp	x19, x20, [x1, #304]
	ldp	x21, x22, [x1, #320]
	ldp	x23, x24, [x1, #336]
	ldp	x25, x26, [x1, #352]
	ldp	x27, x28, [x1, #368]
	csel	x4, x16, x4, eq
	csel	x5, x17, x5, eq
	csel	x6, x19, x6, eq
	csel	x7, x20, x7, eq
	csel	x8, x21, x8, eq
	csel	x9, x22, x9, eq
	csel	x10, x23, x10, eq
	csel	x11, x24, x11, eq
	csel	x12, x25, x12, eq
	csel	x13, x26, x13, eq
	csel	x14, x27, x14, eq
	csel	x15, x28, x15, eq
	mov	x16, #-19
	mov	x17, #-1
	mov	x19, #-1
	mov	x20, #0x7fffffffffffffff
	subs	x16, x16, x12
	sbcs	x17, x17, x13
	sbcs	x19, x19, x14
	sbc	x20, x20, x15
	cmp	x2, #0
	mov	x3, x4
	csel	x4, x8, x4, lt
	csel	x8, x3, x8, lt
	mov	x3, x5
	csel	x5, x9, x5, lt
	csel	x9, x3, x9, lt
	mov	x3, x6
	csel	x6, x10, x6, lt
	csel	x10, x3, x10, lt
	mov	x3, x7
	csel	x7, x11, x7, lt
	csel	x11, x3, x11, lt
	csel	x12, x16, x12, lt
	csel	x13, x17, x13, lt
	csel	x14, x19, x14, lt
	csel	x15, x20, x15, lt
	ldr	x0, [x29, #16]
	stp	x4, x5, [x0]
	stp	x6, x7, [x0, #16]
	stp	x8, x9, [x0, #32]
	stp	x10, x11, [x0, #48]
	stp	x12, x13, [x0, #64]
	stp	x14, x15, [x0, #80]
	ldr	x17, [x29, #40]
	ldr	x19, [x29, #48]
	ldp	x20, x21, [x29, #56]
	ldp	x22, x23, [x29, #72]
	ldp	x24, x25, [x29, #88]
	ldp	x26, x27, [x29, #104]
	ldr	x28, [x29, #120]
	ldp	x29, x30, [sp], #0x80
	ret
#ifndef __APPLE__
	.size	fe_cmov_table,.-fe_cmov_table
#endif /* __APPLE__ */
#ifndef __APPLE__
.text
.globl	fe_mul
.type	fe_mul,@function
.align	2
fe_mul:
#else
.section	__TEXT,__text
.globl	_fe_mul
.p2align	2
_fe_mul:
#endif /* __APPLE__ */
	stp	x29, x30, [sp, #-64]!
	add	x29, sp, #0
	str	x17, [x29, #24]
	str	x19, [x29, #32]
	stp	x20, x21, [x29, #40]
	str	x22, [x29, #56]
	# Multiply
	ldp	x14, x15, [x1]
	ldp	x16, x17, [x1, #16]
	ldp	x19, x20, [x2]
	ldp	x21, x22, [x2, #16]
	#  A[0] * B[0]
	mul	x6, x14, x19
	umulh	x7, x14, x19
	#  A[0] * B[1]
	mul	x3, x14, x20
	umulh	x8, x14, x20
	adds	x7, x7, x3
	adc	x8, x8, xzr
	#  A[1] * B[0]
	mul	x3, x15, x19
	umulh	x4, x15, x19
	adds	x7, x7, x3
	adcs	x8, x8, x4
	adc	x9, xzr, xzr
	#  A[0] * B[2]
	mul	x3, x14, x21
	umulh	x4, x14, x21
	adds	x8, x8, x3
	adc	x9, x9, x4
	#  A[1] * B[1]
	mul	x3, x15, x20
	umulh	x4, x15, x20
	adds	x8, x8, x3
	adcs	x9, x9, x4
	adc	x10, xzr, xzr
	#  A[2] * B[0]
	mul	x3, x16, x19
	umulh	x4, x16, x19
	adds	x8, x8, x3
	adcs	x9, x9, x4
	adc	x10, x10, xzr
	#  A[0] * B[3]
	mul	x3, x14, x22
	umulh	x4, x14, x22
	adds	x9, x9, x3
	adcs	x10, x10, x4
	adc	x11, xzr, xzr
	#  A[1] * B[2]
	mul	x3, x15, x21
	umulh	x4, x15, x21
	adds	x9, x9, x3
	adcs	x10, x10, x4
	adc	x11, x11, xzr
	#  A[2] * B[1]
	mul	x3, x16, x20
	umulh	x4, x16, x20
	adds	x9, x9, x3
	adcs	x10, x10, x4
	adc	x11, x11, xzr
	#  A[3] * B[0]
	mul	x3, x17, x19
	umulh	x4, x17, x19
	adds	x9, x9, x3
	adcs	x10, x10, x4
	adc	x11, x11, xzr
	#  A[1] * B[3]
	mul	x3, x15, x22
	umulh	x4, x15, x22
	adds	x10, x10, x3
	adcs	x11, x11, x4
	adc	x12, xzr, xzr
	#  A[2] * B[2]
	mul	x3, x16, x21
	umulh	x4, x16, x21
	adds	x10, x10, x3
	adcs	x11, x11, x4
	adc	x12, x12, xzr
	#  A[3] * B[1]
	mul	x3, x17, x20
	umulh	x4, x17, x20
	adds	x10, x10, x3
	adcs	x11, x11, x4
	adc	x12, x12, xzr
	#  A[2] * B[3]
	mul	x3, x16, x22
	umulh	x4, x16, x22
	adds	x11, x11, x3
	adcs	x12, x12, x4
	adc	x13, xzr, xzr
	#  A[3] * B[2]
	mul	x3, x17, x21
	umulh	x4, x17, x21
	adds	x11, x11, x3
	adcs	x12, x12, x4
	adc	x13, x13, xzr
	#  A[3] * B[3]
	mul	x3, x17, x22
	umulh	x4, x17, x22
	adds	x12, x12, x3
	adc	x13, x13, x4
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x13, x13, x12, #63
	extr	x12, x12, x11, #63
	extr	x11, x11, x10, #63
	extr	x10, x10, x9, #63
	and	x9, x9, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x3, #19
	mul	x4, x3, x10
	umulh	x10, x3, x10
	adds	x6, x6, x4
	mul	x4, x3, x11
	umulh	x11, x3, x11
	adcs	x7, x7, x4
	mul	x4, x3, x12
	umulh	x12, x3, x12
	adcs	x8, x8, x4
	mul	x4, x3, x13
	umulh	x5, x3, x13
	adcs	x9, x9, x4
	adc	x5, x5, xzr
	#  Add remaining product results in
	adds	x7, x7, x10
	adcs	x8, x8, x11
	adcs	x9, x9, x12
	adc	x5, x5, xzr
	#  Overflow
	extr	x5, x5, x9, #63
	mul	x5, x5, x3
	and	x9, x9, #0x7fffffffffffffff
	adds	x6, x6, x5
	adcs	x7, x7, xzr
	adcs	x8, x8, xzr
	adc	x9, x9, xzr
	# Reduce if top bit set
	and	x5, x3, x9, asr 63
	and	x9, x9, #0x7fffffffffffffff
	adds	x6, x6, x5
	adcs	x7, x7, xzr
	adcs	x8, x8, xzr
	adc	x9, x9, xzr
	# Store
	stp	x6, x7, [x0]
	stp	x8, x9, [x0, #16]
	ldr	x17, [x29, #24]
	ldr	x19, [x29, #32]
	ldp	x20, x21, [x29, #40]
	ldr	x22, [x29, #56]
	ldp	x29, x30, [sp], #0x40
	ret
#ifndef __APPLE__
	.size	fe_mul,.-fe_mul
#endif /* __APPLE__ */
#ifndef __APPLE__
.text
.globl	fe_sq
.type	fe_sq,@function
.align	2
fe_sq:
#else
.section	__TEXT,__text
.globl	_fe_sq
.p2align	2
_fe_sq:
#endif /* __APPLE__ */
	# Square
	ldp	x13, x14, [x1]
	ldp	x15, x16, [x1, #16]
	#  A[0] * A[1]
	mul	x6, x13, x14
	umulh	x7, x13, x14
	#  A[0] * A[2]
	mul	x2, x13, x15
	umulh	x8, x13, x15
	adds	x7, x7, x2
	adc	x8, x8, xzr
	#  A[0] * A[3]
	mul	x2, x13, x16
	umulh	x9, x13, x16
	adds	x8, x8, x2
	adc	x9, x9, xzr
	#  A[1] * A[2]
	mul	x2, x14, x15
	umulh	x3, x14, x15
	adds	x8, x8, x2
	adcs	x9, x9, x3
	adc	x10, xzr, xzr
	#  A[1] * A[3]
	mul	x2, x14, x16
	umulh	x3, x14, x16
	adds	x9, x9, x2
	adc	x10, x10, x3
	#  A[2] * A[3]
	mul	x2, x15, x16
	umulh	x11, x15, x16
	adds	x10, x10, x2
	adc	x11, x11, xzr
	# Double
	adds	x6, x6, x6
	adcs	x7, x7, x7
	adcs	x8, x8, x8
	adcs	x9, x9, x9
	adcs	x10, x10, x10
	adcs	x11, x11, x11
	adc	x12, xzr, xzr
	#  A[0] * A[0]
	mul	x5, x13, x13
	umulh	x4, x13, x13
	#  A[1] * A[1]
	mul	x2, x14, x14
	umulh	x3, x14, x14
	adds	x6, x6, x4
	adcs	x7, x7, x2
	adc	x4, x3, xzr
	#  A[2] * A[2]
	mul	x2, x15, x15
	umulh	x3, x15, x15
	adds	x8, x8, x4
	adcs	x9, x9, x2
	adc	x4, x3, xzr
	#  A[3] * A[3]
	mul	x2, x16, x16
	umulh	x3, x16, x16
	adds	x10, x10, x4
	adcs	x11, x11, x2
	adc	x12, x12, x3
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x12, x12, x11, #63
	extr	x11, x11, x10, #63
	extr	x10, x10, x9, #63
	extr	x9, x9, x8, #63
	and	x8, x8, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x2, #19
	mul	x3, x2, x9
	umulh	x9, x2, x9
	adds	x5, x5, x3
	mul	x3, x2, x10
	umulh	x10, x2, x10
	adcs	x6, x6, x3
	mul	x3, x2, x11
	umulh	x11, x2, x11
	adcs	x7, x7, x3
	mul	x3, x2, x12
	umulh	x4, x2, x12
	adcs	x8, x8, x3
	adc	x4, x4, xzr
	#  Add remaining product results in
	adds	x6, x6, x9
	adcs	x7, x7, x10
	adcs	x8, x8, x11
	adc	x4, x4, xzr
	#  Overflow
	extr	x4, x4, x8, #63
	mul	x4, x4, x2
	and	x8, x8, #0x7fffffffffffffff
	adds	x5, x5, x4
	adcs	x6, x6, xzr
	adcs	x7, x7, xzr
	adc	x8, x8, xzr
	# Reduce if top bit set
	and	x4, x2, x8, asr 63
	and	x8, x8, #0x7fffffffffffffff
	adds	x5, x5, x4
	adcs	x6, x6, xzr
	adcs	x7, x7, xzr
	adc	x8, x8, xzr
	# Store
	stp	x5, x6, [x0]
	stp	x7, x8, [x0, #16]
	ret
#ifndef __APPLE__
	.size	fe_sq,.-fe_sq
#endif /* __APPLE__ */
#ifndef __APPLE__
.text
.globl	fe_invert
.type	fe_invert,@function
.align	2
fe_invert:
#else
.section	__TEXT,__text
.globl	_fe_invert
.p2align	2
_fe_invert:
#endif /* __APPLE__ */
	stp	x29, x30, [sp, #-176]!
	add	x29, sp, #0
	str	x20, [x29, #168]
	# Invert
	str	x0, [x29, #144]
	str	x1, [x29, #152]
	add	x0, x29, #16
#ifndef NDEBUG
	ldr	x1, [x29, #152]
#endif /* !NDEBUG */
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	add	x0, x29, #48
	add	x1, x29, #16
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
#ifndef NDEBUG
	add	x0, x29, #48
#endif /* !NDEBUG */
	add	x1, x29, #48
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
#ifndef NDEBUG
	add	x0, x29, #48
#endif /* !NDEBUG */
	ldr	x1, [x29, #152]
	add	x2, x29, #48
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	add	x0, x29, #16
	add	x1, x29, #16
	add	x2, x29, #48
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	add	x0, x29, #0x50
#ifndef NDEBUG
	add	x1, x29, #16
#endif /* !NDEBUG */
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	add	x0, x29, #48
	add	x1, x29, #48
	add	x2, x29, #0x50
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	add	x0, x29, #0x50
#ifndef NDEBUG
	add	x1, x29, #48
#endif /* !NDEBUG */
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	mov	x20, #3
#ifndef NDEBUG
	add	x0, x29, #0x50
#endif /* !NDEBUG */
	add	x1, x29, #0x50
L_fe_invert1:
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	subs	x20, x20, #1
	bcs	L_fe_invert1
	add	x0, x29, #48
#ifndef NDEBUG
	add	x1, x29, #0x50
#endif /* !NDEBUG */
	add	x2, x29, #48
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	add	x0, x29, #0x50
	add	x1, x29, #48
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	mov	x20, #8
#ifndef NDEBUG
	add	x0, x29, #0x50
#endif /* !NDEBUG */
	add	x1, x29, #0x50
L_fe_invert2:
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	subs	x20, x20, #1
	bcs	L_fe_invert2
#ifndef NDEBUG
	add	x0, x29, #0x50
#endif /* !NDEBUG */
#ifndef NDEBUG
	add	x1, x29, #0x50
#endif /* !NDEBUG */
	add	x2, x29, #48
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	add	x0, x29, #0x70
#ifndef NDEBUG
	add	x1, x29, #0x50
#endif /* !NDEBUG */
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	mov	x20, #18
#ifndef NDEBUG
	add	x0, x29, #0x70
#endif /* !NDEBUG */
	add	x1, x29, #0x70
L_fe_invert3:
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	subs	x20, x20, #1
	bcs	L_fe_invert3
	add	x0, x29, #0x50
#ifndef NDEBUG
	add	x1, x29, #0x70
#endif /* !NDEBUG */
	add	x2, x29, #0x50
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	mov	x20, #9
#ifndef NDEBUG
	add	x0, x29, #0x50
#endif /* !NDEBUG */
	add	x1, x29, #0x50
L_fe_invert4:
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	subs	x20, x20, #1
	bcs	L_fe_invert4
	add	x0, x29, #48
#ifndef NDEBUG
	add	x1, x29, #0x50
#endif /* !NDEBUG */
	add	x2, x29, #48
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	add	x0, x29, #0x50
	add	x1, x29, #48
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	mov	x20, #48
#ifndef NDEBUG
	add	x0, x29, #0x50
#endif /* !NDEBUG */
	add	x1, x29, #0x50
L_fe_invert5:
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	subs	x20, x20, #1
	bcs	L_fe_invert5
#ifndef NDEBUG
	add	x0, x29, #0x50
#endif /* !NDEBUG */
#ifndef NDEBUG
	add	x1, x29, #0x50
#endif /* !NDEBUG */
	add	x2, x29, #48
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	add	x0, x29, #0x70
#ifndef NDEBUG
	add	x1, x29, #0x50
#endif /* !NDEBUG */
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	mov	x20, #0x62
#ifndef NDEBUG
	add	x0, x29, #0x70
#endif /* !NDEBUG */
	add	x1, x29, #0x70
L_fe_invert6:
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	subs	x20, x20, #1
	bcs	L_fe_invert6
	add	x0, x29, #0x50
#ifndef NDEBUG
	add	x1, x29, #0x70
#endif /* !NDEBUG */
	add	x2, x29, #0x50
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	mov	x20, #49
#ifndef NDEBUG
	add	x0, x29, #0x50
#endif /* !NDEBUG */
	add	x1, x29, #0x50
L_fe_invert7:
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	subs	x20, x20, #1
	bcs	L_fe_invert7
	add	x0, x29, #48
#ifndef NDEBUG
	add	x1, x29, #0x50
#endif /* !NDEBUG */
	add	x2, x29, #48
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	mov	x20, #4
#ifndef NDEBUG
	add	x0, x29, #48
#endif /* !NDEBUG */
	add	x1, x29, #48
L_fe_invert8:
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	subs	x20, x20, #1
	bcs	L_fe_invert8
	ldr	x0, [x29, #144]
#ifndef NDEBUG
	add	x1, x29, #48
#endif /* !NDEBUG */
	add	x2, x29, #16
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	ldr	x20, [x29, #168]
	ldp	x29, x30, [sp], #0xb0
	ret
#ifndef __APPLE__
	.size	fe_invert,.-fe_invert
#endif /* __APPLE__ */
#ifndef __APPLE__
.text
.globl	curve25519
.type	curve25519,@function
.align	2
curve25519:
#else
.section	__TEXT,__text
.globl	_curve25519
.p2align	2
_curve25519:
#endif /* __APPLE__ */
	stp	x29, x30, [sp, #-288]!
	add	x29, sp, #0
	str	x17, [x29, #200]
	str	x19, [x29, #208]
	stp	x20, x21, [x29, #216]
	stp	x22, x23, [x29, #232]
	stp	x24, x25, [x29, #248]
	stp	x26, x27, [x29, #264]
	str	x28, [x29, #280]
	mov	x23, xzr
	str	x0, [x29, #176]
	str	x2, [x29, #184]
	# Copy
	ldp	x6, x7, [x2]
	ldp	x8, x9, [x2, #16]
	stp	x6, x7, [x29, #80]
	stp	x8, x9, [x29, #96]
	# Set one
	mov	x2, #1
	stp	x2, xzr, [x0]
	stp	xzr, xzr, [x0, #16]
	# Set zero
	stp	xzr, xzr, [x29, #16]
	stp	xzr, xzr, [x29, #32]
	# Set one
	mov	x2, #1
	stp	x2, xzr, [x29, #48]
	stp	xzr, xzr, [x29, #64]
	mov	x25, #62
	mov	x24, #24
L_curve25519_words:
L_curve25519_bits:
	ldr	x2, [x1, x24]
	lsr	x2, x2, x25
	and	x2, x2, #1
	eor	x23, x23, x2
	# Conditional Swap
	cmp	x23, #1
	ldp	x10, x11, [x0]
	ldp	x12, x13, [x0, #16]
	ldp	x6, x7, [x29, #80]
	ldp	x8, x9, [x29, #96]
	csel	x14, x10, x6, eq
	csel	x10, x6, x10, eq
	csel	x15, x11, x7, eq
	csel	x11, x7, x11, eq
	csel	x16, x12, x8, eq
	csel	x12, x8, x12, eq
	csel	x17, x13, x9, eq
	csel	x13, x9, x13, eq
	# Conditional Swap
	cmp	x23, #1
	ldp	x19, x20, [x29, #16]
	ldp	x21, x22, [x29, #32]
	ldp	x6, x7, [x29, #48]
	ldp	x8, x9, [x29, #64]
	csel	x5, x19, x6, eq
	csel	x19, x6, x19, eq
	csel	x26, x20, x7, eq
	csel	x20, x7, x20, eq
	csel	x27, x21, x8, eq
	csel	x21, x8, x21, eq
	csel	x28, x22, x9, eq
	csel	x22, x9, x22, eq
	mov	x23, x2
	# Add
	adds	x6, x10, x19
	adcs	x7, x11, x20
	adcs	x8, x12, x21
	adc	x9, x13, x22
	mov	x3, #-19
	asr	x2, x9, #63
	#   Mask the modulus
	and	x3, x2, x3
	and	x4, x2, #0x7fffffffffffffff
	#   Sub modulus (if overflow)
	subs	x6, x6, x3
	sbcs	x7, x7, x2
	sbcs	x8, x8, x2
	sbc	x9, x9, x4
	# Sub
	subs	x19, x10, x19
	sbcs	x20, x11, x20
	sbcs	x21, x12, x21
	sbcs	x22, x13, x22
	mov	x3, #-19
	csetm	x2, cc
	#   Mask the modulus
	and	x3, x2, x3
	and	x4, x2, #0x7fffffffffffffff
	#   Add modulus (if underflow)
	adds	x19, x19, x3
	adcs	x20, x20, x2
	adcs	x21, x21, x2
	adc	x22, x22, x4
	stp	x19, x20, [x29, #144]
	stp	x21, x22, [x29, #160]
	# Add
	adds	x10, x14, x5
	adcs	x11, x15, x26
	adcs	x12, x16, x27
	adc	x13, x17, x28
	mov	x3, #-19
	asr	x2, x13, #63
	#   Mask the modulus
	and	x3, x2, x3
	and	x4, x2, #0x7fffffffffffffff
	#   Sub modulus (if overflow)
	subs	x10, x10, x3
	sbcs	x11, x11, x2
	sbcs	x12, x12, x2
	sbc	x13, x13, x4
	# Sub
	subs	x14, x14, x5
	sbcs	x15, x15, x26
	sbcs	x16, x16, x27
	sbcs	x17, x17, x28
	mov	x3, #-19
	csetm	x2, cc
	#   Mask the modulus
	and	x3, x2, x3
	and	x4, x2, #0x7fffffffffffffff
	#   Add modulus (if underflow)
	adds	x14, x14, x3
	adcs	x15, x15, x2
	adcs	x16, x16, x2
	adc	x17, x17, x4
	# Multiply
	#  A[0] * B[0]
	mul	x19, x14, x6
	umulh	x20, x14, x6
	#  A[0] * B[1]
	mul	x3, x14, x7
	umulh	x21, x14, x7
	adds	x20, x20, x3
	adc	x21, x21, xzr
	#  A[1] * B[0]
	mul	x3, x15, x6
	umulh	x4, x15, x6
	adds	x20, x20, x3
	adcs	x21, x21, x4
	adc	x22, xzr, xzr
	#  A[0] * B[2]
	mul	x3, x14, x8
	umulh	x4, x14, x8
	adds	x21, x21, x3
	adc	x22, x22, x4
	#  A[1] * B[1]
	mul	x3, x15, x7
	umulh	x4, x15, x7
	adds	x21, x21, x3
	adcs	x22, x22, x4
	adc	x2, xzr, xzr
	#  A[2] * B[0]
	mul	x3, x16, x6
	umulh	x4, x16, x6
	adds	x21, x21, x3
	adcs	x22, x22, x4
	adc	x2, x2, xzr
	#  A[0] * B[3]
	mul	x3, x14, x9
	umulh	x4, x14, x9
	adds	x22, x22, x3
	adcs	x2, x2, x4
	adc	x26, xzr, xzr
	#  A[1] * B[2]
	mul	x3, x15, x8
	umulh	x4, x15, x8
	adds	x22, x22, x3
	adcs	x2, x2, x4
	adc	x26, x26, xzr
	#  A[2] * B[1]
	mul	x3, x16, x7
	umulh	x4, x16, x7
	adds	x22, x22, x3
	adcs	x2, x2, x4
	adc	x26, x26, xzr
	#  A[3] * B[0]
	mul	x3, x17, x6
	umulh	x4, x17, x6
	adds	x22, x22, x3
	adcs	x2, x2, x4
	adc	x26, x26, xzr
	#  A[1] * B[3]
	mul	x3, x15, x9
	umulh	x4, x15, x9
	adds	x2, x2, x3
	adcs	x26, x26, x4
	adc	x27, xzr, xzr
	#  A[2] * B[2]
	mul	x3, x16, x8
	umulh	x4, x16, x8
	adds	x2, x2, x3
	adcs	x26, x26, x4
	adc	x27, x27, xzr
	#  A[3] * B[1]
	mul	x3, x17, x7
	umulh	x4, x17, x7
	adds	x2, x2, x3
	adcs	x26, x26, x4
	adc	x27, x27, xzr
	#  A[2] * B[3]
	mul	x3, x16, x9
	umulh	x4, x16, x9
	adds	x26, x26, x3
	adcs	x27, x27, x4
	adc	x28, xzr, xzr
	#  A[3] * B[2]
	mul	x3, x17, x8
	umulh	x4, x17, x8
	adds	x26, x26, x3
	adcs	x27, x27, x4
	adc	x28, x28, xzr
	#  A[3] * B[3]
	mul	x3, x17, x9
	umulh	x4, x17, x9
	adds	x27, x27, x3
	adc	x28, x28, x4
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x28, x28, x27, #63
	extr	x27, x27, x26, #63
	extr	x26, x26, x2, #63
	extr	x2, x2, x22, #63
	and	x22, x22, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x3, #19
	mul	x4, x3, x2
	umulh	x2, x3, x2
	adds	x19, x19, x4
	mul	x4, x3, x26
	umulh	x26, x3, x26
	adcs	x20, x20, x4
	mul	x4, x3, x27
	umulh	x27, x3, x27
	adcs	x21, x21, x4
	mul	x4, x3, x28
	umulh	x5, x3, x28
	adcs	x22, x22, x4
	adc	x5, x5, xzr
	#  Add remaining product results in
	adds	x20, x20, x2
	adcs	x21, x21, x26
	adcs	x22, x22, x27
	adc	x5, x5, xzr
	#  Overflow
	extr	x5, x5, x22, #63
	mul	x5, x5, x3
	and	x22, x22, #0x7fffffffffffffff
	adds	x19, x19, x5
	adcs	x20, x20, xzr
	adcs	x21, x21, xzr
	adc	x22, x22, xzr
	# Reduce if top bit set
	and	x5, x3, x22, asr 63
	and	x22, x22, #0x7fffffffffffffff
	adds	x19, x19, x5
	adcs	x20, x20, xzr
	adcs	x21, x21, xzr
	adc	x22, x22, xzr
	# Store
	stp	x19, x20, [x29, #112]
	stp	x21, x22, [x29, #128]
	# Multiply
	ldp	x2, x26, [x29, #144]
	ldp	x27, x28, [x29, #160]
	#  A[0] * B[0]
	mul	x19, x10, x2
	umulh	x20, x10, x2
	#  A[0] * B[1]
	mul	x3, x10, x26
	umulh	x21, x10, x26
	adds	x20, x20, x3
	adc	x21, x21, xzr
	#  A[1] * B[0]
	mul	x3, x11, x2
	umulh	x4, x11, x2
	adds	x20, x20, x3
	adcs	x21, x21, x4
	adc	x22, xzr, xzr
	#  A[0] * B[2]
	mul	x3, x10, x27
	umulh	x4, x10, x27
	adds	x21, x21, x3
	adc	x22, x22, x4
	#  A[1] * B[1]
	mul	x3, x11, x26
	umulh	x4, x11, x26
	adds	x21, x21, x3
	adcs	x22, x22, x4
	adc	x14, xzr, xzr
	#  A[2] * B[0]
	mul	x3, x12, x2
	umulh	x4, x12, x2
	adds	x21, x21, x3
	adcs	x22, x22, x4
	adc	x14, x14, xzr
	#  A[0] * B[3]
	mul	x3, x10, x28
	umulh	x4, x10, x28
	adds	x22, x22, x3
	adcs	x14, x14, x4
	adc	x15, xzr, xzr
	#  A[1] * B[2]
	mul	x3, x11, x27
	umulh	x4, x11, x27
	adds	x22, x22, x3
	adcs	x14, x14, x4
	adc	x15, x15, xzr
	#  A[2] * B[1]
	mul	x3, x12, x26
	umulh	x4, x12, x26
	adds	x22, x22, x3
	adcs	x14, x14, x4
	adc	x15, x15, xzr
	#  A[3] * B[0]
	mul	x3, x13, x2
	umulh	x4, x13, x2
	adds	x22, x22, x3
	adcs	x14, x14, x4
	adc	x15, x15, xzr
	#  A[1] * B[3]
	mul	x3, x11, x28
	umulh	x4, x11, x28
	adds	x14, x14, x3
	adcs	x15, x15, x4
	adc	x16, xzr, xzr
	#  A[2] * B[2]
	mul	x3, x12, x27
	umulh	x4, x12, x27
	adds	x14, x14, x3
	adcs	x15, x15, x4
	adc	x16, x16, xzr
	#  A[3] * B[1]
	mul	x3, x13, x26
	umulh	x4, x13, x26
	adds	x14, x14, x3
	adcs	x15, x15, x4
	adc	x16, x16, xzr
	#  A[2] * B[3]
	mul	x3, x12, x28
	umulh	x4, x12, x28
	adds	x15, x15, x3
	adcs	x16, x16, x4
	adc	x17, xzr, xzr
	#  A[3] * B[2]
	mul	x3, x13, x27
	umulh	x4, x13, x27
	adds	x15, x15, x3
	adcs	x16, x16, x4
	adc	x17, x17, xzr
	#  A[3] * B[3]
	mul	x3, x13, x28
	umulh	x4, x13, x28
	adds	x16, x16, x3
	adc	x17, x17, x4
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x17, x17, x16, #63
	extr	x16, x16, x15, #63
	extr	x15, x15, x14, #63
	extr	x14, x14, x22, #63
	and	x22, x22, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x3, #19
	mul	x4, x3, x14
	umulh	x14, x3, x14
	adds	x19, x19, x4
	mul	x4, x3, x15
	umulh	x15, x3, x15
	adcs	x20, x20, x4
	mul	x4, x3, x16
	umulh	x16, x3, x16
	adcs	x21, x21, x4
	mul	x4, x3, x17
	umulh	x5, x3, x17
	adcs	x22, x22, x4
	adc	x5, x5, xzr
	#  Add remaining product results in
	adds	x20, x20, x14
	adcs	x21, x21, x15
	adcs	x22, x22, x16
	adc	x5, x5, xzr
	#  Overflow
	extr	x5, x5, x22, #63
	mul	x5, x5, x3
	and	x22, x22, #0x7fffffffffffffff
	adds	x19, x19, x5
	adcs	x20, x20, xzr
	adcs	x21, x21, xzr
	adc	x22, x22, xzr
	# Reduce if top bit set
	and	x5, x3, x22, asr 63
	and	x22, x22, #0x7fffffffffffffff
	adds	x19, x19, x5
	adcs	x20, x20, xzr
	adcs	x21, x21, xzr
	adc	x22, x22, xzr
	# Store
	# Square
	#  A[0] * A[1]
	mul	x11, x2, x26
	umulh	x12, x2, x26
	#  A[0] * A[2]
	mul	x3, x2, x27
	umulh	x13, x2, x27
	adds	x12, x12, x3
	adc	x13, x13, xzr
	#  A[0] * A[3]
	mul	x3, x2, x28
	umulh	x14, x2, x28
	adds	x13, x13, x3
	adc	x14, x14, xzr
	#  A[1] * A[2]
	mul	x3, x26, x27
	umulh	x4, x26, x27
	adds	x13, x13, x3
	adcs	x14, x14, x4
	adc	x15, xzr, xzr
	#  A[1] * A[3]
	mul	x3, x26, x28
	umulh	x4, x26, x28
	adds	x14, x14, x3
	adc	x15, x15, x4
	#  A[2] * A[3]
	mul	x3, x27, x28
	umulh	x16, x27, x28
	adds	x15, x15, x3
	adc	x16, x16, xzr
	# Double
	adds	x11, x11, x11
	adcs	x12, x12, x12
	adcs	x13, x13, x13
	adcs	x14, x14, x14
	adcs	x15, x15, x15
	adcs	x16, x16, x16
	adc	x17, xzr, xzr
	#  A[0] * A[0]
	mul	x10, x2, x2
	umulh	x5, x2, x2
	#  A[1] * A[1]
	mul	x3, x26, x26
	umulh	x4, x26, x26
	adds	x11, x11, x5
	adcs	x12, x12, x3
	adc	x5, x4, xzr
	#  A[2] * A[2]
	mul	x3, x27, x27
	umulh	x4, x27, x27
	adds	x13, x13, x5
	adcs	x14, x14, x3
	adc	x5, x4, xzr
	#  A[3] * A[3]
	mul	x3, x28, x28
	umulh	x4, x28, x28
	adds	x15, x15, x5
	adcs	x16, x16, x3
	adc	x17, x17, x4
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x17, x17, x16, #63
	extr	x16, x16, x15, #63
	extr	x15, x15, x14, #63
	extr	x14, x14, x13, #63
	and	x13, x13, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x3, #19
	mul	x4, x3, x14
	umulh	x14, x3, x14
	adds	x10, x10, x4
	mul	x4, x3, x15
	umulh	x15, x3, x15
	adcs	x11, x11, x4
	mul	x4, x3, x16
	umulh	x16, x3, x16
	adcs	x12, x12, x4
	mul	x4, x3, x17
	umulh	x5, x3, x17
	adcs	x13, x13, x4
	adc	x5, x5, xzr
	#  Add remaining product results in
	adds	x11, x11, x14
	adcs	x12, x12, x15
	adcs	x13, x13, x16
	adc	x5, x5, xzr
	#  Overflow
	extr	x5, x5, x13, #63
	mul	x5, x5, x3
	and	x13, x13, #0x7fffffffffffffff
	adds	x10, x10, x5
	adcs	x11, x11, xzr
	adcs	x12, x12, xzr
	adc	x13, x13, xzr
	# Reduce if top bit set
	and	x5, x3, x13, asr 63
	and	x13, x13, #0x7fffffffffffffff
	adds	x10, x10, x5
	adcs	x11, x11, xzr
	adcs	x12, x12, xzr
	adc	x13, x13, xzr
	# Store
	# Square
	#  A[0] * A[1]
	mul	x15, x6, x7
	umulh	x16, x6, x7
	#  A[0] * A[2]
	mul	x3, x6, x8
	umulh	x17, x6, x8
	adds	x16, x16, x3
	adc	x17, x17, xzr
	#  A[0] * A[3]
	mul	x3, x6, x9
	umulh	x2, x6, x9
	adds	x17, x17, x3
	adc	x2, x2, xzr
	#  A[1] * A[2]
	mul	x3, x7, x8
	umulh	x4, x7, x8
	adds	x17, x17, x3
	adcs	x2, x2, x4
	adc	x26, xzr, xzr
	#  A[1] * A[3]
	mul	x3, x7, x9
	umulh	x4, x7, x9
	adds	x2, x2, x3
	adc	x26, x26, x4
	#  A[2] * A[3]
	mul	x3, x8, x9
	umulh	x27, x8, x9
	adds	x26, x26, x3
	adc	x27, x27, xzr
	# Double
	adds	x15, x15, x15
	adcs	x16, x16, x16
	adcs	x17, x17, x17
	adcs	x2, x2, x2
	adcs	x26, x26, x26
	adcs	x27, x27, x27
	adc	x28, xzr, xzr
	#  A[0] * A[0]
	mul	x14, x6, x6
	umulh	x5, x6, x6
	#  A[1] * A[1]
	mul	x3, x7, x7
	umulh	x4, x7, x7
	adds	x15, x15, x5
	adcs	x16, x16, x3
	adc	x5, x4, xzr
	#  A[2] * A[2]
	mul	x3, x8, x8
	umulh	x4, x8, x8
	adds	x17, x17, x5
	adcs	x2, x2, x3
	adc	x5, x4, xzr
	#  A[3] * A[3]
	mul	x3, x9, x9
	umulh	x4, x9, x9
	adds	x26, x26, x5
	adcs	x27, x27, x3
	adc	x28, x28, x4
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x28, x28, x27, #63
	extr	x27, x27, x26, #63
	extr	x26, x26, x2, #63
	extr	x2, x2, x17, #63
	and	x17, x17, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x3, #19
	mul	x4, x3, x2
	umulh	x2, x3, x2
	adds	x14, x14, x4
	mul	x4, x3, x26
	umulh	x26, x3, x26
	adcs	x15, x15, x4
	mul	x4, x3, x27
	umulh	x27, x3, x27
	adcs	x16, x16, x4
	mul	x4, x3, x28
	umulh	x5, x3, x28
	adcs	x17, x17, x4
	adc	x5, x5, xzr
	#  Add remaining product results in
	adds	x15, x15, x2
	adcs	x16, x16, x26
	adcs	x17, x17, x27
	adc	x5, x5, xzr
	#  Overflow
	extr	x5, x5, x17, #63
	mul	x5, x5, x3
	and	x17, x17, #0x7fffffffffffffff
	adds	x14, x14, x5
	adcs	x15, x15, xzr
	adcs	x16, x16, xzr
	adc	x17, x17, xzr
	# Reduce if top bit set
	and	x5, x3, x17, asr 63
	and	x17, x17, #0x7fffffffffffffff
	adds	x14, x14, x5
	adcs	x15, x15, xzr
	adcs	x16, x16, xzr
	adc	x17, x17, xzr
	# Store
	# Multiply
	#  A[0] * B[0]
	mul	x6, x14, x10
	umulh	x7, x14, x10
	#  A[0] * B[1]
	mul	x3, x14, x11
	umulh	x8, x14, x11
	adds	x7, x7, x3
	adc	x8, x8, xzr
	#  A[1] * B[0]
	mul	x3, x15, x10
	umulh	x4, x15, x10
	adds	x7, x7, x3
	adcs	x8, x8, x4
	adc	x9, xzr, xzr
	#  A[0] * B[2]
	mul	x3, x14, x12
	umulh	x4, x14, x12
	adds	x8, x8, x3
	adc	x9, x9, x4
	#  A[1] * B[1]
	mul	x3, x15, x11
	umulh	x4, x15, x11
	adds	x8, x8, x3
	adcs	x9, x9, x4
	adc	x2, xzr, xzr
	#  A[2] * B[0]
	mul	x3, x16, x10
	umulh	x4, x16, x10
	adds	x8, x8, x3
	adcs	x9, x9, x4
	adc	x2, x2, xzr
	#  A[0] * B[3]
	mul	x3, x14, x13
	umulh	x4, x14, x13
	adds	x9, x9, x3
	adcs	x2, x2, x4
	adc	x26, xzr, xzr
	#  A[1] * B[2]
	mul	x3, x15, x12
	umulh	x4, x15, x12
	adds	x9, x9, x3
	adcs	x2, x2, x4
	adc	x26, x26, xzr
	#  A[2] * B[1]
	mul	x3, x16, x11
	umulh	x4, x16, x11
	adds	x9, x9, x3
	adcs	x2, x2, x4
	adc	x26, x26, xzr
	#  A[3] * B[0]
	mul	x3, x17, x10
	umulh	x4, x17, x10
	adds	x9, x9, x3
	adcs	x2, x2, x4
	adc	x26, x26, xzr
	#  A[1] * B[3]
	mul	x3, x15, x13
	umulh	x4, x15, x13
	adds	x2, x2, x3
	adcs	x26, x26, x4
	adc	x27, xzr, xzr
	#  A[2] * B[2]
	mul	x3, x16, x12
	umulh	x4, x16, x12
	adds	x2, x2, x3
	adcs	x26, x26, x4
	adc	x27, x27, xzr
	#  A[3] * B[1]
	mul	x3, x17, x11
	umulh	x4, x17, x11
	adds	x2, x2, x3
	adcs	x26, x26, x4
	adc	x27, x27, xzr
	#  A[2] * B[3]
	mul	x3, x16, x13
	umulh	x4, x16, x13
	adds	x26, x26, x3
	adcs	x27, x27, x4
	adc	x28, xzr, xzr
	#  A[3] * B[2]
	mul	x3, x17, x12
	umulh	x4, x17, x12
	adds	x26, x26, x3
	adcs	x27, x27, x4
	adc	x28, x28, xzr
	#  A[3] * B[3]
	mul	x3, x17, x13
	umulh	x4, x17, x13
	adds	x27, x27, x3
	adc	x28, x28, x4
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x28, x28, x27, #63
	extr	x27, x27, x26, #63
	extr	x26, x26, x2, #63
	extr	x2, x2, x9, #63
	and	x9, x9, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x3, #19
	mul	x4, x3, x2
	umulh	x2, x3, x2
	adds	x6, x6, x4
	mul	x4, x3, x26
	umulh	x26, x3, x26
	adcs	x7, x7, x4
	mul	x4, x3, x27
	umulh	x27, x3, x27
	adcs	x8, x8, x4
	mul	x4, x3, x28
	umulh	x5, x3, x28
	adcs	x9, x9, x4
	adc	x5, x5, xzr
	#  Add remaining product results in
	adds	x7, x7, x2
	adcs	x8, x8, x26
	adcs	x9, x9, x27
	adc	x5, x5, xzr
	#  Overflow
	extr	x5, x5, x9, #63
	mul	x5, x5, x3
	and	x9, x9, #0x7fffffffffffffff
	adds	x6, x6, x5
	adcs	x7, x7, xzr
	adcs	x8, x8, xzr
	adc	x9, x9, xzr
	# Reduce if top bit set
	and	x5, x3, x9, asr 63
	and	x9, x9, #0x7fffffffffffffff
	adds	x6, x6, x5
	adcs	x7, x7, xzr
	adcs	x8, x8, xzr
	adc	x9, x9, xzr
	# Store
	stp	x6, x7, [x0]
	stp	x8, x9, [x0, #16]
	# Sub
	subs	x14, x14, x10
	sbcs	x15, x15, x11
	sbcs	x16, x16, x12
	sbcs	x17, x17, x13
	mov	x3, #-19
	csetm	x2, cc
	#   Mask the modulus
	and	x3, x2, x3
	and	x4, x2, #0x7fffffffffffffff
	#   Add modulus (if underflow)
	adds	x14, x14, x3
	adcs	x15, x15, x2
	adcs	x16, x16, x2
	adc	x17, x17, x4
	# Multiply by 121666
	mov	x5, #0xdb42
	movk	x5, #1, lsl 16
	mul	x6, x14, x5
	umulh	x7, x14, x5
	mul	x3, x15, x5
	umulh	x4, x15, x5
	adds	x7, x7, x3
	adc	x8, xzr, x4
	mul	x3, x16, x5
	umulh	x4, x16, x5
	adds	x8, x8, x3
	adc	x9, xzr, x4
	mul	x3, x17, x5
	umulh	x4, x17, x5
	adds	x9, x9, x3
	adc	x4, xzr, x4
	mov	x5, #19
	extr	x4, x4, x9, #63
	mul	x4, x4, x5
	and	x9, x9, #0x7fffffffffffffff
	adds	x6, x6, x4
	adcs	x7, x7, xzr
	adcs	x8, x8, xzr
	adc	x9, x9, xzr
	# Add
	adds	x10, x10, x6
	adcs	x11, x11, x7
	adcs	x12, x12, x8
	adc	x13, x13, x9
	mov	x3, #-19
	asr	x2, x13, #63
	#   Mask the modulus
	and	x3, x2, x3
	and	x4, x2, #0x7fffffffffffffff
	#   Sub modulus (if overflow)
	subs	x10, x10, x3
	sbcs	x11, x11, x2
	sbcs	x12, x12, x2
	sbc	x13, x13, x4
	# Multiply
	#  A[0] * B[0]
	mul	x6, x14, x10
	umulh	x7, x14, x10
	#  A[0] * B[1]
	mul	x3, x14, x11
	umulh	x8, x14, x11
	adds	x7, x7, x3
	adc	x8, x8, xzr
	#  A[1] * B[0]
	mul	x3, x15, x10
	umulh	x4, x15, x10
	adds	x7, x7, x3
	adcs	x8, x8, x4
	adc	x9, xzr, xzr
	#  A[0] * B[2]
	mul	x3, x14, x12
	umulh	x4, x14, x12
	adds	x8, x8, x3
	adc	x9, x9, x4
	#  A[1] * B[1]
	mul	x3, x15, x11
	umulh	x4, x15, x11
	adds	x8, x8, x3
	adcs	x9, x9, x4
	adc	x2, xzr, xzr
	#  A[2] * B[0]
	mul	x3, x16, x10
	umulh	x4, x16, x10
	adds	x8, x8, x3
	adcs	x9, x9, x4
	adc	x2, x2, xzr
	#  A[0] * B[3]
	mul	x3, x14, x13
	umulh	x4, x14, x13
	adds	x9, x9, x3
	adcs	x2, x2, x4
	adc	x26, xzr, xzr
	#  A[1] * B[2]
	mul	x3, x15, x12
	umulh	x4, x15, x12
	adds	x9, x9, x3
	adcs	x2, x2, x4
	adc	x26, x26, xzr
	#  A[2] * B[1]
	mul	x3, x16, x11
	umulh	x4, x16, x11
	adds	x9, x9, x3
	adcs	x2, x2, x4
	adc	x26, x26, xzr
	#  A[3] * B[0]
	mul	x3, x17, x10
	umulh	x4, x17, x10
	adds	x9, x9, x3
	adcs	x2, x2, x4
	adc	x26, x26, xzr
	#  A[1] * B[3]
	mul	x3, x15, x13
	umulh	x4, x15, x13
	adds	x2, x2, x3
	adcs	x26, x26, x4
	adc	x27, xzr, xzr
	#  A[2] * B[2]
	mul	x3, x16, x12
	umulh	x4, x16, x12
	adds	x2, x2, x3
	adcs	x26, x26, x4
	adc	x27, x27, xzr
	#  A[3] * B[1]
	mul	x3, x17, x11
	umulh	x4, x17, x11
	adds	x2, x2, x3
	adcs	x26, x26, x4
	adc	x27, x27, xzr
	#  A[2] * B[3]
	mul	x3, x16, x13
	umulh	x4, x16, x13
	adds	x26, x26, x3
	adcs	x27, x27, x4
	adc	x28, xzr, xzr
	#  A[3] * B[2]
	mul	x3, x17, x12
	umulh	x4, x17, x12
	adds	x26, x26, x3
	adcs	x27, x27, x4
	adc	x28, x28, xzr
	#  A[3] * B[3]
	mul	x3, x17, x13
	umulh	x4, x17, x13
	adds	x27, x27, x3
	adc	x28, x28, x4
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x28, x28, x27, #63
	extr	x27, x27, x26, #63
	extr	x26, x26, x2, #63
	extr	x2, x2, x9, #63
	and	x9, x9, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x3, #19
	mul	x4, x3, x2
	umulh	x2, x3, x2
	adds	x6, x6, x4
	mul	x4, x3, x26
	umulh	x26, x3, x26
	adcs	x7, x7, x4
	mul	x4, x3, x27
	umulh	x27, x3, x27
	adcs	x8, x8, x4
	mul	x4, x3, x28
	umulh	x5, x3, x28
	adcs	x9, x9, x4
	adc	x5, x5, xzr
	#  Add remaining product results in
	adds	x7, x7, x2
	adcs	x8, x8, x26
	adcs	x9, x9, x27
	adc	x5, x5, xzr
	#  Overflow
	extr	x5, x5, x9, #63
	mul	x5, x5, x3
	and	x9, x9, #0x7fffffffffffffff
	adds	x6, x6, x5
	adcs	x7, x7, xzr
	adcs	x8, x8, xzr
	adc	x9, x9, xzr
	# Reduce if top bit set
	and	x5, x3, x9, asr 63
	and	x9, x9, #0x7fffffffffffffff
	adds	x6, x6, x5
	adcs	x7, x7, xzr
	adcs	x8, x8, xzr
	adc	x9, x9, xzr
	# Store
	stp	x6, x7, [x29, #16]
	stp	x8, x9, [x29, #32]
	# Add
	ldp	x6, x7, [x29, #112]
	ldp	x8, x9, [x29, #128]
	adds	x10, x6, x19
	adcs	x11, x7, x20
	adcs	x12, x8, x21
	adc	x13, x9, x22
	mov	x3, #-19
	asr	x2, x13, #63
	#   Mask the modulus
	and	x3, x2, x3
	and	x4, x2, #0x7fffffffffffffff
	#   Sub modulus (if overflow)
	subs	x10, x10, x3
	sbcs	x11, x11, x2
	sbcs	x12, x12, x2
	sbc	x13, x13, x4
	# Sub
	subs	x19, x6, x19
	sbcs	x20, x7, x20
	sbcs	x21, x8, x21
	sbcs	x22, x9, x22
	mov	x3, #-19
	csetm	x2, cc
	#   Mask the modulus
	and	x3, x2, x3
	and	x4, x2, #0x7fffffffffffffff
	#   Add modulus (if underflow)
	adds	x19, x19, x3
	adcs	x20, x20, x2
	adcs	x21, x21, x2
	adc	x22, x22, x4
	# Square
	#  A[0] * A[1]
	mul	x7, x10, x11
	umulh	x8, x10, x11
	#  A[0] * A[2]
	mul	x3, x10, x12
	umulh	x9, x10, x12
	adds	x8, x8, x3
	adc	x9, x9, xzr
	#  A[0] * A[3]
	mul	x3, x10, x13
	umulh	x2, x10, x13
	adds	x9, x9, x3
	adc	x2, x2, xzr
	#  A[1] * A[2]
	mul	x3, x11, x12
	umulh	x4, x11, x12
	adds	x9, x9, x3
	adcs	x2, x2, x4
	adc	x26, xzr, xzr
	#  A[1] * A[3]
	mul	x3, x11, x13
	umulh	x4, x11, x13
	adds	x2, x2, x3
	adc	x26, x26, x4
	#  A[2] * A[3]
	mul	x3, x12, x13
	umulh	x27, x12, x13
	adds	x26, x26, x3
	adc	x27, x27, xzr
	# Double
	adds	x7, x7, x7
	adcs	x8, x8, x8
	adcs	x9, x9, x9
	adcs	x2, x2, x2
	adcs	x26, x26, x26
	adcs	x27, x27, x27
	adc	x28, xzr, xzr
	#  A[0] * A[0]
	mul	x6, x10, x10
	umulh	x5, x10, x10
	#  A[1] * A[1]
	mul	x3, x11, x11
	umulh	x4, x11, x11
	adds	x7, x7, x5
	adcs	x8, x8, x3
	adc	x5, x4, xzr
	#  A[2] * A[2]
	mul	x3, x12, x12
	umulh	x4, x12, x12
	adds	x9, x9, x5
	adcs	x2, x2, x3
	adc	x5, x4, xzr
	#  A[3] * A[3]
	mul	x3, x13, x13
	umulh	x4, x13, x13
	adds	x26, x26, x5
	adcs	x27, x27, x3
	adc	x28, x28, x4
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x28, x28, x27, #63
	extr	x27, x27, x26, #63
	extr	x26, x26, x2, #63
	extr	x2, x2, x9, #63
	and	x9, x9, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x3, #19
	mul	x4, x3, x2
	umulh	x2, x3, x2
	adds	x6, x6, x4
	mul	x4, x3, x26
	umulh	x26, x3, x26
	adcs	x7, x7, x4
	mul	x4, x3, x27
	umulh	x27, x3, x27
	adcs	x8, x8, x4
	mul	x4, x3, x28
	umulh	x5, x3, x28
	adcs	x9, x9, x4
	adc	x5, x5, xzr
	#  Add remaining product results in
	adds	x7, x7, x2
	adcs	x8, x8, x26
	adcs	x9, x9, x27
	adc	x5, x5, xzr
	#  Overflow
	extr	x5, x5, x9, #63
	mul	x5, x5, x3
	and	x9, x9, #0x7fffffffffffffff
	adds	x6, x6, x5
	adcs	x7, x7, xzr
	adcs	x8, x8, xzr
	adc	x9, x9, xzr
	# Reduce if top bit set
	and	x5, x3, x9, asr 63
	and	x9, x9, #0x7fffffffffffffff
	adds	x6, x6, x5
	adcs	x7, x7, xzr
	adcs	x8, x8, xzr
	adc	x9, x9, xzr
	# Store
	stp	x6, x7, [x29, #80]
	stp	x8, x9, [x29, #96]
	# Square
	#  A[0] * A[1]
	mul	x7, x19, x20
	umulh	x8, x19, x20
	#  A[0] * A[2]
	mul	x3, x19, x21
	umulh	x9, x19, x21
	adds	x8, x8, x3
	adc	x9, x9, xzr
	#  A[0] * A[3]
	mul	x3, x19, x22
	umulh	x2, x19, x22
	adds	x9, x9, x3
	adc	x2, x2, xzr
	#  A[1] * A[2]
	mul	x3, x20, x21
	umulh	x4, x20, x21
	adds	x9, x9, x3
	adcs	x2, x2, x4
	adc	x26, xzr, xzr
	#  A[1] * A[3]
	mul	x3, x20, x22
	umulh	x4, x20, x22
	adds	x2, x2, x3
	adc	x26, x26, x4
	#  A[2] * A[3]
	mul	x3, x21, x22
	umulh	x27, x21, x22
	adds	x26, x26, x3
	adc	x27, x27, xzr
	# Double
	adds	x7, x7, x7
	adcs	x8, x8, x8
	adcs	x9, x9, x9
	adcs	x2, x2, x2
	adcs	x26, x26, x26
	adcs	x27, x27, x27
	adc	x28, xzr, xzr
	#  A[0] * A[0]
	mul	x6, x19, x19
	umulh	x5, x19, x19
	#  A[1] * A[1]
	mul	x3, x20, x20
	umulh	x4, x20, x20
	adds	x7, x7, x5
	adcs	x8, x8, x3
	adc	x5, x4, xzr
	#  A[2] * A[2]
	mul	x3, x21, x21
	umulh	x4, x21, x21
	adds	x9, x9, x5
	adcs	x2, x2, x3
	adc	x5, x4, xzr
	#  A[3] * A[3]
	mul	x3, x22, x22
	umulh	x4, x22, x22
	adds	x26, x26, x5
	adcs	x27, x27, x3
	adc	x28, x28, x4
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x28, x28, x27, #63
	extr	x27, x27, x26, #63
	extr	x26, x26, x2, #63
	extr	x2, x2, x9, #63
	and	x9, x9, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x3, #19
	mul	x4, x3, x2
	umulh	x2, x3, x2
	adds	x6, x6, x4
	mul	x4, x3, x26
	umulh	x26, x3, x26
	adcs	x7, x7, x4
	mul	x4, x3, x27
	umulh	x27, x3, x27
	adcs	x8, x8, x4
	mul	x4, x3, x28
	umulh	x5, x3, x28
	adcs	x9, x9, x4
	adc	x5, x5, xzr
	#  Add remaining product results in
	adds	x7, x7, x2
	adcs	x8, x8, x26
	adcs	x9, x9, x27
	adc	x5, x5, xzr
	#  Overflow
	extr	x5, x5, x9, #63
	mul	x5, x5, x3
	and	x9, x9, #0x7fffffffffffffff
	adds	x6, x6, x5
	adcs	x7, x7, xzr
	adcs	x8, x8, xzr
	adc	x9, x9, xzr
	# Reduce if top bit set
	and	x5, x3, x9, asr 63
	and	x9, x9, #0x7fffffffffffffff
	adds	x6, x6, x5
	adcs	x7, x7, xzr
	adcs	x8, x8, xzr
	adc	x9, x9, xzr
	# Store
	ldr	x2, [x29, #184]
	# Multiply
	ldp	x14, x15, [x2]
	ldp	x16, x17, [x2, #16]
	#  A[0] * B[0]
	mul	x10, x14, x6
	umulh	x11, x14, x6
	#  A[0] * B[1]
	mul	x3, x14, x7
	umulh	x12, x14, x7
	adds	x11, x11, x3
	adc	x12, x12, xzr
	#  A[1] * B[0]
	mul	x3, x15, x6
	umulh	x4, x15, x6
	adds	x11, x11, x3
	adcs	x12, x12, x4
	adc	x13, xzr, xzr
	#  A[0] * B[2]
	mul	x3, x14, x8
	umulh	x4, x14, x8
	adds	x12, x12, x3
	adc	x13, x13, x4
	#  A[1] * B[1]
	mul	x3, x15, x7
	umulh	x4, x15, x7
	adds	x12, x12, x3
	adcs	x13, x13, x4
	adc	x2, xzr, xzr
	#  A[2] * B[0]
	mul	x3, x16, x6
	umulh	x4, x16, x6
	adds	x12, x12, x3
	adcs	x13, x13, x4
	adc	x2, x2, xzr
	#  A[0] * B[3]
	mul	x3, x14, x9
	umulh	x4, x14, x9
	adds	x13, x13, x3
	adcs	x2, x2, x4
	adc	x26, xzr, xzr
	#  A[1] * B[2]
	mul	x3, x15, x8
	umulh	x4, x15, x8
	adds	x13, x13, x3
	adcs	x2, x2, x4
	adc	x26, x26, xzr
	#  A[2] * B[1]
	mul	x3, x16, x7
	umulh	x4, x16, x7
	adds	x13, x13, x3
	adcs	x2, x2, x4
	adc	x26, x26, xzr
	#  A[3] * B[0]
	mul	x3, x17, x6
	umulh	x4, x17, x6
	adds	x13, x13, x3
	adcs	x2, x2, x4
	adc	x26, x26, xzr
	#  A[1] * B[3]
	mul	x3, x15, x9
	umulh	x4, x15, x9
	adds	x2, x2, x3
	adcs	x26, x26, x4
	adc	x27, xzr, xzr
	#  A[2] * B[2]
	mul	x3, x16, x8
	umulh	x4, x16, x8
	adds	x2, x2, x3
	adcs	x26, x26, x4
	adc	x27, x27, xzr
	#  A[3] * B[1]
	mul	x3, x17, x7
	umulh	x4, x17, x7
	adds	x2, x2, x3
	adcs	x26, x26, x4
	adc	x27, x27, xzr
	#  A[2] * B[3]
	mul	x3, x16, x9
	umulh	x4, x16, x9
	adds	x26, x26, x3
	adcs	x27, x27, x4
	adc	x28, xzr, xzr
	#  A[3] * B[2]
	mul	x3, x17, x8
	umulh	x4, x17, x8
	adds	x26, x26, x3
	adcs	x27, x27, x4
	adc	x28, x28, xzr
	#  A[3] * B[3]
	mul	x3, x17, x9
	umulh	x4, x17, x9
	adds	x27, x27, x3
	adc	x28, x28, x4
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x28, x28, x27, #63
	extr	x27, x27, x26, #63
	extr	x26, x26, x2, #63
	extr	x2, x2, x13, #63
	and	x13, x13, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x3, #19
	mul	x4, x3, x2
	umulh	x2, x3, x2
	adds	x10, x10, x4
	mul	x4, x3, x26
	umulh	x26, x3, x26
	adcs	x11, x11, x4
	mul	x4, x3, x27
	umulh	x27, x3, x27
	adcs	x12, x12, x4
	mul	x4, x3, x28
	umulh	x5, x3, x28
	adcs	x13, x13, x4
	adc	x5, x5, xzr
	#  Add remaining product results in
	adds	x11, x11, x2
	adcs	x12, x12, x26
	adcs	x13, x13, x27
	adc	x5, x5, xzr
	#  Overflow
	extr	x5, x5, x13, #63
	mul	x5, x5, x3
	and	x13, x13, #0x7fffffffffffffff
	adds	x10, x10, x5
	adcs	x11, x11, xzr
	adcs	x12, x12, xzr
	adc	x13, x13, xzr
	# Reduce if top bit set
	and	x5, x3, x13, asr 63
	and	x13, x13, #0x7fffffffffffffff
	adds	x10, x10, x5
	adcs	x11, x11, xzr
	adcs	x12, x12, xzr
	adc	x13, x13, xzr
	# Store
	stp	x10, x11, [x29, #48]
	stp	x12, x13, [x29, #64]
	sub	x25, x25, #1
	cmp	x25, #0
	bge	L_curve25519_bits
	mov	x25, #63
	sub	x24, x24, #8
	cmp	x24, #0
	bge	L_curve25519_words
	# Invert
	add	x0, x29, #48
	add	x1, x29, #16
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	add	x0, x29, #0x50
	add	x1, x29, #48
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
#ifndef NDEBUG
	add	x0, x29, #0x50
#endif /* !NDEBUG */
	add	x1, x29, #0x50
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
#ifndef NDEBUG
	add	x0, x29, #0x50
#endif /* !NDEBUG */
	add	x1, x29, #16
	add	x2, x29, #0x50
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	add	x0, x29, #48
	add	x1, x29, #48
	add	x2, x29, #0x50
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	add	x0, x29, #0x70
#ifndef NDEBUG
	add	x1, x29, #48
#endif /* !NDEBUG */
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	add	x0, x29, #0x50
	add	x1, x29, #0x50
	add	x2, x29, #0x70
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	add	x0, x29, #0x70
#ifndef NDEBUG
	add	x1, x29, #0x50
#endif /* !NDEBUG */
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	mov	x24, #3
#ifndef NDEBUG
	add	x0, x29, #0x70
#endif /* !NDEBUG */
	add	x1, x29, #0x70
L_curve25519_inv_1:
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	subs	x24, x24, #1
	bcs	L_curve25519_inv_1
	add	x0, x29, #0x50
#ifndef NDEBUG
	add	x1, x29, #0x70
#endif /* !NDEBUG */
	add	x2, x29, #0x50
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	add	x0, x29, #0x70
	add	x1, x29, #0x50
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	mov	x24, #8
#ifndef NDEBUG
	add	x0, x29, #0x70
#endif /* !NDEBUG */
	add	x1, x29, #0x70
L_curve25519_inv_2:
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	subs	x24, x24, #1
	bcs	L_curve25519_inv_2
#ifndef NDEBUG
	add	x0, x29, #0x70
#endif /* !NDEBUG */
#ifndef NDEBUG
	add	x1, x29, #0x70
#endif /* !NDEBUG */
	add	x2, x29, #0x50
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	add	x0, x29, #0x90
#ifndef NDEBUG
	add	x1, x29, #0x70
#endif /* !NDEBUG */
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	mov	x24, #18
#ifndef NDEBUG
	add	x0, x29, #0x90
#endif /* !NDEBUG */
	add	x1, x29, #0x90
L_curve25519_inv_3:
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	subs	x24, x24, #1
	bcs	L_curve25519_inv_3
	add	x0, x29, #0x70
#ifndef NDEBUG
	add	x1, x29, #0x90
#endif /* !NDEBUG */
	add	x2, x29, #0x70
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	mov	x24, #9
#ifndef NDEBUG
	add	x0, x29, #0x70
#endif /* !NDEBUG */
	add	x1, x29, #0x70
L_curve25519_inv_4:
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	subs	x24, x24, #1
	bcs	L_curve25519_inv_4
	add	x0, x29, #0x50
#ifndef NDEBUG
	add	x1, x29, #0x70
#endif /* !NDEBUG */
	add	x2, x29, #0x50
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	add	x0, x29, #0x70
	add	x1, x29, #0x50
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	mov	x24, #48
#ifndef NDEBUG
	add	x0, x29, #0x70
#endif /* !NDEBUG */
	add	x1, x29, #0x70
L_curve25519_inv_5:
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	subs	x24, x24, #1
	bcs	L_curve25519_inv_5
#ifndef NDEBUG
	add	x0, x29, #0x70
#endif /* !NDEBUG */
#ifndef NDEBUG
	add	x1, x29, #0x70
#endif /* !NDEBUG */
	add	x2, x29, #0x50
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	add	x0, x29, #0x90
#ifndef NDEBUG
	add	x1, x29, #0x70
#endif /* !NDEBUG */
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	mov	x24, #0x62
#ifndef NDEBUG
	add	x0, x29, #0x90
#endif /* !NDEBUG */
	add	x1, x29, #0x90
L_curve25519_inv_6:
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	subs	x24, x24, #1
	bcs	L_curve25519_inv_6
	add	x0, x29, #0x70
#ifndef NDEBUG
	add	x1, x29, #0x90
#endif /* !NDEBUG */
	add	x2, x29, #0x70
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	mov	x24, #49
#ifndef NDEBUG
	add	x0, x29, #0x70
#endif /* !NDEBUG */
	add	x1, x29, #0x70
L_curve25519_inv_7:
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	subs	x24, x24, #1
	bcs	L_curve25519_inv_7
	add	x0, x29, #0x50
#ifndef NDEBUG
	add	x1, x29, #0x70
#endif /* !NDEBUG */
	add	x2, x29, #0x50
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	mov	x24, #4
#ifndef NDEBUG
	add	x0, x29, #0x50
#endif /* !NDEBUG */
	add	x1, x29, #0x50
L_curve25519_inv_8:
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	subs	x24, x24, #1
	bcs	L_curve25519_inv_8
	add	x0, x29, #16
#ifndef NDEBUG
	add	x1, x29, #0x50
#endif /* !NDEBUG */
	add	x2, x29, #48
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	ldr	x0, [x29, #176]
	# Multiply
	ldp	x6, x7, [x0]
	ldp	x8, x9, [x0, #16]
	ldp	x10, x11, [x29, #16]
	ldp	x12, x13, [x29, #32]
	#  A[0] * B[0]
	mul	x14, x6, x10
	umulh	x15, x6, x10
	#  A[0] * B[1]
	mul	x3, x6, x11
	umulh	x16, x6, x11
	adds	x15, x15, x3
	adc	x16, x16, xzr
	#  A[1] * B[0]
	mul	x3, x7, x10
	umulh	x4, x7, x10
	adds	x15, x15, x3
	adcs	x16, x16, x4
	adc	x17, xzr, xzr
	#  A[0] * B[2]
	mul	x3, x6, x12
	umulh	x4, x6, x12
	adds	x16, x16, x3
	adc	x17, x17, x4
	#  A[1] * B[1]
	mul	x3, x7, x11
	umulh	x4, x7, x11
	adds	x16, x16, x3
	adcs	x17, x17, x4
	adc	x19, xzr, xzr
	#  A[2] * B[0]
	mul	x3, x8, x10
	umulh	x4, x8, x10
	adds	x16, x16, x3
	adcs	x17, x17, x4
	adc	x19, x19, xzr
	#  A[0] * B[3]
	mul	x3, x6, x13
	umulh	x4, x6, x13
	adds	x17, x17, x3
	adcs	x19, x19, x4
	adc	x20, xzr, xzr
	#  A[1] * B[2]
	mul	x3, x7, x12
	umulh	x4, x7, x12
	adds	x17, x17, x3
	adcs	x19, x19, x4
	adc	x20, x20, xzr
	#  A[2] * B[1]
	mul	x3, x8, x11
	umulh	x4, x8, x11
	adds	x17, x17, x3
	adcs	x19, x19, x4
	adc	x20, x20, xzr
	#  A[3] * B[0]
	mul	x3, x9, x10
	umulh	x4, x9, x10
	adds	x17, x17, x3
	adcs	x19, x19, x4
	adc	x20, x20, xzr
	#  A[1] * B[3]
	mul	x3, x7, x13
	umulh	x4, x7, x13
	adds	x19, x19, x3
	adcs	x20, x20, x4
	adc	x21, xzr, xzr
	#  A[2] * B[2]
	mul	x3, x8, x12
	umulh	x4, x8, x12
	adds	x19, x19, x3
	adcs	x20, x20, x4
	adc	x21, x21, xzr
	#  A[3] * B[1]
	mul	x3, x9, x11
	umulh	x4, x9, x11
	adds	x19, x19, x3
	adcs	x20, x20, x4
	adc	x21, x21, xzr
	#  A[2] * B[3]
	mul	x3, x8, x13
	umulh	x4, x8, x13
	adds	x20, x20, x3
	adcs	x21, x21, x4
	adc	x22, xzr, xzr
	#  A[3] * B[2]
	mul	x3, x9, x12
	umulh	x4, x9, x12
	adds	x20, x20, x3
	adcs	x21, x21, x4
	adc	x22, x22, xzr
	#  A[3] * B[3]
	mul	x3, x9, x13
	umulh	x4, x9, x13
	adds	x21, x21, x3
	adc	x22, x22, x4
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x22, x22, x21, #63
	extr	x21, x21, x20, #63
	extr	x20, x20, x19, #63
	extr	x19, x19, x17, #63
	and	x17, x17, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x3, #19
	mul	x4, x3, x19
	umulh	x19, x3, x19
	adds	x14, x14, x4
	mul	x4, x3, x20
	umulh	x20, x3, x20
	adcs	x15, x15, x4
	mul	x4, x3, x21
	umulh	x21, x3, x21
	adcs	x16, x16, x4
	mul	x4, x3, x22
	umulh	x5, x3, x22
	adcs	x17, x17, x4
	adc	x5, x5, xzr
	#  Add remaining product results in
	adds	x15, x15, x19
	adcs	x16, x16, x20
	adcs	x17, x17, x21
	adc	x5, x5, xzr
	#  Overflow
	extr	x5, x5, x17, #63
	mul	x5, x5, x3
	and	x17, x17, #0x7fffffffffffffff
	adds	x14, x14, x5
	adcs	x15, x15, xzr
	adcs	x16, x16, xzr
	adc	x17, x17, xzr
	# Reduce if top bit set
	and	x5, x3, x17, asr 63
	and	x17, x17, #0x7fffffffffffffff
	adds	x14, x14, x5
	adcs	x15, x15, xzr
	adcs	x16, x16, xzr
	adc	x17, x17, xzr
	adds	x4, x14, x3
	adcs	x4, x15, xzr
	adcs	x4, x16, xzr
	adc	x4, x17, xzr
	and	x4, x3, x4, asr 63
	adds	x14, x14, x4
	adcs	x15, x15, xzr
	mov	x4, #0x7fffffffffffffff
	adcs	x16, x16, xzr
	adc	x17, x17, xzr
	and	x17, x17, x4
	# Store
	stp	x14, x15, [x0]
	stp	x16, x17, [x0, #16]
	mov	x0, xzr
	ldr	x17, [x29, #200]
	ldr	x19, [x29, #208]
	ldp	x20, x21, [x29, #216]
	ldp	x22, x23, [x29, #232]
	ldp	x24, x25, [x29, #248]
	ldp	x26, x27, [x29, #264]
	ldr	x28, [x29, #280]
	ldp	x29, x30, [sp], #0x120
	ret
#ifndef __APPLE__
	.size	curve25519,.-curve25519
#endif /* __APPLE__ */
#ifndef __APPLE__
.text
.globl	fe_pow22523
.type	fe_pow22523,@function
.align	2
fe_pow22523:
#else
.section	__TEXT,__text
.globl	_fe_pow22523
.p2align	2
_fe_pow22523:
#endif /* __APPLE__ */
	stp	x29, x30, [sp, #-144]!
	add	x29, sp, #0
	str	x23, [x29, #136]
	# pow22523
	str	x0, [x29, #112]
	str	x1, [x29, #120]
	add	x0, x29, #16
#ifndef NDEBUG
	ldr	x1, [x29, #120]
#endif /* !NDEBUG */
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	add	x0, x29, #48
	add	x1, x29, #16
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
#ifndef NDEBUG
	add	x0, x29, #48
#endif /* !NDEBUG */
	add	x1, x29, #48
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
#ifndef NDEBUG
	add	x0, x29, #48
#endif /* !NDEBUG */
	ldr	x1, [x29, #120]
	add	x2, x29, #48
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	add	x0, x29, #16
	add	x1, x29, #16
	add	x2, x29, #48
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
#ifndef NDEBUG
	add	x0, x29, #16
#endif /* !NDEBUG */
#ifndef NDEBUG
	add	x1, x29, #16
#endif /* !NDEBUG */
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
#ifndef NDEBUG
	add	x0, x29, #16
#endif /* !NDEBUG */
	add	x1, x29, #48
	add	x2, x29, #16
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	add	x0, x29, #48
	add	x1, x29, #16
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	mov	x23, #3
#ifndef NDEBUG
	add	x0, x29, #48
#endif /* !NDEBUG */
	add	x1, x29, #48
L_fe_pow22523_1:
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	subs	x23, x23, #1
	bcs	L_fe_pow22523_1
	add	x0, x29, #16
#ifndef NDEBUG
	add	x1, x29, #48
#endif /* !NDEBUG */
	add	x2, x29, #16
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	add	x0, x29, #48
	add	x1, x29, #16
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	mov	x23, #8
#ifndef NDEBUG
	add	x0, x29, #48
#endif /* !NDEBUG */
	add	x1, x29, #48
L_fe_pow22523_2:
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	subs	x23, x23, #1
	bcs	L_fe_pow22523_2
#ifndef NDEBUG
	add	x0, x29, #48
#endif /* !NDEBUG */
#ifndef NDEBUG
	add	x1, x29, #48
#endif /* !NDEBUG */
	add	x2, x29, #16
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	add	x0, x29, #0x50
#ifndef NDEBUG
	add	x1, x29, #48
#endif /* !NDEBUG */
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	mov	x23, #18
#ifndef NDEBUG
	add	x0, x29, #0x50
#endif /* !NDEBUG */
	add	x1, x29, #0x50
L_fe_pow22523_3:
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	subs	x23, x23, #1
	bcs	L_fe_pow22523_3
	add	x0, x29, #48
#ifndef NDEBUG
	add	x1, x29, #0x50
#endif /* !NDEBUG */
	add	x2, x29, #48
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	mov	x23, #9
#ifndef NDEBUG
	add	x0, x29, #48
#endif /* !NDEBUG */
	add	x1, x29, #48
L_fe_pow22523_4:
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	subs	x23, x23, #1
	bcs	L_fe_pow22523_4
	add	x0, x29, #16
#ifndef NDEBUG
	add	x1, x29, #48
#endif /* !NDEBUG */
	add	x2, x29, #16
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	add	x0, x29, #48
	add	x1, x29, #16
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	mov	x23, #48
#ifndef NDEBUG
	add	x0, x29, #48
#endif /* !NDEBUG */
	add	x1, x29, #48
L_fe_pow22523_5:
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	subs	x23, x23, #1
	bcs	L_fe_pow22523_5
#ifndef NDEBUG
	add	x0, x29, #48
#endif /* !NDEBUG */
#ifndef NDEBUG
	add	x1, x29, #48
#endif /* !NDEBUG */
	add	x2, x29, #16
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	add	x0, x29, #0x50
#ifndef NDEBUG
	add	x1, x29, #48
#endif /* !NDEBUG */
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	mov	x23, #0x62
#ifndef NDEBUG
	add	x0, x29, #0x50
#endif /* !NDEBUG */
	add	x1, x29, #0x50
L_fe_pow22523_6:
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	subs	x23, x23, #1
	bcs	L_fe_pow22523_6
	add	x0, x29, #48
#ifndef NDEBUG
	add	x1, x29, #0x50
#endif /* !NDEBUG */
	add	x2, x29, #48
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	mov	x23, #49
#ifndef NDEBUG
	add	x0, x29, #48
#endif /* !NDEBUG */
	add	x1, x29, #48
L_fe_pow22523_7:
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	subs	x23, x23, #1
	bcs	L_fe_pow22523_7
	add	x0, x29, #16
#ifndef NDEBUG
	add	x1, x29, #48
#endif /* !NDEBUG */
	add	x2, x29, #16
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	mov	x23, #1
#ifndef NDEBUG
	add	x0, x29, #16
#endif /* !NDEBUG */
	add	x1, x29, #16
L_fe_pow22523_8:
#ifndef __APPLE__
	bl	fe_sq
#else
	bl	_fe_sq
#endif /* __APPLE__ */
	subs	x23, x23, #1
	bcs	L_fe_pow22523_8
	ldr	x0, [x29, #112]
#ifndef NDEBUG
	add	x1, x29, #16
#endif /* !NDEBUG */
	ldr	x2, [x29, #120]
#ifndef __APPLE__
	bl	fe_mul
#else
	bl	_fe_mul
#endif /* __APPLE__ */
	ldr	x23, [x29, #136]
	ldp	x29, x30, [sp], #0x90
	ret
#ifndef __APPLE__
	.size	fe_pow22523,.-fe_pow22523
#endif /* __APPLE__ */
#ifndef __APPLE__
.text
.globl	fe_ge_to_p2
.type	fe_ge_to_p2,@function
.align	2
fe_ge_to_p2:
#else
.section	__TEXT,__text
.globl	_fe_ge_to_p2
.p2align	2
_fe_ge_to_p2:
#endif /* __APPLE__ */
	stp	x29, x30, [sp, #-112]!
	add	x29, sp, #0
	str	x17, [x29, #72]
	str	x19, [x29, #80]
	stp	x20, x21, [x29, #88]
	str	x22, [x29, #104]
	str	x1, [x29, #16]
	str	x2, [x29, #24]
	str	x3, [x29, #32]
	str	x4, [x29, #40]
	str	x5, [x29, #48]
	str	x6, [x29, #56]
	ldr	x1, [x29, #32]
	ldr	x2, [x29, #56]
	# Multiply
	ldp	x11, x12, [x1]
	ldp	x13, x14, [x1, #16]
	ldp	x15, x16, [x2]
	ldp	x17, x19, [x2, #16]
	#  A[0] * B[0]
	mul	x3, x11, x15
	umulh	x4, x11, x15
	#  A[0] * B[1]
	mul	x20, x11, x16
	umulh	x5, x11, x16
	adds	x4, x4, x20
	adc	x5, x5, xzr
	#  A[1] * B[0]
	mul	x20, x12, x15
	umulh	x21, x12, x15
	adds	x4, x4, x20
	adcs	x5, x5, x21
	adc	x6, xzr, xzr
	#  A[0] * B[2]
	mul	x20, x11, x17
	umulh	x21, x11, x17
	adds	x5, x5, x20
	adc	x6, x6, x21
	#  A[1] * B[1]
	mul	x20, x12, x16
	umulh	x21, x12, x16
	adds	x5, x5, x20
	adcs	x6, x6, x21
	adc	x7, xzr, xzr
	#  A[2] * B[0]
	mul	x20, x13, x15
	umulh	x21, x13, x15
	adds	x5, x5, x20
	adcs	x6, x6, x21
	adc	x7, x7, xzr
	#  A[0] * B[3]
	mul	x20, x11, x19
	umulh	x21, x11, x19
	adds	x6, x6, x20
	adcs	x7, x7, x21
	adc	x8, xzr, xzr
	#  A[1] * B[2]
	mul	x20, x12, x17
	umulh	x21, x12, x17
	adds	x6, x6, x20
	adcs	x7, x7, x21
	adc	x8, x8, xzr
	#  A[2] * B[1]
	mul	x20, x13, x16
	umulh	x21, x13, x16
	adds	x6, x6, x20
	adcs	x7, x7, x21
	adc	x8, x8, xzr
	#  A[3] * B[0]
	mul	x20, x14, x15
	umulh	x21, x14, x15
	adds	x6, x6, x20
	adcs	x7, x7, x21
	adc	x8, x8, xzr
	#  A[1] * B[3]
	mul	x20, x12, x19
	umulh	x21, x12, x19
	adds	x7, x7, x20
	adcs	x8, x8, x21
	adc	x9, xzr, xzr
	#  A[2] * B[2]
	mul	x20, x13, x17
	umulh	x21, x13, x17
	adds	x7, x7, x20
	adcs	x8, x8, x21
	adc	x9, x9, xzr
	#  A[3] * B[1]
	mul	x20, x14, x16
	umulh	x21, x14, x16
	adds	x7, x7, x20
	adcs	x8, x8, x21
	adc	x9, x9, xzr
	#  A[2] * B[3]
	mul	x20, x13, x19
	umulh	x21, x13, x19
	adds	x8, x8, x20
	adcs	x9, x9, x21
	adc	x10, xzr, xzr
	#  A[3] * B[2]
	mul	x20, x14, x17
	umulh	x21, x14, x17
	adds	x8, x8, x20
	adcs	x9, x9, x21
	adc	x10, x10, xzr
	#  A[3] * B[3]
	mul	x20, x14, x19
	umulh	x21, x14, x19
	adds	x9, x9, x20
	adc	x10, x10, x21
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x10, x10, x9, #63
	extr	x9, x9, x8, #63
	extr	x8, x8, x7, #63
	extr	x7, x7, x6, #63
	and	x6, x6, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x20, #19
	mul	x21, x20, x7
	umulh	x7, x20, x7
	adds	x3, x3, x21
	mul	x21, x20, x8
	umulh	x8, x20, x8
	adcs	x4, x4, x21
	mul	x21, x20, x9
	umulh	x9, x20, x9
	adcs	x5, x5, x21
	mul	x21, x20, x10
	umulh	x22, x20, x10
	adcs	x6, x6, x21
	adc	x22, x22, xzr
	#  Add remaining product results in
	adds	x4, x4, x7
	adcs	x5, x5, x8
	adcs	x6, x6, x9
	adc	x22, x22, xzr
	#  Overflow
	extr	x22, x22, x6, #63
	mul	x22, x22, x20
	and	x6, x6, #0x7fffffffffffffff
	adds	x3, x3, x22
	adcs	x4, x4, xzr
	adcs	x5, x5, xzr
	adc	x6, x6, xzr
	# Reduce if top bit set
	and	x22, x20, x6, asr 63
	and	x6, x6, #0x7fffffffffffffff
	adds	x3, x3, x22
	adcs	x4, x4, xzr
	adcs	x5, x5, xzr
	adc	x6, x6, xzr
	# Store
	stp	x3, x4, [x0]
	stp	x5, x6, [x0, #16]
	ldr	x0, [x29, #16]
	ldr	x1, [x29, #40]
	ldr	x2, [x29, #48]
	# Multiply
	ldp	x11, x12, [x1]
	ldp	x13, x14, [x1, #16]
	ldp	x15, x16, [x2]
	ldp	x17, x19, [x2, #16]
	#  A[0] * B[0]
	mul	x3, x11, x15
	umulh	x4, x11, x15
	#  A[0] * B[1]
	mul	x20, x11, x16
	umulh	x5, x11, x16
	adds	x4, x4, x20
	adc	x5, x5, xzr
	#  A[1] * B[0]
	mul	x20, x12, x15
	umulh	x21, x12, x15
	adds	x4, x4, x20
	adcs	x5, x5, x21
	adc	x6, xzr, xzr
	#  A[0] * B[2]
	mul	x20, x11, x17
	umulh	x21, x11, x17
	adds	x5, x5, x20
	adc	x6, x6, x21
	#  A[1] * B[1]
	mul	x20, x12, x16
	umulh	x21, x12, x16
	adds	x5, x5, x20
	adcs	x6, x6, x21
	adc	x7, xzr, xzr
	#  A[2] * B[0]
	mul	x20, x13, x15
	umulh	x21, x13, x15
	adds	x5, x5, x20
	adcs	x6, x6, x21
	adc	x7, x7, xzr
	#  A[0] * B[3]
	mul	x20, x11, x19
	umulh	x21, x11, x19
	adds	x6, x6, x20
	adcs	x7, x7, x21
	adc	x8, xzr, xzr
	#  A[1] * B[2]
	mul	x20, x12, x17
	umulh	x21, x12, x17
	adds	x6, x6, x20
	adcs	x7, x7, x21
	adc	x8, x8, xzr
	#  A[2] * B[1]
	mul	x20, x13, x16
	umulh	x21, x13, x16
	adds	x6, x6, x20
	adcs	x7, x7, x21
	adc	x8, x8, xzr
	#  A[3] * B[0]
	mul	x20, x14, x15
	umulh	x21, x14, x15
	adds	x6, x6, x20
	adcs	x7, x7, x21
	adc	x8, x8, xzr
	#  A[1] * B[3]
	mul	x20, x12, x19
	umulh	x21, x12, x19
	adds	x7, x7, x20
	adcs	x8, x8, x21
	adc	x9, xzr, xzr
	#  A[2] * B[2]
	mul	x20, x13, x17
	umulh	x21, x13, x17
	adds	x7, x7, x20
	adcs	x8, x8, x21
	adc	x9, x9, xzr
	#  A[3] * B[1]
	mul	x20, x14, x16
	umulh	x21, x14, x16
	adds	x7, x7, x20
	adcs	x8, x8, x21
	adc	x9, x9, xzr
	#  A[2] * B[3]
	mul	x20, x13, x19
	umulh	x21, x13, x19
	adds	x8, x8, x20
	adcs	x9, x9, x21
	adc	x10, xzr, xzr
	#  A[3] * B[2]
	mul	x20, x14, x17
	umulh	x21, x14, x17
	adds	x8, x8, x20
	adcs	x9, x9, x21
	adc	x10, x10, xzr
	#  A[3] * B[3]
	mul	x20, x14, x19
	umulh	x21, x14, x19
	adds	x9, x9, x20
	adc	x10, x10, x21
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x10, x10, x9, #63
	extr	x9, x9, x8, #63
	extr	x8, x8, x7, #63
	extr	x7, x7, x6, #63
	and	x6, x6, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x20, #19
	mul	x21, x20, x7
	umulh	x7, x20, x7
	adds	x3, x3, x21
	mul	x21, x20, x8
	umulh	x8, x20, x8
	adcs	x4, x4, x21
	mul	x21, x20, x9
	umulh	x9, x20, x9
	adcs	x5, x5, x21
	mul	x21, x20, x10
	umulh	x22, x20, x10
	adcs	x6, x6, x21
	adc	x22, x22, xzr
	#  Add remaining product results in
	adds	x4, x4, x7
	adcs	x5, x5, x8
	adcs	x6, x6, x9
	adc	x22, x22, xzr
	#  Overflow
	extr	x22, x22, x6, #63
	mul	x22, x22, x20
	and	x6, x6, #0x7fffffffffffffff
	adds	x3, x3, x22
	adcs	x4, x4, xzr
	adcs	x5, x5, xzr
	adc	x6, x6, xzr
	# Reduce if top bit set
	and	x22, x20, x6, asr 63
	and	x6, x6, #0x7fffffffffffffff
	adds	x3, x3, x22
	adcs	x4, x4, xzr
	adcs	x5, x5, xzr
	adc	x6, x6, xzr
	# Store
	stp	x3, x4, [x0]
	stp	x5, x6, [x0, #16]
	ldr	x0, [x29, #24]
	ldr	x2, [x29, #56]
	# Multiply
	ldp	x11, x12, [x2]
	ldp	x13, x14, [x2, #16]
	#  A[0] * B[0]
	mul	x3, x15, x11
	umulh	x4, x15, x11
	#  A[0] * B[1]
	mul	x20, x15, x12
	umulh	x5, x15, x12
	adds	x4, x4, x20
	adc	x5, x5, xzr
	#  A[1] * B[0]
	mul	x20, x16, x11
	umulh	x21, x16, x11
	adds	x4, x4, x20
	adcs	x5, x5, x21
	adc	x6, xzr, xzr
	#  A[0] * B[2]
	mul	x20, x15, x13
	umulh	x21, x15, x13
	adds	x5, x5, x20
	adc	x6, x6, x21
	#  A[1] * B[1]
	mul	x20, x16, x12
	umulh	x21, x16, x12
	adds	x5, x5, x20
	adcs	x6, x6, x21
	adc	x7, xzr, xzr
	#  A[2] * B[0]
	mul	x20, x17, x11
	umulh	x21, x17, x11
	adds	x5, x5, x20
	adcs	x6, x6, x21
	adc	x7, x7, xzr
	#  A[0] * B[3]
	mul	x20, x15, x14
	umulh	x21, x15, x14
	adds	x6, x6, x20
	adcs	x7, x7, x21
	adc	x8, xzr, xzr
	#  A[1] * B[2]
	mul	x20, x16, x13
	umulh	x21, x16, x13
	adds	x6, x6, x20
	adcs	x7, x7, x21
	adc	x8, x8, xzr
	#  A[2] * B[1]
	mul	x20, x17, x12
	umulh	x21, x17, x12
	adds	x6, x6, x20
	adcs	x7, x7, x21
	adc	x8, x8, xzr
	#  A[3] * B[0]
	mul	x20, x19, x11
	umulh	x21, x19, x11
	adds	x6, x6, x20
	adcs	x7, x7, x21
	adc	x8, x8, xzr
	#  A[1] * B[3]
	mul	x20, x16, x14
	umulh	x21, x16, x14
	adds	x7, x7, x20
	adcs	x8, x8, x21
	adc	x9, xzr, xzr
	#  A[2] * B[2]
	mul	x20, x17, x13
	umulh	x21, x17, x13
	adds	x7, x7, x20
	adcs	x8, x8, x21
	adc	x9, x9, xzr
	#  A[3] * B[1]
	mul	x20, x19, x12
	umulh	x21, x19, x12
	adds	x7, x7, x20
	adcs	x8, x8, x21
	adc	x9, x9, xzr
	#  A[2] * B[3]
	mul	x20, x17, x14
	umulh	x21, x17, x14
	adds	x8, x8, x20
	adcs	x9, x9, x21
	adc	x10, xzr, xzr
	#  A[3] * B[2]
	mul	x20, x19, x13
	umulh	x21, x19, x13
	adds	x8, x8, x20
	adcs	x9, x9, x21
	adc	x10, x10, xzr
	#  A[3] * B[3]
	mul	x20, x19, x14
	umulh	x21, x19, x14
	adds	x9, x9, x20
	adc	x10, x10, x21
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x10, x10, x9, #63
	extr	x9, x9, x8, #63
	extr	x8, x8, x7, #63
	extr	x7, x7, x6, #63
	and	x6, x6, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x20, #19
	mul	x21, x20, x7
	umulh	x7, x20, x7
	adds	x3, x3, x21
	mul	x21, x20, x8
	umulh	x8, x20, x8
	adcs	x4, x4, x21
	mul	x21, x20, x9
	umulh	x9, x20, x9
	adcs	x5, x5, x21
	mul	x21, x20, x10
	umulh	x22, x20, x10
	adcs	x6, x6, x21
	adc	x22, x22, xzr
	#  Add remaining product results in
	adds	x4, x4, x7
	adcs	x5, x5, x8
	adcs	x6, x6, x9
	adc	x22, x22, xzr
	#  Overflow
	extr	x22, x22, x6, #63
	mul	x22, x22, x20
	and	x6, x6, #0x7fffffffffffffff
	adds	x3, x3, x22
	adcs	x4, x4, xzr
	adcs	x5, x5, xzr
	adc	x6, x6, xzr
	# Reduce if top bit set
	and	x22, x20, x6, asr 63
	and	x6, x6, #0x7fffffffffffffff
	adds	x3, x3, x22
	adcs	x4, x4, xzr
	adcs	x5, x5, xzr
	adc	x6, x6, xzr
	# Store
	stp	x3, x4, [x0]
	stp	x5, x6, [x0, #16]
	ldr	x17, [x29, #72]
	ldr	x19, [x29, #80]
	ldp	x20, x21, [x29, #88]
	ldr	x22, [x29, #104]
	ldp	x29, x30, [sp], #0x70
	ret
#ifndef __APPLE__
	.size	fe_ge_to_p2,.-fe_ge_to_p2
#endif /* __APPLE__ */
#ifndef __APPLE__
.text
.globl	fe_ge_to_p3
.type	fe_ge_to_p3,@function
.align	2
fe_ge_to_p3:
#else
.section	__TEXT,__text
.globl	_fe_ge_to_p3
.p2align	2
_fe_ge_to_p3:
#endif /* __APPLE__ */
	stp	x29, x30, [sp, #-160]!
	add	x29, sp, #0
	str	x17, [x29, #88]
	str	x19, [x29, #96]
	stp	x20, x21, [x29, #104]
	stp	x22, x23, [x29, #120]
	stp	x24, x25, [x29, #136]
	str	x26, [x29, #152]
	str	x1, [x29, #16]
	str	x2, [x29, #24]
	str	x3, [x29, #32]
	str	x4, [x29, #40]
	str	x5, [x29, #48]
	str	x6, [x29, #56]
	str	x7, [x29, #64]
	ldr	x1, [x29, #40]
	ldr	x2, [x29, #64]
	# Multiply
	ldp	x11, x12, [x1]
	ldp	x13, x14, [x1, #16]
	ldp	x15, x16, [x2]
	ldp	x17, x19, [x2, #16]
	#  A[0] * B[0]
	mul	x3, x11, x15
	umulh	x4, x11, x15
	#  A[0] * B[1]
	mul	x24, x11, x16
	umulh	x5, x11, x16
	adds	x4, x4, x24
	adc	x5, x5, xzr
	#  A[1] * B[0]
	mul	x24, x12, x15
	umulh	x25, x12, x15
	adds	x4, x4, x24
	adcs	x5, x5, x25
	adc	x6, xzr, xzr
	#  A[0] * B[2]
	mul	x24, x11, x17
	umulh	x25, x11, x17
	adds	x5, x5, x24
	adc	x6, x6, x25
	#  A[1] * B[1]
	mul	x24, x12, x16
	umulh	x25, x12, x16
	adds	x5, x5, x24
	adcs	x6, x6, x25
	adc	x7, xzr, xzr
	#  A[2] * B[0]
	mul	x24, x13, x15
	umulh	x25, x13, x15
	adds	x5, x5, x24
	adcs	x6, x6, x25
	adc	x7, x7, xzr
	#  A[0] * B[3]
	mul	x24, x11, x19
	umulh	x25, x11, x19
	adds	x6, x6, x24
	adcs	x7, x7, x25
	adc	x8, xzr, xzr
	#  A[1] * B[2]
	mul	x24, x12, x17
	umulh	x25, x12, x17
	adds	x6, x6, x24
	adcs	x7, x7, x25
	adc	x8, x8, xzr
	#  A[2] * B[1]
	mul	x24, x13, x16
	umulh	x25, x13, x16
	adds	x6, x6, x24
	adcs	x7, x7, x25
	adc	x8, x8, xzr
	#  A[3] * B[0]
	mul	x24, x14, x15
	umulh	x25, x14, x15
	adds	x6, x6, x24
	adcs	x7, x7, x25
	adc	x8, x8, xzr
	#  A[1] * B[3]
	mul	x24, x12, x19
	umulh	x25, x12, x19
	adds	x7, x7, x24
	adcs	x8, x8, x25
	adc	x9, xzr, xzr
	#  A[2] * B[2]
	mul	x24, x13, x17
	umulh	x25, x13, x17
	adds	x7, x7, x24
	adcs	x8, x8, x25
	adc	x9, x9, xzr
	#  A[3] * B[1]
	mul	x24, x14, x16
	umulh	x25, x14, x16
	adds	x7, x7, x24
	adcs	x8, x8, x25
	adc	x9, x9, xzr
	#  A[2] * B[3]
	mul	x24, x13, x19
	umulh	x25, x13, x19
	adds	x8, x8, x24
	adcs	x9, x9, x25
	adc	x10, xzr, xzr
	#  A[3] * B[2]
	mul	x24, x14, x17
	umulh	x25, x14, x17
	adds	x8, x8, x24
	adcs	x9, x9, x25
	adc	x10, x10, xzr
	#  A[3] * B[3]
	mul	x24, x14, x19
	umulh	x25, x14, x19
	adds	x9, x9, x24
	adc	x10, x10, x25
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x10, x10, x9, #63
	extr	x9, x9, x8, #63
	extr	x8, x8, x7, #63
	extr	x7, x7, x6, #63
	and	x6, x6, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x24, #19
	mul	x25, x24, x7
	umulh	x7, x24, x7
	adds	x3, x3, x25
	mul	x25, x24, x8
	umulh	x8, x24, x8
	adcs	x4, x4, x25
	mul	x25, x24, x9
	umulh	x9, x24, x9
	adcs	x5, x5, x25
	mul	x25, x24, x10
	umulh	x26, x24, x10
	adcs	x6, x6, x25
	adc	x26, x26, xzr
	#  Add remaining product results in
	adds	x4, x4, x7
	adcs	x5, x5, x8
	adcs	x6, x6, x9
	adc	x26, x26, xzr
	#  Overflow
	extr	x26, x26, x6, #63
	mul	x26, x26, x24
	and	x6, x6, #0x7fffffffffffffff
	adds	x3, x3, x26
	adcs	x4, x4, xzr
	adcs	x5, x5, xzr
	adc	x6, x6, xzr
	# Reduce if top bit set
	and	x26, x24, x6, asr 63
	and	x6, x6, #0x7fffffffffffffff
	adds	x3, x3, x26
	adcs	x4, x4, xzr
	adcs	x5, x5, xzr
	adc	x6, x6, xzr
	# Store
	stp	x3, x4, [x0]
	stp	x5, x6, [x0, #16]
	ldr	x0, [x29, #32]
	ldr	x2, [x29, #48]
	# Multiply
	ldp	x20, x21, [x2]
	ldp	x22, x23, [x2, #16]
	#  A[0] * B[0]
	mul	x3, x11, x20
	umulh	x4, x11, x20
	#  A[0] * B[1]
	mul	x24, x11, x21
	umulh	x5, x11, x21
	adds	x4, x4, x24
	adc	x5, x5, xzr
	#  A[1] * B[0]
	mul	x24, x12, x20
	umulh	x25, x12, x20
	adds	x4, x4, x24
	adcs	x5, x5, x25
	adc	x6, xzr, xzr
	#  A[0] * B[2]
	mul	x24, x11, x22
	umulh	x25, x11, x22
	adds	x5, x5, x24
	adc	x6, x6, x25
	#  A[1] * B[1]
	mul	x24, x12, x21
	umulh	x25, x12, x21
	adds	x5, x5, x24
	adcs	x6, x6, x25
	adc	x7, xzr, xzr
	#  A[2] * B[0]
	mul	x24, x13, x20
	umulh	x25, x13, x20
	adds	x5, x5, x24
	adcs	x6, x6, x25
	adc	x7, x7, xzr
	#  A[0] * B[3]
	mul	x24, x11, x23
	umulh	x25, x11, x23
	adds	x6, x6, x24
	adcs	x7, x7, x25
	adc	x8, xzr, xzr
	#  A[1] * B[2]
	mul	x24, x12, x22
	umulh	x25, x12, x22
	adds	x6, x6, x24
	adcs	x7, x7, x25
	adc	x8, x8, xzr
	#  A[2] * B[1]
	mul	x24, x13, x21
	umulh	x25, x13, x21
	adds	x6, x6, x24
	adcs	x7, x7, x25
	adc	x8, x8, xzr
	#  A[3] * B[0]
	mul	x24, x14, x20
	umulh	x25, x14, x20
	adds	x6, x6, x24
	adcs	x7, x7, x25
	adc	x8, x8, xzr
	#  A[1] * B[3]
	mul	x24, x12, x23
	umulh	x25, x12, x23
	adds	x7, x7, x24
	adcs	x8, x8, x25
	adc	x9, xzr, xzr
	#  A[2] * B[2]
	mul	x24, x13, x22
	umulh	x25, x13, x22
	adds	x7, x7, x24
	adcs	x8, x8, x25
	adc	x9, x9, xzr
	#  A[3] * B[1]
	mul	x24, x14, x21
	umulh	x25, x14, x21
	adds	x7, x7, x24
	adcs	x8, x8, x25
	adc	x9, x9, xzr
	#  A[2] * B[3]
	mul	x24, x13, x23
	umulh	x25, x13, x23
	adds	x8, x8, x24
	adcs	x9, x9, x25
	adc	x10, xzr, xzr
	#  A[3] * B[2]
	mul	x24, x14, x22
	umulh	x25, x14, x22
	adds	x8, x8, x24
	adcs	x9, x9, x25
	adc	x10, x10, xzr
	#  A[3] * B[3]
	mul	x24, x14, x23
	umulh	x25, x14, x23
	adds	x9, x9, x24
	adc	x10, x10, x25
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x10, x10, x9, #63
	extr	x9, x9, x8, #63
	extr	x8, x8, x7, #63
	extr	x7, x7, x6, #63
	and	x6, x6, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x24, #19
	mul	x25, x24, x7
	umulh	x7, x24, x7
	adds	x3, x3, x25
	mul	x25, x24, x8
	umulh	x8, x24, x8
	adcs	x4, x4, x25
	mul	x25, x24, x9
	umulh	x9, x24, x9
	adcs	x5, x5, x25
	mul	x25, x24, x10
	umulh	x26, x24, x10
	adcs	x6, x6, x25
	adc	x26, x26, xzr
	#  Add remaining product results in
	adds	x4, x4, x7
	adcs	x5, x5, x8
	adcs	x6, x6, x9
	adc	x26, x26, xzr
	#  Overflow
	extr	x26, x26, x6, #63
	mul	x26, x26, x24
	and	x6, x6, #0x7fffffffffffffff
	adds	x3, x3, x26
	adcs	x4, x4, xzr
	adcs	x5, x5, xzr
	adc	x6, x6, xzr
	# Reduce if top bit set
	and	x26, x24, x6, asr 63
	and	x6, x6, #0x7fffffffffffffff
	adds	x3, x3, x26
	adcs	x4, x4, xzr
	adcs	x5, x5, xzr
	adc	x6, x6, xzr
	# Store
	stp	x3, x4, [x0]
	stp	x5, x6, [x0, #16]
	ldr	x0, [x29, #16]
	ldr	x2, [x29, #56]
	# Multiply
	ldp	x11, x12, [x2]
	ldp	x13, x14, [x2, #16]
	#  A[0] * B[0]
	mul	x3, x20, x11
	umulh	x4, x20, x11
	#  A[0] * B[1]
	mul	x24, x20, x12
	umulh	x5, x20, x12
	adds	x4, x4, x24
	adc	x5, x5, xzr
	#  A[1] * B[0]
	mul	x24, x21, x11
	umulh	x25, x21, x11
	adds	x4, x4, x24
	adcs	x5, x5, x25
	adc	x6, xzr, xzr
	#  A[0] * B[2]
	mul	x24, x20, x13
	umulh	x25, x20, x13
	adds	x5, x5, x24
	adc	x6, x6, x25
	#  A[1] * B[1]
	mul	x24, x21, x12
	umulh	x25, x21, x12
	adds	x5, x5, x24
	adcs	x6, x6, x25
	adc	x7, xzr, xzr
	#  A[2] * B[0]
	mul	x24, x22, x11
	umulh	x25, x22, x11
	adds	x5, x5, x24
	adcs	x6, x6, x25
	adc	x7, x7, xzr
	#  A[0] * B[3]
	mul	x24, x20, x14
	umulh	x25, x20, x14
	adds	x6, x6, x24
	adcs	x7, x7, x25
	adc	x8, xzr, xzr
	#  A[1] * B[2]
	mul	x24, x21, x13
	umulh	x25, x21, x13
	adds	x6, x6, x24
	adcs	x7, x7, x25
	adc	x8, x8, xzr
	#  A[2] * B[1]
	mul	x24, x22, x12
	umulh	x25, x22, x12
	adds	x6, x6, x24
	adcs	x7, x7, x25
	adc	x8, x8, xzr
	#  A[3] * B[0]
	mul	x24, x23, x11
	umulh	x25, x23, x11
	adds	x6, x6, x24
	adcs	x7, x7, x25
	adc	x8, x8, xzr
	#  A[1] * B[3]
	mul	x24, x21, x14
	umulh	x25, x21, x14
	adds	x7, x7, x24
	adcs	x8, x8, x25
	adc	x9, xzr, xzr
	#  A[2] * B[2]
	mul	x24, x22, x13
	umulh	x25, x22, x13
	adds	x7, x7, x24
	adcs	x8, x8, x25
	adc	x9, x9, xzr
	#  A[3] * B[1]
	mul	x24, x23, x12
	umulh	x25, x23, x12
	adds	x7, x7, x24
	adcs	x8, x8, x25
	adc	x9, x9, xzr
	#  A[2] * B[3]
	mul	x24, x22, x14
	umulh	x25, x22, x14
	adds	x8, x8, x24
	adcs	x9, x9, x25
	adc	x10, xzr, xzr
	#  A[3] * B[2]
	mul	x24, x23, x13
	umulh	x25, x23, x13
	adds	x8, x8, x24
	adcs	x9, x9, x25
	adc	x10, x10, xzr
	#  A[3] * B[3]
	mul	x24, x23, x14
	umulh	x25, x23, x14
	adds	x9, x9, x24
	adc	x10, x10, x25
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x10, x10, x9, #63
	extr	x9, x9, x8, #63
	extr	x8, x8, x7, #63
	extr	x7, x7, x6, #63
	and	x6, x6, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x24, #19
	mul	x25, x24, x7
	umulh	x7, x24, x7
	adds	x3, x3, x25
	mul	x25, x24, x8
	umulh	x8, x24, x8
	adcs	x4, x4, x25
	mul	x25, x24, x9
	umulh	x9, x24, x9
	adcs	x5, x5, x25
	mul	x25, x24, x10
	umulh	x26, x24, x10
	adcs	x6, x6, x25
	adc	x26, x26, xzr
	#  Add remaining product results in
	adds	x4, x4, x7
	adcs	x5, x5, x8
	adcs	x6, x6, x9
	adc	x26, x26, xzr
	#  Overflow
	extr	x26, x26, x6, #63
	mul	x26, x26, x24
	and	x6, x6, #0x7fffffffffffffff
	adds	x3, x3, x26
	adcs	x4, x4, xzr
	adcs	x5, x5, xzr
	adc	x6, x6, xzr
	# Reduce if top bit set
	and	x26, x24, x6, asr 63
	and	x6, x6, #0x7fffffffffffffff
	adds	x3, x3, x26
	adcs	x4, x4, xzr
	adcs	x5, x5, xzr
	adc	x6, x6, xzr
	# Store
	stp	x3, x4, [x0]
	stp	x5, x6, [x0, #16]
	ldr	x0, [x29, #24]
	# Multiply
	#  A[0] * B[0]
	mul	x3, x11, x15
	umulh	x4, x11, x15
	#  A[0] * B[1]
	mul	x24, x11, x16
	umulh	x5, x11, x16
	adds	x4, x4, x24
	adc	x5, x5, xzr
	#  A[1] * B[0]
	mul	x24, x12, x15
	umulh	x25, x12, x15
	adds	x4, x4, x24
	adcs	x5, x5, x25
	adc	x6, xzr, xzr
	#  A[0] * B[2]
	mul	x24, x11, x17
	umulh	x25, x11, x17
	adds	x5, x5, x24
	adc	x6, x6, x25
	#  A[1] * B[1]
	mul	x24, x12, x16
	umulh	x25, x12, x16
	adds	x5, x5, x24
	adcs	x6, x6, x25
	adc	x7, xzr, xzr
	#  A[2] * B[0]
	mul	x24, x13, x15
	umulh	x25, x13, x15
	adds	x5, x5, x24
	adcs	x6, x6, x25
	adc	x7, x7, xzr
	#  A[0] * B[3]
	mul	x24, x11, x19
	umulh	x25, x11, x19
	adds	x6, x6, x24
	adcs	x7, x7, x25
	adc	x8, xzr, xzr
	#  A[1] * B[2]
	mul	x24, x12, x17
	umulh	x25, x12, x17
	adds	x6, x6, x24
	adcs	x7, x7, x25
	adc	x8, x8, xzr
	#  A[2] * B[1]
	mul	x24, x13, x16
	umulh	x25, x13, x16
	adds	x6, x6, x24
	adcs	x7, x7, x25
	adc	x8, x8, xzr
	#  A[3] * B[0]
	mul	x24, x14, x15
	umulh	x25, x14, x15
	adds	x6, x6, x24
	adcs	x7, x7, x25
	adc	x8, x8, xzr
	#  A[1] * B[3]
	mul	x24, x12, x19
	umulh	x25, x12, x19
	adds	x7, x7, x24
	adcs	x8, x8, x25
	adc	x9, xzr, xzr
	#  A[2] * B[2]
	mul	x24, x13, x17
	umulh	x25, x13, x17
	adds	x7, x7, x24
	adcs	x8, x8, x25
	adc	x9, x9, xzr
	#  A[3] * B[1]
	mul	x24, x14, x16
	umulh	x25, x14, x16
	adds	x7, x7, x24
	adcs	x8, x8, x25
	adc	x9, x9, xzr
	#  A[2] * B[3]
	mul	x24, x13, x19
	umulh	x25, x13, x19
	adds	x8, x8, x24
	adcs	x9, x9, x25
	adc	x10, xzr, xzr
	#  A[3] * B[2]
	mul	x24, x14, x17
	umulh	x25, x14, x17
	adds	x8, x8, x24
	adcs	x9, x9, x25
	adc	x10, x10, xzr
	#  A[3] * B[3]
	mul	x24, x14, x19
	umulh	x25, x14, x19
	adds	x9, x9, x24
	adc	x10, x10, x25
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x10, x10, x9, #63
	extr	x9, x9, x8, #63
	extr	x8, x8, x7, #63
	extr	x7, x7, x6, #63
	and	x6, x6, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x24, #19
	mul	x25, x24, x7
	umulh	x7, x24, x7
	adds	x3, x3, x25
	mul	x25, x24, x8
	umulh	x8, x24, x8
	adcs	x4, x4, x25
	mul	x25, x24, x9
	umulh	x9, x24, x9
	adcs	x5, x5, x25
	mul	x25, x24, x10
	umulh	x26, x24, x10
	adcs	x6, x6, x25
	adc	x26, x26, xzr
	#  Add remaining product results in
	adds	x4, x4, x7
	adcs	x5, x5, x8
	adcs	x6, x6, x9
	adc	x26, x26, xzr
	#  Overflow
	extr	x26, x26, x6, #63
	mul	x26, x26, x24
	and	x6, x6, #0x7fffffffffffffff
	adds	x3, x3, x26
	adcs	x4, x4, xzr
	adcs	x5, x5, xzr
	adc	x6, x6, xzr
	# Reduce if top bit set
	and	x26, x24, x6, asr 63
	and	x6, x6, #0x7fffffffffffffff
	adds	x3, x3, x26
	adcs	x4, x4, xzr
	adcs	x5, x5, xzr
	adc	x6, x6, xzr
	# Store
	stp	x3, x4, [x0]
	stp	x5, x6, [x0, #16]
	ldr	x17, [x29, #88]
	ldr	x19, [x29, #96]
	ldp	x20, x21, [x29, #104]
	ldp	x22, x23, [x29, #120]
	ldp	x24, x25, [x29, #136]
	ldr	x26, [x29, #152]
	ldp	x29, x30, [sp], #0xa0
	ret
#ifndef __APPLE__
	.size	fe_ge_to_p3,.-fe_ge_to_p3
#endif /* __APPLE__ */
#ifndef __APPLE__
.text
.globl	fe_ge_dbl
.type	fe_ge_dbl,@function
.align	2
fe_ge_dbl:
#else
.section	__TEXT,__text
.globl	_fe_ge_dbl
.p2align	2
_fe_ge_dbl:
#endif /* __APPLE__ */
	stp	x29, x30, [sp, #-176]!
	add	x29, sp, #0
	str	x17, [x29, #88]
	str	x19, [x29, #96]
	stp	x20, x21, [x29, #104]
	stp	x22, x23, [x29, #120]
	stp	x24, x25, [x29, #136]
	stp	x26, x27, [x29, #152]
	str	x28, [x29, #168]
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	str	x2, [x29, #32]
	str	x3, [x29, #40]
	str	x4, [x29, #48]
	str	x5, [x29, #56]
	str	x6, [x29, #64]
	ldr	x1, [x29, #48]
	# Square
	ldp	x12, x13, [x1]
	ldp	x14, x15, [x1, #16]
	#  A[0] * A[1]
	mul	x5, x12, x13
	umulh	x6, x12, x13
	#  A[0] * A[2]
	mul	x25, x12, x14
	umulh	x7, x12, x14
	adds	x6, x6, x25
	adc	x7, x7, xzr
	#  A[0] * A[3]
	mul	x25, x12, x15
	umulh	x8, x12, x15
	adds	x7, x7, x25
	adc	x8, x8, xzr
	#  A[1] * A[2]
	mul	x25, x13, x14
	umulh	x26, x13, x14
	adds	x7, x7, x25
	adcs	x8, x8, x26
	adc	x9, xzr, xzr
	#  A[1] * A[3]
	mul	x25, x13, x15
	umulh	x26, x13, x15
	adds	x8, x8, x25
	adc	x9, x9, x26
	#  A[2] * A[3]
	mul	x25, x14, x15
	umulh	x10, x14, x15
	adds	x9, x9, x25
	adc	x10, x10, xzr
	# Double
	adds	x5, x5, x5
	adcs	x6, x6, x6
	adcs	x7, x7, x7
	adcs	x8, x8, x8
	adcs	x9, x9, x9
	adcs	x10, x10, x10
	adc	x11, xzr, xzr
	#  A[0] * A[0]
	mul	x4, x12, x12
	umulh	x27, x12, x12
	#  A[1] * A[1]
	mul	x25, x13, x13
	umulh	x26, x13, x13
	adds	x5, x5, x27
	adcs	x6, x6, x25
	adc	x27, x26, xzr
	#  A[2] * A[2]
	mul	x25, x14, x14
	umulh	x26, x14, x14
	adds	x7, x7, x27
	adcs	x8, x8, x25
	adc	x27, x26, xzr
	#  A[3] * A[3]
	mul	x25, x15, x15
	umulh	x26, x15, x15
	adds	x9, x9, x27
	adcs	x10, x10, x25
	adc	x11, x11, x26
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x11, x11, x10, #63
	extr	x10, x10, x9, #63
	extr	x9, x9, x8, #63
	extr	x8, x8, x7, #63
	and	x7, x7, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x25, #19
	mul	x26, x25, x8
	umulh	x8, x25, x8
	adds	x4, x4, x26
	mul	x26, x25, x9
	umulh	x9, x25, x9
	adcs	x5, x5, x26
	mul	x26, x25, x10
	umulh	x10, x25, x10
	adcs	x6, x6, x26
	mul	x26, x25, x11
	umulh	x27, x25, x11
	adcs	x7, x7, x26
	adc	x27, x27, xzr
	#  Add remaining product results in
	adds	x5, x5, x8
	adcs	x6, x6, x9
	adcs	x7, x7, x10
	adc	x27, x27, xzr
	#  Overflow
	extr	x27, x27, x7, #63
	mul	x27, x27, x25
	and	x7, x7, #0x7fffffffffffffff
	adds	x4, x4, x27
	adcs	x5, x5, xzr
	adcs	x6, x6, xzr
	adc	x7, x7, xzr
	# Reduce if top bit set
	and	x27, x25, x7, asr 63
	and	x7, x7, #0x7fffffffffffffff
	adds	x4, x4, x27
	adcs	x5, x5, xzr
	adcs	x6, x6, xzr
	adc	x7, x7, xzr
	# Store
	stp	x4, x5, [x0]
	stp	x6, x7, [x0, #16]
	ldr	x0, [x29, #32]
	ldr	x1, [x29, #56]
	# Square
	ldp	x21, x22, [x1]
	ldp	x23, x24, [x1, #16]
	#  A[0] * A[1]
	mul	x9, x21, x22
	umulh	x10, x21, x22
	#  A[0] * A[2]
	mul	x25, x21, x23
	umulh	x11, x21, x23
	adds	x10, x10, x25
	adc	x11, x11, xzr
	#  A[0] * A[3]
	mul	x25, x21, x24
	umulh	x16, x21, x24
	adds	x11, x11, x25
	adc	x16, x16, xzr
	#  A[1] * A[2]
	mul	x25, x22, x23
	umulh	x26, x22, x23
	adds	x11, x11, x25
	adcs	x16, x16, x26
	adc	x17, xzr, xzr
	#  A[1] * A[3]
	mul	x25, x22, x24
	umulh	x26, x22, x24
	adds	x16, x16, x25
	adc	x17, x17, x26
	#  A[2] * A[3]
	mul	x25, x23, x24
	umulh	x19, x23, x24
	adds	x17, x17, x25
	adc	x19, x19, xzr
	# Double
	adds	x9, x9, x9
	adcs	x10, x10, x10
	adcs	x11, x11, x11
	adcs	x16, x16, x16
	adcs	x17, x17, x17
	adcs	x19, x19, x19
	adc	x20, xzr, xzr
	#  A[0] * A[0]
	mul	x8, x21, x21
	umulh	x27, x21, x21
	#  A[1] * A[1]
	mul	x25, x22, x22
	umulh	x26, x22, x22
	adds	x9, x9, x27
	adcs	x10, x10, x25
	adc	x27, x26, xzr
	#  A[2] * A[2]
	mul	x25, x23, x23
	umulh	x26, x23, x23
	adds	x11, x11, x27
	adcs	x16, x16, x25
	adc	x27, x26, xzr
	#  A[3] * A[3]
	mul	x25, x24, x24
	umulh	x26, x24, x24
	adds	x17, x17, x27
	adcs	x19, x19, x25
	adc	x20, x20, x26
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x20, x20, x19, #63
	extr	x19, x19, x17, #63
	extr	x17, x17, x16, #63
	extr	x16, x16, x11, #63
	and	x11, x11, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x25, #19
	mul	x26, x25, x16
	umulh	x16, x25, x16
	adds	x8, x8, x26
	mul	x26, x25, x17
	umulh	x17, x25, x17
	adcs	x9, x9, x26
	mul	x26, x25, x19
	umulh	x19, x25, x19
	adcs	x10, x10, x26
	mul	x26, x25, x20
	umulh	x27, x25, x20
	adcs	x11, x11, x26
	adc	x27, x27, xzr
	#  Add remaining product results in
	adds	x9, x9, x16
	adcs	x10, x10, x17
	adcs	x11, x11, x19
	adc	x27, x27, xzr
	#  Overflow
	extr	x27, x27, x11, #63
	mul	x27, x27, x25
	and	x11, x11, #0x7fffffffffffffff
	adds	x8, x8, x27
	adcs	x9, x9, xzr
	adcs	x10, x10, xzr
	adc	x11, x11, xzr
	# Reduce if top bit set
	and	x27, x25, x11, asr 63
	and	x11, x11, #0x7fffffffffffffff
	adds	x8, x8, x27
	adcs	x9, x9, xzr
	adcs	x10, x10, xzr
	adc	x11, x11, xzr
	# Store
	stp	x8, x9, [x0]
	stp	x10, x11, [x0, #16]
	ldr	x0, [x29, #24]
	# Add
	adds	x12, x12, x21
	adcs	x13, x13, x22
	adcs	x14, x14, x23
	adc	x15, x15, x24
	mov	x25, #-19
	asr	x28, x15, #63
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Sub modulus (if overflow)
	subs	x12, x12, x25
	sbcs	x13, x13, x28
	sbcs	x14, x14, x28
	sbc	x15, x15, x26
	ldr	x0, [x29, #40]
	# Square
	#  A[0] * A[1]
	mul	x17, x12, x13
	umulh	x19, x12, x13
	#  A[0] * A[2]
	mul	x25, x12, x14
	umulh	x20, x12, x14
	adds	x19, x19, x25
	adc	x20, x20, xzr
	#  A[0] * A[3]
	mul	x25, x12, x15
	umulh	x21, x12, x15
	adds	x20, x20, x25
	adc	x21, x21, xzr
	#  A[1] * A[2]
	mul	x25, x13, x14
	umulh	x26, x13, x14
	adds	x20, x20, x25
	adcs	x21, x21, x26
	adc	x22, xzr, xzr
	#  A[1] * A[3]
	mul	x25, x13, x15
	umulh	x26, x13, x15
	adds	x21, x21, x25
	adc	x22, x22, x26
	#  A[2] * A[3]
	mul	x25, x14, x15
	umulh	x23, x14, x15
	adds	x22, x22, x25
	adc	x23, x23, xzr
	# Double
	adds	x17, x17, x17
	adcs	x19, x19, x19
	adcs	x20, x20, x20
	adcs	x21, x21, x21
	adcs	x22, x22, x22
	adcs	x23, x23, x23
	adc	x24, xzr, xzr
	#  A[0] * A[0]
	mul	x16, x12, x12
	umulh	x27, x12, x12
	#  A[1] * A[1]
	mul	x25, x13, x13
	umulh	x26, x13, x13
	adds	x17, x17, x27
	adcs	x19, x19, x25
	adc	x27, x26, xzr
	#  A[2] * A[2]
	mul	x25, x14, x14
	umulh	x26, x14, x14
	adds	x20, x20, x27
	adcs	x21, x21, x25
	adc	x27, x26, xzr
	#  A[3] * A[3]
	mul	x25, x15, x15
	umulh	x26, x15, x15
	adds	x22, x22, x27
	adcs	x23, x23, x25
	adc	x24, x24, x26
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x24, x24, x23, #63
	extr	x23, x23, x22, #63
	extr	x22, x22, x21, #63
	extr	x21, x21, x20, #63
	and	x20, x20, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x25, #19
	mul	x26, x25, x21
	umulh	x21, x25, x21
	adds	x16, x16, x26
	mul	x26, x25, x22
	umulh	x22, x25, x22
	adcs	x17, x17, x26
	mul	x26, x25, x23
	umulh	x23, x25, x23
	adcs	x19, x19, x26
	mul	x26, x25, x24
	umulh	x27, x25, x24
	adcs	x20, x20, x26
	adc	x27, x27, xzr
	#  Add remaining product results in
	adds	x17, x17, x21
	adcs	x19, x19, x22
	adcs	x20, x20, x23
	adc	x27, x27, xzr
	#  Overflow
	extr	x27, x27, x20, #63
	mul	x27, x27, x25
	and	x20, x20, #0x7fffffffffffffff
	adds	x16, x16, x27
	adcs	x17, x17, xzr
	adcs	x19, x19, xzr
	adc	x20, x20, xzr
	# Reduce if top bit set
	and	x27, x25, x20, asr 63
	and	x20, x20, #0x7fffffffffffffff
	adds	x16, x16, x27
	adcs	x17, x17, xzr
	adcs	x19, x19, xzr
	adc	x20, x20, xzr
	# Store
	stp	x16, x17, [x0]
	stp	x19, x20, [x0, #16]
	ldr	x0, [x29, #24]
	ldr	x1, [x29, #32]
	# Add
	adds	x12, x8, x4
	adcs	x13, x9, x5
	adcs	x14, x10, x6
	adc	x15, x11, x7
	mov	x25, #-19
	asr	x28, x15, #63
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Sub modulus (if overflow)
	subs	x12, x12, x25
	sbcs	x13, x13, x28
	sbcs	x14, x14, x28
	sbc	x15, x15, x26
	# Sub
	subs	x21, x8, x4
	sbcs	x22, x9, x5
	sbcs	x23, x10, x6
	sbcs	x24, x11, x7
	mov	x25, #-19
	csetm	x28, cc
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Add modulus (if underflow)
	adds	x21, x21, x25
	adcs	x22, x22, x28
	adcs	x23, x23, x28
	adc	x24, x24, x26
	stp	x12, x13, [x0]
	stp	x14, x15, [x0, #16]
	stp	x21, x22, [x1]
	stp	x23, x24, [x1, #16]
	ldr	x0, [x29, #16]
	# Sub
	subs	x16, x16, x12
	sbcs	x17, x17, x13
	sbcs	x19, x19, x14
	sbcs	x20, x20, x15
	mov	x25, #-19
	csetm	x28, cc
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Add modulus (if underflow)
	adds	x16, x16, x25
	adcs	x17, x17, x28
	adcs	x19, x19, x28
	adc	x20, x20, x26
	stp	x16, x17, [x0]
	stp	x19, x20, [x0, #16]
	ldr	x0, [x29, #40]
	ldr	x1, [x29, #64]
	# Square * 2
	ldp	x12, x13, [x1]
	ldp	x14, x15, [x1, #16]
	#  A[0] * A[1]
	mul	x5, x12, x13
	umulh	x6, x12, x13
	#  A[0] * A[2]
	mul	x25, x12, x14
	umulh	x7, x12, x14
	adds	x6, x6, x25
	adc	x7, x7, xzr
	#  A[0] * A[3]
	mul	x25, x12, x15
	umulh	x8, x12, x15
	adds	x7, x7, x25
	adc	x8, x8, xzr
	#  A[1] * A[2]
	mul	x25, x13, x14
	umulh	x26, x13, x14
	adds	x7, x7, x25
	adcs	x8, x8, x26
	adc	x9, xzr, xzr
	#  A[1] * A[3]
	mul	x25, x13, x15
	umulh	x26, x13, x15
	adds	x8, x8, x25
	adc	x9, x9, x26
	#  A[2] * A[3]
	mul	x25, x14, x15
	umulh	x10, x14, x15
	adds	x9, x9, x25
	adc	x10, x10, xzr
	# Double
	adds	x5, x5, x5
	adcs	x6, x6, x6
	adcs	x7, x7, x7
	adcs	x8, x8, x8
	adcs	x9, x9, x9
	adcs	x10, x10, x10
	adc	x11, xzr, xzr
	#  A[0] * A[0]
	mul	x4, x12, x12
	umulh	x28, x12, x12
	#  A[1] * A[1]
	mul	x25, x13, x13
	umulh	x26, x13, x13
	adds	x5, x5, x28
	adcs	x6, x6, x25
	adc	x28, x26, xzr
	#  A[2] * A[2]
	mul	x25, x14, x14
	umulh	x26, x14, x14
	adds	x7, x7, x28
	adcs	x8, x8, x25
	adc	x28, x26, xzr
	#  A[3] * A[3]
	mul	x25, x15, x15
	umulh	x26, x15, x15
	adds	x9, x9, x28
	adcs	x10, x10, x25
	adc	x11, x11, x26
	# Double and Reduce
	mov	x25, #0x169
	#  Move top half into t4-t7 and remove top bit from t3
	lsr	x28, x11, #61
	extr	x11, x11, x10, #62
	extr	x10, x10, x9, #62
	extr	x9, x9, x8, #62
	extr	x8, x8, x7, #62
	extr	x7, x7, x6, #63
	extr	x6, x6, x5, #63
	extr	x5, x5, x4, #63
	lsl	x4, x4, #1
	and	x7, x7, #0x7fffffffffffffff
	#  Two left, only one right
	and	x11, x11, #0x7fffffffffffffff
	#  Multiply top bits by 19*19
	mul	x28, x28, x25
	#  Multiply top half by 19
	mov	x25, #19
	mul	x26, x25, x8
	umulh	x8, x25, x8
	adds	x4, x4, x26
	mul	x26, x25, x9
	umulh	x9, x25, x9
	adcs	x5, x5, x26
	mul	x26, x25, x10
	umulh	x10, x25, x10
	adcs	x6, x6, x26
	mul	x26, x25, x11
	umulh	x27, x25, x11
	adcs	x7, x7, x26
	adc	x27, x27, xzr
	#  Add remaining product results in
	adds	x4, x4, x28
	adcs	x5, x5, x8
	adcs	x6, x6, x9
	adcs	x7, x7, x10
	adc	x27, x27, xzr
	#  Overflow
	extr	x27, x27, x7, #63
	mul	x27, x27, x25
	and	x7, x7, #0x7fffffffffffffff
	adds	x4, x4, x27
	adcs	x5, x5, xzr
	adcs	x6, x6, xzr
	adc	x7, x7, xzr
	# Reduce if top bit set
	and	x27, x25, x7, asr 63
	and	x7, x7, #0x7fffffffffffffff
	adds	x4, x4, x27
	adcs	x5, x5, xzr
	adcs	x6, x6, xzr
	adc	x7, x7, xzr
	# Store
	ldr	x0, [x29, #40]
	# Sub
	subs	x4, x4, x21
	sbcs	x5, x5, x22
	sbcs	x6, x6, x23
	sbcs	x7, x7, x24
	mov	x25, #-19
	csetm	x28, cc
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Add modulus (if underflow)
	adds	x4, x4, x25
	adcs	x5, x5, x28
	adcs	x6, x6, x28
	adc	x7, x7, x26
	stp	x4, x5, [x0]
	stp	x6, x7, [x0, #16]
	ldr	x17, [x29, #88]
	ldr	x19, [x29, #96]
	ldp	x20, x21, [x29, #104]
	ldp	x22, x23, [x29, #120]
	ldp	x24, x25, [x29, #136]
	ldp	x26, x27, [x29, #152]
	ldr	x28, [x29, #168]
	ldp	x29, x30, [sp], #0xb0
	ret
#ifndef __APPLE__
	.size	fe_ge_dbl,.-fe_ge_dbl
#endif /* __APPLE__ */
#ifndef __APPLE__
.text
.globl	fe_ge_madd
.type	fe_ge_madd,@function
.align	2
fe_ge_madd:
#else
.section	__TEXT,__text
.globl	_fe_ge_madd
.p2align	2
_fe_ge_madd:
#endif /* __APPLE__ */
	stp	x29, x30, [sp, #-176]!
	add	x29, sp, #0
	str	x17, [x29, #88]
	str	x19, [x29, #96]
	stp	x20, x21, [x29, #104]
	stp	x22, x23, [x29, #120]
	stp	x24, x25, [x29, #136]
	stp	x26, x27, [x29, #152]
	str	x28, [x29, #168]
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	str	x2, [x29, #32]
	str	x3, [x29, #40]
	str	x4, [x29, #48]
	str	x5, [x29, #56]
	str	x6, [x29, #64]
	str	x7, [x29, #72]
	ldr	x2, [x29, #56]
	ldr	x3, [x29, #48]
	# Add
	ldp	x12, x13, [x2]
	ldp	x14, x15, [x2, #16]
	ldp	x16, x17, [x3]
	ldp	x19, x20, [x3, #16]
	adds	x4, x12, x16
	adcs	x5, x13, x17
	adcs	x6, x14, x19
	adc	x7, x15, x20
	mov	x25, #-19
	asr	x28, x7, #63
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Sub modulus (if overflow)
	subs	x4, x4, x25
	sbcs	x5, x5, x28
	sbcs	x6, x6, x28
	sbc	x7, x7, x26
	# Sub
	subs	x8, x12, x16
	sbcs	x9, x13, x17
	sbcs	x10, x14, x19
	sbcs	x11, x15, x20
	mov	x25, #-19
	csetm	x28, cc
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Add modulus (if underflow)
	adds	x8, x8, x25
	adcs	x9, x9, x28
	adcs	x10, x10, x28
	adc	x11, x11, x26
	ldr	x0, [x29, #32]
	ldr	x2, [x29, #184]
	# Multiply
	ldp	x21, x22, [x2]
	ldp	x23, x24, [x2, #16]
	#  A[0] * B[0]
	mul	x12, x4, x21
	umulh	x13, x4, x21
	#  A[0] * B[1]
	mul	x25, x4, x22
	umulh	x14, x4, x22
	adds	x13, x13, x25
	adc	x14, x14, xzr
	#  A[1] * B[0]
	mul	x25, x5, x21
	umulh	x26, x5, x21
	adds	x13, x13, x25
	adcs	x14, x14, x26
	adc	x15, xzr, xzr
	#  A[0] * B[2]
	mul	x25, x4, x23
	umulh	x26, x4, x23
	adds	x14, x14, x25
	adc	x15, x15, x26
	#  A[1] * B[1]
	mul	x25, x5, x22
	umulh	x26, x5, x22
	adds	x14, x14, x25
	adcs	x15, x15, x26
	adc	x16, xzr, xzr
	#  A[2] * B[0]
	mul	x25, x6, x21
	umulh	x26, x6, x21
	adds	x14, x14, x25
	adcs	x15, x15, x26
	adc	x16, x16, xzr
	#  A[0] * B[3]
	mul	x25, x4, x24
	umulh	x26, x4, x24
	adds	x15, x15, x25
	adcs	x16, x16, x26
	adc	x17, xzr, xzr
	#  A[1] * B[2]
	mul	x25, x5, x23
	umulh	x26, x5, x23
	adds	x15, x15, x25
	adcs	x16, x16, x26
	adc	x17, x17, xzr
	#  A[2] * B[1]
	mul	x25, x6, x22
	umulh	x26, x6, x22
	adds	x15, x15, x25
	adcs	x16, x16, x26
	adc	x17, x17, xzr
	#  A[3] * B[0]
	mul	x25, x7, x21
	umulh	x26, x7, x21
	adds	x15, x15, x25
	adcs	x16, x16, x26
	adc	x17, x17, xzr
	#  A[1] * B[3]
	mul	x25, x5, x24
	umulh	x26, x5, x24
	adds	x16, x16, x25
	adcs	x17, x17, x26
	adc	x19, xzr, xzr
	#  A[2] * B[2]
	mul	x25, x6, x23
	umulh	x26, x6, x23
	adds	x16, x16, x25
	adcs	x17, x17, x26
	adc	x19, x19, xzr
	#  A[3] * B[1]
	mul	x25, x7, x22
	umulh	x26, x7, x22
	adds	x16, x16, x25
	adcs	x17, x17, x26
	adc	x19, x19, xzr
	#  A[2] * B[3]
	mul	x25, x6, x24
	umulh	x26, x6, x24
	adds	x17, x17, x25
	adcs	x19, x19, x26
	adc	x20, xzr, xzr
	#  A[3] * B[2]
	mul	x25, x7, x23
	umulh	x26, x7, x23
	adds	x17, x17, x25
	adcs	x19, x19, x26
	adc	x20, x20, xzr
	#  A[3] * B[3]
	mul	x25, x7, x24
	umulh	x26, x7, x24
	adds	x19, x19, x25
	adc	x20, x20, x26
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x20, x20, x19, #63
	extr	x19, x19, x17, #63
	extr	x17, x17, x16, #63
	extr	x16, x16, x15, #63
	and	x15, x15, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x25, #19
	mul	x26, x25, x16
	umulh	x16, x25, x16
	adds	x12, x12, x26
	mul	x26, x25, x17
	umulh	x17, x25, x17
	adcs	x13, x13, x26
	mul	x26, x25, x19
	umulh	x19, x25, x19
	adcs	x14, x14, x26
	mul	x26, x25, x20
	umulh	x27, x25, x20
	adcs	x15, x15, x26
	adc	x27, x27, xzr
	#  Add remaining product results in
	adds	x13, x13, x16
	adcs	x14, x14, x17
	adcs	x15, x15, x19
	adc	x27, x27, xzr
	#  Overflow
	extr	x27, x27, x15, #63
	mul	x27, x27, x25
	and	x15, x15, #0x7fffffffffffffff
	adds	x12, x12, x27
	adcs	x13, x13, xzr
	adcs	x14, x14, xzr
	adc	x15, x15, xzr
	# Reduce if top bit set
	and	x27, x25, x15, asr 63
	and	x15, x15, #0x7fffffffffffffff
	adds	x12, x12, x27
	adcs	x13, x13, xzr
	adcs	x14, x14, xzr
	adc	x15, x15, xzr
	# Store
	ldr	x0, [x29, #24]
	ldr	x1, [x29, #192]
	# Multiply
	ldp	x21, x22, [x1]
	ldp	x23, x24, [x1, #16]
	#  A[0] * B[0]
	mul	x4, x8, x21
	umulh	x5, x8, x21
	#  A[0] * B[1]
	mul	x25, x8, x22
	umulh	x6, x8, x22
	adds	x5, x5, x25
	adc	x6, x6, xzr
	#  A[1] * B[0]
	mul	x25, x9, x21
	umulh	x26, x9, x21
	adds	x5, x5, x25
	adcs	x6, x6, x26
	adc	x7, xzr, xzr
	#  A[0] * B[2]
	mul	x25, x8, x23
	umulh	x26, x8, x23
	adds	x6, x6, x25
	adc	x7, x7, x26
	#  A[1] * B[1]
	mul	x25, x9, x22
	umulh	x26, x9, x22
	adds	x6, x6, x25
	adcs	x7, x7, x26
	adc	x16, xzr, xzr
	#  A[2] * B[0]
	mul	x25, x10, x21
	umulh	x26, x10, x21
	adds	x6, x6, x25
	adcs	x7, x7, x26
	adc	x16, x16, xzr
	#  A[0] * B[3]
	mul	x25, x8, x24
	umulh	x26, x8, x24
	adds	x7, x7, x25
	adcs	x16, x16, x26
	adc	x17, xzr, xzr
	#  A[1] * B[2]
	mul	x25, x9, x23
	umulh	x26, x9, x23
	adds	x7, x7, x25
	adcs	x16, x16, x26
	adc	x17, x17, xzr
	#  A[2] * B[1]
	mul	x25, x10, x22
	umulh	x26, x10, x22
	adds	x7, x7, x25
	adcs	x16, x16, x26
	adc	x17, x17, xzr
	#  A[3] * B[0]
	mul	x25, x11, x21
	umulh	x26, x11, x21
	adds	x7, x7, x25
	adcs	x16, x16, x26
	adc	x17, x17, xzr
	#  A[1] * B[3]
	mul	x25, x9, x24
	umulh	x26, x9, x24
	adds	x16, x16, x25
	adcs	x17, x17, x26
	adc	x19, xzr, xzr
	#  A[2] * B[2]
	mul	x25, x10, x23
	umulh	x26, x10, x23
	adds	x16, x16, x25
	adcs	x17, x17, x26
	adc	x19, x19, xzr
	#  A[3] * B[1]
	mul	x25, x11, x22
	umulh	x26, x11, x22
	adds	x16, x16, x25
	adcs	x17, x17, x26
	adc	x19, x19, xzr
	#  A[2] * B[3]
	mul	x25, x10, x24
	umulh	x26, x10, x24
	adds	x17, x17, x25
	adcs	x19, x19, x26
	adc	x20, xzr, xzr
	#  A[3] * B[2]
	mul	x25, x11, x23
	umulh	x26, x11, x23
	adds	x17, x17, x25
	adcs	x19, x19, x26
	adc	x20, x20, xzr
	#  A[3] * B[3]
	mul	x25, x11, x24
	umulh	x26, x11, x24
	adds	x19, x19, x25
	adc	x20, x20, x26
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x20, x20, x19, #63
	extr	x19, x19, x17, #63
	extr	x17, x17, x16, #63
	extr	x16, x16, x7, #63
	and	x7, x7, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x25, #19
	mul	x26, x25, x16
	umulh	x16, x25, x16
	adds	x4, x4, x26
	mul	x26, x25, x17
	umulh	x17, x25, x17
	adcs	x5, x5, x26
	mul	x26, x25, x19
	umulh	x19, x25, x19
	adcs	x6, x6, x26
	mul	x26, x25, x20
	umulh	x27, x25, x20
	adcs	x7, x7, x26
	adc	x27, x27, xzr
	#  Add remaining product results in
	adds	x5, x5, x16
	adcs	x6, x6, x17
	adcs	x7, x7, x19
	adc	x27, x27, xzr
	#  Overflow
	extr	x27, x27, x7, #63
	mul	x27, x27, x25
	and	x7, x7, #0x7fffffffffffffff
	adds	x4, x4, x27
	adcs	x5, x5, xzr
	adcs	x6, x6, xzr
	adc	x7, x7, xzr
	# Reduce if top bit set
	and	x27, x25, x7, asr 63
	and	x7, x7, #0x7fffffffffffffff
	adds	x4, x4, x27
	adcs	x5, x5, xzr
	adcs	x6, x6, xzr
	adc	x7, x7, xzr
	# Store
	ldr	x0, [x29, #24]
	ldr	x1, [x29, #16]
	# Add
	adds	x8, x12, x4
	adcs	x9, x13, x5
	adcs	x10, x14, x6
	adc	x11, x15, x7
	mov	x25, #-19
	asr	x28, x11, #63
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Sub modulus (if overflow)
	subs	x8, x8, x25
	sbcs	x9, x9, x28
	sbcs	x10, x10, x28
	sbc	x11, x11, x26
	# Sub
	subs	x16, x12, x4
	sbcs	x17, x13, x5
	sbcs	x19, x14, x6
	sbcs	x20, x15, x7
	mov	x25, #-19
	csetm	x28, cc
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Add modulus (if underflow)
	adds	x16, x16, x25
	adcs	x17, x17, x28
	adcs	x19, x19, x28
	adc	x20, x20, x26
	stp	x8, x9, [x0]
	stp	x10, x11, [x0, #16]
	stp	x16, x17, [x1]
	stp	x19, x20, [x1, #16]
	ldr	x0, [x29, #40]
	ldr	x1, [x29, #176]
	ldr	x3, [x29, #72]
	# Multiply
	ldp	x16, x17, [x1]
	ldp	x19, x20, [x1, #16]
	ldp	x21, x22, [x3]
	ldp	x23, x24, [x3, #16]
	#  A[0] * B[0]
	mul	x4, x16, x21
	umulh	x5, x16, x21
	#  A[0] * B[1]
	mul	x25, x16, x22
	umulh	x6, x16, x22
	adds	x5, x5, x25
	adc	x6, x6, xzr
	#  A[1] * B[0]
	mul	x25, x17, x21
	umulh	x26, x17, x21
	adds	x5, x5, x25
	adcs	x6, x6, x26
	adc	x7, xzr, xzr
	#  A[0] * B[2]
	mul	x25, x16, x23
	umulh	x26, x16, x23
	adds	x6, x6, x25
	adc	x7, x7, x26
	#  A[1] * B[1]
	mul	x25, x17, x22
	umulh	x26, x17, x22
	adds	x6, x6, x25
	adcs	x7, x7, x26
	adc	x8, xzr, xzr
	#  A[2] * B[0]
	mul	x25, x19, x21
	umulh	x26, x19, x21
	adds	x6, x6, x25
	adcs	x7, x7, x26
	adc	x8, x8, xzr
	#  A[0] * B[3]
	mul	x25, x16, x24
	umulh	x26, x16, x24
	adds	x7, x7, x25
	adcs	x8, x8, x26
	adc	x9, xzr, xzr
	#  A[1] * B[2]
	mul	x25, x17, x23
	umulh	x26, x17, x23
	adds	x7, x7, x25
	adcs	x8, x8, x26
	adc	x9, x9, xzr
	#  A[2] * B[1]
	mul	x25, x19, x22
	umulh	x26, x19, x22
	adds	x7, x7, x25
	adcs	x8, x8, x26
	adc	x9, x9, xzr
	#  A[3] * B[0]
	mul	x25, x20, x21
	umulh	x26, x20, x21
	adds	x7, x7, x25
	adcs	x8, x8, x26
	adc	x9, x9, xzr
	#  A[1] * B[3]
	mul	x25, x17, x24
	umulh	x26, x17, x24
	adds	x8, x8, x25
	adcs	x9, x9, x26
	adc	x10, xzr, xzr
	#  A[2] * B[2]
	mul	x25, x19, x23
	umulh	x26, x19, x23
	adds	x8, x8, x25
	adcs	x9, x9, x26
	adc	x10, x10, xzr
	#  A[3] * B[1]
	mul	x25, x20, x22
	umulh	x26, x20, x22
	adds	x8, x8, x25
	adcs	x9, x9, x26
	adc	x10, x10, xzr
	#  A[2] * B[3]
	mul	x25, x19, x24
	umulh	x26, x19, x24
	adds	x9, x9, x25
	adcs	x10, x10, x26
	adc	x11, xzr, xzr
	#  A[3] * B[2]
	mul	x25, x20, x23
	umulh	x26, x20, x23
	adds	x9, x9, x25
	adcs	x10, x10, x26
	adc	x11, x11, xzr
	#  A[3] * B[3]
	mul	x25, x20, x24
	umulh	x26, x20, x24
	adds	x10, x10, x25
	adc	x11, x11, x26
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x11, x11, x10, #63
	extr	x10, x10, x9, #63
	extr	x9, x9, x8, #63
	extr	x8, x8, x7, #63
	and	x7, x7, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x25, #19
	mul	x26, x25, x8
	umulh	x8, x25, x8
	adds	x4, x4, x26
	mul	x26, x25, x9
	umulh	x9, x25, x9
	adcs	x5, x5, x26
	mul	x26, x25, x10
	umulh	x10, x25, x10
	adcs	x6, x6, x26
	mul	x26, x25, x11
	umulh	x27, x25, x11
	adcs	x7, x7, x26
	adc	x27, x27, xzr
	#  Add remaining product results in
	adds	x5, x5, x8
	adcs	x6, x6, x9
	adcs	x7, x7, x10
	adc	x27, x27, xzr
	#  Overflow
	extr	x27, x27, x7, #63
	mul	x27, x27, x25
	and	x7, x7, #0x7fffffffffffffff
	adds	x4, x4, x27
	adcs	x5, x5, xzr
	adcs	x6, x6, xzr
	adc	x7, x7, xzr
	# Reduce if top bit set
	and	x27, x25, x7, asr 63
	and	x7, x7, #0x7fffffffffffffff
	adds	x4, x4, x27
	adcs	x5, x5, xzr
	adcs	x6, x6, xzr
	adc	x7, x7, xzr
	# Store
	ldr	x0, [x29, #32]
	ldr	x1, [x29, #64]
	# Double
	ldp	x8, x9, [x1]
	ldp	x10, x11, [x1, #16]
	adds	x8, x8, x8
	adcs	x9, x9, x9
	adcs	x10, x10, x10
	adc	x11, x11, x11
	mov	x25, #-19
	asr	x28, x11, #63
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Sub modulus (if overflow)
	subs	x8, x8, x25
	sbcs	x9, x9, x28
	sbcs	x10, x10, x28
	sbc	x11, x11, x26
	ldr	x1, [x29, #40]
	# Add
	adds	x12, x8, x4
	adcs	x13, x9, x5
	adcs	x14, x10, x6
	adc	x15, x11, x7
	mov	x25, #-19
	asr	x28, x15, #63
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Sub modulus (if overflow)
	subs	x12, x12, x25
	sbcs	x13, x13, x28
	sbcs	x14, x14, x28
	sbc	x15, x15, x26
	# Sub
	subs	x16, x8, x4
	sbcs	x17, x9, x5
	sbcs	x19, x10, x6
	sbcs	x20, x11, x7
	mov	x25, #-19
	csetm	x28, cc
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Add modulus (if underflow)
	adds	x16, x16, x25
	adcs	x17, x17, x28
	adcs	x19, x19, x28
	adc	x20, x20, x26
	stp	x12, x13, [x0]
	stp	x14, x15, [x0, #16]
	stp	x16, x17, [x1]
	stp	x19, x20, [x1, #16]
	ldr	x17, [x29, #88]
	ldr	x19, [x29, #96]
	ldp	x20, x21, [x29, #104]
	ldp	x22, x23, [x29, #120]
	ldp	x24, x25, [x29, #136]
	ldp	x26, x27, [x29, #152]
	ldr	x28, [x29, #168]
	ldp	x29, x30, [sp], #0xb0
	ret
#ifndef __APPLE__
	.size	fe_ge_madd,.-fe_ge_madd
#endif /* __APPLE__ */
#ifndef __APPLE__
.text
.globl	fe_ge_msub
.type	fe_ge_msub,@function
.align	2
fe_ge_msub:
#else
.section	__TEXT,__text
.globl	_fe_ge_msub
.p2align	2
_fe_ge_msub:
#endif /* __APPLE__ */
	stp	x29, x30, [sp, #-176]!
	add	x29, sp, #0
	str	x17, [x29, #88]
	str	x19, [x29, #96]
	stp	x20, x21, [x29, #104]
	stp	x22, x23, [x29, #120]
	stp	x24, x25, [x29, #136]
	stp	x26, x27, [x29, #152]
	str	x28, [x29, #168]
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	str	x2, [x29, #32]
	str	x3, [x29, #40]
	str	x4, [x29, #48]
	str	x5, [x29, #56]
	str	x6, [x29, #64]
	str	x7, [x29, #72]
	ldr	x2, [x29, #56]
	ldr	x3, [x29, #48]
	# Add
	ldp	x12, x13, [x2]
	ldp	x14, x15, [x2, #16]
	ldp	x16, x17, [x3]
	ldp	x19, x20, [x3, #16]
	adds	x4, x12, x16
	adcs	x5, x13, x17
	adcs	x6, x14, x19
	adc	x7, x15, x20
	mov	x25, #-19
	asr	x28, x7, #63
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Sub modulus (if overflow)
	subs	x4, x4, x25
	sbcs	x5, x5, x28
	sbcs	x6, x6, x28
	sbc	x7, x7, x26
	# Sub
	subs	x8, x12, x16
	sbcs	x9, x13, x17
	sbcs	x10, x14, x19
	sbcs	x11, x15, x20
	mov	x25, #-19
	csetm	x28, cc
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Add modulus (if underflow)
	adds	x8, x8, x25
	adcs	x9, x9, x28
	adcs	x10, x10, x28
	adc	x11, x11, x26
	ldr	x0, [x29, #32]
	ldr	x2, [x29, #192]
	# Multiply
	ldp	x21, x22, [x2]
	ldp	x23, x24, [x2, #16]
	#  A[0] * B[0]
	mul	x12, x4, x21
	umulh	x13, x4, x21
	#  A[0] * B[1]
	mul	x25, x4, x22
	umulh	x14, x4, x22
	adds	x13, x13, x25
	adc	x14, x14, xzr
	#  A[1] * B[0]
	mul	x25, x5, x21
	umulh	x26, x5, x21
	adds	x13, x13, x25
	adcs	x14, x14, x26
	adc	x15, xzr, xzr
	#  A[0] * B[2]
	mul	x25, x4, x23
	umulh	x26, x4, x23
	adds	x14, x14, x25
	adc	x15, x15, x26
	#  A[1] * B[1]
	mul	x25, x5, x22
	umulh	x26, x5, x22
	adds	x14, x14, x25
	adcs	x15, x15, x26
	adc	x16, xzr, xzr
	#  A[2] * B[0]
	mul	x25, x6, x21
	umulh	x26, x6, x21
	adds	x14, x14, x25
	adcs	x15, x15, x26
	adc	x16, x16, xzr
	#  A[0] * B[3]
	mul	x25, x4, x24
	umulh	x26, x4, x24
	adds	x15, x15, x25
	adcs	x16, x16, x26
	adc	x17, xzr, xzr
	#  A[1] * B[2]
	mul	x25, x5, x23
	umulh	x26, x5, x23
	adds	x15, x15, x25
	adcs	x16, x16, x26
	adc	x17, x17, xzr
	#  A[2] * B[1]
	mul	x25, x6, x22
	umulh	x26, x6, x22
	adds	x15, x15, x25
	adcs	x16, x16, x26
	adc	x17, x17, xzr
	#  A[3] * B[0]
	mul	x25, x7, x21
	umulh	x26, x7, x21
	adds	x15, x15, x25
	adcs	x16, x16, x26
	adc	x17, x17, xzr
	#  A[1] * B[3]
	mul	x25, x5, x24
	umulh	x26, x5, x24
	adds	x16, x16, x25
	adcs	x17, x17, x26
	adc	x19, xzr, xzr
	#  A[2] * B[2]
	mul	x25, x6, x23
	umulh	x26, x6, x23
	adds	x16, x16, x25
	adcs	x17, x17, x26
	adc	x19, x19, xzr
	#  A[3] * B[1]
	mul	x25, x7, x22
	umulh	x26, x7, x22
	adds	x16, x16, x25
	adcs	x17, x17, x26
	adc	x19, x19, xzr
	#  A[2] * B[3]
	mul	x25, x6, x24
	umulh	x26, x6, x24
	adds	x17, x17, x25
	adcs	x19, x19, x26
	adc	x20, xzr, xzr
	#  A[3] * B[2]
	mul	x25, x7, x23
	umulh	x26, x7, x23
	adds	x17, x17, x25
	adcs	x19, x19, x26
	adc	x20, x20, xzr
	#  A[3] * B[3]
	mul	x25, x7, x24
	umulh	x26, x7, x24
	adds	x19, x19, x25
	adc	x20, x20, x26
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x20, x20, x19, #63
	extr	x19, x19, x17, #63
	extr	x17, x17, x16, #63
	extr	x16, x16, x15, #63
	and	x15, x15, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x25, #19
	mul	x26, x25, x16
	umulh	x16, x25, x16
	adds	x12, x12, x26
	mul	x26, x25, x17
	umulh	x17, x25, x17
	adcs	x13, x13, x26
	mul	x26, x25, x19
	umulh	x19, x25, x19
	adcs	x14, x14, x26
	mul	x26, x25, x20
	umulh	x27, x25, x20
	adcs	x15, x15, x26
	adc	x27, x27, xzr
	#  Add remaining product results in
	adds	x13, x13, x16
	adcs	x14, x14, x17
	adcs	x15, x15, x19
	adc	x27, x27, xzr
	#  Overflow
	extr	x27, x27, x15, #63
	mul	x27, x27, x25
	and	x15, x15, #0x7fffffffffffffff
	adds	x12, x12, x27
	adcs	x13, x13, xzr
	adcs	x14, x14, xzr
	adc	x15, x15, xzr
	# Reduce if top bit set
	and	x27, x25, x15, asr 63
	and	x15, x15, #0x7fffffffffffffff
	adds	x12, x12, x27
	adcs	x13, x13, xzr
	adcs	x14, x14, xzr
	adc	x15, x15, xzr
	# Store
	ldr	x0, [x29, #24]
	ldr	x1, [x29, #184]
	# Multiply
	ldp	x21, x22, [x1]
	ldp	x23, x24, [x1, #16]
	#  A[0] * B[0]
	mul	x4, x8, x21
	umulh	x5, x8, x21
	#  A[0] * B[1]
	mul	x25, x8, x22
	umulh	x6, x8, x22
	adds	x5, x5, x25
	adc	x6, x6, xzr
	#  A[1] * B[0]
	mul	x25, x9, x21
	umulh	x26, x9, x21
	adds	x5, x5, x25
	adcs	x6, x6, x26
	adc	x7, xzr, xzr
	#  A[0] * B[2]
	mul	x25, x8, x23
	umulh	x26, x8, x23
	adds	x6, x6, x25
	adc	x7, x7, x26
	#  A[1] * B[1]
	mul	x25, x9, x22
	umulh	x26, x9, x22
	adds	x6, x6, x25
	adcs	x7, x7, x26
	adc	x16, xzr, xzr
	#  A[2] * B[0]
	mul	x25, x10, x21
	umulh	x26, x10, x21
	adds	x6, x6, x25
	adcs	x7, x7, x26
	adc	x16, x16, xzr
	#  A[0] * B[3]
	mul	x25, x8, x24
	umulh	x26, x8, x24
	adds	x7, x7, x25
	adcs	x16, x16, x26
	adc	x17, xzr, xzr
	#  A[1] * B[2]
	mul	x25, x9, x23
	umulh	x26, x9, x23
	adds	x7, x7, x25
	adcs	x16, x16, x26
	adc	x17, x17, xzr
	#  A[2] * B[1]
	mul	x25, x10, x22
	umulh	x26, x10, x22
	adds	x7, x7, x25
	adcs	x16, x16, x26
	adc	x17, x17, xzr
	#  A[3] * B[0]
	mul	x25, x11, x21
	umulh	x26, x11, x21
	adds	x7, x7, x25
	adcs	x16, x16, x26
	adc	x17, x17, xzr
	#  A[1] * B[3]
	mul	x25, x9, x24
	umulh	x26, x9, x24
	adds	x16, x16, x25
	adcs	x17, x17, x26
	adc	x19, xzr, xzr
	#  A[2] * B[2]
	mul	x25, x10, x23
	umulh	x26, x10, x23
	adds	x16, x16, x25
	adcs	x17, x17, x26
	adc	x19, x19, xzr
	#  A[3] * B[1]
	mul	x25, x11, x22
	umulh	x26, x11, x22
	adds	x16, x16, x25
	adcs	x17, x17, x26
	adc	x19, x19, xzr
	#  A[2] * B[3]
	mul	x25, x10, x24
	umulh	x26, x10, x24
	adds	x17, x17, x25
	adcs	x19, x19, x26
	adc	x20, xzr, xzr
	#  A[3] * B[2]
	mul	x25, x11, x23
	umulh	x26, x11, x23
	adds	x17, x17, x25
	adcs	x19, x19, x26
	adc	x20, x20, xzr
	#  A[3] * B[3]
	mul	x25, x11, x24
	umulh	x26, x11, x24
	adds	x19, x19, x25
	adc	x20, x20, x26
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x20, x20, x19, #63
	extr	x19, x19, x17, #63
	extr	x17, x17, x16, #63
	extr	x16, x16, x7, #63
	and	x7, x7, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x25, #19
	mul	x26, x25, x16
	umulh	x16, x25, x16
	adds	x4, x4, x26
	mul	x26, x25, x17
	umulh	x17, x25, x17
	adcs	x5, x5, x26
	mul	x26, x25, x19
	umulh	x19, x25, x19
	adcs	x6, x6, x26
	mul	x26, x25, x20
	umulh	x27, x25, x20
	adcs	x7, x7, x26
	adc	x27, x27, xzr
	#  Add remaining product results in
	adds	x5, x5, x16
	adcs	x6, x6, x17
	adcs	x7, x7, x19
	adc	x27, x27, xzr
	#  Overflow
	extr	x27, x27, x7, #63
	mul	x27, x27, x25
	and	x7, x7, #0x7fffffffffffffff
	adds	x4, x4, x27
	adcs	x5, x5, xzr
	adcs	x6, x6, xzr
	adc	x7, x7, xzr
	# Reduce if top bit set
	and	x27, x25, x7, asr 63
	and	x7, x7, #0x7fffffffffffffff
	adds	x4, x4, x27
	adcs	x5, x5, xzr
	adcs	x6, x6, xzr
	adc	x7, x7, xzr
	# Store
	ldr	x0, [x29, #24]
	ldr	x1, [x29, #16]
	# Add
	adds	x8, x12, x4
	adcs	x9, x13, x5
	adcs	x10, x14, x6
	adc	x11, x15, x7
	mov	x25, #-19
	asr	x28, x11, #63
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Sub modulus (if overflow)
	subs	x8, x8, x25
	sbcs	x9, x9, x28
	sbcs	x10, x10, x28
	sbc	x11, x11, x26
	# Sub
	subs	x16, x12, x4
	sbcs	x17, x13, x5
	sbcs	x19, x14, x6
	sbcs	x20, x15, x7
	mov	x25, #-19
	csetm	x28, cc
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Add modulus (if underflow)
	adds	x16, x16, x25
	adcs	x17, x17, x28
	adcs	x19, x19, x28
	adc	x20, x20, x26
	stp	x8, x9, [x0]
	stp	x10, x11, [x0, #16]
	stp	x16, x17, [x1]
	stp	x19, x20, [x1, #16]
	ldr	x0, [x29, #40]
	ldr	x1, [x29, #176]
	ldr	x3, [x29, #72]
	# Multiply
	ldp	x16, x17, [x1]
	ldp	x19, x20, [x1, #16]
	ldp	x21, x22, [x3]
	ldp	x23, x24, [x3, #16]
	#  A[0] * B[0]
	mul	x4, x16, x21
	umulh	x5, x16, x21
	#  A[0] * B[1]
	mul	x25, x16, x22
	umulh	x6, x16, x22
	adds	x5, x5, x25
	adc	x6, x6, xzr
	#  A[1] * B[0]
	mul	x25, x17, x21
	umulh	x26, x17, x21
	adds	x5, x5, x25
	adcs	x6, x6, x26
	adc	x7, xzr, xzr
	#  A[0] * B[2]
	mul	x25, x16, x23
	umulh	x26, x16, x23
	adds	x6, x6, x25
	adc	x7, x7, x26
	#  A[1] * B[1]
	mul	x25, x17, x22
	umulh	x26, x17, x22
	adds	x6, x6, x25
	adcs	x7, x7, x26
	adc	x8, xzr, xzr
	#  A[2] * B[0]
	mul	x25, x19, x21
	umulh	x26, x19, x21
	adds	x6, x6, x25
	adcs	x7, x7, x26
	adc	x8, x8, xzr
	#  A[0] * B[3]
	mul	x25, x16, x24
	umulh	x26, x16, x24
	adds	x7, x7, x25
	adcs	x8, x8, x26
	adc	x9, xzr, xzr
	#  A[1] * B[2]
	mul	x25, x17, x23
	umulh	x26, x17, x23
	adds	x7, x7, x25
	adcs	x8, x8, x26
	adc	x9, x9, xzr
	#  A[2] * B[1]
	mul	x25, x19, x22
	umulh	x26, x19, x22
	adds	x7, x7, x25
	adcs	x8, x8, x26
	adc	x9, x9, xzr
	#  A[3] * B[0]
	mul	x25, x20, x21
	umulh	x26, x20, x21
	adds	x7, x7, x25
	adcs	x8, x8, x26
	adc	x9, x9, xzr
	#  A[1] * B[3]
	mul	x25, x17, x24
	umulh	x26, x17, x24
	adds	x8, x8, x25
	adcs	x9, x9, x26
	adc	x10, xzr, xzr
	#  A[2] * B[2]
	mul	x25, x19, x23
	umulh	x26, x19, x23
	adds	x8, x8, x25
	adcs	x9, x9, x26
	adc	x10, x10, xzr
	#  A[3] * B[1]
	mul	x25, x20, x22
	umulh	x26, x20, x22
	adds	x8, x8, x25
	adcs	x9, x9, x26
	adc	x10, x10, xzr
	#  A[2] * B[3]
	mul	x25, x19, x24
	umulh	x26, x19, x24
	adds	x9, x9, x25
	adcs	x10, x10, x26
	adc	x11, xzr, xzr
	#  A[3] * B[2]
	mul	x25, x20, x23
	umulh	x26, x20, x23
	adds	x9, x9, x25
	adcs	x10, x10, x26
	adc	x11, x11, xzr
	#  A[3] * B[3]
	mul	x25, x20, x24
	umulh	x26, x20, x24
	adds	x10, x10, x25
	adc	x11, x11, x26
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x11, x11, x10, #63
	extr	x10, x10, x9, #63
	extr	x9, x9, x8, #63
	extr	x8, x8, x7, #63
	and	x7, x7, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x25, #19
	mul	x26, x25, x8
	umulh	x8, x25, x8
	adds	x4, x4, x26
	mul	x26, x25, x9
	umulh	x9, x25, x9
	adcs	x5, x5, x26
	mul	x26, x25, x10
	umulh	x10, x25, x10
	adcs	x6, x6, x26
	mul	x26, x25, x11
	umulh	x27, x25, x11
	adcs	x7, x7, x26
	adc	x27, x27, xzr
	#  Add remaining product results in
	adds	x5, x5, x8
	adcs	x6, x6, x9
	adcs	x7, x7, x10
	adc	x27, x27, xzr
	#  Overflow
	extr	x27, x27, x7, #63
	mul	x27, x27, x25
	and	x7, x7, #0x7fffffffffffffff
	adds	x4, x4, x27
	adcs	x5, x5, xzr
	adcs	x6, x6, xzr
	adc	x7, x7, xzr
	# Reduce if top bit set
	and	x27, x25, x7, asr 63
	and	x7, x7, #0x7fffffffffffffff
	adds	x4, x4, x27
	adcs	x5, x5, xzr
	adcs	x6, x6, xzr
	adc	x7, x7, xzr
	# Store
	ldr	x0, [x29, #32]
	ldr	x1, [x29, #64]
	# Double
	ldp	x8, x9, [x1]
	ldp	x10, x11, [x1, #16]
	adds	x8, x8, x8
	adcs	x9, x9, x9
	adcs	x10, x10, x10
	adc	x11, x11, x11
	mov	x25, #-19
	asr	x28, x11, #63
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Sub modulus (if overflow)
	subs	x8, x8, x25
	sbcs	x9, x9, x28
	sbcs	x10, x10, x28
	sbc	x11, x11, x26
	ldr	x1, [x29, #40]
	# Add
	adds	x12, x8, x4
	adcs	x13, x9, x5
	adcs	x14, x10, x6
	adc	x15, x11, x7
	mov	x25, #-19
	asr	x28, x15, #63
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Sub modulus (if overflow)
	subs	x12, x12, x25
	sbcs	x13, x13, x28
	sbcs	x14, x14, x28
	sbc	x15, x15, x26
	# Sub
	subs	x16, x8, x4
	sbcs	x17, x9, x5
	sbcs	x19, x10, x6
	sbcs	x20, x11, x7
	mov	x25, #-19
	csetm	x28, cc
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Add modulus (if underflow)
	adds	x16, x16, x25
	adcs	x17, x17, x28
	adcs	x19, x19, x28
	adc	x20, x20, x26
	stp	x12, x13, [x1]
	stp	x14, x15, [x1, #16]
	stp	x16, x17, [x0]
	stp	x19, x20, [x0, #16]
	ldr	x17, [x29, #88]
	ldr	x19, [x29, #96]
	ldp	x20, x21, [x29, #104]
	ldp	x22, x23, [x29, #120]
	ldp	x24, x25, [x29, #136]
	ldp	x26, x27, [x29, #152]
	ldr	x28, [x29, #168]
	ldp	x29, x30, [sp], #0xb0
	ret
#ifndef __APPLE__
	.size	fe_ge_msub,.-fe_ge_msub
#endif /* __APPLE__ */
#ifndef __APPLE__
.text
.globl	fe_ge_add
.type	fe_ge_add,@function
.align	2
fe_ge_add:
#else
.section	__TEXT,__text
.globl	_fe_ge_add
.p2align	2
_fe_ge_add:
#endif /* __APPLE__ */
	stp	x29, x30, [sp, #-176]!
	add	x29, sp, #0
	str	x17, [x29, #88]
	str	x19, [x29, #96]
	stp	x20, x21, [x29, #104]
	stp	x22, x23, [x29, #120]
	stp	x24, x25, [x29, #136]
	stp	x26, x27, [x29, #152]
	str	x28, [x29, #168]
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	str	x2, [x29, #32]
	str	x3, [x29, #40]
	str	x4, [x29, #48]
	str	x5, [x29, #56]
	str	x6, [x29, #64]
	str	x7, [x29, #72]
	ldr	x2, [x29, #56]
	ldr	x3, [x29, #48]
	# Add
	ldp	x12, x13, [x2]
	ldp	x14, x15, [x2, #16]
	ldp	x16, x17, [x3]
	ldp	x19, x20, [x3, #16]
	adds	x4, x12, x16
	adcs	x5, x13, x17
	adcs	x6, x14, x19
	adc	x7, x15, x20
	mov	x25, #-19
	asr	x28, x7, #63
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Sub modulus (if overflow)
	subs	x4, x4, x25
	sbcs	x5, x5, x28
	sbcs	x6, x6, x28
	sbc	x7, x7, x26
	# Sub
	subs	x8, x12, x16
	sbcs	x9, x13, x17
	sbcs	x10, x14, x19
	sbcs	x11, x15, x20
	mov	x25, #-19
	csetm	x28, cc
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Add modulus (if underflow)
	adds	x8, x8, x25
	adcs	x9, x9, x28
	adcs	x10, x10, x28
	adc	x11, x11, x26
	ldr	x0, [x29, #32]
	ldr	x2, [x29, #192]
	# Multiply
	ldp	x21, x22, [x2]
	ldp	x23, x24, [x2, #16]
	#  A[0] * B[0]
	mul	x12, x4, x21
	umulh	x13, x4, x21
	#  A[0] * B[1]
	mul	x25, x4, x22
	umulh	x14, x4, x22
	adds	x13, x13, x25
	adc	x14, x14, xzr
	#  A[1] * B[0]
	mul	x25, x5, x21
	umulh	x26, x5, x21
	adds	x13, x13, x25
	adcs	x14, x14, x26
	adc	x15, xzr, xzr
	#  A[0] * B[2]
	mul	x25, x4, x23
	umulh	x26, x4, x23
	adds	x14, x14, x25
	adc	x15, x15, x26
	#  A[1] * B[1]
	mul	x25, x5, x22
	umulh	x26, x5, x22
	adds	x14, x14, x25
	adcs	x15, x15, x26
	adc	x16, xzr, xzr
	#  A[2] * B[0]
	mul	x25, x6, x21
	umulh	x26, x6, x21
	adds	x14, x14, x25
	adcs	x15, x15, x26
	adc	x16, x16, xzr
	#  A[0] * B[3]
	mul	x25, x4, x24
	umulh	x26, x4, x24
	adds	x15, x15, x25
	adcs	x16, x16, x26
	adc	x17, xzr, xzr
	#  A[1] * B[2]
	mul	x25, x5, x23
	umulh	x26, x5, x23
	adds	x15, x15, x25
	adcs	x16, x16, x26
	adc	x17, x17, xzr
	#  A[2] * B[1]
	mul	x25, x6, x22
	umulh	x26, x6, x22
	adds	x15, x15, x25
	adcs	x16, x16, x26
	adc	x17, x17, xzr
	#  A[3] * B[0]
	mul	x25, x7, x21
	umulh	x26, x7, x21
	adds	x15, x15, x25
	adcs	x16, x16, x26
	adc	x17, x17, xzr
	#  A[1] * B[3]
	mul	x25, x5, x24
	umulh	x26, x5, x24
	adds	x16, x16, x25
	adcs	x17, x17, x26
	adc	x19, xzr, xzr
	#  A[2] * B[2]
	mul	x25, x6, x23
	umulh	x26, x6, x23
	adds	x16, x16, x25
	adcs	x17, x17, x26
	adc	x19, x19, xzr
	#  A[3] * B[1]
	mul	x25, x7, x22
	umulh	x26, x7, x22
	adds	x16, x16, x25
	adcs	x17, x17, x26
	adc	x19, x19, xzr
	#  A[2] * B[3]
	mul	x25, x6, x24
	umulh	x26, x6, x24
	adds	x17, x17, x25
	adcs	x19, x19, x26
	adc	x20, xzr, xzr
	#  A[3] * B[2]
	mul	x25, x7, x23
	umulh	x26, x7, x23
	adds	x17, x17, x25
	adcs	x19, x19, x26
	adc	x20, x20, xzr
	#  A[3] * B[3]
	mul	x25, x7, x24
	umulh	x26, x7, x24
	adds	x19, x19, x25
	adc	x20, x20, x26
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x20, x20, x19, #63
	extr	x19, x19, x17, #63
	extr	x17, x17, x16, #63
	extr	x16, x16, x15, #63
	and	x15, x15, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x25, #19
	mul	x26, x25, x16
	umulh	x16, x25, x16
	adds	x12, x12, x26
	mul	x26, x25, x17
	umulh	x17, x25, x17
	adcs	x13, x13, x26
	mul	x26, x25, x19
	umulh	x19, x25, x19
	adcs	x14, x14, x26
	mul	x26, x25, x20
	umulh	x27, x25, x20
	adcs	x15, x15, x26
	adc	x27, x27, xzr
	#  Add remaining product results in
	adds	x13, x13, x16
	adcs	x14, x14, x17
	adcs	x15, x15, x19
	adc	x27, x27, xzr
	#  Overflow
	extr	x27, x27, x15, #63
	mul	x27, x27, x25
	and	x15, x15, #0x7fffffffffffffff
	adds	x12, x12, x27
	adcs	x13, x13, xzr
	adcs	x14, x14, xzr
	adc	x15, x15, xzr
	# Reduce if top bit set
	and	x27, x25, x15, asr 63
	and	x15, x15, #0x7fffffffffffffff
	adds	x12, x12, x27
	adcs	x13, x13, xzr
	adcs	x14, x14, xzr
	adc	x15, x15, xzr
	# Store
	ldr	x0, [x29, #24]
	ldr	x1, [x29, #200]
	# Multiply
	ldp	x21, x22, [x1]
	ldp	x23, x24, [x1, #16]
	#  A[0] * B[0]
	mul	x4, x8, x21
	umulh	x5, x8, x21
	#  A[0] * B[1]
	mul	x25, x8, x22
	umulh	x6, x8, x22
	adds	x5, x5, x25
	adc	x6, x6, xzr
	#  A[1] * B[0]
	mul	x25, x9, x21
	umulh	x26, x9, x21
	adds	x5, x5, x25
	adcs	x6, x6, x26
	adc	x7, xzr, xzr
	#  A[0] * B[2]
	mul	x25, x8, x23
	umulh	x26, x8, x23
	adds	x6, x6, x25
	adc	x7, x7, x26
	#  A[1] * B[1]
	mul	x25, x9, x22
	umulh	x26, x9, x22
	adds	x6, x6, x25
	adcs	x7, x7, x26
	adc	x16, xzr, xzr
	#  A[2] * B[0]
	mul	x25, x10, x21
	umulh	x26, x10, x21
	adds	x6, x6, x25
	adcs	x7, x7, x26
	adc	x16, x16, xzr
	#  A[0] * B[3]
	mul	x25, x8, x24
	umulh	x26, x8, x24
	adds	x7, x7, x25
	adcs	x16, x16, x26
	adc	x17, xzr, xzr
	#  A[1] * B[2]
	mul	x25, x9, x23
	umulh	x26, x9, x23
	adds	x7, x7, x25
	adcs	x16, x16, x26
	adc	x17, x17, xzr
	#  A[2] * B[1]
	mul	x25, x10, x22
	umulh	x26, x10, x22
	adds	x7, x7, x25
	adcs	x16, x16, x26
	adc	x17, x17, xzr
	#  A[3] * B[0]
	mul	x25, x11, x21
	umulh	x26, x11, x21
	adds	x7, x7, x25
	adcs	x16, x16, x26
	adc	x17, x17, xzr
	#  A[1] * B[3]
	mul	x25, x9, x24
	umulh	x26, x9, x24
	adds	x16, x16, x25
	adcs	x17, x17, x26
	adc	x19, xzr, xzr
	#  A[2] * B[2]
	mul	x25, x10, x23
	umulh	x26, x10, x23
	adds	x16, x16, x25
	adcs	x17, x17, x26
	adc	x19, x19, xzr
	#  A[3] * B[1]
	mul	x25, x11, x22
	umulh	x26, x11, x22
	adds	x16, x16, x25
	adcs	x17, x17, x26
	adc	x19, x19, xzr
	#  A[2] * B[3]
	mul	x25, x10, x24
	umulh	x26, x10, x24
	adds	x17, x17, x25
	adcs	x19, x19, x26
	adc	x20, xzr, xzr
	#  A[3] * B[2]
	mul	x25, x11, x23
	umulh	x26, x11, x23
	adds	x17, x17, x25
	adcs	x19, x19, x26
	adc	x20, x20, xzr
	#  A[3] * B[3]
	mul	x25, x11, x24
	umulh	x26, x11, x24
	adds	x19, x19, x25
	adc	x20, x20, x26
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x20, x20, x19, #63
	extr	x19, x19, x17, #63
	extr	x17, x17, x16, #63
	extr	x16, x16, x7, #63
	and	x7, x7, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x25, #19
	mul	x26, x25, x16
	umulh	x16, x25, x16
	adds	x4, x4, x26
	mul	x26, x25, x17
	umulh	x17, x25, x17
	adcs	x5, x5, x26
	mul	x26, x25, x19
	umulh	x19, x25, x19
	adcs	x6, x6, x26
	mul	x26, x25, x20
	umulh	x27, x25, x20
	adcs	x7, x7, x26
	adc	x27, x27, xzr
	#  Add remaining product results in
	adds	x5, x5, x16
	adcs	x6, x6, x17
	adcs	x7, x7, x19
	adc	x27, x27, xzr
	#  Overflow
	extr	x27, x27, x7, #63
	mul	x27, x27, x25
	and	x7, x7, #0x7fffffffffffffff
	adds	x4, x4, x27
	adcs	x5, x5, xzr
	adcs	x6, x6, xzr
	adc	x7, x7, xzr
	# Reduce if top bit set
	and	x27, x25, x7, asr 63
	and	x7, x7, #0x7fffffffffffffff
	adds	x4, x4, x27
	adcs	x5, x5, xzr
	adcs	x6, x6, xzr
	adc	x7, x7, xzr
	# Store
	ldr	x0, [x29, #24]
	ldr	x1, [x29, #16]
	# Add
	adds	x8, x12, x4
	adcs	x9, x13, x5
	adcs	x10, x14, x6
	adc	x11, x15, x7
	mov	x25, #-19
	asr	x28, x11, #63
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Sub modulus (if overflow)
	subs	x8, x8, x25
	sbcs	x9, x9, x28
	sbcs	x10, x10, x28
	sbc	x11, x11, x26
	# Sub
	subs	x16, x12, x4
	sbcs	x17, x13, x5
	sbcs	x19, x14, x6
	sbcs	x20, x15, x7
	mov	x25, #-19
	csetm	x28, cc
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Add modulus (if underflow)
	adds	x16, x16, x25
	adcs	x17, x17, x28
	adcs	x19, x19, x28
	adc	x20, x20, x26
	stp	x8, x9, [x0]
	stp	x10, x11, [x0, #16]
	stp	x16, x17, [x1]
	stp	x19, x20, [x1, #16]
	ldr	x0, [x29, #48]
	ldr	x1, [x29, #64]
	ldr	x2, [x29, #176]
	# Multiply
	ldp	x12, x13, [x1]
	ldp	x14, x15, [x1, #16]
	ldp	x16, x17, [x2]
	ldp	x19, x20, [x2, #16]
	#  A[0] * B[0]
	mul	x4, x12, x16
	umulh	x5, x12, x16
	#  A[0] * B[1]
	mul	x25, x12, x17
	umulh	x6, x12, x17
	adds	x5, x5, x25
	adc	x6, x6, xzr
	#  A[1] * B[0]
	mul	x25, x13, x16
	umulh	x26, x13, x16
	adds	x5, x5, x25
	adcs	x6, x6, x26
	adc	x7, xzr, xzr
	#  A[0] * B[2]
	mul	x25, x12, x19
	umulh	x26, x12, x19
	adds	x6, x6, x25
	adc	x7, x7, x26
	#  A[1] * B[1]
	mul	x25, x13, x17
	umulh	x26, x13, x17
	adds	x6, x6, x25
	adcs	x7, x7, x26
	adc	x8, xzr, xzr
	#  A[2] * B[0]
	mul	x25, x14, x16
	umulh	x26, x14, x16
	adds	x6, x6, x25
	adcs	x7, x7, x26
	adc	x8, x8, xzr
	#  A[0] * B[3]
	mul	x25, x12, x20
	umulh	x26, x12, x20
	adds	x7, x7, x25
	adcs	x8, x8, x26
	adc	x9, xzr, xzr
	#  A[1] * B[2]
	mul	x25, x13, x19
	umulh	x26, x13, x19
	adds	x7, x7, x25
	adcs	x8, x8, x26
	adc	x9, x9, xzr
	#  A[2] * B[1]
	mul	x25, x14, x17
	umulh	x26, x14, x17
	adds	x7, x7, x25
	adcs	x8, x8, x26
	adc	x9, x9, xzr
	#  A[3] * B[0]
	mul	x25, x15, x16
	umulh	x26, x15, x16
	adds	x7, x7, x25
	adcs	x8, x8, x26
	adc	x9, x9, xzr
	#  A[1] * B[3]
	mul	x25, x13, x20
	umulh	x26, x13, x20
	adds	x8, x8, x25
	adcs	x9, x9, x26
	adc	x10, xzr, xzr
	#  A[2] * B[2]
	mul	x25, x14, x19
	umulh	x26, x14, x19
	adds	x8, x8, x25
	adcs	x9, x9, x26
	adc	x10, x10, xzr
	#  A[3] * B[1]
	mul	x25, x15, x17
	umulh	x26, x15, x17
	adds	x8, x8, x25
	adcs	x9, x9, x26
	adc	x10, x10, xzr
	#  A[2] * B[3]
	mul	x25, x14, x20
	umulh	x26, x14, x20
	adds	x9, x9, x25
	adcs	x10, x10, x26
	adc	x11, xzr, xzr
	#  A[3] * B[2]
	mul	x25, x15, x19
	umulh	x26, x15, x19
	adds	x9, x9, x25
	adcs	x10, x10, x26
	adc	x11, x11, xzr
	#  A[3] * B[3]
	mul	x25, x15, x20
	umulh	x26, x15, x20
	adds	x10, x10, x25
	adc	x11, x11, x26
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x11, x11, x10, #63
	extr	x10, x10, x9, #63
	extr	x9, x9, x8, #63
	extr	x8, x8, x7, #63
	and	x7, x7, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x25, #19
	mul	x26, x25, x8
	umulh	x8, x25, x8
	adds	x4, x4, x26
	mul	x26, x25, x9
	umulh	x9, x25, x9
	adcs	x5, x5, x26
	mul	x26, x25, x10
	umulh	x10, x25, x10
	adcs	x6, x6, x26
	mul	x26, x25, x11
	umulh	x27, x25, x11
	adcs	x7, x7, x26
	adc	x27, x27, xzr
	#  Add remaining product results in
	adds	x5, x5, x8
	adcs	x6, x6, x9
	adcs	x7, x7, x10
	adc	x27, x27, xzr
	#  Overflow
	extr	x27, x27, x7, #63
	mul	x27, x27, x25
	and	x7, x7, #0x7fffffffffffffff
	adds	x4, x4, x27
	adcs	x5, x5, xzr
	adcs	x6, x6, xzr
	adc	x7, x7, xzr
	# Reduce if top bit set
	and	x27, x25, x7, asr 63
	and	x7, x7, #0x7fffffffffffffff
	adds	x4, x4, x27
	adcs	x5, x5, xzr
	adcs	x6, x6, xzr
	adc	x7, x7, xzr
	# Store
	ldr	x0, [x29, #48]
	# Double
	adds	x4, x4, x4
	adcs	x5, x5, x5
	adcs	x6, x6, x6
	adc	x7, x7, x7
	mov	x25, #-19
	asr	x28, x7, #63
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Sub modulus (if overflow)
	subs	x4, x4, x25
	sbcs	x5, x5, x28
	sbcs	x6, x6, x28
	sbc	x7, x7, x26
	ldr	x0, [x29, #40]
	ldr	x1, [x29, #184]
	ldr	x2, [x29, #72]
	# Multiply
	ldp	x16, x17, [x1]
	ldp	x19, x20, [x1, #16]
	ldp	x21, x22, [x2]
	ldp	x23, x24, [x2, #16]
	#  A[0] * B[0]
	mul	x8, x16, x21
	umulh	x9, x16, x21
	#  A[0] * B[1]
	mul	x25, x16, x22
	umulh	x10, x16, x22
	adds	x9, x9, x25
	adc	x10, x10, xzr
	#  A[1] * B[0]
	mul	x25, x17, x21
	umulh	x26, x17, x21
	adds	x9, x9, x25
	adcs	x10, x10, x26
	adc	x11, xzr, xzr
	#  A[0] * B[2]
	mul	x25, x16, x23
	umulh	x26, x16, x23
	adds	x10, x10, x25
	adc	x11, x11, x26
	#  A[1] * B[1]
	mul	x25, x17, x22
	umulh	x26, x17, x22
	adds	x10, x10, x25
	adcs	x11, x11, x26
	adc	x12, xzr, xzr
	#  A[2] * B[0]
	mul	x25, x19, x21
	umulh	x26, x19, x21
	adds	x10, x10, x25
	adcs	x11, x11, x26
	adc	x12, x12, xzr
	#  A[0] * B[3]
	mul	x25, x16, x24
	umulh	x26, x16, x24
	adds	x11, x11, x25
	adcs	x12, x12, x26
	adc	x13, xzr, xzr
	#  A[1] * B[2]
	mul	x25, x17, x23
	umulh	x26, x17, x23
	adds	x11, x11, x25
	adcs	x12, x12, x26
	adc	x13, x13, xzr
	#  A[2] * B[1]
	mul	x25, x19, x22
	umulh	x26, x19, x22
	adds	x11, x11, x25
	adcs	x12, x12, x26
	adc	x13, x13, xzr
	#  A[3] * B[0]
	mul	x25, x20, x21
	umulh	x26, x20, x21
	adds	x11, x11, x25
	adcs	x12, x12, x26
	adc	x13, x13, xzr
	#  A[1] * B[3]
	mul	x25, x17, x24
	umulh	x26, x17, x24
	adds	x12, x12, x25
	adcs	x13, x13, x26
	adc	x14, xzr, xzr
	#  A[2] * B[2]
	mul	x25, x19, x23
	umulh	x26, x19, x23
	adds	x12, x12, x25
	adcs	x13, x13, x26
	adc	x14, x14, xzr
	#  A[3] * B[1]
	mul	x25, x20, x22
	umulh	x26, x20, x22
	adds	x12, x12, x25
	adcs	x13, x13, x26
	adc	x14, x14, xzr
	#  A[2] * B[3]
	mul	x25, x19, x24
	umulh	x26, x19, x24
	adds	x13, x13, x25
	adcs	x14, x14, x26
	adc	x15, xzr, xzr
	#  A[3] * B[2]
	mul	x25, x20, x23
	umulh	x26, x20, x23
	adds	x13, x13, x25
	adcs	x14, x14, x26
	adc	x15, x15, xzr
	#  A[3] * B[3]
	mul	x25, x20, x24
	umulh	x26, x20, x24
	adds	x14, x14, x25
	adc	x15, x15, x26
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x15, x15, x14, #63
	extr	x14, x14, x13, #63
	extr	x13, x13, x12, #63
	extr	x12, x12, x11, #63
	and	x11, x11, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x25, #19
	mul	x26, x25, x12
	umulh	x12, x25, x12
	adds	x8, x8, x26
	mul	x26, x25, x13
	umulh	x13, x25, x13
	adcs	x9, x9, x26
	mul	x26, x25, x14
	umulh	x14, x25, x14
	adcs	x10, x10, x26
	mul	x26, x25, x15
	umulh	x27, x25, x15
	adcs	x11, x11, x26
	adc	x27, x27, xzr
	#  Add remaining product results in
	adds	x9, x9, x12
	adcs	x10, x10, x13
	adcs	x11, x11, x14
	adc	x27, x27, xzr
	#  Overflow
	extr	x27, x27, x11, #63
	mul	x27, x27, x25
	and	x11, x11, #0x7fffffffffffffff
	adds	x8, x8, x27
	adcs	x9, x9, xzr
	adcs	x10, x10, xzr
	adc	x11, x11, xzr
	# Reduce if top bit set
	and	x27, x25, x11, asr 63
	and	x11, x11, #0x7fffffffffffffff
	adds	x8, x8, x27
	adcs	x9, x9, xzr
	adcs	x10, x10, xzr
	adc	x11, x11, xzr
	# Store
	ldr	x0, [x29, #32]
	ldr	x1, [x29, #40]
	# Add
	adds	x12, x4, x8
	adcs	x13, x5, x9
	adcs	x14, x6, x10
	adc	x15, x7, x11
	mov	x25, #-19
	asr	x28, x15, #63
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Sub modulus (if overflow)
	subs	x12, x12, x25
	sbcs	x13, x13, x28
	sbcs	x14, x14, x28
	sbc	x15, x15, x26
	# Sub
	subs	x16, x4, x8
	sbcs	x17, x5, x9
	sbcs	x19, x6, x10
	sbcs	x20, x7, x11
	mov	x25, #-19
	csetm	x28, cc
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Add modulus (if underflow)
	adds	x16, x16, x25
	adcs	x17, x17, x28
	adcs	x19, x19, x28
	adc	x20, x20, x26
	stp	x12, x13, [x0]
	stp	x14, x15, [x0, #16]
	stp	x16, x17, [x1]
	stp	x19, x20, [x1, #16]
	ldr	x17, [x29, #88]
	ldr	x19, [x29, #96]
	ldp	x20, x21, [x29, #104]
	ldp	x22, x23, [x29, #120]
	ldp	x24, x25, [x29, #136]
	ldp	x26, x27, [x29, #152]
	ldr	x28, [x29, #168]
	ldp	x29, x30, [sp], #0xb0
	ret
#ifndef __APPLE__
	.size	fe_ge_add,.-fe_ge_add
#endif /* __APPLE__ */
#ifndef __APPLE__
.text
.globl	fe_ge_sub
.type	fe_ge_sub,@function
.align	2
fe_ge_sub:
#else
.section	__TEXT,__text
.globl	_fe_ge_sub
.p2align	2
_fe_ge_sub:
#endif /* __APPLE__ */
	stp	x29, x30, [sp, #-176]!
	add	x29, sp, #0
	str	x17, [x29, #88]
	str	x19, [x29, #96]
	stp	x20, x21, [x29, #104]
	stp	x22, x23, [x29, #120]
	stp	x24, x25, [x29, #136]
	stp	x26, x27, [x29, #152]
	str	x28, [x29, #168]
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	str	x2, [x29, #32]
	str	x3, [x29, #40]
	str	x4, [x29, #48]
	str	x5, [x29, #56]
	str	x6, [x29, #64]
	str	x7, [x29, #72]
	ldr	x2, [x29, #56]
	ldr	x3, [x29, #48]
	# Add
	ldp	x12, x13, [x2]
	ldp	x14, x15, [x2, #16]
	ldp	x16, x17, [x3]
	ldp	x19, x20, [x3, #16]
	adds	x4, x12, x16
	adcs	x5, x13, x17
	adcs	x6, x14, x19
	adc	x7, x15, x20
	mov	x25, #-19
	asr	x28, x7, #63
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Sub modulus (if overflow)
	subs	x4, x4, x25
	sbcs	x5, x5, x28
	sbcs	x6, x6, x28
	sbc	x7, x7, x26
	# Sub
	subs	x8, x12, x16
	sbcs	x9, x13, x17
	sbcs	x10, x14, x19
	sbcs	x11, x15, x20
	mov	x25, #-19
	csetm	x28, cc
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Add modulus (if underflow)
	adds	x8, x8, x25
	adcs	x9, x9, x28
	adcs	x10, x10, x28
	adc	x11, x11, x26
	ldr	x0, [x29, #32]
	ldr	x2, [x29, #200]
	# Multiply
	ldp	x21, x22, [x2]
	ldp	x23, x24, [x2, #16]
	#  A[0] * B[0]
	mul	x12, x4, x21
	umulh	x13, x4, x21
	#  A[0] * B[1]
	mul	x25, x4, x22
	umulh	x14, x4, x22
	adds	x13, x13, x25
	adc	x14, x14, xzr
	#  A[1] * B[0]
	mul	x25, x5, x21
	umulh	x26, x5, x21
	adds	x13, x13, x25
	adcs	x14, x14, x26
	adc	x15, xzr, xzr
	#  A[0] * B[2]
	mul	x25, x4, x23
	umulh	x26, x4, x23
	adds	x14, x14, x25
	adc	x15, x15, x26
	#  A[1] * B[1]
	mul	x25, x5, x22
	umulh	x26, x5, x22
	adds	x14, x14, x25
	adcs	x15, x15, x26
	adc	x16, xzr, xzr
	#  A[2] * B[0]
	mul	x25, x6, x21
	umulh	x26, x6, x21
	adds	x14, x14, x25
	adcs	x15, x15, x26
	adc	x16, x16, xzr
	#  A[0] * B[3]
	mul	x25, x4, x24
	umulh	x26, x4, x24
	adds	x15, x15, x25
	adcs	x16, x16, x26
	adc	x17, xzr, xzr
	#  A[1] * B[2]
	mul	x25, x5, x23
	umulh	x26, x5, x23
	adds	x15, x15, x25
	adcs	x16, x16, x26
	adc	x17, x17, xzr
	#  A[2] * B[1]
	mul	x25, x6, x22
	umulh	x26, x6, x22
	adds	x15, x15, x25
	adcs	x16, x16, x26
	adc	x17, x17, xzr
	#  A[3] * B[0]
	mul	x25, x7, x21
	umulh	x26, x7, x21
	adds	x15, x15, x25
	adcs	x16, x16, x26
	adc	x17, x17, xzr
	#  A[1] * B[3]
	mul	x25, x5, x24
	umulh	x26, x5, x24
	adds	x16, x16, x25
	adcs	x17, x17, x26
	adc	x19, xzr, xzr
	#  A[2] * B[2]
	mul	x25, x6, x23
	umulh	x26, x6, x23
	adds	x16, x16, x25
	adcs	x17, x17, x26
	adc	x19, x19, xzr
	#  A[3] * B[1]
	mul	x25, x7, x22
	umulh	x26, x7, x22
	adds	x16, x16, x25
	adcs	x17, x17, x26
	adc	x19, x19, xzr
	#  A[2] * B[3]
	mul	x25, x6, x24
	umulh	x26, x6, x24
	adds	x17, x17, x25
	adcs	x19, x19, x26
	adc	x20, xzr, xzr
	#  A[3] * B[2]
	mul	x25, x7, x23
	umulh	x26, x7, x23
	adds	x17, x17, x25
	adcs	x19, x19, x26
	adc	x20, x20, xzr
	#  A[3] * B[3]
	mul	x25, x7, x24
	umulh	x26, x7, x24
	adds	x19, x19, x25
	adc	x20, x20, x26
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x20, x20, x19, #63
	extr	x19, x19, x17, #63
	extr	x17, x17, x16, #63
	extr	x16, x16, x15, #63
	and	x15, x15, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x25, #19
	mul	x26, x25, x16
	umulh	x16, x25, x16
	adds	x12, x12, x26
	mul	x26, x25, x17
	umulh	x17, x25, x17
	adcs	x13, x13, x26
	mul	x26, x25, x19
	umulh	x19, x25, x19
	adcs	x14, x14, x26
	mul	x26, x25, x20
	umulh	x27, x25, x20
	adcs	x15, x15, x26
	adc	x27, x27, xzr
	#  Add remaining product results in
	adds	x13, x13, x16
	adcs	x14, x14, x17
	adcs	x15, x15, x19
	adc	x27, x27, xzr
	#  Overflow
	extr	x27, x27, x15, #63
	mul	x27, x27, x25
	and	x15, x15, #0x7fffffffffffffff
	adds	x12, x12, x27
	adcs	x13, x13, xzr
	adcs	x14, x14, xzr
	adc	x15, x15, xzr
	# Reduce if top bit set
	and	x27, x25, x15, asr 63
	and	x15, x15, #0x7fffffffffffffff
	adds	x12, x12, x27
	adcs	x13, x13, xzr
	adcs	x14, x14, xzr
	adc	x15, x15, xzr
	# Store
	ldr	x0, [x29, #24]
	ldr	x1, [x29, #192]
	# Multiply
	ldp	x21, x22, [x1]
	ldp	x23, x24, [x1, #16]
	#  A[0] * B[0]
	mul	x4, x8, x21
	umulh	x5, x8, x21
	#  A[0] * B[1]
	mul	x25, x8, x22
	umulh	x6, x8, x22
	adds	x5, x5, x25
	adc	x6, x6, xzr
	#  A[1] * B[0]
	mul	x25, x9, x21
	umulh	x26, x9, x21
	adds	x5, x5, x25
	adcs	x6, x6, x26
	adc	x7, xzr, xzr
	#  A[0] * B[2]
	mul	x25, x8, x23
	umulh	x26, x8, x23
	adds	x6, x6, x25
	adc	x7, x7, x26
	#  A[1] * B[1]
	mul	x25, x9, x22
	umulh	x26, x9, x22
	adds	x6, x6, x25
	adcs	x7, x7, x26
	adc	x16, xzr, xzr
	#  A[2] * B[0]
	mul	x25, x10, x21
	umulh	x26, x10, x21
	adds	x6, x6, x25
	adcs	x7, x7, x26
	adc	x16, x16, xzr
	#  A[0] * B[3]
	mul	x25, x8, x24
	umulh	x26, x8, x24
	adds	x7, x7, x25
	adcs	x16, x16, x26
	adc	x17, xzr, xzr
	#  A[1] * B[2]
	mul	x25, x9, x23
	umulh	x26, x9, x23
	adds	x7, x7, x25
	adcs	x16, x16, x26
	adc	x17, x17, xzr
	#  A[2] * B[1]
	mul	x25, x10, x22
	umulh	x26, x10, x22
	adds	x7, x7, x25
	adcs	x16, x16, x26
	adc	x17, x17, xzr
	#  A[3] * B[0]
	mul	x25, x11, x21
	umulh	x26, x11, x21
	adds	x7, x7, x25
	adcs	x16, x16, x26
	adc	x17, x17, xzr
	#  A[1] * B[3]
	mul	x25, x9, x24
	umulh	x26, x9, x24
	adds	x16, x16, x25
	adcs	x17, x17, x26
	adc	x19, xzr, xzr
	#  A[2] * B[2]
	mul	x25, x10, x23
	umulh	x26, x10, x23
	adds	x16, x16, x25
	adcs	x17, x17, x26
	adc	x19, x19, xzr
	#  A[3] * B[1]
	mul	x25, x11, x22
	umulh	x26, x11, x22
	adds	x16, x16, x25
	adcs	x17, x17, x26
	adc	x19, x19, xzr
	#  A[2] * B[3]
	mul	x25, x10, x24
	umulh	x26, x10, x24
	adds	x17, x17, x25
	adcs	x19, x19, x26
	adc	x20, xzr, xzr
	#  A[3] * B[2]
	mul	x25, x11, x23
	umulh	x26, x11, x23
	adds	x17, x17, x25
	adcs	x19, x19, x26
	adc	x20, x20, xzr
	#  A[3] * B[3]
	mul	x25, x11, x24
	umulh	x26, x11, x24
	adds	x19, x19, x25
	adc	x20, x20, x26
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x20, x20, x19, #63
	extr	x19, x19, x17, #63
	extr	x17, x17, x16, #63
	extr	x16, x16, x7, #63
	and	x7, x7, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x25, #19
	mul	x26, x25, x16
	umulh	x16, x25, x16
	adds	x4, x4, x26
	mul	x26, x25, x17
	umulh	x17, x25, x17
	adcs	x5, x5, x26
	mul	x26, x25, x19
	umulh	x19, x25, x19
	adcs	x6, x6, x26
	mul	x26, x25, x20
	umulh	x27, x25, x20
	adcs	x7, x7, x26
	adc	x27, x27, xzr
	#  Add remaining product results in
	adds	x5, x5, x16
	adcs	x6, x6, x17
	adcs	x7, x7, x19
	adc	x27, x27, xzr
	#  Overflow
	extr	x27, x27, x7, #63
	mul	x27, x27, x25
	and	x7, x7, #0x7fffffffffffffff
	adds	x4, x4, x27
	adcs	x5, x5, xzr
	adcs	x6, x6, xzr
	adc	x7, x7, xzr
	# Reduce if top bit set
	and	x27, x25, x7, asr 63
	and	x7, x7, #0x7fffffffffffffff
	adds	x4, x4, x27
	adcs	x5, x5, xzr
	adcs	x6, x6, xzr
	adc	x7, x7, xzr
	# Store
	ldr	x0, [x29, #24]
	ldr	x1, [x29, #16]
	# Add
	adds	x8, x12, x4
	adcs	x9, x13, x5
	adcs	x10, x14, x6
	adc	x11, x15, x7
	mov	x25, #-19
	asr	x28, x11, #63
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Sub modulus (if overflow)
	subs	x8, x8, x25
	sbcs	x9, x9, x28
	sbcs	x10, x10, x28
	sbc	x11, x11, x26
	# Sub
	subs	x16, x12, x4
	sbcs	x17, x13, x5
	sbcs	x19, x14, x6
	sbcs	x20, x15, x7
	mov	x25, #-19
	csetm	x28, cc
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Add modulus (if underflow)
	adds	x16, x16, x25
	adcs	x17, x17, x28
	adcs	x19, x19, x28
	adc	x20, x20, x26
	stp	x8, x9, [x0]
	stp	x10, x11, [x0, #16]
	stp	x16, x17, [x1]
	stp	x19, x20, [x1, #16]
	ldr	x0, [x29, #48]
	ldr	x1, [x29, #64]
	ldr	x2, [x29, #176]
	# Multiply
	ldp	x12, x13, [x1]
	ldp	x14, x15, [x1, #16]
	ldp	x16, x17, [x2]
	ldp	x19, x20, [x2, #16]
	#  A[0] * B[0]
	mul	x4, x12, x16
	umulh	x5, x12, x16
	#  A[0] * B[1]
	mul	x25, x12, x17
	umulh	x6, x12, x17
	adds	x5, x5, x25
	adc	x6, x6, xzr
	#  A[1] * B[0]
	mul	x25, x13, x16
	umulh	x26, x13, x16
	adds	x5, x5, x25
	adcs	x6, x6, x26
	adc	x7, xzr, xzr
	#  A[0] * B[2]
	mul	x25, x12, x19
	umulh	x26, x12, x19
	adds	x6, x6, x25
	adc	x7, x7, x26
	#  A[1] * B[1]
	mul	x25, x13, x17
	umulh	x26, x13, x17
	adds	x6, x6, x25
	adcs	x7, x7, x26
	adc	x8, xzr, xzr
	#  A[2] * B[0]
	mul	x25, x14, x16
	umulh	x26, x14, x16
	adds	x6, x6, x25
	adcs	x7, x7, x26
	adc	x8, x8, xzr
	#  A[0] * B[3]
	mul	x25, x12, x20
	umulh	x26, x12, x20
	adds	x7, x7, x25
	adcs	x8, x8, x26
	adc	x9, xzr, xzr
	#  A[1] * B[2]
	mul	x25, x13, x19
	umulh	x26, x13, x19
	adds	x7, x7, x25
	adcs	x8, x8, x26
	adc	x9, x9, xzr
	#  A[2] * B[1]
	mul	x25, x14, x17
	umulh	x26, x14, x17
	adds	x7, x7, x25
	adcs	x8, x8, x26
	adc	x9, x9, xzr
	#  A[3] * B[0]
	mul	x25, x15, x16
	umulh	x26, x15, x16
	adds	x7, x7, x25
	adcs	x8, x8, x26
	adc	x9, x9, xzr
	#  A[1] * B[3]
	mul	x25, x13, x20
	umulh	x26, x13, x20
	adds	x8, x8, x25
	adcs	x9, x9, x26
	adc	x10, xzr, xzr
	#  A[2] * B[2]
	mul	x25, x14, x19
	umulh	x26, x14, x19
	adds	x8, x8, x25
	adcs	x9, x9, x26
	adc	x10, x10, xzr
	#  A[3] * B[1]
	mul	x25, x15, x17
	umulh	x26, x15, x17
	adds	x8, x8, x25
	adcs	x9, x9, x26
	adc	x10, x10, xzr
	#  A[2] * B[3]
	mul	x25, x14, x20
	umulh	x26, x14, x20
	adds	x9, x9, x25
	adcs	x10, x10, x26
	adc	x11, xzr, xzr
	#  A[3] * B[2]
	mul	x25, x15, x19
	umulh	x26, x15, x19
	adds	x9, x9, x25
	adcs	x10, x10, x26
	adc	x11, x11, xzr
	#  A[3] * B[3]
	mul	x25, x15, x20
	umulh	x26, x15, x20
	adds	x10, x10, x25
	adc	x11, x11, x26
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x11, x11, x10, #63
	extr	x10, x10, x9, #63
	extr	x9, x9, x8, #63
	extr	x8, x8, x7, #63
	and	x7, x7, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x25, #19
	mul	x26, x25, x8
	umulh	x8, x25, x8
	adds	x4, x4, x26
	mul	x26, x25, x9
	umulh	x9, x25, x9
	adcs	x5, x5, x26
	mul	x26, x25, x10
	umulh	x10, x25, x10
	adcs	x6, x6, x26
	mul	x26, x25, x11
	umulh	x27, x25, x11
	adcs	x7, x7, x26
	adc	x27, x27, xzr
	#  Add remaining product results in
	adds	x5, x5, x8
	adcs	x6, x6, x9
	adcs	x7, x7, x10
	adc	x27, x27, xzr
	#  Overflow
	extr	x27, x27, x7, #63
	mul	x27, x27, x25
	and	x7, x7, #0x7fffffffffffffff
	adds	x4, x4, x27
	adcs	x5, x5, xzr
	adcs	x6, x6, xzr
	adc	x7, x7, xzr
	# Reduce if top bit set
	and	x27, x25, x7, asr 63
	and	x7, x7, #0x7fffffffffffffff
	adds	x4, x4, x27
	adcs	x5, x5, xzr
	adcs	x6, x6, xzr
	adc	x7, x7, xzr
	# Store
	ldr	x0, [x29, #48]
	# Double
	adds	x4, x4, x4
	adcs	x5, x5, x5
	adcs	x6, x6, x6
	adc	x7, x7, x7
	mov	x25, #-19
	asr	x28, x7, #63
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Sub modulus (if overflow)
	subs	x4, x4, x25
	sbcs	x5, x5, x28
	sbcs	x6, x6, x28
	sbc	x7, x7, x26
	ldr	x0, [x29, #40]
	ldr	x1, [x29, #184]
	ldr	x2, [x29, #72]
	# Multiply
	ldp	x16, x17, [x1]
	ldp	x19, x20, [x1, #16]
	ldp	x21, x22, [x2]
	ldp	x23, x24, [x2, #16]
	#  A[0] * B[0]
	mul	x8, x16, x21
	umulh	x9, x16, x21
	#  A[0] * B[1]
	mul	x25, x16, x22
	umulh	x10, x16, x22
	adds	x9, x9, x25
	adc	x10, x10, xzr
	#  A[1] * B[0]
	mul	x25, x17, x21
	umulh	x26, x17, x21
	adds	x9, x9, x25
	adcs	x10, x10, x26
	adc	x11, xzr, xzr
	#  A[0] * B[2]
	mul	x25, x16, x23
	umulh	x26, x16, x23
	adds	x10, x10, x25
	adc	x11, x11, x26
	#  A[1] * B[1]
	mul	x25, x17, x22
	umulh	x26, x17, x22
	adds	x10, x10, x25
	adcs	x11, x11, x26
	adc	x12, xzr, xzr
	#  A[2] * B[0]
	mul	x25, x19, x21
	umulh	x26, x19, x21
	adds	x10, x10, x25
	adcs	x11, x11, x26
	adc	x12, x12, xzr
	#  A[0] * B[3]
	mul	x25, x16, x24
	umulh	x26, x16, x24
	adds	x11, x11, x25
	adcs	x12, x12, x26
	adc	x13, xzr, xzr
	#  A[1] * B[2]
	mul	x25, x17, x23
	umulh	x26, x17, x23
	adds	x11, x11, x25
	adcs	x12, x12, x26
	adc	x13, x13, xzr
	#  A[2] * B[1]
	mul	x25, x19, x22
	umulh	x26, x19, x22
	adds	x11, x11, x25
	adcs	x12, x12, x26
	adc	x13, x13, xzr
	#  A[3] * B[0]
	mul	x25, x20, x21
	umulh	x26, x20, x21
	adds	x11, x11, x25
	adcs	x12, x12, x26
	adc	x13, x13, xzr
	#  A[1] * B[3]
	mul	x25, x17, x24
	umulh	x26, x17, x24
	adds	x12, x12, x25
	adcs	x13, x13, x26
	adc	x14, xzr, xzr
	#  A[2] * B[2]
	mul	x25, x19, x23
	umulh	x26, x19, x23
	adds	x12, x12, x25
	adcs	x13, x13, x26
	adc	x14, x14, xzr
	#  A[3] * B[1]
	mul	x25, x20, x22
	umulh	x26, x20, x22
	adds	x12, x12, x25
	adcs	x13, x13, x26
	adc	x14, x14, xzr
	#  A[2] * B[3]
	mul	x25, x19, x24
	umulh	x26, x19, x24
	adds	x13, x13, x25
	adcs	x14, x14, x26
	adc	x15, xzr, xzr
	#  A[3] * B[2]
	mul	x25, x20, x23
	umulh	x26, x20, x23
	adds	x13, x13, x25
	adcs	x14, x14, x26
	adc	x15, x15, xzr
	#  A[3] * B[3]
	mul	x25, x20, x24
	umulh	x26, x20, x24
	adds	x14, x14, x25
	adc	x15, x15, x26
	# Reduce
	#  Move top half into t4-t7 and remove top bit from t3
	extr	x15, x15, x14, #63
	extr	x14, x14, x13, #63
	extr	x13, x13, x12, #63
	extr	x12, x12, x11, #63
	and	x11, x11, #0x7fffffffffffffff
	#  Multiply top half by 19
	mov	x25, #19
	mul	x26, x25, x12
	umulh	x12, x25, x12
	adds	x8, x8, x26
	mul	x26, x25, x13
	umulh	x13, x25, x13
	adcs	x9, x9, x26
	mul	x26, x25, x14
	umulh	x14, x25, x14
	adcs	x10, x10, x26
	mul	x26, x25, x15
	umulh	x27, x25, x15
	adcs	x11, x11, x26
	adc	x27, x27, xzr
	#  Add remaining product results in
	adds	x9, x9, x12
	adcs	x10, x10, x13
	adcs	x11, x11, x14
	adc	x27, x27, xzr
	#  Overflow
	extr	x27, x27, x11, #63
	mul	x27, x27, x25
	and	x11, x11, #0x7fffffffffffffff
	adds	x8, x8, x27
	adcs	x9, x9, xzr
	adcs	x10, x10, xzr
	adc	x11, x11, xzr
	# Reduce if top bit set
	and	x27, x25, x11, asr 63
	and	x11, x11, #0x7fffffffffffffff
	adds	x8, x8, x27
	adcs	x9, x9, xzr
	adcs	x10, x10, xzr
	adc	x11, x11, xzr
	# Store
	ldr	x0, [x29, #40]
	ldr	x1, [x29, #32]
	# Add
	adds	x12, x4, x8
	adcs	x13, x5, x9
	adcs	x14, x6, x10
	adc	x15, x7, x11
	mov	x25, #-19
	asr	x28, x15, #63
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Sub modulus (if overflow)
	subs	x12, x12, x25
	sbcs	x13, x13, x28
	sbcs	x14, x14, x28
	sbc	x15, x15, x26
	# Sub
	subs	x16, x4, x8
	sbcs	x17, x5, x9
	sbcs	x19, x6, x10
	sbcs	x20, x7, x11
	mov	x25, #-19
	csetm	x28, cc
	#   Mask the modulus
	and	x25, x28, x25
	and	x26, x28, #0x7fffffffffffffff
	#   Add modulus (if underflow)
	adds	x16, x16, x25
	adcs	x17, x17, x28
	adcs	x19, x19, x28
	adc	x20, x20, x26
	stp	x12, x13, [x0]
	stp	x14, x15, [x0, #16]
	stp	x16, x17, [x1]
	stp	x19, x20, [x1, #16]
	ldr	x17, [x29, #88]
	ldr	x19, [x29, #96]
	ldp	x20, x21, [x29, #104]
	ldp	x22, x23, [x29, #120]
	ldp	x24, x25, [x29, #136]
	ldp	x26, x27, [x29, #152]
	ldr	x28, [x29, #168]
	ldp	x29, x30, [sp], #0xb0
	ret
#ifndef __APPLE__
	.size	fe_ge_sub,.-fe_ge_sub
#endif /* __APPLE__ */
#endif /* HAVE_CURVE25519 */
#endif /* __aarch64__ */
#endif /* WOLFSSL_ARMASM */

#if defined(__linux__) && defined(__ELF__)
.section	.note.GNU-stack,"",%progbits
#endif
