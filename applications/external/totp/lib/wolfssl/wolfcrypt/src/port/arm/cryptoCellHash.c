/* cryptoCellHash.c
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

/* This source is included in wc_port.c */
/* WOLFSSL_CRYPTOCELL_HASH_C is defined by wc_port.c in case compile tries
    to include this .c directly */
#ifdef WOLFSSL_CRYPTOCELL_HASH_C
#if !defined(NO_SHA256) && defined(WOLFSSL_CRYPTOCELL)

#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/port/arm/cryptoCell.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

int wc_InitSha256_ex(wc_Sha256* sha256, void* heap, int devId)
{
    CRYSError_t ret = 0;

    (void)heap;
    (void)devId;

    if (sha256 == NULL)
        return BAD_FUNC_ARG;

    XMEMSET(sha256->digest, 0, sizeof(sha256->digest));

    /* initializes the HASH context and machine to the supported mode.*/
    ret = CRYS_HASH_Init(&sha256->ctx, CRYS_HASH_SHA256_mode);

    if (ret != SA_SILIB_RET_OK){
        WOLFSSL_MSG("Error CRYS_HASH_Init failed");
    }

    return ret;
}

int wc_InitSha256(Sha256* sha256)
{
    return wc_InitSha256_ex(sha256, NULL, INVALID_DEVID);
}

int wc_Sha256Update(wc_Sha256* sha256, const byte* data, word32 len)
{
    CRYSError_t         ret = 0;
    size_t              length;
    size_t              remaining = len;
    byte const *        p_cur     = data;

    if (sha256 == NULL || (data == NULL && len > 0)) {
        return BAD_FUNC_ARG;
    }

    if (data == NULL && len == 0) {
        /* valid, but do nothing */
        return 0;
    }

    /* If the input is larger than CC310_MAX_LENGTH_DMA, split into smaller */
    do {
        length = (remaining > CC310_MAX_LENGTH_DMA) ?
                    CC310_MAX_LENGTH_DMA : remaining;

        ret = CRYS_HASH_Update(&sha256->ctx, (uint8_t *)p_cur, length);

        remaining -= length;
        p_cur += length;

    } while (ret == CRYS_OK && remaining > 0);

    return ret;
}

int wc_Sha256Final(wc_Sha256* sha256, byte* hash)
{
    CRYSError_t ret = 0;
    CRYS_HASH_Result_t hashResult;

    if (sha256 == NULL || hash == NULL) {
        return BAD_FUNC_ARG;
    }

    ret = CRYS_HASH_Finish(&sha256->ctx, hashResult);

    if (ret != SA_SILIB_RET_OK){
        WOLFSSL_MSG("Error CRYS_HASH_Finish failed");
        return ret;
    }
    XMEMCPY(sha256->digest, hashResult, WC_SHA256_DIGEST_SIZE);

    XMEMCPY(hash, sha256->digest, WC_SHA256_DIGEST_SIZE);

    /* reset state */
    return wc_InitSha256_ex(sha256, NULL, INVALID_DEVID);
}

void wc_Sha256Free(wc_Sha256* sha256)
{
    if (sha256 == NULL)
        return;
}

#endif /* !NO_SHA256 && WOLFSSL_CRYPTOCELL */
#endif /* WOLFSSL_CRYPTOCELL_HASH_C */
