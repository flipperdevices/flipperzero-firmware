/* armv8-poly1305.c
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

/*
 * Based off the public domain implementations by Andrew Moon
 * and Daniel J. Bernstein
 */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/types.h>

#ifdef WOLFSSL_ARMASM
#ifdef __aarch64__

#ifdef HAVE_POLY1305
#include <wolfssl/wolfcrypt/poly1305.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/cpuid.h>
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif
#ifdef CHACHA_AEAD_TEST
    #include <stdio.h>
#endif

static WC_INLINE void poly1305_blocks_16(Poly1305* ctx, const unsigned char *m,
                                         size_t bytes)
{
    __asm__ __volatile__ (
        "CMP        %[bytes], %[POLY1305_BLOCK_SIZE] \n\t"
        "BLO        L_poly1305_16_64_done_%= \n\t"
        /* Load r and h */
        "LDP        x21, x23, %[ctx_r]   \n\t"
        "LDR        w25, %[ctx_r_4]      \n\t"
        "LDP        x2, x4, %[ctx_h]     \n\t"
        "LDR        w6, %[ctx_h_4]       \n\t"
        "LSR        x22, x21, #32        \n\t"
        "LSR        x24, x23, #32        \n\t"
        "LSR        x3, x2, #32          \n\t"
        "LSR        x5, x4, #32          \n\t"
        "AND        x21, x21, #0x3ffffff \n\t"
        "AND        x23, x23, #0x3ffffff \n\t"
        "AND        x2, x2, #0x3ffffff   \n\t"
        "AND        x4, x4, #0x3ffffff   \n\t"
        /* s1 = r1 * 5; */
        /* s2 = r2 * 5; */
        /* s3 = r3 * 5; */
        /* s4 = r4 * 5; */
        "MOV        x15, #5              \n\t"
        "CMP        %[finished], #0      \n\t"
        "MUL        w7, w22, w15         \n\t"
        "CSET       %[finished], EQ      \n\t"
        "MUL        w8, w23, w15         \n\t"
        "LSL        %[finished], %[finished], #24 \n\t"
        "MUL        w9, w24, w15         \n\t"
        "MOV        x14, #0x3ffffff      \n\t"
        "MUL        w10, w25, w15        \n\t"
        "\n"
        ".align 2 \n\t"
    "L_poly1305_16_64_loop_%=: \n\t"
        /* t0 = U8TO64(&m[0]); */
        /* t1 = U8TO64(&m[8]); */
        "LDP        x16, x17, [%[m]], #16 \n\t"
        /* h0 += (U8TO32(m + 0)) & 0x3ffffff; */
        "AND        x26, x16, #0x3ffffff \n\t"
        "ADD        x2, x2, x26          \n\t"
        /* h1 += (U8TO32(m + 3) >> 2) & 0x3ffffff; */
        "AND        x26, x14, x16, LSR #26 \n\t"
        "ADD        x3, x3, x26          \n\t"
        /* h2 += (U8TO32(m + 6) >> 4) & 0x3ffffff; */
        "EXTR       x26, x17, x16, #52   \n\t"
        "AND        x26, x26, #0x3ffffff \n\t"
        "ADD        x4, x4, x26          \n\t"
        /* h3 += (U8TO32(m + 9) >> 6) & 0x3ffffff; */
        "AND        x26, x14, x17, LSR #14 \n\t"
        "ADD        x5, x5, x26          \n\t"
        /* h4 += (U8TO32(m + 12) >> 8) | hibit; */
        "ORR        x17, %[finished], x17, LSR #40 \n\t"
        "ADD        x6, x6, x17          \n\t"
        /* d0 = h0 * r0 + h1 * s4 + h2 * s3 + h3 * s2 + h4 * s1 */
        /* d1 = h0 * r1 + h1 * r0 + h2 * s4 + h3 * s3 + h4 * s2 */
        /* d2 = h0 * r2 + h1 * r1 + h2 * r0 + h3 * s4 + h4 * s3 */
        /* d3 = h0 * r3 + h1 * r2 + h2 * r1 + h3 * r0 + h4 * s4 */
        /* d4 = h0 * r4 + h1 * r3 + h2 * r2 + h3 * r1 + h4 * r0 */
        "MUL        x16, x2, x21         \n\t"
        "MUL        x17, x2, x22         \n\t"
        "MUL        x26, x2, x23         \n\t"
        "MUL        x19, x2, x24         \n\t"
        "MUL        x20, x2, x25         \n\t"
        "MADD       x16, x3, x10, x16    \n\t"
        "MADD       x17, x3, x21, x17    \n\t"
        "MADD       x26, x3, x22, x26    \n\t"
        "MADD       x19, x3, x23, x19    \n\t"
        "MADD       x20, x3, x24, x20    \n\t"
        "MADD       x16, x4, x9, x16     \n\t"
        "MADD       x17, x4, x10, x17    \n\t"
        "MADD       x26, x4, x21, x26    \n\t"
        "MADD       x19, x4, x22, x19    \n\t"
        "MADD       x20, x4, x23, x20    \n\t"
        "MADD       x16, x5, x8, x16     \n\t"
        "MADD       x17, x5, x9, x17     \n\t"
        "MADD       x26, x5, x10, x26    \n\t"
        "MADD       x19, x5, x21, x19    \n\t"
        "MADD       x20, x5, x22, x20    \n\t"
        "MADD       x16, x6, x7, x16     \n\t"
        "MADD       x17, x6, x8, x17     \n\t"
        "MADD       x26, x6, x9, x26     \n\t"
        "MADD       x19, x6, x10, x19    \n\t"
        "MADD       x20, x6, x21, x20    \n\t"
        /* d1 = d1 + d0 >> 26 */
        /* d2 = d2 + d1 >> 26 */
        /* d3 = d3 + d2 >> 26 */
        /* d4 = d4 + d3 >> 26 */
        /* h0 = d0 & 0x3ffffff */
        /* h1 = d1 & 0x3ffffff */
        /* h2 = d2 & 0x3ffffff */
        /* h0 = h0 + (d4 >> 26) * 5 */
        /* h1 = h1 + h0 >> 26 */
        /* h3 = d3 & 0x3ffffff */
        /* h4 = d4 & 0x3ffffff */
        /* h0 = h0 & 0x3ffffff */
        "ADD        x17, x17, x16, LSR #26 \n\t"
        "ADD        x20, x20, x19, LSR #26 \n\t"
        "AND        x16, x16, #0x3ffffff \n\t"
        "LSR        x2, x20, #26         \n\t"
        "AND        x19, x19, #0x3ffffff \n\t"
        "MADD       x16, x2, x15, x16    \n\t"
        "ADD        x26, x26, x17, LSR #26 \n\t"
        "AND        x17, x17, #0x3ffffff \n\t"
        "AND        x20, x20, #0x3ffffff \n\t"
        "ADD        x19, x19, x26, LSR #26 \n\t"
        "AND        x4, x26, #0x3ffffff  \n\t"
        "ADD        x3, x17, x16, LSR #26 \n\t"
        "AND        x2, x16, #0x3ffffff  \n\t"
        "ADD        x6, x20, x19, LSR #26 \n\t"
        "AND        x5, x19, #0x3ffffff  \n\t"
        "SUB        %[bytes], %[bytes], %[POLY1305_BLOCK_SIZE] \n\t"
        "CMP        %[bytes], %[POLY1305_BLOCK_SIZE] \n\t"
        "BHS        L_poly1305_16_64_loop_%= \n\t"
        /* Store h */
        "ORR        x2, x2, x3, LSL #32  \n\t"
        "ORR        x4, x4, x5, LSL #32  \n\t"
        "STP        x2, x4, %[ctx_h]     \n\t"
        "STR        w6, %[ctx_h_4]       \n\t"
        "\n"
        ".align 2 \n\t"
    "L_poly1305_16_64_done_%=: \n\t"
        : [ctx_h] "+m" (ctx->h[0]),
          [ctx_h_4] "+m" (ctx->h[4]),
          [bytes] "+r" (bytes),
          [m] "+r" (m)
        : [POLY1305_BLOCK_SIZE] "I" (POLY1305_BLOCK_SIZE),
          [ctx_r] "m" (ctx->r[0]),
          [ctx_r_4] "m" (ctx->r[4]),
          [finished] "r" ((word64)ctx->finished)
        : "memory", "cc",
          "w2", "w3", "w4", "w5", "w6", "w7", "w8", "w9", "w10", "w15",
          "w21", "w22", "w23", "w24", "w25", "x2", "x3", "x4", "x5", "x6",
          "x7", "x8", "x9", "x10", "x14", "x15", "x16", "x17", "x19", "x20",
          "x21", "x22", "x23", "x24", "x25", "x26"
    );
}

void poly1305_blocks(Poly1305* ctx, const unsigned char *m,
                            size_t bytes)
{
    __asm__ __volatile__ (
        /* If less than 4 blocks to process then use regular method */
        "CMP        %[bytes], %[POLY1305_BLOCK_SIZE]*4 \n\t"
        "BLO        L_poly1305_64_done_%= \n\t"
        "MOV        x9, #0x3ffffff       \n\t"
        /* Load h */
        "LDP        x20, x22, [%[h]]     \n\t"
        "MOV        v27.D[0], x9         \n\t"
        "LDR        w24, [%[h], #16]     \n\t"
        "MOV        v27.D[1], x9         \n\t"
        "LSR        x21, x20, #32        \n\t"
        "DUP        v29.4S, v27.S[0]     \n\t"
        "LSR        x23, x22, #32        \n\t"
        "MOV        x9, #5               \n\t"
        "AND        x20, x20, #0x3ffffff \n\t"
        "MOV        v28.D[0], x9         \n\t"
        "AND        x22, x22, #0x3ffffff \n\t"
        /* Zero accumulator registers */
        "MOVI       v15.16B, #0x0        \n\t"
        "MOVI       v16.16B, #0x0        \n\t"
        "MOVI       v17.16B, #0x0        \n\t"
        "MOVI       v18.16B, #0x0        \n\t"
        "MOVI       v19.16B, #0x0        \n\t"
        /* Set hibit */
        "CMP        %[finished], #0      \n\t"
        "CSET       x9, EQ               \n\t"
        "LSL        x9, x9, #24          \n\t"
        "MOV        v26.D[0], x9         \n\t"
        "MOV        v26.D[1], x9         \n\t"
        "DUP        v30.4S, v26.S[0]     \n\t"
        "CMP        %[bytes], %[POLY1305_BLOCK_SIZE]*6 \n\t"
        "BLO        L_poly1305_64_start_block_size_64_%= \n\t"
        /* Load r^2 to NEON v0, v1, v2, v3, v4 */
        "LD4        { v0.S-v3.S }[2], [%[r_2]], #16 \n\t"
        "LD1        { v4.S }[2], [%[r_2]] \n\t"
        "SUB        %[r_2], %[r_2], #16  \n\t"
        /* Load r^4 to NEON v0, v1, v2, v3, v4 */
        "LD4        { v0.S-v3.S }[0], [%[r_4]], #16 \n\t"
        "LD1        { v4.S }[0], [%[r_4]] \n\t"
        "SUB        %[r_4], %[r_4], #16  \n\t"
        "MOV        v0.S[1], v0.S[0]     \n\t"
        "MOV        v0.S[3], v0.S[2]     \n\t"
        "MOV        v1.S[1], v1.S[0]     \n\t"
        "MOV        v1.S[3], v1.S[2]     \n\t"
        "MOV        v2.S[1], v2.S[0]     \n\t"
        "MOV        v2.S[3], v2.S[2]     \n\t"
        "MOV        v3.S[1], v3.S[0]     \n\t"
        "MOV        v3.S[3], v3.S[2]     \n\t"
        "MOV        v4.S[1], v4.S[0]     \n\t"
        "MOV        v4.S[3], v4.S[2]     \n\t"
        /* Store [r^4, r^2] * 5 */
        "MUL        v5.4S, v0.4S, v28.S[0] \n\t"
        "MUL        v6.4S, v1.4S, v28.S[0] \n\t"
        "MUL        v7.4S, v2.4S, v28.S[0] \n\t"
        "MUL        v8.4S, v3.4S, v28.S[0] \n\t"
        "MUL        v9.4S, v4.4S, v28.S[0] \n\t"
        /* Copy r^4 to ARM */
        "MOV        w25, v0.S[0]         \n\t"
        "MOV        w26, v1.S[0]         \n\t"
        "MOV        w27, v2.S[0]         \n\t"
        "MOV        w28, v3.S[0]         \n\t"
        "MOV        w30, v4.S[0]         \n\t"
        /* Copy 5*r^4 to ARM */
        "MOV        w15, v5.S[0]         \n\t"
        "MOV        w16, v6.S[0]         \n\t"
        "MOV        w17, v7.S[0]         \n\t"
        "MOV        w8, v8.S[0]          \n\t"
        "MOV        w19, v9.S[0]         \n\t"
        /* Load m */
        /* Load four message blocks to NEON v10, v11, v12, v13, v14 */
        "LD4        { v10.4S-v13.4S }, [%[m]], #64 \n\t"
        "SUB        %[bytes], %[bytes], %[POLY1305_BLOCK_SIZE]*4 \n\t"
        "USHR       v14.4S, v13.4S, #8   \n\t"
        "ORR        v14.16B, v14.16B, v30.16B \n\t"
        "SHL        v13.4S, v13.4S, #18  \n\t"
        "SRI        v13.4S, v12.4S, #14  \n\t"
        "SHL        v12.4S, v12.4S, #12  \n\t"
        "SRI        v12.4S, v11.4S, #20  \n\t"
        "SHL        v11.4S, v11.4S, #6   \n\t"
        "SRI        v11.4S, v10.4S, #26  \n\t"
        "AND        v10.16B, v10.16B, v29.16B \n\t"
        "AND        v11.16B, v11.16B, v29.16B \n\t"
        "AND        v12.16B, v12.16B, v29.16B \n\t"
        "AND        v13.16B, v13.16B, v29.16B \n\t"
        "AND        v14.16B, v14.16B, v29.16B \n\t"
        /* Four message blocks loaded */
        /* Add messages to accumulator */
        "ADD        v15.2S, v15.2S, v10.2S \n\t"
        "ADD        v16.2S, v16.2S, v11.2S \n\t"
        "ADD        v17.2S, v17.2S, v12.2S \n\t"
        "ADD        v18.2S, v18.2S, v13.2S \n\t"
        "ADD        v19.2S, v19.2S, v14.2S \n\t"
        "\n"
        ".align 2 \n\t"
    "L_poly1305_64_loop_128_%=: \n\t"
        /* d0 = h0*r0 + h1*s4 + h2*s3 + h3*s2 + h4*s1 */
        /* d1 = h0*r1 + h1*r0 + h2*s4 + h3*s3 + h4*s2 */
        /* d2 = h0*r2 + h1*r1 + h2*r0 + h3*s4 + h4*s3 */
        /* d3 = h0*r3 + h1*r2 + h2*r1 + h3*r0 + h4*s4 */
        /* d4 = h0*r4 + h1*r3 + h2*r2 + h3*r1 + h4*r0 */
        "UMULL      v21.2D, v15.2S, v0.2S \n\t"
        /* Compute h*r^2 */
        /* d0 = h0 * r0 + h1 * s4 + h2 * s3 + h3 * s2 + h4 * s1 */
        /* d1 = h0 * r1 + h1 * r0 + h2 * s4 + h3 * s3 + h4 * s2 */
        /* d2 = h0 * r2 + h1 * r1 + h2 * r0 + h3 * s4 + h4 * s3 */
        /* d3 = h0 * r3 + h1 * r2 + h2 * r1 + h3 * r0 + h4 * s4 */
        /* d4 = h0 * r4 + h1 * r3 + h2 * r2 + h3 * r1 + h4 * r0 */
        "MUL        x9, x20, x25         \n\t"
        "UMULL      v22.2D, v15.2S, v1.2S \n\t"
        "MUL        x10, x20, x26        \n\t"
        "UMULL      v23.2D, v15.2S, v2.2S \n\t"
        "MUL        x11, x20, x27        \n\t"
        "UMULL      v24.2D, v15.2S, v3.2S \n\t"
        "MUL        x12, x20, x28        \n\t"
        "UMULL      v25.2D, v15.2S, v4.2S \n\t"
        "MUL        x13, x20, x30        \n\t"
        "UMLAL      v21.2D, v16.2S, v9.2S \n\t"
        "MADD       x9, x21, x19, x9     \n\t"
        "UMLAL      v22.2D, v16.2S, v0.2S \n\t"
        "MADD       x10, x21, x25, x10   \n\t"
        "UMLAL      v23.2D, v16.2S, v1.2S \n\t"
        "MADD       x11, x21, x26, x11   \n\t"
        "UMLAL      v24.2D, v16.2S, v2.2S \n\t"
        "MADD       x12, x21, x27, x12   \n\t"
        "UMLAL      v25.2D, v16.2S, v3.2S \n\t"
        "MADD       x13, x21, x28, x13   \n\t"
        "UMLAL      v21.2D, v17.2S, v8.2S \n\t"
        "MADD       x9, x22, x8, x9      \n\t"
        "UMLAL      v22.2D, v17.2S, v9.2S \n\t"
        "MADD       x10, x22, x19, x10   \n\t"
        "UMLAL      v23.2D, v17.2S, v0.2S \n\t"
        "MADD       x11, x22, x25, x11   \n\t"
        "UMLAL      v24.2D, v17.2S, v1.2S \n\t"
        "MADD       x12, x22, x26, x12   \n\t"
        "UMLAL      v25.2D, v17.2S, v2.2S \n\t"
        "MADD       x13, x22, x27, x13   \n\t"
        "UMLAL      v21.2D, v18.2S, v7.2S \n\t"
        "MADD       x9, x23, x17, x9     \n\t"
        "UMLAL      v22.2D, v18.2S, v8.2S \n\t"
        "MADD       x10, x23, x8, x10    \n\t"
        "UMLAL      v23.2D, v18.2S, v9.2S \n\t"
        "MADD       x11, x23, x19, x11   \n\t"
        "UMLAL      v24.2D, v18.2S, v0.2S \n\t"
        "MADD       x12, x23, x25, x12   \n\t"
        "UMLAL      v25.2D, v18.2S, v1.2S \n\t"
        "MADD       x13, x23, x26, x13   \n\t"
        "UMLAL      v21.2D, v19.2S, v6.2S \n\t"
        "MADD       x9, x24, x16, x9     \n\t"
        "UMLAL      v22.2D, v19.2S, v7.2S \n\t"
        "MADD       x10, x24, x17, x10   \n\t"
        "UMLAL      v23.2D, v19.2S, v8.2S \n\t"
        "MADD       x11, x24, x8, x11    \n\t"
        "UMLAL      v24.2D, v19.2S, v9.2S \n\t"
        "MADD       x12, x24, x19, x12   \n\t"
        "UMLAL      v25.2D, v19.2S, v0.2S \n\t"
        "MADD       x13, x24, x25, x13   \n\t"
        /* d0 = h0*r0 + h1*s4 + h2*s3 + h3*s2 + h4*s1 */
        /* d1 = h0*r1 + h1*r0 + h2*s4 + h3*s3 + h4*s2 */
        /* d2 = h0*r2 + h1*r1 + h2*r0 + h3*s4 + h4*s3 */
        /* d3 = h0*r3 + h1*r2 + h2*r1 + h3*r0 + h4*s4 */
        /* d4 = h0*r4 + h1*r3 + h2*r2 + h3*r1 + h4*r0 */
        "UMLAL2     v21.2D, v10.4S, v0.4S \n\t"
        /* Reduce h % P */
        "MOV        x14, #5              \n\t"
        "UMLAL2     v22.2D, v10.4S, v1.4S \n\t"
        "ADD        x10, x10, x9, LSR #26 \n\t"
        "UMLAL2     v23.2D, v10.4S, v2.4S \n\t"
        "ADD        x13, x13, x12, LSR #26 \n\t"
        "UMLAL2     v24.2D, v10.4S, v3.4S \n\t"
        "AND        x9, x9, #0x3ffffff   \n\t"
        "UMLAL2     v25.2D, v10.4S, v4.4S \n\t"
        "LSR        x20, x13, #26        \n\t"
        "UMLAL2     v21.2D, v11.4S, v9.4S \n\t"
        "AND        x12, x12, #0x3ffffff \n\t"
        "UMLAL2     v22.2D, v11.4S, v0.4S \n\t"
        "MADD       x9, x20, x14, x9     \n\t"
        "UMLAL2     v23.2D, v11.4S, v1.4S \n\t"
        "ADD        x11, x11, x10, LSR #26 \n\t"
        "UMLAL2     v24.2D, v11.4S, v2.4S \n\t"
        "AND        x10, x10, #0x3ffffff \n\t"
        "UMLAL2     v25.2D, v11.4S, v3.4S \n\t"
        "AND        x13, x13, #0x3ffffff \n\t"
        "UMLAL2     v21.2D, v12.4S, v8.4S \n\t"
        "ADD        x12, x12, x11, LSR #26 \n\t"
        "UMLAL2     v22.2D, v12.4S, v9.4S \n\t"
        "AND        x22, x11, #0x3ffffff \n\t"
        "UMLAL2     v23.2D, v12.4S, v0.4S \n\t"
        "ADD        x21, x10, x9, LSR #26 \n\t"
        "UMLAL2     v24.2D, v12.4S, v1.4S \n\t"
        "AND        x20, x9, #0x3ffffff  \n\t"
        "UMLAL2     v25.2D, v12.4S, v2.4S \n\t"
        "ADD        x24, x13, x12, LSR #26 \n\t"
        "UMLAL2     v21.2D, v13.4S, v7.4S \n\t"
        "AND        x23, x12, #0x3ffffff \n\t"
        "UMLAL2     v22.2D, v13.4S, v8.4S \n\t"
        "UMLAL2     v23.2D, v13.4S, v9.4S \n\t"
        "UMLAL2     v24.2D, v13.4S, v0.4S \n\t"
        "UMLAL2     v25.2D, v13.4S, v1.4S \n\t"
        "UMLAL2     v21.2D, v14.4S, v6.4S \n\t"
        "UMLAL2     v22.2D, v14.4S, v7.4S \n\t"
        "UMLAL2     v23.2D, v14.4S, v8.4S \n\t"
        "UMLAL2     v24.2D, v14.4S, v9.4S \n\t"
        "UMLAL2     v25.2D, v14.4S, v0.4S \n\t"
        /* If less than six message blocks left then leave loop */
        "CMP        %[bytes], %[POLY1305_BLOCK_SIZE]*6 \n\t"
        "BLS        L_poly1305_64_loop_128_final_%= \n\t"
        /* Load m */
        /* Load four message blocks to NEON v10, v11, v12, v13, v14 */
        "LD4        { v10.4S-v13.4S }, [%[m]], #64 \n\t"
        "SUB        %[bytes], %[bytes], %[POLY1305_BLOCK_SIZE]*4 \n\t"
        "USHR       v14.4S, v13.4S, #8   \n\t"
        "ORR        v14.16B, v14.16B, v30.16B \n\t"
        "SHL        v13.4S, v13.4S, #18  \n\t"
        "SRI        v13.4S, v12.4S, #14  \n\t"
        "SHL        v12.4S, v12.4S, #12  \n\t"
        "SRI        v12.4S, v11.4S, #20  \n\t"
        "SHL        v11.4S, v11.4S, #6   \n\t"
        "SRI        v11.4S, v10.4S, #26  \n\t"
        "AND        v10.16B, v10.16B, v29.16B \n\t"
        "AND        v11.16B, v11.16B, v29.16B \n\t"
        "AND        v12.16B, v12.16B, v29.16B \n\t"
        "AND        v13.16B, v13.16B, v29.16B \n\t"
        "AND        v14.16B, v14.16B, v29.16B \n\t"
        /* Four message blocks loaded */
        /* Add new message block to accumulator */
        "UADDW      v21.2D, v21.2D, v10.2S \n\t"
        "UADDW      v22.2D, v22.2D, v11.2S \n\t"
        "UADDW      v23.2D, v23.2D, v12.2S \n\t"
        "UADDW      v24.2D, v24.2D, v13.2S \n\t"
        "UADDW      v25.2D, v25.2D, v14.2S \n\t"
        /* Reduce radix 26 NEON */
        /* Interleave h0 -> h1 -> h2 -> h3 -> h4 */
        /*       with h3 -> h4 -> h0 -> h1 */
        "USRA       v22.2D, v21.2D, #26  \n\t"
        "AND        v21.16B, v21.16B, v27.16B \n\t"
        "USRA       v25.2D, v24.2D, #26  \n\t"
        "AND        v24.16B, v24.16B, v27.16B \n\t"
        "USHR       v15.2D, v25.2D, #26  \n\t"
        "USRA       v23.2D, v22.2D, #26  \n\t"
        /* Simulate multiplying by 5 using adding and shifting */
        "SHL        v18.2D, v15.2D, #2   \n\t"
        "AND        v16.16B, v22.16B, v27.16B \n\t"
        "ADD        v18.2D, v18.2D, v15.2D \n\t"
        "AND        v19.16B, v25.16B, v27.16B \n\t"
        "ADD        v21.2D, v21.2D, v18.2D \n\t"
        "USRA       v24.2D, v23.2D, #26  \n\t"
        "AND        v17.16B, v23.16B, v27.16B \n\t"
        "USRA       v16.2D, v21.2D, #26  \n\t"
        "AND        v15.16B, v21.16B, v27.16B \n\t"
        "USRA       v19.2D, v24.2D, #26  \n\t"
        "AND        v18.16B, v24.16B, v27.16B \n\t"
        /* Copy values to lower halves of result registers */
        "MOV        v15.S[1], v15.S[2]   \n\t"
        "MOV        v16.S[1], v16.S[2]   \n\t"
        "MOV        v17.S[1], v17.S[2]   \n\t"
        "MOV        v18.S[1], v18.S[2]   \n\t"
        "MOV        v19.S[1], v19.S[2]   \n\t"
        "B          L_poly1305_64_loop_128_%= \n\t"
        "\n"
        ".align 2 \n\t"
    "L_poly1305_64_loop_128_final_%=: \n\t"
        /* Load m */
        /* Load two message blocks to NEON v10, v11, v12, v13, v14 */
        "LD2        { v10.2D-v11.2D }, [%[m]], #32 \n\t"
        /* Copy r^2 to lower half of registers */
        "MOV        v0.D[0], v0.D[1]     \n\t"
        "SUB        %[bytes], %[bytes], %[POLY1305_BLOCK_SIZE]*2 \n\t"
        "MOV        v5.D[0], v5.D[1]     \n\t"
        "USHR       v14.2D, v11.2D, #40  \n\t"
        "MOV        v1.D[0], v1.D[1]     \n\t"
        "ORR        v14.16B, v14.16B, v26.16B \n\t"
        "MOV        v6.D[0], v6.D[1]     \n\t"
        "USHR       v13.2D, v11.2D, #14  \n\t"
        "MOV        v2.D[0], v2.D[1]     \n\t"
        "AND        v13.16B, v13.16B, v27.16B \n\t"
        "MOV        v7.D[0], v7.D[1]     \n\t"
        "SHL        v12.2D, v11.2D, #12  \n\t"
        "MOV        v3.D[0], v3.D[1]     \n\t"
        "SRI        v12.2D, v10.2D, #52  \n\t"
        "MOV        v8.D[0], v8.D[1]     \n\t"
        "AND        v12.16B, v12.16B, v27.16B \n\t"
        "MOV        v4.D[0], v4.D[1]     \n\t"
        "USHR       v11.2D, v10.2D, #26  \n\t"
        "MOV        v9.D[0], v9.D[1]     \n\t"
        "AND        v11.16B, v11.16B, v27.16B \n\t"
        /* Copy r^2 to ARM */
        "MOV        w25, v0.S[2]         \n\t"
        "AND        v10.16B, v10.16B, v27.16B \n\t"
        "MOV        w26, v1.S[2]         \n\t"
        /* Two message blocks loaded */
        /* Add last messages */
        "ADD        v21.2D, v21.2D, v10.2D \n\t"
        "MOV        w27, v2.S[2]         \n\t"
        "ADD        v22.2D, v22.2D, v11.2D \n\t"
        "MOV        w28, v3.S[2]         \n\t"
        "ADD        v23.2D, v23.2D, v12.2D \n\t"
        "MOV        w30, v4.S[2]         \n\t"
        "ADD        v24.2D, v24.2D, v13.2D \n\t"
        /* Copy 5*r^2 to ARM */
        "MOV        w15, v5.S[2]         \n\t"
        "ADD        v25.2D, v25.2D, v14.2D \n\t"
        "MOV        w16, v6.S[2]         \n\t"
        /* Reduce message to be ready for next multiplication */
        /* Reduce radix 26 NEON */
        /* Interleave h0 -> h1 -> h2 -> h3 -> h4 */
        /*       with h3 -> h4 -> h0 -> h1 */
        "USRA       v22.2D, v21.2D, #26  \n\t"
        "MOV        w17, v7.S[2]         \n\t"
        "AND        v21.16B, v21.16B, v27.16B \n\t"
        "MOV        w8, v8.S[2]          \n\t"
        "USRA       v25.2D, v24.2D, #26  \n\t"
        "MOV        w19, v9.S[2]         \n\t"
        "AND        v24.16B, v24.16B, v27.16B \n\t"
        "USHR       v15.2D, v25.2D, #26  \n\t"
        "USRA       v23.2D, v22.2D, #26  \n\t"
        /* Simulate multiplying by 5 using adding and shifting */
        "SHL        v18.2D, v15.2D, #2   \n\t"
        "AND        v16.16B, v22.16B, v27.16B \n\t"
        "ADD        v18.2D, v18.2D, v15.2D \n\t"
        "AND        v19.16B, v25.16B, v27.16B \n\t"
        "ADD        v21.2D, v21.2D, v18.2D \n\t"
        "USRA       v24.2D, v23.2D, #26  \n\t"
        "AND        v17.16B, v23.16B, v27.16B \n\t"
        "USRA       v16.2D, v21.2D, #26  \n\t"
        "AND        v15.16B, v21.16B, v27.16B \n\t"
        "USRA       v19.2D, v24.2D, #26  \n\t"
        "AND        v18.16B, v24.16B, v27.16B \n\t"
        /* Copy values to lower halves of result registers */
        "MOV        v15.S[1], v15.S[2]   \n\t"
        "MOV        v16.S[1], v16.S[2]   \n\t"
        "MOV        v17.S[1], v17.S[2]   \n\t"
        "MOV        v18.S[1], v18.S[2]   \n\t"
        "MOV        v19.S[1], v19.S[2]   \n\t"
        /* If less than 2 blocks left go straight to final multiplication. */
        "CMP        %[bytes], %[POLY1305_BLOCK_SIZE]*2 \n\t"
        "BLO        L_poly1305_64_last_mult_%= \n\t"
        /* Else go to one loop of L_poly1305_64_loop_64 */
        "B          L_poly1305_64_loop_64_%= \n\t"
        "\n"
        ".align 2 \n\t"
    "L_poly1305_64_start_block_size_64_%=: \n\t"
        /* Load r^2 to NEON v0, v1, v2, v3, v4 */
        "LD4R       { v0.2S-v3.2S }, [%[r_2]], #16 \n\t"
        "LD1R       { v4.2S }, [%[r_2]]  \n\t"
        "SUB        %[r_2], %[r_2], #16  \n\t"
        /* Store r^2 * 5 */
        "MUL        v5.4S, v0.4S, v28.S[0] \n\t"
        "MUL        v6.4S, v1.4S, v28.S[0] \n\t"
        "MUL        v7.4S, v2.4S, v28.S[0] \n\t"
        "MUL        v8.4S, v3.4S, v28.S[0] \n\t"
        "MUL        v9.4S, v4.4S, v28.S[0] \n\t"
        /* Copy r^2 to ARM */
        "MOV        w25, v0.S[0]         \n\t"
        "MOV        w26, v1.S[0]         \n\t"
        "MOV        w27, v2.S[0]         \n\t"
        "MOV        w28, v3.S[0]         \n\t"
        "MOV        w30, v4.S[0]         \n\t"
        /* Copy 5*r^2 to ARM */
        "MOV        w15, v5.S[0]         \n\t"
        "MOV        w16, v6.S[0]         \n\t"
        "MOV        w17, v7.S[0]         \n\t"
        "MOV        w8, v8.S[0]          \n\t"
        "MOV        w19, v9.S[0]         \n\t"
        /* Load m */
        /* Load two message blocks to NEON v10, v11, v12, v13, v14 */
        "LD2        { v10.2D-v11.2D }, [%[m]], #32 \n\t"
        "SUB        %[bytes], %[bytes], %[POLY1305_BLOCK_SIZE]*2 \n\t"
        "USHR       v14.2D, v11.2D, #40  \n\t"
        "ORR        v14.16B, v14.16B, v26.16B \n\t"
        "USHR       v13.2D, v11.2D, #14  \n\t"
        "AND        v13.16B, v13.16B, v27.16B \n\t"
        "SHL        v12.2D, v11.2D, #12  \n\t"
        "SRI        v12.2D, v10.2D, #52  \n\t"
        "AND        v12.16B, v12.16B, v27.16B \n\t"
        "USHR       v11.2D, v10.2D, #26  \n\t"
        "AND        v11.16B, v11.16B, v27.16B \n\t"
        "AND        v10.16B, v10.16B, v27.16B \n\t"
        "MOV        v10.S[1], v10.S[2]   \n\t"
        "MOV        v11.S[1], v11.S[2]   \n\t"
        "MOV        v12.S[1], v12.S[2]   \n\t"
        "MOV        v13.S[1], v13.S[2]   \n\t"
        "MOV        v14.S[1], v14.S[2]   \n\t"
        /* Two message blocks loaded */
        /* Add messages to accumulator */
        "ADD        v15.2S, v15.2S, v10.2S \n\t"
        "ADD        v16.2S, v16.2S, v11.2S \n\t"
        "ADD        v17.2S, v17.2S, v12.2S \n\t"
        "ADD        v18.2S, v18.2S, v13.2S \n\t"
        "ADD        v19.2S, v19.2S, v14.2S \n\t"
        "\n"
        ".align 2 \n\t"
    "L_poly1305_64_loop_64_%=: \n\t"
        /* d0 = h0*r0 + h1*s4 + h2*s3 + h3*s2 + h4*s1 */
        /* d1 = h0*r1 + h1*r0 + h2*s4 + h3*s3 + h4*s2 */
        /* d2 = h0*r2 + h1*r1 + h2*r0 + h3*s4 + h4*s3 */
        /* d3 = h0*r3 + h1*r2 + h2*r1 + h3*r0 + h4*s4 */
        /* d4 = h0*r4 + h1*r3 + h2*r2 + h3*r1 + h4*r0 */
        "UMULL      v21.2D, v15.2S, v0.2S \n\t"
        /* Compute h*r^2 */
        /* d0 = h0 * r0 + h1 * s4 + h2 * s3 + h3 * s2 + h4 * s1 */
        /* d1 = h0 * r1 + h1 * r0 + h2 * s4 + h3 * s3 + h4 * s2 */
        /* d2 = h0 * r2 + h1 * r1 + h2 * r0 + h3 * s4 + h4 * s3 */
        /* d3 = h0 * r3 + h1 * r2 + h2 * r1 + h3 * r0 + h4 * s4 */
        /* d4 = h0 * r4 + h1 * r3 + h2 * r2 + h3 * r1 + h4 * r0 */
        "MUL        x9, x20, x25         \n\t"
        "UMULL      v22.2D, v15.2S, v1.2S \n\t"
        "MUL        x10, x20, x26        \n\t"
        "UMULL      v23.2D, v15.2S, v2.2S \n\t"
        "MUL        x11, x20, x27        \n\t"
        "UMULL      v24.2D, v15.2S, v3.2S \n\t"
        "MUL        x12, x20, x28        \n\t"
        "UMULL      v25.2D, v15.2S, v4.2S \n\t"
        "MUL        x13, x20, x30        \n\t"
        "UMLAL      v21.2D, v16.2S, v9.2S \n\t"
        "MADD       x9, x21, x19, x9     \n\t"
        "UMLAL      v22.2D, v16.2S, v0.2S \n\t"
        "MADD       x10, x21, x25, x10   \n\t"
        "UMLAL      v23.2D, v16.2S, v1.2S \n\t"
        "MADD       x11, x21, x26, x11   \n\t"
        "UMLAL      v24.2D, v16.2S, v2.2S \n\t"
        "MADD       x12, x21, x27, x12   \n\t"
        "UMLAL      v25.2D, v16.2S, v3.2S \n\t"
        "MADD       x13, x21, x28, x13   \n\t"
        "UMLAL      v21.2D, v17.2S, v8.2S \n\t"
        "MADD       x9, x22, x8, x9      \n\t"
        "UMLAL      v22.2D, v17.2S, v9.2S \n\t"
        "MADD       x10, x22, x19, x10   \n\t"
        "UMLAL      v23.2D, v17.2S, v0.2S \n\t"
        "MADD       x11, x22, x25, x11   \n\t"
        "UMLAL      v24.2D, v17.2S, v1.2S \n\t"
        "MADD       x12, x22, x26, x12   \n\t"
        "UMLAL      v25.2D, v17.2S, v2.2S \n\t"
        "MADD       x13, x22, x27, x13   \n\t"
        "UMLAL      v21.2D, v18.2S, v7.2S \n\t"
        "MADD       x9, x23, x17, x9     \n\t"
        "UMLAL      v22.2D, v18.2S, v8.2S \n\t"
        "MADD       x10, x23, x8, x10    \n\t"
        "UMLAL      v23.2D, v18.2S, v9.2S \n\t"
        "MADD       x11, x23, x19, x11   \n\t"
        "UMLAL      v24.2D, v18.2S, v0.2S \n\t"
        "MADD       x12, x23, x25, x12   \n\t"
        "UMLAL      v25.2D, v18.2S, v1.2S \n\t"
        "MADD       x13, x23, x26, x13   \n\t"
        "UMLAL      v21.2D, v19.2S, v6.2S \n\t"
        "MADD       x9, x24, x16, x9     \n\t"
        "UMLAL      v22.2D, v19.2S, v7.2S \n\t"
        "MADD       x10, x24, x17, x10   \n\t"
        "UMLAL      v23.2D, v19.2S, v8.2S \n\t"
        "MADD       x11, x24, x8, x11    \n\t"
        "UMLAL      v24.2D, v19.2S, v9.2S \n\t"
        "MADD       x12, x24, x19, x12   \n\t"
        "UMLAL      v25.2D, v19.2S, v0.2S \n\t"
        "MADD       x13, x24, x25, x13   \n\t"
        /* Load m */
        /* Load two message blocks to NEON v10, v11, v12, v13, v14 */
        "LD2        { v10.2D-v11.2D }, [%[m]], #32 \n\t"
        /* Reduce h % P */
        "MOV        x14, #5              \n\t"
        "SUB        %[bytes], %[bytes], %[POLY1305_BLOCK_SIZE]*2 \n\t"
        "ADD        x10, x10, x9, LSR #26 \n\t"
        "USHR       v14.2D, v11.2D, #40  \n\t"
        "ADD        x13, x13, x12, LSR #26 \n\t"
        "ORR        v14.16B, v14.16B, v26.16B \n\t"
        "AND        x9, x9, #0x3ffffff   \n\t"
        "USHR       v13.2D, v11.2D, #14  \n\t"
        "LSR        x20, x13, #26        \n\t"
        "AND        v13.16B, v13.16B, v27.16B \n\t"
        "AND        x12, x12, #0x3ffffff \n\t"
        "SHL        v12.2D, v11.2D, #12  \n\t"
        "MADD       x9, x20, x14, x9     \n\t"
        "SRI        v12.2D, v10.2D, #52  \n\t"
        "ADD        x11, x11, x10, LSR #26 \n\t"
        "AND        v12.16B, v12.16B, v27.16B \n\t"
        "AND        x10, x10, #0x3ffffff \n\t"
        "USHR       v11.2D, v10.2D, #26  \n\t"
        "AND        x13, x13, #0x3ffffff \n\t"
        "AND        v11.16B, v11.16B, v27.16B \n\t"
        "ADD        x12, x12, x11, LSR #26 \n\t"
        "AND        v10.16B, v10.16B, v27.16B \n\t"
        "AND        x22, x11, #0x3ffffff \n\t"
        /* Two message blocks loaded */
        "ADD        v21.2D, v21.2D, v10.2D \n\t"
        "ADD        x21, x10, x9, LSR #26 \n\t"
        "ADD        v22.2D, v22.2D, v11.2D \n\t"
        "AND        x20, x9, #0x3ffffff  \n\t"
        "ADD        v23.2D, v23.2D, v12.2D \n\t"
        "ADD        x24, x13, x12, LSR #26 \n\t"
        "ADD        v24.2D, v24.2D, v13.2D \n\t"
        "AND        x23, x12, #0x3ffffff \n\t"
        "ADD        v25.2D, v25.2D, v14.2D \n\t"
        /* Reduce radix 26 NEON */
        /* Interleave h0 -> h1 -> h2 -> h3 -> h4 */
        /*       with h3 -> h4 -> h0 -> h1 */
        "USRA       v22.2D, v21.2D, #26  \n\t"
        "AND        v21.16B, v21.16B, v27.16B \n\t"
        "USRA       v25.2D, v24.2D, #26  \n\t"
        "AND        v24.16B, v24.16B, v27.16B \n\t"
        "USHR       v15.2D, v25.2D, #26  \n\t"
        "USRA       v23.2D, v22.2D, #26  \n\t"
        /* Simulate multiplying by 5 using adding and shifting */
        "SHL        v18.2D, v15.2D, #2   \n\t"
        "AND        v16.16B, v22.16B, v27.16B \n\t"
        "ADD        v18.2D, v18.2D, v15.2D \n\t"
        "AND        v19.16B, v25.16B, v27.16B \n\t"
        "ADD        v21.2D, v21.2D, v18.2D \n\t"
        "USRA       v24.2D, v23.2D, #26  \n\t"
        "AND        v17.16B, v23.16B, v27.16B \n\t"
        "USRA       v16.2D, v21.2D, #26  \n\t"
        "AND        v15.16B, v21.16B, v27.16B \n\t"
        "USRA       v19.2D, v24.2D, #26  \n\t"
        "AND        v18.16B, v24.16B, v27.16B \n\t"
        /* Copy values to lower halves of result registers */
        "MOV        v15.S[1], v15.S[2]   \n\t"
        "MOV        v16.S[1], v16.S[2]   \n\t"
        "MOV        v17.S[1], v17.S[2]   \n\t"
        "MOV        v18.S[1], v18.S[2]   \n\t"
        "MOV        v19.S[1], v19.S[2]   \n\t"
        /* If at least two message blocks left then loop_64 */
        "CMP        %[bytes], %[POLY1305_BLOCK_SIZE]*2 \n\t"
        "BHS        L_poly1305_64_loop_64_%= \n\t"
        "\n"
        ".align 2 \n\t"
    "L_poly1305_64_last_mult_%=: \n\t"
        /* Load r */
        "LD4        { v0.S-v3.S }[1], [%[r]], #16 \n\t"
        /* Compute h*r^2 */
        /* d0 = h0 * r0 + h1 * s4 + h2 * s3 + h3 * s2 + h4 * s1 */
        /* d1 = h0 * r1 + h1 * r0 + h2 * s4 + h3 * s3 + h4 * s2 */
        /* d2 = h0 * r2 + h1 * r1 + h2 * r0 + h3 * s4 + h4 * s3 */
        /* d3 = h0 * r3 + h1 * r2 + h2 * r1 + h3 * r0 + h4 * s4 */
        /* d4 = h0 * r4 + h1 * r3 + h2 * r2 + h3 * r1 + h4 * r0 */
        "MUL        x9, x20, x25         \n\t"
        "LD1        { v4.S }[1], [%[r]]  \n\t"
        "MUL        x10, x20, x26        \n\t"
        "SUB        %[r], %[r], #16      \n\t"
        "MUL        x11, x20, x27        \n\t"
        /* Store [r^2, r] * 5 */
        "MUL        v5.2S, v0.2S, v28.S[0] \n\t"
        "MUL        x12, x20, x28        \n\t"
        "MUL        v6.2S, v1.2S, v28.S[0] \n\t"
        "MUL        x13, x20, x30        \n\t"
        "MUL        v7.2S, v2.2S, v28.S[0] \n\t"
        "MADD       x9, x21, x19, x9     \n\t"
        "MUL        v8.2S, v3.2S, v28.S[0] \n\t"
        "MADD       x10, x21, x25, x10   \n\t"
        "MUL        v9.2S, v4.2S, v28.S[0] \n\t"
        "MADD       x11, x21, x26, x11   \n\t"
        /* Final multiply by [r^2, r] */
        /* d0 = h0*r0 + h1*s4 + h2*s3 + h3*s2 + h4*s1 */
        /* d1 = h0*r1 + h1*r0 + h2*s4 + h3*s3 + h4*s2 */
        /* d2 = h0*r2 + h1*r1 + h2*r0 + h3*s4 + h4*s3 */
        /* d3 = h0*r3 + h1*r2 + h2*r1 + h3*r0 + h4*s4 */
        /* d4 = h0*r4 + h1*r3 + h2*r2 + h3*r1 + h4*r0 */
        "UMULL      v21.2D, v15.2S, v0.2S \n\t"
        "MADD       x12, x21, x27, x12   \n\t"
        "UMULL      v22.2D, v15.2S, v1.2S \n\t"
        "MADD       x13, x21, x28, x13   \n\t"
        "UMULL      v23.2D, v15.2S, v2.2S \n\t"
        "MADD       x9, x22, x8, x9      \n\t"
        "UMULL      v24.2D, v15.2S, v3.2S \n\t"
        "MADD       x10, x22, x19, x10   \n\t"
        "UMULL      v25.2D, v15.2S, v4.2S \n\t"
        "MADD       x11, x22, x25, x11   \n\t"
        "UMLAL      v21.2D, v16.2S, v9.2S \n\t"
        "MADD       x12, x22, x26, x12   \n\t"
        "UMLAL      v22.2D, v16.2S, v0.2S \n\t"
        "MADD       x13, x22, x27, x13   \n\t"
        "UMLAL      v23.2D, v16.2S, v1.2S \n\t"
        "MADD       x9, x23, x17, x9     \n\t"
        "UMLAL      v24.2D, v16.2S, v2.2S \n\t"
        "MADD       x10, x23, x8, x10    \n\t"
        "UMLAL      v25.2D, v16.2S, v3.2S \n\t"
        "MADD       x11, x23, x19, x11   \n\t"
        "UMLAL      v21.2D, v17.2S, v8.2S \n\t"
        "MADD       x12, x23, x25, x12   \n\t"
        "UMLAL      v22.2D, v17.2S, v9.2S \n\t"
        "MADD       x13, x23, x26, x13   \n\t"
        "UMLAL      v23.2D, v17.2S, v0.2S \n\t"
        "MADD       x9, x24, x16, x9     \n\t"
        "UMLAL      v24.2D, v17.2S, v1.2S \n\t"
        "MADD       x10, x24, x17, x10   \n\t"
        "UMLAL      v25.2D, v17.2S, v2.2S \n\t"
        "MADD       x11, x24, x8, x11    \n\t"
        "UMLAL      v21.2D, v18.2S, v7.2S \n\t"
        "MADD       x12, x24, x19, x12   \n\t"
        "UMLAL      v22.2D, v18.2S, v8.2S \n\t"
        "MADD       x13, x24, x25, x13   \n\t"
        "UMLAL      v23.2D, v18.2S, v9.2S \n\t"
        /* Reduce h % P */
        "MOV        x14, #5              \n\t"
        "UMLAL      v24.2D, v18.2S, v0.2S \n\t"
        "ADD        x10, x10, x9, LSR #26 \n\t"
        "UMLAL      v25.2D, v18.2S, v1.2S \n\t"
        "ADD        x13, x13, x12, LSR #26 \n\t"
        "UMLAL      v21.2D, v19.2S, v6.2S \n\t"
        "AND        x9, x9, #0x3ffffff   \n\t"
        "UMLAL      v22.2D, v19.2S, v7.2S \n\t"
        "LSR        x20, x13, #26        \n\t"
        "UMLAL      v23.2D, v19.2S, v8.2S \n\t"
        "AND        x12, x12, #0x3ffffff \n\t"
        "UMLAL      v24.2D, v19.2S, v9.2S \n\t"
        "MADD       x9, x20, x14, x9     \n\t"
        "UMLAL      v25.2D, v19.2S, v0.2S \n\t"
        "ADD        x11, x11, x10, LSR #26 \n\t"
        /* Add even and odd elements */
        "ADDP       d21, v21.2D          \n\t"
        "AND        x10, x10, #0x3ffffff \n\t"
        "ADDP       d22, v22.2D          \n\t"
        "AND        x13, x13, #0x3ffffff \n\t"
        "ADDP       d23, v23.2D          \n\t"
        "ADD        x12, x12, x11, LSR #26 \n\t"
        "ADDP       d24, v24.2D          \n\t"
        "AND        x22, x11, #0x3ffffff \n\t"
        "ADDP       d25, v25.2D          \n\t"
        "ADD        x21, x10, x9, LSR #26 \n\t"
        "AND        x20, x9, #0x3ffffff  \n\t"
        "ADD        x24, x13, x12, LSR #26 \n\t"
        "AND        x23, x12, #0x3ffffff \n\t"
        /* Load h to NEON */
        "MOV        v5.D[0], x20         \n\t"
        "MOV        v6.D[0], x21         \n\t"
        "MOV        v7.D[0], x22         \n\t"
        "MOV        v8.D[0], x23         \n\t"
        "MOV        v9.D[0], x24         \n\t"
        /* Add ctx->h to current accumulator */
        "ADD        v21.2D, v21.2D, v5.2D \n\t"
        "ADD        v22.2D, v22.2D, v6.2D \n\t"
        "ADD        v23.2D, v23.2D, v7.2D \n\t"
        "ADD        v24.2D, v24.2D, v8.2D \n\t"
        "ADD        v25.2D, v25.2D, v9.2D \n\t"
        /* Reduce h (h % P) */
        /* Reduce radix 26 NEON */
        /* Interleave h0 -> h1 -> h2 -> h3 -> h4 */
        /*       with h3 -> h4 -> h0 -> h1 */
        "USRA       v22.2D, v21.2D, #26  \n\t"
        "AND        v21.16B, v21.16B, v27.16B \n\t"
        "USRA       v25.2D, v24.2D, #26  \n\t"
        "AND        v24.16B, v24.16B, v27.16B \n\t"
        "USHR       v5.2D, v25.2D, #26   \n\t"
        "USRA       v23.2D, v22.2D, #26  \n\t"
        /* Simulate multiplying by 5 using adding and shifting */
        "SHL        v8.2D, v5.2D, #2     \n\t"
        "AND        v6.16B, v22.16B, v27.16B \n\t"
        "ADD        v8.2D, v8.2D, v5.2D  \n\t"
        "AND        v9.16B, v25.16B, v27.16B \n\t"
        "ADD        v21.2D, v21.2D, v8.2D \n\t"
        "USRA       v24.2D, v23.2D, #26  \n\t"
        "AND        v7.16B, v23.16B, v27.16B \n\t"
        "USRA       v6.2D, v21.2D, #26   \n\t"
        "AND        v5.16B, v21.16B, v27.16B \n\t"
        "USRA       v9.2D, v24.2D, #26   \n\t"
        "AND        v8.16B, v24.16B, v27.16B \n\t"
        /* Copy values to lower halves of result registers */
        /* Store h */
        "ST4        { v5.S-v8.S }[0], [%[h]], #16 \n\t"
        "ST1        { v9.S }[0], [%[h]]  \n\t"
        "SUB        %[h], %[h], #16      \n\t"
        "\n"
        ".align 2 \n\t"
    "L_poly1305_64_done_%=: \n\t"
        : [bytes] "+r" (bytes),
          [m] "+r" (m),
          [ctx] "+m" (ctx)
        : [POLY1305_BLOCK_SIZE] "I" (POLY1305_BLOCK_SIZE),
          [h] "r" (ctx->h),
          [r] "r" (ctx->r),
          [r_2] "r" (ctx->r_2),
          [r_4] "r" (ctx->r_4),
          [finished] "r" ((word64)ctx->finished)
        : "memory", "cc",
          "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8", "v9",
          "v10", "v11", "v12", "v13", "v14", "v15", "v16", "v17", "v18", "v19",
          "v21", "v22", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v30",
          "w8", "w9", "w10", "w11", "w12", "w13", "w14", "w15", "w16", "w17",
          "w19", "w20", "w21", "w22", "w23", "w24", "w25", "w26", "w27", "w28",
          "w30", "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16",
          "x17", "x19", "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27",
          "x28", "x30"
    );
    poly1305_blocks_16(ctx, m, bytes);
}

void poly1305_block(Poly1305* ctx, const unsigned char *m)
{
    poly1305_blocks_16(ctx, m, POLY1305_BLOCK_SIZE);
}

#if defined(POLY130564)
static word64 clamp[] = {
    0x0ffffffc0fffffff,
    0x0ffffffc0ffffffc,
};
#endif /* POLY130564 */


int wc_Poly1305SetKey(Poly1305* ctx, const byte* key, word32 keySz)
{
    if (key == NULL)
        return BAD_FUNC_ARG;

#ifdef CHACHA_AEAD_TEST
    word32 k;
    printf("Poly key used:\n");
    for (k = 0; k < keySz; k++) {
        printf("%02x", key[k]);
        if ((k+1) % 8 == 0)
            printf("\n");
    }
    printf("\n");
#endif

    if (keySz != 32 || ctx == NULL)
        return BAD_FUNC_ARG;

    __asm__ __volatile__ (
        /* Load key material */
        "LDP        x8, x9, [%[key]]     \n\t"
        "LDP        x10, x11, [%[key], #16] \n\t"
        /* Load clamp */
        "LDP        x12, x13, [%[clamp]] \n\t"
        /* Apply clamp */
        /* r &= 0xffffffc0ffffffc0ffffffc0fffffff */
        "AND        x8, x8, x12          \n\t"
        "AND        x9, x9, x13          \n\t"
        "MOV        x19, xzr             \n\t"
        "MOV        x20, xzr             \n\t"
        "MOV        x21, xzr             \n\t"
        "MOV        x22, xzr             \n\t"
        "MOV        x23, xzr             \n\t"
        "BFI        x19, x8, #0, #26     \n\t"
        "LSR        x8, x8, #26          \n\t"
        "BFI        x20, x8, #0, #26     \n\t"
        "LSR        x8, x8, #26          \n\t"
        "BFI        x21, x8, #0, #12     \n\t"
        "BFI        x21, x9, #12, #14    \n\t"
        "LSR        x9, x9, #14          \n\t"
        "BFI        x22, x9, #0, #26     \n\t"
        "LSR        x9, x9, #26          \n\t"
        "BFI        x23, x9, #0, #24     \n\t"
        /* Compute r^2 */
        /* r*5 */
        "MOV        x8, #5               \n\t"
        "MUL        x24, x20, x8         \n\t"
        "MUL        x25, x21, x8         \n\t"
        "MUL        x26, x22, x8         \n\t"
        "MUL        x27, x23, x8         \n\t"
        /* d = r*r */
        /* d0 = h0 * r0 + h1 * s4 + h2 * s3 + h3 * s2 + h4 * s1 */
        /* d1 = h0 * r1 + h1 * r0 + h2 * s4 + h3 * s3 + h4 * s2 */
        /* d2 = h0 * r2 + h1 * r1 + h2 * r0 + h3 * s4 + h4 * s3 */
        /* d3 = h0 * r3 + h1 * r2 + h2 * r1 + h3 * r0 + h4 * s4 */
        /* d4 = h0 * r4 + h1 * r3 + h2 * r2 + h3 * r1 + h4 * r0 */
        "MUL        x14, x19, x19        \n\t"
        "MUL        x15, x19, x20        \n\t"
        "MUL        x16, x19, x21        \n\t"
        "MUL        x17, x19, x22        \n\t"
        "MUL        x7, x19, x23         \n\t"
        "MADD       x14, x20, x27, x14   \n\t"
        "MADD       x15, x20, x19, x15   \n\t"
        "MADD       x16, x20, x20, x16   \n\t"
        "MADD       x17, x20, x21, x17   \n\t"
        "MADD       x7, x20, x22, x7     \n\t"
        "MADD       x14, x21, x26, x14   \n\t"
        "MADD       x15, x21, x27, x15   \n\t"
        "MADD       x16, x21, x19, x16   \n\t"
        "MADD       x17, x21, x20, x17   \n\t"
        "MADD       x7, x21, x21, x7     \n\t"
        "MADD       x14, x22, x25, x14   \n\t"
        "MADD       x15, x22, x26, x15   \n\t"
        "MADD       x16, x22, x27, x16   \n\t"
        "MADD       x17, x22, x19, x17   \n\t"
        "MADD       x7, x22, x20, x7     \n\t"
        "MADD       x14, x23, x24, x14   \n\t"
        "MADD       x15, x23, x25, x15   \n\t"
        "MADD       x16, x23, x26, x16   \n\t"
        "MADD       x17, x23, x27, x17   \n\t"
        "MADD       x7, x23, x19, x7     \n\t"
        /* r_2 = r^2 % P */
        "ADD        x15, x15, x14, LSR #26 \n\t"
        "ADD        x7, x7, x17, LSR #26 \n\t"
        "AND        x14, x14, #0x3ffffff \n\t"
        "LSR        x9, x7, #26          \n\t"
        "AND        x17, x17, #0x3ffffff \n\t"
        "MADD       x14, x9, x8, x14     \n\t"
        "ADD        x16, x16, x15, LSR #26 \n\t"
        "AND        x15, x15, #0x3ffffff \n\t"
        "AND        x7, x7, #0x3ffffff   \n\t"
        "ADD        x17, x17, x16, LSR #26 \n\t"
        "AND        x16, x16, #0x3ffffff \n\t"
        "ADD        x15, x15, x14, LSR #26 \n\t"
        "AND        x14, x14, #0x3ffffff \n\t"
        "ADD        x7, x7, x17, LSR #26 \n\t"
        "AND        x17, x17, #0x3ffffff \n\t"
        /* Store r */
        "ORR        x19, x19, x20, LSL #32 \n\t"
        "ORR        x21, x21, x22, LSL #32 \n\t"
        "STP        x19, x21, [%[ctx_r]] \n\t"
        "STR        w23, [%[ctx_r], #16] \n\t"
        "MOV        x8, #5               \n\t"
        "MUL        x24, x15, x8         \n\t"
        "MUL        x25, x16, x8         \n\t"
        "MUL        x26, x17, x8         \n\t"
        "MUL        x27, x7, x8          \n\t"
        /* Compute r^4 */
        /* d0 = h0 * r0 + h1 * s4 + h2 * s3 + h3 * s2 + h4 * s1 */
        /* d1 = h0 * r1 + h1 * r0 + h2 * s4 + h3 * s3 + h4 * s2 */
        /* d2 = h0 * r2 + h1 * r1 + h2 * r0 + h3 * s4 + h4 * s3 */
        /* d3 = h0 * r3 + h1 * r2 + h2 * r1 + h3 * r0 + h4 * s4 */
        /* d4 = h0 * r4 + h1 * r3 + h2 * r2 + h3 * r1 + h4 * r0 */
        "MUL        x19, x14, x14        \n\t"
        "MUL        x20, x14, x15        \n\t"
        "MUL        x21, x14, x16        \n\t"
        "MUL        x22, x14, x17        \n\t"
        "MUL        x23, x14, x7         \n\t"
        "MADD       x19, x15, x27, x19   \n\t"
        "MADD       x20, x15, x14, x20   \n\t"
        "MADD       x21, x15, x15, x21   \n\t"
        "MADD       x22, x15, x16, x22   \n\t"
        "MADD       x23, x15, x17, x23   \n\t"
        "MADD       x19, x16, x26, x19   \n\t"
        "MADD       x20, x16, x27, x20   \n\t"
        "MADD       x21, x16, x14, x21   \n\t"
        "MADD       x22, x16, x15, x22   \n\t"
        "MADD       x23, x16, x16, x23   \n\t"
        "MADD       x19, x17, x25, x19   \n\t"
        "MADD       x20, x17, x26, x20   \n\t"
        "MADD       x21, x17, x27, x21   \n\t"
        "MADD       x22, x17, x14, x22   \n\t"
        "MADD       x23, x17, x15, x23   \n\t"
        "MADD       x19, x7, x24, x19    \n\t"
        "MADD       x20, x7, x25, x20    \n\t"
        "MADD       x21, x7, x26, x21    \n\t"
        "MADD       x22, x7, x27, x22    \n\t"
        "MADD       x23, x7, x14, x23    \n\t"
        /* r^4 % P */
        "ADD        x20, x20, x19, LSR #26 \n\t"
        "ADD        x23, x23, x22, LSR #26 \n\t"
        "AND        x19, x19, #0x3ffffff \n\t"
        "LSR        x9, x23, #26         \n\t"
        "AND        x22, x22, #0x3ffffff \n\t"
        "MADD       x19, x9, x8, x19     \n\t"
        "ADD        x21, x21, x20, LSR #26 \n\t"
        "AND        x20, x20, #0x3ffffff \n\t"
        "AND        x23, x23, #0x3ffffff \n\t"
        "ADD        x22, x22, x21, LSR #26 \n\t"
        "AND        x21, x21, #0x3ffffff \n\t"
        "ADD        x20, x20, x19, LSR #26 \n\t"
        "AND        x19, x19, #0x3ffffff \n\t"
        "ADD        x23, x23, x22, LSR #26 \n\t"
        "AND        x22, x22, #0x3ffffff \n\t"
        /* Store r^2 */
        "ORR        x14, x14, x15, LSL #32 \n\t"
        "ORR        x16, x16, x17, LSL #32 \n\t"
        "STP        x14, x16, [%[ctx_r_2]] \n\t"
        "STR        w7, [%[ctx_r_2], #16] \n\t"
        /* Store r^4 */
        "ORR        x19, x19, x20, LSL #32 \n\t"
        "ORR        x21, x21, x22, LSL #32 \n\t"
        "STP        x19, x21, [%[ctx_r_4]] \n\t"
        "STR        w23, [%[ctx_r_4], #16] \n\t"
        /* h (accumulator) = 0 */
        "STP        xzr, xzr, [%[ctx_h_0]] \n\t"
        "STR        wzr, [%[ctx_h_0], #16] \n\t"
        /* Save pad for later */
        "STP        x10, x11, [%[ctx_pad]] \n\t"
        /* Zero leftover */
        "STR        xzr, [%[ctx_leftover]] \n\t"
        /* Zero finished */
        "STRB       wzr, [%[ctx_finished]] \n\t"
        :
        : [clamp] "r" (clamp),
          [key] "r" (key),
          [ctx_r] "r" (ctx->r),
          [ctx_r_2] "r" (ctx->r_2),
          [ctx_r_4] "r" (ctx->r_4),
          [ctx_h_0] "r" (ctx->h),
          [ctx_pad] "r" (ctx->pad),
          [ctx_leftover] "r" (&ctx->leftover),
          [ctx_finished] "r" (&ctx->finished)
        : "memory", "cc",
          "w7", "w14", "w15", "w16", "w17", "w19", "w20", "w21", "w22", "w23",
          "x7", "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16",
          "x17", "x19", "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27"
    );

    return 0;
}


int wc_Poly1305Final(Poly1305* ctx, byte* mac)
{

    if (ctx == NULL)
        return BAD_FUNC_ARG;

    /* process the remaining block */
    if (ctx->leftover) {
        size_t i = ctx->leftover;
        ctx->buffer[i++] = 1;
        for (; i < POLY1305_BLOCK_SIZE; i++)
            ctx->buffer[i] = 0;
        ctx->finished = 1;
        poly1305_block(ctx, ctx->buffer);
    }

    __asm__ __volatile__ (
        /* Load raw h and zero h registers */
        "LDP        x2, x3, %[h_addr]    \n\t"
        "MOV        x5, xzr              \n\t"
        "LDR        w4, %[h_4_addr]      \n\t"
        "MOV        x6, xzr              \n\t"
        "LDP        x16, x17, %[pad_addr] \n\t"
        /* Base 26 -> Base 64 */
        "MOV        w5, w2               \n\t"
        "LSR        x2, x2, #32          \n\t"
        "ORR        x5, x5, x2, LSL #26  \n\t"
        "ORR        x5, x5, x3, LSL #52  \n\t"
        "LSR        w6, w3, #12          \n\t"
        "LSR        x3, x3, #32          \n\t"
        "ORR        x6, x6, x3, LSL #14  \n\t"
        "ORR        x6, x6, x4, LSL #40  \n\t"
        "LSR        x7, x4, #24          \n\t"
        /* Check if h is larger than p */
        "ADDS       x2, x5, #5           \n\t"
        "ADCS       x3, x6, xzr          \n\t"
        "ADC        x4, x7, xzr          \n\t"
        /* Check if h+5 is larger than 2^130 */
        "CMP        x4, #3               \n\t"
        "CSEL       x5, x2, x5, HI       \n\t"
        "CSEL       x6, x3, x6, HI       \n\t"
        "ADDS       x5, x5, x16          \n\t"
        "ADC        x6, x6, x17          \n\t"
        "STP        x5, x6, [%[mac]]     \n\t"
        : [mac] "+r" (mac)
        : [pad_addr] "m" (ctx->pad),
          [h_addr] "m" (ctx->h),
          [h_4_addr] "m" (ctx->h[4])
        : "memory", "cc",
          "w2", "w3", "w4", "w5", "w6", "w7", "x2", "x3", "x4", "x5",
          "x6", "x7", "x16", "x17"
    );

    /* zero out the state */
    ctx->h[0] = 0;
    ctx->h[1] = 0;
    ctx->h[2] = 0;
    ctx->h[3] = 0;
    ctx->h[4] = 0;
    ctx->r[0] = 0;
    ctx->r[1] = 0;
    ctx->r[2] = 0;
    ctx->r[3] = 0;
    ctx->r[4] = 0;
    ctx->r_2[0] = 0;
    ctx->r_2[1] = 0;
    ctx->r_2[2] = 0;
    ctx->r_2[3] = 0;
    ctx->r_2[4] = 0;
    ctx->r_4[0] = 0;
    ctx->r_4[1] = 0;
    ctx->r_4[2] = 0;
    ctx->r_4[3] = 0;
    ctx->r_4[4] = 0;
    ctx->pad[0] = 0;
    ctx->pad[1] = 0;
    ctx->pad[2] = 0;
    ctx->pad[3] = 0;

    return 0;
}

#endif /* HAVE_POLY1305 */
#endif /* __aarch64__ */
#endif /* WOLFSSL_ARMASM */
