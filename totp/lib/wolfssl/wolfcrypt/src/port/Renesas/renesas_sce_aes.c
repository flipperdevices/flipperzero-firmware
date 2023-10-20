/* renesas_sce_aes.c
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

#ifndef NO_AES

#if (defined(WOLFSSL_RENESAS_SCEPROTECT) || \
     defined(WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY)) && \
    !defined(NO_WOLFSSL_RENESAS_SCEPROTECT_AES)

#include <wolfssl/wolfcrypt/wc_port.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/internal.h>
#include <wolfssl/wolfcrypt/aes.h>
#include "wolfssl/wolfcrypt/port/Renesas/renesas-sce-crypt.h"

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

struct Aes;

#define SCE_AES_GCM_AUTH_TAG_SIZE  16

typedef fsp_err_t (*aesGcmEncInitFn)
        (sce_gcm_handle_t*, sce_aes_wrapped_key_t*, uint8_t*, uint32_t);
typedef fsp_err_t (*aesGcmEncUpdateFn)
        (sce_gcm_handle_t*,uint8_t*, uint8_t*, uint32_t, uint8_t*, uint32_t);
typedef fsp_err_t (*aesGcmEncFinalFn)
        (sce_gcm_handle_t*, uint8_t*, uint32_t*, uint8_t*);

typedef fsp_err_t (*aesGcmDecInitFn)
        (sce_gcm_handle_t*, sce_aes_wrapped_key_t*, uint8_t*, uint32_t);
typedef fsp_err_t (*aesGcmDecUpdateFn)
        (sce_gcm_handle_t*,uint8_t*, uint8_t*, uint32_t, uint8_t*, uint32_t);
typedef fsp_err_t (*aesGcmDecFinalFn)
        (sce_gcm_handle_t*, uint8_t*, uint32_t*, uint8_t*, uint32_t);

/* Perform Aes Gcm encryption by SCE
 *
 * aes    The AES object.
 * out    Buffer to hold cipher text
 * in     Buffer to hold plaintext
 * sz     Length of cipher text/plaintext in bytes
 * iv     Buffer holding IV/nonce
 * ivSz   Length of IV/nonce in bytes
 * authTag Buffer to hold authentication data
 * authTagSz Length of authentication data in bytes
 * ctx    The callback context
 * return FSP_SUCCESS(0) on Success, otherwise negative value
 */
WOLFSSL_LOCAL int  wc_sce_AesGcmEncrypt(struct Aes* aes, byte* out,
                              const byte* in, word32 sz,
                              byte* iv, word32 ivSz,
                              byte* authTag, word32 authTagSz,
                              const byte* authIn, word32 authInSz,
                              void* ctx)
{
    int ret;
    sce_gcm_handle_t    _handle;
    uint32_t            dataLen = sz;
    User_SCEPKCbInfo    *info = (User_SCEPKCbInfo*)ctx;

    aesGcmEncInitFn     initFn;
    aesGcmEncUpdateFn   updateFn;
    aesGcmEncFinalFn    finalFn;

    uint8_t* plainBuf  = NULL;
    uint8_t* cipherBuf = NULL;
    uint8_t* aTagBuf   = NULL;
    uint8_t  delta;
    const uint8_t* iv_l = NULL;
    uint32_t ivSz_l = 0;

    sce_hmac_sha_wrapped_key_t key_client_mac;
    sce_hmac_sha_wrapped_key_t key_server_mac;
    sce_aes_wrapped_key_t      key_client_aes;
    sce_aes_wrapped_key_t      key_server_aes;

    /* sanity check */
    if (aes == NULL || authTagSz > AES_BLOCK_SIZE || ivSz == 0 || ctx == NULL) {
        return BAD_FUNC_ARG;
    }

    if (authTagSz < WOLFSSL_MIN_AUTH_TAG_SZ) {
        WOLFSSL_MSG("GcmEncrypt authTagSz too small error");
        return BAD_FUNC_ARG;
    }

    if (aes->ctx.keySize != 16 && aes->ctx.keySize != 32) {
        WOLFSSL_MSG("keySize is invalid, neither 16 or 32.");
        return BAD_FUNC_ARG;
    }

    if (aes->ctx.keySize == 16) {
        initFn   = R_SCE_AES128GCM_EncryptInit;
        updateFn = R_SCE_AES128GCM_EncryptUpdate;
        finalFn  = R_SCE_AES128GCM_EncryptFinal;
    }
    else {
        initFn   = R_SCE_AES256GCM_EncryptInit;
        updateFn = R_SCE_AES256GCM_EncryptUpdate;
        finalFn  = R_SCE_AES256GCM_EncryptFinal;
    }


    /* check if AES GCM can be used by SCE */
    if ((ret = wc_sce_hw_lock()) == 0) {

        /* allocate buffers for plaintext, ciphertext and authTag to make sure
         * those buffers 32bit aligned as SCE requests.
         */
         delta = ((sz % AES_BLOCK_SIZE) == 0) ? 0 :
                             AES_BLOCK_SIZE - (sz % AES_BLOCK_SIZE);
        plainBuf  = XMALLOC(sz, aes->heap, DYNAMIC_TYPE_AES);
        cipherBuf = XMALLOC(sz + delta, aes->heap, DYNAMIC_TYPE_AES);
        aTagBuf   = XMALLOC(SCE_AES_GCM_AUTH_TAG_SIZE, aes->heap,
                                                        DYNAMIC_TYPE_AES);

        if (plainBuf == NULL || cipherBuf == NULL || aTagBuf == NULL) {
            WOLFSSL_MSG("wc_sce_AesGcmEncrypt: buffer allocation failed");
            ret = -1;
        }

        if (ret == 0) {
            XMEMCPY(plainBuf, in, sz);
            XMEMSET((void*)cipherBuf, 0, sz + delta);
            XMEMSET((void*)authTag,   0, authTagSz);
        }
        
      #if defined(WOLFSSL_RENESAS_SCEPROTECT)
       if (ret == 0 &&
           info->keyflgs_tls.bits.session_key_set == 1) {
            /* generate AES-GCM session key. The key stored in
             * Aes.ctx.tsip_keyIdx is not used here.
             */
            ret = R_SCE_TLS_SessionKeyGenerate(
                    info->sce_cipher,
                    (uint32_t*)info->sce_masterSecret,
                    (uint8_t*) info->sce_clientRandom,
                    (uint8_t*) info->sce_serverRandom,
                    &iv[AESGCM_IMP_IV_SZ], /* use exp_IV */
                    &key_client_mac,
                    &key_server_mac,
                    &key_client_aes,
                    &key_server_aes,
                    NULL, NULL);
            if (ret != FSP_SUCCESS) {
                WOLFSSL_MSG("R_SCE_TLS_SessionKeyGenerate failed");
                ret = -1;
            }

        }
        else {
       #else
        if (ret == 0) {
       #endif
            if (info->keyflgs_crypt.bits.aes256_installedkey_set == 1 ||
                info->keyflgs_crypt.bits.aes128_installedkey_set == 1) {
                if (aes->ctx.keySize == 32) {
                    XMEMCPY(&key_client_aes, 
                        (sce_aes_wrapped_key_t*)info->sce_wrapped_key_aes256,
                        sizeof(sce_aes_wrapped_key_t));
                }
                else {
                    XMEMCPY(&key_client_aes, 
                        (sce_aes_wrapped_key_t*)info->sce_wrapped_key_aes128,
                        sizeof(sce_aes_wrapped_key_t));
                }
                iv_l = iv;
                ivSz_l = ivSz;
            }
            else {
                WOLFSSL_MSG("AES key for SCE is not set.");
                ret = -1;
            }
        }

        if (ret == 0) {

            /* since generated session key is coupled to iv, no need to pass
             * them init func.
             */
            ret = initFn(&_handle, &key_client_aes, (uint8_t*)iv_l, ivSz_l);

            if (ret == FSP_SUCCESS) {
                ret = updateFn(&_handle, NULL, NULL, 0UL, (uint8_t*)authIn,
                                                                    authInSz);
            }
            if (ret == FSP_SUCCESS) {
                ret = updateFn(&_handle, plainBuf, cipherBuf, sz, NULL, 0UL);
            }
            if (ret != FSP_SUCCESS) {
                WOLFSSL_MSG("R_SCE_AesXXXGcmEncryptUpdate2: failed");
                ret = -1;
            }

            if (ret == FSP_SUCCESS) {
                /* Once R_SCE_AesxxxGcmEncryptInit or R_SCE_AesxxxEncryptUpdate is
                * called, R_SCE_AesxxxGcmEncryptFinal must be called regardless of
                * the result of the previous call. Otherwise, SCE can not come out
                * from its error state and all the trailing APIs will fail.
                */
                dataLen = 0;
                ret = finalFn(&_handle,
                           cipherBuf + (sz + delta - AES_BLOCK_SIZE),
                              &dataLen,
                              aTagBuf);

                if (ret == FSP_SUCCESS) {
                   /* copy encrypted data to out */
                    if (sz != dataLen) {
                        WOLFSSL_MSG("sz is not equal to dataLen!!!!");
                        ret = -1;
                    } else {
                        XMEMCPY(out, cipherBuf, dataLen);
                        /* copy auth tag to caller's buffer */
                        XMEMCPY((void*)authTag, (void*)aTagBuf,
                                    min(authTagSz, SCE_AES_GCM_AUTH_TAG_SIZE ));
                    }
                }
                else {
                    WOLFSSL_MSG("R_SCE_AesxxxGcmEncryptFinal: failed");
                    ret = -1;
                }
            }
        }

        XFREE(plainBuf,  aes->heap, DYNAMIC_TYPE_AES);
        XFREE(cipherBuf, aes->heap, DYNAMIC_TYPE_AES);
        XFREE(aTagBuf,   aes->heap, DYNAMIC_TYPE_AES);

        wc_sce_hw_unlock();

    }

    return ret;
}
/* Perform Aes Gcm decryption by SCE
 *
 * aes    The AES object.
 * out    Buffer to hold plaintext
 * in     Buffer to hold cipher text
 * sz     Length of cipher text/plaintext in bytes
 * iv     Buffer holding IV/nonce
 * ivSz   Length of IV/nonce in bytes
 * authTag Buffer to hold authentication data
 * authTagSz Length of authentication data in bytes
 * ctx    The Callback context
 * return FSP_SUCCESS(0) on Success, otherwise negative value
 */
WOLFSSL_LOCAL int  wc_sce_AesGcmDecrypt(struct Aes* aes, byte* out,
                          const byte* in, word32 sz,
                          const byte* iv, word32 ivSz,
                          const byte* authTag, word32 authTagSz,
                          const byte* authIn, word32 authInSz,
                          void* ctx)
{
    int ret;
    sce_gcm_handle_t _handle;
    uint32_t            dataLen = sz;
    User_SCEPKCbInfo    *info = (User_SCEPKCbInfo*)ctx;

    aesGcmDecInitFn     initFn;
    aesGcmDecUpdateFn   updateFn;
    aesGcmDecFinalFn    finalFn;

    uint8_t* cipherBuf = NULL;
    uint8_t* plainBuf  = NULL;
    uint8_t* aTagBuf = NULL;
    uint8_t  delta;
    const uint8_t* iv_l = NULL;
    uint32_t ivSz_l = 0;

    sce_hmac_sha_wrapped_key_t key_client_mac;
    sce_hmac_sha_wrapped_key_t key_server_mac;
    sce_aes_wrapped_key_t      key_client_aes;
    sce_aes_wrapped_key_t      key_server_aes;

    /* sanity check */
    if (aes == NULL || authTagSz > AES_BLOCK_SIZE || ivSz == 0 || ctx == NULL) {
        return BAD_FUNC_ARG;
    }

    if (authTagSz < WOLFSSL_MIN_AUTH_TAG_SZ) {
        WOLFSSL_MSG("GcmEncrypt authTagSz too small error");
        return BAD_FUNC_ARG;
    }

    if (aes->ctx.keySize != 16 && aes->ctx.keySize != 32) {
        WOLFSSL_MSG("keySize is invalid, neither 16 or 32.");
        return BAD_FUNC_ARG;
    }

    if (aes->ctx.keySize == 16) {
        initFn   = R_SCE_AES128GCM_DecryptInit;
        updateFn = R_SCE_AES128GCM_DecryptUpdate;
        finalFn  = R_SCE_AES128GCM_DecryptFinal;
    }
    else {
        initFn   = R_SCE_AES256GCM_DecryptInit;
        updateFn = R_SCE_AES256GCM_DecryptUpdate;
        finalFn  = R_SCE_AES256GCM_DecryptFinal;
    }


    if ((ret = wc_sce_hw_lock()) == 0) {
       /* allocate buffers for plain-text, cipher-text, authTag and AAD.
         * TSIP requests those buffers 32bit aligned.
         */
         delta = ((sz % AES_BLOCK_SIZE) == 0) ? 0 :
                                      AES_BLOCK_SIZE - (sz % AES_BLOCK_SIZE);
        cipherBuf = XMALLOC(sz, aes->heap, DYNAMIC_TYPE_AES);
        plainBuf  = XMALLOC(sz + delta, aes->heap, DYNAMIC_TYPE_AES);
        aTagBuf   = XMALLOC(SCE_AES_GCM_AUTH_TAG_SIZE, aes->heap,
                                                        DYNAMIC_TYPE_AES);

        if (plainBuf == NULL || cipherBuf == NULL || aTagBuf == NULL) {
            ret = -1;
        }

        if (ret == 0) {
            XMEMSET((void*)plainBuf,  0, sz);
            XMEMCPY(cipherBuf, in, sz);
            XMEMCPY(aTagBuf, authTag, authTagSz);
        }
       #if defined(WOLFSSL_RENESAS_SCEPROTECT)
        if (ret == 0 &&
            info->keyflgs_tls.bits.session_key_set == 1) {
            /* generate AES-GCM session key. The key stored in
             * Aes.ctx.tsip_keyIdx is not used here.
             */
            ret = R_SCE_TLS_SessionKeyGenerate(
                    info->sce_cipher,
                    (uint32_t*)info->sce_masterSecret,
                    (uint8_t*) info->sce_clientRandom,
                    (uint8_t*) info->sce_serverRandom,
                    (uint8_t*)&iv[AESGCM_IMP_IV_SZ], /* use exp_IV */
                    &key_client_mac,
                    &key_server_mac,
                    &key_client_aes,
                    &key_server_aes,
                    NULL, NULL);
            if (ret != FSP_SUCCESS) {
                WOLFSSL_MSG("R_SCE_TLS_SessionKeyGenerate failed");
                ret = -1;
            }
        }
        else {
       #else
        if (ret == 0) {
       #endif
            if (info->keyflgs_crypt.bits.aes256_installedkey_set == 1 ||
                info->keyflgs_crypt.bits.aes128_installedkey_set == 1) {
                if (aes->ctx.keySize == 32) {
                    XMEMCPY(&key_server_aes, 
                        (sce_aes_wrapped_key_t*)info->sce_wrapped_key_aes256,
                        sizeof(sce_aes_wrapped_key_t));
                }
                else {
                    XMEMCPY(&key_server_aes, 
                        (sce_aes_wrapped_key_t*)info->sce_wrapped_key_aes128,
                        sizeof(sce_aes_wrapped_key_t));
                }
                iv_l = iv;
                ivSz_l = ivSz;
            }
            else {
                WOLFSSL_MSG("AES key for SCE is not set.");
                ret = -1;
            }
        }
        
        if (ret == 0) {
            /* since key_index has iv and ivSz in it, no need to pass them init
             * func. Pass NULL and 0 as 3rd and 4th parameter respectively.
             */
             ret = initFn(&_handle, &key_server_aes, (uint8_t*)iv_l, ivSz_l);


            if (ret == FSP_SUCCESS) {
                /* pass only AAD and it's size before passing cipher text */
                ret = updateFn(&_handle, NULL, NULL, 0UL, (uint8_t*)authIn,
                                                                    authInSz);
            }
            if (ret == FSP_SUCCESS) {
                ret = updateFn(&_handle, cipherBuf, plainBuf, sz, NULL, 0UL);
            }
            if (ret != FSP_SUCCESS) {
                WOLFSSL_MSG("R_SCE_AesXXXGcmDecryptUpdate: failed in decrypt");
                ret = -1;
            }

            if (ret == FSP_SUCCESS) {
                dataLen = 0;
                ret = finalFn(&_handle,
                                  plainBuf + (sz + delta - AES_BLOCK_SIZE),
                            &dataLen,
                            aTagBuf,
                            min(16, authTagSz));

                if (ret == FSP_SUCCESS) {
                    /* copy plain data to out */
                    if (sz != dataLen) {
                        WOLFSSL_MSG("sz is not equal to dataLen!!!!");
                        ret = -1;
                    }
                    else {
                        XMEMCPY(out, plainBuf, dataLen);
                    }
                }
                else {
                    WOLFSSL_MSG("R_SCE_AesXXXGcmDecryptFinal: failed");
                    ret = -1;
                }
            }
        }

        XFREE(aTagBuf,   aes->heap, DYNAMIC_TYPE_AES);
        XFREE(plainBuf,  aes->heap, DYNAMIC_TYPE_AES);
        XFREE(cipherBuf, aes->heap, DYNAMIC_TYPE_AES);

        wc_sce_hw_unlock();
    }

    return ret;
}
/* Perform Aes Cbc encryption by SCE
 *
 * aes    The AES object.
 * out    Buffer to hold cipher text
 * in     Buffer to hold plain text
 * sz     Length of cipher text/plaintext in bytes
 * return FSP_SUCCESS(0) on Success, otherwise negative value
 */
WOLFSSL_LOCAL int wc_sce_AesCbcEncrypt(struct Aes* aes, byte* out,
                                                const byte* in, word32 sz)
{
    sce_aes_handle_t _handle;
    word32 ret;
    word32 blocks = (sz / AES_BLOCK_SIZE);
    uint32_t dataLength;
    byte *iv;

    if ((in == NULL) || (out == NULL) || (aes == NULL))
      return BAD_FUNC_ARG;

    /* while doing TLS handshake, SCE driver keeps true-key and iv *
     * on the device. iv is dummy                                   */
    iv = (uint8_t*)aes->reg;

    if ((ret = wc_sce_hw_lock()) != 0) {
        WOLFSSL_MSG("Failed to lock");
        return ret;
    }

    if (aes->ctx.keySize == 16) {
        ret = R_SCE_AES128CBC_EncryptInit(&_handle, &aes->ctx.sce_wrapped_key, iv);
    }
    else if (aes->ctx.keySize == 32) {
        ret = R_SCE_AES256CBC_EncryptInit(&_handle, &aes->ctx.sce_wrapped_key, iv);
    }
    else {
        WOLFSSL_MSG("invalid key Size for SCE. Key size is neither 16 or 32.");
        wc_sce_hw_unlock();
        return -1;
    }

    while (ret == FSP_SUCCESS && blocks--) {

        if (aes->ctx.keySize == 16)
            ret = R_SCE_AES128CBC_EncryptUpdate(&_handle, (uint8_t*)in,
                                    (uint8_t*)out, (uint32_t)AES_BLOCK_SIZE);
        else
            ret = R_SCE_AES256CBC_EncryptUpdate(&_handle, (uint8_t*)in,
                                    (uint8_t*)out, (uint32_t)AES_BLOCK_SIZE);

        in  += AES_BLOCK_SIZE;
        out += AES_BLOCK_SIZE;
    }

    if (ret == FSP_SUCCESS) {
        if (aes->ctx.keySize == 16) {
            ret = R_SCE_AES128CBC_EncryptFinal(&_handle, out, &dataLength);
        }
        else {
            ret = R_SCE_AES256CBC_EncryptFinal(&_handle, out, &dataLength);
        }
    }
    else {
        WOLFSSL_MSG("SCE AES CBC encryption failed");
        ret = -1;
    }

    wc_sce_hw_unlock();
    return ret;
}
/* Perform Aes Cbc decryption by SCE
 *
 * aes    The AES object.
 * out    Buffer to hold plain text
 * in     Buffer to hold cipher text
 * sz     Length of cipher text/plaintext in bytes
 * return FSP_SUCCESS(0) on Success, otherwise negative value
 */
WOLFSSL_LOCAL int wc_sce_AesCbcDecrypt(struct Aes* aes, byte* out, const byte* in, word32 sz)
{
    sce_aes_handle_t _handle;
    word32 ret;
    word32 blocks = (sz / AES_BLOCK_SIZE);
    uint32_t dataLength;
    byte *iv;

    if ((in == NULL) || (out == NULL) || (aes == NULL))
      return BAD_FUNC_ARG;

    iv = (uint8_t*)aes->reg;

    if ((ret = wc_sce_hw_lock()) != 0) {
        WOLFSSL_MSG("Failed to lock");
        return ret;
    }

    if (aes->ctx.keySize == 16) {
        ret = R_SCE_AES128CBC_DecryptInit(&_handle, &aes->ctx.sce_wrapped_key, iv);
    }
    else if (aes->ctx.keySize == 32) {
        ret = R_SCE_AES256CBC_DecryptInit(&_handle, &aes->ctx.sce_wrapped_key, iv);
    }
    else {
        wc_sce_hw_unlock();
        return -1;
    }

    while (ret == FSP_SUCCESS && blocks--) {

        if (aes->ctx.keySize == 16)
            ret = R_SCE_AES128CBC_DecryptUpdate(&_handle, (uint8_t*)in,
                                        (uint8_t*)out, (uint32_t)AES_BLOCK_SIZE);
        else
            ret = R_SCE_AES256CBC_DecryptUpdate(&_handle, (uint8_t*)in,
                                        (uint8_t*)out, (uint32_t)AES_BLOCK_SIZE);

        in  += AES_BLOCK_SIZE;
        out += AES_BLOCK_SIZE;
    }

    if (ret == FSP_SUCCESS) {
        if (aes->ctx.keySize == 16)
            ret = R_SCE_AES128CBC_DecryptFinal(&_handle, out, &dataLength);
        else
            ret = R_SCE_AES256CBC_DecryptFinal(&_handle, out, &dataLength);
    }
    else {
        WOLFSSL_MSG("SCE AES CBC decryption failed");
        ret = -1;
    }

    wc_sce_hw_unlock();
    return ret;
}

#endif /* WOLFSSL_RENESAS_TSIP_CRYPT */
#endif /* NO_AES */
