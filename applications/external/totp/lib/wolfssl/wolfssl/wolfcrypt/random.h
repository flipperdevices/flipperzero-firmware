/* random.h
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
    \file wolfssl/wolfcrypt/random.h
*/



#ifndef WOLF_CRYPT_RANDOM_H
#define WOLF_CRYPT_RANDOM_H

#include <wolfssl/wolfcrypt/types.h>

#if defined(HAVE_FIPS) && \
    defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2)
    #include <wolfssl/wolfcrypt/fips.h>
#endif /* HAVE_FIPS_VERSION >= 2 */

#ifdef __cplusplus
    extern "C" {
#endif

 /* Maximum generate block length */
#ifndef RNG_MAX_BLOCK_LEN
    #ifdef HAVE_INTEL_QA
        #define RNG_MAX_BLOCK_LEN (0xFFFFl)
    #else
        #define RNG_MAX_BLOCK_LEN (0x10000l)
    #endif
#endif

/* Size of the BRBG seed */
#ifndef DRBG_SEED_LEN
    #define DRBG_SEED_LEN (440/8)
#endif


#if !defined(CUSTOM_RAND_TYPE)
    /* To maintain compatibility the default is byte */
    #define CUSTOM_RAND_TYPE    byte
#endif

/* make sure Hash DRBG is enabled, unless WC_NO_HASHDRBG is defined
    or CUSTOM_RAND_GENERATE_BLOCK is defined */
#if !defined(WC_NO_HASHDRBG) && !defined(CUSTOM_RAND_GENERATE_BLOCK)
    #undef  HAVE_HASHDRBG
    #define HAVE_HASHDRBG
    #ifndef WC_RESEED_INTERVAL
        #define WC_RESEED_INTERVAL (1000000)
    #endif
#endif


/* avoid redefinition of structs */
#if !defined(HAVE_FIPS) || \
    (defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2))

/* RNG supports the following sources (in order):
 * 1. CUSTOM_RAND_GENERATE_BLOCK: Defines name of function as RNG source and
 *     bypasses the options below.
 * 2. HAVE_INTEL_RDRAND: Uses the Intel RDRAND if supported by CPU.
 * 3. HAVE_HASHDRBG (requires SHA256 enabled): Uses SHA256 based P-RNG
 *     seeded via wc_GenerateSeed. This is the default source.
 */

 /* Seed source can be overridden by defining one of these:
      CUSTOM_RAND_GENERATE_SEED
      CUSTOM_RAND_GENERATE_SEED_OS
      CUSTOM_RAND_GENERATE */


#if defined(CUSTOM_RAND_GENERATE_BLOCK)
    /* To use define the following:
     * #define CUSTOM_RAND_GENERATE_BLOCK myRngFunc
     * extern int myRngFunc(byte* output, word32 sz);
     */
    #if defined(CUSTOM_RAND_GENERATE_BLOCK) && defined(WOLFSSL_KCAPI)
        #undef  CUSTOM_RAND_GENERATE_BLOCK
        #define CUSTOM_RAND_GENERATE_BLOCK wc_hwrng_generate_block
        WOLFSSL_LOCAL int wc_hwrng_generate_block(byte *output, word32 sz);
    #endif
#elif defined(HAVE_HASHDRBG)
    #ifdef NO_SHA256
        #error "Hash DRBG requires SHA-256."
    #endif /* NO_SHA256 */
    #include <wolfssl/wolfcrypt/sha256.h>
#elif defined(HAVE_WNR)
     /* allow whitewood as direct RNG source using wc_GenerateSeed directly */
#elif defined(HAVE_INTEL_RDRAND)
    /* Intel RDRAND or RDSEED */
#elif defined(WOLF_CRYPTO_CB)
    /* Requires registered Crypto Callback to service RNG, with devId set */
#elif !defined(WC_NO_RNG)
    #error No RNG source defined!
#endif

#ifdef HAVE_WNR
    #include <wnr.h>
#endif

#ifdef WOLFSSL_ASYNC_CRYPT
    #include <wolfssl/wolfcrypt/async.h>
#endif


#if defined(USE_WINDOWS_API)
    #if defined(_WIN64)
        typedef unsigned __int64 ProviderHandle;
        /* type HCRYPTPROV, avoid #include <windows.h> */
    #else
        typedef unsigned long ProviderHandle;
    #endif
#endif

#ifndef WC_RNG_TYPE_DEFINED /* guard on redeclaration */
    typedef struct OS_Seed OS_Seed;
    typedef struct WC_RNG WC_RNG;
    #ifdef WC_RNG_SEED_CB
        typedef int (*wc_RngSeed_Cb)(OS_Seed* os, byte* seed, word32 sz);
    #endif
    #define WC_RNG_TYPE_DEFINED
#endif

/* OS specific seeder */
struct OS_Seed {
    #if defined(USE_WINDOWS_API)
        ProviderHandle handle;
    #else
        int fd;
    #endif
    #if defined(WOLF_CRYPTO_CB)
        int devId;
    #endif
};

#ifdef HAVE_HASHDRBG
struct DRBG_internal {
    word32 reseedCtr;
    byte V[DRBG_SEED_LEN];
    byte C[DRBG_SEED_LEN];
    void* heap;
#if defined(WOLFSSL_ASYNC_CRYPT) || defined(WOLF_CRYPTO_CB)
    int devId;
#endif
#ifdef WOLFSSL_SMALL_STACK_CACHE
    wc_Sha256 sha256;
#endif
};
#endif

/* RNG context */
struct WC_RNG {
    struct OS_Seed seed;
    void* heap;
#ifdef HAVE_HASHDRBG
    /* Hash-based Deterministic Random Bit Generator */
    struct DRBG* drbg;
#if defined(WOLFSSL_NO_MALLOC) && !defined(WOLFSSL_STATIC_MEMORY)
    struct DRBG_internal drbg_data;
#endif
    byte status;
#endif
#ifdef WOLFSSL_ASYNC_CRYPT
    WC_ASYNC_DEV asyncDev;
#endif
#if defined(WOLFSSL_ASYNC_CRYPT) || defined(WOLF_CRYPTO_CB)
    int devId;
#endif
};

#endif /* NO FIPS or have FIPS v2*/

/* NO_OLD_RNGNAME removes RNG struct name to prevent possible type conflicts,
 * can't be used with CTaoCrypt FIPS */
#if !defined(NO_OLD_RNGNAME) && !defined(HAVE_FIPS)
    #define RNG WC_RNG
#endif

WOLFSSL_API int wc_GenerateSeed(OS_Seed* os, byte* seed, word32 sz);


#ifdef HAVE_WNR
    /* Whitewood netRandom client library */
    WOLFSSL_API int  wc_InitNetRandom(const char*, wnr_hmac_key, int);
    WOLFSSL_API int  wc_FreeNetRandom(void);
#endif /* HAVE_WNR */


WOLFSSL_ABI WOLFSSL_API WC_RNG* wc_rng_new(byte* nonce, word32 nonceSz, void* heap);
WOLFSSL_ABI WOLFSSL_API void wc_rng_free(WC_RNG* rng);


#ifndef WC_NO_RNG
WOLFSSL_ABI WOLFSSL_API int  wc_InitRng(WC_RNG* rng);
WOLFSSL_API int  wc_InitRng_ex(WC_RNG* rng, void* heap, int devId);
WOLFSSL_API int  wc_InitRngNonce(WC_RNG* rng, byte* nonce, word32 nonceSz);
WOLFSSL_API int  wc_InitRngNonce_ex(WC_RNG* rng, byte* nonce, word32 nonceSz,
                                    void* heap, int devId);
WOLFSSL_ABI WOLFSSL_API int wc_RNG_GenerateBlock(WC_RNG* rng, byte* b, word32 sz);
WOLFSSL_API int  wc_RNG_GenerateByte(WC_RNG* rng, byte* b);
WOLFSSL_API int  wc_FreeRng(WC_RNG* rng);
#else
#include <wolfssl/wolfcrypt/error-crypt.h>
#define wc_InitRng(rng) NOT_COMPILED_IN
#define wc_InitRng_ex(rng, h, d) NOT_COMPILED_IN
#define wc_InitRngNonce(rng, n, s) NOT_COMPILED_IN
#define wc_InitRngNonce_ex(rng, n, s, h, d) NOT_COMPILED_IN
#if defined(__ghs__) || defined(WC_NO_RNG_SIMPLE)
/* some older compilers do not like macro function in expression */
#define wc_RNG_GenerateBlock(rng, b, s) NOT_COMPILED_IN
#else
#define wc_RNG_GenerateBlock(rng, b, s) ({(void)rng; (void)b; (void)s; NOT_COMPILED_IN;})
#endif
#define wc_RNG_GenerateByte(rng, b) NOT_COMPILED_IN
#define wc_FreeRng(rng) (void)NOT_COMPILED_IN
#endif

#ifdef WC_RNG_SEED_CB
    WOLFSSL_API int wc_SetSeed_Cb(wc_RngSeed_Cb cb);
#endif

#ifdef HAVE_HASHDRBG
    WOLFSSL_LOCAL int wc_RNG_DRBG_Reseed(WC_RNG* rng, const byte* entropy,
                                        word32 entropySz);
    WOLFSSL_API int wc_RNG_TestSeed(const byte* seed, word32 seedSz);
    WOLFSSL_API int wc_RNG_HealthTest(int reseed,
                                        const byte* entropyA, word32 entropyASz,
                                        const byte* entropyB, word32 entropyBSz,
                                        byte* output, word32 outputSz);
    WOLFSSL_API int wc_RNG_HealthTest_ex(int reseed,
                                        const byte* nonce, word32 nonceSz,
                                        const byte* entropyA, word32 entropyASz,
                                        const byte* entropyB, word32 entropyBSz,
                                        byte* output, word32 outputSz,
                                        void* heap, int devId);
#endif /* HAVE_HASHDRBG */

#ifdef HAVE_ENTROPY_MEMUSE
/* Maximum entropy bits that can be produced. */
#define MAX_ENTROPY_BITS    256

/* For generating data for assessment. */
WOLFSSL_API int wc_Entropy_GetRawEntropy(unsigned char* raw, int cnt);
WOLFSSL_API int wc_Entropy_Get(int bits, unsigned char* entropy, word32 len);
WOLFSSL_API int wc_Entropy_OnDemandTest(void);

WOLFSSL_LOCAL int Entropy_Init(void);
WOLFSSL_LOCAL void Entropy_Final(void);
#endif

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLF_CRYPT_RANDOM_H */

