/* dh.h
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
    \file wolfssl/wolfcrypt/dh.h
*/

#ifndef WOLF_CRYPT_DH_H
#define WOLF_CRYPT_DH_H

#include <wolfssl/wolfcrypt/types.h>

#ifndef NO_DH

#if defined(HAVE_FIPS) && \
    defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2)
    #include <wolfssl/wolfcrypt/fips.h>
#endif /* HAVE_FIPS_VERSION >= 2 */

#include <wolfssl/wolfcrypt/wolfmath.h>
#include <wolfssl/wolfcrypt/random.h>

#ifdef WOLFSSL_KCAPI_DH
    #include <wolfssl/wolfcrypt/port/kcapi/kcapi_dh.h>
#endif

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef WOLFSSL_ASYNC_CRYPT
    #include <wolfssl/wolfcrypt/async.h>
#endif

typedef struct DhParams {
#ifdef HAVE_FFDHE_Q
    const byte* q;
    word32      q_len;
#endif /* HAVE_FFDHE_Q */
    const byte* p;
    word32      p_len;
    const byte* g;
    word32      g_len;
} DhParams;

/* Diffie-Hellman Key */
struct DhKey {
    mp_int p, g, q; /* group parameters */
#ifdef WOLFSSL_DH_EXTRA
    mp_int pub;
    mp_int priv;
#endif
    void* heap;
#ifdef WOLFSSL_ASYNC_CRYPT
    WC_ASYNC_DEV asyncDev;
#endif
    int trustedGroup;
#ifdef WOLFSSL_KCAPI_DH
    struct kcapi_handle* handle;
#endif
};

#ifndef WC_DH_TYPE_DEFINED
    typedef struct DhKey DhKey;
    #define WC_DH_TYPE_DEFINED
#endif

enum {
    WC_FFDHE_2048 = 256,
    WC_FFDHE_3072 = 257,
    WC_FFDHE_4096 = 258,
    WC_FFDHE_6144 = 259,
    WC_FFDHE_8192 = 260
};

/* DH Private Key size up to 8192 bit */
#ifndef WC_DH_PRIV_MAX_SZ
#define WC_DH_PRIV_MAX_SZ 52
#endif

#ifndef DH_MAX_SIZE
    #ifdef USE_FAST_MATH
        /* FP implementation support numbers up to FP_MAX_BITS / 2 bits. */
        #define DH_MAX_SIZE    (FP_MAX_BITS / 2)
        #if defined(WOLFSSL_MYSQL_COMPATIBLE) && DH_MAX_SIZE < 8192
            #error "MySQL needs FP_MAX_BITS at least at 16384"
        #endif
    #elif defined(WOLFSSL_SP_MATH_ALL) || defined(WOLFSSL_SP_MATH)
        /* SP implementation supports numbers of SP_INT_BITS bits. */
        #define DH_MAX_SIZE    (((SP_INT_BITS + 7) / 8) * 8)
        #if defined(WOLFSSL_MYSQL_COMPATIBLE) && DH_MAX_SIZE < 8192
            #error "MySQL needs SP_INT_BITS at least at 8192"
        #endif
    #else
        #ifdef WOLFSSL_MYSQL_COMPATIBLE
            /* Integer maths is dynamic but we only go up to 8192 bits. */
            #define DH_MAX_SIZE 8192
        #else
            /* Integer maths is dynamic but we only go up to 4096 bits. */
            #define DH_MAX_SIZE 4096
        #endif
    #endif
#endif

#ifdef HAVE_PUBLIC_FFDHE
#ifdef HAVE_FFDHE_2048
WOLFSSL_API const DhParams* wc_Dh_ffdhe2048_Get(void);
#endif
#ifdef HAVE_FFDHE_3072
WOLFSSL_API const DhParams* wc_Dh_ffdhe3072_Get(void);
#endif
#ifdef HAVE_FFDHE_4096
WOLFSSL_API const DhParams* wc_Dh_ffdhe4096_Get(void);
#endif
#ifdef HAVE_FFDHE_6144
WOLFSSL_API const DhParams* wc_Dh_ffdhe6144_Get(void);
#endif
#ifdef HAVE_FFDHE_8192
WOLFSSL_API const DhParams* wc_Dh_ffdhe8192_Get(void);
#endif
#endif

WOLFSSL_API int wc_InitDhKey(DhKey* key);
WOLFSSL_API int wc_InitDhKey_ex(DhKey* key, void* heap, int devId);
WOLFSSL_API int wc_FreeDhKey(DhKey* key);

WOLFSSL_API int wc_DhGenerateKeyPair(DhKey* key, WC_RNG* rng, byte* priv,
                                 word32* privSz, byte* pub, word32* pubSz);
WOLFSSL_API int wc_DhAgree(DhKey* key, byte* agree, word32* agreeSz,
                       const byte* priv, word32 privSz, const byte* otherPub,
                       word32 pubSz);

WOLFSSL_API int wc_DhKeyDecode(const byte* input, word32* inOutIdx, DhKey* key,
                           word32 inSz); /* wc_DhKeyDecode is in asn.c */

WOLFSSL_API int wc_DhSetKey(DhKey* key, const byte* p, word32 pSz, const byte* g,
                        word32 gSz);
WOLFSSL_API int wc_DhSetKey_ex(DhKey* key, const byte* p, word32 pSz,
                        const byte* g, word32 gSz, const byte* q, word32 qSz);
WOLFSSL_API int wc_DhSetNamedKey(DhKey* key, int name);
WOLFSSL_API int wc_DhGetNamedKeyParamSize(int name,
        word32* p, word32* g, word32* q);
WOLFSSL_API word32 wc_DhGetNamedKeyMinSize(int name);
WOLFSSL_API int wc_DhCmpNamedKey(int name, int noQ,
        const byte* p, word32 pSz,
        const byte* g, word32 gSz,
        const byte* q, word32 qSz);
WOLFSSL_API int wc_DhCopyNamedKey(int name,
        byte* p, word32* pSz, byte* g, word32* gSz, byte* q, word32* qSz);

#ifdef WOLFSSL_DH_EXTRA
WOLFSSL_API int wc_DhImportKeyPair(DhKey* key, const byte* priv, word32 privSz,
                                   const byte* pub, word32 pubSz);
WOLFSSL_API int wc_DhExportKeyPair(DhKey* key, byte* priv, word32* pPrivSz,
                                   byte* pub, word32* pPubSz);
WOLFSSL_LOCAL int wc_DhKeyCopy(DhKey* src, DhKey* dst);
#endif
WOLFSSL_API int wc_DhSetCheckKey(DhKey* key, const byte* p, word32 pSz,
                        const byte* g, word32 gSz, const byte* q, word32 qSz,
                        int trusted, WC_RNG* rng);
WOLFSSL_API int wc_DhParamsLoad(const byte* input, word32 inSz, byte* p,
                            word32* pInOutSz, byte* g, word32* gInOutSz);
WOLFSSL_API int wc_DhCheckPubKey(DhKey* key, const byte* pub, word32 pubSz);
WOLFSSL_API int wc_DhCheckPubKey_ex(DhKey* key, const byte* pub, word32 pubSz,
                            const byte* prime, word32 primeSz);
WOLFSSL_API int wc_DhCheckPubValue(const byte* prime, word32 primeSz,
                                   const byte* pub, word32 pubSz);
WOLFSSL_API int wc_DhCheckPrivKey(DhKey* key, const byte* priv, word32 pubSz);
WOLFSSL_API int wc_DhCheckPrivKey_ex(DhKey* key, const byte* priv, word32 pubSz,
                            const byte* prime, word32 primeSz);
WOLFSSL_API int wc_DhCheckKeyPair(DhKey* key, const byte* pub, word32 pubSz,
                        const byte* priv, word32 privSz);
WOLFSSL_API int wc_DhGenerateParams(WC_RNG *rng, int modSz, DhKey *dh);
WOLFSSL_API int wc_DhExportParamsRaw(DhKey* dh, byte* p, word32* pSz,
                       byte* q, word32* qSz, byte* g, word32* gSz);


#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* NO_DH */
#endif /* WOLF_CRYPT_DH_H */
