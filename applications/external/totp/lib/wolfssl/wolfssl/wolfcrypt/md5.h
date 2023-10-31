/* md5.h
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
    \file wolfssl/wolfcrypt/md5.h
*/


#ifndef WOLF_CRYPT_MD5_H
#define WOLF_CRYPT_MD5_H

#include <wolfssl/wolfcrypt/types.h>

#ifndef NO_MD5

#ifdef HAVE_FIPS
    #define wc_InitMd5   InitMd5
    #define wc_Md5Update Md5Update
    #define wc_Md5Final  Md5Final
    #define wc_Md5Hash   Md5Hash
#endif

#ifdef __cplusplus
    extern "C" {
#endif

#ifndef NO_OLD_MD5_NAME
    #define MD5             WC_MD5
#endif

#ifndef NO_OLD_WC_NAMES
    #define Md5             wc_Md5
    #define MD5_BLOCK_SIZE  WC_MD5_BLOCK_SIZE
    #define MD5_DIGEST_SIZE WC_MD5_DIGEST_SIZE
    #define WC_MD5_PAD_SIZE WC_MD5_PAD_SIZE
#endif

/* in bytes */
enum {
    WC_MD5             =  WC_HASH_TYPE_MD5,
    WC_MD5_BLOCK_SIZE  = 64,
    WC_MD5_DIGEST_SIZE = 16,
    WC_MD5_PAD_SIZE    = 56
};


#ifdef WOLFSSL_MICROCHIP_PIC32MZ
    #include <wolfssl/wolfcrypt/port/pic32/pic32mz-crypt.h>
#endif
#ifdef STM32_HASH
    #include <wolfssl/wolfcrypt/port/st/stm32.h>
#endif
#ifdef WOLFSSL_ASYNC_CRYPT
    #include <wolfssl/wolfcrypt/async.h>
#endif

#ifdef WOLFSSL_TI_HASH
    #include "wolfssl/wolfcrypt/port/ti/ti-hash.h"
#elif defined(WOLFSSL_IMX6_CAAM) && !defined(WOLFSSL_QNX_CAAM)
    #include "wolfssl/wolfcrypt/port/caam/wolfcaam_sha.h"
#else

/* MD5 digest */
typedef struct wc_Md5 {
#ifdef STM32_HASH
    STM32_HASH_Context stmCtx;
#else
    word32  buffLen;   /* in bytes          */
    word32  loLen;     /* length in bytes   */
    word32  hiLen;     /* length in bytes   */
    word32  buffer[WC_MD5_BLOCK_SIZE  / sizeof(word32)];
#ifdef WOLFSSL_PIC32MZ_HASH
    word32  digest[PIC32_DIGEST_SIZE / sizeof(word32)];
#else
    word32  digest[WC_MD5_DIGEST_SIZE / sizeof(word32)];
#endif
    void*   heap;
#ifdef WOLFSSL_PIC32MZ_HASH
    hashUpdCache cache; /* cache for updates */
#endif
#endif /* STM32_HASH */
#ifdef WOLFSSL_ASYNC_CRYPT
    WC_ASYNC_DEV asyncDev;
#endif /* WOLFSSL_ASYNC_CRYPT */
#ifdef WOLFSSL_HASH_FLAGS
    word32 flags; /* enum wc_HashFlags in hash.h */
#endif
} wc_Md5;

#endif /* WOLFSSL_TI_HASH */

WOLFSSL_API int wc_InitMd5(wc_Md5* md5);
WOLFSSL_API int wc_InitMd5_ex(wc_Md5* md5, void* heap, int devId);
WOLFSSL_API int wc_Md5Update(wc_Md5* md5, const byte* data, word32 len);
WOLFSSL_API int wc_Md5Final(wc_Md5* md5, byte* hash);
WOLFSSL_API void wc_Md5Free(wc_Md5* md5);
#if defined(OPENSSL_EXTRA) || defined(HAVE_CURL)
WOLFSSL_API int wc_Md5Transform(wc_Md5* md5, const byte* data);
#endif

WOLFSSL_API int  wc_Md5GetHash(wc_Md5* md5, byte* hash);
WOLFSSL_API int  wc_Md5Copy(wc_Md5* src, wc_Md5* dst);

#ifdef WOLFSSL_PIC32MZ_HASH
WOLFSSL_API void wc_Md5SizeSet(wc_Md5* md5, word32 len);
#endif

#ifdef WOLFSSL_HASH_FLAGS
    WOLFSSL_API int wc_Md5SetFlags(wc_Md5* md5, word32 flags);
    WOLFSSL_API int wc_Md5GetFlags(wc_Md5* md5, word32* flags);
#endif

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* NO_MD5 */
#endif /* WOLF_CRYPT_MD5_H */
