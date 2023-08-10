/* Example custom user settings for wolfSSL and INtime RTOS port */

#ifndef WOLFSSL_USER_SETTINGS_H
#define WOLFSSL_USER_SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------- */
/* Port - Platform */
/* ------------------------------------------------------------------------- */
#undef  INTIME_RTOS
#define INTIME_RTOS

#undef  WOLF_EXAMPLES_STACK
#define WOLF_EXAMPLES_STACK         (1<<17)

#undef  WOLFSSL_GENERAL_ALIGNMENT
#define WOLFSSL_GENERAL_ALIGNMENT   4

/* platform already has min()/max() */
#undef  WOLFSSL_HAVE_MIN
#define WOLFSSL_HAVE_MIN
#undef  WOLFSSL_HAVE_MAX
#define WOLFSSL_HAVE_MAX

/* disable directory support */
#undef  NO_WOLFSSL_DIR
//#define NO_WOLFSSL_DIR

/* disable writev */
#undef  NO_WRITEV
#define NO_WRITEV

/* we provide main entry point */
#undef  NO_MAIN_DRIVER
#define NO_MAIN_DRIVER

/* if using in single threaded mode */
#undef  SINGLE_THREADED
//#define SINGLE_THREADED
/* Note: HAVE_THREAD_LS is not support for INtime RTOS */

/* reduces stack usage, by using malloc/free for stack variables over 100 bytes */
#undef  WOLFSSL_SMALL_STACK
//#define WOLFSSL_SMALL_STACK


/* ------------------------------------------------------------------------- */
/* Math Configuration */
/* ------------------------------------------------------------------------- */
/* fast math uses stack and inline assembly to speed up math */
#undef  USE_FAST_MATH
#define USE_FAST_MATH

#ifdef USE_FAST_MATH
    /* timing resistance for side-channel attack protection */
    #undef  TFM_TIMING_RESISTANT
    #define TFM_TIMING_RESISTANT
#endif


/* ------------------------------------------------------------------------- */
/* Crypto */
/* ------------------------------------------------------------------------- */
/* ECC */
#if 1
    #undef  HAVE_ECC
    #define HAVE_ECC

    /* Support for custom curves */
    #define WOLFSSL_CUSTOM_CURVES

    /* Curve types */
    //#define NO_ECC_SECP
    #define HAVE_ECC_SECPR2
    #define HAVE_ECC_SECPR3
    #define HAVE_ECC_BRAINPOOL
    #define HAVE_ECC_KOBLITZ

    /* Curve sizes */
    #undef  HAVE_ALL_CURVES
    //#define HAVE_ALL_CURVES
    #ifndef HAVE_ALL_CURVES
        /* allows enabling custom curve sizes */
        #undef  ECC_USER_CURVES
        #define ECC_USER_CURVES

        //#define HAVE_ECC112
        //#define HAVE_ECC128
        //#define HAVE_ECC160
        #define HAVE_ECC192
        #define HAVE_ECC224
        //#define NO_ECC256
        #define HAVE_ECC384
        #define HAVE_ECC521
    #endif

    /* Fixed point cache (speeds repeated operations against same private key) */
    #undef  FP_ECC
    #define FP_ECC
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
    #define ECC_SHAMIR

    /* Reduces heap usage, but slower */
    /* timing resistance for side-channel attack protection */
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

    /* RSA blinding countermeasures */
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

    #ifdef HAVE_AESGCM
        /* GCM Method: GCM_SMALL, GCM_WORD32 or GCM_TABLE */
        //#define GCM_SMALL
        #define GCM_TABLE
    #endif

    #undef  WOLFSSL_AES_COUNTER
    #define WOLFSSL_AES_COUNTER

    #undef  HAVE_AESCCM
    #define HAVE_AESCCM

    #undef  WOLFSSL_AES_DIRECT
    #define WOLFSSL_AES_DIRECT

    #undef  HAVE_AES_KEYWRAP
    #define HAVE_AES_KEYWRAP
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
/* Enable Features */
/* ------------------------------------------------------------------------- */
#undef  KEEP_PEER_CERT
#define KEEP_PEER_CERT

#undef  HAVE_COMP_KEY
#define HAVE_COMP_KEY

#undef  HAVE_ECC_ENCRYPT
#define HAVE_ECC_ENCRYPT

#undef  HAVE_TLS_EXTENSIONS
#define HAVE_TLS_EXTENSIONS

#undef  HAVE_SUPPORTED_CURVES
#define HAVE_SUPPORTED_CURVES

#undef  HAVE_EXTENDED_MASTER
#define HAVE_EXTENDED_MASTER

#undef  WOLFSSL_DTLS
#define WOLFSSL_DTLS

#undef  OPENSSL_EXTRA
#define OPENSSL_EXTRA

#undef  WOLFSSL_BASE64_ENCODE
#define WOLFSSL_BASE64_ENCODE

#undef  HAVE_HKDF
#define HAVE_HKDF

#undef  WOLFSSL_CMAC
#define WOLFSSL_CMAC

#undef  WOLFSSL_KEY_GEN
#define WOLFSSL_KEY_GEN

#undef  WOLFSSL_CERT_GEN
#define WOLFSSL_CERT_GEN

#undef  WOLFSSL_CERT_REQ
#define WOLFSSL_CERT_REQ

#undef  WOLFSSL_CERT_EXT
#define WOLFSSL_CERT_EXT

#undef  HAVE_PK_CALLBACKS
#define HAVE_PK_CALLBACKS

#undef  HAVE_ALPN
#define HAVE_ALPN

#undef  HAVE_SNI
#define HAVE_SNI

#undef  HAVE_MAX_FRAGMENT
#define HAVE_MAX_FRAGMENT

#undef  HAVE_TRUNCATED_HMAC
#define HAVE_TRUNCATED_HMAC

#undef  SESSION_CERTS
#define SESSION_CERTS

#undef  HAVE_SESSION_TICKET
#define HAVE_SESSION_TICKET

#undef  WOLFCRYPT_HAVE_SRP
#define WOLFCRYPT_HAVE_SRP

#undef  WOLFSSL_HAVE_CERT_SERVICE
#define WOLFSSL_HAVE_CERT_SERVICE

#undef  HAVE_PKCS7
#define HAVE_PKCS7

#undef  HAVE_X963_KDF
#define HAVE_X963_KDF

#undef  WOLFSSL_HAVE_WOLFSCEP
#define WOLFSSL_HAVE_WOLFSCEP

#undef  WOLFSSL_ALWAYS_KEEP_SNI
#define WOLFSSL_ALWAYS_KEEP_SNI

#undef  WOLFSSL_ALWAYS_VERIFY_CB
#define WOLFSSL_ALWAYS_VERIFY_CB

#undef  WOLFSSL_SEP
#define WOLFSSL_SEP

#undef  ATOMIC_USER
#define ATOMIC_USER

#undef  HAVE_OCSP
#define HAVE_OCSP

#undef  HAVE_CERTIFICATE_STATUS_REQUEST
#define HAVE_CERTIFICATE_STATUS_REQUEST

#undef  HAVE_CERTIFICATE_STATUS_REQUEST_V2
#define HAVE_CERTIFICATE_STATUS_REQUEST_V2

#undef  HAVE_CRL
#define HAVE_CRL

#undef  PERSIST_CERT_CACHE
//#define PERSIST_CERT_CACHE

#undef  PERSIST_SESSION_CACHE
//#define PERSIST_SESSION_CACHE

#undef  WOLFSSL_DER_LOAD
//#define WOLFSSL_DER_LOAD

#undef  WOLFSSL_DES_ECB
//#define WOLFSSL_DES_ECB

#undef  HAVE_CAMELLIA
//#define HAVE_CAMELLIA

#undef  HAVE_NULL_CIPHER
//#define HAVE_NULL_CIPHER

#undef  WOLFSSL_RIPEMD
//#define WOLFSSL_RIPEMD


/* TLS Session Cache */
#if 1
    #define SMALL_SESSION_CACHE
    //#define MEDIUM_SESSION_CACHE
    //#define BIG_SESSION_CACHE
    //#define HUGE_SESSION_CACHE
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

/* disables TLS 1.0/1.1 support */
#undef  NO_OLD_TLS
//#define NO_OLD_TLS

/* disable access to filesystem */
#undef  NO_FILESYSTEM
//#define NO_FILESYSTEM

#undef  NO_RC4
#define NO_RC4

#undef  NO_MD4
#define NO_MD4

/* Pre-shared keys */
#undef  NO_PSK
//#define NO_PSK

#undef  NO_DSA
//#define NO_DSA

#undef  NO_DH
//#define NO_DH

#undef  NO_DES3
//#define NO_DES3

#undef  NO_PWDBASED
//#define NO_PWDBASED

/* encoding/decoding support */
#undef  NO_CODING
//#define NO_CODING

/* memory wrappers and memory callbacks */
#undef  NO_WOLFSSL_MEMORY
//#define NO_WOLFSSL_MEMORY

/* In-lining of misc.c functions */
/* If defined, must include wolfcrypt/src/misc.c in build */
/* Slower, but about 1k smaller */
#undef  NO_INLINE
//#define NO_INLINE



/* ------------------------------------------------------------------------- */
/* Benchmark / Test */
/* ------------------------------------------------------------------------- */
#undef  NO_CRYPT_TEST
//#define NO_CRYPT_TEST

#undef  NO_CRYPT_BENCHMARK
//#define NO_CRYPT_BENCHMARK

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
    /* Use this to measure / print heap usage */
    #if 0
        #undef  USE_WOLFSSL_MEMORY
        #define USE_WOLFSSL_MEMORY

        #undef  WOLFSSL_TRACK_MEMORY
        #define WOLFSSL_TRACK_MEMORY
    #endif

    /* Math debugging (adds support for mp_dump) */
    #undef  WOLFSSL_DEBUG_MATH
    //#define WOLFSSL_DEBUG_MATH
#else
    #undef  NO_ERROR_STRINGS
    //#define NO_ERROR_STRINGS
#endif


#ifdef __cplusplus
}
#endif

#endif /* WOLFSSL_USER_SETTINGS_H */
