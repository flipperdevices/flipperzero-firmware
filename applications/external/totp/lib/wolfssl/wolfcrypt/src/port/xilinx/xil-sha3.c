/* xil-sha3.c
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


#if defined(WOLFSSL_SHA3) && defined(WOLFSSL_XILINX_CRYPT)

#include <wolfssl/wolfcrypt/sha3.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>

#if defined(WOLFSSL_XILINX_CRYPT_VERSAL)

#include <xsecure_shaclient.h>

/* Initialize hardware for SHA3 operations
 *
 * sha   SHA3 structure to initialize
 * heap  memory heap hint to use
 * devId used for async operations (currently not supported here)
 */
int wc_InitSha3_384(wc_Sha3* sha, void* heap, int devId)
{
    (void) heap;
    (void) devId;

    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }

    if (wc_InitXsecure(&(sha->xSec))) {
        WOLFSSL_MSG("Can't initialize Xsecure");
        return BAD_STATE_E;
    }

    XSecure_Sha3Initialize();

    return 0;
}


/* Update SHA3 state
 *
 * sha   SHA3 structure to update
 * data  message to update SHA3 state with
 * len   length of data buffer
 */
int wc_Sha3_384_Update(wc_Sha3* sha, const byte* data, word32 len)
{
    if (sha == NULL ||  (data == NULL && len > 0)) {
        return BAD_FUNC_ARG;
    }
    WOLFSSL_XIL_DCACHE_INVALIDATE_RANGE((UINTPTR)data, len);
    XSecure_Sha3Update(&(sha->xSec.cinst), XIL_CAST_U64(data), len);

    return 0;
}


/* Finalize SHA3 state and get digest
 *
 * sha  SHA3 structure to get hash
 * out  digest out, expected to be large enough to hold SHA3 digest
 */
int wc_Sha3_384_Final(wc_Sha3* sha, byte* out)
{
    if (sha == NULL || out == NULL) {
        return BAD_FUNC_ARG;
    }
    WOLFSSL_XIL_DCACHE_INVALIDATE_RANGE((UINTPTR)out, WC_SHA3_384_DIGEST_SIZE);
    XSecure_Sha3Finish(&(sha->xSec.cinst), XIL_CAST_U64(out));

    return wc_InitSha3_384(sha, NULL, INVALID_DEVID);
}


/* Free SHA3 structure
 *
 * sha  SHA3 structure to free
 */
void wc_Sha3_384_Free(wc_Sha3* sha)
{
    (void)sha;
    /* nothing to free yet */
}

#else /* non-versal */

#if !defined(WOLFSSL_NOSHA3_224) || !defined(WOLFSSL_NOSHA3_256) \
    || !defined(WOLFSSL_NOSHA3_512)
    #error sizes of SHA3 other than 384 are not supported
#endif

/* Initialize hardware for SHA3 operations
 *
 * sha   SHA3 structure to initialize
 * heap  memory heap hint to use
 * devId used for async operations (currently not supported here)
 */
int wc_InitSha3_384(wc_Sha3* sha, void* heap, int devId)
{
    XCsuDma_Config* con;

    (void)heap;
    (void)devId;

    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }

    if ((con = XCsuDma_LookupConfig(0)) == NULL) {
        WOLFSSL_MSG("Unable to look up configure for SHA3");
        return BAD_STATE_E;
    }

    /* XST_SUCCESS is success macro from Xilinx header */
    if (XCsuDma_CfgInitialize(&(sha->dma), con, con->BaseAddress) !=
            XST_SUCCESS) {
        WOLFSSL_MSG("Unable to initialize CsuDma");
        return BAD_STATE_E;
    }

    XSecure_Sha3Initialize(&(sha->hw), &(sha->dma));
    XSecure_Sha3Start(&(sha->hw));

    return 0;
}


/* Update SHA3 state
 *
 * sha   SHA3 structure to update
 * data  message to update SHA3 state with
 * len   length of data buffer
 */
int wc_Sha3_384_Update(wc_Sha3* sha, const byte* data, word32 len)
{
    if (sha == NULL ||  (data == NULL && len > 0)) {
        return BAD_FUNC_ARG;
    }
    XSecure_Sha3Update(&(sha->hw), (byte*)data, len);

    return 0;
}


/* Finalize SHA3 state and get digest
 *
 * sha  SHA3 structure to get hash
 * out  digest out, expected to be large enough to hold SHA3 digest
 */
int wc_Sha3_384_Final(wc_Sha3* sha, byte* out)
{
    if (sha == NULL || out == NULL) {
        return BAD_FUNC_ARG;
    }
    XSecure_Sha3Finish(&(sha->hw), out);

    return wc_InitSha3_384(sha, NULL, INVALID_DEVID);
}


/* Free SHA3 structure
 *
 * sha  SHA3 structure to free
 */
void wc_Sha3_384_Free(wc_Sha3* sha)
{
    (void)sha;
    /* nothing to free yet */
}


/* Get SHA3 digest without finalize SHA3 state
 *
 * sha  SHA3 structure to get hash
 * out  digest out, expected to be large enough to hold SHA3 digest
 */
int wc_Sha3_384_GetHash(wc_Sha3* sha, byte* out)
{
#ifdef WOLFSSL_XILINX_CRYPTO_OLD
    wc_Sha3 s;
#endif

    if (sha == NULL || out == NULL) {
        return BAD_FUNC_ARG;
    }
#ifdef WOLFSSL_XILINX_CRYPTO_OLD
        if (wc_Sha3_384_Copy(sha, &s) != 0) {
            WOLFSSL_MSG("Unable to copy SHA3 structure");
            return MEMORY_E;
        }

        return wc_Sha3_384_Final(&s, out);
#else
    XSecure_Sha3_ReadHash(&(sha->hw), out);
    return 0;
#endif
}


/* Get copy of SHA3 structure
 *
 * src SHA3 structure to make copy of
 * dst [out]structure to hold copy
 */
int wc_Sha3_384_Copy(wc_Sha3* src, wc_Sha3* dst)
{
    if (src == NULL || dst== NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_XILINX_CRYPTO_OLD
    XMEMCPY((byte*)dst, (byte*)src, sizeof(wc_Sha3));
    return 0;
#else
    WOLFSSL_MSG("Copy of SHA3 struct not supported with this build");
    return -1;
#endif
}
#endif

#endif
