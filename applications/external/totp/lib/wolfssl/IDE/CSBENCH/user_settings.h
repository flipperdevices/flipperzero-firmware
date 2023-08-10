#ifndef WOLFSSL_CSBENCH_H
#define WOLFSSL_CSBENCH_H

/* wolfSSL settings */
#define WOLFCRYPT_ONLY
#define USE_FAST_MATH
#define TFM_TIMING_RESISTANT
#define WC_RSA_BLINDING

#define SINGLE_THREADED
#define HAVE_AESGCM
#define NO_ASN_TIME

#define HAVE_ECC
#define ECC_TIMING_RESISTANT
#define WOLFSSL_NUCLEUS

/* wolfSSH settings */
#define WOLFSSH_SFTP
//#define DEBUG_WOLFSSH

#endif
