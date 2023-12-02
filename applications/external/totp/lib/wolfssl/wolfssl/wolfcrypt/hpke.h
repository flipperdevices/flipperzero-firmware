/* hpke.h
 *
 * Copyright (C) 2006-2022 wolfSSL Inc.
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
    \file wolfssl/wolfcrypt/hpke.h
*/

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/types.h>
#include <wolfssl/wolfcrypt/ecc.h>

#ifdef __cplusplus
    extern "C" {
#endif

#if defined(HAVE_HPKE) && defined(HAVE_ECC)

#ifndef WOLFCRYPT_HPKE
#define WOLFCRYPT_HPKE

/* KEM enum */
enum {
    DHKEM_P256_HKDF_SHA256 = 0x0010,
    DHKEM_P384_HKDF_SHA384 = 0x0011,
    DHKEM_P521_HKDF_SHA512 = 0x0012,
    DHKEM_X25519_HKDF_SHA256 = 0x0020,
    DHKEM_X448_HKDF_SHA512 = 0x0021,
};

#define DHKEM_P256_ENC_LEN 65
#define DHKEM_P384_ENC_LEN 97
#define DHKEM_P521_ENC_LEN 133
#define DHKEM_X25519_ENC_LEN 32
#define DHKEM_X448_ENC_LEN 56

/* KDF enum */
enum {
    HKDF_SHA256 = 0x0001,
    HKDF_SHA384 = 0x0002,
    HKDF_SHA512 = 0x0003,
};

/* AEAD enum */
enum {
    HPKE_AES_128_GCM = 0x0001,
    HPKE_AES_256_GCM = 0x0002,
};

/* TODO better way of doing this */
#define HPKE_SUPPORTED_KEM_LEN 4
#define HPKE_SUPPORTED_KDF_LEN 3
#define HPKE_SUPPORTED_AEAD_LEN 2
extern const int hpkeSupportedKem[HPKE_SUPPORTED_KEM_LEN];
extern const int hpkeSupportedKdf[HPKE_SUPPORTED_KDF_LEN];
extern const int hpkeSupportedAead[HPKE_SUPPORTED_AEAD_LEN];

#define HPKE_Nh_MAX 64
#define HPKE_Nk_MAX 32
#define HPKE_Nn_MAX 12
#define HPKE_Nt_MAX 16
#define HPKE_Ndh_MAX 66
#define HPKE_Npk_MAX 133
#define HPKE_Nsecret_MAX 64
#define KEM_SUITE_ID_LEN 5
#define HPKE_SUITE_ID_LEN 10

#ifndef MAX_HPKE_LABEL_SZ
#define MAX_HPKE_LABEL_SZ 512
#endif

typedef struct {
    void* heap;
    word32 kem;
    word32 kdf;
    word32 aead;
    word32 Nh;
    word32 Nk;
    word32 Nn;
    word32 Nt;
    word32 Ndh;
    word32 Npk;
    word32 Nsecret;
    int kdf_digest;
    int curve_id;
    byte kem_suite_id[KEM_SUITE_ID_LEN];
    byte hpke_suite_id[HPKE_SUITE_ID_LEN];
} Hpke;

typedef struct {
    int seq;
    byte key[HPKE_Nk_MAX];
    byte base_nonce[HPKE_Nn_MAX];
    byte exporter_secret[HPKE_Nsecret_MAX];
} HpkeBaseContext;


WOLFSSL_API int wc_HpkeInit(Hpke* hpke, int kem, int kdf, int aead, void* heap);
WOLFSSL_API int wc_HpkeGenerateKeyPair(Hpke* hpke, void** keypair, WC_RNG* rng);
WOLFSSL_API int wc_HpkeSerializePublicKey(Hpke* hpke, void* key, byte* out,
    word16* outSz);
WOLFSSL_API int wc_HpkeDeserializePublicKey(Hpke* hpke, void** key,
    const byte* in, word16 inSz);
WOLFSSL_API void wc_HpkeFreeKey(Hpke* hpke, word16 kem, void* keypair,
    void* heap);
WOLFSSL_API int wc_HpkeSealBase(Hpke* hpke, void* ephemeralKey,
    void* receiverKey, byte* info, word32 infoSz, byte* aad, word32 aadSz,
    byte* plaintext, word32 ptSz, byte* ciphertext);
WOLFSSL_API int wc_HpkeOpenBase(Hpke* hpke, void* receiverKey,
    const byte* pubKey, word16 pubKeySz, byte* info, word32 infoSz, byte* aad,
    word32 aadSz, byte* ciphertext, word32 ctSz, byte* plaintext);

#endif

#endif /* HAVE_HPKE && HAVE_ECC */

#ifdef __cplusplus
    }    /* extern "C" */
#endif
