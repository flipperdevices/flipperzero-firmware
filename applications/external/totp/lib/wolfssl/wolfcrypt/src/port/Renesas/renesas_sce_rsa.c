/* renesas_sce_rsa.c
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
 
#if !defined(NO_RSA) && \
    defined(WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY)

#include <string.h>
#include <stdio.h>

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/rsa.h>
#include <wolfssl/wolfcrypt/port/Renesas/renesas-sce-crypt.h>

/* Make Rsa key for SCE and set it to callback ctx
 * Assumes to be called by Crypt Callback
 *
 * size   desired keylenth, in bits. supports 1024 or 2048 bits
 * ctx    Callback context including pointer to hold generated key
 * return FSP_SUCCESS(0) on Success, otherwise negative value
 */
WOLFSSL_LOCAL int wc_sce_MakeRsaKey(int size, void* ctx)
{
    fsp_err_t        ret;
    User_SCEPKCbInfo *info = (User_SCEPKCbInfo*)ctx;

    sce_rsa1024_wrapped_pair_key_t *wrapped_pair1024_key = NULL;
    sce_rsa2048_wrapped_pair_key_t *wrapped_pair2048_key = NULL;

    /* sanity check */
    if (ctx == NULL)
        return BAD_FUNC_ARG;
    

    if ((ret = wc_sce_hw_lock()) == 0) {
        if (size == 1024) {
            wrapped_pair1024_key = 
            (sce_rsa1024_wrapped_pair_key_t*)XMALLOC(
                sizeof(sce_rsa1024_wrapped_pair_key_t), NULL, 
                                                DYNAMIC_TYPE_RSA_BUFFER);
            if (wrapped_pair1024_key == NULL)
                return MEMORY_E;
                
            ret = R_SCE_RSA1024_WrappedKeyPairGenerate(wrapped_pair1024_key);
        }
        else if (size == 2048) {
            wrapped_pair2048_key = 
            (sce_rsa1024_wrapped_pair_key_t*)XMALLOC(
                sizeof(sce_rsa2048_wrapped_pair_key_t), NULL, 
                                                DYNAMIC_TYPE_RSA_BUFFER);
            if (wrapped_pair2048_key == NULL)
                return MEMORY_E;
                
            ret = R_SCE_RSA2048_WrappedKeyPairGenerate(wrapped_pair2048_key);
        }
        else
            return CRYPTOCB_UNAVAILABLE;
            
        if (ret == FSP_SUCCESS) {
            if (size == 1024) {
                if (info->sce_wrapped_key_rsapri1024 != NULL) {
                    XFREE(info->sce_wrapped_key_rsapri1024, NULL, 
                                                DYNAMIC_TYPE_RSA_BUFFER);
                }
                if (info->sce_wrapped_key_rsapub1024 != NULL) {
                    XFREE(info->sce_wrapped_key_rsapub1024, NULL, 
                                                DYNAMIC_TYPE_RSA_BUFFER);
                }
                info->sce_wrapped_key_rsapri1024 = 
                (sce_rsa1024_private_wrapped_key_t*)XMALLOC(
                    sizeof(sce_rsa1024_private_wrapped_key_t), NULL, 
                                                DYNAMIC_TYPE_RSA_BUFFER);
                    
                if (info->sce_wrapped_key_rsapri1024 == NULL) {
                    XFREE(wrapped_pair1024_key, 0, DYNAMIC_TYPE_RSA_BUFFER);
                    return MEMORY_E;
                }
                
                info->sce_wrapped_key_rsapub1024 =
                (sce_rsa1024_public_wrapped_key_t*)XMALLOC(
                    sizeof(sce_rsa1024_public_wrapped_key_t), NULL, 
                                                DYNAMIC_TYPE_RSA_BUFFER);
                    
                if (info->sce_wrapped_key_rsapub1024 == NULL) {
                    XFREE(wrapped_pair1024_key, 0, DYNAMIC_TYPE_RSA_BUFFER);
                    XFREE(info->sce_wrapped_key_rsapub1024, 0, 
                                                DYNAMIC_TYPE_RSA_BUFFER);
                    return MEMORY_E;
                }
                /* copy generated key pair and free malloced key */
                XMEMCPY(info->sce_wrapped_key_rsapri1024, 
                                    &wrapped_pair1024_key->priv_key,
                                    sizeof(sce_rsa1024_private_wrapped_key_t));
                XMEMCPY(info->sce_wrapped_key_rsapub1024, 
                                    &wrapped_pair1024_key->pub_key,
                                    sizeof(sce_rsa1024_public_wrapped_key_t));
                XFREE(wrapped_pair1024_key, 0, DYNAMIC_TYPE_RSA_BUFFER);
                
                info->keyflgs_crypt.bits.rsapri1024_installedkey_set = 1;
                info->keyflgs_crypt.bits.rsapub1024_installedkey_set = 1;
            }
            else if (size == 2048) {
                if (info->sce_wrapped_key_rsapri2048 != NULL) {
                    XFREE(info->sce_wrapped_key_rsapri2048, NULL, 
                                    DYNAMIC_TYPE_RSA_BUFFER);
                }
                if (info->sce_wrapped_key_rsapub2048 != NULL) {
                    XFREE(info->sce_wrapped_key_rsapub2048, NULL, 
                                    DYNAMIC_TYPE_RSA_BUFFER);
                }
                info->sce_wrapped_key_rsapri2048 = 
                (sce_rsa2048_private_wrapped_key_t*)XMALLOC(
                    sizeof(sce_rsa2048_private_wrapped_key_t), NULL, 
                                    DYNAMIC_TYPE_RSA_BUFFER);
                    
                if (info->sce_wrapped_key_rsapri2048 == NULL) {
                    XFREE(wrapped_pair2048_key, 0, DYNAMIC_TYPE_RSA_BUFFER);
                    return MEMORY_E;
                }
                
                info->sce_wrapped_key_rsapub2048 =
                (sce_rsa2048_public_wrapped_key_t*)XMALLOC(
                    sizeof(sce_rsa2048_public_wrapped_key_t), NULL, 
                                    DYNAMIC_TYPE_RSA_BUFFER);
                    
                if (info->sce_wrapped_key_rsapub2048 == NULL) {
                    XFREE(wrapped_pair2048_key, 0, DYNAMIC_TYPE_RSA_BUFFER);
                    XFREE(info->sce_wrapped_key_rsapub1024, 0, 
                                    DYNAMIC_TYPE_RSA_BUFFER);
                    return MEMORY_E;
                }
                /* copy generated key pair and free malloced key */
                XMEMCPY(info->sce_wrapped_key_rsapri2048, 
                            &wrapped_pair2048_key->priv_key,
                            sizeof(sce_rsa2048_private_wrapped_key_t));
                XMEMCPY(info->sce_wrapped_key_rsapub2048, 
                            &wrapped_pair2048_key->pub_key,
                            sizeof(sce_rsa2048_public_wrapped_key_t));
                XFREE(wrapped_pair2048_key, 0, DYNAMIC_TYPE_RSA_BUFFER);
                
                info->keyflgs_crypt.bits.rsapri2048_installedkey_set = 1;
                info->keyflgs_crypt.bits.rsapub2048_installedkey_set = 1;
                
            }
        }
        else {
            WOLFSSL_MSG("Failed to generate key pair by SCE");
            return CRYPTOCB_UNAVAILABLE;
        }
        
        wc_sce_hw_unlock();
    }
}

/* Perform rsa encryption/decryption by SCE
 * Assumes to be called by Crypt Callback
 *
 * in     Buffer to hold plain text
 * inLen  Length of plain text in bytes
 * out    Buffer to hold cipher text
 * outLen Length of cipher in bytes
 * key    Rsa key object
 * rng    rng object
 * ctx    Callback context
 * return FSP_SUCCESS(0) on Success, otherwise negative value
 */
WOLFSSL_LOCAL int wc_sce_RsaFunction(const byte* in, word32 inLen, byte* out,
                    word32 outLen, int type, struct RsaKey* key, 
                    struct WC_RNG* rng, void* ctx)
{
    int ret;
    
    sce_rsa_byte_data_t plain;
    sce_rsa_byte_data_t cipher;
    User_SCEPKCbInfo    *info = (User_SCEPKCbInfo*)ctx;
    
    int keySize;
    
    (void) key;
    (void) rng;
    
    /* sanity check */
    if (in == NULL || out == NULL || outLen == NULL ||
                                            ctx == NULL){
        return BAD_FUNC_ARG;
    }
    
    keySize = 0;
    if (info->keyflgs_crypt.bits.rsapri2048_installedkey_set == 1 ||
        info->keyflgs_crypt.bits.rsapub2048_installedkey_set == 1 )
        keySize = 2048;
    else if (info->keyflgs_crypt.bits.rsapri1024_installedkey_set == 1 ||
             info->keyflgs_crypt.bits.rsapub1024_installedkey_set == 1 )
        keySize = 1024;
    
    if (keySize == 0) {
        WOLFSSL_MSG("keySize is invalid, neither 128 or 256 bytes, "
                                                        "1024 or 2048 bits.");
        return BAD_FUNC_ARG;
    }
    
    if ((ret = wc_sce_hw_lock()) == 0) {
        if (type == RSA_PUBLIC_ENCRYPT) {
            
            plain.pdata = (byte*)in;
            plain.data_length = inLen;
            cipher.pdata = out;
            cipher.data_length = outLen;

            if (keySize == 1024) {
                if(info->keyflgs_crypt.bits.rsapub1024_installedkey_set == 1)
                    ret = R_SCE_RSAES_PKCS1024_Encrypt(&plain, &cipher,
                        (sce_rsa1024_public_wrapped_key_t*)
                            info->sce_wrapped_key_rsapub1024);
                else {
                    WOLFSSL_MSG("wrapped public 1024 bits key is not set.");
                    return BAD_FUNC_ARG;
                }
            }
            else {
                if(info->keyflgs_crypt.bits.rsapub2048_installedkey_set == 1)
                    ret = R_SCE_RSAES_PKCS2048_Encrypt(&plain, &cipher,
                            (sce_rsa2048_public_wrapped_key_t*)
                                info->sce_wrapped_key_rsapub2048);
                else {
                    WOLFSSL_MSG("wrapped public 2048 bits key is not set.");
                    return BAD_FUNC_ARG;
                }
            }
        }
        else if (type == RSA_PRIVATE_DECRYPT) {
            plain.pdata = out;
            plain.data_length = outLen;
            cipher.pdata = (byte*)in;
            cipher.data_length = inLen;
            
            if (keySize == 1024) {
                if(info->keyflgs_crypt.bits.rsapri1024_installedkey_set == 1)
                    ret = R_SCE_RSAES_PKCS1024_Decrypt(&cipher, &plain,
                            (sce_rsa1024_private_wrapped_key_t*)
                                info->sce_wrapped_key_rsapri1024);
                else {
                    WOLFSSL_MSG("wrapped private 2048 bits key is not set.");
                    return BAD_FUNC_ARG;
                }
            }
            else {
                if(info->keyflgs_crypt.bits.rsapri2048_installedkey_set == 1)
                    ret = R_SCE_RSAES_PKCS2048_Decrypt(&cipher, &plain,
                            (sce_rsa2048_private_wrapped_key_t*)
                                info->sce_wrapped_key_rsapri2048);
                else {
                    WOLFSSL_MSG("wrapped private 2048 bits key is not set.");
                    return BAD_FUNC_ARG;
                }
            }
        }
        
        wc_sce_hw_unlock();
    }
    return ret;
}

/* Perform Rsa sign by SCE
 * Assumes to be called by Crypt Callback
 * 
 * in     Buffer to hold plaintext
 * inLen  Length of plaintext in bytes
 * out    Buffer to hold generated signature
 * outLen Length of signature in bytes
 * key    rsa key object
 * ctx    The callback context
 * return FSP_SUCCESS(0) on Success, otherwise negative value
 */
 
WOLFSSL_LOCAL int wc_sce_RsaSign(const byte* in, word32 inLen, byte* out,
                    word32* outLen, struct RsaKey* key, void* ctx)
{
    int ret;
    
    sce_rsa_byte_data_t message_hash;
    sce_rsa_byte_data_t signature;
    User_SCEPKCbInfo    *info = (User_SCEPKCbInfo*)ctx;
    int keySize;
    
    (void) key;
    
    /* sanity check */
    if (in == NULL || out == NULL || outLen == NULL ||
                                key == NULL || ctx == NULL){
        return BAD_FUNC_ARG;
    }
    
    keySize = 0;
    if (info->keyflgs_crypt.bits.rsapri2048_installedkey_set == 1 ||
        info->keyflgs_crypt.bits.rsapub2048_installedkey_set == 1 )
        keySize = 2048;
    else if (info->keyflgs_crypt.bits.rsapri1024_installedkey_set == 1 ||
             info->keyflgs_crypt.bits.rsapub1024_installedkey_set == 1 )
        keySize = 1024;
        
    if (keySize == 0) {
        WOLFSSL_MSG("keySize is invalid, neither 1024 or 2048 bits.");
        return BAD_FUNC_ARG;
    }
    
    message_hash.pdata = in;
    message_hash.data_length = inLen;
    message_hash.data_type = 
            info->keyflgs_crypt.bits.message_type;/* message 0, hash 1 */
    signature.pdata = out;
    signature.data_length = outLen;
    
    if ((ret = wc_sce_hw_lock()) == 0) {
        if (keySize == 1024) {
            
            ret = R_SCE_RSASSA_PKCS1024_SignatureGenerate(&message_hash, 
                        &signature,
                        (sce_rsa1024_private_wrapped_key_t *)
                                    info->sce_wrapped_key_rsapri1024,
                        HW_SCE_RSA_HASH_SHA256);
        }
        else {
            
            ret = R_SCE_RSASSA_PKCS2048_SignatureGenerate(&message_hash, 
                        &signature,
                        (sce_rsa2048_private_wrapped_key_t *)
                                    info->sce_wrapped_key_rsapri2048,
                        HW_SCE_RSA_HASH_SHA256);
        }
        
        wc_sce_hw_unlock();
    }
    
    return ret;
}

/* Perform Rsa verify by SCE
 * Assumes to be called by Crypt Callback
 * 
 * in     Buffer to hold plaintext
 * inLen  Length of plaintext in bytes
 * out    Buffer to hold generated signature
 * outLen Length of signature in bytes
 * key    rsa key object
 * ctx    The callback context
 * return FSP_SUCCESS(0) on Success, otherwise negative value
 */
 
WOLFSSL_LOCAL int wc_sce_RsaVerify(const byte* in, word32 inLen, byte* out,
                    word32* outLen,struct RsaKey* key, void* ctx)
{
    int ret;
    
    sce_rsa_byte_data_t message_hash;
    sce_rsa_byte_data_t signature;
    User_SCEPKCbInfo    *info = (User_SCEPKCbInfo*)ctx;
    int keySize;
    
    (void) key;
    
    /* sanity check */
    if (in == NULL || out == NULL || outLen == NULL ||
                                key == NULL || ctx == NULL){
        return BAD_FUNC_ARG;
    }
    
    keySize = 0;
    if (info->keyflgs_crypt.bits.rsapri2048_installedkey_set == 1 ||
        info->keyflgs_crypt.bits.rsapub2048_installedkey_set == 1 )
        keySize = 2048;
    else if (info->keyflgs_crypt.bits.rsapri1024_installedkey_set == 1 ||
             info->keyflgs_crypt.bits.rsapub1024_installedkey_set == 1 )
        keySize = 1024;
        
    if (keySize == 0) {
        WOLFSSL_MSG("keySize is invalid, neither 1024 or 2048 bits.");
        return BAD_FUNC_ARG;
    }
    
    
    message_hash.pdata = in;
    message_hash.data_length = inLen;
    message_hash.data_type = 
            info->keyflgs_crypt.bits.message_type;/* message 0, hash 1 */
    
    signature.pdata = out;
    signature.data_length = outLen;
    
    if ((ret = wc_sce_hw_lock()) == 0) {
        if (keySize == 1024) {
            
            ret = R_SCE_RSASSA_PKCS1024_SignatureVerify(&signature,
                  &message_hash,
                  (sce_rsa1024_public_wrapped_key_t *)
                        info->sce_wrapped_key_rsapub1024,
                  HW_SCE_RSA_HASH_SHA256);
        }
        else {
            
                ret = R_SCE_RSASSA_PKCS2048_SignatureVerify(&signature, 
                    &message_hash,
                    (sce_rsa2048_public_wrapped_key_t *)
                         info->sce_wrapped_key_rsapub2048,
                    HW_SCE_RSA_HASH_SHA256 );
        }
        
        wc_sce_hw_unlock();
    }
    
    return ret;
}

#endif /* !NO_RSA && WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY */
