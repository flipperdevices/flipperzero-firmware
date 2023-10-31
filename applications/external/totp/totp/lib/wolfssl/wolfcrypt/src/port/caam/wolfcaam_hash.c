/* wolfcaam_hash.c
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

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#if defined(WOLFSSL_CAAM) && defined(WOLFSSL_CAAM_HASH) \
	&& !defined(WOLFSSL_IMXRT1170_CAAM)

#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

#if defined(__INTEGRITY) || defined(INTEGRITY)
#include <INTEGRITY.h>
#endif
#include <wolfssl/wolfcrypt/port/caam/wolfcaam.h>
#include <wolfssl/wolfcrypt/port/caam/wolfcaam_hash.h>

#if defined(WOLFSSL_CAAM_DEBUG) || defined(WOLFSSL_CAAM_PRINT)
#include <stdio.h>
#endif

#ifndef NO_SHA
#include <wolfssl/wolfcrypt/sha.h>
#endif

#if !defined(NO_SHA256) || defined(WOLFSSL_SHA224)
#include <wolfssl/wolfcrypt/sha256.h>
#endif

#if defined(WOLFSSL_SHA384) || defined(WOLFSSL_SHA512)
#include <wolfssl/wolfcrypt/sha512.h>
#endif

#ifndef NO_MD5
#include <wolfssl/wolfcrypt/md5.h>
#endif

#ifndef WC_CAAM_CTXLEN
    #define WC_CAAM_CTXLEN 0
#endif

/******************************************************************************
  Common Code Between SHA Functions
  ****************************************************************************/

#ifndef WOLFSSL_HASH_KEEP
static int _ShaUpdate(byte* buffer, word32* buffLen, const byte* ctx,
    const byte* data, word32 len, word32 digestSz, word32 type)
{
    CAAM_BUFFER buf[2];
    word32 arg[4];
    int ret;

    if (buffer == NULL || ctx == NULL || (data == NULL && len > 0)) {
        return BAD_FUNC_ARG;
    }

    if (len == 0) return 0; /* nothing to do */

    /* check for filling out existing buffer */
    if (*buffLen > 0) {
        word32 add = min(len, WC_CAAM_HASH_BLOCK - *buffLen);
        XMEMCPY(&buffer[*buffLen], data, add);

        *buffLen     += add;
        data         += add;
        len          -= add;

        if (*buffLen == WC_CAAM_HASH_BLOCK) {
            /* Set buffer for context */
            buf[idx].BufferType = DataBuffer;
            buf[idx].TheAddress = (CAAM_ADDRESS)sha->ctx;
            buf[idx].Length     = digestSz + WC_CAAM_CTXLEN;
        #if defined(__INTEGRITY) || defined(INTEGRITY)
            buf[idx].Transferred = 0;
        #endif
            idx++;

            /* data to update with */
            buf[idx].BufferType = DataBuffer | LastBuffer;
            buf[idx].TheAddress = (CAAM_ADDRESS)sha->buffer;
            buf[idx].Length     = *buffLen;
        #if defined(__INTEGRITY) || defined(INTEGRITY)
            buf[idx].Transferred = 0;
        #endif
            idx++;

            arg[0] = CAAM_ALG_UPDATE;
            arg[1] = digestSz + WC_CAAM_CTXLEN;

            if ((ret = wc_caamAddAndWait(buf, idx, arg, type)) != 0) {
                WOLFSSL_MSG("Error with CAAM SHA update");
                return ret;
            }
            *buffLen = 0; /* cleared out buffer */
        }
    }

    /* check if multiple full blocks can be done */
    if (len >= WC_CAAM_HASH_BLOCK) {
        word32 sz = len / WC_CAAM_HASH_BLOCK;
        sz = sz * WC_CAAM_HASH_BLOCK;
        idx = 0;

        /* Set buffer for context */
        buf[idx].BufferType = DataBuffer;
        buf[idx].TheAddress = (CAAM_ADDRESS)sha->ctx;
        buf[idx].Length     = digestSz + WC_CAAM_CTXLEN;
    #if defined(__INTEGRITY) || defined(INTEGRITY)
        buf[idx].Transferred = 0;
    #endif
        idx++;

        /* data to update with */
        buf[idx].BufferType = DataBuffer | LastBuffer;
        buf[idx].TheAddress = (CAAM_ADDRESS)data;
        buf[idx].Length     = sz;
    #if defined(__INTEGRITY) || defined(INTEGRITY)
        buf[idx].Transferred = 0;
    #endif
        idx++;

        arg[0] = CAAM_ALG_UPDATE;
        arg[1] = digestSz + WC_CAAM_CTXLEN;

        if ((ret = wc_caamAddAndWait(buf, idx, arg, type)) != 0) {
            WOLFSSL_MSG("Error with CAAM SHA update");
            return ret;
        }

        len  -= sz;
        data += sz;
    }

    /* check for left overs */
    if (len > 0) {
        word32 add = min(len, WC_CAAM_HASH_BLOCK - *buffLen);
        XMEMCPY(&buffer[*buffLen], data, add);
        *buffLen += add;
    }

    return 0;
}
#endif /* !WOLFSSL_HASH_KEEP */


static int _ShaFinal(byte* ctx, word32 ctxSz, byte* in, word32 inSz, byte* out,
    word32 type)
{
    CAAM_BUFFER buf[2];
    word32 arg[4];
    int ret, idx = 0;

    if (ctx == NULL || out == NULL) {
        return BAD_FUNC_ARG;
    }

    /* Set buffer for context */
    buf[idx].BufferType = DataBuffer;
    buf[idx].TheAddress = (CAAM_ADDRESS)ctx;
    buf[idx].Length     = ctxSz;
#if defined(__INTEGRITY) || defined(INTEGRITY)
    buf[idx].Transferred = 0;
#endif
    idx++;

    /* add any potential left overs */
    buf[idx].BufferType = DataBuffer | LastBuffer;
    buf[idx].TheAddress = (CAAM_ADDRESS)in;
    buf[idx].Length     = inSz;
#if defined(__INTEGRITY) || defined(INTEGRITY)
    buf[idx].Transferred = 0;
#endif
    idx++;

    arg[0] = CAAM_ALG_FINAL;
    arg[1] = ctxSz + WC_CAAM_CTXLEN;

    if ((ret = wc_caamAddAndWait(buf, idx, arg, type)) != 0) {
        WOLFSSL_MSG("Error with CAAM SHA Final");
        return ret;
    }
    XMEMCPY(out, ctx, ctxSz);

    return 0;
}


/******************************************************************************
  SHA 1
  ****************************************************************************/
#if !defined(NO_SHA)
int wc_CAAM_ShaHash(wc_Sha* sha, const byte* in, word32 inSz, byte* digest)
{
    int ret = 0;

    /* in the case of update's just store up all data */
    if (in != NULL) {
    #ifdef WOLFSSL_HASH_KEEP
        ret = _wc_Hash_Grow(&(sha->msg), &(sha->used), &(sha->len), in,
                        inSz, sha->heap);
    #else
        ret = _ShaUpdate((byte*)sha->buffer, &sha->buffLen, (byte*)sha->digest,
            in, inSz, SHA_DIGEST_SIZE, CAAM_SHA);
    #endif
    }

    if (digest != NULL) {
    #ifdef WOLFSSL_HASH_KEEP
        int devId  = sha->devId;
        void* heap = sha->heap;

        ret = _ShaFinal((byte*)sha->digest, SHA_DIGEST_SIZE, sha->msg,
            sha->used, digest, CAAM_SHA);

        wc_ShaFree(sha);
        wc_InitSha_ex(sha, heap, devId);
    #else
        ret = _ShaFinal((byte*)sha->digest, SHA_DIGEST_SIZE,
            (byte*)sha->buffer, sha->buffLen, digest, CAAM_SHA);
    #endif
    }
    return ret;
}
#endif /* !NO_SHA */


/******************************************************************************
  SHA 224
  ****************************************************************************/
#ifdef WOLFSSL_SHA224
int wc_CAAM_Sha224Hash(wc_Sha224* sha224, const byte* in, word32 inSz,
    byte* digest)
{
    int ret = 0;

    /* in the case of update's just store up all data */
    if (in != NULL) {
    #ifdef WOLFSSL_HASH_KEEP
        ret = wc_Sha224_Grow(sha224, in, inSz);
    #else
        ret = _ShaUpdate(sha224->buffer, &sha224->bufferLen,
            (byte*)sha224->digest, data, len, SHA224_DIGEST_SIZE, CAAM_SHA224);
    #endif
    }

    if (digest != NULL) {
    #ifdef WOLFSSL_HASH_KEEP
        int devId  = sha224->devId;
        void* heap = sha224->heap;

        ret = _ShaFinal((byte*)sha224->digest, SHA224_DIGEST_SIZE, sha224->msg,
            sha224->used, digest, CAAM_SHA224);
        wc_Sha224Free(sha224);
        wc_InitSha224_ex(sha224, heap, devId);
    #else
        ret = _ShaFinal((byte*)sha224->digest, SHA224_DIGEST_SIZE,
            (byte*)sha224->buffer, sha224->bufferLen, digest, CAAM_SHA224);
    #endif
    }
    return ret;
}
#endif /* WOLFSSL_SHA224 */


/******************************************************************************
  SHA 256
  ****************************************************************************/
#if !defined(NO_SHA256)
int wc_CAAM_Sha256Hash(wc_Sha256* sha256, const byte* in, word32 inSz,
    byte* digest)
{
    int ret = 0;

    /* in the case of update's just store up all data */
    if (in != NULL) {
    #ifdef WOLFSSL_HASH_KEEP
        ret = wc_Sha256_Grow(sha256, in, inSz);
    #else
        ret = _ShaUpdate((byte*)sha256->buffer, &sha256->buffLen,
            (byte*)sha256->digest, in, inSz, SHA256_DIGEST_SIZE, CAAM_SHA256);
    #endif
    }

    if (digest != NULL) {
    #ifdef WOLFSSL_HASH_KEEP
        int devId  = sha256->devId;
        void* heap = sha256->heap;

        ret = _ShaFinal((byte*)sha256->digest, SHA256_DIGEST_SIZE, sha256->msg,
            sha256->used, digest, CAAM_SHA256);

        wc_Sha256Free(sha256);
        wc_InitSha256_ex(sha256, heap, devId);
    #else
        ret = _ShaFinal((byte*)sha256->digest, SHA256_DIGEST_SIZE,
            (byte*)sha256->buffer, sha256->buffLen, digest, CAAM_SHA256);
    #endif
    }
    return ret;
}
#endif /* !NO_SHA256 */


/******************************************************************************
  SHA 384
  ****************************************************************************/
#ifdef WOLFSSL_SHA384
int wc_CAAM_Sha384Hash(wc_Sha384* sha384, const byte* in, word32 inSz,
    byte* digest)
{
    int ret = 0;

    /* in the case of update's just store up all data */
    if (in != NULL) {
    #ifdef WOLFSSL_HASH_KEEP
        ret = wc_Sha384_Grow(sha384, in, inSz);
    #else
        ret = _ShaUpdate((byte*)sha384->buffer, &sha384->buffLen,
            (byte*)sha384->digest, in, inSz, SHA384_DIGEST_SIZE, CAAM_SHA384);
    #endif
    }

    if (digest != NULL) {
    #ifdef WOLFSSL_HASH_KEEP
        int devId  = sha384->devId;
        void* heap = sha384->heap;

        ret = _ShaFinal((byte*)sha384->digest, SHA384_DIGEST_SIZE, sha384->msg,
            sha384->used, digest, CAAM_SHA384);
        wc_Sha384Free(sha384);
        wc_InitSha384_ex(sha384, heap, devId);
    #else
        ret = _ShaFinal((byte*)sha384->digest, SHA384_DIGEST_SIZE,
            (byte*)sha384->buffer, sha384->buffLen, digest, CAAM_SHA384);
    #endif
    }
    return ret;
}
#endif /* WOLFSSL_SHA384 */



/******************************************************************************
  SHA 512
  ****************************************************************************/
#ifdef WOLFSSL_SHA512
int wc_CAAM_Sha512Hash(wc_Sha512* sha512, const byte* in, word32 inSz,
    byte* digest)
{
    int ret = 0;

    /* in the case of update's just store up all data */
    if (in != NULL) {
    #ifdef WOLFSSL_HASH_KEEP
        ret = wc_Sha512_Grow(sha512, in, inSz);
    #else
        ret = _ShaUpdate((byte*)sha512->buffer, &sha512->buffLen,
            (byte*)sha512->digest, in, inSz, SHA512_DIGEST_SIZE, CAAM_SHA512);
    #endif
    }

    if (digest != NULL) {
    #ifdef WOLFSSL_HASH_KEEP
        int devId  = sha512->devId;
        void* heap = sha512->heap;

        ret = _ShaFinal((byte*)sha512->digest, SHA512_DIGEST_SIZE, sha512->msg,
            sha512->used, digest, CAAM_SHA512);
        wc_Sha512Free(sha512);
        wc_InitSha512_ex(sha512, heap, devId);
    #else
        ret = _ShaFinal((byte*)sha512->digest, SHA512_DIGEST_SIZE,
            (byte*)sha512->buffer, sha512->buffLen, digest, CAAM_SHA512);
    #endif
    }
    return ret;
}
#endif /* WOLFSSL_SHA512 */

#endif /* WOLFSSL_CAAM && WOLFSSL_CAAM_HASH */

