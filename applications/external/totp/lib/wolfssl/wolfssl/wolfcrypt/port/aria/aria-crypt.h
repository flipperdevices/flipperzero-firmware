/* aria-crypt.h
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
    \file wolfssl/wolfcrypt/port/aria/aria-crypt.h
*/
/*

DESCRIPTION
This library provides the interfaces to the ARIA cipher implementation for
encrypting and decrypting data.

*/
#ifndef WOLF_CRYPT_ARIA_CRYPT_H
#define WOLF_CRYPT_ARIA_CRYPT_H

#include <wolfssl/wolfcrypt/types.h>

#ifdef __cplusplus
    extern "C" {
#endif

#include "mcapi.h"
#include "mcapi_error.h"

#define ARIA_128_KEY_SIZE 16
#define ARIA_192_KEY_SIZE 24
#define ARIA_256_KEY_SIZE 32

#define ARIA_BLOCK_SIZE 16
#define ARIA_GCM_AUTH_SZ 16

#define WC_ARIA_GCM_GET_CIPHERTEXT_SIZE(x) (x+ARIA_GCM_AUTH_SZ)

typedef struct {
    MC_HSESSION hSession;
    MC_ALGID algo;
    MC_HOBJECT hKey;
    word32 nonce[ARIA_BLOCK_SIZE / sizeof(word32)];
    word32 nonceSz;
} wc_Aria;

WOLFSSL_API int wc_AriaInitCrypt(wc_Aria* aria, MC_ALGID algo);
WOLFSSL_API int wc_AriaFreeCrypt(wc_Aria* aria);
WOLFSSL_API int wc_AriaSetKey(wc_Aria* aria, byte* key);
WOLFSSL_API int wc_AriaGcmSetExtIV(wc_Aria* aria, const byte* iv, word32 ivSz);
WOLFSSL_API int wc_AriaGcmSetIV(wc_Aria* aria, word32 ivSz,
                   const byte* ivFixed, word32 ivFixedSz,
                   WC_RNG* rng);

WOLFSSL_API int wc_AriaEncrypt(wc_Aria *aria, byte* out, byte* in, word32 inSz,
                                    byte* iv, word32 ivSz, byte* aad, word32 aadSz,
                                    byte* authTag, word32 authTagSz);
WOLFSSL_API int wc_AriaDecrypt(wc_Aria *aria, byte* out, byte* in, word32 inSz,
                                    byte* iv, word32 ivSz, byte* aad, word32 aadSz,
                                    byte* authTag, word32 authTagSz);

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLF_CRYPT_ARIA_CRYPT_H */
