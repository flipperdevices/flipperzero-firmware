/* sphincs.h
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
    \file wolfssl/wolfcrypt/sphincs.h
*/

/* Interfaces for Sphincs:
 *     - SPHINCS_FAST_LEVEL1 (AKA SPHINCS+-SHAKE-128f-simple)
 *     - SPHINCS_FAST_LEVEL3 (AKA SPHINCS+-SHAKE-192f-simple)
 *     - SPHINCS_FAST_LEVEL5 (AKA SPHINCS+-SHAKE-256f-simple)
 *     - SPHINCS_SMALL_LEVEL1 (AKA SPHINCS+-SHAKE-128s-simple)
 *     - SPHINCS_SMALL_LEVEL3 (AKA SPHINCS+-SHAKE-192s-simple)
 *     - SPHINCS_SMALL_LEVEL5 (AKA SPHINCS+-SHAKE-256s-simple)
 */

#ifndef WOLF_CRYPT_SPHINCS_H
#define WOLF_CRYPT_SPHINCS_H

#include <wolfssl/wolfcrypt/types.h>

#if defined(HAVE_PQC) && defined(HAVE_SPHINCS)

#ifdef HAVE_LIBOQS
#include <oqs/oqs.h>
#endif

#ifdef __cplusplus
    extern "C" {
#endif

/* Macros Definitions */

#ifdef HAVE_LIBOQS

#define SPHINCS_FAST_LEVEL1_SIG_SIZE     OQS_SIG_sphincs_shake_128f_simple_length_signature
#define SPHINCS_FAST_LEVEL3_SIG_SIZE     OQS_SIG_sphincs_shake_192f_simple_length_signature
#define SPHINCS_FAST_LEVEL5_SIG_SIZE     OQS_SIG_sphincs_shake_256f_simple_length_signature
#define SPHINCS_SMALL_LEVEL1_SIG_SIZE    OQS_SIG_sphincs_shake_128s_simple_length_signature
#define SPHINCS_SMALL_LEVEL3_SIG_SIZE    OQS_SIG_sphincs_shake_192s_simple_length_signature
#define SPHINCS_SMALL_LEVEL5_SIG_SIZE    OQS_SIG_sphincs_shake_256s_simple_length_signature

#define SPHINCS_LEVEL1_KEY_SIZE     OQS_SIG_sphincs_shake_128f_simple_length_secret_key
#define SPHINCS_LEVEL1_PUB_KEY_SIZE OQS_SIG_sphincs_shake_128f_simple_length_public_key
#define SPHINCS_LEVEL1_PRV_KEY_SIZE (SPHINCS_LEVEL1_PUB_KEY_SIZE+SPHINCS_LEVEL1_KEY_SIZE)

#define SPHINCS_LEVEL3_KEY_SIZE     OQS_SIG_sphincs_shake_192f_simple_length_secret_key
#define SPHINCS_LEVEL3_PUB_KEY_SIZE OQS_SIG_sphincs_shake_192f_simple_length_public_key
#define SPHINCS_LEVEL3_PRV_KEY_SIZE (SPHINCS_LEVEL3_PUB_KEY_SIZE+SPHINCS_LEVEL3_KEY_SIZE)

#define SPHINCS_LEVEL5_KEY_SIZE     OQS_SIG_sphincs_shake_256f_simple_length_secret_key
#define SPHINCS_LEVEL5_PUB_KEY_SIZE OQS_SIG_sphincs_shake_256f_simple_length_public_key
#define SPHINCS_LEVEL5_PRV_KEY_SIZE (SPHINCS_LEVEL5_PUB_KEY_SIZE+SPHINCS_LEVEL5_KEY_SIZE)
#endif

#define SPHINCS_MAX_SIG_SIZE     SPHINCS_FAST_LEVEL5_SIG_SIZE
#define SPHINCS_MAX_KEY_SIZE     SPHINCS_LEVEL5_PRV_KEY_SIZE
#define SPHINCS_MAX_PUB_KEY_SIZE SPHINCS_LEVEL5_PUB_KEY_SIZE
#define SPHINCS_MAX_PRV_KEY_SIZE SPHINCS_LEVEL5_PRV_KEY_SIZE

#define FAST_VARIANT    1
#define SMALL_VARIANT   2

/* Structs */

struct sphincs_key {
    bool pubKeySet;
    bool prvKeySet;
    byte level; /* 1,3 or 5 */
    byte optim; /* FAST_VARIANT or SMALL_VARIANT */
    byte p[SPHINCS_MAX_PUB_KEY_SIZE];
    byte k[SPHINCS_MAX_PRV_KEY_SIZE];
};

#ifndef WC_SPHINCSKEY_TYPE_DEFINED
    typedef struct sphincs_key sphincs_key;
    #define WC_SPHINCSKEY_TYPE_DEFINED
#endif

/* Functions */

WOLFSSL_API
int wc_sphincs_sign_msg(const byte* in, word32 inLen, byte* out, word32 *outLen,
                        sphincs_key* key);
WOLFSSL_API
int wc_sphincs_verify_msg(const byte* sig, word32 sigLen, const byte* msg,
                          word32 msgLen, int* res, sphincs_key* key);

WOLFSSL_API
int wc_sphincs_init(sphincs_key* key);
WOLFSSL_API
int wc_sphincs_set_level_and_optim(sphincs_key* key, byte level, byte optim);
WOLFSSL_API
int wc_sphincs_get_level_and_optim(sphincs_key* key, byte* level, byte *optim);
WOLFSSL_API
void wc_sphincs_free(sphincs_key* key);

WOLFSSL_API
int wc_sphincs_import_public(const byte* in, word32 inLen, sphincs_key* key);
WOLFSSL_API
int wc_sphincs_import_private_only(const byte* priv, word32 privSz,
                                   sphincs_key* key);
WOLFSSL_API
int wc_sphincs_import_private_key(const byte* priv, word32 privSz,
                                  const byte* pub, word32 pubSz,
                                  sphincs_key* key);

WOLFSSL_API
int wc_sphincs_export_public(sphincs_key*, byte* out, word32* outLen);
WOLFSSL_API
int wc_sphincs_export_private_only(sphincs_key* key, byte* out, word32* outLen);
WOLFSSL_API
int wc_sphincs_export_private(sphincs_key* key, byte* out, word32* outLen);
WOLFSSL_API
int wc_sphincs_export_key(sphincs_key* key, byte* priv, word32 *privSz,
                          byte* pub, word32 *pubSz);

WOLFSSL_API
int wc_sphincs_check_key(sphincs_key* key);

WOLFSSL_API
int wc_sphincs_size(sphincs_key* key);
WOLFSSL_API
int wc_sphincs_priv_size(sphincs_key* key);
WOLFSSL_API
int wc_sphincs_pub_size(sphincs_key* key);
WOLFSSL_API
int wc_sphincs_sig_size(sphincs_key* key);

WOLFSSL_API int wc_Sphincs_PrivateKeyDecode(const byte* input,
                                            word32* inOutIdx,
                                            sphincs_key* key, word32 inSz);
WOLFSSL_API int wc_Sphincs_PublicKeyDecode(const byte* input,
                                           word32* inOutIdx,
                                           sphincs_key* key, word32 inSz);
WOLFSSL_API int wc_Sphincs_KeyToDer(sphincs_key* key, byte* output,
                                    word32 inLen);
WOLFSSL_API int wc_Sphincs_PrivateKeyToDer(sphincs_key* key, byte* output,
                                           word32 inLen);
WOLFSSL_API int wc_Sphincs_PublicKeyToDer(sphincs_key* key, byte* output,
                                          word32 inLen, int withAlg);

#ifdef __cplusplus
    }    /* extern "C" */
#endif

#endif /* HAVE_PQC && HAVE_SPHINCS */
#endif /* WOLF_CRYPT_SPHINCS_H */
