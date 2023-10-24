/* compat_types.h
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

/*
 * Move types that cause cyclical dependency errors here.
 */

#ifndef WOLFSSL_OPENSSL_COMPAT_TYPES_H_
#define WOLFSSL_OPENSSL_COMPAT_TYPES_H_

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/types.h>
#include <wolfssl/wolfcrypt/hmac.h>

#ifndef NO_HMAC
typedef struct WOLFSSL_HMAC_CTX {
    Hmac hmac;
    int  type;
    word32  save_ipad[WC_HMAC_BLOCK_SIZE  / sizeof(word32)];  /* same block size all*/
    word32  save_opad[WC_HMAC_BLOCK_SIZE  / sizeof(word32)];
} WOLFSSL_HMAC_CTX;
#endif

typedef char   WOLFSSL_EVP_MD;
typedef char   WOLFSSL_EVP_CIPHER;
typedef int    WOLFSSL_ENGINE;

typedef struct WOLFSSL_EVP_PKEY       WOLFSSL_EVP_PKEY;
typedef struct WOLFSSL_EVP_MD_CTX     WOLFSSL_EVP_MD_CTX;
typedef struct WOLFSSL_EVP_PKEY       WOLFSSL_PKCS8_PRIV_KEY_INFO;
typedef struct WOLFSSL_EVP_PKEY_CTX   WOLFSSL_EVP_PKEY_CTX;
typedef struct WOLFSSL_EVP_CIPHER_CTX WOLFSSL_EVP_CIPHER_CTX;
typedef struct WOLFSSL_ASN1_PCTX      WOLFSSL_ASN1_PCTX;

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
typedef WOLFSSL_EVP_MD         EVP_MD;
typedef WOLFSSL_EVP_MD_CTX     EVP_MD_CTX;
typedef WOLFSSL_EVP_CIPHER     EVP_CIPHER;
typedef WOLFSSL_EVP_CIPHER_CTX EVP_CIPHER_CTX;
typedef WOLFSSL_ASN1_PCTX      ASN1_PCTX;
typedef WOLFSSL_EVP_PKEY       EVP_PKEY;
typedef WOLFSSL_EVP_PKEY       PKCS8_PRIV_KEY_INFO;

typedef WOLFSSL_ENGINE         ENGINE;
typedef WOLFSSL_EVP_PKEY_CTX   EVP_PKEY_CTX;
#endif /* OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL */

typedef unsigned long (*wolf_sk_hash_cb) (const void *v);

#endif /* !WOLFSSL_OPENSSL_COMPAT_TYPES_H_ */
