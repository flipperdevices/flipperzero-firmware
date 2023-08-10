/* renesas_common.c
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

#include <wolfssl/wolfcrypt/settings.h>

#if defined(WOLFSSL_RENESAS_SCEPROTECT) \
    || defined(WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY) \
    || defined(WOLFSSL_RENESAS_TSIP_TLS) \
    || defined(WOLFSSL_RENESAS_TSIP_CRYPTONLY)

#if defined(WOLFSSL_RENESAS_SCEPROTECT) || \
    defined(WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY)
  #include <wolfssl/wolfcrypt/port/Renesas/renesas-sce-crypt.h>
  #define cmn_hw_lock    wc_sce_hw_lock
  #define cmn_hw_unlock  wc_sce_hw_unlock
#elif defined(WOLFSSL_RENESAS_TSIP_TLS) || \
    defined(WOLFSSL_RENESAS_TSIP_CRYPTONLY)
  #include <wolfssl/wolfcrypt/port/Renesas/renesas-tsip-crypt.h>
  #define cmn_hw_lock    tsip_hw_lock
  #define cmn_hw_unlock  tsip_hw_unlock
#endif

#include <wolfssl/wolfcrypt/wc_port.h>
#include <wolfssl/wolfcrypt/types.h>
#include <wolfssl/wolfcrypt/asn.h>
#include <wolfssl/internal.h>
#include <wolfssl/error-ssl.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>

uint32_t   g_CAscm_Idx = (uint32_t)-1; /* index of CM table    */
static int gdevId = 7890;           /* initial dev Id for Crypt Callback */

#ifdef WOLF_CRYPTO_CB

#include <wolfssl/wolfcrypt/cryptocb.h>

#if defined(WOLFSSL_RENESAS_TSIP_TLS)
WOLFSSL_LOCAL int Renesas_cmn_Cleanup(WOLFSSL* ssl)
{
    int ret = 0;
    WOLFSSL_ENTER("Renesas_cmn_Cleanup");

#if defined(WOLFSSL_RENESAS_TSIP_TLS)
    ret = tsip_TlsCleanup(ssl);
#endif

    WOLFSSL_LEAVE("Renesas_cmn_Cleanup", ret);
    return ret;
}
WOLFSSL_LOCAL int Renesas_cmn_RsaSignCb(WOLFSSL* ssl,
                                const unsigned char* in, unsigned int inSz,
                                unsigned char* out, word32* outSz,
                                const unsigned char* keyDer, unsigned int keySz,
                                void* ctx)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    WOLFSSL_ENTER("Renesas_cmn_RsaSignCb");

    /* This is just a stub function that provides no logic */

    WOLFSSL_LEAVE("Renesas_cmn_RsaSignCb", ret);
    return ret;
}
/* This function is a callback passed to wolfSSL_CTX_SetRsaSignCheckCb.
 * It tries to verify the signature passed to it by decrypting with a public
 * key.
 * returns 0 on success, CRYPTOCB_UNAVAILABLE when public key is not set.
 */
WOLFSSL_LOCAL int Renesas_cmn_RsaSignCheckCb(WOLFSSL* ssl,
                                unsigned char* sig, unsigned int sigSz,
                                unsigned char** out,
                                const unsigned char* keyDer, unsigned int keySz,
                                void* ctx)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    WOLFSSL_ENTER("Renesas_cmn_RsaSignCheckCb");

    #if defined(WOLFSSL_RENESAS_TSIP)

    return tsip_VerifyRsaPkcsCb(ssl, sig, sigSz, out, keyDer, keySz, ctx);

    #endif /* WOLFSSL_RENESAS_TSIP */

    WOLFSSL_LEAVE("Renesas_cmn_RsaSignCheckCb", ret);
    return ret;
}

WOLFSSL_LOCAL int Renesas_cmn_EccSignCb(WOLFSSL* ssl,
                                const unsigned char* in, unsigned int inSz,
                                unsigned char* out, word32* outSz,
                                const unsigned char* keyDer, unsigned int keySz,
                                void* ctx)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    WOLFSSL_ENTER("Renesas_cmn_EccSignCb");

    /* This is just a stub function that provides no logic */

    WOLFSSL_LEAVE("Renesas_cmn_EccSignCb", ret);
    return ret;
}
#endif /* WOLFSSL_RENESAS_TSIP_TLS */

/* Renesas Security Library Common Callback
 * For Crypto Callbacks
 *
 * devIdArg device Id
 * info     pointer to wc_CryptInfo
 * ctx      Crypto Callback context
 * return  0 on success, otherwise MEMORY_E or BAD_FUNC_ARG on failure
 */
static int Renesas_cmn_CryptoDevCb(int devIdArg, wc_CryptoInfo* info, void* ctx)
{
    int ret = NOT_COMPILED_IN; /* return this to bypass HW and use SW */

    WOLFSSL_ENTER("Renesas_cmn_CryptoDevCb");

#if defined(WOLFSSL_RENESAS_TSIP_TLS) \
    || defined(WOLFSSL_RENESAS_TSIP_CRYPTONLY)
    TsipUserCtx*      cbInfo = (TsipUserCtx*)ctx;
#elif defined(WOLFSSL_RENESAS_SCEPROTECT) || \
        defined(WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY)
    User_SCEPKCbInfo* cbInfo = (User_SCEPKCbInfo*)ctx;
#endif

    if (info == NULL || ctx == NULL)
        return BAD_FUNC_ARG;

#ifdef DEBUG_WOLFSSL
    printf("CryptoDevCb: Algo Type %d session key set: %d\n",
                                    info->algo_type, cbInfo->session_key_set);
#endif

#if defined(WOLFSSL_RENESAS_TSIP) \
    || defined(WOLFSSL_RENESAS_TSIP_CRYPTONLY)
    ret = CRYPTOCB_UNAVAILABLE;

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
    #if defined(WOLFSSL_KEY_GEN)
    if (info->pk.type == WC_PK_TYPE_RSA_KEYGEN &&
            (info->pk.rsakg.size == 1024 ||
            info->pk.rsakg.size == 2048)) {
        ret = wc_tsip_MakeRsaKey(info->pk.rsakg.size, (void*)ctx);
    }
  #endif

    /* Is called for signing
     * Can handle only RSA PkCS#1v1.5 padding scheme here.
    */
    if (info->algo_type == WC_ALGO_TYPE_PK) {
        #if !defined(NO_RSA)
        if (info->pk.type == WC_PK_TYPE_RSA) {
            if (info->pk.rsa.type == RSA_PRIVATE_ENCRYPT) {
                ret = tsip_SignRsaPkcs(info, ctx);
            }
            #if defined(WOLFSSL_RENESAS_TSIP_CRYPTONLY)
            else if (info->pk.rsa.type == RSA_PUBLIC_DECRYPT /* verify */) {
                    ret = wc_tsip_RsaVerifyPkcs(info, ctx);
            }
            #endif
        }
        #endif /* NO_RSA */
        #if defined(HAVE_ECC) && defined(WOLFSSL_RENESAS_TSIP_TLS)
        else if (info->pk.type == WC_PK_TYPE_ECDSA_SIGN) {
            ret = tsip_SignEcdsa(info, ctx);
        }
        #endif /* HAVE_ECC */
    }
#elif defined(WOLFSSL_RENESAS_SCEPROTECT) ||\
        defined(WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY)

    if (info->algo_type == WC_ALGO_TYPE_CIPHER) {

    #if !defined(NO_AES) || !defined(NO_DES3)
    #ifdef HAVE_AESGCM
        if (info->cipher.type == WC_CIPHER_AES_GCM) {

            if (info->cipher.enc &&
                (cbInfo->keyflgs_tls.bits.session_key_set == 1 ||
                 (cbInfo->keyflgs_crypt.bits.aes256_installedkey_set == 1 &&
                  info->cipher.aesgcm_enc.aes->keylen == 32) ||
                 (cbInfo->keyflgs_crypt.bits.aes128_installedkey_set == 1 &&
                  info->cipher.aesgcm_enc.aes->keylen == 16))) {

                if (cbInfo->keyflgs_crypt.bits.aes256_installedkey_set == 1 &&
                  info->cipher.aesgcm_enc.aes->keylen == 32) {

                    XMEMCPY(&info->cipher.aesgcm_enc.aes->ctx.sce_wrapped_key,
                        &cbInfo->sce_wrapped_key_aes256,
                        sizeof(sce_aes_wrapped_key_t));
                    info->cipher.aesgcm_enc.aes->ctx.keySize = 32;

                }
                else if (
                    cbInfo->keyflgs_crypt.bits.aes128_installedkey_set == 1 &&
                    info->cipher.aesgcm_enc.aes->keylen == 16) {

                    XMEMCPY(&info->cipher.aesgcm_enc.aes->ctx.sce_wrapped_key,
                            &cbInfo->sce_wrapped_key_aes128,
                            sizeof(sce_aes_wrapped_key_t));
                    info->cipher.aesgcm_enc.aes->ctx.keySize = 16;
                }

                ret = wc_sce_AesGcmEncrypt(
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
            else if (cbInfo->keyflgs_tls.bits.session_key_set == 1 ||
                    (cbInfo->keyflgs_crypt.bits.aes256_installedkey_set == 1 &&
                       info->cipher.aesgcm_dec.aes->keylen == 32) ||
                    (cbInfo->keyflgs_crypt.bits.aes128_installedkey_set == 1 &&
                       info->cipher.aesgcm_dec.aes->keylen == 16)) {

                if (cbInfo->keyflgs_crypt.bits.aes256_installedkey_set == 1 &&
                  info->cipher.aesgcm_dec.aes->keylen == 32) {

                    XMEMCPY(&info->cipher.aesgcm_dec.aes->ctx.sce_wrapped_key,
                            &cbInfo->sce_wrapped_key_aes256,
                            sizeof(sce_aes_wrapped_key_t));
                    info->cipher.aesgcm_dec.aes->ctx.keySize = 32;

                }
                else if (
                    cbInfo->keyflgs_crypt.bits.aes128_installedkey_set == 1 &&
                    info->cipher.aesgcm_dec.aes->keylen == 16) {

                    XMEMCPY(&info->cipher.aesgcm_dec.aes->ctx.sce_wrapped_key,
                            &cbInfo->sce_wrapped_key_aes128,
                            sizeof(sce_aes_wrapped_key_t));
                    info->cipher.aesgcm_dec.aes->ctx.keySize = 16;
                }

                ret = wc_sce_AesGcmDecrypt(
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
        if ((info->cipher.type == WC_CIPHER_AES_CBC) &&
            (cbInfo->keyflgs_tls.bits.session_key_set == 1 ||
            (cbInfo->keyflgs_crypt.bits.aes256_installedkey_set == 1 &&
                info->cipher.aescbc.aes->keylen == 32) ||
            (cbInfo->keyflgs_crypt.bits.aes128_installedkey_set == 1 &&
                info->cipher.aescbc.aes->keylen == 16))) {

                if (info->cipher.enc) {
                    if (
                    cbInfo->keyflgs_crypt.bits.aes256_installedkey_set == 1 &&
                    info->cipher.aescbc.aes->keylen == 32) {
                        XMEMCPY(&info->cipher.aescbc.aes->ctx.sce_wrapped_key,
                                &cbInfo->sce_wrapped_key_aes256,
                                sizeof(sce_aes_wrapped_key_t));
                        info->cipher.aescbc.aes->ctx.keySize = 32;

                    }
                    else if (
                        cbInfo->keyflgs_crypt.bits.aes128_installedkey_set == 1
                        && info->cipher.aescbc.aes->keylen == 16) {
                        XMEMCPY(&info->cipher.aescbc.aes->ctx.sce_wrapped_key,
                                &cbInfo->sce_wrapped_key_aes128,
                                sizeof(sce_aes_wrapped_key_t));
                        info->cipher.aescbc.aes->ctx.keySize = 16;
                    }

                    ret = wc_sce_AesCbcEncrypt(
                        info->cipher.aescbc.aes,
                        (byte*)info->cipher.aescbc.out,
                        (byte*)info->cipher.aescbc.in,
                        info->cipher.aescbc.sz);
                }
                else {
                    if (
                    cbInfo->keyflgs_crypt.bits.aes256_installedkey_set == 1 &&
                    info->cipher.aescbc.aes->keylen == 32) {
                        XMEMCPY(&info->cipher.aescbc.aes->ctx.sce_wrapped_key,
                                &cbInfo->sce_wrapped_key_aes256,
                                sizeof(sce_aes_wrapped_key_t));
                        info->cipher.aescbc.aes->ctx.keySize = 32;
                    }  else if (
                        cbInfo->keyflgs_crypt.bits.aes128_installedkey_set == 1
                        && info->cipher.aescbc.aes->keylen == 16) {
                        XMEMCPY(&info->cipher.aescbc.aes->ctx.sce_wrapped_key,
                                &cbInfo->sce_wrapped_key_aes128,
                                sizeof(sce_aes_wrapped_key_t));
                        info->cipher.aescbc.aes->ctx.keySize = 16;
                    }

                    ret = wc_sce_AesCbcDecrypt(
                        info->cipher.aescbc.aes,
                        (byte*)info->cipher.aescbc.out,
                        (byte*)info->cipher.aescbc.in,
                        info->cipher.aescbc.sz);
                }
        }
    #endif /* HAVE_AES_CBC */
    #endif /* !NO_AES || !NO_DES3 */
    }
    #if !defined(NO_RSA) && defined(WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY)
    else if (info->algo_type == WC_ALGO_TYPE_PK) {

       #if !defined(NO_RSA)
       #if defined(WOLFSSL_KEY_GEN)
        if (info->pk.type == WC_PK_TYPE_RSA_KEYGEN &&
            (info->pk.rsakg.size == 1024 ||
             info->pk.rsakg.size == 2048)) {
            ret = wc_sce_MakeRsaKey(info->pk.rsakg.size, (void*)ctx);
        }
       #endif
        if (info->pk.type == WC_PK_TYPE_RSA) {
            /* to perform RSA on SCE, wrapped keys should be installed
             * in advance. SCE supports 1024 or 2048 bits key size.
             * otherwise, falls-through happens.
             */
            if (cbInfo->keyflgs_crypt.bits.rsapri2048_installedkey_set == 1
                 ||
                 cbInfo->keyflgs_crypt.bits.rsapub2048_installedkey_set == 1
                ||
                cbInfo->keyflgs_crypt.bits.rsapri1024_installedkey_set == 1
                 ||
                 cbInfo->keyflgs_crypt.bits.rsapub1024_installedkey_set == 1
                ) {

                if (info->pk.rsa.type == RSA_PRIVATE_DECRYPT ||
                    info->pk.rsa.type == RSA_PUBLIC_ENCRYPT  )
                    {
                        ret = wc_sce_RsaFunction(info->pk.rsa.in,
                                        info->pk.rsa.inLen,
                                        info->pk.rsa.out,
                                        info->pk.rsa.outLen,
                                        info->pk.rsa.type,
                                        info->pk.rsa.key,
                                        info->pk.rsa.rng,
                                        (void*)ctx);
                }
                else if (info->pk.rsa.type == RSA_PRIVATE_ENCRYPT /* sign */){
                   ret = wc_sce_RsaSign(info->pk.rsa.in,
                                        info->pk.rsa.inLen,
                                        info->pk.rsa.out,
                                        info->pk.rsa.outLen,
                                        info->pk.rsa.key,
                                        (void*)ctx);
                }
                else if (info->pk.rsa.type == RSA_PUBLIC_DECRYPT /* verify */) {
                    ret = wc_sce_RsaVerify(info->pk.rsa.in,
                                        info->pk.rsa.inLen,
                                        info->pk.rsa.out,
                                        info->pk.rsa.outLen,
                                        info->pk.rsa.key,
                                        (void*)ctx);
                }
            }
            else {
                WOLFSSL_MSG(
                    "SCE can handle 1024 or 2048 bit key size. "
                    "key size is not either 1024 or 2048. "
                    "Or wrapped key is not installed. "
                    "RSA operation falls through to SW operation.");
            }
        }
       #endif /* NO_RSA && WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY */
    }
    #endif /* NO_RSA */
#endif /* TSIP or SCE */

    (void)devIdArg;
    (void)ctx;
    WOLFSSL_LEAVE("Renesas_cmn_CryptoDevCb", ret);
    return ret;
}

/* Renesas Security Library Common Entry Point
 * For usable method
 *
 * ssl     : a pointer to WOLFSSL object
 * session_key_generated : if session key has been generated
 * return  1 for usable, 0 for unusable
 */
int Renesas_cmn_usable(const WOLFSSL* ssl, byte session_key_generated)
{
    int ret;

    #if defined(WOLFSSL_RENESAS_TSIP_TLS)
        ret = tsip_usable(ssl, session_key_generated);
    #elif defined(WOLFSSL_RENESAS_SCEPROTECT) ||\
            defined(WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY)
        ret = wc_sce_usable(ssl, session_key_generated);
    #endif

    return ret;
}

/* Renesas Security Library Common Method
 * Crypt Callback initialization
 *
 * ssl     : a pointer to WOLFSSL object
 * ctx     : callback context
 * return  valid device Id on success, otherwise INVALID_DEVIID
 *         device Id starts from 7890, and increases + 1 its number
 *         when the method is successfully called.
 */
int wc_CryptoCb_CryptInitRenesasCmn(WOLFSSL* ssl, void* ctx)
{
    (void)ssl;
    (void)ctx;

 #if defined(WOLFSSL_RENESAS_TSIP_TLS) \
    || defined(WOLFSSL_RENESAS_TSIP_CRYPTONLY)
    TsipUserCtx* cbInfo = (TsipUserCtx*)ctx;
 #elif defined(WOLFSSL_RENESAS_SCEPROTECT) || \
       defined(WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY)
    User_SCEPKCbInfo* cbInfo = (User_SCEPKCbInfo*)ctx;
 #endif

    if (cbInfo == NULL
   #if (!defined(WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY) &&\
        !defined(WOLFSSL_RENESAS_TSIP_CRYPTONLY)) && \
       !defined(HAVE_RENESAS_SYNC)
        || ssl == NULL) {
   #else
     ) {
   #endif
        printf("Invalid devId\n");
        return INVALID_DEVID;
    }
    /* need exclusive control because of static variable */
    if ((cmn_hw_lock()) == 0) {
        cbInfo->devId = gdevId++;
        cmn_hw_unlock();
    }
    else {
        WOLFSSL_MSG("Failed to lock tsip hw");
        return INVALID_DEVID;
    }

    if (wc_CryptoCb_RegisterDevice(cbInfo->devId,
                            Renesas_cmn_CryptoDevCb, cbInfo) < 0) {
        /* undo devId number */
        gdevId--;
        return INVALID_DEVID;
    }

   #if !defined(WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY) && \
       !defined(WOLFSSL_RENESAS_TSIP_CRYPTONLY) && \
       !defined(HAVE_RENESAS_SYNC)
    if (ssl)
        wolfSSL_SetDevId(ssl, cbInfo->devId);
   #endif
    /* sanity check for overflow */
    if (gdevId < 0) {
        gdevId = 7890;
    }

    return cbInfo->devId;
}

/* Renesas Security Library Common Method
 * Clean up CryptCb
 *
 * id     : a pointer to device id to unregister
 * no return value
 */
void wc_CryptoCb_CleanupRenesasCmn(int* id)
{
    wc_CryptoCb_UnRegisterDevice(*id);
    *id = INVALID_DEVID;
}

#endif /* WOLF_CRYPTO_CB */
#endif /* WOLFSSL_RENESAS_SCEPROTECT || WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY
          WOLFSSL_RENESAS_TSIP_TLS || WOLFSSL_RENESAS_TSIP_CRYPTONLY */

#if defined(WOLFSSL_RENESAS_SCEPROTECT) || defined(WOLFSSL_RENESAS_TSIP_TLS)

/* Renesas Security Library Common Method
 * Check CA index if CA can be used for SCE/TSIP because
 * the CA has been verified by SCE/TSIP
 *
 * cmdIdx : ca index
 * return 1 can be used, otherwise 0
 */
WOLFSSL_LOCAL byte Renesas_cmn_checkCA(word32 cmIdx)
{
    WOLFSSL_ENTER("Renesas_cmn_checkCA");
    return (cmIdx == g_CAscm_Idx? 1:0);
}

/* check if the root CA has been verified by TSIP/SCE,
 * and it exists in the CM table.
 */
static byte sce_tsip_rootCAverified(void)
{
    WOLFSSL_ENTER("sce_tsip_rootCAverified");
    return (g_CAscm_Idx != (uint32_t)-1 ? 1:0);
}
/* Renesas Security Library Common Callback
 * Callback for Rsa verify
 *
 * ssl      the WOLFSSL object
 * sig      Buffer holding signature
 * sigSz    Length of signature in bytes
 * out      Buffer to hold hash
 * key      Buffer to hold ecc key
 * keySz    Length of key in bytes
 * return FSP_SUCCESS(0) on success, otherwise FSP/TSIP error code
 */
WOLFSSL_LOCAL int Renesas_cmn_RsaVerify(WOLFSSL* ssl, unsigned char* sig,
          unsigned int sigSz, unsigned char** out,
          const unsigned char* key, unsigned int keySz, void* ctx)
{
    int ret = 0;

    WOLFSSL_ENTER("Renesas_cmn_RsaVerify");

#if defined(WOLFSSL_RENESAS_TSIP_TLS)
    ret = wc_tsip_RsaVerify(ssl, sig, sigSz, out, key, keySz, ctx);

    if (ret == 0) {
        /* Set Callback for SharedSecret when successful */
        wolfSSL_CTX_SetEccSharedSecretCb(ssl->ctx, wc_tsip_EccSharedSecret);
        wolfSSL_SetEccSharedSecretCtx(ssl, ctx);
    }
    else {
        WOLFSSL_MSG("failed wc_tsip_RsaVerify");
        wolfSSL_CTX_SetEccSharedSecretCb(ssl->ctx, NULL);
        wolfSSL_SetEccSharedSecretCtx(ssl, NULL);
    }

#elif defined(WOLFSSL_RENESAS_SCEPROTECT)
    ret = wc_SCE_RsaVerify(ssl, sig, sigSz, out,key, keySz, ctx);

    if (ret == 0) {
        /* Set Callback for SharedSecret when successful */
        wolfSSL_CTX_SetEccSharedSecretCb(ssl->ctx, SCE_EccSharedSecret);
        wolfSSL_SetEccSharedSecretCtx(ssl, ctx);
    }
    else {
        WOLFSSL_MSG("failed R_SCE_TLS_ServerKeyExchangeVerify");
        wolfSSL_CTX_SetEccSharedSecretCb(ssl->ctx, NULL);
        wolfSSL_SetEccSharedSecretCtx(ssl, NULL);
    }
#endif

    return ret;
}
/* Renesas Security Library Common Callback
 * Callback for Ecc verify
 *
 * ssl      the WOLFSSL object
 * sig      Buffer holding signature
 * sigSz    Length of signature in bytes
 * hash     Buffer to hold hash
 * hashSz   Length of hash
 * key      Buffer to hold ecc key
 * keySz    Length of key in bytes
 * result   a pointer to int indicates if the verify is ok
 * return FSP_SUCCESS(0) on success, otherwise FSP/TSIP error code
 */
WOLFSSL_LOCAL int Renesas_cmn_EccVerify(WOLFSSL* ssl, const unsigned char* sig,
          unsigned int sigSz, const unsigned char* hash, unsigned int hashSz,
        const unsigned char* key, unsigned int keySz, int* result, void* ctx)
{
    int ret = 0;

    WOLFSSL_ENTER("Renesas_cmn_EccVerify");

#if defined(WOLFSSL_RENESAS_TSIP)
    ret = wc_tsip_EccVerify(ssl, sig, sigSz, hash, hashSz, key, keySz,
                                                        result, ctx);
    if (ret == 0 && *result == 1) {
        /* Set callback for SharedSecret when being successful */
        wolfSSL_CTX_SetEccSharedSecretCb(ssl->ctx, wc_tsip_EccSharedSecret);
        wolfSSL_SetEccSharedSecretCtx(ssl, ctx);
    }
    else {
        WOLFSSL_MSG("failed wc_tsip_EccVerify");
        wolfSSL_CTX_SetEccSharedSecretCb(ssl->ctx, NULL);
        wolfSSL_SetEccSharedSecretCtx(ssl, NULL);
    }
#elif defined(WOLFSSL_RENESAS_SCEPROTECT)
    ret = wc_SCE_EccVerify(ssl, sig, sigSz, hash, hashSz, key, keySz,
                                                        result, ctx);
    if (ret == 0 && *result == 1) {
        /* Set callback for SharedSecret when being successful */
        wolfSSL_CTX_SetEccSharedSecretCb(ssl->ctx, SCE_EccSharedSecret);
        wolfSSL_SetEccSharedSecretCtx(ssl, ctx);
    }
    else {
        WOLFSSL_MSG("failed R_SCE_TLS_ServerKeyExchangeVerify");
        wolfSSL_CTX_SetEccSharedSecretCb(ssl->ctx, NULL);
        wolfSSL_SetEccSharedSecretCtx(ssl, NULL);
    }
#endif

    return ret;
}
/* Renesas Security Library Common Entry Point
 * For ROOT CA verifycation
 *
 * cert     Buffer to hold cert
 * cert_len Length of cert
 * key_n_start Byte position of public key in cert
 * key_n_len   Length of public key in bytes
 * key_e_start Byte position of public key exponent in cert
 * key_e_len   Length of public key exponent
 * cm_row      CA index
 * return FSP_SUCCESS(0) on success, otherwise WOLFSSL_FATAL_ERROR
 */
int wc_Renesas_cmn_RootCertVerify(const byte* cert, word32 cert_len,
        word32 key_n_start, word32 key_n_len, word32 key_e_start,
        word32 key_e_len, word32 cm_row)
{
    int ret;

    WOLFSSL_ENTER("wc_Renesas_cmn_RootCertVerify");

    if (sce_tsip_rootCAverified() == 0) {

    #if defined(WOLFSSL_RENESAS_TSIP_TLS)
        ret = wc_tsip_tls_RootCertVerify(cert, cert_len, key_n_start,
                key_n_len, key_e_start, key_e_len, cm_row);
        if (ret != TSIP_SUCCESS) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    #elif defined(WOLFSSL_RENESAS_SCEPROTECT)

        ret = wc_sce_tls_RootCertVerify(cert, cert_len, key_n_start,
                key_n_len, key_e_start, key_e_len, cm_row);
        if (ret != FSP_SUCCESS) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    #endif

    }
    else {
        /* already verified. skipped */
        ret = 0;
    }
    WOLFSSL_LEAVE("wc_Renesas_cmn_RootCertVerify", ret);
    return ret;
}

/* Renesas Security Library Common Callback
 * Callback for tls finished
 *
 * ssl      the WOLFSSL object
 * side     CLIENT or SERVER
 * handshake_hash hash while doing handshake
 * hashes  calculated data by SCE/TSIP pseudo random function
 * return FSP_SUCCESS(0) on success, otherwise FSP/TSIP error code
 */
WOLFSSL_LOCAL int Renesas_cmn_TlsFinished(WOLFSSL* ssl, const byte *side,
                            const byte *handshake_hash, word32 hashSz,
                            byte *hashes, void* ctx)
{
    int ret = -1;

    (void)hashSz;
    (void)ctx;

    WOLFSSL_ENTER("Renesas_cmn_TlsFinished");

    if (Renesas_cmn_usable(ssl, 1)) {
 #if defined(WOLFSSL_RENESAS_TSIP_TLS)
        ret = wc_tsip_generateVerifyData(ssl->arrays->tsip_masterSecret,
                            side, handshake_hash, hashes);
 #elif defined(WOLFSSL_RENESAS_SCEPROTECT)
         ret = wc_sce_generateVerifyData(ssl->arrays->sce_masterSecret,
                   side, handshake_hash, hashes);
 #endif
    }
     else
         ret = PROTOCOLCB_UNAVAILABLE;

    return ret;
}

/* Renesas Security Library Common Callback
 * Callback for setting Encrypt Keys.
 * Register callback for setting Encrypt Keys when keys are generated
 * by SCE/TSIP
 *
 * ssl      the WOLFSSL object
 * ctx      Callback context
 * return 0 on success, -1 when keys are not generated by SCE/TSIP
 */
static int Renesas_cmn_EncryptKeys(WOLFSSL* ssl, void* ctx)
{
    int ret;

    WOLFSSL_ENTER("Renesas_cmn_EncryptKeys");

    /* sanity check */
    if (ssl == NULL || ctx == NULL)
        return BAD_FUNC_ARG;

 #if defined(WOLFSSL_RENESAS_TSIP_TLS)
    TsipUserCtx* cbInfo = (TsipUserCtx*)ctx;

    if (cbInfo->session_key_set == 1) {
 #elif defined(WOLFSSL_RENESAS_SCEPROTECT)
    User_SCEPKCbInfo* cbInfo = (User_SCEPKCbInfo*)ctx;


    if (cbInfo->keyflgs_tls.bits.session_key_set == 1) {
 #endif
        ret = 0;

        wolfSSL_CTX_SetTlsFinishedCb(ssl->ctx, Renesas_cmn_TlsFinished);
        wolfSSL_SetTlsFinishedCtx(ssl, cbInfo);
    }
    else {
        wolfSSL_CTX_SetTlsFinishedCb(ssl->ctx, NULL);
        wolfSSL_SetTlsFinishedCtx(ssl, NULL);
        ret = -1;
    }

    return ret;
}

/* Renesas Security Library Common Callback
 * Callback for Session Key generation
 * Register callback for Set Keys when keys are successfully
 * generated by SCE/TSIP
 *
 * ssl      the WOLFSSL object
 * ctx      Callback context
 * return FSP_SUCCESS(0) on success, otherwise SCE/TSIP error code
 */
WOLFSSL_LOCAL int Renesas_cmn_generateSessionKey(WOLFSSL* ssl, void* ctx)
{
    int ret = -1;
#if defined(WOLFSSL_RENESAS_TSIP_TLS)
    TsipUserCtx*      cbInfo = (TsipUserCtx*)ctx;
#elif defined(WOLFSSL_RENESAS_SCEPROTECT)
    User_SCEPKCbInfo* cbInfo = (User_SCEPKCbInfo*)ctx;
#endif
    (void)ctx;

    WOLFSSL_ENTER("Renesas_cmn_generateSessionKey");
    if (Renesas_cmn_usable(ssl, 0)) {
#if defined(WOLFSSL_RENESAS_TSIP_TLS)
        ret = wc_tsip_generateSessionKey(ssl, (TsipUserCtx*)ctx, cbInfo->devId);
#elif defined(WOLFSSL_RENESAS_SCEPROTECT)
        ret = wc_sce_generateSessionKey(ssl, ctx, cbInfo->devId);
#endif
    }
    else {
         ret = PROTOCOLCB_UNAVAILABLE;
    }

    if (ret == 0) {
        wolfSSL_CTX_SetEncryptKeysCb(ssl->ctx, Renesas_cmn_EncryptKeys);
        wolfSSL_SetEncryptKeysCtx(ssl, ctx);
    }
    else {
        wolfSSL_CTX_SetEncryptKeysCb(ssl->ctx, NULL);
        wolfSSL_SetEncryptKeysCtx(ssl, NULL);
    }

    return ret;
}

/* Renesas Security Library Common Callback
 * Callback for Premaster Secret generation
 * Register callback for Set Keys when keys are successfully
 * generated by SCE/TSIP
 *
 * ssl      the WOLFSSL object
 * premaster Buffer to hold pre master
 * preSz     Length of pre-master
 * ctx       Callback context
 * return FSP_SUCCESS(0) on success,
 * otherwise PROTOCOLCB_UNAVAILABLE
 *          so that caller could continue to process if want
 */
WOLFSSL_LOCAL int Renesas_cmn_generatePremasterSecret(WOLFSSL* ssl,
                            byte *premaster, word32 preSz, void* ctx)
{
    int ret;

    (void) ctx;
    (void) ssl;

    WOLFSSL_ENTER("Renesas_cmn_generatePremasterSecret");

#if defined(WOLFSSL_RENESAS_TSIP_TLS)
    if (Renesas_cmn_usable(ssl, 0)) {
        ret = wc_tsip_generatePremasterSecret(premaster, preSz);
        ssl->arrays->preMasterSz = preSz;
    }
    else
        ret = PROTOCOLCB_UNAVAILABLE;

#elif defined(WOLFSSL_RENESAS_SCEPROTECT)
    if (Renesas_cmn_usable(ssl, 0)) {
        ret = wc_sce_generatePremasterSecret(premaster, preSz);
        ssl->arrays->preMasterSz = preSz;
    }
    else
        ret = PROTOCOLCB_UNAVAILABLE;
#endif

    return ret;
}

/* Renesas Security Library Common Callback
 * Callback for Master Secret generation
 * Register callback for Session Key Generation when master secret is
 * successfully generated by SCE/TSIP
 *
 * ssl       the WOLFSSL object
 * ctx       Callback context
 * return FSP_SUCCESS(0) on success,
 * otherwise PROTOCOLCB_UNAVAILABLE
 *          so that caller could continue to process if want
 */
WOLFSSL_LOCAL int Renesas_cmn_genMasterSecret(struct WOLFSSL* ssl, void* ctx)
{
    int ret = WOLFSSL_NOT_IMPLEMENTED;

    (void) ret;
    (void) ctx;

    WOLFSSL_ENTER("Renesas_cmn_genMasterSecret");

 #if defined(WOLFSSL_RENESAS_TSIP_TLS)

    if (Renesas_cmn_usable(ssl, 0)) {
    #if (WOLFSSL_RENESAS_TSIP_VER >= 109)
        ret = wc_tsip_generateMasterSecretEx(
                            ssl->options.cipherSuite0,
                            ssl->options.cipherSuite,
                            ssl->arrays->preMasterSecret,
                            ssl->arrays->clientRandom,
                            ssl->arrays->serverRandom,
                            ssl->arrays->tsip_masterSecret);
    #else
        ret = wc_tsip_generateMasterSecret(
                            ssl->arrays->preMasterSecret,
                            ssl->arrays->clientRandom,
                            ssl->arrays->serverRandom,
                            ssl->arrays->tsip_masterSecret);
    #endif

        if (ret == 0) {
            wc_tsip_storeKeyCtx(ssl, (TsipUserCtx*)ctx);
            /* set Session Key generation Callback for use */
            wolfSSL_CTX_SetGenSessionKeyCb(ssl->ctx,
                                                Renesas_cmn_generateSessionKey);
            wolfSSL_SetGenSessionKeyCtx(ssl, ctx);
        }
        else {
            wolfSSL_CTX_SetGenSessionKeyCb(ssl->ctx, NULL);
            wolfSSL_SetGenSessionKeyCtx(ssl, NULL);
        }
    }
    else
        ret = PROTOCOLCB_UNAVAILABLE;

 #elif defined(WOLFSSL_RENESAS_SCEPROTECT)
    if (Renesas_cmn_usable(ssl, 0)) {
        ret = wc_sce_generateMasterSecret(
                            ssl->options.cipherSuite0,
                            ssl->options.cipherSuite,
                            ssl->arrays->preMasterSecret,
                            ssl->arrays->clientRandom,
                            ssl->arrays->serverRandom,
                            ssl->arrays->sce_masterSecret);
        if (ret == 0) {
            wc_sce_storeKeyCtx(ssl, ctx);
            /* set Session Key generation Callback for use */
            wolfSSL_CTX_SetGenSessionKeyCb(ssl->ctx,
                                                Renesas_cmn_generateSessionKey);
            wolfSSL_SetGenSessionKeyCtx(ssl, ctx);
        }
        else {
            wolfSSL_CTX_SetGenSessionKeyCb(ssl->ctx, NULL);
            wolfSSL_SetGenSessionKeyCtx(ssl, NULL);
        }
    }
    else {
        WOLFSSL_MSG("PROTOCOLCB_UNAVAILABLE\n");
        ret = PROTOCOLCB_UNAVAILABLE;
    }

 #endif
    return ret;
}

/* Renesas Security Library Common Callback
 * Callback for Rsa Encryption
 *
 * ssl       the WOLFSSL object
 * in        Buffer to hold plain text
 * inSz      Length of plain text
 * out       Buffer to hold cipher text
 * outSz     Length of cipher text buffer
 * KeyDer    Buffer holding Key in der format
 * KeySz     Length of Key Der
 * ctx       Callback context
 * return FSP_SUCCESS(0) on success,
 * otherwise CRYPTOCB_UNAVAILABLE
 *          so that caller could continue to process if want
 */
WOLFSSL_LOCAL int Renesas_cmn_RsaEnc(WOLFSSL* ssl, const unsigned char* in,
       unsigned int inSz, unsigned char* out, word32* outSz,
       const unsigned char* keyDer, unsigned int keySz, void* ctx)
{
    int ret;
    int EncSz;

    (void)ctx;
    (void)in;
    (void)inSz;
    (void)keyDer;
    (void)keySz;
    (void)EncSz;

    WOLFSSL_ENTER("Renesas_cmn_RsaEnc");

    /* sanity check */
    if (ssl == NULL || in == NULL || out == NULL || keyDer == NULL ||
            ctx == NULL) {
        return BAD_FUNC_ARG;
    }

    EncSz = wc_RsaEncryptSize(ssl->peerRsaKey);

 #if defined(WOLFSSL_RENESAS_TSIP_TLS)
    if (tsip_usable(ssl, 0)) {
        if (EncSz == 256) {
            ret = wc_tsip_generateEncryptPreMasterSecret(ssl, out, outSz);
        }
        else {
            WOLFSSL_MSG("TSIP can only handle 256 bytes for RSA encrypt size.");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }
    else {
        ret = CRYPTOCB_UNAVAILABLE;
    }

 #elif defined(WOLFSSL_RENESAS_SCEPROTECT)
    if (wc_sce_usable(ssl, 0) && EncSz == 256) {
        ret = wc_sce_generateEncryptPreMasterSecret(ssl, out, outSz);
    }
    else {
        if (EncSz != 256)
            WOLFSSL_MSG("SCE cannot use"
                     "because Rsa Encrypt Size isn't 256 bytes(2048 bits).");

        ret = CRYPTOCB_UNAVAILABLE;
    }
 #endif
    return ret;
}

/* Renesas Security Library Common Callback
 * Callback for Verify hmac
 *
 * ssl       the WOLFSSL object
 * message   Buffer to hold message
 * inSz      Length of message
 * macSz     Length of mac size
 * content   content of inner data
 * ctx       Callback context
 * return FSP_SUCCESS(0) on success,
 * otherwise PROTOCOLCB_UNAVAILABLE
 *          so that caller could continue to process if want
 */
WOLFSSL_LOCAL int Renesas_cmn_VerifyHmac(WOLFSSL *ssl, const byte* message,
                    word32 messageSz, word32 macSz, word32 content, void* ctx)
{
    int ret;
    (void)ctx;

    WOLFSSL_ENTER("Renesas_cmn_VerifyHmac");

#if defined(WOLFSSL_RENESAS_TSIP_TLS)
    if (tsip_usable(ssl, 1)) {
        ret = wc_tsip_ShaXHmacVerify(ssl, message, messageSz, macSz, content);
    }
    else
        ret = PROTOCOLCB_UNAVAILABLE;
#elif defined(WOLFSSL_RENESAS_SCEPROTECT)
    if (wc_sce_usable(ssl, 1)) {
        ret = wc_sce_Sha256VerifyHmac(ssl, message, messageSz, macSz, content);
    }
    else
        ret = PROTOCOLCB_UNAVAILABLE;
#endif

    return ret;
}

#ifndef WOLFSSL_AEAD_ONLY
/* Renesas Security Library Common Callback
 * Callback for TLS hmac
 *
 * ssl       the WOLFSSL object
 * digest    Buffer to hold digest by hmac
 * in        Buffer to hold in data
 * sz        Length of in data
 * padSz     Length of padding
 * content   content of inner data
 * epochOrder
 * return FSP_SUCCESS(0) on success, otherwise error code
 */
WOLFSSL_LOCAL int Renesas_cmn_TLS_hmac(WOLFSSL* ssl, byte* digest,
  const byte* in, word32 sz, int padSz, int content, int verify, int epochOrder)
{
    int ret;
    byte   myInner[WOLFSSL_TLS_HMAC_INNER_SZ];

    WOLFSSL_ENTER("Renesas_cmn_TLS_hmac");

#if defined(WOLFSSL_RENESAS_TSIP_TLS)
    if (Renesas_cmn_usable(ssl, 1)) {

        wolfSSL_SetTlsHmacInner(ssl, myInner, sz, content, verify);

        if (ssl->specs.hash_size == WC_SHA_DIGEST_SIZE) {
            ret = wc_tsip_Sha1HmacGenerate(ssl, myInner,
                                    WOLFSSL_TLS_HMAC_INNER_SZ, in, sz, digest);
        }
        else if (ssl->specs.hash_size == WC_SHA256_DIGEST_SIZE) {
            ret = wc_tsip_Sha256HmacGenerate(ssl, myInner,
                                    WOLFSSL_TLS_HMAC_INNER_SZ, in, sz, digest);
        }
        else {
            ret = TSIP_MAC_DIGSZ_E;
        }
    }
    else {
        WOLFSSL_MSG("TLS_hmac is used instead of TSIP");
        /* fall through to original TLS hmac method when TSIP cannot be used */
        ret = TLS_hmac(ssl, digest, in, sz, padSz, content, verify, epochOrder);

    }

#elif defined(WOLFSSL_RENESAS_SCEPROTECT)
    if (Renesas_cmn_usable(ssl, 1)) {
        if (ssl->specs.hash_size == WC_SHA256_DIGEST_SIZE) {
            wolfSSL_SetTlsHmacInner(ssl, myInner, sz, content, verify);
            ret = wc_sce_Sha256GenerateHmac(ssl, myInner,
                                    WOLFSSL_TLS_HMAC_INNER_SZ, in, sz, digest);
        }
        else
            ret = TSIP_MAC_DIGSZ_E;
    }
    else {
        /* fall through to original TLS hmac method when SCE cannot be used */
        ret = TLS_hmac(ssl, digest, in, sz, padSz, content, verify, epochOrder);
    }

#endif

    return ret;
}
#endif /* !WOLFSSL_AEAD_ONLY */

/* Renesas Security Library Common Callback
 * Callback for Signature PK Rsa verify
 *
 * sig      Buffer holding signature
 * sigSz    Length of signature in bytes
 * out      Buffer to hold hash
 * keyDer   Buffer to hold rsa key
 * keySz    Length of key in bytes
 * ctx      Callback context
 * return FSP_SUCCESS(0) on success,
 * otherwise CRYPTOCB_UNAVAILABLE
 *          so that caller could continue to process if want
 */
WOLFSSL_LOCAL int Renesas_cmn_SigPkCbRsaVerify(unsigned char* sig,
        unsigned int sigSz, unsigned char** out, const unsigned char* keyDer,
        unsigned int keySz, void* ctx)
{
    int ret;
    CertAttribute*  CertAtt;

    (void)out;
    (void)keyDer;
    (void)keySz;

    WOLFSSL_ENTER("Renesas_cmn_SigPkCbRsaVerify");

    /* sanity check */
    if (sig == NULL || out == NULL || keyDer == NULL || ctx == NULL)
        return BAD_FUNC_ARG;

    CertAtt = (CertAttribute*)ctx;
    if (!CertAtt) {
        return CRYPTOCB_UNAVAILABLE;
    }

#if defined(WOLFSSL_RENESAS_TSIP_TLS)
    if (CertAtt->keyIndex != NULL) {
        ret = wc_tsip_tls_CertVerify(CertAtt->cert, CertAtt->certSz, sig, sigSz,
                                 CertAtt->pubkey_n_start - CertAtt->certBegin,
                                 CertAtt->pubkey_n_len - 1,
                                 CertAtt->pubkey_e_start - CertAtt->certBegin,
                                 CertAtt->pubkey_e_len -1,
                                 (uint8_t*)CertAtt->keyIndex);
        if (ret == 0) {
            CertAtt->verifyByTSIP_SCE = 1;
        }
        else {
            WOLFSSL_MSG("RSA Verify by TSIP didn't match");
            ret = ASN_SIG_CONFIRM_E;
        }
    }
    else
        ret = CRYPTOCB_UNAVAILABLE;

#elif defined(WOLFSSL_RENESAS_SCEPROTECT)
    if (CertAtt->keyIndex != NULL) {
        ret = wc_sce_tls_CertVerify(CertAtt->cert, CertAtt->certSz, sig, sigSz,
                                 CertAtt->pubkey_n_start - CertAtt->certBegin,
                                 CertAtt->pubkey_n_len - 1,
                                 CertAtt->pubkey_e_start - CertAtt->certBegin,
                                 CertAtt->pubkey_e_len -1,
                                 (uint8_t*)CertAtt->keyIndex);
        if (ret == 0) {
            CertAtt->verifyByTSIP_SCE = 1;
        }
        else {
            WOLFSSL_MSG("RSA Verify by SCE didn't match");
            ret = ASN_SIG_CONFIRM_E;
        }
    }
    else
        ret = CRYPTOCB_UNAVAILABLE;
#endif

    return ret;
}

/* Renesas Security Library Common Callback
 * Callback for Signature PK Ecc verify
 *
 * sig      Buffer holding signature
 * sigSz    Length of signature in bytes
 * has      Buffer to hold hash
 * hashSz   Length of hash
 * keyDer   Buffer to hold rsa key
 * keySz    Length of key in bytes
 * result   A pointer to int indicates a result
 * ctx      Callback context
 * return FSP_SUCCESS(0) on success,
 * otherwise CRYPTOCB_UNAVAILABLE
 *          so that caller could continue to process if want
 */
WOLFSSL_LOCAL int Renesas_cmn_SigPkCbEccVerify(const unsigned char* sig,
        unsigned int sigSz, const unsigned char* hash, unsigned int hashSz,
       const unsigned char* keyDer, unsigned int keySz,
       int* result, void* ctx)
{
    int ret;
    CertAttribute*  CertAtt;

    (void)result;
    (void)keyDer;
    (void)keySz;
    (void)hash;
    (void)hashSz;

    WOLFSSL_ENTER("Renesas_cmn_SigPkCbEccVerify");

    /* sanity check */
    if (sig == NULL || keyDer == NULL || hash == NULL || ctx == NULL ||
        result == NULL)
        return BAD_FUNC_ARG;


    CertAtt = (CertAttribute*)ctx;
    if (!CertAtt) {
        return CRYPTOCB_UNAVAILABLE;
    }

#if defined(WOLFSSL_RENESAS_TSIP_TLS)
    if (CertAtt->keyIndex != NULL) {
        ret = wc_tsip_tls_CertVerify(CertAtt->cert, CertAtt->certSz, sig, sigSz,
                                 CertAtt->pubkey_n_start - CertAtt->certBegin,
                                 CertAtt->pubkey_n_len - 1,
                                 CertAtt->pubkey_e_start - CertAtt->certBegin,
                                 CertAtt->pubkey_e_len -1,
                                 (uint8_t*)CertAtt->keyIndex);
        if (ret == 0) {
            CertAtt->verifyByTSIP_SCE = 1;
            *result = 1;
        }
        else {
            WOLFSSL_MSG("RSA Verify by TSIP didn't match");
            ret = ASN_SIG_CONFIRM_E;
        }
    }
    else
        ret = CRYPTOCB_UNAVAILABLE;
#elif defined(WOLFSSL_RENESAS_SCEPROTECT)
    if (CertAtt->keyIndex != NULL) {
        ret = wc_sce_tls_CertVerify(CertAtt->cert, CertAtt->certSz, sig, sigSz,
                                 CertAtt->pubkey_n_start - CertAtt->certBegin,
                                 CertAtt->pubkey_n_len - 1,
                                 CertAtt->pubkey_e_start - CertAtt->certBegin,
                                 CertAtt->pubkey_e_len -1,
                                 (uint8_t*)CertAtt->keyIndex);
        if (ret == 0) {
            CertAtt->verifyByTSIP_SCE = 1;
            *result = 1;
        }
        else {
            WOLFSSL_MSG("RSA Verify by SCE didn't match");
            ret = ASN_SIG_CONFIRM_E;
        }
    }
    else
        ret = CRYPTOCB_UNAVAILABLE;
#endif

    return ret;
}

#endif /* SCEPROTECT || TSIP */
