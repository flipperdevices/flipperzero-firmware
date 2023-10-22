/* atmel.h
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

#ifndef _ATECC508_H_
#define _ATECC508_H_

#include <stdint.h>

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

#if defined(WOLFSSL_ATECC508A) || defined(WOLFSSL_ATECC608A) || \
    defined(WOLFSSL_ATECC_PKCB)
    #undef SHA_BLOCK_SIZE
    #include <cryptoauthlib.h>
#endif

/* ATECC508A/608A only supports ECC P-256 */
#define ATECC_KEY_SIZE      (32)
#define ATECC_PUBKEY_SIZE   (ATECC_KEY_SIZE*2) /* X and Y */
#define ATECC_SIG_SIZE      (ATECC_KEY_SIZE*2) /* R and S */
#ifndef ATECC_MAX_SLOT
#define ATECC_MAX_SLOT      (0x8) /* Only use 0-7 */
#endif
#define ATECC_INVALID_SLOT  (0xFF)

/* Device Key for signing */
#ifndef ATECC_SLOT_AUTH_PRIV
#define ATECC_SLOT_AUTH_PRIV      (0x0)
#endif
/* Ephemeral key */
#ifndef ATECC_SLOT_ECDHE_PRIV
#define ATECC_SLOT_ECDHE_PRIV     (0x2)
#endif
/* Symmetric encryption key */
#ifndef ATECC_SLOT_I2C_ENC
    #ifdef WOLFSSL_ATECC_TNGTLS
        #define ATECC_SLOT_I2C_ENC        (0x06)
    #else
        #define ATECC_SLOT_I2C_ENC        (0x04)
    #endif
#endif
/* Parent encryption key */
#ifndef ATECC_SLOT_ENC_PARENT
    #ifdef WOLFSSL_ATECC_TNGTLS
        #define ATECC_SLOT_ENC_PARENT     (0x6)
    #else
        #define ATECC_SLOT_ENC_PARENT     (0x7)
    #endif
#endif

/* ATECC_KEY_SIZE required for ecc.h */
#include <wolfssl/wolfcrypt/ecc.h>

struct WOLFSSL;
struct WOLFSSL_CTX;
struct WOLFSSL_X509_STORE_CTX;
struct ecc_key;

/* Atmel port functions */
int  atmel_init(void);
void atmel_finish(void);
int  atmel_get_random_number(uint32_t count, uint8_t* rand_out);
#ifndef ATMEL_GET_RANDOM_BLOCK_DEFINED
    int  atmel_get_random_block(unsigned char* output, unsigned int sz);
    #define ATMEL_GET_RANDOM_BLOCK_DEFINED
#endif
long atmel_get_curr_time_and_date(long* tm);

#if defined(WOLFSSL_ATECC508A) || defined(WOLFSSL_ATECC608A)

enum atmelSlotType {
    ATMEL_SLOT_ANY,
    ATMEL_SLOT_ENCKEY,
    ATMEL_SLOT_DEVICE,
    ATMEL_SLOT_ECDHE,
    ATMEL_SLOT_ECDHE_ENC,
};

int  atmel_ecc_alloc(int slotType);
void atmel_ecc_free(int slotId);

typedef int  (*atmel_slot_alloc_cb)(int);
typedef void (*atmel_slot_dealloc_cb)(int);
int atmel_set_slot_allocator(atmel_slot_alloc_cb alloc,
    atmel_slot_dealloc_cb dealloc);

int  atmel_ecc_translate_err(int status);
int  atmel_get_rev_info(word32* revision);
void atmel_show_rev_info(void);

WOLFSSL_API int wolfCrypt_ATECC_SetConfig(ATCAIfaceCfg* cfg);

/* The macro ATECC_GET_ENC_KEY can be set to override the default
   encryption key with your own at build-time */
#ifndef ATECC_GET_ENC_KEY
    #define ATECC_GET_ENC_KEY(enckey, keysize) atmel_get_enc_key_default((enckey), (keysize))
#endif
int  atmel_get_enc_key_default(byte* enckey, word16 keysize);
int  atmel_ecc_create_pms(int slotId, const uint8_t* peerKey, uint8_t* pms);
int  atmel_ecc_create_key(int slotId, byte* peerKey);
int  atmel_ecc_sign(int slotId, const byte* message, byte* signature);
int  atmel_ecc_verify(const byte* message, const byte* signature,
    const byte* pubkey, int* pVerified);

#endif /* WOLFSSL_ATECC508A */

#ifdef HAVE_PK_CALLBACKS
    int atcatls_create_key_cb(struct WOLFSSL* ssl, struct ecc_key* key, unsigned int keySz,
        int ecc_curve, void* ctx);
    int atcatls_create_pms_cb(struct WOLFSSL* ssl, struct ecc_key* otherKey,
        unsigned char* pubKeyDer, word32* pubKeySz,
        unsigned char* out, word32* outlen,
        int side, void* ctx);
    int atcatls_sign_certificate_cb(struct WOLFSSL* ssl, const byte* in, unsigned int inSz,
        byte* out, word32* outSz, const byte* key, unsigned int keySz, void* ctx);
    int atcatls_verify_signature_cb(struct WOLFSSL* ssl, const byte* sig, unsigned int sigSz,
        const byte* hash, unsigned int hashSz, const byte* key, unsigned int keySz,
        int* result, void* ctx);

    int atcatls_set_callbacks(struct WOLFSSL_CTX* ctx);
    int atcatls_set_callback_ctx(struct WOLFSSL* ssl, void* user_ctx);
#endif

#endif /* _ATECC508_H_ */
