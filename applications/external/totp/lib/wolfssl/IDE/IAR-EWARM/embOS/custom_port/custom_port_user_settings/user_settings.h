#ifndef _EMBOS_USER_SETTINGS_H_
#define _EMBOS_USER_SETTINGS_H_

    #undef WOLFSSL_EMBOS
    #define WOLFSSL_EMBOS

    #undef WOLFCRYPT_ONLY
    #define WOLFCRYPT_ONLY

    #undef NO_64BIT
    #define NO_64BIT

    #undef SIZEOF_LONG
    #define SIZEOF_LONG 4

    #undef SIZEOF_LONG_LONG
    #define SIZEOF_LONG_LONG 8

    #undef HAVE_CHACHA
    #define HAVE_CHACHA

    #undef HAVE_POLY1305
    #define HAVE_POLY1305

    #undef ECC_SHAMIR
    #define ECC_SHAMIR

    #undef HAVE_ECC
    #define HAVE_ECC

    #undef ECC_USER_CURVES
    #define ECC_USER_CURVES

    #undef ALT_ECC_SIZE
    #define ALT_ECC_SIZE

    #undef FP_MAX_BITS_ECC
    #define FP_MAX_BITS_ECC (256 * 2)

    #undef TFM_TIMING_RESISTANT
    #define TFM_TIMING_RESISTANT

    #undef WOLFSSL_SHA512
    #define WOLFSSL_SHA512

    #undef WOLFSSL_SHA384
    #define WOLFSSL_SHA384

    #undef HAVE_AESGCM
    #define HAVE_AESGCM

    #undef NO_INLINE
    #define NO_INLINE

    #undef BENCH_EMBEDDED
    #define BENCH_EMBEDDED

    #undef WOLFSSL_SMALL_STACK
    #define WOLFSSL_SMALL_STACK

    #undef USE_WOLFSSL_MEMORY
    #define USE_WOLFSSL_MEMORY

#endif /* _EMBOS_USER_SETTINGS_H_ */
