/* sha256.c
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

/* For more info on the algorithm, see https://tools.ietf.org/html/rfc6234
 *
 * For more information on NIST FIPS PUB 180-4, see
 * https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf
 */

/*

DESCRIPTION
This library provides the interface to SHA-256 secure hash algorithms.
SHA-256 performs processing on message blocks to produce a final hash digest
output. It can be used to hash a message, M, having a length of L bits,
where 0 <= L < 2^64.

Note that in some cases, hardware acceleration may be enabled, depending
on the specific device platform.

*/

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/types.h>

/*
 * SHA256 Build Options:
 * USE_SLOW_SHA256:            Reduces code size by not partially unrolling
                                (~2KB smaller and ~25% slower) (default OFF)
 * WOLFSSL_SHA256_BY_SPEC:     Uses the Ch/Maj based on SHA256 specification
                                (default ON)
 * WOLFSSL_SHA256_ALT_CH_MAJ:  Alternate Ch/Maj that is easier for compilers to
                                optimize and recognize as SHA256 (default OFF)
 * SHA256_MANY_REGISTERS:      A SHA256 version that keeps all data in registers
                                and partial unrolled (default OFF)
 */

/* Default SHA256 to use Ch/Maj based on specification */
#if !defined(WOLFSSL_SHA256_BY_SPEC) && !defined(WOLFSSL_SHA256_ALT_CH_MAJ)
    #define WOLFSSL_SHA256_BY_SPEC
#endif


#if !defined(NO_SHA256) && !defined(WOLFSSL_ARMASM)

#if defined(HAVE_FIPS) && defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2)
    /* set NO_WRAPPERS before headers, use direct internal f()s not wrappers */
    #define FIPS_NO_WRAPPERS

    #ifdef USE_WINDOWS_API
        #pragma code_seg(".fipsA$d")
        #pragma const_seg(".fipsB$d")
    #endif
#endif

#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/cpuid.h>
#include <wolfssl/wolfcrypt/hash.h>

#ifdef WOLF_CRYPTO_CB
    #include <wolfssl/wolfcrypt/cryptocb.h>
#endif

#ifdef WOLFSSL_IMXRT1170_CAAM
    #include <wolfssl/wolfcrypt/port/caam/wolfcaam_fsl_nxp.h>
#endif


/* determine if we are using Espressif SHA hardware acceleration */
#undef WOLFSSL_USE_ESP32_CRYPT_HASH_HW
#if defined(WOLFSSL_ESP32_CRYPT) && \
    !defined(NO_WOLFSSL_ESP32_CRYPT_HASH)
    /* define a single keyword for simplicity & readability
     *
     * by default the HW acceleration is on for ESP32-WROOM32
     * but individual components can be turned off.
     */
    #define WOLFSSL_USE_ESP32_CRYPT_HASH_HW
#else
    #undef WOLFSSL_USE_ESP32_CRYPT_HASH_HW
#endif

#ifdef WOLFSSL_ESPIDF
    /* Define the ESP_LOGx(TAG, "" value for output messages here.
    **
    ** Beware of possible conflict in test.c (that one now named TEST_TAG)
    */
    static const char* TAG = "wc_sha256";
#endif

#if defined(WOLFSSL_TI_HASH)
    /* #include <wolfcrypt/src/port/ti/ti-hash.c> included by wc_port.c */
#elif defined(WOLFSSL_CRYPTOCELL)
    /* wc_port.c includes wolfcrypt/src/port/arm/cryptoCellHash.c */

#elif defined(WOLFSSL_PSOC6_CRYPTO)


#else

#include <wolfssl/wolfcrypt/logging.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#ifdef WOLFSSL_DEVCRYPTO_HASH
    #include <wolfssl/wolfcrypt/port/devcrypto/wc_devcrypto.h>
#endif
#if defined(WOLFSSL_SE050) && defined(WOLFSSL_SE050_HASH)
    #include <wolfssl/wolfcrypt/port/nxp/se050_port.h>
#endif


#if defined(WOLFSSL_X86_64_BUILD) && defined(USE_INTEL_SPEEDUP)
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
#else
    #undef HAVE_INTEL_AVX1
    #undef HAVE_INTEL_AVX2
#endif /* WOLFSSL_X86_64_BUILD && USE_INTEL_SPEEDUP */

#if defined(HAVE_INTEL_AVX2)
    #define HAVE_INTEL_RORX
#endif


#if !defined(WOLFSSL_PIC32MZ_HASH) && !defined(STM32_HASH_SHA2) && \
    (!defined(WOLFSSL_IMX6_CAAM) || defined(NO_IMX6_CAAM_HASH) || \
     defined(WOLFSSL_QNX_CAAM)) && \
    !defined(WOLFSSL_AFALG_HASH) && !defined(WOLFSSL_DEVCRYPTO_HASH) && \
    (!defined(WOLFSSL_ESP32_CRYPT) || defined(NO_WOLFSSL_ESP32_CRYPT_HASH)) && \
    ((!defined(WOLFSSL_RENESAS_TSIP_TLS) && \
      !defined(WOLFSSL_RENESAS_TSIP_CRYPTONLY)) || \
     defined(NO_WOLFSSL_RENESAS_TSIP_CRYPT_HASH)) && \
    !defined(WOLFSSL_PSOC6_CRYPTO) && !defined(WOLFSSL_IMXRT_DCP) && !defined(WOLFSSL_SILABS_SE_ACCEL) && \
    !defined(WOLFSSL_KCAPI_HASH) && !defined(WOLFSSL_SE050_HASH) && \
    ((!defined(WOLFSSL_RENESAS_SCEPROTECT) && \
        !defined(WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY)) \
      || defined(NO_WOLFSSL_RENESAS_SCEPROTECT_HASH)) && \
    (!defined(WOLFSSL_HAVE_PSA) || defined(WOLFSSL_PSA_NO_HASH)) && \
    !defined(WOLFSSL_RENESAS_RX64_HASH)


static int InitSha256(wc_Sha256* sha256)
{
    int ret = 0;

    if (sha256 == NULL)
        return BAD_FUNC_ARG;

    XMEMSET(sha256->digest, 0, sizeof(sha256->digest));
    sha256->digest[0] = 0x6A09E667L;
    sha256->digest[1] = 0xBB67AE85L;
    sha256->digest[2] = 0x3C6EF372L;
    sha256->digest[3] = 0xA54FF53AL;
    sha256->digest[4] = 0x510E527FL;
    sha256->digest[5] = 0x9B05688CL;
    sha256->digest[6] = 0x1F83D9ABL;
    sha256->digest[7] = 0x5BE0CD19L;

    sha256->buffLen = 0;
    sha256->loLen   = 0;
    sha256->hiLen   = 0;
#ifdef WOLFSSL_HASH_FLAGS
    sha256->flags = 0;
#endif
#ifdef WOLFSSL_HASH_KEEP
    sha256->msg  = NULL;
    sha256->len  = 0;
    sha256->used = 0;
#endif

#ifdef WOLF_CRYPTO_CB
    sha256->devId = wc_CryptoCb_DefaultDevID();
#endif

#ifdef WOLFSSL_MAXQ10XX_CRYPTO
    XMEMSET(&sha256->maxq_ctx, 0, sizeof(sha256->maxq_ctx));
#endif

#ifdef HAVE_ARIA
    sha256->hSession = NULL;
#endif

    return ret;
}
#endif


/* Hardware Acceleration */
#if defined(WOLFSSL_X86_64_BUILD) && defined(USE_INTEL_SPEEDUP) && \
                          (defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2))

    /* in case intel instructions aren't available, plus we need the K[] global */
    #define NEED_SOFT_SHA256

    /*****
    Intel AVX1/AVX2 Macro Control Structure

    #define HAVE_INTEL_AVX1
    #define HAVE_INTEL_AVX2

    #define HAVE_INTEL_RORX


    int InitSha256(wc_Sha256* sha256) {
         Save/Recover XMM, YMM
         ...
    }

    #if defined(HAVE_INTEL_AVX1)|| defined(HAVE_INTEL_AVX2)
      Transform_Sha256(); Function prototype
    #else
      Transform_Sha256() {   }
      int Sha256Final() {
         Save/Recover XMM, YMM
         ...
      }
    #endif

    #if defined(HAVE_INTEL_AVX1)|| defined(HAVE_INTEL_AVX2)
        #if defined(HAVE_INTEL_RORX
             #define RND with rorx instruction
        #else
            #define RND
        #endif
    #endif

    #if defined(HAVE_INTEL_AVX1)

       #define XMM Instructions/inline asm

       int Transform_Sha256() {
           Stitched Message Sched/Round
        }

    #elif defined(HAVE_INTEL_AVX2)

      #define YMM Instructions/inline asm

      int Transform_Sha256() {
          More granular Stitched Message Sched/Round
      }

    #endif

    */

    /* Each platform needs to query info type 1 from cpuid to see if aesni is
     * supported. Also, let's setup a macro for proper linkage w/o ABI conflicts
     */

    /* #if defined(HAVE_INTEL_AVX1/2) at the tail of sha256 */
    static int Transform_Sha256(wc_Sha256* sha256, const byte* data);

#ifdef __cplusplus
    extern "C" {
#endif

    #if defined(HAVE_INTEL_AVX1)
        extern int Transform_Sha256_AVX1(wc_Sha256 *sha256, const byte* data);
        extern int Transform_Sha256_AVX1_Len(wc_Sha256* sha256,
                                             const byte* data, word32 len);
    #endif
    #if defined(HAVE_INTEL_AVX2)
        extern int Transform_Sha256_AVX2(wc_Sha256 *sha256, const byte* data);
        extern int Transform_Sha256_AVX2_Len(wc_Sha256* sha256,
                                             const byte* data, word32 len);
        #ifdef HAVE_INTEL_RORX
        extern int Transform_Sha256_AVX1_RORX(wc_Sha256 *sha256, const byte* data);
        extern int Transform_Sha256_AVX1_RORX_Len(wc_Sha256* sha256,
                                                  const byte* data, word32 len);
        extern int Transform_Sha256_AVX2_RORX(wc_Sha256 *sha256, const byte* data);
        extern int Transform_Sha256_AVX2_RORX_Len(wc_Sha256* sha256,
                                                  const byte* data, word32 len);
        #endif /* HAVE_INTEL_RORX */
    #endif /* HAVE_INTEL_AVX2 */

#ifdef __cplusplus
    }  /* extern "C" */
#endif

    static int (*Transform_Sha256_p)(wc_Sha256* sha256, const byte* data);
                                                       /* = _Transform_Sha256 */
    static int (*Transform_Sha256_Len_p)(wc_Sha256* sha256, const byte* data,
                                         word32 len);
                                                                    /* = NULL */
    static int transform_check = 0;
    static word32 intel_flags;
    static int Transform_Sha256_is_vectorized = 0;

    static WC_INLINE int inline_XTRANSFORM(wc_Sha256* S, const byte* D) {
        int ret;
        ret = (*Transform_Sha256_p)(S, D);
        return ret;
    }
#define XTRANSFORM(...) inline_XTRANSFORM(__VA_ARGS__)

    static WC_INLINE int inline_XTRANSFORM_LEN(wc_Sha256* S, const byte* D, word32 L) {
        int ret;
        ret = (*Transform_Sha256_Len_p)(S, D, L);
        return ret;
    }
#define XTRANSFORM_LEN(...) inline_XTRANSFORM_LEN(__VA_ARGS__)

    static void Sha256_SetTransform(void)
    {

        if (transform_check)
            return;

        intel_flags = cpuid_get_flags();

    #ifdef HAVE_INTEL_AVX2
        if (1 && IS_INTEL_AVX2(intel_flags)) {
        #ifdef HAVE_INTEL_RORX
            if (IS_INTEL_BMI2(intel_flags)) {
                Transform_Sha256_p = Transform_Sha256_AVX2_RORX;
                Transform_Sha256_Len_p = Transform_Sha256_AVX2_RORX_Len;
                Transform_Sha256_is_vectorized = 1;
            }
            else
        #endif
            if (1)
            {
                Transform_Sha256_p = Transform_Sha256_AVX2;
                Transform_Sha256_Len_p = Transform_Sha256_AVX2_Len;
                Transform_Sha256_is_vectorized = 1;
            }
        #ifdef HAVE_INTEL_RORX
            else {
                Transform_Sha256_p = Transform_Sha256_AVX1_RORX;
                Transform_Sha256_Len_p = Transform_Sha256_AVX1_RORX_Len;
                Transform_Sha256_is_vectorized = 1;
            }
        #endif
        }
        else
    #endif
    #ifdef HAVE_INTEL_AVX1
        if (IS_INTEL_AVX1(intel_flags)) {
            Transform_Sha256_p = Transform_Sha256_AVX1;
            Transform_Sha256_Len_p = Transform_Sha256_AVX1_Len;
            Transform_Sha256_is_vectorized = 1;
        }
        else
    #endif
        {
            Transform_Sha256_p = Transform_Sha256;
            Transform_Sha256_Len_p = NULL;
            Transform_Sha256_is_vectorized = 0;
        }

        transform_check = 1;
    }

#if !defined(WOLFSSL_KCAPI_HASH)
    int wc_InitSha256_ex(wc_Sha256* sha256, void* heap, int devId)
    {
        int ret = 0;
        if (sha256 == NULL)
            return BAD_FUNC_ARG;

        sha256->heap = heap;
    #ifdef WOLF_CRYPTO_CB
        sha256->devId = devId;
        sha256->devCtx = NULL;
    #endif
    #ifdef WOLFSSL_SMALL_STACK_CACHE
        sha256->W = NULL;
    #endif

        ret = InitSha256(sha256);
        if (ret != 0)
            return ret;

        /* choose best Transform function under this runtime environment */
        Sha256_SetTransform();

    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA256)
        ret = wolfAsync_DevCtxInit(&sha256->asyncDev,
                            WOLFSSL_ASYNC_MARKER_SHA256, sha256->heap, devId);
    #else
        (void)devId;
    #endif /* WOLFSSL_ASYNC_CRYPT */

        return ret;
    }
#endif /* !WOLFSSL_KCAPI_HASH */

#elif defined(FREESCALE_LTC_SHA)
    int wc_InitSha256_ex(wc_Sha256* sha256, void* heap, int devId)
    {
        (void)heap;
        (void)devId;

        LTC_HASH_Init(LTC_BASE, &sha256->ctx, kLTC_Sha256, NULL, 0);

        return 0;
    }

#elif defined(FREESCALE_MMCAU_SHA)

    #ifdef FREESCALE_MMCAU_CLASSIC_SHA
        #include "cau_api.h"
    #else
        #include "fsl_mmcau.h"
    #endif

    #define XTRANSFORM(S, D)         Transform_Sha256((S),(D))
    #define XTRANSFORM_LEN(S, D, L)  Transform_Sha256_Len((S),(D),(L))

    #ifndef WC_HASH_DATA_ALIGNMENT
        /* these hardware API's require 4 byte (word32) alignment */
        #define WC_HASH_DATA_ALIGNMENT 4
    #endif

    int wc_InitSha256_ex(wc_Sha256* sha256, void* heap, int devId)
    {
        int ret = 0;

        (void)heap;
        (void)devId;

        ret = wolfSSL_CryptHwMutexLock();
        if (ret != 0) {
            return ret;
        }

    #ifdef FREESCALE_MMCAU_CLASSIC_SHA
        cau_sha256_initialize_output(sha256->digest);
    #else
        MMCAU_SHA256_InitializeOutput((uint32_t*)sha256->digest);
    #endif
        wolfSSL_CryptHwMutexUnLock();

        sha256->buffLen = 0;
        sha256->loLen   = 0;
        sha256->hiLen   = 0;
    #ifdef WOLFSSL_SMALL_STACK_CACHE
        sha256->W = NULL;
    #endif

        return ret;
    }

    static int Transform_Sha256(wc_Sha256* sha256, const byte* data)
    {
        int ret = wolfSSL_CryptHwMutexLock();
        if (ret == 0) {
    #ifdef FREESCALE_MMCAU_CLASSIC_SHA
            cau_sha256_hash_n((byte*)data, 1, sha256->digest);
    #else
            MMCAU_SHA256_HashN((byte*)data, 1, (uint32_t*)sha256->digest);
    #endif
            wolfSSL_CryptHwMutexUnLock();
        }
        return ret;
    }

    static int Transform_Sha256_Len(wc_Sha256* sha256, const byte* data,
        word32 len)
    {
        int ret = wolfSSL_CryptHwMutexLock();
        if (ret == 0) {
        #if defined(WC_HASH_DATA_ALIGNMENT) && WC_HASH_DATA_ALIGNMENT > 0
            if ((wc_ptr_t)data % WC_HASH_DATA_ALIGNMENT) {
                /* data pointer is NOT aligned,
                 * so copy and perform one block at a time */
                byte* local = (byte*)sha256->buffer;
                while (len >= WC_SHA256_BLOCK_SIZE) {
                    XMEMCPY(local, data, WC_SHA256_BLOCK_SIZE);
                #ifdef FREESCALE_MMCAU_CLASSIC_SHA
                    cau_sha256_hash_n(local, 1, sha256->digest);
                #else
                    MMCAU_SHA256_HashN(local, 1, (uint32_t*)sha256->digest);
                #endif
                    data += WC_SHA256_BLOCK_SIZE;
                    len  -= WC_SHA256_BLOCK_SIZE;
                }
            }
            else
        #endif
            {
    #ifdef FREESCALE_MMCAU_CLASSIC_SHA
            cau_sha256_hash_n((byte*)data, len/WC_SHA256_BLOCK_SIZE,
                sha256->digest);
    #else
            MMCAU_SHA256_HashN((byte*)data, len/WC_SHA256_BLOCK_SIZE,
                (uint32_t*)sha256->digest);
    #endif
            }
            wolfSSL_CryptHwMutexUnLock();
        }
        return ret;
    }

#elif defined(WOLFSSL_PIC32MZ_HASH)
    #include <wolfssl/wolfcrypt/port/pic32/pic32mz-crypt.h>

#elif defined(STM32_HASH_SHA2)

    /* Supports CubeMX HAL or Standard Peripheral Library */

    int wc_InitSha256_ex(wc_Sha256* sha256, void* heap, int devId)
    {
        if (sha256 == NULL)
            return BAD_FUNC_ARG;

        (void)devId;
        (void)heap;

        XMEMSET(sha256, 0, sizeof(wc_Sha256));
        wc_Stm32_Hash_Init(&sha256->stmCtx);
        return 0;
    }

    int wc_Sha256Update(wc_Sha256* sha256, const byte* data, word32 len)
    {
        int ret = 0;

        if (sha256 == NULL || (data == NULL && len > 0)) {
            return BAD_FUNC_ARG;
        }

        ret = wolfSSL_CryptHwMutexLock();
        if (ret == 0) {
            ret = wc_Stm32_Hash_Update(&sha256->stmCtx,
                HASH_AlgoSelection_SHA256, data, len, WC_SHA256_BLOCK_SIZE);
            wolfSSL_CryptHwMutexUnLock();
        }
        return ret;
    }

    int wc_Sha256Final(wc_Sha256* sha256, byte* hash)
    {
        int ret = 0;

        if (sha256 == NULL || hash == NULL) {
            return BAD_FUNC_ARG;
        }

        ret = wolfSSL_CryptHwMutexLock();
        if (ret == 0) {
            ret = wc_Stm32_Hash_Final(&sha256->stmCtx,
                HASH_AlgoSelection_SHA256, hash, WC_SHA256_DIGEST_SIZE);
            wolfSSL_CryptHwMutexUnLock();
        }

        (void)wc_InitSha256(sha256); /* reset state */

        return ret;
    }

#elif defined(WOLFSSL_IMX6_CAAM) && !defined(NO_IMX6_CAAM_HASH) && \
    !defined(WOLFSSL_QNX_CAAM)
    /* functions defined in wolfcrypt/src/port/caam/caam_sha256.c */

#elif defined(WOLFSSL_SE050) && defined(WOLFSSL_SE050_HASH)

    int wc_InitSha256_ex(wc_Sha256* sha256, void* heap, int devId)
    {
        if (sha256 == NULL) {
            return BAD_FUNC_ARG;
        }
        (void)devId;

        return se050_hash_init(&sha256->se050Ctx, heap);
    }

    int wc_Sha256Update(wc_Sha256* sha256, const byte* data, word32 len)
    {
        return se050_hash_update(&sha256->se050Ctx, data, len);
    }

    int wc_Sha256Final(wc_Sha256* sha256, byte* hash)
    {
        int ret = 0;
        ret = se050_hash_final(&sha256->se050Ctx, hash, WC_SHA256_DIGEST_SIZE,
                               kAlgorithm_SSS_SHA256);
        return ret;
    }
    int wc_Sha256FinalRaw(wc_Sha256* sha256, byte* hash)
    {
        int ret = 0;
        ret = se050_hash_final(&sha256->se050Ctx, hash, WC_SHA256_DIGEST_SIZE,
                               kAlgorithm_SSS_SHA256);
        return ret;
    }

#elif defined(WOLFSSL_AFALG_HASH)
    /* implemented in wolfcrypt/src/port/af_alg/afalg_hash.c */

#elif defined(WOLFSSL_DEVCRYPTO_HASH)
    /* implemented in wolfcrypt/src/port/devcrypto/devcrypt_hash.c */

#elif defined(WOLFSSL_SCE) && !defined(WOLFSSL_SCE_NO_HASH)
    #include "hal_data.h"

    #ifndef WOLFSSL_SCE_SHA256_HANDLE
        #define WOLFSSL_SCE_SHA256_HANDLE g_sce_hash_0
    #endif

    #define WC_SHA256_DIGEST_WORD_SIZE 16
    #define XTRANSFORM(S, D) wc_Sha256SCE_XTRANSFORM((S), (D))
    static int wc_Sha256SCE_XTRANSFORM(wc_Sha256* sha256, const byte* data)
    {
        if (WOLFSSL_SCE_GSCE_HANDLE.p_cfg->endian_flag ==
                CRYPTO_WORD_ENDIAN_LITTLE)
        {
            ByteReverseWords((word32*)data, (word32*)data,
                    WC_SHA256_BLOCK_SIZE);
            ByteReverseWords(sha256->digest, sha256->digest,
                    WC_SHA256_DIGEST_SIZE);
        }

        if (WOLFSSL_SCE_SHA256_HANDLE.p_api->hashUpdate(
                    WOLFSSL_SCE_SHA256_HANDLE.p_ctrl, (word32*)data,
                    WC_SHA256_DIGEST_WORD_SIZE, sha256->digest) != SSP_SUCCESS){
            WOLFSSL_MSG("Unexpected hardware return value");
            return WC_HW_E;
        }

        if (WOLFSSL_SCE_GSCE_HANDLE.p_cfg->endian_flag ==
                CRYPTO_WORD_ENDIAN_LITTLE)
        {
            ByteReverseWords((word32*)data, (word32*)data,
                    WC_SHA256_BLOCK_SIZE);
            ByteReverseWords(sha256->digest, sha256->digest,
                    WC_SHA256_DIGEST_SIZE);
        }

        return 0;
    }


    int wc_InitSha256_ex(wc_Sha256* sha256, void* heap, int devId)
    {
        int ret = 0;
        if (sha256 == NULL)
            return BAD_FUNC_ARG;

        sha256->heap = heap;

        ret = InitSha256(sha256);
        if (ret != 0)
            return ret;

        (void)devId;

        return ret;
    }

#elif defined(WOLFSSL_USE_ESP32_CRYPT_HASH_HW)

    /* HW may fail since there's only one, so we still need SW */
    #define NEED_SOFT_SHA256

    /*
    ** soft SHA needs initialization digest, but HW does not.
    */
    static int InitSha256(wc_Sha256* sha256)
    {
        int ret = 0; /* zero = success */

        if (sha256 == NULL) {
            return BAD_FUNC_ARG;
        }

        sha256->digest[0] = 0x6A09E667L;
        sha256->digest[1] = 0xBB67AE85L;
        sha256->digest[2] = 0x3C6EF372L;
        sha256->digest[3] = 0xA54FF53AL;
        sha256->digest[4] = 0x510E527FL;
        sha256->digest[5] = 0x9B05688CL;
        sha256->digest[6] = 0x1F83D9ABL;
        sha256->digest[7] = 0x5BE0CD19L;

        sha256->buffLen = 0;
        sha256->loLen   = 0;
        sha256->hiLen   = 0;

        ret = esp_sha_init(&(sha256->ctx), WC_HASH_TYPE_SHA256);

        return ret;
    }

    /*
    ** wolfCrypt InitSha256 external wrapper.
    **
    ** we'll assume this is ALWAYS for a new, uninitialized sha256
    */
    int wc_InitSha256_ex(wc_Sha256* sha256, void* heap, int devId)
    {
        (void)devId;
        if (sha256 == NULL) {
            return BAD_FUNC_ARG;
        }

    #ifdef WOLFSSL_USE_ESP32_CRYPT_HASH_HW
        /* We know this is a fresh, uninitialized item, so set to INIT */
        if (sha256->ctx.mode != ESP32_SHA_INIT) {
            ESP_LOGV(TAG, "Set ctx mode from prior value: "
                               "%d", sha256->ctx.mode);
        }
        sha256->ctx.mode = ESP32_SHA_INIT;
    #endif

        return InitSha256(sha256);
    }

#elif (defined(WOLFSSL_RENESAS_TSIP_TLS) || \
       defined(WOLFSSL_RENESAS_TSIP_CRYPTONLY)) && \
    !defined(NO_WOLFSSL_RENESAS_TSIP_CRYPT_HASH)

    /* implemented in wolfcrypt/src/port/Renesas/renesas_tsip_sha.c */

#elif (defined(WOLFSSL_RENESAS_SCEPROTECT) || \
        defined(WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY)) && \
    !defined(NO_WOLFSSL_RENESAS_SCEPROTECT_HASH)

    /* implemented in wolfcrypt/src/port/Renesas/renesas_sce_sha.c */

#elif defined(WOLFSSL_PSOC6_CRYPTO)

    /* implemented in wolfcrypt/src/port/cypress/psoc6_crypto.c */

#elif defined(WOLFSSL_IMXRT_DCP)
    #include <wolfssl/wolfcrypt/port/nxp/dcp_port.h>
    /* implemented in wolfcrypt/src/port/nxp/dcp_port.c */

#elif defined(WOLFSSL_SILABS_SE_ACCEL)
    /* implemented in wolfcrypt/src/port/silabs/silabs_hash.c */

#elif defined(WOLFSSL_KCAPI_HASH)
    /* implemented in wolfcrypt/src/port/kcapi/kcapi_hash.c */

#elif defined(WOLFSSL_HAVE_PSA) && !defined(WOLFSSL_PSA_NO_HASH)
    /* implemented in wolfcrypt/src/port/psa/psa_hash.c */

#elif defined(WOLFSSL_RENESAS_RX64_HASH)

    /* implemented in wolfcrypt/src/port/Renesas/renesas_rx64_hw_sha.c */

#else
    #define NEED_SOFT_SHA256

    int wc_InitSha256_ex(wc_Sha256* sha256, void* heap, int devId)
    {
        int ret = 0;
        if (sha256 == NULL)
            return BAD_FUNC_ARG;
        ret = InitSha256(sha256);
        if (ret != 0)
            return ret;

        sha256->heap = heap;
    #ifdef WOLF_CRYPTO_CB
        sha256->devId = devId;
        sha256->devCtx = NULL;
    #endif
    #ifdef WOLFSSL_SMALL_STACK_CACHE
        sha256->W = NULL;
    #endif

    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA256)
        ret = wolfAsync_DevCtxInit(&sha256->asyncDev,
                            WOLFSSL_ASYNC_MARKER_SHA256, sha256->heap, devId);
    #else
        (void)devId;
    #endif /* WOLFSSL_ASYNC_CRYPT */
    #ifdef WOLFSSL_IMXRT1170_CAAM
         ret = wc_CAAM_HashInit(&sha256->hndl, &sha256->ctx, WC_HASH_TYPE_SHA256);
    #endif

        return ret;
    }
#endif /* End Hardware Acceleration */

#ifdef NEED_SOFT_SHA256

    static const FLASH_QUALIFIER ALIGN32 word32 K[64] = {
        0x428A2F98L, 0x71374491L, 0xB5C0FBCFL, 0xE9B5DBA5L, 0x3956C25BL,
        0x59F111F1L, 0x923F82A4L, 0xAB1C5ED5L, 0xD807AA98L, 0x12835B01L,
        0x243185BEL, 0x550C7DC3L, 0x72BE5D74L, 0x80DEB1FEL, 0x9BDC06A7L,
        0xC19BF174L, 0xE49B69C1L, 0xEFBE4786L, 0x0FC19DC6L, 0x240CA1CCL,
        0x2DE92C6FL, 0x4A7484AAL, 0x5CB0A9DCL, 0x76F988DAL, 0x983E5152L,
        0xA831C66DL, 0xB00327C8L, 0xBF597FC7L, 0xC6E00BF3L, 0xD5A79147L,
        0x06CA6351L, 0x14292967L, 0x27B70A85L, 0x2E1B2138L, 0x4D2C6DFCL,
        0x53380D13L, 0x650A7354L, 0x766A0ABBL, 0x81C2C92EL, 0x92722C85L,
        0xA2BFE8A1L, 0xA81A664BL, 0xC24B8B70L, 0xC76C51A3L, 0xD192E819L,
        0xD6990624L, 0xF40E3585L, 0x106AA070L, 0x19A4C116L, 0x1E376C08L,
        0x2748774CL, 0x34B0BCB5L, 0x391C0CB3L, 0x4ED8AA4AL, 0x5B9CCA4FL,
        0x682E6FF3L, 0x748F82EEL, 0x78A5636FL, 0x84C87814L, 0x8CC70208L,
        0x90BEFFFAL, 0xA4506CEBL, 0xBEF9A3F7L, 0xC67178F2L
    };

/* Both versions of Ch and Maj are logically the same, but with the second set
    the compilers can recognize them better for optimization */
#ifdef WOLFSSL_SHA256_BY_SPEC
    /* SHA256 math based on specification */
    #define Ch(x,y,z)       ((z) ^ ((x) & ((y) ^ (z))))
    #define Maj(x,y,z)      ((((x) | (y)) & (z)) | ((x) & (y)))
#else
    /* SHA256 math reworked for easier compiler optimization */
    #define Ch(x,y,z)       ((((y) ^ (z)) & (x)) ^ (z))
    #define Maj(x,y,z)      ((((x) ^ (y)) & ((y) ^ (z))) ^ (y))
#endif
    #define R(x, n)         (((x) & 0xFFFFFFFFU) >> (n))

    #define S(x, n)         rotrFixed(x, n)
    #define Sigma0(x)       (S(x, 2)  ^ S(x, 13) ^ S(x, 22))
    #define Sigma1(x)       (S(x, 6)  ^ S(x, 11) ^ S(x, 25))
    #define Gamma0(x)       (S(x, 7)  ^ S(x, 18) ^ R(x, 3))
    #define Gamma1(x)       (S(x, 17) ^ S(x, 19) ^ R(x, 10))

    #define a(i) S[(0-(i)) & 7]
    #define b(i) S[(1-(i)) & 7]
    #define c(i) S[(2-(i)) & 7]
    #define d(i) S[(3-(i)) & 7]
    #define e(i) S[(4-(i)) & 7]
    #define f(i) S[(5-(i)) & 7]
    #define g(i) S[(6-(i)) & 7]
    #define h(i) S[(7-(i)) & 7]

    #ifndef XTRANSFORM
         #define XTRANSFORM(S, D)         Transform_Sha256((S),(D))
    #endif

#ifndef SHA256_MANY_REGISTERS
    #define RND(j) \
         t0 = h(j) + Sigma1(e(j)) + Ch(e(j), f(j), g(j)) + K[i+(j)] + W[i+(j)]; \
         t1 = Sigma0(a(j)) + Maj(a(j), b(j), c(j)); \
         d(j) += t0; \
         h(j)  = t0 + t1

    static int Transform_Sha256(wc_Sha256* sha256, const byte* data)
    {
        word32 S[8], t0, t1;
        int i;

    #ifdef WOLFSSL_SMALL_STACK_CACHE
        word32* W = sha256->W;
        if (W == NULL) {
            W = (word32*)XMALLOC(sizeof(word32) * WC_SHA256_BLOCK_SIZE, NULL,
                                                           DYNAMIC_TYPE_DIGEST);
            if (W == NULL)
                return MEMORY_E;
            sha256->W = W;
        }
    #elif defined(WOLFSSL_SMALL_STACK)
        word32* W;
        W = (word32*)XMALLOC(sizeof(word32) * WC_SHA256_BLOCK_SIZE, NULL,
                                                       DYNAMIC_TYPE_TMP_BUFFER);
        if (W == NULL)
            return MEMORY_E;
    #else
        word32 W[WC_SHA256_BLOCK_SIZE];
    #endif

        /* Copy context->state[] to working vars */
        for (i = 0; i < 8; i++)
            S[i] = sha256->digest[i];

        for (i = 0; i < 16; i++)
            W[i] = *((const word32*)&data[i*(int)sizeof(word32)]);

        for (i = 16; i < WC_SHA256_BLOCK_SIZE; i++)
            W[i] = Gamma1(W[i-2]) + W[i-7] + Gamma0(W[i-15]) + W[i-16];

    #ifdef USE_SLOW_SHA256
        /* not unrolled - ~2k smaller and ~25% slower */
        for (i = 0; i < WC_SHA256_BLOCK_SIZE; i += 8) {
            int j;
            for (j = 0; j < 8; j++) { /* braces needed here for macros {} */
                RND(j);
            }
        }
    #else
        /* partially loop unrolled */
        for (i = 0; i < WC_SHA256_BLOCK_SIZE; i += 8) {
            RND(0); RND(1); RND(2); RND(3);
            RND(4); RND(5); RND(6); RND(7);
        }
    #endif /* USE_SLOW_SHA256 */

        /* Add the working vars back into digest state[] */
        for (i = 0; i < 8; i++) {
            sha256->digest[i] += S[i];
        }

    #if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SMALL_STACK_CACHE)
        XFREE(W, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif
        return 0;
    }
#else
    /* SHA256 version that keeps all data in registers */
    #define SCHED1(j) (W[j] = *((word32*)&data[j*sizeof(word32)]))
    #define SCHED(j) (               \
                   W[ j     & 15] += \
            Gamma1(W[(j-2)  & 15])+  \
                   W[(j-7)  & 15] +  \
            Gamma0(W[(j-15) & 15])   \
        )

    #define RND1(j) \
         t0 = h(j) + Sigma1(e(j)) + Ch(e(j), f(j), g(j)) + K[i+j] + SCHED1(j); \
         t1 = Sigma0(a(j)) + Maj(a(j), b(j), c(j)); \
         d(j) += t0; \
         h(j)  = t0 + t1
    #define RNDN(j) \
         t0 = h(j) + Sigma1(e(j)) + Ch(e(j), f(j), g(j)) + K[i+j] + SCHED(j); \
         t1 = Sigma0(a(j)) + Maj(a(j), b(j), c(j)); \
         d(j) += t0; \
         h(j)  = t0 + t1

    static int Transform_Sha256(wc_Sha256* sha256, const byte* data)
    {
        word32 S[8], t0, t1;
        int i;
        word32 W[WC_SHA256_BLOCK_SIZE/sizeof(word32)];

        /* Copy digest to working vars */
        S[0] = sha256->digest[0];
        S[1] = sha256->digest[1];
        S[2] = sha256->digest[2];
        S[3] = sha256->digest[3];
        S[4] = sha256->digest[4];
        S[5] = sha256->digest[5];
        S[6] = sha256->digest[6];
        S[7] = sha256->digest[7];

        i = 0;
        RND1( 0); RND1( 1); RND1( 2); RND1( 3);
        RND1( 4); RND1( 5); RND1( 6); RND1( 7);
        RND1( 8); RND1( 9); RND1(10); RND1(11);
        RND1(12); RND1(13); RND1(14); RND1(15);
        /* 64 operations, partially loop unrolled */
        for (i = 16; i < 64; i += 16) {
            RNDN( 0); RNDN( 1); RNDN( 2); RNDN( 3);
            RNDN( 4); RNDN( 5); RNDN( 6); RNDN( 7);
            RNDN( 8); RNDN( 9); RNDN(10); RNDN(11);
            RNDN(12); RNDN(13); RNDN(14); RNDN(15);
        }

        /* Add the working vars back into digest */
        sha256->digest[0] += S[0];
        sha256->digest[1] += S[1];
        sha256->digest[2] += S[2];
        sha256->digest[3] += S[3];
        sha256->digest[4] += S[4];
        sha256->digest[5] += S[5];
        sha256->digest[6] += S[6];
        sha256->digest[7] += S[7];

        return 0;
    }
#endif /* SHA256_MANY_REGISTERS */
#endif
/* End wc_ software implementation */


#ifdef XTRANSFORM

    static WC_INLINE void AddLength(wc_Sha256* sha256, word32 len)
    {
        word32 tmp = sha256->loLen;
        if ((sha256->loLen += len) < tmp) {
            sha256->hiLen++;                       /* carry low to high */
        }
    }

    /* do block size increments/updates */
    static WC_INLINE int Sha256Update(wc_Sha256* sha256, const byte* data, word32 len)
    {
        int ret = 0;
        word32 blocksLen;
        byte* local;

        if (sha256 == NULL || (data == NULL && len > 0)) {
            return BAD_FUNC_ARG;
        }

        if (data == NULL && len == 0) {
            /* valid, but do nothing */
            return 0;
        }

        /* check that internal buffLen is valid */
        if (sha256->buffLen >= WC_SHA256_BLOCK_SIZE) {
            return BUFFER_E;
        }

        /* add length for final */
        AddLength(sha256, len);

        local = (byte*)sha256->buffer;

        /* process any remainder from previous operation */
        if (sha256->buffLen > 0) {
            blocksLen = min(len, WC_SHA256_BLOCK_SIZE - sha256->buffLen);
            XMEMCPY(&local[sha256->buffLen], data, blocksLen);

            sha256->buffLen += blocksLen;
            data            += blocksLen;
            len             -= blocksLen;

            if (sha256->buffLen == WC_SHA256_BLOCK_SIZE) {
            #if defined(LITTLE_ENDIAN_ORDER) && !defined(FREESCALE_MMCAU_SHA)
                #if defined(WOLFSSL_X86_64_BUILD) && \
                          defined(USE_INTEL_SPEEDUP) && \
                          (defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2))
                if (!IS_INTEL_AVX1(intel_flags) && !IS_INTEL_AVX2(intel_flags))
                #endif
                {
                    ByteReverseWords(sha256->buffer, sha256->buffer,
                        WC_SHA256_BLOCK_SIZE);
                }
            #endif

            #if defined(WOLFSSL_USE_ESP32_CRYPT_HASH_HW)
                if (sha256->ctx.mode == ESP32_SHA_INIT) {
                    ESP_LOGV(TAG, "Sha256Update try hardware");
                    esp_sha_try_hw_lock(&sha256->ctx);
                }

                if (sha256->ctx.mode == ESP32_SHA_SW) {
                    ESP_LOGV(TAG, "Sha256Update process software");
                    ret = XTRANSFORM(sha256, (const byte*)local);
                }
                else {
                    ESP_LOGV(TAG, "Sha256Update process hardware");
                    esp_sha256_process(sha256, (const byte*)local);
                }
            #else
                ret = XTRANSFORM(sha256, (const byte*)local);
            #endif

                if (ret == 0)
                    sha256->buffLen = 0;
                else
                    len = 0; /* error */
            }
        }

        /* process blocks */
    #ifdef XTRANSFORM_LEN
        #if defined(WOLFSSL_X86_64_BUILD) && defined(USE_INTEL_SPEEDUP) && \
                          (defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2))
        if (Transform_Sha256_Len_p != NULL)
        #endif
        {
            /* get number of blocks */
            /* 64-1 = 0x3F (~ Inverted = 0xFFFFFFC0) */
            /* len (masked by 0xFFFFFFC0) returns block aligned length */
            blocksLen = len & ~((word32)WC_SHA256_BLOCK_SIZE-1);
            if (blocksLen > 0) {
                /* Byte reversal and alignment handled in function if required */
                XTRANSFORM_LEN(sha256, data, blocksLen);
                data += blocksLen;
                len  -= blocksLen;
            }
        }
        #if defined(WOLFSSL_X86_64_BUILD) && defined(USE_INTEL_SPEEDUP) && \
                          (defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2))
        else
        #endif
    #endif /* XTRANSFORM_LEN */
    #if !defined(XTRANSFORM_LEN) || \
        (defined(WOLFSSL_X86_64_BUILD) && defined(USE_INTEL_SPEEDUP) && \
         (defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2)))
        {
            while (len >= WC_SHA256_BLOCK_SIZE) {
                word32* local32 = sha256->buffer;
                /* optimization to avoid memcpy if data pointer is properly aligned */
                /* Intel transform function requires use of sha256->buffer */
                /* Little Endian requires byte swap, so can't use data directly */
            #if defined(WC_HASH_DATA_ALIGNMENT) && !defined(LITTLE_ENDIAN_ORDER) && \
                !(defined(WOLFSSL_X86_64_BUILD) && \
                         defined(USE_INTEL_SPEEDUP) && \
                         (defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2)))
                if (((wc_ptr_t)data % WC_HASH_DATA_ALIGNMENT) == 0) {
                    local32 = (word32*)data;
                }
                else
            #endif
                {
                    XMEMCPY(local32, data, WC_SHA256_BLOCK_SIZE);
                }

                data += WC_SHA256_BLOCK_SIZE;
                len  -= WC_SHA256_BLOCK_SIZE;

            #if defined(LITTLE_ENDIAN_ORDER) && !defined(FREESCALE_MMCAU_SHA)
                #if defined(WOLFSSL_X86_64_BUILD) && \
                          defined(USE_INTEL_SPEEDUP) && \
                          (defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2))
                if (!IS_INTEL_AVX1(intel_flags) && !IS_INTEL_AVX2(intel_flags))
                #endif
                {
                    ByteReverseWords(local32, local32, WC_SHA256_BLOCK_SIZE);
                }
            #endif

            #if defined(WOLFSSL_USE_ESP32_CRYPT_HASH_HW)
                if (sha256->ctx.mode == ESP32_SHA_INIT){
                    ESP_LOGV(TAG, "Sha256Update try hardware loop");
                    esp_sha_try_hw_lock(&sha256->ctx);
                }
                if (sha256->ctx.mode == ESP32_SHA_SW) {
                    ESP_LOGV(TAG, "Sha256Update process software loop");
                    ret = XTRANSFORM(sha256, (const byte*)local32);
                }
                else {
                    ESP_LOGV(TAG, "Sha256Update process hardware");
                    esp_sha256_process(sha256, (const byte*)local32);
                }
            #else
                ret = XTRANSFORM(sha256, (const byte*)local32);
            #endif

                if (ret != 0)
                    break;
            }
        }
    #endif

        /* save remainder */
        if (ret == 0 && len > 0) {
            XMEMCPY(local, data, len);
            sha256->buffLen = len;
        }

        return ret;
    }

#if defined(WOLFSSL_KCAPI_HASH)
    /* implemented in wolfcrypt/src/port/kcapi/kcapi_hash.c */

#else
    int wc_Sha256Update(wc_Sha256* sha256, const byte* data, word32 len)
    {
        if (sha256 == NULL || (data == NULL && len > 0)) {
            return BAD_FUNC_ARG;
        }

        if (data == NULL && len == 0) {
            /* valid, but do nothing */
            return 0;
        }

    #ifdef WOLF_CRYPTO_CB
        #ifndef WOLF_CRYPTO_CB_FIND
        if (sha256->devId != INVALID_DEVID)
        #endif
        {
            int ret = wc_CryptoCb_Sha256Hash(sha256, data, len, NULL);
            if (ret != CRYPTOCB_UNAVAILABLE)
                return ret;
            /* fall-through when unavailable */
        }
    #endif
    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA256)
        if (sha256->asyncDev.marker == WOLFSSL_ASYNC_MARKER_SHA256) {
        #if defined(HAVE_INTEL_QA)
            return IntelQaSymSha256(&sha256->asyncDev, NULL, data, len);
        #endif
        }
    #endif /* WOLFSSL_ASYNC_CRYPT */

        return Sha256Update(sha256, data, len);
    }
#endif

    static WC_INLINE int Sha256Final(wc_Sha256* sha256)
    {

        int ret;
        byte* local;

        if (sha256 == NULL) {
            return BAD_FUNC_ARG;
        }

        /* we'll add a 0x80 byte at the end,
        ** so make sure we have appropriate buffer length. */
        if (sha256->buffLen > WC_SHA256_BLOCK_SIZE - 1) {
            /* exit with error code if there's a bad buffer size in buffLen */
            return BAD_STATE_E;
        } /* buffLen check */

        local = (byte*)sha256->buffer;
        local[sha256->buffLen++] = 0x80; /* add 1 */

        /* pad with zeros */
        if (sha256->buffLen > WC_SHA256_PAD_SIZE) {
            XMEMSET(&local[sha256->buffLen], 0,
                WC_SHA256_BLOCK_SIZE - sha256->buffLen);
            sha256->buffLen += WC_SHA256_BLOCK_SIZE - sha256->buffLen;

        #if defined(LITTLE_ENDIAN_ORDER) && !defined(FREESCALE_MMCAU_SHA)
            #if defined(WOLFSSL_X86_64_BUILD) && defined(USE_INTEL_SPEEDUP) && \
                          (defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2))
            if (!IS_INTEL_AVX1(intel_flags) && !IS_INTEL_AVX2(intel_flags))
            #endif
            {
                ByteReverseWords(sha256->buffer, sha256->buffer,
                                                      WC_SHA256_BLOCK_SIZE);
            }
        #endif

        #if defined(WOLFSSL_USE_ESP32_CRYPT_HASH_HW)
            if (sha256->ctx.mode == ESP32_SHA_INIT) {
                esp_sha_try_hw_lock(&sha256->ctx);
            }
            if (sha256->ctx.mode == ESP32_SHA_SW) {
                ret = XTRANSFORM(sha256, (const byte*)local);
            }
            else {
                ret = esp_sha256_process(sha256, (const byte*)local);
            }
        #else
            ret = XTRANSFORM(sha256, (const byte*)local);
        #endif
            if (ret != 0)
                return ret;

            sha256->buffLen = 0;
        }
        XMEMSET(&local[sha256->buffLen], 0,
            WC_SHA256_PAD_SIZE - sha256->buffLen);

        /* put lengths in bits */
        sha256->hiLen = (sha256->loLen >> (8 * sizeof(sha256->loLen) - 3)) +
                                                         (sha256->hiLen << 3);
        sha256->loLen = sha256->loLen << 3;

        /* store lengths */
    #if defined(LITTLE_ENDIAN_ORDER) && !defined(FREESCALE_MMCAU_SHA)
        #if defined(WOLFSSL_X86_64_BUILD) && defined(USE_INTEL_SPEEDUP) && \
                          (defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2))
        if (!IS_INTEL_AVX1(intel_flags) && !IS_INTEL_AVX2(intel_flags))
        #endif
        {
            ByteReverseWords(sha256->buffer, sha256->buffer,
                WC_SHA256_BLOCK_SIZE);
        }
    #endif
        /* ! length ordering dependent on digest endian type ! */
        XMEMCPY(&local[WC_SHA256_PAD_SIZE], &sha256->hiLen, sizeof(word32));
        XMEMCPY(&local[WC_SHA256_PAD_SIZE + sizeof(word32)], &sha256->loLen,
                sizeof(word32));

    #if defined(FREESCALE_MMCAU_SHA) || \
        (defined(WOLFSSL_X86_64_BUILD) && defined(USE_INTEL_SPEEDUP) && \
                         (defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2)))
        /* Kinetis requires only these bytes reversed */
        #if defined(WOLFSSL_X86_64_BUILD) && defined(USE_INTEL_SPEEDUP) && \
                          (defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2))
        if (IS_INTEL_AVX1(intel_flags) || IS_INTEL_AVX2(intel_flags))
        #endif
        {
            ByteReverseWords(
                &sha256->buffer[WC_SHA256_PAD_SIZE / sizeof(word32)],
                &sha256->buffer[WC_SHA256_PAD_SIZE / sizeof(word32)],
                2 * sizeof(word32));
        }
    #endif

    #if defined(WOLFSSL_USE_ESP32_CRYPT_HASH_HW)
        if (sha256->ctx.mode == ESP32_SHA_INIT) {
            esp_sha_try_hw_lock(&sha256->ctx);
        }
        if (sha256->ctx.mode == ESP32_SHA_SW) {
            ret = XTRANSFORM(sha256, (const byte*)local);
        }
        else {
            ret = esp_sha256_digest_process(sha256, 1);
        }
    #else
        ret = XTRANSFORM(sha256, (const byte*)local);
    #endif

        return ret;
    }

#if !defined(WOLFSSL_KCAPI_HASH)

    int wc_Sha256FinalRaw(wc_Sha256* sha256, byte* hash)
    {
    #ifdef LITTLE_ENDIAN_ORDER
        word32 digest[WC_SHA256_DIGEST_SIZE / sizeof(word32)];
    #endif

        if (sha256 == NULL || hash == NULL) {
            return BAD_FUNC_ARG;
        }

    #ifdef LITTLE_ENDIAN_ORDER
        ByteReverseWords((word32*)digest, (word32*)sha256->digest,
                                                         WC_SHA256_DIGEST_SIZE);
        XMEMCPY(hash, digest, WC_SHA256_DIGEST_SIZE);
    #else
        XMEMCPY(hash, sha256->digest, WC_SHA256_DIGEST_SIZE);
    #endif

        return 0;
    }

    int wc_Sha256Final(wc_Sha256* sha256, byte* hash)
    {
        int ret;

        if (sha256 == NULL || hash == NULL) {
            return BAD_FUNC_ARG;
        }

    #ifdef WOLF_CRYPTO_CB
        #ifndef WOLF_CRYPTO_CB_FIND
        if (sha256->devId != INVALID_DEVID)
        #endif
        {
            ret = wc_CryptoCb_Sha256Hash(sha256, NULL, 0, hash);
            if (ret != CRYPTOCB_UNAVAILABLE)
                return ret;
            /* fall-through when unavailable */
        }
    #endif

    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA256)
        if (sha256->asyncDev.marker == WOLFSSL_ASYNC_MARKER_SHA256) {
        #if defined(HAVE_INTEL_QA)
            return IntelQaSymSha256(&sha256->asyncDev, hash, NULL,
                                            WC_SHA256_DIGEST_SIZE);
        #endif
        }
    #endif /* WOLFSSL_ASYNC_CRYPT */

        ret = Sha256Final(sha256);
        if (ret != 0) {
            return ret;
        }

    #if defined(LITTLE_ENDIAN_ORDER)
        ByteReverseWords(sha256->digest, sha256->digest, WC_SHA256_DIGEST_SIZE);
    #endif
        XMEMCPY(hash, sha256->digest, WC_SHA256_DIGEST_SIZE);

        return InitSha256(sha256);  /* reset state */
    }

#if defined(OPENSSL_EXTRA) || defined(HAVE_CURL)
/* Apply SHA256 transformation to the data                */
/* @param sha  a pointer to wc_Sha256 structure           */
/* @param data data to be applied SHA256 transformation   */
/* @return 0 on successful, otherwise non-zero on failure */
    int wc_Sha256Transform(wc_Sha256* sha, const unsigned char* data)
    {
        if (sha == NULL || data == NULL) {
            return BAD_FUNC_ARG;
        }
        return (Transform_Sha256(sha, data));
    }
    #endif
#endif /* OPENSSL_EXTRA */

#endif /* !WOLFSSL_KCAPI_HASH */


#ifdef WOLFSSL_SHA224

#ifdef STM32_HASH_SHA2

    /* Supports CubeMX HAL or Standard Peripheral Library */

    int wc_InitSha224_ex(wc_Sha224* sha224, void* heap, int devId)
    {
        if (sha224 == NULL)
            return BAD_FUNC_ARG;
        (void)devId;
        (void)heap;

        XMEMSET(sha224, 0, sizeof(wc_Sha224));
        wc_Stm32_Hash_Init(&sha224->stmCtx);
        return 0;
    }

    int wc_Sha224Update(wc_Sha224* sha224, const byte* data, word32 len)
    {
        int ret = 0;

        if (sha224 == NULL || (data == NULL && len > 0)) {
            return BAD_FUNC_ARG;
        }

        ret = wolfSSL_CryptHwMutexLock();
        if (ret == 0) {
            ret = wc_Stm32_Hash_Update(&sha224->stmCtx,
                HASH_AlgoSelection_SHA224, data, len, WC_SHA224_BLOCK_SIZE);
            wolfSSL_CryptHwMutexUnLock();
        }
        return ret;
    }

    int wc_Sha224Final(wc_Sha224* sha224, byte* hash)
    {
        int ret = 0;

        if (sha224 == NULL || hash == NULL) {
            return BAD_FUNC_ARG;
        }

        ret = wolfSSL_CryptHwMutexLock();
        if (ret == 0) {
            ret = wc_Stm32_Hash_Final(&sha224->stmCtx,
                HASH_AlgoSelection_SHA224, hash, WC_SHA224_DIGEST_SIZE);
            wolfSSL_CryptHwMutexUnLock();
        }

        (void)wc_InitSha224(sha224); /* reset state */

        return ret;
    }
#elif defined(WOLFSSL_SE050) && defined(WOLFSSL_SE050_HASH)

    int wc_InitSha224_ex(wc_Sha224* sha224, void* heap, int devId)
    {
        if (sha224 == NULL) {
            return BAD_FUNC_ARG;
        }
        (void)devId;

        return se050_hash_init(&sha224->se050Ctx, heap);
    }

    int wc_Sha224Update(wc_Sha224* sha224, const byte* data, word32 len)
    {
        return se050_hash_update(&sha224->se050Ctx, data, len);
    }

    int wc_Sha224Final(wc_Sha224* sha224, byte* hash)
    {
        int ret = 0;
        ret = se050_hash_final(&sha224->se050Ctx, hash, WC_SHA224_DIGEST_SIZE,
                               kAlgorithm_SSS_SHA224);
        (void)wc_InitSha224(sha224);
        return ret;
    }

#elif defined(WOLFSSL_IMX6_CAAM) && !defined(NO_IMX6_CAAM_HASH) && \
    !defined(WOLFSSL_QNX_CAAM)
    /* functions defined in wolfcrypt/src/port/caam/caam_sha256.c */

#elif defined(WOLFSSL_AFALG_HASH)
    #error SHA224 currently not supported with AF_ALG enabled

#elif defined(WOLFSSL_DEVCRYPTO_HASH)
    /* implemented in wolfcrypt/src/port/devcrypto/devcrypt_hash.c */

#elif defined(WOLFSSL_SILABS_SE_ACCEL)
    /* implemented in wolfcrypt/src/port/silabs/silabs_hash.c */

#elif defined(WOLFSSL_KCAPI_HASH) && !defined(WOLFSSL_NO_KCAPI_SHA224)
    /* implemented in wolfcrypt/src/port/kcapi/kcapi_hash.c */

#elif defined(WOLFSSL_HAVE_PSA) && !defined(WOLFSSL_PSA_NO_HASH)
    /* implemented in wolfcrypt/src/port/psa/psa_hash.c */

#elif defined(WOLFSSL_RENESAS_RX64_HASH)

/* implemented in wolfcrypt/src/port/Renesas/renesas_rx64_hw_sha.c */

#else

    #define NEED_SOFT_SHA224


    static int InitSha224(wc_Sha224* sha224)
    {
        int ret = 0;

        if (sha224 == NULL) {
            return BAD_FUNC_ARG;
        }

        sha224->digest[0] = 0xc1059ed8;
        sha224->digest[1] = 0x367cd507;
        sha224->digest[2] = 0x3070dd17;
        sha224->digest[3] = 0xf70e5939;
        sha224->digest[4] = 0xffc00b31;
        sha224->digest[5] = 0x68581511;
        sha224->digest[6] = 0x64f98fa7;
        sha224->digest[7] = 0xbefa4fa4;

        sha224->buffLen = 0;
        sha224->loLen   = 0;
        sha224->hiLen   = 0;

    #if defined(WOLFSSL_X86_64_BUILD) && defined(USE_INTEL_SPEEDUP) && \
                          (defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2))
        /* choose best Transform function under this runtime environment */
        Sha256_SetTransform();
    #endif
    #ifdef WOLFSSL_HASH_FLAGS
        sha224->flags = 0;
    #endif
    #ifdef WOLFSSL_HASH_KEEP
        sha224->msg  = NULL;
        sha224->len  = 0;
        sha224->used = 0;
    #endif

    #if defined(WOLFSSL_USE_ESP32_CRYPT_HASH_HW)
        /* not to be confused with HAS512_224 */
        sha224->ctx.mode = ESP32_SHA_SW; /* no SHA224 HW, so always SW */
    #endif

        return ret;
    }

#endif

#ifdef NEED_SOFT_SHA224
    int wc_InitSha224_ex(wc_Sha224* sha224, void* heap, int devId)
    {
        int ret = 0;

        if (sha224 == NULL)
            return BAD_FUNC_ARG;

        sha224->heap = heap;
    #ifdef WOLFSSL_SMALL_STACK_CACHE
        sha224->W = NULL;
    #endif

    #ifdef WOLFSSL_USE_ESP32_CRYPT_HASH_HW
        /* We know this is a fresh, uninitialized item, so set to INIT */
        if (sha224->ctx.mode != ESP32_SHA_SW) {
            ESP_LOGV(TAG, "Set sha224 ctx mode init to ESP32_SHA_SW. "
                          "Prior value: %d", sha224->ctx.mode);
        }
        /* no sha224 HW support is available, set to SW */
        sha224->ctx.mode = ESP32_SHA_SW;
    #endif

        ret = InitSha224(sha224);
        if (ret != 0) {
            return ret;
        }

    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA224)
        ret = wolfAsync_DevCtxInit(&sha224->asyncDev,
                            WOLFSSL_ASYNC_MARKER_SHA224, sha224->heap, devId);
    #else
        (void)devId;
    #endif /* WOLFSSL_ASYNC_CRYPT */
#ifdef WOLFSSL_IMXRT1170_CAAM
     ret = wc_CAAM_HashInit(&sha224->hndl, &sha224->ctx, WC_HASH_TYPE_SHA224);
#endif

    #ifdef WOLFSSL_USE_ESP32_CRYPT_HASH_HW
        if (sha224->ctx.mode != ESP32_SHA_INIT) {
            ESP_LOGV("SHA224", "Set ctx mode from prior value: "
                               "%d", sha224->ctx.mode);
        }
        /* We know this is a fresh, uninitialized item, so set to INIT */
        sha224->ctx.mode = ESP32_SHA_INIT;
    #endif

        return ret;
    }

    int wc_Sha224Update(wc_Sha224* sha224, const byte* data, word32 len)
    {
        int ret;

        if (sha224 == NULL || (data == NULL && len > 0)) {
            return BAD_FUNC_ARG;
        }

    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA224)
        if (sha224->asyncDev.marker == WOLFSSL_ASYNC_MARKER_SHA224) {
        #if defined(HAVE_INTEL_QA)
            return IntelQaSymSha224(&sha224->asyncDev, NULL, data, len);
        #endif
        }
    #endif /* WOLFSSL_ASYNC_CRYPT */

    #if defined(WOLFSSL_USE_ESP32_CRYPT_HASH_HW)
        sha224->ctx.mode = ESP32_SHA_SW; /* no SHA224 HW, so always SW */
    #endif

        ret = Sha256Update((wc_Sha256*)sha224, data, len);

        return ret;
    }

    int wc_Sha224Final(wc_Sha224* sha224, byte* hash)
    {
        int ret;

        if (sha224 == NULL || hash == NULL) {
            return BAD_FUNC_ARG;
        }

    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA224)
        if (sha224->asyncDev.marker == WOLFSSL_ASYNC_MARKER_SHA224) {
        #if defined(HAVE_INTEL_QA)
            return IntelQaSymSha224(&sha224->asyncDev, hash, NULL,
                                            WC_SHA224_DIGEST_SIZE);
        #endif
        }
    #endif /* WOLFSSL_ASYNC_CRYPT */

    #if defined(WOLFSSL_USE_ESP32_CRYPT_HASH_HW)
        sha224->ctx.mode = ESP32_SHA_SW; /* no SHA224 HW, so always SW */
    #endif

        ret = Sha256Final((wc_Sha256*)sha224);
        if (ret != 0)
            return ret;

    #if defined(LITTLE_ENDIAN_ORDER)
        ByteReverseWords(sha224->digest, sha224->digest, WC_SHA224_DIGEST_SIZE);
    #endif
        XMEMCPY(hash, sha224->digest, WC_SHA224_DIGEST_SIZE);

        return InitSha224(sha224);  /* reset state */
    }
#endif /* end of SHA224 software implementation */

    int wc_InitSha224(wc_Sha224* sha224)
    {
        int devId = INVALID_DEVID;

    #ifdef WOLF_CRYPTO_CB
        devId = wc_CryptoCb_DefaultDevID();
    #endif
        return wc_InitSha224_ex(sha224, NULL, devId);
    }

#if !defined(WOLFSSL_HAVE_PSA) || defined(WOLFSSL_PSA_NO_HASH)
    /* implemented in wolfcrypt/src/port/psa/psa_hash.c */

    void wc_Sha224Free(wc_Sha224* sha224)
    {
        if (sha224 == NULL)
            return;

#ifdef WOLFSSL_SMALL_STACK_CACHE
    if (sha224->W != NULL) {
        XFREE(sha224->W, NULL, DYNAMIC_TYPE_DIGEST);
        sha224->W = NULL;
    }
#endif

    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA224)
        wolfAsync_DevCtxFree(&sha224->asyncDev, WOLFSSL_ASYNC_MARKER_SHA224);
    #endif /* WOLFSSL_ASYNC_CRYPT */

    #ifdef WOLFSSL_PIC32MZ_HASH
        wc_Sha256Pic32Free(sha224);
    #endif
    #if defined(WOLFSSL_KCAPI_HASH)
        KcapiHashFree(&sha224->kcapi);
    #endif
    #if defined(WOLFSSL_RENESAS_RX64_HASH)
    if (sha224->msg != NULL) {
        XFREE(sha224->msg, sha224->heap, DYNAMIC_TYPE_TMP_BUFFER);
        sha224->msg = NULL;
    }
    #endif
    }
#endif /* WOLFSSL_SHA224 */
#endif /* !defined(WOLFSSL_HAVE_PSA) || defined(WOLFSSL_PSA_NO_HASH) */


int wc_InitSha256(wc_Sha256* sha256)
{
    int devId = INVALID_DEVID;

#ifdef WOLF_CRYPTO_CB
    devId = wc_CryptoCb_DefaultDevID();
#endif
    return wc_InitSha256_ex(sha256, NULL, devId);
}

#if !defined(WOLFSSL_HAVE_PSA) || defined(WOLFSSL_PSA_NO_HASH)
    /* implemented in wolfcrypt/src/port/psa/psa_hash.c */

void wc_Sha256Free(wc_Sha256* sha256)
{
    if (sha256 == NULL)
        return;

#ifdef WOLFSSL_SMALL_STACK_CACHE
    if (sha256->W != NULL) {
        XFREE(sha256->W, NULL, DYNAMIC_TYPE_DIGEST);
        sha256->W = NULL;
    }
#endif

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA256)
    wolfAsync_DevCtxFree(&sha256->asyncDev, WOLFSSL_ASYNC_MARKER_SHA256);
#endif /* WOLFSSL_ASYNC_CRYPT */
#ifdef WOLFSSL_PIC32MZ_HASH
    wc_Sha256Pic32Free(sha256);
#endif
#if defined(WOLFSSL_AFALG_HASH)
    if (sha256->alFd > 0) {
        close(sha256->alFd);
        sha256->alFd = -1; /* avoid possible double close on socket */
    }
    if (sha256->rdFd > 0) {
        close(sha256->rdFd);
        sha256->rdFd = -1; /* avoid possible double close on socket */
    }
#endif /* WOLFSSL_AFALG_HASH */
#ifdef WOLFSSL_DEVCRYPTO_HASH
    wc_DevCryptoFree(&sha256->ctx);
#endif /* WOLFSSL_DEVCRYPTO */
#if (defined(WOLFSSL_AFALG_HASH) && defined(WOLFSSL_AFALG_HASH_KEEP)) || \
    (defined(WOLFSSL_DEVCRYPTO_HASH) && defined(WOLFSSL_DEVCRYPTO_HASH_KEEP)) || \
    ((defined(WOLFSSL_RENESAS_TSIP_TLS) || \
      defined(WOLFSSL_RENESAS_TSIP_CRYPTONLY)) && \
    !defined(NO_WOLFSSL_RENESAS_TSIP_CRYPT_HASH)) || \
    ((defined(WOLFSSL_RENESAS_SCEPROTECT) || \
      defined(WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY)) && \
    !defined(NO_WOLFSSL_RENESAS_SCEPROTECT_HASH)) || \
    defined(WOLFSSL_RENESAS_RX64_HASH) || \
    defined(WOLFSSL_HASH_KEEP)

    if (sha256->msg != NULL) {
        XFREE(sha256->msg, sha256->heap, DYNAMIC_TYPE_TMP_BUFFER);
        sha256->msg = NULL;
    }
#endif
#if defined(WOLFSSL_SE050) && defined(WOLFSSL_SE050_HASH)
    se050_hash_free(&sha256->se050Ctx);
#endif
#if defined(WOLFSSL_KCAPI_HASH)
    KcapiHashFree(&sha256->kcapi);
#endif
#ifdef WOLFSSL_IMXRT_DCP
    DCPSha256Free(sha256);
#endif
#ifdef WOLFSSL_MAXQ10XX_CRYPTO
    wc_MAXQ10XX_Sha256Free(sha256);
#endif

#ifdef HAVE_ARIA
    if (sha256->hSession != NULL) {
        MC_CloseSession(sha256->hSession);
        sha256->hSession = NULL;
    }
#endif

/* Espressif embedded hardware acceleration specific: */
#if defined(WOLFSSL_USE_ESP32_CRYPT_HASH_HW)
    if (sha256->ctx.lockDepth > 0) {
        /* probably due to unclean shutdown, error, or other problem.
         *
         * if you find yourself here, code needs to be cleaned up to
         * properly release hardware. this init is only for handling
         * the unexpected. by the time free is called, the hardware
         * should have already been released (lockDepth = 0)
         */
        (void)InitSha256(sha256); /* unlock mutex, set mode to ESP32_SHA_INIT */
        ESP_LOGV(TAG, "Alert: hardware unlock needed in wc_Sha256Free.");
    }
    else {
        ESP_LOGV(TAG, "Hardware unlock not needed in wc_Sha256Free.");
    }
#endif
}

#endif /* !defined(WOLFSSL_HAVE_PSA) || defined(WOLFSSL_PSA_NO_HASH) */
#ifdef WOLFSSL_HASH_KEEP
/* Some hardware have issues with update, this function stores the data to be
 * hashed into an array. Once ready, the Final operation is called on all of the
 * data to be hashed at once.
 * returns 0 on success
 */
int wc_Sha256_Grow(wc_Sha256* sha256, const byte* in, int inSz)
{
    return _wc_Hash_Grow(&(sha256->msg), &(sha256->used), &(sha256->len), in,
                        inSz, sha256->heap);
}
#ifdef WOLFSSL_SHA224
int wc_Sha224_Grow(wc_Sha224* sha224, const byte* in, int inSz)
{
    return _wc_Hash_Grow(&(sha224->msg), &(sha224->used), &(sha224->len), in,
                        inSz, sha224->heap);
}
#endif /* WOLFSSL_SHA224 */
#endif /* WOLFSSL_HASH_KEEP */

#endif /* !WOLFSSL_TI_HASH */


#ifndef WOLFSSL_TI_HASH
#if !defined(WOLFSSL_RENESAS_RX64_HASH)
#ifdef WOLFSSL_SHA224

#if defined(WOLFSSL_KCAPI_HASH) && !defined(WOLFSSL_NO_KCAPI_SHA224)
    /* implemented in wolfcrypt/src/port/kcapi/kcapi_hash.c */
#elif defined(WOLFSSL_HAVE_PSA) && !defined(WOLFSSL_PSA_NO_HASH)
    /* implemented in wolfcrypt/src/port/psa/psa_hash.c */

#else

    int wc_Sha224GetHash(wc_Sha224* sha224, byte* hash)
    {
        int ret;
    #ifdef WOLFSSL_SMALL_STACK
        wc_Sha224* tmpSha224;
    #else
        wc_Sha224  tmpSha224[1];
    #endif

        if (sha224 == NULL || hash == NULL) {
            return BAD_FUNC_ARG;
        }

    #ifdef WOLFSSL_SMALL_STACK
        tmpSha224 = (wc_Sha224*)XMALLOC(sizeof(wc_Sha224), NULL,
            DYNAMIC_TYPE_TMP_BUFFER);
        if (tmpSha224 == NULL) {
            return MEMORY_E;
        }
    #endif

        ret = wc_Sha224Copy(sha224, tmpSha224);
        if (ret == 0) {
            ret = wc_Sha224Final(tmpSha224, hash);
            wc_Sha224Free(tmpSha224);
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(tmpSha224, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif
        return ret;
    }

    int wc_Sha224Copy(wc_Sha224* src, wc_Sha224* dst)
    {
        int ret = 0; /* assume success unless proven otherwise */

        if (src == NULL || dst == NULL) {
            return BAD_FUNC_ARG;
        }

        XMEMCPY(dst, src, sizeof(wc_Sha224));

    #ifdef WOLFSSL_SMALL_STACK_CACHE
        dst->W = NULL;
    #endif

    #if defined(WOLFSSL_SILABS_SE_ACCEL) && defined(WOLFSSL_SILABS_SE_ACCEL_3)
        dst->silabsCtx.hash_ctx.cmd_ctx = &dst->silabsCtx.cmd_ctx;
        dst->silabsCtx.hash_ctx.hash_type_ctx = &dst->silabsCtx.hash_type_ctx;
    #endif

    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA224)
        ret = wolfAsync_DevCopy(&src->asyncDev, &dst->asyncDev);
    #endif

    #if defined(WOLFSSL_USE_ESP32_CRYPT_HASH_HW)
        ret = esp_sha224_ctx_copy(src, dst);
    #endif

    #ifdef WOLFSSL_HASH_FLAGS
        dst->flags |= WC_HASH_FLAG_ISCOPY;
    #endif

    #if defined(WOLFSSL_HASH_KEEP)
        if (src->msg != NULL) {
            dst->msg = (byte*)XMALLOC(src->len, dst->heap,
                                      DYNAMIC_TYPE_TMP_BUFFER);
            if (dst->msg == NULL)
                return MEMORY_E;
            XMEMCPY(dst->msg, src->msg, src->len);
        }
    #endif

        return ret;
    }

#endif /* WOLFSSL_KCAPI_HASH && !WOLFSSL_NO_KCAPI_SHA224 */

#ifdef WOLFSSL_HASH_FLAGS
    int wc_Sha224SetFlags(wc_Sha224* sha224, word32 flags)
    {
        if (sha224) {
            sha224->flags = flags;
        }
        return 0;
    }
    int wc_Sha224GetFlags(wc_Sha224* sha224, word32* flags)
    {
        if (sha224 && flags) {
            *flags = sha224->flags;
        }
        return 0;
    }
#endif

#endif /* WOLFSSL_SHA224 */
#endif /* WOLFSSL_RENESAS_RX64_HASH */

#ifdef WOLFSSL_AFALG_HASH
    /* implemented in wolfcrypt/src/port/af_alg/afalg_hash.c */

#elif defined(WOLFSSL_DEVCRYPTO_HASH)
    /* implemented in wolfcrypt/src/port/devcrypto/devcrypt_hash.c */

#elif (defined(WOLFSSL_RENESAS_TSIP_TLS) || \
       defined(WOLFSSL_RENESAS_TSIP_CRYPTONLY)) && \
    !defined(NO_WOLFSSL_RENESAS_TSIP_CRYPT_HASH)

    /* implemented in wolfcrypt/src/port/Renesas/renesas_tsip_sha.c */

#elif (defined(WOLFSSL_RENESAS_SCEPROTECT) || \
       defined(WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY)) && \
    !defined(NO_WOLFSSL_RENESAS_SCEPROTECT_HASH)

    /* implemented in wolfcrypt/src/port/Renesas/renesas_sce_sha.c */

#elif defined(WOLFSSL_PSOC6_CRYPTO)
    /* implemented in wolfcrypt/src/port/cypress/psoc6_crypto.c */
#elif defined(WOLFSSL_IMXRT_DCP)
    /* implemented in wolfcrypt/src/port/nxp/dcp_port.c */
#elif defined(WOLFSSL_KCAPI_HASH)
    /* implemented in wolfcrypt/src/port/kcapi/kcapi_hash.c */

#elif defined(WOLFSSL_HAVE_PSA) && !defined(WOLFSSL_PSA_NO_HASH)
    /* implemented in wolfcrypt/src/port/psa/psa_hash.c */
#elif defined(WOLFSSL_RENESAS_RX64_HASH)
    /* implemented in wolfcrypt/src/port/Renesas/renesas_rx64_hw_sha.c */

#else

int wc_Sha256GetHash(wc_Sha256* sha256, byte* hash)
{
    int ret;
#ifdef WOLFSSL_SMALL_STACK
    wc_Sha256* tmpSha256;
#else
    wc_Sha256  tmpSha256[1];
#endif

    if (sha256 == NULL || hash == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_SMALL_STACK
    tmpSha256 = (wc_Sha256*)XMALLOC(sizeof(wc_Sha256), NULL,
        DYNAMIC_TYPE_TMP_BUFFER);
    if (tmpSha256 == NULL) {
        return MEMORY_E;
    }
#endif

    ret = wc_Sha256Copy(sha256, tmpSha256);
    if (ret == 0) {
        ret = wc_Sha256Final(tmpSha256, hash);
        wc_Sha256Free(tmpSha256); /* TODO move outside brackets? */
    }


#ifdef WOLFSSL_SMALL_STACK
    XFREE(tmpSha256, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}
int wc_Sha256Copy(wc_Sha256* src, wc_Sha256* dst)
{
    int ret = 0;

    if (src == NULL || dst == NULL) {
        return BAD_FUNC_ARG;
    }

    XMEMCPY(dst, src, sizeof(wc_Sha256));

#ifdef WOLFSSL_MAXQ10XX_CRYPTO
    wc_MAXQ10XX_Sha256Copy(src);
#endif

#ifdef WOLFSSL_SMALL_STACK_CACHE
    dst->W = NULL;
#endif

#if defined(WOLFSSL_SILABS_SE_ACCEL) && defined(WOLFSSL_SILABS_SE_ACCEL_3)
    dst->silabsCtx.hash_ctx.cmd_ctx = &dst->silabsCtx.cmd_ctx;
    dst->silabsCtx.hash_ctx.hash_type_ctx = &dst->silabsCtx.hash_type_ctx;
#endif

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA256)
    ret = wolfAsync_DevCopy(&src->asyncDev, &dst->asyncDev);
#endif

#ifdef WOLFSSL_PIC32MZ_HASH
    ret = wc_Pic32HashCopy(&src->cache, &dst->cache);
#endif

#if defined(WOLFSSL_USE_ESP32_CRYPT_HASH_HW)
    esp_sha256_ctx_copy(src, dst);
#endif

#ifdef HAVE_ARIA
    dst->hSession = NULL;
    if((src->hSession != NULL) && (MC_CopySession(src->hSession, &(dst->hSession)) != MC_OK)) {
        return MEMORY_E;
    }
#endif

#ifdef WOLFSSL_HASH_FLAGS
     dst->flags |= WC_HASH_FLAG_ISCOPY;
#endif

#if defined(WOLFSSL_HASH_KEEP)
    if (src->msg != NULL) {
        dst->msg = (byte*)XMALLOC(src->len, dst->heap, DYNAMIC_TYPE_TMP_BUFFER);
        if (dst->msg == NULL)
            return MEMORY_E;
        XMEMCPY(dst->msg, src->msg, src->len);
    }
#endif

    return ret;
}
#endif

#ifdef WOLFSSL_HASH_FLAGS
int wc_Sha256SetFlags(wc_Sha256* sha256, word32 flags)
{
    if (sha256) {
        sha256->flags = flags;
    }
    return 0;
}
int wc_Sha256GetFlags(wc_Sha256* sha256, word32* flags)
{
    if (sha256 && flags) {
        *flags = sha256->flags;
    }
    return 0;
}
#endif
#endif /* !WOLFSSL_TI_HASH */

#endif /* NO_SHA256 */
