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
 *   ruby ./sha2/sha512.rb arm64 ../wolfssl/wolfcrypt/src/port/arm/armv8-sha512-asm.c
 */
#ifdef WOLFSSL_ARMASM
#ifdef __aarch64__
#include <wolfssl/wolfcrypt/sha512.h>

#ifdef WOLFSSL_SHA512
#ifndef WOLFSSL_ARMASM_CRYPTO_SHA512
static const uint64_t L_SHA512_transform_neon_len_k[] = {
    0x428a2f98d728ae22UL,
    0x7137449123ef65cdUL,
    0xb5c0fbcfec4d3b2fUL,
    0xe9b5dba58189dbbcUL,
    0x3956c25bf348b538UL,
    0x59f111f1b605d019UL,
    0x923f82a4af194f9bUL,
    0xab1c5ed5da6d8118UL,
    0xd807aa98a3030242UL,
    0x12835b0145706fbeUL,
    0x243185be4ee4b28cUL,
    0x550c7dc3d5ffb4e2UL,
    0x72be5d74f27b896fUL,
    0x80deb1fe3b1696b1UL,
    0x9bdc06a725c71235UL,
    0xc19bf174cf692694UL,
    0xe49b69c19ef14ad2UL,
    0xefbe4786384f25e3UL,
    0xfc19dc68b8cd5b5UL,
    0x240ca1cc77ac9c65UL,
    0x2de92c6f592b0275UL,
    0x4a7484aa6ea6e483UL,
    0x5cb0a9dcbd41fbd4UL,
    0x76f988da831153b5UL,
    0x983e5152ee66dfabUL,
    0xa831c66d2db43210UL,
    0xb00327c898fb213fUL,
    0xbf597fc7beef0ee4UL,
    0xc6e00bf33da88fc2UL,
    0xd5a79147930aa725UL,
    0x6ca6351e003826fUL,
    0x142929670a0e6e70UL,
    0x27b70a8546d22ffcUL,
    0x2e1b21385c26c926UL,
    0x4d2c6dfc5ac42aedUL,
    0x53380d139d95b3dfUL,
    0x650a73548baf63deUL,
    0x766a0abb3c77b2a8UL,
    0x81c2c92e47edaee6UL,
    0x92722c851482353bUL,
    0xa2bfe8a14cf10364UL,
    0xa81a664bbc423001UL,
    0xc24b8b70d0f89791UL,
    0xc76c51a30654be30UL,
    0xd192e819d6ef5218UL,
    0xd69906245565a910UL,
    0xf40e35855771202aUL,
    0x106aa07032bbd1b8UL,
    0x19a4c116b8d2d0c8UL,
    0x1e376c085141ab53UL,
    0x2748774cdf8eeb99UL,
    0x34b0bcb5e19b48a8UL,
    0x391c0cb3c5c95a63UL,
    0x4ed8aa4ae3418acbUL,
    0x5b9cca4f7763e373UL,
    0x682e6ff3d6b2b8a3UL,
    0x748f82ee5defb2fcUL,
    0x78a5636f43172f60UL,
    0x84c87814a1f0ab72UL,
    0x8cc702081a6439ecUL,
    0x90befffa23631e28UL,
    0xa4506cebde82bde9UL,
    0xbef9a3f7b2c67915UL,
    0xc67178f2e372532bUL,
    0xca273eceea26619cUL,
    0xd186b8c721c0c207UL,
    0xeada7dd6cde0eb1eUL,
    0xf57d4f7fee6ed178UL,
    0x6f067aa72176fbaUL,
    0xa637dc5a2c898a6UL,
    0x113f9804bef90daeUL,
    0x1b710b35131c471bUL,
    0x28db77f523047d84UL,
    0x32caab7b40c72493UL,
    0x3c9ebe0a15c9bebcUL,
    0x431d67c49c100d4cUL,
    0x4cc5d4becb3e42b6UL,
    0x597f299cfc657e2aUL,
    0x5fcb6fab3ad6faecUL,
    0x6c44198c4a475817UL,
};

static const uint64_t L_SHA512_transform_neon_len_ror8[] = {
    0x7060504030201UL,
    0x80f0e0d0c0b0a09UL,
};

void Transform_Sha512_Len_neon(wc_Sha512* sha512, const byte* data, word32 len)
{
    __asm__ __volatile__ (
#ifndef __APPLE__
        "adrp x3, %[L_SHA512_transform_neon_len_k]\n\t"
        "add  x3, x3, :lo12:%[L_SHA512_transform_neon_len_k]\n\t"
#else
        "adrp x3, %[L_SHA512_transform_neon_len_k]@PAGE\n\t"
        "add  x3, x3, %[L_SHA512_transform_neon_len_k]@PAGEOFF\n\t"
#endif /* __APPLE__ */
#ifndef __APPLE__
        "adrp x27, %[L_SHA512_transform_neon_len_ror8]\n\t"
        "add  x27, x27, :lo12:%[L_SHA512_transform_neon_len_ror8]\n\t"
#else
        "adrp x27, %[L_SHA512_transform_neon_len_ror8]@PAGE\n\t"
        "add  x27, x27, %[L_SHA512_transform_neon_len_ror8]@PAGEOFF\n\t"
#endif /* __APPLE__ */
        "ld1	{v11.16b}, [x27]\n\t"
        /* Load digest into working vars */
        "ldp	x4, x5, [%x[sha512]]\n\t"
        "ldp	x6, x7, [%x[sha512], #16]\n\t"
        "ldp	x8, x9, [%x[sha512], #32]\n\t"
        "ldp	x10, x11, [%x[sha512], #48]\n\t"
        /* Start of loop processing a block */
        "\n"
    "L_sha512_len_neon_begin_%=: \n\t"
        /* Load W */
        /* Copy digest to add in at end */
        "ld1	{v0.2d, v1.2d, v2.2d, v3.2d}, [%x[data]], #0x40\n\t"
        "mov	x19, x4\n\t"
        "ld1	{v4.2d, v5.2d, v6.2d, v7.2d}, [%x[data]], #0x40\n\t"
        "mov	x20, x5\n\t"
        "rev64	v0.16b, v0.16b\n\t"
        "mov	x21, x6\n\t"
        "rev64	v1.16b, v1.16b\n\t"
        "mov	x22, x7\n\t"
        "rev64	v2.16b, v2.16b\n\t"
        "mov	x23, x8\n\t"
        "rev64	v3.16b, v3.16b\n\t"
        "mov	x24, x9\n\t"
        "rev64	v4.16b, v4.16b\n\t"
        "mov	x25, x10\n\t"
        "rev64	v5.16b, v5.16b\n\t"
        "mov	x26, x11\n\t"
        "rev64	v6.16b, v6.16b\n\t"
        "rev64	v7.16b, v7.16b\n\t"
        /* Pre-calc: b ^ c */
        "eor	x16, x5, x6\n\t"
        "mov	x27, #4\n\t"
        /* Start of 16 rounds */
        "\n"
    "L_sha512_len_neon_start_%=: \n\t"
        /* Round 0 */
        "mov	x13, v0.d[0]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ror	x12, x8, #14\n\t"
        "ror	x14, x4, #28\n\t"
        "eor	x12, x12, x8, ror 18\n\t"
        "eor	x14, x14, x4, ror 34\n\t"
        "eor	x12, x12, x8, ror 41\n\t"
        "eor	x14, x14, x4, ror 39\n\t"
        "add	x11, x11, x12\n\t"
        "eor	x17, x4, x5\n\t"
        "eor	x12, x9, x10\n\t"
        "and	x16, x17, x16\n\t"
        "and	x12, x12, x8\n\t"
        "add	x11, x11, x13\n\t"
        "eor	x12, x12, x10\n\t"
        "add	x11, x11, x15\n\t"
        "eor	x16, x16, x5\n\t"
        "add	x11, x11, x12\n\t"
        "add	x14, x14, x16\n\t"
        "add	x7, x7, x11\n\t"
        "add	x11, x11, x14\n\t"
        /* Round 1 */
        "mov	x13, v0.d[1]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ext	v10.16b, v0.16b, v1.16b, #8\n\t"
        "ror	x12, x7, #14\n\t"
        "shl	v8.2d, v7.2d, #45\n\t"
        "ror	x14, x11, #28\n\t"
        "sri	v8.2d, v7.2d, #19\n\t"
        "eor	x12, x12, x7, ror 18\n\t"
        "shl	v9.2d, v7.2d, #3\n\t"
        "eor	x14, x14, x11, ror 34\n\t"
        "sri	v9.2d, v7.2d, #61\n\t"
        "eor	x12, x12, x7, ror 41\n\t"
        "eor	v9.16b, v9.16b, v8.16b\n\t"
        "eor	x14, x14, x11, ror 39\n\t"
        "ushr	v8.2d, v7.2d, #6\n\t"
        "add	x10, x10, x12\n\t"
        "eor	v9.16b, v9.16b, v8.16b\n\t"
        "eor	x16, x11, x4\n\t"
        "add	v0.2d, v0.2d, v9.2d\n\t"
        "eor	x12, x8, x9\n\t"
        "ext	v9.16b, v4.16b, v5.16b, #8\n\t"
        "and	x17, x16, x17\n\t"
        "add	v0.2d, v0.2d, v9.2d\n\t"
        "and	x12, x12, x7\n\t"
        "shl	v8.2d, v10.2d, #63\n\t"
        "add	x10, x10, x13\n\t"
        "sri	v8.2d, v10.2d, #1\n\t"
        "eor	x12, x12, x9\n\t"
        "tbl	v9.16b, {v10.16b}, v11.16b\n\t"
        "add	x10, x10, x15\n\t"
        "eor	v9.16b, v9.16b, v8.16b\n\t"
        "eor	x17, x17, x4\n\t"
        "ushr	v10.2d, v10.2d, #7\n\t"
        "add	x10, x10, x12\n\t"
        "eor	v9.16b, v9.16b, v10.16b\n\t"
        "add	x14, x14, x17\n\t"
        "add	v0.2d, v0.2d, v9.2d\n\t"
        "add	x6, x6, x10\n\t"
        "add	x10, x10, x14\n\t"
        /* Round 2 */
        "mov	x13, v1.d[0]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ror	x12, x6, #14\n\t"
        "ror	x14, x10, #28\n\t"
        "eor	x12, x12, x6, ror 18\n\t"
        "eor	x14, x14, x10, ror 34\n\t"
        "eor	x12, x12, x6, ror 41\n\t"
        "eor	x14, x14, x10, ror 39\n\t"
        "add	x9, x9, x12\n\t"
        "eor	x17, x10, x11\n\t"
        "eor	x12, x7, x8\n\t"
        "and	x16, x17, x16\n\t"
        "and	x12, x12, x6\n\t"
        "add	x9, x9, x13\n\t"
        "eor	x12, x12, x8\n\t"
        "add	x9, x9, x15\n\t"
        "eor	x16, x16, x11\n\t"
        "add	x9, x9, x12\n\t"
        "add	x14, x14, x16\n\t"
        "add	x5, x5, x9\n\t"
        "add	x9, x9, x14\n\t"
        /* Round 3 */
        "mov	x13, v1.d[1]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ext	v10.16b, v1.16b, v2.16b, #8\n\t"
        "ror	x12, x5, #14\n\t"
        "shl	v8.2d, v0.2d, #45\n\t"
        "ror	x14, x9, #28\n\t"
        "sri	v8.2d, v0.2d, #19\n\t"
        "eor	x12, x12, x5, ror 18\n\t"
        "shl	v9.2d, v0.2d, #3\n\t"
        "eor	x14, x14, x9, ror 34\n\t"
        "sri	v9.2d, v0.2d, #61\n\t"
        "eor	x12, x12, x5, ror 41\n\t"
        "eor	v9.16b, v9.16b, v8.16b\n\t"
        "eor	x14, x14, x9, ror 39\n\t"
        "ushr	v8.2d, v0.2d, #6\n\t"
        "add	x8, x8, x12\n\t"
        "eor	v9.16b, v9.16b, v8.16b\n\t"
        "eor	x16, x9, x10\n\t"
        "add	v1.2d, v1.2d, v9.2d\n\t"
        "eor	x12, x6, x7\n\t"
        "ext	v9.16b, v5.16b, v6.16b, #8\n\t"
        "and	x17, x16, x17\n\t"
        "add	v1.2d, v1.2d, v9.2d\n\t"
        "and	x12, x12, x5\n\t"
        "shl	v8.2d, v10.2d, #63\n\t"
        "add	x8, x8, x13\n\t"
        "sri	v8.2d, v10.2d, #1\n\t"
        "eor	x12, x12, x7\n\t"
        "tbl	v9.16b, {v10.16b}, v11.16b\n\t"
        "add	x8, x8, x15\n\t"
        "eor	v9.16b, v9.16b, v8.16b\n\t"
        "eor	x17, x17, x10\n\t"
        "ushr	v10.2d, v10.2d, #7\n\t"
        "add	x8, x8, x12\n\t"
        "eor	v9.16b, v9.16b, v10.16b\n\t"
        "add	x14, x14, x17\n\t"
        "add	v1.2d, v1.2d, v9.2d\n\t"
        "add	x4, x4, x8\n\t"
        "add	x8, x8, x14\n\t"
        /* Round 4 */
        "mov	x13, v2.d[0]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ror	x12, x4, #14\n\t"
        "ror	x14, x8, #28\n\t"
        "eor	x12, x12, x4, ror 18\n\t"
        "eor	x14, x14, x8, ror 34\n\t"
        "eor	x12, x12, x4, ror 41\n\t"
        "eor	x14, x14, x8, ror 39\n\t"
        "add	x7, x7, x12\n\t"
        "eor	x17, x8, x9\n\t"
        "eor	x12, x5, x6\n\t"
        "and	x16, x17, x16\n\t"
        "and	x12, x12, x4\n\t"
        "add	x7, x7, x13\n\t"
        "eor	x12, x12, x6\n\t"
        "add	x7, x7, x15\n\t"
        "eor	x16, x16, x9\n\t"
        "add	x7, x7, x12\n\t"
        "add	x14, x14, x16\n\t"
        "add	x11, x11, x7\n\t"
        "add	x7, x7, x14\n\t"
        /* Round 5 */
        "mov	x13, v2.d[1]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ext	v10.16b, v2.16b, v3.16b, #8\n\t"
        "ror	x12, x11, #14\n\t"
        "shl	v8.2d, v1.2d, #45\n\t"
        "ror	x14, x7, #28\n\t"
        "sri	v8.2d, v1.2d, #19\n\t"
        "eor	x12, x12, x11, ror 18\n\t"
        "shl	v9.2d, v1.2d, #3\n\t"
        "eor	x14, x14, x7, ror 34\n\t"
        "sri	v9.2d, v1.2d, #61\n\t"
        "eor	x12, x12, x11, ror 41\n\t"
        "eor	v9.16b, v9.16b, v8.16b\n\t"
        "eor	x14, x14, x7, ror 39\n\t"
        "ushr	v8.2d, v1.2d, #6\n\t"
        "add	x6, x6, x12\n\t"
        "eor	v9.16b, v9.16b, v8.16b\n\t"
        "eor	x16, x7, x8\n\t"
        "add	v2.2d, v2.2d, v9.2d\n\t"
        "eor	x12, x4, x5\n\t"
        "ext	v9.16b, v6.16b, v7.16b, #8\n\t"
        "and	x17, x16, x17\n\t"
        "add	v2.2d, v2.2d, v9.2d\n\t"
        "and	x12, x12, x11\n\t"
        "shl	v8.2d, v10.2d, #63\n\t"
        "add	x6, x6, x13\n\t"
        "sri	v8.2d, v10.2d, #1\n\t"
        "eor	x12, x12, x5\n\t"
        "tbl	v9.16b, {v10.16b}, v11.16b\n\t"
        "add	x6, x6, x15\n\t"
        "eor	v9.16b, v9.16b, v8.16b\n\t"
        "eor	x17, x17, x8\n\t"
        "ushr	v10.2d, v10.2d, #7\n\t"
        "add	x6, x6, x12\n\t"
        "eor	v9.16b, v9.16b, v10.16b\n\t"
        "add	x14, x14, x17\n\t"
        "add	v2.2d, v2.2d, v9.2d\n\t"
        "add	x10, x10, x6\n\t"
        "add	x6, x6, x14\n\t"
        /* Round 6 */
        "mov	x13, v3.d[0]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ror	x12, x10, #14\n\t"
        "ror	x14, x6, #28\n\t"
        "eor	x12, x12, x10, ror 18\n\t"
        "eor	x14, x14, x6, ror 34\n\t"
        "eor	x12, x12, x10, ror 41\n\t"
        "eor	x14, x14, x6, ror 39\n\t"
        "add	x5, x5, x12\n\t"
        "eor	x17, x6, x7\n\t"
        "eor	x12, x11, x4\n\t"
        "and	x16, x17, x16\n\t"
        "and	x12, x12, x10\n\t"
        "add	x5, x5, x13\n\t"
        "eor	x12, x12, x4\n\t"
        "add	x5, x5, x15\n\t"
        "eor	x16, x16, x7\n\t"
        "add	x5, x5, x12\n\t"
        "add	x14, x14, x16\n\t"
        "add	x9, x9, x5\n\t"
        "add	x5, x5, x14\n\t"
        /* Round 7 */
        "mov	x13, v3.d[1]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ext	v10.16b, v3.16b, v4.16b, #8\n\t"
        "ror	x12, x9, #14\n\t"
        "shl	v8.2d, v2.2d, #45\n\t"
        "ror	x14, x5, #28\n\t"
        "sri	v8.2d, v2.2d, #19\n\t"
        "eor	x12, x12, x9, ror 18\n\t"
        "shl	v9.2d, v2.2d, #3\n\t"
        "eor	x14, x14, x5, ror 34\n\t"
        "sri	v9.2d, v2.2d, #61\n\t"
        "eor	x12, x12, x9, ror 41\n\t"
        "eor	v9.16b, v9.16b, v8.16b\n\t"
        "eor	x14, x14, x5, ror 39\n\t"
        "ushr	v8.2d, v2.2d, #6\n\t"
        "add	x4, x4, x12\n\t"
        "eor	v9.16b, v9.16b, v8.16b\n\t"
        "eor	x16, x5, x6\n\t"
        "add	v3.2d, v3.2d, v9.2d\n\t"
        "eor	x12, x10, x11\n\t"
        "ext	v9.16b, v7.16b, v0.16b, #8\n\t"
        "and	x17, x16, x17\n\t"
        "add	v3.2d, v3.2d, v9.2d\n\t"
        "and	x12, x12, x9\n\t"
        "shl	v8.2d, v10.2d, #63\n\t"
        "add	x4, x4, x13\n\t"
        "sri	v8.2d, v10.2d, #1\n\t"
        "eor	x12, x12, x11\n\t"
        "tbl	v9.16b, {v10.16b}, v11.16b\n\t"
        "add	x4, x4, x15\n\t"
        "eor	v9.16b, v9.16b, v8.16b\n\t"
        "eor	x17, x17, x6\n\t"
        "ushr	v10.2d, v10.2d, #7\n\t"
        "add	x4, x4, x12\n\t"
        "eor	v9.16b, v9.16b, v10.16b\n\t"
        "add	x14, x14, x17\n\t"
        "add	v3.2d, v3.2d, v9.2d\n\t"
        "add	x8, x8, x4\n\t"
        "add	x4, x4, x14\n\t"
        /* Round 8 */
        "mov	x13, v4.d[0]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ror	x12, x8, #14\n\t"
        "ror	x14, x4, #28\n\t"
        "eor	x12, x12, x8, ror 18\n\t"
        "eor	x14, x14, x4, ror 34\n\t"
        "eor	x12, x12, x8, ror 41\n\t"
        "eor	x14, x14, x4, ror 39\n\t"
        "add	x11, x11, x12\n\t"
        "eor	x17, x4, x5\n\t"
        "eor	x12, x9, x10\n\t"
        "and	x16, x17, x16\n\t"
        "and	x12, x12, x8\n\t"
        "add	x11, x11, x13\n\t"
        "eor	x12, x12, x10\n\t"
        "add	x11, x11, x15\n\t"
        "eor	x16, x16, x5\n\t"
        "add	x11, x11, x12\n\t"
        "add	x14, x14, x16\n\t"
        "add	x7, x7, x11\n\t"
        "add	x11, x11, x14\n\t"
        /* Round 9 */
        "mov	x13, v4.d[1]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ext	v10.16b, v4.16b, v5.16b, #8\n\t"
        "ror	x12, x7, #14\n\t"
        "shl	v8.2d, v3.2d, #45\n\t"
        "ror	x14, x11, #28\n\t"
        "sri	v8.2d, v3.2d, #19\n\t"
        "eor	x12, x12, x7, ror 18\n\t"
        "shl	v9.2d, v3.2d, #3\n\t"
        "eor	x14, x14, x11, ror 34\n\t"
        "sri	v9.2d, v3.2d, #61\n\t"
        "eor	x12, x12, x7, ror 41\n\t"
        "eor	v9.16b, v9.16b, v8.16b\n\t"
        "eor	x14, x14, x11, ror 39\n\t"
        "ushr	v8.2d, v3.2d, #6\n\t"
        "add	x10, x10, x12\n\t"
        "eor	v9.16b, v9.16b, v8.16b\n\t"
        "eor	x16, x11, x4\n\t"
        "add	v4.2d, v4.2d, v9.2d\n\t"
        "eor	x12, x8, x9\n\t"
        "ext	v9.16b, v0.16b, v1.16b, #8\n\t"
        "and	x17, x16, x17\n\t"
        "add	v4.2d, v4.2d, v9.2d\n\t"
        "and	x12, x12, x7\n\t"
        "shl	v8.2d, v10.2d, #63\n\t"
        "add	x10, x10, x13\n\t"
        "sri	v8.2d, v10.2d, #1\n\t"
        "eor	x12, x12, x9\n\t"
        "tbl	v9.16b, {v10.16b}, v11.16b\n\t"
        "add	x10, x10, x15\n\t"
        "eor	v9.16b, v9.16b, v8.16b\n\t"
        "eor	x17, x17, x4\n\t"
        "ushr	v10.2d, v10.2d, #7\n\t"
        "add	x10, x10, x12\n\t"
        "eor	v9.16b, v9.16b, v10.16b\n\t"
        "add	x14, x14, x17\n\t"
        "add	v4.2d, v4.2d, v9.2d\n\t"
        "add	x6, x6, x10\n\t"
        "add	x10, x10, x14\n\t"
        /* Round 10 */
        "mov	x13, v5.d[0]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ror	x12, x6, #14\n\t"
        "ror	x14, x10, #28\n\t"
        "eor	x12, x12, x6, ror 18\n\t"
        "eor	x14, x14, x10, ror 34\n\t"
        "eor	x12, x12, x6, ror 41\n\t"
        "eor	x14, x14, x10, ror 39\n\t"
        "add	x9, x9, x12\n\t"
        "eor	x17, x10, x11\n\t"
        "eor	x12, x7, x8\n\t"
        "and	x16, x17, x16\n\t"
        "and	x12, x12, x6\n\t"
        "add	x9, x9, x13\n\t"
        "eor	x12, x12, x8\n\t"
        "add	x9, x9, x15\n\t"
        "eor	x16, x16, x11\n\t"
        "add	x9, x9, x12\n\t"
        "add	x14, x14, x16\n\t"
        "add	x5, x5, x9\n\t"
        "add	x9, x9, x14\n\t"
        /* Round 11 */
        "mov	x13, v5.d[1]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ext	v10.16b, v5.16b, v6.16b, #8\n\t"
        "ror	x12, x5, #14\n\t"
        "shl	v8.2d, v4.2d, #45\n\t"
        "ror	x14, x9, #28\n\t"
        "sri	v8.2d, v4.2d, #19\n\t"
        "eor	x12, x12, x5, ror 18\n\t"
        "shl	v9.2d, v4.2d, #3\n\t"
        "eor	x14, x14, x9, ror 34\n\t"
        "sri	v9.2d, v4.2d, #61\n\t"
        "eor	x12, x12, x5, ror 41\n\t"
        "eor	v9.16b, v9.16b, v8.16b\n\t"
        "eor	x14, x14, x9, ror 39\n\t"
        "ushr	v8.2d, v4.2d, #6\n\t"
        "add	x8, x8, x12\n\t"
        "eor	v9.16b, v9.16b, v8.16b\n\t"
        "eor	x16, x9, x10\n\t"
        "add	v5.2d, v5.2d, v9.2d\n\t"
        "eor	x12, x6, x7\n\t"
        "ext	v9.16b, v1.16b, v2.16b, #8\n\t"
        "and	x17, x16, x17\n\t"
        "add	v5.2d, v5.2d, v9.2d\n\t"
        "and	x12, x12, x5\n\t"
        "shl	v8.2d, v10.2d, #63\n\t"
        "add	x8, x8, x13\n\t"
        "sri	v8.2d, v10.2d, #1\n\t"
        "eor	x12, x12, x7\n\t"
        "tbl	v9.16b, {v10.16b}, v11.16b\n\t"
        "add	x8, x8, x15\n\t"
        "eor	v9.16b, v9.16b, v8.16b\n\t"
        "eor	x17, x17, x10\n\t"
        "ushr	v10.2d, v10.2d, #7\n\t"
        "add	x8, x8, x12\n\t"
        "eor	v9.16b, v9.16b, v10.16b\n\t"
        "add	x14, x14, x17\n\t"
        "add	v5.2d, v5.2d, v9.2d\n\t"
        "add	x4, x4, x8\n\t"
        "add	x8, x8, x14\n\t"
        /* Round 12 */
        "mov	x13, v6.d[0]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ror	x12, x4, #14\n\t"
        "ror	x14, x8, #28\n\t"
        "eor	x12, x12, x4, ror 18\n\t"
        "eor	x14, x14, x8, ror 34\n\t"
        "eor	x12, x12, x4, ror 41\n\t"
        "eor	x14, x14, x8, ror 39\n\t"
        "add	x7, x7, x12\n\t"
        "eor	x17, x8, x9\n\t"
        "eor	x12, x5, x6\n\t"
        "and	x16, x17, x16\n\t"
        "and	x12, x12, x4\n\t"
        "add	x7, x7, x13\n\t"
        "eor	x12, x12, x6\n\t"
        "add	x7, x7, x15\n\t"
        "eor	x16, x16, x9\n\t"
        "add	x7, x7, x12\n\t"
        "add	x14, x14, x16\n\t"
        "add	x11, x11, x7\n\t"
        "add	x7, x7, x14\n\t"
        /* Round 13 */
        "mov	x13, v6.d[1]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ext	v10.16b, v6.16b, v7.16b, #8\n\t"
        "ror	x12, x11, #14\n\t"
        "shl	v8.2d, v5.2d, #45\n\t"
        "ror	x14, x7, #28\n\t"
        "sri	v8.2d, v5.2d, #19\n\t"
        "eor	x12, x12, x11, ror 18\n\t"
        "shl	v9.2d, v5.2d, #3\n\t"
        "eor	x14, x14, x7, ror 34\n\t"
        "sri	v9.2d, v5.2d, #61\n\t"
        "eor	x12, x12, x11, ror 41\n\t"
        "eor	v9.16b, v9.16b, v8.16b\n\t"
        "eor	x14, x14, x7, ror 39\n\t"
        "ushr	v8.2d, v5.2d, #6\n\t"
        "add	x6, x6, x12\n\t"
        "eor	v9.16b, v9.16b, v8.16b\n\t"
        "eor	x16, x7, x8\n\t"
        "add	v6.2d, v6.2d, v9.2d\n\t"
        "eor	x12, x4, x5\n\t"
        "ext	v9.16b, v2.16b, v3.16b, #8\n\t"
        "and	x17, x16, x17\n\t"
        "add	v6.2d, v6.2d, v9.2d\n\t"
        "and	x12, x12, x11\n\t"
        "shl	v8.2d, v10.2d, #63\n\t"
        "add	x6, x6, x13\n\t"
        "sri	v8.2d, v10.2d, #1\n\t"
        "eor	x12, x12, x5\n\t"
        "tbl	v9.16b, {v10.16b}, v11.16b\n\t"
        "add	x6, x6, x15\n\t"
        "eor	v9.16b, v9.16b, v8.16b\n\t"
        "eor	x17, x17, x8\n\t"
        "ushr	v10.2d, v10.2d, #7\n\t"
        "add	x6, x6, x12\n\t"
        "eor	v9.16b, v9.16b, v10.16b\n\t"
        "add	x14, x14, x17\n\t"
        "add	v6.2d, v6.2d, v9.2d\n\t"
        "add	x10, x10, x6\n\t"
        "add	x6, x6, x14\n\t"
        /* Round 14 */
        "mov	x13, v7.d[0]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ror	x12, x10, #14\n\t"
        "ror	x14, x6, #28\n\t"
        "eor	x12, x12, x10, ror 18\n\t"
        "eor	x14, x14, x6, ror 34\n\t"
        "eor	x12, x12, x10, ror 41\n\t"
        "eor	x14, x14, x6, ror 39\n\t"
        "add	x5, x5, x12\n\t"
        "eor	x17, x6, x7\n\t"
        "eor	x12, x11, x4\n\t"
        "and	x16, x17, x16\n\t"
        "and	x12, x12, x10\n\t"
        "add	x5, x5, x13\n\t"
        "eor	x12, x12, x4\n\t"
        "add	x5, x5, x15\n\t"
        "eor	x16, x16, x7\n\t"
        "add	x5, x5, x12\n\t"
        "add	x14, x14, x16\n\t"
        "add	x9, x9, x5\n\t"
        "add	x5, x5, x14\n\t"
        /* Round 15 */
        "mov	x13, v7.d[1]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ext	v10.16b, v7.16b, v0.16b, #8\n\t"
        "ror	x12, x9, #14\n\t"
        "shl	v8.2d, v6.2d, #45\n\t"
        "ror	x14, x5, #28\n\t"
        "sri	v8.2d, v6.2d, #19\n\t"
        "eor	x12, x12, x9, ror 18\n\t"
        "shl	v9.2d, v6.2d, #3\n\t"
        "eor	x14, x14, x5, ror 34\n\t"
        "sri	v9.2d, v6.2d, #61\n\t"
        "eor	x12, x12, x9, ror 41\n\t"
        "eor	v9.16b, v9.16b, v8.16b\n\t"
        "eor	x14, x14, x5, ror 39\n\t"
        "ushr	v8.2d, v6.2d, #6\n\t"
        "add	x4, x4, x12\n\t"
        "eor	v9.16b, v9.16b, v8.16b\n\t"
        "eor	x16, x5, x6\n\t"
        "add	v7.2d, v7.2d, v9.2d\n\t"
        "eor	x12, x10, x11\n\t"
        "ext	v9.16b, v3.16b, v4.16b, #8\n\t"
        "and	x17, x16, x17\n\t"
        "add	v7.2d, v7.2d, v9.2d\n\t"
        "and	x12, x12, x9\n\t"
        "shl	v8.2d, v10.2d, #63\n\t"
        "add	x4, x4, x13\n\t"
        "sri	v8.2d, v10.2d, #1\n\t"
        "eor	x12, x12, x11\n\t"
        "tbl	v9.16b, {v10.16b}, v11.16b\n\t"
        "add	x4, x4, x15\n\t"
        "eor	v9.16b, v9.16b, v8.16b\n\t"
        "eor	x17, x17, x6\n\t"
        "ushr	v10.2d, v10.2d, #7\n\t"
        "add	x4, x4, x12\n\t"
        "eor	v9.16b, v9.16b, v10.16b\n\t"
        "add	x14, x14, x17\n\t"
        "add	v7.2d, v7.2d, v9.2d\n\t"
        "add	x8, x8, x4\n\t"
        "add	x4, x4, x14\n\t"
        "subs	x27, x27, #1\n\t"
        "bne	L_sha512_len_neon_start_%=\n\t"
        /* Round 0 */
        "mov	x13, v0.d[0]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ror	x12, x8, #14\n\t"
        "ror	x14, x4, #28\n\t"
        "eor	x12, x12, x8, ror 18\n\t"
        "eor	x14, x14, x4, ror 34\n\t"
        "eor	x12, x12, x8, ror 41\n\t"
        "eor	x14, x14, x4, ror 39\n\t"
        "add	x11, x11, x12\n\t"
        "eor	x17, x4, x5\n\t"
        "eor	x12, x9, x10\n\t"
        "and	x16, x17, x16\n\t"
        "and	x12, x12, x8\n\t"
        "add	x11, x11, x13\n\t"
        "eor	x12, x12, x10\n\t"
        "add	x11, x11, x15\n\t"
        "eor	x16, x16, x5\n\t"
        "add	x11, x11, x12\n\t"
        "add	x14, x14, x16\n\t"
        "add	x7, x7, x11\n\t"
        "add	x11, x11, x14\n\t"
        /* Round 1 */
        "mov	x13, v0.d[1]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ror	x12, x7, #14\n\t"
        "ror	x14, x11, #28\n\t"
        "eor	x12, x12, x7, ror 18\n\t"
        "eor	x14, x14, x11, ror 34\n\t"
        "eor	x12, x12, x7, ror 41\n\t"
        "eor	x14, x14, x11, ror 39\n\t"
        "add	x10, x10, x12\n\t"
        "eor	x16, x11, x4\n\t"
        "eor	x12, x8, x9\n\t"
        "and	x17, x16, x17\n\t"
        "and	x12, x12, x7\n\t"
        "add	x10, x10, x13\n\t"
        "eor	x12, x12, x9\n\t"
        "add	x10, x10, x15\n\t"
        "eor	x17, x17, x4\n\t"
        "add	x10, x10, x12\n\t"
        "add	x14, x14, x17\n\t"
        "add	x6, x6, x10\n\t"
        "add	x10, x10, x14\n\t"
        /* Round 2 */
        "mov	x13, v1.d[0]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ror	x12, x6, #14\n\t"
        "ror	x14, x10, #28\n\t"
        "eor	x12, x12, x6, ror 18\n\t"
        "eor	x14, x14, x10, ror 34\n\t"
        "eor	x12, x12, x6, ror 41\n\t"
        "eor	x14, x14, x10, ror 39\n\t"
        "add	x9, x9, x12\n\t"
        "eor	x17, x10, x11\n\t"
        "eor	x12, x7, x8\n\t"
        "and	x16, x17, x16\n\t"
        "and	x12, x12, x6\n\t"
        "add	x9, x9, x13\n\t"
        "eor	x12, x12, x8\n\t"
        "add	x9, x9, x15\n\t"
        "eor	x16, x16, x11\n\t"
        "add	x9, x9, x12\n\t"
        "add	x14, x14, x16\n\t"
        "add	x5, x5, x9\n\t"
        "add	x9, x9, x14\n\t"
        /* Round 3 */
        "mov	x13, v1.d[1]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ror	x12, x5, #14\n\t"
        "ror	x14, x9, #28\n\t"
        "eor	x12, x12, x5, ror 18\n\t"
        "eor	x14, x14, x9, ror 34\n\t"
        "eor	x12, x12, x5, ror 41\n\t"
        "eor	x14, x14, x9, ror 39\n\t"
        "add	x8, x8, x12\n\t"
        "eor	x16, x9, x10\n\t"
        "eor	x12, x6, x7\n\t"
        "and	x17, x16, x17\n\t"
        "and	x12, x12, x5\n\t"
        "add	x8, x8, x13\n\t"
        "eor	x12, x12, x7\n\t"
        "add	x8, x8, x15\n\t"
        "eor	x17, x17, x10\n\t"
        "add	x8, x8, x12\n\t"
        "add	x14, x14, x17\n\t"
        "add	x4, x4, x8\n\t"
        "add	x8, x8, x14\n\t"
        /* Round 4 */
        "mov	x13, v2.d[0]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ror	x12, x4, #14\n\t"
        "ror	x14, x8, #28\n\t"
        "eor	x12, x12, x4, ror 18\n\t"
        "eor	x14, x14, x8, ror 34\n\t"
        "eor	x12, x12, x4, ror 41\n\t"
        "eor	x14, x14, x8, ror 39\n\t"
        "add	x7, x7, x12\n\t"
        "eor	x17, x8, x9\n\t"
        "eor	x12, x5, x6\n\t"
        "and	x16, x17, x16\n\t"
        "and	x12, x12, x4\n\t"
        "add	x7, x7, x13\n\t"
        "eor	x12, x12, x6\n\t"
        "add	x7, x7, x15\n\t"
        "eor	x16, x16, x9\n\t"
        "add	x7, x7, x12\n\t"
        "add	x14, x14, x16\n\t"
        "add	x11, x11, x7\n\t"
        "add	x7, x7, x14\n\t"
        /* Round 5 */
        "mov	x13, v2.d[1]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ror	x12, x11, #14\n\t"
        "ror	x14, x7, #28\n\t"
        "eor	x12, x12, x11, ror 18\n\t"
        "eor	x14, x14, x7, ror 34\n\t"
        "eor	x12, x12, x11, ror 41\n\t"
        "eor	x14, x14, x7, ror 39\n\t"
        "add	x6, x6, x12\n\t"
        "eor	x16, x7, x8\n\t"
        "eor	x12, x4, x5\n\t"
        "and	x17, x16, x17\n\t"
        "and	x12, x12, x11\n\t"
        "add	x6, x6, x13\n\t"
        "eor	x12, x12, x5\n\t"
        "add	x6, x6, x15\n\t"
        "eor	x17, x17, x8\n\t"
        "add	x6, x6, x12\n\t"
        "add	x14, x14, x17\n\t"
        "add	x10, x10, x6\n\t"
        "add	x6, x6, x14\n\t"
        /* Round 6 */
        "mov	x13, v3.d[0]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ror	x12, x10, #14\n\t"
        "ror	x14, x6, #28\n\t"
        "eor	x12, x12, x10, ror 18\n\t"
        "eor	x14, x14, x6, ror 34\n\t"
        "eor	x12, x12, x10, ror 41\n\t"
        "eor	x14, x14, x6, ror 39\n\t"
        "add	x5, x5, x12\n\t"
        "eor	x17, x6, x7\n\t"
        "eor	x12, x11, x4\n\t"
        "and	x16, x17, x16\n\t"
        "and	x12, x12, x10\n\t"
        "add	x5, x5, x13\n\t"
        "eor	x12, x12, x4\n\t"
        "add	x5, x5, x15\n\t"
        "eor	x16, x16, x7\n\t"
        "add	x5, x5, x12\n\t"
        "add	x14, x14, x16\n\t"
        "add	x9, x9, x5\n\t"
        "add	x5, x5, x14\n\t"
        /* Round 7 */
        "mov	x13, v3.d[1]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ror	x12, x9, #14\n\t"
        "ror	x14, x5, #28\n\t"
        "eor	x12, x12, x9, ror 18\n\t"
        "eor	x14, x14, x5, ror 34\n\t"
        "eor	x12, x12, x9, ror 41\n\t"
        "eor	x14, x14, x5, ror 39\n\t"
        "add	x4, x4, x12\n\t"
        "eor	x16, x5, x6\n\t"
        "eor	x12, x10, x11\n\t"
        "and	x17, x16, x17\n\t"
        "and	x12, x12, x9\n\t"
        "add	x4, x4, x13\n\t"
        "eor	x12, x12, x11\n\t"
        "add	x4, x4, x15\n\t"
        "eor	x17, x17, x6\n\t"
        "add	x4, x4, x12\n\t"
        "add	x14, x14, x17\n\t"
        "add	x8, x8, x4\n\t"
        "add	x4, x4, x14\n\t"
        /* Round 8 */
        "mov	x13, v4.d[0]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ror	x12, x8, #14\n\t"
        "ror	x14, x4, #28\n\t"
        "eor	x12, x12, x8, ror 18\n\t"
        "eor	x14, x14, x4, ror 34\n\t"
        "eor	x12, x12, x8, ror 41\n\t"
        "eor	x14, x14, x4, ror 39\n\t"
        "add	x11, x11, x12\n\t"
        "eor	x17, x4, x5\n\t"
        "eor	x12, x9, x10\n\t"
        "and	x16, x17, x16\n\t"
        "and	x12, x12, x8\n\t"
        "add	x11, x11, x13\n\t"
        "eor	x12, x12, x10\n\t"
        "add	x11, x11, x15\n\t"
        "eor	x16, x16, x5\n\t"
        "add	x11, x11, x12\n\t"
        "add	x14, x14, x16\n\t"
        "add	x7, x7, x11\n\t"
        "add	x11, x11, x14\n\t"
        /* Round 9 */
        "mov	x13, v4.d[1]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ror	x12, x7, #14\n\t"
        "ror	x14, x11, #28\n\t"
        "eor	x12, x12, x7, ror 18\n\t"
        "eor	x14, x14, x11, ror 34\n\t"
        "eor	x12, x12, x7, ror 41\n\t"
        "eor	x14, x14, x11, ror 39\n\t"
        "add	x10, x10, x12\n\t"
        "eor	x16, x11, x4\n\t"
        "eor	x12, x8, x9\n\t"
        "and	x17, x16, x17\n\t"
        "and	x12, x12, x7\n\t"
        "add	x10, x10, x13\n\t"
        "eor	x12, x12, x9\n\t"
        "add	x10, x10, x15\n\t"
        "eor	x17, x17, x4\n\t"
        "add	x10, x10, x12\n\t"
        "add	x14, x14, x17\n\t"
        "add	x6, x6, x10\n\t"
        "add	x10, x10, x14\n\t"
        /* Round 10 */
        "mov	x13, v5.d[0]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ror	x12, x6, #14\n\t"
        "ror	x14, x10, #28\n\t"
        "eor	x12, x12, x6, ror 18\n\t"
        "eor	x14, x14, x10, ror 34\n\t"
        "eor	x12, x12, x6, ror 41\n\t"
        "eor	x14, x14, x10, ror 39\n\t"
        "add	x9, x9, x12\n\t"
        "eor	x17, x10, x11\n\t"
        "eor	x12, x7, x8\n\t"
        "and	x16, x17, x16\n\t"
        "and	x12, x12, x6\n\t"
        "add	x9, x9, x13\n\t"
        "eor	x12, x12, x8\n\t"
        "add	x9, x9, x15\n\t"
        "eor	x16, x16, x11\n\t"
        "add	x9, x9, x12\n\t"
        "add	x14, x14, x16\n\t"
        "add	x5, x5, x9\n\t"
        "add	x9, x9, x14\n\t"
        /* Round 11 */
        "mov	x13, v5.d[1]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ror	x12, x5, #14\n\t"
        "ror	x14, x9, #28\n\t"
        "eor	x12, x12, x5, ror 18\n\t"
        "eor	x14, x14, x9, ror 34\n\t"
        "eor	x12, x12, x5, ror 41\n\t"
        "eor	x14, x14, x9, ror 39\n\t"
        "add	x8, x8, x12\n\t"
        "eor	x16, x9, x10\n\t"
        "eor	x12, x6, x7\n\t"
        "and	x17, x16, x17\n\t"
        "and	x12, x12, x5\n\t"
        "add	x8, x8, x13\n\t"
        "eor	x12, x12, x7\n\t"
        "add	x8, x8, x15\n\t"
        "eor	x17, x17, x10\n\t"
        "add	x8, x8, x12\n\t"
        "add	x14, x14, x17\n\t"
        "add	x4, x4, x8\n\t"
        "add	x8, x8, x14\n\t"
        /* Round 12 */
        "mov	x13, v6.d[0]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ror	x12, x4, #14\n\t"
        "ror	x14, x8, #28\n\t"
        "eor	x12, x12, x4, ror 18\n\t"
        "eor	x14, x14, x8, ror 34\n\t"
        "eor	x12, x12, x4, ror 41\n\t"
        "eor	x14, x14, x8, ror 39\n\t"
        "add	x7, x7, x12\n\t"
        "eor	x17, x8, x9\n\t"
        "eor	x12, x5, x6\n\t"
        "and	x16, x17, x16\n\t"
        "and	x12, x12, x4\n\t"
        "add	x7, x7, x13\n\t"
        "eor	x12, x12, x6\n\t"
        "add	x7, x7, x15\n\t"
        "eor	x16, x16, x9\n\t"
        "add	x7, x7, x12\n\t"
        "add	x14, x14, x16\n\t"
        "add	x11, x11, x7\n\t"
        "add	x7, x7, x14\n\t"
        /* Round 13 */
        "mov	x13, v6.d[1]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ror	x12, x11, #14\n\t"
        "ror	x14, x7, #28\n\t"
        "eor	x12, x12, x11, ror 18\n\t"
        "eor	x14, x14, x7, ror 34\n\t"
        "eor	x12, x12, x11, ror 41\n\t"
        "eor	x14, x14, x7, ror 39\n\t"
        "add	x6, x6, x12\n\t"
        "eor	x16, x7, x8\n\t"
        "eor	x12, x4, x5\n\t"
        "and	x17, x16, x17\n\t"
        "and	x12, x12, x11\n\t"
        "add	x6, x6, x13\n\t"
        "eor	x12, x12, x5\n\t"
        "add	x6, x6, x15\n\t"
        "eor	x17, x17, x8\n\t"
        "add	x6, x6, x12\n\t"
        "add	x14, x14, x17\n\t"
        "add	x10, x10, x6\n\t"
        "add	x6, x6, x14\n\t"
        /* Round 14 */
        "mov	x13, v7.d[0]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ror	x12, x10, #14\n\t"
        "ror	x14, x6, #28\n\t"
        "eor	x12, x12, x10, ror 18\n\t"
        "eor	x14, x14, x6, ror 34\n\t"
        "eor	x12, x12, x10, ror 41\n\t"
        "eor	x14, x14, x6, ror 39\n\t"
        "add	x5, x5, x12\n\t"
        "eor	x17, x6, x7\n\t"
        "eor	x12, x11, x4\n\t"
        "and	x16, x17, x16\n\t"
        "and	x12, x12, x10\n\t"
        "add	x5, x5, x13\n\t"
        "eor	x12, x12, x4\n\t"
        "add	x5, x5, x15\n\t"
        "eor	x16, x16, x7\n\t"
        "add	x5, x5, x12\n\t"
        "add	x14, x14, x16\n\t"
        "add	x9, x9, x5\n\t"
        "add	x5, x5, x14\n\t"
        /* Round 15 */
        "mov	x13, v7.d[1]\n\t"
        "ldr	x15, [x3], #8\n\t"
        "ror	x12, x9, #14\n\t"
        "ror	x14, x5, #28\n\t"
        "eor	x12, x12, x9, ror 18\n\t"
        "eor	x14, x14, x5, ror 34\n\t"
        "eor	x12, x12, x9, ror 41\n\t"
        "eor	x14, x14, x5, ror 39\n\t"
        "add	x4, x4, x12\n\t"
        "eor	x16, x5, x6\n\t"
        "eor	x12, x10, x11\n\t"
        "and	x17, x16, x17\n\t"
        "and	x12, x12, x9\n\t"
        "add	x4, x4, x13\n\t"
        "eor	x12, x12, x11\n\t"
        "add	x4, x4, x15\n\t"
        "eor	x17, x17, x6\n\t"
        "add	x4, x4, x12\n\t"
        "add	x14, x14, x17\n\t"
        "add	x8, x8, x4\n\t"
        "add	x4, x4, x14\n\t"
        "add	x11, x11, x26\n\t"
        "add	x10, x10, x25\n\t"
        "add	x9, x9, x24\n\t"
        "add	x8, x8, x23\n\t"
        "add	x7, x7, x22\n\t"
        "add	x6, x6, x21\n\t"
        "add	x5, x5, x20\n\t"
        "add	x4, x4, x19\n\t"
#ifndef __APPLE__
        "adrp x3, %[L_SHA512_transform_neon_len_k]\n\t"
        "add  x3, x3, :lo12:%[L_SHA512_transform_neon_len_k]\n\t"
#else
        "adrp x3, %[L_SHA512_transform_neon_len_k]@PAGE\n\t"
        "add  x3, x3, %[L_SHA512_transform_neon_len_k]@PAGEOFF\n\t"
#endif /* __APPLE__ */
        "subs	%w[len], %w[len], #0x80\n\t"
        "bne	L_sha512_len_neon_begin_%=\n\t"
        "stp	x4, x5, [%x[sha512]]\n\t"
        "stp	x6, x7, [%x[sha512], #16]\n\t"
        "stp	x8, x9, [%x[sha512], #32]\n\t"
        "stp	x10, x11, [%x[sha512], #48]\n\t"
        : [sha512] "+r" (sha512), [data] "+r" (data), [len] "+r" (len)
        : [L_SHA512_transform_neon_len_k] "S" (L_SHA512_transform_neon_len_k), [L_SHA512_transform_neon_len_ror8] "S" (L_SHA512_transform_neon_len_ror8)
        : "memory", "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "x19", "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8", "v9", "v10", "v11"
    );
}

#else
static const uint64_t L_SHA512_transform_crypto_len_k[] = {
    0x428a2f98d728ae22UL,
    0x7137449123ef65cdUL,
    0xb5c0fbcfec4d3b2fUL,
    0xe9b5dba58189dbbcUL,
    0x3956c25bf348b538UL,
    0x59f111f1b605d019UL,
    0x923f82a4af194f9bUL,
    0xab1c5ed5da6d8118UL,
    0xd807aa98a3030242UL,
    0x12835b0145706fbeUL,
    0x243185be4ee4b28cUL,
    0x550c7dc3d5ffb4e2UL,
    0x72be5d74f27b896fUL,
    0x80deb1fe3b1696b1UL,
    0x9bdc06a725c71235UL,
    0xc19bf174cf692694UL,
    0xe49b69c19ef14ad2UL,
    0xefbe4786384f25e3UL,
    0xfc19dc68b8cd5b5UL,
    0x240ca1cc77ac9c65UL,
    0x2de92c6f592b0275UL,
    0x4a7484aa6ea6e483UL,
    0x5cb0a9dcbd41fbd4UL,
    0x76f988da831153b5UL,
    0x983e5152ee66dfabUL,
    0xa831c66d2db43210UL,
    0xb00327c898fb213fUL,
    0xbf597fc7beef0ee4UL,
    0xc6e00bf33da88fc2UL,
    0xd5a79147930aa725UL,
    0x6ca6351e003826fUL,
    0x142929670a0e6e70UL,
    0x27b70a8546d22ffcUL,
    0x2e1b21385c26c926UL,
    0x4d2c6dfc5ac42aedUL,
    0x53380d139d95b3dfUL,
    0x650a73548baf63deUL,
    0x766a0abb3c77b2a8UL,
    0x81c2c92e47edaee6UL,
    0x92722c851482353bUL,
    0xa2bfe8a14cf10364UL,
    0xa81a664bbc423001UL,
    0xc24b8b70d0f89791UL,
    0xc76c51a30654be30UL,
    0xd192e819d6ef5218UL,
    0xd69906245565a910UL,
    0xf40e35855771202aUL,
    0x106aa07032bbd1b8UL,
    0x19a4c116b8d2d0c8UL,
    0x1e376c085141ab53UL,
    0x2748774cdf8eeb99UL,
    0x34b0bcb5e19b48a8UL,
    0x391c0cb3c5c95a63UL,
    0x4ed8aa4ae3418acbUL,
    0x5b9cca4f7763e373UL,
    0x682e6ff3d6b2b8a3UL,
    0x748f82ee5defb2fcUL,
    0x78a5636f43172f60UL,
    0x84c87814a1f0ab72UL,
    0x8cc702081a6439ecUL,
    0x90befffa23631e28UL,
    0xa4506cebde82bde9UL,
    0xbef9a3f7b2c67915UL,
    0xc67178f2e372532bUL,
    0xca273eceea26619cUL,
    0xd186b8c721c0c207UL,
    0xeada7dd6cde0eb1eUL,
    0xf57d4f7fee6ed178UL,
    0x6f067aa72176fbaUL,
    0xa637dc5a2c898a6UL,
    0x113f9804bef90daeUL,
    0x1b710b35131c471bUL,
    0x28db77f523047d84UL,
    0x32caab7b40c72493UL,
    0x3c9ebe0a15c9bebcUL,
    0x431d67c49c100d4cUL,
    0x4cc5d4becb3e42b6UL,
    0x597f299cfc657e2aUL,
    0x5fcb6fab3ad6faecUL,
    0x6c44198c4a475817UL,
};

void Transform_Sha512_Len_crypto(wc_Sha512* sha512, const byte* data, word32 len);
void Transform_Sha512_Len_crypto(wc_Sha512* sha512, const byte* data, word32 len)
{
    __asm__ __volatile__ (
#ifdef __APPLE__
    ".arch_extension sha3\n\t"
#endif /* __APPLE__ */
#ifndef __APPLE__
        "adrp x4, %[L_SHA512_transform_crypto_len_k]\n\t"
        "add  x4, x4, :lo12:%[L_SHA512_transform_crypto_len_k]\n\t"
#else
        "adrp x4, %[L_SHA512_transform_crypto_len_k]@PAGE\n\t"
        "add  x4, x4, %[L_SHA512_transform_crypto_len_k]@PAGEOFF\n\t"
#endif /* __APPLE__ */
        /* Load first 16 64-bit words of K permanently */
        "ld1	{v8.2d, v9.2d, v10.2d, v11.2d}, [x4], #0x40\n\t"
        "ld1	{v12.2d, v13.2d, v14.2d, v15.2d}, [x4], #0x40\n\t"
        /* Load digest into working vars */
        "ld1	{v24.2d, v25.2d, v26.2d, v27.2d}, [%x[sha512]]\n\t"
        /* Start of loop processing a block */
        "\n"
    "L_sha512_len_crypto_begin_%=: \n\t"
        "mov	x3, x4\n\t"
        /* Load W */
        "ld1	{v0.2d, v1.2d, v2.2d, v3.2d}, [%x[data]], #0x40\n\t"
        "ld1	{v4.2d, v5.2d, v6.2d, v7.2d}, [%x[data]], #0x40\n\t"
        "rev64	v0.16b, v0.16b\n\t"
        "rev64	v1.16b, v1.16b\n\t"
        "rev64	v2.16b, v2.16b\n\t"
        "rev64	v3.16b, v3.16b\n\t"
        "rev64	v4.16b, v4.16b\n\t"
        "rev64	v5.16b, v5.16b\n\t"
        "rev64	v6.16b, v6.16b\n\t"
        "rev64	v7.16b, v7.16b\n\t"
        /* Copy digest to add in at end */
        "mov	v28.16b, v24.16b\n\t"
        "mov	v29.16b, v25.16b\n\t"
        "mov	v30.16b, v26.16b\n\t"
        "mov	v31.16b, v27.16b\n\t"
        /* Start of 16 rounds */
        /* Round 0 */
        "add	v20.2d, v0.2d, v8.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v26.16b, v27.16b, #8\n\t"
        "ext	v22.16b, v25.16b, v26.16b, #8\n\t"
        "add	v27.2d, v27.2d, v20.2d\n\t"
        "sha512h	q27, q21, v22.2d\n\t"
        "add	v23.2d, v25.2d, v27.2d\n\t"
        "sha512h2	q27, q25, v24.2d\n\t"
        /* Round 1 */
        "add	v20.2d, v1.2d, v9.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v23.16b, v26.16b, #8\n\t"
        "ext	v22.16b, v24.16b, v23.16b, #8\n\t"
        "add	v26.2d, v26.2d, v20.2d\n\t"
        "sha512h	q26, q21, v22.2d\n\t"
        "add	v25.2d, v24.2d, v26.2d\n\t"
        "sha512h2	q26, q24, v27.2d\n\t"
        /* Round 2 */
        "add	v20.2d, v2.2d, v10.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v25.16b, v23.16b, #8\n\t"
        "ext	v22.16b, v27.16b, v25.16b, #8\n\t"
        "add	v23.2d, v23.2d, v20.2d\n\t"
        "sha512h	q23, q21, v22.2d\n\t"
        "add	v24.2d, v27.2d, v23.2d\n\t"
        "sha512h2	q23, q27, v26.2d\n\t"
        /* Round 3 */
        "add	v20.2d, v3.2d, v11.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v24.16b, v25.16b, #8\n\t"
        "ext	v22.16b, v26.16b, v24.16b, #8\n\t"
        "add	v25.2d, v25.2d, v20.2d\n\t"
        "sha512h	q25, q21, v22.2d\n\t"
        "add	v27.2d, v26.2d, v25.2d\n\t"
        "sha512h2	q25, q26, v23.2d\n\t"
        /* Round 4 */
        "add	v20.2d, v4.2d, v12.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v27.16b, v24.16b, #8\n\t"
        "ext	v22.16b, v23.16b, v27.16b, #8\n\t"
        "add	v24.2d, v24.2d, v20.2d\n\t"
        "sha512h	q24, q21, v22.2d\n\t"
        "add	v26.2d, v23.2d, v24.2d\n\t"
        "sha512h2	q24, q23, v25.2d\n\t"
        /* Round 5 */
        "add	v20.2d, v5.2d, v13.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v26.16b, v27.16b, #8\n\t"
        "ext	v22.16b, v25.16b, v26.16b, #8\n\t"
        "add	v27.2d, v27.2d, v20.2d\n\t"
        "sha512h	q27, q21, v22.2d\n\t"
        "add	v23.2d, v25.2d, v27.2d\n\t"
        "sha512h2	q27, q25, v24.2d\n\t"
        /* Round 6 */
        "add	v20.2d, v6.2d, v14.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v23.16b, v26.16b, #8\n\t"
        "ext	v22.16b, v24.16b, v23.16b, #8\n\t"
        "add	v26.2d, v26.2d, v20.2d\n\t"
        "sha512h	q26, q21, v22.2d\n\t"
        "add	v25.2d, v24.2d, v26.2d\n\t"
        "sha512h2	q26, q24, v27.2d\n\t"
        /* Round 7 */
        "add	v20.2d, v7.2d, v15.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v25.16b, v23.16b, #8\n\t"
        "ext	v22.16b, v27.16b, v25.16b, #8\n\t"
        "add	v23.2d, v23.2d, v20.2d\n\t"
        "sha512h	q23, q21, v22.2d\n\t"
        "add	v24.2d, v27.2d, v23.2d\n\t"
        "sha512h2	q23, q27, v26.2d\n\t"
        /* Load next 8 64-bit words of K */
        "ld1	{v16.2d, v17.2d, v18.2d, v19.2d}, [x3], #0x40\n\t"
        /* Round 8 */
        "sha512su0	v0.2d, v1.2d\n\t"
        "ext	v21.16b, v4.16b, v5.16b, #8\n\t"
        "sha512su1	v0.2d, v7.2d, v21.2d\n\t"
        "add	v20.2d, v0.2d, v16.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v24.16b, v25.16b, #8\n\t"
        "ext	v22.16b, v26.16b, v24.16b, #8\n\t"
        "add	v25.2d, v25.2d, v20.2d\n\t"
        "sha512h	q25, q21, v22.2d\n\t"
        "add	v27.2d, v26.2d, v25.2d\n\t"
        "sha512h2	q25, q26, v23.2d\n\t"
        /* Round 9 */
        "sha512su0	v1.2d, v2.2d\n\t"
        "ext	v21.16b, v5.16b, v6.16b, #8\n\t"
        "sha512su1	v1.2d, v0.2d, v21.2d\n\t"
        "add	v20.2d, v1.2d, v17.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v27.16b, v24.16b, #8\n\t"
        "ext	v22.16b, v23.16b, v27.16b, #8\n\t"
        "add	v24.2d, v24.2d, v20.2d\n\t"
        "sha512h	q24, q21, v22.2d\n\t"
        "add	v26.2d, v23.2d, v24.2d\n\t"
        "sha512h2	q24, q23, v25.2d\n\t"
        /* Round 10 */
        "sha512su0	v2.2d, v3.2d\n\t"
        "ext	v21.16b, v6.16b, v7.16b, #8\n\t"
        "sha512su1	v2.2d, v1.2d, v21.2d\n\t"
        "add	v20.2d, v2.2d, v18.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v26.16b, v27.16b, #8\n\t"
        "ext	v22.16b, v25.16b, v26.16b, #8\n\t"
        "add	v27.2d, v27.2d, v20.2d\n\t"
        "sha512h	q27, q21, v22.2d\n\t"
        "add	v23.2d, v25.2d, v27.2d\n\t"
        "sha512h2	q27, q25, v24.2d\n\t"
        /* Round 11 */
        "sha512su0	v3.2d, v4.2d\n\t"
        "ext	v21.16b, v7.16b, v0.16b, #8\n\t"
        "sha512su1	v3.2d, v2.2d, v21.2d\n\t"
        "add	v20.2d, v3.2d, v19.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v23.16b, v26.16b, #8\n\t"
        "ext	v22.16b, v24.16b, v23.16b, #8\n\t"
        "add	v26.2d, v26.2d, v20.2d\n\t"
        "sha512h	q26, q21, v22.2d\n\t"
        "add	v25.2d, v24.2d, v26.2d\n\t"
        "sha512h2	q26, q24, v27.2d\n\t"
        /* Load next 8 64-bit words of K */
        "ld1	{v16.2d, v17.2d, v18.2d, v19.2d}, [x3], #0x40\n\t"
        /* Round 12 */
        "sha512su0	v4.2d, v5.2d\n\t"
        "ext	v21.16b, v0.16b, v1.16b, #8\n\t"
        "sha512su1	v4.2d, v3.2d, v21.2d\n\t"
        "add	v20.2d, v4.2d, v16.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v25.16b, v23.16b, #8\n\t"
        "ext	v22.16b, v27.16b, v25.16b, #8\n\t"
        "add	v23.2d, v23.2d, v20.2d\n\t"
        "sha512h	q23, q21, v22.2d\n\t"
        "add	v24.2d, v27.2d, v23.2d\n\t"
        "sha512h2	q23, q27, v26.2d\n\t"
        /* Round 13 */
        "sha512su0	v5.2d, v6.2d\n\t"
        "ext	v21.16b, v1.16b, v2.16b, #8\n\t"
        "sha512su1	v5.2d, v4.2d, v21.2d\n\t"
        "add	v20.2d, v5.2d, v17.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v24.16b, v25.16b, #8\n\t"
        "ext	v22.16b, v26.16b, v24.16b, #8\n\t"
        "add	v25.2d, v25.2d, v20.2d\n\t"
        "sha512h	q25, q21, v22.2d\n\t"
        "add	v27.2d, v26.2d, v25.2d\n\t"
        "sha512h2	q25, q26, v23.2d\n\t"
        /* Round 14 */
        "sha512su0	v6.2d, v7.2d\n\t"
        "ext	v21.16b, v2.16b, v3.16b, #8\n\t"
        "sha512su1	v6.2d, v5.2d, v21.2d\n\t"
        "add	v20.2d, v6.2d, v18.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v27.16b, v24.16b, #8\n\t"
        "ext	v22.16b, v23.16b, v27.16b, #8\n\t"
        "add	v24.2d, v24.2d, v20.2d\n\t"
        "sha512h	q24, q21, v22.2d\n\t"
        "add	v26.2d, v23.2d, v24.2d\n\t"
        "sha512h2	q24, q23, v25.2d\n\t"
        /* Round 15 */
        "sha512su0	v7.2d, v0.2d\n\t"
        "ext	v21.16b, v3.16b, v4.16b, #8\n\t"
        "sha512su1	v7.2d, v6.2d, v21.2d\n\t"
        "add	v20.2d, v7.2d, v19.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v26.16b, v27.16b, #8\n\t"
        "ext	v22.16b, v25.16b, v26.16b, #8\n\t"
        "add	v27.2d, v27.2d, v20.2d\n\t"
        "sha512h	q27, q21, v22.2d\n\t"
        "add	v23.2d, v25.2d, v27.2d\n\t"
        "sha512h2	q27, q25, v24.2d\n\t"
        /* Load next 8 64-bit words of K */
        "ld1	{v16.2d, v17.2d, v18.2d, v19.2d}, [x3], #0x40\n\t"
        /* Round 16 */
        "sha512su0	v0.2d, v1.2d\n\t"
        "ext	v21.16b, v4.16b, v5.16b, #8\n\t"
        "sha512su1	v0.2d, v7.2d, v21.2d\n\t"
        "add	v20.2d, v0.2d, v16.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v23.16b, v26.16b, #8\n\t"
        "ext	v22.16b, v24.16b, v23.16b, #8\n\t"
        "add	v26.2d, v26.2d, v20.2d\n\t"
        "sha512h	q26, q21, v22.2d\n\t"
        "add	v25.2d, v24.2d, v26.2d\n\t"
        "sha512h2	q26, q24, v27.2d\n\t"
        /* Round 17 */
        "sha512su0	v1.2d, v2.2d\n\t"
        "ext	v21.16b, v5.16b, v6.16b, #8\n\t"
        "sha512su1	v1.2d, v0.2d, v21.2d\n\t"
        "add	v20.2d, v1.2d, v17.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v25.16b, v23.16b, #8\n\t"
        "ext	v22.16b, v27.16b, v25.16b, #8\n\t"
        "add	v23.2d, v23.2d, v20.2d\n\t"
        "sha512h	q23, q21, v22.2d\n\t"
        "add	v24.2d, v27.2d, v23.2d\n\t"
        "sha512h2	q23, q27, v26.2d\n\t"
        /* Round 18 */
        "sha512su0	v2.2d, v3.2d\n\t"
        "ext	v21.16b, v6.16b, v7.16b, #8\n\t"
        "sha512su1	v2.2d, v1.2d, v21.2d\n\t"
        "add	v20.2d, v2.2d, v18.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v24.16b, v25.16b, #8\n\t"
        "ext	v22.16b, v26.16b, v24.16b, #8\n\t"
        "add	v25.2d, v25.2d, v20.2d\n\t"
        "sha512h	q25, q21, v22.2d\n\t"
        "add	v27.2d, v26.2d, v25.2d\n\t"
        "sha512h2	q25, q26, v23.2d\n\t"
        /* Round 19 */
        "sha512su0	v3.2d, v4.2d\n\t"
        "ext	v21.16b, v7.16b, v0.16b, #8\n\t"
        "sha512su1	v3.2d, v2.2d, v21.2d\n\t"
        "add	v20.2d, v3.2d, v19.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v27.16b, v24.16b, #8\n\t"
        "ext	v22.16b, v23.16b, v27.16b, #8\n\t"
        "add	v24.2d, v24.2d, v20.2d\n\t"
        "sha512h	q24, q21, v22.2d\n\t"
        "add	v26.2d, v23.2d, v24.2d\n\t"
        "sha512h2	q24, q23, v25.2d\n\t"
        /* Load next 8 64-bit words of K */
        "ld1	{v16.2d, v17.2d, v18.2d, v19.2d}, [x3], #0x40\n\t"
        /* Round 20 */
        "sha512su0	v4.2d, v5.2d\n\t"
        "ext	v21.16b, v0.16b, v1.16b, #8\n\t"
        "sha512su1	v4.2d, v3.2d, v21.2d\n\t"
        "add	v20.2d, v4.2d, v16.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v26.16b, v27.16b, #8\n\t"
        "ext	v22.16b, v25.16b, v26.16b, #8\n\t"
        "add	v27.2d, v27.2d, v20.2d\n\t"
        "sha512h	q27, q21, v22.2d\n\t"
        "add	v23.2d, v25.2d, v27.2d\n\t"
        "sha512h2	q27, q25, v24.2d\n\t"
        /* Round 21 */
        "sha512su0	v5.2d, v6.2d\n\t"
        "ext	v21.16b, v1.16b, v2.16b, #8\n\t"
        "sha512su1	v5.2d, v4.2d, v21.2d\n\t"
        "add	v20.2d, v5.2d, v17.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v23.16b, v26.16b, #8\n\t"
        "ext	v22.16b, v24.16b, v23.16b, #8\n\t"
        "add	v26.2d, v26.2d, v20.2d\n\t"
        "sha512h	q26, q21, v22.2d\n\t"
        "add	v25.2d, v24.2d, v26.2d\n\t"
        "sha512h2	q26, q24, v27.2d\n\t"
        /* Round 22 */
        "sha512su0	v6.2d, v7.2d\n\t"
        "ext	v21.16b, v2.16b, v3.16b, #8\n\t"
        "sha512su1	v6.2d, v5.2d, v21.2d\n\t"
        "add	v20.2d, v6.2d, v18.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v25.16b, v23.16b, #8\n\t"
        "ext	v22.16b, v27.16b, v25.16b, #8\n\t"
        "add	v23.2d, v23.2d, v20.2d\n\t"
        "sha512h	q23, q21, v22.2d\n\t"
        "add	v24.2d, v27.2d, v23.2d\n\t"
        "sha512h2	q23, q27, v26.2d\n\t"
        /* Round 23 */
        "sha512su0	v7.2d, v0.2d\n\t"
        "ext	v21.16b, v3.16b, v4.16b, #8\n\t"
        "sha512su1	v7.2d, v6.2d, v21.2d\n\t"
        "add	v20.2d, v7.2d, v19.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v24.16b, v25.16b, #8\n\t"
        "ext	v22.16b, v26.16b, v24.16b, #8\n\t"
        "add	v25.2d, v25.2d, v20.2d\n\t"
        "sha512h	q25, q21, v22.2d\n\t"
        "add	v27.2d, v26.2d, v25.2d\n\t"
        "sha512h2	q25, q26, v23.2d\n\t"
        /* Load next 8 64-bit words of K */
        "ld1	{v16.2d, v17.2d, v18.2d, v19.2d}, [x3], #0x40\n\t"
        /* Round 24 */
        "sha512su0	v0.2d, v1.2d\n\t"
        "ext	v21.16b, v4.16b, v5.16b, #8\n\t"
        "sha512su1	v0.2d, v7.2d, v21.2d\n\t"
        "add	v20.2d, v0.2d, v16.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v27.16b, v24.16b, #8\n\t"
        "ext	v22.16b, v23.16b, v27.16b, #8\n\t"
        "add	v24.2d, v24.2d, v20.2d\n\t"
        "sha512h	q24, q21, v22.2d\n\t"
        "add	v26.2d, v23.2d, v24.2d\n\t"
        "sha512h2	q24, q23, v25.2d\n\t"
        /* Round 25 */
        "sha512su0	v1.2d, v2.2d\n\t"
        "ext	v21.16b, v5.16b, v6.16b, #8\n\t"
        "sha512su1	v1.2d, v0.2d, v21.2d\n\t"
        "add	v20.2d, v1.2d, v17.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v26.16b, v27.16b, #8\n\t"
        "ext	v22.16b, v25.16b, v26.16b, #8\n\t"
        "add	v27.2d, v27.2d, v20.2d\n\t"
        "sha512h	q27, q21, v22.2d\n\t"
        "add	v23.2d, v25.2d, v27.2d\n\t"
        "sha512h2	q27, q25, v24.2d\n\t"
        /* Round 26 */
        "sha512su0	v2.2d, v3.2d\n\t"
        "ext	v21.16b, v6.16b, v7.16b, #8\n\t"
        "sha512su1	v2.2d, v1.2d, v21.2d\n\t"
        "add	v20.2d, v2.2d, v18.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v23.16b, v26.16b, #8\n\t"
        "ext	v22.16b, v24.16b, v23.16b, #8\n\t"
        "add	v26.2d, v26.2d, v20.2d\n\t"
        "sha512h	q26, q21, v22.2d\n\t"
        "add	v25.2d, v24.2d, v26.2d\n\t"
        "sha512h2	q26, q24, v27.2d\n\t"
        /* Round 27 */
        "sha512su0	v3.2d, v4.2d\n\t"
        "ext	v21.16b, v7.16b, v0.16b, #8\n\t"
        "sha512su1	v3.2d, v2.2d, v21.2d\n\t"
        "add	v20.2d, v3.2d, v19.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v25.16b, v23.16b, #8\n\t"
        "ext	v22.16b, v27.16b, v25.16b, #8\n\t"
        "add	v23.2d, v23.2d, v20.2d\n\t"
        "sha512h	q23, q21, v22.2d\n\t"
        "add	v24.2d, v27.2d, v23.2d\n\t"
        "sha512h2	q23, q27, v26.2d\n\t"
        /* Load next 8 64-bit words of K */
        "ld1	{v16.2d, v17.2d, v18.2d, v19.2d}, [x3], #0x40\n\t"
        /* Round 28 */
        "sha512su0	v4.2d, v5.2d\n\t"
        "ext	v21.16b, v0.16b, v1.16b, #8\n\t"
        "sha512su1	v4.2d, v3.2d, v21.2d\n\t"
        "add	v20.2d, v4.2d, v16.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v24.16b, v25.16b, #8\n\t"
        "ext	v22.16b, v26.16b, v24.16b, #8\n\t"
        "add	v25.2d, v25.2d, v20.2d\n\t"
        "sha512h	q25, q21, v22.2d\n\t"
        "add	v27.2d, v26.2d, v25.2d\n\t"
        "sha512h2	q25, q26, v23.2d\n\t"
        /* Round 29 */
        "sha512su0	v5.2d, v6.2d\n\t"
        "ext	v21.16b, v1.16b, v2.16b, #8\n\t"
        "sha512su1	v5.2d, v4.2d, v21.2d\n\t"
        "add	v20.2d, v5.2d, v17.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v27.16b, v24.16b, #8\n\t"
        "ext	v22.16b, v23.16b, v27.16b, #8\n\t"
        "add	v24.2d, v24.2d, v20.2d\n\t"
        "sha512h	q24, q21, v22.2d\n\t"
        "add	v26.2d, v23.2d, v24.2d\n\t"
        "sha512h2	q24, q23, v25.2d\n\t"
        /* Round 30 */
        "sha512su0	v6.2d, v7.2d\n\t"
        "ext	v21.16b, v2.16b, v3.16b, #8\n\t"
        "sha512su1	v6.2d, v5.2d, v21.2d\n\t"
        "add	v20.2d, v6.2d, v18.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v26.16b, v27.16b, #8\n\t"
        "ext	v22.16b, v25.16b, v26.16b, #8\n\t"
        "add	v27.2d, v27.2d, v20.2d\n\t"
        "sha512h	q27, q21, v22.2d\n\t"
        "add	v23.2d, v25.2d, v27.2d\n\t"
        "sha512h2	q27, q25, v24.2d\n\t"
        /* Round 31 */
        "sha512su0	v7.2d, v0.2d\n\t"
        "ext	v21.16b, v3.16b, v4.16b, #8\n\t"
        "sha512su1	v7.2d, v6.2d, v21.2d\n\t"
        "add	v20.2d, v7.2d, v19.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v23.16b, v26.16b, #8\n\t"
        "ext	v22.16b, v24.16b, v23.16b, #8\n\t"
        "add	v26.2d, v26.2d, v20.2d\n\t"
        "sha512h	q26, q21, v22.2d\n\t"
        "add	v25.2d, v24.2d, v26.2d\n\t"
        "sha512h2	q26, q24, v27.2d\n\t"
        /* Load next 8 64-bit words of K */
        "ld1	{v16.2d, v17.2d, v18.2d, v19.2d}, [x3], #0x40\n\t"
        /* Round 32 */
        "sha512su0	v0.2d, v1.2d\n\t"
        "ext	v21.16b, v4.16b, v5.16b, #8\n\t"
        "sha512su1	v0.2d, v7.2d, v21.2d\n\t"
        "add	v20.2d, v0.2d, v16.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v25.16b, v23.16b, #8\n\t"
        "ext	v22.16b, v27.16b, v25.16b, #8\n\t"
        "add	v23.2d, v23.2d, v20.2d\n\t"
        "sha512h	q23, q21, v22.2d\n\t"
        "add	v24.2d, v27.2d, v23.2d\n\t"
        "sha512h2	q23, q27, v26.2d\n\t"
        /* Round 33 */
        "sha512su0	v1.2d, v2.2d\n\t"
        "ext	v21.16b, v5.16b, v6.16b, #8\n\t"
        "sha512su1	v1.2d, v0.2d, v21.2d\n\t"
        "add	v20.2d, v1.2d, v17.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v24.16b, v25.16b, #8\n\t"
        "ext	v22.16b, v26.16b, v24.16b, #8\n\t"
        "add	v25.2d, v25.2d, v20.2d\n\t"
        "sha512h	q25, q21, v22.2d\n\t"
        "add	v27.2d, v26.2d, v25.2d\n\t"
        "sha512h2	q25, q26, v23.2d\n\t"
        /* Round 34 */
        "sha512su0	v2.2d, v3.2d\n\t"
        "ext	v21.16b, v6.16b, v7.16b, #8\n\t"
        "sha512su1	v2.2d, v1.2d, v21.2d\n\t"
        "add	v20.2d, v2.2d, v18.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v27.16b, v24.16b, #8\n\t"
        "ext	v22.16b, v23.16b, v27.16b, #8\n\t"
        "add	v24.2d, v24.2d, v20.2d\n\t"
        "sha512h	q24, q21, v22.2d\n\t"
        "add	v26.2d, v23.2d, v24.2d\n\t"
        "sha512h2	q24, q23, v25.2d\n\t"
        /* Round 35 */
        "sha512su0	v3.2d, v4.2d\n\t"
        "ext	v21.16b, v7.16b, v0.16b, #8\n\t"
        "sha512su1	v3.2d, v2.2d, v21.2d\n\t"
        "add	v20.2d, v3.2d, v19.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v26.16b, v27.16b, #8\n\t"
        "ext	v22.16b, v25.16b, v26.16b, #8\n\t"
        "add	v27.2d, v27.2d, v20.2d\n\t"
        "sha512h	q27, q21, v22.2d\n\t"
        "add	v23.2d, v25.2d, v27.2d\n\t"
        "sha512h2	q27, q25, v24.2d\n\t"
        /* Load next 8 64-bit words of K */
        "ld1	{v16.2d, v17.2d, v18.2d, v19.2d}, [x3], #0x40\n\t"
        /* Round 36 */
        "sha512su0	v4.2d, v5.2d\n\t"
        "ext	v21.16b, v0.16b, v1.16b, #8\n\t"
        "sha512su1	v4.2d, v3.2d, v21.2d\n\t"
        "add	v20.2d, v4.2d, v16.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v23.16b, v26.16b, #8\n\t"
        "ext	v22.16b, v24.16b, v23.16b, #8\n\t"
        "add	v26.2d, v26.2d, v20.2d\n\t"
        "sha512h	q26, q21, v22.2d\n\t"
        "add	v25.2d, v24.2d, v26.2d\n\t"
        "sha512h2	q26, q24, v27.2d\n\t"
        /* Round 37 */
        "sha512su0	v5.2d, v6.2d\n\t"
        "ext	v21.16b, v1.16b, v2.16b, #8\n\t"
        "sha512su1	v5.2d, v4.2d, v21.2d\n\t"
        "add	v20.2d, v5.2d, v17.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v25.16b, v23.16b, #8\n\t"
        "ext	v22.16b, v27.16b, v25.16b, #8\n\t"
        "add	v23.2d, v23.2d, v20.2d\n\t"
        "sha512h	q23, q21, v22.2d\n\t"
        "add	v24.2d, v27.2d, v23.2d\n\t"
        "sha512h2	q23, q27, v26.2d\n\t"
        /* Round 38 */
        "sha512su0	v6.2d, v7.2d\n\t"
        "ext	v21.16b, v2.16b, v3.16b, #8\n\t"
        "sha512su1	v6.2d, v5.2d, v21.2d\n\t"
        "add	v20.2d, v6.2d, v18.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v24.16b, v25.16b, #8\n\t"
        "ext	v22.16b, v26.16b, v24.16b, #8\n\t"
        "add	v25.2d, v25.2d, v20.2d\n\t"
        "sha512h	q25, q21, v22.2d\n\t"
        "add	v27.2d, v26.2d, v25.2d\n\t"
        "sha512h2	q25, q26, v23.2d\n\t"
        /* Round 39 */
        "sha512su0	v7.2d, v0.2d\n\t"
        "ext	v21.16b, v3.16b, v4.16b, #8\n\t"
        "sha512su1	v7.2d, v6.2d, v21.2d\n\t"
        "add	v20.2d, v7.2d, v19.2d\n\t"
        "ext	v20.16b, v20.16b, v20.16b, #8\n\t"
        "ext	v21.16b, v27.16b, v24.16b, #8\n\t"
        "ext	v22.16b, v23.16b, v27.16b, #8\n\t"
        "add	v24.2d, v24.2d, v20.2d\n\t"
        "sha512h	q24, q21, v22.2d\n\t"
        "add	v26.2d, v23.2d, v24.2d\n\t"
        "sha512h2	q24, q23, v25.2d\n\t"
        "add	v27.2d, v27.2d, v31.2d\n\t"
        "add	v26.2d, v26.2d, v30.2d\n\t"
        "add	v25.2d, v25.2d, v29.2d\n\t"
        "add	v24.2d, v24.2d, v28.2d\n\t"
        "subs	%w[len], %w[len], #0x80\n\t"
        "bne	L_sha512_len_crypto_begin_%=\n\t"
        /* Store digest back */
        "st1	{v24.2d, v25.2d, v26.2d, v27.2d}, [%x[sha512]]\n\t"
        : [sha512] "+r" (sha512), [data] "+r" (data), [len] "+r" (len)
        : [L_SHA512_transform_crypto_len_k] "S" (L_SHA512_transform_crypto_len_k)
        : "memory", "x3", "x4", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31"
    );
}

#endif /* WOLFSSL_ARMASM_CRYPTO_SHA512 */
#endif /* WOLFSSL_SHA512 */
#endif /* __aarch64__ */
#endif /* WOLFSSL_ARMASM */
