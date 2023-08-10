/* user_settings.h
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
 * user_settings.h
 *
 *  Created on: Mar 20, 2020
 *  Generated using:
 * ./configure --enable-cryptonly --enable-armasm --enable-ecc --enable-aesgcm --enable-pwdbased --enable-sp --enable-sp-asm \
 *     --disable-dh --disable-sha --disable-md5 --disable-sha224 --disable-aescbc --disable-shake256
 *  Result: wolfssl/options.h
 */

#ifndef SRC_USER_SETTINGS_H_
#define SRC_USER_SETTINGS_H_

#include <xparameters.h>

/* Disable all TLS support, only wolfCrypt features */
#define WOLFCRYPT_ONLY

/* Xilinx SDK */
#define WOLFSSL_XILINX
#define WOLFSSL_XILINX_CRYPT
#if defined(versal) && defined(WOLFSSL_XILINX_CRYPT)
#define WOLFSSL_XILINX_CRYPT_VERSAL
#endif
#ifndef FREERTOS
#define SINGLE_THREADED
#endif
#define NO_FILESYSTEM
#define HAVE_UINTPTR_T

/* Platform - remap printf */
#include "xil_printf.h"
#define XPRINTF xil_printf

#if !defined(WOLFSSL_XILINX_CRYPT_VERSAL)
/* Enable ARMv8 (Aarch64) assembly speedups - SHA256 / AESGCM */
/* Note: Requires CFLAGS="-mcpu=generic+crypto -mstrict-align" */
#define WOLFSSL_ARMASM
#endif

/* Math */
#define USE_FAST_MATH
#define FP_MAX_BITS (4096 * 2) /* Max RSA 4096-bit */

/* Use Single Precision assembly math speedups for ECC */
#define WOLFSSL_SP
#define WOLFSSL_SP_ASM
#define WOLFSSL_SP_ARM64_ASM
#define WOLFSSL_HAVE_SP_ECC
#define WOLFSSL_HAVE_SP_RSA

#if defined(WOLFSSL_XILINX_CRYPT_VERSAL)
/* Random: HashDRGB / P-RNG (SHA256) */
/* Versal supports both, with and w/o software-based HashDRGB.
 * It's your choice */
#define HAVE_HASHDRBG

#define WOLFSSL_PSV_TRNG_DEV_ID XPAR_VERSAL_CIPS_0_PSPMC_0_PSV_PMC_TRNG_DEVICE_ID
#if !defined(HAVE_HASHDRBG)
#define CUSTOM_RAND_GENERATE_BLOCK wc_VersalTrngGenerate
#else
#define CUSTOM_RAND_GENERATE_SEED wc_VersalTrngGenerate
#endif
#else
/* If we don't use the Versal Crypto we need the HASHDRBG */
#define HAVE_HASHDRBG
extern unsigned char my_rng_seed_gen(void);
#define CUSTOM_RAND_GENERATE  my_rng_seed_gen
#endif

/* Timing Resistance */
#define TFM_TIMING_RESISTANT
#define ECC_TIMING_RESISTANT
#define WC_RSA_BLINDING
//#define WC_XIL_AESGCM_DPA_CM

/* ECC */
#define HAVE_ECC
#if defined(WOLFSSL_XILINX_CRYPT_VERSAL)
#define ECC_USER_CURVES
#define NO_ECC256
#define WOLFSSL_SP_NO_256
#define HAVE_ECC384
#define WOLFSSL_SP_384
#define HAVE_ECC521
#define WOLFSSL_SP_521
/* Versal also supports usage of RFC6979 if you want to */
//#define WOLFSSL_ECDSA_DETERMINISTIC_K
#else
#define TFM_ECC256
#define ECC_SHAMIR
#endif

/* DH */
#undef NO_DH
#define WOLFSSL_DH_CONST
#define HAVE_FFDHE_2048
#define HAVE_FFDHE_4096

/* Curve25519 / Ed25519 */
#define HAVE_CURVE25519
#define HAVE_ED25519 /* ED25519 Requires SHA512 */
/* 25519 assumes UINT128_T is available for Aarch64 */
#ifndef HAVE___UINT128_T
#define HAVE___UINT128_T
#endif

/* ChaCha20 / Poly1305 */
#define HAVE_CHACHA
#define HAVE_POLY1305

/* AES-CBC is enabled by default if not disabled
 * The others have to be enabled */
#define HAVE_AESCCM
#define HAVE_AESGCM
#define WOLFSSL_AES_DIRECT
#define WOLFSSL_CMAC

/* Hashing */
#define WOLFSSL_SHA512
#define WOLFSSL_SHA384
#define WOLFSSL_SHA224
#define WOLFSSL_SHA3
#define WOLFSSL_NO_HASH_RAW /* not supported with ARMASM */
#define WOLFSSL_NO_SHAKE128
#define WOLFSSL_NO_SHAKE256

/* HKDF */
#define HAVE_HKDF

/* Disable Algorithms */
#define NO_DSA
#define NO_RC4
#define NO_MD4
#define NO_MD5
#define NO_SHA
#define NO_PSK
#define NO_DES3

/* Other */
#define WOLFSSL_IGNORE_FILE_WARN /* Ignore file include warnings */
#define NO_MAIN_FUNCTION /* User supplied "main" entry point */
//#define BENCH_EMBEDDED /* Use smaller buffers for benchmarking */
#define WOLFSSL_KEY_GEN
/* Pre-define AAD size to 16 for benchmarks. When executing the benchmarks we
 * also use a custom size of 13 to measure "the default size". */
#define AES_AUTH_ADD_SZ 16

/* Test with "wolfssl/certs_test.h" buffers - no file system */
#define USE_CERT_BUFFERS_256
#define USE_CERT_BUFFERS_2048

/* Debugging */
#if 0
    #define DEBUG_WOLFSSL
#endif

#endif /* SRC_USER_SETTINGS_H_ */
