/* se050_port.c
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

#include <stdint.h>

#include <wolfssl/wolfcrypt/settings.h>

#ifdef WOLFSSL_SE050

#include <wolfssl/wolfcrypt/types.h> /* for MATH_INT_T */
#include <wolfssl/wolfcrypt/wc_port.h>
#include <wolfssl/wolfcrypt/aes.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/ed25519.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/curve25519.h>

#include <wolfssl/wolfcrypt/port/nxp/se050_port.h>

#ifdef WOLFSSL_SE050_INIT
    #ifndef SE050_DEFAULT_PORT
    #define SE050_DEFAULT_PORT "/dev/i2c-1"
    #endif

    #include "ex_sss_boot.h"
#endif

#ifdef HAVE_ECC
    #include <wolfssl/wolfcrypt/ecc.h>
    struct ecc_key;
    #ifndef SE050_ECC_DER_MAX
    #define SE050_ECC_DER_MAX 256
    #endif
#endif
#ifndef NO_RSA
    #include <wolfssl/wolfcrypt/rsa.h>
    struct RsaKey;
#endif
#include <wolfssl/wolfcrypt/asn.h>

#ifndef SE050_KEYID_START
#define SE050_KEYID_START 100
#endif

/* enable for debugging */
/* #define SE050_DEBUG*/
/* enable to factory erase chip */
/* #define WOLFSSL_SE050_FACTORY_RESET */

/* Global variables */
static sss_session_t *cfg_se050_i2c_pi;
static sss_key_store_t *gHostKeyStore;
static sss_key_store_t *gHeyStore;

int wc_se050_set_config(sss_session_t *pSession, sss_key_store_t *pHostKeyStore,
    sss_key_store_t *pKeyStore)
{
    WOLFSSL_MSG("Setting SE050 session configuration");

    cfg_se050_i2c_pi = pSession;
    gHostKeyStore = pHostKeyStore;
    gHeyStore = pKeyStore;

    return 0;
}

#ifdef WOLFSSL_SE050_INIT
int wc_se050_init(const char* portName)
{
    int ret;
    sss_status_t status;
    static ex_sss_boot_ctx_t pCtx;

    if (portName == NULL) {
        portName = SE050_DEFAULT_PORT;
    }

    status = ex_sss_boot_open(&pCtx, portName);
    if (status == kStatus_SSS_Success) {
        ret = wc_se050_set_config(&pCtx.session,
        #if SSS_HAVE_HOSTCRYPTO_ANY
            &pCtx.host_ks,
        #else
            NULL,
        #endif
            &pCtx.ks);

    #ifdef WOLFSSL_SE050_FACTORY_RESET
        ex_sss_boot_factory_reset(&pCtx);
    #endif
    }
    else {
        WOLFSSL_MSG("Failed to open SE050 context");
        ret = WC_HW_E;
    }
    return ret;
}
#endif

/**
 * Erase and free an object stored in SE050.
 *
 * keyId  ID of object to erase
 *
 * Returns 0 on success, negative on error.
 */
int wc_se050_erase_object(word32 id)
{
    int ret = 0;
    sss_object_t    object;
    sss_key_store_t host_keystore;
    sss_status_t    status = kStatus_SSS_Success;

#ifdef SE050_DEBUG
    printf("wc_se050_erase_object: id %d\n", id);
#endif

    if (cfg_se050_i2c_pi == NULL) {
        return BAD_FUNC_ARG;
    }

    if (wolfSSL_CryptHwMutexLock() != 0) {
        return BAD_MUTEX_E;
    }

    status = sss_key_store_context_init(&host_keystore, cfg_se050_i2c_pi);
    if (status == kStatus_SSS_Success) {
        status = sss_key_store_allocate(&host_keystore,
                                        SE050_KEYSTOREID_GENERIC);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&object, &host_keystore);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_get_handle(&object, id);
    }
    if (status == kStatus_SSS_Success) {
        sss_key_store_erase_key(&host_keystore, &object);
        sss_key_object_free(&object);
    }
    wolfSSL_CryptHwMutexUnLock();

    if (status != kStatus_SSS_Success) {
        ret = WC_HW_E;
    }

    return ret;
}

word32 se050_allocate_key(int keyType)
{
    word32 keyId = 0;
    static word32 keyId_allocator = SE050_KEYID_START;
    switch (keyType) {
        case SE050_AES_KEY:
        case SE050_ECC_KEY:
        case SE050_RSA_KEY:
        case SE050_ED25519_KEY:
        case SE050_CURVE25519_KEY:
        case SE050_ANY_KEY:
            keyId = keyId_allocator++;
            break;
    }
#ifdef SE050_DEBUG
    printf("se050_allocate_key: keyId %d\n", keyId);
#endif
    return keyId;
}

#if !defined(WC_NO_RNG) && !defined(WOLFSSL_SE050_NO_TRNG)
int se050_get_random_number(uint32_t count, uint8_t* rand_out)
{
    int ret = 0;
    sss_status_t status;
    sss_rng_context_t rng;

#ifdef SE050_DEBUG
    printf("se050_get_random_number: %p (%d)\n", rand_out, count);
#endif

    if (cfg_se050_i2c_pi == NULL) {
        return WC_HW_E;
    }

    if (wolfSSL_CryptHwMutexLock() != 0) {
        return BAD_MUTEX_E;
    }
    status = sss_rng_context_init(&rng, cfg_se050_i2c_pi);
    if (status == kStatus_SSS_Success) {
        status = sss_rng_get_random(&rng, rand_out, count);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_rng_context_free(&rng);
    }
    if (status != kStatus_SSS_Success) {
        ret = RNG_FAILURE_E;
    }

    wolfSSL_CryptHwMutexUnLock();

    return ret;
}
#endif /* !WC_NO_RNG && !WOLFSSL_SE050_NO_TRNG */

#ifdef WOLFSSL_SE050_HASH

/* Used for sha/sha224/sha384/sha512 */
int se050_hash_init(SE050_HASH_Context* se050Ctx, void* heap)
{
    se050Ctx->heap = heap;
    se050Ctx->len  = 0;
    se050Ctx->used = 0;
    se050Ctx->msg  = NULL;
    return 0;
}

int se050_hash_copy(SE050_HASH_Context* src, SE050_HASH_Context* dst)
{
    if (src == NULL || dst == NULL || (src->used != dst->used)) {
        return BAD_FUNC_ARG;
    }

    if (src->used > 0) {
        /* dst->msg points to same buffer as src->msg, needs to be allocated
         * and dep copied over instead of plain pointer copy */
        dst->msg = (byte*)XMALLOC(src->used, dst->heap,
                                  DYNAMIC_TYPE_TMP_BUFFER);
        if (dst->msg == NULL) {
            PRINTF("Tried to allocate %d bytes\n", dst->used);
            return MEMORY_E;
        }
        XMEMSET(dst->msg, 0, dst->used);
        XMEMCPY(dst->msg, src->msg, src->used);
        dst->used = src->used;
        dst->len = src->used;
    } else {
        dst->msg = NULL;
        dst->len = 0;
        dst->used = 0;
    }

    return 0;
}

int se050_hash_update(SE050_HASH_Context* se050Ctx, const byte* data, word32 len)
{
	byte* tmp = NULL;

    if (se050Ctx == NULL || (len > 0 && data == NULL)) {
        return BAD_FUNC_ARG;
    }

    if (se050Ctx->len < se050Ctx->used + len) {
        if (se050Ctx->msg == NULL) {
            se050Ctx->msg = (byte*)XMALLOC(se050Ctx->used + len,
                se050Ctx->heap, DYNAMIC_TYPE_TMP_BUFFER);
            XMEMSET(se050Ctx->msg, 0, se050Ctx->used + len);
        }
        else {
            tmp = (byte*)XMALLOC(se050Ctx->used + len, se050Ctx->heap,
                                 DYNAMIC_TYPE_TMP_BUFFER);
            if (tmp == NULL) {
                return MEMORY_E;
            }
            XMEMSET(tmp, 0, se050Ctx->used + len);
            XMEMCPY(tmp, se050Ctx->msg, se050Ctx->used);
            XFREE(se050Ctx->msg, se050Ctx->heap, DYNAMIC_TYPE_TMP_BUFFER);
            se050Ctx->msg = tmp;
        }
        if (se050Ctx->msg == NULL) {
            return MEMORY_E;
        }
        se050Ctx->len = se050Ctx->used + len;
    }

    XMEMCPY(se050Ctx->msg + se050Ctx->used, data, len);
    se050Ctx->used += len;

    return 0;
}

int se050_hash_final(SE050_HASH_Context* se050Ctx, byte* hash, size_t digestLen,
    sss_algorithm_t algo)
{
    int          ret;
    sss_status_t status;
    sss_digest_t digest_ctx;
    const byte*  data = se050Ctx->msg;
    int          size = (se050Ctx->used) / SSS_BLOCK_SIZE;
    int          leftover = (se050Ctx->used) % SSS_BLOCK_SIZE;
    const byte*  blocks = data;

    if (cfg_se050_i2c_pi == NULL) {
        return WC_HW_E;
    }

    if (wolfSSL_CryptHwMutexLock() != 0) {
        return BAD_MUTEX_E;
    }

    status = sss_digest_context_init(&digest_ctx, cfg_se050_i2c_pi, algo,
        kMode_SSS_Digest);
    if (status == kStatus_SSS_Success) {
        status = sss_digest_init(&digest_ctx);
    }
    if (status == kStatus_SSS_Success) {
        /* used to send chunks of size 512 */
        while (status == kStatus_SSS_Success && size--) {
            status = sss_digest_update(&digest_ctx, blocks, SSS_BLOCK_SIZE);
            blocks += SSS_BLOCK_SIZE;
        }
        if (status == kStatus_SSS_Success && leftover) {
            status = sss_digest_update(&digest_ctx, blocks, leftover);
        }
        if (status == kStatus_SSS_Success) {
            status = sss_digest_finish(&digest_ctx, hash, &digestLen);
        }
        sss_digest_context_free(&digest_ctx);
    }

    if (status == kStatus_SSS_Success) {
        /* reset state */
        ret = se050_hash_init(se050Ctx, se050Ctx->heap);
    } else {
        ret = WC_HW_E;
    }

    wolfSSL_CryptHwMutexUnLock();

    return ret;
}

void se050_hash_free(SE050_HASH_Context* se050Ctx)
{
    XFREE(se050Ctx->msg, se050Ctx->heap, DYNAMIC_TYPE_TMP_BUFFER);
    se050Ctx->msg = NULL;
    se050Ctx->len  = 0;
    se050Ctx->used = 0;
}

#endif /* WOLFSSL_SE050_HASH */

#if defined(WOLFSSL_SE050_CRYPT) && !defined(NO_AES)

int se050_aes_set_key(Aes* aes, const byte* key, word32 keylen,
                                        const byte* iv, int dir)
{
    int ret = 0;
    sss_status_t status;
    sss_object_t newKey;
    sss_key_store_t host_keystore;
    word32 keyId;
    int keyCreated = 0;

    if (cfg_se050_i2c_pi == NULL) {
        return WC_HW_E;
    }

    if (wolfSSL_CryptHwMutexLock() != 0) {
        return BAD_MUTEX_E;
    }

    (void)dir;
    (void)iv;

    aes->rounds = keylen/4 + 6;

    /* free existing key in slot first before storing new one */
    ret = wc_se050_erase_object(aes->keyId);
    if (ret != 0) {
        return ret;
    }
    aes->keyIdSet = 0;

    status = sss_key_store_context_init(&host_keystore, cfg_se050_i2c_pi);
    if (status == kStatus_SSS_Success) {
        status = sss_key_store_allocate(&host_keystore, SE050_KEYSTOREID_AES);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&newKey, &host_keystore);
    }
    if (status == kStatus_SSS_Success) {
        keyId = se050_allocate_key(SE050_AES_KEY);
        status = sss_key_object_allocate_handle(&newKey, keyId,
            kSSS_KeyPart_Default, kSSS_CipherType_AES, keylen,
            kKeyObject_Mode_Transient);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_store_set_key(&host_keystore, &newKey, key, keylen,
                                    keylen * 8, NULL, 0);
    }

    if (status == kStatus_SSS_Success) {
        keyCreated = 1;
        aes->keyId = keyId;
        aes->keyIdSet = 1;
        ret = 0;
    }
    else {
        if (keyCreated) {
            sss_key_store_erase_key(&host_keystore, &newKey);
            sss_key_object_free(&newKey);
        }
        ret = WC_HW_E;
    }

    wolfSSL_CryptHwMutexUnLock();

    return ret;
}

int se050_aes_crypt(Aes* aes, const byte* in, byte* out, word32 sz, int dir,
    sss_algorithm_t algorithm)
{
    int             ret = 0;
    sss_status_t    status;
    sss_object_t    keyObject;
    sss_key_store_t host_keystore;

    if (cfg_se050_i2c_pi == NULL) {
        return WC_HW_E;
    }
    if (aes->keyIdSet == 0) {
        return BAD_FUNC_ARG;
    }

    if (wolfSSL_CryptHwMutexLock() != 0) {
        return BAD_MUTEX_E;
    }

    status = sss_key_store_context_init(&host_keystore, cfg_se050_i2c_pi);
    if (status == kStatus_SSS_Success) {
        status = sss_key_store_allocate(&host_keystore, SE050_KEYSTOREID_AES);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&keyObject, &host_keystore);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_get_handle(&keyObject, aes->keyId);
    }

    /* The first call to this function needs an initialization call,
        * subsequent calls just need to call update */
    if (status == kStatus_SSS_Success && aes->ctxInitDone == 0) {
        sss_mode_t      mode;

        XMEMSET(&mode, 0, sizeof(mode));
        if (dir == AES_DECRYPTION)
            mode = kMode_SSS_Decrypt;
        else if (dir == AES_ENCRYPTION)
            mode = kMode_SSS_Encrypt;

        if (status == kStatus_SSS_Success) {
            status = sss_symmetric_context_init(&aes->aes_ctx,
                cfg_se050_i2c_pi, &keyObject, algorithm, mode);
        }
        if (status == kStatus_SSS_Success) {
            aes->ctxInitDone = 1;
            status = sss_cipher_init(&aes->aes_ctx, (uint8_t*)aes->reg,
                sizeof(aes->reg));
        }
    }
    if (status == kStatus_SSS_Success) {
        size_t outSz = (size_t)sz;
        status = sss_cipher_update(&aes->aes_ctx, in, sz, out, &outSz);
    }

    ret = (status == kStatus_SSS_Success) ? 0 : WC_HW_E;

    wolfSSL_CryptHwMutexUnLock();

    return ret;
}

void se050_aes_free(Aes* aes)
{
    if (aes == NULL) {
        return;
    }

    if (aes->ctxInitDone) {
        sss_symmetric_context_free(&aes->aes_ctx);

        /* sets back to zero to indicate that a free has been called */
        aes->ctxInitDone = 0;
    }

    aes->keyId = 0;
    aes->keyIdSet = 0;
}

#endif /* WOLFSSL_SE050_CRYPT && !NO_AES */

/**
 * Get size of a SE05X secure object at specified object ID.
 *
 * keystore  SE050 keystore associated with object
 * keyId     SE050 key ID in which object is stored
 *
 * Size returned depends on object type:
 *   ECC key: curve size
 *   RSA/AES/DES/HMAC key: key size
 *   Binary file: file size
 *
 * Return size or negative on error
 */
static int se050_get_object_size(sss_key_store_t* keystore, word32 keyId)
{
    uint16_t size = 0;
    smStatus_t status = SM_NOT_OK;
    sss_se05x_key_store_t* se05x_keystore = NULL;

    if (keystore == NULL) {
        return BAD_FUNC_ARG;
    }

    se05x_keystore = (sss_se05x_key_store_t*)keystore;
    status = Se05x_API_ReadSize(&se05x_keystore->session->s_ctx,
                                keyId, &size);
    if (status != SM_OK) {
        return WC_HW_E;
    }

    return (int)size;
}

/**
 * Insert binary object into SE050 as persistent object.
 *
 * keyId       SE050 key ID to store object in
 * object      binary object data
 * objectSz    size of binary object, bytes
 *
 * Returns 0 on success, negative on error
 */
int wc_se050_insert_binary_object(word32 keyId, const byte* object,
                                  word32 objectSz)
{
    int             ret = 0;
    sss_object_t    newObj;
    sss_key_store_t host_keystore;
    sss_status_t    status = kStatus_SSS_Success;

    if (wolfSSL_CryptHwMutexLock() != 0) {
        return BAD_MUTEX_E;
    }

    /* Avoid key ID conflicts with temporary key storage */
    if (keyId >= SE050_KEYID_START) {
        return BAD_FUNC_ARG;
    }

    status = sss_key_store_context_init(&host_keystore, cfg_se050_i2c_pi);
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&newObj, &host_keystore);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_allocate_handle(&newObj, keyId,
            kSSS_KeyPart_Default, kSSS_CipherType_Binary, objectSz,
            kKeyObject_Mode_Persistent);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_store_set_key(&host_keystore, &newObj, object,
                                       objectSz, (objectSz * 8), NULL, 0);
    }
    wolfSSL_CryptHwMutexUnLock();

    if (status != kStatus_SSS_Success) {
        ret = WC_HW_E;
    }

    return ret;
}

/**
 * Get binary object from SE050 from specified key ID.
 *
 * keyId  SE050 key ID to get binary object from
 * out    output buffer to place binary object
 * outSz  size of output buffer on input, size of written object on output
 *
 * Returns 0 on success, LENGTH_ONLY_E if out is NULL with outSz set to
 * required buffer size, and other negative on error.
 */
int wc_se050_get_binary_object(word32 keyId, byte* out, word32* outSz)
{
    int             ret = 0;
    sss_object_t    object;
    sss_key_store_t host_keystore;
    sss_status_t    status = kStatus_SSS_Success;
    size_t outBitSz = 0;

    /* If out is NULL, outSz set to required size and LENGTH_ONLY_E returned */
    if (outSz == NULL) {
        return BAD_FUNC_ARG;
    }

    if (wolfSSL_CryptHwMutexLock() != 0) {
        return BAD_MUTEX_E;
    }

    status = sss_key_store_context_init(&host_keystore, cfg_se050_i2c_pi);
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&object, &host_keystore);
    }
    if (status == kStatus_SSS_Success) {
        ret = se050_get_object_size(&host_keystore, keyId);
        if (ret < 0) {
            status = kStatus_SSS_Fail;
        }
        else {
            if (out == NULL) {
                *outSz = ret;
                return LENGTH_ONLY_E;
            }
            if ((word32)ret > *outSz) {
                WOLFSSL_MSG("Output buffer not large enough for object");
                return BAD_LENGTH_E;
            }
            ret = 0;
        }
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_get_handle(&object, keyId);
    }
    if (status == kStatus_SSS_Success) {
        outBitSz = (*outSz) * 8;
        status = sss_key_store_get_key(&host_keystore, &object, out,
                                       (size_t*)outSz, &outBitSz);
    }
    wolfSSL_CryptHwMutexUnLock();

    if (status != kStatus_SSS_Success) {
        ret = WC_HW_E;
    }

    return ret;
}

#ifndef NO_RSA

/**
 * Use specified SE050 key ID with this RsaKey struct.
 * Should be called by wc_RsaUseKeyId() for using pre-populated
 * SE050 keys.
 *
 * key   Pointer to initialized RsaKey structure
 * keyId SE050 key ID containing RSA key object
 *
 * Return 0 on success, negative on error.
 */
int se050_rsa_use_key_id(struct RsaKey* key, word32 keyId)
{
    int ret = 0;
    sss_object_t    keyObject;
    sss_key_store_t host_keystore;
    sss_status_t    status = kStatus_SSS_Success;
    uint8_t*        derBuf = NULL;
    size_t          derSz = 0;
    size_t          derSzBits = 0;
    word32          idx = 0;

#ifdef SE050_DEBUG
    printf("se050_rsa_use_key_id: key %p, keyId %d\n", key, keyId);
#endif
    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    if (cfg_se050_i2c_pi == NULL) {
        return WC_HW_E;
    }

    if (wolfSSL_CryptHwMutexLock() != 0) {
        return BAD_MUTEX_E;
    }

    status = sss_key_store_context_init(&host_keystore, cfg_se050_i2c_pi);
    if (status == kStatus_SSS_Success) {
        status = sss_key_store_allocate(&host_keystore, SE050_KEYSTOREID_RSA);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&keyObject, &host_keystore);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_get_handle(&keyObject, keyId);
    }
    if (status == kStatus_SSS_Success) {
        ret = se050_get_object_size(&host_keystore, keyObject.keyId);
        if (ret <= 0) {
            status = kStatus_SSS_Fail;
        }
        else {
            /* double derSz to allow for ASN.1 encoding space */
            derSz = ((size_t)ret) * 2;
            ret = 0;
            derBuf = (uint8_t*)XMALLOC(derSz, key->heap,
                                       DYNAMIC_TYPE_TMP_BUFFER);
            if (derBuf == NULL) {
                WOLFSSL_MSG("Error calling malloc for RSA DER buffer");
                status = kStatus_SSS_Fail;
            }
        }
    }
    if (status == kStatus_SSS_Success) {
        derSzBits = derSz * 8;
        XMEMSET(derBuf, 0, derSz);
        status = sss_key_store_get_key(&host_keystore, &keyObject,
            derBuf, &derSz, &derSzBits);
        (void)derSzBits; /* not used */
    }
    if (status == kStatus_SSS_Success) {
        /* Populate RsaKey with general key info, for wolfCrypt to use */
        ret = wc_RsaPublicKeyDecode(derBuf, &idx, key, (word32)derSz);
        if (ret != 0) {
            status = kStatus_SSS_Fail;
        }
    }
    if (derBuf != NULL) {
        XFREE(derBuf, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }

    if (status == kStatus_SSS_Success) {
        key->keyId = keyId;
        key->keyIdSet = 1;
        key->type = RSA_PRIVATE;
        ret = 0;
    }
    else if (ret == 0) {
        ret = WC_HW_E;
    }

    sss_key_object_free(&keyObject);

    wolfSSL_CryptHwMutexUnLock();

#ifdef SE050_DEBUG
    printf("se050_rsa_use_key_id: ret %d\n", ret);
#endif

    return ret;
}

/**
 * Get SE050 key ID associated with this RsaKey struct.
 * Should be called by wc_RsaGetKeyId() for the application to get
 * what key ID wolfCrypt picked for this RsaKey struct when generating
 * a key inside the SE050.
 *
 * key   Pointer to initialized RsaKey structure
 * keyId [OUT] SE050 key ID associated with this key structure
 *
 * Return 0 on success, negative on error.
 */
int se050_rsa_get_key_id(struct RsaKey* key, word32* keyId)
{
    int ret = 0;

    if (key == NULL || keyId == NULL) {
        return BAD_FUNC_ARG;
    }

    if (key->keyIdSet == 1) {
        *keyId = key->keyId;

    } else {
        WOLFSSL_MSG("SE050 key ID not set for RsaKey struct");
        ret = WC_HW_E;
    }

    return ret;
}

/**
 * Create RSA key pair inside SE050.
 *
 * key   RsaKey structure to store generated key information in
 * size  RSA key size to generate in bytes
 * e     RSA exponent, must be 65537 for SE050 compatibility
 *
 * Returns 0 on success, negative on error.
 */
int se050_rsa_create_key(struct RsaKey* key, int size, long e)
{
    int             ret = 0;
    word32          keyId;
    int             keyCreated = 0;
    sss_status_t    status = kStatus_SSS_Success;
    sss_object_t    keyPair;
    sss_key_store_t host_keystore;
    uint8_t*        derBuf = NULL;
    size_t          derSz = 0;
    size_t          derSzBits = 0;
    word32          idx = 0;

#ifdef SE050_DEBUG
    printf("se050_rsa_create_key: key %p, size %d, e %ld\n", key, size, e);
#endif

    if (cfg_se050_i2c_pi == NULL) {
        return WC_HW_E;
    }

    if (e != 65537) {
        WOLFSSL_MSG("SE050 RSA key create only supports exponent of 65537");
        return WC_HW_E;
    }

    if (wolfSSL_CryptHwMutexLock() != 0) {
        return BAD_MUTEX_E;
    }

    status = sss_key_store_context_init(&host_keystore, cfg_se050_i2c_pi);
    if (status == kStatus_SSS_Success) {
        status = sss_key_store_allocate(&host_keystore, SE050_KEYSTOREID_RSA);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&keyPair, &host_keystore);
    }
    if (status == kStatus_SSS_Success) {
        keyId = se050_allocate_key(SE050_RSA_KEY);
        status = sss_key_object_allocate_handle(&keyPair, keyId,
            kSSS_KeyPart_Pair, kSSS_CipherType_RSA, (size / 8),
            kKeyObject_Mode_Persistent);
    }
    if (status == kStatus_SSS_Success) {
        /* Try to delete existing key first. Ignore return since will fail
         * if no key exists */
        sss_key_store_erase_key(&host_keystore, &keyPair);

        keyCreated = 1;
        status = sss_key_store_generate_key(&host_keystore, &keyPair,
            size, NULL);
    }
    if (status == kStatus_SSS_Success) {
        ret = se050_get_object_size(&host_keystore, keyPair.keyId);
        if (ret <= 0) {
            status = kStatus_SSS_Fail;
        }
        else {
            /* double derSz to allow for ASN.1 encoding space */
            derSz = ((size_t)ret) * 2;
            ret = 0;
            derBuf = (uint8_t*)XMALLOC(derSz, key->heap,
                                       DYNAMIC_TYPE_TMP_BUFFER);
            if (derBuf == NULL) {
                WOLFSSL_MSG("Error calling malloc for RSA DER buffer");
                status = kStatus_SSS_Fail;
            }
        }
    }
    if (status == kStatus_SSS_Success) {
        derSzBits = derSz * 8;
        XMEMSET(derBuf, 0, derSz);
        status = sss_key_store_get_key(&host_keystore, &keyPair,
            derBuf, &derSz, &derSzBits);
        (void)derSzBits; /* not used */
    }
    if (status == kStatus_SSS_Success) {
        ret = wc_RsaPublicKeyDecode(derBuf, &idx, key, (word32)derSz);
        if (ret != 0) {
            status = kStatus_SSS_Fail;
        }
    }
    if (derBuf != NULL) {
        XFREE(derBuf, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }

    if (status == kStatus_SSS_Success) {
        key->keyId = keyId;
        key->keyIdSet = 1;
        key->type = RSA_PRIVATE;
        ret = 0;
    }
    else {
        if (keyCreated) {
            sss_key_store_erase_key(&host_keystore, &keyPair);
            sss_key_object_free(&keyPair);
        }
        if (ret == 0) {
            ret = WC_HW_E;
        }
    }

    wolfSSL_CryptHwMutexUnLock();

#ifdef SE050_DEBUG
    printf("se050_rsa_create_key: key %p, ret %d, keyId %d\n",
           key, ret, key->keyId);
#endif

    return ret;
}

static int se050_rsa_insert_key(word32 keyId, const byte* rsaDer,
                                word32 rsaDerSize, int keyType)
{
    int             ret = 0;
    int             keySize;
    word32          idx = 0;
    sss_object_t    newKey;
    sss_key_store_t host_keystore;
    sss_status_t    status = kStatus_SSS_Success;
    struct RsaKey   key;
    sss_key_part_t  keyPart = kSSS_KeyPart_Pair;

    if (wolfSSL_CryptHwMutexLock() != 0) {
        return BAD_MUTEX_E;
    }

    /* Avoid key ID conflicts with temporary key storage */
    if (keyId >= SE050_KEYID_START) {
        return BAD_FUNC_ARG;
    }

    ret = wc_InitRsaKey(&key, NULL);
    if (ret != 0) {
        status = kStatus_SSS_Fail;
    }
    else {
        if (keyType == RSA_PUBLIC) {
            keyPart = kSSS_KeyPart_Public;
            ret = wc_RsaPublicKeyDecode(rsaDer, &idx, &key, rsaDerSize);
        }
        else if (keyType == RSA_PRIVATE) {
            keyPart = kSSS_KeyPart_Pair;
            ret = wc_RsaPrivateKeyDecode(rsaDer, &idx, &key, rsaDerSize);
        }
        else {
            ret = BAD_FUNC_ARG;
        }

        if (ret != 0) {
            status = kStatus_SSS_Fail;
        }
    }

    if (status == kStatus_SSS_Success) {
        keySize = wc_RsaEncryptSize(&key);
        if (keySize < 0) {
            status = kStatus_SSS_Fail;
        }
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_store_context_init(&host_keystore, cfg_se050_i2c_pi);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&newKey, &host_keystore);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_allocate_handle(&newKey, keyId,
            keyPart, kSSS_CipherType_RSA, keySize,
            kKeyObject_Mode_Persistent);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_store_set_key(&host_keystore, &newKey, rsaDer,
                                       rsaDerSize, (keySize * 8), NULL, 0);
    }
    wolfSSL_CryptHwMutexUnLock();

    wc_FreeRsaKey(&key);
    if (status != kStatus_SSS_Success) {
        if (ret == 0) {
            ret = WC_HW_E;
        }
    }

    return ret;
}

/**
 * Insert DER encoded RSA private key into SE050 as a persistent key.
 *
 * keyId       SE050 key ID to store key into
 * rsaDer      DER encoded RSA private key
 * rsaDerSize  size of DER buffer, bytes
 *
 * Returns 0 on success, negative on error
 */
int wc_se050_rsa_insert_private_key(word32 keyId, const byte* rsaDer,
                                    word32 rsaDerSize)
{
    return se050_rsa_insert_key(keyId, rsaDer, rsaDerSize, RSA_PRIVATE);
}

/**
 * Insert DER encoded RSA public key into SE050 as a persistent key.
 *
 * keyId       SE050 key ID to store key into
 * rsaDer      DER encoded RSA public key
 * rsaDerSize  size of DER buffer, bytes
 *
 * Returns 0 on success, negative on error
 */
int wc_se050_rsa_insert_public_key(word32 keyId, const byte* rsaDer,
                                   word32 rsaDerSize)
{
    return se050_rsa_insert_key(keyId, rsaDer, rsaDerSize, RSA_PUBLIC);
}

/**
 * Return sss_algorithm_t type for RSA sign/verify based on wolfCrypt pad type,
 * hash value, and mask generation function (mgf).
 *
 * padType  padding type
 * hash     hash function
 * mgf      mask generation function (for PSS)
 *
 * Returns algorithm type or kAlgorithm_None if none supported found
 */
static sss_algorithm_t se050_get_rsa_signature_type(int padType,
        enum wc_HashType hash, int mgf)
{
    sss_algorithm_t alg = kAlgorithm_None;

    switch (padType) {
        case WC_RSA_PKCSV15_PAD:
            if (hash == WC_HASH_TYPE_NONE) {
                alg = kAlgorithm_SSS_RSASSA_PKCS1_V1_5_NO_HASH;
            } else if (hash == WC_HASH_TYPE_SHA) {
                alg = kAlgorithm_SSS_RSASSA_PKCS1_V1_5_SHA1;
            } else if (hash == WC_HASH_TYPE_SHA224) {
                alg = kAlgorithm_SSS_RSASSA_PKCS1_V1_5_SHA224;
            } else if (hash == WC_HASH_TYPE_SHA256) {
                alg = kAlgorithm_SSS_RSASSA_PKCS1_V1_5_SHA256;
            } else if (hash == WC_HASH_TYPE_SHA384) {
                alg = kAlgorithm_SSS_RSASSA_PKCS1_V1_5_SHA384;
            } else if (hash == WC_HASH_TYPE_SHA512) {
                alg = kAlgorithm_SSS_RSASSA_PKCS1_V1_5_SHA512;
            }
            break;
        case WC_RSA_OAEP_PAD:
            if (hash == WC_HASH_TYPE_SHA) {
                alg = kAlgorithm_SSS_RSAES_PKCS1_OAEP_SHA1;
            } else if (hash == WC_HASH_TYPE_SHA224) {
                alg = kAlgorithm_SSS_RSAES_PKCS1_OAEP_SHA224;
            } else if (hash == WC_HASH_TYPE_SHA256) {
                alg = kAlgorithm_SSS_RSAES_PKCS1_OAEP_SHA256;
            } else if (hash == WC_HASH_TYPE_SHA384) {
                alg = kAlgorithm_SSS_RSAES_PKCS1_OAEP_SHA384;
            } else if (hash == WC_HASH_TYPE_SHA512) {
                alg = kAlgorithm_SSS_RSAES_PKCS1_OAEP_SHA512;
            }
            break;
        case WC_RSA_PSS_PAD:
            if (mgf == WC_MGF1SHA1) {
                alg = kAlgorithm_SSS_RSASSA_PKCS1_PSS_MGF1_SHA1;
            } else if (mgf == WC_MGF1SHA224) {
                alg = kAlgorithm_SSS_RSASSA_PKCS1_PSS_MGF1_SHA224;
            } else if (mgf == WC_MGF1SHA256) {
                alg = kAlgorithm_SSS_RSASSA_PKCS1_PSS_MGF1_SHA256;
            } else if (mgf == WC_MGF1SHA384) {
                alg = kAlgorithm_SSS_RSASSA_PKCS1_PSS_MGF1_SHA384;
            } else if (mgf == WC_MGF1SHA512) {
                alg = kAlgorithm_SSS_RSASSA_PKCS1_PSS_MGF1_SHA512;
            }
            break;
        case WC_RSA_NO_PAD:
            alg = kAlgorithm_SSS_RSASSA_NO_PADDING;
            break;
        default:
            break;
    }

    return alg;
}

static sss_algorithm_t se050_get_rsa_encrypt_type(int padType,
        enum wc_HashType hash)
{
    sss_algorithm_t alg = kAlgorithm_None;
    (void)hash;

    switch (padType) {
        case WC_RSA_PKCSV15_PAD:
            alg = kAlgorithm_SSS_RSAES_PKCS1_V1_5;
            break;
        case WC_RSA_OAEP_PAD:
            /* lower level Se05x API translation maps OAEP-SHA1 alg type to
             * kSE05x_RSAEncryptionAlgo_PKCS1_OAEP (generic) */
            alg = kAlgorithm_SSS_RSAES_PKCS1_OAEP_SHA1;
            break;
        case WC_RSA_NO_PAD:
            alg = kAlgorithm_SSS_RSASSA_NO_PADDING;
            break;
        default:
            break;
    }

    return alg;
}

/**
 * RSA sign operation.
 *
 * in        input data to be signed
 * inLen     length of input data, bytes
 * out       output buffer containing signature
 * outLen    length of output buffer, bytes
 * key       pointer to initialized/populated RsaKey structure
 * rsa_type  type of RSA: must be RSA_PRIVATE_ENCRYPT
 * pad_value should be RSA_BLOCK_TYPE_1 for signing
 * pad_type  type of padding: WC_RSA_PKCSV15_PAD, WC_RSA_OAEP_PAD,
 *           WC_RSA_NO_PAD, WC_RSA_PSS_PAD
 * hash      type of hash algorithm, found in wolfssl/wolfcrypt/hash.h
 * mgf       type of mask generation function to use
 * label     optional label, not supported by SE050, must be NULL
 * labelSz   size of label, not supported by SE050, must be 0
 * keySz     size of RSA key, bytes
 *
 * Return size of signature on success, negative on error.
 */
int se050_rsa_sign(const byte* in, word32 inLen, byte* out,
                   word32 outLen, struct RsaKey* key, int rsa_type,
                   byte pad_value, int pad_type, enum wc_HashType hash,
                   int mgf, byte* label, word32 labelSz, int keySz)
{
    int ret = 0;
    int keyCreated = 0;
    word32 keyId;
    size_t sigSz;
    sss_object_t     newKey;
    sss_status_t     status;
    sss_key_store_t  host_keystore;
    sss_algorithm_t  algorithm = kAlgorithm_None;
    sss_asymmetric_t ctx_asymm;
    byte* derBuf = NULL;
    int derSz = 0;

    /* SE050 does not support optional label */
    (void)label;
    (void)labelSz;

#ifdef SE050_DEBUG
    printf("se050_rsa_sign: key %p, in %p (%d), out %p (%d), "
            "key %p, type %d, pad_value = %d, pad_type = %d, mgf = %d\n",
            key, in, inLen, out, outLen, key, rsa_type, pad_value,
            pad_type, mgf);
#endif

    if (in == NULL || out == NULL || key == NULL ||
        rsa_type != RSA_PRIVATE_ENCRYPT || pad_value != RSA_BLOCK_TYPE_1) {
        return BAD_FUNC_ARG;
    }

    if (cfg_se050_i2c_pi == NULL) {
        return WC_HW_E;
    }

    if (wolfSSL_CryptHwMutexLock() != 0) {
        return BAD_MUTEX_E;
    }

    algorithm = se050_get_rsa_signature_type(pad_type, hash, mgf);
    if (algorithm == kAlgorithm_None) {
        WOLFSSL_MSG("Unsupported padding/hash/mgf combination for SE050");
        return BAD_FUNC_ARG;
    }

    status = sss_key_store_context_init(&host_keystore, cfg_se050_i2c_pi);
    if (status == kStatus_SSS_Success) {
        status = sss_key_store_allocate(&host_keystore, SE050_KEYSTOREID_RSA);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&newKey, &host_keystore);
    }
    if (status == kStatus_SSS_Success) {
        keyId = key->keyId;
        if (key->keyIdSet == 0) {
            /* key was not generated in SE050, export RsaKey to DER
             * and use that to store into SE050 keystore */
            derSz = wc_RsaKeyToDer(key, NULL, 0);
            if (derSz < 0) {
                status = kStatus_SSS_Fail;
                ret = derSz;
            }
            else {
                derBuf = (byte*)XMALLOC(derSz, key->heap,
                                        DYNAMIC_TYPE_TMP_BUFFER);
                if (derBuf == NULL) {
                    WOLFSSL_MSG("malloc failed when converting RsaKey to DER");
                    status = kStatus_SSS_Fail;
                    ret = MEMORY_E;
                }
            }
            if (status == kStatus_SSS_Success) {
                derSz = wc_RsaKeyToDer(key, derBuf, derSz);
                if (derSz < 0) {
                    status = kStatus_SSS_Fail;
                    ret = derSz;
                }
            }
            if (status == kStatus_SSS_Success) {
                keyId = se050_allocate_key(SE050_RSA_KEY);
                status = sss_key_object_allocate_handle(&newKey, keyId,
                    kSSS_KeyPart_Pair, kSSS_CipherType_RSA, keySz,
                    kKeyObject_Mode_Persistent);
            }
            if (status == kStatus_SSS_Success) {
                /* Try to delete existing key first, ignore return since will
                 * fail if no key exists yet */
                sss_key_store_erase_key(&host_keystore, &newKey);

                keyCreated = 1;
                status = sss_key_store_set_key(&host_keystore, &newKey, derBuf,
                                               derSz, (keySz * 8), NULL, 0);
            }

            if (derBuf != NULL) {
                XFREE(derBuf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            }
        }
        else {
            status = sss_key_object_get_handle(&newKey, keyId);
        }
    }

    if (status == kStatus_SSS_Success) {
        status = sss_asymmetric_context_init(&ctx_asymm, cfg_se050_i2c_pi,
                                            &newKey, algorithm, kMode_SSS_Sign);
        if (status == kStatus_SSS_Success) {
            sigSz = outLen;
            status = sss_asymmetric_sign_digest(&ctx_asymm, (uint8_t*)in, inLen,
                                                out, &sigSz);
        }
        sss_asymmetric_context_free(&ctx_asymm);
    }

    if (status == kStatus_SSS_Success) {
        key->keyId = keyId;
        key->keyIdSet = 1;
        ret = sigSz;
    }
    else {
        if (keyCreated) {
            sss_key_store_erase_key(&host_keystore, &newKey);
            sss_key_object_free(&newKey);
        }
        if (ret == 0) {
            ret = WC_HW_E;
        }
    }

    wolfSSL_CryptHwMutexUnLock();

#ifdef SE050_DEBUG
    printf("se050_rsa_sign: ret %d, outLen %d\n", ret, outLen);
#endif

    return ret;
}

/**
 * RSA verify operation.
 *
 * in        input signature to be verified
 * inLen     length of sig, bytes
 * out       output buffer containing decoded data
 * outLen    length of output buffer, bytes
 * key       pointer to initialized/populated RsaKey structure
 * rsa_type  type of RSA: must be RSA_PUBLIC_DECRYPT
 * pad_value should be RSA_BLOCK_TYPE_1 for sign/verify
 * pad_type  type of padding: WC_RSA_PKCSV15_PAD, WC_RSA_OAEP_PAD,
 *           WC_RSA_NO_PAD, WC_RSA_PSS_PAD
 * hash      type of hash algorithm, found in wolfssl/wolfcrypt/hash.h
 * mgf       type of mask generation function to use
 * label     optional label, not supported by SE050, must be NULL
 * labelSz   size of label, not supported by SE050, must be 0
 *
 * Returns size of decoded data on success, negative on error.
 */
int se050_rsa_verify(const byte* in, word32 inLen, byte* out, word32 outLen,
                     struct RsaKey* key, int rsa_type, byte pad_value,
                     int pad_type, enum wc_HashType hash, int mgf, byte* label,
                     word32 labelSz)
{
    int ret = 0;
    word32 keyId;
    int keySz;
    int keyCreated = 0;
    size_t decLen  = 0;
    sss_status_t     status;
    sss_object_t     newKey;
    sss_key_store_t  host_keystore;
    sss_asymmetric_t ctx_asymm = {0};
    sss_se05x_asymmetric_t* se050_ctx_asymm = NULL;
    sss_algorithm_t  algorithm = kAlgorithm_None;
    smStatus_t       smStatus = SM_NOT_OK;
    byte* pad    = NULL;
    byte* derBuf = NULL;
    int derSz = 0;

#ifdef SE050_DEBUG
    printf("se050_rsa_pkcs1v15_verify: key %p, in %p (%d), out %p (%d)\n",
            key, in, inLen, out, outLen);
#endif

    if (in == NULL || out == NULL || key == NULL ||
        rsa_type != RSA_PUBLIC_DECRYPT || pad_value != RSA_BLOCK_TYPE_1) {
        return BAD_FUNC_ARG;
    }

    if (cfg_se050_i2c_pi == NULL) {
        return WC_HW_E;
    }

    if (wolfSSL_CryptHwMutexLock() != 0) {
        return BAD_MUTEX_E;
    }

    algorithm = se050_get_rsa_signature_type(pad_type, hash, mgf);
    if (algorithm == kAlgorithm_None) {
        WOLFSSL_MSG("Unsupported padding/hash/mgf combination for SE050");
        return BAD_FUNC_ARG;
    }

    status = sss_key_store_context_init(&host_keystore, cfg_se050_i2c_pi);
    if (status == kStatus_SSS_Success) {
        status = sss_key_store_allocate(&host_keystore, SE050_KEYSTOREID_RSA);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&newKey, &host_keystore);
    }
    if (status == kStatus_SSS_Success) {
        keySz = wc_RsaEncryptSize(key);
        if (keySz < 0) {
            WOLFSSL_MSG("Failed to get RSA key size from struct");
            status = kStatus_SSS_Fail;
        }
    }
    if (status == kStatus_SSS_Success) {
        keyId = key->keyId;
        if (key->keyIdSet == 0) {
            /* key was not generated in SE050, export RsaKey to DER
             * and use that to store into SE050 keystore */
            derSz = wc_RsaKeyToPublicDer(key, NULL, 0);
            if (derSz < 0) {
                status = kStatus_SSS_Fail;
                ret = derSz;
            }
            else {
                derBuf = (byte*)XMALLOC(derSz, key->heap,
                                        DYNAMIC_TYPE_TMP_BUFFER);
                if (derBuf == NULL) {
                    WOLFSSL_MSG("malloc failed when converting RsaKey to DER");
                    status = kStatus_SSS_Fail;
                    ret = MEMORY_E;
                }
            }
            if (status == kStatus_SSS_Success) {
                derSz = wc_RsaKeyToPublicDer(key, derBuf, derSz);
                if (derSz < 0) {
                    status = kStatus_SSS_Fail;
                    ret = derSz;
                }
            }
            if (status == kStatus_SSS_Success) {
                keyId = se050_allocate_key(SE050_RSA_KEY);
                status = sss_key_object_allocate_handle(&newKey, keyId,
                    kSSS_KeyPart_Public, kSSS_CipherType_RSA, keySz,
                    kKeyObject_Mode_Persistent);
            }
            if (status == kStatus_SSS_Success) {
                /* Try to delete existing key first, ignore return since will
                 * fail if no key exists yet */
                sss_key_store_erase_key(&host_keystore, &newKey);

                keyCreated = 1;
                status = sss_key_store_set_key(&host_keystore, &newKey, derBuf,
                                               derSz, (keySz * 8), NULL, 0);
            }

            if (derBuf != NULL) {
                XFREE(derBuf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            }
        }
        else {
            status = sss_key_object_get_handle(&newKey, keyId);
        }
    }

    if (status == kStatus_SSS_Success) {
        status = sss_asymmetric_context_init(&ctx_asymm, cfg_se050_i2c_pi,
                                    &newKey, algorithm, kMode_SSS_Verify);
        if (status == kStatus_SSS_Success) {
            /* Use lower Se05x API instead of sss_asymmetric_verify_digest()
             * since we need to return decoded data not just verify result */
            decLen = outLen;
            se050_ctx_asymm = (sss_se05x_asymmetric_t*)&ctx_asymm;
            smStatus = Se05x_API_RSAEncrypt(&se050_ctx_asymm->session->s_ctx,
                                            se050_ctx_asymm->keyObject->keyId,
                                            kSE05x_RSAEncryptionAlgo_NO_PAD,
                                            in, inLen, out, &decLen);
            if (smStatus == SM_OK) {
                /* find end of padding, pad points to start of actual data */
                ret = wc_RsaUnPad_ex(out, decLen, &pad, pad_value,
                        pad_type, hash, mgf,
                        label, labelSz, RSA_PSS_SALT_LEN_DEFAULT, (keySz * 8),
                        key->heap);
                if (ret >= 0) {
                    XMEMCPY(out, pad, ret);
                }
                else {
                    WOLFSSL_MSG("Error in wc_RsaUnPad_ex for RSA verify");
                    status = kStatus_SSS_Fail;
                }
            }
            else {
                WOLFSSL_MSG("Se05x_API_RSAEncrypt failed");
                status = kStatus_SSS_Fail;
            }
        }

        sss_asymmetric_context_free(&ctx_asymm);
    }

    if (status == kStatus_SSS_Success) {
        key->keyId = keyId;
        key->keyIdSet = 1;
    }
    else {
        if (keyCreated) {
            sss_key_store_erase_key(&host_keystore, &newKey);
            sss_key_object_free(&newKey);
        }
        if (ret == 0) {
            ret = WC_HW_E;
        }
    }

    wolfSSL_CryptHwMutexUnLock();

#ifdef SE050_DEBUG
    printf("se050_rsa_verify: key %p, ret %d\n", key, ret);
#endif

    return ret;
}

/**
 * RSA public encrypt operation.
 *
 * in        input data to be encrypted
 * inLen     length of input data, bytes
 * out       output buffer containing encrypted data
 * outLen    length of output buffer, bytes
 * key       pointer to initialized/populated RsaKey structure
 * rsa_type  type of RSA: must be RSA_PUBLIC_ENCRYPT
 * pad_value should be RSA_BLOCK_TYPE_2 for encrypting
 * pad_type  type of padding: WC_RSA_PKCSV15_PAD, WC_RSA_OAEP_PAD,
 *           WC_RSA_NO_PAD, WC_RSA_PSS_PAD
 * hash      type of hash algorithm, found in wolfssl/wolfcrypt/hash.h
 * mgf       type of mask generation function to use
 * label     optional label, not supported by SE050, must be NULL
 * labelSz   size of label, not supported by SE050, must be 0
 * keySz     size of RSA key, bytes
 *
 * Returns size of encrypted data on success, negative on error.
 */
int se050_rsa_public_encrypt(const byte* in, word32 inLen, byte* out,
                             word32 outLen, struct RsaKey* key, int rsa_type,
                             byte pad_value, int pad_type,
                             enum wc_HashType hash, int mgf, byte* label,
                             word32 labelSz, int keySz)
{
    int ret = 0;
    int keyCreated = 0;
    word32 keyId;
    size_t encSz;
    sss_object_t     newKey;
    sss_status_t     status;
    sss_key_store_t  host_keystore;
    sss_algorithm_t  algorithm = kAlgorithm_None;
    sss_asymmetric_t ctx_asymm;
    byte* derBuf = NULL;
    int derSz = 0;

    /* SE050 does not support optional label */
    (void)label;
    (void)labelSz;
    (void)mgf;

#ifdef SE050_DEBUG
    printf("se050_rsa_public_encrypt: key %p, in %p (%d), out %p (%d), "
            "key %p\n", key, in, inLen, out, outLen, key);
#endif

    if (in == NULL || out == NULL || key == NULL ||
        rsa_type != RSA_PUBLIC_ENCRYPT || pad_value != RSA_BLOCK_TYPE_2) {
        return BAD_FUNC_ARG;
    }

    if (cfg_se050_i2c_pi == NULL) {
        return WC_HW_E;
    }

    if (wolfSSL_CryptHwMutexLock() != 0) {
        return BAD_MUTEX_E;
    }

    algorithm = se050_get_rsa_encrypt_type(pad_type, hash);
    if (algorithm == kAlgorithm_None) {
        WOLFSSL_MSG("Unsupported padding/hash/mgf combination for SE050");
        return BAD_FUNC_ARG;
    }

    status = sss_key_store_context_init(&host_keystore, cfg_se050_i2c_pi);
    if (status == kStatus_SSS_Success) {
        status = sss_key_store_allocate(&host_keystore, SE050_KEYSTOREID_RSA);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&newKey, &host_keystore);
    }
    if (status == kStatus_SSS_Success) {
        keyId = key->keyId;
        if (key->keyIdSet == 0) {
            /* key was not generated in SE050, export RsaKey to DER
             * and use that to store into SE050 keystore */
            derSz = wc_RsaKeyToPublicDer(key, NULL, 0);
            if (derSz < 0) {
                status = kStatus_SSS_Fail;
                ret = derSz;
            }
            else {
                derBuf = (byte*)XMALLOC(derSz, key->heap,
                                        DYNAMIC_TYPE_TMP_BUFFER);
                if (derBuf == NULL) {
                    WOLFSSL_MSG("malloc failed when converting RsaKey to DER");
                    status = kStatus_SSS_Fail;
                    ret = MEMORY_E;
                }
            }
            if (status == kStatus_SSS_Success) {
                derSz = wc_RsaKeyToPublicDer(key, derBuf, derSz);
                if (derSz < 0) {
                    status = kStatus_SSS_Fail;
                    ret = derSz;
                }
            }
            if (status == kStatus_SSS_Success) {
                keyId = se050_allocate_key(SE050_RSA_KEY);
                status = sss_key_object_allocate_handle(&newKey, keyId,
                    kSSS_KeyPart_Public, kSSS_CipherType_RSA, keySz,
                    kKeyObject_Mode_Persistent);
            }
            if (status == kStatus_SSS_Success) {
                /* Try to delete existing key first, ignore return since will
                 * fail if no key exists yet */
                sss_key_store_erase_key(&host_keystore, &newKey);

                keyCreated = 1;
                status = sss_key_store_set_key(&host_keystore, &newKey, derBuf,
                                               derSz, (keySz * 8), NULL, 0);
            }
        }
        else {
            status = sss_key_object_get_handle(&newKey, keyId);
        }

        if (derBuf != NULL) {
            XFREE(derBuf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        }
    }

    if (status == kStatus_SSS_Success) {
        status = sss_asymmetric_context_init(&ctx_asymm, cfg_se050_i2c_pi,
                    &newKey, algorithm, kMode_SSS_Encrypt);
        if (status == kStatus_SSS_Success) {
            encSz = outLen;
            status = sss_asymmetric_encrypt(&ctx_asymm, (uint8_t*)in, inLen,
                                            out, &encSz);
        }
        sss_asymmetric_context_free(&ctx_asymm);
    }

    if (status == kStatus_SSS_Success) {
        key->keyId = keyId;
        key->keyIdSet = 1;
        ret = encSz;
    }
    else {
        if (keyCreated) {
            sss_key_store_erase_key(&host_keystore, &newKey);
            sss_key_object_free(&newKey);
        }
        if (ret == 0) {
            ret = WC_HW_E;
        }
    }

    wolfSSL_CryptHwMutexUnLock();

#ifdef SE050_DEBUG
    printf("se050_rsa_public_encrypt: ret %d, outLen %d\n", ret, outLen);
#endif

    return ret;
}

/**
 * RSA public decrypt operation.
 *
 * in        input data to be decrypted
 * inLen     length of input data, bytes
 * out       output buffer containing decrypted data
 * outLen    length of output buffer, bytes
 * key       pointer to initialized/populated RsaKey structure
 * rsa_type  type of RSA: must be RSA_PRIVATE_DECRYPT
 * pad_value should be RSA_BLOCK_TYPE_2 for encrypting
 * pad_type  type of padding: WC_RSA_PKCSV15_PAD, WC_RSA_OAEP_PAD,
 *           WC_RSA_NO_PAD, WC_RSA_PSS_PAD
 * hash      type of hash algorithm, found in wolfssl/wolfcrypt/hash.h
 * mgf       type of mask generation function to use
 * label     optional label, not supported by SE050, must be NULL
 * labelSz   size of label, not supported by SE050, must be 0
 *
 * Returns size of decrypted data on success, negative on error.
 */
int se050_rsa_private_decrypt(const byte* in, word32 inLen, byte* out,
                             word32 outLen, struct RsaKey* key, int rsa_type,
                             byte pad_value, int pad_type,
                             enum wc_HashType hash, int mgf, byte* label,
                             word32 labelSz)
{
    int ret = 0;
    int keyCreated = 0;
    word32 keyId;
    int keySz;
    size_t decSz;
    sss_object_t     newKey;
    sss_status_t     status;
    sss_key_store_t  host_keystore;
    sss_algorithm_t  algorithm = kAlgorithm_None;
    sss_asymmetric_t ctx_asymm;
    byte* derBuf = NULL;
    int derSz = 0;

    /* SE050 does not support optional label */
    (void)label;
    (void)labelSz;
    (void)mgf;

#ifdef SE050_DEBUG
    printf("se050_rsa_public_decrypt: key %p, in %p (%d), out %p (%d), "
            "key %p\n", key, in, inLen, out, outLen, key);
#endif

    if (in == NULL || out == NULL || key == NULL ||
        rsa_type != RSA_PRIVATE_DECRYPT || pad_value != RSA_BLOCK_TYPE_2) {
        return BAD_FUNC_ARG;
    }

    if (cfg_se050_i2c_pi == NULL) {
        return WC_HW_E;
    }

    if (wolfSSL_CryptHwMutexLock() != 0) {
        return BAD_MUTEX_E;
    }

    algorithm = se050_get_rsa_encrypt_type(pad_type, hash);
    if (algorithm == kAlgorithm_None) {
        WOLFSSL_MSG("Unsupported padding/hash/mgf combination for SE050");
        return BAD_FUNC_ARG;
    }

    status = sss_key_store_context_init(&host_keystore, cfg_se050_i2c_pi);
    if (status == kStatus_SSS_Success) {
        status = sss_key_store_allocate(&host_keystore, SE050_KEYSTOREID_RSA);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&newKey, &host_keystore);
    }
    if (status == kStatus_SSS_Success) {
        keySz = wc_RsaEncryptSize(key);
        if (keySz < 0) {
            WOLFSSL_MSG("Failed to get RSA key size from struct");
            status = kStatus_SSS_Fail;
        }
    }
    if (status == kStatus_SSS_Success) {
        keyId = key->keyId;
        if (key->keyIdSet == 0) {
            /* key was not generated in SE050, export RsaKey to DER
             * and use that to store into SE050 keystore */
            derSz = wc_RsaKeyToDer(key, NULL, 0);
            if (derSz < 0) {
                status = kStatus_SSS_Fail;
                ret = derSz;
            }
            else {
                derBuf = (byte*)XMALLOC(derSz, key->heap,
                                        DYNAMIC_TYPE_TMP_BUFFER);
                if (derBuf == NULL) {
                    WOLFSSL_MSG("malloc failed when converting RsaKey to DER");
                    status = kStatus_SSS_Fail;
                    ret = MEMORY_E;
                }
            }
            if (status == kStatus_SSS_Success) {
                derSz = wc_RsaKeyToDer(key, derBuf, derSz);
                if (derSz < 0) {
                    status = kStatus_SSS_Fail;
                    ret = derSz;
                }
            }
            if (status == kStatus_SSS_Success) {
                keyId = se050_allocate_key(SE050_RSA_KEY);
                status = sss_key_object_allocate_handle(&newKey, keyId,
                    kSSS_KeyPart_Pair, kSSS_CipherType_RSA, keySz,
                    kKeyObject_Mode_Persistent);
            }
            if (status == kStatus_SSS_Success) {
                /* Try to delete existing key first, ignore return since will
                 * fail if no key exists yet */
                sss_key_store_erase_key(&host_keystore, &newKey);

                keyCreated = 1;
                status = sss_key_store_set_key(&host_keystore, &newKey, derBuf,
                                               derSz, (keySz * 8), NULL, 0);
            }
        }
        else {
            status = sss_key_object_get_handle(&newKey, keyId);
        }

        if (derBuf != NULL) {
            XFREE(derBuf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        }
    }

    if (status == kStatus_SSS_Success) {
        status = sss_asymmetric_context_init(&ctx_asymm, cfg_se050_i2c_pi,
                    &newKey, algorithm, kMode_SSS_Decrypt);
        if (status == kStatus_SSS_Success) {
            decSz = outLen;
            status = sss_asymmetric_decrypt(&ctx_asymm, (uint8_t*)in, inLen,
                                            out, &decSz);
        }
        sss_asymmetric_context_free(&ctx_asymm);
    }

    if (status == kStatus_SSS_Success) {
        key->keyId = keyId;
        key->keyIdSet = 1;
        ret = decSz;
    }
    else {
        if (keyCreated) {
            sss_key_store_erase_key(&host_keystore, &newKey);
            sss_key_object_free(&newKey);
        }
        if (ret == 0) {
            ret = WC_HW_E;
        }
    }

    wolfSSL_CryptHwMutexUnLock();

#ifdef SE050_DEBUG
    printf("se050_rsa_public_decrypt: ret %d, outLen %d\n", ret, outLen);
#endif

    return ret;
}

#endif /* NO_RSA */

#ifdef HAVE_ECC

static int se050_map_curve(int curve_id, int keySize,
    int* keySizeBits, sss_cipher_type_t* pcurve_type)
{
    int ret = 0;
    sss_cipher_type_t curve_type = kSSS_CipherType_NONE;

    *keySizeBits = keySize * 8; /* set default */
    switch (curve_id) {
        case ECC_SECP160K1:
        case ECC_SECP192K1:
        case ECC_SECP224K1:
        case ECC_SECP256K1:
        #ifdef HAVE_ECC_KOBLITZ
            curve_type = kSSS_CipherType_EC_NIST_K;
        #else
            ret = ECC_CURVE_OID_E;
        #endif
            break;
        case ECC_BRAINPOOLP160R1:
        case ECC_BRAINPOOLP192R1:
        case ECC_BRAINPOOLP224R1:
        case ECC_BRAINPOOLP256R1:
        case ECC_BRAINPOOLP320R1:
        case ECC_BRAINPOOLP384R1:
        case ECC_BRAINPOOLP512R1:
        #ifdef HAVE_ECC_BRAINPOOL
            curve_type = kSSS_CipherType_EC_BRAINPOOL;
        #else
            ret = ECC_CURVE_OID_E;
        #endif
            break;
        case ECC_CURVE_DEF:
        case ECC_SECP160R1:
        case ECC_SECP192R1:
        case ECC_SECP224R1:
        case ECC_SECP256R1:
        case ECC_SECP384R1:
            curve_type = kSSS_CipherType_EC_NIST_P;
            break;
        case ECC_SECP521R1:
            curve_type = kSSS_CipherType_EC_NIST_P;
            *keySizeBits = 521;
            break;
        case ECC_PRIME239V1:
        case ECC_PRIME192V2:
        case ECC_PRIME192V3:
        default:
            ret = ECC_CURVE_OID_E;
            break;
    }
    if (pcurve_type)
        *pcurve_type = curve_type;
    return ret;
}

static sss_algorithm_t se050_map_hash_alg(int hashLen)
{
    sss_algorithm_t algorithm = kAlgorithm_None;
    if (hashLen == 20) {
        algorithm = kAlgorithm_SSS_SHA1;
    } else if (hashLen == 28) {
        algorithm = kAlgorithm_SSS_SHA224;
    } else if (hashLen == 32) {
        algorithm = kAlgorithm_SSS_SHA256;
    } else if (hashLen == 48) {
        algorithm = kAlgorithm_SSS_SHA384;
    } else if (hashLen == 64 || hashLen == 66) {
        /* ECC P-521 can pass key size 66, use SHA-512 */
        algorithm = kAlgorithm_SSS_SHA512;
    }
    return algorithm;
}

static int se050_ecc_insert_key(word32 keyId, const byte* eccDer,
                                word32 eccDerSize, int keyType)
{
    int               ret = 0;
    struct ecc_key    key;
    sss_object_t      newKey;
    sss_key_store_t   host_keystore;
    sss_status_t      status = kStatus_SSS_Success;
    int               keySizeBits = 0;
    int               keySize = 0;
    word32            idx = 0;
    sss_cipher_type_t curveType = kSSS_CipherType_NONE;
    sss_key_part_t    keyPart = kSSS_KeyPart_Pair;

    if (wolfSSL_CryptHwMutexLock() != 0) {
        return BAD_MUTEX_E;
    }

    /* Avoid key ID conflicts with temporary key storage */
    if (keyId >= SE050_KEYID_START) {
        return BAD_FUNC_ARG;
    }

    ret = wc_ecc_init(&key);
    if (ret != 0) {
        status = kStatus_SSS_Fail;
    } else {
        if (keyType == ECC_PUBLICKEY) {
            keyPart = kSSS_KeyPart_Public;
            ret = wc_EccPublicKeyDecode(eccDer, &idx, &key, eccDerSize);
        }
        else if (keyType == ECC_PRIVATEKEY) {
            keyPart = kSSS_KeyPart_Pair;
            ret = wc_EccPrivateKeyDecode(eccDer, &idx, &key, eccDerSize);
        }
        else {
            ret = BAD_FUNC_ARG;
        }

        if (ret != 0) {
            status = kStatus_SSS_Fail;
        }
    }

    if (status == kStatus_SSS_Success) {
        keySize = key.dp->size;
        ret = se050_map_curve(key.dp->id, keySize, &keySizeBits, &curveType);
        if (ret != 0) {
            status = kStatus_SSS_Fail;
        }
    }
    status = sss_key_store_context_init(&host_keystore, cfg_se050_i2c_pi);
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&newKey, &host_keystore);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_allocate_handle(&newKey, keyId,
            keyPart, curveType, MAX_ECC_BYTES,
            kKeyObject_Mode_Persistent);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_store_set_key(&host_keystore, &newKey, eccDer,
                                       eccDerSize, keySizeBits,
                                       NULL, 0);
    }
    wolfSSL_CryptHwMutexUnLock();

    wc_ecc_free(&key);
    if (status != kStatus_SSS_Success) {
        if (ret == 0)
            ret = WC_HW_E;
    }

    return ret;
}

/**
 * Insert ECC public key into SE050 at specified key ID.
 *
 * keyId       SE050 key ID to place public key into
 * eccDer      DER encoded ECC public key
 * eccDerSize  Size of eccDer, bytes
 *
 * Return 0 on success, negative on error.
 */
int wc_se050_ecc_insert_public_key(word32 keyId, const byte* eccDer,
                                   word32 eccDerSize)
{
    return se050_ecc_insert_key(keyId, eccDer, eccDerSize, ECC_PUBLICKEY);
}

/**
 * Insert ECC private key into SE050 at specified key ID.
 *
 * keyId       SE050 key ID to place private key into
 * eccDer      DER encoded ECC private key
 * eccDerSize  Size of eccDer, bytes
 *
 * Return 0 on success, negative on error.
 */
int wc_se050_ecc_insert_private_key(word32 keyId, const byte* eccDer,
                                    word32 eccDerSize)
{
    return se050_ecc_insert_key(keyId, eccDer, eccDerSize, ECC_PRIVATEKEY);
}

int se050_ecc_sign_hash_ex(const byte* in, word32 inLen, MATH_INT_T* r, MATH_INT_T* s,
                           byte* out, word32 *outLen, struct ecc_key* key)
{
    int                 ret = 0;
    sss_status_t        status;
    sss_asymmetric_t    ctx_asymm;
    sss_key_store_t     host_keystore;
    sss_object_t        newKey;
    sss_algorithm_t     algorithm;
    int                 keySize;
    int                 keySizeBits;
    int                 keyCreated = 0;
    word32              keyId;
    sss_cipher_type_t   curveType;

    byte sigBuf[ECC_MAX_SIG_SIZE];
    size_t sigSz = sizeof(sigBuf);
    word32 rLen = 0;
    word32 sLen = 0;

#ifdef SE050_DEBUG
    printf("se050_ecc_sign_hash_ex: key %p, in %p (%d), out %p (%d), "
            "keyId %d\n", key, in, inLen, out, *outLen, key->keyId);
#endif

    if (in == NULL || r == NULL || s == NULL || out == NULL ||
        outLen == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }

    if (cfg_se050_i2c_pi == NULL) {
        return WC_HW_E;
    }

    keySize = key->dp->size;
    ret = se050_map_curve(key->dp->id, keySize, &keySizeBits, &curveType);
    if (ret != 0) {
        return ret;
    }

    /* truncate if digest is larger than key size */
    if (inLen > (word32)keySize) {
        inLen = (word32)keySize;
    }

    /* For P-521, if inLen is 66, truncate down to 64 for SHA-512 */
    if ((keySize == 66) && (inLen == 66)) {
        inLen = 64;
    }

    algorithm = se050_map_hash_alg(inLen);
    if (algorithm == kAlgorithm_None) {
        inLen = keySize; /* try key size */
        algorithm = se050_map_hash_alg(inLen);
    }
    if (algorithm == kAlgorithm_None) {
        return ECC_CURVE_OID_E;
    }

    if (wolfSSL_CryptHwMutexLock() != 0) {
        return BAD_MUTEX_E;
    }

    status = sss_key_store_context_init(&host_keystore, cfg_se050_i2c_pi);
    if (status == kStatus_SSS_Success) {
        status = sss_key_store_allocate(&host_keystore, SE050_KEYSTOREID_ECC);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&newKey, &host_keystore);
    }
    /* this is run when a key was not generated and was instead passed in */
    if (status == kStatus_SSS_Success) {
        keyId = key->keyId;
        if (key->keyIdSet == 0) {
            byte derBuf[SE050_ECC_DER_MAX];
            word32 derSz;

            ret = wc_EccKeyToDer(key, derBuf, (word32)sizeof(derBuf));
            if (ret >= 0) {
                derSz = ret;
                ret = 0;
            }
            else {
                status = kStatus_SSS_Fail;
            }
            if (status == kStatus_SSS_Success) {
                keyId = se050_allocate_key(SE050_ECC_KEY);
                status = sss_key_object_allocate_handle(&newKey, keyId,
                    kSSS_KeyPart_Pair, curveType, keySize,
                    kKeyObject_Mode_Persistent);
            }
            if (status == kStatus_SSS_Success) {
                /* Try to delete existing key first, ignore return since will
                 * fail if no key exists yet */
                sss_key_store_erase_key(&host_keystore, &newKey);

                keyCreated = 1;
                status = sss_key_store_set_key(&host_keystore, &newKey, derBuf,
                                                derSz, keySizeBits, NULL, 0);
            }
        }
        else {
            status = sss_key_object_get_handle(&newKey, keyId);
        }
    }

    if (status == kStatus_SSS_Success) {

        status = sss_asymmetric_context_init(&ctx_asymm, cfg_se050_i2c_pi,
            &newKey, algorithm, kMode_SSS_Sign);
        if (status == kStatus_SSS_Success) {

            status = sss_asymmetric_sign_digest(&ctx_asymm, (uint8_t*)in, inLen,
                sigBuf, &sigSz);
            if (status == kStatus_SSS_Success) {

                /* SE050 returns ASN.1 encoded signature */
                rLen = keySize;
                sLen = keySize;

                ret = DecodeECC_DSA_Sig_Bin(sigBuf, (word32)sigSz,
                    out,         &rLen,
                    out+keySize, &sLen);

                if (ret != 0) {
                    status = kStatus_SSS_Fail;
                } else {
                    /* In case rLen is smaller than keySize, move S up */
                    XMEMCPY(out + rLen, out + keySize, sLen);
                }
            }
        }
        sss_asymmetric_context_free(&ctx_asymm);
    }

    if (status == kStatus_SSS_Success) {
        /* Load R and S into mp_int */
        ret = mp_read_unsigned_bin(r, out, rLen);
        if (ret == MP_OKAY) {
            ret = mp_read_unsigned_bin(s, out + rLen, sLen);
        }
        if (ret != MP_OKAY) {
            status = kStatus_SSS_Fail;
        }
        ret = 0;
    }

    if (status == kStatus_SSS_Success) {
        key->keyId = keyId;
        key->keyIdSet = 1;
        ret = 0;
    }
    else {
        if (keyCreated) {
            sss_key_store_erase_key(&host_keystore, &newKey);
            sss_key_object_free(&newKey);
        }
        if (ret == 0)
            ret = WC_HW_E;
    }

    wolfSSL_CryptHwMutexUnLock();

#ifdef SE050_DEBUG
    printf("se050_ecc_sign_hash_ex: ret %d, outLen %d\n", ret, *outLen);
#endif

    (void)outLen; /* caller sets outLen */

    return ret;
}

int se050_ecc_verify_hash_ex(const byte* hash, word32 hashLen, MATH_INT_T* r,
                             MATH_INT_T* s, struct ecc_key* key, int* res)
{
    int                 ret = 0;
    sss_status_t        status;
    sss_asymmetric_t    ctx_asymm;
    sss_object_t        newKey;
    sss_key_store_t     host_keystore;
    sss_algorithm_t     algorithm;
    int                 keyId;
    int                 keySize;
    int                 keySizeBits;
    sss_cipher_type_t   curveType;
    int                 keyCreated = 0;

    byte rBuf[ECC_MAX_CRYPTO_HW_SIZE];
    byte sBuf[ECC_MAX_CRYPTO_HW_SIZE];
    byte sigBuf[ECC_MAX_SIG_SIZE];
    word32 rBufSz = (word32)sizeof(rBuf);
    word32 sBufSz = (word32)sizeof(sBuf);
    word32 sigSz  = (word32)sizeof(sigBuf);

#ifdef SE050_DEBUG
    printf("se050_ecc_verify_hash_ex: key %p, hash %p (%d)\n",
        key, hash, hashLen);
#endif

    *res = 0;

    if (cfg_se050_i2c_pi == NULL) {
        return WC_HW_E;
    }

    keySize = key->dp->size;
    ret = se050_map_curve(key->dp->id, keySize, &keySizeBits, &curveType);
    if (ret != 0) {
        return ret;
    }

    /* truncate hash if larger than key size */
    if (hashLen > (word32)keySize) {
        hashLen = (word32)keySize;
    }

    /* For P-521, if inLen is 66, truncate down to 64 for SHA-512 */
    if ((keySize == 66) && (hashLen == 66)) {
        hashLen = 64;
    }

    algorithm = se050_map_hash_alg(hashLen);
    if (algorithm == kAlgorithm_None) {
        hashLen = keySize; /* try key size */
        algorithm = se050_map_hash_alg(hashLen);
    }
    if (algorithm == kAlgorithm_None) {
        return ECC_CURVE_OID_E;
    }

    if (wolfSSL_CryptHwMutexLock() != 0) {
        return BAD_MUTEX_E;
    }

    status = sss_key_store_context_init(&host_keystore, cfg_se050_i2c_pi);
    if (status == kStatus_SSS_Success) {
        status = sss_key_store_allocate(&host_keystore, SE050_KEYSTOREID_ECC);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&newKey, &host_keystore);
    }

    /* this is run when a key was not generated and was instead passed in */
    if (status == kStatus_SSS_Success) {
        keyId = key->keyId;
        if (key->keyIdSet == 0) {
            byte derBuf[SE050_ECC_DER_MAX];
            word32 derSz;

            ret = wc_EccPublicKeyToDer(key, derBuf, (word32)sizeof(derBuf), 1);
            if (ret >= 0) {
                derSz = ret;
                ret = 0;
            }
            else {
                status = kStatus_SSS_Fail;
            }
            if (status == kStatus_SSS_Success) {
                keyId = se050_allocate_key(SE050_ECC_KEY);
                status = sss_key_object_allocate_handle(&newKey, keyId,
                    kSSS_KeyPart_Public, curveType, keySize,
                    kKeyObject_Mode_Persistent);
            }
            if (status == kStatus_SSS_Success) {
                /* Try to delete existing key first, ignore return since will
                 * fail if no key exists yet */
                sss_key_store_erase_key(&host_keystore, &newKey);

                keyCreated = 1;
                status = sss_key_store_set_key(&host_keystore, &newKey, derBuf,
                                                derSz, keySizeBits, NULL, 0);
            }
        }
        else {
            status = sss_key_object_get_handle(&newKey, keyId);
        }
    }

    if (status == kStatus_SSS_Success) {
        status = sss_asymmetric_context_init(&ctx_asymm, cfg_se050_i2c_pi,
                                    &newKey, algorithm, kMode_SSS_Verify);

        if (status == kStatus_SSS_Success) {
            /* SE050 expects ASN.1 encoded signature */
            XMEMSET(rBuf, 0, sizeof(rBuf));
            XMEMSET(sBuf, 0, sizeof(sBuf));

            rBufSz = mp_unsigned_bin_size(r);
            sBufSz = mp_unsigned_bin_size(s);

            if (rBufSz > sizeof(rBuf) || sBufSz > sizeof(sBuf)) {
                WOLFSSL_MSG("Internal R/S buffers too small for signature");
                ret = BUFFER_E;
            }

            if (ret == 0) {
                ret = mp_to_unsigned_bin(r, rBuf);
                if (ret == MP_OKAY) {
                    ret = mp_to_unsigned_bin(s, sBuf);
                }
            }

            if (ret == 0) {
                ret = StoreECC_DSA_Sig_Bin(sigBuf, &sigSz, rBuf, rBufSz,
                                           sBuf, sBufSz);
            }

            if (ret == 0) {
                status = sss_asymmetric_verify_digest(&ctx_asymm,
                    (uint8_t*)hash, hashLen, sigBuf, sigSz);
            }
            else {
                status = kStatus_SSS_Fail;
            }
        }
        sss_asymmetric_context_free(&ctx_asymm);
    }

    if (status == kStatus_SSS_Success) {
        key->keyId = keyId;
        key->keyIdSet = 1;
        *res = 1;
        ret = 0;
    }
    else {
        if (keyCreated) {
            sss_key_store_erase_key(&host_keystore, &newKey);
            sss_key_object_free(&newKey);
        }
        if (ret == 0)
            ret = WC_HW_E;
    }

    wolfSSL_CryptHwMutexUnLock();

#ifdef SE050_DEBUG
    printf("se050_ecc_verify_hash_ex: key %p, ret %d, res %d\n",
        key, ret, *res);
#endif

    return ret;
}


void se050_ecc_free_key(struct ecc_key* key)
{
    sss_status_t    status = kStatus_SSS_Success;
    sss_object_t    keyObject;
    sss_key_store_t host_keystore;

#ifdef SE050_DEBUG
    printf("se050_ecc_free_key: key %p, keyId %d\n", key, key->keyId);
#endif

    if (cfg_se050_i2c_pi == NULL) {
        return;
    }
    if (key->keyIdSet == 0) {
        return;
    }

    if (wolfSSL_CryptHwMutexLock() != 0) {
        return;
    }

    status = sss_key_store_context_init(&host_keystore, cfg_se050_i2c_pi);
    if (status == kStatus_SSS_Success) {
        status = sss_key_store_allocate(&host_keystore, SE050_KEYSTOREID_ECC);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&keyObject, &host_keystore);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_get_handle(&keyObject, key->keyId);
    }
    if (status == kStatus_SSS_Success) {
        sss_key_object_free(&keyObject);
        key->keyId = 0;
        key->keyIdSet = 0;
    }
    wolfSSL_CryptHwMutexUnLock();
}

/**
 * Use specified SE050 key ID with this ecc_key struct.
 * Should be called by wc_ecc_use_key_id() for using pre-populated
 * SE050 keys.
 *
 * key   Pointer to initialized ecc_key structure
 * keyId SE050 key ID containing ECC key object
 *
 * Return 0 on success, negative on error.
 */
int se050_ecc_use_key_id(struct ecc_key* key, word32 keyId)
{
    int ret = 0;
    sss_object_t    keyObject;
    sss_key_store_t host_keystore;
    sss_status_t    status = kStatus_SSS_Success;
    uint8_t         derBuf[SE050_ECC_DER_MAX];
    size_t          derSz = sizeof(derBuf);
    size_t          derSzBits = 0;
    word32          idx = 0;

#ifdef SE050_DEBUG
    printf("se050_ecc_use_key_id: key %p, keyId %d\n", key, keyId);
#endif
    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    if (cfg_se050_i2c_pi == NULL) {
        return WC_HW_E;
    }

    if (wolfSSL_CryptHwMutexLock() != 0) {
        return BAD_MUTEX_E;
    }

    status = sss_key_store_context_init(&host_keystore, cfg_se050_i2c_pi);
    if (status == kStatus_SSS_Success) {
        status = sss_key_store_allocate(&host_keystore, SE050_KEYSTOREID_ECC);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&keyObject, &host_keystore);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_get_handle(&keyObject, keyId);
    }
    if (status == kStatus_SSS_Success) {
        derSzBits = derSz * 8;
        status = sss_key_store_get_key(&host_keystore, &keyObject,
            derBuf, &derSz, &derSzBits);
        (void)derSzBits; /* not used */
    }
    if (status == kStatus_SSS_Success) {
        ret = wc_EccPublicKeyDecode(derBuf, &idx, key, (word32)derSz);
        if (ret != 0) {
            status = kStatus_SSS_Fail;
        }
    }
    if (status == kStatus_SSS_Success) {
        key->keyId = keyId;
        key->keyIdSet = 1;
        ret = 0;
    }
    else if (ret == 0) {
        ret = WC_HW_E;
    }

    sss_key_object_free(&keyObject);

    wolfSSL_CryptHwMutexUnLock();

#ifdef SE050_DEBUG
    printf("se050_ecc_use_key_id: ret %d\n", ret);
#endif

    return ret;
}

/**
 * Get SE050 key ID associated with this ecc_key struct.
 * Should be called by wc_ecc_get_key_id() for the application to get
 * what key ID wolfCrypt picked for this ecc_key struct when generating
 * a key inside the SE050.
 *
 * key   Pointer to initialized ecc_key structure
 * keyId [OUT] SE050 key ID associated with this key structure
 *
 * Return 0 on success, negative on error.
 */
int se050_ecc_get_key_id(struct ecc_key* key, word32* keyId)
{
    int ret = 0;

    if (key == NULL || keyId == NULL) {
        return BAD_FUNC_ARG;
    }

    if (key->keyIdSet == 1) {
        *keyId = key->keyId;

    } else {
        WOLFSSL_MSG("SE050 key ID not set for ecc_key struct");
        ret = WC_HW_E;
    }

    return ret;
}

int se050_ecc_create_key(struct ecc_key* key, int curve_id, int keySize)
{
    int               ret = 0;
    sss_status_t      status = kStatus_SSS_Success;
    sss_object_t      keyPair;
    sss_key_store_t   host_keystore;
    uint8_t           derBuf[SE050_ECC_DER_MAX];
    size_t            derSz = sizeof(derBuf);
    word32            keyId;
    int               keySizeBits;
    sss_cipher_type_t curveType;
    int               keyCreated = 0;

#ifdef SE050_DEBUG
    printf("se050_ecc_create_key: key %p, curve %d, keySize %d\n",
        key, curve_id, keySize);
#endif

    if (cfg_se050_i2c_pi == NULL) {
        return WC_HW_E;
    }

    ret = se050_map_curve(curve_id, keySize, &keySizeBits, &curveType);
    if (ret != 0) {
        return ret;
    }

    if (wolfSSL_CryptHwMutexLock() != 0) {
        return BAD_MUTEX_E;
    }

    status = sss_key_store_context_init(&host_keystore, cfg_se050_i2c_pi);
    if (status == kStatus_SSS_Success) {
        status = sss_key_store_allocate(&host_keystore, SE050_KEYSTOREID_ECC);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&keyPair, &host_keystore);
    }
    if (status == kStatus_SSS_Success) {
        keyId = se050_allocate_key(SE050_ECC_KEY);
        /* Using Transient key type here does not work with SE050 */
        status = sss_key_object_allocate_handle(&keyPair, keyId,
            kSSS_KeyPart_Pair, curveType, keySize,
            kKeyObject_Mode_Persistent);
    }
    if (status == kStatus_SSS_Success) {
        /* Try to delete existing key first. Ignore return since will fail
         * if no key exists */
        sss_key_store_erase_key(&host_keystore, &keyPair);

        keyCreated = 1;
        status = sss_key_store_generate_key(&host_keystore, &keyPair,
            keySizeBits, NULL);
    }
    if (status == kStatus_SSS_Success) {
        size_t derSzBits = derSz * 8;
        status = sss_key_store_get_key(&host_keystore, &keyPair,
            derBuf, &derSz, &derSzBits);
        (void)derSzBits; /* not used */
    }
    if (status == kStatus_SSS_Success) {
        word32 idx = 0;
        ret = wc_EccPublicKeyDecode(derBuf, &idx, key, (word32)derSz);
        if (ret != 0) {
            status = kStatus_SSS_Fail;
        }
    }
    if (status == kStatus_SSS_Success) {
        key->keyId = keyId;
        key->keyIdSet = 1;
        ret = 0;
    }
    else {
        if (keyCreated) {
            sss_key_store_erase_key(&host_keystore, &keyPair);
            sss_key_object_free(&keyPair);
        }
        if (ret == 0)
            ret = WC_HW_E;
    }

    wolfSSL_CryptHwMutexUnLock();

#ifdef SE050_DEBUG
    printf("se050_ecc_create_key: key %p, ret %d, keyId %d\n",
        key, ret, key->keyId);
#endif

    return ret;
}


int se050_ecc_shared_secret(ecc_key* private_key, ecc_key* public_key,
    byte* out, word32* outlen)
{
    int                 ret;
    sss_status_t        status = kStatus_SSS_Success;
    sss_key_store_t     host_keystore;
    sss_object_t        ref_private_key;
    sss_object_t        ref_public_key;
    sss_object_t        deriveKey;
    sss_derive_key_t    ctx_derive_key;
    word32              keyId;
    int                 keySize;
    int                 keySizeBits;
    sss_cipher_type_t   curveType;
    int                 keyCreated = 0;
    int                 deriveKeyCreated = 0;

#ifdef SE050_DEBUG
    printf("se050_ecc_shared_secret: priv %p, pub %p, out %p (%d)\n",
        private_key, public_key, out, *outlen);
#endif

    if (cfg_se050_i2c_pi == NULL) {
        return WC_HW_E;
    }
    if (private_key == NULL || public_key == NULL ||
        private_key->keyIdSet == 0) {
        return BAD_FUNC_ARG;
    }

    keySize = private_key->dp->size;
    ret = se050_map_curve(private_key->dp->id, keySize, &keySizeBits,
                          &curveType);
    if (ret != 0) {
        return ret;
    }

    if (wolfSSL_CryptHwMutexLock() != 0) {
        return BAD_MUTEX_E;
    }

    status = sss_key_store_context_init(&host_keystore, cfg_se050_i2c_pi);
    if (status == kStatus_SSS_Success) {
        status = sss_key_store_allocate(&host_keystore, SE050_KEYSTOREID_ECC);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&ref_private_key, &host_keystore);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_get_handle(&ref_private_key,
                                           private_key->keyId);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&ref_public_key, &host_keystore);
    }
    if (status == kStatus_SSS_Success) {
        keyId = public_key->keyId;
        if (public_key->keyIdSet == 0) {
            byte derBuf[SE050_ECC_DER_MAX];
            word32 derSz;

            ret = wc_EccPublicKeyToDer(public_key, derBuf,
                (word32)sizeof(derBuf), 1);
            if (ret >= 0) {
                derSz = ret;
                ret = 0;
            }
            else {
                status = kStatus_SSS_Fail;
            }
            if (status == kStatus_SSS_Success) {
                keyId = se050_allocate_key(SE050_ECC_KEY);
                status = sss_key_object_allocate_handle(&ref_public_key,
                    keyId, kSSS_KeyPart_Public, curveType, keySize,
                    kKeyObject_Mode_Persistent);
            }
            if (status == kStatus_SSS_Success) {
                /* Try to delete existing key first, ignore return since will
                 * fail if no key exists yet */
                sss_key_store_erase_key(&host_keystore, &ref_public_key);
                status = sss_key_store_set_key(&host_keystore, &ref_public_key,
                    derBuf, derSz, keySizeBits, NULL, 0);
                keyCreated = 1;
            }
        }
        else {
            status = sss_key_object_get_handle(&ref_public_key, keyId);
        }
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&deriveKey, &host_keystore);
    }
    if (status == kStatus_SSS_Success) {
        word32 keyIdAes = se050_allocate_key(SE050_AES_KEY);
        status = sss_key_object_allocate_handle(&deriveKey,
            keyIdAes,
            kSSS_KeyPart_Default,
            kSSS_CipherType_Binary,
            keySize,
            kKeyObject_Mode_Transient);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_derive_key_context_init(&ctx_derive_key, cfg_se050_i2c_pi,
                                    &ref_private_key, kAlgorithm_SSS_ECDH,
                                    kMode_SSS_ComputeSharedSecret);
        if (status == kStatus_SSS_Success) {
            /* Try to delete existing key first, ignore return since will
             * fail if no key exists yet */
            sss_key_store_erase_key(&host_keystore, &deriveKey);
            status = sss_derive_key_dh(&ctx_derive_key, &ref_public_key,
                &deriveKey);
        }
        if (status == kStatus_SSS_Success) {
            size_t outlenSz = (size_t)*outlen;
            size_t outlenSzBits = outlenSz * 8;
            deriveKeyCreated = 1;
            /* derived key export */
            status = sss_key_store_get_key(&host_keystore, &deriveKey,
                out, &outlenSz, &outlenSzBits);
            *outlen = (word32)outlenSz;
            (void)outlenSzBits; /* not used */
        }

        sss_derive_key_context_free(&ctx_derive_key);
    }
    if (deriveKeyCreated) {
        sss_key_store_erase_key(&host_keystore, &deriveKey);
        sss_key_object_free(&deriveKey);
    }

    if (status == kStatus_SSS_Success) {
        public_key->keyId = keyId;
        public_key->keyIdSet = 1;
        ret = 0;
    }
    else {
        if (keyCreated) {
            sss_key_store_erase_key(&host_keystore, &ref_public_key);
            sss_key_object_free(&ref_public_key);
        }
        if (ret == 0) {
            ret = WC_HW_E;
        }
    }

    wolfSSL_CryptHwMutexUnLock();

#ifdef SE050_DEBUG
    printf("se050_ecc_shared_secret: ret %d, outlen %d\n", ret, *outlen);
#endif

    return ret;
}
#endif /* HAVE_ECC */

#ifdef HAVE_ED25519

int se050_ed25519_create_key(ed25519_key* key)
{
    int             ret = 0;
    sss_status_t    status;
    sss_key_store_t host_keystore;
    sss_object_t    newKey;
    word32          keyId;
    int             keySize = ED25519_KEY_SIZE;
    int             keyCreated = 0;

#ifdef SE050_DEBUG
    printf("se050_ed25519_create_key: %p\n", key);
#endif

    if (cfg_se050_i2c_pi == NULL) {
        return WC_HW_E;
    }

    if (wolfSSL_CryptHwMutexLock() != 0) {
        return BAD_MUTEX_E;
    }

    status = sss_key_store_context_init(&host_keystore, cfg_se050_i2c_pi);
    if (status == kStatus_SSS_Success) {
        status = sss_key_store_allocate(&host_keystore, SE050_KEYSTOREID_ED25519);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&newKey, &host_keystore);
    }
    if (status == kStatus_SSS_Success) {
        keyId = se050_allocate_key(SE050_ED25519_KEY);
        status = sss_key_object_allocate_handle(&newKey, keyId,
            kSSS_KeyPart_Pair, kSSS_CipherType_EC_TWISTED_ED, keySize,
            kKeyObject_Mode_Transient);
    }
    if (status == kStatus_SSS_Success) {
        keyCreated = 1;
        status = sss_key_store_generate_key(&host_keystore, &newKey,
            keySize * 8, NULL);
    }

    if (status == kStatus_SSS_Success) {
        key->keyId = keyId;
        key->keyIdSet = 1;
        ret = 0;
    }
    else {
        if (keyCreated) {
            sss_key_store_erase_key(&host_keystore, &newKey);
            sss_key_object_free(&newKey);
        }
        ret = WC_HW_E;
    }

    wolfSSL_CryptHwMutexUnLock();

#ifdef SE050_DEBUG
    printf("se050_ed25519_create_key: ret %d, keyId %ld\n", ret, key->keyId);
#endif

    return ret;
}

void se050_ed25519_free_key(ed25519_key* key)
{
    sss_status_t status;
    sss_object_t newKey;
    sss_key_store_t host_keystore;

#ifdef SE050_DEBUG
    printf("se050_ed25519_free_key: %p, id %ld\n", key, key->keyId);
#endif

    if (cfg_se050_i2c_pi == NULL) {
        return;
    }
    if (key->keyIdSet == 0) {
        return;
    }

    if (wolfSSL_CryptHwMutexLock() != 0) {
        return;
    }

    status = sss_key_store_context_init(&host_keystore, cfg_se050_i2c_pi);

    if (status == kStatus_SSS_Success) {
        status = sss_key_store_allocate(&host_keystore, SE050_KEYSTOREID_ED25519);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&newKey, &host_keystore);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_get_handle(&newKey, key->keyId);
    }
    if (status == kStatus_SSS_Success) {
        sss_key_object_free(&newKey);
        key->keyId = 0;
        key->keyIdSet = 0;
    }
    wolfSSL_CryptHwMutexUnLock();
}

int se050_ed25519_sign_msg(const byte* in, word32 inLen, byte* out,
                         word32 *outLen, ed25519_key* key)
{
    int                 ret = 0;
    sss_status_t        status = kStatus_SSS_Success;
    sss_asymmetric_t    ctx_asymm;
    sss_key_store_t     host_keystore;
    sss_object_t        newKey;
    int                 keySize = ED25519_KEY_SIZE;
    int                 keyCreated = 0;
    word32              keyId;

#ifdef SE050_DEBUG
    printf("se050_ed25519_sign_msg: key %p, in %p (%d), out %p (%d), "
            "keyId %ld\n", key, in, inLen, out, *outLen, key->keyId);
#endif

    if (cfg_se050_i2c_pi == NULL) {
        return WC_HW_E;
    }

    if (wolfSSL_CryptHwMutexLock() != 0) {
        return BAD_MUTEX_E;
    }

    status = sss_key_store_context_init(&host_keystore, cfg_se050_i2c_pi);
    if (status == kStatus_SSS_Success) {
        status = sss_key_store_allocate(&host_keystore, SE050_KEYSTOREID_ED25519);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&newKey, &host_keystore);
    }
    /* this is run when a key was not generated and was instead passed in */
    if (status == kStatus_SSS_Success) {
        keyId = key->keyId;
        if (key->keyIdSet == 0) {
            byte derBuf[SE050_ECC_DER_MAX];
            word32 derSz;

            ret = wc_Ed25519KeyToDer(key, derBuf, (word32)sizeof(derBuf));
            if (ret >= 0) {
                derSz = ret;
                ret = 0;
            }
            else {
                status = kStatus_SSS_Fail;
            }
            if (status == kStatus_SSS_Success) {
                keyId = se050_allocate_key(SE050_ED25519_KEY);
                status = sss_key_object_allocate_handle(&newKey, keyId,
                    kSSS_KeyPart_Pair, kSSS_CipherType_EC_TWISTED_ED, keySize,
                    kKeyObject_Mode_Transient);
            }
            if (status == kStatus_SSS_Success) {
                keyCreated = 1;
                status = sss_key_store_set_key(&host_keystore, &newKey, derBuf,
                                                derSz, keySize * 8, NULL, 0);
            }
        }
        else {
            status = sss_key_object_get_handle(&newKey, keyId);
        }
    }
    if (status == kStatus_SSS_Success) {
        status = sss_asymmetric_context_init(&ctx_asymm, cfg_se050_i2c_pi,
                            &newKey, kAlgorithm_SSS_SHA512, kMode_SSS_Sign);
        if (status == kStatus_SSS_Success) {
            size_t outlenSz = (size_t)*outLen;
            status = sss_se05x_asymmetric_sign(
                    (sss_se05x_asymmetric_t *)&ctx_asymm,
                    (uint8_t *)in, inLen, out, &outlenSz);
            *outLen = (word32)outlenSz;
        }

        sss_asymmetric_context_free(&ctx_asymm);
    }

    if (status != kStatus_SSS_Success) {
        if (keyCreated) {
            sss_key_store_erase_key(&host_keystore, &newKey);
            sss_key_object_free(&newKey);
        }
        ret = WC_HW_E;
    } else {
        key->keyId = keyId;
        key->keyIdSet = 1;
    }

    wolfSSL_CryptHwMutexUnLock();

#ifdef SE050_DEBUG
    printf("se050_ed25519_sign_msg: ret %d, outLen %d\n", ret, *outLen);
#endif

    return ret;
}

int se050_ed25519_verify_msg(const byte* signature, word32 signatureLen,
    const byte* msg, word32 msgLen, struct ed25519_key* key, int* res)
{
    int                 ret = 0;
    sss_status_t        status = kStatus_SSS_Success;
    sss_asymmetric_t    ctx_asymm;
    sss_object_t        newKey;
    sss_key_store_t     host_keystore;
    word32              keyId;
    int                 keySize = ED25519_KEY_SIZE;
    int                 keyCreated = 0;

#ifdef SE050_DEBUG
    printf("se050_ed25519_verify_msg: key %p, sig %p (%d), msg %p (%d)\n",
        key, signature, signatureLen, msg, msgLen);
#endif

    if (cfg_se050_i2c_pi == NULL) {
        return WC_HW_E;
    }

    if (wolfSSL_CryptHwMutexLock() != 0) {
        return BAD_MUTEX_E;
    }

    status = sss_key_store_context_init(&host_keystore, cfg_se050_i2c_pi);
    if (status == kStatus_SSS_Success) {
        status = sss_key_store_allocate(&host_keystore,
                                        SE050_KEYSTOREID_ED25519);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&newKey, &host_keystore);
    }
    if (status == kStatus_SSS_Success) {
        keyId = key->keyId;
        if (key->keyIdSet == 0) {
            byte derBuf[ED25519_PUB_KEY_SIZE + 12]; /* seq + algo + bitstring */
            word32 derSz = 0;

            ret = wc_Ed25519PublicKeyToDer(key, derBuf,
                                           (word32)sizeof(derBuf), 1);
            if (ret >= 0) {
                derSz = ret;
                ret = 0;
            }
            else {
                status = kStatus_SSS_Fail;
            }
            if (status == kStatus_SSS_Success) {
                keyId = se050_allocate_key(SE050_ED25519_KEY);
                status = sss_key_object_allocate_handle(&newKey, keyId,
                    kSSS_KeyPart_Public, kSSS_CipherType_EC_TWISTED_ED, keySize,
                    kKeyObject_Mode_Transient);
            }
            if (status == kStatus_SSS_Success) {
                keyCreated = 1;
                status = sss_key_store_set_key(&host_keystore, &newKey, derBuf,
                                                derSz, keySize * 8, NULL, 0);
            }
        }
        else {
            status = sss_key_object_get_handle(&newKey, keyId);
        }
    }

    if (status == kStatus_SSS_Success) {
        status = sss_asymmetric_context_init(&ctx_asymm, cfg_se050_i2c_pi,
                    &newKey, kAlgorithm_SSS_SHA512, kMode_SSS_Verify);
        if (status == kStatus_SSS_Success) {
            status = sss_se05x_asymmetric_verify(
                    (sss_se05x_asymmetric_t*)&ctx_asymm, (uint8_t*)msg, msgLen,
                    (uint8_t*)signature, (size_t)signatureLen);
        }
        sss_asymmetric_context_free(&ctx_asymm);
    }

    if (status == kStatus_SSS_Success) {
        key->keyId = keyId;
        key->keyIdSet = 1;
        *res = 1;
        ret = 0;
    }
    else {
        if (keyCreated) {
            sss_key_store_erase_key(&host_keystore, &newKey);
            sss_key_object_free(&newKey);
        }
        if (ret == 0)
            ret = WC_HW_E;
    }

    wolfSSL_CryptHwMutexUnLock();

#ifdef SE050_DEBUG
    printf("se050_ed25519_verify_msg: ret %d, res %d\n", ret, *res);
#endif

    return ret;
}

#endif /* HAVE_ED25519 */


#ifdef HAVE_CURVE25519

int se050_curve25519_create_key(curve25519_key* key, int keySize)
{
    int             ret;
    sss_status_t    status = kStatus_SSS_Success;
    sss_object_t    keyPair;
    sss_key_store_t host_keystore;
    uint8_t         derBuf[SE050_ECC_DER_MAX];
    size_t          derSz = sizeof(derBuf);
    word32          keyId;
    int             keyCreated = 0;

#ifdef SE050_DEBUG
    printf("se050_curve25519_create_key: key %p, keySize %d\n",
        key, keySize);
#endif

    if (cfg_se050_i2c_pi == NULL) {
        return WC_HW_E;
    }
    if (wolfSSL_CryptHwMutexLock() != 0) {
        return BAD_MUTEX_E;
    }

    status = sss_key_store_context_init(&host_keystore, cfg_se050_i2c_pi);
    if (status == kStatus_SSS_Success) {
        status = sss_key_store_allocate(&host_keystore,
            SE050_KEYSTOREID_CURVE25519);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&keyPair, &host_keystore);
    }
    if (status == kStatus_SSS_Success) {
        keyId = se050_allocate_key(SE050_CURVE25519_KEY);
        status = sss_key_object_allocate_handle(&keyPair, keyId,
            kSSS_KeyPart_Pair, kSSS_CipherType_EC_MONTGOMERY, keySize,
            kKeyObject_Mode_None);
    }
    if (status == kStatus_SSS_Success) {
        keyCreated = 1;
        status = sss_key_store_generate_key(&host_keystore, &keyPair,
            keySize * 8, NULL);
    }
    if (status == kStatus_SSS_Success) {
        size_t derSzBits = derSz * 8;
        status = sss_key_store_get_key(&host_keystore, &keyPair,
            derBuf, &derSz, &derSzBits);
        (void)derSzBits; /* not used */
    }
    if (status == kStatus_SSS_Success) {
        word32 idx = 0;
        byte   pubKey[CURVE25519_KEYSIZE];
        word32 pubKeyLen = (word32)sizeof(pubKey);

        ret = DecodeAsymKeyPublic(derBuf, &idx, (word32)derSz,
            pubKey, &pubKeyLen, X25519k);
        if (ret == 0) {
            ret = wc_curve25519_import_public_ex(pubKey, pubKeyLen, key,
                EC25519_LITTLE_ENDIAN);
        }
        if (ret != 0) {
            status = kStatus_SSS_Fail;
        }
    }

    if (status == kStatus_SSS_Success) {
        key->keyId = keyId;
        key->keyIdSet = 1;
        ret = 0;
    }
    else {
        if (keyCreated) {
            sss_key_store_erase_key(&host_keystore, &keyPair);
            sss_key_object_free(&keyPair);
        }
        ret = WC_HW_E;
    }
    wolfSSL_CryptHwMutexUnLock();

#ifdef SE050_DEBUG
    printf("se050_curve25519_create_key: key %p, ret %d, keyId %ld\n",
        key, ret, key->keyId);
#endif

    return ret;
}

int se050_curve25519_shared_secret(curve25519_key* private_key,
    curve25519_key* public_key, ECPoint* out)
{
    int               ret = 0;
    sss_status_t      status = kStatus_SSS_Success;
    sss_key_store_t   host_keystore;
    sss_object_t      ref_private_key;
    sss_object_t      ref_public_key;
    sss_object_t      deriveKey;
    sss_derive_key_t  ctx_derive_key;
    word32            keyId;
    int               keySize = CURVE25519_KEYSIZE;
    int               keyCreated = 0;
    int               deriveKeyCreated = 0;

#ifdef SE050_DEBUG
    printf("se050_curve25519_shared_secret: priv %p, pub %p, out %p (%d)\n",
        private_key, public_key, out, out->pointSz);
#endif

    if (cfg_se050_i2c_pi == NULL) {
        return WC_HW_E;
    }

    if (private_key == NULL || public_key == NULL ||
        private_key->keyIdSet == 0) {
        return BAD_FUNC_ARG;
    }

    if (wolfSSL_CryptHwMutexLock() != 0) {
        return BAD_MUTEX_E;
    }

    status = sss_key_store_context_init(&host_keystore, cfg_se050_i2c_pi);
    if (status == kStatus_SSS_Success) {
        status = sss_key_store_allocate(&host_keystore,
            SE050_KEYSTOREID_CURVE25519);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&ref_private_key, &host_keystore);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_get_handle(&ref_private_key,
                                           private_key->keyId);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&ref_public_key, &host_keystore);
    }
    if (status == kStatus_SSS_Success) {
        keyId = public_key->keyId;
        if (public_key->keyIdSet == 0) {
            byte   derBuf[CURVE25519_PUB_KEY_SIZE + 12]; /* seq + algo + bitstring */
            word32 derSz;
            byte   pubKey[CURVE25519_PUB_KEY_SIZE];
            word32 pubKeyLen = (word32)sizeof(pubKey);

            ret = wc_curve25519_export_public_ex(public_key, pubKey, &pubKeyLen,
                EC25519_LITTLE_ENDIAN);
            if (ret == 0) {
                ret = SetAsymKeyDerPublic(pubKey, pubKeyLen, derBuf,
                    (word32)sizeof(derBuf), X25519k, 1);
                if (ret >= 0) {
                    derSz = ret;
                    ret = 0;
                }
            }
            if (ret != 0) {
                status = kStatus_SSS_Fail;
            }
            if (status == kStatus_SSS_Success) {
                keyId = se050_allocate_key(SE050_CURVE25519_KEY);
                status = sss_key_object_allocate_handle(&ref_public_key,
                    keyId, kSSS_KeyPart_Public, kSSS_CipherType_EC_MONTGOMERY,
                    keySize, kKeyObject_Mode_Transient);
            }
            if (status == kStatus_SSS_Success) {
                keyCreated = 1;
                status = sss_key_store_set_key(&host_keystore, &ref_public_key,
                    derBuf, derSz, keySize * 8, NULL, 0);
            }
        }
        else {
            status = sss_key_object_get_handle(&ref_public_key, keyId);
        }
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&deriveKey, &host_keystore);
    }
    if (status == kStatus_SSS_Success) {
        word32 keyIdAes = se050_allocate_key(SE050_AES_KEY);
        deriveKeyCreated = 1;
        status = sss_key_object_allocate_handle(&deriveKey,
            keyIdAes,
            kSSS_KeyPart_Default,
            kSSS_CipherType_Binary,
            keySize,
            kKeyObject_Mode_Transient);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_derive_key_context_init(&ctx_derive_key, cfg_se050_i2c_pi,
                                    &ref_private_key, kAlgorithm_SSS_ECDH,
                                    kMode_SSS_ComputeSharedSecret);
        if (status == kStatus_SSS_Success) {
            status = sss_derive_key_dh(&ctx_derive_key, &ref_public_key,
                &deriveKey);
        }
        if (status == kStatus_SSS_Success) {
            size_t outlenSz = sizeof(out->point);
            size_t outlenSzBits = outlenSz * 8;
            /* derived key export */
            status = sss_key_store_get_key(&host_keystore, &deriveKey,
                out->point, &outlenSz, &outlenSzBits);
            out->pointSz = (word32)outlenSz;
            (void)outlenSzBits; /* not used */
        }

        sss_derive_key_context_free(&ctx_derive_key);
    }
    if (deriveKeyCreated) {
        sss_key_store_erase_key(&host_keystore, &deriveKey);
        sss_key_object_free(&deriveKey);
    }

    if (status == kStatus_SSS_Success) {
        public_key->keyId = keyId;
        public_key->keyIdSet = 1;
        ret = 0;
    }
    else {
        if (keyCreated) {
            sss_key_store_erase_key(&host_keystore, &ref_public_key);
            sss_key_object_free(&ref_public_key);
        }
        if (ret == 0)
            ret = WC_HW_E;
    }

    wolfSSL_CryptHwMutexUnLock();

#ifdef SE050_DEBUG
    printf("se050_curve25519_shared_secret: ret %d, outlen %d\n",
        ret, out->pointSz);
#endif

    return ret;
}

void se050_curve25519_free_key(struct curve25519_key* key)
{
    sss_status_t status;
    sss_object_t newKey;
    sss_key_store_t host_keystore;

#ifdef SE050_DEBUG
    printf("se050_curve25519_free_key: %p, id %ld\n", key, key->keyId);
#endif

    if (cfg_se050_i2c_pi == NULL || key->keyIdSet == 0) {
        return;
    }

    if (wolfSSL_CryptHwMutexLock() != 0) {
        return;
    }

    status = sss_key_store_context_init(&host_keystore, cfg_se050_i2c_pi);

    if (status == kStatus_SSS_Success) {
        status = sss_key_store_allocate(&host_keystore,
            SE050_KEYSTOREID_CURVE25519);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_init(&newKey, &host_keystore);
    }
    if (status == kStatus_SSS_Success) {
        status = sss_key_object_get_handle(&newKey, key->keyId);
    }
    if (status == kStatus_SSS_Success) {
        sss_key_object_free(&newKey);
        key->keyId = 0;
        key->keyIdSet = 0;
    }
    wolfSSL_CryptHwMutexUnLock();
}
#endif /* HAVE_CURVE25519 */

#endif /* WOLFSSL_SE050 */
