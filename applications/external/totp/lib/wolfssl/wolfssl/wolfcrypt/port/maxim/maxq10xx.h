/* maxq10xx.h
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

#ifndef _WOLFPORT_MAXQ10XX_H_
#define _WOLFPORT_MAXQ10XX_H_

#if defined(WOLFSSL_MAXQ1065) || defined(WOLFSSL_MAXQ108X)

#include <wolfssl/wolfcrypt/types.h>

#ifdef WOLF_CRYPTO_CB
    #ifdef WOLFSSL_MAXQ1065
        #define MAXQ_AESGCM
        #define MAXQ_SHA256
        #define MAXQ_RNG
        #define MAXQ_ECC
    #endif /* WOLFSSL_MAXQ1065 */

    #ifdef WOLFSSL_MAXQ108X
        #define MAXQ_AESGCM
        #define MAXQ_SHA256
        #define MAXQ_RNG
        #define MAXQ_ECC
        #define ENABLE_RSA
    #endif /* WOLFSSL_MAXQ108X */
#endif /* WOLF_CRYPTO_CB */

struct WOLFSSL_CTX;
typedef struct WOLFSSL WOLFSSL;
typedef struct DecodedCert DecodedCert;
typedef struct DerBuffer DerBuffer;
typedef struct wc_CryptoInfo wc_CryptoInfo;
typedef struct Aes Aes;
typedef struct wc_Sha256 wc_Sha256;
typedef struct ecc_key ecc_key;
typedef struct ProtocolVersion ProtocolVersion;

typedef struct {
    int use_hw_keys;
    DerBuffer* device_cert;
} maxq_ssl_t;

typedef struct {
    int key_obj_id;
    int key_pending;
    unsigned char key[32];
} maxq_aes_t;

typedef struct {
    int hash_running;
    int soft_hash;
} maxq_sha256_t;

typedef struct {
    int key_obj_id;
    int key_pending;
    int hw_ecc;
    int hw_storage;
    unsigned char ecc_key[32 * 3];
} maxq_ecc_t;

#ifdef __cplusplus
    extern "C" {
#endif

WOLFSSL_LOCAL int maxq10xx_port_init(void);

#ifdef WOLF_CRYPTO_CB
WOLFSSL_LOCAL int wolfSSL_MAXQ10XX_CryptoDevCb(int devId, wc_CryptoInfo* info,
                                               void* ctx);
#endif /* WOLF_CRYPTO_CB */

#ifdef WOLFSSL_MAXQ10XX_CRYPTO
WOLFSSL_LOCAL int wc_MAXQ10XX_AesSetKey(Aes* aes, const byte* userKey,
                                        word32 keylen);
WOLFSSL_LOCAL void wc_MAXQ10XX_AesFree(Aes* aes);
WOLFSSL_LOCAL void wc_MAXQ10XX_Sha256Copy(wc_Sha256* sha256);
WOLFSSL_LOCAL void wc_MAXQ10XX_Sha256Free(wc_Sha256* sha256);
WOLFSSL_LOCAL int wc_MAXQ10XX_EccSetKey(ecc_key* key, word32 keysize);
WOLFSSL_LOCAL void wc_MAXQ10XX_EccFree(ecc_key* key);
#endif /* WOLFSSL_MAXQ10XX_CRYPTO */

#ifdef HAVE_PK_CALLBACKS
WOLFSSL_LOCAL void maxq10xx_SetupPkCallbacks(struct WOLFSSL_CTX* ctx,
                                             ProtocolVersion *pv);
#endif /* HAVE_PK_CALLBACKS */

#if defined(WOLFSSL_MAXQ10XX_TLS)
WOLFSSL_LOCAL int wolfSSL_maxq10xx_load_certificate(WOLFSSL *ssl);
#endif /* WOLFSSL_MAXQ10XX_TLS */

#ifdef __cplusplus
    }
#endif

#endif /* WOLFSSL_MAXQ1065 || WOLFSSL_MAXQ108X */
#endif /* _WOLFPORT_MAXQ10XX_H_ */
