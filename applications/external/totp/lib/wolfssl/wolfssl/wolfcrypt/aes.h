/* aes.h
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

/*!
    \file wolfssl/wolfcrypt/aes.h
*/
/*

DESCRIPTION
This library provides the interfaces to the Advanced Encryption Standard (AES)
for encrypting and decrypting data. AES is the standard known for a symmetric
block cipher mechanism that uses n-bit binary string parameter key with 128-bits,
192-bits, and 256-bits of key sizes.

*/
#ifndef WOLF_CRYPT_AES_H
#define WOLF_CRYPT_AES_H

#include <wolfssl/wolfcrypt/types.h>

#if !defined(NO_AES) || defined(WOLFSSL_SM4)
typedef struct Gcm {
    ALIGN16 byte H[16];
#ifdef OPENSSL_EXTRA
    word32 aadH[4]; /* additional authenticated data GHASH */
    word32 aadLen;  /* additional authenticated data len */
#endif
#ifdef GCM_TABLE
    /* key-based fast multiplication table. */
    ALIGN16 byte M0[256][16];
#elif defined(GCM_TABLE_4BIT)
    #if defined(BIG_ENDIAN_ORDER) || defined(WC_16BIT_CPU)
        ALIGN16 byte M0[16][16];
    #else
        ALIGN16 byte M0[32][16];
    #endif
#endif /* GCM_TABLE */
} Gcm;

WOLFSSL_LOCAL void GenerateM0(Gcm* gcm);
#ifdef WOLFSSL_ARMASM
WOLFSSL_LOCAL void GMULT(byte* X, byte* Y);
#endif
WOLFSSL_LOCAL void GHASH(Gcm* gcm, const byte* a, word32 aSz, const byte* c,
                         word32 cSz, byte* s, word32 sSz);
#endif

#ifndef NO_AES

#if defined(HAVE_FIPS) && \
    defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2)
    #include <wolfssl/wolfcrypt/fips.h>
#endif /* HAVE_FIPS_VERSION >= 2 */

#ifndef WC_NO_RNG
    #include <wolfssl/wolfcrypt/random.h>
#endif
#ifdef STM32_CRYPTO
    #include <wolfssl/wolfcrypt/port/st/stm32.h>
#endif

#ifdef WOLFSSL_IMXRT_DCP
    #include "fsl_dcp.h"
#endif

#ifdef WOLFSSL_XILINX_CRYPT
#ifdef WOLFSSL_XILINX_CRYPT_VERSAL
#include <wolfssl/wolfcrypt/port/xilinx/xil-versal-glue.h>
#include <xsecure_aesclient.h>
#define WOLFSSL_XILINX_AES_KEY_SRC XSECURE_AES_USER_KEY_0
#else /* versal */
#include <xsecure_aes.h>
#define WOLFSSL_XILINX_AES_KEY_SRC XSECURE_CSU_AES_KEY_SRC_KUP
#endif /* !versal */
#endif /* WOLFSSL_XILINX_CRYPT */

#if defined(WOLFSSL_XILINX_CRYPT) || defined(WOLFSSL_AFALG_XILINX_AES)
#if !defined(WOLFSSL_XILINX_AES_KEY_SRC)
#define WOLFSSL_XILINX_AES_KEY_SRC 0
#endif /* !defined(WOLFSSL_XILINX_AES_KEY_SRC) */
#endif /* all Xilinx crypto */

#ifdef WOLFSSL_SE050
    #include <wolfssl/wolfcrypt/port/nxp/se050_port.h>
#endif

#if defined(WOLFSSL_AFALG) || defined(WOLFSSL_AFALG_XILINX_AES)
/* included for struct msghdr */
#include <wolfssl/wolfcrypt/port/af_alg/wc_afalg.h>
#endif

#if defined(WOLFSSL_KCAPI_AES)
#include <wolfssl/wolfcrypt/port/kcapi/wc_kcapi.h>
#endif

#if defined(WOLFSSL_DEVCRYPTO_AES) || defined(WOLFSSL_DEVCRYPTO_CBC)
#include <wolfssl/wolfcrypt/port/devcrypto/wc_devcrypto.h>
#endif

#ifdef WOLFSSL_SILABS_SE_ACCEL
    #include <wolfssl/wolfcrypt/port/silabs/silabs_aes.h>
#endif


#if defined(HAVE_AESGCM) && !defined(WC_NO_RNG)
    #include <wolfssl/wolfcrypt/random.h>
#endif

#if defined(WOLFSSL_HAVE_PSA) && !defined(WOLFSSL_PSA_NO_AES)
#include <psa/crypto.h>
#endif

#if defined(WOLFSSL_CRYPTOCELL)
    #include <wolfssl/wolfcrypt/port/arm/cryptoCell.h>
#endif

#if (defined(WOLFSSL_RENESAS_TSIP_TLS) && \
    defined(WOLFSSL_RENESAS_TSIP_TLS_AES_CRYPT)) ||\
    defined(WOLFSSL_RENESAS_TSIP_CRYPTONLY)
    #include <wolfssl/wolfcrypt/port/Renesas/renesas_tsip_types.h>
#endif

#ifdef WOLFSSL_MAXQ10XX_CRYPTO
    #include <wolfssl/wolfcrypt/port/maxim/maxq10xx.h>
#endif

#ifdef __cplusplus
    extern "C" {
#endif

#ifndef WOLFSSL_AES_KEY_SIZE_ENUM
#define WOLFSSL_AES_KEY_SIZE_ENUM
/* these are required for FIPS and non-FIPS */
enum {
    AES_128_KEY_SIZE    = 16,  /* for 128 bit             */
    AES_192_KEY_SIZE    = 24,  /* for 192 bit             */
    AES_256_KEY_SIZE    = 32,  /* for 256 bit             */

    AES_IV_SIZE         = 16  /* always block size       */
};
#endif

/* avoid redefinition of structs */
#if !defined(HAVE_FIPS) || \
    (defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2))

#ifdef WOLFSSL_ASYNC_CRYPT
    #include <wolfssl/wolfcrypt/async.h>
#endif

enum {
    AES_ENC_TYPE   = WC_CIPHER_AES,   /* cipher unique type */
    AES_ENCRYPTION = 0,
    AES_DECRYPTION = 1,

    AES_BLOCK_SIZE      = 16,

    KEYWRAP_BLOCK_SIZE  = 8,

    GCM_NONCE_MAX_SZ = 16, /* wolfCrypt's maximum nonce size allowed. */
    GCM_NONCE_MID_SZ = 12, /* The default nonce size for AES-GCM. */
    GCM_NONCE_MIN_SZ = 8,  /* wolfCrypt's minimum nonce size allowed. */
    CCM_NONCE_MIN_SZ = 7,
    CCM_NONCE_MAX_SZ = 13,
    CTR_SZ   = 4,
    AES_IV_FIXED_SZ = 4,
#ifdef WOLFSSL_AES_CFB
    AES_CFB_MODE = 1,
#endif
#ifdef WOLFSSL_AES_OFB
    AES_OFB_MODE = 2,
#endif
#ifdef WOLFSSL_AES_XTS
    AES_XTS_MODE = 3,
#endif

#ifdef WOLF_PRIVATE_KEY_ID
    AES_MAX_ID_LEN      = 32,
    AES_MAX_LABEL_LEN   = 32,
#endif

    WOLF_ENUM_DUMMY_LAST_ELEMENT(AES)
};

struct Aes {
    /* AESNI needs key first, rounds 2nd, not sure why yet */
    ALIGN16 word32 key[60];
    word32  rounds;
    int     keylen;

    ALIGN16 word32 reg[AES_BLOCK_SIZE / sizeof(word32)];      /* for CBC mode */
    ALIGN16 word32 tmp[AES_BLOCK_SIZE / sizeof(word32)];      /* same         */

#if defined(HAVE_AESGCM) || defined(HAVE_AESCCM)
    word32 invokeCtr[2];
    word32 nonceSz;
#endif
#ifdef HAVE_AESGCM
    Gcm gcm;

#ifdef WOLFSSL_SE050
    sss_symmetric_t aes_ctx; /* used as the function context */
    int ctxInitDone;
    word32 keyId;
    byte   keyIdSet;
    byte   useSWCrypt; /* Use SW crypt instead of SE050, before SCP03 auth */
#endif
#ifdef HAVE_CAVIUM_OCTEON_SYNC
    word32 y0;
#endif
#endif /* HAVE_AESGCM */
#ifdef WOLFSSL_CAAM
    int blackKey; /* black key / hsm key id */
#endif
#ifdef WOLFSSL_AESNI
    byte use_aesni;
#endif /* WOLFSSL_AESNI */
#ifdef WOLF_CRYPTO_CB
    int    devId;
    void*  devCtx;
#endif
#ifdef WOLF_PRIVATE_KEY_ID
    byte id[AES_MAX_ID_LEN];
    int  idLen;
    char label[AES_MAX_LABEL_LEN];
    int  labelLen;
#endif
#ifdef WOLFSSL_ASYNC_CRYPT
    WC_ASYNC_DEV asyncDev;
#endif /* WOLFSSL_ASYNC_CRYPT */
#if defined(WOLFSSL_AES_COUNTER) || defined(WOLFSSL_AES_CFB) || \
    defined(WOLFSSL_AES_OFB) || defined(WOLFSSL_AES_XTS)
    word32  left;            /* unused bytes left from last call */
#endif
#ifdef WOLFSSL_XILINX_CRYPT
#ifdef WOLFSSL_XILINX_CRYPT_VERSAL
    wc_Xsecure          xSec;
    XSecure_AesKeySize  xKeySize;
    int                 aadStyle;
    byte                keyInit[WOLFSSL_XSECURE_AES_KEY_SIZE] ALIGN64;
#else
    XSecure_Aes xilAes;
    XCsuDma     dma;
    word32      keyInit[8];
#endif
    word32      kup;
#endif
#if defined(WOLFSSL_AFALG) || defined(WOLFSSL_AFALG_XILINX_AES)
    int alFd; /* server socket to bind to */
    int rdFd; /* socket to read from */
    struct msghdr msg;
    int dir;  /* flag for encrpyt or decrypt */
#ifdef WOLFSSL_AFALG_XILINX_AES
    word32 msgBuf[CMSG_SPACE(4) + CMSG_SPACE(sizeof(struct af_alg_iv) +
                  GCM_NONCE_MID_SZ)];
#endif
#endif
#if defined(WOLFSSL_KCAPI_AES)
    struct kcapi_handle* handle;
    int                  init;
#endif
#if defined(WOLF_CRYPTO_CB) || (defined(WOLFSSL_DEVCRYPTO) && \
    (defined(WOLFSSL_DEVCRYPTO_AES) || defined(WOLFSSL_DEVCRYPTO_CBC))) || \
    (defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_AES)) || \
    defined(WOLFSSL_KCAPI_AES)
    word32 devKey[AES_MAX_KEY_SIZE/WOLFSSL_BIT_SIZE/sizeof(word32)]; /* raw key */
#ifdef HAVE_CAVIUM_OCTEON_SYNC
    int    keySet;
#endif
#endif
#if defined(WOLFSSL_DEVCRYPTO) && \
    (defined(WOLFSSL_DEVCRYPTO_AES) || defined(WOLFSSL_DEVCRYPTO_CBC))
    WC_CRYPTODEV ctx;
#endif
#if defined(WOLFSSL_CRYPTOCELL)
    aes_context_t ctx;
#endif
#if (defined(WOLFSSL_RENESAS_TSIP_TLS) && \
    defined(WOLFSSL_RENESAS_TSIP_TLS_AES_CRYPT)) ||\
    defined(WOLFSSL_RENESAS_TSIP_CRYPTONLY)
    TSIP_AES_CTX ctx;
#endif
#if defined(WOLFSSL_RENESAS_SCEPROTECT) ||\
    defined(WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY)
    SCE_AES_CTX ctx;
#endif
#if defined(WOLFSSL_IMXRT_DCP)
    dcp_handle_t handle;
#endif
#if defined(WOLFSSL_SILABS_SE_ACCEL)
    silabs_aes_t ctx;
#endif
#ifdef WOLFSSL_MAXQ10XX_CRYPTO
    maxq_aes_t maxq_ctx;
#endif
#if defined(WOLFSSL_HAVE_PSA) && !defined(WOLFSSL_PSA_NO_AES)
    psa_key_id_t key_id;
    psa_cipher_operation_t psa_ctx;
    int ctx_initialized;
    int key_need_importing;
#endif
    void*  heap; /* memory hint to use */
#ifdef WOLFSSL_AESGCM_STREAM
#if !defined(WOLFSSL_SMALL_STACK) || defined(WOLFSSL_AESNI)
    ALIGN16 byte streamData[5 * AES_BLOCK_SIZE];
#else
    byte*        streamData;
#endif
    word32       aSz;
    word32       cSz;
    byte         over;
    byte         aOver;
    byte         cOver;
    byte         gcmKeySet:1;
    byte         nonceSet:1;
    byte         ctrSet:1;
#endif
};

#ifndef WC_AES_TYPE_DEFINED
    typedef struct Aes Aes;
    #define WC_AES_TYPE_DEFINED
#endif

#ifdef WOLFSSL_AES_XTS
typedef struct XtsAes {
    Aes aes;
    Aes tweak;
} XtsAes;
#endif

#ifdef HAVE_AESGCM
typedef struct Gmac {
    Aes aes;
} Gmac;
#endif /* HAVE_AESGCM */
#endif /* HAVE_FIPS */


/* Authenticate cipher function prototypes */
typedef int (*wc_AesAuthEncryptFunc)(Aes* aes, byte* out,
                                   const byte* in, word32 sz,
                                   const byte* iv, word32 ivSz,
                                   byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);
typedef int (*wc_AesAuthDecryptFunc)(Aes* aes, byte* out,
                                   const byte* in, word32 sz,
                                   const byte* iv, word32 ivSz,
                                   const byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);

/* AES-CBC */
WOLFSSL_API int  wc_AesSetKey(Aes* aes, const byte* key, word32 len,
                              const byte* iv, int dir);
WOLFSSL_API int  wc_AesSetIV(Aes* aes, const byte* iv);

#ifdef HAVE_AES_CBC
WOLFSSL_API int  wc_AesCbcEncrypt(Aes* aes, byte* out,
                                  const byte* in, word32 sz);
WOLFSSL_API int  wc_AesCbcDecrypt(Aes* aes, byte* out,
                                  const byte* in, word32 sz);
#endif

#ifdef WOLFSSL_AES_CFB
WOLFSSL_API int wc_AesCfbEncrypt(Aes* aes, byte* out,
                                    const byte* in, word32 sz);
WOLFSSL_API int wc_AesCfb1Encrypt(Aes* aes, byte* out,
                                    const byte* in, word32 sz);
WOLFSSL_API int wc_AesCfb8Encrypt(Aes* aes, byte* out,
                                    const byte* in, word32 sz);
#ifdef HAVE_AES_DECRYPT
WOLFSSL_API int wc_AesCfbDecrypt(Aes* aes, byte* out,
                                    const byte* in, word32 sz);
WOLFSSL_API int wc_AesCfb1Decrypt(Aes* aes, byte* out,
                                    const byte* in, word32 sz);
WOLFSSL_API int wc_AesCfb8Decrypt(Aes* aes, byte* out,
                                    const byte* in, word32 sz);
#endif /* HAVE_AES_DECRYPT */
#endif /* WOLFSSL_AES_CFB */

#ifdef WOLFSSL_AES_OFB
WOLFSSL_API int wc_AesOfbEncrypt(Aes* aes, byte* out,
                                    const byte* in, word32 sz);
#ifdef HAVE_AES_DECRYPT
WOLFSSL_API int wc_AesOfbDecrypt(Aes* aes, byte* out,
                                    const byte* in, word32 sz);
#endif /* HAVE_AES_DECRYPT */
#endif /* WOLFSSL_AES_OFB */

#ifdef HAVE_AES_ECB
WOLFSSL_API int wc_AesEcbEncrypt(Aes* aes, byte* out,
                                  const byte* in, word32 sz);
WOLFSSL_API int wc_AesEcbDecrypt(Aes* aes, byte* out,
                                  const byte* in, word32 sz);
#endif

/* AES-CTR */
#ifdef WOLFSSL_AES_COUNTER
 WOLFSSL_API int wc_AesCtrEncrypt(Aes* aes, byte* out,
                                   const byte* in, word32 sz);
#endif
/* AES-DIRECT */
#if defined(WOLFSSL_AES_DIRECT)
#if defined(BUILDING_WOLFSSL)
 WOLFSSL_API WARN_UNUSED_RESULT int wc_AesEncryptDirect(Aes* aes, byte* out,
                                                        const byte* in);
 WOLFSSL_API WARN_UNUSED_RESULT int wc_AesDecryptDirect(Aes* aes, byte* out,
                                                        const byte* in);
 WOLFSSL_API WARN_UNUSED_RESULT int wc_AesSetKeyDirect(Aes* aes,
                                                       const byte* key,
                                                       word32 len,
                                const byte* iv, int dir);
#else
 WOLFSSL_API int wc_AesEncryptDirect(Aes* aes, byte* out, const byte* in);
 WOLFSSL_API int wc_AesDecryptDirect(Aes* aes, byte* out, const byte* in);
 WOLFSSL_API int wc_AesSetKeyDirect(Aes* aes, const byte* key, word32 len,
                                const byte* iv, int dir);
#endif
#endif

#ifdef HAVE_AESGCM
#ifdef WOLFSSL_XILINX_CRYPT
 WOLFSSL_API int  wc_AesGcmSetKey_ex(Aes* aes, const byte* key, word32 len,
         word32 kup);
#elif defined(WOLFSSL_AFALG_XILINX_AES)
 WOLFSSL_LOCAL int  wc_AesGcmSetKey_ex(Aes* aes, const byte* key, word32 len,
         word32 kup);
#endif
 WOLFSSL_API int  wc_AesGcmSetKey(Aes* aes, const byte* key, word32 len);
 WOLFSSL_API int  wc_AesGcmEncrypt(Aes* aes, byte* out,
                                   const byte* in, word32 sz,
                                   const byte* iv, word32 ivSz,
                                   byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);
 WOLFSSL_API int  wc_AesGcmDecrypt(Aes* aes, byte* out,
                                   const byte* in, word32 sz,
                                   const byte* iv, word32 ivSz,
                                   const byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);
#ifdef WOLFSSL_AESGCM_STREAM
WOLFSSL_API int wc_AesGcmInit(Aes* aes, const byte* key, word32 len,
        const byte* iv, word32 ivSz);

WOLFSSL_API int wc_AesGcmEncryptInit(Aes* aes, const byte* key, word32 len,
        const byte* iv, word32 ivSz);
WOLFSSL_API int wc_AesGcmEncryptInit_ex(Aes* aes, const byte* key, word32 len,
        byte* ivOut, word32 ivOutSz);
WOLFSSL_API int wc_AesGcmEncryptUpdate(Aes* aes, byte* out, const byte* in,
        word32 sz, const byte* authIn, word32 authInSz);
WOLFSSL_API int wc_AesGcmEncryptFinal(Aes* aes, byte* authTag,
        word32 authTagSz);

WOLFSSL_API int wc_AesGcmDecryptInit(Aes* aes, const byte* key, word32 len,
        const byte* iv, word32 ivSz);
WOLFSSL_API int wc_AesGcmDecryptUpdate(Aes* aes, byte* out, const byte* in,
        word32 sz, const byte* authIn, word32 authInSz);
WOLFSSL_API int wc_AesGcmDecryptFinal(Aes* aes, const byte* authTag,
        word32 authTagSz);
#endif

#ifndef WC_NO_RNG
 WOLFSSL_API int  wc_AesGcmSetExtIV(Aes* aes, const byte* iv, word32 ivSz);
 WOLFSSL_API int  wc_AesGcmSetIV(Aes* aes, word32 ivSz,
                                   const byte* ivFixed, word32 ivFixedSz,
                                   WC_RNG* rng);
 WOLFSSL_API int  wc_AesGcmEncrypt_ex(Aes* aes, byte* out,
                                   const byte* in, word32 sz,
                                   byte* ivOut, word32 ivOutSz,
                                   byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);
#endif /* WC_NO_RNG */

 WOLFSSL_API int wc_GmacSetKey(Gmac* gmac, const byte* key, word32 len);
 WOLFSSL_API int wc_GmacUpdate(Gmac* gmac, const byte* iv, word32 ivSz,
                               const byte* authIn, word32 authInSz,
                               byte* authTag, word32 authTagSz);
#ifndef WC_NO_RNG
 WOLFSSL_API int wc_Gmac(const byte* key, word32 keySz, byte* iv, word32 ivSz,
                               const byte* authIn, word32 authInSz,
                               byte* authTag, word32 authTagSz, WC_RNG* rng);
 WOLFSSL_API int wc_GmacVerify(const byte* key, word32 keySz,
                               const byte* iv, word32 ivSz,
                               const byte* authIn, word32 authInSz,
                               const byte* authTag, word32 authTagSz);
#endif /* WC_NO_RNG */
#endif /* HAVE_AESGCM */
#ifdef HAVE_AESCCM
 WOLFSSL_LOCAL int wc_AesCcmCheckTagSize(int sz);
 WOLFSSL_API int  wc_AesCcmSetKey(Aes* aes, const byte* key, word32 keySz);
 WOLFSSL_API int  wc_AesCcmEncrypt(Aes* aes, byte* out,
                                   const byte* in, word32 inSz,
                                   const byte* nonce, word32 nonceSz,
                                   byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);
 WOLFSSL_API int  wc_AesCcmDecrypt(Aes* aes, byte* out,
                                   const byte* in, word32 inSz,
                                   const byte* nonce, word32 nonceSz,
                                   const byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);
 WOLFSSL_API int  wc_AesCcmSetNonce(Aes* aes,
                                   const byte* nonce, word32 nonceSz);
 WOLFSSL_API int  wc_AesCcmEncrypt_ex(Aes* aes, byte* out,
                                   const byte* in, word32 sz,
                                   byte* ivOut, word32 ivOutSz,
                                   byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);
#endif /* HAVE_AESCCM */
#ifdef HAVE_AES_KEYWRAP
 WOLFSSL_API int  wc_AesKeyWrap(const byte* key, word32 keySz,
                                const byte* in, word32 inSz,
                                byte* out, word32 outSz,
                                const byte* iv);
 WOLFSSL_API int  wc_AesKeyWrap_ex(Aes *aes,
                                const byte* in, word32 inSz,
                                byte* out, word32 outSz,
                                const byte* iv);
 WOLFSSL_API int  wc_AesKeyUnWrap(const byte* key, word32 keySz,
                                const byte* in, word32 inSz,
                                byte* out, word32 outSz,
                                const byte* iv);
 WOLFSSL_API int  wc_AesKeyUnWrap_ex(Aes *aes,
                                const byte* in, word32 inSz,
                                byte* out, word32 outSz,
                                const byte* iv);
#endif /* HAVE_AES_KEYWRAP */

#ifdef WOLFSSL_AES_XTS

WOLFSSL_API int wc_AesXtsSetKey(XtsAes* aes, const byte* key,
         word32 len, int dir, void* heap, int devId);

WOLFSSL_API int wc_AesXtsEncryptSector(XtsAes* aes, byte* out,
         const byte* in, word32 sz, word64 sector);

WOLFSSL_API int wc_AesXtsDecryptSector(XtsAes* aes, byte* out,
         const byte* in, word32 sz, word64 sector);

WOLFSSL_API int wc_AesXtsEncrypt(XtsAes* aes, byte* out,
         const byte* in, word32 sz, const byte* i, word32 iSz);

WOLFSSL_API int wc_AesXtsDecrypt(XtsAes* aes, byte* out,
        const byte* in, word32 sz, const byte* i, word32 iSz);

WOLFSSL_API int wc_AesXtsFree(XtsAes* aes);
#endif

WOLFSSL_API int wc_AesGetKeySize(Aes* aes, word32* keySize);

WOLFSSL_API int  wc_AesInit(Aes* aes, void* heap, int devId);
#ifdef WOLF_PRIVATE_KEY_ID
WOLFSSL_API int  wc_AesInit_Id(Aes* aes, unsigned char* id, int len, void* heap,
        int devId);
WOLFSSL_API int  wc_AesInit_Label(Aes* aes, const char* label, void* heap,
        int devId);
#endif
WOLFSSL_API void wc_AesFree(Aes* aes);

#ifdef WOLFSSL_AES_SIV
WOLFSSL_API
int wc_AesSivEncrypt(const byte* key, word32 keySz, const byte* assoc,
                     word32 assocSz, const byte* nonce, word32 nonceSz,
                     const byte* in, word32 inSz, byte* siv, byte* out);
WOLFSSL_API
int wc_AesSivDecrypt(const byte* key, word32 keySz, const byte* assoc,
                     word32 assocSz, const byte* nonce, word32 nonceSz,
                     const byte* in, word32 inSz, byte* siv, byte* out);
#endif

#ifdef __cplusplus
    } /* extern "C" */
#endif


#endif /* NO_AES */
#endif /* WOLF_CRYPT_AES_H */
