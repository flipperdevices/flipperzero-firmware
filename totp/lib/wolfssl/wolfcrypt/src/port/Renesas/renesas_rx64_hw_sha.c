/* renesas_rx64_hw_sha.c
 *
 * Contributed by Johnson Controls Tyco IP Holdings LLP.
 *
 * Use of this Software is subject to the GPLv2 License
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
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

#if !defined(NO_SHA) || !defined(NO_SHA256)

#include <wolfssl/wolfcrypt/logging.h>

#if defined(WOLFSSL_RENESAS_RX64_HASH)

#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/port/Renesas/renesas-rx64-hw-crypt.h>

#include <wolfssl/wolfcrypt/sha.h>

typedef union
{
    R_sha1   sha1;
    R_sha224 sha224;
    R_sha256 sha256;
} R_Sha_Data;

/**
Default SHA Hash Data When Input Msg Buffers are NULL.

The source of this data can be obtained from a simple python
program that requests the hash of an empty input argument.
Example:
import hashlib
print("SHA default/empty hash values")
print(f"SHA1   {hashlib.sha1(b'').hexdigest()}")
print(f"SHA224 {hashlib.sha224(b'').hexdigest()}")
print(f"SHA256 {hashlib.sha256(b'').hexdigest()}")

OR

The following website also provide data for these hashes when
an empty buffer is given as input
https://www.di-mgt.com.au/sha_testvectors.html
**/

static byte const DefaultShaHashData[] =
{
  0xDA, 0x39, 0xA3, 0xEE, 0x5E, 0x6B, 0x4B, 0x0D,
  0x32, 0x55, 0xBF, 0xEF, 0x95, 0x60, 0x18, 0x90,
  0xAF, 0xD8, 0x07, 0x09
};

static byte const DefaultSha224HashData[] =
{
  0xD1, 0x4A, 0x02, 0x8C, 0x2A, 0x3A, 0x2B, 0xC9,
  0x47, 0x61, 0x02, 0xBB, 0x28, 0x82, 0x34, 0xC4,
  0x15, 0xA2, 0xB0, 0x1F, 0x82, 0x8E, 0xA6, 0x2A,
  0xC5, 0xB3, 0xE4, 0x2F
};

static byte const DefaultSha256HashData[] =
{
  0xE3, 0xB0, 0xC4, 0x42, 0x98, 0xFC, 0x1C, 0x14,
  0x9A, 0xFB, 0xF4, 0xC8, 0x99, 0x6F, 0xB9, 0x24,
  0x27, 0xAE, 0x41, 0xE4, 0x64, 0x9B, 0x93, 0x4C,
  0xA4, 0x95, 0x99, 0x1B, 0x78, 0x52, 0xB8, 0x55
};

/**
 * @brief Calculate a SHA hash using the RX64 SHA subsystem.
 *
 * @param[in] data buffer with data to sha
 * @param[in] len length of data
 * @param[out] out Output buffer to store sha result in
 * @param[in] sha_type Sha type to calculate, from RX64_SHA_TYPE
 * @return int R_PROCESS_COMPLETE (0) on success, see r_sha.h for failure codes.
 */
int RX64_ShaCalc(byte* data, word32 len, byte* out, word32 sha_type)
{
    int ret;
    uint8_t flag = R_SHA_INIT;
    word32 index = 0;
    uint16_t chunk_length;
    R_Sha_Data work_sha;

    if (data == NULL || len == 0 ||
        out == NULL || sha_type >= NUM_RX64_SHA_TYPES)
    {
        return BAD_FUNC_ARG;
    }

    XMEMSET(&work_sha, 0, sizeof(work_sha));

    rx64_hw_lock();
    do {
        /*
        The hardware functions can only accept UINT16_MAX bytes at a time.
        To work around this break the buffer up into chunks and pass the
        R_SHA_FINISH flag with the last chunk.
        */
        if (len - index <= UINT16_MAX) {
            flag = flag | R_SHA_FINISH;
            chunk_length = len - index;
        } else {
            chunk_length = UINT16_MAX;
        }
        /* Based on the hash type call the correct hardware function. */
        if (sha_type == RX64_SHA1) {
            ret = R_Sha1_HashDigest(&data[index], out, chunk_length, flag,
                                    &work_sha.sha1);
        } else if (sha_type == RX64_SHA224) {
            ret = R_Sha224_HashDigest(&data[index], out, chunk_length, flag,
                                      &work_sha.sha224);
        } else if (sha_type == RX64_SHA256) {
            ret = R_Sha256_HashDigest(&data[index], out, chunk_length, flag,
                                      &work_sha.sha256);
        }
        if (ret != R_PROCESS_COMPLETE) {
            /* On failure break, unlock hardware, return error. */
            break;
        }
        index += chunk_length;
        flag = R_SHA_ADD;
    } while (index < len);

    rx64_hw_unlock();
    return ret;
}

/**
 * @brief Free a hash for use with the RX64 SHA subsystem.
 *
 * @param[in] hash The hash to free
 */
static void RX64_HashFree(wolfssl_RX64_HW_Hash* hash)
{
    if (hash == NULL)
        return;

    if (hash->msg != NULL) {
        XFREE(hash->msg, hash->heap, DYNAMIC_TYPE_TMP_BUFFER);
        hash->msg = NULL;
    }
}

/**
 * @brief Initialize a hash for use with the RX64 SHA subsystem.
 *
 * @param[in] hash The hash to initialize
 * @param[in] heap Optional pointer to memory to use.
 * @param devId Unused
 * @param[in] sha_type The SHA type for this hash
 * @return int 0 on success, BAD_FUNC_ARG on failure
 */
static int RX64_HashInit(wolfssl_RX64_HW_Hash* hash, void* heap, int devId,
    word32 sha_type)
{
    if (hash == NULL) {
        return BAD_FUNC_ARG;
    }

    (void)devId;
    XMEMSET(hash, 0, sizeof(wolfssl_RX64_HW_Hash));

    hash->heap = heap;
    hash->len  = 0;
    hash->used = 0;
    hash->msg  = NULL;
    hash->sha_type = sha_type;

    return 0;
}

/**
 * @brief Add data to the hash with the RX64 SHA subsystem.
 *
 * Note that do to the limitations in the RX64 hardware
 * and it's inability to save the current state,
 * this function actually just adds the data to a buffer
 * that will then be processed when calling HashFinal or HashGet
 *
 * @param[in] hash Hash structure
 * @param[in] data data to hash
 * @param[in] sz size of the data
 * @return int 0 on success, BAD_FUNC_ARG or MEMORY_E on failure
 */
static int RX64_HashUpdate(wolfssl_RX64_HW_Hash* hash,
                           const byte* data, word32 sz)
{
    if (hash == NULL || (sz > 0 && data == NULL)) {
        return BAD_FUNC_ARG;
    }

    if (hash->len < hash->used + sz) {
        if (hash->msg == NULL) {
            hash->msg = (byte*)XMALLOC(hash->used + sz, hash->heap,
                    DYNAMIC_TYPE_TMP_BUFFER);
        } else {
            byte* pt = (byte*)XREALLOC(hash->msg, hash->used + sz, hash->heap,
                    DYNAMIC_TYPE_TMP_BUFFER);
            if (pt == NULL) {
                return MEMORY_E;
            }
            hash->msg = pt;
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

/**
 * @brief Calculate hash value with the RX64 SHA subsystem and reset the hash.
 *
 * @param[in] hash Structure containing the information on what to hash
 * @param[out] out Sha hash
 * @retval int R_PROCESS_COMPLETE (0) on success.
 * @retval int BAD_FUNC_ARG or see r_sha.h on failure.
 */
static int RX64_HashFinal(wolfssl_RX64_HW_Hash* hash, byte* out)
{
    int ret = R_PROCESS_COMPLETE;
    void* heap;

    if (hash == NULL || out == NULL) {
        return BAD_FUNC_ARG;
    }
    if (hash->sha_type != RX64_SHA1 &&
        hash->sha_type != RX64_SHA224 &&
        hash->sha_type != RX64_SHA256)
    {
        return BAD_FUNC_ARG;
    }

    heap = hash->heap;

    /*
    RX64 HW SHA operations considers empty msgs to be an error,
    though some wolfSSL operations expects to have successful SHA operations on
    empty incoming buffers (e.g DeriveHandshakeSecret()).
    Thus we must support the expected default SHA hash data for this operation
    since TLS decrypt operations expect a specific SW Hash to be used when the
    input buffer was empty/NULL.
    */
    if ((hash->msg == NULL) && (hash->len == 0) && (hash->used == 0))
    {
        if (hash->sha_type == RX64_SHA1)
        {
            XMEMCPY(out, DefaultShaHashData, sizeof(DefaultShaHashData));
        }
        else if (hash->sha_type == RX64_SHA224)
        {
            XMEMCPY(out, DefaultSha224HashData, sizeof(DefaultSha224HashData));
        }
        else if (hash->sha_type == RX64_SHA256)
        {
            XMEMCPY(out, DefaultSha256HashData, sizeof(DefaultSha256HashData));
        }
    }
    else
    {
        /* Utilize RX64 SHA HW Acceleration for normal SHA operations. */
        ret = RX64_ShaCalc(hash->msg, hash->len, out, hash->sha_type);
        if (ret != R_PROCESS_COMPLETE)
        {
            return ret;
        }
    }

    RX64_HashFree(hash);
    return RX64_HashInit(hash, heap, 0, hash->sha_type);
}

/**
 * @brief Calculate hash value with the RX64 SHA subsystem.
 *
 * @param[in] hash Structure containing the information on what to hash
 * @param[out] out Sha hash
 * @retval int R_PROCESS_COMPLETE (0) on success.
 * @retval int BAD_FUNC_ARG or see r_sha.h on failure.
 */
static int RX64_HashGet(wolfssl_RX64_HW_Hash* hash, byte* out)
{
    int ret;

    if (hash == NULL || out == NULL) {
        return BAD_FUNC_ARG;
    }
    if (hash->sha_type != RX64_SHA1 &&
        hash->sha_type != RX64_SHA224 &&
        hash->sha_type != RX64_SHA256)
    {
        return BAD_FUNC_ARG;
    }

    ret = RX64_ShaCalc(hash->msg, hash->len, out, hash->sha_type);
    if (ret != R_PROCESS_COMPLETE) {
        return ret;
    }

    return 0;
}

/**
 * @brief Copy a hash for use with the RX64 SHA subsystem.
 *
 * @param[in] src Source hash structure
 * @param[out] dst Destination hash structure
 * @return int 0 on success, BAD_FUNC_ARG on failure
 */
static int RX64_HashCopy(wolfssl_RX64_HW_Hash* src, wolfssl_RX64_HW_Hash* dst)
{
    if (src == NULL || dst == NULL) {
        return BAD_FUNC_ARG;
    }

    XMEMCPY(dst, src, sizeof(wolfssl_RX64_HW_Hash));

    if (src->len > 0 && src->msg != NULL) {
        dst->msg = (byte*)XMALLOC(src->len, dst->heap, DYNAMIC_TYPE_TMP_BUFFER);
        if (dst->msg == NULL) {
            return MEMORY_E;
        }
        XMEMCPY(dst->msg, src->msg, src->len);
    }

    return 0;
}

/* WolfCrypt wrapper function for RX64 SHA1 Init */
int wc_InitSha_ex(wc_Sha* sha, void* heap, int devId)
{
    return RX64_HashInit((wolfssl_RX64_HW_Hash*)sha, heap, devId, RX64_SHA1);
}
/* WolfCrypt wrapper function for RX64 SHA1 Update */
int wc_ShaUpdate(wc_Sha* sha, const byte* in, word32 sz)
{
    return RX64_HashUpdate((wolfssl_RX64_HW_Hash*)sha, in, sz);
}
/* WolfCrypt wrapper function for RX64 SHA1 Final */
int wc_ShaFinal(wc_Sha* sha, byte* hash)
{
    return RX64_HashFinal((wolfssl_RX64_HW_Hash*)sha, hash);
}
/* WolfCrypt wrapper function for RX64 SHA1 Get */
int wc_ShaGetHash(wc_Sha* sha, byte* hash)
{
    return RX64_HashGet((wolfssl_RX64_HW_Hash*)sha, hash);
}
/* WolfCrypt wrapper function for RX64 SHA1 Copy */
int wc_ShaCopy(wc_Sha* src, wc_Sha* dst)
{
    return RX64_HashCopy((wolfssl_RX64_HW_Hash*)src, (wolfssl_RX64_HW_Hash*)dst);
}

#if defined(WOLFSSL_SHA224)
#include <wolfssl/wolfcrypt/sha256.h>

/* WolfCrypt wrapper function for RX64 SHA224 Init */
int wc_InitSha224_ex(wc_Sha224* sha, void* heap, int devId)
{
    return RX64_HashInit((wolfssl_RX64_HW_Hash*)sha, heap, devId, RX64_SHA224);
}
/* WolfCrypt wrapper function for RX64 SHA224 Update */
int wc_Sha224Update(wc_Sha224* sha, const byte* in, word32 sz)
{
    return RX64_HashUpdate((wolfssl_RX64_HW_Hash*)sha, in, sz);
}
/* WolfCrypt wrapper function for RX64 SHA224 Final */
int wc_Sha224Final(wc_Sha224* sha, byte* hash)
{
    return RX64_HashFinal((wolfssl_RX64_HW_Hash*)sha, hash);
}
/* WolfCrypt wrapper function for RX64 SHA224 Get */
int wc_Sha224GetHash(wc_Sha224* sha, byte* hash)
{
    return RX64_HashGet((wolfssl_RX64_HW_Hash*)sha, hash);
}
/* WolfCrypt wrapper function for RX64 SHA224 Copy */
int wc_Sha224Copy(wc_Sha224* src, wc_Sha224* dst)
{
    return RX64_HashCopy((wolfssl_RX64_HW_Hash*)src, (wolfssl_RX64_HW_Hash*)dst);
}
#endif /* WOLFSSL_SHA224 */

#if !defined(NO_SHA256)
#include <wolfssl/wolfcrypt/sha256.h>

/* WolfCrypt wrapper function for RX64 SHA256 Init */
int wc_InitSha256_ex(wc_Sha256* sha, void* heap, int devId)
{
    return RX64_HashInit((wolfssl_RX64_HW_Hash*)sha, heap, devId, RX64_SHA256);
}
/* WolfCrypt wrapper function for RX64 SHA256 Update */
int wc_Sha256Update(wc_Sha256* sha, const byte* in, word32 sz)
{
    return RX64_HashUpdate((wolfssl_RX64_HW_Hash*)sha, in, sz);
}
/* WolfCrypt wrapper function for RX64 SHA256 Final */
int wc_Sha256Final(wc_Sha256* sha, byte* hash)
{
    return RX64_HashFinal((wolfssl_RX64_HW_Hash*)sha, hash);
}
/* WolfCrypt wrapper function for RX64 SHA256 Get */
int wc_Sha256GetHash(wc_Sha256* sha, byte* hash)
{
    return RX64_HashGet((wolfssl_RX64_HW_Hash*)sha, hash);
}
/* WolfCrypt wrapper function for RX64 SHA256 Copy */
int wc_Sha256Copy(wc_Sha256* src, wc_Sha256* dst)
{
    return RX64_HashCopy((wolfssl_RX64_HW_Hash*)src, (wolfssl_RX64_HW_Hash*)dst);
}
#endif /* !NO_SHA256 */
#endif /* WOLFSSL_RENESAS_RX64_HASH */
#endif /* #if !defined(NO_SHA) || !defined(NO_SHA256) */
