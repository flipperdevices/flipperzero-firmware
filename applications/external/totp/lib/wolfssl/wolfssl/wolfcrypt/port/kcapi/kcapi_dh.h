/* kcapi_dh.h
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


#ifndef WOLF_CRYPT_KCAPI_DH_H
#define WOLF_CRYPT_KCAPI_DH_H

#include <wolfssl/wolfcrypt/types.h>
#include <wolfssl/wolfcrypt/dh.h>
#include <wolfssl/wolfcrypt/port/kcapi/wc_kcapi.h>

#ifndef WC_DH_TYPE_DEFINED
    typedef struct DhKey DhKey;
    #define WC_DH_TYPE_DEFINED
#endif

void KcapiDh_Free(DhKey* key);
int KcapiDh_MakeKey(DhKey* key, byte* pub, word32* pubSz);
int KcapiDh_SharedSecret(DhKey* private_key, const byte* pub, word32 pubSz,
                         byte* out, word32* outlen);

#endif /* WOLF_CRYPT_KCAPI_DH_H  */

