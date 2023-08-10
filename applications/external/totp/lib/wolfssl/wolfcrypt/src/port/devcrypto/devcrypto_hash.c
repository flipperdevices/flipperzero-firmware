/* devcrypto_hash.c
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

#if defined(WOLFSSL_DEVCRYPTO_HASH)

#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/port/devcrypto/wc_devcrypto.h>

#if !defined(NO_SHA256)
#include <wolfssl/wolfcrypt/sha256.h>
#endif

/* dereference structure based on type to get cryptodev context pointer
 * can return NULL on fail case */
static WC_CRYPTODEV* GetHashContext(void* ctx, int type)
{
    switch (type) {
        case CRYPTO_SHA2_256:
            return &((wc_Sha256*)ctx)->ctx;

        default:
            return NULL;
    }

    return NULL;
}


/* generic hash initialization
 * key is for hmac algorithms and keySz is for the size of key buffer
 * key should be null in the case of non hmac algorithms
 * return 0 on success */
static int HashInit(void* ctx, int type, byte* key, word32 keySz)
{
    WC_CRYPTODEV* cdev;

    if ((cdev = GetHashContext(ctx, type)) == NULL) {
        WOLFSSL_MSG("Unsupported hash type");
        return BAD_FUNC_ARG;
    }

    return wc_DevCryptoCreate(cdev, type, key, keySz);
}


/* generic function for updated hash structure
 * returns 0 on success */
static int HashUpdate(void* ctx, int type, const byte* input, word32 inputSz)
{
    WC_CRYPTODEV*   dev;
    struct crypt_op crt;
    byte digest[64];

    if (inputSz == 0) {
        return 0;
    }

    if ((dev = GetHashContext(ctx, type)) == NULL) {
        WOLFSSL_MSG("Unsupported hash type");
        return BAD_FUNC_ARG;
    }

    wc_SetupCrypt(&crt, dev, (byte*)input, inputSz, NULL, digest,
        COP_FLAG_UPDATE, COP_ENCRYPT);
    if (ioctl(dev->cfd, CIOCCRYPT, &crt)) {
        WOLFSSL_MSG("Error with call to ioctl");
        return WC_DEVCRYPTO_E;
    }

    return 0;
}


/* generic function for getting final digest value */
static int GetDigest(void* ctx, int type, byte* out)
{
    WC_CRYPTODEV*   dev;
    struct crypt_op crt;

    if ((dev = GetHashContext(ctx, type)) == NULL) {
        WOLFSSL_MSG("Unsupported hash type");
        return BAD_FUNC_ARG;
    }

    wc_SetupCrypt(&crt, dev, NULL, 0, NULL, out, COP_FLAG_FINAL, COP_ENCRYPT);
    if (ioctl(dev->cfd, CIOCCRYPT, &crt)) {
        WOLFSSL_MSG("Error with call to ioctl");
        return WC_DEVCRYPTO_E;
    }

    return 0;
}

#if !defined(NO_SHA256)

int wc_InitSha256_ex(wc_Sha256* sha, void* heap, int devId)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }

    (void)devId; /* no async for now */
    XMEMSET(sha, 0, sizeof(wc_Sha256));
    sha->heap = heap;

    return HashInit((void*)sha, CRYPTO_SHA2_256, NULL, 0);
}


int wc_Sha256Update(wc_Sha256* sha, const byte* in, word32 sz)
{
    if (sha == NULL || (sz > 0 && in == NULL)) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_DEVCRYPTO_HASH_KEEP
    /* keep full message to hash at end instead of incremental updates */
    if (sha->len < sha->used + sz) {
        if (sha->msg == NULL) {
            sha->msg = (byte*)XMALLOC(sha->used + sz, sha->heap,
                    DYNAMIC_TYPE_TMP_BUFFER);
        } else {
            byte* pt = (byte*)XREALLOC(sha->msg, sha->used + sz, sha->heap,
                    DYNAMIC_TYPE_TMP_BUFFER);
            if (pt == NULL) {
                return MEMORY_E;
        }
            sha->msg = pt;
        }
        if (sha->msg == NULL) {
            return MEMORY_E;
        }
        sha->len = sha->used + sz;
    }
    XMEMCPY(sha->msg + sha->used, in, sz);
    sha->used += sz;
    return 0;
#else
    return HashUpdate(sha, CRYPTO_SHA2_256, in, sz);
#endif
}


int wc_Sha256Final(wc_Sha256* sha, byte* hash)
{
    int ret;

    if (sha == NULL || hash == NULL) {
        return BAD_FUNC_ARG;
    }

    /* help static analysis tools out */
    XMEMSET(hash, 0, WC_SHA256_DIGEST_SIZE);
#ifdef WOLFSSL_DEVCRYPTO_HASH_KEEP
    /* keep full message to hash at end instead of incremental updates */
    if ((ret = HashUpdate(sha, CRYPTO_SHA2_256, sha->msg, sha->used)) < 0) {
        return ret;
    }
    XFREE(sha->msg, sha->heap, DYNAMIC_TYPE_TMP_BUFFER);
    sha->msg = NULL;
#endif
    ret = GetDigest(sha, CRYPTO_SHA2_256, hash);
    if (ret != 0) {
       return ret;
    }

    wc_Sha256Free(sha);
    return wc_InitSha256_ex(sha, sha->heap, 0);
}


int wc_Sha256GetHash(wc_Sha256* sha, byte* hash)
{
    if (sha == NULL || hash == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_DEVCRYPTO_HASH_KEEP
    {
        int ret;
        wc_Sha256 cpy;
        wc_Sha256Copy(sha, &cpy);

        if ((ret = HashUpdate(&cpy, CRYPTO_SHA2_256, cpy.msg, cpy.used)) == 0) {
            /* help static analysis tools out */
            XMEMSET(hash, 0, WC_SHA256_DIGEST_SIZE);
            ret = GetDigest(&cpy, CRYPTO_SHA2_256, hash);
        }
        wc_Sha256Free(&cpy);
        return ret;
    }
#else
    (void)sha;
    (void)hash;

    WOLFSSL_MSG("Compile with WOLFSSL_DEVCRYPTO_HASH_KEEP for this feature");
    return NOT_COMPILED_IN;
#endif
}

int wc_Sha256Copy(wc_Sha256* src, wc_Sha256* dst)
{
    if (src == NULL || dst == NULL) {
        return BAD_FUNC_ARG;
    }

    wc_InitSha256_ex(dst, src->heap, 0);
#ifdef WOLFSSL_DEVCRYPTO_HASH_KEEP
    dst->len  = src->len;
    dst->used = src->used;
    dst->msg = (byte*)XMALLOC(src->len, dst->heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (dst->msg == NULL) {
        return MEMORY_E;
    }
    XMEMCPY(dst->msg, src->msg, src->len);
#endif

    return 0;
}

#endif /* !NO_SHA256 */

#endif /* WOLFSSL_DEVCRYPTO */
