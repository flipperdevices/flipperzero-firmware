#ifndef _EMBOS_USER_SETTINGS_H_
#define _EMBOS_USER_SETTINGS_H_

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

#endif /* _EMBOS_USER_SETTINGS_H_ */
