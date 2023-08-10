/* renesas_tsip_sha.c
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
#include <string.h>
#include <stdio.h>

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/internal.h>
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif
#if !defined(NO_SHA) || !defined(NO_SHA256)

#include <wolfssl/wolfcrypt/logging.h>

#if (defined(WOLFSSL_RENESAS_TSIP_TLS) || \
     defined(WOLFSSL_RENESAS_TSIP_CRYPTONLY))

#include <wolfssl/wolfcrypt/memory.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/port/Renesas/renesas-tsip-crypt.h>

extern struct WOLFSSL_HEAP_HINT* tsip_heap_hint;

#ifdef WOLFSSL_RENESAS_TSIP_TLS
/*  get hmac from handshake messages exchanged with server.
 *
 */
WOLFSSL_LOCAL int tsip_Tls13GetHmacMessages(struct WOLFSSL* ssl, byte* mac)
{
    int ret     = 0;
    int isTLS13 = 0;
    TsipUserCtx* tuc = NULL;
    e_tsip_err_t err = TSIP_SUCCESS;
    byte hash[WC_SHA256_DIGEST_SIZE];
    int     hmacSz = 0;

    WOLFSSL_ENTER("tsip_Tls13GetHmacMessages");

    if (ssl == NULL)
        ret = BAD_FUNC_ARG;

    if (ret == 0) {
        if (ssl->version.major == SSLv3_MAJOR && 
            ssl->version.minor == TLSv1_3_MINOR)
            isTLS13 = 1;

        /* TSIP works only in TLS13 client side */
        if (!isTLS13 || ssl->options.side != WOLFSSL_CLIENT_END) {
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    /* get user context for TSIP */
    if (ret == 0) {
        tuc = ssl->RenesasUserCtx;
        if (tuc == NULL) {
            ret = CRYPTOCB_UNAVAILABLE;
        }
        else if (!tuc->HandshakeClientTrafficKey_set) {
            WOLFSSL_MSG("Client handshake traffic keys aren't created by TSIP");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    /* get transcript hash */
    if (ret == 0) {
        ForceZero(hash, sizeof(hash));
        ret = tsip_GetMessageSha256(ssl, hash, (int*)&hmacSz);
    }

    if (ret == 0) {
        if ((ret = tsip_hw_lock()) == 0) {

            err = R_TSIP_Sha256HmacGenerateInit(&(tuc->hmacFinished13Handle),
                                                &(tuc->clientFinished13Idx));

            if (err != TSIP_SUCCESS) {
                WOLFSSL_MSG("R_TSIP_Sha256HmacGenerateInit failed");
                ret = WC_HW_E;
            }

            if (ret == 0) {

                err = R_TSIP_Sha256HmacGenerateUpdate(
                                                &(tuc->hmacFinished13Handle),
                                                (uint8_t*)hash,
                                                WC_SHA256_DIGEST_SIZE);

                if (err != TSIP_SUCCESS) {
                    WOLFSSL_MSG("R_TSIP_Sha256HmacGenerateUpdate failed");
                    ret = WC_HW_E;
                }
            }

            if (ret == 0) {
                err = R_TSIP_Sha256HmacGenerateFinal(
                                            &(tuc->hmacFinished13Handle), mac);
                if (err != TSIP_SUCCESS) {
                    WOLFSSL_MSG("R_TSIP_Sha256HmacGenerateFinal failed");
                    ret = WC_HW_E;
                }
            }
            tsip_hw_unlock();
        }
        else {
            WOLFSSL_MSG("mutex locking error");
        }
    }
    WOLFSSL_LEAVE("tsipTls13GetHmacMessages", ret);
    return ret;
}



/* store handshake message for later hash or hmac operation. 
 * 
 */
WOLFSSL_LOCAL int tsip_StoreMessage(struct WOLFSSL* ssl, const byte* data,
                                                                int sz)
{
    int     ret = 0;
    int     isTLS13 = 0;
    word32  messageSz;
    MsgBag* bag = NULL;
    TsipUserCtx* tuc = NULL;

    WOLFSSL_ENTER("tsip_StoreMessage");

    if (ssl == NULL)
        ret = BAD_FUNC_ARG;

    if (ret == 0) {
        if (ssl->version.major == SSLv3_MAJOR && 
            ssl->version.minor == TLSv1_3_MINOR)
            isTLS13 = 1;

        /* TSIP works only in TLS13 client side */
        if (!isTLS13 || ssl->options.side != WOLFSSL_CLIENT_END) {
            WOLFSSL_MSG("Not in tls1.3 or not in client end");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }
    /* should work until handshake is done */ 
    if (ret == 0) {
        if (ssl->options.handShakeDone) {
            WOLFSSL_MSG("handshake is done.");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    /* get user context for TSIP */
    if (ret == 0) {
        tuc = ssl->RenesasUserCtx;
        if (tuc == NULL) {
            WOLFSSL_MSG("RenesasUserCtx is not set in ssl.");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    /* check if TSIP is used for this session */
    if (ret == 0) {
        if (!tuc->Dhe_key_set) {
            WOLFSSL_MSG("DH key not set.");
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }

    /* copy raw handshake message into MsgBag for later sha256 operations. */
    if (ret == 0) {
        c24to32(&data[1], &messageSz);

        bag = &(tuc->messageBag);

        if (bag->msgIdx +1 > MAX_MSGBAG_MESSAGES || 
            bag->buffIdx + sz > MSGBAG_SIZE) {
            WOLFSSL_MSG("Capacity over error in tsip_StoreMessage");
            ret = MEMORY_E;
        }
    
        XMEMCPY(bag->buff + bag->buffIdx, data, sz);
        bag->msgTypes[bag->msgIdx++] = *data;   /* store message type */
        bag->buffIdx += sz;
    }

    WOLFSSL_LEAVE("tsip_StoreMessage", ret);
    return ret;
}



WOLFSSL_LOCAL int tsip_GetMessageSha256(struct WOLFSSL* ssl, byte* hash,
                                                                int* sz)
{
    int     ret = 0;
    int     isTLS13 = 0;
    MsgBag* bag = NULL;
    TsipUserCtx* tuc = NULL;
    tsip_sha_md5_handle_t handle;
    e_tsip_err_t err = TSIP_SUCCESS;
    uint32_t hashSz = 0;

    WOLFSSL_ENTER("tsip_GetMessageSha256");

    if (ssl == NULL)
        ret = BAD_FUNC_ARG;

    if (ret == 0) {
        if (ssl->version.major == SSLv3_MAJOR && 
            ssl->version.minor == TLSv1_3_MINOR)
            isTLS13 = 1;

        /* TSIP works only in TLS13 client side */
        if (!isTLS13 || ssl->options.side != WOLFSSL_CLIENT_END) {
            ret = CRYPTOCB_UNAVAILABLE;
        }
    }
    /* get user context for TSIP */
    if (ret == 0) {
        tuc = ssl->RenesasUserCtx;
        if (tuc == NULL) {
            ret = CRYPTOCB_UNAVAILABLE;
        }
        bag = &(tuc->messageBag);
    }
    
    if (ret == 0) {
        if ((ret = tsip_hw_lock()) == 0) {

            err = R_TSIP_Sha256Init(&handle);

            if (err == TSIP_SUCCESS) {
                err = R_TSIP_Sha256Update(&handle, (uint8_t*)bag->buff, 
                                                                bag->buffIdx);
            }
            if (err == TSIP_SUCCESS) {
                err = R_TSIP_Sha256Final(&handle, hash, &hashSz);
            }
            if (err == TSIP_SUCCESS) {
                if (sz != NULL) {
                    *sz = hashSz;
                }
            }
            else {
                ret = WC_HW_E;
            }
            tsip_hw_unlock();
        }
        else {
            WOLFSSL_MSG("mutex locking error");
        }
    }
    WOLFSSL_LEAVE("tsip_GetMessageSha256", ret);
    return ret;
}
#endif /* WOLFSSL_RENESAS_TSIP_TLS */





static void TSIPHashFree(wolfssl_TSIP_Hash* hash)
{
    if (hash == NULL)
        return;

    if (hash->msg != NULL) {
        XFREE(hash->msg, hash->heap, DYNAMIC_TYPE_TMP_BUFFER);
        hash->msg = NULL;
    }
}

static int TSIPHashInit(wolfssl_TSIP_Hash* hash, void* heap, int devId,
    word32 sha_type)
{
    if (hash == NULL) {
        return BAD_FUNC_ARG;
    }

    (void)devId;
    ForceZero(hash, sizeof(wolfssl_TSIP_Hash));

    if (heap == NULL && tsip_heap_hint != NULL) {
        hash->heap = (struct wolfSSL_HEAP_HINT*)tsip_heap_hint;
    }
    else {
        hash->heap = heap;
    }
    
    hash->len  = 0;
    hash->used = 0;
    hash->msg  = NULL;
    hash->sha_type = sha_type;

    return 0;
}

static int TSIPHashUpdate(wolfssl_TSIP_Hash* hash, const byte* data, word32 sz)
{
    if (hash == NULL || (sz > 0 && data == NULL)) {
        return BAD_FUNC_ARG;
    }

    if (hash->len < hash->used + sz) {
        if (hash->msg == NULL) {
            hash->msg = (byte*)XMALLOC(hash->used + sz, hash->heap,
                    DYNAMIC_TYPE_TMP_BUFFER);
        }
        else {
#ifdef FREERTOS
            byte* pt = (byte*)XMALLOC(hash->used + sz, hash->heap,
                    DYNAMIC_TYPE_TMP_BUFFER);
            if (pt == NULL) {
                return MEMORY_E;
            }
            XMEMCPY(pt, hash->msg, hash->used);
            XFREE(hash->msg, hash->heap, DYNAMIC_TYPE_TMP_BUFFER);
            hash->msg = NULL;
            hash->msg = pt;
#else
            byte* pt = (byte*)XREALLOC(hash->msg, hash->used + sz, hash->heap,
                    DYNAMIC_TYPE_TMP_BUFFER);
            if (pt == NULL) {
                return MEMORY_E;
            }
            hash->msg = pt;
#endif
        }
        if (hash->msg == NULL) {
            return MEMORY_E;
        }
        hash->len = hash->used + sz;
    }
    XMEMCPY(hash->msg + hash->used, data , sz);
    hash->used += sz;

    return 0;
}

static int TSIPHashFinal(wolfssl_TSIP_Hash* hash, byte* out, word32 outSz)
{
    int ret;
    void* heap;
    tsip_sha_md5_handle_t handle;
    uint32_t sz;

    e_tsip_err_t (*Init)(tsip_sha_md5_handle_t*);
    e_tsip_err_t (*Update)(tsip_sha_md5_handle_t*, uint8_t*, uint32_t);
    e_tsip_err_t (*Final )(tsip_sha_md5_handle_t*, uint8_t*, uint32_t*);

    if (hash == NULL || out == NULL) {
        return BAD_FUNC_ARG;
    }

    if (hash->sha_type == TSIP_SHA1) {
        Init = R_TSIP_Sha1Init;
        Update = R_TSIP_Sha1Update;
        Final = R_TSIP_Sha1Final;
    }
    else if (hash->sha_type == TSIP_SHA256) {
        Init = R_TSIP_Sha256Init;
        Update = R_TSIP_Sha256Update;
        Final = R_TSIP_Sha256Final;
    }
    else
        return BAD_FUNC_ARG;

    heap = hash->heap;

    tsip_hw_lock();

    if (Init(&handle) == TSIP_SUCCESS) {
        ret = Update(&handle, (uint8_t*)hash->msg, hash->used);
        if (ret == TSIP_SUCCESS) {
            ret = Final(&handle, out, (uint32_t*)&sz);
            if (ret != TSIP_SUCCESS || sz != outSz) {
                return ret;
            }
        }
    }
    tsip_hw_unlock();

    TSIPHashFree(hash);
    return TSIPHashInit(hash, heap, 0, hash->sha_type);
}

static int TSIPHashGet(wolfssl_TSIP_Hash* hash, byte* out, word32 outSz)
{
    int ret;
    tsip_sha_md5_handle_t handle;
    uint32_t sz;

    e_tsip_err_t (*Init)(tsip_sha_md5_handle_t*);
    e_tsip_err_t (*Update)(tsip_sha_md5_handle_t*, uint8_t*, uint32_t);
    e_tsip_err_t (*Final )(tsip_sha_md5_handle_t*, uint8_t*, uint32_t*);

    if (hash == NULL || out == NULL) {
        return BAD_FUNC_ARG;
    }

    if (hash->sha_type == TSIP_SHA1) {
        Init = R_TSIP_Sha1Init;
        Update = R_TSIP_Sha1Update;
        Final = R_TSIP_Sha1Final;
    }
    else if (hash->sha_type == TSIP_SHA256) {
        Init = R_TSIP_Sha256Init;
        Update = R_TSIP_Sha256Update;
        Final = R_TSIP_Sha256Final;
    }
    else
        return BAD_FUNC_ARG;

    tsip_hw_lock();

    if (Init(&handle) == TSIP_SUCCESS) {
        ret = Update(&handle, (uint8_t*)hash->msg, hash->used);
        if (ret == TSIP_SUCCESS) {
            ret = Final(&handle, out, &sz);
            if (ret != TSIP_SUCCESS || sz != outSz) {
                return ret;
            }
        }
    }

    tsip_hw_unlock();

    return 0;
}

static int TSIPHashCopy(wolfssl_TSIP_Hash* src, wolfssl_TSIP_Hash* dst)
{
    if (src == NULL || dst == NULL) {
        return BAD_FUNC_ARG;
    }

    XMEMCPY(dst, src, sizeof(wolfssl_TSIP_Hash));

    if (src->len > 0 && src->msg != NULL) {
        dst->msg = (byte*)XMALLOC(src->len, dst->heap, DYNAMIC_TYPE_TMP_BUFFER);
        if (dst->msg == NULL) {
            return MEMORY_E;
        }
        XMEMCPY(dst->msg, src->msg, src->len);
    }

    return 0;
}
#if !defined(NO_SHA) && !defined(NO_WOLFSSL_RENESAS_TSIP_CRYPT_HASH)
#include <wolfssl/wolfcrypt/sha.h>

int wc_InitSha_ex(wc_Sha* sha, void* heap, int devId)
{
    return TSIPHashInit(sha, heap, devId, TSIP_SHA1);
}

int wc_ShaUpdate(wc_Sha* sha, const byte* in, word32 sz)
{
    return TSIPHashUpdate(sha, in, sz);
}

int wc_ShaFinal(wc_Sha* sha, byte* hash)
{
    return TSIPHashFinal(sha, hash, WC_SHA_DIGEST_SIZE);
}

int wc_ShaGetHash(wc_Sha* sha, byte* hash)
{
    return TSIPHashGet(sha, hash, WC_SHA_DIGEST_SIZE);
}

int wc_ShaCopy(wc_Sha256* src, wc_Sha256* dst)
{
    return TSIPHashCopy(src, dst);
}
#endif /* !NO_SHA && !NO_WOLFSSL_RENESAS_TSIP_CRYPT_HASH*/

#if !defined(NO_SHA256) && !defined(NO_WOLFSSL_RENESAS_TSIP_CRYPT_HASH)
#include <wolfssl/wolfcrypt/sha256.h>

/*  */
int wc_InitSha256_ex(wc_Sha256* sha, void* heap, int devId)
{
    return TSIPHashInit(sha, heap, devId, TSIP_SHA256);
}

int wc_Sha256Update(wc_Sha256* sha, const byte* in, word32 sz)
{
    return TSIPHashUpdate(sha, in, sz);
}

int wc_Sha256Final(wc_Sha256* sha, byte* hash)
{
    return TSIPHashFinal(sha, hash, WC_SHA256_DIGEST_SIZE);
}

int wc_Sha256GetHash(wc_Sha256* sha, byte* hash)
{
    return TSIPHashGet(sha, hash, WC_SHA256_DIGEST_SIZE);
}

int wc_Sha256Copy(wc_Sha256* src, wc_Sha256* dst)
{
    return TSIPHashCopy(src, dst);
}
#endif /* !NO_SHA256 */
#endif /* WOLFSSL_RENESAS_TSIP_TLS || WOLFSSL_RENESAS_TSIP_CRYPTONLY */
#endif /* #if !defined(NO_SHA) || !defined(NO_SHA256) */
