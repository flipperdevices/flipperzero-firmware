#ifndef _VSARM_USER_SETTINGS_H_
#define _VSARM_USER_SETTINGS_H_

/* Enables blinding mode, to prevent timing attacks */
#define WC_RSA_BLINDING

#define WOLFSSL_SHA384
#define WOLFSSL_SHA512
#define NO_PSK
#define HAVE_EXTENDED_MASTER
#define HAVE_TLS_EXTENSIONS
#define HAVE_SUPPORTED_CURVES
#define HAVE_AESGCM
#define USE_FAST_MATH
#define TFM_TIMING_RESISTANT
#define ECC_TIMING_RESISTANT
#define NO_RC4
#define NO_DSA
#define NO_MD4
#define HAVE_ECC
#define HAVE_HASHDRBG


#endif /* _VSARM_USER_SETTINGS_H_ */
