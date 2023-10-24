/* wolfcaam_aes.h
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

#ifndef WOLFCAAM_AES_H
#define WOLFCAAM_AES_H

#if !defined(NO_AES) && defined(WOLFSSL_CAAM)

#include <wolfssl/wolfcrypt/aes.h>

WOLFSSL_LOCAL int wc_CAAM_AesCcmEncrypt(Aes* aes, const byte* in, byte* out,
        word32 sz, const byte* nonce, word32 nonceSz, byte* authTag,
        word32 authTagSz, const byte* authIn, word32 authInSz);

WOLFSSL_LOCAL int wc_CAAM_AesCcmDecrypt(Aes* aes, const byte* in, byte* out,
        word32 sz, const byte* nonce, word32 nonceSz, const byte* authTag,
        word32 authTagSz, const byte* authIn, word32 authInSz);

WOLFSSL_LOCAL int wc_CAAM_AesGcmEncrypt(Aes* aes, const byte* in, byte* out,
        word32 sz, const byte* nonce, word32 nonceSz, byte* authTag,
        word32 authTagSz, const byte* authIn, word32 authInSz);

WOLFSSL_LOCAL int wc_CAAM_AesGcmDecrypt(Aes* aes, const byte* in, byte* out,
        word32 sz, const byte* nonce, word32 nonceSz, const byte* authTag,
        word32 authTagSz, const byte* authIn, word32 authInSz);

WOLFSSL_LOCAL int wc_CAAM_AesCtrEncrypt(Aes* aes, byte* out, const byte* in,
        word32 sz);

WOLFSSL_LOCAL int wc_CAAM_AesCbcEncrypt(Aes* aes, byte* out, const byte* in,
        word32 sz);

WOLFSSL_LOCAL int wc_CAAM_AesCbcDecrypt(Aes* aes, byte* out, const byte* in,
        word32 sz);

WOLFSSL_LOCAL int wc_CAAM_AesEcbEncrypt(Aes* aes, byte* out, const byte* in,
        word32 sz);

WOLFSSL_LOCAL int wc_CAAM_AesEcbDecrypt(Aes* aes, byte* out, const byte* in,
        word32 sz);
#endif /* WOLFSSL_CAAM && !NO_AES */
#endif /* WOLFCAAM_AES_H */

