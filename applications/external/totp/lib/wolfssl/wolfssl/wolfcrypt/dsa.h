/* dsa.h
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

/*!
    \file wolfssl/wolfcrypt/dsa.h
*/

#ifndef WOLF_CRYPT_DSA_H
#define WOLF_CRYPT_DSA_H

#include <wolfssl/wolfcrypt/types.h>

#ifndef NO_DSA

#include <wolfssl/wolfcrypt/wolfmath.h>
#include <wolfssl/wolfcrypt/random.h>

/* for DSA reverse compatibility */
#define InitDsaKey wc_InitDsaKey
#define FreeDsaKey wc_FreeDsaKey
#define DsaSign wc_DsaSign
#define DsaVerify wc_DsaVerify
#define DsaPublicKeyDecode wc_DsaPublicKeyDecode
#define DsaPrivateKeyDecode wc_DsaPrivateKeyDecode
#define DsaKeyToDer wc_DsaKeyToDer

#ifdef __cplusplus
    extern "C" {
#endif


enum {
    DSA_PUBLIC   = 0,
    DSA_PRIVATE  = 1
};

enum {
    /* 160 bit q length */
    DSA_160_HALF_SIZE = 20,            /* r and s size  */
    DSA_160_SIG_SIZE  = 40,            /* signature size */
    DSA_HALF_SIZE = DSA_160_HALF_SIZE, /* kept for compatibility  */
    DSA_SIG_SIZE = DSA_160_SIG_SIZE,   /* kept for compatibility */
    /* 256 bit q length */
    DSA_256_HALF_SIZE = 32,            /* r and s size  */
    DSA_256_SIG_SIZE  = 64,            /* signature size */

    DSA_MIN_HALF_SIZE = DSA_160_HALF_SIZE,
    DSA_MIN_SIG_SIZE = DSA_160_SIG_SIZE,

    DSA_MAX_HALF_SIZE = DSA_256_HALF_SIZE,
    DSA_MAX_SIG_SIZE = DSA_256_SIG_SIZE
};

/* DSA */
typedef struct DsaKey {
    mp_int p, q, g, y, x;
    int   type;                               /* public or private */
    void* heap;                               /* memory hint */
} DsaKey;

WOLFSSL_API int wc_InitDsaKey(DsaKey* key);
WOLFSSL_API int wc_InitDsaKey_h(DsaKey* key, void* h);
WOLFSSL_API void wc_FreeDsaKey(DsaKey* key);
WOLFSSL_API int wc_DsaSign(const byte* digest, byte* out,
                           DsaKey* key, WC_RNG* rng);
WOLFSSL_API int wc_DsaSign_ex(const byte* digest, word32 digestSz, byte* out,
                           DsaKey* key, WC_RNG* rng);
WOLFSSL_API int wc_DsaVerify(const byte* digest, const byte* sig,
                             DsaKey* key, int* answer);
WOLFSSL_API int wc_DsaVerify_ex(const byte* digest, word32 digestSz,
                                const byte* sig, DsaKey* key, int* answer);
WOLFSSL_API int wc_DsaPublicKeyDecode(const byte* input, word32* inOutIdx,
                                      DsaKey* key, word32 inSz);
WOLFSSL_API int wc_DsaPrivateKeyDecode(const byte* input, word32* inOutIdx,
                                       DsaKey* key, word32 inSz);
WOLFSSL_API int wc_DsaKeyToDer(DsaKey* key, byte* output, word32 inLen);
WOLFSSL_API int wc_SetDsaPublicKey(byte* output, DsaKey* key,
                                   int outLen, int with_header);
WOLFSSL_API int wc_DsaKeyToPublicDer(DsaKey* key, byte* output, word32 inLen);

#ifdef WOLFSSL_KEY_GEN
WOLFSSL_API int wc_MakeDsaKey(WC_RNG *rng, DsaKey *dsa);
WOLFSSL_API int wc_MakeDsaParameters(WC_RNG *rng, int modulus_size, DsaKey *dsa);
#endif

/* raw export functions */
WOLFSSL_API int wc_DsaImportParamsRaw(DsaKey* dsa, const char* p,
                                      const char* q, const char* g);
WOLFSSL_API int wc_DsaImportParamsRawCheck(DsaKey* dsa, const char* p,
                                      const char* q, const char* g,
                                      int trusted, WC_RNG* rng);
WOLFSSL_API int wc_DsaExportParamsRaw(DsaKey* dsa, byte* p, word32* pSz,
                                      byte* q, word32* qSz, byte* g,
                                      word32* gSz);
WOLFSSL_API int wc_DsaExportKeyRaw(DsaKey* dsa, byte* x, word32* xSz, byte* y,
                                   word32* ySz);
#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* NO_DSA */
#endif /* WOLF_CRYPT_DSA_H */

