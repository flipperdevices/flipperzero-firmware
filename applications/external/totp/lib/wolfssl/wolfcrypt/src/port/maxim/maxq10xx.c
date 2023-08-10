/* maxq10xx.c
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

#include <wolfssl/wolfcrypt/types.h>

#if defined(WOLFSSL_MAXQ1065) ||  defined(WOLFSSL_MAXQ108X)

#include <wolfssl/wolfcrypt/port/maxim/maxq10xx.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#include <stdint.h>
#include <wolfssl/internal.h>
#include <wolfssl/wolfcrypt/cryptocb.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/port/maxim/MXQ_API.h>

#ifndef WOLFSSL_HAVE_ECC_KEY_GET_PRIV
    /* FIPS build has replaced ecc.h. */
    #define wc_ecc_key_get_priv(key) (&((key)->k))
    #define WOLFSSL_HAVE_ECC_KEY_GET_PRIV
#endif

#ifdef MAXQ_DEBUG
void dbg_dumphex(const char *identifier, const uint8_t* pdata, uint32_t plen);
#else
#define dbg_dumphex(identifier, pdata, plen) WC_DO_NOTHING
#endif /* MAXQ_DEBUG */

#if defined(USE_WINDOWS_API)
# define maxq_CryptHwMutexTryLock()	(0)
#endif

#define AES_KEY_ID_START      (0x2000)
#define AES_KEY_ID_MAX_NUM    (32)
#define ECC_KEY_ID_START      (AES_KEY_ID_START + AES_KEY_ID_MAX_NUM)
#define ECC_KEY_ID_MAX_NUM    (32)

#define TEMP_KEY_ID_START     (0)
#if defined(WOLFSSL_MAXQ108X)
#define TEMP_KEY_ID_MAX_NUM   (16)
#else
#define TEMP_KEY_ID_MAX_NUM   (2)
#endif

#define PUBKEY_IMPORT_OBJID    0x1000
#define ROOT_CA_CERT_OBJ_ID    0x1003
#define DEVICE_CERT_OBJ_ID     0x1002
#define DEVICE_KEY_PAIR_OBJ_ID 0x1004
#define PSK_OBJ_ID             0x1236
#define K_CHUNKSIZE            2032
#define K_CIPHER_BLOCKSIZE     16
#define ECC256_KEYSIZE         32
#define MAX_HASH_CHUNKSIZE     2000
#define MAX_CERT_DATASIZE      2048
#define MAX_KEY_DATASIZE       256
#define MAX_SIGNKEY_DATASIZE   96
#define MAX_SIG_DATASIZE       64
#define ECC_KEYCOMPLEN         32

#ifdef WOLFSSL_MAXQ108X

#define PSK_KID (0x1235)
static unsigned char tls13active          =  0;
static unsigned char tls13early           =  0;

static int tls13_dh_obj_id                = -1;
static int tls13_ecc_obj_id               = -1;
static int tls13_hs_early_secret_obj_id   = -1;
static int tls13_binder_key_obj_id        = -1;
static int tls13_shared_secret_obj_id     = -1;
static int tls13_early_secret_obj_id      = -1;
static int tls13_derived_secret_obj_id    = -1;
static int tls13_handshake_secret_obj_id  = -1;
static int tls13_master_secret_obj_id     = -1;
static int tls13_client_secret_obj_id     = -1;
static int tls13_server_secret_obj_id     = -1;
static int tls13_client_hs_key_obj_id     = -1;
static int tls13_server_hs_key_obj_id     = -1;
static int tls13_client_app_key_obj_id    = -1;
static int tls13_server_app_key_obj_id    = -1;
static int tls13_server_finish_obj_id     = -1;
static int tls13_client_finish_obj_id     = -1;
static int *tls13_server_key_id           = NULL;
static int *tls13_client_key_id           = NULL;
static int tls13_res_master_obj_id        = -1;
static int tls13_server_cert_id           = -1;
static int tls13_server_key_algo          = -1;
static int tls13_server_key_len           = -1;

#endif /* WOLFSSL_MAXQ108X */

/* Please define MAXQ10XX_PRODUCTION_KEY in your build scripts once you have a
 * production key. */
#if defined(MAXQ10XX_PRODUCTION_KEY) || !defined(DEBUG_WOLFSSL)
#include "maxq10xx_key.h"
#else
/* TEST KEY. This must be changed for production environments!! */
static const mxq_u1 KeyPairImport[] = {
    0xd0,0x97,0x31,0xc7,0x63,0xc0,0x9e,0xe3,0x9a,0xb4,0xd0,0xce,0xa7,0x89,0xab,
    0x52,0xc8,0x80,0x3a,0x91,0x77,0x29,0xc3,0xa0,0x79,0x2e,0xe6,0x61,0x8b,0x2d,
    0x53,0x70,0xcc,0xa4,0x62,0xd5,0x4a,0x47,0x74,0xea,0x22,0xfa,0xa9,0xd4,0x95,
    0x4e,0xca,0x32,0x70,0x88,0xd6,0xeb,0x58,0x24,0xa3,0xc5,0xbf,0x29,0xdc,0xfd,
    0xe5,0xde,0x8f,0x48,0x19,0xe8,0xc6,0x4f,0xf2,0x46,0x10,0xe2,0x58,0xb9,0xb6,
    0x72,0x5e,0x88,0xaf,0xc2,0xee,0x8b,0x6f,0xe5,0x36,0xe3,0x60,0x7c,0xf8,0x2c,
    0xea,0x3a,0x4f,0xe3,0x6d,0x73
};
#endif /* MAXQ10XX_PRODUCTION_KEY || !DEBUG_WOLFSSL */

#if defined(WOLFSSL_MAXQ108X) && defined(HAVE_HKDF)

/* These are from Section 7.1 of the TLS 1.3 specification (RFC 8446). */
static const char derivedLabel[]     = "derived";
static const char cHsTrafficLabel[]  = "c hs traffic";
static const char sHsTrafficLabel[]  = "s hs traffic";
static const char cAppTrafficLabel[] = "c ap traffic";
static const char sAppTrafficLabel[] = "s ap traffic";
static const char appTrafUpdLabel[]  = "traffic upd";
static const char keyLabel[]         = "key";
static const char ivLabel[]          = "iv";
static const char finishedLabel[]    = "finished";
static const char resMasterLabel[]   = "res master";
static const char extBinderLabel[]   = "ext binder";

static int local_is_psk = 0;
static int is_hs_key = 0;

#endif /* WOLFSSL_MAXQ108X && HAVE_HKDF */

static int aes_key_id_arr[AES_KEY_ID_MAX_NUM];
static int ecc_key_id_arr[ECC_KEY_ID_MAX_NUM];

static int init_pk_callbacks = 0;
static int device_key_len = 32;

#if defined(WOLFSSL_MAXQ108X)
static int temp_key_id_arr[TEMP_KEY_ID_MAX_NUM] = {0};
static int *mac_key_obj_id = NULL;
static int mac_comp_active = 0;
#endif /* WOLFSSL_MAXQ108X */

#ifdef WOLFSSL_MAXQ10XX_CRYPTO
/*
 * Helper Functions
 */
static int crypto_sha256(const uint8_t *buf, uint32_t len, uint8_t *hash,
    uint32_t hashSz, uint32_t blkSz)
{
    int ret;
    uint32_t i = 0, chunk;
    wc_Sha256 sha256;

    /* validate arguments */
    if ((buf == NULL && len > 0) || hash == NULL ||
        hashSz < WC_SHA256_DIGEST_SIZE || blkSz == 0) {
        return BAD_FUNC_ARG;
    }

    /* Init Sha256 structure */
    ret = wc_InitSha256(&sha256);
    if (ret != 0) {
        return ret;
    }

    sha256.maxq_ctx.soft_hash = 1;

    while (i < len) {
        chunk = blkSz;
        if ((chunk + i) > len) {
            chunk = len - i;
        }
        /* Perform chunked update */
        ret = wc_Sha256Update(&sha256, (buf + i), chunk);
        if (ret != 0) {
            break;
        }
        i += chunk;
    }

    if (ret == 0) {
        /* Get final digest result */
        ret = wc_Sha256Final(&sha256, hash);
    }
    return ret;
}

static int crypto_ecc_sign(const uint8_t *key, uint32_t keySz,
    const uint8_t *hash, uint32_t hashSz, uint8_t *sig, uint32_t* sigSz,
    uint32_t curveSz, int curveId, WC_RNG* rng)
{
    int ret;
    mp_int r, s;
    ecc_key ecc;

    /* validate arguments */
    if (key == NULL || hash == NULL || sig == NULL || sigSz == NULL ||
        curveSz == 0 || hashSz == 0 || keySz < curveSz ||
        *sigSz < (curveSz * 2)) {
        return BAD_FUNC_ARG;
    }

    /* Initialize signature result */
    XMEMSET(sig, 0, curveSz * 2);

    /* Setup the ECC key */
    ret = wc_ecc_init(&ecc);
    if (ret < 0) {
        return ret;
    }

    ecc.maxq_ctx.hw_ecc = -1;

    /* Setup the signature r/s variables */
    ret = mp_init(&r);
    if (ret != MP_OKAY) {
        wc_ecc_free(&ecc);
        return ret;
    }

    ret = mp_init(&s);
    if (ret != MP_OKAY) {
        mp_clear(&r);
        wc_ecc_free(&ecc);
        return ret;
    }

    /* Import private key "k" */
    ret = wc_ecc_import_private_key_ex(key, keySz, /* private key "d" */
                                       NULL, 0,    /* public (optional) */
                                       &ecc, curveId);

    if (ret == 0) {
        ret = wc_ecc_sign_hash_ex(hash, hashSz, /* computed hash digest */
                                  rng, &ecc,    /* random and key context */
                                  &r, &s);
    }

    if (ret == 0) {
        /* export r/s */
        mp_to_unsigned_bin_len(&r, sig, curveSz);
        mp_to_unsigned_bin_len(&s, sig + curveSz, curveSz);
    }

    mp_clear(&r);
    mp_clear(&s);
    wc_ecc_free(&ecc);
    return ret;
}

#ifdef MAXQ_DEBUG
void dbg_dumphex(const char *identifier, const uint8_t* pdata, uint32_t plen)
{
    uint32_t i;

    printf("%s\n", identifier);

    for (i = 0; i < plen; ++i) {
        if ((i > 0) && !(i % 16)) {
            printf("\n");
        }
        printf("%02X ", pdata[i]);
    }

    printf("\n");
}
#endif /* MAXQ_DEBUG */

/*
 * Personalized Utility Functions
 */
static void LoadDefaultImportKey(unsigned char* key, int* keylen,
                                 int* curve, int* type)
{
    *curve  = MXQ_KEYPARAM_EC_P256R1;
    *type   = MXQ_KEYTYPE_ECC;
    *keylen = 32;

    XMEMCPY(key, KeyPairImport, sizeof(KeyPairImport));
}

static int getSignAlgoFromCurve(int c)
{
    switch(c) {
        case MXQ_KEYPARAM_EC_P256R1:  return ALGO_ECDSA_SHA_256;
        case MXQ_KEYPARAM_EC_P384R1:  return ALGO_ECDSA_SHA_384;
        case MXQ_KEYPARAM_EC_P521R1:  return ALGO_ECDSA_SHA_512;
#ifdef HAVE_ECC_BRAINPOOL
        case MXQ_KEYPARAM_EC_BP256R1: return ALGO_ECDSA_SHA_256;
        case MXQ_KEYPARAM_EC_BP384R1: return ALGO_ECDSA_SHA_384;
        case MXQ_KEYPARAM_EC_BP512R1: return ALGO_ECDSA_SHA_512;
#endif
    }
    return BAD_FUNC_ARG;
}

#ifdef WOLFSSL_MAXQ108X

static int wc_MAXQ10XX_HmacSetKey(int type);
static int wc_MAXQ10XX_HmacUpdate(const byte* msg, word32 length);
static int wc_MAXQ10XX_HmacFinal(byte* hash);

static int getMaxqKeyParamFromCurve(int c)
{
    switch(c) {
    case ECC_SECP256R1:       return MXQ_KEYPARAM_EC_P256R1;
    case ECC_SECP384R1:       return MXQ_KEYPARAM_EC_P384R1;
    case ECC_SECP521R1:       return MXQ_KEYPARAM_EC_P521R1;
    case ECC_BRAINPOOLP256R1: return MXQ_KEYPARAM_EC_BP256R1;
    case ECC_BRAINPOOLP384R1: return MXQ_KEYPARAM_EC_BP384R1;
    case ECC_BRAINPOOLP512R1: return MXQ_KEYPARAM_EC_BP512R1;
    }
    return BAD_FUNC_ARG;
}

#endif /* WOLFSSL_MAXQ108X */

static int ECDSA_sign(mxq_u1* dest, int* signlen, mxq_u1* key,
                      mxq_u1* data, mxq_length data_length, int curve)
{
    int ret;
    int hashlen = WC_SHA256_DIGEST_SIZE;
    unsigned char hash[WC_SHA256_DIGEST_SIZE];
    WC_RNG rng;
    int algo = 0;
    int wc_curve_id = ECC_SECP256R1;
    int wc_curve_size = 32;
    uint32_t sigSz = 0;

    if (curve != MXQ_KEYPARAM_EC_P256R1) {
        return BAD_FUNC_ARG;
    }

    algo = getSignAlgoFromCurve(curve);
    if (algo != ALGO_ECDSA_SHA_256) {
        return BAD_FUNC_ARG;
    }

    sigSz = (2 * wc_curve_size);
    if (*signlen < (int)sigSz) {
        return BAD_FUNC_ARG;
    }

    ret = wc_InitRng(&rng);
    if (ret != 0) {
        return ret;
    }

    ret = crypto_sha256(data, data_length, /* input message */
                        hash, hashlen,     /* hash digest result */
                        32                 /* configurable block/chunk size */
                       );

    if (ret == 0) {
        ret = crypto_ecc_sign(
            (key + (2 * wc_curve_size)), wc_curve_size,    /* private key */
            hash, hashlen,               /* computed hash digest */
            dest, &sigSz,                /* signature r/s */
            wc_curve_size,               /* curve size in bytes */
            wc_curve_id,                 /* curve id */
            &rng);

        *signlen = sigSz;
    }

    wc_FreeRng(&rng);
    return ret;
}

#ifdef MAXQ_AESGCM
static int alloc_aes_key_id(void)
{
    int i;
    for (i = 0; i < AES_KEY_ID_MAX_NUM; ++i) {
        if (aes_key_id_arr[i] == 0) {
            aes_key_id_arr[i] = AES_KEY_ID_START + i;
            break;
        }
    }

    if (i == AES_KEY_ID_MAX_NUM) {
        return 0;
    }
    else {
        return aes_key_id_arr[i];
    }
}
#endif /* MAXQ_AESGCM */

static void free_aes_key_id(int obj_id)
{
    int idx_for_arr = obj_id - AES_KEY_ID_START;

    if ((idx_for_arr >= 0) && (idx_for_arr < AES_KEY_ID_MAX_NUM)) {
        aes_key_id_arr[idx_for_arr] = 0;
    }
}

#ifdef MAXQ_ECC
static int alloc_ecc_key_id(void)
{
    int i;
    for (i = 0; i < ECC_KEY_ID_MAX_NUM; ++i) {
        if (ecc_key_id_arr[i] == 0) {
            ecc_key_id_arr[i] = ECC_KEY_ID_START + i;
            break;
        }
    }

    if (i == ECC_KEY_ID_MAX_NUM) {
        return 0;
    }
    else {
        return ecc_key_id_arr[i];
    }
}
#endif /* MAXQ_ECC */

static void free_ecc_key_id(int obj_id)
{
    int idx_for_arr = obj_id - ECC_KEY_ID_START;

    if ((idx_for_arr >= 0) && (idx_for_arr < ECC_KEY_ID_MAX_NUM)) {
        ecc_key_id_arr[idx_for_arr] = 0;
    }
}

#ifdef WOLFSSL_MAXQ108X
static int alloc_temp_key_id(void)
{
    int i;
    for (i = 0; i < TEMP_KEY_ID_MAX_NUM; ++i) {
        if (temp_key_id_arr[i] == 0) {
            temp_key_id_arr[i] = 1;
            break;
        }
    }

    if (i == TEMP_KEY_ID_MAX_NUM) {
        return -1;
    }
    else {
        return i;
    }
}

static void free_temp_key_id(int obj_id)
{
    int idx_for_arr = obj_id - TEMP_KEY_ID_START;

    if (idx_for_arr >=0 && idx_for_arr < TEMP_KEY_ID_MAX_NUM) {
        temp_key_id_arr[idx_for_arr] = 0;
    }
}
#endif /* WOLFSSL_MAXQ108X */

/*
 * WolfCrypt Functions
 */
int wc_MAXQ10XX_AesSetKey(Aes* aes, const byte* userKey, word32 keylen)
{
    XMEMCPY(aes->maxq_ctx.key, userKey, keylen);
    aes->maxq_ctx.key_pending = 1;
    return 0;
}

int wc_MAXQ10XX_EccSetKey(ecc_key* key, word32 keysize)
{
    int err = 0;
    int keytype = key->type;
    word32 bufflen = 0;

    if (key->maxq_ctx.hw_ecc == -1) {
        err = WC_HW_E;
    }

    if (err == 0) {
        if (key->dp->id != ECC_SECP256R1) {
            err = ECC_CURVE_OID_E;
        }
    }

    if (err == 0) {
        if ((keytype != ECC_PUBLICKEY) && (keytype != ECC_PRIVATEKEY) &&
            (keytype != ECC_PRIVATEKEY_ONLY)) {
            err = BAD_FUNC_ARG;
        }
    }

    if (err == 0) {
        bufflen = keysize;
        if ((keytype == ECC_PUBLICKEY) || (keytype == ECC_PRIVATEKEY)) {
            err = wc_export_int(key->pubkey.x, key->maxq_ctx.ecc_key,
                                &bufflen, keysize, WC_TYPE_UNSIGNED_BIN);
        }
    }

    if (err == 0) {
        if ((keytype == ECC_PUBLICKEY) || (keytype == ECC_PRIVATEKEY)) {
            err = wc_export_int(key->pubkey.y, key->maxq_ctx.ecc_key + keysize,
                                &bufflen, keysize, WC_TYPE_UNSIGNED_BIN);
        }
    }

    if (err == 0) {
        if ((keytype == ECC_PRIVATEKEY) || (keytype == ECC_PRIVATEKEY_ONLY)) {
            err = wc_export_int(wc_ecc_key_get_priv(key),
                key->maxq_ctx.ecc_key + (2 * keysize), &bufflen, keysize,
                WC_TYPE_UNSIGNED_BIN);
        }
    }

    if (err == 0) {
        key->maxq_ctx.hw_ecc = 1;
        key->maxq_ctx.key_pending = 1;
    }
    else {
        key->maxq_ctx.hw_ecc = -1;
    }

    return err;
}

#ifdef MAXQ_AESGCM
static int aes_set_key(Aes* aes, const byte* userKey, word32 keylen)
{
    mxq_u1 key_buff[MAX_KEY_DATASIZE];
    mxq_err_t mxq_rc;
    int rc;
    unsigned char sign_key[MAX_SIGNKEY_DATASIZE];
    int sign_key_len, sign_key_curve, sign_key_type;
    mxq_u1 signature[MAX_SIG_DATASIZE];
    int signature_len = (int)sizeof(signature);


    if (aes == NULL || (keylen != AES_128_KEY_SIZE &&
#ifdef WOLFSSL_AES_192
        keylen != AES_192_KEY_SIZE &&
#endif
        keylen != AES_256_KEY_SIZE)) {
        return BAD_FUNC_ARG;
    }

    rc = maxq_CryptHwMutexTryLock();
    if (rc != 0) {
        WOLFSSL_ERROR_MSG("MAXQ: aes_set_key() lock could not be acquired");
        rc = NOT_COMPILED_IN;
        return rc;
    }

    if (aes->maxq_ctx.key_obj_id) {
        wc_MAXQ10XX_AesFree(aes);
    }

    int obj_id = alloc_aes_key_id();
    if (!obj_id) {
        WOLFSSL_ERROR_MSG("MAXQ: alloc_aes_key_id() failed");
        rc = NOT_COMPILED_IN;
        goto end_AesSetKey;
    }

    mxq_rc = MXQ_CreateObject(obj_id, keylen, MXQ_OBJTYPE_SECRETKEY,
                              OBJPROP_PERSISTENT,
                              (char *)"ahs=rwdgx:ahs=rwdgx:ahs=rwdgx");
    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_CreateObject() failed");
        rc = NOT_COMPILED_IN;
        goto end_AesSetKey;
    }

    /* store the object id in the context */
    aes->maxq_ctx.key_obj_id = obj_id;

    mxq_length key_buff_len = sizeof(key_buff);
    mxq_rc = MXQ_BuildKey(key_buff, &key_buff_len, MXQ_KEYTYPE_AES, 0xff,
                          keylen, keylen, MXQ_KEYUSE_ENCRYPTION,
                          ALGO_CIPHER_AES_ECB, MXQ_KEYUSE_NONE, ALGO_NONE,
                          (mxq_u1 *)userKey);
    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_BuildKey() failed");
        rc = WC_HW_E;
        goto end_AesSetKey;
    }

    LoadDefaultImportKey(sign_key, &sign_key_len, &sign_key_curve,
                         &sign_key_type);

    rc = ECDSA_sign(signature, &signature_len, sign_key, key_buff, key_buff_len,
                    sign_key_curve);
    if (rc) {
        WOLFSSL_ERROR_MSG("MAXQ: ECDSA_sign() failed");
        goto end_AesSetKey;
    }

    mxq_rc = MXQ_ImportKey(obj_id, getSignAlgoFromCurve(sign_key_curve),
                           PUBKEY_IMPORT_OBJID, key_buff, key_buff_len,
                           signature, signature_len);
    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_ImportKey() failed");
        rc = WC_HW_E;
        goto end_AesSetKey;
    }

    /* key stored successfully */
    aes->maxq_ctx.key_pending = 0;

end_AesSetKey:
    wolfSSL_CryptHwMutexUnLock();
    return rc;
}
#endif /* MAXQ_AESGCM */

void wc_MAXQ10XX_AesFree(Aes* aes)
{
    mxq_err_t mxq_rc;
    int rc = 1;

    if (aes->maxq_ctx.key_obj_id != 0) {
        rc = 0;
    }

    if (rc == 0) {
        rc = wolfSSL_CryptHwMutexLock();
    }

    if (rc == 0) {
        mxq_rc = MXQ_DeleteObject(aes->maxq_ctx.key_obj_id);
        if (mxq_rc) {
            WOLFSSL_ERROR_MSG("MAXQ: MXQ_DeleteObject() failed");
            rc = 1;
        }

        if (rc == 0) {
            free_aes_key_id(aes->maxq_ctx.key_obj_id);
            aes->maxq_ctx.key_obj_id = 0;
        }
        wolfSSL_CryptHwMutexUnLock();
    }
}

#ifdef MAXQ_ECC
static int ecc_set_key(ecc_key* key, const byte* userKey, word32 keycomplen)
{
    mxq_err_t mxq_rc;
    int rc;
    word32 keylen;
    int objtype;
    mxq_u1 key_buff[MAX_KEY_DATASIZE];
    mxq_length key_buff_len = sizeof(key_buff);
    unsigned char sign_key[MAX_SIGNKEY_DATASIZE];
    int sign_key_len, sign_key_curve, sign_key_type;
    mxq_u1 signature[MAX_SIG_DATASIZE];
    int signature_len = (int)sizeof(signature);


    if ((key->type != ECC_PUBLICKEY) && (key->type != ECC_PRIVATEKEY) &&
        (key->type != ECC_PRIVATEKEY_ONLY)) {
        return BAD_FUNC_ARG;
    }

    if (key->type == ECC_PUBLICKEY) {
        keylen = keycomplen * 2;
        objtype = MXQ_OBJTYPE_PUBKEY;
    }
    else {
        keylen = keycomplen * 3;
        objtype = MXQ_OBJTYPE_KEYPAIR;
    }

    rc = maxq_CryptHwMutexTryLock();
    if (rc != 0) {
        WOLFSSL_ERROR_MSG("MAXQ: ecc_set_key() lock could not be acquired");
        rc = NOT_COMPILED_IN;
        return rc;
    }

    if (key->maxq_ctx.key_obj_id) {
        wc_MAXQ10XX_EccFree(key);
    }

    int obj_id = alloc_ecc_key_id();
    if (!obj_id) {
        WOLFSSL_ERROR_MSG("MAXQ: alloc_ecc_key_id() failed");
        rc = NOT_COMPILED_IN;
        goto end_EccSetKey;
    }

    mxq_rc = MXQ_CreateObject(obj_id, keylen, objtype, OBJPROP_PERSISTENT,
                              (char *)"ahs=rwdgx:ahs=rwdgx:ahs=rwdgx");
    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_CreateObject() failed");
        rc = NOT_COMPILED_IN;
        goto end_EccSetKey;
    }

    /* store the object id in the context */
    key->maxq_ctx.key_obj_id = obj_id;

    mxq_rc = MXQ_BuildKey(key_buff, &key_buff_len, MXQ_KEYTYPE_ECC,
                          MXQ_KEYPARAM_EC_P256R1, keycomplen, keylen,
                          MXQ_KEYUSE_DATASIGNATURE, ALGO_ECDSA_SHA_256,
                          MXQ_KEYUSE_NONE, ALGO_NONE, (mxq_u1 *)userKey);
    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_BuildKey() failed");
        rc = WC_HW_E;
        goto end_EccSetKey;
    }

    LoadDefaultImportKey(sign_key, &sign_key_len, &sign_key_curve,
                         &sign_key_type);

    rc = ECDSA_sign(signature, &signature_len, sign_key, key_buff, key_buff_len,
                    sign_key_curve);
    if (rc) {
        WOLFSSL_ERROR_MSG("MAXQ: ECDSA_sign() failed");
        goto end_EccSetKey;
    }

    mxq_rc = MXQ_ImportKey(obj_id, getSignAlgoFromCurve(sign_key_curve),
                           PUBKEY_IMPORT_OBJID, key_buff, key_buff_len,
                           signature, signature_len);
    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_ImportKey() failed");
        rc = WC_HW_E;
        goto end_EccSetKey;
    }

    /* key stored successfully */
    key->maxq_ctx.key_pending = 0;

end_EccSetKey:
    wolfSSL_CryptHwMutexUnLock();
    return rc;
}
#endif /* MAXQ_ECC */

void wc_MAXQ10XX_EccFree(ecc_key* key)
{
    if (key->maxq_ctx.key_obj_id == 0) {
        return;
    }

    int rc = wolfSSL_CryptHwMutexLock();
    if (rc != 0) {
        return;
    }

    mxq_err_t mxq_rc = MXQ_DeleteObject(key->maxq_ctx.key_obj_id);
    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_DeleteObject() failed");
        wolfSSL_CryptHwMutexUnLock();
        return;
    }

    free_ecc_key_id(key->maxq_ctx.key_obj_id);
    key->maxq_ctx.key_obj_id = 0;
    wolfSSL_CryptHwMutexUnLock();
}

void wc_MAXQ10XX_Sha256Copy(wc_Sha256* sha256)
{
    /* during copying, change to soft hash mode for one context */
    if (sha256->maxq_ctx.hash_running) {
        sha256->maxq_ctx.hash_running = 0;
        sha256->maxq_ctx.soft_hash = 1;
    }
}

void wc_MAXQ10XX_Sha256Free(wc_Sha256* sha256)
{
    /* release the mutex if a hash operation is running on the maxq10xx device
     */
    if (sha256->maxq_ctx.hash_running) {
        sha256->maxq_ctx.hash_running = 0;
        sha256->maxq_ctx.soft_hash = 1;
        wolfSSL_CryptHwMutexUnLock();
    }
}
#endif /* WOLFSSL_MAXQ10XX_CRYPTO */

#ifdef WOLF_CRYPTO_CB
#ifdef MAXQ_SHA256
static int maxq10xx_hash_update_sha256(const mxq_u1* psrc, mxq_length inlen,
                                       int running)
{
    mxq_err_t mxq_rc;

    if (running == 0) {
        mxq_rc = MXQ_MD_Init(ALGO_MD_SHA256);
        if (mxq_rc) {
            WOLFSSL_ERROR_MSG("MAXQ: MXQ_MD_Init() failed");
            return WC_HW_E;
        }
    }

    mxq_length data_offset = 0;
    mxq_length data_len;

    while (inlen) {
        data_len = (inlen < MAX_HASH_CHUNKSIZE) ? inlen : MAX_HASH_CHUNKSIZE;

        mxq_rc = MXQ_MD_Update(&psrc[data_offset], data_len);
        if (mxq_rc) {
            WOLFSSL_ERROR_MSG("MAXQ: MXQ_MD_Update() failed");
            return WC_HW_E;
        }

        data_offset += data_len;
        inlen -= data_len;
    }

    return 0;
}

static int maxq10xx_hash_finish_sha256(mxq_u1* pdest)
{
    mxq_err_t mxq_rc;
    mxq_length hashlen = WC_SHA256_DIGEST_SIZE;

    mxq_rc = MXQ_MD_Finish(pdest, &hashlen);
    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_MD_Finish() failed");
        return WC_HW_E;
    }

    return 0;
}
#endif /* MAXQ_SHA256 */

static int maxq10xx_cipher_do(mxq_algo_id_t algo_id, mxq_u1 encrypt,
                              mxq_u2 key_id, mxq_u1* p_in, mxq_u1* p_out,
                              mxq_length data_size, mxq_u1* p_iv,
                              mxq_length iv_len, mxq_u1* p_aad,
                              mxq_length aad_len, mxq_u1* p_tag,
                              mxq_length tag_len)
{
    mxq_err_t mxq_rc;
    ciph_params_t cparams;

    mxq_u1 internal_data[K_CHUNKSIZE + K_CIPHER_BLOCKSIZE];
    mxq_u1 *p_int_data = internal_data;

    mxq_length data_offset = 0;
    mxq_length proc_len = 0, req_len = 0;

    XMEMSET(&cparams, 0, sizeof(cparams));

    cparams.data_length  = data_size;
    cparams.p_iv         = p_iv;
    cparams.iv_length    = iv_len;
    cparams.p_aad        = p_aad;
    cparams.aad_length   = aad_len;

    if (encrypt) {
        cparams.aead_tag_len = tag_len;
    }
    else {
        XMEMCPY(cparams.aead_tag, p_tag, tag_len);
        cparams.aead_tag_len = tag_len;
    }

    mxq_rc = MXQ_Cipher_Init(encrypt, algo_id, key_id, &cparams, 0);
    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_Cipher_Init() failed");
        return WC_HW_E;
    }

    while (data_size) {
        proc_len = (data_size < K_CHUNKSIZE) ? data_size : K_CHUNKSIZE;
        req_len  = proc_len;

        mxq_rc = MXQ_Cipher_Update(&p_out, &p_in[data_offset], &proc_len);
        if (mxq_rc) {
            WOLFSSL_ERROR_MSG("MAXQ: MXQ_Cipher_Update() failed");
            return WC_HW_E;
        }

        data_offset += req_len;
        data_size -= req_len;
    }

    if (encrypt) {
        proc_len = tag_len;
        mxq_rc = MXQ_Cipher_Finish(&p_int_data, &proc_len);
        if (mxq_rc) {
            WOLFSSL_ERROR_MSG("MAXQ: Encrypt, MXQ_Cipher_Finish() failed");
            return WC_HW_E;
        }

        if (proc_len > tag_len) {
            XMEMCPY(p_out, internal_data, proc_len - tag_len);
        }

        if ((tag_len != 0) && (proc_len >= tag_len)) {
            XMEMCPY(p_tag, &internal_data[proc_len - tag_len], tag_len);
        }
    }
    else {
        internal_data[0] = 0xDE;
        XMEMCPY(&internal_data[1], p_tag, tag_len);
        proc_len = tag_len;

        mxq_rc = MXQ_Cipher_Finish(&p_int_data, &proc_len);
        if (mxq_rc) {
            WOLFSSL_ERROR_MSG("MAXQ: Decrypt, MXQ_Cipher_Finish() failed");
            return WC_HW_E;
        }

        if (proc_len) {
            XMEMCPY(p_out, internal_data, proc_len);
        }
    }

    return 0;
}

static int maxq10xx_ecc_sign_local(mxq_u2 key_id,
                                   mxq_u1* p_in, mxq_u2 data_size,
                                   mxq_u1* p_sign_out, mxq_length* sign_len,
                                   mxq_length keycomplen)
{
    mxq_err_t mxq_rc;
    int rc;
    mxq_u1 *input_digest = NULL;
    mxq_u1 *buff_sign = NULL;
    mxq_length buff_len = keycomplen * 2;
    byte *r = NULL;
    byte *s = NULL;

    input_digest = (mxq_u1 *)XMALLOC(keycomplen, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    buff_sign = (mxq_u1 *)XMALLOC(keycomplen * 2, NULL,
                                  DYNAMIC_TYPE_TMP_BUFFER);
    if (input_digest == NULL || buff_sign == NULL) {
        XFREE(input_digest, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(buff_sign, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        return MEMORY_E;
    }
    r = &buff_sign[0];
    s = &buff_sign[keycomplen];

    /* truncate input to match key size */
    if (data_size > keycomplen) {
        data_size = keycomplen;
    }

    /* build input digest */
    XMEMSET(input_digest, 0, keycomplen);
    XMEMCPY(&input_digest[keycomplen - data_size], p_in, data_size);

    mxq_rc = MXQ_Sign(ALGO_ECDSA_PLAIN, key_id, input_digest,
                      keycomplen, buff_sign, &buff_len);
    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_Sign() failed");
        return WC_HW_E;
    }

    /* convert r and s to signature */
    rc = wc_ecc_rs_raw_to_sig((const byte *)r, keycomplen, (const byte *)s,
                              keycomplen, p_sign_out, sign_len);
    if (rc != 0) {
        WOLFSSL_ERROR_MSG("MAXQ: converting r and s to signature failed");
    }

    XFREE(input_digest, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(buff_sign, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    return rc;
}

#ifdef MAXQ_ECC
static int maxq10xx_ecc_verify_local(
               mxq_u2 key_id, mxq_u1* p_in, mxq_u2 data_size,
               mxq_u1* p_sign, mxq_u1 sign_len, int *result,
               mxq_length keycomplen)
{
    int rc;
    mxq_err_t mxq_rc;
    mxq_u1 *buff_rs = NULL;
    mxq_u1 *input_digest = NULL;
    mxq_u1 *buff_signature = NULL;
    byte *r = NULL;
    byte *s = NULL;
    word32 r_len = keycomplen;
    word32 s_len = keycomplen;

    input_digest = (mxq_u1 *)XMALLOC(keycomplen, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    buff_rs = (mxq_u1 *)XMALLOC(keycomplen * 2, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    buff_signature = (mxq_u1 *)XMALLOC(keycomplen * 2, NULL,
                                       DYNAMIC_TYPE_TMP_BUFFER);

    if (input_digest == NULL || buff_rs == NULL || buff_signature == NULL) {
        XFREE(input_digest, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(buff_rs, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(buff_signature, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        return MEMORY_E;
    }
    r = &buff_rs[0];
    s = &buff_rs[keycomplen];

    /* truncate input to match key size */
    if (data_size > keycomplen) {
        data_size = keycomplen;
    }

    /* build input digest */
    XMEMSET(input_digest, 0, keycomplen);
    XMEMCPY(&input_digest[keycomplen - data_size], p_in, data_size);

    /* extract r and s from signature */
    XMEMSET(buff_rs, 0, keycomplen * 2);

    rc = wc_ecc_sig_to_rs(p_sign, sign_len, r, &r_len, s, &s_len);
    if (rc != 0) {
        XFREE(input_digest, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(buff_rs, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(buff_signature, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        WOLFSSL_ERROR_MSG("MAXQ: extracting r and s from signature failed");
        *result = 0;
        return rc;
    }

    if ((r_len > keycomplen) || (s_len > keycomplen)) {
        XFREE(input_digest, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(buff_rs, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(buff_signature, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        WOLFSSL_ERROR_MSG("MAXQ: r and s corrupted");
        *result = 0;
        return BUFFER_E;
    }

    /* prepare raw signature */
    XMEMSET(buff_signature, 0, keycomplen * 2);

    /* add leading zeros if necessary */
    XMEMCPY(&buff_signature[keycomplen - r_len], r, r_len);
    XMEMCPY(&buff_signature[(keycomplen * 2) - s_len], s, s_len);

    mxq_rc = MXQ_Verify(ALGO_ECDSA_PLAIN, key_id, input_digest,
                        keycomplen, buff_signature, keycomplen * 2);

    XFREE(input_digest, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(buff_rs, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(buff_signature, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    *result = (mxq_rc ? 0 : 1);
    return 0;
}
#endif /* MAXQ_ECC */

#ifdef MAXQ_RNG
static int maxq10xx_random(byte* output, unsigned short sz)
{
#if defined(WOLFSSL_MAXQ108X)
    if (!tls13active) {
        return NOT_COMPILED_IN;
    }
#endif

    if (output == NULL) {
        return BUFFER_E;
    }

    int ret = maxq_CryptHwMutexTryLock();
    if (ret != 0) {
        WOLFSSL_ERROR_MSG("MAXQ: maxq10xx_random() lock could not be acquired");
        ret = NOT_COMPILED_IN;
        return ret;
    }

    if (MXQ_Get_Random_Ext(output, sz, 0)) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_Get_Random_Ext() failed");
        wolfSSL_CryptHwMutexUnLock();
        return WC_HW_E;
    }

    wolfSSL_CryptHwMutexUnLock();
    return 0;
}
#endif /* MAXQ_RNG */

#if !defined(NO_AES) && defined(HAVE_AESGCM) && defined(MAXQ_AESGCM)
static int do_aesgcm(wc_CryptoInfo* info)
{
    int rc;
    if (info->cipher.enc) {
        if (info->cipher.aesgcm_enc.authTagSz > 16) {
            return CRYPTOCB_UNAVAILABLE;
        }

        if (info->cipher.aesgcm_enc.sz == 0) {
            return CRYPTOCB_UNAVAILABLE;
        }

        if (info->cipher.aesgcm_enc.ivSz != 12) {
            return CRYPTOCB_UNAVAILABLE;
        }

        if (info->cipher.aesgcm_enc.aes->maxq_ctx.key_pending) {
            rc = aes_set_key(
                info->cipher.aesgcm_enc.aes,
                (const byte *)info->cipher.aesgcm_enc.aes->maxq_ctx.key,
                info->cipher.aesgcm_enc.aes->keylen);
            if (rc != 0) {
                return rc;
            }
        }

        rc = wolfSSL_CryptHwMutexLock();
        if (rc != 0) {
            return rc;
        }

        rc = maxq10xx_cipher_do(
            ALGO_CIPHER_AES_GCM,
            1,
            info->cipher.aesgcm_enc.aes->maxq_ctx.key_obj_id,
            (byte *)info->cipher.aesgcm_enc.in,
            (byte *)info->cipher.aesgcm_enc.out,
            info->cipher.aesgcm_enc.sz,
            (byte *)info->cipher.aesgcm_enc.iv,
            info->cipher.aesgcm_enc.ivSz,
            (byte *)info->cipher.aesgcm_enc.authIn,
            info->cipher.aesgcm_enc.authInSz,
            (byte *)info->cipher.aesgcm_enc.authTag,
            info->cipher.aesgcm_enc.authTagSz);
        if (rc != 0) {
            wolfSSL_CryptHwMutexUnLock();
            return rc;
        }

        wolfSSL_CryptHwMutexUnLock();
    }
    else {
        if (info->cipher.aesgcm_dec.authTagSz != 16) {
            return CRYPTOCB_UNAVAILABLE;
        }

        if (info->cipher.aesgcm_dec.sz == 0) {
            return CRYPTOCB_UNAVAILABLE;
        }

        if (info->cipher.aesgcm_dec.ivSz != 12) {
            return CRYPTOCB_UNAVAILABLE;
        }

        if (info->cipher.aesgcm_dec.aes->maxq_ctx.key_pending) {
            rc = aes_set_key(
                info->cipher.aesgcm_dec.aes,
                (const byte *)info->cipher.aesgcm_dec.aes->maxq_ctx.key,
                info->cipher.aesgcm_dec.aes->keylen);
            if (rc != 0) {
                return rc;
            }
        }

        rc = wolfSSL_CryptHwMutexLock();
        if (rc != 0) {
            return rc;
        }

        rc = maxq10xx_cipher_do(
            ALGO_CIPHER_AES_GCM,
            0,
            info->cipher.aesgcm_dec.aes->maxq_ctx.key_obj_id,
            (byte *)info->cipher.aesgcm_dec.in,
            (byte *)info->cipher.aesgcm_dec.out,
            info->cipher.aesgcm_dec.sz,
            (byte *)info->cipher.aesgcm_dec.iv,
            info->cipher.aesgcm_dec.ivSz,
            (byte *)info->cipher.aesgcm_dec.authIn,
            info->cipher.aesgcm_dec.authInSz,
            (byte *)info->cipher.aesgcm_dec.authTag,
            info->cipher.aesgcm_dec.authTagSz);
        if (rc != 0) {
            wolfSSL_CryptHwMutexUnLock();
            return rc;
        }

        wolfSSL_CryptHwMutexUnLock();
    }

    /* done */
    return 0;
}
#endif /* !NO_AES && HAVE_AESGCM && MAXQ_AESGCM */

#if !defined(NO_SHA) && !defined(NO_SHA256) && defined(MAXQ_SHA256)
static int do_sha256(wc_CryptoInfo* info)
{
    int rc;
    if (info->hash.sha256->maxq_ctx.soft_hash) {
        return CRYPTOCB_UNAVAILABLE;
    }

    if ((info->hash.in == NULL) && (info->hash.digest == NULL)) {
        return WC_HW_E;
    }

    if (info->hash.sha256->maxq_ctx.hash_running == 0) {
        rc = maxq_CryptHwMutexTryLock();
        if (rc != 0) {
            info->hash.sha256->maxq_ctx.soft_hash = 1;
            return CRYPTOCB_UNAVAILABLE;
        }
    }

    if (info->hash.in != NULL) {
        /* wc_Sha256Update */
        if ((info->hash.sha256->maxq_ctx.hash_running == 0) &&
                (info->hash.inSz == 0)) {
            info->hash.sha256->maxq_ctx.soft_hash = 1;
            wolfSSL_CryptHwMutexUnLock();
            return CRYPTOCB_UNAVAILABLE;
        }

        rc = maxq10xx_hash_update_sha256(info->hash.in, info->hash.inSz,
                    info->hash.sha256->maxq_ctx.hash_running);
        if (rc != 0) {
            info->hash.sha256->maxq_ctx.hash_running = 0;
            wolfSSL_CryptHwMutexUnLock();
            return rc;
        }

        info->hash.sha256->maxq_ctx.hash_running = 1;

        /* save soft hash context in case of wc_Sha256Copy */
        return CRYPTOCB_UNAVAILABLE;
    }
    else if (info->hash.digest != NULL) {
        /* wc_Sha256Final */
        if (info->hash.sha256->maxq_ctx.hash_running == 0) {
            info->hash.sha256->maxq_ctx.soft_hash = 1;
            wolfSSL_CryptHwMutexUnLock();
            return CRYPTOCB_UNAVAILABLE;
        }

        rc = maxq10xx_hash_finish_sha256(info->hash.digest);
        if (rc != 0) {
            info->hash.sha256->maxq_ctx.hash_running = 0;
            wolfSSL_CryptHwMutexUnLock();
            return rc;
        }

        info->hash.sha256->maxq_ctx.hash_running = 0;
        wolfSSL_CryptHwMutexUnLock();
        /* done */
        rc = 0;
    }
    return rc;
}
#endif /* !NO_SHA && !NO_SHA256 && MAXQ_SHA256 */

int wolfSSL_MAXQ10XX_CryptoDevCb(int devId, wc_CryptoInfo* info, void* ctx)
{
    int rc = CRYPTOCB_UNAVAILABLE;
    (void)devId;
    (void)ctx;

#if defined(WOLFSSL_MAXQ108X)
    if (!tls13active)
#endif
        return CRYPTOCB_UNAVAILABLE;

    if (info->algo_type == WC_ALGO_TYPE_CIPHER) {
#if !defined(NO_AES) || !defined(NO_DES3)
    #if defined(HAVE_AESGCM) && defined(MAXQ_AESGCM)
        if (info->cipher.type == WC_CIPHER_AES_GCM) {
            rc = do_aesgcm(info);
        }
    #endif /* HAVE_AESGCM && MAXQ_AESGCM */
    #ifdef HAVE_AES_CBC
        if (info->cipher.type == WC_CIPHER_AES_CBC) {
            /* TODO */
            return CRYPTOCB_UNAVAILABLE;
        }
    #endif /* HAVE_AES_CBC */
#endif /* !NO_AES || !NO_DES3 */
    }
#if !defined(NO_SHA) || !defined(NO_SHA256)
    else if (info->algo_type == WC_ALGO_TYPE_HASH) {
    #if !defined(NO_SHA) && defined(MAXQ_SHA1)
        if (info->hash.type == WC_HASH_TYPE_SHA) {
            /* TODO */
            return CRYPTOCB_UNAVAILABLE;
        }
        else
    #endif /* !NO_SHA && MAXQ_SHA1 */
    #if !defined(NO_SHA256) && defined(MAXQ_SHA256)
        if (info->hash.type == WC_HASH_TYPE_SHA256) {
            rc = do_sha256(info);
        }
    #endif /* !NO_SHA256 && MAXQ_SHA256 */
    }
#endif /* !NO_SHA || !NO_SHA256 */
#if !defined(WC_NO_RNG) && defined(MAXQ_RNG)
    else if (info->algo_type == WC_ALGO_TYPE_SEED) {
        rc = maxq10xx_random(info->seed.seed, info->seed.sz);
    }
    else if (info->algo_type == WC_ALGO_TYPE_RNG) {
        rc = maxq10xx_random(info->rng.out, info->rng.sz);
    }
#endif /* !WC_NO_RNG && MAXQ_RNG */
    else if (info->algo_type == WC_ALGO_TYPE_PK) {
    #if defined(HAVE_ECC) && defined(MAXQ_ECC)
        if (info->pk.type == WC_PK_TYPE_EC_KEYGEN) {
            /* TODO */
            return CRYPTOCB_UNAVAILABLE;
        }
        else if (info->pk.type == WC_PK_TYPE_ECDH) {
            /* TODO */
            return CRYPTOCB_UNAVAILABLE;
        }
        else if (info->pk.type == WC_PK_TYPE_ECDSA_SIGN) {
            if (info->pk.eccsign.key->maxq_ctx.hw_ecc == 0) {
                rc = wc_MAXQ10XX_EccSetKey(info->pk.eccsign.key,
                                           info->pk.eccsign.key->dp->size);
                if (rc != 0) {
                    return rc;
                }
            }

            if (info->pk.eccsign.key->maxq_ctx.hw_ecc == -1) {
                return CRYPTOCB_UNAVAILABLE;
            }

            if (info->pk.eccsign.key->maxq_ctx.key_pending) {
                rc = ecc_set_key(info->pk.eccsign.key,
                                 info->pk.eccsign.key->maxq_ctx.ecc_key,
                                 info->pk.eccsign.key->dp->size);
                if (rc != 0) {
                    return rc;
                }
            }

            rc = wolfSSL_CryptHwMutexLock();
            if (rc != 0) {
                return rc;
            }

            rc = maxq10xx_ecc_sign_local(
                     info->pk.eccsign.key->maxq_ctx.key_obj_id,
                     (byte *)info->pk.eccsign.in, info->pk.eccsign.inlen,
                     info->pk.eccsign.out, info->pk.eccsign.outlen,
                     info->pk.eccsign.key->dp->size);

            if (rc != 0) {
                wolfSSL_CryptHwMutexUnLock();
                return rc;
            }

            wolfSSL_CryptHwMutexUnLock();
            /* done */
            rc = 0;
        }
        else if (info->pk.type == WC_PK_TYPE_ECDSA_VERIFY) {
            if (info->pk.eccverify.key->type == ECC_PRIVATEKEY_ONLY) {
                return CRYPTOCB_UNAVAILABLE;
            }

            if (info->pk.eccverify.key->maxq_ctx.hw_ecc == 0) {
                rc = wc_MAXQ10XX_EccSetKey(info->pk.eccverify.key,
                                           info->pk.eccverify.key->dp->size);
                if (rc != 0) {
                    return rc;
                }
            }

            if (info->pk.eccverify.key->maxq_ctx.hw_ecc == -1) {
                return CRYPTOCB_UNAVAILABLE;
            }

            if (info->pk.eccverify.key->maxq_ctx.key_pending) {
                rc = ecc_set_key(info->pk.eccverify.key,
                                 info->pk.eccverify.key->maxq_ctx.ecc_key,
                                 info->pk.eccverify.key->dp->size);
                if (rc != 0) {
                    return rc;
                }
            }

            rc = wolfSSL_CryptHwMutexLock();
            if (rc != 0) {
                return rc;
            }

            rc =
                maxq10xx_ecc_verify_local(
                    info->pk.eccverify.key->maxq_ctx.key_obj_id,
                    (byte *)info->pk.eccverify.hash,
                    info->pk.eccverify.hashlen,
                    (byte *)info->pk.eccverify.sig,
                    info->pk.eccverify.siglen,
                    info->pk.eccverify.res,
                    info->pk.eccverify.key->dp->size);

            wolfSSL_CryptHwMutexUnLock();

            if (rc != 0) {
                return rc;
            }

            /* Success */
            rc = 0;
        }
    #endif /* HAVE_ECC && MAXQ_ECC */
    }
#ifdef WOLFSSL_MAXQ108X
    else if (info->algo_type == WC_ALGO_TYPE_HMAC) {
        if (info->hmac.in != NULL && info->hmac.digest == NULL) {
            rc = 0;
            if (mac_comp_active == 0) {
                rc = wc_MAXQ10XX_HmacSetKey(info->hmac.macType);
            }
            if (rc == 0) {
                rc = wc_MAXQ10XX_HmacUpdate(info->hmac.in, info->hmac.inSz);
            }
        }
        else if (info->hmac.in == NULL && info->hmac.digest != NULL) {
            rc = wc_MAXQ10XX_HmacFinal(info->hmac.digest);
        }
        else {
            rc = BAD_FUNC_ARG;
        }
    }
#endif /* WOLFSSL_MAXQ108X */

    if (rc != 0 && rc != CRYPTOCB_UNAVAILABLE) {
        rc = WC_HW_E;
    }

    return rc;
}

static int wolfSSL_Soft_CryptoDevCb(int devId, wc_CryptoInfo* info, void* ctx)
{
    (void)devId;
    (void)info;
    (void)ctx;

    return CRYPTOCB_UNAVAILABLE;
}
#endif /* WOLF_CRYPTO_CB */

#ifdef WOLFSSL_MAXQ10XX_TLS
#ifdef WOLFSSL_MAXQ108X
static int calculate_modulus_offset(const unsigned char * cert_data, int offset)
{
    int i;
    int l_offset = offset;
    for (i = 0; i < 2; i++) {
        if ((cert_data[l_offset]) & 0x80) {
            if ((cert_data[l_offset] & 0x7f) == 1) {
                l_offset += 3;
            }
            if ((cert_data[l_offset] & 0x7f) == 2) {
                l_offset += 4;
            }
        }
        else {
            l_offset += 2;
        }
    }
    return l_offset;
}

static int maxq_curve_wolfssl_id2mxq_id(word32 curve_id, mxq_length *keycomplen)
{
    switch (curve_id) {
    case ECC_SECP256R1_OID:
        *keycomplen = 32;
        return MXQ_KEYPARAM_EC_P256R1;
    case ECC_SECP384R1_OID:
        *keycomplen = 48;
        return MXQ_KEYPARAM_EC_P384R1;
    case ECC_SECP521R1_OID:
        *keycomplen = 66;
        return MXQ_KEYPARAM_EC_P521R1;
    case ECC_BRAINPOOLP256R1_OID:
        *keycomplen = 32;
        return MXQ_KEYPARAM_EC_BP256R1;
    case ECC_BRAINPOOLP384R1_OID:
        *keycomplen = 48;
        return MXQ_KEYPARAM_EC_BP384R1;
    case ECC_BRAINPOOLP512R1_OID:
        *keycomplen = 64;
        return MXQ_KEYPARAM_EC_BP512R1;
    default:
        return MXQ_UNKNOWN_CURVE;
    }
}

static int mxq_get_sign_alg_from_sig_oid(word32 maxq_id) {

    switch (maxq_id) {
    case CTC_SHA256wECDSA:
        return ALGO_ECDSA_SHA_256;
    case CTC_SHA384wECDSA:
        return ALGO_ECDSA_SHA_384;
    case CTC_SHA512wECDSA:
        return ALGO_ECDSA_SHA_512;
    case CTC_SHA256wRSA:
        return ALGO_RSASSAPSSPKCS1_V1_5_SHA256;
    case CTC_SHA384wRSA:
        return ALGO_RSASSAPSSPKCS1_V1_5_SHA384;
    case CTC_SHA512wRSA:
        return ALGO_RSASSAPSSPKCS1_V1_5_SHA512;
    default:
        return ALGO_INVALID;
    }
}
#endif /* WOLFSSL_MAXQ108X */

/* TODO: Get rid of publicKeyIndex. Pass in the information it is parsing.
 *       Then Improve this to pass in the parts of decoded cert it needs.
 */
static int maxq10xx_process_server_certificate(WOLFSSL* ssl,
                                               DecodedCert* p_cert)
{
    mxq_keytype_id_t key_type = MXQ_KEYTYPE_ECC;
    mxq_keyparam_id_t keyparam = MXQ_KEYPARAM_EC_P256R1;
    mxq_length totalkeylen;
    mxq_algo_id_t sign_algo = ALGO_ECDSA_SHA_256;
    int pk_offset = p_cert->publicKeyIndex;
    mxq_length keycomplen = ECC_KEYCOMPLEN;
    int rc;
    mxq_err_t mxq_rc;
    mxq_u1 certdata[MAX_CERT_DATASIZE];
    mxq_length certdatalen = sizeof(certdata);
    (void)ssl;
#if defined(WOLFSSL_MAXQ1065)

    if (p_cert->signatureOID != CTC_SHA256wECDSA) {
        WOLFSSL_MSG("MAXQ: signature algo not supported");
        return NOT_COMPILED_IN;
    }

    if (p_cert->keyOID != ECDSAk) {
        WOLFSSL_MSG("MAXQ: key algo not supported");
        return NOT_COMPILED_IN;
    }

    if (p_cert->pkCurveOID != ECC_SECP256R1_OID) {
        WOLFSSL_MSG("MAXQ: key curve not supported");
        return NOT_COMPILED_IN;
    }

    totalkeylen = keycomplen * 2;

#elif defined(WOLFSSL_MAXQ108X)

    if (p_cert->keyOID == ECDSAk )
    {
        keyparam = maxq_curve_wolfssl_id2mxq_id(p_cert->pkCurveOID,
                                                &keycomplen);
        if (keyparam == MXQ_UNKNOWN_CURVE) {
            WOLFSSL_MSG("MAXQ: key curve not supported");
            return NOT_COMPILED_IN;
        }
        totalkeylen = keycomplen * 2;
    }
    else if (p_cert->keyOID == RSAk) {
        pk_offset = calculate_modulus_offset(p_cert->source,
                                             p_cert->publicKeyIndex+1);
        keycomplen = ((p_cert->source[pk_offset-2] << 8) |
                      p_cert->source[pk_offset-1]);

        /* Is there a more elegant way for checking pub key??? */
        if (p_cert->publicKey[p_cert->pubKeySize-1] == 0x03 &&
            p_cert->publicKey[p_cert->pubKeySize-2] == 0x01) {
            keyparam = MXQ_KEYPARAM_RSA_PUB3;
        }
        else if (p_cert->publicKey[p_cert->pubKeySize-1] == 0x01 &&
                 p_cert->publicKey[p_cert->pubKeySize-2] == 0x00 &&
                 p_cert->publicKey[p_cert->pubKeySize-3] == 0x01 &&
                 p_cert->publicKey[p_cert->pubKeySize-4] == 0x03) {
            keyparam = MXQ_KEYPARAM_RSA_PUB65537;
        }
        else {
            WOLFSSL_MSG("MAXQ: RSA public key not supported");
            return NOT_COMPILED_IN;
        }
        key_type = MXQ_KEYTYPE_RSA;
        totalkeylen = keycomplen;

    }
    else {
        WOLFSSL_MSG("MAXQ: key algo not supported");
        return NOT_COMPILED_IN;
    }

    tls13_server_key_algo = p_cert->keyOID;
    tls13_server_key_len = keycomplen;
    sign_algo = mxq_get_sign_alg_from_sig_oid(p_cert->signatureOID);

    if (sign_algo == ALGO_INVALID) {
        WOLFSSL_MSG("MAXQ: signature algo not supported");
        return NOT_COMPILED_IN;
    }
#endif

    rc = wolfSSL_CryptHwMutexLock();
    if (rc != 0) {
        return rc;
    }

    mxq_rc = MXQ_Build_EC_Cert(certdata, &certdatalen, key_type, keyparam,
                               keycomplen, totalkeylen, pk_offset,
                               p_cert->certBegin,
                               (p_cert->sigIndex - p_cert->certBegin),
                               p_cert->maxIdx, sign_algo, ROOT_CA_CERT_OBJ_ID,
                               MXQ_KEYUSE_VERIFY_KEY_CERT, ALGO_ECDSA_SHA_any,
                               MXQ_KEYUSE_DATASIGNATURE, ALGO_ECDSA_SHA_any,
                               (mxq_u1 *)p_cert->source);
    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_Build_EC_Cert() failed");
        wolfSSL_CryptHwMutexUnLock();
        return WC_HW_E;
    }

#if defined(WOLFSSL_MAXQ108X)
    if (tls13_server_cert_id == -1) {
        tls13_server_cert_id = alloc_temp_key_id();
        if (tls13_server_cert_id == -1) {
            WOLFSSL_ERROR_MSG("MAXQ: alloc_temp_key_id() failed");
            wolfSSL_CryptHwMutexUnLock();
            return WC_HW_E;
        }
    }

    /* temporary certificate stored in object id cert_id */
    mxq_rc = MXQ_ImportChildCert(tls13_server_cert_id, certdata, certdatalen);
#else
    /* temporary certificate stored in object id 0 */
    mxq_rc = MXQ_ImportChildCert(0, certdata, certdatalen);
#endif

    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_ImportChildCert() failed");
        wolfSSL_CryptHwMutexUnLock();
        return WC_HW_E;
    }

    wolfSSL_CryptHwMutexUnLock();
    return 0;
}

static int maxq10xx_process_server_sig_kex(WOLFSSL* ssl, byte p_sig_algo,
               const byte* p_sig, word32 p_sig_len,
               const byte* p_rand, word32 p_rand_len,
               const byte* p_server_params, word32 p_server_params_len)
{
    int rc;
    mxq_err_t mxq_rc;
    if (ssl->specs.kea != ecc_diffie_hellman_kea) {
        WOLFSSL_MSG("MAXQ: key exchange algo not supported");
        return NOT_COMPILED_IN;
    }

    if (ssl->ecdhCurveOID != ECC_SECP256R1_OID) {
        WOLFSSL_MSG("MAXQ: key curve not supported");
        return NOT_COMPILED_IN;
    }

    if (p_sig_algo != ecc_dsa_sa_algo) {
        WOLFSSL_MSG("MAXQ: signature algo not supported");
        return NOT_COMPILED_IN;
    }

    rc = wolfSSL_CryptHwMutexLock();
    if (rc != 0) {
        return rc;
    }

    /* NOTE: this function also does verification of the signature as well! */
    mxq_rc = MXQ_SetECDHEKey(ALGO_ECDSA_SHA_256, MXQ_KEYPARAM_EC_P256R1, 0,
                             (mxq_u1 *)p_rand, p_rand_len,
                             (mxq_u1 *)p_server_params, p_server_params_len,
                             (mxq_u1 *)p_sig, p_sig_len);
    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_SetECDHEKey() failed");
        wolfSSL_CryptHwMutexUnLock();
        return WC_HW_E;
    }

    wolfSSL_CryptHwMutexUnLock();
    return 0;
}

static int maxq10xx_gen_tls_master_secret(WOLFSSL* ssl, void *ctx)
{
    int rc;
    mxq_err_t mxq_rc;
    mxq_secret_context_api_t secret_conf;
    mxq_u1 tls_rand[SEED_LEN];
    (void)ctx;

    if ((ssl->specs.kea != ecc_diffie_hellman_kea) &&
        (ssl->specs.kea != psk_kea)) {
        WOLFSSL_MSG("MAXQ: key exchange algo not supported");
        return NOT_COMPILED_IN;
    }

    if (ssl->specs.kea == ecc_diffie_hellman_kea) {
        if ((ssl->hsKey == NULL ) ||
            (((ecc_key*)ssl->hsKey)->maxq_ctx.hw_storage == 0)) {
            return NOT_COMPILED_IN;
        }
    }

    XMEMCPY(tls_rand, ssl->arrays->clientRandom, RAN_LEN);
    XMEMCPY(&tls_rand[RAN_LEN], ssl->arrays->serverRandom, RAN_LEN);

    XMEMSET(&secret_conf, 0 ,sizeof(secret_conf));
    secret_conf.pass = 0;
    secret_conf.CSID = ssl->options.cipherSuite |
                       (ssl->options.cipherSuite0 << 8);
    secret_conf.Random = tls_rand;
    secret_conf.Random_size = SEED_LEN;
    secret_conf.is_session_key_secret = 1;
    if (ssl->specs.kea == psk_kea) {
        secret_conf.PSK_info.psk_id = PSK_OBJ_ID;
    }

    rc = wolfSSL_CryptHwMutexLock();
    if (rc != 0) {
        return rc;
    }

    mxq_rc = MXQ_Perform_Key_Exchange(&secret_conf);
    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_Perform_Key_Exchange() failed");
        wolfSSL_CryptHwMutexUnLock();
        return WC_HW_E;
    }

    ssl->maxq_ctx.use_hw_keys = 1;
    wolfSSL_CryptHwMutexUnLock();

#ifdef MAXQ_EXPORT_TLS_KEYS
    rc = StoreKeys(ssl, secret_conf.PSK_info.psk_key_bloc,
                   PROVISION_CLIENT_SERVER);
    if (rc != 0) {
        WOLFSSL_ERROR_MSG("MAXQ: StoreKeys() failed");
        return rc;
    }
#endif

    return 0;
}

static int maxq10xx_perform_client_finished(WOLFSSL* ssl, const byte* p_label,
               const byte* p_seed, word32 seedSz, byte* p_dest, void* ctx)
{
    int rc;
    mxq_err_t mxq_rc;
    (void)ssl;
    (void)ctx;

    rc = wolfSSL_CryptHwMutexLock();
    if (rc != 0) {
        return rc;
    }

    mxq_rc = MXQ_tls_prf_sha_256(0, p_label, FINISHED_LABEL_SZ,
                                 p_seed, seedSz,
                                 p_dest, TLS_FINISHED_SZ);
    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_tls_prf_sha_256() failed");
        wolfSSL_CryptHwMutexUnLock();
        return WC_HW_E;
    }

    wolfSSL_CryptHwMutexUnLock();
    return 0;
}

static int maxq10xx_perform_tls12_record_processing(WOLFSSL* ssl,
               int is_encrypt,
               byte* out, const byte* in, word32 sz,
               const byte* iv, word32 ivSz,
               byte* authTag, word32 authTagSz,
               const byte* authIn, word32 authInSz)
{
    int rc;
    mxq_err_t mxq_rc;
    mxq_u2 key_id = (is_encrypt == 1) ? 1 : 0;
    mxq_algo_id_t algo_id = 0;

    if (! ssl->maxq_ctx.use_hw_keys) {
        return NOT_COMPILED_IN;
    }

    if ((ssl->specs.bulk_cipher_algorithm != wolfssl_aes_gcm) &&
            (ssl->specs.bulk_cipher_algorithm != wolfssl_aes_ccm)) {
        WOLFSSL_MSG("MAXQ: tls record cipher algo not supported");
        return NOT_COMPILED_IN;
    }

    if (ssl->specs.bulk_cipher_algorithm == wolfssl_aes_gcm) {
        algo_id = ALGO_CIPHER_AES_GCM;
    }
    else if (ssl->specs.bulk_cipher_algorithm == wolfssl_aes_ccm) {
        algo_id = ALGO_CIPHER_AES_CCM;
    }

    rc = wolfSSL_CryptHwMutexLock();
    if (rc != 0) {
        return rc;
    }

    mxq_rc = maxq10xx_cipher_do(algo_id, is_encrypt, key_id, (mxq_u1 *)in,
                                out, sz, (mxq_u1 *)iv, ivSz,
                                (mxq_u1 *)authIn, authInSz, authTag, authTagSz);
    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: maxq10xx_cipher_do() failed");
        wolfSSL_CryptHwMutexUnLock();
        return WC_HW_E;
    }

    wolfSSL_CryptHwMutexUnLock();
    return 0;
}

static int maxq10xx_read_device_cert_der(byte* p_dest_buff, word32* p_len)
{
    int rc;
    mxq_err_t mxq_rc;
    word32 cert_size = 0;

#if defined(WOLFSSL_MAXQ108X)
    DecodedCert decoded;
    mxq_keyparam_id_t keyparam = MXQ_KEYPARAM_EC_P256R1;
    int pk_offset = 0;
#endif

    WOLFSSL_ENTER("maxq10xx_read_device_cert_der");
    if (!p_dest_buff || !p_len) {
        return BAD_FUNC_ARG;
    }

    if (*p_len < 1024) {
        WOLFSSL_ERROR_MSG("MAXQ: insufficient buffer length");
        return BAD_FUNC_ARG;
    }

    rc = wolfSSL_CryptHwMutexLock();
    if (rc != 0) {
        return rc;
    }

    mxq_rc = MXQ_ReadObject(DEVICE_CERT_OBJ_ID, 24, p_dest_buff, p_len);
    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_ReadObject() failed");
        wolfSSL_CryptHwMutexUnLock();
        return WC_HW_E;
    }
    wolfSSL_CryptHwMutexUnLock();

#if defined(WOLFSSL_MAXQ108X)
    wc_InitDecodedCert(&decoded, p_dest_buff, *p_len, NULL);
    wc_ParseCert(&decoded, CERT_TYPE, NO_VERIFY, NULL);
    pk_offset = decoded.publicKeyIndex;
    if (decoded.keyOID == ECDSAk ) {
        keyparam = maxq_curve_wolfssl_id2mxq_id(decoded.pkCurveOID,
                       (unsigned int *)&device_key_len);
        if (keyparam == MXQ_UNKNOWN_CURVE) {
            WOLFSSL_MSG("MAXQ: key curve not supported");
            return NOT_COMPILED_IN;
        }
    }
    else if (decoded.keyOID == RSAk) {
        pk_offset = calculate_modulus_offset(decoded.source,
                                             decoded.publicKeyIndex+1);
        device_key_len = ((decoded.source[pk_offset-2] << 8) |
                          decoded.source[pk_offset-1]);
    }
#endif

    cert_size = (p_dest_buff[2] << 8) + p_dest_buff[3] + 4;
    if (*p_len < cert_size) {
        return BUFFER_E;
    }
    *p_len = cert_size;
    return 0;
}

#if defined(WOLFSSL_MAXQ10XX_TLS)
int wolfSSL_maxq10xx_load_certificate(WOLFSSL *ssl) {
    DerBuffer* maxq_der = NULL;
    int ret = 0;

    ret = AllocDer(&maxq_der, FILE_BUFFER_SIZE, CERT_TYPE, ssl->heap);
    if (ret != 0) {
        return ret;
    }

    ret = maxq10xx_read_device_cert_der(maxq_der->buffer,
                                        &maxq_der->length);
    if (ret != 0) {
        return ret;
    }

    ssl->maxq_ctx.device_cert = maxq_der;

    if (ssl->buffers.weOwnCert) {
        FreeDer(&ssl->buffers.certificate);
    }

    ssl->buffers.certificate = maxq_der;
    ssl->buffers.weOwnCert = 1;
    return WOLFSSL_SUCCESS;
}
#endif /* WOLFSSL_MAXQ10XX_TLS */

static int maxq10xx_ecc_sign(WOLFSSL* ssl,
               const unsigned char* p_in, unsigned int p_in_len,
               unsigned char* p_out, word32* p_out_len,
               const unsigned char* keyDer, unsigned int keySz, void* ctx)
{
    int rc;
    (void)ssl;
    (void)keyDer;
    (void)keySz;
    (void)ctx;

    rc = wolfSSL_CryptHwMutexLock();
    if (rc != 0) {
        return rc;
    }

    rc = maxq10xx_ecc_sign_local(DEVICE_KEY_PAIR_OBJ_ID, (byte *)p_in, p_in_len,
                                 p_out, p_out_len, device_key_len);

    wolfSSL_CryptHwMutexUnLock();
    if (rc) {
        WOLFSSL_ERROR_MSG("MAXQ: maxq10xx_ecc_sign() failed");
        return rc;
    }

    return 0;
}
#endif /* WOLFSSL_MAXQ10XX_TLS */

int maxq10xx_port_init(void)
{
    int ret = 0;
    mxq_err_t mxq_rc;

    #ifdef WOLF_CRYPTO_CB
    ret = wc_CryptoCb_RegisterDevice(0, wolfSSL_Soft_CryptoDevCb, NULL);
    if (ret != 0) {
        WOLFSSL_ERROR_MSG("MAXQ: wolfSSL_Soft_CryptoDevCb, "
                    "wc_CryptoCb_RegisterDevice() failed");
        return ret;
    }
    #endif

    ret = maxq_CryptHwMutexTryLock();
    if (ret) {
        WOLFSSL_ERROR_MSG("MAXQ: maxq10xx_port_init() -> device is busy "
                    "(switching to soft mode)");
        return 0;
    }

    mxq_rc = MXQ_Module_Init();
    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_Module_Init() failed");
        ret = WC_HW_E;
    }

    wolfSSL_CryptHwMutexUnLock();

    #if defined(WOLF_CRYPTO_CB)
    if (ret == 0) {
        ret = wc_CryptoCb_RegisterDevice(MAXQ_DEVICE_ID,
                                         wolfSSL_MAXQ10XX_CryptoDevCb, NULL);
        if (ret != 0) {
            WOLFSSL_ERROR_MSG("MAXQ: wolfSSL_MAXQ10XX_CryptoDevCb, "
                        "wc_CryptoCb_RegisterDevice() failed");
        }
    }
    #endif

    return ret;
}

/* This basically does all the work that SendClientKeyExchange() does in the
 * case of ECDSA. That's because if a callback is defined, almost nothing is
 * done in SendClientKeyExchange(). */
static int maxq10xx_tls12_ecc_shared_secret(WOLFSSL* ssl, ecc_key* otherKey,
               unsigned char* pubKeyDer, word32* pubKeySz,
               unsigned char* out, word32* outlen,
               int side, void* ctx)
{
    int rc;
    mxq_err_t mxq_rc;
    ecc_key *p_key = NULL;
    mxq_length key_len_param;
    mxq_u1* server_public_key_param;
    mxq_u2 csid_param = ssl->options.cipherSuite |
                        (ssl->options.cipherSuite0 << 8);
    byte result_public_key[1 + (2 * ECC256_KEYSIZE)];
    (void)ctx;
    (void)otherKey;
    (void)out;
    (void)outlen;
    (void)side;

    WOLFSSL_ENTER("maxq10xx_ecc_shared_secret");

    if (ssl->specs.kea != ecc_diffie_hellman_kea) {
        WOLFSSL_MSG("MAXQ: key exchange algo not supported");
        return NOT_COMPILED_IN;
    }

    if (ssl->ecdhCurveOID != ECC_SECP256R1_OID) {
        WOLFSSL_MSG("MAXQ: key curve not supported");
        return NOT_COMPILED_IN;
    }

    ssl->hsType = DYNAMIC_TYPE_ECC;
    rc = AllocKey(ssl, ssl->hsType, &ssl->hsKey);
    if (rc != 0) {
        return rc;
    }

    p_key = (ecc_key*)ssl->hsKey;

    rc = wolfSSL_CryptHwMutexLock();
    if (rc != 0) {
        return rc;
    }

    XMEMSET(result_public_key, 0, sizeof(result_public_key));

    server_public_key_param = NULL;
    key_len_param = sizeof(result_public_key);

    mxq_rc = MXQ_Ecdh_Compute_Shared(MXQ_KEYPARAM_EC_P256R1,
                                     server_public_key_param, result_public_key,
                                     key_len_param, csid_param);
    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_Ecdh_Compute_Shared() failed");
        wolfSSL_CryptHwMutexUnLock();
        return WC_HW_E;
    }

    wolfSSL_CryptHwMutexUnLock();

    /* client public key */
    p_key->state = 0;

    rc = wc_ecc_set_curve(p_key, ECC256_KEYSIZE, ECC_SECP256R1);
    if (rc != 0) {
        WOLFSSL_ERROR_MSG("MAXQ: wc_ecc_set_curve() failed");
        return rc;
    }

    p_key->flags = WC_ECC_FLAG_NONE;
    p_key->type = ECC_PUBLICKEY;

    rc = mp_read_unsigned_bin(p_key->pubkey.x, &result_public_key[1],
                              ECC256_KEYSIZE);
    if (rc != 0) {
        WOLFSSL_ERROR_MSG("MAXQ: mp_read_unsigned_bin() failed");
        return rc;
    }

    rc = mp_read_unsigned_bin(p_key->pubkey.y,
                              &result_public_key[1 + ECC256_KEYSIZE],
                              ECC256_KEYSIZE);
    if (rc != 0) {
        WOLFSSL_ERROR_MSG("MAXQ: mp_read_unsigned_bin() failed");
        return rc;
    }

    p_key->maxq_ctx.hw_storage = 1;

    PRIVATE_KEY_UNLOCK();
    rc = wc_ecc_export_x963(p_key, pubKeyDer, pubKeySz);
    PRIVATE_KEY_LOCK();

    return rc;
}

#ifdef WOLFSSL_MAXQ108X
static int wc_MAXQ10XX_HmacSetKey(int type)
{
    mxq_algo_id_t algo;
    int rc;
    mxq_err_t mxq_rc;

    if (!tls13active) {
        return NOT_COMPILED_IN;
    }

    if (type == WC_SHA256) {
        algo = ALGO_HMAC_SHA256;
    }
    else if (type == WC_SHA384) {
        algo = ALGO_HMAC_SHA384;
    }
    else {
        return NOT_COMPILED_IN;
    }

    if (tls13_server_finish_obj_id != -1) {
        free_temp_key_id(*tls13_server_key_id);
        *tls13_server_key_id = -1;
        mac_key_obj_id = &tls13_server_finish_obj_id;
    }
    else if (tls13_client_finish_obj_id != -1) {
        mac_key_obj_id = &tls13_client_finish_obj_id;
    }

    if (mac_key_obj_id == NULL) {
        WOLFSSL_MSG("MAXQ: wc_MAXQ10XX_HmacSetKey No MAC Key is set");
        return NOT_COMPILED_IN;
    }

    rc = wolfSSL_CryptHwMutexLock();
    if (rc != 0) {
        return rc;
    }

    mxq_rc = MXQ_MAC_Init(0x02, algo, *mac_key_obj_id, NULL, 0);
    wolfSSL_CryptHwMutexUnLock();

    if (mxq_rc == 0) {
        mac_comp_active = 1;
    }
    else {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_MAC_Init() failed");
        rc = WC_HW_E;
    }

    return rc;
}

static int wc_MAXQ10XX_HmacUpdate(const byte* msg, word32 length)
{
    int rc;
    mxq_err_t mxq_rc;
    if (!tls13active || !mac_comp_active) {
        return NOT_COMPILED_IN;
    }

    rc = wolfSSL_CryptHwMutexLock();
    if (rc != 0) {
        return rc;
    }

    mxq_rc = MXQ_MAC_Update((unsigned char *)msg, length);
    wolfSSL_CryptHwMutexUnLock();

    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_MAC_Update() failed");
        rc = WC_HW_E;
    }

    return rc;
}

static int wc_MAXQ10XX_HmacFinal(byte* hash)
{
    int rc;
    mxq_err_t mxq_rc;
    mxq_length maclen = 64;
    if (!tls13active || !mac_comp_active) {
        return NOT_COMPILED_IN;
    }

    rc = wolfSSL_CryptHwMutexLock();
    if (rc != 0) {
        return rc;
    }

    mxq_rc = MXQ_MAC_Finish(hash, &maclen);
    wolfSSL_CryptHwMutexUnLock();
    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_MAC_Finish() failed");
        rc = WC_HW_E;
    }

    free_temp_key_id(*mac_key_obj_id);
    *mac_key_obj_id = -1;
    mac_key_obj_id = NULL;
    mac_comp_active = 0;

    return rc;
}

static int maxq10xx_create_dh_key(byte* p, word32 pSz, byte* g, word32 gSz,
                                  byte* pub, word32* pubSz)
{
    int rc;
    mxq_err_t mxq_rc;

    WOLFSSL_ENTER("maxq10xx_create_dh_key");
    if (!tls13active) {
        return NOT_COMPILED_IN;
    }

    *pubSz = pSz;
    if (tls13_dh_obj_id == -1) {
        tls13_dh_obj_id = alloc_temp_key_id();
        if (tls13_dh_obj_id == -1) {
            WOLFSSL_MSG("MAXQ: alloc_temp_key_id() failed");
            rc = NOT_COMPILED_IN;
            return rc;
        }
    }

    rc = wolfSSL_CryptHwMutexLock();
    if (rc != 0) {
        return rc;
    }

    mxq_rc = MXQ_TLS13_Generate_Key(pub, tls13_dh_obj_id, 0, MXQ_KEYPARAM_DHE,
                                    pSz, p, gSz, g);

    wolfSSL_CryptHwMutexUnLock();
    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_TLS13_Generate_Key() failed");
        rc = WC_HW_E;
    }

    return rc;
}

static int maxq10xx_dh_gen_key_pair(DhKey* key, WC_RNG* rng,
                                    byte* priv, word32* privSz,
                                    byte* pub, word32* pubSz) {
    word32 p_size, g_size;
    unsigned char pbuf[256], gbuf[4];
    (void)rng;
    (void)priv;
    (void)privSz;

    p_size = mp_unsigned_bin_size(&key->p);
    mp_to_unsigned_bin(&key->p, pbuf);

    g_size = mp_unsigned_bin_size(&key->g);
    mp_to_unsigned_bin(&key->g, gbuf);

    return maxq10xx_create_dh_key(pbuf, p_size, gbuf, g_size, pub, pubSz);
}

static int maxq10xx_dh_agree(WOLFSSL* ssl, struct DhKey* key,
               const unsigned char* priv, unsigned int privSz,
               const unsigned char* pubKeyDer, unsigned int pubKeySz,
               unsigned char* out, unsigned int* outlen,
               void* ctx)
{
    int rc;
    mxq_err_t mxq_rc;
    (void)ctx;
    (void)key;
    (void)priv;
    (void)privSz;

    WOLFSSL_ENTER("maxq10xx_dh_agree");

    mxq_u2 csid_param = ssl->options.cipherSuite |
                        (ssl->options.cipherSuite0 << 8);

    if (tls13_dh_obj_id == -1) {
        WOLFSSL_ERROR_MSG("MAXQ: DH key is not created before");
        rc = NOT_COMPILED_IN;
        return rc;
    }

    if (tls13_shared_secret_obj_id == -1) {
        tls13_shared_secret_obj_id = alloc_temp_key_id();
        if (tls13_shared_secret_obj_id == -1) {
            WOLFSSL_ERROR_MSG("MAXQ: alloc_temp_key_id() failed");
            return NOT_COMPILED_IN;
        }
    }

    rc = wolfSSL_CryptHwMutexLock();
    if (rc != 0) {
        return rc;
    }

    mxq_rc = MXQ_TLS13_Create_Secret((unsigned char*)pubKeyDer, pubKeySz,
                                     tls13_dh_obj_id, 0, MXQ_KEYPARAM_DHE,
                                     csid_param, tls13_shared_secret_obj_id,
                                     out, outlen);
    wolfSSL_CryptHwMutexUnLock();
    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: wolfSSL_CryptHwMutexUnLock() failed");
        rc = WC_HW_E;
    }

    *outlen = pubKeySz;
    free_temp_key_id(tls13_dh_obj_id);
    tls13_dh_obj_id = -1;
    free_temp_key_id(tls13_ecc_obj_id);
    tls13_ecc_obj_id = -1;

    return rc;
}

static int  maxq10xx_ecc_key_gen(WOLFSSL* ssl, ecc_key* key, word32 keySz,
                int ecc_curve, void* ctx)
{
    int rc;
    mxq_err_t mxq_rc;
    unsigned char mxq_key[MAX_EC_KEY_SIZE];
    (void)ctx;
    (void)ssl;

    WOLFSSL_ENTER("maxq10xx_ecc_key_gen");

    if (tls13_ecc_obj_id == -1) {
        tls13_ecc_obj_id = alloc_temp_key_id();
        if (tls13_ecc_obj_id == -1) {
            WOLFSSL_ERROR_MSG("MAXQ: alloc_temp_key_id() failed");
            rc = NOT_COMPILED_IN;
            return rc;
        }
    }

    rc = wolfSSL_CryptHwMutexLock();
    if (rc != 0) {
        return rc;
    }
    mxq_rc = MXQ_TLS13_Generate_Key(mxq_key, tls13_ecc_obj_id, MXQ_KEYTYPE_ECC,
                                    getMaxqKeyParamFromCurve(key->dp->id),
                                    keySz, NULL, 0, NULL);

    wolfSSL_CryptHwMutexUnLock();
    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_TLS13_Generate_Key() failed");
        return WC_HW_E;
    }

    rc = wc_ecc_import_unsigned(key, (byte*)mxq_key, (byte*)mxq_key + keySz,
                                NULL, ecc_curve);
    if (rc) {
        WOLFSSL_ERROR_MSG("MAXQ: wc_ecc_import_raw_ex() failed");
    }

    return rc;
}

static int maxq10xx_ecc_verify(WOLFSSL* ssl, const byte* sig,
                               word32 sigSz, const byte* hash,
                               word32 hashSz, const byte* key,
                               word32 keySz, int* result, void* ctx)
{
    int rc;
    (void)ssl;
    (void)key;
    (void)keySz;
    (void)ctx;

    WOLFSSL_ENTER("maxq10xx_ecc_verify");

    if (!tls13active) {
        return CRYPTOCB_UNAVAILABLE;
    }

    if (tls13_server_key_algo != ECDSAk) {
        return CRYPTOCB_UNAVAILABLE;
    }

    rc = wolfSSL_CryptHwMutexLock();
    if (rc != 0) {
        return rc;
    }

    rc = maxq10xx_ecc_verify_local(tls13_server_cert_id, (mxq_u1*)hash, hashSz,
                                   (mxq_u1*)sig, sigSz,
                                   result, tls13_server_key_len);
    wolfSSL_CryptHwMutexUnLock();

    return rc;
}

static int maxq10xx_tls13_ecc_shared_secret(WOLFSSL* ssl, ecc_key* otherKey,
               unsigned char* pubKeyDer, word32* pubKeySz,
               unsigned char* out, word32* outlen,
               int side, void* ctx)
{
    int rc;
    mxq_err_t mxq_rc;
    word32 peerKeySz = otherKey->dp->size;
    uint8_t  peerKeyBuf[MAX_EC_KEY_SIZE];
    uint8_t* peerKey = peerKeyBuf;
    uint8_t* qx = peerKey;
    uint8_t* qy = &peerKey[peerKeySz];
    word32 qxLen = peerKeySz,  qyLen = peerKeySz;
    mxq_u2 csid_param = ssl->options.cipherSuite |
                        (ssl->options.cipherSuite0 << 8);
    (void)ctx;
    (void)pubKeyDer;
    (void)side;
    (void)pubKeySz;

    WOLFSSL_ENTER("maxq10xx_ecc_shared_secret");

    rc = wc_ecc_export_public_raw(otherKey, qx, &qxLen, qy, &qyLen);

    if (tls13_ecc_obj_id == -1) {
        WOLFSSL_ERROR_MSG("MAXQ: ECDHE key is not created before");
        rc = NOT_COMPILED_IN;
        return rc;
    }

    if (tls13_shared_secret_obj_id == -1) {
        tls13_shared_secret_obj_id = alloc_temp_key_id();
        if (tls13_shared_secret_obj_id == -1) {
            WOLFSSL_ERROR_MSG("MAXQ: alloc_temp_key_id() failed");
            return NOT_COMPILED_IN;
        }
    }

    rc = wolfSSL_CryptHwMutexLock();
    if (rc != 0) {
        return rc;
    }

    mxq_rc = MXQ_TLS13_Create_Secret(peerKey, (2*peerKeySz), tls13_ecc_obj_id,
                                     MXQ_KEYTYPE_ECC,
                                     getMaxqKeyParamFromCurve(otherKey->dp->id),
                                     csid_param, tls13_shared_secret_obj_id,
                                     out, outlen);

    wolfSSL_CryptHwMutexUnLock();
    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_TLS13_Create_Secret() failed");
        rc = WC_HW_E;
    }

    *outlen = otherKey->dp->size;
    free_temp_key_id(tls13_dh_obj_id);
    tls13_dh_obj_id = -1;
    free_temp_key_id(tls13_ecc_obj_id);
    tls13_ecc_obj_id = -1;

    return rc;
}

static int maxq10xx_rsa_pss_verify_ex(WOLFSSL* ssl,
                                      byte* hashed_msg, word32 hashed_msg_sz,
                                      byte* pss_sign, word32 pss_signlen,
                                      int isCertId)
{
    mxq_u2 pubkey_objectid;
    int ret;
    mxq_err_t mxq_rc;
    (void)ssl;

    WOLFSSL_ENTER("maxq10xx_rsa_pss_verify_ex");

    if (!tls13active) {
        return NOT_COMPILED_IN;
    }

    if (isCertId) {
        pubkey_objectid = tls13_server_cert_id;
    }
    else {
        pubkey_objectid = DEVICE_KEY_PAIR_OBJ_ID;
    }

    ret = wolfSSL_CryptHwMutexLock();
    if (ret != 0) {
        return ret;
    }
    mxq_rc = MXQ_Verify(ALGO_RSASSAPSSPKCS1_V2_1_PLAIN, pubkey_objectid,
                        hashed_msg, hashed_msg_sz, pss_sign, pss_signlen);

    wolfSSL_CryptHwMutexUnLock();

    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_Verify() failed");
        ret = WC_HW_E;
    }
    return ret;
}

/* This will do all the work that is normally done in RsaVerify() and
 * CheckRSASignature(). That is why at the bottom, we release the key. Because
 * verification has been completed and CheckRSASignature() should be skipped.
 * The same reasoning for maxq10xx_RsaSkipSignCheck() doing nothing. */
static int maxq10xx_rsa_pss_verify(WOLFSSL* ssl,
                                   unsigned char* in, unsigned int inSz,
                                   unsigned char** out, int hash, int mgf,
                                   const unsigned char* key, unsigned int keySz,
                                   void* ctx) {
    int    ret = 0;
    byte   sigData[MAX_SIG_DATA_SZ];
    word16 sigDataSz;
    word32 sigSz;
    (void)out;
    (void)mgf;
    (void)key;
    (void)keySz;
    (void)ctx;

    if (hash == SHA256h) {
        hash = sha256_mac;
    }
    else if (hash == SHA384h) {
        hash = sha384_mac;
    }
    else if (hash  == SHA512h) {
        hash = sha512_mac;
    }
    else {
        return BAD_FUNC_ARG;
    }

    ret = CreateSigData(ssl, sigData, &sigDataSz, 1);
    if (ret != 0)
        return ret;

    /* PSS signature can be done in-place */
    ret = CreateRSAEncodedSig(sigData, sigData, sigDataSz,
                              rsa_pss_sa_algo, hash);
    if (ret < 0)
        return ret;
    sigSz = ret;

    ret = maxq10xx_rsa_pss_verify_ex(ssl, sigData, sigSz, in, inSz, 1);

    if (ret == 0) {
        /* This ensures call to CheckRSASignature() is skipped. */
        ssl->peerRsaKeyPresent = 0;
        FreeKey(ssl, DYNAMIC_TYPE_RSA, (void**)&ssl->peerRsaKey);
        ssl->options.peerAuthGood = 1;
    }

    return ret;
}

static int maxq10xx_skip_sign_check(WOLFSSL* ssl,
               unsigned char* sig, unsigned int sigSz,
               unsigned char** out, int hash, int mgf,
               const unsigned char* key, unsigned int keySz,
               void* ctx) {
    (void)ssl;
    (void)sig;
    (void)sigSz;
    (void)out;
    (void)hash;
    (void)mgf;
    (void)key;
    (void)keySz;
    (void)ctx;
    return 0;
}

static int maxq10xx_rsa_pss_sign(WOLFSSL* ssl, const byte* in, word32 inSz,
                                 byte* out, word32* outSz, int hash, int mgf,
                                 const byte* key, word32 keySz, void* ctx)
{
    int ret;
    mxq_err_t mxq_rc;
    (void)ssl;
    (void)hash;
    (void)mgf;
    (void)key;
    (void)keySz;
    (void)ctx;

    WOLFSSL_ENTER("maxq10xx_rsa_pss_sign");

    if (!tls13active) {
        return NOT_COMPILED_IN;
    }

    ret = wolfSSL_CryptHwMutexLock();
    if (ret != 0) {
        return ret;
    }

    mxq_rc = MXQ_Sign(ALGO_RSASSAPSSPKCS1_V2_1_PLAIN, DEVICE_KEY_PAIR_OBJ_ID,
                      in, inSz, out, outSz);

    wolfSSL_CryptHwMutexUnLock();

    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_Sign() failed");
        return WC_HW_E;
    }

    ret = wolfSSL_CryptHwMutexLock();
    if (ret != 0) {
        return ret;
    }

    mxq_rc = MXQ_Verify(ALGO_RSASSAPSSPKCS1_V2_1_PLAIN, DEVICE_KEY_PAIR_OBJ_ID,
                        in, inSz, out, *outSz);

    wolfSSL_CryptHwMutexUnLock();
    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_Verify() failed");
        return WC_HW_E;
    }

    return ret;
}

#ifdef HAVE_HKDF
static int maxq10xx_hkdf_extract(byte* prk, const byte* salt, word32 saltLen,
               byte* ikm, word32 ikmLen, int digest, void* ctx)
{
    int rc;
    mxq_err_t mxq_rc;
    mxq_length prk_len = 0;
    mxq_algo_id_t mxq_digest_id = ALGO_INVALID;
    int salt_kid = -1, ikm_kid = -1, ret_kid = -1;

    (void)ctx;

    switch (digest) {
        #ifndef NO_SHA256
        case WC_SHA256:
            prk_len = WC_SHA256_DIGEST_SIZE;
            mxq_digest_id = ALGO_MD_SHA256;
            break;
        #endif

        #ifdef WOLFSSL_SHA384
        case WC_SHA384:
            prk_len = WC_SHA384_DIGEST_SIZE;
            mxq_digest_id = ALGO_MD_SHA384;
            break;
        #endif

        #ifdef WOLFSSL_TLS13_SHA512
        case WC_SHA512:
            prk_len = WC_SHA512_DIGEST_SIZE;
            mxq_digest_id = ALGO_MD_SHA512;
            break;
        #endif
        default:
            return BAD_FUNC_ARG;
    }

    /* Prepare key id parameters */
    if (saltLen != 0 && ikmLen != 0) {
        /* handshake_secret = HKDF-Extract(salt: derived_secret,
         *                        key: shared_secret) */
        if (tls13_handshake_secret_obj_id == -1) {
            tls13_handshake_secret_obj_id = alloc_temp_key_id();
            if (tls13_handshake_secret_obj_id == -1) {
                WOLFSSL_ERROR_MSG("MAXQ: alloc_temp_key_id() failed");
                return NOT_COMPILED_IN;
            }
        }
        salt_kid = tls13_derived_secret_obj_id;
        ikm_kid  = tls13_shared_secret_obj_id;
        ret_kid  = tls13_handshake_secret_obj_id;
        free_temp_key_id(tls13_derived_secret_obj_id);
        tls13_derived_secret_obj_id = -1;
    }
    else if (saltLen != 0 && ikmLen == 0) {
        /* master_secret = HKDF-Extract(salt: derived_secret,
         *                     key: 00...) */
        if (tls13_master_secret_obj_id == -1) {
            tls13_master_secret_obj_id = alloc_temp_key_id();
            if (tls13_master_secret_obj_id == -1) {
                WOLFSSL_ERROR_MSG("MAXQ: alloc_temp_key_id() failed");
                return NOT_COMPILED_IN;
            }
        }

        salt_kid = tls13_derived_secret_obj_id;
        ikm_kid  = -1;
        ret_kid  = tls13_master_secret_obj_id;
        free_temp_key_id(tls13_derived_secret_obj_id);
        tls13_derived_secret_obj_id = -1;
    }
    else if (saltLen == 0 && ikmLen == 0) {
        /* early_secret = HKDF-Extract(salt: 00, key: 00...) */
        if (tls13_early_secret_obj_id == -1) {
            tls13_early_secret_obj_id = alloc_temp_key_id();
            if (tls13_early_secret_obj_id == -1) {
                WOLFSSL_ERROR_MSG("MAXQ: alloc_temp_key_id() failed");
                return NOT_COMPILED_IN;
            }
        }

        salt_kid = -1;
        ikm_kid  = -1;
        ret_kid  = tls13_early_secret_obj_id;

        tls13early = 1;
    }
    else if (saltLen == 0 && ikmLen != 0) {
        /* early_secret = HKDF-Extract(salt: 00, key: 00...) */
        if (tls13_hs_early_secret_obj_id == -1) {
            tls13_hs_early_secret_obj_id = alloc_temp_key_id();
            if (tls13_hs_early_secret_obj_id == -1) {
                WOLFSSL_ERROR_MSG("MAXQ: alloc_temp_key_id() failed");
                return NOT_COMPILED_IN;
            }
        }

        salt_kid = -1;
        ikm_kid  = PSK_KID;
        ret_kid  = tls13_hs_early_secret_obj_id;

        tls13early = 1;
    }
    else {
        WOLFSSL_MSG("MAXQ: MXQ_TLS13_Extract_Secret() does not support");
        return NOT_COMPILED_IN;
    }

    /* When length is 0 then use zeroed data of digest length. */
    if (ikmLen == 0) {
        ikmLen = prk_len;
        XMEMSET(ikm, 0, prk_len);
    }

    if (salt_kid != -1) {
        saltLen = 0;
    }

    if (ikm_kid != -1) {
        ikmLen = 0;
    }

    if (ret_kid != -1) {
        XMEMSET(prk, 0, prk_len);
        prk_len = 0;
    }

    rc = wolfSSL_CryptHwMutexLock();
    if (rc != 0) {
        return rc;
    }

    mxq_rc = MXQ_TLS13_Extract_Secret(mxq_digest_id, (mxq_u2)ret_kid,
                                      prk, &prk_len, (mxq_u2)salt_kid,
                                      salt, (mxq_u2)saltLen,
                                      (mxq_u2)ikm_kid, ikm, (mxq_u2)ikmLen);
    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_TLS13_Extract_Secret() failed");
        wolfSSL_CryptHwMutexUnLock();
        return WC_HW_E;
    }

    wolfSSL_CryptHwMutexUnLock();
    return 0;
}

static char *strstr_with_size(char *str, const char *substr, size_t n)
{
    char *p = str, *pEnd = str + n;
    size_t substr_len = XSTRLEN(substr);

    if (substr_len == 0) {
        return str;
    }

    pEnd -= (substr_len - 1);

    for (;p < pEnd; ++p) {
        if (0 == XSTRNCMP(p, substr, substr_len))
            return p;
    }

    return NULL;
}

static int maxq10xx_HkdfExpand(int digest, const byte* inKey, word32 inKeySz,
                               const byte* info, word32 infoSz, byte* out,
                               word32 outSz, int side)
{
    int rc;
    mxq_err_t mxq_rc;
    mxq_algo_id_t mxq_digest_id = ALGO_INVALID;
    mxq_keytype_id_t ret_keytype = MXQ_KEYTYPE_IKM;
    int prk_kid = -1, ret_kid = -1;
    int ret_isiv = 0;

    switch (digest) {
        #ifndef NO_SHA256
        case WC_SHA256:
            mxq_digest_id = ALGO_MD_SHA256;
            break;
        #endif

        #ifdef WOLFSSL_SHA384
        case WC_SHA384:
            mxq_digest_id = ALGO_MD_SHA384;
            break;
        #endif

        #ifdef WOLFSSL_TLS13_SHA512
        case WC_SHA512:
            mxq_digest_id = ALGO_MD_SHA512;
            break;
        #endif
        default:
            return BAD_FUNC_ARG;
    }

    /* Prepare key id parameters */
    if (strstr_with_size((char *)info, derivedLabel, infoSz) != NULL) {
        if (tls13early) {
            if (local_is_psk) {
                if (tls13_hs_early_secret_obj_id == -1) {
                        WOLFSSL_ERROR_MSG("MAXQ: Handshake early secret is not "
                                    "created yet");
                        return NOT_COMPILED_IN;
                }

                tls13_derived_secret_obj_id = alloc_temp_key_id();
                if (tls13_derived_secret_obj_id == -1) {
                    WOLFSSL_ERROR_MSG("MAXQ: alloc_temp_key_id() failed");
                    return NOT_COMPILED_IN;
                }
                prk_kid = tls13_hs_early_secret_obj_id;
                ret_kid = tls13_derived_secret_obj_id;
                ret_keytype = MXQ_KEYTYPE_IKM;
                free_temp_key_id(tls13_hs_early_secret_obj_id);
                tls13_hs_early_secret_obj_id = -1;
            }
            else {
                /* derived_secret = HKDF-Expand-Label(key: early_secret,
                 *                      label: "derived", ctx: empty_hash) */
                tls13_derived_secret_obj_id = alloc_temp_key_id();
                if (tls13_derived_secret_obj_id == -1) {
                    WOLFSSL_ERROR_MSG("MAXQ: alloc_temp_key_id() failed");
                    return NOT_COMPILED_IN;
                }

                prk_kid = tls13_early_secret_obj_id;
                ret_kid = tls13_derived_secret_obj_id;
                ret_keytype = MXQ_KEYTYPE_IKM;
                free_temp_key_id(tls13_early_secret_obj_id);
                tls13_early_secret_obj_id = -1;
            }
            ret_isiv = 0;
            tls13early = 0;
        }
        else {
            /* derived_secret = HKDF-Expand-Label(key: handshake_secret,
             *                      label: "derived", ctx: empty_hash) */
            tls13_derived_secret_obj_id = alloc_temp_key_id();
            if (tls13_derived_secret_obj_id == -1) {
                WOLFSSL_ERROR_MSG("MAXQ: alloc_temp_key_id() failed");
                return NOT_COMPILED_IN;
            }

            prk_kid = tls13_handshake_secret_obj_id;
            ret_kid = tls13_derived_secret_obj_id;
            ret_keytype = MXQ_KEYTYPE_IKM;
            free_temp_key_id(tls13_handshake_secret_obj_id);
            tls13_handshake_secret_obj_id = -1;
            ret_isiv = 0;
        }
    }
    else if (strstr_with_size((char *)info, cHsTrafficLabel, infoSz)
               != NULL) {
        is_hs_key = 1;
        /* client_secret = HKDF-Expand-Label(key: handshake_secret,
         *                     label: "c hs traffic", ctx: hello_hash) */
        if (tls13_client_secret_obj_id == -1) {
            tls13_client_secret_obj_id = alloc_temp_key_id();
            if (tls13_client_secret_obj_id == -1) {
                WOLFSSL_ERROR_MSG("MAXQ: alloc_temp_key_id() failed");
                return NOT_COMPILED_IN;
            }
        }

        prk_kid = tls13_handshake_secret_obj_id;
        ret_kid = tls13_client_secret_obj_id;
        ret_keytype = MXQ_KEYTYPE_IKM;
        ret_isiv = 0;
    }
    else if (strstr_with_size((char *)info, sHsTrafficLabel, infoSz)
               != NULL) {
        /* client_secret = HKDF-Expand-Label(key: handshake_secret,
         *                     label: "s hs traffic", ctx: hello_hash) */
        if (tls13_server_secret_obj_id == -1) {
            tls13_server_secret_obj_id = alloc_temp_key_id();
            if (tls13_server_secret_obj_id == -1) {
                WOLFSSL_ERROR_MSG("MAXQ: alloc_temp_key_id() failed");
                return NOT_COMPILED_IN;
            }
        }

        prk_kid = tls13_handshake_secret_obj_id;
        ret_kid = tls13_server_secret_obj_id;
        ret_keytype = MXQ_KEYTYPE_IKM;
        ret_isiv = 0;
    }
    else if (strstr_with_size((char *)info, cAppTrafficLabel, infoSz)
               != NULL) {
        is_hs_key = 0;
        /* client_secret = HKDF-Expand-Label(key: master_secret,
         *                     label: "c ap traffic", ctx: handshake_hash) */
        if (tls13_client_secret_obj_id == -1) {
            tls13_client_secret_obj_id = alloc_temp_key_id();
            if (tls13_client_secret_obj_id == -1) {
                WOLFSSL_ERROR_MSG("MAXQ: alloc_temp_key_id() failed");
                return NOT_COMPILED_IN;
            }
        }

        prk_kid = tls13_master_secret_obj_id;
        ret_kid = tls13_client_secret_obj_id;
        ret_keytype = MXQ_KEYTYPE_IKM;
        ret_isiv = 0;
    }
    else if (strstr_with_size((char *)info, sAppTrafficLabel, infoSz)
               != NULL) {
        /* server_secret = HKDF-Expand-Label(key: master_secret,
         *                     label: "s ap traffic", ctx: handshake_hash) */
        tls13_server_secret_obj_id = alloc_temp_key_id();
        if (tls13_server_secret_obj_id == -1) {
            WOLFSSL_ERROR_MSG("MAXQ: alloc_temp_key_id() failed");
            return NOT_COMPILED_IN;
        }

        prk_kid = tls13_master_secret_obj_id;
        ret_kid = tls13_server_secret_obj_id;
        ret_keytype = MXQ_KEYTYPE_IKM;
        ret_isiv = 0;
    }
    else if (strstr_with_size((char *)info, keyLabel, infoSz) != NULL) {
        /* first client key then server */
        if (side == WOLFSSL_CLIENT_END) {
            /* client_handshake_key = HKDF-Expand-Label(key: client_secret,
             *                            label: "key", ctx: "")
             * client_application_key = HKDF-Expand-Label(key: client_secret,
             *                              label: "key", ctx: "") */
            int tls13_client_key_obj_id = -1;
            if (is_hs_key) {
                if (tls13_client_hs_key_obj_id == -1) {
                    tls13_client_hs_key_obj_id = alloc_temp_key_id();
                    if (tls13_client_hs_key_obj_id == -1) {
                        WOLFSSL_ERROR_MSG("MAXQ: alloc_temp_key_id() failed");
                        return NOT_COMPILED_IN;
                    }
                }
                tls13_client_key_obj_id = tls13_client_hs_key_obj_id;
            }
            else {
                if (tls13_client_app_key_obj_id == -1) {
                    tls13_client_app_key_obj_id = alloc_temp_key_id();
                    if (tls13_client_app_key_obj_id == -1) {
                        WOLFSSL_ERROR_MSG("MAXQ: alloc_temp_key_id() failed");
                        return NOT_COMPILED_IN;
                    }
                }
                tls13_client_key_obj_id = tls13_client_app_key_obj_id;
            }

            prk_kid = tls13_client_secret_obj_id;
            ret_kid = tls13_client_key_obj_id;
            ret_keytype = MXQ_KEYTYPE_AES;
            ret_isiv = 0;
        }
        else {
            /* server_handshake_key = HKDF-Expand-Label(key: server_secret,
             *                            label: "key", ctx: "")
             * server_application_key = HKDF-Expand-Label(key: server_secret,
                                            label: "key", ctx: "") */
            int tls13_server_key_obj_id = -1;
            if (is_hs_key) {
                if (tls13_server_hs_key_obj_id == -1) {
                    tls13_server_hs_key_obj_id = alloc_temp_key_id();
                    if (tls13_server_hs_key_obj_id == -1) {
                        WOLFSSL_ERROR_MSG("MAXQ: alloc_temp_key_id() failed");
                        return NOT_COMPILED_IN;
                    }
                }
                tls13_server_key_obj_id = tls13_server_hs_key_obj_id;
            }
            else {
                if (tls13_server_app_key_obj_id == -1) {
                    tls13_server_app_key_obj_id = alloc_temp_key_id();
                    if (tls13_server_app_key_obj_id == -1) {
                        WOLFSSL_ERROR_MSG("MAXQ: alloc_temp_key_id() failed");
                        return NOT_COMPILED_IN;
                    }
                }
                tls13_server_key_obj_id = tls13_server_app_key_obj_id;
            }

            prk_kid = tls13_server_secret_obj_id;
            ret_kid = tls13_server_key_obj_id;
            ret_keytype = MXQ_KEYTYPE_AES;
            ret_isiv = 0;
        }
    }
    else if (strstr_with_size((char *)info, ivLabel, infoSz) != NULL) {
        /* first client key then server */
        if (side == WOLFSSL_CLIENT_END) {
            /* client_handshake_iv = HKDF-Expand-Label(key: client_secret,
             *                           label: "iv", ctx: "")
             * cient_application_iv = HKDF-Expand-Label(key: client_secret,
             *                            label: "iv", ctx: "") */
            int tls13_client_iv_obj_id = -1;
            if (is_hs_key) {
                if (tls13_client_hs_key_obj_id == -1) {
                    WOLFSSL_ERROR_MSG("MAXQ: alloc_temp_key_id() failed");
                    return NOT_COMPILED_IN;
                }
                tls13_client_iv_obj_id = tls13_client_hs_key_obj_id;
            }
            else {
                if (tls13_client_app_key_obj_id == -1) {
                    WOLFSSL_ERROR_MSG("MAXQ: alloc_temp_key_id() failed");
                    return NOT_COMPILED_IN;
                }
                tls13_client_iv_obj_id = tls13_client_app_key_obj_id;
            }

            prk_kid = tls13_client_secret_obj_id;
            ret_kid = tls13_client_iv_obj_id;
            ret_keytype = MXQ_KEYTYPE_AES;
            ret_isiv = 1;
        }
        else {
            /* server_handshake_iv = HKDF-Expand-Label(key: server_secret,
             *                           label: "iv", ctx: "")
             * server_application_iv = HKDF-Expand-Label(key: server_secret,
             * label: "iv", ctx: "") */
            int tls13_server_iv_obj_id = -1;
            if (is_hs_key) {
                if (tls13_server_hs_key_obj_id == -1) {
                    WOLFSSL_ERROR_MSG("MAXQ: alloc_temp_key_id() failed");
                    return NOT_COMPILED_IN;
                }
                tls13_server_iv_obj_id = tls13_server_hs_key_obj_id;
            }
            else {
                if (tls13_server_app_key_obj_id == -1) {
                    WOLFSSL_ERROR_MSG("MAXQ: alloc_temp_key_id() failed");
                    return NOT_COMPILED_IN;
                }
                tls13_server_iv_obj_id = tls13_server_app_key_obj_id;
            }

            prk_kid = tls13_server_secret_obj_id;
            ret_kid = tls13_server_iv_obj_id;
            ret_keytype = MXQ_KEYTYPE_AES;
            ret_isiv = 1;
            local_is_psk = 0;
        }
    }
    else if (strstr_with_size((char *)info, finishedLabel, infoSz) != NULL) {
        if (local_is_psk) {
            if (tls13_client_finish_obj_id == -1) {
                tls13_client_finish_obj_id = alloc_temp_key_id();
                if (tls13_client_finish_obj_id == -1) {
                    WOLFSSL_ERROR_MSG("MAXQ: alloc_temp_key_id() failed");
                    return NOT_COMPILED_IN;
                }
            }
            if (tls13_binder_key_obj_id == -1) {
                WOLFSSL_ERROR_MSG("MAXQ: Binder key is not created yet");
                return NOT_COMPILED_IN;
            }
            prk_kid = tls13_binder_key_obj_id;
            ret_kid = tls13_client_finish_obj_id;
            tls13_client_key_id = &tls13_binder_key_obj_id;
            ret_keytype = MXQ_KEYTYPE_HMAC;
            ret_isiv = 0;
        }
        else {
            /* first client key then server */
            if (side == WOLFSSL_CLIENT_END) {
                /* finished_key = HKDF-Expand-Label(key: client_secret,
                 * label: "finished", ctx: "") */
                if (is_hs_key) {
                    tls13_client_key_id = &tls13_client_hs_key_obj_id;
                }
                else {
                    tls13_client_key_id = &tls13_client_app_key_obj_id;
                }
                if (tls13_client_finish_obj_id == -1) {
                    tls13_client_finish_obj_id = alloc_temp_key_id();
                    if (tls13_client_finish_obj_id == -1) {
                        WOLFSSL_ERROR_MSG("MAXQ: alloc_temp_key_id() failed");
                        return NOT_COMPILED_IN;
                    }
                }

                prk_kid = tls13_client_secret_obj_id;
                ret_kid = tls13_client_finish_obj_id;

                ret_keytype = MXQ_KEYTYPE_HMAC;
                ret_isiv = 0;
                free_temp_key_id(tls13_client_secret_obj_id);
                tls13_client_secret_obj_id = -1;

            }
            else {
                /* finished_key = HKDF-Expand-Label(key: server_secret,
                 *                    label: "finished", ctx: "") */
                if (is_hs_key) {
                    tls13_server_key_id = &tls13_server_hs_key_obj_id;
                }
                else {
                    tls13_server_key_id = &tls13_server_app_key_obj_id;
                }
                if (tls13_server_finish_obj_id == -1) {
                    tls13_server_finish_obj_id = alloc_temp_key_id();
                    if (tls13_server_finish_obj_id == -1) {
                        WOLFSSL_ERROR_MSG("MAXQ: alloc_temp_key_id() failed");
                        return NOT_COMPILED_IN;
                    }
                }

                prk_kid = tls13_server_secret_obj_id;
                ret_kid = tls13_server_finish_obj_id;
                ret_keytype = MXQ_KEYTYPE_HMAC;
                ret_isiv = 0;
                free_temp_key_id(tls13_server_secret_obj_id);
                tls13_server_secret_obj_id = -1;

            }
        }
    }
    else if (strstr_with_size((char *)info, extBinderLabel, infoSz) != NULL) {

            /* binder_key = HKDF-Expand-Label(key: hs_early_secret,
             *                  label: "ext binder", ctx: empty_hash) */
            tls13_binder_key_obj_id = alloc_temp_key_id();
            if (tls13_binder_key_obj_id == -1) {
                WOLFSSL_ERROR_MSG("MAXQ: alloc_temp_key_id() failed");
                return NOT_COMPILED_IN;
            }

            prk_kid = tls13_hs_early_secret_obj_id;
            ret_kid = tls13_binder_key_obj_id;
            ret_keytype = MXQ_KEYTYPE_IKM;
            ret_isiv = 0;
            local_is_psk = 1;

    }
    else if (strstr_with_size((char *)info, resMasterLabel, infoSz) != NULL) {
        /* TODO: */
        tls13_res_master_obj_id = alloc_temp_key_id();
        if (tls13_res_master_obj_id == -1) {
            WOLFSSL_ERROR_MSG("MAXQ: alloc_temp_key_id() failed");
            return NOT_COMPILED_IN;
        }
        prk_kid = tls13_master_secret_obj_id;
        ret_kid = tls13_res_master_obj_id;
        ret_keytype = MXQ_KEYTYPE_IKM;
        ret_isiv = 0;
        free_temp_key_id(*tls13_client_key_id);
        *tls13_client_key_id = -1;
    }
    else if (strstr_with_size((char *)info, appTrafUpdLabel, infoSz) != NULL) {
        if (side == WOLFSSL_CLIENT_END) {
            /* updated_client_secret = HKDF-Expand-Label(key: client_secret,
             *                             label: "traffic upd", ctx: "") */
            if (tls13_client_app_key_obj_id == -1) {
                WOLFSSL_ERROR_MSG("MAXQ: Client Application Key was not set");
                return NOT_COMPILED_IN;
            }
            prk_kid = tls13_client_secret_obj_id;
            ret_kid = tls13_client_secret_obj_id;
            ret_keytype = MXQ_KEYTYPE_IKM;
            ret_isiv = 0;
        }
        else {
            /* updated_server_secret = HKDF-Expand-Label(key: server_secret,
             *                             label: "traffic upd", ctx: "") */
            if (tls13_server_app_key_obj_id == -1) {
                WOLFSSL_ERROR_MSG("MAXQ: Client Application Key was not set");
                return NOT_COMPILED_IN;
            }
            prk_kid = tls13_server_secret_obj_id;
            ret_kid = tls13_server_secret_obj_id;
            ret_keytype = MXQ_KEYTYPE_IKM;
            ret_isiv = 0;
        }
    }
    else {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_TLS13_Expand_Secret() does not support");
        return NOT_COMPILED_IN;
    }

    if (prk_kid != -1) {
        inKeySz = 0;
    }

    if (ret_kid != -1) {
        XMEMSET(out, 0, outSz);
    }

    rc = wolfSSL_CryptHwMutexLock();
    if (rc != 0) {
        return rc;
    }

    mxq_rc = MXQ_TLS13_Expand_Secret(mxq_digest_id, (mxq_u2)ret_kid,
                                     ret_keytype, ret_isiv, out, &outSz,
                                     (mxq_u2)prk_kid, inKey, inKeySz,
                                     info, infoSz );
    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_TLS13_Expand_Secret() failed");
        wolfSSL_CryptHwMutexUnLock();
        return WC_HW_E;
    }

    wolfSSL_CryptHwMutexUnLock();
    return 0;
}

static int maxq10xx_hkdf_expand_label(byte* okm, word32 okmLen,
                                      const byte* prk, word32 prkLen,
                                      const byte* protocol, word32 protocolLen,
                                      const byte* label, word32 labelLen,
                                      const byte* info, word32 infoLen,
                                      int digest, int side)
{
    int    ret = 0;
    int    idx = 0;
    byte   data[MAX_TLS13_HKDF_LABEL_SZ];

    /* Output length. */
    data[idx++] = (byte)(okmLen >> 8);
    data[idx++] = (byte)okmLen;
    /* Length of protocol | label. */
    data[idx++] = (byte)(protocolLen + labelLen);
    /* Protocol */
    XMEMCPY(&data[idx], protocol, protocolLen);
    idx += protocolLen;
    /* Label */
    XMEMCPY(&data[idx], label, labelLen);
    idx += labelLen;
    /* Length of hash of messages */
    data[idx++] = (byte)infoLen;
    /* Hash of messages */
    XMEMCPY(&data[idx], info, infoLen);
    idx += infoLen;

#ifdef WOLFSSL_CHECK_MEM_ZERO
    wc_MemZero_Add("wc_Tls13_HKDF_Expand_Label data", data, idx);
#endif

    ret = maxq10xx_HkdfExpand(digest, prk, prkLen, data, idx, okm, okmLen,
                              side);
    ForceZero(data, idx);

#ifdef WOLFSSL_CHECK_MEM_ZERO
    wc_MemZero_Check(data, MAX_TLS13_HKDF_LABEL_SZ);
#endif

    return ret;
}

static int maxq10xx_perform_tls13_record_processing(WOLFSSL* ssl,
               int is_encrypt, byte* out, const byte* in,
               word32 sz, const byte* iv, word32 ivSz,
               byte* authTag, word32 authTagSz,
               const byte* authIn, word32 authInSz)
{
    int rc;
    mxq_err_t mxq_rc;
    mxq_u2 key_id;

    if (!tls13active) {
        return NOT_COMPILED_IN;
    }

    if ((ssl->specs.bulk_cipher_algorithm != wolfssl_aes_gcm) &&
        (ssl->specs.bulk_cipher_algorithm != wolfssl_aes_ccm)) {
        WOLFSSL_MSG("MAXQ: tls record cipher algo not supported");
        return NOT_COMPILED_IN;
    }

    if (is_encrypt) {
        if (tls13_client_hs_key_obj_id != -1) {
            key_id = tls13_client_hs_key_obj_id;
        }
        else if (tls13_client_app_key_obj_id != -1) {
            key_id = tls13_client_app_key_obj_id;
        }
        else {
            WOLFSSL_ERROR_MSG("MAXQ: tls record enc key was not selected");
        }
    }
    else {
        if (tls13_server_hs_key_obj_id != -1) {
            key_id = tls13_server_hs_key_obj_id;
        }
        else if (tls13_server_app_key_obj_id != -1) {
            key_id = tls13_server_app_key_obj_id;
        }
        else {
            WOLFSSL_ERROR_MSG("MAXQ: tls record dec key was not selected");
        }
    }
    mxq_algo_id_t algo_id = 0;

    if (ssl->specs.bulk_cipher_algorithm == wolfssl_aes_gcm) {
        algo_id = ALGO_CIPHER_AES_GCM;
    }
    else if (ssl->specs.bulk_cipher_algorithm == wolfssl_aes_ccm) {
        algo_id = ALGO_CIPHER_AES_CCM;
    }

    rc = wolfSSL_CryptHwMutexLock();
    if (rc != 0) {
        return rc;
    }

    WOLFSSL_MSG("MAXQ: MXQ_TLS13_Update_IV");
    mxq_rc = MXQ_TLS13_Update_IV( key_id, (mxq_u1 *)iv, ivSz);
    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: MXQ_TLS13_Update_IV() failed");
        wolfSSL_CryptHwMutexUnLock();
        return WC_HW_E;
    }

    mxq_rc = maxq10xx_cipher_do(algo_id, is_encrypt, key_id, (mxq_u1 *)in, out,
                                sz, (mxq_u1 *)iv, ivSz, (mxq_u1 *)authIn,
                                authInSz, authTag, authTagSz);
    if (mxq_rc) {
        WOLFSSL_ERROR_MSG("MAXQ: maxq10xx_cipher_do() failed");
        wolfSSL_CryptHwMutexUnLock();
        return WC_HW_E;
    }

    wolfSSL_CryptHwMutexUnLock();
    return 0;
}
#endif /* HAVE_HKDF */
#endif /* WOLFSSL_MAXQ108X */

void maxq10xx_SetupPkCallbacks(struct WOLFSSL_CTX* ctx, ProtocolVersion *pv)
{
    (void)pv;

    if (pv == NULL) {
        return;
    }

    WOLFSSL_ENTER("maxq10xx_SetupPkCallbacks");
    if (init_pk_callbacks) {
        return;
    }

#ifdef WOLFSSL_MAXQ108X
    #ifdef HAVE_HKDF
    wolfSSL_CTX_SetHKDFExtractCb(ctx, maxq10xx_hkdf_extract);
    wolfSSL_CTX_SetHKDFExpandLabelCb(ctx, maxq10xx_hkdf_expand_label);
    #endif

    if ((pv->major == SSLv3_MAJOR) && (pv->minor == WOLFSSL_TLSV1_3)) {
        tls13active = 1;
        wolfSSL_CTX_SetEccKeyGenCb(ctx, maxq10xx_ecc_key_gen);
        wolfSSL_CTX_SetEccSharedSecretCb(ctx, maxq10xx_tls13_ecc_shared_secret);
        wolfSSL_CTX_SetEccVerifyCb(ctx, maxq10xx_ecc_verify);
        wolfSSL_CTX_SetDhGenerateKeyPair(ctx, maxq10xx_dh_gen_key_pair);
        wolfSSL_CTX_SetDhAgreeCb(ctx, maxq10xx_dh_agree);
        wolfSSL_CTX_SetRsaPssSignCb(ctx, maxq10xx_rsa_pss_sign);
        wolfSSL_CTX_SetRsaPssSignCheckCb(ctx, maxq10xx_skip_sign_check);
        wolfSSL_CTX_SetRsaPssVerifyCb(ctx, maxq10xx_rsa_pss_verify);

        wolfSSL_CTX_SetPerformTlsRecordProcessingCb(ctx,
            maxq10xx_perform_tls13_record_processing);
    }
    else
#endif /* WOLFSSL_MAXQ108X */
    {
        wolfSSL_CTX_SetEccSharedSecretCb(ctx, maxq10xx_tls12_ecc_shared_secret);
        wolfSSL_CTX_SetPerformTlsRecordProcessingCb(ctx,
            maxq10xx_perform_tls12_record_processing);
    }

    wolfSSL_CTX_SetProcessPeerCertCb(ctx, maxq10xx_process_server_certificate);
    wolfSSL_CTX_SetProcessServerSigKexCb(ctx, maxq10xx_process_server_sig_kex);
    wolfSSL_CTX_SetGenMasterSecretCb(ctx, maxq10xx_gen_tls_master_secret);
    wolfSSL_CTX_SetTlsFinishedCb(ctx, maxq10xx_perform_client_finished);

    wolfSSL_CTX_SetEccSignCb(ctx, maxq10xx_ecc_sign);

    init_pk_callbacks = 1;
}

#endif /* WOLFSSL_MAXQ1065 || WOLFSSL_MAXQ108X */
