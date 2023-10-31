/* sha512.c
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

#ifdef WOLFSSL_ARMASM
#if defined(WOLFSSL_SHA512) || defined(WOLFSSL_SHA384)

#ifdef HAVE_FIPS
#undef HAVE_FIPS
#endif

#include <wolfssl/wolfcrypt/sha512.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/hash.h>

#include <wolfssl/wolfcrypt/logging.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif
#ifdef WOLF_CRYPTO_CB
#include <wolfssl/wolfcrypt/cryptocb.h>
#endif

#ifdef WOLFSSL_SHA512

static int InitSha512(wc_Sha512* sha512)
{
    if (sha512 == NULL)
        return BAD_FUNC_ARG;

    sha512->digest[0] = W64LIT(0x6a09e667f3bcc908);
    sha512->digest[1] = W64LIT(0xbb67ae8584caa73b);
    sha512->digest[2] = W64LIT(0x3c6ef372fe94f82b);
    sha512->digest[3] = W64LIT(0xa54ff53a5f1d36f1);
    sha512->digest[4] = W64LIT(0x510e527fade682d1);
    sha512->digest[5] = W64LIT(0x9b05688c2b3e6c1f);
    sha512->digest[6] = W64LIT(0x1f83d9abfb41bd6b);
    sha512->digest[7] = W64LIT(0x5be0cd19137e2179);

    sha512->buffLen = 0;
    sha512->loLen   = 0;
    sha512->hiLen   = 0;
#ifdef WOLFSSL_HASH_FLAGS
    sha512->flags = 0;
#endif

    return 0;
}

#if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
#if !defined(WOLFSSL_NOSHA512_224)
/**
 * Initialize given wc_Sha512 structure with value specific to sha512/224.
 * Note that sha512/224 has different initial hash value from sha512.
 * The initial hash value consists of eight 64bit words. They are given
 * in FIPS180-4.
 */
static int InitSha512_224(wc_Sha512* sha512)
{
    if (sha512 == NULL)
        return BAD_FUNC_ARG;

    sha512->digest[0] = W64LIT(0x8c3d37c819544da2);
    sha512->digest[1] = W64LIT(0x73e1996689dcd4d6);
    sha512->digest[2] = W64LIT(0x1dfab7ae32ff9c82);
    sha512->digest[3] = W64LIT(0x679dd514582f9fcf);
    sha512->digest[4] = W64LIT(0x0f6d2b697bd44da8);
    sha512->digest[5] = W64LIT(0x77e36f7304c48942);
    sha512->digest[6] = W64LIT(0x3f9d85a86a1d36c8);
    sha512->digest[7] = W64LIT(0x1112e6ad91d692a1);

    sha512->buffLen = 0;
    sha512->loLen   = 0;
    sha512->hiLen   = 0;
#ifdef WOLFSSL_HASH_FLAGS
    sha512->flags = 0;
#endif

    return 0;
}
#endif /* !WOLFSSL_NOSHA512_224 */
#endif /* !HAVE_FIPS && !HAVE_SELFTEST */

#if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
#if !defined(WOLFSSL_NOSHA512_256)
/**
 * Initialize given wc_Sha512 structure with value specific to sha512/256.
 * Note that sha512/256 has different initial hash value from sha512.
 * The initial hash value consists of eight 64bit words. They are given
 * in FIPS180-4.
 */
static int InitSha512_256(wc_Sha512* sha512)
{
    if (sha512 == NULL)
        return BAD_FUNC_ARG;

    sha512->digest[0] = W64LIT(0x22312194fc2bf72c);
    sha512->digest[1] = W64LIT(0x9f555fa3c84c64c2);
    sha512->digest[2] = W64LIT(0x2393b86b6f53b151);
    sha512->digest[3] = W64LIT(0x963877195940eabd);
    sha512->digest[4] = W64LIT(0x96283ee2a88effe3);
    sha512->digest[5] = W64LIT(0xbe5e1e2553863992);
    sha512->digest[6] = W64LIT(0x2b0199fc2c85b8aa);
    sha512->digest[7] = W64LIT(0x0eb72ddc81c52ca2);

    sha512->buffLen = 0;
    sha512->loLen   = 0;
    sha512->hiLen   = 0;
#ifdef WOLFSSL_HASH_FLAGS
    sha512->flags = 0;
#endif

    return 0;
}
#endif /* !WOLFSSL_NOSHA512_256 */
#endif /* !HAVE_FIPS && !HAVE_SELFTEST */

#endif /* WOLFSSL_SHA512 */

#ifdef WOLFSSL_SHA512

#ifdef WOLFSSL_ARMASM
#ifdef __aarch64__
#ifndef WOLFSSL_ARMASM_CRYPTO_SHA512
    extern void Transform_Sha512_Len_neon(wc_Sha512* sha512, const byte* data,
        word32 len);
    #define Transform_Sha512_Len    Transform_Sha512_Len_neon
#else
    extern void Transform_Sha512_Len_crypto(wc_Sha512* sha512, const byte* data,
        word32 len);
    #define Transform_Sha512_Len    Transform_Sha512_Len_crypto
#endif
#else
extern void Transform_Sha512_Len(wc_Sha512* sha512, const byte* data,
    word32 len);
#endif
#endif

static int InitSha512_Family(wc_Sha512* sha512, void* heap, int devId,
                                                    enum wc_HashType type)
{
   int ret = 0;

    if (sha512 == NULL)
        return BAD_FUNC_ARG;

    sha512->heap = heap;
#ifdef WOLFSSL_SMALL_STACK_CACHE
    sha512->W = NULL;
#endif

    if (type == WC_HASH_TYPE_SHA512) {
        ret = InitSha512(sha512);
    }
#if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
#if !defined(WOLFSSL_NOSHA512_224)
    else if (type == WC_HASH_TYPE_SHA512_224) {
        ret = InitSha512_224(sha512);
    }
#endif
#if !defined(WOLFSSL_NOSHA512_256)
    else if (type == WC_HASH_TYPE_SHA512_256) {
        ret = InitSha512_256(sha512);
    }
#endif
#endif /* !HAVE_FIPS && !HAVE_SELFTEST */
    else
        ret = BAD_FUNC_ARG;

    if (ret != 0)
        return ret;

    (void)devId;

    return ret;
}

int wc_InitSha512_ex(wc_Sha512* sha512, void* heap, int devId)
{
    return InitSha512_Family(sha512, heap, devId, WC_HASH_TYPE_SHA512);
}

#if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
#if !defined(WOLFSSL_NOSHA512_224)
int wc_InitSha512_224_ex(wc_Sha512* sha512, void* heap, int devId)
{
    return InitSha512_Family(sha512, heap, devId, WC_HASH_TYPE_SHA512_224);
}
#endif /* !WOLFSSL_NOSHA512_224 */
#endif /* !HAVE_FIPS && !HAVE_SELFTEST */

#if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
#if !defined(WOLFSSL_NOSHA512_256)
int wc_InitSha512_256_ex(wc_Sha512* sha512, void* heap, int devId)
{
    return InitSha512_Family(sha512, heap, devId, WC_HASH_TYPE_SHA512_256);
}
#endif /* !WOLFSSL_NOSHA512_256 */
#endif /* !HAVE_FIPS && !HAVE_SELFTEST */

#endif /* WOLFSSL_SHA512 */

#ifndef WOLFSSL_ARMASM
static const word64 K512[80] = {
    W64LIT(0x428a2f98d728ae22), W64LIT(0x7137449123ef65cd),
    W64LIT(0xb5c0fbcfec4d3b2f), W64LIT(0xe9b5dba58189dbbc),
    W64LIT(0x3956c25bf348b538), W64LIT(0x59f111f1b605d019),
    W64LIT(0x923f82a4af194f9b), W64LIT(0xab1c5ed5da6d8118),
    W64LIT(0xd807aa98a3030242), W64LIT(0x12835b0145706fbe),
    W64LIT(0x243185be4ee4b28c), W64LIT(0x550c7dc3d5ffb4e2),
    W64LIT(0x72be5d74f27b896f), W64LIT(0x80deb1fe3b1696b1),
    W64LIT(0x9bdc06a725c71235), W64LIT(0xc19bf174cf692694),
    W64LIT(0xe49b69c19ef14ad2), W64LIT(0xefbe4786384f25e3),
    W64LIT(0x0fc19dc68b8cd5b5), W64LIT(0x240ca1cc77ac9c65),
    W64LIT(0x2de92c6f592b0275), W64LIT(0x4a7484aa6ea6e483),
    W64LIT(0x5cb0a9dcbd41fbd4), W64LIT(0x76f988da831153b5),
    W64LIT(0x983e5152ee66dfab), W64LIT(0xa831c66d2db43210),
    W64LIT(0xb00327c898fb213f), W64LIT(0xbf597fc7beef0ee4),
    W64LIT(0xc6e00bf33da88fc2), W64LIT(0xd5a79147930aa725),
    W64LIT(0x06ca6351e003826f), W64LIT(0x142929670a0e6e70),
    W64LIT(0x27b70a8546d22ffc), W64LIT(0x2e1b21385c26c926),
    W64LIT(0x4d2c6dfc5ac42aed), W64LIT(0x53380d139d95b3df),
    W64LIT(0x650a73548baf63de), W64LIT(0x766a0abb3c77b2a8),
    W64LIT(0x81c2c92e47edaee6), W64LIT(0x92722c851482353b),
    W64LIT(0xa2bfe8a14cf10364), W64LIT(0xa81a664bbc423001),
    W64LIT(0xc24b8b70d0f89791), W64LIT(0xc76c51a30654be30),
    W64LIT(0xd192e819d6ef5218), W64LIT(0xd69906245565a910),
    W64LIT(0xf40e35855771202a), W64LIT(0x106aa07032bbd1b8),
    W64LIT(0x19a4c116b8d2d0c8), W64LIT(0x1e376c085141ab53),
    W64LIT(0x2748774cdf8eeb99), W64LIT(0x34b0bcb5e19b48a8),
    W64LIT(0x391c0cb3c5c95a63), W64LIT(0x4ed8aa4ae3418acb),
    W64LIT(0x5b9cca4f7763e373), W64LIT(0x682e6ff3d6b2b8a3),
    W64LIT(0x748f82ee5defb2fc), W64LIT(0x78a5636f43172f60),
    W64LIT(0x84c87814a1f0ab72), W64LIT(0x8cc702081a6439ec),
    W64LIT(0x90befffa23631e28), W64LIT(0xa4506cebde82bde9),
    W64LIT(0xbef9a3f7b2c67915), W64LIT(0xc67178f2e372532b),
    W64LIT(0xca273eceea26619c), W64LIT(0xd186b8c721c0c207),
    W64LIT(0xeada7dd6cde0eb1e), W64LIT(0xf57d4f7fee6ed178),
    W64LIT(0x06f067aa72176fba), W64LIT(0x0a637dc5a2c898a6),
    W64LIT(0x113f9804bef90dae), W64LIT(0x1b710b35131c471b),
    W64LIT(0x28db77f523047d84), W64LIT(0x32caab7b40c72493),
    W64LIT(0x3c9ebe0a15c9bebc), W64LIT(0x431d67c49c100d4c),
    W64LIT(0x4cc5d4becb3e42b6), W64LIT(0x597f299cfc657e2a),
    W64LIT(0x5fcb6fab3ad6faec), W64LIT(0x6c44198c4a475817)
};

#ifdef LITTLE_ENDIAN_ORDER
#define blk0(i) (W[i] = ByteReverseWord64(DATA[i]))
#else
#define blk0(i) (W[i] = DATA[i])
#endif

#define blk2(i) (                \
               W[ i         ] += \
            s1(W[(i- 2) & 15])+  \
               W[(i- 7) & 15] +  \
            s0(W[(i-15) & 15])   \
        )

#define Ch(x,y,z)  (z ^ ((z ^ y) & x))
#define Maj(x,y,z) (y ^ ((y ^ z) & (x ^ y)))

#define a(i) T[(0-i) & 7]
#define b(i) T[(1-i) & 7]
#define c(i) T[(2-i) & 7]
#define d(i) T[(3-i) & 7]
#define e(i) T[(4-i) & 7]
#define f(i) T[(5-i) & 7]
#define g(i) T[(6-i) & 7]
#define h(i) T[(7-i) & 7]

#define S0(x) (rotrFixed64(x,28) ^ rotrFixed64(x,34) ^ rotrFixed64(x,39))
#define S1(x) (rotrFixed64(x,14) ^ rotrFixed64(x,18) ^ rotrFixed64(x,41))
#define s0(x) (rotrFixed64(x, 1) ^ rotrFixed64(x, 8) ^ (x>>7))
#define s1(x) (rotrFixed64(x,19) ^ rotrFixed64(x,61) ^ (x>>6))

#define R0(i)                                                 \
    h(i) += S1(e(i)) + Ch(e(i),f(i),g(i)) + K[i+j] + blk0(i); \
    d(i) += h(i);                                             \
    h(i) += S0(a(i)) + Maj(a(i),b(i),c(i))
#define R(i)                                                  \
    h(i) += S1(e(i)) + Ch(e(i),f(i),g(i)) + K[i+j] + blk2(i); \
    d(i) += h(i);                                             \
    h(i) += S0(a(i)) + Maj(a(i),b(i),c(i))

#define DATA    sha512->buffer
static void Transform_Sha512(wc_Sha512* sha512)
{
    const word64* K = K512;
    word32 j;
    word64 T[8];
    word64 W[16];

    /* Copy digest to working vars */
    T[0] = sha512->digest[0];
    T[1] = sha512->digest[1];
    T[2] = sha512->digest[2];
    T[3] = sha512->digest[3];
    T[4] = sha512->digest[4];
    T[5] = sha512->digest[5];
    T[6] = sha512->digest[6];
    T[7] = sha512->digest[7];

    /* 80 operations, partially loop unrolled */
    j = 0;
    R0( 0); R0( 1); R0( 2); R0( 3);
    R0( 4); R0( 5); R0( 6); R0( 7);
    R0( 8); R0( 9); R0(10); R0(11);
    R0(12); R0(13); R0(14); R0(15);
    for (j = 16; j < 80; j += 16) {
        R( 0); R( 1); R( 2); R( 3);
        R( 4); R( 5); R( 6); R( 7);
        R( 8); R( 9); R(10); R(11);
        R(12); R(13); R(14); R(15);
    }

    /* Add the working vars back into digest */
    sha512->digest[0] += T[0];
    sha512->digest[1] += T[1];
    sha512->digest[2] += T[2];
    sha512->digest[3] += T[3];
    sha512->digest[4] += T[4];
    sha512->digest[5] += T[5];
    sha512->digest[6] += T[6];
    sha512->digest[7] += T[7];

    return 0;
}
#undef DATA

#define DATA    ((word64*)data)
static void Transform_Sha512_Len(wc_Sha512* sha512, const byte* data, word32 len)
{
    const word64* K = K512;
    word32 j;
    word64 T[8];
    word64 TO[8];
    word64 W[16];

    /* Copy digest to working vars */
    T[0] = sha512->digest[0];
    T[1] = sha512->digest[1];
    T[2] = sha512->digest[2];
    T[3] = sha512->digest[3];
    T[4] = sha512->digest[4];
    T[5] = sha512->digest[5];
    T[6] = sha512->digest[6];
    T[7] = sha512->digest[7];

    do {
        TO[0] = T[0];
        TO[1] = T[1];
        TO[2] = T[2];
        TO[3] = T[3];
        TO[4] = T[4];
        TO[5] = T[5];
        TO[6] = T[6];
        TO[7] = T[7];

        /* 80 operations, partially loop unrolled */
        j = 0;
        R0( 0); R0( 1); R0( 2); R0( 3);
        R0( 4); R0( 5); R0( 6); R0( 7);
        R0( 8); R0( 9); R0(10); R0(11);
        R0(12); R0(13); R0(14); R0(15);
        for (j = 16; j < 80; j += 16) {
            R( 0); R( 1); R( 2); R( 3);
            R( 4); R( 5); R( 6); R( 7);
            R( 8); R( 9); R(10); R(11);
            R(12); R(13); R(14); R(15);
        }

        T[0] += TO[0];
        T[1] += TO[1];
        T[2] += TO[2];
        T[3] += TO[3];
        T[4] += TO[4];
        T[5] += TO[5];
        T[6] += TO[6];
        T[7] += TO[7];

        data += 128;
        len -= 128;
    }
    while (len > 0);

    /* Add the working vars back into digest */
    sha512->digest[0] = T[0];
    sha512->digest[1] = T[1];
    sha512->digest[2] = T[2];
    sha512->digest[3] = T[3];
    sha512->digest[4] = T[4];
    sha512->digest[5] = T[5];
    sha512->digest[6] = T[6];
    sha512->digest[7] = T[7];

    return 0;
}
#undef DATA

#endif


static WC_INLINE void AddLength(wc_Sha512* sha512, word32 len)
{
    word64 tmp = sha512->loLen;
    if ( (sha512->loLen += len) < tmp)
        sha512->hiLen++;                       /* carry low to high */
}

static WC_INLINE int Sha512Update(wc_Sha512* sha512, const byte* data, word32 len)
{
    int ret = 0;
    /* do block size increments */
    byte* local = (byte*)sha512->buffer;
    word32 blocksLen;

    /* check that internal buffLen is valid */
    if (sha512->buffLen >= WC_SHA512_BLOCK_SIZE)
        return BUFFER_E;

    AddLength(sha512, len);

    if (sha512->buffLen > 0) {
        word32 add = min(len, WC_SHA512_BLOCK_SIZE - sha512->buffLen);
        if (add > 0) {
            XMEMCPY(&local[sha512->buffLen], data, add);

            sha512->buffLen += add;
            data            += add;
            len             -= add;
        }

        if (sha512->buffLen == WC_SHA512_BLOCK_SIZE) {
#ifndef WOLFSSL_ARMASM
            Transform_Sha512(sha512);
#else
            Transform_Sha512_Len(sha512, (const byte*)sha512->buffer,
                                                          WC_SHA512_BLOCK_SIZE);
#endif
            sha512->buffLen = 0;
        }
    }

    blocksLen = len & ~(WC_SHA512_BLOCK_SIZE-1);
    if (blocksLen > 0) {
        /* Byte reversal performed in function if required. */
    #ifndef WOLFSSL_ARMASM_NO_NEON
        /* Data must be 64-bit aligned to be passed to Transform_Sha512_Len().
         * 64 bits is 8 bytes.
         */
        if (((size_t)data & 0x7) != 0) {
            word32 i;

            for (i = 0; i < blocksLen; i += WC_SHA512_BLOCK_SIZE) {
                word64 buffer[WC_SHA512_BLOCK_SIZE / sizeof(word64)];
                XMEMCPY(buffer, data + i, WC_SHA512_BLOCK_SIZE);
                Transform_Sha512_Len(sha512, (const byte*)buffer,
                                                          WC_SHA512_BLOCK_SIZE);
            }
        }
        else
    #endif
        {
            Transform_Sha512_Len(sha512, data, blocksLen);
        }
        data += blocksLen;
        len  -= blocksLen;
    }

    if (len > 0) {
        XMEMCPY(local, data, len);
        sha512->buffLen = len;
    }

    return ret;
}

#ifdef WOLFSSL_SHA512

int wc_Sha512Update(wc_Sha512* sha512, const byte* data, word32 len)
{
    if (sha512 == NULL || (data == NULL && len > 0)) {
        return BAD_FUNC_ARG;
    }

    return Sha512Update(sha512, data, len);
}

#endif /* WOLFSSL_SHA512 */

static WC_INLINE int Sha512Final(wc_Sha512* sha512)
{
    byte* local = (byte*)sha512->buffer;

    if (sha512 == NULL) {
        return BAD_FUNC_ARG;
    }

    local[sha512->buffLen++] = 0x80;  /* add 1 */

    /* pad with zeros */
    if (sha512->buffLen > WC_SHA512_PAD_SIZE) {
        XMEMSET(&local[sha512->buffLen], 0, WC_SHA512_BLOCK_SIZE -
                                                               sha512->buffLen);
        sha512->buffLen += WC_SHA512_BLOCK_SIZE - sha512->buffLen;
#ifndef WOLFSSL_ARMASM
        Transform_Sha512(sha512);
#else
        Transform_Sha512_Len(sha512, (const byte*)sha512->buffer,
                                                          WC_SHA512_BLOCK_SIZE);
#endif

        sha512->buffLen = 0;
    }
    XMEMSET(&local[sha512->buffLen], 0, WC_SHA512_PAD_SIZE - sha512->buffLen);

    /* put lengths in bits */
    sha512->hiLen = (sha512->loLen >> (8 * sizeof(sha512->loLen) - 3)) +
                                                         (sha512->hiLen << 3);
    sha512->loLen = sha512->loLen << 3;

    /* store lengths */
    /* ! length ordering dependent on digest endian type ! */

    sha512->buffer[WC_SHA512_BLOCK_SIZE / sizeof(word64) - 2] = sha512->hiLen;
    sha512->buffer[WC_SHA512_BLOCK_SIZE / sizeof(word64) - 1] = sha512->loLen;

    ByteReverseWords64(
                   &(sha512->buffer[WC_SHA512_BLOCK_SIZE / sizeof(word64) - 2]),
                   &(sha512->buffer[WC_SHA512_BLOCK_SIZE / sizeof(word64) - 2]),
                   WC_SHA512_BLOCK_SIZE - WC_SHA512_PAD_SIZE);
#ifndef WOLFSSL_ARMASM
    Transform_Sha512(sha512);
#else
    Transform_Sha512_Len(sha512, (const byte*)sha512->buffer,
                                                          WC_SHA512_BLOCK_SIZE);
#endif

#ifdef LITTLE_ENDIAN_ORDER
    ByteReverseWords64(sha512->digest, sha512->digest, WC_SHA512_DIGEST_SIZE);
#endif

    return 0;
}

#ifdef WOLFSSL_SHA512

int wc_Sha512FinalRaw(wc_Sha512* sha512, byte* hash)
{
#ifdef LITTLE_ENDIAN_ORDER
    word64 digest[WC_SHA512_DIGEST_SIZE / sizeof(word64)];
#endif

    if (sha512 == NULL || hash == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef LITTLE_ENDIAN_ORDER
    ByteReverseWords64((word64*)digest, (word64*)sha512->digest,
                                                         WC_SHA512_DIGEST_SIZE);
    XMEMCPY(hash, digest, WC_SHA512_DIGEST_SIZE);
#else
    XMEMCPY(hash, sha512->digest, WC_SHA512_DIGEST_SIZE);
#endif

    return 0;
}

static int Sha512_Family_Final(wc_Sha512* sha512, byte* hash,
                                                    enum wc_HashType type)
{
    int ret;
    int digestSz;
    int (*initfp)(wc_Sha512*);

    (void)initfp;

    if (sha512 == NULL || hash == NULL) {
        return BAD_FUNC_ARG;
    }

    if (type == WC_HASH_TYPE_SHA512) {
        initfp    = InitSha512;
        digestSz = WC_SHA512_DIGEST_SIZE;
    }
#if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
#if !defined(WOLFSSL_NOSHA512_224)
    else if (type == WC_HASH_TYPE_SHA512_224) {
        initfp    = InitSha512_224;
        digestSz = WC_SHA512_224_DIGEST_SIZE;
    }
#endif
#if !defined(WOLFSSL_NOSHA512_256)
    else if (type == WC_HASH_TYPE_SHA512_256) {
        initfp = InitSha512_256;
        digestSz = WC_SHA512_256_DIGEST_SIZE;
    }
#endif
#endif /* !HAVE_FIPS && !HAVE_SELFTEST */
    else
        return BAD_FUNC_ARG;

#ifdef WOLF_CRYPTO_CB
    if (sha512->devId != INVALID_DEVID) {
        ret = wc_CryptoCb_Sha512Hash(sha512, NULL, 0, hash);
        if (ret != CRYPTOCB_UNAVAILABLE)
            return ret;
        /* fall-through when unavailable */
    }
#endif
#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA512)
    if (sha512->asyncDev.marker == WOLFSSL_ASYNC_MARKER_SHA512) {
    #if defined(HAVE_INTEL_QA)
        return IntelQaSymSha512(&sha512->asyncDev, hash, NULL, digestSz);
    #endif
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    ret = Sha512Final(sha512);
    if (ret != 0)
        return ret;

    XMEMCPY(hash, sha512->digest, digestSz);

    /* initialize Sha512 structure for the next use */
    if (initfp != NULL) {
        ret = initfp(sha512);
    }
    return ret;
}

int wc_Sha512Final(wc_Sha512* sha512, byte* hash)
{
    return Sha512_Family_Final(sha512, hash, WC_HASH_TYPE_SHA512);
}

int wc_InitSha512(wc_Sha512* sha512)
{
    return wc_InitSha512_ex(sha512, NULL, INVALID_DEVID);
}

void wc_Sha512Free(wc_Sha512* sha512)
{
    if (sha512 == NULL)
        return;

#ifdef WOLFSSL_SMALL_STACK_CACHE
    if (sha512->W != NULL) {
        XFREE(sha512->W, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        sha512->W = NULL;
    }
#endif
}

#ifdef OPENSSL_EXTRA
int wc_Sha512Transform(wc_Sha512* sha512, const unsigned char* data)
{
    if (sha512 == NULL || data == NULL) {
        return BAD_FUNC_ARG;
    }
    Transform_Sha512_Len(sha512, data, WC_SHA512_BLOCK_SIZE);
    return 0;
}
#endif

#endif /* WOLFSSL_SHA512 */

/* -------------------------------------------------------------------------- */
/* SHA384 */
/* -------------------------------------------------------------------------- */
#ifdef WOLFSSL_SHA384

static int InitSha384(wc_Sha384* sha384)
{
    if (sha384 == NULL) {
        return BAD_FUNC_ARG;
    }

    sha384->digest[0] = W64LIT(0xcbbb9d5dc1059ed8);
    sha384->digest[1] = W64LIT(0x629a292a367cd507);
    sha384->digest[2] = W64LIT(0x9159015a3070dd17);
    sha384->digest[3] = W64LIT(0x152fecd8f70e5939);
    sha384->digest[4] = W64LIT(0x67332667ffc00b31);
    sha384->digest[5] = W64LIT(0x8eb44a8768581511);
    sha384->digest[6] = W64LIT(0xdb0c2e0d64f98fa7);
    sha384->digest[7] = W64LIT(0x47b5481dbefa4fa4);

    sha384->buffLen = 0;
    sha384->loLen   = 0;
    sha384->hiLen   = 0;
#ifdef WOLFSSL_HASH_FLAGS
    sha384->flags = 0;
#endif

    return 0;
}

int wc_Sha384Update(wc_Sha384* sha384, const byte* data, word32 len)
{
    if (sha384 == NULL || (data == NULL && len > 0)) {
        return BAD_FUNC_ARG;
    }

    return Sha512Update((wc_Sha512*)sha384, data, len);
}


int wc_Sha384FinalRaw(wc_Sha384* sha384, byte* hash)
{
#ifdef LITTLE_ENDIAN_ORDER
    word64 digest[WC_SHA384_DIGEST_SIZE / sizeof(word64)];
#endif

    if (sha384 == NULL || hash == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef LITTLE_ENDIAN_ORDER
    ByteReverseWords64((word64*)digest, (word64*)sha384->digest,
                                                         WC_SHA384_DIGEST_SIZE);
    XMEMCPY(hash, digest, WC_SHA384_DIGEST_SIZE);
#else
    XMEMCPY(hash, sha384->digest, WC_SHA384_DIGEST_SIZE);
#endif

    return 0;
}

int wc_Sha384Final(wc_Sha384* sha384, byte* hash)
{
    int ret;

    if (sha384 == NULL || hash == NULL) {
        return BAD_FUNC_ARG;
    }

    ret = Sha512Final((wc_Sha512*)sha384);
    if (ret != 0)
        return ret;

    XMEMCPY(hash, sha384->digest, WC_SHA384_DIGEST_SIZE);

    return InitSha384(sha384);  /* reset state */
}

int wc_InitSha384_ex(wc_Sha384* sha384, void* heap, int devId)
{
    int ret;

    if (sha384 == NULL) {
        return BAD_FUNC_ARG;
    }

    sha384->heap = heap;
    ret = InitSha384(sha384);
    if (ret != 0)
        return ret;

#ifdef WOLFSSL_SMALL_STACK_CACHE
    sha384->W = NULL;
#endif

    (void)devId;

    return ret;
}

int wc_InitSha384(wc_Sha384* sha384)
{
    return wc_InitSha384_ex(sha384, NULL, INVALID_DEVID);
}

void wc_Sha384Free(wc_Sha384* sha384)
{
    if (sha384 == NULL)
        return;

#ifdef WOLFSSL_SMALL_STACK_CACHE
    if (sha384->W != NULL) {
        XFREE(sha384->W, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        sha384->W = NULL;
    }
#endif
}

#endif /* WOLFSSL_SHA384 */

#ifdef WOLFSSL_SHA512

#if !defined(WOLFSSL_NOSHA512_224) || !defined(WOLFSSL_NOSHA512_256)

static int Sha512_Family_GetHash(wc_Sha512* sha512, byte* hash,
                                            enum wc_HashType type )
{
    int (*finalfp)(wc_Sha512*, byte*);
    int ret;
    wc_Sha512 tmpSha512;

    if (sha512 == NULL || hash == NULL)
        return BAD_FUNC_ARG;

    if (type == WC_HASH_TYPE_SHA512)
        finalfp = wc_Sha512Final;
#if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
#if !defined(WOLFSSL_NOSHA512_224)
    else if (type == WC_HASH_TYPE_SHA512_224)
        finalfp = wc_Sha512_224Final;
#endif
#if !defined(WOLFSSL_NOSHA512_256)
    else if (type == WC_HASH_TYPE_SHA512_256)
        finalfp = wc_Sha512_256Final;
#endif
#endif /* !HAVE_FIPS && !HAVE_SELFTEST */
    else
        finalfp = NULL;

    if (finalfp == NULL)
        return BAD_FUNC_ARG;

    ret = wc_Sha512Copy(sha512, &tmpSha512);
    if (ret == 0) {
        ret = finalfp(&tmpSha512, hash);
        wc_Sha512Free(&tmpSha512);
    }
    return ret;
}

#endif /* !WOLFSSL_NOSHA512_224 || !WOLFSSL_NOSHA512_256 */

int wc_Sha512GetHash(wc_Sha512* sha512, byte* hash)
{
    int ret;
    wc_Sha512 tmpSha512;

    if (sha512 == NULL || hash == NULL)
        return BAD_FUNC_ARG;

    ret = wc_Sha512Copy(sha512, &tmpSha512);
    if (ret == 0) {
        ret = wc_Sha512Final(&tmpSha512, hash);
        wc_Sha512Free(&tmpSha512);
    }
    return ret;
}

int wc_Sha512Copy(wc_Sha512* src, wc_Sha512* dst)
{
    int ret = 0;

    if (src == NULL || dst == NULL)
        return BAD_FUNC_ARG;

    XMEMCPY(dst, src, sizeof(wc_Sha512));
#ifdef WOLFSSL_SMALL_STACK_CACHE
    dst->W = NULL;
#endif

#ifdef WOLFSSL_HASH_FLAGS
     dst->flags |= WC_HASH_FLAG_ISCOPY;
#endif

    return ret;
}

#ifdef WOLFSSL_HASH_FLAGS
int wc_Sha512SetFlags(wc_Sha512* sha512, word32 flags)
{
    if (sha512) {
        sha512->flags = flags;
    }
    return 0;
}
int wc_Sha512GetFlags(wc_Sha512* sha512, word32* flags)
{
    if (sha512 && flags) {
        *flags = sha512->flags;
    }
    return 0;
}
#endif

#if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)

#if !defined(WOLFSSL_NOSHA512_224)
int wc_InitSha512_224(wc_Sha512* sha)
{
    return wc_InitSha512_224_ex(sha, NULL, INVALID_DEVID);
}
int wc_Sha512_224Update(wc_Sha512* sha, const byte* data, word32 len)
{
    return wc_Sha512Update(sha, data, len);
}
int wc_Sha512_224FinalRaw(wc_Sha512* sha, byte* hash)
{
    return wc_Sha512FinalRaw(sha, hash);
}
int wc_Sha512_224Final(wc_Sha512* sha512, byte* hash)
{
    return Sha512_Family_Final(sha512, hash, WC_HASH_TYPE_SHA512_224);
}
void wc_Sha512_224Free(wc_Sha512* sha)
{
    wc_Sha512Free(sha);
}
int wc_Sha512_224GetHash(wc_Sha512* sha512, byte* hash)
{
    return Sha512_Family_GetHash(sha512, hash, WC_HASH_TYPE_SHA512_224);
}
int wc_Sha512_224Copy(wc_Sha512* src, wc_Sha512* dst)
{
    return wc_Sha512Copy(src, dst);
}

#ifdef WOLFSSL_HASH_FLAGS
int wc_Sha512_224SetFlags(wc_Sha512* sha, word32 flags)
{
    return wc_Sha512SetFlags(sha, flags);
}
int wc_Sha512_224GetFlags(wc_Sha512* sha, word32* flags)
{
    return wc_Sha512GetFlags(sha, flags);
}
#endif /* WOLFSSL_HASH_FLAGS */

#if defined(OPENSSL_EXTRA)
int wc_Sha512_224Transform(wc_Sha512* sha, const unsigned char* data)
{
    return wc_Sha512Transform(sha, data);
}
#endif /* OPENSSL_EXTRA */

#endif /* !WOLFSSL_NOSHA512_224 */

#if !defined(WOLFSSL_NOSHA512_256)
int wc_InitSha512_256(wc_Sha512* sha)
{
    return wc_InitSha512_256_ex(sha, NULL, INVALID_DEVID);
}
int wc_Sha512_256Update(wc_Sha512* sha, const byte* data, word32 len)
{
    return wc_Sha512Update(sha, data, len);
}
int wc_Sha512_256FinalRaw(wc_Sha512* sha, byte* hash)
{
    return wc_Sha512FinalRaw(sha, hash);
}
int wc_Sha512_256Final(wc_Sha512* sha512, byte* hash)
{
    return Sha512_Family_Final(sha512, hash, WC_HASH_TYPE_SHA512_256);
}
void wc_Sha512_256Free(wc_Sha512* sha)
{
    wc_Sha512Free(sha);
}
int wc_Sha512_256GetHash(wc_Sha512* sha512, byte* hash)
{
    return Sha512_Family_GetHash(sha512, hash, WC_HASH_TYPE_SHA512_256);
}
int wc_Sha512_256Copy(wc_Sha512* src, wc_Sha512* dst)
{
    return wc_Sha512Copy(src, dst);
}

#ifdef WOLFSSL_HASH_FLAGS
int wc_Sha512_256SetFlags(wc_Sha512* sha, word32 flags)
{
    return wc_Sha512SetFlags(sha, flags);
}
int wc_Sha512_256GetFlags(wc_Sha512* sha, word32* flags)
{
    return wc_Sha512GetFlags(sha, flags);
}
#endif /* WOLFSSL_HASH_FLAGS */

#if defined(OPENSSL_EXTRA)
int wc_Sha512_256Transform(wc_Sha512* sha, const unsigned char* data)
{
    return wc_Sha512Transform(sha, data);
}
#endif /* OPENSSL_EXTRA */

#endif /* !WOLFSSL_NOSHA512_224 */
#endif /* !HAVE_FIPS && !HAVE_SELFTEST */

#endif /* WOLFSSL_SHA512 */

#ifdef WOLFSSL_SHA384

int wc_Sha384GetHash(wc_Sha384* sha384, byte* hash)
{
    int ret;
    wc_Sha384 tmpSha384;

    if (sha384 == NULL || hash == NULL)
        return BAD_FUNC_ARG;
    ret = wc_Sha384Copy(sha384, &tmpSha384);
    if (ret == 0) {
        ret = wc_Sha384Final(&tmpSha384, hash);
        wc_Sha384Free(&tmpSha384);
    }
    return ret;
}
int wc_Sha384Copy(wc_Sha384* src, wc_Sha384* dst)
{
    int ret = 0;

    if (src == NULL || dst == NULL)
        return BAD_FUNC_ARG;

    XMEMCPY(dst, src, sizeof(wc_Sha384));
#ifdef WOLFSSL_SMALL_STACK_CACHE
    dst->W = NULL;
#endif

#ifdef WOLFSSL_HASH_FLAGS
     dst->flags |= WC_HASH_FLAG_ISCOPY;
#endif

    return ret;
}

#ifdef WOLFSSL_HASH_FLAGS
int wc_Sha384SetFlags(wc_Sha384* sha384, word32 flags)
{
    if (sha384) {
        sha384->flags = flags;
    }
    return 0;
}
int wc_Sha384GetFlags(wc_Sha384* sha384, word32* flags)
{
    if (sha384 && flags) {
        *flags = sha384->flags;
    }
    return 0;
}
#endif

#endif /* WOLFSSL_SHA384 */

#endif /* WOLFSSL_SHA512 || WOLFSSL_SHA384 */
#endif /* WOLFSSL_ARMASM */
