/* armv8-sha512-asm
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
 *   ruby ./sha2/sha512.rb arm64 ../wolfssl/wolfcrypt/src/port/arm/armv8-sha512-asm.S
 */
#ifdef WOLFSSL_ARMASM
#ifdef __aarch64__
#ifdef WOLFSSL_SHA512
#ifndef WOLFSSL_ARMASM_CRYPTO_SHA512
#ifndef __APPLE__
	.text
	.type	L_SHA512_transform_neon_len_k, %object
	.section	.rodata
	.size	L_SHA512_transform_neon_len_k, 640
#else
	.section	__DATA,__data
#endif /* __APPLE__ */
#ifndef __APPLE__
	.align	3
#else
	.p2align	3
#endif /* __APPLE__ */
L_SHA512_transform_neon_len_k:
	.xword	0x428a2f98d728ae22
	.xword	0x7137449123ef65cd
	.xword	0xb5c0fbcfec4d3b2f
	.xword	0xe9b5dba58189dbbc
	.xword	0x3956c25bf348b538
	.xword	0x59f111f1b605d019
	.xword	0x923f82a4af194f9b
	.xword	0xab1c5ed5da6d8118
	.xword	0xd807aa98a3030242
	.xword	0x12835b0145706fbe
	.xword	0x243185be4ee4b28c
	.xword	0x550c7dc3d5ffb4e2
	.xword	0x72be5d74f27b896f
	.xword	0x80deb1fe3b1696b1
	.xword	0x9bdc06a725c71235
	.xword	0xc19bf174cf692694
	.xword	0xe49b69c19ef14ad2
	.xword	0xefbe4786384f25e3
	.xword	0xfc19dc68b8cd5b5
	.xword	0x240ca1cc77ac9c65
	.xword	0x2de92c6f592b0275
	.xword	0x4a7484aa6ea6e483
	.xword	0x5cb0a9dcbd41fbd4
	.xword	0x76f988da831153b5
	.xword	0x983e5152ee66dfab
	.xword	0xa831c66d2db43210
	.xword	0xb00327c898fb213f
	.xword	0xbf597fc7beef0ee4
	.xword	0xc6e00bf33da88fc2
	.xword	0xd5a79147930aa725
	.xword	0x6ca6351e003826f
	.xword	0x142929670a0e6e70
	.xword	0x27b70a8546d22ffc
	.xword	0x2e1b21385c26c926
	.xword	0x4d2c6dfc5ac42aed
	.xword	0x53380d139d95b3df
	.xword	0x650a73548baf63de
	.xword	0x766a0abb3c77b2a8
	.xword	0x81c2c92e47edaee6
	.xword	0x92722c851482353b
	.xword	0xa2bfe8a14cf10364
	.xword	0xa81a664bbc423001
	.xword	0xc24b8b70d0f89791
	.xword	0xc76c51a30654be30
	.xword	0xd192e819d6ef5218
	.xword	0xd69906245565a910
	.xword	0xf40e35855771202a
	.xword	0x106aa07032bbd1b8
	.xword	0x19a4c116b8d2d0c8
	.xword	0x1e376c085141ab53
	.xword	0x2748774cdf8eeb99
	.xword	0x34b0bcb5e19b48a8
	.xword	0x391c0cb3c5c95a63
	.xword	0x4ed8aa4ae3418acb
	.xword	0x5b9cca4f7763e373
	.xword	0x682e6ff3d6b2b8a3
	.xword	0x748f82ee5defb2fc
	.xword	0x78a5636f43172f60
	.xword	0x84c87814a1f0ab72
	.xword	0x8cc702081a6439ec
	.xword	0x90befffa23631e28
	.xword	0xa4506cebde82bde9
	.xword	0xbef9a3f7b2c67915
	.xword	0xc67178f2e372532b
	.xword	0xca273eceea26619c
	.xword	0xd186b8c721c0c207
	.xword	0xeada7dd6cde0eb1e
	.xword	0xf57d4f7fee6ed178
	.xword	0x6f067aa72176fba
	.xword	0xa637dc5a2c898a6
	.xword	0x113f9804bef90dae
	.xword	0x1b710b35131c471b
	.xword	0x28db77f523047d84
	.xword	0x32caab7b40c72493
	.xword	0x3c9ebe0a15c9bebc
	.xword	0x431d67c49c100d4c
	.xword	0x4cc5d4becb3e42b6
	.xword	0x597f299cfc657e2a
	.xword	0x5fcb6fab3ad6faec
	.xword	0x6c44198c4a475817
#ifndef __APPLE__
	.text
	.type	L_SHA512_transform_neon_len_ror8, %object
	.section	.rodata
	.size	L_SHA512_transform_neon_len_ror8, 16
#else
	.section	__DATA,__data
#endif /* __APPLE__ */
#ifndef __APPLE__
	.align	4
#else
	.p2align	4
#endif /* __APPLE__ */
L_SHA512_transform_neon_len_ror8:
	.xword	0x7060504030201, 0x80f0e0d0c0b0a09
#ifndef __APPLE__
.text
.globl	Transform_Sha512_Len_neon
.type	Transform_Sha512_Len_neon,@function
.align	2
Transform_Sha512_Len_neon:
#else
.section	__TEXT,__text
.globl	_Transform_Sha512_Len_neon
.p2align	2
_Transform_Sha512_Len_neon:
#endif /* __APPLE__ */
	stp	x29, x30, [sp, #-128]!
	add	x29, sp, #0
	str	x17, [x29, #16]
	str	x19, [x29, #24]
	stp	x20, x21, [x29, #32]
	stp	x22, x23, [x29, #48]
	stp	x24, x25, [x29, #64]
	stp	x26, x27, [x29, #80]
	stp	d8, d9, [x29, #96]
	stp	d10, d11, [x29, #112]
#ifndef __APPLE__
	adrp x3, L_SHA512_transform_neon_len_k
	add  x3, x3, :lo12:L_SHA512_transform_neon_len_k
#else
	adrp x3, L_SHA512_transform_neon_len_k@PAGE
	add  x3, x3, :lo12:L_SHA512_transform_neon_len_k@PAGEOFF
#endif /* __APPLE__ */
#ifndef __APPLE__
	adrp x27, L_SHA512_transform_neon_len_ror8
	add  x27, x27, :lo12:L_SHA512_transform_neon_len_ror8
#else
	adrp x27, L_SHA512_transform_neon_len_ror8@PAGE
	add  x27, x27, :lo12:L_SHA512_transform_neon_len_ror8@PAGEOFF
#endif /* __APPLE__ */
	ld1	{v11.16b}, [x27]
	# Load digest into working vars
	ldp	x4, x5, [x0]
	ldp	x6, x7, [x0, #16]
	ldp	x8, x9, [x0, #32]
	ldp	x10, x11, [x0, #48]
	# Start of loop processing a block
L_sha512_len_neon_begin:
	# Load W
	# Copy digest to add in at end
	ld1	{v0.2d, v1.2d, v2.2d, v3.2d}, [x1], #0x40
	mov	x19, x4
	ld1	{v4.2d, v5.2d, v6.2d, v7.2d}, [x1], #0x40
	mov	x20, x5
	rev64	v0.16b, v0.16b
	mov	x21, x6
	rev64	v1.16b, v1.16b
	mov	x22, x7
	rev64	v2.16b, v2.16b
	mov	x23, x8
	rev64	v3.16b, v3.16b
	mov	x24, x9
	rev64	v4.16b, v4.16b
	mov	x25, x10
	rev64	v5.16b, v5.16b
	mov	x26, x11
	rev64	v6.16b, v6.16b
	rev64	v7.16b, v7.16b
	# Pre-calc: b ^ c
	eor	x16, x5, x6
	mov	x27, #4
	# Start of 16 rounds
L_sha512_len_neon_start:
	# Round 0
	mov	x13, v0.d[0]
	ldr	x15, [x3], #8
	ror	x12, x8, #14
	ror	x14, x4, #28
	eor	x12, x12, x8, ror 18
	eor	x14, x14, x4, ror 34
	eor	x12, x12, x8, ror 41
	eor	x14, x14, x4, ror 39
	add	x11, x11, x12
	eor	x17, x4, x5
	eor	x12, x9, x10
	and	x16, x17, x16
	and	x12, x12, x8
	add	x11, x11, x13
	eor	x12, x12, x10
	add	x11, x11, x15
	eor	x16, x16, x5
	add	x11, x11, x12
	add	x14, x14, x16
	add	x7, x7, x11
	add	x11, x11, x14
	# Round 1
	mov	x13, v0.d[1]
	ldr	x15, [x3], #8
	ext	v10.16b, v0.16b, v1.16b, #8
	ror	x12, x7, #14
	shl	v8.2d, v7.2d, #45
	ror	x14, x11, #28
	sri	v8.2d, v7.2d, #19
	eor	x12, x12, x7, ror 18
	shl	v9.2d, v7.2d, #3
	eor	x14, x14, x11, ror 34
	sri	v9.2d, v7.2d, #61
	eor	x12, x12, x7, ror 41
	eor	v9.16b, v9.16b, v8.16b
	eor	x14, x14, x11, ror 39
	ushr	v8.2d, v7.2d, #6
	add	x10, x10, x12
	eor	v9.16b, v9.16b, v8.16b
	eor	x16, x11, x4
	add	v0.2d, v0.2d, v9.2d
	eor	x12, x8, x9
	ext	v9.16b, v4.16b, v5.16b, #8
	and	x17, x16, x17
	add	v0.2d, v0.2d, v9.2d
	and	x12, x12, x7
	shl	v8.2d, v10.2d, #63
	add	x10, x10, x13
	sri	v8.2d, v10.2d, #1
	eor	x12, x12, x9
	tbl	v9.16b, {v10.16b}, v11.16b
	add	x10, x10, x15
	eor	v9.16b, v9.16b, v8.16b
	eor	x17, x17, x4
	ushr	v10.2d, v10.2d, #7
	add	x10, x10, x12
	eor	v9.16b, v9.16b, v10.16b
	add	x14, x14, x17
	add	v0.2d, v0.2d, v9.2d
	add	x6, x6, x10
	add	x10, x10, x14
	# Round 2
	mov	x13, v1.d[0]
	ldr	x15, [x3], #8
	ror	x12, x6, #14
	ror	x14, x10, #28
	eor	x12, x12, x6, ror 18
	eor	x14, x14, x10, ror 34
	eor	x12, x12, x6, ror 41
	eor	x14, x14, x10, ror 39
	add	x9, x9, x12
	eor	x17, x10, x11
	eor	x12, x7, x8
	and	x16, x17, x16
	and	x12, x12, x6
	add	x9, x9, x13
	eor	x12, x12, x8
	add	x9, x9, x15
	eor	x16, x16, x11
	add	x9, x9, x12
	add	x14, x14, x16
	add	x5, x5, x9
	add	x9, x9, x14
	# Round 3
	mov	x13, v1.d[1]
	ldr	x15, [x3], #8
	ext	v10.16b, v1.16b, v2.16b, #8
	ror	x12, x5, #14
	shl	v8.2d, v0.2d, #45
	ror	x14, x9, #28
	sri	v8.2d, v0.2d, #19
	eor	x12, x12, x5, ror 18
	shl	v9.2d, v0.2d, #3
	eor	x14, x14, x9, ror 34
	sri	v9.2d, v0.2d, #61
	eor	x12, x12, x5, ror 41
	eor	v9.16b, v9.16b, v8.16b
	eor	x14, x14, x9, ror 39
	ushr	v8.2d, v0.2d, #6
	add	x8, x8, x12
	eor	v9.16b, v9.16b, v8.16b
	eor	x16, x9, x10
	add	v1.2d, v1.2d, v9.2d
	eor	x12, x6, x7
	ext	v9.16b, v5.16b, v6.16b, #8
	and	x17, x16, x17
	add	v1.2d, v1.2d, v9.2d
	and	x12, x12, x5
	shl	v8.2d, v10.2d, #63
	add	x8, x8, x13
	sri	v8.2d, v10.2d, #1
	eor	x12, x12, x7
	tbl	v9.16b, {v10.16b}, v11.16b
	add	x8, x8, x15
	eor	v9.16b, v9.16b, v8.16b
	eor	x17, x17, x10
	ushr	v10.2d, v10.2d, #7
	add	x8, x8, x12
	eor	v9.16b, v9.16b, v10.16b
	add	x14, x14, x17
	add	v1.2d, v1.2d, v9.2d
	add	x4, x4, x8
	add	x8, x8, x14
	# Round 4
	mov	x13, v2.d[0]
	ldr	x15, [x3], #8
	ror	x12, x4, #14
	ror	x14, x8, #28
	eor	x12, x12, x4, ror 18
	eor	x14, x14, x8, ror 34
	eor	x12, x12, x4, ror 41
	eor	x14, x14, x8, ror 39
	add	x7, x7, x12
	eor	x17, x8, x9
	eor	x12, x5, x6
	and	x16, x17, x16
	and	x12, x12, x4
	add	x7, x7, x13
	eor	x12, x12, x6
	add	x7, x7, x15
	eor	x16, x16, x9
	add	x7, x7, x12
	add	x14, x14, x16
	add	x11, x11, x7
	add	x7, x7, x14
	# Round 5
	mov	x13, v2.d[1]
	ldr	x15, [x3], #8
	ext	v10.16b, v2.16b, v3.16b, #8
	ror	x12, x11, #14
	shl	v8.2d, v1.2d, #45
	ror	x14, x7, #28
	sri	v8.2d, v1.2d, #19
	eor	x12, x12, x11, ror 18
	shl	v9.2d, v1.2d, #3
	eor	x14, x14, x7, ror 34
	sri	v9.2d, v1.2d, #61
	eor	x12, x12, x11, ror 41
	eor	v9.16b, v9.16b, v8.16b
	eor	x14, x14, x7, ror 39
	ushr	v8.2d, v1.2d, #6
	add	x6, x6, x12
	eor	v9.16b, v9.16b, v8.16b
	eor	x16, x7, x8
	add	v2.2d, v2.2d, v9.2d
	eor	x12, x4, x5
	ext	v9.16b, v6.16b, v7.16b, #8
	and	x17, x16, x17
	add	v2.2d, v2.2d, v9.2d
	and	x12, x12, x11
	shl	v8.2d, v10.2d, #63
	add	x6, x6, x13
	sri	v8.2d, v10.2d, #1
	eor	x12, x12, x5
	tbl	v9.16b, {v10.16b}, v11.16b
	add	x6, x6, x15
	eor	v9.16b, v9.16b, v8.16b
	eor	x17, x17, x8
	ushr	v10.2d, v10.2d, #7
	add	x6, x6, x12
	eor	v9.16b, v9.16b, v10.16b
	add	x14, x14, x17
	add	v2.2d, v2.2d, v9.2d
	add	x10, x10, x6
	add	x6, x6, x14
	# Round 6
	mov	x13, v3.d[0]
	ldr	x15, [x3], #8
	ror	x12, x10, #14
	ror	x14, x6, #28
	eor	x12, x12, x10, ror 18
	eor	x14, x14, x6, ror 34
	eor	x12, x12, x10, ror 41
	eor	x14, x14, x6, ror 39
	add	x5, x5, x12
	eor	x17, x6, x7
	eor	x12, x11, x4
	and	x16, x17, x16
	and	x12, x12, x10
	add	x5, x5, x13
	eor	x12, x12, x4
	add	x5, x5, x15
	eor	x16, x16, x7
	add	x5, x5, x12
	add	x14, x14, x16
	add	x9, x9, x5
	add	x5, x5, x14
	# Round 7
	mov	x13, v3.d[1]
	ldr	x15, [x3], #8
	ext	v10.16b, v3.16b, v4.16b, #8
	ror	x12, x9, #14
	shl	v8.2d, v2.2d, #45
	ror	x14, x5, #28
	sri	v8.2d, v2.2d, #19
	eor	x12, x12, x9, ror 18
	shl	v9.2d, v2.2d, #3
	eor	x14, x14, x5, ror 34
	sri	v9.2d, v2.2d, #61
	eor	x12, x12, x9, ror 41
	eor	v9.16b, v9.16b, v8.16b
	eor	x14, x14, x5, ror 39
	ushr	v8.2d, v2.2d, #6
	add	x4, x4, x12
	eor	v9.16b, v9.16b, v8.16b
	eor	x16, x5, x6
	add	v3.2d, v3.2d, v9.2d
	eor	x12, x10, x11
	ext	v9.16b, v7.16b, v0.16b, #8
	and	x17, x16, x17
	add	v3.2d, v3.2d, v9.2d
	and	x12, x12, x9
	shl	v8.2d, v10.2d, #63
	add	x4, x4, x13
	sri	v8.2d, v10.2d, #1
	eor	x12, x12, x11
	tbl	v9.16b, {v10.16b}, v11.16b
	add	x4, x4, x15
	eor	v9.16b, v9.16b, v8.16b
	eor	x17, x17, x6
	ushr	v10.2d, v10.2d, #7
	add	x4, x4, x12
	eor	v9.16b, v9.16b, v10.16b
	add	x14, x14, x17
	add	v3.2d, v3.2d, v9.2d
	add	x8, x8, x4
	add	x4, x4, x14
	# Round 8
	mov	x13, v4.d[0]
	ldr	x15, [x3], #8
	ror	x12, x8, #14
	ror	x14, x4, #28
	eor	x12, x12, x8, ror 18
	eor	x14, x14, x4, ror 34
	eor	x12, x12, x8, ror 41
	eor	x14, x14, x4, ror 39
	add	x11, x11, x12
	eor	x17, x4, x5
	eor	x12, x9, x10
	and	x16, x17, x16
	and	x12, x12, x8
	add	x11, x11, x13
	eor	x12, x12, x10
	add	x11, x11, x15
	eor	x16, x16, x5
	add	x11, x11, x12
	add	x14, x14, x16
	add	x7, x7, x11
	add	x11, x11, x14
	# Round 9
	mov	x13, v4.d[1]
	ldr	x15, [x3], #8
	ext	v10.16b, v4.16b, v5.16b, #8
	ror	x12, x7, #14
	shl	v8.2d, v3.2d, #45
	ror	x14, x11, #28
	sri	v8.2d, v3.2d, #19
	eor	x12, x12, x7, ror 18
	shl	v9.2d, v3.2d, #3
	eor	x14, x14, x11, ror 34
	sri	v9.2d, v3.2d, #61
	eor	x12, x12, x7, ror 41
	eor	v9.16b, v9.16b, v8.16b
	eor	x14, x14, x11, ror 39
	ushr	v8.2d, v3.2d, #6
	add	x10, x10, x12
	eor	v9.16b, v9.16b, v8.16b
	eor	x16, x11, x4
	add	v4.2d, v4.2d, v9.2d
	eor	x12, x8, x9
	ext	v9.16b, v0.16b, v1.16b, #8
	and	x17, x16, x17
	add	v4.2d, v4.2d, v9.2d
	and	x12, x12, x7
	shl	v8.2d, v10.2d, #63
	add	x10, x10, x13
	sri	v8.2d, v10.2d, #1
	eor	x12, x12, x9
	tbl	v9.16b, {v10.16b}, v11.16b
	add	x10, x10, x15
	eor	v9.16b, v9.16b, v8.16b
	eor	x17, x17, x4
	ushr	v10.2d, v10.2d, #7
	add	x10, x10, x12
	eor	v9.16b, v9.16b, v10.16b
	add	x14, x14, x17
	add	v4.2d, v4.2d, v9.2d
	add	x6, x6, x10
	add	x10, x10, x14
	# Round 10
	mov	x13, v5.d[0]
	ldr	x15, [x3], #8
	ror	x12, x6, #14
	ror	x14, x10, #28
	eor	x12, x12, x6, ror 18
	eor	x14, x14, x10, ror 34
	eor	x12, x12, x6, ror 41
	eor	x14, x14, x10, ror 39
	add	x9, x9, x12
	eor	x17, x10, x11
	eor	x12, x7, x8
	and	x16, x17, x16
	and	x12, x12, x6
	add	x9, x9, x13
	eor	x12, x12, x8
	add	x9, x9, x15
	eor	x16, x16, x11
	add	x9, x9, x12
	add	x14, x14, x16
	add	x5, x5, x9
	add	x9, x9, x14
	# Round 11
	mov	x13, v5.d[1]
	ldr	x15, [x3], #8
	ext	v10.16b, v5.16b, v6.16b, #8
	ror	x12, x5, #14
	shl	v8.2d, v4.2d, #45
	ror	x14, x9, #28
	sri	v8.2d, v4.2d, #19
	eor	x12, x12, x5, ror 18
	shl	v9.2d, v4.2d, #3
	eor	x14, x14, x9, ror 34
	sri	v9.2d, v4.2d, #61
	eor	x12, x12, x5, ror 41
	eor	v9.16b, v9.16b, v8.16b
	eor	x14, x14, x9, ror 39
	ushr	v8.2d, v4.2d, #6
	add	x8, x8, x12
	eor	v9.16b, v9.16b, v8.16b
	eor	x16, x9, x10
	add	v5.2d, v5.2d, v9.2d
	eor	x12, x6, x7
	ext	v9.16b, v1.16b, v2.16b, #8
	and	x17, x16, x17
	add	v5.2d, v5.2d, v9.2d
	and	x12, x12, x5
	shl	v8.2d, v10.2d, #63
	add	x8, x8, x13
	sri	v8.2d, v10.2d, #1
	eor	x12, x12, x7
	tbl	v9.16b, {v10.16b}, v11.16b
	add	x8, x8, x15
	eor	v9.16b, v9.16b, v8.16b
	eor	x17, x17, x10
	ushr	v10.2d, v10.2d, #7
	add	x8, x8, x12
	eor	v9.16b, v9.16b, v10.16b
	add	x14, x14, x17
	add	v5.2d, v5.2d, v9.2d
	add	x4, x4, x8
	add	x8, x8, x14
	# Round 12
	mov	x13, v6.d[0]
	ldr	x15, [x3], #8
	ror	x12, x4, #14
	ror	x14, x8, #28
	eor	x12, x12, x4, ror 18
	eor	x14, x14, x8, ror 34
	eor	x12, x12, x4, ror 41
	eor	x14, x14, x8, ror 39
	add	x7, x7, x12
	eor	x17, x8, x9
	eor	x12, x5, x6
	and	x16, x17, x16
	and	x12, x12, x4
	add	x7, x7, x13
	eor	x12, x12, x6
	add	x7, x7, x15
	eor	x16, x16, x9
	add	x7, x7, x12
	add	x14, x14, x16
	add	x11, x11, x7
	add	x7, x7, x14
	# Round 13
	mov	x13, v6.d[1]
	ldr	x15, [x3], #8
	ext	v10.16b, v6.16b, v7.16b, #8
	ror	x12, x11, #14
	shl	v8.2d, v5.2d, #45
	ror	x14, x7, #28
	sri	v8.2d, v5.2d, #19
	eor	x12, x12, x11, ror 18
	shl	v9.2d, v5.2d, #3
	eor	x14, x14, x7, ror 34
	sri	v9.2d, v5.2d, #61
	eor	x12, x12, x11, ror 41
	eor	v9.16b, v9.16b, v8.16b
	eor	x14, x14, x7, ror 39
	ushr	v8.2d, v5.2d, #6
	add	x6, x6, x12
	eor	v9.16b, v9.16b, v8.16b
	eor	x16, x7, x8
	add	v6.2d, v6.2d, v9.2d
	eor	x12, x4, x5
	ext	v9.16b, v2.16b, v3.16b, #8
	and	x17, x16, x17
	add	v6.2d, v6.2d, v9.2d
	and	x12, x12, x11
	shl	v8.2d, v10.2d, #63
	add	x6, x6, x13
	sri	v8.2d, v10.2d, #1
	eor	x12, x12, x5
	tbl	v9.16b, {v10.16b}, v11.16b
	add	x6, x6, x15
	eor	v9.16b, v9.16b, v8.16b
	eor	x17, x17, x8
	ushr	v10.2d, v10.2d, #7
	add	x6, x6, x12
	eor	v9.16b, v9.16b, v10.16b
	add	x14, x14, x17
	add	v6.2d, v6.2d, v9.2d
	add	x10, x10, x6
	add	x6, x6, x14
	# Round 14
	mov	x13, v7.d[0]
	ldr	x15, [x3], #8
	ror	x12, x10, #14
	ror	x14, x6, #28
	eor	x12, x12, x10, ror 18
	eor	x14, x14, x6, ror 34
	eor	x12, x12, x10, ror 41
	eor	x14, x14, x6, ror 39
	add	x5, x5, x12
	eor	x17, x6, x7
	eor	x12, x11, x4
	and	x16, x17, x16
	and	x12, x12, x10
	add	x5, x5, x13
	eor	x12, x12, x4
	add	x5, x5, x15
	eor	x16, x16, x7
	add	x5, x5, x12
	add	x14, x14, x16
	add	x9, x9, x5
	add	x5, x5, x14
	# Round 15
	mov	x13, v7.d[1]
	ldr	x15, [x3], #8
	ext	v10.16b, v7.16b, v0.16b, #8
	ror	x12, x9, #14
	shl	v8.2d, v6.2d, #45
	ror	x14, x5, #28
	sri	v8.2d, v6.2d, #19
	eor	x12, x12, x9, ror 18
	shl	v9.2d, v6.2d, #3
	eor	x14, x14, x5, ror 34
	sri	v9.2d, v6.2d, #61
	eor	x12, x12, x9, ror 41
	eor	v9.16b, v9.16b, v8.16b
	eor	x14, x14, x5, ror 39
	ushr	v8.2d, v6.2d, #6
	add	x4, x4, x12
	eor	v9.16b, v9.16b, v8.16b
	eor	x16, x5, x6
	add	v7.2d, v7.2d, v9.2d
	eor	x12, x10, x11
	ext	v9.16b, v3.16b, v4.16b, #8
	and	x17, x16, x17
	add	v7.2d, v7.2d, v9.2d
	and	x12, x12, x9
	shl	v8.2d, v10.2d, #63
	add	x4, x4, x13
	sri	v8.2d, v10.2d, #1
	eor	x12, x12, x11
	tbl	v9.16b, {v10.16b}, v11.16b
	add	x4, x4, x15
	eor	v9.16b, v9.16b, v8.16b
	eor	x17, x17, x6
	ushr	v10.2d, v10.2d, #7
	add	x4, x4, x12
	eor	v9.16b, v9.16b, v10.16b
	add	x14, x14, x17
	add	v7.2d, v7.2d, v9.2d
	add	x8, x8, x4
	add	x4, x4, x14
	subs	x27, x27, #1
	bne	L_sha512_len_neon_start
	# Round 0
	mov	x13, v0.d[0]
	ldr	x15, [x3], #8
	ror	x12, x8, #14
	ror	x14, x4, #28
	eor	x12, x12, x8, ror 18
	eor	x14, x14, x4, ror 34
	eor	x12, x12, x8, ror 41
	eor	x14, x14, x4, ror 39
	add	x11, x11, x12
	eor	x17, x4, x5
	eor	x12, x9, x10
	and	x16, x17, x16
	and	x12, x12, x8
	add	x11, x11, x13
	eor	x12, x12, x10
	add	x11, x11, x15
	eor	x16, x16, x5
	add	x11, x11, x12
	add	x14, x14, x16
	add	x7, x7, x11
	add	x11, x11, x14
	# Round 1
	mov	x13, v0.d[1]
	ldr	x15, [x3], #8
	ror	x12, x7, #14
	ror	x14, x11, #28
	eor	x12, x12, x7, ror 18
	eor	x14, x14, x11, ror 34
	eor	x12, x12, x7, ror 41
	eor	x14, x14, x11, ror 39
	add	x10, x10, x12
	eor	x16, x11, x4
	eor	x12, x8, x9
	and	x17, x16, x17
	and	x12, x12, x7
	add	x10, x10, x13
	eor	x12, x12, x9
	add	x10, x10, x15
	eor	x17, x17, x4
	add	x10, x10, x12
	add	x14, x14, x17
	add	x6, x6, x10
	add	x10, x10, x14
	# Round 2
	mov	x13, v1.d[0]
	ldr	x15, [x3], #8
	ror	x12, x6, #14
	ror	x14, x10, #28
	eor	x12, x12, x6, ror 18
	eor	x14, x14, x10, ror 34
	eor	x12, x12, x6, ror 41
	eor	x14, x14, x10, ror 39
	add	x9, x9, x12
	eor	x17, x10, x11
	eor	x12, x7, x8
	and	x16, x17, x16
	and	x12, x12, x6
	add	x9, x9, x13
	eor	x12, x12, x8
	add	x9, x9, x15
	eor	x16, x16, x11
	add	x9, x9, x12
	add	x14, x14, x16
	add	x5, x5, x9
	add	x9, x9, x14
	# Round 3
	mov	x13, v1.d[1]
	ldr	x15, [x3], #8
	ror	x12, x5, #14
	ror	x14, x9, #28
	eor	x12, x12, x5, ror 18
	eor	x14, x14, x9, ror 34
	eor	x12, x12, x5, ror 41
	eor	x14, x14, x9, ror 39
	add	x8, x8, x12
	eor	x16, x9, x10
	eor	x12, x6, x7
	and	x17, x16, x17
	and	x12, x12, x5
	add	x8, x8, x13
	eor	x12, x12, x7
	add	x8, x8, x15
	eor	x17, x17, x10
	add	x8, x8, x12
	add	x14, x14, x17
	add	x4, x4, x8
	add	x8, x8, x14
	# Round 4
	mov	x13, v2.d[0]
	ldr	x15, [x3], #8
	ror	x12, x4, #14
	ror	x14, x8, #28
	eor	x12, x12, x4, ror 18
	eor	x14, x14, x8, ror 34
	eor	x12, x12, x4, ror 41
	eor	x14, x14, x8, ror 39
	add	x7, x7, x12
	eor	x17, x8, x9
	eor	x12, x5, x6
	and	x16, x17, x16
	and	x12, x12, x4
	add	x7, x7, x13
	eor	x12, x12, x6
	add	x7, x7, x15
	eor	x16, x16, x9
	add	x7, x7, x12
	add	x14, x14, x16
	add	x11, x11, x7
	add	x7, x7, x14
	# Round 5
	mov	x13, v2.d[1]
	ldr	x15, [x3], #8
	ror	x12, x11, #14
	ror	x14, x7, #28
	eor	x12, x12, x11, ror 18
	eor	x14, x14, x7, ror 34
	eor	x12, x12, x11, ror 41
	eor	x14, x14, x7, ror 39
	add	x6, x6, x12
	eor	x16, x7, x8
	eor	x12, x4, x5
	and	x17, x16, x17
	and	x12, x12, x11
	add	x6, x6, x13
	eor	x12, x12, x5
	add	x6, x6, x15
	eor	x17, x17, x8
	add	x6, x6, x12
	add	x14, x14, x17
	add	x10, x10, x6
	add	x6, x6, x14
	# Round 6
	mov	x13, v3.d[0]
	ldr	x15, [x3], #8
	ror	x12, x10, #14
	ror	x14, x6, #28
	eor	x12, x12, x10, ror 18
	eor	x14, x14, x6, ror 34
	eor	x12, x12, x10, ror 41
	eor	x14, x14, x6, ror 39
	add	x5, x5, x12
	eor	x17, x6, x7
	eor	x12, x11, x4
	and	x16, x17, x16
	and	x12, x12, x10
	add	x5, x5, x13
	eor	x12, x12, x4
	add	x5, x5, x15
	eor	x16, x16, x7
	add	x5, x5, x12
	add	x14, x14, x16
	add	x9, x9, x5
	add	x5, x5, x14
	# Round 7
	mov	x13, v3.d[1]
	ldr	x15, [x3], #8
	ror	x12, x9, #14
	ror	x14, x5, #28
	eor	x12, x12, x9, ror 18
	eor	x14, x14, x5, ror 34
	eor	x12, x12, x9, ror 41
	eor	x14, x14, x5, ror 39
	add	x4, x4, x12
	eor	x16, x5, x6
	eor	x12, x10, x11
	and	x17, x16, x17
	and	x12, x12, x9
	add	x4, x4, x13
	eor	x12, x12, x11
	add	x4, x4, x15
	eor	x17, x17, x6
	add	x4, x4, x12
	add	x14, x14, x17
	add	x8, x8, x4
	add	x4, x4, x14
	# Round 8
	mov	x13, v4.d[0]
	ldr	x15, [x3], #8
	ror	x12, x8, #14
	ror	x14, x4, #28
	eor	x12, x12, x8, ror 18
	eor	x14, x14, x4, ror 34
	eor	x12, x12, x8, ror 41
	eor	x14, x14, x4, ror 39
	add	x11, x11, x12
	eor	x17, x4, x5
	eor	x12, x9, x10
	and	x16, x17, x16
	and	x12, x12, x8
	add	x11, x11, x13
	eor	x12, x12, x10
	add	x11, x11, x15
	eor	x16, x16, x5
	add	x11, x11, x12
	add	x14, x14, x16
	add	x7, x7, x11
	add	x11, x11, x14
	# Round 9
	mov	x13, v4.d[1]
	ldr	x15, [x3], #8
	ror	x12, x7, #14
	ror	x14, x11, #28
	eor	x12, x12, x7, ror 18
	eor	x14, x14, x11, ror 34
	eor	x12, x12, x7, ror 41
	eor	x14, x14, x11, ror 39
	add	x10, x10, x12
	eor	x16, x11, x4
	eor	x12, x8, x9
	and	x17, x16, x17
	and	x12, x12, x7
	add	x10, x10, x13
	eor	x12, x12, x9
	add	x10, x10, x15
	eor	x17, x17, x4
	add	x10, x10, x12
	add	x14, x14, x17
	add	x6, x6, x10
	add	x10, x10, x14
	# Round 10
	mov	x13, v5.d[0]
	ldr	x15, [x3], #8
	ror	x12, x6, #14
	ror	x14, x10, #28
	eor	x12, x12, x6, ror 18
	eor	x14, x14, x10, ror 34
	eor	x12, x12, x6, ror 41
	eor	x14, x14, x10, ror 39
	add	x9, x9, x12
	eor	x17, x10, x11
	eor	x12, x7, x8
	and	x16, x17, x16
	and	x12, x12, x6
	add	x9, x9, x13
	eor	x12, x12, x8
	add	x9, x9, x15
	eor	x16, x16, x11
	add	x9, x9, x12
	add	x14, x14, x16
	add	x5, x5, x9
	add	x9, x9, x14
	# Round 11
	mov	x13, v5.d[1]
	ldr	x15, [x3], #8
	ror	x12, x5, #14
	ror	x14, x9, #28
	eor	x12, x12, x5, ror 18
	eor	x14, x14, x9, ror 34
	eor	x12, x12, x5, ror 41
	eor	x14, x14, x9, ror 39
	add	x8, x8, x12
	eor	x16, x9, x10
	eor	x12, x6, x7
	and	x17, x16, x17
	and	x12, x12, x5
	add	x8, x8, x13
	eor	x12, x12, x7
	add	x8, x8, x15
	eor	x17, x17, x10
	add	x8, x8, x12
	add	x14, x14, x17
	add	x4, x4, x8
	add	x8, x8, x14
	# Round 12
	mov	x13, v6.d[0]
	ldr	x15, [x3], #8
	ror	x12, x4, #14
	ror	x14, x8, #28
	eor	x12, x12, x4, ror 18
	eor	x14, x14, x8, ror 34
	eor	x12, x12, x4, ror 41
	eor	x14, x14, x8, ror 39
	add	x7, x7, x12
	eor	x17, x8, x9
	eor	x12, x5, x6
	and	x16, x17, x16
	and	x12, x12, x4
	add	x7, x7, x13
	eor	x12, x12, x6
	add	x7, x7, x15
	eor	x16, x16, x9
	add	x7, x7, x12
	add	x14, x14, x16
	add	x11, x11, x7
	add	x7, x7, x14
	# Round 13
	mov	x13, v6.d[1]
	ldr	x15, [x3], #8
	ror	x12, x11, #14
	ror	x14, x7, #28
	eor	x12, x12, x11, ror 18
	eor	x14, x14, x7, ror 34
	eor	x12, x12, x11, ror 41
	eor	x14, x14, x7, ror 39
	add	x6, x6, x12
	eor	x16, x7, x8
	eor	x12, x4, x5
	and	x17, x16, x17
	and	x12, x12, x11
	add	x6, x6, x13
	eor	x12, x12, x5
	add	x6, x6, x15
	eor	x17, x17, x8
	add	x6, x6, x12
	add	x14, x14, x17
	add	x10, x10, x6
	add	x6, x6, x14
	# Round 14
	mov	x13, v7.d[0]
	ldr	x15, [x3], #8
	ror	x12, x10, #14
	ror	x14, x6, #28
	eor	x12, x12, x10, ror 18
	eor	x14, x14, x6, ror 34
	eor	x12, x12, x10, ror 41
	eor	x14, x14, x6, ror 39
	add	x5, x5, x12
	eor	x17, x6, x7
	eor	x12, x11, x4
	and	x16, x17, x16
	and	x12, x12, x10
	add	x5, x5, x13
	eor	x12, x12, x4
	add	x5, x5, x15
	eor	x16, x16, x7
	add	x5, x5, x12
	add	x14, x14, x16
	add	x9, x9, x5
	add	x5, x5, x14
	# Round 15
	mov	x13, v7.d[1]
	ldr	x15, [x3], #8
	ror	x12, x9, #14
	ror	x14, x5, #28
	eor	x12, x12, x9, ror 18
	eor	x14, x14, x5, ror 34
	eor	x12, x12, x9, ror 41
	eor	x14, x14, x5, ror 39
	add	x4, x4, x12
	eor	x16, x5, x6
	eor	x12, x10, x11
	and	x17, x16, x17
	and	x12, x12, x9
	add	x4, x4, x13
	eor	x12, x12, x11
	add	x4, x4, x15
	eor	x17, x17, x6
	add	x4, x4, x12
	add	x14, x14, x17
	add	x8, x8, x4
	add	x4, x4, x14
	add	x11, x11, x26
	add	x10, x10, x25
	add	x9, x9, x24
	add	x8, x8, x23
	add	x7, x7, x22
	add	x6, x6, x21
	add	x5, x5, x20
	add	x4, x4, x19
#ifndef __APPLE__
	adrp x3, L_SHA512_transform_neon_len_k
	add  x3, x3, :lo12:L_SHA512_transform_neon_len_k
#else
	adrp x3, L_SHA512_transform_neon_len_k@PAGE
	add  x3, x3, :lo12:L_SHA512_transform_neon_len_k@PAGEOFF
#endif /* __APPLE__ */
	subs	w2, w2, #0x80
	bne	L_sha512_len_neon_begin
	stp	x4, x5, [x0]
	stp	x6, x7, [x0, #16]
	stp	x8, x9, [x0, #32]
	stp	x10, x11, [x0, #48]
	ldr	x17, [x29, #16]
	ldr	x19, [x29, #24]
	ldp	x20, x21, [x29, #32]
	ldp	x22, x23, [x29, #48]
	ldp	x24, x25, [x29, #64]
	ldp	x26, x27, [x29, #80]
	ldp	d8, d9, [x29, #96]
	ldp	d10, d11, [x29, #112]
	ldp	x29, x30, [sp], #0x80
	ret
#ifndef __APPLE__
	.size	Transform_Sha512_Len_neon,.-Transform_Sha512_Len_neon
#endif /* __APPLE__ */
#else
#ifndef __APPLE__
	.text
	.type	L_SHA512_transform_crypto_len_k, %object
	.section	.rodata
	.size	L_SHA512_transform_crypto_len_k, 640
#else
	.section	__DATA,__data
#endif /* __APPLE__ */
#ifndef __APPLE__
	.align	3
#else
	.p2align	3
#endif /* __APPLE__ */
L_SHA512_transform_crypto_len_k:
	.xword	0x428a2f98d728ae22
	.xword	0x7137449123ef65cd
	.xword	0xb5c0fbcfec4d3b2f
	.xword	0xe9b5dba58189dbbc
	.xword	0x3956c25bf348b538
	.xword	0x59f111f1b605d019
	.xword	0x923f82a4af194f9b
	.xword	0xab1c5ed5da6d8118
	.xword	0xd807aa98a3030242
	.xword	0x12835b0145706fbe
	.xword	0x243185be4ee4b28c
	.xword	0x550c7dc3d5ffb4e2
	.xword	0x72be5d74f27b896f
	.xword	0x80deb1fe3b1696b1
	.xword	0x9bdc06a725c71235
	.xword	0xc19bf174cf692694
	.xword	0xe49b69c19ef14ad2
	.xword	0xefbe4786384f25e3
	.xword	0xfc19dc68b8cd5b5
	.xword	0x240ca1cc77ac9c65
	.xword	0x2de92c6f592b0275
	.xword	0x4a7484aa6ea6e483
	.xword	0x5cb0a9dcbd41fbd4
	.xword	0x76f988da831153b5
	.xword	0x983e5152ee66dfab
	.xword	0xa831c66d2db43210
	.xword	0xb00327c898fb213f
	.xword	0xbf597fc7beef0ee4
	.xword	0xc6e00bf33da88fc2
	.xword	0xd5a79147930aa725
	.xword	0x6ca6351e003826f
	.xword	0x142929670a0e6e70
	.xword	0x27b70a8546d22ffc
	.xword	0x2e1b21385c26c926
	.xword	0x4d2c6dfc5ac42aed
	.xword	0x53380d139d95b3df
	.xword	0x650a73548baf63de
	.xword	0x766a0abb3c77b2a8
	.xword	0x81c2c92e47edaee6
	.xword	0x92722c851482353b
	.xword	0xa2bfe8a14cf10364
	.xword	0xa81a664bbc423001
	.xword	0xc24b8b70d0f89791
	.xword	0xc76c51a30654be30
	.xword	0xd192e819d6ef5218
	.xword	0xd69906245565a910
	.xword	0xf40e35855771202a
	.xword	0x106aa07032bbd1b8
	.xword	0x19a4c116b8d2d0c8
	.xword	0x1e376c085141ab53
	.xword	0x2748774cdf8eeb99
	.xword	0x34b0bcb5e19b48a8
	.xword	0x391c0cb3c5c95a63
	.xword	0x4ed8aa4ae3418acb
	.xword	0x5b9cca4f7763e373
	.xword	0x682e6ff3d6b2b8a3
	.xword	0x748f82ee5defb2fc
	.xword	0x78a5636f43172f60
	.xword	0x84c87814a1f0ab72
	.xword	0x8cc702081a6439ec
	.xword	0x90befffa23631e28
	.xword	0xa4506cebde82bde9
	.xword	0xbef9a3f7b2c67915
	.xword	0xc67178f2e372532b
	.xword	0xca273eceea26619c
	.xword	0xd186b8c721c0c207
	.xword	0xeada7dd6cde0eb1e
	.xword	0xf57d4f7fee6ed178
	.xword	0x6f067aa72176fba
	.xword	0xa637dc5a2c898a6
	.xword	0x113f9804bef90dae
	.xword	0x1b710b35131c471b
	.xword	0x28db77f523047d84
	.xword	0x32caab7b40c72493
	.xword	0x3c9ebe0a15c9bebc
	.xword	0x431d67c49c100d4c
	.xword	0x4cc5d4becb3e42b6
	.xword	0x597f299cfc657e2a
	.xword	0x5fcb6fab3ad6faec
	.xword	0x6c44198c4a475817
#ifndef __APPLE__
.text
.globl	Transform_Sha512_Len_crypto
.type	Transform_Sha512_Len_crypto,@function
.align	2
Transform_Sha512_Len_crypto:
#else
.section	__TEXT,__text
.globl	_Transform_Sha512_Len_crypto
.p2align	2
_Transform_Sha512_Len_crypto:
#endif /* __APPLE__ */
	stp	x29, x30, [sp, #-80]!
	add	x29, sp, #0
	stp	d8, d9, [x29, #16]
	stp	d10, d11, [x29, #32]
	stp	d12, d13, [x29, #48]
	stp	d14, d15, [x29, #64]
#ifdef __APPLE__
.arch_extension sha3
#endif /* __APPLE__ */
#ifndef __APPLE__
	adrp x4, L_SHA512_transform_crypto_len_k
	add  x4, x4, :lo12:L_SHA512_transform_crypto_len_k
#else
	adrp x4, L_SHA512_transform_crypto_len_k@PAGE
	add  x4, x4, :lo12:L_SHA512_transform_crypto_len_k@PAGEOFF
#endif /* __APPLE__ */
	# Load first 16 64-bit words of K permanently
	ld1	{v8.2d, v9.2d, v10.2d, v11.2d}, [x4], #0x40
	ld1	{v12.2d, v13.2d, v14.2d, v15.2d}, [x4], #0x40
	# Load digest into working vars
	ld1	{v24.2d, v25.2d, v26.2d, v27.2d}, [x0]
	# Start of loop processing a block
L_sha512_len_crypto_begin:
	mov	x3, x4
	# Load W
	ld1	{v0.2d, v1.2d, v2.2d, v3.2d}, [x1], #0x40
	ld1	{v4.2d, v5.2d, v6.2d, v7.2d}, [x1], #0x40
	rev64	v0.16b, v0.16b
	rev64	v1.16b, v1.16b
	rev64	v2.16b, v2.16b
	rev64	v3.16b, v3.16b
	rev64	v4.16b, v4.16b
	rev64	v5.16b, v5.16b
	rev64	v6.16b, v6.16b
	rev64	v7.16b, v7.16b
	# Copy digest to add in at end
	mov	v28.16b, v24.16b
	mov	v29.16b, v25.16b
	mov	v30.16b, v26.16b
	mov	v31.16b, v27.16b
	# Start of 16 rounds
	# Round 0
	add	v20.2d, v0.2d, v8.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v26.16b, v27.16b, #8
	ext	v22.16b, v25.16b, v26.16b, #8
	add	v27.2d, v27.2d, v20.2d
	sha512h	q27, q21, v22.2d
	add	v23.2d, v25.2d, v27.2d
	sha512h2	q27, q25, v24.2d
	# Round 1
	add	v20.2d, v1.2d, v9.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v23.16b, v26.16b, #8
	ext	v22.16b, v24.16b, v23.16b, #8
	add	v26.2d, v26.2d, v20.2d
	sha512h	q26, q21, v22.2d
	add	v25.2d, v24.2d, v26.2d
	sha512h2	q26, q24, v27.2d
	# Round 2
	add	v20.2d, v2.2d, v10.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v25.16b, v23.16b, #8
	ext	v22.16b, v27.16b, v25.16b, #8
	add	v23.2d, v23.2d, v20.2d
	sha512h	q23, q21, v22.2d
	add	v24.2d, v27.2d, v23.2d
	sha512h2	q23, q27, v26.2d
	# Round 3
	add	v20.2d, v3.2d, v11.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v24.16b, v25.16b, #8
	ext	v22.16b, v26.16b, v24.16b, #8
	add	v25.2d, v25.2d, v20.2d
	sha512h	q25, q21, v22.2d
	add	v27.2d, v26.2d, v25.2d
	sha512h2	q25, q26, v23.2d
	# Round 4
	add	v20.2d, v4.2d, v12.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v27.16b, v24.16b, #8
	ext	v22.16b, v23.16b, v27.16b, #8
	add	v24.2d, v24.2d, v20.2d
	sha512h	q24, q21, v22.2d
	add	v26.2d, v23.2d, v24.2d
	sha512h2	q24, q23, v25.2d
	# Round 5
	add	v20.2d, v5.2d, v13.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v26.16b, v27.16b, #8
	ext	v22.16b, v25.16b, v26.16b, #8
	add	v27.2d, v27.2d, v20.2d
	sha512h	q27, q21, v22.2d
	add	v23.2d, v25.2d, v27.2d
	sha512h2	q27, q25, v24.2d
	# Round 6
	add	v20.2d, v6.2d, v14.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v23.16b, v26.16b, #8
	ext	v22.16b, v24.16b, v23.16b, #8
	add	v26.2d, v26.2d, v20.2d
	sha512h	q26, q21, v22.2d
	add	v25.2d, v24.2d, v26.2d
	sha512h2	q26, q24, v27.2d
	# Round 7
	add	v20.2d, v7.2d, v15.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v25.16b, v23.16b, #8
	ext	v22.16b, v27.16b, v25.16b, #8
	add	v23.2d, v23.2d, v20.2d
	sha512h	q23, q21, v22.2d
	add	v24.2d, v27.2d, v23.2d
	sha512h2	q23, q27, v26.2d
	# Load next 8 64-bit words of K
	ld1	{v16.2d, v17.2d, v18.2d, v19.2d}, [x3], #0x40
	# Round 8
	sha512su0	v0.2d, v1.2d
	ext	v21.16b, v4.16b, v5.16b, #8
	sha512su1	v0.2d, v7.2d, v21.2d
	add	v20.2d, v0.2d, v16.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v24.16b, v25.16b, #8
	ext	v22.16b, v26.16b, v24.16b, #8
	add	v25.2d, v25.2d, v20.2d
	sha512h	q25, q21, v22.2d
	add	v27.2d, v26.2d, v25.2d
	sha512h2	q25, q26, v23.2d
	# Round 9
	sha512su0	v1.2d, v2.2d
	ext	v21.16b, v5.16b, v6.16b, #8
	sha512su1	v1.2d, v0.2d, v21.2d
	add	v20.2d, v1.2d, v17.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v27.16b, v24.16b, #8
	ext	v22.16b, v23.16b, v27.16b, #8
	add	v24.2d, v24.2d, v20.2d
	sha512h	q24, q21, v22.2d
	add	v26.2d, v23.2d, v24.2d
	sha512h2	q24, q23, v25.2d
	# Round 10
	sha512su0	v2.2d, v3.2d
	ext	v21.16b, v6.16b, v7.16b, #8
	sha512su1	v2.2d, v1.2d, v21.2d
	add	v20.2d, v2.2d, v18.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v26.16b, v27.16b, #8
	ext	v22.16b, v25.16b, v26.16b, #8
	add	v27.2d, v27.2d, v20.2d
	sha512h	q27, q21, v22.2d
	add	v23.2d, v25.2d, v27.2d
	sha512h2	q27, q25, v24.2d
	# Round 11
	sha512su0	v3.2d, v4.2d
	ext	v21.16b, v7.16b, v0.16b, #8
	sha512su1	v3.2d, v2.2d, v21.2d
	add	v20.2d, v3.2d, v19.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v23.16b, v26.16b, #8
	ext	v22.16b, v24.16b, v23.16b, #8
	add	v26.2d, v26.2d, v20.2d
	sha512h	q26, q21, v22.2d
	add	v25.2d, v24.2d, v26.2d
	sha512h2	q26, q24, v27.2d
	# Load next 8 64-bit words of K
	ld1	{v16.2d, v17.2d, v18.2d, v19.2d}, [x3], #0x40
	# Round 12
	sha512su0	v4.2d, v5.2d
	ext	v21.16b, v0.16b, v1.16b, #8
	sha512su1	v4.2d, v3.2d, v21.2d
	add	v20.2d, v4.2d, v16.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v25.16b, v23.16b, #8
	ext	v22.16b, v27.16b, v25.16b, #8
	add	v23.2d, v23.2d, v20.2d
	sha512h	q23, q21, v22.2d
	add	v24.2d, v27.2d, v23.2d
	sha512h2	q23, q27, v26.2d
	# Round 13
	sha512su0	v5.2d, v6.2d
	ext	v21.16b, v1.16b, v2.16b, #8
	sha512su1	v5.2d, v4.2d, v21.2d
	add	v20.2d, v5.2d, v17.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v24.16b, v25.16b, #8
	ext	v22.16b, v26.16b, v24.16b, #8
	add	v25.2d, v25.2d, v20.2d
	sha512h	q25, q21, v22.2d
	add	v27.2d, v26.2d, v25.2d
	sha512h2	q25, q26, v23.2d
	# Round 14
	sha512su0	v6.2d, v7.2d
	ext	v21.16b, v2.16b, v3.16b, #8
	sha512su1	v6.2d, v5.2d, v21.2d
	add	v20.2d, v6.2d, v18.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v27.16b, v24.16b, #8
	ext	v22.16b, v23.16b, v27.16b, #8
	add	v24.2d, v24.2d, v20.2d
	sha512h	q24, q21, v22.2d
	add	v26.2d, v23.2d, v24.2d
	sha512h2	q24, q23, v25.2d
	# Round 15
	sha512su0	v7.2d, v0.2d
	ext	v21.16b, v3.16b, v4.16b, #8
	sha512su1	v7.2d, v6.2d, v21.2d
	add	v20.2d, v7.2d, v19.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v26.16b, v27.16b, #8
	ext	v22.16b, v25.16b, v26.16b, #8
	add	v27.2d, v27.2d, v20.2d
	sha512h	q27, q21, v22.2d
	add	v23.2d, v25.2d, v27.2d
	sha512h2	q27, q25, v24.2d
	# Load next 8 64-bit words of K
	ld1	{v16.2d, v17.2d, v18.2d, v19.2d}, [x3], #0x40
	# Round 16
	sha512su0	v0.2d, v1.2d
	ext	v21.16b, v4.16b, v5.16b, #8
	sha512su1	v0.2d, v7.2d, v21.2d
	add	v20.2d, v0.2d, v16.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v23.16b, v26.16b, #8
	ext	v22.16b, v24.16b, v23.16b, #8
	add	v26.2d, v26.2d, v20.2d
	sha512h	q26, q21, v22.2d
	add	v25.2d, v24.2d, v26.2d
	sha512h2	q26, q24, v27.2d
	# Round 17
	sha512su0	v1.2d, v2.2d
	ext	v21.16b, v5.16b, v6.16b, #8
	sha512su1	v1.2d, v0.2d, v21.2d
	add	v20.2d, v1.2d, v17.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v25.16b, v23.16b, #8
	ext	v22.16b, v27.16b, v25.16b, #8
	add	v23.2d, v23.2d, v20.2d
	sha512h	q23, q21, v22.2d
	add	v24.2d, v27.2d, v23.2d
	sha512h2	q23, q27, v26.2d
	# Round 18
	sha512su0	v2.2d, v3.2d
	ext	v21.16b, v6.16b, v7.16b, #8
	sha512su1	v2.2d, v1.2d, v21.2d
	add	v20.2d, v2.2d, v18.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v24.16b, v25.16b, #8
	ext	v22.16b, v26.16b, v24.16b, #8
	add	v25.2d, v25.2d, v20.2d
	sha512h	q25, q21, v22.2d
	add	v27.2d, v26.2d, v25.2d
	sha512h2	q25, q26, v23.2d
	# Round 19
	sha512su0	v3.2d, v4.2d
	ext	v21.16b, v7.16b, v0.16b, #8
	sha512su1	v3.2d, v2.2d, v21.2d
	add	v20.2d, v3.2d, v19.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v27.16b, v24.16b, #8
	ext	v22.16b, v23.16b, v27.16b, #8
	add	v24.2d, v24.2d, v20.2d
	sha512h	q24, q21, v22.2d
	add	v26.2d, v23.2d, v24.2d
	sha512h2	q24, q23, v25.2d
	# Load next 8 64-bit words of K
	ld1	{v16.2d, v17.2d, v18.2d, v19.2d}, [x3], #0x40
	# Round 20
	sha512su0	v4.2d, v5.2d
	ext	v21.16b, v0.16b, v1.16b, #8
	sha512su1	v4.2d, v3.2d, v21.2d
	add	v20.2d, v4.2d, v16.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v26.16b, v27.16b, #8
	ext	v22.16b, v25.16b, v26.16b, #8
	add	v27.2d, v27.2d, v20.2d
	sha512h	q27, q21, v22.2d
	add	v23.2d, v25.2d, v27.2d
	sha512h2	q27, q25, v24.2d
	# Round 21
	sha512su0	v5.2d, v6.2d
	ext	v21.16b, v1.16b, v2.16b, #8
	sha512su1	v5.2d, v4.2d, v21.2d
	add	v20.2d, v5.2d, v17.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v23.16b, v26.16b, #8
	ext	v22.16b, v24.16b, v23.16b, #8
	add	v26.2d, v26.2d, v20.2d
	sha512h	q26, q21, v22.2d
	add	v25.2d, v24.2d, v26.2d
	sha512h2	q26, q24, v27.2d
	# Round 22
	sha512su0	v6.2d, v7.2d
	ext	v21.16b, v2.16b, v3.16b, #8
	sha512su1	v6.2d, v5.2d, v21.2d
	add	v20.2d, v6.2d, v18.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v25.16b, v23.16b, #8
	ext	v22.16b, v27.16b, v25.16b, #8
	add	v23.2d, v23.2d, v20.2d
	sha512h	q23, q21, v22.2d
	add	v24.2d, v27.2d, v23.2d
	sha512h2	q23, q27, v26.2d
	# Round 23
	sha512su0	v7.2d, v0.2d
	ext	v21.16b, v3.16b, v4.16b, #8
	sha512su1	v7.2d, v6.2d, v21.2d
	add	v20.2d, v7.2d, v19.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v24.16b, v25.16b, #8
	ext	v22.16b, v26.16b, v24.16b, #8
	add	v25.2d, v25.2d, v20.2d
	sha512h	q25, q21, v22.2d
	add	v27.2d, v26.2d, v25.2d
	sha512h2	q25, q26, v23.2d
	# Load next 8 64-bit words of K
	ld1	{v16.2d, v17.2d, v18.2d, v19.2d}, [x3], #0x40
	# Round 24
	sha512su0	v0.2d, v1.2d
	ext	v21.16b, v4.16b, v5.16b, #8
	sha512su1	v0.2d, v7.2d, v21.2d
	add	v20.2d, v0.2d, v16.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v27.16b, v24.16b, #8
	ext	v22.16b, v23.16b, v27.16b, #8
	add	v24.2d, v24.2d, v20.2d
	sha512h	q24, q21, v22.2d
	add	v26.2d, v23.2d, v24.2d
	sha512h2	q24, q23, v25.2d
	# Round 25
	sha512su0	v1.2d, v2.2d
	ext	v21.16b, v5.16b, v6.16b, #8
	sha512su1	v1.2d, v0.2d, v21.2d
	add	v20.2d, v1.2d, v17.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v26.16b, v27.16b, #8
	ext	v22.16b, v25.16b, v26.16b, #8
	add	v27.2d, v27.2d, v20.2d
	sha512h	q27, q21, v22.2d
	add	v23.2d, v25.2d, v27.2d
	sha512h2	q27, q25, v24.2d
	# Round 26
	sha512su0	v2.2d, v3.2d
	ext	v21.16b, v6.16b, v7.16b, #8
	sha512su1	v2.2d, v1.2d, v21.2d
	add	v20.2d, v2.2d, v18.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v23.16b, v26.16b, #8
	ext	v22.16b, v24.16b, v23.16b, #8
	add	v26.2d, v26.2d, v20.2d
	sha512h	q26, q21, v22.2d
	add	v25.2d, v24.2d, v26.2d
	sha512h2	q26, q24, v27.2d
	# Round 27
	sha512su0	v3.2d, v4.2d
	ext	v21.16b, v7.16b, v0.16b, #8
	sha512su1	v3.2d, v2.2d, v21.2d
	add	v20.2d, v3.2d, v19.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v25.16b, v23.16b, #8
	ext	v22.16b, v27.16b, v25.16b, #8
	add	v23.2d, v23.2d, v20.2d
	sha512h	q23, q21, v22.2d
	add	v24.2d, v27.2d, v23.2d
	sha512h2	q23, q27, v26.2d
	# Load next 8 64-bit words of K
	ld1	{v16.2d, v17.2d, v18.2d, v19.2d}, [x3], #0x40
	# Round 28
	sha512su0	v4.2d, v5.2d
	ext	v21.16b, v0.16b, v1.16b, #8
	sha512su1	v4.2d, v3.2d, v21.2d
	add	v20.2d, v4.2d, v16.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v24.16b, v25.16b, #8
	ext	v22.16b, v26.16b, v24.16b, #8
	add	v25.2d, v25.2d, v20.2d
	sha512h	q25, q21, v22.2d
	add	v27.2d, v26.2d, v25.2d
	sha512h2	q25, q26, v23.2d
	# Round 29
	sha512su0	v5.2d, v6.2d
	ext	v21.16b, v1.16b, v2.16b, #8
	sha512su1	v5.2d, v4.2d, v21.2d
	add	v20.2d, v5.2d, v17.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v27.16b, v24.16b, #8
	ext	v22.16b, v23.16b, v27.16b, #8
	add	v24.2d, v24.2d, v20.2d
	sha512h	q24, q21, v22.2d
	add	v26.2d, v23.2d, v24.2d
	sha512h2	q24, q23, v25.2d
	# Round 30
	sha512su0	v6.2d, v7.2d
	ext	v21.16b, v2.16b, v3.16b, #8
	sha512su1	v6.2d, v5.2d, v21.2d
	add	v20.2d, v6.2d, v18.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v26.16b, v27.16b, #8
	ext	v22.16b, v25.16b, v26.16b, #8
	add	v27.2d, v27.2d, v20.2d
	sha512h	q27, q21, v22.2d
	add	v23.2d, v25.2d, v27.2d
	sha512h2	q27, q25, v24.2d
	# Round 31
	sha512su0	v7.2d, v0.2d
	ext	v21.16b, v3.16b, v4.16b, #8
	sha512su1	v7.2d, v6.2d, v21.2d
	add	v20.2d, v7.2d, v19.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v23.16b, v26.16b, #8
	ext	v22.16b, v24.16b, v23.16b, #8
	add	v26.2d, v26.2d, v20.2d
	sha512h	q26, q21, v22.2d
	add	v25.2d, v24.2d, v26.2d
	sha512h2	q26, q24, v27.2d
	# Load next 8 64-bit words of K
	ld1	{v16.2d, v17.2d, v18.2d, v19.2d}, [x3], #0x40
	# Round 32
	sha512su0	v0.2d, v1.2d
	ext	v21.16b, v4.16b, v5.16b, #8
	sha512su1	v0.2d, v7.2d, v21.2d
	add	v20.2d, v0.2d, v16.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v25.16b, v23.16b, #8
	ext	v22.16b, v27.16b, v25.16b, #8
	add	v23.2d, v23.2d, v20.2d
	sha512h	q23, q21, v22.2d
	add	v24.2d, v27.2d, v23.2d
	sha512h2	q23, q27, v26.2d
	# Round 33
	sha512su0	v1.2d, v2.2d
	ext	v21.16b, v5.16b, v6.16b, #8
	sha512su1	v1.2d, v0.2d, v21.2d
	add	v20.2d, v1.2d, v17.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v24.16b, v25.16b, #8
	ext	v22.16b, v26.16b, v24.16b, #8
	add	v25.2d, v25.2d, v20.2d
	sha512h	q25, q21, v22.2d
	add	v27.2d, v26.2d, v25.2d
	sha512h2	q25, q26, v23.2d
	# Round 34
	sha512su0	v2.2d, v3.2d
	ext	v21.16b, v6.16b, v7.16b, #8
	sha512su1	v2.2d, v1.2d, v21.2d
	add	v20.2d, v2.2d, v18.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v27.16b, v24.16b, #8
	ext	v22.16b, v23.16b, v27.16b, #8
	add	v24.2d, v24.2d, v20.2d
	sha512h	q24, q21, v22.2d
	add	v26.2d, v23.2d, v24.2d
	sha512h2	q24, q23, v25.2d
	# Round 35
	sha512su0	v3.2d, v4.2d
	ext	v21.16b, v7.16b, v0.16b, #8
	sha512su1	v3.2d, v2.2d, v21.2d
	add	v20.2d, v3.2d, v19.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v26.16b, v27.16b, #8
	ext	v22.16b, v25.16b, v26.16b, #8
	add	v27.2d, v27.2d, v20.2d
	sha512h	q27, q21, v22.2d
	add	v23.2d, v25.2d, v27.2d
	sha512h2	q27, q25, v24.2d
	# Load next 8 64-bit words of K
	ld1	{v16.2d, v17.2d, v18.2d, v19.2d}, [x3], #0x40
	# Round 36
	sha512su0	v4.2d, v5.2d
	ext	v21.16b, v0.16b, v1.16b, #8
	sha512su1	v4.2d, v3.2d, v21.2d
	add	v20.2d, v4.2d, v16.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v23.16b, v26.16b, #8
	ext	v22.16b, v24.16b, v23.16b, #8
	add	v26.2d, v26.2d, v20.2d
	sha512h	q26, q21, v22.2d
	add	v25.2d, v24.2d, v26.2d
	sha512h2	q26, q24, v27.2d
	# Round 37
	sha512su0	v5.2d, v6.2d
	ext	v21.16b, v1.16b, v2.16b, #8
	sha512su1	v5.2d, v4.2d, v21.2d
	add	v20.2d, v5.2d, v17.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v25.16b, v23.16b, #8
	ext	v22.16b, v27.16b, v25.16b, #8
	add	v23.2d, v23.2d, v20.2d
	sha512h	q23, q21, v22.2d
	add	v24.2d, v27.2d, v23.2d
	sha512h2	q23, q27, v26.2d
	# Round 38
	sha512su0	v6.2d, v7.2d
	ext	v21.16b, v2.16b, v3.16b, #8
	sha512su1	v6.2d, v5.2d, v21.2d
	add	v20.2d, v6.2d, v18.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v24.16b, v25.16b, #8
	ext	v22.16b, v26.16b, v24.16b, #8
	add	v25.2d, v25.2d, v20.2d
	sha512h	q25, q21, v22.2d
	add	v27.2d, v26.2d, v25.2d
	sha512h2	q25, q26, v23.2d
	# Round 39
	sha512su0	v7.2d, v0.2d
	ext	v21.16b, v3.16b, v4.16b, #8
	sha512su1	v7.2d, v6.2d, v21.2d
	add	v20.2d, v7.2d, v19.2d
	ext	v20.16b, v20.16b, v20.16b, #8
	ext	v21.16b, v27.16b, v24.16b, #8
	ext	v22.16b, v23.16b, v27.16b, #8
	add	v24.2d, v24.2d, v20.2d
	sha512h	q24, q21, v22.2d
	add	v26.2d, v23.2d, v24.2d
	sha512h2	q24, q23, v25.2d
	add	v27.2d, v27.2d, v31.2d
	add	v26.2d, v26.2d, v30.2d
	add	v25.2d, v25.2d, v29.2d
	add	v24.2d, v24.2d, v28.2d
	subs	w2, w2, #0x80
	bne	L_sha512_len_crypto_begin
	# Store digest back
	st1	{v24.2d, v25.2d, v26.2d, v27.2d}, [x0]
	ldp	d8, d9, [x29, #16]
	ldp	d10, d11, [x29, #32]
	ldp	d12, d13, [x29, #48]
	ldp	d14, d15, [x29, #64]
	ldp	x29, x30, [sp], #0x50
	ret
#ifndef __APPLE__
	.size	Transform_Sha512_Len_crypto,.-Transform_Sha512_Len_crypto
#endif /* __APPLE__ */
#endif /* WOLFSSL_ARMASM_CRYPTO_SHA512 */
#endif /* WOLFSSL_SHA512 */
#endif /* __aarch64__ */
#endif /* WOLFSSL_ARMASM */

#if defined(__linux__) && defined(__ELF__)
.section	.note.GNU-stack,"",%progbits
#endif
