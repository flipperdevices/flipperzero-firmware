/* wolfcaam_hash.h
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

#include <wolfssl/wolfcrypt/settings.h>

#ifndef WOLFCAAM_HASH_H
#define WOLFCAAM_HASH_H

#include <wolfssl/wolfcrypt/sha256.h>

#ifdef WOLFSSL_IMXRT1170_CAAM
WOLFSSL_LOCAL int wc_CAAM_HashInit(caam_handle_t* hndl, caam_hash_ctx_t* ctx, int type);
#endif

#ifndef NO_SHA
WOLFSSL_LOCAL int wc_CAAM_ShaHash(wc_Sha* sha, const byte* in, word32 inSz,
    byte* digest);
#endif
#ifdef WOLFSSL_SHA224
WOLFSSL_LOCAL int wc_CAAM_Sha224Hash(wc_Sha224* sha224, const byte* in,
    word32 inSz, byte* digest);
#endif
WOLFSSL_LOCAL int wc_CAAM_Sha256Hash(wc_Sha256* sha256, const byte* in,
    word32 inSz, byte* digest);
#if defined(WOLFSSL_SHA384)
WOLFSSL_LOCAL int wc_CAAM_Sha384Hash(wc_Sha384* sha384, const byte* in,
    word32 inSz, byte* digest);
#endif
#if defined(WOLFSSL_SHA512)
WOLFSSL_LOCAL int wc_CAAM_Sha512Hash(wc_Sha512* sha512, const byte* in,
    word32 inSz, byte* digest);
#endif

#if !defined(NO_HMAC)
#ifndef WC_HMAC_TYPE_DEFINED
    typedef struct Hmac Hmac;
    #define WC_HMAC_TYPE_DEFINED
#endif
WOLFSSL_LOCAL int wc_CAAM_Hmac(Hmac* hmac, int macType, const byte* msg,
    int msgSz, byte* digest);
#endif

#ifndef WC_CAAM_HASH_BLOCK
    #define WC_CAAM_HASH_BLOCK 64
#endif
#endif

