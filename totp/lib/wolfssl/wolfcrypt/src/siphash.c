/* siphash.c
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
#endif

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/types.h>

#include <wolfssl/wolfcrypt/siphash.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif


/* DESCRIPTION
 *
 * SipHash is a PseudoRandom Function (PRF) that can be used with small
 * messages (less than 256 bytes).
 * SipHash can be used for Message Authentication Codes (MACs) and as such must
 * be passed a secret key.
 * https://eprint.iacr.org/2012/351.pdf
 *
 * SipHash is commonly used in hash tables.
 * Do not use this as a hash not as a general purpose MAC.
 *
 * WOLFSSL_SIPHASH_CROUNDS and WOLFSSL_SIPHASH_DROUNDS can be defined at build
 * time to change the algorithm.
 * Default is SipHash-2-4:
 *   WOLFSSL_SIPHASH_CROUNDS = 2
 *   WOLFSSL_SIPHASH_DROUNDS = 4
 *
 * Inline assembly implementations of wc_SipHash() written for:
 *   - GCC for Intel x86_64
 *   - GCC for Aarch64.
 */

#ifdef WOLFSSL_SIPHASH

#ifdef LITTLE_ENDIAN_ORDER
/**
 * Decode little-endian byte array to 64-bit number.
 *
 * @param [in] a  Little-endian byte array.
 * @return 64-bit number.
 */
#define GET_U64(a)      (*(word64*)(a))
/**
 * Decode little-endian byte array to 32-bit number.
 *
 * @param [in] a  Little-endian byte array.
 * @return 32-bit number.
 */
#define GET_U32(a)      (*(word32*)(a))
/**
 * Decode little-endian byte array to 16-bit number.
 *
 * @param [in] a  Little-endian byte array.
 * @return 16-bit number.
 */
#define GET_U16(a)      (*(word16*)(a))
/**
 * Encode 64-bit number to a little-endian byte array.
 *
 * @param [out] a  Byte array to write into.
 * @param [in]  n  Number to encode.
 */
#define SET_U64(a, n)   ((*(word64*)(a)) = (n))
#else
/**
 * Decode little-endian byte array to 64-bit number.
 *
 * @param [in] a  Little-endian byte array.
 * @return 64-bit number.
 */
#define GET_U64(a)      (((word64)((a)[7]) << 56) |     \
                         ((word64)((a)[6]) << 48) |     \
                         ((word64)((a)[5]) << 40) |     \
                         ((word64)((a)[4]) << 32) |     \
                         ((word64)((a)[3]) << 24) |     \
                         ((word64)((a)[2]) << 16) |     \
                         ((word64)((a)[1]) <<  8) |     \
                         ((word64)((a)[0])      ))
/**
 * Decode little-endian byte array to 32-bit number.
 *
 * @param [in] a  Little-endian byte array.
 * @return 32-bit number.
 */
#define GET_U32(a)      (((word64)((a)[3]) << 24) |     \
                         ((word32)((a)[2]) << 16) |     \
                         ((word32)((a)[1]) <<  8) |     \
                         ((word32)((a)[0])      ))
/**
 * Decode little-endian byte array to 16-bit number.
 *
 * @param [in] a  Little-endian byte array.
 * @return 16-bit number.
 */
#define GET_U16(a)      (((word16)((a)[1]) <<  8) |     \
                         ((word16)((a)[0])      ))
/**
 * Encode 64-bit number to a little-endian byte array.
 *
 * @param [out] a  Byte array to write into.
 * @param [in]  n  Number to encode.
 */
#define SET_U64(a, n)   (a)[0] = (byte)((n)      );     \
                        (a)[1] = (byte)((n) >>  8);     \
                        (a)[2] = (byte)((n) >> 16);     \
                        (a)[3] = (byte)((n) >> 24);     \
                        (a)[4] = (byte)((n) >> 32);     \
                        (a)[5] = (byte)((n) >> 40);     \
                        (a)[6] = (byte)((n) >> 48);     \
                        (a)[7] = (byte)((n) >> 56)
#endif

/**
 * Initialize SipHash operation with a key.
 *
 * @param [out] sipHash  SipHash object.
 * @param [in]  key      16 byte array - little endian.
 * @return  BAD_FUNC_ARG when sipHash or key is NULL.
 * @return  BAD_FUNC_ARG when outSz is neither 8 nor 16.
 * @return  0 on success.
 */
int wc_InitSipHash(SipHash* sipHash, const unsigned char* key,
    unsigned char outSz)
{
    int ret = 0;

    /* Validate parameters. */
    if ((sipHash == NULL) || (key == NULL) ||
        ((outSz != SIPHASH_MAC_SIZE_8) && (outSz != SIPHASH_MAC_SIZE_16))) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        word64 k0 = GET_U64(key + 0);
        word64 k1 = GET_U64(key + 8);

        /* Initialize state with key. */
        sipHash->v[0] = W64LIT(0x736f6d6570736575);
        if (outSz == SIPHASH_MAC_SIZE_8) {
            sipHash->v[1] = W64LIT(0x646f72616e646f6d);
        }
        else {
            sipHash->v[1] = W64LIT(0x646f72616e646f83);
        }
        sipHash->v[2] = W64LIT(0x6c7967656e657261);
        sipHash->v[3] = W64LIT(0x7465646279746573);

        sipHash->v[0] ^= k0;
        sipHash->v[1] ^= k1;
        sipHash->v[2] ^= k0;
        sipHash->v[3] ^= k1;

        /* No cached message bytes. */
        sipHash->cacheCnt = 0;
        /* No message bytes compressed yet. */
        sipHash->inCnt = 0;
        /* Keep the output size to check against final call. */
        sipHash->outSz = outSz;
    }

    return ret;
}

/**
 * One round of SipHash.
 *
 * @param [in, out] sipHash  SipHash object.
 */
static WC_INLINE void SipRound(SipHash *sipHash)
{
    word64* v = sipHash->v;

    v[0] += v[1];
    v[2] += v[3];
    v[1] = rotlFixed64(v[1], 13);
    v[3] = rotlFixed64(v[3], 16);
    v[1] ^= v[0];
    v[3] ^= v[2];
    v[0] = rotlFixed64(v[0], 32);
    v[2] += v[1];
    v[0] += v[3];
    v[1] = rotlFixed64(v[1], 17);
    v[3] = rotlFixed64(v[3], 21);
    v[1] ^= v[2];
    v[3] ^= v[0];
    v[2] = rotlFixed64(v[2], 32);
}

/**
 * One step of the compression operation.
 *
 * @param [in, out] sipHash  SipHash object.
 * @param [in]      m        Message to compress.
 */
static WC_INLINE void SipHashCompress(SipHash* sipHash, const byte* m)
{
    int i;

    sipHash->v[3] ^= GET_U64(m);
    for (i = 0; i < WOLFSSL_SIPHASH_CROUNDS; i++) {
        SipRound(sipHash);
    }
    sipHash->v[0] ^= GET_U64(m);
}

/**
 * Update the SipHash operation with more data.
 *
 * @param [in, out] sipHash  SipHash object.
 * @param [in]      in       Input message.
 * @param [in]      inSz     Size of input message.
 * @return  BAD_FUNC_ARG when sipHash is NULL.
 * @return  BAD_FUNC_ARG when in is NULL and inSz is not zero.
 * @return  0 on success.
 */
int wc_SipHashUpdate(SipHash* sipHash, const unsigned char* in, word32 inSz)
{
    int ret = 0;

    /* Validate parameters. */
    if ((sipHash == NULL) || ((in == NULL) && (inSz != 0))) {
        ret = BAD_FUNC_ARG;
    }

    /* Process any message bytes. */
    if ((ret == 0) && (inSz > 0)) {
        /* Add to cache if already started. */
        if (sipHash->cacheCnt > 0) {
            byte len = SIPHASH_BLOCK_SIZE - sipHash->cacheCnt;
            if (len > inSz) {
                len = (byte)inSz;
            }
            XMEMCPY(sipHash->cache + sipHash->cacheCnt, in, len);
            in += len;
            inSz -= len;
            sipHash->cacheCnt += len;

            if (sipHash->cacheCnt == SIPHASH_BLOCK_SIZE) {
                /* Compress the block from the cache. */
                SipHashCompress(sipHash, sipHash->cache);
                sipHash->inCnt += SIPHASH_BLOCK_SIZE;
                sipHash->cacheCnt = 0;
            }
        }

        /* Process more blocks from message. */
        while (inSz >= SIPHASH_BLOCK_SIZE) {
            /* Compress the next block from the message data. */
            SipHashCompress(sipHash, in);
            in += SIPHASH_BLOCK_SIZE;
            inSz -= SIPHASH_BLOCK_SIZE;
            sipHash->inCnt += SIPHASH_BLOCK_SIZE;
        }

        if (inSz > 0) {
            /* Cache remaining message bytes less than a block. */
            XMEMCPY(sipHash->cache, in, inSz);
            sipHash->cacheCnt = (byte)inSz;
        }
    }

    return ret;
}

/**
 * Calculate 8-bytes of output.
 *
 * @param [in, out] sipHash  SipHash object.
 * @param [out]     out      Buffer to place 8-bytes of MAC into.
 */
static WC_INLINE void SipHashOut(SipHash* sipHash, byte* out)
{
    word64 n;
    int i;

    for (i = 0; i < WOLFSSL_SIPHASH_DROUNDS; i++) {
        SipRound(sipHash);
    }
    n = sipHash->v[0] ^ sipHash->v[1] ^ sipHash->v[2] ^ sipHash->v[3];
    SET_U64(out, n);
}

/**
 * Finalize SipHash operation.
 *
 * @param [in, out] sipHash  SipHash object.
 * @param [out]     out      Buffer to place MAC into.
 * @param [in]      outSz    Size of output MAC. 8 or 16 only.
 * @return  BAD_FUNC_ARG when sipHash or out is NULL.
 * @return  BAD_FUNC_ARG when outSz is not the same as initialized value.
 * @return  0 on success.
 */
int wc_SipHashFinal(SipHash* sipHash, unsigned char* out, unsigned char outSz)
{
    int ret = 0;

    /* Validate parameters. */
    if ((sipHash == NULL) || (out == NULL) || (outSz != sipHash->outSz)) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        /* Put in remaining cached message bytes. */
        XMEMSET(sipHash->cache + sipHash->cacheCnt, 0, 7 - sipHash->cacheCnt);
        sipHash->cache[7] = (byte)(sipHash->inCnt + sipHash->cacheCnt);

        SipHashCompress(sipHash, sipHash->cache);
        sipHash->cacheCnt = 0;

        /* Output either 8 or 16 bytes. */
        if (outSz == SIPHASH_MAC_SIZE_8) {
            sipHash->v[2] ^= (word64)0xff;
            SipHashOut(sipHash, out);
        }
        else {
            sipHash->v[2] ^= (word64)0xee;
            SipHashOut(sipHash, out);
            sipHash->v[1] ^= (word64)0xdd;
            SipHashOut(sipHash, out + 8);
        }
    }

    return ret;
}

#if !defined(WOLFSSL_NO_ASM) && defined(__GNUC__) && defined(__x86_64__) && \
    (WOLFSSL_SIPHASH_CROUNDS == 1 || WOLFSSL_SIPHASH_CROUNDS == 2) && \
    (WOLFSSL_SIPHASH_DROUNDS == 2 || WOLFSSL_SIPHASH_DROUNDS == 4)

#define SIPHASH_ROUND(v0, v1, v2, v3)   \
        "addq   " #v1 ", " #v0 "\n\t"   \
        "addq   " #v3 ", " #v2 "\n\t"   \
        "rolq   $13, " #v1 "\n\t"       \
        "rolq   $16, " #v3 "\n\t"       \
        "xorq   " #v0 ", " #v1 "\n\t"   \
        "xorq   " #v2 ", " #v3 "\n\t"   \
        "rolq   $32, " #v0 "\n\t"       \
        "addq   " #v1 ", " #v2 "\n\t"   \
        "addq   " #v3 ", " #v0 "\n\t"   \
        "rolq   $17, " #v1 "\n\t"       \
        "rolq   $21, " #v3 "\n\t"       \
        "xorq   " #v2 ", " #v1 "\n\t"   \
        "xorq   " #v0 ", " #v3 "\n\t"   \
        "rolq   $32, " #v2 "\n\t"

#define SIPHASH_LAST_ROUND(v0, v1, v2, v3)  \
        "addq   " #v1 ", " #v0 "\n\t"       \
        "addq   " #v3 ", " #v2 "\n\t"       \
        "rolq   $13, " #v1 "\n\t"           \
        "rolq   $16, " #v3 "\n\t"           \
        "xorq   " #v0 ", " #v1 "\n\t"       \
        "xorq   " #v2 ", " #v3 "\n\t"       \
        "addq   " #v1 ", " #v2 "\n\t"       \
        "rolq   $17, " #v1 "\n\t"           \
        "rolq   $21, " #v3 "\n\t"           \
        "xorq   " #v2 ", " #v1 "\n\t"       \
        "rolq   $32, " #v2 "\n\t"

/**
 * Perform SipHash operation on input with key.
 *
 * @param [in]      key      16 byte array - little endian.
 * @param [in]      in       Input message.
 * @param [in]      inSz     Size of input message.
 * @param [out]     out      Buffer to place MAC into.
 * @param [in]      outSz    Size of output MAC. 8 or 16 only.
 * @return  BAD_FUNC_ARG when key or out is NULL.
 * @return  BAD_FUNC_ARG when in is NULL and inSz is not zero.
 * @return  BAD_FUNC_ARG when outSz is neither 8 nor 16.
 * @return  0 on success.
 */
int wc_SipHash(const unsigned char* key, const unsigned char* in, word32 inSz,
    unsigned char* out, unsigned char outSz)
{
    word64 v0 = 0x736f6d6570736575L;
    word64 v1 = 0x646f72616e646f6dL;
    word64 v2 = 0x6c7967656e657261L;
    word64 v3 = 0x7465646279746573L;
    word64 k0;
    word64 k1;

    if ((key == NULL) || ((in == NULL) && (inSz != 0)) || (out == NULL) ||
            ((outSz != SIPHASH_MAC_SIZE_8) && (outSz != SIPHASH_MAC_SIZE_16))) {
        return BAD_FUNC_ARG;
    }

    k0 = ((word64*)key)[0];
    k1 = ((word64*)key)[1];
    __asm__ __volatile__ (
        "xorq   %[k0], %[v0]\n\t"
        "xorq   %[k1], %[v1]\n\t"
        "xorq   %[k0], %[v2]\n\t"
        "xorq   %[k1], %[v3]\n\t"

        "cmp    $8, %[outSz]\n\t"
        "mov    %[inSz], %k[k1]\n\t"
        "je     L_siphash_8_top\n\t"
        "xorq   $0xee, %[v1]\n\t"
        "L_siphash_8_top:\n\t"

        "sub    $8, %[inSz]\n\t"
        "jb     L_siphash_done_input_8\n\t"
        "L_siphash_input:\n\t"
        "movq   (%[in]), %[k0]\n\t"
        "addq   $8, %[in]\n\t"
        "xorq   %[k0], %[v3]\n\t"
#if WOLFSSL_SIPHASH_CROUNDS == 1
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
#elif WOLFSSL_SIPHASH_CROUNDS == 2
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
#endif
        "xorq   %[k0], %[v0]\n\t"
        "sub    $8, %[inSz]\n\t"
        "jge    L_siphash_input\n\t"
        "L_siphash_done_input_8:\n\t"
        "add    $8, %[inSz]\n\t"

        "shlq   $56, %[k1]\n\t"
        "cmp    $0, %[inSz]\n\t"
        "je     L_siphash_last_done\n\t"
        "cmp    $4, %[inSz]\n\t"
        "jl     L_siphash_last_lt4\n\t"

        "cmp    $7, %[inSz]\n\t"
        "jl     L_siphash_n7\n\t"
        "movzbq 6(%[in]), %[k0]\n\t"
        "shlq   $48, %[k0]\n\t"
        "orq    %[k0], %[k1]\n\t"
        "L_siphash_n7:\n\t"

        "cmp    $6, %[inSz]\n\t"
        "jl     L_siphash_n6\n\t"
        "movzbq 5(%[in]), %[k0]\n\t"
        "shlq   $40, %[k0]\n\t"
        "orq    %[k0], %[k1]\n\t"
        "L_siphash_n6:\n\t"

        : [in] "+r" (in), [inSz] "+r" (inSz), [k0] "+r" (k0), [k1] "+r" (k1),
          [v0] "+r" (v0), [v1] "+r" (v1), [v2] "+r" (v2), [v3] "+r" (v3)
        : [key] "r" (key), [out] "r" (out) , [outSz] "r" (outSz)
        : "memory"
    );

    __asm__ __volatile__ (
        "cmp    $5, %[inSz]\n\t"
        "jl     L_siphash_n5\n\t"
        "movzbq 4(%[in]), %[k0]\n\t"
        "shlq   $32, %[k0]\n\t"
        "orq    %[k0], %[k1]\n\t"
        "L_siphash_n5:\n\t"

        "mov    (%[in]), %k[k0]\n\t"
        "orq    %[k0], %[k1]\n\t"
        "jmp    L_siphash_last_done\n\t"

        "L_siphash_last_lt4:\n\t"

        "cmp    $1, %[inSz]\n\t"
        "je     L_siphash_last_1\n\t"

        "cmp    $3, %[inSz]\n\t"
        "jl     L_siphash_n3\n\t"
        "movzbq 2(%[in]), %[k0]\n\t"
        "shlq   $16, %[k0]\n\t"
        "orq    %[k0], %[k1]\n\t"
        "L_siphash_n3:\n\t"

        "movw   (%[in]), %w[k0]\n\t"
        "or     %w[k0], %w[k1]\n\t"
        "jmp    L_siphash_last_done\n\t"

        "L_siphash_last_1:\n\t"
        "movb   (%[in]), %b[k0]\n\t"
        "or     %b[k0], %b[k1]\n\t"

        "L_siphash_last_done:\n\t"

        "xorq   %[k1], %[v3]\n\t"
#if WOLFSSL_SIPHASH_CROUNDS == 1
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
#elif WOLFSSL_SIPHASH_CROUNDS == 2
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
#endif
        "xorq   %[k1], %[v0]\n\t"

        "cmp    $8, %[outSz]\n\t"
        "je     L_siphash_8_end\n\t"

        : [in] "+r" (in), [inSz] "+r" (inSz), [k0] "+r" (k0), [k1] "+r" (k1),
          [v0] "+r" (v0), [v1] "+r" (v1), [v2] "+r" (v2), [v3] "+r" (v3)
        : [key] "r" (key), [out] "r" (out) , [outSz] "r" (outSz)
        : "memory"
    );

    __asm__ __volatile__ (
        "xor    $0xee, %b[v2]\n\t"
#if WOLFSSL_SIPHASH_DROUNDS == 2
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
#elif WOLFSSL_SIPHASH_DROUNDS == 4
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
#endif
        "movq   %[v0], %[k0]\n\t"
        "xorq   %[v1], %[k0]\n\t"
        "xorq   %[v2], %[k0]\n\t"
        "xorq   %[v3], %[k0]\n\t"
        "movq   %[k0], (%[out])\n\t"

        "xor    $0xdd, %b[v1]\n\t"
#if WOLFSSL_SIPHASH_DROUNDS == 2
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
        SIPHASH_LAST_ROUND(%[v0], %[v1], %[v2], %[v3])
#elif WOLFSSL_SIPHASH_DROUNDS == 4
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
        SIPHASH_LAST_ROUND(%[v0], %[v1], %[v2], %[v3])
#endif
        "xorq   %[v3], %[v1]\n\t"
        "xorq   %[v2], %[v1]\n\t"
        "movq   %[v1], 8(%[out])\n\t"
        "jmp    L_siphash_done\n\t"

        "L_siphash_8_end:\n\t"
        "xor    $0xff, %b[v2]\n\t"
#if WOLFSSL_SIPHASH_DROUNDS == 2
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
        SIPHASH_LAST_ROUND(%[v0], %[v1], %[v2], %[v3])
#elif WOLFSSL_SIPHASH_DROUNDS == 4
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
        SIPHASH_LAST_ROUND(%[v0], %[v1], %[v2], %[v3])
#endif
        "xorq   %[v3], %[v1]\n\t"
        "xorq   %[v2], %[v1]\n\t"
        "movq   %[v1], (%[out])\n\t"

        "L_siphash_done:\n\t"

        : [in] "+r" (in), [inSz] "+r" (inSz), [k0] "+r" (k0), [k1] "+r" (k1),
          [v0] "+r" (v0), [v1] "+r" (v1), [v2] "+r" (v2), [v3] "+r" (v3)
        : [key] "r" (key), [out] "r" (out) , [outSz] "r" (outSz)
        : "memory"
    );

    return 0;
}

#elif !defined(WOLFSSL_NO_ASM) && defined(__GNUC__) && defined(__aarch64__) && \
    (WOLFSSL_SIPHASH_CROUNDS == 1 || WOLFSSL_SIPHASH_CROUNDS == 2) && \
    (WOLFSSL_SIPHASH_DROUNDS == 2 || WOLFSSL_SIPHASH_DROUNDS == 4)

#define SIPHASH_ROUND(v0, v1, v2, v3)            \
        "add    " #v0 ", " #v0 ", " #v1 "\n\t"   \
        "add    " #v2 ", " #v2 ", " #v3 "\n\t"   \
        "ror    " #v1 ", " #v1 ", #51\n\t"       \
        "ror    " #v3 ", " #v3 ", #48\n\t"       \
        "eor    " #v1 ", " #v1 ", " #v0 "\n\t"   \
        "eor    " #v3 ", " #v3 ", " #v2 "\n\t"   \
        "ror    " #v0 ", " #v0 ", #32\n\t"       \
        "add    " #v2 ", " #v2 ", " #v1 "\n\t"   \
        "add    " #v0 ", " #v0 ", " #v3 "\n\t"   \
        "ror    " #v1 ", " #v1 ", #47\n\t"       \
        "ror    " #v3 ", " #v3 ", #43\n\t"       \
        "eor    " #v1 ", " #v1 ", " #v2 "\n\t"   \
        "eor    " #v3 ", " #v3 ", " #v0 "\n\t"   \
        "ror    " #v2 ", " #v2 ", #32\n\t"

#define SIPHASH_LAST_ROUND(v0, v1, v2, v3)       \
        "add    " #v0 ", " #v0 ", " #v1 "\n\t"   \
        "add    " #v2 ", " #v2 ", " #v3 "\n\t"   \
        "ror    " #v1 ", " #v1 ", #51\n\t"       \
        "ror    " #v3 ", " #v3 ", #48\n\t"       \
        "eor    " #v1 ", " #v1 ", " #v0 "\n\t"   \
        "eor    " #v3 ", " #v3 ", " #v2 "\n\t"   \
        "add    " #v2 ", " #v2 ", " #v1 "\n\t"   \
        "ror    " #v1 ", " #v1 ", #47\n\t"       \
        "ror    " #v3 ", " #v3 ", #43\n\t"       \
        "eor    " #v1 ", " #v1 ", " #v2 "\n\t"   \
        "ror    " #v2 ", " #v2 ", #32\n\t"

/**
 * Perform SipHash operation on input with key.
 *
 * @param [in]      key      16 byte array - little endian.
 * @param [in]      in       Input message.
 * @param [in]      inSz     Size of input message.
 * @param [out]     out      Buffer to place MAC into.
 * @param [in]      outSz    Size of output MAC. 8 or 16 only.
 * @return  BAD_FUNC_ARG when key or out is NULL.
 * @return  BAD_FUNC_ARG when in is NULL and inSz is not zero.
 * @return  BAD_FUNC_ARG when outSz is not 8 nor 16.
 * @return  0 on success.
 */
int wc_SipHash(const unsigned char* key, const unsigned char* in, word32 inSz,
    unsigned char* out, unsigned char outSz)
{
    word64 v0 = 0x736f6d6570736575L;
    word64 v1 = 0x646f72616e646f6dL;
    word64 v2 = 0x6c7967656e657261L;
    word64 v3 = 0x7465646279746573L;
    word64 k0;
    word64 k1;

    if ((key == NULL) || ((in == NULL) && (inSz != 0)) || (out == NULL) ||
            ((outSz != SIPHASH_MAC_SIZE_8) && (outSz != SIPHASH_MAC_SIZE_16))) {
        return BAD_FUNC_ARG;
    }

    k0 = ((word64*)key)[0];
    k1 = ((word64*)key)[1];
    __asm__ __volatile__ (
        "eor    %[v0], %[v0], %[k0]\n\t"
        "eor    %[v1], %[v1], %[k1]\n\t"
        "eor    %[v2], %[v2], %[k0]\n\t"
        "eor    %[v3], %[v3], %[k1]\n\t"

        "mov    %w[k1], %w[inSz]\n\t"
        "cmp    %w[outSz], #8\n\t"
        "b.eq   L_siphash_8_top\n\t"
        "mov    %w[k0], #0xee\n\t"
        "eor    %[v1], %[v1], %[k0]\n\t"
        "L_siphash_8_top:\n\t"

        "subs   %w[inSz], %w[inSz], #8\n\t"
        "b.mi   L_siphash_done_input_8\n\t"
        "L_siphash_input:\n\t"
        "ldr    %[k0], [%[in]], #8\n\t"
        "eor    %[v3], %[v3], %[k0]\n\t"
#if WOLFSSL_SIPHASH_CROUNDS == 1
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
#elif WOLFSSL_SIPHASH_CROUNDS == 2
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
#endif
        "eor    %[v0], %[v0], %[k0]\n\t"
        "subs   %w[inSz], %w[inSz], #8\n\t"
        "b.ge   L_siphash_input\n\t"
        "L_siphash_done_input_8:\n\t"
        "add    %w[inSz], %w[inSz], #8\n\t"

        "lsl    %[k1], %[k1], #56\n\t"
        "cmp    %w[inSz], #0\n\t"
        "b.eq   L_siphash_last_done\n\t"
        "cmp    %w[inSz], #4\n\t"
        "b.lt   L_siphash_last_lt4\n\t"

        "cmp    %w[inSz], #7\n\t"
        "b.lt   L_siphash_n7\n\t"
        "ldrb   %w[k0], [%[in], 6]\n\t"
        "orr    %[k1], %[k1], %[k0], lsl 48\n\t"
        "L_siphash_n7:\n\t"

        "cmp    %w[inSz], #6\n\t"
        "b.lt   L_siphash_n6\n\t"
        "ldrb   %w[k0], [%[in], 5]\n\t"
        "orr    %[k1], %[k1], %[k0], lsl 40\n\t"
        "L_siphash_n6:\n\t"

        "cmp    %w[inSz], #5\n\t"
        "b.lt   L_siphash_n5\n\t"
        "ldrb   %w[k0], [%[in], 4]\n\t"
        "orr    %[k1], %[k1], %[k0], lsl 32\n\t"
        "L_siphash_n5:\n\t"

        "ldr    %w[k0], [%[in]]\n\t"
        "orr    %[k1], %[k1], %[k0]\n\t"
        "b      L_siphash_last_done\n\t"

        "L_siphash_last_lt4:\n\t"

        "cmp    %w[inSz], #1\n\t"
        "b.eq   L_siphash_last_1\n\t"

        "cmp    %w[inSz], #3\n\t"
        "b.lt   L_siphash_n3\n\t"
        "ldrb   %w[k0], [%[in], 2]\n\t"
        "orr    %[k1], %[k1], %[k0], lsl 16\n\t"
        "L_siphash_n3:\n\t"

        "ldrh   %w[k0], [%[in]]\n\t"
        "orr    %[k1], %[k1], %[k0]\n\t"
        "b      L_siphash_last_done\n\t"

        "L_siphash_last_1:\n\t"
        "ldrb   %w[k0], [%[in]]\n\t"
        "orr    %[k1], %[k1], %[k0]\n\t"

        "L_siphash_last_done:\n\t"

        "eor    %[v3], %[v3], %[k1]\n\t"
#if WOLFSSL_SIPHASH_CROUNDS == 1
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
#elif WOLFSSL_SIPHASH_CROUNDS == 2
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
#endif
        "eor    %[v0], %[v0], %[k1]\n\t"

        "cmp    %w[outSz], #8\n\t"
        "b.eq   L_siphash_8_end\n\t"

        : [in] "+r" (in), [inSz] "+r" (inSz), [k0] "+r" (k0), [k1] "+r" (k1),
          [v0] "+r" (v0), [v1] "+r" (v1), [v2] "+r" (v2), [v3] "+r" (v3)
        : [key] "r" (key), [out] "r" (out) , [outSz] "r" (outSz)
        : "memory"
    );

    __asm__ __volatile__ (

        "mov    %w[k1], #0xee\n\t"
        "eor    %[v2], %[v2], %[k1]\n\t"
#if WOLFSSL_SIPHASH_DROUNDS == 2
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
#elif WOLFSSL_SIPHASH_DROUNDS == 4
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
#endif
        "eor    %[k0], %[v0], %[v1]\n\t"
        "eor    %[k1], %[v2], %[v3]\n\t"
        "eor    %[k0], %[k0], %[k1]\n\t"

        "mov    %w[k1], #0xdd\n\t"
        "eor    %[v1], %[v1], %[k1]\n\t"
#if WOLFSSL_SIPHASH_DROUNDS == 2
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
        SIPHASH_LAST_ROUND(%[v0], %[v1], %[v2], %[v3])
#elif WOLFSSL_SIPHASH_DROUNDS == 4
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
        SIPHASH_LAST_ROUND(%[v0], %[v1], %[v2], %[v3])
#endif
        "eor    %[k1], %[v3], %[v1]\n\t"
        "eor    %[k1], %[k1], %[v2]\n\t"
        "stp    %[k0], %[k1], [%[out]]\n\t"
        "b      L_siphash_done\n\t"

        "L_siphash_8_end:\n\t"
        "mov    %w[k1], #0xff\n\t"
        "eor    %[v2], %[v2], %[k1]\n\t"
#if WOLFSSL_SIPHASH_DROUNDS == 2
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
        SIPHASH_LAST_ROUND(%[v0], %[v1], %[v2], %[v3])
#elif WOLFSSL_SIPHASH_DROUNDS == 4
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
        SIPHASH_ROUND(%[v0], %[v1], %[v2], %[v3])
        SIPHASH_LAST_ROUND(%[v0], %[v1], %[v2], %[v3])
#endif
        "eor    %[k1], %[v3], %[v1]\n\t"
        "eor    %[k1], %[k1], %[v2]\n\t"
        "str    %[k1], [%[out]]\n\t"

        "L_siphash_done:\n\t"

        : [in] "+r" (in), [inSz] "+r" (inSz), [k0] "+r" (k0), [k1] "+r" (k1),
          [v0] "+r" (v0), [v1] "+r" (v1), [v2] "+r" (v2), [v3] "+r" (v3)
        : [key] "r" (key), [out] "r" (out) , [outSz] "r" (outSz)
        : "memory"
    );

    return 0;
}

#else

#define SipRoundV(v0, v1, v2, v3)   \
    v0 += v1;                       \
    v2 += v3;                       \
    v1 = rotlFixed64(v1, 13);       \
    v3 = rotlFixed64(v3, 16);       \
    v1 ^= v0;                       \
    v3 ^= v2;                       \
    v0 = rotlFixed64(v0, 32);       \
    v2 += v1;                       \
    v0 += v3;                       \
    v1 = rotlFixed64(v1, 17);       \
    v3 = rotlFixed64(v3, 21);       \
    v1 ^= v2;                       \
    v3 ^= v0;                       \
    v2 = rotlFixed64(v2, 32);

#define SipHashCompressV(v0, v1, v2, v3, m)             \
    do {                                                \
        int i;                                          \
        v3 ^= m;                                        \
        for (i = 0; i < WOLFSSL_SIPHASH_CROUNDS; i++) { \
            SipRoundV(v0, v1, v2, v3);                  \
        }                                               \
        v0 ^= m;                                        \
    }                                                   \
    while (0)

#define SipHashOutV(v0, v1, v2, v3, out)                \
    do {                                                \
        word64 n;                                       \
        int i;                                          \
                                                        \
        for (i = 0; i < WOLFSSL_SIPHASH_DROUNDS; i++) { \
            SipRoundV(v0, v1, v2, v3);                  \
        }                                               \
        n = v0 ^ v1 ^ v2 ^ v3;                          \
        SET_U64(out, n);                                \
    }                                                   \
    while (0)

/**
 * Perform SipHash operation on input with key.
 *
 * @param [in]      key      16 byte array - little endian.
 * @param [in]      in       Input message.
 * @param [in]      inSz     Size of input message.
 * @param [out]     out      Buffer to place MAC into.
 * @param [in]      outSz    Size of output MAC. 8 or 16 only.
 * @return  BAD_FUNC_ARG when key or out is NULL.
 * @return  BAD_FUNC_ARG when in is NULL and inSz is not zero.
 * @return  BAD_FUNC_ARG when outSz is not 8 nor 16.
 * @return  0 on success.
 */
int wc_SipHash(const unsigned char* key, const unsigned char* in, word32 inSz,
    unsigned char* out, unsigned char outSz)
{
    int ret = 0;

    if ((key == NULL) || ((in == NULL) && (inSz != 0)) || (out == NULL) ||
            ((outSz != SIPHASH_MAC_SIZE_8) && (outSz != SIPHASH_MAC_SIZE_16))) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        word64 v0, v1, v2, v3;
        word64 k0 = GET_U64(key + 0);
        word64 k1 = GET_U64(key + 8);
        word64 b = (word64)((word64)inSz << 56);

        /* Initialize state with key. */
        v0 = 0x736f6d6570736575ULL;
        v1 = 0x646f72616e646f6dULL;
        v2 = 0x6c7967656e657261ULL;
        v3 = 0x7465646279746573ULL;

        if (outSz == SIPHASH_MAC_SIZE_16) {
            v1 ^= 0xee;
        }

        v0 ^= k0;
        v1 ^= k1;
        v2 ^= k0;
        v3 ^= k1;

        /* Process blocks from message. */
        while (inSz >= SIPHASH_BLOCK_SIZE) {
            word64 m = GET_U64(in);
            /* Compress the next block from the message data. */
            SipHashCompressV(v0, v1, v2, v3, m);
            in += SIPHASH_BLOCK_SIZE;
            inSz -= SIPHASH_BLOCK_SIZE;
        }

        switch (inSz) {
            case 7:
                b |= (word64)in[6] << 48;
                FALL_THROUGH;
            case 6:
                b |= (word64)in[5] << 40;
                FALL_THROUGH;
            case 5:
                b |= (word64)in[4] << 32;
                FALL_THROUGH;
            case 4:
                b |= (word64)GET_U32(in);
                break;
            case 3:
                b |= (word64)in[2] << 16;
                FALL_THROUGH;
            case 2:
                b |= (word64)GET_U16(in);
                break;
            case 1:
                b |= (word64)in[0];
                break;
            case 0:
                break;
        }
        SipHashCompressV(v0, v1, v2, v3, b);

        /* Output either 8 or 16 bytes. */
        if (outSz == SIPHASH_MAC_SIZE_8) {
            v2 ^= (word64)0xff;
            SipHashOutV(v0, v1, v2, v3, out);
        }
        else {
            v2 ^= (word64)0xee;
            SipHashOutV(v0, v1, v2, v3, out);
            v1 ^= (word64)0xdd;
            SipHashOutV(v0, v1, v2, v3, out + 8);
        }
    }

    return ret;
}
#endif /* !ASM */

#endif /* WOLFSSL_SIPHASH */
