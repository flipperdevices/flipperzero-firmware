#ifndef USER_SETTINGS_H
#define USER_SETTINGS_H

/* Math library to use */
#undef  WOLFSSL_SP
#define WOLFSSL_SP

#undef  WOLFSSL_SP_MATH_ALL
#define WOLFSSL_SP_MATH_ALL

#undef  WOLFSSL_SP_ARM32
#define WOLFSSL_SP_ARM32

/* hardening (timing resistance) */
#undef  ECC_TIMING_RESISTANT
#define ECC_TIMING_RESISTANT

#undef  WC_RSA_BLINDING
#define WC_RSA_BLINDING

/* hardware size of long long */
#define SIZE_OF_LONG_LONG 8

#undef  NO_FILESYSTEM
#define NO_FILESYSTEM

#undef  WOLFSSL_USE_ALIGN
#define WOLFSSL_USE_ALIGN

/* Build CAAM support */
#undef  WOLFSSL_CAAM
#define WOLFSSL_CAAM

#undef  WOLFSSL_QNX_CAAM
#define WOLFSSL_QNX_CAAM

#undef  WOLFSSL_IMX6Q_CAAM
#define WOLFSSL_IMX6Q_CAAM

/* Algorithms Enabled */
#undef  HAVE_AESCCM
#define HAVE_AESCCM

#undef  WOLFSSL_SHA512
#define WOLFSSL_SHA512

#undef  WOLFSSL_SHA384
#define WOLFSSL_SHA384

#undef  HAVE_HKDF
#define HAVE_HKDF

#undef  HAVE_ECC
#define HAVE_ECC

#undef  TFM_ECC256
#define TFM_ECC256

#undef  ECC_SHAMIR
#define ECC_SHAMIR

#undef  HAVE_HASHDRBG
#define HAVE_HASHDRBG

#undef  WOLF_CRYPTO_CB
#define WOLF_CRYPTO_CB

#undef  WOLFSSL_AES_COUNTER
#define WOLFSSL_AES_COUNTER

#undef  WOLFSSL_AES_DIRECT
#define WOLFSSL_AES_DIRECT

#undef  GCM_TABLE_4BIT
#define GCM_TABLE_4BIT

#undef  HAVE_AESGCM
#define HAVE_AESGCM

#undef  WC_RSA_PSS
#define WC_RSA_PSS

#undef  WOLFSSL_PSS_LONG_SALT
#define WOLFSSL_PSS_LONG_SALT

#undef  HAVE_TLS_EXTENSIONS
#define HAVE_TLS_EXTENSIONS

#undef  HAVE_FFDHE_2048
#define HAVE_FFDHE_2048

#undef  HAVE_SUPPORTED_CURVES
#define HAVE_SUPPORTED_CURVES


/* Algorithms disabled */
#undef  NO_DSA
#define NO_DSA

#undef  NO_RC4
#define NO_RC4

#undef  NO_PSK
#define NO_PSK

#undef  NO_MD4
#define NO_MD4

#undef  WOLFSSL_ASN_TEMPLATE
#define WOLFSSL_ASN_TEMPLATE

#undef  WOLFSSL_NO_SHAKE128
#define WOLFSSL_NO_SHAKE128

#undef  WOLFSSL_NO_SHAKE256
#define WOLFSSL_NO_SHAKE256

#undef  WC_NO_ASYNC_THREADING
#define WC_NO_ASYNC_THREADING

#undef  HAVE_DH_DEFAULT_PARAMS
#define HAVE_DH_DEFAULT_PARAMS

#undef  NO_DES3
#define NO_DES3
#endif
