/* armv8-sha3-asm
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
 *   ruby ./sha3/sha3.rb arm64 ../wolfssl/wolfcrypt/src/port/arm/armv8-sha3-asm.S
 */
#ifdef WOLFSSL_ARMASM
#ifdef __aarch64__
#ifdef WOLFSSL_SHA3
#ifdef WOLFSSL_ARMASM_CRYPTO_SHA3
#ifndef __APPLE__
	.text
	.type	L_SHA3_transform_crypto_r, %object
	.section	.rodata
	.size	L_SHA3_transform_crypto_r, 192
#else
	.section	__DATA,__data
#endif /* __APPLE__ */
#ifndef __APPLE__
	.align	3
#else
	.p2align	3
#endif /* __APPLE__ */
L_SHA3_transform_crypto_r:
	.xword	0x1
	.xword	0x8082
	.xword	0x800000000000808a
	.xword	0x8000000080008000
	.xword	0x808b
	.xword	0x80000001
	.xword	0x8000000080008081
	.xword	0x8000000000008009
	.xword	0x8a
	.xword	0x88
	.xword	0x80008009
	.xword	0x8000000a
	.xword	0x8000808b
	.xword	0x800000000000008b
	.xword	0x8000000000008089
	.xword	0x8000000000008003
	.xword	0x8000000000008002
	.xword	0x8000000000000080
	.xword	0x800a
	.xword	0x800000008000000a
	.xword	0x8000000080008081
	.xword	0x8000000000008080
	.xword	0x80000001
	.xword	0x8000000080008008
#ifndef __APPLE__
.text
.globl	BlockSha3
.type	BlockSha3,@function
.align	2
BlockSha3:
#else
.section	__TEXT,__text
.globl	_BlockSha3
.p2align	2
_BlockSha3:
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
	adrp x1, L_SHA3_transform_crypto_r
	add  x1, x1, :lo12:L_SHA3_transform_crypto_r
#else
	adrp x1, L_SHA3_transform_crypto_r@PAGE
	add  x1, x1, :lo12:L_SHA3_transform_crypto_r@PAGEOFF
#endif /* __APPLE__ */
	ld4	{v0.d, v1.d, v2.d, v3.d}[0], [x0], #32
	ld4	{v4.d, v5.d, v6.d, v7.d}[0], [x0], #32
	ld4	{v8.d, v9.d, v10.d, v11.d}[0], [x0], #32
	ld4	{v12.d, v13.d, v14.d, v15.d}[0], [x0], #32
	ld4	{v16.d, v17.d, v18.d, v19.d}[0], [x0], #32
	ld4	{v20.d, v21.d, v22.d, v23.d}[0], [x0], #32
	ld1	{v24.1d}, [x0]
	sub	x0, x0, #0xc0
	mov	x2, #24
	# Start of 24 rounds
L_sha3_crypto_begin:
	# Col Mix
	eor3	v31.16b, v0.16b, v5.16b, v10.16b
	eor3	v27.16b, v1.16b, v6.16b, v11.16b
	eor3	v28.16b, v2.16b, v7.16b, v12.16b
	eor3	v29.16b, v3.16b, v8.16b, v13.16b
	eor3	v30.16b, v4.16b, v9.16b, v14.16b
	eor3	v31.16b, v31.16b, v15.16b, v20.16b
	eor3	v27.16b, v27.16b, v16.16b, v21.16b
	eor3	v28.16b, v28.16b, v17.16b, v22.16b
	eor3	v29.16b, v29.16b, v18.16b, v23.16b
	eor3	v30.16b, v30.16b, v19.16b, v24.16b
	rax1	v25.2d, v30.2d, v27.2d
	rax1	v26.2d, v31.2d, v28.2d
	rax1	v27.2d, v27.2d, v29.2d
	rax1	v28.2d, v28.2d, v30.2d
	rax1	v29.2d, v29.2d, v31.2d
	eor	v0.16b, v0.16b, v25.16b
	xar	v30.2d, v1.2d, v26.2d, #63
	xar	v1.2d, v6.2d, v26.2d, #20
	xar	v6.2d, v9.2d, v29.2d, #44
	xar	v9.2d, v22.2d, v27.2d, #3
	xar	v22.2d, v14.2d, v29.2d, #25
	xar	v14.2d, v20.2d, v25.2d, #46
	xar	v20.2d, v2.2d, v27.2d, #2
	xar	v2.2d, v12.2d, v27.2d, #21
	xar	v12.2d, v13.2d, v28.2d, #39
	xar	v13.2d, v19.2d, v29.2d, #56
	xar	v19.2d, v23.2d, v28.2d, #8
	xar	v23.2d, v15.2d, v25.2d, #23
	xar	v15.2d, v4.2d, v29.2d, #37
	xar	v4.2d, v24.2d, v29.2d, #50
	xar	v24.2d, v21.2d, v26.2d, #62
	xar	v21.2d, v8.2d, v28.2d, #9
	xar	v8.2d, v16.2d, v26.2d, #19
	xar	v16.2d, v5.2d, v25.2d, #28
	xar	v5.2d, v3.2d, v28.2d, #36
	xar	v3.2d, v18.2d, v28.2d, #43
	xar	v18.2d, v17.2d, v27.2d, #49
	xar	v17.2d, v11.2d, v26.2d, #54
	xar	v11.2d, v7.2d, v27.2d, #58
	xar	v7.2d, v10.2d, v25.2d, #61
	# Row Mix
	mov	v25.16b, v0.16b
	mov	v26.16b, v1.16b
	bcax	v0.16b, v25.16b, v2.16b, v26.16b
	bcax	v1.16b, v26.16b, v3.16b, v2.16b
	bcax	v2.16b, v2.16b, v4.16b, v3.16b
	bcax	v3.16b, v3.16b, v25.16b, v4.16b
	bcax	v4.16b, v4.16b, v26.16b, v25.16b
	mov	v25.16b, v5.16b
	mov	v26.16b, v6.16b
	bcax	v5.16b, v25.16b, v7.16b, v26.16b
	bcax	v6.16b, v26.16b, v8.16b, v7.16b
	bcax	v7.16b, v7.16b, v9.16b, v8.16b
	bcax	v8.16b, v8.16b, v25.16b, v9.16b
	bcax	v9.16b, v9.16b, v26.16b, v25.16b
	mov	v26.16b, v11.16b
	bcax	v10.16b, v30.16b, v12.16b, v26.16b
	bcax	v11.16b, v26.16b, v13.16b, v12.16b
	bcax	v12.16b, v12.16b, v14.16b, v13.16b
	bcax	v13.16b, v13.16b, v30.16b, v14.16b
	bcax	v14.16b, v14.16b, v26.16b, v30.16b
	mov	v25.16b, v15.16b
	mov	v26.16b, v16.16b
	bcax	v15.16b, v25.16b, v17.16b, v26.16b
	bcax	v16.16b, v26.16b, v18.16b, v17.16b
	bcax	v17.16b, v17.16b, v19.16b, v18.16b
	bcax	v18.16b, v18.16b, v25.16b, v19.16b
	bcax	v19.16b, v19.16b, v26.16b, v25.16b
	mov	v25.16b, v20.16b
	mov	v26.16b, v21.16b
	bcax	v20.16b, v25.16b, v22.16b, v26.16b
	bcax	v21.16b, v26.16b, v23.16b, v22.16b
	bcax	v22.16b, v22.16b, v24.16b, v23.16b
	bcax	v23.16b, v23.16b, v25.16b, v24.16b
	bcax	v24.16b, v24.16b, v26.16b, v25.16b
	ld1r	{v30.2d}, [x1], #8
	subs	x2, x2, #1
	eor	v0.16b, v0.16b, v30.16b
	bne	L_sha3_crypto_begin
	st4	{v0.d, v1.d, v2.d, v3.d}[0], [x0], #32
	st4	{v4.d, v5.d, v6.d, v7.d}[0], [x0], #32
	st4	{v8.d, v9.d, v10.d, v11.d}[0], [x0], #32
	st4	{v12.d, v13.d, v14.d, v15.d}[0], [x0], #32
	st4	{v16.d, v17.d, v18.d, v19.d}[0], [x0], #32
	st4	{v20.d, v21.d, v22.d, v23.d}[0], [x0], #32
	st1	{v24.1d}, [x0]
	ldp	d8, d9, [x29, #16]
	ldp	d10, d11, [x29, #32]
	ldp	d12, d13, [x29, #48]
	ldp	d14, d15, [x29, #64]
	ldp	x29, x30, [sp], #0x50
	ret
#ifndef __APPLE__
	.size	BlockSha3,.-BlockSha3
#endif /* __APPLE__ */
#endif /* WOLFSSL_ARMASM_CRYPTO_SHA3 */
#endif /* WOLFSSL_SHA3 */
#endif /* __aarch64__ */
#endif /* WOLFSSL_ARMASM */

#if defined(__linux__) && defined(__ELF__)
.section	.note.GNU-stack,"",%progbits
#endif
