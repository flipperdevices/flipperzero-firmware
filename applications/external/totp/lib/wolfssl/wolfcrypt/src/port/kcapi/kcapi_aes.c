/* kcapi_aes.c
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

#include <errno.h>

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

#if !defined(NO_AES) && defined(WOLFSSL_KCAPI_AES)

#if defined(HAVE_FIPS) && \
    defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2)

    /* set NO_WRAPPERS before headers, use direct internal f()s not wrappers */
    #define FIPS_NO_WRAPPERS

    #ifdef USE_WINDOWS_API
        #pragma code_seg(".fipsA$g")
        #pragma const_seg(".fipsB$g")
    #endif
#endif

#include <wolfssl/wolfcrypt/aes.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/port/kcapi/wc_kcapi.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

/* AES-CBC */
#if defined(HAVE_AES_CBC) && defined(WOLFSSL_KCAPI_AES) && \
                                              !defined(WOLFSSL_NO_KCAPI_AES_CBC)
    static const char WC_NAME_AESCBC[] = "cbc(aes)";

    int wc_AesCbcEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
    {
        int          ret = 0;
        struct iovec iov;

        if (aes == NULL || out == NULL || in == NULL) {
            ret = BAD_FUNC_ARG;
        }

        if (ret == 0 && aes->handle == NULL) {
            ret = kcapi_cipher_init(&aes->handle, WC_NAME_AESCBC, 0);
            if (ret != 0) {
                WOLFSSL_MSG("Error with first time setup of kcapi");
            }
        }

        if (ret == 0 && aes->init == 0) {
            ret = kcapi_cipher_setkey(aes->handle, (byte*)aes->devKey,
                                      aes->keylen);
            if (ret != 0) {
                WOLFSSL_MSG("Error setting key through KCAPI");
            }
        }
        if (ret == 0 && aes->init == 0) {
            ret = (int)kcapi_cipher_stream_init_enc(aes->handle, (byte*)aes->reg,
                                                    NULL, 0);
            if (ret != 0) {
                WOLFSSL_MSG("Error initializing IV through KCAPI");
            }
        }

        if (ret == 0) {
            aes->init = 1;
            iov.iov_base = (byte*)in;
            iov.iov_len = sz;
            ret = (int)kcapi_cipher_stream_update(aes->handle, &iov, 1);
            if (ret < 0) {
                WOLFSSL_MSG("CbcEncrypt error updating through KCAPI");
            }
        }
        if (ret >= 0) {
            iov.iov_base = out;
            iov.iov_len = sz;
            ret = (int)kcapi_cipher_stream_op(aes->handle, &iov, 1);
            if (ret < 0) {
                WOLFSSL_MSG("CbcEncrypt error with op in KCAPI");
            }
        }

        if (ret > 0) {
            ret = 0;
        }

        return ret;
    }

    #ifdef HAVE_AES_DECRYPT
    int wc_AesCbcDecrypt(Aes* aes, byte* out, const byte* in, word32 sz)
    {
        int          ret = 0;
        struct iovec iov;

        if (aes == NULL || out == NULL || in == NULL || \
                                                     sz % AES_BLOCK_SIZE != 0) {
            ret = BAD_FUNC_ARG;
        }

        if (ret == 0 && aes->handle == NULL) {
            ret = kcapi_cipher_init(&aes->handle, WC_NAME_AESCBC, 0);
            if (ret != 0) {
                WOLFSSL_MSG("Error with first time setup of kcapi");
            }
        }

        if (ret == 0 && aes->init == 0) {
            ret = kcapi_cipher_setkey(aes->handle, (byte*)aes->devKey,
                                      aes->keylen);
            if (ret != 0) {
                WOLFSSL_MSG("Error setting key through KCAPI");
            }
        }
        if (ret == 0 && aes->init == 0) {
            ret = (int)kcapi_cipher_stream_init_dec(aes->handle, (byte*)aes->reg,
                                                    NULL, 0);
            if (ret != 0) {
                WOLFSSL_MSG("Error initializing IV through KCAPI");
            }
        }

        if (ret == 0) {
            aes->init = 1;
            iov.iov_base = (byte*)in;
            iov.iov_len = sz;
            ret = (int)kcapi_cipher_stream_update(aes->handle, &iov, 1);
            if (ret < 0) {
                WOLFSSL_MSG("CbcDecrypt error updating through KCAPI");
            }
        }
        if (ret >= 0) {
            iov.iov_base = out;
            iov.iov_len = sz;
            ret = (int)kcapi_cipher_stream_op(aes->handle, &iov, 1);
            if (ret < 0) {
                WOLFSSL_MSG("CbcDecrypt error with op in KCAPI");
            }
        }

        if (ret > 0) {
            ret = 0;
        }

        return ret;
    }
    #endif

#endif /* HAVE_AES_CBC */


#ifdef HAVE_AESGCM


static const char WC_NAME_AESGCM[] = "gcm(aes)";

#ifndef WC_SYSTEM_AESGCM_IV
/* size of IV allowed on system for AES-GCM */
#define WC_SYSTEM_AESGCM_IV 12
#endif

#ifndef WOLFSSL_MAX_AUTH_TAG_SZ
/* size of tag is restricted by system for AES-GCM
 * check 'cat /proc/crypto' to see restricted size */
#define WOLFSSL_MAX_AUTH_TAG_SZ 16
#endif

int wc_AesGcmSetKey(Aes* aes, const byte* key, word32 len)
{
#if defined(AES_MAX_KEY_SIZE)
    const word32 max_key_len = (AES_MAX_KEY_SIZE / 8);
#endif
    int ret = 0;

    if (aes == NULL || !((len == 16) || (len == 24) || (len == 32))) {
        ret = BAD_FUNC_ARG;
    }

#if defined(AES_MAX_KEY_SIZE)
    /* Check key length */
    if ((ret == 0) && (len > max_key_len)) {
        ret = BAD_FUNC_ARG;
    }
#endif

    if (ret == 0) {
        aes->keylen = len;
        aes->rounds = len/4 + 6;

        /* save key until type is known i.e. CBC, ECB, ... */
        XMEMCPY((byte*)(aes->devKey), key, len);
    }

    return ret;
}



/* Performs AES-GCM encryption and returns 0 on success
 */
int wc_AesGcmEncrypt(Aes* aes, byte* out, const byte* in, word32 sz,
                   const byte* iv, word32 ivSz,
                   byte* authTag, word32 authTagSz,
                   const byte* authIn, word32 authInSz)
{
    int     ret = 0;
    byte*   data = NULL;
    word32  dataSz;
    int     inbuflen = 0, outbuflen = 0;
#ifndef KCAPI_USE_XMALLOC
    size_t  pageSz = (size_t)sysconf(_SC_PAGESIZE);
#endif

    /* argument checks */
    if ((aes == NULL) || ((sz != 0 && (in == NULL || out == NULL))) ||
        (iv == NULL) || ((authTag == NULL) && (authTagSz > 0)) ||
        (authTagSz > AES_BLOCK_SIZE) || ((authIn == NULL) && (authInSz > 0))) {
        ret = BAD_FUNC_ARG;
    }

    if ((ret == 0) && ((ivSz != WC_SYSTEM_AESGCM_IV) ||
                                       (authTagSz > WOLFSSL_MAX_AUTH_TAG_SZ))) {
        WOLFSSL_MSG("IV/AAD size not supported on system");
        ret = BAD_FUNC_ARG;
    }

    if ((ret == 0) && (authTagSz < WOLFSSL_MIN_AUTH_TAG_SZ)) {
        WOLFSSL_MSG("GcmEncrypt authTagSz too small error");
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        ret = kcapi_aead_init(&aes->handle, WC_NAME_AESGCM, 0);
        if (ret != 0) {
            WOLFSSL_MSG("Error with first time setup of kcapi");
        }
    }

    if (ret == 0) {
        inbuflen  = (int)kcapi_aead_inbuflen_enc( aes->handle, sz, authInSz,
            authTagSz);
        outbuflen = (int)kcapi_aead_outbuflen_enc(aes->handle, sz, authInSz,
            authTagSz);
        dataSz = (inbuflen > outbuflen) ? inbuflen : outbuflen;
    #ifdef KCAPI_USE_XMALLOC
        data = (byte *)XMALLOC(dataSz, aes->heap, DYNAMIC_TYPE_TMP_BUFFER);
        if (data == NULL) {
            ret = MEMORY_E;
        }
    #else
        ret = posix_memalign((void*)&data, pageSz, dataSz);
        if (ret < 0) {
            ret = MEMORY_E;
        }
    #endif
    }

    if (ret >= 0) {
        ret = kcapi_aead_setkey(aes->handle, (byte*)aes->devKey, aes->keylen);
        if (ret != 0) {
            WOLFSSL_MSG("GcmEncrypt set key failed");
        }
    }

    if (ret == 0) {
        ret = kcapi_aead_settaglen(aes->handle, authTagSz);
        if (ret != 0) {
            WOLFSSL_MSG("GcmEncrypt set tag len failed");
        }
    }

    if (ret == 0) {
        kcapi_aead_setassoclen(aes->handle, authInSz);
        XMEMCPY(data, authIn, authInSz);
        XMEMCPY(data + authInSz, in, sz);

        ret = (int)kcapi_aead_encrypt(aes->handle, data, inbuflen, iv, data,
            outbuflen, KCAPI_ACCESS_HEURISTIC);
        if (ret < 0) {
            WOLFSSL_MSG("GcmEncrypt failed");
        }
        else if (ret != outbuflen) {
            WOLFSSL_MSG("GcmEncrypt produced wrong output length");
            ret = BAD_FUNC_ARG;
        }
        else {
            ret = 0; /* success */
        }
    }

    if (ret == 0) {
        XMEMCPY(out, data + authInSz, sz);
        XMEMCPY(authTag, data + authInSz + sz, authTagSz);
    }

    if (data != NULL) {
    #ifdef KCAPI_USE_XMALLOC
        XFREE(data, aes->heap, DYNAMIC_TYPE_TMP_BUFFER);
    #else
        free(data);
    #endif
    }
    if (aes != NULL && aes->handle != NULL) {
        kcapi_aead_destroy(aes->handle);
        aes->handle = NULL;
    }

    return ret;
}

#if defined(HAVE_AES_DECRYPT) || defined(HAVE_AESGCM_DECRYPT)
/* Performs AES-GCM decryption and returns 0 on success
 */
int wc_AesGcmDecrypt(Aes* aes, byte* out, const byte* in, word32 sz,
                     const byte* iv, word32 ivSz,
                     const byte* authTag, word32 authTagSz,
                     const byte* authIn, word32 authInSz)
{
    int     ret = 0;
    byte*   data = NULL;
    word32  dataSz;
    int     inbuflen = 0, outbuflen = 0;
#ifndef KCAPI_USE_XMALLOC
    size_t  pageSz = (size_t)sysconf(_SC_PAGESIZE);
#endif

    /* argument checks */
    if ((aes == NULL) || ((sz != 0 && (in == NULL || out == NULL))) ||
        (iv == NULL) || ((authTag == NULL) && (authTagSz > 0)) ||
        (authTagSz > AES_BLOCK_SIZE) || ((authIn == NULL) && (authInSz > 0))) {
        ret = BAD_FUNC_ARG;
    }

    if ((ret == 0) && ((ivSz != WC_SYSTEM_AESGCM_IV) ||
                                       (authTagSz > WOLFSSL_MAX_AUTH_TAG_SZ))) {
        WOLFSSL_MSG("IV/AAD size not supported on system");
        ret = BAD_FUNC_ARG;
    }

    if ((ret == 0) && (authTagSz < WOLFSSL_MIN_AUTH_TAG_SZ)) {
        WOLFSSL_MSG("GcmDecrypt authTagSz too small error");
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        ret = kcapi_aead_init(&aes->handle, WC_NAME_AESGCM, 0);
        if (ret != 0) {
            WOLFSSL_MSG("Error with first time setup of kcapi");
        }
    }

    if (ret == 0) {
        inbuflen  = (int)kcapi_aead_inbuflen_dec( aes->handle, sz, authInSz,
            authTagSz);
        outbuflen = (int)kcapi_aead_outbuflen_dec(aes->handle, sz, authInSz,
            authTagSz);
        dataSz = (inbuflen > outbuflen) ? inbuflen : outbuflen;
    #ifdef KCAPI_USE_XMALLOC
        data = (byte*)XMALLOC(dataSz, aes->heap, DYNAMIC_TYPE_TMP_BUFFER);
        if (data == NULL) {
            ret = MEMORY_E;
        }
    #else
        ret = posix_memalign((void*)&data, pageSz, dataSz);
        if (ret < 0) {
            ret = MEMORY_E;
        }
    #endif
    }

    if (ret >= 0) {
        ret = kcapi_aead_setkey(aes->handle, (byte*)aes->devKey, aes->keylen);
        if (ret != 0) {
            WOLFSSL_MSG("GcmDecrypt set key failed");
        }
    }

    if (ret == 0) {
        ret = kcapi_aead_settaglen(aes->handle, authTagSz);
    }

    if (ret == 0) {
        kcapi_aead_setassoclen(aes->handle, authInSz);
        XMEMCPY(data, authIn, authInSz);
        XMEMCPY(data + authInSz, in, sz);
        XMEMCPY(data + authInSz + sz, authTag, authTagSz);

        ret = (int)kcapi_aead_decrypt(aes->handle, data, inbuflen, iv, data,
            outbuflen, KCAPI_ACCESS_HEURISTIC);
        if (ret < 0) {
            WOLFSSL_MSG("GcmDecrypt failed");
            if (ret == -EBADMSG)
                ret = AES_GCM_AUTH_E;
        }
        else if (ret != outbuflen) {
            WOLFSSL_MSG("GcmDecrypt produced wrong output length");
            ret = BAD_FUNC_ARG;
        }
        else {
            ret = 0; /* success */
        }
    }

    if (ret == 0) {
        XMEMCPY(out, data + authInSz, sz);
    }

    if (data != NULL) {
    #ifdef KCAPI_USE_XMALLOC
        XFREE(data, aes->heap, DYNAMIC_TYPE_TMP_BUFFER);
    #else
        free(data);
    #endif
    }
    if (aes != NULL && aes->handle != NULL) {
        kcapi_aead_destroy(aes->handle);
        aes->handle = NULL;
    }

    return ret;
}
#endif /* HAVE_AES_DECRYPT || HAVE_AESGCM_DECRYPT */
#endif /* HAVE_AESGCM */

#endif /* !NO_AES && WOLFSSL_KCAPI_AES */

