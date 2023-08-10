/* ed25519.h
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
    \file wolfssl/wolfcrypt/ed25519.h
*/


#ifndef WOLF_CRYPT_ED25519_H
#define WOLF_CRYPT_ED25519_H

#include <wolfssl/wolfcrypt/types.h>

#ifdef HAVE_ED25519

#include <wolfssl/wolfcrypt/fe_operations.h>
#include <wolfssl/wolfcrypt/ge_operations.h>
#include <wolfssl/wolfcrypt/random.h>
#ifndef WOLFSSL_SHA512
#error ED25519 requires SHA512
#endif
#include <wolfssl/wolfcrypt/sha512.h>

#ifdef WOLFSSL_ASYNC_CRYPT
    #include <wolfssl/wolfcrypt/async.h>
#endif

#ifdef __cplusplus
    extern "C" {
#endif


/* info about EdDSA curve specifically ed25519, defined as an elliptic curve
   over GF(p) */
/*
    32,                key size
    "ED25519",         curve name
    "2^255-19",        prime number
    "SHA512",          hash function
    "-121665/121666",  value of d
*/

#define ED25519_KEY_SIZE     32 /* private key only */
#define ED25519_SIG_SIZE     64

#define ED25519_PUB_KEY_SIZE 32 /* compressed */
/* both private and public key */
#define ED25519_PRV_KEY_SIZE (ED25519_PUB_KEY_SIZE+ED25519_KEY_SIZE)


enum {
    Ed25519    = -1,
    Ed25519ctx = 0,
    Ed25519ph  = 1
};

#ifndef WC_ED25519KEY_TYPE_DEFINED
    typedef struct ed25519_key ed25519_key;
    #define WC_ED25519KEY_TYPE_DEFINED
#endif

/* ED25519 Flags */
enum {
    WC_ED25519_FLAG_NONE     = 0x00,
    WC_ED25519_FLAG_DEC_SIGN = 0x01
};

/* An ED25519 Key */
struct ed25519_key {
    byte    p[ED25519_PUB_KEY_SIZE]; /* compressed public key */
    byte    k[ED25519_PRV_KEY_SIZE]; /* private key : 32 secret -- 32 public */
#ifdef FREESCALE_LTC_ECC
    /* uncompressed point coordinates */
    byte pointX[ED25519_KEY_SIZE]; /* recovered X coordinate */
    byte pointY[ED25519_KEY_SIZE]; /* Y coordinate is the public key with The most significant bit of the final octet always zero. */
#endif
#ifdef WOLFSSL_SE050
    word32 keyId;
    word32 flags;
    byte   keyIdSet;
#endif
    word16 privKeySet:1;
    word16 pubKeySet:1;
#ifdef WOLFSSL_ASYNC_CRYPT
    WC_ASYNC_DEV asyncDev;
#endif
#if defined(WOLF_CRYPTO_CB)
    int devId;
#endif
    void *heap;
#ifdef WOLFSSL_ED25519_PERSISTENT_SHA
    wc_Sha512 sha;
    int sha_clean_flag;
#endif
};


WOLFSSL_API
int wc_ed25519_make_public(ed25519_key* key, unsigned char* pubKey,
                           word32 pubKeySz);
WOLFSSL_API
int wc_ed25519_make_key(WC_RNG* rng, int keysize, ed25519_key* key);
#ifdef HAVE_ED25519_SIGN
WOLFSSL_API
int wc_ed25519_sign_msg(const byte* in, word32 inLen, byte* out,
                        word32 *outLen, ed25519_key* key);
WOLFSSL_API
int wc_ed25519ctx_sign_msg(const byte* in, word32 inLen, byte* out,
                           word32 *outLen, ed25519_key* key,
                           const byte* context, byte contextLen);
WOLFSSL_API
int wc_ed25519ph_sign_hash(const byte* hash, word32 hashLen, byte* out,
                           word32 *outLen, ed25519_key* key,
                           const byte* context, byte contextLen);
WOLFSSL_API
int wc_ed25519ph_sign_msg(const byte* in, word32 inLen, byte* out,
                          word32 *outLen, ed25519_key* key, const byte* context,
                          byte contextLen);
WOLFSSL_API
int wc_ed25519_sign_msg_ex(const byte* in, word32 inLen, byte* out,
                            word32 *outLen, ed25519_key* key, byte type,
                            const byte* context, byte contextLen);
#endif /* HAVE_ED25519_SIGN */
#ifdef HAVE_ED25519_VERIFY
WOLFSSL_API
int wc_ed25519_verify_msg(const byte* sig, word32 sigLen, const byte* msg,
                          word32 msgLen, int* res, ed25519_key* key);
WOLFSSL_API
int wc_ed25519ctx_verify_msg(const byte* sig, word32 sigLen, const byte* msg,
                             word32 msgLen, int* res, ed25519_key* key,
                             const byte* context, byte contextLen);
WOLFSSL_API
int wc_ed25519ph_verify_hash(const byte* sig, word32 sigLen, const byte* hash,
                             word32 hashLen, int* res, ed25519_key* key,
                             const byte* context, byte contextLen);
WOLFSSL_API
int wc_ed25519ph_verify_msg(const byte* sig, word32 sigLen, const byte* msg,
                            word32 msgLen, int* res, ed25519_key* key,
                            const byte* context, byte contextLen);
WOLFSSL_API
int wc_ed25519_verify_msg_ex(const byte* sig, word32 sigLen, const byte* msg,
                              word32 msgLen, int* res, ed25519_key* key,
                              byte type, const byte* context, byte contextLen);
#ifdef WOLFSSL_ED25519_STREAMING_VERIFY
WOLFSSL_API
int wc_ed25519_verify_msg_init(const byte* sig, word32 sigLen, ed25519_key* key,
                               byte type, const byte* context, byte contextLen);
WOLFSSL_API
int wc_ed25519_verify_msg_update(const byte* msgSegment, word32 msgSegmentLen,
                               ed25519_key* key);
WOLFSSL_API
int wc_ed25519_verify_msg_final(const byte* sig, word32 sigLen, int* res,
                                ed25519_key* key);
#endif /* WOLFSSL_ED25519_STREAMING_VERIFY */
#endif /* HAVE_ED25519_VERIFY */


WOLFSSL_API
int wc_ed25519_init(ed25519_key* key);
WOLFSSL_API
int wc_ed25519_init_ex(ed25519_key* key, void* heap, int devId);
WOLFSSL_API
void wc_ed25519_free(ed25519_key* key);
#ifdef HAVE_ED25519_KEY_IMPORT
WOLFSSL_API
int wc_ed25519_import_public(const byte* in, word32 inLen, ed25519_key* key);
WOLFSSL_API
int wc_ed25519_import_public_ex(const byte* in, word32 inLen, ed25519_key* key,
                                int trusted);
WOLFSSL_API
int wc_ed25519_import_private_only(const byte* priv, word32 privSz,
                                                              ed25519_key* key);
WOLFSSL_API
int wc_ed25519_import_private_key(const byte* priv, word32 privSz,
                               const byte* pub, word32 pubSz, ed25519_key* key);
WOLFSSL_API
int wc_ed25519_import_private_key_ex(const byte* priv, word32 privSz,
    const byte* pub, word32 pubSz, ed25519_key* key, int trusted);
#endif /* HAVE_ED25519_KEY_IMPORT */

#ifdef HAVE_ED25519_KEY_EXPORT
WOLFSSL_API
int wc_ed25519_export_public(ed25519_key* key, byte* out, word32* outLen);
WOLFSSL_API
int wc_ed25519_export_private_only(ed25519_key* key, byte* out, word32* outLen);
WOLFSSL_API
int wc_ed25519_export_private(ed25519_key* key, byte* out, word32* outLen);
WOLFSSL_API
int wc_ed25519_export_key(ed25519_key* key,
                          byte* priv, word32 *privSz,
                          byte* pub, word32 *pubSz);
#endif /* HAVE_ED25519_KEY_EXPORT */

WOLFSSL_API
int wc_ed25519_check_key(ed25519_key* key);

/* size helper */
WOLFSSL_API
int wc_ed25519_size(ed25519_key* key);
WOLFSSL_API
int wc_ed25519_priv_size(ed25519_key* key);
WOLFSSL_API
int wc_ed25519_pub_size(ed25519_key* key);
WOLFSSL_API
int wc_ed25519_sig_size(ed25519_key* key);

#ifdef __cplusplus
    }    /* extern "C" */
#endif

#endif /* HAVE_ED25519 */
#endif /* WOLF_CRYPT_ED25519_H */
