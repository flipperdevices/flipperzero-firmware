/* rsa.h
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
    \file wolfssl/wolfcrypt/rsa.h
*/

/*

DESCRIPTION
This library provides the interface to the RSA.
RSA keys can be used to encrypt, decrypt, sign and verify data.

*/
#ifndef WOLF_CRYPT_RSA_H
#define WOLF_CRYPT_RSA_H

#include <wolfssl/wolfcrypt/types.h>

#ifndef NO_RSA


/* RSA default exponent */
#ifndef WC_RSA_EXPONENT
    #define WC_RSA_EXPONENT 65537L
#endif

#if defined(WC_RSA_NONBLOCK)
    /* enable support for fast math based non-blocking exptmod */
    /* this splits the RSA function into many smaller operations */
    #ifndef USE_FAST_MATH
        #error RSA non-blocking mode only supported using fast math
    #endif
    #ifndef TFM_TIMING_RESISTANT
      #error RSA non-blocking mode only supported with timing resistance enabled
    #endif

    /* RSA bounds check is not supported with RSA non-blocking mode */
    #undef  NO_RSA_BOUNDS_CHECK
    #define NO_RSA_BOUNDS_CHECK
#endif

/* allow for user to plug in own crypto */
#if !defined(HAVE_FIPS) && (defined(HAVE_USER_RSA) || defined(HAVE_FAST_RSA))
    #include "user_rsa.h"
#else

#include <wolfssl/wolfcrypt/wolfmath.h>
#include <wolfssl/wolfcrypt/random.h>

#if defined(HAVE_FIPS) && \
        defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2)
#include <wolfssl/wolfcrypt/fips.h>
#endif

/* header file needed for OAEP padding */
#include <wolfssl/wolfcrypt/hash.h>

#ifdef WOLFSSL_XILINX_CRYPT
#ifdef WOLFSSL_XILINX_CRYPT_VERSAL
#include <wolfssl/wolfcrypt/port/xilinx/xil-versal-glue.h>
#else
#include <xsecure_rsa.h>
#endif
#endif

#if defined(WOLFSSL_CRYPTOCELL)
    #include <wolfssl/wolfcrypt/port/arm/cryptoCell.h>
#endif

#if defined(WOLFSSL_KCAPI_RSA)
    #include <wolfssl/wolfcrypt/port/kcapi/kcapi_rsa.h>
#endif

#if defined(WOLFSSL_DEVCRYPTO_RSA)
    #include <wolfssl/wolfcrypt/port/devcrypto/wc_devcrypto.h>
#endif

#ifdef __cplusplus
    extern "C" {
#endif

#ifndef RSA_MIN_SIZE
#define RSA_MIN_SIZE 512
#endif

#ifndef RSA_MAX_SIZE
    #ifdef USE_FAST_MATH
        /* FP implementation support numbers up to FP_MAX_BITS / 2 bits. */
        #define RSA_MAX_SIZE    (FP_MAX_BITS / 2)
        #if defined(WOLFSSL_MYSQL_COMPATIBLE) && RSA_MAX_SIZE < 8192
            #error "MySQL needs FP_MAX_BITS at least at 16384"
        #endif
    #elif defined(WOLFSSL_SP_MATH_ALL) || defined(WOLFSSL_SP_MATH)
        /* SP implementation supports numbers of SP_INT_BITS bits. */
        #define RSA_MAX_SIZE    (((SP_INT_BITS + 7) / 8) * 8)
        #if defined(WOLFSSL_MYSQL_COMPATIBLE) && RSA_MAX_SIZE < 8192
            #error "MySQL needs SP_INT_BITS at least at 8192"
        #endif
    #else
        #ifdef WOLFSSL_MYSQL_COMPATIBLE
            /* Integer maths is dynamic but we only go up to 8192 bits. */
            #define RSA_MAX_SIZE 8192
        #else
            /* Integer maths is dynamic but we only go up to 4096 bits. */
            #define RSA_MAX_SIZE 4096
        #endif
    #endif
#endif

/* avoid redefinition of structs */
#if !defined(HAVE_FIPS) || \
    (defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2))

#ifdef WOLFSSL_ASYNC_CRYPT
    #include <wolfssl/wolfcrypt/async.h>
    #ifdef WOLFSSL_CERT_GEN
        #include <wolfssl/wolfcrypt/asn.h>
    #endif
#endif

enum {
    RSA_PUBLIC   = 0,
    RSA_PRIVATE  = 1,

    RSA_TYPE_UNKNOWN    = -1,
    RSA_PUBLIC_ENCRYPT  = 0,
    RSA_PUBLIC_DECRYPT  = 1,
    RSA_PRIVATE_ENCRYPT = 2,
    RSA_PRIVATE_DECRYPT = 3,

    RSA_BLOCK_TYPE_1 = 1,
    RSA_BLOCK_TYPE_2 = 2,

    RSA_MIN_PAD_SZ   = 11,     /* separator + 0 + pad value + 8 pads */

    RSA_PSS_PAD_SZ = 8,
    RSA_PSS_SALT_MAX_SZ = 62,

#ifdef OPENSSL_EXTRA
    RSA_PKCS1_PADDING_SIZE = 11,
    RSA_PKCS1_OAEP_PADDING_SIZE = 42, /* (2 * hashlen(SHA-1)) + 2 */
#endif
#ifdef WC_RSA_PSS
    RSA_PSS_PAD_TERM = 0xBC,
#endif

    RSA_PSS_SALT_LEN_DEFAULT  = -1,
#ifdef WOLFSSL_PSS_SALT_LEN_DISCOVER
    RSA_PSS_SALT_LEN_DISCOVER = -2,
#endif

#ifdef WOLF_PRIVATE_KEY_ID
    RSA_MAX_ID_LEN      = 32,
    RSA_MAX_LABEL_LEN   = 32,
#endif

    WOLF_ENUM_DUMMY_LAST_ELEMENT(RSA)
};

#ifdef WC_RSA_NONBLOCK
typedef struct RsaNb {
    exptModNb_t exptmod; /* non-block expt_mod */
    mp_int tmp;
} RsaNb;
#endif

/* RSA */
struct RsaKey {
    mp_int n, e;
#ifndef WOLFSSL_RSA_PUBLIC_ONLY
    mp_int d, p, q;
#if defined(WOLFSSL_KEY_GEN) || defined(OPENSSL_EXTRA) || !defined(RSA_LOW_MEM)
    mp_int dP, dQ, u;
#endif
#endif
    void* heap;                               /* for user memory overrides */
    byte* data;                               /* temp buffer for async RSA */
    int   type;                               /* public or private */
    int   state;
    word32 dataLen;
#ifdef WC_RSA_BLINDING
    WC_RNG* rng;                              /* for PrivateDecrypt blinding */
#endif
#ifdef WOLFSSL_SE050
    word32 keyId;
    byte   keyIdSet;
#endif
#ifdef WOLF_CRYPTO_CB
    int   devId;
#endif
#if defined(HAVE_PKCS11)
    byte isPkcs11 : 1; /* indicate if PKCS11 is preferred */
#endif
#ifdef WOLFSSL_ASYNC_CRYPT
    WC_ASYNC_DEV asyncDev;
    #ifdef WOLFSSL_CERT_GEN
        CertSignCtx certSignCtx; /* context info for cert sign (MakeSignature) */
    #endif
#endif /* WOLFSSL_ASYNC_CRYPT */
#ifdef WOLFSSL_XILINX_CRYPT
    word32 pubExp; /* to keep values in scope they are here in struct */
    byte*  mod;
#if defined(WOLFSSL_XILINX_CRYPT_VERSAL)
    int mSz;
    wc_Xsecure xSec;
#else
    XSecure_Rsa xRsa;
#endif
#endif
#if defined(WOLFSSL_KCAPI_RSA)
    struct kcapi_handle* handle;
#endif
#ifdef WOLF_PRIVATE_KEY_ID
    byte id[RSA_MAX_ID_LEN];
    int  idLen;
    char label[RSA_MAX_LABEL_LEN];
    int  labelLen;
#endif
#if defined(WOLFSSL_ASYNC_CRYPT) || !defined(WOLFSSL_RSA_VERIFY_INLINE) && \
    !defined(WOLFSSL_NO_MALLOC)
    byte   dataIsAlloc;
#endif
#ifdef WC_RSA_NONBLOCK
    RsaNb* nb;
#endif
#ifdef WOLFSSL_AFALG_XILINX_RSA
    int alFd;
    int rdFd;
#endif
#if defined(WOLFSSL_CRYPTOCELL)
    rsa_context_t ctx;
#endif
#if defined(WOLFSSL_CAAM)
    word32 blackKey;
#endif
#if defined(WOLFSSL_DEVCRYPTO_RSA)
    WC_CRYPTODEV ctx;
#endif
};

#ifndef WC_RSAKEY_TYPE_DEFINED
    typedef struct RsaKey RsaKey;
    #define WC_RSAKEY_TYPE_DEFINED
#endif

#endif /* HAVE_FIPS */

WOLFSSL_API int  wc_InitRsaKey(RsaKey* key, void* heap);
WOLFSSL_API int  wc_InitRsaKey_ex(RsaKey* key, void* heap, int devId);
WOLFSSL_API int  wc_FreeRsaKey(RsaKey* key);
#ifdef WOLF_PRIVATE_KEY_ID
WOLFSSL_API int wc_InitRsaKey_Id(RsaKey* key, unsigned char* id, int len,
                                 void* heap, int devId);
WOLFSSL_API int wc_InitRsaKey_Label(RsaKey* key, const char* label, void* heap,
                                    int devId);
#endif
WOLFSSL_API int  wc_CheckRsaKey(RsaKey* key);
#ifdef WOLFSSL_XILINX_CRYPT
WOLFSSL_LOCAL int wc_InitRsaHw(RsaKey* key);
#endif /* WOLFSSL_XILINX_CRYPT */
#ifdef WOLFSSL_SE050
WOLFSSL_API int wc_RsaUseKeyId(RsaKey* key, word32 keyId, word32 flags);
WOLFSSL_API int wc_RsaGetKeyId(RsaKey* key, word32* keyId);
#endif /* WOLFSSL_SE050 */

WOLFSSL_API int  wc_RsaFunction(const byte* in, word32 inLen, byte* out,
                           word32* outLen, int type, RsaKey* key, WC_RNG* rng);

WOLFSSL_API int  wc_RsaPublicEncrypt(const byte* in, word32 inLen, byte* out,
                                 word32 outLen, RsaKey* key, WC_RNG* rng);
WOLFSSL_API int  wc_RsaPrivateDecryptInline(byte* in, word32 inLen, byte** out,
                                        RsaKey* key);
WOLFSSL_API int  wc_RsaPrivateDecrypt(const byte* in, word32 inLen, byte* out,
                                  word32 outLen, RsaKey* key);
WOLFSSL_API int  wc_RsaSSL_Sign(const byte* in, word32 inLen, byte* out,
                            word32 outLen, RsaKey* key, WC_RNG* rng);
WOLFSSL_API int  wc_RsaPSS_Sign(const byte* in, word32 inLen, byte* out,
                                word32 outLen, enum wc_HashType hash, int mgf,
                                RsaKey* key, WC_RNG* rng);
WOLFSSL_API int  wc_RsaPSS_Sign_ex(const byte* in, word32 inLen, byte* out,
                                   word32 outLen, enum wc_HashType hash,
                                   int mgf, int saltLen, RsaKey* key,
                                   WC_RNG* rng);
WOLFSSL_API int  wc_RsaSSL_VerifyInline(byte* in, word32 inLen, byte** out,
                                    RsaKey* key);
WOLFSSL_API int  wc_RsaSSL_Verify(const byte* in, word32 inLen, byte* out,
                              word32 outLen, RsaKey* key);
WOLFSSL_API int  wc_RsaSSL_Verify_ex(const byte* in, word32 inLen, byte* out,
                              word32 outLen, RsaKey* key, int pad_type);
WOLFSSL_API int  wc_RsaSSL_Verify_ex2(const byte* in, word32 inLen, byte* out,
                              word32 outLen, RsaKey* key, int pad_type,
                              enum wc_HashType hash);
WOLFSSL_API int  wc_RsaPSS_VerifyInline(byte* in, word32 inLen, byte** out,
                                        enum wc_HashType hash, int mgf,
                                        RsaKey* key);
WOLFSSL_API int  wc_RsaPSS_VerifyInline_ex(byte* in, word32 inLen, byte** out,
                                           enum wc_HashType hash, int mgf,
                                           int saltLen, RsaKey* key);
WOLFSSL_API int  wc_RsaPSS_Verify(byte* in, word32 inLen, byte* out,
                                  word32 outLen, enum wc_HashType hash, int mgf,
                                  RsaKey* key);
WOLFSSL_API int  wc_RsaPSS_Verify_ex(byte* in, word32 inLen, byte* out,
                                     word32 outLen, enum wc_HashType hash,
                                     int mgf, int saltLen, RsaKey* key);
WOLFSSL_API int  wc_RsaPSS_CheckPadding(const byte* in, word32 inLen, byte* sig,
                                        word32 sigSz,
                                        enum wc_HashType hashType);
WOLFSSL_API int  wc_RsaPSS_CheckPadding_ex(const byte* in, word32 inLen,
                                           byte* sig, word32 sigSz,
                                           enum wc_HashType hashType,
                                           int saltLen, int bits);
WOLFSSL_API int  wc_RsaPSS_CheckPadding_ex2(const byte* in, word32 inLen,
                                           byte* sig, word32 sigSz,
                                           enum wc_HashType hashType,
                                           int saltLen, int bits, void* heap);
WOLFSSL_API int  wc_RsaPSS_VerifyCheckInline(byte* in, word32 inLen, byte** out,
                               const byte* digest, word32 digentLen,
                               enum wc_HashType hash, int mgf,
                               RsaKey* key);
WOLFSSL_API int  wc_RsaPSS_VerifyCheck(byte* in, word32 inLen,
                               byte* out, word32 outLen,
                               const byte* digest, word32 digestLen,
                               enum wc_HashType hash, int mgf,
                               RsaKey* key);

WOLFSSL_API int  wc_RsaEncryptSize(const RsaKey* key);

#if !defined(HAVE_FIPS) || \
        (defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2))
/* to avoid asn duplicate symbols @wc_fips */
WOLFSSL_API int  wc_RsaPrivateKeyDecode(const byte* input, word32* inOutIdx,
                                        RsaKey* key, word32 inSz);
#if !defined(HAVE_FIPS) || \
        (defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION > 2))
WOLFSSL_API int  wc_RsaPrivateKeyValidate(const byte* input, word32* inOutIdx,
                                          int* keySz, word32 inSz);
#endif
WOLFSSL_API int  wc_RsaPublicKeyDecode(const byte* input, word32* inOutIdx,
                                       RsaKey* key, word32 inSz);
WOLFSSL_API int  wc_RsaPublicKeyDecodeRaw(const byte* n, word32 nSz,
                                        const byte* e, word32 eSz, RsaKey* key);
#if defined(WOLFSSL_KEY_GEN) || defined(OPENSSL_EXTRA) || \
        defined(WOLFSSL_KCAPI_RSA) || defined(WOLFSSL_SE050)
    WOLFSSL_API int wc_RsaKeyToDer(RsaKey* key, byte* output, word32 inLen);
#endif

#ifdef WC_RSA_BLINDING
    WOLFSSL_API int wc_RsaSetRNG(RsaKey* key, WC_RNG* rng);
#endif
#ifdef WC_RSA_NONBLOCK
    WOLFSSL_API int wc_RsaSetNonBlock(RsaKey* key, RsaNb* nb);
    #ifdef WC_RSA_NONBLOCK_TIME
    WOLFSSL_API int wc_RsaSetNonBlockTime(RsaKey* key, word32 maxBlockUs,
                                          word32 cpuMHz);
    #endif
#endif

/*
   choice of padding added after fips, so not available when using fips RSA
 */

/* Mask Generation Function Identifiers */
#define WC_MGF1NONE       0
#define WC_MGF1SHA1       26
#define WC_MGF1SHA224     4
#define WC_MGF1SHA256     1
#define WC_MGF1SHA384     2
#define WC_MGF1SHA512     3
#define WC_MGF1SHA512_224 5
#define WC_MGF1SHA512_256 6

/* Padding types */
#define WC_RSA_PKCSV15_PAD 0
#define WC_RSA_OAEP_PAD    1
#define WC_RSA_PSS_PAD     2
#define WC_RSA_NO_PAD      3

WOLFSSL_API int  wc_RsaPublicEncrypt_ex(const byte* in, word32 inLen, byte* out,
                   word32 outLen, RsaKey* key, WC_RNG* rng, int type,
                   enum wc_HashType hash, int mgf, byte* label, word32 labelSz);
WOLFSSL_API int  wc_RsaPrivateDecrypt_ex(const byte* in, word32 inLen,
                   byte* out, word32 outLen, RsaKey* key, int type,
                   enum wc_HashType hash, int mgf, byte* label, word32 labelSz);
WOLFSSL_API int  wc_RsaPrivateDecryptInline_ex(byte* in, word32 inLen,
                      byte** out, RsaKey* key, int type, enum wc_HashType hash,
                      int mgf, byte* label, word32 labelSz);
#if defined(WC_RSA_DIRECT) || defined(WC_RSA_NO_PADDING)
WOLFSSL_API int wc_RsaDirect(byte* in, word32 inLen, byte* out, word32* outSz,
                   RsaKey* key, int type, WC_RNG* rng);
#endif

#endif /* HAVE_FIPS */

WOLFSSL_API int  wc_RsaFlattenPublicKey(RsaKey* key, byte* e, word32* eSz,
                                        byte* n, word32* nSz);
WOLFSSL_API int wc_RsaExportKey(RsaKey* key,
                                byte* e, word32* eSz,
                                byte* n, word32* nSz,
                                byte* d, word32* dSz,
                                byte* p, word32* pSz,
                                byte* q, word32* qSz);

#ifdef WOLFSSL_KEY_GEN
    WOLFSSL_API int wc_MakeRsaKey(RsaKey* key, int size, long e, WC_RNG* rng);
    WOLFSSL_API int wc_CheckProbablePrime_ex(const byte* p, word32 pSz,
                                          const byte* q, word32 qSz,
                                          const byte* e, word32 eSz,
                                          int nlen, int* isPrime, WC_RNG* rng);
    WOLFSSL_API int wc_CheckProbablePrime(const byte* p, word32 pSz,
                                          const byte* q, word32 qSz,
                                          const byte* e, word32 eSz,
                                          int nlen, int* isPrime);
#endif

WOLFSSL_LOCAL int wc_RsaPad_ex(const byte* input, word32 inputLen, byte* pkcsBlock,
        word32 pkcsBlockLen, byte padValue, WC_RNG* rng, int padType,
        enum wc_HashType hType, int mgf, byte* optLabel, word32 labelLen,
        int saltLen, int bits, void* heap);
WOLFSSL_LOCAL int wc_RsaUnPad_ex(byte* pkcsBlock, word32 pkcsBlockLen, byte** out,
                                   byte padValue, int padType, enum wc_HashType hType,
                                   int mgf, byte* optLabel, word32 labelLen, int saltLen,
                                   int bits, void* heap);

WOLFSSL_LOCAL int wc_hash2mgf(enum wc_HashType hType);

#endif /* HAVE_USER_RSA */

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* NO_RSA */
#endif /* WOLF_CRYPT_RSA_H */

