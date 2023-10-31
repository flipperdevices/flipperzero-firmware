/* rc2.h
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

#ifndef WOLF_CRYPT_RC2_H
#define WOLF_CRYPT_RC2_H

#include <wolfssl/wolfcrypt/types.h>

#ifdef WC_RC2

#ifdef __cplusplus
    extern "C" {
#endif

enum {
    RC2_MAX_KEY_SIZE = 128, /* max effective key size, octets */
    RC2_BLOCK_SIZE   = 8
};

/* RC2 encryption and decryption */
typedef struct Rc2 {
    ALIGN16 word16 key[RC2_MAX_KEY_SIZE/2];
    ALIGN16 word32 reg[RC2_BLOCK_SIZE / sizeof(word32)];    /* for CBC mode */
    ALIGN16 word32 tmp[RC2_BLOCK_SIZE / sizeof(word32)];    /* same         */
    word32 keylen;      /* key length, octets */
    word32 bits;        /* effective key length, bits */
} Rc2;

WOLFSSL_API int wc_Rc2SetKey(Rc2* rc2, const byte* key, word32 length,
                             const byte* iv, word32 bits);
WOLFSSL_API int wc_Rc2SetIV(Rc2* rc2, const byte* iv);

/* RC2-ECB */
WOLFSSL_API int wc_Rc2EcbEncrypt(Rc2* rc2, byte* out,
                                 const byte* in, word32 sz);
WOLFSSL_API int wc_Rc2EcbDecrypt(Rc2* rc2, byte* out,
                                 const byte* in, word32 sz);

/* RC2-CBC */
WOLFSSL_API int wc_Rc2CbcEncrypt(Rc2* rc2, byte* out,
                                 const byte* in, word32 sz);
WOLFSSL_API int wc_Rc2CbcDecrypt(Rc2* rc2, byte* out,
                                 const byte* in, word32 sz);

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WC_RC2 */
#endif /* WOLF_CRYPT_RC2_H */
