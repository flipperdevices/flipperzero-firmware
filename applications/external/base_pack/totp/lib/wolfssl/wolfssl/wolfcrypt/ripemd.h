/* ripemd.h
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
    \file wolfssl/wolfcrypt/ripemd.h
*/

#ifndef WOLF_CRYPT_RIPEMD_H
#define WOLF_CRYPT_RIPEMD_H

#include <wolfssl/wolfcrypt/types.h>

#ifdef WOLFSSL_RIPEMD

#ifdef __cplusplus
    extern "C" {
#endif


/* in bytes */
enum {
    RIPEMD             =  3,    /* hash type unique */
    RIPEMD_BLOCK_SIZE  = 64,
    RIPEMD_DIGEST_SIZE = 20,
    RIPEMD_PAD_SIZE    = 56
};


/* RipeMd 160 digest */
typedef struct RipeMd {
    word32  buffLen;   /* in bytes          */
    word32  loLen;     /* length in bytes   */
    word32  hiLen;     /* length in bytes   */
    word32  digest[RIPEMD_DIGEST_SIZE / sizeof(word32)];
    word32  buffer[RIPEMD_BLOCK_SIZE  / sizeof(word32)];
} RipeMd;


WOLFSSL_API int wc_InitRipeMd(RipeMd* ripemd);
WOLFSSL_API int wc_RipeMdUpdate(RipeMd* ripemd, const byte* data, word32 len);
WOLFSSL_API int wc_RipeMdFinal(RipeMd* ripemd, byte* hash);


#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLFSSL_RIPEMD */
#endif /* WOLF_CRYPT_RIPEMD_H */
