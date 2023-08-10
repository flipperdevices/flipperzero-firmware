/* dcp_port.c
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

#ifdef WOLFSSL_IMXRT_DCP
#include <wolfssl/wolfcrypt/aes.h>
#include <wolfssl/wolfcrypt/sha.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U) && defined(DCP_USE_DCACHE) && (DCP_USE_DCACHE == 1U)
#error "DCACHE not supported by this driver. Please undefine DCP_USE_DCACHE."
#endif

#ifndef DCP_USE_OTP_KEY
#define DCP_USE_OTP_KEY 0 /* Set to 1 to select OTP key for AES encryption/decryption. */
#endif

#include "fsl_dcp.h"

#ifndef SINGLE_THREADED
#define dcp_lock_init() wolfSSL_CryptHwMutexInit()
#define dcp_lock() wolfSSL_CryptHwMutexLock()
#define dcp_unlock() wolfSSL_CryptHwMutexUnLock()
#else
#define dcp_lock_init() WC_DO_NOTHING
#define dcp_lock()      WC_DO_NOTHING
#define dcp_unlock()    WC_DO_NOTHING
#endif

#if DCP_USE_OTP_KEY
typedef enum _dcp_otp_key_select
{
    kDCP_OTPMKKeyLow  = 1U, /* Use [127:0] from snvs key as dcp key */
    kDCP_OTPMKKeyHigh = 2U, /* Use [255:128] from snvs key as dcp key */
    kDCP_OCOTPKeyLow  = 3U, /* Use [127:0] from ocotp key as dcp key */
    kDCP_OCOTPKeyHigh = 4U  /* Use [255:128] from ocotp key as dcp key */
} dcp_otp_key_select;
#endif

#if DCP_USE_OTP_KEY
static status_t DCP_OTPKeySelect(dcp_otp_key_select keySelect)
{
    status_t retval = kStatus_Success;
    if (keySelect == kDCP_OTPMKKeyLow)
    {
        IOMUXC_GPR->GPR3 &= ~(1 << IOMUXC_GPR_GPR3_DCP_KEY_SEL_SHIFT);
        IOMUXC_GPR->GPR10 &= ~(1 << IOMUXC_GPR_GPR10_DCPKEY_OCOTP_OR_KEYMUX_SHIFT);
    }

    else if (keySelect == kDCP_OTPMKKeyHigh)
    {
        IOMUXC_GPR->GPR3 |= (1 << IOMUXC_GPR_GPR3_DCP_KEY_SEL_SHIFT);
        IOMUXC_GPR->GPR10 &= ~(1 << IOMUXC_GPR_GPR10_DCPKEY_OCOTP_OR_KEYMUX_SHIFT);
    }

    else if (keySelect == kDCP_OCOTPKeyLow)
    {
        IOMUXC_GPR->GPR3 &= ~(1 << IOMUXC_GPR_GPR3_DCP_KEY_SEL_SHIFT);
        IOMUXC_GPR->GPR10 |= (1 << IOMUXC_GPR_GPR10_DCPKEY_OCOTP_OR_KEYMUX_SHIFT);
    }

    else if (keySelect == kDCP_OCOTPKeyHigh)
    {
        IOMUXC_GPR->GPR3 |= (1 << IOMUXC_GPR_GPR3_DCP_KEY_SEL_SHIFT);
        IOMUXC_GPR->GPR10 |= (1 << IOMUXC_GPR_GPR10_DCPKEY_OCOTP_OR_KEYMUX_SHIFT);
    }

    else
    {
        retval = kStatus_InvalidArgument;
    }
    return retval;
}
#endif

static const int dcp_channels[4] = {
    kDCP_Channel0,
    kDCP_Channel1,
    kDCP_Channel2,
    kDCP_Channel3
};

#ifndef SINGLE_THREADED
static int dcp_status[4] = {0, 0, 0, 0};
#endif

static int dcp_get_channel(void)
{
#ifdef SINGLE_THREADED
    return dcp_channels[0];
#else
    int i;
    int ret = 0;
    dcp_lock();
    for (i = 0; i < 4; i++) {
        if (dcp_status[i] == 0) {
            dcp_status[i]++;
            ret = dcp_channels[i];
            break;
        }
    }
    dcp_unlock();
    return ret;
#endif
}

static int dcp_key_slot(int ch)
{
    int ret = -1;

#if DCP_USE_OTP_KEY
    return kDCP_OtpKey;
#endif

#ifndef SINGLE_THREADED
    int i;
    dcp_lock();
    for (i = 0; i < 4; i++) {
        if (ch == dcp_channels[i]) {
            ret = i;
            break;
        }
    }
    dcp_unlock();
#else
    ret = 0;
#endif
    return ret;
}


int wc_dcp_init(void)
{
    dcp_config_t dcpConfig;
    dcp_lock_init();
    dcp_lock();
    DCP_GetDefaultConfig(&dcpConfig);

    /* Reset and initialize DCP */
    DCP_Init(DCP, &dcpConfig);
#if DCP_USE_OTP_KEY
    /* Set OTP key type in IOMUX registers before initializing DCP. */
    /* Software reset of DCP must be issued after changing the OTP key type. */
    DCP_OTPKeySelect(kDCP_OTPMKKeyLow);
#endif
    /* Release mutex */
    dcp_unlock();
    return 0;
}

static void dcp_free(int ch)
{
#ifndef SINGLE_THREADED
    int i;
    dcp_lock();
    for (i = 0; i < 4; i++) {
        if (ch == dcp_channels[i]) {
            dcp_status[i] = 0;
            break;
        }
    }
    dcp_unlock();
#endif
}


#ifndef NO_AES
int DCPAesInit(Aes *aes)
{
    int ch;
    if (!aes)
        return BAD_FUNC_ARG;
    ch = dcp_get_channel();
    if (ch == 0)
        return WC_PENDING_E;
    XMEMSET(&aes->handle, 0, sizeof(aes->handle));
    aes->handle.channel = (dcp_channel_t)ch;
    aes->handle.keySlot = (dcp_key_slot_t)dcp_key_slot(aes->handle.channel);
    aes->handle.swapConfig = kDCP_NoSwap;
    return 0;
}

void DCPAesFree(Aes *aes)
{
    dcp_free(aes->handle.channel);
    aes->handle.channel = 0;
}


static unsigned char  aes_key_aligned[16] __attribute__((aligned(0x10)));
int  DCPAesSetKey(Aes* aes, const byte* key, word32 len, const byte* iv,
                          int dir)
{

#if DCP_USE_OTP_KEY
#warning Please update cipherAes128 variables to match expected AES ciphertext for your OTP key.
#endif
    status_t status;
    if (!aes || !key)
        return BAD_FUNC_ARG;

    if (len != 16)
        return BAD_FUNC_ARG;
    if (aes->handle.channel == 0) {
        if (DCPAesInit(aes) != 0)
            return WC_HW_E;
    }
    dcp_lock();
    memcpy(aes_key_aligned, key, 16);
    status = DCP_AES_SetKey(DCP, &aes->handle, aes_key_aligned, 16);
    if (status != kStatus_Success)
        status = WC_HW_E;
    else {
        if (iv)
            XMEMCPY(aes->reg, iv, 16);
        else
            XMEMSET(aes->reg, 0, 16);
    }
    dcp_unlock();
    return status;
}

int  DCPAesCbcEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    int ret;
    if (sz % 16)
        return BAD_FUNC_ARG;
    dcp_lock();
    ret = DCP_AES_EncryptCbc(DCP, &aes->handle, in, out, sz, (const byte *)aes->reg);
    if (ret)
        ret = WC_HW_E;
    else
        XMEMCPY(aes->reg, out, AES_BLOCK_SIZE);
    dcp_unlock();
    return ret;
}

int  DCPAesCbcDecrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    int ret;
    if (sz % 16)
        return BAD_FUNC_ARG;
    dcp_lock();
    ret = DCP_AES_DecryptCbc(DCP, &aes->handle, in, out, sz, (const byte *)aes->reg);
    if (ret)
        ret = WC_HW_E;
    else
        XMEMCPY(aes->reg, in, AES_BLOCK_SIZE);
    dcp_unlock();
    return ret;
}

int  DCPAesEcbEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    int ret;
    if (sz % 16)
        return BAD_FUNC_ARG;
    dcp_lock();
    ret = DCP_AES_EncryptEcb(DCP, &aes->handle, in, out, sz);
    if (ret)
        ret = WC_HW_E;
    dcp_unlock();
    return ret;
}

int  DCPAesEcbDecrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    int ret;
    if (sz % 16)
        return BAD_FUNC_ARG;
    dcp_lock();
    ret = DCP_AES_DecryptEcb(DCP, &aes->handle, in, out, sz);
    if (ret)
        ret = WC_HW_E;
    dcp_unlock();
    return ret;
}

#endif

#ifndef NO_SHA256
int wc_InitSha256_ex(wc_Sha256* sha256, void* heap, int devId)
{
    int ret;
    int ch;
    int keyslot;
    if (sha256 == NULL)
        return BAD_FUNC_ARG;
    ch = dcp_get_channel();
    if (ch == 0)
        return WC_PENDING_E;
    keyslot = dcp_key_slot(ch);

    dcp_lock();
    (void)devId;
    XMEMSET(sha256, 0, sizeof(wc_Sha256));
    sha256->handle.channel    = (dcp_channel_t)ch;
    sha256->handle.keySlot    = (dcp_key_slot_t)keyslot;
    sha256->handle.swapConfig = kDCP_NoSwap;
    ret = DCP_HASH_Init(DCP, &sha256->handle, &sha256->ctx, kDCP_Sha256);
    if (ret != kStatus_Success)
        ret = WC_HW_E;
    dcp_unlock();

    return ret;
}

void DCPSha256Free(wc_Sha256* sha256)
{
    if (sha256)
        dcp_free(sha256->handle.channel);
}

int wc_Sha256Update(wc_Sha256* sha256, const byte* data, word32 len)
{
    int ret;
    if (sha256 == NULL || (data == NULL && len != 0)) {
        return BAD_FUNC_ARG;
    }
    dcp_lock();
    ret = DCP_HASH_Update(DCP, &sha256->ctx, data, len);
    if (ret != kStatus_Success)
        ret = WC_HW_E;
    dcp_unlock();
    return ret;
}

int wc_Sha256GetHash(wc_Sha256* sha256, byte* hash)
{
    int ret;
    size_t outlen = WC_SHA256_DIGEST_SIZE;
    dcp_hash_ctx_t saved_ctx;
    if (sha256 == NULL || hash == NULL)
        return BAD_FUNC_ARG;
    dcp_lock();
    XMEMCPY(&saved_ctx, &sha256->ctx, sizeof(dcp_hash_ctx_t));
    XMEMSET(hash, 0, WC_SHA256_DIGEST_SIZE);
    ret = DCP_HASH_Finish(DCP, &sha256->ctx, hash, &outlen);
    if ((ret != kStatus_Success) || (outlen != SHA256_DIGEST_SIZE))
        ret = WC_HW_E;
    else
        XMEMCPY(&sha256->ctx, &saved_ctx, sizeof(dcp_hash_ctx_t));
    dcp_unlock();
    return 0;
}

int wc_Sha256Final(wc_Sha256* sha256, byte* hash)
{
    int ret;
    size_t outlen = WC_SHA256_DIGEST_SIZE;
    dcp_lock();
    ret = DCP_HASH_Finish(DCP, &sha256->ctx, hash, &outlen);
    if ((ret != kStatus_Success) || (outlen != SHA256_DIGEST_SIZE))
        ret = WC_HW_E;
    else {
        ret = DCP_HASH_Init(DCP, &sha256->handle, &sha256->ctx, kDCP_Sha256);
        if (ret < 0)
            ret = WC_HW_E;
    }
    dcp_unlock();
    return ret;
}

#ifdef WOLFSSL_HASH_FLAGS
int wc_Sha256SetFlags(wc_Sha256* sha256, word32 flags)
{
    if (sha256) {
        sha256->flags = flags;
    }
    return 0;
}
int wc_Sha256GetFlags(wc_Sha256* sha256, word32* flags)
{
    if (sha256 && flags) {
        *flags = sha256->flags;
    }
    return 0;
}
#endif /* WOLFSSL_HASH_FLAGS */

int wc_Sha256Copy(wc_Sha256* src, wc_Sha256* dst)
{
    if (src == NULL || dst == NULL)
        return BAD_FUNC_ARG;
    dcp_lock();
    XMEMCPY(&dst->ctx, &src->ctx, sizeof(dcp_hash_ctx_t));
    dcp_unlock();
    return 0;
}
#endif /* !NO_SHA256 */


#ifndef NO_SHA

int wc_InitSha_ex(wc_Sha* sha, void* heap, int devId)
{
    int ret;
    int ch;
    int keyslot;
    if (sha == NULL)
        return BAD_FUNC_ARG;
    ch = dcp_get_channel();
    if (ch == 0)
        return WC_PENDING_E;
    keyslot = dcp_key_slot(ch);
    dcp_lock();
    (void)devId;
    XMEMSET(sha, 0, sizeof(wc_Sha));
    sha->handle.channel    = (dcp_channel_t)ch;
    sha->handle.keySlot    = (dcp_key_slot_t)keyslot;
    sha->handle.swapConfig = kDCP_NoSwap;
    ret = DCP_HASH_Init(DCP, &sha->handle, &sha->ctx, kDCP_Sha1);
    if (ret != kStatus_Success)
        ret = WC_HW_E;
    dcp_unlock();
    return ret;
}

void DCPShaFree(wc_Sha* sha)
{
    if (sha)
        dcp_free(sha->handle.channel);
}

int wc_ShaUpdate(wc_Sha* sha, const byte* data, word32 len)
{
    int ret;
    if (sha == NULL || (data == NULL && len != 0)) {
        return BAD_FUNC_ARG;
    }
    dcp_lock();
    ret = DCP_HASH_Update(DCP, &sha->ctx, data, len);
    if (ret != kStatus_Success)
        ret = WC_HW_E;
    dcp_unlock();
    return ret;
}


int wc_ShaGetHash(wc_Sha* sha, byte* hash)
{
    int ret;
    size_t outlen = WC_SHA_DIGEST_SIZE;
    dcp_hash_ctx_t saved_ctx;
    if (sha == NULL || hash == NULL)
        return BAD_FUNC_ARG;
    dcp_lock();
    XMEMCPY(&saved_ctx, &sha->ctx, sizeof(dcp_hash_ctx_t));
    XMEMSET(hash, 0, WC_SHA_DIGEST_SIZE);
    ret = DCP_HASH_Finish(DCP, &sha->ctx, hash, &outlen);
    if ((ret != kStatus_Success) || (outlen != WC_SHA_DIGEST_SIZE))
        ret = WC_HW_E;
    else
        XMEMCPY(&sha->ctx, &saved_ctx, sizeof(dcp_hash_ctx_t));
    dcp_unlock();
    return 0;
}

int wc_ShaFinal(wc_Sha* sha, byte* hash)
{
    int ret;
    size_t outlen = WC_SHA_DIGEST_SIZE;
    dcp_lock();
    ret = DCP_HASH_Finish(DCP, &sha->ctx, hash, &outlen);
    if ((ret != kStatus_Success) || (outlen != SHA_DIGEST_SIZE)) {
        ret = WC_HW_E;
    } else {
        ret = DCP_HASH_Init(DCP, &sha->handle, &sha->ctx, kDCP_Sha1);
        if (ret < 0)
            ret = WC_HW_E;
    }
    dcp_unlock();
    return ret;
}

#ifdef WOLFSSL_HASH_FLAGS
int wc_ShaSetFlags(wc_Sha* sha, word32 flags)
{
    if (sha) {
        sha->flags = flags;
    }
    return 0;
}
int wc_ShaGetFlags(wc_Sha* sha, word32* flags)
{
    if (sha && flags) {
        *flags = sha->flags;
    }
    return 0;
}
#endif /* WOLFSSL_HASH_FLAGS */

int wc_ShaCopy(wc_Sha* src, wc_Sha* dst)
{
    if (src == NULL || dst == NULL)
        return BAD_FUNC_ARG;
    dcp_lock();
    XMEMCPY(&dst->ctx, &src->ctx, sizeof(dcp_hash_ctx_t));
    dcp_unlock();
    return 0;
}
#endif /* !NO_SHA */

#endif /* WOLFSSL_IMXRT_DCP */
