/* stsafe.c
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

#include <wolfssl/wolfcrypt/types.h>
#include <wolfssl/wolfcrypt/port/st/stsafe.h>
#include <wolfssl/wolfcrypt/logging.h>

#ifndef STSAFE_INTERFACE_PRINTF
#define STSAFE_INTERFACE_PRINTF(...) WC_DO_NOTHING
#endif

#ifdef WOLFSSL_STSAFEA100

int SSL_STSAFE_LoadDeviceCertificate(byte** pRawCertificate,
    word32* pRawCertificateLen)
{
    int err;

    if (pRawCertificate == NULL || pRawCertificateLen == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef USE_STSAFE_VERBOSE
    WOLFSSL_MSG("SSL_STSAFE_LoadDeviceCertificate");
#endif

    /* Try reading device certificate from ST-SAFE Zone 0 */
    err = stsafe_interface_read_device_certificate_raw(
        pRawCertificate, (uint32_t*)pRawCertificateLen);
    if (err == STSAFE_A_OK) {
    #if 0
        /* example for loading into WOLFSSL_CTX */
        err = wolfSSL_CTX_use_certificate_buffer(ctx,
            *pRawCertificate, *pRawCertificateLen, SSL_FILETYPE_ASN1);
        if (err != WOLFSSL_SUCCESS) {
            /* failed */
        }
        /* can free now */
        XFREE(*pRawCertificate, NULL, DYNAMIC_TEMP_BUFFER);
        *pRawCertificate = NULL;
    #endif
    }
    else {
        err = WC_HW_E;
    }

    return err;
}

#ifdef HAVE_PK_CALLBACKS

/**
 * \brief Key Gen Callback (used by TLS server)
 */
int SSL_STSAFE_CreateKeyCb(WOLFSSL* ssl, ecc_key* key, word32 keySz,
    int ecc_curve, void* ctx)
{
    int err;
    byte pubKeyRaw[STSAFE_MAX_PUBKEY_RAW_LEN];
    StSafeA_KeySlotNumber slot;
    StSafeA_CurveId curve_id;

    (void)ssl;
    (void)ctx;

#ifdef USE_STSAFE_VERBOSE
    WOLFSSL_MSG("CreateKeyCb: STSAFE");
#endif

    /* get curve */
    curve_id = stsafe_get_ecc_curve_id(ecc_curve);

    /* generate new ephemeral key on device */
    err = stsafe_interface_create_key(&slot, curve_id, (uint8_t*)&pubKeyRaw[0]);
    if (err != STSAFE_A_OK) {
    #ifdef USE_STSAFE_VERBOSE
        STSAFE_INTERFACE_PRINTF("stsafe_interface_create_key error: %d\n", err);
    #endif
        err = WC_HW_E;
        return err;
    }

    /* load generated public key into key, used by wolfSSL */
    err = wc_ecc_import_unsigned(key, &pubKeyRaw[0], &pubKeyRaw[keySz],
        NULL, ecc_curve);

    return err;
}

/**
 * \brief Verify Peer Cert Callback.
 */
int SSL_STSAFE_VerifyPeerCertCb(WOLFSSL* ssl,
                                const unsigned char* sig, unsigned int sigSz,
                                const unsigned char* hash, unsigned int hashSz,
                                const unsigned char* keyDer, unsigned int keySz,
                                int* result, void* ctx)
{
    int err;
    byte sigRS[STSAFE_MAX_SIG_LEN];
    byte *r, *s;
    word32 r_len = STSAFE_MAX_SIG_LEN/2, s_len = STSAFE_MAX_SIG_LEN/2;
    byte pubKeyX[STSAFE_MAX_PUBKEY_RAW_LEN/2];
    byte pubKeyY[STSAFE_MAX_PUBKEY_RAW_LEN/2];
    word32 pubKeyX_len = sizeof(pubKeyX);
    word32 pubKeyY_len = sizeof(pubKeyY);
    ecc_key key;
    word32 inOutIdx = 0;
    StSafeA_CurveId curve_id = STSAFE_A_NIST_P_256;
    int ecc_curve;

    (void)ssl;
    (void)ctx;

#ifdef USE_STSAFE_VERBOSE
    WOLFSSL_MSG("VerifyPeerCertCB: STSAFE");
#endif

    err = wc_ecc_init(&key);
    if (err != 0) {
        return err;
    }

    /* Decode the public key */
    err = wc_EccPublicKeyDecode(keyDer, &inOutIdx, &key, keySz);
    if (err == 0) {
        /* Extract Raw X and Y coordinates of the public key */
        err = wc_ecc_export_public_raw(&key, pubKeyX, &pubKeyX_len,
            pubKeyY, &pubKeyY_len);
    }
    if (err == 0) {
        int key_sz;

        /* determine curve */
        ecc_curve = key.dp->id;
        curve_id = stsafe_get_ecc_curve_id(ecc_curve);
        key_sz = stsafe_get_key_size(curve_id);

        /* Extract R and S from signature */
        XMEMSET(sigRS, 0, sizeof(sigRS));
        r = &sigRS[0];
        s = &sigRS[key_sz];
        err = wc_ecc_sig_to_rs(sig, sigSz, r, &r_len, s, &s_len);
        (void)r_len;
        (void)s_len;
    }

    if (err == 0) {
        /* Verify signature */
        err = stsafe_interface_verify(curve_id, (uint8_t*)hash, sigRS,
            pubKeyX, pubKeyY, (int32_t*)result);
        if (err != STSAFE_A_OK) {
        #ifdef USE_STSAFE_VERBOSE
            STSAFE_INTERFACE_PRINTF("stsafe_interface_verify error: %d\n", err);
        #endif
            err = -err;
        }
    }

    wc_ecc_free(&key);
    return err;
}

/**
 * \brief Sign Certificate Callback.
 */
int SSL_STSAFE_SignCertificateCb(WOLFSSL* ssl, const byte* in,
                                 word32 inSz, byte* out, word32* outSz,
                                 const byte* key, word32 keySz, void* ctx)
{
    int err;
    byte digest[STSAFE_MAX_KEY_LEN];
    byte sigRS[STSAFE_MAX_SIG_LEN];
    byte *r, *s;
    StSafeA_CurveId curve_id;
    int key_sz;

    (void)ssl;
    (void)ctx;

#ifdef USE_STSAFE_VERBOSE
    WOLFSSL_MSG("SignCertificateCb: STSAFE");
#endif

    curve_id = stsafe_get_curve_mode();
    key_sz = stsafe_get_key_size(curve_id);

    /* Build input digest */
    if (inSz > key_sz)
        inSz = key_sz;
    XMEMSET(&digest[0], 0, sizeof(digest));
    XMEMCPY(&digest[key_sz - inSz], in, inSz);

    /* Sign using slot 0: Result is R then S */
    /* Sign will always use the curve type in slot 0 (the TLS curve needs to match) */
    XMEMSET(sigRS, 0, sizeof(sigRS));
    err = stsafe_interface_sign(STSAFE_A_SLOT_0, curve_id, digest, sigRS);
    if (err != STSAFE_A_OK) {
    #ifdef USE_STSAFE_VERBOSE
        STSAFE_INTERFACE_PRINTF("stsafe_interface_sign error: %d\n", err);
    #endif
        err = WC_HW_E;
        return err;
    }

    /* Convert R and S to signature */
    r = &sigRS[0];
    s = &sigRS[key_sz];
    err = wc_ecc_rs_raw_to_sig((const byte*)r, key_sz, (const byte*)s, key_sz,
        out, outSz);
    if (err != 0) {
    #ifdef USE_STSAFE_VERBOSE
        WOLFSSL_MSG("Error converting RS to Signature");
    #endif
    }

    return err;
}


/**
 * \brief Create pre master secret using peer's public key and self private key.
 */
int SSL_STSAFE_SharedSecretCb(WOLFSSL* ssl, ecc_key* otherKey,
                              unsigned char* pubKeyDer, unsigned int* pubKeySz,
                              unsigned char* out, unsigned int* outlen,
                              int side, void* ctx)
{
    int err;
    byte otherKeyX[STSAFE_MAX_KEY_LEN];
    byte otherKeyY[STSAFE_MAX_KEY_LEN];
    word32 otherKeyX_len = sizeof(otherKeyX);
    word32 otherKeyY_len = sizeof(otherKeyY);
    byte pubKeyRaw[STSAFE_MAX_PUBKEY_RAW_LEN];
    StSafeA_KeySlotNumber slot = STSAFE_A_SLOT_0;
    StSafeA_CurveId curve_id;
    ecc_key tmpKey;
    int ecc_curve;
    int key_sz;

    (void)ssl;
    (void)ctx;

#ifdef USE_STSAFE_VERBOSE
    WOLFSSL_MSG("SharedSecretCb: STSAFE");
#endif

    err = wc_ecc_init(&tmpKey);
    if (err != 0) {
        return err;
    }

    /* set curve */
    ecc_curve = otherKey->dp->id;
    curve_id = stsafe_get_ecc_curve_id(ecc_curve);
    key_sz = stsafe_get_key_size(curve_id);

    /* for client: create and export public key */
    if (side == WOLFSSL_CLIENT_END) {
        /* Export otherKey raw X and Y */
        err = wc_ecc_export_public_raw(otherKey,
            &otherKeyX[0], (word32*)&otherKeyX_len,
            &otherKeyY[0], (word32*)&otherKeyY_len);
        if (err != 0) {
            return err;
        }

        err = stsafe_interface_create_key(&slot, curve_id, (uint8_t*)&pubKeyRaw[0]);
        if (err != STSAFE_A_OK) {
        #ifdef USE_STSAFE_VERBOSE
            STSAFE_INTERFACE_PRINTF("stsafe_interface_create_key error: %d\n", err);
        #endif
            err = WC_HW_E;
            return err;
        }

        /* convert raw unsigned public key to X.963 format for TLS */
        err = wc_ecc_init(&tmpKey);
        if (err == 0) {
            err = wc_ecc_import_unsigned(&tmpKey, &pubKeyRaw[0], &pubKeyRaw[key_sz],
                NULL, ecc_curve);
            if (err == 0) {
                err = wc_ecc_export_x963(&tmpKey, pubKeyDer, pubKeySz);
            }
            wc_ecc_free(&tmpKey);
        }
    }
    /* for server: import public key */
    else if (side == WOLFSSL_SERVER_END) {
        /* import peer's key and export as raw unsigned for hardware */
        err = wc_ecc_import_x963_ex(pubKeyDer, *pubKeySz, &tmpKey, ecc_curve);
        if (err == 0) {
            err = wc_ecc_export_public_raw(&tmpKey, otherKeyX, &otherKeyX_len,
                otherKeyY, &otherKeyY_len);
        }
    }
    else {
        err = BAD_FUNC_ARG;
    }

    wc_ecc_free(&tmpKey);

    if (err != 0) {
        return err;
    }

    /* Compute shared secret */
    err = stsafe_interface_shared_secret(
#ifdef WOLFSSL_STSAFE_TAKES_SLOT
        slot,
#endif
        curve_id, &otherKeyX[0], &otherKeyY[0],
        out, (int32_t*)outlen);
    if (err != STSAFE_A_OK) {
    #ifdef USE_STSAFE_VERBOSE
        STSAFE_INTERFACE_PRINTF("stsafe_interface_shared_secret error: %d\n", err);
    #endif
        err = WC_HW_E;
    }

    return err;
}

int SSL_STSAFE_SetupPkCallbacks(WOLFSSL_CTX* ctx)
{
    wolfSSL_CTX_SetEccKeyGenCb(ctx, SSL_STSAFE_CreateKeyCb);
    wolfSSL_CTX_SetEccSignCb(ctx, SSL_STSAFE_SignCertificateCb);
    wolfSSL_CTX_SetEccVerifyCb(ctx, SSL_STSAFE_VerifyPeerCertCb);
    wolfSSL_CTX_SetEccSharedSecretCb(ctx, SSL_STSAFE_SharedSecretCb);
    wolfSSL_CTX_SetDevId(ctx, 0); /* enables wolfCrypt `wc_ecc_*` ST-Safe use */
    return 0;
}

int SSL_STSAFE_SetupPkCallbackCtx(WOLFSSL* ssl, void* user_ctx)
{
    wolfSSL_SetEccKeyGenCtx(ssl, user_ctx);
    wolfSSL_SetEccSharedSecretCtx(ssl, user_ctx);
    wolfSSL_SetEccSignCtx(ssl, user_ctx);
    wolfSSL_SetEccVerifyCtx(ssl, user_ctx);
    return 0;
}


#endif /* HAVE_PK_CALLBACKS */

#ifdef WOLF_CRYPTO_CB

int wolfSSL_STSAFE_CryptoDevCb(int devId, wc_CryptoInfo* info, void* ctx)
{
    int rc = CRYPTOCB_UNAVAILABLE;
    wolfSTSAFE_CryptoCb_Ctx* stsCtx = (wolfSTSAFE_CryptoCb_Ctx*)ctx;

    if (info == NULL || ctx == NULL)
        return BAD_FUNC_ARG;

    (void)devId;
    (void)stsCtx;

    if (info->algo_type == WC_ALGO_TYPE_SEED) {
        /* use the STSAFE hardware for RNG seed */
    #if !defined(WC_NO_RNG) && defined(USE_STSAFE_RNG_SEED)
        while (info->seed.sz > 0) {
            rc = stsafe_interface_getrandom(info->seed.seed, info->seed.sz);
            if (rc < 0) {
                return rc;
            }
            info->seed.seed += rc;
            info->seed.sz -= rc;
        }
        rc = 0;
    #else
        rc = CRYPTOCB_UNAVAILABLE;
    #endif
    }
#ifdef HAVE_ECC
    else if (info->algo_type == WC_ALGO_TYPE_PK) {
    #ifdef USE_STSAFE_VERBOSE
        STSAFE_INTERFACE_PRINTF("STSAFE Pk: Type %d\n", info->pk.type);
    #endif

        if (info->pk.type == WC_PK_TYPE_EC_KEYGEN) {
            byte pubKeyRaw[STSAFE_MAX_PUBKEY_RAW_LEN];
            StSafeA_KeySlotNumber slot;
            StSafeA_CurveId curve_id;
            int ecc_curve, key_sz;

            WOLFSSL_MSG("STSAFE: ECC KeyGen");

            /* get curve */
            ecc_curve = info->pk.eckg.curveId;
            curve_id = stsafe_get_ecc_curve_id(ecc_curve);
            key_sz = stsafe_get_key_size(curve_id);

            /* generate new ephemeral key on device */
            rc = stsafe_interface_create_key(&slot, curve_id,
                (uint8_t*)pubKeyRaw);
            if (rc != STSAFE_A_OK) {
            #ifdef USE_STSAFE_VERBOSE
                STSAFE_INTERFACE_PRINTF("stsafe_interface_create_key error: %d\n", rc);
            #endif
                rc = WC_HW_E;
                return rc;
            }

            /* load generated public key into key, used by wolfSSL */
            rc = wc_ecc_import_unsigned(info->pk.eckg.key, pubKeyRaw,
                &pubKeyRaw[key_sz], NULL, ecc_curve);
        }
        else if (info->pk.type == WC_PK_TYPE_ECDSA_SIGN) {
            byte digest[STSAFE_MAX_KEY_LEN];
            byte sigRS[STSAFE_MAX_SIG_LEN];
            byte *r, *s;
            StSafeA_CurveId curve_id;
            word32 inSz = info->pk.eccsign.inlen;
            int key_sz;

            WOLFSSL_MSG("STSAFE: ECC Sign");

            curve_id = stsafe_get_curve_mode();
            key_sz = stsafe_get_key_size(curve_id);

            /* truncate input to match key size */
            if (inSz > key_sz)
                inSz = key_sz;

            /* Build input digest */
            XMEMSET(&digest[0], 0, sizeof(digest));
            XMEMCPY(&digest[key_sz - inSz], info->pk.eccsign.in, inSz);

            /* Sign using slot 0: Result is R then S */
            /* Sign will always use the curve type in slot 0
                (the TLS curve needs to match) */
            XMEMSET(sigRS, 0, sizeof(sigRS));
            rc = stsafe_interface_sign(STSAFE_A_SLOT_0, curve_id,
                (uint8_t*)info->pk.eccsign.in, sigRS);
            if (rc != STSAFE_A_OK) {
            #ifdef USE_STSAFE_VERBOSE
                STSAFE_INTERFACE_PRINTF("stsafe_interface_sign error: %d\n", rc);
            #endif
                rc = WC_HW_E;
                return rc;
            }

            /* Convert R and S to signature */
            r = &sigRS[0];
            s = &sigRS[key_sz];
            rc = wc_ecc_rs_raw_to_sig((const byte*)r, key_sz, (const byte*)s,
                key_sz, info->pk.eccsign.out, info->pk.eccsign.outlen);
            if (rc != 0) {
                WOLFSSL_MSG("Error converting RS to Signature");
            }
        }
        else if (info->pk.type == WC_PK_TYPE_ECDSA_VERIFY) {
            byte sigRS[STSAFE_MAX_SIG_LEN];
            byte *r, *s;
            word32 r_len = STSAFE_MAX_SIG_LEN/2, s_len = STSAFE_MAX_SIG_LEN/2;
            byte pubKeyX[STSAFE_MAX_PUBKEY_RAW_LEN/2];
            byte pubKeyY[STSAFE_MAX_PUBKEY_RAW_LEN/2];
            word32 pubKeyX_len = sizeof(pubKeyX);
            word32 pubKeyY_len = sizeof(pubKeyY);
            StSafeA_CurveId curve_id;
            int ecc_curve, key_sz;

            WOLFSSL_MSG("STSAFE: ECC Verify");

            if (info->pk.eccverify.key == NULL)
                return BAD_FUNC_ARG;

            /* determine curve */
            ecc_curve = info->pk.eccverify.key->dp->id;
            curve_id = stsafe_get_ecc_curve_id(ecc_curve);
            key_sz = stsafe_get_key_size(curve_id);

            /* Extract Raw X and Y coordinates of the public key */
            rc = wc_ecc_export_public_raw(info->pk.eccverify.key,
                pubKeyX, &pubKeyX_len,
                pubKeyY, &pubKeyY_len);
            if (rc == 0) {
                /* Extract R and S from signature */
                XMEMSET(sigRS, 0, sizeof(sigRS));
                r = &sigRS[0];
                s = &sigRS[key_sz];
                rc = wc_ecc_sig_to_rs(info->pk.eccverify.sig,
                    info->pk.eccverify.siglen, r, &r_len, s, &s_len);
                (void)r_len;
                (void)s_len;
            }
            if (rc == 0) {
                /* Verify signature */
                rc = stsafe_interface_verify(curve_id,
                    (uint8_t*)info->pk.eccverify.hash, sigRS, pubKeyX, pubKeyY,
                    (int32_t*)info->pk.eccverify.res);
                if (rc != STSAFE_A_OK) {
                #ifdef USE_STSAFE_VERBOSE
                    STSAFE_INTERFACE_PRINTF("stsafe_interface_verify error: %d\n", rc);
                #endif
                    rc = -rc;
                }
            }
        }
        else if (info->pk.type == WC_PK_TYPE_ECDH) {
            byte otherKeyX[STSAFE_MAX_KEY_LEN];
            byte otherKeyY[STSAFE_MAX_KEY_LEN];
            word32 otherKeyX_len = sizeof(otherKeyX);
            word32 otherKeyY_len = sizeof(otherKeyY);
            StSafeA_CurveId curve_id;
            int ecc_curve;

            WOLFSSL_MSG("STSAFE: PMS");

            if (info->pk.ecdh.public_key == NULL)
                return BAD_FUNC_ARG;

            /* get curve */
            ecc_curve = info->pk.ecdh.public_key->dp->id;
            curve_id = stsafe_get_ecc_curve_id(ecc_curve);

            /* Export otherKey raw X and Y */
            rc = wc_ecc_export_public_raw(info->pk.ecdh.public_key,
                &otherKeyX[0], (word32*)&otherKeyX_len,
                &otherKeyY[0], (word32*)&otherKeyY_len);
            if (rc == 0) {
                /* Compute shared secret */
            	*info->pk.ecdh.outlen = 0;
                rc = stsafe_interface_shared_secret(
        #ifdef WOLFSSL_STSAFE_TAKES_SLOT
                    STSAFE_A_SLOT_0,
        #endif
                    curve_id,
                    otherKeyX, otherKeyY,
                    info->pk.ecdh.out, (int32_t*)info->pk.ecdh.outlen);
                if (rc != STSAFE_A_OK) {
                #ifdef USE_STSAFE_VERBOSE
                    STSAFE_INTERFACE_PRINTF("stsafe_interface_shared_secret error: %d\n", rc);
                #endif
                    rc = WC_HW_E;
                }
            }
        }
    }
#endif /* HAVE_ECC */

    /* need to return negative here for error */
    if (rc != 0 && rc != CRYPTOCB_UNAVAILABLE) {
        WOLFSSL_MSG("STSAFE: CryptoCb failed");
    #ifdef USE_STSAFE_VERBOSE
        STSAFE_INTERFACE_PRINTF("STSAFE: CryptoCb failed %d\n", rc);
    #endif
        rc = WC_HW_E;
    }

    return rc;
}

#endif /* WOLF_CRYPTO_CB */

#endif /* WOLFSSL_STSAFEA100 */
