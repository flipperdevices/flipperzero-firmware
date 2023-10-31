/* blake2.h
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
    \file wolfssl/wolfcrypt/blake2.h
*/

#ifndef WOLF_CRYPT_BLAKE2_H
#define WOLF_CRYPT_BLAKE2_H

#include <wolfssl/wolfcrypt/settings.h>

#if defined(HAVE_BLAKE2) || defined(HAVE_BLAKE2S)

#include <wolfssl/wolfcrypt/blake2-int.h>

/* call old functions if using fips for the sake of hmac @wc_fips */
#ifdef HAVE_FIPS
    /* Since hmac can call blake functions provide original calls */
    #define wc_InitBlake2b   InitBlake2b
    #define wc_Blake2bUpdate Blake2bUpdate
    #define wc_Blake2bFinal  Blake2bFinal
#endif

#ifdef __cplusplus
    extern "C" {
#endif

/* in bytes, variable digest size up to 512 bits (64 bytes) */
enum {
#ifdef HAVE_BLAKE2B
    BLAKE2B_ID  = WC_HASH_TYPE_BLAKE2B,
    BLAKE2B_256 = 32,  /* 256 bit type, SSL default */
#endif
#ifdef HAVE_BLAKE2S
    BLAKE2S_ID  = WC_HASH_TYPE_BLAKE2S,
    BLAKE2S_256 = 32   /* 256 bit type */
#endif
};


#ifdef HAVE_BLAKE2B
/* BLAKE2b digest */
typedef struct Blake2b {
    blake2b_state S[1];         /* our state */
    word32        digestSz;     /* digest size used on init */
} Blake2b;
#endif

#ifdef HAVE_BLAKE2S
/* BLAKE2s digest */
typedef struct Blake2s {
    blake2s_state S[1];         /* our state */
    word32        digestSz;     /* digest size used on init */
} Blake2s;
#endif


#ifdef HAVE_BLAKE2B
WOLFSSL_API int wc_InitBlake2b(Blake2b* b2b, word32 digestSz);
WOLFSSL_API int wc_InitBlake2b_WithKey(Blake2b* b2b, word32 digestSz,
                                       const byte *key, word32 keylen);
WOLFSSL_API int wc_Blake2bUpdate(Blake2b* b2b, const byte* data, word32 sz);
WOLFSSL_API int wc_Blake2bFinal(Blake2b* b2b, byte* final, word32 requestSz);
#endif

#ifdef HAVE_BLAKE2S
WOLFSSL_API int wc_InitBlake2s(Blake2s* b2s, word32 digestSz);
WOLFSSL_API int wc_InitBlake2s_WithKey(Blake2s* b2s, word32 digestSz,
                                       const byte *key, word32 keylen);
WOLFSSL_API int wc_Blake2sUpdate(Blake2s* b2s, const byte* data, word32 sz);
WOLFSSL_API int wc_Blake2sFinal(Blake2s* b2s, byte* final, word32 requestSz);
#endif


#ifdef __cplusplus
    }
#endif

#endif  /* HAVE_BLAKE2 || HAVE_BLAKE2S */
#endif  /* WOLF_CRYPT_BLAKE2_H */

