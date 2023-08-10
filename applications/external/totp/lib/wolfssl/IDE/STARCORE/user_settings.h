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
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
*/

/* Custom wolfSSL user settings for Vortec Scheduler,
 *                                  VxWorks 6.9 and VxWorks 7.0  */

#ifndef WOLFSSL_USER_SETTINGS_H
#define WOLFSSL_USER_SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------- */
/* Platform */
/* ------------------------------------------------------------------------- */
#undef WOLFSSL_GENERAL_ALIGNMENT
#define WOLFSSL_GENERAL_ALIGNMENT 4

/* Platform */
#undef WOLFSSL_STARCORE
#define WOLFSSL_STARCORE

/* Endianness */
#undef BIG_ENDIAN_ORDER
#define BIG_ENDIAN_ORDER

#undef WOLFSSL_SMALL_STACK
#define WOLFSSL_SMALL_STACK

#undef WOLFSSL_USER_IO
/* #define WOLFSSL_USER_IO */

#undef SINGLE_THREADED
#define SINGLE_THREADED

/* ------------------------------------------------------------------------- */
/* Math Configuration */
/* ------------------------------------------------------------------------- */
#undef SIZEOF_LONG_LONG
#define SIZEOF_LONG_LONG 8

#undef SIZEOF_LONG
#define SIZEOF_LONG 4

#undef USE_FAST_MATH
#if 1
    #define USE_FAST_MATH

    #undef TFM_TIMING_RESISTANT
    #define TFM_TIMING_RESISTANT

    /* Optimizations (footprint) */
    #define WOLFCRYPT_ONLY
    #define TFM_NO_ASM
#endif

#if 0
    #define WOLFSSL_SP
    #define WOLFSSL_SP_SMALL
    #define WOLFSSL_HAVE_SP_RSA
    #define WOLFSSL_HAVE_SP_DH
    #define WOLFSSL_HAVE_SP_ECC
#endif

/* ------------------------------------------------------------------------- */
/* FIPS - Requires eval or license from wolfSSL */
/* ------------------------------------------------------------------------- */
#undef HAVE_FIPS
#if 1
    #define HAVE_FIPS

    #undef HAVE_FIPS_VERSION
    #define HAVE_FIPS_VERSION 2

    #ifdef SINGLE_THREADED
        #undef NO_THREAD_LS
        #define NO_THREAD_LS
    #endif

    #if 1
        #undef NO_ATTRIBUTE_CONSTRUCTOR
        #define NO_ATTRIBUTE_CONSTRUCTOR
    #endif
#endif


/* ------------------------------------------------------------------------- */
/* Crypto */
/* ------------------------------------------------------------------------- */
/* RSA */
#undef NO_RSA
#if 1
    #ifdef USE_FAST_MATH
        /* Maximum math bits (Max RSA key bits * 2) */
        #undef FP_MAX_BITS
        #define FP_MAX_BITS 8192
    #endif

    /* half as much memory but twice as slow */
    #undef RSA_LOW_MEM
    #define RSA_LOW_MEM

    /* Enables blinding mode, to prevent timing attacks */
    #if 0
        #undef WC_RSA_BLINDING
        #define WC_RSA_BLINDING
    #else
        #undef WC_NO_HARDEN
        #define WC_NO_HARDEN
    #endif

    /* RSA PSS Support */
    #if 1
        #define WC_RSA_PSS
    #endif

    #if 1
        #define WC_RSA_NO_PADDING
    #endif
#else
    #define NO_RSA
#endif

/* ECC */
#undef HAVE_ECC
#if 1
    #define HAVE_ECC

    /* Manually define enabled curves */
    #undef ECC_USER_CURVES
    #define ECC_USER_CURVES

    #ifdef ECC_USER_CURVES
    /* Manual Curve Selection */
        #define HAVE_ECC192
        #define HAVE_ECC224
        #undef NO_ECC256
        #define HAVE_ECC384
        #define HAVE_ECC521
    #endif

    /* Fixed point cache (speeds repeated operations against same private key) */
    #undef FP_ECC
    /* #define FP_ECC */

    #ifdef FP_ECC
        /* Bits / Entries */
        #undef FP_ENTRIES
        #define FP_ENTRIES 2
        #undef FP_LUT
        #define FP_LUT 4
    #endif

    /* Optional ECC calculation method */
    /* Note: doubles heap usage, but slightly faster */
    #undef ECC_SHAMIR
    /* #define ECC_SHAMIR */

    /* Reduces heap usage, but slower */
    #undef ECC_TIMING_RESISTANT
    #define ECC_TIMING_RESISTANT

    #ifdef HAVE_FIPS
        #undef HAVE_ECC_CDH
        #define HAVE_ECC_CDH /* Enable cofactor support */

        #undef NO_STRICT_ECDSA_LEN
        #define NO_STRICT_ECDSA_LEN /* Do not force fixed len w/ FIPS */

        #undef WOLFSSL_VALIDATE_ECC_IMPORT
        #define WOLFSSL_VALIDATE_ECC_IMPORT /* Validate import */
    #endif

    /* Compressed Key Support */
    #undef HAVE_COMP_KEY
    /* #define HAVE_COMP_KEY */

    /* Use alternate ECC size for ECC math */
    #ifdef USE_FAST_MATH
        /* MAX ECC BITS = ROUND8(MAX ECC) * 2 */
        #ifdef NO_RSA
            /* Custom fastmath size if not using RSA */
            #undef FP_MAX_BITS
            #define FP_MAX_BITS (256 * 2)
        #else
            #undef ALT_ECC_SIZE
            #define ALT_ECC_SIZE
            /* wolfSSL will compute the FP_MAX_BITS_ECC, but it can be overridden */
            /* #undef FP_MAX_BITS_ECC */
            /* #define FP_MAX_BITS_ECC (256 * 2) */
        #endif

        /* Speedups specific to curve */
        #ifndef NO_ECC256
            #undef TFM_ECC256
            #define TFM_ECC256
        #endif
    #endif
#endif

/* DH */
#undef NO_DH
#if 1
    /* Use table for DH instead of -lm (math) lib dependency */
    #if 1
        #define WOLFSSL_DH_CONST
        #define HAVE_FFDHE_2048
        #define HAVE_FFDHE_3072
        #define HAVE_FFDHE_4096
        /* #define HAVE_FFDHE_6144 */
        /* #define HAVE_FFDHE_8192 */
    #endif

    #ifdef HAVE_FIPS
        #define WOLFSSL_VALIDATE_FFC_IMPORT
        #define HAVE_FFDHE_Q
    #endif
#else
    #define NO_DH
#endif


/* AES */
#undef NO_AES
#if 1
    #undef HAVE_AES_CBC
    #define HAVE_AES_CBC

    #undef HAVE_AESGCM
    #define HAVE_AESGCM

    /* GCM Method: GCM_SMALL, GCM_WORD32 or GCM_TABLE */
    /*  #define GCM_SMALL */
    /*  #define GCM_WORD32 */
    #define GCM_TABLE

    #undef WOLFSSL_AES_DIRECT
    #define WOLFSSL_AES_DIRECT

    #undef HAVE_AES_ECB
    #define HAVE_AES_ECB

    #undef WOLFSSL_AES_COUNTER
    #define WOLFSSL_AES_COUNTER

    #undef HAVE_AESCCM
    #define HAVE_AESCCM
#else
    #define NO_AES
#endif


/* DES3 */
#undef NO_DES3
#if 1
    /* No change */
#else
    #define NO_DES3
#endif

/* ChaCha20 / Poly1305 */
#undef HAVE_CHACHA
#undef HAVE_POLY1305
#if 0
    #define HAVE_CHACHA
    #define HAVE_POLY1305

    /* Needed for Poly1305 */
    #undef HAVE_ONE_TIME_AUTH
    #define HAVE_ONE_TIME_AUTH
#endif

/* Ed25519 / Curve25519 */
#undef HAVE_CURVE25519
#undef HAVE_ED25519
#if 0
    #define HAVE_CURVE25519
    #define HAVE_ED25519 /* ED25519 Requires SHA512 */

    /* Optionally use small math (less flash usage, but much slower) */
    #if 1
        #define CURVED25519_SMALL
    #endif
#endif


/* ------------------------------------------------------------------------- */
/* Hashing */
/* ------------------------------------------------------------------------- */
/* Sha */
#undef NO_SHA
#if 1
    /* 1k smaller, but 25% slower */
    /* #define USE_SLOW_SHA */
#else
    #define NO_SHA
#endif

/* Sha256 */
#undef NO_SHA256
#if 1
    /* not unrolled - ~2k smaller and ~25% slower */
    /* #define USE_SLOW_SHA256 */

    /* Sha224 */
    #if 1
        #define WOLFSSL_SHA224
    #endif
#else
    #define NO_SHA256
#endif

/* Sha512 */
#undef WOLFSSL_SHA512
#if 1
    #define WOLFSSL_SHA512

    /* Sha384 */
    #undef WOLFSSL_SHA384
    #if 1
        #define WOLFSSL_SHA384
    #endif

    /* over twice as small, but 50% slower */
    /* #define USE_SLOW_SHA512 */
#endif

/* Sha3 */
#undef WOLFSSL_SHA3
#if 1
    #define WOLFSSL_SHA3
#endif

/* MD5 */
#undef NO_MD5
#if 1
    /* No change */
#else
    #define NO_MD5
#endif

/* HKDF */
#undef HAVE_HKDF
#if 1
    #define HAVE_HKDF
#endif

/* CMAC */
#undef WOLFSSL_CMAC
#if 1
    #define WOLFSSL_CMAC
#endif


/* ------------------------------------------------------------------------- */
/* Benchmark / Test */
/* ------------------------------------------------------------------------- */
/* Use reduced benchmark / test sizes */
#undef BENCH_EMBEDDED
#define BENCH_EMBEDDED

#undef USE_CERT_BUFFERS_2048
#define USE_CERT_BUFFERS_2048

#undef USE_CERT_BUFFERS_1024
/* #define USE_CERT_BUFFERS_1024 */

#undef USE_CERT_BUFFERS_256
#define USE_CERT_BUFFERS_256

#undef FORCE_BUFFER_TEST
#define FORCE_BUFFER_TEST


/* ------------------------------------------------------------------------- */
/* Debugging */
/* ------------------------------------------------------------------------- */

#undef DEBUG_WOLFSSL
#undef NO_ERROR_STRINGS
#if 0
    #define DEBUG_WOLFSSL
#else
    #if 1
        #define NO_ERROR_STRINGS
    #endif
#endif


/* ------------------------------------------------------------------------- */
/* Memory */
/* ------------------------------------------------------------------------- */

/* Override Memory API's */
#if 0
    #undef XMALLOC_OVERRIDE
    #define XMALLOC_OVERRIDE

    /* prototypes for user heap override functions */
    /* Note: Realloc only required for normal math */
    #include <stddef.h> /* for size_t */
    extern void *myMalloc(size_t n, void* heap, int type);
    extern void myFree(void *p, void* heap, int type);
    extern void *myRealloc(void *p, size_t n, void* heap, int type);

    #define XMALLOC(n, h, t) myMalloc(n, h, t)
    #define XFREE(p, h, t) myFree(p, h, t)
    #define XREALLOC(p, n, h, t) myRealloc(p, n, h, t)
#endif

#if 0
    /* Static memory requires fast math */
    #define WOLFSSL_STATIC_MEMORY

    /* Disable fallback malloc/free */
    #define WOLFSSL_NO_MALLOC
    #if 1
    #define WOLFSSL_MALLOC_CHECK /* trap malloc failure */
    #endif
#endif

/* Memory callbacks */
#if 1
    #undef USE_WOLFSSL_MEMORY
    #define USE_WOLFSSL_MEMORY

    /* Use this to measure / print heap usage */
    #if 0
        #undef WOLFSSL_TRACK_MEMORY
        /*  #define WOLFSSL_TRACK_MEMORY */

        #undef WOLFSSL_DEBUG_MEMORY
        /* #define WOLFSSL_DEBUG_MEMORY */

        #undef WOLFSSL_DEBUG_MEMORY_PRINT
        /* #define WOLFSSL_DEBUG_MEMORY_PRINT */
    #endif
#else
    #ifndef WOLFSSL_STATIC_MEMORY
        #define NO_WOLFSSL_MEMORY
        /* Otherwise we will use stdlib malloc, free and realloc */
    #endif
#endif

/* ------------------------------------------------------------------------- */
/* RNG */
/* ------------------------------------------------------------------------- */

/* Seed Source */
/* Seed Source */
#if 1
    extern int my_rng_generate_seed(unsigned char* output, int sz);
    #undef CUSTOM_RAND_GENERATE_SEED
    #define CUSTOM_RAND_GENERATE_SEED my_rng_generate_seed
#endif

/* NETOS */
#if 0
    extern unsigned char get_byte_from_pool(void);
    #define CUSTOM_RAND_GENERATE  get_byte_from_pool
    #define CUSTOM_RAND_TYPE      unsigned char
#endif

/* Choose RNG method */
#if 1
    /* Use built-in P-RNG (SHA256 based) with HW RNG */
    /* P-RNG + HW RNG (P-RNG is ~8K) */
    /* #define WOLFSSL_GENSEED_FORTEST */
    #undef HAVE_HASHDRBG
    #define HAVE_HASHDRBG
#else
    #undef WC_NO_HASHDRBG
    #define WC_NO_HASHDRBG

    /* Bypass P-RNG and use only HW RNG */
    extern int my_rng_gen_block(unsigned char* output, unsigned int sz);
    #undef CUSTOM_RAND_GENERATE_BLOCK
    #define CUSTOM_RAND_GENERATE_BLOCK my_rng_gen_block
#endif

/* ------------------------------------------------------------------------- */
/* Enable Features */
/* ------------------------------------------------------------------------- */
#undef WOLFSSL_TLS13
#if 0
    #define WOLFSSL_TLS13
#endif

#undef WOLFSSL_KEY_GEN
#if 1
    #define WOLFSSL_KEY_GEN
#endif

#if defined(HAVE_FIPS) && !defined(WOLFSSL_KEY_GEN)
    #define WOLFSSL_OLD_PRIME_CHECK
#endif

#undef KEEP_PEER_CERT
/* #define KEEP_PEER_CERT */

#undef HAVE_COMP_KEY
/* #define HAVE_COMP_KEY */

#undef HAVE_TLS_EXTENSIONS
/* #define HAVE_TLS_EXTENSIONS */

#undef HAVE_SUPPORTED_CURVES
/* #define HAVE_SUPPORTED_CURVES */

#undef WOLFSSL_BASE64_ENCODE
#define WOLFSSL_BASE64_ENCODE

/* TLS Session Cache */
#if 0
    #define SMALL_SESSION_CACHE
#else
    #define NO_SESSION_CACHE
#endif


/* ------------------------------------------------------------------------- */
/* Disable Features */
/* ------------------------------------------------------------------------- */
#undef NO_WOLFSSL_SERVER
/* #define NO_WOLFSSL_SERVER */

#undef NO_WOLFSSL_CLIENT
/* #define NO_WOLFSSL_CLIENT */

#undef NO_CRYPT_TEST
/* #define NO_CRYPT_TEST */

#undef NO_CRYPT_BENCHMARK
/* #define NO_CRYPT_BENCHMARK */

/* In-lining of misc.c functions */
/* If defined, must include wolfcrypt/src/misc.c in build */
/* Slower, but about 1k smaller */
#undef NO_INLINE
/* #define NO_INLINE */

#undef NO_FILESYSTEM
/* #define NO_FILESYSTEM */
    
#undef NO_WRITE_TEMP_FILES
#define NO_WRITE_TEMP_FILES

#undef NO_WOLFSSL_DIR
#define NO_WOLFSSL_DIR

#undef NO_WRITEV
#define NO_WRITEV
    
#undef WOLFSSL_NO_SOCK
#define WOLFSSL_NO_SOCK

#undef NO_MAIN_DRIVER
#define NO_MAIN_DRIVER

#undef NO_DEV_RANDOM
/* #define NO_DEV_RANDOM */

#undef NO_DSA
#define NO_DSA

#undef NO_RC4
#define NO_RC4

#undef NO_OLD_TLS
#define NO_OLD_TLS

#undef NO_PSK
#define NO_PSK

#undef NO_MD4
#define NO_MD4
    
#undef WOLFSSL_NO_SHAKE128
#define WOLFSSL_NO_SHAKE128
    
#undef WOLFSSL_NO_SHAKE256
#define WOLFSSL_NO_SHAKE256

#undef NO_PWDBASED
/* #define NO_PWDBASED */

#undef NO_CODING
/* #define NO_CODING */

#undef NO_ASN_TIME
/* #define NO_ASN_TIME */

#undef NO_CERTS
/* #define NO_CERTS */

#undef NO_SIG_WRAPPER
/* #define NO_SIG_WRAPPER */

/* ACVP Testing ONLY specific settings */
#if 0
    #define ACVP_NO_DIR_SUPPORT
    /* #define DEBUG_ACVP_PRINTF */

    #undef USE_UART_READ_LINE
    /* #define USE_UART_READ_LINE */

    #undef USE_SMALL_MONTE
    /* #define USE_SMALL_MONTE */

    #undef WOLFSSL_PUBLIC_MP
    /* #define WOLFSSL_PUBLIC_MP */

    #undef HAVE_FORCE_FIPS_FAILURE
    #define HAVE_FORCE_FIPS_FAILURE
#endif

#ifdef __cplusplus
}
#endif

#endif /* WOLFSSL_USER_SETTINGS_H */
