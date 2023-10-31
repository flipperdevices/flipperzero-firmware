/* renesas-sce-crypt.h
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
#ifndef __RENESAS_SCE_CRYPT_H__
#define __RENESAS_SCE_CRYPT_H__

#include "r_sce.h"
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/logging.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SCE_SESSIONKEY_NONCE_SIZE           8
#define WOLFSSL_SCE_ILLEGAL_CIPHERSUITE     -1

#define MAX_SCE_CBINDEX 5

typedef void* renesas_sce_wrappedkey;

/* flsgas related to TLS */
struct sce_keyflgs_tls {
    uint8_t pk_key_set:1;
    uint8_t session_key_set:1;
};

/* flags Crypt Only */
struct sce_keyflgs_cryt {
    uint8_t aes256_installedkey_set:1;
    uint8_t aes128_installedkey_set:1;
    uint8_t rsapri2048_installedkey_set:1;
    uint8_t rsapub2048_installedkey_set:1;
    uint8_t rsapri1024_installedkey_set:1;
    uint8_t rsapub1024_installedkey_set:1;
    uint8_t message_type:1;/*message 0, hashed 1*/
};

typedef struct tagUser_SCEPKCbInfo {
    /* unique number for each session */
    int devId;
   #if defined(WOLFSSL_RENESAS_SCEPROTECT) && \
        !defined(WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY)
    /* out from R_SCE_TLS_ServerKeyExchangeVerify */
    uint32_t encrypted_ephemeral_ecdh_public_key[SCE_TLS_ENCRYPTED_ECCPUBKEY_SZ];
    /* out from R_SCE_TLS_ECC_secp256r1_EphemeralWrappedKeyPairGenerate */
    sce_tls_p256_ecc_wrapped_key_t ecc_p256_wrapped_key;
    uint8_t ecc_ecdh_public_key[HW_SCE_ECC_PUBLIC_KEY_BYTE_SIZE];
    
    uint32_t    sce_masterSecret[SCE_TLS_MASTERSECRET_SIZE/4];
    uint8_t     sce_clientRandom[SCE_TLS_CLIENTRANDOM_SZ];
    uint8_t     sce_serverRandom[SCE_TLS_SERVERRANDOM_SZ];
    uint8_t     sce_cipher;

   #endif
    
    /* installed key handling */
    /* aes */
    renesas_sce_wrappedkey   sce_wrapped_key_aes256;
    renesas_sce_wrappedkey   sce_wrapped_key_aes128;
    
   #if defined(WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY)
    /* rsa */
    renesas_sce_wrappedkey   sce_wrapped_key_rsapri2048;
    renesas_sce_wrappedkey   sce_wrapped_key_rsapub2048;
    renesas_sce_wrappedkey   sce_wrapped_key_rsapri1024;
    renesas_sce_wrappedkey   sce_wrapped_key_rsapub1024;
   #endif
    
    /* key status flags */
    /* flag whether encrypted ec key is set */
    union {
        uint8_t chr;
        struct sce_keyflgs_tls bits;
    } keyflgs_tls;
    /* key status flags */
    /* flags shows status if wrapped keys are installed */
    union {
        uint8_t chr;
        struct sce_keyflgs_cryt bits;
    } keyflgs_crypt;
    
} User_SCEPKCbInfo;

typedef struct tagSCE_PKCbInfo {
    User_SCEPKCbInfo *user_PKCbInfo[MAX_SCE_CBINDEX];
    uint32_t    num_session;
} SCE_PKCbInfo;

typedef struct
{
    uint8_t                          *encrypted_provisioning_key;
    uint8_t                          *iv;
    uint8_t                          *encrypted_user_tls_key;
    uint32_t                          encrypted_user_tls_key_type;
    sce_tls_ca_certification_public_wrapped_key_t  user_rsa2048_tls_wrappedkey;
} sce_key_data;
    
struct WOLFSSL;
struct WOLFSSL_CTX;
struct ecc_key;
    
WOLFSSL_LOCAL int     wc_sce_Open();
WOLFSSL_LOCAL void    wc_sce_Close();
WOLFSSL_LOCAL int     wc_sce_hw_lock();
WOLFSSL_LOCAL void    wc_sce_hw_unlock( void );
WOLFSSL_LOCAL int     wc_sce_usable(const struct WOLFSSL *ssl, 
                                    uint8_t session_key_generated);

typedef struct {
    sce_aes_wrapped_key_t   sce_wrapped_key;
    word32                  keySize;
    byte                    setup;
} SCE_AES_CTX;

struct Aes;
WOLFSSL_LOCAL int wc_sce_AesCbcEncrypt(struct Aes* aes, byte* out, const byte* in,
                             word32 sz);
WOLFSSL_LOCAL int wc_sce_AesCbcDecrypt(struct Aes* aes, byte* out, const byte* in,
                             word32 sz);

WOLFSSL_LOCAL int  wc_sce_AesGcmEncrypt(struct Aes* aes, byte* out,
                          const byte* in, word32 sz,
                          byte* iv, word32 ivSz,
                          byte* authTag, word32 authTagSz,
                          const byte* authIn, word32 authInSz,
                          void* ctx);

WOLFSSL_LOCAL int  wc_sce_AesGcmDecrypt(struct Aes* aes, byte* out,
                          const byte* in, word32 sz,
                          const byte* iv, word32 ivSz,
                          const byte* authTag, word32 authTagSz,
                          const byte* authIn, word32 authInSz,
                          void* ctx);
                          
#if !defined(NO_SHA256) && !defined(NO_WOLFSSL_RENESAS_SCEPROTECT_HASH)

typedef enum {
    SCE_SHA256 = 1,
} SCE_SHA_TYPE;

typedef struct {
    byte*  msg;
    void*  heap;
    word32 used;
    word32 len;
    word32 sha_type;
#if defined(WOLF_CRYPTO_CB)
    word32 flags;
    int devId;
#endif
} wolfssl_SCE_Hash;

/* RAW hash function APIs are not implemented with SCE */
#undef  WOLFSSL_NO_HASH_RAW
#define WOLFSSL_NO_HASH_RAW

typedef wolfssl_SCE_Hash wc_Sha256;

#endif /* NO_SHA */

#if defined(WOLFSSL_RENESAS_SCEPROTECT) && \
        !defined(WOLFSSL_RENESAS_SCEPROTECT_CRYPT_ONLY)
        
WOLFSSL_LOCAL int     wc_sce_tls_RootCertVerify(
            const   uint8_t* cert,         uint32_t cert_len,
            uint32_t  key_n_start,        uint32_t key_n_len,
            uint32_t  key_e_start,        uint32_t key_e_len,
            uint32_t  cm_row);

WOLFSSL_LOCAL int     wc_sce_tls_CertVerify(
            const   uint8_t* cert,         uint32_t certSz,
            const   uint8_t* signature,    uint32_t sigSz,
            uint32_t  key_n_start,        uint32_t key_n_len,
            uint32_t  key_e_start,        uint32_t key_e_len,
            uint8_t*   sce_encRsaKeyIdx);


WOLFSSL_LOCAL int     wc_sce_generatePremasterSecret(
            uint8_t*   premaster,
            uint32_t  preSz);

WOLFSSL_LOCAL int     wc_sce_generateEncryptPreMasterSecret(
            struct WOLFSSL* ssl, 
            uint8_t*           out, 
            uint32_t*         outSz);

WOLFSSL_LOCAL int     wc_sce_Sha256GenerateHmac(
            const struct WOLFSSL *ssl,
            const uint8_t* myInner, 
            uint32_t      innerSz,
            const uint8_t* in,
            uint32_t      sz,
            uint8_t*       digest);
    
WOLFSSL_LOCAL int wc_sce_Sha256VerifyHmac(
        const struct WOLFSSL *ssl,
        const uint8_t* message, 
        uint32_t      messageSz,
        uint32_t      macSz,
        uint32_t      content);

WOLFSSL_LOCAL int wc_sce_storeKeyCtx(
        struct WOLFSSL* ssl,
        User_SCEPKCbInfo* info);

WOLFSSL_LOCAL int wc_sce_generateVerifyData(
        const uint8_t*  ms, /* master secret */
        const uint8_t*  side,
        const uint8_t*  handshake_hash,
        uint8_t*        hashes /* out */);

WOLFSSL_LOCAL int wc_sce_generateSessionKey(
        struct WOLFSSL*   ssl,
        User_SCEPKCbInfo* cbInfo, 
        int               devId);
        
WOLFSSL_LOCAL int wc_sce_generateMasterSecret(
        uint8_t        cipherSuiteFirst,
        uint8_t        cipherSuite,
        const uint8_t *pr, /* pre-master    */
        const uint8_t *cr, /* client random */
        const uint8_t *sr, /* server random */
        uint8_t *ms);

WOLFSSL_LOCAL int wc_SCE_RsaVerify(struct WOLFSSL* ssl, byte* sig, uint32_t sigSz,
        uint8_t** out, const byte* key, uint32_t keySz, void* ctx);
WOLFSSL_LOCAL int wc_SCE_EccVerify(struct WOLFSSL* ssl, const uint8_t* sig, uint32_t sigSz,
        const uint8_t* hash, uint32_t hashSz, const uint8_t* key, uint32_t keySz,
        int* result, void* ctx);
/* Callback for EccShareSecret */
WOLFSSL_LOCAL int SCE_EccSharedSecret(struct WOLFSSL* ssl, struct ecc_key* otherKey,
        uint8_t* pubKeyDer, unsigned int* pubKeySz,
        uint8_t* out, unsigned int* outlen, int side, void* ctx);

/* user API */
WOLFSSL_API void wc_sce_inform_user_keys(
    uint8_t*     encrypted_provisioning_key,
    uint8_t*     iv,
    uint8_t*     encrypted_user_tls_key,
    uint32_t    encrypted_user_tls_key_type);

WOLFSSL_API void wc_sce_set_callbacks(struct WOLFSSL_CTX* ctx);
WOLFSSL_API int  wc_sce_set_callback_ctx(struct WOLFSSL* ssl, void* user_ctx);
WOLFSSL_API void wc_sce_inform_cert_sign(const uint8_t *sign);

/* rsa */
struct RsaKey;
struct WC_RNG;
WOLFSSL_API int  wc_sce_RsaFunction(const byte* in, word32 inLen, byte* out,
    word32 outLen, int type, struct RsaKey* key, struct WC_RNG* rng, void* ctx);
WOLFSSL_API int  wc_sce_MakeRsaKey(int size, void* ctx);
WOLFSSL_API int  wc_sce_RsaSign(const byte* in, word32 inLen, byte* out,
                    word32* outLen, struct RsaKey* key, void* ctx);
WOLFSSL_API int  wc_sce_RsaVerify(const byte* in, word32 inLen, byte* out,
                    word32* outLen,struct RsaKey* key, void* ctx);

#endif  /* WOLFSSL_RENESAS_SCEPROTECT && 
         * !WOLFSSL_RENESAS_SCEPROTECT_CRYPT_ONLY */

#endif  /* __RENESAS_SCE_CRYPT_H__ */
