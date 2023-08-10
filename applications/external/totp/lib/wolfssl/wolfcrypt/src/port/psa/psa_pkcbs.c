/* psa_pkcbs.c
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
#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#if defined(WOLFSSL_HAVE_PSA)
#if defined(HAVE_PK_CALLBACKS)
#if !defined(WOLFSSL_PSA_NO_PKCBS)

#include <psa/crypto.h>

#include <wolfssl/wolfcrypt/port/psa/psa.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/internal.h>
#include <wolfssl/wolfcrypt/asn.h>
#include <wolfssl/wolfcrypt/logging.h>

#ifdef HAVE_ECC

#include <wolfssl/wolfcrypt/ecc.h>

static int psa_ecc_get_curve_info_from_curve_id(int ecc_curve,
                                                psa_ecc_family_t *family,
                                                size_t *key_size)
{
    if (ecc_curve != ECC_SECP256R1)
        return NOT_COMPILED_IN;

    *family = PSA_ECC_FAMILY_SECP_R1;
    if (key_size != NULL)
        *key_size = 32;

    return 0;
}

/**
 * psa_ecc_export_to_wc_key() - export psa @key_id into ecc_key @key
 * @key: already initialized ecc_key to export the key into [out]
 * @key_id: key to export [in]
 * @key_size: size of the key [in]
 * @curve_id: wc id of the curve [in]
 *
 * returns 0 on success
 */
static int psa_ecc_export_to_wc_key(struct ecc_key *key,
                                    psa_key_id_t key_id,
                                    int curve_id)
{
    uint8_t raw[(MAX_ECC_BYTES * 2) + 1];
    psa_status_t status;
    size_t length;
    int ret;

    PSA_LOCK();
    status = psa_export_public_key(key_id,
                                   raw, sizeof(raw), &length);
    PSA_UNLOCK();

    if (status != PSA_SUCCESS) {
        WOLFSSL_MSG("PSA: export key error");
        return WC_HW_E;
    }

    ret = wc_ecc_import_x963_ex(raw, (word32)length, key, curve_id);
    if (ret != 0) {
        WOLFSSL_MSG("PSA: import key error");
        return WC_HW_E;
    }

    return 0;
}

static int psa_ecc_keygen(int ecc_curve, unsigned key_size,
                          psa_key_id_t *key_id)
{
    psa_key_attributes_t key_attr = { 0 };
    size_t ecc_curve_key_size;
    psa_ecc_family_t family;
    psa_status_t status;
    int ret;

    ret = psa_ecc_get_curve_info_from_curve_id(ecc_curve, &family,
                                               &ecc_curve_key_size);
    if (ret != 0)
        return ret;

    if (key_size != 0 && key_size != ecc_curve_key_size)
        return BAD_FUNC_ARG;

    psa_set_key_type(&key_attr, PSA_KEY_TYPE_ECC_KEY_PAIR(family));
    psa_set_key_bits(&key_attr, ecc_curve_key_size * 8);
    psa_set_key_algorithm(&key_attr, PSA_ALG_ECDH);
    psa_set_key_usage_flags(&key_attr, PSA_KEY_USAGE_DERIVE);

    PSA_LOCK();
    status = psa_generate_key(&key_attr, key_id);
    PSA_UNLOCK();
    if (status != PSA_SUCCESS)
        return WC_HW_E;

    return 0;
}

static int psa_ecc_keygen_cb(WOLFSSL* ssl, struct ecc_key* key,
                             unsigned int key_size, int ecc_curve, void* ctx)
{
    struct psa_ssl_ctx *psa_ctx = (struct psa_ssl_ctx *)ctx;
    psa_key_id_t psa_key_id;
    int ret;

    (void)ssl;

    WOLFSSL_ENTER("psa_ecc_keygen_cb");

    if (psa_ctx == NULL)
        return BAD_FUNC_ARG;

    ret = psa_ecc_keygen(ecc_curve, key_size, &psa_key_id);
    if (ret != 0)
        return WC_HW_E;

    ret = psa_ecc_export_to_wc_key(key, psa_key_id, ecc_curve);
    if (ret != 0) {
        PSA_LOCK();
        psa_destroy_key(psa_key_id);
        PSA_UNLOCK();
        return WC_HW_E;
    }

    psa_ctx->dh_key = psa_key_id;

    return 0;
}

static int psa_ecc_shared_secret_tls12_client(struct ecc_key* other_key,
                                              unsigned char* pubKeyDer,
                                              word32* pubKeySz,
                                              unsigned char* out,
                                              word32* output_size)
{
    uint8_t raw[(MAX_ECC_BYTES * 2) + 1];
    psa_status_t status;
    word32 raw_length;
    psa_key_id_t key;
    size_t length;
    int ret;

    ret = psa_ecc_keygen(other_key->dp->id, 0, &key);
    if (ret != 0)
        return ret;

    PSA_LOCK();
    status = psa_export_public_key(key, pubKeyDer, *pubKeySz, &length);
    PSA_UNLOCK();
    if (status != PSA_SUCCESS) {
        ret = WC_HW_E;
        goto exit;
    }

    *pubKeySz = (word32)length;

    raw_length = (word32)sizeof(raw);
    ret = wc_ecc_export_x963_ex(other_key, raw, &raw_length, 0);
    if (ret != 0)
        goto exit;

    PSA_LOCK();
    status = psa_raw_key_agreement(PSA_ALG_ECDH, key,
                                   raw, raw_length,
                                   out, *output_size,
                                   &length);
    PSA_UNLOCK();
    if (status != PSA_SUCCESS) {
        ret = WC_HW_E;
        goto exit;
    }

    *output_size = (word32)length;
    ret = 0;

 exit:
    PSA_LOCK();
    psa_destroy_key(key);
    PSA_UNLOCK();

    return ret;
}

static int psa_ecc_shared_secret_cb(WOLFSSL* ssl, struct ecc_key* other_key,
                                    unsigned char* pubKeyDer, word32* pubKeySz,
                                    unsigned char* out, word32* output_size,
                                    int side, void* ctx)
{
    struct psa_ssl_ctx *psa_ctx = (struct psa_ssl_ctx *)ctx;
    size_t output_length;
    psa_status_t status;

    if (psa_ctx == NULL)
        return BAD_FUNC_ARG;

    WOLFSSL_ENTER("psa_ecc_shared_secret_cb");

    /* client before tls13 must create the key and export it in pubkeyDer. The
       server key is stored in other_key */
    if (side == WOLFSSL_CLIENT_END &&
        wolfSSL_GetVersion(ssl) < WOLFSSL_TLSV1_3) {
        return psa_ecc_shared_secret_tls12_client(other_key,
                                                  pubKeyDer, pubKeySz,
                                                  out, output_size);
    }

    PSA_LOCK();
    status = psa_raw_key_agreement(PSA_ALG_ECDH,
                                   psa_ctx->dh_key,
                                   pubKeyDer,
                                   *pubKeySz,
                                   out, *output_size,
                                   &output_length);
    PSA_UNLOCK();

    if (status != PSA_SUCCESS) {
        WOLFSSL_MSG("PSA: error raw_key_agreement");
        return WC_HW_E;
    }

    *output_size = (word32)output_length;

    PSA_LOCK();
    psa_destroy_key(psa_ctx->dh_key);
    PSA_UNLOCK();

    psa_ctx->dh_key = PSA_KEY_ID_NULL;

    return 0;
}

/* Map hash length to equivalent psa_algorithm_t type.
 *
 * hash_len - length of hash
 *
 * Return psa_algorithm_t representing hash algorithm for hash length, or
 * PSA_ALG_NONE if no match.
 */
static int psa_map_hash_alg(int hash_len)
{
    switch (hash_len) {
        case 20:
            return PSA_ALG_SHA_1;
        case 28:
            return PSA_ALG_SHA_224;
        case 32:
            return PSA_ALG_SHA_256;
        case 48:
            return PSA_ALG_SHA_384;
        case 64:
            return PSA_ALG_SHA_512;
        default:
            return PSA_ALG_NONE;
    }
}

static int psa_ecc_sign_cb(WOLFSSL* ssl, const unsigned char* input,
                           unsigned int input_length,
                           unsigned char* signature, word32* signature_size,
                           const unsigned char* keyDer, unsigned int keySz,
                           void* ctx)
{
    struct psa_ssl_ctx *psa_ctx = (struct psa_ssl_ctx*)ctx;
    uint8_t rs[MAX_ECC_BYTES * 2];
    psa_status_t status;
    size_t rs_length;
    word32 point_len;
    psa_algorithm_t hash_algo;
    int ret;

    (void)ssl;
    (void)keyDer;
    (void)keySz;

    WOLFSSL_ENTER("psa_ecc_sign_cb");

    if (psa_ctx == NULL)
        return BAD_FUNC_ARG;

    /* Get correct hash algorithm that matches input hash length */
    hash_algo = psa_map_hash_alg(input_length);

    status = psa_sign_hash(psa_ctx->private_key,
                           PSA_ALG_ECDSA(hash_algo), input,
                           input_length, rs, sizeof(rs),
                           &rs_length);
    if (status != PSA_SUCCESS)
        return WC_HW_E;

    point_len = (word32)(rs_length / 2);
    ret = wc_ecc_rs_raw_to_sig(rs, point_len, rs + point_len, point_len,
                               signature, signature_size);
    if (ret != 0)
        return -1;

    return 0;
}

static int psa_ecc_decode_public_key(const uint8_t *key, word32 key_length,
                                     psa_key_id_t *key_id,
                                     psa_algorithm_t hash_algo)
{
    uint8_t raw_key[(MAX_ECC_BYTES * 2) + 1];
    psa_key_attributes_t attr = { 0 };
    word32 raw_key_length, idx;
    psa_ecc_family_t ecc_curve;
    size_t ecc_curve_size;
    psa_status_t status;
    ecc_key tmp_key;
    int ret;

    ret = wc_ecc_init(&tmp_key);
    if (ret != 0 )
        return ret;

    idx = 0;
    ret = wc_EccPublicKeyDecode(key, &idx, &tmp_key, key_length);
    if (ret != 0 )
        goto exit;

    raw_key_length = (word32)sizeof(raw_key);
    ret = wc_ecc_export_x963_ex(&tmp_key, raw_key, &raw_key_length, 0);
    if (ret != 0 )
        goto exit;

    ret = psa_ecc_get_curve_info_from_curve_id(tmp_key.dp->id,
                                               &ecc_curve, &ecc_curve_size);
    if (ret != 0 )
        goto exit;

    psa_set_key_type(&attr, PSA_KEY_TYPE_ECC_PUBLIC_KEY(ecc_curve));
    psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_bits(&attr, ecc_curve_size * 8);
    psa_set_key_algorithm(&attr, PSA_ALG_ECDSA(hash_algo));

    PSA_LOCK();
    status = psa_import_key(&attr, raw_key, raw_key_length, key_id);
    PSA_UNLOCK();
    if (status != PSA_SUCCESS) {
        WOLFSSL_MSG("psa_ecc_decode_public_key: can't import the key");
        ret = WC_HW_E;
        goto exit;
    }

    ret = 0;

exit:
    wc_ecc_free(&tmp_key);
    return ret;
}

static int psa_ecc_verify_cb(WOLFSSL* ssl, const byte* sig, word32 sig_length,
                             const byte* hash, word32 hash_length,
                             const byte* key, word32 key_length,
                             int* result, void* ctx)
{
    uint8_t raw_signature[MAX_ECC_BYTES * 2], *s;
    psa_key_id_t tmp_key;
    word32 r_len, s_len;
    psa_status_t status;
    psa_algorithm_t hash_algo;
    int ret;

    (void)ssl;
    (void)ctx;
    WOLFSSL_ENTER("psa_ecc_verify_cb");

    /* Get correct hash algorithm that matches input hash length */
    hash_algo = psa_map_hash_alg(hash_length);

    ret = psa_ecc_decode_public_key(key, key_length, &tmp_key, hash_algo);
    if (ret != 0)
        return ret;

    r_len = s_len = MAX_ECC_BYTES;
    s = raw_signature + r_len;
    ret = wc_ecc_sig_to_rs(sig, sig_length, raw_signature, &r_len,
                           s, &s_len);
    if (ret != 0)
        goto exit;

    /* coalescence of r and s in the buffer */
    XMEMCPY(raw_signature + r_len, s, s_len);

    PSA_LOCK();
    status = psa_verify_hash(tmp_key, PSA_ALG_ECDSA(hash_algo), hash,
                             hash_length, raw_signature, r_len + s_len);
    PSA_UNLOCK();

    if (status == PSA_SUCCESS) {
        *result = 1;
    } else {
        *result = 0;

        if (status != PSA_ERROR_INVALID_SIGNATURE) {
            WOLFSSL_MSG("psa_ecc_verify_cb: can't verify hash");
            ret = WC_HW_E;
        }

        /* returns 0 for invalid signature as well */
    }

 exit:
    PSA_LOCK();
    psa_destroy_key(tmp_key);
    PSA_UNLOCK();

    return ret;
}
#endif /* HAVE_ECC */

#ifdef HAVE_HKDF
static int psa_hkdf_extract_cb(byte* prk, const byte* salt,
                               word32 salt_length, byte* ikm,
                               word32 ikm_length, int digest,
                               void* ctx)
{
    psa_key_attributes_t attr = { 0 };
    psa_algorithm_t hash;
    psa_key_id_t tmp_key;
    psa_status_t status;
    size_t digest_len;

    (void)ctx;

    WOLFSSL_ENTER("psa hkdf cb");

    if (salt_length != 0 && salt == NULL)
        return BAD_FUNC_ARG;

    switch (digest) {
#ifndef NO_SHA256
        case WC_SHA256:
            digest_len = WC_SHA256_DIGEST_SIZE;
            hash = PSA_ALG_SHA_256;
            break;
#endif
#ifdef WOLFSSL_SHA384
        case WC_SHA384:
            digest_len = WC_SHA384_DIGEST_SIZE;
            hash = PSA_ALG_SHA_384;
            break;
#endif
        default:
            return BAD_FUNC_ARG;
    }

    psa_set_key_type(&attr, PSA_KEY_TYPE_HMAC);
    psa_set_key_algorithm(&attr, PSA_ALG_HMAC(hash));
    psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_SIGN_MESSAGE);

    /* salt not provided, use zeroed digest len string. Use temporarily the prk
       buffer as we are sure is big enough */
    if (salt == NULL) {
        XMEMSET(prk, 0, digest_len);
        salt = prk;
        salt_length = (word32)digest_len;
    }

    /* When length is 0 then use zeroed data of digest length. */
    if (ikm_length == 0) {
        ikm_length = (word32)digest_len;
        XMEMSET(ikm, 0, digest_len);
    }

    PSA_LOCK();
    status = psa_import_key(&attr, salt, salt_length, &tmp_key);
    PSA_UNLOCK();
    if (status != PSA_SUCCESS) {
        WOLFSSL_MSG("psa_hkdf_cb: failed to import tmp key");
        return WC_HW_E;
    }

    PSA_LOCK();
    status = psa_mac_compute(tmp_key, PSA_ALG_HMAC(hash),
                             ikm, ikm_length, prk, digest_len,
                             &digest_len);
    PSA_UNLOCK();
    if (status != PSA_SUCCESS) {
        WOLFSSL_MSG("psa_hkdf_cb: failed to compute hashed mac");
        PSA_LOCK();
        psa_destroy_key(tmp_key);
        PSA_UNLOCK();
        return WC_HW_E;
    }

    PSA_LOCK();
    psa_destroy_key(tmp_key);
    PSA_UNLOCK();

    return 0;
}
#endif /* HAVE_HKDF */

int wolfSSL_psa_set_private_key_id(struct psa_ssl_ctx *ctx, psa_key_id_t id)
{
    if (ctx == NULL)
        return BAD_FUNC_ARG;
    if (id == PSA_KEY_ID_NULL)
        return BAD_FUNC_ARG;

    ctx->private_key = id;

    return WOLFSSL_SUCCESS;
}

void wolfSSL_free_psa_ctx(struct psa_ssl_ctx *ctx)
{
    if (ctx->dh_key != PSA_KEY_ID_NULL) {
        PSA_LOCK();
        psa_destroy_key(ctx->dh_key);
        PSA_UNLOCK();
        ctx->dh_key = PSA_KEY_ID_NULL;
    }
}

int wolfSSL_set_psa_ctx(WOLFSSL *ssl, struct psa_ssl_ctx *ctx)
{
    if (ctx == NULL || ssl == NULL)
        return BAD_FUNC_ARG;

#ifdef HAVE_ECC
    wolfSSL_SetEccSignCtx(ssl, ctx);
    wolfSSL_SetEccKeyGenCtx(ssl, ctx);
    wolfSSL_SetEccSharedSecretCtx(ssl, ctx);
    wolfSSL_SetEccVerifyCtx(ssl, ctx);
#endif
#ifdef HAVE_HKDF
    wolfSSL_SetHKDFExtractCtx(ssl, ctx);
#endif

    return WOLFSSL_SUCCESS;
}

int wolfSSL_CTX_psa_enable(WOLFSSL_CTX *ctx)
{
    if (ctx == NULL)
        return BAD_FUNC_ARG;

#ifdef HAVE_ECC
    wolfSSL_CTX_SetEccSignCb(ctx, psa_ecc_sign_cb);
    wolfSSL_CTX_SetEccKeyGenCb(ctx, psa_ecc_keygen_cb);
    wolfSSL_CTX_SetEccSharedSecretCb(ctx, psa_ecc_shared_secret_cb);
    wolfSSL_CTX_SetEccVerifyCb(ctx, psa_ecc_verify_cb);
#endif
#ifdef HAVE_HKDF
    wolfSSL_CTX_SetHKDFExtractCb(ctx, psa_hkdf_extract_cb);
#endif
    return WOLFSSL_SUCCESS;
}

#endif /* !WOLFSSL_PSA_NO_PKCBS */
#endif /* HAVE_PK_CALLBACKS */
#endif /* WOLFSSL_HAVE_PSA */
