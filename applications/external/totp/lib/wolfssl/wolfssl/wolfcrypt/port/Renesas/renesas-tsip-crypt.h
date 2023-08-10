/* renesas-tsip-crypt.h
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
#ifndef __RENESAS_TSIP_CRYPT_H__
#define __RENESAS_TSIP_CRYPT_H__

#if !defined(WOLFCRYPT_ONLY) || \
    defined(WOLFSSL_RENESAS_TSIP_CRYPTONLY)

#if defined(WOLFSSL_RENESAS_TSIP_IAREWRX)
    #include "r_bsp/mcu/all/r_rx_compiler.h"
    #include "r_bsp/platform.h"
    #include "r_tsip_rx_if.h"
#endif

#if defined(WOLFSSL_RENESAS_TSIP) || \
    defined(WOLFSSL_RENESAS_TSIP_CRYPTONLY)
    #include "r_tsip_rx_if.h"    
#endif


#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/ssl.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TSIP_SESSIONKEY_NONCE_SIZE      8

#define tsip_Sha256HmacVerify tsip_ShaXHmacVerify /* for backward compat */
#define sce_tsip_checkCA tsip_checkCA

typedef enum {
    WOLFSSL_TSIP_NOERROR = 0,
    WOLFSSL_TSIP_ILLEGAL_CIPHERSUITE = 0xffffffff,
}wolfssl_tsip_error_number;

typedef enum {
    tsip_Key_SESSION = 1,
    tsip_Key_AES128  = 2,
    tsip_Key_AES256  = 3,
    tsip_Key_RSA1024 = 4,
    tsip_Key_RSA2048 = 5,
    tsip_Key_tls_Rsa2048 = 6,
    tsip_Key_unknown = -1,
} wolfssl_TSIP_KEY_IV;

enum {
    l_TLS_RSA_WITH_AES_128_CBC_SHA            = 0x2F,
    l_TLS_RSA_WITH_AES_128_CBC_SHA256         = 0x3c,
    l_TLS_RSA_WITH_AES_256_CBC_SHA            = 0x35,
    l_TLS_RSA_WITH_AES_256_CBC_SHA256         = 0x3d,
    l_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256 = 0x23,
    l_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256   = 0x27,
    l_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256 = 0x2b,
    l_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256   = 0x2f,
    l_TLS_AES_128_GCM_SHA256                  = 0x01,
    l_TLS_AES_128_CCM_SHA256                  = 0x04,
};

enum {
    ENCRYPTED_ECDHE_PUBKEY_SZ  = 96,
    ECCP256_PUBKEY_SZ          = 64,
    TSIP_TLS_CLIENTRANDOM_SZ   = 32,
    TSIP_TLS_SERVERRANDOM_SZ   = 32,
    TSIP_TLS_VERIFY_DATA_WD_SZ = 8,
    TSIP_TLS_MAX_SIGDATA_SZ    = 130,
    TSIP_TEMP_WORK_SIZE        = 128,
};

typedef enum {
    TSIP_KEY_TYPE_RSA2048      = 0,
    TSIP_KEY_TYPE_RSA4096      = 1,
    TSIP_KEY_TYPE_ECDSAP256    = 2,
    #ifdef WOLFSSL_RENESAS_TSIP_CRYPTONLY
    TSIP_KEY_TYPE_RSA1024      = 3,
    #endif
    
} wolfssl_TSIP_KEY_TYPE;

struct WOLFSSL;
struct KeyShareEntry;

/*  MsgBag stands for message bag and acts as a buffer for holding plain text
 *  handshake messages exchanged between client and server.
 *  MsgBag was introduced as a workaround for the TSIP's limitation that TSIP 
 *  can not process multiple hash algorithms at the same time. If the
 *  limitation is resolved in a future TSIP, MsgBag should be removed.    
 *  The contents in this MsgBag is used for transcript hashing. The hash value
 *  is used for the key derivation and Finished-message. 
 *  The capacity of the MsgBag is defined as MSGBAG_SIZE and the actual
 *  size is 8KB. The size should be large enough to hold all the handshake
 *  messages including the server and client certificate messages.
 */
#define MSGBAG_SIZE (1024 * 8)
#define MAX_MSGBAG_MESSAGES 10
typedef struct MsgBag
{
    int   msgIdx;
    int   buffIdx;
    byte  msgTypes[MAX_MSGBAG_MESSAGES];
    byte  buff[MSGBAG_SIZE];
} MsgBag;

#ifdef WOLFSSL_RENESAS_TSIP_CRYPTONLY
 
 typedef void* renesas_tsip_key;
 
 /* flags Crypt Only */
 struct tsip_keyflgs_cryt {
    uint8_t aes256_key_set:1;
    uint8_t aes128_key_set:1;
    uint8_t rsapri2048_key_set:1;
    uint8_t rsapub2048_key_set:1;
    uint8_t rsapri1024_key_set:1;
    uint8_t rsapub1024_key_set:1;
    uint8_t message_type:1;/*message 0, hashed 1*/
 };
#endif
/*
 * TsipUserCtx holds mainly keys used for TLS handshake in TSIP specific format.
 */
typedef struct TsipUserCtx {
    /* unique number for each session */
    int devId;
#ifdef WOLFSSL_RENESAS_TSIP_TLS
    /* 0:working as a TLS client, 1: as a server */
    byte                    side;

    /* public key index for verification of RootCA cert */
    uint32_t                user_key_id;
    
    /* WOLFSSL object associated with */
    struct WOLFSSL*         ssl;
    struct WOLFSSL_CTX*     ctx;

    /* HEAP_HINT */
    void*                   heap;

    /* TLSv1.3 handshake related members, mainly keys */

    /* handle is used as work area for Tls13 handshake */
    tsip_tls13_handle_t                                handle13;
#endif /* WOLFSSL_RENESAS_TSIP_TLS */
    /* client key pair wrapped by provisioning key */
    byte*                                              wrappedPrivateKey;
    byte*                                              wrappedPublicKey;

    int                                                wrappedKeyType;
#ifdef WOLFSSL_RENESAS_TSIP_TLS
    #if !defined(NO_RSA)
    /* RSA-2048bit private and public key-index for client authentication */
    tsip_rsa2048_private_key_index_t                   Rsa2048PrivateKeyIdx;
    tsip_rsa2048_public_key_index_t                    Rsa2048PublicKeyIdx;
    #endif /* !NO_RSA */

    #if defined(HAVE_ECC)
    /* ECC P256 private and public key-index for client authentication */
    tsip_ecc_private_key_index_t                       EcdsaP256PrivateKeyIdx;
    tsip_ecc_public_key_index_t                        EcdsaP256PublicKeyIdx;
    #endif /* HAVE_ECC */

    /* ECDHE private key index for Tls13 handshake */
    tsip_tls_p256_ecc_key_index_t                      EcdhPrivKey13Idx;

    /* ECDHE pre-master secret */
    tsip_tls13_ephemeral_shared_secret_key_index_t     sharedSecret13Idx;
    
    /* Handshake secret for Tls13 handshake */
    tsip_tls13_ephemeral_handshake_secret_key_index_t  handshakeSecret13Idx;
    
    /* the key to decrypt server-finished message */
    tsip_tls13_ephemeral_server_finished_key_index_t   serverFinished13Idx;

    /* key for Sha256-Hmac to gen "Client Finished" */
    tsip_hmac_sha_key_index_t                          clientFinished13Idx;

    /* AES decryption key for handshake */
    tsip_aes_key_index_t                               serverWriteKey13Idx;

    /* AES encryption key for handshake */
    tsip_aes_key_index_t                               clientWriteKey13Idx;

    /* Handshake verified data used for master secret */
    word32                          verifyData13Idx[TSIP_TLS_VERIFY_DATA_WD_SZ];

    /* master secret for TLS1.3 */
    tsip_tls13_ephemeral_master_secret_key_index_t     masterSecret13Idx;

    /* server app traffic secret */
    tsip_tls13_ephemeral_app_secret_key_index_t        serverAppTraffic13Secret;

    /* client app traffic secret */
    tsip_tls13_ephemeral_app_secret_key_index_t        clientAppTraffic13Secret;

    /* server write key */
    tsip_aes_key_index_t                               serverAppWriteKey13Idx;

    /* client write key */
    tsip_aes_key_index_t                               clientAppWriteKey13Idx;

    /* hash handle for transcript hash of handshake messages */
    tsip_hmac_sha_handle_t                             hmacFinished13Handle;

    /* storage for handshake messages */
    MsgBag                                             messageBag;

    /* signature data area for TLS1.3 CertificateVerify message  */
    byte                             sigDataCertVerify[TSIP_TLS_MAX_SIGDATA_SZ];

    
#if (WOLFSSL_RENESAS_TSIP_VER >=109)
    /* out from R_SCE_TLS_ServerKeyExchangeVerify */
    uint32_t encrypted_ephemeral_ecdh_public_key[ENCRYPTED_ECDHE_PUBKEY_SZ];
    
    /* ephemeral ECDH pubkey index 
     * got from R_TSIP_GenerateTlsP256EccKeyIndex.
     * Input to R_TSIP_TlsGeneratePreMasterSecretWithEccP256Key.
     */
    tsip_tls_p256_ecc_key_index_t ecc_p256_wrapped_key;

    /* ephemeral ECDH pub-key Qx(256bit)||Qy(256bit)
     * got from  R_TSIP_GenerateTlsP256EccKeyIndex.
     * Should be sent to peer(server) in Client Key Exchange msg.
     */
    uint8_t ecc_ecdh_public_key[ECCP256_PUBKEY_SZ];
#endif /* WOLFSSL_RENESAS_TSIP_VER >=109 */

    /* info to generate session key */
    uint32_t    tsip_masterSecret[TSIP_TLS_MASTERSECRET_SIZE/4];
    uint8_t     tsip_clientRandom[TSIP_TLS_CLIENTRANDOM_SZ];
    uint8_t     tsip_serverRandom[TSIP_TLS_SERVERRANDOM_SZ];
#endif /* WOLFSSL_RENESAS_TSIP_TLS */
/* for tsip crypt only mode */
#ifdef WOLFSSL_RENESAS_TSIP_CRYPTONLY
    
    renesas_tsip_key    rsa1024pri_keyIdx;
    renesas_tsip_key    rsa1024pub_keyIdx;
    renesas_tsip_key    rsa2048pri_keyIdx;
    renesas_tsip_key    rsa2048pub_keyIdx;
    
    /* sign/verify hash type :
     * md5, sha1 or sha256
     */
    int sing_hash_type;
    
    /* flags shows status if tsip keys are installed */
    union {
        uint8_t chr;
        struct tsip_keyflgs_cryt bits;
    } keyflgs_crypt;
    
#endif
    /* installed key handling */
    tsip_aes_key_index_t user_aes256_key_index;
    uint8_t user_aes256_key_set:1;
    tsip_aes_key_index_t user_aes128_key_index;
    uint8_t user_aes128_key_set:1;
    
    /* TSIP defined cipher suite number */
    uint32_t    tsip_cipher;
    
    /* flags */
#ifdef WOLFSSL_RENESAS_TSIP_TLS
    #if !defined(NO_RSA)
    uint8_t ClientRsa2048PrivKey_set:1;
    uint8_t ClientRsa2048PubKey_set:1;
    #endif

    #if defined(HAVE_ECC)
    uint8_t ClientEccP256PrivKey_set:1;
    uint8_t ClientEccP256PubKey_set:1;
    #endif

    uint8_t HmacInitialized:1;
    uint8_t RootCAverified:1;
    uint8_t EcdsaPrivKey_set:1;
    uint8_t Dhe_key_set:1;
    uint8_t SharedSecret_set:1;
    uint8_t EarlySecret_set:1;
    uint8_t HandshakeSecret_set:1;
    uint8_t HandshakeClientTrafficKey_set:1;
    uint8_t HandshakeServerTrafficKey_set:1;
    uint8_t HandshakeVerifiedData_set:1;
    uint8_t MasterSecret_set:1;
    uint8_t ServerTrafficSecret_set:1;
    uint8_t ClientTrafficSecret_set:1;
    uint8_t ServerWriteTrafficKey_set:1;
    uint8_t ClientWriteTrafficKey_set:1;
    uint8_t session_key_set:1;
#endif /* WOLFSSL_RENESAS_TSIP_TLS */

} TsipUserCtx;

typedef TsipUserCtx RenesasUserCtx;
typedef TsipUserCtx user_PKCbInfo;

typedef struct
{
    TsipUserCtx* userCtx; 
} TsipPKCbInfo;


#if (WOLFSSL_RENESAS_TSIP_VER >=109)

typedef struct
{
    uint8_t *  encrypted_provisioning_key;
    uint8_t *  iv;
    uint8_t *  encrypted_user_tls_key;
    uint32_t   encrypted_user_tls_key_type;
    uint8_t *  encrypted_user_private_key;
    uint32_t   encrypted_user_private_key_type;
    uint8_t *  encrypted_user_public_key;
    uint32_t   encrypted_user_public_key_type;
    tsip_ecc_private_key_index_t client_private_key_index;
    tsip_tls_ca_certification_public_key_index_t  user_rsa2048_tls_pubindex;
} tsip_key_data;

#else
typedef struct
{
    uint8_t*  encrypted_session_key;
    uint8_t*  iv;
    uint8_t*  encrypted_user_tls_key;
    tsip_tls_ca_certification_public_key_index_t  user_rsa2048_tls_pubindex;
} tsip_key_data;

#endif

struct Aes;
struct WOLFSSL;
struct WOLFSSL_CTX;
struct wc_CryptoInfo;
/*----------------------------------------------------*/
/*   APIs                                             */
/*----------------------------------------------------*/

WOLFSSL_API void tsip_inform_cert_sign(const byte *sign);

WOLFSSL_API void tsip_set_callbacks(struct WOLFSSL_CTX* ctx);

WOLFSSL_API int  tsip_set_callback_ctx(struct WOLFSSL* ssl, void* user_ctx);

WOLFSSL_API int  tsip_set_clientPrivateKeyEnc(const byte* key, int keyType);

#if defined(WOLF_PRIVATE_KEY_ID)

#if defined(WOLFSSL_RENESAS_TSIP_TLS)
WOLFSSL_API int tsip_use_PublicKey_buffer_TLS(WOLFSSL* ssl,
                                const char* keyBuf, int keyBufLen, int keyType);
WOLFSSL_API int tsip_use_PrivateKey_buffer_TLS(struct WOLFSSL* ssl,
                                const char* keyBuf, int keyBufLen, int keyType);
#endif

#if defined(WOLFSSL_RENESAS_TSIP_CRYPTONLY)
WOLFSSL_API int tsip_use_PubicKey_buffer_crypt(TsipUserCtx *uc,
                                const char* keyBuf, int keyBufLen, int keyType);
WOLFSSL_API int tsip_use_PrivateKey_buffer_crypt(TsipUserCtx *uc,
                                const char* keyBuf, int keyBufLen, int keyType);
#endif

#endif /* WOLF_PRIVATE_KEY_ID */

#if (WOLFSSL_RENESAS_TSIP_VER >=109)

#define wc_tsip_inform_user_keys_ex tsip_inform_user_keys_ex
WOLFSSL_API void tsip_inform_user_keys_ex(
    byte*       provisioning_key,   /* key got from DLM server */
    byte*       iv,                 /* iv used for public key  */
    byte*       encrypted_public_key,/*RSA2048 or ECDSAp256 public key*/
    word32      public_key_type);   /* 0: RSA-2048 2:ECDSA P-256 */    

#else

WOLFSSL_API void tsip_inform_user_keys(
    byte*       encrypted_session_key, 
    byte*       iv,
    byte*       encrypted_user_tls_key);

#endif

/*----------------------------------------------------*/
/*   internal use functions                           */
/*----------------------------------------------------*/
WOLFSSL_LOCAL int tsip_SignRsaPkcs(wc_CryptoInfo* info, TsipUserCtx* tuc);

WOLFSSL_LOCAL int tsip_VerifyRsaPkcsCb(
                        WOLFSSL* ssl, 
                        unsigned char* sig, unsigned int sigSz,
                        unsigned char** out,
                        const unsigned char* keyDer, unsigned int keySz,
                        void* ctx);

WOLFSSL_LOCAL int tsip_SignEcdsa(wc_CryptoInfo* info, TsipUserCtx* tuc);


#ifdef WOLF_CRYPTO_CB

struct wc_CryptoInfo;

WOLFSSL_LOCAL int tsip_TlsCleanup(struct WOLFSSL* ssl);

WOLFSSL_LOCAL int tsip_StoreMessage(struct WOLFSSL* ssl, const byte* data,
                                                         int sz);

WOLFSSL_LOCAL int tsip_GetMessageSha256(struct WOLFSSL* ssl, byte* hash,
                                                                int* sz);

WOLFSSL_LOCAL int tsip_Tls13GetHmacMessages(struct WOLFSSL* ssl, byte* mac);

WOLFSSL_LOCAL int tsip_Tls13GenEccKeyPair(struct WOLFSSL* ssl, 
                                                struct KeyShareEntry* kse);

WOLFSSL_LOCAL int tsip_Tls13GenSharedSecret(struct WOLFSSL* ssl,
                                                struct KeyShareEntry* kse);

WOLFSSL_LOCAL int tsip_Tls13DeriveEarlySecret(struct WOLFSSL* ssl);

WOLFSSL_LOCAL int tsip_Tls13DeriveHandshakeSecret(struct WOLFSSL* ssl);

WOLFSSL_LOCAL int tsip_Tls13DeriveKeys(struct WOLFSSL* ssl,
                                                int keyType, int side);

WOLFSSL_LOCAL int tsip_Tls13DeriveMasterSecret(struct WOLFSSL* ssl);

WOLFSSL_LOCAL int tsip_Tls13DeriveHandshakeTrafficKeys(struct WOLFSSL* ssl);

WOLFSSL_LOCAL int tsip_Tls13HandleFinished(struct WOLFSSL* ssl,
                                           const byte*     input,
                                           word32*         inOutIdx,
                                           word32          size,
                                           word32          totalSz);

WOLFSSL_LOCAL int tsip_Tls13BuildMessage(struct WOLFSSL* ssl,
                                         byte* output, int outSz,
                                         const byte* input, int inSz,
                                         int type,
                                         int hashOutput);

WOLFSSL_LOCAL int tsip_Tls13SendFinished(struct WOLFSSL* ssl,
                                             byte*       output,
                                             int         outSz,
                                             const byte* input,
                                             int         hashOutput);

WOLFSSL_LOCAL int tsip_Tls13VerifyHandshake(struct WOLFSSL* ssl,
                                               const byte* input, byte* hash,
                                               word32* pHashSz);

WOLFSSL_LOCAL int tsip_Tls13AesDecrypt(struct WOLFSSL* ssl, 
                                    byte* output, const byte* input, word16 sz);

WOLFSSL_LOCAL int tsip_Tls13AesEncrypt(struct WOLFSSL* ssl,
                                    byte* output, const byte* input, word16 sz);

WOLFSSL_LOCAL int tsip_Tls13CertificateVerify(struct WOLFSSL* ssl, 
                                            const byte* input, word32* inOutIdx,
                                            word32 totalSz);

WOLFSSL_LOCAL int tsip_Tls13SendCertVerify(struct WOLFSSL*ssl);

#endif /* WOLF_CRYPTO_CB */



#if (WOLFSSL_RENESAS_TSIP_VER >=109)
WOLFSSL_LOCAL int wc_tsip_AesCipher(int devIdArg, struct wc_CryptoInfo* info, 
                                                                    void* ctx);
WOLFSSL_LOCAL int wc_tsip_generateMasterSecretEx(
        byte        cipherSuiteFirst,
        byte        cipherSuite,
        const byte* pr,                 /* pre-master    */
        const byte* cr,                 /* client random */
        const byte* sr,                 /* server random */
        byte*       ms);

#else

WOLFSSL_LOCAL int wc_tsip_generateMasterSecret(
        const byte *pre,
        const byte *cr,
        const byte *sr,
        byte *ms);

#endif /* WOLFSSL_RENESAS_TSIP_VER */


WOLFSSL_LOCAL int wc_tsip_storeKeyCtx(
        WOLFSSL *ssl,
        TsipUserCtx *userCtx);

WOLFSSL_LOCAL int wc_tsip_generateEncryptPreMasterSecret(
        WOLFSSL*  ssl,
        byte*       out,
        word32*     outSz);

WOLFSSL_LOCAL int wc_tsip_EccSharedSecret(
        WOLFSSL* ssl,
        struct ecc_key* otherKey,
        unsigned char* pubKeyDer, unsigned int* pubKeySz,
        unsigned char* out, unsigned int* outlen,
        int side, void* ctx);

WOLFSSL_LOCAL int wc_tsip_RsaVerify(
        WOLFSSL* ssl,
        byte* sig,
        word32 sigSz,
        byte** out,
        const byte* key,
        word32 keySz,
        void* ctx);

WOLFSSL_LOCAL int wc_tsip_EccVerify(
        WOLFSSL*  ssl, 
        const byte* sig,    word32  sigSz,
        const byte* hash,   word32  hashSz,
        const byte* key,    word32  keySz,
        int*  result, void*   ctx);

WOLFSSL_LOCAL int wc_tsip_generateVerifyData(
        const uint8_t*  masterSecret,
        const uint8_t*  side,
        const uint8_t*  handshake_hash,
        uint8_t*        hashes);
#ifndef NO_AES
WOLFSSL_LOCAL int wc_tsip_AesCbcEncrypt(
        Aes* aes,
        byte*       out,
        const byte* in,
        word32      sz);

WOLFSSL_LOCAL int wc_tsip_AesCbcDecrypt(
        Aes* aes,
        byte*       out,
        const byte* in,
        word32      sz);
 
WOLFSSL_LOCAL int wc_tsip_AesGcmEncrypt(
        Aes* aes, byte* out,
        const byte* in, word32 sz,
              byte* iv, word32 ivSz,
              byte* authTag, word32 authTagSz,
        const byte* authIn, word32 authInSz,
        void* ctx);
            
WOLFSSL_LOCAL int wc_tsip_AesGcmDecrypt(
        Aes* aes, byte* out,
        const byte* in, word32 sz,
        const byte* iv, word32 ivSz,
        const byte* authTag, word32 authTagSz,
        const byte* authIn, word32 authInSz,
        void* ctx);
#endif /* NO_AES */
WOLFSSL_LOCAL int wc_tsip_ShaXHmacVerify(
        const WOLFSSL *ssl,
        const byte* message, 
        word32      messageSz,
        word32      macSz,
        word32      content);

WOLFSSL_LOCAL int wc_tsip_Sha1HmacGenerate(
        const WOLFSSL *ssl,
        const byte* myInner, 
        word32      innerSz,
        const byte* in,
        word32      sz, 
        byte*       digest);

WOLFSSL_LOCAL int wc_tsip_Sha256HmacGenerate(
        const WOLFSSL *ssl,
        const byte* myInner,
        word32      innerSz,
        const byte* in,
        word32      sz,
        byte*       digest);

WOLFSSL_LOCAL int  tsip_Open();

WOLFSSL_LOCAL void tsip_Close();

WOLFSSL_LOCAL int  tsip_hw_lock();

WOLFSSL_LOCAL void tsip_hw_unlock( void );

WOLFSSL_LOCAL int  tsip_usable(const WOLFSSL *ssl,
                                uint8_t session_key_generated);

WOLFSSL_LOCAL void tsip_inform_sflash_signedcacert(
        const byte* ps_flash, 
        const byte* psigned_ca_cert,
            word32  len);

WOLFSSL_LOCAL byte tsip_rootCAverified();

WOLFSSL_LOCAL byte tsip_checkCA(word32 cmIdx);

WOLFSSL_LOCAL int  wc_tsip_tls_RootCertVerify(
        const   byte* cert,   word32 cert_len,
        word32  key_n_start,  word32 key_n_len,
        word32  key_e_start,  word32 key_e_len,
        word32  cm_row);

WOLFSSL_LOCAL int  wc_tsip_tls_CertVerify(
        const   uint8_t* cert,      uint32_t certSz,
        const   uint8_t* signature, uint32_t sigSz,
        uint32_t  key_n_start,      uint32_t key_n_len,
        uint32_t  key_e_start,      uint32_t key_e_len,
        uint8_t*  tsip_encRsaKeyIdx);

WOLFSSL_LOCAL int  wc_tsip_generatePremasterSecret(
        byte*   premaster,
        word32  preSz);

WOLFSSL_LOCAL int  wc_tsip_generateSessionKey(
        WOLFSSL* ssl,
        TsipUserCtx*    ctx,
        int             devId);

WOLFSSL_LOCAL int wc_tsip_MakeRsaKey(int size, void* ctx);
WOLFSSL_LOCAL int wc_tsip_RsaVerifyPkcs(wc_CryptoInfo* info, 
                                                TsipUserCtx* tuc);


#if defined(WOLFSSL_RENESAS_TSIP_CRYPT_DEBUG)
byte *ret2err(word32 ret);

#endif

#ifdef __cplusplus
}
#endif

#endif  /* !WOLFCRYPT_ONLY */
#endif  /* __RENESAS_TSIP_CRYPT_H__ */
