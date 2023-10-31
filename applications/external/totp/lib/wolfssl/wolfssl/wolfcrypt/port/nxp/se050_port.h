/* se050_port.h
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

#ifndef _SE050_PORT_H_
#define _SE050_PORT_H_

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/visibility.h>
#include <wolfssl/wolfcrypt/types.h> /* for MATH_INT_T */

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wredundant-decls"
#endif

#include "fsl_sss_se05x_types.h"
#include "fsl_sss_se05x_apis.h"
#include "se05x_APDU.h"

#if (SSS_HAVE_SSS > 1)
#include "fsl_sss_api.h"
#endif

#if defined(WOLFSSL_SE050) && defined(WOLFSSL_SE050_HASH)
    /* NXP SE050 - Disable SHA512 224/256 support */
    #ifndef WOLFSSL_NOSHA512_224
    #define WOLFSSL_NOSHA512_224
    #endif
    #ifndef WOLFSSL_NOSHA512_256
    #define WOLFSSL_NOSHA512_256
    #endif
#endif

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif


/* Default key ID's */
#ifndef SE050_KEYSTOREID_AES
#define SE050_KEYSTOREID_AES     55
#endif
#ifndef SE050_KEYSTOREID_ED25519
#define SE050_KEYSTOREID_ED25519 58
#endif
#ifndef SE050_KEYSTOREID_CURVE25519
#define SE050_KEYSTOREID_CURVE25519 59
#endif
#ifndef SE050_KEYSTOREID_ECC
#define SE050_KEYSTOREID_ECC     60
#endif
#ifndef SE050_KEYSTOREID_RSA
#define SE050_KEYSTOREID_RSA     61
#endif
#ifndef SE050_KEYSTOREID_GENERIC
#define SE050_KEYSTOREID_GENERIC 62
#endif

/* old public API was renamed to add wc_ */
#define se050_ecc_insert_private_key wc_se050_ecc_insert_private_key

enum {
    SSS_BLOCK_SIZE   = 512,
    SSS_MAX_ECC_BITS = 521
};

enum SE050KeyType {
    SE050_ANY_KEY,
    SE050_AES_KEY,
    SE050_ECC_KEY,
    SE050_RSA_KEY,
    SE050_ED25519_KEY,
    SE050_CURVE25519_KEY
};


#ifdef WOLFSSL_SE050_HASH
typedef struct {
    void*  heap;
    byte*  msg;
    word32 used;
    word32 len;
} SE050_HASH_Context;
#endif

/* Public Functions */
WOLFSSL_API int wc_se050_set_config(sss_session_t *pSession,
    sss_key_store_t *pHostKeyStore, sss_key_store_t *pKeyStore);
#ifdef WOLFSSL_SE050_INIT
WOLFSSL_API int wc_se050_init(const char* portName);
#endif
WOLFSSL_API int wc_se050_erase_object(word32 keyId);

WOLFSSL_API int wc_se050_ecc_insert_public_key(word32 keyId,
    const byte* eccDer, word32 eccDerSize);
WOLFSSL_API int wc_se050_ecc_insert_private_key(word32 keyId,
    const byte* eccDer, word32 eccDerSize);

WOLFSSL_API int wc_se050_rsa_insert_public_key(word32 keyId,
    const byte* rsaDer, word32 rsaDerSize);
WOLFSSL_API int wc_se050_rsa_insert_private_key(word32 keyId,
    const byte* rsaDer, word32 rsaDerSize);

WOLFSSL_API int wc_se050_insert_binary_object(word32 keyId,
    const byte* object, word32 objectSz);
WOLFSSL_API int wc_se050_get_binary_object(word32 keyId,
    byte* out, word32* outSz);

/* Private Functions */
WOLFSSL_LOCAL word32 se050_allocate_key(int keyType);
#if !defined(WC_NO_RNG) && !defined(WOLFSSL_SE050_NO_TRNG)
WOLFSSL_LOCAL int se050_get_random_number(uint32_t count, uint8_t* rand_out);
#endif

#ifdef WOLFSSL_SE050_HASH
WOLFSSL_LOCAL int se050_hash_init(SE050_HASH_Context* se050Ctx, void* heap);
WOLFSSL_LOCAL int se050_hash_update(SE050_HASH_Context* se050Ctx,
    const byte* data, word32 len);
WOLFSSL_LOCAL int se050_hash_final(SE050_HASH_Context* se050Ctx, byte* hash,
    size_t digestLen, sss_algorithm_t algo);
WOLFSSL_LOCAL int se050_hash_copy(SE050_HASH_Context* src,
    SE050_HASH_Context* dst);
WOLFSSL_LOCAL void se050_hash_free(SE050_HASH_Context* se050Ctx);
#endif

#if defined(WOLFSSL_SE050_CRYPT) && !defined(NO_AES)
struct Aes;
WOLFSSL_LOCAL int se050_aes_free_key_store_object(struct Aes* aes);
WOLFSSL_LOCAL int se050_aes_set_key(struct Aes* aes, const byte* key,
    word32 len, const byte* iv, int dir);
WOLFSSL_LOCAL int se050_aes_crypt(struct Aes* aes, const byte* in, byte* out,
    word32 sz, int dir, sss_algorithm_t algorithm);
WOLFSSL_LOCAL void se050_aes_free(struct Aes* aes);
#endif

struct WC_RNG;
struct ecc_key;

WOLFSSL_LOCAL int se050_ecc_use_key_id(struct ecc_key* key, word32 keyId);
WOLFSSL_LOCAL int se050_ecc_get_key_id(struct ecc_key* key, word32* keyId);
WOLFSSL_LOCAL int se050_ecc_sign_hash_ex(const byte* in, word32 inLen,
    mp_int* r, mp_int* s, byte* out, word32 *outLen, struct ecc_key* key);

WOLFSSL_LOCAL int se050_ecc_verify_hash_ex(const byte* hash, word32 hashlen,
    mp_int* r, mp_int* s, struct ecc_key* key, int* res);

WOLFSSL_LOCAL int se050_ecc_create_key(struct ecc_key* key, int curve_id,
    int keySize);
WOLFSSL_LOCAL int se050_ecc_shared_secret(struct ecc_key* private_key,
    struct ecc_key* public_key, byte* out, word32* outlen);
WOLFSSL_LOCAL void se050_ecc_free_key(struct ecc_key* key);
#endif /* HAVE_ECC */

#ifndef NO_RSA
struct RsaKey;
WOLFSSL_LOCAL int se050_rsa_use_key_id(struct RsaKey* key, word32 keyId);
WOLFSSL_LOCAL int se050_rsa_get_key_id(struct RsaKey* key, word32* keyId);
WOLFSSL_LOCAL int se050_rsa_create_key(struct RsaKey* key, int size, long e);
WOLFSSL_LOCAL void se050_rsa_free_key(struct RsaKey* key);
WOLFSSL_LOCAL int se050_rsa_sign(const byte* in, word32 inLen, byte* out,
    word32 outLen, struct RsaKey* key, int rsa_type, byte pad_value,
    int pad_type, enum wc_HashType hash, int mgf, byte* label, word32 labelSz,
    int keySz);
WOLFSSL_LOCAL int se050_rsa_verify(const byte* in, word32 inLen, byte* out,
    word32 outLen, struct RsaKey* key, int rsa_type, byte pad_value,
    int pad_type, enum wc_HashType hash, int mgf, byte* label, word32 labelSz);
WOLFSSL_LOCAL int se050_rsa_public_encrypt(const byte* in, word32 inLen,
    byte* out, word32 outLen, struct RsaKey* key, int rsa_type, byte pad_value,
    int pad_type, enum wc_HashType hash, int mgf, byte* label,
    word32 labelSz, int keySz);
WOLFSSL_LOCAL int se050_rsa_private_decrypt(const byte* in, word32 inLen,
    byte* out, word32 outLen, struct RsaKey* key, int rsa_type, byte pad_value,
    int pad_type, enum wc_HashType hash, int mgf, byte* label, word32 labelSz);
#endif

#ifdef HAVE_ED25519
struct ed25519_key;
WOLFSSL_LOCAL int se050_ed25519_create_key(struct ed25519_key* key);
WOLFSSL_LOCAL void se050_ed25519_free_key(struct ed25519_key* key);
WOLFSSL_LOCAL int se050_ed25519_sign_msg(const byte* in, word32 inLen,
    byte* out, word32 *outLen, struct ed25519_key* key);
WOLFSSL_LOCAL int se050_ed25519_verify_msg(const byte* signature,
    word32 signatureLen, const byte* msg, word32 msgLen,
    struct ed25519_key* key, int* res);
#endif /* HAVE_ED25519 */

#ifdef HAVE_CURVE25519
struct curve25519_key;
struct ECPoint;
WOLFSSL_LOCAL int se050_curve25519_create_key(struct curve25519_key* key,
    int keySize);
WOLFSSL_LOCAL int se050_curve25519_shared_secret(
    struct curve25519_key* private_key, struct curve25519_key* public_key,
    struct ECPoint* out);
WOLFSSL_LOCAL void se050_curve25519_free_key(struct curve25519_key* key);
#endif /* HAVE_CURVE25519 */
#endif /* _SE050_PORT_H_ */
