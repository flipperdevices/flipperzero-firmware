/* pem.h
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

/* pem.h for openssl */

/*!
    \file wolfssl/openssl/pem.h
*/


#ifndef WOLFSSL_PEM_H_
#define WOLFSSL_PEM_H_

#include <wolfssl/openssl/evp.h>
#include <wolfssl/openssl/bio.h>
#include <wolfssl/openssl/rsa.h>
#include <wolfssl/openssl/dsa.h>
#include <wolfssl/ssl.h>

#ifdef __cplusplus
    extern "C" {
#endif

/* RSA */
WOLFSSL_API
int wolfSSL_PEM_write_bio_RSAPrivateKey(WOLFSSL_BIO* bio, WOLFSSL_RSA* rsa,
                                        const WOLFSSL_EVP_CIPHER* cipher,
                                        unsigned char* passwd, int len,
                                        wc_pem_password_cb* cb, void* arg);
WOLFSSL_API
WOLFSSL_RSA* wolfSSL_PEM_read_bio_RSAPrivateKey(WOLFSSL_BIO* bio,
        WOLFSSL_RSA** rsa, wc_pem_password_cb* cb, void* pass);

WOLFSSL_API
int wolfSSL_PEM_write_bio_RSA_PUBKEY(WOLFSSL_BIO* bio, WOLFSSL_RSA* rsa);

WOLFSSL_API
WOLFSSL_RSA *wolfSSL_PEM_read_bio_RSA_PUBKEY(WOLFSSL_BIO* bio,
                                             WOLFSSL_RSA** rsa,
                                             wc_pem_password_cb* cb, void *u);

WOLFSSL_API
WOLFSSL_EC_GROUP* wolfSSL_PEM_read_bio_ECPKParameters(WOLFSSL_BIO* bio,
                                                      WOLFSSL_EC_GROUP** group,
                                                      wc_pem_password_cb* cb,
                                                      void* pass);
WOLFSSL_API
int wolfSSL_PEM_write_mem_RSAPrivateKey(WOLFSSL_RSA* rsa,
                                        const WOLFSSL_EVP_CIPHER* cipher,
                                        unsigned char* passwd, int len,
                                        unsigned char **pem, int *plen);
#if !defined(NO_FILESYSTEM)
WOLFSSL_API
int wolfSSL_PEM_write_RSAPrivateKey(XFILE fp, WOLFSSL_RSA *rsa,
                                    const WOLFSSL_EVP_CIPHER *enc,
                                    unsigned char *kstr, int klen,
                                    wc_pem_password_cb *cb, void *u);

WOLFSSL_API
WOLFSSL_RSA* wolfSSL_PEM_read_RSAPrivateKey(XFILE fp, WOLFSSL_RSA** rsa,
                                            wc_pem_password_cb* cb, void* pass);

WOLFSSL_API
WOLFSSL_RSA *wolfSSL_PEM_read_RSAPublicKey(XFILE fp, WOLFSSL_RSA **x,
                                           wc_pem_password_cb *cb, void *u);
WOLFSSL_API
int wolfSSL_PEM_write_RSAPublicKey(XFILE fp, WOLFSSL_RSA* key);

WOLFSSL_API
int wolfSSL_PEM_write_RSA_PUBKEY(XFILE fp, WOLFSSL_RSA *x);

WOLFSSL_API
WOLFSSL_RSA *wolfSSL_PEM_read_RSA_PUBKEY(XFILE fp, WOLFSSL_RSA** rsa,
                                         wc_pem_password_cb* cb, void *pass);
#endif /* NO_FILESYSTEM */

/* DSA */
WOLFSSL_API
int wolfSSL_PEM_write_bio_DSAPrivateKey(WOLFSSL_BIO* bio,
                                        WOLFSSL_DSA* dsa,
                                        const WOLFSSL_EVP_CIPHER* cipher,
                                        unsigned char* passwd, int len,
                                        wc_pem_password_cb* cb, void* arg);

WOLFSSL_API
WOLFSSL_DSA* wolfSSL_PEM_read_bio_DSAPrivateKey(WOLFSSL_BIO* bio,
                                                WOLFSSL_DSA** dsa,
                                                wc_pem_password_cb* cb,
                                                void *pass);

WOLFSSL_API
WOLFSSL_DSA *wolfSSL_PEM_read_bio_DSA_PUBKEY(WOLFSSL_BIO* bio,
                                             WOLFSSL_DSA** dsa,
                                             wc_pem_password_cb* cb,
                                             void *pass);

WOLFSSL_API
int wolfSSL_PEM_write_bio_DSA_PUBKEY(WOLFSSL_BIO* bio, WOLFSSL_DSA* dsa);

WOLFSSL_API
int wolfSSL_PEM_write_mem_DSAPrivateKey(WOLFSSL_DSA* dsa,
                                        const WOLFSSL_EVP_CIPHER* cipher,
                                        unsigned char* passwd, int len,
                                        unsigned char **pem, int *plen);
#if !defined(NO_FILESYSTEM)
WOLFSSL_API
int wolfSSL_PEM_write_DSAPrivateKey(XFILE fp, WOLFSSL_DSA *dsa,
                                    const WOLFSSL_EVP_CIPHER *enc,
                                    unsigned char *kstr, int klen,
                                    wc_pem_password_cb *cb, void *u);
WOLFSSL_API
int wolfSSL_PEM_write_DSA_PUBKEY(XFILE fp, WOLFSSL_DSA *x);
#endif /* NO_FILESYSTEM */

/* ECC */
WOLFSSL_API
int wolfSSL_PEM_write_bio_ECPrivateKey(WOLFSSL_BIO* bio, WOLFSSL_EC_KEY* ec,
                                       const WOLFSSL_EVP_CIPHER* cipher,
                                       unsigned char* passwd, int len,
                                       wc_pem_password_cb* cb, void* arg);
WOLFSSL_API
WOLFSSL_EC_KEY* wolfSSL_PEM_read_bio_ECPrivateKey(WOLFSSL_BIO* bio,
                                                  WOLFSSL_EC_KEY** ec,
                                                  wc_pem_password_cb* cb,
                                                  void *pass);
WOLFSSL_API
int wolfSSL_PEM_write_bio_EC_PUBKEY(WOLFSSL_BIO* bio, WOLFSSL_EC_KEY* ec);

WOLFSSL_API
int wolfSSL_PEM_write_mem_ECPrivateKey(WOLFSSL_EC_KEY* key,
                                       const WOLFSSL_EVP_CIPHER* cipher,
                                       unsigned char* passwd, int len,
                                       unsigned char **pem, int *plen);
#if !defined(NO_FILESYSTEM)
WOLFSSL_API
int wolfSSL_PEM_write_ECPrivateKey(XFILE fp, WOLFSSL_EC_KEY *key,
                                   const WOLFSSL_EVP_CIPHER *enc,
                                   unsigned char *kstr, int klen,
                                   wc_pem_password_cb *cb, void *u);
WOLFSSL_API
int wolfSSL_PEM_write_EC_PUBKEY(XFILE fp, WOLFSSL_EC_KEY* key);
#endif

#ifndef NO_BIO
WOLFSSL_API
WOLFSSL_EC_KEY* wolfSSL_PEM_read_bio_EC_PUBKEY(WOLFSSL_BIO* bio,
                                               WOLFSSL_EC_KEY** ec,
                                               wc_pem_password_cb* cb,
                                               void *pass);
#endif /* !NO_BIO */

/* EVP_KEY */
WOLFSSL_API
WOLFSSL_EVP_PKEY* wolfSSL_PEM_read_bio_PrivateKey(WOLFSSL_BIO* bio,
                                                  WOLFSSL_EVP_PKEY** key,
                                                  wc_pem_password_cb* cb,
                                                  void* pass);
WOLFSSL_API
WOLFSSL_EVP_PKEY *wolfSSL_PEM_read_bio_PUBKEY(WOLFSSL_BIO* bio,
                                              WOLFSSL_EVP_PKEY **key,
                                              wc_pem_password_cb *cb,
                                              void *pass);
WOLFSSL_API
int wolfSSL_PEM_write_bio_PrivateKey(WOLFSSL_BIO* bio, WOLFSSL_EVP_PKEY* key,
                                        const WOLFSSL_EVP_CIPHER* cipher,
                                        unsigned char* passwd, int len,
                                        wc_pem_password_cb* cb, void* arg);
WOLFSSL_API
int wolfSSL_PEM_write_bio_PUBKEY(WOLFSSL_BIO* bio, WOLFSSL_EVP_PKEY* key);


WOLFSSL_API
int wolfSSL_PEM_read_bio(WOLFSSL_BIO* bio, char **name, char **header,
                         unsigned char **data, long *len);
WOLFSSL_API
int wolfSSL_PEM_write_bio(WOLFSSL_BIO *bio, const char *name,
                          const char *header, const unsigned char *data,
                          long len);
#if !defined(NO_FILESYSTEM)
WOLFSSL_API
int wolfSSL_PEM_read(XFILE fp, char **name, char **header, unsigned char **data,
                     long *len);
WOLFSSL_API
int wolfSSL_PEM_write(XFILE fp, const char *name, const char *header,
                      const unsigned char *data, long len);
#endif

#if !defined(NO_FILESYSTEM)
WOLFSSL_API
WOLFSSL_EVP_PKEY *wolfSSL_PEM_read_PUBKEY(XFILE fp, WOLFSSL_EVP_PKEY **x,
                                          wc_pem_password_cb *cb, void *u);
WOLFSSL_API
WOLFSSL_X509 *wolfSSL_PEM_read_X509(XFILE fp, WOLFSSL_X509 **x,
                                          wc_pem_password_cb *cb, void *u);
WOLFSSL_API
WOLFSSL_EVP_PKEY *wolfSSL_PEM_read_PrivateKey(XFILE fp, WOLFSSL_EVP_PKEY **x,
                                          wc_pem_password_cb *cb, void *u);

WOLFSSL_API
int wolfSSL_PEM_write_X509(XFILE fp, WOLFSSL_X509 *x);
WOLFSSL_API
int wolfSSL_PEM_write_DHparams(XFILE fp, WOLFSSL_DH* dh);
#endif /* NO_FILESYSTEM */

#define PEM_BUFSIZE WOLF_PEM_BUFSIZE

#define PEM_read                        wolfSSL_PEM_read
#define PEM_read_bio                    wolfSSL_PEM_read_bio
#define PEM_write                       wolfSSL_PEM_write
#define PEM_write_bio                   wolfSSL_PEM_write_bio

#define PEM_read_X509                   wolfSSL_PEM_read_X509
#define PEM_read_PrivateKey             wolfSSL_PEM_read_PrivateKey
#define PEM_write_X509                  wolfSSL_PEM_write_X509
#define PEM_write_bio_PrivateKey        wolfSSL_PEM_write_bio_PrivateKey
#define PEM_write_bio_PKCS8PrivateKey   wolfSSL_PEM_write_bio_PKCS8PrivateKey
#define PEM_write_PKCS8PrivateKey       wolfSSL_PEM_write_PKCS8PrivateKey

/* DH */
#define PEM_write_DHparams              wolfSSL_PEM_write_DHparams
/* RSA */
#define PEM_write_bio_RSAPrivateKey     wolfSSL_PEM_write_bio_RSAPrivateKey
#define PEM_read_bio_RSAPrivateKey      wolfSSL_PEM_read_bio_RSAPrivateKey
#define PEM_read_RSAPrivateKey          wolfSSL_PEM_read_RSAPrivateKey
#define PEM_write_bio_RSA_PUBKEY        wolfSSL_PEM_write_bio_RSA_PUBKEY
#define PEM_read_bio_RSA_PUBKEY         wolfSSL_PEM_read_bio_RSA_PUBKEY
#define PEM_read_bio_RSAPublicKey       wolfSSL_PEM_read_bio_RSA_PUBKEY
#define PEM_read_bio_ECPKParameters     wolfSSL_PEM_read_bio_ECPKParameters
#define PEM_write_RSAPrivateKey         wolfSSL_PEM_write_RSAPrivateKey
#define PEM_write_RSA_PUBKEY            wolfSSL_PEM_write_RSA_PUBKEY
#define PEM_read_RSA_PUBKEY             wolfSSL_PEM_read_RSA_PUBKEY
#define PEM_write_RSAPublicKey          wolfSSL_PEM_write_RSAPublicKey
#define PEM_read_RSAPublicKey           wolfSSL_PEM_read_RSAPublicKey
/* DSA */
#define PEM_write_bio_DSAPrivateKey     wolfSSL_PEM_write_bio_DSAPrivateKey
#define PEM_write_DSAPrivateKey         wolfSSL_PEM_write_DSAPrivateKey
#define PEM_write_bio_DSA_PUBKEY        wolfSSL_PEM_write_bio_DSA_PUBKEY
#define PEM_write_DSA_PUBKEY            wolfSSL_PEM_write_DSA_PUBKEY
#define PEM_read_bio_DSAPrivateKey      wolfSSL_PEM_read_bio_DSAPrivateKey
#define PEM_read_bio_DSA_PUBKEY         wolfSSL_PEM_read_bio_DSA_PUBKEY
/* ECC */
#define PEM_write_bio_ECPrivateKey      wolfSSL_PEM_write_bio_ECPrivateKey
#define PEM_write_bio_EC_PUBKEY         wolfSSL_PEM_write_bio_EC_PUBKEY
#define PEM_write_EC_PUBKEY             wolfSSL_PEM_write_EC_PUBKEY
#define PEM_write_ECPrivateKey          wolfSSL_PEM_write_ECPrivateKey
#define PEM_read_bio_ECPrivateKey       wolfSSL_PEM_read_bio_ECPrivateKey
#define PEM_read_bio_EC_PUBKEY          wolfSSL_PEM_read_bio_EC_PUBKEY
#ifndef NO_WOLFSSL_STUB
#define PEM_write_bio_ECPKParameters(...) 0
#endif
/* EVP_KEY */
#define PEM_read_bio_PrivateKey         wolfSSL_PEM_read_bio_PrivateKey
#define PEM_read_PUBKEY                 wolfSSL_PEM_read_PUBKEY
#define PEM_read_bio_PUBKEY             wolfSSL_PEM_read_bio_PUBKEY
#define PEM_write_bio_PUBKEY            wolfSSL_PEM_write_bio_PUBKEY

#ifdef __cplusplus
    }  /* extern "C" */
#endif

#endif /* WOLFSSL_PEM_H_ */

