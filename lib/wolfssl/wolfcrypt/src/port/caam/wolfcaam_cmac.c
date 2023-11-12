/* wolfcaam_cmac.c
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

#if defined(WOLFSSL_CAAM) && defined(WOLFSSL_CMAC) && defined(WOLFSSL_CAAM_CMAC)

#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/port/caam/wolfcaam.h>
#include <wolfssl/wolfcrypt/port/caam/wolfcaam_cmac.h>

/* returns 0 on success */
int wc_CAAM_Cmac(Cmac* cmac, const byte* key, word32 keySz, const byte* in,
        word32 inSz, byte* out, word32* outSz, int type, void* ctx)
{
    word32 args[4] = {0};
    CAAM_BUFFER buf[9];
    word32 idx = 0;
    byte scratch[AES_BLOCK_SIZE];
    int ret;
    int blocks = 0;

    byte* pt = (byte*)in;
    int   sz = inSz;
    (void)type;

    args[0] = 0;
    if (outSz != NULL && *outSz > 16) {
        return BAD_FUNC_ARG;
    }

    if (out != NULL && outSz == NULL) {
        return BAD_FUNC_ARG;
    }

    if (key != NULL || ctx != NULL) {
        XMEMSET(&cmac->aes, 0, sizeof(Aes));
        if (cmac->blackKey == 0) {
            if (ctx != NULL) {
                cmac->blackKey = 1;
                XMEMCPY((byte*)cmac->aes.key, (byte*)ctx, keySz + 16);
            }
            else {
                XMEMCPY((byte*)cmac->aes.key, (byte*)key, keySz);
            }
        }
        cmac->keylen = keySz;
        cmac->initialized = 0;
        cmac->bufferSz = 0;
        XMEMSET(cmac->buffer, 0, AES_BLOCK_SIZE);
        return 0;
    }

    buf[idx].TheAddress = (CAAM_ADDRESS)cmac->aes.key;
    buf[idx].Length =  cmac->keylen;
    idx++;

    buf[idx].TheAddress = (CAAM_ADDRESS)cmac->ctx;
    buf[idx].Length = sizeof(cmac->ctx);
    idx++;

    if (in != NULL) {
    #ifdef WOLFSSL_HASH_KEEP
        if (wc_CMAC_Grow(cmac, in, inSz) != 0) {
            WOLFSSL_MSG("Error growing CMAC buffer");
            return -1;
        }
    #else
        args[0] |= CAAM_ALG_UPDATE;

        /* first take care of any left overs */
        if (cmac->bufferSz > 0) {
            word32 add;

            if (cmac->bufferSz > AES_BLOCK_SIZE) {
                WOLFSSL_MSG("Error with CMAC buffer size");
                return -1;
            }
            add = (sz < ((int)(AES_BLOCK_SIZE - cmac->bufferSz))) ? sz :
                   (int)(AES_BLOCK_SIZE - cmac->bufferSz);
            XMEMCPY(&cmac->buffer[cmac->bufferSz], pt, add);

            cmac->bufferSz += add;
            pt += add;
            sz -= add;
        }

        /* flash out temporary storage for block size if full and more data
         * is coming, otherwise hold it until final operation */
        if (cmac->bufferSz == AES_BLOCK_SIZE && (sz > 0)) {
            buf[idx].TheAddress = (CAAM_ADDRESS)scratch;
            buf[idx].Length = cmac->bufferSz;
            idx++;
            blocks++;
            cmac->bufferSz = 0;
            XMEMCPY(scratch, (byte*)cmac->buffer, AES_BLOCK_SIZE);
        }

        /* In order to trigger read of CTX state there needs to be some data
         * saved until final call */
        if ((sz >= AES_BLOCK_SIZE) && (sz % AES_BLOCK_SIZE == 0)) {

            if (cmac->bufferSz > 0) {
                /* this case should never be hit */
                return BAD_FUNC_ARG;
            }

            XMEMCPY(&cmac->buffer[0], pt + sz - AES_BLOCK_SIZE, AES_BLOCK_SIZE);
            cmac->bufferSz = AES_BLOCK_SIZE;
            sz -= AES_BLOCK_SIZE;
        }

        if (sz >= AES_BLOCK_SIZE) {
            buf[idx].TheAddress = (CAAM_ADDRESS)pt;
            buf[idx].Length = sz - (sz % AES_BLOCK_SIZE);
            blocks += sz / AES_BLOCK_SIZE;
            sz -= buf[idx].Length;
            pt += buf[idx].Length;
            idx++;
        }
    #endif
    }

    if (out != NULL) {
    #ifdef WOLFSSL_HASH_KEEP
        if (cmac->msg != NULL) {
            buf[idx].TheAddress = (CAAM_ADDRESS)cmac->msg;
            buf[idx].Length = cmac->used;
            idx++;
        }
    #else
        /* handle any leftovers */
        if (cmac->bufferSz > 0) {
            buf[idx].TheAddress = (CAAM_ADDRESS)cmac->buffer;
            buf[idx].Length = cmac->bufferSz;
            idx++;
        }
    #endif
        args[0] |= CAAM_ALG_FINAL;
        blocks++; /* always run on final call */
    }

    /* set key size */
    args[1] = cmac->keylen;
    args[2] = cmac->blackKey;

    /* only call down to CAAM if we have a full block to do or is final */
    if (blocks > 0) {
        if (cmac->initialized == 0) {
            args[0] |= CAAM_ALG_INIT;
            cmac->initialized = 1;
        }

        ret = wc_caamAddAndWait(buf, idx, args, CAAM_CMAC);
        if (ret != 0) {
            return -1;
        }
    }

    if (out != NULL) {
        XMEMCPY(out, cmac->ctx, *outSz);
    }

    /* store leftovers */
    if (sz > 0) {
        word32 add = (sz < (int)(AES_BLOCK_SIZE - cmac->bufferSz))?
                                              (word32)sz :
                                              (AES_BLOCK_SIZE - cmac->bufferSz);

        if (pt == NULL) {
            return MEMORY_E;
        }
        XMEMCPY(&cmac->buffer[cmac->bufferSz], pt, add);
        cmac->bufferSz += add;
    }

    (void)scratch;
    return 0;
}

#endif /* WOLFSSL_CAAM && WOLFSSL_CMAC */

