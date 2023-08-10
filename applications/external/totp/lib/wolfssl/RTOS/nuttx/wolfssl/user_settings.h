#include <nuttx/config.h>

/* Library */
#define SINGLE_THREADED
#define WOLFSSL_SMALL_STACK

/* Environment */
#define NO_FILESYSTEM
#define HAVE_STRINGS_H
#define WOLF_C99

/* Math */
#if defined(CONFIG_ARCH_CHIP_STM32L552ZE) || defined(CONFIG_ARCH_CHIP_STM32F746ZG) || defined(CONFIG_ARCH_CHIP_STM32H743ZI)
    #define WOLFSSL_SP
    #define WOLFSSL_SP_SMALL
    #define WOLFSSL_HAVE_SP_RSA
    #define WOLFSSL_HAVE_SP_DH
    #define WOLFSSL_HAVE_SP_ECC
    #define WOLFSSL_SP_MATH
    #define SP_WORD_SIZE 32
    #define WOLFSSL_SP_ASM
    #define WOLFSSL_SP_ARM_CORTEX_M_ASM
#else
    #define WOLFSSL_SP_MATH_ALL
#endif

/* Crypto */
#define HAVE_ECC
#define ECC_TIMING_RESISTANT
#define WC_RSA_BLINDING
#undef  RSA_LOW_MEM
#define NO_MD4
#define NO_DSA

/* RNG */
#define WOLFSSL_GENSEED_FORTEST

/* Applications */
#define NO_MAIN_FUNCTION
#define BENCH_EMBEDDED
#define WOLFSSL_BENCHMARK_FIXED_UNITS_MB

/* Development */
/*#define DEBUG_WOLFSSL*/

#define HAVE_TLS_EXTENSIONS
#define HAVE_SUPPORTED_CURVES
#define HAVE_ENCRYPT_THEN_MAC
#define HAVE_EXTENDED_MASTER
#define WOLFSSL_TLS13
#define HAVE_AESGCM
#define HAVE_HKDF
#define HAVE_DH
#define HAVE_FFDHE_2048
#define HAVE_DH_DEFAULT_PARAMS
#define WC_RSA_PSS
#define HAVE_AEAD
#define WOLFSSL_SHA224
#define WOLFSSL_SHA384
#define WOLFSSL_SHA512
#define WOLFSSL_SHA3
#define HAVE_POLY1305
#define HAVE_CHACHA
#define HAVE_ENCRYPT_THEN_MAC
#define NO_OLD_TLS
