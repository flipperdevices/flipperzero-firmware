/* random.c
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
This library contains implementation for the random number generator.

*/
#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

/* on HPUX 11 you may need to install /dev/random see
   http://h20293.www2.hp.com/portal/swdepot/displayProductInfo.do?productNumber=KRNG11I

*/
#if defined(ESP_IDF_VERSION_MAJOR) && ESP_IDF_VERSION_MAJOR >= 5
    #include <esp_random.h>
#endif

#if defined(HAVE_FIPS) && \
    defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2)

    /* set NO_WRAPPERS before headers, use direct internal f()s not wrappers */
    #define FIPS_NO_WRAPPERS

    #ifdef USE_WINDOWS_API
        #pragma code_seg(".fipsA$c")
        #pragma const_seg(".fipsB$c")
    #endif
#endif


#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/cpuid.h>
#ifdef HAVE_ENTROPY_MEMUSE
#include <wolfssl/wolfcrypt/sha3.h>
#if defined(__APPLE__) || defined(__MACH__)
#include <mach/mach_time.h>
#endif
#endif


#ifndef WC_NO_RNG /* if not FIPS and RNG is disabled then do not compile */

#include <wolfssl/wolfcrypt/sha256.h>

#ifdef WOLF_CRYPTO_CB
    #include <wolfssl/wolfcrypt/cryptocb.h>
#endif

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#if defined(WOLFSSL_SGX)
    #include <sgx_trts.h>
#elif defined(USE_WINDOWS_API)
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x0400
    #endif
    #include <windows.h>
    #include <wincrypt.h>
#elif defined(HAVE_WNR)
    #include <wnr.h>
    #include <wolfssl/wolfcrypt/logging.h>
    wolfSSL_Mutex wnr_mutex;    /* global netRandom mutex */
    int wnr_timeout     = 0;    /* entropy timeout, milliseconds */
    int wnr_mutex_init  = 0;    /* flag for mutex init */
    wnr_context*  wnr_ctx;      /* global netRandom context */
#elif defined(FREESCALE_KSDK_2_0_TRNG)
    #include "fsl_trng.h"
#elif defined(FREESCALE_KSDK_2_0_RNGA)
    #include "fsl_rnga.h"
#elif defined(WOLFSSL_WICED)
    #include "wiced_crypto.h"
#elif defined(WOLFSSL_NETBURNER)
    #include <predef.h>
    #include <basictypes.h>
    #include <random.h>
#elif defined(WOLFSSL_XILINX_CRYPT_VERSAL)
#include "wolfssl/wolfcrypt/port/xilinx/xil-versal-trng.h"
#elif defined(NO_DEV_RANDOM)
#elif defined(CUSTOM_RAND_GENERATE)
#elif defined(CUSTOM_RAND_GENERATE_BLOCK)
#elif defined(CUSTOM_RAND_GENERATE_SEED)
#elif defined(WOLFSSL_GENSEED_FORTEST)
#elif defined(WOLFSSL_MDK_ARM)
#elif defined(WOLFSSL_IAR_ARM)
#elif defined(WOLFSSL_ROWLEY_ARM)
#elif defined(WOLFSSL_EMBOS)
#elif defined(WOLFSSL_DEOS)
#elif defined(MICRIUM)
#elif defined(WOLFSSL_NUCLEUS)
#elif defined(WOLFSSL_PB)
#elif defined(WOLFSSL_ZEPHYR)
#elif defined(WOLFSSL_TELIT_M2MB)
#elif defined(WOLFSSL_SCE) && !defined(WOLFSSL_SCE_NO_TRNG)
#elif defined(WOLFSSL_IMXRT1170_CAAM)
#elif defined(WOLFSSL_GETRANDOM)
    #include <errno.h>
    #include <sys/random.h>
#else
    /* include headers that may be needed to get good seed */
    #include <fcntl.h>
    #ifndef EBSNET
        #include <unistd.h>
    #endif
#endif

#if defined(WOLFSSL_SILABS_SE_ACCEL)
#include <wolfssl/wolfcrypt/port/silabs/silabs_random.h>
#endif

#if defined(WOLFSSL_IOTSAFE) && defined(HAVE_IOTSAFE_HWRNG)
#include <wolfssl/wolfcrypt/port/iotsafe/iotsafe.h>
#endif

#if defined(WOLFSSL_HAVE_PSA) && !defined(WOLFSSL_PSA_NO_RNG)
#include <wolfssl/wolfcrypt/port/psa/psa.h>
#endif

#if defined(HAVE_INTEL_RDRAND) || defined(HAVE_INTEL_RDSEED) || \
    defined(HAVE_AMD_RDSEED)
    static word32 intel_flags = 0;
    static void wc_InitRng_IntelRD(void)
    {
        intel_flags = cpuid_get_flags();
    }
    #if (defined(HAVE_INTEL_RDSEED) || defined(HAVE_AMD_RDSEED)) && \
        !defined(WOLFSSL_LINUXKM)
    static int wc_GenerateSeed_IntelRD(OS_Seed* os, byte* output, word32 sz);
    #endif
    #ifdef HAVE_INTEL_RDRAND
    static int wc_GenerateRand_IntelRD(OS_Seed* os, byte* output, word32 sz);
    #endif

#ifdef USE_WINDOWS_API
    #define USE_INTEL_INTRINSICS
#elif !defined __GNUC__ || defined __clang__ || __GNUC__ > 4
    #define USE_INTEL_INTRINSICS
#else
    #undef USE_INTEL_INTRINSICS
#endif

#ifdef USE_INTEL_INTRINSICS
    #include <immintrin.h>
    /* Before clang 7 or GCC 9, immintrin.h did not define _rdseed64_step() */
    #ifndef HAVE_INTEL_RDSEED
    #elif defined __clang__ && __clang_major__ > 6
    #elif !defined __GNUC__
    #elif __GNUC__ > 8
    #else
        #ifndef __clang__
            #pragma GCC push_options
            #pragma GCC target("rdseed")
        #else
            #define __RDSEED__
        #endif
        #include <x86intrin.h>
        #ifndef __clang__
            #pragma GCC pop_options
        #endif
    #endif
#endif /* USE_WINDOWS_API */
#endif

/* Start NIST DRBG code */
#ifdef HAVE_HASHDRBG

#define OUTPUT_BLOCK_LEN  (WC_SHA256_DIGEST_SIZE)
#define MAX_REQUEST_LEN   (0x10000)
#define RESEED_INTERVAL   WC_RESEED_INTERVAL


/* The security strength for the RNG is the target number of bits of
 * entropy you are looking for in a seed. */
#ifndef RNG_SECURITY_STRENGTH
    /* SHA-256 requires a minimum of 256-bits of entropy. */
    #define RNG_SECURITY_STRENGTH (256)
#endif

#ifndef ENTROPY_SCALE_FACTOR
    /* The entropy scale factor should be the whole number inverse of the
     * minimum bits of entropy per bit of NDRNG output. */
    #if defined(HAVE_ENTROPY_MEMUSE)
        /* Full strength, conditioned entropy is requested of MemUse Entropy. */
        #if defined(HAVE_FIPS) && defined(HAVE_FIPS_VERSION) && \
            (HAVE_FIPS_VERSION >= 2)
            #define ENTROPY_SCALE_FACTOR (4)
        #else
            #define ENTROPY_SCALE_FACTOR (1)
        #endif
    #elif defined(HAVE_AMD_RDSEED)
        /* This will yield a SEED_SZ of 16kb. Since nonceSz will be 0,
         * we'll add an additional 8kb on top. */
        #define ENTROPY_SCALE_FACTOR  (512)
    #elif defined(HAVE_INTEL_RDSEED) || defined(HAVE_INTEL_RDRAND)
        /* The value of 2 applies to Intel's RDSEED which provides about
         * 0.5 bits minimum of entropy per bit. The value of 4 gives a
         * conservative margin for FIPS. */
        #if defined(HAVE_FIPS) && defined(HAVE_FIPS_VERSION) && \
            (HAVE_FIPS_VERSION >= 2)
            #define ENTROPY_SCALE_FACTOR (2*4)
        #else
            /* Not FIPS, but Intel RDSEED, only double. */
            #define ENTROPY_SCALE_FACTOR (2)
        #endif
    #elif defined(HAVE_FIPS) && defined(HAVE_FIPS_VERSION) && \
        (HAVE_FIPS_VERSION >= 2)
        /* If doing a FIPS build without a specific scale factor, default
         * to 4. This will give 1024 bits of entropy. More is better, but
         * more is also slower. */
        #define ENTROPY_SCALE_FACTOR (4)
    #else
        /* Setting the default to 1. */
        #define ENTROPY_SCALE_FACTOR (1)
    #endif
#endif

#ifndef SEED_BLOCK_SZ
    /* The seed block size, is the size of the output of the underlying NDRNG.
     * This value is used for testing the output of the NDRNG. */
    #if defined(HAVE_AMD_RDSEED)
        /* AMD's RDSEED instruction works in 128-bit blocks read 64-bits
        * at a time. */
        #define SEED_BLOCK_SZ (sizeof(word64)*2)
    #elif defined(HAVE_INTEL_RDSEED) || defined(HAVE_INTEL_RDRAND)
        /* RDSEED outputs in blocks of 64-bits. */
        #define SEED_BLOCK_SZ sizeof(word64)
    #else
        /* Setting the default to 4. */
        #define SEED_BLOCK_SZ 4
    #endif
#endif

#define SEED_SZ        (RNG_SECURITY_STRENGTH*ENTROPY_SCALE_FACTOR/8)

/* The maximum seed size will be the seed size plus a seed block for the
 * test, and an additional half of the seed size. This additional half
 * is in case the user does not supply a nonce. A nonce will be obtained
 * from the NDRNG. */
#define MAX_SEED_SZ    (SEED_SZ + SEED_SZ/2 + SEED_BLOCK_SZ)


#ifdef WC_RNG_SEED_CB

static wc_RngSeed_Cb seedCb = NULL;

int wc_SetSeed_Cb(wc_RngSeed_Cb cb)
{
    seedCb = cb;
    return 0;
}

#endif


/* Internal return codes */
#define DRBG_SUCCESS      0
#define DRBG_FAILURE      1
#define DRBG_NEED_RESEED  2
#define DRBG_CONT_FAILURE 3
#define DRBG_NO_SEED_CB   4

/* RNG health states */
#define DRBG_NOT_INIT     0
#define DRBG_OK           1
#define DRBG_FAILED       2
#define DRBG_CONT_FAILED  3

#define RNG_HEALTH_TEST_CHECK_SIZE (WC_SHA256_DIGEST_SIZE * 4)

/* Verify max gen block len */
#if RNG_MAX_BLOCK_LEN > MAX_REQUEST_LEN
    #error RNG_MAX_BLOCK_LEN is larger than NIST DBRG max request length
#endif

enum {
    drbgInitC     = 0,
    drbgReseed    = 1,
    drbgGenerateW = 2,
    drbgGenerateH = 3,
    drbgInitV     = 4
};

typedef struct DRBG_internal DRBG_internal;

static int wc_RNG_HealthTestLocal(int reseed);

/* Hash Derivation Function */
/* Returns: DRBG_SUCCESS or DRBG_FAILURE */
static int Hash_df(DRBG_internal* drbg, byte* out, word32 outSz, byte type,
                                                  const byte* inA, word32 inASz,
                                                  const byte* inB, word32 inBSz)
{
    int ret = DRBG_FAILURE;
    byte ctr;
    word32 i;
    word32 len;
    word32 bits = (outSz * 8); /* reverse byte order */
#ifdef WOLFSSL_SMALL_STACK_CACHE
    wc_Sha256* sha = &drbg->sha256;
#else
    wc_Sha256 sha[1];
#endif
#ifdef WOLFSSL_SMALL_STACK
    byte* digest;
#else
    byte digest[WC_SHA256_DIGEST_SIZE];
#endif

    if (drbg == NULL) {
        return DRBG_FAILURE;
    }

#ifdef WOLFSSL_SMALL_STACK
    digest = (byte*)XMALLOC(WC_SHA256_DIGEST_SIZE, drbg->heap,
        DYNAMIC_TYPE_DIGEST);
    if (digest == NULL)
        return DRBG_FAILURE;
#endif

#ifdef LITTLE_ENDIAN_ORDER
    bits = ByteReverseWord32(bits);
#endif
    len = (outSz / OUTPUT_BLOCK_LEN)
        + ((outSz % OUTPUT_BLOCK_LEN) ? 1 : 0);

    ctr = 1;
    for (i = 0; i < len; i++) {
#ifndef WOLFSSL_SMALL_STACK_CACHE
    #if defined(WOLFSSL_ASYNC_CRYPT) || defined(WOLF_CRYPTO_CB)
        ret = wc_InitSha256_ex(sha, drbg->heap, drbg->devId);
    #else
        ret = wc_InitSha256(sha);
    #endif
        if (ret != 0)
            break;
#endif
        ret = wc_Sha256Update(sha, &ctr, sizeof(ctr));
        if (ret == 0) {
            ctr++;
            ret = wc_Sha256Update(sha, (byte*)&bits, sizeof(bits));
        }

        if (ret == 0) {
            /* churning V is the only string that doesn't have the type added */
            if (type != drbgInitV)
                ret = wc_Sha256Update(sha, &type, sizeof(type));
        }
        if (ret == 0)
            ret = wc_Sha256Update(sha, inA, inASz);
        if (ret == 0) {
            if (inB != NULL && inBSz > 0)
                ret = wc_Sha256Update(sha, inB, inBSz);
        }
        if (ret == 0)
            ret = wc_Sha256Final(sha, digest);

#ifndef WOLFSSL_SMALL_STACK_CACHE
        wc_Sha256Free(sha);
#endif
        if (ret == 0) {
            if (outSz > OUTPUT_BLOCK_LEN) {
                XMEMCPY(out, digest, OUTPUT_BLOCK_LEN);
                outSz -= OUTPUT_BLOCK_LEN;
                out += OUTPUT_BLOCK_LEN;
            }
            else {
                XMEMCPY(out, digest, outSz);
            }
        }
    }

    ForceZero(digest, WC_SHA256_DIGEST_SIZE);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(digest, drbg->heap, DYNAMIC_TYPE_DIGEST);
#endif

    return (ret == 0) ? DRBG_SUCCESS : DRBG_FAILURE;
}

/* Returns: DRBG_SUCCESS or DRBG_FAILURE */
static int Hash_DRBG_Reseed(DRBG_internal* drbg, const byte* seed, word32 seedSz)
{
    int ret;
#ifdef WOLFSSL_SMALL_STACK
    byte* newV;
#else
    byte newV[DRBG_SEED_LEN];
#endif

    if (drbg == NULL) {
        return DRBG_FAILURE;
    }

#ifdef WOLFSSL_SMALL_STACK
    newV = (byte*)XMALLOC(DRBG_SEED_LEN, drbg->heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (newV == NULL) {
        return MEMORY_E;
    }
#endif
    XMEMSET(newV, 0, DRBG_SEED_LEN);

    ret = Hash_df(drbg, newV, DRBG_SEED_LEN, drbgReseed,
                drbg->V, sizeof(drbg->V), seed, seedSz);
    if (ret == DRBG_SUCCESS) {
        XMEMCPY(drbg->V, newV, sizeof(drbg->V));
        ForceZero(newV, DRBG_SEED_LEN);

        ret = Hash_df(drbg, drbg->C, sizeof(drbg->C), drbgInitC, drbg->V,
                                    sizeof(drbg->V), NULL, 0);
    }
    if (ret == DRBG_SUCCESS) {
        drbg->reseedCtr = 1;
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(newV, drbg->heap, DYNAMIC_TYPE_TMP_BUFFER);
#endif
    return ret;
}

/* Returns: DRBG_SUCCESS and DRBG_FAILURE or BAD_FUNC_ARG on fail */
int wc_RNG_DRBG_Reseed(WC_RNG* rng, const byte* seed, word32 seedSz)
{
    if (rng == NULL || seed == NULL) {
        return BAD_FUNC_ARG;
    }

    if (rng->drbg == NULL) {
    #if defined(HAVE_INTEL_RDSEED) || defined(HAVE_INTEL_RDRAND)
        if (IS_INTEL_RDRAND(intel_flags)) {
            /* using RDRAND not DRBG, so return success */
            return 0;
        }
        return BAD_FUNC_ARG;
    #endif
    }

    return Hash_DRBG_Reseed((DRBG_internal *)rng->drbg, seed, seedSz);
}

static WC_INLINE void array_add_one(byte* data, word32 dataSz)
{
    int i;
    for (i = (int)dataSz - 1; i >= 0; i--) {
        data[i]++;
        if (data[i] != 0) break;
    }
}

/* Returns: DRBG_SUCCESS or DRBG_FAILURE */
static int Hash_gen(DRBG_internal* drbg, byte* out, word32 outSz, const byte* V)
{
    int ret = DRBG_FAILURE;
#ifdef WOLFSSL_SMALL_STACK
    byte* data;
    byte* digest;
#else
    byte data[DRBG_SEED_LEN];
    byte digest[WC_SHA256_DIGEST_SIZE];
#endif
    word32 i;
    word32 len;
#ifdef WOLFSSL_SMALL_STACK_CACHE
    wc_Sha256* sha = &drbg->sha256;
#else
    wc_Sha256 sha[1];
#endif

    if (drbg == NULL) {
        return DRBG_FAILURE;
    }

#ifdef WOLFSSL_SMALL_STACK
    data = (byte*)XMALLOC(DRBG_SEED_LEN, drbg->heap, DYNAMIC_TYPE_TMP_BUFFER);
    digest = (byte*)XMALLOC(WC_SHA256_DIGEST_SIZE, drbg->heap,
        DYNAMIC_TYPE_DIGEST);
    if (data == NULL || digest == NULL) {
        XFREE(digest, drbg->heap, DYNAMIC_TYPE_DIGEST);
        XFREE(data, drbg->heap, DYNAMIC_TYPE_TMP_BUFFER);
        return DRBG_FAILURE;
    }
#endif

    /* Special case: outSz is 0 and out is NULL. wc_Generate a block to save for
     * the continuous test. */

    if (outSz == 0) {
        outSz = 1;
    }

    len = (outSz / OUTPUT_BLOCK_LEN) + ((outSz % OUTPUT_BLOCK_LEN) ? 1 : 0);

    XMEMCPY(data, V, DRBG_SEED_LEN);
    for (i = 0; i < len; i++) {
#ifndef WOLFSSL_SMALL_STACK_CACHE
    #if defined(WOLFSSL_ASYNC_CRYPT) || defined(WOLF_CRYPTO_CB)
        ret = wc_InitSha256_ex(sha, drbg->heap, drbg->devId);
    #else
        ret = wc_InitSha256(sha);
    #endif
        if (ret == 0)
#endif
            ret = wc_Sha256Update(sha, data, DRBG_SEED_LEN);
        if (ret == 0)
            ret = wc_Sha256Final(sha, digest);
#ifndef WOLFSSL_SMALL_STACK_CACHE
        wc_Sha256Free(sha);
#endif

        if (ret == 0) {
            if (out != NULL && outSz != 0) {
                if (outSz >= OUTPUT_BLOCK_LEN) {
                    XMEMCPY(out, digest, OUTPUT_BLOCK_LEN);
                    outSz -= OUTPUT_BLOCK_LEN;
                    out += OUTPUT_BLOCK_LEN;
                    array_add_one(data, DRBG_SEED_LEN);
                }
                else {
                    XMEMCPY(out, digest, outSz);
                    outSz = 0;
                }
            }
        }
        else {
            /* wc_Sha256Update or wc_Sha256Final returned error */
            break;
        }
    }
    ForceZero(data, DRBG_SEED_LEN);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(digest, drbg->heap, DYNAMIC_TYPE_DIGEST);
    XFREE(data, drbg->heap, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return (ret == 0) ? DRBG_SUCCESS : DRBG_FAILURE;
}

static WC_INLINE void array_add(byte* d, word32 dLen, const byte* s, word32 sLen)
{
    if (dLen > 0 && sLen > 0 && dLen >= sLen) {
        int sIdx, dIdx;
        word16 carry = 0;

        dIdx = (int)dLen - 1;
        for (sIdx = (int)sLen - 1; sIdx >= 0; sIdx--) {
            carry += (word16)(d[dIdx] + s[sIdx]);
            d[dIdx] = (byte)carry;
            carry >>= 8;
            dIdx--;
        }

        for (; dIdx >= 0; dIdx--) {
            carry += (word16)d[dIdx];
            d[dIdx] = (byte)carry;
            carry >>= 8;
        }
    }
}

/* Returns: DRBG_SUCCESS, DRBG_NEED_RESEED, or DRBG_FAILURE */
static int Hash_DRBG_Generate(DRBG_internal* drbg, byte* out, word32 outSz)
{
    int ret;
#ifdef WOLFSSL_SMALL_STACK_CACHE
    wc_Sha256* sha = &drbg->sha256;
#else
    wc_Sha256 sha[1];
#endif
    byte type;
    word32 reseedCtr;

    if (drbg == NULL) {
        return DRBG_FAILURE;
    }

    if (drbg->reseedCtr == RESEED_INTERVAL) {
        return DRBG_NEED_RESEED;
    }
    else {
    #ifndef WOLFSSL_SMALL_STACK
        byte digest[WC_SHA256_DIGEST_SIZE];
    #else
        byte* digest = (byte*)XMALLOC(WC_SHA256_DIGEST_SIZE, drbg->heap,
            DYNAMIC_TYPE_DIGEST);
        if (digest == NULL)
            return DRBG_FAILURE;
    #endif

        type = drbgGenerateH;
        reseedCtr = drbg->reseedCtr;

        ret = Hash_gen(drbg, out, outSz, drbg->V);
        if (ret == DRBG_SUCCESS) {
#ifndef WOLFSSL_SMALL_STACK_CACHE
        #if defined(WOLFSSL_ASYNC_CRYPT) || defined(WOLF_CRYPTO_CB)
            ret = wc_InitSha256_ex(sha, drbg->heap, drbg->devId);
        #else
            ret = wc_InitSha256(sha);
        #endif
            if (ret == 0)
#endif
                ret = wc_Sha256Update(sha, &type, sizeof(type));
            if (ret == 0)
                ret = wc_Sha256Update(sha, drbg->V, sizeof(drbg->V));
            if (ret == 0)
                ret = wc_Sha256Final(sha, digest);

#ifndef WOLFSSL_SMALL_STACK_CACHE
            wc_Sha256Free(sha);
#endif

            if (ret == 0) {
                array_add(drbg->V, sizeof(drbg->V), digest, WC_SHA256_DIGEST_SIZE);
                array_add(drbg->V, sizeof(drbg->V), drbg->C, sizeof(drbg->C));
            #ifdef LITTLE_ENDIAN_ORDER
                reseedCtr = ByteReverseWord32(reseedCtr);
            #endif
                array_add(drbg->V, sizeof(drbg->V),
                                          (byte*)&reseedCtr, sizeof(reseedCtr));
                ret = DRBG_SUCCESS;
            }
            drbg->reseedCtr++;
        }
        ForceZero(digest, WC_SHA256_DIGEST_SIZE);
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(digest, drbg->heap, DYNAMIC_TYPE_DIGEST);
    #endif
    }

    return (ret == 0) ? DRBG_SUCCESS : DRBG_FAILURE;
}

/* Returns: DRBG_SUCCESS or DRBG_FAILURE */
static int Hash_DRBG_Instantiate(DRBG_internal* drbg, const byte* seed, word32 seedSz,
                                             const byte* nonce, word32 nonceSz,
                                             void* heap, int devId)
{
    int ret = DRBG_FAILURE;

    XMEMSET(drbg, 0, sizeof(DRBG_internal));
    drbg->heap = heap;
#if defined(WOLFSSL_ASYNC_CRYPT) || defined(WOLF_CRYPTO_CB)
    drbg->devId = devId;
#else
    (void)devId;
#endif

#ifdef WOLFSSL_SMALL_STACK_CACHE
    #if defined(WOLFSSL_ASYNC_CRYPT) || defined(WOLF_CRYPTO_CB)
        ret = wc_InitSha256_ex(&drbg->sha256, drbg->heap, drbg->devId);
    #else
        ret = wc_InitSha256(&drbg->sha256);
    #endif
    if (ret != 0)
        return ret;
#endif

    if (Hash_df(drbg, drbg->V, sizeof(drbg->V), drbgInitV, seed, seedSz,
                                              nonce, nonceSz) == DRBG_SUCCESS &&
        Hash_df(drbg, drbg->C, sizeof(drbg->C), drbgInitC, drbg->V,
                                    sizeof(drbg->V), NULL, 0) == DRBG_SUCCESS) {

        drbg->reseedCtr = 1;
        ret = DRBG_SUCCESS;
    }

    return ret;
}

/* Returns: DRBG_SUCCESS or DRBG_FAILURE */
static int Hash_DRBG_Uninstantiate(DRBG_internal* drbg)
{
    word32 i;
    int    compareSum = 0;
    byte*  compareDrbg = (byte*)drbg;

#ifdef WOLFSSL_SMALL_STACK_CACHE
    wc_Sha256Free(&drbg->sha256);
#endif

    ForceZero(drbg, sizeof(DRBG_internal));

    for (i = 0; i < sizeof(DRBG_internal); i++) {
        compareSum |= compareDrbg[i] ^ 0;
    }

    return (compareSum == 0) ? DRBG_SUCCESS : DRBG_FAILURE;
}


int wc_RNG_TestSeed(const byte* seed, word32 seedSz)
{
    int ret = 0;

    /* Check the seed for duplicate words. */
    word32 seedIdx = 0;
    word32 scratchSz = min(SEED_BLOCK_SZ, seedSz - SEED_BLOCK_SZ);

    while (seedIdx < seedSz - SEED_BLOCK_SZ) {
        if (ConstantCompare(seed + seedIdx,
                            seed + seedIdx + scratchSz,
                            (int)scratchSz) == 0) {

            ret = DRBG_CONT_FAILURE;
        }
        seedIdx += SEED_BLOCK_SZ;
        scratchSz = min(SEED_BLOCK_SZ, (seedSz - seedIdx));
    }

    return ret;
}
#endif /* HAVE_HASHDRBG */
/* End NIST DRBG Code */

#ifdef HAVE_ENTROPY_MEMUSE
/* Define ENTROPY_MEMUSE_THREAD to force use of counter in a new thread.
 * Only do this when high resolution timer not otherwise available.
 */

/* Number of bytes that will hold the maximum entropy bits. */
#define MAX_ENTROPY_BYTES    (MAX_ENTROPY_BITS / 8)
/* Number of bits stored for one sample. */
#define ENTROPY_BITS_USED    8

/* Minimum entropy from a sample. */
#define ENTROPY_MIN          1
/* Number of extra samples to ensure full entropy. */
#define ENTROPY_EXTRA        64
/* Maximum number of bytes to sample to produce max entropy. */
#define MAX_NOISE_CNT        (MAX_ENTROPY_BITS * 8 + ENTROPY_EXTRA)

/* MemUse entropy global state initialized. */
static int entropy_memuse_initialized = 0;
/* Global SHA-3 object used for conditioning entropy and creating noise. */
static wc_Sha3 entropyHash;
/* Reset the health tests. */
static void Entropy_HealthTest_Reset(void);

#if !defined(ENTROPY_MEMUSE_THREAD) && \
    (defined(__x86_64__) || defined(__i386__))
/* Get the high resolution time counter.
 *
 * @return  64-bit count of CPU cycles.
 */
static WC_INLINE word64 Entropy_TimeHiRes(void)
{
    unsigned int lo_c, hi_c;
    __asm__ __volatile__ (
        "rdtsc"
            : "=a"(lo_c), "=d"(hi_c)   /* out */
            : "a"(0)                   /* in */
            : "%ebx", "%ecx");         /* clobber */
    return ((word64)lo_c) | (((word64)hi_c) << 32);
}
#elif !defined(ENTROPY_MEMUSE_THREAD) && \
      (defined(__APPLE__) || defined(__MACH__))
/* Get the high resolution time counter.
 *
 * @return  64-bit time in nanoseconds.
 */
static WC_INLINE word64 Entropy_TimeHiRes(void)
{
    return mach_absolute_time();
}
#elif !defined(ENTROPY_MEMUSE_THREAD) && defined(__aarch64__)
/* Get the high resolution time counter.
 *
 * @return  64-bit timer count.
 */
static WC_INLINE word64 Entropy_TimeHiRes(void)
{
    word64 cnt;
    __asm__ __volatile__ (
        "mrs %[cnt], cntvct_el0"
        : [cnt] "=r"(cnt)
        :
        :
    );
    return cnt;
}
#elif !defined(ENTROPY_MEMUSE_THREAD) && (_POSIX_C_SOURCE >= 199309L)
/* Get the high resolution time counter.
 *
 * @return  64-bit time that is the nanoseconds of current time.
 */
static WC_INLINE word64 Entropy_TimeHiRes(void)
{
    struct timespec now;

    clock_gettime(CLOCK_REALTIME, &now);

    return now.tv_nsec;
}
#elif defined(_WIN32) /* USE_WINDOWS_API */
/* Get the high resolution time counter.
 *
 * @return  64-bit timer
 */
static WC_INLINE word64 Entropy_TimeHiRes(void)
{
    LARGE_INTEGER count;
    QueryPerformanceCounter(&count);
    return (word64)(count.QuadPart);
}
#elif defined(WOLFSSL_THREAD_NO_JOIN)

/* Start and stop thread that counts as a proxy for time counter. */
#define ENTROPY_MEMUSE_THREADED

/* Data for entropy thread. */
typedef struct ENTROPY_THREAD_DATA {
    /* Current counter - proxy for time. */
    word64 counter;
    /* Whether to stop thread. */
    int stop;
} ENTROPY_THREAD_DATA;

/* Track whether entropy thread has been started already. */
static int entropy_thread_started = 0;
/* Data for thread to update/observer. */
static volatile ENTROPY_THREAD_DATA entropy_thread_data = { 0, 0 };

/* Get the high resolution time counter. Counter incremented in thread.
 *
 * @return  64-bit counter.
 */
static WC_INLINE word64 Entropy_TimeHiRes(void)
{
    /* Return counter update in thread. */
    return entropy_thread_data.counter;
}

/* Thread that increments counter while not told to stop.
 *
 * @param [in,out] args  Entropy data including: counter and stop flag.
 * @return  NULL always.
 */
static THREAD_RETURN WOLFSSL_THREAD_NO_JOIN Entropy_IncCounter(void* args)
{
    (void)args;

    /* Keep going until caller tells us to stop and exit. */
    while (!entropy_thread_data.stop) {
        /* Increment counter acting as high resolution timer. */
        entropy_thread_data.counter++;
    }

#ifdef WOLFSSL_DEBUG_ENTROPY_MEMUSE
    fprintf(stderr, "EXITING ENTROPY COUNTER THREAD\n");
#endif
    /* Exit from thread. */
    WOLFSSL_RETURN_FROM_THREAD(0);
}

/* Start a thread that increments counter if not one already.
 *
 * Won't start a new thread if one already running.
 * Waits for thread to start by waiting for counter to have incremented.
 *
 * @return  0 on success.
 * @return  Negative on failure.
 */
static int Entropy_StartThread(void)
{
    int ret = 0;

    /* Only continue if we haven't started a thread. */
    if (!entropy_thread_started) {
        /* Get counter before starting thread. */
        word64 start_counter = entropy_thread_data.counter;

        /* In case of restarting thread, set stop indicator to false. */
        entropy_thread_data.stop = 0;

    #ifdef WOLFSSL_DEBUG_ENTROPY_MEMUSE
        fprintf(stderr, "STARTING ENTROPY COUNTER THREAD\n");
    #endif
        /* Create a thread that increments the counter in the data. */
        /* Thread resources to be disposed of. */
        ret = wolfSSL_NewThreadNoJoin(Entropy_IncCounter, NULL);
        if (ret == 0) {
            /* Wait for the counter to increase indicating thread started. */
            while (entropy_thread_data.counter == start_counter) {
                sched_yield();
            }
        }

        entropy_thread_started = (ret == 0);
    }

    return ret;
}

/* Tell thread to stop and wait for it to complete.
 *
 * Called by wolfCrypt_Cleanup().
 */
static void Entropy_StopThread(void)
{
    /* Only stop a thread if one is running. */
    if (entropy_thread_started) {
        /* Tell thread to stop. */
        entropy_thread_data.stop = 1;
        /* Stopped thread so no thread started anymore. */
        entropy_thread_started = 0;
    }
}
    /* end if defined(HAVE_PTHREAD) */

#else

#error "No high precision time available for MemUse Entropy."

#endif

#ifndef ENTROPY_NUM_WORDS_BITS
    /* Number of bits to count of 64-bit words in state. */
    #define ENTROPY_NUM_WORDS_BITS      14
#endif

/* Floor of 8 yields pool of 256x 64-bit word samples
 * 9  -> 512x 64-bit word samples
 * 10 -> 1,024x 64-bit word samples
 * 11 -> 2,048x 64-bit word samples
 * 12 -> 4,096x 64-bit word samples
 * 13 -> 8,192x 64-bit word samples
 * 14 -> 16,384x 64-bit word samples
 * 15 -> 32,768x 64-bit word samples
 * ... doubling every time up to a maximum of:
 * 30 -> 1,073,741,824x 64-bit word samples
 * 1 billion+ samples should be more then sufficient for any use-case
 */
#if ENTROPY_NUM_WORDS_BITS < 8
    #error "ENTROPY_NUM_WORDS_BITS must be 8 or more"
#elif ENTROPY_NUM_WORDS_BITS > 30
    #error "ENTROPY_NUM_WORDS_BITS must be less than 31"
#endif
/* Number of 64-bit words in state. */
#define ENTROPY_NUM_WORDS               (1 << ENTROPY_NUM_WORDS_BITS)

/* Size of one block of 64-bit words. */
#define ENTROPY_BLOCK_SZ                (ENTROPY_NUM_WORDS_BITS - 8)

#ifndef ENTROPY_NUM_UPDATES
    /* Number of times to update random blocks.
     * Less than 2^ENTROPY_BLOCK_SZ (default: 2^6 = 64).
     * Maximize value to maximize entropy per sample.
     * Limit value to ensure entropy is collected in a timely manner.
     */
    #define ENTROPY_NUM_UPDATES         18
    /* Upper round of log2(ENTROPY_NUM_UPDATES) */
    #define ENTROPY_NUM_UPDATES_BITS    5
#elif !defined(ENTROPY_NUM_UPDATES_BITS)
    #define ENTROPY_NUM_UPDATES_BITS     ENTROPY_BLOCK_SZ
#endif
/* Amount to shift offset to get better coverage of a block */
#define ENTROPY_OFFSET_SHIFTING          \
    (ENTROPY_BLOCK_SZ / ENTROPY_NUM_UPDATES_BITS)

#ifndef ENTROPY_NUM_64BIT_WORDS
    /* Number of 64-bit words to update - 32. */
    #define ENTROPY_NUM_64BIT_WORDS     WC_SHA3_256_DIGEST_SIZE
#elif ENTROPY_NUM_64BIT_WORDS > WC_SHA3_256_DIGEST_SIZE
    #error "ENTROPY_NUM_64BIT_WORDS must be <= SHA3-256 digest size in bytes"
#endif

/* State to update that is multiple cache lines long. */
static word64 entropy_state[ENTROPY_NUM_WORDS] = {0};

/* Using memory will take different amount of times depending on the CPU's
 * caches and business.
 */
static void Entropy_MemUse(void)
{
    int i;
    static byte d[WC_SHA3_256_DIGEST_SIZE];
    int j;

    for (j = 0; j < ENTROPY_NUM_UPDATES; j++) {
        /* Hash the first 32 64-bit words of state. */
        wc_Sha3_256_Update(&entropyHash, (byte*)entropy_state,
            sizeof(*entropy_state) * ENTROPY_NUM_64BIT_WORDS);
        /* Get pseudo-random indices. */
        wc_Sha3_256_Final(&entropyHash, d);

        for (i = 0; i < ENTROPY_NUM_64BIT_WORDS; i++) {
            /* Choose a 64-bit word from a pseudo-random block.*/
            int idx = ((int)d[i] << ENTROPY_BLOCK_SZ) +
                      (j << ENTROPY_OFFSET_SHIFTING);
            /* Update a pseudo-random 64-bit word with a pseudo-random value. */
            entropy_state[idx] += Entropy_TimeHiRes();
            /* Ensure part of state that is hashed is updated. */
            entropy_state[i] += entropy_state[idx];
        }
    }
}


/* Last time entropy sample was gathered. */
static word64 entropy_last_time = 0;

/* Get a sample of noise.
 *
 * Value is time taken to use memory.
 *
 * Called to test raw entropy.
 *
 * @return  64-bit value that is the noise.
 */
static word64 Entropy_GetSample(void)
{
    word64 now;
    word64 ret;

    /* Use memory such that it will take an unpredictable amount of time. */
    Entropy_MemUse();

    /* Get the time now to subtract from previous end time. */
    now = Entropy_TimeHiRes();
    /* Calculate time diff since last sampling. */
    ret = now - entropy_last_time;
    /* Store last time. */
    entropy_last_time = now;

    return ret;
}

/* Get as many samples of noise as required.
 *
 * One sample is one byte.
 *
 * @param [out] noise    Buffer to hold samples.
 * @param [in]  samples  Number of one byte samples to get.
 */
static void Entropy_GetNoise(unsigned char* noise, int samples)
{
    int i;

    /* Do it once to get things going. */
    Entropy_MemUse();

    /* Get as many samples as required. */
    for (i = 0; i < samples; i++) {
       noise[i] = (byte)Entropy_GetSample();
    }
}

/* Generate raw entropy for performing assessment.
 *
 * @param [out] raw  Buffer to hold raw entropy data.
 * @param [in]  cnt  Number of bytes of raw entropy to get.
 * @return  0 on success.
 * @return  Negative when creating a thread fails - when no high resolution
 * clock available.
 */
int wc_Entropy_GetRawEntropy(unsigned char* raw, int cnt)
{
    int ret = 0;

#ifdef ENTROPY_MEMUSE_THREADED
    /* Start the counter thread as a proxy for time counter. */
    ret = Entropy_StartThread();
    if (ret == 0)
#endif
    {
        Entropy_GetNoise(raw, cnt);
    }
#ifdef ENTROPY_MEMUSE_THREADED
    /* Stop the counter thread to avoid thrashing the system. */
    Entropy_StopThread();
#endif

    return ret;
}

#if ENTROPY_MIN == 1
/* SP800-90b 4.4.1 - Repetition Test
 * C = 1 + upper(-log2(alpha) / H)
 * When alpha = 2^-30 and H = 1,
 * C = 1 + upper(30 / 1) = 31
 */
#define REP_CUTOFF           31
#else
#error "Minimum entropy not defined to a recognized value."
#endif

/* Have valid previous sample for repetition test. */
static int rep_have_prev = 0;
/* Previous sample value. */
static byte rep_prev_noise;

static void Entropy_HealthTest_Repetition_Reset(void)
{
    /* No previous stored. */
    rep_have_prev = 0;
    /* Clear previous. */
    rep_prev_noise = 0;
}

/* Test sample value with repetition test.
 *
 * @param [in] noise  Sample to test.
 * @return  0 on success.
 * @return  ENTROPY_RT_E on failure.
 */
static int Entropy_HealthTest_Repetition(byte noise)
{
    int ret = 0;
    /* Number of times previous value has been seen continuously. */
    static int rep_cnt = 0;

    /* If we don't have a previous then store this one for next time. */
    if (!rep_have_prev) {
        rep_prev_noise = noise;
        rep_have_prev = 1;
    }
    /* Check whether this sample matches last. */
    else if (noise == rep_prev_noise) {
        /* Update count of repetitions. */
        rep_cnt++;
        /* Fail if we reach cutoff. */
        if (rep_cnt >= REP_CUTOFF) {
        #ifdef WOLFSSL_DEBUG_ENTROPY_MEMUSE
            fprintf(stderr, "REPETITION FAILED: %d\n", noise);
        #endif
            Entropy_HealthTest_Repetition_Reset();
            ret = ENTROPY_RT_E;
        }
    }
    else {
        /* Cache new previous and seen one so far. */
        rep_prev_noise = noise;
        rep_cnt = 1;
    }

    return ret;
}

/* SP800-90b 4.4.2 - Adaptive Proportion Test
 * Para 2
 *   ... The window size W is selected based on the alphabet size ... 512 if
 *   the noise source is not binary ...
 */
#define PROP_WINDOW_SIZE     512
#if ENTROPY_MIN == 1
/* SP800-90b 4.4.2 - Adaptive Proportion Test
 * Note 10
 * C = 1 + CRITBINOM(W, power(2,( -H)),1-alpha)
 * alpa = 2^-30 = POWER(2,-30), H = 1, W = 512
 * C = 1 + CRITBINOM(512, 0.5, 1-POWER(2,-30)) = 1 + 324 = 325
 */
#define PROP_CUTOFF          325
#else
#error "Minimum entropy not defined to a recognized value."
#endif

/* Total number of samples storef for Adaptive proportion test.
 * Need the next 512 samples to compare this this one.
 */
static word16 prop_total = 0;
/* Index of first sample. */
static word16 prop_first = 0;
/* Index to put next sample in. */
static word16 prop_last = 0;
/* Count of each value seen in queue. */
static word16 prop_cnt[1 << ENTROPY_BITS_USED] = { 0 };
/* Circular queue of samples. */
static word16 prop_samples[PROP_WINDOW_SIZE];

/* Resets the data for the Adaptive Proportion Test.
 */
static void Entropy_HealthTest_Proportion_Reset(void)
{
    /* Clear out samples. */
    XMEMSET(prop_samples, 0, sizeof(prop_samples));
    /* Clear out counts. */
    XMEMSET(prop_cnt, 0, sizeof(prop_cnt));
    /* Clear stored count. */
    prop_total = 0;
    /* Reset first and last index for samples. */
    prop_first = 0;
    prop_last = 0;
}

/* Add sample to Adaptive Proportion test.
 *
 * SP800-90b 4.4.2 - Adaptive Proportion Test
 *
 * Sample is accumulated into buffer until required successive values seen.
 *
 * @param [in] noise  Sample to test.
 * @return  0 on success.
 * @return  ENTROPY_APT_E on failure.
 */
static int Entropy_HealthTest_Proportion(byte noise)
{
    int ret = 0;

    /* Need at least 512-1 samples to test with. */
    if (prop_total < PROP_WINDOW_SIZE - 1) {
        /* Store sample at last position in circular queue. */
        prop_samples[prop_last++] = noise;
        /* Update count of seen value based on new sample. */
        prop_cnt[noise]++;
        /* Update count of store values. */
        prop_total++;
    }
    else {
        /* Get first value in queue - value to test. */
        byte val = (byte)prop_samples[prop_first];
        /* Store new sample in queue. */
        prop_samples[prop_last] = noise;
        /* Update first index now that we have removed in from the queue. */
        prop_first = (prop_first + 1) % PROP_WINDOW_SIZE;
        /* Update last index now that we have added new sample to queue. */
        prop_last = (prop_last + 1) % PROP_WINDOW_SIZE;
        /* Removed sample from queue - remove count. */
        prop_cnt[val]--;
        /* Added sample to queue - add count. */
        prop_cnt[noise]++;
        /* Check whether removed value has too many repetitions in queue. */
        if (prop_cnt[val] >= PROP_CUTOFF) {
        #ifdef WOLFSSL_DEBUG_ENTROPY_MEMUSE
            fprintf(stderr, "PROPORTION FAILED: %d %d\n", val, prop_cnt[val]);
        #endif
            Entropy_HealthTest_Proportion_Reset();
            /* Error code returned. */
            ret = ENTROPY_APT_E;
        }
    }

    return ret;
}

/* SP800-90b 4.3 - Requirements for Health Tests
 * 1.4: The entropy source's startup tests shall run the continuous health
 * tests over at least 1024 consecutive samples.
 *
 * Adaptive Proportion Test requires a number of samples to compared too.
 */
#define ENTROPY_INITIAL_COUNT   (1024 + PROP_WINDOW_SIZE)

/* Perform startup health testing.
 *
 * Fill adaptive proportion test buffer and then do 1024 samples.
 * Perform repetition test on all samples expect last.
 *
 * Discards samples from health tests on failure.
 *
 * @return  0 on success.
 * @return  ENTROPY_RT_E or ENTROPY_APT_E on failure.
 */
static int Entropy_HealthTest_Startup(void)
{
    int ret = 0;
    byte initial[ENTROPY_INITIAL_COUNT];
    int i;

#ifdef WOLFSSL_DEBUG_ENTROPY_MEMUSE
    fprintf(stderr, "STARTUP HEALTH TEST\n");
#endif
    /* Fill initial sample buffer with noise. */
    Entropy_GetNoise(initial, ENTROPY_INITIAL_COUNT);
    /* Health check initial noise. */
    for (i = 0; (ret == 0) && (i < ENTROPY_INITIAL_COUNT); i++) {
        ret = Entropy_HealthTest_Repetition(initial[i]);
        if (ret == 0) {
            ret = Entropy_HealthTest_Proportion(initial[i]);
        }
    }

    if (ret != 0) {
        /* Failing test only resets its own data. */
        Entropy_HealthTest_Reset();
    }

    return ret;
}

/* Condition raw entropy noise using SHA-3-256.
 *
 * Put noise into a hash function: SHA-3-256.
 * Add the current time counter to help with uniqueness.
 *
 * @param [out]  output     Buffer to conditioned data.
 * @param [in]   len        Number of bytes to put into output buffer.
 * @param [in]   noise      Buffer with raw noise data.
 * @param [in]   noise_len  Length of noise data in bytes.
 * @return  0 on success.
 * @return  Negative on failure.
 */
static int Entropy_Condition(byte* output, word32 len, byte* noise,
    word32 noise_len)
{
    int ret;

    /* Add noise to initialized hash. */
    ret = wc_Sha3_256_Update(&entropyHash, noise, noise_len);
    if (ret == 0) {
        word64 now = Entropy_TimeHiRes();
        /* Add time now counter. */
        ret = wc_Sha3_256_Update(&entropyHash, (byte*)&now, sizeof(now));
    }
    if (ret == 0) {
        /* Finalize into output buffer. */
        if (len == WC_SHA3_256_DIGEST_SIZE) {
            ret = wc_Sha3_256_Final(&entropyHash, output);
        }
        else {
            byte hash[WC_SHA3_256_DIGEST_SIZE];

            ret = wc_Sha3_256_Final(&entropyHash, hash);
            if (ret == 0) {
                XMEMCPY(output, hash, len);
            }
        }
    }

    return ret;
}

/* Mutex to prevent multiple callers requesting entropy operations at the
 * same time.
 */
static wolfSSL_Mutex entropy_mutex;

/* Get entropy of specified strength.
 *
 * SP800-90b 2.3.1 - GetEntropy: An Interface to the Entropy Source
 *
 * In threaded environment, only one thread at a time can get entropy.
 *
 * @param [in]  bits     Number of entropy bits. 256 is max value.
 * @param [out] entropy  Buffer to hold entropy.
 * @param [in]  len      Length of data to put into buffer in bytes.
 * @return  0 on success.
 * @return  ENTROPY_RT_E or ENTROPY_APT_E on failure.
 * @return  BAD_MUTEX_E when unable to lock mutex.
 */
int wc_Entropy_Get(int bits, unsigned char* entropy, word32 len)
{
    int ret = 0;
    byte noise[MAX_NOISE_CNT];
    /* Noise length is the number of 8 byte samples required to get the bits of
     * entropy requested. */
    int noise_len = (bits + ENTROPY_EXTRA) / ENTROPY_MIN;

    /* Lock the mutex as collection uses globals. */
    if (wc_LockMutex(&entropy_mutex) != 0) {
        ret = BAD_MUTEX_E;
    }

#ifdef ENTROPY_MEMUSE_THREADED
    if (ret == 0) {
        /* Start the counter thread as a proxy for time counter. */
        ret = Entropy_StartThread();
    }
#endif

    /* Check we have had a startup health check pass. */
    if ((ret == 0) && ((prop_total == 0) || (!rep_have_prev))) {
        /* Try again as check failed. */
        ret = Entropy_HealthTest_Startup();
    }

    /* Keep putting data into buffer until full. */
    while ((ret == 0) && (len > 0)) {
        int i;
        word32 entropy_len = WC_SHA3_256_DIGEST_SIZE;

        /* Output 32 bytes at a time unless buffer has fewer bytes remaining. */
        if (len < entropy_len) {
            entropy_len = len;
        }

        /* Get raw entropy noise. */
        Entropy_GetNoise(noise, noise_len);
        /* Health check each noise value. */
        for (i = 0; (ret == 0) && (i < noise_len); i++) {
            ret = Entropy_HealthTest_Repetition(noise[i]);
            if (ret == 0) {
                ret = Entropy_HealthTest_Proportion(noise[i]);
            }
        }

        if (ret == 0) {
            /* Condition noise value down to 32-bytes or less. */
            ret = Entropy_Condition(entropy, entropy_len, noise, noise_len);
        }
        if (ret == 0) {
            /* Update buffer pointer and count of bytes left to generate. */
            entropy += entropy_len;
            len -= entropy_len;
        }
    }

#ifdef ENTROPY_MEMUSE_THREADED
    /* Stop the counter thread to avoid thrashing the system. */
    Entropy_StopThread();
#endif

    if (ret != BAD_MUTEX_E) {
        /* Unlock mutex now we are done. */
        wc_UnLockMutex(&entropy_mutex);
    }

    return ret;
}

/* Performs on-demand testing.
 *
 * In threaded environment, locks out other threads from getting entropy.
 *
 * @return  0 on success.
 * @return  ENTROPY_RT_E or ENTROPY_APT_E on failure.
 * @return  BAD_MUTEX_E when unable to lock mutex.
 */
int wc_Entropy_OnDemandTest()
{
    int ret = 0;

    /* Lock the mutex as we don't want collecting to happen during testing. */
    if (wc_LockMutex(&entropy_mutex) != 0) {
        ret = BAD_MUTEX_E;
    }

    if (ret == 0) {
        /* Reset health test state for startup test. */
        Entropy_HealthTest_Reset();
        /* Perform startup tests. */
        ret = Entropy_HealthTest_Startup();
    }

    if (ret != BAD_MUTEX_E) {
        /* Unlock mutex now we are done. */
        wc_UnLockMutex(&entropy_mutex);
    }
    return ret;
}

/* Initialize global state for MemUse Entropy and do startup health test.
 *
 * @return  0 on success.
 * @return  Negative on failure.
 */
int Entropy_Init()
{
    int ret = 0;

    /* Check whether initialization has succeeded before. */
    if (!entropy_memuse_initialized) {
    #ifndef SINGLE_THREADED
        ret = wc_InitMutex(&entropy_mutex);
    #endif
        if (ret == 0) {
            /* Initialize a SHA3-256 object for use in entropy operations. */
            ret = wc_InitSha3_256(&entropyHash, NULL, INVALID_DEVID);
        }
        /* Set globals initialized. */
        entropy_memuse_initialized = (ret == 0);
        if (ret == 0) {
        #ifdef ENTROPY_MEMUSE_THREADED
            /* Start the counter thread as a proxy for time counter. */
            ret = Entropy_StartThread();
            if (ret == 0)
        #endif
            {
                /* Do first startup test now. */
                ret = Entropy_HealthTest_Startup();
            }
        #ifdef ENTROPY_MEMUSE_THREADED
            /* Stop the counter thread to avoid thrashing the system. */
            Entropy_StopThread();
        #endif
        }
    }

    return ret;
}

/* Finalize the data associated with the MemUse Entropy source.
 */
void Entropy_Final()
{
    /* Only finalize when initialized. */
    if (entropy_memuse_initialized) {
        /* Dispose of the SHA3-356 hash object. */
        wc_Sha3_256_Free(&entropyHash);
    #ifndef SINGLE_THREADED
        wc_FreeMutex(&entropy_mutex);
    #endif
        /* Clear health test data. */
        Entropy_HealthTest_Reset();
        /* No longer initialized. */
        entropy_memuse_initialized = 0;
    }
}

/* Reset the data associated with the MemUse Entropy health tests.
 */
static void Entropy_HealthTest_Reset(void)
{
    Entropy_HealthTest_Repetition_Reset();
    Entropy_HealthTest_Proportion_Reset();
}

#endif /* HAVE_ENTROPY_MEMUSE */

static int _InitRng(WC_RNG* rng, byte* nonce, word32 nonceSz,
                    void* heap, int devId)
{
    int ret = 0;
#ifdef HAVE_HASHDRBG
    word32 seedSz = SEED_SZ + SEED_BLOCK_SZ;
#endif

    (void)nonce;
    (void)nonceSz;

    if (rng == NULL)
        return BAD_FUNC_ARG;
    if (nonce == NULL && nonceSz != 0)
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_HEAP_TEST
    rng->heap = (void*)WOLFSSL_HEAP_TEST;
    (void)heap;
#else
    rng->heap = heap;
#endif
#if defined(WOLFSSL_ASYNC_CRYPT) || defined(WOLF_CRYPTO_CB)
    rng->devId = devId;
    #if defined(WOLF_CRYPTO_CB)
        rng->seed.devId = devId;
    #endif
#else
    (void)devId;
#endif

#ifdef HAVE_HASHDRBG
    /* init the DBRG to known values */
    rng->drbg = NULL;
    rng->status = DRBG_NOT_INIT;
#endif

#if defined(HAVE_INTEL_RDSEED) || defined(HAVE_INTEL_RDRAND) || \
    defined(HAVE_AMD_RDSEED)
    /* init the intel RD seed and/or rand */
    wc_InitRng_IntelRD();
#endif

    /* configure async RNG source if available */
#ifdef WOLFSSL_ASYNC_CRYPT
    ret = wolfAsync_DevCtxInit(&rng->asyncDev, WOLFSSL_ASYNC_MARKER_RNG,
                                                        rng->heap, rng->devId);
    if (ret != 0) {
    #ifdef HAVE_HASHDRBG
        rng->status = DRBG_OK;
    #endif
        return ret;
    }
#endif

#ifdef HAVE_INTEL_RDRAND
    /* if CPU supports RDRAND, use it directly and by-pass DRBG init */
    if (IS_INTEL_RDRAND(intel_flags)) {
    #ifdef HAVE_HASHDRBG
        rng->status = DRBG_OK;
    #endif
        return 0;
    }
#endif

#ifdef WOLFSSL_XILINX_CRYPT_VERSAL
    ret = wc_VersalTrngInit(nonce, nonceSz);
    if (ret) {
    #ifdef HAVE_HASHDRBG
        rng->status = DRBG_OK;
    #endif
        return ret;
    }
#endif

#ifdef CUSTOM_RAND_GENERATE_BLOCK
    ret = 0; /* success */
#else
#ifdef HAVE_HASHDRBG
    if (nonceSz == 0)
        seedSz = MAX_SEED_SZ;

    if (wc_RNG_HealthTestLocal(0) == 0) {
    #ifndef WOLFSSL_SMALL_STACK
        byte seed[MAX_SEED_SZ];
    #else
        byte* seed = (byte*)XMALLOC(MAX_SEED_SZ, rng->heap,
            DYNAMIC_TYPE_SEED);
        if (seed == NULL)
            return MEMORY_E;
    #endif

#if !defined(WOLFSSL_NO_MALLOC) || defined(WOLFSSL_STATIC_MEMORY)
        rng->drbg =
                (struct DRBG*)XMALLOC(sizeof(DRBG_internal), rng->heap,
                                                          DYNAMIC_TYPE_RNG);
        if (rng->drbg == NULL) {
            ret = MEMORY_E;
            rng->status = DRBG_FAILED;
        }
#else
        rng->drbg = (struct DRBG*)&rng->drbg_data;
#endif
        if (ret == 0) {
#ifdef WC_RNG_SEED_CB
            if (seedCb == NULL) {
                ret = DRBG_NO_SEED_CB;
            }
            else {
                ret = seedCb(&rng->seed, seed, seedSz);
                if (ret != 0) {
                    ret = DRBG_FAILURE;
                }
            }
#else
            ret = wc_GenerateSeed(&rng->seed, seed, seedSz);
#endif
            if (ret == 0)
                ret = wc_RNG_TestSeed(seed, seedSz);
            else {
                ret = DRBG_FAILURE;
                rng->status = DRBG_FAILED;
            }

            if (ret == DRBG_SUCCESS)
                ret = Hash_DRBG_Instantiate((DRBG_internal *)rng->drbg,
                            seed + SEED_BLOCK_SZ, seedSz - SEED_BLOCK_SZ,
                            nonce, nonceSz, rng->heap, devId);

            if (ret != DRBG_SUCCESS) {
            #if !defined(WOLFSSL_NO_MALLOC) || defined(WOLFSSL_STATIC_MEMORY)
                XFREE(rng->drbg, rng->heap, DYNAMIC_TYPE_RNG);
            #endif
                rng->drbg = NULL;
            }
        }

        ForceZero(seed, seedSz);
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(seed, rng->heap, DYNAMIC_TYPE_SEED);
    #endif
    }
    else {
        ret = DRBG_CONT_FAILURE;
    }

    if (ret == DRBG_SUCCESS) {
#ifdef WOLFSSL_CHECK_MEM_ZERO
#ifdef HAVE_HASHDRBG
        struct DRBG_internal* drbg = (struct DRBG_internal*)rng->drbg;
        wc_MemZero_Add("DRBG V", &drbg->V, sizeof(drbg->V));
        wc_MemZero_Add("DRBG C", &drbg->C, sizeof(drbg->C));
#endif
#endif

        rng->status = DRBG_OK;
        ret = 0;
    }
    else if (ret == DRBG_CONT_FAILURE) {
        rng->status = DRBG_CONT_FAILED;
        ret = DRBG_CONT_FIPS_E;
    }
    else if (ret == DRBG_FAILURE) {
        rng->status = DRBG_FAILED;
        ret = RNG_FAILURE_E;
    }
    else {
        rng->status = DRBG_FAILED;
    }
#endif /* HAVE_HASHDRBG */
#endif /* CUSTOM_RAND_GENERATE_BLOCK */

    return ret;
}


WOLFSSL_ABI
WC_RNG* wc_rng_new(byte* nonce, word32 nonceSz, void* heap)
{
    WC_RNG* rng;

    rng = (WC_RNG*)XMALLOC(sizeof(WC_RNG), heap, DYNAMIC_TYPE_RNG);
    if (rng) {
        int error = _InitRng(rng, nonce, nonceSz, heap, INVALID_DEVID) != 0;
        if (error) {
            XFREE(rng, heap, DYNAMIC_TYPE_RNG);
            rng = NULL;
        }
    }

    return rng;
}


WOLFSSL_ABI
void wc_rng_free(WC_RNG* rng)
{
    if (rng) {
        void* heap = rng->heap;

        wc_FreeRng(rng);
        ForceZero(rng, sizeof(WC_RNG));
        XFREE(rng, heap, DYNAMIC_TYPE_RNG);
        (void)heap;
    }
}

WOLFSSL_ABI
int wc_InitRng(WC_RNG* rng)
{
    return _InitRng(rng, NULL, 0, NULL, INVALID_DEVID);
}


int wc_InitRng_ex(WC_RNG* rng, void* heap, int devId)
{
    return _InitRng(rng, NULL, 0, heap, devId);
}


int wc_InitRngNonce(WC_RNG* rng, byte* nonce, word32 nonceSz)
{
    return _InitRng(rng, nonce, nonceSz, NULL, INVALID_DEVID);
}


int wc_InitRngNonce_ex(WC_RNG* rng, byte* nonce, word32 nonceSz,
                       void* heap, int devId)
{
    return _InitRng(rng, nonce, nonceSz, heap, devId);
}


/* place a generated block in output */
WOLFSSL_ABI
int wc_RNG_GenerateBlock(WC_RNG* rng, byte* output, word32 sz)
{
    int ret;

    if (rng == NULL || output == NULL)
        return BAD_FUNC_ARG;

    if (sz == 0)
        return 0;

#ifdef WOLF_CRYPTO_CB
    #ifndef WOLF_CRYPTO_CB_FIND
    if (rng->devId != INVALID_DEVID)
    #endif
    {
        ret = wc_CryptoCb_RandomBlock(rng, output, sz);
        if (ret != CRYPTOCB_UNAVAILABLE)
            return ret;
        /* fall-through when unavailable */
    }
#endif

#ifdef HAVE_INTEL_RDRAND
    if (IS_INTEL_RDRAND(intel_flags))
        return wc_GenerateRand_IntelRD(NULL, output, sz);
#endif

#if defined(WOLFSSL_SILABS_SE_ACCEL) && defined(WOLFSSL_SILABS_TRNG)
    return silabs_GenerateRand(output, sz);
#endif

#if defined(WOLFSSL_ASYNC_CRYPT)
    if (rng->asyncDev.marker == WOLFSSL_ASYNC_MARKER_RNG) {
        /* these are blocking */
    #ifdef HAVE_CAVIUM
        return NitroxRngGenerateBlock(rng, output, sz);
    #elif defined(HAVE_INTEL_QA) && defined(QAT_ENABLE_RNG)
        return IntelQaDrbg(&rng->asyncDev, output, sz);
    #else
        /* simulator not supported */
    #endif
    }
#endif

#ifdef CUSTOM_RAND_GENERATE_BLOCK
    XMEMSET(output, 0, sz);
    ret = (int)CUSTOM_RAND_GENERATE_BLOCK(output, sz);
#else

#ifdef HAVE_HASHDRBG
    if (sz > RNG_MAX_BLOCK_LEN)
        return BAD_FUNC_ARG;

    if (rng->status != DRBG_OK)
        return RNG_FAILURE_E;

    ret = Hash_DRBG_Generate((DRBG_internal *)rng->drbg, output, sz);
    if (ret == DRBG_NEED_RESEED) {
        if (wc_RNG_HealthTestLocal(1) == 0) {
        #ifndef WOLFSSL_SMALL_STACK
            byte newSeed[SEED_SZ + SEED_BLOCK_SZ];
            ret = DRBG_SUCCESS;
        #else
            byte* newSeed = (byte*)XMALLOC(SEED_SZ + SEED_BLOCK_SZ, rng->heap,
                DYNAMIC_TYPE_SEED);
            ret = (newSeed == NULL) ? MEMORY_E : DRBG_SUCCESS;
        #endif
            if (ret == DRBG_SUCCESS) {
            #ifdef WC_RNG_SEED_CB
                if (seedCb == NULL) {
                    ret = DRBG_NO_SEED_CB;
                }
                else {
                    ret = seedCb(&rng->seed, newSeed, SEED_SZ + SEED_BLOCK_SZ);
                    if (ret != 0) {
                        ret = DRBG_FAILURE;
                    }
                }
            #else
                ret = wc_GenerateSeed(&rng->seed, newSeed,
                                  SEED_SZ + SEED_BLOCK_SZ);
            #endif
                if (ret != 0)
                    ret = DRBG_FAILURE;
            }
            if (ret == DRBG_SUCCESS)
                ret = wc_RNG_TestSeed(newSeed, SEED_SZ + SEED_BLOCK_SZ);

            if (ret == DRBG_SUCCESS)
                ret = Hash_DRBG_Reseed((DRBG_internal *)rng->drbg,
                                       newSeed + SEED_BLOCK_SZ, SEED_SZ);
            if (ret == DRBG_SUCCESS)
                ret = Hash_DRBG_Generate((DRBG_internal *)rng->drbg, output, sz);

            ForceZero(newSeed, sizeof(newSeed));
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(newSeed, rng->heap, DYNAMIC_TYPE_SEED);
        #endif
        }
        else {
            ret = DRBG_CONT_FAILURE;
        }
    }

    if (ret == DRBG_SUCCESS) {
        ret = 0;
    }
    else if (ret == DRBG_CONT_FAILURE) {
        ret = DRBG_CONT_FIPS_E;
        rng->status = DRBG_CONT_FAILED;
    }
    else {
        ret = RNG_FAILURE_E;
        rng->status = DRBG_FAILED;
    }
#else

    /* if we get here then there is an RNG configuration error */
    ret = RNG_FAILURE_E;

#endif /* HAVE_HASHDRBG */
#endif /* CUSTOM_RAND_GENERATE_BLOCK */

    return ret;
}


int wc_RNG_GenerateByte(WC_RNG* rng, byte* b)
{
    return wc_RNG_GenerateBlock(rng, b, 1);
}


int wc_FreeRng(WC_RNG* rng)
{
    int ret = 0;

    if (rng == NULL)
        return BAD_FUNC_ARG;

#if defined(WOLFSSL_ASYNC_CRYPT)
    wolfAsync_DevCtxFree(&rng->asyncDev, WOLFSSL_ASYNC_MARKER_RNG);
#endif

#ifdef HAVE_HASHDRBG
    if (rng->drbg != NULL) {
      if (Hash_DRBG_Uninstantiate((DRBG_internal *)rng->drbg) != DRBG_SUCCESS)
            ret = RNG_FAILURE_E;

    #if !defined(WOLFSSL_NO_MALLOC) || defined(WOLFSSL_STATIC_MEMORY)
        XFREE(rng->drbg, rng->heap, DYNAMIC_TYPE_RNG);
    #elif defined(WOLFSSL_CHECK_MEM_ZERO)
        wc_MemZero_Check(rng->drbg, sizeof(DRBG_internal));
    #endif
        rng->drbg = NULL;
    }

    rng->status = DRBG_NOT_INIT;
#endif /* HAVE_HASHDRBG */

#ifdef WOLFSSL_XILINX_CRYPT_VERSAL
    /* don't overwrite previously set error */
    if (wc_VersalTrngReset() && !ret)
        ret = WC_HW_E;
#endif

    return ret;
}

#ifdef HAVE_HASHDRBG
int wc_RNG_HealthTest(int reseed, const byte* seedA, word32 seedASz,
                                  const byte* seedB, word32 seedBSz,
                                  byte* output, word32 outputSz)
{
    return wc_RNG_HealthTest_ex(reseed, NULL, 0,
                                seedA, seedASz, seedB, seedBSz,
                                output, outputSz,
                                NULL, INVALID_DEVID);
}


int wc_RNG_HealthTest_ex(int reseed, const byte* nonce, word32 nonceSz,
                                  const byte* seedA, word32 seedASz,
                                  const byte* seedB, word32 seedBSz,
                                  byte* output, word32 outputSz,
                                  void* heap, int devId)
{
    int ret = -1;
    DRBG_internal* drbg;
#ifndef WOLFSSL_SMALL_STACK
    DRBG_internal  drbg_var;
#endif

    if (seedA == NULL || output == NULL) {
        return BAD_FUNC_ARG;
    }

    if (reseed != 0 && seedB == NULL) {
        return BAD_FUNC_ARG;
    }

    if (outputSz != RNG_HEALTH_TEST_CHECK_SIZE) {
        return ret;
    }

#ifdef WOLFSSL_SMALL_STACK
    drbg = (DRBG_internal*)XMALLOC(sizeof(DRBG_internal), heap,
        DYNAMIC_TYPE_RNG);
    if (drbg == NULL) {
        return MEMORY_E;
    }
#else
    drbg = &drbg_var;
#endif

    if (Hash_DRBG_Instantiate(drbg, seedA, seedASz, nonce, nonceSz,
                              heap, devId) != 0) {
        goto exit_rng_ht;
    }

    if (reseed) {
        if (Hash_DRBG_Reseed(drbg, seedB, seedBSz) != 0) {
            goto exit_rng_ht;
        }
    }

    /* This call to generate is prescribed by the NIST DRBGVS
     * procedure. The results are thrown away. The known
     * answer test checks the second block of DRBG out of
     * the generator to ensure the internal state is updated
     * as expected. */
    if (Hash_DRBG_Generate(drbg, output, outputSz) != 0) {
        goto exit_rng_ht;
    }

    if (Hash_DRBG_Generate(drbg, output, outputSz) != 0) {
        goto exit_rng_ht;
    }

    /* Mark success */
    ret = 0;

exit_rng_ht:

    /* This is safe to call even if Hash_DRBG_Instantiate fails */
    if (Hash_DRBG_Uninstantiate(drbg) != 0) {
        ret = -1;
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(drbg, heap, DYNAMIC_TYPE_RNG);
#endif

    return ret;
}


const FLASH_QUALIFIER byte seedA_data[] = {
    0x63, 0x36, 0x33, 0x77, 0xe4, 0x1e, 0x86, 0x46, 0x8d, 0xeb, 0x0a, 0xb4,
    0xa8, 0xed, 0x68, 0x3f, 0x6a, 0x13, 0x4e, 0x47, 0xe0, 0x14, 0xc7, 0x00,
    0x45, 0x4e, 0x81, 0xe9, 0x53, 0x58, 0xa5, 0x69, 0x80, 0x8a, 0xa3, 0x8f,
    0x2a, 0x72, 0xa6, 0x23, 0x59, 0x91, 0x5a, 0x9f, 0x8a, 0x04, 0xca, 0x68
};

const FLASH_QUALIFIER byte reseedSeedA_data[] = {
    0xe6, 0x2b, 0x8a, 0x8e, 0xe8, 0xf1, 0x41, 0xb6, 0x98, 0x05, 0x66, 0xe3,
    0xbf, 0xe3, 0xc0, 0x49, 0x03, 0xda, 0xd4, 0xac, 0x2c, 0xdf, 0x9f, 0x22,
    0x80, 0x01, 0x0a, 0x67, 0x39, 0xbc, 0x83, 0xd3
};

const FLASH_QUALIFIER byte outputA_data[] = {
    0x04, 0xee, 0xc6, 0x3b, 0xb2, 0x31, 0xdf, 0x2c, 0x63, 0x0a, 0x1a, 0xfb,
    0xe7, 0x24, 0x94, 0x9d, 0x00, 0x5a, 0x58, 0x78, 0x51, 0xe1, 0xaa, 0x79,
    0x5e, 0x47, 0x73, 0x47, 0xc8, 0xb0, 0x56, 0x62, 0x1c, 0x18, 0xbd, 0xdc,
    0xdd, 0x8d, 0x99, 0xfc, 0x5f, 0xc2, 0xb9, 0x20, 0x53, 0xd8, 0xcf, 0xac,
    0xfb, 0x0b, 0xb8, 0x83, 0x12, 0x05, 0xfa, 0xd1, 0xdd, 0xd6, 0xc0, 0x71,
    0x31, 0x8a, 0x60, 0x18, 0xf0, 0x3b, 0x73, 0xf5, 0xed, 0xe4, 0xd4, 0xd0,
    0x71, 0xf9, 0xde, 0x03, 0xfd, 0x7a, 0xea, 0x10, 0x5d, 0x92, 0x99, 0xb8,
    0xaf, 0x99, 0xaa, 0x07, 0x5b, 0xdb, 0x4d, 0xb9, 0xaa, 0x28, 0xc1, 0x8d,
    0x17, 0x4b, 0x56, 0xee, 0x2a, 0x01, 0x4d, 0x09, 0x88, 0x96, 0xff, 0x22,
    0x82, 0xc9, 0x55, 0xa8, 0x19, 0x69, 0xe0, 0x69, 0xfa, 0x8c, 0xe0, 0x07,
    0xa1, 0x80, 0x18, 0x3a, 0x07, 0xdf, 0xae, 0x17
};

const FLASH_QUALIFIER byte seedB_data[] = {
    0xa6, 0x5a, 0xd0, 0xf3, 0x45, 0xdb, 0x4e, 0x0e, 0xff, 0xe8, 0x75, 0xc3,
    0xa2, 0xe7, 0x1f, 0x42, 0xc7, 0x12, 0x9d, 0x62, 0x0f, 0xf5, 0xc1, 0x19,
    0xa9, 0xef, 0x55, 0xf0, 0x51, 0x85, 0xe0, 0xfb, /* nonce next */
    0x85, 0x81, 0xf9, 0x31, 0x75, 0x17, 0x27, 0x6e, 0x06, 0xe9, 0x60, 0x7d,
    0xdb, 0xcb, 0xcc, 0x2e
};

const FLASH_QUALIFIER byte outputB_data[] = {
    0xd3, 0xe1, 0x60, 0xc3, 0x5b, 0x99, 0xf3, 0x40, 0xb2, 0x62, 0x82, 0x64,
    0xd1, 0x75, 0x10, 0x60, 0xe0, 0x04, 0x5d, 0xa3, 0x83, 0xff, 0x57, 0xa5,
    0x7d, 0x73, 0xa6, 0x73, 0xd2, 0xb8, 0xd8, 0x0d, 0xaa, 0xf6, 0xa6, 0xc3,
    0x5a, 0x91, 0xbb, 0x45, 0x79, 0xd7, 0x3f, 0xd0, 0xc8, 0xfe, 0xd1, 0x11,
    0xb0, 0x39, 0x13, 0x06, 0x82, 0x8a, 0xdf, 0xed, 0x52, 0x8f, 0x01, 0x81,
    0x21, 0xb3, 0xfe, 0xbd, 0xc3, 0x43, 0xe7, 0x97, 0xb8, 0x7d, 0xbb, 0x63,
    0xdb, 0x13, 0x33, 0xde, 0xd9, 0xd1, 0xec, 0xe1, 0x77, 0xcf, 0xa6, 0xb7,
    0x1f, 0xe8, 0xab, 0x1d, 0xa4, 0x66, 0x24, 0xed, 0x64, 0x15, 0xe5, 0x1c,
    0xcd, 0xe2, 0xc7, 0xca, 0x86, 0xe2, 0x83, 0x99, 0x0e, 0xea, 0xeb, 0x91,
    0x12, 0x04, 0x15, 0x52, 0x8b, 0x22, 0x95, 0x91, 0x02, 0x81, 0xb0, 0x2d,
    0xd4, 0x31, 0xf4, 0xc9, 0xf7, 0x04, 0x27, 0xdf
};


static int wc_RNG_HealthTestLocal(int reseed)
{
    int ret = 0;
#ifdef WOLFSSL_SMALL_STACK
    byte* check;
#else
    byte  check[RNG_HEALTH_TEST_CHECK_SIZE];
#endif

#ifdef WOLFSSL_SMALL_STACK
    check = (byte*)XMALLOC(RNG_HEALTH_TEST_CHECK_SIZE, NULL,
                           DYNAMIC_TYPE_TMP_BUFFER);
    if (check == NULL) {
        return MEMORY_E;
    }
#endif

    if (reseed) {
#ifdef WOLFSSL_USE_FLASHMEM
        byte* seedA = (byte*)XMALLOC(sizeof(seedA_data), NULL,
                             DYNAMIC_TYPE_TMP_BUFFER);
        byte* reseedSeedA = (byte*)XMALLOC(sizeof(reseedSeedA_data), NULL,
                             DYNAMIC_TYPE_TMP_BUFFER);
        byte* outputA = (byte*)XMALLOC(sizeof(outputA_data), NULL,
                             DYNAMIC_TYPE_TMP_BUFFER);

        if (!seedA || !reseedSeedA || !outputA) {
            XFREE(seedA, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            XFREE(reseedSeedA, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            XFREE(outputA, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            ret = MEMORY_E;
        }
        else {
            XMEMCPY_P(seedA, seedA_data, sizeof(seedA_data));
            XMEMCPY_P(reseedSeedA, reseedSeedA_data, sizeof(reseedSeedA_data));
            XMEMCPY_P(outputA, outputA_data, sizeof(outputA_data));
#else
        const byte* seedA = seedA_data;
        const byte* reseedSeedA = reseedSeedA_data;
        const byte* outputA = outputA_data;
#endif
        ret = wc_RNG_HealthTest(1, seedA, sizeof(seedA_data),
                                reseedSeedA, sizeof(reseedSeedA_data),
                                check, RNG_HEALTH_TEST_CHECK_SIZE);
        if (ret == 0) {
            if (ConstantCompare(check, outputA,
                                RNG_HEALTH_TEST_CHECK_SIZE) != 0)
                ret = -1;
        }

#ifdef WOLFSSL_USE_FLASHMEM
            XFREE(seedA, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            XFREE(reseedSeedA, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            XFREE(outputA, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        }
#endif
    }
    else {
#ifdef WOLFSSL_USE_FLASHMEM
        byte* seedB = (byte*)XMALLOC(sizeof(seedB_data), NULL,
                             DYNAMIC_TYPE_TMP_BUFFER);
        byte* outputB = (byte*)XMALLOC(sizeof(outputB_data), NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);

        if (!seedB || !outputB) {
            XFREE(seedB, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            XFREE(outputB, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            ret = MEMORY_E;
        }
        else {
            XMEMCPY_P(seedB, seedB_data, sizeof(seedB_data));
            XMEMCPY_P(outputB, outputB_data, sizeof(outputB_data));
#else
        const byte* seedB = seedB_data;
        const byte* outputB = outputB_data;
#endif
        ret = wc_RNG_HealthTest(0, seedB, sizeof(seedB_data),
                                NULL, 0,
                                check, RNG_HEALTH_TEST_CHECK_SIZE);
        if (ret == 0) {
            if (ConstantCompare(check, outputB,
                                RNG_HEALTH_TEST_CHECK_SIZE) != 0)
                ret = -1;
        }

        /* The previous test cases use a large seed instead of a seed and nonce.
         * seedB is actually from a test case with a seed and nonce, and
         * just concatenates them. The pivot point between seed and nonce is
         * byte 32, feed them into the health test separately. */
        if (ret == 0) {
            ret = wc_RNG_HealthTest_ex(0,
                                    seedB + 32, sizeof(seedB_data) - 32,
                                    seedB, 32,
                                    NULL, 0,
                                    check, RNG_HEALTH_TEST_CHECK_SIZE,
                                    NULL, INVALID_DEVID);
            if (ret == 0) {
                if (ConstantCompare(check, outputB, sizeof(outputB_data)) != 0)
                    ret = -1;
            }
        }

#ifdef WOLFSSL_USE_FLASHMEM
            XFREE(seedB, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            XFREE(outputB, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        }
#endif
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(check, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}

#endif /* HAVE_HASHDRBG */


#ifdef HAVE_WNR

/*
 * Init global Whitewood netRandom context
 * Returns 0 on success, negative on error
 */
int wc_InitNetRandom(const char* configFile, wnr_hmac_key hmac_cb, int timeout)
{
    if (configFile == NULL || timeout < 0)
        return BAD_FUNC_ARG;

    if (wnr_mutex_init > 0) {
        WOLFSSL_MSG("netRandom context already created, skipping");
        return 0;
    }

    if (wc_InitMutex(&wnr_mutex) != 0) {
        WOLFSSL_MSG("Bad Init Mutex wnr_mutex");
        return BAD_MUTEX_E;
    }
    wnr_mutex_init = 1;

    if (wc_LockMutex(&wnr_mutex) != 0) {
        WOLFSSL_MSG("Bad Lock Mutex wnr_mutex");
        return BAD_MUTEX_E;
    }

    /* store entropy timeout */
    wnr_timeout = timeout;

    /* create global wnr_context struct */
    if (wnr_create(&wnr_ctx) != WNR_ERROR_NONE) {
        WOLFSSL_MSG("Error creating global netRandom context");
        return RNG_FAILURE_E;
    }

    /* load config file */
    if (wnr_config_loadf(wnr_ctx, (char*)configFile) != WNR_ERROR_NONE) {
        WOLFSSL_MSG("Error loading config file into netRandom context");
        wnr_destroy(wnr_ctx);
        wnr_ctx = NULL;
        return RNG_FAILURE_E;
    }

    /* create/init polling mechanism */
    if (wnr_poll_create() != WNR_ERROR_NONE) {
        WOLFSSL_MSG("Error initializing netRandom polling mechanism");
        wnr_destroy(wnr_ctx);
        wnr_ctx = NULL;
        return RNG_FAILURE_E;
    }

    /* validate config, set HMAC callback (optional) */
    if (wnr_setup(wnr_ctx, hmac_cb) != WNR_ERROR_NONE) {
        WOLFSSL_MSG("Error setting up netRandom context");
        wnr_destroy(wnr_ctx);
        wnr_ctx = NULL;
        wnr_poll_destroy();
        return RNG_FAILURE_E;
    }

    wc_UnLockMutex(&wnr_mutex);

    return 0;
}

/*
 * Free global Whitewood netRandom context
 * Returns 0 on success, negative on error
 */
int wc_FreeNetRandom(void)
{
    if (wnr_mutex_init > 0) {

        if (wc_LockMutex(&wnr_mutex) != 0) {
            WOLFSSL_MSG("Bad Lock Mutex wnr_mutex");
            return BAD_MUTEX_E;
        }

        if (wnr_ctx != NULL) {
            wnr_destroy(wnr_ctx);
            wnr_ctx = NULL;
        }
        wnr_poll_destroy();

        wc_UnLockMutex(&wnr_mutex);

        wc_FreeMutex(&wnr_mutex);
        wnr_mutex_init = 0;
    }

    return 0;
}

#endif /* HAVE_WNR */


#if defined(HAVE_INTEL_RDRAND) || defined(HAVE_INTEL_RDSEED) || \
    defined(HAVE_AMD_RDSEED)

#ifdef WOLFSSL_ASYNC_CRYPT
    /* need more retries if multiple cores */
    #define INTELRD_RETRY (32 * 8)
#else
    #define INTELRD_RETRY 32
#endif

#if defined(HAVE_INTEL_RDSEED) || defined(HAVE_AMD_RDSEED)

#ifndef USE_INTEL_INTRINSICS

    /* return 0 on success */
    static WC_INLINE int IntelRDseed64(word64* seed)
    {
        unsigned char ok;

        __asm__ volatile("rdseed %0; setc %1":"=r"(*seed), "=qm"(ok));
        return (ok) ? 0 : -1;
    }

#else /* USE_INTEL_INTRINSICS */
    /* The compiler Visual Studio uses does not allow inline assembly.
     * It does allow for Intel intrinsic functions. */

    /* return 0 on success */
# ifdef __GNUC__
    __attribute__((target("rdseed")))
# endif
    static WC_INLINE int IntelRDseed64(word64* seed)
    {
        int ok;

        ok = _rdseed64_step((unsigned long long*) seed);
        return (ok) ? 0 : -1;
    }

#endif /* USE_INTEL_INTRINSICS */

/* return 0 on success */
static WC_INLINE int IntelRDseed64_r(word64* rnd)
{
    int i;
    for (i = 0; i < INTELRD_RETRY; i++) {
        if (IntelRDseed64(rnd) == 0)
            return 0;
    }
    return -1;
}

#ifndef WOLFSSL_LINUXKM
/* return 0 on success */
static int wc_GenerateSeed_IntelRD(OS_Seed* os, byte* output, word32 sz)
{
    int ret;
    word64 rndTmp;

    (void)os;

    if (!IS_INTEL_RDSEED(intel_flags))
        return -1;

    for (; (sz / sizeof(word64)) > 0; sz -= sizeof(word64),
                                                    output += sizeof(word64)) {
        ret = IntelRDseed64_r((word64*)output);
        if (ret != 0)
            return ret;
    }
    if (sz == 0)
        return 0;

    /* handle unaligned remainder */
    ret = IntelRDseed64_r(&rndTmp);
    if (ret != 0)
        return ret;

    XMEMCPY(output, &rndTmp, sz);
    ForceZero(&rndTmp, sizeof(rndTmp));

    return 0;
}
#endif

#endif /* HAVE_INTEL_RDSEED || HAVE_AMD_RDSEED */

#ifdef HAVE_INTEL_RDRAND

#ifndef USE_INTEL_INTRINSICS

/* return 0 on success */
static WC_INLINE int IntelRDrand64(word64 *rnd)
{
    unsigned char ok;

    __asm__ volatile("rdrand %0; setc %1":"=r"(*rnd), "=qm"(ok));

    return (ok) ? 0 : -1;
}

#else /* USE_INTEL_INTRINSICS */
    /* The compiler Visual Studio uses does not allow inline assembly.
     * It does allow for Intel intrinsic functions. */

/* return 0 on success */
# ifdef __GNUC__
__attribute__((target("rdrnd")))
# endif
static WC_INLINE int IntelRDrand64(word64 *rnd)
{
    int ok;

    ok = _rdrand64_step((unsigned long long*) rnd);

    return (ok) ? 0 : -1;
}

#endif /* USE_INTEL_INTRINSICS */

/* return 0 on success */
static WC_INLINE int IntelRDrand64_r(word64 *rnd)
{
    int i;
    for (i = 0; i < INTELRD_RETRY; i++) {
        if (IntelRDrand64(rnd) == 0)
            return 0;
    }
    return -1;
}

/* return 0 on success */
static int wc_GenerateRand_IntelRD(OS_Seed* os, byte* output, word32 sz)
{
    int ret;
    word64 rndTmp;

    (void)os;

    if (!IS_INTEL_RDRAND(intel_flags))
        return -1;

    for (; (sz / sizeof(word64)) > 0; sz -= sizeof(word64),
                                                    output += sizeof(word64)) {
        ret = IntelRDrand64_r((word64 *)output);
        if (ret != 0)
            return ret;
    }
    if (sz == 0)
        return 0;

    /* handle unaligned remainder */
    ret = IntelRDrand64_r(&rndTmp);
    if (ret != 0)
        return ret;

    XMEMCPY(output, &rndTmp, sz);

    return 0;
}

#endif /* HAVE_INTEL_RDRAND */
#endif /* HAVE_INTEL_RDRAND || HAVE_INTEL_RDSEED || HAVE_AMD_RDSEED */


/* Begin wc_GenerateSeed Implementations */
#if defined(CUSTOM_RAND_GENERATE_SEED)

    /* Implement your own random generation function
     * Return 0 to indicate success
     * int rand_gen_seed(byte* output, word32 sz);
     * #define CUSTOM_RAND_GENERATE_SEED  rand_gen_seed */

    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        (void)os; /* Suppress unused arg warning */
        return CUSTOM_RAND_GENERATE_SEED(output, sz);
    }

#elif defined(CUSTOM_RAND_GENERATE_SEED_OS)

    /* Implement your own random generation function,
     *  which includes OS_Seed.
     * Return 0 to indicate success
     * int rand_gen_seed(OS_Seed* os, byte* output, word32 sz);
     * #define CUSTOM_RAND_GENERATE_SEED_OS  rand_gen_seed */

    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        return CUSTOM_RAND_GENERATE_SEED_OS(os, output, sz);
    }

#elif defined(CUSTOM_RAND_GENERATE)

   /* Implement your own random generation function
    * word32 rand_gen(void);
    * #define CUSTOM_RAND_GENERATE  rand_gen  */

    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        word32 i = 0;

        (void)os;

        while (i < sz)
        {
            /* If not aligned or there is odd/remainder */
            if( (i + sizeof(CUSTOM_RAND_TYPE)) > sz ||
                ((wc_ptr_t)&output[i] % sizeof(CUSTOM_RAND_TYPE)) != 0
            ) {
                /* Single byte at a time */
                output[i++] = (byte)CUSTOM_RAND_GENERATE();
            }
            else {
                /* Use native 8, 16, 32 or 64 copy instruction */
                *((CUSTOM_RAND_TYPE*)&output[i]) = CUSTOM_RAND_GENERATE();
                i += sizeof(CUSTOM_RAND_TYPE);
            }
        }

        return 0;
    }

#elif defined(WOLFSSL_SGX)

int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
{
    int ret = !SGX_SUCCESS;
    int i, read_max = 10;

    for (i = 0; i < read_max && ret != SGX_SUCCESS; i++) {
        ret = sgx_read_rand(output, sz);
    }

    (void)os;
    return (ret == SGX_SUCCESS) ? 0 : 1;
}

#elif defined(USE_WINDOWS_API)

int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
{
#ifdef WOLF_CRYPTO_CB
    int ret;

    if (os != NULL
    #ifndef WOLF_CRYPTO_CB_FIND
        && os->devId != INVALID_DEVID)
    #endif
    {
        ret = wc_CryptoCb_RandomSeed(os, output, sz);
        if (ret != CRYPTOCB_UNAVAILABLE)
            return ret;
        /* fall-through when unavailable */
    }
#endif

    #ifdef HAVE_INTEL_RDSEED
        if (IS_INTEL_RDSEED(intel_flags)) {
             if (!wc_GenerateSeed_IntelRD(NULL, output, sz)) {
                 /* success, we're done */
                 return 0;
             }
        #ifdef FORCE_FAILURE_RDSEED
             /* don't fall back to CryptoAPI */
             return READ_RAN_E;
        #endif
        }
    #endif /* HAVE_INTEL_RDSEED */

    if(!CryptAcquireContext(&os->handle, 0, 0, PROV_RSA_FULL,
                            CRYPT_VERIFYCONTEXT))
        return WINCRYPT_E;

    if (!CryptGenRandom(os->handle, sz, output))
        return CRYPTGEN_E;

    CryptReleaseContext(os->handle, 0);

    return 0;
}


#elif defined(HAVE_RTP_SYS) || defined(EBSNET)

#include "rtprand.h"   /* rtp_rand () */

#if (defined(HAVE_RTP_SYS) || (defined(RTPLATFORM) && (RTPLATFORM != 0)))
#include "rtptime.h"   /* rtp_get_system_msec() */

int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
{
    word32 i;

    rtp_srand(rtp_get_system_msec());
    for (i = 0; i < sz; i++ ) {
        output[i] = rtp_rand() % 256;
    }

    return 0;
}
#else
int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
{
    word32 i;
    KS_SEED(ks_get_ticks());

    for (i = 0; i < sz; i++ ) {
        output[i] = KS_RANDOM() % 256;
    }

    return 0;
}
#endif /* defined(HAVE_RTP_SYS) || (defined(RTPLATFORM) && (RTPLATFORM != 0)) */

#elif (defined(WOLFSSL_ATMEL) || defined(WOLFSSL_ATECC_RNG)) && \
      !defined(WOLFSSL_PIC32MZ_RNG)
    /* enable ATECC RNG unless using PIC32MZ one instead */
    #include <wolfssl/wolfcrypt/port/atmel/atmel.h>

    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        int ret = 0;

        (void)os;
        if (output == NULL) {
            return BUFFER_E;
        }

        ret = atmel_get_random_number(sz, output);

        return ret;
    }

#elif defined(MICROCHIP_PIC32)

    #ifdef MICROCHIP_MPLAB_HARMONY
        #ifdef MICROCHIP_MPLAB_HARMONY_3
            #include "system/time/sys_time.h"
            #define PIC32_SEED_COUNT SYS_TIME_CounterGet
        #else
            #define PIC32_SEED_COUNT _CP0_GET_COUNT
        #endif
    #else
        #if !defined(WOLFSSL_MICROCHIP_PIC32MZ)
            #include <peripheral/timer.h>
        #endif
        extern word32 ReadCoreTimer(void);
        #define PIC32_SEED_COUNT ReadCoreTimer
    #endif

    #ifdef WOLFSSL_PIC32MZ_RNG
        #include "xc.h"
        int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
        {
            int i;
            byte rnd[8];
            word32 *rnd32 = (word32 *)rnd;
            word32 size = sz;
            byte* op = output;

#if ((__PIC32_FEATURE_SET0 == 'E') && (__PIC32_FEATURE_SET1 == 'C'))
            RNGNUMGEN1 = _CP0_GET_COUNT();
            RNGPOLY1 = _CP0_GET_COUNT();
            RNGPOLY2 = _CP0_GET_COUNT();
            RNGNUMGEN2 = _CP0_GET_COUNT();
#else
            /* All others can be seeded from the TRNG */
            RNGCONbits.TRNGMODE = 1;
            RNGCONbits.TRNGEN = 1;
            while (RNGCNT < 64);
            RNGCONbits.LOAD = 1;
            while (RNGCONbits.LOAD == 1);
            while (RNGCNT < 64);
            RNGPOLY2 = RNGSEED2;
            RNGPOLY1 = RNGSEED1;
#endif

            RNGCONbits.PLEN = 0x40;
            RNGCONbits.PRNGEN = 1;
            for (i=0; i<5; i++) { /* wait for RNGNUMGEN ready */
                volatile int x, y;
                x = RNGNUMGEN1;
                y = RNGNUMGEN2;
                (void)x;
                (void)y;
            }
            do {
                rnd32[0] = RNGNUMGEN1;
                rnd32[1] = RNGNUMGEN2;

                for(i=0; i<8; i++, op++) {
                    *op = rnd[i];
                    size --;
                    if(size==0)break;
                }
            } while(size);
            return 0;
        }
    #else  /* WOLFSSL_PIC32MZ_RNG */
        /* uses the core timer, in nanoseconds to seed srand */
        int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
        {
            int i;
            srand(PIC32_SEED_COUNT() * 25);

            for (i = 0; i < sz; i++ ) {
                output[i] = rand() % 256;
                if ( (i % 8) == 7)
                    srand(PIC32_SEED_COUNT() * 25);
            }
            return 0;
        }
    #endif /* WOLFSSL_PIC32MZ_RNG */

#elif defined(FREESCALE_K70_RNGA) || defined(FREESCALE_RNGA)
    /*
     * wc_Generates a RNG seed using the Random Number Generator Accelerator
     * on the Kinetis K70. Documentation located in Chapter 37 of
     * K70 Sub-Family Reference Manual (see Note 3 in the README for link).
     */
    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        word32 i;

        /* turn on RNGA module */
        #if defined(SIM_SCGC3_RNGA_MASK)
            SIM_SCGC3 |= SIM_SCGC3_RNGA_MASK;
        #endif
        #if defined(SIM_SCGC6_RNGA_MASK)
            /* additionally needed for at least K64F */
            SIM_SCGC6 |= SIM_SCGC6_RNGA_MASK;
        #endif

        /* set SLP bit to 0 - "RNGA is not in sleep mode" */
        RNG_CR &= ~RNG_CR_SLP_MASK;

        /* set HA bit to 1 - "security violations masked" */
        RNG_CR |= RNG_CR_HA_MASK;

        /* set GO bit to 1 - "output register loaded with data" */
        RNG_CR |= RNG_CR_GO_MASK;

        for (i = 0; i < sz; i++) {

            /* wait for RNG FIFO to be full */
            while((RNG_SR & RNG_SR_OREG_LVL(0xF)) == 0) {}

            /* get value */
            output[i] = RNG_OR;
        }

        return 0;
    }

#elif defined(FREESCALE_K53_RNGB) || defined(FREESCALE_RNGB)
    /*
     * wc_Generates a RNG seed using the Random Number Generator (RNGB)
     * on the Kinetis K53. Documentation located in Chapter 33 of
     * K53 Sub-Family Reference Manual (see note in the README for link).
     */
    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        int i;

        /* turn on RNGB module */
        SIM_SCGC3 |= SIM_SCGC3_RNGB_MASK;

        /* reset RNGB */
        RNG_CMD |= RNG_CMD_SR_MASK;

        /* FIFO generate interrupt, return all zeros on underflow,
         * set auto reseed */
        RNG_CR |= (RNG_CR_FUFMOD_MASK | RNG_CR_AR_MASK);

        /* gen seed, clear interrupts, clear errors */
        RNG_CMD |= (RNG_CMD_GS_MASK | RNG_CMD_CI_MASK | RNG_CMD_CE_MASK);

        /* wait for seeding to complete */
        while ((RNG_SR & RNG_SR_SDN_MASK) == 0) {}

        for (i = 0; i < sz; i++) {

            /* wait for a word to be available from FIFO */
            while((RNG_SR & RNG_SR_FIFO_LVL_MASK) == 0) {}

            /* get value */
            output[i] = RNG_OUT;
        }

        return 0;
    }

#elif defined(FREESCALE_KSDK_2_0_TRNG)
    #ifndef TRNG0
    #define TRNG0 TRNG
    #endif

    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        status_t status;
        status = TRNG_GetRandomData(TRNG0, output, sz);
        (void)os;
        if (status == kStatus_Success)
        {
            return(0);
        }
        return RAN_BLOCK_E;
    }

#elif defined(FREESCALE_KSDK_2_0_RNGA)

    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        status_t status;
        status = RNGA_GetRandomData(RNG, output, sz);
        (void)os;
        if (status == kStatus_Success)
        {
            return(0);
        }
        return RAN_BLOCK_E;
    }


#elif defined(FREESCALE_RNGA)

    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        status_t status;
        status = RNGA_GetRandomData(RNG, output, sz);
        (void)os;
        if (status == kStatus_Success)
        {
            return(0);
        }
        return RAN_BLOCK_E;
    }

#elif !defined(WOLFSSL_CAAM) && \
    (defined(FREESCALE_MQX) || defined(FREESCALE_KSDK_MQX) || \
     defined(FREESCALE_KSDK_BM) || defined(FREESCALE_FREE_RTOS))
    /*
     * Fallback to USE_TEST_GENSEED if a FREESCALE platform did not match any
     * of the TRNG/RNGA/RNGB support
     */
    #define USE_TEST_GENSEED

#elif defined(WOLFSSL_SILABS_SE_ACCEL)
    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        (void)os;
        return silabs_GenerateRand(output, sz);
    }

#elif defined(STM32_RNG)
     /* Generate a RNG seed using the hardware random number generator
      * on the STM32F2/F4/F7/L4. */

    #ifdef WOLFSSL_STM32_CUBEMX
    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        int ret;
        RNG_HandleTypeDef hrng;
        word32 i = 0;
        (void)os;

        ret = wolfSSL_CryptHwMutexLock();
        if (ret != 0) {
            return ret;
        }

        /* enable RNG clock source */
        __HAL_RCC_RNG_CLK_ENABLE();

        /* enable RNG peripheral */
        XMEMSET(&hrng, 0, sizeof(hrng));
        hrng.Instance = RNG;
        HAL_RNG_Init(&hrng);

        while (i < sz) {
            /* If not aligned or there is odd/remainder */
            if( (i + sizeof(word32)) > sz ||
                ((wc_ptr_t)&output[i] % sizeof(word32)) != 0
            ) {
                /* Single byte at a time */
                uint32_t tmpRng = 0;
                if (HAL_RNG_GenerateRandomNumber(&hrng, &tmpRng) != HAL_OK) {
                    wolfSSL_CryptHwMutexUnLock();
                    return RAN_BLOCK_E;
                }
                output[i++] = (byte)tmpRng;
            }
            else {
                /* Use native 32 instruction */
                if (HAL_RNG_GenerateRandomNumber(&hrng, (uint32_t*)&output[i]) != HAL_OK) {
                    wolfSSL_CryptHwMutexUnLock();
                    return RAN_BLOCK_E;
                }
                i += sizeof(word32);
            }
        }

        HAL_RNG_DeInit(&hrng);

        wolfSSL_CryptHwMutexUnLock();

        return 0;
    }
    #elif defined(WOLFSSL_STM32F427_RNG) || defined(WOLFSSL_STM32_RNG_NOLIB) \
        || defined(STM32_NUTTX_RNG)

    #ifdef STM32_NUTTX_RNG
        #include "hardware/stm32_rng.h"
        /* Set CONFIG_STM32U5_RNG in NuttX to enable the RCC */
        #define WC_RNG_CR *((volatile uint32_t*)(STM32_RNG_CR))
        #define WC_RNG_SR *((volatile uint32_t*)(STM32_RNG_SR))
        #define WC_RNG_DR *((volatile uint32_t*)(STM32_RNG_DR))
    #else
        /* Comes from "stm32xxxx_hal.h" */
        #define WC_RNG_CR RNG->CR
        #define WC_RNG_SR RNG->SR
        #define WC_RNG_DR RNG->DR
    #endif


    /* Generate a RNG seed using the hardware RNG on the STM32F427
     * directly, following steps outlined in STM32F4 Reference
     * Manual (Chapter 24) for STM32F4xx family. */
    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        int ret;
        word32 i;
        (void)os;

        ret = wolfSSL_CryptHwMutexLock();
        if (ret != 0) {
            return ret;
        }

    #ifndef STM32_NUTTX_RNG
        /* enable RNG peripheral clock */
        RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN;
    #endif

        /* enable RNG interrupt, set IE bit in RNG->CR register */
        WC_RNG_CR |= RNG_CR_IE;

        /* enable RNG, set RNGEN bit in RNG->CR. Activates RNG,
         * RNG_LFSR, and error detector */
        WC_RNG_CR |= RNG_CR_RNGEN;

        /* verify no errors, make sure SEIS and CEIS bits are 0
         * in RNG->SR register */
        if (WC_RNG_SR & (RNG_SR_SECS | RNG_SR_CECS)) {
            wolfSSL_CryptHwMutexUnLock();
            return RNG_FAILURE_E;
        }

        for (i = 0; i < sz; i++) {
            /* wait until RNG number is ready */
            while ((WC_RNG_SR & RNG_SR_DRDY) == 0) { }

            /* get value */
            output[i] = WC_RNG_DR;
        }

        wolfSSL_CryptHwMutexUnLock();

        return 0;
    }

    #else

    /* Generate a RNG seed using the STM32 Standard Peripheral Library */
    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        int ret;
        word32 i;
        (void)os;

        ret = wolfSSL_CryptHwMutexLock();
        if (ret != 0) {
            return ret;
        }

        /* enable RNG clock source */
        RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);

        /* reset RNG */
        RNG_DeInit();

        /* enable RNG peripheral */
        RNG_Cmd(ENABLE);

        /* verify no errors with RNG_CLK or Seed */
        if (RNG_GetFlagStatus(RNG_FLAG_SECS | RNG_FLAG_CECS) != RESET) {
            wolfSSL_CryptHwMutexUnLock();
            return RNG_FAILURE_E;
        }

        for (i = 0; i < sz; i++) {
            /* wait until RNG number is ready */
            while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET) { }

            /* get value */
            output[i] = RNG_GetRandomNumber();
        }

        wolfSSL_CryptHwMutexUnLock();

        return 0;
    }
    #endif /* WOLFSSL_STM32_CUBEMX */

#elif defined(WOLFSSL_TIRTOS)
    #warning "potential for not enough entropy, currently being used for testing"
    #include <xdc/runtime/Timestamp.h>
    #include <stdlib.h>
    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        int i;
        srand(xdc_runtime_Timestamp_get32());

        for (i = 0; i < sz; i++ ) {
            output[i] = rand() % 256;
            if ((i % 8) == 7) {
                srand(xdc_runtime_Timestamp_get32());
            }
        }

        return 0;
    }

#elif defined(WOLFSSL_PB)

    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        word32 i;
        for (i = 0; i < sz; i++)
            output[i] = UTL_Rand();

        (void)os;

        return 0;
    }

#elif defined(WOLFSSL_NUCLEUS)
#include "nucleus.h"
#include "kernel/plus_common.h"

#warning "potential for not enough entropy, currently being used for testing"
int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
{
    int i;
    srand(NU_Get_Time_Stamp());

    for (i = 0; i < sz; i++ ) {
        output[i] = rand() % 256;
        if ((i % 8) == 7) {
            srand(NU_Get_Time_Stamp());
        }
    }

    return 0;
}
#elif defined(WOLFSSL_DEOS) && !defined(CUSTOM_RAND_GENERATE)
    #include "stdlib.h"

    #warning "potential for not enough entropy, currently being used for testing Deos"
    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        int i;
        int seed = XTIME(0);
        (void)os;

        for (i = 0; i < sz; i++ ) {
            output[i] = rand_r(&seed) % 256;
            if ((i % 8) == 7) {
                seed = XTIME(0);
                rand_r(&seed);
            }
        }

        return 0;
    }
#elif defined(WOLFSSL_VXWORKS)
    #ifdef WOLFSSL_VXWORKS_6_x
        #include "stdlib.h"
        #warning "potential for not enough entropy, currently being used for testing"
        int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
        {
            int i;
            unsigned int seed = (unsigned int)XTIME(0);
            (void)os;

            for (i = 0; i < sz; i++ ) {
                output[i] = rand_r(&seed) % 256;
                if ((i % 8) == 7) {
                    seed = (unsigned int)XTIME(0);
                    rand_r(&seed);
                }
            }

            return 0;
        }
    #else
        #include <randomNumGen.h>
        #include <tickLib.h>

        int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz) {
            STATUS                status   = ERROR;
            RANDOM_NUM_GEN_STATUS r_status = RANDOM_NUM_GEN_ERROR;
            _Vx_ticks_t           seed = 0;

            #ifdef VXWORKS_SIM
                /* cannot generate true entropy with VxWorks simulator */
                #warning "not enough entropy, simulator for testing only"
                int i = 0;

                for (i = 0; i < 1000; i++) {
                    randomAddTimeStamp();
                }
            #endif

            /*
              wolfSSL can request 52 Bytes of random bytes. We need to add
              buffer to the entropy pool to ensure we can get more than 32 Bytes.
              Because VxWorks has entropy limits (ENTROPY_MIN and ENTROPY_MAX)
              defined as 256 and 1024 bits, see randomSWNumGenLib.c.

              randStatus() can return the following status:
              RANDOM_NUM_GEN_NO_ENTROPY when entropy is 0
              RANDOM_NUM_GEN_ERROR, entropy is not initialized
              RANDOM_NUM_GEN_NOT_ENOUGH_ENTROPY if entropy < 32 Bytes
              RANDOM_NUM_GEN_ENOUGH_ENTROPY if entropy is between 32 and 128 Bytes
              RANDOM_NUM_GEN_MAX_ENTROPY if entropy is greater than 128 Bytes
            */

            do {
                seed = tickGet();
                status = randAdd(&seed, sizeof(_Vx_ticks_t), 2);
                if (status == OK)
                    r_status = randStatus();

            } while (r_status != RANDOM_NUM_GEN_MAX_ENTROPY &&
                     r_status != RANDOM_NUM_GEN_ERROR && status == OK);

            if (r_status == RANDOM_NUM_GEN_ERROR)
                return RNG_FAILURE_E;

            status = randBytes (output, sz);

            if (status == ERROR) {
                return RNG_FAILURE_E;
            }

            return 0;
        }
    #endif
#elif defined(WOLFSSL_NRF51) || defined(WOLFSSL_NRF5x)
    #include "app_error.h"
    #include "nrf_drv_rng.h"
    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        int remaining = sz, pos = 0;
        word32 err_code;
        byte available;
        static byte initialized = 0;

        (void)os;

        /* Make sure RNG is running */
        if (!initialized) {
            err_code = nrf_drv_rng_init(NULL);
            if (err_code != NRF_SUCCESS && err_code != NRF_ERROR_INVALID_STATE
            #ifdef NRF_ERROR_MODULE_ALREADY_INITIALIZED
                && err_code != NRF_ERROR_MODULE_ALREADY_INITIALIZED
            #endif
            ) {
                return -1;
            }
            initialized = 1;
        }

        while (remaining > 0) {
            int length;
            available = 0;
            nrf_drv_rng_bytes_available(&available); /* void func */
            length = (remaining < available) ? remaining : available;
            if (length > 0) {
                err_code = nrf_drv_rng_rand(&output[pos], length);
                if (err_code != NRF_SUCCESS) {
                    break;
                }
                remaining -= length;
                pos += length;
            }
        }

        return (err_code == NRF_SUCCESS) ? 0 : -1;
    }

#elif defined(HAVE_WNR)

    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        if (os == NULL || output == NULL || wnr_ctx == NULL ||
                wnr_timeout < 0) {
            return BAD_FUNC_ARG;
        }

        if (wnr_mutex_init == 0) {
            WOLFSSL_MSG("netRandom context must be created before use");
            return RNG_FAILURE_E;
        }

        if (wc_LockMutex(&wnr_mutex) != 0) {
            WOLFSSL_MSG("Bad Lock Mutex wnr_mutex");
            return BAD_MUTEX_E;
        }

        if (wnr_get_entropy(wnr_ctx, wnr_timeout, output, sz, sz) !=
                WNR_ERROR_NONE)
            return RNG_FAILURE_E;

        wc_UnLockMutex(&wnr_mutex);

        return 0;
    }

#elif defined(INTIME_RTOS)
    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        uint32_t randval;
        word32 len;

        if (output == NULL) {
            return BUFFER_E;
        }

    #ifdef INTIMEVER
        /* If INTIMEVER exists then it is INTIME RTOS v6 or later */
        #define INTIME_RAND_FUNC arc4random
        len = 4;
    #else
        /* v5 and older */
        #define INTIME_RAND_FUNC rand
        srand(time(0));
        len = 2; /* don't use all 31 returned bits */
    #endif

        while (sz > 0) {
            if (sz < len)
                len = sz;
            randval = INTIME_RAND_FUNC();
            XMEMCPY(output, &randval, len);
            output += len;
            sz -= len;
        }
        (void)os;

        return 0;
    }

#elif defined(WOLFSSL_WICED)
    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        int ret;
        (void)os;

        if (output == NULL || UINT16_MAX < sz) {
            return BUFFER_E;
        }

        if ((ret = wiced_crypto_get_random((void*) output, sz) )
                         != WICED_SUCCESS) {
            return ret;
        }

        return ret;
    }

#elif defined(WOLFSSL_NETBURNER)
    #warning using NetBurner pseudo random GetRandomByte for seed
    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        word32 i;
        (void)os;

        if (output == NULL) {
            return BUFFER_E;
        }

        for (i = 0; i < sz; i++) {
            output[i] = GetRandomByte();

            /* check if was a valid random number */
            if (!RandomValid())
                return RNG_FAILURE_E;
        }

        return 0;
    }
#elif defined(IDIRECT_DEV_RANDOM)

    extern int getRandom( int sz, unsigned char *output );

    int GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        int num_bytes_returned = 0;

        num_bytes_returned = getRandom( (int) sz, (unsigned char *) output );

        return 0;
    }

#elif defined(WOLFSSL_CAAM)

    #include <wolfssl/wolfcrypt/port/caam/wolfcaam.h>

    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        unsigned int args[4] = {0};
        CAAM_BUFFER buf[1];
        int ret    = 0;
        int times  = 1000, i; /* 1000 is an arbitrary number chosen */
        word32 idx = 0;

        (void)os;

        if (output == NULL) {
            return BUFFER_E;
        }

        /* Check Waiting to make sure entropy is ready */
        for (i = 0; i < times; i++) {
            buf[0].BufferType = DataBuffer | LastBuffer;
            buf[0].TheAddress = (CAAM_ADDRESS)(output + idx);
            buf[0].Length     = ((sz - idx) < WC_CAAM_MAX_ENTROPY)?
                                sz - idx : WC_CAAM_MAX_ENTROPY;

            args[0] = buf[0].Length;
            ret = wc_caamAddAndWait(buf, 1, args, CAAM_ENTROPY);
            if (ret == 0) {
                idx += buf[0].Length;
                if (idx == sz)
                    break;
            }

            /* driver could be waiting for entropy */
            if (ret != RAN_BLOCK_E && ret != 0) {
                return ret;
            }
#ifndef WOLFSSL_IMXRT1170_CAAM
            usleep(100);
#endif
        }

        if (i == times && ret != 0) {
             return RNG_FAILURE_E;
        }
        else { /* Success case */
            ret = 0;
        }

        return ret;
    }

#elif defined(WOLFSSL_APACHE_MYNEWT)

    #include <stdlib.h>
    #include "os/os_time.h"
    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        int i;
        srand(os_time_get());

        for (i = 0; i < sz; i++ ) {
            output[i] = rand() % 256;
            if ((i % 8) == 7) {
                srand(os_time_get());
            }
        }

        return 0;
    }

#elif defined(WOLFSSL_ESPIDF)

    /* Espressif */
    #if defined(WOLFSSL_ESP32) || defined(WOLFSSL_ESPWROOM32SE)

        /* Espressif ESP32 */
        #include <esp_system.h>
        #if defined(CONFIG_IDF_TARGET_ESP32S3)
            #include <esp_random.h>
        #endif

        int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
        {
            word32 rand;
            while (sz > 0) {
                word32 len = sizeof(rand);
                if (sz < len)
                    len = sz;
                /* Get one random 32-bit word from hw RNG */
                rand = esp_random( );
                XMEMCPY(output, &rand, len);
                output += len;
                sz -= len;
            }

            return 0;
        }

    #elif defined(WOLFSSL_ESP8266)

        /* Espressif ESP8266 */
        #include <esp_system.h>

        int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
        {
            word32 rand;
            while (sz > 0) {
                word32 len = sizeof(rand);
                if (sz < len)
                    len = sz;
                /* Get one random 32-bit word from hw RNG */
                rand = esp_random( );
                XMEMCPY(output, &rand, len);
                output += len;
                sz -= len;
            }

            return 0;
        }
    #endif /* end WOLFSSL_ESP32 */

#elif defined(WOLFSSL_LINUXKM)
    #include <linux/random.h>
    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        (void)os;

        get_random_bytes(output, sz);

        return 0;
    }

#elif defined(WOLFSSL_RENESAS_TSIP)
#if defined(WOLFSSL_RENESA_TSIP_IAREWRX)
   #include "r_bsp/mcu/all/r_rx_compiler.h"
#endif
   #include "r_bsp/platform.h"
    #include "r_tsip_rx_if.h"

    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        int ret = 0;
        word32 buffer[4];

        while (sz > 0) {
            word32 len = sizeof(buffer);

            if (sz < len) {
                len = sz;
            }
            /* return 4 words random number*/
            ret = R_TSIP_GenerateRandomNumber((uint32_t*)buffer);
            if(ret == TSIP_SUCCESS) {
                XMEMCPY(output, &buffer, len);
                output += len;
                sz -= len;
            } else
                return ret;
        }
        return ret;
    }
#elif defined(WOLFSSL_RENESAS_SCEPROTECT) || \
          defined(WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY)
    #include "r_sce.h"

    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        int ret = 0;
        word32 buffer[4];

        while (sz > 0) {
            word32 len = sizeof(buffer);

            if (sz < len) {
                len = sz;
            }
            /* return 4 words random number*/
            ret = R_SCE_RandomNumberGenerate(buffer);
            if(ret == FSP_SUCCESS) {
                XMEMCPY(output, &buffer, len);
                output += len;
                sz -= len;
            } else
                return ret;
        }
        return ret;
    }

#elif defined(WOLFSSL_SCE) && !defined(WOLFSSL_SCE_NO_TRNG)
    #include "hal_data.h"

    #ifndef WOLFSSL_SCE_TRNG_HANDLE
        #define WOLFSSL_SCE_TRNG_HANDLE g_sce_trng
    #endif

    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        word32 ret;
        word32 blocks;
        word32 len = sz;

        ret = WOLFSSL_SCE_TRNG_HANDLE.p_api->open(WOLFSSL_SCE_TRNG_HANDLE.p_ctrl,
                                                  WOLFSSL_SCE_TRNG_HANDLE.p_cfg);
        if (ret != SSP_SUCCESS && ret != SSP_ERR_CRYPTO_ALREADY_OPEN) {
            /* error opening TRNG driver */
            return -1;
        }

        blocks = sz / sizeof(word32);
        if (blocks > 0) {
            ret = WOLFSSL_SCE_TRNG_HANDLE.p_api->read(WOLFSSL_SCE_TRNG_HANDLE.p_ctrl,
                                                       (word32*)output, blocks);
            if (ret != SSP_SUCCESS) {
                return -1;
            }
        }

        len = len - (blocks * sizeof(word32));
        if (len > 0) {
            word32 tmp;

            if (len > sizeof(word32)) {
                return -1;
            }
            ret = WOLFSSL_SCE_TRNG_HANDLE.p_api->read(WOLFSSL_SCE_TRNG_HANDLE.p_ctrl,
                                                      (word32*)&tmp, 1);
            if (ret != SSP_SUCCESS) {
                return -1;
            }
            XMEMCPY(output + (blocks * sizeof(word32)), (byte*)&tmp, len);
        }

        ret = WOLFSSL_SCE_TRNG_HANDLE.p_api->close(WOLFSSL_SCE_TRNG_HANDLE.p_ctrl);
        if (ret != SSP_SUCCESS) {
            /* error opening TRNG driver */
            return -1;
        }
        return 0;
    }
#elif defined(CUSTOM_RAND_GENERATE_BLOCK)
    /* #define CUSTOM_RAND_GENERATE_BLOCK myRngFunc
     * extern int myRngFunc(byte* output, word32 sz);
     */

#elif defined(WOLFSSL_SAFERTOS) || defined(WOLFSSL_LEANPSK) || \
      defined(WOLFSSL_IAR_ARM)  || defined(WOLFSSL_MDK_ARM) || \
      defined(WOLFSSL_uITRON4)  || defined(WOLFSSL_uTKERNEL2) || \
      defined(WOLFSSL_LPC43xx)  || defined(NO_STM32_RNG) || \
      defined(MBED)             || defined(WOLFSSL_EMBOS) || \
      defined(WOLFSSL_GENSEED_FORTEST) || defined(WOLFSSL_CHIBIOS) || \
      defined(WOLFSSL_CONTIKI)  || defined(WOLFSSL_AZSPHERE)

    /* these platforms do not have a default random seed and
       you'll need to implement your own wc_GenerateSeed or define via
       CUSTOM_RAND_GENERATE_BLOCK */

    #define USE_TEST_GENSEED

#elif defined(WOLFSSL_ZEPHYR)

        #include <zephyr/random/rand32.h>
    #ifndef _POSIX_C_SOURCE
        #include <zephyr/posix/time.h>
    #else
        #include <time.h>
    #endif

        int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
        {
            sys_rand_get(output, sz);
            return 0;
        }

#elif defined(WOLFSSL_TELIT_M2MB)

        #include "stdlib.h"
        static long get_timestamp(void) {
            long myTime = 0;
            INT32 fd = m2mb_rtc_open("/dev/rtc0", 0);
            if (fd >= 0) {
                M2MB_RTC_TIMEVAL_T timeval;
                m2mb_rtc_ioctl(fd, M2MB_RTC_IOCTL_GET_TIMEVAL, &timeval);
                myTime = timeval.msec;
                m2mb_rtc_close(fd);
            }
            return myTime;
        }
        int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
        {
            int i;
            srand(get_timestamp());
            for (i = 0; i < sz; i++ ) {
                output[i] = rand() % 256;
                if ((i % 8) == 7) {
                    srand(get_timestamp());
                }
            }
            return 0;
        }
#elif defined(WOLFSSL_SE050) && !defined(WOLFSSL_SE050_NO_TRNG)
     #include <wolfssl/wolfcrypt/port/nxp/se050_port.h>

    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz){
        int ret = 0;

        (void)os;

        if (output == NULL) {
            return BUFFER_E;
        }
        ret = wolfSSL_CryptHwMutexLock();
        if (ret == 0) {
            ret = se050_get_random_number(sz, output);
            wolfSSL_CryptHwMutexUnLock();
        }
        return ret;
    }

#elif defined(DOLPHIN_EMULATOR)

        int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
        {
            word32 i;
            (void)os;
            srand(time(NULL));
            for (i = 0; i < sz; i++)
                output[i] = (byte)rand();
            return 0;
        }

#elif defined(WOLFSSL_GETRANDOM)

    /* getrandom() was added to the Linux kernel in version 3.17.
     * Added to glibc in version 2.25. */
    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        int ret = 0;
        (void)os;

        while (sz) {
            int len;

            errno = 0;
            len = (int)getrandom(output, sz, 0);
            if (len == -1) {
                if (errno == EINTR) {
                    /* interrupted, call getrandom again */
                    continue;
                }
                else {
                    ret = READ_RAN_E;
                }
                break;
            }

            sz     -= len;
            output += len;
        }
        return ret;
    }

#elif defined(NO_DEV_RANDOM)

    #error "you need to write an os specific wc_GenerateSeed() here"

    /*
    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        return 0;
    }
    */

#else

    /* may block */
    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        int ret = 0;

        if (os == NULL) {
            return BAD_FUNC_ARG;
        }

    #ifdef WOLF_CRYPTO_CB
        #ifndef WOLF_CRYPTO_CB_FIND
        if (os->devId != INVALID_DEVID)
        #endif
        {
            ret = wc_CryptoCb_RandomSeed(os, output, sz);
            if (ret != CRYPTOCB_UNAVAILABLE)
                return ret;
            /* fall-through when unavailable */
            ret = 0; /* reset error code */
        }
    #endif

    #ifdef HAVE_ENTROPY_MEMUSE
        ret = wc_Entropy_Get(MAX_ENTROPY_BITS, output, sz);
        if (ret == 0) {
            return 0;
        }
     #ifdef ENTROPY_MEMUSE_FORCE_FAILURE
        /* Don't fallback to /dev/urandom. */
        return ret;
    #endif
    #endif

    #if defined(HAVE_INTEL_RDSEED) || defined(HAVE_AMD_RDSEED)
        if (IS_INTEL_RDSEED(intel_flags)) {
             ret = wc_GenerateSeed_IntelRD(NULL, output, sz);
             if (ret == 0) {
                 /* success, we're done */
                 return ret;
             }
        #ifdef FORCE_FAILURE_RDSEED
             /* don't fallback to /dev/urandom */
             return ret;
        #else
             /* reset error and fallback to using /dev/urandom */
             ret = 0;
        #endif
        }
    #endif /* HAVE_INTEL_RDSEED || HAVE_AMD_RDSEED */

    #ifndef NO_DEV_URANDOM /* way to disable use of /dev/urandom */
        os->fd = open("/dev/urandom", O_RDONLY);
        if (os->fd == -1)
    #endif
        {
            /* may still have /dev/random */
            os->fd = open("/dev/random", O_RDONLY);
            if (os->fd == -1)
                return OPEN_RAN_E;
        }

        while (sz) {
            int len = (int)read(os->fd, output, sz);
            if (len == -1) {
                ret = READ_RAN_E;
                break;
            }

            sz     -= (word32)len;
            output += len;

            if (sz) {
    #if defined(BLOCKING) || defined(WC_RNG_BLOCKING)
                sleep(0);             /* context switch */
    #else
                ret = RAN_BLOCK_E;
                break;
    #endif
            }
        }
        close(os->fd);

        return ret;
    }

#endif

#ifdef USE_TEST_GENSEED
    #ifndef _MSC_VER
        #warning "write a real random seed!!!!, just for testing now"
    #else
        #pragma message("Warning: write a real random seed!!!!, just for testing now")
    #endif
    int wc_GenerateSeed(OS_Seed* os, byte* output, word32 sz)
    {
        word32 i;
        for (i = 0; i < sz; i++ )
            output[i] = i;

        (void)os;

        return 0;
    }
#endif
/* End wc_GenerateSeed */

#if defined(CUSTOM_RAND_GENERATE_BLOCK) && defined(WOLFSSL_KCAPI)
#include <fcntl.h>
int wc_hwrng_generate_block(byte *output, word32 sz)
{
    int fd;
    int ret = 0;
    fd = open("/dev/hwrng", O_RDONLY);
    if (fd == -1)
        return OPEN_RAN_E;
    while(sz)
    {
        int len = (int)read(fd, output, sz);
        if (len == -1)
        {
            ret = READ_RAN_E;
            break;
        }
        sz -= len;
        output += len;
    }
    close(fd);
    return ret;
}
#endif

#endif /* WC_NO_RNG */
