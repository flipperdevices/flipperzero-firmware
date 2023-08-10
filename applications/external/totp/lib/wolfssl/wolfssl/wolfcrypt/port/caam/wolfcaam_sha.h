/* wolfcaam_sha.h
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

#ifndef WOLF_CRYPT_CAAM_SHA_H
#define WOLF_CRYPT_CAAM_SHA_H

#include <wolfssl/wolfcrypt/settings.h>

#ifdef WOLFSSL_IMX6_CAAM

#include <wolfssl/wolfcrypt/types.h>

#define WOLFSSL_NO_HASH_RAW

#ifndef WC_CAAM_CTXLEN
/* last 8 bytes of context is for length */
#define WC_CAAM_CTXLEN 8
#endif

#ifndef WC_CAAM_HASH_BLOCK
/* define sha structures and also get the max possible digest. Start with SHA
   digest size */
#define WC_CAAM_HASH_BLOCK 64
#endif

#ifndef WC_CAAM_MAX_DIGEST
#define WC_CAAM_MAX_DIGEST 20
#ifdef WOLFSSL_SHA224
    #undef WC_CAAM_MAX_DIGEST
    #define WC_CAAM_MAX_DIGEST 32
#endif

#ifndef NO_SHA256
    #undef WC_CAAM_MAX_DIGEST
    #define WC_CAAM_MAX_DIGEST 32
#endif

#ifdef WOLFSSL_SHA384
    #undef WC_CAAM_MAX_DIGEST
    #define WC_CAAM_MAX_DIGEST 64
#endif

#ifdef WOLFSSL_SHA512
    #undef WC_CAAM_MAX_DIGEST
    #define WC_CAAM_MAX_DIGEST 64
#endif
#endif /* WC_CAAM_MAX_DIGEST */


typedef struct wc_Sha {
    word32  ctx[(WC_CAAM_MAX_DIGEST + WC_CAAM_CTXLEN) / sizeof(word32)];
    word32  buffLen;   /* in bytes          */
    word32  buffer[WC_CAAM_HASH_BLOCK  / sizeof(word32)];
} wc_Sha;

#ifndef NO_MD5
    typedef struct wc_Sha wc_Md5;
#endif

#ifndef NO_SHA256
    typedef struct wc_Sha wc_Sha256;
#endif

#ifdef WOLFSSL_SHA512
    typedef struct wc_Sha wc_Sha512;
#endif

#endif /* WOLFSSL_IMX6_CAAM */

#endif /* WOLF_CRYPT_CAAM_SHA_H */
