/* user_settings_wolftpm.h
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


/* To use the rename file to user_settings.h and define WOLFSSL_USER_SETTINGS */

/* Test using:
cp ./examples/configs/user_settings_wolftpm.h user_settings.h
./configure --enable-usersettings --disable-examples CFLAGS="-Os"
make
*/

/* Options generated from configure and wolfssl/options.h using:
./configure --enable-wolftpm --disable-dh --disable-oldtls \
  --disable-sha3 --disable-sha512 --disable-sha384 --disable-sha224 \
  --disable-pkcs12 --disable-chacha --disable-poly1305 \
  --disable-sys-ca-certs --disable-examples
*/

/* Options for USE_LOW_RESOURCE generated with configure and wolfssl/options.h
 * using:
./configure --enable-lowresource --enable-cryptonly --enable-aescfb \
  --enable-hmac --enable-cryptocb --enable-singlethreaded \
  --enable-sp=smallrsa2048 --enable-sp-math --disable-sp-asm \
  --disable-filesystem --disable-asm --disable-pkcs8 --disable-pkcs12 \
  --disable-ecc --disable-dh \
  --disable-sha224 --disable-sha384 --disable-sha512 \
  --disable-md5 --disable-sha3 \
  --disable-aescbc --disable-aesgcm --disable-aesccm \
  --disable-poly1305 --disable-chacha \
  --disable-hashdrbg \
  --disable-sys-ca-certs --disable-error-queue-per-thread \
  --disable-oldtls --disable-errorstrings --disable-memory --disable-coding \
  --disable-examples --disable-optflags \
  CFLAGS="-Os -DNO_SIG_WRAPPER -DNO_AES_192 -DNO_AES_256 \
  -DWOLFSSL_RSA_PUBLIC_ONLY -DWOLFSSL_RSA_VERIFY_INLINE -DNO_CHECK_PRIVATE_KEY"
 */

/* For the wolfTPM build:
./configure --enable-swtpm CFLAGS="-Os" && make
*/


#ifndef WOLF_USER_SETTINGS_TPM_H
#define WOLF_USER_SETTINGS_TPM_H

#ifdef __cplusplus
extern "C" {
#endif

/* enable for low resource options */
#if 0
    #define USE_LOW_RESOURCE
#endif

#ifdef USE_LOW_RESOURCE
    /* wolfCrypt only (no SSL/TLS) */
    #define WOLFCRYPT_ONLY
#else
    /* wolfTPM with TLS example (v1.3 only) */
    #define WOLFSSL_TLS13
    #define WOLFSSL_NO_TLS12
    #define NO_OLD_TLS

    #define HAVE_TLS_EXTENSIONS
    #define HAVE_SUPPORTED_CURVES
    #define HAVE_SERVER_RENEGOTIATION_INFO
    #define HAVE_ENCRYPT_THEN_MAC

    #define HAVE_HKDF
    #define WC_RSA_PSS
    #define WOLFSSL_PSS_LONG_SALT
#endif

/* No threading or file system */
#define SINGLE_THREADED

/* Enable crypto callbacks */
#define WOLF_CRYPTO_CB

#ifdef USE_LOW_RESOURCE
    /* Single Precision math for RSA 2048 only (small) */
    #define WOLFSSL_HAVE_SP_RSA
    #define WOLFSSL_SP_MATH
    #define WOLFSSL_SP_SMALL
    #define WOLFSSL_SP_NO_3072 /* 2048-only */
#else
    /* Enable SP math all (sp_int.c) with multi-precision support */
    #define WOLFSSL_SP_MATH_ALL
#endif

/* Enable hardening (timing resistance) */
#define TFM_TIMING_RESISTANT
#define ECC_TIMING_RESISTANT
#define WC_RSA_BLINDING

/* Enable PRNG (SHA2-256) */
#ifdef USE_LOW_RESOURCE
    /* use TPM TRNG */
    #define WC_NO_HASHDRBG
#else
    #define HAVE_HASHDRBG
#endif

/* Asymmetric */
#if 1 /* RSA - needed to encrypt salt */
    #undef  NO_RSA
    #ifdef USE_LOW_RESOURCE
        #define WOLFSSL_RSA_PUBLIC_ONLY
        #define WOLFSSL_RSA_VERIFY_INLINE
        #define NO_CHECK_PRIVATE_KEY
    #endif
#else
    #define NO_RSA
#endif
#ifndef USE_LOW_RESOURCE /* ECC */
    #define HAVE_ECC
    #define ECC_USER_CURVES /* default to only SECP256R1 */
#endif
#ifndef USE_LOW_RESOURCE /* DH */
    #undef  NO_DH
    #define HAVE_FFDHE_2048
    #define HAVE_DH_DEFAULT_PARAMS
#else
    #define NO_DH
#endif

/* Symmetric Hash */
#undef NO_SHA
#undef NO_SHA256
#ifndef USE_LOW_RESOURCE
    #define WOLFSSL_SHA512
    #define WOLFSSL_SHA384
#endif

/* Symmetric Cipher */
#define WOLFSSL_AES_CFB
#define HAVE_AES_DECRYPT
#ifndef USE_LOW_RESOURCE
    #define HAVE_AES_KEYWRAP
    #define WOLFSSL_AES_DIRECT
    #define HAVE_AESGCM
    #define GCM_TABLE_4BIT
#else
    #define NO_AES_CBC
#endif

#if 0 /* ChaCha20 / Poly1305 */
    #define HAVE_POLY1305
    #define HAVE_CHACHA
#endif

/* Features */
#define WOLFSSL_ASN_TEMPLATE
#define WOLFSSL_USER_IO /* user recv/send callbacks for network IO */
#ifndef USE_LOW_RESOURCE
    #define WOLFSSL_CERT_GEN
    #define WOLFSSL_CERT_REQ
    #define WOLFSSL_CERT_EXT

    #define HAVE_PKCS7
    #define HAVE_X963_KDF
    #define WOLFSSL_BASE64_ENCODE
#endif

/* Disables */
#define NO_PKCS8
#define NO_PKCS12
#define NO_PWDBASED
#define NO_DSA
#define NO_DES3
#define NO_RC4
#define NO_PSK
#define NO_MD4
#define NO_MD5
#define WOLFSSL_NO_SHAKE128
#define WOLFSSL_NO_SHAKE256
#define NO_WRITEV

/* Low Resource Options */
#ifdef USE_LOW_RESOURCE
    #define NO_FILESYSTEM /* File system disable */
    #define NO_ERROR_STRINGS
    #define WOLFSSL_NO_ASM
    #define TFM_NO_ASM
    #define NO_WOLFSSL_MEMORY
    #define NO_SESSION_CACHE
    #define RSA_LOW_MEM
    #define WOLFSSL_AES_SMALL_TABLES
    #define WOLFSSL_AES_NO_UNROLL
    #define GCM_SMALL
    #undef  GCM_TABLE_4BIT
    #define NO_AES_192
    #define NO_AES_256
    #define USE_SLOW_SHA
    #define USE_SLOW_SHA256
    #define USE_SLOW_SHA512
    #define NO_SIG_WRAPPER
    #define NO_ASN_TIME
    #define NO_CODING
    #define NO_BIG_INT
#endif

#ifdef __cplusplus
}
#endif

#endif /* WOLF_USER_SETTINGS_TPM_H */
