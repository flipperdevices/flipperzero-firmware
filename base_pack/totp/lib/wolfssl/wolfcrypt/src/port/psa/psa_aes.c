/* psa_aes.c
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
#if !defined(WOLFSSL_PSA_NO_AES)
#if !defined(NO_AES)

#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/port/psa/psa.h>
#include <wolfssl/wolfcrypt/logging.h>

#ifndef NO_INLINE
#define WOLFSSL_MISC_INCLUDED
#include <wolfcrypt/src/misc.c>
#else
#include <wolfssl/wolfcrypt/misc.h>
#endif

static int wc_psa_aes_import_key(Aes *aes, const uint8_t *key,
                                 size_t key_length, psa_algorithm_t alg,
                                 int dir)
{
    psa_key_attributes_t key_attr;
    psa_key_id_t id;
    psa_status_t s;

    XMEMSET(&key_attr, 0, sizeof(key_attr));
    aes->key_id = 0;
    aes->ctx_initialized = 0;

    psa_set_key_type(&key_attr, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&key_attr, key_length * 8);
    psa_set_key_usage_flags(&key_attr,
                            dir == AES_ENCRYPTION ? PSA_KEY_USAGE_ENCRYPT :
                            dir == AES_DECRYPTION ? PSA_KEY_USAGE_DECRYPT : 0);
    psa_set_key_algorithm(&key_attr, alg);

    PSA_LOCK();
    s = psa_import_key(&key_attr, key, key_length, &id);
    PSA_UNLOCK();
    if (s != PSA_SUCCESS)
        return WC_HW_E;

    aes->key_id = id;
    aes->key_need_importing = 0;

    return 0;
}

/**
 * wc_psa_aes_init() - init @aes PSA resources
 * @aes: Aes object
 */
int wc_psa_aes_init(Aes *aes)
{
    aes->key_id = 0;
    aes->ctx_initialized = 0;
    aes->key_need_importing = 0;
    XMEMSET(&aes->psa_ctx, 0, sizeof(aes->psa_ctx));
    return 0;
}

/**
 * wc_psa_aes_get_key_size() - get the size of the key in @aes
 * @aes: Aes object
 * @keySize: where to store the size of the key
 *
 * returns: 0 on success
 */
int wc_psa_aes_get_key_size(Aes *aes, word32 *keySize)
{
    psa_key_attributes_t attr;
    psa_status_t s;

    if (aes->key_need_importing == 1) {
        *keySize = aes->keylen;
        return 0;
    }

    if (aes->key_id == PSA_KEY_ID_NULL)
        return BAD_FUNC_ARG;

    PSA_LOCK();
    s = psa_get_key_attributes(aes->key_id, &attr);
    PSA_UNLOCK();
    if (s != PSA_SUCCESS)
        return WC_HW_E;

    *keySize = (word32)(psa_get_key_bits(&attr) / 8);
    PSA_LOCK();
    psa_reset_key_attributes(&attr);
    PSA_UNLOCK();
    return 0;
}

/**
 * wc_psa_aes_set_key() - set key / iv to object *aes
 * @aes: object to set the key into
 * @key: key to import
 * @key_length: size of the key in bytes
 * @iv: IV (can be null)
 * @alg: algorithm (mode) to use with this key (can be 0)
 * @dir: direction to use with this key
 *
 *
 * NOTE: if we don't know the  mode or the direction (@alg == 0) the key
 * import operation will be delayed until the first wc_psa_aes_encrypt_decrypt()
 * invocation. In this case the key is temporary stored inside the AES
 * object. Indeed PSA requires that the mode of operation is already known when
 * importing a new key.
 *
 * returns: 0 on success, WC_HW_E on PSA error
*/
int wc_psa_aes_set_key(Aes *aes, const uint8_t *key, size_t key_length,
                       uint8_t *iv,  psa_algorithm_t alg, int dir)
{
    psa_status_t s;
    int ret;

    /* the object was already used for other encryption. Reset the context */
    if (aes->ctx_initialized) {
        PSA_LOCK();
        s = psa_cipher_abort(&aes->psa_ctx);
        PSA_UNLOCK();
        if (s != PSA_SUCCESS)
            return WC_HW_E;

        aes->ctx_initialized = 0;
    }

    /* a key was already imported, destroy it first */
    if (aes->key_id != PSA_KEY_ID_NULL) {
        PSA_LOCK();
        psa_destroy_key(aes->key_id);
        PSA_UNLOCK();

        aes->key_id = PSA_KEY_ID_NULL;
    }

    /* we have been invoked from a generic wcSetKey. We don't know the mode that
       will be used, so we can't import the key in PSA yet. Let's copy the key
       inside the object, we will import it when we'll know the cipher mode */
    if (alg == PSA_ALG_NONE) {
        XMEMCPY(aes->key, key, key_length);
        aes->key_need_importing = 1;
    } else {
        PSA_LOCK();
        ret = wc_psa_aes_import_key(aes, key, key_length, alg, dir);
        PSA_UNLOCK();
        if (ret != 0)
            return ret;
    }

    return wc_AesSetIV(aes, iv);
}

/**
 * wc_psa_aes_encrypt_decrypt() - do an encrypt/decrypt step
 * @aes: Aes object
 * @input: input data
 * @output: where to store the result of the operation
 * @length: size of the input data
 * @alg: algorithm (mode) to use in the operation
 * @direction: either @AES_ENCRYPT or @AES_DECRYPT
 *
 * returns:
 * 0 on success
 * BAD_FUNC_ARG for bad argument
 * WC_HW_E for PSA error
 */
int wc_psa_aes_encrypt_decrypt(Aes *aes, const uint8_t *input,
                               uint8_t *output, size_t length,
                               psa_algorithm_t alg, int direction)
{
    size_t output_length;
    psa_status_t s;
    int r;

    if (aes == NULL || input == NULL || output == NULL)
        return BAD_FUNC_ARG;

    /* This is the first time we invoke encrypt/decrypt */
    if (aes->ctx_initialized == 0) {

        /* import the key */
        if (aes->key_need_importing == 1) {
            r = wc_psa_aes_import_key(aes, (uint8_t*)aes->key, aes->keylen,
                                      alg, direction);
            if (r != 0)
                return r;

            ForceZero(aes->key, aes->keylen);
            aes->key_need_importing = 0;
            aes->keylen = 0;
        }

        if (direction == AES_ENCRYPTION) {
            PSA_LOCK();
            s = psa_cipher_encrypt_setup(&aes->psa_ctx, aes->key_id, alg);
            PSA_UNLOCK();
        } else {
            PSA_LOCK();
            s = psa_cipher_decrypt_setup(&aes->psa_ctx, aes->key_id, alg);
            PSA_UNLOCK();
        }

        if (s != PSA_SUCCESS)
            goto err;

        aes->ctx_initialized = 1;

        /* ECB doesn't use IV */
        if (alg != PSA_ALG_ECB_NO_PADDING) {

            /* wc_SetIV stores the IV in reg */
            PSA_LOCK();
            s = psa_cipher_set_iv(&aes->psa_ctx,
                                  (uint8_t*)aes->reg, AES_IV_SIZE);
            PSA_UNLOCK();

            if (s != PSA_SUCCESS)
                goto err;
        }

    }

    PSA_LOCK();
    s = psa_cipher_update(&aes->psa_ctx, input,
                          length, output, length, &output_length);
    PSA_UNLOCK();
    if (s != PSA_SUCCESS)
        goto err;

    if (output_length != length)
        goto err;

    return 0;

 err:
    wc_psa_aes_free(aes);
    return WC_HW_E;
 }

/**
 * wc_psa_aes_free() - PSA cipher cleanup
 * @aes: the Aes object to cleanup
 */
int wc_psa_aes_free(Aes *aes)
{
    if (aes->key_id != PSA_KEY_ID_NULL) {
        PSA_LOCK();
        psa_destroy_key(aes->key_id);
        PSA_UNLOCK();
        aes->key_id = PSA_KEY_ID_NULL;
    }

    if (aes->ctx_initialized == 1) {
        PSA_LOCK();
        psa_cipher_abort(&aes->psa_ctx);
        PSA_UNLOCK();
        aes->ctx_initialized = 0;
    }

    aes->ctx_initialized = 0;
    aes->key_need_importing = 0;

    return 0;
}

int wc_AesEncrypt(Aes *aes, const byte *inBlock, byte *outBlock)
{
    return wc_psa_aes_encrypt_decrypt(aes, inBlock, outBlock,
                                      AES_BLOCK_SIZE, PSA_ALG_ECB_NO_PADDING,
                                      AES_ENCRYPTION);
}

#if defined(HAVE_AES_DECRYPT)
int wc_AesDecrypt(Aes *aes, const byte *inBlock, byte *outBlock)
{
    return wc_psa_aes_encrypt_decrypt(aes, inBlock, outBlock,
                                      AES_BLOCK_SIZE, PSA_ALG_ECB_NO_PADDING,
                                      AES_DECRYPTION);
}
#endif

#if defined(WOLFSSL_AES_COUNTER)

int wc_AesCtrEncrypt(Aes *aes, byte *out, const byte *in, word32 sz)
{
    return wc_psa_aes_encrypt_decrypt(aes, in, out, sz, PSA_ALG_CTR,
                                      AES_ENCRYPTION);
}
#endif

#if defined (HAVE_AES_CBC)
int wc_AesCbcEncrypt(Aes *aes, byte *out, const byte *in, word32 sz)
{

    if (sz % AES_BLOCK_SIZE != 0)
#if defined (WOLFSSL_AES_CBC_LENGTH_CHECKS)
        return BAD_LENGTH_E;
#else
        return BAD_FUNC_ARG;
#endif

    return wc_psa_aes_encrypt_decrypt(aes, in, out, sz,
                                      PSA_ALG_CBC_NO_PADDING,
                                      AES_ENCRYPTION);
}

int wc_AesCbcDecrypt(Aes *aes, byte *out, const byte *in, word32 sz)
{

    if (sz % AES_BLOCK_SIZE != 0)
#if defined (WOLFSSL_AES_CBC_LENGTH_CHECKS)
        return BAD_LENGTH_E;
#else
        return BAD_FUNC_ARG;
#endif

    return wc_psa_aes_encrypt_decrypt(aes, in, out, sz,
                                      PSA_ALG_CBC_NO_PADDING,
                                      AES_DECRYPTION);
}
#endif /* HAVE_AES_CBC */

#endif /* ! NO_AES */
#endif /* ! WOLFSSL_PSA_NO_AES */
#endif /* WOLFSSL_HAVE_PSA */
