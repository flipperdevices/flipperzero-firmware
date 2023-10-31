/* fips_test.h
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



#ifndef WOLF_CRYPT_FIPS_TEST_H
#define WOLF_CRYPT_FIPS_TEST_H

#include <wolfssl/wolfcrypt/types.h>


#ifdef __cplusplus
    extern "C" {
#endif

/* Added for FIPS v5.3 or later */
#if defined(FIPS_VERSION_GE) && FIPS_VERSION_GE(5,3)
    /* Determine FIPS in core hash type and size */
    #ifndef NO_SHA256
        #define FIPS_IN_CORE_DIGEST_SIZE 32
        #define FIPS_IN_CORE_HASH_TYPE   WC_SHA256
        #define FIPS_IN_CORE_KEY_SZ      32
        #define FIPS_IN_CORE_VERIFY_SZ   FIPS_IN_CORE_KEY_SZ
    #elif defined(WOLFSSL_SHA384)
        #define FIPS_IN_CORE_DIGEST_SIZE 48
        #define FIPS_IN_CORE_HASH_TYPE   WC_SHA384
        #define FIPS_IN_CORE_KEY_SZ      48
        #define FIPS_IN_CORE_VERIFY_SZ   FIPS_IN_CORE_KEY_SZ
    #else
        #error No FIPS hash (SHA2-256 or SHA2-384)
    #endif
#endif /* FIPS v5.3 or later */


enum FipsCastId {
    FIPS_CAST_AES_CBC,
    FIPS_CAST_AES_GCM,
    FIPS_CAST_HMAC_SHA1,
    FIPS_CAST_HMAC_SHA2_256,
    FIPS_CAST_HMAC_SHA2_512,
    FIPS_CAST_HMAC_SHA3_256,
    FIPS_CAST_DRBG,
    FIPS_CAST_RSA_SIGN_PKCS1v15,
    FIPS_CAST_ECC_CDH,
    FIPS_CAST_ECC_PRIMITIVE_Z,
    FIPS_CAST_DH_PRIMITIVE_Z,
    FIPS_CAST_ECDSA,
    FIPS_CAST_KDF_TLS12,
    FIPS_CAST_KDF_TLS13,
    FIPS_CAST_KDF_SSH,
    FIPS_CAST_COUNT
};

enum FipsCastStateId {
    FIPS_CAST_STATE_INIT,
    FIPS_CAST_STATE_PROCESSING,
    FIPS_CAST_STATE_SUCCESS,
    FIPS_CAST_STATE_FAILURE
};

enum FipsModeId {
    FIPS_MODE_INIT,
    FIPS_MODE_NORMAL,
    FIPS_MODE_DEGRADED,
    FIPS_MODE_FAILED
};


/* FIPS failure callback */
typedef void(*wolfCrypt_fips_cb)(int ok, int err, const char* hash);

/* Public set function */
WOLFSSL_API int wolfCrypt_SetCb_fips(wolfCrypt_fips_cb cbf);

/* Public get status functions */
WOLFSSL_API int wolfCrypt_GetStatus_fips(void);
WOLFSSL_API const char* wolfCrypt_GetCoreHash_fips(void);

#ifdef HAVE_FORCE_FIPS_FAILURE
    /* Public function to force failure mode for operational testing */
    WOLFSSL_API int wolfCrypt_SetStatus_fips(int status);
#endif

WOLFSSL_LOCAL int DoPOST(char* base16_hash, int base16_hashSz);
WOLFSSL_LOCAL int DoCAST(int type);
WOLFSSL_LOCAL int DoKnownAnswerTests(char* base16_hash, int base16_hashSz); /* FIPSv1 and FIPSv2 */

WOLFSSL_API int wc_RunCast_fips(int type);
WOLFSSL_API int wc_GetCastStatus_fips(int type);

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLF_CRYPT_FIPS_TEST_H */

