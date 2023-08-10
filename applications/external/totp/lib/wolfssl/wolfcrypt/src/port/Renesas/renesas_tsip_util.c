/* renesas_tsip_util.c
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

#if defined(WOLFSSL_RENESAS_TSIP)

#include <wolfssl/wolfcrypt/wc_port.h>
#include <wolfssl/wolfcrypt/types.h>
#include <wolfssl/wolfcrypt/asn.h>
#include <wolfssl/wolfcrypt/memory.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/aes.h>
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif
#include <wolfssl/ssl.h>
#include <wolfssl/internal.h>
#include <wolfssl/error-ssl.h>
#include <wolfssl/wolfcrypt/port/Renesas/renesas-tsip-crypt.h>
#include <wolfssl/wolfcrypt/port/Renesas/renesas_cmn.h>
#include <stdio.h>

#define TSIP_SIGNING_DATA_PREFIX_SZ     64
#define TSIP_SIGNING_DATA_PREFIX_BYTE   0x20
#define TSIP_MAX_SIG_DATA_SZ            130
#define TSIP_CERT_VFY_LABEL_SZ          34

/* function pointer typedefs for TSIP SHAxx HMAC Verification  */
typedef e_tsip_err_t (*shaHmacInitFn)
        (tsip_hmac_sha_handle_t*, tsip_hmac_sha_key_index_t*);
typedef e_tsip_err_t (*shaHmacUpdateFn)
        (tsip_hmac_sha_handle_t*, uint8_t*, uint32_t);
typedef e_tsip_err_t (*shaHmacFinalFn)
        (tsip_hmac_sha_handle_t*, uint8_t*, uint32_t);

/* ./ca-cert.der.sign,  */
/* expect to have these variables defined at user application */
#if defined(WOLFSSL_RENESAS_TSIP) && (WOLFSSL_RENESAS_TSIP_VER>=109)
extern uint32_t     s_inst2[R_TSIP_SINST2_WORD_SIZE];
#elif defined(WOLFSSL_RENESAS_TSIP) && (WOLFSSL_RENESAS_TSIP_VER>=106)
extern uint32_t     s_flash[];
extern uint32_t     s_inst1[R_TSIP_SINST_WORD_SIZE];
#endif

wolfSSL_Mutex       tsip_mutex;
static int          tsip_CryptHwMutexInit_ = 0;
static tsip_key_data g_user_key_info;
struct WOLFSSL_HEAP_HINT*  tsip_heap_hint = NULL;

/* tsip only keep one encrypted ca public key */
#if defined(WOLFSSL_RENESAS_TSIP_TLS)
static const byte*  ca_cert_sig = NULL;
static uint32_t     g_encrypted_publicCA_key[R_TSIP_SINST_WORD_SIZE];

/* index of CM table. must be global since renesas_common access it. */
extern uint32_t     g_CAscm_Idx;

#if defined(WOLFSSL_TLS13)
/* The server certificate verification label. */
static const byte serverCertVfyLabel[TSIP_CERT_VFY_LABEL_SZ] =
    "TLS 1.3, server CertificateVerify";
static const byte clientCertVfyLabel[TSIP_CERT_VFY_LABEL_SZ] =
    "TLS 1.3, client CertificateVerify";
#endif /* WOLFSSL_TLS13 */

#endif /* WOLFSSL_RENESAS_TSIP_TLS */



static int tsip_CryptHwMutexInit(wolfSSL_Mutex* mutex)
{
    return wc_InitMutex(mutex);
}

static int tsip_CryptHwMutexLock(wolfSSL_Mutex* mutex)
{
    return wc_LockMutex(mutex);
}

static int tsip_CryptHwMutexUnLock(wolfSSL_Mutex* mutex)
{
    return wc_UnLockMutex(mutex);
}

#if defined(WOLFSSL_RENESAS_TSIP_TLS)
/* Set client encrypted public key data.
 * parameters:
 * ssl      WOLFSSL object
 * keyBuf  buffer holding wrapped key which Renesas key tool generated.
 * keyBufLen buffer length
 * keyType  0: RSA 2048bit, 1: RSA 4096bit, 2 ECC P256
 * return   0 on success, others on failure.
 */
WOLFSSL_API int tsip_use_PublicKey_buffer_TLS(WOLFSSL* ssl,
                                const char* keyBuf, int keyBufLen, int keyType)
{
    int ret = 0;
    TsipUserCtx* tuc = NULL;

    WOLFSSL_ENTER("tsip_use_PublicKey_buffer_TLS");

    if (ssl == NULL
    || keyBuf == NULL || keyBufLen == 0) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0){
        tuc = ssl->RenesasUserCtx;
        tuc->wrappedPublicKey  = (uint8_t*)keyBuf;
        tuc->wrappedKeyType    = keyType;
    }

    WOLFSSL_LEAVE("tsip_use_PublicKey_buffer", ret);
    return ret;
}
/* Set client encrypted private key data.
 * parameters:
 * ssl      WOLFSSL object
 * keyBuf  buffer holding wrapped key which Renesas key tool generated.
 * keyBufLen buffer length
 * keyType  0: RSA 2048bit, 1: RSA 4096bit, 2 ECC P256
 * return   0 on success, others on failure.
 */
WOLFSSL_API int tsip_use_PrivateKey_buffer_TLS(struct WOLFSSL* ssl,
                                const char* keyBuf, int keyBufLen, int keyType)
{
    int ret = 0;
    TsipUserCtx* tuc = NULL;

    WOLFSSL_ENTER("tsip_use_PrivateKey_buffer_TLS");

    if (ssl == NULL || keyBuf == NULL || keyBufLen == 0 ) {
        ret = BAD_FUNC_ARG;
    }
    if (ret == 0){
        tuc = ssl->RenesasUserCtx;

        tuc->wrappedPrivateKey = (uint8_t*)keyBuf;
        tuc->wrappedKeyType    = keyType;

        /* store keyType as Id since Id capacity is 32 bytes */
        ret = wolfSSL_use_PrivateKey_Id(ssl,
                                (const unsigned char*)keyBuf, 32, tuc->devId);
        if (ret == WOLFSSL_SUCCESS) {
            ret = 0;
        }
    }

    WOLFSSL_LEAVE("tsip_use_PrivateKey_buffer_TLS", ret);
    return ret;
}
#endif /* WOLFSSL_RENESAS_TSIP_TLS */
#if defined(WOLFSSL_RENESAS_TSIP_CRYPTONLY)

/* Set client encrypted public key data.
 * parameters:
 * uc      Tsip user context
 * keyBuf  buffer holding wrapped key which Renesas key tool generated.
 * keyBufLen buffer length
 * keyType  0: RSA 2048bit, 1: RSA 4096bit, 2 ECC P256
 * return   0 on success, others on failure.
 */
WOLFSSL_API int tsip_use_PublicKey_buffer_crypt(TsipUserCtx *uc,
                                const char* keyBuf, int keyBufLen, int keyType)
{
    int ret = 0;
    TsipUserCtx* tuc = NULL;

    WOLFSSL_ENTER("tsip_use_PublicKey_buffer_crypt");

    if (uc == NULL
    || keyBuf == NULL || keyBufLen == 0) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0){
        tuc = uc;
        tuc->wrappedPublicKey  = (uint8_t*)keyBuf;
        tuc->wrappedKeyType    = keyType;
    }

    WOLFSSL_LEAVE("tsip_use_PublicKey_buffer_crypt", ret);
    return ret;
}
/* Set client encrypted private key data.
 * parameters:
 * uc      Tsip user context
 * keyBuf  buffer holding wrapped key which Renesas key tool generated.
 * keyBufLen buffer length
 * keyType  0: RSA 2048bit, 1: RSA 4096bit, 2 ECC P256
 * return   0 on success, others on failure.
 */
WOLFSSL_API int tsip_use_PrivateKey_buffer_crypt(TsipUserCtx *uc,
                                const char* keyBuf, int keyBufLen, int keyType)
{
    int ret = 0;
    TsipUserCtx* tuc = NULL;

    WOLFSSL_ENTER("tsip_use_PrivateKey_buffer_crypt");

    if (uc == NULL || keyBuf == NULL || keyBufLen == 0 ) {
        ret = BAD_FUNC_ARG;
    }
    if (ret == 0){
        tuc = uc;

        tuc->wrappedPrivateKey = (uint8_t*)keyBuf;
        tuc->wrappedKeyType    = keyType;
    }

    WOLFSSL_LEAVE("tsip_use_PrivateKey_buffer_crypt", ret);
    return ret;
}
#endif /* WOLFSSL_RENESAS_TSIP_CRYPTONLY */

#ifdef WOLFSSL_RENESAS_TSIP_TLS

/* Obsolete function. Use tsip_use_PrivateKey_buffer instead.
 * Set client encrypted private key data.
 * parameters:
 * key      Renesas Secure Flash Programmer generated key. 
 * keyType  0: RSA 2048bit, 1: RSA 4096bit, 2 ECC P256
 * return   0 on success, others on failure.
 */
WOLFSSL_API int  tsip_set_clientPrivateKeyEnc(const byte* encKey, int keyType)
{
    int ret = 0;

    WOLFSSL_ENTER("tsip_set_clientPrivateKeyEnc");

    if (ret == 0) {
        g_user_key_info.encrypted_user_private_key      = (uint8_t*)encKey;
        g_user_key_info.encrypted_user_private_key_type = keyType;
    }
    
    WOLFSSL_LEAVE("tsip_set_clientPrivateKeyEnc", ret);
    return ret;
}


/*  Flush raw handshake messages in MsgBag
 *
 */
static void tsipFlushMessages(struct WOLFSSL* ssl)
{
    TsipUserCtx* tuc = NULL;
    MsgBag* bag = NULL;

    if (ssl == NULL)
        return;

    /* get user context for TSIP */
    tuc = ssl->RenesasUserCtx;
    if (tuc == NULL) {
        return;
    }

    bag = &(tuc->messageBag);

    ForceZero(bag, sizeof(MsgBag));

}



WOLFSSL_LOCAL int tsip_TlsCleanup(struct WOLFSSL* ssl)
{
    int ret = 0;
    TsipUserCtx* tuc = NULL;

    if (ssl == NULL)
        return BAD_FUNC_ARG;

    tuc = ssl->RenesasUserCtx;

    if (tuc == NULL)
        return ret;

    /* free stored messages */
    tsipFlushMessages(ssl);
    
    return ret;
}



#if defined(WOLFSSL_TLS13)
/* generate ECC P265 key pair for ECDHE.
 * generated public key is stored in KeyShareEntry.pubkey and private key is
 * stored in TsipUserCtx.EcdhPrivKey13Idx as tsip specific format.
 * return 0 on success, CRYPTOCB_UNAVAILABLE when tsip can not handle and is
 * expecting to fallback to S/W, other negative values on error.
 */
WOLFSSL_LOCAL int tsip_Tls13GenEccKeyPair(WOLFSSL* ssl, KeyShareEntry* kse)
{
    int ret = 0;
    e_tsip_err_t    err = TSIP_SUCCESS;
    int             isTLS13 = 0;
    word16          curveId;
    ecc_key*        ecckey = NULL;
    TsipUserCtx*    tuc = NULL;

    WOLFSSL_ENTER("tsip_Tls13GenEccKeyPair");
    
    if (ssl == NULL || kse == NULL)
        ret = BAD_FUNC_ARG;

    if (ret == 0) {
        if (ssl->version.major == SSLv3_MAJOR && 
            ssl->version.minor == TLSv1_3_MINOR) {
            isTLS13 = 1;
        }
        /* TSIP works only in TLS13 client side */
        if (!isTLS13 || ssl->options.side != WOLFSSL_CLIENT_END) {
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    if (ret == 0) {
        /* TSIP can handle SECP256R1 */
        if (kse->group != WOLFSSL_ECC_SECP256R1) {
            WOLFSSL_MSG("TSIP can't handle the specified ECC curve.");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    if (ret == 0) {
        /* get user context for TSIP */
        tuc = ssl->RenesasUserCtx;
        if (tuc == NULL) {
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    curveId = ECC_SECP256R1;

    /* Allocate space for the public key */
    if (ret == 0) {
        kse->pubKey = (byte*)XMALLOC(kse->pubKeyLen, ssl->heap,
                                                DYNAMIC_TYPE_PUBLIC_KEY);
        if (kse->pubKey == NULL) {
            WOLFSSL_MSG("Key data Memory error");
            ret = MEMORY_E;
        }
        else {
            ForceZero(kse->pubKey, kse->pubKeyLen);
        }
    }

    /* Allocate an ECC key to hold private key. */
    if (ret == 0) {
        kse->key = (byte*)XMALLOC(sizeof(ecc_key), ssl->heap, DYNAMIC_TYPE_ECC);
        if (kse->key == NULL) {
            WOLFSSL_MSG("EccTempKey Memory error");
            ret = MEMORY_E;
        }
        else {
            ret = wc_ecc_init_ex((ecc_key*)kse->key, ssl->heap, ssl->devId);
        }
    }
    if (ret == 0) {
        ecckey = (ecc_key*)kse->key;
        ret = wc_ecc_set_curve(ecckey, kse->keyLen, curveId);
    }
    
    kse->pubKey[0] = ECC_POINT_UNCOMP;

    /* generate ecc key pair with TSIP */
    if (ret == 0) {
        if ((ret = tsip_hw_lock()) == 0) {

            tuc->Dhe_key_set  =0;

            err = R_TSIP_GenerateTls13P256EccKeyIndex(
                            &(tuc->handle13),
                            TSIP_TLS13_MODE_FULL_HANDSHAKE,
                            &(tuc->EcdhPrivKey13Idx),    /* private key index */
                            &(kse->pubKey[1]));       /* generated public key */

            if (err != TSIP_SUCCESS){ret = WC_HW_E;}

            if (ret == 0) {
                WOLFSSL_MSG("ECDH private key-index is stored by TSIP");
                tuc->Dhe_key_set  =1;
            }

            tsip_hw_unlock();
        }
        else {
            WOLFSSL_MSG("mutex locking error");
        }
    }

    if (ret != 0) {
        if (kse->key != NULL)
            XFREE(kse->key, ssl->heap, DYNAMIC_TYPE_PRIVATE_KEY);
        if (kse->pubKey != NULL)
            XFREE(kse->pubKey, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
    }
    WOLFSSL_LEAVE("tsip_Tls13GenEccKeyPair", ret);
    return ret;
}
#endif /* WOLFSSL_TLS13 */



#if defined(WOLFSSL_TLS13)
/* generate shared secret(pre-master secret)
 * get peer's raw ECDHE public key from KeyShareEntry.
 * The pre-master secret generated by TSIP is stored into 
 * TsipUserCtx.sharedSecret13Idx as TSIP specific format.
 * 
 * return 0 on success, CRYPTOCB_UNAVAILABLE when tsip can not handle and is
 * expecting to fallback to S/W, other negative values on error.
 */
WOLFSSL_LOCAL int tsip_Tls13GenSharedSecret(struct WOLFSSL* ssl,
                                                    struct KeyShareEntry* kse)
{
    int ret = 0;
    e_tsip_err_t    err   = TSIP_SUCCESS;
    int             isTLS13   = 0;
    uint8_t*        pubkeyraw = NULL;
    TsipUserCtx*    tuc = NULL;

    WOLFSSL_ENTER("tsip_Tls13GenSharedSecret");
    if (ssl == NULL || kse == NULL)
        ret = BAD_FUNC_ARG;

    if (ret == 0) {
        if (ssl->version.major == SSLv3_MAJOR && 
            ssl->version.minor == TLSv1_3_MINOR) {
            isTLS13 = 1;
        }
        if (!isTLS13 || ssl->options.side != WOLFSSL_CLIENT_END) {
            WOLFSSL_MSG("Not in TLS1.3 or in client");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    if (ret == 0) {
        /* make sure it is in TLS13 and in client side */
        if (kse->group != WOLFSSL_ECC_SECP256R1) {
            WOLFSSL_MSG("TSIP can't handle the specified group");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    if (ret == 0) {
        /* get user context for TSIP */
        tuc = ssl->RenesasUserCtx;    
        if (tuc == NULL) {
            WOLFSSL_MSG("TsipUserCtx hasn't been set to ssl.");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    if (ret == 0) {
        if (!tuc->Dhe_key_set) {
            WOLFSSL_MSG("TSIP wasn't involved in the key-exchange.");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    if (ret == 0) {
        if ((ret = tsip_hw_lock()) == 0) {

            tuc->SharedSecret_set = 0;
            pubkeyraw = kse->ke + 1;        /* peer's raw public key data */

            /* derive shared secret */
            err = R_TSIP_Tls13GenerateEcdheSharedSecret(
                        TSIP_TLS13_MODE_FULL_HANDSHAKE,
                        pubkeyraw,                 /* peer's ECDHE public key */
                        &(tuc->EcdhPrivKey13Idx),  /*(out) own ECDHE priv key */
                        &(tuc->sharedSecret13Idx));   /*(out) PreMasterSecret */

            if (err != TSIP_SUCCESS) {
                WOLFSSL_MSG("R_TSIP_Tls13GenerateEcdheSharedSecret error");
                ret = WC_HW_E;
            }
            if (ret == 0) {
                /* set flag for later tsip operations */
                tuc->SharedSecret_set = 1;
            }

            tsip_hw_unlock();
        }
        else {
            WOLFSSL_MSG("mutex locking error");
        }
    }

    WOLFSSL_LEAVE("tsip_Tls13GenSharedSecret", ret);
    return ret;
}
#endif /* WOLFSSL_TLS13 */



#if defined(WOLFSSL_TLS13)
WOLFSSL_LOCAL int tsip_Tls13DeriveEarlySecret(struct WOLFSSL* ssl)
{
    int ret = 0;
    TsipUserCtx*    tuc = NULL;

    WOLFSSL_ENTER("tsip_Tls13DeriveEarlySecret");
    if (ssl == NULL)
        ret = BAD_FUNC_ARG;

    if (ret == 0) {
        /* get user context for TSIP */
        tuc = ssl->RenesasUserCtx;    
        if (tuc == NULL) {
            WOLFSSL_MSG("TsipUserCtx hasn't been set to ssl.");
            ret = CRYPTOCB_UNAVAILABLE;
        }
        else {
            tuc->EarlySecret_set = 1;
        }
    }
    
    WOLFSSL_LEAVE("tsip_Tls13DeriveEarlySecret", ret);
    return ret;
}
#endif /* WOLFSSL_TLS13 */



#if defined(WOLFSSL_TLS13)
/* derive handshake secret.
 * get pre-master secret stored in TsipUserCtx.sharedSecret13Idx.
 * Derived handshake secret is stored into TsipUserCtx.handshakeSecret13Idx
 * as tsip specific format.
 * 
 * return 0 on success, CRYPTOCB_UNAVAILABLE when tsip can not handle and is
 * expecting to fallback to S/W, other negative values on error.
 */
WOLFSSL_LOCAL int tsip_Tls13DeriveHandshakeSecret(struct WOLFSSL* ssl)
{
    int ret = 0;
    e_tsip_err_t err = TSIP_SUCCESS;
    int isTLS13 = 0;
    TsipUserCtx*    tuc = NULL;

    WOLFSSL_ENTER("tsip_Tls13DeriveHandshakeSecret");
    if (ssl == NULL)
        ret = BAD_FUNC_ARG;

    if (ret == 0) {
        if (ssl->version.major == SSLv3_MAJOR && 
            ssl->version.minor == TLSv1_3_MINOR) {
            isTLS13 = 1;
        }

        if (!isTLS13 || (ssl->options.side != WOLFSSL_CLIENT_END)) {
            ret = CRYPTOCB_UNAVAILABLE;   /* expecting to fallback to S/W */
        }
    }

    if (ret == 0) {
        /* get user context for TSIP */
        tuc = ssl->RenesasUserCtx;    
        if (tuc == NULL) {
            WOLFSSL_MSG("TsipUserCtx hasn't been set to ssl.");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    if (ret == 0) {
        /* check if pre-master secret is generated by tsip */
        if (!tuc->SharedSecret_set) {
            WOLFSSL_MSG("TSIP wasn't involved in the key-exchange.");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    if (ret == 0) {
        if ((ret = tsip_hw_lock()) == 0) {
 
            tuc->HandshakeSecret_set = 0;

            err = R_TSIP_Tls13GenerateHandshakeSecret(
                        &(tuc->sharedSecret13Idx),
                        &(tuc->handshakeSecret13Idx));

            if (err != TSIP_SUCCESS) {
                WOLFSSL_MSG("R_TSIP_Tls13GenerateHandshakeSecret error");
                ret = WC_HW_E;
            }
            if (ret == 0) {
                tuc->HandshakeSecret_set = 1;
            }
            tsip_hw_unlock();
        }
        else {
            WOLFSSL_MSG("mutex locking error");
        }
    }

    WOLFSSL_LEAVE("tsip_Tls13DeriveHandshakeSecret", ret);
    return ret;
}
#endif /* WOLFSSL_TLS13 */



#if defined(WOLFSSL_TLS13)
static int tsipTls13DeriveClientHandshakeKeys(struct WOLFSSL* ssl)
{
    int ret = 0;
    e_tsip_err_t    err = TSIP_SUCCESS;
    int             isTLS13 = 0;
    TsipUserCtx*    tuc = NULL;
    byte            hash[WC_SHA256_DIGEST_SIZE];


    WOLFSSL_ENTER("tsipTls13DeriveClientHandshakeKeys");
    if (ssl == NULL)
        ret = BAD_FUNC_ARG;

    if (ret == 0) {
        if (ssl->version.major == SSLv3_MAJOR && 
            ssl->version.minor == TLSv1_3_MINOR) {
            isTLS13 = 1;
        }
        if (!isTLS13 || (ssl->options.side != WOLFSSL_CLIENT_END)) {
            ret = CRYPTOCB_UNAVAILABLE;   /* expecting to fallback to S/W */
        }
    }

    if (ret == 0) {
        /* get user context for TSIP */
        tuc = ssl->RenesasUserCtx;    
        if (tuc == NULL) {
            WOLFSSL_MSG("TsipUserCtx hasn't been set to ssl.");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    if (ret == 0) {
        /* make sure client handshake secret is generated by tsip */
        if (!tuc->HandshakeSecret_set) {
            WOLFSSL_MSG("TSIP wasn't involved in the key-exchange.");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    if (ret == 0) {
        /* get digest of handshake messages */
        ret = tsip_GetMessageSha256(ssl, hash, NULL);
    }

    if (ret == 0) {
        if ((ret = tsip_hw_lock()) == 0) {
 
            tuc->HandshakeClientTrafficKey_set = 0;
           
            err = R_TSIP_Tls13GenerateClientHandshakeTrafficKey(
                    &(tuc->handle13),
                    TSIP_TLS13_MODE_FULL_HANDSHAKE,
                    &(tuc->handshakeSecret13Idx),
                    hash,
                    &(tuc->clientWriteKey13Idx),
                    &(tuc->clientFinished13Idx));

            if (err != TSIP_SUCCESS) {
                WOLFSSL_MSG(
                    "R_TSIP_Tls13GenerateClientHandshakeTrafficKey error");
                ret = WC_HW_E;                    
            }

            /* key derivation succeeded */
            if (ret == 0) {
                tuc->HandshakeClientTrafficKey_set = 1;
            }

            tsip_hw_unlock();
        }
        else {
            WOLFSSL_MSG("mutex locking error");
        }
    }

    WOLFSSL_LEAVE("tsipTls13DeriveClientHandshakeKeys", ret);
    return ret;
}
#endif /* WOLFSSL_TLS13 */



#if defined(WOLFSSL_TLS13)
static int tsipTls13DeriveServerHandshakeKeys(struct WOLFSSL* ssl)
{
    int ret = 0;
    e_tsip_err_t    err = TSIP_SUCCESS;
    int             isTLS13 = 0;
    TsipUserCtx*    tuc = NULL;
    byte            hash[WC_SHA256_DIGEST_SIZE];


    WOLFSSL_ENTER("tsipTls13DeriveServerHandshakeKeys");
    if (ssl == NULL)
        ret = BAD_FUNC_ARG;

    if (ret == 0) {
        if (ssl->version.major == SSLv3_MAJOR && 
            ssl->version.minor == TLSv1_3_MINOR) {
            isTLS13 = 1;
        }
        if (!isTLS13 || (ssl->options.side != WOLFSSL_CLIENT_END)) {
            ret = CRYPTOCB_UNAVAILABLE;   /* expecting to fallback to S/W */
        }
    }

    if (ret == 0) {
        /* get user context for TSIP */
        tuc = ssl->RenesasUserCtx;    
        if (tuc == NULL) {
            WOLFSSL_MSG("TsipUserCtx hasn't been set to ssl.");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    if (ret == 0) {
        /* make sure client handshake secret is generated by tsip */
        if (!tuc->HandshakeSecret_set) {
            WOLFSSL_MSG("TSIP wasn't involved in the key-exchange.");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    if (ret == 0) {
        /* get digest of handshake packets */
        ret = tsip_GetMessageSha256(ssl, hash, NULL);
    }

    if (ret == 0) {
        if ((ret = tsip_hw_lock()) == 0) {
 
            tuc->HandshakeServerTrafficKey_set = 0;

            err = R_TSIP_Tls13GenerateServerHandshakeTrafficKey(
                        &(tuc->handle13),
                        TSIP_TLS13_MODE_FULL_HANDSHAKE,
                        &(tuc->handshakeSecret13Idx),
                        hash,
                        &(tuc->serverWriteKey13Idx),
                        &(tuc->serverFinished13Idx));

            if (err != TSIP_SUCCESS) {
                WOLFSSL_MSG(
                    "R_TSIP_Tls13GenerateServerHandshakeTrafficKey error");
                ret = WC_HW_E;
            }
            
            /* key derivation succeeded */
            if (ret == 0) {
                tuc->HandshakeServerTrafficKey_set = 1;
            }

            tsip_hw_unlock();
        }
        else {
            WOLFSSL_MSG("mutex locking error");
        }
    }

    WOLFSSL_LEAVE("tsipTls13DeriveServerHandshakeKeys", ret);
    return ret;
}
#endif /* WOLFSSL_TLS13 */



#if defined(WOLFSSL_TLS13)
static int tsipTls13DeriveTrafficKeys(struct WOLFSSL* ssl)
{
    int ret = 0;
    e_tsip_err_t    err = TSIP_SUCCESS;
    int             isTLS13 = 0;
    TsipUserCtx*    tuc = NULL;
    byte            hash[WC_SHA256_DIGEST_SIZE];


    WOLFSSL_ENTER("tsipTls13DeriveTrafficKeys");
    if (ssl == NULL)
        ret = BAD_FUNC_ARG;

    if (ret == 0) {
        if (ssl->version.major == SSLv3_MAJOR && 
            ssl->version.minor == TLSv1_3_MINOR) {
            isTLS13 = 1;
        }
        if (!isTLS13 || (ssl->options.side != WOLFSSL_CLIENT_END)) {
            ret = CRYPTOCB_UNAVAILABLE;   /* expecting to fallback to S/W */
        }
    }

    if (ret == 0) {
        /* get user context for TSIP */
        tuc = ssl->RenesasUserCtx;    
        if (tuc == NULL) {
            WOLFSSL_MSG("TsipUserCtx hasn't been set to ssl.");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    if (ret == 0) {
        /* make sure master secret is generated by tsip */
        if (!tuc->MasterSecret_set) {
            WOLFSSL_MSG("TSIP wasn't involved in the key-exchange.");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    if (ret == 0) {
        /* get digest of handshake messages */
        ret = tsip_GetMessageSha256(ssl, hash, NULL);
    }

    if (ret == 0) {
        if ((ret = tsip_hw_lock()) == 0) {
 
            tuc->ServerTrafficSecret_set   = 0;
            tuc->ClientTrafficSecret_set   = 0;
            tuc->ServerWriteTrafficKey_set = 0;
            tuc->ClientWriteTrafficKey_set = 0;

            err = R_TSIP_Tls13GenerateApplicationTrafficKey(
                        &(tuc->handle13),
                        TSIP_TLS13_MODE_FULL_HANDSHAKE,
                        &(tuc->masterSecret13Idx),
                        (uint8_t*)hash,
                        &(tuc->serverAppTraffic13Secret),
                        &(tuc->clientAppTraffic13Secret),
                        &(tuc->serverAppWriteKey13Idx),
                        &(tuc->clientAppWriteKey13Idx));

            if (err != TSIP_SUCCESS) {
                WOLFSSL_MSG(
                    "R_TSIP_Tls13GenerateApplicationTrafficKey error");
                ret = WC_HW_E;
            }
            
            /* key derivation succeeded */
            if (ret == 0) {
                tuc->ServerTrafficSecret_set   = 1;
                tuc->ClientTrafficSecret_set   = 1;
                tuc->ServerWriteTrafficKey_set = 1;
                tuc->ClientWriteTrafficKey_set = 1;
            }

            tsip_hw_unlock();
        }
        else {
            WOLFSSL_MSG("mutex locking error");
        }
    }

    WOLFSSL_LEAVE("tsipTls13DeriveTrafficKeys", ret);
    return ret;
}
#endif /* WOLFSSL_TLS13 */



#if defined(WOLFSSL_TLS13)
static int tsipTls13UpdateClientTrafficKeys(struct WOLFSSL* ssl)
{
    int ret     = 0;
    e_tsip_err_t    err = TSIP_SUCCESS;
    int             isTLS13 = 0;
    TsipUserCtx*    tuc = NULL;

     WOLFSSL_ENTER("tsipTls13UpdateClientTrafficKeys");

    if (ssl == NULL)
        ret = BAD_FUNC_ARG;

    if (ret == 0) {
        if (ssl->version.major == SSLv3_MAJOR && 
            ssl->version.minor == TLSv1_3_MINOR) {
            isTLS13 = 1;
        }
        if (!isTLS13 || (ssl->options.side != WOLFSSL_CLIENT_END)) {
            ret = CRYPTOCB_UNAVAILABLE;   /* expecting to fallback to S/W */
        }
    }

    if (ret == 0) {
        /* get user context for TSIP */
        tuc = ssl->RenesasUserCtx;    
        if (tuc == NULL) {
            WOLFSSL_MSG("TsipUserCtx hasn't been set to ssl.");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    if (ret == 0) {
        /* make sure application secret is generated by tsip */
        if (!tuc->ClientTrafficSecret_set) {
            WOLFSSL_MSG("TSIP wasn't involved in the key-exchange.");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }
    if (ret == 0) {
        if ((ret = tsip_hw_lock()) == 0) {

            tuc->ClientWriteTrafficKey_set = 0;

            err = R_TSIP_Tls13UpdateApplicationTrafficKey(
                        &(tuc->handle13),
                        TSIP_TLS13_MODE_FULL_HANDSHAKE,
                        TSIP_TLS13_UPDATE_CLIENT_KEY,
                        &(tuc->clientAppTraffic13Secret),
                        &(tuc->clientAppTraffic13Secret),
                        &(tuc->clientAppWriteKey13Idx));
            if (err != TSIP_SUCCESS) {
                WOLFSSL_MSG("R_TSIP_Tls13UpdateApplicationTrafficKey error");
                ret = WC_HW_E;
            }
            else {
                tuc->ClientWriteTrafficKey_set = 1;
            }
            tsip_hw_unlock();
        }
        else {
            WOLFSSL_MSG("mutex locking error");
        }
    }

    WOLFSSL_LEAVE("tsipTls13UpdateClientTrafficKeys", ret);
    return ret;
}
#endif /* WOLFSSL_TLS13 */



#if defined(WOLFSSL_TLS13)
static int tsipTls13UpdateServerTrafficKeys(struct WOLFSSL* ssl)
{
    int ret     = 0;
    e_tsip_err_t    err = TSIP_SUCCESS;
    int             isTLS13 = 0;
    TsipUserCtx*    tuc = NULL;

     WOLFSSL_ENTER("tsipTls13UpdateServerTrafficKeys");

    if (ssl == NULL)
        ret = BAD_FUNC_ARG;

    if (ret == 0) {
        if (ssl->version.major == SSLv3_MAJOR && 
            ssl->version.minor == TLSv1_3_MINOR) {
            isTLS13 = 1;
        }
        if (!isTLS13 || (ssl->options.side != WOLFSSL_CLIENT_END)) {
            ret = CRYPTOCB_UNAVAILABLE;   /* expecting to fallback to S/W */
        }
    }

    if (ret == 0) {
        /* get user context for TSIP */
        tuc = ssl->RenesasUserCtx;    
        if (tuc == NULL) {
            WOLFSSL_MSG("TsipUserCtx hasn't been set to ssl.");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    if (ret == 0) {
        /* make sure application secret is generated by tsip */
        if (!tuc->ServerTrafficSecret_set) {
            WOLFSSL_MSG("TSIP wasn't involved in the key-exchange.");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }
    if (ret == 0) {
        if ((ret = tsip_hw_lock()) == 0) {

            tuc->ServerWriteTrafficKey_set = 0;

            err = R_TSIP_Tls13UpdateApplicationTrafficKey(
                        &(tuc->handle13),
                        TSIP_TLS13_MODE_FULL_HANDSHAKE,
                        TSIP_TLS13_UPDATE_SERVER_KEY,
                        &(tuc->serverAppTraffic13Secret),
                        &(tuc->serverAppTraffic13Secret),
                        &(tuc->serverAppWriteKey13Idx));
            if (err != TSIP_SUCCESS) {
                WOLFSSL_MSG("R_TSIP_Tls13UpdateApplicationTrafficKey error");
                ret = WC_HW_E;
            }
            else {
                tuc->ServerWriteTrafficKey_set = 1;
            }
            tsip_hw_unlock();
        }
        else {
            WOLFSSL_MSG("mutex locking error");
        }
    }

    WOLFSSL_LEAVE("tsipTls13UpdateServerTrafficKeys", ret);
    return ret;
}
#endif /* WOLFSSL_TLS13 */



#if defined(WOLFSSL_TLS13)
/* Derive the keys for TLS v1.3.
 *
 * ssl    The WOLFSSL object.
 * keyType  kind of keys to derive.
 *        handshake_key: when deriving keys for encrypting handshake messages.
 *        traffic_key: when deriving first keys for encrypting traffic messages.
 *        update_traffic_key: when deriving next keys for encrypting
 *        traffic messages.
 *
 * side   ENCRYPT_SIDE_ONLY: when only encryption secret needs to be derived.
 *        DECRYPT_SIDE_ONLY: when only decryption secret needs to be derived.
 *        ENCRYPT_AND_DECRYPT_SIDE: when both secret needs to be derived.
 * 
 * returns 0 on success, CRYPTOCB_UNAVAILABLE when tsip can not handle and is
 * expecting to fallback to S/W, other negative values on error.
 */
WOLFSSL_LOCAL int tsip_Tls13DeriveKeys(struct WOLFSSL* ssl,
                                                int keyType, int side)
{
    int ret = 0;
    int provision;

    WOLFSSL_ENTER("tsip_Tls13DeriveKeys");

    if (side == ENCRYPT_AND_DECRYPT_SIDE) {
        provision = PROVISION_CLIENT_SERVER;
    }
    else {
        provision = ((ssl->options.side != WOLFSSL_CLIENT_END) ^
                     (side == ENCRYPT_SIDE_ONLY)) ? PROVISION_CLIENT :
                                                    PROVISION_SERVER;
    }
    /* derive client key */
    switch (keyType) {
        case early_data_key:
            WOLFSSL_MSG("TSIP can't handle early data key");
            ret = CRYPTOCB_UNAVAILABLE;
            break;

        case handshake_key:
            if (provision & PROVISION_CLIENT) {
                ret = tsipTls13DeriveClientHandshakeKeys(ssl);
            }
            break;

        case traffic_key:
            ret = tsipTls13DeriveTrafficKeys(ssl);
            break;

        case update_traffic_key:
            if (provision & PROVISION_CLIENT) {
                ret = tsipTls13UpdateClientTrafficKeys(ssl);
            }
            break;

        default:
            ret = CRYPTOCB_UNAVAILABLE;
            break;
    }

    if (ret == 0) {
        /* derive server key */
        switch (keyType) {
            case early_data_key:
                WOLFSSL_MSG("TSIP can't handle early data key");
                ret = CRYPTOCB_UNAVAILABLE;
                break;

            case handshake_key:
                if (provision & PROVISION_SERVER) {
                    ret = tsipTls13DeriveServerHandshakeKeys(ssl);
                }
                break;

            case traffic_key:
                /* traffic key for server was derived in
                 * tsipTls13DeriveTrafficKeys
                 */
                break;

            case update_traffic_key:
                if (provision & PROVISION_SERVER) {
                    ret = tsipTls13UpdateServerTrafficKeys(ssl);
                }
                break;

            default:
                ret = CRYPTOCB_UNAVAILABLE;
                break;
        }
    }
    WOLFSSL_LEAVE("tsip_Tls13DeriveKeys", ret);
    return ret;
}
#endif /* WOLFSSL_TLS13 */



#if defined(WOLFSSL_TLS13)
WOLFSSL_LOCAL int tsip_Tls13DeriveMasterSecret(struct WOLFSSL* ssl)
{
    int ret = 0;
    e_tsip_err_t    err = TSIP_SUCCESS;
    int             isTLS13 = 0;
    TsipUserCtx*    tuc = NULL;

    WOLFSSL_ENTER("tsip_Tls13DeriveMasterSecret");

    if (ssl == NULL)
        ret = BAD_FUNC_ARG;

    if (ret == 0) {
        if (ssl->version.major == SSLv3_MAJOR && 
            ssl->version.minor == TLSv1_3_MINOR) {
            isTLS13 = 1;
        }
        if (!isTLS13 || (ssl->options.side != WOLFSSL_CLIENT_END)) {
            ret = CRYPTOCB_UNAVAILABLE;   /* expecting to fallback to S/W */
        }
    }
    if (ret == 0) {
        /* get user context for TSIP */
        tuc = ssl->RenesasUserCtx;    
        if (tuc == NULL) {
            WOLFSSL_MSG("TsipUserCtx hasn't been set to ssl.");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }
    if (ret == 0) {
        /* make sure handshake secret and verify data has been set by TSIP */
        if (!tuc->HandshakeSecret_set || 
            !tuc->HandshakeVerifiedData_set) {
            WOLFSSL_MSG("TSIP wasn't involved in the key-exchange.");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }
    if (ret == 0) {
        if ((ret = tsip_hw_lock()) == 0) {
 
            tuc->MasterSecret_set = 0;

            err = R_TSIP_Tls13GenerateMasterSecret(
                        &(tuc->handle13),
                        TSIP_TLS13_MODE_FULL_HANDSHAKE,
                        &(tuc->handshakeSecret13Idx),
                        (uint32_t*)tuc->verifyData13Idx,
                        &(tuc->masterSecret13Idx));

            if (err != TSIP_SUCCESS) {
                WOLFSSL_MSG(
                    "R_TSIP_Tls13GenerateMasterSecret( error");
                ret = WC_HW_E;
            }
            
            if (ret == 0) {
                tuc->MasterSecret_set = 1;
            }

            tsip_hw_unlock();
        }
        else {
            WOLFSSL_MSG("mutex locking error");
        }
    }

    WOLFSSL_LEAVE("tsip_Tls13DeriveMasterSecret", ret);
    return ret;
}
#endif /* WOLFSSL_TLS13 */



#if defined(WOLFSSL_TLS13)
/* verify handshake
 * ssl     WOLFSSL object
 * hash    buffer holding decrypted finished message content from server.
 * 
 */
static int tsipTls13VerifyHandshake(struct WOLFSSL* ssl,
                                    const byte* hash)/*finished message*/
{
    int             ret = 0;
    e_tsip_err_t    err = TSIP_SUCCESS;
    int             isTLS13 = 0;
    TsipUserCtx*    tuc = NULL;
    word32          msgHash[WC_SHA256_DIGEST_SIZE/sizeof(word32)];

    WOLFSSL_ENTER("tsipTls13VerifyHandshake");

    if (ssl == NULL)
        ret = BAD_FUNC_ARG;

    if (ret == 0) {
        if (ssl->version.major == SSLv3_MAJOR && 
            ssl->version.minor == TLSv1_3_MINOR) {
            isTLS13 = 1;
        }
        if (!isTLS13 || (ssl->options.side != WOLFSSL_CLIENT_END)) {
            ret = CRYPTOCB_UNAVAILABLE;   /* expecting to fallback to S/W */
        }
    }

    if (ret == 0) {
        /* get user context for TSIP */
        tuc = ssl->RenesasUserCtx;    
        if (tuc == NULL) {
            WOLFSSL_MSG("TsipUserCtx hasn't been set to ssl.");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    if (ret == 0) {
        /* make sure handshake secret is generated by tsip */
        if (!tuc->HandshakeServerTrafficKey_set) {
            WOLFSSL_MSG("TSIP wasn't involved in the key-exchange.");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }
    /* get digest of handshake messages */
    if (ret == 0) {
        ret = tsip_GetMessageSha256(ssl, (byte*)msgHash, NULL);
    }

    if (ret == 0) {
        if ((ret = tsip_hw_lock()) == 0) {
            
            tuc->HandshakeVerifiedData_set = 0;

            err = R_TSIP_Tls13ServerHandshakeVerification(
                                        TSIP_TLS13_MODE_FULL_HANDSHAKE,
                                        &(tuc->serverFinished13Idx),
                                        (uint8_t*)msgHash,
                                        (uint8_t*)hash,
                                        (uint32_t*)(tuc->verifyData13Idx));

            if (err == TSIP_ERR_VERIFICATION_FAIL) {
                WOLFSSL_MSG("Handshake verification error");
                ret = VERIFY_FINISHED_ERROR;
            }
            else if (err != TSIP_SUCCESS) {
                WOLFSSL_MSG("R_TSIP_Tls13ServerHandshakeVerification error");
                ret = WC_HW_E;                    
            }
            if (ret == 0) {
                WOLFSSL_MSG("Verified handshake");
                tuc->HandshakeVerifiedData_set = 1;
            }

            tsip_hw_unlock();
        }
        else {
            WOLFSSL_MSG("mutex locking error");
        }
    }

    WOLFSSL_LEAVE("tsipTls13VerifyHandshake", ret);
    return ret;
}
#endif /* WOLFSSL_TLS13 */



#if defined(WOLFSSL_TLS13)
/* handles finished message from server.
 * verify hmac in the message. Also output verify data to 
 * TsipUserCtx.verifyDataIdx, which is used for deriving master secret.
 * 
 *  ssl       WOLFSSL object
 *  input     the buffer holding decrypted finished message, type and padding
 *  inOutIdx  On entry, the index into the message content of Finished.
 *            On exit, the index of byte after the Finished message and padding.
 *  size      Length of message content(excluding type and padding)
 *  totalSz   Length in the record header. means message + type + pad.
 *  return    0, on success, others on failure.
 */
WOLFSSL_LOCAL int tsip_Tls13HandleFinished(
                                            struct WOLFSSL* ssl,
                                            const byte*     input,
                                            word32*         inOutIdx,
                                            word32          size,
                                            word32          totalSz)
{
    int ret = 0;

    WOLFSSL_ENTER("tsip_Tls13HandleFinished");

    if (ssl == NULL || input == NULL || inOutIdx == NULL) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        ret = tsipTls13VerifyHandshake(ssl, input + *inOutIdx);
    }

    if (ret == 0) {
        /* Force input exhaustion at ProcessReply by consuming padSz. */
        *inOutIdx += size + ssl->keys.padSz;

        ssl->options.serverState = SERVER_FINISHED_COMPLETE;
    }

    WOLFSSL_LEAVE("tsip_Tls13HandleFinished", ret);
    return ret;
}
#endif /* WOLFSSL_TLS13 */



#if defined(WOLFSSL_TLS13)
/* Build TLS v1.3 Message and make it encrypted with AEAD algorithm.
 * TSIP supports AES-GCM and AES-CCM.
 * ssl         The WOLFSSL object.
 * output      The buffer to write record message to.
 * outSz       Size of the buffer being written into.
 * input       The handshake message data to encrypt (excluding trailing type).
 * inSz        The size of the handshake message (including message header).
 * type        The real content type being put after the message data.
 * hashOutput  Whether to hash the unencrypted record data.
 * returns     the size of the record including header, CRYPTOCB_UNAVAILABLE 
 *             when tsip can not handle and is expecting to fallback to S/W,
 *             other negative values on error.
 */
WOLFSSL_LOCAL int tsip_Tls13BuildMessage(struct WOLFSSL* ssl,
                                         byte* output,
                                         int   outSz,
                                         const byte* input,
                                         int   inSz,
                                         int   type,
                                         int hashOutput)
{
    int ret = 0;
    int recSz;
    int isTLS13 = 0;
    RecordLayerHeader* rl = NULL;
    (void)outSz;
    
    WOLFSSL_ENTER("tsip_Tls13BuildMessage");

    if (ssl == NULL || output == NULL || input == NULL) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        if (ssl->version.major == SSLv3_MAJOR && 
            ssl->version.minor == TLSv1_3_MINOR) {
            isTLS13 = 1;
        }
        if (!isTLS13 || (ssl->options.side != WOLFSSL_CLIENT_END)) {
            ret = CRYPTOCB_UNAVAILABLE;   /* expecting to fallback to S/W */
        }
    }

    if (ret == 0) {
        /* make sure hash algorithm is SHA256 */
        if (ssl->specs.mac_algorithm != sha256_mac ) {
            WOLFSSL_MSG("TSIP can't handle this hash algorithm.");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    if (ret == 0) {
        if ((ssl->specs.bulk_cipher_algorithm != wolfssl_aes_gcm) &&
            (ssl->specs.bulk_cipher_algorithm != wolfssl_aes_ccm)) {
            WOLFSSL_MSG("TSIP can't handle the specified algorithm");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    if (ret == 0) {
        /* set size in record header */
        recSz = inSz + 1 + ssl->specs.aead_mac_size;

        /* update the record header with the new size. */
        rl = (RecordLayerHeader*)output;
        rl->type    = application_data;
        rl->pvMajor = ssl->version.major;
        rl->pvMinor = TLSv1_2_MINOR;
        c16toa((word16)recSz, rl->length);

        if (input != output + RECORD_HEADER_SZ) {
            XMEMCPY(output + RECORD_HEADER_SZ, input, inSz);
        }

        if (hashOutput) {
            ret = HashOutput(ssl, output, RECORD_HEADER_SZ + inSz, 0);
        }
    }
    if (ret == 0) {
        /* The real record content type goes at the end of the data. */
        output[RECORD_HEADER_SZ + inSz] = (byte)type;

        ret = tsip_Tls13AesEncrypt(ssl, 
                           output + RECORD_HEADER_SZ, /* output */
                           output + RECORD_HEADER_SZ, /* plain message */
                           inSz + 1); /* plain data size(= inSz + 1 for type) */

        if (ret > 0) {
            ret = recSz + RECORD_HEADER_SZ; /* return record size */
        }
    }

    WOLFSSL_LEAVE("tsip_Tls13BuildMessage", ret);
    return ret;
}
#endif /* WOLFSSL_TLS13 */



#if defined(WOLFSSL_TLS13)
/* Send finished message to the server.
 * 
 * ssl     WOLFSSL object
 * output  buffer to output packet, including packet header and finished message
 * outSz   buffer size of output
 * input   buffer holding finished message
 * hashOut
 * return  0 on success, CRYPTOCB_UNAVAILABLE when TSIP can not handle,
 *         other negative values on error.
 */
WOLFSSL_LOCAL int tsip_Tls13SendFinished(
                                struct WOLFSSL* ssl,
                                byte*       output,
                                int         outSz,
                                const byte* input,
                                int         hashOut)
{
    int ret         = 0;
    int finishedSz;
    int headerSz    = HANDSHAKE_HEADER_SZ;
    int recordSz;

    WOLFSSL_ENTER("tsip_Tls13SendFinished");

    if (ssl == NULL || output == NULL || input == NULL || outSz == 0) {
        ret  = BAD_FUNC_ARG;
    }
    
    if (ret == 0) {
        finishedSz  = ssl->specs.hash_size;

        ret = tsip_Tls13GetHmacMessages(ssl, (byte*)&input[headerSz]);
    }

    if (ret == 0) {
       recordSz = WC_MAX_DIGEST_SIZE + DTLS_HANDSHAKE_HEADER_SZ + MAX_MSG_EXTRA;
        /* check for available size */
        ret = CheckAvailableSize(ssl, recordSz);
        recordSz = 0;
    }

    if (ret == 0) {
        recordSz = tsip_Tls13BuildMessage(ssl,
                                     output, outSz,
                                     input, headerSz + finishedSz,
                                     handshake, hashOut);
    
        if (recordSz > 0) {
            ssl->options.clientState    = CLIENT_FINISHED_COMPLETE;
            ssl->options.handShakeState = HANDSHAKE_DONE;
            ssl->options.handShakeDone  = 1;
            ssl->buffers.outputBuffer.length += recordSz; /* advance length */

            ret = SendBuffered(ssl);
        }
        else {
            ret = recordSz;
        }
    }
    WOLFSSL_LEAVE("tsip_Tls13SendFinished", ret);
    return ret;
}
#endif /* WOLFSSL_TLS13 */

#if defined(WOLFSSL_TLS13)
/* Parse and handle a TLS v1.3 CertificateVerify message sent from a server.
 *
 * ssl       WOLFSSL object
 * input     buffer holding certificate verify message
 * inOutIdx  On entry, the index into the message buffer of
 *           CertificateVerify.
 *           On exit, the index of byte after the CertificateVerify message.
 * totalSz   The length of the current handshake message.
 * return    0 on success, CRYPTOCB_UNAVAILABLE when TSIP can not handle,
 *           other negative values on error.
 */
WOLFSSL_LOCAL int tsip_Tls13CertificateVerify(struct WOLFSSL* ssl, 
                                            const byte* input, word32* inOutIdx,
                                            word32 totalSz)
{
    int     ret = 0;
    byte*   sigData = NULL;
    byte    hiAlgo,loAlgo;
    int     messageSz;
    word16  signatureLen;
    word16  idx;
    e_tsip_err_t  err = TSIP_SUCCESS;
    TsipUserCtx*  tuc = NULL;
    e_tsip_tls13_signature_scheme_type_t sig_scheme;
    
    WOLFSSL_ENTER("tsip_Tls13CertificateVerify");


    if (ssl == NULL || input == NULL || inOutIdx == NULL) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        if (ENUM_LEN + ENUM_LEN > totalSz) {
            ret = BUFFER_ERROR;
        }
        /* parse certificate verify message to get hash-algo */
        hiAlgo  = *(input + *inOutIdx);
        loAlgo  = *(input + *inOutIdx + 1);
    }
    if (ret == 0) {
        /* get signature length */
        ato16(input + *inOutIdx + 2, &signatureLen);

        if (ENUM_LEN + ENUM_LEN + OPAQUE16_LEN > totalSz) {
            ret = BUFFER_ERROR;
        }
    }
    if (ret == 0) {
        if (ENUM_LEN + ENUM_LEN + OPAQUE16_LEN + signatureLen > totalSz) {
            ret = BUFFER_ERROR;
        }
    }
    if (ret == 0) {
        /* check if tsip accepts signature algorithm */
        if (hiAlgo == NEW_SA_MAJOR && loAlgo == sha256_mac) {
            /* rsa_pss_rsae_sha256 0x0804 */
            WOLFSSL_MSG("Peer sent RSA sig");
            sig_scheme = TSIP_TLS13_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA256;
        }
        else if (hiAlgo == 0x04 && loAlgo == ecc_dsa_sa_algo) {
            /* ecdsa_secp256r1_sha256 0x0403 */
            WOLFSSL_MSG("Peer sent ECC sig");
            sig_scheme = TSIP_TLS13_SIGNATURE_SCHEME_ECDSA_SECP256R1_SHA256;
        }
        else {
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    if (ret == 0) {
        /* get user context for TSIP */
        tuc = ssl->RenesasUserCtx;    
        if (tuc == NULL) {
            WOLFSSL_MSG("TsipUserCtx is not set to ssl.");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    /* check if peer's public key is stored */
    if (ret == 0) {
        if (ssl->peerSceTsipEncRsaKeyIndex == NULL) {
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    if (ret == 0) {
        /* create sign data */
        sigData = tuc->sigDataCertVerify;

        idx = 0;
        ForceZero(sigData, sizeof(tuc->sigDataCertVerify));
        XMEMSET(sigData, TSIP_SIGNING_DATA_PREFIX_BYTE, 
                                                TSIP_SIGNING_DATA_PREFIX_SZ);

        idx += TSIP_SIGNING_DATA_PREFIX_SZ;
        XMEMCPY(&sigData[idx], serverCertVfyLabel, TSIP_CERT_VFY_LABEL_SZ);

        idx += TSIP_CERT_VFY_LABEL_SZ;
        ret = tsip_GetMessageSha256(ssl, &sigData[idx], &messageSz);
    }
 
    if (ret == 0) {

        if ((ret = tsip_hw_lock()) == 0) {
            err = R_TSIP_Tls13CertificateVerifyVerification(
                        (uint32_t*)ssl->peerSceTsipEncRsaKeyIndex,
                        sig_scheme,
                        &sigData[idx],
                        (uint8_t*)(input + *inOutIdx),
                        totalSz);

            if (err == TSIP_SUCCESS) {
                
                *inOutIdx += totalSz;
                *inOutIdx += ssl->keys.padSz;
                ssl->options.peerAuthGood = 1;
                ssl->options.havePeerVerify = 1;
            #if !defined(NO_WOLFSSL_CLIENT)
                if (ssl->options.side == WOLFSSL_CLIENT_END)
                    ssl->options.serverState = SERVER_CERT_VERIFY_COMPLETE;
            #endif
            }
            else {
                ret = WC_HW_E;
                if (err == TSIP_ERR_AUTHENTICATION) {
                    WOLFSSL_MSG("Certificate Verification failed.");
                }
            }

            tsip_hw_unlock();
        }
        else {
            WOLFSSL_MSG("mutex locking error");
        }
    }

    WOLFSSL_LEAVE("tsip_Tls13CertificateVerify", ret);
    return ret;
}
#endif /* WOLFSSL_TLS13 */

#if defined(WOLFSSL_TLS13)
/* Send the TLS v1.3 CertificateVerify message. A part of the message is
 * processed by TSIP for acceleration.
 *
 * Prior to this function call, the appropriate key-pair should be set via 
 * tsip_use_PrivateKey_buffer_TLS and tsip_use_PublicKey_buffer_TLS APIs. 
 * Those key pair can be generated by the tool named 
 * "Renesas secure flash programmer".
 * When RSA certificate is used, both public and private keys should be set.
 * The public key is used for self-verify the generated certificateVerify
 * message. When ECC certificate is used, the self-verify will be performed only
 * WOLFSSL_CHECK_SIG_FAULTS is defined.
 * 
 * Returns 0 on success, CRYPTOCB_UNAVAILABLE when the required key is not 
 * provided or unsupported algo is specified and otherwise failure. 
 */
WOLFSSL_LOCAL int tsip_Tls13SendCertVerify(WOLFSSL* ssl)
{
    int ret = 0;
    e_tsip_err_t    err = TSIP_SUCCESS;
    byte*           sigData = NULL;
    word16          idx;
    int             isTLS13 = 0;
    TsipUserCtx*    tuc     = NULL;
    byte*           output  = NULL;
    byte*           message = NULL;
    byte*           derSig  = NULL;
    int             isRsa   = -1;
    uint32_t        messageSz,recordSz,hashSz;
    byte            hash[WC_SHA256_DIGEST_SIZE];
    byte            sig_rs[R_TSIP_ECDSA_DATA_BYTE_SIZE];
    tsip_rsa_byte_data_t   rsa_sig,rsa_hash;
    tsip_ecdsa_byte_data_t ecdsa_sig,ecdsa_hash;

    WOLFSSL_ENTER("tsip_Tls13SendCertVerify");
    (void)derSig;
    (void)rsa_sig;
    (void)rsa_hash;
    (void)ecdsa_sig;
    (void)ecdsa_hash;
    (void)sig_rs;

    if (ssl == NULL) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        if (ssl->version.major == SSLv3_MAJOR && 
            ssl->version.minor == TLSv1_3_MINOR)
            isTLS13 = 1;

        /* check if it's TLS13 and client side */
        if (!isTLS13 || ssl->options.side != WOLFSSL_CLIENT_END) {
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }
    
    if (ret == 0) {
        /* get user context for TSIP */
        tuc = ssl->RenesasUserCtx;
        if (tuc == NULL) {
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    if (ret == 0) {
        #if !defined(NO_RSA) 
        if (ssl->options.haveRSA)
            isRsa = 1;
        else 
        #endif
        #ifdef HAVE_ECC
        if (ssl->options.haveECC)
            isRsa = 0;
        else
        #endif /* HAVE_ECC */
            isRsa = -1;

        if (isRsa != 0 && isRsa != 1) {
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    if (ret == 0) {
        ret = tsipImportPrivateKey(tuc, tuc->wrappedKeyType);
    }

    if (ret == 0) {
        if (isRsa) {
            if (!tuc->ClientRsa2048PrivKey_set) {
                ret = NO_PRIVATE_KEY;
            }
        }
        else {
            if (!tuc->ClientEccP256PrivKey_set) {
                ret = NO_PRIVATE_KEY;
            }
        }
    }

    if (ret == 0) {    
        /* get message hash */
        ForceZero(hash, sizeof(hash));
        ret = tsip_GetMessageSha256(ssl, hash, (int*)&hashSz);
    }

    if (ret == 0) {
        recordSz = MAX_CERT_VERIFY_SZ + MAX_MSG_EXTRA * 2;
        /* check for available size */
        ret = CheckAvailableSize(ssl, recordSz);
        recordSz = 0;
    }

    if (ret == 0) {
        /* get output buffer for record header */
        output = ssl->buffers.outputBuffer.buffer +
                 ssl->buffers.outputBuffer.length;

        /* buffer for message header */
        message = output + RECORD_HEADER_SZ;
    }

    /* generate signature */
    if (ret == 0) {
        if ((ret = tsip_hw_lock()) == 0) {
            if (isRsa) {
                err = R_TSIP_Tls13CertificateVerifyGenerate(
                            (uint32_t*)&(tuc->Rsa2048PrivateKeyIdx),
                            TSIP_TLS13_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA256,
                                                hash,
                                                message + HANDSHAKE_HEADER_SZ,
                                                &messageSz); 
            }
            else {
                err = R_TSIP_Tls13CertificateVerifyGenerate(
                            (uint32_t*)&(tuc->EcdsaP256PrivateKeyIdx),
                            TSIP_TLS13_SIGNATURE_SCHEME_ECDSA_SECP256R1_SHA256,
                                                hash,
                                                message + HANDSHAKE_HEADER_SZ,
                                                &messageSz); 
            }
            if (err != TSIP_SUCCESS) {
                WOLFSSL_MSG("failed to make certificate verify message");
                ret = WC_HW_E;
            }
            tsip_hw_unlock();
        }
        else {
            WOLFSSL_MSG("mutex locking error");
        }
    }

    if (ret == 0) {
        if (isRsa) {
            ret = tsipImportPublicKey(tuc, tuc->wrappedKeyType);
        }
        else {
#if defined(WOLFSSL_CHECK_SIG_FAULTS)
            ret = tsipImportPublicKey(tuc, tuc->wrappedKeyType);
#endif
        }
    }

    if (ret == 0) {
        if (isRsa) {
            if (!tuc->ClientRsa2048PubKey_set) {
                ret = NO_PRIVATE_KEY;
            }
        }
        else {
#if defined(WOLFSSL_CHECK_SIG_FAULTS)    
            if (!tuc->ClientEccP256PubKey_set) {
                ret = NO_PRIVATE_KEY;
            }
#endif /* WOLFSSL_CHECK_SIG_FAULTS */
        }
    }

    if (ret == 0) {
        sigData = tuc->sigDataCertVerify;

        idx = 0;
        ForceZero(sigData, sizeof(tuc->sigDataCertVerify));
        XMEMSET(sigData, TSIP_SIGNING_DATA_PREFIX_BYTE, 
                                                TSIP_SIGNING_DATA_PREFIX_SZ);

        idx += TSIP_SIGNING_DATA_PREFIX_SZ;
        XMEMCPY(&sigData[idx], clientCertVfyLabel, TSIP_CERT_VFY_LABEL_SZ);

        idx += TSIP_CERT_VFY_LABEL_SZ;
        XMEMCPY(&sigData[idx], hash, hashSz);
    }

    if (ret == 0) {
        /* extract signature data from generated CertificateVerify message */
        if (!isRsa) {
#if defined(WOLFSSL_CHECK_SIG_FAULTS)            
            idx = 4; 
            derSig = message + 
                        HANDSHAKE_HEADER_SZ + HASH_SIG_SIZE + VERIFY_HEADER;
            if (derSig[idx] == 0x00)
                idx++;
            XMEMCPY(sig_rs, &derSig[idx], R_TSIP_ECDSA_DATA_BYTE_SIZE / 2);
            idx += (R_TSIP_ECDSA_DATA_BYTE_SIZE / 2) + ASN_TAG_SZ + 1;
            if (derSig[idx] == 0x00)
                idx++;
            XMEMCPY(&sig_rs[R_TSIP_ECDSA_DATA_BYTE_SIZE / 2],
                            &derSig[idx], R_TSIP_ECDSA_DATA_BYTE_SIZE / 2);
#endif /* WOLFSSL_CHECK_SIG_FAULTS */
        }
    }

    if (ret == 0) {
        if ((ret = tsip_hw_lock()) == 0) {
            if (isRsa) {
                rsa_sig.pdata  = message + HANDSHAKE_HEADER_SZ +
                                                HASH_SIG_SIZE + VERIFY_HEADER;
                rsa_hash.pdata = sigData;
                rsa_hash.data_length = TSIP_SIGNING_DATA_PREFIX_SZ +
                                         TSIP_CERT_VFY_LABEL_SZ + sizeof(hash);

                rsa_hash.data_type = 0;

                err = R_TSIP_RsassaPss2048SignatureVerification(
                            &rsa_sig, &rsa_hash,
                            &tuc->Rsa2048PublicKeyIdx,
                            R_TSIP_RSA_HASH_SHA256);
                WOLFSSL_MSG("Perform self-verify for rsa signature");
            }
            else {
                err = TSIP_SUCCESS;
#if defined(WOLFSSL_CHECK_SIG_FAULTS)
                ecdsa_sig.pdata  = sig_rs;
                ecdsa_hash.pdata = sigData;
                ecdsa_hash.data_length = TSIP_SIGNING_DATA_PREFIX_SZ +
                                         TSIP_CERT_VFY_LABEL_SZ + sizeof(hash);
                ecdsa_hash.data_type = 0;

                err = R_TSIP_EcdsaP256SignatureVerification(
                            &ecdsa_sig, &ecdsa_hash,
                            &tuc->EcdsaP256PublicKeyIdx);
                WOLFSSL_MSG("Perform self-verify for ecc signature");
#endif /* WOLFSSL_CHECK_SIG_FAULTS */
            }
            if (err != TSIP_SUCCESS) {
                WOLFSSL_MSG("Failed to verify signature");
                ret = VERIFY_SIGN_ERROR;
            }
            tsip_hw_unlock();
        }
        else {
            WOLFSSL_MSG("mutex locking error");
        }
    }

    /* create message header */
    if (ret == 0) {

        ((HandShakeHeader*)message)->type = certificate_verify;

        c32to24(messageSz, ((HandShakeHeader*)message)->length);
        
        recordSz = tsip_Tls13BuildMessage(ssl, output, 0, message, 
                                          messageSz + HANDSHAKE_HEADER_SZ,
                                                             handshake, 1);

        if (recordSz > 0) {
            ssl->buffers.outputBuffer.length += recordSz;
            ret = SendBuffered(ssl);
        }
        else {
            ret = recordSz;
        }
    }

    WOLFSSL_LEAVE("tsip_Tls13SendCertVerify", ret);
    return ret;
}
#endif /* WOLFSSL_TLS13 */
#endif /* WOLFSSL_RENESAS_TSIP_TLS */

#if defined(WOLFSSL_RENESAS_TSIP_TLS) && (WOLFSSL_RENESAS_TSIP_VER >=109)

static uint32_t GetTsipCipherSuite(
                    uint8_t cipherSuiteFirst,
                    uint8_t cipherSuite)
{
    WOLFSSL_ENTER("GetTsipCipherSuite");
    uint32_t tsipCipher;

    if (cipherSuiteFirst == CIPHER_BYTE)
    {
        switch(cipherSuite) {

            case TLS_RSA_WITH_AES_128_CBC_SHA: /*2F*/
                tsipCipher = R_TSIP_TLS_RSA_WITH_AES_128_CBC_SHA; /*0*/
                break;

            case TLS_RSA_WITH_AES_128_CBC_SHA256:
                tsipCipher = R_TSIP_TLS_RSA_WITH_AES_128_CBC_SHA256;
                break;

            case TLS_RSA_WITH_AES_256_CBC_SHA:
                tsipCipher = R_TSIP_TLS_RSA_WITH_AES_256_CBC_SHA;
                break;

            case TLS_RSA_WITH_AES_256_CBC_SHA256:
                tsipCipher = R_TSIP_TLS_RSA_WITH_AES_256_CBC_SHA256;
                break;

            default:
                tsipCipher = (uint32_t)WOLFSSL_TSIP_ILLEGAL_CIPHERSUITE;
                break;
        }
        WOLFSSL_LEAVE("GetTsipCipherSuite", tsipCipher);
        return tsipCipher;
    }
    else if (cipherSuiteFirst == ECC_BYTE)
    {
        tsipCipher = (uint32_t)WOLFSSL_TSIP_ILLEGAL_CIPHERSUITE;

        switch(cipherSuite) {

            case TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256:
                tsipCipher = R_TSIP_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256;
                break;

            case TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256:
                tsipCipher = R_TSIP_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256;
                break;

            case TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256:
                tsipCipher = R_TSIP_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256;
                break;

            case TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256:
                tsipCipher = R_TSIP_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256;
                break;

            default:
                tsipCipher = (uint32_t)WOLFSSL_TSIP_ILLEGAL_CIPHERSUITE;
                break;
        }
    }
    else {
        tsipCipher = (uint32_t)WOLFSSL_TSIP_ILLEGAL_CIPHERSUITE;
    }

    WOLFSSL_LEAVE("GetTsipCipherSuite", tsipCipher);

    return tsipCipher;
}

/*  Attempt to get a public key exchanged with the peer in ECDHE.
 *  the public key is verified by given signature then stored into ctx.
 *
 *  return WOLFSSL_SUCCESS on success, WOLFSSL_FAILURE on failure.
 */
static int tsip_ServerKeyExVerify(
    word32      type,       /* public key type 0:RSA2048 2:ECDSA P-256 */
    WOLFSSL*    ssl,
    const byte* sig,
    word32      sigSz,
    void*       ctx)
{
    int     ret = WOLFSSL_FAILURE;
    byte    qx[MAX_ECC_BYTES], qy[MAX_ECC_BYTES];
    byte*   peerkey = NULL;
    word32  qxLen = sizeof(qx), qyLen = sizeof(qy);
    TsipUserCtx*   userCtx = NULL;

    WOLFSSL_ENTER("tsip_ServerKeyExVerify");

    /* sanity check */
    if (ssl == NULL || sig == NULL || ctx == NULL)
        return ret;

    userCtx = (TsipUserCtx*)ctx;

    /* export public peer public key */
    ret = wc_ecc_export_public_raw(ssl->peerEccKey, qx, &qxLen, qy, &qyLen);

    if (ret != 0) {
        WOLFSSL_MSG("failed to export peer ecc key");
        WOLFSSL_LEAVE("tsip_ServerKeyExVerify", ret);
        return ret;
    }
    /* make peer ecc key data for SCE */
    /* 0padding(24bit) || 04(8bit) || Qx(256bit) || Qy(256bit) */
    peerkey = (byte*)XMALLOC((3 + 1 + qxLen + qyLen), NULL,
                                                 DYNAMIC_TYPE_TMP_BUFFER);
    if (peerkey == NULL) {
        WOLFSSL_MSG("failed to malloc ecc key");
        WOLFSSL_LEAVE("tsip_ServerKeyExVerify", ret);
        return WOLFSSL_FAILURE;
    }

    ForceZero(peerkey, (3 + 1 + qxLen + qyLen));
    peerkey[3] = ECC_POINT_UNCOMP;
    XMEMCPY(&peerkey[4], qx, qxLen);
    XMEMCPY(&peerkey[4+qxLen], qy, qyLen);

    /* 0 : RSA 2048bit, 1 : Reserved, 2 : ECDSA P-256 */
    if ((ret = tsip_hw_lock()) == 0) {
        ret = R_TSIP_TlsServersEphemeralEcdhPublicKeyRetrieves(
            type,
            (uint8_t*) ssl->arrays->clientRandom,
            (uint8_t*) ssl->arrays->serverRandom,
            (uint8_t*) peerkey,
            (uint8_t*) sig,
            (uint32_t*)ssl->peerSceTsipEncRsaKeyIndex,
            (uint32_t*)userCtx->encrypted_ephemeral_ecdh_public_key);

        if (ret !=TSIP_SUCCESS) {
            WOLFSSL_MSG("R_TSIP_TlsServersEphemeralEcdhPublicKeyRetrieves failed");
        }
        else {
            ret = WOLFSSL_SUCCESS;
        }

        tsip_hw_unlock();
    }
    else {
        WOLFSSL_MSG("Failed to lock tsip hw");
    }

    XFREE(peerkey, 0, DYNAMIC_TYPE_TMP_BUFFER);

    WOLFSSL_LEAVE("tsip_ServerKeyExVerify", ret);
    return ret;
}
/*
 *  return 0 on success
 */
int wc_tsip_RsaVerify(
        WOLFSSL* ssl,
        byte* sig,      word32 sigSz,
        byte** out,
        const byte* key,
        word32 keySz,
        void* ctx)
{
    int ret;

    WOLFSSL_ENTER("tsip_RsaVerify");

    if (tsip_usable(ssl, 0))
        ret = tsip_ServerKeyExVerify(0, ssl, sig, sigSz, ctx);
    else
        ret = CRYPTOCB_UNAVAILABLE;

    if (ret == WOLFSSL_SUCCESS)
        ret = 0;

    WOLFSSL_LEAVE("tsip_RsaVerify", ret);
    return ret;
}
/*  Verify signature for Server Key Exchange with TSIP
 *  TSIP can handle prime256v1 curve and sha256 hash
 *  parameters:
 *   ssl    WOLFSSL object
 *   sig    buffer holding DER encoded ecdsa signature data
 *   sigSz  signature data size
 *   hash   buffer holding sha256 hash data
 *   hashSz hash data size
 *   key    buffer holding peer's public key (NOT used in this function)
 *   keySz  public key size((NOT used in this function))
 *   result address of the variable to output result
 *   ctx    context 
 *  return 0 on success, CRYPTOCB_UNAVAILABLE in case TSIP cannot handle 
 */
int wc_tsip_EccVerify(
        WOLFSSL*  ssl,
        const byte* sig,    word32 sigSz,
        const byte* hash,   word32 hashSz,
        const byte* key,    word32 keySz,
        int*  result,       void*  ctx)
{
    int         ret = WOLFSSL_FAILURE;
    uint8_t     sigforSCE [R_TSIP_ECDSA_DATA_BYTE_SIZE] = {0};
    const byte  rs_size = R_TSIP_ECDSA_DATA_BYTE_SIZE/2;
    byte        offset = 0x3;

    WOLFSSL_ENTER("wc_tsip_EccVerify");

    /* check if TSIP can handle given cipher suite */
    if (!tsip_usable(ssl, 0)) {
        WOLFSSL_MSG("Cannot handle cipher suite by TSIP");
        WOLFSSL_LEAVE("wc_tsip_EccVerify", CRYPTOCB_UNAVAILABLE);
        return CRYPTOCB_UNAVAILABLE;
    }

    /* in TLS1.3 */ 
    if (ssl->version.major == SSLv3_MAJOR && 
        ssl->version.minor == TLSv1_3_MINOR) {
        WOLFSSL_LEAVE("wc_tsip_EccVerify", CRYPTOCB_UNAVAILABLE);
        return CRYPTOCB_UNAVAILABLE;
    }

    /* concatenate r and s parts of the signature so that TSIP can handle it*/
    /* r */
    if (sig[offset] == 0x20) {
        XMEMCPY(sigforSCE, &sig[offset+1], rs_size);

        offset = 0x25;
        /* s */
        if (sig[offset] == 0x20) {
          XMEMCPY(&sigforSCE[rs_size], &sig[offset+1], rs_size);
        }
        else {
          XMEMCPY(&sigforSCE[rs_size], &sig[offset+2], rs_size);
        }
    }
    else {
        XMEMCPY(sigforSCE, &sig[offset+2], rs_size);

        offset = 0x26;
        /* s */
        if (sig[offset] == rs_size) {
          XMEMCPY(&sigforSCE[rs_size], &sig[offset+1], rs_size);
        }
        else {
          XMEMCPY(&sigforSCE[rs_size], &sig[offset+2], rs_size);
        }
    }

    ret = tsip_ServerKeyExVerify(2, ssl, sigforSCE, 64, ctx);

    if (ret == WOLFSSL_SUCCESS) {
        *result = 1;
        ret = 0; /* for success */
    }
    else
        *result = 0;

    WOLFSSL_LEAVE("wc_tsip_EccVerify", ret);
    return ret;
}

/*
 *  generate premaster secret
 *  1. generate P256 ECC key pair for ECDHE key exchange
 *  2. generate pre-master secret
 *  output 64 bytes premaster secret to "out" buffer.
 */
int wc_tsip_EccSharedSecret(
    WOLFSSL* ssl,
    ecc_key* otherKey,
    unsigned char* pubKeyDer,   unsigned int* pubKeySz,
    unsigned char* out,         unsigned int* outlen,
    int side, void* ctx)
{
    int       ret;
    TsipUserCtx* usrCtx = (TsipUserCtx*)ctx;

    (void)ssl;
    (void)otherKey;

    WOLFSSL_ENTER("wc_tsip_EccSharedSecret");
    /* sanity check */
    if (ssl == NULL || pubKeyDer == NULL || pubKeySz == NULL ||
        out == NULL || outlen == NULL || ctx == NULL) {
        WOLFSSL_LEAVE("wc_tsip_EccSharedSecret", WOLFSSL_FAILURE);
        return WOLFSSL_FAILURE;
    }
    if ((ret = tsip_hw_lock()) == 0) {
        /* Generate ECC public key for key exchange */
        ret = R_TSIP_GenerateTlsP256EccKeyIndex(
                    &usrCtx->ecc_p256_wrapped_key,
                    (uint8_t*)&usrCtx->ecc_ecdh_public_key);

        if (ret == TSIP_SUCCESS) {

            /* copy generated ecdh public key into buffer */
            pubKeyDer[0] = ECC_POINT_UNCOMP;
            *pubKeySz = 1 + sizeof(usrCtx->ecc_ecdh_public_key);
            XMEMCPY(&pubKeyDer[1], &usrCtx->ecc_ecdh_public_key,
                        sizeof(usrCtx->ecc_ecdh_public_key));

            /* Generate Premaster Secret */
            ret = R_TSIP_TlsGeneratePreMasterSecretWithEccP256Key(
                        (uint32_t*)&usrCtx->encrypted_ephemeral_ecdh_public_key,
                        &usrCtx->ecc_p256_wrapped_key,
                        (uint32_t*)out/* pre-master secret 64 bytes */);
        }
        if (ret == TSIP_SUCCESS) {
            *outlen = 64;
            wolfSSL_CTX_SetGenMasterSecretCb(ssl->ctx,
                                                Renesas_cmn_genMasterSecret);
            wolfSSL_SetGenMasterSecretCtx(ssl, usrCtx);

        }

        tsip_hw_unlock();
    }
    else {
        WOLFSSL_MSG("Failed to lock tsip hw");
    }
    WOLFSSL_LEAVE("wc_tsip_EccSharedSecret", ret);
    return ret;
}


WOLFSSL_API void tsip_set_callbacks(WOLFSSL_CTX* ctx)
{
    WOLFSSL_ENTER("tsip_set_callbacks");
    wolfSSL_CTX_SetEccVerifyCb(ctx, (CallbackEccVerify)Renesas_cmn_EccVerify);
    wolfSSL_CTX_SetRsaVerifyCb(ctx, (CallbackRsaVerify)Renesas_cmn_RsaVerify);
    wolfSSL_CTX_SetGenPreMasterCb(ctx, Renesas_cmn_generatePremasterSecret);
    wolfSSL_CTX_SetRsaEncCb(ctx, Renesas_cmn_RsaEnc);
#if !defined(WOLFSSL_NO_TLS12) && !defined(WOLFSSL_AEAD_ONLY)
    wolfSSL_CTX_SetVerifyMacCb(ctx, (CallbackVerifyMac)Renesas_cmn_VerifyHmac);
#endif /* !WOLFSSL_NO_TLS12 && !WOLFSSL_AEAD_ONLY */
    wolfSSL_CTX_SetEccSharedSecretCb(ctx, NULL);
    /* Set ssl-> options.sendVerify to SEND_CERT by the following two
     * registrations. This will allow the client certificate to be sent to 
     * the server even if the private key is empty. The two callbacks do
     * virtually nothing.
     */
    #ifdef WOLFSSL_TLS13
    #ifdef HAVE_ECC
    wolfSSL_CTX_SetEccSignCb(ctx, Renesas_cmn_EccSignCb);
    #endif
    #ifndef NO_RSA
    wolfSSL_CTX_SetRsaSignCb(ctx, Renesas_cmn_RsaSignCb);
    #endif
    #endif /* WOLFSSL_TLS13 */

    wolfSSL_CTX_SetRsaSignCheckCb(ctx, Renesas_cmn_RsaSignCheckCb);

    /* set heap-hint to tsip_heap_hint so that tsip sha funcs can refer it */
    if (ctx->heap != NULL) {
        tsip_heap_hint = ctx->heap;
    }

    WOLFSSL_LEAVE("tsip_set_callbacks", 0);
}

WOLFSSL_API int tsip_set_callback_ctx(WOLFSSL* ssl, void* user_ctx)
{
    WOLFSSL_ENTER("tsip_set_callback_ctx");

    TsipUserCtx* uCtx = (TsipUserCtx*)user_ctx;
    if (user_ctx == NULL) {
        WOLFSSL_LEAVE("tsip_set_callback_ctx", 0);
        return 0;
    }
    ForceZero(uCtx, sizeof(TsipUserCtx));
    uCtx->ssl  = ssl;
    uCtx->ctx  = ssl->ctx;
    uCtx->heap = ssl->heap;
    uCtx->side = ssl->ctx->method->side;

    ssl->RenesasUserCtx = user_ctx;     /* ssl doesn't own user_ctx */

    wolfSSL_SetEccVerifyCtx(ssl, user_ctx);
    wolfSSL_SetRsaEncCtx(ssl, user_ctx);
    wolfSSL_SetRsaVerifyCtx(ssl, user_ctx);
    wolfSSL_SetRsaSignCtx(ssl, user_ctx);
    wolfSSL_SetGenPreMasterCtx(ssl, user_ctx);
    wolfSSL_SetEccSharedSecretCtx(ssl, NULL);
#if !defined(WOLFSSL_NO_TLS12) && !defined(WOLFSSL_AEAD_ONLY)    
    wolfSSL_SetVerifyMacCtx(ssl, user_ctx);
#endif /* !WOLFSSL_NO_TLS12 && !WOLFSSL_AEAD_ONLY */
    /* set up crypt callback */
    wc_CryptoCb_CryptInitRenesasCmn(ssl, user_ctx);
    WOLFSSL_LEAVE("tsip_set_callback_ctx", 0);
    return 0;
}

#elif defined(WOLFSSL_RENESAS_TSIP_TLS) && (WOLFSSL_RENESAS_TSIP_VER >=106)

/* convert def to tsip define */
static byte _tls2tsipdef(byte cipher)
{
    byte def = R_TSIP_TLS_RSA_WITH_AES_128_CBC_SHA;
    switch(cipher) {
        case l_TLS_RSA_WITH_AES_128_CBC_SHA:
            break;
        case l_TLS_RSA_WITH_AES_128_CBC_SHA256:
            def = R_TSIP_TLS_RSA_WITH_AES_128_CBC_SHA256;
            break;
        case l_TLS_RSA_WITH_AES_256_CBC_SHA:
            def = R_TSIP_TLS_RSA_WITH_AES_256_CBC_SHA;
            break;
        case l_TLS_RSA_WITH_AES_256_CBC_SHA256:
            def = R_TSIP_TLS_RSA_WITH_AES_256_CBC_SHA256;
            break;
        default:break;
    }
    return def;
}
#endif

#ifdef WOLFSSL_RENESAS_TSIP_TLS
/*
 * Import wrapped private key then convert it into TSIP key_index format.
 * The target key should be set with tsip_use_PrivateKey_buffer in advance.
 * Acceptable key types are:
 *   TSIP_KEY_TYPE_RSA2048     rsa 2048 bit key
 *   TSIP_KEY_TYPE_RSA4096     rsa 4096 bit key(Not supported as of now)
 *   TSIP_KEY_TYPE_ECDSAP256   ecdsa p256r1 key
 */
static int tsipImportPrivateKey(TsipUserCtx* tuc, int keyType)
{
    int          ret = 0;
    e_tsip_err_t err = TSIP_SUCCESS;
    uint8_t* provisioning_key = g_user_key_info.encrypted_provisioning_key;
    uint8_t* iv               = g_user_key_info.iv;
    uint8_t* encPrivKey;

    WOLFSSL_ENTER("tsipImportPrivateKey");

    if (tuc == NULL)
        return BAD_FUNC_ARG;

    encPrivKey = tuc->wrappedPrivateKey;

    if (encPrivKey == NULL || provisioning_key == NULL || iv == NULL) {
        WOLFSSL_MSG("Missing some key materials used for import" );
        return CRYPTOCB_UNAVAILABLE;
    } 

    if (ret == 0) {
        if (keyType != tuc->wrappedKeyType) {
            WOLFSSL_MSG("No public key of specified type is set" );
            return CRYPTOCB_UNAVAILABLE;
        }
    }

    if ((ret = tsip_hw_lock()) == 0) {
        switch (keyType) {

            #if !defined(NO_RSA)
            case TSIP_KEY_TYPE_RSA2048:
                
                tuc->ClientRsa2048PrivKey_set = 0;
                err = R_TSIP_GenerateRsa2048PrivateKeyIndex(
                                    provisioning_key, iv, (uint8_t*)encPrivKey,
                                    &(tuc->Rsa2048PrivateKeyIdx));
                if (err == TSIP_SUCCESS) {
                    tuc->ClientRsa2048PrivKey_set = 1;
                }
                else {
                    ret = WC_HW_E;
                }
                break;
            #endif

            case TSIP_KEY_TYPE_RSA4096:
                /* not supported as of TSIPv1.15 */ 
                ret = CRYPTOCB_UNAVAILABLE;
                break;

            #if defined(HAVE_ECC)
            case TSIP_KEY_TYPE_ECDSAP256:

                tuc->ClientEccP256PrivKey_set = 0;
                err = R_TSIP_GenerateEccP256PrivateKeyIndex(
                                    provisioning_key, iv, (uint8_t*)encPrivKey,
                                    &(tuc->EcdsaP256PrivateKeyIdx));
                if (err == TSIP_SUCCESS) {
                    tuc->ClientEccP256PrivKey_set = 1;
                }
                else {
                    ret = WC_HW_E;
                }
                break;
            #endif

            default:
                ret = BAD_FUNC_ARG;
                break;
        }
        tsip_hw_unlock();
    }
    else {
        WOLFSSL_MSG("mutex locking error");
    }
    WOLFSSL_LEAVE("tsipImportPrivateKey", ret);
    return ret;
}

#endif /* WOLFSSL_RENESAS_TSIP_TLS */

/*
 * Import wrapped public key then convert it into TSIP key_index format.
 * The target key should be set with tsip_use_PublicKey_buffer in advance.
 * Acceptable key types are:
 *   TSIP_KEY_TYPE_RSA2048     rsa 2048 bit key
 *   TSIP_KEY_TYPE_RSA4096     rsa 4096 bit key(Not supported as of now)
 *   TSIP_KEY_TYPE_ECDSAP256   ecdsa p256r1 key
 */
WOLFSSL_LOCAL int tsipImportPublicKey(TsipUserCtx* tuc, int keyType)
{
    int          ret = 0;
    e_tsip_err_t err = TSIP_SUCCESS;
    uint8_t* provisioning_key = g_user_key_info.encrypted_provisioning_key;
    uint8_t* iv               = g_user_key_info.iv;
    uint8_t* encPubKey;

    WOLFSSL_ENTER("tsipImportPublicKey");

    if (tuc == NULL ) {
        return BAD_FUNC_ARG;
    }

    encPubKey = tuc->wrappedPublicKey;

    if (encPubKey == NULL || provisioning_key == NULL || iv == NULL) {
        WOLFSSL_MSG("Missing some key materials used for import" );
        return CRYPTOCB_UNAVAILABLE;
    } 

    if (ret == 0) {
        if (keyType != tuc->wrappedKeyType) {
            WOLFSSL_MSG("No public key of specified type is set" );
            return CRYPTOCB_UNAVAILABLE;
        }
    }

    if ((ret = tsip_hw_lock()) == 0) {
        switch(keyType) {
            
        #if !defined(NO_RSA)
            case TSIP_KEY_TYPE_RSA2048:
            #if defined(WOLFSSL_RENESAS_TSIP_TLS)
                tuc->ClientRsa2048PubKey_set = 0;
            #elif defined(WOLFSSL_RENESAS_TSIP_CRYPTONLY)
                if (tuc->rsa2048pub_keyIdx != NULL) {
                    XFREE(tuc->rsa2048pub_keyIdx, NULL,
                                    DYNAMIC_TYPE_RSA_BUFFER);
                }
                
                tuc->rsa2048pub_keyIdx =
                (tsip_rsa2048_public_key_index_t*)XMALLOC(
                    sizeof(tsip_rsa2048_public_key_index_t), NULL, 
                                    DYNAMIC_TYPE_RSA_BUFFER);
                if (tuc->rsa2048pub_keyIdx == NULL) {
                    return MEMORY_E;
                }
            #endif
                err = R_TSIP_GenerateRsa2048PublicKeyIndex(
                                    provisioning_key, iv, (uint8_t*)encPubKey,
                                #if defined(WOLFSSL_RENESAS_TSIP_TLS)
                                     &(tuc->Rsa2048PublicKeyIdx)
                                #elif defined(WOLFSSL_RENESAS_TSIP_CRYPTONLY)
                                     tuc->rsa2048pub_keyIdx
                                #endif
                                    );
                if (err == TSIP_SUCCESS) {
                #if defined(WOLFSSL_RENESAS_TSIP_TLS)
                    tuc->ClientRsa2048PubKey_set = 1;
                #elif defined(WOLFSSL_RENESAS_TSIP_CRYPTONLY)
                    tuc->keyflgs_crypt.bits.rsapub2048_key_set = 1;
                #endif
                }
                else {
                    ret = WC_HW_E;
                }
                break;
        #endif
            
        #if !defined(NO_RSA)
            case TSIP_KEY_TYPE_RSA4096:
                /* not supported as of TSIPv1.15 */ 
                ret = CRYPTOCB_UNAVAILABLE;
                break;
        #endif
            
        #if defined(HAVE_ECC) && \
            defined(WOLFSSL_RENESAS_TSIP_TLS)
            case TSIP_KEY_TYPE_ECDSAP256:

                tuc->ClientEccP256PubKey_set = 0;
                err = R_TSIP_GenerateEccP256PublicKeyIndex(
                                    provisioning_key, iv, (uint8_t*)encPubKey,
                                    &(tuc->EcdsaP256PublicKeyIdx));
                if (err == TSIP_SUCCESS) {
                    tuc->ClientEccP256PubKey_set = 1;
                }
                else {
                    ret = WC_HW_E;
                }
                break;
        #endif

            default:
                ret = BAD_FUNC_ARG;
                break;
        }
        tsip_hw_unlock();
    }
    else {
        WOLFSSL_MSG("mutex locking error");
    }
    WOLFSSL_LEAVE("tsipImportPublicKey", ret);
    return ret;
}
/*
* lock hw engine.
* this should be called before using engine.
*/
WOLFSSL_LOCAL int tsip_hw_lock(void)
{
    int ret = 0;

    if (tsip_CryptHwMutexInit_ == 0) {

        ret = tsip_CryptHwMutexInit(&tsip_mutex);

        if (ret == 0) {
            tsip_CryptHwMutexInit_ = 1;
        }
        else {
            WOLFSSL_MSG(" mutex initialization failed.");
            return -1;
        }
    }
    if (tsip_CryptHwMutexLock(&tsip_mutex) != 0) {
        /* this should not happens */
        return -1;
    }

    return ret;
}

/*
* release hw engine
*/
WOLFSSL_LOCAL void tsip_hw_unlock(void)
{
    tsip_CryptHwMutexUnLock(&tsip_mutex);
}
#if defined(WOLFSSL_RENESAS_TSIP_TLS)
/* check if tsip tls functions can be used for the cipher      */
/* return  :1 when tsip can be used , 0 not be used.           */
int tsip_usable(const WOLFSSL *ssl, uint8_t session_key_generated)
{
    byte cipher0 = ssl->options.cipherSuite0;
    byte cipher  = ssl->options.cipherSuite;
    byte side    = ssl->options.side;
    int  ret     = WOLFSSL_SUCCESS;
    const Ciphers *enc;
    const Ciphers *dec;

    WOLFSSL_ENTER("tsip_usable");

    /* sanity check */
    if (ssl == NULL) {
        WOLFSSL_MSG("ssl is NULL");
        ret = BAD_FUNC_ARG;
    }

    /* when rsa key index == NULL, tsip isn't used for cert verification. */
    /* in the case, we cannot use TSIP.                                   */
    if (ret == WOLFSSL_SUCCESS) {
        if (!ssl->peerSceTsipEncRsaKeyIndex) {
            WOLFSSL_MSG("ssl->peerSceTsipEncRsaKeyIndex is NULL");
            ret = WOLFSSL_FAILURE;
        }
    }

    if (ret == WOLFSSL_SUCCESS) {
        if (session_key_generated) {
            enc = &ssl->encrypt;
            dec = &ssl->decrypt;
            if (enc == NULL || dec == NULL) {
                /* something wrong */
                ret = WOLFSSL_FAILURE;
            }
            if (enc->aes == NULL || dec->aes == NULL) {
                ret = WOLFSSL_FAILURE;
            }
            if (enc->aes->ctx.setup == 0) {
                /* session key for SCE is not created */
                ret = WOLFSSL_FAILURE;
            }
        }
    }

    /* when enabled Extended Master Secret, we cannot use TSIP. */

    if (ret == WOLFSSL_SUCCESS) {
        if (ssl->options.haveEMS) {
            WOLFSSL_MSG("ssl->options.haveEMS");
            ret = WOLFSSL_FAILURE;
        }
    }
    /* TSIP works only for TLS client */
    if (ret == WOLFSSL_SUCCESS) {
        if (side != WOLFSSL_CLIENT_END) {
            WOLFSSL_MSG("Not client side");
            ret = WOLFSSL_FAILURE;
        }
    }
    /* Check if TSIP can handle cipher suite */
    if (ret == WOLFSSL_SUCCESS) {
        if (
            (cipher0 == CIPHER_BYTE &&
            (cipher == l_TLS_RSA_WITH_AES_128_CBC_SHA ||
             cipher == l_TLS_RSA_WITH_AES_128_CBC_SHA256 ||
             cipher == l_TLS_RSA_WITH_AES_256_CBC_SHA ||
             cipher == l_TLS_RSA_WITH_AES_256_CBC_SHA256))
        #if (WOLFSSL_RENESAS_TSIP_VER >= 109)
            ||
            (cipher0 == ECC_BYTE &&
            (cipher == l_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256 ||
             cipher == l_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256 ||
             cipher == l_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256 ||
             cipher == l_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256))
        #endif
        #if (WOLFSSL_RENESAS_TSIP_VER >= 114)
            ||
            (cipher0 == TLS13_BYTE &&
            (cipher == l_TLS_AES_128_GCM_SHA256 ||
             cipher == l_TLS_AES_128_CCM_SHA256))
        #endif
        ) {
            WOLFSSL_MSG("supported cipher suite");
        }
        else {
            WOLFSSL_MSG("unsupported cipher suite");
            ret = WOLFSSL_FAILURE;;
        }
    }

    WOLFSSL_LEAVE("tsip_usable", ret);
    return ret;
}
#endif /* WOLFSSL_RENESAS_TSIP_TLS */


/* open TSIP driver
 * return 0 on success.
 */
WOLFSSL_LOCAL int tsip_Open(void)
{
    int ret = TSIP_SUCCESS;

    WOLFSSL_ENTER("tsip_Open");

    if ((ret = tsip_hw_lock()) == 0) {

#if defined(WOLFSSL_RENESAS_TSIP) && (WOLFSSL_RENESAS_TSIP_VER>=109)

        ret = R_TSIP_Open(NULL,NULL);
        if (ret != TSIP_SUCCESS) {
            WOLFSSL_MSG("RENESAS TSIP Open failed");
        }
    #if defined(WOLFSSL_RENESAS_TSIP_TLS)
        if (ret == TSIP_SUCCESS && g_user_key_info.encrypted_user_tls_key) {

            ret = R_TSIP_GenerateTlsRsaPublicKeyIndex(
                    g_user_key_info.encrypted_provisioning_key,
                    g_user_key_info.iv,
                    g_user_key_info.encrypted_user_tls_key,
                    &g_user_key_info.user_rsa2048_tls_pubindex); /* OUT */

            R_TSIP_Close();       /* close once */

            if (ret != TSIP_SUCCESS) {

                WOLFSSL_MSG("R_TSIP_GenerateTlsRsa: NG");

            }
            else {

                /* open again with newly created TLS public key index*/
                ret = R_TSIP_Open(
                        &g_user_key_info.user_rsa2048_tls_pubindex,
                        (tsip_update_key_ring_t*)s_inst2);

                if (ret != TSIP_SUCCESS) {
                    WOLFSSL_MSG("R_TSIP_(Re)Open: NG");
                }
                    /* init vars */
                g_CAscm_Idx = (uint32_t)-1;
            }
        }
    #endif
#elif defined(WOLFSSL_RENESAS_TSIP) && (WOLFSSL_RENESAS_TSIP_VER>=106)

        ret = R_TSIP_Open((uint32_t*)s_flash, s_inst1, s_inst2);
        if (ret != TSIP_SUCCESS) {
            WOLFSSL_MSG("RENESAS TSIP Open failed");
        }
       #if defined(WOLFSSL_RENESAS_TLS)
        /* generate TLS Rsa public key for Certificate verification */
        if (ret == TSIP_SUCCESS && g_user_key_info.encrypted_user_tls_key) {
            ret = R_TSIP_GenerateTlsRsaPublicKeyIndex(
                    g_user_key_info.encrypted_session_key,
                    g_user_key_info.iv,
                    g_user_key_info.encrypted_user_tls_key,
                    &g_user_key_info.user_rsa2048_tls_pubindex);

            if (ret != TSIP_SUCCESS) {
                WOLFSSL_MSG("R_TSIP_GenerateTlsRsaPublicKeyIndex failed");
            }
            else {
                /* close once */
                tsip_Close();
                /* open again with s_inst[] */
                XMEMCPY(s_inst1,
                    g_user_key_info.user_rsa2048_tls_pubindex.value,
                    sizeof(s_inst1));
                ret = R_TSIP_Open((uint32_t*)s_flash, s_inst1, s_inst2);
                if (ret != TSIP_SUCCESS) {
                    WOLFSSL_MSG("R_TSIP_(Re)Open failed");
                }
                 /* init vars */
                g_CAscm_Idx = (uint32_t)-1;
            }
        }
    #endif
#else
        ret = R_TSIP_Open((uint32_t*)s_flash, s_inst1, s_inst2);
        if (ret != TSIP_SUCCESS) {
            WOLFSSL_MSG("RENESAS TSIP Open failed");
        }
#endif
        /* unlock hw */
        tsip_hw_unlock();
    }
    else
        WOLFSSL_MSG("Failed to lock tsip hw ");

    WOLFSSL_LEAVE("tsip_Open", ret);
    return ret;
}

/* close TSIP driver */
WOLFSSL_LOCAL void tsip_Close(void)
{
    WOLFSSL_ENTER("tsip_Close");
    int ret;

    if ((ret = tsip_hw_lock()) == 0) {
        /* close TSIP */
        ret = R_TSIP_Close();
#if defined(WOLFSSL_RENESAS_TSIP_TLS)
        g_CAscm_Idx = (uint32_t)-1;
#endif
        /* unlock hw */
        tsip_hw_unlock();
        if (ret != TSIP_SUCCESS) {
            WOLFSSL_MSG("RENESAS TSIP Close failed");
        }
    }
    else
        WOLFSSL_MSG("Failed to unlock tsip hw");
    WOLFSSL_LEAVE("tsip_Close", 0);
}

#if (WOLFSSL_RENESAS_TSIP_VER>=109)
void tsip_inform_user_keys_ex(
    byte*     encrypted_provisioning_key,
    byte*     iv,
    byte*     encrypted_user_tls_key,
    word32    encrypted_user_tls_key_type)
{
    WOLFSSL_ENTER("tsip_inform_user_keys_ex");
    ForceZero(&g_user_key_info, sizeof(g_user_key_info));
    g_user_key_info.encrypted_provisioning_key = NULL;
    g_user_key_info.iv = NULL;

    if (encrypted_provisioning_key) {
        g_user_key_info.encrypted_provisioning_key = encrypted_provisioning_key;
    }
    if (iv) {
        g_user_key_info.iv = iv;
    }
    if (encrypted_user_tls_key) {
        g_user_key_info.encrypted_user_tls_key = encrypted_user_tls_key;
    }

    g_user_key_info.encrypted_user_tls_key_type = encrypted_user_tls_key_type;
    WOLFSSL_LEAVE("tsip_inform_user_keys_ex", 0);
}
#elif (WOLFSSL_RENESAS_TSIP_VER>=106)
/* inform user key                                                     */
/* the function expects to be called from user application             */
/* user has to create these key information by Renesas tool in advance.*/
void tsip_inform_user_keys(
    byte *encrypted_session_key,
    byte *iv,
    byte *encrypted_user_tls_key)
{
    g_user_key_info.encrypted_session_key = NULL;
    g_user_key_info.iv = NULL;
    g_user_key_info.encrypted_user_tls_key = NULL;

    if (encrypted_session_key) {
        g_user_key_info.encrypted_session_key = encrypted_session_key;
    }
    if (iv) {
        g_user_key_info.iv = iv;
    }
    if (encrypted_user_tls_key) {
        g_user_key_info.encrypted_user_tls_key = encrypted_user_tls_key;
    }
}
#endif

/* Support functions for TSIP TLS Capability */
#if defined(WOLFSSL_RENESAS_TSIP_TLS)

/* to inform ca certificate sign */
/* signature format expects RSA 2048 PSS with SHA256 */
void tsip_inform_cert_sign(const byte *sign)
{
    if (sign)
        ca_cert_sig = sign;
}
/* Sha1Hmac */
int wc_tsip_Sha1HmacGenerate(
        const WOLFSSL *ssl,
        const byte* myInner,
        word32      innerSz,
        const byte* in,
        word32      sz,
        byte*       digest)
{
    WOLFSSL_ENTER("wc_tsip_Sha1HmacGenerate");

    tsip_hmac_sha_handle_t _handle;
    tsip_hmac_sha_key_index_t key_index;
    int ret;

    if ((ssl == NULL) || (myInner == NULL) || (in == NULL) ||
        (digest == NULL)) {
        WOLFSSL_LEAVE("wc_tsip_Sha1HmacGenerate", BAD_FUNC_ARG);
        return BAD_FUNC_ARG;
    }

    if ((ret = tsip_hw_lock()) != 0) {
        WOLFSSL_MSG("hw lock failed");
        WOLFSSL_LEAVE("wc_tsip_Sha1HmacGenerate", ret);
        return ret;
    }

    key_index = ssl->keys.tsip_client_write_MAC_secret;

    ret = R_TSIP_Sha1HmacGenerateInit(
                    &_handle,
                    &key_index);

    if (ret == TSIP_SUCCESS)
        ret = R_TSIP_Sha1HmacGenerateUpdate(
                    &_handle,
                    (uint8_t*)myInner,
                    (uint32_t)innerSz);

    if (ret == TSIP_SUCCESS)
        ret = R_TSIP_Sha1HmacGenerateUpdate(
                    &_handle,
                    (uint8_t*)in,
                    sz);

    if (ret == TSIP_SUCCESS)
        ret = R_TSIP_Sha1HmacGenerateFinal(
                    &_handle,
                    digest);

    tsip_hw_unlock();

    WOLFSSL_LEAVE("wc_tsip_Sha1HmacGenerate", ret);
    return ret;
}


/* Sha256Hmac */
int wc_tsip_Sha256HmacGenerate(
        const WOLFSSL *ssl,
        const byte* myInner,
        word32      innerSz,
        const byte* in,
        word32      sz,
        byte*       digest)
{
    WOLFSSL_ENTER("wc_tsip_Sha256HmacGenerate");

    tsip_hmac_sha_handle_t _handle;
    tsip_hmac_sha_key_index_t key_index;
    int ret;

    if ((ssl == NULL) || (myInner == NULL) || (in == NULL) ||
        (digest == NULL))
      return BAD_FUNC_ARG;

    key_index = ssl->keys.tsip_client_write_MAC_secret;

    if ((ret = tsip_hw_lock()) != 0) {
        WOLFSSL_MSG("hw lock failed");
        return ret;
    }

    ret = R_TSIP_Sha256HmacGenerateInit(
                &_handle,
                &key_index);

    if (ret == TSIP_SUCCESS) {
        ret = R_TSIP_Sha256HmacGenerateUpdate(
                &_handle,
                (uint8_t*)myInner,
                innerSz);
    }
    else {
        WOLFSSL_MSG("R_TSIP_Sha256HmacGenerateInit failed");
    }

    if (ret == TSIP_SUCCESS) {
        ret = R_TSIP_Sha256HmacGenerateUpdate(
                &_handle,
                (uint8_t*)in,
                sz);
    }
    else {
        WOLFSSL_MSG("R_TSIP_Sha256HmacGenerateUpdate: inner failed");
    }
    if (ret == TSIP_SUCCESS) {

        ret = R_TSIP_Sha256HmacGenerateFinal(
                &_handle,
                digest);
    }
    else {
        WOLFSSL_MSG("R_TSIP_Sha256HmacGenerateUpdate: in failed");
    }
    if (ret != TSIP_SUCCESS) {
        WOLFSSL_MSG("R_TSIP_Sha256HmacGenerateFinal failed");
        ret = 1;
    }
    /* unlock hw */
    tsip_hw_unlock();
    WOLFSSL_LEAVE("wc_tsip_Sha256HmacGenerate", ret);
    return ret;
}
/*
 *  Perform SHA1 and SHA256 Hmac verification
 */
int wc_tsip_ShaXHmacVerify(
        const WOLFSSL *ssl,
        const byte* message,
        word32      messageSz,
        word32      macSz,
        word32      content)
{
    WOLFSSL_ENTER("tsip_ShaXHmacVerify");

    tsip_hmac_sha_handle_t    handle;
    tsip_hmac_sha_key_index_t wrapped_key;

    shaHmacInitFn   initFn   = NULL;
    shaHmacUpdateFn updateFn = NULL;
    shaHmacFinalFn  finalFn  = NULL;

    byte   myInner[WOLFSSL_TLS_HMAC_INNER_SZ];
    int ret;

    if ((ssl == NULL) || (message == NULL)) {
        WOLFSSL_LEAVE("tsip_ShaXHmacVerify", BAD_FUNC_ARG);
        return BAD_FUNC_ARG;
    }
    wrapped_key = ssl->keys.tsip_server_write_MAC_secret;

    if (wrapped_key.type == TSIP_KEY_INDEX_TYPE_HMAC_SHA1_FOR_TLS) {
        WOLFSSL_MSG("perform Sha1-Hmac verification");
        initFn   = R_TSIP_Sha1HmacVerifyInit;
        updateFn = R_TSIP_Sha1HmacVerifyUpdate;
        finalFn  = R_TSIP_Sha1HmacVerifyFinal;
    }
    else if (wrapped_key.type == TSIP_KEY_INDEX_TYPE_HMAC_SHA256_FOR_TLS) {
        WOLFSSL_MSG("perform Sha256-Hmac verification");
        initFn   = R_TSIP_Sha256HmacVerifyInit;
        updateFn = R_TSIP_Sha256HmacVerifyUpdate;
        finalFn  = R_TSIP_Sha256HmacVerifyFinal;
    }
    else {
        WOLFSSL_MSG("unsupported key type");
        WOLFSSL_LEAVE("tsip_ShaXHmacVerify", BAD_FUNC_ARG);
        return BAD_FUNC_ARG;
    }

    if ((ret = tsip_hw_lock()) != 0) {
        WOLFSSL_MSG("hw lock failed\n");
        WOLFSSL_LEAVE("tsip_ShaXHmacVerify", ret);
        return ret;
    }

    wolfSSL_SetTlsHmacInner((WOLFSSL*)ssl, (byte*)myInner,
                                                     messageSz, content, 1);

    ret = initFn(&handle, &wrapped_key);

    if (ret == TSIP_SUCCESS) {
        ret = updateFn(&handle, myInner, WOLFSSL_TLS_HMAC_INNER_SZ);
    }
    if (ret == TSIP_SUCCESS) {
        ret = updateFn(&handle, (uint8_t*)message, (uint32_t)messageSz);
    }
    if (ret == TSIP_SUCCESS) {
        ret = finalFn(&handle, (uint8_t*)(message + messageSz), (uint32_t)macSz);
    }
    if (ret != TSIP_SUCCESS) {
        WOLFSSL_MSG("TSIP Mac verification failed");
    }

    /* unlock hw */
    tsip_hw_unlock();
    WOLFSSL_LEAVE("tsip_ShaXHmacVerify", ret);
    return ret;
}

/* generate Verify Data based on master secret */
int wc_tsip_generateVerifyData(
    const byte* ms,                 /* master secret */
    const byte* side,               /* 0:client-side 1:server-side */
    const byte* handshake_hash,
          byte* hashes)             /* out */
{
    int ret ;
    uint32_t l_side = R_TSIP_TLS_GENERATE_CLIENT_VERIFY;

    WOLFSSL_ENTER("tsip_generateVerifyData");

    if ((ms == NULL) || (side == NULL) || (handshake_hash == NULL) ||
        (hashes == NULL)) {
        WOLFSSL_LEAVE("tsip_generateVerifyData", BAD_FUNC_ARG);
        return BAD_FUNC_ARG;
    }
    if (XSTRNCMP((const char*)side, (const char*)kTlsServerFinStr, 
                                                FINISHED_LABEL_SZ) == 0) {
        l_side = R_TSIP_TLS_GENERATE_SERVER_VERIFY;
    }

    if ((ret = tsip_hw_lock()) == 0) {
        ret = R_TSIP_TlsGenerateVerifyData(l_side, (uint32_t*)ms,
                       (uint8_t*)handshake_hash, hashes/* out */);
        if (ret != TSIP_SUCCESS) {
            WOLFSSL_MSG("R_TSIP_TlsGenerateSessionKey failed");
        }
    }
    /* unlock hw */
    tsip_hw_unlock();
    WOLFSSL_LEAVE("tsip_generateVerifyData", ret);
    return ret;
}

/* generate keys for TLS communication */
int wc_tsip_generateSessionKey(
    WOLFSSL *ssl,
    TsipUserCtx*    ctx,
    int             devId)
{
    int ret;
    Ciphers *enc;
    Ciphers *dec;
    tsip_hmac_sha_key_index_t key_client_mac;
    tsip_hmac_sha_key_index_t key_server_mac;
    tsip_aes_key_index_t key_client_aes;
    tsip_aes_key_index_t key_server_aes;

    WOLFSSL_ENTER("wc_tsip_generateSessionKey");

    if (ssl== NULL)
      return BAD_FUNC_ARG;

    if ((ret = tsip_hw_lock()) == 0) {

#if (WOLFSSL_RENESAS_TSIP_VER>=109)

        uint32_t tsipCS = GetTsipCipherSuite(ssl->options.cipherSuite0,
                                             ssl->options.cipherSuite);

        if (tsipCS == R_TSIP_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256 ||
            tsipCS == R_TSIP_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256) {
            WOLFSSL_MSG("Session key for AES-GCM generation skipped.");

            /*  Do not release ssl-> arrays to reference the master secret and
             *  randoms, as the AES GCM session key will be generated in
             *  encryption or description timing.
             */
            wolfSSL_KeepArrays(ssl);
            ret = TSIP_SUCCESS;
        }
        else {

            ret = R_TSIP_TlsGenerateSessionKey(
                    tsipCS,
                    (uint32_t*)ssl->arrays->tsip_masterSecret,
                    (uint8_t*) ssl->arrays->clientRandom,
                    (uint8_t*) ssl->arrays->serverRandom,
                    NULL, /* nonce is required only for AES-GCM key */
                    &key_client_mac,
                    &key_server_mac,
                    &key_client_aes,
                    &key_server_aes,
                    NULL, NULL);
        }
#else /* WOLFSSL_RENESAS_TSIP_VER < 109 */

        ret = R_TSIP_TlsGenerateSessionKey(
                    _tls2tsipdef(ssl->options.cipherSuite),
                    (uint32_t*)ssl->arrays->tsip_masterSecret,
                    (uint8_t*)ssl->arrays->clientRandom,
                    (uint8_t*)ssl->arrays->serverRandom,
                    &key_client_mac,
                    &key_server_mac,
                    &key_client_aes,
                    &key_server_aes,
                    NULL, NULL);
#endif
        if (ret != TSIP_SUCCESS) {
            WOLFSSL_MSG("R_TSIP_TlsGenerateSessionKey failed");
        }
        else {
            /* succeeded creating session keys */
            /* alloc aes instance for both enc and dec */
            enc = &ssl->encrypt;
            dec = &ssl->decrypt;

            if (enc) {
                if (enc->aes == NULL) {
                    enc->aes = (Aes*)XMALLOC(sizeof(Aes), ssl->heap,
                                                    DYNAMIC_TYPE_CIPHER);
                    if (enc->aes == NULL)
                        return MEMORY_E;
                }

                ForceZero(enc->aes, sizeof(Aes));
            }
            if (dec) {
                if (dec->aes == NULL) {
                    dec->aes = (Aes*)XMALLOC(sizeof(Aes), ssl->heap,
                                                    DYNAMIC_TYPE_CIPHER);
                    if (dec->aes == NULL) {
                        if (enc) {
                            XFREE(enc->aes, NULL, DYNAMIC_TYPE_CIPHER);
                        }
                        return MEMORY_E;
                    }
                }

                ForceZero(dec->aes, sizeof(Aes));
            }

            /* copy key index into aes */
            if (ssl->options.side == PROVISION_CLIENT) {
                XMEMCPY(&enc->aes->ctx.tsip_keyIdx, &key_client_aes,
                                                    sizeof(key_client_aes));
                XMEMCPY(&dec->aes->ctx.tsip_keyIdx, &key_server_aes,
                                                    sizeof(key_server_aes));
            }
            else {
                XMEMCPY(&enc->aes->ctx.tsip_keyIdx, &key_server_aes,
                                                    sizeof(key_server_aes));
                XMEMCPY(&dec->aes->ctx.tsip_keyIdx, &key_client_aes,
                                                    sizeof(key_client_aes));
            }

            /* copy hac key index into keys */
            ssl->keys.tsip_client_write_MAC_secret = key_client_mac;
            ssl->keys.tsip_server_write_MAC_secret = key_server_mac;

            /* set up key size and marked ready */
            if (enc) {
                enc->aes->ctx.keySize = ssl->specs.key_size;
                enc->aes->ctx.setup = 1;
                /* ready for use */
                enc->setup = 1;
            }
            /* set up key size and marked ready */
            if (dec) {
                dec->aes->ctx.keySize = ssl->specs.key_size;
                dec->aes->ctx.setup = 1;
                /* ready for use */
                dec->setup = 1;
            }

            if (ctx->tsip_cipher ==
                            R_TSIP_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256 ||
                ctx->tsip_cipher ==
                            R_TSIP_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256) {
                enc->aes->nonceSz = AEAD_MAX_IMP_SZ;
                dec->aes->nonceSz = AEAD_MAX_IMP_SZ;
            }

            enc->aes->devId = devId;
            dec->aes->devId = devId;

            ctx->session_key_set = 1;
        }
        /* unlock hw */
        tsip_hw_unlock();
    }
    else
        WOLFSSL_MSG("hw lock failed");

    WOLFSSL_LEAVE("wc_tsip_generateSessionKey", ret);
    return ret;
}



/* generate Master secrete by TSIP */
#if (WOLFSSL_RENESAS_TSIP_VER>=109)

int wc_tsip_generateMasterSecretEx(
        byte        cipherSuiteFirst,
        byte        cipherSuite,
        const byte *pr, /* pre-master    */
        const byte *cr, /* client random */
        const byte *sr, /* server random */
        byte *ms)
{
    int ret;

    WOLFSSL_ENTER("tsip_generateMasterSecretEx");

    if ((pr == NULL) || (cr == NULL) || (sr == NULL) ||
        (ms == NULL))
      return BAD_FUNC_ARG;

    uint32_t tsipCS = GetTsipCipherSuite(cipherSuiteFirst,cipherSuite);
    if (tsipCS == 0xffffffff)
        return BAD_FUNC_ARG;

    if ((ret = tsip_hw_lock()) == 0) {
        ret = R_TSIP_TlsGenerateMasterSecret(
            tsipCS,
            (uint32_t*)pr,
            (uint8_t*)cr, (uint8_t*)sr, (uint32_t*)ms);
        if (ret != TSIP_SUCCESS) {
            WOLFSSL_MSG("R_TSIP_TlsGenerateMasterSecret failed");
        }
        /* unlock hw */
        tsip_hw_unlock();
    }
    else {
        WOLFSSL_MSG(" hw lock failed ");
    }
    WOLFSSL_LEAVE("tsip_generateMasterSecretEx", ret);
    return ret;
}

#else /* WOLFSSL_RENESAS_TSIP_VER < 109 */

int wc_tsip_generateMasterSecret(
        const byte* pr, /* pre-master    */
        const byte* cr, /* client random */
        const byte* sr, /* server random */
        byte*       ms)
{
    int ret;
    WOLFSSL_ENTER("tsip_generateMasterSecret");
    if ((pr == NULL) || (cr == NULL) || (sr == NULL) ||
        (ms == NULL))
      return BAD_FUNC_ARG;

    if ((ret = tsip_hw_lock()) == 0) {
        ret = R_TSIP_TlsGenerateMasterSecret(
                (uint32_t*)pr,
                (uint8_t*)cr,
                (uint8_t*)sr,
                (uint32_t*)ms);

        if (ret != TSIP_SUCCESS) {
            WOLFSSL_MSG("R_TSIP_TlsGenerateMasterSecret failed");
        }
        /* unlock hw */
        tsip_hw_unlock();
    }
    else {
        WOLFSSL_MSG(" hw lock failed ");
    }
    WOLFSSL_LEAVE("tsip_generateMasterSecret", ret);
    return ret;
}
#endif /* WOLFSSL_RENESAS_TSIP_VER */

/*  store elements for session key generation into ssl->keys.
 *  return 0 on success, negative value on failure
 */
int wc_tsip_storeKeyCtx(WOLFSSL* ssl, TsipUserCtx* userCtx)
{
    int ret = 0;

    WOLFSSL_ENTER("tsip_storeKeyCtx");

    if (ssl == NULL || userCtx == NULL)
        ret = BAD_FUNC_ARG;

    if (ret == 0) {
        XMEMCPY(userCtx->tsip_masterSecret, ssl->arrays->tsip_masterSecret,
                                                TSIP_TLS_MASTERSECRET_SIZE);
        XMEMCPY(userCtx->tsip_clientRandom, ssl->arrays->clientRandom,
                                                TSIP_TLS_CLIENTRANDOM_SZ);
        XMEMCPY(userCtx->tsip_serverRandom, ssl->arrays->serverRandom,
                                                TSIP_TLS_SERVERRANDOM_SZ);
        userCtx->tsip_cipher = GetTsipCipherSuite(ssl->options.cipherSuite0,
                                                ssl->options.cipherSuite);
    }

    WOLFSSL_LEAVE("tsip_storeKeyCtx", ret);
    return ret;
}

/* generate pre-Master secrete by TSIP */
int wc_tsip_generatePremasterSecret(byte *premaster, word32 preSz)
{
    WOLFSSL_ENTER("tsip_generatePremasterSecret");
    int ret;

    if (premaster == NULL)
      return BAD_FUNC_ARG;

    if ((ret = tsip_hw_lock()) == 0 && preSz >=
                                    (R_TSIP_TLS_MASTER_SECRET_WORD_SIZE*4)) {
        /* generate pre-master, 80 bytes */
        ret = R_TSIP_TlsGeneratePreMasterSecret((uint32_t*)premaster);
        if (ret != TSIP_SUCCESS) {
            WOLFSSL_MSG(" R_TSIP_TlsGeneratePreMasterSecret failed");
        }

        /* unlock hw */
        tsip_hw_unlock();
    }
    else {
        WOLFSSL_MSG(" hw lock failed or preSz is smaller than 80");
    }
    WOLFSSL_LEAVE("tsip_generatePremasterSecret", ret);
    return ret;
}

/*
* generate encrypted pre-Master secrete by TSIP
*/
int wc_tsip_generateEncryptPreMasterSecret(
        WOLFSSL*    ssl,
        byte*       out,
        word32*     outSz)
{
    int ret;

    WOLFSSL_ENTER("tsip_generateEncryptPreMasterSecret");

    if ((ssl == NULL) || (out == NULL) || (outSz == NULL))
      return BAD_FUNC_ARG;

    if ((ret = tsip_hw_lock()) == 0) {
        if (*outSz >= 256)

            #if  (WOLFSSL_RENESAS_TSIP_VER>=109)

            ret = R_TSIP_TlsEncryptPreMasterSecretWithRsa2048PublicKey(
                        (uint32_t*)ssl->peerSceTsipEncRsaKeyIndex,
                        (uint32_t*)ssl->arrays->preMasterSecret,
                        (uint8_t*)out);

            #else

            ret = R_TSIP_TlsEncryptPreMasterSecret(
                          (uint32_t*)ssl->peerSceTsipEncRsaKeyIndex,
                          (uint32_t*)ssl->arrays->preMasterSecret,
                          (uint8_t*)out);

            #endif
        else
            ret = -1;

        if (ret != TSIP_SUCCESS) {
            WOLFSSL_MSG(" R_TSIP_TlsEncryptPreMasterSecret failed");
        }
        else {
            *outSz = 256; /* TSIP can only handles 2048 RSA */
            void* ctx = wolfSSL_GetRsaVerifyCtx(ssl);
            wolfSSL_CTX_SetGenMasterSecretCb(ssl->ctx,
                                                Renesas_cmn_genMasterSecret);
            wolfSSL_SetGenMasterSecretCtx(ssl, ctx);
        }

        tsip_hw_unlock();

    }
    else {
        WOLFSSL_MSG(" hw lock failed ");
    }
    WOLFSSL_LEAVE("tsip_generateEncryptPreMasterSecret", ret);
    return ret;
}


/* Certificate verification by TSIP */
int wc_tsip_tls_CertVerify(
        const uint8_t* cert,       uint32_t certSz,
        const uint8_t* signature,  uint32_t sigSz,
        uint32_t      key_n_start, uint32_t key_n_len,
        uint32_t      key_e_start, uint32_t key_e_len,
        uint8_t*      tsip_encRsaKeyIndex)
{
    int ret;
    uint8_t *sigforSCE = NULL;
    uint8_t *pSig = NULL;
    const byte rs_size = 0x20;
    byte offset = 0x3;

    WOLFSSL_ENTER("wc_tsip_tls_CertVerify");

    if (cert == NULL)
      return BAD_FUNC_ARG;

    if (!signature) {
        WOLFSSL_MSG(" signature for ca verification is not set");
        return -1;
    }
    if (!tsip_encRsaKeyIndex) {
        WOLFSSL_MSG(" tsip_encRsaKeyIndex is NULL.");
        return -1;
    }

    /* Public key type: Prime256r1 */
    if (g_user_key_info.encrypted_user_tls_key_type ==
                                    R_TSIP_TLS_PUBLIC_KEY_TYPE_ECDSA_P256) {

        if ((sigforSCE = (uint8_t*)XMALLOC(R_TSIP_ECDSA_DATA_BYTE_SIZE,
                                        NULL, DYNAMIC_TYPE_ECC)) == NULL) {
            WOLFSSL_MSG("failed to malloc memory");
            return MEMORY_E;
        }
        /* initialization */
        ForceZero(sigforSCE, R_TSIP_ECDSA_DATA_BYTE_SIZE);

        if (signature[offset] == 0x20) {
            XMEMCPY(sigforSCE, &signature[offset+1], rs_size);

            offset = 0x25;
            if (signature[offset] == 0x20) {
                XMEMCPY(&sigforSCE[rs_size], &signature[offset+1], rs_size);
            }
            else {
                XMEMCPY(&sigforSCE[rs_size], &signature[offset+2], rs_size);
            }
        }
        else {
            XMEMCPY(sigforSCE, &signature[offset+2], rs_size);
            offset = 0x26;

            if (signature[offset] == rs_size) {
                XMEMCPY(&sigforSCE[rs_size], &signature[offset+1], rs_size);
            }
            else {
                XMEMCPY(&sigforSCE[rs_size], &signature[offset+2], rs_size);
            }
        }
        pSig = sigforSCE;
    }
    /* Public key type: RSA 2048bit */
    else {
        pSig = (uint8_t*)signature;
    }

    if ((ret = tsip_hw_lock()) == 0) {

        #if (WOLFSSL_RENESAS_TSIP_VER>=109)

        ret = R_TSIP_TlsCertificateVerification(
            g_user_key_info.encrypted_user_tls_key_type,
            (uint32_t*)g_encrypted_publicCA_key,/* encrypted public key  */
            (uint8_t*)cert,                    /* certificate der        */
            certSz,                            /* length of der          */
            (uint8_t*)pSig,                    /* sign data by RSA PSS   */
            key_n_start,  /* start position of public key n in bytes     */
            (key_n_start + key_n_len),     /* length of the public key n */
            key_e_start,                   /* start pos, key e in bytes  */
            (key_e_start + key_e_len),     /* length of the public key e */
            (uint32_t*)tsip_encRsaKeyIndex /* returned encrypted key     */
        );

        #elif (WOLFSSL_RENESAS_TSIP_VER>=106)

        ret = R_TSIP_TlsCertificateVerification(
            (uint32_t*)g_encrypted_publicCA_key,/* encrypted public key  */
            (uint8_t*)cert,                    /* certificate der        */
            certSz,                            /* length of der          */
            (uint8_t*)pSig,                    /* sign data by RSA PSS   */
            key_n_start,  /* start position of public key n in bytes     */
            (key_n_start + key_n_len),     /* length of the public key n */
            key_e_start,                   /* start pos, key e in bytes  */
            (key_e_start + key_e_len),     /* length of the public key e */
            (uint32_t*)tsip_encRsaKeyIndex /* returned encrypted key     */
        );
        #endif

        if (ret != TSIP_SUCCESS) {
            WOLFSSL_MSG(" R_TSIP_TlsCertificateVerification failed");
        }
        if (sigforSCE) {
            XFREE(sigforSCE, NULL, DYNAMIC_TYPE_ECC);
        }
        tsip_hw_unlock();
    }
    else {
        WOLFSSL_MSG(" hw lock failed ");
    }
    WOLFSSL_LEAVE("wc_tsip_tls_CertVerify", ret);
    return ret;
}
/* Root Certificate verification */
int wc_tsip_tls_RootCertVerify(
        const byte* cert,           word32 cert_len,
        word32      key_n_start,    word32 key_n_len,
        word32      key_e_start,    word32 key_e_len,
        word32      cm_row)
{
    int ret;
    /* call to generate encrypted public key for certificate verification */
    uint8_t *signature = (uint8_t*)ca_cert_sig;

    WOLFSSL_ENTER("wc_tsip_tls_RootCertVerify");

    if (cert == NULL)
      return BAD_FUNC_ARG;

    if (!signature) {
        WOLFSSL_MSG(" signature for ca verification is not set");
        return -1;
    }

    if ((ret = tsip_hw_lock()) == 0) {

    #if (WOLFSSL_RENESAS_TSIP_VER>=109)
        ret = R_TSIP_TlsRootCertificateVerification(
            g_user_key_info.encrypted_user_tls_key_type,
            (uint8_t*)cert,             /* CA cert */
            (uint32_t)cert_len,         /* length of CA cert */
            key_n_start,                /* Byte position of public key */
            (key_n_start + key_n_len),
            key_e_start,
            (key_e_start + key_e_len),
            (uint8_t*)ca_cert_sig,      /* RSA 2048 PSS with SHA256 */
            g_encrypted_publicCA_key    /* RSA-2048 public key 560 bytes */
        );
    #else /* WOLFSSL_RENESAS_TSIP_VER < 109 */
        ret = R_TSIP_TlsRootCertificateVerification(
            (uint8_t*)cert,/* CA cert */
            (uint32_t)cert_len,/* length of CA cert */
            key_n_start, /* Byte position of public key */
            (key_n_start + key_n_len),
            key_e_start,
            (key_e_start + key_e_len),
            (uint8_t*)ca_cert_sig,/* "RSA 2048 PSS with SHA256" */
            /* RSA-2048 public key used by RSA-2048 PSS with SHA256. 560 Bytes */
            g_encrypted_publicCA_key
        );
    #endif

        if (ret != TSIP_SUCCESS) {
            WOLFSSL_MSG(" R_TSIP_TlsRootCertificateVerification failed");
        }
        else {
            g_CAscm_Idx = cm_row;
        }

        tsip_hw_unlock();
    }
    else {
        WOLFSSL_MSG(" hw lock failed ");
    }
    WOLFSSL_LEAVE("wc_tsip_tls_RootCertVerify", ret);
    return ret;
}
#endif /* WOLFSSL_RENESAS_TSIP_TLS */
#if !defined(NO_RSA)
/*  Perform signing with the client's RSA private key on hash value of messages
 *  exchanged with server.
 *
 * parameters
 *   info->pk.rsa.in    : not used
 *   info->pk.rsa.inlen : not used
 *   info->pk.rsa.out   : the buffer where the signature data is output to
 *   info->pk.rsa.outlen: the length of the pk.rsa.out
 *   tuc:  the pointer to the TsipUserCtx structure
 * returns
 *   0 on success, CRYPTOCB_UNAVAILABLE on unsupported key type specified.
 *
 */
WOLFSSL_LOCAL int tsip_SignRsaPkcs(wc_CryptoInfo* info, TsipUserCtx* tuc)
{
    int ret = 0;
    e_tsip_err_t    err = TSIP_SUCCESS;
    tsip_rsa_byte_data_t hashData, sigData;
    WOLFSSL* ssl = NULL;
    uint8_t  tsip_hash_type;

    WOLFSSL_ENTER("tsip_SignRsaPkcs");

    if (info == NULL || tuc == NULL 
    #ifndef WOLFSSL_RENESAS_TSIP_CRYPTONLY
    || tuc->ssl == NULL
    #endif
    ) {
            ret = BAD_FUNC_ARG;
    }
    
#ifdef WOLFSSL_RENESAS_TSIP_TLS
    if (ret == 0) {
        ssl = tuc->ssl;

        if (ssl->version.major == SSLv3_MAJOR &&
            ssl->version.minor == TLSv1_3_MINOR) {
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    if (ret == 0) {
        /* import private key_index from wrapped key */
        ret = tsipImportPrivateKey(tuc, tuc->wrappedKeyType);
    }

    
    if (ret == 0) {
        if (ssl->options.hashAlgo == md5_mac)
            tsip_hash_type = R_TSIP_RSA_HASH_MD5;
        else if (ssl->options.hashAlgo == sha_mac)
            tsip_hash_type = R_TSIP_RSA_HASH_SHA1;
        else if (ssl->options.hashAlgo == sha256_mac)
            tsip_hash_type = R_TSIP_RSA_HASH_SHA256;
        else
            ret = CRYPTOCB_UNAVAILABLE;
    }
#else
    (void)ssl;
    
    if (ret == 0) {
       if (tuc->sing_hash_type == md5_mac)
           tsip_hash_type = R_TSIP_RSA_HASH_MD5;
       else if (tuc->sing_hash_type == sha_mac)
           tsip_hash_type = R_TSIP_RSA_HASH_SHA1;
       else if (tuc->sing_hash_type == sha256_mac)
           tsip_hash_type = R_TSIP_RSA_HASH_SHA256;
       else
           ret = CRYPTOCB_UNAVAILABLE;
    }
    
    switch (tuc->wrappedKeyType) {
        case TSIP_KEY_TYPE_RSA1024:
            if (tuc->keyflgs_crypt.bits.rsapri1024_key_set != 1)
            {
                WOLFSSL_MSG("tsip rsa private key 2048 not set");
                    ret = CRYPTOCB_UNAVAILABLE;
            }
            break;
        case TSIP_KEY_TYPE_RSA2048:
            if (tuc->keyflgs_crypt.bits.rsapri2048_key_set != 1)
            {
                WOLFSSL_MSG("tsip rsa private key 2048 not set");
                    ret = CRYPTOCB_UNAVAILABLE;
            }
            break;
        default:
            WOLFSSL_MSG("wrapped private key is not supported");
            ret = CRYPTOCB_UNAVAILABLE;
            break;
    }
    
#endif
    
    if (ret == 0) {
       #ifdef WOLFSSL_RENESAS_TSIP_TLS
        hashData.pdata      = (uint8_t*)ssl->buffers.digest.buffer;
        hashData.data_type  = 1;
        sigData.pdata       = (uint8_t*)info->pk.rsa.in;
        sigData.data_length = 0; /* signature size will be returned here */
       #else
        hashData.pdata      = (uint8_t*)info->pk.rsa.in;
        hashData.data_length= info->pk.rsa.inLen;
        hashData.data_type  = tuc->keyflgs_crypt.bits.message_type;
        sigData.pdata       = (uint8_t*)info->pk.rsa.out;
        sigData.data_length = 0;
       #endif
        if ((ret = tsip_hw_lock()) == 0) {
            switch (tuc->wrappedKeyType) {
            #ifdef WOLFSSL_RENESAS_TSIP_CRYPTONLY
                case TSIP_KEY_TYPE_RSA1024:
                    err = R_TSIP_RsassaPkcs1024SignatureGenerate(
                                                &hashData, &sigData,
                                       (tsip_rsa1024_private_key_index_t*)
                                                tuc->rsa1024pri_keyIdx,
                                                tsip_hash_type);

                    if (err != TSIP_SUCCESS) {
                        ret = WC_HW_E; 
                    }
                    break;
            #endif
                case TSIP_KEY_TYPE_RSA2048:

                    err = R_TSIP_RsassaPkcs2048SignatureGenerate(
                                                &hashData, &sigData,
                                   #ifdef WOLFSSL_RENESAS_TSIP_TLS
                                                &tuc->Rsa2048PrivateKeyIdx,
                                   #else
                                       (tsip_rsa2048_private_key_index_t*)
                                                tuc->rsa2048pri_keyIdx,
                                   #endif
                                                tsip_hash_type);

                    if (err != TSIP_SUCCESS) {
                        ret = WC_HW_E; 
                    }
                    break;

                case TSIP_KEY_TYPE_RSA4096:
                    ret = CRYPTOCB_UNAVAILABLE;
                    break;

                default:
                    WOLFSSL_MSG("wrapped private key is not supported");
                    ret = CRYPTOCB_UNAVAILABLE;
                    break;
            }

            tsip_hw_unlock();
        }
        else {
            WOLFSSL_MSG("mutex locking error");
        }
    }

    WOLFSSL_LEAVE("tsip_SignRsaPkcs", ret);
    return ret;
}
#endif /* !NO_RSA */

#if !defined(NO_RSA) && defined(WOLFSSL_RENESAS_TSIP_TLS)
WOLFSSL_LOCAL int tsip_VerifyRsaPkcsCb(
                        WOLFSSL* ssl, 
                        unsigned char* sig, unsigned int sigSz,
                        unsigned char** out,
                        const unsigned char* keyDer, unsigned int keySz,
                        void* ctx)
{
    int ret = 0;
    e_tsip_err_t    err = TSIP_SUCCESS;
    tsip_rsa_byte_data_t hashData, sigData;
    TsipUserCtx* tuc = NULL;
    uint8_t  tsip_hash_type;
    (void)keyDer;
    (void)keySz;

    WOLFSSL_ENTER("tsip_VerifyRsaPkcsCb");

    if (sig == NULL || out == NULL || ctx == NULL) {
            ret = BAD_FUNC_ARG;
    }

    if (ssl->version.major == SSLv3_MAJOR &&
        ssl->version.minor == TLSv1_3_MINOR) {
        ret = CRYPTOCB_UNAVAILABLE;
    }

    if (ret == 0) {
        tuc = (TsipUserCtx*)ctx;
        if (tuc == NULL)
            ret = CRYPTOCB_UNAVAILABLE;
    }

    if (ret == 0) {
        /* import public key_index from wrapped key */
        ret = tsipImportPublicKey(tuc, tuc->wrappedKeyType);
    }
    
    if (ret == 0) {
        if (ssl->options.hashAlgo == md5_mac)
            tsip_hash_type = R_TSIP_RSA_HASH_MD5;
        else if (ssl->options.hashAlgo == sha_mac)
            tsip_hash_type = R_TSIP_RSA_HASH_SHA1;
        else if (ssl->options.hashAlgo == sha256_mac)
            tsip_hash_type = R_TSIP_RSA_HASH_SHA256;
        else {
            ret = CRYPTOCB_UNAVAILABLE;
        } 
    }

    if (ret == 0) {
        sigData.pdata       = (uint8_t*)sig;
        hashData.pdata      = (uint8_t*)ssl->buffers.digest.buffer;
        hashData.data_type  = 1;  /* hash value */

        if ((ret = tsip_hw_lock()) == 0) {

            switch (tuc->wrappedKeyType) {
                
                case TSIP_KEY_TYPE_RSA2048:
                    sigData.data_length = 256;
                    err = R_TSIP_RsassaPkcs2048SignatureVerification(
                                                &sigData, &hashData,
                                                &tuc->Rsa2048PublicKeyIdx,
                                                tsip_hash_type);

                    if (err == TSIP_ERR_AUTHENTICATION) {
                        ret = VERIFY_CERT_ERROR;
                    }
                    else if (err == TSIP_SUCCESS) {
                        ret = 0;
                    }
                    else {
                        ret = WC_HW_E;
                    }
                    break;

                case TSIP_KEY_TYPE_RSA4096:
                    ret = CRYPTOCB_UNAVAILABLE;
                    break;

                default:
                    WOLFSSL_MSG("wrapped private key is not supported");
                    ret = CRYPTOCB_UNAVAILABLE;
                    break;
            }
            tsip_hw_unlock();
        }
        else {
            WOLFSSL_MSG("mutex locking error");
        }
    }
    WOLFSSL_LEAVE("tsip_VerifyRsaPkcsCb", ret);
    return ret;
}
#endif /* !NO_RSA && TSIP_TLS */

#if defined(HAVE_ECC) && defined(WOLFSSL_RENESAS_TSIP_TLS)
/*   Perform signing with the client's ECC private key on hash value of messages
 *   exchanged with server. 
 *
 * parameters
 *   info->pk.eccsign.in    : the buffer holding hash value of messages
 *   info->pk.eccsign.inlen : hash data size
 *   info->pk.eccsign.out   : the buffer where the signature data is output to
 *   info->pk.eccsign.outlen: the length of the buffer pk.eccsign.out
 *   tuc:  the pointer to the TsipUserCtx structure
 * returns
 *   0 on success, CRYPTOCB_UNAVAILABLE on unsupported key type specified.
 * note
 *   signature will be DER encoded and stored into out buffer.
 *   the private key must be imported as TSIP specific format.
 */
WOLFSSL_LOCAL int tsip_SignEcdsa(wc_CryptoInfo* info, TsipUserCtx* tuc)
{
    int ret = 0;
    e_tsip_err_t    err = TSIP_SUCCESS;
    tsip_rsa_byte_data_t hashData, sigData;
    byte  offsetForWork;
    byte* out = NULL;
    byte* sig = NULL;
    int   rSz = 0;
    int   sSz = 0;
    int   idx = 0;
    int   sz = 0;
    WOLFSSL* ssl = NULL;

    WOLFSSL_ENTER("tsip_SignEcdsa");

    if (info == NULL || tuc == NULL) {
        ret = CRYPTOCB_UNAVAILABLE;
    }

    if (ret == 0) {
        ssl = tuc->ssl;

        if (ssl->version.major == SSLv3_MAJOR &&
            ssl->version.minor == TLSv1_3_MINOR) {
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    if (ret == 0) {
        /* import private key_index from wrapped key */
        ret = tsipImportPrivateKey(tuc, tuc->wrappedKeyType);
    }
 
    if (ret == 0) {
        hashData.pdata      = (uint8_t*)info->pk.eccsign.in;
        hashData.data_type  = 1;
        sigData.pdata       = (uint8_t*)info->pk.eccsign.out;
        sigData.data_length = 0; /* signature size will be returned here */

        if ((ret = tsip_hw_lock()) == 0) {        
            switch (tuc->wrappedKeyType) {

                #if defined(HAVE_ECC)
                case TSIP_KEY_TYPE_ECDSAP256:
                    offsetForWork = R_TSIP_ECDSA_DATA_BYTE_SIZE + 32;
                    if (*(info->pk.eccsign.outlen) < 
                                R_TSIP_ECDSA_DATA_BYTE_SIZE + offsetForWork) {
                        ret = BUFFER_E;
                        break;
                    }

                    sigData.pdata = (uint8_t*)info->pk.eccsign.out + 
                                                            offsetForWork;
                    err = R_TSIP_EcdsaP256SignatureGenerate(
                                                &hashData, &sigData,
                                                &tuc->EcdsaP256PrivateKeyIdx);
                    if (err != TSIP_SUCCESS) {
                        ret = WC_HW_E;
                        break; 
                    }

                    out = info->pk.eccsign.out;
                    sig = sigData.pdata;

                    rSz =  sSz = R_TSIP_ECDSA_DATA_BYTE_SIZE / 2;
                    rSz += (sig[0] & 0x80)?1:0;
                    sSz += (sig[sSz] & 0x80)?1:0;
                    sz  =  (ASN_TAG_SZ + 1) * 2 + rSz + sSz;

                    /* encode ASN sequence */
                    out[idx++] = ASN_SEQUENCE | ASN_CONSTRUCTED;
                    out[idx++] = sz;
                    
                    /* copy r part */
                    out[idx++] = ASN_INTEGER;
                    out[idx++] = rSz;
                    if (rSz > R_TSIP_ECDSA_DATA_BYTE_SIZE / 2)
                        out[idx++] = 0x00;
                    XMEMCPY(&out[idx], sig, R_TSIP_ECDSA_DATA_BYTE_SIZE / 2);
                    idx += R_TSIP_ECDSA_DATA_BYTE_SIZE / 2;
                    sig += R_TSIP_ECDSA_DATA_BYTE_SIZE / 2;
                    /* copy s part */
                    out[idx++] = ASN_INTEGER;
                    out[idx++] = sSz;
                    if (sSz > R_TSIP_ECDSA_DATA_BYTE_SIZE / 2)
                        out[idx++] = 0x00;
                    XMEMCPY(&out[idx], sig, R_TSIP_ECDSA_DATA_BYTE_SIZE / 2);
                
                    /* out size */
                    *(info->pk.eccsign.outlen) = ASN_TAG_SZ + 1 + sz;
                    break;
                #endif

                #if defined(HAVE_ECC192)
                case TSIP_KEY_TYPE_ECDSAP192:
                    ret = CRYPTOCB_UNAVAILABLE;
                    break;
                #endif
                #if defined(HAVE_ECC224)
                case TSIP_KEY_TYPE_ECDSAP224:
                    ret = CRYPTOCB_UNAVAILABLE;
                    break;
                #endif
                #if defined(HAVE_ECC384)
                case TSIP_KEY_TYPE_ECDSAP384:
                    ret = CRYPTOCB_UNAVAILABLE;
                    break;
                #endif

                default:
                    WOLFSSL_MSG("wrapped private key is not supported");
                    ret = CRYPTOCB_UNAVAILABLE;
                    break;
            }
            tsip_hw_unlock();
        }
        else {
            WOLFSSL_MSG("mutex locking error");
        }
    }
    WOLFSSL_LEAVE("tsip_SignEcdsa", ret);
    return ret;
}
#endif /* HAVE_ECC && TSIP_TLS */


#ifdef WOLFSSL_RENESAS_TSIP_CRYPT_DEBUG

/* err
 * e_tsip_err
    TSIP_SUCCESS = 0,
    TSIP_ERR_SELF_CHECK1,  // Self-check 1 fail or TSIP function internal err.
    TSIP_ERR_RESOURCE_CONFLICT, // A resource conflict occurred.
    TSIP_ERR_SELF_CHECK2,       // Self-check 2 fail.
    TSIP_ERR_KEY_SET,           // setting the invalid key.
    TSIP_ERR_AUTHENTICATION,    // Authentication failed.
    TSIP_ERR_CALLBACK_UNREGIST, // Callback function is not registered.
    TSIP_ERR_PARAMETER,         // Illegal Input data.
    TSIP_ERR_PROHIBIT_FUNCTION, // An invalid function call occurred.
 *  TSIP_RESUME_FIRMWARE_GENERATE_MAC,
                  // There is a continuation of R_TSIP_GenerateFirmwareMAC.
*/

static void hexdump(const uint8_t* in, uint32_t len)
{
    uint32_t i;

    if (in == NULL)
        return;

    for (i = 0; i <= len;i++, in++) {
        printf("%02x:", *in);
        if (((i+1)%16)==0) {
            printf("\n");
        }
    }
    printf("\n");
}

byte *ret2err(word32 ret)
{
    switch(ret) {
        case TSIP_SUCCESS:     return "success";
        case TSIP_ERR_SELF_CHECK1: return "selfcheck1";
        case TSIP_ERR_RESOURCE_CONFLICT: return "rsconflict";
        case TSIP_ERR_SELF_CHECK2: return "selfcheck2";
        case TSIP_ERR_KEY_SET: return "keyset";
        case TSIP_ERR_AUTHENTICATION: return "authentication";
        case TSIP_ERR_CALLBACK_UNREGIST: return "callback unreg";
        case TSIP_ERR_PARAMETER: return "badarg";
        case TSIP_ERR_PROHIBIT_FUNCTION: return "prohibitfunc";
        case TSIP_RESUME_FIRMWARE_GENERATE_MAC: return "conti-generate-mac";
        default:return "unknown";
    }
}

#endif /* WOLFSSL_RENESAS_TSIP_CRYPT_DEBUG */
#endif /* WOLFSSL_RENESAS_TSIP */
