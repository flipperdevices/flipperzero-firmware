/* dilithium.h
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
    \file wolfssl/wolfcrypt/dilithium.h
*/

/* Interfaces for Dilithium NIST Level 1 (Dilithium512) and Dilithium NIST Level 5
 * (Dilithium1024). */

#ifndef WOLF_CRYPT_DILITHIUM_H
#define WOLF_CRYPT_DILITHIUM_H

#include <wolfssl/wolfcrypt/types.h>

#if defined(HAVE_PQC) && defined(HAVE_DILITHIUM)

#ifdef HAVE_LIBOQS
#include <oqs/oqs.h>
#endif

#ifdef __cplusplus
    extern "C" {
#endif

/* Macros Definitions */

#ifdef HAVE_LIBOQS
#define DILITHIUM_LEVEL2_KEY_SIZE     OQS_SIG_dilithium_2_length_secret_key
#define DILITHIUM_LEVEL2_SIG_SIZE     OQS_SIG_dilithium_2_length_signature
#define DILITHIUM_LEVEL2_PUB_KEY_SIZE OQS_SIG_dilithium_2_length_public_key
#define DILITHIUM_LEVEL2_PRV_KEY_SIZE (DILITHIUM_LEVEL2_PUB_KEY_SIZE+DILITHIUM_LEVEL2_KEY_SIZE)

#define DILITHIUM_LEVEL3_KEY_SIZE     OQS_SIG_dilithium_3_length_secret_key
#define DILITHIUM_LEVEL3_SIG_SIZE     OQS_SIG_dilithium_3_length_signature
#define DILITHIUM_LEVEL3_PUB_KEY_SIZE OQS_SIG_dilithium_3_length_public_key
#define DILITHIUM_LEVEL3_PRV_KEY_SIZE (DILITHIUM_LEVEL3_PUB_KEY_SIZE+DILITHIUM_LEVEL3_KEY_SIZE)

#define DILITHIUM_LEVEL5_KEY_SIZE     OQS_SIG_dilithium_5_length_secret_key
#define DILITHIUM_LEVEL5_SIG_SIZE     OQS_SIG_dilithium_5_length_signature
#define DILITHIUM_LEVEL5_PUB_KEY_SIZE OQS_SIG_dilithium_5_length_public_key
#define DILITHIUM_LEVEL5_PRV_KEY_SIZE (DILITHIUM_LEVEL5_PUB_KEY_SIZE+DILITHIUM_LEVEL5_KEY_SIZE)
#endif

#define DILITHIUM_MAX_KEY_SIZE     DILITHIUM_LEVEL5_PRV_KEY_SIZE
#define DILITHIUM_MAX_SIG_SIZE     DILITHIUM_LEVEL5_SIG_SIZE
#define DILITHIUM_MAX_PUB_KEY_SIZE DILITHIUM_LEVEL5_PUB_KEY_SIZE
#define DILITHIUM_MAX_PRV_KEY_SIZE DILITHIUM_LEVEL5_PRV_KEY_SIZE

/* Structs */

struct dilithium_key {
    bool pubKeySet;
    bool prvKeySet;
    byte level; /* 2,3 or 5 */
    byte p[DILITHIUM_MAX_PUB_KEY_SIZE];
    byte k[DILITHIUM_MAX_PRV_KEY_SIZE];
};

#ifndef WC_DILITHIUMKEY_TYPE_DEFINED
    typedef struct dilithium_key dilithium_key;
    #define WC_DILITHIUMKEY_TYPE_DEFINED
#endif

/* Functions */

WOLFSSL_API
int wc_dilithium_sign_msg(const byte* in, word32 inLen, byte* out, word32 *outLen,
                       dilithium_key* key);
WOLFSSL_API
int wc_dilithium_verify_msg(const byte* sig, word32 sigLen, const byte* msg,
                         word32 msgLen, int* res, dilithium_key* key);

WOLFSSL_API
int wc_dilithium_init(dilithium_key* key);
WOLFSSL_API
int wc_dilithium_set_level(dilithium_key* key, byte level);
WOLFSSL_API
int wc_dilithium_get_level(dilithium_key* key, byte* level);
WOLFSSL_API
void wc_dilithium_free(dilithium_key* key);

WOLFSSL_API
int wc_dilithium_import_public(const byte* in, word32 inLen, dilithium_key* key);
WOLFSSL_API
int wc_dilithium_import_private_only(const byte* priv, word32 privSz,
                                  dilithium_key* key);
WOLFSSL_API
int wc_dilithium_import_private_key(const byte* priv, word32 privSz,
                                 const byte* pub, word32 pubSz,
                                 dilithium_key* key);

WOLFSSL_API
int wc_dilithium_export_public(dilithium_key*, byte* out, word32* outLen);
WOLFSSL_API
int wc_dilithium_export_private_only(dilithium_key* key, byte* out, word32* outLen);
WOLFSSL_API
int wc_dilithium_export_private(dilithium_key* key, byte* out, word32* outLen);
WOLFSSL_API
int wc_dilithium_export_key(dilithium_key* key, byte* priv, word32 *privSz,
                         byte* pub, word32 *pubSz);

WOLFSSL_API
int wc_dilithium_check_key(dilithium_key* key);

WOLFSSL_API
int wc_dilithium_size(dilithium_key* key);
WOLFSSL_API
int wc_dilithium_priv_size(dilithium_key* key);
WOLFSSL_API
int wc_dilithium_pub_size(dilithium_key* key);
WOLFSSL_API
int wc_dilithium_sig_size(dilithium_key* key);

WOLFSSL_API int wc_Dilithium_PrivateKeyDecode(const byte* input,
                                              word32* inOutIdx,
                                              dilithium_key* key, word32 inSz);
WOLFSSL_API int wc_Dilithium_PublicKeyDecode(const byte* input,
                                             word32* inOutIdx,
                                             dilithium_key* key, word32 inSz);
WOLFSSL_API int wc_Dilithium_KeyToDer(dilithium_key* key, byte* output,
                                      word32 inLen);
WOLFSSL_API int wc_Dilithium_PrivateKeyToDer(dilithium_key* key, byte* output,
                                             word32 inLen);
WOLFSSL_API int wc_Dilithium_PublicKeyToDer(dilithium_key* key, byte* output,
                                            word32 inLen, int withAlg);

#ifdef __cplusplus
    }    /* extern "C" */
#endif

#endif /* HAVE_PQC && HAVE_DILITHIUM */
#endif /* WOLF_CRYPT_DILITHIUM_H */
