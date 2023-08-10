/* ed448.h
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
    \file wolfssl/wolfcrypt/ed448.h
*/


#ifndef WOLF_CRYPT_ED448_H
#define WOLF_CRYPT_ED448_H

#include <wolfssl/wolfcrypt/types.h>

#ifdef HAVE_ED448

#include <wolfssl/wolfcrypt/fe_448.h>
#include <wolfssl/wolfcrypt/ge_448.h>
#include <wolfssl/wolfcrypt/random.h>
#ifndef WOLFSSL_SHAKE256
#error ED448 requires SHAKE256
#endif
#include <wolfssl/wolfcrypt/sha3.h>

#ifdef WOLFSSL_ASYNC_CRYPT
    #include <wolfssl/wolfcrypt/async.h>
#endif

#ifdef __cplusplus
    extern "C" {
#endif


/* info about EdDSA curve specifically ed448, defined as an elliptic curve
 * over GF(p)
 *
 *  56                 key size
 *  "ED448"            curve name
 *  "2^448-2^224-1"    prime number
 *  "-39081"           value of d
 *  "SHAKE256"         hash function
 */

#define ED448_KEY_SIZE     57   /* private key only */
#define ED448_SIG_SIZE     114  /* two elements */

#define ED448_PUB_KEY_SIZE 57   /* compressed */
/* both private and public key */
#define ED448_PRV_KEY_SIZE (ED448_PUB_KEY_SIZE+ED448_KEY_SIZE)

#define ED448_PREHASH_SIZE 64

enum {
    Ed448    = 0,
    Ed448ph  = 1
};

#ifndef WC_ED448KEY_TYPE_DEFINED
    typedef struct ed448_key ed448_key;
    #define WC_ED448KEY_TYPE_DEFINED
#endif

/* An ED448 Key */
struct ed448_key {
    byte    p[ED448_PUB_KEY_SIZE]; /* compressed public key */
    byte    k[ED448_PRV_KEY_SIZE]; /* private key : 56 secret -- 56 public */
#ifdef FREESCALE_LTC_ECC
    /* uncompressed point coordinates */
    byte pointX[ED448_KEY_SIZE]; /* recovered X coordinate */
    byte pointY[ED448_KEY_SIZE]; /* Y coordinate is the public key with The most significant bit of the final octet always zero. */
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
#ifdef WOLFSSL_ED448_PERSISTENT_SHA
    wc_Shake sha;
    int sha_clean_flag;
#endif
};


WOLFSSL_API
int wc_ed448_make_public(ed448_key* key, unsigned char* pubKey,
                         word32 pubKeySz);
WOLFSSL_API
int wc_ed448_make_key(WC_RNG* rng, int keysize, ed448_key* key);
#ifdef HAVE_ED448_SIGN
WOLFSSL_API
int wc_ed448_sign_msg(const byte* in, word32 inLen, byte* out, word32 *outLen,
                      ed448_key* key, const byte* context, byte contextLen);
WOLFSSL_API
int wc_ed448ph_sign_hash(const byte* hash, word32 hashLen, byte* out,
                         word32 *outLen, ed448_key* key,
                         const byte* context, byte contextLen);
WOLFSSL_API
int wc_ed448_sign_msg_ex(const byte* in, word32 inLen, byte* out,
                         word32 *outLen, ed448_key* key, byte type,
                         const byte* context, byte contextLen);
WOLFSSL_API
int wc_ed448ph_sign_msg(const byte* in, word32 inLen, byte* out,
                        word32 *outLen, ed448_key* key, const byte* context,
                        byte contextLen);
#endif /* HAVE_ED448_SIGN */
#ifdef HAVE_ED448_VERIFY
WOLFSSL_API
int wc_ed448_verify_msg_ex(const byte* sig, word32 sigLen, const byte* msg,
                            word32 msgLen, int* res, ed448_key* key,
                            byte type, const byte* context, byte contextLen);
#ifdef WOLFSSL_ED448_STREAMING_VERIFY
WOLFSSL_API
int wc_ed448_verify_msg_init(const byte* sig, word32 sigLen, ed448_key* key,
                        byte type, const byte* context, byte contextLen);
WOLFSSL_API
int wc_ed448_verify_msg_update(const byte* msgSegment, word32 msgSegmentLen,
                             ed448_key* key);
WOLFSSL_API
int wc_ed448_verify_msg_final(const byte* sig, word32 sigLen,
                              int* res, ed448_key* key);
#endif /* WOLFSSL_ED448_STREAMING_VERIFY */
WOLFSSL_API
int wc_ed448_verify_msg(const byte* sig, word32 sigLen, const byte* msg,
                        word32 msgLen, int* res, ed448_key* key,
                        const byte* context, byte contextLen);
WOLFSSL_API
int wc_ed448ph_verify_hash(const byte* sig, word32 sigLen, const byte* hash,
                           word32 hashLen, int* res, ed448_key* key,
                           const byte* context, byte contextLen);
WOLFSSL_API
int wc_ed448ph_verify_msg(const byte* sig, word32 sigLen, const byte* msg,
                          word32 msgLen, int* res, ed448_key* key,
                          const byte* context, byte contextLen);
#endif /* HAVE_ED448_VERIFY */
WOLFSSL_API
int wc_ed448_init_ex(ed448_key* key, void *heap, int devId);
WOLFSSL_API
int wc_ed448_init(ed448_key* key);
WOLFSSL_API
void wc_ed448_free(ed448_key* key);

#ifdef HAVE_ED448_KEY_IMPORT
WOLFSSL_API
int wc_ed448_import_public(const byte* in, word32 inLen, ed448_key* key);
WOLFSSL_API
int wc_ed448_import_public_ex(const byte* in, word32 inLen, ed448_key* key,
                              int trusted);
WOLFSSL_API
int wc_ed448_import_private_only(const byte* priv, word32 privSz,
                                 ed448_key* key);
WOLFSSL_API
int wc_ed448_import_private_key(const byte* priv, word32 privSz,
                                const byte* pub, word32 pubSz, ed448_key* key);
WOLFSSL_API
int wc_ed448_import_private_key_ex(const byte* priv, word32 privSz,
    const byte* pub, word32 pubSz, ed448_key* key, int trusted);
#endif /* HAVE_ED448_KEY_IMPORT */

#ifdef HAVE_ED448_KEY_EXPORT
WOLFSSL_API
int wc_ed448_export_public(ed448_key* key, byte* out, word32* outLen);
WOLFSSL_API
int wc_ed448_export_private_only(ed448_key* key, byte* out, word32* outLen);
WOLFSSL_API
int wc_ed448_export_private(ed448_key* key, byte* out, word32* outLen);
WOLFSSL_API
int wc_ed448_export_key(ed448_key* key, byte* priv, word32 *privSz,
                        byte* pub, word32 *pubSz);
#endif /* HAVE_ED448_KEY_EXPORT */

WOLFSSL_API
int wc_ed448_check_key(ed448_key* key);

/* size helper */
WOLFSSL_API
int wc_ed448_size(ed448_key* key);
WOLFSSL_API
int wc_ed448_priv_size(ed448_key* key);
WOLFSSL_API
int wc_ed448_pub_size(ed448_key* key);
WOLFSSL_API
int wc_ed448_sig_size(ed448_key* key);

#ifdef __cplusplus
    }    /* extern "C" */
#endif

#endif /* HAVE_ED448 */
#endif /* WOLF_CRYPT_ED448_H */
