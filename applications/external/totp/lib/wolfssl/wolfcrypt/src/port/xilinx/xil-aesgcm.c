/* xil-aesgcm.c
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

/* Make calls to Xilinx hardened AES-GCM crypto */

#if !defined(NO_AES) && defined(WOLFSSL_XILINX_CRYPT)

#include <wolfssl/wolfcrypt/aes.h>

#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

enum {
    AEAD_NONCE_SZ       = 12,
    AES_GCM_AUTH_SZ     = 16, /* AES-GCM Auth Tag length    */
};


#ifdef HAVE_AESGCM

#if defined(WOLFSSL_XILINX_CRYPT_VERSAL)

#include "xsecure_aesclient.h"

typedef int (*init_fn)(XSecure_ClientInstance *InstancePtr,
                       XSecure_AesKeySource KeySrc,
                       u32 Size,
                       u64 IvAddr);

enum aad_state {
    SW_AAD,
    HW_ENGINE_AAD
};

#define XIL_AESGCM_ALIGN 4u
#define NEEDS_ALIGNMENT(ptr, align) (((wc_ptr_t)ptr % align) != 0u)

static WC_INLINE int aligned_xmalloc(byte** buf, byte** aligned, void* heap, word32 size, word32 align)
{
#ifndef NO_WOLFSSL_ALLOC_ALIGN
    byte* tmp = (byte*)XMALLOC(size + align, heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (tmp == NULL)
        return MEMORY_E;

    *buf = tmp;
    *aligned = tmp + (align - ((wc_ptr_t)tmp % align));

    return 0;
#else
    WOLFSSL_MSG("AES-ECB encrypt with bad alignment");
    WOLFSSL_ERROR_VERBOSE(BAD_ALIGN_E);
    return BAD_ALIGN_E;
#endif
}

static WC_INLINE void aligned_xfree(void* buf, void* heap)
{
	if (buf == NULL)
		return;
	XFREE(buf, heap, DYNAMIC_TYPE_TMP_BUFFER);
}

static WC_INLINE int check_keysize(word32 len)
{
    if (len != AES_128_KEY_SIZE && len != AES_256_KEY_SIZE) {
        WOLFSSL_MSG("Expecting a 128 or 256 bit key");
        return BAD_FUNC_ARG;
    }
    return 0;
}

int wc_AesGcmSetKey_ex(Aes* aes, const byte* key, word32 len, word32 kup)
{
    int ret;

    if (aes == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }

    if (check_keysize(len)) {
        return BAD_FUNC_ARG;
    }

    ret = wc_InitXsecure(&(aes->xSec));
    if (ret != 0) {
        WOLFSSL_MSG("Unable to initialize xSecure for AES-GCM");
        return ret;
    }
    ret = XSecure_AesInitialize(&(aes->xSec.cinst));
    if (ret != XST_SUCCESS) {
        WOLFSSL_XIL_ERROR("Failed to initialize AES client instance", ret);
        return WC_HW_E;
    }
#ifdef WC_XIL_AESGCM_DPA_CM
    ret = XSecure_AesSetDpaCm(&(aes->xSec.cinst), 1);
    if (ret != XST_SUCCESS) {
        WOLFSSL_XIL_ERROR("Failed to enable AES DPA countermeasures", ret);
        return WC_HW_E;
    }
#endif
    aes->keylen = len;
    aes->kup = kup;
    aes->xKeySize =
            len == AES_128_KEY_SIZE ? XSECURE_AES_KEY_SIZE_128 :
                                      XSECURE_AES_KEY_SIZE_256;
    XMEMCPY(aes->keyInit, key, len);

    return 0;
}

static WC_INLINE int setup(Aes* aes,
                           init_fn init,
                           const byte* iv,
                           const byte* authIn, word32 authInSz)
{
    ALIGN64 byte iv_[AEAD_NONCE_SZ];
    byte *piv = (byte*)iv;
    byte *aad = (byte*)authIn;
    byte *aad_buf = NULL;
    int err;

    WOLFSSL_XIL_DCACHE_INVALIDATE_RANGE((UINTPTR)aes->keyInit,
                                        sizeof(aes->keyInit));

    if (XSecure_AesWriteKey(&(aes->xSec.cinst), aes->kup, aes->xKeySize,
                            XIL_CAST_U64(aes->keyInit))) {
        WOLFSSL_XIL_MSG("Failed to write key");
        return 1;
    }

    if (NEEDS_ALIGNMENT(iv, XIL_AESGCM_ALIGN)) {
        XMEMCPY(iv_, iv, AEAD_NONCE_SZ);
        piv = iv_;
    }
    WOLFSSL_XIL_DCACHE_INVALIDATE_RANGE((UINTPTR)piv, AEAD_NONCE_SZ);

    if (init(&(aes->xSec.cinst), aes->kup, aes->xKeySize, XIL_CAST_U64(piv))) {
        WOLFSSL_XIL_MSG("Failed to init");
        return 1;
    }

    /* Stop here in case there's no AAD ...
     */
    if (authIn == NULL || authInSz == 0) {
        aes->aadStyle = HW_ENGINE_AAD;
        return 0;
    }
    /* ... or the AAD length is no multiple of 16,
     * as the HW-engine can't handle this.
     */
    if ((authInSz % WOLFSSL_XSECURE_QWORD_SIZE) != 0) {
        aes->aadStyle = SW_AAD;
        return 0;
    }

    if (NEEDS_ALIGNMENT(authIn, XIL_AESGCM_ALIGN)) {
        err = aligned_xmalloc(&aad_buf, (byte**)&aad, aes->heap, authInSz,
        XIL_AESGCM_ALIGN);
        if (err) {
            WOLFSSL_MSG("Failed to alloc memory for AESGCM AAD alignment");
            return 1;
        }
        XMEMCPY((void* )aad, authIn, authInSz);
    }

    WOLFSSL_XIL_DCACHE_INVALIDATE_RANGE((UINTPTR)aad, authInSz);

    if (XSecure_AesUpdateAad(&(aes->xSec.cinst), XIL_CAST_U64(authIn),
                             authInSz)) {
        WOLFSSL_XIL_MSG("Failed to set AAD");
        err = 1;
    } else {
        aes->aadStyle = HW_ENGINE_AAD;
        err = 0;
    }

    aligned_xfree(aad_buf, aes->heap);

    return err;
}

static WC_INLINE int handle_aad(       Aes* aes,
                                const byte* data, word32 sz,
                                const byte* iv,
                                      byte* authTag,
                                const byte* authIn, word32 authInSz) {
    int ret;
    byte scratch[AES_BLOCK_SIZE];
    byte initalCounter[AES_BLOCK_SIZE] = { 0 };
    XMEMCPY(initalCounter, iv, AEAD_NONCE_SZ);
    initalCounter[AES_BLOCK_SIZE - 1] = 1;
    GHASH(&aes->gcm, authIn, authInSz, data, sz, authTag, AES_GCM_AUTH_SZ);
    ret = wc_AesEncryptDirect(aes, scratch, initalCounter);
    if (ret == 0)
        xorbuf(authTag, scratch, AES_GCM_AUTH_SZ);
    return ret;
}

int wc_AesGcmEncrypt(       Aes* aes, byte* out,
                     const byte* in, word32 sz,
                     const byte* iv, word32 ivSz,
                           byte* authTag, word32 authTagSz,
                     const byte* authIn, word32 authInSz)
{
    int ret = 0;
    ALIGN64 u8 tag[AES_GCM_AUTH_SZ];
    byte *in_aligned;
    byte *in_buf = NULL;
    byte *out_aligned;
    byte *out_buf = NULL;

    if (aes == NULL || (in == NULL && sz > 0u) || iv == NULL || authTag == NULL
        || authTagSz != AES_GCM_AUTH_SZ) {
        return BAD_FUNC_ARG;
    }

    if (ivSz != AEAD_NONCE_SZ) {
        WOLFSSL_MSG("Expecting an IV size of 12");
        return BAD_FUNC_ARG;
    }

    if (in == NULL || out == NULL || sz == 0u) {
        return handle_aad(aes, NULL, 0u, iv, authTag, authIn, authInSz);
    }

    if (check_keysize(aes->keylen)) {
        return BAD_FUNC_ARG;
    }

    if (setup(aes, XSecure_AesEncryptInit, iv, authIn, authInSz)) {
        return WC_HW_E;
    }

    in_aligned = (byte*) in;
    if (NEEDS_ALIGNMENT(in, XIL_AESGCM_ALIGN)) {
        ret = aligned_xmalloc(&in_buf, &in_aligned, aes->heap, sz,
                              XIL_AESGCM_ALIGN);
        if (ret) {
            WOLFSSL_MSG(
                    "Failed to alloc memory for AESGCM Encrypt alignment (in)");
            return 1;
        }
        XMEMCPY(in_aligned, in, sz);
    }

    if (NEEDS_ALIGNMENT(out, XIL_AESGCM_ALIGN)) {
        if (in != in_aligned) {
            /* In case `in` has been copied already, reuse that buffer
             * and also write to it instead of allocating another one.
             */
            out_aligned = in_aligned;
        } else {
            ret = aligned_xmalloc(&out_buf, &out_aligned, aes->heap, sz,
                                  XIL_AESGCM_ALIGN);
            if (ret) {
                aligned_xfree(in_buf, aes->heap);
                WOLFSSL_MSG(
                        "Failed to alloc memory for AESGCM Encrypt alignment (out)");
                return 1;
            }
            XMEMCPY(out_aligned, out, sz);
        }
    } else {
        out_aligned = out;
    }

    WOLFSSL_XIL_DCACHE_INVALIDATE_RANGE((UINTPTR)in_aligned, sz);
    WOLFSSL_XIL_DCACHE_INVALIDATE_RANGE((UINTPTR)out_aligned, sz);
    WOLFSSL_XIL_DCACHE_INVALIDATE_RANGE((UINTPTR)tag, sizeof(tag));

    if (XSecure_AesEncryptUpdate(&(aes->xSec.cinst), XIL_CAST_U64(in_aligned),
                                 XIL_CAST_U64(out_aligned), sz, TRUE)) {
        WOLFSSL_XIL_MSG("Encrypt failed");
        ret = WC_HW_E;
        goto error_out;
    }

    ret = XSecure_AesEncryptFinal(&(aes->xSec.cinst), XIL_CAST_U64(tag));
    if (ret != XST_SUCCESS) {
        WOLFSSL_XIL_ERROR("EncryptFinal failed", ret);
        ret = WC_HW_E;
        ForceZero(authTag, authTagSz);
    } else {
        WOLFSSL_XIL_DCACHE_INVALIDATE_RANGE((UINTPTR)out_aligned, sz);
        WOLFSSL_XIL_DCACHE_INVALIDATE_RANGE((UINTPTR)tag, sizeof(tag));

        if (aes->aadStyle == SW_AAD) {
            ret = handle_aad(aes, out_aligned, sz, iv, authTag, authIn,
                             authInSz);
        } else {
            XMEMCPY(authTag, tag, authTagSz);
        }

        if (out != out_aligned) {
            XMEMCPY(out, out_aligned, sz);
        }
    }

error_out:
    aligned_xfree(out_buf, aes->heap);
    aligned_xfree(in_buf, aes->heap);
    ForceZero(tag, sizeof(tag));
    XSecure_AesKeyZero(&(aes->xSec.cinst), aes->kup);

    return ret;
}

int  wc_AesGcmDecrypt(       Aes* aes, byte* out,
                      const byte* in, word32 sz,
                      const byte* iv, word32 ivSz,
                      const byte* authTag, word32 authTagSz,
                      const byte* authIn, word32 authInSz)
{
    int ret = 0;
    ALIGN64 u8 buf[AES_GCM_AUTH_SZ];
    ALIGN64 u8 real_tag[AES_GCM_AUTH_SZ];
    const u8 *tag;
    byte *in_aligned;
    byte *in_buf = NULL;
    byte *out_aligned;
    byte *out_buf = NULL;

    if (aes == NULL || (in == NULL && sz > 0u) || iv == NULL || authTag == NULL
        || authTagSz != AES_GCM_AUTH_SZ) {
        return BAD_FUNC_ARG;
    }

    if (ivSz != AEAD_NONCE_SZ) {
        WOLFSSL_MSG("Expecting an IV size of 12");
        return BAD_FUNC_ARG;
    }

    if (in == NULL || out == NULL || sz == 0u) {
        ret = handle_aad(aes, NULL, 0u, iv, buf, authIn, authInSz);
        if (ret == 0 && ConstantCompare(authTag, buf, authTagSz) != 0) {
            ret = AES_GCM_AUTH_E;
        }
        return ret;
    }

    if (check_keysize(aes->keylen)) {
        return BAD_FUNC_ARG;
    }

    if (setup(aes, XSecure_AesDecryptInit, iv, authIn, authInSz)) {
        return WC_HW_E;
    }

    in_aligned = (byte*) in;
    if (NEEDS_ALIGNMENT(in, XIL_AESGCM_ALIGN)) {
        ret = aligned_xmalloc(&in_buf, &in_aligned, aes->heap, sz,
                              XIL_AESGCM_ALIGN);
        if (ret) {
            WOLFSSL_MSG(
                    "Failed to alloc memory for AESGCM Decrypt alignment (in)");
            return 1;
        }
        XMEMCPY(in_aligned, in, sz);
    }

    if (NEEDS_ALIGNMENT(out, XIL_AESGCM_ALIGN)) {
        if (in != in_aligned) {
            /* In case `in` has been copied already, reuse that buffer
             * and also write to it instead of allocating another one.
             */
            out_aligned = in_aligned;
        } else {
            ret = aligned_xmalloc(&out_buf, &out_aligned, aes->heap, sz,
                                  XIL_AESGCM_ALIGN);
            if (ret) {
                aligned_xfree(in_buf, aes->heap);
                WOLFSSL_MSG(
                        "Failed to alloc memory for AESGCM Decrypt alignment (out)");
                return 1;
            }
            XMEMCPY(out_aligned, out, sz);
        }
    } else {
        out_aligned = out;
    }

    WOLFSSL_XIL_DCACHE_INVALIDATE_RANGE((UINTPTR)in_aligned, sz);
    WOLFSSL_XIL_DCACHE_INVALIDATE_RANGE((UINTPTR)out_aligned, sz);

    if (aes->aadStyle == HW_ENGINE_AAD) {
        /* Use the originally provided tag */
        if (NEEDS_ALIGNMENT(authTag, XIL_AESGCM_ALIGN)) {
            XMEMCPY(real_tag, authTag, AES_GCM_AUTH_SZ);
            tag = real_tag;
        } else {
            tag = authTag;
        }
    } else {
        /* Create a fake tag that will match, so the HW-engine is OK when comparing the tags */
        ret = handle_aad(aes, in_aligned, sz, iv, buf, NULL, 0);
        if (ret)
            goto error_out;
        tag = buf;
        /* and create the real tag to compare against later */
        ret = handle_aad(aes, in_aligned, sz, iv, real_tag, authIn, authInSz);
        if (ret)
            goto error_out;
    }

    if (XSecure_AesDecryptUpdate(&(aes->xSec.cinst), XIL_CAST_U64(in_aligned),
                                 XIL_CAST_U64(out_aligned), sz, TRUE)) {
        WOLFSSL_XIL_MSG("Decrypt failed");
        ret = WC_HW_E;
        goto error_out;
    }

    WOLFSSL_XIL_DCACHE_INVALIDATE_RANGE((UINTPTR)tag, AES_GCM_AUTH_SZ);

    if (XSecure_AesDecryptFinal(&(aes->xSec.cinst), XIL_CAST_U64(tag))) {
        WOLFSSL_XIL_MSG("DecryptFinal failed");
        ret = WC_HW_E;
    } else {
        WOLFSSL_XIL_DCACHE_INVALIDATE_RANGE((UINTPTR)out_aligned, sz);
        WOLFSSL_XIL_DCACHE_INVALIDATE_RANGE((UINTPTR)buf, sizeof(buf));

        if (aes->aadStyle == SW_AAD) {
            if (ConstantCompare(authTag, real_tag, authTagSz) != 0) {
                ret = AES_GCM_AUTH_E;
            }
        }

        if (out != out_aligned) {
            XMEMCPY(out, out_aligned, sz);
        }
    }

error_out:
    aligned_xfree(out_buf, aes->heap);
    aligned_xfree(in_buf, aes->heap);
    ForceZero(real_tag, sizeof(real_tag));
    ForceZero(buf, sizeof(buf));
    XSecure_AesKeyZero(&(aes->xSec.cinst), aes->kup);

    return ret;
}

#else


int  wc_AesGcmSetKey_ex(Aes* aes, const byte* key, word32 len, word32 kup)
{
    XCsuDma_Config* con;

    if (aes == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }

    if (len != AES_256_KEY_SIZE) {
        WOLFSSL_MSG("Expecting a 256 bit key");
        return BAD_FUNC_ARG;
    }

    if ((con = XCsuDma_LookupConfig(0)) == NULL) {
        WOLFSSL_MSG("Failed to look up config");
        return MEMORY_E;
    }

    /* XST_SUCCESS comes from Xilinx header file */
    if (XCsuDma_CfgInitialize(&(aes->dma), con, con->BaseAddress) !=
            XST_SUCCESS) {
        WOLFSSL_MSG("Failed to initialize hardware");
        return MEMORY_E;
    }

    aes->keylen = len;
    aes->kup    = kup;
    XMEMCPY((byte*)(aes->keyInit), key, len);

    return 0;
}



int  wc_AesGcmEncrypt(Aes* aes, byte* out,
                                   const byte* in, word32 sz,
                                   const byte* iv, word32 ivSz,
                                   byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz)
{
    byte* tmp;
    byte scratch[AES_BLOCK_SIZE];
    byte initalCounter[AES_BLOCK_SIZE];
    int ret;

    if ((in == NULL && sz > 0) || iv == NULL || authTag == NULL ||
            authTagSz > AES_GCM_AUTH_SZ) {
        return BAD_FUNC_ARG;
    }

    if (ivSz != AEAD_NONCE_SZ) {
        WOLFSSL_MSG("Expecting an IV size of 12");
        return BAD_FUNC_ARG;
    }

    /* API expects that output is size of input + 16 byte tag. A temporary
     * buffer is created to keep AES encrypt from writing over the end of
     * out buffer. */
    if (in != NULL) {
        if (aes->keylen != AES_256_KEY_SIZE) {
            WOLFSSL_MSG("Expecting 256 bit AES key");
            return BAD_FUNC_ARG;
        }

        tmp = (byte*)XMALLOC(sz + AES_GCM_AUTH_SZ, aes->heap,
            DYNAMIC_TYPE_TMP_BUFFER);
        if (tmp == NULL) {
            return MEMORY_E;
        }

        XSecure_AesInitialize(&(aes->xilAes), &(aes->dma), aes->kup, (word32*)iv,
            aes->keyInit);
        XSecure_AesEncryptData(&(aes->xilAes), tmp, in, sz);
        XMEMCPY(out, tmp, sz);
        XMEMCPY(authTag, tmp + sz, authTagSz);
        XFREE(tmp, aes->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }

    /* handle completing tag with any additional data */
    if (authIn != NULL) {
        /* @TODO avoid hashing out again since Xilinx call already does */
        XMEMSET(initalCounter, 0, AES_BLOCK_SIZE);
        XMEMCPY(initalCounter, iv, ivSz);
        initalCounter[AES_BLOCK_SIZE - 1] = 1;
        GHASH(&aes->gcm, authIn, authInSz, out, sz, authTag, authTagSz);
        ret = wc_AesEncryptDirect(aes, scratch, initalCounter);
        if (ret < 0)
            return ret;
        xorbuf(authTag, scratch, authTagSz);
    }

    return 0;
}


int  wc_AesGcmDecrypt(Aes* aes, byte* out,
                                   const byte* in, word32 sz,
                                   const byte* iv, word32 ivSz,
                                   const byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz)
{
    byte* tag;
    byte buf[AES_GCM_AUTH_SZ];
    byte scratch[AES_BLOCK_SIZE];
    byte initalCounter[AES_BLOCK_SIZE];
    int ret;

    if (in == NULL || iv == NULL || authTag == NULL ||
            authTagSz < AES_GCM_AUTH_SZ) {
        return BAD_FUNC_ARG;
    }

    if (ivSz != AEAD_NONCE_SZ) {
        WOLFSSL_MSG("Expecting an IV size of 12");
        return BAD_FUNC_ARG;
    }

    /* account for additional data */
    if (authIn != NULL && authInSz > 0) {
        XMEMSET(initalCounter, 0, AES_BLOCK_SIZE);
        XMEMCPY(initalCounter, iv, ivSz);
        initalCounter[AES_BLOCK_SIZE - 1] = 1;
        tag = buf;
        GHASH(&aes->gcm, NULL, 0, in, sz, tag, AES_GCM_AUTH_SZ);
        ret = wc_AesEncryptDirect(aes, scratch, initalCounter);
        if (ret < 0)
            return ret;
        xorbuf(tag, scratch, AES_GCM_AUTH_SZ);
    }
    else {
        tag = authTag;
    }

    /* calls to hardened crypto */
    XSecure_AesInitialize(&(aes->xilAes), &(aes->dma), aes->kup,
                (word32*)iv, aes->keyInit);
    XSecure_AesDecryptData(&(aes->xilAes), out, in, sz, tag);

    /* account for additional data */
    if (authIn != NULL && authInSz > 0) {
        GHASH(&aes->gcm, authIn, authInSz, in, sz, tag, AES_GCM_AUTH_SZ);
        ret = wc_AesEncryptDirect(aes, scratch, initalCounter);
        if (ret < 0)
            return ret;
        xorbuf(tag, scratch, AES_GCM_AUTH_SZ);
        if (ConstantCompare(authTag, tag, authTagSz) != 0) {
            return AES_GCM_AUTH_E;
        }
    }

    return 0;

}
#endif /* versal */
#endif /* HAVE_AESGCM */

#endif
