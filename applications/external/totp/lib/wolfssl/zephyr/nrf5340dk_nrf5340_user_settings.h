/* nrf5340dk_nrf5340_user_settings.h
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

#ifndef WOLFSSL_OPTIONS_H
#define WOLFSSL_OPTIONS_H


#ifdef __cplusplus
extern "C" {
#endif

/* Platform */
#undef  WOLFSSL_ZEPHYR
#define WOLFSSL_ZEPHYR

#define WOLFSSL_GENERAL_ALIGNMENT 4
#define SIZEOF_LONG_LONG 8

/* Enable PSA Crypto API for CryptoCell 312 crypto use */
#define WOLFSSL_HAVE_PSA
#define WOLFSSL_PSA_GLOBAL_LOCK

/* Enable SP Math */
#define WOLFSSL_SP_MATH
#define WOLFSSL_SP_MATH_ALL
#define WOLFSSL_HAVE_SP_RSA
#define WOLFSSL_HAVE_SP_DH
#define WOLFSSL_HAVE_SP_ECC

/* Enable SP Math assembly support for ARM32 */
#define SP_WORD_SIZE 32
#define WOLFSSL_SP_ASM
#define WOLFSSL_SP_ARM32
#define WOLFSSL_SP_ARM32_ASM

/* Crypto */
#define WC_RSA_BLINDING
#define WC_RSA_PSS
#define WOLFSSL_DH_CONST
#define HAVE_FFDHE_2048

#define HAVE_ECC
#define ECC_USER_CURVES
/* #define HAVE_ECC192 */
/* #define HAVE_ECC224 */
#undef NO_ECC256
/* #define HAVE_ECC384 */
/* #define HAVE_ECC521 */
#define ECC_SHAMIR
#define ECC_TIMING_RESISTANT

#define WOLFSSL_AES_DIRECT
#define HAVE_AES_ECB
#define HAVE_AES_CBC
#define HAVE_AESCCM
#define HAVE_AESGCM
#define GCM_TABLE_4BIT

/* AES-CTR is not working correctly with Nordic PSA Crypto API */
/* #define WOLFSSL_AES_COUNTER */

#define HAVE_CHACHA
#define HAVE_POLY1305
#define HAVE_ONE_TIME_AUTH

/* Nordic Security PSA Crypto CryptoCell integration does not support SHA-1 */
#define NO_SHA
#define WOLFSSL_SHA224
#define WOLFSSL_SHA384
#define WOLFSSL_SHA512
#define WOLFSSL_SHA3

#define HAVE_HKDF
#define WOLFSSL_CMAC

/* Benchmark / Test */
#define BENCH_EMBEDDED
#define USE_CERT_BUFFERS_256
#define USE_CERT_BUFFERS_2048
#define NO_FILESYSTEM

/* RNG */
#define HAVE_HASHDRBG

/* Features */
#define WOLFSSL_TLS13
#define WOLFSSL_OLD_PRIME_CHECK
#define HAVE_TLS_EXTENSIONS
#define HAVE_SUPPORTED_CURVES
#define HAVE_EXTENDED_MASTER
#define WOLFSSL_BASE64_ENCODE
#define WC_NO_ASYNC_THREADING

/* Disable features that require SHA-1 (see note above) */
#define NO_OLD_TLS
#define NO_DSA

/* Disable other features (re-enable if needed) */
#define NO_RC4
#define NO_PSK
#define NO_MD4
#define NO_PWDBASED
#define NO_DES3

#if defined(CONFIG_WOLFSSL_DEBUG)
#undef  DEBUG_WOLFSSL
#define DEBUG_WOLFSSL
#endif

#ifdef __cplusplus
}
#endif

#endif /* WOLFSSL_OPTIONS_H */

