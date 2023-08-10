/* cryptoCell.h
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

#ifndef WOLFSSL_CRYPTOCELL_H
#define WOLFSSL_CRYPTOCELL_H

#if defined(WOLFSSL_CRYPTOCELL)
#ifdef __cplusplus
extern "C" {
#endif

#include <wolfssl/wolfcrypt/types.h>

#include "sns_silib.h"

#ifndef NO_SHA256
    #include "crys_hash.h"
    #include "crys_hash_error.h"
    /* largest possible input data on CC310 DMA. */
    #define CC310_MAX_LENGTH_DMA        (0xFFFF)
#endif

#ifndef NO_AES
    #include "ssi_aes.h"

    typedef struct aes_context_t {
        SaSiAesUserContext_t user_ctx;    /* CC310 AES User context */
        SaSiAesUserKeyData_t key;         /* CC310 AES key structure */
        SaSiAesEncryptMode_t mode;        /* encrypt or decrypt */
    } aes_context_t;
    #define CC310_MAX_LENGTH_DMA_AES    (0xFFF0)
#endif /* NO_AES */

#if !defined(WC_NO_RNG)
    #if defined(WOLFSSL_nRF5x_SDK_15_2)

/* To fix warning. MIN/MAX are defined in tfm.h and Nordic (re)defines them */
        #undef MIN
        #undef MAX
/* includes to use RNG on the nRF52 */
        #include "nrf_drv_rng.h"
        #include "nrf_assert.h"
    #endif

    /*RNG Global variables*/
    extern CRYS_RND_State_t     wc_rndState;
    extern CRYS_RND_WorkBuff_t  wc_rndWorkBuff;
    extern SaSiRndGenerateVectWorkFunc_t wc_rndGenVectFunc;
    int    cc310_random_generate(byte* output, word32 size);
#endif

#ifndef NO_RSA
    #include "crys_rsa_types.h"
    #include "crys_rnd.h"
    #include "crys_rsa_schemes.h"
    #include "crys_rsa_kg.h"
    #include "crys_rsa_build.h"

    typedef struct rsa_context_t {
        CRYS_RSAUserPrivKey_t privKey;
        CRYS_RSAUserPubKey_t  pubKey;
    } rsa_context_t;
CRYS_RSA_HASH_OpMode_t cc310_hashModeRSA(enum wc_HashType hash_type, int isHashed);
#endif

#ifdef HAVE_ECC
    #include "crys_ecpki_kg.h"
    #include "crys_ecpki_dh.h"
    #include "crys_ecpki_build.h"
    #include "crys_ecpki_domain.h"
    #include "crys_ecpki_ecdsa.h"

    typedef struct ecc_context_t {
        CRYS_ECPKI_UserPrivKey_t privKey;
        CRYS_ECPKI_UserPublKey_t pubKey;
    } ecc_context_t;

CRYS_ECPKI_DomainID_t cc310_mapCurve(int curve_id);
CRYS_ECPKI_HASH_OpMode_t cc310_hashModeECC(int hash_size);
#endif /* HAVE_ECC */

#if !defined(NO_CRYPT_BENCHMARK) && defined(WOLFSSL_nRF5x_SDK_15_2)
    /* includes to use Real-time counter (RTC) on nRF52840 */
    #include "nrf_gpio.h"
    #include "nrf_drv_rtc.h"
    #include "nrf_drv_clock.h"
    #include "boards.h"
    #include <stdint.h>
    #include <stdbool.h>
#endif /* NO_CRYPT_BENCHMARK && WOLFSSL_nRF5x_SDK_15_2*/

int  cc310_Init(void);
void cc310_Free(void);

#ifdef __cplusplus
}
#endif
#endif /* WOLFSSL_CRYPTOCELL */
#endif /* WOLFSSL_CRYPTOCELL_H */
