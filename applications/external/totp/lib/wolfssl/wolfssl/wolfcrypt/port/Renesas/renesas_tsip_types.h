
/* renesas_tsip_types.h
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
 
#ifndef __RENESAS_TSIP_TYPES_H__
#define __RENESAS_TSIP_TYPES_H__


#include <wolfssl/wolfcrypt/settings.h>

#if (!defined(NO_SHA) || !defined(NO_SHA256)) && \
    !defined(NO_WOLFSSL_RENESAS_TSIP_CRYPT_HASH)
typedef enum {
    TSIP_SHA1 = 0,
    TSIP_SHA256 = 1,
} TSIP_SHA_TYPE;

typedef enum {
    TSIP_RSA2048,
    TSIP_RSA4096,
    TSIP_ECCP256,
} TSIP_KEY_TYPE;

typedef struct {
    byte*  msg;
    void*  heap;
    word32 used;
    word32 len;
    word32 sha_type;
#if defined(WOLF_CRYPTO_CB)
    word32 flags;
    int devId;
#endif    
} wolfssl_TSIP_Hash;

/* RAW hash function APIs are not implemented with TSIP */
#define WOLFSSL_NO_HASH_RAW

typedef wolfssl_TSIP_Hash wc_Sha;

#if !defined(NO_SHA256)
typedef wolfssl_TSIP_Hash wc_Sha256;
#endif

#endif /* NO_SHA */


#if defined(WOLFSSL_RENESAS_TSIP_TLS_AES_CRYPT) ||\
    defined(WOLFSSL_RENESAS_TSIP_CRYPTONLY)
#include "r_tsip_rx_if.h"

typedef struct {
    tsip_aes_key_index_t tsip_keyIdx;
    word32               keySize;
    byte                 setup;
} TSIP_AES_CTX;

#endif

#endif /* __RENESAS_TSIP_TYPES_H__ */
