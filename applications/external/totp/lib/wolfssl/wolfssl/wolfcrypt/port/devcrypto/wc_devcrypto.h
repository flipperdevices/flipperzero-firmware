/* wc_devcrypto.h
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


#ifndef WOLFSSL_DEVCRYPTO_H
#define WOLFSSL_DEVCRYPTO_H

#include <wolfssl/wolfcrypt/types.h>

#ifdef WOLFSSL_DEVCRYPTO

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <crypto/cryptodev.h>

#ifdef DEBUG_DEVCRYPTO
#include <stdio.h>
#endif

typedef struct WC_CRYPTODEV {
    int cfd;
    struct session_op sess;
} WC_CRYPTODEV;

WOLFSSL_LOCAL int wc_DevCryptoCreate(WC_CRYPTODEV* ctx, int type, byte* key, word32 keySz);
WOLFSSL_LOCAL void wc_DevCryptoFree(WC_CRYPTODEV* ctx);
WOLFSSL_LOCAL void wc_SetupCrypt(struct crypt_op* crt, WC_CRYPTODEV* dev,
        byte* src, int srcSz, byte* dst, byte* dig, int flag, int op);
WOLFSSL_LOCAL void wc_SetupCryptSym(struct crypt_op* crt, WC_CRYPTODEV* dev,
        byte* src, word32 srcSz, byte* dst, byte* iv, int flag);
WOLFSSL_LOCAL void wc_SetupCryptAead(struct crypt_auth_op* crt, WC_CRYPTODEV* dev,
         byte* src, word32 srcSz, byte* dst, byte* iv, word32 ivSz, int flag,
         byte* authIn, word32 authInSz, byte* authTag, word32 authTagSz);

WOLFSSL_LOCAL int wc_DevCryptoInit(void);
WOLFSSL_LOCAL void wc_DevCryptoCleanup(void);

/* currently local API */
#if defined(WOLFSSL_DEVCRYPTO_HMAC)
#ifndef WC_HMAC_TYPE_DEFINED
    typedef struct Hmac Hmac;
    #define WC_HMAC_TYPE_DEFINED
#endif
WOLFSSL_LOCAL int wc_DevCrypto_HmacSetKey(Hmac* hmac, int t, const byte* key,
        word32 keySz);
WOLFSSL_LOCAL int wc_DevCrypto_HmacUpdate(Hmac* hmac, const byte* input,
        word32 inputSz);
WOLFSSL_LOCAL int wc_DevCrypto_HmacFinal(Hmac* hmac, byte* out);
WOLFSSL_LOCAL int wc_DevCrypto_HmacInit(Hmac* hmac, void* heap, int devId);
WOLFSSL_LOCAL void wc_DevCrypto_HmacFree(Hmac* hmac);
#endif /* WOLFSSL_DEVCRYPTO_HMAC */

#if defined(WOLFSSL_DEVCRYPTO_RSA)
#ifndef WC_RSAKEY_TYPE_DEFINED
    typedef struct RsaKey RsaKey;
    #define WC_RSAKEY_TYPE_DEFINED
#endif
#ifndef WC_RNG_TYPE_DEFINED /* guard on redeclaration */
    typedef struct OS_Seed OS_Seed;
    typedef struct WC_RNG WC_RNG;
    #ifdef WC_RNG_SEED_CB
        typedef int (*wc_RngSeed_Cb)(OS_Seed* os, byte* seed, word32 sz);
    #endif
    #define WC_RNG_TYPE_DEFINED
#endif
WOLFSSL_LOCAL int wc_DevCrypto_RsaEncrypt(const byte* in, word32 inlen, byte* out,
        word32* outlen, RsaKey *key, int type);
WOLFSSL_LOCAL int wc_DevCrypto_RsaDecrypt(const byte* in, word32 inlen,
        byte* out, word32 outlen, RsaKey* key, int type);
WOLFSSL_LOCAL int wc_DevCrypto_MakeRsaKey(RsaKey* key, int size, long e, WC_RNG* rng);
WOLFSSL_LOCAL void wc_DevCrypto_RsaFree(RsaKey* key);
#endif /* WOLFSSL_DEVCRYPTO_RSA */

#if defined(WOLFSSL_DEVCRYPTO_CURVE25519)
WOLFSSL_LOCAL int wc_DevCryptoCurve25519(byte* out, word32 outSz, const byte* k,
    word32 kSz, const byte* a, word32 aSz, int endian);
#endif

#if defined(WOLFSSL_DEVCRYPTO_ECDSA)
int wc_DevCryptoEccKeyGen(int curveId, int enc, byte* pri, word32 priSz, byte* pub, word32 pubSz);
int wc_DevCryptoEccEcdh(int curveId, int enc, byte* pri, word32 priSz,
    byte* pub, word32 pubSz, byte* out, word32 outSz);
int wc_DevCryptoEccSign(int curveId, int enc, byte* pri, word32 priSz,
    const byte* hash, word32 hashSz, byte* r, word32 rSz, byte* s, word32 sSz);
int wc_DevCryptoEccVerify(int curveId, byte* pub, word32 pubSz,
    const byte* hash, word32 hashSz, byte* r, word32 rSz, byte* s, word32 sSz);
#endif

#endif /* WOLFSSL_DEVCRYPTO */
#endif /* WOLFSSL_DEVCRYPTO_H */


