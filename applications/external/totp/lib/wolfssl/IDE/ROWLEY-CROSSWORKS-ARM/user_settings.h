/* Example custom user settings for wolfSSL */

#ifndef WOLFSSL_USER_SETTINGS_H
#define WOLFSSL_USER_SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h> /* for size_t */

/* ------------------------------------------------------------------------- */
/* Platform */
/* ------------------------------------------------------------------------- */
#undef  WOLFSSL_GENERAL_ALIGNMENT
#define WOLFSSL_GENERAL_ALIGNMENT   4

#undef  SINGLE_THREADED
#define SINGLE_THREADED

#undef  WOLFSSL_SMALL_STACK
//#define WOLFSSL_SMALL_STACK

#undef  WOLFSSL_SMALL_STACK_CACHE
//#define WOLFSSL_SMALL_STACK_CACHE

#define WOLFSSL_IGNORE_FILE_WARN

/* ------------------------------------------------------------------------- */
/* Math Configuration */
/* ------------------------------------------------------------------------- */
#undef  USE_FAST_MATH
#define USE_FAST_MATH

#ifdef USE_FAST_MATH
    #undef  TFM_TIMING_RESISTANT
    #define TFM_TIMING_RESISTANT

    /* Optimizations (TFM_ARM, TFM_ASM or none) */
    #define TFM_ARM
    //#define TFM_ASM
#endif

/* Math debugging (adds support for mp_dump) */
#undef  WOLFSSL_DEBUG_MATH
//#define WOLFSSL_DEBUG_MATH

/* Wolf Single Precision Math */
#undef WOLFSSL_SP
#if 0
    #define WOLFSSL_SP
    #define WOLFSSL_SP_SMALL      /* use smaller version of code */
    #define WOLFSSL_HAVE_SP_RSA
    #define WOLFSSL_HAVE_SP_DH
    #define WOLFSSL_HAVE_SP_ECC
    //#define WOLFSSL_SP_CACHE_RESISTANT
    #define WOLFSSL_SP_MATH     /* only SP math - eliminates fast math code */

    /* SP Assembly Speedups */
    #define WOLFSSL_SP_ASM      /* required if using the ASM versions */
    //#define WOLFSSL_SP_ARM32_ASM
    //#define WOLFSSL_SP_ARM64_ASM
    //#define WOLFSSL_SP_ARM_THUMB_ASM
    #define WOLFSSL_SP_ARM_CORTEX_M_ASM
#endif


/* ------------------------------------------------------------------------- */
/* Crypto */
/* ------------------------------------------------------------------------- */
/* ECC */
#if 1
    #undef  HAVE_ECC
    #define HAVE_ECC

    /* Manually define enabled curves */
    #undef  ECC_USER_CURVES
    #define ECC_USER_CURVES

    #define HAVE_ECC192
    #define HAVE_ECC224
    #undef NO_ECC256
    #define HAVE_ECC384
    #ifndef USE_NXP_LTC /* NXP LTC HW supports up to 384 */
        #define HAVE_ECC521
    #endif

    /* Fixed point cache (speeds repeated operations against same private key) */
    #undef  FP_ECC
    //#define FP_ECC
    #ifdef FP_ECC
        /* Bits / Entries */
        #undef  FP_ENTRIES
        #define FP_ENTRIES  2
        #undef  FP_LUT
        #define FP_LUT      4
    #endif

    /* Optional ECC calculation method */
    /* Note: doubles heap usage, but slightly faster */
    #undef  ECC_SHAMIR
    #ifndef USE_NXP_LTC /* Don't enable Shamir code for HW ECC */
        #define ECC_SHAMIR
    #endif

    /* Reduces heap usage, but slower */
    #undef  ECC_TIMING_RESISTANT
    #define ECC_TIMING_RESISTANT

    #ifdef USE_FAST_MATH
        /* use reduced size math buffers for ecc points */
        #undef  ALT_ECC_SIZE
        #define ALT_ECC_SIZE

        /* Enable TFM optimizations for ECC */
        #if defined(HAVE_ECC192) || defined(HAVE_ALL_CURVES)
            #define TFM_ECC192
        #endif
        #if defined(HAVE_ECC224) || defined(HAVE_ALL_CURVES)
            #define TFM_ECC224
        #endif
        #if !defined(NO_ECC256) || defined(HAVE_ALL_CURVES)
            #define TFM_ECC256
        #endif
        #if defined(HAVE_ECC384) || defined(HAVE_ALL_CURVES)
            #define TFM_ECC384
        #endif
        #if defined(HAVE_ECC521) || defined(HAVE_ALL_CURVES)
            #define TFM_ECC521
        #endif
    #endif
#endif

/* RSA */
#undef NO_RSA
#if 1
    #ifdef USE_FAST_MATH
        /* Maximum math bits (Max RSA key bits * 2) */
        #undef  FP_MAX_BITS
        #define FP_MAX_BITS     4096
    #endif

    /* half as much memory but twice as slow */
    #undef  RSA_LOW_MEM
    //#define RSA_LOW_MEM

    /* Enables blinding mode, to prevent timing attacks */
    #undef  WC_RSA_BLINDING
    #define WC_RSA_BLINDING

#else
    #define NO_RSA
#endif

/* AES */
#undef NO_AES
#if 1
    #undef  HAVE_AESGCM
    #define HAVE_AESGCM

    /* GCM Method: GCM_SMALL, GCM_WORD32 or GCM_TABLE */
    #undef  GCM_SMALL
    #define GCM_SMALL
#else
    #define NO_AES
#endif

/* ChaCha20 / Poly1305 */
#undef HAVE_CHACHA
#undef HAVE_POLY1305
#if 1
    #define HAVE_CHACHA
    #define HAVE_POLY1305

    /* Needed for Poly1305 */
    #undef  HAVE_ONE_TIME_AUTH
    #define HAVE_ONE_TIME_AUTH
#endif

/* Ed25519 / Curve25519 */
#undef HAVE_CURVE25519
#undef HAVE_ED25519
#if 1
    #define HAVE_CURVE25519
    #define HAVE_ED25519

    /* Optionally use small math (less flash usage, but much slower) */
    #if 0
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
    //#define USE_SLOW_SHA
#else
    #define NO_SHA
#endif

/* Sha256 */
#undef NO_SHA256
#if 1
#else
    #define NO_SHA256
#endif

/* Sha512 */
#undef WOLFSSL_SHA512
#if 1
    #define WOLFSSL_SHA512

    /* Sha384 */
    #undef  WOLFSSL_SHA384
    #if 1
        #define WOLFSSL_SHA384
    #endif

    /* over twice as small, but 50% slower */
    //#define USE_SLOW_SHA2
#endif

/* MD5 */
#undef  NO_MD5
#if 1
#else
    #define NO_MD5
#endif


/* ------------------------------------------------------------------------- */
/* HW Crypto Acceleration */
/* ------------------------------------------------------------------------- */
#define FSL_HW_CRYPTO_MANUAL_SELECTION
#if 1
    /* NXP MMCAU / LTC Support (See README.md for instructions) */
    #if defined(USE_NXP_MMCAU) || defined(USE_NXP_LTC)
        #ifdef USE_NXP_MMCAU
            #define FREESCALE_USE_MMCAU
        #endif
        #ifdef USE_NXP_LTC
            #define FREESCALE_USE_LTC
            #define LTC_MAX_ECC_BITS    (384)
            #define LTC_MAX_INT_BYTES   (256)

            //#define FREESCALE_LTC_TFM_RSA_4096_ENABLE
        #endif
    #endif
#endif

/* ------------------------------------------------------------------------- */
/* Benchmark / Test */
/* ------------------------------------------------------------------------- */
/* Use reduced benchmark / test sizes */
#undef  BENCH_EMBEDDED
#define BENCH_EMBEDDED

#undef  USE_CERT_BUFFERS_2048
#define USE_CERT_BUFFERS_2048

#undef  USE_CERT_BUFFERS_256
#define USE_CERT_BUFFERS_256


/* ------------------------------------------------------------------------- */
/* Debugging */
/* ------------------------------------------------------------------------- */
#undef  DEBUG_WOLFSSL
//#define DEBUG_WOLFSSL

#ifdef DEBUG_WOLFSSL
    #define fprintf(file, format, ...)   printf(format, ##__VA_ARGS__)

    /* Use this to measure / print heap usage */
    #if 0
        #undef  USE_WOLFSSL_MEMORY
        #define USE_WOLFSSL_MEMORY

        #undef  WOLFSSL_TRACK_MEMORY
        #define WOLFSSL_TRACK_MEMORY
    #endif
#else
    #undef  NO_WOLFSSL_MEMORY
    #define NO_WOLFSSL_MEMORY

    #undef  NO_ERROR_STRINGS
    //#define NO_ERROR_STRINGS
#endif


/* ------------------------------------------------------------------------- */
/* Port */
/* ------------------------------------------------------------------------- */

/* Override Current Time */
/* Allows custom "custom_time()" function to be used for benchmark */
#define WOLFSSL_USER_CURRTIME
#define USER_TICKS
extern unsigned long ksdk_time(unsigned long* timer);
#define XTIME ksdk_time


/* ------------------------------------------------------------------------- */
/* RNG */
/* ------------------------------------------------------------------------- */
/* Size of returned HW RNG value */
#define CUSTOM_RAND_TYPE      unsigned int

/* Seed source */
extern unsigned int custom_rand_generate(void);
#undef  CUSTOM_RAND_GENERATE
#define CUSTOM_RAND_GENERATE  custom_rand_generate

/* Choose RNG method */
#if 1
    /* Use built-in P-RNG (SHA256 based) with HW RNG */
    /* P-RNG + HW RNG (P-RNG is ~8K) */
    #undef  HAVE_HASHDRBG
    #define HAVE_HASHDRBG
#else
    #undef  WC_NO_HASHDRBG
    #define WC_NO_HASHDRBG

    /* Bypass P-RNG and use only HW RNG */
    extern int custom_rand_generate_block(unsigned char* output, unsigned int sz);
    #undef  CUSTOM_RAND_GENERATE_BLOCK
    #define CUSTOM_RAND_GENERATE_BLOCK  custom_rand_generate_block
#endif


/* ------------------------------------------------------------------------- */
/* Enable Features */
/* ------------------------------------------------------------------------- */
#undef  KEEP_PEER_CERT
//#define KEEP_PEER_CERT

#undef  HAVE_COMP_KEY
//#define HAVE_COMP_KEY

#undef  HAVE_TLS_EXTENSIONS
//#define HAVE_TLS_EXTENSIONS

#undef  HAVE_SUPPORTED_CURVES
//#define HAVE_SUPPORTED_CURVES

#undef  WOLFSSL_BASE64_ENCODE
//#define WOLFSSL_BASE64_ENCODE

/* TLS Session Cache */
#if 0
    #define SMALL_SESSION_CACHE
#else
    #define NO_SESSION_CACHE
#endif


/* ------------------------------------------------------------------------- */
/* Disable Features */
/* ------------------------------------------------------------------------- */
#undef  NO_WOLFSSL_SERVER
//#define NO_WOLFSSL_SERVER

#undef  NO_WOLFSSL_CLIENT
//#define NO_WOLFSSL_CLIENT

#undef  NO_CRYPT_TEST
//#define NO_CRYPT_TEST

#undef  NO_CRYPT_BENCHMARK
//#define NO_CRYPT_BENCHMARK

/* In-lining of misc.c functions */
/* If defined, must include wolfcrypt/src/misc.c in build */
/* Slower, but about 1k smaller */
#undef  NO_INLINE
//#define NO_INLINE

#undef  NO_FILESYSTEM
#define NO_FILESYSTEM

#undef  NO_WRITEV
#define NO_WRITEV

#undef  NO_MAIN_DRIVER
#define NO_MAIN_DRIVER

#undef  NO_DEV_RANDOM
#define NO_DEV_RANDOM

#undef  NO_DSA
#define NO_DSA

#undef  NO_DH
#define NO_DH

#undef  NO_DES3
#define NO_DES3

#undef  NO_RC4
#define NO_RC4

#undef  NO_OLD_TLS
#define NO_OLD_TLS

#undef  NO_PSK
#define NO_PSK

#undef  NO_MD4
#define NO_MD4

#undef  NO_PWDBASED
#define NO_PWDBASED

#undef  NO_CODING
#define NO_CODING


#ifdef __cplusplus
}
#endif

#endif /* WOLFSSL_USER_SETTINGS_H */
