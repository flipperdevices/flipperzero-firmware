/* cryptocb.h
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

#ifndef _WOLF_CRYPTO_CB_H_
#define _WOLF_CRYPTO_CB_H_

#include <wolfssl/wolfcrypt/types.h>

#ifdef __cplusplus
    extern "C" {
#endif

/* Defines the Crypto Callback interface version, for compatibility */
/* Increment this when Crypto Callback interface changes are made */
#define CRYPTO_CB_VER   2


#ifdef WOLF_CRYPTO_CB

#ifndef NO_RSA
    #include <wolfssl/wolfcrypt/rsa.h>
#endif
#ifdef HAVE_ECC
    #include <wolfssl/wolfcrypt/ecc.h>
#endif
#ifndef NO_AES
    #include <wolfssl/wolfcrypt/aes.h>
#endif
#ifndef NO_SHA
    #include <wolfssl/wolfcrypt/sha.h>
#endif
#ifndef NO_SHA256
    #include <wolfssl/wolfcrypt/sha256.h>
#endif
#ifndef NO_HMAC
    #include <wolfssl/wolfcrypt/hmac.h>
#endif
#ifndef WC_NO_RNG
    #include <wolfssl/wolfcrypt/random.h>
#endif
#ifndef NO_DES3
    #include <wolfssl/wolfcrypt/des3.h>
#endif
#ifdef WOLFSSL_CMAC
    #include <wolfssl/wolfcrypt/cmac.h>
#endif
#ifdef HAVE_ED25519
    #include <wolfssl/wolfcrypt/ed25519.h>
#endif
#ifdef HAVE_CURVE25519
    #include <wolfssl/wolfcrypt/curve25519.h>
#endif
#if defined(WOLFSSL_SHA512) || defined(WOLFSSL_SHA384)
    #include <wolfssl/wolfcrypt/sha512.h>
#endif

#ifdef WOLF_CRYPTO_CB_CMD
/* CryptoCb Commands */
enum wc_CryptoCbCmdType {
    WC_CRYPTOCB_CMD_TYPE_NONE = 0,
    WC_CRYPTOCB_CMD_TYPE_REGISTER,
    WC_CRYPTOCB_CMD_TYPE_UNREGISTER,

    WC_CRYPTOCB_CMD_TYPE_MAX = WC_CRYPTOCB_CMD_TYPE_UNREGISTER
};
#endif

/* Crypto Information Structure for callbacks */
typedef struct wc_CryptoInfo {
    int algo_type; /* enum wc_AlgoType */
#if HAVE_ANONYMOUS_INLINE_AGGREGATES
    union {
#endif
#if !defined(NO_RSA) || defined(HAVE_ECC)
    struct {
        int type; /* enum wc_PkType */
#if HAVE_ANONYMOUS_INLINE_AGGREGATES
        union {
#endif
        #ifndef NO_RSA
            struct {
                const byte* in;
                word32      inLen;
                byte*       out;
                word32*     outLen;
                int         type;
                RsaKey*     key;
                WC_RNG*     rng;
            } rsa;
        #ifdef WOLFSSL_KEY_GEN
            struct {
                RsaKey* key;
                int     size;
                long    e;
                WC_RNG* rng;
            } rsakg;
        #endif
            struct {
                RsaKey*     key;
                const byte* pubKey;
                word32      pubKeySz;
            } rsa_check;
            struct {
                const RsaKey* key;
                int*          keySize;
            } rsa_get_size;
        #endif
        #ifdef HAVE_ECC
            struct {
                WC_RNG*  rng;
                int      size;
                ecc_key* key;
                int      curveId;
            } eckg;
            struct {
                ecc_key* private_key;
                ecc_key* public_key;
                byte*    out;
                word32*  outlen;
            } ecdh;
            struct {
                const byte* in;
                word32      inlen;
                byte*       out;
                word32*     outlen;
                WC_RNG*     rng;
                ecc_key*    key;
            } eccsign;
            struct {
                const byte* sig;
                word32      siglen;
                const byte* hash;
                word32      hashlen;
                int*        res;
                ecc_key*    key;
            } eccverify;
            struct {
                ecc_key*    key;
                const byte* pubKey;
                word32      pubKeySz;
            } ecc_check;
        #endif
        #ifdef HAVE_CURVE25519
            struct {
                WC_RNG*  rng;
                int      size;
                curve25519_key* key;
                int      curveId;
            } curve25519kg;
            struct {
                curve25519_key* private_key;
                curve25519_key* public_key;
                byte*    out;
                word32*  outlen;
                int      endian;
            } curve25519;
        #endif
        #ifdef HAVE_ED25519
            struct {
                WC_RNG*  rng;
                int      size;
                ed25519_key* key;
                int      curveId;
            } ed25519kg;
            struct {
                const byte*  in;
                word32       inLen;
                byte*        out;
                word32*      outLen;
                ed25519_key* key;
                byte         type;
                const byte*  context;
                byte         contextLen;
            } ed25519sign;
            struct {
                const byte*  sig;
                word32       sigLen;
                const byte*  msg;
                word32       msgLen;
                int*         res;
                ed25519_key* key;
                byte         type;
                const byte*  context;
                byte         contextLen;
            } ed25519verify;
        #endif
#if HAVE_ANONYMOUS_INLINE_AGGREGATES
        };
#endif
    } pk;
#endif /* !NO_RSA || HAVE_ECC */
#if !defined(NO_AES) || !defined(NO_DES3)
    struct {
        int type; /* enum wc_CipherType */
        int enc;
#if HAVE_ANONYMOUS_INLINE_AGGREGATES
        union {
#endif
        #ifdef HAVE_AESGCM
            struct {
                Aes*        aes;
                byte*       out;
                const byte* in;
                word32      sz;
                const byte* iv;
                word32      ivSz;
                byte*       authTag;
                word32      authTagSz;
                const byte* authIn;
                word32      authInSz;
            } aesgcm_enc;
            struct {
                Aes*        aes;
                byte*       out;
                const byte* in;
                word32      sz;
                const byte* iv;
                word32      ivSz;
                const byte* authTag;
                word32      authTagSz;
                const byte* authIn;
                word32      authInSz;
            } aesgcm_dec;
        #endif /* HAVE_AESGCM */
        #ifdef HAVE_AESCCM
            struct {
                Aes*        aes;
                byte*       out;
                const byte* in;
                word32      sz;
                const byte* nonce;
                word32      nonceSz;
                byte*       authTag;
                word32      authTagSz;
                const byte* authIn;
                word32      authInSz;
            } aesccm_enc;
            struct {
                Aes*        aes;
                byte*       out;
                const byte* in;
                word32      sz;
                const byte* nonce;
                word32      nonceSz;
                const byte* authTag;
                word32      authTagSz;
                const byte* authIn;
                word32      authInSz;
            } aesccm_dec;
        #endif /* HAVE_AESCCM */
        #if defined(HAVE_AES_CBC)
            struct {
                Aes*        aes;
                byte*       out;
                const byte* in;
                word32      sz;
            } aescbc;
        #endif /* HAVE_AES_CBC */
        #if defined(WOLFSSL_AES_COUNTER)
            struct {
                Aes*        aes;
                byte*       out;
                const byte* in;
                word32      sz;
            } aesctr;
        #endif /* WOLFSSL_AES_COUNTER */
        #if defined(HAVE_AES_ECB)
            struct {
                Aes*        aes;
                byte*       out;
                const byte* in;
                word32      sz;
            } aesecb;
        #endif /* HAVE_AES_ECB */
        #ifndef NO_DES3
            struct {
                Des3*       des;
                byte*       out;
                const byte* in;
                word32      sz;
            } des3;
        #endif
#if HAVE_ANONYMOUS_INLINE_AGGREGATES
        };
#endif
    } cipher;
#endif /* !NO_AES || !NO_DES3 */
#if !defined(NO_SHA) || !defined(NO_SHA256) || \
    defined(WOLFSSL_SHA512) || defined(WOLFSSL_SHA384)
    struct {
        int type; /* enum wc_HashType */
        const byte* in;
        word32 inSz;
        byte* digest;
#if HAVE_ANONYMOUS_INLINE_AGGREGATES
        union {
#endif
        #ifndef NO_SHA
            wc_Sha* sha1;
        #endif
        #ifdef WOLFSSL_SHA224
            wc_Sha224* sha224;
        #endif
        #ifndef NO_SHA256
            wc_Sha256* sha256;
        #endif
        #ifdef WOLFSSL_SHA384
            wc_Sha384* sha384;
        #endif
        #ifdef WOLFSSL_SHA512
            wc_Sha512* sha512;
        #endif
#if HAVE_ANONYMOUS_INLINE_AGGREGATES
        };
#endif
    } hash;
#endif /* !NO_SHA || !NO_SHA256 */
#ifndef NO_HMAC
    struct {
        int macType; /* enum wc_HashType */
        const byte* in;
        word32 inSz;
        byte* digest;
        Hmac* hmac;
    } hmac;
#endif
#ifndef WC_NO_RNG
    struct {
        WC_RNG* rng;
        byte* out;
        word32 sz;
    } rng;
    struct {
        OS_Seed* os;
        byte* seed;
        word32 sz;
    } seed;
#endif
#ifdef WOLFSSL_CMAC
    struct {
        Cmac* cmac;
        void* ctx;
        const byte* key;
        const byte* in;
        byte*       out;
        word32* outSz;
        word32  keySz;
        word32  inSz;
        int type;
    } cmac;
#endif
#ifdef WOLF_CRYPTO_CB_CMD
    struct {      /* uses wc_AlgoType=ALGO_NONE */
        int type; /* enum wc_CryptoCbCmdType */
        void *ctx;
    } cmd;
#endif
#if HAVE_ANONYMOUS_INLINE_AGGREGATES
    };
#endif
} wc_CryptoInfo;


typedef int (*CryptoDevCallbackFunc)(int devId, wc_CryptoInfo* info, void* ctx);

WOLFSSL_LOCAL void wc_CryptoCb_Init(void);
WOLFSSL_LOCAL void wc_CryptoCb_Cleanup(void);
WOLFSSL_LOCAL int wc_CryptoCb_GetDevIdAtIndex(int startIdx);
WOLFSSL_API int  wc_CryptoCb_RegisterDevice(int devId, CryptoDevCallbackFunc cb, void* ctx);
WOLFSSL_API void wc_CryptoCb_UnRegisterDevice(int devId);
WOLFSSL_API int wc_CryptoCb_DefaultDevID(void);

#ifdef WOLF_CRYPTO_CB_FIND
typedef int (*CryptoDevCallbackFind)(int devId, int algoType);
WOLFSSL_API void wc_CryptoCb_SetDeviceFindCb(CryptoDevCallbackFind cb);
#endif

#ifdef DEBUG_CRYPTOCB
WOLFSSL_API void wc_CryptoCb_InfoString(wc_CryptoInfo* info);
#endif

/* old function names */
#define wc_CryptoDev_RegisterDevice   wc_CryptoCb_RegisterDevice
#define wc_CryptoDev_UnRegisterDevice wc_CryptoCb_UnRegisterDevice


#ifndef NO_RSA
WOLFSSL_LOCAL int wc_CryptoCb_Rsa(const byte* in, word32 inLen, byte* out,
    word32* outLen, int type, RsaKey* key, WC_RNG* rng);

#ifdef WOLFSSL_KEY_GEN
WOLFSSL_LOCAL int wc_CryptoCb_MakeRsaKey(RsaKey* key, int size, long e,
    WC_RNG* rng);
#endif /* WOLFSSL_KEY_GEN */

WOLFSSL_LOCAL int wc_CryptoCb_RsaCheckPrivKey(RsaKey* key, const byte* pubKey,
    word32 pubKeySz);
WOLFSSL_LOCAL int wc_CryptoCb_RsaGetSize(const RsaKey* key, int* keySize);
#endif /* !NO_RSA */

#ifdef HAVE_ECC
WOLFSSL_LOCAL int wc_CryptoCb_MakeEccKey(WC_RNG* rng, int keySize,
    ecc_key* key, int curveId);

WOLFSSL_LOCAL int wc_CryptoCb_Ecdh(ecc_key* private_key, ecc_key* public_key,
    byte* out, word32* outlen);

WOLFSSL_LOCAL int wc_CryptoCb_EccSign(const byte* in, word32 inlen, byte* out,
    word32 *outlen, WC_RNG* rng, ecc_key* key);

WOLFSSL_LOCAL int wc_CryptoCb_EccVerify(const byte* sig, word32 siglen,
    const byte* hash, word32 hashlen, int* res, ecc_key* key);

WOLFSSL_LOCAL int wc_CryptoCb_EccCheckPrivKey(ecc_key* key, const byte* pubKey,
    word32 pubKeySz);
#endif /* HAVE_ECC */

#ifdef HAVE_CURVE25519
WOLFSSL_LOCAL int wc_CryptoCb_Curve25519Gen(WC_RNG* rng, int keySize,
    curve25519_key* key);

WOLFSSL_LOCAL int wc_CryptoCb_Curve25519(curve25519_key* private_key,
    curve25519_key* public_key, byte* out, word32* outlen, int endian);
#endif /* HAVE_CURVE25519 */

#ifdef HAVE_ED25519
WOLFSSL_LOCAL int wc_CryptoCb_Ed25519Gen(WC_RNG* rng, int keySize,
    ed25519_key* key);
WOLFSSL_LOCAL int wc_CryptoCb_Ed25519Sign(const byte* in, word32 inLen,
    byte* out, word32 *outLen, ed25519_key* key, byte type, const byte* context,
    byte contextLen);
WOLFSSL_LOCAL int wc_CryptoCb_Ed25519Verify(const byte* sig, word32 sigLen,
    const byte* msg, word32 msgLen, int* res, ed25519_key* key, byte type,
    const byte* context, byte contextLen);
#endif /* HAVE_ED25519 */

#ifndef NO_AES
#ifdef HAVE_AESGCM
WOLFSSL_LOCAL int wc_CryptoCb_AesGcmEncrypt(Aes* aes, byte* out,
     const byte* in, word32 sz, const byte* iv, word32 ivSz,
     byte* authTag, word32 authTagSz, const byte* authIn, word32 authInSz);

WOLFSSL_LOCAL int wc_CryptoCb_AesGcmDecrypt(Aes* aes, byte* out,
     const byte* in, word32 sz, const byte* iv, word32 ivSz,
     const byte* authTag, word32 authTagSz,
     const byte* authIn, word32 authInSz);
#endif /* HAVE_AESGCM */
#ifdef HAVE_AESCCM
WOLFSSL_LOCAL int wc_CryptoCb_AesCcmEncrypt(Aes* aes, byte* out,
    const byte* in, word32 sz,
    const byte* nonce, word32 nonceSz,
    byte* authTag, word32 authTagSz,
    const byte* authIn, word32 authInSz);

WOLFSSL_LOCAL int wc_CryptoCb_AesCcmDecrypt(Aes* aes, byte* out,
    const byte* in, word32 sz,
    const byte* nonce, word32 nonceSz,
    const byte* authTag, word32 authTagSz,
    const byte* authIn, word32 authInSz);
#endif /* HAVE_AESCCM */
#ifdef HAVE_AES_CBC
WOLFSSL_LOCAL int wc_CryptoCb_AesCbcEncrypt(Aes* aes, byte* out,
                               const byte* in, word32 sz);
WOLFSSL_LOCAL int wc_CryptoCb_AesCbcDecrypt(Aes* aes, byte* out,
                               const byte* in, word32 sz);
#endif /* HAVE_AES_CBC */
#ifdef WOLFSSL_AES_COUNTER
WOLFSSL_LOCAL int wc_CryptoCb_AesCtrEncrypt(Aes* aes, byte* out,
                               const byte* in, word32 sz);
#endif /* WOLFSSL_AES_COUNTER */
#ifdef HAVE_AES_ECB
WOLFSSL_LOCAL int wc_CryptoCb_AesEcbEncrypt(Aes* aes, byte* out,
                               const byte* in, word32 sz);
WOLFSSL_LOCAL int wc_CryptoCb_AesEcbDecrypt(Aes* aes, byte* out,
                               const byte* in, word32 sz);
#endif /* HAVE_AES_ECB */
#endif /* !NO_AES */

#ifndef NO_DES3
WOLFSSL_LOCAL int wc_CryptoCb_Des3Encrypt(Des3* des3, byte* out,
                               const byte* in, word32 sz);
WOLFSSL_LOCAL int wc_CryptoCb_Des3Decrypt(Des3* des3, byte* out,
                               const byte* in, word32 sz);
#endif /* !NO_DES3 */

#ifndef NO_SHA
WOLFSSL_LOCAL int wc_CryptoCb_ShaHash(wc_Sha* sha, const byte* in,
    word32 inSz, byte* digest);
#endif /* !NO_SHA */

#ifndef NO_SHA256
WOLFSSL_LOCAL int wc_CryptoCb_Sha256Hash(wc_Sha256* sha256, const byte* in,
    word32 inSz, byte* digest);
#endif /* !NO_SHA256 */
#ifdef WOLFSSL_SHA384
WOLFSSL_LOCAL int wc_CryptoCb_Sha384Hash(wc_Sha384* sha384, const byte* in,
    word32 inSz, byte* digest);
#endif
#ifdef WOLFSSL_SHA512
WOLFSSL_LOCAL int wc_CryptoCb_Sha512Hash(wc_Sha512* sha512, const byte* in,
    word32 inSz, byte* digest);
#endif

#ifndef NO_HMAC
WOLFSSL_LOCAL int wc_CryptoCb_Hmac(Hmac* hmac, int macType, const byte* in,
    word32 inSz, byte* digest);
#endif /* !NO_HMAC */

#ifndef WC_NO_RNG
WOLFSSL_LOCAL int wc_CryptoCb_RandomBlock(WC_RNG* rng, byte* out, word32 sz);
WOLFSSL_LOCAL int wc_CryptoCb_RandomSeed(OS_Seed* os, byte* seed, word32 sz);
#endif

#ifdef WOLFSSL_CMAC
WOLFSSL_LOCAL int wc_CryptoCb_Cmac(Cmac* cmac, const byte* key, word32 keySz,
        const byte* in, word32 inSz, byte* out, word32* outSz, int type,
        void* ctx);
#endif

#endif /* WOLF_CRYPTO_CB */

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* _WOLF_CRYPTO_CB_H_ */
