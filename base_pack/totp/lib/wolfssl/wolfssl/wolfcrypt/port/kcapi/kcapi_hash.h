/* kcapi_hash.h
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


#ifndef WOLF_CRYPT_KCAPI_HASH_H
#define WOLF_CRYPT_KCAPI_HASH_H

#include <wolfssl/wolfcrypt/types.h>
#include <wolfssl/wolfcrypt/port/kcapi/wc_kcapi.h>

#undef  WOLFSSL_NO_HASH_RAW
#define WOLFSSL_NO_HASH_RAW

typedef struct wolfssl_KCAPI_Hash {
    void*                heap;
    struct kcapi_handle* handle;
    char                 type[10];
#if defined(WOLFSSL_KCAPI_HASH_KEEP)
    byte*                msg;
    word32               used;
    word32               len;
#endif
} wolfssl_KCAPI_Hash;

#ifndef WC_SHA512_TYPE_DEFINED
    typedef struct wc_Sha512 wc_Sha512;
    typedef struct wc_Sha512 wc_Sha512_224;
    typedef struct wc_Sha512 wc_Sha512_256;

#define WC_SHA512_TYPE_DEFINED
#endif

WOLFSSL_LOCAL int Sha512_Family_Final(wc_Sha512* sha512, byte* hash,
    enum wc_HashType type);
WOLFSSL_LOCAL int Sha512_Family_GetHash(wc_Sha512* sha512, byte* hash,
    enum wc_HashType type);

WOLFSSL_LOCAL void KcapiHashFree(wolfssl_KCAPI_Hash* hash);

#endif /* WOLF_CRYPT_KCAPI_HASH_H  */

