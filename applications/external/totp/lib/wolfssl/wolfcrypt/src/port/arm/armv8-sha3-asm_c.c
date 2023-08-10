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
 *   ruby ./sha3/sha3.rb arm64 ../wolfssl/wolfcrypt/src/port/arm/armv8-sha3-asm.c
 */
#ifdef WOLFSSL_ARMASM
#ifdef __aarch64__
#include <wolfssl/wolfcrypt/sha3.h>

#ifdef WOLFSSL_SHA3
#ifdef WOLFSSL_ARMASM_CRYPTO_SHA3
static const uint64_t L_SHA3_transform_crypto_r[] = {
    0x1UL,
    0x8082UL,
    0x800000000000808aUL,
    0x8000000080008000UL,
    0x808bUL,
    0x80000001UL,
    0x8000000080008081UL,
    0x8000000000008009UL,
    0x8aUL,
    0x88UL,
    0x80008009UL,
    0x8000000aUL,
    0x8000808bUL,
    0x800000000000008bUL,
    0x8000000000008089UL,
    0x8000000000008003UL,
    0x8000000000008002UL,
    0x8000000000000080UL,
    0x800aUL,
    0x800000008000000aUL,
    0x8000000080008081UL,
    0x8000000000008080UL,
    0x80000001UL,
    0x8000000080008008UL,
};

void BlockSha3(unsigned long* state)
{
    __asm__ __volatile__ (
#ifdef __APPLE__
    ".arch_extension sha3\n\t"
#endif /* __APPLE__ */
#ifndef __APPLE__
        "adrp x1, %[L_SHA3_transform_crypto_r]\n\t"
        "add  x1, x1, :lo12:%[L_SHA3_transform_crypto_r]\n\t"
#else
        "adrp x1, %[L_SHA3_transform_crypto_r]@PAGE\n\t"
        "add  x1, x1, %[L_SHA3_transform_crypto_r]@PAGEOFF\n\t"
#endif /* __APPLE__ */
        "ld4	{v0.d, v1.d, v2.d, v3.d}[0], [%x[state]], #32\n\t"
        "ld4	{v4.d, v5.d, v6.d, v7.d}[0], [%x[state]], #32\n\t"
        "ld4	{v8.d, v9.d, v10.d, v11.d}[0], [%x[state]], #32\n\t"
        "ld4	{v12.d, v13.d, v14.d, v15.d}[0], [%x[state]], #32\n\t"
        "ld4	{v16.d, v17.d, v18.d, v19.d}[0], [%x[state]], #32\n\t"
        "ld4	{v20.d, v21.d, v22.d, v23.d}[0], [%x[state]], #32\n\t"
        "ld1	{v24.1d}, [%x[state]]\n\t"
        "sub	%x[state], %x[state], #0xc0\n\t"
        "mov	x2, #24\n\t"
        /* Start of 24 rounds */
        "\n"
    "L_sha3_crypto_begin_%=: \n\t"
        /* Col Mix */
        "eor3	v31.16b, v0.16b, v5.16b, v10.16b\n\t"
        "eor3	v27.16b, v1.16b, v6.16b, v11.16b\n\t"
        "eor3	v28.16b, v2.16b, v7.16b, v12.16b\n\t"
        "eor3	v29.16b, v3.16b, v8.16b, v13.16b\n\t"
        "eor3	v30.16b, v4.16b, v9.16b, v14.16b\n\t"
        "eor3	v31.16b, v31.16b, v15.16b, v20.16b\n\t"
        "eor3	v27.16b, v27.16b, v16.16b, v21.16b\n\t"
        "eor3	v28.16b, v28.16b, v17.16b, v22.16b\n\t"
        "eor3	v29.16b, v29.16b, v18.16b, v23.16b\n\t"
        "eor3	v30.16b, v30.16b, v19.16b, v24.16b\n\t"
        "rax1	v25.2d, v30.2d, v27.2d\n\t"
        "rax1	v26.2d, v31.2d, v28.2d\n\t"
        "rax1	v27.2d, v27.2d, v29.2d\n\t"
        "rax1	v28.2d, v28.2d, v30.2d\n\t"
        "rax1	v29.2d, v29.2d, v31.2d\n\t"
        "eor	v0.16b, v0.16b, v25.16b\n\t"
        "xar	v30.2d, v1.2d, v26.2d, #63\n\t"
        "xar	v1.2d, v6.2d, v26.2d, #20\n\t"
        "xar	v6.2d, v9.2d, v29.2d, #44\n\t"
        "xar	v9.2d, v22.2d, v27.2d, #3\n\t"
        "xar	v22.2d, v14.2d, v29.2d, #25\n\t"
        "xar	v14.2d, v20.2d, v25.2d, #46\n\t"
        "xar	v20.2d, v2.2d, v27.2d, #2\n\t"
        "xar	v2.2d, v12.2d, v27.2d, #21\n\t"
        "xar	v12.2d, v13.2d, v28.2d, #39\n\t"
        "xar	v13.2d, v19.2d, v29.2d, #56\n\t"
        "xar	v19.2d, v23.2d, v28.2d, #8\n\t"
        "xar	v23.2d, v15.2d, v25.2d, #23\n\t"
        "xar	v15.2d, v4.2d, v29.2d, #37\n\t"
        "xar	v4.2d, v24.2d, v29.2d, #50\n\t"
        "xar	v24.2d, v21.2d, v26.2d, #62\n\t"
        "xar	v21.2d, v8.2d, v28.2d, #9\n\t"
        "xar	v8.2d, v16.2d, v26.2d, #19\n\t"
        "xar	v16.2d, v5.2d, v25.2d, #28\n\t"
        "xar	v5.2d, v3.2d, v28.2d, #36\n\t"
        "xar	v3.2d, v18.2d, v28.2d, #43\n\t"
        "xar	v18.2d, v17.2d, v27.2d, #49\n\t"
        "xar	v17.2d, v11.2d, v26.2d, #54\n\t"
        "xar	v11.2d, v7.2d, v27.2d, #58\n\t"
        "xar	v7.2d, v10.2d, v25.2d, #61\n\t"
        /* Row Mix */
        "mov	v25.16b, v0.16b\n\t"
        "mov	v26.16b, v1.16b\n\t"
        "bcax	v0.16b, v25.16b, v2.16b, v26.16b\n\t"
        "bcax	v1.16b, v26.16b, v3.16b, v2.16b\n\t"
        "bcax	v2.16b, v2.16b, v4.16b, v3.16b\n\t"
        "bcax	v3.16b, v3.16b, v25.16b, v4.16b\n\t"
        "bcax	v4.16b, v4.16b, v26.16b, v25.16b\n\t"
        "mov	v25.16b, v5.16b\n\t"
        "mov	v26.16b, v6.16b\n\t"
        "bcax	v5.16b, v25.16b, v7.16b, v26.16b\n\t"
        "bcax	v6.16b, v26.16b, v8.16b, v7.16b\n\t"
        "bcax	v7.16b, v7.16b, v9.16b, v8.16b\n\t"
        "bcax	v8.16b, v8.16b, v25.16b, v9.16b\n\t"
        "bcax	v9.16b, v9.16b, v26.16b, v25.16b\n\t"
        "mov	v26.16b, v11.16b\n\t"
        "bcax	v10.16b, v30.16b, v12.16b, v26.16b\n\t"
        "bcax	v11.16b, v26.16b, v13.16b, v12.16b\n\t"
        "bcax	v12.16b, v12.16b, v14.16b, v13.16b\n\t"
        "bcax	v13.16b, v13.16b, v30.16b, v14.16b\n\t"
        "bcax	v14.16b, v14.16b, v26.16b, v30.16b\n\t"
        "mov	v25.16b, v15.16b\n\t"
        "mov	v26.16b, v16.16b\n\t"
        "bcax	v15.16b, v25.16b, v17.16b, v26.16b\n\t"
        "bcax	v16.16b, v26.16b, v18.16b, v17.16b\n\t"
        "bcax	v17.16b, v17.16b, v19.16b, v18.16b\n\t"
        "bcax	v18.16b, v18.16b, v25.16b, v19.16b\n\t"
        "bcax	v19.16b, v19.16b, v26.16b, v25.16b\n\t"
        "mov	v25.16b, v20.16b\n\t"
        "mov	v26.16b, v21.16b\n\t"
        "bcax	v20.16b, v25.16b, v22.16b, v26.16b\n\t"
        "bcax	v21.16b, v26.16b, v23.16b, v22.16b\n\t"
        "bcax	v22.16b, v22.16b, v24.16b, v23.16b\n\t"
        "bcax	v23.16b, v23.16b, v25.16b, v24.16b\n\t"
        "bcax	v24.16b, v24.16b, v26.16b, v25.16b\n\t"
        "ld1r	{v30.2d}, [x1], #8\n\t"
        "subs	x2, x2, #1\n\t"
        "eor	v0.16b, v0.16b, v30.16b\n\t"
        "bne	L_sha3_crypto_begin_%=\n\t"
        "st4	{v0.d, v1.d, v2.d, v3.d}[0], [%x[state]], #32\n\t"
        "st4	{v4.d, v5.d, v6.d, v7.d}[0], [%x[state]], #32\n\t"
        "st4	{v8.d, v9.d, v10.d, v11.d}[0], [%x[state]], #32\n\t"
        "st4	{v12.d, v13.d, v14.d, v15.d}[0], [%x[state]], #32\n\t"
        "st4	{v16.d, v17.d, v18.d, v19.d}[0], [%x[state]], #32\n\t"
        "st4	{v20.d, v21.d, v22.d, v23.d}[0], [%x[state]], #32\n\t"
        "st1	{v24.1d}, [%x[state]]\n\t"
        : [state] "+r" (state)
        : [L_SHA3_transform_crypto_r] "S" (L_SHA3_transform_crypto_r)
        : "memory", "x1", "x2", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31"
    );
}

#endif /* WOLFSSL_ARMASM_CRYPTO_SHA3 */
#endif /* WOLFSSL_SHA3 */
#endif /* __aarch64__ */
#endif /* WOLFSSL_ARMASM */
