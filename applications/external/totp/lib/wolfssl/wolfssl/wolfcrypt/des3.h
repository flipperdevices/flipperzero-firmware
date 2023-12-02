/* des3.h
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
    \file wolfssl/wolfcrypt/des3.h
*/

#ifndef WOLF_CRYPT_DES3_H
#define WOLF_CRYPT_DES3_H

#include <wolfssl/wolfcrypt/types.h>

#ifndef NO_DES3

#if defined(HAVE_FIPS) && defined(HAVE_FIPS_VERSION) && \
        (HAVE_FIPS_VERSION == 2 || HAVE_FIPS_VERSION == 3)
    #include <wolfssl/wolfcrypt/fips.h>
#endif /* HAVE_FIPS_VERSION >= 2 */

#ifdef __cplusplus
    extern "C" {
#endif

/* these are required for FIPS and non-FIPS */
enum {
    DES_KEY_SIZE        =  8,  /* des                     */
    DES3_KEY_SIZE       = 24,  /* 3 des ede               */
    DES_IV_SIZE         =  8   /* should be the same as DES_BLOCK_SIZE */
};


/* avoid redefinition of structs */
#if !defined(HAVE_FIPS) || (defined(HAVE_FIPS_VERSION) && \
    HAVE_FIPS_VERSION >= 2)

#ifdef WOLFSSL_ASYNC_CRYPT
    #include <wolfssl/wolfcrypt/async.h>
#endif

#ifdef WOLFSSL_SE050
    /* SE050 SDK also defines DES_BLOCK_SIZE */
    #undef DES_BLOCK_SIZE
#endif

enum {
    DES_ENC_TYPE    = WC_CIPHER_DES,     /* cipher unique type */
    DES3_ENC_TYPE   = WC_CIPHER_DES3,    /* cipher unique type */

    DES_BLOCK_SIZE  = 8,
    DES_KS_SIZE     = 32,    /* internal DES key buffer size */

    DES_ENCRYPTION  = 0,
    DES_DECRYPTION  = 1
};

#define DES_IVLEN 8
#define DES_KEYLEN 8
#define DES3_IVLEN 8
#define DES3_KEYLEN 24


#if defined(STM32_CRYPTO)

#include <wolfssl/wolfcrypt/port/st/stm32.h>

enum {
    DES_CBC = 0,
    DES_ECB = 1
};
#endif


/* DES encryption and decryption */
typedef struct Des {
    word32 reg[DES_BLOCK_SIZE / sizeof(word32)];      /* for CBC mode */
    word32 tmp[DES_BLOCK_SIZE / sizeof(word32)];      /* same         */
    word32 key[DES_KS_SIZE];
} Des;


/* DES3 encryption and decryption */
struct Des3 {
    word32 key[3][DES_KS_SIZE];
    word32 reg[DES_BLOCK_SIZE / sizeof(word32)];      /* for CBC mode */
    word32 tmp[DES_BLOCK_SIZE / sizeof(word32)];      /* same         */
#ifdef WOLFSSL_ASYNC_CRYPT
    WC_ASYNC_DEV asyncDev;
#endif
#if defined(WOLF_CRYPTO_CB) || \
    (defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_3DES))
    word32 devKey[DES3_KEYLEN/sizeof(word32)]; /* raw key */
#endif
#ifdef WOLF_CRYPTO_CB
    int    devId;
    void*  devCtx;
#endif
    void* heap;
};

#ifndef WC_DES3_TYPE_DEFINED
    typedef struct Des3 Des3;
    #define WC_DES3_TYPE_DEFINED
#endif
#endif /* HAVE_FIPS && HAVE_FIPS_VERSION >= 2 */


WOLFSSL_API int  wc_Des_SetKey(Des* des, const byte* key,
                               const byte* iv, int dir);
WOLFSSL_API void wc_Des_SetIV(Des* des, const byte* iv);
WOLFSSL_API int  wc_Des_CbcEncrypt(Des* des, byte* out,
                                   const byte* in, word32 sz);
WOLFSSL_API int  wc_Des_CbcDecrypt(Des* des, byte* out,
                                   const byte* in, word32 sz);
WOLFSSL_API int  wc_Des_EcbEncrypt(Des* des, byte* out,
                                   const byte* in, word32 sz);
WOLFSSL_API int wc_Des3_EcbEncrypt(Des3* des, byte* out,
                                   const byte* in, word32 sz);

/* ECB decrypt same process as encrypt but with decrypt key */
#define wc_Des_EcbDecrypt  wc_Des_EcbEncrypt
#define wc_Des3_EcbDecrypt wc_Des3_EcbEncrypt

WOLFSSL_API int  wc_Des3_SetKey(Des3* des, const byte* key,
                                const byte* iv,int dir);
WOLFSSL_API int  wc_Des3_SetIV(Des3* des, const byte* iv);
WOLFSSL_API int  wc_Des3_CbcEncrypt(Des3* des, byte* out,
                                    const byte* in,word32 sz);
WOLFSSL_API int  wc_Des3_CbcDecrypt(Des3* des, byte* out,
                                    const byte* in,word32 sz);

/* These are only required when using either:
  static memory (WOLFSSL_STATIC_MEMORY) or asynchronous (WOLFSSL_ASYNC_CRYPT) */
WOLFSSL_API int  wc_Des3Init(Des3* des3, void* heap, int devId);
WOLFSSL_API void wc_Des3Free(Des3* des3);

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* NO_DES3 */
#endif /* WOLF_CRYPT_DES3_H */

