
#ifndef USER_SETTINGS_H
#define USER_SETTINGS_H

//#define DEBUG_WOLFSSL

#define NO_MAIN_DRIVER
#define USE_CERT_BUFFERS_2048
#define USE_CERT_BUFFERS_256

/* print out cycles per byte with benchmark when component r_wdt WDT is enabled */
#define SYNERGY_CYCLE_COUNT
#define BENCH_EMBEDDED

/* Use turn on all SCE acceleration */
#define WOLFSSL_SCE

/* Used to turn off TRNG */
/* #define WOLFSSL_SCE_NO_TRNG */

/* Used to turn off AES hardware acc. */
/* #define WOLFSSL_SCE_NO_AES */

/* Used to turn off HASH hardware acc. */
/* #define WOLFSSL_SCE_NO_HASH */

#if defined(WOLFSSL_SCE_NO_TRNG)
    /* use unsafe test seed if TRNG not used (not for production) */
    #define WOLFSSL_GENSEED_FORTEST
#endif

#define HAVE_ECC
#define ALT_ECC_SIZE

#define HAVE_CHACHA
#define HAVE_POLY1305
#define HAVE_ONE_TIME_AUTH
#define HAVE_AESGCM

#define HAVE_AES_ECB
#define WOLFSSL_AES_DIRECT

#define USE_FAST_MATH

#define TFM_TIMING_RESISTANT
#define WC_RSA_BLINDING
#define ECC_TIMING_RESISTANT

#define NO_WOLFSSL_DIR

#define HAVE_NETX
#define THREADX
#define THREADX_NO_DC_PRINTF
#define NO_WRITEV
#define SIZEOF_LONG 4
#define SIZEOF_LONG_LONG 8

#if 1
    /* Optimizations */
    #define WOLFSSL_HAVE_SP_RSA
    #define WOLFSSL_HAVE_SP_ECC
    #define WOLFSSL_SP_ARM_CORTEX_M_ASM
#endif

/* TLS 1.3 */
#define WOLFSSL_TLS13
#define HAVE_TLS_EXTENSIONS
#define HAVE_SUPPORTED_CURVES
#define HAVE_FFDHE_2048
#define HAVE_HKDF
#define WC_RSA_PSS

#define HAVE_CURVE25519
#define HAVE_ED25519
#define WOLFSSL_SHA512

#endif
