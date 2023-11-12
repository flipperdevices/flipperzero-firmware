/* caam_sha.c
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

#if defined(WOLFSSL_IMX6_CAAM) && !defined(NO_IMX6_CAAM_HASH)

#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif


#if defined(__INTEGRITY) || defined(INTEGRITY)
#include <INTEGRITY.h>
#endif
#include <wolfssl/wolfcrypt/port/caam/caam_driver.h>
#include <wolfssl/wolfcrypt/port/caam/wolfcaam.h>

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

/******************************************************************************
  Common Code Between SHA Functions
  ****************************************************************************/

static int _InitSha(wc_Sha* sha, void* heap, int devId, word32 digestSz,
    word32 type)
{
    Buffer buf[1];
    word32 arg[4];
    int ret;

    (void)heap;
    (void)devId;

    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }

    XMEMSET(sha, 0, sizeof(Sha));

    /* Set buffer for context */
    buf[0].BufferType = DataBuffer | LastBuffer;
    buf[0].TheAddress = (Address)sha->ctx;
    buf[0].Length     = digestSz + WC_CAAM_CTXLEN;
    buf[0].Transferred = 0;

    arg[0] = CAAM_ALG_INIT;
    arg[1] = digestSz + WC_CAAM_CTXLEN;

    if ((ret = wc_caamAddAndWait(buf, arg, type)) != 0) {
        WOLFSSL_MSG("Error with CAAM SHA init");
        return ret;
    }

    return 0;
}


static int _ShaUpdate(wc_Sha* sha, const byte* data, word32 len, word32 digestSz,
    word32 type)
{
    Buffer buf[2];
    word32 arg[4];
    int ret;
    byte* local;

    if (sha == NULL ||(data == NULL && len > 0)) {
        return BAD_FUNC_ARG;
    }

    if (len == 0) return 0; /* nothing to do */

    local = (byte*)sha->buffer;
    /* check for filling out existing buffer */
    if (sha->buffLen > 0) {
        word32 add = min(len, WC_CAAM_HASH_BLOCK - sha->buffLen);
        XMEMCPY(&local[sha->buffLen], data, add);

        sha->buffLen += add;
        data         += add;
        len          -= add;

        if (sha->buffLen == WC_CAAM_HASH_BLOCK) {
            /* Set buffer for context */
            buf[0].BufferType = DataBuffer;
            buf[0].TheAddress = (Address)sha->ctx;
            buf[0].Length     = digestSz + WC_CAAM_CTXLEN;
            buf[0].Transferred = 0;

            /* data to update with */
            buf[1].BufferType = DataBuffer | LastBuffer;
            buf[1].TheAddress = (Address)sha->buffer;
            buf[1].Length     = sha->buffLen;
            buf[1].Transferred = 0;

            arg[0] = CAAM_ALG_UPDATE;
            arg[1] = digestSz + WC_CAAM_CTXLEN;

            if ((ret = wc_caamAddAndWait(buf, arg, type)) != 0) {
                WOLFSSL_MSG("Error with CAAM SHA update");
                return ret;
            }
            sha->buffLen = 0; /* cleared out buffer */
        }
    }

    /* check if multiple full blocks can be done */
    if (len >= WC_CAAM_HASH_BLOCK) {
        word32 sz = len / WC_CAAM_HASH_BLOCK;
        sz = sz * WC_CAAM_HASH_BLOCK;

        /* Set buffer for context */
        buf[0].BufferType = DataBuffer;
        buf[0].TheAddress = (Address)sha->ctx;
        buf[0].Length     = digestSz + WC_CAAM_CTXLEN;
        buf[0].Transferred = 0;

        /* data to update with */
        buf[1].BufferType = DataBuffer | LastBuffer;
        buf[1].TheAddress = (Address)data;
        buf[1].Length     = sz;
        buf[1].Transferred = 0;

        arg[0] = CAAM_ALG_UPDATE;
        arg[1] = digestSz + WC_CAAM_CTXLEN;

        if ((ret = wc_caamAddAndWait(buf, arg, type)) != 0) {
            WOLFSSL_MSG("Error with CAAM SHA update");
            return ret;
        }

        len  -= sz;
        data += sz;
    }

    /* check for left overs */
    if (len > 0) {
        word32 add = min(len, WC_CAAM_HASH_BLOCK - sha->buffLen);
        XMEMCPY(&local[sha->buffLen], data, add);
        sha->buffLen += add;
    }

    return 0;
}


static int _ShaFinal(wc_Sha* sha, byte* out, word32 digestSz,
    word32 type)
{
    Buffer buf[2];
    word32 arg[4];
    int ret;

    if (sha == NULL || out == NULL) {
        return BAD_FUNC_ARG;
    }

    /* Set buffer for context */
    buf[0].BufferType = DataBuffer;
    buf[0].TheAddress = (Address)sha->ctx;
    buf[0].Length     = digestSz + WC_CAAM_CTXLEN;
    buf[0].Transferred = 0;

    /* add any potential left overs */
    buf[1].BufferType = DataBuffer | LastBuffer;
    buf[1].TheAddress = (Address)sha->buffer;
    buf[1].Length     = sha->buffLen;
    buf[1].Transferred = 0;

    arg[0] = CAAM_ALG_FINAL;
    arg[1] = digestSz + WC_CAAM_CTXLEN;

    if ((ret = wc_caamAddAndWait(buf, arg, type)) != 0) {
        WOLFSSL_MSG("Error with CAAM SHA init");
        return ret;
    }

    return 0;
}

/******************************************************************************
  MD5
  ****************************************************************************/
#if !defined(NO_MD5)
int wc_InitMd5_ex(wc_Md5* sha, void* heap, int devId)
{
    return _InitSha(sha, heap, devId, MD5_DIGEST_SIZE, CAAM_MD5);
}


int wc_Md5Update(wc_Md5* sha, const byte* data, word32 len)
{
    return _ShaUpdate(sha, data, len, MD5_DIGEST_SIZE, CAAM_MD5);
}


int wc_Md5Final(wc_Md5* sha, byte* hash)
{
    int ret;
    if ((ret = _ShaFinal(sha, hash, MD5_DIGEST_SIZE, CAAM_MD5)) != 0) {
        return ret;
    }

    XMEMCPY(hash, (byte*)sha->ctx, MD5_DIGEST_SIZE);
    return _InitSha(sha, NULL, 0, MD5_DIGEST_SIZE, CAAM_MD5);
}
#endif /* !NO_MD5 */


/******************************************************************************
  SHA 1
  ****************************************************************************/
#if !defined(NO_SHA)
int wc_InitSha_ex(wc_Sha* sha, void* heap, int devId)
{
    return _InitSha(sha, heap, devId, SHA_DIGEST_SIZE, CAAM_SHA);
}


int wc_ShaUpdate(wc_Sha* sha, const byte* data, word32 len)
{
    return _ShaUpdate(sha, data, len, SHA_DIGEST_SIZE, CAAM_SHA);
}


int wc_ShaFinal(wc_Sha* sha, byte* out)
{
    int ret;
    if ((ret = _ShaFinal(sha, out, SHA_DIGEST_SIZE, CAAM_SHA)) != 0) {
        return ret;
    }

    XMEMCPY(out, (byte*)sha->ctx, SHA_DIGEST_SIZE);
    return _InitSha(sha, NULL, 0, SHA_DIGEST_SIZE, CAAM_SHA);
}
#endif /* !NO_SHA */


/******************************************************************************
  SHA 224
  ****************************************************************************/
#ifdef WOLFSSL_SHA224
int wc_InitSha224_ex(wc_Sha224* sha, void* heap, int devId)
{
    return _InitSha(sha, heap, devId, SHA256_DIGEST_SIZE, CAAM_SHA224);
}


int wc_Sha224Update(wc_Sha224* sha, const byte* data, word32 len)
{
    return _ShaUpdate(sha, data, len, SHA256_DIGEST_SIZE, CAAM_SHA224);
}


int wc_Sha224Final(wc_Sha224* sha, byte* out)
{
    int ret;
    if ((ret = _ShaFinal(sha, out, SHA256_DIGEST_SIZE, CAAM_SHA224)) != 0) {
        return ret;
    }

    XMEMCPY(out, (byte*)sha->ctx, SHA224_DIGEST_SIZE);
    return _InitSha(sha, NULL, 0, SHA256_DIGEST_SIZE, CAAM_SHA224);
}
#endif /* WOLFSSL_SHA224 */


/******************************************************************************
  SHA 256
  ****************************************************************************/
#if !defined(NO_SHA256)
int wc_InitSha256_ex(wc_Sha256* sha, void* heap, int devId)
{
    return _InitSha(sha, heap, devId, SHA256_DIGEST_SIZE, CAAM_SHA256);
}


int wc_Sha256Update(wc_Sha256* sha, const byte* data, word32 len)
{
    return _ShaUpdate(sha, data, len, SHA256_DIGEST_SIZE, CAAM_SHA256);
}


int wc_Sha256Final(wc_Sha256* sha, byte* out)
{
    int ret;
    if ((ret = _ShaFinal(sha, out, SHA256_DIGEST_SIZE, CAAM_SHA256)) != 0) {
        return ret;
    }

    XMEMCPY(out, (byte*)sha->ctx, SHA256_DIGEST_SIZE);
    return _InitSha(sha, NULL, 0, SHA256_DIGEST_SIZE, CAAM_SHA256);
}
#endif /* !NO_SHA256 */


/******************************************************************************
  SHA 384
  ****************************************************************************/
#ifdef WOLFSSL_SHA384
int wc_InitSha384_ex(wc_Sha384* sha, void* heap, int devId)
{
    return _InitSha(sha, heap, devId, SHA512_DIGEST_SIZE, CAAM_SHA384);
}


int wc_Sha384Update(wc_Sha384* sha, const byte* data, word32 len)
{
    return _ShaUpdate(sha, data, len, SHA512_DIGEST_SIZE, CAAM_SHA384);
}


int wc_Sha384Final(wc_Sha384* sha, byte* out)
{
    int ret;
    if ((ret = _ShaFinal(sha, out, SHA512_DIGEST_SIZE, CAAM_SHA384)) != 0) {
        return ret;
    }

    XMEMCPY(out, (byte*)sha->ctx, SHA384_DIGEST_SIZE);
    return _InitSha(sha, NULL, 0, SHA512_DIGEST_SIZE, CAAM_SHA384);
}
#endif /* WOLFSSL_SHA384 */



/******************************************************************************
  SHA 512
  ****************************************************************************/
#ifdef WOLFSSL_SHA512
int wc_InitSha512_ex(wc_Sha512* sha, void* heap, int devId)
{
    return _InitSha(sha, heap, devId, SHA512_DIGEST_SIZE, CAAM_SHA512);
}


int wc_Sha512Update(wc_Sha512* sha, const byte* data, word32 len)
{
    return _ShaUpdate(sha, data, len, SHA512_DIGEST_SIZE, CAAM_SHA512);
}


int wc_Sha512Final(wc_Sha512* sha, byte* out)
{
    int ret;
    if ((ret = _ShaFinal(sha, out, SHA512_DIGEST_SIZE, CAAM_SHA512)) != 0) {
        return ret;
    }

    XMEMCPY(out, (byte*)sha->ctx, SHA512_DIGEST_SIZE);
    return _InitSha(sha, NULL, 0, SHA512_DIGEST_SIZE, CAAM_SHA512);
}
#endif /* WOLFSSL_SHA512 */

#endif /* WOLFSSL_IMX6_CAAM && !NO_IMX6_CAAM_HASH */
