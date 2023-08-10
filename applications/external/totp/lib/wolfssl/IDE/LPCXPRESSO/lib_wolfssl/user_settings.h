#include <stdint.h>

/* Configuration */
#define WOLFSSL_USER_IO
#define WOLFSSL_GENERAL_ALIGNMENT 4
#define WOLFSSL_SMALL_STACK
#define WOLFSSL_BASE64_ENCODE
#define WOLFSSL_SHA512

#define HAVE_ECC
#define HAVE_AESGCM
#define HAVE_CURVE25519
#define HAVE_HKDF
#define HAVE_HASHDRBG
#define HAVE_CHACHA
#define HAVE_POLY1305
#define HAVE_ONE_TIME_AUTH
#define HAVE_TLS_EXTENSIONS
#define HAVE_SUPPORTED_CURVES
#define HAVE_ERRNO_H
#define HAVE_LWIP_NATIVE

#define FP_LUT                   4
#define FP_MAX_BITS              2048 /* 4096 */
#define ECC_USER_CURVES /* Disables P-112, P-128, P-160, P-192, P-224, P-384, P-521 but leaves P-256 enabled */
#define FP_MAX_BITS_ECC          (256 * 2)
#define ALT_ECC_SIZE
#define USE_FAST_MATH
#define SMALL_SESSION_CACHE
#define CURVED25519_SMALL
#define RSA_LOW_MEM
#define GCM_SMALL
#define ECC_SHAMIR
#define USE_SLOW_SHA2
#define MP_LOW_MEM
#define TFM_TIMING_RESISTANT
//#define TFM_ARM


/* Remove Features */
#define NO_DEV_RANDOM
#define NO_FILESYSTEM
#define NO_WRITEV
#define NO_MAIN_DRIVER
#define NO_WOLFSSL_MEMORY
#define NO_DEV_RANDOM
#define NO_MD4
#define NO_DSA
#define NO_PWDBASED
#define NO_PSK
#define NO_64BIT
#define NO_WOLFSSL_SERVER
#define NO_OLD_TLS
#define NO_DES3
#define NO_MD5
#define NO_RC4
#define NO_DH
#define NO_SHA


/* Benchmark / Testing */
#define BENCH_EMBEDDED
#define USE_CERT_BUFFERS_1024


/* Custom functions */
extern uint32_t rand_gen(void);
#define CUSTOM_RAND_GENERATE rand_gen
#define CUSTOM_RAND_TYPE     uint32_t

extern double current_time(int reset);
#define WOLFSSL_USER_CURRTIME


/* Debugging - Optional */
#if 0
#define fprintf(file, format, ...)   printf(format, ##__VA_ARGS__)
#define DEBUG_WOLFSSL
#endif
