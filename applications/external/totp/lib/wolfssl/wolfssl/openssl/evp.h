/* evp.h
 *
 * Copyright (C) 2006-2023 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */



/*!
    \file wolfssl/openssl/evp.h
    \brief evp.h defines mini evp openssl compatibility layer
 */


#ifndef WOLFSSL_EVP_H_
#define WOLFSSL_EVP_H_

#include <wolfssl/wolfcrypt/types.h>

#ifdef WOLFSSL_PREFIX
#include "prefix_evp.h"
#endif

#ifndef NO_MD4
    #include <wolfssl/openssl/md4.h>
#endif
#ifndef NO_MD5
    #include <wolfssl/openssl/md5.h>
#endif
#include <wolfssl/openssl/sha.h>
#include <wolfssl/openssl/sha3.h>
#include <wolfssl/openssl/ripemd.h>
#include <wolfssl/openssl/rsa.h>
#include <wolfssl/openssl/dsa.h>
#include <wolfssl/openssl/ec.h>
#include <wolfssl/openssl/dh.h>
#include <wolfssl/openssl/opensslv.h>
#include <wolfssl/openssl/compat_types.h>

#include <wolfssl/wolfcrypt/aes.h>
#include <wolfssl/wolfcrypt/des3.h>
#include <wolfssl/wolfcrypt/arc4.h>
#include <wolfssl/wolfcrypt/chacha20_poly1305.h>
#include <wolfssl/wolfcrypt/hmac.h>
#include <wolfssl/wolfcrypt/pwdbased.h>
#ifdef WOLFSSL_SM3
    #include <wolfssl/wolfcrypt/sm3.h>
#endif
#ifdef WOLFSSL_SM4
    #include <wolfssl/wolfcrypt/sm4.h>
#endif

#if defined(WOLFSSL_BASE64_ENCODE) || defined(WOLFSSL_BASE64_DECODE)
#include <wolfssl/wolfcrypt/coding.h>
#endif

#ifdef HAVE_ARIA
    #include <wolfssl/wolfcrypt/port/aria/aria-crypt.h>
#endif

#ifdef __cplusplus
    extern "C" {
#endif


#ifndef NO_MD4
    WOLFSSL_API const WOLFSSL_EVP_MD* wolfSSL_EVP_md4(void);
#endif
#ifndef NO_MD5
    WOLFSSL_API const WOLFSSL_EVP_MD* wolfSSL_EVP_md5(void);
#endif
WOLFSSL_API void wolfSSL_EVP_set_pw_prompt(const char *prompt);
WOLFSSL_API const WOLFSSL_EVP_MD* wolfSSL_EVP_mdc2(void);
WOLFSSL_API const WOLFSSL_EVP_MD* wolfSSL_EVP_sha1(void);
WOLFSSL_API const WOLFSSL_EVP_MD* wolfSSL_EVP_sha224(void);
WOLFSSL_API const WOLFSSL_EVP_MD* wolfSSL_EVP_sha256(void);
WOLFSSL_API const WOLFSSL_EVP_MD* wolfSSL_EVP_sha384(void);
WOLFSSL_API const WOLFSSL_EVP_MD* wolfSSL_EVP_sha512(void);
WOLFSSL_API const WOLFSSL_EVP_MD* wolfSSL_EVP_shake128(void);
WOLFSSL_API const WOLFSSL_EVP_MD* wolfSSL_EVP_shake256(void);
WOLFSSL_API const WOLFSSL_EVP_MD *wolfSSL_EVP_sha512_224(void);
WOLFSSL_API const WOLFSSL_EVP_MD *wolfSSL_EVP_sha512_256(void);
WOLFSSL_API const WOLFSSL_EVP_MD *wolfSSL_EVP_ripemd160(void);

WOLFSSL_API const WOLFSSL_EVP_MD* wolfSSL_EVP_sha3_224(void);
WOLFSSL_API const WOLFSSL_EVP_MD* wolfSSL_EVP_sha3_256(void);
WOLFSSL_API const WOLFSSL_EVP_MD* wolfSSL_EVP_sha3_384(void);
WOLFSSL_API const WOLFSSL_EVP_MD* wolfSSL_EVP_sha3_512(void);

WOLFSSL_API const WOLFSSL_EVP_MD* wolfSSL_EVP_sm3(void);

WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_128_ecb(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_192_ecb(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_256_ecb(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_128_cbc(void);
#if !defined(NO_AES) && (defined(HAVE_AES_CBC) || defined(WOLFSSL_AES_DIRECT))
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_192_cbc(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_256_cbc(void);
#endif
#ifndef NO_AES
#ifdef WOLFSSL_AES_CFB
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_128_cfb1(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_192_cfb1(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_256_cfb1(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_128_cfb8(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_192_cfb8(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_256_cfb8(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_128_cfb128(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_192_cfb128(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_256_cfb128(void);
#endif
#ifdef WOLFSSL_AES_OFB
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_128_ofb(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_192_ofb(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_256_ofb(void);
#endif
#ifdef WOLFSSL_AES_XTS
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_128_xts(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_256_xts(void);
#endif
#endif /* NO_AES */
#if !defined(NO_AES) && defined(HAVE_AESGCM)
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_128_gcm(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_192_gcm(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_256_gcm(void);
#endif
#if !defined(NO_AES) && defined(HAVE_AESCCM)
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_128_ccm(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_192_ccm(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_256_ccm(void);
#endif
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_128_ctr(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_192_ctr(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_256_ctr(void);
#if defined(HAVE_ARIA)
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aria_128_gcm(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aria_192_gcm(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aria_256_gcm(void);
#endif
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_des_ecb(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_des_ede3_ecb(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_des_cbc(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_des_ede3_cbc(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_rc4(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_enc_null(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_rc2_cbc(void);
#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_chacha20_poly1305(void);
#endif
#ifdef HAVE_CHACHA
/* ChaCha IV + counter is set as one IV in EVP */
#define WOLFSSL_EVP_CHACHA_IV_BYTES     (CHACHA_IV_BYTES + sizeof(word32))
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_chacha20(void);
#endif
#ifdef WOLFSSL_SM4_ECB
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_sm4_ecb(void);
#endif
#ifdef WOLFSSL_SM4_CBC
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_sm4_cbc(void);
#endif
#ifdef WOLFSSL_SM4_CTR
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_sm4_ctr(void);
#endif
#ifdef WOLFSSL_SM4_GCM
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_sm4_gcm(void);
#endif
#ifdef WOLFSSL_SM4_CCM
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_sm4_ccm(void);
#endif

typedef union {
    #ifndef NO_MD4
        WOLFSSL_MD4_CTX    md4;
    #endif
    #ifndef NO_MD5
        WOLFSSL_MD5_CTX    md5;
    #endif
    #ifndef NO_SHA
        WOLFSSL_SHA_CTX    sha;
    #endif
    #ifdef WOLFSSL_SHA224
        WOLFSSL_SHA224_CTX sha224;
    #endif
    #ifndef NO_SHA256
        WOLFSSL_SHA256_CTX sha256;
    #endif
    #ifdef WOLFSSL_SHA384
        WOLFSSL_SHA384_CTX sha384;
    #endif
    #ifdef WOLFSSL_SHA512
        WOLFSSL_SHA512_CTX sha512;
    #endif
    #ifdef WOLFSSL_RIPEMD
        WOLFSSL_RIPEMD_CTX ripemd;
    #endif
    #ifndef WOLFSSL_NOSHA3_224
        WOLFSSL_SHA3_224_CTX sha3_224;
    #endif
    #ifndef WOLFSSL_NOSHA3_256
        WOLFSSL_SHA3_256_CTX sha3_256;
    #endif
        WOLFSSL_SHA3_384_CTX sha3_384;
    #ifndef WOLFSSL_NOSHA3_512
        WOLFSSL_SHA3_512_CTX sha3_512;
    #endif
    #ifdef WOLFSSL_SM3
        wc_Sm3               sm3;
    #endif
} WOLFSSL_Hasher;


struct WOLFSSL_EVP_MD_CTX {
    union {
        WOLFSSL_Hasher digest;
    #ifndef NO_HMAC
        Hmac hmac;
    #endif
    } hash;
    enum wc_HashType macType;
    WOLFSSL_EVP_PKEY_CTX *pctx;
#ifndef NO_HMAC
    unsigned int isHMAC;
#endif
};


typedef union {
#ifndef NO_AES
    Aes  aes;
#ifdef WOLFSSL_AES_XTS
    XtsAes xts;
#endif
#endif
#ifdef HAVE_ARIA
    wc_Aria aria;
#endif
#ifndef NO_DES3
    Des  des;
    Des3 des3;
#endif
    Arc4 arc4;
#ifdef WOLFSSL_QT
    int (*ctrl) (WOLFSSL_EVP_CIPHER_CTX *, int type, int arg, void *ptr);
#endif
#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
    ChaChaPoly_Aead chachaPoly;
#endif
#ifdef HAVE_CHACHA
    ChaCha chacha;
#endif
#ifdef WOLFSSL_SM4
    wc_Sm4 sm4;
#endif
} WOLFSSL_Cipher;

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)

#define NID_aes_128_cbc                 419
#define NID_aes_192_cbc                 423
#define NID_aes_256_cbc                 427
#define NID_aes_128_ccm                 896
#define NID_aes_192_ccm                 899
#define NID_aes_256_ccm                 902
#define NID_aes_128_gcm                 895
#define NID_aes_192_gcm                 898
#define NID_aes_256_gcm                 901
#define NID_aes_128_ctr                 904
#define NID_aes_192_ctr                 905
#define NID_aes_256_ctr                 906
#define NID_aes_128_ecb                 418
#define NID_aes_192_ecb                 422
#define NID_aes_256_ecb                 426
#define NID_des_cbc                     31
#define NID_des_ecb                     29
#define NID_des_ede3_cbc                44
#define NID_des_ede3_ecb                33
#define NID_aes_128_cfb1                650
#define NID_aes_192_cfb1                651
#define NID_aes_256_cfb1                652
#define NID_aes_128_cfb8                653
#define NID_aes_192_cfb8                654
#define NID_aes_256_cfb8                655
#define NID_aes_128_cfb128              421
#define NID_aes_192_cfb128              425
#define NID_aes_256_cfb128              429
#define NID_aes_128_ofb                 420
#define NID_aes_192_ofb                 424
#define NID_aes_256_ofb                 428
#define NID_aes_128_xts                 913
#define NID_aes_256_xts                 914
#define NID_camellia_128_cbc            751
#define NID_camellia_256_cbc            753
#define NID_chacha20_poly1305           1018
#define NID_chacha20                    1019
#define NID_sm4_ecb                     1133
#define NID_sm4_cbc                     1134
#define NID_sm4_ctr                     1139
#define NID_sm4_gcm                     1248
#define NID_sm4_ccm                     1249
#define NID_md5WithRSA                  104
#define NID_md2WithRSAEncryption        9
#define NID_md5WithRSAEncryption        99
#define NID_dsaWithSHA1                 113
#define NID_dsaWithSHA1_2               70
#define NID_sha1WithRSA                 115
#define NID_sha1WithRSAEncryption       65
#define NID_sha224WithRSAEncryption     671
#define NID_sha256WithRSAEncryption     668
#define NID_sha384WithRSAEncryption     669
#define NID_sha512WithRSAEncryption     670
#define NID_RSA_SHA3_224                1116
#define NID_RSA_SHA3_256                1117
#define NID_RSA_SHA3_384                1118
#define NID_RSA_SHA3_512                1119
#define NID_rsassaPss                   912
#define NID_ecdsa_with_SHA1             416
#define NID_ecdsa_with_SHA224           793
#define NID_ecdsa_with_SHA256           794
#define NID_ecdsa_with_SHA384           795
#define NID_ecdsa_with_SHA512           796
#define NID_ecdsa_with_SHA3_224         1112
#define NID_ecdsa_with_SHA3_256         1113
#define NID_ecdsa_with_SHA3_384         1114
#define NID_ecdsa_with_SHA3_512         1115
#define NID_dsa_with_SHA224             802
#define NID_dsa_with_SHA256             803
#define NID_sha3_224                    1096
#define NID_sha3_256                    1097
#define NID_sha3_384                    1098
#define NID_sha3_512                    1099
#define NID_blake2b512                  1056
#define NID_blake2s256                  1057
#define NID_shake128                    1100
#define NID_shake256                    1101
#define NID_sha1                        64
#define NID_sha224                      675
#define NID_sm3                         1143
#define NID_md2                         77
#define NID_md4                         257
#define NID_md5                         40
#define NID_hmac                        855
#define NID_hmacWithSHA1                163
#define NID_hmacWithSHA224              798
#define NID_hmacWithSHA256              799
#define NID_hmacWithSHA384              800
#define NID_hmacWithSHA512              801
#define NID_hkdf                        1036
#define NID_cmac                        894
#define NID_dhKeyAgreement              28
#define NID_ffdhe2048                   1126
#define NID_ffdhe3072                   1127
#define NID_ffdhe4096                   1128
#define NID_rc4                         5
#define NID_bf_cbc                      91
#define NID_bf_ecb                      92
#define NID_bf_cfb64                    93
#define NID_bf_ofb64                    94
#define NID_cast5_cbc                   108
#define NID_cast5_ecb                   109
#define NID_cast5_cfb64                 110
#define NID_cast5_ofb64                 111
/* key exchange */
#define NID_kx_rsa                      1037
#define NID_kx_ecdhe                    1038
#define NID_kx_dhe                      1039
#define NID_kx_ecdhe_psk                1040
#define NID_kx_dhe_psk                  1041
#define NID_kx_rsa_psk                  1042
#define NID_kx_psk                      1043
#define NID_kx_srp                      1044
#define NID_kx_gost                     1045
#define NID_kx_any                      1063
/* server authentication */
#define NID_auth_rsa                    1046
#define NID_auth_ecdsa                  1047
#define NID_auth_psk                    1048
#define NID_auth_dss                    1049
#define NID_auth_srp                    1052
#define NID_auth_null                   1054
#define NID_auth_any                    1055
/* Curve */
#define NID_aria_128_gcm                1123
#define NID_aria_192_gcm                1124
#define NID_aria_256_gcm                1125
#define NID_sm2                         1172

#define NID_X9_62_id_ecPublicKey EVP_PKEY_EC
#define NID_rsaEncryption        EVP_PKEY_RSA
#define NID_dsa                  EVP_PKEY_DSA

#define EVP_PKEY_OP_SIGN    (1 << 3)
#define EVP_PKEY_OP_VERIFY  (1 << 5)
#define EVP_PKEY_OP_ENCRYPT (1 << 6)
#define EVP_PKEY_OP_DECRYPT (1 << 7)
#define EVP_PKEY_OP_DERIVE  (1 << 8)

#define EVP_PKEY_PRINT_INDENT_MAX    128

enum {
    AES_128_CBC_TYPE       = 1,
    AES_192_CBC_TYPE       = 2,
    AES_256_CBC_TYPE       = 3,
    AES_128_CTR_TYPE       = 4,
    AES_192_CTR_TYPE       = 5,
    AES_256_CTR_TYPE       = 6,
    AES_128_ECB_TYPE       = 7,
    AES_192_ECB_TYPE       = 8,
    AES_256_ECB_TYPE       = 9,
    DES_CBC_TYPE           = 10,
    DES_ECB_TYPE           = 11,
    DES_EDE3_CBC_TYPE      = 12,
    DES_EDE3_ECB_TYPE      = 13,
    ARC4_TYPE              = 14,
    NULL_CIPHER_TYPE       = 15,
    EVP_PKEY_RSA           = 16,
    EVP_PKEY_DSA           = 17,
    EVP_PKEY_EC            = 18,
    AES_128_GCM_TYPE       = 21,
    AES_192_GCM_TYPE       = 22,
    AES_256_GCM_TYPE       = 23,
    EVP_PKEY_DH            = NID_dhKeyAgreement,
    EVP_PKEY_HMAC          = NID_hmac,
    EVP_PKEY_CMAC          = NID_cmac,
    EVP_PKEY_HKDF          = NID_hkdf,
    EVP_PKEY_FALCON        = 300, /* Randomly picked value. */
    EVP_PKEY_DILITHIUM     = 301, /* Randomly picked value. */
    AES_128_CFB1_TYPE      = 24,
    AES_192_CFB1_TYPE      = 25,
    AES_256_CFB1_TYPE      = 26,
    AES_128_CFB8_TYPE      = 27,
    AES_192_CFB8_TYPE      = 28,
    AES_256_CFB8_TYPE      = 29,
    AES_128_CFB128_TYPE    = 30,
    AES_192_CFB128_TYPE    = 31,
    AES_256_CFB128_TYPE    = 32,
    AES_128_OFB_TYPE       = 33,
    AES_192_OFB_TYPE       = 34,
    AES_256_OFB_TYPE       = 35,
    AES_128_XTS_TYPE       = 36,
    AES_256_XTS_TYPE       = 37,
    CHACHA20_POLY1305_TYPE = 38,
    CHACHA20_TYPE          = 39,
    AES_128_CCM_TYPE       = 40,
    AES_192_CCM_TYPE       = 41,
    AES_256_CCM_TYPE       = 42,
    SM4_ECB_TYPE           = 43,
    SM4_CBC_TYPE           = 44,
    SM4_CTR_TYPE           = 45,
    SM4_GCM_TYPE           = 46,
    SM4_CCM_TYPE           = 47,
    ARIA_128_GCM_TYPE      = 48,
    ARIA_192_GCM_TYPE      = 49,
    ARIA_256_GCM_TYPE      = 50
};

#endif /* OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL */


#define WOLFSSL_EVP_BUF_SIZE 16
struct WOLFSSL_EVP_CIPHER_CTX {
    int            keyLen;         /* user may set for variable */
    int            block_size;
    unsigned long  flags;
    unsigned char  enc;            /* if encrypt side, then true */
    unsigned char  cipherType;
#if !defined(NO_AES)
    /* working iv pointer into cipher */
    ALIGN16 unsigned char  iv[AES_BLOCK_SIZE];
#elif defined(WOLFSSL_SM4)
    ALIGN16 unsigned char  iv[SM4_BLOCK_SIZE];
#elif defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
    ALIGN16 unsigned char  iv[CHACHA20_POLY1305_AEAD_IV_SIZE];
#elif !defined(NO_DES3)
    ALIGN16 unsigned char  iv[DES_BLOCK_SIZE];
#endif
    WOLFSSL_Cipher  cipher;
    ALIGN16 byte buf[WOLFSSL_EVP_BUF_SIZE];
    int  bufUsed;
    ALIGN16 byte lastBlock[WOLFSSL_EVP_BUF_SIZE];
    int  lastUsed;
#if !defined(NO_AES) || !defined(NO_DES3) || defined(HAVE_AESGCM) || \
    defined (WOLFSSL_AES_XTS) || (defined(HAVE_CHACHA) || \
    defined(HAVE_POLY1305) || defined(HAVE_AESCCM)) || \
    defined(WOLFSSL_SM4_GCM) || defined(WOLFSSL_SM4_CCM)
#define HAVE_WOLFSSL_EVP_CIPHER_CTX_IV
    int    ivSz;
#if defined(HAVE_AESGCM) || defined(HAVE_AESCCM) || \
    defined(WOLFSSL_SM4_GCM) || defined(WOLFSSL_SM4_CCM)
    byte*   authBuffer;
    int     authBufferLen;
    byte*   authIn;
    int     authInSz;
#endif
#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
    byte*   key;                 /* used in partial Init()s */
#endif
#if defined(HAVE_AESGCM) || defined(HAVE_AESCCM) || defined(HAVE_ARIA) || \
    defined(WOLFSSL_SM4_GCM) || defined(WOLFSSL_SM4_CCM) || \
    (defined(HAVE_CHACHA) && defined(HAVE_POLY1305))
#if defined(HAVE_AESGCM) || defined(HAVE_AESCCM) || defined(HAVE_ARIA)
    ALIGN16 unsigned char authTag[AES_BLOCK_SIZE];
#elif defined(WOLFSSL_SM4_GCM) || defined(WOLFSSL_SM4_CCM)
    ALIGN16 unsigned char authTag[SM4_BLOCK_SIZE];
#else
    ALIGN16 unsigned char authTag[CHACHA20_POLY1305_AEAD_AUTHTAG_SIZE];
#endif
    int     authTagSz;
#endif
#if defined(HAVE_AESGCM) || defined(HAVE_AESCCM) || \
    defined(WOLFSSL_SM4_GCM) || defined(WOLFSSL_SM4_CCM)
    byte    authIvGenEnable:1;
    byte    authIncIv:1;
#endif
#endif
};

struct WOLFSSL_EVP_PKEY_CTX {
    WOLFSSL_EVP_PKEY *pkey;
    WOLFSSL_EVP_PKEY *peerKey;
    int op; /* operation */
    int padding;
    int nbits;
#ifdef HAVE_ECC
    int curveNID;
#endif
#ifndef NO_RSA
    const WOLFSSL_EVP_MD* md;
#endif
};

struct WOLFSSL_ASN1_PCTX {
    int dummy;
};
#if defined(WOLFSSL_BASE64_ENCODE) || defined(WOLFSSL_BASE64_DECODE)

#define   BASE64_ENCODE_BLOCK_SIZE  48
#define   BASE64_ENCODE_RESULT_BLOCK_SIZE 64
#define   BASE64_DECODE_BLOCK_SIZE  4

struct WOLFSSL_EVP_ENCODE_CTX {
    void* heap;
    int   remaining;     /* num of bytes in data[] */
    byte  data[BASE64_ENCODE_BLOCK_SIZE];/* storage for unprocessed raw data */
};
typedef struct WOLFSSL_EVP_ENCODE_CTX WOLFSSL_EVP_ENCODE_CTX;

WOLFSSL_API struct WOLFSSL_EVP_ENCODE_CTX* wolfSSL_EVP_ENCODE_CTX_new(void);
WOLFSSL_API void wolfSSL_EVP_ENCODE_CTX_free(WOLFSSL_EVP_ENCODE_CTX* ctx);
#endif /* WOLFSSL_BASE64_ENCODE || WOLFSSL_BASE64_DECODE */

#if defined(WOLFSSL_BASE64_ENCODE)
WOLFSSL_API void wolfSSL_EVP_EncodeInit(WOLFSSL_EVP_ENCODE_CTX* ctx);
WOLFSSL_API int  wolfSSL_EVP_EncodeUpdate(WOLFSSL_EVP_ENCODE_CTX* ctx,
                 unsigned char*out, int *outl, const unsigned char*in, int inl);
WOLFSSL_API void wolfSSL_EVP_EncodeFinal(WOLFSSL_EVP_ENCODE_CTX* ctx,
                 unsigned char*out, int *outl);
WOLFSSL_API int wolfSSL_EVP_EncodeBlock(unsigned char *out,
                const unsigned char *in, int inLen);
WOLFSSL_API int wolfSSL_EVP_DecodeBlock(unsigned char *out,
                const unsigned char *in, int inLen);
#endif /* WOLFSSL_BASE64_ENCODE */

#if defined(WOLFSSL_BASE64_DECODE)
WOLFSSL_API void wolfSSL_EVP_DecodeInit(WOLFSSL_EVP_ENCODE_CTX* ctx);
WOLFSSL_API int  wolfSSL_EVP_DecodeUpdate(WOLFSSL_EVP_ENCODE_CTX* ctx,
                unsigned char*out, int *outl, const unsigned char*in, int inl);
WOLFSSL_API int wolfSSL_EVP_DecodeFinal(WOLFSSL_EVP_ENCODE_CTX* ctx,
                unsigned char*out, int *outl);
#endif /* WOLFSSL_BASE64_DECODE */

WOLFSSL_API const WOLFSSL_EVP_MD* wolfSSL_EVP_blake2b512(void);
WOLFSSL_API const WOLFSSL_EVP_MD* wolfSSL_EVP_blake2s256(void);

WOLFSSL_API void wolfSSL_EVP_init(void);
WOLFSSL_API int  wolfSSL_EVP_MD_size(const WOLFSSL_EVP_MD* type);
WOLFSSL_API int  wolfSSL_EVP_MD_type(const WOLFSSL_EVP_MD* type);
WOLFSSL_API int  wolfSSL_EVP_MD_block_size(const WOLFSSL_EVP_MD* type);
WOLFSSL_API int  wolfSSL_EVP_MD_pkey_type(const WOLFSSL_EVP_MD* type);

WOLFSSL_API WOLFSSL_EVP_MD_CTX *wolfSSL_EVP_MD_CTX_new (void);
WOLFSSL_API void                wolfSSL_EVP_MD_CTX_free(WOLFSSL_EVP_MD_CTX* ctx);
WOLFSSL_API void wolfSSL_EVP_MD_CTX_init(WOLFSSL_EVP_MD_CTX* ctx);
WOLFSSL_API int  wolfSSL_EVP_MD_CTX_cleanup(WOLFSSL_EVP_MD_CTX* ctx);
WOLFSSL_API int  wolfSSL_EVP_MD_CTX_copy(WOLFSSL_EVP_MD_CTX *out, const WOLFSSL_EVP_MD_CTX *in);
WOLFSSL_API int  wolfSSL_EVP_MD_CTX_copy_ex(WOLFSSL_EVP_MD_CTX *out, const WOLFSSL_EVP_MD_CTX *in);
WOLFSSL_API int  wolfSSL_EVP_MD_CTX_type(const WOLFSSL_EVP_MD_CTX *ctx);
WOLFSSL_API int  wolfSSL_EVP_MD_CTX_size(const WOLFSSL_EVP_MD_CTX *ctx);
WOLFSSL_API int  wolfSSL_EVP_MD_CTX_block_size(const WOLFSSL_EVP_MD_CTX *ctx);
WOLFSSL_API const WOLFSSL_EVP_MD *wolfSSL_EVP_MD_CTX_md(const WOLFSSL_EVP_MD_CTX *ctx);
WOLFSSL_API const WOLFSSL_EVP_CIPHER *wolfSSL_EVP_get_cipherbyname(const char *name);
WOLFSSL_API const WOLFSSL_EVP_MD     *wolfSSL_EVP_get_digestbyname(const char *name);
WOLFSSL_API int wolfSSL_EVP_CIPHER_nid(const WOLFSSL_EVP_CIPHER *cipher);

WOLFSSL_API int wolfSSL_EVP_DigestInit(WOLFSSL_EVP_MD_CTX* ctx,
                                     const WOLFSSL_EVP_MD* type);
WOLFSSL_API int wolfSSL_EVP_DigestUpdate(WOLFSSL_EVP_MD_CTX* ctx, const void* data,
                                       size_t sz);
WOLFSSL_API int wolfSSL_EVP_DigestFinal(WOLFSSL_EVP_MD_CTX* ctx, unsigned char* md,
                                      unsigned int* s);
WOLFSSL_API int wolfSSL_EVP_DigestFinal_ex(WOLFSSL_EVP_MD_CTX* ctx,
                                            unsigned char* md, unsigned int* s);
WOLFSSL_API int wolfSSL_EVP_DigestSignUpdate(WOLFSSL_EVP_MD_CTX *ctx,
                                             const void *d, unsigned int cnt);
WOLFSSL_API int wolfSSL_EVP_DigestSignFinal(WOLFSSL_EVP_MD_CTX *ctx,
                                            unsigned char *sig, size_t *siglen);
WOLFSSL_API int wolfSSL_EVP_DigestVerifyUpdate(WOLFSSL_EVP_MD_CTX *ctx,
                                               const void *d, size_t cnt);
WOLFSSL_API int wolfSSL_EVP_DigestVerifyFinal(WOLFSSL_EVP_MD_CTX *ctx,
                                              const unsigned char *sig,
                                              size_t siglen);

WOLFSSL_API int wolfSSL_EVP_BytesToKey(const WOLFSSL_EVP_CIPHER* type,
                       const WOLFSSL_EVP_MD* md, const byte* salt,
                       const byte* data, int sz, int count, byte* key, byte* iv);

WOLFSSL_API void wolfSSL_EVP_CIPHER_CTX_init(WOLFSSL_EVP_CIPHER_CTX* ctx);
WOLFSSL_API int  wolfSSL_EVP_CIPHER_CTX_cleanup(WOLFSSL_EVP_CIPHER_CTX* ctx);
WOLFSSL_API int  wolfSSL_EVP_CIPHER_CTX_ctrl(WOLFSSL_EVP_CIPHER_CTX *ctx, \
                                             int type, int arg, void *ptr);
WOLFSSL_API int  wolfSSL_EVP_CIPHER_CTX_iv_length(
                                    const WOLFSSL_EVP_CIPHER_CTX* ctx);
WOLFSSL_API int  wolfSSL_EVP_CIPHER_iv_length(const WOLFSSL_EVP_CIPHER* cipher);
WOLFSSL_API int wolfSSL_EVP_Cipher_key_length(const WOLFSSL_EVP_CIPHER* c);


WOLFSSL_API int  wolfSSL_EVP_CipherInit(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                    const WOLFSSL_EVP_CIPHER* type,
                                    const unsigned char* key,
                                    const unsigned char* iv,
                                    int enc);
WOLFSSL_API int wolfSSL_EVP_CipherUpdate(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl,
                                   const unsigned char *in, int inl);
WOLFSSL_API int  wolfSSL_EVP_CipherFinal(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl);
WOLFSSL_API int  wolfSSL_EVP_CipherFinal_ex(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl, int enc);
WOLFSSL_API int  wolfSSL_EVP_EncryptFinal(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl);
WOLFSSL_API int  wolfSSL_EVP_EncryptFinal_ex(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl);
WOLFSSL_API int  wolfSSL_EVP_DecryptFinal(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl);
WOLFSSL_API int  wolfSSL_EVP_DecryptFinal_ex(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl);
WOLFSSL_API int  wolfSSL_EVP_DecryptFinal_legacy(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl);

WOLFSSL_API WOLFSSL_EVP_CIPHER_CTX *wolfSSL_EVP_CIPHER_CTX_new(void);
WOLFSSL_API void wolfSSL_EVP_CIPHER_CTX_free(WOLFSSL_EVP_CIPHER_CTX *ctx);
WOLFSSL_API int  wolfSSL_EVP_CIPHER_CTX_reset(WOLFSSL_EVP_CIPHER_CTX *ctx);
WOLFSSL_API int  wolfSSL_EVP_CIPHER_CTX_nid(const WOLFSSL_EVP_CIPHER_CTX *ctx);
WOLFSSL_API int  wolfSSL_EVP_CIPHER_CTX_key_length(WOLFSSL_EVP_CIPHER_CTX* ctx);
WOLFSSL_API int  wolfSSL_EVP_CIPHER_CTX_set_key_length(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                                     int keylen);
WOLFSSL_API int  wolfSSL_EVP_CIPHER_CTX_set_iv_length(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                                     int ivLen);
WOLFSSL_API int  wolfSSL_EVP_CIPHER_CTX_set_iv(WOLFSSL_EVP_CIPHER_CTX* ctx, byte* iv,
                                                     int ivLen);
WOLFSSL_API int  wolfSSL_EVP_CIPHER_CTX_get_iv(WOLFSSL_EVP_CIPHER_CTX* ctx, byte* iv,
                                                     int ivLen);
WOLFSSL_API int  wolfSSL_EVP_Cipher(WOLFSSL_EVP_CIPHER_CTX* ctx,
                          unsigned char* dst, unsigned char* src,
                          unsigned int len);

WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_get_cipherbynid(int id);
WOLFSSL_API const WOLFSSL_EVP_MD* wolfSSL_EVP_get_digestbynid(int id);
WOLFSSL_API const WOLFSSL_EVP_CIPHER *wolfSSL_EVP_CIPHER_CTX_cipher(const WOLFSSL_EVP_CIPHER_CTX *ctx);

WOLFSSL_API int wolfSSL_EVP_PKEY_assign_RSA(WOLFSSL_EVP_PKEY* pkey,
                                            WOLFSSL_RSA* key);
WOLFSSL_API int wolfSSL_EVP_PKEY_assign_EC_KEY(WOLFSSL_EVP_PKEY* pkey,
                                               WOLFSSL_EC_KEY* key);
WOLFSSL_API int wolfSSL_EVP_PKEY_assign_DSA(WOLFSSL_EVP_PKEY* pkey, WOLFSSL_DSA* key);
WOLFSSL_API int wolfSSL_EVP_PKEY_assign_DH(WOLFSSL_EVP_PKEY* pkey, WOLFSSL_DH* key);
WOLFSSL_API WOLFSSL_RSA* wolfSSL_EVP_PKEY_get0_RSA(WOLFSSL_EVP_PKEY *pkey);
WOLFSSL_API WOLFSSL_DSA* wolfSSL_EVP_PKEY_get0_DSA(WOLFSSL_EVP_PKEY *pkey);
WOLFSSL_API WOLFSSL_RSA* wolfSSL_EVP_PKEY_get1_RSA(WOLFSSL_EVP_PKEY* key);
WOLFSSL_API WOLFSSL_DSA* wolfSSL_EVP_PKEY_get1_DSA(WOLFSSL_EVP_PKEY* key);
WOLFSSL_API WOLFSSL_EC_KEY *wolfSSL_EVP_PKEY_get0_EC_KEY(WOLFSSL_EVP_PKEY *pkey);
WOLFSSL_API WOLFSSL_EC_KEY *wolfSSL_EVP_PKEY_get1_EC_KEY(WOLFSSL_EVP_PKEY *key);
WOLFSSL_API WOLFSSL_DH* wolfSSL_EVP_PKEY_get0_DH(WOLFSSL_EVP_PKEY* key);
WOLFSSL_API WOLFSSL_DH* wolfSSL_EVP_PKEY_get1_DH(WOLFSSL_EVP_PKEY* key);
WOLFSSL_API int wolfSSL_EVP_PKEY_set1_RSA(WOLFSSL_EVP_PKEY *pkey, WOLFSSL_RSA *key);
WOLFSSL_API int wolfSSL_EVP_PKEY_set1_DSA(WOLFSSL_EVP_PKEY *pkey, WOLFSSL_DSA *key);
WOLFSSL_API int wolfSSL_EVP_PKEY_set1_DH(WOLFSSL_EVP_PKEY *pkey, WOLFSSL_DH *key);
WOLFSSL_API int wolfSSL_EVP_PKEY_set1_EC_KEY(WOLFSSL_EVP_PKEY *pkey, WOLFSSL_EC_KEY *key);
WOLFSSL_API int wolfSSL_EVP_PKEY_assign(WOLFSSL_EVP_PKEY *pkey, int type, void *key);

WOLFSSL_API const unsigned char* wolfSSL_EVP_PKEY_get0_hmac(const WOLFSSL_EVP_PKEY* pkey,
        size_t* len);
WOLFSSL_API int wolfSSL_EVP_PKEY_sign_init(WOLFSSL_EVP_PKEY_CTX *ctx);
WOLFSSL_API int wolfSSL_EVP_PKEY_sign(WOLFSSL_EVP_PKEY_CTX *ctx,
  unsigned char *sig, size_t *siglen, const unsigned char *tbs, size_t tbslen);
WOLFSSL_API int wolfSSL_EVP_PKEY_verify_init(WOLFSSL_EVP_PKEY_CTX *ctx);
WOLFSSL_API int wolfSSL_EVP_PKEY_verify(WOLFSSL_EVP_PKEY_CTX *ctx, const unsigned char *sig,
  size_t siglen, const unsigned char *tbs, size_t tbslen);
WOLFSSL_API int wolfSSL_EVP_PKEY_paramgen_init(WOLFSSL_EVP_PKEY_CTX *ctx);
WOLFSSL_API int wolfSSL_EVP_PKEY_CTX_set_ec_paramgen_curve_nid(WOLFSSL_EVP_PKEY_CTX *ctx,
        int nid);
WOLFSSL_API int wolfSSL_EVP_PKEY_paramgen(WOLFSSL_EVP_PKEY_CTX* ctx,
                                          WOLFSSL_EVP_PKEY** pkey);
WOLFSSL_API int wolfSSL_EVP_PKEY_CTX_set_ec_param_enc(WOLFSSL_EVP_PKEY_CTX *ctx,
        int flag);
WOLFSSL_API int wolfSSL_EVP_PKEY_keygen_init(WOLFSSL_EVP_PKEY_CTX *ctx);
WOLFSSL_API int wolfSSL_EVP_PKEY_keygen(WOLFSSL_EVP_PKEY_CTX *ctx,
  WOLFSSL_EVP_PKEY **ppkey);
WOLFSSL_API int wolfSSL_EVP_PKEY_bits(const WOLFSSL_EVP_PKEY *pkey);
#if defined(OPENSSL_VERSION_NUMBER) && OPENSSL_VERSION_NUMBER >= 0x10100000L
WOLFSSL_API void wolfSSL_EVP_PKEY_CTX_free(WOLFSSL_EVP_PKEY_CTX *ctx);
#else
WOLFSSL_API int wolfSSL_EVP_PKEY_CTX_free(WOLFSSL_EVP_PKEY_CTX *ctx);
#endif
WOLFSSL_API int wolfSSL_EVP_PKEY_CTX_set_rsa_padding(WOLFSSL_EVP_PKEY_CTX *ctx, int padding);
WOLFSSL_API int wolfSSL_EVP_PKEY_CTX_set_signature_md(WOLFSSL_EVP_PKEY_CTX *ctx,
    const WOLFSSL_EVP_MD* md);
WOLFSSL_API int wolfSSL_EVP_PKEY_CTX_set_rsa_keygen_bits(WOLFSSL_EVP_PKEY_CTX *ctx, int bits);

WOLFSSL_API int wolfSSL_EVP_PKEY_derive_init(WOLFSSL_EVP_PKEY_CTX *ctx);
WOLFSSL_API int wolfSSL_EVP_PKEY_derive_set_peer(WOLFSSL_EVP_PKEY_CTX *ctx, WOLFSSL_EVP_PKEY *peer);
WOLFSSL_API int wolfSSL_EVP_PKEY_derive(WOLFSSL_EVP_PKEY_CTX *ctx, unsigned char *key, size_t *keylen);

WOLFSSL_API int wolfSSL_EVP_PKEY_CTX_ctrl_str(WOLFSSL_EVP_PKEY_CTX *ctx,
                          const char *name, const char *value);

WOLFSSL_API int wolfSSL_EVP_PKEY_decrypt(WOLFSSL_EVP_PKEY_CTX *ctx,
                     unsigned char *out, size_t *outlen,
                     const unsigned char *in, size_t inlen);
WOLFSSL_API int wolfSSL_EVP_PKEY_decrypt_init(WOLFSSL_EVP_PKEY_CTX *ctx);
WOLFSSL_API int wolfSSL_EVP_PKEY_encrypt(WOLFSSL_EVP_PKEY_CTX *ctx,
                     unsigned char *out, size_t *outlen,
                     const unsigned char *in, size_t inlen);
WOLFSSL_API int wolfSSL_EVP_PKEY_encrypt_init(WOLFSSL_EVP_PKEY_CTX *ctx);
WOLFSSL_API WOLFSSL_EVP_PKEY *wolfSSL_EVP_PKEY_new(void);
WOLFSSL_API WOLFSSL_EVP_PKEY* wolfSSL_EVP_PKEY_new_ex(void* heap);
WOLFSSL_API void wolfSSL_EVP_PKEY_free(WOLFSSL_EVP_PKEY* key);
WOLFSSL_API int wolfSSL_EVP_PKEY_size(WOLFSSL_EVP_PKEY *pkey);
WOLFSSL_API int wolfSSL_EVP_PKEY_copy_parameters(WOLFSSL_EVP_PKEY *to, const WOLFSSL_EVP_PKEY *from);
WOLFSSL_API int wolfSSL_EVP_PKEY_missing_parameters(WOLFSSL_EVP_PKEY *pkey);
WOLFSSL_API int wolfSSL_EVP_PKEY_cmp(const WOLFSSL_EVP_PKEY *a, const WOLFSSL_EVP_PKEY *b);
WOLFSSL_API int wolfSSL_EVP_PKEY_type(int type);
WOLFSSL_API int wolfSSL_EVP_PKEY_id(const WOLFSSL_EVP_PKEY *pkey);
WOLFSSL_API int wolfSSL_EVP_PKEY_base_id(const WOLFSSL_EVP_PKEY *pkey);
WOLFSSL_API int wolfSSL_EVP_PKEY_get_default_digest_nid(WOLFSSL_EVP_PKEY *pkey, int *pnid);
WOLFSSL_API WOLFSSL_EVP_PKEY* wolfSSL_EVP_PKCS82PKEY(const WOLFSSL_PKCS8_PRIV_KEY_INFO* p8);
WOLFSSL_API WOLFSSL_PKCS8_PRIV_KEY_INFO* wolfSSL_EVP_PKEY2PKCS8(const WOLFSSL_EVP_PKEY* pkey);

WOLFSSL_API int wolfSSL_EVP_SignFinal(WOLFSSL_EVP_MD_CTX *ctx, unsigned char *sigret,
                  unsigned int *siglen, WOLFSSL_EVP_PKEY *pkey);
WOLFSSL_API int wolfSSL_EVP_SignInit(WOLFSSL_EVP_MD_CTX *ctx, const WOLFSSL_EVP_MD *type);
WOLFSSL_API int wolfSSL_EVP_SignUpdate(WOLFSSL_EVP_MD_CTX *ctx, const void *data, size_t len);
WOLFSSL_API int wolfSSL_EVP_VerifyFinal(WOLFSSL_EVP_MD_CTX *ctx,
        const unsigned char* sig, unsigned int sig_len, WOLFSSL_EVP_PKEY *pkey);
WOLFSSL_API int wolfSSL_EVP_VerifyInit(WOLFSSL_EVP_MD_CTX *ctx, const WOLFSSL_EVP_MD *type);
WOLFSSL_API int wolfSSL_EVP_VerifyUpdate(WOLFSSL_EVP_MD_CTX *ctx, const void *data, size_t len);


/* these next ones don't need real OpenSSL type, for OpenSSH compat only */
WOLFSSL_API void* wolfSSL_EVP_X_STATE(const WOLFSSL_EVP_CIPHER_CTX* ctx);
WOLFSSL_API int   wolfSSL_EVP_X_STATE_LEN(const WOLFSSL_EVP_CIPHER_CTX* ctx);

WOLFSSL_API void  wolfSSL_3des_iv(WOLFSSL_EVP_CIPHER_CTX* ctx, int doset,
                                unsigned char* iv, int len);
WOLFSSL_API void  wolfSSL_aes_ctr_iv(WOLFSSL_EVP_CIPHER_CTX* ctx, int doset,
                                unsigned char* iv, int len);

WOLFSSL_API int  wolfSSL_StoreExternalIV(WOLFSSL_EVP_CIPHER_CTX* ctx);
WOLFSSL_API int  wolfSSL_SetInternalIV(WOLFSSL_EVP_CIPHER_CTX* ctx);

WOLFSSL_API int wolfSSL_EVP_CIPHER_CTX_block_size(const WOLFSSL_EVP_CIPHER_CTX *ctx);
WOLFSSL_API int wolfSSL_EVP_CIPHER_block_size(const WOLFSSL_EVP_CIPHER *cipher);
WOLFSSL_API unsigned long WOLFSSL_EVP_CIPHER_mode(const WOLFSSL_EVP_CIPHER *cipher);
WOLFSSL_API unsigned long WOLFSSL_CIPHER_mode(const WOLFSSL_EVP_CIPHER *cipher);
WOLFSSL_API unsigned long wolfSSL_EVP_CIPHER_flags(const WOLFSSL_EVP_CIPHER *cipher);
WOLFSSL_API void wolfSSL_EVP_CIPHER_CTX_set_flags(WOLFSSL_EVP_CIPHER_CTX *ctx, int flags);
WOLFSSL_API void wolfSSL_EVP_CIPHER_CTX_clear_flags(WOLFSSL_EVP_CIPHER_CTX *ctx, int flags);
WOLFSSL_API unsigned long wolfSSL_EVP_CIPHER_CTX_flags(const WOLFSSL_EVP_CIPHER_CTX *ctx);
WOLFSSL_API unsigned long wolfSSL_EVP_CIPHER_CTX_mode(const WOLFSSL_EVP_CIPHER_CTX *ctx);
WOLFSSL_API int  wolfSSL_EVP_CIPHER_CTX_set_padding(WOLFSSL_EVP_CIPHER_CTX *c, int pad);
WOLFSSL_API int  wolfSSL_EVP_add_digest(const WOLFSSL_EVP_MD *digest);
WOLFSSL_API int  wolfSSL_EVP_add_cipher(const WOLFSSL_EVP_CIPHER *cipher);
WOLFSSL_API void wolfSSL_EVP_cleanup(void);
WOLFSSL_API int  wolfSSL_add_all_algorithms(void);
WOLFSSL_API int  wolfSSL_OpenSSL_add_all_algorithms_conf(void);
WOLFSSL_API int  wolfSSL_OpenSSL_add_all_algorithms_noconf(void);
WOLFSSL_API int wolfSSL_EVP_read_pw_string(char*, int, const char*, int);

WOLFSSL_API int wolfSSL_PKCS5_PBKDF2_HMAC_SHA1(const char * pass, int passlen,
                                               const unsigned char * salt,
                                               int saltlen, int iter,
                                               int keylen, unsigned char *out);

WOLFSSL_API int wolfSSL_PKCS5_PBKDF2_HMAC(const char *pass, int passlen,
                                           const unsigned char *salt,
                                           int saltlen, int iter,
                                           const WOLFSSL_EVP_MD *digest,
                                           int keylen, unsigned char *out);

#if defined(HAVE_SCRYPT) && defined(HAVE_PBKDF2) && !defined(NO_PWDBASED) && \
                                                    !defined(NO_SHA256)
WOLFSSL_API int wolfSSL_EVP_PBE_scrypt(const char *pass, size_t passlen,
                            const unsigned char *salt, size_t saltlen,
                            word64 N, word64 r, word64 p,
                            word64 maxmem, unsigned char *key, size_t keylen);
#endif /* HAVE_SCRYPT && HAVE_PBKDF2 && !NO_PWDBASED && !NO_SHA256 */

WOLFSSL_LOCAL int wolfSSL_EVP_get_hashinfo(const WOLFSSL_EVP_MD* evp,
                                           int* pHash, int* pHashSz);

WOLFSSL_API void wolfSSL_EVP_MD_do_all(void (*fn) (const WOLFSSL_EVP_MD *md,
                                                   const char* from, const char* to,
                                                   void* xx), void* args);

#ifdef HAVE_HKDF
WOLFSSL_API int wolfSSL_EVP_PKEY_CTX_set_hkdf_md(WOLFSSL_EVP_PKEY_CTX* ctx,
                                                 const WOLFSSL_EVP_MD* md);
WOLFSSL_API int wolfSSL_EVP_PKEY_CTX_set1_hkdf_salt(WOLFSSL_EVP_PKEY_CTX* ctx,
                                                    const byte* salt,
                                                    int saltSz);
WOLFSSL_API int wolfSSL_EVP_PKEY_CTX_set1_hkdf_key(WOLFSSL_EVP_PKEY_CTX* ctx,
                                                   const byte* key, int keySz);
WOLFSSL_API int wolfSSL_EVP_PKEY_CTX_add1_hkdf_info(WOLFSSL_EVP_PKEY_CTX* ctx,
                                                    const byte* info,
                                                    int infoSz);
WOLFSSL_API int wolfSSL_EVP_PKEY_CTX_hkdf_mode(WOLFSSL_EVP_PKEY_CTX* ctx,
                                               int mode);
#endif

#define WOLFSSL_EVP_CIPH_MODE            0x0007
#define WOLFSSL_EVP_CIPH_STREAM_CIPHER      0x0
#define WOLFSSL_EVP_CIPH_ECB_MODE           0x1
#define WOLFSSL_EVP_CIPH_CBC_MODE           0x2
#define WOLFSSL_EVP_CIPH_CFB_MODE           0x3
#define WOLFSSL_EVP_CIPH_OFB_MODE           0x4
#define WOLFSSL_EVP_CIPH_CTR_MODE           0x5
#define WOLFSSL_EVP_CIPH_GCM_MODE           0x6
#define WOLFSSL_EVP_CIPH_CCM_MODE           0x7
#define WOLFSSL_EVP_CIPH_XTS_MODE          0x10
#define WOLFSSL_EVP_CIPH_FLAG_AEAD_CIPHER  0x20
#define WOLFSSL_EVP_CIPH_NO_PADDING       0x100
#define WOLFSSL_EVP_CIPH_VARIABLE_LENGTH  0x200
#define WOLFSSL_EVP_CIPH_TYPE_INIT         0xff


#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)

/* EVP ENGINE API's */
WOLFSSL_API WOLFSSL_EVP_PKEY* wolfSSL_EVP_PKEY_new_mac_key(int type, WOLFSSL_ENGINE* e,
                                          const unsigned char* key, int keylen);

WOLFSSL_API WOLFSSL_EVP_PKEY* wolfSSL_EVP_PKEY_new_CMAC_key(WOLFSSL_ENGINE* e,
                                          const unsigned char* priv, size_t len,
                                          const WOLFSSL_EVP_CIPHER* cipher);

WOLFSSL_API int wolfSSL_EVP_DigestInit_ex(WOLFSSL_EVP_MD_CTX* ctx,
                                     const WOLFSSL_EVP_MD* type,
                                     WOLFSSL_ENGINE *impl);

WOLFSSL_API int wolfSSL_EVP_DigestSignInit(WOLFSSL_EVP_MD_CTX *ctx,
                                           WOLFSSL_EVP_PKEY_CTX **pctx,
                                           const WOLFSSL_EVP_MD *type,
                                           WOLFSSL_ENGINE *e,
                                           WOLFSSL_EVP_PKEY *pkey);
WOLFSSL_API int wolfSSL_EVP_DigestVerifyInit(WOLFSSL_EVP_MD_CTX *ctx,
                                             WOLFSSL_EVP_PKEY_CTX **pctx,
                                             const WOLFSSL_EVP_MD *type,
                                             WOLFSSL_ENGINE *e,
                                             WOLFSSL_EVP_PKEY *pkey);
WOLFSSL_API int wolfSSL_EVP_Digest(const unsigned char* in, int inSz, unsigned char* out,
                              unsigned int* outSz, const WOLFSSL_EVP_MD* evp,
                              WOLFSSL_ENGINE* eng);
WOLFSSL_API int  wolfSSL_EVP_CipherInit_ex(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                    const WOLFSSL_EVP_CIPHER* type,
                                    WOLFSSL_ENGINE *impl,
                                    const unsigned char* key,
                                    const unsigned char* iv,
                                    int enc);
WOLFSSL_API int  wolfSSL_EVP_EncryptInit(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                    const WOLFSSL_EVP_CIPHER* type,
                                    const unsigned char* key,
                                    const unsigned char* iv);
WOLFSSL_API int  wolfSSL_EVP_EncryptInit_ex(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                    const WOLFSSL_EVP_CIPHER* type,
                                    WOLFSSL_ENGINE *impl,
                                    const unsigned char* key,
                                    const unsigned char* iv);
WOLFSSL_API int  wolfSSL_EVP_DecryptInit(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                    const WOLFSSL_EVP_CIPHER* type,
                                    const unsigned char* key,
                                    const unsigned char* iv);
WOLFSSL_API int  wolfSSL_EVP_DecryptInit_ex(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                    const WOLFSSL_EVP_CIPHER* type,
                                    WOLFSSL_ENGINE *impl,
                                    const unsigned char* key,
                                    const unsigned char* iv);
WOLFSSL_API WOLFSSL_EVP_PKEY_CTX *wolfSSL_EVP_PKEY_CTX_new(WOLFSSL_EVP_PKEY *pkey, WOLFSSL_ENGINE *e);
WOLFSSL_API WOLFSSL_EVP_PKEY_CTX *wolfSSL_EVP_PKEY_CTX_new_id(int id, WOLFSSL_ENGINE *e);
WOLFSSL_API int wolfSSL_EVP_SignInit_ex(WOLFSSL_EVP_MD_CTX* ctx,
                                     const WOLFSSL_EVP_MD* type,
                                     WOLFSSL_ENGINE *impl);

#define EVP_CIPH_STREAM_CIPHER    WOLFSSL_EVP_CIPH_STREAM_CIPHER
#define EVP_CIPH_VARIABLE_LENGTH  WOLFSSL_EVP_CIPH_VARIABLE_LENGTH
#define EVP_CIPH_ECB_MODE         WOLFSSL_EVP_CIPH_ECB_MODE
#define EVP_CIPH_CBC_MODE         WOLFSSL_EVP_CIPH_CBC_MODE
#define EVP_CIPH_CFB_MODE         WOLFSSL_EVP_CIPH_CFB_MODE
#define EVP_CIPH_OFB_MODE         WOLFSSL_EVP_CIPH_OFB_MODE
#define EVP_CIPH_CTR_MODE         WOLFSSL_EVP_CIPH_CTR_MODE
#define EVP_CIPH_GCM_MODE         WOLFSSL_EVP_CIPH_GCM_MODE
#define EVP_CIPH_CCM_MODE         WOLFSSL_EVP_CIPH_CCM_MODE
#define EVP_CIPH_XTS_MODE         WOLFSSL_EVP_CIPH_XTS_MODE

#define EVP_CIPH_FLAG_AEAD_CIPHER WOLFSSL_EVP_CIPH_FLAG_AEAD_CIPHER

#ifndef NO_MD4
    #define EVP_md4       wolfSSL_EVP_md4
#endif
#ifndef NO_MD5
    #define EVP_md5       wolfSSL_EVP_md5
#endif
#define EVP_sha1          wolfSSL_EVP_sha1
#define EVP_mdc2          wolfSSL_EVP_mdc2
#define EVP_dds1          wolfSSL_EVP_sha1
#define EVP_sha224        wolfSSL_EVP_sha224
#define EVP_sha256        wolfSSL_EVP_sha256
#define EVP_sha384        wolfSSL_EVP_sha384
#define EVP_sha512        wolfSSL_EVP_sha512
#define EVP_sha512_224    wolfSSL_EVP_sha512_224
#define EVP_sha512_256    wolfSSL_EVP_sha512_256
#define EVP_ripemd160     wolfSSL_EVP_ripemd160
#define EVP_shake128      wolfSSL_EVP_shake128
#define EVP_shake256      wolfSSL_EVP_shake256
#define EVP_sm3           wolfSSL_EVP_sm3
#define EVP_set_pw_prompt wolfSSL_EVP_set_pw_prompt

#define EVP_sha3_224    wolfSSL_EVP_sha3_224
#define EVP_sha3_256    wolfSSL_EVP_sha3_256
#define EVP_sha3_384    wolfSSL_EVP_sha3_384
#define EVP_sha3_512    wolfSSL_EVP_sha3_512

#define EVP_aes_128_cbc       wolfSSL_EVP_aes_128_cbc
#define EVP_aes_192_cbc       wolfSSL_EVP_aes_192_cbc
#define EVP_aes_256_cbc       wolfSSL_EVP_aes_256_cbc
#define EVP_aes_128_cfb1      wolfSSL_EVP_aes_128_cfb1
#define EVP_aes_192_cfb1      wolfSSL_EVP_aes_192_cfb1
#define EVP_aes_256_cfb1      wolfSSL_EVP_aes_256_cfb1
#define EVP_aes_128_cfb8      wolfSSL_EVP_aes_128_cfb8
#define EVP_aes_192_cfb8      wolfSSL_EVP_aes_192_cfb8
#define EVP_aes_256_cfb8      wolfSSL_EVP_aes_256_cfb8
#define EVP_aes_128_cfb128    wolfSSL_EVP_aes_128_cfb128
#define EVP_aes_192_cfb128    wolfSSL_EVP_aes_192_cfb128
#define EVP_aes_256_cfb128    wolfSSL_EVP_aes_256_cfb128
#define EVP_aes_128_cfb       wolfSSL_EVP_aes_128_cfb128
#define EVP_aes_192_cfb       wolfSSL_EVP_aes_192_cfb128
#define EVP_aes_256_cfb       wolfSSL_EVP_aes_256_cfb128
#define EVP_aes_128_ofb       wolfSSL_EVP_aes_128_ofb
#define EVP_aes_192_ofb       wolfSSL_EVP_aes_192_ofb
#define EVP_aes_256_ofb       wolfSSL_EVP_aes_256_ofb
#define EVP_aes_128_xts       wolfSSL_EVP_aes_128_xts
#define EVP_aes_256_xts       wolfSSL_EVP_aes_256_xts
#define EVP_aes_128_gcm       wolfSSL_EVP_aes_128_gcm
#define EVP_aes_192_gcm       wolfSSL_EVP_aes_192_gcm
#define EVP_aes_256_gcm       wolfSSL_EVP_aes_256_gcm
#define EVP_aes_128_ccm       wolfSSL_EVP_aes_128_ccm
#define EVP_aes_192_ccm       wolfSSL_EVP_aes_192_ccm
#define EVP_aes_256_ccm       wolfSSL_EVP_aes_256_ccm
#define EVP_aes_128_ecb       wolfSSL_EVP_aes_128_ecb
#define EVP_aes_192_ecb       wolfSSL_EVP_aes_192_ecb
#define EVP_aes_256_ecb       wolfSSL_EVP_aes_256_ecb
#define EVP_aes_128_ctr       wolfSSL_EVP_aes_128_ctr
#define EVP_aes_192_ctr       wolfSSL_EVP_aes_192_ctr
#define EVP_aes_256_ctr       wolfSSL_EVP_aes_256_ctr
#define EVP_des_cbc           wolfSSL_EVP_des_cbc
#define EVP_des_ecb           wolfSSL_EVP_des_ecb
#define EVP_des_ede3_cbc      wolfSSL_EVP_des_ede3_cbc
#define EVP_des_ede3_ecb      wolfSSL_EVP_des_ede3_ecb
#define EVP_rc4               wolfSSL_EVP_rc4
#define EVP_chacha20          wolfSSL_EVP_chacha20
#define EVP_chacha20_poly1305 wolfSSL_EVP_chacha20_poly1305
#define EVP_aria_128_gcm      wolfSSL_EVP_aria_128_gcm
#define EVP_aria_192_gcm      wolfSSL_EVP_aria_192_gcm
#define EVP_aria_256_gcm      wolfSSL_EVP_aria_256_gcm
#define EVP_sm4_ecb           wolfSSL_EVP_sm4_ecb
#define EVP_sm4_cbc           wolfSSL_EVP_sm4_cbc
#define EVP_sm4_ctr           wolfSSL_EVP_sm4_ctr
#define EVP_sm4_gcm           wolfSSL_EVP_sm4_gcm
#define EVP_sm4_ccm           wolfSSL_EVP_sm4_ccm
#define EVP_enc_null          wolfSSL_EVP_enc_null

#define EVP_MD_size             wolfSSL_EVP_MD_size
#define EVP_MD_pkey_type        wolfSSL_EVP_MD_pkey_type
#define EVP_MD_CTX_new          wolfSSL_EVP_MD_CTX_new
#define EVP_MD_CTX_create       wolfSSL_EVP_MD_CTX_new
#define EVP_MD_CTX_free         wolfSSL_EVP_MD_CTX_free
#define EVP_MD_CTX_destroy      wolfSSL_EVP_MD_CTX_free
#define EVP_MD_CTX_init         wolfSSL_EVP_MD_CTX_init
#define EVP_MD_CTX_cleanup      wolfSSL_EVP_MD_CTX_cleanup
#define EVP_MD_CTX_reset        wolfSSL_EVP_MD_CTX_cleanup
#define EVP_MD_CTX_md           wolfSSL_EVP_MD_CTX_md
#define EVP_MD_CTX_type         wolfSSL_EVP_MD_CTX_type
#define EVP_MD_CTX_size         wolfSSL_EVP_MD_CTX_size
#define EVP_MD_CTX_block_size   wolfSSL_EVP_MD_CTX_block_size
#define EVP_MD_block_size       wolfSSL_EVP_MD_block_size
#define EVP_MD_type             wolfSSL_EVP_MD_type
#ifndef NO_WOLFSSL_STUB
#define EVP_MD_CTX_set_flags(...) WC_DO_NOTHING
#endif

#define EVP_Digest             wolfSSL_EVP_Digest
#define EVP_DigestInit         wolfSSL_EVP_DigestInit
#define EVP_DigestInit_ex      wolfSSL_EVP_DigestInit_ex
#define EVP_DigestUpdate       wolfSSL_EVP_DigestUpdate
#define EVP_DigestFinal        wolfSSL_EVP_DigestFinal
#define EVP_DigestFinal_ex     wolfSSL_EVP_DigestFinal_ex
#define EVP_DigestSignInit     wolfSSL_EVP_DigestSignInit
#define EVP_DigestSignUpdate   wolfSSL_EVP_DigestSignUpdate
#define EVP_DigestSignFinal    wolfSSL_EVP_DigestSignFinal
#define EVP_DigestVerifyInit   wolfSSL_EVP_DigestVerifyInit
#define EVP_DigestVerifyUpdate wolfSSL_EVP_DigestVerifyUpdate
#define EVP_DigestVerifyFinal  wolfSSL_EVP_DigestVerifyFinal
#define EVP_BytesToKey         wolfSSL_EVP_BytesToKey

#define EVP_get_cipherbyname wolfSSL_EVP_get_cipherbyname
#define EVP_get_digestbyname wolfSSL_EVP_get_digestbyname

#define EVP_CIPHER_CTX_init           wolfSSL_EVP_CIPHER_CTX_init
#define EVP_CIPHER_CTX_cleanup        wolfSSL_EVP_CIPHER_CTX_cleanup
#define EVP_CIPHER_CTX_iv_length      wolfSSL_EVP_CIPHER_CTX_iv_length
#define EVP_CIPHER_CTX_nid            wolfSSL_EVP_CIPHER_CTX_nid
#define EVP_CIPHER_CTX_key_length     wolfSSL_EVP_CIPHER_CTX_key_length
#define EVP_CIPHER_CTX_set_key_length wolfSSL_EVP_CIPHER_CTX_set_key_length
#define EVP_CIPHER_CTX_set_iv_length  wolfSSL_EVP_CIPHER_CTX_set_iv_length
#define EVP_CIPHER_CTX_mode           wolfSSL_EVP_CIPHER_CTX_mode
#define EVP_CIPHER_CTX_cipher         wolfSSL_EVP_CIPHER_CTX_cipher

#define EVP_CIPHER_iv_length          wolfSSL_EVP_CIPHER_iv_length
#define EVP_CIPHER_key_length         wolfSSL_EVP_Cipher_key_length

#define EVP_CipherInit                wolfSSL_EVP_CipherInit
#define EVP_CipherInit_ex             wolfSSL_EVP_CipherInit_ex
#define EVP_EncryptInit               wolfSSL_EVP_EncryptInit
#define EVP_EncryptInit_ex            wolfSSL_EVP_EncryptInit_ex
#define EVP_DecryptInit               wolfSSL_EVP_DecryptInit
#define EVP_DecryptInit_ex            wolfSSL_EVP_DecryptInit_ex

#define EVP_Cipher                    wolfSSL_EVP_Cipher
#define EVP_CipherUpdate              wolfSSL_EVP_CipherUpdate
#define EVP_EncryptUpdate             wolfSSL_EVP_CipherUpdate
#define EVP_DecryptUpdate             wolfSSL_EVP_CipherUpdate
#define EVP_CipherFinal               wolfSSL_EVP_CipherFinal
#define EVP_CipherFinal_ex            wolfSSL_EVP_CipherFinal
#define EVP_EncryptFinal              wolfSSL_EVP_CipherFinal
#define EVP_EncryptFinal_ex           wolfSSL_EVP_CipherFinal
#define EVP_DecryptFinal              wolfSSL_EVP_CipherFinal
#define EVP_DecryptFinal_ex           wolfSSL_EVP_CipherFinal

#define EVP_CIPHER_CTX_free           wolfSSL_EVP_CIPHER_CTX_free
#define EVP_CIPHER_CTX_reset          wolfSSL_EVP_CIPHER_CTX_reset
#define EVP_CIPHER_CTX_new            wolfSSL_EVP_CIPHER_CTX_new

#define EVP_get_cipherbynid           wolfSSL_EVP_get_cipherbynid
#define EVP_get_digestbynid           wolfSSL_EVP_get_digestbynid
#define EVP_MD_nid                    wolfSSL_EVP_MD_type

#define EVP_PKEY_assign                wolfSSL_EVP_PKEY_assign
#define EVP_PKEY_assign_RSA            wolfSSL_EVP_PKEY_assign_RSA
#define EVP_PKEY_assign_DSA            wolfSSL_EVP_PKEY_assign_DSA
#define EVP_PKEY_assign_DH             wolfSSL_EVP_PKEY_assign_DH
#define EVP_PKEY_assign_EC_KEY         wolfSSL_EVP_PKEY_assign_EC_KEY
#define EVP_PKEY_get1_DSA              wolfSSL_EVP_PKEY_get1_DSA
#define EVP_PKEY_set1_DSA              wolfSSL_EVP_PKEY_set1_DSA
#define EVP_PKEY_get0_RSA              wolfSSL_EVP_PKEY_get0_RSA
#define EVP_PKEY_get1_RSA              wolfSSL_EVP_PKEY_get1_RSA
#define EVP_PKEY_set1_RSA              wolfSSL_EVP_PKEY_set1_RSA
#define EVP_PKEY_set1_EC_KEY           wolfSSL_EVP_PKEY_set1_EC_KEY
#define EVP_PKEY_get1_EC_KEY           wolfSSL_EVP_PKEY_get1_EC_KEY
#define EVP_PKEY_set1_DH               wolfSSL_EVP_PKEY_set1_DH
#define EVP_PKEY_get0_DH               wolfSSL_EVP_PKEY_get0_DH
#define EVP_PKEY_get1_DH               wolfSSL_EVP_PKEY_get1_DH
#define EVP_PKEY_get0_EC_KEY           wolfSSL_EVP_PKEY_get0_EC_KEY
#define EVP_PKEY_get0_hmac             wolfSSL_EVP_PKEY_get0_hmac
#define EVP_PKEY_new_mac_key           wolfSSL_EVP_PKEY_new_mac_key
#define EVP_PKEY_new_CMAC_key          wolfSSL_EVP_PKEY_new_CMAC_key
#define EVP_MD_CTX_copy                wolfSSL_EVP_MD_CTX_copy
#define EVP_MD_CTX_copy_ex             wolfSSL_EVP_MD_CTX_copy_ex
#define EVP_PKEY_sign_init             wolfSSL_EVP_PKEY_sign_init
#define EVP_PKEY_sign                  wolfSSL_EVP_PKEY_sign
#define EVP_PKEY_verify_init           wolfSSL_EVP_PKEY_verify_init
#define EVP_PKEY_verify                wolfSSL_EVP_PKEY_verify
#define EVP_PKEY_paramgen_init         wolfSSL_EVP_PKEY_paramgen_init
#define EVP_PKEY_CTX_set_ec_param_enc  wolfSSL_EVP_PKEY_CTX_set_ec_param_enc
#define EVP_PKEY_CTX_set_ec_paramgen_curve_nid wolfSSL_EVP_PKEY_CTX_set_ec_paramgen_curve_nid
#define EVP_PKEY_paramgen              wolfSSL_EVP_PKEY_paramgen
#define EVP_PKEY_keygen                wolfSSL_EVP_PKEY_keygen
#define EVP_PKEY_keygen_init           wolfSSL_EVP_PKEY_keygen_init
#define EVP_PKEY_bits                  wolfSSL_EVP_PKEY_bits
#define EVP_PKEY_CTX_free              wolfSSL_EVP_PKEY_CTX_free
#define EVP_PKEY_CTX_new               wolfSSL_EVP_PKEY_CTX_new
#define EVP_PKEY_CTX_set_rsa_padding   wolfSSL_EVP_PKEY_CTX_set_rsa_padding
#define EVP_PKEY_CTX_set_signature_md  wolfSSL_EVP_PKEY_CTX_set_signature_md
#define EVP_PKEY_CTX_new_id            wolfSSL_EVP_PKEY_CTX_new_id
#define EVP_PKEY_CTX_set_rsa_keygen_bits wolfSSL_EVP_PKEY_CTX_set_rsa_keygen_bits
#define EVP_PKEY_derive_init           wolfSSL_EVP_PKEY_derive_init
#define EVP_PKEY_derive_set_peer       wolfSSL_EVP_PKEY_derive_set_peer
#define EVP_PKEY_derive                wolfSSL_EVP_PKEY_derive
#define EVP_PKEY_decrypt               wolfSSL_EVP_PKEY_decrypt
#define EVP_PKEY_decrypt_init          wolfSSL_EVP_PKEY_decrypt_init
#define EVP_PKEY_encrypt               wolfSSL_EVP_PKEY_encrypt
#define EVP_PKEY_encrypt_init          wolfSSL_EVP_PKEY_encrypt_init
#define EVP_PKEY_new                   wolfSSL_EVP_PKEY_new
#define EVP_PKEY_free                  wolfSSL_EVP_PKEY_free
#define EVP_PKEY_up_ref                wolfSSL_EVP_PKEY_up_ref
#define EVP_PKEY_size                  wolfSSL_EVP_PKEY_size
#define EVP_PKEY_copy_parameters       wolfSSL_EVP_PKEY_copy_parameters
#define EVP_PKEY_missing_parameters    wolfSSL_EVP_PKEY_missing_parameters
#define EVP_PKEY_cmp                   wolfSSL_EVP_PKEY_cmp
#define EVP_PKEY_type                  wolfSSL_EVP_PKEY_type
#define EVP_PKEY_base_id               wolfSSL_EVP_PKEY_base_id
#define EVP_PKEY_get_default_digest_nid wolfSSL_EVP_PKEY_get_default_digest_nid
#define EVP_PKEY_id                    wolfSSL_EVP_PKEY_id
#define EVP_PKEY_CTX_ctrl_str          wolfSSL_EVP_PKEY_CTX_ctrl_str
#define EVP_PKCS82PKEY                 wolfSSL_EVP_PKCS82PKEY
#define EVP_PKEY2PKCS8                 wolfSSL_EVP_PKEY2PKCS8
#define EVP_SignFinal                  wolfSSL_EVP_SignFinal
#define EVP_SignInit                   wolfSSL_EVP_SignInit
#define EVP_SignInit_ex                wolfSSL_EVP_SignInit_ex
#define EVP_SignUpdate                 wolfSSL_EVP_SignUpdate
#define EVP_VerifyFinal                wolfSSL_EVP_VerifyFinal
#define EVP_VerifyInit                 wolfSSL_EVP_VerifyInit
#define EVP_VerifyUpdate               wolfSSL_EVP_VerifyUpdate

#define EVP_CIPHER_CTX_ctrl        wolfSSL_EVP_CIPHER_CTX_ctrl
#define EVP_CIPHER_CTX_block_size  wolfSSL_EVP_CIPHER_CTX_block_size
#define EVP_CIPHER_block_size      wolfSSL_EVP_CIPHER_block_size
#define EVP_CIPHER_flags           wolfSSL_EVP_CIPHER_flags
#define EVP_CIPHER_CTX_set_flags   wolfSSL_EVP_CIPHER_CTX_set_flags
#define EVP_CIPHER_CTX_clear_flags wolfSSL_EVP_CIPHER_CTX_clear_flags
#define EVP_CIPHER_CTX_set_padding wolfSSL_EVP_CIPHER_CTX_set_padding
#define EVP_CIPHER_CTX_flags       wolfSSL_EVP_CIPHER_CTX_flags
#define EVP_CIPHER_CTX_set_iv      wolfSSL_EVP_CIPHER_CTX_set_iv
#define EVP_CIPHER_CTX_get_iv      wolfSSL_EVP_CIPHER_CTX_get_iv
#define EVP_add_digest             wolfSSL_EVP_add_digest
#define EVP_add_cipher             wolfSSL_EVP_add_cipher
#define EVP_cleanup                wolfSSL_EVP_cleanup
#define EVP_read_pw_string         wolfSSL_EVP_read_pw_string
#define EVP_rc2_cbc                wolfSSL_EVP_rc2_cbc

#define OpenSSL_add_all_digests()  wolfSSL_EVP_init()
#define OpenSSL_add_all_ciphers()  wolfSSL_EVP_init()
#define OpenSSL_add_all_algorithms wolfSSL_add_all_algorithms
#define OpenSSL_add_all_algorithms_noconf wolfSSL_OpenSSL_add_all_algorithms_noconf
#define OpenSSL_add_all_algorithms_conf   wolfSSL_OpenSSL_add_all_algorithms_conf

#define wolfSSL_OPENSSL_add_all_algorithms_noconf wolfSSL_OpenSSL_add_all_algorithms_noconf
#define wolfSSL_OPENSSL_add_all_algorithms_conf   wolfSSL_OpenSSL_add_all_algorithms_conf

/* provides older OpenSSL API compatibility  */
#define OPENSSL_add_all_algorithms        OpenSSL_add_all_algorithms
#define OPENSSL_add_all_algorithms_noconf OpenSSL_add_all_algorithms_noconf
#define OPENSSL_add_all_algorithms_conf   OpenSSL_add_all_algorithms_conf

#define NO_PADDING_BLOCK_SIZE      1

#define PKCS5_PBKDF2_HMAC_SHA1     wolfSSL_PKCS5_PBKDF2_HMAC_SHA1
#define PKCS5_PBKDF2_HMAC          wolfSSL_PKCS5_PBKDF2_HMAC
#define EVP_PBE_scrypt             wolfSSL_EVP_PBE_scrypt

/* OpenSSL compat. ctrl values */
#define EVP_CTRL_INIT                  0x0
#define EVP_CTRL_SET_KEY_LENGTH        0x1
#define EVP_CTRL_SET_RC2_KEY_BITS      0x3  /* needed for qt compilation */

#define EVP_CTRL_AEAD_SET_IVLEN        0x9
#define EVP_CTRL_AEAD_GET_TAG          0x10
#define EVP_CTRL_AEAD_SET_TAG          0x11
#define EVP_CTRL_AEAD_SET_IV_FIXED     0x12
#define EVP_CTRL_GCM_IV_GEN            0x13
#define EVP_CTRL_GCM_SET_IVLEN         EVP_CTRL_AEAD_SET_IVLEN
#define EVP_CTRL_GCM_GET_TAG           EVP_CTRL_AEAD_GET_TAG
#define EVP_CTRL_GCM_SET_TAG           EVP_CTRL_AEAD_SET_TAG
#define EVP_CTRL_GCM_SET_IV_FIXED      EVP_CTRL_AEAD_SET_IV_FIXED
#define EVP_CTRL_CCM_SET_IVLEN         EVP_CTRL_AEAD_SET_IVLEN
#define EVP_CTRL_CCM_GET_TAG           EVP_CTRL_AEAD_GET_TAG
#define EVP_CTRL_CCM_SET_TAG           EVP_CTRL_AEAD_SET_TAG
#define EVP_CTRL_CCM_SET_L             0x14
#define EVP_CTRL_CCM_SET_MSGLEN        0x15

#define EVP_PKEY_print_public           wolfSSL_EVP_PKEY_print_public
#define EVP_PKEY_print_private(arg1, arg2, arg3, arg4) WC_DO_NOTHING

#ifndef EVP_MAX_MD_SIZE
    #define EVP_MAX_MD_SIZE   64     /* sha512 */
#endif

#ifndef EVP_MAX_KEY_LENGTH
#define EVP_MAX_KEY_LENGTH    64
#endif

#ifndef EVP_MAX_IV_LENGTH
#define EVP_MAX_IV_LENGTH     16
#endif

#ifndef EVP_MAX_BLOCK_LENGTH
    #define EVP_MAX_BLOCK_LENGTH   32  /* 2 * blocklen(AES)? */
    /* They define this as 32. Using the same value here. */
#endif

#ifndef EVP_MAX_IV_LENGTH
    #define EVP_MAX_IV_LENGTH       16
#endif


#define EVP_R_BAD_DECRYPT               (-MIN_CODE_E + 100 + 1)
#define EVP_R_BN_DECODE_ERROR           (-MIN_CODE_E + 100 + 2)
#define EVP_R_DECODE_ERROR              (-MIN_CODE_E + 100 + 3)
#define EVP_R_PRIVATE_KEY_DECODE_ERROR  (-MIN_CODE_E + 100 + 4)

#define EVP_PKEY_NONE                   NID_undef
#define EVP_PKEY_DH                     28
#define EVP_CIPHER_mode                 WOLFSSL_EVP_CIPHER_mode
/* WOLFSSL_EVP_CIPHER is just the string name of the cipher */
#define EVP_CIPHER_name(x)              x
#define EVP_MD_CTX_reset                wolfSSL_EVP_MD_CTX_cleanup
/* WOLFSSL_EVP_MD is just the string name of the digest */
#define EVP_MD_name(x)                  x
#define EVP_CIPHER_nid                  wolfSSL_EVP_CIPHER_nid

/* Base64 encoding/decoding APIs */
#if defined(WOLFSSL_BASE64_ENCODE) || defined(WOLFSSL_BASE64_DECODE)
#define EVP_ENCODE_CTX       WOLFSSL_EVP_ENCODE_CTX
#define EVP_ENCODE_CTX_new   wolfSSL_EVP_ENCODE_CTX_new
#define EVP_ENCODE_CTX_free  wolfSSL_EVP_ENCODE_CTX_free
#endif /* WOLFSSL_BASE64_ENCODE || WOLFSSL_BASE64_DECODE*/
#if defined(WOLFSSL_BASE64_ENCODE)
#define EVP_EncodeInit       wolfSSL_EVP_EncodeInit
#define EVP_EncodeUpdate     wolfSSL_EVP_EncodeUpdate
#define EVP_EncodeFinal      wolfSSL_EVP_EncodeFinal
#define EVP_EncodeBlock      wolfSSL_EVP_EncodeBlock
#define EVP_DecodeBlock      wolfSSL_EVP_DecodeBlock
#endif /* WOLFSSL_BASE64_ENCODE */
#if defined(WOLFSSL_BASE64_DECODE)
#define EVP_DecodeInit       wolfSSL_EVP_DecodeInit
#define EVP_DecodeUpdate     wolfSSL_EVP_DecodeUpdate
#define EVP_DecodeFinal      wolfSSL_EVP_DecodeFinal
#endif /* WOLFSSL_BASE64_DECODE */

#define EVP_blake2b512       wolfSSL_EVP_blake2b512
#define EVP_blake2s256       wolfSSL_EVP_blake2s256
#define EVP_MD_do_all        wolfSSL_EVP_MD_do_all

#ifdef HAVE_HKDF
#define EVP_PKEY_CTX_set_hkdf_md    wolfSSL_EVP_PKEY_CTX_set_hkdf_md
#define EVP_PKEY_CTX_set1_hkdf_salt wolfSSL_EVP_PKEY_CTX_set1_hkdf_salt
#define EVP_PKEY_CTX_set1_hkdf_key  wolfSSL_EVP_PKEY_CTX_set1_hkdf_key
#define EVP_PKEY_CTX_add1_hkdf_info wolfSSL_EVP_PKEY_CTX_add1_hkdf_info
#define EVP_PKEY_CTX_hkdf_mode      wolfSSL_EVP_PKEY_CTX_hkdf_mode
#endif

WOLFSSL_API void printPKEY(WOLFSSL_EVP_PKEY *k);

#endif /* OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL */

#ifdef __cplusplus
    } /* extern "C" */
#endif

#include <wolfssl/openssl/objects.h>

#endif /* WOLFSSL_EVP_H_ */
