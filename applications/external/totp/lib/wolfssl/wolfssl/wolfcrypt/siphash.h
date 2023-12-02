/* siphash.h
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


#ifndef WOLF_CRYPT_SIPHASH_H
#define WOLF_CRYPT_SIPHASH_H

#include <wolfssl/wolfcrypt/types.h>

#if defined(WOLFSSL_SIPHASH)

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
 */

#ifndef WOLFSSL_SIPHASH_CROUNDS
/* Number of rounds to perform in compression operation. */
#define WOLFSSL_SIPHASH_CROUNDS  2
#endif /* WOLFSSL_SIPHASH_CROUNDS */

#ifndef WOLFSSL_SIPHASH_DROUNDS
/* Number of rounds to perform in final operation. */
#define WOLFSSL_SIPHASH_DROUNDS  4
#endif /* WOLFSSL_SIPHASH_DROUNDS */

enum {
    SIPHASH_KEY_SIZE    = 16,   /* Key size of SipHash. */
    SIPHASH_BLOCK_SIZE  = 8,    /* Block size of SipHash. */
    SIPHASH_MAC_SIZE_8  = 8,    /* Output an 8 byte MAC. */
    SIPHASH_MAC_SIZE_16 = 16    /* Output a 16 byte MAC. */
};

typedef struct SipHash SipHash;

struct SipHash {
    /* Internal state. */
    word64 v[4];
    /* Cached message data. */
    byte cache[SIPHASH_BLOCK_SIZE];
    /* Number of bytes cached. */
    byte cacheCnt;
    /* Number of output bytes. */
    byte outSz;
    /* Number of input bytes processed. */
    word32 inCnt;
};


#ifdef __cplusplus
    extern "C" {
#endif

WOLFSSL_API int wc_InitSipHash(SipHash* sipHash, const unsigned char* key,
    unsigned char outSz);
WOLFSSL_API int wc_SipHashUpdate(SipHash* sipHash, const unsigned char* in,
    word32 inSz);
WOLFSSL_API int wc_SipHashFinal(SipHash* sipHash, unsigned char* out,
    unsigned char outSz);
WOLFSSL_API int wc_SipHash(const unsigned char* key, const unsigned char* in,
    word32 inSz, unsigned char* out, unsigned char outSz);

#ifdef __cplusplus
    } /* extern "C" */
#endif


#endif /* NO_AES && WOLFSSL_SIPHASH */
#endif /* WOLF_CRYPT_SIPHASH_H */

