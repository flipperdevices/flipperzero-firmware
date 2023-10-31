/* kcapi_ecc.h
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


#ifndef WOLF_CRYPT_KCAPI_ECC_H
#define WOLF_CRYPT_KCAPI_ECC_H

#define KCAPI_PARAM_SZ     2


#include <wolfssl/wolfcrypt/types.h>
#include <wolfssl/wolfcrypt/ecc.h>
#include <wolfssl/wolfcrypt/port/kcapi/wc_kcapi.h>


#ifndef WC_ECCKEY_TYPE_DEFINED
    typedef struct ecc_key ecc_key;
    #define WC_ECCKEY_TYPE_DEFINED
#endif

WOLFSSL_LOCAL void KcapiEcc_Free(ecc_key* key);
WOLFSSL_LOCAL int KcapiEcc_MakeKey(ecc_key* key, int keysize, int curve_id);
WOLFSSL_LOCAL int KcapiEcc_LoadKey(ecc_key* key, byte* pubkey_raw,
                  word32* pubkey_sz, int release_handle);
WOLFSSL_LOCAL int KcapiEcc_SharedSecret(ecc_key* private_key,
                  ecc_key* public_key, byte* out, word32* outlen);
WOLFSSL_LOCAL int KcapiEcc_Sign(ecc_key* key, const byte* hash, word32 hashLen,
                  byte* sig, word32 sigLen);
WOLFSSL_LOCAL int KcapiEcc_Verify(ecc_key* key, const byte* hash, word32 hashLen,
                  byte* sig, word32 sigLen);

#endif /* WOLF_CRYPT_KCAPI_ECC_H  */

