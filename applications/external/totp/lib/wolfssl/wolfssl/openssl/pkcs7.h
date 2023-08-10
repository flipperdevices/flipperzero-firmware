/* pkcs7.h
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

/* pkcs7.h for openSSL */


#ifndef WOLFSSL_PKCS7_H_
#define WOLFSSL_PKCS7_H_

#include <wolfssl/openssl/ssl.h>
#include <wolfssl/wolfcrypt/pkcs7.h>

#ifdef __cplusplus
    extern "C" {
#endif

#if defined(OPENSSL_ALL) && defined(HAVE_PKCS7)

#define PKCS7_TEXT             0x1
#define PKCS7_NOCERTS          0x2
#define PKCS7_DETACHED         0x40
#define PKCS7_BINARY           0x80
#define PKCS7_NOINTERN         0x0010
#define PKCS7_NOVERIFY         0x0020
#define PKCS7_STREAM           0x1000
#define PKCS7_PARTIAL          0x4000

typedef struct WOLFSSL_PKCS7
{
    PKCS7 pkcs7;
    unsigned char* data;
    int len;
    int type;   /* from PKCS7_TYPES, for PKCS7_final() */
    WOLFSSL_STACK* certs;
} WOLFSSL_PKCS7;


WOLFSSL_API PKCS7* wolfSSL_PKCS7_new(void);
WOLFSSL_API PKCS7_SIGNED* wolfSSL_PKCS7_SIGNED_new(void);
WOLFSSL_API void wolfSSL_PKCS7_free(PKCS7* p7);
WOLFSSL_API void wolfSSL_PKCS7_SIGNED_free(PKCS7_SIGNED* p7);
WOLFSSL_API PKCS7* wolfSSL_d2i_PKCS7(PKCS7** p7, const unsigned char** in,
    int len);
WOLFSSL_LOCAL PKCS7* wolfSSL_d2i_PKCS7_ex(PKCS7** p7, const unsigned char** in,
    int len, byte* content, word32 contentSz);
WOLFSSL_API PKCS7* wolfSSL_d2i_PKCS7_bio(WOLFSSL_BIO* bio, PKCS7** p7);
WOLFSSL_API int wolfSSL_i2d_PKCS7_bio(WOLFSSL_BIO *bio, PKCS7 *p7);
WOLFSSL_API int wolfSSL_i2d_PKCS7(PKCS7 *p7, unsigned char **out);
WOLFSSL_API PKCS7* wolfSSL_PKCS7_sign(WOLFSSL_X509* signer,
    WOLFSSL_EVP_PKEY* pkey, WOLFSSL_STACK* certs, WOLFSSL_BIO* in, int flags);
WOLFSSL_API int wolfSSL_PKCS7_verify(PKCS7* p7, WOLFSSL_STACK* certs,
    WOLFSSL_X509_STORE* store, WOLFSSL_BIO* in, WOLFSSL_BIO* out, int flags);
WOLFSSL_API int wolfSSL_PKCS7_final(PKCS7* pkcs7, WOLFSSL_BIO* in, int flags);
WOLFSSL_API int wolfSSL_PKCS7_encode_certs(PKCS7* p7, WOLFSSL_STACK* certs,
                                           WOLFSSL_BIO* out);
WOLFSSL_API WOLFSSL_STACK* wolfSSL_PKCS7_to_stack(PKCS7* pkcs7);
WOLFSSL_API WOLFSSL_STACK* wolfSSL_PKCS7_get0_signers(PKCS7* p7,
    WOLFSSL_STACK* certs, int flags);
WOLFSSL_API int wolfSSL_PEM_write_bio_PKCS7(WOLFSSL_BIO* bio, PKCS7* p7);
#if defined(HAVE_SMIME)
WOLFSSL_API PKCS7* wolfSSL_SMIME_read_PKCS7(WOLFSSL_BIO* in, WOLFSSL_BIO** bcont);
WOLFSSL_API int wolfSSL_SMIME_write_PKCS7(WOLFSSL_BIO* out, PKCS7* pkcs7,
                                          WOLFSSL_BIO* in, int flags);
#endif /* HAVE_SMIME */


#define PKCS7_new                      wolfSSL_PKCS7_new
#define PKCS7_SIGNED_new               wolfSSL_PKCS7_SIGNED_new
#define PKCS7_free                     wolfSSL_PKCS7_free
#define PKCS7_SIGNED_free              wolfSSL_PKCS7_SIGNED_free
#define d2i_PKCS7                      wolfSSL_d2i_PKCS7
#define d2i_PKCS7_bio                  wolfSSL_d2i_PKCS7_bio
#define i2d_PKCS7_bio                  wolfSSL_i2d_PKCS7_bio
#define i2d_PKCS7                      wolfSSL_i2d_PKCS7
#define PKCS7_sign                     wolfSSL_PKCS7_sign
#define PKCS7_verify                   wolfSSL_PKCS7_verify
#define PKCS7_final                    wolfSSL_PKCS7_final
#define PKCS7_get0_signers             wolfSSL_PKCS7_get0_signers
#define PEM_write_bio_PKCS7            wolfSSL_PEM_write_bio_PKCS7
#if defined(HAVE_SMIME)
#define SMIME_read_PKCS7               wolfSSL_SMIME_read_PKCS7
#define SMIME_write_PKCS7              wolfSSL_SMIME_write_PKCS7
#endif /* HAVE_SMIME */

#endif /* OPENSSL_ALL && HAVE_PKCS7 */

#ifdef __cplusplus
    }  /* extern "C" */
#endif

#endif /* WOLFSSL_PKCS7_H_ */

