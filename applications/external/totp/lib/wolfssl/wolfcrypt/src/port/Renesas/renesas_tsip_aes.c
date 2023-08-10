/* renesas_tsip_aes.c
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

#include <string.h>
#include <stdio.h>

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif
#include <wolfssl/wolfcrypt/settings.h>
#include <stdio.h>

#ifndef NO_AES

#if (defined(WOLFSSL_RENESAS_TSIP_TLS) || \
     defined(WOLFSSL_RENESAS_TSIP_CRYPTONLY)) && \
    !defined(NO_WOLFSSL_RENESAS_TSIP_CRYPT_AES)

#include <wolfssl/wolfcrypt/wc_port.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/internal.h>
#include <wolfssl/wolfcrypt/aes.h>
#include "wolfssl/wolfcrypt/port/Renesas/renesas-tsip-crypt.h"
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#define TSIP_AES_GCM_AUTH_TAG_SIZE  16

typedef e_tsip_err_t (*aesGcmEncInitFn)
        (tsip_gcm_handle_t*, tsip_aes_key_index_t*, uint8_t*, uint32_t);
typedef e_tsip_err_t (*aesGcmEncUpdateFn)
        (tsip_gcm_handle_t*,uint8_t*, uint8_t*, uint32_t, uint8_t*, uint32_t);
typedef e_tsip_err_t (*aesGcmEncFinalFn)
        (tsip_gcm_handle_t*, uint8_t*, uint32_t*, uint8_t*);
typedef e_tsip_err_t (*aesGcmDecInitFn)
        (tsip_gcm_handle_t*, tsip_aes_key_index_t*, uint8_t*, uint32_t);
typedef e_tsip_err_t (*aesGcmDecUpdateFn)
        (tsip_gcm_handle_t*,uint8_t*, uint8_t*, uint32_t, uint8_t*, uint32_t);
typedef e_tsip_err_t (*aesGcmDecFinalFn)
        (tsip_gcm_handle_t*, uint8_t*, uint32_t*, uint8_t*, uint32_t);

#ifdef WOLFSSL_RENESAS_TSIP_TLS

/* function pointer type defs for TLSv13 handshake AES-GCM/CCM encryption */
typedef e_tsip_err_t (*Tls13AesEncInitFn)
        (tsip_tls13_handle_t*, e_tsip_tls13_phase_t, e_tsip_tls13_mode_t,
         e_tsip_tls13_cipher_suite_t, tsip_aes_key_index_t*, uint32_t);
typedef e_tsip_err_t (*Tls13AesEncUpdateFn)
        (tsip_tls13_handle_t*, uint8_t*, uint8_t*, uint32_t);
typedef e_tsip_err_t (*Tls13AesEncFinalFn)
        (tsip_tls13_handle_t*, uint8_t*, uint32_t*);

/* function pointer type defs for TLSv13 handshake AES-GCM/CCM decryption */
typedef e_tsip_err_t (*Tls13AesDecInitFn)
        (tsip_tls13_handle_t*, e_tsip_tls13_phase_t, e_tsip_tls13_mode_t,
         e_tsip_tls13_cipher_suite_t, tsip_aes_key_index_t*, uint32_t);
typedef e_tsip_err_t (*Tls13AesDecUpdateFn)
        (tsip_tls13_handle_t*, uint8_t*, uint8_t*, uint32_t);
typedef e_tsip_err_t (*Tls13AesDecFinalFn)
        (tsip_tls13_handle_t*, uint8_t*, uint32_t*);





/*  encrypt plain data.
 *  
 *  return cipher data size on success, negative value on failure.
 *         CRYPTOCB_UNAVAILABLE may be returned.   
 */
WOLFSSL_LOCAL int tsip_Tls13AesEncrypt(
                            struct WOLFSSL* ssl,
                            byte* output,
                            const byte* input,
                            word16 sz)
{
    int ret = 0;
    e_tsip_err_t    err = TSIP_SUCCESS;
    TsipUserCtx*    tuc = NULL;
    e_tsip_tls13_cipher_suite_t cs;
    word32  cipher[(AES_BLOCK_SIZE + TSIP_AES_GCM_AUTH_TAG_SIZE) /
                                                             sizeof(word32)];
    word32  plain[AES_BLOCK_SIZE / sizeof(word32)];
    int             idxIn,idxOut;
    uint32_t        remain;
    uint32_t        dataSz, finalSz;
    e_tsip_tls13_phase_t phase;
    tsip_aes_key_index_t* key = NULL;

    WOLFSSL_ENTER("tsip_Tls13AesEncrypt");

    if ((ssl == NULL) || (input == NULL) || (output == NULL) || (sz == 0)) {
        return BAD_FUNC_ARG;
    }

    if (ssl->options.side != WOLFSSL_CLIENT_END) {
        return CRYPTOCB_UNAVAILABLE;   /* expecting to fallback to S/W */
    }

    /* get user context for TSIP */
    tuc = ssl->RenesasUserCtx;
    if (tuc == NULL) {
        WOLFSSL_MSG("TsipUserCtx hasn't been set to ssl.");
        return CRYPTOCB_UNAVAILABLE;
    }

    /* select the appropriate encryption key and phase */
    if (ssl->options.handShakeDone) {
        if (!tuc->ClientWriteTrafficKey_set) {
            WOLFSSL_MSG("TSIP wasn't involved in the key-exchange.");
            return CRYPTOCB_UNAVAILABLE;
        }

        key = &(tuc->clientAppWriteKey13Idx);
        phase = TSIP_TLS13_PHASE_APPLICATION;
    }
    else {
        if (!tuc->HandshakeClientTrafficKey_set) {
            WOLFSSL_MSG("TSIP wasn't involved in the key-exchange.");
            return CRYPTOCB_UNAVAILABLE;
        }

        key = &(tuc->clientWriteKey13Idx);
        phase = TSIP_TLS13_PHASE_HANDSHAKE;
    }

    /* select AES mode */
    if (ssl->specs.bulk_cipher_algorithm == wolfssl_aes_gcm)
        cs = TSIP_TLS13_CIPHER_SUITE_AES_128_GCM_SHA256;
    else if (ssl->specs.bulk_cipher_algorithm == wolfssl_aes_ccm)
        cs = TSIP_TLS13_CIPHER_SUITE_AES_128_CCM_SHA256;
    else
        return CRYPTOCB_UNAVAILABLE;

    remain  = sz;
    finalSz = 0;

    if ((ret = tsip_hw_lock()) == 0) {

        err = R_TSIP_Tls13EncryptInit(
                                    &(tuc->handle13),
                                    phase,
                                    TSIP_TLS13_MODE_FULL_HANDSHAKE,
                                    cs,
                                    key,
                                    sz);
 
        if (err != TSIP_SUCCESS) {
            WOLFSSL_MSG("R_TSIP_Tls13DecryptUpdate error");
            ret = WC_HW_E;
        }

        idxIn  = 0;
        idxOut = 0;

        while (err == TSIP_SUCCESS && remain > 0) {

            dataSz = min(remain, AES_BLOCK_SIZE);
            ForceZero(plain, sizeof(plain));
            ForceZero(cipher, sizeof(cipher));
            XMEMCPY(plain, input + idxIn, dataSz);


            err = R_TSIP_Tls13EncryptUpdate(
                                    &(tuc->handle13),
                                    (uint8_t*)plain,
                                    (uint8_t*)cipher,
                                    dataSz);

            if (err == TSIP_SUCCESS) {
                if (dataSz >= AES_BLOCK_SIZE) {
                    XMEMCPY(output + idxOut, cipher, dataSz);
                    idxOut += dataSz;
                }
                idxIn  += dataSz;
                remain -= dataSz;
            }
            else {
                WOLFSSL_MSG("R_TSIP_Tls13DecryptUpdate error");
                ret = WC_HW_E;
            }
        }

        if (err == TSIP_SUCCESS) {

            ForceZero(cipher, sizeof(cipher));
            /* R_TSIP_Tls13EncryptFinal outputs encrypted content and auth-data
             * to the buffer.
             */
            err = R_TSIP_Tls13EncryptFinal(
                                    &(tuc->handle13),
                                    (uint8_t*)cipher,
                                    &finalSz);          /* total output size */

            if (err == TSIP_SUCCESS) {
                XMEMCPY(output + idxOut, cipher, finalSz - idxOut);
                ret = finalSz;
            }
            else {
                WOLFSSL_MSG("R_TSIP_Tls13EncryptFinal error");
                ret = WC_HW_E;
            }
        }
        tsip_hw_unlock();
    }

    WOLFSSL_LEAVE("tsip_Tls13AesEncrypt", ret);
    return ret;
}




/* decrypt encrypted handshake data for TLSv1.3
 * AES-GCM or AES-CCM can be used
 * return 0 on success, otherwise on error.
 */ 
WOLFSSL_LOCAL int tsip_Tls13AesDecrypt(
                            struct WOLFSSL* ssl,
                            byte* output,
                            const byte* input,
                            word16 sz)
{
    int ret = 0;
    e_tsip_err_t    err = TSIP_SUCCESS;
    TsipUserCtx*    tuc = NULL;
    e_tsip_tls13_cipher_suite_t cs;
    word32          cipher[AES_BLOCK_SIZE / sizeof(word32)];
    word32          plain[AES_BLOCK_SIZE / sizeof(word32)];
    int             idxIn,idxOut;
    int             blocks;
    uint32_t        remain,conRemain;
    uint32_t        dataSz, finalSz;
    e_tsip_tls13_phase_t     phase;
    tsip_aes_key_index_t* key = NULL;

    WOLFSSL_ENTER("tsip_Tls13AesDecrypt");

    if ((ssl == NULL) || (input == NULL) || (output == NULL) || (sz == 0)) {
        return BAD_FUNC_ARG;
    }

    if (ssl->options.side != WOLFSSL_CLIENT_END) {
        return CRYPTOCB_UNAVAILABLE;   /* expecting to fallback to S/W */
    }

    /* get user context for TSIP */
    tuc = ssl->RenesasUserCtx;
    if (tuc == NULL) {
        WOLFSSL_MSG("TsipUserCtx hasn't been set to ssl.");
        return CRYPTOCB_UNAVAILABLE;
    }

    /* select the appropriate encryption key and phase */
    if (ssl->options.handShakeDone) {
        if (!tuc->ServerWriteTrafficKey_set) {
            WOLFSSL_MSG("TSIP wasn't involved in the key-exchange.");
            return CRYPTOCB_UNAVAILABLE;
        }

        key = &(tuc->serverAppWriteKey13Idx);
        phase = TSIP_TLS13_PHASE_APPLICATION;
    }
    else {
        if (!tuc->HandshakeServerTrafficKey_set) {
            WOLFSSL_MSG("TSIP wasn't involved in the key-exchange.");
            return CRYPTOCB_UNAVAILABLE;
        }

        key = &(tuc->serverWriteKey13Idx);
        phase = TSIP_TLS13_PHASE_HANDSHAKE;
    }

    /* select AES mode */
    if (ssl->specs.bulk_cipher_algorithm == wolfssl_aes_gcm)
        cs = TSIP_TLS13_CIPHER_SUITE_AES_128_GCM_SHA256;
    else if (ssl->specs.bulk_cipher_algorithm == wolfssl_aes_ccm)
        cs = TSIP_TLS13_CIPHER_SUITE_AES_128_CCM_SHA256;
    else
        return CRYPTOCB_UNAVAILABLE;


    blocks    = sz / AES_BLOCK_SIZE;
    remain    = sz;
    conRemain = sz - TSIP_AES_GCM_AUTH_TAG_SIZE;

    if ((ret = tsip_hw_lock()) == 0) {

        err = R_TSIP_Tls13DecryptInit(
                                    &(tuc->handle13),
                                    phase,
                                    TSIP_TLS13_MODE_FULL_HANDSHAKE,
                                    cs,
                                    key,
                                    sz);

        if (err != TSIP_SUCCESS) {
            WOLFSSL_MSG("R_TSIP_Tls13DecryptInit error");
            ret = WC_HW_E;
        }

        idxIn  = 0;
        idxOut = 0;

        while (err == TSIP_SUCCESS && (blocks--) >= 0) {

            dataSz = min(remain, AES_BLOCK_SIZE);
            XMEMCPY(cipher, input + idxIn, dataSz);
            ForceZero(plain, AES_BLOCK_SIZE);

            err = R_TSIP_Tls13DecryptUpdate(
                                    &(tuc->handle13),
                                    (uint8_t*)cipher,
                                    (uint8_t*)plain,
                                    dataSz);

            if (err == TSIP_SUCCESS) {
                if (dataSz >= AES_BLOCK_SIZE && conRemain >= AES_BLOCK_SIZE) {
                    XMEMCPY(output + idxOut, plain, dataSz);
                    idxOut += dataSz;
                    conRemain -= min(conRemain, dataSz);
                }
                idxIn  += dataSz;
                remain -= dataSz;
            }
            else {
                WOLFSSL_MSG("R_TSIP_Tls13DecryptUpdate error");
                ret = WC_HW_E;
            }
        }

        if (err == TSIP_SUCCESS) {
            err = R_TSIP_Tls13DecryptFinal(
                                    &(tuc->handle13),
                                    (uint8_t*)plain,
                                    &finalSz); /* total size will be returned */

            if (err == TSIP_SUCCESS) {
                XMEMCPY(output + idxOut, plain, conRemain);
            }
            else if (err== TSIP_ERR_AUTHENTICATION) {
                WOLFSSL_MSG("tsip_Tls13AesDecrypt authentication error");
                ret = AES_GCM_AUTH_E;
            }
            else {
                WOLFSSL_MSG("R_TSIP_Tls13DecryptFinal error");
                ret = WC_HW_E;
            }
        }
        tsip_hw_unlock();
    }

    WOLFSSL_LEAVE("tsip_Tls13AesDecrypt", ret);
    return ret;
}
#endif /* WOLFSSL_RENESAS_TSIP_TLS */


#if (WOLFSSL_RENESAS_TSIP_VER >= 109)
#ifdef WOLF_CRYPTO_CB

WOLFSSL_LOCAL int wc_tsip_AesCipher(int devIdArg, wc_CryptoInfo* info,
                                                                    void* ctx)
{
    int ret = NOT_COMPILED_IN;
    TsipUserCtx* cbInfo = (TsipUserCtx*)ctx;

    WOLFSSL_ENTER("wc_tsip_AesCipher");

    if (info == NULL || ctx == NULL)
        return BAD_FUNC_ARG;

    if (info->algo_type == WC_ALGO_TYPE_CIPHER) {

#if !defined(NO_AES) || !defined(NO_DES3)
#ifdef HAVE_AESGCM
        if (info->cipher.type == WC_CIPHER_AES_GCM
        #ifdef WOLFSSL_RENESAS_TSIP_TLS
            && cbInfo->session_key_set == 1
        #endif
            ) {

            if (info->cipher.enc) {
                ret = wc_tsip_AesGcmEncrypt(
                        info->cipher.aesgcm_enc.aes,
                        (byte*)info->cipher.aesgcm_enc.out,
                        (byte*)info->cipher.aesgcm_enc.in,
                        info->cipher.aesgcm_enc.sz,
                        (byte*)info->cipher.aesgcm_enc.iv,
                        info->cipher.aesgcm_enc.ivSz,
                        (byte*)info->cipher.aesgcm_enc.authTag,
                        info->cipher.aesgcm_enc.authTagSz,
                        (byte*)info->cipher.aesgcm_enc.authIn,
                        info->cipher.aesgcm_enc.authInSz,
                        (void*)ctx);

            }
            else {
                ret = wc_tsip_AesGcmDecrypt(
                        info->cipher.aesgcm_dec.aes,
                        (byte*)info->cipher.aesgcm_dec.out,
                        (byte*)info->cipher.aesgcm_dec.in,
                        info->cipher.aesgcm_dec.sz,
                        (byte*)info->cipher.aesgcm_dec.iv,
                        info->cipher.aesgcm_dec.ivSz,
                        (byte*)info->cipher.aesgcm_dec.authTag,
                        info->cipher.aesgcm_dec.authTagSz,
                        (byte*)info->cipher.aesgcm_dec.authIn,
                        info->cipher.aesgcm_dec.authInSz,
                        (void*)ctx);
            }
        }
    #endif /* HAVE_AESGCM */
    #ifdef HAVE_AES_CBC
        if (info->cipher.type == WC_CIPHER_AES_CBC
        #ifdef WOLFSSL_RENESAS_TSIP_TLS
            && cbInfo->session_key_set == 1
        #endif
            ) {

            if (info->cipher.enc) {
                ret = wc_tsip_AesCbcEncrypt(
                    info->cipher.aescbc.aes,
                    (byte*)info->cipher.aescbc.out,
                    (byte*)info->cipher.aescbc.in,
                    info->cipher.aescbc.sz);

            }
            else {
                ret = wc_tsip_AesCbcDecrypt(
                    info->cipher.aescbc.aes,
                    (byte*)info->cipher.aescbc.out,
                    (byte*)info->cipher.aescbc.in,
                    info->cipher.aescbc.sz);
            }
        }
    #endif /* HAVE_AES_CBC */
    #endif /* !NO_AES || !NO_DES3 */

    }
    WOLFSSL_LEAVE("wc_tsip_AesCipher", ret);
    return ret;
}
#endif /* WOLF_CRYPTO_CB */
#endif /* WOLFSSL_RENESAS_TSIP_VER >= 109 */



int wc_tsip_AesCbcEncrypt(struct Aes* aes, byte* out, const byte* in, word32 sz)
{
    tsip_aes_handle_t _handle;
    int ret;
    word32 blocks = (sz / AES_BLOCK_SIZE);
    uint32_t dataLength;
    byte *iv;

    if ((in == NULL) || (out == NULL) || (aes == NULL))
      return BAD_FUNC_ARG;

    /* while doing TLS handshake, TSIP driver keeps true-key and iv *
     * on the device. iv is dummy                                   */
    iv = (uint8_t*)aes->reg;

    if ((ret = tsip_hw_lock()) != 0) {
        WOLFSSL_MSG("Failed to lock");
        return ret;
    }

    if (aes->ctx.keySize == 16) {
        ret = R_TSIP_Aes128CbcEncryptInit(&_handle, &aes->ctx.tsip_keyIdx, iv);
    }
    else if (aes->ctx.keySize == 32) {
        ret = R_TSIP_Aes256CbcEncryptInit(&_handle, &aes->ctx.tsip_keyIdx, iv);
    }
    else {
        tsip_hw_unlock();
        return -1;
    }

    while (ret == TSIP_SUCCESS && blocks--) {
        if (aes->ctx.keySize == 16)
            ret = R_TSIP_Aes128CbcEncryptUpdate(&_handle, (uint8_t*)in,
                                    (uint8_t*)out, (uint32_t)AES_BLOCK_SIZE);
        else
            ret = R_TSIP_Aes256CbcEncryptUpdate(&_handle, (uint8_t*)in,
                                    (uint8_t*)out, (uint32_t)AES_BLOCK_SIZE);

        in  += AES_BLOCK_SIZE;
        out += AES_BLOCK_SIZE;
    }

    if (ret == TSIP_SUCCESS) {
        if (aes->ctx.keySize == 16) {
            ret = R_TSIP_Aes128CbcEncryptFinal(&_handle, out, &dataLength);
        }
        else {
            ret = R_TSIP_Aes256CbcEncryptFinal(&_handle, out, &dataLength);
        }
    }
    else {
        WOLFSSL_MSG("TSIP AES CBC encryption failed");
        ret = -1;
    }

    tsip_hw_unlock();
    return ret;
}

int wc_tsip_AesCbcDecrypt(struct Aes* aes, byte* out, const byte* in, word32 sz)
{
   tsip_aes_handle_t _handle;
    int ret;
    word32 blocks = (sz / AES_BLOCK_SIZE);
    uint32_t dataLength;
    byte *iv;

    if ((in == NULL) || (out == NULL) || (aes == NULL))
      return BAD_FUNC_ARG;

    iv = (uint8_t*)aes->reg;

    if ((ret = tsip_hw_lock()) != 0) {
        WOLFSSL_MSG("Failed to lock");
        return ret;
    }

    if (aes->ctx.keySize == 16) {
        ret = R_TSIP_Aes128CbcDecryptInit(&_handle, &aes->ctx.tsip_keyIdx, iv);
    }
    else if (aes->ctx.keySize == 32) {
        ret = R_TSIP_Aes256CbcDecryptInit(&_handle, &aes->ctx.tsip_keyIdx, iv);
    }
    else {
        tsip_hw_unlock();
        return -1;
    }

    while (ret == TSIP_SUCCESS && blocks--) {

        if (aes->ctx.keySize == 16)
            ret = R_TSIP_Aes128CbcDecryptUpdate(&_handle, (uint8_t*)in,
                                        (uint8_t*)out, (uint32_t)AES_BLOCK_SIZE);
        else
            ret = R_TSIP_Aes256CbcDecryptUpdate(&_handle, (uint8_t*)in,
                                        (uint8_t*)out, (uint32_t)AES_BLOCK_SIZE);

        in  += AES_BLOCK_SIZE;
        out += AES_BLOCK_SIZE;
    }

    if (ret == TSIP_SUCCESS) {
        if (aes->ctx.keySize == 16)
            ret = R_TSIP_Aes128CbcDecryptFinal(&_handle, out, &dataLength);
        else
            ret = R_TSIP_Aes256CbcDecryptFinal(&_handle, out, &dataLength);
    }
    else {
        WOLFSSL_MSG("TSIP AES CBC decryption failed");
        ret = -1;
    }

    tsip_hw_unlock();
    return ret;
}
/*
 * Encrypt plain data then output encrypted data and authentication tag data.
 * The session key used for encryption is generated inside this function and
 * the key which has been generated and stored in Aes is not used.
 * parameter
 *  - aes:  Aes structure
 *  - out:  buffer where the cipher text is output
 *  - in:   buffer where the plain data is storead
 *  - sz:   size of plain data and also means output size
 *  - iv:   iv should be consist of implicit-iv of 4 bytes and exp-iv of 8 bytes
 *  - authTag: buffer where the authentication data is output.
 *  - authTagSz: buffer size for authentication data.
 *  - authIn: buffer holding Additional Authentication Data(AAD)
 *  - authInSz: AAD size
 *  - ctx:   TsipUserCtx
 * return 0 on success, otherwise on error.
 * Note: As of TSIPv1.13, only accept 128 and 256 bit of key size
 *
 */
int wc_tsip_AesGcmEncrypt(
    struct Aes*  aes,     byte* out,
    const  byte* in,       word32 sz,
           byte* iv,       word32 ivSz,
           byte* authTag,  word32 authTagSz, /* auth Tag */
    const  byte* authIn,   word32 authInSz,  /* AAD */
           void* ctx)
{
    int                 ret = -1;
    e_tsip_err_t        err;
    tsip_gcm_handle_t   hdl;
    uint32_t            dataLen = sz;
    uint32_t            cipherBufSz;

    aesGcmEncInitFn     initFn;
    aesGcmEncUpdateFn   updateFn;
    aesGcmEncFinalFn    finalFn;

    uint8_t* plainBuf  = NULL;
    uint8_t* cipherBuf = NULL;
    uint8_t* aTagBuf   = NULL;
    uint8_t* aadBuf    = NULL;
    const uint8_t* iv_l = NULL;
    uint32_t ivSz_l = 0;
    
    tsip_aes_key_index_t key_client_aes;
    TsipUserCtx *userCtx;

    WOLFSSL_ENTER("wc_tsip_AesGcmEncrypt");

    if (aes == NULL || ctx == NULL || (ivSz == 0)   ||
       (sz != 0       && (in == NULL  || out == NULL)) ||
       (ivSz != 0     &&  iv == NULL) ||
       (authInSz != 0 && authIn == NULL)) {
        WOLFSSL_LEAVE("wc_tsip_AesGcmEncrypt", BAD_FUNC_ARG);
        return BAD_FUNC_ARG;
    }
    /* TSIP can handle 128 and 256 bit key only */
    if (aes->ctx.keySize != 16 && aes->ctx.keySize != 32) {
        WOLFSSL_MSG("illegal key size");
        WOLFSSL_LEAVE("wc_tsip_AesGcmEncrypt", BAD_FUNC_ARG);
        return  BAD_FUNC_ARG;
    }

    if (aes->ctx.keySize == 16) {
        initFn   = R_TSIP_Aes128GcmEncryptInit;
        updateFn = R_TSIP_Aes128GcmEncryptUpdate;
        finalFn  = R_TSIP_Aes128GcmEncryptFinal;
    }
    else {
        initFn   = R_TSIP_Aes256GcmEncryptInit;
        updateFn = R_TSIP_Aes256GcmEncryptUpdate;
        finalFn  = R_TSIP_Aes256GcmEncryptFinal;
    }

    userCtx = (TsipUserCtx*)ctx;

    /* buffer for cipher data output must be multiple of AES_BLOCK_SIZE */
    cipherBufSz = ((sz / AES_BLOCK_SIZE) + 1) * AES_BLOCK_SIZE;

    if ((ret = tsip_hw_lock()) == 0) {

        /* allocate buffers for plaintext, ciphertext, authTag and aad to make
         * sure those buffers 32bit aligned as TSIP requests.
         */
        plainBuf  = XMALLOC(sz, aes->heap, DYNAMIC_TYPE_AES);
        cipherBuf = XMALLOC(cipherBufSz, aes->heap, DYNAMIC_TYPE_AES);
        aTagBuf   = XMALLOC(TSIP_AES_GCM_AUTH_TAG_SIZE, aes->heap,
                                                        DYNAMIC_TYPE_AES);
        aadBuf    = XMALLOC(authInSz, aes->heap, DYNAMIC_TYPE_AES);

        if (plainBuf == NULL || cipherBuf == NULL || aTagBuf == NULL ||
                                                      aadBuf == NULL ) {
            WOLFSSL_MSG("wc_tsip_AesGcmEncrypt: buffer allocation failed");
            ret = -1;
        }

        if (ret == 0) {
            XMEMCPY(plainBuf, in, sz);
            ForceZero(cipherBuf, cipherBufSz);
            ForceZero(authTag, authTagSz);
            XMEMCPY(aadBuf, authIn, authInSz);
        }

    #if defined(WOLFSSL_RENESAS_TSIP_TLS)
        if (ret == 0 &&
            userCtx->session_key_set == 1) {
            /* generate AES-GCM session key. The key stored in
             * Aes.ctx.tsip_keyIdx is not used here.
             */
            err = R_TSIP_TlsGenerateSessionKey(
                    userCtx->tsip_cipher,
                    (uint32_t*)userCtx->tsip_masterSecret,
                    (uint8_t*) userCtx->tsip_clientRandom,
                    (uint8_t*) userCtx->tsip_serverRandom,
                    &iv[AESGCM_IMP_IV_SZ], /* use exp_IV */
                    NULL,
                    NULL,
                    &key_client_aes,
                    NULL,
                    NULL, NULL);
            if (err != TSIP_SUCCESS) {

                WOLFSSL_MSG("R_TSIP_TlsGenerateSessionKey failed");
                ret = -1;
            }
        } else
    #endif
        if (ret == 0 &&
            (userCtx->user_aes128_key_set == 1 ||
             userCtx->user_aes256_key_set == 1)) {
            if (aes->ctx.keySize == 32) {
                XMEMCPY(&key_client_aes, &userCtx->user_aes256_key_index,
                        sizeof(tsip_aes_key_index_t));
            }
            else {
                 XMEMCPY(&key_client_aes, &userCtx->user_aes128_key_index,
                        sizeof(tsip_aes_key_index_t));
            }
            
            iv_l = iv;
            ivSz_l = ivSz;
            
        }

        if (ret == 0) {

            /* Since generated session key is coupled to iv, no need to pass
             * iv init func.
             * It expects to pass iv when users create their own key.
             */
            err = initFn(&hdl, &key_client_aes, (uint8_t*)iv_l, ivSz_l);

            if (err == TSIP_SUCCESS) {
                err = updateFn(&hdl, NULL, NULL, 0UL, (uint8_t*)aadBuf, authInSz);
            }
            if (err == TSIP_SUCCESS) {
                err = updateFn(&hdl, plainBuf, cipherBuf, sz, NULL, 0UL);
            }
            if (err != TSIP_SUCCESS) {
                WOLFSSL_MSG("R_TSIP_AesXXXGcmEncryptUpdate: failed");
                ret = -1;
            }

            /* Once R_TSIP_AesxxxGcmEncryptInit or R_TSIP_AesxxxEncryptUpdate is
            * called, R_TSIP_AesxxxGcmEncryptFinal must be called regardless of
            * the result of the previous call. Otherwise, TSIP can not come out
            * from its error state and all the trailing APIs will fail.
            */
            dataLen = 0;
            err = finalFn(&hdl,
                          cipherBuf + (sz / AES_BLOCK_SIZE) * AES_BLOCK_SIZE,
                          &dataLen,
                          aTagBuf); /* aad of 16 bytes will be output */

            if (err == TSIP_SUCCESS) {
                /* copy encrypted data to out */
                XMEMCPY(out, cipherBuf, sz);

                /* copy auth tag to caller's buffer */
                XMEMCPY((void*)authTag, (void*)aTagBuf,
                                min(authTagSz, TSIP_AES_GCM_AUTH_TAG_SIZE ));

            }
            else {
                WOLFSSL_MSG("R_TSIP_AesxxxGcmEncryptFinal: failed");
                ret = -1;
            }
        }

        XFREE(plainBuf,  aes->heap, DYNAMIC_TYPE_AES);
        XFREE(cipherBuf, aes->heap, DYNAMIC_TYPE_AES);
        XFREE(aTagBuf,   aes->heap, DYNAMIC_TYPE_AES);
        XFREE(aadBuf,    aes->heap, DYNAMIC_TYPE_AES);

        tsip_hw_unlock();
    }
    return ret;
}
/*
 * Decrypt cipher data into plaindata and output authentication tag data.
 * The session key used for decryption is generated inside this function and
 * the key which has been generated and stored in Aes is not used.
 * parameter
 *  - aes:  Aes structure
 *  - out:  buffer where the plain text is output
 *  - in:   buffer where the cipher data is storead
 *  - sz:   size of cipher data and also means output size
 *  - iv:   iv should be consist of implicit-iv of 4 bytes and exp-iv of 8 bytes
 *  - authTag: buffer where the authentication data is stored.
 *  - authTagSz: buffer size for authentication data.
 *  - authIn: buffer where Additional Authentication Data(AAD) is stored
 *  - authInSz: AAD size
 * return 0 on success, otherwise on error.
 * Note: As of TSIPv1.13, only accept 128 and 256 bit of key size
 *
 */
int wc_tsip_AesGcmDecrypt(
    Aes*        aes,      byte* out,
    const byte* in,       word32 sz,
    const byte* iv,       word32 ivSz,
    const byte* authTag,  word32 authTagSz,
    const byte* authIn,   word32 authInSz,
          void* ctx)
{
    int                 ret = -1;
    e_tsip_err_t        err;
    tsip_gcm_handle_t   hdl;

    uint32_t            dataLen;
    uint32_t            plainBufSz;

    aesGcmDecInitFn     initFn;
    aesGcmDecUpdateFn   updateFn;
    aesGcmDecFinalFn    finalFn;

    uint8_t* cipherBuf = NULL;
    uint8_t* plainBuf  = NULL;
    uint8_t* aTagBuf   = NULL;
    uint8_t* aadBuf    = NULL;
    const uint8_t* iv_l = NULL;
    uint32_t ivSz_l = 0;

    tsip_aes_key_index_t key_server_aes;
    TsipUserCtx *userCtx;

    WOLFSSL_ENTER("wc_tsip_AesGcmDecrypt");

    if (aes == NULL || in == NULL || out == NULL || sz == 0 || ctx == NULL ||
        iv == 0 ||
        (authInSz != 0 && authIn == NULL) ||
        (authInSz == 0 && authIn != NULL) ||
        (authTagSz != 0 && authTag == NULL) ||
        (authTagSz == 0 && authTag != NULL)) {
        WOLFSSL_LEAVE("wc_tsip_AesGcmDecrypt", BAD_FUNC_ARG);
        return BAD_FUNC_ARG;
    }
    if (aes->ctx.keySize != 16 && aes->ctx.keySize != 32) {
        WOLFSSL_MSG("illegal key size");
        WOLFSSL_LEAVE("wc_tsip_AesGcmDecrypt", BAD_FUNC_ARG);
        return  BAD_FUNC_ARG;
    }

    if (aes->ctx.keySize == 16) {
        initFn   = R_TSIP_Aes128GcmDecryptInit;
        updateFn = R_TSIP_Aes128GcmDecryptUpdate;
        finalFn  = R_TSIP_Aes128GcmDecryptFinal;
    }
    else {
        initFn   = R_TSIP_Aes256GcmDecryptInit;
        updateFn = R_TSIP_Aes256GcmDecryptUpdate;
        finalFn  = R_TSIP_Aes256GcmDecryptFinal;
    }

    userCtx = (TsipUserCtx *)ctx;

    /* buffer for plain data output must be multiple of AES_BLOCK_SIZE */
    plainBufSz = ((sz / AES_BLOCK_SIZE) + 1) * AES_BLOCK_SIZE;

    if ((ret = tsip_hw_lock()) == 0) {

        /* allocate buffers for plaintext, cipher-text, authTag and AAD.
         * TSIP requests those buffers 32bit aligned.
         */
        cipherBuf = XMALLOC(sz, aes->heap, DYNAMIC_TYPE_AES);
        plainBuf  = XMALLOC(plainBufSz, aes->heap, DYNAMIC_TYPE_AES);
        aTagBuf   = XMALLOC(TSIP_AES_GCM_AUTH_TAG_SIZE, aes->heap,
                                                        DYNAMIC_TYPE_AES);
        aadBuf    = XMALLOC(authInSz, aes->heap, DYNAMIC_TYPE_AES);

        if (plainBuf == NULL || cipherBuf == NULL || aTagBuf == NULL ||
                                                        aadBuf == NULL) {
            ret = -1;
        }

        if (ret == 0) {
            ForceZero(plainBuf, plainBufSz);
            XMEMCPY(cipherBuf, in, sz);
            ForceZero(aTagBuf, TSIP_AES_GCM_AUTH_TAG_SIZE);
            XMEMCPY(aTagBuf,authTag,min(authTagSz, TSIP_AES_GCM_AUTH_TAG_SIZE));
            XMEMCPY(aadBuf, authIn, authInSz);
        }

    #if defined(WOLFSSL_RENESAS_TSIP_TLS)
        if (ret == 0 &&
            userCtx->session_key_set == 1) {
            /* generate AES-GCM session key. The key stored in
             * Aes.ctx.tsip_keyIdx is not used here.
             */
            err = R_TSIP_TlsGenerateSessionKey(
                    userCtx->tsip_cipher,
                    (uint32_t*)userCtx->tsip_masterSecret,
                    (uint8_t*) userCtx->tsip_clientRandom,
                    (uint8_t*) userCtx->tsip_serverRandom,
                    (uint8_t*)&iv[AESGCM_IMP_IV_SZ], /* use exp_IV */
                    NULL,
                    NULL,
                    NULL,
                    &key_server_aes,
                    NULL, NULL);
            if (err != TSIP_SUCCESS) {
                WOLFSSL_MSG("R_TSIP_TlsGenerateSessionKey failed");
                ret = -1;
            }
        } else

    #endif
        if (ret == 0 &&
            (userCtx->user_aes128_key_set == 1 ||
             userCtx->user_aes256_key_set == 1)) {
            if (aes->ctx.keySize == 32) {
                XMEMCPY(&key_server_aes, &userCtx->user_aes256_key_index,
                        sizeof(tsip_aes_key_index_t));
            }
            else {
                 XMEMCPY(&key_server_aes, &userCtx->user_aes128_key_index,
                        sizeof(tsip_aes_key_index_t));
            }

            iv_l = iv;
            ivSz_l = ivSz;

        }

        if (ret == 0) {
            /* since key_index has iv and ivSz in it, no need to pass them init
             * func. Pass NULL and 0 as 3rd and 4th parameter respectively.
             *
             * It expects to pass iv when users create their own key.
             */
            err = initFn(&hdl, &key_server_aes, (uint8_t*)iv_l, ivSz_l);

            if (err == TSIP_SUCCESS) {
                /* pass only AAD and it's size before passing cipher text */
                err = updateFn(&hdl, NULL, NULL, 0UL, (uint8_t*)authIn,
                                                                    authInSz);
            }
            if (err == TSIP_SUCCESS) {
                err = updateFn(&hdl, cipherBuf, plainBuf, sz, NULL, 0UL);
            }
            if (err != TSIP_SUCCESS) {
                WOLFSSL_MSG("R_TSIP_AesXXXGcmDecryptUpdate: failed in decrypt");
                ret = -1;
            }
            if (err == TSIP_SUCCESS) {
                dataLen = 0;
                err = finalFn(&hdl,
                        plainBuf + (sz / AES_BLOCK_SIZE) * AES_BLOCK_SIZE,
                        &dataLen,
                        aTagBuf,
                        min(16, authTagSz)); /* TSIP accepts upto 16 byte */
            }
            if (err == TSIP_SUCCESS) {
                /* copy plain data to out */
                XMEMCPY(out, plainBuf, sz);
            }
            else {
                WOLFSSL_MSG("R_TSIP_AesXXXGcmDecryptFinal: failed");
                ret = -1;
            }
        }

        XFREE(plainBuf,  aes->heap, DYNAMIC_TYPE_AES);
        XFREE(cipherBuf, aes->heap, DYNAMIC_TYPE_AES);
        XFREE(aTagBuf,   aes->heap, DYNAMIC_TYPE_AES);
        XFREE(aadBuf,    aes->heap, DYNAMIC_TYPE_AES);

        tsip_hw_unlock();
    }
    WOLFSSL_LEAVE("wc_tsip_AesGcmDecrypt", ret);
    return ret;
}
#endif /* WOLFSSL_RENESAS_TSIP_TLS) || WOLFSSL_RENESAS_TSIP_CRYPTONLY
         && NO_WOLFSSL_RENESAS_TSIP_CRYPT_AES */
#endif /* NO_AES */
