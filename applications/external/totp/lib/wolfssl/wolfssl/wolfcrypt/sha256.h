/* sha256.h
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
    \file wolfssl/wolfcrypt/sha256.h
*/



#ifndef WOLF_CRYPT_SHA256_H
#define WOLF_CRYPT_SHA256_H

#include <wolfssl/wolfcrypt/types.h>

#ifndef NO_SHA256

#if defined(HAVE_FIPS) && \
    defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2)
    #include <wolfssl/wolfcrypt/fips.h>
#endif /* HAVE_FIPS_VERSION >= 2 */

#ifdef FREESCALE_LTC_SHA
    #include "fsl_ltc.h"
#endif

#if defined(WOLFSSL_IMXRT1170_CAAM)
    #include "fsl_device_registers.h"
    #include "fsl_caam.h"
#endif

#ifdef WOLFSSL_IMXRT_DCP
    #include "fsl_dcp.h"
#endif

#if defined(WOLFSSL_PSOC6_CRYPTO)
#include "cy_crypto_core_sha.h"
#include "cy_device_headers.h"
#include "cy_crypto_common.h"
#include "cy_crypto_core.h"
#endif

#ifdef __cplusplus
    extern "C" {
#endif

/* avoid redefinition of structs */
#if !defined(HAVE_FIPS) || \
    (defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2))

#ifdef WOLFSSL_MICROCHIP_PIC32MZ
    #include <wolfssl/wolfcrypt/port/pic32/pic32mz-crypt.h>
#endif
#ifdef STM32_HASH
    #include <wolfssl/wolfcrypt/port/st/stm32.h>
#endif
#ifdef WOLFSSL_ASYNC_CRYPT
    #include <wolfssl/wolfcrypt/async.h>
#endif
#if defined(WOLFSSL_DEVCRYPTO) && defined(WOLFSSL_DEVCRYPTO_HASH)
    #include <wolfssl/wolfcrypt/port/devcrypto/wc_devcrypto.h>
#endif
#if defined(WOLFSSL_ESP32_CRYPT)
    #include "wolfssl/wolfcrypt/port/Espressif/esp32-crypt.h"
#endif
#if defined(WOLFSSL_CRYPTOCELL)
    #include <wolfssl/wolfcrypt/port/arm/cryptoCell.h>
#endif
#if defined(WOLFSSL_SILABS_SE_ACCEL)
    #include <wolfssl/wolfcrypt/port/silabs/silabs_hash.h>
#endif
#if defined(WOLFSSL_KCAPI_HASH)
    #include "wolfssl/wolfcrypt/port/kcapi/kcapi_hash.h"
#endif

#if defined(WOLFSSL_HAVE_PSA) && !defined(WOLFSSL_PSA_NO_HASH)
#include <psa/crypto.h>
#undef  WOLFSSL_NO_HASH_RAW
#define WOLFSSL_NO_HASH_RAW
#endif

#if defined(_MSC_VER)
    #define SHA256_NOINLINE __declspec(noinline)
#elif defined(__IAR_SYSTEMS_ICC__) || defined(__GNUC__)
    #define SHA256_NOINLINE __attribute__((noinline))
#else
    #define SHA256_NOINLINE
#endif

#if !defined(NO_OLD_SHA_NAMES)
    #define SHA256             WC_SHA256
#endif

#ifndef NO_OLD_WC_NAMES
    #define Sha256             wc_Sha256
    #define SHA256_BLOCK_SIZE  WC_SHA256_BLOCK_SIZE
    #define SHA256_DIGEST_SIZE WC_SHA256_DIGEST_SIZE
    #define SHA256_PAD_SIZE    WC_SHA256_PAD_SIZE
#endif

/* in bytes */
enum {
    WC_SHA256              =  WC_HASH_TYPE_SHA256,
    WC_SHA256_BLOCK_SIZE   = 64,
    WC_SHA256_DIGEST_SIZE  = 32,
    WC_SHA256_PAD_SIZE     = 56
};


#ifdef WOLFSSL_TI_HASH
    #include "wolfssl/wolfcrypt/port/ti/ti-hash.h"
#elif defined(WOLFSSL_IMX6_CAAM) && !defined(WOLFSSL_QNX_CAAM)
    #include "wolfssl/wolfcrypt/port/caam/wolfcaam_sha.h"
#elif defined(WOLFSSL_AFALG_HASH)
    #include "wolfssl/wolfcrypt/port/af_alg/afalg_hash.h"
#elif (defined(WOLFSSL_RENESAS_TSIP_TLS) || \
       defined(WOLFSSL_RENESAS_TSIP_CRYPTONLY)) && \
   !defined(NO_WOLFSSL_RENESAS_TSIP_CRYPT_HASH)
    #include "wolfssl/wolfcrypt/port/Renesas/renesas_tsip_types.h"
#elif (defined(WOLFSSL_RENESAS_SCEPROTECT) || \
        defined(WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY)) && \
   !defined(NO_WOLFSSL_RENESAS_SCEPROTECT_HASH)
    #include "wolfssl/wolfcrypt/port/Renesas/renesas-sce-crypt.h"
#elif defined(WOLFSSL_RENESAS_RX64_HASH)
    #include "wolfssl/wolfcrypt/port/Renesas/renesas-rx64-hw-crypt.h"
#else

#if defined(WOLFSSL_SE050) && defined(WOLFSSL_SE050_HASH)
    #include "wolfssl/wolfcrypt/port/nxp/se050_port.h"
#endif

#ifdef WOLFSSL_MAXQ10XX_CRYPTO
    #include <wolfssl/wolfcrypt/port/maxim/maxq10xx.h>
#endif

#ifdef HAVE_ARIA
    #include "mcapi.h"
    #include "mcapi_error.h"
#endif

/* wc_Sha256 digest */
struct wc_Sha256 {
#ifdef FREESCALE_LTC_SHA
    ltc_hash_ctx_t ctx;
#elif defined(WOLFSSL_SE050) && defined(WOLFSSL_SE050_HASH)
    SE050_HASH_Context se050Ctx;
#elif defined(STM32_HASH_SHA2)
    STM32_HASH_Context stmCtx;
#elif defined(WOLFSSL_SILABS_SE_ACCEL)
  wc_silabs_sha_t silabsCtx;
#elif defined(WOLFSSL_IMXRT_DCP)
    dcp_handle_t handle;
    dcp_hash_ctx_t ctx;
#elif defined(WOLFSSL_PSOC6_CRYPTO)
    cy_stc_crypto_sha_state_t hash_state;
    cy_en_crypto_sha_mode_t sha_mode;
    cy_stc_crypto_v2_sha256_buffers_t sha_buffers;
#elif defined(WOLFSSL_HAVE_PSA) && !defined(WOLFSSL_PSA_NO_HASH)
    psa_hash_operation_t psa_ctx;
#else
    /* alignment on digest and buffer speeds up ARMv8 crypto operations */
    ALIGN16 word32  digest[WC_SHA256_DIGEST_SIZE / sizeof(word32)];
    ALIGN16 word32  buffer[WC_SHA256_BLOCK_SIZE  / sizeof(word32)];
    word32  buffLen;   /* in bytes          */
    word32  loLen;     /* length in bytes   */
    word32  hiLen;     /* length in bytes   */
    void*   heap;
#endif
#ifdef WOLFSSL_PIC32MZ_HASH
    hashUpdCache cache; /* cache for updates */
#endif
#ifdef WOLFSSL_ASYNC_CRYPT
    WC_ASYNC_DEV asyncDev;
#endif /* WOLFSSL_ASYNC_CRYPT */
#ifdef WOLFSSL_SMALL_STACK_CACHE
    word32* W;
#endif /* !FREESCALE_LTC_SHA && !STM32_HASH_SHA2 */
#ifdef WOLFSSL_DEVCRYPTO_HASH
    WC_CRYPTODEV ctx;
#endif
#if defined(WOLFSSL_DEVCRYPTO_HASH) || defined(WOLFSSL_HASH_KEEP)
    byte*  msg;
    word32 used;
    word32 len;
#endif
#if defined(WOLFSSL_ESP32_CRYPT) && \
   !defined(NO_WOLFSSL_ESP32_CRYPT_HASH)
    WC_ESP32SHA ctx;
#endif
#ifdef WOLFSSL_MAXQ10XX_CRYPTO
    maxq_sha256_t maxq_ctx;
#endif
#ifdef WOLFSSL_CRYPTOCELL
    CRYS_HASHUserContext_t ctx;
#endif
#ifdef WOLFSSL_KCAPI_HASH
    wolfssl_KCAPI_Hash kcapi;
#endif
#ifdef WOLF_CRYPTO_CB
    int    devId;
    void*  devCtx; /* generic crypto callback context */
#endif
#ifdef WOLFSSL_IMXRT1170_CAAM
    caam_hash_ctx_t ctx;
    caam_handle_t hndl;
#endif
#ifdef HAVE_ARIA
    MC_HSESSION hSession;
#endif
#ifdef WOLFSSL_HASH_FLAGS
    word32 flags; /* enum wc_HashFlags in hash.h */
#endif
};

#ifndef WC_SHA256_TYPE_DEFINED
    typedef struct wc_Sha256 wc_Sha256;
    #define WC_SHA256_TYPE_DEFINED
#endif

#endif

#endif /* HAVE_FIPS */

WOLFSSL_API int wc_InitSha256(wc_Sha256* sha);
WOLFSSL_API int wc_InitSha256_ex(wc_Sha256* sha, void* heap, int devId);
WOLFSSL_API int wc_Sha256Update(wc_Sha256* sha, const byte* data, word32 len);
WOLFSSL_API int wc_Sha256FinalRaw(wc_Sha256* sha256, byte* hash);
WOLFSSL_API int wc_Sha256Final(wc_Sha256* sha256, byte* hash);
WOLFSSL_API void wc_Sha256Free(wc_Sha256* sha256);
#if defined(OPENSSL_EXTRA) || defined(HAVE_CURL)
WOLFSSL_API int wc_Sha256Transform(wc_Sha256* sha, const unsigned char* data);
#endif
#if defined(WOLFSSL_HASH_KEEP)
WOLFSSL_API int wc_Sha256_Grow(wc_Sha256* sha256, const byte* in, int inSz);
#endif
WOLFSSL_API int wc_Sha256GetHash(wc_Sha256* sha256, byte* hash);
WOLFSSL_API int wc_Sha256Copy(wc_Sha256* src, wc_Sha256* dst);

#ifdef WOLFSSL_PIC32MZ_HASH
WOLFSSL_API void wc_Sha256SizeSet(wc_Sha256* sha256, word32 len);
#endif

#ifdef WOLFSSL_HASH_FLAGS
    WOLFSSL_API int wc_Sha256SetFlags(wc_Sha256* sha256, word32 flags);
    WOLFSSL_API int wc_Sha256GetFlags(wc_Sha256* sha256, word32* flags);
#endif

#ifdef WOLFSSL_SHA224
/* avoid redefinition of structs */
#if !defined(HAVE_FIPS) || \
    (defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2))

#if !defined(NO_OLD_SHA_NAMES)
    #define SHA224             WC_SHA224
#endif

#ifndef NO_OLD_WC_NAMES
    #define Sha224             wc_Sha224
    #define SHA224_BLOCK_SIZE  WC_SHA224_BLOCK_SIZE
    #define SHA224_DIGEST_SIZE WC_SHA224_DIGEST_SIZE
    #define SHA224_PAD_SIZE    WC_SHA224_PAD_SIZE
#endif

/* in bytes */
enum {
    WC_SHA224              =   WC_HASH_TYPE_SHA224,
    WC_SHA224_BLOCK_SIZE   =   WC_SHA256_BLOCK_SIZE,
    WC_SHA224_DIGEST_SIZE  =   28,
    WC_SHA224_PAD_SIZE     =   WC_SHA256_PAD_SIZE
};


#ifndef WC_SHA224_TYPE_DEFINED
    typedef struct wc_Sha256 wc_Sha224;
    #define WC_SHA224_TYPE_DEFINED
#endif
#endif /* HAVE_FIPS */

WOLFSSL_API int wc_InitSha224(wc_Sha224* sha224);
WOLFSSL_API int wc_InitSha224_ex(wc_Sha224* sha224, void* heap, int devId);
WOLFSSL_API int wc_Sha224Update(wc_Sha224* sha224, const byte* data, word32 len);
WOLFSSL_API int wc_Sha224Final(wc_Sha224* sha224, byte* hash);
WOLFSSL_API void wc_Sha224Free(wc_Sha224* sha224);

#if defined(WOLFSSL_HASH_KEEP)
WOLFSSL_API int wc_Sha224_Grow(wc_Sha224* sha224, const byte* in, int inSz);
#endif
WOLFSSL_API int wc_Sha224GetHash(wc_Sha224* sha224, byte* hash);
WOLFSSL_API int wc_Sha224Copy(wc_Sha224* src, wc_Sha224* dst);

#ifdef WOLFSSL_HASH_FLAGS
    WOLFSSL_API int wc_Sha224SetFlags(wc_Sha224* sha224, word32 flags);
    WOLFSSL_API int wc_Sha224GetFlags(wc_Sha224* sha224, word32* flags);
#endif

#endif /* WOLFSSL_SHA224 */

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* NO_SHA256 */
#endif /* WOLF_CRYPT_SHA256_H */
