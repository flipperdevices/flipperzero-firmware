/* cryptocb.c
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

/* This framework provides a central place for crypto hardware integration
   using the devId scheme. If not supported return `CRYPTOCB_UNAVAILABLE`. */

/* Some common, optional build settings:
 * these can also be set in wolfssl/options.h or user_settings.h
 * -------------------------------------------------------------
 * enable the find device callback functions
 * WOLF_CRYPTO_CB_FIND
 *
 * enable the command callback functions to invoke the callback during
 * register and unregister
 * WOLF_CRYPTO_CB_CMD
 *
 * enable debug InfoString functions
 * DEBUG_CRYPTO_CB
 */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifdef WOLF_CRYPTO_CB

#include <wolfssl/wolfcrypt/cryptocb.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>

#ifdef HAVE_ARIA
    #include <wolfssl/wolfcrypt/port/aria/aria-cryptocb.h>
#endif

#ifdef WOLFSSL_CAAM
    #include <wolfssl/wolfcrypt/port/caam/wolfcaam.h>
#endif

/* TODO: Consider linked list with mutex */
#ifndef MAX_CRYPTO_DEVID_CALLBACKS
#define MAX_CRYPTO_DEVID_CALLBACKS 8
#endif

typedef struct CryptoCb {
    int devId;
    CryptoDevCallbackFunc cb;
    void* ctx;
} CryptoCb;
static WOLFSSL_GLOBAL CryptoCb gCryptoDev[MAX_CRYPTO_DEVID_CALLBACKS];

#ifdef WOLF_CRYPTO_CB_FIND
static CryptoDevCallbackFind CryptoCb_FindCb = NULL;
#endif

#ifdef DEBUG_CRYPTOCB
static const char* GetAlgoTypeStr(int algo)
{
    switch (algo) { /* enum wc_AlgoType */
#ifdef WOLF_CRYPTO_CB_CMD
        case WC_ALGO_TYPE_NONE:   return "None-Command";
#endif
        case WC_ALGO_TYPE_HASH:   return "Hash";
        case WC_ALGO_TYPE_CIPHER: return "Cipher";
        case WC_ALGO_TYPE_PK:     return "PK";
        case WC_ALGO_TYPE_RNG:    return "RNG";
        case WC_ALGO_TYPE_SEED:   return "Seed";
        case WC_ALGO_TYPE_HMAC:   return "HMAC";
    }
    return NULL;
}
static const char* GetPkTypeStr(int pk)
{
    switch (pk) {
        case WC_PK_TYPE_RSA: return "RSA";
        case WC_PK_TYPE_DH: return "DH";
        case WC_PK_TYPE_ECDH: return "ECDH";
        case WC_PK_TYPE_ECDSA_SIGN: return "ECDSA-Sign";
        case WC_PK_TYPE_ECDSA_VERIFY: return "ECDSA-Verify";
        case WC_PK_TYPE_ED25519_SIGN: return "ED25519-Sign";
        case WC_PK_TYPE_ED25519_VERIFY: return "ED25519-Verify";
        case WC_PK_TYPE_CURVE25519: return "CURVE25519";
        case WC_PK_TYPE_RSA_KEYGEN: return "RSA KeyGen";
        case WC_PK_TYPE_EC_KEYGEN: return "ECC KeyGen";
    }
    return NULL;
}
static const char* GetCipherTypeStr(int cipher)
{
    switch (cipher) {
        case WC_CIPHER_AES: return "AES ECB";
        case WC_CIPHER_AES_CBC: return "AES CBC";
        case WC_CIPHER_AES_GCM: return "AES GCM";
        case WC_CIPHER_AES_CTR: return "AES CTR";
        case WC_CIPHER_AES_XTS: return "AES XTS";
        case WC_CIPHER_AES_CFB: return "AES CFB";
        case WC_CIPHER_DES3: return "DES3";
        case WC_CIPHER_DES: return "DES";
        case WC_CIPHER_CHACHA: return "ChaCha20";
    }
    return NULL;
}
static const char* GetHashTypeStr(int hash)
{
    switch (hash) {
        case WC_HASH_TYPE_MD2: return "MD2";
        case WC_HASH_TYPE_MD4: return "MD4";
        case WC_HASH_TYPE_MD5: return "MD5";
        case WC_HASH_TYPE_SHA: return "SHA-1";
        case WC_HASH_TYPE_SHA224: return "SHA-224";
        case WC_HASH_TYPE_SHA256: return "SHA-256";
        case WC_HASH_TYPE_SHA384: return "SHA-384";
        case WC_HASH_TYPE_SHA512: return "SHA-512";
        case WC_HASH_TYPE_MD5_SHA: return "MD5-SHA1";
        case WC_HASH_TYPE_SHA3_224: return "SHA3-224";
        case WC_HASH_TYPE_SHA3_256: return "SHA3-256";
        case WC_HASH_TYPE_SHA3_384: return "SHA3-384";
        case WC_HASH_TYPE_SHA3_512: return "SHA3-512";
        case WC_HASH_TYPE_BLAKE2B: return "Blake2B";
        case WC_HASH_TYPE_BLAKE2S: return "Blake2S";
    }
    return NULL;
}

#ifndef NO_RSA
static const char* GetRsaType(int type)
{
    switch (type) {
        case RSA_PUBLIC_ENCRYPT:  return "Public Encrypt";
        case RSA_PUBLIC_DECRYPT:  return "Public Decrypt";
        case RSA_PRIVATE_ENCRYPT: return "Private Encrypt";
        case RSA_PRIVATE_DECRYPT: return "Private Decrypt";
    }
    return NULL;
}
#endif

static const char* GetCryptoCbCmdTypeStr(int type)
{
    switch (type) {
        case WC_CRYPTOCB_CMD_TYPE_REGISTER:   return "Register";
        case WC_CRYPTOCB_CMD_TYPE_UNREGISTER: return "UnRegister";
    }
    return NULL;
}
WOLFSSL_API void wc_CryptoCb_InfoString(wc_CryptoInfo* info)
{
    if (info == NULL)
        return;

    if (info->algo_type == WC_ALGO_TYPE_PK) {
    #ifndef NO_RSA
        if (info->pk.type == WC_PK_TYPE_RSA) {
            printf("Crypto CB: %s %s (%d), %s, Len %d\n",
                GetAlgoTypeStr(info->algo_type),
                GetPkTypeStr(info->pk.type), info->pk.type,
                GetRsaType(info->pk.rsa.type), info->pk.rsa.inLen);
        }
        else
    #endif
        {
            printf("Crypto CB: %s %s (%d)\n", GetAlgoTypeStr(info->algo_type),
                GetPkTypeStr(info->pk.type), info->pk.type);
        }
    }
    else if (info->algo_type == WC_ALGO_TYPE_CIPHER) {
        printf("Crypto CB: %s %s (%d)\n", GetAlgoTypeStr(info->algo_type),
            GetCipherTypeStr(info->cipher.type), info->cipher.type);
    }
    else if (info->algo_type == WC_ALGO_TYPE_HASH) {
        printf("Crypto CB: %s %s (%d)\n", GetAlgoTypeStr(info->algo_type),
            GetHashTypeStr(info->hash.type), info->hash.type);
    }
    else if (info->algo_type == WC_ALGO_TYPE_HMAC) {
        printf("Crypto CB: %s %s (%d)\n", GetAlgoTypeStr(info->algo_type),
            GetHashTypeStr(info->hmac.macType), info->hmac.macType);
    }
    else if (info->algo_type == WC_ALGO_TYPE_NONE) {
        printf("Crypto CB: %s %s (%d)\n", GetAlgoTypeStr(info->algo_type),
            GetCryptoCbCmdTypeStr(info->cmd.type), info->cmd.type);
    }
    else {
        printf("CryptoCb: %s \n", GetAlgoTypeStr(info->algo_type));
    }
}
#endif /* DEBUG_CRYPTOCB */

/* Search through listed devices and return the first matching device ID
 * found. */
static CryptoCb* wc_CryptoCb_GetDevice(int devId)
{
    int i;
    for (i = 0; i < MAX_CRYPTO_DEVID_CALLBACKS; i++) {
        if (gCryptoDev[i].devId == devId)
            return &gCryptoDev[i];
    }
    return NULL;
}


/* Filters through find callback set when trying to get the device,
 * returns the device found on success and null if not found. */
static CryptoCb* wc_CryptoCb_FindDevice(int devId, int algoType)
{
    int localDevId = devId;

#ifdef WOLF_CRYPTO_CB_FIND
    if (CryptoCb_FindCb != NULL) {
        localDevId = CryptoCb_FindCb(devId, algoType);
    }
#endif /* WOLF_CRYPTO_CB_FIND */
    (void)algoType;
    return wc_CryptoCb_GetDevice(localDevId);
}


static CryptoCb* wc_CryptoCb_FindDeviceByIndex(int startIdx)
{
    int i;
    for (i=startIdx; i<MAX_CRYPTO_DEVID_CALLBACKS; i++) {
        if (gCryptoDev[i].devId != INVALID_DEVID)
            return &gCryptoDev[i];
    }
    return NULL;
}

static WC_INLINE int wc_CryptoCb_TranslateErrorCode(int ret)
{
    if (ret == NOT_COMPILED_IN) {
        /* backwards compatibility for older NOT_COMPILED_IN syntax */
        ret = CRYPTOCB_UNAVAILABLE;
    }
    return ret;
}

/* Helper function to reset a device entry to invalid */
static WC_INLINE void wc_CryptoCb_ClearDev(CryptoCb *dev)
{
    XMEMSET(dev, 0, sizeof(*dev));
    dev->devId = INVALID_DEVID;
}

void wc_CryptoCb_Init(void)
{
    int i;
    for (i = 0; i < MAX_CRYPTO_DEVID_CALLBACKS; i++) {
        wc_CryptoCb_ClearDev(&gCryptoDev[i]);
    }
}

void wc_CryptoCb_Cleanup(void)
{
    int i;
    for (i = 0; i < MAX_CRYPTO_DEVID_CALLBACKS; i++) {
        if(gCryptoDev[i].devId != INVALID_DEVID) {
            wc_CryptoCb_UnRegisterDevice(gCryptoDev[i].devId);
        }
    }
}

int wc_CryptoCb_GetDevIdAtIndex(int startIdx)
{
    int devId = INVALID_DEVID;
    CryptoCb* dev = wc_CryptoCb_FindDeviceByIndex(startIdx);
    if (dev) {
        devId = dev->devId;
    }
    return devId;
}


#ifdef WOLF_CRYPTO_CB_FIND
/* Used to register a find device function. Useful for cases where the
 * device ID in the struct may not have been set but still wanting to use
 * a specific crypto callback device ID. The find callback is global and
 * not thread safe. */
void wc_CryptoCb_SetDeviceFindCb(CryptoDevCallbackFind cb)
{
    CryptoCb_FindCb = cb;
}
#endif

int wc_CryptoCb_RegisterDevice(int devId, CryptoDevCallbackFunc cb, void* ctx)
{
    int rc = 0;

    /* find existing or new */
    CryptoCb* dev = wc_CryptoCb_GetDevice(devId);
    if (dev == NULL)
        dev = wc_CryptoCb_GetDevice(INVALID_DEVID);

    if (dev == NULL)
        return BUFFER_E; /* out of devices */

    dev->devId = devId;
    dev->cb    = cb;
    dev->ctx   = ctx;

#ifdef WOLF_CRYPTO_CB_CMD
    if (cb != NULL) {
        /* Invoke callback with register command */
        wc_CryptoInfo info;
        XMEMSET(&info, 0, sizeof(info));
        info.algo_type = WC_ALGO_TYPE_NONE;
        info.cmd.type  = WC_CRYPTOCB_CMD_TYPE_REGISTER;
        info.cmd.ctx   = ctx;  /* cb may update on success */

        rc = cb(devId, &info, ctx);
        if (rc == 0) {
            /* Success.  Update dev->ctx */
            dev->ctx = info.cmd.ctx;
        }
        else if ((rc == CRYPTOCB_UNAVAILABLE) ||
                 (rc == NOT_COMPILED_IN)) {
            /* Not implemented.  Return success*/
            rc = 0;
        }
        else {
            /* Error in callback register cmd. Don't register */
            wc_CryptoCb_ClearDev(dev);
        }
    }
#endif
    return rc;
}

void wc_CryptoCb_UnRegisterDevice(int devId)
{
    CryptoCb* dev = NULL;

    /* Can't unregister the invalid device */
    if (devId == INVALID_DEVID)
        return;

    /* Find the matching dev */
    dev = wc_CryptoCb_GetDevice(devId);
    if (dev == NULL)
        return;

#ifdef WOLF_CRYPTO_CB_CMD
    if (dev->cb != NULL) {
        /* Invoke callback with unregister command.*/
        wc_CryptoInfo info;
        XMEMSET(&info, 0, sizeof(info));
        info.algo_type = WC_ALGO_TYPE_NONE;
        info.cmd.type  = WC_CRYPTOCB_CMD_TYPE_UNREGISTER;
        info.cmd.ctx   = NULL;  /* Not used */

        /* Ignore errors here */
        dev->cb(devId, &info, dev->ctx);
    }
#endif
    wc_CryptoCb_ClearDev(dev);
}

#ifndef NO_RSA
int wc_CryptoCb_Rsa(const byte* in, word32 inLen, byte* out,
    word32* outLen, int type, RsaKey* key, WC_RNG* rng)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    if (key == NULL)
        return ret;

    /* locate registered callback */
    dev = wc_CryptoCb_FindDevice(key->devId, WC_ALGO_TYPE_PK);
    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_PK;
        cryptoInfo.pk.type = WC_PK_TYPE_RSA;
        cryptoInfo.pk.rsa.in = in;
        cryptoInfo.pk.rsa.inLen = inLen;
        cryptoInfo.pk.rsa.out = out;
        cryptoInfo.pk.rsa.outLen = outLen;
        cryptoInfo.pk.rsa.type = type;
        cryptoInfo.pk.rsa.key = key;
        cryptoInfo.pk.rsa.rng = rng;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}

#ifdef WOLFSSL_KEY_GEN
int wc_CryptoCb_MakeRsaKey(RsaKey* key, int size, long e, WC_RNG* rng)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    if (key == NULL)
        return ret;

    /* locate registered callback */
    dev = wc_CryptoCb_FindDevice(key->devId, WC_ALGO_TYPE_PK);
    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_PK;
        cryptoInfo.pk.type = WC_PK_TYPE_RSA_KEYGEN;
        cryptoInfo.pk.rsakg.key = key;
        cryptoInfo.pk.rsakg.size = size;
        cryptoInfo.pk.rsakg.e = e;
        cryptoInfo.pk.rsakg.rng = rng;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}
#endif

int wc_CryptoCb_RsaCheckPrivKey(RsaKey* key, const byte* pubKey,
    word32 pubKeySz)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    if (key == NULL)
        return ret;

    /* locate registered callback */
    dev = wc_CryptoCb_FindDevice(key->devId, WC_ALGO_TYPE_PK);
    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_PK;
        cryptoInfo.pk.type = WC_PK_TYPE_RSA_CHECK_PRIV_KEY;
        cryptoInfo.pk.rsa_check.key = key;
        cryptoInfo.pk.rsa_check.pubKey = pubKey;
        cryptoInfo.pk.rsa_check.pubKeySz = pubKeySz;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}

int wc_CryptoCb_RsaGetSize(const RsaKey* key, int* keySize)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    if (key == NULL)
        return ret;

    /* locate registered callback */
    dev = wc_CryptoCb_FindDevice(key->devId, WC_ALGO_TYPE_PK);
    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_PK;
        cryptoInfo.pk.type = WC_PK_TYPE_RSA_GET_SIZE;
        cryptoInfo.pk.rsa_get_size.key = key;
        cryptoInfo.pk.rsa_get_size.keySize = keySize;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}
#endif /* !NO_RSA */

#ifdef HAVE_ECC
int wc_CryptoCb_MakeEccKey(WC_RNG* rng, int keySize, ecc_key* key, int curveId)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    if (key == NULL)
        return ret;

    /* locate registered callback */
    dev = wc_CryptoCb_FindDevice(key->devId, WC_ALGO_TYPE_PK);
    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_PK;
        cryptoInfo.pk.type = WC_PK_TYPE_EC_KEYGEN;
        cryptoInfo.pk.eckg.rng = rng;
        cryptoInfo.pk.eckg.size = keySize;
        cryptoInfo.pk.eckg.key = key;
        cryptoInfo.pk.eckg.curveId = curveId;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}

int wc_CryptoCb_Ecdh(ecc_key* private_key, ecc_key* public_key,
    byte* out, word32* outlen)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    if (private_key == NULL)
        return ret;

    /* locate registered callback */
    dev = wc_CryptoCb_FindDevice(private_key->devId, WC_ALGO_TYPE_PK);
    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_PK;
        cryptoInfo.pk.type = WC_PK_TYPE_ECDH;
        cryptoInfo.pk.ecdh.private_key = private_key;
        cryptoInfo.pk.ecdh.public_key = public_key;
        cryptoInfo.pk.ecdh.out = out;
        cryptoInfo.pk.ecdh.outlen = outlen;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}

int wc_CryptoCb_EccSign(const byte* in, word32 inlen, byte* out,
    word32 *outlen, WC_RNG* rng, ecc_key* key)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    if (key == NULL)
        return ret;

    /* locate registered callback */
    dev = wc_CryptoCb_FindDevice(key->devId, WC_ALGO_TYPE_PK);
    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_PK;
        cryptoInfo.pk.type = WC_PK_TYPE_ECDSA_SIGN;
        cryptoInfo.pk.eccsign.in = in;
        cryptoInfo.pk.eccsign.inlen = inlen;
        cryptoInfo.pk.eccsign.out = out;
        cryptoInfo.pk.eccsign.outlen = outlen;
        cryptoInfo.pk.eccsign.rng = rng;
        cryptoInfo.pk.eccsign.key = key;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}

int wc_CryptoCb_EccVerify(const byte* sig, word32 siglen,
    const byte* hash, word32 hashlen, int* res, ecc_key* key)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    if (key == NULL)
        return ret;

    /* locate registered callback */
    dev = wc_CryptoCb_FindDevice(key->devId, WC_ALGO_TYPE_PK);
    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_PK;
        cryptoInfo.pk.type = WC_PK_TYPE_ECDSA_VERIFY;
        cryptoInfo.pk.eccverify.sig = sig;
        cryptoInfo.pk.eccverify.siglen = siglen;
        cryptoInfo.pk.eccverify.hash = hash;
        cryptoInfo.pk.eccverify.hashlen = hashlen;
        cryptoInfo.pk.eccverify.res = res;
        cryptoInfo.pk.eccverify.key = key;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}

int wc_CryptoCb_EccCheckPrivKey(ecc_key* key, const byte* pubKey,
    word32 pubKeySz)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    if (key == NULL)
        return ret;

    /* locate registered callback */
    dev = wc_CryptoCb_FindDevice(key->devId, WC_ALGO_TYPE_PK);
    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_PK;
        cryptoInfo.pk.type = WC_PK_TYPE_EC_CHECK_PRIV_KEY;
        cryptoInfo.pk.ecc_check.key = key;
        cryptoInfo.pk.ecc_check.pubKey = pubKey;
        cryptoInfo.pk.ecc_check.pubKeySz = pubKeySz;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}
#endif /* HAVE_ECC */

#ifdef HAVE_CURVE25519
int wc_CryptoCb_Curve25519Gen(WC_RNG* rng, int keySize,
    curve25519_key* key)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    if (key == NULL)
        return ret;

    /* locate registered callback */
    dev = wc_CryptoCb_FindDevice(key->devId, WC_ALGO_TYPE_PK);
    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_PK;
        cryptoInfo.pk.type = WC_PK_TYPE_CURVE25519_KEYGEN;
        cryptoInfo.pk.curve25519kg.rng = rng;
        cryptoInfo.pk.curve25519kg.size = keySize;
        cryptoInfo.pk.curve25519kg.key = key;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}

int wc_CryptoCb_Curve25519(curve25519_key* private_key,
    curve25519_key* public_key, byte* out, word32* outlen, int endian)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    if (private_key == NULL)
        return ret;

    /* locate registered callback */
    dev = wc_CryptoCb_FindDevice(private_key->devId, WC_ALGO_TYPE_PK);
    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_PK;
        cryptoInfo.pk.type = WC_PK_TYPE_CURVE25519;
        cryptoInfo.pk.curve25519.private_key = private_key;
        cryptoInfo.pk.curve25519.public_key = public_key;
        cryptoInfo.pk.curve25519.out = out;
        cryptoInfo.pk.curve25519.outlen = outlen;
        cryptoInfo.pk.curve25519.endian = endian;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}
#endif /* HAVE_CURVE25519 */

#ifdef HAVE_ED25519
int wc_CryptoCb_Ed25519Gen(WC_RNG* rng, int keySize,
    ed25519_key* key)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    if (key == NULL)
        return ret;

    /* locate registered callback */
    dev = wc_CryptoCb_FindDevice(key->devId, WC_ALGO_TYPE_PK);
    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_PK;
        cryptoInfo.pk.type = WC_PK_TYPE_ED25519_KEYGEN;
        cryptoInfo.pk.ed25519kg.rng = rng;
        cryptoInfo.pk.ed25519kg.size = keySize;
        cryptoInfo.pk.ed25519kg.key = key;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}

int wc_CryptoCb_Ed25519Sign(const byte* in, word32 inLen, byte* out,
                            word32 *outLen, ed25519_key* key, byte type,
                            const byte* context, byte contextLen)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    if (key == NULL)
        return ret;

    /* locate registered callback */
    dev = wc_CryptoCb_FindDevice(key->devId, WC_ALGO_TYPE_PK);
    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_PK;
        cryptoInfo.pk.type = WC_PK_TYPE_ED25519_SIGN;
        cryptoInfo.pk.ed25519sign.in = in;
        cryptoInfo.pk.ed25519sign.inLen = inLen;
        cryptoInfo.pk.ed25519sign.out = out;
        cryptoInfo.pk.ed25519sign.outLen = outLen;
        cryptoInfo.pk.ed25519sign.key = key;
        cryptoInfo.pk.ed25519sign.type = type;
        cryptoInfo.pk.ed25519sign.context = context;
        cryptoInfo.pk.ed25519sign.contextLen = contextLen;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}

int wc_CryptoCb_Ed25519Verify(const byte* sig, word32 sigLen,
    const byte* msg, word32 msgLen, int* res, ed25519_key* key, byte type,
    const byte* context, byte contextLen)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    if (key == NULL)
        return ret;

    /* locate registered callback */
    dev = wc_CryptoCb_FindDevice(key->devId, WC_ALGO_TYPE_PK);
    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_PK;
        cryptoInfo.pk.type = WC_PK_TYPE_ED25519_VERIFY;
        cryptoInfo.pk.ed25519verify.sig = sig;
        cryptoInfo.pk.ed25519verify.sigLen = sigLen;
        cryptoInfo.pk.ed25519verify.msg = msg;
        cryptoInfo.pk.ed25519verify.msgLen = msgLen;
        cryptoInfo.pk.ed25519verify.res = res;
        cryptoInfo.pk.ed25519verify.key = key;
        cryptoInfo.pk.ed25519verify.type = type;
        cryptoInfo.pk.ed25519verify.context = context;
        cryptoInfo.pk.ed25519verify.contextLen = contextLen;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}
#endif /* HAVE_ED25519 */

#ifndef NO_AES
#ifdef HAVE_AESGCM
int wc_CryptoCb_AesGcmEncrypt(Aes* aes, byte* out,
                               const byte* in, word32 sz,
                               const byte* iv, word32 ivSz,
                               byte* authTag, word32 authTagSz,
                               const byte* authIn, word32 authInSz)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    /* locate registered callback */
    if (aes) {
        dev = wc_CryptoCb_FindDevice(aes->devId, WC_ALGO_TYPE_CIPHER);
    }
    else {
        /* locate first callback and try using it */
        dev = wc_CryptoCb_FindDeviceByIndex(0);
    }

    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_CIPHER;
        cryptoInfo.cipher.type = WC_CIPHER_AES_GCM;
        cryptoInfo.cipher.enc = 1;
        cryptoInfo.cipher.aesgcm_enc.aes       = aes;
        cryptoInfo.cipher.aesgcm_enc.out       = out;
        cryptoInfo.cipher.aesgcm_enc.in        = in;
        cryptoInfo.cipher.aesgcm_enc.sz        = sz;
        cryptoInfo.cipher.aesgcm_enc.iv        = iv;
        cryptoInfo.cipher.aesgcm_enc.ivSz      = ivSz;
        cryptoInfo.cipher.aesgcm_enc.authTag   = authTag;
        cryptoInfo.cipher.aesgcm_enc.authTagSz = authTagSz;
        cryptoInfo.cipher.aesgcm_enc.authIn    = authIn;
        cryptoInfo.cipher.aesgcm_enc.authInSz  = authInSz;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}

int wc_CryptoCb_AesGcmDecrypt(Aes* aes, byte* out,
                               const byte* in, word32 sz,
                               const byte* iv, word32 ivSz,
                               const byte* authTag, word32 authTagSz,
                               const byte* authIn, word32 authInSz)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    /* locate registered callback */
    if (aes) {
        dev = wc_CryptoCb_FindDevice(aes->devId, WC_ALGO_TYPE_CIPHER);
    }
    else {
        /* locate first callback and try using it */
        dev = wc_CryptoCb_FindDeviceByIndex(0);
    }

    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_CIPHER;
        cryptoInfo.cipher.type = WC_CIPHER_AES_GCM;
        cryptoInfo.cipher.enc = 0;
        cryptoInfo.cipher.aesgcm_dec.aes       = aes;
        cryptoInfo.cipher.aesgcm_dec.out       = out;
        cryptoInfo.cipher.aesgcm_dec.in        = in;
        cryptoInfo.cipher.aesgcm_dec.sz        = sz;
        cryptoInfo.cipher.aesgcm_dec.iv        = iv;
        cryptoInfo.cipher.aesgcm_dec.ivSz      = ivSz;
        cryptoInfo.cipher.aesgcm_dec.authTag   = authTag;
        cryptoInfo.cipher.aesgcm_dec.authTagSz = authTagSz;
        cryptoInfo.cipher.aesgcm_dec.authIn    = authIn;
        cryptoInfo.cipher.aesgcm_dec.authInSz  = authInSz;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}
#endif /* HAVE_AESGCM */

#ifdef HAVE_AESCCM
int wc_CryptoCb_AesCcmEncrypt(Aes* aes, byte* out,
                               const byte* in, word32 sz,
                               const byte* nonce, word32 nonceSz,
                               byte* authTag, word32 authTagSz,
                               const byte* authIn, word32 authInSz)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    /* locate registered callback */
    if (aes) {
        dev = wc_CryptoCb_FindDevice(aes->devId, WC_ALGO_TYPE_CIPHER);
    }
    else {
        /* locate first callback and try using it */
        dev = wc_CryptoCb_FindDeviceByIndex(0);
    }

    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_CIPHER;
        cryptoInfo.cipher.type = WC_CIPHER_AES_CCM;
        cryptoInfo.cipher.enc = 1;
        cryptoInfo.cipher.aesccm_enc.aes       = aes;
        cryptoInfo.cipher.aesccm_enc.out       = out;
        cryptoInfo.cipher.aesccm_enc.in        = in;
        cryptoInfo.cipher.aesccm_enc.sz        = sz;
        cryptoInfo.cipher.aesccm_enc.nonce     = nonce;
        cryptoInfo.cipher.aesccm_enc.nonceSz   = nonceSz;
        cryptoInfo.cipher.aesccm_enc.authTag   = authTag;
        cryptoInfo.cipher.aesccm_enc.authTagSz = authTagSz;
        cryptoInfo.cipher.aesccm_enc.authIn    = authIn;
        cryptoInfo.cipher.aesccm_enc.authInSz  = authInSz;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}

int wc_CryptoCb_AesCcmDecrypt(Aes* aes, byte* out,
                               const byte* in, word32 sz,
                               const byte* nonce, word32 nonceSz,
                               const byte* authTag, word32 authTagSz,
                               const byte* authIn, word32 authInSz)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    /* locate registered callback */
    if (aes) {
        dev = wc_CryptoCb_FindDevice(aes->devId, WC_ALGO_TYPE_CIPHER);
    }
    else {
        /* locate first callback and try using it */
        dev = wc_CryptoCb_FindDeviceByIndex(0);
    }

    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_CIPHER;
        cryptoInfo.cipher.type = WC_CIPHER_AES_CCM;
        cryptoInfo.cipher.enc = 0;
        cryptoInfo.cipher.aesccm_dec.aes       = aes;
        cryptoInfo.cipher.aesccm_dec.out       = out;
        cryptoInfo.cipher.aesccm_dec.in        = in;
        cryptoInfo.cipher.aesccm_dec.sz        = sz;
        cryptoInfo.cipher.aesccm_dec.nonce     = nonce;
        cryptoInfo.cipher.aesccm_dec.nonceSz   = nonceSz;
        cryptoInfo.cipher.aesccm_dec.authTag   = authTag;
        cryptoInfo.cipher.aesccm_dec.authTagSz = authTagSz;
        cryptoInfo.cipher.aesccm_dec.authIn    = authIn;
        cryptoInfo.cipher.aesccm_dec.authInSz  = authInSz;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}
#endif /* HAVE_AESCCM */

#ifdef HAVE_AES_CBC
int wc_CryptoCb_AesCbcEncrypt(Aes* aes, byte* out,
                               const byte* in, word32 sz)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    /* locate registered callback */
    if (aes) {
        dev = wc_CryptoCb_FindDevice(aes->devId, WC_ALGO_TYPE_CIPHER);
    }
    else {
        /* locate first callback and try using it */
        dev = wc_CryptoCb_FindDeviceByIndex(0);
    }

    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_CIPHER;
        cryptoInfo.cipher.type = WC_CIPHER_AES_CBC;
        cryptoInfo.cipher.enc = 1;
        cryptoInfo.cipher.aescbc.aes = aes;
        cryptoInfo.cipher.aescbc.out = out;
        cryptoInfo.cipher.aescbc.in = in;
        cryptoInfo.cipher.aescbc.sz = sz;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}

int wc_CryptoCb_AesCbcDecrypt(Aes* aes, byte* out,
                               const byte* in, word32 sz)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    /* locate registered callback */
    if (aes) {
        dev = wc_CryptoCb_FindDevice(aes->devId, WC_ALGO_TYPE_CIPHER);
    }
    else {
        /* locate first callback and try using it */
        dev = wc_CryptoCb_FindDeviceByIndex(0);
    }

    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_CIPHER;
        cryptoInfo.cipher.type = WC_CIPHER_AES_CBC;
        cryptoInfo.cipher.enc = 0;
        cryptoInfo.cipher.aescbc.aes = aes;
        cryptoInfo.cipher.aescbc.out = out;
        cryptoInfo.cipher.aescbc.in = in;
        cryptoInfo.cipher.aescbc.sz = sz;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}
#endif /* HAVE_AES_CBC */
#ifdef WOLFSSL_AES_COUNTER
int wc_CryptoCb_AesCtrEncrypt(Aes* aes, byte* out,
                               const byte* in, word32 sz)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    /* locate registered callback */
    if (aes) {
        dev = wc_CryptoCb_FindDevice(aes->devId, WC_ALGO_TYPE_CIPHER);
    }
    else {
        /* locate first callback and try using it */
        dev = wc_CryptoCb_FindDeviceByIndex(0);
    }

    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_CIPHER;
        cryptoInfo.cipher.type = WC_CIPHER_AES_CTR;
        cryptoInfo.cipher.enc = 1;
        cryptoInfo.cipher.aesctr.aes = aes;
        cryptoInfo.cipher.aesctr.out = out;
        cryptoInfo.cipher.aesctr.in = in;
        cryptoInfo.cipher.aesctr.sz = sz;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}
#endif /* WOLFSSL_AES_COUNTER */
#ifdef HAVE_AES_ECB
int wc_CryptoCb_AesEcbEncrypt(Aes* aes, byte* out,
                               const byte* in, word32 sz)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    /* locate registered callback */
    if (aes) {
        dev = wc_CryptoCb_FindDevice(aes->devId, WC_ALGO_TYPE_CIPHER);
    }
    else {
        /* locate first callback and try using it */
        dev = wc_CryptoCb_FindDeviceByIndex(0);
    }

    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_CIPHER;
        cryptoInfo.cipher.type = WC_CIPHER_AES_ECB;
        cryptoInfo.cipher.enc = 1;
        cryptoInfo.cipher.aesecb.aes = aes;
        cryptoInfo.cipher.aesecb.out = out;
        cryptoInfo.cipher.aesecb.in = in;
        cryptoInfo.cipher.aesecb.sz = sz;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}

int wc_CryptoCb_AesEcbDecrypt(Aes* aes, byte* out,
                               const byte* in, word32 sz)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    /* locate registered callback */
    if (aes) {
        dev = wc_CryptoCb_FindDevice(aes->devId, WC_ALGO_TYPE_CIPHER);
    }
    else {
        /* locate first callback and try using it */
        dev = wc_CryptoCb_FindDeviceByIndex(0);
    }

    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_CIPHER;
        cryptoInfo.cipher.type = WC_CIPHER_AES_ECB;
        cryptoInfo.cipher.enc = 0;
        cryptoInfo.cipher.aesecb.aes = aes;
        cryptoInfo.cipher.aesecb.out = out;
        cryptoInfo.cipher.aesecb.in = in;
        cryptoInfo.cipher.aesecb.sz = sz;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}
#endif /* HAVE_AES_ECB */
#endif /* !NO_AES */

#ifndef NO_DES3
int wc_CryptoCb_Des3Encrypt(Des3* des3, byte* out,
                               const byte* in, word32 sz)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    /* locate registered callback */
    if (des3) {
        dev = wc_CryptoCb_FindDevice(des3->devId, WC_ALGO_TYPE_CIPHER);
    }
    else {
        /* locate first callback and try using it */
        dev = wc_CryptoCb_FindDeviceByIndex(0);
    }

    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_CIPHER;
        cryptoInfo.cipher.type = WC_CIPHER_DES3;
        cryptoInfo.cipher.enc = 1;
        cryptoInfo.cipher.des3.des = des3;
        cryptoInfo.cipher.des3.out = out;
        cryptoInfo.cipher.des3.in = in;
        cryptoInfo.cipher.des3.sz = sz;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}

int wc_CryptoCb_Des3Decrypt(Des3* des3, byte* out,
                               const byte* in, word32 sz)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    /* locate registered callback */
    if (des3) {
        dev = wc_CryptoCb_FindDevice(des3->devId, WC_ALGO_TYPE_CIPHER);
    }
    else {
        /* locate first callback and try using it */
        dev = wc_CryptoCb_FindDeviceByIndex(0);
    }

    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_CIPHER;
        cryptoInfo.cipher.type = WC_CIPHER_DES3;
        cryptoInfo.cipher.enc = 0;
        cryptoInfo.cipher.des3.des = des3;
        cryptoInfo.cipher.des3.out = out;
        cryptoInfo.cipher.des3.in = in;
        cryptoInfo.cipher.des3.sz = sz;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}
#endif /* !NO_DES3 */

#ifndef NO_SHA
int wc_CryptoCb_ShaHash(wc_Sha* sha, const byte* in,
    word32 inSz, byte* digest)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    /* locate registered callback */
    if (sha) {
        dev = wc_CryptoCb_FindDevice(sha->devId, WC_ALGO_TYPE_HASH);
    }
    else {
        /* locate first callback and try using it */
        dev = wc_CryptoCb_FindDeviceByIndex(0);
    }

    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_HASH;
        cryptoInfo.hash.type = WC_HASH_TYPE_SHA;
        cryptoInfo.hash.sha1 = sha;
        cryptoInfo.hash.in = in;
        cryptoInfo.hash.inSz = inSz;
        cryptoInfo.hash.digest = digest;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}
#endif /* !NO_SHA */

#ifndef NO_SHA256
int wc_CryptoCb_Sha256Hash(wc_Sha256* sha256, const byte* in,
    word32 inSz, byte* digest)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    /* locate registered callback */
    if (sha256) {
        dev = wc_CryptoCb_FindDevice(sha256->devId, WC_ALGO_TYPE_HASH);
    }
    else {
        /* locate first callback and try using it */
        dev = wc_CryptoCb_FindDeviceByIndex(0);
    }

    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_HASH;
        cryptoInfo.hash.type = WC_HASH_TYPE_SHA256;
        cryptoInfo.hash.sha256 = sha256;
        cryptoInfo.hash.in = in;
        cryptoInfo.hash.inSz = inSz;
        cryptoInfo.hash.digest = digest;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}
#endif /* !NO_SHA256 */

#ifdef WOLFSSL_SHA384
int wc_CryptoCb_Sha384Hash(wc_Sha384* sha384, const byte* in,
    word32 inSz, byte* digest)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    /* locate registered callback */
    #ifndef NO_SHA2_CRYPTO_CB
    if (sha384) {
        dev = wc_CryptoCb_FindDevice(sha384->devId, WC_ALGO_TYPE_HASH);
    }
    else
    #endif
    {
        /* locate first callback and try using it */
        dev = wc_CryptoCb_FindDeviceByIndex(0);
    }

    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_HASH;
        cryptoInfo.hash.type = WC_HASH_TYPE_SHA384;
        cryptoInfo.hash.sha384 = sha384;
        cryptoInfo.hash.in = in;
        cryptoInfo.hash.inSz = inSz;
        cryptoInfo.hash.digest = digest;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}
#endif /* WOLFSSL_SHA384 */

#ifdef WOLFSSL_SHA512
int wc_CryptoCb_Sha512Hash(wc_Sha512* sha512, const byte* in,
    word32 inSz, byte* digest)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    /* locate registered callback */
    #ifndef NO_SHA2_CRYPTO_CB
    if (sha512) {
        dev = wc_CryptoCb_FindDevice(sha512->devId, WC_ALGO_TYPE_HASH);
    }
    else
    #endif
    {
        /* locate first callback and try using it */
        dev = wc_CryptoCb_FindDeviceByIndex(0);
    }

    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_HASH;
        cryptoInfo.hash.type = WC_HASH_TYPE_SHA512;
        cryptoInfo.hash.sha512 = sha512;
        cryptoInfo.hash.in = in;
        cryptoInfo.hash.inSz = inSz;
        cryptoInfo.hash.digest = digest;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}
#endif /* WOLFSSL_SHA512 */

#ifndef NO_HMAC
int wc_CryptoCb_Hmac(Hmac* hmac, int macType, const byte* in, word32 inSz,
    byte* digest)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    if (hmac == NULL)
        return ret;

    /* locate registered callback */
    dev = wc_CryptoCb_FindDevice(hmac->devId, WC_ALGO_TYPE_HMAC);
    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_HMAC;
        cryptoInfo.hmac.macType = macType;
        cryptoInfo.hmac.in = in;
        cryptoInfo.hmac.inSz = inSz;
        cryptoInfo.hmac.digest = digest;
        cryptoInfo.hmac.hmac = hmac;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}
#endif /* !NO_HMAC */

#ifndef WC_NO_RNG
int wc_CryptoCb_RandomBlock(WC_RNG* rng, byte* out, word32 sz)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    /* locate registered callback */
    if (rng) {
        dev = wc_CryptoCb_FindDevice(rng->devId, WC_ALGO_TYPE_RNG);
    }
    else {
        /* locate first callback and try using it */
        dev = wc_CryptoCb_FindDeviceByIndex(0);
    }

    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_RNG;
        cryptoInfo.rng.rng = rng;
        cryptoInfo.rng.out = out;
        cryptoInfo.rng.sz = sz;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}

int wc_CryptoCb_RandomSeed(OS_Seed* os, byte* seed, word32 sz)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    /* locate registered callback */
    dev = wc_CryptoCb_FindDevice(os->devId, WC_ALGO_TYPE_SEED);
    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_SEED;
        cryptoInfo.seed.os = os;
        cryptoInfo.seed.seed = seed;
        cryptoInfo.seed.sz = sz;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}
#endif /* !WC_NO_RNG */
#ifdef WOLFSSL_CMAC
int wc_CryptoCb_Cmac(Cmac* cmac, const byte* key, word32 keySz,
        const byte* in, word32 inSz, byte* out, word32* outSz, int type,
        void* ctx)
{
    int ret = CRYPTOCB_UNAVAILABLE;
    CryptoCb* dev;

    /* locate registered callback */
    if (cmac) {
        dev = wc_CryptoCb_FindDevice(cmac->devId, WC_ALGO_TYPE_CMAC);
    }
    else {
        /* locate first callback and try using it */
        dev = wc_CryptoCb_FindDeviceByIndex(0);
    }

    if (dev && dev->cb) {
        wc_CryptoInfo cryptoInfo;
        XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
        cryptoInfo.algo_type = WC_ALGO_TYPE_CMAC;

        cryptoInfo.cmac.cmac  = cmac;
        cryptoInfo.cmac.ctx   = ctx;
        cryptoInfo.cmac.key   = key;
        cryptoInfo.cmac.in    = in;
        cryptoInfo.cmac.out   = out;
        cryptoInfo.cmac.outSz = outSz;
        cryptoInfo.cmac.keySz = keySz;
        cryptoInfo.cmac.inSz  = inSz;
        cryptoInfo.cmac.type  = type;

        ret = dev->cb(dev->devId, &cryptoInfo, dev->ctx);
    }

    return wc_CryptoCb_TranslateErrorCode(ret);
}
#endif

/* returns the default dev id for the current build */
int wc_CryptoCb_DefaultDevID(void)
{
    int ret;

    /* conditional macro selection based on build */
#ifdef WOLFSSL_CAAM_DEVID
    ret = WOLFSSL_CAAM_DEVID;
#elif defined(HAVE_ARIA)
    ret = WOLFSSL_ARIA_DEVID;
#elif defined(WC_USE_DEVID)
    ret = WC_USE_DEVID;
#else
    ret = INVALID_DEVID;
#endif

    return ret;
}

#endif /* WOLF_CRYPTO_CB */
