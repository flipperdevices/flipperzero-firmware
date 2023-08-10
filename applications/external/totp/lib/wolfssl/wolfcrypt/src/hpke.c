/* hpke.c
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

/* The HPKE supports ECC and X25519 with AES GCM only.
 * TODO: Add X448 and ChaCha20
 */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#if defined(HAVE_HPKE) && (defined(HAVE_ECC) || defined(HAVE_CURVE25519)) && \
    defined(HAVE_AESGCM)

#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/ecc.h>
#include <wolfssl/wolfcrypt/curve25519.h>
#include <wolfssl/wolfcrypt/curve448.h>
#include <wolfssl/wolfcrypt/hmac.h>
#include <wolfssl/wolfcrypt/hash.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/sha512.h>
#include <wolfssl/wolfcrypt/aes.h>
#include <wolfssl/wolfcrypt/hpke.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

const int hpkeSupportedKem[HPKE_SUPPORTED_KEM_LEN] = {
    DHKEM_P256_HKDF_SHA256,
    DHKEM_P384_HKDF_SHA384,
    DHKEM_P521_HKDF_SHA512,
    DHKEM_X25519_HKDF_SHA256,
};

const int hpkeSupportedKdf[HPKE_SUPPORTED_KDF_LEN] = {
    HKDF_SHA256,
    HKDF_SHA384,
    HKDF_SHA512,
};

const int hpkeSupportedAead[HPKE_SUPPORTED_AEAD_LEN] = {
    HPKE_AES_128_GCM,
    HPKE_AES_256_GCM,
};

static const char* KEM_STR = "KEM";
static const int   KEM_STR_LEN = 3;

static const char* HPKE_STR = "HPKE";
static const int   HPKE_STR_LEN = 4;

static const char* HPKE_VERSION_STR = "HPKE-v1";
static const int   HPKE_VERSION_STR_LEN = 7;

static const char* EAE_PRK_LABEL_STR = "eae_prk";
static const int   EAE_PRK_LABEL_STR_LEN = 7;

static const char* SHARED_SECRET_LABEL_STR = "shared_secret";
static const int   SHARED_SECRET_LABEL_STR_LEN = 13;

static const char* PSK_ID_HASH_LABEL_STR = "psk_id_hash";
static const int   PSK_ID_HASH_LABEL_STR_LEN = 11;

static const char* INFO_HASH_LABEL_STR = "info_hash";
static const int   INFO_HASH_LABEL_STR_LEN = 9;

static const char* SECRET_LABEL_STR = "secret";
static const int   SECRET_LABEL_STR_LEN = 6;

static const char* KEY_LABEL_STR = "key";
static const int   KEY_LABEL_STR_LEN = 3;

static const char* BASE_NONCE_LABEL_STR = "base_nonce";
static const int   BASE_NONCE_LABEL_STR_LEN = 10;

static const char* EXP_LABEL_STR = "exp";
static const int   EXP_LABEL_STR_LEN = 3;

/* encode n as a byte string with length w, return 0 or error */
static int I2OSP(int n, int w, byte* out)
{
    int i;

    if (w <= 0 || w > 32) {
        return MP_VAL;
    }

    /* if width is less than int max check that n is less than w bytes max */
    /* if width is greater than int max check that n is less than int max */
    if ((w < 4 && n > ((1 << (w * 8)) - 1)) || (w >= 4 && n > 0x7fffffff)) {
        return MP_VAL;
    }

    /* make sure the byte string is cleared */
    XMEMSET(out, 0, (size_t)w);

    for (i = 0; i < w && n > 0; i++) {
        out[w-(i + 1)] = (byte)n;
        n >>= 8;
    }

    return 0;
}

/* initialize the hpke struct with the desired ciphersuites, return 0 or error*/
int wc_HpkeInit(Hpke* hpke, int kem, int kdf, int aead, void* heap)
{
    int ret;
    byte* id;

    if (hpke == NULL || kem == 0 || kdf == 0 || aead == 0) {
        return BAD_FUNC_ARG;
    }

    XMEMSET(hpke, 0, sizeof(*hpke));
    hpke->kem = (word32)kem;
    hpke->kdf = (word32)kdf;
    hpke->aead = (word32)aead;
    hpke->heap = heap;

    /* set kem_suite_id */
    id = hpke->kem_suite_id;

    XMEMCPY(id, KEM_STR, KEM_STR_LEN);
    id += KEM_STR_LEN;

    ret = I2OSP(kem, 2, id);

    /* set hpke_suite_id */
    id = hpke->hpke_suite_id;

    XMEMCPY(id, HPKE_STR, HPKE_STR_LEN);
    id += HPKE_STR_LEN;

    if (ret == 0) {
        ret = I2OSP(kem, 2, id);
        id += 2;
    }
    if (ret == 0) {
        ret = I2OSP(kdf, 2, id);
        id += 2;
    }
    if (ret == 0) {
        ret = I2OSP(aead, 2, id);
    }

    if (ret == 0) {
        switch (kem) {
#if defined(HAVE_ECC)
#if defined(WOLFSSL_SHA224) || !defined(NO_SHA256)
        case DHKEM_P256_HKDF_SHA256:
            hpke->curve_id = ECC_SECP256R1;
            hpke->Nsecret = WC_SHA256_DIGEST_SIZE;
            hpke->Nh = WC_SHA256_DIGEST_SIZE;
            hpke->Ndh = (word32)wc_ecc_get_curve_size_from_id(hpke->curve_id);
            hpke->Npk = 1 + hpke->Ndh * 2;
            break;
#endif

#ifdef WOLFSSL_SHA384
        case DHKEM_P384_HKDF_SHA384:
            hpke->curve_id = ECC_SECP384R1;
            hpke->Nsecret = WC_SHA384_DIGEST_SIZE;
            hpke->Nh = WC_SHA384_DIGEST_SIZE;
            hpke->Ndh = (word32)wc_ecc_get_curve_size_from_id(hpke->curve_id);
            hpke->Npk = 1 + hpke->Ndh * 2;
            break;
#endif

#if defined(WOLFSSL_SHA384) || defined(WOLFSSL_SHA512)
        case DHKEM_P521_HKDF_SHA512:
            hpke->curve_id = ECC_SECP521R1;
            hpke->Nsecret = WC_SHA512_DIGEST_SIZE;
            hpke->Nh = WC_SHA512_DIGEST_SIZE;
            hpke->Ndh = (word32)wc_ecc_get_curve_size_from_id(hpke->curve_id);
            hpke->Npk = 1 + hpke->Ndh * 2;
            break;
#endif
#endif

#if defined(HAVE_CURVE25519) &&\
    (defined(WOLFSSL_SHA224) || !defined(NO_SHA256))
        case DHKEM_X25519_HKDF_SHA256:
            hpke->Nsecret = WC_SHA256_DIGEST_SIZE;
            hpke->Nh = WC_SHA256_DIGEST_SIZE;
            hpke->Ndh = CURVE25519_KEYSIZE;
            hpke->Npk = CURVE25519_PUB_KEY_SIZE;
            break;
#endif

#if defined(HAVE_CURVE448) &&\
    (defined(WOLFSSL_SHA384) || defined(WOLFSSL_SHA512))
        case DHKEM_X448_HKDF_SHA512:
            hpke->Nsecret = WC_SHA512_DIGEST_SIZE;
            hpke->Nh = WC_SHA512_DIGEST_SIZE;
            /* size of x448 shared secret */
            hpke->Ndh = 64;
            hpke->Npk = CURVE448_PUB_KEY_SIZE;
            ret = BAD_FUNC_ARG; /* TODO: Add X448 */
            break;
#endif

        default:
            ret = BAD_FUNC_ARG;
            break;
        }
    }

    if (ret == 0) {
        switch (kdf) {
        case HKDF_SHA256:
            hpke->kdf_digest = WC_SHA256;
            break;

        case HKDF_SHA384:
            hpke->kdf_digest = WC_SHA384;
            break;

        case HKDF_SHA512:
            hpke->kdf_digest = WC_SHA512;
            break;

        default:
            ret = BAD_FUNC_ARG;
            break;
        }
    }

    if (ret == 0) {
        switch (aead) {
        case HPKE_AES_128_GCM:
            hpke->Nk = AES_128_KEY_SIZE;
            hpke->Nn = GCM_NONCE_MID_SZ;
            hpke->Nt = AES_BLOCK_SIZE;
            break;

        case HPKE_AES_256_GCM:
            hpke->Nk = AES_256_KEY_SIZE;
            hpke->Nn = GCM_NONCE_MID_SZ;
            hpke->Nt = AES_BLOCK_SIZE;
            break;

        default:
            ret = BAD_FUNC_ARG;
            break;
        }
    }

    if ((int)hpke->Ndh < 0) {
        return (int)hpke->Ndh;
    }

    return ret;
}

/* generate a keypair for use with the supplied hpke kem method, return 0 or
 * error */
int wc_HpkeGenerateKeyPair(Hpke* hpke, void** keypair, WC_RNG* rng)
{
    int ret = 0;

    if (hpke == NULL || keypair == NULL || rng == NULL)
        return BAD_FUNC_ARG;

    switch (hpke->kem) {
#if defined(HAVE_ECC)
        case DHKEM_P256_HKDF_SHA256:
            *keypair = wc_ecc_key_new(hpke->heap);
            if (*keypair != NULL)
                ret = wc_ecc_make_key_ex(rng, 32, (ecc_key*)*keypair,
                    ECC_SECP256R1);
            break;
        case DHKEM_P384_HKDF_SHA384:
            *keypair = wc_ecc_key_new(hpke->heap);
            if (*keypair != NULL)
                ret = wc_ecc_make_key_ex(rng, 48, (ecc_key*)*keypair,
                    ECC_SECP384R1);
            break;
        case DHKEM_P521_HKDF_SHA512:
            *keypair = wc_ecc_key_new(hpke->heap);
            if (*keypair != NULL)
                ret = wc_ecc_make_key_ex(rng, 66, (ecc_key*)*keypair,
                  ECC_SECP521R1);
            break;
#endif
#if defined(HAVE_CURVE25519)
        case DHKEM_X25519_HKDF_SHA256:
            *keypair = XMALLOC(sizeof(curve25519_key), hpke->heap,
                DYNAMIC_TYPE_CURVE25519);
            if (*keypair != NULL) {
                ret = wc_curve25519_init_ex((curve25519_key*)*keypair,
                    hpke->heap, INVALID_DEVID);
                if (ret == 0)
                    ret = wc_curve25519_make_key(rng, 32,
                        (curve25519_key*)*keypair);
            }
            break;
#endif
        case DHKEM_X448_HKDF_SHA512:
            /* TODO: Add X448 */
        default:
            ret = BAD_FUNC_ARG;
            break;
    }

    if (ret == 0 && *keypair == NULL)
        ret = MEMORY_E;

    if (ret != 0 && *keypair != NULL) {
        wc_HpkeFreeKey(hpke, (word16)hpke->kem, *keypair, hpke->heap);
        *keypair = NULL;
    }

    return ret;
}

/* encode the provided kem key into a byte string, return 0 or error */
int wc_HpkeSerializePublicKey(Hpke* hpke, void* key, byte* out, word16* outSz)
{
    int ret;
    word32 tmpOutSz;

    if (hpke == NULL || key == NULL || out == NULL || outSz == NULL) {
        return BAD_FUNC_ARG;
    }

    tmpOutSz = *outSz;

    switch (hpke->kem)
    {
#if defined(HAVE_ECC)
        case DHKEM_P256_HKDF_SHA256:
        case DHKEM_P384_HKDF_SHA384:
        case DHKEM_P521_HKDF_SHA512:
            /* export x963 uncompressed */
            ret = wc_ecc_export_x963_ex((ecc_key*)key, out, &tmpOutSz, 0);
            break;
#endif
#if defined(HAVE_CURVE25519)
        case DHKEM_X25519_HKDF_SHA256:
            ret = wc_curve25519_export_public_ex((curve25519_key*)key, out,
                &tmpOutSz, EC25519_LITTLE_ENDIAN);
            break;
#endif
        case DHKEM_X448_HKDF_SHA512:
        default:
            ret = -1;
            break;
    }

    *outSz = (word16)tmpOutSz;

    return ret;
}

/* load a serialized kem key into a wolfcrypt key struct depending on the kem */
int wc_HpkeDeserializePublicKey(Hpke* hpke, void** key, const byte* in,
    word16 inSz)
{
    int ret = 0;

    if (hpke == NULL || key == NULL || in == NULL) {
        return BAD_FUNC_ARG;
    }

    if (inSz < (word32)hpke->Npk) {
        return BUFFER_E;
    }

    switch (hpke->kem)
    {
#if defined(HAVE_ECC)
        case DHKEM_P256_HKDF_SHA256:
        case DHKEM_P384_HKDF_SHA384:
        case DHKEM_P521_HKDF_SHA512:
            /* init the ecc key */
            *key = wc_ecc_key_new(hpke->heap);
            if (*key != NULL) {
                /* import the x963 key */
                ret = wc_ecc_import_x963_ex(in, inSz, (ecc_key*)*key,
                    hpke->curve_id);
            }
            break;
#endif
#if defined(HAVE_CURVE25519)
        case DHKEM_X25519_HKDF_SHA256:
            *key = XMALLOC(sizeof(curve25519_key), hpke->heap,
                DYNAMIC_TYPE_CURVE25519);
            if (*key != NULL) {
                ret = wc_curve25519_init_ex((curve25519_key*)*key, hpke->heap,
                    INVALID_DEVID);
                if (ret == 0)
                    ret = wc_curve25519_import_public_ex(in, inSz,
                        (curve25519_key*)*key, EC25519_LITTLE_ENDIAN);
            }
            break;
#endif
        case DHKEM_X448_HKDF_SHA512:
        default:
            ret = -1;
            break;
    }

    if (ret == 0 && *key == NULL)
        ret = MEMORY_E;

    if (ret != 0 && *key != NULL) {
        wc_HpkeFreeKey(hpke, (word16)hpke->kem, *key, hpke->heap);
        *key = NULL;
    }

    return ret;
}

/* free a kem key */
void wc_HpkeFreeKey(Hpke* hpke, word16 kem, void* keypair, void* heap)
{
    switch (kem)
    {
#if defined(HAVE_ECC)
        case DHKEM_P256_HKDF_SHA256:
        case DHKEM_P384_HKDF_SHA384:
        case DHKEM_P521_HKDF_SHA512:
            wc_ecc_key_free((ecc_key*)keypair);
            break;
#endif
#if defined(HAVE_CURVE25519)
        case DHKEM_X25519_HKDF_SHA256:
            wc_curve25519_free((curve25519_key*)keypair);
            XFREE(keypair, heap, DYNAMIC_TYPE_CURVE25519);
            break;
#endif
        case DHKEM_X448_HKDF_SHA512:
            /* TODO: Add X448 */
        default:
            break;
    }
    (void)hpke;
    (void)heap;
}

static int wc_HpkeLabeledExtract(Hpke* hpke, byte* suite_id,
    word32 suite_id_len, byte* salt, word32 salt_len, byte* label,
    word32 label_len, byte* ikm, word32 ikm_len, byte* out)
{
    int ret;
    byte* labeled_ikm_p;
#ifndef WOLFSSL_SMALL_STACK
    byte labeled_ikm[MAX_HPKE_LABEL_SZ];
#else
    byte* labeled_ikm;
#endif

    if (hpke == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_SMALL_STACK
    labeled_ikm = (byte*)XMALLOC(MAX_HPKE_LABEL_SZ, hpke->heap,
        DYNAMIC_TYPE_TMP_BUFFER);
    if (labeled_ikm == NULL) {
        return MEMORY_E;
    }
#endif

    /* concat the labeled_ikm */
    /* version */
    XMEMCPY(labeled_ikm, HPKE_VERSION_STR, HPKE_VERSION_STR_LEN);
    labeled_ikm_p = labeled_ikm + HPKE_VERSION_STR_LEN;

    /* suite_id */
    XMEMCPY(labeled_ikm_p, suite_id, suite_id_len);
    labeled_ikm_p += suite_id_len;

    /* label */
    XMEMCPY(labeled_ikm_p, label, label_len);
    labeled_ikm_p += label_len;

    /* ikm */
    if (ikm_len != 0) {
        XMEMCPY(labeled_ikm_p, ikm, ikm_len);
        labeled_ikm_p += ikm_len;
    }

    /* call extract */
    PRIVATE_KEY_UNLOCK();
    ret = wc_HKDF_Extract(hpke->kdf_digest, salt, salt_len, labeled_ikm,
        (word32)(size_t)(labeled_ikm_p - labeled_ikm), out);
    PRIVATE_KEY_LOCK();

#ifdef WOLFSSL_SMALL_STACK
    XFREE(labeled_ikm, hpke->heap, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}

/* do hkdf expand with the format specified in the hpke rfc, return 0 or
 * error */
static int wc_HpkeLabeledExpand(Hpke* hpke, byte* suite_id, word32 suite_id_len,
    byte* prk, word32 prk_len, byte* label, word32 label_len, byte* info,
    word32 infoSz, word32 L, byte* out)
{
    int ret;
    byte* labeled_info_p;
#ifndef WOLFSSL_SMALL_STACK
    byte labeled_info[MAX_HPKE_LABEL_SZ];
#else
    byte* labeled_info;
#endif

    if (hpke == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_SMALL_STACK
    labeled_info = (byte*)XMALLOC(MAX_HPKE_LABEL_SZ, hpke->heap,
        DYNAMIC_TYPE_TMP_BUFFER);
    if (labeled_info == NULL) {
        return MEMORY_E;
    }
#endif

    /* copy length */
    ret = I2OSP((int)L, 2, labeled_info);
    labeled_info_p = labeled_info + 2;

    if (ret == 0) {
        /* version */
        XMEMCPY(labeled_info_p, HPKE_VERSION_STR, HPKE_VERSION_STR_LEN);
        labeled_info_p += HPKE_VERSION_STR_LEN;

        /* suite_id */
        XMEMCPY(labeled_info_p, suite_id, suite_id_len);
        labeled_info_p += suite_id_len;

        /* label */
        XMEMCPY(labeled_info_p, label, label_len);
        labeled_info_p += label_len;

        /* info */
        XMEMCPY(labeled_info_p, info, infoSz);
        labeled_info_p += infoSz;

        /* call expand */
        PRIVATE_KEY_UNLOCK();
        ret = wc_HKDF_Expand(hpke->kdf_digest,
            prk, prk_len,
            labeled_info, (word32)(size_t)(labeled_info_p - labeled_info),
            out, L);
        PRIVATE_KEY_LOCK();
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(labeled_info, hpke->heap, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}

/* compute the current nonce from the base nonce using the sequence value,
 * return 0 or error */
static int wc_HpkeContextComputeNonce(Hpke* hpke, HpkeBaseContext* context,
    byte* out)
{
    int ret;
    byte seq_bytes[HPKE_Nn_MAX];

    /* convert the sequence into a byte string with the same length as the
     * nonce */
    ret = I2OSP(context->seq, (int)hpke->Nn, seq_bytes);
    if (ret == 0) {
        xorbufout(out, context->base_nonce, seq_bytes, hpke->Nn);
    }

    return ret;
}

/* call extract and expand as specified in the hpke rfc, return 0 or error */
static int wc_HpkeExtractAndExpand( Hpke* hpke, byte* dh, word32 dh_len,
    byte* kemContext, word32 kem_context_length, byte* sharedSecret)
{
    int ret;
    /* max length is the largest hmac digest possible */
#ifndef WOLFSSL_SMALL_STACK
    byte eae_prk[WC_MAX_DIGEST_SIZE];
#else
    byte* eae_prk;
#endif

    if (hpke == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_SMALL_STACK
    eae_prk = (byte*)XMALLOC(WC_MAX_DIGEST_SIZE, hpke->heap,
        DYNAMIC_TYPE_DIGEST);
    if (eae_prk == NULL) {
        return MEMORY_E;
    }
#endif

    /* extract */
    ret = wc_HpkeLabeledExtract(hpke, hpke->kem_suite_id,
        sizeof( hpke->kem_suite_id ), NULL, 0, (byte*)EAE_PRK_LABEL_STR,
        EAE_PRK_LABEL_STR_LEN, dh, dh_len, eae_prk);

    /* expand */
    if ( ret == 0 )
        ret = wc_HpkeLabeledExpand(hpke, hpke->kem_suite_id,
            sizeof( hpke->kem_suite_id ), eae_prk, hpke->Nh,
            (byte*)SHARED_SECRET_LABEL_STR, SHARED_SECRET_LABEL_STR_LEN,
            kemContext, kem_context_length, hpke->Nsecret, sharedSecret);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(eae_prk, hpke->heap, DYNAMIC_TYPE_DIGEST);
#endif

    return ret;
}

/* derive the key, nonce and exporter secret and store them in the context
 * struct, return 0 or error */
static int wc_HpkeKeyScheduleBase(Hpke* hpke, HpkeBaseContext* context,
    byte* sharedSecret, byte* info, word32 infoSz)
{
    int ret;
#ifndef WOLFSSL_SMALL_STACK
    /* 1 for mode and WC_MAX_DIGEST_SIZE times 2 for psk_id_hash and */
    /* info_hash */
    byte key_schedule_context[1 + 2 * WC_MAX_DIGEST_SIZE];
    /* maximum size of secret is largest hash of extract */
    byte secret[WC_MAX_DIGEST_SIZE];
#else
    byte* key_schedule_context = NULL;
    byte* secret = NULL;
#endif

    if (hpke == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_SMALL_STACK
    key_schedule_context = (byte*)XMALLOC((1 + 2 * WC_MAX_DIGEST_SIZE),
        hpke->heap, DYNAMIC_TYPE_TMP_BUFFER);
    secret = (byte*)XMALLOC(WC_MAX_DIGEST_SIZE, hpke->heap,
        DYNAMIC_TYPE_DIGEST);
    if (key_schedule_context == NULL || secret == NULL) {
        XFREE(key_schedule_context, hpke->heap, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(secret, hpke->heap, DYNAMIC_TYPE_DIGEST);
        return MEMORY_E;
    }
#endif

    /* set the sequence to 0 */
    context->seq = 0;

    /* 0 for mode */
    key_schedule_context[0] = 0;

    /* extract psk_id, which for base is null */
    ret = wc_HpkeLabeledExtract(hpke, hpke->hpke_suite_id,
        sizeof( hpke->hpke_suite_id ), NULL, 0, (byte*)PSK_ID_HASH_LABEL_STR,
        PSK_ID_HASH_LABEL_STR_LEN, NULL, 0, key_schedule_context + 1);

    /* extract info */
    if (ret == 0) {
        ret = wc_HpkeLabeledExtract(hpke, hpke->hpke_suite_id,
            sizeof( hpke->hpke_suite_id ), NULL, 0, (byte*)INFO_HASH_LABEL_STR,
            INFO_HASH_LABEL_STR_LEN, info, infoSz,
            key_schedule_context + 1 + hpke->Nh);
    }

    /* extract secret */
    if (ret == 0) {
        ret = wc_HpkeLabeledExtract(hpke, hpke->hpke_suite_id,
            sizeof( hpke->hpke_suite_id ), sharedSecret, hpke->Nsecret,
            (byte*)SECRET_LABEL_STR, SECRET_LABEL_STR_LEN, NULL, 0, secret);
    }

    /* expand key */
    if (ret == 0)
        ret = wc_HpkeLabeledExpand(hpke, hpke->hpke_suite_id,
            sizeof( hpke->hpke_suite_id ), secret, hpke->Nh,
            (byte*)KEY_LABEL_STR, KEY_LABEL_STR_LEN, key_schedule_context,
            1 + 2 * hpke->Nh, hpke->Nk, context->key);

    /* expand nonce */
    if (ret == 0) {
        ret = wc_HpkeLabeledExpand(hpke, hpke->hpke_suite_id,
            sizeof( hpke->hpke_suite_id ), secret, hpke->Nh,
            (byte*)BASE_NONCE_LABEL_STR, BASE_NONCE_LABEL_STR_LEN,
            key_schedule_context, 1 + 2 * hpke->Nh, hpke->Nn,
            context->base_nonce);
    }

    /* expand exporter_secret */
    if (ret == 0) {
        ret = wc_HpkeLabeledExpand(hpke, hpke->hpke_suite_id,
            sizeof( hpke->hpke_suite_id ), secret, hpke->Nh,
            (byte*)EXP_LABEL_STR, EXP_LABEL_STR_LEN, key_schedule_context,
            1 + 2 * hpke->Nh, hpke->Nh, context->exporter_secret);
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(key_schedule_context, hpke->heap, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(secret, hpke->heap, DYNAMIC_TYPE_DIGEST);
#endif

    return ret;
}

/* compute the shared secret from the ephemeral and receiver kem keys */
static int wc_HpkeEncap(Hpke* hpke, void* ephemeralKey, void* receiverKey,
    byte* sharedSecret)
{
    int ret;
#ifdef ECC_TIMING_RESISTANT
    WC_RNG* rng;
#endif
    word32 dh_len;
    word16 receiverPubKeySz;
    word16 ephemeralPubKeySz;
#ifndef WOLFSSL_SMALL_STACK
    byte dh[HPKE_Ndh_MAX];
    byte kemContext[HPKE_Npk_MAX * 2];
#else
    byte* dh = NULL;
    byte* kemContext = NULL;
#endif

    if (hpke == NULL || ephemeralKey == NULL || receiverKey == NULL ||
        sharedSecret == NULL) {
        return BAD_FUNC_ARG;
    }

    receiverPubKeySz = (word16)hpke->Npk;
    ephemeralPubKeySz = (word16)hpke->Npk;

#ifdef WOLFSSL_SMALL_STACK
    dh = (byte*)XMALLOC(hpke->Ndh, hpke->heap, DYNAMIC_TYPE_TMP_BUFFER);
    kemContext = (byte*)XMALLOC(hpke->Npk * 2, hpke->heap,
        DYNAMIC_TYPE_TMP_BUFFER);
    if (dh == NULL || kemContext == NULL) {
        XFREE(dh, hpke->heap, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(kemContext, hpke->heap, DYNAMIC_TYPE_TMP_BUFFER);
        return MEMORY_E;
    }
#endif

    /* generate dh */
    dh_len = hpke->Ndh;

    switch (hpke->kem)
    {
#if defined(HAVE_ECC)
        case DHKEM_P256_HKDF_SHA256:
        case DHKEM_P384_HKDF_SHA384:
        case DHKEM_P521_HKDF_SHA512:
#ifdef ECC_TIMING_RESISTANT
            rng = wc_rng_new(NULL, 0, hpke->heap);

            if (rng == NULL)
                return RNG_FAILURE_E;

            wc_ecc_set_rng((ecc_key*)ephemeralKey, rng);
#endif

            ret = wc_ecc_shared_secret((ecc_key*)ephemeralKey,
                (ecc_key*)receiverKey, dh, &dh_len);

#ifdef ECC_TIMING_RESISTANT
            wc_rng_free(rng);
#endif
            break;
#endif
#if defined(HAVE_CURVE25519)
        case DHKEM_X25519_HKDF_SHA256:
            ret = wc_curve25519_shared_secret_ex((curve25519_key*)ephemeralKey,
                (curve25519_key*)receiverKey, dh, &dh_len,
                EC25519_LITTLE_ENDIAN);
            break;
#endif
        case DHKEM_X448_HKDF_SHA512:
            /* TODO: Add X448 */
        default:
            ret = -1;
            break;
    }

    if (ret == 0) {
        /* serialize ephemeralKey into kemContext */
        ret = wc_HpkeSerializePublicKey(hpke, ephemeralKey,
            kemContext, &ephemeralPubKeySz);
    }
    if (ret == 0) {
        /* serialize pkR into kemContext */
        ret = wc_HpkeSerializePublicKey(hpke, receiverKey,
            kemContext + ephemeralPubKeySz, &receiverPubKeySz);
    }
    if (ret == 0) {
        /* compute the shared secret */
        ret = wc_HpkeExtractAndExpand(hpke, dh, dh_len, kemContext,
            hpke->Npk * 2, sharedSecret);
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(dh, hpke->heap, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(kemContext, hpke->heap, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}

/* setup the sender context with shared key, nonce and exporter secret */
static int wc_HpkeSetupBaseSender(Hpke* hpke, HpkeBaseContext* context,
    void* ephemeralKey, void* receiverKey, byte* info, word32 infoSz)
{
    int ret;
#ifndef WOLFSSL_SMALL_STACK
    byte sharedSecret[HPKE_Nsecret_MAX];
#else
    byte* sharedSecret;
#endif

    if (hpke == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_SMALL_STACK
    sharedSecret = (byte*)XMALLOC(hpke->Nsecret, hpke->heap,
        DYNAMIC_TYPE_TMP_BUFFER);
#endif

    /* encap */
    ret = wc_HpkeEncap(hpke, ephemeralKey, receiverKey, sharedSecret);

    /* schedule */
    if (ret == 0) {
        ret = wc_HpkeKeyScheduleBase(hpke, context, sharedSecret, info,
            infoSz);
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(sharedSecret, hpke->heap, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}

/* encrypt a message using an hpke base context, return 0 or error */
static int wc_HpkeContextSealBase(Hpke* hpke, HpkeBaseContext* context,
    byte* aad, word32 aadSz, byte* plaintext, word32 ptSz, byte* out)
{
    int ret;
    byte nonce[HPKE_Nn_MAX];
#ifndef WOLFSSL_SMALL_STACK
    Aes aes_key[1];
#else
    Aes* aes_key;
#endif

    if (hpke == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_SMALL_STACK
    aes_key = (Aes*)XMALLOC(sizeof(Aes), hpke->heap, DYNAMIC_TYPE_AES);
    if (aes_key == NULL) {
        return MEMORY_E;
    }
#endif

    ret = wc_AesInit(aes_key, hpke->heap, INVALID_DEVID);
    if (ret == 0) {
        ret = wc_HpkeContextComputeNonce(hpke, context, nonce);
        if (ret == 0) {
            ret = wc_AesGcmSetKey(aes_key, context->key, hpke->Nk);
        }
        if (ret == 0) {
            ret = wc_AesGcmEncrypt(aes_key, out, plaintext, ptSz, nonce,
                hpke->Nn, out + ptSz, hpke->Nt, aad, aadSz);
        }
        if (ret == 0) {
            context->seq++;
        }
        wc_AesFree(aes_key);
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(aes_key, hpke->heap, DYNAMIC_TYPE_AES);
#endif

    return ret;
}

/* encrypt a message using the provided ephemeral and receiver kem keys */
int wc_HpkeSealBase(Hpke* hpke, void* ephemeralKey, void* receiverKey,
    byte* info, word32 infoSz, byte* aad, word32 aadSz, byte* plaintext,
    word32 ptSz, byte* ciphertext)
{
    int ret;
#ifdef WOLFSSL_SMALL_STACK
    HpkeBaseContext* context;
#else
    HpkeBaseContext context[1];
#endif

    /* check that all the buffers are non NULL or optional with 0 length */
    if (hpke == NULL || ephemeralKey == NULL || receiverKey == NULL ||
        (info == NULL && infoSz != 0) || (aad == NULL && aadSz != 0) ||
        plaintext == NULL || ciphertext == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_SMALL_STACK
    context = (HpkeBaseContext*)XMALLOC(sizeof(HpkeBaseContext), hpke->heap,
        DYNAMIC_TYPE_TMP_BUFFER);
    if (context == NULL) {
        return MEMORY_E;
    }
#endif

    PRIVATE_KEY_UNLOCK();

    /* setup the context and pubKey */
    ret = wc_HpkeSetupBaseSender(hpke, context, ephemeralKey, receiverKey, info,
        infoSz);

    /* run seal using the context */
    if (ret == 0) {
        ret = wc_HpkeContextSealBase(hpke, context, aad, aadSz, plaintext,
            ptSz, ciphertext);
    }

    PRIVATE_KEY_LOCK();

#ifdef WOLFSSL_SMALL_STACK
    XFREE(context, hpke->heap, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}

/* compute the shared secret from the ephemeral and receiver kem keys */
static int wc_HpkeDecap(Hpke* hpke, void* receiverKey, const byte* pubKey,
    word16 pubKeySz, byte* sharedSecret)
{
    int ret;
#ifdef ECC_TIMING_RESISTANT
    WC_RNG* rng;
#endif
    word32 dh_len;
    word16 receiverPubKeySz;
    void* ephemeralKey = NULL;
#ifndef WOLFSSL_SMALL_STACK
    byte dh[HPKE_Ndh_MAX];
    byte kemContext[HPKE_Npk_MAX * 2];
#else
    byte* dh = NULL;
    byte* kemContext = NULL;
#endif

    if (hpke == NULL || receiverKey == NULL) {
        return BAD_FUNC_ARG;
    }

    receiverPubKeySz = (word16)hpke->Npk;

#ifdef WOLFSSL_SMALL_STACK
    dh = (byte*)XMALLOC(hpke->Ndh, hpke->heap, DYNAMIC_TYPE_TMP_BUFFER);
    kemContext = (byte*)XMALLOC(hpke->Npk * 2, hpke->heap,
        DYNAMIC_TYPE_TMP_BUFFER);
    if (dh == NULL || kemContext == NULL) {
        XFREE(dh, hpke->heap, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(kemContext, hpke->heap, DYNAMIC_TYPE_TMP_BUFFER);
        return MEMORY_E;
    }
#endif

    /* deserialize ephemeralKey from pubKey */
    ret = wc_HpkeDeserializePublicKey(hpke, &ephemeralKey, pubKey, pubKeySz);

    /* generate dh */
    dh_len = hpke->Ndh;

    if (ret == 0)
        switch (hpke->kem)
        {
#if defined(HAVE_ECC)
            case DHKEM_P256_HKDF_SHA256:
            case DHKEM_P384_HKDF_SHA384:
            case DHKEM_P521_HKDF_SHA512:
#ifdef ECC_TIMING_RESISTANT
                rng = wc_rng_new(NULL, 0, hpke->heap);

                if (rng == NULL)
                    return RNG_FAILURE_E;

                wc_ecc_set_rng((ecc_key*)receiverKey, rng);
#endif

                ret = wc_ecc_shared_secret((ecc_key*)receiverKey,
                    (ecc_key*)ephemeralKey, dh, &dh_len);

#ifdef ECC_TIMING_RESISTANT
                wc_rng_free(rng);
#endif
                break;
#endif
#if defined(HAVE_CURVE25519)
            case DHKEM_X25519_HKDF_SHA256:
                ret = wc_curve25519_shared_secret_ex(
                    (curve25519_key*)receiverKey, (curve25519_key*)ephemeralKey,
                    dh, &dh_len, EC25519_LITTLE_ENDIAN);
                break;
#endif
            case DHKEM_X448_HKDF_SHA512:
                /* TODO: Add X448 */
            default:
                ret = -1;
                break;
        }

    if (ephemeralKey != NULL)
        wc_HpkeFreeKey(hpke, (word16)hpke->kem, ephemeralKey, hpke->heap);

    if (ret == 0) {
        /* copy pubKey into kemContext */
        XMEMCPY(kemContext, pubKey, hpke->Npk);

        /* serialize pkR into kemContext */
        ret = wc_HpkeSerializePublicKey(hpke, receiverKey,
            kemContext + hpke->Npk, &receiverPubKeySz);
    }

    /* compute the shared secret */
    if (ret == 0) {
        ret = wc_HpkeExtractAndExpand(hpke, dh, dh_len, kemContext,
            hpke->Npk * 2, sharedSecret);
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(dh, hpke->heap, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(kemContext, hpke->heap, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}

/* setup an hpke base context for decrypting messages, return 0 or error */
static int wc_HpkeSetupBaseReceiver(Hpke* hpke, HpkeBaseContext* context,
    void* receiverKey, const byte* pubKey, word16 pubKeySz, byte* info,
    word32 infoSz)
{
    int ret;
#ifndef WOLFSSL_SMALL_STACK
    byte sharedSecret[HPKE_Nsecret_MAX];
#else
    byte* sharedSecret;
#endif

#ifdef WOLFSSL_SMALL_STACK
    sharedSecret = (byte*)XMALLOC(hpke->Nsecret, hpke->heap,
        DYNAMIC_TYPE_TMP_BUFFER);
    if (sharedSecret == NULL) {
        return MEMORY_E;
    }
#endif

    /* decap */
    ret = wc_HpkeDecap(hpke, receiverKey, pubKey, pubKeySz, sharedSecret);

    /* schedule */
    if (ret == 0) {
        ret = wc_HpkeKeyScheduleBase(hpke, context, sharedSecret, info,
            infoSz);
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(sharedSecret, hpke->heap, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}

/* decrypt a message using a setup hpke context, return 0 or error */
static int wc_HpkeContextOpenBase(Hpke* hpke, HpkeBaseContext* context,
    byte* aad, word32 aadSz, byte* ciphertext, word32 ctSz, byte* out)
{
    int ret;
    byte nonce[HPKE_Nn_MAX];
#ifndef WOLFSSL_SMALL_STACK
    Aes aes_key[1];
#else
    Aes* aes_key;
#endif

    if (hpke == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_SMALL_STACK
    aes_key = (Aes*)XMALLOC(sizeof(Aes), hpke->heap, DYNAMIC_TYPE_AES);
    if (aes_key == NULL) {
        return MEMORY_E;
    }
#endif

    ret = wc_HpkeContextComputeNonce(hpke, context, nonce);
    if (ret == 0)
        ret = wc_AesInit(aes_key, hpke->heap, INVALID_DEVID);
    if (ret == 0) {
        ret = wc_AesGcmSetKey(aes_key, context->key, hpke->Nk);
        if (ret == 0) {
            ret = wc_AesGcmDecrypt(aes_key, out, ciphertext, ctSz, nonce,
                hpke->Nn, ciphertext + ctSz, hpke->Nt, aad, aadSz);
        }
        if (ret == 0) {
            context->seq++;
        }
        wc_AesFree(aes_key);
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(aes_key, hpke->heap, DYNAMIC_TYPE_AES);
#endif

    return ret;
}

/* decrypt a message using the receiver and encoded ephemeral key, return 0 or
 * error */
int wc_HpkeOpenBase(Hpke* hpke, void* receiverKey, const byte* pubKey,
    word16 pubKeySz, byte* info, word32 infoSz, byte* aad, word32 aadSz,
    byte* ciphertext, word32 ctSz, byte* plaintext)
{
    int ret;
#ifndef WOLFSSL_SMALL_STACK
    HpkeBaseContext context[1];
#else
    HpkeBaseContext* context;
#endif

    /* check that all the buffer are non NULL or optional with 0 length */
    if (hpke == NULL || receiverKey == NULL || pubKey == NULL ||
        pubKeySz == 0 || (info == NULL && infoSz != 0) ||
        (aad == NULL && aadSz != 0) || plaintext == NULL ||
        ciphertext == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_SMALL_STACK
    context = (HpkeBaseContext*)XMALLOC(sizeof(HpkeBaseContext), hpke->heap,
        DYNAMIC_TYPE_TMP_BUFFER);
    if (context == NULL) {
        return MEMORY_E;
    }
#endif

    PRIVATE_KEY_UNLOCK();

    /* setup receiver */
    ret = wc_HpkeSetupBaseReceiver(hpke, context, receiverKey, pubKey,
        pubKeySz, info, infoSz);

    if (ret == 0) {
        /* open the ciphertext */
        ret = wc_HpkeContextOpenBase(hpke, context, aad, aadSz, ciphertext,
            ctSz, plaintext);
    }

    PRIVATE_KEY_LOCK();

#ifdef WOLFSSL_SMALL_STACK
    XFREE(context, hpke->heap, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}

#endif /* HAVE_HPKE && (HAVE_ECC || HAVE_CURVE25519) && HAVE_AESGCM */
