/* poly1305.c
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

DESCRIPTION
This library contains implementation for the Poly1305 authenticator.

Based off the public domain implementations by Andrew Moon
and Daniel J. Bernstein

*/


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

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

#ifdef _MSC_VER
    /* 4127 warning constant while(1)  */
    #pragma warning(disable: 4127)
#endif

#if defined(WOLFSSL_X86_64_BUILD) && defined(USE_INTEL_SPEEDUP)
    #include <emmintrin.h>
    #include <immintrin.h>

    #if defined(__GNUC__) && ((__GNUC__ < 4) || \
                              (__GNUC__ == 4 && __GNUC_MINOR__ <= 8))
        #undef  NO_AVX2_SUPPORT
        #define NO_AVX2_SUPPORT
    #endif
    #if defined(__clang__) && ((__clang_major__ < 3) || \
                               (__clang_major__ == 3 && __clang_minor__ <= 5))
        #define NO_AVX2_SUPPORT
    #elif defined(__clang__) && defined(NO_AVX2_SUPPORT)
        #undef NO_AVX2_SUPPORT
    #endif

    #define HAVE_INTEL_AVX1
    #ifndef NO_AVX2_SUPPORT
        #define HAVE_INTEL_AVX2
    #endif
#endif

#if defined(WOLFSSL_X86_64_BUILD) && defined(USE_INTEL_SPEEDUP)
static word32 intel_flags = 0;
static word32 cpu_flags_set = 0;
#endif

#if (defined(WOLFSSL_X86_64_BUILD) && defined(USE_INTEL_SPEEDUP)) || \
        defined(POLY130564)
    #if defined(_MSC_VER)
        #define POLY1305_NOINLINE __declspec(noinline)
    #elif defined(__GNUC__)
        #define POLY1305_NOINLINE __attribute__((noinline))
    #else
        #define POLY1305_NOINLINE
    #endif

    #if defined(_MSC_VER)
        #include <intrin.h>

        typedef struct word128 {
            word64 lo;
            word64 hi;
        } word128;

        #define MUL(out, x, y) out.lo = _umul128((x), (y), &out.hi)
        #define ADD(out, in) { word64 t = out.lo; out.lo += in.lo; \
                               out.hi += (out.lo < t) + in.hi; }
        #define ADDLO(out, in) { word64 t = out.lo; out.lo += in; \
                                 out.hi += (out.lo < t); }
        #define SHR(in, shift) (__shiftright128(in.lo, in.hi, (shift)))
        #define LO(in) (in.lo)

    #elif defined(__GNUC__)
        #if defined(__SIZEOF_INT128__)
            PEDANTIC_EXTENSION typedef unsigned __int128 word128;
        #else
            typedef unsigned word128 __attribute__((mode(TI)));
        #endif

        #define MUL(out, x, y) out = ((word128)(x) * (y))
        #define ADD(out, in) (out) += (in)
        #define ADDLO(out, in) (out) += (in)
        #define SHR(in, shift) (word64)((in) >> (shift))
        #define LO(in) (word64)(in)
    #endif
#endif

#if defined(WOLFSSL_X86_64_BUILD) && defined(USE_INTEL_SPEEDUP)
#ifdef __cplusplus
    extern "C" {
#endif

#ifdef HAVE_INTEL_AVX1
/* Process one block (16 bytes) of data.
 *
 * ctx  Poly1305 context.
 * m    One block of message data.
 */
extern void poly1305_block_avx(Poly1305* ctx, const unsigned char *m);
/* Process multiple blocks (n * 16 bytes) of data.
 *
 * ctx    Poly1305 context.
 * m      Blocks of message data.
 * bytes  The number of bytes to process.
 */
extern void poly1305_blocks_avx(Poly1305* ctx, const unsigned char* m,
                                size_t bytes);
/* Set the key to use when processing data.
 * Initialize the context.
 *
 * ctx  Poly1305 context.
 * key  The key data (16 bytes).
 */
extern void poly1305_setkey_avx(Poly1305* ctx, const byte* key);
/* Calculate the final result - authentication data.
 * Zeros out the private data in the context.
 *
 * ctx  Poly1305 context.
 * mac  Buffer to hold 16 bytes.
 */
extern void poly1305_final_avx(Poly1305* ctx, byte* mac);
#endif

#ifdef HAVE_INTEL_AVX2
/* Process multiple blocks (n * 16 bytes) of data.
 *
 * ctx    Poly1305 context.
 * m      Blocks of message data.
 * bytes  The number of bytes to process.
 */
extern void poly1305_blocks_avx2(Poly1305* ctx, const unsigned char* m,
                                 size_t bytes);
/* Calculate R^1, R^2, R^3 and R^4 and store them in the context.
 *
 * ctx    Poly1305 context.
 */
extern void poly1305_calc_powers_avx2(Poly1305* ctx);
/* Set the key to use when processing data.
 * Initialize the context.
 * Calls AVX set key function as final function calls AVX code.
 *
 * ctx  Poly1305 context.
 * key  The key data (16 bytes).
 */
extern void poly1305_setkey_avx2(Poly1305* ctx, const byte* key);
/* Calculate the final result - authentication data.
 * Zeros out the private data in the context.
 * Calls AVX final function to quickly process last blocks.
 *
 * ctx  Poly1305 context.
 * mac  Buffer to hold 16 bytes - authentication data.
 */
extern void poly1305_final_avx2(Poly1305* ctx, byte* mac);
#endif

#ifdef __cplusplus
    }  /* extern "C" */
#endif

#elif defined(POLY130564)
#ifndef WOLFSSL_ARMASM
    static word64 U8TO64(const byte* p)
    {
        return
            (((word64)(p[0] & 0xff)      ) |
             ((word64)(p[1] & 0xff) <<  8) |
             ((word64)(p[2] & 0xff) << 16) |
             ((word64)(p[3] & 0xff) << 24) |
             ((word64)(p[4] & 0xff) << 32) |
             ((word64)(p[5] & 0xff) << 40) |
             ((word64)(p[6] & 0xff) << 48) |
             ((word64)(p[7] & 0xff) << 56));
    }

    static void U64TO8(byte* p, word64 v) {
        p[0] = (byte)v;
        p[1] = (byte)(v >>  8);
        p[2] = (byte)(v >> 16);
        p[3] = (byte)(v >> 24);
        p[4] = (byte)(v >> 32);
        p[5] = (byte)(v >> 40);
        p[6] = (byte)(v >> 48);
        p[7] = (byte)(v >> 56);
    }
#endif/* WOLFSSL_ARMASM */
#else /* if not 64 bit then use 32 bit */

    static word32 U8TO32(const byte *p)
    {
        return
            (((word32)(p[0] & 0xff)      ) |
             ((word32)(p[1] & 0xff) <<  8) |
             ((word32)(p[2] & 0xff) << 16) |
             ((word32)(p[3] & 0xff) << 24));
    }

    static void U32TO8(byte *p, word32 v) {
        p[0] = (byte)((v      ) & 0xff);
        p[1] = (byte)((v >>  8) & 0xff);
        p[2] = (byte)((v >> 16) & 0xff);
        p[3] = (byte)((v >> 24) & 0xff);
    }
#endif

/* convert 32-bit unsigned to little endian 64 bit type as byte array */
static WC_INLINE void u32tole64(const word32 inLe32, byte outLe64[8])
{
#ifndef WOLFSSL_X86_64_BUILD
    outLe64[0] = (byte)(inLe32  & 0x000000FF);
    outLe64[1] = (byte)((inLe32 & 0x0000FF00) >> 8);
    outLe64[2] = (byte)((inLe32 & 0x00FF0000) >> 16);
    outLe64[3] = (byte)((inLe32 & 0xFF000000) >> 24);
    outLe64[4] = 0;
    outLe64[5] = 0;
    outLe64[6] = 0;
    outLe64[7] = 0;
#else
    *(word64*)outLe64 = inLe32;
#endif
}


#if !defined(WOLFSSL_ARMASM) || !defined(__aarch64__)
/*
This local function operates on a message with a given number of bytes
with a given ctx pointer to a Poly1305 structure.
*/
static int poly1305_blocks(Poly1305* ctx, const unsigned char *m,
                     size_t bytes)
{
#if defined(WOLFSSL_X86_64_BUILD) && defined(USE_INTEL_SPEEDUP)
    /* AVX2 is handled in wc_Poly1305Update. */
    SAVE_VECTOR_REGISTERS(return _svr_ret;);
    poly1305_blocks_avx(ctx, m, bytes);
    RESTORE_VECTOR_REGISTERS();
    return 0;
#elif defined(POLY130564)
    const word64 hibit = (ctx->finished) ? 0 : ((word64)1 << 40); /* 1 << 128 */
    word64 r0,r1,r2;
    word64 s1,s2;
    word64 h0,h1,h2;
    word64 c;
    word128 d0,d1,d2,d;

    r0 = ctx->r[0];
    r1 = ctx->r[1];
    r2 = ctx->r[2];

    h0 = ctx->h[0];
    h1 = ctx->h[1];
    h2 = ctx->h[2];

    s1 = r1 * (5 << 2);
    s2 = r2 * (5 << 2);

    while (bytes >= POLY1305_BLOCK_SIZE) {
        word64 t0,t1;

        /* h += m[i] */
        t0 = U8TO64(&m[0]);
        t1 = U8TO64(&m[8]);

        h0 += (( t0                    ) & 0xfffffffffff);
        h1 += (((t0 >> 44) | (t1 << 20)) & 0xfffffffffff);
        h2 += (((t1 >> 24)             ) & 0x3ffffffffff) | hibit;

        /* h *= r */
        MUL(d0, h0, r0); MUL(d, h1, s2); ADD(d0, d); MUL(d, h2, s1); ADD(d0, d);
        MUL(d1, h0, r1); MUL(d, h1, r0); ADD(d1, d); MUL(d, h2, s2); ADD(d1, d);
        MUL(d2, h0, r2); MUL(d, h1, r1); ADD(d2, d); MUL(d, h2, r0); ADD(d2, d);

        /* (partial) h %= p */
                      c = SHR(d0, 44); h0 = LO(d0) & 0xfffffffffff;
        ADDLO(d1, c); c = SHR(d1, 44); h1 = LO(d1) & 0xfffffffffff;
        ADDLO(d2, c); c = SHR(d2, 42); h2 = LO(d2) & 0x3ffffffffff;
        h0  += c * 5; c = (h0 >> 44);  h0 =    h0  & 0xfffffffffff;
        h1  += c;

        m += POLY1305_BLOCK_SIZE;
        bytes -= POLY1305_BLOCK_SIZE;
    }

    ctx->h[0] = h0;
    ctx->h[1] = h1;
    ctx->h[2] = h2;

    return 0;

#else /* if not 64 bit then use 32 bit */
    const word32 hibit = (ctx->finished) ? 0 : ((word32)1 << 24); /* 1 << 128 */
    word32 r0,r1,r2,r3,r4;
    word32 s1,s2,s3,s4;
    word32 h0,h1,h2,h3,h4;
    word64 d0,d1,d2,d3,d4;
    word32 c;


    r0 = ctx->r[0];
    r1 = ctx->r[1];
    r2 = ctx->r[2];
    r3 = ctx->r[3];
    r4 = ctx->r[4];

    s1 = r1 * 5;
    s2 = r2 * 5;
    s3 = r3 * 5;
    s4 = r4 * 5;

    h0 = ctx->h[0];
    h1 = ctx->h[1];
    h2 = ctx->h[2];
    h3 = ctx->h[3];
    h4 = ctx->h[4];

    while (bytes >= POLY1305_BLOCK_SIZE) {
        /* h += m[i] */
        h0 += (U8TO32(m+ 0)     ) & 0x3ffffff;
        h1 += (U8TO32(m+ 3) >> 2) & 0x3ffffff;
        h2 += (U8TO32(m+ 6) >> 4) & 0x3ffffff;
        h3 += (U8TO32(m+ 9) >> 6) & 0x3ffffff;
        h4 += (U8TO32(m+12) >> 8) | hibit;

        /* h *= r */
        d0 = ((word64)h0 * r0) + ((word64)h1 * s4) + ((word64)h2 * s3) +
             ((word64)h3 * s2) + ((word64)h4 * s1);
        d1 = ((word64)h0 * r1) + ((word64)h1 * r0) + ((word64)h2 * s4) +
             ((word64)h3 * s3) + ((word64)h4 * s2);
        d2 = ((word64)h0 * r2) + ((word64)h1 * r1) + ((word64)h2 * r0) +
             ((word64)h3 * s4) + ((word64)h4 * s3);
        d3 = ((word64)h0 * r3) + ((word64)h1 * r2) + ((word64)h2 * r1) +
             ((word64)h3 * r0) + ((word64)h4 * s4);
        d4 = ((word64)h0 * r4) + ((word64)h1 * r3) + ((word64)h2 * r2) +
             ((word64)h3 * r1) + ((word64)h4 * r0);

        /* (partial) h %= p */
                      c = (word32)(d0 >> 26); h0 = (word32)d0 & 0x3ffffff;
        d1 += c;      c = (word32)(d1 >> 26); h1 = (word32)d1 & 0x3ffffff;
        d2 += c;      c = (word32)(d2 >> 26); h2 = (word32)d2 & 0x3ffffff;
        d3 += c;      c = (word32)(d3 >> 26); h3 = (word32)d3 & 0x3ffffff;
        d4 += c;      c = (word32)(d4 >> 26); h4 = (word32)d4 & 0x3ffffff;
        h0 += c * 5;  c =  (h0 >> 26); h0 =                h0 & 0x3ffffff;
        h1 += c;

        m += POLY1305_BLOCK_SIZE;
        bytes -= POLY1305_BLOCK_SIZE;
    }

    ctx->h[0] = h0;
    ctx->h[1] = h1;
    ctx->h[2] = h2;
    ctx->h[3] = h3;
    ctx->h[4] = h4;

    return 0;

#endif /* end of 64 bit cpu blocks or 32 bit cpu */
}

/*
This local function is used for the last call when a message with a given
number of bytes is less than the block size.
*/
static int poly1305_block(Poly1305* ctx, const unsigned char *m)
{
#if defined(WOLFSSL_X86_64_BUILD) && defined(USE_INTEL_SPEEDUP)
    /* No call to poly1305_block when AVX2, AVX2 does 4 blocks at a time. */
    SAVE_VECTOR_REGISTERS(return _svr_ret;);
    poly1305_block_avx(ctx, m);
    RESTORE_VECTOR_REGISTERS();
    return 0;
#else
    return poly1305_blocks(ctx, m, POLY1305_BLOCK_SIZE);
#endif
}
#endif /* !defined(WOLFSSL_ARMASM) || !defined(__aarch64__) */

#if !defined(WOLFSSL_ARMASM) || !defined(__aarch64__)
int wc_Poly1305SetKey(Poly1305* ctx, const byte* key, word32 keySz)
{
#if defined(POLY130564) && \
    !(defined(WOLFSSL_X86_64_BUILD) && defined(USE_INTEL_SPEEDUP))
    word64 t0,t1;
#endif

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

#if defined(WOLFSSL_X86_64_BUILD) && defined(USE_INTEL_SPEEDUP)
    if (!cpu_flags_set) {
        intel_flags = cpuid_get_flags();
        cpu_flags_set = 1;
    }
    SAVE_VECTOR_REGISTERS(return _svr_ret;);
    #ifdef HAVE_INTEL_AVX2
    if (IS_INTEL_AVX2(intel_flags))
        poly1305_setkey_avx2(ctx, key);
    else
    #endif
        poly1305_setkey_avx(ctx, key);
    RESTORE_VECTOR_REGISTERS();
#elif defined(POLY130564)

    /* r &= 0xffffffc0ffffffc0ffffffc0fffffff */
    t0 = U8TO64(key + 0);
    t1 = U8TO64(key + 8);

    ctx->r[0] = ( t0                    ) & 0xffc0fffffff;
    ctx->r[1] = ((t0 >> 44) | (t1 << 20)) & 0xfffffc0ffff;
    ctx->r[2] = ((t1 >> 24)             ) & 0x00ffffffc0f;

    /* h (accumulator) = 0 */
    ctx->h[0] = 0;
    ctx->h[1] = 0;
    ctx->h[2] = 0;

    /* save pad for later */
    ctx->pad[0] = U8TO64(key + 16);
    ctx->pad[1] = U8TO64(key + 24);

    ctx->leftover = 0;
    ctx->finished = 0;

#else /* if not 64 bit then use 32 bit */

    /* r &= 0xffffffc0ffffffc0ffffffc0fffffff */
    ctx->r[0] = (U8TO32(key +  0)     ) & 0x3ffffff;
    ctx->r[1] = (U8TO32(key +  3) >> 2) & 0x3ffff03;
    ctx->r[2] = (U8TO32(key +  6) >> 4) & 0x3ffc0ff;
    ctx->r[3] = (U8TO32(key +  9) >> 6) & 0x3f03fff;
    ctx->r[4] = (U8TO32(key + 12) >> 8) & 0x00fffff;

    /* h = 0 */
    ctx->h[0] = 0;
    ctx->h[1] = 0;
    ctx->h[2] = 0;
    ctx->h[3] = 0;
    ctx->h[4] = 0;

    /* save pad for later */
    ctx->pad[0] = U8TO32(key + 16);
    ctx->pad[1] = U8TO32(key + 20);
    ctx->pad[2] = U8TO32(key + 24);
    ctx->pad[3] = U8TO32(key + 28);

    ctx->leftover = 0;
    ctx->finished = 0;

#endif

    return 0;
}

int wc_Poly1305Final(Poly1305* ctx, byte* mac)
{
#if defined(WOLFSSL_X86_64_BUILD) && defined(USE_INTEL_SPEEDUP)
#elif defined(POLY130564)

    word64 h0,h1,h2,c;
    word64 g0,g1,g2;
    word64 t0,t1;

#else

    word32 h0,h1,h2,h3,h4,c;
    word32 g0,g1,g2,g3,g4;
    word64 f;
    word32 mask;

#endif

    if (ctx == NULL || mac == NULL)
        return BAD_FUNC_ARG;

#if defined(WOLFSSL_X86_64_BUILD) && defined(USE_INTEL_SPEEDUP)
    SAVE_VECTOR_REGISTERS(return _svr_ret;);
    #ifdef HAVE_INTEL_AVX2
    if (IS_INTEL_AVX2(intel_flags))
        poly1305_final_avx2(ctx, mac);
    else
    #endif
        poly1305_final_avx(ctx, mac);
    RESTORE_VECTOR_REGISTERS();
#elif defined(POLY130564)

    /* process the remaining block */
    if (ctx->leftover) {
        size_t i = ctx->leftover;
        ctx->buffer[i] = 1;
        for (i = i + 1; i < POLY1305_BLOCK_SIZE; i++)
            ctx->buffer[i] = 0;
        ctx->finished = 1;
        poly1305_block(ctx, ctx->buffer);
    }

    /* fully carry h */
    h0 = ctx->h[0];
    h1 = ctx->h[1];
    h2 = ctx->h[2];

                 c = (h1 >> 44); h1 &= 0xfffffffffff;
    h2 += c;     c = (h2 >> 42); h2 &= 0x3ffffffffff;
    h0 += c * 5; c = (h0 >> 44); h0 &= 0xfffffffffff;
    h1 += c;     c = (h1 >> 44); h1 &= 0xfffffffffff;
    h2 += c;     c = (h2 >> 42); h2 &= 0x3ffffffffff;
    h0 += c * 5; c = (h0 >> 44); h0 &= 0xfffffffffff;
    h1 += c;

    /* compute h + -p */
    g0 = h0 + 5; c = (g0 >> 44); g0 &= 0xfffffffffff;
    g1 = h1 + c; c = (g1 >> 44); g1 &= 0xfffffffffff;
    g2 = h2 + c - ((word64)1 << 42);

    /* select h if h < p, or h + -p if h >= p */
    c = (g2 >> ((sizeof(word64) * 8) - 1)) - 1;
    g0 &= c;
    g1 &= c;
    g2 &= c;
    c = ~c;
    h0 = (h0 & c) | g0;
    h1 = (h1 & c) | g1;
    h2 = (h2 & c) | g2;

    /* h = (h + pad) */
    t0 = ctx->pad[0];
    t1 = ctx->pad[1];

    h0 += (( t0                    ) & 0xfffffffffff)    ;
    c = (h0 >> 44); h0 &= 0xfffffffffff;
    h1 += (((t0 >> 44) | (t1 << 20)) & 0xfffffffffff) + c;
    c = (h1 >> 44); h1 &= 0xfffffffffff;
    h2 += (((t1 >> 24)             ) & 0x3ffffffffff) + c;
    h2 &= 0x3ffffffffff;

    /* mac = h % (2^128) */
    h0 = ((h0      ) | (h1 << 44));
    h1 = ((h1 >> 20) | (h2 << 24));

    U64TO8(mac + 0, h0);
    U64TO8(mac + 8, h1);

    /* zero out the state */
    ctx->h[0] = 0;
    ctx->h[1] = 0;
    ctx->h[2] = 0;
    ctx->r[0] = 0;
    ctx->r[1] = 0;
    ctx->r[2] = 0;
    ctx->pad[0] = 0;
    ctx->pad[1] = 0;

#else /* if not 64 bit then use 32 bit */

    /* process the remaining block */
    if (ctx->leftover) {
        size_t i = ctx->leftover;
        ctx->buffer[i++] = 1;
        for (; i < POLY1305_BLOCK_SIZE; i++)
            ctx->buffer[i] = 0;
        ctx->finished = 1;
        poly1305_block(ctx, ctx->buffer);
    }

    /* fully carry h */
    h0 = ctx->h[0];
    h1 = ctx->h[1];
    h2 = ctx->h[2];
    h3 = ctx->h[3];
    h4 = ctx->h[4];

                 c = h1 >> 26; h1 = h1 & 0x3ffffff;
    h2 +=     c; c = h2 >> 26; h2 = h2 & 0x3ffffff;
    h3 +=     c; c = h3 >> 26; h3 = h3 & 0x3ffffff;
    h4 +=     c; c = h4 >> 26; h4 = h4 & 0x3ffffff;
    h0 += c * 5; c = h0 >> 26; h0 = h0 & 0x3ffffff;
    h1 +=     c;

    /* compute h + -p */
    g0 = h0 + 5; c = g0 >> 26; g0 &= 0x3ffffff;
    g1 = h1 + c; c = g1 >> 26; g1 &= 0x3ffffff;
    g2 = h2 + c; c = g2 >> 26; g2 &= 0x3ffffff;
    g3 = h3 + c; c = g3 >> 26; g3 &= 0x3ffffff;
    g4 = h4 + c - ((word32)1 << 26);

    /* select h if h < p, or h + -p if h >= p */
    mask = ((word32)g4 >> ((sizeof(word32) * 8) - 1)) - 1;
    g0 &= mask;
    g1 &= mask;
    g2 &= mask;
    g3 &= mask;
    g4 &= mask;
    mask = ~mask;
    h0 = (h0 & mask) | g0;
    h1 = (h1 & mask) | g1;
    h2 = (h2 & mask) | g2;
    h3 = (h3 & mask) | g3;
    h4 = (h4 & mask) | g4;

    /* h = h % (2^128) */
    h0 = ((h0      ) | (h1 << 26)) & 0xffffffff;
    h1 = ((h1 >>  6) | (h2 << 20)) & 0xffffffff;
    h2 = ((h2 >> 12) | (h3 << 14)) & 0xffffffff;
    h3 = ((h3 >> 18) | (h4 <<  8)) & 0xffffffff;

    /* mac = (h + pad) % (2^128) */
    f = (word64)h0 + ctx->pad[0]            ; h0 = (word32)f;
    f = (word64)h1 + ctx->pad[1] + (f >> 32); h1 = (word32)f;
    f = (word64)h2 + ctx->pad[2] + (f >> 32); h2 = (word32)f;
    f = (word64)h3 + ctx->pad[3] + (f >> 32); h3 = (word32)f;

    U32TO8(mac + 0, h0);
    U32TO8(mac + 4, h1);
    U32TO8(mac + 8, h2);
    U32TO8(mac + 12, h3);

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
    ctx->pad[0] = 0;
    ctx->pad[1] = 0;
    ctx->pad[2] = 0;
    ctx->pad[3] = 0;

#endif

    return 0;
}
#endif /* !defined(WOLFSSL_ARMASM) || !defined(__aarch64__) */


int wc_Poly1305Update(Poly1305* ctx, const byte* m, word32 bytes)
{
    size_t i;

    if (ctx == NULL || (m == NULL && bytes > 0))
        return BAD_FUNC_ARG;

    if (bytes == 0) {
        /* valid, but do nothing */
        return 0;
    }
#ifdef CHACHA_AEAD_TEST
    word32 k;
    printf("Raw input to poly:\n");
    for (k = 0; k < bytes; k++) {
        printf("%02x", m[k]);
        if ((k+1) % 16 == 0)
            printf("\n");
    }
    printf("\n");
#endif

#if defined(WOLFSSL_X86_64_BUILD) && defined(USE_INTEL_SPEEDUP)
    #ifdef HAVE_INTEL_AVX2
    if (IS_INTEL_AVX2(intel_flags)) {
        SAVE_VECTOR_REGISTERS(return _svr_ret;);

        /* handle leftover */

        if (ctx->leftover) {
            size_t want = sizeof(ctx->buffer) - ctx->leftover;
            if (want > bytes)
                want = bytes;

            for (i = 0; i < want; i++)
                ctx->buffer[ctx->leftover + i] = m[i];
            bytes -= (word32)want;
            m += want;
            ctx->leftover += want;
            if (ctx->leftover < sizeof(ctx->buffer)) {
                RESTORE_VECTOR_REGISTERS();
                return 0;
            }

            if (!ctx->started)
                poly1305_calc_powers_avx2(ctx);
            poly1305_blocks_avx2(ctx, ctx->buffer, sizeof(ctx->buffer));
            ctx->leftover = 0;
        }

        /* process full blocks */
        if (bytes >= sizeof(ctx->buffer)) {
            size_t want = bytes & ~(sizeof(ctx->buffer) - 1);

            if (!ctx->started)
                poly1305_calc_powers_avx2(ctx);
            poly1305_blocks_avx2(ctx, m, want);
            m += want;
            bytes -= (word32)want;
        }

        /* store leftover */
        if (bytes) {
            for (i = 0; i < bytes; i++)
                ctx->buffer[ctx->leftover + i] = m[i];
            ctx->leftover += bytes;
        }
        RESTORE_VECTOR_REGISTERS();
    }
    else
    #endif
#endif
    {
        /* handle leftover */
        if (ctx->leftover) {
            size_t want = (POLY1305_BLOCK_SIZE - ctx->leftover);
            if (want > bytes)
                want = bytes;
            for (i = 0; i < want; i++)
                ctx->buffer[ctx->leftover + i] = m[i];
            bytes -= (word32)want;
            m += want;
            ctx->leftover += want;
            if (ctx->leftover < POLY1305_BLOCK_SIZE)
                return 0;
            poly1305_block(ctx, ctx->buffer);
            ctx->leftover = 0;
        }

        /* process full blocks */
        if (bytes >= POLY1305_BLOCK_SIZE) {
            size_t want = ((size_t)bytes & ~((size_t)POLY1305_BLOCK_SIZE - 1));
#if !defined(WOLFSSL_ARMASM) || !defined(__aarch64__)
            int ret;
            ret = poly1305_blocks(ctx, m, want);
            if (ret != 0)
                return ret;
#else
            poly1305_blocks(ctx, m, want);
#endif
            m += want;
            bytes -= (word32)want;
        }

        /* store leftover */
        if (bytes) {
            for (i = 0; i < bytes; i++)
                ctx->buffer[ctx->leftover + i] = m[i];
            ctx->leftover += bytes;
        }
    }

    return 0;
}

/*  Takes a Poly1305 struct that has a key loaded and pads the provided length
    ctx        : Initialized Poly1305 struct to use
    lenToPad   : Current number of bytes updated that needs padding to 16
 */
int wc_Poly1305_Pad(Poly1305* ctx, word32 lenToPad)
{
    int ret = 0;
    word32 paddingLen;
    byte padding[WC_POLY1305_PAD_SZ - 1];

    if (ctx == NULL) {
        return BAD_FUNC_ARG;
    }
    if (lenToPad == 0) {
        return 0; /* nothing needs to be done */
    }

    XMEMSET(padding, 0, sizeof(padding));

    /* Pad length to 16 bytes */
    paddingLen = (-(int)lenToPad) & (WC_POLY1305_PAD_SZ - 1);
    if ((paddingLen > 0) && (paddingLen < WC_POLY1305_PAD_SZ)) {
        ret = wc_Poly1305Update(ctx, padding, paddingLen);
    }
    return ret;
}

/*  Takes a Poly1305 struct that has a key loaded and adds the AEAD length
    encoding in 64-bit little endian
    aadSz      : Size of the additional authentication data
    dataSz     : Size of the plaintext or ciphertext
 */
int wc_Poly1305_EncodeSizes(Poly1305* ctx, word32 aadSz, word32 dataSz)
{
    int ret;
    byte little64[16]; /* sizeof(word64) * 2 */

    if (ctx == NULL) {
        return BAD_FUNC_ARG;
    }

    XMEMSET(little64, 0, sizeof(little64));

    /* size of additional data and input data as little endian 64 bit types */
    u32tole64(aadSz,  little64);
    u32tole64(dataSz, little64 + 8);
    ret = wc_Poly1305Update(ctx, little64, sizeof(little64));

    return ret;
}

#ifdef WORD64_AVAILABLE
int wc_Poly1305_EncodeSizes64(Poly1305* ctx, word64 aadSz, word64 dataSz)
{
    int ret;
    word64 little64[2];

    if (ctx == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef BIG_ENDIAN_ORDER
    little64[0] = ByteReverseWord64(aadSz);
    little64[1] = ByteReverseWord64(dataSz);
#else
    little64[0] = aadSz;
    little64[1] = dataSz;
#endif

    ret = wc_Poly1305Update(ctx, (byte *)little64, sizeof(little64));

    return ret;
}
#endif

/*  Takes in an initialized Poly1305 struct that has a key loaded and creates
    a MAC (tag) using recent TLS AEAD padding scheme.
    ctx        : Initialized Poly1305 struct to use
    additional : Additional data to use
    addSz      : Size of additional buffer
    input      : Input buffer to create tag from
    sz         : Size of input buffer
    tag        : Buffer to hold created tag
    tagSz      : Size of input tag buffer (must be at least
                 WC_POLY1305_MAC_SZ(16))
 */
int wc_Poly1305_MAC(Poly1305* ctx, const byte* additional, word32 addSz,
                    const byte* input, word32 sz, byte* tag, word32 tagSz)
{
    int ret;

    /* sanity check on arguments */
    if (ctx == NULL || input == NULL || tag == NULL ||
                                                   tagSz < WC_POLY1305_MAC_SZ) {
        return BAD_FUNC_ARG;
    }

    /* additional allowed to be 0 */
    if (addSz > 0) {
        if (additional == NULL)
            return BAD_FUNC_ARG;

        /* additional data plus padding */
        if ((ret = wc_Poly1305Update(ctx, additional, addSz)) != 0) {
            return ret;
        }
        /* pad additional data */
        if ((ret = wc_Poly1305_Pad(ctx, addSz)) != 0) {
            return ret;
        }
    }

    /* input plus padding */
    if ((ret = wc_Poly1305Update(ctx, input, sz)) != 0) {
        return ret;
    }
    /* pad input data */
    if ((ret = wc_Poly1305_Pad(ctx, sz)) != 0) {
        return ret;
    }

    /* encode size of AAD and input data as little endian 64 bit types */
    if ((ret = wc_Poly1305_EncodeSizes(ctx, addSz, sz)) != 0) {
        return ret;
    }

    /* Finalize the auth tag */
    ret = wc_Poly1305Final(ctx, tag);

    return ret;

}
#endif /* HAVE_POLY1305 */
