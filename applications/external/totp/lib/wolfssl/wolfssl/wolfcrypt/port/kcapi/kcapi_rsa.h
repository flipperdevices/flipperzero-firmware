/* kcapi_rsa.h
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


#ifndef WOLF_CRYPT_KCAPI_RSA_H
#define WOLF_CRYPT_KCAPI_RSA_H

#include <wolfssl/wolfcrypt/types.h>
#include <wolfssl/wolfcrypt/rsa.h>
#include <wolfssl/wolfcrypt/port/kcapi/wc_kcapi.h>

#ifndef WC_RSAKEY_TYPE_DEFINED
    typedef struct RsaKey RsaKey;
    #define WC_RSAKEY_TYPE_DEFINED
#endif

void KcapiRsa_Free(RsaKey* key);
int KcapiRsa_Decrypt(RsaKey* key, const byte* in, word32 inLen, byte* out,
                     word32* outLen);
int KcapiRsa_Encrypt(RsaKey* key, const byte* in, word32 inLen, byte* out,
                     word32* outLen);

#endif /* WOLF_CRYPT_KCAPI_RSA_H  */

